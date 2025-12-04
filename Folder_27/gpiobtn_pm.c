#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/interrupt.h>
#include <linux/pm.h>

MODULE_LICENSE("GPL");

/* ===== driver data ===== */
static struct gpio_desc *led_gpiod;
static int irq_num;
static int led_state;

/* ===== suspend ===== */
static int gpiobtn_suspend(struct device *dev)
{
    pr_info("[gpiobtn] Suspending\n");

    disable_irq(irq_num);          // stop interrupts
    gpiod_set_value(led_gpiod, 0); // turn off LED

    return 0;
}

/* ===== resume ===== */
static int gpiobtn_resume(struct device *dev)
{
    pr_info("[gpiobtn] Resuming\n");

    if (led_state)
        gpiod_set_value(led_gpiod, 1);  // restore LED

    enable_irq(irq_num);               // re-enable IRQ
    return 0;
}

/* ===== PM ops ===== */
static const struct dev_pm_ops gpiobtn_pm_ops = {
    .suspend = gpiobtn_suspend,
    .resume  = gpiobtn_resume,
};

/* ===== probe ===== */
static int gpiobtn_probe(struct platform_device *pdev)
{
    led_gpiod = gpiod_get(&pdev->dev, NULL, GPIOD_OUT_LOW);
    if (IS_ERR(led_gpiod))
        return PTR_ERR(led_gpiod);

    irq_num = platform_get_irq(pdev, 0);
    if (irq_num < 0)
        return irq_num;

    led_state = 1;
    gpiod_set_value(led_gpiod, led_state);

    pr_info("[gpiobtn] Driver loaded\n");
    return 0;
}

/* ===== remove ===== */
static int gpiobtn_remove(struct platform_device *pdev)
{
    gpiod_set_value(led_gpiod, 0);
    gpiod_put(led_gpiod);
    return 0;
}

/* ===== DT match ===== */
static const struct of_device_id gpiobtn_dt_ids[] = {
    { .compatible = "myvendor,gpiobtn" },
    { }
};
MODULE_DEVICE_TABLE(of, gpiobtn_dt_ids);

/* ===== platform driver ===== */
static struct platform_driver gpiobtn_driver = {
    .probe  = gpiobtn_probe,
    .remove = gpiobtn_remove,
    .driver = {
        .name = "gpiobtn",
        .of_match_table = gpiobtn_dt_ids,
        .pm = &gpiobtn_pm_ops,   // ✅ PM hooked here
    },
};

module_platform_driver(gpiobtn_driver);

