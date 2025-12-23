#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

static dev_t dev;   // kernel fills this

static int __init dynamic_init(void)
{
    // DYNAMIC: kernel chooses major number
    if (alloc_chrdev_region(&dev, 0, 1, "dynamic_dev") < 0) {
        printk(KERN_ERR "Failed to allocate dynamic dev\n");
        return -1;
    }

    printk(KERN_INFO "DYNAMIC -> Major = %d, Minor = %d\n",
           MAJOR(dev), MINOR(dev));

    return 0;
}

static void __exit dynamic_exit(void)
{
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "Dynamic dev removed\n");
}

module_init(dynamic_init);
module_exit(dynamic_exit);

