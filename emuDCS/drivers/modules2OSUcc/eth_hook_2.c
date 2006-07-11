/*
 *	Example of simple character device driver (schar)
 */
#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/config.h>

#include <linux/module.h>

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/timer.h>	/* for timers */
#include <linux/fs.h>		/* file modes and device registration */

#include <linux/poll.h>		/* for poll */
#include <linux/proc_fs.h>
#include <linux/sysctl.h>
#include <linux/init.h>

// #include <linux/spinlock.h>

#include <linux/netdevice.h>   // struct device and dev_xxx()
#include <linux/etherdevice.h> // eth_xxx()


#include "eth_hook_2.h"

/* settable parameters */
static char *schar_name = NULL;



/* forward declarations for _fops */
void cleanup_module(void);
int netif_rx_hook_2(struct sk_buff *skb);
int init_module(void);

static ssize_t schar_read_2(struct file *file, char *buf, size_t count, loff_t *offset); 
static ssize_t schar_write_2(struct file *file, const char *buf, size_t count,loff_t *offset);
static int schar_ioctl_2(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static int schar_open_2(struct inode *inode, struct file *file);
static int schar_release_2(struct inode *inode, struct file *file);

// static spinlock_t eth_lock2;

static struct file_operations schar_fops_2 = {
	read: schar_read_2, 
        write: schar_write_2,
	ioctl: schar_ioctl_2,
	open: schar_open_2,
	release: schar_release_2,
};


 static wait_queue_head_t schar_wq_2;
// static wait_queue_head_t  schar_poll_read_2;


/* sysctl entries */
static char schar_proc_string_2[SCHAR_MAX_SYSCTL];
static struct ctl_table_header *schar_root_header_2 = NULL;
static int schar_read_proc_2(ctl_table *ctl, int write, struct file *file,
			   void *buffer, size_t *lenp);

static ctl_table schar_sysctl_table_2[] = {
	{ DEV_SCHAR_ENTRY,	/* binary id */
	  "2",			/* name */
	  &schar_proc_string_2,	/* data */
	  SCHAR_MAX_SYSCTL,	/* max size of output */
	  0644,			/* mode */
	  0,			/* child - none */
	  &schar_read_proc_2 },	/* set up text */
	{ 0 }
};

static ctl_table schar_dir_2[] = {
	{ DEV_SCHAR,		/* /proc/dev/schar */
	  "schar",		/* name */
	  NULL,
	  0,
	  0555,
	  schar_sysctl_table_2 },	/* the child */
	{ 0 }
};

static ctl_table schar_root_dir_2[] = {
	{ CTL_DEV,		/* /proc/dev */
	  "dev",		/* name */
	  NULL,
	  0,
	  0555,
	  schar_dir_2 },		/* the child */
	{ 0 }
};


#define MMT_BUF_SIZE 131072
#define SKB_EXTRA 14
char *pnt_ring_2;
static int pack_left_2;
static char *bufw_2;
static int nbufw_2={0};
static char *bufr_2;
static int endcond_2={0};
static int wakecond_2={0};
static int ERROR_2={0};
//Added by Jinghua Liu
static int wakestatus_2=0;
static int pack_drop_2=0;

static unsigned long int proc_rpackets_2;
static unsigned long int proc_rbytesL_2;
static unsigned short int proc_rbytesH_2;
static unsigned long int proc_tpackets_2;
static unsigned long int proc_tbytesL_2;
static unsigned short int proc_tbytesH_2;



/* module parameters and descriptions */
MODULE_PARM(schar_name, "s");
MODULE_PARM_DESC(schar_name, "Name of device");


MODULE_DESCRIPTION("schar2, Sample character with ethernet hook");
MODULE_AUTHOR("S. Durkin");


int netif_rx_hook_2(struct sk_buff *skb)
{ 
  int i,icnt;
  unsigned long int flags;
  // spin_lock_irqsave(&eth_lock,flags);
// write length to first word
  if(nbufw_2+skb->len+16 > MMT_BUF_SIZE)
    { printk(KERN_INFO "eth_hook: out of memory, incoming packet dropped! \n");
      pack_drop_2++;
      ERROR_2=1;
      kfree_skb(skb);
      return 1;
    }
  *(int *)bufw_2=skb->len+14;
  bufw_2=bufw_2+2;
// fill bigphys memory and increment counters
  icnt=(skb->len+16)>>2;
    for(i=0;i<icnt;i++){
    *(unsigned long int *)(bufw_2+i*4)=*(unsigned long int *)(skb->data+i*4-14);
    }
  nbufw_2=nbufw_2+skb->len+16;
  bufw_2=bufw_2+skb->len+14; 
  pack_left_2++; 
 
/* We don't want to see a catastrophe, do we?
  if(nbufw_2+9000> MMT_BUF_SIZE){
        printk(KERN_CRIT "LSD: Catastropic Error! Overwrote memory! \n"); 
    ERROR_2=1;
  }
*/
  //   spin_unlock_irqrestore(&eth_lock,flags);
// wake from blocking sleep
  if(wakecond_2==0&&pack_left_2>0){
                     wake_up_interruptible(&schar_wq_2);
                     wakestatus_2=2;
//		       wake_up_interruptible(&schar_poll_read_2);
                     wakecond_2=1; 
  }

// calculate count and bytes for proc 
   proc_rpackets_2=proc_rpackets_2+1;  
   proc_rbytesL_2=proc_rbytesL_2+skb->len+SKB_EXTRA;
   if(proc_rbytesL_2>1000000000){
      proc_rbytesL_2=proc_rbytesL_2-1000000000;
      proc_rbytesH_2=proc_rbytesH_2+1;
   }
// return to gigabit driver
  kfree_skb(skb);
  return 1;
}


/* ********************* schar driver taken from Linux Programming ***********
   2nd Edition, Richard Stones, Neil Mathews           */


static int schar_ioctl_2(struct inode *inode, struct file *file,
		       unsigned int cmd, unsigned long arg)
{

/*    printk(KERN_INFO "ioctl: inside ioctl \n"); */

	/* make sure that the command is really one of schar's */
	if (_IOC_TYPE(cmd) != SCHAR_IOCTL_BASE) {
           printk(KERN_INFO "ioctl: not valid \n");
           return -ENOTTY;
	}
		
	switch (cmd) {

		case SCHAR_RESET: {
                  pack_left_2=0;
                  pack_drop_2=0;
                  bufw_2=pnt_ring_2;
                  nbufw_2=0;
                  bufr_2=pnt_ring_2;
                  ERROR_2=0;
		  printk(KERN_INFO "ioctl: SCHAR_RESET \n");
		return 0;
		}

		case SCHAR_END: { 
                  if(wakecond_2==0){
                    endcond_2=1;
                    wakecond_2=1; 
   		    wake_up_interruptible(&schar_wq_2);
//		    wake_up_interruptible(&schar_poll_read_2);
                  } 
		// printk(KERN_INFO "ioct:l SCHAR_END %d  \n",blocking); 
	        return 0;
		}

                case SCHAR_INQR: {
                return (pack_left_2 & 0xffff) | (endcond_2<<16)
                         | (wakecond_2<<20) | (wakestatus_2<<24) | (ERROR_2<<28);
               }
                                                                       
       		default: {
		  // MSG("ioctl: no such command\n");
			return -ENOTTY;
		}
	}

	/* to keep gcc happy */
	return 0;
}

static int schar_read_proc_2(ctl_table *ctl, int write, struct file *file,
			   void *buffer, size_t *lenp)
{
	int len = 0;
	
	//	MSG("proc: %s\n", write ? "write" : "read");

	/* someone is writing data to us */
	if (write) {
		char *tmp = (char *) get_free_page(GFP_KERNEL);
		//	MSG("proc: someone wrote %u bytes\n", (unsigned)*lenp);
		if (tmp) {
		  /*	if (!copy_from_user(tmp, buffer, *lenp))
			// MSG("proc: %s", tmp); */
			free_page((unsigned long)tmp);
			file->f_pos += *lenp;
		}
		return 0;
	}
	len += sprintf(schar_proc_string_2, "GIGABIT SIMPLE CHAR DRIVER\n\n");
	len += sprintf(schar_proc_string_2+len, " LEFT TO READ: \n");
        len += sprintf(schar_proc_string_2+len," pack_left\t\t%d packets\n",pack_left_2); 
	len += sprintf(schar_proc_string_2+len, " wakestatus_2\t\t%d\n",wakestatus_2);
	len += sprintf(schar_proc_string_2+len, " wakecond_2\t\t%d\n",wakecond_2);
	len += sprintf(schar_proc_string_2+len, " endcond_2\t\t%d\n",endcond_2);
        len += sprintf(schar_proc_string_2+len, " error_2\t\t%d\n",ERROR_2);
	len += sprintf(schar_proc_string_2+len, " RECEIVE: \n");
	len += sprintf(schar_proc_string_2+len, "  recieve\t\t%ld packets\n",proc_rpackets_2);
        len += sprintf(schar_proc_string_2+len, "  receive    \t\t\t%02d%09ld bytes\n",proc_rbytesH_2,proc_rbytesL_2); 
        len += sprintf(schar_proc_string_2+len, "  memory  \t\t\t%09d bytes\n",MMT_BUF_SIZE);
        len += sprintf(schar_proc_string_2+len, "  dropped \t\t%d packets\n",pack_drop_2);
 	len += sprintf(schar_proc_string_2+len, " TRANSMIT: \n");
        len += sprintf(schar_proc_string_2+len, "  transmit\t\t%ld packets\n",proc_tpackets_2);
        len += sprintf(schar_proc_string_2+len, "  transmit    \t\t\t%02d%09ld bytes\n\n",proc_tbytesH_2,proc_tbytesL_2); 
	*lenp = len;
	return proc_dostring(ctl, write, file, buffer, lenp);
		
}


static ssize_t schar_read_2(struct file *file, char *buf, size_t count,
			  loff_t *offset)
{ unsigned short int tbuf[3];
 unsigned long int flags;
  int tsend,lsend;
  char *bufr2;
  int i,j;
  
  if(pack_left_2<=0&&endcond_2!=1){
                wakecond_2=0;
                wakestatus_2=1;
//		interruptible_sleep_on(&schar_wq_2);
// Modified by Jinghua Liu. Don't use interruptible_sleep_on.
// Set the timeout to 6000 for now.
                wait_event_interruptible_timeout(schar_wq_2, pack_left_2, 6000);
                wakestatus_2=3;
		if (signal_pending(current))return -EINTR;
  }

  if(pack_left_2>0){
    //  spin_lock_irqsave(&eth_lock,flags);
        lsend=*(unsigned short *)bufr_2;
        bufr2=bufr_2+2; 
        if (copy_to_user(buf,bufr2,lsend))
		return -EFAULT;
        count=lsend;
	file->f_pos += count;
        bufr_2=bufr_2+lsend+2;
        pack_left_2--;
        if(pack_left_2<=0){
                  pack_left_2=0;
                  bufw_2=pnt_ring_2;
                  nbufw_2=0;
                  bufr_2=pnt_ring_2;
        }
	//  spin_unlock_irqrestore(&eth_lock,flags);
	return count;
  }
  if(endcond_2==1){
          tbuf[0]=6;
          tbuf[1]=0;
          tbuf[2]=0;
	  tsend=6;
          count=tsend;
          if (copy_to_user(buf,tbuf,tsend))
		return -EFAULT;  
  }
  return count;
}



static int schar_open_2(struct inode *inode, struct file *file)
{
	/* increment usage count */
/*  printk(KERN_INFO "opening"); */
	MOD_INC_USE_COUNT;
	return 0;
}

static int schar_release_2(struct inode *inode, struct file *file)
{
	MOD_DEC_USE_COUNT;
   
	return 0;
}


int init_module(void)
{
	int res;
	
/* initialise static variables */

     pnt_ring_2=kmalloc(MMT_BUF_SIZE,GFP_KERNEL);
     if (!pnt_ring_2)
     {  printk(KERN_INFO "failed kmalloc\n");
        return -EFAULT;
     }
     pack_left_2=0;
     pack_drop_2=0;
     bufw_2=pnt_ring_2;
     nbufw_2=0;
     bufr_2=pnt_ring_2;
     ERROR_2=0;

	if (schar_name == NULL)
		schar_name = "schar2";
		
	
	/* register device with kernel */
	res = register_chrdev(SCHAR_MAJOR, schar_name, &schar_fops_2);
        printk(KERN_INFO "lsd: %d %d %s \n",res,SCHAR_MAJOR,schar_name);
	if (res) {
	  printk(KERN_INFO "WOOO,can't register device with kernel \n");
	  // MSG("can't register device with kernel\n");
		return res;
	}
	
	/* register proc entry */
	schar_root_header_2 = register_sysctl_table(schar_root_dir_2, 0);
	// schar_root_dir->child->de->fill_inode = &schar_fill_inode;
	
        init_waitqueue_head(&schar_wq_2);
        // init_waitqueue_head(&schar_poll_read_2);

	return 0;
}

void cleanup_module(void)
{
        if(pnt_ring_2) kfree(pnt_ring_2);
	/* unregister device and proc entry */
	unregister_chrdev(SCHAR_MAJOR, "schar2");
	if (schar_root_header_2)
		unregister_sysctl_table(schar_root_header_2);
	
	return;
}



static ssize_t schar_write_2(struct file *file, const char *buf, size_t count,
			   loff_t *offset)
{
  //  struct sock *sk;
  // char *sbuf;
  // char *pnt2;
  // unsigned char *addr;
  int len;
  int err;
  int i;
  static struct net_device *dev;
  static struct sk_buff *skb;
  static unsigned short proto=0;
  // printk(KERN_INFO "write packet  length %d \n",count);
  /* on a very fast dual processor it was necessary to add 
     the following line. Ben's controller cannot seem to 
     handle too many packets too close together! I am
     consulting Ben... */
  for(i=0;i<30000;i++);
  // sbuf=kmalloc(9000,GFP_KERNEL);
  len=count;
  dev=dev_get_by_name("eth3");
  err=-ENODEV;
  if (dev == NULL)
    {printk(KERN_INFO "dev is null \n"); goto out_unlock;}
            
/*
 *      You may not queue a frame bigger than the mtu. This is the lowest level
 *      raw protocol and you must do your own fragmentation at this level.
*/
                
  err = -EMSGSIZE;
  if(len>dev->mtu+dev->hard_header_len)
    {printk(KERN_INFO "len too large \n");goto out_unlock;}
     
  err = -ENOBUFS;
  //  skb = sock_wmalloc(sk, len+dev->hard_header_len+15, 0, GFP_KERNEL);
  skb=dev_alloc_skb(len+dev->hard_header_len+15);   
/*
 *      If the write buffer is full, then tough. At this level the user gets to
 *      deal with the problem - do your own algorithmic backoffs. That's far
 *      more flexible.
*/
              
  if (skb == NULL) 
    {printk(KERN_INFO "write buffer full \n");goto out_unlock;}
     
/*
*      Fill it in 
*/
              
/* FIXME: Save some space for broken drivers that write a
* hard header at transmission time by themselves. PPP is the
* notable one here. This should really be fixed at the driver level.
*/
   skb_reserve(skb,(dev->hard_header_len+15)&~15);
   skb->nh.raw = skb->data;
   proto=htons(ETH_P_ALL);
   /*     	if (dev->hard_header) {
		int res;
		err = -EINVAL;
                addr=NULL;
		res = dev->hard_header(skb, dev, ntohs(proto), addr, NULL, len);
			skb->tail = skb->data;
			skb->len = 0;
			} */
        			
                        skb->tail = skb->data;
			skb->len = 0;

/* Try to align data part correctly */
			/*      if (dev->hard_header) {
      skb->data -= dev->hard_header_len;
      skb->tail -= dev->hard_header_len;
      } */
 			
  
     //  printk(KERN_INFO " header length %d  \n",dev->hard_header_len);
/* Returns -EFAULT on error */
   //  err = memcpy_fromiovec(skb_put(skb,len), msg->msg_iov, len);
	
    err = copy_from_user(skb_put(skb,len),buf, count);
   // err = memcpy_fromio(skb_put(skb,len),sbuf,len);
   // printk(KERN_INFO " lsd: len count %d %d %02x  \n",len,count,*(skb->data+98)&0xff);
   skb->protocol = htons(ETH_P_ALL);
   skb->dev = dev;
   skb->priority = 0;
   // skb->pkt_type=PACKET_MR_PROMISC;
   skb->ip_summed=CHECKSUM_UNNECESSARY;
   if (err)
     {printk(KERN_INFO "err \n");goto out_free;}
     
   err = -ENETDOWN;
   if (!(dev->flags & IFF_UP))
     {printk(KERN_INFO " device is down \n");goto out_free;}
     
/*
*      Now send it
*/
     
   dev_queue_xmit(skb);
   dev_put(dev); 

   // kfree(sbuf);
   proc_tpackets_2=proc_tpackets_2+1;
   proc_tbytesL_2=proc_tbytesL_2+len;
   if(proc_tbytesL_2>1000000000){
      proc_tbytesL_2=proc_tbytesL_2-1000000000;
      proc_tbytesH_2=proc_tbytesH_2+1;
   } 
   return count;
     
   out_free:
   kfree_skb(skb);
   out_unlock:
     // if (dev)dev_put(dev);
      // kfree(sbuf);           
  return -EFAULT;
}
