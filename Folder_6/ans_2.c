#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static char *author = "DefaultAuthor";
module_param(author, charp, 0660);

static int repeat_count = 1;
module_param(repeat_count, int, 0660);

static int __init param_init(void)
{
    int i;

    printk(KERN_INFO "Module Loaded!\n");

    for (i = 0; i < repeat_count; i++) {
        printk(KERN_INFO "Author = %s\n", author);
    }

    return 0;
}

static void __exit param_exit(void)
{
    printk(KERN_INFO "Module Unloaded.\n");
}

module_init(param_init);
module_exit(param_exit);

MODULE_LICENSE("GPL");

