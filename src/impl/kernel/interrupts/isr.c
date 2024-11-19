#include "isr.h"
#include "util.h"
#include "stdio.h"
#include "print.h"
#include "keyboard.h"
#include "mouse.h"

void pic_EOI() {
    out_port_b(0x20, 0x20);
}

void slave_pic_EOI() {
    out_port_b(0xa0, 0x20);
    pic_EOI();
}

char* interrupt_messages[] = {
    "Divide By Zero",                   // 0    Fault       No Error Code
    "Debug",                            // 1    Fault/Trap  No Error Code
    "NMI",                              // 2    Interrupt   No Error Code
    "Breakpoint",                       // 3    Trap        No Error Code
    "Into Detected Overflow",           // 4    Trap        No Error Code
    "Out of Bounds",                    // 5    Fault       No Error Code
    "Invalid Opcode",                   // 6    Fault       No Error Code
    "No Coprocessor",                   // 7    Fault       No Error Code
    "Double Fault",                     // 8    Abort       Error Code Present (0)
    "Coprocessor Segment Overrun",      // 9    Fault       No Error Code   
    "Bad TSS",                          // 10   Fault       Error Code Present
    "Segment not Present",              // 11   Fault       Error Code Present
    "Stack Fault",                      // 12   Fault       Error Code Present
    "General Protection Fault",         // 13   Fault       Error Code Present
    "Page Fault",                       // 14   Fault       Error Code Present
    "Unknown Interrupt",                // 15   
    "Coprocessor Fault",                // 16   Fault       No Error Code
    "Alignment Fault",                  // 17   Fault       Error Code Present
    "Machine Check",                    // 18
    "SIMD Floating-Point Exception",    // 19  
    "Virtualization Exception",         // 20
    "Control Protection Exception",     // 21
    "Reserved",                         // 22
    "Reserved",                         // 23
    "Reserved",                         // 24
    "Reserved",                         // 25
    "Reserved",                         // 26
    "Reserved",                         // 27
    "Hypervisor Injection Exception",   // 28
    "VMM Communication Exception",      // 29
    "Security Exception",               // 30
    "Reserved"                          // 31
};

void (*isr_routines[])(struct int_regs*) = {
    divide_by_zero,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    invalid_opcode,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    general_protection,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved
};
void (*irq_routines[16])(struct int_regs*) = {
    timer, keyboard_press, unimplemented, unimplemented, unimplemented, unimplemented, unimplemented, unimplemented,
    unimplemented, unimplemented, unimplemented, unimplemented, mouse_input, unimplemented, unimplemented, unimplemented
};


void isr_handler(struct int_regs* regs) {
    isr_routines[regs->int_num](regs);
}

void irq_handler(struct int_regs* regs) {
    irq_routines[regs->int_num - 32](regs); // starts at 32

    if(regs->int_num >= 40) 
        slave_pic_EOI();
    else
        pic_EOI();
}

void unimplemented(struct int_regs* regs) {
    printf("Unimplemented Interrupt Recieved! Number: %d, Error Code: %d\n", regs->int_num, regs->err_code);
}


void divide_by_zero(struct int_regs* regs) {
    print_set_color(PRINT_COLOR_RED, PRINT_COLOR_WHITE);
    print_str("Divide by zero recieved!\n");
    panic();
}

void invalid_opcode(struct int_regs* regs) {
    print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_RED);
    printf("\nInvalid opcode recieved at address: 0x%p\n4 bytes following RIP: 0x%x 0x%x 0x%x 0x%x",
     regs->rip, *(uint8_t*)regs->rip, *(uint8_t*)(regs->rip + 1), *(uint8_t*)(regs->rip + 2), *(uint8_t*)(regs->rip + 3));
    for(;;);
}

void general_protection(struct int_regs* regs) {
    printf("General Protection Fault! Error code: 0x%x\n", regs->err_code);
}

void reserved(struct int_regs* regs) {
    printf("Reserved interrupt number: %d", regs->int_num);
    panic();
}




extern uint64_t startup_timer;
void timer(struct int_regs* regs) {
    startup_timer++;
}

void keyboard_press(struct int_regs* regs){
    handle_keyboard_input(in_port_b(0x60));
}

void mouse_input(struct int_regs* regs){
    send_mouse_packet(in_port_b(0x60));
}