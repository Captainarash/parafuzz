#include "fuzz_interface.h"
#include <stdio.h>
#include <pthread.h>

int test(char *name)
{
  valid_types_ptr valid_types = find_connection_types(name);
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
    io_connect_t io_connection = get_user_client(name,valid_types->types[i]);
    if(io_connection == IO_OBJECT_NULL){
      printf("failed to get the user client\n");
      continue;
    }
    if (IOCCM_fuzz_selectors(io_connection)) {
      //this should never happen!
      printf("what?\n");
    }
    IOServiceClose(io_connection);
  }

  // if(argc < 2)
  // {
  //   printf("requires 1 argument!\n");
  //   return 0;
  // }
  // io_connect_t io_connection = get_user_client("IntelAccelerator",);
  // if(io_connection == IO_OBJECT_NULL){
  //   printf("failed to get the user client\n");
  //   return 0;
  // }
  // IOCCM_fuzz_selectors(io_connection);
  // IOServiceClose(io_connection);
  return 0;
}


int main(int argc, char** argv){
  if(argc < 2)
  {
    printf("requires 1 argument!\n");
    return 0;
  }
  pid_t child_pid = fork();
  if (child_pid == -1) {
    printf("fork failed\n");
    return 0;
  }
  printf("got service\n");
  pthread_t th;
  pthread_create(&th, NULL, test, argv[1]);
    while(1){;}
  pthread_join(th, NULL);

  int loc = 0;
  wait(&loc);

  return 0;
}
