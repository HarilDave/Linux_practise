#include <linux/module.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");

static int gpiobtn_suspend(struct device *dev)
{
    pr_info("gpiobtn: suspend\n");
    return 0;
}

static int gpiobtn_resume(struct device *dev)
{
    pr_info("gpiobtn: resume\n");
    return 0;
}

static const struct dev_pm_ops gpiobtn_pm_ops = {
    .suspend = gpiobtn_suspend,
    .resume  = gpiobtn_resume,
};

static int gpiobtn_probe(struct platform_device *pdev)
{
    pr_info("gpiobtn: probe OK\n");
    return 0;    // success
}

static int gpiobtn_remove(struct platform_device *pdev)
{
    pr_info("gpiobtn: removed\n");
    return 0;
}

static struct platform_driver gpiobtn_driver = {
    .probe  = gpiobtn_probe,
    .remove = gpiobtn_remove,
    .driver = {
        .name = "gpiobtn",
        .pm   = &gpiobtn_pm_ops,
    },
};

module_platform_driver(gpiobtn_driver);

