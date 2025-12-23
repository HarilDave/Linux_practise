#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/err.h>

#define IRQ_NO 11

static void my_work_fn(struct  work_struct *work);

DECLARE_WORK(my_work,my_work_fn);

void my_work_fn(struct  work_struct *work)
{
	printk(KERN_INFO "Executing Workqueue Function\n");
}

static irqreturn_t irq_handler(int irq,void *dev_id)
{
	printk(KERN_INFO "IRQ %d triggered \n", irq);
	schedule_work(&my_work);
	return IRQ_HANDLED;
}

static int __init myinit(void)
{
	if(request_irq(IRQ_NO,irq_handler,IRQF_SHARED,"my_irq_shared",(void*)&my_work_fn))
	{
	        printk(KERN_ERR "Failed to register IRQ\n");
		return -1;
	}
	
	printk(KERN_INFO "Module loaded: IRQ + Workqueue ready\n");
	
	asm("int $0x3B");
	
    	return 0;
}

static void __exit myexit(void)
{
    free_irq(IRQ_NO, (void *)&my_work_fn);
    printk(KERN_INFO "Module unloaded\n");
}

module_init(myinit);
module_exit(myexit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HARIL");
