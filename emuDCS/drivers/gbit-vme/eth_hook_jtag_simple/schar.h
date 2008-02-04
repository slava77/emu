#define SCHAR_MAJOR		42

#define SCHAR_INC		1024
#define SCHAR_TIMER_DELAY	5*HZ
#define SCHAR_POOL_MIN		10*1024

#define SCHAR_MAX_SYSCTL	512
#define DEV_SCHAR		10
#define DEV_SCHAR_ENTRY		1

/* ioctl's for schar. */
#define SCHAR_IOCTL_BASE	0xbb
#define SCHAR_RESET     	_IO(SCHAR_IOCTL_BASE, 0)
#define SCHAR_END		_IOR(SCHAR_IOCTL_BASE, 1, int)
#define SCHAR_SET_TIMEOUT       _IOW(SCHAR_IOCTL_BASE, 2, int)
#define SCHAR_GET_TIMEOUT       _IOR(SCHAR_IOCTL_BASE, 3, int)
#define SCHAR_INQR              _IOR(SCHAR_IOCTL_BASE, 6, int)

#define DEBUG

#ifdef DEBUG
#define MSG(string, args...) if (schar_debug) printk(KERN_DEBUG "schar: " string, ##args)
#else
#define MSG(string, args...)
#endif
