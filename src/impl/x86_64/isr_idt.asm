.global handler

.section .text
.code64
handler:
    iretq


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
    #pusha
    #movq %ds, %rax
    #pushq %rax
    #movq %cr2, %eax
    #pushq %rax
#
    #movq $0x10, %rax
    #mov %ax, %ds
    #mov %ax, %es
    #mov %ax, %fs
    #mov %ax, %gs
#
    #pushq %rsp
    #call isr_handler
#
    #add $8, %rsp
    #popq %rbx
    #mov %bx, %ds
    #mov %bx, %es
    #mov %bx, %fs
    #mov %bx, %gs
#
    #popa
    #add $8, %rsp
    #sti
    #iretq
#
