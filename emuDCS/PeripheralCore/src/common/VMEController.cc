//----------------------------------------------------------------------
// $Id: VMEController.cc,v 3.18 2008/01/08 10:59:32 liu Exp $
// $Log: VMEController.cc,v $
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

#define SCHAR_IOCTL_BASE	0xbb
#define SCHAR_RESET     	_IO(SCHAR_IOCTL_BASE, 0)
#define SCHAR_END		_IOR(SCHAR_IOCTL_BASE, 1, 0)
#define SCHAR_BLOCKON		_IOR(SCHAR_IOCTL_BASE, 2, 0)
#define SCHAR_BLOCKOFF		_IOR(SCHAR_IOCTL_BASE, 3, 0)
#define SCHAR_DUMPON		_IOR(SCHAR_IOCTL_BASE, 4, 0)
#define SCHAR_DUMPOFF		_IOR(SCHAR_IOCTL_BASE, 5, 0)
#define SCHAR_INQR              _IOR(SCHAR_IOCTL_BASE, 6, 0)

#define        Set_FF_VME 0x02
#define       MRst_Ext_FF 0xE6


// #define MAX_DATA 8990
#define MAX_DATA 6000
#define VME_CMDS 0x20
#define ACNLG 0x20
#define ACNLG_LOOP 0x60
#define LOOP_CNTRL 0xff
#define ATYPE 2    // VME A24 bit transfer
#define TSIZE 1    // VME 16 bit data transfer
#define TTYPE 0    // VME single transfer mode
#define PACKETOUTDUMP 0   // to print dump set 1


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
  //
  done_init_=false;
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

void VMEController::init() {
  //
  // SendOutput("VMEController : Init()","INFO");
  cout << "VMEController : Init()" << endl;
  //
  theSocket=do_schar(1); // register a new schar device
  //
  cout << "VMEController opened socket = " << theSocket << endl;
  cout << "VMEController is using eth" << port_ << endl;
  enable_Reset(); 
  //read_CR();
  //
  // This writes the default VME CR we need and then stores it in the flash memory....
  write_VME_CR();
  save_cnfg_num(1);
  set_cnfg_dflt(1);
  //read_CR();
  //
  disable_errpkt();
  //
  done_init_=true;
  //
}

void VMEController::reset() {
//  mrst_ff();
//  set_VME_mode();   
  reload_FPGA();  
//  enable_Reset();
  disable_errpkt();
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
  usleep(5000);
  //std::cout << "Udelay..." << std::endl;
  //std::cout << "Waiting...." << std::endl;
  //std::cout << "udelay..." << itim << std::endl;
  usleep(itim*10);
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
   if(size<0)return size;
   if(size<7)
   {   if(rbuf[0]==0x03&&loopcnt<10)
       {   usleep(1000);
           loopcnt=loopcnt+1;
           goto GETMORE;
       }
   }
   return size;
}

void VMEController::mrst_ff()
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=MRst_Ext_FF;
  nwbuf=2;
  n=eth_write();
  std::cout << "Full reset of FIFO done." << std::endl;
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMEController::save_cnfg_num(int cnum)
{
  int n,l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=Save_Cnfg_Num;
  wbuf[2]=0x00;
  wbuf[3]=(cnum&0x1f);  // cnum must be <= 20
  nwbuf=4;
  n=eth_write();
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMEController::set_cnfg_dflt(int dflt)
{
  int n,l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=Set_Cnfg_Dflt;
  wbuf[2]=0x00;
  wbuf[3]=(dflt&0x1f);
  nwbuf=4;
  n=eth_write();
  for(l=0;l<8000;l++)lcnt++;
  return ;
}

void VMEController::set_VME_mode()
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=Set_FF_VME;
  nwbuf=2;
  n=eth_write();
  std::cout << "Controller is in VME mode." << std::endl;
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMEController::reload_FPGA()
{
  int n;
  int l,lcnt;
  wbuf[0]=0x40;
  wbuf[1]=0xF9;
  nwbuf=2;
  n=eth_write();
  std::cout << "Controller's FPGA reloaded." << std::endl;
  for(l=0;l<80000;l++)lcnt++;
  return;
}

bool VMEController::SelfTest()
{ 
   int size, i, l,lcnt;
   unsigned char *radd_to, *radd_from;

// To read back controller serial number
   wbuf[0]=0x20;
   wbuf[1]=0x1E;
   nwbuf=2;
   eth_write();
   for(l=0;l<8000;l++) lcnt++;
   do {
     size=eth_read();
     if(size<10) return 0;
// Jinghua Liu to debug
     if(DEBUG>10)
     {
        printf("Read back size %d \n",size);
        for(i=0;i<size;i++) printf("%02X ",rbuf[i]&0xff);
        printf("\n");
     }
     radd_to=(unsigned char *)rbuf;
     radd_from=(unsigned char *)rbuf+6;

// Check if the packet is from the controller, to reject unwanted broadcast packets.
// No check on the content of the return packet (yet).

   } while(memcmp(radd_from, hw_dest_addr,6));
   clear_error();
   return 1;
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
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=0x0F;
  wbuf[2]=0x00;
  wbuf[3]=0x10;
  nwbuf=4;
  n=eth_write();
  std::cout << "Controller error packets disabled." << std::endl;
  for(l=0;l<8000;l++)lcnt++;
  return;
}


void VMEController::enable_Reset()
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=0x11;
  wbuf[2]=0x00;
  wbuf[3]=0x1B;
  nwbuf=4;
  n=eth_write();
  std::cout << "Controller Hard Reset enabled." << std::endl;
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMEController::disable_Reset()
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=0x11;
  wbuf[2]=0x00;
  wbuf[3]=0x13;
  nwbuf=4;
  n=eth_write();
  std::cout << "Controller Hard Reset disabled." << std::endl;
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMEController::write_VME_CR()
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=0x12;
  wbuf[2]=0x20;
  wbuf[3]=0x00;
  wbuf[4]=0x1D;
  wbuf[5]=0x1F;
  nwbuf=6;
  n=eth_write();
  std::cout << "WriteCR" << std::endl;
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMEController::read_CR()
{
  int n;
  int l,lcnt;
  wbuf[0]=0x20;
  wbuf[1]=0x0E;
  nwbuf=2;
  n=eth_write();
  //
  int size=eth_read();
  //
  printf("Read back size %d \n",size);
  for(int i=0;i<size;i++) printf("%02X ",rbuf[i]&0xff);
  printf("\n");
  //
  for(l=0;l<8000;l++)lcnt++;
  return;
}


void VMEController::set_Timeout(int to)
{
  // "to" is in microsecond
  int n;
  int l,lcnt;
  if(to<0) return;
  n=(to*1000)>>4;
  wbuf[0]=0x00;
  wbuf[1]=0x13;
  wbuf[2]=(n>>8)&0xff;
  wbuf[3]=n&0xff;
  nwbuf=4;
  n=eth_write();
  std::cout << "VME Bus Timeout set to " << to << " microseconds" <<std::endl;
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMEController::set_ErrorServer()
{
  // Set the controller's "Default Error Server" to the current MAC.
  // In future firmware, hopefully this will be no longer necessary.
  int n,l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=0x0b;
  wbuf[2]=0x00;
  wbuf[3]=0x04;
  memcpy(wbuf+4, hw_source_addr, 6);
  nwbuf=10;
  n=eth_write();
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMEController::set_GrantTimeout(int to)
{
  // "to" is in microsecond
  int n;
  int l,lcnt;
  if(to<0) return;
  n=(to*1000)>>4;
  wbuf[0]=0x00;
  wbuf[1]=0x14;
  wbuf[2]=(n>>8)&0xff;
  wbuf[3]=n&0xff;
  nwbuf=4;
  n=eth_write();
  std::cout << "VME BusGrant Timeout set to " << to << " microseconds" <<std::endl;
  for(l=0;l<8000;l++)lcnt++;
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
 

  //printf("vme_control: %02x %08x %04x \n",irdwr, (unsigned long int)ptr, data[0]);


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
    if(nread>0) wbuf[0]=ACNLG;
    wbuf[2]=(nvme&0xff00)>>8;
    wbuf[3]=nvme&0xff;
    nwrtn=eth_write();
    //
    packet_delay=(long int)fpacket_delay+1;
    packet_delay=packet_delay+15; 
    //if ( usedelay_ ) udelay(packet_delay);
    //
    fpacket_delay=0.0;
    packet_delay=0;
    //
    // printf(" nwrtn %d nwbuf %d \n",nwrtn,nwbuf);
    //
    nwbuf=4;
    nvme=0;

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
      
      if(!memcmp(radd_to, broadcast_addr, 6) || memcmp(radd_from, hw_dest_addr,6))
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
// Need to discard all error packets!
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
//
