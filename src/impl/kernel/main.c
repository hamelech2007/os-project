#include "print.h"
#include "gdt.h"

void kernel_main() {
    // print_clear();
    print_set_color(PRINT_COLOR_LIGHT_BLUE, PRINT_COLOR_BLACK);
    print_str("Hello, World from Kernel!");
    for(;;) ;
}