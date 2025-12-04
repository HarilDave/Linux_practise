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
	{ 0xc1514a3b, "free_irq" },
	{ 0x622e0173, "gpiod_put" },
	{ 0x122c3a7e, "_printk" },
	{ 0x6079cf62, "_dev_info" },
	{ 0x5207f8c, "gpiod_get" },
	{ 0xbbac351e, "gpiod_to_irq" },
	{ 0x92d5838e, "request_threaded_irq" },
	{ 0xfe15742c, "_dev_err" },
	{ 0x4a77885d, "platform_driver_unregister" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x23509fba, "__platform_driver_register" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xe2fd41e5, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("of:N*T*Cmyvendor,gpiobtn");
MODULE_ALIAS("of:N*T*Cmyvendor,gpiobtnC*");

MODULE_INFO(srcversion, "3F7C5E32B4BD4AB88E46FA4");
