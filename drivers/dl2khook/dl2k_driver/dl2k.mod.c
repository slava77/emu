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
	{ 0x44f0bcfa, "pci_unregister_driver" },
	{ 0x61079cf0, "pci_register_driver" },
	{ 0xa5ba5f5, "unregister_netdev" },
	{ 0xde737a3, "del_timer_sync" },
	{ 0xf20dabd8, "free_irq" },
	{ 0xe523ad75, "synchronize_irq" },
	{ 0xe28e6b41, "__netdev_watchdog_up" },
	{ 0xd6d5cbc3, "linkwatch_fire_event" },
	{ 0x6d69932f, "eth_type_trans" },
	{ 0x463a476a, "skb_over_panic" },
	{ 0x3caeb253, "_spin_unlock" },
	{ 0xd8c152cd, "raise_softirq_irqoff" },
	{ 0x280f9f14, "__per_cpu_offset" },
	{ 0x43d1fd7c, "per_cpu__softnet_data" },
	{ 0xe93e887, "_spin_lock" },
	{ 0x2e9ca3e1, "__kfree_skb" },
	{ 0xb2372b5b, "_spin_unlock_irqrestore" },
	{ 0x48d35066, "alloc_skb" },
	{ 0x6804547e, "_spin_lock_irqsave" },
	{ 0x7ce3b391, "__mod_timer" },
	{ 0xda02d67, "jiffies" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0x6b2dc060, "dump_stack" },
	{ 0x26e96637, "request_irq" },
	{ 0xa34f1ef5, "crc32_le" },
	{ 0xedabf378, "pci_disable_device" },
	{ 0x2de0d5a6, "pci_release_regions" },
	{ 0x213a0658, "free_netdev" },
	{ 0x8bb43ea4, "dma_free_coherent" },
	{ 0x5ac2f881, "register_netdev" },
	{ 0x8dd74347, "pci_bus_read_config_byte" },
	{ 0x78e1c689, "dma_alloc_coherent" },
	{ 0xbbb6b172, "alloc_etherdev" },
	{ 0x8f69a758, "pci_set_master" },
	{ 0x8563ebdd, "pci_request_regions" },
	{ 0x8425183, "pci_enable_device" },
	{ 0x1b7d4074, "printk" },
	{ 0x89b301d4, "param_get_int" },
	{ 0x98bd6f46, "param_set_int" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("pci:v00001186d00004000sv*sd*bc*sc*i*");
