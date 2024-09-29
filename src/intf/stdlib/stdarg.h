#pragma once

#include "stdint.h"


uint64_t getArgument(uint8_t num, uint64_t* argp);

// Macro to get a variadic argument based on its position
#define GET_VA_ARG(n) \
    ({ \
        __typeof__(n) __result; \
        if (n == 1) { \
            asm volatile("movq %%rdi, %0" : "=r" (__result)); \
        } else if (n == 2) { \
            asm volatile("movq %%rsi, %0" : "=r" (__result)); \
        } else if (n == 3) { \
            asm volatile("movq %%rdx, %0" : "=r" (__result)); \
        } else if (n == 4) { \
            asm volatile("movq %%rcx, %0" : "=r" (__result)); \
        } else if (n == 5) { \
            asm volatile("movq %%r8, %0" : "=r" (__result)); \
        } else if (n == 6) { \
            asm volatile("movq %%r9, %0" : "=r" (__result)); \
        } else { \
            asm volatile("movq %%rbp, %%rax;" \
                         "addq $16, %0;" \
                         "leaq (%0, %1, 8), %%rax;"  /* Load address of the nth argument */ \
                         "movq (%%rax), %0;"  /* Move the argument into result */ \
                         : "=r" (__result) \
                         : "r" (n) \
                         : "%rax"); \
        } \
        __result; \
    })