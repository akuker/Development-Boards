// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include "usb_host.h"
#include "usb_host_internal.h"


// ===================================================================================
// Get the configuration descriptor and return it in TxBuffer
// Return ERR_USB_BUF_OVER: wrong descriptor length
//        ERR_SUCCESS:      success
// ===================================================================================
uint8_t CtrlGetConfigDescr(void) {
  uint8_t s,len;
  CopySetupReqPkg(SetupGetCfgDescr);
  s = HostCtrlTransfer(Com_Buffer, (uint8_t *)&len);  // execute control transfer
  if(s != ERR_SUCCESS) return(s);
  len = (uint8_t)(((PXUSB_CFG_DESCR)Com_Buffer)->wTotalLength);
  CopySetupReqPkg(SetupGetCfgDescr);
  if(HubLowSpeed) pSetupReq -> wLengthH = len;        // total length of configuration descriptor 
  else            pSetupReq -> wLengthL = len;
  s = HostCtrlTransfer(Com_Buffer, (uint8_t *)&len);  // execute control transfer
  if(s != ERR_SUCCESS) return(s);
  #ifdef DISK_BASE_BUF_LEN
  if(len>64) len = 64;
  for(s=0; s!=len; s++) TxBuffer[s]=Com_Buffer[s];    // when using U-disk, it needs to be copied to TxBuffer
  #endif
  return(ERR_SUCCESS);
}