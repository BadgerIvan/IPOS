#include <stdint.h>
#include <arch/memory/paging.h>
#include <kernel/panic.h>
#include <debug/debug.h>

typedef struct {
    uint32_t present : 1;
    uint32_t read_write : 1;
    uint32_t user_supervisor : 1;
    uint32_t pwt : 1;               //write_throught
    uint32_t pcd : 1;               //chahe disable
    uint32_t accessed : 1;
    uint32_t reserved : 1;
    uint32_t always_zero : 1; 
    uint32_t available : 4;
    uint32_t address : 20;
} __attribute__((packed)) page_dir_t;

typedef struct {
    uint32_t present : 1;
    uint32_t read_write : 1;
    uint32_t user_supervisor : 1;
    uint32_t pwt : 1;               //write_throught
    uint32_t pcd : 1;               //chahe disable
    uint32_t accessed : 1;
    uint32_t dirty : 1;
    uint32_t pat : 1;               //page attr table
    uint32_t global : 1;
    uint32_t available : 3;
    uint32_t address : 20;
} __attribute__((packed)) page_table_t;

page_dir_t page_dir[1024] __attribute__((aligned(4096)));

page_table_t first_page_table[1024] __attribute__((aligned(4096)));

void test_paging() {
    int* test_address = (int*)0x300000;
    *test_address = 10;
    if(first_page_table[768].accessed != 1)
        panic("PAGING ABORT!");
    if(first_page_table[768].dirty != 1)
        panic("PAGING ABORT!");
    if(*test_address != 10)
        panic("PAGING ABORT!");
}

void init_paging() {
    assertk(sizeof(page_dir_t) == 4);
    assertk(sizeof(page_table_t) == 4);
    
    __builtin_memset(&page_dir, 0, sizeof(page_dir_t) * 1024);
    __builtin_memset(&first_page_table, 0, sizeof(page_table_t) * 1024);
    for(int i = 0; i < 1024; i++) {
        first_page_table[i].present = 1;
        first_page_table[i].read_write = 1;
        first_page_table[i].address = i;
    }
    page_dir[0].present = 1;
    page_dir[0].read_write = 1;
    page_dir[0].address = ((uint32_t)first_page_table) >> 12;

    asm volatile("mov %0, %%cr3" : : "r" (page_dir));
    
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r" (cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0" : : "r" (cr0));

    test_paging();
}

