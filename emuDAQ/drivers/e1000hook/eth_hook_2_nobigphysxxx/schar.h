#define SCHAR_INC		1024
#define SCHAR_TIMER_DELAY	5*HZ
#define SCHAR_POOL_MIN		10*1024

#define SCHAR_MAX_SYSCTL	1024
#define DEV_SCHAR		10
#define DEV_SCHAR_ENTRY		1

/* ioctl's for schar. */
#define SCHAR_IOCTL_BASE	0xbb
#define SCHAR_RESET     	_IO(SCHAR_IOCTL_BASE, 0)
#define SCHAR_PAGES		_IOR(SCHAR_IOCTL_BASE, 1, int)
#define SCHAR_RING		_IOR(SCHAR_IOCTL_BASE, 2, int)
#define SCHAR_PMISSING		_IOR(SCHAR_IOCTL_BASE, 3, int)
#define SCHAR_EEVENT		_IOR(SCHAR_IOCTL_BASE, 4, int)

/* definitions used by DDUReadout */
#define SCHAR_END  		_IOR(SCHAR_IOCTL_BASE, 1, 0)
#define SCHAR_BLOCKON		_IOR(SCHAR_IOCTL_BASE, 2, 0)
#define SCHAR_BLOCKOFF		_IOR(SCHAR_IOCTL_BASE, 3, 0)


//#define DEBUG

#ifdef DEBUG
#define MSG(string, args...) if (schar_debug) printk(KERN_DEBUG "schar: " string, ##args)
#else
#define MSG(string, args...)
#endif
