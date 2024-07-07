// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include "usb_host.h"


// ===================================================================================
// Close the HUB port
// ===================================================================================
void DisableRootHubPort(void) {
  #ifdef FOR_ROOT_UDISK_ONLY
  CH554DiskStatus = DISK_DISCONNECT;
  #endif
  #ifndef DISK_BASE_BUF_LEN
  ThisUsbDev.DeviceStatus = ROOT_DEV_DISCONNECT;
  ThisUsbDev.DeviceAddress = 0x00;
  #endif
}