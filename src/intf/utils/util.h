#pragma once
#include "stdint.h"

void panic();
void outPortB(uint16_t port, uint8_t value);

uint8_t inPortB(uint16_t port);