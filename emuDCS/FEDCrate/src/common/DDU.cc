/* New motherboard VTX2 not VTX  so MCTRL,6  not MCTRL,5 */
#include "DDU.h"
#include "VMEController.h"
#include <fstream>
#include <stdio.h>
#include <cmath>
#include <unistd.h>
#include "GenDATA.h"
#include "geom_def.h"

using namespace std;

#ifndef debugV //silent mode
#define PRINT(x) 
#define PRINTSTRING(x)  
#else //verbose mode
#define PRINT(x) cout << #x << ":\t" << x << endl; 
#define PRINTSTRING(x) cout << #x << endl; 
#endif

char filename[100];
unsigned int hexval;
short int intval;
short int intval2;

// declarations
void Parse(char *buf,int *Count,char **Word);
void shuffle(char *a,char *b);


DDU::DDU(int newcrate,int newslot):
  VMEModule(newcrate, newslot)
{
  // cout<<"DDU \n";
}


DDU::~DDU() {
  // cout << "Killing DDDU" << endl;
}


void DDU::end()
{
  cout << "calling DDU::end" << endl;
  theController->send_last();
  VMEModule::end();
}


void DDU::configure() {
}


void DDU::ddu_init()
{
  devdo(DDUFPGA,-1,cmd,0,sndbuf,rcvbuf,2);
}

void DDU::ddu_shfttst(int tst)
{ 
  int shft2in;
  cmd[0]=VTX2P_BYPASS_L;
  cmd[1]=VTX2P_BYPASS_H;
  if(tst==0){
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  }else{
  sndbuf[0]=0xAD;
  sndbuf[1]=0xDE;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  }
  tst=tst+1;if(tst==2)tst=0;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" all %02x %02x %02x %02x %02x \n",0xff&rcvbuf[4],0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
shft2in=(((0x01&rcvbuf[2])<<15)|((0xff&rcvbuf[1])<<7)|(0xfe&rcvbuf[0])>>1);
  printf("   ----> 40-bit FPGA shift test:  sent 0x%02X%02X, got back 0x%04X \n",sndbuf[1]&0xff,sndbuf[0]&0xff,shft2in);
}

void DDU::ddu_lowfpgastat()
{
int i,shft0in,shft1in,shft2in;
long int code;
enum DEVTYPE devstp,dv;
  
  printf(" ddu_lowfpgastat \n");
  cmd[0]=VTX2P_USR1_L;
  cmd[1]=VTX2P_USR1_H;
  sndbuf[0]=4;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);

  cmd[0]=VTX2P_BYPASS_L;
  cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX2P_USR2_L;
  cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  //  printf("   rcvbuf[3:0] = %02x%02x/%02x%02x\n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" 16-low-bits DDU Control FPGA Status:  %02x%02x \n",0xff&rcvbuf[1],0xff&rcvbuf[0]);
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  //shft2in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xff&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }

  ddu_status_decode(code);
  cmd[0]=VTX2P_BYPASS_L;
  cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX2P_USR1_L;
  cmd[1]=VTX2P_USR1_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;
  cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_hifpgastat()
{
int i,shft0in,shft1in,shft2in;
long int code;
enum DEVTYPE devstp,dv;  
  printf(" ddu_hifpgastat \n");
  cmd[0]=VTX2P_USR1_L;
  cmd[1]=VTX2P_USR1_H;
  sndbuf[0]=5;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);

  cmd[0]=VTX2P_BYPASS_L;
  cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX2P_USR2_L;
  cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;  
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" all %02x %02x %02x %02x %02x \n",0xff&rcvbuf[4],0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
  printf(" 16-high-bits DDU Control FPGA Status:  %02x%02x \n",0xff&rcvbuf[1],0xff&rcvbuf[0]);
  code=(((0x00ff&rcvbuf[0])<<16)|((0x00ff&rcvbuf[1])<<24))&0xffff0000;
  //shft2in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xff&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  ddu_status_decode(code);

  cmd[0]=VTX2P_BYPASS_L;
  cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX2P_USR1_L;
  cmd[1]=VTX2P_USR1_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;
  cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_checkFIFOa()
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;  
  printf(" ddu_checkFIFOa (DDU_Ctrl FPGA) \n");
  cmd[0]=VTX2P_USR1_L;
  cmd[1]=VTX2P_USR1_H;
  sndbuf[0]=7;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;
  cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" FIFO-A Status [15-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  for(j=15;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/4)*4==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  printf("                StuckData.FIFOerr.L1err.FIFOactive\n");
  printf("     DDR Vote:  StuckData.FIFOerr.V_Err.FIFOactive\n");
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_checkFIFOb()
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;
  printf(" ddu_checkFIFOb (DDU_Ctrl FPGA) \n");
  cmd[0]=VTX2P_USR1_L;
  cmd[1]=VTX2P_USR1_H;
  sndbuf[0]=8;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;
  cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" FIFO-B Status [14-0]:  "); //bit15 is always set high, ignore
  code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;
  for(j=14;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if(j==10)printf("/");
    if(j==14)printf(".");
    if((j/4)*4==j&&j>0&&j<9)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  printf("                    EmptyFIFOs/AlmostFullFIFOs\n");
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_checkFIFOc()
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;
  printf(" ddu_checkFIFOc (DDU_Ctrl FPGA) \n");
  cmd[0]=VTX2P_USR1_L;
  cmd[1]=VTX2P_USR1_H;
  sndbuf[0]=11;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;
  cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" FIFO-C Status [15-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  for(j=15;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/4)*4==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  printf("            Timeout-EA.Timeout-EW.Timeout-ST.LostInData\n");
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_rdfibererr()
//JRG: Delete
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;  
  printf(" ddu_rdfibererr \n");
  cmd[0]=VTX2P_USR1_L;
  cmd[1]=VTX2P_USR1_H;
  sndbuf[0]=6;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;
  cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;
  cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" FiberErr Status bits [14-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;
  for(j=14;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P_BYPASS_L;
  cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;
  cmd[1]=VTX2P_USR1_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;
  cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_rdfiberok()
//JRG: Delete
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;

  
  printf(" ddu_rdfiberok \n");
  cmd[0]=VTX2P_USR1_L;
  cmd[1]=VTX2P_USR1_H;
  sndbuf[0]=7;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;
  cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" FiberOK Status bits [14-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;
  for(j=14;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_rdkillfiber()
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;  
  printf("    ddu_rdkillfiber \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=13;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  sndbuf[6]=0x00;
  sndbuf[7]=0x00;
  devdo(DDUFPGA,10,cmd,44,sndbuf,rcvbuf,1);
  printf(" KillFiber Register bits [19-0]:\n                       ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8)|((0x000f&rcvbuf[2])<<16))&0x000fffff;
  for(j=19;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %05x\n",code);

  printf("       key:  b17==TMB    b16==ALCT    b[14:0]==DDU Inputs\n");
  if((code&0x00010000)==0)printf("                        ALCT readout decode is disabled\n");
  if((code&0x00020000)==0)printf("                        TMB readout decode is disabled\n");

  shft2in=(((0x0f&rcvbuf[4])<<12)|((0xff&rcvbuf[3])<<4)|(0xf0&rcvbuf[2])>>4);
  printf("   ----> 44-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_loadkillfiber(int regval)
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;  
  printf("    ddu_loadkillfiber,  Received value=%X \n",regval);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=14;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
//There are 15 fibers....test load "4CAB" in register:
/*sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x00;
  sndbuf[3]=0xAB;
  sndbuf[4]=0x4C;*/
//Default load "e7FEF" in kill reg:
//                disable ALCT, enable TMB & all fibers except bad #4:
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x00;
  sndbuf[3]=0xEF;
  sndbuf[4]=0x7F;
  sndbuf[5]=0x0E;
  sndbuf[6]=0x00;
  sndbuf[7]=0x00;
  if(regval>=0&&regval<=0x000FFFFF){
    sndbuf[3]= 0x000000FF&regval;
    sndbuf[4]=(0x0000FF00&regval)>>8;
    sndbuf[5]=(0x000F0000&regval)>>16;
  }
  printf("          Sending to KillFiber:  %01X%02X%02X \n",0x000F&sndbuf[5],0x00FF&sndbuf[4],0x00FF&sndbuf[3]);
  devdo(DDUFPGA,10,cmd,44,sndbuf,rcvbuf,1);
  printf(" readback of Previous KillFiber setting, bits [19-0]:\n                       ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8)|((0x000f&rcvbuf[2])<<16))&0x000fffff;
  for(j=19;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %05x\n",code);

  printf("       key:  b17==TMB    b16==ALCT    b[14:0]==DDU Inputs\n");
  //  if((code&0x00010000)==0)printf("                        ALCT readout decode was disabled\n");
  //  if((code&0x00020000)==0)printf("                        TMB readout decode was disabled\n");

//  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
//  shft2in=(((0x1f&rcvbuf[4])<<11)|((0xff&rcvbuf[3])<<3)|(0xe0&rcvbuf[2])>>5);
  shft2in=(((0x0f&rcvbuf[4])<<12)|((0xff&rcvbuf[3])<<4)|(0xf0&rcvbuf[2])>>4);
  printf("   ----> 44-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_rdcrcerr()
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;
  printf(" ddu_rdcrcerr \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=10;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" CRC Error Register bits [14-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;
  for(j=14;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  shft2in=(((0x7f&rcvbuf[3])<<9)|((0xff&rcvbuf[2])<<1)|((0x80&rcvbuf[1])>>7));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_rdl1aerr()
//JRG: Delete
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;  
  printf(" ddu_rdl1aerr \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=11;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" L1A Mismatch Error Register bits [14-0]:\n                       ");
  code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;
  for(j=14;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
//  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  shft2in=(((0x7f&rcvbuf[3])<<9)|((0xff&rcvbuf[2])<<1)|((0x80&rcvbuf[1])>>7));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_rdxmiterr()
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;  
  printf(" ddu_rdxmiterr \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=12;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,104,sndbuf,rcvbuf,1);
  //  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" Data Transmit Error Register bits [14-0]:\n                       ");
  code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;
  for(j=14;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  //  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  shft2in=(((0x7f&rcvbuf[3])<<9)|((0xff&rcvbuf[2])<<1)|((0x80&rcvbuf[1])>>7));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }

  printf("For DDR Vote Error report [63-0]:");
/*
  code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;
  for(j=14;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
*/
  printf("  Hex %02x%02x %02x%02x %02x%02x %02x%02x\n",rcvbuf[7]&0x00ff,rcvbuf[6]&0x00ff,rcvbuf[5]&0x00ff,rcvbuf[4]&0x00ff,rcvbuf[3]&0x00ff,rcvbuf[2]&0x00ff,rcvbuf[1]&0x00ff,rcvbuf[0]&0x00ff);
  shft2in=(((0xff&rcvbuf[9])<<8)|((0xff&rcvbuf[8])));
  printf("   ----> 104-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<11;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }


  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}



void DDU::ddu_rdtimesterr()
//JRG: Delete
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;  
  printf(" ddu_rdtimesterr \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=13;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" Timeout-start Error Reg bits [14-0]:\n                       ");
  code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;
  for(j=14;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  //  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  shft2in=(((0x7f&rcvbuf[3])<<9)|((0xff&rcvbuf[2])<<1)|((0x80&rcvbuf[1])>>7));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_rdtimeewerr()
//JRG: Delete
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;  
  printf(" ddu_rdtimeewerr \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=14;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" Timeout-end-wait Error Reg bits [14-0]:\n                       ");
  code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;
  for(j=14;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  //  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  shft2in=(((0x7f&rcvbuf[3])<<9)|((0xff&rcvbuf[2])<<1)|((0x80&rcvbuf[1])>>7));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_rdtimeeaerr()
//JRG: Delete
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;
  printf(" ddu_rdtimeeaerr \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=15;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" Timeout-end-active Error Reg bits [14-0]:\n                       ");
  code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;
  for(j=14;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  //  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  shft2in=(((0x7f&rcvbuf[3])<<9)|((0xff&rcvbuf[2])<<1)|((0x80&rcvbuf[1])>>7));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_rddmberr()
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;
  printf(" ddu_rddmberr \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=15;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" DMB Error Register bits [14-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;
  for(j=14;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  //  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  shft2in=(((0x7f&rcvbuf[3])<<9)|((0xff&rcvbuf[2])<<1)|((0x80&rcvbuf[1])>>7));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_rdtmberr()
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;  
  printf(" ddu_rdtmberr \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=16;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" TMB Error Register bits [14-0]: ");
  code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;
  for(j=14;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(", Hex code %04x\n",code);
  //  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  shft2in=(((0x7f&rcvbuf[3])<<9)|((0xff&rcvbuf[2])<<1)|((0x80&rcvbuf[1])>>7));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_rdlieerr()
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;
  printf(" ddu_rdlieerr \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=18;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" Lost-In-Event Error Register bits [14-0]:\n                       ");
  code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;
  for(j=14;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  //  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  shft2in=(((0x7f&rcvbuf[3])<<9)|((0xff&rcvbuf[2])<<1)|((0x80&rcvbuf[1])>>7));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_rdliderr()
//JRG: Delete
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;  
  printf(" ddu_rdliderr \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=19;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" Lost-In-Data Error Register bits [14-0]:\n                       ");
  code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;
  for(j=14;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  //  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  shft2in=(((0x7f&rcvbuf[3])<<9)|((0xff&rcvbuf[2])<<1)|((0x80&rcvbuf[1])>>7));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_rdpaferr()
//JRG: Delete
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;  
  printf(" ddu_rdpaferr \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=20;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" FIFO Almost Full Register bits [15-0]:\n                       ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  for(j=15;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  shft2in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_rdfferr()
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;  
  printf(" ddu_rdfferr \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=9;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" FIFO Full Register bits [9-0]:\n                       ");
  code=((0x00ff&rcvbuf[0])|((0x0003&rcvbuf[1])<<8))&0x000003ff;
  for(j=9;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/4)*4==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  shft2in=(((0x03&rcvbuf[3])<<14)|((0xff&rcvbuf[2])<<6)|(0xfc&rcvbuf[1])>>2);
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_rderareg()
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;  
  printf(" ddu_rderareg \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=22;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" Error bus A Register bits [15-0]:\n                       ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  for(j=15;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/4)*4==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  //  shft2in=( ((0xff&rcvbuf[3])<<8)|(0xff&rcvbuf[2]) );
  //JRG, 8 June 2003: occassional bad timing (bit loss) seen in VME readout
  //shft2in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  ddu_era_decode(code);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_rderbreg()
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;
  printf(" ddu_rderbreg \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=23;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" Error bus B Register bits [15-0]:\n                       ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  for(j=15;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/4)*4==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  //  shft2in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  ddu_erb_decode(code);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_rdercreg()
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;  
  printf(" ddu_rdercreg \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=24;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" Error bus C Register bits [15-0]:\n                       ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  for(j=15;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/4)*4==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  //  shft2in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  ddu_erc_decode(code);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_rd_verr_cnt()
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;  
  printf(" ddu_rd_verr_cnt \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=25;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  printf(" DDR Vote Error Count = %d,  Hex code %04x\n",code,code);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_rd_cons_cnt()
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;  
  printf(" ddu_rd_Consec-verr_cnt \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=26;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  printf(" DDR Consecutive-Vote-Error Count = %d,  Hex code %04x\n",code,code);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_fifo0verr_cnt()
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code; 
  printf(" ddu_rd_fifo0verr_cnt \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=27;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  printf(" DDR FIFO-0 Error Count = %d,  Hex code %04x\n",code,code);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_fifo1verr_cnt()
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;  
  printf(" ddu_rd_fifo1verr_cnt \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=28;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  printf(" DDR FIFO-1 Error Count = %d,  Hex code %04x\n",code,code);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_earlyVerr_cnt()
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;
  printf(" ddu_rd_earlyVerr_cnt \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=19;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  printf(" DDR Early 200ns  Error Count = %d,  Hex code %04x\n",code,code);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_verr23cnt()
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code; 
  printf(" ddu_verr23cnt \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=20;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  printf(" DDR VoteError23 Count = %d,  Hex code %04x\n",code,code);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_verr55cnt()
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code; 
  printf(" ddu_verr55cnt \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=21;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  printf(" DDR VoteError55 Count = %d,  Hex code %04x\n",code,code);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_rdostat()
{
int i,shft0in,shft1in,shft2in;
long int code;
enum DEVTYPE devstp,dv;  
  printf(" ddu_rdostat \n");

  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
//JRG, pre-v57:  sndbuf[0]=24;
//JRG-new, v57+:  sndbuf[0]=27;
//JRG-DDU4:
  sndbuf[0]=6;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);

  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  printf(" 16-bit DDU Output Status:  %02x%02x \n",0xff&rcvbuf[1],0xff&rcvbuf[0]);
  //  shft2in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  ddu_ostatus_decode(code);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_rdempty()
//JRG: Delete
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;  
  printf(" ddu_rdempty \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=25;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" FIFO Empty Register bits [15-0]:\n                       ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  for(j=15;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  //  shft2in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_rdstuckbuf()
//JRG: Delete
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;  
  printf(" ddu_rdstuckbuf \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=26;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" Data-Stuck-In-FIFO Register bits [14-0]:\n                       ");
  code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;
  for(j=14;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}




void DDU::ddu_rdscaler()
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;
  printf(" ddu_rdscaler \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=2;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,49,sndbuf,rcvbuf,1);
  printf(" DDU L1 Event Scaler, bits [23-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8)|((0x00ff&rcvbuf[2])<<16))&0x00ffffff;
  printf("    Hex code %06x\n",code);
  printf("    Decimal count =  %8d\n",code);
  shft2in=(((0xff&rcvbuf[4])<<8)|((0xff&rcvbuf[3])));
  printf("   ----> 49-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_rdalcterr()
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;  
  printf(" ddu_rdalcterr \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=17;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" ALCT Error Register bits [14-0]: ");
  code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;
  for(j=14;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(", Hex code %04x\n",code);
  //  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  shft2in=(((0x7f&rcvbuf[3])<<9)|((0xff&rcvbuf[2])<<1)|((0x80&rcvbuf[1])>>7));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}




void DDU::ddu_loadbxorbit(int regval)
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code;
  printf("    ddu_loadbxorbit,  Received value=%d \n",regval);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=29;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x00;
  sndbuf[3]=0xEB;
  sndbuf[4]=0xBD;
  sndbuf[5]=0x39;
  sndbuf[6]=0xEF;
  sndbuf[7]=0x7F;
  if(regval>=0&&regval<=0x00000FFF){
    sndbuf[3]= 0x000000FF&regval;
    sndbuf[4]=(0x00000F00&regval)>>8;
  }
  printf("          Sending to BX_Orbit:  %01X%02X \n",0x000F&sndbuf[4],0x00FF&sndbuf[3]);
  devdo(DDUFPGA,10,cmd,36,sndbuf,rcvbuf,1);
  printf(" readback of Previous BX_Orbit setting, bits [11-0]:\n");
  code=((0x00ff&rcvbuf[0])|((0x000f&rcvbuf[1])<<8))&0x00000fff;
  //  printf("    Hex code %03x\n",code);
  printf("    BX/Orbit = %d  (%03x hex)\n",code,code);

//  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
//  shft2in=(((0x1f&rcvbuf[3])<<11)|((0xff&rcvbuf[2])<<3)|(0xe0&rcvbuf[1])>>5);
  shft2in=(((0x0f&rcvbuf[3])<<12)|((0xff&rcvbuf[2])<<4)|(0xf0&rcvbuf[1])>>4);
  printf("   ----> 36-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}




void DDU::ddu_rdbxorbit()
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code; 
  printf("    ddu_rdbxorbit \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=30;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  sndbuf[6]=0x00;
  sndbuf[7]=0x00;
  devdo(DDUFPGA,10,cmd,41,sndbuf,rcvbuf,1);
  printf(" BX_Orbit Register bits [11-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x000f&rcvbuf[1])<<8))&0x00000fff;
  printf("    Hex code %03x\n",code);
  printf("    Decimal BX/Orbit = %d\n",code);
  //  printf("  Hex code %03x\n",code);

  //  printf("   ----> 44-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  shft2in=(((0x0f&rcvbuf[3])<<12)|((0xff&rcvbuf[2])<<4)|(0xf0&rcvbuf[1])>>4);
  printf("   ----> 41-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}




void DDU::ddu_lvl1onoff()
{
int i,j,k,n,shft0in,shft1in,shft2in;
enum DEVTYPE devstp,dv;
long int code; 
  printf("    DDU Toggle Cal L1A \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=31;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);

  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);

  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_rd_boardID()
{
int i,shft0in,shft1in,shft2in;
long int code;
enum DEVTYPE devstp,dv;
  printf(" ddu_rd_boardID \n");
  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=32;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);

  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  printf(" 16-bit DDU Board ID:  %02x%02x \n",0xff&rcvbuf[1],0xff&rcvbuf[0]);
  //  shft2in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::ddu_fpgastat()
{
int i,shft0in,shft1in,shft2in;
long int errcode;
enum DEVTYPE devstp,dv;
  printf(" ddu_fpgastat \n");

  cmd[0]=VTX2P_USR1_L;
  cmd[1]=VTX2P_USR1_H;
  sndbuf[0]=3;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;
  cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX2P_USR2_L;
  cmd[1]=VTX2P_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  sndbuf[6]=0xFE;
  sndbuf[7]=0xCA;
  sndbuf[8]=0x01;
  sndbuf[9]=0x00;
  sndbuf[10]=0x00;
  sndbuf[11]=0x00;
  sndbuf[12]=0x00;
  sndbuf[13]=0x00;
  sndbuf[14]=0x00;
  sndbuf[15]=0x00;
  sndbuf[16]=0x00;
  devdo(DDUFPGA,10,cmd,70,sndbuf,rcvbuf,1);
  printf(" 32-bit DDU Control DFPGA Status:  %02x%02x/%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
  errcode=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8)|((0x00ff&rcvbuf[2])<<16)|((0x00ff&rcvbuf[3])<<24));
  shft2in=(((0x01&rcvbuf[6])<<15)|((0xff&rcvbuf[5])<<7)|(0xfe&rcvbuf[4])>>1);
  //  shft2in=(((0xff&rcvbuf[5])<<8)|(0xff&rcvbuf[4]));
  shft2in=(((0xff&rcvbuf[5])<<8)|(0xff&rcvbuf[4]));
  printf("   ----> 70-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<11;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  ddu_status_decode(errcode);
  cmd[0]=VTX2P_BYPASS_L;
  cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX2P_USR1_L;
  cmd[1]=VTX2P_USR1_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;
  cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
}

void DDU::ddu_reset()
{
  short int pause;
  
  cmd[0]=VTX2P_USR1_L;
  cmd[1]=VTX2P_USR1_H;
  sndbuf[0]=DDUFPGA_RST;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;
  cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  

  cmd[0]=VTX2P_USR1_L;
  cmd[1]=VTX2P_USR1_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;
  cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
  printf(" DDUFPGA reset done. \n");
}


void DDU::ddu_vmel1a()
{
  short int pause;
  
  cmd[0]=VTX2P_USR1_L;
  cmd[1]=VTX2P_USR1_H;
  sndbuf[0]=33;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;
  cmd[1]=VTX2P_BYPASS_H;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
  

  cmd[0]=VTX2P_USR1_L;
  cmd[1]=VTX2P_USR1_H;
  sndbuf[0]=NORM_MODE;
  devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P_BYPASS_L;
  cmd[1]=VTX2P_BYPASS_H;
  sndbuf[0]=0;
  devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,2);
  printf(" DDUFPGA VME L1A done. \n");
}

void DDU::ddu_status_decode(int long code)
{
// JRG, some problem here: need 32-bit int for code, LONG INT?

//printf("\nReceived code=%08X\n",code);

// if((code&0x008FFFFF)>0){
/*OLD, ignore....
Non-triggered^^^^^ status bits:
	      bit[31:28] = Private gigabit Ethernet status
	      bit[27:24] = details related to a triggered error
	      bit[23:21] = less important status/warning bits
*/

// JRG, low-order 16-bit status (most serious errors):
 if((code&0x0000F000)>0){
   if((0x00008000&code)>0)printf("   DDU Critical Error, ** needs reset **\n");
   if((0x00004000&code)>0)printf("   DDU Single Error, bad event");
   if((0x00002000&code)>0)printf("   DDU Single Warning");
   if((0x00001000&code)>0)printf("   DDU Near Full Warning");
   printf("\n");
 }
 if((code&0x00000F00)>0){
   if((0x00000800&code)>0)printf("   DDU RX Error");
   if((0x00000400&code)>0)printf("   DDU Control DLL Error occured");
   if((0x00000200&code)>0)printf("   DDU DMB Error occurred");
   if((0x00000100&code)>0)printf("   DDU Lost In Event Error");
   printf("\n");
 }
 if((code&0x000000F0)>0){
   if((0x00000080&code)>0)printf("   DDU Lost In Data Error occurred");
   if((0x00000040&code)>0)printf("   DDU Timeout Error");
// Multiple-bit vote failures (or Rx Errors) in one 64-bit word:
//   if((0x00000020&code)>0)printf("   DDU Critical Data Error");
   if((0x00000020&code)>0)printf("   DDU Trigger CRC Error");
// Multiple single-bit vote failures (or Rx Errors) over time from one DMB:
   if((0x00000010&code)>0)printf("   DDU Multiple Transmit Errors");
   printf("\n");
 }
 if((code&0x0000000F)>0){
   if((0x00000008&code)>0)printf("   DDU FIFO Full Error");
   if((0x00000004&code)>0)printf("   DDU Fiber Error");
   if((0x00000002&code)>0)printf("   DDU L1A Match Error");
   if((0x00000001&code)>0)printf("   DDU CRC Error");
   printf("\n");
 }
 if((code&0xF0000000)>0){
// JRG, high-order 16-bit status (not-so-serious errors):
/*Pre-ver51 encoding:
   if((0x80000000&code)>0)printf("   DDU G-Bit FIFO Not Empty");
   if((0x40000000&code)>0)printf("   DDU G-Bit FIFO Near Full Warning");
   if((0x20000000&code)>0)printf("   DDU G-Bit FIFO Full Warning");
*/
   if((0x80000000&code)>0)printf("   DDU Output Limited Buffer Overflow");
   if((0x40000000&code)>0)printf("   DDU G-Bit FIFO Full Warning");
//pre-ddu3ctrl_v12:   if((0x20000000&code)>0)printf("   DDU G-Bit FIFO Near Full Warning");
   if((0x20000000&code)>0)printf("   DDU Ethernet Xmit Limit flag");
   if((0x10000000&code)>0)printf("   DDU G-Bit Fiber Error");
   printf("\n");
 }
 if((code&0x0F000000)>0){
   if((0x08000000&code)>0)printf("   DDU FirstDat Error");
//Pre-ddu3ctrl_v8r15576:   if((0x04000000&code)>0)printf("   DDU BadFirstWord Error");
   if((0x04000000&code)>0)printf("   DDU L1A-FIFO Full Error");
//Pre-ddu2ctrl_ver53:   if((0x02000000&code)>0)printf("   DDU BadCtrlWord Error");
   if((0x02000000&code)>0)printf("   DDU Data Stuck in FIFO");
   if((0x01000000&code)>0)printf("   DDU NoLiveFibers Error");
   printf("\n");
 }
 if((code&0x00F00000)>0){
   if((0x00800000&code)>0)printf("   DDU Spwd single-bit Warning");
   if((0x00400000&code)>0)printf("   DDU Ethernet DLL Error");
   if((0x00200000&code)>0)printf("   DDU S-Link Full Bit set");
   if((0x00100000&code)>0)printf("   DDU S-Link Not Ready");
   if((0x00300000&code)==0x00200000)printf("\n     DDU S-Link Stopped (backpressure)");
   printf("\n");
 }
 if((code&0x000F0000)>0){
   if((0x00080000&code)>0)printf("   DDU TMB Error");
/*Pre-ddu2ctrl_ver57:
   if((0x00040000&code)>0)printf("   DDU FIFO-PAF Warning");
   if((0x00020000&code)>0)printf("   DDU L1A-FIFO Near Full Warning");
*/
/*
Pre-ddu3ctrl_v8r15576:
   if((0x00040000&code)>0)printf("   DDU Trigger Readout CRC Error");
   if((0x00020000&code)>0)printf("   DDU Trigger Readout Wordcount Error");
   if((0x00010000&code)>0)printf("   DDU L1A-FIFO Full Error");
*/
   if((0x00040000&code)>0)printf("   DDU ALCT Error");
   if((0x00020000&code)>0)printf("   DDU Trigger Readout Wordcount Error");
   if((0x00010000&code)>0)printf("   DDU Trigger L1A Match Error");
   printf("\n");
 }
}




void DDU::ddu_ostatus_decode(int long code)
{
//printf("\nReceived code=%08X\n",code);
// JRG, 16-bit DDU output path status:
 if((code&0x0000F000)>0){
   if((0x00008000&code)>0)printf("   DDU Output Limited Buffer Overflow Error occurred\n");
   if((0x00004000&code)>0)printf("   DDU SLink Wait occurred");
   if((0x00002000&code)>0)printf("   DDU SLink Full occurred");
//JRG, before ver52:   if((0x00001000&code)>0)printf("   DDU SLink Ready occurred");
   if((0x00001000&code)>0)printf("   DDU SLink Never Ready");
   printf("\n");
 }
 if((code&0x00000F00)>0){
   if((0x00000800&code)>0)printf("   DDU Gigabit Ethernet Overflow occurred");
   if((0x00000400&code)>0)printf("   DDU Gigabit Ethernet Xmit Limit occurred");
//JRG, before ver52:   if((0x00000200&code)>0)printf("   DDU Gigabit Ethernet FIFO Not Empty occurred");
   if((0x00000200&code)>0)printf("   DDU Gigabit Ethernet FIFO Always Empty");
   if((0x00000100&code)>0)printf("   DDU Gigabit Ethernet Fiber Error occurred");
   printf("\n");
 }
 if((code&0x000000F0)>0){
   if((0x00000080&code)>0)printf("   DDU SLink Limited Overflow occurred");
   if((0x00000040&code)>0)printf("   DDU SLink Wait");
   if((0x00000020&code)>0)printf("   DDU SLink Full");
//JRG, before ver52:   if((0x00000010&code)>0)printf("   DDU SLink Ready");
   if((0x00000010&code)>0)printf("   DDU SLink Not Ready");
   printf("\n");
 }
 if((code&0x0000000F)>0){
   if((0x00000008&code)>0)printf("   DDU Gigabit Ethernet FIFO Full");
   if((0x00000004&code)>0)printf("   DDU Gigabit Ethernet Xmit Limit flag");
   if((0x00000002&code)>0)printf("   DDU Gigabit Ethernet FIFO Not Empty");
   if((0x00000001&code)>0)printf("   DDU Gigabit Ethernet Fiber Error");
   printf("\n");
 }

}





void DDU::ddu_era_decode(int long code)
{
//printf("\nReceived code=%08X\n",code);
// JRG, 16-bit DDU Error Bus A:
 if((code&0x0000F000)>0){
   if((0x00008000&code)>0)printf("   DDU Critical Error, ** needs reset **\n");
   if((0x00004000&code)>0)printf("   DDU DMB Error occurred");
   if((0x00002000&code)>0)printf("   DDU L1A-FIFO Near Full Warning");
   if((0x00001000&code)>0)printf("   DDU Gigabit Ethernet FIFO PAF flag");
   printf("\n");
 }
 if((code&0x00000F00)>0){
   if((0x00000800&code)>0)printf("   DDU Input-FIFO Near Full Warning");
   if((0x00000400&code)>0)printf("   DDU Near Full Warning");
   if((0x00000200&code)>0)printf("   DDU CFEB-CRC not OK");
   if((0x00000100&code)>0)printf("   DDU CFEB-CRC End Error");
   printf("\n");
 }
 if((code&0x000000F0)>0){
   if((0x00000080&code)>0)printf("   DDU CFEB-CRC Count Error");
   if((0x00000040&code)>0)printf("   DDU CFEB-CRC Error occurred");
   if((0x00000020&code)>0)printf("   DDU Latched Trigger Trail");
   if((0x00000010&code)>0)printf("   DDU Trigger Trail_Done");
   printf("\n");
 }
 if((code&0x0000000F)>0){
   if((0x00000008&code)>0)printf("   DDU Trigger Readout Error");
   if((0x00000004&code)>0)printf("   DDU End Timeout");
   if((0x00000002&code)>0)printf("   DDU Start Timeout");
   if((0x00000001&code)>0)printf("   DDU Timeout Error occurred");
   printf("\n");
 }
}





void DDU::ddu_erb_decode(int long code)
{
//printf("\nReceived code=%08X\n",code);
// JRG, 16-bit DDU Error Bus B:
 if((code&0x0000F000)>0){
   if((0x00008000&code)>0)printf("   DDU Lost In Event Error");
   if((0x00004000&code)>0)printf("   DDU DMB Error occurred");
   if((0x00002000&code)>0)printf("   DDU Control DLL Error occured");
   if((0x00001000&code)>0)printf("   DDU 2nd Header First flag");
   printf("\n");
 }
 if((code&0x00000F00)>0){
   if((0x00000800&code)>0)printf("   DDU Early 2nd Trailer flag");
   if((0x00000400&code)>0)printf("   DDU Extra 1st Trailer flag");
   if((0x00000200&code)>0)printf("   DDU Extra Trigger Trailer flag");
   if((0x00000100&code)>0)printf("   DDU Extra 2nd Header flag");
   printf("\n");
 }
 if((code&0x000000F0)>0){
   if((0x00000080&code)>0)printf("   DDU CFEB-DMB Error flag");
   if((0x00000040&code)>0)printf("   DDU First Header flag");
   if((0x00000020&code)>0)printf("   DDU Lone Word Event flag");
   if((0x00000010&code)>0)printf("   DDU Bad Control Word Error occurred");
   printf("\n");
 }
 if((code&0x0000000F)>0){
   if((0x00000008&code)>0)printf("   DDU Missed Trigger Trailer Error occurred");
   if((0x00000004&code)>0)printf("   DDU First Dat Error");
   if((0x00000002&code)>0)printf("   DDU Bad First Word");
   if((0x00000001&code)>0)printf("   DDU Lost In Data occured");
   printf("\n");
 }
}





void DDU::ddu_erc_decode(int long code)
{
//printf("\nReceived code=%08X\n",code);
// JRG, 16-bit DDU Error Bus C:
 if((code&0x0000F000)>0){
   if((0x00008000&code)>0)printf("   DDU Trigger Readout Error");
   if((0x00004000&code)>0)printf("   DDU ALCT Trailer Done");
   if((0x00002000&code)>0)printf("   DDU ALCT DAV Vote True occurred");
   if((0x00001000&code)>0)printf("   DDU do_ALCT flag");
   printf("\n");
 }
 if((code&0x00000F00)>0){
   if((0x00000800&code)>0)printf("   DDU ALCT CRC Error");
   if((0x00000400&code)>0)printf("   DDU ALCT Wordcount Error");
   if((0x00000200&code)>0)printf("   DDU Missed ALCT Trailer");
   if((0x00000100&code)>0)printf("   DDU ALCT Error");
   printf("\n");
 }
 if((code&0x000000F0)>0){
   if((0x00000080&code)>0)printf("   DDU Compare Trigger CRC flag");
   if((0x00000040&code)>0)printf("   DDU TMB Trailer Done");
   if((0x00000020&code)>0)printf("   DDU TMB DAV Vote True occurred");
   if((0x00000010&code)>0)printf("   DDU do_TMB flag");
   printf("\n");
 }
 if((code&0x0000000F)>0){
   if((0x00000008&code)>0)printf("   DDU TMB CRC Error");
   if((0x00000004&code)>0)printf("   DDU TMB Word Count Error");
   if((0x00000002&code)>0)printf("   DDU Missed TMB Trailer");
   if((0x00000001&code)>0)printf("   DDU TMB Error");
   printf("\n");
 }
}

unsigned long int  DDU::ddufpga_idcode()
{
int i;
enum DEVTYPE devstp,dv;  
// printf(" DDU Virtex PRO ID should be  xxxxx \n");
  dv=DDUFPGA;
      cmd[0]=VTX2P_IDCODE_L;
      cmd[1]=VTX2P_IDCODE_H;
      sndbuf[0]=0xFF;
      sndbuf[1]=0xFF;
      sndbuf[2]=0xFF;
      sndbuf[3]=0xFF;
      sndbuf[4]=0xFF;
      devdo(dv,10,cmd,32,sndbuf,rcvbuf,1);
      // printf(" The DDU FPGA Chip IDCODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
      cmd[0]=VTX2P_BYPASS_L;
      cmd[1]=VTX2P_BYPASS_H;
      sndbuf[0]=0;
      devdo(dv,10,cmd,0,sndbuf,rcvbuf,0); 
      unsigned long int ibrd=0x00000000;
      ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24)|ibrd;
      return ibrd;

}

unsigned long int DDU::infpga_idcode0()
{
int i;
enum DEVTYPE devstp,dv;  
// printf(" INFPGA Virtex PRO ID should be  xxxxx \n");
  dv=INFPGA0;
      cmd[0]=VTX2P20_IDCODE_L;
      cmd[1]=VTX2P20_IDCODE_H;
      sndbuf[0]=0xFF;
      sndbuf[1]=0xFF;
      sndbuf[2]=0xFF;
      sndbuf[3]=0xFF;
      sndbuf[4]=0xFF;
      devdo(dv,14,cmd,32,sndbuf,rcvbuf,1); 
      // printf(" The INFPGA-0 Chip IDCODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
      cmd[0]=VTX2P20_BYPASS_L;
      cmd[1]=VTX2P20_BYPASS_H;
      sndbuf[0]=0;
      unsigned long int ibrd=0x00000000;
      ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24)|ibrd;
      return ibrd;
      devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
}

unsigned long int DDU::infpga_idcode1()
{
int i;
enum DEVTYPE devstp,dv;  
  dv=INFPGA1;
      cmd[0]=VTX2P20_IDCODE_L;
      cmd[1]=VTX2P20_IDCODE_H;
      sndbuf[0]=0xFF;
      sndbuf[1]=0xFF;
      sndbuf[2]=0xFF;
      sndbuf[3]=0xFF;
      sndbuf[4]=0xFF;
      devdo(dv,14,cmd,32,sndbuf,rcvbuf,1);
      //  printf(" The INFPGA-1 Chip IDCODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
      cmd[0]=VTX2P20_BYPASS_L;
      cmd[1]=VTX2P20_BYPASS_H;
      sndbuf[0]=0;
      devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
      unsigned long int ibrd=0x00000000;
      ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24)|ibrd;
      return ibrd;
}


unsigned long int DDU::ddufpga_usercode()
{
int i;
enum DEVTYPE devstp,dv;
  dv=DDUFPGA;
      cmd[0]=VTX2P_USERCODE_L;
      cmd[1]=VTX2P_USERCODE_H;
      sndbuf[0]=0xFF;
      sndbuf[1]=0xFF;
      sndbuf[2]=0xFF;
      sndbuf[3]=0xFF;
      sndbuf[4]=0xFF;
      devdo(dv,10,cmd,32,sndbuf,rcvbuf,1);
      // printf(" The DDU FPGA Chip USERCODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
      cmd[0]=VTX2P_BYPASS_L;
      cmd[1]=VTX2P_BYPASS_H;
      sndbuf[0]=0;
      devdo(dv,10,cmd,0,sndbuf,rcvbuf,0);
      unsigned long int ibrd=0x00000000;
      ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24)|ibrd;
      return ibrd;
}

unsigned long int DDU::infpga_usercode0()
{
int i;
enum DEVTYPE devstp,dv;
//  printf(" INFPGA Virtex PRO ID should be  xxxxx \n");
  dv=INFPGA0;
      cmd[0]=VTX2P20_USERCODE_L;
      cmd[1]=VTX2P20_USERCODE_H;
      sndbuf[0]=0xFF;
      sndbuf[1]=0xFF;
      sndbuf[2]=0xFF;
      sndbuf[3]=0xFF;
      sndbuf[4]=0xFF;
      devdo(dv,14,cmd,32,sndbuf,rcvbuf,1);
      // printf(" The INFPGA-0 Chip USERCODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
      cmd[0]=VTX2P20_BYPASS_L;
      cmd[1]=VTX2P20_BYPASS_H;
      sndbuf[0]=0;
      devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
      unsigned long int ibrd=0x00000000;
      ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24)|ibrd;
      return ibrd;
}

unsigned long int DDU::infpga_usercode1()
{
int i;
enum DEVTYPE devstp,dv;
  dv=INFPGA1;
      cmd[0]=VTX2P20_USERCODE_L;
      cmd[1]=VTX2P20_USERCODE_H;
      sndbuf[0]=0xFF;
      sndbuf[1]=0xFF;
      sndbuf[2]=0xFF;
      sndbuf[3]=0xFF;
      sndbuf[4]=0xFF;
      devdo(dv,14,cmd,32,sndbuf,rcvbuf,1);
     // printf(" The INFPGA-1 Chip USERCODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
      cmd[0]=VTX2P20_BYPASS_L;
      cmd[1]=VTX2P20_BYPASS_H;
      sndbuf[0]=0;
      devdo(dv,14,cmd,0,sndbuf,rcvbuf,0); 
      unsigned long int ibrd=0x00000000;
      ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24)|ibrd;
      return ibrd;
}


void DDU::infpga_shfttst(enum DEVTYPE dv,int tst)
{
  int shft2in;
  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  if(tst==0){
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  }else{
  sndbuf[0]=0xAD;
  sndbuf[1]=0xDE;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  }
  tst=tst+1;if(tst==2)tst=0;
  devdo(dv,14,cmd,40,sndbuf,rcvbuf,1);
  printf(" all %02x %02x %02x %02x %02x \n",0xff&rcvbuf[4],0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
shft2in=(((0x01&rcvbuf[2])<<15)|((0xff&rcvbuf[1])<<7)|(0xfe&rcvbuf[0])>>1);
  printf("   ----> 40-bit FPGA shift test:  sent 0x%02X%02X, got back 0x%04X \n",sndbuf[1]&0xff,sndbuf[0]&0xff,shft2in);
}

void DDU::infpga_reset(enum DEVTYPE dv)
{
  short int pause;
  
  cmd[0]=VTX2P20_USR1_L;
  cmd[1]=VTX2P20_USR1_H;
  sndbuf[0]=DDUFPGA_RST;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  

  cmd[0]=VTX2P20_USR1_L;
  cmd[1]=VTX2P20_USR1_H;
  sndbuf[0]=NORM_MODE;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,2);
  printf(" INFPGA reset done. \n");
}


void DDU::infpga_rdscaler(enum DEVTYPE dv)
{
int i,j,k,n,shft0in,shft1in,shft2in;
long int code;  
  printf(" infpga_rdscaler \n");
  cmd[0]=VTX2P20_USR1_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=2;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR2_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(dv,14,cmd,49,sndbuf,rcvbuf,1);
  printf(" DDU-InFPGA L1 Event Scaler, bits [23-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8)|((0x00ff&rcvbuf[2])<<16))&0x00ffffff;
  printf("    Hex code %06x\n",code);
  printf("    Decimal count =  %8d\n",code);
  shft2in=(((0xff&rcvbuf[4])<<8)|((0xff&rcvbuf[3])));
  printf("   ----> 49-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR1_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,2);
}



void DDU::infpga_lowstat(enum DEVTYPE dv)
{
int i,shft0in,shft1in,shft2in;
long int code;  
  printf(" infpga_lowstat \n");
  cmd[0]=VTX2P20_USR1_L;
  cmd[1]=VTX2P20_USR1_H;
  sndbuf[0]=4;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);

  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX2P20_USR2_L;
  cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  //  printf("   rcvbuf[3:0] = %02x%02x/%02x%02x\n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
  devdo(dv,14,cmd,40,sndbuf,rcvbuf,1);
  printf(" 16-low-bits INFPGA Status:  %02x%02x \n",0xff&rcvbuf[1],0xff&rcvbuf[0]);
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  //shft2in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xff&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }

//  in_status_decode(code);
  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX2P20_USR1_L;
  cmd[1]=VTX2P20_USR1_H;
  sndbuf[0]=NORM_MODE;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::infpga_histat(enum DEVTYPE dv)
{
int i,shft0in,shft1in,shft2in;
long int code;  
  printf(" infpga_histat \n");
  cmd[0]=VTX2P20_USR1_L;
  cmd[1]=VTX2P20_USR1_H;
  sndbuf[0]=5;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);

  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX2P20_USR2_L;
  cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;  
  devdo(dv,14,cmd,40,sndbuf,rcvbuf,1);
  printf(" all %02x %02x %02x %02x %02x \n",0xff&rcvbuf[4],0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
  printf(" 16-high-bits INFPGA Status:  %02x%02x \n",0xff&rcvbuf[1],0xff&rcvbuf[0]);
  code=(((0x00ff&rcvbuf[0])<<16)|((0x00ff&rcvbuf[1])<<24))&0xffff0000;
  //shft2in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xff&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
//  in_status_decode(code);

  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX2P20_USR1_L;
  cmd[1]=VTX2P20_USR1_H;
  sndbuf[0]=NORM_MODE;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,2);
}

void DDU::infpgastat(enum DEVTYPE dv)
     //void DDU::infpga_dfpgastat(enum DEVTYPE dv)
{
int i,shft0in,shft1in,shft2in;
long int errcode;
  printf(" infpgastat \n");

  cmd[0]=VTX2P20_USR1_L;
  cmd[1]=VTX2P20_USR1_H;
  sndbuf[0]=3;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX2P20_USR2_L;
  cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  sndbuf[6]=0xFE;
  sndbuf[7]=0xCA;
  sndbuf[8]=0x01;
  sndbuf[9]=0x00;
  sndbuf[10]=0x00;
  sndbuf[11]=0x00;
  sndbuf[12]=0x00;
  sndbuf[13]=0x00;
  sndbuf[14]=0x00;
  sndbuf[15]=0x00;
  sndbuf[16]=0x00;
  devdo(dv,14,cmd,70,sndbuf,rcvbuf,1);
  printf(" 32-bit INFPGA Status:  %02x%02x/%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
  errcode=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8)|((0x00ff&rcvbuf[2])<<16)|((0x00ff&rcvbuf[3])<<24));
  shft2in=(((0x01&rcvbuf[6])<<15)|((0xff&rcvbuf[5])<<7)|(0xfe&rcvbuf[4])>>1);
  //  shft2in=(((0xff&rcvbuf[5])<<8)|(0xff&rcvbuf[4]));
  shft2in=(((0xff&rcvbuf[5])<<8)|(0xff&rcvbuf[4]));
  printf("   ----> 70-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<11;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
//  in_status_decode(errcode);
  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX2P20_USR1_L;
  cmd[1]=VTX2P20_USR1_H;
  sndbuf[0]=NORM_MODE;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,2);
}

void DDU::infpga_CheckFiber(enum DEVTYPE dv)
{
int i,j,k,n,shft0in,shft1in,shft2in;
long int code;  
  printf(" infpga_CheckFiber (DDU In_Ctrl FPGA) \n");
  cmd[0]=VTX2P20_USR1_L;
  cmd[1]=VTX2P20_USR1_H;
  sndbuf[0]=6;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR2_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(dv,14,cmd,40,sndbuf,rcvbuf,1);
  printf(" Check Fiber Status [15-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  for(j=15;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/8)*8==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  printf("                             FiberErr.FiberOK\n");
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR1_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::infpga_DMBsync(enum DEVTYPE dv)
{
int i,j,k,n,shft0in,shft1in,shft2in;
long int code;
  printf(" infpga_DMBsync (DDU In_Ctrl FPGA) \n");
  cmd[0]=VTX2P20_USR1_L;
  cmd[1]=VTX2P20_USR1_H;
  sndbuf[0]=7;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR2_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(dv,14,cmd,40,sndbuf,rcvbuf,1);
  printf(" DDU DMB Sync Checks [15-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  for(j=15;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/8)*8==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  printf("                             StuckData.L1Amismatch\n");
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR1_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::infpga_FIFOstatus(enum DEVTYPE dv)
{
int i,j,k,n,shft0in,shft1in,shft2in;
long int code;  
  printf(" infpga_FIFOstatus (DDU In_Ctrl FPGA) \n");
  cmd[0]=VTX2P20_USR1_L;
  cmd[1]=VTX2P20_USR1_H;
  sndbuf[0]=8;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR2_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(dv,14,cmd,40,sndbuf,rcvbuf,1);
  printf(" DDU Input FIFO Status [15-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  for(j=15;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if(j==10)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  printf("                        FIFOalmostFull.FIFOempty\n");
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR1_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::infpga_FIFOfull(enum DEVTYPE dv)
{
int i,j,k,n,shft0in,shft1in,shft2in;
long int code;  
  printf(" infpga_FIFOfull (DDU In_Ctrl FPGA) \n");
  cmd[0]=VTX2P20_USR1_L;
  cmd[1]=VTX2P20_USR1_H;
  sndbuf[0]=9;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR2_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(dv,14,cmd,40,sndbuf,rcvbuf,1);
  printf(" DDU FIFO Full [11-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x000f&rcvbuf[1])<<8))&0x00000fff;
  for(j=11;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/8)*8==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  shft2in=(((0x0f&rcvbuf[3])<<12)|((0xff&rcvbuf[2])<<4)|((0xf0&rcvbuf[1])>>4));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR1_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,2);
}



void DDU::infpga_RxErr(enum DEVTYPE dv)
{
int i,j,k,n,shft0in,shft1in,shft2in;
long int code;  
  printf(" infpga_RxErr (DDU In_Ctrl FPGA) \n");
  cmd[0]=VTX2P20_USR1_L;
  cmd[1]=VTX2P20_USR1_H;
  sndbuf[0]=10;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR2_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(dv,14,cmd,40,sndbuf,rcvbuf,1);
  printf(" DDU Rx Error [15-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  for(j=15;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/8)*8==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  printf("                   FiberRxError.StartTimeout\n");
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR1_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,2);
}



void DDU::infpga_Timeout(enum DEVTYPE dv)
{
int i,j,k,n,shft0in,shft1in,shft2in;
long int code;  
  printf(" infpga_Timeout (DDU In_Ctrl FPGA) \n");
  cmd[0]=VTX2P20_USR1_L;
  cmd[1]=VTX2P20_USR1_H;
  sndbuf[0]=11;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR2_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(dv,14,cmd,40,sndbuf,rcvbuf,1);
  printf(" DDU Timeout [15-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  for(j=15;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/8)*8==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  printf("                EndBusyTimeout.EndWaitTimeout\n");
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR1_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,2);
}



void DDU::infpga_XmitErr(enum DEVTYPE dv)
{
int i,j,k,n,shft0in,shft1in,shft2in;
long int code;  
  printf(" infpga_XmitErr (DDU In_Ctrl FPGA) \n");
  cmd[0]=VTX2P20_USR1_L;
  cmd[1]=VTX2P20_USR1_H;
  sndbuf[0]=12;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR2_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(dv,14,cmd,40,sndbuf,rcvbuf,1);
  printf(" Xmit Error [15-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  for(j=15;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/8)*8==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  printf("                     DMBerror.XmitError\n");
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR1_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::infpga_WrMemActive(enum DEVTYPE dv,int ifiber)
{
int i,j,k,n,shft0in,shft1in,shft2in;
long int code;  
  k=dv-8; //InFPGA #0 or 1
  if((ifiber>3)||(ifiber<0))ifiber=0;
  printf(" infpga_WrMemActive for Fibers %d & %d (DDU In_Ctrl FPGA-%d) \n",((k*8)+(2*ifiber))+1,(k*8)+(2*ifiber),k);
  cmd[0]=VTX2P20_USR1_L;
  cmd[1]=VTX2P20_USR1_H;
  sndbuf[0]=13+ifiber;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR2_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(dv,14,cmd,40,sndbuf,rcvbuf,1);
  code=(((0x00e0&rcvbuf[0])>>5)|((0x0003&rcvbuf[1])<<3))&0x0000001f;
  printf(" InFpga Fiber %d WrMemActive: %02Xh\n",(8*k)+(2*ifiber),(rcvbuf[0]&0x001f) );
  printf("        Fiber %d WrMemActive: %02Xh\n",(8*k)+(2*ifiber)+1,code);
  shft2in=(((0x03&rcvbuf[3])<<14)|((0xff&rcvbuf[2])<<6)|((0xfc&rcvbuf[1])>>2));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<3;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("\n");
  }
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR1_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,2);
}

void DDU::infpga_MemAvail(enum DEVTYPE dv)
{
int i,j,k,n,shft0in,shft1in,shft2in;
long int code;  
  printf(" infpga_MemAvail (DDU In_Ctrl FPGA) \n");
  cmd[0]=VTX2P20_USR1_L;
  cmd[1]=VTX2P20_USR1_H;
  sndbuf[0]=17;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR2_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(dv,14,cmd,40,sndbuf,rcvbuf,1);
  printf(" InFpga FIFO Memory Available [1 & 0]:  ");
  code=(((0x00e0&rcvbuf[0])>>5)|((0x0003&rcvbuf[1])<<3))&0x0000001f;
  printf("MemCtrl-0 = %d free\n",(rcvbuf[0]&0x001f) );
  printf("                                        MemCtrl-1 = %d free\n",code);
  shft2in=(((0x03&rcvbuf[3])<<14)|((0xff&rcvbuf[2])<<6)|((0xfc&rcvbuf[1])>>2));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<3;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("\n");
  }
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR1_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,2);
}



void DDU::infpga_Min_Mem(enum DEVTYPE dv)
{
int i,j,k,n,shft0in,shft1in,shft2in;
long int code;  
  printf(" infpga_Mem_Min (DDU In_Ctrl FPGA) \n");
  cmd[0]=VTX2P20_USR1_L;
  cmd[1]=VTX2P20_USR1_H;
  sndbuf[0]=18;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR2_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(dv,14,cmd,40,sndbuf,rcvbuf,1);
  printf(" InFpga Minimum FIFO Memory Availabile Record [1 & 0]:\n");
  code=(((0x00e0&rcvbuf[0])>>5)|((0x0003&rcvbuf[1])<<3))&0x0000001f;
  printf("     MemCtrl-0 min = %d free\n",(rcvbuf[0]&0x001f) );
  printf("     MemCtrl-1 min = %d free\n",code);
  shft2in=(((0x03&rcvbuf[3])<<14)|((0xff&rcvbuf[2])<<6)|((0xfc&rcvbuf[1])>>2));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<3;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("\n");
  }
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR1_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,2);
}



void DDU::infpga_LostErr(enum DEVTYPE dv)
{
int i,j,k,n,shft0in,shft1in,shft2in;
long int code;  
  printf(" infpga_LostErr (DDU In_Ctrl FPGA) \n");
  cmd[0]=VTX2P20_USR1_L;
  cmd[1]=VTX2P20_USR1_H;
  sndbuf[0]=19;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR2_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(dv,14,cmd,40,sndbuf,rcvbuf,1);
  printf(" DDU Lost Error [15-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  for(j=15;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/8)*8==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  printf("                      LostInEvent.LostInData\n");
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR1_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,2);
}



void DDU::infpga_StatA(enum DEVTYPE dv)
{
int i,j,k,n,shft0in,shft1in,shft2in;
long int code;  
  printf(" infpga_StatA (DDU In_Ctrl FPGA) \n");
  cmd[0]=VTX2P20_USR1_L;
  cmd[1]=VTX2P20_USR1_H;
  sndbuf[0]=22;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR2_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(dv,14,cmd,40,sndbuf,rcvbuf,1);
  printf(" DDU Status Reg A [15-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  for(j=15;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/8)*8==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR1_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,2);
}



void DDU::infpga_StatB(enum DEVTYPE dv)
{
int i,j,k,n,shft0in,shft1in,shft2in;
long int code;  
  printf(" infpga_StatB (DDU In_Ctrl FPGA) \n");
  cmd[0]=VTX2P20_USR1_L;
  cmd[1]=VTX2P20_USR1_H;
  sndbuf[0]=23;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR2_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(dv,14,cmd,40,sndbuf,rcvbuf,1);
  printf(" DDU Status Reg B [15-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  for(j=15;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/8)*8==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR1_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,2);
}



void DDU::infpga_StatC(enum DEVTYPE dv)
{
int i,j,k,n,shft0in,shft1in,shft2in;
long int code;
  printf(" infpga_StatC (DDU In_Ctrl FPGA-%d) \n",dv-8);
  cmd[0]=VTX2P20_USR1_L;
  cmd[1]=VTX2P20_USR1_H;
  sndbuf[0]=24;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;
  cmd[1]=VTX2P20_BYPASS_H;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR2_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0x04;
  sndbuf[3]=0x04;
  sndbuf[4]=0x00;
  sndbuf[5]=0x00;
  devdo(dv,14,cmd,40,sndbuf,rcvbuf,1);
  printf(" DDU Status Reg C [15-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  for(j=15;j>=0;j--){
    printf("%1d",(code>>j)&0x00000001);
    if((j/8)*8==j&&j>0)printf(".");
  }
  printf(",  Hex code %04x\n",code);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      no right-shift needed\n");
  }
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_USR1_L;cmd[1]=VTX2P20_USR2_H;
  sndbuf[0]=NORM_MODE;
  devdo(dv,14,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2P20_BYPASS_L;cmd[1]=VTX2P20_BYPASS_H;
  sndbuf[0]=0;
  devdo(dv,14,cmd,0,sndbuf,rcvbuf,2);
}



unsigned long int DDU::inprom_idcode1()
{
int i,loopi;
enum DEVTYPE dv;
// printf(" inprom_idcode entered \n");
      dv=INPROM1;
      // printf("inprom:dv %d \n",dv);
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
      // printf(" The INPROM-1 Chip ID CODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
      cmd[0]=PROM_BYPASS;
      sndbuf[0]=0;
      devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
      unsigned long int ibrd=0x00000000;
      ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24)|ibrd;
      return ibrd;
}

unsigned long int DDU::inprom_idcode0()
{
int i,loopi;
enum DEVTYPE dv;
// printf(" inprom_idcode entered \n");

      cmd[0]=PROM_BYPASS;
      sndbuf[0]=0;
      devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
      dv=INPROM0;
      cmd[0]=PROM_IDCODE;
      sndbuf[0]=0xFF;
      sndbuf[1]=0xFF;
      sndbuf[2]=0xFF;
      sndbuf[3]=0xFF;
      sndbuf[4]=0xFF;
      devdo(dv,8,cmd,32,sndbuf,rcvbuf,1);
      // printf(" The INPROM-0 Chip ID CODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
      cmd[0]=PROM_BYPASS;
      sndbuf[0]=0;
      devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
      unsigned long int ibrd=0x00000000;
      ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24)|ibrd;
      return ibrd;
}

unsigned long int DDU::vmeprom_idcode()
{
int i,loopi;
enum DEVTYPE dv;
// printf(" vmeprom_idcode entered \n");
      dv=VMEPROM;
      cmd[0]=PROM_IDCODE;
      sndbuf[0]=0xFF;
      sndbuf[1]=0xFF;
      sndbuf[2]=0xFF;
      sndbuf[3]=0xFF;
      sndbuf[4]=0xFF;
      devdo(dv,8,cmd,32,sndbuf,rcvbuf,1);
      // printf(" The VMEPROM Chip ID CODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
      cmd[0]=PROM_BYPASS;
      sndbuf[0]=0;
      devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
      unsigned long int ibrd=0x00000000;
      ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24)|ibrd;
      return ibrd;

}

unsigned long int DDU::dduprom_idcode1()
{
int i,loopi;
enum DEVTYPE dv;
// printf(" dduprom_idcode entered \n");
      dv=DDUPROM0;
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
      //      printf(" The DDUPROM-1 Chip ID CODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
      cmd[0]=PROM_BYPASS;
      sndbuf[0]=0;
      devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
      unsigned long int ibrd=0x00000000;
      ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24)|ibrd;
      return ibrd;
}

unsigned long int DDU::dduprom_idcode0()
{     
int i,loopi;
enum DEVTYPE dv;
// printf(" dduprom_idcode entered \n");
      dv=DDUPROM1;
      cmd[0]=PROM_IDCODE;
      sndbuf[0]=0xFF;
      sndbuf[1]=0xFF;
      sndbuf[2]=0xFF;
      sndbuf[3]=0xFF;
      sndbuf[4]=0xFF;
      devdo(dv,8,cmd,32,sndbuf,rcvbuf,1);
      // printf(" The DDUPROM-0 Chip ID CODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
      cmd[0]=PROM_BYPASS;
      sndbuf[0]=0;
      devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
      unsigned long int ibrd=0x00000000;
      ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24)|ibrd;
      return ibrd;
}

unsigned long int  DDU::inprom_usercode0()
{
int i,loopi;
enum DEVTYPE dv;
// printf(" inprom_usercode entered \n");
      dv=INPROM0;
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
      //   printf(" The INPROM-1 Chip USER CODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
      cmd[0]=PROM_BYPASS;
      sndbuf[0]=0;
      devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
      unsigned long int ibrd=0x00000000;
      ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24)|ibrd;
      return ibrd;
}

unsigned long int DDU::inprom_usercode1()
{
int i;
enum DEVTYPE devstp,dv;  
      dv=INPROM1;
      cmd[0]=PROM_USERCODE;
      sndbuf[0]=0xFF;
      sndbuf[1]=0xFF;
      sndbuf[2]=0xFF;
      sndbuf[3]=0xFF;
      sndbuf[4]=0xFF;
      devdo(dv,8,cmd,32,sndbuf,rcvbuf,1);
      // printf(" The INPROM-0 Chip USER CODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
      cmd[0]=PROM_BYPASS;
      sndbuf[0]=0;
      devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
      unsigned long int ibrd=0x00000000;
      ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24)|ibrd;
      return ibrd;
}

unsigned long int  DDU::vmeprom_usercode()
{
int i,loopi;
enum DEVTYPE dv;
// printf(" vmeprom_usercode entered \n");
      dv=VMEPROM;
      cmd[0]=PROM_USERCODE;
      sndbuf[0]=0xFF;
      sndbuf[1]=0xFF;
      sndbuf[2]=0xFF;
      sndbuf[3]=0xFF;
      sndbuf[4]=0xFF;
      devdo(dv,8,cmd,32,sndbuf,rcvbuf,1);
      // printf(" The VMEPROM Chip USER CODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
      cmd[0]=PROM_BYPASS;
      sndbuf[0]=0;
      devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
      unsigned long int ibrd=0x00000000;
      ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24)|ibrd;
      return ibrd;
}

unsigned long int  DDU::dduprom_usercode1()
{
int i,loopi;
enum DEVTYPE dv;
// printf(" dduprom_usercode entered \n");
      dv=DDUPROM1;
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
      // printf(" The DDUPROM-1 Chip USER CODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
      cmd[0]=PROM_BYPASS;
      sndbuf[0]=0;
      devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
      unsigned long int ibrd=0x00000000;
      ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24)|ibrd;
      return ibrd;
}

unsigned long int  DDU::dduprom_usercode0()
{
int i,loopi;
enum DEVTYPE dv;
      dv=DDUPROM0;
      cmd[0]=PROM_USERCODE;
      sndbuf[0]=0xFF;
      sndbuf[1]=0xFF;
      sndbuf[2]=0xFF;
      sndbuf[3]=0xFF;
      sndbuf[4]=0xFF;
      devdo(dv,8,cmd,32,sndbuf,rcvbuf,1);
      // printf(" The DDUPROM-0 Chip USER CODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
      cmd[0]=PROM_BYPASS;
      sndbuf[0]=0;
      devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
      unsigned long int ibrd=0x00000000;
      ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24)|ibrd;
      return ibrd;}

void DDU::vmepara_busy()
{ 
  cmd[0]=0x00;
  cmd[1]=0x00;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  rcvbuf[0]=0;
  rcvbuf[1]=0;
  rcvbuf[2]=0;
  rcvbuf[3]=0;
  devdo(VMEPARA,1,cmd,0,sndbuf,rcvbuf,2);
  printf("Read FMM bit0: %02x%02x\n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
}

void DDU::vmepara_fullwarn()
{ 
  cmd[0]=0x01;
  cmd[1]=0x00;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  rcvbuf[0]=0;
  rcvbuf[1]=0;
  rcvbuf[2]=0;
  rcvbuf[3]=0;
  devdo(VMEPARA,1,cmd,0,sndbuf,rcvbuf,2);
  printf("Read FMM bit1: %02x%02x\n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
}

void DDU::vmepara_lostsync()
{ 
  cmd[0]=0x02;
  cmd[1]=0x00;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  rcvbuf[0]=0;
  rcvbuf[1]=0;
  rcvbuf[2]=0;
  rcvbuf[3]=0;
  devdo(VMEPARA,1,cmd,0,sndbuf,rcvbuf,2);
  printf("Read FMM bit2: %02x%02x\n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
}

void DDU::vmepara_error()
{ 
  cmd[0]=0x03;
  cmd[1]=0x00;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  rcvbuf[0]=0;
  rcvbuf[1]=0;
  rcvbuf[2]=0;
  rcvbuf[3]=0;
  devdo(VMEPARA,1,cmd,0,sndbuf,rcvbuf,2);
  printf("Read FMM bit3: %02x%02x\n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
}

void DDU::vmepara_switch()
{ 
  cmd[0]=0x0e;
  cmd[1]=0x00;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  rcvbuf[0]=0;
  rcvbuf[1]=0;
  rcvbuf[2]=0;
  rcvbuf[3]=0;
  devdo(VMEPARA,1,cmd,0,sndbuf,rcvbuf,2);
  printf("Read DDU switches: %02x\n",rcvbuf[0]&0xff);
}

void DDU::vmepara_status()
{ 
  cmd[0]=0x0f;
  cmd[1]=0x00;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  rcvbuf[0]=0;
  rcvbuf[1]=0;
  rcvbuf[2]=0;
  rcvbuf[3]=0;
  devdo(VMEPARA,1,cmd,0,sndbuf,rcvbuf,2);
  printf("Read DDU status1: %02x%02x\n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
}

void DDU::vmepara_status2()
{ 
  cmd[0]=0x0f;
  cmd[1]=0x00;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  rcvbuf[0]=0;
  rcvbuf[1]=0;
  rcvbuf[2]=0;
  rcvbuf[3]=0;
  devdo(VMEPARA,1,cmd,0,sndbuf,rcvbuf,2);
  // printf("Read DDU status2 %02x%02x\n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
}

void DDU::vmepara_rd_inreg0()
{ 
  cmd[0]=0x08; //dev 0x08 is serial-input register
  cmd[1]=0x00; //cmd 0x00 is the first 16-bit reg (MSB word)
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  rcvbuf[0]=0;
  rcvbuf[1]=0;
  rcvbuf[2]=0;
  rcvbuf[3]=0;
  devdo(VMEPARA,1,cmd,0,sndbuf,rcvbuf,2);
  printf("Read DDU VMEser InReg0: %02x%02x\n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
}

void DDU::vmepara_rd_inreg1()
{ 
  cmd[0]=0x08; //dev 0x08 is serial-input register
  cmd[1]=0x01; //cmd 0x01 is the 2nd 16-bit reg
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  rcvbuf[0]=0;
  rcvbuf[1]=0;
  rcvbuf[2]=0;
  rcvbuf[3]=0;
  devdo(VMEPARA,1,cmd,0,sndbuf,rcvbuf,2);
  printf("Read DDU VMEser InReg1: %02x%02x\n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
}

void DDU::vmepara_rd_inreg2()
{ 
  cmd[0]=0x08; //dev 0x08 is serial-input register
  cmd[1]=0x02; //cmd 0x02 is the third 16-bit reg
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  rcvbuf[0]=0;
  rcvbuf[1]=0;
  rcvbuf[2]=0;
  rcvbuf[3]=0;
  devdo(VMEPARA,1,cmd,0,sndbuf,rcvbuf,2);
  printf("Read DDU VMEser InReg2: %02x%02x\n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
}

void DDU::vmepara_wr_inreg(unsigned int par_val)
{
  cmd[0]=0x08; //dev 0x08 is serial-input register
  cmd[1]=0x80; //cmd 0x00 is first 16-bit reg (MSB word); set MSB for Write
  sndbuf[7]=0xBE;
  sndbuf[6]=0xAD;
  sndbuf[5]=0x13;
  sndbuf[4]=0x79;
  sndbuf[3]=0x24;
  sndbuf[2]=0x68;
  sndbuf[1]=0xDE;
  sndbuf[0]=0xAF;
  rcvbuf[0]=0;
  rcvbuf[1]=0;
  rcvbuf[2]=0;
  rcvbuf[3]=0;
  sndbuf[1]=(par_val&0xff00)>>8;
  sndbuf[0]=(par_val&0x00ff);
  //
  devdo(VMEPARA,1,  cmd,  0,  sndbuf, rcvbuf,  2);
  //    dev,  ncmd, cmd, nbuf, inbuf, outbuf, irdsnd
  /* irdsnd for jtag
          irdsnd = 0 send immediately, no read
          irdsnd = 1 send immediately, read
          irdsnd = 2 send in buffer, no read
  */

  printf("Wrote to DDU VMEser InReg0: %02x%02x \n",sndbuf[1]&0xff,sndbuf[0]&0xff);
}

void DDU::read_status()
{
int i;
  cmd[0]=0x04; //dev 0x04 is flash sram
  cmd[1]=0x00; // cmd 0x00 is read flash status, 8 bits
  sndbuf[0]=0xf1; // low data byte
  sndbuf[1]=0xf2; // high data byte 
  devdo(VMESERI,2,cmd,0,sndbuf,rcvbuf,0);
  printf("Read from Flash Status Register: ");
  printf(" %02x\n",rcvbuf[1]&0xff);
}

void DDU::read_page1()
{
int i;
  cmd[0]=0x04; //dev 0x04 is flash sram
  cmd[1]=0x01; //read 16 bits
  sndbuf[0]=0xff; // low data byte
  sndbuf[1]=0xaa; // high data byte 
  devdo(VMESERI,2,cmd,0,sndbuf,rcvbuf,0);
  printf("Read from Flash Memory Page1 (Kill Channel Mask): ");
  for(i=0;i<2;i++)printf("%02x",rcvbuf[i]&0xff);
  printf("\n");
}

void DDU::write_page1()
{
int i;
  cmd[0]=0x04; //dev 0x04 is flash sram
  cmd[1]=0x09; // cmd 0x09 is program page 1, 16 bits
/*
  sndbuf[0]=0xf4; // low data byte
  sndbuf[1]=0xf9; // high data byte 
*/
  sndbuf[0]=0xff; // low data byte, Kill only channel 16 (D.N.E.)
  sndbuf[1]=0x7f; // high data byte 
  printf("Programming Flash Memory Page1 (Kill Cahnnel Mask):  0x");
  for(i=0;i<2;i++)printf("%02x",sndbuf[1-i]&0xff);
  printf("\n");
  devdo(VMESERI,2,cmd,0,sndbuf,rcvbuf,1);
}

void DDU::read_page4()
{
int i;
unsigned int code[3];
  cmd[0]=0x04; //dev 0x04 is flash sram
  cmd[1]=0x04; //read 32 bits
  sndbuf[0]=0xee; // low data byte
  sndbuf[1]=0x66;
  sndbuf[2]=0xff;
  sndbuf[3]=0x88; // high data byte 
  devdo(VMESERI,2,cmd,0,sndbuf,rcvbuf,0);
  printf("Read from Flash Memory Page4 (DDR FIFO Thresholds, 32 bits): ");
  code[0]=((rcvbuf[1]&0xFF))|((rcvbuf[0]&0xFF)<<8);
  code[1]=((rcvbuf[3]&0xFF))|((rcvbuf[2]&0xFF)<<8);
  printf("%04x/%04x\n",code[1],code[0]);
  //  printf("Read from Flash Memory Page4 (DDR Input FIFO Thresholds): ");
  //  for(i=0;i<4;i++)printf("%02x",rcvbuf[i]&0xff);
  //  printf("\n");
}

void DDU::write_page4()
{
int i;
  cmd[0]=0x04; //dev 0x04 is flash sram
  cmd[1]=0x0c; // cmd 0x0c is program page 4, 32 bits
/*
  sndbuf[0]=0xfe; // low data byte
  sndbuf[1]=0xca;
  sndbuf[2]=0xed;
  sndbuf[3]=0xfe; // high data byte 
*/
  sndbuf[0]=0xff; // low data byte
  sndbuf[1]=0x00; //  PAE: m=255
  sndbuf[2]=0x10; //  PAF: n=16
  sndbuf[3]=0x00; // high data byte 
  printf("Programming Flash Memory Page4 (DDR Input FIFO Thresholds):  0x");
  for(i=0;i<4;i++)printf("%02x",sndbuf[3-i]&0xff);
  printf("\n");
  devdo(VMESERI,2,cmd,0,sndbuf,rcvbuf,1);
}


void DDU::read_page5()
{
int i;
unsigned int code[3];
  cmd[0]=0x04; //dev 0x04 is flash sram
  cmd[1]=0x05; //read 34 bits
  sndbuf[0]=0xee; // low data byte
  sndbuf[1]=0x66;
  sndbuf[2]=0xff;
  sndbuf[3]=0x88;
  sndbuf[4]=0xaa; // high data byte 
  sndbuf[5]=0x68;
  devdo(VMESERI,2,cmd,0,sndbuf,rcvbuf,0);
  printf("Read from Flash Memory Page5 (GBE FIFO Thresholds, 34 bits): ");
/* old 34-bit shift:
*/
  code[0]=(((rcvbuf[0]&0xC0)>>6)|((rcvbuf[3]&0xFF)<<2)|((rcvbuf[2]&0x3F)<<10));
  code[1]=(((rcvbuf[2]&0xC0)>>6)|((rcvbuf[5]&0xFF)<<2)|((rcvbuf[4]&0x3F)<<10));
  code[2]=((rcvbuf[4]&0xC0)>>6);
/* 48-bit shift test:
  code[0]=((rcvbuf[1]&0xFF)|((rcvbuf[0]&0xFF)<<8));
  code[1]=((rcvbuf[3]&0xFF)|((rcvbuf[2]&0xFF)<<8));
  code[2]=((rcvbuf[5]&0x03));
*/
  printf("%01x/%04x/%04x\n",code[2],code[1],code[0]);
  //  printf("%01x",rcvbuf[1]&0x03);
  //  for(i=2;i<6;i++)printf("%02x",rcvbuf[i]&0xff);
  //  printf("\n");
}

void DDU::write_page5()
{
int i;
  cmd[0]=0x04; //dev 0x04 is flash sram
  cmd[1]=0x0d; // cmd 0x0d is program page 5, 34 bits

  sndbuf[0]=0xff; // low data byte
  sndbuf[1]=0x00; //  PAE: m=255, 17 bits
  sndbuf[2]=0x00; //  PAF: n=32768 (01000h), 17 bits
  sndbuf[3]=0x20;
  sndbuf[4]=0x00; // high data byte 
  sndbuf[5]=0x68;
  printf("Programming Flash Memory Page5 (GBE FIFO Thresholds):  0x");
  for(i=0;i<5;i++)printf("%02x",sndbuf[4-i]&0xff);
  printf("\n");
  devdo(VMESERI,2,cmd,0,sndbuf,rcvbuf,1);
}


void DDU::read_page7()
{
int i;
  cmd[0]=0x04; //dev 0x04 is flash sram
  cmd[1]=0x07; //read 16 bits
  sndbuf[0]=0xee; // low data byte
  sndbuf[1]=0xaa; // high data byte 
  devdo(VMESERI,2,cmd,0,sndbuf,rcvbuf,0);
  printf("Read from Flash Memory Page7 (DDU Board ID): ");
  for(i=0;i<2;i++)printf("%02x",rcvbuf[i]&0xff);
  printf("\n");
}

void DDU::write_page7()
{
int i;
  cmd[0]=0x04; //dev 0x04 is flash sram
  cmd[1]=0x0f; // cmd 0x0f is program page 7, 16 bits

  sndbuf[0]=0xde; // low data byte
  sndbuf[1]=0xfa; // high data byte 
  printf("Programming Flash Memory Page7 (DDU Board ID):  0x");
  for(i=0;i<2;i++)printf("%02x",sndbuf[1-i]&0xff);
  printf("\n");
  devdo(VMESERI,2,cmd,0,sndbuf,rcvbuf,1);
}


void DDU::read_vmesd0()
{
int i;
unsigned int code[3];
  cmd[0]=0x00; //dev 0x00 is "Read VME Serial Device 0" 32 bits
  cmd[1]=0x00; //ignore this byte
  sndbuf[0]=0xee; // low data byte
  sndbuf[1]=0xaa;
  sndbuf[2]=0xff;
  sndbuf[3]=0xcc; // high data byte 
  devdo(VMESERI,2,cmd,0,sndbuf,rcvbuf,0);
  printf("Read Thresholds from DDR Input FIFO 0: ");
  code[0]=((rcvbuf[1]&0xFF))|((rcvbuf[0]&0xFF)<<8);
  code[1]=((rcvbuf[3]&0xFF))|((rcvbuf[2]&0xFF)<<8);
  printf("%04x/%04x\n",code[1],code[0]);
  //  for(i=0;i<4;i++)printf("%02x",rcvbuf[3-i]&0xff);
  //  printf("\n");
}


void DDU::read_vmesd1()
{
int i;
unsigned int code[3];
  cmd[0]=0x01; //dev 0x01 is "Read VME Serial Device 1" 32 bits
  cmd[1]=0x00; //ignore this byte
  sndbuf[0]=0xee; // low data byte
  sndbuf[1]=0xaa;
  sndbuf[2]=0xff;
  sndbuf[3]=0xcc; // high data byte 
  devdo(VMESERI,2,cmd,0,sndbuf,rcvbuf,0);
  printf("Read Thresholds from DDR Input FIFO 1: ");
  code[0]=((rcvbuf[1]&0xFF))|((rcvbuf[0]&0xFF)<<8);
  code[1]=((rcvbuf[3]&0xFF))|((rcvbuf[2]&0xFF)<<8);
  printf("%04x/%04x\n",code[1],code[0]);
  //  for(i=0;i<4;i++)printf("%02x",rcvbuf[3-i]&0xff);
  //  printf("\n");
}


void DDU::read_vmesd2()
{
int i;
unsigned int code[3];
  cmd[0]=0x02; //dev 0x02 is "Read VME Serial Device 2" 32 bits
  cmd[1]=0x00; //ignore this byte
  sndbuf[0]=0xee; // low data byte
  sndbuf[1]=0xaa;
  sndbuf[2]=0xff;
  sndbuf[3]=0xcc; // high data byte 
  devdo(VMESERI,2,cmd,0,sndbuf,rcvbuf,0);
  printf("Read Thresholds from DDR Input FIFO 2: ");
  code[0]=((rcvbuf[1]&0xFF))|((rcvbuf[0]&0xFF)<<8);
  code[1]=((rcvbuf[3]&0xFF))|((rcvbuf[2]&0xFF)<<8);
  printf("%04x/%04x\n",code[1],code[0]);
  //  for(i=0;i<4;i++)printf("%02x",rcvbuf[3-i]&0xff);
  //  printf("\n");
}


void DDU::read_vmesd3()
{
int i;
unsigned int code[3];
  cmd[0]=0x03; //dev 0x03 is "Read VME Serial Device 3" 32 bits
  cmd[1]=0x00; //ignore this byte
  sndbuf[0]=0xee; // low data byte
  sndbuf[1]=0xaa;
  sndbuf[2]=0xff;
  sndbuf[3]=0xcc; // high data byte 
  devdo(VMESERI,2,cmd,0,sndbuf,rcvbuf,0);
  printf("Read Thresholds from DDR Input FIFO 3: ");
  code[0]=((rcvbuf[1]&0xFF))|((rcvbuf[0]&0xFF)<<8);
  code[1]=((rcvbuf[3]&0xFF))|((rcvbuf[2]&0xFF)<<8);
  printf("%04x/%04x\n",code[1],code[0]);
  //  for(i=0;i<4;i++)printf("%02x",rcvbuf[3-i]&0xff);
  //  printf("\n");
}


void DDU::write_vmesdF()
{
int i;
  cmd[0]=0x0f; //dev 0x0F is all DDR FIFOs
  cmd[1]=0x00; // cmd is ignored for dev>=8

  sndbuf[0]=0xad; // low data byte, ignored
  sndbuf[1]=0xbe; // high data byte , ignored
  printf("Write to dev F:  force load of all DDR FIFOs from Flash Page 4\n");
  devdo(VMESERI,2,cmd,0,sndbuf,rcvbuf,1);
}



/* DAQMB   Voltages  */

float DDU::adcplus(int ichp,int ichn){
  unsigned int ival= readADC(ichp, ichn);
  return (float) ival;
}


float DDU::adcminus(int ichp,int ichn){
  unsigned int ival= readADC(ichp, ichn);
  if((0x0800&ival)==0x0800)ival=ival|0xf000;
  return (float) ival;
}

/* Thermometers */

float DDU::readthermx(int it)
{
  float cval,fval;
  float Vout= (float) readADC(1, it) / 1000.;
    cval = 1/(0.1049406423E-2+0.2133635468E-3*log(65000.0/Vout-13000.0)+0.7522287E-7*pow(log(65000.0/Vout-13000.0),3.0))-0.27315E3;
    fval=9.0/5.0*cval+32.;
    return fval;  
}

unsigned int DDU::readADC(int ireg, int ichn) {
  cmd[0]=ireg; /* register 1-4 */
  cmd[1]=ichn; /* channel 0-7 */
  devdo(SADC,16,cmd,0,sndbuf,rcvbuf,2);
  return unpack_ival();
}


void DDU::read_therm()
{
         printf("\nReading all DDU Temperatures\n");
         printf("Reading Temp 0: %5.2f F \n",readthermx(0));
         printf("Reading Temp 1: %5.2f F \n",readthermx(1));
         printf("Reading Temp 2: %5.2f F \n",readthermx(2));
         printf("Reading Temp 3: %5.2f F \n",readthermx(3));
}

void DDU::read_voltages()
{
         printf("\nReading all DDU Voltages\n");
         printf("Reading V15P: %5.2f V \n",adcplus(1,4));
         printf("Reading V25P: %5.2f V \n",adcplus(1,5));
         printf("Reading V25P Analog:%5.2f V \n",adcplus(1,6));
         printf("Reading V33P: %5.2f V \n",adcplus(1,7));
}

unsigned int DDU::unpack_ival(){
  return ((rcvbuf[1]<<8)&0xff00)|(rcvbuf[0]&0xff);
}


void DDU::Parse(char *buf,int *Count,char **Word)
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


void DDU::epromload(char *design,enum DEVTYPE devnum,char *downfile,int writ,char *cbrdnum)
{
enum DEVTYPE devstp,dv;
char *devstr;
FILE *dwnfp,*fpout;
char ch,buf[8192],buf2[256];
char *Word[256],cmdstr[256],*lastn;
double t1,t2;
int Count,i,j,k,l,m,n,id,len,nbits,nbytes,pause,ipd,xtrbits,looppause;
float fpause;
int tmp,cmpflag;
int tstusr;
int nowrit;
char snd[5000],expect[5000],rmask[5000],smask[5000],cmpbuf[5000];
static int count;
extern struct GEOM geo[];
// printf(" epromload %d \n",devnum);
 
 /*  if(devnum==ALL){
    devnum=F1PROM;
    devstp=F5PROM;
  }
  else {
    devstp=devnum;
    } */
 devstp=devnum;
  for(id=devnum;id<=devstp;id++){
    dv=(DEVTYPE)id;
    xtrbits=geo[dv].sxtrbits;
    //    printf(" ************************** xtrbits %d geo[dv].sxtrbits %d \n",xtrbits,geo[dv].sxtrbits);
    devstr=geo[dv].nam;
    dwnfp    = fopen(downfile,"r");
    fpout=fopen("eprom.bit","w");
    //  printf("Programming Design %s (%s) with %s\n",design,devstr,downfile);

    while (fgets(buf,256,dwnfp) != NULL)  {
      // printf("%s",buf);
     if((buf[0]=='/'&&buf[1]=='/')||buf[0]=='!'){
       // printf("%s",buf);
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
        for(i=0;i<1024;i++){
          cmpbuf[i]=0;
          sndbuf[i]=0;
          rcvbuf[i]=0;
        }
        Parse(buf, &Count, &(Word[0]));
        // count=count+1;
        // printf(" count %d \n",count);
        if(strcmp(Word[0],"SDR")==0){
          cmpflag=0;    //disable the comparison for no TDO SDR
          sscanf(Word[1],"%d",&nbits);
          nbytes=(nbits-1)/8+1;
          for(i=2;i<Count;i+=2){
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
          for(i=0;i<nbytes;i++){
	    //            sndbuf[i]=snd[i]&smask[i];
            sndbuf[i]=snd[i]&0xff;
          }
	  //   printf("D%04d",nbits+xtrbits);
          // for(i=0;i<(nbits+xtrbits)/8+1;i++)printf("%02x",sndbuf[i]&0xff);printf("\n");
          if(nowrit==0){
             if((geo[dv].jchan==12)){
                   scan_reset(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
             }else{
                   scan(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
             }
          }else{
	     if(writ==1){

                if((geo[dv].jchan==12)){
                   scan_reset(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
                }else{ 
                   scan(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
                }
             }
          } 
      
	  //  Data readback comparison here:
          for (i=0;i<nbytes;i++) {
            tmp=(rcvbuf[i]>>3)&0x1F;
            rcvbuf[i]=tmp | (rcvbuf[i+1]<<5&0xE0);
	    /*  if (((rcvbuf[i]^expect[i]) & (rmask[i]))!=0 && cmpflag==1) 
		printf("read back wrong, at i %02d  rdbk %02X  expect %02X  rmask %02X\n",i,rcvbuf[i]&0xFF,expect[i]&0xFF,rmask[i]&0xFF); */
          }
          if (cmpflag==1) {
            for (i=0;i<nbytes;i++) {
	        fprintf(fpout," %02X",rcvbuf[i]&0xFF);
              if (i%4==3) fprintf(fpout,"\n");
	      }
	  }
        }

        else if(strcmp(Word[0],"SIR")==0){
          nowrit=0;
          sscanf(Word[1],"%d",&nbits);
          nbytes=(nbits-1)/8+1;
          for(i=2;i<Count;i+=2){
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
          for(i=0;i<nbytes;i++){
	    //            sndbuf[i]=snd[i]&smask[i];
            sndbuf[i]=snd[i];
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
          fpause=pause;
          // pause=pause/2;
          if (pause>65535) {
            sndbuf[0]=255;
            sndbuf[1]=255;
            for (looppause=0;looppause<pause/65536;looppause++) devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,0);
            pause=65535;
	    }
          sndbuf[0]=pause-(pause/256)*256;
          sndbuf[1]=pause/256;
	  // printf(" sndbuf %d %d %d \n",sndbuf[1],sndbuf[0],pause);
          devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,2);
          fpause=fpause*1.5+100;
          pause=fpause; 
          flush_vme();
          usleep(pause);
          // printf(" send sleep \n");  
        }
        else if((strcmp(Word[0],"STATE")==0)&&(strcmp(Word[1],"RESET")==0)&&(strcmp(Word[2],"IDLE;")==0)){
	   printf("goto reset idle state\n"); 
	   devdo(dv,-1,sndbuf,0,sndbuf,rcvbuf,2);
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
  flush_vme();
  send_last();
}



void DDU::all_chip_info()
{
int i;
  char *fpga[3]={" ddufpga-0"," infpga-0 "," infpga-1 "};
  char *prom[5]={" inprom-0 "," inprom-1 "," vmeprom  "," dduprom-0"," dduprom-1"};
  printf("\n idCode: \n");
  for(i=0;i<3;i++)printf("%s",fpga[i]);printf("\n");
  printf(" %08x ",ddufpga_idcode()); printf(" %08x ",infpga_idcode0()); printf(" %08x ",infpga_idcode1());printf("\n");
  for(i=0;i<5;i++)printf("%s",prom[i]);printf("\n");
  printf(" %08x ",inprom_idcode0()); printf(" %08x ",inprom_idcode1()); printf(" %08x ",vmeprom_idcode());  printf(" %08x ",dduprom_idcode0()); printf(" %08x ",dduprom_idcode1());printf("\n");
  usleep(500);
  printf("\n userCode: \n");
  for(i=0;i<3;i++)printf("%s",fpga[i]);printf("\n");
  printf(" %08x ",ddufpga_usercode()); printf(" %08x ",infpga_usercode0()); printf(" %08x ",infpga_usercode1());printf("\n");

  for(i=0;i<5;i++)printf("%s",prom[i]);printf("\n");
  printf(" %08x ",inprom_usercode0()); printf(" %08x ",inprom_usercode1()); printf(" %08x ",vmeprom_usercode());  printf(" %08x ",dduprom_usercode0()); printf(" %08x ",dduprom_usercode1());printf("\n\n");
}

void DDU::vmeprgprom(char *buf)
{
char *cbrdnum;
            cbrdnum=(char*)malloc(5);
            cbrdnum[0]=0x00;cbrdnum[1]=0x00;cbrdnum[2]=0x00;cbrdnum[3]=0x00;
            epromload("VMEPROM",VMEPROM,buf,1,cbrdnum); 
}

void DDU::dduprgprom(char *buf)
{
int i;
char *cbrdnum;
           cbrdnum=(char*)malloc(5);
           cbrdnum[0]=0x00;cbrdnum[1]=0x00;cbrdnum[2]=0x00;cbrdnum[3]=0x00;
           for(i=0;i<128;i++){
	     if(buf[i]=='_'&&(buf[i+1]=='0'||buf[i+1]=='1'))goto END2;
           }
           END2:
           buf[i+1]='1';
           printf(" %s \n",buf);
           epromload("DDUPROM0",DDUPROM0,buf,1,cbrdnum);  
	   buf[i+1]='0';
           printf(" %s \n",buf);  
           epromload("DDUPROM1",DDUPROM1,buf,1,cbrdnum);
           free(cbrdnum);
}

void DDU::inprgprom(char *buf)
{
int i;
char *cbrdnum;
           cbrdnum=(char*)malloc(5);
           cbrdnum[0]=0x00;cbrdnum[1]=0x00;cbrdnum[2]=0x00;cbrdnum[3]=0x00;
           for(i=0;i<128;i++){
	     if(buf[i]=='_'&&(buf[i+1]=='0'||buf[i+1]=='1'))goto END1;
           }
           END1:
           buf[i+1]='1';
           printf(" %s \n",buf);
           epromload("INPROM0",INPROM0,buf,1,cbrdnum);  
	   buf[i+1]='0';
           printf(" %s \n",buf);  
           epromload("INPROM1",INPROM1,buf,1,cbrdnum);
           free(cbrdnum);      
}

void DDU::resetprgprom(char *buf)
{
int i;
char *cbrdnum;
 printf(" inside resetprgprom \n");
            cbrdnum=(char*)malloc(5);
            cbrdnum[0]=0x00;cbrdnum[1]=0x00;cbrdnum[2]=0x00;cbrdnum[3]=0x00;
	    epromload("RESETPROM",RESET,buf,1,cbrdnum); 
}



void DDU::executeCommand(string command)
{
}
