// temp_driver.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>    // copy_to_user, copy_from_user
#include <linux/mutex.h>


extern int get_sensor_temp(void);


#define TEMP_IOC_MAGIC      'T'
#define TEMP_IOC_SET_HIGH   _IOW(TEMP_IOC_MAGIC, 1, int)
#define TEMP_IOC_SET_LOW    _IOW(TEMP_IOC_MAGIC, 2, int)


static int calib[3] = {0, 0, 0};
static int calib_count = 3;
module_param_array(calib, int, &calib_count, 0644);
MODULE_PARM_DESC(calib, "Three calibration integers");


static dev_t dev_num;
static struct cdev temp_cdev;
static struct class *temp_class;


static int high_threshold = 80;
static int low_threshold  = 20;
static DEFINE_MUTEX(config_lock);


static int apply_calibration(int raw)
{
    int adjusted = raw + calib[0] - calib[1] + calib[2];
    return adjusted;
}



static int temp_open(struct inode *inode, struct file *file)
{
    pr_info("temp_driver: device opened\n");
    return 0;
}

static int temp_release(struct inode *inode, struct file *file)
{
    pr_info("temp_driver: device closed\n");
    return 0;
}

// read(): return current temperature as text using copy_to_user
static ssize_t temp_read(struct file *file, char __user *buf,
                         size_t count, loff_t *ppos)
{
    int raw_temp, temp;
    char kbuf[64];
    int len;

    // so that `cat /dev/temp_sensor` returns only once
    if (*ppos > 0)
        return 0;

    raw_temp = get_sensor_temp();
    temp = apply_calibration(raw_temp);

    // Check thresholds (just print warnings in kernel log)
    if (temp > high_threshold)
        pr_warn("temp_driver: HIGH TEMP ALARM! temp=%d > %d\n",
                temp, high_threshold);
    if (temp < low_threshold)
        pr_warn("temp_driver: LOW TEMP ALARM! temp=%d < %d\n",
                temp, low_threshold);

    len = snprintf(kbuf, sizeof(kbuf),
                   "temp=%d (raw=%d) low=%d high=%d calib=[%d,%d,%d]\n",
                   temp, raw_temp, low_threshold, high_threshold,
                   calib[0], calib[1], calib[2]);

    if (count < len)
        return -EINVAL;

    if (copy_to_user(buf, kbuf, len))
        return -EFAULT;

    *ppos += len;
    return len;
}

// write(): unused as per requirement
static ssize_t temp_write(struct file *file, const char __user *buf,
                          size_t count, loff_t *ppos)
{
    pr_info("temp_driver: write() is unused\n");
    return -EINVAL; // or 0, but better to show it's not supported
}

// ioctl(): set high/low thresholds using copy_from_user + mutex
static long temp_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int value;

    switch (cmd)
    {
        case TEMP_IOC_SET_HIGH:
            if (copy_from_user(&value, (int __user *)arg, sizeof(int)))
                return -EFAULT;

            mutex_lock(&config_lock);
            high_threshold = value;
            mutex_unlock(&config_lock);

            pr_info("temp_driver: high_threshold set to %d\n", high_threshold);
            break;

        case TEMP_IOC_SET_LOW:
            if (copy_from_user(&value, (int __user *)arg, sizeof(int)))
                return -EFAULT;

            mutex_lock(&config_lock);
            low_threshold = value;
            mutex_unlock(&config_lock);

            pr_info("temp_driver: low_threshold set to %d\n", low_threshold);
            break;

        default:
            return -ENOTTY;  // command not supported
    }

    return 0;
}

static const struct file_operations temp_fops = {
    .owner          = THIS_MODULE,
    .open           = temp_open,
    .release        = temp_release,
    .read           = temp_read,
    .write          = temp_write,
    .unlocked_ioctl = temp_ioctl,
};

// === Module init/exit ===

static int __init temp_driver_init(void)
{
    int ret;

    // Allocate device number
    ret = alloc_chrdev_region(&dev_num, 0, 1, "temp_sensor");
    if (ret < 0) {
        pr_err("temp_driver: alloc_chrdev_region failed\n");
        return ret;
    }

    // Init and add cdev
    cdev_init(&temp_cdev, &temp_fops);
    temp_cdev.owner = THIS_MODULE;

    ret = cdev_add(&temp_cdev, dev_num, 1);
    if (ret < 0) {
        pr_err("temp_driver: cdev_add failed\n");
        unregister_chrdev_region(dev_num, 1);
        return ret;
    }

    // Create class and device (for /dev node)
    temp_class = class_create("temp_class");
    if (IS_ERR(temp_class)) {
        pr_err("temp_driver: class_create failed\n");
        cdev_del(&temp_cdev);
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(temp_class);
    }

    if (!device_create(temp_class, NULL, dev_num, NULL, "temp_sensor")) {
        pr_err("temp_driver: device_create failed\n");
        class_destroy(temp_class);
        cdev_del(&temp_cdev);
        unregister_chrdev_region(dev_num, 1);
        return -EINVAL;
    }

    mutex_init(&config_lock);

    pr_info("temp_driver: loaded. Major=%d Minor=%d\n",
            MAJOR(dev_num), MINOR(dev_num));
    pr_info("temp_driver: initial thresholds low=%d high=%d calib=[%d,%d,%d]\n",
            low_threshold, high_threshold, calib[0], calib[1], calib[2]);

    return 0;
}

static void __exit temp_driver_exit(void)
{
    device_destroy(temp_class, dev_num);
    class_destroy(temp_class);
    cdev_del(&temp_cdev);
    unregister_chrdev_region(dev_num, 1);

    pr_info("temp_driver: unloaded\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haril");
MODULE_DESCRIPTION("Temperature Sensor Driver with Threshold Alarms");
MODULE_VERSION("1.0");

module_init(temp_driver_init);
module_exit(temp_driver_exit);
