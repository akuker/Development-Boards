// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include "usb_host.h"

// ===================================================================================
// Set the address of the USB device currently operated by the USB host
// ===================================================================================
void SetHostUsbAddr(uint8_t addr) {
  USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | addr & 0x7F;
}

// ===================================================================================
// Set the current USB speed
// ===================================================================================
#ifndef	FOR_ROOT_UDISK_ONLY
void SetUsbSpeed(uint8_t FullSpeed) {
  if(FullSpeed) {                       // full speed
    USB_CTRL &= ~bUC_LOW_SPEED;         // set full speed
    UH_SETUP &= ~bUH_PRE_PID_EN;        // disable PRE PID
  }
  else {
    USB_CTRL |=  bUC_LOW_SPEED;         // set low speed
  }
}
#endif