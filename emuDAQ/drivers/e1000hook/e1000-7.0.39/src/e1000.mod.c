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
	{ 0x3ce4ca6f, "disable_irq" },
	{ 0x8dd74347, "pci_bus_read_config_byte" },
	{ 0x7da8156e, "__kmalloc" },
	{ 0x2b2980f2, "_spin_trylock" },
	{ 0xf9a482f9, "msleep" },
	{ 0x2e9ca3e1, "__kfree_skb" },
	{ 0x91091698, "mem_map" },
	{ 0xd6ee688f, "vmalloc" },
	{ 0x89b301d4, "param_get_int" },
	{ 0xbbb6b172, "alloc_etherdev" },
	{ 0xedabf378, "pci_disable_device" },
	{ 0xe93e887, "_spin_lock" },
	{ 0x9861d5e6, "ethtool_op_get_sg" },
	{ 0x773d1b0a, "schedule_work" },
	{ 0x806d5133, "param_array_get" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0x2de0d5a6, "pci_release_regions" },
	{ 0xd6d5cbc3, "linkwatch_fire_event" },
	{ 0xd8c152cd, "raise_softirq_irqoff" },
	{ 0x3952fdd2, "pci_enable_wake" },
	{ 0x2fd1d81c, "vfree" },
	{ 0xf18549b9, "pci_bus_write_config_word" },
	{ 0x98bd6f46, "param_set_int" },
	{ 0x6804547e, "_spin_lock_irqsave" },
	{ 0xe28e6b41, "__netdev_watchdog_up" },
	{ 0x1d26aa98, "sprintf" },
	{ 0x89cef6fb, "param_array_set" },
	{ 0x66b41ba1, "pci_dev_driver" },
	{ 0xda02d67, "jiffies" },
	{ 0xa44c412b, "kunmap_atomic" },
	{ 0x771b82cc, "netif_rx" },
	{ 0x482b1e77, "__pskb_pull_tail" },
	{ 0x8f69a758, "pci_set_master" },
	{ 0x77d96a71, "kmap_atomic" },
	{ 0xcd9f3914, "__alloc_pages" },
	{ 0xde737a3, "del_timer_sync" },
	{ 0x31165ca4, "pci_set_dma_mask" },
	{ 0x1b7d4074, "printk" },
	{ 0x7b8ce1f3, "ethtool_op_get_link" },
	{ 0x48d35066, "alloc_skb" },
	{ 0xfa0c2cd9, "pci_find_device" },
	{ 0x213a0658, "free_netdev" },
	{ 0x5ac2f881, "register_netdev" },
	{ 0x8bb43ea4, "dma_free_coherent" },
	{ 0x32a5b47a, "netif_receive_skb" },
	{ 0x65ed66d6, "pci_bus_write_config_dword" },
	{ 0xb2372b5b, "_spin_unlock_irqrestore" },
	{ 0xe7f7c901, "mod_timer" },
	{ 0x9eac042a, "__ioremap" },
	{ 0x3caeb253, "_spin_unlock" },
	{ 0xe38117fd, "pci_enable_msi" },
	{ 0x567cb954, "contig_page_data" },
	{ 0x78e1c689, "dma_alloc_coherent" },
	{ 0x8748d847, "irq_stat" },
	{ 0xe523ad75, "synchronize_irq" },
	{ 0x3980aac1, "unregister_reboot_notifier" },
	{ 0x48f293a9, "pci_set_mwi" },
	{ 0x463a476a, "skb_over_panic" },
	{ 0x7dceceac, "capable" },
	{ 0x10eb30b, "pci_bus_read_config_word" },
	{ 0x1cc6719a, "register_reboot_notifier" },
	{ 0x50b372cd, "ethtool_op_set_sg" },
	{ 0xdff5f0ae, "pci_bus_read_config_dword" },
	{ 0x26e96637, "request_irq" },
	{ 0x17d59d01, "schedule_timeout" },
	{ 0x6b2dc060, "dump_stack" },
	{ 0x6d69932f, "eth_type_trans" },
	{ 0x44f0bcfa, "pci_unregister_driver" },
	{ 0xcc5005fe, "msleep_interruptible" },
	{ 0x280f9f14, "__per_cpu_offset" },
	{ 0x18541cf5, "pci_set_power_state" },
	{ 0xc71a3c97, "pci_clear_mwi" },
	{ 0xfcec0987, "enable_irq" },
	{ 0x37a0cba, "kfree" },
	{ 0x2e60bace, "memcpy" },
	{ 0x801678, "flush_scheduled_work" },
	{ 0x28b58c79, "___pskb_trim" },
	{ 0x8563ebdd, "pci_request_regions" },
	{ 0x32008bbe, "pci_disable_msi" },
	{ 0xedc03953, "iounmap" },
	{ 0x759659f2, "put_page" },
	{ 0xa5ba5f5, "unregister_netdev" },
	{ 0x3be35a82, "ethtool_op_get_tso" },
	{ 0x8425183, "pci_enable_device" },
	{ 0x61f950a7, "pci_set_consistent_dma_mask" },
	{ 0x9e7d6bd0, "__udelay" },
	{ 0xf20dabd8, "free_irq" },
	{ 0x61079cf0, "pci_register_driver" },
	{ 0x43d1fd7c, "per_cpu__softnet_data" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("pci:v00008086d00001000sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00001001sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00001004sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00001008sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00001009sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d0000100Csv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d0000100Dsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d0000100Esv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d0000100Fsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00001010sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00001011sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00001012sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00001013sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00001014sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00001015sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00001016sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00001017sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00001018sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00001019sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d0000101Asv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d0000101Dsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d0000101Esv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00001026sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00001027sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00001028sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d0000105Esv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d0000105Fsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00001060sv*sd*bc*sc*i*");
