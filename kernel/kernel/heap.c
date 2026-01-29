#include <kernel/heap.h>
#include <arch/memory/paging.h>
#include <stdint.h>

//code from https://github.com/blanham/liballoc

/**  Durand's Ridiculously Amazing Super Duper Memory functions.  */

#define ALIGNMENT 16ul

#define ALIGN_TYPE char
#define ALIGN_INFO	sizeof(ALIGN_TYPE)*16

#define USE_CASE1
#define USE_CASE2
#define USE_CASE3
#define USE_CASE4
#define USE_CASE5

#define ALIGN( ptr )													\
		if ( ALIGNMENT > 1 )											\
		{																\
			uintptr_t diff;												\
			ptr = (void*)((uintptr_t)ptr + ALIGN_INFO);					\
			diff = (uintptr_t)ptr & (ALIGNMENT-1);						\
			if ( diff != 0 )											\
			{															\
				diff = ALIGNMENT - diff;								\
				ptr = (void*)((uintptr_t)ptr + diff);					\
			}															\
			*((ALIGN_TYPE*)((uintptr_t)ptr - ALIGN_INFO)) = 			\
				diff + ALIGN_INFO;										\
		}															


#define UNALIGN( ptr )													\
		if ( ALIGNMENT > 1 )											\
		{																\
			uintptr_t diff = *((ALIGN_TYPE*)((uintptr_t)ptr - ALIGN_INFO));	\
			if ( diff < (ALIGNMENT + ALIGN_INFO) )						\
			{															\
				ptr = (void*)((uintptr_t)ptr - diff);					\
			}															\
		}
				


#define LIBALLOC_MAGIC	0xc001c0de
#define LIBALLOC_DEAD	0xdeaddead

struct liballoc_major
{
	struct liballoc_major *prev;
	struct liballoc_major *next;
	unsigned int pages;
	unsigned int size;
	unsigned int usage;
	struct liballoc_minor *first;
};

struct	liballoc_minor
{
	struct liballoc_minor *prev;
	struct liballoc_minor *next;
	struct liballoc_major *block;
	unsigned int magic;
	unsigned int size;
	unsigned int req_size;
};


static struct liballoc_major *l_memRoot = NULL;
static struct liballoc_major *l_bestBet = NULL;

static unsigned int l_pageSize  = 4096;
static unsigned int l_pageCount = 1;
static unsigned long long l_allocated = 0;
static unsigned long long l_inuse	 = 0;


static long long l_warningCount = 0;
static long long l_errorCount = 0;
static long long l_possibleOverruns = 0;

/** This function is supposed to lock the memory data structures. It
 * could be as simple as disabling interrupts or acquiring a spinlock.
 * It's up to you to decide. 
 *
 * \return 0 if the lock was acquired successfully. Anything else is
 * failure.
 */
#define liballoc_lock() ((void)0)

/** This function unlocks what was previously locked by the liballoc_lock
 * function.  If it disabled interrupts, it enables interrupts. If it
 * had acquiried a spinlock, it releases the spinlock. etc.
 *
 * \return 0 if the lock was successfully released.
 */
#define liballoc_unlock() ((void)0)

void* liballoc_alloc(int pages) {
	if(pages < 0) return NULL;
	uint32_t addr = map_pages((uint32_t)pages, PG_KERNEL | PG_READWRITE);
	if(addr == 0)
		return NULL;
	return (void*)addr;
}

int liballoc_free(void* addr, int pages) {
	if(addr == NULL || pages < 0) return -1;
	unmap_pages((uint32_t)addr, pages);
	return 0;
}

static void *liballoc_memset(void* s, int c, size_t n)
{
	unsigned int i;
	for ( i = 0; i < n ; i++)
		((char*)s)[i] = c;
	
	return s;
}
static void* liballoc_memcpy(void* s1, const void* s2, size_t n)
{
  char *cdest;
  char *csrc;
  unsigned int *ldest = (unsigned int*)s1;
  unsigned int *lsrc  = (unsigned int*)s2;

  while ( n >= sizeof(unsigned int) )
  {
      *ldest++ = *lsrc++;
	  n -= sizeof(unsigned int);
  }

  cdest = (char*)ldest;
  csrc  = (char*)lsrc;
  
  while ( n > 0 )
  {
      *cdest++ = *csrc++;
	  n -= 1;
  }
  
  return s1;
}

static struct liballoc_major *allocate_new_page( unsigned int size )
{
	unsigned int st;
	struct liballoc_major *maj;

		st  = size + sizeof(struct liballoc_major);
		st += sizeof(struct liballoc_minor);

		if ( (st % l_pageSize) == 0 )
			st  = st / (l_pageSize);
		else
			st  = st / (l_pageSize) + 1;

		
		//if ( st < l_pageCount ) st = l_pageCount;
		
		maj = (struct liballoc_major*)liballoc_alloc( st );

		if ( maj == NULL ) 
		{
			l_warningCount += 1;
			return NULL;
		}
		
		maj->prev 	= NULL;
		maj->next 	= NULL;
		maj->pages 	= st;
		maj->size 	= st * l_pageSize;
		maj->usage 	= sizeof(struct liballoc_major);
		maj->first 	= NULL;

		l_allocated += maj->size;
		
      return maj;
}

void* malloc(size_t req_size) {
	int startedBet = 0;
	unsigned long long bestSize = 0;
	void *p = NULL;
	uintptr_t diff;
	struct liballoc_major *maj;
	struct liballoc_minor *min;
	struct liballoc_minor *new_min;
	unsigned long size = req_size;

	if ( ALIGNMENT > 1 )
	{
		size += ALIGNMENT + ALIGN_INFO;
	}
	
	liballoc_lock();

	if ( size == 0 )
	{
		l_warningCount += 1;
		liballoc_unlock();
		return malloc(1);
	}
	

	if ( l_memRoot == NULL )
	{
			
		l_memRoot = allocate_new_page( size );
		if ( l_memRoot == NULL )
		{
		  liballoc_unlock();
		}
	}


	maj = l_memRoot;
	startedBet = 0;
	
	if ( l_bestBet != NULL )
	{
		bestSize = l_bestBet->size - l_bestBet->usage;

		if ( bestSize > (size + sizeof(struct liballoc_minor)))
		{
			maj = l_bestBet;
			startedBet = 1;
		}
	}
	
	while ( maj != NULL )
	{
		diff  = maj->size - maj->usage;	

		if ( bestSize < diff )
		{
			l_bestBet = maj;
			bestSize = diff;
		}
		
		
#ifdef USE_CASE1
			
		// CASE 1:  There is not enough space in this major block.
		if ( diff < (size + sizeof( struct liballoc_minor )) )
		{

			if ( maj->next != NULL ) 
			{
				maj = maj->next;
				continue;
			}

			if ( startedBet == 1 )
			{
				maj = l_memRoot;
				startedBet = 0;
				continue;
			}

			maj->next = allocate_new_page( size );
			if ( maj->next == NULL ) break;
			maj->next->prev = maj;
			maj = maj->next;

		}

#endif

#ifdef USE_CASE2
		
		// CASE 2: It's a brand new block.
		if ( maj->first == NULL )
		{
			maj->first = (struct liballoc_minor*)((uintptr_t)maj + sizeof(struct liballoc_major) );

			
			maj->first->magic 		= LIBALLOC_MAGIC;
			maj->first->prev 		= NULL;
			maj->first->next 		= NULL;
			maj->first->block 		= maj;
			maj->first->size 		= size;
			maj->first->req_size 	= req_size;
			maj->usage 	+= size + sizeof( struct liballoc_minor );


			l_inuse += size;
			
			
			p = (void*)((uintptr_t)(maj->first) + sizeof( struct liballoc_minor ));

			ALIGN( p );
			
			liballoc_unlock();
			return p;
		}

#endif
				
#ifdef USE_CASE3

		// CASE 3: Block in use and enough space at the start of the block.
		diff =  (uintptr_t)(maj->first);
		diff -= (uintptr_t)maj;
		diff -= sizeof(struct liballoc_major);

		if ( diff >= (size + sizeof(struct liballoc_minor)) )
		{
			maj->first->prev = (struct liballoc_minor*)((uintptr_t)maj + sizeof(struct liballoc_major) );
			maj->first->prev->next = maj->first;
			maj->first = maj->first->prev;
				
			maj->first->magic 	= LIBALLOC_MAGIC;
			maj->first->prev 	= NULL;
			maj->first->block 	= maj;
			maj->first->size 	= size;
			maj->first->req_size 	= req_size;
			maj->usage 			+= size + sizeof( struct liballoc_minor );

			l_inuse += size;

			p = (void*)((uintptr_t)(maj->first) + sizeof( struct liballoc_minor ));
			ALIGN( p );

			liballoc_unlock();
			return p;
		}
		
#endif


#ifdef USE_CASE4

		// CASE 4: There is enough space in this block. But is it contiguous?
		min = maj->first;
		
		while ( min != NULL )
		{
				// CASE 4.1: End of minors in a block. Space from last and end?
				if ( min->next == NULL )
				{
					diff = (uintptr_t)(maj) + maj->size;
					diff -= (uintptr_t)min;
					diff -= sizeof( struct liballoc_minor );
					diff -= min->size; 

					if ( diff >= (size + sizeof( struct liballoc_minor )) )
					{
						min->next = (struct liballoc_minor*)((uintptr_t)min + sizeof( struct liballoc_minor ) + min->size);
						min->next->prev = min;
						min = min->next;
						min->next = NULL;
						min->magic = LIBALLOC_MAGIC;
						min->block = maj;
						min->size = size;
						min->req_size = req_size;
						maj->usage += size + sizeof( struct liballoc_minor );

						l_inuse += size;
						
						p = (void*)((uintptr_t)min + sizeof( struct liballoc_minor ));
						ALIGN( p );

						liballoc_unlock();
						return p;
					}
				}



				// CASE 4.2: Is there space between two minors?
				if ( min->next != NULL )
				{
					diff  = (uintptr_t)(min->next);
					diff -= (uintptr_t)min;
					diff -= sizeof( struct liballoc_minor );
					diff -= min->size;

					if ( diff >= (size + sizeof( struct liballoc_minor )) )
					{
						new_min = (struct liballoc_minor*)((uintptr_t)min + sizeof( struct liballoc_minor ) + min->size);

						new_min->magic = LIBALLOC_MAGIC;
						new_min->next = min->next;
						new_min->prev = min;
						new_min->size = size;
						new_min->req_size = req_size;
						new_min->block = maj;
						min->next->prev = new_min;
						min->next = new_min;
						maj->usage += size + sizeof( struct liballoc_minor );
						
						l_inuse += size;
						
						p = (void*)((uintptr_t)new_min + sizeof( struct liballoc_minor ));
						ALIGN( p );
						
						liballoc_unlock();
						return p;
					}
				}	// min->next != NULL

				min = min->next;
		} // while min != NULL ...


#endif

#ifdef USE_CASE5

		// CASE 5: Block full! Ensure next block and loop.
		if ( maj->next == NULL ) 
		{

			if ( startedBet == 1 )
			{
				maj = l_memRoot;
				startedBet = 0;
				continue;
			}
				
			maj->next = allocate_new_page( size );
			if ( maj->next == NULL ) break;
			maj->next->prev = maj;

		}

#endif

		maj = maj->next;
	} // while (maj != NULL)
	
	liballoc_unlock();

	return NULL;
}

void free(void *ptr) {
	struct liballoc_minor *min;
	struct liballoc_major *maj;

	if ( ptr == NULL ) 
	{
		l_warningCount += 1;
		return;
	}

	UNALIGN( ptr );

	liballoc_lock();


	min = (struct liballoc_minor*)((uintptr_t)ptr - sizeof( struct liballoc_minor ));

	
	if ( min->magic != LIBALLOC_MAGIC ) 
	{
		l_errorCount += 1;
		if ( 
			((min->magic & 0xFFFFFF) == (LIBALLOC_MAGIC & 0xFFFFFF)) || 
			((min->magic & 0xFFFF) == (LIBALLOC_MAGIC & 0xFFFF)) || 
			((min->magic & 0xFF) == (LIBALLOC_MAGIC & 0xFF)) 
		   )
		{
			l_possibleOverruns += 1;
		}
			
		liballoc_unlock();
		return;
	}
	

		maj = min->block;

		l_inuse -= min->size;

		maj->usage -= (min->size + sizeof( struct liballoc_minor ));
		min->magic  = LIBALLOC_DEAD;

		if ( min->next != NULL ) min->next->prev = min->prev;
		if ( min->prev != NULL ) min->prev->next = min->next;

		if ( min->prev == NULL ) maj->first = min->next;	

	if ( maj->first == NULL )
	{
		if ( l_memRoot == maj ) l_memRoot = maj->next;
		if ( l_bestBet == maj ) l_bestBet = NULL;
		if ( maj->prev != NULL ) maj->prev->next = maj->next;
		if ( maj->next != NULL ) maj->next->prev = maj->prev;
		l_allocated -= maj->size;

		liballoc_free( maj, maj->pages );
	}
	else
	{
		if ( l_bestBet != NULL )
		{
			int bestSize = l_bestBet->size  - l_bestBet->usage;
			int majSize = maj->size - maj->usage;

			if ( majSize > bestSize ) l_bestBet = maj;
		}

	}
	
	liballoc_unlock();
}





void* calloc(size_t nobj, size_t size) {
       int real_size;
       void *p;

       real_size = nobj * size;
       
       p = malloc( real_size );

       liballoc_memset( p, 0, real_size );

       return p;
}



void* realloc(void *p, size_t size) {
	void *ptr;
	struct liballoc_minor *min;
	unsigned int real_size;
	
	if ( size == 0 ) 
	{
		free( p );
		return NULL;
	}

	if ( p == NULL ) return malloc( size );

	ptr = p;
	UNALIGN(ptr);

	liballoc_lock();

		min = (struct liballoc_minor*)((uintptr_t)ptr - sizeof( struct liballoc_minor ));
		if ( min->magic != LIBALLOC_MAGIC ) 
		{
			l_errorCount += 1;
			if ( 
				((min->magic & 0xFFFFFF) == (LIBALLOC_MAGIC & 0xFFFFFF)) || 
				((min->magic & 0xFFFF) == (LIBALLOC_MAGIC & 0xFFFF)) || 
				((min->magic & 0xFF) == (LIBALLOC_MAGIC & 0xFF)) 
			   )
			{
				l_possibleOverruns += 1;
			}
			
			liballoc_unlock();
			return NULL;
		}	
		
		real_size = min->req_size;

		if ( real_size >= size ) 
		{
			min->req_size = size;
			liballoc_unlock();
			return p;
		}

	liballoc_unlock();

	ptr = malloc( size );
	liballoc_memcpy( ptr, p, real_size );
	free( p );

	return ptr;
}
