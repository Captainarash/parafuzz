
#ifndef FUZZ_INTERFACE
#define FUZZ_INTERFACE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <IOKit/IOKitLib.h>
#include <mach/mach.h>
#include <mach/vm_map.h>
#include <unistd.h>
#include <dlfcn.h>
#include <time.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>


#define MAX_TYPES 256

/******************************************************************************
* struct: VALID_TYPES
*           Used by find_connection_types()
******************************************************************************/
typedef struct VALID_TYPES {
  size_t size;
  uint32_t *types;
} valid_types, *valid_types_ptr;

typedef struct raw_mach_msg_s raw_mach_msg_t;
struct raw_mach_msg_s {
  mach_msg_header_t header;
  //mach_msg_port_descriptor_t ports;
  char padding[0x10];
};

typedef struct complex_mach_msg_ool_s complex_mach_msg_ool_t;
struct complex_mach_msg_ool_s {
  mach_msg_header_t header;
  mach_msg_body_t body;
  mach_msg_ool_descriptor_t data;
  char padding[0x4];
  uint32_t ool_size;
  //mach_msg_type_number_t count;
};

typedef struct driver_s driver_t;
struct driver_s {
  char *name;
  int requires_root;
};

mach_port_t get_user_client(char *name, uint32_t type);
valid_types_ptr find_connection_types(char *name);
int IOCCM_fuzz_selectors(io_connect_t io_connection);
kern_return_t get_service_from_bootstrap(char *service_name, mach_port_t *mach_port);
void fill_buffer(char *buf, size_t s);
void record_call_id(uint32_t type, uint32_t selector);
void reverse_buf(char *buf, size_t s);
void flip_buf(char *buf, size_t s);
void clamp_buf(char *buf, size_t s);

extern kern_return_t bootstrap_look_up(mach_port_t bootstrap_port, char* service_name, mach_port_t* service_port);

#endif
