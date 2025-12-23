#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

static dev_t dev;   // holds major+minor

static int __init static_init(void)
{
    dev = MKDEV(120, 30);       // STATIC: you choose major & minor

    if (register_chrdev_region(dev, 1, "static_dev") < 0) {
        printk(KERN_ERR "Failed to register static dev\n");
        return -1;
    }

    printk(KERN_INFO "STATIC -> Major = %d, Minor = %d\n",
           MAJOR(dev), MINOR(dev));

    return 0;
}

static void __exit static_exit(void)
{
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "Static dev removed\n");
}

module_init(static_init);
module_exit(static_exit);

