#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/poll.h>
#include <linux/wait.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Char device with poll() support");

/* ===== device info ===== */
#define DEVICE_NAME "gpiobtn"
#define CLASS_NAME  "gpiobtnclass"

static dev_t dev_num;
static struct cdev gpiobtn_cdev;
static struct class *gpiobtn_class;
static struct device *gpiobtn_device;

/* ===== driver state ===== */
static int press_count;
static int data_ready;

/* ===== wait queue ===== */
static DECLARE_WAIT_QUEUE_HEAD(wq);

/* ===== file ops ===== */
static int gpiobtn_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int gpiobtn_release(struct inode *inode, struct file *file)
{
    return 0;
}

/* ===== read (blocks until event) ===== */
static ssize_t gpiobtn_read(struct file *file,
                            char __user *buf,
                            size_t len, loff_t *off)
{
    char msg[64];
    int msg_len;

    wait_event_interruptible(wq, data_ready);

    msg_len = snprintf(msg, sizeof(msg),
                       "Press Count: %d\n", press_count);

    data_ready = 0;
    *off = 0;  // ✅ FIX: reset offset after event

    if (copy_to_user(buf, msg, msg_len))
        return -EFAULT;

    return msg_len;
}
























/* ===== write (simulate button press) ===== */
static ssize_t gpiobtn_write(struct file *file,
                             const char __user *buf,
                             size_t len, loff_t *off)
{
    press_count++;
    data_ready = 1;
    wake_up_interruptible(&wq);

    return len;
}

/* ===== poll ===== */
static __poll_t gpiobtn_poll(struct file *file, poll_table *wait)
{
    poll_wait(file, &wq, wait);

    if (data_ready)
        return POLLIN | POLLRDNORM;

    return 0;
}

/* ===== fops ===== */
static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = gpiobtn_open,
    .release = gpiobtn_release,
    .read    = gpiobtn_read,
    .write   = gpiobtn_write,
    .poll    = gpiobtn_poll,
};

/* ===== init ===== */
static int __init gpiobtn_init(void)
{
    alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);

    cdev_init(&gpiobtn_cdev, &fops);
    cdev_add(&gpiobtn_cdev, dev_num, 1);

    gpiobtn_class = class_create(CLASS_NAME);
    gpiobtn_device =
        device_create(gpiobtn_class, NULL,
                      dev_num, NULL, DEVICE_NAME);

    return 0;
}

/* ===== exit ===== */
static void __exit gpiobtn_exit(void)
{
    device_destroy(gpiobtn_class, dev_num);
    class_destroy(gpiobtn_class);
    cdev_del(&gpiobtn_cdev);
    unregister_chrdev_region(dev_num, 1);
}

module_init(gpiobtn_init);
module_exit(gpiobtn_exit);

