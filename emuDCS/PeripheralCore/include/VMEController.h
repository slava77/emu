
#ifdef D360

//----------------------------------------------------------------------
// $Id: VMEController.h,v 2.4 2005/11/30 16:25:42 mey Exp $
// $Log: VMEController.h,v $
// Revision 2.4  2005/11/30 16:25:42  mey
// Update
//
// Revision 2.3  2005/11/30 12:59:43  mey
// DMB firmware loading
//
// Revision 2.2  2005/11/21 15:47:38  mey
// Update
//
// Revision 2.1  2005/11/02 16:17:01  mey
// Update for new controller
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
// Revision 1.14  2004/07/22 18:52:38  tfcvs
// added accessor functions for DCS integration
//
//----------------------------------------------------------------------
#ifndef VMEController_h
#define VMEController_h
#include <iostream>
#include "JTAG_constants.h"
#include <string>
#include <arpa/inet.h>

class VMEModule;
class Crate;

class VMEController {
public:
  VMEController(int crate, std::string ipAddr, int port);
  ~VMEController();

  enum ENDIAN {SWAP, NOSWAP};
  enum {MAXLINE = 70000};
  

  int openSocket();
  void closeSocket();

  std::string ipAddress() const {return ipAddress_;}
  int port() const {return port_;}

  /// if not current modules, it stops current and starts new
  /// this base routine sends a signal consisting of the
  /// university and slot
  void start(VMEModule * module);
  /// ends whatever module is current
  void end();

  int readn(char *line);
  int writen(const register char *ptr, register int nbytes);
  int writenn(const char *ptr,int nbytes);
  int readline(char * line, int maxbytes);

  /// JTAG stuff

  int udelay(long int itim);
  void devdo(DEVTYPE dev,int ncmd, const char *cmd,int nbuf,char *inbuf,char *outbuf,int irdsnd);
  void scan(int reg,const char *snd,int cnt,char *rcv,int ird);
  void d360sleep();
  void RestoreIdle();

  /// sets plev=2, meaning you're going to talk to JTAG
  void goToScanLevel();
  void initDevice(int idev, int feuse);

  // brings plev up to 1 by sending terminate signal
  void send_last();
  /// just sets plev to 2, signalling end of jtag
  void release_plev() {plev = 1;}

  void InitJTAG(int port);
  void CloseJTAG();

  VMEModule* getTheCurrentModule();

private:
  int theSocket;
  std::string ipAddress_;
  int port_;
  int crate_;
  VMEModule * theCurrentModule;
  sockaddr_in serv_addr;
  const ENDIAN indian;

  int max_buff;
  int tot_buff;
  /// previous fe used
  int feuseo;
  /// JTAG level.  When plev=2, we're in JTAG mode.
  /// if it's not reset when we're done with JTAG,
  /// the Dynatem will hang.
  int plev;
  int idevo;

  // needs to stay persistent between calls to scan()
  char Tdata[70000];
  int  Tbytes;

};

#endif

#endif

#ifdef OSUcc

//----------------------------------------------------------------------
// $Id: VMEController.h,v 2.4 2005/11/30 16:25:42 mey Exp $
// $Log: VMEController.h,v $
// Revision 2.4  2005/11/30 16:25:42  mey
// Update
//
// Revision 2.3  2005/11/30 12:59:43  mey
// DMB firmware loading
//
// Revision 2.2  2005/11/21 15:47:38  mey
// Update
//
// Revision 2.1  2005/11/02 16:17:01  mey
// Update for new controller
//
// Revision 1.14  2004/07/22 18:52:38  tfcvs
// added accessor functions for DCS integration
//
//
//----------------------------------------------------------------------
#ifndef VMEController_h
#define VMEController_h

using namespace std;
#include <vector>
#include <iostream>
class VMEModule;
class Crate;
#include "JTAG_constants.h"
#include <string>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <time.h>
#include <sys/time.h>


class VMEController {
public:
  VMEController(int crate, string ipAddr, int port);
  ~VMEController();

  enum ENDIAN {SWAP, NOSWAP};
  enum {MAXLINE = 70000};
  

  int openSocket();
  void closeSocket();

  string ipAddress() const {return ipAddress_;}
  int port() const {return port_;}

  /// if not current modules, it stops current and starts new
  /// this base routine sends a signal consisting of the
  /// university and slot
  void start(VMEModule * module);
  /// ends whatever module is current
  void end();

 
  /// JTAG stuff
  void devdo(DEVTYPE dev,int ncmd,const char *cmd,int nbuf,const char *inbuf,char *outbuf,int irdsnd);
  void scan(int reg,const char *snd,int cnt2,char *rcv,int ird);
  void initDevice(int idev, int feuse);
  void RestoreIdle();
  void InitJTAG(int port);
  void CloseJTAG();
  void send_last();
  void RestoreIdle_reset();
  void load_cdac(const char *snd);
  void vme_adc(int ichp,int ichn,char *rcv);
  void buckflash(const char *cmd,const char *inbuf,char *rcv);
  void lowvolt(int ichp,int ichn,char *rcv);
  void  scan_reset(int reg, const char *snd, int cnt2, char *rcv,int ird);
  void  sleep_vme(const char *outbuf);   // in usecs (min 16 usec)
  void  sleep_vme2(unsigned short int time); // time in usec
  void  long_sleep_vme2(float time);   // time in usec
  void handshake_vme();
  void flush_vme();
  void  daqmb_fifo(int irdwr,int ififo,int nbyte,unsigned short int *buf,unsigned char *rcv);
  void vme_controller(int irdwr,unsigned short int *ptr,unsigned short int *data,char *rcv);
  void dump_outpacket(int nvme);
  int eth_reset(void);
  int eth_read();
  int eth_write();
  void mrst_ff();
  void set_VME_mode();
  void get_macaddr();
  void setuse();
  void goToScanLevel();
  void release_plev();
  void sdly();
  void RestoreIdle_alct();
  void scan_alct(int reg, const char *snd, int cnt, char *rcv,int ird);
  
  VMEModule* getTheCurrentModule();

private:
  bool usedelay_;
  int theSocket;
  string ipAddress_;
  int port_;
  int crate_;
  VMEModule * theCurrentModule;
  sockaddr_in serv_addr;
  const ENDIAN indian;

  unsigned char hw_source_addr[6];
  unsigned char hw_dest_addr[6];
  struct ethhdr ether_header; 

  char wbuf[9000];
  int nwbuf;
  char rbuf[9000];
  int nrbuf;

  int max_buff;
  int tot_buff;
  /// previous fe used
  int feuseo;
  /// JTAG level.  When plev=2, we're in JTAG mode.
  /// if it's not reset when we're done with JTAG,
  /// the Dynatem will hang.
  int plev;
  int idevo;
  int board; //board type
  unsigned long vmeadd; // current VME base address for the module
  unsigned short int pvme; // value for ALCT JTAG register (0x70000)
  int feuse;
  int ucla_ldev;
  long int packet_delay;
  float fpacket_delay;
  int packet_delay_flg;
  float DELAY2;
  float DELAY3;

};

#endif


#endif




