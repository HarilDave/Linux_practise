#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HARIL");

#define IRQ_NO 11

static struct work_struct mywork;

static void my_work_fn(struct work_struct *work)
{
    printk(KERN_INFO "execution workqueue\n");
}

static irqreturn_t irq_handler(int irq, void *dev_id)
{
    printk(KERN_INFO "IRQ %d triggered\n", irq);
    schedule_work(&mywork);
    return IRQ_HANDLED;
}

static dev_t dev;
static struct cdev mycdev;
static struct class *myclass;
static struct device *mydevice;
static struct kobject *mykobj;

static int myopen(struct inode *, struct file *);
static int myrelease(struct inode *, struct file *);
static ssize_t myread(struct file *, char __user *, size_t, loff_t *);
static ssize_t mywrite(struct file *, const char __user *, size_t, loff_t *);

static int value = 0;

static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", value);
}

static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr,
                           const char *buf, size_t count)
{
    sscanf(buf, "%d", &value);
    return count;
}

static struct kobj_attribute value_attr =
    __ATTR(value, 0660, sysfs_show, sysfs_store);

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
    printk(KERN_INFO "Device opened\n");
    return 0;
}

static int myrelease(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Device closed\n");
    return 0;
}

static ssize_t myread(struct file *file, char __user *buf,
                      size_t len, loff_t *pos)
{
    asm("int $0x3B");
    return 0;
}

static ssize_t mywrite(struct file *file, const char __user *buf,
                       size_t len, loff_t *pos)
{
    return len;
}

static int __init myinit(void)
{
    if (alloc_chrdev_region(&dev, 0, 1, "mydevice") < 0)
        return -1;

    cdev_init(&mycdev, &fops);
    if (cdev_add(&mycdev, dev, 1) < 0)
        goto err_unregister;

    myclass = class_create("myclass");
    if (IS_ERR(myclass))
        goto err_cdev;

    mydevice = device_create(myclass, NULL, dev, NULL, "mydevice");
    if (IS_ERR(mydevice))
        goto err_class;

    mykobj = kobject_create_and_add("myobj", kernel_kobj);
    if (!mykobj)
        goto err_device;

    if (sysfs_create_file(mykobj, &value_attr.attr))
        goto err_kobj;

    INIT_WORK(&mywork, my_work_fn);

    if (request_irq(IRQ_NO, irq_handler, IRQF_SHARED,
                    "my_irq_shared", &mycdev))
        goto err_sysfs;

    printk(KERN_INFO "Dynamic Workqueue Driver Loaded\n");
    return 0;

err_sysfs:
    sysfs_remove_file(mykobj, &value_attr.attr);

err_kobj:
    kobject_put(mykobj);

err_device:
    device_destroy(myclass, dev);

err_class:
    class_destroy(myclass);

err_cdev:
    cdev_del(&mycdev);

err_unregister:
    unregister_chrdev_region(dev, 1);

    return -1;
}

static void __exit myexit(void)
{
    free_irq(IRQ_NO, &mycdev);
    sysfs_remove_file(mykobj, &value_attr.attr);
    kobject_put(mykobj);
    device_destroy(myclass, dev);
    class_destroy(myclass);
    cdev_del(&mycdev);
    unregister_chrdev_region(dev, 1);

    printk(KERN_INFO "Dynamic Workqueue Driver UnLoaded\n");
}

module_init(myinit);
module_exit(myexit);

