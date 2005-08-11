//-----------------------------------------------------------------------
// $Id: DAQMB.cc,v 2.2 2005/08/11 08:13:04 mey Exp $
// $Log: DAQMB.cc,v $
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
#include "geom.h"

//using namespace std;

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
  tmb_dav_counter_(-1), alct_dav_counter_(-1)
{
  cfebs_.clear();
  std::cout << "DMB: crate=" << this->crate() << " slot=" << this->slot() << std::endl;
}

DAQMB::DAQMB(int newcrate,int newslot,  int newcfeb):
  VMEModule(newcrate, newslot)
{
  cfebs_.push_back(CFEB(newcfeb));
  std::cout << "DMB: crate=" << this->crate() << " slot=" << this->slot() << std::endl; 
}

DAQMB::DAQMB(int newcrate,int newslot, const std::vector<CFEB> & cfebs):
  VMEModule(newcrate, newslot),
  cfebs_(cfebs)
{
  std::cout << "DMB: crate=" << this->crate() << " slot=" << this->slot() << std::endl; 
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


void DAQMB::configure() {
   std::cout << std::endl;
   std::cout << "CFEB size="<<cfebs_.size()<<std::endl;
   std::cout << "DAQMB: configure() for crate " << this->crate() << " slot " << this->slot() << std::endl;
   int cal_delay_bits = (calibration_LCT_delay_ & 0xF)
      | (calibration_l1acc_delay_ & 0x1F) << 4
      | (pulse_delay_ & 0x1F) << 9
      | (inject_delay_ & 0x1F) << 14;
   std::cout << "DAQMB:configure: caldelay " << std::hex << cal_delay_bits << std::dec << std::endl;
   setcaldelay(cal_delay_bits);

 
   //
   int dav_delay_bits = (feb_dav_delay_    & 0x1F)
      | (tmb_dav_delay_ & 0X1F) << 5
      | (push_dav_delay_   & 0x1F) << 10
      | (l1acc_dav_delay_  & 0x3F) << 15
      | (ALCT_dav_delay_   & 0x1F) << 21;
   std::cout << "doing setdavdelay " << dav_delay_bits << std::endl;
   setdavdelay(dav_delay_bits);
   //
   std::cout << "doing fxpreblkend " << pre_block_end_ << std::endl;
   fxpreblkend(pre_block_end_);
   calctrl_fifomrst();
   //
   int comp_mode_bits = (comp_mode_ & 3) | ((comp_timing_ & 7) << 2);
   std::cout << "doing set_comp_mode " << comp_mode_bits << std::endl;
   std::cout << comp_mode_ << " " << comp_timing_ << std::endl;
   set_comp_mode(comp_mode_bits);
   //fg where did these lines come from ...????
   //fg usleep(100);
   //fg set_comp_mode(comp_mode_bits);
   //
   std::cout << "doing set_comp_thresh " << set_comp_thresh_ << std::endl;
   set_comp_thresh(set_comp_thresh_);
   std::cout << "doing preamp_initx() " << std::endl;
   preamp_initx();
   std::cout << "doing set_cal_dac " << inj_dac_set_ << " " 
	<<  pul_dac_set_ << std::endl;
   set_cal_dac(inj_dac_set_, pul_dac_set_);
   load_strip();
   //
   // As suggested by Valery Sitnik: switch all LVs on (computer-controlled)
   // std::cout << "DAQMB: switching on LVs on LVMB" << endl; 
   //fg lowv_onof(0x3f);
}


void DAQMB::load_strip() {
  cmd[0]=VTX2_USR1;
  sndbuf[0]=LOAD_STR;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  int j=0;
  //for(i=0;i<5;i++){if(dp->iuse[i]==1)j=j+k;k=k*2;}
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
}

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

  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  std::cout << "setfebdelay to " << dword << std::endl;
}

void DAQMB::setcaldelay(int dword)
{

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
  std::cout << "caldelay was set to " << std::hex << dword <<std::dec << std::endl;

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
  std::cout << "set dav delay to " << dword << std::endl;
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
  printf(" %02x CAL_FIFOMRST \n",sndbuf[0]&0xff);
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
  std::cout << " FIFOMRST reset done " << std::endl;
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
  std::cout << "calling set_comp_mode " << dword << std::endl;
}

void DAQMB::set_comp_thresh(float thresh)
{
char dt[2];
 
/* digitize voltages */
  int dthresh=int(4095*((3.5-thresh)/3.5)); 
  dt[0]=0;
  dt[1]=0;
  for(int i=0;i<8;i++){
    dt[0]|=((dthresh>>(i+7))&1)<<(7-i);
    dt[1]|=((dthresh>>i)&1)<<(6-i);
  }
  dt[0]=((dt[1]<<7)&0x80) + ((dt[0]>>1)&0x7f);
  dt[1]=dt[1]>>1;
  std::cout << "CFEB size="<<cfebs_.size() << std::endl;
  for(int i=0; i<cfebs_.size();i++) {
     std::cout << i << " CFEB number" << cfebs_[i].number() << std::endl;
  }
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
  printf(" CDAC %04x %04x %02x %02x \n",dacout0,dacout1,cmd[0]&0xff,cmd[1])&0xff;
  devdo(CDAC,32,cmd,0,sndbuf,rcvbuf,2); 
}


void DAQMB::halfset(int icrd,int ipln,int ihalf,int chan[5][6][16])
{

   int ichan,iside;

   //std::cout << "DAQMB.halfset " << std::endl;
   
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


void DAQMB::trigsetx(int *hp)
{
   
   std::cout << "DAQMB.trigsetx" << std::endl;
   std::cout << "CFEB size="<<cfebs_.size() << std::endl;;

  int hs[6];
  int i,j,k,l;
  int chan[5][6][16];
  for(i=0;i<5;i++){
    for(j=0;j<6;j++){
      for(k=0;k<16;k++){
        chan[i][j][k]=NORM_RUN;
      }
    }
  }

  std::cout << "Setting Staggering" << std::endl;
  
  for(i=0;i<6;i+=2){
     hs[i]=-1;
     hs[i+1]=0;
  } 
  
  std::cout << " Setting halfset " << std::endl;
  
  for(k=0;k<5;k++){
     for(j=0;j<6;j++){
	halfset(k,j,hp[j]+hs[j],chan);
     }
  }
  chan2shift(chan);
}

void DAQMB::chan2shift(int chan[5][6][16])
{
   
   int i,j,k,l;
   int chip,lay,nchips;
   char chip_mask;
   char shft_bits[6][6];
   
   std::cout << "CFEB size=" << cfebs_.size() << std::endl;
   std::cout << "CFEB numbers" << std::endl;

   for(unsigned icfeb = 0; icfeb < cfebs_.size(); ++icfeb) std::cout << " " << cfebs_[icfeb].number() ;

   std::cout << std::endl;
   
   for(lay=0;lay<6;lay++){
      for(unsigned icfeb = 0; icfeb < cfebs_.size(); ++icfeb) {
	 int brdn = cfebs_[icfeb].number();
	 for(int i=0; i<16;i++) {
	    if ( chan[brdn][lay][i] > 0 ) printf("%c[01;43m", '\033');
	    std::cout << chan[brdn][lay][i] << "" ;
	    printf("%c[0m", '\033'); 
	 }
	 std::cout << " | " ;		
      }
      std::cout << std::endl;
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
  int ucla_c_hs;
  int ucla_c_nplns;
  int ucla_c_ndata;
  int i,j,k,count;
  int hp[6],ho[]={-1,0,1,2};
  int ierr;
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
                  printf("Pattern :\n");
                  for(k=0;k<6;k++){
		     printf(" %d ",hp[k]);
                  }
                  printf(" \n");
                  printf("Hits set = %d\n",hits);
                  trigsetx(hp);
                  inject(1,0x4f);
		  std::cout << " Next ?"<< std::endl;
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
  unsigned int ival= readADC(ichp, ichn);
  return (float) ival;
}


float DAQMB::adcminus(int ichp,int ichn){
  unsigned int ival= readADC(ichp, ichn);
  if((0x0800&ival)==0x0800)ival=ival|0xf000;
  return (float) ival;
}


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
  printf(" The FPGA USERCODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
  ibrd = unpack_ibrd();
  cmd[0]=VTX_BYPASS;
  sndbuf[0]=0;
  devdo(dv,5,cmd,0,sndbuf,rcvbuf,0);
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
  printf(" The FEB %d FPGA Chip should be 610093 (last 6 digits) \n",dv-F1SCAM+1);
  printf(" The FPGA Chip IDCODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
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
unsigned long int ibrd;
 

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



// DAQMB calibrate

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
      // std::cout<<" first devdo call \n";
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


int DAQMB::buck_shift_test()
{
#ifdef debugV
  std::cout << "inside buck_shift_test()" << std::endl;
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
	// std::cout<< hex <<pat[i] << dec;
        printf("%02x",pat[i]&0xff);
        if(pat[i]==chk[i])nmtch++;
      }
      printf("\n");
      // std::cout<< endl;
      if(nmtch==6){
        pass=1;
        std::cout<<"Pattern returned is OK\n";
      } else {
        pass=0;
        std::cout<<"Pattern returned is Wrong\n";
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
  std::cout << "done with preamp init " << std::endl;
}


void DAQMB::set_cal_tim_pulse(int ntim)

{
  //std::cout<< "setting pulse timing to " << ntim << std::endl;  
  int dword;
  dword=(CAL_DEF_DELAY)&0x1ff;
  dword=dword|((ntim&0x1f)<<9); 
  setcaldelay(dword);

}


void DAQMB::set_cal_tim_inject(int ntim)
{
  std::cout<< "setting inject timing to " << ntim << std::endl;
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

   std::cout << "DAQMB.inject " << std::endl;

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

  printf(" sndfifo: %d %02x %02x \n",nsndfifo,sndfifo[0]&0xff,sndfifo[1]&0xff);
/* fifo write */  
 cmd[0]=4;
  std::cout << "wrtfifo devnum FIFO7 " << devnum << " " << FIFO7 << std::endl;
 if(devnum-FIFO7!=0){
 devdo(devnum,1,cmd,nsndfifo*2,sndfifo,rcvbuf,2);}
 else{
 devdo(devnum,1,cmd,nsndfifo,sndfifo,rcvbuf,2);} 
}

void DAQMB::readfifo(int fifo,int nrcvfifo,char* rcvfifo)
{  
  PRINTSTRING(OVAL: before start routine in readfifo);
  PRINTSTRING(OVAL: after start routine in readfifo);
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
 
  cmd[0]=5;
  devdo(devnum,1,cmd,nrcvfifo*2+2,sndbuf,rcvfifo,2);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=FIFO_RD;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  sndbuf[0]=0; 
  devdo(MCTRL,6,cmd,3,sndbuf,rcvbuf,0); 
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  printf(" readfifo: %d %02x %02x \n",nrcvfifo,rcvfifo[0]&0xff,rcvfifo[1]&0xff); 
}

// DAQMB load and read flash memory (electronics experts only)

void DAQMB::buckflash_load(char *fshift)
{
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


// DAQMB program proms


void DAQMB::epromload(DEVTYPE devnum,char *downfile,int writ,char *cbrdnum)
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
int nowrit;

  printf(" epromload \n");

  if(devnum==ALL){
    devnum=F1PROM;
    devstp=F5PROM;
  }
  else {
    devstp=devnum;
  }
  for(int i=devnum;i<=devstp;i++){
    dv=(DEVTYPE)i;
    xtrbits=geo[dv].sxtrbits;
    //    printf(" ************************** xtrbits %d geo[dv].sxtrbits %d \n",xtrbits,geo[dv].sxtrbits);
    devstr=geo[dv].nam;
    dwnfp    = fopen(downfile,"r");
    fpout=fopen("eprom.bit","w");
    //  printf("Programming Design %s (%s) with %s\n",design,devstr,downfile);

    while (fgets(buf,256,dwnfp) != NULL)  {
      if((buf[0]=='/'&&buf[1]=='/')||buf[0]=='!'){
	//  printf("%s",buf);
      }
      else {
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
              if(nowrit==1&&(cbrdnum[0]|cbrdnum[1]|cbrdnum[2]|cbrdnum[3])!=0){
                tstusr=0;
                snd[0]=cbrdnum[0];
                snd[1]=cbrdnum[1];
                snd[2]=cbrdnum[2]; 
                snd[3]=cbrdnum[3];
		//        printf(" snd %02x %02x %02x %02x \n",snd[0],snd[1],snd[2],snd[3]);
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
            sndbuf[i]=snd[i]&smask[i];
          }
	  //   printf("D%04d",nbits+xtrbits);
          // for(i=0;i<(nbits+xtrbits)/8+1;i++)printf("%02x",sndbuf[i]&0xff);printf("\n");
          if(nowrit==0){
            scan(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
          }else{
	     if(writ==1) scan(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
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
            sndbuf[i]=snd[i]&smask[i];
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
	  //          printf("RUNTEST = %d\n",pause);
	  /*   ipd=83*pause;
          // sleep(1);
          t1=(double) clock()/(double) CLOCKS_PER_SEC;
          for(i=0;i<ipd;i++);
          t2=(double) clock()/(double) CLOCKS_PER_SEC;
	  //  if(pause>1000)printf("pause = %f s  while erasing\n",t2-t1); */
	  //          for (i=0;i<pause/100;i++)
	  //  devdo(dv,-1,sndbuf,0,sndbuf,rcvbuf,2);
          pause=pause/2;
          sndbuf[0]=pause-(pause/256)*256;
          sndbuf[1]=pause/256;
	  // printf(" sndbuf %d %d %d \n",sndbuf[1],sndbuf[0],pause);
          devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,0);
          // printf(" send sleep \n");  
/*          printf("pause      %d us\n",pause);*/
        }
        else if((strcmp(Word[0],"STATE")==0)&&(strcmp(Word[1],"RESET")==0)&&(strcmp(Word[2],"IDLE;")==0)){
	   printf("goto reset idle state\n"); 
	   //  devdo(dv,-1,sndbuf,0,sndbuf,rcvbuf,2);
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
  sndbuf[0]=0x01;
  sndbuf[1]=0x00;
	  // printf(" sndbuf %d %d %d \n",sndbuf[1],sndbuf[0],pause);
  devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,2);

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
    std::cout << "inside DAQMB executeCommand"<< std::endl;
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



#ifdef USEDCS

void DAQMB::cfeb_vtx_prom(enum DEVTYPE devnum) {
  //enum DEVTYPE devstp,dv;
  std::cout << "DAQMB: cfeb_vtx_prom" << std::endl;

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
 
  std::cout << "DAQMB: SCA Master is programmed by PROM (Calcntrl command)." <<std::endl;
}

void DAQMB::devdoReset(){
/// used for emergency loading
  devdo(RESET,-1,cmd,0,sndbuf,rcvbuf,2);
}
#endif

void DAQMB::readtiming()
{
  //GenDATA *dp;
  printf(" Entered READ_TIMING \n");
  //dp = (GenDATA *)data;
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
  l1a_lct_counter_  = rcvbuf[0]&0xFF ;
  cfeb_dav_counter_ = rcvbuf[1]&0xff ;
  tmb_dav_counter_  = rcvbuf[2]&0xff ;
  alct_dav_counter_ = rcvbuf[3]&0xff ;
  //
  //
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
}
