#ifndef VMECC_h
#define VMECC_h

#include "VMEModule.h"
#include <iostream>
#include <vector>
#include <string>

typedef struct mac_t {  /* MAC addresses */
        unsigned char device[6];
        unsigned char mcast1[6];
        unsigned char mcast2[6];
        unsigned char mcast3[6];
        unsigned char dflt_srv[6];
} MAC_t;

typedef struct cnfg_t {  /* Configuration elements */
        MAC_t mac;
        unsigned short int ether;
        unsigned short int ext_fifo;
        unsigned short int rst_misc;
        unsigned int vme;
        unsigned short int vme_bto;
        unsigned short int vme_bgto;
} CNFG_t;

typedef CNFG_t *CNFG_ptr;

typedef struct sn_t {  /* Serial Number type structure */
        int status;
        int err_typ;
        int sn;
} SN_t;


class Crate;

class VMECC: public VMEModule
{
 public:

  VMECC(Crate *,int slot) ;

  virtual ~VMECC();
  
   friend std::ostream & operator<<(std::ostream & os, VMECC & vmecc);


  FILE *fplog;
  std::string VCC_frmw_ver;

  unsigned int boardType() const {return VMECC_ENUM;}
  bool SelfTest();
  void init();
  void configure();
  void start(){};
  void end(){};


  //prom programming routines
  inline void SetVCC_frmw_ver(std::string Ver) {VCC_frmw_ver=Ver;}
  void prg_vcc_prom_ver(const char *path,const char *ver);
  void prg_vcc_prom_bcast(const char *path,const char *ver);
  // old jtag_subs.c
  void jtag_init();
  int chk_jtag_conn();
  unsigned int read_dev_id();
  unsigned int read_user_code();
  char *read_customer_code();
  int erase_prom();
  int erase_prom_bcast();
  void program_prom_cmd();
  void program_prom_cmd_no_ack();
  void reload_fpga();
  void verify_prom_cmd();
  void ld_rtn_base_addr(unsigned short base);
  void exec_routine(int rtn);
  unsigned int get_jtag_status();
  void abort_jtag_cmnds();
  void write_jtag_fifo_words(unsigned short *buf, int nw);
  void write_jtag_fifo_bytes(unsigned char *buf, int nb);
  int read_prg_space(unsigned short base);
  std::string print_routines();
  void  rd_back_prom(const char *fn);
  int  read_mcs(const char *fn);
  void send_ver_prom_data();
  void send_prg_prom_data();
  void send_prg_prom_data_bcast();
  void send_uc_cc_data(const char *fn);
// old pkt_utils.c
  char *dcode_msg_pkt(char *buf);
  void *ptr_bin_srch(int code, struct ucw *arr, int n);
  struct rspn_t flush_pkts();
// old cnfg_subs.c
   CNFG_ptr load_cnfg(char *fn);
   int write_cnfg(CNFG_ptr cp, char *fn);
   int set_cnfg_dflt(int dflt);
   int read_cnfg_dflt();
   int save_cnfg_num(int cnum);
   int rstr_cnfg_num(int cnum);
   int read_cnfg_num_dir(int cnum, char **dpp);
   CNFG_ptr read_cnfg_num_dcd(int cnum);
   CNFG_ptr read_crs();
   int set_macs(int mcid, CNFG_ptr cp);
   int read_macs_dir(char **dpp);
   CNFG_ptr read_macs_dcd();
   int wrt_crs(int crid, CNFG_ptr cp);
   int set_clr_bits(int sc, int crid, unsigned int mask);
   int wrt_ser_num(int ser_num);
   SN_t rd_ser_num();
   void cp_macs(CNFG_ptr to, CNFG_ptr from);
   void cp_crs(CNFG_ptr to, CNFG_ptr from);
   void cp_cnfg(CNFG_ptr to, CNFG_ptr from); 
   int cmp_macs(CNFG_ptr exp, CNFG_ptr rbk);
   int cmp_crs(CNFG_ptr exp, CNFG_ptr rbk);
   int cmp_cnfg(CNFG_ptr exp, CNFG_ptr rbk);
   void print_macs(CNFG_ptr cp);
   void print_crs(CNFG_ptr cp);
   void print_cnfg(CNFG_ptr cp);
   void log_mac_diffs(CNFG_ptr cp1, CNFG_ptr cp2);
   void log_cr_diffs(CNFG_ptr cp1, CNFG_ptr cp2);
   void log_cnfg_diffs(CNFG_ptr cp1, CNFG_ptr cp2);
   // old cntrl_subs.c
   int flash_cmds(int nwrds);
   int vme_cmds(int nwrds);
   int wrt_ff(int nwrds);
   int rd_ff(int nwrds, int retrans, unsigned short int *buf);
   int rd_pkt();
   void prst_ff();
   void mrst_ff();
   void set_mark();
   void rst_mark();
   void set_ff_test_mode();
   void set_VME_mode();
   void enable_ECC();
   void disable_ECC();
   void rst_err_cnt();
   void enable_inj_errs();
   void disable_inj_errs();
   void enable_warn_on_shutdown();
   void disable_warn_on_shutdown();
   void snd_startup_pkt();
   void no_startup_pkt();
   void rst_seq_id();
   void force_reload();
   int rd_err_cnts(unsigned int *cor_p, unsigned int *uncor_p);
   int prg_off(unsigned int full_off, unsigned int mt_off);
   int rdbk_off(unsigned int *full_p, unsigned int *mt_p);
   void ld_usr_reg(unsigned int user);
   // new routines for GUIs
   int read_dev_id_broadcast(char *out);
   int vme_read_broadcast(char *out);
   int dcs_read_broadcast(char *out);
   float dcs_convert(int n,char *buf);
 private:

};

#endif
