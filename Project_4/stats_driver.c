#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/timekeeping.h>

void stats_register_event(const char *op);
static int stats_enable = 1;
module_param(stats_enable,int,0644);
MODULE_PARM_DESC(stats_enable,"Enable or disable statistics");
 
struct stats_data
{
	unsigned long open_count;

	unsigned long read_count;
	unsigned long write_count;
	char last_op[16];
	struct timespec64 last_time;
};

static struct stats_data dev_stats;
static DEFINE_MUTEX(stats_lock);


#define STATS_IOC_MAGIC 'S'


#define STATS_IOC_GET _IOR(STATS_IOC_MAGIC ,1,struct stats_data)

static dev_t dev_num;
static struct cdev stats_cdev;
static struct class *stats_class; 

void stats_register_event(const char *op)
{
	if(!stats_enable)
		return;
	mutex_lock(&stats_lock);

	strncpy(dev_stats.last_op,op,sizeof(dev_stats.last_op));
	ktime_get_real_ts64(&dev_stats.last_time);

	mutex_unlock(&stats_lock);
}
EXPORT_SYMBOL(stats_register_event);

static int stats_open(struct inode *inode,struct file *file)
{
	if(stats_enable)
	{
		mutex_lock(&stats_lock);
		dev_stats.open_count++;
		strncpy(dev_stats.last_op,"open",16);
		ktime_get_real_ts64(&dev_stats.last_time);
		mutex_unlock(&stats_lock);
	}
	return 0;
}

static ssize_t stats_read(struct file *file,char __user *buf,size_t count, loff_t *ppos)
{
	char kbuf[64];
	int len;

	if(*ppos>0)
		return 0;
	mutex_lock(&stats_lock);
	len = snprintf(kbuf,sizeof(kbuf),"last_op = %s\n",dev_stats.last_op);
	dev_stats.read_count++;

	mutex_unlock(&stats_lock);

	if(copy_to_user(buf,kbuf,len))
		return -EFAULT;

	*ppos += len;
	return len;
}

static ssize_t stats_write(struct file *file,const char __user *buf,size_t count, loff_t *ppos)
{
	if(stats_enable)
	{
		mutex_lock(&stats_lock);
		dev_stats.write_count++;
		strncpy(dev_stats.last_op,"write",16);
		ktime_get_real_ts64(&dev_stats.last_time);
		mutex_unlock(&stats_lock);
	}
	return count;
}

static long stats_ioctl(struct file *file,
                        unsigned int cmd,
                        unsigned long arg)
{
    if (cmd != STATS_IOC_GET)
        return -ENOTTY;

    if (copy_to_user((void __user *)arg,
                      &dev_stats,
                      sizeof(dev_stats)))
        return -EFAULT;

    return 0;
}

static const struct file_operations stats_fops = {
    .owner          = THIS_MODULE,
    .open           = stats_open,
    .read           = stats_read,
    .write          = stats_write,
    .unlocked_ioctl = stats_ioctl,
};

static int __init stats_init(void)
{
    alloc_chrdev_region(&dev_num, 0, 1, "stats_dev");

    cdev_init(&stats_cdev, &stats_fops);
    cdev_add(&stats_cdev, dev_num, 1);

    stats_class = class_create("stats_class");
    device_create(stats_class, NULL, dev_num, NULL, "stats_dev");

    memset(&dev_stats, 0, sizeof(dev_stats));

    pr_info("stats_driver loaded\n");
    return 0;
}

static void __exit stats_exit(void)
{
    device_destroy(stats_class, dev_num);
    class_destroy(stats_class);
    cdev_del(&stats_cdev);
    unregister_chrdev_region(dev_num, 1);

    pr_info("stats_driver unloaded\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haril");
MODULE_DESCRIPTION("Device Statistics Driver");

module_init(stats_init);
module_exit(stats_exit);   























































































