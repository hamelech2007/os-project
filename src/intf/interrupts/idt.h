#pragma once
#include "stdint.h"


struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attributes;
    uint16_t offset_middle;
    uint32_t offset_high;
    uint32_t reserved;
} __attribute__((packed)) ;

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

void init_idt();
void store_idt(struct idt_ptr *idtr);
void load_idt(struct idt_ptr *idtr);
void set_gate_offset(struct idt_entry *gate, uint64_t addr);
uint64_t get_gate_offset(struct idt_entry *gate);
void set_trap_gate(uint8_t num, uint64_t addr, uint8_t ist, uint8_t dpl, bool user);
void set_int_gate(uint8_t num, uint64_t addr, uint8_t ist, uint8_t dpl, bool user);

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

extern void isr128();
extern void isr177();

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();