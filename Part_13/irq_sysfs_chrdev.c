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

#define IRQ_NO 11

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HARIL");

static dev_t dev;
static struct cdev mycdev;
static struct class *myclass;
static struct device *mydevice;
static struct kobject *mykobj;

static int value = 0;

/* ---------------- IRQ ---------------- */
static irqreturn_t irq_handler(int irq, void *dev_id)
{
    pr_info("IRQ %d triggered\n", irq);
    return IRQ_HANDLED;
}

/* ---------------- SYSFS ---------------- */
static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", value);
}

static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    sscanf(buf, "%d", &value);
    return count;
}

static struct kobj_attribute value_attr = __ATTR(value, 0660, sysfs_show, sysfs_store);

/* ---------------- FILE OPS ---------------- */
static int myopen(struct inode *inode, struct file *file)
{
    pr_info("Device opened\n");
    return 0;
}

static int myrelease(struct inode *inode, struct file *file)
{
    pr_info("Device closed\n");
    return 0;
}

static ssize_t myread(struct file *file, char __user *buf, size_t len, loff_t *ppos)
{
    asm volatile("int $0x3B");
    return 0;
}

static ssize_t mywrite(struct file *file, const char __user *buf, size_t len, loff_t *ppos)
{
    return len;
}

static struct file_operations fops =
{
    .owner   = THIS_MODULE,
    .open    = myopen,
    .release = myrelease,
    .read    = myread,
    .write   = mywrite,
};

/* ---------------- INIT ---------------- */
static int __init myinit(void)
{
    int ret;

    ret = alloc_chrdev_region(&dev, 0, 1, "my_chr_dev");
    if (ret)
        return ret;

    cdev_init(&mycdev, &fops);
    ret = cdev_add(&mycdev, dev, 1);
    if (ret)
        goto err_unreg;

    myclass = class_create("myclass");
    if (IS_ERR(myclass)) {
        ret = PTR_ERR(myclass);
        goto err_cdev;
    }

    mydevice = device_create(myclass, NULL, dev, NULL, "mydevice");
    if (IS_ERR(mydevice)) {
        ret = PTR_ERR(mydevice);
        goto err_class;
    }

    mykobj = kobject_create_and_add("mykobj", kernel_kobj);
    if (!mykobj) {
        ret = -ENOMEM;
        goto err_device;
    }

    ret = sysfs_create_file(mykobj, &value_attr.attr);
    if (ret)
        goto err_kobj;

    ret = request_irq(IRQ_NO,
                      irq_handler,
                      IRQF_SHARED,
                      "mydevice",
                      (void *)&mycdev);
    if (ret)
        goto err_sysfs;

    pr_info("Driver loaded\n");
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
err_unreg:
    unregister_chrdev_region(dev, 1);
    return ret;
}

/* ---------------- EXIT ---------------- */
static void __exit myexit(void)
{
    free_irq(IRQ_NO, (void *)&mycdev);
    sysfs_remove_file(mykobj, &value_attr.attr);
    kobject_put(mykobj);
    device_destroy(myclass, dev);
    class_destroy(myclass);
    cdev_del(&mycdev);
    unregister_chrdev_region(dev, 1);
    pr_info("Driver removed\n");
}

module_init(myinit);
module_exit(myexit);

