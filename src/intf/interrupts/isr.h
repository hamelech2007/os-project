#pragma once
#include "stdint.h"

struct int_regs {
    uint64_t cr2, cr3, gs, fs, es, ds; // our push in common stub
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8, rdi, rsi, rbp, /*zero here*/ rbx, rdx, rcx, rax; // our push in common stub
    uint64_t int_num, err_code; // our push in isr/int_num
    uint64_t rip, cs, rflags, rsp, ss; // push by CPU
}__attribute__((packed));


void isr_handler(struct int_regs* regs);
void irq_handler(struct int_regs* regs);

void unimplemented(struct int_regs* regs);
void divide_by_zero(struct int_regs* regs);
void invalid_opcode(struct int_regs* regs);
void general_protection(struct int_regs* regs);
void reserved(struct int_regs* regs);


void timer(struct int_regs* regs);
void keyboard_press(struct int_regs* regs);
void mouse_input(struct int_regs* regs);