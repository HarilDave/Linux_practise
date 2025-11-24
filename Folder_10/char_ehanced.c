#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>   // kmalloc, kfree

#define MSG_LEN 100

static char device_name[20] = "securedev";
static int major;

/* Dynamic ring buffer */
static int max_messages = 5;        // module parameter
static char **msg_buffer = NULL;    // pointer to array of char*
static int msg_count = 0;
static int read_index = 0, write_index = 0;

/* Mutex for thread safety */
static DEFINE_MUTEX(buffer_lock);

/* Module parameters */
module_param_string(device_name, device_name, sizeof(device_name), 0660);
MODULE_PARM_DESC(device_name, "Name of the device");

module_param(max_messages, int, 0660);
MODULE_PARM_DESC(max_messages, "Maximum number of messages in buffer");

/* ----------------------- OPEN ----------------------- */
static int dev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "[%s] Device opened\n", device_name);
    return 0;
}

/* ----------------------- CLOSE ----------------------- */
static int dev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "[%s] Device closed\n", device_name);
    return 0;
}

/* ----------------------- WRITE ----------------------- */
static ssize_t dev_write(struct file *file, const char __user *buf,
                         size_t len, loff_t *offset)
{
    if (len > MSG_LEN - 1)
        len = MSG_LEN - 1;

    mutex_lock(&buffer_lock);

    if (msg_count >= max_messages) {
        printk(KERN_WARNING "[%s] Buffer full, message dropped\n", device_name);
        mutex_unlock(&buffer_lock);
        return -ENOMEM;
    }

    if (copy_from_user(msg_buffer[write_index], buf, len)) {
        mutex_unlock(&buffer_lock);
        return -EFAULT;
    }

    msg_buffer[write_index][len] = '\0';

    write_index = (write_index + 1) % max_messages;
    msg_count++;

    mutex_unlock(&buffer_lock);
    return len;
}

/* ----------------------- READ ----------------------- */
static ssize_t dev_read(struct file *file, char __user *buf,
                        size_t len, loff_t *offset)
{
    ssize_t ret;

    mutex_lock(&buffer_lock);

    if (msg_count == 0) {
        mutex_unlock(&buffer_lock);
        return 0;  // no data → EOF
    }

    *offset = 0;

    ret = simple_read_from_buffer(
        buf, len, offset,
        msg_buffer[read_index],
        strlen(msg_buffer[read_index])
    );

    read_index = (read_index + 1) % max_messages;
    msg_count--;

    mutex_unlock(&buffer_lock);
    return ret;
}

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = dev_open,
    .read    = dev_read,
    .write   = dev_write,
    .release = dev_release,
};

/* ----------------------- INIT ----------------------- */
static int __init char_enhanced_init(void)
{
    int i, j;

    major = register_chrdev(0, device_name, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Failed to register driver\n");
        return major;
    }

    msg_buffer = kmalloc_array(max_messages, sizeof(char *), GFP_KERNEL);
    if (!msg_buffer)
        return -ENOMEM;

    for (i = 0; i < max_messages; i++) {
        msg_buffer[i] = kmalloc(MSG_LEN, GFP_KERNEL);
        if (!msg_buffer[i]) {
            for (j = 0; j < i; j++)
                kfree(msg_buffer[j]);
            kfree(msg_buffer);
            return -ENOMEM;
        }
    }

    printk(KERN_INFO "[%s] registered with major %d, max_messages=%d\n",
           device_name, major, max_messages);

    return 0;
}

/* ----------------------- EXIT ----------------------- */
static void __exit char_enhanced_exit(void)
{
    int i;

    for (i = 0; i < max_messages; i++)
        kfree(msg_buffer[i]);

    kfree(msg_buffer);

    unregister_chrdev(major, device_name);

    printk(KERN_INFO "[%s] unregistered\n", device_name);
}

module_init(char_enhanced_init);
module_exit(char_enhanced_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Enhanced char driver with dynamic ring buffer");

