// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================


#include "usb_host.h"

#ifndef DISK_BASE_BUF_LEN
// ===================================================================================
// Search for the port number of the specified type of device on each port of ROOT-HUB
// and external HUB, if the output port number is 0xFFFF, it cannot be found.
// Input:   The type of device being searched for
// Return:  The high 8 bits of the output are the ROOT-HUB port number, the low 8 bits 
//          are the port number of the external HUB, and if the low 8 bits are 0, the 
//          device is directly on the ROOT-HUB port.
//          Of course, you can also search according to the PID of the USB manufacturer's 
//          VID product (record the VID and PID of each device in advance), and specify 
//          the search number.
// ===================================================================================
uint16_t SearchTypeDevice(uint8_t type) {
  // CH554 has only one USB port, RootHubIndex = 0, just look at the lower eight bits 
  // of the return value
  uint8_t RootHubIndex;
  uint8_t HubPortIndex;
  RootHubIndex = 0;
  if((ThisUsbDev.DeviceType == USB_DEV_CLASS_HUB) 
    && (ThisUsbDev.DeviceStatus >= ROOT_DEV_SUCCESS)) { // external HUB and enumeration is successful

    // Search each port of the external HUB
    for(HubPortIndex = 1; HubPortIndex <= ThisUsbDev.GpHUBPortNum; HubPortIndex++) {
      if(DevOnHubPort[HubPortIndex-1].DeviceType == type && DevOnHubPort[HubPortIndex-1].DeviceStatus >= ROOT_DEV_SUCCESS)
        return(((uint16_t)RootHubIndex << 8 ) | HubPortIndex);  // type matches and enumeration succeeds
    }
  }
  if((ThisUsbDev.DeviceType == type) && (ThisUsbDev.DeviceStatus >= ROOT_DEV_SUCCESS))
    return((uint16_t)RootHubIndex << 8);  // type matches and enumeration is successful, on the ROOT-HUB port
  return( 0xFFFF );
}
#endif



