#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>

#define DEV_NAME   "myauto"
#define CLASS_NAME "myclass"

static int major;
static struct class *my_class;
static struct device *my_device;

/* ---- minimal file ops ---- */
static int my_open(struct inode *inode, struct file *file) {
    pr_info("myauto: open\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file) {
    pr_info("myauto: release\n");
    return 0;
}

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = my_open,
    .release = my_release,
};

/* ------------ INIT -------------- */
static int __init my_init(void)
{
    pr_info("auto node: init\n");

    /* 1. Register char device (get major number automatically) */
    major = register_chrdev(0, DEV_NAME, &fops);
    if (major < 0) {
        pr_err("Failed to register chrdev\n");
        return major;
    }
    pr_info("Major = %d\n", major);

    /* 2. Create class  */
    my_class = class_create(CLASS_NAME);
    if (IS_ERR(my_class)) {
        unregister_chrdev(major, DEV_NAME);
        return PTR_ERR(my_class);
    }

    /* 3. Create device node automatically ( /dev/myauto ) */
    my_device = device_create(my_class, NULL, MKDEV(major, 0), NULL, DEV_NAME);
    if (IS_ERR(my_device)) {
        class_destroy(my_class);
        unregister_chrdev(major, DEV_NAME);
        return PTR_ERR(my_device);
    }

    return 0;
}

/* ------------ EXIT -------------- */
static void __exit my_exit(void)
{
    device_destroy(my_class, MKDEV(major, 0));
    class_destroy(my_class);
    unregister_chrdev(major, DEV_NAME);

    pr_info("auto node: exit\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");

