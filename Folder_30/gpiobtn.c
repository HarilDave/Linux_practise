// gpiobtn.c
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/slab.h>

static struct timer_list btn_timer;
static struct work_struct btn_work;
static u32 press_count;
static u32 led_state;
static bool debug_enabled = true;

static dev_t devt;
static struct cdev gpiobtn_cdev;
static struct class *gpiobtn_class;
static struct dentry *debug_dir;

/* Simulate button event by scheduling work from timer */
static void btn_work_handler(struct work_struct *w)
{
	if (debug_enabled)
		pr_info("gpiobtn: simulated button press %u\n", press_count + 1);

	press_count++;
}

/* Timer callback schedules work (acts like IRQ) */
static void btn_timer_fn(struct timer_list *t)
{
	/* schedule one simulated press */
	schedule_work(&btn_work);
	/* re-arm timer to fire again in 5 seconds (simulate occasional presses) */
	mod_timer(&btn_timer, jiffies + msecs_to_jiffies(5000));
}

/******** Character device operations ********/
static ssize_t gpiobtn_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
	char tmp[32];
	int n = snprintf(tmp, sizeof(tmp), "%u\n", press_count);
	return simple_read_from_buffer(buf, len, off, tmp, n);
}

static ssize_t gpiobtn_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
	char k;
	if (len < 1) return -EINVAL;
	if (copy_from_user(&k, buf, 1)) return -EFAULT;

	if (k == '1') { led_state = 1; }
	else          { led_state = 0; }

	return len;
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.read  = gpiobtn_read,
	.write = gpiobtn_write,
};

/******** debugfs self-test ********/
static ssize_t self_test_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
	pr_info("gpiobtn: running self-test\n");
	led_state = 1;
	msleep(200);
	led_state = 0;
	pr_info("gpiobtn: self-test complete\n");
	return len;
}

static const struct file_operations test_fops = {
	.owner = THIS_MODULE,
	.write = self_test_write,
};

/******** Probe / Remove / PM ********/
static int gpiobtn_probe(struct platform_device *pdev)
{
	int ret;

	/* init simulated resources */
	press_count = 0;
	led_state = 0;

	/* char device */
	ret = alloc_chrdev_region(&devt, 0, 1, "gpiobtn");
	if (ret) return ret;
	cdev_init(&gpiobtn_cdev, &fops);
	ret = cdev_add(&gpiobtn_cdev, devt, 1);
	if (ret) {
		unregister_chrdev_region(devt,1);
		return ret;
	}
	gpiobtn_class = class_create("gpiobtn");
	if (IS_ERR(gpiobtn_class)) {
		cdev_del(&gpiobtn_cdev);
		unregister_chrdev_region(devt,1);
		return PTR_ERR(gpiobtn_class);
	}
	device_create(gpiobtn_class, NULL, devt, NULL, "gpiobtn");

	/* debugfs */
	debug_dir = debugfs_create_dir("gpiobtn", NULL);
	debugfs_create_u32("press_count", 0444, debug_dir, &press_count);
	debugfs_create_u32("led_state",   0444, debug_dir, &led_state);
	debugfs_create_bool("debug_enabled", 0644, debug_dir, &debug_enabled);
	debugfs_create_file("self_test", 0200, debug_dir, NULL, &test_fops);

	/* work & timer simulate interrupts */
	INIT_WORK(&btn_work, btn_work_handler);
	timer_setup(&btn_timer, btn_timer_fn, 0);
	mod_timer(&btn_timer, jiffies + msecs_to_jiffies(5000));

	pr_info("gpiobtn: probe OK (simulated)\n");
	return 0;
}

static int gpiobtn_remove(struct platform_device *pdev)
{
	/* cleanup */
	del_timer_sync(&btn_timer);
	cancel_work_sync(&btn_work);

	debugfs_remove_recursive(debug_dir);

	device_destroy(gpiobtn_class, devt);
	class_destroy(gpiobtn_class);
	cdev_del(&gpiobtn_cdev);
	unregister_chrdev_region(devt, 1);

	pr_info("gpiobtn: removed\n");
	return 0;
}

#ifdef CONFIG_PM
static int gpiobtn_suspend(struct device *dev)
{
	pr_info("gpiobtn: suspend - stopping timer\n");
	del_timer_sync(&btn_timer);
	return 0;
}

static int gpiobtn_resume(struct device *dev)
{
	pr_info("gpiobtn: resume - restarting timer\n");
	mod_timer(&btn_timer, jiffies + msecs_to_jiffies(5000));
	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(gpiobtn_pm_ops, gpiobtn_suspend, gpiobtn_resume);

static const struct of_device_id gpiobtn_of_match[] = {
	{ .compatible = "myvendor,gpiobtn" },
	{}
};
MODULE_DEVICE_TABLE(of, gpiobtn_of_match);

static struct platform_driver gpiobtn_driver = {
	.probe = gpiobtn_probe,
	.remove = gpiobtn_remove,
	.driver = {
		.name = "gpiobtn",
		.of_match_table = gpiobtn_of_match,
		.pm = &gpiobtn_pm_ops,
	},
};

/* We will create a matching platform device in module init to force probe() */
static struct platform_device *gpiobtn_pdev;

static int __init gpiobtn_init(void)
{
	int ret;

	ret = platform_driver_register(&gpiobtn_driver);
	if (ret) return ret;

	/* create a simple platform device that matches the driver */
	gpiobtn_pdev = platform_device_register_simple("gpiobtn", -1, NULL, 0);
	if (IS_ERR(gpiobtn_pdev)) {
		platform_driver_unregister(&gpiobtn_driver);
		return PTR_ERR(gpiobtn_pdev);
	}

	pr_info("gpiobtn: module loaded (simulated platform device created)\n");
	return 0;
}

static void __exit gpiobtn_exit(void)
{
	platform_device_unregister(gpiobtn_pdev);
	platform_driver_unregister(&gpiobtn_driver);
	pr_info("gpiobtn: module unloaded\n");
}

module_init(gpiobtn_init);
module_exit(gpiobtn_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haril");
MODULE_DESCRIPTION("gpiobtn simulated driver for x86 (Assignment 1)");

