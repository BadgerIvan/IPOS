#include <stdint.h>
#include <stddef.h>
#include <arch/memory/frames.h>
#include <arch/memory/paging.h>
#include <kernel/panic.h>
#include <debug/debug.h>

typedef union page_directory_entry {
    struct {
        uint32_t present : 1;
        uint32_t read_write : 1;
        uint32_t user_supervisor : 1;
        uint32_t pwt : 1;               // write through
        uint32_t pcd : 1;               // cache disable
        uint32_t accessed : 1;
        uint32_t reserved : 1;
        uint32_t page_size : 1;         // 0 for 4KB pages
        uint32_t global : 1;            // ignored for page directory
        uint32_t available : 3;
        uint32_t address : 20;          // page table address (shifted right 12 bits)
    } bits;
    uint32_t value;
} page_directory_entry_t;

typedef union page_table_entry {
    struct {
        uint32_t present : 1;
        uint32_t read_write : 1;
        uint32_t user_supervisor : 1;
        uint32_t pwt : 1;               // write through
        uint32_t pcd : 1;               // cache disable
        uint32_t accessed : 1;
        uint32_t dirty : 1;
        uint32_t pat : 1;               // page attribute table
        uint32_t global : 1;
        uint32_t available : 3;
        uint32_t address : 20;          // page frame number (shifted right 12 bits)
    } bits;
    uint32_t value;
} page_table_entry_t;

page_directory_entry_t* page_dir = NULL;

page_table_entry_t* first_page_table = NULL;

void init_paging(void* _page_dir, void* _first_page_table) {
    assertk(sizeof(page_directory_entry_t) == 4);
    assertk(sizeof(page_table_entry_t) == 4);

    page_dir = (page_directory_entry_t*)_page_dir;
    first_page_table = (page_table_entry_t*)_first_page_table;

    assertk(page_dir != NULL);
    assertk(first_page_table != NULL);

    debugf("page_dir address: 0x%08X\n", (uint32_t)page_dir);
    debugf("first_page_table address: 0x%08X\n", (uint32_t)first_page_table);
}

uint32_t map_frame(uint32_t phys_addr, uint32_t flags) {
    for(int i = 0; i < 1024; i++) {
        if(first_page_table[i].bits.present == 0) {
            first_page_table[i].value = 0;
            first_page_table[i].value |= flags & 0x6;
            first_page_table[i].bits.address = phys_addr >> 12;
            first_page_table[i].bits.present = 1;
            uint32_t virt_addr = i * 4096;
            if((flags & PG_KERNEL) == 0)
                virt_addr += 0xC0000000;
            asm volatile("invlpg (%0)" : : "r" (virt_addr));
            return virt_addr;
        }
    }
    panic("Out of first_page_table");
}

//void unmap_frame(uint32_t)
