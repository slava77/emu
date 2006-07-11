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
#define SCHAR_INQR		_IOR(SCHAR_IOCTL_BASE, 6, 0)

#define DEBUG

#ifdef DEBUG
#define MSG(string, args...) if printk(KERN_DEBUG "schar: " string, ##args)
#else
#define MSG(string, args...)
#endif

/*
        ----------------------------------------------------------------------
        The following implements an interruptible wait_event
        with a timeout.  This is used instead of the function
        interruptible_sleep_on_timeout() since this is susceptible
        to race conditions.
        ----------------------------------------------------------------------
*/
#define __wait_event_interruptible_timeout(wq, condition, ret)		\
do {                                                          		\
        wait_queue_t __wait;                                  		\
        init_waitqueue_entry(&__wait, current);               		\
                                                              		\
        add_wait_queue(&wq, &__wait);                         		\
        for (;;) {                                            		\
                set_current_state(TASK_INTERRUPTIBLE);        		\
                if (condition)                                          \
                        break;                                		\
                if (!signal_pending(current)) {               		\
                        ret = schedule_timeout(ret);          		\
                        if (!ret)                             		\
                                break;                        		\
                        continue;                             		\
                }                                             		\
                ret = -ERESTARTSYS;                           		\
                break;                                        		\
        }                                                     		\
        current->state = TASK_RUNNING;                        		\
        remove_wait_queue(&wq, &__wait);                      		\
} while (0)

#define wait_event_interruptible_timeout(wq, condition, timeout)	\
({                                                              	\
        long __ret = timeout;                                   	\
        if (!(condition))                                       	\
                __wait_event_interruptible_timeout(wq, condition, __ret); \
        __ret;                                                          \
})

