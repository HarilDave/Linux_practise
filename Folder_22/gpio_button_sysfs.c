#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("GPIO Button Sysfs Example");

/* Driver state */
static int press_count;
static int led_state;

/* sysfs class + device */
static struct class *gpiobtn_class;
static struct device *gpiobtn_dev;

/* ===== press_count : READ ONLY ===== */
static ssize_t press_count_show(struct device *dev,
                                struct device_attribute *attr,
                                char *buf)
{
    return sprintf(buf, "%d\n", press_count);
}
static DEVICE_ATTR_RO(press_count);

/* ===== toggle_led : WRITE ONLY ===== */
static ssize_t toggle_led_store(struct device *dev,
                                struct device_attribute *attr,
                                const char *buf, size_t count)
{
    led_state = !led_state;
    press_count++;

    pr_info("LED %s, press_count=%d\n",
            led_state ? "ON" : "OFF",
            press_count);

    return count;
}
static DEVICE_ATTR_WO(toggle_led);

/* ===== module init ===== */
static int __init gpiobtn_init(void)
{
    gpiobtn_class = class_create("gpiobtn");
    if (IS_ERR(gpiobtn_class))
        return PTR_ERR(gpiobtn_class);

    gpiobtn_dev = device_create(gpiobtn_class, NULL, 0, NULL, "gpiobtn0");
    if (IS_ERR(gpiobtn_dev)) {
        class_destroy(gpiobtn_class);
        return PTR_ERR(gpiobtn_dev);
    }

    device_create_file(gpiobtn_dev, &dev_attr_press_count);
    device_create_file(gpiobtn_dev, &dev_attr_toggle_led);

    pr_info("gpiobtn sysfs driver loaded\n");
    return 0;
}

/* ===== module exit ===== */
static void __exit gpiobtn_exit(void)
{
    device_remove_file(gpiobtn_dev, &dev_attr_toggle_led);
    device_remove_file(gpiobtn_dev, &dev_attr_press_count);

    device_destroy(gpiobtn_class, 0);
    class_destroy(gpiobtn_class);

    pr_info("gpiobtn sysfs driver removed\n");
}

module_init(gpiobtn_init);
module_exit(gpiobtn_exit);

