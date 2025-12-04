#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Simple char device: /dev/gpiobtn");

/* ===== IOCTL definitions ===== */
#define GPIOBTN_MAGIC 'G'
#define LED_ON  _IO(GPIOBTN_MAGIC, 0)
#define LED_OFF _IO(GPIOBTN_MAGIC, 1)

/* ===== Device info ===== */
#define DEVICE_NAME "gpiobtn"
#define CLASS_NAME  "gpiobtnclass"

static dev_t dev_num;
static struct cdev gpiobtn_cdev;
static struct class *gpiobtn_class;
static struct device *gpiobtn_device;

/* ===== Driver state ===== */
static int press_count;
static int led_state;

/* ===== file operations ===== */
static int gpiobtn_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int gpiobtn_release(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t gpiobtn_read(struct file *file,
                            char __user *buf,
                            size_t len, loff_t *off)
{
    char msg[64];
    int msg_len;

    msg_len = snprintf(msg, sizeof(msg),
                       "Press Count: %d\n", press_count);

    if (*off >= msg_len)
        return 0;

    if (copy_to_user(buf, msg, msg_len))
        return -EFAULT;

    *off += msg_len;
    return msg_len;
}

static ssize_t gpiobtn_write(struct file *file,
                             const char __user *buf,
                             size_t len, loff_t *off)
{
    char val;

    if (copy_from_user(&val, buf, 1))
        return -EFAULT;

    if (val == '1') {
        led_state = !led_state;
        press_count++;
        pr_info("LED %s\n", led_state ? "ON" : "OFF");
    }

    return len;
}

static long gpiobtn_ioctl(struct file *file,
                          unsigned int cmd,
                          unsigned long arg)
{
    switch (cmd) {
    case LED_ON:
        led_state = 1;
        pr_info("LED ON via ioctl\n");
        break;
    case LED_OFF:
        led_state = 0;
        pr_info("LED OFF via ioctl\n");
        break;
    default:
        return -EINVAL;
    }
    return 0;
}

/* ===== fops ===== */
static struct file_operations fops = {
    .owner          = THIS_MODULE,
    .open           = gpiobtn_open,
    .release        = gpiobtn_release,
    .read           = gpiobtn_read,
    .write          = gpiobtn_write,
    .unlocked_ioctl = gpiobtn_ioctl,
};

/* ===== module init ===== */
static int __init gpiobtn_init(void)
{
    alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);

    cdev_init(&gpiobtn_cdev, &fops);
    cdev_add(&gpiobtn_cdev, dev_num, 1);

    gpiobtn_class = class_create(CLASS_NAME);
    gpiobtn_device =
        device_create(gpiobtn_class, NULL, dev_num,
                      NULL, DEVICE_NAME);

    pr_info("/dev/gpiobtn created\n");
    return 0;
}

/* ===== module exit ===== */
static void __exit gpiobtn_exit(void)
{
    device_destroy(gpiobtn_class, dev_num);
    class_destroy(gpiobtn_class);
    cdev_del(&gpiobtn_cdev);
    unregister_chrdev_region(dev_num, 1);

    pr_info("/dev/gpiobtn removed\n");
}

module_init(gpiobtn_init);
module_exit(gpiobtn_exit);

