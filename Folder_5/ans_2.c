#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/timekeeping.h>

static int __init hello_init(void)
{
    time64_t now = ktime_get_real_seconds();
    printk(KERN_INFO "Module Loaded by: Haril\n");
    printk(KERN_INFO "Current Time (seconds since Epoch): %lld\n", now);
    return 0;
}

static void __exit hello_exit(void)
{
    printk(KERN_INFO "Goodbye! Module %s removed.\n", THIS_MODULE->name);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haril");
MODULE_DESCRIPTION("Module printing name and time");

