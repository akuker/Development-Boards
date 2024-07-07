// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include "usb_host.h"


// ===================================================================================
// Enable the ROOT-HUB port, set the corresponding bUH_PORT_EN to 1 to open the port, 
// and the disconnection of the device may cause the return failure.
// Return ERR_SUCCESS for a new connection detected, return ERR_USB_DISCON for no 
// connection.
// ===================================================================================
uint8_t EnableRootHubPort(void) {
  #ifdef DISK_BASE_BUF_LEN
  if(CH554DiskStatus < DISK_CONNECT) CH554DiskStatus = DISK_CONNECT;
  #else
  if(ThisUsbDev.DeviceStatus < ROOT_DEV_CONNECTED) ThisUsbDev.DeviceStatus = ROOT_DEV_CONNECTED;
  #endif
  if(USB_MIS_ST & bUMS_DEV_ATTACH) {                                // have device
    #ifndef DISK_BASE_BUF_LEN
    if((UHOST_CTRL & bUH_PORT_EN) == 0x00 ) {                       // not yet enabled
      ThisUsbDev.DeviceSpeed = USB_MIS_ST & bUMS_DM_LEVEL ? 0 : 1;
      if(ThisUsbDev.DeviceSpeed == 0) UHOST_CTRL |= bUH_LOW_SPEED;  // low speed
    }
    #endif
    USB_CTRL |= bUC_DMA_EN;       // start USB host and DMA, automatically suspend before interrupt flag is cleared
    UH_SETUP = bUH_SOF_EN;		
    UHOST_CTRL |= bUH_PORT_EN;    // enable HUB port
    return(ERR_SUCCESS);
  }
  return(ERR_USB_DISCON);
}
