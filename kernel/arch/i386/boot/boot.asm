KALIGN equ (1 << 0)
MEMINFO equ (1 << 1)
MAGIC equ 1BADB002h 
FLAGS equ (KALIGN | MEMINFO)
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
stack_top:

section .text

global start
[extern _init]
[extern kernel_main]

start:
    mov esp, stack_top

    push ebx

    call _init

    call kernel_main

    cli

    .end:
    hlt
    jmp .end

