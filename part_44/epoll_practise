#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>

/* Wait queue */
DECLARE_WAIT_QUEUE_HEAD(my_wq);

/* Globals */
static dev_t mydev;
static struct class *myclass;
static struct cdev mycdev;
static struct kobject *mykobj;

static bool my_read_ready  = false;
static bool my_write_ready = false;
static char my_buf[20];

/* Prototypes */
static int  __init my_init(void);
static void __exit my_exit(void);

static int myopen(struct inode *, struct file *);
static int myclose(struct inode *, struct file *);
static ssize_t myread(struct file *, char __user *, size_t, loff_t *);
static ssize_t mywrite(struct file *, const char __user *, size_t, loff_t *);
static unsigned int mypoll(struct file *, struct poll_table_struct *);

static ssize_t sysfs_show(struct kobject *, struct kobj_attribute *, char *);
static ssize_t sysfs_store(struct kobject *, struct kobj_attribute *, const char *, size_t);

static struct kobj_attribute my_attr = __ATTR(data, 0660, sysfs_show, sysfs_store);

static struct file_operations fops =
{
    .owner   = THIS_MODULE,
    .open    = myopen,
    .release = myclose,
    .read    = myread,
    .write   = mywrite,
    .poll    = mypoll,
};

static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    my_write_ready = true;
    wake_up(&my_wq);
    return sprintf(buf, "Success\n");
}

static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    strcpy(my_buf, buf);
    my_read_ready = true;
    wake_up(&my_wq);
    return count;
}

static int myopen(struct inode *inode, struct file *file)
{
    return 0;
}

static int myclose(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t myread(struct file *file, char __user *buf, size_t len, loff_t *off)
{
    if (!my_read_ready)
        return 0;

    my_read_ready = false;

    if (copy_to_user(buf, my_buf, strlen(my_buf)))
        return -EFAULT;

    return strlen(my_buf);
}


static ssize_t mywrite(struct file *file, const char __user *buf, size_t len, loff_t *off)
{
    strcpy(my_buf, buf);
    return len;
}

static unsigned int mypoll(struct file *file, struct poll_table_struct *wait)
{
    __poll_t mask = 0;

    poll_wait(file, &my_wq, wait);

    if (my_read_ready) 
    {
        my_read_ready = false;
        mask |= POLLIN | POLLRDNORM;
    }

    if (my_write_ready) 
    {
        my_write_ready = false;
        mask |= POLLOUT | POLLWRNORM;
    }

    return mask;
}

static int __init my_init(void)
{
    alloc_chrdev_region(&mydev, 0, 1, "mydevice");

    cdev_init(&mycdev, &fops);
    cdev_add(&mycdev, mydev, 1);

    myclass = class_create("myclass");
    device_create(myclass, NULL, mydev, NULL, "mydevice");

    mykobj = kobject_create_and_add("my_sysfs", kernel_kobj);
    sysfs_create_file(mykobj, &my_attr.attr);

    return 0;
}

static void __exit my_exit(void)
{
    sysfs_remove_file(mykobj, &my_attr.attr);
    kobject_put(mykobj);

    device_destroy(myclass, mydev);
    class_destroy(myclass);

    cdev_del(&mycdev);
    unregister_chrdev_region(mydev, 1);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haril");

