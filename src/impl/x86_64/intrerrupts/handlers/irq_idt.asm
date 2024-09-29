.section .text
.code64

.macro gen_irq num, arg
.global irq\num
irq\num:
    cli
    pushq $0
    pushq $\arg
    jmp irq_common_stub
.endm

gen_irq 0, 32
gen_irq 1, 33
gen_irq 2, 34
gen_irq 3, 35
gen_irq 4, 36
gen_irq 5, 37
gen_irq 6, 38
gen_irq 7, 39
gen_irq 8, 40
gen_irq 9, 41
gen_irq 10, 42
gen_irq 11, 43
gen_irq 12, 44
gen_irq 13, 45
gen_irq 14, 46
gen_irq 15, 47

.extern irq_handler
irq_common_stub:
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
    call irq_handler

    pop %rax # remove cr2

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
    # he adds 8 because of the zero, remember to check
    popq %rbx
    popq %rdx
    popq %rcx
    popq %rax

    addq $16, %rsp # get rid of int_num and error_code
    sti
    iretq