// gpiobtn.c - Final clean driver (no platform device needed)
#include <linux/module.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DRIVER_NAME "gpiobtn"

static int gpiobtn_value = 1;

static ssize_t gpiobtn_read(struct file *file, char __user *buf,
                            size_t count, loff_t *ppos)
{
    char val = gpiobtn_value;

    if (*ppos > 0)
        return 0;

    if (copy_to_user(buf, &val, 1))
        return -EFAULT;

    *ppos = 1;
    return 1;
}

static const struct file_operations gpiobtn_fops = {
    .owner = THIS_MODULE,
    .read  = gpiobtn_read,
};

static struct miscdevice gpiobtn_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = "gpiobtn",
    .fops  = &gpiobtn_fops,
};

static int __init gpiobtn_init(void)
{
    pr_info("gpiobtn: module loaded\n");
    return misc_register(&gpiobtn_miscdev);
}

static void __exit gpiobtn_exit(void)
{
    pr_info("gpiobtn: module unloaded\n");
    misc_deregister(&gpiobtn_miscdev);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haril");
MODULE_DESCRIPTION("Final GPIO Button Test Driver (No Platform Device Needed)");
MODULE_ALIAS("platform:gpiobtn");

module_init(gpiobtn_init);
module_exit(gpiobtn_exit);

