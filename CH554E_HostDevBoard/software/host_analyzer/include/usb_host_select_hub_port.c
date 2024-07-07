// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include <delay.h>
#include "usb_host.h"

// ===================================================================================
// Select the HUB port to be operated
// ===================================================================================
#ifndef DISK_BASE_BUF_LEN
void SelectHubPort(uint8_t HubPortIndex) {
  if(HubPortIndex) {  // Select designated port of external HUB that operates designated ROOT-HUB port
    SetHostUsbAddr(DevOnHubPort[HubPortIndex-1].DeviceAddress); // set address of USB device currently operated by USB host
    SetUsbSpeed(DevOnHubPort[HubPortIndex-1].DeviceSpeed);      // set current USB speed
    if(DevOnHubPort[HubPortIndex-1].DeviceSpeed == 0) {         // communication with low-speed USB devices through an external HUB requires a pre-ID
      UH_SETUP |= bUH_PRE_PID_EN;                               // enable PRE PIDs
      HubLowSpeed = 1;
      DLY_us(100);
    }
  }
  else {
    HubLowSpeed = 0;        			
    SetHostUsbAddr(ThisUsbDev.DeviceAddress);   // set address of USB device currently operated by USB host
    SetUsbSpeed(ThisUsbDev.DeviceSpeed);        // set speed of USB device
  }
}
#endif