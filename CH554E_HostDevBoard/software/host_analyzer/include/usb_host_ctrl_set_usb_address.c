// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include <delay.h>
#include <string.h>
#include "usb_host.h"
#include "usb_host_internal.h"


// ===================================================================================
// Set USB device address
// Return ERR_SUCCESS:      success
// ===================================================================================
uint8_t CtrlSetUsbAddress(uint8_t addr) {
  uint8_t s;
  CopySetupReqPkg(SetupSetUsbAddr);
  if(HubLowSpeed) pSetupReq -> wValueH = addr;        // USB device address
  else            pSetupReq -> wValueL = addr;
  s = HostCtrlTransfer(NULL, NULL);                   // execute control transfer
  if(s != ERR_SUCCESS) return(s);
  SetHostUsbAddr(addr);     // set address of USB device currently operated by USB host
  DLY_ms(10);               // wait for USB device to complete operation
  return(ERR_SUCCESS);
}