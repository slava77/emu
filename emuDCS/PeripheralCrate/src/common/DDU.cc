//-----------------------------------------------------------------------
// $Id: DDU.cc,v 2.1 2006/01/11 13:47:39 mey Exp $
// $Log: DDU.cc,v $
// Revision 2.1  2006/01/11 13:47:39  mey
// Update
//
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
/* Programs here now talk to TCB Spartan via Dynatem/VME */
#include "DDU.h"
#include <cstdio>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>  /* sleep function */
#include <signal.h>  /* Usual interrupt handler stuff */
#include <string.h>
#include "GenDATA.h"
#include "VMEController.h"


DDU::DDU(int crate, int slot)
  : VMEModule(crate, slot){
  std::cout << "DDU: crate " << this->crate() << " slot " << this->slot() << std::endl;
}


void DDU::init(){
}

void DDU::SelfTest(){
}

void DDU::configure() {
}

void DDU::end() {
  std::cout << "DDU: calling end" << std::endl;
  theController->send_last();
  VMEModule::end();
}


void DDU::ddu_end() {
  cmd[0]=VTX_BYPASS;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(DCTRL,5,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR1;
  sndbuf[0]=NORM_MODE;
  devdo(DCTRL,5,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  sndbuf[0]=0;
  devdo(DCTRL,5,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::tcb_end() {
  cmd[0]=VTX_USR1;
  sndbuf[0]=NORM_MODE;
  devdo(TCTRL,3,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  sndbuf[0]=0;
  devdo(TCTRL,3,cmd,0,sndbuf,rcvbuf,2);
}


void DDU::tcb_trig()
{
  cmd[0]=VTX_USR1;
  sndbuf[0]=0x08;  //Use TCB function F8 for one trig. 
  devdo(TCTRL,3,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(TCTRL,3,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX_USR1;
  sndbuf[0]=NORM_MODE;
  devdo(TCTRL,3,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  sndbuf[0]=0;
  devdo(TCTRL,3,cmd,0,sndbuf,rcvbuf,2);
  std::cout << "DDU: TCB pulse done" << std::endl;
}


void DDU::tcb_burst()
{
  cmd[0]=VTX_USR1;
  sndbuf[0]=0x04;  //Use TCB function F4 for trigger burst 1000. 
  devdo(TCTRL,3,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(TCTRL,3,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX_USR1;
  sndbuf[0]=NORM_MODE;
  devdo(TCTRL,3,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  sndbuf[0]=0;
  devdo(TCTRL,3,cmd,0,sndbuf,rcvbuf,2);
  std::cout << "DDU: TCB burst done." << std::endl;
}
/* Programs here now talk to TCB Spartan via Dynatem/VME */



void DDU::tcb_bxr()
/* JRG: similar to "Clear LCT BX number", or the same?
	    Check  rice_routines/reset_lctbxnum.c  and CCB manual. */
{
  cmd[0]=VTX_USR1;
  sndbuf[0]=0x05;  //Use TCB function F5 for CCB BXR. 
  devdo(TCTRL,3,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(TCTRL,3,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX_USR1;
  sndbuf[0]=NORM_MODE;
  devdo(TCTRL,3,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  sndbuf[0]=0;
  devdo(TCTRL,3,cmd,0,sndbuf,rcvbuf,2);
  std::cout << "DDU: TCB pulse done" << std::endl;
}


void DDU::tcb_hrst()
{
  cmd[0]=VTX_USR1;
  sndbuf[0]=0x09;  //Use TCB function F9 for CCB hard reset. 
  devdo(TCTRL,3,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(TCTRL,3,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX_USR1;
  sndbuf[0]=NORM_MODE;
  devdo(TCTRL,3,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  sndbuf[0]=0;
  devdo(TCTRL,3,cmd,0,sndbuf,rcvbuf,2);
  std::cout << "DDU: TCB pulse done" << std::endl;
}

void DDU::dcntrl_reset()
{
  cmd[0]=VTX_USR1;
  sndbuf[0]=DCNTRL_RST;
  devdo(DCTRL,5,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(DCTRL,5,cmd,0,sndbuf,rcvbuf,0);
  //  sleep(1);

/*  cmd[0]=VTX_USR2;
  devdo(DCTRL,5,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(DCTRL,5,cmd,0,sndbuf,rcvbuf,0);
*/

// Put in a short wait (~10usec?):
/*  pause=10;
  sndbuf[0]=pause-(pause/256)*256;
  sndbuf[1]=pause/256;
  devdo(DCTRL,-99,sndbuf,0,sndbuf,rcvbuf,0);
*/

  cmd[0]=VTX_USR1;
  sndbuf[0]=NORM_MODE;
  devdo(DCTRL,5,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  sndbuf[0]=0;
  devdo(DCTRL,5,cmd,0,sndbuf,rcvbuf,2);
  std::cout << "DDU: DCNTRL reset done." << std::endl;
}

void DDU::ddu_dpromuser(void *data)
{
int i,shft0in,shft1in,shft2in;
enum DEVTYPE dv;
 std::cout << "DDU: ddu_dpromuser"<<std::endl;
  dv=DPROM;
  cmd[0]=PROM_USERCODE;
  //  cmd[0]=PROM_IDCODE;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0xF0;
  sndbuf[3]=0x0F;
  sndbuf[4]=0xFF;
  sndbuf[5]=0xF0;
  sndbuf[6]=0x0F;
  sndbuf[7]=0xCB;
  sndbuf[8]=0x04;
  sndbuf[9]=0x04;
  sndbuf[10]=0x04;
  sndbuf[11]=0x04;
  sndbuf[12]=0x00;
  devdo(dv,8,cmd,70,sndbuf,rcvbuf,1);
  shft0in=(((0x01&rcvbuf[2])<<15)|((0xff&rcvbuf[1])<<7)|(0xfe&rcvbuf[0])>>1);
  shft1in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  printf(" DDU DPROM USER CODE:  %04x/%04x \n",shft1in,shft0in);
  shft2in=(((0x01&rcvbuf[6])<<15)|((0xff&rcvbuf[5])<<7)|(0xfe&rcvbuf[4])>>1);
  printf("   ----> 70-bit PROM shift test:  sent 0xFACE first, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  cmd[0]=PROM_BYPASS;
  sndbuf[0]=0;
  devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
}


void DDU::ddu_dpromid(void *data)
{
int i,shft0in,shft1in,shft2in;
enum DEVTYPE dv;
 std::cout << " ddu_dpromid"<<std::endl;
  dv=DPROM;
  cmd[0]=PROM_IDCODE;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0xF0;
  sndbuf[3]=0x0F;
  sndbuf[4]=0xFF;
  sndbuf[5]=0xF0;
  sndbuf[6]=0x0F;
  sndbuf[7]=0xCB;
  sndbuf[8]=0x04;
  sndbuf[9]=0x04;
  sndbuf[10]=0x04;
  sndbuf[11]=0x04;
  sndbuf[12]=0x00;
  devdo(dv,8,cmd,70,sndbuf,rcvbuf,1);
  shft0in=(((0x01&rcvbuf[2])<<15)|((0xff&rcvbuf[1])<<7)|(0xfe&rcvbuf[0])>>1);
  shft1in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  printf(" DDU DPROM ID CODE:  %04x/%04x \n",shft1in,shft0in);
  shft2in=(((0x01&rcvbuf[6])<<15)|((0xff&rcvbuf[5])<<7)|(0xfe&rcvbuf[4])>>1);
  printf("   ----> 70-bit PROM shift test:  sent 0xFACE first, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  cmd[0]=PROM_BYPASS;
  sndbuf[0]=0;
  devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
}


void DDU::ddu_dfpgastat(void *data)
{
int i,shft0in,shft1in,shft2in;
long int errcode;
 std::cout << "DDU: ddu_dfpgastat"<<std::endl;

  cmd[0]=VTX_USR1;
  sndbuf[0]=3;
  devdo(DCTRL,5,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(DCTRL,5,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX_USR2;
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
  devdo(DCTRL,5,cmd,70,sndbuf,rcvbuf,1);
  printf(" 32-bit DDU DFPGA Status:  %02X%02x/%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
  errcode=((0x00ff&rcvbuf[0])|((0x00ff&rcvbuf[1])<<8)|((0x00ff&rcvbuf[2])<<16)|((0x00ff&rcvbuf[3])<<24));
  shft2in=(((0x01&rcvbuf[6])<<15)|((0xff&rcvbuf[5])<<7)|(0xfe&rcvbuf[4])>>1);
  //  shft2in=(((0xff&rcvbuf[5])<<8)|(0xff&rcvbuf[4]));
  printf("   ----> 70-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  ddu_status_decode(errcode);
  ddu_end();
}

void DDU::toggle_ddu_cal_l1a()
{
  cmd[0]=VTX_USR1;
  sndbuf[0]=0x31;
  devdo(DCTRL,5,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(DCTRL,5,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX_USR1;
  sndbuf[0]=NORM_MODE;
  devdo(DCTRL,5,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  sndbuf[0]=0;
  devdo(DCTRL,5,cmd,0,sndbuf,rcvbuf,2);
  std::cout << "DDU: toggle_ddu_cal_l1a ...done."<<std::endl;
}


/* For DDU 32-bit Error/Status Decode */

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
   if((0x00000020&code)>0)printf("   DDU Critical Data Error");
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

void DDU::ecntrl_reset()
{
  cmd[0]=VTX_USR1;
  sndbuf[0]=ECNTRL_RST;
  devdo(ECTRL,5,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(ECTRL,5,cmd,0,sndbuf,rcvbuf,0);
  /*  cmd[0]=VTX_USR2;
  devdo(ECTRL,5,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(ECTRL,5,cmd,0,sndbuf,rcvbuf,0); */
  cmd[0]=VTX_USR1;
  sndbuf[0]=NORM_MODE;
  devdo(ECTRL,5,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  sndbuf[0]=0;
  devdo(ECTRL,5,cmd,0,sndbuf,rcvbuf,2); 
  printf(" ECNTRL reset done. \n");
}

void DDU::ddu_epromuser(void *data)
{
int i,shft0in,shft1in,shft2in;
enum DEVTYPE dv;
  printf(" ddu_epromuser \n");
  dv=EPROM;
  cmd[0]=PROM_USERCODE;
  //  cmd[0]=PROM_IDCODE;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0xF0;
  sndbuf[3]=0x0F;
  sndbuf[4]=0xFF;
  sndbuf[5]=0xF0;
  sndbuf[6]=0x0F;
  sndbuf[7]=0xCB;
  sndbuf[8]=0x04;
  sndbuf[9]=0x04;
  sndbuf[10]=0x04;
  sndbuf[11]=0x04;
  sndbuf[12]=0x00;
  devdo(dv,8,cmd,70,sndbuf,rcvbuf,1);
  shft0in=(((0x01&rcvbuf[2])<<15)|((0xff&rcvbuf[1])<<7)|(0xfe&rcvbuf[0])>>1);
  shft1in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  printf(" DDU EPROM USER CODE:  %02x%02x/%02x%02x \n",(0x00ff&rcvbuf[3]),(0x00ff&rcvbuf[2]),(0x00ff&rcvbuf[1]),(0x00ff&rcvbuf[0])); //JRG: was shft1in,shft0in
  shft2in=(((0x01&rcvbuf[6])<<15)|((0xff&rcvbuf[5])<<7)|(0xfe&rcvbuf[4])>>1);
  printf("   ----> 70-bit PROM shift test:  sent 0xFACE first, got back 0x%04X \n",((rcvbuf[5]&0x00ff)<<8)|(rcvbuf[4]&0x00ff));  // JRG: was shft2in
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x\n",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
//JRG, no shift needed:    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  cmd[0]=PROM_BYPASS;
  sndbuf[0]=0;
  devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
}


void DDU::ddu_epromid(void *data)
{
int i,shft0in,shft1in,shft2in;
enum DEVTYPE dv;
  printf(" ddu_epromid \n");
  dv=EPROM;
  cmd[0]=PROM_IDCODE;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0xF0;
  sndbuf[3]=0x0F;
  sndbuf[4]=0xFF;
  sndbuf[5]=0xF0;
  sndbuf[6]=0x0F;
  sndbuf[7]=0xCB;
  sndbuf[8]=0x04;
  sndbuf[9]=0x04;
  sndbuf[10]=0x04;
  sndbuf[11]=0x04;
  sndbuf[12]=0x00;
  devdo(dv,8,cmd,70,sndbuf,rcvbuf,1);
  shft0in=(((0x01&rcvbuf[2])<<15)|((0xff&rcvbuf[1])<<7)|(0xfe&rcvbuf[0])>>1);
  shft1in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  printf(" DDU EPROM ID CODE:  %02x%02x/%02x%02x \n",(0x00ff&rcvbuf[3]),(0x00ff&rcvbuf[2]),(0x00ff&rcvbuf[1]),(0x00ff&rcvbuf[0])); //JRG: was shft1in,shft0in
  shft2in=(((0x01&rcvbuf[6])<<15)|((0xff&rcvbuf[5])<<7)|(0xfe&rcvbuf[4])>>1);
  printf("   ----> 70-bit PROM shift test:  sent 0xFACE first, got back 0x%04X \n",((rcvbuf[5]&0x00ff)<<8)|(rcvbuf[4]&0x00ff));  // JRG: was shft2in
  for(i=0;i<7;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x\n",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
//JRG, no shift needed:    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  cmd[0]=PROM_BYPASS;
  sndbuf[0]=0;
  devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
}


void DDU::ddu_efpgastat(void *data)
{
int i,shft0in,shft1in,shft2in;
  printf(" ddu_efpgastat \n");

  cmd[0]=VTX_USR1;
  sndbuf[0]=STATUS_S;
  devdo(ECTRL,5,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(ECTRL,5,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX_USR2;
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
  devdo(ECTRL,5,cmd,40,sndbuf,rcvbuf,1);
  printf(" 21-bit DDU EFPGA Status:  %02x%02x%02x \n",0x1f&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
  shft2in=(((0x1f&rcvbuf[4])<<11)|((0xff&rcvbuf[3])<<3)|(0xe0&rcvbuf[2])>>5);
  //  shft2in=(((0xff&rcvbuf[5])<<8)|(0xff&rcvbuf[4]));
  printf("   ----> 40-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x1f&rcvbuf[i+2])<<11)|((0xff&rcvbuf[i+1])<<3)|(0xe0&rcvbuf[i])>>5);
    shft1in=(((0x1f&rcvbuf[i+4])<<11)|((0xff&rcvbuf[i+3])<<3)|(0xe0&rcvbuf[i+2])>>5);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted five: %04x/%04x\n",shft1in,shft0in);
  }
  cmd[0]=VTX_BYPASS;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(ECTRL,5,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX_USR1;
  sndbuf[0]=NORM_MODE;
  devdo(ECTRL,5,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  sndbuf[0]=0;
  devdo(ECTRL,5,cmd,0,sndbuf,rcvbuf,2);
}

int DDU::load_hex(void *data){

  GenDATA *dp;
  char file2[80];
  int i,j,k;
  FILE *fp;
  char *buf;
  char *buf2;
  enum DEVTYPE devnum;
  dp=(GenDATA *)data;

 sprintf(file2,"%s/hexdump00.dat",dp->tmp_dir);
 printf(" %s \n",file2);
 fp=fopen(file2,"r");
 buf2=(char *)malloc(32001);
  i=0; 
 LOOP:
  k=fscanf(fp,"%04x\n",&j);
  if(i>=16){
  buf2[i-16]=j&0x00ff;
  buf2[i-15]=(j&0xff00)>>8;
  }
  i=i+2;
  // printf(" %04x %d \n",j,k);
  if(k>0)goto LOOP;
  fclose(fp);
  i=i-2;
  k=i;
  
   /* fifo write */  
   buf=buf2;
   j=0;
   for(i=0;i<5;i++){
     if(i==0)devnum=FIFO1;
     if(i==1)devnum=FIFO2;
     if(i==2)devnum=FIFO3;
     if(i==3)devnum=FIFO4;
     if(i==4)devnum=FIFO5;
     printf(" j k %d %d \n",j,k);
       if(dp->iuse[i]==1&&j+33<k){
       cmd[0]=4;
       devdo(devnum,1,cmd,3200,buf,rcvbuf,2); 
       printf(" devnum %d buf %02x %02x \n",devnum,buf[1]&0xff,buf[0]&0xff);
       buf=buf+3200;
       j=j+3200;
       theController->CloseJTAG();
       }
   }
   free(buf2);
   return 0;
}





void DDU::tcb_rdpromuser(void *data)
{
int i,shft0in,shft1in,shft2in;
enum DEVTYPE dv;
  printf(" tcb_rdpromuser \n");
  dv=TPROM;
  cmd[0]=PROM_USERCODE;
  //  cmd[0]=PROM_IDCODE;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0xF0;
  sndbuf[3]=0x0F;
  sndbuf[4]=0xFF;
  sndbuf[5]=0xF0;
  sndbuf[6]=0x0F;
  sndbuf[7]=0xCB;
  sndbuf[8]=0x04;
  sndbuf[9]=0x04;
  sndbuf[10]=0x04;
  sndbuf[11]=0x04;
  sndbuf[12]=0x00;
  devdo(dv,8,cmd,150,sndbuf,rcvbuf,1);
  shft0in=(((0x01&rcvbuf[2])<<15)|((0xff&rcvbuf[1])<<7)|(0xfe&rcvbuf[0])>>1);
  shft1in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  printf(" TCB PROM USER CODE:  %04x/%04x \n",shft1in,shft0in);
  shft2in=(((0x01&rcvbuf[6])<<15)|((0xff&rcvbuf[5])<<7)|(0xfe&rcvbuf[4])>>1);
  printf("   ----> 150-bit PROM shift test:  sent 0xFACE first, got back 0x%04X \n",shft2in);
  for(i=0;i<20;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  cmd[0]=PROM_BYPASS;
  sndbuf[0]=0;
  devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
}


void DDU::tcb_rdpromid(void *data)
{
int i,shft0in,shft1in,shft2in;
enum DEVTYPE dv;
  printf(" tcb_rdpromid \n");
  dv=TPROM;
  cmd[0]=PROM_IDCODE;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0xF0;
  sndbuf[3]=0x0F;
  sndbuf[4]=0xFF;
  sndbuf[5]=0xF0;
  sndbuf[6]=0x0F;
  sndbuf[7]=0xCB;
  sndbuf[8]=0x04;
  sndbuf[9]=0x04;
  sndbuf[10]=0x04;
  sndbuf[11]=0x04;
  sndbuf[12]=0x00;
  devdo(dv,8,cmd,50,sndbuf,rcvbuf,1);
  shft0in=(((0x01&rcvbuf[2])<<15)|((0xff&rcvbuf[1])<<7)|(0xfe&rcvbuf[0])>>1);
  shft1in=(((0x01&rcvbuf[4])<<15)|((0xff&rcvbuf[3])<<7)|(0xfe&rcvbuf[2])>>1);
  printf(" TCB PROM USER CODE:  %04x/%04x \n",shft1in,shft0in);
  shft2in=(((0x01&rcvbuf[6])<<15)|((0xff&rcvbuf[5])<<7)|(0xfe&rcvbuf[4])>>1);
  printf("   ----> 50-bit PROM shift test:  sent 0xFACE first, got back 0x%04X \n",shft2in);
  for(i=0;i<8;i=i+4){
    shft0in=(((0x01&rcvbuf[i+2])<<15)|((0xff&rcvbuf[i+1])<<7)|(0xfe&rcvbuf[i])>>1);
    shft1in=(((0x01&rcvbuf[i+4])<<15)|((0xff&rcvbuf[i+3])<<7)|(0xfe&rcvbuf[i+2])>>1);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted one: %04x/%04x\n",shft1in,shft0in);
  }
  cmd[0]=PROM_BYPASS;
  sndbuf[0]=0;
  devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
}


void DDU::tcb_rdfpgastat(void *data)
{
int i,shft0in,shft1in,shft2in;
  printf(" tcb_rdfpgastat \n");

  cmd[0]=VTX_USR1;
  sndbuf[0]=STATUS_S;
  devdo(TCTRL,3,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(TCTRL,3,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX_USR2;
  sndbuf[0]=0xB0;
  sndbuf[1]=0xEC;
  sndbuf[2]=0xCE;
  sndbuf[3]=0xFA;
  sndbuf[4]=0x04;
  sndbuf[5]=0x04;
  sndbuf[6]=0x00;
  sndbuf[7]=0x00;
/* The next 17 bits are last into shift register;
   set the highest (17th) bit to override switches with shifted bits! */
  sndbuf[8]=0xFE;
  sndbuf[9]=0xCA;
  sndbuf[10]=0x01;
  sndbuf[11]=0x00;
  sndbuf[12]=0x00;
  sndbuf[13]=0x00;
  sndbuf[14]=0x00;
  sndbuf[15]=0x00;
  devdo(TCTRL,3,cmd,81,sndbuf,rcvbuf,1);
  printf(" 17-bit TCB FPGA Setting:  %01x%02x%02x \n",0x01&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
  shft2in=(((0x03&rcvbuf[6])<<14)|((0xff&rcvbuf[5])<<6)|(0xfc&rcvbuf[4])>>2);
  //  shft2in=(((0xff&rcvbuf[5])<<8)|(0xff&rcvbuf[4]));
  printf("   ----> 81-bit FPGA shift test:  sent 0xFACE, got back 0x%04X \n",shft2in);
  for(i=0;i<7;i=i+4){
    shft0in=(((0x03&rcvbuf[i+2])<<14)|((0xff&rcvbuf[i+1])<<6)|(0xfc&rcvbuf[i])>>2);
    shft1in=(((0x03&rcvbuf[i+4])<<14)|((0xff&rcvbuf[i+3])<<6)|(0xfc&rcvbuf[i+2])>>2);
    printf("      rcv bytes %d-%d:  %02x%02x/%02x%02x",i+3,i,0xff&rcvbuf[i+3],0xff&rcvbuf[i+2],0xff&rcvbuf[i+1],0xff&rcvbuf[i]);
    printf("      right-shifted two: %04x/%04x\n",shft1in,shft0in);
  }
  cmd[0]=VTX_BYPASS;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(TCTRL,3,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX_USR1;
  sndbuf[0]=NORM_MODE;
  devdo(TCTRL,3,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  sndbuf[0]=0;
  devdo(TCTRL,3,cmd,0,sndbuf,rcvbuf,2);

  /*  Gu method to read Virtex FPGA user code:
  SPARTAN uses 3-bit IR
  dv=TCTRL;
  cmd[0]=VTX_USERCODE;
  sndbuf[0]=0xCE;
  sndbuf[1]=0xFA;
  sndbuf[2]=0xFF;
  sndbuf[3]=0xFF;
  sndbuf[4]=0xFF;
  devdo(dv,5,cmd,25,sndbuf,rcvbuf,1);
  printf(" The FPGA USER CODE is %02x%02x%02x \n",0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
  cmd[0]=VTX_BYPASS;
  sndbuf[0]=0;
  devdo(dv,5,cmd,0,sndbuf,rcvbuf,2);*/
}

void DDU::tcb_reset()
{
  cmd[0]=VTX_USR1;
  sndbuf[0]=DCNTRL_RST;
  // JG: for Virtex was  devdo(TCTRL,5,cmd,8,sndbuf,rcvbuf,0);
  devdo(TCTRL,3,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(TCTRL,3,cmd,0,sndbuf,rcvbuf,0);

/*  cmd[0]=VTX_USR2;
  devdo(TCTRL,3,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(TCTRL,3,cmd,0,sndbuf,rcvbuf,0);
*/
  // pause = 1000000;
  // pause();
  cmd[0]=VTX_USR1;
  sndbuf[0]=NORM_MODE;
  devdo(TCTRL,3,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  sndbuf[0]=0;
  devdo(TCTRL,3,cmd,0,sndbuf,rcvbuf,2);
  printf(" TCB reset done. \n");
}
/*  
    Simulated running environment, issue a trigger via JTAG and place
    resulting readout in a file.  "Synchronous" because each triggered
    result is read before the next trigger is issued.
*/


void DDU::pause(DEVTYPE dv, int microseconds) {
  sndbuf[0] = microseconds % 256;
  sndbuf[1] = microseconds / 256;
  devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,0);
}


unsigned short int crcdivide(unsigned short int *datastream)
{
  unsigned long int remainder=0;
  unsigned short int polynomial=32771;  //2**15+2+1
  unsigned short int blocksize=96, crcsize=15, crc,i,j;
  for (j=0;j<blocksize;j++)
  {
    remainder=(remainder<<13) + (datastream[j]&8191);
    for (i=13;i>0;i--)
        if (remainder & (1<<(i+crcsize-1)))  remainder ^= (polynomial<<(i-1));
  }
  // to append 15 zero at the end
  remainder=(remainder<<15);
  for (i=15;i>0;i--)
    if (remainder & (1<<(i+crcsize-1))) remainder ^= (polynomial<<(i-1));
  crc=remainder & 32767;
  return crc;
}

