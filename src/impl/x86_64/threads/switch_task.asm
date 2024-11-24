.code64
.section .text
.global switch_stack

switch_stack:
    pushq %rbp
    movq %rsp, %rbp

    pushq %r15
    pushq %r14
    pushq %r13
    pushq %r12
    pushq %rbx
    pushq %rbp

    movq %rsp, (%rdi)
    movq (%rsi), %rsp

    popq %rbp
    popq %rbx
    popq %r12
    popq %r13
    popq %r14
    popq %r15

    leaveq
    ret