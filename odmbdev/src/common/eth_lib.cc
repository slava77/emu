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
//#include "emu/odmbdev/eth_hook_2.h"
//#include "emu/odmbdev/Spy.h"
#include "emu/odmbdev/page.h"
#ifndef __NETINET_IF_ETHER_H
#include <netinet/if_ether.h>
#endif

#define PAGE_SIZE 4096
#define BIGPHYS_PAGES_2 5000 
#define RING_PAGES_2 1000
#define RING_ENTRY_LENGTH 8
#define MAXPACKET_2 9100
//#define MAXEVENT_2 30100
#define MAXEVENT_2 180000
#define TAILMEM 100
#define TAILPOS  80
#define SKB_EXTRA 12
#define SKB_OFFSET -14

using namespace std;

unsigned char hw_dest_addr[6]={0x02,0x00,0x00,0x00,0x00,0xFF};
unsigned char hw_source_addr[6]={0x00,0x00,0x00,0x00,0x00,0x00};
struct ethhdr *ether_header[2];

struct EthBuf {
    char *buf_start;            	///< pointer to start of data ring buffer
    unsigned long int buf_pnt;  	///< read pointer (index; number of bytes) w.r.t. the beginning of data ring buffer
    unsigned long int buf_end;  	///< end of data ring buffer w.r.t its beginning
    unsigned long int buf_eend; 	///< index in data ring buffer beyond which an event may not fit any more
    char *ring_start;           	///< pointer to start of packet info ring buffer 
    unsigned long int ring_size;        ///< size of packet info ring buffer
    unsigned long int ring_pnt; 	///< read pointer (index; number of bytes) w.r.t. the beginning of packet info ring buffer
    unsigned short ring_loop;           ///< the number of times the reading of the data ring buffer has looped back
    unsigned short pmissing;    	///< packets are  missing at beginning
    unsigned short pmissing_prev; 	///< packets are missing at end
    unsigned short packets; 		///< number of packets in event
    char *tail_start;           	///< not used
    int fd_schar;
};

struct EthStr {
  int n_evt;
  vector<char> rbuf;
};

void eth_open(char *dev_name, EthBuf &eth); 
void eth_reset(EthBuf &eth);
void eth_reset_close(EthBuf &eth);
void eth_register_mac();
EthStr eth_readmm(EthBuf &eth);
void eth_close(EthBuf &eth); 

void eth_open(char *dev_name, EthBuf &eth)
{
  char *dev;
  if((dev=(char *)malloc((6+strlen(dev_name))*sizeof(char)))==0){
    printf("Device name allocation error!\n");
    return;
  }
  sprintf(dev,"/dev/%s",dev_name);
  int fd_schar = open(dev, O_RDWR);
  if (fd_schar == -1) {
    perror("open");
    return;
  }
  printf("Opened network path on device %s\n",dev);
  free(dev);

  // eth_reset(); 

  char *buf_start = (char *)mmap(NULL,BIGPHYS_PAGES_2*PAGE_SIZE,PROT_READ,MAP_SHARED,fd_schar,0);

  if (buf_start==MAP_FAILED) {
    printf("emu::daq::reader::Spy::open: FATAL in memorymap - ");
    printf("emu::daq::reader::Spy::open will abort!!!");
    abort();
  }
  printf("emu::daq::reader::Spy::open: Memory map succeeded ");

  eth.buf_start=buf_start;
  eth.fd_schar=fd_schar;
  eth.buf_end=(BIGPHYS_PAGES_2-RING_PAGES_2)*PAGE_SIZE-MAXPACKET_2;
  eth.buf_eend=(BIGPHYS_PAGES_2-RING_PAGES_2)*PAGE_SIZE-TAILPOS-MAXEVENT_2;
  eth.ring_start=buf_start+(BIGPHYS_PAGES_2-RING_PAGES_2)*PAGE_SIZE;
  eth.ring_size=(RING_PAGES_2*PAGE_SIZE-RING_ENTRY_LENGTH-TAILMEM)/RING_ENTRY_LENGTH;
  eth.tail_start=buf_start+BIGPHYS_PAGES_2*PAGE_SIZE-TAILPOS;
  eth.buf_pnt=0;
  eth.ring_pnt=0;
  eth.ring_loop=0;
  eth.pmissing=0;
  eth.pmissing_prev=0;

  // if(ioctl(fd_schar,SCHAR_GET_MAC,hw_source_addr){
  //  printf("Error reading MAC address\n");
  // }
  printf("NIC MAC addr.  (source): ");
  for(int i=0;i<6;i++){
    printf("%02x:",hw_source_addr[i]&0xff);
  }
  printf("\n");
}


void  eth_reset(EthBuf &eth){ 
  if(ioctl(eth.fd_schar,SCHAR_RESET)){
    printf("Error in SCHAR_RESET \n");
  }
  eth.buf_pnt=0;
  eth.ring_pnt=0;
  eth.ring_loop=0;
  eth.pmissing=0;
  eth.pmissing_prev=0;

}


void eth_reset_close(EthBuf &eth){
  // new MemoryMapped DDU readout
  munmap((void *)eth.buf_start,BIGPHYS_PAGES_2*PAGE_SIZE);
  close(eth.fd_schar);
}


void eth_register_mac(){
  int mac_idx = 0; 
  if((ether_header[mac_idx]=(struct ethhdr *)malloc(sizeof(struct ethhdr)))==0){
    printf("Could not register MAC address: Allocation error\n");
    return;
  }
  memcpy(ether_header[mac_idx]->h_dest,hw_dest_addr, ETH_ALEN);
  memcpy(ether_header[mac_idx]->h_source,hw_source_addr, ETH_ALEN);
  // printf("Registered Dest. MAC %2d \n ",mac_idx);
  for(int i=0;i<6;i++){
    printf("%02x:",ether_header[mac_idx]->h_dest[i]&0xff);
  }
  printf("\n");
}


EthStr eth_readmm(EthBuf &eth){
  //-------------------------------------------------------------------//
  //  MemoryMapped DDU2004 readout
  //-------------------------------------------------------------------//
  char *buf;
  long int iloop=0;
  int len=0;
  int jloop=0;
  vector<char> rbufs;

  int pmissingCount=0;          ///< the number of times packets were missing
  eth.pmissing=0;
  eth.pmissing_prev=0;
  eth.packets=0;

  while (jloop!=1){
    jloop++; 
    char *buf_data;             ///< pointer to data to be read from ring buffer

    // Get the write pointer (relative to buf_start) of the kernel driver.
    unsigned long int buf_pnt_kern=*(unsigned long int *)(eth.buf_start+BIGPHYS_PAGES_2*PAGE_SIZE-TAILPOS);
    unsigned short end_event;   ///< end of event seen
    if(end_event==0){
      //      for(int j=0;j<100;j++)printf(" %08x ",(unsigned long int )buf_start[j]);printf("\n");
      // If no data for a long time, abort.
      if(iloop>50000){printf(" eth_readmm timeout- no data"); break;}

      // If the write pointer buf_pnt_kern hasn't yet moved from the read pointer buf_pnt, 
      // wait a bit and retry in the next loop.
      if(eth.buf_pnt==buf_pnt_kern){for (int j=0;j<5000000;j++); iloop++; continue;}

      // The kernel driver has written new data. No more idle looping. Reset the idle loop counter.
      iloop=0;

      // From the current entry of the packet info ring,...
      unsigned short ring_loop_kern;
      ring_loop_kern= *(unsigned short int *)(eth.ring_start+eth.ring_pnt*RING_ENTRY_LENGTH);
      // ...get the missing packet flag,...
      eth.pmissing=ring_loop_kern&0x8000;
      // ...the end-of-event flag,...
      end_event=ring_loop_kern&0x4000;
      printf(" end_event %02x missing %02x \n",end_event,eth.pmissing);
      end_event=0x4000;
      eth.pmissing=0x0000;
      // ...the reset ("loop-back") counter,...
      ring_loop_kern=ring_loop_kern&0x3fff;
      // ...and the length of data in bytes.
      int length=*(unsigned short int *)(eth.ring_start+eth.ring_pnt*RING_ENTRY_LENGTH+4);
      //    length=*(unsigned short int *)(ring_start+ring_pnt*RING_ENTRY_LENGTH+8);
      printf(" length %d \n",length);
      // Get the reset counter from the first entry of the packet info ring...
      unsigned short ring_loop_kern2;
      ring_loop_kern2=*(unsigned short int *)eth.ring_start;
      ring_loop_kern2=0x3fff&ring_loop_kern2;

      if( ( (ring_loop_kern2==eth.ring_loop+1)&&(eth.buf_pnt<=buf_pnt_kern) ) ||
	  (ring_loop_kern2>eth.ring_loop+1)                                  )
      {
	printf(":eth_readmm buffer overwrite.");
	// Reset the read pointers.
	eth.buf_pnt  = 0;
	eth.ring_pnt = 0;
	// Synchronize our loop-back counter to the driver's.
	eth.ring_loop = ring_loop_kern2;
	// Read no data this time.
	len = 0;

	break;
      }

      // The data may not have been overwritten, but the packet info ring may. 
      // Check whether the driver's loop-back count is different from ours.
      if(ring_loop_kern!=eth.ring_loop){
	printf("eth_readmm: loop overwrite.");

	// Reset the read pointers.
	eth.buf_pnt  = 0;
	eth.ring_pnt = 0;
	// Synchronize our loop-back counter to the driver's.
	eth.ring_loop = ring_loop_kern2;
	// Read no data this time.
	len = 0;
	// Let the next event start with a clean record.

	break;
      }

      // Remember the position of the start of data...
      if(eth.packets==0){
	buf_data=eth.buf_start+eth.buf_pnt;
      }
      // ...and add its length to the total.
      len += length;
      // Increment data ring pointer...
      eth.buf_pnt += length;
      // ...and packet info ring pointer.
      eth.ring_pnt++;

      // If this packet ends the event but another event may not fit in the remaining space (beyond buf_eend),
      // OR another packet may not fit in the remaining space (beyond buf_end),
      // OR the end of the packet info ring has been reached, 
      // then reset the read pointers (loop back) and increment the loop-back counter.
      // This condition must be exactly the same as that in the driver (eth_hook_<N>.c) for the
      // write and read pointers to loop back from the same point.

      if (((end_event==0x4000)&&(eth.buf_pnt>eth.buf_eend))||(eth.buf_pnt > eth.buf_end)||(eth.ring_pnt>=eth.ring_size)){
	eth.ring_pnt=0;
	eth.ring_loop++;
	eth.buf_pnt=0;
      }

      // Increment packet count.
      eth.packets++;

      // Mark this event as oversized to keep a tally
      if(len>MAXEVENT_2){
	printf(" eth_readmm : event too long \n");
      }

      // If this event already has packets missing, don't read it


      // If packets are missing, don't read out anything, just keep a tally.
      if(eth.pmissing!=0){
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
    for(int j=0;j<len;j++) {rbufs.push_back(buf[j]);}//rbuf[j]=buf[j];
    //   std::cout << "Data length " << len << std::endl << "without padding " << theDataLength << std::endl << std::flush;
  }
  
  printf(" pmissingCount: %02x\n", pmissingCount);

  EthStr o_ethstr;
  o_ethstr.n_evt = len;
  o_ethstr.rbuf  = rbufs;

  return o_ethstr;

}


void eth_close(EthBuf &eth){

  free(ether_header[0]);
  // new MemoryMapped DDU readout
  munmap((void *)eth.buf_start,BIGPHYS_PAGES_2*PAGE_SIZE);
  close(eth.fd_schar);
}


