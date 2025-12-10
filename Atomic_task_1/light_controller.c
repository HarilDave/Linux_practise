#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/atomic.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HARIL");
MODULE_DESCRIPTION("Smart Light Controller using atomic variables");

struct light_state {
    atomic_t brightness;      // 0–100
    atomic_t temperature;     // 2000–6500
    atomic_t is_on;           // 0 or 1
    atomic_t active_users;    // reference count
};

static struct light_state light = {
    .brightness = ATOMIC_INIT(0),
    .temperature = ATOMIC_INIT(2000),
    .is_on = ATOMIC_INIT(0),
    .active_users = ATOMIC_INIT(0),
};

static void set_on(int val)
{
    atomic_set(&light.is_on, val ? 1 : 0);
}

static void set_brightness(int value)
{
    atomic_set(&light.brightness, value);
}

static void set_temperature(int value)
{
    atomic_set(&light.temperature, value);
}

static void smooth_brightness(int from, int to)
{
    int i;
    int step = (from < to) ? 1 : -1;

    for (i = from; i != to; i += step)
        atomic_set(&light.brightness, i);

    atomic_set(&light.brightness, to);
}

static void read_state(int *on, int *b, int *t)
{
    *on = atomic_read(&light.is_on);
    *b  = atomic_read(&light.brightness);
    *t  = atomic_read(&light.temperature);
}

static int __init light_init(void)
{
    int on, b, t;

    atomic_inc(&light.active_users);

    set_on(1);

    set_brightness(50);

    set_brightness(75);

    read_state(&on, &b, &t);
    pr_info("LIGHT STATE -> ON=%d BRIGHT=%d TEMP=%d\n", on, b, t);

    return 0;
}

static void __exit light_exit(void)
{
    atomic_dec(&light.active_users);
    pr_info("Smart Light Controller removed\n");
}

module_init(light_init);
module_exit(light_exit);
