#include <stdint.h>
#include <string.h>
#include <multiboot.h>
#include <debug/debug.h>
#include <kernel/panic.h>
#include <arch/memory/frames.h>

#define TOTAL_UINT32_FOR_BITMAP 32768

static uint32_t bitmap[TOTAL_UINT32_FOR_BITMAP] = { 0 };

extern uint32_t KERNEL_END;
static uint32_t kernel_end = (uint32_t)&KERNEL_END;

static uint32_t start_search = 0;
static uint32_t end_search = TOTAL_UINT32_FOR_BITMAP;

#define lock() ((void)0)

#define unlock() ((void)0)

static inline __attribute__((always_inline))
void mark_frame(uint32_t page_num) {
    bitmap[page_num >> 5] |= (1U << (page_num & 31));
}

#define mark_frame_addr(addr) mark_frame((addr) >> 12)

static inline __attribute__((always_inline))
void mark_frames(uint32_t start_page_num, uint32_t end_page_num) {
    for(uint32_t i = start_page_num; i < end_page_num; ++i) mark_frame(i);
}

#define mark_frames_addr(start_addr, end_addr) mark_frames((start_addr) >> 12, (end_addr) >> 12)

static inline __attribute__((always_inline))
int is_frame_marked(uint32_t page_num) {
    return bitmap[page_num >> 5] & (1U << (page_num & 31));
}

#define is_frame_marked_addr(addr) is_frame_marked((addr) >> 12)

static inline  __attribute__((always_inline))
int find_first_zero_bit(uint32_t num) {
    num = ~num;
    return __builtin_ffs(num) - 1;
}

#ifdef NDEBUG
#define test() ((void)0)
#else
static inline __attribute__((always_inline))
void test() {
    uint32_t first = alloc_frame();
    uint32_t second = alloc_frame();
    if(first == second)
        panic("Frames: test failed");
    free_frame(first);
    free_frame(second);
}
#endif

void mark_with_mmap(multiboot_info_t* mbd) {
    if(!(mbd->flags & MULTIBOOT_INFO_MEMORY)) {
        panic("Invalid info about memory");
    }
    debugf("end memory: 0x%08X\n", mbd->mem_upper << 10);
    debugf("total memory: %u KB (%u B)\n", mbd->mem_upper + 1024, (mbd->mem_upper + 1024) << 10);
    if(!(mbd->flags & MULTIBOOT_INFO_MEM_MAP)) {
        panic("Invalid memory map");
    }
    mark_frames_addr((mbd->mem_upper + 1024) << 10, 0xFFFFFFFF);
    mbd->mmap_addr += 0xC0000000;
    for(uint32_t i = 0; i < mbd->mmap_length;) {
        multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)(mbd->mmap_addr + i);
        if(mmap->type != MULTIBOOT_MEMORY_AVAILABLE) {
            uint32_t addr_high = ((mmap->base_addr_low + mmap->length_low) + 0xFFF) & ~0xFFF;
            uint32_t addr_low = mmap->base_addr_low & ~0xFFF;
            debugf("busy memory: 0x%08X (0x%08X), end: 0x%08X (0x%08X), len: %u\n", \
                mmap->base_addr_low, addr_low, \
                mmap->base_addr_low + mmap->length_low, addr_high, \
                mmap->length_low);
            mark_frames_addr(addr_low, addr_high);
        }
        i += mmap->size + sizeof(mmap->size);
    }
}

static inline __attribute__((always_inline))
void set_bounds(multiboot_info_t* mbd) {
    start_search = kernel_end >> 12;
    end_search = (((mbd->mem_upper << 10) + 0x100000) & ~0xFFF) >> 12;
}

void init_frames(multiboot_info_t* mbd) {
    kernel_end = (kernel_end + 0xFFF) & ~0xFFF;
    assertk((kernel_end & 0xFFF) == 0);

    mark_frames_addr(0, kernel_end);

    mark_with_mmap(mbd);

    set_bounds(mbd);
    debugf("start search: %u, end: %u\n", start_search, end_search);

    test();
}

uint32_t alloc_frame() {
    lock();
    uint32_t start_idx = start_search >> 5;
    uint32_t end_idx = (end_search + 31) >> 5;
    for(uint32_t i = start_idx; i < end_idx; ++i) {
        if(bitmap[i] != UINT32_MAX) {
            uint32_t page_num = (i << 5) + find_first_zero_bit(bitmap[i]);
            mark_frame(page_num);
            unlock();
            return page_num << 12;
        }
    }
    panic("Out of memory");
}

void free_frame(uint32_t frame_phys_addr) {
    lock();
    uint32_t page_num = frame_phys_addr >> 12;
    bitmap[page_num >> 5] &= ~(1U << (page_num & 31));
    unlock();
}
