#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haril");
MODULE_DESCRIPTION("Simulated GPIO button interrupt driver");

/* Chosen test IRQ (unused on PC) */
static int irq_num = 11;

/* Export IRQ number so simulator can trigger */
int gpiobtn_irq = 11;
EXPORT_SYMBOL(gpiobtn_irq);

/* ISR */
irqreturn_t btn_irq_handler(int irq, void *dev_id)
{
    pr_info("[gpiobtn] Button pressed (IRQ simulated)\n");
    return IRQ_HANDLED;
}
EXPORT_SYMBOL(btn_irq_handler);

static int __init btn_init(void)
{
    int ret;

    ret = request_irq(irq_num,
                      btn_irq_handler,
                      IRQF_SHARED,
                      "gpiobtn_sim",
                      &irq_num);
    if (ret) {
        pr_err("request_irq failed\n");
        return ret;
    }

    pr_info("[gpiobtn] Driver loaded, IRQ %d\n", irq_num);
    return 0;
}

static void __exit btn_exit(void)
{
    free_irq(irq_num, &irq_num);
    pr_info("[gpiobtn] Driver removed\n");
}

module_init(btn_init);
module_exit(btn_exit);

