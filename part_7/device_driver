#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define mem_size 1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haril");
MODULE_DESCRIPTION("Simple Char Device Driver");

dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
uint8_t *kernel_buffer;

/* function prototypes */
static int      __init etx_driver_init(void);
static void     __exit etx_driver_exit(void);
static int      etx_open(struct inode *inode, struct file *file);
static int      etx_release(struct inode *inode, struct file *file);
static ssize_t  etx_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t  etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);

/* file operations */
static struct file_operations fops =
{
    .owner   = THIS_MODULE,
    .open    = etx_open,
    .release = etx_release,
    .read    = etx_read,
    .write   = etx_write,
};

/* open */
static int etx_open(struct inode *inode, struct file *file)
{
    pr_info("Device File Opened\n");
    return 0;
}

/* close */
static int etx_release(struct inode *inode, struct file *file)
{
    pr_info("Device File Closed\n");
    return 0;
}

/* read */
static ssize_t etx_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    if (copy_to_user(buf, kernel_buffer, mem_size))
    {
        pr_err("Data Read Error\n");
        return -EFAULT;
    }

    pr_info("Data Read Done\n");
    return mem_size;
}

/* write */
static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    if (copy_from_user(kernel_buffer, buf, len))
    {
        pr_err("Data Write Error\n");
        return -EFAULT;
    }

    pr_info("Data Write Done\n");
    return len;
}

/* init */
static int __init etx_driver_init(void)
{
    if (alloc_chrdev_region(&dev, 0, 1, "etx_Dev") < 0)
    {
        pr_err("Cannot allocate major number\n");
        return -1;
    }

    pr_info("Major = %d Minor = %d\n", MAJOR(dev), MINOR(dev));

    cdev_init(&etx_cdev, &fops);

    if (cdev_add(&etx_cdev, dev, 1) < 0)
    {
        pr_err("Cannot add cdev\n");
        goto r_unreg;
    }

    /* kernel 6.x API */
    dev_class = class_create("etx_class");
    if (IS_ERR(dev_class))
    {
        pr_err("Cannot create class\n");
        goto r_cdev;
    }

    if (IS_ERR(device_create(dev_class, NULL, dev, NULL, "etx_device")))
    {
        pr_err("Cannot create device\n");
        goto r_class;
    }

    kernel_buffer = kmalloc(mem_size, GFP_KERNEL);
    if (!kernel_buffer)
    {
        pr_err("Cannot allocate kernel memory\n");
        goto r_device;
    }

    strcpy(kernel_buffer, "Hello_World");

    pr_info("Device Driver Inserted Successfully\n");
    return 0;

r_device:
    device_destroy(dev_class, dev);
r_class:
    class_destroy(dev_class);
r_cdev:
    cdev_del(&etx_cdev);
r_unreg:
    unregister_chrdev_region(dev, 1);
    return -1;
}

/* exit */
static void __exit etx_driver_exit(void)
{
    kfree(kernel_buffer);
    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    cdev_del(&etx_cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("Device Driver Removed\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

