// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include <delay.h>
#include "usb_host.h"

#ifndef DISK_BASE_BUF_LEN
// ===================================================================================
// Initialize the USB device of the specified ROOT-HUB port
// RootHubIndex:  Designated port, built-in HUB port number 0/1
// ===================================================================================
uint8_t InitRootDevice(void) {
  uint8_t t, i, s, cfg, dv_cls, if_cls, ifc;
  uint8_t touchaoatm = 0;
  t = 0;
  s = 0;
  #if DEBUG_ENABLE
  printf("Reset USB Port\n");
  #endif

USBDevEnum:
  for(i=0; i<t; i++) {
    DLY_ms(100);	
    if(t > 10) return(s);			
  }
  ResetRootHubPort();                         // after detecting device, reset USB bus of corresponding port
  for(i=0, s=0; i<100; i++) {                 // wait for USB device to reset and reconnect, 100ms timeout
    DLY_ms(1);
    if(EnableRootHubPort() == ERR_SUCCESS) {  // enable the ROOT-HUB port
      i = 0;
      s++;                                    // timer waits for USB device to stabilize after connection
      if(s > (20 + t)) break;                 // has been connected stably for 15ms
    }	
    if(i) {                                   // device not connecting after reset
      DisableRootHubPort();
      #if DEBUG_ENABLE
      //printf("Disable USB port because of disconnect\n");
      #endif
    }
  }
  SelectHubPort(0);

  #if DEBUG_ENABLE
  printf("Device Descriptor: ");
  #endif

  s = CtrlGetDeviceDescr();                   // get device descriptor
  if(s == ERR_SUCCESS) {
    #if DEBUG_ENABLE
    for (i = 0; i<((PUSB_SETUP_REQ)SetupGetDevDescr)->wLengthL; i++)
      printf("0x%02X ", (uint16_t)(Com_Buffer[i]));				
    printf("\n");                             // show descriptor
    #endif

    ThisUsbDev.DeviceVID = ((PXUSB_DEV_DESCR)Com_Buffer)->idVendor;  // save VID
    ThisUsbDev.DevicePID = ((PXUSB_DEV_DESCR)Com_Buffer)->idProduct; // save PID
    dv_cls = ((PXUSB_DEV_DESCR)Com_Buffer)->bDeviceClass;   // device class code
    // Set address of USB device, RootHubIndex ensures that the two HUB ports are assigned different addresses
    s = CtrlSetUsbAddress(((PUSB_SETUP_REQ)SetupSetUsbAddr)->wValueL);
    if(s == ERR_SUCCESS) {
      ThisUsbDev.DeviceAddress = ((PUSB_SETUP_REQ)SetupSetUsbAddr)->wValueL;  // save USB address
      #if DEBUG_ENABLE
      printf("Config Descriptor: ");
      #endif
      s = CtrlGetConfigDescr();               // get configuration descriptor
      if(s == ERR_SUCCESS) {
        cfg = ((PXUSB_CFG_DESCR)Com_Buffer) -> bConfigurationValue;
        ifc = ((PXUSB_CFG_DESCR)Com_Buffer) -> bNumInterfaces;
        #if DEBUG_ENABLE
        for(i=0; i<(uint8_t)(((PXUSB_CFG_DESCR)Com_Buffer)->wTotalLength); i++)
          printf("0x%02X ", (uint16_t)(Com_Buffer[i]));
        printf("\n");
        #endif
        // Analyze the configuration descriptor, obtain endpoint data/endpoint address/endpoint size, etc., 
        // update variables endp_addr and endp_size, etc.
        if_cls = ((PXUSB_CFG_DESCR_LONG)Com_Buffer)->itf_descr.bInterfaceClass; // interface class code
        // USB storage device, basically confirmed to be a U disk
        if(dv_cls == 0x00 && if_cls == USB_DEV_CLASS_STORAGE) {
          AnalyzeBulkEndp(Com_Buffer, 0);
          #if DEBUG_ENABLE
          for(i=0; i!=4 ;i++)
            printf("%02x ",(uint16_t)ThisUsbDev.GpVar[i] );
          printf("\n");
          #endif
          s = CtrlSetUsbConfig(cfg);          // set USB device configuration
          if(s == ERR_SUCCESS) {
            ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
            ThisUsbDev.DeviceType = USB_DEV_CLASS_STORAGE;
            #if DEBUG_ENABLE												
            printf("USB-Disk Ready\n");
            #endif											
            SetUsbSpeed(1);                   // default is full speed
            return(ERR_SUCCESS);
          }
        }

        // Printer device
        else if(dv_cls == 0x00 && if_cls == USB_DEV_CLASS_PRINTER 
          && ((PXUSB_CFG_DESCR_LONG)Com_Buffer) -> itf_descr.bInterfaceSubClass == 0x01) {
          #if DEBUG_ENABLE										
          printf("USB-Print OK\n");
          #endif									
          if((Com_Buffer[19] == 5) && (Com_Buffer[20]&&0x80))
            ThisUsbDev.GpVar[0] = Com_Buffer[20];               // IN endpoint
          else if((Com_Buffer[19] == 5) && ((Com_Buffer[20]&&0x80) == 0))
            ThisUsbDev.GpVar[1] = Com_Buffer[20];               // OUT endpoint
          if((Com_Buffer[26] == 5) && (Com_Buffer[20]&&0x80))
            ThisUsbDev.GpVar[0] = Com_Buffer[27];               // IN endpoint
          else if((Com_Buffer[26] == 5) && ((Com_Buffer[20]&&0x80) == 0))
            ThisUsbDev.GpVar[1] = Com_Buffer[27];               // OUT endpoint
          s = CtrlSetUsbConfig(cfg);                            // set USB device configuration
          if(s == ERR_SUCCESS)  {
            s = CtrlSetUsbIntercace(cfg);
            s = CtrlGetXPrinterReport1();                       // printer class commands
            if(s == ERR_SUCCESS) {
              ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
              ThisUsbDev.DeviceType = USB_DEV_CLASS_PRINTER;
              #if DEBUG_ENABLE														 
              printf("USB-Print Ready\n");
              #endif													 
              SetUsbSpeed(1);                                   // default is full speed
              return(ERR_SUCCESS);
            }
          }
        }

        // HID device (keyboard, mouse, etc.)
        else if((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_HID) 
          && (((PXUSB_CFG_DESCR_LONG)Com_Buffer) -> itf_descr.bInterfaceSubClass <= 0x01)) {
          // Analyze the address of the HID interrupt endpoint from the descriptor
          s = AnalyzeHidIntEndp(Com_Buffer, 0);
          #if DEBUG_ENABLE
          printf("AnalyzeHidIntEndp %02x\n",(uint16_t)s);
          #endif
          if_cls = ((PXUSB_CFG_DESCR_LONG)Com_Buffer) -> itf_descr.bInterfaceProtocol;
          #if DEBUG_ENABLE
          printf("CtrlSetUsbConfig %02x\n",(uint16_t)cfg);
          #endif
          s = CtrlSetUsbConfig(cfg);                            // set USB device configuration
          if(s == ERR_SUCCESS) {
            #if DEBUG_ENABLE
            printf("HID Report Descriptor: ");
            #endif
            for(dv_cls=0; dv_cls<ifc; dv_cls++) {
              s = CtrlGetHIDDeviceReport(dv_cls);               // get report descriptor
              if(s == ERR_SUCCESS) {
                #if DEBUG_ENABLE
                for (i=0; i<64; i++)
                  printf("0x%02X ", (uint16_t)(Com_Buffer[i]));
                printf("\n");
                #endif
              }
            }
            //Set_Idle();
            // The endpoint information needs to be saved so that the main program can perform USB transmission
            ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;

            // Keyboard
            if(if_cls == 1) {
              ThisUsbDev.DeviceType = DEV_TYPE_KEYBOARD;
              // Further initialization, such as device keyboard indicator LED, etc.
              if(ifc > 1) {
                #if DEBUG_ENABLE
                printf("USB_DEV_CLASS_HID Ready\n");
                #endif
                ThisUsbDev.DeviceType = USB_DEV_CLASS_HID;      // composite HID device
              }
              #if DEBUG_ENABLE														
              printf("USB-Keyboard Ready\n");
              #endif
              SetUsbSpeed(1);                                   // default is full speed
              return(ERR_SUCCESS);
            }

            // Mouse
            else if(if_cls == 2) {
              ThisUsbDev.DeviceType = DEV_TYPE_MOUSE;
              // In order to query the mouse state in the future, the descriptor should 
              // be analyzed to obtain the address, length and other information of the 
              // interrupt port.
              if(ifc > 1) {
                #if DEBUG_ENABLE
                printf("USB_DEV_CLASS_HID Ready\n");
                #endif
                ThisUsbDev.DeviceType = USB_DEV_CLASS_HID;      // composite HID device
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
          // Analyze the address of the HID interrupt endpoint from the descriptor
          s = AnalyzeHidIntEndp(Com_Buffer, 0);
          #if DEBUG_ENABLE
          printf("AnalyzeHidIntEndp %02x\n", (uint16_t)s);
          printf("Hub Descriptor:");
          #endif
          s = CtrlGetHubDescr();
          if(s == ERR_SUCCESS) {
            #if DEBUG_ENABLE
            for(i=0; i<Com_Buffer[0]; i++)
              printf("0x%02X ",(uint16_t)(Com_Buffer[i]));
            printf("\n");
            #endif
            ThisUsbDev.GpHUBPortNum = ((PXUSB_HUB_DESCR)Com_Buffer) -> bNbrPorts;   // save number of HUB ports
            // Because when defining the structure DevOnHubPort, it is artificially 
            // assumed that each HUB does not exceed HUB_MAX_PORTS ports
            if(ThisUsbDev.GpHUBPortNum > HUB_MAX_PORTS)
              ThisUsbDev.GpHUBPortNum = HUB_MAX_PORTS;
            #if DEBUG_ENABLE
            printf("Hub Product\n");
            #endif
            s = CtrlSetUsbConfig(cfg);                          // set USB device configuration
            if(s == ERR_SUCCESS) {
              ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
              ThisUsbDev.DeviceType = USB_DEV_CLASS_HUB;
              // The endpoint information needs to be saved so that the main program 
              // can perform USB transmission. Originally, the interrupt endpoint can 
              // be used for HUB event notification, but this program uses query status 
              // control transmission instead.
              // Power on each port of the HUB, query the status of each port, initialize 
              // the HUB port with device connection, and initialize the device.
              for(i=1; i<=ThisUsbDev.GpHUBPortNum; i++) {       // power on each port of the HUB
                DevOnHubPort[i-1].DeviceStatus = ROOT_DEV_DISCONNECT; // clear status of device on external HUB port
                s = HubSetPortFeature(i, HUB_PORT_POWER);
                if(s != ERR_SUCCESS) {
                  #if DEBUG_ENABLE
                  printf("Ext-HUB Port_%1d# power on error\n", (uint16_t)i);  // failed to power on the port
                  #endif
                }
              }
              SetUsbSpeed(1);                                   // default is full speed
              return(ERR_SUCCESS);
            }
          }
        }

        // Other devices
        else {
          #if DEBUG_ENABLE
          printf("dv_cls %02x\n", (uint16_t)dv_cls);
          printf("if_cls %02x\n", (uint16_t)if_cls);
          printf("if_subcls %02x\n", (uint16_t)( (PXUSB_CFG_DESCR_LONG)Com_Buffer) -> itf_descr.bInterfaceSubClass);
          #endif
          AnalyzeBulkEndp(Com_Buffer , 0);                      // parse out bulk endpoints
          #if DEBUG_ENABLE
          for(i=0; i!=4; i++)
            printf("%02x ", (uint16_t)ThisUsbDev.GpVar[i]);
          printf("\n");
          #endif
          s = CtrlSetUsbConfig(cfg);                            // set USB device configuration
          if(s == ERR_SUCCESS) {
            #if DEBUG_ENABLE						
            printf("%02x %02x\n", (uint16_t)ThisUsbDev.DeviceVID, (uint16_t)ThisUsbDev.DevicePID);
            #endif
            if((ThisUsbDev.DeviceVID == 0x18D1) && (ThisUsbDev.DevicePID&0xff00) == 0x2D00) {   // AOA accessories
              #if DEBUG_ENABLE
              printf("AOA Mode\n");
              #endif
              ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
              ThisUsbDev.DeviceType = DEF_AOA_DEVICE;           // just a custom variable class, not a USB protocol class
              SetUsbSpeed(1);                                   // default is full speed
              return(ERR_SUCCESS);
            }
            // If it is not AOA accessory mode, try starting accessory mode.
            else {
              s = TouchStartAOA();
              if(s == ERR_SUCCESS) {
                if(touchaoatm < 3) {  // limit number of AOA starts
                  touchaoatm++;
                  DLY_ms(500);        // some Android devices automatically disconnect and reconnect, so it is best to have a delay here
                  goto USBDevEnum;    // In fact, there is no need to jump here. The AOA protocol stipulates that the device will automatically reconnect to the bus.
                }
                // Execute to this point, indicating that AOA may not be supported, or other devices
                ThisUsbDev.DeviceType = dv_cls ? dv_cls : if_cls;
                ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
                SetUsbSpeed(1);                                 // default is full speed
                return(ERR_SUCCESS);                            // unknown device initialized successfully
              }							
            }
          }
        }
      }
    }
  }
  #if DEBUG_ENABLE
  printf("InitRootDev Err = %02X\n", (uint16_t)s);
  #endif
  ThisUsbDev.DeviceStatus = ROOT_DEV_FAILED;
  SetUsbSpeed(1);                                               // default is full speed
  t++;
  goto USBDevEnum;
}

#else
//#ifdef DISK_BASE_BUF_LEN

// ===================================================================================
// Initialize the USB device
// ===================================================================================
uint8_t	InitRootDevice(void) {
  uint8_t i, s, cfg, dv_cls, if_cls;
  #if DEBUG_ENABLE
  printf("Reset host port\n");
  #endif
  ResetRootHubPort();                 // after detecting device, reset USB bus of corresponding port
  for(i=0, s=0; i<100; i++) {         // wait for USB device to reset and reconnect, 100mS timeout
    DLY_ms(1);
    if(EnableRootHubPort() == ERR_SUCCESS) {  // enable port
      i = 0;
      s++;                            // timer waits for USB device to stabilize after connection
      if(s > 100) break;              // has been connected stably for 100mS
    }
  }
  if(i) {                             // device not connected after reset
    DisableRootHubPort();
    #if DEBUG_ENABLE
    printf("Disable host port because of disconnect\n");
    #endif
    return(ERR_USB_DISCON);
  }
  SetUsbSpeed(1);                     // set current USB speed
  s = CtrlGetDeviceDescr();           // get device descriptor
  if(s == ERR_SUCCESS) {
    #if DEBUG_ENABLE
    printf("Device Descriptor: ");
    for(i=0; i<((PUSB_SETUP_REQ)SetupGetDevDescr)->wLengthL; i++) 
      printf("0x%02X ", (uint16_t)(Com_Buffer[i]));
    printf("\n");                     // show descriptor
    #endif
    dv_cls = ((PXUSB_DEV_DESCR)Com_Buffer) -> bDeviceClass;               // device class code
    s = CtrlSetUsbAddress(((PUSB_SETUP_REQ)SetupSetUsbAddr) -> wValueL);  // set USB device address
    if(s == ERR_SUCCESS) {
      s = CtrlGetConfigDescr();       // get configuration descriptor
      if(s == ERR_SUCCESS) {
        cfg = ((PXUSB_CFG_DESCR)Com_Buffer) -> bConfigurationValue;
        #if DEBUG_ENABLE
        printf("Config Descriptor: ");
        for(i=0; i<((PXUSB_CFG_DESCR)Com_Buffer)->wTotalLengthL; i++) 
          printf("0x%02X ", (uint16_t)(Com_Buffer[i]));
        printf("\n");
        #endif

        // Analyze configuration descriptor, obtain endpoint data/endpoint address/endpoint size, etc., 
        // update variables endp_addr and endp_size, etc.
        if_cls = ((PXUSB_CFG_DESCR_LONG)Com_Buffer) -> itf_descr.bInterfaceClass; // interface class code
        if((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_STORAGE)) { // USB storage device, basically confirmed to be a U-disk					
          //s = CtrlSetUsbConfig(cfg);                              // set USB device configuration
          //if(s == ERR_SUCCESS) {
            CH554DiskStatus = DISK_USB_ADDR;
            return(ERR_SUCCESS);
          //}
          //else return(ERR_USB_UNSUPPORT);
        }
        else return(ERR_USB_UNSUPPORT);
      }
    }
  }
  #if DEBUG_ENABLE
  printf("InitRootDev Err = %02X\n", (uint16_t)s);
  #endif
  CH554DiskStatus = DISK_CONNECT;
  SetUsbSpeed(1);                     // default is full speed
  return(s);
}
#endif