// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include "usb_host.h"
#include "usb_host_internal.h"



// ===================================================================================
// Initialize the USB host
// ===================================================================================
void InitUSB_Host(void) {
  uint8_t i;
  IE_USB  = 0;
//LED_CFG = 1;
//LED_RUN = 0;
  USB_CTRL    =  bUC_HOST_MODE;                 // set mode first
  UHOST_CTRL &= ~bUH_PD_DIS;                    // enable host pulldown
  USB_DEV_AD  =  0x00;
  UH_EP_MOD   =  bUH_EP_TX_EN 
              |  bUH_EP_RX_EN ;
  UH_RX_DMA   =  (uint16_t)RxBuffer;
  UH_TX_DMA   =  (uint16_t)TxBuffer;
  UH_RX_CTRL  =  0x00;
  UH_TX_CTRL  =  0x00;
  USB_CTRL    =  bUC_HOST_MODE                  // start USB host
//            |  bUC_DMA_EN                     // enable DMA
              |  bUC_INT_BUSY;                  // automatically suspend before the interrupt flag is cleared
//UHUB0_CTRL  =  0x00;
//UHUB1_CTRL  =  0x00;
//UH_SETUP    =  bUH_SOF_EN;
  USB_INT_FG  =  0xFF;                          // clear interrupt flag
  for(i=0; i!=2; i++) DisableRootHubPort();     // empty
  USB_INT_EN  =  bUIE_TRANSFER | bUIE_DETECT;
//IE_USB = 1;                                   // query mode
}