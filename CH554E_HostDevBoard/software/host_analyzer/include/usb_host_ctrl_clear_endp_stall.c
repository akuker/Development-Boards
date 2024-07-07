// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include "usb_host.h"
#include "usb_host_internal.h"
#include <string.h>


// ===================================================================================
// Clear endpoint STALL
// Return ERR_SUCCESS:      success
// ===================================================================================
uint8_t CtrlClearEndpStall(uint8_t endp) {
  CopySetupReqPkg(SetupClrEndpStall);                 // clear endpoint errors
  if(HubLowSpeed) pSetupReq -> wIndexH = endp;        // endpoint address
  else            pSetupReq -> wIndexL = endp;
  return(HostCtrlTransfer(NULL, NULL));               // execute control transfer
}