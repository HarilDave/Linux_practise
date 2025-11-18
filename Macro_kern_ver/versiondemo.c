#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/version.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haril");
MODULE_DESCRIPTION("Kernel version demo module");

static int __init versiondemo_init(void)
{
    printk(KERN_INFO "versiondemo: module loaded\n");

    printk(KERN_INFO "LINUX_VERSION_CODE = %u\n", LINUX_VERSION_CODE);

    printk(KERN_INFO "KERNEL_VERSION(6,8,0) = %u\n",
           KERNEL_VERSION(6, 8, 0));

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6,8,0)
    printk(KERN_INFO "Running on kernel >= 6.8.0\n");
#else
    printk(KERN_INFO "Running on kernel < 6.8.0\n");
#endif

    return 0;
}

static void __exit versiondemo_exit(void)
{
    printk(KERN_INFO "versiondemo: module unloaded\n");
}

module_init(versiondemo_init);
module_exit(versiondemo_exit);

