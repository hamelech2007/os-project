#pragma once
#include "stdint.h"

void initialize_PS2_mouse();
void send_mouse_packet(uint8_t packet);
void mouse_update();