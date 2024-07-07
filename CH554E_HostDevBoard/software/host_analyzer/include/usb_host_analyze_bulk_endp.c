// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================


#include <string.h>
#include "usb_host.h"

#ifndef DISK_BASE_BUF_LEN

// ===================================================================================
// Analyze the batch endpoints, GpVar[0], GpVar[1] store the upload endpoints. 
// GpVar[2], GpVar[3] store the download endpoint
// buf：           data buffer address to be analyzed
// HubPortIndex：  0 means the root HUB, non-0 means the port number under the external HUB
// Return:        0
// ===================================================================================
uint8_t AnalyzeBulkEndp(__xdata uint8_t *buf, uint8_t HubPortIndex) {
  uint8_t i, s1,s2, l;
  s1 = 0; s2 = 2;
  if(HubPortIndex)
    memset(DevOnHubPort[HubPortIndex-1].GpVar,0,sizeof(DevOnHubPort[HubPortIndex-1].GpVar));          // empty array
  else
    memset(ThisUsbDev.GpVar,0,sizeof(ThisUsbDev.GpVar));  // empty array

  // Search interrupt endpoint descriptors, skip configuration descriptors and interface descriptors
  for(i=0; i<(uint8_t)(((PXUSB_CFG_DESCR)buf)->wTotalLength); i+=l) {
    if((((PXUSB_ENDP_DESCR)(buf+i))->bDescriptorType == USB_DESCR_TYP_ENDP)                           // endpoint descriptor?
      && ((((PXUSB_ENDP_DESCR)(buf+i))->bmAttributes & USB_ENDP_TYPE_MASK) == USB_ENDP_TYPE_BULK)) {  // bulk endpoint?

      if(HubPortIndex) {
        if(((PXUSB_ENDP_DESCR)(buf+i)) -> bEndpointAddress & USB_ENDP_DIR_MASK)
          DevOnHubPort[HubPortIndex-1].GpVar[s1++] = ((PXUSB_ENDP_DESCR)(buf+i)) -> bEndpointAddress & USB_ENDP_ADDR_MASK;
        else
          DevOnHubPort[HubPortIndex-1].GpVar[s2++] = ((PXUSB_ENDP_DESCR)(buf+i)) -> bEndpointAddress & USB_ENDP_ADDR_MASK;
      }
      else {
        if(((PXUSB_ENDP_DESCR)(buf+i)) -> bEndpointAddress & USB_ENDP_DIR_MASK)
          ThisUsbDev.GpVar[s1++] = ((PXUSB_ENDP_DESCR)(buf+i)) -> bEndpointAddress & USB_ENDP_ADDR_MASK;
        else
          ThisUsbDev.GpVar[s2++] = ((PXUSB_ENDP_DESCR)(buf+i)) -> bEndpointAddress & USB_ENDP_ADDR_MASK;
      }
      if(s1 == 2) s1 = 1;
      if(s2 == 4) s2 = 3;			
    }
    l = ((PXUSB_ENDP_DESCR)(buf+i)) -> bLength;           // current descriptor length, skip
    if(l > 16) break;
  }
  return(0);
}
#endif