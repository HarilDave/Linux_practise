#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>

static struct class *my_class;

static int __init my_init(void)
{
pr_info("class driver loaded");

my_class =class_create("myclass");
if(IS_ERR(my_class))
{
	pr_err("Failed tocreate class");
	return PTR_ERR(my_class);
}

pr_info("class created/n");
return 0;
}

static void __exit my_exit(void)
{
	class_destroy(my_class);
	pr_info("class removed");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");








































 
