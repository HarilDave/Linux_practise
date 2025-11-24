#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int arr[3] = {0, 0, 0};
static int count = 0;    // how many values were passed

module_param_array(arr, int, &count, 0660);

static int __init array_init(void)
{
    int sum = 0, avg = 0;
    int i;

    printk(KERN_INFO "Array values received: count = %d\n", count);

    for (i = 0; i < 3; i++) {
        printk(KERN_INFO "arr[%d] = %d\n", i, arr[i]);
        sum += arr[i];
    }

    avg = sum / 3;

    printk(KERN_INFO "Average = %d\n", avg);

    return 0;
}

static void __exit array_exit(void)
{
    printk(KERN_INFO "Array Module Unloaded\n");
}

module_init(array_init);
module_exit(array_exit);

MODULE_LICENSE("GPL");

