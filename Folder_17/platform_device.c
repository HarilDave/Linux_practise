#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include "my_pdata.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Platform device with platform data");







// Create platform data instance
static struct my_pdata my_data = {
    .id = 5,
    .label = "SensorNode",
};

// Declare platform device
static struct platform_device my_pdev = {
    .name = "mypdata-device",
    .id = -1,
};

static int __init pdev_init(void)
{
    printk(KERN_INFO "Registering platform device with pdata...\n");

    // Attach platform data to device
    my_pdev.dev.platform_data = &my_data;

    return platform_device_register(&my_pdev);
}

static void __exit pdev_exit(void)
{
    printk(KERN_INFO "Unregistering platform device...\n");
    platform_device_unregister(&my_pdev);
}

module_init(pdev_init);
module_exit(pdev_exit);

