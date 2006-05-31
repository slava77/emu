//-----------------------------------------------------------------------
// $Id: DAQMB.cc,v 2.60 2006/05/31 15:58:46 mey Exp $
// $Log: DAQMB.cc,v $
// Revision 2.60  2006/05/31 15:58:46  mey
// Update
//
// Revision 2.59  2006/05/29 08:09:20  mey
// Update
//
// Revision 2.58  2006/05/22 16:32:40  mey
// UPdate
//
// Revision 2.57  2006/05/22 16:30:59  mey
// UPdate
//
// Revision 2.56  2006/05/22 16:28:10  mey
// UPdate
//
// Revision 2.55  2006/05/22 16:21:43  mey
// UPdate
//
// Revision 2.54  2006/05/22 16:20:38  mey
// UPdate
//
// Revision 2.53  2006/05/19 15:13:32  mey
// UPDate
//
// Revision 2.52  2006/05/19 13:35:03  mey
// Update
//
// Revision 2.51  2006/05/19 13:24:42  mey
// Update
//
// Revision 2.50  2006/05/19 13:22:36  mey
// Update
//
// Revision 2.49  2006/05/19 13:19:11  mey
// Update
//
// Revision 2.48  2006/05/19 13:17:16  mey
// Update
//
// Revision 2.47  2006/05/12 11:38:39  mey
// Update
//
// Revision 2.46  2006/05/12 11:09:26  mey
// Update
//
// Revision 2.45  2006/05/12 11:03:12  mey
// Update
//
// Revision 2.44  2006/05/12 10:54:13  mey
// Update
//
// Revision 2.43  2006/05/12 10:53:20  mey
// Update
//
// Revision 2.42  2006/05/12 10:39:44  mey
// Update
//
// Revision 2.41  2006/05/12 08:03:06  mey
// Update
//
// Revision 2.40  2006/05/11 13:14:33  mey
// Update
//
// Revision 2.39  2006/05/11 11:00:26  mey
// Update
//
// Revision 2.38  2006/05/10 10:07:56  mey
// Update
//
// Revision 2.37  2006/05/02 21:23:27  mey
// Update
//
// Revision 2.36  2006/05/02 21:12:43  mey
// UPdate
//
// Revision 2.35  2006/05/02 21:04:28  mey
// UPdate
//
// Revision 2.34  2006/04/24 14:57:21  mey
// Update
//
// Revision 2.33  2006/03/31 09:22:05  mey
// Update
//
// Revision 2.32  2006/03/15 13:50:11  mey
// Update
//
// Revision 2.31  2006/02/25 11:25:11  mey
// UPdate
//
// Revision 2.30  2006/02/06 14:06:55  mey
// Fixed stream
//
// Revision 2.29  2006/01/30 09:29:53  mey
// Update
//
// Revision 2.28  2006/01/25 19:25:06  mey
// Update
//
// Revision 2.27  2006/01/19 10:03:46  mey
// Update
//
// Revision 2.26  2006/01/12 23:44:26  mey
// Update
//
// Revision 2.25  2006/01/12 12:28:41  mey
// UPdate
//
// Revision 2.24  2006/01/11 13:47:32  mey
// Update
//
// Revision 2.23  2006/01/10 23:32:40  mey
// Update
//
// Revision 2.22  2005/12/16 17:49:47  mey
// Update
//
// Revision 2.21  2005/12/15 14:23:34  mey
// Update
//
// Revision 2.20  2005/12/06 17:58:26  mey
// Update
//
// Revision 2.19  2005/12/05 13:20:58  mey
// Got rid of flush_vme
//
// Revision 2.18  2005/11/30 16:49:26  mey
// Bug DMB firmware load
//
// Revision 2.17  2005/11/30 12:59:52  mey
// DMB firmware loading
//
// Revision 2.16  2005/11/25 23:42:33  mey
// Update
//
// Revision 2.15  2005/11/22 15:14:53  mey
// Update
//
// Revision 2.14  2005/11/21 15:48:06  mey
// Update
//
// Revision 2.13  2005/11/08 06:50:12  mey
// Update
//
// Revision 2.12  2005/11/02 10:01:05  mey
// Update
//
// Revision 2.11  2005/10/04 16:02:07  mey
// Added Output
//
// Revision 2.10  2005/09/28 16:57:22  mey
// Update Tests
//
// Revision 2.9  2005/09/26 07:27:14  mey
// Added BXN toogle routine
//
// Revision 2.8  2005/09/19 07:13:50  mey
// Update
//
// Revision 2.7  2005/09/13 14:46:40  mey
// Get DMB crate id; and DCS
//
// Revision 2.6  2005/09/07 16:18:16  mey
// DMB timing routines
//
// Revision 2.5  2005/09/07 13:54:39  mey
// Included new timing routines from Jianhui
//
// Revision 2.4  2005/09/06 12:11:51  mey
// Update
//
// Revision 2.3  2005/08/31 15:12:58  mey
// Bug fixes, updates and new routine for timing in DMB
//
// Revision 2.2  2005/08/11 08:13:04  mey
// Update
//
// Revision 2.1  2005/06/06 10:05:51  geurts
// calibration-related updates by Alex Tumanov and Jason Gilmore
//
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
/* New motherboard VTX2 not VTX  so MCTRL,6  not MCTRL,5 */
#include "DAQMB.h"
#include "VMEController.h"
#include <fstream>
#include <stdio.h>
#include <cmath>
#include <unistd.h>
#include <iomanip>
#include "geom.h"

using namespace std;

#ifndef debugV //silent mode
#define PRINT(x) 
#define PRINTSTRING(x)  
#else //verbose mode
#define PRINT(x) std::cout << #x << ":\t" << x << std::endl; 
#define PRINTSTRING(x) std::cout << #x << std::endl; 
#endif


// declarations
void Parse(char *buf,int *Count,char **Word);
void shuffle(char *a,char *b);


// static init
const int DAQMB::layers[6] = {2,0,4,5,3,1};
const int DAQMB::chip_use[5][6] = {
  {1,1,1,1,1,1},
  {1,1,1,1,1,1},
  {1,1,1,1,1,1},
  {1,1,1,1,1,1},
  {1,1,1,1,1,1}
};

const int DAQMB::nchips[5] = {6,6,6,6,6};

DAQMB::DAQMB(int newcrate,int newslot):
  VMEModule(newcrate, newslot),
  feb_dav_delay_(24),tmb_dav_delay_(24), 
  push_dav_delay_(31), l1acc_dav_delay_(24), ALCT_dav_delay_(0),
  calibration_LCT_delay_(8), calibration_l1acc_delay_(22),
  pulse_delay_(15), inject_delay_(15),
  pul_dac_set_(1.0), inj_dac_set_(1.0),
  set_comp_thresh_(0.06), feb_clock_delay_(0),
  comp_timing_(2), comp_mode_(2), pre_block_end_(7),
  l1a_lct_counter_(-1), cfeb_dav_counter_(-1), 
  tmb_dav_counter_(-1), alct_dav_counter_(-1), cable_delay_(0), 
  crate_id_(0xfe), toogle_bxn_(1)
{
  MyOutput_ = &std::cout ;
  cfebs_.clear();
  std::cout << "DMB: crate=" << this->crate() << " slot=" << this->slot() << std::endl;
  for(int i=0;i<20;i++) TestStatus_[i]=-1;
  //
  //std::cout << "&&&&&&&&&&&&&&&&&& CrateId " << crate_id_ << std::endl ;
  //
}

DAQMB::DAQMB(int newcrate,int newslot,  int newcfeb):
  VMEModule(newcrate, newslot)
{
  MyOutput_ = &std::cout ;
  cfebs_.push_back(CFEB(newcfeb));
  std::cout << "DMB: crate=" << this->crate() << " slot=" << this->slot() << std::endl; 
  for(int i=0;i<20;i++) TestStatus_[i]=-1;
}

DAQMB::DAQMB(int newcrate,int newslot, const std::vector<CFEB> & cfebs):
  VMEModule(newcrate, newslot),
  cfebs_(cfebs)
{
  MyOutput_ = &std::cout ;
  std::cout << "DMB: crate=" << this->crate() << " slot=" << this->slot() << std::endl; 
  for(int i=0;i<20;i++) TestStatus_[i]=-1;
}


DAQMB::~DAQMB() {
  std::cout << "Killing DAQMB" << std::endl;
}


void DAQMB::end()
{
  //std::cout << "calling DAQMB::end" << std::endl;
  theController->send_last();
  VMEModule::end();
}

void DAQMB::init(){
}

bool DAQMB::SelfTest(){
  return 0;
}

void DAQMB::configure() {
  //
  (*MyOutput_) << std::endl;
  (*MyOutput_) << "CFEB size="<<cfebs_.size()<<std::endl;
  (*MyOutput_) << "DAQMB: configure() for crate " << this->crate() << " slot " << this->slot() << std::endl;
  int cal_delay_bits = (calibration_LCT_delay_ & 0xF)
     | (calibration_l1acc_delay_ & 0x1F) << 4
      | (pulse_delay_ & 0x1F) << 9
      | (inject_delay_ & 0x1F) << 14;
   (*MyOutput_) << "DAQMB:configure: caldelay " << std::hex << cal_delay_bits << std::dec << std::endl;
   setcaldelay(cal_delay_bits);
   //
   int dav_delay_bits = (feb_dav_delay_    & 0x1F)
      | (tmb_dav_delay_ & 0X1F) << 5
      | (push_dav_delay_   & 0x1F) << 10
      | (l1acc_dav_delay_  & 0x3F) << 15
      | (ALCT_dav_delay_   & 0x1F) << 21;
   (*MyOutput_) << "doing setdavdelay " << dav_delay_bits << std::endl;
   setdavdelay(dav_delay_bits);
   //
   (*MyOutput_) << "doing fxpreblkend " << pre_block_end_ << std::endl;
   fxpreblkend(pre_block_end_);
   calctrl_fifomrst();
   //
   int comp_mode_bits = (comp_mode_ & 3) | ((comp_timing_ & 7) << 2);
   (*MyOutput_) << "doing set_comp_mode " << comp_mode_bits << std::endl;
   (*MyOutput_) << comp_mode_ << " " << comp_timing_ << std::endl;
   set_comp_mode(comp_mode_bits);
   //
   //fg where did these lines come from ...????
   //fg usleep(100);
   //fg set_comp_mode(comp_mode_bits);
   //
   (*MyOutput_) << "doing set_comp_thresh " << set_comp_thresh_ << std::endl;
   set_comp_thresh(set_comp_thresh_);
   (*MyOutput_) << "doing preamp_initx() " << std::endl;
   preamp_initx();
   (*MyOutput_) << "doing set_cal_dac " << inj_dac_set_ << " " 
	<<  pul_dac_set_ << std::endl;
   set_cal_dac(inj_dac_set_, pul_dac_set_);
   load_strip(); //enable..disable CFEBs
   //
   (*MyOutput_) << "Set cable delay " << cable_delay_ << std::endl ;
   setcbldly(cable_delay_);
   //
   //(*MyOutput_) << "&&&&&&&&&&&&&&&& Set crate id " << crate_id_ << std::endl ;
   setcrateid(crate_id_);
   //
   (*MyOutput_) << "Toogle bxn " << crate_id_ << std::endl ;
   if (toogle_bxn_) ToogleBXN();
   //
   // As suggested by Valery Sitnik: switch all LVs on (computer-controlled)
   // (*MyOutput_) << "DAQMB: switching on LVs on LVMB" << endl; 
   lowv_onoff(0x3f);
}
//
void DAQMB::load_strip() {
  cmd[0]=VTX2_USR1;
  sndbuf[0]=LOAD_STR;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  int j=0;
  //
  //for(i=0;i<5;i++){if(dp->iuse[i]==1)j=j+k;k=k*2;}
  //
  for(unsigned i = 0; i < cfebs_.size(); ++i) {
    j += (int) pow(2.0, cfebs_[i].number()); 
  }
  printf("Trigger Strips set to %02X LOAD_STR is %d\n",j,LOAD_STR);
  sndbuf[0]=j;
  devdo(MCTRL,6,cmd,5,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
}
//
void DAQMB::setcrateid(int dword)
{
  cmd[0]=VTX2_USR1;
  sndbuf[0]=CRATE_ID;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  sndbuf[0]=dword&0XFF; 
  sndbuf[1]=(dword>>8)&0xFF;
  sndbuf[2]=(dword>>16)&0xFF;
  devdo(MCTRL,6,cmd,7,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  //
  // Update
  //
  cmd[0]=VTX_USR1;
  sndbuf[0]=22;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
}
//
void DAQMB::setfebdelay(int dword)
{
  cmd[0]=VTX2_USR1;
  sndbuf[0]=FEB_DELAY;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  sndbuf[0]=dword&0XFF; 
  devdo(MCTRL,6,cmd,5,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=FEB_DELAY;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  //
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  (*MyOutput_) << "setfebdelay to " << dword << std::endl;
  //
  // Update
  //
  cmd[0]=VTX2_USR1;
  sndbuf[0]=23;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
}

void DAQMB::setcaldelay(int dword)
{
  //
  cmd[0]=VTX2_USR1;
  sndbuf[0]=CAL_DELAY;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  sndbuf[0]=dword&0XFF; 
  sndbuf[1]=(dword>>8)&0xFF;
  sndbuf[2]=(dword>>16)&0xFF;
  devdo(MCTRL,6,cmd,19,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2); 
  (*MyOutput_) << "caldelay was set to " << std::hex << dword <<std::dec << std::endl;
  //
}

void DAQMB::setdavdelay(int dword)
{
  cmd[0]=VTX2_USR1;
  sndbuf[0]=TRG_DAV_DELAY;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  sndbuf[0]=dword&0XFF; 
  sndbuf[1]=(dword>>8)&0xFF;
  sndbuf[2]=(dword>>16)&0xFF;
  sndbuf[3]=(dword>>24)&0x03;
  devdo(MCTRL,6,cmd,26,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  (*MyOutput_) << "set dav delay to " << dword << std::endl;
}

void DAQMB::fxpreblkend(int dword)
{
  for(unsigned icfeb = 0; icfeb < cfebs_.size(); ++icfeb) {
    DEVTYPE dv = cfebs_[icfeb].scamDevice();
    cmd[0]=VTX_USR1;
    sndbuf[0]=PREBLKEND;
    devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
    cmd[0]=VTX_USR2;
    //  default preblkend is state 5
    sndbuf[0]=dword&0x0F; 
    devdo(dv,5,cmd,4,sndbuf,rcvbuf,0);
    cmd[0]=VTX_BYPASS;
    sndbuf[0]=0;
    devdo(dv,5,cmd,0,sndbuf,rcvbuf,2);
  }
}

void DAQMB::calctrl_fifomrst()
{
  cmd[0]=VTX2_USR1;
  sndbuf[0]=CAL_FIFOMRST;
  (*MyOutput_) << " CAL_FIFOMRST " << std::hex << (sndbuf[0]&0xff) << std::dec << std::endl;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  for(int i=0;i<1000000;i++); /* wait 15 msec (1 loop = ~15ns) */
  /*  sleep(1); 
  set user mode 1 and put LCA into normal mode
*/
  cmd[0]=VTX2_USR1;
  sndbuf[0]=NORM_MODE;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  (*MyOutput_) << " FIFOMRST reset done " << std::endl;
}

void DAQMB::calctrl_global()
{

  cmd[0]=VTX2_USR1;
  sndbuf[0]=GLOBAL_RST;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  usleep(10000);
}


void DAQMB::restoreCFEBIdle() {
  for(unsigned icfeb = 0; icfeb < cfebs_.size(); ++icfeb) {
    DEVTYPE dv = cfebs_[icfeb].scamDevice();
    devdo(dv,-1,cmd,0,sndbuf,rcvbuf,2);
  }
}


void DAQMB::restoreMotherboardIdle() {
  devdo(MCTRL,-1,cmd,0,sndbuf,rcvbuf,2);
}

/* DAQMB trigger primitives */

void  DAQMB::set_comp_mode(int dword)
{
  for(unsigned icfeb = 0; icfeb < cfebs_.size(); ++icfeb) {
      DEVTYPE dv = cfebs_[icfeb].scamDevice();
      
      cmd[0]=VTX_USR1;
      sndbuf[0]=COMP_MODE;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=VTX_USR2;
      // default is 0x17
      sndbuf[0]=dword&0XFF;
      devdo(dv,5,cmd,5,sndbuf,rcvbuf,0);
      cmd[0]=VTX_BYPASS;
      sndbuf[0]=0;
      devdo(dv,5,cmd,0,sndbuf,rcvbuf,0);
  }
  (*MyOutput_) << "calling set_comp_mode " << dword << std::endl;
}

void DAQMB::set_comp_thresh(float thresh)
{
char dt[2];
// 
/* digitize voltages */
// 
 int dthresh=int(4095*((3.5-thresh)/3.5)); 
 dt[0]=0;
 dt[1]=0;
 for(int i=0;i<8;i++){
   dt[0]|=((dthresh>>(i+7))&1)<<(7-i);
   dt[1]|=((dthresh>>i)&1)<<(6-i);
 }
 dt[0]=((dt[1]<<7)&0x80) + ((dt[0]>>1)&0x7f);
 dt[1]=dt[1]>>1;
 (*MyOutput_) << "CFEB size="<<cfebs_.size() << std::endl;
 //
 for(int i=0; i<cfebs_.size();i++) {
   (*MyOutput_) << i << " CFEB number" << cfebs_[i].number() << std::endl;
 }
 //
 for(unsigned icfeb = 0; icfeb < cfebs_.size(); ++icfeb) {
   DEVTYPE dv = cfebs_[icfeb].scamDevice();
   cmd[0]=VTX_USR1;
   sndbuf[0]=COMP_DAC;
   devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
   cmd[0]=VTX_USR2;
   sndbuf[0]=dt[0];
   sndbuf[1]=dt[1];
   sndbuf[2]=0x00; 
   devdo(dv,5,cmd,15,sndbuf,rcvbuf,0);
   cmd[0]=VTX_USR1;
   sndbuf[0]=NOOP;
   devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
   cmd[0]=VTX_BYPASS;
   devdo(dv,5,cmd,0,sndbuf,rcvbuf,0);
   usleep(20);
 }
}
//
void DAQMB::set_comp_thresh(int icfeb, float thresh)
{
char dt[2];
// 
/* digitize voltages */
// 
 int dthresh=int(4095*((3.5-thresh)/3.5)); 
 dt[0]=0;
 dt[1]=0;
 for(int i=0;i<8;i++){
   dt[0]|=((dthresh>>(i+7))&1)<<(7-i);
   dt[1]|=((dthresh>>i)&1)<<(6-i);
 }
 dt[0]=((dt[1]<<7)&0x80) + ((dt[0]>>1)&0x7f);
 dt[1]=dt[1]>>1;
 //
 (*MyOutput_) << "Set_comp_thresh.icfeb=" << cfebs_[icfeb].number() << " thresh=" << thresh << std::endl;
 //
 DEVTYPE dv = cfebs_[icfeb].scamDevice();
 //
 (*MyOutput_) << "cfeb= " << icfeb << std::endl;
 (*MyOutput_) << "dv= " << dv << std::endl;
 //
 cmd[0]=VTX_USR1;
 sndbuf[0]=COMP_DAC;
 devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
 cmd[0]=VTX_USR2;
 sndbuf[0]=dt[0];
 sndbuf[1]=dt[1];
 sndbuf[2]=0x00; 
 devdo(dv,5,cmd,15,sndbuf,rcvbuf,0);
 cmd[0]=VTX_USR1;
 sndbuf[0]=NOOP;
 devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
 cmd[0]=VTX_BYPASS;
 devdo(dv,5,cmd,0,sndbuf,rcvbuf,0);
 usleep(20);
}
//
void DAQMB::set_dac(float volt0,float volt1)
{
  unsigned short int dacout0,dacout1;
  /* digitize voltages */
  dacout0=(unsigned short int)(volt0*4095./5.0);
  dacout1=(unsigned short int)(volt1*4095./5.0);
  /* load cdac */
  /* cmd[0]=(0xff&dacout0);
     cmd[1]=((dacout0>>8)&0xff);
     cmd[2]=(0xff&dacout1);
     cmd[3]=((dacout1>>8)&0xff);
  */
  //Temperarily swapped here, GU
  cmd[2]=(0xff&dacout0);
  cmd[3]=((dacout0>>8)&0xff);
  cmd[0]=(0xff&dacout1);
  cmd[1]=((dacout1>>8)&0xff);
  //printf(" CDAC %04x %04x %02x %02x \n",dacout0,dacout1,cmd[0]&0xff,cmd[1])&0xff;
  devdo(CDAC,32,cmd,0,sndbuf,rcvbuf,2); 
}


void DAQMB::halfset(int icrd,int ipln,int ihalf,int chan[5][6][16])
{

   int ichan,iside;

   //(*MyOutput_) << "DAQMB.halfset " << std::endl;
   
   if (ihalf<0 && icrd-1>=0) {
      ihalf += 32;
      icrd  -= 1;
   }

   //if(ihalf>=0&&ihalf<32){
      ichan=ihalf/2;
      iside=ihalf-2*ichan;
      if(iside==0){
	 if(ichan-1>=0)chan[icrd][ipln][ichan-1]=MEDIUM_CAP;
	 if(ichan-1<0&&icrd-1>=0)chan[icrd-1][ipln][15]=MEDIUM_CAP;	 
	 if(ichan>=0&&ichan<=15)chan[icrd][ipln][ichan]=LARGE_CAP;	 
	 if(ichan+1<=15)chan[icrd][ipln][ichan+1]=SMALL_CAP;
	 if(ichan+1>15&&icrd+1<5)chan[icrd+1][ipln][0]=SMALL_CAP;
      }
      if(iside==1){
	 if(ichan-1>=0)chan[icrd][ipln][ichan-1]=SMALL_CAP;
	 if(ichan-1<0&&icrd-1>=0)chan[icrd-1][ipln][15]=SMALL_CAP;	 
	 if(ichan>=0&&ichan<=15)chan[icrd][ipln][ichan]=LARGE_CAP;	 
	 if(ichan+1<=15)chan[icrd][ipln][ichan+1]=MEDIUM_CAP;
	 if(ichan+1>15&&icrd+1<5)chan[icrd+1][ipln][0]=MEDIUM_CAP;
      }
      //}
      //else {
      //printf("Half strip out of range: %d (must be between 0<=hs<=31)\n",ihalf);
      //}
}


void  DAQMB::halfset(int ifeb,int ipln,int ihalf)
{
  if(ihalf>=0&&ihalf<32){
    int ichan=ihalf/2;
    int iside=ihalf-2*ichan;
    if(iside==0){
      if(ichan-1>=0) shift_array[ifeb][ipln][ichan-1]=MEDIUM_CAP;
      if(ichan-1<0&&ifeb-1>=0) shift_array[ifeb-1][ipln][15]=MEDIUM_CAP;

      if(ichan>=0&&ichan<=15) shift_array[ifeb][ipln][ichan]=LARGE_CAP;

      if(ichan+1<=15) shift_array[ifeb][ipln][ichan+1]=SMALL_CAP;
      if(ichan+1>15&&ifeb+1<5) shift_array[ifeb+1][ipln][0]=SMALL_CAP;
    }
    if(iside==1){
      if(ichan-1>=0) shift_array[ifeb][ipln][ichan-1]=SMALL_CAP;
      if(ichan-1<0&&ifeb-1>=0) shift_array[ifeb-1][ipln][15]=SMALL_CAP;

      if(ichan>=0&&ichan<=15) shift_array[ifeb][ipln][ichan]=LARGE_CAP;

      if(ichan+1<=15) shift_array[ifeb][ipln][ichan+1]=MEDIUM_CAP;
      if(ichan+1>15&&ifeb+1<5) shift_array[ifeb+1][ipln][0]=MEDIUM_CAP;
    }
  }

}

//
void DAQMB::trigsetx(int *hp)
{
  //
  (*MyOutput_) << "DAQMB.trigsetx" << std::endl;
  (*MyOutput_) << "CFEB size="<<cfebs_.size() << std::endl;;
  //
  int hs[6];
  int i,j,k;
  int chan[5][6][16];
  for(i=0;i<5;i++){
    for(j=0;j<6;j++){
      for(k=0;k<16;k++){
        chan[i][j][k]=NORM_RUN;
      }
    }
  }
  //
  (*MyOutput_) << "Setting Staggering" << std::endl;
  //
  for(i=0;i<6;i+=2){
     hs[i]=-1;
     hs[i+1]=0;
  } 
  //
  (*MyOutput_) << " Setting halfset " << std::endl;
  //
  for(k=0;k<5;k++){
     for(j=0;j<6;j++){
	halfset(k,j,hp[j]+hs[j],chan);
     }
  }
  chan2shift(chan);
}
//
void DAQMB::chan2shift(int chan[5][6][16])
{
   
   int i,j,l;
   int chip,lay,nchips;
   char chip_mask;
   char shft_bits[6][6];
   
   (*MyOutput_) << "CFEB size=" << cfebs_.size() << std::endl;
   (*MyOutput_) << "CFEB numbers" << std::endl;

   for(unsigned icfeb = 0; icfeb < cfebs_.size(); ++icfeb) (*MyOutput_) << " " << cfebs_[icfeb].number() ;

   (*MyOutput_) << std::endl;
   
   for(lay=0;lay<6;lay++){
      for(unsigned icfeb = 0; icfeb < cfebs_.size(); ++icfeb) {
	 int brdn = cfebs_[icfeb].number();
	 for(int i=0; i<16;i++) {
	    if ( chan[brdn][lay][i] > 0 ) printf("%c[01;43m", '\033');
	    (*MyOutput_) << chan[brdn][lay][i] << "" ;
	    printf("%c[0m", '\033'); 
	 }
	 (*MyOutput_) << " | " ;		
      }
      (*MyOutput_) << std::endl;
   }
   
   for(unsigned icfeb = 0; icfeb < cfebs_.size(); ++icfeb) {
      DEVTYPE dv   = cfebs_[icfeb].scamDevice() ;
      int brdn = cfebs_[icfeb].number();
      printf(" brdn=%d F1SCAM=%d dv=%d\n",brdn,F1SCAM,dv);
      for(lay=0;lay<6;lay++){
	 shft_bits[lay][0]=((chan[brdn][lay][13]<<6)|(chan[brdn][lay][14]<<3)|chan[brdn][lay][15])&0XFF;
	 shft_bits[lay][1]=((chan[brdn][lay][10]<<7)|(chan[brdn][lay][11]<<4)|(chan[brdn][lay][12]<<1)|(chan[brdn][lay][13]>>2))&0XFF;
	 shft_bits[lay][2]=((chan[brdn][lay][8]<<5)|(chan[brdn][lay][9]<<2)|(chan[brdn][lay][10]>>1))&0XFF;
	 shft_bits[lay][3]=((chan[brdn][lay][5]<<6)|(chan[brdn][lay][6]<<3)|chan[brdn][lay][7])&0XFF;
	 shft_bits[lay][4]=((chan[brdn][lay][2]<<7)|(chan[brdn][lay][3]<<4)|(chan[brdn][lay][4]<<1)|(chan[brdn][lay][5]>>2))&0XFF;
	 shft_bits[lay][5]=((chan[brdn][lay][0]<<5)|(chan[brdn][lay][1]<<2)|(chan[brdn][lay][2]>>1))&0XFF;
     }
      cmd[0]=VTX_USR1;
      sndbuf[0]=CHIP_MASK;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=VTX_USR2;
      chip_mask=0;
      for(i=0;i<6;i++){
	 chip_mask=chip_mask|(1<<(5-i));
      }
      devdo(dv,5,cmd,6,&chip_mask,rcvbuf,0);
      nchips=6;
      cmd[0]=VTX_USR1;
      sndbuf[0]=CHIP_SHFT;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=VTX_USR2;
      j=0;
      for(chip=5;chip>=0;chip--){
	 lay=layers[chip];
	 for(i=0;i<6;i++){
	    sndbuf[j*6+i]=shft_bits[lay][i];
	 }
	 j++;
      }
      devdo(dv,5,cmd,nchips*48,sndbuf,rcvbuf,0);
      cmd[0]=VTX_USR1;
      sndbuf[0]=NOOP;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=VTX_BYPASS;
      devdo(dv,5,cmd,0,sndbuf,rcvbuf,0);
   }
}


void DAQMB::trigtest()
{
  int i,k;
  int hp[6],ho[]={-1,0,1,2};
  int imid,hits;
  int ip0,ip1,ip2,ip3,ip4,ip5;
  int p0hit,p1hit,p2hit,p3hit,p4hit,p5hit;
  //
  // Set the comparator threshold for all
  //
  set_comp_thresh(.05);
  //
  for(i=0;i<5;i++){
      set_dac(.5,0);
      imid=16;
      ho[3]=-imid-1;
      p0hit=1;
      for(ip0=0;ip0<4;ip0++){
        hp[0]=imid+ho[ip0];
        if(ip0==3)p0hit=0;
        p1hit=1;
        for(ip1=0;ip1<4;ip1++){
          hp[1]=imid+ho[ip1];
          if(ip1==3)p1hit=0;
          p2hit=1;
          for(ip2=1;ip2<4;ip2+=2){
            hp[2]=imid+ho[ip2];
            if(ip2==3)p2hit=0;
            p3hit=1;
            for(ip3=0;ip3<4;ip3++){
              hp[3]=imid+ho[ip3];
              if(ip3==3)p3hit=0;
              p4hit=1;
              for(ip4=0;ip4<4;ip4++){
                hp[4]=imid+ho[ip4];
                if(ip4==3)p4hit=0;
                p5hit=1;
                for(ip5=0;ip5<4;ip5++){
                  hp[5]=imid+ho[ip5];
                  if(ip5==3)p5hit=0;
                  hits=p0hit+p1hit+p2hit+p3hit+p4hit+p5hit;
                  (*MyOutput_) << "Pattern :" << std::endl ;
                  for(k=0;k<6;k++){
		    printf(" %d ",hp[k]);
                  }
                  (*MyOutput_) << std::endl;
                  (*MyOutput_) << "Hits set = " << hits << std::endl;
                  trigsetx(hp);
                  inject(1,0x4f);
		  (*MyOutput_) << " Next ?"<< std::endl;
		  int next;
		  std::cin >> next;
                }
              }
            }
          }
        }
      }
  }
}

/*DAQMB set trigger source */

//extern char cmd[];
//extern char sndbuf[];
//extern char rcvbuf[];

void DAQMB::settrgsrc(int dword)
{
  //
  cmd[0]=VTX2_USR1;
  sndbuf[0]=37;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  //
  //devdo(MCTRL,6,cmd,4,sndbuf,rcvbuf,0);
  //printf("Cal_trg source was Set to %01x (Hex). \n",unpack_ival());
  //
  sndbuf[0]=dword&0x0F; 
  devdo(MCTRL,6,cmd,4,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  //
  printf("Cal_trg source are Set to %01x (Hex). \n",dword&0xf);
  //
}


/* DAQMB   Voltages  */

float DAQMB::adcplus(int ichp,int ichn){
  unsigned int ival= (readADC(ichp, ichn)&0x0fff);
  return (float) ival;
}

//
float DAQMB::adcminus(int ichp,int ichn){
  short int ival= (readADC(ichp, ichn)&0x0fff);
  if((0x0800&ival)==0x0800)ival=ival|0xf000;
  float cval;
  cval = ival;
  return (float) ival;
}

//
float DAQMB::adc16(int ichp,int ichn){
  unsigned int ival= readADC(ichp, ichn);
  float cval=ival*4.999924/65535.;
  return cval;
}

/* Thermometers */


float DAQMB::readthermx(int feb)
{
  float cval,fval;
  float Vout= (float) readADC(1, feb) / 1000.;
  if(feb<7){
    cval = 1/(0.1049406423E-2+0.2133635468E-3*log(65000.0/Vout-13000.0)+0.7522287E-7*pow(log(65000.0/Vout-13000.0),3.0))-0.27315E3;
    fval=9.0/5.0*cval+32.;
  }else{
    fval=Vout;
  }
  return fval;
}


unsigned int DAQMB::readADC(int ireg, int ichn) {
  cmd[0]=ireg; /* register 1-4 */
  cmd[1]=ichn; /* channel 0-7 */
  devdo(MADC,16,cmd,0,sndbuf,rcvbuf,2);
  return unpack_ival();
}


/* DAQMB LVMB routines */

float DAQMB::lowv_adc(int ichp,int ichn)
{
static int conv_lowv[5][8]={
  {2,2,1,1,2,2,1,2},
  {2,1,2,2,1,2,2,1},
  {2,2,1,2,2,2,2,2},
  {2,2,2,2,2,2,2,2},
  {2,2,2,2,2,2,2,2}
}; 

 cmd[0]=ichp; 
 cmd[1]=ichn; 
 devdo(LOWVOLT,16,cmd,0,sndbuf,rcvbuf,2);
 unsigned int ival=unpack_ival();
  ival=ival*conv_lowv[ichp-1][ichn];
 float cval=ival*5.0/4.096;
  return cval;
} 

void DAQMB::lowv_onoff(char c)
{
 cmd[0]=0x08; /* write power register */
 cmd[1]=c; /* 0x3f means all on, 0x00 means all off  */
 devdo(LOWVOLT,16,cmd,0,sndbuf,rcvbuf,2);
}

unsigned int DAQMB::lowv_rdpwrreg()
{
 cmd[0]=0x09; /* write power register */
 cmd[1]=0x00; 
 devdo(LOWVOLT,16,cmd,0,sndbuf,rcvbuf,2);
 return unpack_ival();
}

/* FPGA and PROM codes  */

unsigned long int DAQMB::febpromuser(const CFEB & cfeb)
{
  DEVTYPE dv = cfeb.promDevice();
  cmd[0]=PROM_USERCODE;
  sndbuf[0]=0xFF;
  sndbuf[1]=0xFF;
  sndbuf[2]=0xFF;
  sndbuf[3]=0xFF;
  sndbuf[4]=0xFF;
  devdo(dv,8,cmd,33,sndbuf,rcvbuf,1);
  rcvbuf[0]=((rcvbuf[0]>>1)&0x7f)+((rcvbuf[1]<<7)&0x80);
  rcvbuf[1]=((rcvbuf[1]>>1)&0x7f)+((rcvbuf[2]<<7)&0x80);
  rcvbuf[2]=((rcvbuf[2]>>1)&0x7f)+((rcvbuf[3]<<7)&0x80);
  rcvbuf[3]=((rcvbuf[3]>>1)&0x7f)+((rcvbuf[4]<<7)&0x80);
  unsigned long int ibrd=unpack_ibrd();
  cmd[0]=PROM_BYPASS;
  sndbuf[0]=0;
  devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
  usleep(100);
  return ibrd;
}

unsigned long int  DAQMB::febpromid(const CFEB & cfeb)
{
  DEVTYPE dv = cfeb.promDevice();
  cmd[0]=PROM_IDCODE;
  sndbuf[0]=0xFF;
  sndbuf[1]=0xFF;
  sndbuf[2]=0xFF;
  sndbuf[3]=0xFF;
  sndbuf[4]=0xFF;
  devdo(dv,8,cmd,33,sndbuf,rcvbuf,1);
  rcvbuf[0]=((rcvbuf[0]>>1)&0x7f)+((rcvbuf[1]<<7)&0x80);
  rcvbuf[1]=((rcvbuf[1]>>1)&0x7f)+((rcvbuf[2]<<7)&0x80);
  rcvbuf[2]=((rcvbuf[2]>>1)&0x7f)+((rcvbuf[3]<<7)&0x80);
  rcvbuf[3]=((rcvbuf[3]>>1)&0x7f)+((rcvbuf[4]<<7)&0x80);
  unsigned long int ibrd=unpack_ibrd();
  cmd[0]=PROM_BYPASS;
  sndbuf[0]=0;
  devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
  return ibrd;
}

unsigned long int  DAQMB::febfpgauser(const CFEB & cfeb)
{
  unsigned long int ibrd;
  DEVTYPE dv = cfeb.scamDevice();
  cmd[0]=VTX_USERCODE;
  sndbuf[0]=0xFF;
  sndbuf[1]=0xFF;
  sndbuf[2]=0xFF;
  sndbuf[3]=0xFF;
  devdo(dv,5,cmd,32,sndbuf,rcvbuf,1);
  // RPW not sure about this
  //printf(" The FPGA USERCODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
  ibrd = unpack_ibrd();
  cmd[0]=VTX_BYPASS;
  sndbuf[0]=0;
  devdo(dv,5,cmd,0,sndbuf,rcvbuf,0);
  usleep(100);
  return ibrd;
}

unsigned long int  DAQMB::febfpgaid(const CFEB & cfeb)
{
  DEVTYPE dv = cfeb.scamDevice();
  cmd[0]=VTX_IDCODE;
  sndbuf[0]=0xFF;
  sndbuf[1]=0xFF;
  sndbuf[2]=0xFF;
  sndbuf[3]=0xFF;
  devdo(dv,5,cmd,32,sndbuf,rcvbuf,1);
  (*MyOutput_) << " The FEB " << dv-F1SCAM+1 << "FPGA Chip should be 610093 (last 6 digits) "  << std::endl;
  (*MyOutput_) << " The FPGA Chip IDCODE is " << hex << 
    (0xff&rcvbuf[3]) << (0xff&rcvbuf[2]) << (0xff&rcvbuf[1]) << (0xff&rcvbuf[0]) << std::endl;
  // RPW not sure about this
  unsigned long ibrd = unpack_ibrd();
  cmd[0]=VTX_BYPASS;
  sndbuf[0]=0;
  devdo(dv,5,cmd,0,sndbuf,rcvbuf,0);
  return ibrd;
}

unsigned long int DAQMB::mbpromuser(int prom)
{
unsigned long int ibrd;
DEVTYPE dv;
 

  if(prom==0){dv=VPROM;}else{dv=MPROM;}
      cmd[0]=PROM_USERCODE;
      sndbuf[0]=0xFF;
      sndbuf[1]=0xFF;
      sndbuf[2]=0xFF;
      sndbuf[3]=0xFF;
      sndbuf[4]=0xFF;
      devdo(dv,8,cmd,32,sndbuf,rcvbuf,1);
      ibrd=unpack_ibrd();
      cmd[0]=PROM_BYPASS;
      sndbuf[0]=0;
      devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
      return ibrd;
}

unsigned long int  DAQMB::mbpromid(int prom)
{
unsigned long int ibrd;
DEVTYPE dv;
 

  if(prom==0){dv=VPROM;}else{dv=MPROM;}
      cmd[0]=PROM_IDCODE;
      sndbuf[0]=0xFF;
      sndbuf[1]=0xFF;
      sndbuf[2]=0xFF;
      sndbuf[3]=0xFF;
      sndbuf[4]=0xFF;
      devdo(dv,8,cmd,32,sndbuf,rcvbuf,1);
      ibrd=unpack_ibrd();
      cmd[0]=PROM_BYPASS;
      sndbuf[0]=0;
      devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
      return ibrd;
}


unsigned long int  DAQMB::mbfpgauser()
{
 
  DEVTYPE dv=MCTRL;
  cmd[0]=VTX2_USERCODE;
  sndbuf[0]=0xFF;
  sndbuf[1]=0xFF;
  sndbuf[2]=0xFF;
  sndbuf[3]=0xFF;
  sndbuf[4]=0xFF;
  devdo(dv,6,cmd,32,sndbuf,rcvbuf,1);
  unsigned long int ibrd=unpack_ibrd();
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(dv,6,cmd,0,sndbuf,rcvbuf,0);
  return ibrd;
}

unsigned long int  DAQMB::mbfpgaid()
{
  //
  unsigned long int ibrd;
  //
  DEVTYPE dv=MCTRL;
  cmd[0]=VTX2_IDCODE;
  sndbuf[0]=0xFF;
  sndbuf[1]=0xFF;
  sndbuf[2]=0xFF;
  sndbuf[3]=0xFF;
  sndbuf[4]=0xFF;
  devdo(dv,6,cmd,32,sndbuf,rcvbuf,1);
  ibrd=unpack_ibrd();
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(dv,6,cmd,0,sndbuf,rcvbuf,0);
  return ibrd;
}

//
// DAQMB calibrate
//
void DAQMB::set_cal_dac(float volt0,float volt1)
{
unsigned short int dacout0,dacout1;

/* digitize voltages */  
  dacout0=int(volt0*4095./5.0);
  dacout1=int(volt1*4095./5.0);
 /* load cdac */
  /* cmd[0]=(0xff&dacout0);
 cmd[1]=((dacout0>>8)&0xff);
 cmd[2]=(0xff&dacout1);
 cmd[3]=((dacout1>>8)&0xff);
  */
  //Temperarily swapped here, GU
 cmd[2]=(0xff&dacout0);
 cmd[3]=((dacout0>>8)&0xff);
 cmd[0]=(0xff&dacout1);
 cmd[1]=((dacout1>>8)&0xff);
 devdo(CDAC,32,cmd,0,sndbuf,rcvbuf,2); 
}

void DAQMB::buck_shift()
{
int lay,i,j;
int nchips2;
char shft_bits[6][6];

  for(CFEBItr cfebItr = cfebs_.begin(); cfebItr != cfebs_.end(); ++cfebItr) {
    
    DEVTYPE dv = cfebItr->scamDevice();
    int brd=cfebItr->number();
      for(lay=0;lay<6;lay++){
        shft_bits[lay][0]=((shift_array[brd][lay][13]<<6)|(shift_array[brd][lay][14]<<3)|shift_array[brd][lay][15])&0XFF;
        shft_bits[lay][1]=((shift_array[brd][lay][10]<<7)|(shift_array[brd][lay][11]<<4)|(shift_array[brd][lay][12]<<1)|(shift_array[brd][lay][13]>>2))&0XFF;
        shft_bits[lay][2]=((shift_array[brd][lay][8]<<5)|(shift_array[brd][lay][9]<<2)|(shift_array[brd][lay][10]>>1))&0XFF;
        shft_bits[lay][3]=((shift_array[brd][lay][5]<<6)|(shift_array[brd][lay][6]<<3)|shift_array[brd][lay][7])&0XFF;
        shft_bits[lay][4]=((shift_array[brd][lay][2]<<7)|(shift_array[brd][lay][3]<<4)|(shift_array[brd][lay][4]<<1)|(shift_array[brd][lay][5]>>2))&0XFF;
        shft_bits[lay][5]=((shift_array[brd][lay][0]<<5)|(shift_array[brd][lay][1]<<2)|(shift_array[brd][lay][2]>>1))&0XFF;
      }
      cmd[0]=VTX_USR1;
      sndbuf[0]=CHIP_MASK;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      // (*MyOutput_)<<" first devdo call \n";
      cmd[0]=VTX_USR2;
      char chip_mask= cfebItr->chipMask();
      devdo(dv,5,cmd,6,&chip_mask,rcvbuf,0);
      std::vector<BuckeyeChip> buckeyes = cfebItr->buckeyeChips();
      nchips2=buckeyes.size();
      cmd[0]=VTX_USR1;
      sndbuf[0]=CHIP_SHFT;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=VTX_USR2;
      j=0;
      for(int ichip = nchips2-1; ichip >= 0; --ichip) {
        int chip = buckeyes[ichip].number();

        lay=layers[chip];
        for(i=0;i<6;i++){
          sndbuf[j*6+i]=shft_bits[lay][i];
        }
        j++;
      }

      devdo(dv,5,cmd,nchips2*48,sndbuf,rcvbuf,0);
      cmd[0]=VTX_USR1;
      sndbuf[0]=NOOP;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=VTX_BYPASS;
      devdo(dv,5,cmd,0,sndbuf,rcvbuf,0);
  }
}

void DAQMB::buck_shift_out()
{
int lay,i,j;
int nchips2;
int boffset;
int xtrabits = 2;
int swtchbits = -1;
char shft_bits[6][6];
  boffset=xtrabits+swtchbits;

  for(CFEBItr cfebItr = cfebs_.begin(); cfebItr != cfebs_.end(); ++cfebItr) {
    
    DEVTYPE dv = cfebItr->scamDevice();
    int brd=cfebItr->number();
    printf(" BUCKSHIFT scamdev %d brd %d \n",dv,brd);
      for(lay=0;lay<6;lay++){
        shft_bits[lay][0]=((shift_array[brd][lay][13]<<6)|(shift_array[brd][lay][14]<<3)|shift_array[brd][lay][15])&0XFF;
        shft_bits[lay][1]=((shift_array[brd][lay][10]<<7)|(shift_array[brd][lay][11]<<4)|(shift_array[brd][lay][12]<<1)|(shift_array[brd][lay][13]>>2))&0XFF;
        shft_bits[lay][2]=((shift_array[brd][lay][8]<<5)|(shift_array[brd][lay][9]<<2)|(shift_array[brd][lay][10]>>1))&0XFF;
        shft_bits[lay][3]=((shift_array[brd][lay][5]<<6)|(shift_array[brd][lay][6]<<3)|shift_array[brd][lay][7])&0XFF;
        shft_bits[lay][4]=((shift_array[brd][lay][2]<<7)|(shift_array[brd][lay][3]<<4)|(shift_array[brd][lay][4]<<1)|(shift_array[brd][lay][5]>>2))&0XFF;
        shft_bits[lay][5]=((shift_array[brd][lay][0]<<5)|(shift_array[brd][lay][1]<<2)|(shift_array[brd][lay][2]>>1))&0XFF;
      }
      cmd[0]=VTX_USR1;
      sndbuf[0]=CHIP_MASK;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      // cout<<" first devdo call \n";
      cmd[0]=VTX_USR2;
      char chip_mask= cfebItr->chipMask();
      devdo(dv,5,cmd,6,&chip_mask,rcvbuf,0);
      vector<BuckeyeChip> buckeyes = cfebItr->buckeyeChips();
      nchips2=buckeyes.size();
      cmd[0]=VTX_USR1;
      sndbuf[0]=CHIP_SHFT;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=VTX_USR2;
      j=0;
      for(int ichip = nchips2-1; ichip >= 0; --ichip) {
        int chip = buckeyes[ichip].number();

        lay=layers[chip];
        for(i=0;i<6;i++){
          sndbuf[j*6+i]=shft_bits[lay][i];
        }
        j++;
      }

      devdo(dv,5,cmd,nchips2*48+xtrabits,sndbuf,rcvbuf,0);
      cmd[0]=VTX_USR1;
      sndbuf[0]=NOOP;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=VTX_BYPASS;
      devdo(dv,5,cmd,0,sndbuf,rcvbuf,2); 
      for(int k=0;k<6*nchips2;k++)shift_out[brd][k]=0;
      int jj=0; 
      for(int k=6*(nchips2);k>=0;k--){
        shift_out[brd][jj]=shift_out[brd][jj]|(rcvbuf[k]<<(8-boffset));
        shift_out[brd][jj]=shift_out[brd][jj]|((rcvbuf[k-1]>>boffset)&~(~0<<(8-boffset)));
        jj++;
      }
  }
}



int DAQMB::buck_shift_test()
{
#ifdef debugV
  (*MyOutput_) << "inside buck_shift_test()" << std::endl;
#endif

int i,j,nmtch;
int nchips2;
int xtrabits = 2;
int swtchbits = -1;
char pat[42],chk[42] = {0xBA,0xDF,0xEE,0xD5,0xDE,0xAD};

  int pass=0;
  int boffset=xtrabits+swtchbits;

  for(CFEBItr cfebItr = cfebs_.begin(); cfebItr != cfebs_.end(); ++cfebItr) {

    DEVTYPE dv = cfebItr->scamDevice();
    //int brdn=cfebItr->number();

      cmd[0]=VTX_USR1;
      sndbuf[0]=CHIP_MASK;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=VTX_USR2;
      char chip_mask = cfebItr->chipMask();
      devdo(dv,5,cmd,6,&chip_mask,rcvbuf,0);
      nchips2=cfebItr->buckeyeChips().size();
      printf(" nchips2 %d chip_mask %04x \n",nchips2,chip_mask);
      for(i=0;i<6*nchips2;i++)pat[i]=0;
      cmd[0]=VTX_USR1;
      sndbuf[0]=CHIP_SHFT;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,2);
      cmd[0]=VTX_USR2;
      sndbuf[0]=0xAD;
      sndbuf[1]=0xDE;
      sndbuf[2]=0xD5;
      sndbuf[3]=0xEE;
      sndbuf[4]=0xDF;
      sndbuf[5]=0xBA;
      for(i=6;i<6*(nchips2+1)+1;i++)sndbuf[i]=0x00;
      devdo(dv,5,cmd,6*(nchips2+1)*8+xtrabits,sndbuf,rcvbuf,1);
      j=0;
      //   for(i=6*(nchips2+1);i>=6*nchips2;i--){
      for(i=6*(nchips2+1);i>=0;i--){
        printf("shfttst %d %02X \n",i,rcvbuf[i]&0xFF);
        pat[j]=pat[j]|(rcvbuf[i]<<(8-boffset));
        pat[j]=pat[j]|((rcvbuf[i-1]>>boffset)&~(~0<<(8-boffset)));
        j++;
      }
      nmtch=0;
      for(i=0;i<6;i++){
	// (*MyOutput_)<< hex <<pat[i] << dec;
        printf("%02x",pat[i]&0xff);
        if(pat[i]==chk[i])nmtch++;
      }
      printf("\n");
      // (*MyOutput_)<< endl;
      if(nmtch==6){
        pass=1;
        (*MyOutput_)<<"Pattern returned is OK\n";
      } else {
        pass=0;
        (*MyOutput_)<<"Pattern returned is Wrong\n";
      }
      cmd[0]=VTX_USR1;
      sndbuf[0]=NOOP;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=VTX_BYPASS;
      devdo(dv,5,cmd,0,sndbuf,rcvbuf,2);
  }
  return pass;
}


void DAQMB::preamp_initx()
{
int i,j,nchips2;
  for(CFEBItr cfebItr = cfebs_.begin(); cfebItr != cfebs_.end(); ++cfebItr) {

    DEVTYPE dv = cfebItr->scamDevice();

    cmd[0]=VTX_USR1;
    sndbuf[0]=CHIP_MASK;
    devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
    cmd[0]=VTX_USR2;
    char chip_mask= cfebItr->chipMask();
    devdo(dv,5,cmd,6,&chip_mask,rcvbuf,0);
    nchips2 = cfebItr->buckeyeChips().size();
    cmd[0]=VTX_USR1;
    sndbuf[0]=CHIP_SHFT;
    devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
    cmd[0]=VTX_USR2;
    for(i=0;i<nchips2;i++){
      for(j=0;j<6;j++){
        sndbuf[i*6+j]=0;
      }
    }
    devdo(dv,5,cmd,nchips2*48,sndbuf,rcvbuf,0);
    cmd[0]=VTX_USR1;
    sndbuf[0]=NOOP;
    devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
    cmd[0]=VTX_BYPASS;
    devdo(dv,5,cmd,0,sndbuf,rcvbuf,0);
  }
  (*MyOutput_) << "done with preamp init " << std::endl;
}


void DAQMB::set_cal_tim_pulse(int ntim)

{
  //(*MyOutput_)<< "setting pulse timing to " << ntim << std::endl;  
  int dword;
  dword=(CAL_DEF_DELAY)&0x1ff;
  dword=dword|((ntim&0x1f)<<9); 
  setcaldelay(dword);

}


void DAQMB::set_cal_tim_inject(int ntim)
{
  (*MyOutput_)<< "setting inject timing to " << ntim << std::endl;
  int dword;
  dword=(CAL_DEF_DELAY)&0x3fff;
  dword=dword|((ntim&0x1f)<<14);
  setcaldelay(dword);
}
  

void DAQMB::toggle_pedestal()
{
    cmd[0]=VTX2_USR1;
    sndbuf[0]=PED_TRIG;
    devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,1);
/*    printf(" Returned from CYCLYE_TRIG instr: %02X  \n",rcvbuf[0]&0xFF); 
    cmd[0]=VTX2_BYPASS;
    devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
    cmd[0]=VTX2_USR2;
    devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0); */
    cmd[0]=VTX2_USR1;
    sndbuf[0]=NOOP;
    devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
    /*  printf(" Returned from NOOP instr: %02X  \n",rcvbuf[0]&0xFF);*/
    cmd[0]=VTX2_BYPASS;
    devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
}


void DAQMB::pulse(int Num_pulse,unsigned int pulse_delay)
{
  int j;
   for(j=0;j<Num_pulse;j++){
     sndbuf[1]=(pulse_delay&0xff00)>>8;
     sndbuf[0]=(pulse_delay&0x00ff);
     devdo(MCTRL,-99,sndbuf,0,sndbuf,rcvbuf,0); 
     cmd[0]=VTX2_USR1;
     sndbuf[0]=PULSE;
     devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
     cmd[0]=VTX2_BYPASS;
     devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
     cmd[0]=VTX2_USR1;
     sndbuf[0]=NOOP;
     devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
     cmd[0]=VTX2_BYPASS;
     if(j<(Num_pulse-1)){
     devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);  
     }
     else{
     devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
     }
   }
}

void DAQMB::inject(int Num_pulse,unsigned int pulse_delay)
{

   (*MyOutput_) << "DAQMB.inject " << std::endl;

   for(int j=0;j<Num_pulse;j++){
     sndbuf[1]=(pulse_delay&0xff00)>>8;
     sndbuf[0]=(pulse_delay&0x00ff);
     devdo(MCTRL,-99,sndbuf,0,sndbuf,rcvbuf,0); 
     cmd[0]=VTX2_USR1;
     sndbuf[0]=INJECT;
     devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
     cmd[0]=VTX2_BYPASS;
     devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
     cmd[0]=VTX2_USR1;
     sndbuf[0]=NOOP;
     devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
     cmd[0]=VTX2_BYPASS;
     if(j<(Num_pulse-1)){
	devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);  
     }
     else
     {
	devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
     }
   }
}


// DAQMB fifo write and reads

void DAQMB::wrtfifo(int fifo,int nsndfifo,char* sndfifo)
{
  int i=fifo+FIFO1;
  DEVTYPE devnum=(DEVTYPE)i;
  //
  printf(" sndfifo: %d %02x %02x \n",nsndfifo,sndfifo[0]&0xff,sndfifo[1]&0xff);
  /* fifo write */  
  cmd[0]=4;
  //(*MyOutput_) << "wrtfifo devnum FIFO7 " << devnum << " " << FIFO7 << std::endl;
  if(devnum-FIFO7!=0){
    //(*MyOutput_) << "devdo1" << std::endl;
    devdo(devnum,1,cmd,nsndfifo*2,sndfifo,rcvbuf,2);
    //(*MyOutput_) << "devdo1back" << std::endl;
  }
  else{
    //(*MyOutput_) << "devdo2" << std::endl;
    devdo(devnum,1,cmd,nsndfifo,sndfifo,rcvbuf,2);
    //(*MyOutput_) << "devdo2back" << std::endl;
  } 
}
//
void DAQMB::readfifo(int fifo,int nrcvfifo,char* rcvfifo)
{  
  //
  //(*MyOutput_) << "readfifo" << std::endl;
  //
  PRINTSTRING(OVAL: before start routine in readfifo);
  PRINTSTRING(OVAL: after start routine in readfifo);
  //
  //(*MyOutput_) << "readfifo2" << std::endl;
  //
  int i=fifo+FIFO1;
  DEVTYPE devnum=(DEVTYPE)i;
  cmd[0]=VTX2_USR1;
  sndbuf[0]=FIFO_RD;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  sndbuf[0]=devnum-FIFO1+1; 
  devdo(MCTRL,6,cmd,3,sndbuf,rcvbuf,0); 
  cmd[0]=VTX2_USR1;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,2);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  //
  //(*MyOutput_) << "readfifo3" << std::endl;
  //
  cmd[0]=5;
  devdo(devnum,1,cmd,nrcvfifo*2,sndbuf,rcvfifo,2);
  //
  for(i=0;i<16380;i++)printf(" %d %04x ",i,((rcvbuf[2*i]<<8)&0xff00)|(rcvbuf[2*i+1]&0xff)); 
  //
  //(*MyOutput_) << "readfifo4" << std::endl;
  //
  cmd[0]=VTX2_USR1;
  sndbuf[0]=FIFO_RD;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;

  sndbuf[0]=0; 
  devdo(MCTRL,6,cmd,3,sndbuf,rcvbuf,0); 
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  //
  printf("readfifo: %d %02x %02x \n",nrcvfifo,rcvfifo[0]&0xff,rcvfifo[1]&0xff); 
  //
}

// DAQMB load and read flash memory (electronics experts only)

void DAQMB::buckflash_load(char *fshift)
{
  
  (*MyOutput_) << "inside load" <<std::endl;
  
 cmd[0]=0;
 devdo(BUCSHF,1,cmd,0,sndbuf,rcvbuf,0); // initialize programming
 for(int i=0;i<295;i++)sndbuf[i]=fshift[i];
 sndbuf[288]=0x01|sndbuf[288];
 sndbuf[289]=0x01|sndbuf[289];
 cmd[0]=1;
 devdo(BUCSHF,1,cmd,295*8,sndbuf,rcvbuf,0); // load buckeye pattern
 //Just to put some extra TMS low after the process for safety.
 //The exact number needed is 295.
}

void DAQMB::buckflash_read(char *rshift)
{
 cmd[0]=0;
 devdo(BUCSHF,1,cmd,0,sndbuf,rcvbuf,1); // initialize programming 
 cmd[0]=3;
 devdo(BUCSHF,1,cmd,0,sndbuf,rshift,1); 
 /* return 296 bits */
}

void DAQMB::buckflash_pflash()
{
 cmd[0]=0;
 devdo(BUCSHF,1,cmd,0,sndbuf,rcvbuf,0); //initialize the counter
 cmd[0]=2;
 devdo(BUCSHF,1,cmd,0,sndbuf,rcvbuf,1);
 // sndbuf[0]=0xff;
 // sndbuf[1]=0xff;
 // devdo(BUCSHF,-99,sndbuf,sndbuf,rcvbuf,0); 
}

void DAQMB::buckflash_init()
{
 cmd[0]=0;
 devdo(BUCSHF,1,cmd,0,sndbuf,rcvbuf,0); // initialize programming
 cmd[0]=4;
 devdo(BUCSHF,1,cmd,0,sndbuf,rcvbuf,2); 
 // sndbuf[0]=0xff;
 // sndbuf[1]=0x03;
 // devdo(BUCSHF,-99,sndbuf,0,sndbuf,rcvbuf,0);
}

void DAQMB::buckflash_erase()
{
  //
  cmd[0]=0;
  devdo(BUCSHF,1,cmd,0,sndbuf,rcvbuf,1); // erase Flash memory
  cmd[0]=5;
  devdo(BUCSHF,1,cmd,0,sndbuf,rcvbuf,1); 
  (*MyOutput_) << " Wait for 10 Seconds for Flash Memory to finish " << std::endl;
  sleep(10);  // 10 seconds are required after erase
  //
}


// DAQMB program proms


void DAQMB::epromload(DEVTYPE devnum,const char *downfile,int writ,char *cbrdnum)
{
  char snd[1024],expect[1024],rmask[1024],smask[1024],cmpbuf[1024];
  DEVTYPE devstp,dv;
  char *devstr;
  FILE *dwnfp,*fpout;
  char buf[8192],buf2[256];
  char *Word[256],*lastn;
  int Count,j,nbits,nbytes,pause,xtrbits;
  int tmp,cmpflag;
  int tstusr;
  int nowrit=0;
  // 
  (*MyOutput_) << " epromload " << std::endl;
  (*MyOutput_) << " devnum    " << devnum << std::endl;
  //
  if(devnum==ALL){
    devnum=F1PROM;
    devstp=F5PROM;
  }
  else {
    devstp=devnum;
  }
  //
#ifdef OSUcc
  theController->SetUseDelay(true);
#endif OSUcc
  //
  for(int i=devnum;i<=devstp;i++){
    dv=(DEVTYPE)i;
    xtrbits=geo[dv].sxtrbits;
    //    printf(" ************************** xtrbits %d geo[dv].sxtrbits %d \n",xtrbits,geo[dv].sxtrbits);
    devstr=geo[dv].nam;
    dwnfp    = fopen(downfile,"r");
    fpout=fopen("eprom.bit","w");
    //  printf("Programming Design %s (%s) with %s\n",design,devstr,downfile);
    //
    char bogobuf[8192];
    unsigned long int nlines=0;
    unsigned long int line=1;
    FILE *bogodwnfp=fopen(downfile,"r");
    while (fgets(bogobuf,256,bogodwnfp) != NULL)
      if (strrchr(bogobuf,';')!=0) nlines++;
    float percent;
    while (fgets(buf,256,dwnfp) != NULL)  {
      percent = (float)line/(float)nlines;
      printf("<   > Processed line %d of %d (%.1f%%)\r",line,nlines,percent*100.0);
      fflush(stdout);
      if((buf[0]=='/'&&buf[1]=='/')||buf[0]=='!'){
	//  printf("%s",buf);
      }
      else {
	line++;
	if(strrchr(buf,';')==0){
	  do {
	    lastn=strrchr(buf,'\n');
	    if(lastn!=0)lastn[0]='\0';
	    if (fgets(buf2,256,dwnfp) != NULL){
	      strcat(buf,buf2);
	    }
	    else {
	     //    printf("End of File encountered.  Quiting\n");
	      return;
	   }
	  }
	  while (strrchr(buf,';')==0);
	}
       for(int i=0;i<1024;i++){
	 cmpbuf[i]=0;
	 sndbuf[i]=0;
	 rcvbuf[i]=0;
       }
       Parse(buf, &Count, &(Word[0]));
       // count=count+1;
       if(strcmp(Word[0],"SDR")==0){
	 cmpflag=0;    //disable the comparison for no TDO SDR
	 sscanf(Word[1],"%d",&nbits);
	 nbytes=(nbits-1)/8+1;
	 for(int i=2;i<Count;i+=2){
	   if(strcmp(Word[i],"TDI")==0){
	     for(j=0;j<nbytes;j++){
	       sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",&snd[j]);
	     }
	     /*JRG, new selective way to download UNALTERED PromUserCode from SVF to
	       ANY prom:  just set cbrdnum[3,2,1,0]=0 in calling routine!
	       was  if(nowrit==1){  */
	     /*	     if(nowrit==1&&(cbrdnum[0]|cbrdnum[1]|cbrdnum[2]|cbrdnum[3])!=0){
	       tstusr=0;
	       snd[0]=cbrdnum[0];
	       snd[1]=cbrdnum[1];
	       snd[2]=cbrdnum[2]; 
	       snd[3]=cbrdnum[3];
	     */
	       // printf(" snd %02x %02x %02x %02x \n",snd[0],snd[1],snd[2],snd[3]);
	     //}
	     //}
	     if(nowrit==1&&cbrdnum[0]!=0) {
	       tstusr=0;
	       snd[0]=cbrdnum[0];
	     }
	     if(nowrit==1){
	       //  printf(" snd %02x %02x %02x %02x \n",snd[0],snd[1],snd[2],snd[3]);
	       //FOO[0]=((snd[3]&0x000000ff)<<24)|((snd[2]&0x000000ff)<<16)|((snd[1]&0x000000ff)<<8)|(snd[0]&0x000000ff);
               // printf(" FOO %08x \n",FOO[0]);
	     }
	   }
	   if(strcmp(Word[i],"SMASK")==0){
	     for(j=0;j<nbytes;j++){
                sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",&smask[j]);
	     }
	   }
	   if(strcmp(Word[i],"TDO")==0){
	     cmpflag=1;
	     for(j=0;j<nbytes;j++){
	       sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",&expect[j]);
              }
	   }
	   if(strcmp(Word[i],"MASK")==0){
	     for(j=0;j<nbytes;j++){
	       sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",&rmask[j]);
	     }
	   }
	 }
	 for(int i=0;i<nbytes;i++){
	   //sndbuf[i]=snd[i]&smask[i];
	   sndbuf[i]=snd[i]&0xff;
          }
	 //   printf("D%04d",nbits+xtrbits);
          // for(i=0;i<(nbits+xtrbits)/8+1;i++)printf("%02x",sndbuf[i]&0xff);printf("\n");
	 if(nowrit==0){
             if((geo[dv].jchan==12)){
	       //scan_reset(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
             }else{
                   scan(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
             }
	 }else{
	   if(writ==1) {
	     if((geo[dv].jchan==12)){
	       //scan_reset(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
	     }else{ 
	       scan(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
	     }
	   }
	 } 
	 //  Data readback comparison here:
	 for (int i=0;i<nbytes;i++) {
	   tmp=(rcvbuf[i]>>3)&0x1F;
	   rcvbuf[i]=tmp | (rcvbuf[i+1]<<5&0xE0);
	   /*  if (((rcvbuf[i]^expect[i]) & (rmask[i]))!=0 && cmpflag==1) 
		printf("read back wrong, at i %02d  rdbk %02X  expect %02X  rmask %02X\n",i,rcvbuf[i]&0xFF,expect[i]&0xFF,rmask[i]&0xFF); */
	 }
	 if (cmpflag==1) {
	   for (int i=0;i<nbytes;i++) {
	     fprintf(fpout," %02X",rcvbuf[i]&0xFF);
	     if (i%4==3) fprintf(fpout,"\n");
	   }
	 }
       }
       //
       else if(strcmp(Word[0],"SIR")==0){
	 nowrit=0;
	 sscanf(Word[1],"%d",&nbits);
          nbytes=(nbits-1)/8+1;
          for(int i=2;i<Count;i+=2){
            if(strcmp(Word[i],"TDI")==0){
              for(j=0;j<nbytes;j++){
                sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",&snd[j]);
              }
              if(nbytes==1){if(0xfd==(snd[0]&0xff))nowrit=1;} // nowrit=1  
            }
            else if(strcmp(Word[i],"SMASK")==0){
              for(j=0;j<nbytes;j++){
                sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",&smask[j]);
              }
            }
            if(strcmp(Word[i],"TDO")==0){
              for(j=0;j<nbytes;j++){
                sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",&expect[j]);
              }
            }
            else if(strcmp(Word[i],"MASK")==0){
              for(j=0;j<nbytes;j++){
                sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",&rmask[j]);
              }
            }
          }
          for(int i=0;i<nbytes;i++){
            //sndbuf[i]=snd[i]&smask[i];
            sndbuf[i]=snd[i];
          }
	  //   printf("I%04d",nbits);
          // for(i=0;i<nbits/8+1;i++)printf("%02x",sndbuf[i]&0xff);printf("\n");
	  /*JRG, brute-force way to download UNALTERED PromUserCode from SVF file to
	    DDU prom, but screws up CFEB/DMB program method:      nowrit=0;  */
          if(nowrit==0){
	    devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);}
          else{
            if(writ==1)devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);
            if(writ==0)printf(" ***************** nowrit %02x \n",sndbuf[0]);
          }
	  /*
          printf("send %2d instr bits %02X %02X %02X %02X %02X\n",nbits,sndbuf[4]&0xFF,sndbuf[3]&0xFF,sndbuf[2]&0xFF,sndbuf[1]&0xFF,sndbuf[0]&0xFF);
          printf("expect %2d instr bits %02X %02X %02X %02X %02X\n",nbits,expect[4]&0xFF,expect[3]&0xFF,expect[2]&0xFF,expect[1]&0xFF,expect[0]&0xFF);
	  */
        }
        else if(strcmp(Word[0],"RUNTEST")==0){
          sscanf(Word[1],"%d",&pause);
	  printf("RUNTEST = %d\n",pause);
	  //usleep(pause);
	  /*   ipd=83*pause;
          // sleep(1);
          t1=(double) clock()/(double) CLOCKS_PER_SEC;
          for(i=0;i<ipd;i++);
          t2=(double) clock()/(double) CLOCKS_PER_SEC;
	  //  if(pause>1000)printf("pause = %f s  while erasing\n",t2-t1); */
	  //          for (i=0;i<pause/100;i++)
	  //  devdo(dv,-1,sndbuf,0,sndbuf,rcvbuf,2);
          pause=pause/2;
          if (pause>65535) {
            sndbuf[0]=255;
            sndbuf[1]=255;
            for (int looppause=0;looppause<pause/65536;looppause++) devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,0);
            pause=65535;
	  }
          sndbuf[0]=pause-(pause/256)*256;
          sndbuf[1]=pause/256;
	  // printf(" sndbuf %d %d %d \n",sndbuf[1],sndbuf[0],pause);
          devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,2);
          // printf(" send sleep \n");  
	  /* printf("pause      %d us\n",pause);*/
	  //#ifdef OSUcc
	  //theController->flush_vme();
	  //#endif OSUcc
        }
        else if((strcmp(Word[0],"STATE")==0)&&(strcmp(Word[1],"RESET")==0)&&(strcmp(Word[2],"IDLE;")==0)){
	   printf("goto reset idle state\n"); 
	   devdo(dv,-1,sndbuf,0,sndbuf,rcvbuf,2);
	   //#ifdef OSUcc
	   //theController->flush_vme();
	   //#endif OSUcc
        }
       else if(strcmp(Word[0],"TRST")==0){
       }
       else if(strcmp(Word[0],"ENDIR")==0){
       }
       else if(strcmp(Word[0],"ENDDR")==0){
       }
      }
    }
    fclose(fpout);
    fclose(dwnfp);
  }
  //
  //#ifdef OSUcc
  //theController->flush_vme();
  //#endif OSUcc
  //
  theController->send_last();
  //
#ifdef OSUcc
  theController->SetUseDelay(false);
#endif OSUcc
  //
  //sndbuf[0]=0x01;
  //sndbuf[1]=0x00;
  // printf(" sndbuf %d %d %d \n",sndbuf[1],sndbuf[0],pause);
  //devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,1);
  //
}

void Parse(char *buf,int *Count,char **Word)
{

  *Word = buf;
  *Count = 0;
  while(*buf != '\0')  {
    while ((*buf==' ') || (*buf=='\t') || (*buf=='\n') || (*buf=='"')) *(buf++)='\0';
    if ((*buf != '\n') && (*buf != '\0'))  {
      Word[(*Count)++] = buf;
    }
    while ((*buf!=' ')&&(*buf!='\0')&&(*buf!='\n')&&(*buf!='\t')&&(*buf!='"')) {
      buf++;
    }
  }
  *buf = '\0';
}



void DAQMB::rdbkvirtexII()
{
char a[4],b[4];
#define clbword 2494 //(x32)
int i,k,totbits,totbytes;
char sndbuf2[36];
int ival;
  FILE *fp=fopen("rbk.dat","w");
 
  DEVTYPE dv=MCTRL;

  for(k=0;k<1;k++){
      cmd[0]=VTX2_CFG_IN;

      sndbuf[0]=0xFF;
      sndbuf[1]=0xFF;
      sndbuf[2]=0xFF;
      sndbuf[3]=0xFF;
      sndbuf[4]=0x55;
      sndbuf[5]=0x99;
      sndbuf[6]=0xAA;
      sndbuf[7]=0x66; 
     // write to FLR register
      sndbuf[8]=0x0C;
      sndbuf[9]=0x80;
      sndbuf[10]=0x06;
      sndbuf[11]=0x80;
      // fill frame length
      sndbuf[12]=0x00;
      sndbuf[13]=0x00;
      sndbuf[14]=0x00;
      sndbuf[15]=0x6a;  //AA 
      // write to CMD register
      sndbuf[16]=0x0C;
      sndbuf[17]=0x00;
      sndbuf[18]=0x01;
      sndbuf[19]=0x80;
      // RCRC
      sndbuf[20]=0x00;
      sndbuf[21]=0x00;
      sndbuf[22]=0x00;
      sndbuf[23]=0xe0;  
      //write to CMD register, READBACK command
      sndbuf[24]=0x0C;
      sndbuf[25]=0x00;
      sndbuf[26]=0x01;
      sndbuf[27]=0x80;
      //register value for RCFG
      sndbuf[28]=0x00;
      sndbuf[29]=0x00;
      sndbuf[30]=0x00;
      sndbuf[31]=0x20;
      //send to FAR
      sndbuf[32]=0x0C;
      sndbuf[33]=0x00;
      sndbuf[34]=0x04;
      sndbuf[35]=0x80;
      //starting CLB
      ival=0;
      a[3]=(ival&0x000000ff);
      a[2]=(ival&0x0000ff00)>>8;
      a[1]=(ival&0x00ff0000)>>16;
      a[0]=0x00;
      shuffle(a,b);
      for(i=0;i<4;i++)sndbuf[36+i]=b[3-i];
      //read from RDRO register
      sndbuf[40]=0x14;
      sndbuf[41]=0x00;
      sndbuf[42]=0x06;
      sndbuf[43]=0x00;
      //VTX CLB word counter xcv50 15876 words
      // xcv50 48003e04
      // xcv151 480076b0
      // xc2v500 480009be
      a[0]=0x48;
      a[1]=0x01;
      a[2]=0x37;
      a[3]=0xc0;
      shuffle(a,b); 
      for(i=0;i<4;i++)sndbuf[44+i]=b[3-i];
      /*  sndbuf[44]=0x12;
      sndbuf[45]=0x80; //00;
      sndbuf[46]=0x93; //00;
      sndbuf[47]=0x51; //96; */
      //flush pipe
      sndbuf[48]=0x00;
      sndbuf[49]=0x00;
      sndbuf[50]=0x00;
      sndbuf[51]=0x00;
      //durkin added more flush pipe
      sndbuf[52]=0x00;
      sndbuf[53]=0x00;
      sndbuf[54]=0x00;
      sndbuf[55]=0x00;
 
      for(i=0;i<52;i++)sndbuf2[i]=sndbuf[i+4]; 
      devdo(dv,6,cmd,416,sndbuf2,rcvbuf,0);
      cmd[0]=VTX2_CFG_OUT;
        totbytes=4*clbword;
        totbits=8*totbytes;
        devdo(dv,6,cmd,2752,sndbuf,rcvbuf,1);
	/*  bits=0;
    for(i=0;i<totbytes;i++){
      for(j=0;j<8;j++) { 
        if (bits>0) fprintf(fp,"%d",(rtn_ptr[i]>>j)&1);
        bits++;
        if (bits>2 && (bits-3)%32==0) fprintf(fp,"\n"); 
        if (bits==totbits) fprintf(fp,"0\n");
      }
      } */
    }
    for(k=0;k<928;k++){
      cmd[0]=VTX2_CFG_IN;

      sndbuf[0]=0xFF;
      sndbuf[1]=0xFF;
      sndbuf[2]=0xFF;
      sndbuf[3]=0xFF;
      sndbuf[4]=0x55;
      sndbuf[5]=0x99;
      sndbuf[6]=0xAA;
      sndbuf[7]=0x66; 
     // write to FLR register
      sndbuf[8]=0x0C;
      sndbuf[9]=0x80;
      sndbuf[10]=0x06;
      sndbuf[11]=0x80;
      // fill frame length
      sndbuf[12]=0x00;
      sndbuf[13]=0x00;
      sndbuf[14]=0x00;
      sndbuf[15]=0x6a; 
      // write to CMD register
      sndbuf[16]=0x0C;
      sndbuf[17]=0x00;
      sndbuf[18]=0x01;
      sndbuf[19]=0x80;
      // RCRC
      sndbuf[20]=0x00;
      sndbuf[21]=0x00;
      sndbuf[22]=0x00;
      sndbuf[23]=0xe0;  
      //write to CMD register, READBACK command
      sndbuf[24]=0x0C;
      sndbuf[25]=0x00;
      sndbuf[26]=0x01;
      sndbuf[27]=0x80;
      //register value for RCFG
      sndbuf[28]=0x00;
      sndbuf[29]=0x00;
      sndbuf[30]=0x00;
      sndbuf[31]=0x20;
      //read from RDRO register
      sndbuf[32]=0x14;
      sndbuf[33]=0x00;
      sndbuf[34]=0x06;
      sndbuf[35]=0x00;
      //VTX CLB word counter xcv50 15876 words
      // xcv50 48003e04
      // xcv151 480076b0
      // xc2v500 480009be
      a[0]=0x48;
      a[1]=0x01;
      a[2]=0x37;
      a[3]=0xc0;
      shuffle(a,b); 
      for(i=0;i<4;i++)sndbuf[36+i]=b[3-i];

      //flush pipe
      sndbuf[40]=0x00;
      sndbuf[41]=0x00;
      sndbuf[42]=0x00;
      sndbuf[43]=0x00;
      //durkin added more flush pipe
      sndbuf[44]=0x00;
      sndbuf[45]=0x00;
      sndbuf[46]=0x00;
      sndbuf[47]=0x00;
 
      for(i=0;i<40;i++)sndbuf2[i]=sndbuf[i+8]; 
      devdo(dv,6,cmd,320,sndbuf2,rcvbuf,0);
      cmd[0]=VTX2_CFG_OUT;
        totbytes=4*clbword;
        totbits=8*totbytes;
        devdo(dv,6,cmd,2752,sndbuf,rcvbuf,1);
        for(i=0;i<344;i++)fprintf(fp,"%02x",rcvbuf[i]&0xff);fprintf(fp,"\n");
    cmd[0]=VTX2_BYPASS;
    devdo(dv,6,cmd,0,sndbuf,rcvbuf,0);
    
  }
 fclose(fp);
return;
}

void shuffle(char *a,char *b)
{
int i,j,k;
char msk;
for(k=0;k<4;k++)b[k]=0x00;
for(i=0;i<4;i++){
  j=3-i;
  for(k=0;k<8;k++){
    msk=(0x01<<k);
    msk=msk&a[i];
    if(msk!=0x00)b[j]=b[j]|(0x80>>k);
  }
}
}


void DAQMB::rdbkvirtex(DEVTYPE devnum)  //FEB FPGA 1-5 or  F1SCAM->F5SCAM
{
const int clbword2 = 15876;
//const int bramword = 780;   // That is just for XCV50
int i,j,totbits,totbytes,bits;
char sndbuf2[36];
int ibstr,ibstr2,ival,imod,imod2,ival2;
char cval;
  FILE * fp=fopen("rbk.dat","w");
 
  DEVTYPE dv=devnum;

      cmd[0]=VTX_CFG_IN;

      sndbuf[0]=0x00;
      sndbuf[1]=0x00;
      sndbuf[2]=0x00;
      sndbuf[3]=0x00;
      sndbuf[4]=0x55;
      sndbuf[5]=0x99;
      sndbuf[6]=0xAA;
      sndbuf[7]=0x66;
      //send to FAR
      sndbuf[8]=0x0C;
      sndbuf[9]=0x00;
      sndbuf[10]=0x04;
      sndbuf[11]=0x80;
      //starting CLB
      sndbuf[12]=0x00;
      sndbuf[13]=0x00;
      sndbuf[14]=0x00;
      sndbuf[15]=0x00;
      //write to CMD register, READBACK command
      sndbuf[16]=0x0C;
      sndbuf[17]=0x00;
      sndbuf[18]=0x01;
      sndbuf[19]=0x80;
      //register value for RCFG
      sndbuf[20]=0x00;
      sndbuf[21]=0x00;
      sndbuf[22]=0x00;
      sndbuf[23]=0x20;
      //read from RDRO register
      sndbuf[24]=0x14;
      sndbuf[25]=0x00;
      sndbuf[26]=0x06;
      sndbuf[27]=0x00;
      //VTX CLB word counter xcv50 15876 words
      // xcv50 48003e04
      // xcv151 480076b0
      sndbuf[28]=0x12;
      sndbuf[29]=0x00;
      sndbuf[30]=0x7C;
      sndbuf[31]=0x20;
      //flush pipe
      sndbuf[32]=0x00;
      sndbuf[33]=0x00;
      sndbuf[34]=0x00;
      sndbuf[35]=0x00;
      // bits have to be shifed for 3 bit delay
      for(i=0;i<36;i++)sndbuf2[i]=0x00;
      ibstr=3;
      ibstr2=0;
LOOP:     
      imod=ibstr-8*(ibstr/8);
      ival=ibstr/8;
      imod2=ibstr2-8*(ibstr2/8);
      ival2=ibstr2/8;
      cval=((sndbuf[ival]>>imod)&0x01);
      if(cval==0x01){
          sndbuf2[ival2]=sndbuf2[ival2]+int(pow(2.0,imod2));
      }
      ibstr=ibstr+1;
      ibstr2=ibstr2+1;
      if(ibstr<288)goto LOOP;
      //   for(i=0;i<10;i++)printf(" %d %02x \n",i,sndbuf2[i]);
      devdo(dv,5,cmd,285,sndbuf2,rcvbuf,0);
      cmd[0]=VTX_CFG_OUT;
      devdo(dv,5,cmd,0,sndbuf,rcvbuf,0);
      totbytes=4*clbword2;
      totbits=8*totbytes;
      char * data_ptr = new char;
      char * rtn_ptr = new char;
      // data_ptr=(char *)malloc((clbword2*4+1),sizeof(char));
      // rtn_ptr=(char *)malloc((clbword2*4+1),sizeof(char));
      //@@ looks wrong!
      scan(DATA_REG,data_ptr,totbits+2,rtn_ptr,1);
      
      bits=0;
      for(i=0;i<totbytes;i++){
	for(j=0;j<8;j++) { 
	  if (bits>0) fprintf(fp,"%d",(rtn_ptr[i]>>j)&1);
	  bits++;
	  if (bits>2 && (bits-3)%32==0) fprintf(fp,"\n"); 
	  if (bits==totbits) fprintf(fp,"0\n");
	}
      }
      delete data_ptr;
      delete rtn_ptr;
      //  printf("\n Total bits read %d \n",totbits);
      cmd[0]=VTX_BYPASS;
      devdo(dv,5,cmd,0,sndbuf,rcvbuf,0);
      
      // free(rtn_ptr);
      // free(data_ptr);
      
      //  printf(" SCA Master Configure file is read back. \n\n");
      fclose(fp);
}


void DAQMB::shift_all(int mode) {
  for(int brd=0;brd<5;brd++){
    for(int chip=0;chip<6;chip++){
      for(int ch=0;ch<16;ch++){
        shift_array[brd][chip][ch]=mode;
      }
    }
  }
}


unsigned long int DAQMB::unpack_ibrd() const {
  int ibrd=0x00000000;
  return ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)
      |((rcvbuf[3]&0xff)<<24)|ibrd;
}


unsigned int DAQMB::unpack_ival() const {
  return ((rcvbuf[1]<<8)&0xff00)|(rcvbuf[0]&0xff);
}


void DAQMB::executeCommand(std::string command) {
  #ifdef debugV
    (*MyOutput_) << "inside DAQMB executeCommand"<< std::endl;
  #endif
  if(command == "BuckeyeShiftTest")  buck_shift_test();
/*
  if(command == "fifo")                fifo();
  if(command == "dump")                dump();
  if(command == "bucktest")            bucktest();
  if(command == "daqmb_init")          daqmb_init();
  if(command == "cfeb_pulse")          cfeb_pulse();
  if(command == "cfeb_inject")         cfeb_inject();
  if(command == "cfeb_pedestals")      cfeb_pedestals();
  if(command == "lowv_dump")           lowv_dump();
  if(command == "daqmb_adc_dump")      daqmb_adc_dump();
  if(command == "daqmb_promfpga_dump") daqmb_promfpga_dump();
*/
}



void DAQMB::cfeb_vtx_prom(enum DEVTYPE devnum) {
  //enum DEVTYPE devstp,dv;
  (*MyOutput_) << "DAQMB: cfeb_vtx_prom" << std::endl;

  cmd[0]=VTX2_USR1;
  sndbuf[0]=CAL_PROGFEB;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
      
  cmd[0]=VTX2_USR2;
  sndbuf[0]=0xFF;
  devdo(MCTRL,6,cmd,4,sndbuf,rcvbuf,0);
 
  cmd[0]=VTX2_USR1;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);

  cmd[0]=VTX2_USR1;
  sndbuf[0]=CAL_PROGFEB;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
      
  cmd[0]=VTX2_USR2;
  sndbuf[0]=0x00;
  devdo(MCTRL,6,cmd,4,sndbuf,rcvbuf,0);

  cmd[0]=VTX2_USR1;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);

  cmd[0]=VTX2_USR1;
  sndbuf[0]=CAL_PROGFEB;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
      
  cmd[0]=VTX2_USR2;
  sndbuf[0]=0xFF;
  devdo(MCTRL,6,cmd,4,sndbuf,rcvbuf,0);
 
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
 
  (*MyOutput_) << "DAQMB: SCA Master is programmed by PROM (Calcntrl command)." <<std::endl;
}

void DAQMB::febpromuser2(const CFEB & cfeb,char *cbrdnum)
{
int i;
    DEVTYPE dv = cfeb.promDevice();
      cmd[0]=PROM_USERCODE;
      sndbuf[0]=0xFF;
      sndbuf[1]=0xFF;
      sndbuf[2]=0xFF;
      sndbuf[3]=0xFF;
      sndbuf[4]=0xFF;
      devdo(dv,8,cmd,33,sndbuf,rcvbuf,1);
      rcvbuf[0]=((rcvbuf[0]>>1)&0x7f)+((rcvbuf[1]<<7)&0x80);
      rcvbuf[1]=((rcvbuf[1]>>1)&0x7f)+((rcvbuf[2]<<7)&0x80);
      rcvbuf[2]=((rcvbuf[2]>>1)&0x7f)+((rcvbuf[3]<<7)&0x80);
      rcvbuf[3]=((rcvbuf[3]>>1)&0x7f)+((rcvbuf[4]<<7)&0x80);
      printf(" The ISPROM USERCODE is %02x%02x%02x%02x  \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
      cbrdnum[0]=rcvbuf[0];
      cbrdnum[1]=rcvbuf[1];
      cbrdnum[2]=rcvbuf[2];
      cbrdnum[3]=rcvbuf[3];
      cmd[0]=PROM_BYPASS;
      sndbuf[0]=0;
      devdo(dv,8,cmd,0,sndbuf,rcvbuf,1);
}

void DAQMB::toggle_caltrg()
{
  cmd[0]=VTX2_USR1;
  sndbuf[0]=11;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  printf("Toggled Cal_trigger_generation_disable in MCTRL FPGA\n");
}

void DAQMB::set_ext_chanx(int schan)
{
  for(int brd=0;brd<5;brd++){
    for(int chip=0;chip<6;chip++){
      for(int ch=0;ch<16;ch++){
        shift_array[brd][chip][ch]=NORM_RUN;
      }
      shift_array[brd][chip][schan]=EXT_CAP;
    }
  }
}


void DAQMB::setpulsedelay(int tinj){
  int cal_delay_bits;
  if(tinj==-1){
     cal_delay_bits = (calibration_LCT_delay_ & 0xF)
      | (calibration_l1acc_delay_ & 0x1F) << 4
      | (pulse_delay_ & 0x1F) << 9
      | (inject_delay_ & 0x1F) << 14;
  }else{
     cal_delay_bits = (calibration_LCT_delay_ & 0xF)
      | (calibration_l1acc_delay_ & 0x1F) << 4
      | (tinj & 0x1F) << 9
      | (inject_delay_ & 0x1F) << 14;
  }
   cout << "DAQMB:configure: caldelay " << hex << cal_delay_bits << dec << endl;
   setcaldelay(cal_delay_bits);
}

void DAQMB::toggle_rndmtrg_start()
{
  cmd[0]=VTX2_USR1;
  sndbuf[0]=RTRG_TGL;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);

  cmd[0]=VTX2_USR2;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX2_USR1;
  sndbuf[0]=0;   // NO_OP mode
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,1);
  printf("Toggled RandomTrigger Start in MCTRL FPGA, RTRG_TGL %02x\n",RTRG_TGL);
}
//
void DAQMB::burst_rndmtrg()
{
  toggle_rndmtrg_start();
  usleep(5000);
  toggle_rndmtrg_start();
}
//
void DAQMB::sfm_test_load(char *sndpat)
{    
  //Program SFM with 0,1,2,3,...,263
  cmd[0]=VTX2_USR1;
  sndbuf[0]=35;   //Serial Flash Memory TEST
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  sndbuf[0]=0x41;  //bit reversed for 82H
  sndbuf[1]=0xc0;
  sndbuf[2]=0x3c;
  sndbuf[3]=0x00;
  for (int i=0;i<264;i++) sndbuf[i+4]=sndpat[i];
  devdo(MCTRL,6,cmd,2144,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,1);
}
//
void DAQMB::sfm_test_read(char *rcvpat)
{
  int boffset;
  //Read SFM 
  cmd[0]=VTX2_USR1;
  sndbuf[0]=35;   //Serial Flash Memory TEST
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  sndbuf[0]=0x4B;  //bit reversed for D2H
  sndbuf[0]=0x4A;  //bit reversed for 52H
  sndbuf[1]=0xc0;
  sndbuf[2]=0x3c;
  sndbuf[3]=0x00;
  for (int i=0;i<268;i++) sndbuf[i+4]=0;  //264 data bytes plus 4 dummy bytes
  devdo(MCTRL,6,cmd,2176,sndbuf,rcvbuf,1);
  boffset=2;
  for(int i=0;i<256;i++){
    // printf("shfttst %d %02X \n",i,rcvbuf[i]&0xFF);
    rcvpat[i]=0x00;
    rcvpat[i]=rcvpat[i]|(rcvbuf[i+9]<<(8-boffset));
    rcvpat[i]=rcvpat[i]|((rcvbuf[i+8]>>boffset)&~(~0<<(8-boffset)));
  }
  cmd[0]=VTX2_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,1);
}
//

void DAQMB::cbldly_init(){
	 printf(" Initialize \n");
         cmd[0]=VTX_USR1; 
         sndbuf[0]=0x1A;
         devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
         cmd[0]=VTX_USR1;
         sndbuf[0]=NOOP;
         devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
         cmd[0]=VTX_BYPASS;
         devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
}

void DAQMB::trigset2(int nset, int iuse[5])
{
  int i,j,k;
  char tsndbuf[512];
  //
  //  printf(" Loading Motherboard trigger register...");
  /* load trigger pattern */
  //
  for(i=0;i<512;i++){
    tsndbuf[i]=0;
  }
  //
  /*
  if(nset<2)tsndbuf[4]=0x03;
  if(nset<3)tsndbuf[10]=0x03;
  if(nset<4)tsndbuf[44]=0x03;
  */
  if (nset>0) tsndbuf[4]=0x03;
  if (nset>1) tsndbuf[7]=0x03;
  //
  //  if (nset>1) tsndbuf[40]=0x03;
  //  if (nset>1) tsndbuf[60]=0x03;
  //
  if (nset>2) tsndbuf[44]=0x03;
  //
  cmd[0]=VTX2_USR1;
  sndbuf[0]=LOAD_TRIG;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0); 
  for(i=0;i<512;i++)sndbuf[i]=tsndbuf[i]&0xFF;
  cmd[0]=VTX2_USR2;
  devdo(MCTRL,6,cmd,4096,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=LOAD_STR;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  j=0;
  k=1;
  for(i=0;i<5;i++){if(iuse[i]==1)j=j+k;k=k*2;}
  //  printf(" here is what we set %d \n",j);
  sndbuf[0]=j;
  devdo(MCTRL,6,cmd,5,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  //
}


void DAQMB::trgfire()
{
  cmd[0]=VTX2_USR1;
  sndbuf[0]=CYCLE_TRIG;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,1);
  /*    printf(" Returned from CYCLYE_TRIG instr: %02X  \n",rcvbuf[0]&0xFF); */
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,1);
  /*  printf(" Returned from NOOP instr: %02X  \n",rcvbuf[0]&0xFF);*/
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
}

void DAQMB::cbldly_trig(){
          printf(" Trigger Once \n");
         cmd[0]=VTX_USR1; 
         sndbuf[0]=0x03;
         devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
         cmd[0]=VTX_USR1;
         sndbuf[0]=NOOP;
         devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
         cmd[0]=VTX_BYPASS;
         devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
}

void DAQMB::cbldly_phaseA(){
         printf(" Detect Phase A \n");
         cmd[0]=VTX_USR1; 
         sndbuf[0]=0x1B;
         devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
         cmd[0]=VTX_USR1;
         sndbuf[0]=NOOP;
         devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
         cmd[0]=VTX_BYPASS;
         devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
}

void DAQMB::cbldly_phaseB(){
         printf(" Detect Phase B \n");
         cmd[0]=VTX_USR1; 
         sndbuf[0]=0x1C;
         devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
         cmd[0]=VTX_USR1;
         sndbuf[0]=NOOP;
         devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
         cmd[0]=VTX_BYPASS;
         devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
}

void DAQMB::cbldly_loadfinedelay(){
  //
  printf(" Load Fine Delay \n");
  cmd[0]=VTX_USR1; 
  sndbuf[0]=0x15;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  //
}

void DAQMB::cbldly_programSFM(){
  (*MyOutput_) <<" Program Serial Flash Memory" << std::endl ;
  cmd[0]=VTX_USR1; 
  sndbuf[0]=0x18;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
}

void DAQMB::cbldly_wrtprotectSFM(){
  (*MyOutput_) << " SFM Write Protect" <<std::endl;
  cmd[0]=VTX_USR1; 
  sndbuf[0]=0x1e; 
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
}

void DAQMB::cbldly_loadmbidSFM(){
	 printf(" Load DAQMB ID to SFM  \n");
         cmd[0]=VTX_USR1; 
         sndbuf[0]=0x16;
         devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
         cmd[0]=VTX_USR1;
         sndbuf[0]=NOOP;
         devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
         cmd[0]=VTX_BYPASS;
         devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
}

void DAQMB::cbldly_loadcfebdlySFM(){
	 printf(" Load CFEB clock delay to SFM \n"); 
         cmd[0]=VTX_USR1; 
         sndbuf[0]=0x17;
         devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
         cmd[0]=VTX_USR1;
         sndbuf[0]=NOOP;
         devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
         cmd[0]=VTX_BYPASS;
         devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
}

void DAQMB::cbldly_refreshcfebdly(){
  printf(" Refresh Onboard CFEB delay \n");
  cmd[0]=VTX_USR1; 
  sndbuf[0]=0x1d;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
}

//
void DAQMB::devdoReset(){
/// used for emergency loading
  devdo(RESET,-1,cmd,0,sndbuf,rcvbuf,2);
}
//
std::string DAQMB::CounterName(int counter){
  //
  std::string name = "NO counter found" ;
  //
  if ( counter == 0 ) name = "L1A to LCT delay";
  if ( counter == 1 ) name = "CFEB DAV delay  ";
  if ( counter == 2 ) name = "TMB DAV delay   ";
  if ( counter == 3 ) name = "ALCT DAV delay  ";
  if ( counter == 4 ) name = "L1A to LCT Scope";
  if ( counter == 5 ) name = "CFEB DAV Scope  ";
  if ( counter == 6 ) name = "TMB DAV Scope   ";
  if ( counter == 7 ) name = "ALCT DAV Scope  ";
  //
  return name;
  //
}
//
void DAQMB::PrintCounters(){
  //
  readtimingCounter();
  //
  readtimingScope();
  //
  printf("  L1A to LCT delay: %d", GetL1aLctCounter()  ); printf(" CMS clock cycles \n");
  printf("  CFEB DAV delay:   %d", GetCfebDavCounter() ); printf(" CMS clock cycles \n");
  printf("  TMB DAV delay:    %d", GetTmbDavCounter()  ); printf(" CMS clock cycles \n");
  printf("  ALCT DAV delay:   %d", GetAlctDavCounter() ); printf(" CMS clock cycles \n");
  //
  cout << endl ;
  //
  cout << "  L1A to LCT Scope: " ;
  cout << setw(3) << GetL1aLctScope() << " " ;
  for( int i=4; i>-1; i--) cout << ((GetL1aLctScope()>>i)&0x1) ;
  cout << endl ;
  //
  cout << "  CFEB DAV Scope:   " ;
  cout << setw(3) << GetCfebDavScope() << " " ;
  for( int i=4; i>-1; i--) cout << ((GetCfebDavScope()>>i)&0x1) ;
  cout << endl ;
  //
  cout << "  TMB DAV Scope:    " ;
  cout << setw(3) << GetTmbDavScope() << " " ;
  for( int i=4; i>-1; i--) cout << ((GetTmbDavScope()>>i)&0x1) ;
  cout << endl ;
  //
  cout << "  ALCT DAV Scope:   " ;
  cout << setw(3) << GetAlctDavScope() << " " ;
  for( int i=4; i>-1; i--) cout << ((GetAlctDavScope()>>i)&0x1) ;
  cout << endl ;
  //
  cout << "  Active DAV Scope: " ;
  cout << setw(3) << GetActiveDavScope() << " " ;
  for( int i=4; i>-1; i--) cout << ((GetActiveDavScope()>>i)&0x1) ;
  cout << endl ;
  //
}

void DAQMB::readtimingCounter()
{
  //printf(" Entered READ_TIMING \n");
  //
  cmd[0]=VTX2_USR1;
  sndbuf[0]=CAL_STATUS;
  sndbuf[0]=36;      //F36 in DMB6cntl, July 5, 2005
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  //
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  //
  cmd[0]=VTX2_USR2;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(MCTRL,6,cmd,32,sndbuf,rcvbuf,1);
  //
  l1a_lct_counter_  = rcvbuf[0]&0xff ;
  cfeb_dav_counter_ = rcvbuf[1]&0xff ;
  tmb_dav_counter_  = rcvbuf[2]&0xff ;
  alct_dav_counter_ = rcvbuf[3]&0xff ;
  //
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  //
}

void DAQMB::readtimingScope()
{
  //printf(" Entered READ_TIMING \n");
  //
  cmd[0]=VTX2_USR1;
  sndbuf[0]=CAL_STATUS;
  sndbuf[0]=38;      //F38
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  //
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  //
  cmd[0]=VTX2_USR2;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(MCTRL,6,cmd,32,sndbuf,rcvbuf,1);
  //
  cfeb_dav_scope_   = rcvbuf[0]&0x1f ;
  tmb_dav_scope_    = ((rcvbuf[1]<<3)&0x18) + ((rcvbuf[0]>>5)&0x07);
  alct_dav_scope_   = (rcvbuf[1]>>2)&0x1f;
  active_dav_scope_ = ((rcvbuf[2]<<1)&0x1e)+((rcvbuf[1]>>7)&0x01);  
  l1a_lct_scope_    = ((rcvbuf[3]<<4)&0x10) + ((rcvbuf[2]>>4)&0x0f);
  //
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  //
}
//
void DAQMB::lowv_dump()
{
  (*MyOutput_) << " CFEB1 OCM 3 = "  << lowv_adc(1,0)/1000 << std::endl;
  (*MyOutput_) << " CFEB1 OCM 5 = "  << lowv_adc(1,1)/1000 << std::endl;
  (*MyOutput_) << " CFEB1 OCM 6 = "  << lowv_adc(1,2)/1000 << std::endl;
  (*MyOutput_) << " CFEB2 OCM 3 = "  << lowv_adc(1,3)/1000 << std::endl;
  (*MyOutput_) << " CFEB2 OCM 5 = "  << lowv_adc(1,4)/1000 << std::endl;
  (*MyOutput_) << " CFEB2 OCM 6 = "  << lowv_adc(1,5)/1000 << std::endl;
  (*MyOutput_) << " CFEB3 OCM 3 = "  << lowv_adc(1,6)/1000 << std::endl;
  (*MyOutput_) << " CFEB3 OCM 5 = "  << lowv_adc(1,7)/1000 << std::endl;
  (*MyOutput_) << " CFEB3 OCM 6 = "  << lowv_adc(2,0)/1000 << std::endl;
  (*MyOutput_) << " CFEB4 OCM 3 = "  << lowv_adc(2,1)/1000 << std::endl;
  (*MyOutput_) << " CFEB4 OCM 5 = "  << lowv_adc(2,2)/1000 << std::endl;
  (*MyOutput_) << " CFEB4 OCM 6 = "  << lowv_adc(2,3)/1000 << std::endl;
  (*MyOutput_) << " CFEB5 OCM 3 = "  << lowv_adc(2,4)/1000 << std::endl;
  (*MyOutput_) << " CFEB5 OCM 5 = "  << lowv_adc(2,5)/1000 << std::endl;
  (*MyOutput_) << " CFEB5 OCM 6 = "  << lowv_adc(2,6)/1000 << std::endl;
  (*MyOutput_) << " ALCT OCM 3.3 = " << lowv_adc(2,7)/1000 << std::endl;
  (*MyOutput_) << " ALCT OCM 1.8 = " << lowv_adc(3,0)/1000 << std::endl;
  (*MyOutput_) << " ALCT OCM 5B = "  << lowv_adc(3,1)/1000 << std::endl;
  (*MyOutput_) << " ALCT OCM 5A = "  << lowv_adc(3,2)/1000 << std::endl;
  (*MyOutput_) << " CFEB1 3.3V = "   << lowv_adc(3,3)/1000 << std::endl;
  (*MyOutput_) << " CFEB1 5.0V = "   << lowv_adc(3,4)/1000 << std::endl;
  (*MyOutput_) << " CFEB1 6.0V = "   << lowv_adc(3,5)/1000 << std::endl;
  (*MyOutput_) << " CFEB2 3.3V = "   << lowv_adc(3,6)/1000 << std::endl;
  (*MyOutput_) << " CFEB2 5.0V = "   << lowv_adc(3,7)/1000 << std::endl;
  (*MyOutput_) << " CFEB2 6.0V = "   << lowv_adc(4,0)/1000 << std::endl;
  (*MyOutput_) << " CFEB3 3.3V = "   << lowv_adc(4,1)/1000 << std::endl;
  (*MyOutput_) << " CFEB3 5.0V = "   << lowv_adc(4,2)/1000 << std::endl;
  (*MyOutput_) << " CFEB3 6.0V = "   << lowv_adc(4,3)/1000 << std::endl;
  (*MyOutput_) << " CFEB4 3.3V = "   << lowv_adc(4,4)/1000 << std::endl;
  (*MyOutput_) << " CFEB4 5.0V = "   << lowv_adc(4,5)/1000 << std::endl;
  (*MyOutput_) << " CFEB4 6.0V = "   << lowv_adc(4,6)/1000 << std::endl;
  (*MyOutput_) << " CFEB5 3.3V = "   << lowv_adc(4,7)/1000 << std::endl;
  (*MyOutput_) << " CFEB5 5.0V = "   << lowv_adc(5,0)/1000 << std::endl;
  (*MyOutput_) << " CFEB5 6.0V = "   << lowv_adc(5,1)/1000 << std::endl;
  (*MyOutput_) << " ALCT 3.3V = "    << lowv_adc(5,2)/1000 << std::endl;
  (*MyOutput_) << " ALCT 1.8V = "    << lowv_adc(5,3)/1000 << std::endl;
  (*MyOutput_) << " ALCT 5.5V B = "  << lowv_adc(5,4)/1000 << std::endl;
  (*MyOutput_) << " ALCT 5.5V A = "  << lowv_adc(5,5)/1000 << std::endl;
}




/*********** DAQMB ADC dump ***************/

void DAQMB::daqmb_adc_dump()
{ 
  (*MyOutput_) << " DAC1 Volts = "  << adcplus(2,0) << std::endl;
  (*MyOutput_) << " DAC2 Volts = "  << adcplus(2,1) << std::endl;
  (*MyOutput_) << " DAC3 Volts = "  << adcplus(2,2) << std::endl;
  (*MyOutput_) << " DAC4 Volts = "  << adcplus(2,3) << std::endl;
  (*MyOutput_) << " DAC5 Volts = "  << adcplus(2,4) << std::endl;
  (*MyOutput_) << " 1.8V Chip 1 = " << adcplus(1,6) << std::endl;
  (*MyOutput_) << " 1.8V Chip 2 = " << adcplus(2,6) << std::endl;
  (*MyOutput_) << " 1.8V Chip 3 = " << adcminus(3,6) << std::endl;
  (*MyOutput_) << " DMB GND = "     << adcminus(3,0) << std::endl;
  (*MyOutput_) << " CFEB1 GND = "   << adcminus(3,1) << std::endl;
  (*MyOutput_) << " CFEB2 GND = "   << adcminus(3,2) << std::endl;
  (*MyOutput_) << " CFEB3 GND = "   << adcminus(3,3) << std::endl;
  (*MyOutput_) << " CFEB4 GND = "   << adcminus(3,4) << std::endl;
  (*MyOutput_) << " CFEB5 GND = "   << adcminus(3,5) << std::endl;
  (*MyOutput_) << " Internal Calib. DAC = "  << adcplus(2,7) << std::endl;
  (*MyOutput_) << " External Calib. DAC = "  << adcminus(3,7) << std::endl; 
  (*MyOutput_) << " Precision Calib. DAC = " << adc16(4,0) << std::endl;
  (*MyOutput_) << " DAQMB temperature = "    << readthermx(0) << std::endl;
  (*MyOutput_) << " FEB1 temperature = "     << readthermx(1) << std::endl;
  (*MyOutput_) << " FEB2 temperature = "     << readthermx(2) << std::endl;
  (*MyOutput_) << " FEB3 temperature = "     << readthermx(3) << std::endl;
  (*MyOutput_) << " FEB4 temperature = "     << readthermx(4) << std::endl;
  (*MyOutput_) << " FEB5 temperature = "     << readthermx(5) << std::endl; 
}
 

/*********** DAQMB PROM/FPGA dumps ***************/

void DAQMB::daqmb_promfpga_dump()
{
  (*MyOutput_) << "DMB VPROM usercodes = " << std::hex << mbpromuser(0) << std::endl;
  (*MyOutput_) << "DMB VPROM ID codes = "  << std::hex << mbpromid(0) << std::endl;
  (*MyOutput_) << "DMB MPROM usercodes = " << std::hex << mbpromuser(1) << std::endl;
  (*MyOutput_) << "DMB MPROM ID codes = "       << std::hex << mbpromid(1) << std::endl;
  (*MyOutput_) << "DMB Virtex usercodes = "     << std::hex << mbfpgauser() << std::endl;
  (*MyOutput_) << "DMB Virtex Chip ID codes = " << std::hex << mbfpgaid() << std::endl;
  /*
  std::vector<CFEB> cfebs = cfebs();
  for(unsigned i = 0; i < cfebs.size(); ++i) {
    (*MyOutput_) << " CFEB FPGA usercodes = " << std::hex << febpromuser(cfebs[i]) << std::endl;
    (*MyOutput_) << " CFEB ISPROM usercodes = " << std::hex << febfpgauser(cfebs[i]) << std::endl;
    (*MyOutput_) << " CFEB FPGA Chip ID code = " << std::hex << febpromid(cfebs[i]) << std::endl;
    (*MyOutput_) << " CFEB ISPROM Chip ID code = " << std::hex << febfpgaid(cfebs[i]) << std::endl;
  }
  */
  (*MyOutput_) << std::dec;
}
//
void DAQMB::ProgramSFM(){
  //
  (*MyOutput_) << " Program Serial Flash Memory" << std::endl;
  cmd[0]=VTX_USR1; 
  sndbuf[0]=0x18;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  //
}
//
void DAQMB::LoadCFEBDelaySFM(){
  //
  printf(" Load CFEB clock delay to SFM \n"); 
  cmd[0]=VTX_USR1; 
  sndbuf[0]=0x17;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  //
}
//
void DAQMB::LoadDMBIdSFM(){
  //
  printf(" Load DAQMB ID to SFM  \n");
  cmd[0]=VTX_USR1; 
  sndbuf[0]=0x16;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  //
}
//
void DAQMB::SFMWriteProtect(){
  //
  (*MyOutput_) << " SFM Write Protect" << std::endl;
  cmd[0]=VTX_USR1; 
  sndbuf[0]=0x1e; 
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  //
}
//
void DAQMB::ToogleBXN(){
  //
  cmd[0]=VTX_USR1; 
  sndbuf[0]=34; 
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  //
}
//
void DAQMB::LoadCableDelaySFM()
{
  printf(" Load Cable delay \n");
  cmd[0]=VTX_USR1; 
  sndbuf[0]=0x15;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);  
}
//
void DAQMB::setcbldly(int dword)
{
  //
  cmd[0]=VTX2_USR1;
  sndbuf[0]=28;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  sndbuf[0]=dword&0XFF; 
  //
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  //
  printf(" Cable delay Set to %02X. \n",dword&0x3F);
  //
  // Update
  //
  cmd[0]=VTX_USR1;
  sndbuf[0]=21;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  //
}
void DAQMB::WriteSFM(){
  //
  //setcrateid(0);
  //
  LoadCableDelaySFM();
  LoadDMBIdSFM();
  LoadCFEBDelaySFM();
  // Enable
  SFMWriteProtect();
  // Write
  ProgramSFM();
  sleep(2);
  // Disable
  SFMWriteProtect();
  //
}

void DAQMB::PrintCounters(int user_option){
  //
  (*MyOutput_) << "Enter 1 for simple print-out" << std::endl;
  (*MyOutput_) << "      2 for print most frequent values" <<std::endl;
  (*MyOutput_) << "      3 for cuts by TMB DAV and/or same DAV, then print most frequent" <<std::endl;
  //
  if(user_option<1 | user_option>3) (*MyOutput_) << "Invalid option entered" << std::endl;
  //
  //Simple read counters option:
  //
  if(user_option==1) {
    //
    readtimingCounter();
    //
    readtimingScope();
    //
    (*MyOutput_) << "  Counters " << std::endl ;
    //
    (*MyOutput_) << "  L1A to LCT delay: " << GetL1aLctCounter()  << std::endl ;
    (*MyOutput_) << "  CFEB DAV delay:   " << GetCfebDavCounter() << std::endl ;
    (*MyOutput_) << "  TMB DAV delay:    " << GetTmbDavCounter()  << std::endl ;
    (*MyOutput_) << "  ALCT DAV delay:   " << GetAlctDavCounter() << std::endl ;
    //
    (*MyOutput_) << std::endl ;
    //
    int trials = 0;
    while ( GetL1aLctScope() == 0 && trials < 10 ) {
      readtimingScope();
      trials++;
    }
    //
    (*MyOutput_) << "  L1A to LCT Scope: " ;
    (*MyOutput_) << setw(5) << GetL1aLctScope() << " " ;
    for( int i=4; i>-1; i--) (*MyOutput_) << ((GetL1aLctScope()>>i)&0x1) ;
    (*MyOutput_) << std::endl ;
    //
    trials = 0;
    while ( GetCfebDavScope() == 0 && trials < 10 ) {
      readtimingScope();
      trials++;
    }
    //
    (*MyOutput_) << "  CFEB DAV Scope:   " ;
    (*MyOutput_) << setw(5) << GetCfebDavScope() << " " ;
    for( int i=4; i>-1; i--) (*MyOutput_) << ((GetCfebDavScope()>>i)&0x1) ;
    (*MyOutput_) << std::endl ;
    //
    trials = 0;
    while ( GetTmbDavScope() == 0 && trials < 10 ) {
      readtimingScope();
      trials++;
    }
    //
    (*MyOutput_) << "  TMB DAV Scope:    " ;
    (*MyOutput_) << setw(5) << GetTmbDavScope() << " " ;
    for( int i=4; i>-1; i--) (*MyOutput_) << ((GetTmbDavScope()>>i)&0x1) ;
    (*MyOutput_) << std::endl ;
    //
    trials = 0;
    while ( GetAlctDavScope() == 0 && trials < 10 ) {
      readtimingScope();
      trials++;
    }
    //
    (*MyOutput_) << "  ALCT DAV Scope:   " ;
    (*MyOutput_) << setw(5) << GetAlctDavScope() << " " ;
    for( int i=4; i>-1; i--) (*MyOutput_) << ((GetAlctDavScope()>>i)&0x1) ;
    (*MyOutput_) << std::endl ;
    //
    (*MyOutput_) << "  Active DAV Scope: " ;
    (*MyOutput_) << setw(5) << GetActiveDavScope() << " " ;
    for( int i=4; i>-1; i--) (*MyOutput_) << ((GetActiveDavScope()>>i)&0x1) ;
    (*MyOutput_) << std::endl ;
    //
    (*MyOutput_) << std::endl ;
    //
  }
  //Loop and choose "best" option:
  else {
    //
    int nloop = 100;
    //
    int davsame;
    int tmbdavcut;
    if(user_option==3) {
      (*MyOutput_) << "Enter value of TMB DAV to cut on (suggest 6, use -1 for no cut):" << std::endl;
      std::cin >> tmbdavcut;
      //
      (*MyOutput_) << "Enter -1 for no cut against same DAV delays or 1 to use cut:" << std::endl;
      std::cin >> davsame;
    }
    //
    //Before looping, zero all of the counters
    //
    int type;
    int delay;
    // counts for [time bin,type] where 
    //       type=0 for LCT-L1A delay, 1 for CFEBDAV, 2 for TMBDAV, 3 for ALCTDAV
    int counts[256][4]; 
    for(type=0;type<4;type++) { 
      for(delay=0;delay<256;delay++) {
	counts[delay][type] = 0;}
    }
    //
    //Next read the counters nloop times and accumulate statistics
    //
    int iloop;
    int passcuts=0;
    for(iloop=0;iloop<nloop;iloop++){
      readtimingCounter();
      //
      int l1alct  = GetL1aLctCounter();
      int cfebdav = GetCfebDavCounter();
      int tmbdav  = GetTmbDavCounter();
      int alctdav = GetAlctDavCounter();
      //
      //	  (*MyOutput_) << "Debug: l1alct= " << l1alct << "cfebdav=" << cfebdav;
      //      (*MyOutput_) << " tmbdav=" <<tmbdav << " alctdav=" << alctdav << std::endl;
      //
      //Pass cuts?
      //Maybe no cuts, or else have to 
      //pass tmbdav value cuts and pass not same cuts
      //
      bool nocuts=(user_option==2);
      bool tmbdavok=(tmbdavcut==-1 || tmbdav==tmbdavcut);
      bool davdiff=(davsame==-1 || (cfebdav!=tmbdav || cfebdav!=alctdav || tmbdav!=alctdav));
      //
      if (nocuts || (tmbdavok && davdiff))
	{
	  passcuts+=1;
	  counts[ l1alct  ][0] += 1;
	  counts[ cfebdav ][1] += 1;
	  counts[ tmbdav  ][2] += 1;
	  counts[ alctdav ][3] += 1;
	}
    }
    //
    //Next analyze the counters to find the most frequent setting
    //
    int maxnum[4],maxdelay[4];
    //
    maxnum[0]=-1;
    maxnum[1]=-1;
    maxnum[2]=-1;
    maxnum[3]=-1;
    //
    // Exclude delay=0 (no meaning!)
    //
    for(delay=1;delay<256;delay++)
      {
	//
	//Debug
	//	    (*MyOutput_) << "Delay=" << delay << " Counts are";
	//	    (*MyOutput_) << "  LCT-L1A : " << counts[delay][0]; 
	//	    (*MyOutput_) << "  CFEB-DAV: " << counts[delay][1];
	//	    (*MyOutput_) << "  TMB-DAV:  " << counts[delay][2];
	//	    (*MyOutput_) << "  ALCT-DAV: " << counts[delay][3] << std::endl; 
	//
	for(type=0;type<4;type++)
	  {
	    if( counts[delay][type] > maxnum[type] )
	      {
		maxnum[type]=counts[delay][type];
		maxdelay[type]=delay;
	      }
	  }	    
      }
    //
    (*MyOutput_) << std::endl << " Best delay settings in " << nloop << " readings and " 
		 << passcuts << " passing cuts are:" << std::endl;
    (*MyOutput_) << std::endl;
    (*MyOutput_) << "  LCT -L1A delay=" << maxdelay[0] << " (" << maxnum[0] << "readings)" << std::endl;
    (*MyOutput_) << "  CFEB-DAV delay=" << maxdelay[1] << " (" << maxnum[1] << "readings)" << std::endl;
    (*MyOutput_) << "  TMB -DAV delay=" << maxdelay[2] << " (" << maxnum[2] << "readings)" << std::endl;
    (*MyOutput_) << "  ALCT-DAV delay=" << maxdelay[3] << " (" << maxnum[3] << "readings)" << std::endl;
    (*MyOutput_) << std::endl;
    //
  }    
  //
}
//
void DAQMB::test3()
{
  int errs,err[8];
  int pass;
  //
  calctrl_fifomrst(); 
  usleep(5000);
  /*
  errs=0;
  printf("Running Memchk\n");
  (*MyOutput_) << "FIFO1 " << std::endl ;
  err[1]=memchk(FIFO1);
  errs+=err[1];
  (*MyOutput_) << "FIFO2 " << std::endl ;
  err[2]=memchk(FIFO2);
  errs+=err[2];
  (*MyOutput_) << "FIFO3 " << std::endl ;
  err[3]=memchk(FIFO3);
  errs+=err[3];
  (*MyOutput_) << "FIFO4 " << std::endl ;
  err[4]=memchk(FIFO4);
  errs+=err[4];
  (*MyOutput_) << "FIFO5 " << std::endl ;
  err[5]=memchk(FIFO5);
  errs+=err[5];
  (*MyOutput_) << "FIFO7 " << std::endl ;
  err[7]=memchk(FIFO7);
  errs+=err[7];
  //
  calctrl_fifomrst(); 
  usleep(5000);
  */
  //
  pass=1;
  errs=0; 
  //
  calctrl_fifomrst(); 
  usleep(5000);
  //
  (*MyOutput_) << "Running Memchk" << std::endl;
  err[1]=memchk(FIFO1);
  errs+=err[1];
  if(err[1]==0){
    (*MyOutput_) << " FIFO1 is OK  " << std::endl;
  }else{
    (*MyOutput_) << " FIFO1 is Bad " << err[1] << std::endl;
  }
  //
  err[2]= memchk(FIFO2);
  errs+=err[2];
  if(err[2]==0){
    (*MyOutput_) << " FIFO2 is OK " << std::endl ;
  }else{
    (*MyOutput_) << " FIFO2 is Bad " << err[2] << std::endl;
  }
  err[3]= memchk(FIFO3);
  errs+=err[3];
  if(err[3]==0){
    (*MyOutput_) << " FIFO3 is OK " << std::endl;
  }else{
    (*MyOutput_) << " FIFO3 is Bad " << err[3] << std::endl;
  }
  err[4]= memchk(FIFO4);
  errs+=err[4];
  if(err[4]==0){
    (*MyOutput_) << " FIFO4 is OK " << std::endl;
  }else{
    (*MyOutput_) << " FIFO4 is Bad " << err[4] << std::endl;
  }
  err[5]= memchk(FIFO5);
  errs+=err[5];
  if(err[5]==0){
    (*MyOutput_) << " FIFO5 is OK " << std::endl;
  }else{
    (*MyOutput_) << " FIFO5 is Bad " << err[5] << std::endl;
  }
  err[6]= memchk(FIFO6);
  if(err[6]==0){
    (*MyOutput_) << " FIFO6 is OK " << std::endl;
  }else{
    (*MyOutput_) << " FIFO6 is Bad " << err[6] << std::endl;
  }
  errs+=err[6];
  //
  err[7]= memchk(FIFO7);
  errs+=err[7];
  if(err[7]==0){
    (*MyOutput_) << " FIFO7 is OK " << std::endl;
  }else{
    (*MyOutput_) << " FIFO7 is Bad " << err[7] << std::endl;
  }
  errs+=err[7];
  calctrl_fifomrst();
  if(errs!=0)pass=0;
  //
  TestStatus_[3] = errs;
  //
}
//
void DAQMB::wrtfifox(enum DEVTYPE devnum,unsigned short int pass)
{ 

 if(devnum<FIFO1||devnum>FIFO7){
    printf(" Device is not a FIFO \n");
    return;
 }
 //
 /* fifo write */  
 cmd[0]=0;
 sndbuf[0]=pass&0xff;
 sndbuf[1]=((pass>>8)&0xff);
 //  printf(" wrtfifox 16384 %02x %02x \n",sndbuf[1]&0xff,sndbuf[0]&0xff);
 if(devnum-FIFO7!=0){
   devdo(devnum,1,cmd,16380*2,sndbuf,rcvbuf,2);}
 else{
   devdo(devnum,1,cmd,8190*2,sndbuf,rcvbuf,2);}
}
//
int DAQMB::readfifox_chk(enum DEVTYPE devnum,unsigned int short memchk)
     
{
 int bad;
 if(devnum<FIFO1||devnum>FIFO7){
    printf(" Device is not a FIFO \n");
    return -1;
 }
 //
 cmd[0]=VTX2_USR1;
 sndbuf[0]=FIFO_RD;
 devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
 cmd[0]=VTX2_USR2;
 sndbuf[0]=devnum-FIFO1+1; 
 devdo(MCTRL,6,cmd,3,sndbuf,rcvbuf,0); 
 cmd[0]=VTX2_USR1;
 sndbuf[0]=0;
 devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,2);
 cmd[0]=VTX2_BYPASS;
 sndbuf[0]=0;
 devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
 //
 /* fifo read check */  
 cmd[0]=1;
 sndbuf[0]=memchk&0xff;
 sndbuf[1]=((memchk>>8)&0xff);
 devdo(devnum,1,cmd,16380*2,sndbuf,rcvbuf,2);
 //
 (*MyOutput_) << " Number Bad: rcvbuf " << std::hex << rcvbuf[1] << " " << rcvbuf[0] << std::endl; 
 //
 bad=256*rcvbuf[1]+rcvbuf[0];
 //
 cmd[0]=VTX2_USR1;
 sndbuf[0]=FIFO_RD;
 devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
 cmd[0]=VTX2_USR2;
 sndbuf[0]=0; 
 devdo(MCTRL,6,cmd,3,sndbuf,rcvbuf,0); 
 cmd[0]=VTX2_BYPASS;
 sndbuf[0]=0;
 devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
 //
 return bad;
 //
}
//
int DAQMB::memchk(enum DEVTYPE devnum)
{
  int ierr,ierr2;
  unsigned long int lmsk;
  //
  if(devnum<FIFO1||devnum>FIFO7){
    (*MyOutput_) << " Device is not a FIFO " << std::endl;
    return -1;
  }
  //
  for (int i=0; i<sizeof(rcvbuf);i++) {
    rcvbuf[i] = 0;
  }
  //
  ierr2=0;
  wrtfifox(devnum,0xffff);
  ierr=readfifox_chk(devnum,0xffff);
  ierr2+=ierr;
  (*MyOutput_) << " ierr " << ierr << ierr2 << std::endl;
  wrtfifox(devnum,0x0000);
  ierr=readfifox_chk(devnum,0x0000);
  (*MyOutput_) << " ierr "  << ierr << std::endl;
  ierr2+=ierr;
  wrtfifox(devnum,0xaaaa);
  ierr=readfifox_chk(devnum,0xaaaa);
  (*MyOutput_) << " ierr " << ierr << std::endl;
  ierr2+=ierr; 
  wrtfifox(devnum,0x5555);
  ierr=readfifox_chk(devnum,0x5555);
  (*MyOutput_) << " ierr " << ierr << std::endl ;
  ierr2+=ierr;  
  wrtfifo_123(devnum);
  ierr=readfifox_123chk(devnum);
  (*MyOutput_) << " ierr " << ierr << std::endl;
  ierr2+=ierr; 
  wrtfifo_toggle(devnum);
  ierr=readfifox_togglechk(devnum);
  (*MyOutput_) << " ierr " << ierr << std::endl ;
  ierr2+=ierr;   
  (*MyOutput_) << " ierr2 " << ierr2 << std::endl;
  return ierr2;
}
//
int DAQMB::readfifox_togglechk(enum DEVTYPE devnum)
  //
{
 int bad;
 if(devnum<FIFO1||devnum>FIFO7){
    printf(" Device is not a FIFO \n");
    return -1;
 }

 /* fifo read  */  
 
 cmd[0]=VTX2_USR1;
 sndbuf[0]=FIFO_RD;
 devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
 cmd[0]=VTX2_USR2;
 sndbuf[0]=devnum-FIFO1+1; 
 devdo(MCTRL,6,cmd,3,sndbuf,rcvbuf,0); 
 cmd[0]=VTX2_USR1;
 sndbuf[0]=0;
 devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,2);
 cmd[0]=VTX2_BYPASS;
 sndbuf[0]=0;
 devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
 
 cmd[0]=3;
 sndbuf[0]=0;
 devdo(devnum,1,cmd,16380*2,sndbuf,rcvbuf,2);
 //  printf(" Number Bad: rcvbuf %02x %02x \n",rcvbuf[1],rcvbuf[0]); 
 bad=256*rcvbuf[1]+rcvbuf[0];

 cmd[0]=VTX2_USR1;
 sndbuf[0]=FIFO_RD;
 devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
 cmd[0]=VTX2_USR2;
 sndbuf[0]=0; 
 devdo(MCTRL,6,cmd,3,sndbuf,rcvbuf,0); 
 cmd[0]=VTX2_BYPASS;
 sndbuf[0]=0;
 devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
 
 return bad;
}
//
void DAQMB::wrtfifo_toggle(enum DEVTYPE devnum)
{
    if(devnum<FIFO1||devnum>FIFO7){
    printf(" Device is not a FIFO \n");
    return;
 }

   /* fifo write */  
 cmd[0]=2;
 sndbuf[0]=0;
 if(devnum-FIFO7!=0){
   devdo(devnum,1,cmd,16380*2,sndbuf,rcvbuf,2);}
 else{ 
   devdo(devnum,1,cmd,8190*2,sndbuf,rcvbuf,2);}
}
//
void DAQMB::wrtfifo_123(enum DEVTYPE devnum)
{
  if(devnum<FIFO1||devnum>FIFO7){
    printf(" Device is not a FIFO \n");
    return;
  }
  //
  /* fifo write */  
  cmd[0]=2;
  sndbuf[0]=2;
  if(devnum-FIFO7!=0){
    devdo(devnum,1,cmd,16380*2,sndbuf,rcvbuf,2);}
  else{
    devdo(devnum,1,cmd,8190*2,sndbuf,rcvbuf,2);}
}

//
int DAQMB::readfifox_123chk(enum DEVTYPE devnum)
     
{
 int bad;
 if(devnum<FIFO1||devnum>FIFO7){
    printf(" Device is not a FIFO \n");
    return -1;
 }

   /* fifo read */
    cmd[0]=VTX2_USR1;
  sndbuf[0]=FIFO_RD;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  sndbuf[0]=devnum-FIFO1+1; 
  devdo(MCTRL,6,cmd,3,sndbuf,rcvbuf,0); 
  cmd[0]=VTX2_USR1;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,2);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);

 cmd[0]=3;
 sndbuf[0]=2;
 devdo(devnum,1,cmd,16380*2,sndbuf,rcvbuf,2);
 // printf(" Number Bad: rcvbuf %02x %02x \n",rcvbuf[1],rcvbuf[0]); 
 bad=256*rcvbuf[1]+rcvbuf[0];

  cmd[0]=VTX2_USR1;
  sndbuf[0]=FIFO_RD;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  sndbuf[0]=0; 
  devdo(MCTRL,6,cmd,3,sndbuf,rcvbuf,0); 
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);

 return bad;
}

//

int DAQMB::memchk(int fifo)
{
  static int fifosize=16380;
  int err,err1;
  char *sndfifo;
  char *rcvfifo;
  sndfifo=(char *)malloc(33000);
  rcvfifo=(char *)malloc(33000);
  //
  (*MyOutput_) << " MEMCHK for FIFO"<<fifo<<std::endl;
  calctrl_fifomrst(); usleep(5000);
  // 0xffff
  for(int i=0;i<fifosize*2;i++)sndfifo[i]=0xff;
  wrtfifo(fifo,fifosize,sndfifo);
  readfifo(fifo,fifosize,rcvfifo);
  err=0;for(int i=0;i<fifosize*2;i++)if(sndfifo[i]!=rcvfifo[i])err=err+1;err1=err1+err;
  (*MyOutput_) << " Error 0xffff "<< err << std::endl;
  /* 
  // 0x0000
  for(int i=0;i<fifosize*2;i++)sndfifo[i]=0x00;
  wrtfifo(fifo,fifosize,sndfifo);
  readfifo(fifo,fifosize,rcvfifo);
  err=0;for(int i=0;i<fifosize*2;i++)if(sndfifo[i]!=rcvfifo[i])err=err+1;err1=err1+err;
  (*MyOutput_) << " Error 0x0000 " << err << std::endl; 
  // 0x5555
  for(int i=0;i<fifosize*2;i++)sndfifo[i]=0x55;
  wrtfifo(fifo,fifosize,sndfifo);
  readfifo(fifo,fifosize,rcvfifo);
  err=0;for(int i=0;i<fifosize*2;i++)if(sndfifo[i]!=rcvfifo[i])err=err+1;err1=err1+err;
  (*MyOutput_) << " Error 0x5555 "<< err << std::endl; 
// 0xaaaa
  for(int i=0;i<fifosize*2;i++)sndfifo[i]=0xaa;
  wrtfifo(fifo,fifosize,sndfifo);
  readfifo(fifo,fifosize,rcvfifo);
  err=0;for(int i=0;i<fifosize*2;i++)if(sndfifo[i]!=rcvfifo[i])err=err+1;err1=err1+err;
  (*MyOutput_) << " Error 0xaaaa "<< err << std::endl; 
  // 0,1,2,3,... 
  for(int i=0;i<fifosize*2;i++)sndfifo[i]=(i&0xff);
  wrtfifo(fifo,fifosize,sndfifo);
  readfifo(fifo,fifosize,rcvfifo);
  err=0;for(int i=0;i<fifosize*2;i++)if(sndfifo[i]!=rcvfifo[i])err=err+1;err1=err1+err;
  (*MyOutput_) << " Error 0x1234 " << err << std::endl; 
  free(sndfifo);
  free(rcvfifo);
  //
  */
  return err1;
  //
}
//
int DAQMB::test4()
{
  //
  int pass;
  float voltage, temp;
  //
  int ierr=0; 
  //
  for(int j=1;j<=3;j++){
    (*MyOutput_) << "Reading 1.8V Chip "<<j<<std::endl;
    voltage = adcminus(j,6);
    (*MyOutput_) << "The voltage is " << voltage <<" mv" << std::endl;
    //
    if(voltage>=1740.0 && voltage<=1860.0){
      (*MyOutput_) << "The voltage on 1.8V Chip "<<j<<" is good " << std::endl;
    }else{
      (*MyOutput_) << "The voltage on 1.8V Chip " << j << " is out of range" << std::endl;
      ierr=1;
    }
  }
  //
  (*MyOutput_) << "Reading DMB GND" << std::endl;
  voltage=adcminus(3,0); 
  (*MyOutput_) << "The voltage is " << voltage << " mv " << std::endl;
  //
  if(voltage>=-10.0 &&voltage<=10.0){
    (*MyOutput_) << "The voltage on DMB GND is good" << std::endl;
  }
  else {
    (*MyOutput_) << "The voltage on DMB GND is out of range" << std::endl;
    ierr=1;
  }
  //
  for(int k=1;k<6;k++){
    (*MyOutput_) << "Reading CFEB"<<k<< " GND "<<std::endl;
    //
    voltage = adcminus(3,k);
    (*MyOutput_) << "The voltage is " << voltage << " mv " << std::endl;
    //
    if(voltage>=-35.0 &&voltage<=35.0){
      (*MyOutput_) << "The voltage on CFEB"<<k<< "is good"<<std::endl;
    } else {
      (*MyOutput_) << "The voltage on CFEB"<<k<< "is out of range" << std::endl;
      ierr=1;
    }
  }
  //
  (*MyOutput_) << "Temperature Reading for DMB" << std::endl;
  temp=readthermx(0); 
  (*MyOutput_) << "The temperature is " << temp <<" F " << std::endl;
  //
  if(temp>=50.0 &&temp<=95.0){
    (*MyOutput_) << "The temperature on DMB is good " << std::endl;
  } else {
    (*MyOutput_) << "The temperature on DMB is out of range" << std::endl;
    ierr=1;
  }
  //
  for(int k=1;k<6;k++){
    (*MyOutput_) << "Temperature Reading CFEB "<<k << std::endl;
    temp = readthermx(k);
    (*MyOutput_) << "The temperature is " << temp << " F " << std::endl;
    //
    if(temp>=50.0 && temp<=95.0){
      (*MyOutput_) << "The temperature on CFEB"<<k<<" is good " << std::endl;
      
    }else{
      (*MyOutput_) << "The temperature on CFEB"<<k<<" is out of range " << std::endl;
      ierr=1;
    }
  }
  //
  TestStatus_[4] = ierr;
  //
  pass=1; 
  if(ierr!=0)pass=0;
  return pass;
}
//
int DAQMB::test5()
{
  int i,k;
  int pass,ierr;
  int itog;
  int f1[5]={3,3,4,4,5};
  int f2[5]={4,7,2,5,0};
  float value;
  unsigned int ival;
  char c;
  //
  ierr=0;  
  itog=0;
  for(k=0;k<2;k++){
    if(itog==0)c=0x00; 
    if(itog==1)c=0x3f;
    lowv_onoff(c);
    lowv_rdpwrreg(); 
    (*MyOutput_) << " power register " << ival << std::endl;
    sleep(1);
    itog=itog+1;
    if(itog==2)itog=0;
    /* these are the adcs connected to the power register */
    (*MyOutput_) << " Selected Reg. Channel" << std::endl;
    //
    /*j is 0-7 i is 1-5*/
    for(i=0;i<5;i++){
      value = lowv_adc(f1[i],f2[i]);
      if(itog==0){
	if(value<5000 && value>4100.0){
	  (*MyOutput_) <<"The voltage on "<<f1[i]<<" "<<f2[i]
		       <<" is good - "<<value<<" mv "<<std::endl;
	}else{   
	  (*MyOutput_) <<"The voltage on "<<f1[i]<<" "<<f2[i]
		       <<" is out of range - "<<value<<" mv "<<std::endl;
	  ierr=1;
	} 
      }
      //
      if(itog==1){
	if(value>-200 && value<200){
	  (*MyOutput_) << "The voltage off "<<f1[i]<<" "<<f2[i]
		       <<" is good - "<<value<<" mv " << std::endl;
	}else{   
	  (*MyOutput_) << "The voltage off "<<f1[i]<<" "<<f2[i]
		       <<" is out of range - "<<value<<" mv " << std::endl;
	  ierr=1;
	} 
      }
    }
  }
  //
  TestStatus_[5] = ierr;
  //
  pass=1; 
  if(ierr!=0)pass=0;
  return pass;
}
//
int DAQMB::test6()
{
  //
  int err2,pass;
  int ierr=0;
  unsigned long int ival;
  //
  err2=0;
  if(cfebs_.size()!=5){
    err2=err2+100;
    (*MyOutput_) << "ERROR: only " <<cfebs_.size()<< " FEBs found " << std::endl;
  }
  err2=0;
  for(unsigned i = 0; i < cfebs_.size(); ++i) {
    ival=febfpgaid(cfebs_[i]);
    usleep(100);
    if(ival!=0x20610093){
      err2=err2+1;
      ierr=1;
      (*MyOutput_) << " ERROR: "<<hex<<i<<" febfpgaid is: "<<ival
		   << " and should be 0x20610093" << std::endl;
    }
  }
  //
  TestStatus_[6] = ierr;
  //
  pass=1; 
  if(err2!=0)pass=0;
  //
  return pass;
  //
}
//
int  DAQMB::test8()
{
  //
  int i,ierr,err2,pass;
  float v0,v1,vout,diff,diff2;
  //
  double sn,sx,sy,sxy,sx2;
  float x[9],y[9];
  float a,b;
  ierr=0;
  //
  // check comparator DAC/ADC
  //
  if(cfebs_.size()!=5){
     ierr=ierr+100;
     (*MyOutput_) << "ERROR: only "<<cfebs_.size()<<" FEBs found "<<std::endl;
  }
  err2=0;
  for(int cfeb = 0; cfeb < cfebs_.size(); ++cfeb) {
    //
    for(i=0;i<10;i++){
      //
      v0=0.25*i;
      set_comp_thresh(cfeb,v0);
      //set_comp_thresh(v0);
      usleep(500000);
      //
      for(int cfeb=0; cfeb<cfebs_.size(); ++cfeb) {
	vout=adcplus(2,cfebs_[cfeb].number());
	(*MyOutput_) << "cfeb="<<cfeb<<" "<<" v0=" << v0 << " vout="<<vout<<std::endl;
      }
      //
      vout=adcplus(2,cfebs_[cfeb].number());
      if(i>0){
	x[i-1]=v0;
	y[i-1]=vout;
	(*MyOutput_) << "CFEB=" << cfeb<<" "<<v0<<" "<<vout<<std::endl;
      }
      //
    }
    //
    sn=0.;
    sx=0.;
    sy=0.;
    sxy=0.;
    sx2=0.;
    //
    for(i=0;i<9;i++){
      sn=sn+1;
      sx=sx+x[i];
      sy=sy+y[i];
      sxy=sxy+x[i]*y[i];
      sx2=sx2+x[i]*x[i];
    }
    //
    a=(sxy*sn-sx*sy)/(sn*sx2-sx*sx);
    b=(sy*sx2-sxy*sx)/(sn*sx2-sx*sx);
    (*MyOutput_)<<"a " <<a<< " b " <<b<<std::endl;
    //
    if(a<-1025.||a>-800.){
      ierr=1;
      (*MyOutput_) << " slope     out of range- got "<<a<<" should be -1005. " <<std::endl;
    }
    //
    if(b<3400.||b>3800.){ //changed a>3800 to b>3800
      ierr=1;
      (*MyOutput_) << " intercept out of range- got "<<a<<" should be 3600. " << std::endl;
    }
    //
    for(i=0;i<9;i++){
      diff=y[i]-a*x[i]-b;
      diff2=diff;
      if(diff2<0)diff2=-diff2;
      if(diff2<2.0){
	(*MyOutput_) << "Good - i "<<i<<" " <<y[i]<<" "<<a*x[i]+b<<" "<<diff<<std::endl;
      }else{
	ierr=1;
	(*MyOutput_) << "Bad  - i "<<i<<" " <<y[i]<<" "<<a*x[i]+b<<" "<<diff<<std::endl;
      }
    }
  }
  //
  // end comparator DAQ/ADC
  //
  TestStatus_[8] = ierr;
  //
  pass=1; 
  if(ierr!=0)pass=0;
  return pass;
}
//
int DAQMB::test9()
{
  int i,ierr,pass;
  float v0,v1,vout,diff;
  int ius;
  float voff;
  float capn[5] ={0.0,0.25,0.50,0.748,0.992};
  float presn[5]={0.0,0.25,0.50,0.745,0.988};
  //
  ierr=0;  
  // 
  ius=0;
  voff=0.0;
  for(i=0;i<5;i++){
    v0=.25*i;
    v1=v0;
    set_dac(v0,v1);
    usleep(100);
    vout=adcplus(2,7);
    vout=vout/1000.;
    if(ius==0&&vout>0.0){ius=1;voff=fabs(vout-v0);}
    diff=fabs(capn[i]-vout)-voff;
    if(diff<0.0)diff=0;
    if(diff>0.020){
      ierr=1;
      (*MyOutput_) << "Problem -Int CDAC - got " << vout << " expect " << v0 << std::endl;
    }else{ 
      (*MyOutput_) << "Good    -Int CDAC - got " << vout << " expect " << v0 << std::endl;
    }
  }
  // end int cal
  (*MyOutput_) << " end int cal " << std::endl;
  // ext cal
  ius=0;
  voff=0.;
  for(i=0;i<5;i++){
    v0=.25*i;
    v1=v0;
    set_dac(v0,v1);
    usleep(100);
    vout=adcminus(3,7);
    vout=vout/1000.; 
    if(ius==0&&vout>0.0){ius=1;voff=fabs(v0-vout);}
    diff=fabs(presn[i]-vout)-voff;
    if(diff<0.0)diff=0.;
    if(diff>0.020){
      ierr=1;
      (*MyOutput_) << "Problem -Ext CDAC - got "<<vout<<" expect "<<v0<<std::endl;
    }else{ 
      (*MyOutput_) << "Good    -Ext CDAC - got "<<vout<<" expect "<<v0<<std::endl;
    }
  }
  // end ext cal
  (*MyOutput_) << "end ext cal " << std::endl;
  // ext cal ptrd
  ius=0;
  voff=0.0;
  for(i=0;i<5;i++){
    v0=.25*i;
    v1=v0;
    set_dac(v0,v1);
    usleep(100);
    vout=adc16(4,0);
    if(ius==0&&vout>0.0){ius=1;voff=fabs(v0-vout);}
    diff=fabs(presn[i]-vout)-voff;
    if(diff<0.0)diff=0.;
    if(diff>0.020){
      ierr=1;
      (*MyOutput_) << "Problem -Pres Ext CDAC - got "<<vout<<" expect "<<v0<<std::endl;
    }else{ 
      (*MyOutput_) << "Good    -Pres Ext CDAC - got "<<vout<<" expect "<<v0<<std::endl;
    }
  }
  // end ext pres
  (*MyOutput_) << " end ext pres " << std::endl;
  //
  pass=1; 
  if(ierr!=0)pass=0;
  //
  TestStatus_[9] = ierr;
  //
  return pass;
  //
}
//
int DAQMB::test10()
{
  //
  int i,ierr,pass;
  char cmp;
  char rcvpat[256];
  char sndpat[256];
  int match;
  //
  ierr=0;
  //Program SFM with 0,1,2,3,...,263
  for(i=0;i<264;i++)sndpat[i]=i&0xff;
  //
  sfm_test_load(sndpat);  
  //
  SFMWriteProtect();
  // Write
  ProgramSFM();
  sleep(2);
  // Disable
  SFMWriteProtect();
  //
  //Read SFM 
  //
  sfm_test_read(rcvpat);
  //
  match=0;
  for(i=0;i<256;i++){cmp=i&0xff;if(cmp==rcvpat[i])match=match+1;}
  if(match!=256){
    for (i=0;i<256;i++) printf("%02x ",rcvpat[i]&0xff);printf("\n");
    ierr=match;
  }
  //
  pass=1; 
  if(ierr!=0)pass=0;
  //
  TestStatus_[10] = ierr;
  //
  return pass;
}
//
int  DAQMB::test11()
{  
  enum DEVTYPE dv;
  int i,j,itog;
  int nmtch;
  char pat[36],chk[32];
  char vshift[300];
  char v[2]={0x00,0xff};
  int ierr,pass;
  //
  ierr=0;
  itog=0;
  j=0;
 LOOP: j=j+1; 
   itog=itog+1;
   if(itog==2)itog=0;
   buckflash_erase();
   printf(" enter buckflash_load2 \n");
   //
   for(int k=0;k<295;k++)vshift[k]=v[itog];
   //
   buckflash_load(vshift);
   printf(" Sleep after buckflash Load \n");
   sleep(1);
   //
   buckflash_pflash();
   printf(" Sleep after buckflash program \n");
   sleep(5);
   //
   buckflash_init();
   sleep(1);
   //
   buckflash_init();         //add an extra init
   printf(" Sleep after buckeye init \n");
   sleep(2);
   //
   buck_shift_out();
   for(int brd=0;brd<5;brd++){
     for(i=0;i<36;i++)pat[i]=shift_out[brd][i];
     for(i=0;i<36;i++)chk[i]=v[itog];
     if(itog==1)chk[0]=0x7f;
     nmtch=0;
     for(i=0;i<36;i++){
       if(pat[i]==chk[i])nmtch++;
     }
     if(nmtch!=36){
       ierr=ierr+1;}
     printf(" nmtch %d \n",nmtch);
     for(i=0;i<36;i++)printf("%02x",pat[i]&0xff);printf("\n");
     printf("*Bad CFEB %d",dv); for(i=0;i<36;i++)printf("%02x",pat[i]&0xff);printf("\n");
   }
   if(j==2)goto ENDR;
   goto LOOP;
 ENDR: 
   //
   TestStatus_[11] = ierr;
   //
   pass=1; 
   if(ierr!=0)pass=0;
   return pass;
   //
}
//
