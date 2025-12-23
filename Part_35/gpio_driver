#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

#define GPIO_NUM 53

static dev_t dev;
static struct class *myclass;
static struct device *mydevice;
static struct cdev mycdev;

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

static int myopen(struct inode *inode, struct file *file)
{
    pr_info("GPIO driver opened\n");
    return 0;
}

static int myrelease(struct inode *inode, struct file *file)
{
    pr_info("GPIO driver closed\n");
    return 0;
}

static ssize_t myread(struct file *file, char __user *buf, size_t len, loff_t *ppos)
{
    int value;

    if (*ppos > 0)
        return 0;

    value = gpio_get_value(GPIO_NUM);

    if (copy_to_user(buf, &value, 1))
        return -EFAULT;

    *ppos = 1;
    return 1;
}

static ssize_t mywrite(struct file *file, const char __user *buf, size_t len, loff_t *ppos)
{
    char val;

    if (copy_from_user(&val, buf, 1))
        return -EFAULT;

    if (val == '1')
        gpio_set_value(GPIO_NUM, 1);
    
    else if (val == '0')
        gpio_set_value(GPIO_NUM, 0);

    return len;
}

static int __init gpio_init(void)
{
    alloc_chrdev_region(&dev, 0, 1, "gpio_dev");

    cdev_init(&mycdev, &fops);
    cdev_add(&mycdev, dev, 1);

    myclass = class_create("gpio_class");
    mydevice = device_create(myclass, NULL, dev, NULL, "gpio_device");

    gpio_request(GPIO_NUM, "gpio21");
    gpio_direction_output(GPIO_NUM, 0);
    //gpio_export(GPIO_NUM, false);

    pr_info("GPIO module loaded\n");
    return 0;
}

static void __exit gpio_exit(void)
{
    //gpio_unexport(GPIO_NUM);
    gpio_free(GPIO_NUM);

    device_destroy(myclass, dev);
    class_destroy(myclass);

    cdev_del(&mycdev);
    unregister_chrdev_region(dev, 1);

    pr_info("GPIO module unloaded\n");
}

module_init(gpio_init);
module_exit(gpio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HARIL");

