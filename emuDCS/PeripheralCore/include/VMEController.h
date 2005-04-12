//----------------------------------------------------------------------
// $Id: VMEController.h,v 2.0 2005/04/12 08:07:03 geurts Exp $
// $Log: VMEController.h,v $
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

