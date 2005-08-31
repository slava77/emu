#ifndef GenDATA_h
#define GenDATA_H

#define MXINT 100
#define MXFLT 100
#define MXCHAR 100
#define MXWIDGET 100
#define SAMPLESIZE 16
#define NFPGA 4

typedef struct Data{
  enum { EMACS,VMEPRGPROM,INPRGPROM,DDUPRGPROM,RESETPRGPROM,SETKILLREG,SETBXORBITREG,MPRGPROM,INPRGPROM2,SETRATEDELAY,SETNPERBURST,SETNUMBURST} fsel_funct;
  enum { MFIFO,MTTC,L1ARATE,L1ANUMB} data_funct;
  enum { PLSLOOP, INJLOOP} tst_funct;
  int         slot_ddu;
  int         slot_dcc;
  float       f[MXFLT];
  char       *s[MXCHAR];
  char       *config_file,*config_pfile,*config_prepath_ddu,*config_prepath_dcc,*tmp_dir,*kumac_dir;
  char       *config_downloaded;
  char       *design[NFPGA],*device[NFPGA],*fullpath[NFPGA];
  void       *w[MXWIDGET];
  unsigned short int mfifo,mttc,l1arate,l1anumb;
  unsigned short int rate_delay;
  enum DEVTYPE devnum;
} GenDATA;
#endif

