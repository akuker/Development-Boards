// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include <string.h>
#include "usb_host.h"
#include "usb_host_internal.h"

// ===================================================================================
// Set USB device configuration
// Return ERR_SUCCESS:      success
// ===================================================================================
uint8_t CtrlSetUsbConfig(uint8_t cfg) {
  CopySetupReqPkg(SetupSetUsbConfig);
  if(HubLowSpeed) pSetupReq -> wValueH = cfg;         // USB device configuration
  else            pSetupReq -> wValueL = cfg;
  return(HostCtrlTransfer(NULL, NULL));               // execute control transfer
}
