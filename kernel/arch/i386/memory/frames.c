//1048576 - всего страниц
//32768 - нужно uint32_t для битовых масок
//128Кб для хранения
#include <stdint.h>
#include <multiboot.h>
#include <debug/debug.h>
#include <kernel/panic.h>

static uint32_t bitmap[32768] = { 0 };

extern uint32_t KERNEL_END;
static uint32_t _end = (uint32_t)&KERNEL_END;

#define MARK_NUM(num) bitmap[num >> 5] |= (1 << (num & 31))

void init_frames() {
    _end = (_end + 0xFFF) & ~0xFFF;
    assertk((_end & 0xFFF) == 0);

    uint32_t num_pages = _end >> 12;

    for(uint32_t i = 0; i < num_pages; i += 1) {
        MARK_NUM(i);
    }
}

void mark_with_mmap(multiboot_info_t* mbd) {
    if(!(mbd->flags & MULTIBOOT_INFO_MEM_MAP)) {
        panic("Invalid memory map");
    }

    for(uint32_t i = 0; i < mbd->mmap_length;) {
        multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)(mbd->mmap_addr + i);
        if(mmap->type != MULTIBOOT_MEMORY_AVAILABLE) {
            uint64_t addr_high = ((mmap->addr + mmap->length) + 0xFFF) & ~0xFFF;
            uint64_t addr_low = mmap->addr & ~0xFFF;
            for(uint64_t addr = addr_low; addr < addr_high; addr += 0x1000) {
                MARK_NUM(addr >> 12);
            }
        }
        i += mmap->size + sizeof(mmap->size);
    }
}