#ifndef EPROMBadBit_SVFFileFix_h
#define EPROMBadBit_SVFFileFix_h

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <string.h>

namespace emu {
  namespace pc {

#define CRC16POLY 0x8005;

class EPROMBadBit_SVFFileFix{

 public:
    EPROMBadBit_SVFFileFix(int nfix,int *fix,const char *downfile,const char *fixfile);
    ~EPROMBadBit_SVFFileFix();
    int outputfixfile();
    int test();
    void Parse(char *buf,int *Count,char **Word);
    void virtexengine(int pass,unsigned char *snd,unsigned char *rcv,FILE *vein);
    void spartanengine(int pass,unsigned char *snd,unsigned char *rcv,FILE *vein);
    void dshuffle2(unsigned char *a,unsigned char *b,int off);
    void crcupdate(unsigned char *in);
    void fifo(int irw,int n,unsigned char *data);
    void readsvf(int ipass);    
    enum ADDType {svf_NOOP=0,svf_LONGWRT=1,svf_SHORTWRT=2,svf_AUTOCRC=3};
    void insertword(enum ADDType flag,int size);
    void getinversecrc(unsigned char *c);
    void setflags();
 private:
    int NBITSTOREDINSVF; //bits stored in svf file DMB 4096bit CFEB 2048bits
    int NFIXBWRD,FIXBWRD[10],FIXBWRDUSE[10];
    const char *downfile;
    const char *fixfile;
    int ibad;
    int secondtry;
    int v_instr; // expect 0-data 1-instr
    int v_pktyp;  // packet 1-type1  2-type2
    int v_data; // expect data if 1
    int v_count;
    int v_ecount;
    int v_register;
    int v_endcrc;
    uint16_t crc16;
    int ncrcbits;
    int fdri_cnt;
    int tfdri_cnt;
    int fdri_pos[10],fdri_size[10],fdri_ifirst,fdri_ilast;
    int TOTWRDS;
    int nfixoff;

    // fixbit word counter
    int fixbitcount;

    // fifo parameters
    char FIFO[2000];
    int FIFOpnt;

    // checks in pass 2
    int numcrc;
    int crcchk;
    int dontknowchk;
    int notfixed;
    int badcrc;

    // compare readback files
    FILE *cmp,*cmp2;

    // debug out files
    int cmpdebug;
};


  } // namespace emu::pc
} // namespace emu
#endif

