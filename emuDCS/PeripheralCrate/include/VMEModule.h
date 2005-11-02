
#ifdef D360

//----------------------------------------------------------------------
// $Id: VMEModule.h,v 2.2 2005/11/02 16:17:08 mey Exp $
// $Log: VMEModule.h,v $
// Revision 2.2  2005/11/02 16:17:08  mey
// Update for new controller
//
// Revision 2.1  2005/08/11 08:13:59  mey
// Update
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
// Revision 1.16  2004/08/05 04:10:13  tfcvs
// Update
//
// Revision 1.15  2004/07/22 18:52:38  tfcvs
// added accessor functions for DCS integration
//
//----------------------------------------------------------------------
#ifndef VMEModule_h
#define VMEModule_h
#include "Crate.h"
#include "JTAG_constants.h"

class VMEController;

class VMEModule
{
public:
   enum {MAXLINE = 70000};

   /// probably should make the user pass a Crate
   /// rather than a number
   /// automatically registers itself with the Crate
   VMEModule(int crate, int slot);
   virtual ~VMEModule() {};
   int crate() const {return theCrate->number();}
   int slot() const {return theSlot;}

   /// these will only be called by the VMEController
   virtual void start();
   virtual void end();
   /// should automatically start().  Here's what you do if
   /// you want to end() by hand
   void endDevice();

   enum BOARDTYPE { DMB_ENUM=0, CCB_ENUM, TMB_ENUM, MPC_ENUM };
   virtual unsigned int boardType() const = 0;

   VMEController* getTheController();

protected:
   /// used for calls to do_vme
   enum FCN { VME_READ=1, VME_WRITE=2 };
   enum WRT { LATER, NOW };
   /// meant to replace things like rice_vme and tmb_vme
   void do_vme(char fcn, char vme, const char *snd,char *rcv, int wrt);

   /// these things are wrapped inot VMEController so the
   /// appropriate start() and end() routines are called
   /// maybe change these to HAL interface someday?
   int readn(char *line);
   int writen(const register char *ptr, register int nbytes);
   int writenn(const char *ptr,int nbytes);
   int readline(char * line, int maxbytes);
   void devdo(DEVTYPE dev,int ncmd, const char *cmd,int nbuf,char *inbuf,char *outbuf,int irdsnd);
   void scan(int reg,const char *snd,int cnt,char *rcv,int ird);
   void InitJTAG(int port);
   void CloseJTAG();

   Crate * theCrate;
   VMEController * theController;
   int theSlot;

      /// is this really needed?
  char sndbuf[4096];
  char rcvbuf[4096];
  char rcvbuf2[4096];
  char cmd[4096];

  // used by do_vme
  enum {TDATASIZE = 70000};
  char Tdata[TDATASIZE];
  int cnt;

};

#endif

#endif

#ifdef OSUcc

//----------------------------------------------------------------------
// $Id: VMEModule.h,v 2.2 2005/11/02 16:17:08 mey Exp $
// $Log: VMEModule.h,v $
// Revision 2.2  2005/11/02 16:17:08  mey
// Update for new controller
//
// Revision 1.16  2004/08/05 04:10:13  tfcvs
// Update
//
// Revision 1.15  2004/07/22 18:52:38  tfcvs
// added accessor functions for DCS integration
//
//
//----------------------------------------------------------------------
#ifndef VMEModule_h
#define VMEModule_h

#include "Crate.h"
class VMEController;
#include "JTAG_constants.h"

class VMEModule
{
public:
   enum {MAXLINE = 70000};

   /// probably should make the user pass a Crate
   /// rather than a number
   /// automatically registers itself with the Crate
   VMEModule(int crate, int slot);
   virtual ~VMEModule() {};
   int crate() const {return theCrate->number();}
   int slot() const {return theSlot;}

   /// these will only be called by the VMEController
   virtual void start();
   virtual void end();
   /// should automatically start().  Here's what you do if
   /// you want to end() by hand
   void endDevice();

   enum BOARDTYPE { DMB_ENUM=0, CCB_ENUM, TMB_ENUM, MPC_ENUM };
   virtual unsigned int boardType() const = 0;

   VMEController* getTheController();


protected:
   /// used for calls to do_vme
   enum FCN { VME_READ=1, VME_WRITE=2 };
   enum WRT { LATER, NOW };
   /// meant to replace things like rice_vme and tmb_vme
   void do_vme(char fcn, char vme,const char *snd,char *rcv, int wrt);

   /// these things are wrapped inot VMEController so the
   /// appropriate start() and end() routines are called
   /// maybe change these to HAL interface someday?
   void devdo(DEVTYPE dev,int ncmd,const  char *cmd,int nbuf,const char *inbuf,char *outbuf,int irdsnd);
   void scan(int reg,const char *snd,int cnt2,char *rcv,int ird);
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
 void ccb_sleep_vme(const char *outbuf);
void  sleep_vme2(unsigned short int time); // time in usec
void  long_sleep_vme2(float time);   // time in usec
 void handshake_vme();
 void ccb_handshake_vme();
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
 void scan_alct(int reg,const char *snd, int cnt, char *rcv,int ird);

   Crate * theCrate;
   VMEController * theController;
   int theSlot;
   unsigned long vmebase;

      /// is this really needed?
  char sndbuf[4096];
  char rcvbuf[4096];
  char rcvbuf2[4096];
  char cmd[4096];

};

#endif

#endif
