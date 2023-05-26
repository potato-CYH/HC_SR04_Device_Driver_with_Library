#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "hcsr04.h"


struct Node{
  char n;
  char buf[100];
};

int main()
{
  
  int fd = init();

  if(fd< 0){
    printf("OPEN ERROR\n");
    finalization(fd);
    return 0;
  }
  
  while(1){
    float ret = measure(fd);
    printf("value : %f\n", ret);
    usleep(60000);
  }
  finalization(fd);
  return 0;
}

