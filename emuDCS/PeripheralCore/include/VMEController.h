//----------------------------------------------------------------------
// $Id: VMEController.h,v 3.12 2007/09/12 16:01:47 liu Exp $
// $Log: VMEController.h,v $
// Revision 3.12  2007/09/12 16:01:47  liu
// remove log4cplus dependency
//
// Revision 3.11  2007/08/28 20:37:13  liu
// remove compiler warnings
//
// Revision 3.10  2006/11/10 16:51:44  mey
// Update
//
// Revision 3.9  2006/10/30 15:56:05  mey
// Update
//
// Revision 3.8  2006/10/19 10:17:25  mey
// Update
//
// Revision 3.7  2006/10/02 18:18:33  mey
// UPdate
//
// Revision 3.6  2006/09/13 14:13:32  mey
// Update
//
// Revision 3.5  2006/09/12 15:50:01  mey
// New software changes to DMB abd CFEB
//
// Revision 3.4  2006/09/06 12:38:10  rakness
// correct time stamp/copy vectors for user prom
//
// Revision 3.3  2006/09/05 10:13:17  rakness
// ALCT configure from prom
//
// Revision 3.2  2006/08/21 20:34:21  liu
// update
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.27  2006/07/20 14:03:11  mey
// Update
//
// Revision 2.26  2006/07/20 09:49:55  mey
// UPdate
//
// Revision 2.25  2006/07/19 15:24:03  mey
// UPdate
//
// Revision 2.24  2006/07/19 09:05:44  mey
// Added write_CR
//
// Revision 2.23  2006/07/16 04:55:17  liu
// update
//
// Revision 2.22  2006/07/16 04:13:16  liu
// update
//
// Revision 2.21  2006/07/11 12:46:43  mey
// UPdate
//
// Revision 2.20  2006/07/11 09:31:11  mey
// Update
//
// Revision 2.19  2006/07/06 07:31:48  mey
// MPC firmware loading added
//
// Revision 2.18  2006/07/04 15:06:18  mey
// Fixed JTAG
//
// Revision 2.17  2006/06/16 13:05:24  mey
// Got rid of Compiler switches
//
// Revision 2.16  2006/06/15 10:21:06  mey
// Update
//
// Revision 2.15  2006/05/31 04:54:43  liu
// Fixed bugs in initialization
//
// Revision 2.14  2006/05/30 22:49:17  liu
// update
//
// Revision 2.13  2006/05/22 04:49:25  liu
// update
//
// Revision 2.12  2006/05/18 15:11:34  liu
// update error handling
//
// Revision 2.11  2006/05/11 00:00:06  liu
// Update for Production Controller with firmware 3.59
//
// Revision 2.10  2006/03/10 13:13:12  mey
// Jinghua's changes
//
// Revision 2.7  2006/01/09 07:27:15  mey
// Update
//
// Revision 2.6  2005/12/02 18:12:13  mey
// get rid of D360
//
// Revision 2.5  2005/11/30 16:49:49  mey
// Bug DMB firmware load
//
// Revision 2.4  2005/11/30 16:25:42  mey
// Update
//
// Revision 2.3  2005/11/30 12:59:43  mey
// DMB firmware loading
//
// Revision 2.2  2005/11/21 15:47:38  mey
// Update
//
// Revision 2.1  2005/11/02 16:17:01  mey
// Update for new controller
//
// Revision 1.14  2004/07/22 18:52:38  tfcvs
// added accessor functions for DCS integration
//
//
//----------------------------------------------------------------------
#ifndef VMEController_h
#define VMEController_h

using namespace std;
#include <vector>
#include <string>
#include <iostream>
// class Crate;
#include "JTAG_constants.h"
#include <string>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <time.h>
#include <sys/time.h>


class VMEController
{
public:
  VMEController(int crate);
  ~VMEController();

  enum ENDIAN {SWAP, NOSWAP};
  enum {MAXLINE = 70000};
  
  void init(string ipAddr, int port);
  void init();
  void reset();
  int  do_schar(int open_or_close);
  void do_vme(char fcn, char vme,const char *snd,char *rcv, int wrt);

  void SetUseDelay(bool state){usedelay_ = state;}

  string ipAddress() const {return ipAddress_;}
  int port() const {return port_;}

  inline void SetupJtagBaseAddress(int adr) { JtagBaseAddress_ = adr;}
  inline void SetupTCK(int adr) { TCK_ = adr;}
  inline void SetupTMS(int adr) { TMS_ = adr;}
  inline void SetupTDI(int adr) { TDI_ = adr;}
  inline void SetupTDO(int adr) { TDO_ = adr;}

  void start(int slot, int boardtype);
  void end();

 
  /// JTAG stuff
  void devdo(DEVTYPE dev,int ncmd,const char *cmd,int nbuf,const char *inbuf,char *outbuf,int irdsnd);
  void scan(int reg,const char *snd,int cnt2,char *rcv,int ird);
  void scan_reset(int reg, const char *snd, int cnt2, char *rcv,int ird);
  //
  void initDevice(int idev);
  void RestoreIdle();
  void InitJTAG(int port);
  void CloseJTAG();
  void send_last();
  void RestoreIdle_reset();
  void goToScanLevel();
  void release_plev();
  void sdly();
  void RestoreIdle_alct();
  void RestoreIdle_jtag();
  void RestoreReset_jtag();
  void scan_alct(int reg, const char *snd, int cnt, char *rcv,int ird);
  void scan_jtag(int reg, const char *snd, int cnt, char *rcv,int ird);
  //
  void  sleep_vme(const char *outbuf);   // time in usec
  void  sleep_vme(int time); // time in usec
  bool SelfTest();
  bool exist(int slot);
  int error() const {return (error_count<<16)+error_type;}
  void clear_error();
  void disable_errpkt();
  void enable_Reset();
  void write_VME_CR();
  void read_CR();
  void disable_Reset();
  void set_Timeout(int to);
  void set_GrantTimeout(int to);
  void Debug(int dbg) { DEBUG=dbg; }
  int  GetDebug() { return DEBUG; }
  void set_ErrorServer();
  inline void SetPort(int port) {port_=port;}
  inline void SetVMEAddress(std::string address) {ipAddress_=address;}
  inline std::string GetVMEAddress(){return ipAddress_;}
  //
  //
  inline void Set_OkVmeWriteAddress(bool address_ok) { ok_vme_write_ = address_ok; }
  inline bool Get_OkVmeWriteAddress() { return ok_vme_write_; }
  //
  inline void Set_FillVmeWriteVecs(bool fill_vectors_or_not) { fill_write_vme_vectors_ = fill_vectors_or_not; }
  inline bool Get_FillVmeWriteVecs() { return fill_write_vme_vectors_; }
  //
  void Clear_VmeWriteVecs();
  inline std::vector<int> Get_VecVmeAddress() { return write_vme_address_; }
  inline std::vector<int> Get_VecDataLsb() { return write_data_lsb_; }
  inline std::vector<int> Get_VecDataMsb() { return write_data_msb_; }
  //
private:
  bool usedelay_;
  int theSocket;
  std::string ipAddress_;
  int port_;
  int crate_;
  sockaddr_in serv_addr;
  const ENDIAN indian;

  unsigned char hw_source_addr[6];
  unsigned char hw_dest_addr[6];
  struct ethhdr ether_header; 

  char wbuf[9000];
  int nwbuf;
  char rbuf[9000];
  int nrbuf;
  char spebuff[MAXLINE];
  bool done_init_;
  
  int max_buff;
  int tot_buff;
  /// previous fe used
  int feuseo;
  /// JTAG level.  When plev=2, we're in JTAG mode.
  /// if it's not reset when we're done with JTAG,
  /// the Dynatem will hang.
  int plev;
  int idevo;
  int board; //board type
  unsigned long add_ucla; // current VME address for JTAG
  unsigned long vmeadd; // current VME base address for the module
  unsigned short int pvme; // value for ALCT JTAG register (0x70000)
  unsigned long vmeaddo;
  int feuse;
  int ucla_ldev;
  long int packet_delay;
  float fpacket_delay;
  int packet_delay_flg;
  float DELAY2;
  float DELAY3;
  int error_type;
  int error_count;
  int DEBUG;
  int JtagBaseAddress_ ;
  int TCK_, TMS_, TDI_, TDO_;
 
 // I like to keep them private. 
  void load_cdac(const char *snd);
  void vme_adc(int ichp,int ichn,char *rcv);
  //void buckflash(const char *cmd,const char *inbuf,char *rcv);
  void buckflash(const char *cmd,int nbuf,const char *inbuf,char *rcv);
  void lowvolt(int ichp,int ichn,char *rcv);
  void handshake_vme();
  void flush_vme();
  void daqmb_fifo(int irdwr,int ififo,int nbyte,unsigned short int *buf,unsigned char *rcv);
  void vme_controller(int irdwr,unsigned short int *ptr,unsigned short int *data,char *rcv);
  void VME_controller(int irdwr,unsigned short int *ptr,unsigned short int *data,char *rcv);
  void dump_outpacket(int nvme);
  int eth_reset(int ethsocket);
  int eth_read();
  int eth_write();
  void mrst_ff();
  void set_VME_mode();
  void set_cnfg_dflt(int);
  void save_cnfg_num(int);
  void reload_FPGA();
  void get_macaddr(int port);
  void setuse();
  //
  static const int Save_Cnfg_Num = 0x05;
  static const int Set_Cnfg_Dflt = 0x09;
  //
  bool ok_vme_write_;
  //
  bool fill_write_vme_vectors_;
  std::vector<int> write_vme_address_;
  std::vector<int> write_data_lsb_;
  std::vector<int> write_data_msb_;
  //
};

#endif
