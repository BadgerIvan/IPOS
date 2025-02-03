#include <stdint.h>

#include <arch/cpu/gdt.h>

typedef struct gdt_entry
{
    uint16_t limit_low;          
    uint16_t base_low;            
    uint8_t  base_middle;         
    uint8_t  access;              
    uint8_t  granularity;         
    uint8_t  base_high;       
} __attribute__((packed)) gdt_entry_t;

typedef struct gdt_ptr
{
    uint16_t limit;              
    uint32_t base;                
} __attribute__((packed)) gdt_ptr_t;

extern void load_gdt(gdt_ptr_t *gdt_ptr);

static void gdt_set_gate(int32_t,uint32_t,uint32_t,uint8_t,uint8_t);

gdt_entry_t gdt_entries[5];
gdt_ptr_t   gdt_ptr;


void init_gdt(const multiboot_info_t *mbd)
{
    gdt_ptr.limit = (sizeof(gdt_entry_t)*5) - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;

    debugf("Kernel size: %d bytes\n", (uint32_t)&KERNEL_END - (uint32_t)&KERNEL_START);

    uint32_t max_memory = 0;

    if (!(mbd->flags >> 6 & 0x1)) {
        panic("invalid memory map given by bootloader");
    }

     for (uint32_t i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t)) {
        const multiboot_memory_map_t* mmmt = (multiboot_memory_map_t*)(mbd->mmap_addr + i);

        if (mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) {
           uint32_t end_addr = mmmt->base_addr_low + mmmt->length_low;
            if (end_addr > max_memory) {
                max_memory = end_addr;
            }
        }
    }

    uint32_t memory_limit = max_memory - 1;

    if (memory_limit > (uint32_t)&KERNEL_END) {
        memory_limit = (uint32_t)&KERNEL_END - 1;
    }

    gdt_set_gate(0,0,0,0,0);                      //Null segment
    gdt_set_gate(1, 0, memory_limit, 0x9A, 0xCF); //Code segment
    gdt_set_gate(2, 0, memory_limit, 0x92, 0xCF); //Data segment
    gdt_set_gate(3, 0, memory_limit, 0xFA, 0xCF); //User mode code segment
    gdt_set_gate(4, 0, memory_limit, 0xF2, 0xCF); //User mode data segment

    load_gdt(&gdt_ptr);
}

static void gdt_set_gate(int32_t entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    gdt_entries[entry].base_low = (base & 0xFFFF);
    gdt_entries[entry].base_middle = (base >> 16) & 0xFF;
    gdt_entries[entry].base_high = (base >> 24) & 0xFF;

    gdt_entries[entry].limit_low = (limit & 0xFFFF);
    gdt_entries[entry].granularity = (limit >> 16) & 0x0F;

    gdt_entries[entry].granularity |= gran & 0xF0;
    gdt_entries[entry].access = access;
}