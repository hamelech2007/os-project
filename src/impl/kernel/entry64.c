#include "print.h"
#include "gdt.h"
#include "stdio.h"
#include "idt.h"
#include "util.h"
#include "drivers.h"
#include "multiboot.h"

extern void error();
extern void kernel_main();

extern void* page_table_l2;


extern struct pdt_entry *PDT;

void entry_64(multiboot2_info_t* bootInfo) {
    initGdt(); // initialize global descriptor table
    initIdt(); // initialize interrupt descriptor table
    initializeDrivers(); // initialize drivers

    print_set_color(PRINT_COLOR_YELLOW, PRINT_COLOR_BLACK);
    print_str("Gdt and Idt are both setup!\n");


    kernel_main();
}