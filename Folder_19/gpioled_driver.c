#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <linux/sysfs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("GPIO LED platform driver with sysfs control");

/* GPIO descriptor */
static struct gpio_desc *led_gpiod;

/* ================= SYSFS SHOW ================= */
static ssize_t led_show(struct device *dev,
                        struct device_attribute *attr,
                        char *buf)
{
    int value;

    value = gpiod_get_value(led_gpiod);
    return sprintf(buf, "%d\n", value);
}

/* ================= SYSFS STORE ================= */
static ssize_t led_store(struct device *dev,
                         struct device_attribute *attr,
                         const char *buf,
                         size_t count)
{
    int value;

    if (kstrtoint(buf, 10, &value))
        return -EINVAL;

    gpiod_set_value(led_gpiod, value ? 1 : 0);

    return count;
}

/* ================= SYSFS ATTRIBUTE ================= */
static DEVICE_ATTR(control, 0664, led_show, led_store);

/* ================= PROBE ================= */
static int gpioled_probe(struct platform_device *pdev)
{
    int ret;
    struct device *dev = &pdev->dev;

    /* Get GPIO from Device Tree */
    led_gpiod = devm_gpiod_get(dev, NULL, GPIOD_OUT_LOW);
    if (IS_ERR(led_gpiod)) {
        dev_err(dev, "Failed to get GPIO\n");
        return PTR_ERR(led_gpiod);
    }

    /* Create sysfs file */
    ret = device_create_file(dev, &dev_attr_control);
    if (ret) {
        dev_err(dev, "Failed to create sysfs file\n");
        return ret;
    }

    dev_info(dev, "GPIO LED driver probed\n");
    return 0;
}

/* ================= REMOVE ================= */
static int gpioled_remove(struct platform_device *pdev)
{
    device_remove_file(&pdev->dev, &dev_attr_control);
    gpiod_set_value(led_gpiod, 0);

    dev_info(&pdev->dev, "GPIO LED driver removed\n");
    return 0;
}

/* ================= DEVICE TREE MATCH ================= */
static const struct of_device_id gpioled_of_ids[] = {
    { .compatible = "myvendor,gpioled" },
    { }
};
MODULE_DEVICE_TABLE(of, gpioled_of_ids);

/* ================= PLATFORM DRIVER ================= */
static struct platform_driver gpioled_driver = {
    .probe  = gpioled_probe,
    .remove = gpioled_remove,
    .driver = {
        .name = "gpioled",
        .of_match_table = gpioled_of_ids,
    },
};

module_platform_driver(gpioled_driver);

