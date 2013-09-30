//----------------------------------------------------------------------
// $Id: VMEModule.h,v 1.13 2013/01/14 03:44:19 liu Exp $
// $Log: VMEModule.h,v $
// Revision 1.13  2013/01/14 03:44:19  liu
// increase array size for DCFEB
//
// Revision 1.12  2012/10/16 22:39:07  liu
// read & write FPGA internal registers
//
// Revision 1.11  2012/10/11 21:26:45  liu
// add DCFEB firmware download and readback
//
// Revision 1.10  2012/09/30 21:19:42  liu
// update for ME11 new electronics
//
// Revision 1.9  2012/09/05 21:39:11  liu
// remove ODMB class
//
// Revision 1.7  2012/06/20 08:45:00  kkotov
//
// New faster DMB/CFEB EPROM readback routines
//
// Revision 1.6  2011/11/04 20:05:52  liu
// change svfLoad interface
//
// Revision 1.5  2011/10/28 18:13:37  liu
// add DDU class
//
// Revision 1.4  2011/07/01 03:19:38  liu
// new JTAG functions
//
// Revision 1.3  2011/02/23 11:42:05  liu
// updated svfLoad, added PROM read back functions
//
// Revision 1.2  2009/03/25 10:19:41  liu
// move header files to include/emu/pc
//
// Revision 1.1  2009/03/25 10:07:43  liu
// move header files to include/emu/pc
//
// Revision 3.11  2008/08/13 11:30:53  geurts
// introduce emu::pc:: namespaces
// remove any occurences of "using namespace" and make std:: references explicit
//
// Revision 3.10  2008/05/28 10:35:30  liu
// DMB counters in jumbo packet
//
// Revision 3.9  2008/04/22 08:32:35  liu
// Ben's Crate controller utilities
//
// Revision 3.8  2008/04/02 13:42:50  liu
// add f/w downloading verify for CCB & MPC
//
// Revision 3.7  2008/02/18 12:08:48  liu
// new functions for monitoring
//
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
#include "emu/pc/JTAG_constants.h"


namespace emu {
  namespace pc {

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
  inline Crate* getCrate(){ return theCrate_; };
  int slot() const {return theSlot;}
  bool exist();
  
  /// these will only be called by the VMEController
  virtual void start();
  virtual void end();
  /// should automatically start().  Here's what you do if
  /// you want to end() by hand
  void endDevice();
  
  enum BOARDTYPE { DMB_ENUM=0, CCB_ENUM, TMB_ENUM, MPC_ENUM, VMECC_ENUM, DDU_ENUM };
  virtual unsigned int boardType() const = 0;
  virtual bool SelfTest() = 0;
  virtual void init() = 0;
  virtual void configure() = 0;
  
  void Parse(char *buf,int *Count,char **Word);
  int  svfLoad(int, const char *, int, int verify=0);
  
  VMEController* getTheController();
  
  inline void SetJtagSource(int source){JtagSource_ = source;}

  int new_vme(char fcn, unsigned vme, 
                       unsigned short data, char *rcv, int when);
  void write_later(unsigned  address, unsigned short data); 
  void  read_later(unsigned  address);
  int    write_now(unsigned  address, unsigned short data, char *rdbuf);
  int     read_now(unsigned  address, char *rdbuf);
  void   vme_delay(unsigned short useconds);

  char wbuf[9000];
  int nwbuf;
  char rbuf[9000];
  int nrbuf;
  int eth_write();
  int eth_read();
  int eth_read_previous();
  int eth_read_timeout(int rd_tmo);
  int LeftToRead();
  int read_prom(const char *vfyfile, const char *mcsfile );
  void write_mcs(char *buf, int nbytes, FILE *outf);
  int read_mcs(char *binbuf, FILE *finp);

protected:
  //
  int JtagSource_;
  //
  /// used for calls to do_vme
  enum FCN { VME_READ=1, VME_WRITE=2, VME_DELAY=3 };
  enum WRT { LATER, NOW };
  /// meant to replace things like rice_vme and tmb_vme
  void do_vme(char fcn, char vme,const char *snd,char *rcv, int wrt);
  
  /// these things are wrapped into VMEController so the
  /// appropriate start() and end() routines are called
  /// maybe change these to HAL interface someday?
  void devdo(DEVTYPE dev,int ncmd,const  char *cmd,int nbuf,
	     const char *inbuf,char *outbuf,int irdsnd);
  void new_devdo(DEVTYPE dev,int ncmd,const  char *cmd,int nbuf,
	     const char *inbuf,char *outbuf,int irdsnd);
  void scan(int reg,const char *snd,int cnt2,char *rcv,int ird);
	void scan_reset(int reg,const char *snd,int cnt2,char *rcv,int ird);
	void scan_reset_headtail(int reg,const char *snd,int cnt,char *rcv,int headtail,int ird);
	void scan_dmb_headtail(int reg,const char *snd,int cnt,char *rcv,int ird,int headtail,int when);
	void RestoreIdle();
  void RestoreReset();
  void InitJTAG(int port);
  void CloseJTAG();
  void SetupJtag();
  void SendOutput(std::string,std::string = "INFO");
  void shift_state(int cnt, int mask);
  void shift_bits(int reg,const char *snd, int cnt, char *rcv,int ird);
  void scan_word(int reg,const char *snd, int cnt, char *rcv,int ird);

  // Liu--2012, Sept.
  // new Jtag routines to bypass VMEController_jtag 
  void Jtag_Ohio(int dev, int reg, const char *snd, int cnt, char *rcv, int ird, int when); 
  void Jtag_Lite(int dev, int reg, const char *snd, int cnt, char *rcv, int ird, int when);
  void Jtag_Test(int dev, int reg, const char *snd, int cnt, char *rcv, int ird, int when);
  void udelay(long int usec);
  unsigned shuffle32(unsigned value);
  void shuffle57(void *data);
  char* add_headtail(char *datain, int osize, int head, int tail);
  char* cut_headtail(char *datain, int osize, int head, int tail);

  Crate * theCrate_;
  VMEController * theController;
  int theSlot;
  
  // Liu--2013, Jan. 11
  // Increase array size to 8192 to accomodate BPI interface of DCFEB and ODMB.
  //
  char sndbuf[8192];
  char rcvbuf[8192];
  char rcvbuf2[8192];
  char cmd[8192];
  //
  // for PROM read back
  FILE *bitfile;
  char *bitstream;
  int bitbufindex;
};
  } // namespace emu::pc
} // namespace emu

#endif
