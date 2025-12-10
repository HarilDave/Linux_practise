#include <linux/module.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");

static struct platform_device fake_dev = {
    .name = "gpiobtn",
};

static int __init fake_init(void)
{
    return platform_device_register(&fake_dev);
}

static void __exit fake_exit(void)
{
    platform_device_unregister(&fake_dev);
}

module_init(fake_init);
module_exit(fake_exit);

