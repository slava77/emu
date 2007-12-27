//----------------------------------------------------------------------
// $Id: VMEModule.h,v 3.6 2007/12/27 00:34:26 liu Exp $
// $Log: VMEModule.h,v $
// Revision 3.6  2007/12/27 00:34:26  liu
// update
//
// Revision 3.5  2007/11/26 14:38:21  liu
// reduce memory usage
//
// Revision 3.4  2007/09/12 16:01:36  liu
// remove log4cplus dependency
//
// Revision 3.3  2006/11/15 16:01:36  mey
// Cleaning up code
//
// Revision 3.2  2006/10/02 18:18:25  mey
// UPdate
//
// Revision 3.1  2006/08/08 19:23:08  mey
// Included Jtag sources
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.21  2006/07/18 14:10:33  mey
// Update
//
// Revision 2.20  2006/07/13 15:46:37  mey
// New Parser strurture
//
// Revision 2.19  2006/07/12 07:58:18  mey
// Update
//
// Revision 2.18  2006/07/11 09:31:11  mey
// Update
//
// Revision 2.17  2006/07/06 07:31:48  mey
// MPC firmware loading added
//
// Revision 2.16  2006/06/20 12:25:36  mey
// Update
//
// Revision 2.15  2006/06/16 13:05:24  mey
// Got rid of Compiler switches
//
// Revision 2.14  2006/05/10 23:59:47  liu
// Update for Production Controller with firmware 3.59
//
// Revision 2.13  2006/03/23 12:41:41  mey
// UPdate
//
// Revision 2.12  2006/03/10 13:13:12  mey
// Jinghua's changes
//
// Revision 2.9  2006/01/13 09:36:19  geurts
// changed SelfTest return type from void to bool for both controllers
//
// Revision 2.8  2006/01/12 23:44:11  mey
// Update
//
// Revision 2.7  2006/01/12 12:28:09  mey
// UPdate
//
// Revision 2.6  2006/01/11 13:55:03  mey
// Update
//
// Revision 2.5  2006/01/11 13:26:45  mey
// Update
//
// Revision 2.4  2005/12/02 18:12:08  mey
// get rid of D360
//
// Revision 2.3  2005/11/21 19:42:39  mey
// Update
//
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

#include <vector>
#include <iostream>
#include "JTAG_constants.h"

class VMEController;
class Crate;

class VMEModule
{
public:
  enum {MAXLINE = 70000};
  
  /// probably should make the user pass a Crate
  /// rather than a number
  /// automatically registers itself with the Crate
  VMEModule(Crate *, int );
  virtual ~VMEModule() {};
  int crate();
  int slot() const {return theSlot;}
  bool exist();
  
  /// these will only be called by the VMEController
  virtual void start();
  virtual void end();
  /// should automatically start().  Here's what you do if
  /// you want to end() by hand
  void endDevice();
  
  enum BOARDTYPE { DMB_ENUM=0, CCB_ENUM, TMB_ENUM, MPC_ENUM };
  virtual unsigned int boardType() const = 0;
  virtual bool SelfTest() = 0;
  virtual void init() = 0;
  virtual void configure() = 0;
  
  void Parse(char *buf,int *Count,char **Word);
  int  svfLoad(int*, const char *, int);
  
  VMEController* getTheController();
  
  inline void SetJtagSource(int source){JtagSource_ = source;}

protected:
  //
  int JtagSource_;
  //
  /// used for calls to do_vme
  enum FCN { VME_READ=1, VME_WRITE=2 };
  enum WRT { LATER, NOW };
  /// meant to replace things like rice_vme and tmb_vme
  void do_vme(char fcn, char vme,const char *snd,char *rcv, int wrt);
  
  /// these things are wrapped inot VMEController so the
  /// appropriate start() and end() routines are called
  /// maybe change these to HAL interface someday?
  void devdo(DEVTYPE dev,int ncmd,const  char *cmd,int nbuf,
	     const char *inbuf,char *outbuf,int irdsnd);
  void scan(int reg,const char *snd,int cnt2,char *rcv,int ird);
  void scan_reset(int reg,const char *snd,int cnt2,char *rcv,int ird);
  void RestoreIdle();
  void RestoreReset();
  void InitJTAG(int port);
  void CloseJTAG();
  void SetupJtag();
  void SendOutput(std::string,std::string = "INFO");
  
  Crate * theCrate_;
  VMEController * theController;
  int theSlot;
  
  // Liu--2007, Nov. 26
  // reduced array sizes from 4096 to 1024,
  // I even believe 16 should be enough.
  char sndbuf[1024];
  char rcvbuf[1024];
  char rcvbuf2[1024];
  char cmd[1024];
  //
};

#endif
