// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include "usb_host.h"


// ===================================================================================
// Get the device descriptor and return it in TxBuffer
// Return ERR_USB_BUF_OVER: wrong descriptor length
//        ERR_SUCCESS:      success
// ===================================================================================
uint8_t CtrlGetDeviceDescr(void) {
  uint8_t s;
  uint8_t len;
  UsbDevEndp0Size = DEFAULT_ENDP0_SIZE;
  CopySetupReqPkg(SetupGetDevDescr);
  s = HostCtrlTransfer(Com_Buffer, (uint8_t*)&len);   // execute control transfer
  if(s != ERR_SUCCESS) return(s);

  // Endpoint 0 maximum packet length, this is simplified processing, normally you 
  // should first obtain the first 8 bytes and update UsbDevEndp0Size immediately 
  // before continuing
  UsbDevEndp0Size = ((PXUSB_DEV_DESCR)Com_Buffer) -> bMaxPacketSize0;
  if(len < ((PUSB_SETUP_REQ)SetupGetDevDescr) -> wLengthL) return(ERR_USB_BUF_OVER);  // wrong descriptor length
  return(ERR_SUCCESS);
}
