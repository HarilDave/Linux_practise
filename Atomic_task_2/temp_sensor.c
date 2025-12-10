#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/atomic.h>
#include <linux/wait.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HARIL");
MODULE_DESCRIPTION("Temperature Sensor with Alerts using atomic variables");

struct temp_sensor 
{
    atomic_t current_temp;     // milli °C
    atomic_t high_threshold;
    atomic_t low_threshold;
    atomic_t alert_count;
    wait_queue_head_t alert_queue;
};

static struct temp_sensor sensor;

static int history[10];
static int h_index = 0;

static void add_history(int value)
{
    history[h_index % 10] = value;
    h_index++;
}

static void set_thresholds(int low, int high)
{
    atomic_set(&sensor.low_threshold, low);
    atomic_set(&sensor.high_threshold, high);
}

static void update_temp(int value)
{
    atomic_set(&sensor.current_temp, value);
    add_history(value);

    if (value <= atomic_read(&sensor.low_threshold) ||
        value >= atomic_read(&sensor.high_threshold))
    {
        atomic_inc(&sensor.alert_count);
        wake_up_interruptible(&sensor.alert_queue);
    }
}

static int get_temp(void)
{
    return atomic_read(&sensor.current_temp);
}

static int __init sensor_init(void)
{
    int i;

    atomic_set(&sensor.current_temp, 25000);
    atomic_set(&sensor.low_threshold, 20000);
    atomic_set(&sensor.high_threshold, 30000);
    atomic_set(&sensor.alert_count, 0);

    init_waitqueue_head(&sensor.alert_queue);

    for (i = 0; i < 10; i++)
        history[i] = 0;

    update_temp(31000); 
    update_temp(18000);

    pr_info("Temp Sensor Loaded: TEMP=%d ALERTS=%d\n",
            get_temp(), atomic_read(&sensor.alert_count));

    return 0;
}

static void __exit sensor_exit(void)
{
    pr_info("Temp Sensor Module Removed\n");
}

module_init(sensor_init);
module_exit(sensor_exit);
