#pragma once
#include "stdint.h"

void panic();
void out_port_b(uint16_t port, uint8_t value);

uint8_t in_port_b(uint16_t port);