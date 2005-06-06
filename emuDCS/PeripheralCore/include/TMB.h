//-----------------------------------------------------------------------
// $Id: TMB.h,v 2.1 2005/06/06 15:17:17 geurts Exp $
// $Log: TMB.h,v $
// Revision 2.1  2005/06/06 15:17:17  geurts
// TMB/ALCT timing updates (Martin vd Mey)
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef TMB_h
#define TMB_h

#include "VMEModule.h"
#include <cstdio>
#include <vector>
#include <string>
#include <bitset>
#include "AnodeChannel.h"

class ALCTController;
class TMBParser;


class TMB : public VMEModule {

public:
      friend class ALCTController;
      friend class TMBParser;
      
      TMB(int newcrate, int slot);
      virtual ~TMB();
      /// from the BOARDTYPE enum
      virtual unsigned int boardType() const {return TMB_ENUM;}
      /// ucla_start was always called with a dev and a slot
      virtual void start();
      virtual void end();
      /// ALCTs need to go to lower scan level, whatever that means
      void start(int idev);
      /// does start(1)
      void tmb_vme(char fcn, char vme, const char *snd,char *rcv, int wrt);      
      int  GetWordCount();
      int  GetALCTWordCount();
      void configure();
      void GetTTC();
      int  ReadRegister(int);
      void WriteRegister(int,int);
      void DecodeCLCT();
      void DecodeALCT();
      void DumpAddress(int);
      void lctrequestdelay(int dword);
      void clear_i2c();
      void clk_delays(unsigned short int time,int cfeb_id);
      void scope(int scp_arm,int scp_readout);
      void decode();
      void ALCTRawhits();
      void TMBRawhits();
      void ForceScopeTrigger();
      void OnlyReadTMBRawhits();
      void fifomode();
      void init_alct(int choice);
      void load_cscid();
      void PrintCounters();
      // should have bx window coded in.  See trgmode.
      void lvl1_delay(unsigned short int time);
      void optimize();
      void read_delays();
      void reset();
      void scan_rx_clock();
      void trgmode(int choice);
      void ResetCounters();
      void SetALCTPatternTrigger();
      void SetCLCTPatternTrigger();
      /// called by TRGMODE, depending on version_
      void setupNewDelayChips();
      void setupOldDelayChips();
      void trgmode_bprsq_alct();
      void trgmode_bprsq_clct(int choice);
      void activecfeb();
      void toggle_l1req();
      void trgmode_bprsq_dmb();
      void firmwareVersion();
      void setLogicAnalyzerToDataStream(bool yesorno);
      void tmb_PHOS4_alct(int time=0);
      void tmb_PHOS4_cfeb();
      void EnableCLCTInputs(int CLCInputs );
      void DisableCLCTInputs();
      void tmb_clk_delays(unsigned short int time, int cfeb_id);
      //
      void ResetRAMAddress();
      void ResetALCTRAMAddress();
      void GetCounters();
      int TestArray();
      int TMBCRCcalc(std::vector< std::bitset<16> >& datain );
      std::bitset<22> calCRC22(const std::vector< std::bitset<16> >& datain);
      std::bitset<22> nextCRC22_D16(const std::bitset<16>& D, const std::bitset<22>& C);
      //
      void SetALCTController(ALCTController* a) {alctController_=a;}
      ALCTController * alctController() const {return alctController_;}
//      
      int tmb_get_id(struct tmb_id_regs* tmb_id);
      int tmb_set_jtag_src(unsigned short int jtag_src);
      int tmb_get_jtag_src(unsigned short int* jtag_src);
      int tmb_set_jtag_chain(unsigned int jchain);
      int tmb_set_reg(unsigned int vmereg, unsigned short int value );
      int tmb_get_reg(unsigned int vmereg, unsigned short int* value );
      int tmb_vme_reg(unsigned int vmereg, unsigned short int* value);
      int tmb_jtag_io(unsigned char tms, unsigned char tdi, unsigned char* tdo);
      int tmb_get_boot_reg(unsigned short int* value);
      int tmb_set_boot_reg(unsigned short int value);
      int tmb_hard_reset_alct_fpga();
      int tmb_hard_reset_tmb_fpga();
      int tmb_enable_alct_hard_reset(int flag_enable);
      int tmb_enable_vme_commands(int flag_enable);      
      /// mostly for GUI
      void executeCommand(std::string command);      
      friend std::ostream & operator<<(std::ostream & os, TMB & tmb);
      //
      int GetCLCT0Cfeb() { return CLCT0_cfeb_; }
      int GetCLCT1Cfeb() { return CLCT1_cfeb_; }
      int GetCLCT0Nhit() { return CLCT0_nhit_; }
      int GetCLCT1Nhit() { return CLCT1_nhit_; }
      int GetCLCT0keyHalfStrip() { return CLCT0_keyHalfStrip_; }
      int GetCLCT1keyHalfStrip() { return CLCT1_keyHalfStrip_; }
      //
      int FmState();
      //
      int GetAlct0Valid()    { return alct0_valid_; }
      int GetAlct0Quality()  { return alct0_quality_; }
      int GetAlct0Amu()      { return alct0_amu_; }
      int GetAlct0FirstKey() { return alct0_first_key_; }
      int GetAlct0FirstBxn() { return alct0_first_bxn_; }
      //
      int GetAlct1Valid()     { return alct1_valid_; }
      int GetAlct1Quality()   { return alct1_quality_; }
      int GetAlct1Amu()       { return alct1_amu_; }
      int GetAlct1SecondKey() { return alct1_second_key_; }
      int GetAlct1SecondBxn() { return alct1_second_bxn_; }
      //
      void enableAllClocks();
      void disableAllClocks();
      //
   public:
      //
      FILE *pfile;
      FILE *pfile2;
      int ucla_ldev;
      int trigMB_dav_delay_;
      int ALCT_dav_delay_; 
      int push_dav_delay_;
      int l1acc_dav_delay_;
      int cfeb0delay_;
      int cfeb1delay_;
      int cfeb2delay_;
      int cfeb3delay_;
      int cfeb4delay_;
      int l1a_offset_;
      int bxn_offset_;
      int alct_tx_clock_delay_;
      int alct_rx_clock_delay_;
      int l1a_window_size_;
      int l1adelay_;
      int alct_match_window_size_;
      int alct_vpf_delay_;
      std::string version_;
      int mpc_delay_;
      int ALCT_input_;
      int rpc_exists_;
      int fifo_mode_;
      int fifo_tbins_;
      int fifo_pretrig_;
      int alct_clear_;
      int mpc_tx_delay_;
      //
   protected:
      /// for PHOS4 chips, v2001
      void old_clk_delays(unsigned short int time, int cfeb_id);
      /// for DDD chips, v2004
      void new_clk_delays(unsigned short int time, int cfeb_id);
      
      ALCTController * alctController_;
      
   private:
      //
      int CLCT0_cfeb_;
      int CLCT1_cfeb_;
      int CLCT0_nhit_;
      int CLCT1_nhit_;
      int CLCT0_keyHalfStrip_;
      int CLCT1_keyHalfStrip_;
      static const int MaxCounter = 0x2d;
      long int FinalCounter[MaxCounter];
      //
      int alct0_valid_;
      int alct0_quality_;
      int alct0_amu_;
      int alct0_first_key_;
      int alct0_first_bxn_;
      //
      int alct1_valid_;
      int alct1_quality_;
      int alct1_amu_;
      int alct1_second_key_;
      int alct1_second_bxn_;
      //
};

#endif

