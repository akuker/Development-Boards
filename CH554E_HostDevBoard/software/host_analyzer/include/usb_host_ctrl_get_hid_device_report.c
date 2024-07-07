// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include "usb_host.h"
#include "usb_host_internal.h"

#ifndef DISK_BASE_BUF_LEN


// ===================================================================================
// Get the HID device report descriptor and return it in TxBuffer
// Return ERR_SUCCESS:      success
// ===================================================================================
uint8_t CtrlGetHIDDeviceReport(uint8_t infc) {
  uint8_t s;
  uint8_t len;
  CopySetupReqPkg(SetupSetHIDIdle);
  if(HubLowSpeed) TxBuffer[5] = infc;
  else            TxBuffer[4] = infc;
  s = HostCtrlTransfer(Com_Buffer, (uint8_t *)&len);  // execute control transfer
  if(s != ERR_SUCCESS) return(s);
  CopySetupReqPkg(SetupGetHIDDevReport);
  if(HubLowSpeed) TxBuffer[5] = infc;
  else            TxBuffer[4] = infc;
  s = HostCtrlTransfer(Com_Buffer, (uint8_t *)&len);  // execute control transfer
  if(s != ERR_SUCCESS) return(s);
  return(ERR_SUCCESS);
}
#endif