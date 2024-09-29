#include "idt.h"
#include "memory.h"
#include "util.h"

struct idt_entry idt[256];
struct idt_ptr idt_pointer;


void setupPic() {
    outPortB(0x20, 0x11);
    outPortB(0xA0, 0x11);

    outPortB(0x21, 0x20);
    outPortB(0xa1, 0x28);

    outPortB(0x21, 0x04);
    outPortB(0xa1, 0x02);

    outPortB(0x21, 0x01);
    outPortB(0xa1, 0x01);

    outPortB(0x21, 0x0);
    outPortB(0xa1, 0x0);
}

void setupGates() {
    set_int_gate(0, (uint64_t) isr0, 0, 0, false);
    set_int_gate(1, (uint64_t) isr1, 0, 0, false);
    set_int_gate(2, (uint64_t) isr2, 0, 0, false);
    set_int_gate(3, (uint64_t) isr3, 0, 0, false);
    set_int_gate(4, (uint64_t) isr4, 0, 0, false);
    set_int_gate(5, (uint64_t) isr5, 0, 0, false);
    set_int_gate(6, (uint64_t) isr6, 0, 0, false);
    set_int_gate(7, (uint64_t) isr7, 0, 0, false);
    set_int_gate(8, (uint64_t) isr8, 0, 0, false);
    set_int_gate(9, (uint64_t) isr9, 0, 0, false);
    set_int_gate(10, (uint64_t) isr10, 0, 0, false);
    set_int_gate(11, (uint64_t) isr11, 0, 0, false);
    set_int_gate(12, (uint64_t) isr12, 0, 0, false);
    set_int_gate(13, (uint64_t) isr13, 0, 0, false);
    set_int_gate(14, (uint64_t) isr14, 0, 0, false);
    set_int_gate(15, (uint64_t) isr15, 0, 0, false);
    set_int_gate(16, (uint64_t) isr16, 0, 0, false);
    set_int_gate(17, (uint64_t) isr17, 0, 0, false);
    set_int_gate(18, (uint64_t) isr18, 0, 0, false);
    set_int_gate(19, (uint64_t) isr19, 0, 0, false);
    set_int_gate(20, (uint64_t) isr20, 0, 0, false);
    set_int_gate(21, (uint64_t) isr21, 0, 0, false);
    set_int_gate(22, (uint64_t) isr22, 0, 0, false);
    set_int_gate(23, (uint64_t) isr23, 0, 0, false);
    set_int_gate(24, (uint64_t) isr24, 0, 0, false);
    set_int_gate(25, (uint64_t) isr25, 0, 0, false);
    set_int_gate(26, (uint64_t) isr26, 0, 0, false);
    set_int_gate(27, (uint64_t) isr27, 0, 0, false);
    set_int_gate(28, (uint64_t) isr28, 0, 0, false);
    set_int_gate(29, (uint64_t) isr29, 0, 0, false);
    set_int_gate(30, (uint64_t) isr30, 0, 0, false);
    set_int_gate(31, (uint64_t) isr31, 0, 0, false);

    set_int_gate(128, (uint64_t) isr128, 0, 0, false); // system calls
    set_int_gate(177, (uint64_t) isr177, 0, 0, false); // system calls
}

void setupIRQ() {
    set_int_gate(32, (uint64_t) irq0, 0, 0, false);
    set_int_gate(33, (uint64_t) irq1, 0, 0, false);
    set_int_gate(34, (uint64_t) irq2, 0, 0, false);
    set_int_gate(35, (uint64_t) irq3, 0, 0, false);
    set_int_gate(36, (uint64_t) irq4, 0, 0, false);
    set_int_gate(37, (uint64_t) irq5, 0, 0, false);
    set_int_gate(38, (uint64_t) irq6, 0, 0, false);
    set_int_gate(39, (uint64_t) irq7, 0, 0, false);
    set_int_gate(40, (uint64_t) irq8, 0, 0, false);
    set_int_gate(41, (uint64_t) irq9, 0, 0, false);
    set_int_gate(42, (uint64_t) irq10, 0, 0, false);
    set_int_gate(43, (uint64_t) irq11, 0, 0, false);
    set_int_gate(44, (uint64_t) irq12, 0, 0, false);
    set_int_gate(45, (uint64_t) irq13, 0, 0, false);
    set_int_gate(46, (uint64_t) irq14, 0, 0, false);
    set_int_gate(47, (uint64_t) irq15, 0, 0, false);
}

void initIdt(){
    memset(&idt, 0, sizeof(idt));
    idt_pointer.base = (uint64_t) &idt;
    idt_pointer.limit = (uint16_t) sizeof(struct idt_entry) * 256 - 1;

    setupPic();
    //outPortB(0x21, 0xef); // mask out irq15 - repeatedly getting called for some reason, need to investigate
    setupGates();
    setupIRQ();
    load_idt(&idt_pointer);
    asm volatile("sti;"); // enable interrupts
}

void set_int_gate(uint8_t num, uint64_t addr, uint8_t ist, uint8_t dpl, bool user) {
    set_gate_offset(&idt[num], addr);
    idt[num].selector = 0x8 | (user ? 0x3 : 0);
    idt[num].ist = (ist & 0x8);
    idt[num].type_attributes = (0xe) | (1 << 7); // 64bit-interrupt | present
}

void set_trap_gate(uint8_t num, uint64_t addr, uint8_t ist, uint8_t dpl, bool user) {
    set_gate_offset(&idt[num], addr);
    idt[num].selector = 0x8 | (user ? 0x3 : 0);
    idt[num].ist = (ist & 0x8);
    idt[num].type_attributes = (0xf) | (1 << 7); // 64bit-trap | present
}

void store_idt(struct idt_ptr *idtr) {
    asm volatile ("sidt %0;" : "=m"(*idtr));
}

void load_idt(struct idt_ptr *idtr) {
    asm volatile ("lidt %0;" : : "m"(*idtr));
}

void set_gate_offset(struct idt_entry *gate, uint64_t addr) {
    gate->offset_low = addr & 0xFFFF;
    gate->offset_middle = (addr >> 16) & 0xFFFF;
    gate->offset_high = (addr >> 32) & 0xFFFFFFFF;
}

uint64_t get_gate_offset(struct idt_entry *gate) {
    uint64_t return_value = 0;
    uint64_t offset = 0;

    return_value = gate->offset_low;

    offset = gate->offset_middle;
    return_value |= (offset << 16);

    offset = gate->offset_high;
    return_value |= (offset << 32);

    return return_value;
}
