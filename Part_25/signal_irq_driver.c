#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/interrupt.h>
#include <linux/sched/signal.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HARIL");

#define MY_SIG      44
#define MY_IOCTL_REG_TASK   _IOW('a','a',int32_t*)
#define MY_IRQ      11

static struct task_struct *my_task = NULL;
static int my_signum = MY_SIG;

static dev_t mydev;
static struct cdev mycdev;
static struct class *myclass;

/* ---------------- File Ops ---------------- */
static int my_open(struct inode *inode, struct file *file)
{
    pr_info("mydevice: open\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
    if (my_task == current)
        my_task = NULL;

    pr_info("mydevice: release\n");
    return 0;
}

static ssize_t my_read(struct file *filp, char __user *buf,
                       size_t len, loff_t *off)
{
    pr_info("mydevice: read -> triggering interrupt\n");
    asm volatile("int $0x3B");
    return 0;
}

static ssize_t my_write(struct file *filp,
                        const char __user *buf,
                        size_t len, loff_t *off)
{
    pr_info("mydevice: write\n");
    return len;
}

static long my_ioctl(struct file *file,
                     unsigned int cmd,
                     unsigned long arg)
{
    if (cmd == MY_IOCTL_REG_TASK) {
        my_task = current;
        pr_info("mydevice: user task registered for signal\n");
    }
    return 0;
}

static struct file_operations fops =
{
    .owner          = THIS_MODULE,
    .read           = my_read,
    .write          = my_write,
    .open           = my_open,
    .release        = my_release,
    .unlocked_ioctl = my_ioctl,
};

/* ---------------- IRQ ---------------- */
static irqreturn_t my_irq_handler(int irq, void *dev_id)
{
    struct kernel_siginfo info;

    pr_info("mydevice: IRQ received\n");

    if (!my_task)
        return IRQ_HANDLED;

    memset(&info, 0, sizeof(info));
    info.si_signo = my_signum;
    info.si_code  = SI_QUEUE;
    info.si_int   = 1;

    if (send_sig_info(my_signum, &info, my_task) < 0)
        pr_warn("mydevice: signal send failed\n");

    return IRQ_HANDLED;
}

/* ---------------- Init ---------------- */
static int __init my_driver_init(void)
{
    if (alloc_chrdev_region(&mydev, 0, 1, "mydevice_sig"))
        return -1;

    cdev_init(&mycdev, &fops);
    if (cdev_add(&mycdev, mydev, 1))
        goto err_chrdev;

    myclass = class_create("myclass_sig");
    if (IS_ERR(myclass))
        goto err_class;

    if (IS_ERR(device_create(myclass, NULL, mydev,
                             NULL, "mydevice_sig")))
        goto err_device;

    if (request_irq(MY_IRQ, my_irq_handler,
                    IRQF_SHARED,
                    "mydevice_sig", &mydev))
        goto err_irq;

    pr_info("Signal driver loaded\n");
    return 0;

err_irq:
    device_destroy(myclass, mydev);
    class_destroy(myclass);
err_device:
    cdev_del(&mycdev);
err_class:
    unregister_chrdev_region(mydev, 1);
err_chrdev:
    return -1;
}

/* ---------------- Exit ---------------- */
static void __exit my_driver_exit(void)
{
    free_irq(MY_IRQ, &mydev);
    device_destroy(myclass, mydev);
    class_destroy(myclass);
    cdev_del(&mycdev);
    unregister_chrdev_region(mydev, 1);
    pr_info("Signal driver unloaded\n");
}

module_init(my_driver_init);
module_exit(my_driver_exit);

