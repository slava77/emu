//----------------------------------------------------------------------
// $Id: VMEController.h,v 1.1 2005/08/31 18:26:19 gilmore Exp $
// $Log: VMEController.h,v $
// Revision 1.1  2005/08/31 18:26:19  gilmore
// First try for FED Crate includes.
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


/* ioctl's for schar. */
#define EMULATOR 0    // set to one for D360 emulator
#define SCHAR_IOCTL_BASE	0xbb
#define SCHAR_RESET     	_IO(SCHAR_IOCTL_BASE, 0)
#define SCHAR_END		_IOR(SCHAR_IOCTL_BASE, 1, 0)
#define SCHAR_BLOCKON		_IOR(SCHAR_IOCTL_BASE, 2, 0)
#define SCHAR_BLOCKOFF		_IOR(SCHAR_IOCTL_BASE, 3, 0)
#define SCHAR_DUMPON		_IOR(SCHAR_IOCTL_BASE, 4, 0)
#define SCHAR_DUMPOFF		_IOR(SCHAR_IOCTL_BASE, 5, 0)



class VMEController {
public:
  VMEController(string ipAddr, int port);
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
  void RestoreIdle();
  void InitJTAG(int port);
  void CloseJTAG();
  void send_last();
  void RestoreIdle_reset();
  void  scan_reset(int reg, const char *snd, int cnt2, char *rcv,int ird);
void  sleep_vme(const char *outbuf);   // in usecs (min 16 usec)
void  sleep_vme2(unsigned short int time); // time in usec
void  long_sleep_vme2(float time);   // time in usec
 void handshake_vme();
void flush_vme();
 void vmeser(const char *cmd,const char *snd,char *rcv);
 void vmepara(const char *cmd,const char *snd,char *rcv);
 void dcc(const char *cmd,char *rcv);
 void vme_adc(int ichp,int ichn,char *rcv);
 void vme_controller(int irdwr,unsigned short int *ptr,unsigned short int *data,char *rcv);
 void dump_outpacket(int nvme);
 int eth_read();
int eth_write();
 int eth_enableblock(void);
 int eth_disableblock(void);
 void get_macaddr();
 void sdly();
 void initDevice(int a);
  // EPROM reprogramming (EXPERTS ONLY !)
  void epromload(char *design,enum DEVTYPE devnum,char *downfile,int writ,char *cbrdnum);
  void Parse(char *buf,int *Count,char **Word);

 
  VMEModule* getTheCurrentModule();

private:
  int theSocket;
  string ipAddress_;
  int port_;
  VMEModule * theCurrentModule;
  sockaddr_in serv_addr;
  const ENDIAN indian;
  int idevo;
  int feuseo;
  int plev;
};

#endif

