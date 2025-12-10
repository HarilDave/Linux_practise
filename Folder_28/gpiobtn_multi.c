#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/of.h>

MODULE_LICENSE("GPL");

struct gbtn {
    struct gpio_desc *btn;
    struct gpio_desc *led;
    int irq;
    const char *name;
};

static irqreturn_t btn_irq(int irq, void *data)
{
    struct gbtn *d = data;

    int v = gpiod_get_value(d->btn);
    gpiod_set_value(d->led, !gpiod_get_value(d->led));

    pr_info("%s: button=%d → LED toggled\n", d->name, v);
    return IRQ_HANDLED;
}

static int gpiobtn_probe(struct platform_device *pdev)
{
    struct gbtn *d;
    int ret;

    d = devm_kzalloc(&pdev->dev, sizeof(*d), GFP_KERNEL);
    if (!d) return -ENOMEM;

    d->name = dev_name(&pdev->dev);

    d->btn = devm_gpiod_get(&pdev->dev, "button", GPIOD_IN);
    if (IS_ERR(d->btn)) return PTR_ERR(d->btn);

    d->led = devm_gpiod_get(&pdev->dev, "led", GPIOD_OUT_LOW);
    if (IS_ERR(d->led)) return PTR_ERR(d->led);

    d->irq = gpiod_to_irq(d->btn);
    if (d->irq < 0) return d->irq;

    ret = devm_request_threaded_irq(&pdev->dev, d->irq, NULL, btn_irq,
                                    IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_ONESHOT,
                                    d->name, d);
    if (ret) return ret;

    pr_info("%s: probe OK irq=%d\n", d->name, d->irq);

    return 0;
}

static int gpiobtn_remove(struct platform_device *pdev)
{
    pr_info("%s: remove\n", dev_name(&pdev->dev));
    return 0;
}

static const struct of_device_id match[] = {
    { .compatible = "myvendor,gpiobtn" },
    { }
};
MODULE_DEVICE_TABLE(of, match);

static struct platform_driver dr = {
    .probe = gpiobtn_probe,
    .remove = gpiobtn_remove,
    .driver = {
        .name = "gpiobtn",
        .of_match_table = match,
    },
};
module_platform_driver(dr);

