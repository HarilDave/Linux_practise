#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HARIL");

/* parameters */
int value;
int arr_value[4];
char *name;
int cb_value = 0;

/* normal params */
module_param(value, int, S_IRUSR | S_IWUSR);
module_param(name, charp, S_IRUSR | S_IWUSR);
module_param_array(arr_value, int, NULL, S_IRUSR | S_IWUSR);

/* callback */
int notify_param(const char *val, const struct kernel_param *kp)
{
    int ret;

    ret = param_set_int(val, kp);
    if (ret == 0) {
        printk(KERN_INFO "Callback called\n");
        printk(KERN_INFO "cb_value = %d\n", cb_value);
        return 0;
    }
    return -EINVAL;
}

const struct kernel_param_ops my_param_ops = {
    .set = notify_param,
    .get = param_get_int,
};

module_param_cb(cb_value, &my_param_ops, &cb_value, S_IRUGO | S_IWUSR);

static int __init my_init(void)
{
    int i;

    printk(KERN_INFO "value = %d\n", value);
    printk(KERN_INFO "cb_value = %d\n", cb_value);
    printk(KERN_INFO "name = %s\n", name);

    for (i = 0; i < (sizeof(arr_value) / sizeof(int)); i++) {
        printk(KERN_INFO "arr_value[%d] = %d\n", i, arr_value[i]);
    }

    printk(KERN_INFO "Module inserted\n");
    return 0;
}

static void __exit my_exit(void)
{
    printk(KERN_INFO "Module removed\n");
}

module_init(my_init);
module_exit(my_exit);

