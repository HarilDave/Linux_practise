#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/kobject.h>
#include <linux/string.h>

#define DEV_NAME   "mydevice"
#define CLASS_NAME "myclass"

/* -------- Global objects -------- */
static dev_t dev;
static struct cdev mycdev;
static struct class *myclass;
static struct device *mydevice;
static struct kobject *poll_kobj;

/* -------- Wait queue -------- */
DECLARE_WAIT_QUEUE_HEAD(poll_wq);

/* -------- Flags -------- */
static bool data_ready = false;
static bool space_ready = false;

/* -------- Data -------- */
static char data_buf[20];

/* -------- File operations -------- */
static int myopen(struct inode *inode, struct file *file)
{
    pr_info("Device opened\n");
    return 0;
}

static int myclose(struct inode *inode, struct file *file)
{
    pr_info("Device closed\n");
    return 0;
}

static ssize_t myread(struct file *file, char __user *buf, size_t len, loff_t *off)
{
    if (!data_ready)
        return 0;

    len = strlen(data_buf);

    if (copy_to_user(buf, data_buf, len))
        return -EFAULT;

    data_ready = false;
    return len;
}

static ssize_t mywrite(struct file *file, const char __user *buf, size_t len, loff_t *off)
{
    if (len >= sizeof(data_buf))
        len = sizeof(data_buf) - 1;

    if (copy_from_user(data_buf, buf, len))
        return -EFAULT;

    data_buf[len] = '\0';
    data_ready = true;      
    space_ready = true;

    wake_up(&poll_wq);
    return len;
}

static unsigned int mypoll(struct file *file, poll_table *wait)
{
    unsigned int mask = 0;

    poll_wait(file, &poll_wq, wait);

    if (data_ready)
        mask |= POLLIN | POLLRDNORM;

    if (space_ready)
        mask |= POLLOUT | POLLWRNORM;

    space_ready = false;
    return mask;
}

static struct file_operations fops =
{
    .owner   = THIS_MODULE,
    .open    = myopen,
    .release = myclose,
    .read    = myread,
    .write   = mywrite,
    .poll    = mypoll,
};

/* -------- Sysfs -------- */
static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    data_ready = true;
    wake_up(&poll_wq);
    return sprintf(buf, "ok\n");
}

static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    strncpy(data_buf, buf, sizeof(data_buf));
    data_ready = true;
    wake_up(&poll_wq);
    return count;
}

static struct kobj_attribute poll_attr = __ATTR(data, 0660, sysfs_show, sysfs_store);

/* -------- Init -------- */
static int __init poll_init(void)
{
    alloc_chrdev_region(&dev, 0, 1, DEV_NAME);

    cdev_init(&mycdev, &fops);
    cdev_add(&mycdev, dev, 1);

    myclass = class_create(CLASS_NAME);
    mydevice = device_create(myclass, NULL, dev, NULL, DEV_NAME);

    poll_kobj = kobject_create_and_add("poll_sysfs", kernel_kobj);
    sysfs_create_file(poll_kobj, &poll_attr.attr);

    pr_info("Poll driver loaded\n");
    return 0;
}

/* -------- Exit -------- */
static void __exit poll_exit(void)
{
    sysfs_remove_file(poll_kobj, &poll_attr.attr);
    kobject_put(poll_kobj);

    device_destroy(myclass, dev);
    class_destroy(myclass);

    cdev_del(&mycdev);
    unregister_chrdev_region(dev, 1);

    pr_info("Poll driver unloaded\n");
}

module_init(poll_init);
module_exit(poll_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haril");

