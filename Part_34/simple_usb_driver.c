#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/usb.h>

#define VENDOR_ID   0x067b
#define PRODUCT_ID  0x2303

#define PRINT_INTERFACE(i) do {                          \
    pr_info("Interface Descriptor\n");                   \
    pr_info("Interface Number : %d\n", (i).bInterfaceNumber); \
    pr_info("Endpoints        : %d\n", (i).bNumEndpoints);    \
    pr_info("Class            : %d\n", (i).bInterfaceClass);  \
    pr_info("\n");                                       \
} while (0)

#define PRINT_ENDPOINT(e) do {                            \
    pr_info("Endpoint Descriptor\n");                     \
    pr_info("Endpoint Addr : 0x%x\n", (e).bEndpointAddress); \
    pr_info("Attributes    : 0x%x\n", (e).bmAttributes);     \
    pr_info("Max Packet    : %d\n", (e).wMaxPacketSize);     \
    pr_info("\n");                                       \
} while (0)

static int usb_probe(struct usb_interface *intf,const struct usb_device_id *id)
{
    int i;
    struct usb_host_interface *iface = intf->cur_altsetting;

    dev_info(&intf->dev, "USB Connected (VID=0x%04x, PID=0x%04x)\n", id->idVendor, id->idProduct);

    PRINT_INTERFACE(iface->desc);

    for (i = 0; i < iface->desc.bNumEndpoints; i++)
        PRINT_ENDPOINT(iface->endpoint[i].desc);

    return 0;
}

static void usb_disconnect(struct usb_interface *intf)
{
    dev_info(&intf->dev, "USB Disconnected\n");
}

static const struct usb_device_id usb_table[] = 
{
    { USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
    { }
};

MODULE_DEVICE_TABLE(usb, usb_table);

static struct usb_driver simple_usb_driver = 
{
    .name       = "simple_usb_driver",
    .probe      = usb_probe,
    .disconnect = usb_disconnect,
    .id_table   = usb_table,
};

module_usb_driver(simple_usb_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haril");

