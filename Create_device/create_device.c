#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>

#define DEV_NAME   "mydevice"
#define CLASS_NAME "myclass"

static int major;
static struct class *my_class;
static struct device *my_device;

/* -------- Minimal file operations -------- */
static int dummy_open(struct inode *inode, struct file *file)
{
    pr_info("mydevice: open\n");
    return 0;
}

static int dummy_release(struct inode *inode, struct file *file)
{
    pr_info("mydevice: release\n");
    return 0;
}

static ssize_t dummy_read(struct file *file, char __user *buf, size_t len, loff_t *off)
{
    pr_info("mydevice: read\n");
    return 0;
}

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = dummy_open,
    .release = dummy_release,
    .read    = dummy_read,
};

/* -------------------- Init ------------------------ */
static int __init my_init(void)
{
    pr_info("create_device: init\n");

    // 1. register char device
    major = register_chrdev(0, DEV_NAME, &fops);
    if (major < 0) {
        pr_err("Failed to register chrdev\n");
        return major;
    }
    pr_info("Major number = %d\n", major);

    // 2. create class  (OLD PROTOTYPE — only takes class name)
    my_class = class_create(CLASS_NAME);
    if (IS_ERR(my_class)) {
        unregister_chrdev(major, DEV_NAME);
        return PTR_ERR(my_class);
    }

    // 3. create device node
    my_device = device_create(my_class, NULL, MKDEV(major, 0), NULL, DEV_NAME);
    if (IS_ERR(my_device)) {
        class_destroy(my_class);
        unregister_chrdev(major, DEV_NAME);
        return PTR_ERR(my_device);
    }

    pr_info("/dev/%s created\n", DEV_NAME);
    return 0;
}

/* -------------------- Exit ------------------------ */
static void __exit my_exit(void)
{
    device_destroy(my_class, MKDEV(major, 0));
    class_destroy(my_class);
    unregister_chrdev(major, DEV_NAME);

    pr_info("create_device: exit\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");

