// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include "usb_host.h"
#include "usb_host_internal.h"

#ifndef DISK_BASE_BUF_LEN
// ===================================================================================
// NumLock lighting judgment
// Input:   key
// ===================================================================================
uint8_t SETorOFFNumLock(uint8_t *buf) {
  uint8_t tmp[]= {0x21,0x09,0x00,0x02,0x00,0x00,0x01,0x00};
  uint8_t len,s;
  if((buf[2]==0x53)&(buf[0]|buf[1]|buf[3]|buf[4]|buf[5]|buf[6]|buf[7]==0)) {
    if(HubLowSpeed) {                                 // Low-speed devices under HUB
      ((__xdata uint8_t *)pSetupReq)[0] = 0X21;					
      for(s=1; s!=sizeof(tmp)+1; s++)
        ((__xdata uint8_t *)pSetupReq)[s] = tmp[s];
    }
    else {
      for(s=0; s!=sizeof(tmp); s++)
        ((__xdata uint8_t *)pSetupReq)[s] = tmp[s];
    }
    s = HostCtrlTransfer(Com_Buffer, &len);           // execute control transfer
    if(s != ERR_SUCCESS) return(s);
  }
  return(ERR_SUCCESS);
}
#endif