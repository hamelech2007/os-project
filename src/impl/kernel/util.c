#include "util.h"

void outPortB(uint16_t port, uint8_t value) {
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}



uint8_t inPortB(uint16_t port) {
    uint8_t value;

    // Inline assembly to read a byte from the specified port
    __asm__ volatile (
        "inb %1, %0"       // Read from the specified port into the value variable
        : "=a" (value)     // Output operand: store the read byte in `value` (AL)
        : "Nd" (port)      // Input operand: the specified port (16-bit)
    );

    return value;
}