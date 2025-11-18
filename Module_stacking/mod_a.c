#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");

int func_a(int x)
{
    printk(KERN_INFO "func_a called: x = %d\n", x);
    return x + 100;
}
EXPORT_SYMBOL(func_a);

static int __init mod_a_init(void)
{
    printk(KERN_INFO "Module A loaded\n");
    return 0;
}

static void __exit mod_a_exit(void)
{
    printk(KERN_INFO "Module A unloaded\n");
}

module_init(mod_a_init);
module_exit(mod_a_exit);

