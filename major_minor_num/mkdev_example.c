#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

static int __init mkdev_example_init(void)
{
    dev_t dev;

    dev = MKDEV(120, 30);

    printk(KERN_INFO "Major = %d\n", MAJOR(dev));
    printk(KERN_INFO "Minor = %d\n", MINOR(dev));

    return 0;
}

static void __exit mkdev_example_exit(void)
{
    printk(KERN_INFO "Module unloaded\n");
}

module_init(mkdev_example_init);
module_exit(mkdev_example_exit);

