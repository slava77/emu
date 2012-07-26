//-----------------------------------------------------------------------
// $Id: DCFEB.h,v 1.3 2012/07/26 07:57:37 ahart Exp $
// $Log: DCFEB.h,v $
// Revision 1.3  2012/07/26 07:57:37  ahart
// Get rid of empty definition of configure in the header.
//
// Revision 1.2  2012/07/26 07:43:52  ahart
// Increase sleeps during firmware programming and add functions for loading timing constants to the EPROM.
//
// Revision 1.1  2012/07/12 12:59:16  ahart
//
// First commit of new files for the DCFEB.
//
// Revision 1.2  2009/03/25 10:19:41  liu
// move header files to include/emu/pc
//
// Revision 1.1  2009/03/25 10:07:42  liu
// move header files to include/emu/pc
//
// Revision 3.1  2008/08/13 11:30:53  geurts
// introduce emu::pc:: namespaces
// remove any occurences of "using namespace" and make std:: references explicit
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef DCFEB_h
#define DCFEB_h
#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <fcntl.h>
#include <cmath>
#include <unistd.h>
#include <iomanip>
#include <sys/types.h>
#include <sys/stat.h>
#include "emu/pc/Crate.h"
#include "emu/pc/VMEModule.h"
#include "emu/pc/JTAG_constants.h"
#include "emu/pc/BuckeyeChip.h"
#include "emu/pc/EmuLogger.h"
#include <vector>

namespace emu {
  namespace pc {

    class DCFEB: public VMEModule, public EmuLogger
{
public:
  DCFEB(Crate *,int,int number, std::vector<BuckeyeChip> buckeyes);
  /// assumes all default Buckeyes are present
  explicit DCFEB(Crate *,int,int number);
  ~DCFEB();
  int number() const {return number_;}
  DEVTYPE dscamDevice() const {return (DEVTYPE) (F1DCFEBM+number_);}
  DEVTYPE dpromDevice() const {return (DEVTYPE) (F1DCFEBM+number_);}  //load prom through fpga
 
  int NSAMPLE;
  unsigned short int pdata[6][16][100];

  std::vector<BuckeyeChip> buckeyeChips() const {return buckeyeChips_;}
  unsigned int boardType() const {return DCFEB_ENUM;}
  void end(){};
  void init(){};
  void configure();
  bool SelfTest(){return true;};

  static const int layers[6];
  char chipMask() const;


  // int NSAMPLE;
  unsigned long int febpromuser();
  unsigned long int febpromid();
  unsigned long int febfpgauser();
  unsigned long int febfpgaid();
  unsigned long int unpack_ibrd() const;
  unsigned int unpack_ival() const;
  void dParse(char *buf,int *Count,char **Word);
  void dshuffle(char *a,char *b);
  void dshuffle2(char*a,char *b,int off);
  void rdbkmskexp(FILE *fdesc,int cnt,char *array);
  void rdbkmskexp_asci(FILE *fp,int cnt,char *array,int init);
  void restoreCFEBIdle();
  void fxpreblkend(int word);
  void LctL1aDelay(int dword);
  void set_comp_mode(int dword);     //xcfeb
  void set_comp_thresh_bc(float thresh); //xcfeb
  void set_comp_thresh(float thresh);  //xcfeb
  void epromload_verify(const char *downfile,int writ,char *cbrdnum); //xcfeb
  void epromload(const char *downfile,int writ,char *cbrdnum);  //xcfeb
  void programvirtex6(const char *mcsfile);  //xcfeb
  void DCFEB_loopback();
  void DCFEB_Firmware_Simulation(); 
  void FADC_SetMask(short int mask);
  void FADC_Initialize();
  void FADC_ShiftData(unsigned long int shft);
  void Set_PipelineDepth(short int depth);
  void Pipeline_Restart();
  void Set_NSAMPLE(int nsample);
  void JTAGPipeline_read(char *rawdata);
  void GetDCFEBData(); 
  void ReadTemp();
  void ReadVolts(); 
  void Exec_Bit_Slip(int odd);
  void Sel_Chip(int chip);

  void epromdirect_loadaddress(unsigned int uaddr,unsigned int laddr,int isnd); 
  void epromdirect_manual(unsigned int ncnt,unsigned short int *manbuf,int isnd); 
  void epromdirect_unlockerase(int isnd);
  void epromdirect_unlock(int isnd);
  void epromdirect_noop(int isnd);
  void epromdirect_bufferprogram(unsigned int paddr,unsigned short int *prm_dat,int isnd);
  void epromdirect_lock(int isnd);
  void epromdirect_jread_only(unsigned int n,unsigned short int *pdata);
  void epromdirect_read(unsigned int paddr,unsigned short int *pdata);
  void epromdirect_readtest(unsigned int paddr);
  void epromdirect_timerstart();
  void epromdirect_timerstop();
  void epromdirect_timerreset();
  void epromdirect_clearstatus();
  void epromdirect_timerread();
  unsigned short int epromdirect_status();
  void epromdirect_enablefifo();
  void epromdirect_disablefifo();
  void epromdirect_reset();
  int epromdirect_erasepolling();
  int read_mcs(const char *fn);
  int epromloadtest_mcs();
  int epromreadtest_mcs();
  void make_eprom_mcs_file(char *wrt);
  int epromload_mcs(const char *fn,int loadlowhigh);
  int check_svf(const char *fn);
  int check_mcs(const char *fn);
  int epromread_mcs(const char *fn,FILE *fptst,int show,int loadlowhigh);
  void epromload_parameters(int paramblock,int nval,unsigned short int *val); 
  void epromread_parameters(int paramblock,int nval,unsigned short int  *val);
  void epromread_frame(unsigned frame,unsigned short int *val);
  void rdbkvirtex6();
  int check_rdbkvirtex6(FILE *fpout);
  void rdbkframevirtex6(unsigned int far,char *fpga);
  void wrtframevirtex6(unsigned int frame);
  void LoadFPGACompare_Frame(int framenumber,char *comp);
  void LoadFPGAMask_Frame(int framenumber,char *exp);
  void FPGAComp_Frame_makefile_randomaccess();
  void FPGAMask_Frame_makefile_randomaccess();
  void LoadFPGAComp_Frame_randomaccess(int framenumber,char *cmp);
  void LoadFPGAMask_Frame_randomaccess(int framenumber,char *msk);
  void CheckFPGAEPROMFILE_Frames(int framenumber);
  unsigned int frame2far(unsigned int frame);
  int far2frame(unsigned int far);
  unsigned long int febvirtex6dna();
  void rdbkstatusvirtex6();
  void rdbkidcodevirtex6();
  unsigned int rdbkfarvirtex6();
  void rdbkctl0virtex6();
  void rdbkcrcvirtex6();
  void wrtcrcvirtex6();
  void clearcrcvirtex6();
  void rdbkwbstarvirtex6();
  void wrtwbstarvirtex6(unsigned int reg);
  unsigned int rdbkregvirtex6(int REGNAME);
  void wrtregvirtex6(int REGNAME,unsigned int VALUE);
  int cfeb_testjtag_shift(char *out);
  void BuckeyeShift(int nchips,char chip_mask,char shftbits[6][6]);
  void BuckeyeShiftOut(int nchips,char chip_mask,char shftbits[6][6],char *shft_out);
  void buck_shift_ext_bc(int nstrip);
  void buck_shift_comp_bc(int nstrip);
  void BuckeyeShiftTest(int nchips,char chip_mask,char *pat);
  void dcfeb_raw_shift(char febbuf[7][4]);
  int dcfeb_testjtag_shift(char *out);

  void SetCompModeCfeb(int cfeb, int value){comp_mode_cfeb_[cfeb]=value;}  //xcfeb
  void SetCompTimingCfeb(int cfeb, int value){comp_timing_cfeb_[cfeb]=value;} //xcfeb
  void SetCompThresholdsCfeb(int cfeb, float value){comp_thresh_cfeb_[cfeb]=value;} //xcfeb
  void SetPipelineLengthCfeb(int cfeb, int value){pipeline_length_[cfeb]=value;} //xcfeb
  void SetTriggerClkPhaseCfeb(int cfeb, int value){trigger_clk_phase_[cfeb]=value;} //xcfeb
  void SetDaqClkPhaseCfeb(int cfeb, int value){daq_clk_phase_[cfeb]=value;} //xcfeb

private:
  int number_;
  std::vector<BuckeyeChip> buckeyeChips_;
  int comp_mode_cfeb_[7];
  int comp_timing_cfeb_[7];
  float comp_thresh_cfeb_[7];
  int pipeline_length_[7];
  int trigger_clk_phase_[7];
  int daq_clk_phase_[7];
};

  } // namespace emu::pc
  } // namespace emu
#endif

