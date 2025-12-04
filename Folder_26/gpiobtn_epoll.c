#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");

/* ===== device info ===== */
#define DEVICE_NAME "gpiobtn"
#define CLASS_NAME  "gpiocls"

static dev_t dev_num;
static struct cdev cdev;
static struct class *cls;
static struct device *dev;

/* ===== driver state ===== */
static int press_count;
static int data_ready;

/* ===== debounce ===== */
static unsigned long last_press_jiffies;
static unsigned int debounce_ms = 50;

/* ===== wait queue ===== */
static DECLARE_WAIT_QUEUE_HEAD(wq);

/* ===== read ===== */
static ssize_t gpiobtn_read(struct file *f,
                            char __user *buf,
                            size_t len,
                            loff_t *off)
{
    char msg[64];
    int l;

    wait_event_interruptible(wq, data_ready);

    l = snprintf(msg, sizeof(msg),
                 "Press Count: %d\n", press_count);

    data_ready = 0;
    *off = 0;

    if (copy_to_user(buf, msg, l))
        return -EFAULT;

    return l;
}

/* ===== write (simulated button press) ===== */
static ssize_t gpiobtn_write(struct file *f,
                             const char __user *buf,
                             size_t len,
                             loff_t *off)
{
    unsigned long now = jiffies;

    if (time_before(now,
        last_press_jiffies + msecs_to_jiffies(debounce_ms)))
        return len;   // debounce: ignore

    last_press_jiffies = now;

    press_count++;
    data_ready = 1;
    wake_up_interruptible(&wq);

    return len;
}

/* ===== poll ===== */
static __poll_t gpiobtn_poll(struct file *f, poll_table *wait)
{
    poll_wait(f, &wq, wait);

    if (data_ready)
        return POLLIN | POLLRDNORM;

    return 0;
}

/* ===== fops ===== */
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read  = gpiobtn_read,
    .write = gpiobtn_write,
    .poll  = gpiobtn_poll,
};

/* ===== init ===== */
static int __init gpiobtn_init(void)
{
    alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    cdev_init(&cdev, &fops);
    cdev_add(&cdev, dev_num, 1);

    cls = class_create(CLASS_NAME);
    dev = device_create(cls, NULL, dev_num,
                        NULL, DEVICE_NAME);
    return 0;
}

/* ===== exit ===== */
static void __exit gpiobtn_exit(void)
{
    device_destroy(cls, dev_num);
    class_destroy(cls);
    cdev_del(&cdev);
    unregister_chrdev_region(dev_num, 1);
}

module_init(gpiobtn_init);
module_exit(gpiobtn_exit);


