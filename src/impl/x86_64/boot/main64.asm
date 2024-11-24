.global long_mode_start, long_jump_new_gdt
.extern entry_64, error, kernel_main

.section .bss
stack_bottom:
    .skip 16384*4
stack_top:

.section .text
.code64
long_mode_start:
    cli
    movw $0, %ax
    movw %ax, %ss
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    movq %rsp, %rax
    
    movabs $stack_top, %rsp
    pushq %rax

    call entry_64
    
    popq %rsp
    hlt


long_jump_new_gdt:
    
    mov $0x10, %ax  # Kernel Data segment (0x10 = index 2 in GDT)
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    sub $16, %rsp
    movq $8, 8(%rsp)
    movabsq $.1, %rax
    mov %rax, (%rsp)
    lretq
.1:
    ret