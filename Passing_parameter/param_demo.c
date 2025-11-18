#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haril");
MODULE_DESCRIPTION("Module parameter demo");

/* ---- Module parameters ---- */
static int myint = 10;
module_param(myint, int, 0644);

static char *mystr = "default";
module_param(mystr, charp, 0644);

static int myarr[5] = {1, 2, 3, 4, 5};
static int arr_count = 0;
module_param_array(myarr, int, &arr_count, 0644);

/* ---- Module init ---- */
static int __init param_demo_init(void)
{
    int i;

    printk(KERN_INFO "myint = %d\n", myint);
    printk(KERN_INFO "mystr = %s\n", mystr);
    printk(KERN_INFO "array values: ");
    for (i = 0; i < arr_count; i++)
        printk(KERN_INFO "myarr[%d] = %d\n", i, myarr[i]);

    return 0;
}

/* ---- Module exit ---- */
static void __exit param_demo_exit(void)
{
    printk(KERN_INFO "Exiting parameter demo module\n");
}

module_init(param_demo_init);
module_exit(param_demo_exit);

