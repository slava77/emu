//----------------------------------------------------------------------
// $Id: VMEController.cc,v 3.31 2008/02/05 09:26:56 liu Exp $
// $Log: VMEController.cc,v $
// Revision 3.31  2008/02/05 09:26:56  liu
// disable changing VCC configuration in DCS
//
// Revision 3.30  2008/02/04 15:04:32  liu
// update SVFload process
//
// Revision 3.29  2008/02/04 14:02:36  liu
// further update for VCC issues
//
// Revision 3.28  2008/02/03 12:27:27  liu
// disable VCC prom read routines
//
// Revision 3.27  2008/01/31 16:34:38  liu
// disable VCC hard-reset
//
// Revision 3.26  2008/01/31 15:08:09  liu
// fix vcc_dump_config format
//
// Revision 3.25  2008/01/31 14:22:01  liu
// config change for firmware 4.x
//
// Revision 3.24  2008/01/28 09:25:13  liu
// turned on VCC error packets
//
// Revision 3.23  2008/01/24 23:16:12  liu
// update broadcast read
//
// Revision 3.22  2008/01/24 13:10:05  liu
// new controller firmware
//
// Revision 3.21  2008/01/17 16:19:15  rakness
// comment out new VMEController completely
//
// Revision 3.20  2008/01/17 11:54:02  rakness
// comment out added functions
//
// Revision 3.19  2008/01/16 16:04:28  gujh
// Added the firmware laoding
//
// Revision 3.18  2008/01/08 10:59:32  liu
// remove exit() in functions
//
// Revision 3.17  2007/12/25 00:12:31  liu
// *** empty log message ***
//
// Revision 3.16  2007/12/17 15:01:12  liu
// remove Crate dependence from VMEController
//
// Revision 3.15  2007/09/12 16:02:14  liu
// remove log4cplus dependency
//
// Revision 3.14  2007/08/28 20:38:04  liu
// remove compiler warnings
//
// Revision 3.13  2007/05/22 10:20:50  gujh
// Fix the code, so eth0 will correspond to schar0
//         --- May 22, 2007   GU
//
// Revision 3.12  2007/03/02 20:29:49  gujh
// Set the Jumbo package to <8000 (6000 for now)
// For longer controller delay, send down the eth immediately
//                          ---- Mar. 2, 2007  GU
//
// Revision 3.11  2006/11/15 16:01:37  mey
// Cleaning up code
//
// Revision 3.10  2006/11/10 16:51:45  mey
// Update
//
// Revision 3.9  2006/10/23 09:08:39  mey
// UPdate
//
// Revision 3.8  2006/10/12 17:52:13  mey
// Update
//
// Revision 3.7  2006/10/02 12:59:40  mey
// UPdate
//
// Revision 3.6  2006/09/12 15:50:01  mey
// New software changes to DMB abd CFEB
//
// Revision 3.5  2006/09/06 12:38:11  rakness
// correct time stamp/copy vectors for user prom
//
// Revision 3.4  2006/09/05 10:13:18  rakness
// ALCT configure from prom
//
// Revision 3.3  2006/08/25 03:11:33  liu
// update
//
// Revision 3.2  2006/08/21 20:34:36  liu
// update
//
// Revision 3.0  2006/07/20 21:15:48  geurts
// *** empty log message ***
//
// Revision 2.35  2006/07/20 14:03:12  mey
// Update
//
// Revision 2.34  2006/07/20 09:49:55  mey
// UPdate
//
// Revision 2.33  2006/07/19 15:23:58  mey
// UPdate
//
// Revision 2.32  2006/07/19 10:45:20  mey
// UPdate
//
// Revision 2.31  2006/07/19 09:11:19  mey
// Update
//
// Revision 2.30  2006/07/17 17:06:39  liu
// fix typo
//
// Revision 2.29  2006/07/16 04:55:32  liu
// update
//
// Revision 2.28  2006/07/16 04:15:37  liu
// update
//
// Revision 2.27  2006/07/07 10:03:53  mey
// Update
//
// Revision 2.26  2006/07/06 07:31:48  mey
// MPC firmware loading added
//
// Revision 2.25  2006/07/04 15:06:19  mey
// Fixed JTAG
//
// Revision 2.24  2006/06/16 13:05:24  mey
// Got rid of Compiler switches
//
// Revision 2.23  2006/05/31 04:55:07  liu
// Fixed bugs in initialization
//
// Revision 2.22  2006/05/30 22:49:44  liu
// update
//
// Revision 2.21  2006/05/22 04:56:03  liu
// update
//
// Revision 2.20  2006/05/22 04:49:40  liu
// update
//
// Revision 2.19  2006/05/18 15:11:10  liu
// update error handling
//
// Revision 2.18  2006/05/10 23:58:45  liu
// Update for Production Controller with firmware 3.59
//
// Revision 2.17  2006/03/10 13:13:13  mey
// Jinghua's changes
//
// Revision 2.14  2006/02/06 10:30:16  mey
// Fixed DMB loading
//
// Revision 2.13  2006/01/25 19:49:44  mey
// UPdate
//
// Revision 2.12  2006/01/16 20:29:16  mey
// Update
//
// Revision 2.11  2006/01/09 07:27:37  mey
// Update
//
// Revision 2.10  2005/12/15 14:32:21  mey
// Update
//
// Revision 2.9  2005/12/05 13:21:14  mey
// Update
//
// Revision 2.8  2005/12/05 13:10:50  mey
// Update
//
// Revision 2.7  2005/12/05 08:59:45  mey
// Update
//
// Revision 2.6  2005/12/02 18:12:30  mey
// get rid of D360
//
// Revision 2.5  2005/11/30 16:26:53  mey
// DMB Firmware upload delay
//
// Revision 2.4  2005/11/30 12:59:59  mey
// DMB firmware loading
//
// Revision 2.3  2005/11/25 23:43:00  mey
// Update
//
// Revision 2.2  2005/11/21 15:48:24  mey
// Update
//
// Revision 2.1  2005/11/02 16:16:24  mey
// Update for new controller
//
// Revision 1.25  2004/07/22 18:52:38  tfcvs
// added accessor functions for DCS integration
//
//
//----------------------------------------------------------------------
#include "VMEController.h"
#include <cmath>
#include <string>
#include <stdio.h>
#include <iostream>
#include <unistd.h> // read and write
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netpacket/packet.h>

#include <sys/socket.h>
#include <unistd.h>

#include "EMU_CC_constants.h" 

#define SCHAR_IOCTL_BASE	0xbb
#define SCHAR_RESET     	_IO(SCHAR_IOCTL_BASE, 0)
#define SCHAR_END		_IOR(SCHAR_IOCTL_BASE, 1, 0)

#define SCHAR_INQR              _IOR(SCHAR_IOCTL_BASE, 6, 0)
#define SCHAR_READ_TIMEOUT	_IOW(SCHAR_IOCTL_BASE, 2, unsigned int)


// #define MAX_DATA 8990
#define MAX_DATA 7800
#define VME_CMDS 0x20
#define ACNLG 0x20
#define ACNLG_LOOP 0x60
#define LOOP_CNTRL 0xff
#define ATYPE 2    // VME A24 bit transfer
#define TSIZE 1    // VME 16 bit data transfer
#define TTYPE 0    // VME single transfer mode


#ifndef debugV //silent mode
#define PRINT(x) 
#define PRINTSTRING(x)  
#else //verbose mode
#define PRINT(x) cout << #x << ":\t" << x << endl; 
#define PRINTSTRING(x) cout << #x << endl; 
#endif

VMEController::VMEController(): 
  port_(2), indian(SWAP),  max_buff(0), tot_buff(0), 
  plev(1), idevo(0), error_type(0), error_count(0), DEBUG(0),
  ok_vme_write_(false), fill_write_vme_vectors_(false)
{
  //
  fpacket_delay = 0;
  packet_delay = 0;
  packet_delay_flg = 0;
  //
  DELAY2 = 0.016;
  DELAY3 = 16.384;
  //
  JtagBaseAddress_ = 0x0;
  add_ucla = 0xffffffff;
  //
  usedelay_ = false ;
  useDCS_ = true;
  //
  done_init_=false;

// please note the byte swap with respect to the book values 
  CR_ethernet=0x5000;
  CR_ext_fifo=0x0200;
  CR_res_misc=0x1303;
  CR_VME_low=0x0F1D;
  CR_VME_hi=0xFFED;
  CR_BUS_timeout=0xD430;
  CR_BUS_grant=0x350C;

}
//
VMEController::~VMEController(){
  cout << "destructing VMEController .. closing socket " << endl;
  do_schar(2); // give up the schar device
}

void VMEController::init(string ipAddr, int port) {
  ipAddress_= ipAddr;
  port_=port;
  //
  init();
  //
}

void VMEController::init() 
{
  if(done_init_) return;
  //
  cout << "VMEController : Init()" << endl;
  //
  theSocket=do_schar(1); // register a new schar device
  //
  cout << "VMEController opened socket = " << theSocket << endl;
  cout << "VMEController is using eth" << port_ << endl;

  if(!useDCS_)
  {   
     vcc_check_config();

     set_ErrorServer();
  }

  done_init_=true;
  //
}

void VMEController::start(int slot, int boardtype) {
  //
  if(!done_init_) init();
  //
  board=boardtype;
  vmeadd=slot<<19;
  //std::cout << "Start.vmeadd="<<std::hex<<vmeadd<<std::endl;
  //std::cout << "Start.boardtype="<<boardtype<<std::endl;
  //std::cout << "JtagBaseAddress_="<<JtagBaseAddress_<<std::endl;
  add_ucla=vmeadd|JtagBaseAddress_;
  //std::cout << "add_ucla="<<add_ucla<<std::endl;
}

void VMEController::end() {
}

void VMEController::send_last() {
}

void VMEController::do_vme(char fcn, char vme,
                           const char *snd,char *rcv, int when) {
unsigned short int it[1]; 
unsigned short int tmp[1]={0x0000};
unsigned short int *ptr_rice;
unsigned long add_rice;
unsigned short int itwr[2]={1,3};
unsigned short int itrd[2]={2,2};
char ttt;

//printf("in VMEController::do_vme. fcn=%d, baseadd=%08X\n",fcn,vmeadd);
 if(fcn==15)return;
 add_rice=vmeadd|(unsigned char)vme;
 ptr_rice=(unsigned short int *)add_rice;
 if(fcn==2){
   //printf(" rice VME W:%08x %04x \n",ptr_rice,it[0]);
   //Jinghua Liu to added extra byte swap for those modules use do_vme(TMB,CCB,MPC)
   it[0]=snd[1]&0x00ff;
   it[0]=it[0]|((snd[0]<<8)&0xff00);
   vme_controller(itwr[when],ptr_rice,it,rcv);
 }
 //
 if(fcn==1 ){
   //printf(" rice VME R: %08x %04x \n",ptr_rice,*rcv);
   vme_controller(itrd[when],ptr_rice,tmp,rcv);
   //Jinghua Liu to added extra byte swap for those modules use do_vme(TMB,CCB,MPC)
   ttt=rcv[0];
   rcv[0]=rcv[1];
   rcv[1]=ttt;
   //
 }
 //
 if(fcn==5){ // Need this to speak to the TMB bootregister MvdM
   //
   add_rice=vmeadd|0x70000;
   ptr_rice=(unsigned short int *)add_rice;
   //
   //printf(" rice VME R: %08x %04x \n",ptr_rice,*rcv);
   vme_controller(itrd[when],ptr_rice,tmp,rcv);
   //Jinghua Liu to added extra byte swap for those modules use do_vme(TMB,CCB,MPC)
   ttt=rcv[0];
   rcv[0]=rcv[1];
   rcv[1]=ttt;
   //
   //std::cout << "ptr " << ptr_rice << std::endl;
   //
 }
 //
 if(fcn==6){ // Need this to speak to the TMB bootregister MvdM
   //
   //printf(" rice VME W:%08x %04x \n",ptr_rice,it[0]);
   //Jinghua Liu to added extra byte swap for those modules use do_vme(TMB,CCB,MPC)
   it[0]=snd[1]&0x00ff;
   it[0]=it[0]|((snd[0]<<8)&0xff00);
   //
   add_rice=vmeadd|0x70000;
   ptr_rice=(unsigned short int *)add_rice;
   //
   vme_controller(itwr[when],ptr_rice,it,rcv);
 }
 //
 if(fcn==3) sleep_vme(snd); // sleep 
 if(fcn==4) handshake_vme(); // handshake
}


int VMEController::do_schar(int open_or_close) 
{
  static int scharhandles[10];
  static int scharcounts[10];
  int realport=2;
  int schsocket;
  if(port_ >-1 && port_ <10)  realport= port_;

  if(open_or_close==1) 
  {
    if(scharcounts[realport]<1)
    {
       char schardev_name[12]="/dev/schar0";
       schardev_name[10] += realport;
       schardev_name[11]=0;
       std::cout << "Opening " << schardev_name << std::endl ;
       schsocket = open(schardev_name, O_RDWR);
       if (schsocket == -1) 
       {
          std::cout << "ERROR opening /dev/schar device...ERROR" << std::endl;
          exit(-1);
       }
       // eth_enableblock();
       eth_reset(schsocket);
       scharhandles[realport]=schsocket;
       scharcounts[realport]=0;
    }
    else 
    { 
       schsocket = scharhandles[realport];
    }
    scharcounts[realport]++;
    get_macaddr(realport);
    return schsocket;
  }
  else if(open_or_close==2)
  {
    if(scharcounts[realport]>0)
    {
       schsocket=scharhandles[realport];
       scharcounts[realport]--;    
       if(scharcounts[realport]==0) 
       {  close(schsocket);
          schsocket = 0;
       } 
       return schsocket;
    }
    return -1;
  }
  else return -100; // wrong call, should not happen
}

void udelay(long int itim)
{
  usleep(itim);
  return;
//  usleep(5000);
  //std::cout << "Udelay..." << std::endl;
  //std::cout << "Waiting...." << std::endl;
  //std::cout << "udelay..." << itim << std::endl;
//  usleep(itim*10);
  //
  struct  timeval tp;
  long usec1,usec2;
  int tim;
  int i,j,k;
  static int inter=1000;
  float xinter;
  static int mdelay;

  /* calibrate on first entry */
  if(inter==1000){
    mdelay=0;
    for(j=0;j<10;j++){
    RETRY:
      usleep(1);
      gettimeofday (&tp,NULL);
      usec1=tp.tv_usec;
      for(k=0;k<100;k++)for(i=0;i<inter;i++);
      gettimeofday (&tp,NULL);
      usec2=tp.tv_usec;
      tim=usec2-usec1;
      if(tim<0)goto RETRY;
      // printf(" inter tim %d %d \n",inter,tim);
      xinter=inter*110./tim;
      inter=(int)xinter+1;
      if(j>3&&inter>mdelay)mdelay=inter;
    }
    printf(" udelay calibration: %d loops for 1 usec \n",mdelay);
  }
  /* now do loop delay */
  //printf(" loop itim loop %d %d \n",loop,itim);
  //for(j=0;j<itim;j++){
  //for(i=0;i<mdelay;i++);
  //usleep(mdelay);
  //} 
}

void VMEController::sdly()
{
  char tmp[1]={0x00};
  unsigned short int tmp2[2]={0,0};
  unsigned short int *ptr;
  tmp2[0]=50;  // 50x16=800ns delay
  //  cout <<" sdly() called "<<endl;
  vme_controller(6,ptr,tmp2,tmp);
}


void  VMEController::sleep_vme(const char *outbuf)   // time in usec
{
  unsigned short int *time;
  unsigned long tmp_time;
  char tmp[1]={0x00};
  unsigned short int tmp2[2]={0,0};
  unsigned short int *ptr;
  time = (unsigned short int *) outbuf;
  tmp_time=time[0]*1000+15; // in nsec
  tmp_time >>= 4; // in 16 nsec
  tmp2[0]=tmp_time & 0xffff;
  tmp2[1]=(tmp_time >> 16) & 0xffff;
  vme_controller(6,ptr,tmp2,tmp);
}

void  VMEController::sleep_vme(int time) // time in usec
{
  unsigned long tmp_time;
  char tmp[1]={0x00};
  unsigned short int tmp2[2]={0,0};
  unsigned short int *ptr;
  tmp_time=time*1000+15; // in nsec
  tmp_time >>= 4; // in 16 nsec
  tmp2[0]=tmp_time & 0xffff;
  tmp2[1]=(tmp_time >> 16) & 0xffff;
  vme_controller(6,ptr,tmp2,tmp);
}

void VMEController::handshake_vme()
{
/* no such thing as handshake. The following code is meaningless.

  char tmp[1]={0x00};
  unsigned short int tmp2[1]={0x0000};
  unsigned short int *ptr;
  add_control_r=msk_control_r;   
  ptr=(unsigned short int *)add_control_r;
  vme_controller(4,ptr,tmp2,tmp);
  vme_controller(5,ptr,tmp2,tmp);

*/
}

void VMEController::flush_vme()
{
  // should never been used.
  //char tmp[1]={0x00};
  //unsigned short int tmp2[1]={0x0000};
  //unsigned short int *ptr;
  // printf(" flush buffers to VME \n");
  //vme_controller(4,ptr,tmp2,tmp); // flush
  //
}

int VMEController::eth_reset(int ethsocket)
{ 
  if(ioctl(ethsocket,SCHAR_RESET)==-1){
    std::cout << "ERROR in SCHAR_RESET" << std::endl;
    return -1;
  }
  else return 0;
}

void VMEController::clear_error()
{  
   error_count=0;
   error_type=0;
}

int VMEController::eth_read()
{  
   int size;
   int loopcnt;

   loopcnt=0;
   size=0;
GETMORE: 
   size=read(theSocket,rbuf,nrbuf);
   nrbuf=size;
//   if(size==6){nrbuf=0;return 0;}
   if(size<0)return size;
   if(size<7)
   {   if(rbuf[0]==0x04 && loopcnt<2)
       {   // usleep(50);
           loopcnt=loopcnt+1;
           goto GETMORE;
       }
   }
   return size;
}

int VMEController::eth_write()
{  char *msg;
  int msg_size;
  int nwritten;
  int i;
   //Creating the packet
     ether_header.h_proto = htons(nwbuf);
     //   ether_header.h_proto = htons(0xfff);

   msg_size = sizeof(ether_header) + nwbuf;
   if((msg = (char *)malloc(msg_size*sizeof(unsigned char))) == NULL){ 
          std::cout << "ERROR in eth_write(): malloc(): No memory available" << std::endl;
           exit(1);
   }
   memcpy(msg, &ether_header, sizeof(ether_header));
   memcpy(msg + sizeof(ether_header), wbuf, nwbuf); 
   nwritten = write(theSocket, (const void *)msg, msg_size);
// Jinghua Liu to debug
  if(DEBUG>10)
   {
     printf("ETH_WRITE****");
     for(i=0;i<msg_size;i++) printf("%02X ",msg[i]&0xff);
     printf("\n");
     printf("Packet written : %d\n", nwritten);
   }
   free(msg);
   return nwritten; 
}

//JHL  Jan. 29, 2008 new functions for VCC firmware 4.x
//
// generic function for VCC's "READ" commands 
//
int VMEController::vcc_read_command(int code, int n_words, unsigned short *readback)
{ 
   int n, l, lcnt;
   char *radd_to, *radd_from;
   int ptyp;
   const char broadcast_addr[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

   if(code<0 || code > 0xFF || n_words<=0) return -1;
   wbuf[0]=0x00;
   wbuf[1]=code & 0xFF;
   nwbuf=2;
   n=eth_write();
   if(n<2)
   {  if(DEBUG) printf("Error: Error in writing VCC %02X, bytes written %d\n", code&0xFF, n);
      return -2;
   }
   if(DEBUG) printf("write VCC command %02X, to read back %d data words\n", code&0xFF, n_words);
READ_IT_CR:
   for(l=0;l<8000;l++) lcnt++;
   n=eth_read();
// Jinghua Liu to debug
   if(DEBUG>10)
     {
        printf("Read back size %d \n",n);
        for(int i=0;i<n;i++) printf("%02X ",rbuf[i]&0xff);
        printf("\n");
     }
   if(n>6)
   {
     radd_to=rbuf;
     radd_from=rbuf+6;

// Check if the packet is from the controller, to reject unwanted broadcast packets.

     if(memcmp(radd_from, hw_dest_addr,6) && memcmp(hw_dest_addr, broadcast_addr, 6)) goto READ_IT_CR;
     ptyp = rbuf[PKT_TYP_OFF]&0xff;
     if(ptyp>=INFO_PKT) 
     {  
        // discard INFO/WARNING/ERROR packets

        if(DEBUG) printf("%s",dcode_msg_pkt(rbuf));
        goto READ_IT_CR;
     }
     if(readback) memcpy(readback, rbuf+DATA_OFF, n_words*2);
     if(DEBUG) 
     {  printf("data read back:");
        for(int i=0; i<n_words*2; i++) printf("%02X ", rbuf[DATA_OFF+i]&0xFF);
        printf("\n");
     }
   }
   else 
   {
     if(DEBUG) printf("Error: Error in reading VCC %02X\n", code&0xFF);
     return -3;
   }
   return n_words;
}

//
// generic function for VCC's "WRITE" and "No-Data" commands 
//
int VMEController::vcc_write_command(int code, int n_words, unsigned short *writedata)
{ 
   int n,l,lcnt;

// disabled changing VCC's configuration in DCS
   if(useDCS_) return -100;

   if(code<0 || code > 0xFF || n_words<0) return -1;
   wbuf[0]=0x00;
   wbuf[1]=code & 0xFF;
   if(n_words) memcpy(wbuf+2, writedata, n_words*2);
   nwbuf=2+n_words*2;
   n=eth_write();
   for(l=0;l<8000;l++) lcnt++;
   if(n<(2+n_words*2)) 
   {  if(DEBUG) printf("Error: Error in writing VCC %02X, bytes written %d\n", code&0xFF, n);
      return -2;
   }
   if(DEBUG) printf("write VCC command %02X with %d data words\n", code&0xFF, n_words);
   return n_words;
}

//
// generic function for VCC's "No-Data" commands 
//
int VMEController::vcc_write_command(int code)
{ 
  return vcc_write_command(code, 0, NULL);
}

void VMEController::vcc_check_config()
{
  // check VCC's configuration registers against "correct" values
  // and reset them if necessary.
  // in the future, the correct values should come from XML file

  unsigned short regbuf[30], tvalue[4];
  bool config_change=false;
  int n;

  n=vcc_read_command(0x0E, 7, regbuf);
  if(n!=7)
  {  printf("ERROR in reading VCC's configuration registers. Quit sending new data...\n");
     return;
  }

  // Ethernet CR
  if(regbuf[0]!=CR_ethernet)
  {   
     tvalue[0]=CR_ethernet;
     n=vcc_write_command(0x0F, 1, tvalue);
     if(n!=1)    printf("ERROR in writing VCC's Ethernet CR\n");
       else      printf("write VCC's Ethernet CR to %04X\n", CR_ethernet);
     config_change=true;
  }
  // Ext FIFO CR
  if(regbuf[1]!=CR_ext_fifo)
  {  return; 
     tvalue[0]=CR_ext_fifo;
     n=vcc_write_command(0x10, 1, tvalue);
     if(n!=1)    printf("ERROR in writing VCC's Ext FIFO CR\n");
       else      printf("write VCC's Ext FIFO CR to %04X\n", CR_ext_fifo);
     config_change=true;
  }
  // Reset Misc. CR
  if(regbuf[2]!=CR_res_misc)
  {  
     tvalue[0]=CR_res_misc;
     n=vcc_write_command(0x11, 1, tvalue);
     if(n!=1)    printf("ERROR in writing VCC's Reset Misc. CR\n");
       else      printf("write VCC's Reset Misc. CR to %04X\n", CR_res_misc);
     config_change=true;
  }
  // VME CR
  if(regbuf[4]!=CR_VME_low)
  {  
     tvalue[0]=CR_VME_hi;
     tvalue[1]=CR_VME_low;
     n=vcc_write_command(0x12, 2, tvalue);
     if(n!=2)    printf("ERROR in writing VCC's VME CR\n");
       else      printf("write VCC's VME CR to %04X %04X\n", CR_VME_hi, CR_VME_low);
     config_change=true;
  }
  // VME BUS Timeout
  if(regbuf[5]!=CR_BUS_timeout)
  {  
     tvalue[0]=CR_BUS_timeout;
     n=vcc_write_command(0x13, 1, tvalue);
     if(n!=1)    printf("ERROR in writing VCC's BUS timeout\n");
       else      printf("write VCC's BUS timeout to %04X\n", CR_BUS_timeout);
     config_change=true;
  }
  // VME BUS Grant Timeout
  if(regbuf[6]!=CR_BUS_grant)
  {  
     tvalue[0]=CR_BUS_grant;
     n=vcc_write_command(0x14, 1, tvalue);
     if(n!=1)    printf("ERROR in writing VCC's BUS Grant timeout\n");
       else      printf("write VCC's BUS Grant timeout to %04X\n", CR_BUS_grant);
     config_change=true;
  }

  // save current configuration as default
  if(config_change)
  {
     tvalue[0]=0;
     n=vcc_read_command(0x0A, 1, regbuf);
     if(n!=1)    printf("ERROR in reading VCC's configuration number\n");
       else      printf("write VCC's configuration numnber: %04X\n", regbuf[0]);
     if(n!=1 || regbuf[0]!=0) n=vcc_write_command(0x09, 1, tvalue);
     n=vcc_write_command(0x05, 1, tvalue);
     if(n!=1)    printf("ERROR in saving VCC's configuration\n");
       else      printf("VCC's current configuration saved as default\n");
  }

}

void VMEController::set_ErrorServer()
{
  // Set the controller's "Default Error Server MAC" to the current MAC if it isn't.

  unsigned short regbuf[30], tvalue[4];
  int n;

  n=vcc_read_command(0x0D, 15, regbuf);
  if(n!=15)
  {  printf("ERROR in reading VCC's Deafult Server MAC. Quit sending new data...\n");
     return;
  }
  if(memcmp(regbuf+12, hw_source_addr, 6)==0) return; 
  tvalue[0]=0x0400;
  memcpy(tvalue+1, hw_source_addr, 6);
  n=vcc_write_command(0x0B, 4, tvalue);
  if(n!=4)
  {  printf("ERROR in writing VCC's Deafult Server MAC address\n");
  }
  if(DEBUG) printf("write VCC's Default Error Server MAC to the current MAC\n");

  return;
}

void VMEController::vcc_dump_config()
{
  unsigned short regbuf[30];
  char *mmmac;
  int n, i, j;

  n=vcc_read_command(0x0E, 7, regbuf);
  if(n!=7)
  {  printf( "ERROR in reading VCC's configuration registers.\n");
  }
  else
  {  printf("VCC CRs: ");   
     for(i=0;i<7;i++) printf("%04X ", regbuf[i]&0xFFFF);
     printf("\n");
  }

  n=vcc_read_command(0x0D, 15, regbuf);
  mmmac=(char *)regbuf;
  if(n!=15)
  {  printf("ERROR in reading VCC's Deafult Server MAC.\n");
  }
  else
  {  printf("VCC MACs: \n");   
     for(j=0;j<5;j++)     
     {  printf("   ");
        for(i=0;i<6;i++) printf("%02X:", mmmac[j*6+i]&0xFF);
        printf( "\n");
     }
  }

}

void VMEController::mrst_ff()
{
  vcc_write_command(MRst_Ext_FF);
  std::cout << "Full reset of FIFO done." << std::endl;
  return;
}

void VMEController::set_VME_mode()
{
  vcc_write_command(Set_FF_VME);
  std::cout << "Controller is in VME mode." << std::endl;
  return;
}

void VMEController::reset()
{
  vcc_write_command(Force_Reload);
  std::cout << "Controller's FPGA reloaded." << std::endl;
  return;
}

bool VMEController::SelfTest()
{ 
// To read back controller serial number
   int n=vcc_read_command(0x1E, 2, NULL);
   if(n==2)
   {
      clear_error();
      return true;
   }
   else return  false;
}

bool VMEController::exist(int slot)
{ 
   unsigned char tmp[2]={0, 0};
//   bool v_return;
   unsigned short int tmp2[1]={0x0000}, *ptr;

   if(slot<1 || slot>21) return 0;
   if(slot==1) return SelfTest();
   int add_ptr = slot<<19;
   if(slot%2==1 && slot!=13) add_ptr += 0x6024;

   ptr=(unsigned short int *)add_ptr;
   vme_controller(2,ptr,tmp2,(char *)tmp);
   if(DEBUG) printf("read back: %02X%02X\n", tmp[1]&0xff, tmp[0]&0xff);
/*
   v_return=!error_count;
   clear_error();
   return v_return;
*/
   // When the controller's infor packet disabled, this is the only way,
   // it may not be always reliable though.
   return !(tmp[0]==0xAD && tmp[1]==0xBA);
}

void VMEController::disable_errpkt()
{ //disabled for now
  std::cout << "Controller error packets disabled." << std::endl;
  return;
}


void VMEController::enable_Reset()
{ //disabled for now
  std::cout << "Controller Hard Reset enabled." << std::endl;
  return;
}

void VMEController::disable_Reset()
{ //disabled for now
  std::cout << "Controller Hard Reset disabled." << std::endl;
  return;
}


void VMEController::set_Timeout(int to)
{
  // "to" is in microsecond
  if(to<0) return;
  unsigned n=(to*1000)>>4;
  unsigned short tvalue=(n>>8)&0xff +((n&0xff)<<8);
  vcc_write_command(0x13, 1, &tvalue);
  std::cout << "VME Bus Timeout set to " << to << " microseconds" <<std::endl;
  return;
}

void VMEController::set_GrantTimeout(int to)
{
  // "to" is in microsecond
  if(to<0) return;
  unsigned n=(to*1000)>>4;
  unsigned short tvalue=(n>>8)&0xff +((n&0xff)<<8);
  vcc_write_command(0x14, 1, &tvalue);
  std::cout << "VME BusGrant Timeout set to " << to << " microseconds" <<std::endl;
  return;
}

void VMEController::get_macaddr(int realport)
{
  int msock_fd;
  struct ifreq mifr;

  char eth[5]="eth2";

   eth[3] = '0' + realport; 
   //create socket
   if((msock_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
     std::cout << "Error in call: socket()" << std::endl;
     
   //get MAC address
   strcpy(mifr.ifr_name, eth);
   if(ioctl(msock_fd,SIOCGIFHWADDR,&mifr) < 0)
     std::cout << "Error in call ioctl(socket, SIOCGIFHWADDR)" << std::endl;
   
   memcpy(hw_source_addr,mifr.ifr_addr.sa_data, ETH_ALEN);
   memcpy(ether_header.h_source, hw_source_addr, ETH_ALEN);
   close(msock_fd);

   sscanf(ipAddress_.c_str(), "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
       hw_dest_addr, hw_dest_addr+1, hw_dest_addr+2,
       hw_dest_addr+3, hw_dest_addr+4, hw_dest_addr+5);
   memcpy(ether_header.h_dest, hw_dest_addr, ETH_ALEN);
}

//
int VMEController::VME_controller(int irdwr,unsigned short int *ptr,unsigned short int *data,char *rcv)
{
  /* irdwr:   
     0 bufread
     1 bufwrite 
     2 bufread snd  
     3 bufwrite snd 
     4 flush to VME (disabled)
     5 loop back (disabled)
     6 delay
  */

  const char a_mask[8]={0x00,0x20,0x40,0x50,0x80,0x90,0x00,0x00};
  const char r_mask=0x00;
  const char w_mask=0x10;
  const char ts_mask[4]={0x00,0x04,0x08,0x0c};
  const char ts_size[4]={1,2,4,8};
  const char tt_mask[4]={0x00,0x01,0x02,0x03};;
  const char delay_mask[8]={0,1,2,3,4,5,6,7};
  const char broadcast_addr[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

  int LRG_read_flag;
  static unsigned int LRG_read_pnt=0;

  static int nvme;
  static int nread=0;
  unsigned char *radd_to;
  unsigned char *radd_from;
  unsigned char *nbytet;
  unsigned short int r_nbyte;
  unsigned char *r_head0;
  unsigned char *r_head1;
  unsigned char *r_head2;
  unsigned char *r_head3;
  unsigned short r_num;
  unsigned char *r_datat;
  unsigned char return_type;
  int size,nwrtn;
  int i;
  unsigned long int ptrt;

  static int istrt=0;  

/* initialize */
  if(istrt==0){
    nwbuf=4;
    nvme=0;
    istrt=1;
  }
 
  // Jinghua Liu to debug
  if ( DEBUG ) {
    if(irdwr==6) 
      printf("vme_control: %02x %04x%04x", irdwr, data[1], data[0]);
    else
    {
      printf("vme_control: %02x %08lx",irdwr, (unsigned long int)ptr);
      if(irdwr==1 || irdwr==3) printf(" %04X",data[0]);
    }
    printf("\n");
    fflush(NULL);
  }
  //
  /* flush to vme: disabled
     1. If we know what's in the buffer, then "flush to vme"
        is, at the best, a lazy approach. It only acts as a cover-up 
        for some serious bugs in the code because the buffer 
        should never be left unattended.
     2. If we don't know what's in the buffer, then sending the buffer 
        to vme is reckless.
  */
  if(irdwr==4) return 0;


  // LOOP back to/from Controller , disabled.
  // if(irdwr==5) {wbuf[0]=ACNLG_LOOP;wbuf[1]=LOOP_CNTRL;irdwr=2;}
  if(irdwr==5) return 0;

  /* skip zero delay */
  if(irdwr==6 && data[0]==0 && data[1]==0) return 0;

  ptrt=(unsigned long int)ptr;
  // Jinghua Liu:
  // VME address higher than 0xC00000 is for broadcasting,
  // READ is not allowed in the software. 
  if((irdwr==0 || irdwr==2) && ptrt >= 0xC00000) return 0;

  // Jinghua Liu: 
  // to prevent the OSU controller hanging up on invalid VME address
  if(irdwr<=3 && ptrt<0x80000) 
      {  printf("VME ADDRESS ERROR: %06lX\n",ptrt);
         return -10;
      }
  //printf("vme_control: %02x %08x ",irdwr, (unsigned long int)ptr);
  //printf(" %02x %04x%04x\n", irdwr, data[1], data[0]);

  /*  fill buffer  */
  nvme=nvme+1;
 //  VME command function code
  // wbuf[0]=ACNLG;
  wbuf[0]=0x00;
  wbuf[1]=VME_CMDS;

  wbuf[nwbuf+0]=0x00;

  // VME Read/Write 
  if(irdwr==0||irdwr==2) {
     wbuf[nwbuf+1]=a_mask[ATYPE]|r_mask|ts_mask[TSIZE]|tt_mask[TTYPE];
     nread=nread+ts_size[TSIZE];
  }
  if(irdwr==1||irdwr==3) {
     wbuf[nwbuf+1]=a_mask[ATYPE]|w_mask|ts_mask[TSIZE]|tt_mask[TTYPE];
  } 
  if(irdwr<=3){
    wbuf[nwbuf+2]=0x00;
    wbuf[nwbuf+3]=(ptrt&0xff0000)>>16;
    wbuf[nwbuf+4]=(ptrt&0xff00)>>8;
    wbuf[nwbuf+5]=(ptrt&0xff);
    // Jinghua Liu: no byte swap for CCB,MPC,TMB 
//    wbuf[nwbuf+6]=(*data&0xff);
//    wbuf[nwbuf+7]=(*data&0xff00)>>8;
    wbuf[nwbuf+6]=(*data&0xff00)>>8;
    wbuf[nwbuf+7]=(*data&0xff);
    // end byte swap
    if(irdwr==1||irdwr==3)nwbuf=nwbuf+8;
    if(irdwr==0||irdwr==2)nwbuf=nwbuf+6;   
  } 

  // delay
  if(irdwr==6){
    // only use delay type 2 and 5 (in 16 ns)
    int delay_type=2;
    //    cout <<" really delayed "<<data[0]<<data[1]<<endl;
    if(data[1]) delay_type=5;
    wbuf[nwbuf+0]=delay_mask[delay_type];
    wbuf[nwbuf+1]=0x00;
    if(delay_type==2){
      wbuf[nwbuf+3]=(data[0]&0xff);
      wbuf[nwbuf+2]=(data[0]&0xff00)>>8;
      nwbuf=nwbuf+4;
    }else{
      wbuf[nwbuf+5]=(data[0]&0xff);
      wbuf[nwbuf+4]=(data[0]&0xff00)>>8;
      wbuf[nwbuf+3]=(data[1]&0xff);
      wbuf[nwbuf+2]=(data[1]&0xff00)>>8;
      nwbuf=nwbuf+6;
    }
    fpacket_delay=fpacket_delay+(data[0]+data[1]*65536)*DELAY2;
    //  if(delay_type==3)fpacket_delay=fpacket_delay+(*data)*DELAY3;
    irdwr=1;  // delay always acts like a buffered WRITE command.
    if (data[1]) irdwr=3;  //send immediately for longer delays  
  } 

    /* check for overflow */
    LRG_read_flag=0;
    if(nwbuf>MAX_DATA && (irdwr==1 || irdwr==0)){
       if(DEBUG){
          printf("Jumbo packet limit reached: %d, forced sending\n", nwbuf);
       }
       LRG_read_flag=1;  // flag for forced sending
       irdwr += 2;
    }

  /* write VME commands to vme */

  if(irdwr==2||irdwr==3){
    // durkin acnlg no longer needed on read
    // durkin if(nread>0) wbuf[0]=ACNLG;
    wbuf[2]=(nvme&0xff00)>>8;
    wbuf[3]=nvme&0xff;
    nwrtn=eth_write();
    //
    packet_delay=(long int)fpacket_delay+1;
    packet_delay=packet_delay+15; 
//JHL delay turned on
    if ( usedelay_ ) udelay(packet_delay);
    //
    fpacket_delay=0.0;
    packet_delay=0;
    //
    // printf(" nwrtn %d nwbuf %d \n",nwrtn,nwbuf);
    //
    nwbuf=4;
    nvme=0;
    istrt=0;

  /* for normal READ/WRITE, need copy the data out of the spebuff. */

  if(LRG_read_flag==0 && LRG_read_pnt>0)
     memcpy(rcv, spebuff, LRG_read_pnt);
   
  /* read back bytes from vme if needed */
 
  if(nread>0){
    clear_error();
READETH:
    nrbuf=nread;
    size=eth_read();
    if(size<10)
         {  printf(" ERROR: no data read back \n\n");
            int schar_status=ioctl(theSocket,SCHAR_INQR);
            if(schar_status!=-1) {
              printf("   schar driver status:\n");
              printf("      pack_left\t\t%d\n", schar_status&0xffff);
              printf("      end_cond\t\t%d\n", (schar_status>>16)&0xf);
              printf("      wake_cond\t\t%d\n", (schar_status>>20)&0xf);
              printf("      wake_stat\t\t%d\n", (schar_status>>24)&0xf);
              printf("      err_count\t\t%d\n", (schar_status>>28)&0xf);
            } else {
              printf(" failed to inquire schar driver status\n");
            }
            fflush(NULL);
            return -100;
         }
// Jinghua Liu to debug
   
    if(DEBUG>10)
    {
      printf("Read back size %d \n",size);
      for(i=0;i<size;i++) printf("%02X ",rbuf[i]&0xff);
      printf("\n");
    }
      radd_to=(unsigned char *)rbuf;
      radd_from=(unsigned char *)rbuf+6;
// Check if the packet is expected. To reject unwanted broadcast packets.
// Don't like GOTO, just keep it for the time being.
      
      if(!memcmp(radd_to, broadcast_addr, 6) || (memcmp(radd_from, hw_dest_addr,6) && memcmp(hw_dest_addr, broadcast_addr, 6)))
        { 
printf("From %02X:%02X:%02X:%02X:%02X:%02X, need %02X:%02X:%02X:%02X:%02X:%02X\n",
radd_from[0],radd_from[1],radd_from[2],radd_from[3],radd_from[4], radd_from[5],
hw_dest_addr[0],hw_dest_addr[1],hw_dest_addr[2],hw_dest_addr[3],hw_dest_addr[4], hw_dest_addr[5]);

printf("To %02X:%02X:%02X:%02X:%02X:%02X, need %02X:%02X:%02X:%02X:%02X:%02X\n",
radd_to[0],radd_to[1],radd_to[2],radd_to[3],radd_to[4],radd_to[5],
hw_source_addr[0],hw_source_addr[1],hw_source_addr[2],hw_source_addr[3],hw_source_addr[4],hw_source_addr[5]);

 goto READETH; //disable for broadcast MvdM
        }
      nbytet=(unsigned char *)rbuf+12;
      r_nbyte=((nbytet[0]<<8)&0xff00)|(nbytet[1]&0xff);
      r_head0=(unsigned char *)rbuf+14;
      r_head1=(unsigned char *)rbuf+16;
      r_head2=(unsigned char *)rbuf+18;
      r_head3=(unsigned char *)rbuf+20;
      r_datat=(unsigned char *)rbuf+22;
      r_num=((r_head3[0]<<8)&0xff00)|(r_head3[1]&0xff);  
      return_type=r_head0[1];
      if(return_type!=5)
       {  
          if(return_type==0xff)
          {
             error_type=(r_datat[0]&0xf0)>>4;
             if(error_type==0) error_type=16;
             error_count++;
             if(DEBUG) printf("Error packet: type: %d\n", return_type);
          }
          else
          {
             printf("Error: wrong return data type: %d \n", return_type);
          }
//
// Need to discard all error/warning/info packets!
// In the case of multiple VME commands in one packet, it can be
// very complicated. Have to deal with that later. Jinghua Liu 5/5/2006.
//
          goto READETH;
       }

    if(LRG_read_flag>0) 
    {  // forced read, store the data in the special buffer
// Jinghua Liu: byte swap!!!
       for(i=0;i<r_num;i++)
       {  spebuff[2*i+LRG_read_pnt]=r_datat[2*i+1];
          spebuff[2*i+1+LRG_read_pnt]=r_datat[2*i];
       }
       LRG_read_pnt += 2*r_num;  //data in special buffer
       if(DEBUG){
           printf("LARGE READ: %d bytes stored in the special buffer\n", 2*r_num);
       }
    }
    else 
    {  // normal read
// Jinghua Liu: byte swap!!!
       for(i=0;i<r_num;i++)
       {  rcv[2*i+LRG_read_pnt]=r_datat[2*i+1];
          rcv[2*i+1+LRG_read_pnt]=r_datat[2*i];
       }
       if(DEBUG) printf("return data: %02X %02X\n", r_datat[2*i]&0xFF, r_datat[2*i+1]&0xFF);
    }
    nread=0;
  }

  // after normal READ/WRITE, always turn off LARGE_read
  if(LRG_read_flag==0) LRG_read_pnt=0;   

  }
  return 0;
}

//
void VMEController::Clear_VmeWriteVecs() {
  //
  write_vme_address_.clear();
  write_data_lsb_.clear();
  write_data_msb_.clear();
  //
  return;
}
//
int VMEController::vme_controller(int irdwr,unsigned short int *ptr,unsigned short int *data,char *rcv) {
  //
  int address = ( (int)ptr ) & 0xff;
  //
  if ( Get_FillVmeWriteVecs()     &&  
       (irdwr == 1 || irdwr == 3) &&
       Get_OkVmeWriteAddress() ) {
    //    std::cout << "CTL address, data = " << std::hex 
    //    	      << address                 << " " 
    //    	      << (int)( (data[0]>>12) & 0xf )
    //    	      << (int)( (data[0]>> 8) & 0xf )
    //    	      << (int)( (data[0]>> 4) & 0xf )
    //    	      << (int)(  data[0]     & 0xf ) 
    //	      << std::endl;
    write_vme_address_.push_back( address );
    write_data_lsb_.push_back( (data[0] & 0xff) );
    write_data_msb_.push_back( ((data[0]>>8) & 0xff) );
    //    ::sleep(1);
    //
  }    
  //
  return VME_controller(irdwr,ptr,data,rcv); 
  
  //
}


void VMEController::write_Ethernet_CR(unsigned short int val)
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=0x0F;
  wbuf[2]=(val>>8)&0xff;
  wbuf[3]=val&0xff;
  nwbuf=4;
  n=eth_write();
  std::cout << "Write_Ethernet_CR" << std::endl;
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMEController::write_FIFO_CR(unsigned short int val)
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=0x10;
  wbuf[2]=(val>>8)&0xff;
  wbuf[3]=val&0xff;
  nwbuf=4;
  n=eth_write();
  std::cout << "Write_FIFO_CR" << std::endl;
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMEController::write_ResetMisc_CR(unsigned short int val)
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=0x11;
  wbuf[2]=(val>>8)&0xff;
  wbuf[3]=val&0xff;
  nwbuf=4;
  n=eth_write();
  std::cout << "Write_ResetMisc_CR" << std::endl;
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMEController::write_VME_CR(unsigned int val)
{
  int n;
  int l,lcnt;
  printf(" inside val %08x \n",val);
  wbuf[0]=0x00;
  wbuf[1]=0x12;
  wbuf[2]=(val>>24)&0xff;
  wbuf[3]=(val>>16)&0xff;
  wbuf[4]=(val>>8)&0xff;
  wbuf[5]=val&0xff;
  nwbuf=6;
  n=eth_write();
  char buf[10];
  sprintf(buf," %02x %02x %02x %02x ",wbuf[2]&0xff,wbuf[3]&0xff,wbuf[4]&0xff,wbuf[5]&0xff);
  std::cout << "Write_VME_CR" << buf << std::endl;
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMEController::write_BusTimeOut_CR(unsigned short int val)
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=0x13;
  wbuf[2]=(val>>8)&0xff;
  wbuf[3]=val&0xff;
  nwbuf=4;
  n=eth_write();
  std::cout << "Write_BusTimeOut_CR" << std::endl;
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMEController::write_BusGrantTimeOut_CR(unsigned short int val)
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=0x14;
  wbuf[2]=(val>>8)&0xff;
  wbuf[3]=val&0xff;
  nwbuf=4;
  n=eth_write();
  std::cout << "Write_BusGrantTimeOut_CR" << std::endl;
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMEController::jtag_init()
{
  int i,n;
  int rtn;
  JINSTR_t *jrtn;
  //  for(rtn=JC_Rd_DevID; rtn<=JC_Chk_Conn; rtn++){
  for(rtn=JC_Rd_DevID; rtn<=JC_User; rtn++){
    ld_rtn_base_addr(jtr[rtn].base);
    wbuf[0]=0x00;
    wbuf[1]=Write_Prg_Space;
    nwbuf=2;
    for(i=0;i<jtr[rtn].n;i++){
      jrtn = jtr[rtn].rtn+i;
      wbuf[nwbuf]=((jrtn->instr)>>8)&0xFF;
      wbuf[nwbuf+1]=(jrtn->instr)&0xFF;
      wbuf[nwbuf+2]=((jrtn->bc)>>8)&0xFF;
      wbuf[nwbuf+3]=(jrtn->bc)&0xFF;
      wbuf[nwbuf+4]=((jrtn->data)>>8)&0xFF;
      wbuf[nwbuf+5]=(jrtn->data)&0xFF;
      nwbuf+=6;
    }
    n=eth_write();
  }
}

int VMEController::chk_jtag_conn()
{
  int n,rslt;
  int ptyp;
  //Return value:
  //  1: Successful connection made 
  //  0: No response after sending check connection command 
  // -1: Problem with JTAG connection
  eth_read_timeout(RD_TMO_long);
  wbuf[0]=0x20;
  wbuf[1]=Chk_JTAG_Conn;
  nwbuf=2;
  n=eth_write();
  n=eth_read();
  if(n>6){
    ptyp = rbuf[PKT_TYP_OFF]&0xff;
    if(ptyp>=INFO_PKT){
      printf("%s",dcode_msg_pkt(rbuf));
      rslt = 0;
    }
    if(AK_STATUS(rbuf) == CC_S){
      rslt = 1;
    }else{
      eth_read_timeout(RD_TMO_short);
      n=eth_read();
      if(n>6){
	ptyp = rbuf[PKT_TYP_OFF]&0xff;
	if(ptyp>=INFO_PKT){
          printf("%s",dcode_msg_pkt(rbuf));
	}
      }
      rslt = -1;
    }
  }else{
    printf("No Response after Chk_JTAG_Conn command\n");
    rslt = 0;
  }
  eth_read_timeout(RD_TMO_short);
  return rslt;
}

unsigned int VMEController::read_dev_id()
{
// Jinghua & Ben: disabled for now Feb.3, 2008
  return 0;
  int n;
  unsigned int dev_id;
  int ptyp;
  wbuf[0]=0x00;
  wbuf[1]=Rd_Dev_ID;
  nwbuf=2;
  n=eth_write();
  n=eth_read();
  ptyp = rbuf[PKT_TYP_OFF]&0xff;
  if(ptyp>=INFO_PKT){
    printf("%s",dcode_msg_pkt(rbuf));
    return 0;
  }
  dev_id = ((rbuf[DATA_OFF+2]&0xff)<<24)|((rbuf[DATA_OFF+3]&0xff)<<16)|((rbuf[DATA_OFF]&0xff)<<8)|(rbuf[DATA_OFF+1]&0xff);
  return dev_id;
}

unsigned int VMEController::read_user_code()
{
// Jinghua & Ben: disabled for now Feb.3, 2008
  return 0;
  int n;
  unsigned int user_code;
  int ptyp;
  wbuf[0]=0x00;
  wbuf[1]=Rd_User_Code;
  nwbuf=2;
  n=eth_write();
  n=eth_read();
  ptyp = rbuf[PKT_TYP_OFF]&0xff;
  if(ptyp>=INFO_PKT){
    printf("%s",dcode_msg_pkt(rbuf));
    return 0;
  }
  user_code = ((rbuf[DATA_OFF+2]&0xff)<<24)|((rbuf[DATA_OFF+3]&0xff)<<16)|((rbuf[DATA_OFF]&0xff)<<8)|(rbuf[DATA_OFF+1]&0xff);
  return user_code;
}

char * VMEController::read_customer_code()
{
// Jinghua & Ben: disabled for now Feb.3, 2008
  return 0;
  int n;
  int ptyp;
  wbuf[0]=0x00;
  wbuf[1]=Rd_Cust_Code;
  nwbuf=2;
  n=eth_write();
  n=eth_read();
  if(n>6){
    ptyp = rbuf[PKT_TYP_OFF]&0xff;
    if(ptyp>=INFO_PKT){
      printf("%s",dcode_msg_pkt(rbuf));
      return 0;
    }
/*      for(i=DATA_OFF;i<n;i++){ */
/*        printf("%02X",rbuf[i]&0xFF); */
/*      } */
/*      printf("\n"); */
    return &rbuf[DATA_OFF];
  }
  else {
    printf("no response\n");
    return 0;
  }
}

char * VMEController::dcode_msg_pkt(char *buf)
{
  //  static char *pkt_typ[] = {"INFO","WARN","ERROR","Not a message type"};
  static char *msg_src[] = {"Misc","VME Ctrl","VME Mstr","VME Rdbk",
			    "VME IH","VME Slave","VME Arb","Ext FIFO",
			    "Eth Rcvr","Eth Trns","JTAG mod","Flash mod",
			    "Config mod","CP mod","Rst Hndlr","StrtupShdwn"};
  static char *msg_typ[] = {"INF","WRN","ERR","NAL"};
  // static char *unknwn = "Unknown message";
  static char *trns_typ[] = {"SNGL", "BLOCK", "RMW", "UNALG"};
  static char *data_sz[]   = {"D08", "D16", "D32", "D64"};
  static char *addr_sz[]   = {"Axx","A16","A24","A32","A40","A64","Axx","Axx"};
  static char *rdwrt[]     = {"Read","Write"};
  static char *dly_type[]  = {"No_Dly","D4nsX16","D16nsX16","D16usX16",
			      "D4nsX32","D16nsX32","D16usX32","undefined"};
  static char *mstr_st[]   = {"M_idle","Capture1","Req_DTB","R1","No_Exe",
			      "Not_Used","Not_Used","Not_Used","Not_Used",
			      "Wait_4_DTB",
                              "Start","Add_Phase","Wait_4_DTACK_Rel","Sngl_Block",
                              "RMW_1","RMW_Read","RMW_2","RMW_Write","M_Read",
                              "M_Write","Cycle_Done","Next_Cycle","Xfer_Done",
                              "Load_Delay","Delay","Ld_Err_Typ","Bus_Error"};
  static char *ih_st[]     = {"IH_idle","IH_Cap_IRQ","IH_Req_DTB","IH_Wait4DTB",
                              "IH_Start","IACK_Cycle","Status_ID","IACK_Done",
                              "IH_Ld_Err","IH_Bus_Err","IH_Rel_BR","Mask_Intr",
                              "IH_Ld_Warn","IH_Warn"};

  static char *acc_typ_pg[] = {"DATA","PRGM"};
  static char *acc_typ_sp[] = {"NPRV","SUPV"};
  static char *acc_typ_lk[] = {"    ","LOCK"};
  static char *acc_typ_cr[] = {"    ","CCSR"};
  static char *acc_typ_ud[] = {"    ","UDAM"};


  static char msg[MAX_MSG_SIZE];
  static char tmp[MAX_MSG_SIZE];

  union hdr_stat  unh;
  union ctrl_stat unc;
  union mstr_stat unm;

  int i;
  int ptyp;
  int ack_stat;
  int pkt_flags;
  int srcid;
  int typid;

  int ih_state,ih_irq,ih_add,curmsk;


  unsigned short int proc_tag, cmnd_ack, seq_id, wrd_cnt;
  unsigned short int code;
  struct ucw *ucptr;

  msg[0]='\0';
  ptyp = buf[PKT_TYP_OFF]&0XFF;
  if(ptyp >= INFO_PKT){

    /* Header info */
    unh.full = buf[HDR_OFF];
    ack_stat = AK_STATUS(buf);
    pkt_flags = PKT_FLAGS(buf);
    proc_tag = PROC_TAG(buf);
    cmnd_ack = buf[CMND_ACK_OFF]&0xFF;
    seq_id   = SEQ_PKT_ID(buf);
    wrd_cnt  = DAT_WRD_CNT(buf);
    /* Message info */
    srcid = SOURCE_ID(buf);
    typid = MSG_TYP_ID(buf);
    code = UNV_CODE_WRD(buf);
    ucptr = (struct ucw *)ptr_bin_srch((int)code, ucwtab, nerrcodes);

    /* Start forming message */
    if(ucptr != NULL){
      sprintf(msg,"%3s: 0x%03X: %-15s: %s\n",ucptr->type,ucptr->code,ucptr->mnem,ucptr->msg);
    /* Check for additional information */
      switch(srcid){
      case VME_CTRL:
	switch(code){
	case VC_MTEr_Fifo:
	case VC_RdEr_Units:
	case VC_RdEr_CtrlWrd:
	  break;
	default:
          strcat(msg,"Additional information:\n");
	  unc.full=SECND_WRD(buf);
	  sprintf(tmp,"\tTransfer Type: %-3s:%-3s:%-5s:%-5s\n",addr_sz[unc.tg.asz],
		  data_sz[unc.tg.dsz],trns_typ[unc.tg.tt],rdwrt[unc.tg.wrt]);
	  strcat(msg,tmp);
	  sprintf(tmp,"\t  Access Type: %4s+%4s+%4s+%4s+%4s\n",acc_typ_sp[unc.fl.sp],
		  acc_typ_pg[unc.fl.pg],acc_typ_lk[unc.fl.lk],
		  acc_typ_cr[unc.fl.cr],acc_typ_ud[unc.fl.ud]);
	  strcat(msg,tmp);
	  sprintf(tmp,"\t   Delay Type: %s\n",dly_type[unc.tg.dt]);
	  strcat(msg,tmp);
	  break;
	}
	break;
      case VME_MSTR:
        strcat(msg,"Additional information:\n");
	unm.full=SECND_WRD(buf);
	sprintf(tmp,"\t    Master State: 0x%02X: %-16s\n",unm.tg.state,mstr_st[unm.tg.state]);
	strcat(msg,tmp);
	sprintf(tmp,"\t   Transfer Type: %-3s:%-5s:%-5s\n",data_sz[unm.tg.dsz],
		trns_typ[unm.tg.tt],rdwrt[unm.tg.wrt]);
	strcat(msg,tmp);
	sprintf(tmp,"\tAddress Modifier: %02X\n",unm.tg.am&0xFF);
	strcat(msg,tmp);
	strcat(msg,"\t  64 bit Address: "); 
	for(i=0;i<8;i++)sprintf(&tmp[2*i],"%02X",buf[MSG_OFF+4+i]&0xFF);
	strcat(msg,tmp);
	strcat(msg,"\n");
	break;
      case VME_IH:
        strcat(msg,"Additional information:\n");
	switch(ptyp){
	case ERR_PKT:
	  ih_state=EXTRACT_IH_STATE(buf);
	  ih_irq=EXTRACT_IH_IRQ(buf);
	  ih_add=EXTRACT_IH_ADD(buf);
	  sprintf(tmp,"\t       IH State: 0x%01X: %-16s\n",ih_state,ih_st[ih_state]);
	  strcat(msg,tmp);
	  sprintf(tmp,"\tIH IRQs pending: ");
	  strcat(msg,tmp);
	  for(i=0;i<7;i++){
	    sprintf(&tmp[i],"%01d",(ih_irq>>(6-i))&1);
	  }
	  strcat(msg,tmp);
	  strcat(msg,"\n");
	  sprintf(tmp,"\tIH IACK Address: %1d\n",ih_add);
	  strcat(msg,tmp);
	  break;
	default:
	  curmsk=EXTRACT_IRQ_MSK(buf);
	  strcat(msg,"\tIH Current IRQ Mask: ");
	  for(i=0;i<7;i++){
	    sprintf(&tmp[i],"%01d",(curmsk>>(6-i))&1);
	  }
	  strcat(msg,tmp);
	  strcat(msg,"\n");
	  break;
	}
	break;
      case ETH_RCV:
        strcat(msg,"Additional information:\n");
	unm.full=SECND_WRD(buf);
	sprintf(tmp,"\t      Command : 0x%02X\n",cmnd_ack&0xFF);
	strcat(msg,tmp);
	sprintf(tmp,"\t  Process Tag : 0x%02X\n",proc_tag&0xFF);
	strcat(msg,tmp);
	sprintf(tmp,"\tSequential ID : 0x%04X\n",seq_id&0xFFFF);
	strcat(msg,tmp);
	sprintf(tmp,"\t   Word Count : 0x%04X (%d)\n",wrd_cnt&0xFFFF,wrd_cnt);
	strcat(msg,tmp);
	break;
      case JTAG_MOD:
        strcat(msg,"Additional information:\n");
	sprintf(tmp,"\t       Status : 0x%04hX\n",SECND_WRD(buf));
	strcat(msg,tmp);
	break;
      default:
/*          strcat(msg,"Additional information:\n"); */
/*  	unm.full=SECND_WRD(buf); */
/*  	sprintf(tmp,"\t      Command : 0x%02X\n",cmnd_ack&0xFF); */
/*  	strcat(msg,tmp); */
/*  	sprintf(tmp,"\t  Process Tag : 0x%02X\n",proc_tag&0xFF); */
/*  	strcat(msg,tmp); */
/*  	sprintf(tmp,"\tSequential ID : 0x%04X\n",seq_id&0xFFFF); */
/*  	strcat(msg,tmp); */
/*  	sprintf(tmp,"\t   Word Count : 0x%04X (%d)\n",wrd_cnt&0xFFFF,wrd_cnt); */
/*  	strcat(msg,tmp); */
/*          sprintf(tmp,"\t       Source : %01X\n",srcid&0xF); */
/*          strcat(msg,tmp); */
/*          sprintf(tmp,"\t    Msg. Type : %01X\n",typid&0xF); */
/*          strcat(msg,tmp); */
/*          sprintf(tmp,"\t         Code : %03X\n",code&0xFFF); */
/*          strcat(msg,tmp); */
/*          for(i=0;i<DATA_OFF+2*wrd_cnt;i++){ */
/*            printf("%02X",buf[i]&0xFF); */
/*          } */
/*          printf("\n"); */
	break;
      }
    }
    else {
      sprintf(msg,"%3s: 0x%03X: %-15s: Unmatched code 0x%03X is undefined.\n",msg_typ[typid],code,msg_src[srcid],code);
      strcat(msg,"Additional information:\n");
      if((pkt_flags & SPONT)!=0){
        strcat(msg,"\tThis is a spontaneous packet\n");
      }
      else {
        sprintf(tmp,"\t     Status: %-6s: %s\n",ak_status[ack_stat].mnem,ak_status[ack_stat].status);
        strcat(msg,tmp);
        sprintf(tmp,"\tProcess Tag: 0x%02X\n",proc_tag);
        strcat(msg,tmp);
        sprintf(tmp,"\tCommand Ack: 0x%02X\n",cmnd_ack);
        strcat(msg,tmp);
        sprintf(tmp,"\tSeq.Pkt.ID : %d\n",seq_id);
        strcat(msg,tmp);
      }
    }
  }
  else {
    strcat(msg,"Not a message packet type");
  }
  strcat(msg,"\n");
  return msg;
}

void * VMEController::ptr_bin_srch(int code, struct ucw *arr, int n)
{
  struct ucw *low  = &arr[0];
  struct ucw *high = &arr[n-1];
  struct ucw *mid;

  while(low < high){
    mid = low + (high-low)/2;
    if(code < mid->code){
      high = mid;
    }
    else if(code > mid->code){
      low = mid;
    }
    else {
      return mid;
    }
  }
  return NULL;
}

struct rspn_t VMEController::flush_pkts()
{
  int n;
  union hdr_stat hdr;
  int pkt_type;
  //int srcid;
  int ackn;

  int spont,frag,newp,prio,data;
  // int maxbytes,maxwrds,maxints,maxlongs;
  int numwrds;
  //unsigned char *cp;
  // unsigned short int *hp;
  unsigned short int code;
  // unsigned int *ip,*lph,*lpl;

  unsigned char opcode;
  struct ucw *fopptr;
  struct ucw *pktptr;
  struct rspn_t rsp;

  rsp.npkt=0;
  rsp.nspt=0;
  rsp.nerr=0;
  rsp.nberr=0;
  rsp.nbto=0;
  rsp.nwrn=0;
  rsp.ninf=0;
  rsp.nintr=0;
  rsp.ndat=0;
  rsp.nack_only=0;
  rsp.nbad=0;
  rsp.buf_cnt=0;
  rsp.buf_typ=0;
  rsp.totwrds=0;
  rsp.ackn=0;

  while((n=eth_read())>6){
    rsp.npkt++;
    hdr.full = rbuf[HDR_OFF];
    spont = hdr.tg.spnt;
    frag  = hdr.tg.frag;
    newp   = hdr.tg.newp;
    prio  = hdr.tg.prio;
    ackn  = AK_STATUS(rbuf);
    data  = WITH_DATA(rbuf);
    pkt_type = rbuf[PKT_TYP_OFF] & 0xFF;
    opcode = rbuf[CMND_ACK_OFF];
    fopptr = (struct ucw *)ptr_bin_srch((int)opcode,foptab,nopcodes);
    pktptr = (struct ucw *)ptr_bin_srch((int)pkt_type,pkttab,npktcodes);
    numwrds = DAT_WRD_CNT(rbuf);
    if(spont){
      rsp.nspt++;
      if(pkt_type>=INFO_PKT){
        printf("    Message from controller:\n");
        printf("    %s", dcode_msg_pkt(rbuf));
	// if(fplog != NULL){
          printf("    Message from controller:\n");
          printf("    %s", dcode_msg_pkt(rbuf));
	  // }
        code = UNV_CODE_WRD(rbuf);
        switch(pkt_type){
	case INFO_PKT:
	  rsp.ninf++;
	  break;
	case WARN_PKT:
	  rsp.nwrn++;
	  break;
	case ERR_PKT:
 	  rsp.nerr++;
	  if(code == VM_BERR_Slv || code == VI_BERR_Slv){
            rsp.nberr++;
          }
          if(code == VM_BTO || code == VI_BTO){
            rsp.nbto++;
	  }
	  break;
	default:
	  break;
	}
      }
      else if(pkt_type>=IHD08_STAT_ID_PKT && pkt_type<=IHD32_STAT_ID_PKT){
        rsp.nintr++;
        rsp.totwrds += numwrds;
      }
      else {
	printf("Unexpected Spontaneous packet type: %02X\n",pkt_type&0xFF);
      }
    }
    else if(ackn >0 && data ==0){
      rsp.nack_only++;
      rsp.ackn=ackn;
      printf("Acknowledge packet:\n  Status:%-6s:%20s %s\n",ak_status[ackn].mnem,fopptr->mnem,ak_status[ackn].status);
    }
    else if(numwrds > 0){
      rsp.ndat++;
      rsp.totwrds += numwrds;
      printf("Flushing %d of data from a %s packet\n",numwrds,pktptr->mnem);
    }
    else {
      rsp.nbad++;
      printf("Illegal Header encountered\n");
    }
  }
  return rsp;
}

int VMEController::erase_prom()
{
  int n,rslt;
  int ptyp;
  //Return value:
  //  1: Successful completion of erase 
  //  0: No response after sending erase command 
  // -1: Unsuccessful erasure 
  eth_read_timeout(RD_TMO_xlong);
  wbuf[0]=0x20;
  wbuf[1]=Erase_PROM;
  nwbuf=2;
  n=eth_write();
  n=eth_read();
  if(n>6){
    ptyp = rbuf[PKT_TYP_OFF]&0xff;
    if(ptyp>=INFO_PKT){
      printf("%s",dcode_msg_pkt(rbuf));
      rslt = 0;
    }
    if(AK_STATUS(rbuf) == CC_S){
      rslt = 1;
    }else{
      eth_read_timeout(RD_TMO_short);
      n=eth_read();
      if(n>6){
	ptyp = rbuf[PKT_TYP_OFF]&0xff;
	if(ptyp>=INFO_PKT){
          printf("%s",dcode_msg_pkt(rbuf));
	}
      }
      rslt = -1;
    }
  }else{
    printf("No Response after Erase_PROM command\n");
    rslt = 0;
  }
  eth_read_timeout(RD_TMO_short);
  return rslt;
}

int VMEController::erase_prom_bcast()
{
  int n;

  //Return value:
  //  1: Successful completion of erase 
  //  0: No response after sending erase command 
  // -1: Unsuccessful erasure 
  wbuf[0]=0x00;
  wbuf[1]=Erase_PROM;
  nwbuf=2;
  n=eth_write();
  sleep(15);
  return 1;
}

void VMEController::program_prom_cmd()
{
  int n;
  wbuf[0]=0x20;
  wbuf[1]=Program_PROM;
  nwbuf=2;
  n=eth_write();
}
void VMEController::program_prom_cmd_no_ack()
{
  int n;
  wbuf[0]=0x00;
  wbuf[1]=Program_PROM;
  nwbuf=2;
  n=eth_write();
}
void VMEController::reload_fpga()
{
  int n;
  int ptyp;
  wbuf[0]=0x00;
  wbuf[1]=Reload_FPGA;
  nwbuf=2;
  n=eth_write();
  while((n=eth_read())>6){
    ptyp = rbuf[PKT_TYP_OFF]&0xff;
    if(ptyp>=INFO_PKT){
      printf("%s",dcode_msg_pkt(rbuf));
    }
  }
}
void VMEController::verify_prom_cmd()
{
  int n;
  wbuf[0]=0x20;
  wbuf[1]=Verify_PROM;
  nwbuf=2;
  n=eth_write();
}
void VMEController::ld_rtn_base_addr(unsigned short base)
{
  int n;
  wbuf[0]=0x00;
  wbuf[1]=Ld_Rtn_Base_Addr;
  wbuf[2]=(base>>8)&0xFF;
  wbuf[3]=base &0xFF;
  nwbuf=4;
  n=eth_write();
}
void VMEController::exec_routine(int rtn)
{
  int n;
  wbuf[0]=0x20;
  wbuf[1]=Exec_Routine;
  wbuf[2]=0x00;
  wbuf[3]=(unsigned char) rtn;
  nwbuf=4;
  n=eth_write();
}
unsigned int VMEController::get_jtag_status()
{
  int n;
  int ptyp;
  unsigned int status;
  wbuf[0]=0x00;
  wbuf[1]=Module_Status;
  nwbuf=2;
  n=eth_write();
  n=eth_read();
  ptyp = rbuf[PKT_TYP_OFF]&0xff;
  if(ptyp>=INFO_PKT){
    printf("%s",dcode_msg_pkt(rbuf));
    return 0;
  }
  status = ((rbuf[DATA_OFF]&0xff)<<8)|(rbuf[DATA_OFF+1]&0xff);
  return status;
}
void VMEController::abort_jtag_cmnds()
{
  int n;
  wbuf[0]=0x00;
  wbuf[1]=Abort_JTAG_Cmnds;
  nwbuf=2;
  n=eth_write();
}

void VMEController::write_jtag_fifo_words(unsigned short *buf, int nw)
{
  int i,n;
  wbuf[0]=0x00;
  wbuf[1]=Write_JTAG_FIFO;
  nwbuf=2;
  for(i=0; i<nw; i++){
    wbuf[nwbuf]=(buf[i]>>8)&0xFF;
    wbuf[nwbuf+1]=(buf[i])&0xFF;
    nwbuf+=2;
  }
  n=eth_write();
}
void VMEController::write_jtag_fifo_bytes(unsigned char *buf, int nb)
{
  int i,n;
  wbuf[0]=0x00;
  wbuf[1]=Write_JTAG_FIFO;
  nwbuf=2;
  for(i=0; i<nb; i++){
    wbuf[nwbuf]=(buf[i])&0xFF;
    nwbuf++;
  }
  n=eth_write();
}
int VMEController::read_prg_space(unsigned short base)
{  
  int i,n,nwrds;
  int offset;
  int ptyp;
  ld_rtn_base_addr(base);
  wbuf[0]=0x00;
  wbuf[1]=Read_Prg_Space;
  nwbuf=2;
  n=eth_write();
  n=eth_read();
  ptyp = rbuf[PKT_TYP_OFF]&0xff;
  if(ptyp>=INFO_PKT){
    printf("%s",dcode_msg_pkt(rbuf));
    return 0;
  }
  nwrds = DAT_WRD_CNT(rbuf);
  printf("Nwords = %d\n",nwrds);
  for(i=0;i<nwrds/3;i++){
    offset = 6*i+DATA_OFF;
    rbk_prg[i].instr = ((rbuf[offset]&0xff)<<8)|(rbuf[offset+1]&0xff);
    rbk_prg[i].bc    = ((rbuf[offset+2]&0xff)<<8)|(rbuf[offset+3]&0xff);
    rbk_prg[i].data  = ((rbuf[offset+4]&0xff)<<8)|(rbuf[offset+5]&0xff);
  }
  return nwrds/3;
}

void VMEController::print_routines()
{
  int i,ninstr;
  int rtn;
  char *rtnstr[] = {"Read Device ID", "Read User Code", "Read Customer Code",
                    "Readback PROM", "Erase PROM", "Program PROM", 
                    "Reload FPGA", "Verify PROM", "Check JTAG Connection"};
  FILE *fp;
  fp=fopen("prgmemrbk.dat","w");
  //  for(rtn=JC_Rd_DevID; rtn<=JC_Chk_Conn; rtn++){
  for(rtn=JC_Rd_DevID; rtn<=JC_User; rtn++){
    fprintf(fp,"\nRoutine: %s\n-----------------------------------------\n",rtnstr[rtn]);
    ninstr = read_prg_space(jtr[rtn].base);
    fprintf(fp,"ninstr = %d\n",ninstr);
    fprintf(fp,"Address  Instruction  Bit Count  Data\n");
    fprintf(fp,"--------------------------------------\n");
    for(i=0;i<ninstr;i++){
      fprintf(fp,"  %03X :      %02X        %04hX     %04hX\n",jtr[rtn].base+i,
             rbk_prg[i].instr&0x3F,rbk_prg[i].bc,rbk_prg[i].data);
    }
  }
  fclose(fp);
}
void  VMEController::rd_back_prom()
{
  int i,n,wc,naddr,wcntr,prtd,npkt;
  unsigned int faddr,chksum;
  unsigned short addr[2048];
  int ptyp;
  union {
    unsigned char *cp;
    unsigned short int *shtp;
  } flex_buf;
  FILE *fp;
  fp = fopen("Prom_readback.dat","w");
  naddr = 1024;
  faddr = 0;
  for(i=0;i<naddr;i++){
    addr[2*i] = (1024*i)&0xFFFF;
    addr[2*i+1] = ((1024*i)>>16)&0xFF;
  }
  wbuf[0]=0x00;
  wbuf[1]=Rd_Back_PROM;
  nwbuf=2;
  n=eth_write();
  write_jtag_fifo_words(addr,2*naddr);
  wcntr=0;
  prtd=0;
  npkt=0;
  while((n=eth_read())>6){
    npkt++;
    flex_buf.cp =(unsigned char *) &rbuf[DATA_OFF];
    wc = DAT_WRD_CNT(rbuf);
    //    printf("Packet no. %d; size %d; word count %d\n",npkt,n,wc);
    ptyp = rbuf[PKT_TYP_OFF]&0xff;
    if(ptyp>=INFO_PKT){
      printf("%s",dcode_msg_pkt(rbuf));
    } else {
      for(i=0;i<wc;i++){
	if(faddr%0x10000==0 && !prtd){
	  chksum = ~((6+((faddr>>24)&0xFF)+((faddr>>16)&0xFF))%256)+1;
	  fprintf(fp,":02000004%04X%02X\r\n",(faddr>>16)&0xFFFF,chksum&0xFF);
          prtd=1;
	}
        if(wcntr%8==0){
          chksum = 16+((faddr>>8)&0xFF)+(faddr&0xFF);
          fprintf(fp,":10%04X00",faddr&0xFFFF);
	}
        chksum = chksum+(flex_buf.cp[2*i]&0xFF)+(flex_buf.cp[2*i+1]&0xFF);
        fprintf(fp,"%04hX",flex_buf.shtp[i]);
        wcntr++;
        if(wcntr!=0 && wcntr%8==0){
          fprintf(fp,"%02X\r\n",(~(chksum%256)+1)&0xFF);
          faddr+=16;
          prtd=0;
	}
      }
    }
  }
  fprintf(fp,":00000001FF\r\n");
  fclose(fp);
}

void VMEController::read_mcs(char *fn)
{
  char line[1024];
  FILE *fp;
  int paddr,lpcnt,parms;
  unsigned int tt;
  unsigned int ddl,ddh;
  unsigned int ll,addr;

  fp = fopen(fn,"r");
  paddr = 0;
  lpcnt = 0;
  tt=0;
  while(tt!=1){
    parms = fscanf(fp,":%2x%4x%2x",&ll,&addr,&tt);
    //    printf("%1x:%02X%04X%02X",parms,ll,addr,tt);
    if(tt==0){
      for(unsigned int ii=0;ii<ll/2;ii++){
        parms = fscanf(fp,"%2x%2x",&ddl,&ddh);
        prm_dat[paddr++] = ((ddh&0xFF)<<8) | (ddl&0xFF);
	//        printf("%5X: %04hX\n",paddr,prm_dat[paddr-1]);
      }
    }
    fgets(line,1024,fp);
    lpcnt++;
    //    printf("%s %d\n",line,lpcnt);
  }
  fclose(fp);
}

void VMEController::send_ver_prom_data()
{
  int i,j,n,nw,pktnum,nsend,tot_bytes;
  int lp,cdwrd,send,pause,abort;
  int addr,pnum,bi;
  int ptyp;
  int ack;
  VERIFY_t ver_dat[147];
  nw = 512;
  send = 1;
  pause = 0;
  abort = 0;
  pktnum = 0;
  lp=0;
  addr = 0;
  nsend = 0;
  pnum =-1;
  for(i=0;i<1024;i++){
    if(i!=0 && i%7==0){
      ver_dat[pnum].nw = bi;
    }
    if(i%7==0){
      ver_dat[++pnum].buf = (unsigned short int *)malloc(2*7*(nw+2));
      bi = 0;
    }
    ver_dat[pnum].buf[bi++] = addr&0xFFFF;
    ver_dat[pnum].buf[bi++] = (addr>>16)&0xFF;
    for(j=0;j<nw;j++){
      ver_dat[pnum].buf[bi++] = prm_dat[nw*i+j];
    }
    addr+=1024;
  }
  ver_dat[pnum].nw = bi;
  tot_bytes = 0;
  while(lp<pnum+1 && !abort){
    if(send){
      nsend++;
      write_jtag_fifo_words(ver_dat[lp].buf, ver_dat[lp].nw);
      tot_bytes+=2*ver_dat[lp++].nw;
      printf("%d: have sent %d bytes\n",nsend,tot_bytes);
      if(nsend%4 == 0){
        pause = 1;
        send  = 0;
        abort = 0;
      }
    }
    if(pause){
      while((n=eth_read())>6){
	pktnum++;
	printf("\n");
	ptyp = rbuf[PKT_TYP_OFF]&0xff;
	if(ptyp>=INFO_PKT){
	  printf("pktnum A%d: %s",pktnum,dcode_msg_pkt(rbuf));
	  cdwrd = UNV_CODE_WRD(rbuf);
	  if(cdwrd==JT_Buf_AF){
	    pause = 1;
	    send = 0;
	    abort = 0;
	  } else if(cdwrd==JT_Buf_AMT){
	    pause = 0;
	    send = 1;
	    abort = 0;
	  } else if((cdwrd==JT_Buf_Ovfl) || (cdwrd==JT_Ver_Fail) || (cdwrd==JT_Buf_RdErr)){
	    pause = 0;
	    send = 0;
	    abort = 1;
	  }
	} else {
	  printf("pktnum A%d:",pktnum);
	  ack = AK_STATUS(rbuf); 
	  switch(ack){
	  case NO_ACK:
	    printf("Non message or no acknowledge packet received\n");
	    break;
	  case CC_S:
	    printf("Verify completed successfully\n");
	    break;
	  case CC_W: case CC_E: case CE_I: case CIP_W: case CIP_E:
	    printf("\aAn error or warning occured verify!\n");
	    break;
	  default:
	    printf("Unknown packet returned\n");
	    break;
	  }
	}
      }
    }
  }
  if(abort){
    abort_jtag_cmnds();
  }
  while((n=eth_read())>6){
    pktnum++;
    ptyp = rbuf[PKT_TYP_OFF]&0xff;
    if(ptyp>=INFO_PKT){
      printf("pktnum B%d: %s",pktnum,dcode_msg_pkt(rbuf));
    } else {
      printf("pktnum B%d:",pktnum);
      ack = AK_STATUS(rbuf); 
      switch(ack){
      case NO_ACK:
	printf("Non message or no acknowledge packet received\n");
	break;
      case CC_S:
	printf("Verify completed successfully\n");
	break;
      case CC_W:
	printf("Verify completed with a warning\n");
	break;
      case CC_E:
	printf("Verify completed with an error\n");
	break;
      case CE_I:
	printf("Verify execution finished incomplete\n");
	break;
      case CIP_W: case CIP_E:
	printf("Command in progress with an error or warning\n");
	break;
      default:
	printf("Unknown packet returned\n");
	break;
      }
    }
  }
  for(i=0;i<pnum+1;i++){
    free(ver_dat[i].buf);
  }
}

void VMEController::send_prg_prom_data()
{
  int n,nw,pktnum,nsend,tot_bytes;
  int cdwrd,send,pause,abort;
  int ptyp;
  int ack;
  int npus_max = 32768;
  int npus_per_pkt_max = 249;
  int npus,tot_npus;
  send = 1;
  pause = 0;
  abort = 0;
  pktnum = 0;
  nsend = 0;
  tot_npus = 0;
  tot_bytes = 0;
  while(tot_npus<npus_max && !abort){
    if(send){
      nsend++;
      if((npus_max-tot_npus)< npus_per_pkt_max){
        npus = npus_max - tot_npus;
      }else{
        npus = npus_per_pkt_max;
      }
      nw=16*npus;
      write_jtag_fifo_words(&prm_dat[16*tot_npus], nw);
      tot_npus+=npus;
      tot_bytes+=2*nw;
      printf("%d: have sent %d pu's and %d bytes\n",nsend,tot_npus,tot_bytes);
      if(nsend%4 == 0){
        pause = 1;
        send  = 0;
        abort = 0;
      }
    }
    if(pause){
      while((n=eth_read())>6){
	pktnum++;
	printf("\n");
	ptyp = rbuf[PKT_TYP_OFF]&0xff;
	if(ptyp>=INFO_PKT){
	  printf("pktnum A%d: %s",pktnum,dcode_msg_pkt(rbuf));
	  cdwrd = UNV_CODE_WRD(rbuf);
	  if(cdwrd==JT_Buf_AF){
	    pause = 1;
	    send = 0;
	    abort = 0;
	  } else if(cdwrd==JT_Buf_AMT){
	    pause = 0;
	    send = 1;
	    abort = 0;
	  } else if((cdwrd==JT_Buf_Ovfl) || (cdwrd==JT_Ver_Fail) || (cdwrd==JT_Buf_RdErr)){
	    pause = 0;
	    send = 0;
	    abort = 1;
	  }
	} else {
	  printf("pktnum A%d:",pktnum);
	  ack = AK_STATUS(rbuf); 
	  switch(ack){
	  case NO_ACK:
	    printf("Non message or no acknowledge packet received\n");
	    break;
	  case CC_S:
	    printf("Programming completed successfully\n");
	    break;
	  case CC_W: case CC_E: case CE_I: case CIP_W: case CIP_E:
	    printf("\aAn error or warning occured during programming\n");
	    break;
	  default:
	    printf("Unknown packet returned\n");
	    break;
	  }
	}
      }
    }
  }
  if(abort){
    abort_jtag_cmnds();
  }
}
void VMEController::send_prg_prom_data_bcast()
{
  int nw,nsend,tot_bytes;
  int npus_max = 32768;
  int npus_per_pkt_max = 249;
  int npus,tot_npus;
  nsend = 0;
  tot_npus = 0;
  tot_bytes = 0;
  while(tot_npus<npus_max){
    nsend++;
    if((npus_max-tot_npus)< npus_per_pkt_max){
      npus = npus_max - tot_npus;
    }else{
      npus = npus_per_pkt_max;
    }
    nw=16*npus;
    write_jtag_fifo_words(&prm_dat[16*tot_npus], nw);
    tot_npus+=npus;
    tot_bytes+=2*nw;
    printf("%d: have sent %d pu's and %d bytes\n",nsend,tot_npus,tot_bytes);
    if(nsend%4 == 0){
      usleep(500000);
    }
  }
}
void VMEController::send_uc_cc_data(char *fn)
{
  int i;
  unsigned int uc,proj,maj_ver,min_ver;
  char cc[33];
  for(i=0;i<33;i++){
    cc[i] = 0;
  }
  strncpy(cc,fn,32);
  sscanf(fn,"%5X.V%1X.%2X.mcs",&proj,&maj_ver,&min_ver);
  printf("proj: %05X\n",proj);
  printf("majv: %01X\n",maj_ver);
  printf("minv: %02X\n",min_ver);
  uc = (proj<<12)|(maj_ver<<8)|(min_ver);
  printf("uc  : %08X\n",uc);
  printf("cc  : %s\n\n",cc);

  write_jtag_fifo_words((unsigned short *)&uc, 2);
/*    printf("user code packet:\n"); */
/*    for(i=0;i<6;i++)printf("%02X",wbuf[i]&0xFF); */
/*    printf("\n"); */

  write_jtag_fifo_words((unsigned short *)&uc, 2);
/*    printf("user code verify packet:\n"); */
/*    for(i=0;i<6;i++)printf("%02X",wbuf[i]&0xFF); */
/*    printf("\n"); */

  write_jtag_fifo_bytes((unsigned char *)cc, 32);
/*    printf("customer code packet:\n"); */
/*    for(i=0;i<34;i++)printf("%02X",wbuf[i]&0xFF); */
/*    printf("\n"); */
}
    
void VMEController::prg_vcc_prom_ver(const char *path,const char *ver)
{
  char fname[36];
  char fullname[256];
  int n,pktnum,rslt;
  unsigned int temp_uint;
  static unsigned int device_id = 0xF5057093;
  char *tmp_cp;
  int ptyp;
  int ack;
  char buf[100];
  strcpy(fname,"D783C.V");
  strcat(fname,ver);
  strcat(fname,".mcs");
  strcpy(fullname,path);
  strcat(fullname,"/");
  strcat(fullname,fname);
  sprintf(buf," %s \n",fullname);
  std::cout << "VMECC PromLoad Filename: "<< buf << "\n" <<std::endl;
  rslt = chk_jtag_conn();
  if(rslt == 1){
    temp_uint = read_dev_id();
    if(temp_uint == device_id){
      temp_uint = read_user_code();
      printf("User Code is %08X\n",temp_uint);
      tmp_cp = read_customer_code();
      if(tmp_cp!=0){
	printf("PROM currently programmed with %s\n",tmp_cp);
      }
      else {
	printf("Did not receive customer code. Proceeding anyway\n");
      }
      printf("Erasing PROM...\n");
      rslt = erase_prom();
      if(rslt == 1){
	printf("PROM successfully erased.\n");
	pktnum = 0;
	printf("Reading PROM data file...\n");
	read_mcs(fullname);
	printf("Programming...\n");
	program_prom_cmd();
	send_prg_prom_data();
	send_uc_cc_data(fname);
	while((n=eth_read())>6){
	  pktnum++;
	  ptyp = rbuf[PKT_TYP_OFF]&0xff;
	  if(ptyp>=INFO_PKT){
	    printf("pktnum B%d: %s",pktnum,dcode_msg_pkt(rbuf));
	  } else {
	    printf("pktnum B%d:",pktnum);
	    ack = AK_STATUS(rbuf); 
	    switch(ack){
	    case NO_ACK:
	      printf("Non message or no acknowledge packet received\n");
	      break;
	    case CC_S:
	      printf("Programming completed successfully\n");
	      break;
	    case CC_W:
	      printf("Programming completed with a warning\n");
	      abort_jtag_cmnds();
	      break;
	    case CC_E:
	      printf("Programming completed with an error\n");
	      abort_jtag_cmnds();
	      break;
	    case CE_I:
	      printf("Command execution finished incomplete\n");
	      abort_jtag_cmnds();
	      break;
	    case CIP_W: case CIP_E:
	      printf("Command in progress with an error or warning\n");
	      break;
	    default:
	      printf("Unknown packet returned\n");
	      break;
	    }
	  }
	}
	printf("Verifying...\n");
	verify_prom_cmd();
	send_ver_prom_data();
	printf("Reprogramming FPGA...\n");
	reload_fpga();
      }
      else{
	printf("\a\aPROM is not erased!\n");
      }
    }
    else {
      printf("\a\aWrong device ID -- aborting\n"); 
    }
  }
  else{
    printf("\a\aJTAG connection is bad! Can not proceed.\n");
  }
}

void VMEController::prg_vcc_prom_bcast(const char *path, const char *ver)
{
  char fname[36];
  char fullname[256];
  int rslt;
  //unsigned int temp_uint;
//  static unsigned int device_id = 0xF5057093;
  //char *tmp_cp;

  strcpy(fname,"D783C.V");
  strcat(fname,ver);
  strcat(fname,".mcs");
  strcpy(fullname,path);
  strcat(fullname,"/");
  strcat(fullname,fname);

  printf("Disabling messages...\n");
  set_clr_bits(CLR,RESET,~RST_CR_MSGLVL);

  printf("Erasing PROM...\n");
  rslt = erase_prom_bcast();
  printf("Reading PROM data file...\n");
  read_mcs(fullname);
  printf("Programming...\n");
  program_prom_cmd_no_ack();
  send_prg_prom_data_bcast();
  send_uc_cc_data(fname);
  printf("Reprogramming FPGA...\n");
  reload_fpga();
  printf("Done\n");
}

int VMEController::eth_read_timeout(int rd_tmo)
{
  if(ioctl(theSocket,SCHAR_READ_TIMEOUT,rd_tmo)){
    printf("Error in SCHAR_READ_TIMEOUT \n");
  }
  return 0;
}

int VMEController::set_clr_bits(enum SET_CLR sc, enum CR_ID crid, unsigned int mask)
{
  int n,l,lcnt,err;
  err=0;
  wbuf[0]=0x00;
  wbuf[1]=Set_Clr_CRs;
  wbuf[2]=sc;
  wbuf[3]=crid;
  nwbuf=4;
  switch(crid){
    case ETHER: case EXTFIFO: case RESET: case BTO: case BGTO:
      wbuf[4]=(mask>>8)&0xFF;
      wbuf[5]=mask&0xFF;
      nwbuf+=2;
      break;
    case VME:
      wbuf[4]=(mask>>24)&0xFF;
      wbuf[5]=(mask>>16)&0xFF;
      wbuf[6]=(mask>>8)&0xFF;
      wbuf[7]=(mask)&0xFF;
      nwbuf+=4;
      break;
    default:
      printf("illeagal CR ID\n");
      err=-1;
      break;
  }
  if(err!=0){
    return(err);
  }
  n=eth_write();
  for(l=0;l<8000;l++)lcnt++;
  return(n);
}

int VMEController::read_dev_id_broadcast(char * crates_info)
{
// JHL & Ben: disabled Feb 4, 2008 
  return 0;
  int n=0, cs=0;
  int ptyp;

  std::cout<<"Enter: read_dev_id_broadcast() \n"<<std::endl;
  wbuf[0]=0x00;
  wbuf[1]=Rd_Dev_ID;
  nwbuf=2;
  n=eth_write();
  eth_read_timeout(RD_TMO_short);
  while(n>8){
    n=eth_read();

    ptyp = rbuf[PKT_TYP_OFF]&0xff;
    if(ptyp>=INFO_PKT){
      printf("%s",dcode_msg_pkt(rbuf));
      return cs;
    }
    if(n>8){
      memcpy(crates_info+cs*10, rbuf+MAC_SRC_OFF, 6);
      crates_info[cs*10+6]=rbuf[DATA_OFF+1];
      crates_info[cs*10+7]=rbuf[DATA_OFF];
      crates_info[cs*10+8]=rbuf[DATA_OFF+3];
      crates_info[cs*10+9]=rbuf[DATA_OFF+2];
      cs++;
    }
  }
  eth_read_timeout(RD_TMO_short);
  return cs;
}

int VMEController::vme_read_broadcast(char *dmbs_info)
{ 
  int n=100, ndmbs=0;
  int ptyp;
  if(nrbuf>8){
      memcpy(dmbs_info+ndmbs*10, rbuf+MAC_SRC_OFF, 6);
      dmbs_info[ndmbs*10+6]=rbuf[DATA_OFF+1];
      dmbs_info[ndmbs*10+7]=rbuf[DATA_OFF];
      dmbs_info[ndmbs*10+8]=rbuf[DATA_OFF+3];
      dmbs_info[ndmbs*10+9]=rbuf[DATA_OFF+2];
      ndmbs++;
  }
  eth_read_timeout(RD_TMO_short);
  while(n>8){
    n=eth_read();
    
    ptyp = rbuf[PKT_TYP_OFF]&0xff;
    if(ptyp>=INFO_PKT){
      if(DEBUG) printf("%s",dcode_msg_pkt(rbuf));
      return ndmbs;
    }
    if(n>8){
      memcpy(dmbs_info+ndmbs*10, rbuf+MAC_SRC_OFF, 6);
      dmbs_info[ndmbs*10+6]=rbuf[DATA_OFF+1];
      dmbs_info[ndmbs*10+7]=rbuf[DATA_OFF];
      dmbs_info[ndmbs*10+8]=rbuf[DATA_OFF+3];
      dmbs_info[ndmbs*10+9]=rbuf[DATA_OFF+2];
      ndmbs++;
    }
  }
  eth_read_timeout(RD_TMO_short);
  return ndmbs;
}

