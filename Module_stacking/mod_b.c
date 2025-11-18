#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");

extern int func_a(int x);  // import from A

int func_b(int y)
{
    int val = func_a(y);
    printk(KERN_INFO "func_b: received %d, returning %d\n", y, val * 2);
    return val * 2;
}
EXPORT_SYMBOL(func_b);

static int __init mod_b_init(void)
{
    printk(KERN_INFO "Module B loaded\n");
    func_b(10);
    return 0;
}

static void __exit mod_b_exit(void)
{
    printk(KERN_INFO "Module B unloaded\n");
}

module_init(mod_b_init);
module_exit(mod_b_exit);

