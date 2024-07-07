// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include "delay.h"
#include "usb_host.h"

// ===================================================================================
// CH554 transmission transaction, input destination endpoint address/PID token, 
// synchronization flag, NAK retry total time in 20uS (0 means no retry, 
// 0xFFFF infinite retry), return 0 success, timeout/error retry
// This subroutine focuses on easy understanding, but in practical applications, in 
// order to increase the running speed, the code of this subroutine should be optimized.
// Return ERR_USB_UNKNOWN:  timeout, possible hardware exception
//        ERR_USB_DISCON:   device disconnected
//        ERR_USB_CONNECT:  device connected
//        ERR_SUCCESS:      transfer complete
// ===================================================================================
uint8_t USBHostTransact(uint8_t endp_pid, uint8_t tog, uint16_t timeout) {
  //uint8_t TransRetry;
  #define TransRetry UEP0_T_LEN         // save memory
  uint8_t s, r;
  uint16_t i;
  UH_RX_CTRL = UH_TX_CTRL = tog;
  TransRetry = 0;

  do {
    UH_EP_PID = endp_pid;               // specify token PID and destination endpoint number
    UIF_TRANSFER = 0;                   // allow transmission
    // s = WaitUSB_Interrupt();
    for(i = WAIT_USB_TOUT_200US; i != 0 && UIF_TRANSFER == 0; i--);
    UH_EP_PID = 0x00;                   // stop USB transfer
    //if(s != ERR_SUCCESS) return(s);   // interrupt timeout, may be a hardware exception
    if(UIF_TRANSFER == 0) return(ERR_USB_UNKNOWN);
    if(UIF_DETECT) {                    // USB device plug event
    //DLY_us(200);                      // wait for the transfer to complete
    UIF_DETECT = 0;                     // clear interrupt flag
    s = AnalyzeRootHub();               // analyze ROOT-HUB status
    if(s == ERR_USB_CONNECT) FoundNewDev = 1;
      #ifdef DISK_BASE_BUF_LEN
      if(CH554DiskStatus == DISK_DISCONNECT) return(ERR_USB_DISCON);  // USB device disconnect event
      if(CH554DiskStatus == DISK_CONNECT) return(ERR_USB_CONNECT);    // USB device connect event
      #else
      if(ThisUsbDev.DeviceStatus == ROOT_DEV_DISCONNECT) return(ERR_USB_DISCON);  // USB device disconnect event
      if(ThisUsbDev.DeviceStatus == ROOT_DEV_CONNECTED) return(ERR_USB_CONNECT);  // USB device connect event
      #endif
      DLY_us(200);                      // wait for the transfer to complete
    }
    if(UIF_TRANSFER) {                  // transfer complete
      if(U_TOG_OK) return(ERR_SUCCESS);
      r = USB_INT_ST & MASK_UIS_H_RES;  // USB device answer status
      if(r == USB_PID_STALL) return(r | ERR_USB_TRANSFER);
      if(r == USB_PID_NAK) {
        if(timeout == 0) return(r | ERR_USB_TRANSFER);
        if(timeout < 0xFFFF) timeout--;
        --TransRetry;
      }
      else switch(endp_pid >> 4) {
        case USB_PID_SETUP:
        case USB_PID_OUT:
          //if(U_TOG_OK) return(ERR_SUCCESS);
          //if(r == USB_PID_ACK) return(ERR_SUCCESS);
          //if(r == USB_PID_STALL || r == USB_PID_NAK) return(r | ERR_USB_TRANSFER);
          if(r) return(r | ERR_USB_TRANSFER);   // not timeout/error, unexpected response
          break;                                // timeout retry
        case USB_PID_IN:
          //if(U_TOG_OK) return(ERR_SUCCESS);
          //if(tog ? r == USB_PID_DATA1 : r == USB_PID_DATA0) return(ERR_SUCCESS);
          //if(r == USB_PID_STALL || r == USB_PID_NAK) return(r | ERR_USB_TRANSFER);
          if(r == USB_PID_DATA0 && r == USB_PID_DATA1) {  // if not synchronized, discard it and try again
          }                                               // retry out of sync
          else if(r) return(r | ERR_USB_TRANSFER);        // not timeout/error, unexpected response
          break;                                          // timeout retry
        default:
          return(ERR_USB_UNKNOWN);                        // impossible situation
          break;
      }
    }
    else {                        // other interrupts, situations that should not happen
      USB_INT_FG = 0xFF;          // clear interrupt flag
    }
    DLY_us(15);	
  } while(++TransRetry < 3);
  return(ERR_USB_TRANSFER);       // response timeout
}
