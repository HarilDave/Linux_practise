
// sensor_helper.c

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int fake_temp = 25;  // start from 25 degree C

// This function will be used by temp_driver.c
int get_sensor_temp(void)
{
   // simulate temperature changing 
    fake_temp++;
    if (fake_temp > 100)
        fake_temp = 25;

    pr_info("sensor_helper: get_sensor_temp() = %d\n", fake_temp);
    return fake_temp;
}
EXPORT_SYMBOL(get_sensor_temp);

static int __init sensor_helper_init(void)
{
    pr_info("sensor_helper: loaded\n");
    return 0;
}

static void __exit sensor_helper_exit(void)
{
    pr_info("sensor_helper: unloaded\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haril");
MODULE_DESCRIPTION("Helper module providing get_sensor_temp()");
MODULE_VERSION("1.0");

module_init(sensor_helper_init);
module_exit(sensor_helper_exit);
