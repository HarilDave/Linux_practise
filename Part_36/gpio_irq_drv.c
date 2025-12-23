#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>

#define GPIO_LED     21
#define GPIO_BUTTON  25

#define DEBOUNCE

static dev_t dev;
static struct class *myclass;
static struct device *mydevice;
static struct cdev mycdev;

static unsigned int gpio_irq;
static unsigned int led_state;
static unsigned long last_jiffies;

static int myopen(struct inode *inode, struct file *file);
static int myrelease(struct inode *inode, struct file *file);
static ssize_t myread(struct file *file, char __user *buf, size_t len, loff_t *ppos);
static ssize_t mywrite(struct file *file, const char __user *buf, size_t len, loff_t *ppos);

static struct file_operations fops =
{
    .owner   = THIS_MODULE,
    .open    = myopen,
    .release = myrelease,
    .read    = myread,
    .write   = mywrite,
};

static irqreturn_t button_irq_handler(int irq,void *data)
{
	#ifdef DEBOUNCE
		if (jiffies - last_jiffies <20)
			return IRQ_HANDLED;
		
		last_jiffies = jiffies;
	#endif
		led_state ^= 1;
		gpio_set_value(GPIO_LED,led_state);
		
		return IRQ_HANDLED;
}

static int myopen(struct inode *inode, struct file *file)
{
    pr_info("GPIO interrupt driver opened\n");
    return 0;
}

static int myrelease(struct inode *inode, struct file *file)
{
    pr_info("GPIO interrupt driver closed\n");
    return 0;
}

static ssize_t myread(struct file *file, char __user *buf, size_t len, loff_t *ppos)
{
	int value;
	
	if(*ppos>0)
	{
		return 0;
	}
	
	value = gpio_get_value(GPIO_LED);
	
	if(copy_to_user(buf,&value,sizeof(int)))
	{
		return -EFAULT;
	}
	
	*ppos = 1;
	
	return 1;
}

static ssize_t mywrite(struct file *file, const char __user *buf, size_t len, loff_t *ppos)
{
	char val;
	
	if(copy_from_user(&val,buf,sizeof(char)))
	{
		return -EFAULT;
	}
	
	if (val == '1')
        	gpio_set_value(GPIO_LED, 1);
    
    	else if (val == '0')
        	gpio_set_value(GPIO_LED, 0);

    	return len;
}

static int __init gpio_init(void)
{
    alloc_chrdev_region(&dev, 0, 1, "gpio_dev");

    cdev_init(&mycdev, &fops);
    cdev_add(&mycdev, dev, 1);

    myclass = class_create("gpio_class");
    mydevice = device_create(myclass, NULL, dev, NULL, "gpio_device");

    gpio_request(GPIO_LED,"gpio_led");
    gpio_direction_output(GPIO_LED,0);
    
    gpio_request(GPIO_BUTTON,"gpio_button");
    gpio_direction_input(GPIO_BUTTON);
    
    gpio_irq = gpio_to_irq(GPIO_BUTTON);
    request_irq(gpio_irq,button_irq_handler,IRQF_TRIGGER_RISING,"gpio_irq",NULL);

    pr_info("GPIO module loaded\n");
    return 0;
}

static void __exit gpio_exit(void)
{
	free_irq(gpio_irq,NULL);
	
	gpio_free(GPIO_BUTTON);
	gpio_free(GPIO_LED);
	
	device_destroy(myclass,dev);
	class_destroy(myclass);
	
	cdev_del(&mycdev);
	unregister_chrdev_region(dev,1);
	
	pr_info("GPIO interrupt driver unloaded\n");
}

module_init(gpio_init);
module_exit(gpio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HARIL");
