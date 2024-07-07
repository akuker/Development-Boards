// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include <delay.h>
#include "usb_host.h"

#ifndef DISK_BASE_BUF_LEN

// ===================================================================================
// Enumerates each port of the external HUB on the specified ROOT-HUB port, checks 
// whether each port has a connection or removal event and initializes the secondary 
// USB device.
// Return ERR_SUCCESS:      success
// ===================================================================================
uint8_t EnumHubPort(void) {
  uint8_t i, s;
  for(i=1; i<=ThisUsbDev.GpHUBPortNum; i++) {   // query whether the port of the hub has changed
    SelectHubPort(0);                           // select to operate designated ROOT-HUB port, set current USB speed and USB address of operated device
    s = HubGetPortStatus(i);                    // get port status
    if(s != ERR_SUCCESS) return(s);             // maybe HUB is disconnected

    // Found that there is a device connected
    if(((Com_Buffer[0]&(1<<(HUB_PORT_CONNECTION&0x07))) 
      && (Com_Buffer[2]&(1<<(HUB_C_PORT_CONNECTION&0x07)))) 
      || (Com_Buffer[2] == 0x10)) {
      DevOnHubPort[i-1].DeviceStatus = ROOT_DEV_CONNECTED;  // there is a device connected
      DevOnHubPort[i-1].DeviceAddress = 0x00;
      s = HubGetPortStatus(i);                    // get port status
      if(s != ERR_SUCCESS) return(s);             // maybe HUB is disconnected

      DevOnHubPort[i-1].DeviceSpeed = Com_Buffer[1] & (1<<(HUB_PORT_LOW_SPEED&0x07)) ? 0 : 1; // low speed or full speed
      #if DEBUG_ENABLE
      if(DevOnHubPort[i-1].DeviceSpeed)
        printf("Found full speed device on port %1d\n", (uint16_t)i);
      else
        printf("Found low speed device on port %1d\n", (uint16_t)i);
      #endif

      DLY_ms(200);                                // wait for device to power on and stabilize
      s = HubSetPortFeature(i, HUB_PORT_RESET);   // reset port with device connection
      if(s != ERR_SUCCESS) return(s);             // maybe the HUB is disconnected
      #if DEBUG_ENABLE
      printf("Reset port and then wait in\n");
      #endif

      // Query reset port until reset is completed, and display completed status
      do {
        DLY_ms(1);
        s = HubGetPortStatus(i);
        if(s != ERR_SUCCESS) return(s);                     // maybe the HUB is disconnected
      } while(Com_Buffer[0] & (1<<(HUB_PORT_RESET&0x07)));  // port is reset, wait
      DLY_ms(100);
      s = HubClearPortFeature(i, HUB_C_PORT_RESET);         // clear reset complete flag
      //s = HubSetPortFeature(i, HUB_PORT_ENABLE);          // enable HUB port
      s = HubClearPortFeature(i, HUB_C_PORT_CONNECTION);    // clear connection or remove change flag
      if(s != ERR_SUCCESS) return(s);
      s = HubGetPortStatus(i);                              // read status again and check whether device is still there
      if(s != ERR_SUCCESS) return(s);
      if((Com_Buffer[0]&(1<<(HUB_PORT_CONNECTION&0x07))) == 0)
        DevOnHubPort[i-1].DeviceStatus = ROOT_DEV_DISCONNECT; // device is gone
      s = InitDevOnHub( i );                                // initialize secondary USB device
      if(s != ERR_SUCCESS) return(s);
      SetUsbSpeed(1);                                       // default is full speed
    }
    else if(Com_Buffer[2]&(1<<(HUB_C_PORT_ENABLE&0x07))) {  // device connection error
      HubClearPortFeature(i, HUB_C_PORT_ENABLE);            // clear connection error flag
      #if DEBUG_ENABLE
      printf("Device on port error\n");
      #endif
      s = HubSetPortFeature(i, HUB_PORT_RESET);             // reset port with device
      if(s != ERR_SUCCESS) return(s);                       // maybe the HUB is disconnected

      // Query reset port until reset is completed, and display completed status
      do {
        DLY_ms(1);
        s = HubGetPortStatus(i);
        if(s != ERR_SUCCESS) return(s);                     // maybe the HUB is disconnected
      } while(Com_Buffer[0] & (1<<(HUB_PORT_RESET&0x07)));  // port is reset, wait
    }
    else if((Com_Buffer[0]&(1<<(HUB_PORT_CONNECTION&0x07))) == 0) { // device disconnected
      if(DevOnHubPort[i-1].DeviceStatus >= ROOT_DEV_CONNECTED) {
        #if DEBUG_ENABLE
        printf("Device on port %1d removed\n", (uint16_t)i);
        #endif
      }
      DevOnHubPort[i-1].DeviceStatus = ROOT_DEV_DISCONNECT; // there is a device connected
      if(Com_Buffer[2]&(1<<(HUB_C_PORT_CONNECTION&0x07)))
        HubClearPortFeature(i, HUB_C_PORT_CONNECTION);      // clear remove change flag
    }
  }
  return(ERR_SUCCESS);                                      // return operation successful
}
#endif

