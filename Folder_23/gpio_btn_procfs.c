#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/ktime.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("procfs status interface example");

/* ===== Driver state ===== */
static int press_count = 0;
static int led_state = 0;

/* proc entry */
static struct proc_dir_entry *proc_entry;

/* ===== proc show ===== */
static int gpiobtn_proc_show(struct seq_file *m, void *v)
{
    u64 uptime = ktime_get_boottime_seconds();

    seq_printf(m, "Driver: GPIO Button + LED\n");
    seq_printf(m, "Press Count: %d\n", press_count);
    seq_printf(m, "LED State: %s\n", led_state ? "ON" : "OFF");
    seq_printf(m, "Uptime (s): %llu\n", uptime);

    return 0;
}

/* ===== proc open ===== */
static int gpiobtn_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, gpiobtn_proc_show, NULL);
}

/* ===== proc ops ===== */
static const struct proc_ops gpiobtn_proc_ops = {
    .proc_open    = gpiobtn_proc_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

/* ===== module init ===== */
static int __init gpiobtn_init(void)
{
    proc_entry = proc_create("gpiobtn_info", 0444, NULL,
                             &gpiobtn_proc_ops);
    if (!proc_entry)
        return -ENOMEM;

    pr_info("procfs driver loaded\n");
    return 0;
}

/* ===== module exit ===== */
static void __exit gpiobtn_exit(void)
{
    proc_remove(proc_entry);
    pr_info("procfs driver removed\n");
}

module_init(gpiobtn_init);
module_exit(gpiobtn_exit);

