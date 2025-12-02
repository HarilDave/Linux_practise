// led_hw.c
#include <linux/module.h>
#include <linux/kernel.h>

static int current_on;
static unsigned int current_blink;

void led_hw_set_state(int on, unsigned int blink_rate)
{
    current_on = on;
    current_blink = blink_rate;

    pr_info("led_hw: LED %s, blink_rate=%u\n",
            on ? "ON" : "OFF", blink_rate);
}
EXPORT_SYMBOL(led_hw_set_state);

static int __init led_hw_init(void)
{
    pr_info("led_hw: module loaded\n");
    return 0;
}

static void __exit led_hw_exit(void)
{
    pr_info("led_hw: module unloaded\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haril");
MODULE_DESCRIPTION("Dummy LED hardware helper");

module_init(led_hw_init);
module_exit(led_hw_exit);

