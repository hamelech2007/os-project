#include "isr.h"
#include "util.h"
#include "stdio.h"
#include "print.h"

void picEOI() {
    outPortB(0x20, 0x20);
}

void slavePicEOI() {
    outPortB(0xa0, 0x20);
    picEOI();
}

char* interrupt_messages[] = {
    "Divide By Zero",
    "Debug",
    "NMI",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Fault",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Security Exception",
    "Reserved"
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
    unimplemented, unimplemented, unimplemented, unimplemented, unimplemented, unimplemented, unimplemented, unimplemented
};


void isr_handler(struct int_regs* regs) {
    isr_routines[regs->int_num](regs);
}

void irq_handler(struct int_regs* regs) {
    irq_routines[regs->int_num - 32](regs); // starts at 32

    if(regs->int_num >= 40) 
        slavePicEOI();
    else
        picEOI();
}

void unimplemented(struct int_regs* regs) {
    if(regs->int_num == 47) return;
    print_str("Unimplemented Exception Recieved! Number: ");
    print_int(regs->int_num);
    print_char('\n');
}


void divide_by_zero(struct int_regs* regs) {
    print_set_color(PRINT_COLOR_RED, PRINT_COLOR_WHITE);
    print_str("Divide by zero recieved!\n");
    regs->rip++;
}

void invalid_opcode(struct int_regs* regs) {
    print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_RED);
    print_str("Invalid opcode recieved!\n");
}

void general_protection(struct int_regs* regs) {
    print_str("General Protection Fault! Error code: \n");
    print_int(regs->err_code);
}

void reserved(struct int_regs* regs) {
    print_str("Interrupt Recieved! Interrupt Number: ");
    print_int((uint8_t) regs->int_num);
    print_str(" Description: ");
    print_str(interrupt_messages[regs->int_num]);
    print_char('\n');
}




extern uint64_t startup_timer;
void timer(struct int_regs* regs) {
    startup_timer++;
}

void keyboard_press(struct int_regs* regs){
    // use different colors each time to see easily
    static uint8_t color = PRINT_COLOR_LIGHT_RED;
    color++;
    color%=16;
    if(color == 0) color++;
    print_set_color(color, PRINT_COLOR_BLACK);
    // color end
    print_str("Key pressed!\n");

    uint8_t scancode = inPortB(0x60);
    print_str("Scancode: ");
    print_int(scancode);
    print_char('\n');

}