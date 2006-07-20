 // $Id: ddutest.cpp,v 3.0 2006/07/20 21:16:11 geurts Exp $
// $Log: ddutest.cpp,v $
// Revision 3.0  2006/07/20 21:16:11  geurts
// *** empty log message ***
//
// Revision 1.1  2005/09/01 16:09:21  gilmore
// *** empty log message ***
//
// Revision 1.16  2004/10/03 01:55:51  tfcvs
// various updates at H2a
//
// Revision 1.15  2004/09/15 21:44:42  tfcvs
// New timing routines
//
// Revision 1.14  2004/09/14 00:27:07  tfcvs
// Update files
//
// Revision 1.13  2004/05/21 10:05:05  tfcvs
// *** empty log message ***
//
// Revision 1.12  2004/05/20 09:52:23  tfcvs
// fixed type in CVS keyword
//
// Revision 1.11  2004/05/20 09:50:32  tfcvs
// Introduced "-f" commandline option to set the configuration file.
// Wait for user input before disabling the TestBeamController. (FG)
//
// ----
#include <iostream>
#include "VMEController.h"
#include "Crate.h"
#include "DDUTester.h"
#include "DDU.h"
#include "DCC.h"
#include <unistd.h>
#include "FEDCrateController.h"
#include "FEDCrateParser.h"
#include <stdio.h>
#include <fcntl.h>

int nbuf;
char buf[200];
char val[2];
char type[10];
char type2[10];
char command[100];
char command2[100];

extern char filename[100];
extern unsigned int hexval;
extern short int intval;
extern short int intval2;


int main(int argc, char **argv)
{
  int i;
  enum DEVTYPE dv;
  static int tintval,tintval2;
  char *xmlFile = "configTest.xml";
  int fdr;
  int fdw;
  int n,ityp,icmd,ival,nval;
 
   cout << "FEDCrate configuration file: " << xmlFile << endl;
   FEDCrateParser parser;
   parser.parseFile(xmlFile);

   DDU *ddu = parser.dduParser().ddu();
   DCC *dcc = parser.dccParser().dcc();
 
   // fork off java gui
    system("java ControlJava &");
    // start parsing commands from gui
    fdr = open("pipew.txt",O_RDONLY);
    fdw = open("piper.txt",O_WRONLY);
    ival=0;
    nval=0;
 LOOP:
  nbuf=read(fdr,buf,200);
  buf[nbuf]='\0';
  if(nbuf<=0)goto ENDR;
  if(strcmp(buf,"Exit")==0)goto ENDR;
  ityp=1;
  icmd=0;
  n=0;
  // printf("input:");for(i=0;i<nbuf;i++)printf("%c",buf[i]);printf("\n");

 
  for(i=1;i<nbuf;i++){
    if(ityp==1)type[i-1]=buf[i];
    if(icmd==1)command[i-n]=buf[i];
    if(buf[i]==':'){
      icmd=1;
      ityp=0;
      type[i-1]='\0';
      n=i+1;
    }
  }
  icmd=0;
  command[nbuf-n]='\0';
  if((strcmp(type,"DDU")==0)||(strcmp(type,"DCC")==0)){
     val[0]=buf[0];
     val[1]='\0';
     if(strcmp(val,"0")==0)ival=0;
     if(strcmp(val,"1")==0)ival=1;
     for(i=0;i<10;i++)type2[i]=type[i];
     for(i=0;i<100;i++)command2[i]=command[i];
  }

  printf(" typ: %s \n",type);
  printf(" cmd: %s \n",command);

  if(strcmp(type,"FILE")==0){
    nval=-1;
    for(i=0;i<90;i++)filename[i]=command[i];
  }
  if(strcmp(type,"4Hex")==0){
    sscanf(command,"%x\n",&hexval);
  }
  if(strcmp(type,"2Hex")==0){
    sscanf(command,"%x\n",&hexval);
  }
  if(strcmp(type,"Int")==0){
    sscanf(command,"%x\n",&intval);
  }
  if(strcmp(type,"Int2")==0){
    sscanf(command,"%d %d\n",&intval,&intval2);
    printf("Int2 %d %d \n",intval,intval2);
  } 

  /*****   DDU Commands *****/

  if(strcmp(type2,"DDU")==0&&ival==nval){
    //process DDU commands 
    printf(" %s.Execute\n",type2);    
   
  // Program: Prog DDU
      if(strcmp(command2,"Load VMEPROM")==0)ddu->vmeprgprom(filename);       
      if(strcmp(command2,"Load DDUPROM")==0)ddu->dduprgprom(filename);       
      if(strcmp(command2,"Load INPROM")==0)ddu->inprgprom(filename);        
      if(strcmp(command2,"Load Emergency VMEPROM")==0)ddu->resetprgprom(filename);       
      if(strcmp(command2,"Dump userid/chipids")==0)ddu->all_chip_info();    
      if(strcmp(command2,"VMEPROM idcode")==0)printf(" %08x \n",ddu->vmeprom_idcode());
      if(strcmp(command2,"VMEPROM usercode")==0)printf(" %08x \n",ddu->vmeprom_usercode());
      if(strcmp(command2,"DDUPROM idcode0")==0)printf(" %08x \n",ddu->dduprom_idcode0());
      if(strcmp(command2,"DDUPROM idcode1")==0)printf(" %08x \n",ddu->dduprom_idcode1());
      if(strcmp(command2,"DDUPROM usercode0")==0)printf(" %08x \n",ddu->dduprom_usercode0());
      if(strcmp(command2,"DDUPROM usercode1")==0)printf(" %08x \n",ddu->dduprom_usercode1());
      if(strcmp(command2,"DDUFPGA idcode")==0)printf(" %08x \n",ddu->ddufpga_idcode());
      if(strcmp(command2,"DDUFPGA usercode")==0)printf(" %08x \n",ddu->ddufpga_usercode());
      if(strcmp(command2,"INPROM idcode0")==0)printf(" %08x \n",ddu->inprom_idcode0());
      if(strcmp(command2,"INPROM idcode1")==0)printf(" %08x \n",ddu->inprom_idcode1());
      if(strcmp(command2,"INPROM usercode0")==0)printf(" %08x \n",ddu->inprom_usercode0());
      if(strcmp(command2,"INPROM usercode1")==0)printf(" %08x \n",ddu->inprom_usercode1());
      if(strcmp(command2,"INFPGA idcode0")==0)printf(" %08x \n",ddu->infpga_idcode0());
      if(strcmp(command2,"INFPGA idcode1")==0)printf(" %08x \n",ddu->infpga_idcode1());
      if(strcmp(command2,"INFPGA usercode0")==0)printf(" %08x \n",ddu->infpga_usercode0());
      if(strcmp(command2,"INFPGA usercode1")==0)printf(" %08x \n",ddu->infpga_usercode1());
  // DDU Control: ddufpga
      if(strcmp(command2,"DDUddu reset")==0)ddu->ddu_reset();         
      if(strcmp(command2,"ddu low status")==0)ddu->ddu_lowfpgastat();   
      if(strcmp(command2,"ddu high status")==0)ddu->ddu_hifpgastat();  
      if(strcmp(command2,"ddu 32-bit status")==0)ddu->ddu_fpgastat();     
      if(strcmp(command2,"Read FIFO-A Reg")==0)ddu->ddu_checkFIFOa();     
      if(strcmp(command2,"Read FIFO-B Reg")==0)ddu->ddu_checkFIFOb();     
      if(strcmp(command2,"Read FIFO-C Reg")==0)ddu->ddu_checkFIFOc();     
      if(strcmp(command2,"FIFO Full Reg")==0)ddu->ddu_rdfferr();
      if(strcmp(command2,"Read KillFiber Reg")==0)ddu->ddu_rdkillfiber();
      if(strcmp(command2,"Load KillFiber Reg")==0)ddu->ddu_loadkillfiber(hexval);  
      if(strcmp(command2,"CRC Error Reg")==0)ddu->ddu_rdcrcerr();
      if(strcmp(command2,"Data Transmit Error Reg")==0)ddu->ddu_rdxmiterr();
      if(strcmp(command2,"DMB Error Reg")==0)ddu->ddu_rddmberr();
      if(strcmp(command2,"TMB Error Reg")==0)ddu->ddu_rdtmberr(); 
      if(strcmp(command2,"ALCT Error Reg")==0)ddu->ddu_rdalcterr();
      if(strcmp(command2,"LostInEvent Reg")==0)ddu->ddu_rdlieerr();
      if(strcmp(command2,"DDU L1 Scale")==0)ddu->ddu_rdscaler();
      if(strcmp(command2,"ERA Error Reg")==0)ddu->ddu_rderareg();
      if(strcmp(command2,"ERB Error Reg")==0)ddu->ddu_rderbreg();
      if(strcmp(command2,"ERC Error Reg")==0)ddu->ddu_rdercreg();
      if(strcmp(command2,"Output Status")==0)ddu->ddu_rdostat();
      if(strcmp(command2,"Load BX_Orbit")==0)ddu->ddu_loadbxorbit(hexval);
      if(strcmp(command2,"Read BX_Orbit")==0)ddu->ddu_rdbxorbit();
      if(strcmp(command2,"Toggle DDU Cal L1A")==0)ddu->ddu_lvl1onoff();
      if(strcmp(command2,"DDU shift test")==0)ddu->ddu_shfttst(intval);
      if(strcmp(command2,"Read DDR VoteErr Count")==0)ddu->ddu_rd_verr_cnt();
      if(strcmp(command2,"Read Consec Verr Count")==0)ddu->ddu_rd_cons_cnt();
      if(strcmp(command2,"FIFO-0 Verr Count")==0)ddu->ddu_fifo0verr_cnt();
      if(strcmp(command2,"FIFO-1 Verr Count")==0)ddu->ddu_fifo1verr_cnt();
      if(strcmp(command2,"Early 200ns Verr Count")==0)ddu->ddu_earlyVerr_cnt();
      if(strcmp(command2,"DDR Verr23 Count")==0)ddu->ddu_verr23cnt();
      if(strcmp(command2,"DDR Verr55 Count")==0)ddu->ddu_verr55cnt();
      if(strcmp(command2,"Read Board ID")==0)ddu->ddu_rd_boardID();
      if(strcmp(command2,"DDU VME L1A")==0)ddu->ddu_vmel1a();

  //DDU Control: infpga: infpga 0
      if(strcmp(command2,"infpga0 reset")==0)ddu->infpga_reset(INFPGA0);
      if(strcmp(command2,"infpga0 Read L1 Scaler")==0)ddu->infpga_rdscaler(INFPGA0);
      if(strcmp(command2,"infpga0 low status")==0)ddu->infpga_lowstat(INFPGA0);
      if(strcmp(command2,"infpga0 high status")==0)ddu->infpga_histat(INFPGA0);
      if(strcmp(command2,"infpga0 32-bit status")==0)ddu->infpgastat(INFPGA0);
      if(strcmp(command2,"infpga0 shift test")==0)ddu->infpga_shfttst(INFPGA0,intval);
      if(strcmp(command2,"infpga0 Check Fiber")==0)ddu->infpga_CheckFiber(INFPGA0);
      if(strcmp(command2,"infpga0 DMB Sync")==0)ddu->infpga_DMBsync(INFPGA0);
      if(strcmp(command2,"infpga0 FIFO Status")==0)ddu->infpga_FIFOstatus(INFPGA0);
      if(strcmp(command2,"infpga0 FIFO Full")==0)ddu->infpga_FIFOfull(INFPGA0);
      if(strcmp(command2,"infpga0 RxError")==0)ddu->infpga_RxErr(INFPGA0);
      if(strcmp(command2,"infpga0 Timeout")==0)ddu->infpga_Timeout(INFPGA0);
      if(strcmp(command2,"infpga0 XmitErr")==0)ddu->infpga_XmitErr(INFPGA0);
      if(strcmp(command2,"infpga0 Lost Error")==0)ddu->infpga_LostErr(INFPGA0);
      if(strcmp(command2,"infpga0 Stat-A Reg")==0)ddu->infpga_StatA(INFPGA0);
      if(strcmp(command2,"infpga0 Stat-B Reg")==0)ddu->infpga_StatB(INFPGA0);
      if(strcmp(command2,"infpga0 Stat-C Reg")==0)ddu->infpga_StatC(INFPGA0);
      if(strcmp(command2,"infpga0 MemAvail")==0)ddu->infpga_MemAvail(INFPGA0);
      if(strcmp(command2,"Fiber0 1-0 WrMemActive")==0)ddu->infpga_WrMemActive(INFPGA0,0); // needs to be fixed
      if(strcmp(command2,"Fiber0 3-2 WrMemActive")==0)ddu->infpga_WrMemActive(INFPGA0,0);
      if(strcmp(command2,"Fiber0 5-4 WrMemActive")==0)ddu->infpga_WrMemActive(INFPGA0,0);
      if(strcmp(command2,"Fiber0 7-6 WrMemActive")==0)ddu->infpga_WrMemActive(INFPGA0,0);
      if(strcmp(command2,"infpga0 Min MemAvail")==0)ddu->infpga_Min_Mem(INFPGA0);
  //DDU Control: infpga: infpga 1
      if(strcmp(command2,"infpga1 reset")==0)ddu->infpga_reset(INFPGA1);
      if(strcmp(command2,"infpga1 Read L1 Scaler")==0)ddu->infpga_rdscaler(INFPGA1);
      if(strcmp(command2,"infpga1 low status")==0)ddu->infpga_lowstat(INFPGA1);
      if(strcmp(command2,"infpga1 high status")==0)ddu->infpga_histat(INFPGA1);
      if(strcmp(command2,"infpga1 32-bit status")==0)ddu->infpgastat(INFPGA1);
      if(strcmp(command2,"infpga1 shift test")==0)ddu->infpga_shfttst(INFPGA1,intval);
      if(strcmp(command2,"infpga1 Check Fiber")==0)ddu->infpga_CheckFiber(INFPGA1);
      if(strcmp(command2,"infpga1 DMB Sync")==0)ddu->infpga_DMBsync(INFPGA1);
      if(strcmp(command2,"infpga1 FIFO Status")==0)ddu->infpga_FIFOstatus(INFPGA1);
      if(strcmp(command2,"infpga1 FIFO Full")==0)ddu->infpga_FIFOfull(INFPGA1);
      if(strcmp(command2,"infpga1 RxError")==0)ddu->infpga_RxErr(INFPGA1);
      if(strcmp(command2,"infpga1 Timeout")==0)ddu->infpga_Timeout(INFPGA1);
      if(strcmp(command2,"infpga1 XmitErr")==0)ddu->infpga_XmitErr(INFPGA1);
      if(strcmp(command2,"infpga1 Lost Error")==0)ddu->infpga_LostErr(INFPGA1);
      if(strcmp(command2,"infpga1 Stat-A Reg")==0)ddu->infpga_StatA(INFPGA1);
      if(strcmp(command2,"infpga1 Stat-B Reg")==0)ddu->infpga_StatB(INFPGA1);
      if(strcmp(command2,"infpga1 Stat-C Reg")==0)ddu->infpga_StatC(INFPGA1);
      if(strcmp(command2,"infpga1 MemAvail")==0)ddu->infpga_MemAvail(INFPGA1);
      if(strcmp(command2,"Fiber1 1-0 WrMemActive")==0)ddu->infpga_WrMemActive(INFPGA1,0); // needs to be fixed
      if(strcmp(command2,"Fiber1 3-2 WrMemActive")==0)ddu->infpga_WrMemActive(INFPGA1,0);
      if(strcmp(command2,"Fiber1 5-4 WrMemActive")==0)ddu->infpga_WrMemActive(INFPGA1,0);
      if(strcmp(command2,"Fiber1 7-6 WrMemActive")==0)ddu->infpga_WrMemActive(INFPGA1,0);
      if(strcmp(command2,"infpga1 Min MemAvail")==0)ddu->infpga_Min_Mem(INFPGA1);
  //DDU Control: vmeparallel
      if(strcmp(command2,"status")==0)ddu->vmepara_status();
      if(strcmp(command2,"busy")==0)ddu->vmepara_busy();
      if(strcmp(command2,"warning/near full")==0)ddu->vmepara_fullwarn();
      if(strcmp(command2,"lost sync")==0)ddu->vmepara_lostsync();
      if(strcmp(command2,"error")==0)ddu->vmepara_error();
      if(strcmp(command2,"switches")==0)ddu->vmepara_switch();
      if(strcmp(command2,"Rd Inreg0")==0)ddu->vmepara_rd_inreg0();
      if(strcmp(command2,"Rd Inreg1")==0)ddu->vmepara_rd_inreg1();
      if(strcmp(command2,"Rd Inreg2")==0)ddu->vmepara_rd_inreg2();
      if(strcmp(command2,"Write Inreg")==0)ddu->vmepara_wr_inreg(hexval);  
  //DDU Control: vmeserial
      if(strcmp(command2,"read status")==0)ddu->read_status();
      if(strcmp(command2,"Wr Flash KillCh")==0)ddu->write_page1();
      if(strcmp(command2,"Rd Flash KillCh")==0)ddu->read_page1();
      if(strcmp(command2,"Wr Flash DDRthresh")==0)ddu->write_page4();
      if(strcmp(command2,"Rd Flash DDRthresh")==0)ddu->read_page4();
      if(strcmp(command2,"Wr Flash GBEthresh")==0)ddu->write_page5();
      if(strcmp(command2,"Rd Flash GBEthresh")==0)ddu->read_page5();
      if(strcmp(command2,"Wr Flash DDUid")==0)ddu-> write_page7();
      if(strcmp(command2,"Rd Flash DDUid")==0)ddu->read_page7();
      if(strcmp(command2,"Rd DDR-FIFOthresh0")==0)ddu->read_vmesd0();
      if(strcmp(command2,"Rd DDR-FIFOthresh1")==0)ddu->read_vmesd1();
      if(strcmp(command2,"Rd DDR-FIFOthresh2")==0)ddu->read_vmesd2();
      if(strcmp(command2,"Rd DDR-FIFOthresh3")==0)ddu->read_vmesd3();
      if(strcmp(command2,"Force DDR Load")==0)ddu->write_vmesdF();
      if(strcmp(command2,"DDU Temps")==0)ddu->read_therm();
      if(strcmp(command2,"DDU Voltages")==0)ddu->read_voltages();
    nval=-1;
  }


  /*****   DCC Commands ****/


  if(strcmp(type2,"DCC")==0&&ival==nval){
    //process DCC commands
     printf(" %s.Execute\n",type2);    
     // program DCC
  
      if(strcmp(command2,"Load MPROMs")==0)dcc->prgmgprom(filename);
      if(strcmp(command2,"Load INPROM")==0)dcc->prginprom(filename);
      if(strcmp(command2,"INPROM Userid")==0)printf("INPROM USERID %08x \n",dcc->inprom_userid());
      if(strcmp(command2,"M1PROM Userid")==0)printf("M1PROM USERID %08x \n",dcc->m1prom_userid());
      if(strcmp(command2,"M2PROM Userid")==0)printf("M2PROM USERID %08x \n",dcc->m2prom_userid());
      if(strcmp(command2,"INPROM Chipid")==0)printf("INPROM CHIPID %08x \n",dcc->inprom_chipid());
      if(strcmp(command2,"M1PROM Chipid")==0)printf("M1PROM CHIPID %08x \n",dcc->m1prom_chipid());
      if(strcmp(command2,"M2PROM Chipid")==0)printf("M2PROM CHIPID %08x \n",dcc->m2prom_chipid());

// Control DDU: set

      if(strcmp(command2,"BX Reset")==0)dcc->mctrl_bxr();
      if(strcmp(command2,"EVN Reset")==0)dcc->mctrl_evnr();
      if(strcmp(command2,"Set FIFOs Used")==0)dcc->mctrl_fifoinuse(hexval);
      if(strcmp(command2,"TTC Command")==0)dcc->mctrl_ttccmd(intval);

// Control DDU: status

      if(strcmp(command2,"Read Status (low)")==0)dcc->mctrl_statl();
      if(strcmp(command2,"Read Status (high)")==0)dcc->mctrl_stath();

// Control DDU: fl1a

      if(strcmp(command2,"Load L1A (prompt)")==0){
                    dcc->mctrl_fakeL1A(intval,intval2);
                    tintval=intval;
                    tintval2=intval2;
                    }
      if(strcmp(command2,"Load L1A (no prompt)")==0)dcc->mctrl_fakeL1A(tintval,tintval2);
     nval=-1;
  }
   nval=nval+1;
   write(fdw,buf,1);
  goto LOOP;
 ENDR:
  close(fdr);
  close(fdw);
  return 0;
}


