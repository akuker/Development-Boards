// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================


#include "usb_host.h"


#ifndef DISK_BASE_BUF_LEN



// ===================================================================================
// Get the HUB descriptor and return it in TxBuffer
// Return ERR_SUCCESS:      success
//        ERR_USB_BUF_OVER: wrong length
// ===================================================================================
uint8_t CtrlGetHubDescr(void) {
  uint8_t s;
  uint8_t len;
  CopySetupReqPkg(SetupGetHubDescr);
  s = HostCtrlTransfer(Com_Buffer, (uint8_t *)&len);  // execute control transfer
  if(s != ERR_SUCCESS) return(s);
  if(len < ((PUSB_SETUP_REQ)SetupGetHubDescr) -> wLengthL)
    return(ERR_USB_BUF_OVER);                         // wrong descriptor length
  //if(len < 4) return(ERR_USB_BUF_OVER);
  return(ERR_SUCCESS);
}
#endif