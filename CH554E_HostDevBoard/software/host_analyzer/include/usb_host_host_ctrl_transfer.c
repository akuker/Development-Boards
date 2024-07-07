// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include <delay.h>
#include <string.h>
#include "usb_host.h"
#include "usb_host_internal.h"



// ===================================================================================
// Execute control transmission, the 8-byte request code is in pSetupReq, and DataBuf 
// is an optional sending and receiving buffer.
// Return ERR_USB_BUF_OVER: IN state phase error
//        ERR_SUCCESS:      data exchange succeeded
// ===================================================================================
uint8_t HostCtrlTransfer(__xdata uint8_t *DataBuf, uint8_t *RetLen) {
  uint16_t RemLen = 0;
  uint8_t s, RxLen, RxCnt, TxCnt;
  __xdata uint8_t *pBuf;
  uint8_t *pLen;
  pBuf = DataBuf;
  pLen = RetLen;
  DLY_us(200);
  if(pLen) *pLen = 0;       // total length of actual successful sending and receiving
  UH_TX_LEN = sizeof(USB_SETUP_REQ);
  s = USBHostTransact((uint8_t)(USB_PID_SETUP << 4 | 0x00), 0x00, 10000); // SETUP stage, 200mS timeout
  if(s != ERR_SUCCESS) return(s);
  UH_RX_CTRL = UH_TX_CTRL = bUH_R_TOG | bUH_R_AUTO_TOG | bUH_T_TOG | bUH_T_AUTO_TOG;  // default DATA1
  UH_TX_LEN = 0x01;     // default state of no data is IN
  RemLen = (pSetupReq -> wLengthH << 8)|( pSetupReq -> wLengthL);
  if(RemLen && pBuf) {  // need to send and receive data
    if(pSetupReq -> bRequestType & USB_REQ_TYP_IN) {    // receive
      while(RemLen) {
        DLY_us(200);
        s = USBHostTransact((uint8_t)(USB_PID_IN << 4 | 0x00), UH_RX_CTRL, 200000/20);  // IN data
        if(s != ERR_SUCCESS) return(s);
        RxLen = USB_RX_LEN < RemLen ? USB_RX_LEN : RemLen;
        RemLen -= RxLen;
        if(pLen) *pLen += RxLen;  // total length of actual successful sending and receiving
        //memcpy(pBuf, RxBuffer, RxLen);
        //pBuf += RxLen;
        for(RxCnt = 0; RxCnt != RxLen; RxCnt ++) {
          *pBuf = RxBuffer[RxCnt];
          pBuf++;
        }
        if(USB_RX_LEN == 0 || (USB_RX_LEN & (UsbDevEndp0Size - 1))) break;  // short bag
      }
      UH_TX_LEN = 0x00;                     // status phase is OUT
    }
    else {                                  // send
      while(RemLen) {
        DLY_us(200);
        UH_TX_LEN = RemLen >= UsbDevEndp0Size ? UsbDevEndp0Size : RemLen;
        //memcpy(TxBuffer, pBuf, UH_TX_LEN);
        //pBuf += UH_TX_LEN;
        #ifndef DISK_BASE_BUF_LEN
        if(pBuf[1] == 0x09) {               // HID class command processing
          Set_Port = Set_Port^1;
          *pBuf = Set_Port;
          #if DEBUG_ENABLE									
          printf("SET_PORT  %02X  %02X ",(uint16_t)(*pBuf),(uint16_t)(Set_Port));
          #endif									
        }
        #endif
        for(TxCnt = 0; TxCnt != UH_TX_LEN; TxCnt ++) {
          TxBuffer[TxCnt] = *pBuf;
          pBuf ++;
        }
        s = USBHostTransact(USB_PID_OUT << 4 | 0x00, UH_TX_CTRL, 200000/20);  // OUT data
        if(s != ERR_SUCCESS) return(s);
        RemLen -= UH_TX_LEN;
        if(pLen) *pLen += UH_TX_LEN;        // total length of actual successful sending and receiving
      }
      //UH_TX_LEN = 0x01;                   // status phase is IN
    }
  }
  DLY_us(200);
  s = USBHostTransact((UH_TX_LEN ? USB_PID_IN << 4 | 0x00: USB_PID_OUT << 4 | 0x00), bUH_R_TOG | bUH_T_TOG, 200000/20); // STATUS stage
  if(s != ERR_SUCCESS) return(s);
  if(UH_TX_LEN == 0) return(ERR_SUCCESS);   // state OUT
  if(USB_RX_LEN == 0) return(ERR_SUCCESS);  // state IN, check IN state return data length
  return(ERR_USB_BUF_OVER);                 // IN state phase error
}
