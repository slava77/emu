#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/if_ether.h> 
#include <netinet/in.h> 
#include <netinet/ip.h> 
#include <net/if.h> 
#include <sys/ioctl.h> 
#include <netpacket/packet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>
#include "emu/odmbdev/schar.h"
#include "emu/odmbdev/eth_hook_2.h"
#include "emu/odmbdev/Spy.h"
#include "emu/odmbdev/page.h"
#ifndef __NETINET_IF_ETHER_H
#include <netinet/if_ether.h>
#endif

static int fd_schar;

unsigned char hw_dest_addr[6]={0x02,0x00,0x00,0x00,0x00,0xFF};
unsigned char hw_source_addr[6]={0x00,0x00,0x00,0x00,0x00,0x00};
struct ethhdr *ether_header[2];
int  eth_open(char *dev_name);
int  eth_register_mac();
void eth_close(void);
void eth_reset_close(void);
int  eth_reset(void);
int  eth_readmm();
int  eth_write();
void eth_enableBlock();
void eth_disableBlock();
void eth_resetAndEnable();

int nwbuf;   
char ebuf[10000];
char *wbuf=ebuf+ETH_HLEN;
int nrbuf;
char rbuf[10000];



int eth_open(char *dev_name)
{
  int i;
  char *dev;
  if((dev=(char *)malloc((6+strlen(dev_name))*sizeof(char)))==0){
    printf("Device name allocation error!\n");
    return -1;
  }
  sprintf(dev,"/dev/%s",dev_name);
  fd_schar = open(dev, O_RDWR);
  if (fd_schar == -1) {
    perror("open");
    return -1;
  }
  printf("Opened network path on device %s\n",dev);
  free(dev);

  // eth_reset(); 

  buf_start = (char *)mmap(NULL,BIGPHYS_PAGES_2*PAGE_SIZE,PROT_READ,MAP_SHARED,fd_schar,0);
  if (buf_start==MAP_FAILED) {
    printf("emu::daq::reader::Spy::open: FATAL in memorymap - ");
    printf("emu::daq::reader::Spy::open will abort!!!");
    abort();
  }
  printf("emu::daq::reader::Spy::open: Memory map succeeded ");

  buf_end=(BIGPHYS_PAGES_2-RING_PAGES_2)*PAGE_SIZE-MAXPACKET_2;
  buf_eend=(BIGPHYS_PAGES_2-RING_PAGES_2)*PAGE_SIZE-TAILPOS-MAXEVENT_2;
  ring_start=buf_start+(BIGPHYS_PAGES_2-RING_PAGES_2)*PAGE_SIZE;
  ring_size=(RING_PAGES_2*PAGE_SIZE-RING_ENTRY_LENGTH-TAILMEM)/RING_ENTRY_LENGTH;
  tail_start=buf_start+BIGPHYS_PAGES_2*PAGE_SIZE-TAILPOS;
  buf_pnt=0;
  ring_pnt=0;
  ring_loop=0;
  pmissing=0;
  pmissing_prev=0;

  // if(ioctl(fd_schar,SCHAR_GET_MAC,hw_source_addr){
  //  printf("Error reading MAC address\n");
  // }
  printf("NIC MAC addr.  (source): ");
  for(i=0;i<6;i++){
    printf("%02x:",hw_source_addr[i]&0xff);
  }
  printf("\n");
  return 1;
}

int eth_register_mac(){
  int  i,mac_idx;
  mac_idx = 0; 
  if((ether_header[mac_idx]=(struct ethhdr *)malloc(sizeof(struct ethhdr)))==0){
    printf("Could not register MAC address: Allocation error\n");
    return -1;
  }
  memcpy(ether_header[mac_idx]->h_dest,hw_dest_addr, ETH_ALEN);
  memcpy(ether_header[mac_idx]->h_source,hw_source_addr, ETH_ALEN);
  // printf("Registered Dest. MAC %2d \n ",mac_idx);
  for(i=0;i<6;i++){
    printf("%02x:",ether_header[mac_idx]->h_dest[i]&0xff);
  }
  printf("\n");
  return mac_idx;
}


int eth_reset(void){ 
  if(ioctl(fd_schar,SCHAR_RESET)){
    printf("Error in SCHAR_RESET \n");
  }
  buf_pnt=0;
  ring_pnt=0;
  ring_loop=0;
  pmissing=0;
  pmissing_prev=0;

  return 0;
}

/* int eth_read()
   {
   int size;
   int lp;
   size=0;
   do {
   size=read(fd_schar,rbuf,nrbuf);
   printf(" size %d \n",size);
   for(lp=0;size>=0 && size <7 && rbuf[0]==0x03 && lp<100;lp++){
   usleep(100);
   size=read(fd_schar,rbuf,nrbuf);
   }
   } while(size>6 && (rbuf[0]&1)==1); //ignore multicast packets
   return size;
   } */

int eth_write(){
  int pkt_size;//,pkt_stp;
  int i,n_written;
  int cur_mac; 
  printf(" Creating the packet: nwbuf %d \n",nwbuf);
  //Creating the packet
  cur_mac=0;
  ether_header[cur_mac]->h_proto = htons(nwbuf);/* Length of data */
  memcpy(ebuf, ether_header[cur_mac], ETH_HLEN);
  pkt_size = ETH_HLEN + nwbuf;
  printf(" nwbuf %d \n",nwbuf);
  //  memcpy(ebuf+ETH_HLEN, wbuf, nwbuf); /*don't need this extra copy now*/
  for(i=0;i<pkt_size;i++)printf("%02X ",ebuf[i]&0xff);
  printf("\n");
  n_written = write(fd_schar, (const void *)ebuf, pkt_size);
  return n_written;
}

void eth_close(void){
  int i;
  int nmacs;
  nmacs=1;
  for(i=0;i<nmacs;i++){
    free(ether_header[i]);
  }  
  // new MemoryMapped DDU readout
  munmap((void *)buf_start,BIGPHYS_PAGES_2*PAGE_SIZE);
  close(fd_schar);
}

void eth_reset_close(void){

  // new MemoryMapped DDU readout
  munmap((void *)buf_start,BIGPHYS_PAGES_2*PAGE_SIZE);
  close(fd_schar);
}


void eth_enableBlock(void){
  int status;
  if((status=ioctl(fd_schar,SCHAR_BLOCKON))==-1){
    printf("Spy: error in enableBlock ");
  }
}

void eth_disableBlock(void){
  int status;
  if((status=ioctl(fd_schar,SCHAR_BLOCKOFF))==-1){
    printf("Spy: error in disableBlock ");
  }
}

void eth_resetAndEnable(){
  eth_reset();
  eth_enableBlock();
}

int eth_readmm(){
  //-------------------------------------------------------------------//
  //  MemoryMapped DDU2004 readout
  //-------------------------------------------------------------------//
  //  int theDataLength;
  int jloop,j;
  int length,len;
  long int iloop;
  char *buf;
  len=0;
  iloop=0;
  length=0;
  jloop=0;
  timeout=0;
  overwrite=0;
  pmissing=0;
  pmissing_prev=0;
  packets=0;
  visitCount++;
  fflush(stdout);
  while (jloop!=1){
    jloop++; 
    //printf(" iloop %d \n",iloop);
    // Get the write pointer (relative to buf_start) of the kernel driver.
    buf_pnt_kern=*(unsigned long int *)(buf_start+BIGPHYS_PAGES_2*PAGE_SIZE-TAILPOS);
    //printf(" buf_pnt_kern %ld %ld %d \n",buf_pnt_kern,buf_pnt,iloop);
    //printf("end_event %02x\n", end_event);
    if(end_event==0){
      //      for(j=0;j<100;j++)printf(" %08x ",(unsigned long int )buf_start[j]);printf("\n");
      // If no data for a long time, abort.
      if(iloop>50000){printf(" eth_readmm timeout- no data"); break;}

      // If the write pointer buf_pnt_kern hasn't yet moved from the read pointer buf_pnt, 
      // wait a bit and retry in the next loop.
      if(buf_pnt==buf_pnt_kern){for (j=0;j<5000000;j++); iloop++; continue;}

      // The kernel driver has written new data. No more idle looping. Reset the idle loop counter.
      iloop=0;

      // From the current entry of the packet info ring,...
      ring_loop_kern= *(unsigned short int *)(ring_start+ring_pnt*RING_ENTRY_LENGTH);
      // ...get the missing packet flag,...
      pmissing=ring_loop_kern&0x8000;
      // ...the end-of-event flag,...
      end_event=ring_loop_kern&0x4000;
      printf(" end_event %02x missing %02x \n",end_event,pmissing);
      end_event=0x4000;
      pmissing=0x0000;
      // ...the reset ("loop-back") counter,...
      ring_loop_kern=ring_loop_kern&0x3fff;
      // ...and the length of data in bytes.
      length=*(unsigned short int *)(ring_start+ring_pnt*RING_ENTRY_LENGTH+4);
      //    length=*(unsigned short int *)(ring_start+ring_pnt*RING_ENTRY_LENGTH+8);
      printf(" length %d \n",length);
      // Get the reset counter from the first entry of the packet info ring...
      ring_loop_kern2=*(unsigned short int *)ring_start;
      ring_loop_kern2=0x3fff&ring_loop_kern2;

      if( ( (ring_loop_kern2==ring_loop+1)&&(buf_pnt<=buf_pnt_kern) ) ||
	  (ring_loop_kern2>ring_loop+1)                                  ){
	printf(":eth_readmm buffer overwrite.");
	// Reset the read pointers.
	buf_pnt  = 0;
	ring_pnt = 0;
	// Synchronize our loop-back counter to the driver's.
	ring_loop = ring_loop_kern2;
	// Read no data this time.
	len = 0;

	break;
      }

      // The data may not have been overwritten, but the packet info ring may. 
      // Check whether the driver's loop-back count is different from ours.
      if(ring_loop_kern!=ring_loop){
	printf("eth_readmm: loop overwrite.");

	// Reset the read pointers.
	buf_pnt  = 0;
	ring_pnt = 0;
	// Synchronize our loop-back counter to the driver's.
	ring_loop = ring_loop_kern2;
	// Read no data this time.
	len = 0;
	// Let the next event start with a clean record.


	break;
      }

      // Remember the position of the start of data...
      if(packets==0){
	buf_data=buf_start+buf_pnt;
      }
      // ...and add its length to the total.
      len=len+length;
      // Increment data ring pointer...
      buf_pnt=buf_pnt+length;
      // ...and packet info ring pointer.
      ring_pnt=ring_pnt+1;

      // If this packet ends the event but another event may not fit in the remaining space (beyond buf_eend),
      // OR another packet may not fit in the remaining space (beyond buf_end),
      // OR the end of the packet info ring has been reached, 
      // then reset the read pointers (loop back) and increment the loop-back counter.
      // This condition must be exactly the same as that in the driver (eth_hook_<N>.c) for the
      // write and read pointers to loop back from the same point.

      if (((end_event==0x4000)&&(buf_pnt>buf_eend))||(buf_pnt > buf_end)||(ring_pnt>=ring_size)){
	ring_pnt=0;
	ring_loop=ring_loop+1;
	buf_pnt=0;
      }

      // Increment packet count.
      packets=packets+1;

      // Mark this event as oversized to keep a tally
      if(len>MAXEVENT_2){
	printf(" eth_readmm : event too long \n");
      }

      // If this event already has packets missing, don't read it


      // If packets are missing, don't read out anything, just keep a tally.
      if(pmissing!=0){
	pmissingCount++; 
	// Remember that we are inside a defective event until we reach the end of it.
	// (Or the end of the next event, for that matter, it the end of this one happens to be missing.)

	len = 0;
      }


    } // while (true)

    end_event=0;
    //MAINEND:
    buf=buf_data;

    //theDataLength = dataLengthWithoutPadding( buf, len );
    for(j=0;j<len;j++)rbuf[j]=buf[j];
    //   std::cout << "Data length " << len << std::endl << "without padding " << theDataLength << std::endl << std::flush;
  }
  
  printf(" pmissingCount: %02x\n", pmissingCount);

  return len;

}
