//-----------------------------------------------------------------------
// $Id: DDU.h,v 2.4 2006/01/13 10:06:53 mey Exp $
// $Log: DDU.h,v $
// Revision 2.4  2006/01/13 10:06:53  mey
// Got rid of virtuals
//
// Revision 2.3  2006/01/12 23:43:48  mey
// Update
//
// Revision 2.2  2006/01/12 12:27:50  mey
// UPdate
//
// Revision 2.1  2006/01/11 13:26:21  mey
// Update
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef DDU_h
#define DDU_h

#include "VMEModule.h"
#include <iostream>

class DDU: public VMEModule {
public:
  DDU(int crate, int slot);
  virtual ~DDU() {std::cout << "killing DDU" << std::endl;}

  /// same as DAQMB::end()
  void end();
  void ddu_end();
  void tcb_end();
  void tcb_trig();
  void tcb_burst();
  void tcb_bxr();
  void tcb_hrst();
  void dcntrl_reset();
  void ddu_dpromuser(void *data);
  void ddu_dpromid(void *data);
  void ddu_dfpgastat(void *data);
  void toggle_ddu_cal_l1a();
  void ddu_dlowfpgastat(void *data);
  void ddu_dhifpgastat(void *data);
  void ddu_rdfibererr(void *data);
  void ddu_rdfiberok(void *data);
  void ddu_rdkillfiber(void *data);
  void ddu_loadkillfiber(int regval, void *data);
  void ddu_rdcrcerr(void *data);
  void ddu_rdl1aerr(void *data);
  void ddu_rdxmiterr(void *data);
  void ddu_rdtimesterr(void *data);
  void ddu_rdtimeewerr(void *data);
  void ddu_rdtimeeaerr(void *data);
  void ddu_rddmberr(void *data);
  void ddu_rdtmberr(void *data);
  void ddu_rdlieerr(void *data);
  void ddu_rdliderr(void *data);
  void ddu_rdpaferr(void *data);
  void ddu_rdfferr(void *data);
  void ddu_rderareg(void *data);
  void ddu_rderbreg(void *data);
  void ddu_rdercreg(void *data);
  void ddu_rdostat(void *data);
  void ddu_rdempty(void *data);
  void ddu_rdstuckbuf(void *data);
  void ddu_rdscaler(void *data);
  void ddu_rdalcterr(void *data);
  void ddu_rdbxorbit(void *data);
  void ddu_loadbxorbit(int regval, void *data);
  void ddu_status_decode(int long code);
  void ddu_ostatus_decode(int long code);
  void ddu_era_decode(int long code);
  void ddu_erb_decode(int long code);
  void ddu_erc_decode(int long code);
  void ecntrl_reset();
  void ddu_epromuser(void *data);
  void ddu_epromid(void *data);
  void ddu_efpgastat(void *data);
  int load_hex(void *data);
  void tcb_rdpromuser(void *data);
  void tcb_rdpromid(void *data);
  void tcb_rdfpgastat(void *data);
  void tcb_reset();

  void pause(DEVTYPE dv, int microseconds = 10);
   
  //@@ it sends the same code to VME as DAQMB does.  Maybe can fix this later
  unsigned int boardType() const {return DMB_ENUM;} 
  //
  bool SelfTest() ;
  void init() ;
  void configure() ;
  //
  
private:
  //@@ NOT FILLED YET
  CCB * ccb;





};

#endif

