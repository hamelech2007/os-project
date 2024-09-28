#include "print.h"
#include "gdt.h"
#include "stdio.h"
#include "idt.h"

extern void error();
extern void kernel_main();
void entry_64() {
    //error();
    initGdt(); // initialize global descriptor table
    initIdt(); // initialize interrupt descriptor table
    

    print_set_color(PRINT_COLOR_YELLOW, PRINT_COLOR_BLACK);
    print_str("Gdt and Idt are both setup!\n");

    kernel_main();
}