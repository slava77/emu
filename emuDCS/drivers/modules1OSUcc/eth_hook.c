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
#include <linux/wrapper.h>	/* mem_map_reserve,mem_map_unreserve */
#include <linux/proc_fs.h>
#include <linux/sysctl.h>
#include <linux/init.h>

#include <linux/spinlock.h>

#include <linux/netdevice.h>   // struct device and dev_xxx()
#include <linux/etherdevice.h> // eth_xxx()

#include <asm/io.h>
#include <linux/bigphysarea.h>

#include "eth_hook.h"

/* settable parameters */
static char *schar_name = NULL;



/* forward declarations for _fops */
void get_event(void);
int cleanup_exit2(void);
void cleanup_module(void);
int netif_rx_hook(struct sk_buff *skb);
int init_module2(void);
int init_module(void);

static ssize_t schar_read(struct file *file, char *buf, size_t count, loff_t *offset); 
static ssize_t schar_write(struct file *file, const char *buf, size_t count,loff_t *offset);
static int schar_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static int schar_open(struct inode *inode, struct file *file);
static int schar_release(struct inode *inode, struct file *file);

static spinlock_t eth_lock;

static struct file_operations schar_fops = {
	read: schar_read, 
        write: schar_write,
	ioctl: schar_ioctl,
	open: schar_open,
	release: schar_release,
};




 static wait_queue_head_t schar_wq;
 static wait_queue_head_t  schar_poll_read;


/* sysctl entries */
static char schar_proc_string[SCHAR_MAX_SYSCTL];
static struct ctl_table_header *schar_root_header = NULL;
static int schar_read_proc(ctl_table *ctl, int write, struct file *file,
			   void *buffer, size_t *lenp);

static ctl_table schar_sysctl_table[] = {
	{ DEV_SCHAR_ENTRY,	/* binary id */
	  "0",			/* name */
	  &schar_proc_string,	/* data */
	  SCHAR_MAX_SYSCTL,	/* max size of output */
	  0644,			/* mode */
	  0,			/* child - none */
	  &schar_read_proc },	/* set up text */
	{ 0 }
};

static ctl_table schar_dir[] = {
	{ DEV_SCHAR,		/* /proc/dev/schar */
	  "schar",		/* name */
	  NULL,
	  0,
	  0555,
	  schar_sysctl_table },	/* the child */
	{ 0 }
};

static ctl_table schar_root_dir[] = {
	{ CTL_DEV,		/* /proc/dev */
	  "dev",		/* name */
	  NULL,
	  0,
	  0555,
	  schar_dir },		/* the child */
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
//Added by Jinghua Liu
static int wakestatus=0;

static unsigned long int proc_rpackets;
static unsigned long int proc_rbytesL;
static unsigned short int proc_rbytesH;
static unsigned long int proc_tpackets;
static unsigned long int proc_tbytesL;
static unsigned short int proc_tbytesH;



/* module parameters and descriptions */
MODULE_PARM(schar_name, "s");
MODULE_PARM_DESC(schar_name, "Name of device");


MODULE_DESCRIPTION("schar, Sample character with ethernet hook");
MODULE_AUTHOR("S. Durkin");

int init_module2(void)
{ 
  pnt_ring=kmalloc(MMT_BUF_SIZE,GFP_KERNEL);
  if (!pnt_ring)printk(KERN_INFO "failed kmalloc\n");
  pack_left=0;
  bufw=pnt_ring;
  nbufw=0;
  bufr=pnt_ring;
  ERROR=0;
  return 0;
}


int cleanup_exit2(void)
{
  kfree(pnt_ring);
  return 0;
}



int netif_rx_hook(struct sk_buff *skb)
{ 
  int i,icnt;
  unsigned long int flags;
  // spin_lock_irqsave(&eth_lock,flags);
// write length to first word
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
        printk(KERN_CRIT "LSD: Catastropic Error! Overwrote memory! \n"); 
    ERROR=1;
  }
  //   spin_unlock_irqrestore(&eth_lock,flags);
// wake from blocking sleep
  if(wakecond==0&&pack_left>0){
                     wake_up_interruptible(&schar_wq);
                     wakestatus=2;
//		       wake_up_interruptible(&schar_poll_read);
                     wakecond=1; 
  }

// calculate count and bytes for proc 
   proc_rpackets=proc_rpackets+1;  
   proc_rbytesL=proc_rbytesL+skb->len+SKB_EXTRA;
   if(proc_rbytesL>1000000000){
      proc_rbytesL=proc_rbytesL-1000000000;
      proc_rbytesH=proc_rbytesH+1;
   }
// return to gigabit driver
  kfree_skb(skb);
  return 1;
}


/*

int netif_rx_hook(struct sk_buff *skb)
{
  kfree_skb(skb);
  return 1;
}

*/

/* ********************* schar driver taken from Linux Programming ***********
   2nd Edition, Richard Stones, Neil Mathews           */




static int schar_ioctl(struct inode *inode, struct file *file,
		       unsigned int cmd, unsigned long arg)
{

    printk(KERN_INFO "ioct:inside ioctl \n");

	/* make sure that the command is really one of schar's */
  if (_IOC_TYPE(cmd) != SCHAR_IOCTL_BASE){
    printk(KERN_INFO "ioct:not valid \n");
    return -ENOTTY;
  }
		
	switch (cmd) {

		case SCHAR_RESET: {
                  pack_left=0;
                  bufw=pnt_ring;
                  nbufw=0;
                  bufr=pnt_ring;
                  ERROR=0;
		  printk(KERN_INFO "ioct:l SCHAR_RESET \n");
		return 0;
		}

		case SCHAR_END: { 
                if(wakecond==0){
                  endcond=1;
                  wakecond=1; 
   		  wake_up_interruptible(&schar_wq);
//		  wake_up_interruptible(&schar_poll_read);
                } 
		// printk(KERN_INFO "ioct:l SCHAR_END %d  \n",blocking); 
	        return 0;
		}


		default: {
		  // MSG("ioctl: no such command\n");
			return -ENOTTY;
		}
	}

	/* to keep gcc happy */
	return 0;
}

static int schar_read_proc(ctl_table *ctl, int write, struct file *file,
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
	len += sprintf(schar_proc_string, "GIGABIT DRIVER SIMPLE JTAG\n\n");
	len += sprintf(schar_proc_string+len, " LEFT TO READ: \n");
        len += sprintf(schar_proc_string+len," pack_left\t\t%d packets\n",pack_left); 
	len += sprintf(schar_proc_string+len, " wakestatus\t\t%d\n",wakestatus);
	len += sprintf(schar_proc_string+len, " wakecond\t\t%d\n",wakecond);
	len += sprintf(schar_proc_string+len, " endcond\t\t%d\n",endcond);
	len += sprintf(schar_proc_string+len, " RECEIVE: \n");
	len += sprintf(schar_proc_string+len, "  recieve\t\t%ld packets\n",proc_rpackets);
        len += sprintf(schar_proc_string+len, "  receive    \t\t\t%02d%09ld bytes\n",proc_rbytesH,proc_rbytesL); 
        len += sprintf(schar_proc_string+len, "  memory  \t\t\t%09d bytes\n",MMT_BUF_SIZE);
 	len += sprintf(schar_proc_string+len, " TRANSMIT: \n");
        len += sprintf(schar_proc_string+len, "  transmit\t\t%ld packets \n",proc_tpackets);
        len += sprintf(schar_proc_string+len, "  transmit    \t\t\t%02d%09ld bytes\n\n",proc_tbytesH,proc_tbytesL); 
	*lenp = len;
	return proc_dostring(ctl, write, file, buffer, lenp);
	
	
}


static ssize_t schar_read(struct file *file, char *buf, size_t count,
			  loff_t *offset)
{ unsigned short int tbuf[3];
 unsigned long int flags;
  int tsend,lsend;
  char *bufr2;
  int i,j;
  
  if(pack_left<=0&&endcond!=1){
                wakecond=0;
                wakestatus=1;
//		interruptible_sleep_on(&schar_wq);
// Modified by Jinghua Liu. Don't use interruptible_sleep_on.
// Set the timeout to 6000 for now.
                wait_event_interruptible_timeout(schar_wq, pack_left, 6000);
                wakestatus=3;
		if (signal_pending(current))return -EINTR;
  }

  if(pack_left>0){
    //  spin_lock_irqsave(&eth_lock,flags);
        lsend=*(unsigned short *)bufr;
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
	//  spin_unlock_irqrestore(&eth_lock,flags);
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



static int schar_open(struct inode *inode, struct file *file)
{
	/* increment usage count */
  printk(KERN_INFO "opening");
	MOD_INC_USE_COUNT;
	return 0;
}
static int schar_release(struct inode *inode, struct file *file)
{
	MOD_DEC_USE_COUNT;
   
	return 0;
}


int init_module(void)
{
	int res;
        init_module2();
	if (schar_name == NULL)
		schar_name = "schar";
		
	
	/* register device with kernel */
	res = register_chrdev(SCHAR_MAJOR, schar_name, &schar_fops);
        // printk(KERN_INFO "lsd: %d %d %s \n",res,SCHAR_MAJOR,schar_name);
	if (res) {
	  // printk(KERN_INFO "can't register device with kernel \n");
	  // MSG("can't register device with kernel\n");
		return res;
	}
	
	/* register proc entry */
	schar_root_header = register_sysctl_table(schar_root_dir, 0);
	// schar_root_dir->child->de->fill_inode = &schar_fill_inode;
	
        init_waitqueue_head(&schar_wq);
        init_waitqueue_head(&schar_poll_read);


	return 0;
}

void cleanup_module(void)
{
	/* unregister device and proc entry */
  cleanup_exit2();
	unregister_chrdev(SCHAR_MAJOR, "schar");
	if (schar_root_header)
		unregister_sysctl_table(schar_root_header);
	
	return;
}



static ssize_t schar_write(struct file *file, const char *buf, size_t count,
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
  dev=dev_get_by_name("eth2");
  err=-ENODEV;
  if (dev == NULL)
    {printk(KERN_INFO "dev is null \n"); goto out_unlock;}
            
/*
 *      You may not queue a frame bigger than the mtu. This is the lowest level
 *      raw protocol and you must do your own fragmentation at this level.
*/
                
  err = -EMSGSIZE;
  if(len>dev->mtu+dev->hard_header_len)
    {printk(KERN_INFO "len to large \n");goto out_unlock;}
     
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
   proc_tpackets=proc_tpackets+1;
   proc_tbytesL=proc_tbytesL+len;
   if(proc_tbytesL>1000000000){
      proc_tbytesL=proc_tbytesL-1000000000;
      proc_tbytesH=proc_tbytesH+1;
   } 
   return count;
     
   out_free:
   kfree_skb(skb);
   out_unlock:
     // if (dev)dev_put(dev);
      // kfree(sbuf);           
  return -EFAULT;
}

