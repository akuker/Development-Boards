// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================


#include "usb_host.h"


#ifndef DISK_BASE_BUF_LEN

// ===================================================================================
// Initialize the secondary USB device after enumerating the external HUB
// Return ERR_SUCCESS:      success
//        ERR_USB_UNKNOWN:  unknown device
// ===================================================================================
uint8_t InitDevOnHub(uint8_t HubPortIndex) {
  uint8_t i, s, cfg, dv_cls, if_cls;
  uint8_t ifc;
  #if DEBUG_ENABLE
  printf("Init dev @ExtHub-port_%1d ", (uint16_t)HubPortIndex);
  #endif
  if(HubPortIndex == 0) return(ERR_USB_UNKNOWN);

  // Select the specified port of the external HUB that operates the specified 
  // ROOT-HUB port, select the speed
  SelectHubPort(HubPortIndex);
  #if DEBUG_ENABLE
  printf("GetDevDescr: ");
  #endif
  s = CtrlGetDeviceDescr();                               // get device descriptor
  if(s != ERR_SUCCESS) return(s);
  DevOnHubPort[HubPortIndex-1].DeviceVID = ((PXUSB_DEV_DESCR)Com_Buffer)->idVendor;  // VID
  DevOnHubPort[HubPortIndex-1].DevicePID = ((PXUSB_DEV_DESCR)Com_Buffer)->idProduct; // PID

  dv_cls = ((PXUSB_DEV_DESCR)Com_Buffer) -> bDeviceClass; // device class code
  cfg = (1<<4) + HubPortIndex;                            // calculate a USB address to avoid address overlap
  s = CtrlSetUsbAddress(cfg);                             // set USB device address
  if(s != ERR_SUCCESS) return(s);
  DevOnHubPort[HubPortIndex-1].DeviceAddress = cfg;       // save the assigned USB address

  #if DEBUG_ENABLE
  printf("Config Descriptor: ");
  #endif
  s = CtrlGetConfigDescr();                               // get configuration descriptor
  if(s != ERR_SUCCESS) return(s);
  cfg = ((PXUSB_CFG_DESCR)Com_Buffer) -> bConfigurationValue;
  #if DEBUG_ENABLE
  for(i=0; i<(uint8_t)(((PXUSB_CFG_DESCR)Com_Buffer)->wTotalLength); i++)
    printf("0x%02X ", (uint16_t)(Com_Buffer[i]));
  printf("\n");
  #endif

  // Analyze the configuration descriptor, obtain endpoint data/endpoint address/endpoint size, etc., 
  // update variables endp_addr and endp_size, etc.
  if_cls = ((PXUSB_CFG_DESCR_LONG)Com_Buffer) -> itf_descr.bInterfaceClass; // interface class code

  // USB storage device, basically confirmed to be a U-disk
  if(dv_cls == 0x00 && if_cls == USB_DEV_CLASS_STORAGE) {
    AnalyzeBulkEndp(Com_Buffer, HubPortIndex);
    #if DEBUG_ENABLE
    for(i=0; i!=4; i++)
      printf("%02x ", (uint16_t)DevOnHubPort[HubPortIndex-1].GpVar[i]);
    printf("\n");
    #endif
    s = CtrlSetUsbConfig(cfg);                            // set USB device configuration
    if(s == ERR_SUCCESS) {
      DevOnHubPort[HubPortIndex-1].DeviceStatus = ROOT_DEV_SUCCESS;
      DevOnHubPort[HubPortIndex-1].DeviceType = USB_DEV_CLASS_STORAGE;
      #if DEBUG_ENABLE
      printf("USB-Disk Ready\n");
      #endif
      SetUsbSpeed(1);                                     // default is full speed
      return(ERR_SUCCESS);
    }
  }

  // HID device (keyboard, mouse, etc.)
  else if((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_HID) 
    && (((PXUSB_CFG_DESCR_LONG)Com_Buffer) -> itf_descr.bInterfaceSubClass <= 0x01)) {
    ifc = ((PXUSB_CFG_DESCR_LONG)Com_Buffer) -> cfg_descr.bNumInterfaces;
    // analyze the address of the HID interrupt endpoint from the descriptor
    s = AnalyzeHidIntEndp(Com_Buffer, HubPortIndex);
    #if DEBUG_ENABLE
    printf("AnalyzeHidIntEndp %02x\n", (uint16_t)s);
    #endif
    if_cls = ((PXUSB_CFG_DESCR_LONG)Com_Buffer) -> itf_descr.bInterfaceProtocol;
    s = CtrlSetUsbConfig(cfg);                            // set USB device configuration
    if(s == ERR_SUCCESS) {
      for(dv_cls=0; dv_cls<ifc; dv_cls++) {
        s = CtrlGetHIDDeviceReport(dv_cls);               // get report descriptor
        if(s == ERR_SUCCESS) {
          #if DEBUG_ENABLE
          for(i=0; i<64; i++)
            printf("0x%02X ", (uint16_t)(Com_Buffer[i]));
          printf("\n");
          #endif
        }
      }

      // The endpoint information needs to be saved so that the main program can 
      // perform USB transmission
      DevOnHubPort[HubPortIndex-1].DeviceStatus = ROOT_DEV_SUCCESS;

      // Keyboard
      if(if_cls == 1) {
        DevOnHubPort[HubPortIndex-1].DeviceType = DEV_TYPE_KEYBOARD;
        // Further initialization, such as device keyboard indicator LED, etc.
        if(ifc > 1) {
          #if DEBUG_ENABLE
          printf("USB_DEV_CLASS_HID Ready\n");
          #endif
          DevOnHubPort[HubPortIndex-1].DeviceType = USB_DEV_CLASS_HID;  // composite HID device
        }
        #if DEBUG_ENABLE
        printf("USB-Keyboard Ready\n");
        #endif
        SetUsbSpeed(1);                                   // default is full speed
        return(ERR_SUCCESS);
      }

      // Mouse
      else if(if_cls == 2) {
        DevOnHubPort[HubPortIndex-1].DeviceType = DEV_TYPE_MOUSE;
        // In order to query the mouse state in the future, the descriptor should 
        // be analyzed to obtain the address, length and other information of the 
        // interrupt port.
        if(ifc > 1) {
          #if DEBUG_ENABLE
          printf("USB_DEV_CLASS_HID Ready\n");
          #endif
          DevOnHubPort[HubPortIndex-1].DeviceType = USB_DEV_CLASS_HID;  // composite HID device
        }
        #if DEBUG_ENABLE
        printf("USB-Mouse Ready\n");
        #endif
        SetUsbSpeed(1);                                   // default is full speed
        return(ERR_SUCCESS);
      }
      s = ERR_USB_UNSUPPORT;
    }
  }

  // HUB type device (a hub, etc.)
  else if(dv_cls == USB_DEV_CLASS_HUB) {
    DevOnHubPort[HubPortIndex-1].DeviceType = USB_DEV_CLASS_HUB;
    #if DEBUG_ENABLE
    // Need to support multi-level HUB cascading, please refer to this program for expansion
    printf("This program don't support Level 2 HUB\n");
    #endif
    s = HubClearPortFeature(i, HUB_PORT_ENABLE);          // disable HUB port
    if(s != ERR_SUCCESS) return(s);
    s = ERR_USB_UNSUPPORT;
  }

  // Other devices
  else {
    AnalyzeBulkEndp(Com_Buffer , HubPortIndex);           // parse out bulk endpoints
    #if DEBUG_ENABLE
    for(i=0; i!=4; i++)
      printf("%02x ", (uint16_t)DevOnHubPort[HubPortIndex-1].GpVar[i]);
    printf("\n");
    #endif
    s = CtrlSetUsbConfig(cfg);                            // set USB device configuration
    if(s == ERR_SUCCESS) {
      // The endpoint information needs to be saved so that the main program can perform USB transmission
      DevOnHubPort[HubPortIndex-1].DeviceStatus = ROOT_DEV_SUCCESS;
      DevOnHubPort[HubPortIndex-1].DeviceType = dv_cls ? dv_cls : if_cls;
      SetUsbSpeed(1);                                     // default is full speed
      return(ERR_SUCCESS);                                // unknown device initialized successfully
    }
  }
  #if DEBUG_ENABLE
  printf("InitDevOnHub Err = %02X\n", (uint16_t)s);
  #endif
  DevOnHubPort[HubPortIndex-1].DeviceStatus = ROOT_DEV_FAILED;
  SetUsbSpeed(1);                                         // default is full speed
  return(s);
}
#endif


