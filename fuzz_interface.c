#include "fuzz_interface.h"
//
/******************************************************************************
* function: get_user_client()
*           It tries to create a connection to the user client of a matching
*            device.
* inputs:
*   char* name:     string to build a key-value dictionary.
*                   we expect a valid a property value to
*                   be able to find matching devices later.
*
*   uint32_t type:  type of the connection to create for IOServiceOpen()
*
*
* returns:  the io_connect_t object if succeeds. If failed, returns 0.
******************************************************************************/
mach_port_t get_user_client(char *name, uint32_t type)
{
  if (!name) {
    printf("char *name can't be null!\n");
    return 0;
  }
  CFMutableDictionaryRef device_match_dictionary = NULL;
  device_match_dictionary = IOServiceMatching(name);

  if (!device_match_dictionary) {
    printf("Failed to create device_match_dictionary with the value: %s\n", name);
    return 0;
  }

  io_iterator_t iterator = IO_OBJECT_NULL;
  kern_return_t error = IOServiceGetMatchingServices(kIOMasterPortDefault, device_match_dictionary, &iterator);
  if (error != KERN_SUCCESS) {
#ifdef DEBUG
    printf("Failed to find any matches with the dictionary which was made with the value: %s\n", name);
#endif
    return 0;
  }

  io_object_t io_object = IOIteratorNext(iterator);

   if (io_object == IO_OBJECT_NULL) {
#ifdef DEBUG
    printf("Unable to find io_object\n");
#endif
    return 0;
   }
#ifdef DEBUG
   printf("Got the io_object: %x\n", io_object);
#endif
   io_connect_t io_connection = IO_OBJECT_NULL;
   kern_return_t err = IOServiceOpen(io_object, mach_task_self(), type, &io_connection);
   if (err != KERN_SUCCESS){
#ifdef DEBUG
    printf("unable to get user client connection\n");
#endif
    return 0;
   }
#ifdef DEBUG
    printf("Got the connection to user client: %x\n", io_connection);
#endif
    return io_connection;
}


/******************************************************************************
* function: find_connection_types()
*           It tries to find all the possible connection types to the user
*           client of a matching device. The iteration of types is from 0 to
*           255.
* inputs:
*   char* name:     string to build a key-value dictionary.
*                   we expect a valid a property value to
*                   be able to find matching devices later.
*
* returns:  the valid_types_ptr that contains valid types of connection to a
*           device if succeeds. If failed, returns 0.
******************************************************************************/
valid_types_ptr find_connection_types(char *name)
{
  uint32_t types[MAX_TYPES]; // temporary array to keep the valid types
  valid_types_ptr valid_types = (valid_types_ptr) malloc(sizeof(struct VALID_TYPES));
  if (!valid_types) {
    printf("Invalid valid_types struct! null_ptr\n");
    return 0;
  }

  // check types between 0-255
  io_connect_t io_connection = IO_OBJECT_NULL;
  uint32_t size = 0;
  uint32_t type_copy = 0;
  for (uint32_t type = 0; type < MAX_TYPES; type++) {
    io_connect_t io_connection = get_user_client(name,type);
    if (io_connection) {
      types[size] = type;
      size++;
      IOServiceClose(io_connection);
      io_connection = IO_OBJECT_NULL;
    }
  }
  printf("found %u valid types\n", size);
  valid_types->size = size;
  valid_types->types = (uint32_t *) malloc(size);
  for (uint32_t i = 0; i < size; i++) {
    valid_types->types[i] = types[i];
  }
  return valid_types;
}

int IOCCM_fuzz_selectors(io_connect_t io_connection) {
  //testing with random selectors
  uint64_t input[3];
  uint64_t inputCnt = 3;
  memset(input,0x01,3);

  char inputStruct[100];
  memset(inputStruct,0x32,100);
  size_t inputStructCnt = -1;
  //memset(inputStruct, 0x4, 16);
  //*(uint64_t*)inputStruct = 0x41414141;
  //*(uint64_t*)inputStruct = 0x0;

  uint64_t output[8];
  uint32_t outputCnt = 6587687;
  char outputStruct[8];
  memset(output,0xFF,8);
  size_t outputStructCnt = -1;

   for (uint32_t selector = 0; selector < 0x2000; selector++) {
    //printf("Selector: %u\n", selector);
    //sleep(1);
    // //sleep(0.2);
    // FILE *logfile = fopen("logfile.txt","a");
    // fprintf (logfile, "selector: %u\n",selector);
    // fclose(logfile);
    if (io_connection == IO_OBJECT_NULL)
    {
      printf("io_connect_t object died :/\n");
      return 0;
    }
    kern_return_t err = IOConnectCallMethod
    (
      io_connection,
      selector,
      input,
      inputCnt,
      inputStruct,
      inputStructCnt,
      output,
      &outputCnt,
      outputStruct,
      &outputStructCnt
    );
    //printf("output: 0x%x\n",*(uint64_t*)output);
    //char *deref = *(uint64_t*)output;
    //printf("output: 0x%x\n",*deref);
  }

  return 0;
}

/******************************************************************************
* function: get_service_from_bootstrap()
*           It tries to find a given serive in bootstrap and get a handle to
*           the serive if found. It returns whatever the kernel returns,
*           including errors.
* inputs:
*   char* service_name:   string containing the service name to search for.
*                         an example could be: com.apple.logd
*
*   mach_port_t *mach_port: a pointer to a mach_port_t which which will contain
*                           the resulting handle to the requested service upon
*                           success.
*
* returns:  returns KERN_SUCCESS if succeeds and the input mach_port_t pointer
*           will contain the valid handle to the requested service. If fails,
*           returns the error and nulls the mach_port_t pointer.
******************************************************************************/
kern_return_t get_service_from_bootstrap(char *service_name, mach_port_t *mach_port) {
  mach_port_t bootstrap_port = MACH_PORT_NULL;
  kern_return_t err = task_get_bootstrap_port(mach_task_self(), &bootstrap_port);
  if (err != KERN_SUCCESS) {
    mach_port = MACH_PORT_NULL;
    printf("failed to get the bootstrap_port\n");
    return err;
  }

  err = bootstrap_look_up(bootstrap_port, service_name, mach_port);
  if (err != KERN_SUCCESS) {
    mach_port = MACH_PORT_NULL;
    printf("could not find the %s service: %s\n", service_name, mach_error_string(err));
    return err;
  }
  return err;
}