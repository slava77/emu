//----------------------------------------------------------------------
// $Id: VMEController.h,v 3.31 2008/08/13 11:30:53 geurts Exp $
// $Log: VMEController.h,v $
// Revision 3.31  2008/08/13 11:30:53  geurts
// introduce emu::pc:: namespaces
// remove any occurences of "using namespace" and make std:: references explicit
//
// Revision 3.30  2008/06/19 18:54:40  bylsma
// Added accessor functions for VCC xml parameters
//
// Revision 3.29  2008/06/09 09:38:01  bylsma
// Added Set... routines for new VCC xml parameters, and initialized parameter variables
//
// Revision 3.28  2008/05/28 10:35:30  liu
// DMB counters in jumbo packet
//
// Revision 3.27  2008/05/20 10:46:01  liu
// error handling update
//
// Revision 3.26  2008/04/22 08:32:35  liu
// Ben's Crate controller utilities
//
// Revision 3.25  2008/02/29 08:55:11  liu
// updated delays and error message handling
//
// Revision 3.24  2008/02/19 14:39:32  gujh
// VME Controller fix
//
// Revision 3.23  2008/02/18 12:08:48  liu
// new functions for monitoring
//
// Revision 3.22  2008/02/05 09:27:19  liu
// disable changing VCC configuration in DCS
//
// Revision 3.21  2008/01/31 14:21:44  liu
// config change for firmware 4.x
//
// Revision 3.20  2008/01/24 23:16:11  liu
// update broadcast read
//
// Revision 3.19  2008/01/24 13:10:20  liu
// new controller firmware
//
// Revision 3.18  2008/01/17 16:19:14  rakness
// comment out new VMEController completely
//
// Revision 3.17  2008/01/17 11:54:02  rakness
// comment out added functions
//
// Revision 3.16  2008/01/16 16:02:17  gujh
// function added for EPROM firmware loading
//
// Revision 3.15  2008/01/08 10:58:56  liu
// remove exit() in functions
//
// Revision 3.14  2007/12/27 00:34:26  liu
// update
//
// Revision 3.13  2007/12/17 15:01:44  liu
// remove Crate dependence from VMEController
//
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

#include <vector>
#include <string>
#include <iostream>
#include "JTAG_constants.h"
#include <string>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <time.h>
#include <sys/time.h>

namespace emu {
  namespace pc {

class VMEController
{
public:
  VMEController();
  ~VMEController();

  enum ENDIAN {SWAP, NOSWAP};
  enum {MAXLINE = 70000};


  
  void init(std::string ipAddr, int port);
  void init();
  void reset();
  int  do_schar(int open_or_close);
  void do_vme(char fcn, char vme,const char *snd,char *rcv, int wrt);
  int new_vme(char fcn, unsigned vme, unsigned short data, char *rcv, int when);

  void SetUseDelay(bool state){usedelay_ = state;}
  void SetUseDCS(bool state){useDCS_ = state;}
  void SetWarn_On_Shtdwn(bool state){Warn_On_Shtdwn = state;}
  void SetPkt_On_Startup(bool state){Pkt_On_Startup = state;}
  std::string GetWarn_On_Shtdwn();
  std::string GetPkt_On_Startup();

  void SetMAC(int type, std::string MAC);
  void SetCR(int type, std::string CR);
  std::string GetMAC(int type);
  std::string GetCR(int type);

  std::string ipAddress() const {return ipAddress_;}
  int port() const {return port_;}

  inline void SetupJtagBaseAddress(int adr) { JtagBaseAddress_ = adr;}
  inline void SetupTCK(int adr) { TCK_ = adr;}
  inline void SetupTMS(int adr) { TMS_ = adr;}
  inline void SetupTDI(int adr) { TDI_ = adr;}
  inline void SetupTDO(int adr) { TDO_ = adr;}

  void start(int slot, int boardtype);
  void end();
  inline void SetLife(bool life) { alive_=life; }
  inline bool IsAlive() { return alive_; }

 
  /// JTAG stuff
  void devdo(DEVTYPE dev,int ncmd,const char *cmd,int nbuf,const char *inbuf,char *outbuf,int irdsnd);
  void scan(int reg,const char *snd,int cnt2,char *rcv,int ird);
  void new_devdo(DEVTYPE dev,int ncmd,const char *cmd,int nbuf,const char *inbuf,char *outbuf,int irdsnd);
  void scan_dmb(int reg,const char *snd,int cnt2,char *rcv,int ird, int when);
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
  void disable_Reset();
  void set_Timeout(int to);
  void set_GrantTimeout(int to);
  void Debug(int dbg) { DEBUG=dbg; }
  int  GetDebug() { return DEBUG; }  

  unsigned char GetDestMAC(int i){return hw_dest_addr[i];} 
  unsigned char GetSrcMAC(int i){return hw_source_addr[i];}  
  void write_Ethernet_CR(unsigned short int val);
  void write_FIFO_CR(unsigned short int val);
  void write_ResetMisc_CR(unsigned short int val);
  void write_VME_CR(unsigned int val);
  void write_BusTimeOut_CR(unsigned short int val);
  void write_BusGrantTimeOut_CR(unsigned short int val);
  void vcc_dump_config();
  void vcc_check_config();


  void set_ErrorServer();
  inline void SetMsg_Lvl(int val) {Msg_Lvl=val;}
  inline int  GetMsg_Lvl() {return Msg_Lvl;}
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
 
  char wbuf[9000];
  int nwbuf;
  char rbuf[9000];
  int nrbuf;
  int eth_read();
  int eth_write();
  int eth_read_timeout(int rd_tmo);
  int LeftToRead();
  int NoWriteToEther;
   //
private:
  bool usedelay_;
  bool useDCS_;
  bool Warn_On_Shtdwn;
  bool Pkt_On_Startup;
  int theSocket;
  std::string ipAddress_;
  int port_;
  int Msg_Lvl;
  const ENDIAN indian;

  unsigned char hw_source_addr[6];
  unsigned char hw_dest_addr[6];
  struct ethhdr ether_header;
 
  unsigned char MAC_addr[6];
  unsigned char MCAST_1[6];
  unsigned char MCAST_2[6];
  unsigned char MCAST_3[6];
  unsigned char Dflt_Srv_MAC[6];

  char spebuff[MAXLINE];
  bool done_init_;
  bool alive_;
  int slot_error[22];
 
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
  int error_type;
  int error_count;
  int DEBUG;
  int JtagBaseAddress_ ;
  int TCK_, TMS_, TDI_, TDO_;

  unsigned short CR_ethernet,CR_ext_fifo,CR_res_misc,CR_VME_low,CR_VME_hi,
                 CR_BUS_timeout,CR_BUS_grant;
 
 // I like to keep them private. 
  void udelay(long int itim);
  void load_cdac(const char *snd);
  void vme_adc(int ichp,int ichn,char *rcv);
  //void buckflash(const char *cmd,const char *inbuf,char *rcv);
  void buckflash(const char *cmd,int nbuf,const char *inbuf,char *rcv);
  void lowvolt(int ichp,int ichn,char *rcv);
  void handshake_vme();
  void flush_vme();
  void daqmb_fifo(int irdwr,int ififo,int nbyte,unsigned short int *buf,unsigned char *rcv);
  int vme_controller(int irdwr,unsigned short int *ptr,unsigned short int *data,char *rcv);
  int VME_controller(int irdwr,unsigned short int *ptr,unsigned short int *data,char *rcv);
  void dump_outpacket(int nvme);
  int eth_reset(int ethsocket);
  void mrst_ff();
  void set_VME_mode();
  void get_macaddr(int port);
  void setuse();
  int vcc_read_command(int code, int n_words, unsigned short *readback);
  int vcc_write_command(int code, int n_words, unsigned short *writedata);
  int vcc_write_command(int code);
  //
  //
  bool ok_vme_write_;
  //
  bool fill_write_vme_vectors_;
  std::vector<int> write_vme_address_;
  std::vector<int> write_data_lsb_;
  std::vector<int> write_data_msb_;
  //
};
  } // namespace emu::pc
} // namespace emu

#endif
