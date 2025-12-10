#include <linux/module.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");

static struct platform_device dev0 = { .name = "gpiobtn", .id = 0 };
static struct platform_device dev1 = { .name = "gpiobtn", .id = 1 };

static int __init fake_init(void)
{
    platform_device_register(&dev0);
    platform_device_register(&dev1);
    return 0;
}

static void __exit fake_exit(void)
{
    platform_device_unregister(&dev0);
    platform_device_unregister(&dev1);
}

module_init(fake_init);
module_exit(fake_exit);

