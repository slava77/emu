//----------------------------------------------------------------------
// $Id: VMEModule.cc,v 3.1 2006/08/08 14:48:42 rakness Exp $
// $Log: VMEModule.cc,v $
// Revision 3.1  2006/08/08 14:48:42  rakness
// give TMB JTAG chains to FPGA
//
// Revision 3.0  2006/07/20 21:15:48  geurts
// *** empty log message ***
//
// Revision 2.27  2006/07/13 15:52:22  mey
// New Parser structure
//
// Revision 2.26  2006/07/13 15:46:37  mey
// New Parser strurture
//
// Revision 2.25  2006/07/12 07:58:18  mey
// Update
//
// Revision 2.24  2006/07/11 12:46:43  mey
// UPdate
//
// Revision 2.23  2006/07/11 09:31:12  mey
// Update
//
// Revision 2.22  2006/07/06 07:31:48  mey
// MPC firmware loading added
//
// Revision 2.21  2006/07/05 09:29:18  mey
// Update
//
// Revision 2.20  2006/07/04 15:29:31  mey
// Update
//
// Revision 2.19  2006/06/20 12:25:37  mey
// Update
//
// Revision 2.18  2006/06/19 14:17:58  mey
// Update
//
// Revision 2.17  2006/06/16 13:05:24  mey
// Got rid of Compiler switches
//
// Revision 2.16  2006/05/10 23:57:24  liu
// // Update for Production Controller with firmware 3.59
//
// Revision 2.15  2006/03/23 12:41:41  mey
// UPdate
//
// Revision 2.14  2006/03/10 13:13:13  mey
// Jinghua's changes
//
// Revision 2.11  2006/01/31 08:52:29  mey
// UPdate
//
// Revision 2.10  2006/01/23 15:01:34  mey
// Update
//
// Revision 2.9  2006/01/23 15:00:10  mey
// Update
//
// Revision 2.8  2006/01/14 22:25:09  mey
// UPdate
//
// Revision 2.7  2006/01/12 11:48:17  mey
// Update
//
// Revision 2.6  2006/01/12 11:32:50  mey
// Update
//
// Revision 2.5  2005/12/08 12:00:32  mey
// Update
//
// Revision 2.4  2005/12/06 15:32:09  mey
// Fix bug
//
// Revision 2.3  2005/12/02 18:12:19  mey
// get rid of D360
//
// Revision 2.2  2005/11/21 19:42:28  mey
// Update
//
// Revision 2.1  2005/11/02 16:16:43  mey
// Update for new controller
//
// Revision 1.23  2004/07/22 18:52:38  tfcvs
// added accessor functions for DCS integration
//
//
//----------------------------------------------------------------------
#include "VMEModule.h"
#include "VMEController.h"
#include "Crate.h"
#include "CrateSetup.h"
#include <cmath>
#include <string>
#include <stdio.h>
#include <iostream>
#include <unistd.h> // read and write

#ifndef debugV //silent mode
#define PRINT(x) 
#define PRINTSTRING(x)  
#else //verbose mode
#define PRINT(x) cout << #x << ":\t" << x << endl; 
#define PRINTSTRING(x) cout << #x << endl; 
#endif



VMEModule::VMEModule(Crate * theCrate, int newslot): 
  theSlot(newslot), csc_(0x0)
{
  theCrate_ = theCrate;
  //
#ifdef debugV
  cout << "creating VMEModule in crate " << theCrate->number() << endl;
#endif 
  theController = theCrate->vmeController();
  theCrate->addModule(this);
}


void VMEModule::start() {
  PRINTSTRING(OVAL: start() from VMEModule have been called...);
  //
#ifdef debugV
  cout << "starting VMEModule for slot " << dec << theSlot << " boardType " << boardType() << " line " << (int) c << endl;
#endif  
  theController->start( theSlot, boardType() );
}


void VMEModule::end() {
}

 
void VMEModule::endDevice() {
  theController->end();
}

void VMEModule::do_vme(char fcn, char vme, 
                       const char *snd,char *rcv, int when) {
   theController->start( theSlot, boardType() );
   theController->do_vme(fcn, vme, snd, rcv, when);
}

void VMEModule::devdo(DEVTYPE dev,int ncmd,const char *cmd,int nbuf,
                     const char *inbuf,char *outbuf,int irdsnd) {
  //printf("VMEModule::devdo\n");
  theController->start( theSlot, boardType() );
  theController->devdo(dev, ncmd, cmd, nbuf, inbuf, outbuf, irdsnd);
}

void VMEModule::scan(int reg,const char *snd,int cnt,char *rcv,int ird) {
  theController->start( theSlot, boardType() );
  if(boardType()==TMB_ENUM )
    theController->scan_jtag(reg, snd, cnt, rcv, ird);
  else if (boardType()==MPC_ENUM )
    theController->scan_jtag(reg, snd, cnt, rcv, ird);
  else if (boardType()==CCB_ENUM )
    theController->scan_jtag(reg, snd, cnt, rcv, ird);
  else
    theController->scan(reg, snd, cnt, rcv, ird);
}

void VMEModule::RestoreIdle() {
  theController->start( theSlot, boardType() );
  if(boardType()==TMB_ENUM)
    theController->RestoreIdle_alct();
  if(boardType()==MPC_ENUM)
    theController->RestoreIdle_jtag();
}

void VMEModule::RestoreReset() {
  theController->start( theSlot, boardType() );
  if(boardType()==MPC_ENUM)
    theController->RestoreReset_jtag();
}

void VMEModule::SetupJtag() {
  //
  if(boardType()==TMB_ENUM){
    //theController->SetupJtagBaseAddress(0x70000);
    theController->SetupJtagBaseAddress(0x10);
    theController->SetupTCK(2);
    theController->SetupTMS(1);
    theController->SetupTDI(0);
    theController->SetupTDO(7);
  }
  if(boardType()==MPC_ENUM){
    theController->SetupJtagBaseAddress(0x0);
    theController->SetupTCK(7);
    theController->SetupTMS(6);
    theController->SetupTDI(5);
    theController->SetupTDO(0);
  }
  if(boardType()==CCB_ENUM){
    theController->SetupJtagBaseAddress(0x0);
    theController->SetupTCK(7);
    theController->SetupTMS(6);
    theController->SetupTDI(5);
    theController->SetupTDO(0);
  }
  //
  //
}

void VMEModule::InitJTAG(int port) {
  theController->start( theSlot, boardType() );
  theController->InitJTAG(port);
}

void VMEModule::CloseJTAG() {
  theController->start( theSlot, boardType() );
  theController->CloseJTAG();
  theController->end();
}


VMEController* VMEModule::getTheController(){
  return theController;
}

bool VMEModule::exist(){
  return theController->exist( theSlot );
}

int VMEModule::svfLoad(int *jch, const char *fn, int db )
{
  int MAXBUFSIZE=8200;
  unsigned char snd[MAXBUFSIZE], rcv[MAXBUFSIZE], expect[MAXBUFSIZE],rmask[MAXBUFSIZE],smask[MAXBUFSIZE],cmpbuf[MAXBUFSIZE];
  unsigned char sndbuf[MAXBUFSIZE],rcvbuf[MAXBUFSIZE], realsnd[MAXBUFSIZE];
  unsigned char sndhdr[MAXBUFSIZE],sndtdr[MAXBUFSIZE], sndhir[MAXBUFSIZE], sndtir[MAXBUFSIZE];
  unsigned char hdrsmask[MAXBUFSIZE],tdrsmask[MAXBUFSIZE], hirsmask[MAXBUFSIZE], tirsmask[MAXBUFSIZE];
  int rcvword;
  FILE *dwnfp;
  char buf[MAXBUFSIZE], buf2[256];
  //  char buf[8192],buf2[256];
  char *Word[256],*lastn;
  const char *downfile;
  //char fStop;
  int jchan;
  unsigned char sndvalue;
  fpos_t ftdi_pos, fsmask_pos;
  unsigned char send_tmp;//, rcv_tmp;
  int i,j,Count,nbytes,nbits,nframes,step_mode,pause;
  int hdrbits = 0, tdrbits = 0, hirbits = 0, tirbits = 0;
  int hdrbytes = 0, tdrbytes = 0, hirbytes = 0, tirbytes = 0; 
  int nowrit, cmpflag, errcntr;
  static int count;
  // MvdM struct JTAG_BBitStruct   driver_data;
  // int jtag_chain[4] = {1, 0, 5, 4};
  //int jtag_chain_tmb[6] = {7, 6, 9, 8, 3, 1};
  // === SIR Go through SelectDRScan->SelectIRScan->CaptureIR->ShiftIR  
  //char tms_pre_sir[4]={ 1, 1, 0, 0 }; 
  char tdi_pre_sir[4]={ 0, 0, 0, 0 };
  // === SDR Go through SelectDRScan->CaptureDR->ShiftDR
  // char tms_pre_sdr[3]={ 1, 0, 0 };
  char tdi_pre_sdr[3]={ 0, 0, 0 };
  // === SDR,SIR Go to RunTestIdle after scan
  // char tms_post[4]={ 0, 1, 1, 0 };
  char tdi_post[4]={ 0, 0, 0, 0 };
  int total_packages ;
  int send_packages ;
  
  total_packages = 0 ;
  send_packages = 0 ;
  jchan = *jch;
  downfile = fn;
  errcntr = 0;
  if (downfile==NULL)    downfile="default.svf";
  
  dwnfp    = fopen(downfile,"r");
  while (fgets(buf,256,dwnfp) != NULL) 
    {
      memcpy(buf,buf,256);
      Parse(buf, &Count, &(Word[0]));
      if( strcmp(Word[0],"SDR")==0 ) total_packages++ ;
    }
  fclose(dwnfp) ;
  
  printf("=== Programming Design with %s through JTAG chain %d\n",downfile, jchan);  
  printf("=== Have to send %d DATA packages \n",total_packages) ;
  dwnfp    = fopen(downfile,"r");
  
  if (dwnfp == NULL)
    {
      perror(downfile);
      
      return -1;
    }
  
  this->start(); 
  
  count=0; 
  nowrit=1;
  step_mode=0;
  while (fgets(buf,256,dwnfp) != NULL)  
    {
      if((buf[0]=='/'&&buf[1]=='/')||buf[0]=='!')
	{
	  if (db>4)          printf("%s",buf);
	}
      else 
	{
	  if(strrchr(buf,';')==0)
	    {
	      lastn=strrchr(buf,'\r');
	      if(lastn!=0)lastn[0]='\0';
	      lastn=strrchr(buf,'\n');
	      if(lastn!=0)lastn[0]='\0';
	      memcpy(buf2,buf,256);
	      Parse(buf2, &Count, &(Word[0]));
	      if(( strcmp(Word[0],"SDR")==0) || (strcmp(Word[0],"SIR")==0))
		{
		  sscanf(Word[1],"%d",&nbits);
		  if (nbits>MAXBUFSIZE) // === Handle Big Bitstreams
		    {
		      //(*MyOutput_) << "EMUjtag. nbits larger than buffer size" << std::endl;
		    }
		  else do  // == Handle Normal Bitstreams
		    {
		      lastn=strrchr(buf,'\r');
		      if(lastn!=0)lastn[0]='\0';
		      lastn=strrchr(buf,'\n');
		      if(lastn!=0)lastn[0]='\0';
		      if (fgets(buf2,256,dwnfp) != NULL)
			{
			  strcat(buf,buf2);
			}
		      else 
			{
			  if (db)              printf("End of File encountered.  Quiting\n");
			  return -1;
			}
		    }
		  while (strrchr(buf,';')==0);
		}
	    } 
	  bzero(snd, sizeof(snd));
	  bzero(cmpbuf, sizeof(cmpbuf));
	  bzero(sndbuf, sizeof(sndbuf));
	  bzero(rcvbuf, sizeof(rcvbuf));
	  
	  /*
	    for(i=0;i<MAXBUFSIZE;i++)
	    {
	    snd[i]=0;
	    cmpbuf[i]=0;
	    sndbuf[i]=0;
	    rcvbuf[i]=0;
	    }
	  */
	  Parse(buf, &Count, &(Word[0]));
	  count=count+1;
	  cmpflag=0;
	  // ==================  Parsing commands from SVF file ====================
	  // === Handling HDR ===
	  if(strcmp(Word[0],"HDR")==0)
	    {
	      sscanf(Word[1],"%d",&hdrbits);
	      hdrbytes=(hdrbits)?(hdrbits-1)/8+1:0;
	    if (db)	  
	      printf("Sending %d bits of Header Data\n", hdrbits);
	    // if (db>3)          printf("HDR: Num of bits - %d, num of bytes - %d\n",hdrbits,hdrbytes);
	    for(i=2;i<Count;i+=2)
	      {
		if(strcmp(Word[i],"TDI")==0)
		  {
		    for(j=0;j<hdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(hdrbytes-j-1)+1],"%2X",(int *)&sndhdr[j]);
			// printf("%2X",sndhdr[j]);
		      }
		    // printf("\n%d\n",nbytes);
    		  }
      		if(strcmp(Word[i],"SMASK")==0)
		  {
		    for(j=0;j<hdrbytes;j++)
		      {
      		  	sscanf(&Word[i+1][2*(hdrbytes-j-1)+1],"%2X",(int *)&hdrsmask[j]);
		      }
		  }
     	 	if(strcmp(Word[i],"TDO")==0)
		  {
		    //if (db>2)             cmpflag=1;
		    cmpflag=1;
		    for(j=0;j<hdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(hdrbytes-j-1)+1],"%2X",(int *)&expect[j]);
		      }
		  }
      		if(strcmp(Word[i],"MASK")==0)
		  {
		    for(j=0;j<hdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(hdrbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		      }
		  }
	      }
	    }
	  
	  // === Handling HIR ===
	  else if(strcmp(Word[0],"HIR")==0)
	    {
	      // for(i=0;i<3;i++)sndbuf[i]=tdi_pre_sdr[i];
	      // cmpflag=1;    //disable the comparison for no TDO SDR
	      sscanf(Word[1],"%d",&hirbits);
	      hirbytes=(hirbits)?(hirbits-1)/8+1:0;
	      if (db)	  
		printf("Sending %d bits of Header Data\n", hirbits);
	      // if (db>3)          printf("HIR: Num of bits - %d, num of bytes - %d\n",hirbits,hirbytes);
	      for(i=2;i<Count;i+=2)
		{
		  if(strcmp(Word[i],"TDI")==0)
		  {
		    for(j=0;j<hirbytes;j++)
		      {
			sscanf(&Word[i+1][2*(hirbytes-j-1)+1],"%2X",(int *)&sndhir[j]);
			// printf("%2X",sndhir[j]);
		      }
		    // printf("\n%d\n",nbytes);
    		  }
		  if(strcmp(Word[i],"SMASK")==0)
		    {
		      for(j=0;j<hirbytes;j++)
		      {
      		  	sscanf(&Word[i+1][2*(hirbytes-j-1)+1],"%2X",(int *)&hirsmask[j]);
		      }
		    }
		  if(strcmp(Word[i],"TDO")==0)
		    {
		      //if (db>2)             cmpflag=1;
		      cmpflag=1;
		      for(j=0;j<hirbytes;j++)
			{
			  sscanf(&Word[i+1][2*(hirbytes-j-1)+1],"%2X",(int *)&expect[j]);
			}
		    }
		  if(strcmp(Word[i],"MASK")==0)
		    {
		      for(j=0;j<hirbytes;j++)
			{
			  sscanf(&Word[i+1][2*(hirbytes-j-1)+1],"%2X",(int *)&rmask[j]);
			}
		    }
		}
	    }	
	  
	  // === Handling TDR ===
	  else if(strcmp(Word[0],"TDR")==0)
	    {
	      // for(i=0;i<3;i++)sndbuf[i]=tdi_pre_sdr[i];
	      // cmpflag=1;    //disable the comparison for no TDO SDR
	      sscanf(Word[1],"%d",&tdrbits);
	      tdrbytes=(tdrbits)?(tdrbits-1)/8+1:0;
	      if (db)	  
		printf("Sending %d bits of Tailer Data\n", tdrbits);
	      // if (db>3)          printf("TDR: Num of bits - %d, num of bytes - %d\n",tdrbits,tdrbytes);
	      for(i=2;i<Count;i+=2)
	      {
		if(strcmp(Word[i],"TDI")==0)
		  {
		    for(j=0;j<tdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tdrbytes-j-1)+1],"%2X",(int *)&sndtdr[j]);
			// printf("%2X",sndhir[j]);
		      }
		    // printf("\n%d\n",nbytes);
    		  }
      		if(strcmp(Word[i],"SMASK")==0)
		  {
		    for(j=0;j<tdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tdrbytes-j-1)+1],"%2X",(int *)&tdrsmask[j]);
		      }
		  }
		if(strcmp(Word[i],"TDO")==0)
		  {
		    //if (db>2)             cmpflag=1;
		    cmpflag=1;
		    for(j=0;j<tdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tdrbytes-j-1)+1],"%2X",(int *)&expect[j]);
		      }
		  }
      		if(strcmp(Word[i],"MASK")==0)
		  {
		    for(j=0;j<tdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tdrbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		      }	
		  }
	      }
	    }
	  
	  // === Handling TDR ===
	  else if(strcmp(Word[0],"TIR")==0)
	  {
	    // for(i=0;i<3;i++)sndbuf[i]=tdi_pre_sdr[i];
	    // cmpflag=1;    //disable the comparison for no TDO SDR
	    sscanf(Word[1],"%d",&tirbits);
	    tirbytes=(tirbits)?(tirbits-1)/8+1:0;
	    if (db)	  
	      printf("Sending %d bits of Tailer Data\n", tdrbits);
	    // if (db>3)          printf("TIR: Num of bits - %d, num of bytes - %d\n",tirbits,tirbytes);
	    for(i=2;i<Count;i+=2)
	      {
		if(strcmp(Word[i],"TDI")==0)
		  {
		    for(j=0;j<tirbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tirbytes-j-1)+1],"%2X",(int *)&sndtir[j]);
			    // printf("%2X",sndhir[j]);
		      }
		    // printf("\n%d\n",nbytes);
    		  }
      		if(strcmp(Word[i],"SMASK")==0)
		  {
		    for(j=0;j<tirbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tirbytes-j-1)+1],"%2X",(int *)&tirsmask[j]);
		      }
		  }
		if(strcmp(Word[i],"TDO")==0)
		  {
		    //if (db>2)             cmpflag=1;
		    cmpflag=1;
		    for(j=0;j<tirbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tirbytes-j-1)+1],"%2X",(int *)&expect[j]);
		      }
		  }
      		if(strcmp(Word[i],"MASK")==0)
		  {
		    for(j=0;j<tirbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tirbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		      }
		  }
	      }
	  }
	  // === Handling SDR ===
	  else if(strcmp(Word[0],"SDR")==0)
	    {
	      //std::cout << "SDR" << std::endl;
	      for(i=0;i<3;i++)sndbuf[i]=tdi_pre_sdr[i];
	      // cmpflag=1;    //disable the comparison for no TDO SDR
	    sscanf(Word[1],"%d",&nbits);
	    nbytes=(nbits)?(nbits-1)/8+1:0;
	    if (db)	  printf("Sending %d bits Data\n", nbits);
	    // if (db>3)          printf("SDR: Num of bits - %d, num of bytes - %d\n",nbits,nbytes);
	    for(i=2;i<Count;i+=2)
	      {
	      if(strcmp(Word[i],"TDI")==0)
		{
		  for(j=0;j<nbytes;j++)
		    {
		      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&snd[j]);
		      //                printf("%2X",snd[j]);
		    }
		  //                printf("\n%d\n",nbytes);
		}
	      if(strcmp(Word[i],"SMASK")==0)
		{
		  for(j=0;j<nbytes;j++)
		    {
		      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&smask[j]);
		    }
		}
	      if(strcmp(Word[i],"TDO")==0)
		{
		  //if (db>2)             cmpflag=1;
		  cmpflag=1;
		  for(j=0;j<nbytes;j++)
		    {
		      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&expect[j]);
		    }
		}
	      if(strcmp(Word[i],"MASK")==0)
		{
		  for(j=0;j<nbytes;j++)
		    {
		      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		    }
		}
	      }
	    for(i=0;i<nbytes;i++)
	      {
	      send_tmp = snd[i]&smask[i];
	      for(j=0;j<8;j++)
		{
		  if ((i*8+j)< nbits) 
		    { 
		      sndbuf[i*8+j+3]=send_tmp&0x01; 
		    }
		  send_tmp = send_tmp >> 1;
		}
	      }
	    for(i=0;i<4;i++)sndbuf[nbits+3]=tdi_post[i];         
	    nframes=nbits+7;
          // Put send SDR here
	    for (i=0; i< ((hdrbits+nbits+tdrbits-1)/8+1); i++)
	      realsnd[i] = 0;
	    if (hdrbytes>0) {
	      for (i=0;i<hdrbytes;i++)
		realsnd[i]=sndhdr[i];
	    }
	    for (i=0;i<nbits;i++)
	    realsnd[(i+hdrbits)/8] |= (snd[i/8] >> (i%8)) << ((i+hdrbits)%8);
	    if (tdrbytes>0) {
	      for (i=0;i<tdrbits;i++)
		realsnd[(i+hdrbits+nbits)/8] |= (sndtdr[i/8] >> (i%8)) << ((i+hdrbits+nbits)%8);
	    }	    
	    //
	    {	
	      printf("SDR Sent Data:\n");
	      for (i=0; i< ((hdrbits+nbits+tdrbits-1)/8+1); i++) 
		printf("%02X",realsnd[i]);
	      printf("\n");
	      //
	      printf("SDR Readback Data:\n");
	      for (i=0; i< ((hdrbits+nbits+tdrbits)); i++) 
		printf("%02X",rcv[i]);
	      printf("\n");
	    }		    
	    //
	    send_packages++ ;
	    printf("%c[0m", '\033');
	    printf("%c[1m", '\033');
	    //printf("%c[2;50H", '\033');
	    if ( send_packages == 1 )   {
	      printf("%c7", '\033');
	    }
	    printf("%c8", '\033'); 
	    printf(" Sending %d/%d ",send_packages,total_packages) ;
	    printf("%c8", '\033'); 
	    printf("%c[0m", '\033');
	    if ( send_packages == total_packages ) printf("\n") ;
	  //
	  this->scan(DATA_REG, (char*)realsnd, hdrbits+nbits+tdrbits, (char*)rcv, 2); 
	  //
	  if (cmpflag==1)
	    {     
	      /*
		for(i=0;i<nbytes;i++)
		{
		rcv_tmp = 0;
		for(j=0;j<8;j++)
		{
		if ((i*8+j) < nbits)
		{
		rcv_tmp |= ((rcvbuf[i*8+j+3]<<7) &0x80); 
		}
		rcv_tmp = rcv_tmp >> (j<7);
		
		}	
		rcv[nbytes-1-i] = rcv_tmp;		
		}
	      */
	      for(i=0;i<nbytes;i++)
		{
		  rcvword = rcv[i+(hdrbits/8)]+(((int)rcv[i+1+(hdrbits/8)])<<8);
		  rcvword = rcvword>>(hdrbits%8);
		  rcvword = rcv[i];
		  // if (((rcv[nbytes-1-i]^expect[i]) & (rmask[i]))!=0 && cmpflag==1)
		  if ((((rcvword&0xFF)^expect[i]) & (rmask[i]))!=0 && cmpflag==1)
		    {
		      //printf("1.read back wrong, at i %02d  rdbk %02X  expect %02X  rmask %02X\n",i,rcv[i]&0xFF,expect[i]&0xFF,rmask[i]&0xFF);
		      errcntr++;
		    }
		}	
	    }
	  /*         if (cmpflag==1)
		     {
		     for(i=3;i<nbits+3;i++) printf("%d",rcvbuf[i]);
		     printf("\n");
		     }
	  */       
          }
        // === Handling SIR ===
        else if(strcmp(Word[0],"SIR")==0)
          {
	    for(i=0;i<4;i++)sndbuf[i]=tdi_pre_sir[i];
	    // cmpflag=1;    //disable the comparison for no TDO SDR
	    sscanf(Word[1],"%d",&nbits);
	    nbytes=(nbits)?(nbits-1)/8+1:0;
	    if (db)	  printf("Sending %d bits of Command\n",nbits);
	    // if (db>3)          printf("SIR: Num of bits - %d, num of bytes - %d\n",nbits,nbytes);
	    for(i=2;i<Count;i+=2)
	      {
		if(strcmp(Word[i],"TDI")==0)
		  {
		    for(j=0;j<nbytes;j++)
		      {
			sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&snd[j]);
		      }
		  }
		if(strcmp(Word[i],"SMASK")==0)
		  {
		    for(j=0;j<nbytes;j++)
		      {
			sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&smask[j]);
		      }
		  }
		if(strcmp(Word[i],"TDO")==0)
		  {
			cmpflag=1;
			// if (db>2)              cmpflag=1;
			for(j=0;j<nbytes;j++)
			  {
			    sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&expect[j]);
			  }
		  }
		if(strcmp(Word[i],"MASK")==0)
		  {
		    for(j=0;j<nbytes;j++)
		      {
			sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		      }
		  }
	      }
	    for(i=0;i<nbytes;i++)
	      {
		send_tmp = snd[i]&smask[i];
		// printf("\n%d - ", send_tmp);
		for(j=0;j<8;j++)
		  {
		    if ((i*8+j)< nbits) 
		      {
			sndbuf[i*8+j+4]=send_tmp&0x01;
			// printf("%d", sndbuf[i*8+j+4]);
		      }
		    send_tmp = send_tmp >> 1;
		  }
	      }
	    for(i=0;i<4;i++)sndbuf[nbits+4]=tdi_post[i];
	    nframes=nbits+8;
	    // Put send SIR here
	    for (i=0; i< ((hirbits+nbits+tirbits-1)/8+1);  i++)
	      realsnd[i] = 0;
	    if (hirbytes>0) {
	      for (i=0;i<hirbytes;i++)
		realsnd[i]=sndhir[i];
	    }
	    for (i=0;i<nbits;i++)
	      realsnd[(i+hirbits)/8] |= (snd[i/8] >> (i%8)) << ((i+hirbits)%8);
	    if (tirbytes>0) {
	      for (i=0;i<tirbits;i++)
		realsnd[(i+hirbits+nbits)/8] |= (sndtir[i/8] >> (i%8)) << ((i+hirbits+nbits)%8);
	    }
	    //
	    this->scan(INSTR_REG, (char*)realsnd, hirbits+nbits+tirbits, (char*)rcv, 2); 
	    //	   
	    { 	printf("SIR Send Data:\n");
	    for (i=0; i< ((hirbits+nbits+tirbits-1)/8+1);  i++)
	      printf("%02X",realsnd[i]);
	    printf("\n");
	    printf("Readback: \n");
	    for (i=0; i< ((hirbits+nbits+tirbits));  i++)
	      printf("%02X ",rcv[i]);
	    printf("\n");
	    }
	    //
	    if (cmpflag==1)
	      {
		/*               for(i=0;i<nbytes;i++)
				 {
				 rcv_tmp = 0;
				 for(j=0;j<8;j++)
				 {
				 if ((i*8+j) < nbits)
				 {
				 rcv_tmp |= ((rcvbuf[i*8+j+4]<<7) &0x80);
				 }
				 rcv_tmp = rcv_tmp >> (j<7);
				 
				 }
				 rcv[nbytes-1-i] = rcv_tmp;
				 }
		*/
		if (db>4){                printf("SIR Readback Data:\n");
                //for(i=0;i<nbytes;i++) printf("%02X",rcv[i]);
		for (i=0; i< ((hdrbits+nbits+tdrbits-1)/8+1); i++) printf("%02X",rcv[i]);
                printf("\n");
		}
		
                for(i=0;i<nbytes;i++)
		  {
		    rcvword = rcv[i+(hirbits/8)<<3]+(((int)rcv[i+1+(hirbits/8)])<<2)+(((int)rcv[i+2+(hirbits/8)])<<1)+(((int)rcv[i+3+(hirbits/8)]));
		    printf("%02x %02x\n",rcv[i+(hirbits/8)],(((int)rcv[i+1+(hirbits/8)])<<8));
		    printf("hirbits=%d %02x\n",hirbits,rcvword);
		    //rcvword = rcv[i];
		    // if (((rcv[nbytes-1-i]^expect[i]) & (rmask[i]))!=0 && cmpflag==1)
		    if ((((rcvword&0xFF)^expect[i]) & (rmask[i]))!=0 && cmpflag==1)
		      {
			printf("2.read back wrong, at i %02d  rdbk %02X  expect %02X  rmask %02X\n",i,rcv[i]&0xFF,expect[i]&0xFF,rmask[i]&0xFF);
                	errcntr++;
		      }
		  }
	      }
	    /*
	      if (cmpflag==1)
	      {
	      for(i=4;i<nbits+4;i++) printf("%d",rcvbuf[i]);
	      printf("\n");
	      }
	    */   
          }
	  // === Handling RUNTEST ===
	  else if(strcmp(Word[0],"RUNTEST")==0)
	    {
	      sscanf(Word[1],"%d",&pause);
	      //printf("RUNTEST:  %d\n",pause);
	      usleep(pause+100);
	      // InsertDelayJTAG(pause,MYMICROSECONDS);
	    }
	  // === Handling STATE ===
	  else if((strcmp(Word[0],"STATE")==0)&&(strcmp(Word[1],"RESET")==0))
	    {
	      if(strcmp(Word[2],"IDLE;")==0)
		{
		  cout << "STATE: goto reset idle state" << std::endl;
		  RestoreIdle();
		} 
	    }
	  else if((strcmp(Word[0],"STATE")==0)&&(strcmp(Word[1],"RESET;")==0))
	    {
	      cout << "STATE: goto reset state" << std::endl;
	      RestoreReset();
	    }
	  else if(strcmp(Word[0],"TRST")==0)
	    {
	      //          printf("TRST\n");
	    }
	  // === Handling ENDIR ===
	  else if(strcmp(Word[0],"ENDIR")==0)
	    {
	      //          printf("ENDIR\n");
	    }
	  // === Handling ENDDR ===
	  else if(strcmp(Word[0],"ENDDR")==0)
	    {
	    //	   printf("ENDDR\n");
	    }
	}
    }
  this->endDevice();
  fclose(dwnfp);
  return errcntr; 
}


// ====
// SVF File Parser module
// ====
void VMEModule::Parse(char *buf,int *Count,char **Word)
{

  //std::cout << buf << std::endl;

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
  buf = '\0';
}


