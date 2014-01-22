

/*
 *	Example of simple character device driver (schar)
 */
#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

// #include <linux/config.h>

#include <linux/module.h>

#if defined(CONFIG_SMP)
#define __SMP__
#endif


#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/timer.h>	/* for timers */
#include <linux/fs.h>		/* file modes and device registration */
#include <linux/poll.h>		/* for poll */
#include <net/irda/wrapper.h>	/* mem_map_reserve,mem_map_unreserve */
#include <linux/proc_fs.h>
#include <linux/sysctl.h>
#include <linux/init.h>


#include <linux/netdevice.h>   // struct device and dev_xxx()
#include <linux/etherdevice.h> // eth_xxx()

#include <asm/io.h>
// #include <linux/bigphysarea.h>

#include "schar.h"

#define SCHAR_MAJOR_5 235

/* settable parameters */
static char *schar_name = NULL;



/* forward declarations for _fops */
void get_event(void);
int cleanup_exit2_5(void);
void ethcleanup_module(void);
int netif_rx_hook_5(struct sk_buff *skb);
int init_module2_5(void);
int ethinit_module(void);

static ssize_t schar_read_5(struct file *file, char *buf, size_t count, loff_t *offset); 
static ssize_t schar_write_5(struct file *file, const char *buf, size_t count,loff_t *offset);
static int schar_ioctl_5(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static int schar_open_5(struct inode *inode, struct file *file);
static int schar_release_5(struct inode *inode, struct file *file);

EXPORT_SYMBOL(netif_rx_hook_5);

static struct file_operations schar_fops = {
	read: schar_read_5, 
        write: schar_write_5,
	ioctl: schar_ioctl_5,
	open: schar_open_5,
	release: schar_release_5,
};




 static wait_queue_head_t schar_wq_5;
 static wait_queue_head_t  schar_poll_read_5;


/* sysctl entries */
static char schar_proc_string_5[SCHAR_MAX_SYSCTL];
static struct ctl_table_header *schar_root_header_5 = NULL;
static int schar_read_proc_5(ctl_table *ctl, int write, struct file *file,
			   void *buffer, size_t *lenp, loff_t *ppos);

static ctl_table schar_sysctl_table_5[] = {
	{ DEV_SCHAR_ENTRY,	/* binary id */
	  "5",			/* name */
	  &schar_proc_string_5,	/* data */
	  SCHAR_MAX_SYSCTL,	/* max size of output */
	  0644,			/* mode */
	  0,			/* child - none */
	  &schar_read_proc_5 },	/* set up text */
	{ 0 }
};

static ctl_table schar_dir_5[] = {
	{ DEV_SCHAR,		/* /proc/dev/schar */
	  "schar",		/* name */
	  NULL,
	  0,
	  0555,
	  schar_sysctl_table_5 },	/* the child */
	{ 0 }
};

static ctl_table schar_root_dir_5[] = {
	{ CTL_DEV,		/* /proc/dev */
	  "dev",		/* name */
	  NULL,
	  0,
	  0555,
	  schar_dir_5 },		/* the child */
	{ 0 }
};


#define MMT_BUF_SIZE 131072
#define SKB_EXTRA 14
char *pnt_ring;
static int pack_left;
static char *bufw;
static int nbufw={0};
static char *bufr;
static int endcond={0};
static int wakecond={0};
static int ERROR={0};
static int rd_tmo = {100};
//Added by Jinghua Liu
static int wakestatus=0;
static int pack_drop=0;



static unsigned long int proc_rpackets_5;
static unsigned long int proc_rbytesL_5;
static unsigned short int proc_rbytesH_5;
static unsigned short int proc_tpackets_5;
static unsigned long int proc_tbytesL_5;
static unsigned short int proc_tbytesH_5;



/* module parameters and descriptions */
// MODULE_PARM(schar_name, "s");
// MODULE_PARM_DESC(schar_name, "Name of device");
MODULE_LICENSE("GPL");

MODULE_DESCRIPTION("schar, Sample character with ethernet hook");
MODULE_AUTHOR("S. Durkin");

int init_module2_5(void)
{ 
  pnt_ring=kmalloc(MMT_BUF_SIZE,GFP_KERNEL);
  if (!pnt_ring)printk(KERN_INFO "failed kmalloc\n");
  pack_left=0;
  pack_drop=0;
  bufw=pnt_ring;
  nbufw=0;
  bufr=pnt_ring;
  ERROR=0;
  return 0;
}


int cleanup_exit2_5(void)
{
  kfree(pnt_ring);
  return 0;
}



int netif_rx_hook_5(struct sk_buff *skb)
{ 
  int i,icnt;
// write length to first word 
  if(nbufw+skb->len+16 > MMT_BUF_SIZE)
  { printk(KERN_INFO "eth_hook_5: out of memory, incoming packet dropped! \n");
    pack_drop++;
    ERROR=1;
    kfree_skb(skb);
    return 1;
  }

  *(int *)bufw=skb->len+14;
  bufw=bufw+2;
// fill bigphys memory and increment counters
  icnt=(skb->len+16)>>2;
    for(i=0;i<icnt;i++){
    *(unsigned long int *)(bufw+i*4)=*(unsigned long int *)(skb->data+i*4-14);
    }
  nbufw=nbufw+skb->len+16;
  bufw=bufw+skb->len+14; 
  pack_left=pack_left+1; 
 
  if(nbufw+9000> MMT_BUF_SIZE){
        printk(KERN_CRIT "LSD: Catastropic Error! Overwrote memory! Reset! \n");
	pack_left=0;
	bufw=pnt_ring;
	nbufw=0;
	bufr=pnt_ring;
     ERROR=1;
  }

// wake from blocking sleep
  if(wakecond==0&&pack_left>0){
                       wakecond=1;
		       wake_up_interruptible(&schar_wq_5);
                       wakestatus=2;
		       wake_up_interruptible(&schar_poll_read_5); 
  }

// calculate count and bytes for proc 
   proc_rpackets_5=proc_rpackets_5+1;  
   proc_rbytesL_5=proc_rbytesL_5+skb->len+SKB_EXTRA;
   if(proc_rbytesL_5>1000000000){
      proc_rbytesL_5=proc_rbytesL_5-1000000000;
      proc_rbytesH_5=proc_rbytesH_5+1;
   }
// return to gigabit driver
  kfree_skb(skb);
  return 1;
}


/*

int netif_rx_hook_5(struct sk_buff *skb)
{
  kfree_skb(skb);
  return 1;
}

*/

/* ********************* schar driver taken from Linux Programming ***********
   2nd Edition, Richard Stones, Neil Mathews           */




static int schar_ioctl_5(struct inode *inode, struct file *file,
		       unsigned int cmd, unsigned long arg)
{

	/* make sure that the command is really one of schar's */
	if (_IOC_TYPE(cmd) != SCHAR_IOCTL_BASE)
		return -ENOTTY;
		
	switch (cmd) {

		case SCHAR_RESET: {
                  pack_left=0;
                  pack_drop=0;
                  bufw=pnt_ring;
                  nbufw=0;
                  bufr=pnt_ring;
                  ERROR=0;
		//  printk(KERN_INFO "ioct:l SCHAR_RESET \n");
		return 0;
		}

		case SCHAR_END: { 
                if(wakecond==0){
                  endcond=1;
                  wakecond=1; 
   		  wake_up_interruptible(&schar_wq_5);
		  wake_up_interruptible(&schar_poll_read_5);
                } 
		// printk(KERN_INFO "ioct:l SCHAR_END %d  \n",blocking); 
	        return 0;
		}

	        case SCHAR_SET_TIMEOUT: {
                  if((int)arg >0)   rd_tmo=(int)arg;
		  //printk(KERN_INFO "SCHAR_SET_TIMEOUT %d ",rd_tmo);
                  return 0;  
	        }

	        case SCHAR_GET_TIMEOUT: {
                  return rd_tmo;  
	        }

                case SCHAR_INQR: {
                return (pack_left & 0xffff) | (endcond<<16) 
                       | (wakecond<<20) | (wakestatus<<24) | (ERROR<<28);
                }


		default: {
		  // MSG("ioctl: no such command\n");
			return -ENOTTY;
		}
	}

	/* to keep gcc happy */
	return 0;
}

static int schar_read_proc_5(ctl_table *ctl, int write, struct file *file,
			   void *buffer, size_t *lenp, loff_t *ppos)
{
	int len = 0;
	
	//	MSG("proc: %s\n", write ? "write" : "read");

	/* someone is writing data to us */
	/* v2.6remove
	if (write) {
		char *tmp = (char *) get_free_page(GFP_KERNEL);
		//	MSG("proc: someone wrote %u bytes\n", (unsigned)*lenp);
		if (tmp) {
		        free_page((unsigned long)tmp);
			file->f_pos += *lenp;
		}
		return 0;
	}
	v2.6remove */
	len += sprintf(schar_proc_string_5, "GIGABIT SIMPLE CHAR DRIVER ETH5\n\n");
	len += sprintf(schar_proc_string_5+len, " LEFT TO READ: \n");
        len += sprintf(schar_proc_string_5+len," pack_left\t\t%d packets\n",pack_left);
 	len += sprintf(schar_proc_string_5+len, " wakestatus\t\t%d\n",wakestatus);
        len += sprintf(schar_proc_string_5+len, " wakecond\t\t%d\n",wakecond);
	len += sprintf(schar_proc_string_5+len, " endcond\t\t%d\n",endcond);
	len += sprintf(schar_proc_string_5+len, " error\t\t%d\n",ERROR);	
	len += sprintf(schar_proc_string_5+len, " RECEIVE: \n");
	len += sprintf(schar_proc_string_5+len, "  recieve\t\t%ld packets\n",proc_rpackets_5);
        len += sprintf(schar_proc_string_5+len, "  receive    \t\t\t%02d%09ld bytes\n",proc_rbytesH_5,proc_rbytesL_5); 
        len += sprintf(schar_proc_string_5+len, "  memory  \t\t\t%09d bytes\n",MMT_BUF_SIZE);
        len += sprintf(schar_proc_string_5+len, "  dropped \t\t%d packets\n",pack_drop);
 	len += sprintf(schar_proc_string_5+len, " TRANSMIT: \n");
        len += sprintf(schar_proc_string_5+len, "  transmit\t\t%d packets \n",proc_tpackets_5);
        len += sprintf(schar_proc_string_5+len, "  transmit    \t\t\t%02d%09ld bytes\n\n",proc_tbytesH_5,proc_tbytesL_5); 
	*lenp = len;
	return proc_dostring(ctl, write, file, buffer, lenp, ppos);
	
	
}


static ssize_t schar_read_5(struct file *file, char *buf, size_t count,
			  loff_t *offset)
{ unsigned short int tbuf[3];
  int tsend,lsend;
  char *bufr2;
  
  /* remove sleep_on
  if(pack_left<=0&&endcond!=1){
                wakecond=0;
		interruptible_sleep_on(&schar_wq);
		if (signal_pending(current))return -EINTR;
  } 
  remove sleep_on */

  if(pack_left<=0&&endcond!=1)wakecond=0;
  wakestatus=1;
  wait_event_interruptible_timeout(schar_wq_5,(wakecond!=0),rd_tmo);
  wakestatus=3;
  if (signal_pending(current))return -EINTR;

  if(pack_left>0){
        lsend=*(unsigned short int *)bufr;
        bufr2=bufr+2;
        if (copy_to_user(buf,bufr2,lsend))
		return -EFAULT;
        count=lsend;
	file->f_pos += count;
        bufr=bufr+lsend+2;
        pack_left=pack_left-1;
        if(pack_left<=0){
                  pack_left=0;
                  bufw=pnt_ring;
                  nbufw=0;
                  bufr=pnt_ring;
        }

	return count;
  }else{
          tbuf[0]=4;
          tbuf[1]=0;
          tbuf[2]=0;
	  tsend=6;
          count=tsend;
          if (copy_to_user(buf,tbuf,tsend))
		return -EFAULT; 
   	return count; 
  }
  if(endcond==1){
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



static int schar_open_5(struct inode *inode, struct file *file)
{
	/* increment usage count */
	// MOD_INC_USE_COUNT;
//  printk(KERN_INFO " schar open \n");
	return 0;
}
static int schar_release_5(struct inode *inode, struct file *file)
{
  //MOD_DEC_USE_COUNT;
   
	return 0;
}


int ethinit_module(void)
{
	int res;
        init_module2_5();
	if (schar_name == NULL)
		schar_name = "schar5";
		
	
	/* register device with kernel */
	res = register_chrdev(SCHAR_MAJOR_5, schar_name, &schar_fops);
        // printk(KERN_INFO "lsd: %d %d %s \n",res,SCHAR_MAJOR_5,schar_name);
	if (res) {
	  // printk(KERN_INFO "can't register device with kernel \n");
	  // MSG("can't register device with kernel\n");
		return res;
	}
	
	/* register proc entry */
	schar_root_header_5 = register_sysctl_table(schar_root_dir_5, 0);
	// schar_root_dir_5->child->de->fill_inode = &schar_fill_inode_5;
	
        init_waitqueue_head(&schar_wq_5);
        init_waitqueue_head(&schar_poll_read_5);


	return 0;
}

void ethcleanup_module(void)
{
	/* unregister device and proc entry */
  cleanup_exit2_5();
	unregister_chrdev(SCHAR_MAJOR_5, "schar5");
	if (schar_root_header_5)
		unregister_sysctl_table(schar_root_header_5);
	
	return;
}



static ssize_t schar_write_5(struct file *file, const char *buf, size_t count,
			   loff_t *offset)
{
  //  struct sock *sk;
  // char *sbuf;
  // char *pnt2;
  // unsigned char *addr;
  int len;
  int err;
  // int i;
  static struct net_device *dev;
  static struct sk_buff *skb;
  static unsigned short proto=0;
  // printk(KERN_INFO " length %d \n",count);

  // sbuf=kmalloc(9000,GFP_KERNEL);
  len=count;
  dev=dev_get_by_name("eth5");
  err=-ENODEV;
  if (dev == NULL)
   goto out_unlock;
            
/*
 *      You may not queue a frame bigger than the mtu. This is the lowest level
 *      raw protocol and you must do your own fragmentation at this level.
*/
                
  err = -EMSGSIZE;
  if(len>dev->mtu+dev->hard_header_len)
  goto out_unlock;
     
  err = -ENOBUFS;
  //  skb = sock_wmalloc(sk, len+dev->hard_header_len+15, 0, GFP_KERNEL);
  skb=dev_alloc_skb(len+dev->hard_header_len+15);   
/*
 *      If the write buffer is full, then tough. At this level the user gets to
 *      deal with the problem - do your own algorithmic backoffs. That's far
 *      more flexible.
*/
              
  if (skb == NULL) 
  goto out_unlock;
     
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
   goto out_free;
     
   err = -ENETDOWN;
   if (!(dev->flags & IFF_UP))
   goto out_free;
     
/*
*      Now send it
*/
     
   dev_queue_xmit(skb);
   dev_put(dev); 
   // printk(KERN_INFO " lsd: len count %d %d %02x  \n",len,count,*(skb->data+98)&0xff);

   // kfree(sbuf);
   proc_tpackets_5=proc_tpackets_5+1;
   proc_tbytesL_5=proc_tbytesL_5+len;
   if(proc_tbytesL_5>1000000000){
      proc_tbytesL_5=proc_tbytesL_5-1000000000;
      proc_tbytesH_5=proc_tbytesH_5+1;
   } 
   return count;
     
   out_free:
     kfree_skb(skb);
   out_unlock:
     // if (dev)dev_put(dev);
      // kfree(sbuf);           
  return -EFAULT;
}

module_init(ethinit_module);
module_exit(ethcleanup_module);




