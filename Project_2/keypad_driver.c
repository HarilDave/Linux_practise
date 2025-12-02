// keypad_driver.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/semaphore.h>

// ================== CONFIG ==================
void keypad_inject_event(int key);
#define QUEUE_SIZE 16



static int keymap[8] = {1,2,3,4,5,6,7,8};
static int keymap_count = 8;

module_param_array(keymap, int, &keymap_count, 0444);
MODULE_PARM_DESC(keymap, "Keypad mapping");

// ================== QUEUE ==================

static int queue[QUEUE_SIZE];
static int head = 0;
static int tail = 0;
static int count = 0;

// ================== DEVICE ==================

static dev_t dev_num;
static struct cdev kp_cdev;
static struct class *kp_class;

// ================== SYNCHRONIZATION ==================

static struct semaphore write_sem;

// ================== IOCTL ==================

#define KP_MAGIC 'K'
#define KP_CLEAR_QUEUE _IO(KP_MAGIC, 1)

// ================== QUEUE HELPERS ==================

static int enqueue(int key)
{
    if (count == QUEUE_SIZE)
        return -1;

    queue[tail] = key;
    tail = (tail + 1) % QUEUE_SIZE;
    count++;
    return 0;
}

static int dequeue(int *key)
{
    if (count == 0)
        return -1;

    *key = queue[head];
    head = (head + 1) % QUEUE_SIZE;
    count--;
    return 0;
}

// ================== EXPORTED FUNCTION ==================

void keypad_inject_event(int key)
{
    enqueue(key);
}
EXPORT_SYMBOL(keypad_inject_event);

// ================== FILE OPS ==================

static int kp_open(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t kp_read(struct file *file, char __user *buf,
                       size_t count, loff_t *ppos)
{
    int key;

    if (dequeue(&key) < 0)
        return 0;

    if (copy_to_user(buf, &key, sizeof(int)))
        return -EFAULT;

    return sizeof(int);
}

static ssize_t kp_write(struct file *file,
                        const char __user *buf,
                        size_t count, loff_t *ppos)
{
    int key;

    if (down_interruptible(&write_sem))
        return -ERESTARTSYS;

    if (copy_from_user(&key, buf, sizeof(int))) {
        up(&write_sem);
        return -EFAULT;
    }

    enqueue(key);
    up(&write_sem);

    return sizeof(int);
}

static long kp_ioctl(struct file *file,
                     unsigned int cmd,
                     unsigned long arg)
{
    if (cmd == KP_CLEAR_QUEUE) {
        head = tail = count = 0;
        return 0;
    }
    return -ENOTTY;
}

static struct file_operations kp_fops = {
    .owner          = THIS_MODULE,
    .open           = kp_open,
    .read           = kp_read,
    .write          = kp_write,
    .unlocked_ioctl = kp_ioctl,
};

// ================== INIT / EXIT ==================

static int __init kp_init(void)
{
    alloc_chrdev_region(&dev_num, 0, 1, "keypad");

    cdev_init(&kp_cdev, &kp_fops);
    cdev_add(&kp_cdev, dev_num, 1);

    kp_class = class_create("keypad_class");
    device_create(kp_class, NULL, dev_num, NULL, "keypad");

    sema_init(&write_sem, 1);

    pr_info("keypad driver loaded\n");
    return 0;
}

static void __exit kp_exit(void)
{
    device_destroy(kp_class, dev_num);
    class_destroy(kp_class);
    cdev_del(&kp_cdev);
    unregister_chrdev_region(dev_num, 1);

    pr_info("keypad driver unloaded\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haril");
MODULE_DESCRIPTION("Keypad Driver with Event Queue");

module_init(kp_init);
module_exit(kp_exit);

