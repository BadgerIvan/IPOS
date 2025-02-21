KALIGN equ 1 << 0
MEMINFO equ 1 << 1
MAGIC equ 0x1BADB002
FLAGS equ KALIGN | MEMINFO
CHECKSUM equ -(MAGIC + FLAGS)

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

section .text

global _start
[extern _init]
[extern kernel_main]

_start:
    cli

    mov esp, stack_top

    cmp eax, 0x2BADB002
    jnz .end
     
    push ebx

    call _init

    call kernel_main

    cli

    .end:
    hlt
    jmp .end

