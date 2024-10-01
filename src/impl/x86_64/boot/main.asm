.global _start, gdt_flush, error
.extern long_mode_start, initGdt, kernel_code_segment, entry_32, PML4T, PDPT, PDT, PT

.section .boot
.code32
_start:
    leal stack_top, %esp
    pushl %ebx

    call check_multiboot
    call check_cpuid
    call check_long_mode

    call entry_32
    call setup_page_tables
    call enable_paging



    lgdt gdt64_pointer
    popl %edi
    ljmp $gdt64_code_segment, $long_mode_start
    
    hlt

check_multiboot:
    cmpl $0x36d76289, %eax
    jne .no_multiboot
    ret

.no_multiboot:
    movb $'M', %al
    jmp error

check_cpuid:
    pushfl
    popl %eax
    movl %eax, %ecx
    xorl $(1 << 21), %eax
    pushl %eax
    popfl
    pushfl
    popl %eax
    pushl %ecx
    popfl
    cmpl %ecx, %eax
    je .no_cpuid
    ret

.no_cpuid:
    movb $'C', %al
    jmp error

check_long_mode:
    movl $0x80000000, %eax
    cpuid
    cmp $0x80000001, %eax
    jb .no_long_mode

    movl $0x80000001, %eax
    cpuid
    test $(1 << 29), %edx
    jz .no_long_mode
    ret

.no_long_mode:
    movb $'L', %al
    jmp error

setup_page_tables:
    leal page_table_l3, %eax
    orl $0b11, %eax # present, writable flags
    movl %eax, page_table_l4

    leal page_table_l2, %eax
    orl $0b11, %eax # present, writable flags
    movl %eax, page_table_l3

    movl $0, %ecx
.loop:

    movl $0x200000, %eax
    mul %ecx
    orl $0b10000011, %eax # present, writable flags, huge page
    movl %eax, page_table_l2(,%ecx, 8)

    incl %ecx
    cmpl $512, %ecx
    jne .loop

    ret

enable_paging:
    leal page_table_l4, %eax
    movl %eax, %cr3

    movl %cr4, %eax
    orl $(1 << 5), %eax
    movl %eax, %cr4

    movl $0xc0000080, %ecx
    rdmsr
    orl $(1 << 8), %eax
    wrmsr

    movl %cr0, %eax
    orl $(1 << 31), %eax
    movl %eax, %cr0

    ret

error:
    movl $0x4f524f45 ,0xb8000
    movl $0x4f3a4f52 ,0xb8004
    movl $0x4f204f20 ,0xb8008
    movb %al, 0xb800a
    hlt



.section .bss
.align 4096
.global page_table_l4, page_table_l3, page_table_l2, page_table_l1
page_table_l4:  # PML4T
    .skip 4096
page_table_l3:  # PDPT
    .skip 4096
page_table_l2:  # PDT
    .skip 4096  
page_table_l1:  # PT
    .skip 4096
stack_bottom:
    .skip 16384*4
stack_top:

.section .rodata
gdt64:
    .quad 0
.equ gdt64_code_segment, . - gdt64
    .quad (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)

gdt64_pointer:
    .short . - gdt64 - 1
    .quad gdt64


