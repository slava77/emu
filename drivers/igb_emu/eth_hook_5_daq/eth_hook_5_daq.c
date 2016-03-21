/* #define DEBUG_PRINT */

/*    S. Durkin
 *	a simple character/memory map device driver (schar)
*    intercepting ethernet packets through netif_rx
 */

/* memory management taken from the linux video camera firewire interface
                                                                                
/usr/src/linux/drivers/ieee1394/video1394.c
  
There is a fundamental limit to the total amount of memory allocation
all kernel modules/drivers can use. It is presently set to 128 Megabytes.

see:
                                                                                
/usr/src/linux-2.4/include/asm-i386/page.h
                                                                                
#define __VMALLOC_RESERVE       (128 << 20)
                                                                                
                                                                              
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
//#define __SMP__
#endif


#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/timer.h>	
#include <linux/fs.h>		
#include <linux/poll.h>		
#include <net/irda/wrapper.h>	
#include <linux/proc_fs.h>
#include <linux/sysctl.h>
#include <linux/init.h>

#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/smp_lock.h>
#include <linux/vmalloc.h>

#include <linux/netdevice.h>   
#include <linux/etherdevice.h> 

#include <asm/io.h>
#include <asm/param.h>
#include <asm/byteorder.h>
#include <asm/atomic.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include <asm/pgtable.h>
#include <asm/page.h>
#include <asm/segment.h>
#include <net/irda//wrapper.h>

#include "SLC_version.h"
#include "schar.h"
//#define VMALLOC_VMADDR(x) ((unsigned long)(x))


#define SCHAR_MAJOR_5 235

/* settable parameters */
static char *schar_name = NULL;

/* See /lib/modules/$(uname -r)/build/include/linux/skbuff.h for the definition of struct sk_buff */

/* forward declarations for _fops */
int cleanup_exit2_5(void);
void ethcleanup_module(void);
int netif_rx_hook_5(struct sk_buff *skb);
int init_module2_5(void);
int ethinit_module(void);
static int schar_mmap_5(struct file *filp, struct vm_area_struct *vma);

static ssize_t schar_write_5(struct file *file, const char *buf, size_t count,loff_t *offset);
static int schar_ioctl_5(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static int schar_open_5(struct inode *inode, struct file *file);
static int schar_release_5(struct inode *inode, struct file *file); 
void schar_reset_5(void);
static void rvnail(int *mem,unsigned long size);
static void rvunnail(int *mem,unsigned long size);
//static int do_rv_mmap(struct vm_area_struct *vma,const char *adr, unsigned long size);

EXPORT_SYMBOL(netif_rx_hook_5);

static struct file_operations schar_fops = {
      write: schar_write_5,
      mmap:  schar_mmap_5,
      ioctl: schar_ioctl_5,
      open: schar_open_5,
      release: schar_release_5,
};




/* sysctl entries */
static char schar_proc_string_5[SCHAR_MAX_SYSCTL];
static struct ctl_table_header *schar_root_header_5 = NULL;
#if   SLC_VERSION == 5
static int schar_read_proc_5(ctl_table *ctl, int write, struct file *file,
			   void *buffer, size_t *lenp, loff_t *ppos);
#elif SLC_VERSION == 6
static int schar_read_proc_5(struct ctl_table *ctl, int write,
			     void __user *buffer, size_t *lenp, loff_t *ppos);
#endif

static ctl_table schar_sysctl_table_5[] = {
	{ DEV_SCHAR_ENTRY,	/* binary id */
	  "5",			/* name */
	  &schar_proc_string_5,	/* data */
	  SCHAR_MAX_SYSCTL,	/* max size of output */
	  0644,			/* mode */
	  0,			/* child - none */
#if SLC_VERSION == 6
	  0,			/* parent */
#endif
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


#include "eth_hook_5_daq.h"

int i;
static int *vmalloc_area; 
static long int LEN;
static char *buf_start_5;
static char *buf_end_5;
static char *buf_eend_5;
static char *buf_pnt_5;

static char *ring_start_5;
static unsigned long int ring_size_5;
static unsigned long int ring_pnt_5;
static int short ring_loop_5;

static char *tail_start_5;

static unsigned long int proc_rpackets_5;
static unsigned long int proc_rbytesL_5;
static unsigned short int proc_rbytesH_5;
static unsigned long int proc_pmissing_5;
static unsigned short int proc_last_pmissing_5;
static unsigned long int proc_rfirst_5=0;
static unsigned long int proc_rsum_5;
static unsigned long int proc_rate_5=0;
static unsigned long int proc_previousjiffies_5;

static unsigned short int proc_tpackets_5;
static unsigned long int proc_tbytesL_5;
static unsigned short int proc_tbytesH_5;

static int flag_pmissing_5={1};   // check for missing packets
static int flag_eevent_5={1};     // check for end of events
static int flag_rate_5={1};       // turn on rate monitoring

static spinlock_t eth_lock;

/* module parameters and descriptions */
//MODULE_PARM(schar_name);
//MODULE_PARM_DESC(schar_name, "Name of device");
MODULE_DESCRIPTION("schar 5, Sample character with ethernet hook");
MODULE_AUTHOR("S. Durkin <durkin@mps.ohio-state.edu>");
MODULE_LICENSE("Dual BSD/GPL");
int init_module2_5(void)
{ 
  printk(KERN_INFO "LSD: enter init_module2 \n");
  LEN=BIGPHYS_PAGES_5*PAGE_SIZE;
  printk(KERN_INFO "LSD: BIGPHYS_PAGES %d PAGE_SIZE %ld \n",BIGPHYS_PAGES_5,PAGE_SIZE);
  printk(KERN_INFO "LSD: LEN %ld \n",LEN);
  if((vmalloc_area=(int *)vmalloc_32(LEN))==NULL){
    printk(KERN_INFO " Can't allocate memory \n");
    return -ENOMEM;
  }
  printk(KERN_INFO " vmalloc allocated \n");
  rvnail(vmalloc_area,LEN);
  buf_start_5=(char *)vmalloc_area;
    if (!buf_start_5){
        printk(KERN_INFO " eth_hook_5 asked for too much memory!\n");
        printk(KERN_INFO " decrease BIGPHYS_PAGES_5 !\n");
  } 
  printk(KERN_INFO "LSD: buf_start_5 is allocated \n");
  buf_end_5=buf_start_5+(BIGPHYS_PAGES_5-RING_PAGES_5)*PAGE_SIZE-MAXPACKET_5;
  buf_eend_5=buf_start_5+(BIGPHYS_PAGES_5-RING_PAGES_5)*PAGE_SIZE-TAILPOS-MAXEVENT_5;
  ring_start_5=buf_start_5+(BIGPHYS_PAGES_5-RING_PAGES_5)*PAGE_SIZE;
  ring_size_5=(RING_PAGES_5*PAGE_SIZE-RING_ENTRY_LENGTH-TAILMEM)/RING_ENTRY_LENGTH;
  tail_start_5=buf_start_5+BIGPHYS_PAGES_5*PAGE_SIZE-TAILPOS;
  schar_reset_5();
  return 0;
}


int cleanup_exit2_5(void)
{
  rvunnail(vmalloc_area,LEN);
   vfree(vmalloc_area);
  return 0;
}

int netif_rx_hook_5(struct sk_buff *skb)
{
int i,icnt;
unsigned short int packet_count=0;
// unsigned short int gucount;
unsigned short int missing_mask;
unsigned short int eevent_mask;
unsigned short int end1,end2,end3,end4;
unsigned long flags;

long int delta_jiffies;

  spin_lock_irqsave(&eth_lock,flags);
  
// write data to buffer memory
  icnt=(skb->len+SKB_EXTRA)>>3; // Divide by 2^3 as sizeof( unsigned long int ) = 8 on 64-bit systems
  if ( skb->len+SKB_EXTRA > icnt*8 ) icnt++;
#ifdef DEBUG_PRINT
  printk(KERN_INFO "-------------------------------------------------------------\n");
  printk(KERN_INFO "ETH5 icnt %d len %d \n",icnt,skb->len);
#endif
  for(i=0;i<icnt;i++){
    *(unsigned long int *)(buf_pnt_5+i*8)=*(unsigned long int *)(skb->data+i*8+SKB_OFFSET);
  }

/* // write data to buffer memory */
/*   icnt=(skb->len+SKB_EXTRA)>>2; */
/* #ifdef DEBUG_PRINT */
/*   printk(KERN_INFO "ETH5 icnt %d len %d \n",icnt,skb->len); */
/* #endif */
/*   for(i=0;i<icnt;i++){ */
/*     *(unsigned int *)(buf_pnt_5+i*4)=*(unsigned int *)(skb->data+i*4+SKB_OFFSET); */
/*   } */

// calculate count and bytes for proc
   proc_rpackets_5=proc_rpackets_5+1;
   proc_rbytesL_5=proc_rbytesL_5+skb->len+SKB_EXTRA;
   if(proc_rbytesL_5>1000000000){
      proc_rbytesL_5=proc_rbytesL_5-1000000000;
      proc_rbytesH_5=proc_rbytesH_5+1;
   }
// calculate rate for proc
   if(flag_rate_5==1){
     proc_rsum_5 += skb->len+SKB_EXTRA;
     if ( proc_rfirst_5==0 ){
       proc_previousjiffies_5 = get_jiffies_64();
       proc_rfirst_5=1;
     }
     else{
       delta_jiffies = get_jiffies_64() - proc_previousjiffies_5;
       if ( delta_jiffies > HZ ){ // Update ~every second
   	 proc_rate_5 = ( HZ * proc_rsum_5<<3 ) / delta_jiffies; // bit/s (proc_rsum_5 counts bytes; HZ=jiffies/s)
   	 proc_previousjiffies_5 = get_jiffies_64();
   	 proc_rsum_5 = 0;
       }
     }
   }

// check packet counter for missing packets from DDU
   missing_mask=0x0000;
   if(flag_pmissing_5==1){
   packet_count=*(unsigned short int *)(skb->data+skb->len+SKB_EXTRA+SKB_OFFSET);

   /* packet_count=*(unsigned short int *)(skb->data+skb->len+12+SKB_OFFSET); */
   /*   for (igu=-10;igu<10;igu+=2) {
     gucount=*(unsigned short int *)(skb->data+skb->len+8+SKB_OFFSET+igu);
     printk(KERN_INFO " packet count %d  %04x \n",igu,gucount );
   }
   */
   if(proc_last_pmissing_5+1!=packet_count){
     if(ring_pnt_5!=0||ring_loop_5!=0){
        if(proc_last_pmissing_5==0xffff){
           if(packet_count!=0){proc_pmissing_5=proc_pmissing_5+1;missing_mask=0x8000;}
        }else{
           proc_pmissing_5=proc_pmissing_5+1;missing_mask=0x8000;
        }
     }
   }
   proc_last_pmissing_5=packet_count;
   }

#ifdef DEBUG_PRINT
   // DEBUG START
   printk(KERN_INFO "ETH5 Length %d, packet_count %d, proc_last_pmissing %d, proc_pmissing %ld, missing_mask 0x%04x\n",skb->len,packet_count,proc_last_pmissing_5,proc_pmissing_5,missing_mask);
   printk(KERN_INFO "ETH5  ");
   for(i=90;i>=-50;i-=2)
     printk("%+4d ",-i);
   printk("\n");
   printk(KERN_INFO "ETH5  ");
   for(i=90;i>=-50;i-=2)
     printk("%04x ",*(unsigned short int*)(skb->data+skb->len-i));
   printk("\n");
   for(i=-32;i<(int)(skb->len);i+=2){
     if ( i%16 == 0 ) printk("%04d   ",i);
     printk("%04x ",*(unsigned short int*)(skb->data+i));
     if ( (i+2)%16 == 0 ) printk("\n");
   }
   printk("\n");
   // DEBUG END
#endif

   eevent_mask=0x0000;
   if(flag_eevent_5==1){
     // See if this packet contains the end of event.
     if ( skb->len+SKB_EXTRA <= 64 ){
       // This short a packet can contain either an entire (empty?) event or the rump of a long (and therefore split) event.
       // In either case, it _must_ contain the end of an event, no need to check for it.
       // (It may even be split in such a way that it doesn't contain a complete trailer.
       // And it may be padded with 0xff to meet the minimum Ethernet packet length requirement, 
       // in which case the trailer is not at the end of the packet.)
       // So we blindly declare this packet to be the end of event. 
       eevent_mask=0x4000;
#ifdef DEBUG_PRINT
       printk(KERN_INFO "ETH5  %d-byte packet ends event\n",skb->len+SKB_EXTRA);
#endif
     } // if ( skb->len+SKB_EXTRA <= 64 )
     else{
       // Packets of this size should certainly not contain padding.
       // We can therefore assume that the trailer, if any, is at the end of the packet.
#ifdef _DDU_SPY_
       // Check for end of event 0x8000 0xffff 0x8000 0x8000 (unique word in DDU trailer)
       unsigned char *end_of_packet = skb->data + SKB_OFFSET + skb->len + SKB_EXTRA;
       const int DDU_trailer_length = 24; // bytes
       // If present, it must be at the end of packet.
       end1=*(unsigned short int *)(end_of_packet-DDU_trailer_length+0);
       end2=*(unsigned short int *)(end_of_packet-DDU_trailer_length+2);
       end3=*(unsigned short int *)(end_of_packet-DDU_trailer_length+4);
       end4=*(unsigned short int *)(end_of_packet-DDU_trailer_length+6);
#ifdef DEBUG_PRINT
       printk(KERN_INFO "ETH5  DDU trailer?  %04x %04x %04x %04x\n",end1,end2,end3,end4);
#endif
       if((end1==0x8000)&&(end2==0x8000)&&(end3==0xffff)&&(end4==0x8000)){
	 // Caught a DDU trailer
	 eevent_mask=0x4000;
#ifdef DEBUG_PRINT
	 printk(KERN_INFO "ETH5  DDU EoE\n");
#endif
       }
#endif
#ifdef _DMB_SPY_
       // Check for 0xf___ 0xf___ 0xf___ 0xf___ 0xe___ 0xe___ 0xe___ 0xe___ (DMB trailer)
       unsigned char *end_of_packet = skb->data + SKB_OFFSET + skb->len + SKB_EXTRA;
       const int DMB_trailer_length=16; // bytes
       unsigned short int end5,end6,end7,end8;
       end1=*(unsigned short int *)(end_of_packet-DMB_trailer_length+ 0); 
       end2=*(unsigned short int *)(end_of_packet-DMB_trailer_length+ 2); 
       end3=*(unsigned short int *)(end_of_packet-DMB_trailer_length+ 4); 
       end4=*(unsigned short int *)(end_of_packet-DMB_trailer_length+ 6); 
       end5=*(unsigned short int *)(end_of_packet-DMB_trailer_length+ 8); 
       end6=*(unsigned short int *)(end_of_packet-DMB_trailer_length+10); 
       end7=*(unsigned short int *)(end_of_packet-DMB_trailer_length+12); 
       end8=*(unsigned short int *)(end_of_packet-DMB_trailer_length+14); 
#ifdef DEBUG_PRINT
       printk(KERN_INFO "ETH5  DMB trailer?\n  %04x %04x %04x %04x\n  %04x %04x %04x %04x\n",end1,end2,end3,end4,end5,end6,end7,end8);
#endif
       if( ((end1&0xf000)==0xf000) &&
	   ((end2&0xf000)==0xf000) &&
	   ((end3&0xf000)==0xf000) &&
	   ((end4&0xf000)==0xf000) &&
	   ((end5&0xf000)==0xe000) &&
	   ((end6&0xf000)==0xe000) &&
	   ((end7&0xf000)==0xe000) &&
	   ((end8&0xf000)==0xe000)    ){
	 // Caught a DMB trailer
	 eevent_mask=0x4000;
#ifdef DEBUG_PRINT
	 printk(KERN_INFO "ETH5  DMB EoE\n");
#endif
       }
#endif
     } // if ( skb->len+SKB_EXTRA <= 64 ) else
   } // if(flag_eevent_5==1)

// write data to ring buffer
   *(unsigned short int *)(ring_start_5+ring_pnt_5*RING_ENTRY_LENGTH)=ring_loop_5|(missing_mask|eevent_mask);
   *(unsigned short int *)(ring_start_5+ring_pnt_5*RING_ENTRY_LENGTH+4)=skb->len+SKB_EXTRA;

// increment ring and buf pointers
   ring_pnt_5=ring_pnt_5+1;
   buf_pnt_5=buf_pnt_5+skb->len+SKB_EXTRA;

 //update ring and buf pointers
 // if((buf_pnt_5 > buf_end_5)||(ring_pnt_5>=ring_size_5)){
   if(((eevent_mask==0x4000)&&(buf_pnt_5 > buf_eend_5))||(buf_pnt_5 > buf_end_5)||(ring_pnt_5>=ring_size_5)){
     ring_pnt_5=0;
     ring_loop_5=ring_loop_5+1;
     buf_pnt_5=buf_start_5;
  }

// write data to tail word
  *(unsigned long int *)tail_start_5=buf_pnt_5-buf_start_5;

   dev_kfree_skb_any(skb);
   spin_unlock_irqrestore(&eth_lock,flags);
return 1;
}





/*

int netif_rx_hook_5(struct sk_buff *skb)
{
  kfree_skb(skb);
  return 1;
}

*/


static int schar_ioctl_5(struct inode *inode, struct file *file,
		       unsigned int cmd, unsigned long arg)
{
unsigned long int pagsiz;
 printk(KERN_INFO " ioctl: Entered \n");
	/* make sure that the command is really one of schar's */
	if (_IOC_TYPE(cmd) != SCHAR_IOCTL_BASE)
		return -ENOTTY;
	// printk(KERN_INFO " cmd %d \n",cmd);		
	switch (cmd) {

	case SCHAR_RESET: {
	  //  printk(KERN_INFO "ioct:l SCHAR_RESET \n");
		  schar_reset_5();
		return 0;
		}

	case SCHAR_PAGES:{
                  pagsiz=BIGPHYS_PAGES_5;
                  copy_to_user ((void *)arg,&pagsiz,8);
		      return 0;
            }

	case SCHAR_RING:{
                  pagsiz=RING_PAGES_5;
                  copy_to_user ((void *)arg,&pagsiz,8);
		      return 0;
            }

	case SCHAR_PMISSING:{
		  if(flag_pmissing_5==0)flag_pmissing_5=1;
		  if(flag_pmissing_5>0)flag_pmissing_5=0;
		      return 0;
            }

	case SCHAR_EEVENT:{
		  if(flag_eevent_5==0)flag_eevent_5=1;
		  if(flag_eevent_5>0)flag_eevent_5=0;
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

#if   SLC_VERSION == 5
static int schar_read_proc_5(ctl_table *ctl, int write, struct file *file,
			   void *buffer, size_t *lenp, loff_t *ppos)
#elif SLC_VERSION == 6
static int schar_read_proc_5(struct ctl_table *ctl, int write,
			     void __user *buffer, size_t *lenp, loff_t *ppos)
#endif
{
	int len = 0;

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
	len += sprintf(schar_proc_string_5, "GIGABIT DRIVER ETH5: mm \n\n");
	len += sprintf(schar_proc_string_5+len, " RECEIVE: \n");
	len += sprintf(schar_proc_string_5+len, "  recieve\t\t%ld packets\n",proc_rpackets_5);
      len += sprintf(schar_proc_string_5+len, "  receive    \t\t\t%04d%09ld bytes\n",proc_rbytesH_5,proc_rbytesL_5); 
      len += sprintf(schar_proc_string_5+len, "  memory  \t\t\t%09ld bytes\n",(BIGPHYS_PAGES_5*PAGE_SIZE));
      len += sprintf(schar_proc_string_5+len, "  rate  \t\t\t%6ld KiB/s (%10ld bit/s)\n",proc_rate_5>>13,proc_rate_5);
      len += sprintf(schar_proc_string_5+len, "  loop %d count %ld\n\n",ring_loop_5,ring_pnt_5);
	len += sprintf(schar_proc_string_5+len, " TRANSMIT: \n");
      len += sprintf(schar_proc_string_5+len, "  transmit\t\t%d packets \n",proc_tpackets_5);
      len += sprintf(schar_proc_string_5+len, "  transmit    \t\t\t%d%08ld bytes\n\n",proc_tbytesH_5,proc_tbytesL_5); 
  	len += sprintf(schar_proc_string_5+len, " ERROR STATISTICS: \n");
      len += sprintf(schar_proc_string_5+len, "  missing packets \t\t%ld\n",proc_pmissing_5);
	*lenp = len;
#if   SLC_VERSION == 5
	return proc_dostring(ctl, write, file, buffer, lenp, ppos);
#elif SLC_VERSION == 6
	return proc_dostring(ctl, write, buffer, lenp, ppos);
#endif
}



static int schar_open_5(struct inode *inode, struct file *file)
{
	/* increment usage count */
	//v2.6remove MOD_INC_USE_COUNT;
       	return 0;
}
static int schar_release_5(struct inode *inode, struct file *file)
{
  //v2.6removeMOD_DEC_USE_COUNT;
	return 0;
}


int ethinit_module(void)
{
	int res;
        printk(KERN_INFO "init module called \n");
        init_module2_5();

        spin_lock_init(&eth_lock);

	  schar_name = "schar5";
		
	
	/* register device with kernel */
	res = register_chrdev(SCHAR_MAJOR_5, schar_name, &schar_fops);
       
	if (res) {
	      printk(KERN_INFO "can't register device with kernel\n");
		return res;
	}
	
	/* register proc entry */
#if   SLC_VERSION == 5
	schar_root_header_5 = register_sysctl_table(schar_root_dir_5, 0);
#elif SLC_VERSION == 6
	schar_root_header_5 = register_sysctl_table(schar_root_dir_5);
#endif

	return 0;
}

void ethcleanup_module(void)
{


	/* unregister device and proc entry */
 
	unregister_chrdev(SCHAR_MAJOR_5, "schar5");
	if (schar_root_header_5)
		unregister_sysctl_table(schar_root_header_5);
    
        cleanup_exit2_5();
	return;
}


static ssize_t schar_write_5(struct file *file, const char *buf, size_t count,
			   loff_t *offset)
{
  /* structured after af_packet.c by S. Durkin */
  
  int len;
  int err;
  
  static struct net_device *dev;
  static struct sk_buff *skb;
  static unsigned short proto=0;
  // printk(KERN_INFO " LSD: write length %d \n",count);

  // sbuf=kmalloc(9000,GFP_KERNEL);
  len=count;
#if   SLC_VERSION == 5
  dev=dev_get_by_name("eth5");
#elif SLC_VERSION == 6
  dev=dev_get_by_name(&init_net,"p2p2");
#endif
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
#if   SLC_VERSION == 5
   skb->nh.raw = skb->data;
#elif SLC_VERSION == 6
   skb_reset_network_header(skb);
#endif

   proto=htons(ETH_P_ALL);
   /*     	if (dev->hard_header) {
		int res;
		err = -EINVAL;
                addr=NULL;
		res = dev->hard_header(skb, dev, ntohs(proto), addr, NULL, len);
			skb->tail = skb->data;
			skb->len = 0;
			} */
        			
#if   SLC_VERSION == 5
                        skb->tail = skb->data;
#elif SLC_VERSION == 6
			skb_reset_tail_pointer(skb);
#endif

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
   // for(i=120;i<160;i++)printk(KERN_INFO " %02x",*(skb->data+i)&0xff);
   // printk(KERN_INFO "\n");
   // kfree(sbuf);
  
   proc_tpackets_5=proc_tpackets_5+1;
   proc_tbytesL_5=proc_tbytesL_5+len;
   if(proc_tbytesL_5>1000000000){
      proc_tbytesL_5=proc_tbytesL_5-1000000000;
      proc_tbytesH_5=proc_tbytesH_5+1;
      //      printk(KERN_INFO "tbytesH %d \n",proc_tbytesH_5);
      //      printk(KERN_INFO "tbytesH %d \n",proc_tbytesH_5);
      //      printk(KERN_INFO "tbytesH %d \n",proc_tbytesH_5);
      //      printk(KERN_INFO "tbytesH %d \n",proc_tbytesH_5);
   }

   return count;
     
   out_free:
     kfree_skb(skb);
   out_unlock:
     if (dev)dev_put(dev);
      // kfree(sbuf);           
  return -err;
}


static int schar_mmap_5(struct file *filp, struct vm_area_struct *vma)
{
  int ret;
  long length = vma->vm_end - vma->vm_start;
  unsigned long start = vma->vm_start;
  char *vmalloc_area_ptr = (char *)vmalloc_area;
  unsigned long pfn;
  lock_kernel();
  printk(KERN_INFO " entering mmap \n");
  if (!buf_start_5) {
    printk(KERN_INFO " Data was not initialized");
    unlock_kernel();
    return -EINVAL;
  }

  /* check length - do not allow larger mappings than the number of
       pages allocated */
  if (length > BIGPHYS_PAGES_5*PAGE_SIZE){
    printk(KERN_INFO " Page allocation problem %ld %ld \n",length,BIGPHYS_PAGES_5*PAGE_SIZE);
    unlock_kernel();      
     return -EIO;
  }

        /* loop over all pages, map it page individually */
  while (length > 0) {
           pfn = vmalloc_to_pfn(vmalloc_area_ptr);
           if ((ret = remap_pfn_range(vma, start, pfn, PAGE_SIZE,
                                      PAGE_SHARED)) < 0) {
	     printk(KERN_INFO "do_rvmmap BAD RETURN %ld %ld ret %d \n",length,BIGPHYS_PAGES_5*PAGE_SIZE,ret);
             printk(KERN_INFO " this can happen if MEM_SHARED is not used by calling process ! \n");
             unlock_kernel();
             return ret;
           }
           start += PAGE_SIZE;
           vmalloc_area_ptr += PAGE_SIZE;
           length -= PAGE_SIZE;
  }
  vma->vm_flags |= VM_RESERVED;   /* avoid to swap out this VMA */
  unlock_kernel();
  return 0;
}


void schar_reset_5()
{ 

buf_pnt_5=buf_start_5;

ring_pnt_5=0;
ring_loop_5=0;

proc_rpackets_5=0;
proc_rbytesL_5=0;
proc_rbytesH_5=0;
proc_pmissing_5=0;
proc_last_pmissing_5=0;

proc_tpackets_5=0;
proc_tbytesL_5=0;
proc_tbytesH_5=0;

*(unsigned long int *)tail_start_5=0;

}


/* LSD rework of mmap handiling for Kernel > 2.6.10 */

static void rvnail(int *mem,unsigned long size)
{
  int i;
  printk(KERN_INFO " before vrnail %ld \n",size);      
  if (mem) 
    {
      memset(mem, 0, size);  
      /* reserve the pages */			    
      for (i = 0; i < size; i+= PAGE_SIZE) {
	SetPageReserved(vmalloc_to_page((void *)(((unsigned long)mem) + i)));
      }
      printk(KERN_INFO " pages have been nailed %ld \n",size);
    } 
  return ;
}



static void rvunnail(int *mem, unsigned long size)
{
  int i;
        
  if (mem) 
    {
 /* unreserve the pages */
      for (i = 0; i < size; i+= PAGE_SIZE) {
	ClearPageReserved(vmalloc_to_page((void *)(((unsigned long)mem) + i)));
      }
      printk(" pages have been unnailed %ld \n",size);
    }
}





/* static int do_rv_mmap(struct vm_area_struct *vma,const char *adr, unsigned long size)
{
  int i;
  int ret;
  long length = vma->vm_end - vma->vm_start;
  unsigned long start = vma->vm_start;
  char *vmalloc_area_ptr = (char *)buf_start_5;
  unsigned long pfn;
  for(i=0;i<100;i++)printk(KERN_INFO " %d %02x \n",i,buf_start_5[i]);
  printk(KERN_INFO " mmap to user space called \n");
  if (!buf_start_5) {
    printk(KERN_INFO " Data was not initialized");
    return -EINVAL;
  }

  if (length > size){
    printk(KERN_INFO " Page allocation problem %ld %ld \n",length,size);      
     return -EIO;
  }

  if(length>0) {
           pfn = vmalloc_to_pfn(vmalloc_area_ptr);
           if ((ret = remap_pfn_range(vma, start, pfn, PAGE_SIZE,
                                      PAGE_SHARED)) < 0) {
	     printk(KERN_INFO "do_rvmmap BAD RETURN %ld %ld \n",length,size);
                   return ret;
           }
           start += PAGE_SIZE;
           vmalloc_area_ptr += PAGE_SIZE;
           length -= PAGE_SIZE;
  }
  printk(KERN_INFO " return 0 \n");
  return 0;
} */

 
module_init(ethinit_module);
module_exit(ethcleanup_module);

