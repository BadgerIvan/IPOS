#include <stdint.h>
#include <stddef.h>
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

page_dir_t* page_dir = NULL;

page_table_t* first_page_table = NULL;

void test_paging() {
    int* test_address = (int*)0xC0300000;
    *test_address = 0xDEADBEEF;
    int value = *test_address;
    if(value != 0xDEADBEEF) {
        debugf("Paging test failed: wrote 0xDEADBEEF, read 0x%08X\n", value);
        panic("Paging abort!");
    }
}

void init_paging(void* _page_dir, void* _first_page_table) {
    assertk(sizeof(page_dir_t) == 4);
    assertk(sizeof(page_table_t) == 4);

    page_dir = (page_dir_t*)_page_dir;
    first_page_table = (page_table_t*)_first_page_table;

    assertk(page_dir != NULL);
    assertk(first_page_table != NULL);

    debugf("page_dir address: 0x%08X\n", (uint32_t)page_dir);
    debugf("first_page_table address: 0x%08X\n", (uint32_t)first_page_table);

    test_paging();
}

