.global _start, error, page_table_l4, page_table_l3, page_table_l2, page_table_l1, page_table_l3_higher, page_table_l2_higher, page_table_l1_higher
.extern long_mode_start

.section .boot
.code32
_start:
    leal stack_top_boot, %esp
    pushl %ebx

    call check_multiboot
    call check_cpuid
    call check_long_mode

    call setup_page_tables
tst5:
    call enable_paging


tst4:
    lgdt gdt64_pointer
    popl %edi
    ljmp $gdt64_code_segment, $enter_64
    
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

    leal page_table_l3_higher, %eax
    orl $0b11, %eax # present, writable flags
    movl %eax, page_table_l4+4088 # pml4[511]

    leal page_table_l2, %eax
    orl $0b11, %eax # present, writable flags
    movl %eax, page_table_l3

    leal page_table_l2_higher, %eax
    orl $0b11, %eax # present, writable flags
    movl %eax, page_table_l3_higher # pdpt[510]

    leal page_table_l1, %eax
    orl $0b11, %eax
    movl %eax, page_table_l2

    leal page_table_l1_higher, %eax
    orl $0b11, %eax
    movl %eax, page_table_l2_higher

    movl $0, %ecx
.loop:
    movl $0x1000, %eax
    mul %ecx
    orl $0b11, %eax # present, writable flags
    movl %eax, page_table_l1(,%ecx, 8)
    incl %ecx
    cmpl $512, %ecx
    jne .loop

    movl $0, %ecx
.loop2:
    movl $0x1000, %eax
    mul %ecx
    orl $0b11, %eax # present, writable flags
    movl %eax, page_table_l1_higher(,%ecx, 8)
    incl %ecx
    cmpl $512, %ecx
    jne .loop2

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



.section .protected_memory
.align 4096
page_table_l4:
    .skip 4096
page_table_l3:
    .skip 4096
page_table_l3_higher:
    .skip 4096
page_table_l2:
    .skip 4096
page_table_l2_higher:
    .skip 4096
page_table_l1:
    .skip 4096
page_table_l1_higher:
    .skip 4096
stack_bottom_boot:
    .skip 4096
stack_top_boot:

gdt64:
    .quad 0
.equ gdt64_code_segment, . - gdt64
    .quad (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)

gdt64_pointer:
    .short . - gdt64 - 1
    .quad gdt64

.section .boot
.code64
enter_64:
    movabs $long_mode_start, %rax
tst65:
    jmp *%rax