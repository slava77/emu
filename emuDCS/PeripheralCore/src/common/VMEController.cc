//----------------------------------------------------------------------
// $Id: VMEController.cc,v 3.52 2008/09/21 18:24:35 liu Exp $
// $Log: VMEController.cc,v $
// Revision 3.52  2008/09/21 18:24:35  liu
// change JTAG Idle sequence
//
// Revision 3.51  2008/08/13 11:30:54  geurts
// introduce emu::pc:: namespaces
// remove any occurences of "using namespace" and make std:: references explicit
//
// Revision 3.50  2008/06/19 18:54:40  bylsma
// Added accessor functions for VCC xml parameters
//
// Revision 3.49  2008/06/09 09:38:02  bylsma
// Added Set... routines for new VCC xml parameters, and initialized parameter variables
//
// Revision 3.48  2008/05/20 10:46:01  liu
// error handling update
//
// Revision 3.47  2008/05/16 13:41:03  liu
// fix a bug in vme delay
//
// Revision 3.46  2008/04/22 11:02:16  liu
// update error messages
//
// Revision 3.45  2008/04/22 08:32:35  liu
// Ben's Crate controller utilities
//
// Revision 3.44  2008/04/15 14:29:49  liu
// enable VCC hard reset
//
// Revision 3.43  2008/03/27 15:20:49  liu
// turn on controller delay in SVFload
//
// Revision 3.42  2008/03/27 14:12:48  liu
// increase controller jumbo packet delay
//
// Revision 3.41  2008/03/07 10:26:25  liu
// changed some comments
//
// Revision 3.40  2008/03/01 15:06:06  liu
// update
//
// Revision 3.39  2008/02/29 08:55:11  liu
// updated delays and error message handling
//
// Revision 3.38  2008/02/27 17:02:35  liu
// add Info/Warning packet handling
//
// Revision 3.37  2008/02/26 18:23:37  gujh
// Fix the VMEController::prg_vcc readback check
//
// Revision 3.36  2008/02/26 18:11:33  gujh
// Enable the VCC usercode, IDcode, customcode read back
//
// Revision 3.35  2008/02/25 13:04:34  liu
// bug fixes for VCC firmware downloading
//
// Revision 3.34  2008/02/21 12:24:30  liu
// *** empty log message ***
//
// Revision 3.33  2008/02/19 14:39:45  gujh
// VME Controller fix
//
// Revision 3.32  2008/02/18 12:09:19  liu
// new functions for monitoring
//
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
// disable VCC hardvme_-reset
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

// durkin kludge 
#define PKT_TYP_OFF  15
#define DATA_OFF     22
#define INFO_PKT   0XFD

namespace emu {
  namespace pc {


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
  //
  JtagBaseAddress_ = 0x0;
  add_ucla = 0xffffffff;
  //
  usedelay_ = false ;
  useDCS_ = true;
  alive_=true;
  Warn_On_Shtdwn=false;
  Pkt_On_Startup=false;
  //
  done_init_=false;

// please note the byte swap with respect to the book values 
  CR_ethernet=0x5000;
  CR_ext_fifo=0x0200;
  CR_res_misc=0x1B02;
  CR_VME_low=0x0F1D;
  CR_VME_hi=0xFFED;
  CR_BUS_timeout=0xD430;
  CR_BUS_grant=0x350C;
  //other default values
  Msg_Lvl=2;
  for(int i=0;i<5;i++){
    MCAST_1[i]=0xFF;
    MCAST_2[i]=0xFF;
    MCAST_3[i]=0xFF;
  }
  MCAST_1[5]=0xFE;
  MCAST_2[5]=0xFD;
  MCAST_3[5]=0xFC;
  Dflt_Srv_MAC[0]=0x00;
  Dflt_Srv_MAC[1]=0x11;
  Dflt_Srv_MAC[2]=0x95;
  Dflt_Srv_MAC[3]=0x8B;
  Dflt_Srv_MAC[4]=0x48;
  Dflt_Srv_MAC[5]=0x89;
}
//
VMEController::~VMEController(){
  std::cout << "destructing VMEController .. closing socket " << std::endl;
  do_schar(2); // give up the schar device
}

void VMEController::init(std::string ipAddr, int port) {
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
  std::cout << "VMEController : Init()" << std::endl;
  //
  theSocket=do_schar(1); // register a new schar device
  //
  std::cout << "VMEController opened socket = " << theSocket << std::endl;
  std::cout << "VMEController is using eth" << port_ << std::endl;
//   cout << "VCC Config Parameters are:" << endl;
//   cout << "  ipAddress : " << ipAddress_ << endl;
//   cout << "  MAC_addr  : " << std::hex << (int) MAC_addr[0] << "-" << (int) MAC_addr[1] << "-" << (int) MAC_addr[2] << "-" << (int) MAC_addr[3] << "-" << (int) MAC_addr[4] << "-" << (int) MAC_addr[5] <<  endl;
//   cout << "  MCAST_1  : " << std::hex << (int) MCAST_1[0] << "-" << (int) MCAST_1[1] << "-" << (int) MCAST_1[2] << "-" << (int) MCAST_1[3] << "-" << (int) MCAST_1[4] << "-" << (int) MCAST_1[5] <<  endl;
//   cout << "  MCAST_2  : " << std::hex << (int) MCAST_2[0] << "-" << (int) MCAST_2[1] << "-" << (int) MCAST_2[2] << "-" << (int) MCAST_2[3] << "-" << (int) MCAST_2[4] << "-" << (int) MCAST_2[5] <<  endl;
//   cout << "  MCAST_3  : " << std::hex << (int) MCAST_3[0] << "-" << (int) MCAST_3[1] << "-" << (int) MCAST_3[2] << "-" << (int) MCAST_3[3] << "-" << (int) MCAST_3[4] << "-" << (int) MCAST_3[5] <<  endl;
//   cout << "  Dflt_Srv_MAC  : " << std::hex << (int) Dflt_Srv_MAC[0] << "-" << (int) Dflt_Srv_MAC[1] << "-" << (int) Dflt_Srv_MAC[2] << "-" << (int) Dflt_Srv_MAC[3] << "-" << (int) Dflt_Srv_MAC[4] << "-" << (int) Dflt_Srv_MAC[5] <<  endl;
//   cout << "  Ethernet CR  : " << std::hex <<(int) CR_ethernet <<  endl;
//   cout << "  Ext_FIFO CR  : " << std::hex << (int) CR_ext_fifo <<  endl;
//   cout << "  Rst_Misc CR  : " << std::hex << CR_res_misc <<  endl;
//   cout << "  VME(low) CR  : " << std::hex << CR_VME_low <<  endl;
//   cout << "  VME(high) CR : " << std::hex << CR_VME_hi <<  endl;
//   cout << "  BTO          : " << std::hex << CR_BUS_timeout <<  endl;
//   cout << "  BGTO         : " << std::hex << CR_BUS_grant <<  endl;
//   cout << "  Msg_Lvl      : " << std::dec << Msg_Lvl <<  endl;
//   cout << "  Warn_On_Shtdwn : " << boolalpha << Warn_On_Shtdwn <<  endl;
//   cout << "  Pkt_On_Startup : " << boolalpha << Pkt_On_Startup <<  endl;

  if(!useDCS_)
  {   
     vcc_check_config();

     set_ErrorServer();
  }

  done_init_=true;
  //
}

void VMEController::SetMAC(int type, std::string MAC) {
  unsigned char tmp[6];
  sscanf(MAC.c_str(), "%02hhx-%02hhx-%02hhx-%02hhx-%02hhx-%02hhx",
       tmp, tmp+1, tmp+2, tmp+3, tmp+4, tmp+5);
  switch(type){
  case 0:
    memcpy(MAC_addr, tmp, 6);
    break;
  case 1:
    memcpy(MCAST_1, tmp, 6);
    break;
  case 2:
    memcpy(MCAST_2, tmp, 6);
    break;
  case 3:
    memcpy(MCAST_3, tmp, 6);
    break;
  case 4:
    memcpy(Dflt_Srv_MAC, tmp, 6);
    break;
  default:
    break;
  }
}

std::string VMEController::GetMAC(int type) {
  char s[32];
  std::string stmp;
  char sfmt[]="%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX";
  switch(type){
  case 0:
    sprintf(s,sfmt,MAC_addr[0],MAC_addr[1],MAC_addr[2],MAC_addr[3],MAC_addr[4],MAC_addr[5]);
    break;
  case 1:
    sprintf(s,sfmt,MCAST_1[0],MCAST_1[1],MCAST_1[2],MCAST_1[3],MCAST_1[4],MCAST_1[5]);
    break;
  case 2:
    sprintf(s,sfmt,MCAST_2[0],MCAST_2[1],MCAST_2[2],MCAST_2[3],MCAST_2[4],MCAST_2[5]);
    break;
  case 3:
    sprintf(s,sfmt,MCAST_3[0],MCAST_3[1],MCAST_3[2],MCAST_3[3],MCAST_3[4],MCAST_3[5]);
    break;
  case 4:
    sprintf(s,sfmt,Dflt_Srv_MAC[0],Dflt_Srv_MAC[1],Dflt_Srv_MAC[2],Dflt_Srv_MAC[3],Dflt_Srv_MAC[4],Dflt_Srv_MAC[5]);
    break;
  default:
    sprintf(s,"Invalid MAC type");
    break;
  }
  stmp=s;
  return stmp;
}

void VMEController::SetCR(int type, std::string CR) {
  union Tmp_U {
    unsigned char tc[4];
    unsigned short ts[2];
  } tu;

  if(CR.length() == 8){
    sscanf(CR.c_str(),"%02hhx%02hhx%02hhx%02hhx",tu.tc+2,tu.tc+3,tu.tc,tu.tc+1);
  }else{
    sscanf(CR.c_str(),"%02hhx%02hhx",tu.tc,tu.tc+1);
  }
  switch(type){
  case 0://Ethernet CR
    CR_ethernet=tu.ts[0];
    break;
  case 1://External FIFO CR
    CR_ext_fifo=tu.ts[0];
    break;
  case 2://Reset/Misc. CR
    CR_res_misc=tu.ts[0];
    break;
  case 3://VME CR
    CR_VME_low=tu.ts[0];
    CR_VME_hi=tu.ts[1];
    break;
  case 4://Bus Time Out CR
    CR_BUS_timeout=tu.ts[0];
    break;
  case 5://Bus Grant Time Out CR
    CR_BUS_grant=tu.ts[0];
    break;
  default:
    break;
  }
}

std::string VMEController::GetCR(int type) {
  char s[32];
  std::string stmp;

  switch(type){
  case 0://Ethernet CR
    sprintf(s,"%02X%02X",CR_ethernet&0xFF,(CR_ethernet>>8)&0xFF);
    break;
  case 1://External FIFO CR
    sprintf(s,"%02X%02X",CR_ext_fifo&0xFF,(CR_ext_fifo>>8)&0xFF);
    break;
  case 2://Reset/Misc. CR
    sprintf(s,"%02X%02X",CR_res_misc&0xFF,(CR_res_misc>>8)&0xFF);
    break;
  case 3://VME CR
    sprintf(s,"%02X%02X%02X%02X",CR_VME_hi&0xFF,(CR_VME_hi>>8)&0xFF,CR_VME_low&0xFF,(CR_VME_low>>8)&0xFF);
    break;
  case 4://Bus Time Out CR
    sprintf(s,"%02X%02X",CR_BUS_timeout&0xFF,(CR_BUS_timeout>>8)&0xFF);
    break;
  case 5://Bus Grant Time Out CR
    sprintf(s,"%02X%02X",CR_BUS_grant&0xFF,(CR_BUS_grant>>8)&0xFF);
    break;
  default:
    sprintf(s,"Invalid CR type");
    break;
  }
  stmp=s;
  return stmp;
}

std::string VMEController::GetWarn_On_Shtdwn() {
  std::string stmp;
  stmp="false";
  if(Warn_On_Shtdwn) stmp="true";
  return stmp;
}

std::string VMEController::GetPkt_On_Startup() {
  std::string stmp;
  stmp="false";
  if(Pkt_On_Startup) stmp="true";
  return stmp;
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

// do_vme() VME READ command never buffered

unsigned short int it[1]; 
unsigned short int tmp[1]={0x0000};
unsigned short int *ptr_rice;
unsigned long add_rice;
int itwr[2]={1,3};
int itrd[2]={2,2};
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

int VMEController::new_vme(char fcn, unsigned vme, unsigned short data, char *rcv, int when) 
{
  // in new_vme: both VME READ and WRITE commands can be buffered

  unsigned short int *ptr_rice;
  unsigned long add_rice;
  int itrd[2]={0,2};

  if(fcn<1 || fcn>3) return -1;
  add_rice=vmeadd | vme;
  ptr_rice=(unsigned short int *)add_rice;
  if(fcn==1 || fcn==2)
     return VME_controller(itrd[when]+fcn-1, ptr_rice, &data, rcv);
  else if(fcn==3) 
     sleep_vme((int)data);  
  return 0;
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

void VMEController::udelay(long int itim)
{
  if(itim>1000000)
  {
     ::usleep(itim);
  } else {
     // std::cout << "Udelay using nanosleep..." << itim << std::endl;
     struct timespec req= { 0, itim*1000};
     ::nanosleep(&req, NULL); 
  }
  return;

#if 0
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
#endif
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
  tmp_time=((time+1)/2)*125;   // in 16ns
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
   {   if(rbuf[0]==0x04 && loopcnt<1)
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


int VMEController::eth_read_timeout(int rd_tmo)
{
  if(ioctl(theSocket,SCHAR_READ_TIMEOUT,rd_tmo)){
    printf("Error in SCHAR_READ_TIMEOUT \n");
  }
  return 0;
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

        if(DEBUG) printf("EWI packet: %d\n", ptyp);
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
   if(useDCS_ && n_words>0) return -100;

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
     SetLife(false);
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
     SetLife(false);
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
  vcc_write_command(0xE6);
  std::cout << "Full reset of FIFO done." << std::endl;
  return;
}

void VMEController::set_VME_mode()
{
  vcc_write_command(0x02);
  std::cout << "Controller is in VME mode." << std::endl;
  return;
}

void VMEController::reset()
{
  vcc_write_command(0xF9);
  std::cout << "Controller's FPGA reloaded." << std::endl;
  return;
}

bool VMEController::SelfTest()
{ 
   init();

// To read back controller serial number
   int n=vcc_read_command(0x1E, 2, NULL);
   if(n==2)
   {
      clear_error();
      SetLife(true);
      return true;
   }
   else 
   {  SetLife(false);
      return  false;
   }
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
   int rt=VME_controller(2,ptr,tmp2,(char *)tmp);
   if(rt<0) return false;
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
  int actual_return;

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
    irdwr=1;  // delay always acts like a buffered WRITE command.
    if (data[1]) irdwr=3;  //send immediately for longer delays  
  } 

    /* check for overflow */
    LRG_read_flag=0;
    if(nwbuf>MAX_DATA && (irdwr==1 || irdwr==0)){
       if(DEBUG)
          printf("Jumbo packet limit reached: %d, forced sending\n", nwbuf);
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
    if(LRG_read_flag) 
      {  udelay(400);  //add extra delay for forced Jumbo packet out
         if(DEBUG) printf("Forced Jumbo packet delay 400 us\n");
      }
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
  actual_return=0; 
  if(LRG_read_flag==0 && LRG_read_pnt>0)
  {   memcpy(rcv, spebuff, LRG_read_pnt);
      actual_return += LRG_read_pnt/2;
  } 
  /* read back bytes from vme if needed */
 
  if(nread>0){
    clear_error();
READETH:
    nrbuf=nread;
    size=eth_read();
    if(size<10)
        { printf(" ERROR: no data read back from crate %02X, address %08lX\n", hw_dest_addr[5]&0xff, ptrt);
          error_count++;
          if(DEBUG) {
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
          }
          fflush(NULL);
          SetLife(false);
          nread=0;
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
      // if we can reach here, the controller still alive
      SetLife(true);
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
          if(return_type==0xff || return_type==0xfe || return_type==0xfd)
          {
             error_type=(r_datat[0]&0x3)*256+r_datat[1];
             int EWI=(r_datat[0]&0x0C)>>2;
             int Source_ID=(r_datat[0]&0xF0)>>4;
             if(EWI==2) error_count++;
             if(DEBUG || return_type==0xff || return_type==0xfe) 
             {
               printf("EWI packet %02X, %02X%02X, ", return_type&0xff, r_datat[0]&0xff, r_datat[1]&0xff);
               printf("type: %d, source: %d, code: %d from %d, crate %02X, address %08lX\n", EWI, Source_ID, error_type, irdwr, hw_dest_addr[5]&0xff, ptrt);
             }
          }
          else
          { 
             printf("Error: wrong return data type: %d from %d %08lX\n", return_type, irdwr, ptrt);
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
          if(DEBUG) printf("return data: %02X %02X\n", r_datat[2*i]&0xFF, r_datat[2*i+1]&0xFF);
       }
       actual_return += r_num;
    }
    nread=0;
  }

  // after normal READ/WRITE, always turn off LARGE_read
  if(LRG_read_flag==0) LRG_read_pnt=0;   

  }
  return actual_return;
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
  unsigned short tvalue=(val>>8)&0xff +((val&0xff)<<8);
  vcc_write_command(0x0F, 1, &tvalue);
  std::cout << "Write_Ethernet_CR to " << std::hex << val << std::dec << std::endl;
  return;
}

void VMEController::write_FIFO_CR(unsigned short int val)
{
  unsigned short tvalue=(val>>8)&0xff +((val&0xff)<<8);
  vcc_write_command(0x10, 1, &tvalue);
  std::cout << "Write_FIFO_CR to " << std::hex << val << std::dec << std::endl;
  return;
}

void VMEController::write_ResetMisc_CR(unsigned short int val)
{
  unsigned short tvalue=(val>>8)&0xff +((val&0xff)<<8);
  vcc_write_command(0x11, 1, &tvalue);
  std::cout << "Write_ResetMisc_CR to " << std::hex << val << std::dec << std::endl;
  return;
}

void VMEController::write_VME_CR(unsigned int val)
{
  unsigned short tvalue[2];
  tvalue[1]=(val>>8)&0xff +((val&0xff)<<8); 
  tvalue[0]=(val>>24)&0xff +(((val>>16)&0xff)<<8); 
  vcc_write_command(0x12, 2, tvalue);
  std::cout << "Write_VME_CR to " << std::hex << val << std::dec << std::endl;
  return;
}

void VMEController::write_BusTimeOut_CR(unsigned short int val)
{
  unsigned short tvalue=(val>>8)&0xff +((val&0xff)<<8);
  vcc_write_command(0x13, 1, &tvalue);
  std::cout << "Write_BusTimeOut_CR to " << std::hex << val << std::dec << std::endl;
  return;
}

void VMEController::write_BusGrantTimeOut_CR(unsigned short int val)
{

  unsigned short tvalue=(val>>8)&0xff +((val&0xff)<<8);
  vcc_write_command(0x14, 1, &tvalue);
  std::cout << "Write_BusGrantTimeOut_CR to " << std::hex << val << std::dec << std::endl;
  return;
}
                                                                                
int VMEController::LeftToRead()
{
 return ioctl(theSocket,SCHAR_INQR)&0xffff;
}


} // namespace emu::pc  
} // namespace emu  

