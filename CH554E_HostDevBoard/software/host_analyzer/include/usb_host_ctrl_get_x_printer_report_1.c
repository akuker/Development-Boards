// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================


#include "usb_host.h"



#ifndef DISK_BASE_BUF_LEN

// ===================================================================================
// Printer class commands
// Return ERR_USB_BUF_OVER: wrong descriptor length
//        ERR_SUCCESS:      success
// ===================================================================================
uint8_t CtrlGetXPrinterReport1(void) {
  uint8_t s;
  uint16_t len;
  CopySetupReqPkg(XPrinterReport);
  s = HostCtrlTransfer(Com_Buffer, (uint8_t *)&len);  // execute control transfer
  if(s != ERR_SUCCESS) return(s);
  //if(len < ((XPrinterReport[7]<<8) | (XPrinterReport[6])))
  //  return(ERR_USB_BUF_OVER);                       // wrong descriptor length
  return(ERR_SUCCESS);
}
#endif