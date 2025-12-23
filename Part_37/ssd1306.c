#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/delay.h>

#define SSD1306_ADDR   0x3C
#define DRIVER_NAME    "ssd1306_oled"

/* I2C client */
static struct i2c_client *oled_client;

/* ---------- I2C helpers ---------- */
static int oled_write(u8 ctrl, u8 data)
{
    u8 buf[2] = { ctrl, data };
    return i2c_master_send(oled_client, buf, 2);
}

static void oled_cmd(u8 cmd)
{
    oled_write(0x00, cmd);
}

static void oled_data(u8 data)
{
    oled_write(0x40, data);
}

/* ---------- OLED init ---------- */
static void oled_init_display(void)
{
    msleep(100);

    oled_cmd(0xAE);
    oled_cmd(0xD5);
    oled_cmd(0x80);
    oled_cmd(0xA8);
    oled_cmd(0x3F);
    oled_cmd(0xD3);
    oled_cmd(0x00);
    oled_cmd(0x40);
    oled_cmd(0x8D);
    oled_cmd(0x14);
    oled_cmd(0x20);
    oled_cmd(0x00);
    oled_cmd(0xA1);
    oled_cmd(0xC8);
    oled_cmd(0xDA);
    oled_cmd(0x12);
    oled_cmd(0x81);
    oled_cmd(0x80);
    oled_cmd(0xD9);
    oled_cmd(0xF1);
    oled_cmd(0xDB);
    oled_cmd(0x20);
    oled_cmd(0xA4);
    oled_cmd(0xA6);
    oled_cmd(0xAF);
}

/* ---------- Fill display ---------- */
static void oled_fill(u8 val)
{
    int i;
    for (i = 0; i < 128 * 8; i++)
        oled_data(val);
}

/* ---------- Probe (Kernel 6.x) ---------- */
static int oled_probe(struct i2c_client *client)
{
    oled_client = client;

    pr_info("SSD1306: probe called\n");

    oled_init_display();
    oled_fill(0xFF);

    pr_info("SSD1306 OLED initialized\n");
    return 0;
}

/* ---------- Remove (Kernel 6.x) ---------- */
static void oled_remove(struct i2c_client *client)
{
    oled_fill(0x00);
    pr_info("SSD1306 OLED removed\n");
}

/* ---------- ID table ---------- */
static const struct i2c_device_id oled_id[] =
{
    { DRIVER_NAME, 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, oled_id);

/* ---------- I2C driver ---------- */
static struct i2c_driver oled_driver =
{
    .driver = 
    {
        .name = DRIVER_NAME,
    },
    .probe  = oled_probe,
    .remove = oled_remove,
    .id_table = oled_id,
};

module_i2c_driver(oled_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HARIL");

