/*****************************************************************************\
* $Id: DDU.cc,v 3.26 2008/08/25 12:25:49 paste Exp $
*
* $Log: DDU.cc,v $
* Revision 3.26  2008/08/25 12:25:49  paste
* Major updates to VMEController/VMEModule handling of CAEN instructions.  Also, added version file for future RPMs.
*
* Revision 3.25  2008/08/19 14:51:02  paste
* Update to make VMEModules more independent of VMEControllers.
*
* Revision 3.24  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "DDU.h"

#include <iostream>
//#include <fstream>
#include <stdio.h>
#include <cmath>
//#include <unistd.h>
//#include <time.h>
#include <iomanip>
#include <math.h>
#include <bitset>
#include <sstream>

#include "Chamber.h"

emu::fed::DDU::DDU(int myCrate,int mySlot):
	VMEModule(mySlot),
	//skip_vme_load_(0),
	gbe_prescale_(0),
	killfiber_(0xf7fff)
{
	std::cerr << "Deprecated constructor.  Use DDU(int) instead of DDU(int,int)." << std::endl;
}

emu::fed::DDU::DDU(int mySlot):
	VMEModule(mySlot),
	//skip_vme_load_(0),
	gbe_prescale_(0),
	killfiber_(0xf7fff)
{
  //  std::cout<<"DDU construct\n";
}


emu::fed::DDU::~DDU() {
  //  std::cout << "DDU destruct" << std::endl;
}

/*
void emu::fed::DDU::end()
{
	//   std::cout << "calling emu::fed::DDU::end" << std::endl;
	//theController->start(this);
	//send_last();
	//VMEModule::end();
}
*/

void emu::fed::DDU::configure() {
	//printf(" ********************DDU configure is called, slot %d\n",slot());
	//  printf(" DDU slot %d gbe_prescale %d  \n",slot(),gbe_prescale_);
	//if(skip_vme_load_==0){
	if (slot()<21) {
		writeGbEPrescale(gbe_prescale_);
		ddu_loadkillfiber(killfiber_);
	}
	//}
	//else{
		//printf("     skipping config download for this DDU. \n");
	//}
}


/*
void emu::fed::DDU::ddu_init()
{
	devdo(DDUFPGA,-1,cmd,0,sndbuf,rcvbuf,2);
}
*/

/*
void emu::fed::DDU::ddu_shfttst(int tst)
{
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
	int shft2in=(((0x01&rcvbuf[2])<<15)|((0xff&rcvbuf[1])<<7)|(0xfe&rcvbuf[0])>>1);
	ddu_code0=shft2in;
	printf("   ----> 40-bit FPGA shift test:  sent 0x%02X%02X, got back 0x%04X \n",sndbuf[1]&0xff,sndbuf[0]&0xff,shft2in);
}
*/

/*
void emu::fed::DDU::ddu_lowfpgastat()
{
int i,shft0in,shft1in,shft2in;
unsigned long int code;
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
  ddu_code0=code;
  ddu_shift0=shft2in;
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  //  ddu5status_decode(code);
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
*/

/*
void emu::fed::DDU::ddu_hifpgastat()
{
int i,shft0in,shft1in,shft2in;
unsigned long int code;
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
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  //shft2in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xff&rcvbuf[2]));
  ddu_code0=code;
  ddu_shift0=shft2in;
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  //  ddu5status_decode(code);
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
*/

/*
unsigned int emu::fed::DDU::ddu_checkFIFOa()
{
int i,j,shft2in;
unsigned long int code;
  //printf(" ddu_checkFIFOa (DDU_Ctrl FPGA) \n");
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
  //printf(" FIFO-A Status [15-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  for(j=15;j>=0;j--){
    //printf("%1ld",(code>>j)&0x00000001);
    //if((j/4)*4==j&&j>0)printf(".");
  }
  //printf(",  Hex code %04lx\n",code);
  //printf("                StuckData.FIFOerr.L1err.FIFOactive\n");
  //printf("     BitTest Vote:  StuckData.FIFOerr.V_Err.FIFOactive\n");
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  ddu_code0=code;
  ddu_shift0=shft2in;
  //printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    //printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    //printf("      no right-shift needed\n");
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

  return code;
}
*/

/*
unsigned int emu::fed::DDU::ddu_checkFIFOb()
{
int i,j,shft2in;
unsigned long int code;
  //printf(" ddu_checkFIFOb (DDU_Ctrl FPGA) \n");
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
  //printf(" FIFO-B Status [15-0]:  "); // old: bit15 was always set high
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  for(j=15;j>=0;j--){
    //printf("%1ld",(code>>j)&0x00000001);
    //if(j==10)printf("/");
    //if(j==14)printf(".");
    //if((j/4)*4==j&&j>0&&j<9)printf(".");
  }
  //printf(",  Hex code %04lx\n",code);
  //printf("                    EmptyFIFOs/AlmostFullFIFOs\n");
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xff&rcvbuf[2]));
  ddu_code0=code;
  ddu_shift0=shft2in;

  //printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    //printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    //printf("      no right-shift needed\n");
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

  return code;
}
*/

/*
unsigned int emu::fed::DDU::ddu_checkFIFOc()
{
int i,j,shft2in;

unsigned long int code;
  //printf(" ddu_checkFIFOc (DDU_Ctrl FPGA) \n");
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
  //printf(" FIFO-C Status [15-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  for(j=15;j>=0;j--){
    //printf("%1ld",(code>>j)&0x00000001);
    //if((j/4)*4==j&&j>0)printf(".");
  }
  //printf(",  Hex code %04lx\n",code);
  //printf("            Timeout-EA.Timeout-EW.Timeout-ST.LostInData\n");
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  ddu_code0=code;
  ddu_shift0=shft2in;
 //printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    //printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    //printf("      no right-shift needed\n");
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

  return code;
}
*/

/*
void emu::fed::DDU::ddu_rdfibererr()
//JRG: Delete
{
int i,j,shft2in;
unsigned long int code;
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
    printf("%1ld",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %04lx\n",code);
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
*/

/*
void emu::fed::DDU::ddu_rdfiberok()
//JRG: Delete
{
int i,j,shft2in;
unsigned long int code;
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
    printf("%1ld",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %04lx\n",code);
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
*/


long unsigned int emu::fed::DDU::ddu_rdkillfiber()
{
	char cmd[2];
	char sndbuf[8];
	char rcvbuf[8];
	//printf("    ddu_rdkillfiber \n");
	cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
	sndbuf[0]=13;
	devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
	cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
	devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
	cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
	sndbuf[0]=0xCE;
	sndbuf[1]=0xFA;
	sndbuf[2]=0x68;
	sndbuf[3]=0x04;
	sndbuf[4]=0x02;
	sndbuf[5]=0x01;
	sndbuf[6]=0xED;
	sndbuf[7]=0x4C;
	devdo(DDUFPGA,10,cmd,47,sndbuf,rcvbuf,1);
	//printf(" KillFiber Register bits [19-0]:\n                       ");
	unsigned long int code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8)|((0x000f&rcvbuf[2])<<16))&0x000fffff;
	//for(j=19;j>=0;j--){
		//printf("%1ld",(code>>j)&0x00000001);
		//if((j/5)*5==j&&j>0)printf(".");
	//}
	//printf(",  Hex code %05lx\n",code);
	
	//printf("       key:  b17==TMB    b16==ALCT    b[14:0]==DDU Inputs\n");
	//if((code&0x00010000)==0)printf("                        ALCT readout decode is disabled\n");
	//if((code&0x00020000)==0)printf("                        TMB readout decode is disabled\n");

	//int shft2in=(((0x0f&rcvbuf[4])<<12)|((0xff&rcvbuf[3])<<4)|(0xf0&rcvbuf[2])>>4);
	//ddu_code1=(0x000f&rcvbuf[2]);
	//ddu_code0=(0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8);
	//ddu_shift0=shft2in;
	//printf("   ----> 47-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
	//for(i=0;i<7;i=i+4){
		//printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
		//printf("      no right-shift needed\n");
	//}
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

	return code;
}



void emu::fed::DDU::ddu_loadkillfiber(long int regval)
{
	//int i,j,shft2in;
	//unsigned long int code;
	//printf("    ddu_loadkillfiber,  Received value=%lX \n",regval);
	char cmd[2];
	char sndbuf[10];
	char rcvbuf[10];
	
	cmd[0]=VTX2P_USR1_L;cmd[1]=VTX2P_USR2_H;
	sndbuf[0]=14;
	devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
	cmd[0]=VTX2P_BYPASS_L;cmd[1]=VTX2P_BYPASS_H;
	devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
	cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
	//Default load "e7FFF" in kill reg:
	//                disable ALCT, enable TMB & all fibers
	sndbuf[0]=0xCE;
	sndbuf[1]=0xFA;
	sndbuf[2]=0x00;
	sndbuf[3]=0x24;
	sndbuf[4]=0xFF;
	sndbuf[5]=0x7F;
	sndbuf[6]=0x0E;
	sndbuf[7]=0x00;
	sndbuf[8]=0x00;
	sndbuf[9]=0x00;
	if(regval>=0&&regval<=0x000FFFFF){
		sndbuf[4]= 0x000000FF&regval;
		sndbuf[5]=(0x0000FF00&regval)>>8;
		sndbuf[6]=(0x000F0000&regval)>>16;
	}
	//printf("          Sending to KillFiber:  %01X%02X%02X \n",0x000F&sndbuf[6],0x00FF&sndbuf[5],0x00FF&sndbuf[4]);
	
	rcvbuf[0]=0x00;
	rcvbuf[1]=0x00;
	rcvbuf[2]=0x00;
	devdo(DDUFPGA,10,cmd,52,sndbuf,rcvbuf,1);
	//printf(" readback of Previous KillFiber setting, bits [19-0]:\n                       ");
	//unsigned long int code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8)|((0x000f&rcvbuf[2])<<16))&0x000fffff;
	//for(j=19;j>=0;j--){
	//printf("%1ld",(code>>j)&0x00000001);
	//if((j/5)*5==j&&j>0)printf(".");
	//}
	//printf(",  Hex code %05lx\n",code);
	
	//printf("       key:  b17==TMB    b16==ALCT    b[14:0]==DDU Inputs\n");
	//  if((code&0x00010000)==0)printf("                        ALCT readout decode was disabled\n");
	//  if((code&0x00020000)==0)printf("                        TMB readout decode was disabled\n");
	
	//  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
	//  shft2in=(((0x1f&rcvbuf[4])<<11)|((0xff&rcvbuf[3])<<3)|(0xe0&rcvbuf[2])>>5);
	//unsigned long int shft2in=(((0x0f&rcvbuf[4])<<12)|((0xff&rcvbuf[3])<<4)|(0xf0&rcvbuf[2])>>4);
	//printf("   ----> 52-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
	//for(i=0;i<7;i=i+4){
	//printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
	//printf("      no right-shift needed\n");
	//}
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


/*
int emu::fed::DDU::ddu_rdcrcerr()
{
int i,j,shft2in;

long int code;
  //printf(" ddu_rdcrstd::cerr \n");
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
  //printf(" CRC Error Register bits [14-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;
  for(j=14;j>=0;j--){
    //printf("%1ld",(code>>j)&0x00000001);
    //if((j/5)*5==j&&j>0)printf(".");
  }
  //printf(",  Hex code %04lx\n",code);
  shft2in=(((0x7f&rcvbuf[3])<<9)|((0xff&rcvbuf[2])<<1)|((0x80&rcvbuf[1])>>7));
  ddu_code0=code;
  ddu_shift0=shft2in;
  //printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    //printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    //printf("      no right-shift needed\n");
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

  return code;
}
*/

/*
void emu::fed::DDU::ddu_rdl1aerr()
//JRG: Delete
{
int i,j,shft2in;

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
    printf("%1ld",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %04lx\n",code);
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
*/

/*
int emu::fed::DDU::ddu_rdxmiterr()
{
int i,j,shft2in;

long int code;
  //printf(" ddu_rdxmiterr \n");
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
  // PGK Note:  It doesn't hurt to shift MORE than the number of bits you need.
  // It is done here because this register was hijacked for the DDU Test Suite
  // firmware.  It's probably best for time reasons to shift only what you need,
  // which would be just the number of bits of the rgeister plus the number of
  // bits you want to shift as a shift test.
  devdo(DDUFPGA,10,cmd,104,sndbuf,rcvbuf,1);
  //  devdo(DDUFPGA,10,cmd,40,sndbuf,rcvbuf,1);
  //printf(" Data Transmit Error Register bits [14-0]:\n                       ");
  code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;
  for(j=14;j>=0;j--){
    //printf("%1ld",(code>>j)&0x00000001);
    //if((j/5)*5==j&&j>0)printf(".");
  }
  //printf(",  Hex code %04lx\n",code);
  //  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  shft2in=(((0x7f&rcvbuf[3])<<9)|((0xff&rcvbuf[2])<<1)|((0x80&rcvbuf[1])>>7));
  ddu_code0=code;
  ddu_shift0=shft2in;
  //printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    //printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    //printf("      no right-shift needed\n");
  }

  //printf("For BitTest Vote Error report [63-0]:");

// code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;
//for(j=14;j>=0;j--){
//  printf("%1d",(code>>j)&0x00000001);
//  if((j/5)*5==j&&j>0)printf(".");
//}

  //printf("  Hex %02x%02x %02x%02x %02x%02x %02x%02x\n",rcvbuf[7]&0x00ff,rcvbuf[6]&0x00ff,rcvbuf[5]&0x00ff,rcvbuf[4]&0x00ff,rcvbuf[3]&0x00ff,rcvbuf[2]&0x00ff,rcvbuf[1]&0x00ff,rcvbuf[0]&0x00ff);
  shft2in=(((0xff&rcvbuf[9])<<8)|((0xff&rcvbuf[8])));
  //printf("   ----> 104-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<11;i=i+4){
    //printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    //printf("      no right-shift needed\n");
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

  return code;
}
*/

/*
void emu::fed::DDU::ddu_rdtimesterr()
//JRG: Delete
{
int i,j,shft2in;

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
    printf("%1ld",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %04lx\n",code);
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
*/

/*
void emu::fed::DDU::ddu_rdtimeewerr()
//JRG: Delete
{
int i,j,shft2in;

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
    printf("%1ld",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %04lx\n",code);
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
*/

/*
void emu::fed::DDU::ddu_rdtimeeaerr()
//JRG: Delete
{
int i,j,shft2in;

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
    printf("%1ld",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %04lx\n",code);
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
*/

/*
int emu::fed::DDU::ddu_rddmberr()
{
int i,j,shft2in;

long int code;
  //printf(" ddu_rddmberr \n");
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
  //printf(" DMB Error Register bits [14-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;
  for(j=14;j>=0;j--){
    //printf("%1ld",(code>>j)&0x00000001);
    //if((j/5)*5==j&&j>0)printf(".");
  }
  //printf(",  Hex code %04lx\n",code);
  //  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  shft2in=(((0x7f&rcvbuf[3])<<9)|((0xff&rcvbuf[2])<<1)|((0x80&rcvbuf[1])>>7));
  ddu_code0=code;
  ddu_shift0=shft2in;
  //printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    //printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    //printf("      no right-shift needed\n");
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

  return code;
}
*/

/*
int emu::fed::DDU::ddu_rdtmberr()
{
int i,j,shft2in;

long int code;
  //printf(" ddu_rdtmberr \n");
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
  //printf(" TMB Error Register bits [14-0]: ");
  code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;
  for(j=14;j>=0;j--){
    //printf("%1ld",(code>>j)&0x00000001);
    //if((j/5)*5==j&&j>0)printf(".");
  }
  //printf(", Hex code %04lx\n",code);
  //  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  shft2in=(((0x7f&rcvbuf[3])<<9)|((0xff&rcvbuf[2])<<1)|((0x80&rcvbuf[1])>>7));
  ddu_code0=code;
  ddu_shift0=shft2in;
  //printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    //printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    //printf("      no right-shift needed\n");
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

  return code;
}
*/

/*
int emu::fed::DDU::ddu_rdlieerr()
{
int i,j,shft2in;

long int code;
  //printf(" ddu_rdlieerr \n");
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
  //printf(" Lost-In-Event Error Register bits [14-0]:\n                       ");
  code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;
  for(j=14;j>=0;j--){
    //printf("%1ld",(code>>j)&0x00000001);
    //if((j/5)*5==j&&j>0)printf(".");
  }
  //printf(",  Hex code %04lx\n",code);
  //  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  shft2in=(((0x7f&rcvbuf[3])<<9)|((0xff&rcvbuf[2])<<1)|((0x80&rcvbuf[1])>>7));
  ddu_code0=code;
  ddu_shift0=shft2in;
  //printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    //printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    //printf("      no right-shift needed\n");
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

  return code;
}
*/

/*
void emu::fed::DDU::ddu_rdliderr()
//JRG: Delete
{
int i,j,shft2in;

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
    printf("%1ld",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %04lx\n",code);
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
*/

/*
void emu::fed::DDU::ddu_rdpaferr()
//JRG: Delete
{
int i,j,shft0in,shft1in,shft2in;

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
    printf("%1ld",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %04lx\n",code);
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
*/

/*
int emu::fed::DDU::ddu_rdfferr()
{
int j,shft2in;

long int code;
  //printf(" ddu_rdfferr \n");
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
  //printf(" FIFO Full Register bits [14-0]:\n                       ");
//  code=((0x00ff&rcvbuf[0])|((0x0003&rcvbuf[1])<<8))&0x000003ff;
  code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;
  for(j=14;j>=0;j--){
    //printf("%1ld",(code>>j)&0x00000001);
    if((j==14)||(j==10)||(j==8)||(j==4))printf(".");
  }
  //printf(",  Hex code %04lx\n",code);
  //printf("         L1aMT.InMT.GbEFF+L1aFF.InRdFF.InFF\n");

//for(j=9;j>=0;j--){
//  printf("%1ld",(code>>j)&0x00000001);
//  if((j/4)*4==j&&j>0)printf(".");
//}
//printf(",  Hex code %04lx\n",code);

  // shft2in=(((0x03&rcvbuf[3])<<14)|((0xff&rcvbuf[2])<<6)|(0xfc&rcvbuf[1])>>2);
  shft2in=((0x00ff&rcvbuf[3])<<8)|((0x00ff&rcvbuf[2]));
  ddu_code0=code;
  ddu_shift0=shft2in;
  //printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
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

  return code;
}
*/

/*
unsigned int emu::fed::DDU::ddu_rderareg()
{
int i,j,shft0in,shft1in,shft2in;

long int code;
  //printf(" ddu_rderareg \n");
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
  //printf(" Error bus A Register bits [15-0]:\n                       ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  for(j=15;j>=0;j--){
    //printf("%1ld",(code>>j)&0x00000001);
    //if((j/4)*4==j&&j>0)printf(".");
  }
  //printf(",  Hex code %04lx\n",code);
  //  shft2in=( ((0xff&rcvbuf[3])<<8)|(0xff&rcvbuf[2]) );
  //JRG, 8 June 2003: occassional bad timing (bit loss) seen in VME readout
  //shft2in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  ddu_code0=code;
  ddu_shift0=shft2in;
  //printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    //printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    //printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  //  ddu_era_decode(code);
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

  return code;
}
*/

/*
unsigned int emu::fed::DDU::ddu_rderbreg()
{
int i,j,shft0in,shft1in,shft2in;

long int code;
  //printf(" ddu_rderbreg \n");
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
  //printf(" Error bus B Register bits [15-0]:\n                       ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  for(j=15;j>=0;j--){
    //printf("%1ld",(code>>j)&0x00000001);
    //if((j/4)*4==j&&j>0)printf(".");
  }
  //printf(",  Hex code %04lx\n",code);

  //  shft2in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  ddu_code0=code;
  ddu_shift0=shft2in;
  //printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    //printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    //printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  //  ddu_erb_decode(code);
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

  return code;
}
*/

/*
unsigned int emu::fed::DDU::ddu_rdercreg()
{
int i,j,shft0in,shft1in,shft2in;

long int code;
  //printf(" ddu_rdercreg \n");
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
  //printf(" Error bus C Register bits [15-0]:\n                       ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  for(j=15;j>=0;j--){
    //printf("%1ld",(code>>j)&0x00000001);
    //if((j/4)*4==j&&j>0)printf(".");
  }
  //printf(",  Hex code %04lx\n",code);

  //  shft2in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  ddu_code0=code;
  ddu_shift0=shft2in;

  //printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    //printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    //printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  //  ddu_erc_decode(code);
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

  return code;
}
*/

/*
unsigned int emu::fed::DDU::ddu_InRDstat()
// JRG, 16-bit Persistent Register, can include in Monitor Loop
//      Error triggered by any bits true.
{
int i,j,shft2in;
// enum DEVTYPE devstp,dv;
long int code;
  //printf(" ddu_InRDstat (DDU_Ctrl FPGA) \n");
  cmd[0]=VTX2P_USR1_L;
  cmd[1]=VTX2P_USR1_H;
  sndbuf[0]=19;
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
  //printf(" InRDctrl Status [15-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;

  for(j=15;j>=0;j--){
    //{printf("%1ld",(code>>j)&0x00000001);
    //if((j/4)*4==j&&j>0)printf(".");}
  }
  //printf(",  Hex code %04lx\n",code);
  //printf("                  InRDcrit.InRdsync.InRDsingle.InRDtimeout\n");
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xff&rcvbuf[2]));
  ddu_code0=code;
  ddu_shift0=shft2in;
  int ret=0;
  if((shft2in&0x0000ffff)!=0xFACE){
    ret=1;
    //printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
    for(i=0;i<7;i=i+4){
      //printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
      //printf("      no right-shift needed\n");
    }
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
  return code;
}
*/


unsigned long int emu::fed::DDU::ddu_InC_Hist()
// JRG, 16-bit Persistent Register, can include in Monitor Loop
//      Error triggered by any bits true.
{
	char cmd[2];
	char sndbuf[8];
	char rcvbuf[8];
	
	//int i,j,shft2in;
	// enum DEVTYPE devstp,dv;
	//long int code;
	//printf(" ddu_InC_Hist (DDU_Ctrl FPGA) \n");
	cmd[0]=VTX2P_USR1_L;
	cmd[1]=VTX2P_USR1_H;
	sndbuf[0]=20;
	devdo(DDUFPGA,10,cmd,8,sndbuf,rcvbuf,0);
	cmd[0]=VTX2P_BYPASS_L;
	cmd[1]=VTX2P_BYPASS_H;
	devdo(DDUFPGA,10,cmd,0,sndbuf,rcvbuf,0);
	cmd[0]=VTX2P_USR2_L;cmd[1]=VTX2P_USR2_H;
	sndbuf[0]=0xCE;
	sndbuf[1]=0xFA;
	sndbuf[2]=0x08;
	sndbuf[3]=0x04;
	sndbuf[4]=0x02;
	sndbuf[5]=0x00;
	sndbuf[6]=0x00;
	sndbuf[7]=0x00;
	devdo(DDUFPGA,10,cmd,44,sndbuf,rcvbuf,1);
	//printf(" InRDctrl MxmitErr Reg [15-12] & C-code History [8-0]: \n            ");
	unsigned long int code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
	
	//for(j=15;j>=0;j--){
		//printf("%1ld",(code>>j)&0x00000001);
		//if((j/4)*4==j&&j>0)printf(".");
		//if(j==10)printf(".");
	//}
	//printf(",  Hex code %04lx\n",code);
	//printf("    InRDMxmit(4).EndCerr/BeginCerr.0/InRdL1er(1).InRD_C-code(8)\n");
	//shft2in=(((0xff&rcvbuf[3])<<8)|(0xff&rcvbuf[2]));
	//ddu_code0=code;
	//ddu_shift0=shft2in;
	//int ret=0;
	//if((shft2in&0x0000ffff)!=0xFACE){
	//ret=1;
	//printf("   ----> 44-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
	//	for(i=0;i<7;i=i+4){
		//printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
		//printf("      no right-shift needed\n");
	//}
	//}
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
	
	return code;
}


/*
void emu::fed::DDU::ddu_rd_verr_cnt()
{
int i,shft0in,shft1in,shft2in;

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
  printf(" BitTest Vote Error Count = %ld,  Hex code %04lx\n",code,code);
  // shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xff&rcvbuf[2]));
  ddu_code0=code;
  ddu_shift0=shft2in;
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
*/

/*
void emu::fed::DDU::ddu_rd_cons_cnt()
{
int i,shft0in,shft1in,shft2in;

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
  printf(" BitTest Consecutive-Vote-Error Count = %ld,  Hex code %04lx\n",code,code);
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
*/

/*
void emu::fed::DDU::ddu_fifo0verr_cnt()
{
int i,shft0in,shft1in,shft2in;

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
  printf(" BitTest FIFO-0 Error Count = %ld,  Hex code %04lx\n",code,code);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  ddu_code0=code;
  ddu_shift0=shft2in;
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
*/

/*
void emu::fed::DDU::ddu_fifo1verr_cnt()
{
int i,shft0in,shft1in,shft2in;

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
  printf(" BitTest FIFO-1 Error Count = %ld,  Hex code %04lx\n",code,code);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  ddu_code0=code;
  ddu_shift0=shft2in;
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
*/

/*
void emu::fed::DDU::ddu_earlyVerr_cnt()
{
int i,shft0in,shft1in,shft2in;

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
  printf(" BitTest Early 200ns  Error Count = %ld,  Hex code %04lx\n",code,code);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  ddu_code0=code;
  ddu_shift0=shft2in;
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
*/

/*
void emu::fed::DDU::ddu_verr23cnt()
{
int i,shft0in,shft1in,shft2in;

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
  printf(" BitTest VoteError23 Count = %ld,  Hex code %04lx\n",code,code);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  ddu_code0=code;
  ddu_shift0=shft2in;
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
*/

/*
void emu::fed::DDU::ddu_verr55cnt()
{
int i,shft0in,shft1in,shft2in;

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
  printf(" BitTest VoteError55 Count = %ld,  Hex code %04lx\n",code,code);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xff&rcvbuf[2]));
  ddu_code0=code;
  ddu_shift0=shft2in;
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
*/

/*
unsigned int emu::fed::DDU::ddu_rdostat()
{
int i,shft0in,shft1in,shft2in;
long int code;
  //printf(" ddu_rdostat \n");
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
  //printf(" 16-bit DDU Output Status:  %02x%02x \n",0xff&rcvbuf[1],0xff&rcvbuf[0]);
  //  shft2in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  ddu_code0=code;
  ddu_shift0=shft2in;
  //printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    //printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    //printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  //  ddu5ostatus_decode(code);
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

  return code;
}
*/

/*
void emu::fed::DDU::ddu_rdempty()
//JRG: Delete
{
int i,j,shft0in,shft1in,shft2in;

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
    printf("%1ld",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %04lx\n",code);
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
*/

/*
void emu::fed::DDU::ddu_rdstuckbuf()
//JRG: Delete
{
int i,j,shft2in;

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
    printf("%1ld",(code>>j)&0x00000001);
    if((j/5)*5==j&&j>0)printf(".");
  }
  printf(",  Hex code %04lx\n",code);
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
*/


unsigned long int emu::fed::DDU::ddu_rdscaler()
{

	char cmd[2];
	char sndbuf[6];
	char rcvbuf[6];
	//printf(" ddu_rdscaler \n");
	
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
	//printf(" DDU L1 Event Scaler, bits [23-0]:  ");
	unsigned long int code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8)|((0x00ff&rcvbuf[2])<<16))&0x00ffffff;
	//printf("    Hex code %06lx\n",code);
	//printf("    Decimal count =  %8ld\n",code);
	//unsigned long int shft2in=(((0xff&rcvbuf[4])<<8)|((0xff&rcvbuf[3])));
	//ddu_code1=(0x00ff&rcvbuf[2]);
	//ddu_code0=(0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8);
	//ddu_shift0=shft2in;
	//printf("   ----> 49-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
	//for(i=0;i<7;i=i+4){
		//printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
		//printf("      no right-shift needed\n");
	//}
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
	
	return code;
}


/*
unsigned long int emu::fed::DDU::ddu_int_rdscaler()
{
	return ddu_rdscaler();
}
*/

/*
int emu::fed::DDU::ddu_rdalcterr()
{
int i,j,shft2in;

long int code;
  //printf(" ddu_rdalcterr \n");
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
  //printf(" ALCT Error Register bits [14-0]: ");
  code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;
  for(j=14;j>=0;j--){
    //printf("%1ld",(code>>j)&0x00000001);
    //if((j/5)*5==j&&j>0)printf(".");
  }
  //printf(", Hex code %04lx\n",code);
  //  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  shft2in=(((0x7f&rcvbuf[3])<<9)|((0xff&rcvbuf[2])<<1)|((0x80&rcvbuf[1])>>7));
  ddu_code0=code;
  ddu_shift0=shft2in;
  //printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    //printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    //printf("      no right-shift needed\n");
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

  return code;
}
*/


void emu::fed::DDU::ddu_loadbxorbit(int regval)
{
	char cmd[2];
	char sndbuf[8];
	char rcvbuf[8];
	
	//printf("    ddu_loadbxorbit,  Received value=%d \n",regval);
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
	//printf("          Sending to BX_Orbit:  %01X%02X \n",0x000F&sndbuf[4],0x00FF&sndbuf[3]);
	devdo(DDUFPGA,10,cmd,36,sndbuf,rcvbuf,1);
	//printf(" readback of Previous BX_Orbit setting, bits [11-0]:\n");
	//code=((0x00ff&rcvbuf[0])|((0x000f&rcvbuf[1])<<8))&0x00000fff;
	//  printf("    Hex code %03lx\n",code);
	//printf("    BX/Orbit = %ld  (%03lx hex)\n",code,code);
	
	//  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
	//  shft2in=(((0x1f&rcvbuf[3])<<11)|((0xff&rcvbuf[2])<<3)|(0xe0&rcvbuf[1])>>5);
	//shft2in=(((0x0f&rcvbuf[3])<<12)|((0xff&rcvbuf[2])<<4)|(0xf0&rcvbuf[1])>>4);
	//printf("   ----> 36-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
	//for(i=0;i<7;i=i+4){
		//printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
		//printf("      no right-shift needed\n");
	//}
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


/*
int emu::fed::DDU::ddu_rdbxorbit()
{
int i,shft2in;

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
  printf("    Hex code %03lx\n",code);
  printf("    Decimal BX/Orbit = %ld\n",code);
  //  printf("  Hex code %03lx\n",code);

  //  printf("   ----> 44-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  shft2in=(((0x0f&rcvbuf[3])<<12)|((0xff&rcvbuf[2])<<4)|(0xf0&rcvbuf[1])>>4);
  ddu_code0=code;
  ddu_shift0=shft2in;
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

  return code;
}
*/

/*
void emu::fed::DDU::ddu_lvl1onoff()
{
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
*/

/*
unsigned int emu::fed::DDU::ddu_rd_boardID()
{
 int i,shft0in,shft1in,shft2in;
 long int code;

  //printf(" ddu_rd_boardID \n");
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
  //printf(" 16-bit DDU Board ID:  %02x%02x \n",0xff&rcvbuf[1],0xff&rcvbuf[0]);
  //  shft2in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  ddu_code0=code;
  ddu_shift0=shft2in;
  //printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    //printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    //printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
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

  return code;
}
*/


unsigned long int emu::fed::DDU::ddu_fpgastat()
{
	char cmd[2];
	char sndbuf[17];
	char rcvbuf[17];
	//  printf(" ddu_fpgastat \n");
	
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
	unsigned long int code = ((((0x00ff&rcvbuf[2])|((0x00ff&rcvbuf[3])<<8))&0x0000ffff)<<16) | (((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff);
	//  printf(" 32-bit DDU Control DFPGA Status:  %02x%02x/%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
	//errcode=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8)|((0x00ff&rcvbuf[2])<<16)|((0x00ff&rcvbuf[3])<<24));
	//shft2in=(((0x01&rcvbuf[6])<<15)|((0xff&rcvbuf[5])<<7)|(0xfe&rcvbuf[4])>>1);
	//  shft2in=(((0xff&rcvbuf[5])<<8)|(0xff&rcvbuf[4]));
	//shft2in=(((0xff&rcvbuf[5])<<8)|(0xff&rcvbuf[4]));
	//ddu_code1=((0x00ff&rcvbuf[2])|((0x00ff&rcvbuf[3])<<8));
	//ddu_code0=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8));
	//ddu_shift0=shft2in;
	/*
	printf("   ----> 70-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
	for(i=0;i<11;i=i+4){
		shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
		shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
		printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
		printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
	}
	*/
	//  ddu5status_decode(errcode);
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
	
	return code;
}



std::vector<unsigned long int> emu::fed::DDU::ddu_occmon()
{
	char cmd[2];
	char sndbuf[17];
	char rcvbuf[17];

	std::vector<unsigned long int> retVal;
	
	//printf(" ddu_occmon (CSC Board Occupancy Monitor) \n");
	for(int j=0;j<4;j++){
		cmd[0]=VTX2P_USR1_L;
		cmd[1]=VTX2P_USR1_H;
		sndbuf[0]=34;
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
		retVal.push_back((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8)|((0x00ff&rcvbuf[2])<<16)|((0x00ff&rcvbuf[3])<<24));

		//shft2in=(((0xff&rcvbuf[5])<<8)|(0xff&rcvbuf[4]));
		//if(j==0)ddu_shift0=shft2in;
		//if((shft2in&0x0000ffff)!=0xFACE){
		//	ddu_shift0=shft2in;
			//printf("   ----> 70-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
		//	for(int i=0;i<11;i=i+4){
		//		shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
		//		shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
				//printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
				//printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
		//	}
		//}
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

	return retVal;
}



std::vector<unsigned long int> emu::fed::DDU::ddu_fpgatrap()
// JRG, 192-bits, Uses custom decode routine, skip for now in Monitor Loop
{

	char cmd[2];
	char sndbuf[33];
	char rcvbuf[33];
	//printf(" ddu_fpgatrap \n");

	std::vector<unsigned long int> retVal;

	cmd[0]=VTX2P_USR1_L;
	cmd[1]=VTX2P_USR1_H;
	sndbuf[0]=21;
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
	sndbuf[17]=0x00;
	sndbuf[18]=0x00;
	sndbuf[19]=0x00;
	sndbuf[20]=0x00;
	sndbuf[21]=0x00;
	sndbuf[22]=0x00;
	sndbuf[23]=0x00;
	sndbuf[24]=0x00;
	sndbuf[25]=0x00;
	sndbuf[26]=0x00;
	sndbuf[27]=0x00;
	sndbuf[28]=0x00;
	sndbuf[29]=0x00;
	sndbuf[30]=0x00;
	sndbuf[31]=0x00;
	sndbuf[32]=0x00;
	devdo(DDUFPGA,10,cmd,224,sndbuf,rcvbuf,1);
	//printf("  192-bit DDU Control Diagnostic Trap (24 bytes) \n");
	//i=23;
	//printf("                        ostat   fful  fifo-c fifo-b \n");
	//printf("      rcv bytes %2d-%2d:   %02x%02x   %02x%02x   %02x%02x   %02x%02x \n",i,i-7,0xff&rcvbuf[i],0xff&rcvbuf[i-1],0xff&rcvbuf[i-2],0xff&rcvbuf[i-3],0xff&rcvbuf[i-4],0xff&rcvbuf[i-5],0xff&rcvbuf[i-6],0xff&rcvbuf[i-7]);
	//i=15;
	//printf("\n                        fifo-a instat c-code  erc \n");
	//printf("      rcv bytes %2d-%2d:   %02x%02x   %02x%02x   %02x%02x   %02x%02x \n",i,i-7,0xff&rcvbuf[i],0xff&rcvbuf[i-1],0xff&rcvbuf[i-2],0xff&rcvbuf[i-3],0xff&rcvbuf[i-4],0xff&rcvbuf[i-5],0xff&rcvbuf[i-6],0xff&rcvbuf[i-7]);
	//i=7;
	//printf("\n                         erb    era   32-bit status \n");
	//printf("      rcv bytes %2d-%2d:   %02x%02x   %02x%02x   %02x%02x   %02x%02x \n\n",i,i-7,0xff&rcvbuf[i],0xff&rcvbuf[i-1],0xff&rcvbuf[i-2],0xff&rcvbuf[i-3],0xff&rcvbuf[i-4],0xff&rcvbuf[i-5],0xff&rcvbuf[i-6],0xff&rcvbuf[i-7]);
	
	retVal.push_back((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8)|((0x00ff&rcvbuf[2])<<16)|((0x00ff&rcvbuf[3])<<24));
	retVal.push_back((0x00ff&rcvbuf[4])|((0x00ff&rcvbuf[5])<<8)|((0x00ff&rcvbuf[6])<<16)|((0x00ff&rcvbuf[7])<<24));
	retVal.push_back((0x00ff&rcvbuf[8])|((0x00ff&rcvbuf[9])<<8)|((0x00ff&rcvbuf[10])<<16)|((0x00ff&rcvbuf[11])<<24));
	retVal.push_back((0x00ff&rcvbuf[12])|((0x00ff&rcvbuf[13])<<8)|((0x00ff&rcvbuf[14])<<16)|((0x00ff&rcvbuf[15])<<24));
	retVal.push_back((0x00ff&rcvbuf[16])|((0x00ff&rcvbuf[17])<<8)|((0x00ff&rcvbuf[18])<<16)|((0x00ff&rcvbuf[19])<<24));
	retVal.push_back((0x00ff&rcvbuf[20])|((0x00ff&rcvbuf[21])<<8)|((0x00ff&rcvbuf[22])<<16)|((0x00ff&rcvbuf[23])<<24));
	//shft2in=(((0xff&rcvbuf[25])<<8)|(0xff&rcvbuf[24]));
	//ddu_shift0=shft2in;
	//int ret=0;
	//if((shft2in&0x0000ffff)!=0xFACE){
		//ret=1;
		//printf("   ----> 224-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
		//for(i=0;i<27;i=i+4){
		//shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
		//shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
		//printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
		//printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
		//}
		//}
	//printf("   32-bit DDU Control DFPGA Status:  %02x%02x/%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
	//  ddu5status_decode(errcode);
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

	return retVal;
}


/*
void emu::fed::DDU::ddu_reset()
{
  printf(" Enter DDUFPGA reset. \n");
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
*/

/*
void emu::fed::DDU::ddu_l1calonoff()
{
  printf(" Enter ddu_l1calonoff (toggle). \n");
  cmd[0]=VTX2P_USR1_L;
  cmd[1]=VTX2P_USR1_H;
  sndbuf[0]=31;
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
  printf(" DDUFPGA Calibration==L1A Toggle done. \n");
}
*/


void emu::fed::DDU::ddu_vmel1a()
{
	char cmd[2];
	char sndbuf[1];
	char rcvbuf[1];
	
	//printf(" Enter ddu_vmel1a. \n");
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
	//printf(" DDUFPGA VME L1A done. \n");
}

/*
void emu::fed::DDU::ddu_status_decode(int long code)  // Old outdated DDU3? do not use!
{
// JRG 21feb06, old outdated routine, only good for DDU prototypes!

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
//Pre-ver51 encoding:
// if((0x80000000&code)>0)printf("   DDU G-Bit FIFO Not Empty");
// if((0x40000000&code)>0)printf("   DDU G-Bit FIFO Near Full Warning");
// if((0x20000000&code)>0)printf("   DDU G-Bit FIFO Full Warning");

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
//Pre-ddu2ctrl_ver57:
// if((0x00040000&code)>0)printf("   DDU FIFO-PAF Warning");
// if((0x00020000&code)>0)printf("   DDU L1A-FIFO Near Full Warning");
//
//
//Pre-ddu3ctrl_v8r15576:
// if((0x00040000&code)>0)printf("   DDU Trigger Readout CRC Error");
// if((0x00020000&code)>0)printf("   DDU Trigger Readout Wordcount Error");
// if((0x00010000&code)>0)printf("   DDU L1A-FIFO Full Error");
//
   if((0x00040000&code)>0)printf("   DDU ALCT Error");
   if((0x00020000&code)>0)printf("   DDU Trigger Readout Wordcount Error");
   if((0x00010000&code)>0)printf("   DDU Trigger L1A Match Error");
   printf("\n");
 }
}
*/

/*
void emu::fed::DDU::ddu_ostatus_decode(int long code)  // Old outdated DDU3? do not use!
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
*/

/*
void emu::fed::DDU::ddu_era_decode(int long code)
{
//printf("\nReceived code=%08X\n",code);
// JRG, 16-bit DDU Error Bus A:
 if((code&0x0000F000)>0){
   if((0x00008000&code)>0)printf("   DDU Critical Error, ** needs reset **\n");
// for DDU3/4 & DDU5 up to DDUctrl v8.
// if((0x00004000&code)>0)printf("   DDU DMB Error occurred");
//
   if((0x00004000&code)>0)printf("   DDU Mult L1A Error occurred");
   if((0x00002000&code)>0)printf("   DDU L1A-FIFO Near Full Warning");
   if((0x00001000&code)>0)printf("   DDU Gigabit Ethernet FIFO PAF flag");
   printf("\n");
 }
 if((code&0x00000F00)>0){
   if((0x00000800&code)>0)printf("   DDU Ext.FIFO Near Full Warning");
   if((0x00000400&code)>0)printf("   DDU Near Full Warning");
   if((0x00000200&code)>0)printf("   DDU CFEB-CRC not OK");
   if((0x00000100&code)>0)printf("   DDU CFEB-CRC End Error");
   printf("\n");
 }
 if((code&0x000000F0)>0){
   if((0x00000080&code)>0)printf("   DDU CFEB-CRC Count Error");
   if((0x00000040&code)>0)printf("   DDU CFEB-CRC Error occurred");
   //   if((0x00000020&code)>0)printf("   DDU Latched Trigger Trail");
   if((0x00000020&code)>0)printf("   DDU Trigger Readout Error");
   if((0x00000010&code)>0)printf("   DDU Trigger Trail Done");
   printf("\n");
 }
 if((code&0x0000000F)>0){
   if((0x00000008&code)>0)printf("   DDU Start Timeout");
   if((0x00000004&code)>0)printf("   DDU End Timeout");
   if((0x00000002&code)>0)printf("   DDU SP/TF Error in event");
   if((0x00000001&code)>0)printf("   DDU SP/TF data in event");
   printf("\n");
 }
}
*/

/*
void emu::fed::DDU::ddu_erb_decode(int long code)
{
//printf("\nReceived code=%08X\n",code);
// JRG, 16-bit DDU Error Bus B:
 if((code&0x0000F000)>0){
   if((0x00008000&code)>0)printf("   DDU Lost In Event Error");
   if((0x00004000&code)>0)printf("   DDU DMB Error in event");
   if((0x00002000&code)>0)printf("   DDU Control DLL Error occured");
   if((0x00001000&code)>0)printf("   DDU 2nd Header First flag");
   printf("\n");
 }
 if((code&0x00000F00)>0){
   if((0x00000800&code)>0)printf("   DDU Early 2nd Trailer flag");
   if((0x00000400&code)>0)printf("   DDU Extra 1st Trailer flag");
// for DDU3/4 & DDU5 up to DDUctrl v8.
// if((0x00000200&code)>0)printf("   DDU Extra Trigger Trailer flag");
//
   if((0x00000200&code)>0)printf("   DDU Extra 1st Header flag");
   if((0x00000100&code)>0)printf("   DDU Extra 2nd Header flag");
   printf("\n");
 }
 if((code&0x000000F0)>0){
// for DDU3/4 & DDU5 up to DDUctrl v8.
// if((0x00000080&code)>0)printf("   DDU CFEB-DMB Error flag");
// if((0x00000040&code)>0)printf("   DDU First Header flag");

   if((0x00000080&code)>0)printf("   DDU SCA Full detected this Event");
   if((0x00000040&code)>0)printf("   DDU DMB Full occurred");
   if((0x00000020&code)>0)printf("   DDU Lone Word DMB in event");
   if((0x00000010&code)>0)printf("   DDU Bad Control Word Error occurred");
   printf("\n");
 }
 if((code&0x0000000F)>0){
   if((0x00000008&code)>0)printf("   DDU Missed Trigger Trail");
   if((0x00000004&code)>0)printf("   DDU First Dat Error");
   if((0x00000002&code)>0)printf("   DDU Bad First Word");
   if((0x00000001&code)>0)printf("   DDU Lost In Data occured");
   printf("\n");
 }
}
*/

/*
void emu::fed::DDU::ddu_erc_decode(int long code)
{
//printf("\nReceived code=%08X\n",code);
// JRG, 16-bit DDU Error Bus C:
 if((code&0x0000F000)>0){
   if((0x00008000&code)>0)printf("   DDU Trigger Readout Error");
   if((0x00004000&code)>0)printf("   DDU ALCT Trailer Done");
   if((0x00002000&code)>0)printf("   DDU ALCT DAV Vote True occurred");
   //   if((0x00001000&code)>0)printf("   DDU do_ALCT flag");
   if((0x00001000&code)>0)printf("   DDU ALCT L1 mismatch occurred");
   printf("\n");
 }
 if((code&0x00000F00)>0){
   if((0x00000800&code)>0)printf("   DDU ALCT CRC Error occurred");
   if((0x00000400&code)>0)printf("   DDU ALCT Wordcount Error occurred");
   if((0x00000200&code)>0)printf("   DDU Missing ALCT Trailer occurred");
   if((0x00000100&code)>0)printf("   DDU ALCT Error occurred");
   printf("\n");
 }
 if((code&0x000000F0)>0){
   if((0x00000080&code)>0)printf("   DDU Compare Trigger CRC flag");
   if((0x00000040&code)>0)printf("   DDU TMB Trailer Done");
   if((0x00000020&code)>0)printf("   DDU TMB DAV Vote True occurred");
   //   if((0x00000010&code)>0)printf("   DDU do_TMB flag");
   if((0x00000010&code)>0)printf("   DDU TMB L1 mismatch occurred");
   printf("\n");
 }
 if((code&0x0000000F)>0){
   if((0x00000008&code)>0)printf("   DDU TMB CRC Error occurred");
   if((0x00000004&code)>0)printf("   DDU TMB Word Count Error occurred");
   if((0x00000002&code)>0)printf("   DDU Missing TMB Trailer occurred");
   if((0x00000001&code)>0)printf("   DDU TMB Error occurred");
   printf("\n");
 }
}
*/

/*
void emu::fed::DDU::ddu5status_decode(int long code)
{
// JRG, low-order 16-bit status (most serious errors):
 if((code&0x0000F000)>0){
   if((0x00008000&code)>0)printf("   DDU Critical Error, ** needs reset **\n");
   if((0x00004000&code)>0)printf("   DDU Single Error, bad event");
   if((0x00002000&code)>0)printf("   DDU single warning, possible data problem");
   if((0x00001000&code)>0)printf("   DDU Near Full Warning");
   printf("\n");
 }
 if((code&0x00000F00)>0){
   if((0x00000800&code)>0)printf("   DDU RX Error");
   if((0x00000400&code)>0)printf("   DDU Control DLL Error (recent)");
   if((0x00000200&code)>0)printf("   DDU DMB Error in event");
   if((0x00000100&code)>0)printf("   DDU Lost In Event Error");
   printf("\n");
 }
 if((code&0x000000F0)>0){
   if((0x00000080&code)>0)printf("   DDU Lost In Data Error occurred");
   if((0x00000040&code)>0)printf("   DDU Timeout Error occurred");
   if((0x00000020&code)>0)printf("   DDU Trigger CRC Error");
   if((0x00000010&code)>0)printf("   DDU Multiple Transmit Errors occurred");
   printf("\n");
 }
 if((code&0x0000000F)>0){
   if((0x00000008&code)>0)printf("   DDU Sync Lost occurred (FIFO Full or L1A Error)");
   if((0x00000004&code)>0)printf("   DDU Fiber Connection Error");
   if((0x00000002&code)>0)printf("   DDU L1A Match Error");
   if((0x00000001&code)>0)printf("   DDU CFEB CRC Error");
   printf("\n");
 }
 if((code&0xF0000000)>0){
// JRG, high-order 16-bit status (not-so-serious errors):
   if((0x80000000&code)>0)printf("   DDU DMB DAV/LCT/MOVLP Mismatch");
   if((0x40000000&code)>0)printf("   DDU CFEB L1 Mismatch");
   if((0x20000000&code)>0)printf("   DDU saw No Good DMB CRCs");
   if((0x10000000&code)>0)printf("   DDU CFEB Count Mismatch");
   printf("\n");
 }
 if((code&0x0F000000)>0){
   if((0x08000000&code)>0)printf("   DDU FirstDat Error");
   if((0x04000000&code)>0)printf("   DDU L1A-FIFO Full Error");
   if((0x02000000&code)>0)printf("   DDU Data Stuck in FIFO");
   if((0x01000000&code)>0)printf("   DDU NoLiveFibers");
   printf("\n");
 }
 if((code&0x00F00000)>0){
   if((0x00800000&code)>0)printf("   DDU Spwd voted-bit Warning");
   if((0x00400000&code)>0)printf("   DDU InRDctrl Error");
   if((0x00200000&code)>0)printf("   DDU DAQ Stop Bit set");
   if((0x00100000&code)>0)printf("   DDU DAQ Not Ready");
   if((0x00300000&code)==0x00200000)printf("\n     DDU DAQ Stopped (DCC/S-Link backpressure)");
   printf("\n");
 }
 if((code&0x000F0000)>0){
   if((0x00080000&code)>0)printf("   DDU TMB Error");
   if((0x00040000&code)>0)printf("   DDU ALCT Error");
   if((0x00020000&code)>0)printf("   DDU Trigger Wordcount Error");
   if((0x00010000&code)>0)printf("   DDU Trigger L1A Match Error");
   printf("\n");
 }
}
*/

/*
void ddu5begin_decode(int long begin_status)
{
// Begin_Status_Decode:
  if(begin_status&0x0001){
    printf("  DDU-BOE: Start Timeout");
  }
  if(begin_status&0x0002){
    printf("  DDU-BOE: L1A Mismatch");
  }
  if(begin_status&0x0004){
    printf("  DDU-BOE: Single Error");
  }
  if(begin_status&0x0008){
    printf("  DDU-BOE: Critical Error, reset needed");
  }
  if((begin_status&0x000f)&&(begin_status&0x0070))printf("\n");
  if(begin_status&0x0010)printf("  DDU-BOE: Fiber Connection Error");
  if(begin_status&0x0020){
    printf("  DDU-BOE: Sync Lost (FIFO Full or L1A Error)");
    //    printf("  DDU-BOE: DDU Full FIFO");
  }
  if(begin_status&0x0040){
    printf("  DDU-BOE: single warning, possible data problems");
    //    printf("  DDU-BOE: DDU DLL Error");
  }
  printf("\n");
}
*/

/*
void ddu5ostatus_decode(int long code)
{
//printf("\nReceived code=%08X\n",code);
// JRG, 16-bit DDU output path status:
 if((code&0x0000F000)>0){
   if((0x00008000&code)>0)printf("   DDU Output Limited Buffer Overflow occurred\n");
   if((0x00004000&code)>0)printf("   DAQ (DCC/S-Link) Wait occurred");
   if((0x00002000&code)>0)printf("   DDU S-Link Full occurred");
   if((0x00001000&code)>0)printf("   DDU S-Link Never Ready");
   printf("\n");
 }
 if((code&0x00000F00)>0){
   if((0x00000800&code)>0)printf("   DDU Gigabit Ethernet Overflow occurred");
   if((0x00000400&code)>0)printf("   DDU Gigabit Ethernet Xmit Limit occurred");
   if((0x00000200&code)>0)printf("   DDU Gigabit Ethernet FIFO Always Empty");
   if((0x00000100&code)>0)printf("   DDU Gigabit Ethernet Fiber Error occurred");
   printf("\n");
 }
 if((code&0x000000F0)>0){
   if((0x00000080&code)>0)printf("   DAQ (DCC/S-Link) Limited Overflow occurred\n");
   if((0x00000040&code)>0)printf("   DAQ (DCC/S-Link Wait");
   if((0x00000020&code)>0)printf("   DDU S-Link Full/Stop");
   if((0x00000010&code)>0)printf("   DDU S-Link Not Ready");
   printf("\n");
 }
 if((code&0x0000000F)>0){
   if((0x00000008&code)>0)printf("   DDU Gigabit Ethernet FIFO Full");
   if((0x00000004&code)>0)printf("   DDU Skipped SPY Event (GbE data not sent)");
   if((0x00000002&code)>0)printf("   DDU Gigabit Ethernet FIFO Not Empty");
   if((0x00000001&code)>0)printf("   DCC Link Not Ready");
   printf("\n");
 }
}
*/

/*
void ddu5vmestat_decode(int long code)
{
//printf("\nReceived code=%08X\n",code);
// JRG, 16-bit DDU VME FPGA status:
 unsigned short int VMEslot=0;
 VMEslot=(~code)&0x001f;
 printf(" DDU VME slot = %d\n",VMEslot);
 if((code&0x0000F000)>0){
   if((0x00008000&code)>0)printf("   DDU-VME Reports Problem");
   if((0x00004000&code)>0)printf("   DDU-VME DLL Lost Lock occurred");
   if((0x00002000&code)>0)printf("   DDU-VME DLL Lock Failed or RESET");
   if((0x00001000&code)>0)printf("   DDU Never Ready");
   printf("\n");
 }
 if((0x00000F00&code)==0x800)printf("   DDU reports OK to FMM. \n");
 else if((0x00000F00&code)==0xC00)printf("   DDU reports ERROR to FMM. \n");
 else if((0x00000F00&code)==0x400)printf("   DDU reports BUSY to FMM. \n");
 else if((0x00000F00&code)==0x200)printf("   DDU reports LostSYNC to FMM. \n");
 else if((0x00000F00&code)==0x100)printf("   DDU reports WARN to FMM. \n");
 else printf("   Invalid FMM code read from DDU. \n");
 if((code&0x000000C0)>0){
   if((0x00000080&code)>0)printf("   DDU-VME DLL-2 Not Locked");
   if((0x00000040&code)>0)printf("   DDU-VME DLL-1 Not Locked");
   printf("\n");
 }
}
*/

/*
void in_stat_decode(int long code)
{
// JRG, low-order 16-bit status (most serious errors):
 if((code&0x0000F000)>0){
   if((0x00008000&code)>0)printf("   InFPGA Critical Error, ** needs reset **\n");
   if((0x00004000&code)>0)printf("   InFPGA Single Error, bad event");
   if((0x00002000&code)>0)printf("   InFPGA single warning, possible data problem");
   if((0x00001000&code)>0)printf("   InFPGA Near Full Warning");
   printf("\n");
 }
 if((code&0x00000F00)>0){
   if((0x00000800&code)>0)printf("   InFPGA RX Error");
   if((0x00000400&code)>0)printf("   InFPGA DLL Error (temp)");
   if((0x00000200&code)>0)printf("   InFPGA SCA Full detected");
   //   if((0x00000100&code)>0)printf("   InFPGA Special Word Error");
   if((0x00000100&code)>0)printf("   InFPGA Spwd voted-bit Warning");
   printf("\n");
 }
 if((code&0x000000F0)>0){
   if((0x00000080&code)>0)printf("   InFPGA Stuck Data occurred");
   if((0x00000040&code)>0)printf("   InFPGA Timeout Occurred");
   if((0x00000020&code)>0)printf("   InFPGA Multiple voted-bit Errors");
   if((0x00000010&code)>0)printf("   InFPGA Multiple Transmit Errors");
   printf("\n");
 }
 if((code&0x0000000F)>0){
   if((0x00000008&code)>0)printf("   InFPGA Mem/FIFO Full Error");
   if((0x00000004&code)>0)printf("   InFPGA Fiber Connection Error");
   if((0x00000002&code)>0)printf("   InFPGA L1A Match Error");
   if((0x00000001&code)>0)printf("   InFPGA Not Ready Error");
   printf("\n");
 }
 if((code&0xF0000000)>0){
   if((0x80000000&code)>0)printf("   InFPGA DDL2 Not Locked");
   if((0x40000000&code)>0)printf("   InFPGA DDL1 Not Locked");
   if((0x20000000&code)>0)printf("   InFPGA RdCtrl-1 Not Ready");
   if((0x10000000&code)>0)printf("   InFPGA RdCtrl-0 Not Ready");
   printf("\n");
 }
 if((code&0x0F000000)>0){
   if((0x08000000&code)>0)printf("   InFPGA NoLiveFiber 0 or 1");
   if((0x04000000&code)>0)printf("   InFPGA DLL Error occurred");
   if((0x02000000&code)>0)printf("   InFPGA InRD1 DMB Warn");
   if((0x01000000&code)>0)printf("   InFPGA InRD0 DMB Warn");
   printf("\n");
 }
 if((code&0x00F00000)>0){
   if((0x00800000&code)>0)printf("   InFPGA InRD1 DMB Full occurred");
   if((0x00400000&code)>0)printf("   InFPGA Mem/FIFO-1 Error occurred");
   if((0x00200000&code)>0)printf("   InFPGA MultL1A Error-1 occurred");
   if((0x00100000&code)>0)printf("   InFPGA NoLiveFiber-1");
   printf("\n");
 }
 if((code&0x000F0000)>0){
   if((0x00080000&code)>0)printf("   InFPGA InRD0 DMB Full occurred");
   if((0x00040000&code)>0)printf("   InFPGA Mem/FIFO-0 Error occurred");
   if((0x00020000&code)>0)printf("   InFPGA MultL1A Error-0 occurred");
   if((0x00010000&code)>0)printf("   InFPGA NoLiveFiber-0");
   printf("\n");
 }
}
*/

/*
void in_Ccode_decode(int long code)
{
// JRG, high-order 8-bits for InRD1:
 if((code&0x0000F000)>0){
   if((0x00008000&code)>0)printf("   InRD1 Critical Error, ** needs reset **\n");
   if((0x00004000&code)>0)printf("   InRD1 Sync Error, * needs reset *\n");
   if((0x00002000&code)>0)printf("   InRD1 Single Error, bad event");
   if((0x00001000&code)>0)printf("   InRD1 Memory Full Error occurred");
   printf("\n");
 }
 if((code&0x00000F00)>0){
   if((0x00000800&code)>0)printf("   InRD1 Fiber Connection Error occurred");
   if((0x00000400&code)>0)printf("   InRD1 Multi-Xmit-Error occurred");
   if((0x00000200&code)>0)printf("   InRD1 Stuck Data occurred");
   if((0x00000100&code)>0)printf("   InRD1 Timeout Error occurred");
   printf("\n");
 }
// JRG, low-order 8-bits for InRD0:
 if((code&0x000000F0)>0){
   if((0x00000080&code)>0)printf("   InRD0 Critical Error, ** needs reset **\n");
   if((0x00000040&code)>0)printf("   InRD0 Sync Error, * needs reset *\n");
   if((0x00000020&code)>0)printf("   InRD0 Single Error, bad event");
   if((0x00000010&code)>0)printf("   InRD0 Memory Full Error occurred");
   printf("\n");
 }
 if((code&0x0000000F)>0){
   if((0x00000008&code)>0)printf("   InRD0 Fiber Connection Error occurred");
   if((0x00000004&code)>0)printf("   InRD0 Multi-Xmit-Error occurred");
   if((0x00000002&code)>0)printf("   InRD0 Stuck Data occurred");
   if((0x00000001&code)>0)printf("   InRD0 Timeout Error occurred");
   printf("\n");
 }
}
*/

/*
unsigned long int emu::fed::DDU::ddufpga_idcode()
{
 enum DEVTYPE dv;
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
*/

/*
unsigned long int emu::fed::DDU::infpga_idcode0()
{
enum DEVTYPE dv;
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
*/

/*
unsigned long int emu::fed::DDU::infpga_idcode1()
{
enum DEVTYPE dv;
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
*/

/*
unsigned long int emu::fed::DDU::ddufpga_usercode()
{
enum DEVTYPE dv;
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
*/

/*
unsigned long int emu::fed::DDU::infpga_usercode0()
{
enum DEVTYPE dv;
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
*/

/*
unsigned long int emu::fed::DDU::infpga_usercode1()
{
enum DEVTYPE dv;
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
*/

/*
void emu::fed::DDU::infpga_shfttst(enum DEVTYPE dv,int tst)
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
  infpga_shift0=shft2in;
}
*/


void emu::fed::DDU::infpga_reset(enum DEVTYPE dv)
{
	char cmd[2];
	char sndbuf[1];
	char rcvbuf[1];
	
	//printf(" Enter INFPGA reset. \n");
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
	//printf(" INFPGA reset done. \n");
}



unsigned long int emu::fed::DDU::infpga_rdscaler(enum DEVTYPE dv)
{

	char cmd[2];
	char sndbuf[6];
	char rcvbuf[6];
	
	//printf(" infpga_rdscaler \n");
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
	//printf(" DDU-InFPGA L1 Event Scaler, bits [23-0]:  ");
	unsigned long int code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8)|((0x00ff&rcvbuf[2])<<16))&0x00ffffff;
	//printf("    Hex code %06lx\n",code);
	//printf("    Decimal count =  %8ld\n",code);
	//infpga_code1=(0x00ff&rcvbuf[2]);
	//infpga_code0=code&0xffff;
	//shft2in=(((0xff&rcvbuf[4])<<8)|((0xff&rcvbuf[3])));
	//infpga_shift0=shft2in;
	//printf("   ----> 49-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
	//for(i=0;i<7;i=i+4){
		//printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
		//printf("      no right-shift needed\n");
	//}
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
	
	return code;
}


/*
int emu::fed::DDU::ddu_dmblive()
// JRG, 15-bits, can include in Monitor Loop; has a transient moment at
//   _every_ Begining-of-Event, but all events should end w/the same state
//      Error not triggered here!
{
int i,j,shft2in;
// enum DEVTYPE devstp,dv;
long int code;
  //printf(" ddu_dmblive \n");
  cmd[0]=VTX2P_USR1_L;
  cmd[1]=VTX2P_USR1_H;
  sndbuf[0]=25;
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
  //printf(" DMBLIVE bits [14-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;

  for(j=14;j>=0;j--){
    //printf("%1ld",(code>>j)&0x00000001);
    //if((j/4)*4==j&&j>0)printf(".");
  }
  //printf(",  Hex code %04lx\n",code);
  shft2in=(((0x7f&rcvbuf[3])<<9)|((0xff&rcvbuf[2])<<1)|((0x80&rcvbuf[1])>>7));
  ddu_code0=code;
  ddu_shift0=shft2in;
  int ret=0;
  if((shft2in&0x0000ffff)!=0xFACE){
    ret=1;
    //printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
    for(i=0;i<7;i=i+4){
      //printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
      //printf("      no right-shift needed\n");
    }
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
  return code;
}
*/

/*
int emu::fed::DDU::ddu_pdmblive()
// JRG, 15-bits, can include in Monitor Loop; has a transient moment at
//   _first_ Begining-of-Event, Persistent thereafter.
//   Should compare exactly with DMBlive.
//      Error not triggered here!
{
int i,j,shft2in;
// enum DEVTYPE devstp,dv;
long int code;
  //printf(" ddu_pdmblive \n");
  cmd[0]=VTX2P_USR1_L;
  cmd[1]=VTX2P_USR1_H;
  sndbuf[0]=26;
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
  //printf(" Permanent DMBLIVE bits [14-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x007f&rcvbuf[1])<<8))&0x00007fff;
  for(j=14;j>=0;j--){
    //printf("%1ld",(code>>j)&0x00000001);
    //if((j/4)*4==j&&j>0)printf(".");
  }
  //printf(",  Hex code %04lx\n",code);
  shft2in=(((0x7f&rcvbuf[3])<<9)|((0xff&rcvbuf[2])<<1)|((0x80&rcvbuf[1])>>7));
  ddu_code0=code;
  ddu_shift0=shft2in;
  int ret=0;
  if((shft2in&0x0000ffff)!=0xFACE){
    ret=1;
    //printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
    for(i=0;i<7;i=i+4){
      //printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
      //printf("      no right-shift needed\n");
    }
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
  return code;
}
*/

/*
int emu::fed::DDU::ddu_rd_WarnMon()
// JRG, 16-bit Register, can include in Monitor Loop
//      Error triggered by any bits true, indicates a Warning state occurred
//         bits 7-0 are not persistent
{
int i,shft0in,shft1in,shft2in;
long int code;
// enum DEVTYPE devstp,dv;
  printf(" ddu_rd_WarnMon \n");
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
  printf(" 16-bit DDU FMM-Warn Monitor:  %02x%02x \n",0xff&rcvbuf[1],0xff&rcvbuf[0]);
  code=((rcvbuf[1]&0x000000ff)<<8)|(rcvbuf[0]&0x000000ff);
  //  shft2in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xff&rcvbuf[2]));
  ddu_code0=code;
  ddu_shift0=shft2in;
  int ret=0;
  if((shft2in&0x0000ffff)!=0xFACE){
    ret=1;
    printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
    for(i=0;i<7;i=i+4){
      shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
      shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
      printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
      printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
    }
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
  return ret;
}
*/

/*
void emu::fed::DDU::ddu_maxTimeCount()
// JRG, 16-bits, maybe read once at run/Loop start, Persistent.
//      Error not triggered here!
{
int i,shft0in,shft1in,shft2in;
  printf(" ddu_maxTimeCount \n");
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
  printf(" 16-bit DDU Max Timeout Counts:  %02x %02x \n",0xff&rcvbuf[1],0xff&rcvbuf[0]);
  printf("   End-Timeout Counter Max =  %d (%04xh, %d ns) \n",(0xff&rcvbuf[1])*256,(0xff&rcvbuf[1])*256,(0xff&rcvbuf[1])*256*25);
  printf("   Start-Timeout Counter Max =  %d (%04xh, %d ns) \n",(0xff&rcvbuf[0])*16,(0xff&rcvbuf[0])*16,(0xff&rcvbuf[0])*16*25);
  ddu_code0=((rcvbuf[1]&0x000000ff)<<8)|(rcvbuf[0]&0x000000ff);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xff&rcvbuf[2]));
  ddu_shift0=shft2in;
  int ret=0;
  if((shft2in&0x0000ffff)!=0xFACE){
    ret=1;
    printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
    for(i=0;i<7;i=i+4){
      shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
      shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
      printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
      printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
    }
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
  //  return ret;
}
*/


unsigned long int emu::fed::DDU::infpga_rd1scaler(enum DEVTYPE dv)
// JRG, 24-bits, can include in Monitor Loop, changes for each event
//      Error not triggered here!
{
	char cmd[2];
	char sndbuf[6];
	char rcvbuf[6];
	
	//  if (bar==NULL)printf(" infpga_rd1scaler \n");
	cmd[0]=VTX2P20_USR1_L;cmd[1]=VTX2P20_USR2_H;
	sndbuf[0]=26;
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
	//  if (bar==NULL)
	//printf(" DDU-InFPGA L1 Event Scaler1, bits [23-0]:  ");
	unsigned long int code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8)|((0x00ff&rcvbuf[2])<<16))&0x00ffffff;
	//  if (bar==NULL)
	//printf("    Hex code %06lx\n",code);
	//  if (bar==NULL)
	//printf("    Decimal count =  %8ld\n",code);
	//infpga_code1=(0x00ff&rcvbuf[2]);
	//infpga_code0=code&0xffff;
	//shft2in=(((0xff&rcvbuf[4])<<8)|((0xff&rcvbuf[3])));
	//infpga_shift0=shft2in;
	//int ret=0;
	//if((shft2in&0x0000ffff)!=0xFACE){
	//	ret=1;
	//	printf("   ----> 49-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
	//	for(i=0;i<7;i=i+4){
	//	printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
	//	printf("      no right-shift needed\n");
	//	}
	//}
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
	return code;
}


/*
void emu::fed::DDU::infpga_lowstat(enum DEVTYPE dv)
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
  infpga_code0=code;
  //shft2in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xff&rcvbuf[2]));
  infpga_shift0=shft2in;
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  //  in_stat_decode(code);
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
*/

/*
void emu::fed::DDU::infpga_histat(enum DEVTYPE dv)
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
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  infpga_code0=code;
  //shft2in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xff&rcvbuf[2]));
  infpga_shift0=shft2in;
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  //  in_stat_decode(code);
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
*/


unsigned long int emu::fed::DDU::infpgastat(enum DEVTYPE dv)
     //void emu::fed::DDU::infpga_dfpgastat(enum DEVTYPE dv)
{
	char cmd[2];
	char sndbuf[17];
	char rcvbuf[17];
//  printf(" infpgastat \n");

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
	//  printf(" 32-bit INFPGA Status:  %02x%02x/%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
	//errcode=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8)|((0x00ff&rcvbuf[2])<<16)|((0x00ff&rcvbuf[3])<<24));
	//infpga_code1=(0x00ff&rcvbuf[2])|((0x00ff&rcvbuf[3])<<8);
	//infpga_code0=(0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8);
	unsigned long int code = ((((0x00ff&rcvbuf[2])|((0x00ff&rcvbuf[3])<<8))&0x0000ffff)<<16) | (((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff);
	//shft2in=(((0x01&rcvbuf[6])<<15)|((0xff&rcvbuf[5])<<7)|(0xfe&rcvbuf[4])>>1);
	//shft2in=(((0xff&rcvbuf[5])<<8)|(0xff&rcvbuf[4]));
	//infpga_shift0=shft2in;
	
	//printf("   ----> 70-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
	//for(i=0;i<11;i=i+4){
	//  shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
	//  shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
	//  printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
	//  printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
	//  }
	
	//  in_stat_decode(errcode);
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
	
	return code;
}


/*
int emu::fed::DDU::infpga_CheckFiber(enum DEVTYPE dv)
{
int i,j,shft2in;
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
    printf("%1ld",(code>>j)&0x00000001);
    if((j/8)*8==j&&j>0)printf(".");
  }
  infpga_code0=code;
  printf(",  Hex code %04lx\n",code);
  printf("                             FiberErr.FiberOK\n");
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  infpga_shift0=shft2in;
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

	return code;
}
*/

/*
int emu::fed::DDU::infpga_int_CheckFiber(enum DEVTYPE dv)
{
	return emu::fed::DDU::infpga_CheckFiber(dv);
}
*/

/*
void emu::fed::DDU::infpga_DMBsync(enum DEVTYPE dv)
{
int i,j,shft2in;
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
    printf("%1ld",(code>>j)&0x00000001);
    if((j/8)*8==j&&j>0)printf(".");
  }
  printf(",  Hex code %04lx\n",code);
  printf("                             StuckData.L1Amismatch\n");
  infpga_code0=code;
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  infpga_shift0=shft2in;
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
*/

/*
void emu::fed::DDU::infpga_FIFOstatus(enum DEVTYPE dv)
{
int i,j,shft2in;
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
    printf("%1ld",(code>>j)&0x00000001);
    if(j==10)printf(".");
  }
  printf(",  Hex code %04lx\n",code);
  printf("                        FIFOalmostFull.FIFOempty\n");
  infpga_code0=code;
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  infpga_shift0=shft2in;
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
*/

/*
void emu::fed::DDU::infpga_FIFOfull(enum DEVTYPE dv)
{
int i,j,shft2in;
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
    printf("%1ld",(code>>j)&0x00000001);
    if((j/8)*8==j&&j>0)printf(".");
  }
  printf(",  Hex code %04lx\n",code);
  infpga_code0=code;
  shft2in=(((0x0f&rcvbuf[3])<<12)|((0xff&rcvbuf[2])<<4)|((0xf0&rcvbuf[1])>>4));
  infpga_shift0=shft2in;
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
*/

/*
void emu::fed::DDU::infpga_RxErr(enum DEVTYPE dv)
{
int i,j,shft2in;
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
    printf("%1ld",(code>>j)&0x00000001);
    if((j/8)*8==j&&j>0)printf(".");
  }
  printf(",  Hex code %04lx\n",code);
  printf("                   FiberRxError.StartTimeout\n");
  infpga_code0=code;
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  infpga_shift0=shft2in;
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
*/

/*
void emu::fed::DDU::infpga_Timeout(enum DEVTYPE dv)
{
int i,j,shft2in;
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
    printf("%1ld",(code>>j)&0x00000001);
    if((j/8)*8==j&&j>0)printf(".");
  }
  printf(",  Hex code %04lx\n",code);
  printf("                EndBusyTimeout.EndWaitTimeout\n");
  infpga_code0=code;
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  infpga_shift0=shft2in;
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
*/

/*
void emu::fed::DDU::infpga_XmitErr(enum DEVTYPE dv)
{
int i,j,shft2in;
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
    printf("%1ld",(code>>j)&0x00000001);
    if((j/8)*8==j&&j>0)printf(".");
  }
  printf(",  Hex code %04lx\n",code);
  printf("                     DMBerror.XmitError\n");
  infpga_code0=code;
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  infpga_shift0=shft2in;
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
*/

/*
int emu::fed::DDU::infpga_WrMemActive(enum DEVTYPE dv,int ifiber)
{
int i,k,shft2in;
long int code;
  k=dv-8; //InFPGA #0 or 1
  if((ifiber>3)||(ifiber<0))ifiber=0;
  //printf(" infpga_WrMemActive for Fibers %d & %d (DDU In_Ctrl FPGA-%d) \n",((k*8)+(2*ifiber))+1,(k*8)+(2*ifiber),k);
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
  infpga_code0=(rcvbuf[0]&0x001f);
  infpga_code1=code;
  //printf(" InFpga Fiber %d WrMemActive: %02Xh\n",(8*k)+(2*ifiber),(rcvbuf[0]&0x001f) );
  //printf("        Fiber %d WrMemActive: %02lXh\n",(8*k)+(2*ifiber)+1,code);
  shft2in=(((0x03&rcvbuf[3])<<14)|((0xff&rcvbuf[2])<<6)|((0xfc&rcvbuf[1])>>2));
  infpga_shift0=shft2in;
  //printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<3;i=i+4){
    //printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    //printf("\n");
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

  return infpga_code0 | (infpga_code1 << 5);
}
*/

/*
int emu::fed::DDU::infpga_DMBwarn(enum DEVTYPE dv)
// JRG, 16-bit Persistent Register, can include in Monitor Loop
//      Error triggered by any bits true.

{
int i,j,shft2in;
long int code;
  printf(" infpga_DMBwarn (DDU In_Ctrl FPGA) \n");
  cmd[0]=VTX2P20_USR1_L;
  cmd[1]=VTX2P20_USR1_H;
  sndbuf[0]=21;
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
  printf(" DMB Full/Warn [15-0]:  ");
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
  for(j=15;j>=0;j--){
    printf("%1ld",(code>>j)&0x00000001);
    if((j/8)*8==j&&j>0)printf(".");
  }
  printf(",  Hex code %04lx\n",code);
  printf("                        DMBfull.DMBwarn\n");
  infpga_code0=code;
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  infpga_shift0=shft2in;
  int ret=0;
  if((shft2in&0x0000ffff)!=0xFACE){
    ret=1;
    printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
    for(i=0;i<7;i=i+4){
      printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
      printf("      no right-shift needed\n");
    }
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
  return ret;
}
*/


unsigned long int emu::fed::DDU::infpga_MemAvail(enum DEVTYPE dv)
{
	char cmd[2];
	char sndbuf[4];
	char rcvbuf[4];
	
	//printf(" infpga_MemAvail (DDU In_Ctrl FPGA) \n");
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
	//printf(" InFpga FIFO Memory Available [1 & 0]:  ");
	unsigned long int code = ((rcvbuf[0]&0x001f) << 5) | (((0x00e0&rcvbuf[0])>>5)|((0x0003&rcvbuf[1])<<3))&0x0000001f;
	//infpga_code0=(rcvbuf[0]&0x001f);
	//infpga_code1=code;
	//printf("MemCtrl-0 = %d free\n",(rcvbuf[0]&0x001f) );
	//printf("                                        MemCtrl-1 = %ld free\n",code);
	//shft2in=(((0x03&rcvbuf[3])<<14)|((0xff&rcvbuf[2])<<6)|((0xfc&rcvbuf[1])>>2));
	//infpga_shift0=shft2in;
	//printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
	//for(i=0;i<3;i=i+4){
		//printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
		//printf("\n");
	//}
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
	
	return code;
}



unsigned long int emu::fed::DDU::infpga_Min_Mem(enum DEVTYPE dv)
{

	char cmd[2];
	char sndbuf[6];
	char rcvbuf[6];
	
	//printf(" infpga_Mem_Min (DDU In_Ctrl FPGA) \n");
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
	//printf(" InFpga Minimum FIFO Memory Availabile Record [1 & 0]:\n");
	unsigned long int code = (((((0x00e0&rcvbuf[0])>>5)|((0x0003&rcvbuf[1])<<3))&0x0000001f) << 5) | (rcvbuf[0]&0x001f);
	//infpga_code0=(rcvbuf[0]&0x001f);
	//infpga_code1=code;
	//printf("     MemCtrl-0 min = %d free\n",(rcvbuf[0]&0x001f) );
	//printf("     MemCtrl-1 min = %ld free\n",code);
	//shft2in=(((0x03&rcvbuf[3])<<14)|((0xff&rcvbuf[2])<<6)|((0xfc&rcvbuf[1])>>2));
	//infpga_shift0=shft2in;
	//printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
	//	for(i=0;i<3;i=i+4){
		//printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
		//printf("\n");
	//}
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
	
	return code & 0x3ff;
}


/*
void emu::fed::DDU::infpga_LostErr(enum DEVTYPE dv)
{
int i,j,shft2in;
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
    printf("%1ld",(code>>j)&0x00000001);
    if((j/8)*8==j&&j>0)printf(".");
  }
  printf(",  Hex code %04lx\n",code);
  printf("                      LostInEvent.LostInData\n");
  infpga_code0=code;
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  infpga_shift0=shft2in;
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
*/


unsigned long int emu::fed::DDU::infpga_CcodeStat(enum DEVTYPE dv)
// JRG, 16-bits, Uses custom decode routine, can include in Monitor Loop
//      Error triggered by these bits: 15-0
//         bits 13,5 are not persistent
{
	char cmd[2];
	char sndbuf[6];
	char rcvbuf[6];
	
	//printf(" infpga_CcodeStatus (DDU In_Ctrl FPGA) \n");
	cmd[0]=VTX2P20_USR1_L;
	cmd[1]=VTX2P20_USR1_H;
	sndbuf[0]=20;
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
	unsigned long int code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8))&0x0000ffff;
	
	//printf(" DDU C-code Status [15-0]:  ");
	/*
	printf(" InRd0 DDU C-code Status [7-0]:  ");
	code=rcvbuf[0]&0x000000ff;
	printf(" InRd1 DDU C-code Status [7-0]:  ");
	code=rcvbuf[1]&0x000000ff;
	*/
	//for(j=15;j>=0;j--){
		//printf("%1ld",(code>>j)&0x00000001);
		//if((j/8)*8==j&&j>0)printf(".");
	//}
	//printf(",  Hex code %04lx\n",code);
	//printf("                             RdCtrl-1.RdCtrl-0\n");
	//infpga_code0=code;
	//shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
	//infpga_shift0=shft2in;
	//int ret=0;
	//if((shft2in&0x0000ffff)!=0xFACE){
	//ret=1;
		//printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
	//for(i=0;i<7;i=i+4){
		//printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
		//printf("      no right-shift needed\n");
	//}
	//}
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
	
	return code;
}


/*
void emu::fed::DDU::infpga_StatA(enum DEVTYPE dv)
{
int i,j,shft2in;
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
    printf("%1ld",(code>>j)&0x00000001);
    if((j/8)*8==j&&j>0)printf(".");
  }
  printf(",  Hex code %04lx\n",code);
  infpga_code0=code;
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  infpga_shift0=shft2in;
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
*/

/*
void emu::fed::DDU::infpga_StatB(enum DEVTYPE dv)
{
int i,j,shft2in;
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
    printf("%1ld",(code>>j)&0x00000001);
    if((j/8)*8==j&&j>0)printf(".");
  }
  printf(",  Hex code %04lx\n",code);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  infpga_code0=code;
  infpga_shift0=shft2in;
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
*/

/*
void emu::fed::DDU::infpga_StatC(enum DEVTYPE dv)
{
int i,j,shft2in;
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
    printf("%1ld",(code>>j)&0x00000001);
    if((j/8)*8==j&&j>0)printf(".");
  }
  printf(",  Hex code %04lx\n",code);
  shft2in=(((0xff&rcvbuf[3])<<8)|(0xfe&rcvbuf[2]));
  infpga_code0=code;
  infpga_shift0=shft2in;
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
*/

/*
void emu::fed::DDU::infpga_FiberDiagA(enum DEVTYPE dv)
{
int i,shft2in;
long int code;
  printf(" infpga_FiberDiagA (DDU In_Ctrl FPGA-%d) \n",dv-8);
  cmd[0]=VTX2P20_USR1_L;
  cmd[1]=VTX2P20_USR1_H;
  sndbuf[0]=30;
  sndbuf[1]=0;
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
  sndbuf[6]=0x00;
  sndbuf[7]=0x00;
  sndbuf[8]=0x00;
  devdo(dv,14,cmd,56,sndbuf,rcvbuf,1);
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8)|((0x00ff&rcvbuf[2])<<16)|((0x00ff&rcvbuf[3])<<24));
  if(dv!=9){
    printf(" DDU InFPGA Fiber 3-0 Memory Diagnostic code:  0x%08lx\n",code);
    printf("     Input#3-usage=%d,  Input#2-usage=%d,  Input#1-usage=%d,  Input#0-usage=%d\n",(rcvbuf[3]&0x001f),(rcvbuf[2]&0x001f),(rcvbuf[1]&0x001f),(rcvbuf[0]&0x001f));
    printf("     Mem Request Flags (Inputs 3-0): %1d%1d%1d%1d\n",(rcvbuf[3]&0x0020)>>5,(rcvbuf[2]&0x0020)>>5,(rcvbuf[1]&0x0020)>>5,(rcvbuf[0]&0x0020)>>5);
    printf("     New Mem Assigned (Inputs 3-0): %1d%1d%1d%1d\n",(rcvbuf[3]&0x0040)>>6,(rcvbuf[2]&0x0040)>>6,(rcvbuf[1]&0x0040)>>6,(rcvbuf[0]&0x0040)>>6);
    printf("     Write Next Mem (Inputs 3-0): %1d%1d%1d%1d\n",(rcvbuf[3]&0x0080)>>7,(rcvbuf[2]&0x0080)>>7,(rcvbuf[1]&0x0080)>>7,(rcvbuf[0]&0x0080)>>7);
  }
  else{
    printf(" DDU InFPGA Fiber 11-8 Memory Diagnostic code:  0x%08lx\n",code);
    printf("     Input#11-usage=%d,  Input#10-usage=%d,  Input#9-usage=%d,  Input#8-usage=%d\n",(rcvbuf[3]&0x001f),(rcvbuf[2]&0x001f),(rcvbuf[1]&0x001f),(rcvbuf[0]&0x001f));
    printf("     Mem Request Flags (Inputs 11-8): %1d%1d%1d%1d\n",(rcvbuf[3]&0x0020)>>5,(rcvbuf[2]&0x0020)>>5,(rcvbuf[1]&0x0020)>>5,(rcvbuf[0]&0x0020)>>5);
    printf("     New Mem Assigned (Inputs 11-8): %1d%1d%1d%1d\n",(rcvbuf[3]&0x0040)>>6,(rcvbuf[2]&0x0040)>>6,(rcvbuf[1]&0x0040)>>6,(rcvbuf[0]&0x0040)>>6);
    printf("     Write Next Mem (Inputs 11-8): %1d%1d%1d%1d\n",(rcvbuf[3]&0x0080)>>7,(rcvbuf[2]&0x0080)>>7,(rcvbuf[1]&0x0080)>>7,(rcvbuf[0]&0x0080)>>7);
  }

  shft2in=(((0xff&rcvbuf[5])<<8)|(0xfe&rcvbuf[4]));
  fpga_lcode[0]=code;
  infpga_shift0=shft2in;
  if((shft2in&0x0000ffff)!=0xFACE){
    printf("   ----> 56-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
    for(i=0;i<7;i=i+4){
      printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
      printf("      no right-shift needed\n");
    }
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
*/

/*
void emu::fed::DDU::infpga_FiberDiagB(enum DEVTYPE dv)
{
int i,shft2in;
long int code;
  printf(" infpga_FiberDiagB (DDU In_Ctrl FPGA-%d) \n",dv-8);
  cmd[0]=VTX2P20_USR1_L;
  cmd[1]=VTX2P20_USR1_H;
  sndbuf[0]=31;
  sndbuf[1]=0;
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
  sndbuf[6]=0x00;
  sndbuf[7]=0x00;
  sndbuf[8]=0x00;
  devdo(dv,14,cmd,56,sndbuf,rcvbuf,1);
  code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8)|((0x00ff&rcvbuf[2])<<16)|((0x00ff&rcvbuf[3])<<24));
  if(dv!=9){
    printf(" DDU InFPGA Fiber 7-4 Memory Diagnostic code:  0x%08lx\n",code);
    printf("     Input#7-usage=%d,  Input#6-usage=%d,  Input#5-usage=%d,  Input#4-usage=%d\n",(rcvbuf[3]&0x001f),(rcvbuf[2]&0x001f),(rcvbuf[1]&0x001f),(rcvbuf[0]&0x001f));
    printf("     Mem Request Flags (Inputs 7-4): %1d%1d%1d%1d\n",(rcvbuf[3]&0x0020)>>5,(rcvbuf[2]&0x0020)>>5,(rcvbuf[1]&0x0020)>>5,(rcvbuf[0]&0x0020)>>5);
    printf("     New Mem Assigned (Inputs 7-4): %1d%1d%1d%1d\n",(rcvbuf[3]&0x0040)>>6,(rcvbuf[2]&0x0040)>>6,(rcvbuf[1]&0x0040)>>6,(rcvbuf[0]&0x0040)>>6);
    printf("     Write Next Mem (Inputs 7-4): %1d%1d%1d%1d\n",(rcvbuf[3]&0x0080)>>7,(rcvbuf[2]&0x0080)>>7,(rcvbuf[1]&0x0080)>>7,(rcvbuf[0]&0x0080)>>7);
  }
  else{
    code=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8)|((0x00ff&rcvbuf[3])<<24));
    printf(" DDU InFPGA Fiber 14-12 Memory Diagnostic code:  0x%06lx\n",code);
    printf("     Input#14-usage=%d,  Input#13-usage=%d,  Input#12-usage=%d\n",(rcvbuf[3]&0x001f),(rcvbuf[1]&0x001f),(rcvbuf[0]&0x001f));
    printf("     Mem Request Flags (Inputs 14-12): %1d%1d%1d\n",(rcvbuf[3]&0x0020)>>5,(rcvbuf[1]&0x0020)>>5,(rcvbuf[0]&0x0020)>>5);
    printf("     New Mem Assigned (Inputs 14-12): %1d%1d%1d\n",(rcvbuf[3]&0x0040)>>6,(rcvbuf[1]&0x0040)>>6,(rcvbuf[0]&0x0040)>>6);
    printf("     Write Next Mem (Inputs 14-12): %1d%1d%1d\n",(rcvbuf[3]&0x0080)>>7,(rcvbuf[1]&0x0080)>>7,(rcvbuf[0]&0x0080)>>7);
  }

  shft2in=(((0xff&rcvbuf[5])<<8)|(0xfe&rcvbuf[4]));
  fpga_lcode[0]=code;
  infpga_shift0=shft2in;
  if((shft2in&0x0000ffff)!=0xFACE){
    printf("   ----> 56-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
    for(i=0;i<7;i=i+4){
      printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
      printf("      no right-shift needed\n");
    }
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
*/


std::vector<unsigned long int> emu::fed::DDU::infpga_trap(enum DEVTYPE dv)
     // JRG, 192-bits, Uses custom decode routine, skip for now in Monitor Loop
{

	char cmd[2];
	char sndbuf[33];
	char rcvbuf[33];
	
	std::vector<unsigned long int> retVal;

	// long int code;
	//printf(" infpga_trap (DDU In_Ctrl FPGA-%d) \n",dv-8);
	cmd[0]=VTX2P20_USR1_L;
	cmd[1]=VTX2P20_USR1_H;
	sndbuf[0]=25;
	sndbuf[1]=0;
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
	sndbuf[17]=0x00;
	sndbuf[18]=0x00;
	sndbuf[19]=0x00;
	sndbuf[20]=0x00;
	sndbuf[21]=0x00;
	sndbuf[22]=0x00;
	sndbuf[23]=0x00;
	sndbuf[24]=0x00;
	sndbuf[25]=0x00;
	sndbuf[26]=0x00;
	sndbuf[27]=0x00;
	sndbuf[28]=0x00;
	sndbuf[29]=0x00;
	sndbuf[30]=0x00;
	sndbuf[31]=0x00;
	sndbuf[32]=0x00;
	devdo(dv,14,cmd,224,sndbuf,rcvbuf,1);
	
	//printf("  192-bit DDU InFPGA Diagnostic Trap (24 bytes) \n");
	//i=23;
	//printf("                       LFfull MemAvail C-code End-TO \n");
	//printf("      rcv bytes %2d-%2d:   %02x%02x   %02x%02x   %02x%02x   %02x%02x \n",i,i-7,0xff&rcvbuf[i],0xff&rcvbuf[i-1],0xff&rcvbuf[i-2],0xff&rcvbuf[i-3],0xff&rcvbuf[i-4],0xff&rcvbuf[i-5],0xff&rcvbuf[i-6],0xff&rcvbuf[i-7]);
	//i=15;
	//printf("\n                      Start-TO  Nrdy  L1err  DMBwarn \n");
	//printf("      rcv bytes %2d-%2d:   %02x%02x   %02x%02x   %02x%02x   %02x%02x \n",i,i-7,0xff&rcvbuf[i],0xff&rcvbuf[i-1],0xff&rcvbuf[i-2],0xff&rcvbuf[i-3],0xff&rcvbuf[i-4],0xff&rcvbuf[i-5],0xff&rcvbuf[i-6],0xff&rcvbuf[i-7]);
	//i=7;
	//printf("\n                       32-bit-Empty0M 32-bit-status \n");
	//printf("      rcv bytes %2d-%2d:   %02x%02x   %02x%02x   %02x%02x   %02x%02x \n\n",i,i-7,0xff&rcvbuf[i],0xff&rcvbuf[i-1],0xff&rcvbuf[i-2],0xff&rcvbuf[i-3],0xff&rcvbuf[i-4],0xff&rcvbuf[i-5],0xff&rcvbuf[i-6],0xff&rcvbuf[i-7]);
	retVal.push_back((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8)|((0x00ff&rcvbuf[2])<<16)|((0x00ff&rcvbuf[3])<<24));
	retVal.push_back((0x00ff&rcvbuf[4])|((0x00ff&rcvbuf[5])<<8)|((0x00ff&rcvbuf[6])<<16)|((0x00ff&rcvbuf[7])<<24));
	retVal.push_back((0x00ff&rcvbuf[8])|((0x00ff&rcvbuf[9])<<8)|((0x00ff&rcvbuf[10])<<16)|((0x00ff&rcvbuf[11])<<24));
	retVal.push_back((0x00ff&rcvbuf[12])|((0x00ff&rcvbuf[13])<<8)|((0x00ff&rcvbuf[14])<<16)|((0x00ff&rcvbuf[15])<<24));
	retVal.push_back((0x00ff&rcvbuf[16])|((0x00ff&rcvbuf[17])<<8)|((0x00ff&rcvbuf[18])<<16)|((0x00ff&rcvbuf[19])<<24));
	retVal.push_back((0x00ff&rcvbuf[20])|((0x00ff&rcvbuf[21])<<8)|((0x00ff&rcvbuf[22])<<16)|((0x00ff&rcvbuf[23])<<24));
	//shft2in=(((0xff&rcvbuf[25])<<8)|(0xff&rcvbuf[24]));
	//infpga_shift0=shft2in;
	//int ret=0;
	//if((shft2in&0x0000ffff)!=0xFACE){
	//ret=1;
		//printf("   ----> 224-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
	//for(i=0;i<27;i=i+4){
	//shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
	//shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
		//printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
		//printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
	//}
	//}
	//printf("   32-bit DDU InFPGA Status:  %02x%02x/%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
	//  in_stat_decode(errcode);
	cmd[0]=VTX2P_BYPASS_L;
	cmd[1]=VTX2P_BYPASS_H;
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
	
	return retVal;
}


/*
unsigned long int emu::fed::DDU::inprom_idcode1()
{
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
      //   printf(" The INPROM-1 Chip ID CODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
      cmd[0]=PROM_BYPASS;
      sndbuf[0]=0;
      devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
      unsigned long int ibrd=0x00000000;
      ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24)|ibrd;
      return ibrd;
}


unsigned long int emu::fed::DDU::inprom_idcode0()
{
enum DEVTYPE dv;
printf(" inprom_idcode entered \n");
 usleep(1);
// cmd[0]=PROM_BYPASS;
//     sndbuf[0]=0;
//      devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
      dv=INPROM0;
      cmd[0]=PROM_IDCODE;
      sndbuf[0]=0xFF;
      sndbuf[1]=0xFF;
      sndbuf[2]=0xFF;
      sndbuf[3]=0xFF;
      sndbuf[4]=0xFF;
      devdo(dv,8,cmd,32,sndbuf,rcvbuf,1);
      usleep(1);
      printf(" The INPROM-0 Chip ID CODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
      cmd[0]=PROM_BYPASS;
      sndbuf[0]=0;
      devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
      unsigned long int ibrd=0x00000000;
      ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24)|ibrd;
      printf(" leaving inprom idcode 0 \n");
      return ibrd;
}
*/

/*
unsigned long int emu::fed::DDU::vmeprom_idcode()
{
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


unsigned long int emu::fed::DDU::dduprom_idcode1()
{
enum DEVTYPE dv;
// printf(" dduprom_idcode entered \n");
      dv=DDUPROM1;
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
*/

/*
unsigned long int emu::fed::DDU::dduprom_idcode0()
{
enum DEVTYPE dv;
// printf(" dduprom_idcode entered \n");
      dv=DDUPROM0;
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
*/

/*
unsigned long int  emu::fed::DDU::inprom_usercode1()
{
enum DEVTYPE dv;
printf(" inprom_usercode entered \n");
      dv=INPROM1;

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
      printf(" The INPROM-1 Chip USER CODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
      cmd[0]=PROM_BYPASS;
      sndbuf[0]=0;
      devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
      unsigned long int ibrd=0x00000000;
      ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24)|ibrd;
      printf(" leaving \n");
      return ibrd;
}
*/

/*
unsigned long int emu::fed::DDU::inprom_usercode0()
{
enum DEVTYPE dv;
printf(" entering inprom usercode 1 \n");
      dv=INPROM0;
      cmd[0]=PROM_BYPASS;
      sndbuf[0]=0;
      devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);

      cmd[0]=PROM_USERCODE;
      sndbuf[0]=0xFF;
      sndbuf[1]=0xFF;
      sndbuf[2]=0xFF;
      sndbuf[3]=0xFF;
      sndbuf[4]=0xFF;
      devdo(dv,8,cmd,32,sndbuf,rcvbuf,1);
      printf(" The INPROM-0 Chip USER CODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
      cmd[0]=PROM_BYPASS;
      sndbuf[0]=0;
      devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
      unsigned long int ibrd=0x00000000;
      ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24)|ibrd;
      printf(" leaving \n");
      return ibrd;
}
*/

/*
unsigned long int  emu::fed::DDU::vmeprom_usercode()
{
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
*/

/*
unsigned long int  emu::fed::DDU::dduprom_usercode1()
{
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
*/

/*
unsigned long int  emu::fed::DDU::dduprom_usercode0()
{
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
      return ibrd;
}
*/

/*
unsigned short int  emu::fed::DDU::vmepara_busy()
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
  return((rcvbuf[1]&0xff)<<8)|(rcvbuf[0]&0xff);
}
*/

/*
unsigned short int  emu::fed::DDU::vmepara_fullwarn()
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
  return((rcvbuf[1]&0xff)<<8)|(rcvbuf[0]&0xff);
}
*/

/*
unsigned short int  emu::fed::DDU::vmepara_lostsync()
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
  return((rcvbuf[1]&0xff)<<8)|(rcvbuf[0]&0xff);
}
*/

/*
unsigned short int  emu::fed::DDU::vmepara_error()
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
  return((rcvbuf[1]&0xff)<<8)|(rcvbuf[0]&0xff);
}
*/

/*
unsigned short int emu::fed::DDU::vmepara_CSCstat()
// JRG, 16-bit Persistent Register, can include in Monitor Loop
//      Error triggered by any bits true.
{
  cmd[0]=0x04;
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
  //  printf("Read CSC status summary for FMM: %02x%02x\n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
  return((rcvbuf[1]&0xff)<<8)|(rcvbuf[0]&0xff);
}
*/

/*
unsigned short int  emu::fed::DDU::vmepara_switch()
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
  rcvbuf[1]=0x00;
  return((rcvbuf[1]&0xff)<<8)|(rcvbuf[0]&0xff);
}
*/

/*
unsigned short int  emu::fed::DDU::vmepara_status()
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
  //printf("Read DDU status1: %02x%02x\n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
  unsigned short int status=((rcvbuf[1]<<8)&0xff00)|(rcvbuf[0]&0x00ff);
  return status;
}
*/

/*
unsigned short int emu::fed::DDU::vmepara_rd_inreg0()
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
  return((rcvbuf[1]&0xff)<<8)|(rcvbuf[0]&0xff);
}
*/

/*
unsigned short int emu::fed::DDU::vmepara_rd_inreg1()
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
  return((rcvbuf[1]&0xff)<<8)|(rcvbuf[0]&0xff);
}
*/

/*
unsigned short int emu::fed::DDU::vmepara_rd_inreg2()
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
  return((rcvbuf[1]&0xff)<<8)|(rcvbuf[0]&0xff);
}
*/

/*
void emu::fed::DDU::vmepara_wr_inreg(unsigned short int par_val)
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
// irdsnd for jtag
//        irdsnd = 0 send immediately, no read
//        irdsnd = 1 send immediately, read
//        irdsnd = 2 send in buffer, no read
//

  printf("Wrote to DDU VMEser InReg0: %02x%02x \n",sndbuf[1]&0xff,sndbuf[0]&0xff);
}
*/

/*
void  emu::fed::DDU::vmepara_wr_fmmreg(unsigned short int par_val)
// JRG, expert use only
{

  cmd[0]=0x09; //dev 0x09 is Special Controls register
  cmd[1]=0x8F; //cmd 0x0F is 16-bit FMM reg; set MSB for Write
  sndbuf[7]=0xBE;
  sndbuf[6]=0xAD;
  sndbuf[5]=0x13;
  sndbuf[4]=0x79;
  sndbuf[3]=0x24;
  sndbuf[2]=0x68;
//  sndbuf[1]=0xF0;  // Reg. must contain 0xF0EX to override free-run state!
//  sndbuf[0]=0xE0;  //             Then "X" will be the new state.

// Note: set to 0xFEDX to set the "FMM Error Disable" feature
//         -- BUSY and WARNING are Not Affected by this, just Error and LostSync.
//         -- X<8  fully Disables FMM Error & LostSync reporting
//         -- X>=8 will either
//              1) Disable FMM Error & LostSync reporting
//              2) If already Disabled, toggle to ENABLED for ~200ns, then Disable again.
//         -- This feature is designed to be Broadcast to all DDUs in the Crate.
//         -- Remove the 0xFEDX to Enable Free-running FMM reporting (e.g. 0x0000)
//              1) the default on HardReset is 0x0000 (Fully Enabled FMM reporting).

  rcvbuf[0]=0;
  rcvbuf[1]=0;
  rcvbuf[2]=0;
  rcvbuf[3]=0;
  //  sndbuf[0]=(sndbuf[0]|(par_val&0x000f));
  sndbuf[1]=(par_val>>8)&0x00ff;
  sndbuf[0]=(par_val&0x00ff);
  //
  devdo(VMEPARA,1,  cmd,  0,  sndbuf, rcvbuf,  2);
  //    dev,  ncmd, cmd, nbuf, inbuf, outbuf, irdsnd
// irdsnd for jtag
//        irdsnd = 0 send immediately, no read
//        irdsnd = 1 send immediately, read
//        irdsnd = 2 send in buffer, no read
//

  //printf("Wrote to DDU VME FMM Reg: %02x%02x \n",sndbuf[1]&0xff,sndbuf[0]&0xff);
}
*/

/*
unsigned short int  emu::fed::DDU::vmepara_rd_fmmreg()
// JRG, expert use only
{
  cmd[0]=0x09; //dev 0x09 is Special Controls register
  cmd[1]=0x0F; //cmd 0x0F is 16-bit FMM reg; set MSB for Write
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  rcvbuf[0]=0;
  rcvbuf[1]=0;
  rcvbuf[2]=0;
  rcvbuf[3]=0;
  devdo(VMEPARA,1,cmd,0,sndbuf,rcvbuf,2);
  //printf("Read DDU FMM Reg: %02x%02x\n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
  return((rcvbuf[1]&0xff)<<8)|(rcvbuf[0]&0xff);
}
*/

/*
void emu::fed::DDU::vmepara_wr_fakel1reg(unsigned short int par_val)
// JRG, expert use only
{
  cmd[0]=0x09; //dev 0x09 is Special Controls register
  cmd[1]=0x85; //cmd 0x05 is 16-bit Fake L1A reg; set MSB for Write
  sndbuf[7]=0xBE;
  sndbuf[6]=0xAD;
  sndbuf[5]=0x13;
  sndbuf[4]=0x79;
  sndbuf[3]=0x24;
  sndbuf[2]=0x68;
  sndbuf[1]=0xF0;
  sndbuf[0]=0xE0;
  rcvbuf[0]=0;
  rcvbuf[1]=0;
  rcvbuf[2]=0;
  rcvbuf[3]=0;
// pk070805:  !(par_val&0x0007) = 0.  I think you wanted
//            0x000F-(par_val&0x0007)
//  sndbuf[0]=(((!(par_val&0x0007))<<4)|(par_val&0x0007));
  sndbuf[0]=(((0x000F-(par_val&0x0007))<<4)|(par_val&0x0007));
  sndbuf[1]=sndbuf[0];
  //
  devdo(VMEPARA,1,  cmd,  0,  sndbuf, rcvbuf,  2);
  //    dev,  ncmd, cmd, nbuf, inbuf, outbuf, irdsnd
// irdsnd for jtag
//        irdsnd = 0 send immediately, no read
//        irdsnd = 1 send immediately, read
//        irdsnd = 2 send in buffer, no read
//

  printf("Wrote to DDU VME Fake L1 Reg: %02x%02x \n",sndbuf[1]&0xff,sndbuf[0]&0xff);
}
*/

/*
unsigned short int  emu::fed::DDU::vmepara_rd_fakel1reg()
// JRG, 16-bits, maybe read once at run/Loop start, otherwise expert use only
//      Error not triggered here!
{
  cmd[0]=0x09; //dev 0x09 is Special Controls register
  cmd[1]=0x05; //cmd 0x05 is 16-bit Fake L1A reg; set MSB for Write
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  rcvbuf[0]=0;
  rcvbuf[1]=0;
  rcvbuf[2]=0;
  rcvbuf[3]=0;
  devdo(VMEPARA,1,cmd,0,sndbuf,rcvbuf,2);
  printf("Read DDU Fake L1 Reg: %02x%02x\n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
  return((rcvbuf[1]&0xff)<<8)|(rcvbuf[0]&0xff);
}
*/

/*
void emu::fed::DDU::vmepara_wr_GbEprescale(unsigned short int par_val)
// JRG, expert use only
{
	std::cout << "vmepara_wr_GbEprescale " << par_val << std::endl;
	cmd[0]=0x09; //dev 0x09 is Special Controls register
	cmd[1]=0x80; //cmd 0x00 is 16-bit GbE Prescale & SLink/DCC Wait Disable reg
				// set MSB for Write
	sndbuf[7]=0xBE;
	sndbuf[6]=0xAD;
	sndbuf[5]=0x13;
	sndbuf[4]=0x79;
	sndbuf[3]=0x24;
	sndbuf[2]=0x68;
	sndbuf[1]=0xF0;
	sndbuf[0]=0xE0;
	rcvbuf[0]=0;
	rcvbuf[1]=0;
	rcvbuf[2]=0;
	rcvbuf[3]=0;
	// pk070805:  !(par_val&0x000f) = 0.  I think you wanted
	//            0x000F-(par_val&0x000f)
	//  sndbuf[0]=(((!(par_val&0x000f))<<4)|(par_val&0x000f));
	sndbuf[0]=(((0x000F-(par_val&0x000f)<<4))|(par_val&0x000f));
	sndbuf[1]=sndbuf[0];
	//
	devdo(VMEPARA,1,  cmd,  0,  sndbuf, rcvbuf,  2);
	//    dev,  ncmd, cmd, nbuf, inbuf, outbuf, irdsnd
// irdsnd for jtag
//		irdsnd = 0 send immediately, no read
//		irdsnd = 1 send immediately, read
//		irdsnd = 2 send in buffer, no read
	
	printf("Wrote to DDU GbE Prescale/SLink_Wait Reg: %02x%02x \n",sndbuf[1]&0xff,sndbuf[0]&0xff);
}
*/

/*
unsigned short int  emu::fed::DDU::vmepara_rd_GbEprescale()
// JRG, 16-bits, maybe read once at run/Loop start, otherwise expert use only
//      Error not triggered here!
{
  cmd[0]=0x09; //dev 0x09 is Special Controls register
  cmd[1]=0x00; //cmd 0x00 is 16-bit GbE Prescale & SLink/DCC Wait Disable reg
               // set MSB for Write
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  rcvbuf[0]=0;
  rcvbuf[1]=0;
  rcvbuf[2]=0;
  rcvbuf[3]=0;
  devdo(VMEPARA,1,cmd,0,sndbuf,rcvbuf,2);
  printf("Read DDU GbE Prescale/SLink_Wait Reg: %02x%02x\n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
  return((rcvbuf[1]&0xff)<<8)|(rcvbuf[0]&0xff);
}
*/

/*
unsigned short int emu::fed::DDU::vmepara_rd_testreg0()
// JRG, expert use only
{
  cmd[0]=0x08; //dev 0x08 is related to serial-input register
  cmd[1]=0x03; //cmd 0x3 is 16-bit Read Test Reg 0, EvCntRst
               // Written with Wr_Inreg0 using  vmepara_wr_inreg()
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  rcvbuf[0]=0;
  rcvbuf[1]=0;
  rcvbuf[2]=0;
  rcvbuf[3]=0;
  devdo(VMEPARA,1,cmd,0,sndbuf,rcvbuf,2);
  printf("Read DDU Test Reg 0: %02x%02x\n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
  return((rcvbuf[1]&0xff)<<8)|(rcvbuf[0]&0xff);
}
*/

/*
unsigned short int emu::fed::DDU::vmepara_rd_testreg1()
// JRG, expert use only
{
  cmd[0]=0x08; //dev 0x08 is related to serial-input register
  cmd[1]=0x04; //cmd 0x4 is 16-bit Read Test Reg 1, BC0
               // Written with Wr_Inreg0 using  vmepara_wr_inreg()
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  rcvbuf[0]=0;
  rcvbuf[1]=0;
  rcvbuf[2]=0;
  rcvbuf[3]=0;
  devdo(VMEPARA,1,cmd,0,sndbuf,rcvbuf,2);
  printf("Read DDU Test Reg 1: %02x%02x\n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
  return((rcvbuf[1]&0xff)<<8)|(rcvbuf[0]&0xff);
}
*/

/*
unsigned short int  emu::fed::DDU::vmepara_rd_testreg2()
// JRG, expert use only
{
  cmd[0]=0x08; //dev 0x08 is related to serial-input register
  cmd[1]=0x05; //cmd 0x5 is 16-bit Read Test Reg 2, SyncRst
               // Written with Wr_Inreg0 using  vmepara_wr_inreg()
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  rcvbuf[0]=0;
  rcvbuf[1]=0;
  rcvbuf[2]=0;
  rcvbuf[3]=0;
  devdo(VMEPARA,1,cmd,0,sndbuf,rcvbuf,2);
  printf("Read DDU Test Reg 2: %02x%02x\n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
  return((rcvbuf[1]&0xff)<<8)|(rcvbuf[0]&0xff);
}
*/

/*
unsigned short int emu::fed::DDU::vmepara_rd_testreg3()
// JRG, expert use only
{
  cmd[0]=0x08; //dev 0x08 is related to serial-input register
  cmd[1]=0x06; //cmd 0x6 is 16-bit Read Test Reg 3, SoftRst
               // Written with Wr_Inreg0 using  vmepara_wr_inreg()
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  rcvbuf[0]=0;
  rcvbuf[1]=0;
  rcvbuf[2]=0;
  rcvbuf[3]=0;
  devdo(VMEPARA,1,cmd,0,sndbuf,rcvbuf,2);
  printf("Read DDU Test Reg 3: %02x%02x\n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
  return((rcvbuf[1]&0xff)<<8)|(rcvbuf[0]&0xff);
}
*/

/*
unsigned short int emu::fed::DDU::vmepara_rd_testreg4()
// JRG, expert use only
{
  cmd[0]=0x08; //dev 0x08 is related to serial-input register
  cmd[1]=0x07; //cmd 0x7 is 16-bit Read Test Reg 4, HardRst
               // Written with Wr_Inreg0 using  vmepara_wr_inreg()
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  rcvbuf[0]=0;
  rcvbuf[1]=0;
  rcvbuf[2]=0;
  rcvbuf[3]=0;
  devdo(VMEPARA,1,cmd,0,sndbuf,rcvbuf,2);
  printf("Read DDU Test Reg 4: %02x%02x\n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
  return((rcvbuf[1]&0xff)<<8)|(rcvbuf[0]&0xff);
}
*/

/*
unsigned short int emu::fed::DDU::vmepara_warnhist()
// JRG, 16-bit Persistent Register, can include in Monitor Loop
//      Error triggered by any bit changes...really Warning state flags
//      Historical FMM-WARN flags
{
  cmd[0]=0x05;
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
  printf("Read FMM Warn History: %02x%02x\n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
  return((rcvbuf[1]&0xff)<<8)|(rcvbuf[0]&0xff);
}
*/

/*
unsigned short int emu::fed::DDU::vmepara_busyhist()
// JRG, 16-bit Persistent Register, can include in Monitor Loop
//      Error triggered by any bits changes.
//      Historical FMM-BUSY flags
{
  cmd[0]=0x06;
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
  printf("Read FMM Busy History: %02x%02x\n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
  return((rcvbuf[1]&0xff)<<8)|(rcvbuf[0]&0xff);
}
*/

// all serial DEVDOs were ...,0) for Reads, ...,1) for Writes
/*
int emu::fed::DDU::read_status()
{
  cmd[0]=0x04; //dev 0x04 is flash sram
  cmd[1]=0x00; // cmd 0x00 is read flash status, 8 bits
  sndbuf[0]=0xf1; // low data byte
  sndbuf[1]=0xf2; // high data byte
  devdo(VMESERI,2,cmd,0,sndbuf,(char *)rcv_serial,0);
  //printf("Read from Flash Status Register: ");
  //printf(" %02x\n",rcv_serial[1]&0xff);
  return rcv_serial[1]&0xff;
}
*/

/*
int emu::fed::DDU::read_int_page1()
{
	return read_page1();
}
*/


int emu::fed::DDU::read_page1()
{
	char cmd[2];
	char sndbuf[2];
	char rcvbuf[2];
	
	cmd[0]=0x04; //dev 0x04 is flash sram
	cmd[1]=0x01; //read 16 bits from page 1
	sndbuf[0]=0xff; // low data byte
	sndbuf[1]=0xaa; // high data byte
	devdo(VMESERI,2,cmd,0,sndbuf,rcvbuf,0);
	//printf("Read from Flash Memory Page1 (Kill Channel Mask): ");
	//for(i=0;i<2;i++)printf("%02x",rcv_serial[i]&0xff);
	//printf("\n");
	
	return (((rcvbuf[0]&0x00ff)<<8)|(rcvbuf[1]&0x00ff));
}


/*
void emu::fed::DDU::write_page1()
{
int i;
  cmd[0]=0x04; //dev 0x04 is flash sram
  cmd[1]=0x09; // cmd 0x09 is program page 1, 16 bits
  sndbuf[0]=snd_serial[1]; // low data byte, Kill only channel 16 (D.N.E.)
  sndbuf[1]=snd_serial[0]; // high data byte
  printf("Programming Flash Memory Page1 (Kill Channel Mask):  0x");
  for(i=0;i<2;i++)printf("%02x",sndbuf[1-i]&0xff);
  printf("\n");
// JG, sndbuf is ignored here, uses InReg0 instead:
  devdo(VMESERI,2,cmd,0,sndbuf,rcvbuf,0);
  usleep(20000);
}
*/

/*
int emu::fed::DDU::read_page3()
{
int i;
  cmd[0]=0x04; //dev 0x04 is flash sram
  cmd[1]=0x03; //read 16 bits from page 3
  sndbuf[0]=0xee; // low data byte
  sndbuf[1]=0xaa; // high data byte
  devdo(VMESERI,2,cmd,0,sndbuf,rcv_serial,0);
  printf("Read from Flash Memory Page3 (DDU RUI): ");
  for(i=0;i<2;i++)printf("%02x",rcv_serial[i]&0xff);
  printf("\n");
  int brdnum=((rcv_serial[0]<<8)&0xff00)|(rcv_serial[1]&0x00ff);
  printf(" brdnum %d \n",brdnum);
  return brdnum;
}
*/

/*
void emu::fed::DDU::write_page3()
{
int i;
  cmd[0]=0x04; //dev 0x04 is flash sram
  cmd[1]=0x0b; // cmd 0x0b is program page 3, 16 bits
  sndbuf[0]=0xde; // low data byte
  sndbuf[1]=0xfa; // high data byte
  printf("Programming Flash Memory Page3 (DDU RUI):  0x");
  for(i=0;i<2;i++)printf("%02x",snd_serial[i]&0xff);
  printf("\n");
// JG, sndbuf is ignored here, uses InReg0 instead:
  devdo(VMESERI,2,cmd,0,sndbuf,rcv_serial,1);
  usleep(20000);
}
*/

/*
void emu::fed::DDU::read_page4()
{
unsigned int code[3];
  cmd[0]=0x04; //dev 0x04 is flash sram
  cmd[1]=0x04; //read 32 bits from page 4
  sndbuf[0]=0xee; // low data byte
  sndbuf[1]=0x66;
  sndbuf[2]=0xff;
  sndbuf[3]=0x88; // high data byte
  devdo(VMESERI,2,cmd,0,sndbuf,rcv_serial,0);
  printf("Read from Flash Memory Page4 (DDR FIFO Thresholds, 32 bits): ");
  code[0]=((rcv_serial[1]&0xFF))|((rcv_serial[0]&0xFF)<<8);
  code[1]=((rcv_serial[3]&0xFF))|((rcv_serial[2]&0xFF)<<8);
  printf("%04x/%04x\n",code[1],code[0]);
  //  printf("Read from Flash Memory Page4 (DDR Input FIFO Thresholds): ");
  //  for(i=0;i<4;i++)printf("%02x",rcvbuf[i]&0xff);
  //  printf("\n");
}
*/

/*
void emu::fed::DDU::write_page4()
{
int i;
  cmd[0]=0x04; //dev 0x04 is flash sram
  cmd[1]=0x0c; // cmd 0x0c is program page 4, 32 bits
  sndbuf[0]=snd_serial[3]; // low data byte
  sndbuf[1]=snd_serial[2]; //  PAE: m=255
  sndbuf[2]=snd_serial[1]; //  PAF: n=16
  sndbuf[3]=snd_serial[0]; // high data byte
  printf("Programming Flash Memory Page4 (DDR Input FIFO Thresholds):  0x");
  for(i=0;i<4;i++)printf("%02x",snd_serial[3-i]&0xff);
  printf("\n");
// JG, sndbuf is ignored here, uses InReg0 instead:
  devdo(VMESERI,2,cmd,0,sndbuf,rcvbuf,1);
  usleep(20000);
}
*/


std::vector<unsigned long int> emu::fed::DDU::read_page5()
{
	char cmd[2];
	char sndbuf[6];
	char rcvbuf[6];
	
	std::vector<unsigned long int> retVal;

	//unsigned int code[3];
	cmd[0]=0x04; //dev 0x04 is flash sram
	cmd[1]=0x05; //read 34 bits from page 5
	sndbuf[0]=0xee; // low data byte
	sndbuf[1]=0x66;
	sndbuf[2]=0xff;
	sndbuf[3]=0x88;
	sndbuf[4]=0xaa; // high data byte
	sndbuf[5]=0x68;
	devdo(VMESERI,2,cmd,0,sndbuf,rcvbuf,0);
	//printf("Read from Flash Memory Page5 (GBE FIFO Thresholds, 34 bits): ");
	for (int i=0; i<6; i++) {
		retVal.push_back(rcvbuf[i]);
	}
	//code[0]=(((rcv_serial[0]&0xC0)>>6)|((rcv_serial[3]&0xFF)<<2)|((rcv_serial[2]&0x3F)<<10));
	//code[1]=(((rcv_serial[2]&0xC0)>>6)|((rcv_serial[5]&0xFF)<<2)|((rcv_serial[4]&0x3F)<<10));
	//code[2]=((rcv_serial[4]&0xC0)>>6);
	//printf("%01x/%04x/%04x\n",code[2],code[1],code[0]);

	return retVal;
}


/*
void emu::fed::DDU::write_page5()
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
  sndbuf[0]=snd_serial[4]; // low data byte
  sndbuf[1]=snd_serial[3];
  sndbuf[2]=snd_serial[2];
  sndbuf[3]=snd_serial[1];
  sndbuf[4]=snd_serial[0]; // high data byte
  printf("Programming Flash Memory Page5 (GBE FIFO Thresholds):  0x");
  for(i=0;i<4;i++)printf("%02x",sndbuf[4-i]&0xff);
  printf("%01X\n",(sndbuf[0]&0x0f));
// JG, sndbuf is ignored here, uses InReg0 instead:
  devdo(VMESERI,2,cmd,0,sndbuf,rcv_serial,1);
  usleep(20000);
}
*/

/*
int emu::fed::DDU::read_page7()
{
int i;
  cmd[0]=0x04; //dev 0x04 is flash sram
  cmd[1]=0x07; //read 16 bits from page 7
  sndbuf[0]=0xee; // low data byte
  sndbuf[1]=0xaa; // high data byte
  devdo(VMESERI,2,cmd,0,sndbuf,rcv_serial,0);
  printf("Read from Flash Memory Page7 (DDU Board ID): ");
  for(i=0;i<2;i++)printf("%02x",rcv_serial[i]&0xff);
  printf("\n");
  int sourceID=((rcv_serial[0]<<8)&0xff00)|(rcv_serial[1]&0x00ff);
  printf(" sourceID %d \n",sourceID);
  return sourceID;
}
*/

/*
void emu::fed::DDU::write_page7()
{
int i;
  cmd[0]=0x04; //dev 0x04 is flash sram
  cmd[1]=0x0f; // cmd 0x0f is program page 7, 16 bits
  sndbuf[0]=0xde; // low data byte
  sndbuf[1]=0xfa; // high data byte
  printf("Programming Flash Memory Page7 (DDU Board ID):  0x");
  for(i=0;i<2;i++)printf("%02x",snd_serial[i]&0xff);
  printf("\n");
// JG, sndbuf is ignored here, uses InReg0 instead:
  devdo(VMESERI,2,cmd,0,sndbuf,rcv_serial,1);
  usleep(20000);
// Worked for Dynatem/VCC, but not Caen.  Used usleep() instead:
// JRG, add delay after Serial Write, 10-20ms required!
//   --from "pause" below....
//for(i=0;i<11;i++){
//  sndbuf[0]=254;
//  sndbuf[1]=254;
//  printf(" pausing for %d usec \n",( ((sndbuf[1]&0x00ff)<<8)|(sndbuf[0]&0x00ff) ) );
//  devdo(VMESERI,-99,sndbuf,0,sndbuf,rcvbuf,0);
//}


}
*/

/*
void emu::fed::DDU::read_vmesd0()
{

unsigned int code[3];
  cmd[0]=0x00; //dev 0x00 is "Read VME Serial Device 0" 32 bits
  cmd[1]=0x00; //ignore this byte
  sndbuf[0]=0xee; // low data byte
  sndbuf[1]=0xaa;
  sndbuf[2]=0xff;
  sndbuf[3]=0xcc; // high data byte
  devdo(VMESERI,2,cmd,0,sndbuf,rcv_serial,0);
  printf("Read Thresholds from DDR Input FIFO 0: ");
  code[0]=((rcv_serial[1]&0xFF))|((rcv_serial[0]&0xFF)<<8);
  code[1]=((rcv_serial[3]&0xFF))|((rcv_serial[2]&0xFF)<<8);
  printf("%04x/%04x\n",code[1],code[0]);
  //  for(i=0;i<4;i++)printf("%02x",rcv_serial[3-i]&0xff);
  //  printf("\n");
}
*/

/*
void emu::fed::DDU::read_vmesd1()
{

unsigned int code[3];
  cmd[0]=0x01; //dev 0x01 is "Read VME Serial Device 1" 32 bits
  cmd[1]=0x00; //ignore this byte
  sndbuf[0]=0xee; // low data byte
  sndbuf[1]=0xaa;
  sndbuf[2]=0xff;
  sndbuf[3]=0xcc; // high data byte
  devdo(VMESERI,2,cmd,0,sndbuf,rcv_serial,0);
  printf("Read Thresholds from DDR Input FIFO 1: ");
  code[0]=((rcv_serial[1]&0xFF))|((rcv_serial[0]&0xFF)<<8);
  code[1]=((rcv_serial[3]&0xFF))|((rcv_serial[2]&0xFF)<<8);
  printf("%04x/%04x\n",code[1],code[0]);
  //  for(i=0;i<4;i++)printf("%02x",rcv_serial[3-i]&0xff);
  //  printf("\n");
}
*/

/*
void emu::fed::DDU::read_vmesd2()
{

unsigned int code[3];
  cmd[0]=0x02; //dev 0x02 is "Read VME Serial Device 2" 32 bits
  cmd[1]=0x00; //ignore this byte
  sndbuf[0]=0xee; // low data byte
  sndbuf[1]=0xaa;
  sndbuf[2]=0xff;
  sndbuf[3]=0xcc; // high data byte
  devdo(VMESERI,2,cmd,0,sndbuf,rcv_serial,0);
  printf("Read Thresholds from DDR Input FIFO 2: ");
  code[0]=((rcv_serial[1]&0xFF))|((rcv_serial[0]&0xFF)<<8);
  code[1]=((rcv_serial[3]&0xFF))|((rcv_serial[2]&0xFF)<<8);
  printf("%04x/%04x\n",code[1],code[0]);
  //  for(i=0;i<4;i++)printf("%02x",rcv_serial[3-i]&0xff);
  //  printf("\n");
}
*/

/*
void emu::fed::DDU::read_vmesd3()
{

unsigned int code[3];
  cmd[0]=0x03; //dev 0x03 is "Read VME Serial Device 3" 32 bits
  cmd[1]=0x00; //ignore this byte
  sndbuf[0]=0xee; // low data byte
  sndbuf[1]=0xaa;
  sndbuf[2]=0xff;
  sndbuf[3]=0xcc; // high data byte
  devdo(VMESERI,2,cmd,0,sndbuf,rcv_serial,0);
  printf("Read Thresholds from DDR Input FIFO 3: ");
  code[0]=((rcv_serial[1]&0xFF))|((rcv_serial[0]&0xFF)<<8);
  code[1]=((rcv_serial[3]&0xFF))|((rcv_serial[2]&0xFF)<<8);
  printf("%04x/%04x\n",code[1],code[0]);
  //  for(i=0;i<4;i++)printf("%02x",rcv_serial[3-i]&0xff);
  //  printf("\n");
}
*/

/*
void emu::fed::DDU::write_vmesdF()
{

  cmd[0]=0x0f; //dev 0x0F is all DDR FIFOs
  cmd[1]=0x00; // cmd is ignored for dev>=8

  sndbuf[0]=0xad; // low data byte, ignored
  sndbuf[1]=0xbe; // high data byte , ignored
  printf("Write to dev F:  force load of all DDR FIFOs from Flash Page 4\n");
  devdo(VMESERI,2,cmd,0,sndbuf,rcv_serial,1);
}
*/


float emu::fed::DDU::adcplus(int ichp,int ichn){
	//printf(" inside adcplus %d %d \n",ichp,ichn);
	unsigned int ival= readADC(ichp, ichn);
	if(ival < 1410 || ival > 3440){
	// JRG, try a re-read in case of a "bad read"
		//printf("  **read adc out-of-range, retry: %d (%04xh) ",ival,ival);
		ival = readADC(ichp, ichn);
		//printf("---> %d (%04xh)\n",ival,ival);
	}
	return (float) ival;
}



float emu::fed::DDU::adcminus(int ichp,int ichn){
	unsigned int ival= readADC(ichp, ichn);
	if((0x0800&ival)==0x0800)ival=ival|0xf000;
	return (float) ival;
}



float emu::fed::DDU::readthermx(int it)
{
	float cval,fval;
	float Vout= (float) readADC(1, it) / 1000.;
	
	// JRG, try a re-read in case of a "bad read"
	if(Vout < 0.5 || Vout > 1.9){
		//printf("  **read temp Vout out-of-range, retry: %f ",Vout);
		Vout= (float) readADC(1, it) / 1000.;
		//printf("---> %f\n",Vout);
	}

	cval = 1/(0.1049406423E-2+0.2133635468E-3*log(65000.0/Vout-13000.0)+0.7522287E-7*pow(log(65000.0/Vout-13000.0),3.0))-0.27315E3;
	fval=9.0/5.0*cval+32.;
	return fval;
}



unsigned int emu::fed::DDU::readADC(int ireg, int ichn) {
	char cmd[2];
	char sndbuf[2];
	char rcvbuf[2];
	cmd[0]=ireg;
	cmd[1]=ichn;
	
	devdo(SADC,16,cmd,0,sndbuf,rcvbuf,2);
	return ((rcvbuf[1]<<8)&0x0f00)|(rcvbuf[0]&0xff);
}


/*
void emu::fed::DDU::read_therm()
{
         printf("\nReading all DDU Temperatures\n");
         printf("Reading Temp 0: %5.2f F \n",readthermx(0));
         printf("Reading Temp 1: %5.2f F \n",readthermx(1));
         printf("Reading Temp 2: %5.2f F \n",readthermx(2));
         printf("Reading Temp 3: %5.2f F \n",readthermx(3));
}
*/

/*
void emu::fed::DDU::read_voltages()
{
         printf("\nReading all DDU Voltages\n");
         printf("Reading V15P: %5.2f V \n",adcplus(1,4));
         printf("Reading V25P: %5.2f V \n",adcplus(1,5));
         printf("Reading V25P Analog:%5.2f V \n",adcplus(1,6));
         printf("Reading V33P: %5.2f V \n",adcplus(1,7));
}
*/

/*
unsigned int emu::fed::DDU::unpack_ival(){
  return ((rcvbuf[1]<<8)&0x0f00)|(rcvbuf[0]&0xff);
}
*/


void emu::fed::DDU::Parse(char *buf,int *Count,char **Word)
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


void emu::fed::DDU::epromload(char *design,enum DEVTYPE devnum,char *downfile,int writ,char *cbrdnum)
{
	epromload(design,devnum,downfile,writ,cbrdnum,4);
}



void emu::fed::DDU::epromload(char *design,enum DEVTYPE devnum,char *downfile,int writ,char *cbrdnum,int ipass)
{
	enum DEVTYPE devstp,dv;
	char *devstr;
	FILE *dwnfp,*fpout;
	char buf[8192],buf2[256];
	char *Word[256],*lastn;

	int Count,i,j,id,nbits,nbytes,pause,xtrbits,looppause;

	int tmp,cmpflag;
	int tstusr;
	int nowrit;
	char snd[5000],expect[5000],rmask[5000],smask[5000],cmpbuf[5000];
	char sndbuf[1024], rcvbuf[1024];
	int intCache;

	/* ipass acts as a hiccup.
	ipass == 1 - load up to the part where you have to load the board number
	ipass == 2 - load only the board number
	ipass == 3 - load only the stuff after the board number
	ipass == 4 - do everything always
	*/
	int pass = 1;

	//printf(" epromload %d \n",devnum);

	devstp=devnum;
	for(id=devnum;id<=devstp;id++){
		//std::cout << "id " << id << std::endl;
		dv=(DEVTYPE)id;
		xtrbits=geo[dv].sxtrbits;
		devstr=geo[dv].nam;
		dwnfp    = fopen(downfile,"r");
		fpout=fopen("eprom.bit","w");
		while (fgets(buf,256,dwnfp) != NULL)  {
			//std::cout << "buf " << buf << std::endl;
			if((buf[0]=='/'&&buf[1]=='/')||buf[0]=='!'){
			} else {
				if(strrchr(buf,';')==0){
					do {
						lastn=strrchr(buf,'\n');
						if(lastn!=0)lastn[0]='\0';
						if (fgets(buf2,256,dwnfp) != NULL){
							strcat(buf,buf2);
						} else {
							return;
						}
					} while (strrchr(buf,';')==0);
				}
				for(i=0;i<1024;i++){
					//std::cout << "i " << i << std::endl;
					cmpbuf[i]=0;
					sndbuf[i]=0;
					rcvbuf[i]=0;
				}
				//std::cout << "Parse! " << buf << std::endl;
				Parse(buf, &Count, &(Word[0]));
				// count=count+1;
				// printf(" count %d \n",count);
				if(strcmp(Word[0],"SDR")==0){
					cmpflag=0;    //disable the comparison for no TDO SDR
					sscanf(Word[1],"%d",&nbits);
					nbytes=(nbits-1)/8+1;
					for(i=2;i<Count;i+=2){
						//std::cout << "Count " << Count << std::endl;

		/* PGK Here is where we load up the board number.
		I have to stop here and only send this stuff if pass==1 */

						if(strcmp(Word[i],"TDI")==0){
							for(j=0;j<nbytes;j++){
								//std::cout << "j " << j << std::endl;
								sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",&intCache);
								snd[j] = intCache;
							}
		/*JRG, new selective way to download UNALTERED PromUserCode from SVF to
			ANY prom:  just set cbrdnum[3,2,1,0]=0 in calling routine!
			was  if(nowrit==1){  */
		//    if(nowrit==1&&(cbrdnum[0]|cbrdnum[1]|cbrdnum[2]|cbrdnum[3])!=0){
							if(nowrit==1&&(cbrdnum[1]|cbrdnum[2]|cbrdnum[3])!=0){
								tstusr=0;
								snd[0]=cbrdnum[0];
								snd[1]=cbrdnum[1];
								snd[2]=cbrdnum[2];
								snd[3]=cbrdnum[3];
		//        printf(" snd %02x %02x %02x %02x \n",snd[0],snd[1],snd[2],snd[3]);
							}
		// JRG, try fix for dduprom case:
							else if(nowrit==1&&(cbrdnum[0]!=0)){
								tstusr=0;
								snd[0]=cbrdnum[0];
								pass++;
							}
						}
						if(strcmp(Word[i],"SMASK")==0){
							for(j=0;j<nbytes;j++){
								sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",&intCache);
								smask[j] = intCache;
							}
						}
						if(strcmp(Word[i],"TDO")==0){
							cmpflag=1;
							for(j=0;j<nbytes;j++){
								sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",&intCache);
								expect[j] = intCache;
							}
						}
						if(strcmp(Word[i],"MASK")==0){
							for(j=0;j<nbytes;j++){
								sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",&intCache);
								rmask[j] = intCache;
							}
						}
					}
					for(i=0;i<nbytes;i++){
						//std::cout << "i " << i << std::endl;
	// 					sndbuf[i]=snd[i]&smask[i];
						sndbuf[i]=snd[i]&0xff;
					}
			//   printf("D%04d",nbits+xtrbits);
					// for(i=0;i<(nbits+xtrbits)/8+1;i++)printf("%02x",sndbuf[i]&0xff);printf("\n");
					if(nowrit==0){
						if((geo[dv].jchan==12)){
								if (pass == ipass || ipass == 4) scan(RESET,DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
								if (pass == 2) pass++;
						}else{
								if (pass == ipass || ipass == 4) scan(NONE,DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
								if (pass == 2) pass++;
						}
					}else{
						if(writ==1){

							if((geo[dv].jchan==12)){
								if (pass == ipass || ipass == 4) scan(RESET,DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
								if (pass == 2) pass++;
							}else{
								if (pass == ipass || ipass == 4) scan(NONE,DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
								if (pass == 2) pass++;
							}
						}
					}

			//  Data readback comparison here:
					for (i=0;i<nbytes;i++) {
						//std::cout << "i " << i << std::endl;
						tmp=(rcvbuf[i]>>3)&0x1F;
						rcvbuf[i]=tmp | (rcvbuf[i+1]<<5&0xE0);
				/*  if (((rcvbuf[i]^expect[i]) & (rmask[i]))!=0 && cmpflag==1)
				printf("read back wrong, at i %02d  rdbk %02X  expect %02X  rmask %02X\n",i,rcvbuf[i]&0xFF,expect[i]&0xFF,rmask[i]&0xFF); */
					}
					if (cmpflag==1) {
						for (i=0;i<nbytes;i++) {
							//std::cout << "i " << i << std::endl;
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
								//std::cout << "j " << j << std::endl;
								sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",&intCache);
								snd[j] = intCache;
							}
							if(nbytes==1){if(0xfd==(snd[0]&0xff))nowrit=1;} // nowrit=1
						}
						else if(strcmp(Word[i],"SMASK")==0){
							for(j=0;j<nbytes;j++){
								//std::cout << "j " << j << std::endl;
								sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",&intCache);
								smask[j] = intCache;
							}
						}
						if(strcmp(Word[i],"TDO")==0){
							for(j=0;j<nbytes;j++){
								//std::cout << "j " << j << std::endl;
								sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",&intCache);
								expect[j] = intCache;
							}
						}
						else if(strcmp(Word[i],"MASK")==0){
							for(j=0;j<nbytes;j++){
								//std::cout << "j " << j << std::endl;
								sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",&intCache);
								rmask[j] = intCache;
							}
						}
					}
					for(i=0;i<nbytes;i++){
						//std::cout << "i " << i << std::endl;
	// 				sndbuf[i]=snd[i]&smask[i];
						sndbuf[i]=snd[i];
					}
			//   printf("I%04d",nbits);
					// for(i=0;i<nbits/8+1;i++)printf("%02x",sndbuf[i]&0xff);printf("\n");
		/*JRG, brute-force way to download UNALTERED PromUserCode from SVF file to
			DDU prom, but screws up CFEB/DMB program method:      nowrit=0;  */
					//std::cout << "nowrit " << nowrit << std::endl;
					//std::cout << "pass " << pass << std::endl;
					//std::cout << "ipass " << ipass << std::endl;
					//std::cout << "dv " << dv << std::endl;
					//std::cout << "nbits " << nbits << std::endl;
					//std::cout << "sndbuf[0] " << sndbuf[0] << std::endl;
					//std::cout << "devdo now..." << std::endl;
					if(nowrit==0){
						if (pass == ipass || ipass == 4) devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);
					} else {
						if(writ==1 && (pass == ipass || ipass == 4))devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);
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
					// fpause=pause;
					// pause=pause/2;
		/*
		// JRG, tried this delay for CAEN, no good:
			printf("  RUNTEST, pause for %d usec\n",pause);
			if(pause>1000)usleep(pause);
			else usleep(1000);
		*/

					if (pause>65535) {
						sndbuf[0]=255;
						sndbuf[1]=255;
						for (looppause=0;looppause<pause/65536;looppause++) {
							if (pass == ipass || ipass == 4) { devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,0); }
						}
						pause=65535;
					}
					sndbuf[0]=pause-(pause/256)*256;
					sndbuf[1]=pause/256;
			// printf(" sndbuf %d %d %d \n",sndbuf[1],sndbuf[0],pause);
					if (pass == ipass || ipass == 4) devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,2);
					// fpause=fpause*1.5+100;
					// pause=fpause;
					//flush_vme();
					// usleep(pause);
					// printf(" send sleep \n");
				}
				else if((strcmp(Word[0],"STATE")==0)&&(strcmp(Word[1],"RESET")==0)&&(strcmp(Word[2],"IDLE;")==0)){
					printf("goto reset idle state\n");
					//	   usleep(1000);
					// DEBUG There is an error here vvv
					devdo(dv,-1,sndbuf,0,sndbuf,rcvbuf,2);
					//	   usleep(1000);
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
	//flush_vme();
	//send_last();
}


/*
void emu::fed::DDU::all_chip_info()
{
int i;
  char *fpga[3]={" ddufpga-0"," infpga-0 "," infpga-1 "};
  char *prom[5]={" inprom-0 "," inprom-1 "," vmeprom  "," dduprom-0"," dduprom-1"};
  printf("\n idCode: \n");
  for(i=0;i<3;i++)printf("%s",fpga[i]);printf("\n");
  printf(" %08lx ",ddufpga_idcode()); printf(" %08lx ",infpga_idcode0()); printf(" %08lx ",infpga_idcode1());printf("\n");
  for(i=0;i<5;i++)printf("%s",prom[i]);printf("\n");
  printf(" %08lx ",inprom_idcode0()); printf(" %08lx ",inprom_idcode1()); printf(" %08lx ",vmeprom_idcode());  printf(" %08lx ",dduprom_idcode0()); printf(" %08lx ",dduprom_idcode1());printf("\n");
  usleep(500);
  printf("\n userCode: \n");
  for(i=0;i<3;i++)printf("%s",fpga[i]);printf("\n");
  printf(" %08lx ",ddufpga_usercode()); printf(" %08lx ",infpga_usercode0()); printf(" %08lx ",infpga_usercode1());printf("\n");

  for(i=0;i<5;i++)printf("%s",prom[i]);printf("\n");
  printf(" %08lx ",inprom_usercode0()); printf(" %08lx ",inprom_usercode1()); printf(" %08lx ",vmeprom_usercode());  printf(" %08lx ",dduprom_usercode0()); printf(" %08lx ",dduprom_usercode1());printf("\n\n");
}
*/

/*
void emu::fed::DDU::executeCommand(std::string command)
{
}
*/

/** Part of the suite of chamber methods.
@returns a std::vector of chambers in fiber-order.
**/
std::vector<emu::fed::Chamber *> emu::fed::DDU::getChambers()
{
	return chamberVector_;
}


/** Part of the suite of chamber methods.
@param fiberNumber runs from 0-14 on (most) DDUs.
@returns the chamber at the given fiber input number.
**/
emu::fed::Chamber *emu::fed::DDU::getChamber(unsigned int fiberNumber)
{
	if (fiberNumber >= chamberVector_.size()) {
		//std::cerr << "chamberVector_ overflow!" << std::endl;
		return 0;
	} else return chamberVector_[fiberNumber];
}


/** Part of the suite of chamber methods.
@param chamber is the chamber being added.
@param fiberNumber is the fiber slot of the chamber.
**/
void emu::fed::DDU::addChamber(emu::fed::Chamber* chamber, unsigned int fiberNumber) {
	chamberVector_[fiberNumber] = chamber;
}


/** Part of the suite of chamber methods.
@param chamberVector is a std::vector of chambers to overwrite the internal std::vector.
**/
void emu::fed::DDU::setChambers(std::vector<emu::fed::Chamber *> chamberVector) {
	chamberVector_ = chamberVector;
}



// void emu::fed::DDU::init()
// 	throw (FEDException)
// {
// 	devdo(DDUFPGA,-1,cmd,0,sndbuf,rcvbuf,2);
// }



unsigned long int emu::fed::DDU::readReg(enum DEVTYPE dt, char reg, const unsigned int nbits = 16)
	throw (FEDException)
{

	char cmd[2];
	char sndbuf[1024];
	char rcvbuf[1024];
	
	if (nbits > 32) {
		XCEPT_RAISE(FEDException, "cannot read more than 32 bits at a time");
	}

	if (dt == DDUFPGA) {
		cmd[0]=VTX2P_USR1_L;
		cmd[1]=VTX2P_USR1_H;
		sndbuf[0]=reg;
		// Open USR1 pathway and write the command
		devdo(dt,10,cmd,8,sndbuf,rcvbuf,0);

		cmd[0]=VTX2P_BYPASS_L;
		cmd[1]=VTX2P_BYPASS_H;
		// Close USR1 pathway to execute the read.
		devdo(dt,10,cmd,0,sndbuf,rcvbuf,0);
	} else if (dt == INFPGA0 || dt == INFPGA1) {
		cmd[0]=VTX2P20_USR1_L;
		cmd[1]=VTX2P20_USR1_H;
		sndbuf[0]=reg;
		// Open USR1 pathway and write the command
		devdo(dt,14,cmd,8,sndbuf,rcvbuf,0);

		cmd[0]=VTX2P20_BYPASS_L;
		cmd[1]=VTX2P20_BYPASS_H;
		// Close USR1 pathway to execute the read.
		devdo(dt,14,cmd,0,sndbuf,rcvbuf,0);
	}else {
		XCEPT_RAISE(FEDException,"Can't read from that device");
	}

	// Now we shove through the number of bits we need with a shift test.
	//srand( time(NULL) );
	unsigned long int testVal = rand();
	sndbuf[0]=testVal & 0xff;
	sndbuf[1]=(testVal >> 8) & 0xff;
	//sndbuf[2]=(testVal >> 16) & 0xff;
	//sndbuf[3]=(testVal >> 24) & 0xff;

	std::bitset<16> test(testVal);

	//std::cout << "Reading register " << (int) reg << std::endl;
	//std::cout << "Shift test is       " << test << std::endl;

	if (dt == DDUFPGA) {
		cmd[0]=VTX2P_USR2_L;
		cmd[1]=VTX2P_USR2_H;
		// Open USR2 pathway and shove the bits through
		devdo(dt,10,cmd,nbits*2,sndbuf,rcvbuf,1);
	} else if (dt == INFPGA0 || dt == INFPGA1) {
		cmd[0]=VTX2P20_USR2_L;
		cmd[1]=VTX2P20_USR2_H;
		// Open USR2 pathway and shove the bits through
		devdo(dt,14,cmd,nbits*2,sndbuf,rcvbuf,1);
	}

	// The very first nbits bits are what we want to return.
	// Crawl through the buffers and give us what we want using std::bitsets.
	std::bitset<32> ret( rcvbuf[0]|(rcvbuf[1]<<8)|(rcvbuf[2]<<16)|(rcvbuf[3]<<24) );
	if (nbits < 32) ret &= ((1 << nbits) - 1);

	//std::cout << "The read returned   " << ret << std::endl;

	std::bitset<16> shiftTest;
	// Now crawl through the buffers for the shift-test.
	for (unsigned int ibit=nbits; ibit<(nbits+16); ibit++) {
		unsigned int ibuf = ibit/8;
		if (rcvbuf[ibuf] & (1 << (ibit%8))) shiftTest.set(ibit - nbits);
	}

	//std::cout << "Shift test returned " << shiftTest << std::endl << std::endl;

	// For some reason, the shift test only makes sense with reads of integer
	//  multiples of 16.  Go figure.
	if (shiftTest != test && nbits%16 == 0) {
		std::stringstream xceptString;
		xceptString << "shiftTest returned "<< shiftTest << ", should have been " << test;
		//XCEPT_RAISE(FEDException,xceptString.str());
	}

	// Close the read paths.
	if (dt == DDUFPGA) {
		cmd[0]=VTX2P_BYPASS_L;
		cmd[1]=VTX2P_BYPASS_H;
		// Close USR2 pathway to end the read
		devdo(dt,10,cmd,0,sndbuf,rcvbuf,0);
	} else if (dt == INFPGA0 || dt == INFPGA1) {
		cmd[0]=VTX2P20_BYPASS_L;
		cmd[1]=VTX2P20_BYPASS_H;
		// Close USR2 pathway to end the read
		devdo(dt,14,cmd,0,sndbuf,rcvbuf,0);
	}
	// Finally, set normal on the FPGA.
	setNormal(dt);

	return ret.to_ulong();

}



/** Same thing as the readReg routine, but we have to be a bit smarter about
*	where we place the shift test and where we put the data.
**/
// unsigned long int emu::fed::DDU::writeReg(enum DEVTYPE dt, char reg, unsigned long int value, const unsigned int nbits = 16)
// 	throw (FEDException)
// {
//
// 	if (nbits > 32) {
// 		XCEPT_RAISE(FEDException, "cannot write more than 32 bits at a time");
// 	}
//
// 	if (dt == DDUFPGA) {
// 		cmd[0]=VTX2P_USR1_L;
// 		cmd[1]=VTX2P_USR1_H;
// 		sndbuf[0]=reg;
// 		// Open USR1 pathway and write the command
// 		devdo(dt,10,cmd,8,sndbuf,rcvbuf,0);
//
// 		cmd[0]=VTX2P_BYPASS_L;
// 		cmd[1]=VTX2P_BYPASS_H;
// 		// Close USR1 pathway to execute the read and begin the write
// 		devdo(dt,10,cmd,0,sndbuf,rcvbuf,0);
// 	} else if (dt == INFPGA0 || dt == INFPGA1) {
// 		cmd[0]=VTX2P20_USR1_L;
// 		cmd[1]=VTX2P20_USR1_H;
// 		sndbuf[0]=reg;
// 		// Open USR1 pathway and write the command
// 		devdo(dt,14,cmd,8,sndbuf,rcvbuf,0);
//
// 		cmd[0]=VTX2P20_BYPASS_L;
// 		cmd[1]=VTX2P20_BYPASS_H;
// 		// Close USR1 pathway to execute the read and begin the write
// 		devdo(dt,14,cmd,0,sndbuf,rcvbuf,0);
// 	} else {
// 		XCEPT_RAISE(FEDException,"Can't write to that device");
// 	}
//
// 	// Now we shove through the number of bits we need with a shift test
// 	// AND the data we want to write.
// 	//srand( time(NULL) );
// 	unsigned long int testVal = rand();
// 	sndbuf[0]=testVal & 0xff;
// 	sndbuf[1]=(testVal >> 8) & 0xff;
// 	//sndbuf[2]=(testVal >> 16) & 0xff;
// 	//sndbuf[3]=(testVal >> 24) & 0xff;
//
// 	std::bitset<16> test(testVal);
//
// 	std::bitset<32> writeVal(value & ((1 << nbits) - 1));
//
// 	for (unsigned int ibit=0; ibit<nbits; ibit++) {
// 		unsigned int ibuf = ibit/8 + 2;
// 		if (writeVal.test(ibit)) {
// 			sndbuf[ibuf] |= (1 << (ibit%8));
// 		} else {
// 			sndbuf[ibuf] &= (255 - (1 << (ibit%8)));
// 		}
// 	}
// 	std::cout << "Attempting to write " << writeVal << " to register " << (int) reg << std::endl;
//
// 	std::cout << "Shift test is       " << test << std::endl;
//
// 	if (dt == DDUFPGA) {
// 		cmd[0]=VTX2P_USR2_L;
// 		cmd[1]=VTX2P_USR2_H;
// 		// Open USR2 pathway and shove the bits through
// 		devdo(dt,10,cmd,nbits + 16,sndbuf,rcvbuf,1);
//
// 		cmd[0]=VTX2P_BYPASS_L;
// 		cmd[1]=VTX2P_BYPASS_H;
// 		// Close USR2 pathway to commit the write
// 		devdo(dt,10,cmd,0,sndbuf,rcvbuf,0);
// 	} else if (dt == INFPGA0 || dt == INFPGA1) {
// 		cmd[0]=VTX2P20_USR2_L;
// 		cmd[1]=VTX2P20_USR2_H;
// 		// Open USR2 pathway and shove the bits through
// 		devdo(dt,14,cmd,nbits + 16,sndbuf,rcvbuf,1);
//
// 		cmd[0]=VTX2P20_BYPASS_L;
// 		cmd[1]=VTX2P20_BYPASS_H;
// 		// Close USR2 pathway to commit the write
// 		devdo(dt,14,cmd,0,sndbuf,rcvbuf,0);
// 	}
// 	setNormal(dt);
//
// 	// The old value is the first nbits bits.
// 	// The shift test is the next 32 bits.
//
// 	// Return is easy
// 	std::bitset<32> ret((rcvbuf[0] | (rcvbuf[1] << 8) | (rcvbuf[2] <<16) | (rcvbuf[3] << 24)) & ((1 << nbits) - 1));
//
// 	std::cout << "The old value was   " << ret << std::endl;
//
// 	std::bitset<16> shiftTest;
//
// 	// Crawl through the buffers and give us the shift test.
// 	for (unsigned int ibit=nbits; ibit<nbits+16; ibit++) {
// 		unsigned int ibuf = ibit/8;
// 		if (rcvbuf[ibuf] & (1 << (ibit%8))) shiftTest.set(ibit - nbits);
// 	}
//
// 	std::cout << "Shift test returned " << shiftTest << std::endl << std::endl;
//
// 	if (shiftTest != test) {
// 		std::stringstream xceptString;
// 		//xceptString << "shiftTest returned 0x" << std::hex << shiftTest << std::dec << ", should have been 0x" << std::hex << testBS << std::dec;
// 		xceptString << "shiftTest returned "<< shiftTest << ", should have been " << test;
// 		XCEPT_RAISE(FEDException,xceptString.str());
// 	}
//
// 	return ret.to_ulong(); // The previous value of the register.
//
// }



void emu::fed::DDU::reset(enum DEVTYPE dt = DDUFPGA)
	throw (FEDException)
{

	char cmd[2];
	char sndbuf[1];
	char rcvbuf[1];
	
	if (dt == DDUFPGA) {
		cmd[0]=VTX2P_USR1_L;
		cmd[1]=VTX2P_USR1_H;
		sndbuf[0]=DDUFPGA_RST;
		devdo(dt,10,cmd,8,sndbuf,rcvbuf,0);
		cmd[0]=VTX2P_BYPASS_L;
		cmd[1]=VTX2P_BYPASS_H;
		devdo(dt,10,cmd,0,sndbuf,rcvbuf,0);

		cmd[0]=VTX2P_USR1_L;
		cmd[1]=VTX2P_USR1_H;
		sndbuf[0]=NORM_MODE;
		devdo(dt,10,cmd,8,sndbuf,rcvbuf,0);
		cmd[0]=VTX2P_BYPASS_L;
		cmd[1]=VTX2P_BYPASS_H;
		sndbuf[0]=0;
		devdo(dt,10,cmd,0,sndbuf,rcvbuf,0);
	} else if (dt == INFPGA0 || dt == INFPGA1) {
		cmd[0]=VTX2P20_USR1_L;
		cmd[1]=VTX2P20_USR1_H;
		sndbuf[0]=DDUFPGA_RST;
		devdo(dt,14,cmd,8,sndbuf,rcvbuf,0);
		cmd[0]=VTX2P20_BYPASS_L;
		cmd[1]=VTX2P20_BYPASS_H;
		devdo(dt,14,cmd,0,sndbuf,rcvbuf,0);

		cmd[0]=VTX2P20_USR1_L;
		cmd[1]=VTX2P20_USR1_H;
		sndbuf[0]=NORM_MODE;
		devdo(dt,14,cmd,8,sndbuf,rcvbuf,0);
		cmd[0]=VTX2P20_BYPASS_L;
		cmd[1]=VTX2P20_BYPASS_H;
		sndbuf[0]=0;
		devdo(dt,14,cmd,0,sndbuf,rcvbuf,0);
	}

}



void emu::fed::DDU::setNormal(enum DEVTYPE dt = DDUFPGA)
	throw (FEDException)
{
	char cmd[2];
	char sndbuf[1];
	char rcvbuf[1];
	
	if (dt == DDUFPGA) {

		cmd[0]=VTX2P_USR1_L;
		cmd[1]=VTX2P_USR1_H;
		sndbuf[0]=NORM_MODE;
		devdo(dt,10,cmd,8,sndbuf,rcvbuf,0);

		cmd[0]=VTX2P_BYPASS_L;
		cmd[1]=VTX2P_BYPASS_H;
		sndbuf[0]=0;
		devdo(dt,10,cmd,0,sndbuf,rcvbuf,0);
	} else {
		cmd[0]=VTX2P20_USR1_L;
		cmd[1]=VTX2P20_USR1_H;
		sndbuf[0]=NORM_MODE;
		devdo(dt,14,cmd,8,sndbuf,rcvbuf,0);

		cmd[0]=VTX2P20_BYPASS_L;
		cmd[1]=VTX2P20_BYPASS_H;
		sndbuf[0]=0;
		devdo(dt,14,cmd,0,sndbuf,rcvbuf,0);
	}
}



unsigned long int emu::fed::DDU::readFPGAStat(enum DEVTYPE dt)
	throw (FEDException)
{
	try { return readReg(dt,3,32); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::checkFIFO(int fifo)
	throw (FEDException)
{
	if (fifo < 1 || fifo > 3) {
		XCEPT_RAISE(FEDException, "argument must be between 1 and 3 (inclusive)");
	}

	int command;
	if (fifo == 1)
		command=7;
	else if (fifo == 2)
		command=8;
	else if (fifo == 3)
		command=11;

	try { return readReg(DDUFPGA,command,16); }
	catch (FEDException &e) { throw; }
}



long int emu::fed::DDU::readKillFiber()
	throw (FEDException)
{
	try { return readReg(DDUFPGA,13,20); }
	catch (FEDException &e) { throw; }
}



// void emu::fed::DDU::writeKillFiber(long int killFiber = 0xf7fff)
// 	throw (FEDException)
// {
// 	try { return writeReg(DDUFPGA,14,killFiber,20); }
// 	catch (FEDException &e) { throw; }
// }



int emu::fed::DDU::readCRCError()
	throw (FEDException)
{
	try { return readReg(DDUFPGA,10,15); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readXmitError()
	throw (FEDException)
{
	try { return readReg(DDUFPGA,12,15); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readDMBError()
	throw (FEDException)
{
	try { return readReg(DDUFPGA,15,15); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readTMBError()
	throw (FEDException)
{
	try { return readReg(DDUFPGA,16,15); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readLIEError()
	throw (FEDException)
{
	try { return readReg(DDUFPGA,18,15); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readFFError()
	throw(FEDException)
{
	try { return readReg(DDUFPGA,9,15); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readEBReg(int reg = 1)
	throw(FEDException)
{
	if (reg < 1 || reg > 3) {
		XCEPT_RAISE(FEDException, "argument must be 1, 2, or 3");
	}

	try { return readReg(DDUFPGA,21 + reg,16); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readInRDStat()
	throw (FEDException)
{
	try { return readReg(DDUFPGA,19,16); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readInCHistory()
	throw (FEDException)
{
	try { return readReg(DDUFPGA,20,16); }
	catch (FEDException &e) { throw; }
}
//
//
//
// int emu::fed::DDU::readVoteErrorCount()
// 	throw (FEDException)
// {
// 	try { return readReg(DDUFPGA,25,16); }
// 	catch (FEDException &e) { throw; }
// }
//
//
//
// int emu::fed::DDU::readConsVoteErrorCount()
// 	throw (FEDException)
// {
// 	try { return readReg(DDUFPGA,26,16); }
// 	catch (FEDException &e) { throw; }
// }
//
//
//
// int emu::fed::DDU::readFIFOVoteErrorCount(int fifo = 0)
// 	throw (FEDException)
// {
// 	if (fifo < 0 || fifo > 1) {
// 		XCEPT_RAISE(FEDException, "argument must be 0 or 1");
// 	}
//
// 	try { return readReg(DDUFPGA,27+fifo,16); }
// 	catch (FEDException &e) { throw; }
// }
//
//
//
// int emu::fed::DDU::readEarlyVoteErrorCount()
// 	throw (FEDException)
// {
// 	try { return readReg(DDUFPGA,19,16); }
// 	catch (FEDException &e) { throw; }
// }
//
//
//
// int emu::fed::DDU::readVoteError23Count()
// 	throw (FEDException)
// {
// 	try { return readReg(DDUFPGA,20,16); }
// 	catch (FEDException &e) { throw; }
// }
//
//
//
// int emu::fed::DDU::readVoteError55Count()
// 	throw (FEDException)
// {
// 	try { return readReg(DDUFPGA,21,16); }
// 	catch (FEDException &e) { throw; }
// }
//
//
//
int emu::fed::DDU::readOutputStat()
	throw (FEDException)
{
	try { return readReg(DDUFPGA,6,16); }
	catch (FEDException &e) { throw; }
}
//
//
//
long int emu::fed::DDU::readL1Scaler(enum DEVTYPE dt = DDUFPGA)
	throw (FEDException)
{
	try { return readReg(dt,2,24); }
	catch (FEDException &e) { throw; }
}
//
//
//
int emu::fed::DDU::readALCTError()
	throw (FEDException)
{
	try { return readReg(DDUFPGA,17,15); }
	catch (FEDException &e) { throw; }
}



// int emu::fed::DDU::writeBXOrbit(int BXOrbit)
// 	throw (FEDException)
// {
// 	try { return writeReg(DDUFPGA,29,BXOrbit,12); }
// 	catch (FEDException &e) { throw; }
// }



int emu::fed::DDU::readBXOrbit()
	throw (FEDException)
{
	try { return readReg(DDUFPGA,30,12); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readRUI()
	throw (FEDException)
{
	try { return readReg(DDUFPGA,32,16); }
	catch (FEDException &e) { throw; }
}
//
//
//
// unsigned long int emu::fed::DDU::readOccupancyMon()
// 	throw (FEDException)
// {
// 	try { return readReg(DDUFPGA,34,32); }
// 	catch (FEDException &e) { throw; }
// }



void emu::fed::DDU::toggleL1Cal()
	throw (FEDException)
{
	try { readReg(DDUFPGA,31); }
	catch (FEDException &e) { throw; }
}



void emu::fed::DDU::vmeL1A()
	throw (FEDException)
{
	try { readReg(DDUFPGA,33); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readDMBLive()
	throw (FEDException)
{
	try { return readReg(DDUFPGA,25,15); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readPermDMBLive()
	throw (FEDException)
{
	try { return readReg(DDUFPGA,26,15); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readWarnMon()
	throw (FEDException)
{
	try { return readReg(DDUFPGA,27,16); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readMaxTimeoutCount()
	throw (FEDException)
{
	try { return readReg(DDUFPGA,28,16); }
	catch (FEDException &e) { throw; }
}



long int emu::fed::DDU::readL1Scaler1(enum DEVTYPE dt)
	throw (FEDException)
{
	if (dt != INFPGA0 && dt != INFPGA1) {
		XCEPT_RAISE(FEDException,"this can only be called with a DEVTYPE INFPGA0 or INFPGA1");
	}

	try { return readReg(dt,26,24); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::checkFiber(enum DEVTYPE dt)
	throw (FEDException)
{

	if (dt != INFPGA0 && dt != INFPGA1) {
		XCEPT_RAISE(FEDException,"this can only be called with a DEVTYPE INFPGA0 or INFPGA1");
	}

	try { return readReg(dt,6,16); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readDMBSync(enum DEVTYPE dt)
	throw (FEDException)
{

	if (dt != INFPGA0 && dt != INFPGA1) {
		XCEPT_RAISE(FEDException,"this can only be called with a DEVTYPE INFPGA0 or INFPGA1");
	}

	try { return readReg(dt,7,16); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readFIFOStat(enum DEVTYPE dt)
	throw (FEDException)
{

	if (dt != INFPGA0 && dt != INFPGA1) {
		XCEPT_RAISE(FEDException,"this can only be called with a DEVTYPE INFPGA0 or INFPGA1");
	}

	try { return readReg(dt,8,16); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readFIFOFull(enum DEVTYPE dt)
	throw (FEDException)
{

	if (dt != INFPGA0 && dt != INFPGA1) {
		XCEPT_RAISE(FEDException,"this can only be called with a DEVTYPE INFPGA0 or INFPGA1");
	}

	try { return readReg(dt,9,12); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readRxError(enum DEVTYPE dt)
	throw (FEDException)
{

	if (dt != INFPGA0 && dt != INFPGA1) {
		XCEPT_RAISE(FEDException,"this can only be called with a DEVTYPE INFPGA0 or INFPGA1");
	}

	try { return readReg(dt,10,16); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readTimeout(enum DEVTYPE dt)
	throw (FEDException)
{

	if (dt != INFPGA0 && dt != INFPGA1) {
		XCEPT_RAISE(FEDException,"this can only be called with a DEVTYPE INFPGA0 or INFPGA1");
	}

	try { return readReg(dt,11,16); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readTxError(enum DEVTYPE dt)
	throw (FEDException)
{

	if (dt != INFPGA0 && dt != INFPGA1) {
		XCEPT_RAISE(FEDException,"this can only be called with a DEVTYPE INFPGA0 or INFPGA1");
	}

	try { return readReg(dt,12,16); }
	catch (FEDException &e) { throw; }

}



int emu::fed::DDU::readWriteMemoryActive(enum DEVTYPE dt, int iFiber)
	throw (FEDException)
{

	if (dt != INFPGA0 && dt != INFPGA1) {
		XCEPT_RAISE(FEDException,"this can only be called with a DEVTYPE INFPGA0 or INFPGA1");
	}

	if (iFiber > 3 || iFiber < 0) {
		XCEPT_RAISE(FEDException,"second argument must be between 0 and 3 (inclusive)");
	}

	try { return readReg(dt,13 + iFiber,10); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readDMBWarning(enum DEVTYPE dt)
	throw (FEDException)
{

	if (dt != INFPGA0 && dt != INFPGA1) {
		XCEPT_RAISE(FEDException,"this can only be called with a DEVTYPE INFPGA0 or INFPGA1");
	}

	try { return readReg(dt,21,16); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readMemoryAvailable(enum DEVTYPE dt)
	throw (FEDException)
{

	if (dt != INFPGA0 && dt != INFPGA1) {
		XCEPT_RAISE(FEDException,"this can only be called with a DEVTYPE INFPGA0 or INFPGA1");
	}

	try { return readReg(dt,17,10); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readMinMemory(enum DEVTYPE dt)
	throw (FEDException)
{

	if (dt != INFPGA0 && dt != INFPGA1) {
		XCEPT_RAISE(FEDException,"this can only be called with a DEVTYPE INFPGA0 or INFPGA1");
	}

	try { return readReg(dt,18,10); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readLostError(enum DEVTYPE dt)
	throw (FEDException)
{

	if (dt != INFPGA0 && dt != INFPGA1) {
		XCEPT_RAISE(FEDException,"this can only be called with a DEVTYPE INFPGA0 or INFPGA1");
	}

	try { return readReg(dt,19,16); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readCCodeStat(enum DEVTYPE dt)
	throw (FEDException)
{

	if (dt != INFPGA0 && dt != INFPGA1) {
		XCEPT_RAISE(FEDException,"this can only be called with a DEVTYPE INFPGA0 or INFPGA1");
	}

	try { return readReg(dt,20,16); }
	catch (FEDException &e) { throw; }
}
//
//
//
// int emu::fed::DDU::readINFPGAStatusReg(enum DEVTYPE dt, int i)
// 	throw (FEDException)
// {
//
// 	if (dt != INFPGA0 && dt != INFPGA1) {
// 		XCEPT_RAISE(FEDException,"this can only be called with a DEVTYPE INFPGA0 or INFPGA1");
// 	}
//
// 	if (i < 1 || i > 3) {
// 		XCEPT_RAISE(FEDException,"second argument must be between 1 and 3 (inclusive)");
// 	}
//
// 	try { return readReg(dt,21+i,16); }
// 	catch (FEDException &e) { throw; }
//
// }



long unsigned int emu::fed::DDU::readFiberDiagnostics(enum DEVTYPE dt, int i)
	throw (FEDException)
{

	if (dt != INFPGA0 && dt != INFPGA1) {
		XCEPT_RAISE(FEDException,"this can only be called with a DEVTYPE INFPGA0 or INFPGA1");
	}

	if (i < 0 || i > 1) {
		XCEPT_RAISE(FEDException,"second argument must be 0 or 1");
	}

	try { return readReg(dt,30+i,32); }
	catch (FEDException &e) { throw; }
}



unsigned long int emu::fed::DDU::readFPGAUserCode(enum DEVTYPE dt)
	throw (FEDException)
{
	char cmd[2];
	char sndbuf[5];
	char rcvbuf[5];
	
	int address = 0;
	if (dt == DDUFPGA) {
		cmd[0]=VTX2P_USERCODE_L;
		cmd[1]=VTX2P_USERCODE_H;
		address = 10;
	} else if (dt == INFPGA0 || dt == INFPGA1) {
		cmd[0]=VTX2P20_USERCODE_L;
		cmd[1]=VTX2P20_USERCODE_H;
		address = 14;
	} else {
		XCEPT_RAISE(FEDException,"DEVTYPE not understood as an FPGA");
	}

	sndbuf[0]=0xFF;
	sndbuf[1]=0xFF;
	sndbuf[2]=0xFF;
	sndbuf[3]=0xFF;
	sndbuf[4]=0xFF;

	devdo(dt,address,cmd,32,sndbuf,rcvbuf,1);
	if (dt == DDUFPGA) {
		cmd[0]=VTX2P_BYPASS_L;
		cmd[1]=VTX2P_BYPASS_H;
	} else if (dt == INFPGA0 || dt == INFPGA1) {
		cmd[0]=VTX2P20_BYPASS_L;
		cmd[1]=VTX2P20_BYPASS_H;
	}

	sndbuf[0]=0;
	devdo(dt,address,cmd,0,sndbuf,rcvbuf,0);
	return (rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24);
}



unsigned long int emu::fed::DDU::readPROMUserCode(enum DEVTYPE dt)
	throw (FEDException)
{
	char cmd[2];
	char sndbuf[5];
	char rcvbuf[5];
	
	if (dt != DDUPROM0 && dt != DDUPROM1 && dt != INPROM0 && dt != INPROM1 && dt != VMEPROM) {
		XCEPT_RAISE(FEDException,"DEVTYPE not understood as a PROM");
	}

	cmd[0] = PROM_USERCODE;
	sndbuf[0]=0xFF;
	sndbuf[1]=0xFF;
	sndbuf[2]=0xFF;
	sndbuf[3]=0xFF;
	sndbuf[4]=0xFF;

	if (dt == INPROM1 || dt == DDUPROM1) {
		devdo(dt,8,cmd,33,sndbuf,rcvbuf,1);
		rcvbuf[0]=((rcvbuf[0]>>1)&0x7f)+((rcvbuf[1]<<7)&0x80);
		rcvbuf[1]=((rcvbuf[1]>>1)&0x7f)+((rcvbuf[2]<<7)&0x80);
		rcvbuf[2]=((rcvbuf[2]>>1)&0x7f)+((rcvbuf[3]<<7)&0x80);
		rcvbuf[3]=((rcvbuf[3]>>1)&0x7f)+((rcvbuf[4]<<7)&0x80);
	} else {
		devdo(dt,8,cmd,32,sndbuf,rcvbuf,1);
	}
	cmd[0] = PROM_BYPASS;  
	sndbuf[0]=0;
	devdo(dt,8,cmd,0,sndbuf,rcvbuf,0);

	return (rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24);
}




//
//
//
// unsigned long int emu::fed::DDU::readIDCode(enum DEVTYPE dt)
// 	throw (FEDException)
// {
//
// 	sndbuf[0]=0xFF;
// 	sndbuf[1]=0xFF;
// 	sndbuf[2]=0xFF;
// 	sndbuf[3]=0xFF;
// 	sndbuf[4]=0xFF;
//
// 	if (dt == DDUFPGA) {
// 		cmd[0]=VTX2P_IDCODE_L;
// 		cmd[1]=VTX2P_IDCODE_H;
// 		devdo(dt,10,cmd,32,sndbuf,rcvbuf,1);
// 		cmd[0]=VTX2P_BYPASS_L;
// 		cmd[1]=VTX2P_BYPASS_H;
// 		sndbuf[0]=0;
// 		devdo(dt,10,cmd,0,sndbuf,rcvbuf,0);
// 	} else if (dt == INFPGA0 || dt == INFPGA1) {
// 		cmd[0]=VTX2P20_IDCODE_L;
// 		cmd[1]=VTX2P20_IDCODE_H;
// 		devdo(dt,14,cmd,32,sndbuf,rcvbuf,1);
// 		cmd[0]=VTX2P20_BYPASS_L;
// 		cmd[1]=VTX2P20_BYPASS_H;
// 		sndbuf[0]=0;
// 		devdo(dt,14,cmd,0,sndbuf,rcvbuf,0);
// 	} else {
// 		cmd[0]=PROM_IDCODE;
// 		if (dt == DDUPROM1 || dt == INPROM1) {
// 			devdo(dt,8,cmd,33,sndbuf,rcvbuf,1);
// 		} else {
// 			devdo(dt,8,cmd,32,sndbuf,rcvbuf,1);
// 		}
// 		cmd[0]=PROM_BYPASS;
// 		sndbuf[0]=0;
// 		devdo(dt,8,cmd,0,sndbuf,rcvbuf,0);
// 	}
//
// 	unsigned long int ibrd=0x00000000;
// 	ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24)|ibrd;
// 	return ibrd;
// }
//
//
//
// unsigned long int emu::fed::DDU::readUserCode(enum DEVTYPE dt)
// 	throw (FEDException)
// {
//
// 	sndbuf[0]=0xFF;
// 	sndbuf[1]=0xFF;
// 	sndbuf[2]=0xFF;
// 	sndbuf[3]=0xFF;
// 	sndbuf[4]=0xFF;
//
// 	if (dt == DDUFPGA) {
// 		cmd[0]=VTX2P_USERCODE_L;
// 		cmd[1]=VTX2P_USERCODE_H;
// 		devdo(dt,10,cmd,32,sndbuf,rcvbuf,1);
// 		cmd[0]=VTX2P_BYPASS_L;
// 		cmd[1]=VTX2P_BYPASS_H;
// 		sndbuf[0]=0;
// 		devdo(dt,10,cmd,0,sndbuf,rcvbuf,0);
// 	} else if (dt == INFPGA0 || dt == INFPGA1) {
// 		cmd[0]=VTX2P20_USERCODE_L;
// 		cmd[1]=VTX2P20_USERCODE_H;
// 		devdo(dt,14,cmd,32,sndbuf,rcvbuf,1);
// 		cmd[0]=VTX2P20_BYPASS_L;
// 		cmd[1]=VTX2P20_BYPASS_H;
// 		sndbuf[0]=0;
// 		devdo(dt,14,cmd,0,sndbuf,rcvbuf,0);
// 	} else {
// 		cmd[0]=PROM_USERCODE;
// 		if (dt == DDUPROM1 || dt == INPROM1) {
// 			devdo(dt,8,cmd,33,sndbuf,rcvbuf,1);
// 		} else {
// 			devdo(dt,8,cmd,32,sndbuf,rcvbuf,1);
// 		}
// 		cmd[0]=PROM_BYPASS;
// 		sndbuf[0]=0;
// 		devdo(dt,8,cmd,0,sndbuf,rcvbuf,0);
// 	}
//
// 	unsigned long int ibrd=0x00000000;
// 	ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24)|ibrd;
// 	return ibrd;
// }
//
//
// Always 16 bits.
int emu::fed::DDU::readParallel(int command)
	throw (FEDException)
{
	char cmd[2];
	char sndbuf[4];
	char rcvbuf[4];
	
	cmd[0] = command&0x00ff;
	cmd[1] = (command&0xff00) >> 8;
	sndbuf[0]=0;
	sndbuf[1]=0;
	sndbuf[2]=0;
	sndbuf[3]=0;
	rcvbuf[0]=0;
	rcvbuf[1]=0;
	rcvbuf[2]=0;
	rcvbuf[3]=0;
	devdo(VMEPARA,1,cmd,0,sndbuf,rcvbuf,2);

	return ((rcvbuf[1]<<8)&0xff00)|(rcvbuf[0]&0xff);
}



void emu::fed::DDU::writeParallel(int command, int val)
	throw (FEDException)
{
	char cmd[2];
	char sndbuf[2];
	char rcvbuf[2];
	
	cmd[0]=(command&0x00ff);
	cmd[1]=(command&0xff00) >> 8;

	sndbuf[0]=(val&0x00ff);
	sndbuf[1]=(val&0xff00) >> 8;

	// Parallel writes are all 16-bit, so garbage the rest
	//srand( time(NULL) );
	for (int i = 2; i <= 7; i++) {
		//sndbuf[i]=rand() % 256;
		sndbuf[i] = 0;
	}
	rcvbuf[0]=0;
	rcvbuf[1]=0;

	devdo(VMEPARA,1,cmd,0,sndbuf,rcvbuf,2);
}



int emu::fed::DDU::readFMMBusy()
	throw (FEDException)
{
	try { return readParallel(0x0000); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readFMMFullWarning()
	throw (FEDException)
{
	try { return readParallel(0x0001); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readFMMLostSync()
	throw (FEDException)
{
	try { return readParallel(0x0002); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readFMMError()
	throw (FEDException)
{
	try { return readParallel(0x0003); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readCSCStat()
	throw (FEDException)
{
	try { return readParallel(0x0004); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readFMMReg()
	throw (FEDException)
{
	try { return readParallel(0x0F09); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readSwitches()
	throw (FEDException)
{
	try { return readParallel(0x000e); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readParallelStat()
	throw (FEDException)
{
	try { return readParallel(0x000f); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readInputReg(int iReg)
	throw (FEDException)
{
	if (iReg < 0 || iReg > 2) {
		XCEPT_RAISE(FEDException,"argument must be between 0 and 2 (inclusive)");
	}
	try { return readParallel(0x0008 | (iReg << 8)); }
	catch (FEDException &e) { throw; }
}



void emu::fed::DDU::writeInputReg(int val)
	throw (FEDException)
{
	try { writeParallel(0x8008, val); }
	catch (FEDException &e) { throw; }
}



void emu::fed::DDU::writeFMMReg(int val)
	throw (FEDException)
{
	try { writeParallel(0x8f09, val); }
	catch (FEDException &e) { throw; }
}



void emu::fed::DDU::writeFakeL1Reg(int val)
	throw (FEDException)
{
	try { writeParallel(0x8509, val); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readFakeL1Reg()
	throw (FEDException)
{
	try { return readParallel(0x0509); }
	catch (FEDException &e) { throw; }
}



void emu::fed::DDU::writeGbEPrescale(int val)
	throw (FEDException)
{
	try { writeParallel(0x8009, val); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readGbEPrescale()
	throw (FEDException)
{
	try { return readParallel(0x0009); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readTestReg(int iReg)
	throw (FEDException)
{
	if (iReg < 0 || iReg > 4) {
		XCEPT_RAISE(FEDException,"argument must be between 0 and 4 (inclusive)");
	}
	try { return readParallel(0x0008 | ((iReg + 3) << 8)); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readWarningHistory()
	throw (FEDException)
{
	try { return readParallel(0x0005); }
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readBusyHistory()
	throw (FEDException)
{
	try { return readParallel(0x0006); }
	catch (FEDException &e) { throw; }
}



unsigned long int emu::fed::DDU::readSerial(int command, const unsigned int nbits = 16)
	throw (FEDException)
{
	char cmd[2];
	char sndbuf[1024];
	char rcvbuf[1024];
	
	cmd[0] = command&0x00ff;
	cmd[1] = (command&0xff00) >> 8;

	devdo(VMESERI,2,cmd,0,sndbuf,(char *)rcvbuf,0);

	// Serial reads give rcv_serial[0] as the MSB
	std::bitset<32> ret;
	for (unsigned int ibit = 0; ibit < nbits; ibit++) {
		unsigned int ibuf = nbits/8 - ibit/8 - 1;
		if (rcvbuf[ibuf] & (1 << (ibit%8))) ret.set(ibit);
	}

	return ret.to_ulong();
}



void emu::fed::DDU::writeSerial(int command)
	throw (FEDException)
{
	// load writes into the inreg before calling this...
	char cmd[2];
	char sndbuf[1];
	char rcvbuf[1];
	
	cmd[0] = command&0x00ff;
	cmd[1] = (command&0xff00) >> 8;

	devdo(VMESERI,2,cmd,0,sndbuf,rcvbuf,0);
	usleep(20000);
}



char emu::fed::DDU::readSerialStat()
	throw (FEDException)
{
	try { return readSerial(0x0004,8); }
	catch (FEDException &e) { throw; }
}


/*
int emu::fed::DDU::readFlashKillFiber()
	throw (FEDException)
{
	try { return readSerial(0x0104,16); }
	catch (FEDException &e) { throw; }
}
*/


void emu::fed::DDU::writeFlashKillFiber(int val)
	throw (FEDException)
{
	try {
		writeInputReg(val);
		writeSerial(0x0904);
	}
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readFlashBoardID()
	throw (FEDException)
{
	try { return readSerial(0x0304,16); }
	catch (FEDException &e) { throw; }
}



void emu::fed::DDU::writeFlashBoardID(int val)
	throw (FEDException)
{
	try {
		writeInputReg(val);
		writeSerial(0x0b04);
	}
	catch (FEDException &e) { throw; }
	return;
}
//
//
//
// unsigned long int emu::fed::DDU::readFlashInFIFOThresholds()
// 	throw (FEDException)
// {
// 	try { return readSerial(0x0404,32); }
// 	catch (FEDException &e) { throw; }
// }
//
//
//
// void emu::fed::DDU::writeFlashInFIFOThresholds(unsigned long int val)
// 	throw (FEDException)
// {
// 	int val1 = val&0x0000ffff;
// 	int val2 = (val&0xffff0000) >> 16;
// 	try {
// 		writeInputReg(val2);
// 		writeInputReg(val1);
// 		writeSerial(0x0c04,val1, val2);
// 	}
// 	catch (FEDException &e) { throw; }
// 	return;
// }



unsigned long int emu::fed::DDU::readFlashGbEFIFOThresholds()
	throw (FEDException)
{
	try { return readSerial(0x0504,32); }
	catch (FEDException &e) { throw; }
}



void emu::fed::DDU::writeFlashGbEFIFOThresholds(int val1, int val2, int val3)
	throw (FEDException)
{
	try {
		writeInputReg(val3);
		writeInputReg(val2);
		writeInputReg(val1);
		writeSerial(0x0d04);
	}
	catch (FEDException &e) { throw; }
}



int emu::fed::DDU::readFlashRUI()
	throw (FEDException)
{
	try { return readSerial(0x0704,16); }
	catch (FEDException &e) { throw; }
}



void emu::fed::DDU::writeFlashRUI(int val)
	throw (FEDException)
{
	try {
		writeInputReg(val);
		writeSerial(0x0f04);
	}
	catch (FEDException &e) { throw; }
}
//
//
//
// unsigned long int emu::fed::DDU::readInFIFOThreshold(int iFifo)
// 	throw (FEDException)
// {
// 	if (iFifo > 3 || iFifo < 0) {
// 		XCEPT_RAISE(FEDException,"argument must be between 0 and 3 (inclusive)");
// 	}
//
// 	try { return readSerial(0x0000 + iFifo,32); }
// 	catch (FEDException &e) { throw; }
// }
//
//
//
// void emu::fed::DDU::forceLoadFIFOsFromFlash()
// 	throw (FEDException)
// {
// 	try { readSerial(0x000f); }
// 	catch (FEDException &e) { throw; }
// }
