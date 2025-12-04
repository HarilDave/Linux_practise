#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/mutex.h>
#include <linux/string.h>

#define DEVICE_NAME "polldev"
#define CLASS_NAME  "pollclass"
#define BUF_LEN 256

static dev_t dev_num;
static struct cdev poll_cdev;
static struct class *poll_class;
static struct device *poll_device;

static char buffer[BUF_LEN];
static int data_available = 0;   // 1 = there is data to read
static int can_write = 1;        // 1 = driver ready to accept write()

static DECLARE_WAIT_QUEUE_HEAD(wq);
static DEFINE_MUTEX(lock);

static int dev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "[%s] opened\n", DEVICE_NAME);
    return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "[%s] closed\n", DEVICE_NAME);
    return 0;
}

static ssize_t dev_read(struct file *file, char __user *buf, size_t len, loff_t *off)
{
    int ret;

    // Block until data_available becomes non-zero
    if (wait_event_interruptible(wq, data_available))
        return -ERESTARTSYS;

    mutex_lock(&lock);

    // Reset offset so every read starts from beginning for simplicity
    *off = 0;

    ret = simple_read_from_buffer(buf, len, off, buffer, strlen(buffer));

    // After reading, no more data, and we allow write again
    data_available = 0;
    can_write = 1;

    mutex_unlock(&lock);

    // Wake up any poll() waiters
    wake_up_interruptible(&wq);

    return ret;
}

static ssize_t dev_write(struct file *file, const char __user *buf, size_t len, loff_t *off)
{
    if (!can_write) {
        // Simulate buffer full: non-blocking writers will get EAGAIN
        return -EAGAIN;
    }

    if (len > BUF_LEN - 1)
        len = BUF_LEN - 1;

    mutex_lock(&lock);

    if (copy_from_user(buffer, buf, len)) {
        mutex_unlock(&lock);
        return -EFAULT;
    }

    buffer[len] = '\0';

    data_available = 1;
    can_write = 0;   // After one write, consider buffer full until read

    mutex_unlock(&lock);

    // Wake processes waiting in read() or poll()
    wake_up_interruptible(&wq);

    printk(KERN_INFO "[%s] wrote: %s\n", DEVICE_NAME, buffer);
    return len;
}

static __poll_t dev_poll(struct file *file, poll_table *wait)
{
    __poll_t mask = 0;

    // Register this file descriptor with the wait queue
    poll_wait(file, &wq, wait);

    // Ready to read?
    if (data_available)
        mask |= POLLIN | POLLRDNORM;

    // Ready to write?
    if (can_write)
        mask |= POLLOUT | POLLWRNORM;

    return mask;
}

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = dev_open,
    .release = dev_release,
    .read    = dev_read,
    .write   = dev_write,
    .poll    = dev_poll,
};

static int __init poll_driver_init(void)
{
    int ret;

    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        printk(KERN_ERR "[%s] failed to alloc_chrdev_region\n", DEVICE_NAME);
        return ret;
    }

    cdev_init(&poll_cdev, &fops);
    poll_cdev.owner = THIS_MODULE;

    ret = cdev_add(&poll_cdev, dev_num, 1);
    if (ret < 0) {
        unregister_chrdev_region(dev_num, 1);
        printk(KERN_ERR "[%s] failed to cdev_add\n", DEVICE_NAME);
        return ret;
    }

    poll_class = class_create(CLASS_NAME);
    if (IS_ERR(poll_class)) {
        cdev_del(&poll_cdev);
        unregister_chrdev_region(dev_num, 1);
        printk(KERN_ERR "[%s] failed to create class\n", DEVICE_NAME);
        return PTR_ERR(poll_class);
    }

    poll_device = device_create(poll_class, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(poll_device)) {
        class_destroy(poll_class);
        cdev_del(&poll_cdev);
        unregister_chrdev_region(dev_num, 1);
        printk(KERN_ERR "[%s] failed to create device\n", DEVICE_NAME);
        return PTR_ERR(poll_device);
    }

    printk(KERN_INFO "[%s] loaded: major=%d minor=%d\n",
           DEVICE_NAME, MAJOR(dev_num), MINOR(dev_num));

    return 0;
}

static void __exit poll_driver_exit(void)
{
    device_destroy(poll_class, dev_num);
    class_destroy(poll_class);
    cdev_del(&poll_cdev);
    unregister_chrdev_region(dev_num, 1);
    printk(KERN_INFO "[%s] unloaded\n", DEVICE_NAME);
}

module_init(poll_driver_init);
module_exit(poll_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Character driver with poll() support for POLLIN and POLLOUT");

