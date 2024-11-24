#pragma once
#include "stdint.h"

// Function to execute the CPUID instruction
inline void cpuid(uint32_t function_id, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx);