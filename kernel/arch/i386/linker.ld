ENTRY(_start)

VIRTUAL_BASE = 0xC0000000;
PHYSICAL_BASE = 0x00100000;

SECTIONS
{
    
    . = PHYSICAL_BASE;
    
    .multiboot BLOCK(4K) : ALIGN(4K) 
	{
        *(.multiboot)
    }
    
    .boot.text BLOCK(4K) : ALIGN(4K) 
	{
        *(.boot.text)
    }
       
    . += VIRTUAL_BASE;
    
    .text BLOCK(4K) : AT(ADDR(.text) - VIRTUAL_BASE) ALIGN(4K) 
    {
        *(.text)
    }
    
    .rodata BLOCK(4K) : AT(ADDR(.rodata) - VIRTUAL_BASE) ALIGN(4K) 
    {
        *(.rodata)
    }
    
    .data BLOCK(4K) : AT(ADDR(.data) - VIRTUAL_BASE) ALIGN(4K) 
    {
        *(.data)
    }
    
    .bss BLOCK(4K) : AT(ADDR(.bss) - VIRTUAL_BASE) ALIGN(4K) 
    {
        *(COMMON)
        *(.bss)
    }

	KERNEL_START = PHYSICAL_BASE;
	KERNEL_VIRTUAL_START = VIRTUAL_BASE + PHYSICAL_BASE;
	KERNEL_VIRTUAL_END = .;
	KERNEL_END = . - VIRTUAL_BASE;
}