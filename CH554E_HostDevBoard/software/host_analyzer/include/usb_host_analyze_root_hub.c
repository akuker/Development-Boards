// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include <stdio.h>
#include <string.h>
#include "usb_host.h"


// ===================================================================================
// Analyze the status of ROOT-HUB and handle the event of device plugging and un-
// plugging on the ROOT-HUB port. If the device is pulled out, the DisableRootHubPort()
// function is called in the function to close the port, insert an event, and set the 
// status bit of the corresponding port.
// Return ERR_SUCCESS for no case, return ERR_USB_CONNECT for detected new connection, 
// return ERR_USB_DISCON for detected disconnection.
// ===================================================================================
uint8_t AnalyzeRootHub(void) { 
  uint8_t s;
  s = ERR_SUCCESS;
  if(USB_MIS_ST & bUMS_DEV_ATTACH) {                  // device exists
    #ifdef DISK_BASE_BUF_LEN
    if(CH554DiskStatus == DISK_DISCONNECT
    #else
    if(ThisUsbDev.DeviceStatus == ROOT_DEV_DISCONNECT // device plugged in detected
    #endif
    || (UHOST_CTRL & bUH_PORT_EN) == 0x00 ) {
      // It is detected that a device is plugged in, but it has not been allowed, 
      // indicating that it has just been plugged in.
      DisableRootHubPort();                           // close port
      #ifdef DISK_BASE_BUF_LEN
      CH554DiskStatus = DISK_CONNECT;
      #else
      //ThisUsbDev.DeviceSpeed = USB_HUB_ST & bUHS_DM_LEVEL ? 0 : 1;
      ThisUsbDev.DeviceStatus = ROOT_DEV_CONNECTED;   // set connection flag
      #endif
      #if DEBUG_ENABLE
      printf("USB dev in\n");
      #endif
      s = ERR_USB_CONNECT;
    }
  }
  #ifdef DISK_BASE_BUF_LEN
  else if(CH554DiskStatus >= DISK_CONNECT) {
  #else
  else if(ThisUsbDev.DeviceStatus >= ROOT_DEV_CONNECTED) {  // device unplugged detected
  #endif
    DisableRootHubPort();                             // close port
    #if DEBUG_ENABLE		
    printf("USB dev out\n");
    #endif
    if(s == ERR_SUCCESS) s = ERR_USB_DISCON;
  }
  //UIF_DETECT = 0;                                     // clear interrupt flag
  return(s);
}