// file: printk_ratelimit_demo.c
#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Demo: using printk_ratelimit()");
MODULE_VERSION("1.0");

static int __init printk_ratelimit_demo_init(void)
{
    int i;

    printk(KERN_INFO "printk_ratelimit_demo: init\n");

    for (i = 0; i < 1000; i++) {
        if (printk_ratelimit()) {
            printk(KERN_INFO "printk_ratelimit_demo: message %d\n", i);
        }
    }

    return 0;
}

static void __exit printk_ratelimit_demo_exit(void)
{
    printk(KERN_INFO "printk_ratelimit_demo: exit\n");
}

module_init(printk_ratelimit_demo_init);
module_exit(printk_ratelimit_demo_exit);

