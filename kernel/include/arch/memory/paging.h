#ifndef _PAGING_H
#define _PAGING_H

#include <stdint.h>

#define PG_ONLYREAD (0 << 1)
#define PG_READWRITE (1 << 1)

#define PG_KERNEL (0 << 2)
#define PG_USER (1 << 2)

#define PG_PHYS_ADDR (1 << 3)

void init_paging(void* page_dir, void* first_page_table);
uint32_t map_pages(uint32_t pages, uint32_t flags);
void unmap_pages(uint32_t virt_addr, uint32_t pages);

#endif