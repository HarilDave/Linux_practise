#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/sysfs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HARIL");

static int value = 0;

static dev_t dev;
static struct cdev mycdev;
static struct class *myclass;
static struct device *mydevice;
static struct kobject *my_kobj;

static int dev_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int dev_close(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t dev_read(struct file *f, char __user *b, size_t l, loff_t *o)
{
    return 0;
}

static ssize_t dev_write(struct file *f, const char __user *b, size_t l, loff_t *o)
{
    return l;
}

static ssize_t sys_show(struct kobject *k, struct kobj_attribute *a, char *buf)
{
    return sprintf(buf, "%d\n", value);
}

static ssize_t sys_store(struct kobject *k, struct kobj_attribute *a, const char *buf, size_t count)
{
    sscanf(buf, "%d", &value);
    return count;
}

static struct kobj_attribute value_attr = __ATTR(value, 0664, sys_show, sys_store);

static struct file_operations fops =
{
    .owner   = THIS_MODULE,
    .open    = dev_open,
    .read    = dev_read,
    .write   = dev_write,
    .release = dev_close,
};

static int __init my_driver_init(void)
{
    int ret;

    alloc_chrdev_region(&dev, 0, 1, "my_dev");

    cdev_init(&mycdev, &fops);
    cdev_add(&mycdev, dev, 1);

    myclass = class_create("my_class");
    mydevice = device_create(myclass, NULL, dev, NULL, "my_device");

    my_kobj = kobject_create_and_add("my_sysfs_test", kernel_kobj);
    if (!my_kobj)
        return -ENOMEM;

    ret = sysfs_create_file(my_kobj, &value_attr.attr);
    if (ret) {
        kobject_put(my_kobj);
        return ret;
    }

    return 0;
}

static void __exit my_driver_exit(void)
{
    sysfs_remove_file(my_kobj, &value_attr.attr);
    kobject_put(my_kobj);

    device_destroy(myclass, dev);
    class_destroy(myclass);

    cdev_del(&mycdev);
    unregister_chrdev_region(dev, 1);
}

module_init(my_driver_init);
module_exit(my_driver_exit);

