// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include <string.h>
#include "usb_host.h"
#include "usb_host_internal.h"

#ifndef DISK_BASE_BUF_LEN
// ===================================================================================
// Set the USB device interface
// Return ERR_SUCCESS:      success
// ===================================================================================
uint8_t CtrlSetUsbIntercace(uint8_t cfg) {
  CopySetupReqPkg(SetupSetUsbInterface);
  if(HubLowSpeed) pSetupReq -> wValueH = cfg;         // USB device configuration
  else            pSetupReq -> wValueL = cfg;
  return(HostCtrlTransfer(NULL, NULL));               // execute control transfer
}
#endif