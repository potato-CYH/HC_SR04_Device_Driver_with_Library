import fnctl
import os

class Data:
    value: int = None

def init():
    device_file = '/dev/hcsr04'
    fd = os.open(device_file, os.O_RDWR)
    return fd

def measure(fd):
    data = Data()
    fnctl.ioctl(fd, _IO(0, 3), data)

    return (float)((data.value)/58)

def finalization(fd):
    os.close(fd)