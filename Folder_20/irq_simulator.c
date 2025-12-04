#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haril");
MODULE_DESCRIPTION("Software IRQ trigger");

extern int gpiobtn_irq;
extern irqreturn_t btn_irq_handler(int, void *);

static int __init irq_sim_init(void)
{
    pr_info("[irq_sim] Simulating button press\n");

    /* Directly invoke handler */
    btn_irq_handler(gpiobtn_irq, NULL);

    return 0;
}

static void __exit irq_sim_exit(void)
{
    pr_info("[irq_sim] Removed\n");
}

module_init(irq_sim_init);
module_exit(irq_sim_exit);

