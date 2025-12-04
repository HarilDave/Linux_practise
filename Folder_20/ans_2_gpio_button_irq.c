#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haril");
MODULE_DESCRIPTION("GPIO Button IRQ (DT configurable trigger)");

static struct gpio_desc *btn_gpiod;
static int irq_num;

/* ISR */
static irqreturn_t btn_irq_handler(int irq, void *dev_id)
{
    pr_info("[gpiobtn] Button interrupt received\n");
    return IRQ_HANDLED;
}

/* Probe */
static int btn_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    const char *trigger;
    unsigned long irq_flags;
    int ret;

    dev_info(dev, "Button probe started\n");

    /* Get GPIO from DT */
    btn_gpiod = gpiod_get(dev, NULL, GPIOD_IN);
    if (IS_ERR(btn_gpiod)) {
        dev_err(dev, "Failed to get GPIO\n");
        return PTR_ERR(btn_gpiod);
    }

    /* Convert GPIO to IRQ */
    irq_num = gpiod_to_irq(btn_gpiod);
    if (irq_num < 0) {
        dev_err(dev, "Failed to get IRQ from GPIO\n");
        ret = irq_num;
        goto err_gpio;
    }

    /* Read irq-trigger from DT */
    ret = of_property_read_string(dev->of_node,
                                  "irq-trigger",
                                  &trigger);
    if (ret) {
        dev_info(dev, "irq-trigger not found, default = falling\n");
        irq_flags = IRQF_TRIGGER_FALLING;
    } else if (!strcmp(trigger, "rising")) {
        irq_flags = IRQF_TRIGGER_RISING;
    } else if (!strcmp(trigger, "falling")) {
        irq_flags = IRQF_TRIGGER_FALLING;
    } else if (!strcmp(trigger, "both")) {
        irq_flags = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING;
    } else {
        dev_err(dev, "Invalid irq-trigger value\n");
        ret = -EINVAL;
        goto err_gpio;
    }

    /* Request IRQ */
    ret = request_irq(irq_num,
                      btn_irq_handler,
                      irq_flags,
                      "gpiobtn_irq",
                      NULL);
    if (ret) {
        dev_err(dev, "request_irq failed\n");
        goto err_gpio;
    }

    dev_info(dev, "Button IRQ registered (IRQ=%d)\n", irq_num);
    return 0;

err_gpio:
    gpiod_put(btn_gpiod);
    return ret;
}

/* Remove */
static int btn_remove(struct platform_device *pdev)
{
    free_irq(irq_num, NULL);
    gpiod_put(btn_gpiod);
    pr_info("[gpiobtn] Driver removed\n");
    return 0;
}

/* DT match table */
static const struct of_device_id btn_of_match[] = {
    { .compatible = "myvendor,gpiobtn" },
    { }
};
MODULE_DEVICE_TABLE(of, btn_of_match);

/* Platform driver */
static struct platform_driver btn_driver = {
    .probe  = btn_probe,
    .remove = btn_remove,
    .driver = {
        .name = "gpiobtn",
        .of_match_table = btn_of_match,
    },
};

module_platform_driver(btn_driver);

