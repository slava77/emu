#ifndef __SLC_version_h__
#define __SLC_version_h__

#include <linux/version.h>

#if   LINUX_VERSION_CODE == KERNEL_VERSION(2,6,18)
#define SLC_VERSION 5
#elif LINUX_VERSION_CODE == KERNEL_VERSION(2,6,32)
#define SLC_VERSION 6
#else
#warning Do not know this version of the kernel.
#endif

#endif
