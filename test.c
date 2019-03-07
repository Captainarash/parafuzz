#include "fuzz_interface.h"
#include <stdio.h>

char *global_io_driver_fuzz_list[] = {
  "IOSurfaceRoot",
  //"IntelAccelerator",
  //"IntelFBClientControl",
  "IODisplayWrangler",
  "IOAVBNub",
  "AppleMobileFileIntegrity",
  "IOHIDSystem",
  "IOHIDUserDevice",
  "AppleUSBRootHubDevice",
  "AppleUSBInterface",
  "AppleUSBDevice",
  "AppleKeyStore",
  "AppleFDEKeyStore",
  "IOTimeSyncClockManager",
  "AppleBroadcomBluetoothHostController",
  "IOBluetoothHCIController",
  "IOReportHub",
  "AppleSMC",
  "BridgeAudioControllerPCI",
  "BridgeAudioCommunicationService",
  "com_apple_AVEBridge",
  "IOUSBHostHIDDevice",
  "AppleActuatorDevice",
  "AppleEffaceableNOR",
  "IONVMeBlockStorageDevice",
  "IOThunderboltController",
  "IOFramebufferI2CInterface",
  "AppleUpstreamUserClientDriver",
  "AppleMCCSControlModule",
  "AppleIntelFramebuffer",
  "AGPM",
  "AppleIntelMEClientController",
  //"AGDPClientControl",
  "IOPMrootDomain",
  "ApplePlatformEnabler"
};

void fuzz_all_drivers(){
  for (size_t j = 0; j < 32; j++) {
     for (uint32_t i = 0; i < 100; i++) {
      io_connect_t io_connection = get_user_client(global_io_driver_fuzz_list[j],i);
      if(io_connection != IO_OBJECT_NULL) {
        printf("%s type = %u\n", global_io_driver_fuzz_list[j], i);
        IOCCM_fuzz_selectors(io_connection);
        IOServiceClose(io_connection);
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
