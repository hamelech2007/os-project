#pragma once
#include "stdint.h"

void initializePS2Mouse();
void sendMousePacket(uint8_t packet);
void mouseUpdate();