// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include <string.h>
#include "usb_host.h"


#ifndef DISK_BASE_BUF_LEN

// ===================================================================================
// Analyze the address of the HID interrupt endpoint from the descriptor, if the 
// HubPortIndex is 0, save it to ROOTHUB, if it is a non-zero value, save it to 
// the structure under the HUB.
// buf：           data buffer address to be analyzed
// HubPortIndex：  0 means the root HUB, non-0 means the port number under the external HUB
// Return:        endpoints
// ===================================================================================
uint8_t AnalyzeHidIntEndp(__xdata uint8_t *buf, uint8_t HubPortIndex) {
  uint8_t i, s, l;
  s = 0;
  if(HubPortIndex)
    memset(DevOnHubPort[HubPortIndex-1].GpVar,0,sizeof(DevOnHubPort[HubPortIndex-1].GpVar));  // empty array
  else
    memset(ThisUsbDev.GpVar,0,sizeof(ThisUsbDev.GpVar));    // empty array

  // Search interrupt endpoint descriptors, skip configuration descriptors and interface descriptors
  for(i=0; i<(uint8_t)(((PXUSB_CFG_DESCR)buf)->wTotalLength); i+=l) {
    if(((PXUSB_ENDP_DESCR)(buf+i))->bDescriptorType == USB_DESCR_TYP_ENDP                       // enpoint descriptor?
      &&(((PXUSB_ENDP_DESCR)(buf+i))->bmAttributes & USB_ENDP_TYPE_MASK) == USB_ENDP_TYPE_INTER // interrupt endpoint?
      &&(((PXUSB_ENDP_DESCR)(buf+i))->bEndpointAddress & USB_ENDP_DIR_MASK)) {                  // IN endpoint?

      // Save address of interrupt endpoint, bit 7 is used for synchronization flag bit, cleared to 0
      if(HubPortIndex)
        DevOnHubPort[HubPortIndex-1].GpVar[s] = ((PXUSB_ENDP_DESCR)(buf+i))->bEndpointAddress & USB_ENDP_ADDR_MASK;
      else
        ThisUsbDev.GpVar[s] = ((PXUSB_ENDP_DESCR)(buf+i))->bEndpointAddress & USB_ENDP_ADDR_MASK;                                                        
      #if DEBUG_ENABLE	
      printf("Endpoint: %02x ",(uint16_t)ThisUsbDev.GpVar[s]);
      #endif
      s++;
      if(s >= 4) break;                               // only 4 endpoints are analyzed
    }
    l = ((PXUSB_ENDP_DESCR)(buf+i)) -> bLength;       // current descriptor length, skip
    if(l > 16) break;
  }
  #if DEBUG_ENABLE
  printf("\n");
  #endif
  return(s);
}
#endif