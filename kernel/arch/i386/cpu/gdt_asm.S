.section .text
.global load_gdt

.type load_gdt, @function

load_gdt:
    movl    4(%esp), %eax
    lgdt	(%eax)

    movw	$0x10, %ax
    movw	%ax, %ds
    movw	%ax, %es
    movw	%ax, %fs
    movw	%ax, %gs
    movw	%ax, %ss
    ljmp	$0x08, $.flush  # Long jump to our new code segment
.flush:
    ret