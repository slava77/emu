#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

#undef unix
struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = __stringify(KBUILD_MODNAME),
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
};

static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
	{ 0x98af416e, "struct_module" },
	{ 0x2e9ca3e1, "__kfree_skb" },
	{ 0x91091698, "mem_map" },
	{ 0xdb19f1c4, "register_sysctl_table" },
	{ 0x6bee212d, "page_address" },
	{ 0x832b990e, "init_mm" },
	{ 0x6be70cd8, "dev_get_by_name" },
	{ 0xe93e887, "_spin_lock" },
	{ 0x2fd1d81c, "vfree" },
	{ 0x6804547e, "_spin_lock_irqsave" },
	{ 0x1d26aa98, "sprintf" },
	{ 0xda02d67, "jiffies" },
	{ 0x9925ce9c, "__might_sleep" },
	{ 0xd7474566, "__copy_to_user_ll" },
	{ 0x5258b833, "remap_page_range" },
	{ 0x1af40e18, "__copy_from_user_ll" },
	{ 0x1b7d4074, "printk" },
	{ 0x48d35066, "alloc_skb" },
	{ 0xb2372b5b, "_spin_unlock_irqrestore" },
	{ 0x3caeb253, "_spin_unlock" },
	{ 0x2fc64a00, "proc_dostring" },
	{ 0x49e79940, "__cond_resched" },
	{ 0x463a476a, "skb_over_panic" },
	{ 0xb3240cac, "unregister_sysctl_table" },
	{ 0x2071e84e, "register_chrdev" },
	{ 0xa0b04675, "vmalloc_32" },
	{ 0xc192d491, "unregister_chrdev" },
	{ 0x7112ead2, "dev_queue_xmit" },
	{ 0xeeb761df, "kernel_flag" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";

