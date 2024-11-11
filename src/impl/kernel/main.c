#include "print.h"
#include "gdt.h"
#include "stdint.h"

uint64_t startup_timer = 0;

void kernel_main() {
    // print_clear();
    print_set_color(PRINT_COLOR_LIGHT_BLUE, PRINT_COLOR_BLACK);
    print_str("Welcome to os!\n");
    print_prefix();
    disable_deletion();

    //int i = 0/0;
    for(;;) ;
}