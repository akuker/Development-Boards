// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include <delay.h>
#include <string.h>
#include "usb_host.h"

// ===================================================================================
// After detecting the device, reset the bus, prepare for enumerating the device, set 
// the default to full speed.
// ===================================================================================
void ResetRootHubPort(void) {
  UsbDevEndp0Size = DEFAULT_ENDP0_SIZE;       // maximum packet size for endpoint 0 of a USB device
  #ifndef DISK_BASE_BUF_LEN	
  memset(&ThisUsbDev,0,sizeof(ThisUsbDev));   // empty structure
  #endif
  SetHostUsbAddr(0x00);
  UHOST_CTRL &= ~bUH_PORT_EN;                 // turn off the port
  SetUsbSpeed(1);                             // default is full speed
  UHOST_CTRL = UHOST_CTRL & ~bUH_LOW_SPEED | bUH_BUS_RESET;  // start reset
  DLY_ms(20);                                 // reset time 10mS to 20mS
  UHOST_CTRL = UHOST_CTRL & ~bUH_BUS_RESET;   // end reset
  DLY_us(250);
  UIF_DETECT = 0;                             // clear interrupt flag
}