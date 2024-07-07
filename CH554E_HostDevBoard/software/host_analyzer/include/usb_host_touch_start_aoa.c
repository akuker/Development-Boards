// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include <string.h>
#include "usb_host.h"
#include "usb_host_internal.h"

#ifndef DISK_BASE_BUF_LEN
// ===================================================================================
// Try to start AOA mode
// ===================================================================================
uint8_t TouchStartAOA(void) {
  uint8_t len,s,i,Num;
  uint16_t cp_len;
  CopySetupReqPkg(GetProtocol);                       // get protocol version number
  s = HostCtrlTransfer(Com_Buffer, &len);             // execute control transfer
  if(s != ERR_SUCCESS) return(s);
  if(Com_Buffer[0] <2 ) return ERR_AOA_PROTOCOL;
  for(i=0; i<6; i++) {                                // output string
    Num=Sendlen[i];
    CopySetupReqPkg(&SetStringID[8*i]);
    cp_len = (pSetupReq -> wLengthH << 8) | (pSetupReq -> wLengthL);
    memcpy(Com_Buffer, &StringID[Num], cp_len);
    s = HostCtrlTransfer(Com_Buffer, &len);           // execute control transfer
    if(s != ERR_SUCCESS) return(s);
  }

  CopySetupReqPkg(TouchAOAMode);
  s = HostCtrlTransfer(Com_Buffer, &len);             // execute control transfer
  if(s != ERR_SUCCESS) return(s);
  return ERR_SUCCESS;
}
#endif