#include <linux/module.h>
#include <linux/kernel.h>

static int counter = 0;

static int __init counter_init(void)
{
    counter++;
    printk(KERN_INFO "counter.c: Module loaded. Count = %d\n", counter);
    return 0;
}

static void __exit counter_exit(void)
{
    printk(KERN_INFO "counter.c: Module unloaded. Final count = %d\n", counter);
}

module_init(counter_init);
module_exit(counter_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haril");
MODULE_DESCRIPTION("A module that counts its loads");

