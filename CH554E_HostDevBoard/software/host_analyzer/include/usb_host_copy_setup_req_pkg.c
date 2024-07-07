// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include "usb_host.h"
#include "usb_host_internal.h"

// ===================================================================================
// Copy control transfer request packet
// ===================================================================================
void CopySetupReqPkg(__code uint8_t *pReqPkt) {
  uint8_t i;
  if(HubLowSpeed) {                         // low-speed devices under the HUB
    ((__xdata uint8_t*)pSetupReq)[0] = *pReqPkt;			
    for(i=1; i!=sizeof(USB_SETUP_REQ)+1; i++) {
      ((__xdata uint8_t*)pSetupReq)[i] = *pReqPkt;
      pReqPkt++;
    }
  }
  if(HubLowSpeed == 0) {
    for(i=0; i!=sizeof(USB_SETUP_REQ); i++) {
      ((__xdata uint8_t*)pSetupReq)[i] = *pReqPkt;
      pReqPkt++;
    }			
  }
}
