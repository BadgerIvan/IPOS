#include <stdint.h>
#include <stddef.h>
#include <arch/memory/frames.h>
#include <arch/memory/paging.h>
#include <kernel/panic.h>
#include <debug/debug.h>

#define FOR_NEXT_PAGE 0x1

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

page_table_entry_t* page_tables[1024] = { 0 };

uint32_t start_index = 768;
uint32_t end_index = 769;
uint32_t index_save_for_next_page_table = 0;
page_table_entry_t* save_for_next_page_table = NULL;

static inline __attribute__((always_inline))
void invlpg(uint32_t virt_addr) {
    asm volatile("invlpg (%0)" : : "r" (virt_addr));
}

void init_paging(void* _page_dir, void* _first_page_table) {
    assertk(sizeof(page_directory_entry_t) == 4);
    assertk(sizeof(page_table_entry_t) == 4);

    page_dir = (page_directory_entry_t*)_page_dir;
    page_tables[start_index] = (page_table_entry_t*)_first_page_table;

    assertk(page_dir != NULL);

    debugf("page_dir address: 0x%08X\n", (uint32_t)page_dir);
    debugf("first_page_table address: 0x%08X\n", (uint32_t)page_tables[start_index]);
    
    int flag = 0;
    for(uint32_t i = 0; i < 1024; i++) {
        if(page_tables[start_index][i].bits.present == 0) {
            debugf("Kernel + 1MB takes %d pages\n", i);
            page_tables[start_index][i].bits.available = FOR_NEXT_PAGE;
            save_for_next_page_table = &page_tables[start_index][i];
            index_save_for_next_page_table = i;
            flag = 1;
            break;
        }
    }
    if(!flag)
        panic("Out of first_page_table");
}

static void create_page_table(uint32_t index_in_page_dir) {
    debugf("START CREATE NEW TABLE %d\n", index_in_page_dir);
    uint32_t phys_addr = alloc_frame();
    save_for_next_page_table->bits.read_write = 1;
    save_for_next_page_table->bits.address = phys_addr >> 12;
    save_for_next_page_table->bits.present = 1;
    uint32_t virt_addr = (index_in_page_dir - 1 << 22) + (index_save_for_next_page_table << 12);
    page_dir[index_in_page_dir].value = 0;
    page_dir[index_in_page_dir].bits.read_write = 1;
    page_dir[index_in_page_dir].bits.address = phys_addr >> 12;
    page_dir[index_in_page_dir].bits.present = 1;
    invlpg(virt_addr);
    __builtin_memset((void*)virt_addr, 0, 4096);
    page_table_entry_t* page_entry_next = (page_table_entry_t*)(virt_addr);
    page_tables[index_in_page_dir] = page_entry_next;
    page_entry_next[0].bits.available = FOR_NEXT_PAGE;
    save_for_next_page_table = &page_entry_next[0];
    index_save_for_next_page_table = 0;
    debugf("END CREATE NEW TABLE %d\n", index_in_page_dir);
}

uint32_t map_frame(uint32_t phys_addr, uint32_t flags) {
    if(flags & PG_USER)
        panic("User mode blocked");
    flags &= 0x6;
    for(uint32_t i = start_index; i < end_index; i++) {
        for(uint32_t j = 0; j < 1024; j++) {
            if(page_tables[i] == NULL)
                break;
            if(page_tables[i][j].bits.present == 0 && page_tables[i][j].bits.available != FOR_NEXT_PAGE) {
                page_tables[i][j].value = 0;
                page_tables[i][j].value |= flags;
                page_tables[i][j].bits.address = phys_addr >> 12;
                page_tables[i][j].bits.present = 1;
                uint32_t virt_addr = (i << 22) + (j << 12);
                invlpg(virt_addr);
                return virt_addr;
            }
        }
        if(page_tables[i] == NULL)
            break;
    }
    if(end_index == 1024)
            panic("Ouf of memory kernel");
    create_page_table(end_index);
    page_tables[end_index][1].value = 0;
    page_tables[end_index][1].value |= flags;
    page_tables[end_index][1].bits.address = phys_addr >> 12;
    page_tables[end_index][1].bits.present = 1;
    uint32_t virt_addr = (end_index << 22) + (1 << 12);
    end_index++;
    invlpg(virt_addr);
    return virt_addr;
}

void unmap_frame(uint32_t virt_addr) {
    uint32_t pd_index = virt_addr >> 22;
    uint32_t pte_index = (virt_addr >> 12) & 0x3FF;
    page_tables[pd_index][pte_index].value = 0;
    invlpg(virt_addr);
}
