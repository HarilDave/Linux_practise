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
	{ 0x6b732375, "cdev_add" },
	{ 0x6ca9b86a, "class_create" },
	{ 0x3b69de06, "device_create" },
	{ 0x8bb6054c, "kernel_kobj" },
	{ 0x39977daa, "kobject_create_and_add" },
	{ 0x24783c2b, "sysfs_create_file_ns" },
	{ 0x7040f7ea, "sysfs_remove_file_ns" },
	{ 0xc2673d19, "kobject_put" },
	{ 0x5b40b481, "device_destroy" },
	{ 0x75646747, "class_destroy" },
	{ 0xc892ac3e, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xa916b694, "strnlen" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0x54b1fac6, "__ubsan_handle_load_invalid_value" },
	{ 0x65487097, "__x86_indirect_thunk_rax" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x754d539c, "strlen" },
	{ 0x69acdf38, "memcpy" },
	{ 0xcbd4898c, "fortify_panic" },
	{ 0xe2964344, "__wake_up" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x858c69be, "cdev_init" },
	{ 0xe2fd41e5, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "70CDBD5C18CD2577001CD1C");
