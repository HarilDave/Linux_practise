#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/jiffies.h>

static char *author = "Unknown";
module_param(author, charp, 0660);

static int counter = 0;

static int myinfo_show(struct seq_file *m, void *v)
{
    unsigned long uptime_sec = jiffies / HZ;   

    counter++;

    seq_printf(m, "Author: %s\n", author);
    seq_printf(m, "System Uptime: %lu seconds\n", uptime_sec);
    seq_printf(m, "File Read Counter: %d\n", counter);

    return 0;
}

static int myinfo_open(struct inode *inode, struct file *file)
{
    return single_open(file, myinfo_show, NULL);
}

static const struct proc_ops myinfo_fops = {
    .proc_open    = myinfo_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

static int __init myinfo_init(void)
{
    proc_create("myinfo", 0666, NULL, &myinfo_fops);
    printk(KERN_INFO "/proc/myinfo created\n");
    return 0;
}

static void __exit myinfo_exit(void)
{
    remove_proc_entry("myinfo", NULL);
    printk(KERN_INFO "/proc/myinfo removed\n");
}

module_init(myinfo_init);
module_exit(myinfo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Custom /proc display module");

