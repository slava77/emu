


/*    S. Durkin
 *	a simple character/memory map device driver (schar)
 *    intercepting ethernet packets through netif_rx
 */
#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/config.h>

#include <linux/module.h>

#if defined(CONFIG_SMP)
#define __SMP__
#endif


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
#include <linux/mm.h>

#include <linux/netdevice.h>   // struct device and dev_xxx()
#include <linux/etherdevice.h> // eth_xxx()

#include <asm/io.h>
#include <asm/param.h>
#include <linux/bigphysarea.h>

#include "schar.h"

#define SCHAR_MAJOR_2 232

/* settable parameters */
static char *schar_name = NULL;



/* forward declarations for _fops */
int cleanup_exit2_2(void);
void cleanup_module(void);
int netif_rx_hook_2(struct sk_buff *skb);
int init_module2_2(void);
int init_module(void);
static int schar_mmap_2(struct file *filp, struct vm_area_struct *vma);

static ssize_t schar_write_2(struct file *file, const char *buf, size_t count,loff_t *offset);
static int schar_ioctl_2(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static int schar_open_2(struct inode *inode, struct file *file);
static int schar_release_2(struct inode *inode, struct file *file);
void schar_nail_pages (void *kernel_address, int size);
void schar_unnail_pages (void *kernel_address, int size); 
void schar_reset_2();

static struct file_operations schar_fops = {
      write: schar_write_2,
      mmap:  schar_mmap_2,
      ioctl: schar_ioctl_2,
      open: schar_open_2,
      release: schar_release_2,
};




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

/*
moved definitions to eth_hook_2.h
#define BIGPHYS_PAGES_2 50000 
#define RING_PAGES_2 1000
#define RING_ENTRY_LENGTH 8
#define MAXPACKET_2 9100
#define MAXEVENT_2 30100
#define TAILMEM 100
#define TAILPOS  80
#define SKB_EXTRA 14
#define SKB_OFFSET -14
*/
#include "eth_hook_2.h"

static char *buf_start_2;
static char *buf_end_2;
static char *buf_eend_2;
static char *buf_pnt_2;

static char *ring_start_2;
static unsigned long int ring_size_2;
static unsigned long int ring_pnt_2;
static int short ring_loop_2;

static char *tail_start_2;

static unsigned long int proc_rpackets_2;
static unsigned long int proc_rbytesL_2;
static unsigned short int proc_rbytesH_2;
static unsigned long int proc_pmissing_2;
static unsigned short int proc_last_pmissing_2;
static unsigned long int proc_rfirst_2=0;
static unsigned long int proc_rsum_2;
static unsigned long int proc_rate_2=0;

static unsigned short int proc_tpackets_2;
static unsigned long int proc_tbytesL_2;
static unsigned short int proc_tbytesH_2;

static int flag_pmissing_2={1};   // check for missing packets
static int flag_eevent_2={1};     // check for end of events
static int flag_rate_2={1};       // turn on rate monitoring

static spinlock_t eth_lock;

/* module parameters and descriptions */
MODULE_PARM(schar_name, "s");
MODULE_PARM_DESC(schar_name, "Name of device");


MODULE_DESCRIPTION("schar 2, Sample character with ethernet hook");
MODULE_AUTHOR("S. Durkin");
MODULE_LICENSE("GPL");
int init_module2_2(void)
{ 
  buf_start_2=bigphysarea_alloc_pages(BIGPHYS_PAGES_2,0,GFP_KERNEL);
  if (!buf_start_2){
        printk(KERN_INFO " eth_hook_2 asked for too much memory!\n");
        printk(KERN_INFO " decrease BIGPHYS_PAGES_2 !\n");
  } 
  buf_end_2=buf_start_2+(BIGPHYS_PAGES_2-RING_PAGES_2)*PAGE_SIZE-MAXPACKET_2;
  buf_eend_2=buf_start_2+(BIGPHYS_PAGES_2-RING_PAGES_2)*PAGE_SIZE-TAILPOS-MAXEVENT_2;
  ring_start_2=buf_start_2+(BIGPHYS_PAGES_2-RING_PAGES_2)*PAGE_SIZE;
  ring_size_2=(RING_PAGES_2*PAGE_SIZE-RING_ENTRY_LENGTH-TAILMEM)/RING_ENTRY_LENGTH;
  tail_start_2=buf_start_2+BIGPHYS_PAGES_2*PAGE_SIZE-TAILPOS;
  schar_reset_2();
  return 0;
}


int cleanup_exit2_2(void)
{
  bigphysarea_free_pages(buf_start_2);
  return 0;
}

int netif_rx_hook_2(struct sk_buff *skb)
{
int i,icnt;
unsigned short int packet_count;
unsigned short int missing_mask;
unsigned short int eevent_mask;
unsigned short int end1,end2,end3,end4;
unsigned long flags;

  spin_lock_irqsave(&eth_lock,flags);
  
// write data to buffer memory
  icnt=(skb->len+SKB_EXTRA)>>2;
  for(i=0;i<icnt;i++){
    *(unsigned long int *)(buf_pnt_2+i*4)=*(unsigned long int *)(skb->data+i*4+SKB_OFFSET);
  }

// calculate count and bytes for proc 
   proc_rpackets_2=proc_rpackets_2+1;  
   proc_rbytesL_2=proc_rbytesL_2+skb->len+SKB_EXTRA;
   if(proc_rbytesL_2>1000000000){
      proc_rbytesL_2=proc_rbytesL_2-1000000000;
      proc_rbytesH_2=proc_rbytesH_2+1;
   }
// calculate rate for proc
   if(flag_rate_2==1){
      proc_rsum_2=proc_rsum_2+skb->len+SKB_EXTRA;
      if(jiffies%HZ!=0)proc_rfirst_2=0;
      if(jiffies%HZ==0&&proc_rfirst_2==0){
	proc_rate_2=(proc_rsum_2<<3);
        proc_rsum_2=0;
        proc_rfirst_2=1;
      }
   }



// check packet counter for missing packets from DDU
   missing_mask=0x0000;
   if(flag_pmissing_2==1){
   packet_count=*(unsigned short int *)(skb->data+skb->len-2);
   if(proc_last_pmissing_2+1!=packet_count){
     if(ring_pnt_2!=0||ring_loop_2!=0){
        if(proc_last_pmissing_2==0xffff){
           if(packet_count!=0){proc_pmissing_2=proc_pmissing_2+1;missing_mask=0x8000;}
        }else{
           proc_pmissing_2=proc_pmissing_2+1;missing_mask=0x8000;
        }
     }
   }
   proc_last_pmissing_2=packet_count;
   }

// check for end of event 0x8000 0xffff 0x8000 0x8000
   eevent_mask=0x0000;
   if(flag_eevent_2==1){
         end1=*(unsigned short int *)(skb->data+skb->len-26);
         end2=*(unsigned short int *)(skb->data+skb->len-24);
         end3=*(unsigned short int *)(skb->data+skb->len-22);
         end4=*(unsigned short int *)(skb->data+skb->len-20);
         if((end1==0x8000)&&(end2==0x8000)&&(end3==0xffff)&&(end4==0x8000))eevent_mask=0x4000;
          
         end1=*(unsigned short int *)(skb->data+skb->len-14);
         end2=*(unsigned short int *)(skb->data+skb->len-6);
         if(((end1&0xef00)==0xef00)&&((end1&0xaf00)==0xaf00))eevent_mask=0x4000;
	 end1=*(unsigned short int *)(skb->data+skb->len-10);
         end2=*(unsigned short int *)(skb->data+skb->len-6);
         if(((end1&0xf000)==0xf000)&&((end2&0xf000)==0xe000))eevent_mask=0x4000;
   }  

// write data to ring buffer
     *(unsigned short int *)(ring_start_2+ring_pnt_2*RING_ENTRY_LENGTH)=ring_loop_2|(missing_mask|eevent_mask);
          *(unsigned short int *)(ring_start_2+ring_pnt_2*RING_ENTRY_LENGTH+4)=skb->len+SKB_EXTRA-2;

// increment ring and buf pointers
   ring_pnt_2=ring_pnt_2+1;  
   buf_pnt_2=buf_pnt_2+skb->len+SKB_EXTRA-2; 

 //update ring and buf pointers
 // if((buf_pnt_2 > buf_end_2)||(ring_pnt_2>=ring_size_2)){
   if(((eevent_mask==0x4000)&&(buf_pnt_2 > buf_eend_2))||(buf_pnt_2 > buf_end_2)||(ring_pnt_2>=ring_size_2)){
     ring_pnt_2=0;
     ring_loop_2=ring_loop_2+1;
     buf_pnt_2=buf_start_2;
  }

// write data to tail word
  *(unsigned long int *)tail_start_2=buf_pnt_2-buf_start_2;

   kfree_skb(skb);
   spin_unlock_irqrestore(&eth_lock,flags);
return 1;
}





/*

int netif_rx_hook_2(struct sk_buff *skb)
{
  kfree_skb(skb);
  return 1;
}

*/


static int schar_ioctl_2(struct inode *inode, struct file *file,
		       unsigned int cmd, unsigned long arg)
{unsigned long int pagsiz;

	/* make sure that the command is really one of schar's */
	if (_IOC_TYPE(cmd) != SCHAR_IOCTL_BASE)
		return -ENOTTY;
		
	switch (cmd) {

		case SCHAR_RESET: {
		//  printk(KERN_INFO "ioct:l SCHAR_RESET \n");
		  schar_reset_2();
		return 0;
		}

	      case SCHAR_PAGES:{
                  pagsiz=BIGPHYS_PAGES_2;
                  copy_to_user ((void *)arg,&pagsiz,8);
		      return 0;
            }

	      case SCHAR_RING:{
                  pagsiz=RING_PAGES_2;
                  copy_to_user ((void *)arg,&pagsiz,8);
		      return 0;
            }

	      case SCHAR_PMISSING:{
		  if(flag_pmissing_2==0)flag_pmissing_2=1;
		  if(flag_pmissing_2>0)flag_pmissing_2=0;
		      return 0;
            }

	      case SCHAR_EEVENT:{
		  if(flag_eevent_2==0)flag_eevent_2=1;
		  if(flag_eevent_2>0)flag_eevent_2=0;
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

static int schar_read_proc_2(ctl_table *ctl, int write, struct file *file,
			   void *buffer, size_t *lenp)
{
	int len = 0;

	/* someone is writing data to us */
	if (write) {
		char *tmp = (char *) get_free_page(GFP_KERNEL);
		//	MSG("proc: someone wrote %u bytes\n", (unsigned)*lenp);
		if (tmp) {
			free_page((unsigned long)tmp);
			file->f_pos += *lenp;
		}
		return 0;
	}
	
	len += sprintf(schar_proc_string_2, "GIGABIT DRIVER ETH2: mm \n\n");
	len += sprintf(schar_proc_string_2+len, " RECEIVE: \n");
	len += sprintf(schar_proc_string_2+len, "  recieve\t\t%ld packets\n",proc_rpackets_2);
      len += sprintf(schar_proc_string_2+len, "  receive    \t\t\t%04d%09ld bytes\n",proc_rbytesH_2,proc_rbytesL_2); 
      len += sprintf(schar_proc_string_2+len, "  memory  \t\t\t%09ld bytes\n",(BIGPHYS_PAGES_2*PAGE_SIZE));
      len += sprintf(schar_proc_string_2+len, "  rate  \t\t\t%6ld KBits/s \n",proc_rate_2/1000);
      len += sprintf(schar_proc_string_2+len, "  loop %d count %ld\n\n",ring_loop_2,ring_pnt_2);
	len += sprintf(schar_proc_string_2+len, " TRANSMIT: \n");
      len += sprintf(schar_proc_string_2+len, "  transmit\t\t%d packets \n",proc_tpackets_2);
      len += sprintf(schar_proc_string_2+len, "  transmit    \t\t\t%d%08ld bytes\n\n",proc_tbytesH_2,proc_tbytesL_2); 
  	len += sprintf(schar_proc_string_2+len, " ERROR STATISTICS: \n");
      len += sprintf(schar_proc_string_2+len, "  missing packets \t\t%ld\n",proc_pmissing_2);
	*lenp = len;
	return proc_dostring(ctl, write, file, buffer, lenp);
}



static int schar_open_2(struct inode *inode, struct file *file)
{
	/* increment usage count */
	MOD_INC_USE_COUNT;
        schar_nail_pages(buf_start_2,BIGPHYS_PAGES_2*PAGE_SIZE);

	return 0;
}
static int schar_release_2(struct inode *inode, struct file *file)
{
	MOD_DEC_USE_COUNT;
        schar_unnail_pages(buf_start_2,BIGPHYS_PAGES_2*PAGE_SIZE);
	return 0;
}


int init_module(void)
{
	int res;
        init_module2_2();
        spin_lock_init(&eth_lock);

	  schar_name = "schar2";
		
	
	/* register device with kernel */
	res = register_chrdev(SCHAR_MAJOR_2, schar_name, &schar_fops);
       
	if (res) {
	      printk(KERN_INFO "can't register device with kernel\n");
		return res;
	}
	
	/* register proc entry */
	schar_root_header_2 = register_sysctl_table(schar_root_dir_2, 0);

	return 0;
}

void cleanup_module(void)
{
	/* unregister device and proc entry */
 
	unregister_chrdev(SCHAR_MAJOR_2, "schar2");
	if (schar_root_header_2)
		unregister_sysctl_table(schar_root_header_2);
	 cleanup_exit2_2();
 
	return;
}


static ssize_t schar_write_2(struct file *file, const char *buf, size_t count,
			   loff_t *offset)
{
  /* structured after af_packet.c by S. Durkin */
  
  int len;
  int err;

  static struct net_device *dev;
  static struct sk_buff *skb;
  static unsigned short proto=0;
  // printk(KERN_INFO " length %d \n",count);

  // sbuf=kmalloc(9000,GFP_KERNEL);
  len=count;
  dev=dev_get_by_name("eth2");
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
  
   proc_tpackets_2=proc_tpackets_2+1;
   proc_tbytesL_2=proc_tbytesL_2+len;
   if(proc_tbytesL_2>1000000000){
      proc_tbytesL_2=proc_tbytesL_2-1000000000;
      proc_tbytesH_2=proc_tbytesH_2+1;
      //      printk(KERN_INFO "tbytesH %d \n",proc_tbytesH_2);
      //      printk(KERN_INFO "tbytesH %d \n",proc_tbytesH_2);
      //      printk(KERN_INFO "tbytesH %d \n",proc_tbytesH_2);
      //      printk(KERN_INFO "tbytesH %d \n",proc_tbytesH_2);
   }

   return count;
     
   out_free:
     kfree_skb(skb);
   out_unlock:
     if (dev)dev_put(dev);
      // kfree(sbuf);           
  return -err;
}


static int schar_mmap_2(struct file *filp, struct vm_area_struct *vma)
{
        unsigned long page,pos; 
	unsigned long size = (unsigned long)(vma->vm_end-vma->vm_start);
	unsigned long start = (unsigned long)vma->vm_start;  

	printk(KERN_INFO "enter mmap \n");
	/* start off at the start of the buffer */ 
        pos=(unsigned long)buf_start_2 ;
 
	/* loop through all the physical pages in the buffer */ 
	/* Remember this won't work for vmalloc()d memory ! */
        while (size > 0) {
		/* remap a single physical page to the process's vma */ 
                page = virt_to_phys((void *)pos);
		/* fourth argument is the protection of the map. you might
		 * want to use vma->vm_page_prot instead.
		 */
                if (remap_page_range(vma,start, page, PAGE_SIZE, PAGE_SHARED))
                                  return -EAGAIN;
                start+=PAGE_SIZE;
                pos+=PAGE_SIZE;
                size-=PAGE_SIZE;
        }
        return 0; 
}


/** kfedkit_nail_pages */
void schar_nail_pages (void *kernel_address, int size) 
{
    /* we will nail all the pages */
    struct page * page;
    printk(KERN_INFO "nail \n");
    for (page = virt_to_page(kernel_address); 
         page <= virt_to_page(kernel_address + size -1); 
         page++){mem_map_reserve(page);} 
}

/** kfedkit_unnail_pages */
void schar_unnail_pages (void *kernel_address, int size) 
{
    /* we will nail all the pages */
    struct page * page;
    printk(KERN_INFO "unnail \n");
    for (page = virt_to_page(kernel_address); 
         page <= virt_to_page(kernel_address + size -1); 
         page++){mem_map_unreserve(page);}
}

void schar_reset_2()
{ 

buf_pnt_2=buf_start_2;

ring_pnt_2=0;
ring_loop_2=0;

proc_rpackets_2=0;
proc_rbytesL_2=0;
proc_rbytesH_2=0;
proc_pmissing_2=0;
proc_last_pmissing_2=0;

proc_tpackets_2=0;
proc_tbytesL_2=0;
proc_tbytesH_2=0;

*(unsigned long int *)tail_start_2=0;

}

