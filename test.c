#include "fuzz_interface.h"
#include <stdio.h>
#include <IOKit/pwr_mgt/IOPMLib.h>

driver_t global_io_driver_fuzz_list[] = {
//   {"IOSurfaceRoot", FALSE},
//   {"IntelAccelerator", FALSE},
//   {"IntelFBClientControl", FALSE},
//   {"IODisplayWrangler", FALSE},
//   {"IOAVBNub", FALSE},
//   {"AppleMobileFileIntegrity", FALSE},
//   {"IOHIDSystem", FALSE},
//   {"IOHIDUserDevice", FALSE},
//   {"AppleUSBRootHubDevice", FALSE},
//   {"AppleUSBInterface", FALSE},
//   {"AppleUSBDevice", FALSE},
//   {"AppleKeyStore", FALSE},
//   {"AppleFDEKeyStore", FALSE},
//   {"IOTimeSyncClockManager", FALSE},
//   {"AppleBroadcomBluetoothHostController", FALSE},
//   {"IOBluetoothHCIController", FALSE},
//   {"IOReportHub", FALSE},
//   {"AppleSMC", FALSE},
//   {"BridgeAudioControllerPCI", FALSE},
//   {"BridgeAudioCommunicationService", FALSE},
//   {"com_apple_AVEBridge", FALSE},
//   {"IOUSBHostHIDDevice", FALSE},
//   {"AppleActuatorDevice", FALSE},
//   {"AppleEffaceableNOR", FALSE},
//   {"IONVMeBlockStorageDevice", FALSE},
//   {"IOThunderboltController", FALSE},
//   {"IOFramebufferI2CInterface", FALSE},
//   {"AppleUpstreamUserClientDriver", FALSE},
//   {"AppleMCCSControlModule", FALSE},
//   {"AppleIntelFramebuffer", FALSE},
//   {"AGPM", FALSE},
//   {"AppleIntelMEClientController", FALSE},
//   {"AGDPClientControl", FALSE},
//   {"IOPMrootDomain", FALSE},
//   {"ApplePlatformEnabler" FALSE},
//   {"AppleThunderboltPCIDownAdapter", FALSE},
//   {"AppleSEPManager", TRUE}
};
void SleepNow()
{
    io_connect_t fb = IOPMFindPowerManagement(MACH_PORT_NULL);
    if (fb != MACH_PORT_NULL)
    {
        IOPMSleepSystem(fb);
        IOServiceClose(fb);
    }
}

void fuzz_all_drivers(){
  //SleepNow();
  for (size_t j = 0; j < sizeof(global_io_driver_fuzz_list)/sizeof(global_io_driver_fuzz_list[0]); j++) {
     for (uint32_t i = 0; i < 100; i++) {
      io_connect_t io_connection = get_user_client(global_io_driver_fuzz_list[j].name,i);
      if(io_connection != IO_OBJECT_NULL) {
        printf("%s type = %u\n", global_io_driver_fuzz_list[j].name, i);
        int ld = open("/Users/arash/Documents/Github/parafuzz/last_driver.txt", O_CREAT | O_WRONLY | O_EXLOCK);
        printf("ld=%d\n",ld);
        fsync(ld);
        fsync(ld);
        write(ld, global_io_driver_fuzz_list[j].name, strlen(global_io_driver_fuzz_list[j].name) + 1);
        fsync(ld);
        fsync(ld);
        close(ld);
        IOCCM_fuzz_selectors(io_connection);
        IOServiceClose(io_connection);
        remove("/Users/arash/Documents/Github/parafuzz/last_driver.txt");
        remove("/Users/arash/Documents/Github/parafuzz/trigger.buf");
        sync();
      }
    }
  }
}

int main(int argc, char** argv)
{
  if(argc < 2)
  {
    printf("requires 1 argument!\n");
    return 0;
  }
  srand(time(NULL));

  if (strncmp(argv[1], "all", 3) == 0) {
    fuzz_all_drivers();
    return 0;
  }
  valid_types_ptr valid_types = find_connection_types(argv[1]);
  if (!valid_types) {
    printf("failed!\n");
    return -1;
  }
  for (uint32_t i = 0; i < valid_types->size; i++) {
    printf("%u\n", valid_types->types[i]);
  }

  FILE *logfile = fopen("logfile.txt","w");
  if (!logfile) {
    printf("failed to create file\n");
    perror("fopen");
    return 0;
  }
  fprintf (logfile, "this is the logfile for the crash\n");
  fclose(logfile);


  for (uint32_t i = 0; i < valid_types->size; i++) {
    printf("Type: %u\n", valid_types->types[i]);
    FILE *logfile = fopen("logfile.txt","a");
    if (!logfile) {
      printf("failed to create file\n");
      perror("fopen");
      return 0;
    }
    fprintf (logfile, "type: %u\n",valid_types->types[i]);
    fclose(logfile);
    io_connect_t io_connection = get_user_client(argv[1],valid_types->types[i]);
    if(io_connection == IO_OBJECT_NULL){
      printf("failed to get the user client\n");
      continue;
    }
    IOServiceClose(io_connection);
  }
  return 0;
}
