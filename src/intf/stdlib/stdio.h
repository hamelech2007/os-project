#pragma once
#include "stdint.h"
void printf(const char* fmt, ...);

void printf_number(uint64_t value, int length, bool sign, int radix);