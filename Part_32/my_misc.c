#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

static int my_open(struct inode *inode, struct file *file)
{
    pr_info("miscdevice: open\n");
    return 0;
}

static int my_close(struct inode *inode, struct file *file)
{
    pr_info("miscdevice: close\n");
    return 0;
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *ppos)
{
    pr_info("miscdevice: write\n");
    return len;
}

static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *ppos)
{
    pr_info("miscdevice: read\n");
    return 0;
}

static const struct file_operations my_fops = 
{
    .owner   = THIS_MODULE,
    .open    = my_open,
    .release = my_close,
    .read    = my_read,
    .write   = my_write,
    .llseek  = no_llseek,
};

static struct miscdevice my_misc_device = 
{
    .minor = MISC_DYNAMIC_MINOR,
    .name  = "my_misc",
    .fops  = &my_fops,
};

static int __init my_misc_init(void)
{
    int ret = misc_register(&my_misc_device);
    if (ret) 
    {
        pr_err("misc_register failed\n");
        return ret;
    }

    pr_info("misc device registered\n");
    return 0;
}

static void __exit my_misc_exit(void)
{
    misc_deregister(&my_misc_device);
    pr_info("misc device removed\n");
}

module_init(my_misc_init);
module_exit(my_misc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HARIL");

