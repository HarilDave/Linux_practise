
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/init.h> 
#include <linux/string.h>
#include <linux/proc_fs.h>

#define PROC_NAME "HARIL"
#define MAX_LEN 100

static char message[MAX_LEN];
static int logging_enabled =0;


ssize_t writer_read(struct file *fie,char __user *buf,size_t count,loff_t *ppos)
{
	char state[50];
	int len;


	if(logging_enabled)
		len=sprintf(state,"logging: ON\n");
	else
		len = sprintf(state,"logging : OFF\n");

	return simple_read_from_buffer(buf,count,ppos,state,len);
}
ssize_t writer_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    if (count > MAX_LEN - 1)
        count = MAX_LEN - 1;

    // Copy from user space to kernel space
    if (copy_from_user(message, buf, count))
        return -EFAULT;

    message[count] = '\0';  // Null terminate

    // Remove possible newline at end (from echo)
    if (message[strlen(message) - 1] == '\n')
        message[strlen(message) - 1] = '\0';

    // Check command
    if (strncmp(message, "ON", 2) == 0) {
        logging_enabled = 1;
        printk(KERN_INFO "writer: Logging turned ON\n");
    }
    else if (strncmp(message, "OFF", 3) == 0) {
        logging_enabled = 0;
        printk(KERN_INFO "writer: Logging turned OFF\n");
    }
    else {
        printk(KERN_INFO "writer: Unknown command: %s\n", message);
    }

    return count;
}

// File operations for /proc entry
static const struct proc_ops writer_fops = {
    .proc_read  = writer_read,
    .proc_write = writer_write,
};

// Module init
static int __init proc_write_init(void)
{
    if (!proc_create(PROC_NAME, 0666, NULL, &writer_fops)) {
        printk(KERN_ERR "writer: Failed to create /proc/%s\n", PROC_NAME);
        return -ENOMEM;
    }

    printk(KERN_INFO "writer: /proc/%s created. You can write ON/OFF to it.\n", PROC_NAME);
    return 0;
}

// Module exit
static void __exit proc_write_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "writer: /proc/%s removed.\n", PROC_NAME);
}

module_init(proc_write_init);
module_exit(proc_write_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haril");
MODULE_DESCRIPTION("Proc write logging ON/OFF demo");

