.section .text
.code64

.macro gen_isr_noerror num
.global isr\num
isr\num:
    cli
    pushq $0
    pushq $\num
    jmp isr_common_stub
.endm

.macro gen_isr_error num
.global isr\num
isr\num:
    cli
    pushq $\num
    jmp isr_common_stub
.endm

gen_isr_noerror 0
gen_isr_noerror 1
gen_isr_noerror 2
gen_isr_noerror 3
gen_isr_noerror 4
gen_isr_noerror 5
gen_isr_noerror 6
gen_isr_noerror 7
gen_isr_error   8
gen_isr_noerror 9
gen_isr_error   10
gen_isr_error   11
gen_isr_error   12
gen_isr_error   13
gen_isr_error   14
gen_isr_noerror 15
gen_isr_noerror 16
gen_isr_noerror 17
gen_isr_noerror 18
gen_isr_noerror 19
gen_isr_noerror 20
gen_isr_noerror 21
gen_isr_noerror 22
gen_isr_noerror 23
gen_isr_noerror 24
gen_isr_noerror 25
gen_isr_noerror 26
gen_isr_noerror 27
gen_isr_noerror 28
gen_isr_noerror 29
gen_isr_noerror 30
gen_isr_noerror 31
gen_isr_noerror 128
gen_isr_noerror 177

.extern isr_handler
isr_common_stub:
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %rbx
    // he pushes 0 here for some reason, check if not work
    pushq %rbp
    pushq %rsi
    pushq %rdi
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    
    xorq %rax, %rax
    mov %ds, %ax
    pushq %rax

    mov %es, %ax
    pushq %rax

    pushq %fs
    pushq %gs

    movq %cr3, %rax
    pushq %rax

    movq %cr2, %rax
    pushq %rax

    movq %rsp, %rdi # pass rsp as first parameter, basically struct regs
    call isr_handler

    pop %rax // remove cr2

    pop %rax
    movq %rax, %cr3

    popq %gs
    popq %fs

    popq %rax
    mov %ax, %es

    popq %rax
    mov %ax, %ds

    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdi
    popq %rsi
    popq %rbp
    // he adds 8 because of the zero, remember to check
    popq %rbx
    popq %rdx
    popq %rcx
    popq %rax

    addq $16, %rsp // get rid of int_num and error_code
    sti
    iretq