// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include <string.h>
#include "usb_host.h"
#include "usb_host_internal.h"

#ifndef DISK_BASE_BUF_LEN

// ===================================================================================
// Set HUB port characteristics
// Return ERR_SUCCESS:      success
// ===================================================================================
uint8_t HubSetPortFeature(uint8_t HubPortIndex, uint8_t FeatureSelt) {
  pSetupReq -> bRequestType = HUB_SET_PORT_FEATURE;
  pSetupReq -> bRequest = HUB_SET_FEATURE;
  pSetupReq -> wValueL = FeatureSelt;
  pSetupReq -> wValueH = 0x00;
  pSetupReq -> wIndexL = HubPortIndex;
  pSetupReq -> wIndexH = 0x00;
  pSetupReq -> wLengthL = 0x00;
  pSetupReq -> wLengthH = 0x00;
  return(HostCtrlTransfer(NULL, NULL));               // execute control transfer
}
#endif