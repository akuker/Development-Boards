// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include "usb_host.h"

#ifndef DISK_BASE_BUF_LEN
// ===================================================================================
// Enumerate USB devices of all ROOT-HUB ports
// ===================================================================================
uint8_t EnumAllRootDevice(void) {
  __idata uint8_t s;
  #if DEBUG_ENABLE
  printf("EnumUSBDev\n");
  #endif
  if(ThisUsbDev.DeviceStatus == ROOT_DEV_CONNECTED) { // device has just been plugged in and has not been initialized
    s = InitRootDevice();                             // initialize/enumerate USB devices of specified HUB port
    if(s != ERR_SUCCESS) return(s);
  }
  return(ERR_SUCCESS);
}
#endif