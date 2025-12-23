#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <linux/uaccess.h>

static dev_t dev;
static struct cdev mycdev;
static struct class *myclass;
static struct device *mydevice;

static struct workqueue_struct *my_wq;
static struct work_struct *my_work;

static void my_work_handler(struct work_struct *work)
{
    pr_info("Workqueue: Work executed successfully!\n");
}

static ssize_t my_write(struct file *f,
                        const char __user *buf,
                        size_t len,
                        loff_t *ppos)
{
    pr_info("Write called, scheduling work\n");
    queue_work(my_wq, my_work);
    return len;
}

static int my_open(struct inode *i, struct file *f)
{
    pr_info("Device opened\n");
    return 0;
}

static int my_release(struct inode *i, struct file *f)
{
    pr_info("Device closed\n");
    return 0;
}

static struct file_operations fops =
{
    .owner   = THIS_MODULE,
    .open    = my_open,
    .write   = my_write,
    .release = my_release,
};

static int __init my_init(void)
{
    alloc_chrdev_region(&dev, 0, 1, "haril_workqueue");

    cdev_init(&mycdev, &fops);
    cdev_add(&mycdev, dev, 1);

    myclass = class_create("Haril_class");
    mydevice = device_create(myclass, NULL, dev, NULL, "Haril_device");

    my_wq = alloc_workqueue("Haril_wq_dynamic", WQ_UNBOUND, 0);

    my_work = kmalloc(sizeof(struct work_struct), GFP_KERNEL);
    INIT_WORK(my_work, my_work_handler);

    return 0;
}

static void __exit my_exit(void)
{
    flush_workqueue(my_wq);
    destroy_workqueue(my_wq);

    kfree(my_work);

    device_destroy(myclass, dev);
    class_destroy(myclass);

    cdev_del(&mycdev);
    unregister_chrdev_region(dev, 1);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haril");

