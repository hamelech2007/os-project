#pragma once
#include "stdint.h"

void panic();
void out_port_b(uint16_t port, uint8_t value);
void out_port_l(uint16_t port, uint32_t value);

uint8_t in_port_b(uint16_t port);
uint32_t in_port_l(uint16_t port);

uint8_t* incptr(void* ptr, uint64_t size);