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


void init_gdt()
{
    gdt_ptr.limit = (sizeof(gdt_entry_t)*5) - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;

    // if(!(mbd->flags >> 6 & 0x1)) {
    //     panic("invalid memory map given by GRUB bootloader");
    // }

    // /* Loop through the memory map and display the values */
    // int i;
    // for(i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t)) 
    // {
    //     multiboot_memory_map_t* mmmt = 
    //         (multiboot_memory_map_t*) (mbd->mmap_addr + i);

    //     if(mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) {
    //        printf("Start Addr: %x | Length: %d | Size: %d | Type: %d\n",
    //         mmmt->addr, (int)mmmt->len, mmmt->size, mmmt->type);
    //     }
    // }

    gdt_set_gate(0,0,0,0,0);                    //Null segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); //Code segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); //Data segment
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); //User mode code segment
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); //User mode data segment

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