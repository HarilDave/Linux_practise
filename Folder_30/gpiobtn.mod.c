#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xf9a482f9, "msleep" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x656e4a6e, "snprintf" },
	{ 0x619cb7dd, "simple_read_from_buffer" },
	{ 0x32a08632, "platform_device_unregister" },
	{ 0x4a77885d, "platform_driver_unregister" },
	{ 0x54b1fac6, "__ubsan_handle_load_invalid_value" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x858c69be, "cdev_init" },
	{ 0x6b732375, "cdev_add" },
	{ 0x6ca9b86a, "class_create" },
	{ 0x3b69de06, "device_create" },
	{ 0x4292a372, "debugfs_create_dir" },
	{ 0xf8d1e162, "debugfs_create_u32" },
	{ 0x83445d73, "debugfs_create_bool" },
	{ 0xea9a721b, "debugfs_create_file" },
	{ 0xc6f46339, "init_timer_key" },
	{ 0x23509fba, "__platform_driver_register" },
	{ 0xf6f06725, "platform_device_register_full" },
	{ 0x2d3385d3, "system_wq" },
	{ 0xc5b6f236, "queue_work_on" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x122c3a7e, "_printk" },
	{ 0x15ba50a6, "jiffies" },
	{ 0xc38c83b8, "mod_timer" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x82ee90dc, "timer_delete_sync" },
	{ 0x3c12dfe, "cancel_work_sync" },
	{ 0x6a7a92b8, "debugfs_remove" },
	{ 0x5b40b481, "device_destroy" },
	{ 0x75646747, "class_destroy" },
	{ 0xc892ac3e, "cdev_del" },
	{ 0xe2fd41e5, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("of:N*T*Cmyvendor,gpiobtn");
MODULE_ALIAS("of:N*T*Cmyvendor,gpiobtnC*");

MODULE_INFO(srcversion, "9CD5E68BB61816E31F2B23A");
