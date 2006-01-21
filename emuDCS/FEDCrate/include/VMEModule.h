//----------------------------------------------------------------------
// $Id: VMEModule.h,v 1.2 2006/01/21 20:20:14 gilmore Exp $
// $Log: VMEModule.h,v $
// Revision 1.2  2006/01/21 20:20:14  gilmore
// *** empty log message ***
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

   enum BOARDTYPE { DDU_ENUM=0, DCC_ENUM };
   virtual unsigned int boardType() const = 0;

   VMEController* getTheController();


  // IRQ VME Interrupts
static void *IRQ_Interrupt_Handler(void *threadarg);
void irq_pthread_start();
void irq_pthread_end();
void irq_pthread_info();
int irq_tester(int ival);
int CAEN_err();
void CAEN_err_reset();

protected:


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
void CAEN_close(void);
int CAEN_reset(void);
int CAEN_read(unsigned long Address,unsigned short int *data);
int CAEN_write(unsigned long Address,unsigned short int *data);

 void sdly();
 void initDevice(int a);
   /// used for calls to do_vme
   enum FCN { VME_READ=1, VME_WRITE=2 };
   enum WRT { LATER, NOW };
   Crate * theCrate;
   VMEController * theController;
   int theSlot;

      /// is this really needed?
  char sndbuf[4096];
  char rcvbuf[4096];
  char rcvbuf2[4096];
  char cmd[4096];

};

#endif

