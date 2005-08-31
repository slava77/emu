/* 
     S. Durkin      1/25/05

 */
#ifndef DDU_h
#define DDU_h

using namespace std;
#include <iostream>
#include <vector>
#include <string>
#include "VMEModule.h"
#include "JTAG_constants.h"


class DDU: public VMEModule
{
public:
  friend class DDUParser;

// standard routines
  DDU(int newcrate, int slot);
  virtual ~DDU();
  /// from the BOARDTYPE enum
  virtual unsigned int boardType() const {return DDU_ENUM;}
  virtual void end();

  void configure();

// DDU commands initialization/reset
  void ddu_init();
  void ddu_reset();
  void ddu_vmel1a();
// DDU register control 
  void ddu_shfttst(int tst);
  void ddu_lowfpgastat();
  void ddu_hifpgastat();
  void ddu_checkFIFOa();
  void ddu_checkFIFOb();
  void ddu_checkFIFOc();
  void ddu_rdfibererr();
  void ddu_rdfiberok();
  void ddu_rdkillfiber();
  void ddu_loadkillfiber(int regval);
  void ddu_rdcrcerr();
  void ddu_rdl1aerr();
  void ddu_rdxmiterr();
  void ddu_rdtimesterr();
  void ddu_rdtimeewerr();
  void ddu_rdtimeeaerr();
  void ddu_rddmberr();
  void ddu_rdtmberr();
  void ddu_rdlieerr();
  void ddu_rdliderr();
  void ddu_rdpaferr();
  void ddu_rdfferr();
  void ddu_rderareg();
  void ddu_rderbreg();
  void ddu_rdercreg();
  void ddu_rd_verr_cnt();
  void ddu_rd_cons_cnt();
  void ddu_fifo0verr_cnt();
  void ddu_fifo1verr_cnt();
  void ddu_earlyVerr_cnt();
  void ddu_verr23cnt();
  void ddu_verr55cnt();
  void ddu_rdostat();
  void ddu_rdempty();
  void ddu_rdstuckbuf();
  void ddu_rdscaler();
  void ddu_rdalcterr();
  void ddu_loadbxorbit(int regval);
  void ddu_rdbxorbit();
  void ddu_lvl1onoff();
  void ddu_rd_boardID();
  void ddu_fpgastat();
  // INFPGA register control
  void infpga_shfttst(enum DEVTYPE dv,int tst);
  void infpga_reset(enum DEVTYPE dv);
  void infpga_rdscaler(enum DEVTYPE dv);
  void infpga_lowstat(enum DEVTYPE dv);
  void infpga_histat(enum DEVTYPE dv);
  void infpgastat(enum DEVTYPE dv);
  void infpga_CheckFiber(enum DEVTYPE dv);
  void infpga_DMBsync(enum DEVTYPE dv);
  void infpga_FIFOstatus(enum DEVTYPE dv);
  void infpga_FIFOfull(enum DEVTYPE dv);
  void infpga_RxErr(enum DEVTYPE dv);
  void infpga_Timeout(enum DEVTYPE dv);
  void infpga_XmitErr(enum DEVTYPE dv);
  void infpga_WrMemActive(enum DEVTYPE dv,int ifiber);
  void infpga_MemAvail(enum DEVTYPE dv);
  void infpga_Min_Mem(enum DEVTYPE dv);
  void infpga_LostErr(enum DEVTYPE dv);
  void infpga_StatA(enum DEVTYPE dv);
  void infpga_StatB(enum DEVTYPE dv);
  void infpga_StatC(enum DEVTYPE dv);
  // DDU Status Decode
  void ddu_status_decode(int long code);
  void ddu_ostatus_decode(int long code);
  void ddu_era_decode(int long code);
  void ddu_erb_decode(int long code);
  void ddu_erc_decode(int long code);

  // DDU FPGA id/user codes
  unsigned long int ddufpga_idcode();
  unsigned long int infpga_idcode0(); 
  unsigned long int infpga_idcode1(); 
  unsigned long int ddufpga_usercode();
  unsigned long int infpga_usercode0();
  unsigned long int infpga_usercode1();
  unsigned long int inprom_idcode0();
  unsigned long int inprom_idcode1();
  unsigned long int vmeprom_idcode();
  unsigned long int dduprom_idcode0();
  unsigned long int dduprom_idcode1();
  unsigned long int inprom_usercode0();
  unsigned long int inprom_usercode1();
  unsigned long int vmeprom_usercode();
  unsigned long int dduprom_usercode0();
  unsigned long int dduprom_usercode1();
  void all_chip_info();

  // DDU parallel 
  void vmepara_busy();
  void vmepara_fullwarn();
  void vmepara_lostsync();
  void vmepara_error();
  void vmepara_switch();
  void vmepara_status();
  void vmepara_status2();
  void vmepara_rd_inreg0();
  void vmepara_rd_inreg1();
  void vmepara_rd_inreg2();
  void vmepara_wr_inreg(unsigned int par_val);

  // DDU serial 
  void read_status();
  void read_page1();
  void write_page1();
  void read_page4();
  void write_page4();
  void read_page5();
  void write_page5();
  void read_page7();
  void write_page7();
  void read_vmesd0();
  void read_vmesd1();
  void read_vmesd2();
  void read_vmesd3();
  void write_vmesdF();

  // Voltages and Thermometers
  float adcplus(int ichp,int ichn);
  float adcminus(int ichp,int ichn);
  float readthermx(int it);
  unsigned int readADC(int ireg, int ichn);
  void read_therm();
  void read_voltages();

  // Unpack characters to integers
  unsigned int DDU::unpack_ival();

  // EPROM reprogramming (EXPERTS ONLY !)
  void epromload(char *design,enum DEVTYPE devnum,char *downfile,int writ,char *cbrdnum);
  void vmeprgprom(char *buf);
  void dduprgprom(char *buf);
  void inprgprom(char *buf);
  void resetprgprom(char *buf);
  void Parse(char *buf,int *Count,char **Word);

  /// sends commands by name
  void executeCommand(string command);

public:
  // unpacks rcvbuf from FPGA operations
  unsigned long int unpack_ibrd() const;
  unsigned int unpack_ival() const;
};


#endif

