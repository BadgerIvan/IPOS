KALIGN equ 1 << 0
MEMINFO equ 1 << 1
MAGIC equ 0x1BADB002
FLAGS equ KALIGN | MEMINFO
CHECKSUM equ -(MAGIC + FLAGS)

VIRTUAL_BASE equ 0xC0000000

[bits 32]

section .multiboot
align 4
dd MAGIC
dd FLAGS
dd CHECKSUM

section .bss
align 16
stack_bottom:
resb 16384  ; 16 KiB
stack_top equ stack_bottom + 16384

section .data
align 16
boot_page_dir:
    times 4096 db 0
boot_first_page_table:
    times 4096 db 0

section .boot.text

global _start
extern KERNEL_START
extern KERNEL_END

_start:
    cli

    cmp eax, 0x2BADB002
    jnz .end

    call to_higher_half_kernel

    .end:
    hlt
    jmp .end

to_higher_half_kernel:
    mov edi, (boot_first_page_table - VIRTUAL_BASE)
    mov esi, 0
    mov ecx, 1024

    .map_pages:
    cmp esi, KERNEL_END
    jge .done_mapping
    mov edx, esi
    or edx, 0x003
    mov [edi], edx

    add esi, 4096
    add edi, 4
    loop .map_pages

    .done_mapping:

    mov dword [boot_page_dir - VIRTUAL_BASE + 0], (boot_first_page_table - VIRTUAL_BASE + 0x003)
    mov dword [boot_page_dir - VIRTUAL_BASE + 768 * 4], (boot_first_page_table - VIRTUAL_BASE + 0x003)

    mov ecx, (boot_page_dir - VIRTUAL_BASE)
    mov cr3, ecx

    mov ecx, cr0
    or ecx, 0x80010000
    mov cr0, ecx

    lea ecx, [.in_higher_half]
    jmp ecx


section .text
.in_higher_half:
    mov dword [boot_page_dir + 0], 0

    mov ecx, cr3
    mov cr3, ecx

    mov esp, stack_top

    push dword boot_first_page_table

    push dword boot_page_dir

    add ebx, VIRTUAL_BASE
    push ebx

    extern kernel_main
    call kernel_main

    cli
    .end:
    hlt
    jmp .end

