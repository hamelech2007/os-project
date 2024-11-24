#include "util.h"
#include "print.h"


void panic() {
    print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_RED);
    print_str("\n------------------------------------------\nKERNEL PANIC\n------------------------------------------");
    asm volatile ("hlt");
    for(;;);
}

void out_port_b(uint16_t port, uint8_t value) {
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

void out_port_l(uint16_t port, uint32_t value) {
    asm volatile ("outl %1, %0" : : "dN" (port), "a" (value));
}


uint8_t in_port_b(uint16_t port) {
    uint8_t value;

    // Inline assembly to read a byte from the specified port
    __asm__ volatile (
        "inb %1, %0"       // Read from the specified port into the value variable
        : "=a" (value)     // Output operand: store the read byte in `value` (AL)
        : "Nd" (port)      // Input operand: the specified port (16-bit)
    );

    return value;
}

uint32_t in_port_l(uint16_t port) {
    uint32_t value;

    // Inline assembly to read a byte from the specified port
    __asm__ volatile (
        "inl %1, %0"       // Read from the specified port into the value variable
        : "=a" (value)     // Output operand: store the read byte in `value` (AL)
        : "Nd" (port)      // Input operand: the specified port (16-bit)
    );

    return value;
}


uint8_t* incptr(void* ptr, uint64_t size)  {
    return (uint8_t*)ptr + size;
}