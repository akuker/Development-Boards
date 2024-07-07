// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include <debug.h>
#include <delay.h>
#include <stdio.h>
#include <string.h>
#include "usb_host.h"

#pragma disable_warning 84
#pragma disable_warning 110


__bit HubLowSpeed;

// Define user temporary buffers, used to process descriptors during enumeration, and 
// can also be used as ordinary temporary buffers at the end of enumeration.
__xdata uint8_t Com_Buffer[COM_BUF_SIZE];









