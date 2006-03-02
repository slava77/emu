#define PAGE_SIZE 4096
#define BIGPHYS_PAGES_2 5000 
#define RING_PAGES_2 1000
#define RING_ENTRY_LENGTH 8
#define MAXPACKET_2 9100
#define MAXEVENT_2 30100
#define TAILMEM 100
#define TAILPOS  80
#define SKB_EXTRA 14
#define SKB_OFFSET -14

#define SCHAR_INC		1024
#define SCHAR_TIMER_DELAY	5*HZ
#define SCHAR_POOL_MIN		10*1024

#define SCHAR_MAX_SYSCTL	1024
#define DEV_SCHAR		10
#define DEV_SCHAR_ENTRY		1

/* ioctl's for schar. */
#define SCHAR_IOCTL_BASE	0xbb
#define SCHAR_RESET     	_IO(SCHAR_IOCTL_BASE, 0)
#define SCHAR_PAGES		_IOR(SCHAR_IOCTL_BASE, 1, 0)
#define SCHAR_RING		_IOR(SCHAR_IOCTL_BASE, 2, 0)
#define SCHAR_PMISSING		_IOR(SCHAR_IOCTL_BASE, 3, 0)
#define SCHAR_EEVENT		_IOR(SCHAR_IOCTL_BASE, 4, 0)

#define DEBUG

#ifdef DEBUG
#define MSG(string, args...) if (schar_debug) printk(KERN_DEBUG "schar: " string, ##args)
#else
#define MSG(string, args...)
#endif
