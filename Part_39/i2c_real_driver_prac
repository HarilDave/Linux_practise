#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/gpio.h>

#define ADAPTER_NAME "SIMPLE_I2C_ADAPTER"

/* GPIO pins */
#define SCL_GPIO 20
#define SDA_GPIO 21
#define I2C_DELAY usleep_range(5, 10)

/* GPIO helpers */
static bool read_sda(void)
{
    gpio_direction_input(SDA_GPIO);
    return gpio_get_value(SDA_GPIO);
}

static void clr_scl(void)
{
    gpio_direction_output(SCL_GPIO, 0);
}

static void clr_sda(void)
{
    gpio_direction_output(SDA_GPIO, 0);
}

static void set_scl(void)
{
    gpio_direction_output(SCL_GPIO, 1);
}

static void set_sda(void)
{
    gpio_direction_output(SDA_GPIO, 1);
}

/* init / deinit */
static int i2c_gpio_init(void)
{
    if (!gpio_is_valid(SCL_GPIO) || !gpio_is_valid(SDA_GPIO))
        return -1;

    if (gpio_request(SCL_GPIO, "scl"))
        return -1;

    if (gpio_request(SDA_GPIO, "sda")) {
        gpio_free(SCL_GPIO);
        return -1;
    }

    set_scl();
    set_sda();
    return 0;
}

static void i2c_gpio_deinit(void)
{
    gpio_free(SCL_GPIO);
    gpio_free(SDA_GPIO);
}

/* I2C bitbang */
static void i2c_start(void)
{
    set_sda();
    set_scl();
    I2C_DELAY;
    clr_sda();
    I2C_DELAY;
    clr_scl();
}

static void i2c_stop(void)
{
    clr_sda();
    I2C_DELAY;
    set_scl();
    I2C_DELAY;
    set_sda();
}

static int i2c_ack(void)
{
    int ok = 1;

    I2C_DELAY;
    set_scl();
    I2C_DELAY;

    if (read_sda())
        ok = 0;

    clr_scl();
    return ok;
}

static int i2c_send_addr(u8 addr, bool read)
{
    int i;

    for (i = 6; i >= 0; i--) 
    {
        (addr & (1 << i)) ? set_sda() : clr_sda();
        I2C_DELAY;
        set_scl();
        I2C_DELAY;
        clr_scl();
    }

    read ? set_sda() : clr_sda();
    I2C_DELAY;
    set_scl();
    I2C_DELAY;
    clr_scl();

    return i2c_ack() ? 0 : -1;
}

static int i2c_send_byte(u8 data)
{
    int i;

    for (i = 7; i >= 0; i--) 
    {
        (data & (1 << i)) ? set_sda() : clr_sda();
        I2C_DELAY;
        set_scl();
        I2C_DELAY;
        clr_scl();
    }

    return i2c_ack() ? 0 : -1;
}

static int i2c_send(u8 addr, u8 *buf, u16 len)
{
    int i;

    if (i2c_send_addr(addr, false))
        return -1;

    for (i = 0; i < len; i++) 
    {
        if (i2c_send_byte(buf[i]))
            return -1;
    }
    return 0;
}

/* adapter ops */
static u32 simple_func(struct i2c_adapter *adap)
{
    return I2C_FUNC_I2C | I2C_FUNC_SMBUS_BYTE | I2C_FUNC_SMBUS_BYTE_DATA;
}

static s32 simple_xfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num)
{
    int i, ret = 0;

    if (i2c_gpio_init())
        return -1;

    i2c_start();

    for (i = 0; i < num; i++) 
    {
        if (i2c_send(msgs[i].addr, msgs[i].buf, msgs[i].len))
            break;
        ret++;
    }

    i2c_stop();
    i2c_gpio_deinit();
    return ret;
}

static struct i2c_algorithm simple_algo = 
{
    .master_xfer   = simple_xfer,
    .functionality = simple_func,
};

static struct i2c_adapter simple_adapter = 
{
    .owner = THIS_MODULE,
    .class = I2C_CLASS_HWMON,
    .algo  = &simple_algo,
    .name  = ADAPTER_NAME,
    .nr    = -1,   
};

/* module */
static int __init simple_init(void)
{
    return i2c_add_numbered_adapter(&simple_adapter);
}

static void __exit simple_exit(void)
{
    i2c_del_adapter(&simple_adapter);
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haril");
