#include <linux/init.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Platform driver using Device Tree properties");

struct mydt_data {
    int id;
    const char *label;
    u32 gpio;
    u32 irq;
};

static int dt_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct device_node *np = dev->of_node;
    struct mydt_data data;

    if (!np) {
        dev_err(dev, "No device tree node found\n");
        return -EINVAL;
    }

    /* Read DT properties */
    if (of_property_read_u32(np, "id", &data.id))
        data.id = -1;

    if (of_property_read_string(np, "label", &data.label))
        data.label = "unknown";

    if (of_property_read_u32(np, "gpio", &data.gpio))
        data.gpio = -1;

    if (of_property_read_u32(np, "irq", &data.irq))
        data.irq = -1;

    dev_info(dev, "DT Probed: id=%d, label=%s, gpio=%d, irq=%d\n",
             data.id, data.label, data.gpio, data.irq);

    return 0;
}

static int dt_remove(struct platform_device *pdev)
{
    dev_info(&pdev->dev, "Device removed\n");
    return 0;
}

/* Match table for Device Tree compatible string */
static const struct of_device_id dt_ids[] = {
    { .compatible = "myvendor,mydevice" },
    { }
};
MODULE_DEVICE_TABLE(of, dt_ids);

/* Register platform driver */
static struct platform_driver dt_driver = {
    .probe = dt_probe,
    .remove = dt_remove,
    .driver = {
        .name = "mydevice",
        .of_match_table = dt_ids,
    },
};

module_platform_driver(dt_driver);

