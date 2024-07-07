// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include "usb_host.h"
#include "usb_host_internal.h"

#ifndef DISK_BASE_BUF_LEN

// ===================================================================================
// Query the status of the HUB port and return it in TxBuffer
// Return ERR_SUCCESS:      success
//        ERR_USB_BUF_OVER: wrong length
// ===================================================================================
uint8_t HubGetPortStatus(uint8_t HubPortIndex) {
  uint8_t s;
  uint8_t len;
  pSetupReq -> bRequestType = HUB_GET_PORT_STATUS;
  pSetupReq -> bRequest = HUB_GET_STATUS;
  pSetupReq -> wValueL = 0x00;
  pSetupReq -> wValueH = 0x00;
  pSetupReq -> wIndexL = HubPortIndex;
  pSetupReq -> wIndexH = 0x00;
  pSetupReq -> wLengthL = 0x04;
  pSetupReq -> wLengthH = 0x00;
  s = HostCtrlTransfer(Com_Buffer, (uint8_t *)&len);  // execute control transfer
  if(s != ERR_SUCCESS) return(s);
  if(len < 4) return(ERR_USB_BUF_OVER);               // wrong descriptor length
  return(ERR_SUCCESS);
}

#endif