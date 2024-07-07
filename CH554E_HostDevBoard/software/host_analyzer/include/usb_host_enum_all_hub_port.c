// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include "usb_host.h"


#ifndef DISK_BASE_BUF_LEN
// ===================================================================================
// Enumerate all secondary USB devices behind the external HUB under the ROOT-HUB port
// Return ERR_SUCCESS:      success
// ===================================================================================
uint8_t EnumAllHubPort(void) {
  uint8_t s;
  if((ThisUsbDev.DeviceStatus >= ROOT_DEV_SUCCESS) 
    && (ThisUsbDev.DeviceType == USB_DEV_CLASS_HUB)) { // HUB enumeration succeeded
    // Select to operate designated ROOT-HUB port, set current USB speed and 
    // USB address of operated device
    SelectHubPort(0);
    // Power on each port of HUB, query status of each port, initialize HUB port 
    // with device connection, and initialize device
    //for(i=1; i<=ThisUsbDev.GpVar; i++) {        // initialize each port of HUB
      //s = HubSetPortFeature(i, HUB_PORT_POWER); // power on each port of HUB
      //if(s != ERR_SUCCESS) return(s);           // maybe the HUB is disconnected
    //}

    // Enumerate each port of external HUB hub on specified ROOT-HUB port, and 
    // check whether each port has a connection or removal event
    s = EnumHubPort();
    if(s != ERR_SUCCESS) {                        // maybe the HUB is disconnected
      #if DEBUG_ENABLE
      printf("EnumAllHubPort err = %02X\n", (uint16_t)s);
      #endif
    }
    SetUsbSpeed(1);                               // default is full speed
  }
  return(ERR_SUCCESS);
}
#endif

