#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

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



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x122c3a7e, "_printk" },
	{ 0x60634223, "gpiod_set_value" },
	{ 0xe8b509ca, "device_destroy" },
	{ 0x6775d5d3, "class_destroy" },
	{ 0x27271c6b, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xa01f13a6, "cdev_init" },
	{ 0x3a6d85d3, "cdev_add" },
	{ 0x59c02473, "class_create" },
	{ 0x26961511, "device_create" },
	{ 0x31da6023, "gpio_to_desc" },
	{ 0xb986a570, "gpiod_direction_output" },
	{ 0x656e4a6e, "snprintf" },
	{ 0x6cbbfc54, "__arch_copy_to_user" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xdcb764ad, "memset" },
	{ 0x12a4e128, "__arch_copy_from_user" },
	{ 0x474e54d2, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "C09E88E9484EA40F53C01A2");
