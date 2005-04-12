//-----------------------------------------------------------------------
// $Id: GenDATA.h,v 2.0 2005/04/12 08:07:03 geurts Exp $
// $Log: GenDATA.h,v $
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef GenDATA_h
#define GenDATA_H

#define MXINT 100
#define MXFLT 100
#define MXCHAR 100
#define MXWIDGET 100
#define SAMPLESIZE 16
#define NFPGA 4
#include "JTAG_constants.h"

typedef struct Data{
  enum { CONFIG, EMACS, PRGFPGA, PRGCPLD, PRGPROM, MPRGPROM, PRGVTX,MPRGPROM2,UCLAPRGPROM, ALCTPRGPROM  } fsel_funct;
  enum { DACSET, SETCHAN, HLFSTRP, CMPTHRSH,
         CMPMODE, RDDDU, SETCRTID ,SETCALDLY, SETFEBDLY, SETPBKEND, SETDAVDLY, SETTRGRATE, SETVMESLT, SETDELAY,SETNUMBURST,SETPERBURST,SETCRTCHMBR,SETKILLREG,SETTMBDELAY,SETCCBLVL1DELAY,SETTMBLVL1DELAY,SETREQUESTDELAY} data_funct;
  enum { PLSLOOP, INJLOOP} tst_funct;
  int         i[MXINT],samplesiz,inuse,iuse[5],ihit[5],ichan,patset;
  int         chips_inuse[5],chip_use[5][6];
  int         nfpgas;
  int         crate_id,crate_slt,ddu_slt,ccb_slt,tmb_slt,jtag_port,pre_blk_end,feb_delay,cal_delay,dav_delay,comp_mode;
  float       inj_dac_set,pul_dac_set,set_comp_thresh;
  float       inj_volts,pul_volts,comp_thresh,pul_time;
  float       f[MXFLT];
  char       *s[MXCHAR];
  char       *config_file,*config_pfile,*config_prepath,*tmp_dir,*kumac_dir;
  char       *config_downloaded;
  char       *design[NFPGA],*device[NFPGA],*fullpath[NFPGA];
  void       *w[MXWIDGET];
  enum DEVTYPE devnum,prg_devnum[NFPGA];
  unsigned short int data_ddu[10][SAMPLESIZE][16][6][5];
  unsigned short int data[SAMPLESIZE][16][6][5];
  unsigned short int dbuf[SAMPLESIZE][16][6][5];
  unsigned short int err[SAMPLESIZE][6][5];
  unsigned short int ovr[SAMPLESIZE][6][5];
  unsigned short int lctreg[SAMPLESIZE][6][5];
  unsigned short int block[SAMPLESIZE][6][5];
  unsigned short int stat1[SAMPLESIZE][6][5];
  unsigned short int stat2[SAMPLESIZE][6][5];
  unsigned short int deflg[SAMPLESIZE][6][5];
  unsigned long int bxnum,l1acc;
  unsigned short crt_id,daqmb_id;
  unsigned short int pulse_delay;
} GenDATA;

#endif

