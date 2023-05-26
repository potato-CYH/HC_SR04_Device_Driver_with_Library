#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/ioctl.h>

struct Data{

  long long int value;
};
int init()
{

  int fd = open("/dev/hcsr04", O_RDWR);
  return fd;
}

float measure(int fd){
  
  struct Data data;
  ioctl(fd, _IO(0, 3), &data);
  
  return (float)((data.value)/58);
}

void finalization(int fd){
  close(fd);
}