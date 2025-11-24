#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define BUFFER_SIZE 256

// ---------------------------
// STEP 1: Module parameters
// ---------------------------
static char *device_name = "autodev";
static char *class_name  = "myclass";

module_param(device_name, charp, 0660);
module_param(class_name, charp, 0660);

static dev_t dev_number;
static struct cdev my_cdev;
static struct class *my_class;
static struct device *my_device;

static char message[BUFFER_SIZE] = "Hello from kernel!\n";

// File ops
static int dev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "[%s] opened\n", device_name);
    return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "[%s] closed\n", device_name);
    return 0;
}

static ssize_t dev_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    return simple_read_from_buffer(buf, len, offset, message, strlen(message));
}

static ssize_t dev_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
    if (len >= BUFFER_SIZE)
        len = BUFFER_SIZE - 1;

    if (copy_from_user(message, buf, len))
        return -EFAULT;

    message[len] = '\0';
    printk(KERN_INFO "[%s] received: %s\n", device_name, message);
    return len;
}

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = dev_open,
    .read    = dev_read,
    .write   = dev_write,
    .release = dev_release
};

// --------------------------------------
// Module Init
// --------------------------------------
static int __init autodev_init(void)
{
    int ret;

    // STEP 2: allocate major/minor
    ret = alloc_chrdev_region(&dev_number, 0, 1, device_name);
    if (ret < 0) {
        printk(KERN_ALERT "alloc_chrdev_region failed\n");
        return ret;
    }

    // STEP 3: cdev init + add
    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;

    ret = cdev_add(&my_cdev, dev_number, 1);
    if (ret < 0) {
        unregister_chrdev_region(dev_number, 1);
        return ret;
    }

    // STEP 4: class create
my_class = class_create(class_name);

    if (IS_ERR(my_class)) {
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_number, 1);
        return PTR_ERR(my_class);
    }

    // STEP 5: device create
    my_device = device_create(my_class, NULL, dev_number, NULL, device_name);
    if (IS_ERR(my_device)) {
        class_destroy(my_class);
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_number, 1);
        return PTR_ERR(my_device);
    }

    printk(KERN_INFO "Created /dev/%s under class %s\n", device_name, class_name);
    return 0;
}

// --------------------------------------
// Module Exit
// --------------------------------------
static void __exit autodev_exit(void)
{
    device_destroy(my_class, dev_number);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_number, 1);

    printk(KERN_INFO "Device %s removed\n", device_name);
}

module_init(autodev_init);
module_exit(autodev_exit);

MODULE_LICENSE("GPL");

