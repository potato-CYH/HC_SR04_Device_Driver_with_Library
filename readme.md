
# HC-SR04 Linux Device Driver Manual

  
| Device Type |  Ultrasonic Sensor  |
|--|--|
| Target Kernel |  Linux  |
|  Target Architecture |  ARM (Cortex-A72, bcm2711)|
| Target Board |  Raspberry PI |
| License  |  GPL |
| Author | mydream11123@naver.com |


### 1. Hardware Conntection Guide

  

![image](https://github.com/potato-CYH/HC_SR04_Device_Driver_with_Library/assets/57744586/831e9fd9-9c6e-4096-bb8e-8354a82ff607)

  

- Connect **`Trig`**  to  **`GPIO`** you want

- Connect **`Echo`** to **`GPIO`** you want

  

**Remember GPIO Pin number you connected HC-SR04. This Information will use “*section 2.4”***

  

### 2. Device Driver Installation Guide

  

- 2-1. Clone this repository into your target board ( **Network must be conntected** )

- 2-2. Place folder ( cloned at “*section 2.1”* ) in your project folder.

- 2-3. Open “hc_sr04_driver.c” with text editor. ( such as vi editor, vim, etc. but I recommend using Visual Studio Code )

- 2-4. Modify line 13, 14 to the pin number you remember in *“section1”*.

```c

13 | #define TRIG_PIN *<GPIO Pin Number you connected in Trig Pin>
14 | #define ECHO_PIN *<GPIO Pin Number you connected in Echo Pin>

```

- 2-5. Open Terminal In the HC-SR04 driver folder

- 2-6. Enter command below in order.

```c

$ sudo mknod /dev/hcsr04 c 200  0
$ sudo chmod 666 /dev/hcsr04
$ make all
$ sudo insmod ./hc_sr04_driver.ko**

```

  

### 3. Programming Guide with Device Driver
  
- You can use example app at **`./app`** and **`app.c`**

- You must call **`init()`** before using call **`measure()`**

- When you want to finish using driver, must call **`finallization()`**

-  **Make sure to delay at least 60ms for repeated calls `measure()`** ( for prevent hardware marfunction )
  

- Example

```c

int fd = init();

if(fd< 0){
	printf("OPEN ERROR\n");
	finalization(fd);

	return  0;
}

while(1){
	float ret = measure(fd);
	printf("value : %f\n", ret);
	usleep(60000);
}

finalization(fd);

return  0;

```