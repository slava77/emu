//-----------------------------------------------------------------------
// $Id: schar.h,v 2.0 2005/04/13 10:52:57 geurts Exp $
// $Log: schar.h,v $
// Revision 2.0  2005/04/13 10:52:57  geurts
// Makefile
//
//
//-----------------------------------------------------------------------
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
#define SCHAR_END		_IOR(SCHAR_IOCTL_BASE, 1, 0)
#define SCHAR_BLOCKON		_IOR(SCHAR_IOCTL_BASE, 2, 0)
#define SCHAR_BLOCKOFF		_IOR(SCHAR_IOCTL_BASE, 3, 0)


//needed for ddu2004 only
// MemoryMapped read out parameters
#define PAGE_SIZE 4096
#define BIGPHYS_PAGES_2 50000 
#define RING_PAGES_2 1000
#define RING_ENTRY_LENGTH 8
#define MAXPACKET_2 9100
#define TAILMEM 100
#define TAILPOS  80
#define SKB_EXTRA 14
#define SKB_OFFSET -14

