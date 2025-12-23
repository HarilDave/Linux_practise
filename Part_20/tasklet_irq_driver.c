#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HARIL");

#define MY_IRQ 11

/* ---------------- Tasklet ---------------- */
static void my_tasklet_fn(struct tasklet_struct *t);

static DECLARE_TASKLET(mytasklet, my_tasklet_fn);

static void my_tasklet_fn(struct tasklet_struct *t)
{
    pr_info("Tasklet executed\n");
}

/* ---------------- IRQ ---------------- */
static irqreturn_t my_irq_handler(int irq, void *dev_id)
{
    pr_info("IRQ received\n");
    tasklet_schedule(&mytasklet);
    return IRQ_HANDLED;
}

/* ---------------- Globals ---------------- */
static dev_t dev;
static struct cdev mycdev;
static struct class *myclass;
static struct kobject *mykobj;
static int my_value;

/* ---------------- File ops ---------------- */
static int my_open(struct inode *i, struct file *f)
{
    pr_info("open\n");
    return 0;
}

static int my_release(struct inode *i, struct file *f)
{
    pr_info("release\n");
    return 0;
}

static ssize_t my_read(struct file *f, char __user *b,
                       size_t l, loff_t *o)
{
    asm volatile("int $0x3B");
    return 0;
}

static ssize_t my_write(struct file *f, const char __user *b,
                        size_t l, loff_t *o)
{
    return l;
}

static struct file_operations fops =
{
    .owner   = THIS_MODULE,
    .open    = my_open,
    .read    = my_read,
    .write   = my_write,
    .release = my_release,
};

/* ---------------- Sysfs ---------------- */
static ssize_t my_show(struct kobject *kobj,
                       struct kobj_attribute *attr,
                       char *buf)
{
    return sprintf(buf, "%d\n", my_value);
}

static ssize_t my_store(struct kobject *kobj,
                        struct kobj_attribute *attr,
                        const char *buf,
                        size_t count)
{
    sscanf(buf, "%d", &my_value);
    return count;
}

static struct kobj_attribute my_attr =
    __ATTR(my_value, 0660, my_show, my_store);

/* ---------------- Init ---------------- */
static int __init my_driver_init(void)
{
    if (alloc_chrdev_region(&dev, 0, 1, "mydevice"))
        return -1;

    cdev_init(&mycdev, &fops);
    if (cdev_add(&mycdev, dev, 1))
        goto err_chrdev;

    myclass = class_create("myclass");
    if (IS_ERR(myclass))
        goto err_cdev;

    device_create(myclass, NULL, dev, NULL, "mydevice");

    mykobj = kobject_create_and_add("my_sysfs", kernel_kobj);
    if (!mykobj)
        goto err_class;

    if (sysfs_create_file(mykobj, &my_attr.attr))
        goto err_kobj;

    if (request_irq(MY_IRQ, my_irq_handler,
                    IRQF_SHARED, "mydevice", &dev))
        goto err_sysfs;

    pr_info("Driver loaded\n");
    return 0;

err_sysfs:
    sysfs_remove_file(mykobj, &my_attr.attr);
err_kobj:
    kobject_put(mykobj);
err_class:
    class_destroy(myclass);
err_cdev:
    cdev_del(&mycdev);
err_chrdev:
    unregister_chrdev_region(dev, 1);
    return -1;
}

/* ---------------- Exit ---------------- */
static void __exit my_driver_exit(void)
{
    tasklet_kill(&mytasklet);
    free_irq(MY_IRQ, &dev);

    sysfs_remove_file(mykobj, &my_attr.attr);
    kobject_put(mykobj);

    device_destroy(myclass, dev);
    class_destroy(myclass);

    cdev_del(&mycdev);
    unregister_chrdev_region(dev, 1);

    pr_info("Driver unloaded\n");
}

module_init(my_driver_init);
module_exit(my_driver_exit);

