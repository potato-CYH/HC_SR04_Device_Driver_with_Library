#include <linux/module.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>
#include<linux/string.h>
#include<linux/interrupt.h>
#include <linux/delay.h>
#include <linux/gpio.h>


#define NOD_MAJOR 200
#define NOD_NAME "hcsr04"
#define TRIG_PIN 23
#define ECHO_PIN 24
static int echo_irq;
static int flag;
static ktime_t echo_start_t;
static ktime_t echo_stop_t;


struct Data {
	long long int value;
};


static int dev_open(struct inode* inode, struct file* filp) {
	printk(KERN_INFO "HC SR04 Device File Opened\n");
	return 0;
}


static int dev_release(struct inode* inode, struct file* filp) {
	printk(KERN_INFO "HC SR04 Device File Released\n");
	return 0;
}


static long long int measure(void) {
	int cnt;
	long long int buf;
	gpio_set_value(TRIG_PIN, 1);
	udelay(10);
	gpio_set_value(TRIG_PIN, 0);
	flag = 0;

	cnt = 0;
	while (flag == 0) {
		if (++cnt > 23200) {
			buf = -1;
		}
		udelay(1);
	}
	 //convert microseconds unit
	buf = ktime_to_ns(echo_stop_t - echo_start_t)/1000;
	return buf;
}

//Interrupt Service Routine
static irqreturn_t ISR(int irq, void* data) {
	//������ �ð��� time_val�� ���� 
	ktime_t time_val;

	//���� �����Ҷ�
	if (flag == 0) {
		time_val = ktime_get();
		//echo pin ���� HIGH ��ȣ ���� �ð� ���
		if (gpio_get_value(ECHO_PIN) == 1) {
			//printk(KERN_INFO "start time recorded %lld\n", (long long int)echo_start_t);
			echo_start_t = time_val;
		}
		//echo pin ���� falling edge �� ������ ��,
		else {
			echo_stop_t = time_val;
				  //���� hold, trig ���� �ٽ� �����ĸ� �߻��� �� 0���� ������
			flag = 1;
		}
	}
	return IRQ_HANDLED;
}

static long dev_ioctl(struct file* file, unsigned cmd, unsigned long arg) {
	//long long int buf;
	int ret;
	struct Data data;
	switch (cmd) {
	case _IO(0, 3):
		data.value = measure();

		ret = copy_to_user((struct Data*)arg, &data, sizeof(struct Data));
		if (ret < 0) {
			printk(KERN_ERR "copy to user error\n");
			return -EFAULT;
		}

		break;
	}

	return 0;

}

static struct file_operations fops = {
  .open = dev_open,
  .release = dev_release,
  .unlocked_ioctl = dev_ioctl,

};

static int hcsr04_init(void) {
	int ret;

	ret = gpio_request(TRIG_PIN, "TRIG");
	if (ret < 0) {
		printk(KERN_ERR "Trig Pin Request failed.\n");
		return -1;
	}

	ret = gpio_request(ECHO_PIN, "ECHO");
	if (ret < 0) {
		printk(KERN_ERR "Echo Pin Request failed.\n");
		return -1;
	}

	ret = gpio_direction_output(TRIG_PIN, 0);
	if (ret < 0) {
		printk(KERN_ERR "Set Trig Pin output failed.\n");
		return -1;
	}

	ret = gpio_direction_input(ECHO_PIN);
	if (ret < 0) {
		printk(KERN_ERR "Set Echo Pin input failed.\n");
		return -1;
	}

	//interrupt ��û
	ret = gpio_to_irq(ECHO_PIN);
	if (ret < 0) {
		printk(KERN_ERR "Echo Pin interrupt request failed.\n");
		return -1;
	}
	else {
		echo_irq = ret;
	}

	//interrupt handler ���
	ret = request_irq(echo_irq, ISR, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "echo", NULL);
	if (ret) {
		printk(KERN_ERR "Enroll Interrupt Handler Error.\n");
		return -1;
	}


	if (register_chrdev(NOD_MAJOR, NOD_NAME, &fops) < 0) {
		printk("INIT FAIL\n");
	}
	printk(KERN_INFO "Init Success\n");
	return 0;
}


static void hcsr04_exit(void) {
	free_irq(echo_irq, NULL);
	gpio_free(TRIG_PIN);
	gpio_free(ECHO_PIN);
	unregister_chrdev(NOD_MAJOR, NOD_NAME);
	printk(KERN_INFO "HC SR04 Exit Success\n");
}

module_init(hcsr04_init);
module_exit(hcsr04_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("mydream11123@naver.com");
MODULE_DESCRIPTION("HC SR04 Ultrasonic Sensor Device Driver");