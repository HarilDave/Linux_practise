#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/workqueue.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Simulated GPIO Button + LED using Workqueue");

/* ===== Dummy platform device ===== */
static struct platform_device *sim_pdev;

/* ===== Workqueue ===== */
static struct work_struct btn_work;
static int led_state;

/* ===== Work handler (simulates LED toggle) ===== */
static void btn_work_handler(struct work_struct *work)
{
    led_state = !led_state;
    pr_info("[SIM] Button pressed -> LED %s\n",
            led_state ? "ON" : "OFF");
}

/* ===== Simulated IRQ trigger ===== */
static ssize_t trigger_store(struct device *dev,
                             struct device_attribute *attr,
                             const char *buf, size_t count)
{
    schedule_work(&btn_work);  // simulate ISR → workqueue
    return count;
}

static DEVICE_ATTR(trigger, 0220, NULL, trigger_store);

/* ===== Probe ===== */
static int sim_probe(struct platform_device *pdev)
{
    INIT_WORK(&btn_work, btn_work_handler);

    device_create_file(&pdev->dev, &dev_attr_trigger);

    pr_info("[SIM] Driver probed (no real hardware)\n");
    return 0;
}

/* ===== Remove ===== */
static int sim_remove(struct platform_device *pdev)
{
    device_remove_file(&pdev->dev, &dev_attr_trigger);
    cancel_work_sync(&btn_work);

    pr_info("[SIM] Driver removed\n");
    return 0;
}

/* ===== Platform driver ===== */
static struct platform_driver sim_driver = {
    .probe = sim_probe,
    .remove = sim_remove,
    .driver = {
        .name = "gpio-sim",
    },
};

/* ===== Init / Exit ===== */
static int __init sim_init(void)
{
    sim_pdev = platform_device_register_simple("gpio-sim", -1, NULL, 0);
    return platform_driver_register(&sim_driver);
}

static void __exit sim_exit(void)
{
    platform_device_unregister(sim_pdev);
    platform_driver_unregister(&sim_driver);
}

module_init(sim_init);
module_exit(sim_exit);

