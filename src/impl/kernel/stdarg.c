#include "stdarg.h"

#define RDI 0
#define RSI 1
#define RDX 2
#define RCX 3
#define R8  4
#define R9  5

// note: this function doesn't work, it doesn't take into account the fact that rdi and rsi get overriden because of it...
uint64_t getArgument(uint8_t num, uint64_t* argp) {
    if(num <= 5) {
        // read from regs
        uint64_t value;
        switch(num) {
            case RDI:
                asm volatile("movq %%rdi, %0" : "=r"(value) : :);
                break;
            case RSI:
                asm volatile("movq %%rsi, %0" : "=r"(value) : :);
                break;
            case RDX:
                asm volatile("movq %%rdx, %0" : "=r"(value) : :);
                break;
            case RCX:
                asm volatile("movq %%rcx, %0" : "=r"(value) : :);
                break;
            case R8:
                asm volatile("movq %%r8, %0" : "=r"(value) : :);
                break;
            case R9:
                asm volatile("movq %%r9, %0" : "=r"(value) : :);
                break;
        }
        return value;
    } else {
        num-=7; // 7th param is the 1st pushed to stack
        return *(argp+num);
    }
}