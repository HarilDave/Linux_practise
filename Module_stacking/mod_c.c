#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");

extern int func_b(int y);  // import from B

static int __init mod_c_init(void)
{
    printk(KERN_INFO "Module C loaded\n");
    int result = func_b(50);
    printk(KERN_INFO "Module C: result = %d\n", result);
    return 0;
}

static void __exit mod_c_exit(void)
{
    printk(KERN_INFO "Module C unloaded\n");
}

module_init(mod_c_init);
module_exit(mod_c_exit);

