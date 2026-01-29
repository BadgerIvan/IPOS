#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <arch/memory/frames.h>
#include <arch/memory/paging.h>
#include <arch/cpu/isr.h>
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

static page_directory_entry_t* page_dir = NULL;

static page_table_entry_t* page_tables[1024] = { 0 };

static uint32_t start_index = 768;
static uint32_t end_index = 769;

#define lock() ((void)0)

#define unlock() ((void)0)

static inline __attribute__((always_inline))
void invlpg(uint32_t virt_addr) {
    asm volatile("invlpg (%0)" : : "r" (virt_addr));
}

void page_fault(registers_t* regs) {
    uint32_t faulting_address;
    asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

    int present = !(regs->err_code & 0x1);
    int rw = regs->err_code & 0x2 ? 1 : 0;
    int us = regs->err_code & 0x4 ? 1 : 0;
    int reserved = regs->err_code & 0x8 ? 1 : 0;

    printk("Page fault: p:%d rw:%d su:%d r:%d at 0x%08X\n", 
        present, rw, us, reserved, faulting_address);
    uint32_t index_in_pd = faulting_address >> 22;
    uint32_t index_in_pt = (faulting_address >> 12) & 0x3FF;
    printk("index in pd: %d, index in pt: %d\n", index_in_pd, index_in_pt);
    panic("Page fault");
}

static
uint32_t map_page(uint32_t idx_in_pde, uint32_t idx_in_pte, uint32_t flags, ...) {
    if(page_tables[idx_in_pde] == NULL || page_tables[idx_in_pde][idx_in_pte].bits.present == 1)
        return 0;
    uint32_t phys_addr = 0;
    if(flags & PG_PHYS_ADDR) {
        va_list arg;
        va_start(arg, flags);
        phys_addr = (uint32_t)va_arg(arg, uint32_t);
        va_end(arg);
    } else{
        phys_addr = alloc_frame();
    }
    flags &= 0x6;
    page_tables[idx_in_pde][idx_in_pte].value = 0;
    page_tables[idx_in_pde][idx_in_pte].value |= flags;
    page_tables[idx_in_pde][idx_in_pte].bits.address = phys_addr >> 12;
    page_tables[idx_in_pde][idx_in_pte].bits.present = 1;
    uint32_t virt_addr = (idx_in_pde << 22) + (idx_in_pte << 12);
    invlpg(virt_addr);
    __builtin_memset((void*)virt_addr, 0, 4096);
    return virt_addr;
}

void init_paging(void* _page_dir, void* _first_page_table) {
    register_interrupt_handler(ISR14, page_fault);

    assertk(sizeof(page_directory_entry_t) == 4);
    assertk(sizeof(page_table_entry_t) == 4);

    page_dir = (page_directory_entry_t*)_page_dir;
    page_tables[start_index] = (page_table_entry_t*)_first_page_table;

    assertk(page_dir != NULL);
    assertk(page_tables[start_index] != NULL);

    debugf("page_dir address: 0x%08X\n", (uint32_t)page_dir);
    debugf("first_page_table address: 0x%08X\n", (uint32_t)page_tables[start_index]);
    
    int flag = 0;
    for(uint32_t i = start_index; i < end_index; i++) {
        for(uint32_t j = 0; j < 1024; j++) {
            if(page_tables[i] == NULL || page_tables[i][j].bits.present == 1)
                continue;
            uint32_t phys_addr = alloc_frame();
            uint32_t virt_addr = map_page(i, j, PG_KERNEL | PG_READWRITE | PG_PHYS_ADDR, phys_addr);
            page_tables[1023] = (page_table_entry_t*)virt_addr;
            page_dir[1023].bits.address = phys_addr >> 12;
            page_dir[1023].bits.read_write = 1;
            page_dir[1023].bits.present = 1;
            flag = 1;
            break;
        }
    }
    if(!flag)
        panic("Out of first pte");
}

static 
int create_new_pte(uint32_t idx_in_pd) {
    if(idx_in_pd > 1022)
        return 0;
    if(page_dir[idx_in_pd].bits.present == 1)
        return 0;
    uint32_t phys_addr = alloc_frame();
    uint32_t virt_addr = map_page(1023, idx_in_pd, PG_KERNEL | PG_READWRITE | PG_PHYS_ADDR, phys_addr);
    if(virt_addr == 0) {
        free_frame(phys_addr);
        return 0;
    }
    page_tables[idx_in_pd] = (page_table_entry_t*)virt_addr;
    page_dir[idx_in_pd].value = 0;
    page_dir[idx_in_pd].bits.read_write = 1;
    page_dir[idx_in_pd].bits.address = phys_addr >> 12;
    page_dir[idx_in_pd].bits.present = 1;
    return 1;
}

static
int find_free_pages(uint32_t pages, uint32_t* idx_in_pd, uint32_t* idx_in_pte, \
    uint32_t* last_idx_in_pd, uint32_t* last_idx_in_pte) {
    do {
        uint32_t find_pages = 0;
        for(uint32_t i = start_index; i < end_index; i++) {
            if(page_tables[i] == NULL) {
                find_pages = 0;
                break;
            }
            for(uint32_t j = 0; j < 1024; j++) {
                if(page_tables[i][j].bits.present == 0) {
                    find_pages++;
                    if(find_pages == 1) {
                        *idx_in_pd = i;
                        *idx_in_pte = j;
                    }
                    if(find_pages == pages) {
                        *last_idx_in_pd = i;
                        *last_idx_in_pte = j;
                        return 1;
                    }
                } else
                    find_pages = 0;
            }
        }
        if(!create_new_pte(end_index)) {
            return 0;
        }
        end_index++;
    } while(1);
}

uint32_t map_pages(uint32_t pages, uint32_t flags) {
    if(flags & PG_USER) 
        panic("Not yet");
    flags &= 0x6;
    uint32_t idx_in_pd = 0;
    uint32_t idx_in_pte = 0;
    uint32_t last_idx_in_pd = 0;
    uint32_t last_idx_in_pte = 0;
    lock();
    if(!find_free_pages(pages, &idx_in_pd, &idx_in_pte, &last_idx_in_pd, &last_idx_in_pte))
        return 0;
    for(uint32_t i = idx_in_pd; i <= last_idx_in_pd; i++) {
        uint32_t start = 0;
        uint32_t end = 0;
        if(i == idx_in_pd)
            start = idx_in_pte;
        else 
            start = 0;
        if(i == last_idx_in_pd)
            end = last_idx_in_pte;
        else 
            end = 1023;
        for(uint32_t j = start; j <= end; j++) {
            uint32_t tmp = map_page(i, j, flags);
            if(tmp == 0)
                panic("Map page fault");
        }
    }
    unlock();
    return (idx_in_pd << 22) + (idx_in_pte << 12);
}

void unmap_pages(uint32_t virt_addr, uint32_t pages) {
    if(virt_addr & 0xFFF)
        return;
    lock();
    for(uint32_t i = 0; i < pages; i++) {
        uint32_t pd_index = virt_addr >> 22;
        uint32_t pte_index = (virt_addr >> 12) & 0x3FF;
        uint32_t phys_addr = page_tables[pd_index][pte_index].bits.address << 12;
        page_tables[pd_index][pte_index].value = 0;
        invlpg(virt_addr);
        free_frame(phys_addr);
    }
    unlock();
}
