// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include "usb_host.h"

// ===================================================================================
// Wait for USB interrupt
// Return ERR_SUCCESS:     data received or sent successfully
//        ERR_USB_UNKNOWN: failed to receive or send data
// ===================================================================================
uint8_t WaitUSB_Interrupt(void) {
  uint16_t  i;
  for(i = WAIT_USB_TOUT_200US; i != 0 && UIF_TRANSFER == 0; i -- ){;}
  return(UIF_TRANSFER ? ERR_SUCCESS : ERR_USB_UNKNOWN);
}