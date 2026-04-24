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
	{ 0x31da6023, "gpio_to_desc" },
	{ 0xb986a570, "gpiod_direction_output" },
	{ 0x122c3a7e, "_printk" },
	{ 0xea82d349, "hrtimer_init" },
	{ 0xc0b7c197, "hrtimer_start_range_ns" },
	{ 0x102fe6de, "hrtimer_cancel" },
	{ 0x60634223, "gpiod_set_value" },
	{ 0x135bb7ec, "hrtimer_forward" },
	{ 0x474e54d2, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "E9F815E4631A6BC910123BE");
