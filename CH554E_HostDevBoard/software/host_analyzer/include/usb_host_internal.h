// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#pragma once

#include <stdint.h>

extern __xdata __at (0x0000) uint8_t RxBuffer[MAX_PACKET_SIZE];
extern __xdata __at (0x0040) uint8_t TxBuffer[MAX_PACKET_SIZE];
