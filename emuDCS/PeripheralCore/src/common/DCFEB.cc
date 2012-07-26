//-----------------------------------------------------------------------
// $Id: DCFEB.cc,v 1.2 2012/07/26 07:43:52 ahart Exp $
// $Log: DCFEB.cc,v $
// Revision 1.2  2012/07/26 07:43:52  ahart
// Increase sleeps during firmware programming and add functions for loading timing constants to the EPROM.
//
// Revision 1.1  2012/07/12 12:59:17  ahart
//
// First commit of new files for the DCFEB.
//
//
//
//-----------------------------------------------------------------------
#include "emu/pc/DCFEB.h"
#include "emu/pc/VMEController.h"
#include "emu/pc/geom.h"

namespace emu {
  namespace pc {

const int DCFEB::layers[6] = {2,0,4,5,3,1};


DCFEB::DCFEB(Crate * theCrate,int newslot,int number, std::vector<BuckeyeChip> buckeyes):
  VMEModule(theCrate, newslot, number),
  EmuLogger(),
  number_(number),
  buckeyeChips_(buckeyes)
{
}


DCFEB::DCFEB(Crate * theCrate,int newslot,int number) :
  VMEModule(theCrate, newslot, number),
  EmuLogger(),
  number_(number)
{
  printf(" DCFEB 2nd Instantiator\n");fflush(stdout);
  for(unsigned i = 0; i < 6; ++i) {
    buckeyeChips_.push_back(BuckeyeChip(i));
  }
  printf(" number %d \n",number);fflush(stdout);
  printf(" return from Instantiator\n");fflush(stdout);
}

DCFEB::~DCFEB(){
  printf(" DCFEB destroyed \n");
}


char DCFEB::chipMask() const {
  char chip_mask=0;
  for(unsigned ichip=0;ichip<buckeyeChips_.size();ichip++){
    int i = buckeyeChips_[ichip].number();
    chip_mask |= (1<<(5-i));
  }
  return chip_mask;
}

unsigned long int DCFEB::febpromuser()
{ 
  unsigned long int ibrd;
  DEVTYPE dv = dpromDevice();
  printf("%d \n",dv);
  cmd[0]=PROM_USERCODE;
  sndbuf[0]=0xFF;
  sndbuf[1]=0xFF;
  sndbuf[2]=0xFF;
  sndbuf[3]=0xFF;
  sndbuf[4]=0xFF;
  for (int i=0;i<3;i++) {
    devdo(dv,8,cmd,33,sndbuf,rcvbuf,1);
    rcvbuf[0]=((rcvbuf[0]>>1)&0x7f)+((rcvbuf[1]<<7)&0x80);
    rcvbuf[1]=((rcvbuf[1]>>1)&0x7f)+((rcvbuf[2]<<7)&0x80);
    rcvbuf[2]=((rcvbuf[2]>>1)&0x7f)+((rcvbuf[3]<<7)&0x80);
    rcvbuf[3]=((rcvbuf[3]>>1)&0x7f)+((rcvbuf[4]<<7)&0x80);
    ibrd=unpack_ibrd();
    cmd[0]=PROM_BYPASS;
    sndbuf[0]=0;
    devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
    usleep(100);
    if (((0xff&rcvbuf[0])!=0xff)||((0xff&rcvbuf[1])!=0xff)||
        ((0xff&rcvbuf[2])!=0xff)||((0xff&rcvbuf[3])!=0xff)) return ibrd;
  }
  return ibrd;
}

unsigned long int  DCFEB::febpromid()
{
  DEVTYPE dv = dpromDevice();
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
  unsigned long int ibrd=unpack_ibrd();
  cmd[0]=PROM_BYPASS;
  sndbuf[0]=0;
  devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
  return ibrd;
}

unsigned long int DCFEB::febfpgauser()
{
  unsigned long int ibrd;
  DEVTYPE dv = dscamDevice();
  printf(" dscanDevice dv %d \n",dv);
  cmd[0]=(VTX6_USERCODE&0xff);
  cmd[1]=((VTX6_USERCODE&0x300)>>8);
  sndbuf[0]=0xFF;
  sndbuf[1]=0xFF;
  sndbuf[2]=0xFF;
  sndbuf[3]=0xFF;
  devdo(dv,10,cmd,32,sndbuf,rcvbuf,1);
  // RPW not sure about this
  printf(" The FPGA USERCODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
  ibrd = unpack_ibrd();
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  sndbuf[0]=0;
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,0);  
  usleep(100);
  return ibrd;
}

unsigned long int DCFEB::febfpgaid()
{
  DEVTYPE dv = dscamDevice();
  printf(" febfgpaid: dv %d ",dv);
  cmd[0]=(VTX6_IDCODE&0xff);
  cmd[1]=((VTX6_IDCODE&0x300)>>8);
  sndbuf[0]=0xFF;
  sndbuf[1]=0xFF;
  sndbuf[2]=0xFF;
  sndbuf[3]=0xFF;
  devdo(dv,10,cmd,32,sndbuf,rcvbuf,1);
  (*MyOutput_) << " The FEB " << dv-F1DCFEBM+1 << "FPGA Chip should be 610093 (last 6 digits) "  << std::endl;
  (*MyOutput_) << " The FPGA Chip IDCODE is " << std::hex << 
    (0xff&rcvbuf[3]) << (0xff&rcvbuf[2]) << (0xff&rcvbuf[1]) << (0xff&rcvbuf[0]) << std::endl;
  // RPW not sure about this
  unsigned long ibrd = unpack_ibrd();
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  sndbuf[0]=0;
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,0);
  return ibrd;
}

unsigned long int DCFEB::unpack_ibrd() const {
  int ibrd=0x00000000;
  return ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)
    |((rcvbuf[3]&0xff)<<24)|ibrd;
}


unsigned int DCFEB::unpack_ival() const {
  return ((rcvbuf[1]<<8)&0xff00)|(rcvbuf[0]&0xff);
}

void DCFEB::fxpreblkend(int dword){
    DEVTYPE dv = dscamDevice();
    cmd[0]=(VTX6_USR1&0xff);
    cmd[1]=((VTX6_USR1&0x300)>>8);    
    sndbuf[0]=PREBLKEND;
    devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
    cmd[0]=(VTX6_USR2&0xff);
    cmd[1]=((VTX6_USR2&0x300)>>8);
    //  default preblkend is state 5
    sndbuf[0]=dword&0x0F; 
    devdo(dv,10,cmd,4,sndbuf,rcvbuf,0);
    cmd[0]=(VTX6_BYPASS&0xff);
    cmd[1]=((VTX6_BYPASS&0x300)>>8);
    sndbuf[0]=0;
    devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
} 

void DCFEB::LctL1aDelay(int dword){
  DEVTYPE dv = dscamDevice();
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);
  sndbuf[0]=LCTL1ADELAY;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=(char)VTX6_USR2;
  cmd[1]=((VTX6_USR2&0x300)>>8);
  // 
  sndbuf[0]=dword&0x03; 
  devdo(dv,10,cmd,2,sndbuf,rcvbuf,0);
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  sndbuf[0]=0;
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
}

void DCFEB::restoreCFEBIdle(){
  DEVTYPE dv=dscamDevice();   
  devdo(dv,-1,cmd,0,sndbuf,rcvbuf,2);
}

void  DCFEB::set_comp_mode(int dword)
{
      DEVTYPE dv = dscamDevice();
      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=COMP_MODE;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=(VTX6_USR2&0xff);
      cmd[1]=((VTX6_USR2&0x300)>>8);
      // default is 0x17
      sndbuf[0]=dword&0XFF;
      devdo(dv,10,cmd,5,sndbuf,rcvbuf,0);
      cmd[0]=(VTX6_BYPASS&0xff);
      cmd[1]=((VTX6_BYPASS&0x300)>>8);
      sndbuf[0]=0;
      devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
  (*MyOutput_) << "calling set_comp_mode " << dword << std::endl;
}

void DCFEB::set_comp_thresh_bc(float thresh)
{
char dt[2];
// 
/* digitize voltages */
// 
 int dthresh=int(4095*((3.5-thresh)/3.5)); 
 dt[0]=0;
 dt[1]=0;
 for(int i=0;i<8;i++){
   dt[0]|=((dthresh>>(i+7))&1)<<(7-i);
   dt[1]|=((dthresh>>i)&1)<<(6-i);
 }
 dt[0]=((dt[1]<<7)&0x80) + ((dt[0]>>1)&0x7f);
 dt[1]=dt[1]>>1;
 //
   DEVTYPE dv = FADCFEBM;
   cmd[0]=(VTX6_USR1&0xff);
   cmd[1]=((VTX6_USR1&0x300)>>8);
   sndbuf[0]=COMP_DAC;
   devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
   cmd[0]=(VTX6_USR2&0xff);
   cmd[1]=((VTX6_USR2&0x300)>>8);
   sndbuf[0]=dt[0];
   sndbuf[1]=dt[1];
   sndbuf[2]=0x00; 
   devdo(dv,10,cmd,15,sndbuf,rcvbuf,0);
   cmd[0]=(VTX6_USR1&0xff);
   cmd[1]=((VTX6_USR1&0x300)>>8);
   sndbuf[0]=NOOP;
   devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
   cmd[0]=(VTX6_BYPASS&0xff);
   cmd[1]=((VTX6_BYPASS&0x300)>>8);
   devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
   usleep(20);
 }
//

void DCFEB::set_comp_thresh(float thresh)
{
char dt[2];
// 
/* digitize voltages */
// 
 int dthresh=int(4095*((3.5-thresh)/3.5)); 
 dt[0]=0;
 dt[1]=0;
 for(int i=0;i<8;i++){
   dt[0]|=((dthresh>>(i+7))&1)<<(7-i);
   dt[1]|=((dthresh>>i)&1)<<(6-i);
 }
 dt[0]=((dt[1]<<7)&0x80) + ((dt[0]>>1)&0x7f);
 dt[1]=dt[1]>>1;
 //
 (*MyOutput_) << "Set_comp_thresh.icfeb=" << number() << " thresh=" << thresh << std::endl;
 //
 DEVTYPE dv = dscamDevice();
 //
 (*MyOutput_) << "cfeb= " << number() << std::endl;
 (*MyOutput_) << "dv= " << dv << std::endl;
 //
 cmd[0]=(VTX6_USR1&0xff);
 cmd[1]=((VTX6_USR1&0x300)>>8);
 sndbuf[0]=COMP_DAC;
 devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
 cmd[0]=(VTX6_USR2&0xff);
 cmd[1]=((VTX6_USR2&0x300)>>8);
 sndbuf[0]=dt[0];
 sndbuf[1]=dt[1];
 sndbuf[2]=0x00; 
 devdo(dv,10,cmd,15,sndbuf,rcvbuf,0);
 cmd[0]=(VTX6_USR1&0xff);
 cmd[1]=((VTX6_USR1&0x300)>>8);
 sndbuf[0]=NOOP;
 devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
 cmd[0]=(VTX6_BYPASS&0xff);
 cmd[1]=((VTX6_BYPASS&0x300)>>8);
 devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
 usleep(20);
}
//
void DCFEB::epromload_verify(const char *downfile,int writ,char *cbrdnum)
{
  // obsolete routine - no prom path in DCFEB                                                                                 
  // return;
 
  //
  std::cout << "New epromload GUJH " << std::endl;
  //
  char snd[1024],expect[1024],rmask[1024],smask[1024],cmpbuf[1024];
  DEVTYPE devstp,devnum;
  char *devstr;
  FILE *dwnfp,*fpout;
  char buf[10000],buf2[256];
  char *Word[256],*lastn;
  int Count,j,nbits,nbytes,pause,xtrbits;
  int tmp,cmpflag;
  int tstusr;
  int nowrit=0;
  //
  DEVTYPE dv = dscamDevice();
  devnum=dv; 
  (*MyOutput_) << " epromload board " <<number() << std::endl;
  (*MyOutput_) << " devnum    " << devnum << std::endl;
  // 

    devstp=devnum;

#ifdef OSUcc
  theController->SetUseDelay(true);
#endif
  //
  for(int i=devnum;i<=devstp;i++){
    dv=(DEVTYPE)i;
    xtrbits=geo[dv].sxtrbits;
    //    printf(" ************************** xtrbits %d geo[dv].sxtrbits %d \n",xtrbits,geo[dv].sxtrbits);
    devstr=geo[dv].nam;
    dwnfp    = fopen(downfile,"r");
    fpout=fopen("/tmp/eprom.bit","w");
    chmod("/tmp/eprom.bit",S_IRUSR| S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    //  printf("Programming Design %s (%s) with %s\n",design,devstr,downfile);
    //
    char bogobuf[8192];
    unsigned long int nlines=0;
    unsigned long int line=1;
    FILE *bogodwnfp=fopen(downfile,"r");
    while (fgets(bogobuf,256,bogodwnfp) != NULL)
      if (strrchr(bogobuf,';')!=0) nlines++;
    float percent;
    while (fgets(buf,256,dwnfp) != NULL)  {
      percent = (float)line/(float)nlines;
      if ((line%200)==0) printf("<   > Processed line %lu of %lu (%.1f%%)\n",line,nlines,percent*100.0);
      // printf("%s \n",buf); //lsd print
      if((buf[0]=='/'&&buf[1]=='/')||buf[0]=='!'){
	//  printf("%s",buf);
      }
      else {
	line++;
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
       for(int i=0;i<1024;i++){
	 cmpbuf[i]=0;
	 sndbuf[i]=0;
	 rcvbuf[i]=0;
       }
       dParse(buf, &Count, &(Word[0]));
       // count=count+1;
       if(strcmp(Word[0],"SDR")==0){
	 cmpflag=0;    //disable the comparison for no TDO SDR
	 sscanf(Word[1],"%d",&nbits);
	 std::cout << " SIR nbits " << nbits << std::endl;
	 nbytes=(nbits-1)/8+1;
	 for(int i=2;i<Count;i+=2){
           if(strcmp(Word[i],"TDI")==0){
	     for(j=0;j<nbytes;j++){
	       sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&snd[j]);
	     }
	     if(nowrit==1&&((cbrdnum[0]!=0)||(cbrdnum[1]!=0))) {
	       tstusr=0;
	       snd[0]=cbrdnum[0];
	       snd[1]=cbrdnum[1];
	       snd[2]=cbrdnum[2];
	       snd[3]=cbrdnum[3];
	     }
	     if(nowrit==1){
	       //  printf(" snd %02x %02x %02x %02x \n",snd[0],snd[1],snd[2],snd[3]);
	       //FOO[0]=((snd[3]&0x000000ff)<<24)|((snd[2]&0x000000ff)<<16)|((snd[1]&0x000000ff)<<8)|(snd[0]&0x000000ff);
               // printf(" FOO %08x \n",FOO[0]);
	     }
	   }
	   if(strcmp(Word[i],"SMASK")==0){
	     for(j=0;j<nbytes;j++){
                sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&smask[j]);
	     }
	   }
	   if(strcmp(Word[i],"TDO")==0){
	     cmpflag=1;
	     for(j=0;j<nbytes;j++){
	       sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&expect[j]);
              }
	   }
	   if(strcmp(Word[i],"MASK")==0){
	     for(j=0;j<nbytes;j++){
	       sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&rmask[j]);
	     }
	   }
	 }
	 for(int i=0;i<nbytes;i++){
	   //sndbuf[i]=snd[i]&smask[i];
	   sndbuf[i]=snd[i]&0xff;
          }
	 //   printf("D%04d",nbits+xtrbits);
          // for(i=0;i<(nbits+xtrbits)/8+1;i++)printf("%02x",sndbuf[i]&0xff);printf("\n");
	 //         printf(" geo[dv].jchan %d \n \n",(geo[dv].jchan));
	 //         sleep(10);
 	 if(nowrit==0){
             if((geo[dv].jchan==11)){
	       scan_reset(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
	       // printf(" Emergency load SDR \n");
             }else{
                   if(nbits==4096){
                        scan(DATA_REG,sndbuf,nbits+xtrbits+1,rcvbuf,1);
                   }else{
                        scan(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,1);
                   }
             }
	 }else{
	   if(writ==1) {
	     if((geo[dv].jchan==11)){
	       scan_reset(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
	     }else{ 
	       if(nbits==4096){
		 scan(DATA_REG,sndbuf,nbits+xtrbits+1,rcvbuf,1);
	       }else{
		 scan(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,1);
	       }
	     }
	   }
	 } 
	 //  Data readback comparison here:
	 std::cout << " Data readback ntypes "<<nbytes<<std::endl;
         if (cmpflag!=20) {
	   for (int i=0;i<nbytes;i++) {
             printf("%02x",rcvbuf[i]&0xff);
	     fprintf(fpout," %02X",rcvbuf[i]&0xFF);
	     if (i%4==3)printf("\n");
             if (i%4==3)fprintf(fpout,"\n");
	   }
           printf("\n");
	 }
         if(nbytes==512){
	   rmask[511]=rmask[511]&0x7f; //do not compare bit 7 for the very last byte
	 for (int i=0;i<nbytes;i++) {
	   printf("%02x",rcvbuf[i]&0xff);
	   //   tmp=(rcvbuf[i]>>3)&0x1F;
	   //  rcvbuf[i]=tmp | (rcvbuf[i+1]<<5&0xE0); 
              tmp=(rcvbuf[i]>>1)&0x7F;
	      rcvbuf[i]=tmp | (rcvbuf[i+1]<<7&0x80);

	  if (((rcvbuf[i]^expect[i]) & (rmask[i]))!=0 && cmpflag==1) 
		printf("\n GU *** read back wrong, at i %02d  rdbk %02X  expect %02X  rmask %02X\n",i,rcvbuf[i]&0xFF,expect[i]&0xFF,rmask[i]&0xFF); 
	 }
         printf("\n");
	 /* if (cmpflag!=20) {
	   for (int i=0;i<nbytes;i++) {
             printf("%02x",rcvbuf[i]&0xff);
	     fprintf(fpout," %02X",rcvbuf[i]&0xFF);
	     if (i%4==3) fprintf(fpout,"\n");
	   }
           printf("\n");
	   }*/
         }
       } else if(strcmp(Word[0],"SIR")==0){

	 nowrit=0;
	 sscanf(Word[1],"%d",&nbits);
	 std::cout << " SIR nbits " << nbits << std::endl;
          nbytes=(nbits-1)/8+1;
          for(int i=2;i<Count;i+=2){
            if(strcmp(Word[i],"TDI")==0){
              for(j=0;j<nbytes;j++){
                sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&snd[j]);
              }
              if(nbytes==1){if(0xfd==(snd[0]&0xff))nowrit=1;} // nowrit=1  
            }
            else if(strcmp(Word[i],"SMASK")==0){
              for(j=0;j<nbytes;j++){
                sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&smask[j]);
              }
            }
            if(strcmp(Word[i],"TDO")==0){
              for(j=0;j<nbytes;j++){
                sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&expect[j]);
              }
            }
            else if(strcmp(Word[i],"MASK")==0){
              for(j=0;j<nbytes;j++){
                sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&rmask[j]);
              }
            }
          }
          for(int i=0;i<nbytes;i++){
            //sndbuf[i]=snd[i]&smask[i];
            sndbuf[i]=snd[i];
          }
          if(nowrit==0){
  	      devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);
	  }
          else{
            if(writ==1) {devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);}
            if(writ==0)printf(" ***************** nowrit %02x \n",sndbuf[0]);
          }
	  /*
          printf("send %2d instr bits %02X %02X %02X %02X %02X\n",nbits,sndbuf[4]&0xFF,sndbuf[3]&0xFF,sndbuf[2]&0xFF,sndbuf[1]&0xFF,sndbuf[0]&0xFF);
          printf("expect %2d instr bits %02X %02X %02X %02X %02X\n",nbits,expect[4]&0xFF,expect[3]&0xFF,expect[2]&0xFF,expect[1]&0xFF,expect[0]&0xFF);
	  */
        }
        else if(strcmp(Word[0],"RUNTEST")==0){
          sscanf(Word[1],"%d",&pause);
	  printf("RUNTEST = %d\n",pause);
	  //usleep(pause);
	  /*   ipd=83*pause;
          // sleep(1);
          t1=(double) clock()/(double) CLOCKS_PER_SEC;
          for(i=0;i<ipd;i++);
          t2=(double) clock()/(double) CLOCKS_PER_SEC;
	  //  if(pause>1000)printf("pause = %f s  while erasing\n",t2-t1); */
	  //          for (i=0;i<pause/100;i++)
	  //  devdo(dv,-1,sndbuf,0,sndbuf,rcvbuf,2);
          pause=pause;
	  if (pause<100000) pause=2*pause+100;
          if (pause>65535) {
            sndbuf[0]=255;
            sndbuf[1]=255;
            for (int looppause=0;looppause<pause/65536;looppause++) {
              devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,1);
              usleep(65535);}
            pause=65535;
	  }
          sndbuf[0]=pause-(pause/256)*256;
          sndbuf[1]=pause/256;
	  // printf(" sndbuf %d %d %d \n",sndbuf[1],sndbuf[0],pause);
          devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,1);
	  usleep(pause);
          // printf(" send sleep \n");  
	  /* printf("pause      %d us\n",pause);*/
	  //#ifdef OSUcc
	  //theController->flush_vme();
	  //#endif OSUcc
        }
        else if((strcmp(Word[0],"STATE")==0)&&(strcmp(Word[1],"RESET")==0)&&(strcmp(Word[2],"IDLE;")==0)){
	  //  printf("goto reset idle state\n"); 
	   devdo(dv,-1,sndbuf,0,sndbuf,rcvbuf,2);
	   //#ifdef OSUcc
	   //theController->flush_vme();
	   //#endif OSUcc
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
}

void DCFEB::epromload(const char *downfile,int writ,char *cbrdnum)
{
  //
  std::cout << "New epromload" << std::endl;
  //
  char snd[8192],expect[8192],rmask[8192],smask[8192],cmpbuf[8192];
  DEVTYPE devstp,devnum;
  char *devstr;
  FILE *dwnfp,*fpout;
  char buf[25000],buf2[256];
  char *Word[256],*lastn;
  int Count,j,nbits,nbytes,pause,xtrbits;
  int tmp,cmpflag;
  int tstusr;
  int nowrit=0;
  //
  DEVTYPE dv = dscamDevice();
  devnum=dv;
  devstp=devnum;
 
  (*MyOutput_) << " epromload board " << number() << std::endl;
  (*MyOutput_) << " devnum    " << devnum << std::endl;
  //
  //
#ifdef OSUcc
  theController->SetUseDelay(true);
#endif
  //
  for(int i=devnum;i<=devstp;i++){
    dv=(DEVTYPE)i;
    xtrbits=geo[dv].sxtrbits;
    //    printf(" ************************** xtrbits %d geo[dv].sxtrbits %d \n",xtrbits,geo[dv].sxtrbits);
    devstr=geo[dv].nam;
    dwnfp    = fopen(downfile,"r");
    if(dwnfp==NULL) std::cout << "Can't open firmware file " << downfile << std::endl;
    fpout=fopen("/tmp/eprom.bit","w");
    chmod("/tmp/eprom.bit",S_IRUSR| S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if(fpout==NULL) std::cout << "Can't open eprom.bit file"<< std::endl;
    
    printf("Programming Design %s with %s\n",devstr,downfile);
    //
    char bogobuf[8192];
    unsigned long int nlines=0;
    unsigned long int line=1;
    FILE *bogodwnfp=fopen(downfile,"r");
    while (fgets(bogobuf,256,bogodwnfp) != NULL)
      if (strrchr(bogobuf,';')!=0) nlines++;
    fclose(bogodwnfp);
    float percent;
    while (fgets(buf,256,dwnfp) != NULL)  {
      percent = (float)line/(float)nlines;
      if ((line%20)==0) printf("<   > Processed line %lu of %lu (%.1f%%)\n",line,nlines,percent*100.0);
      // printf(" %s \n",buf);
      // fflush(stdout);
      if((buf[0]=='/'&&buf[1]=='/')||buf[0]=='!'){
	// printf("%s",buf);
      }
      else {
	line++;
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
       for(int i=0;i<8192;i++){
	 cmpbuf[i]=0;
	 sndbuf[i]=0;
	 rcvbuf[i]=0;
       }
       dParse(buf, &Count, &(Word[0]));
       // count=count+1;
       if(strcmp(Word[0],"SDR")==0){
	 cmpflag=0;    //disable the comparison for no TDO SDR
	 sscanf(Word[1],"%d",&nbits);
	 nbytes=(nbits-1)/8+1;
	 for(int i=2;i<Count;i+=2){
	   if(strcmp(Word[i],"TDI")==0){
	     for(j=0;j<nbytes;j++){
	       sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&snd[j]);
	     }
	     if(nowrit==1&&((cbrdnum[0]!=0)||(cbrdnum[1]!=0))) {
	       tstusr=0;
	       snd[0]=cbrdnum[0];
	       snd[1]=cbrdnum[1];
	       snd[2]=cbrdnum[2];
	       snd[3]=cbrdnum[3];
	     }
	     if(nowrit==1){
	       //  printf(" snd %02x %02x %02x %02x \n",snd[0],snd[1],snd[2],snd[3]);
	       //FOO[0]=((snd[3]&0x000000ff)<<24)|((snd[2]&0x000000ff)<<16)|((snd[1]&0x000000ff)<<8)|(snd[0]&0x000000ff);
               // printf(" FOO %08x \n",FOO[0]);
	     }
	   }
	   if(strcmp(Word[i],"SMASK")==0){
	     for(j=0;j<nbytes;j++){
                sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&smask[j]);
	     }
	   }
	   if(strcmp(Word[i],"TDO")==0){
	     cmpflag=1;
	     for(j=0;j<nbytes;j++){
	       sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&expect[j]);
              }
	   }
	   if(strcmp(Word[i],"MASK")==0){
	     for(j=0;j<nbytes;j++){
	       sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&rmask[j]);
	     }
	   }
	 }
	 for(int i=0;i<nbytes;i++){
	   //sndbuf[i]=snd[i]&smask[i];
	   sndbuf[i]=snd[i]&0xff;
          }
	 //   printf("D%04d",nbits+xtrbits);
          // for(i=0;i<(nbits+xtrbits)/8+1;i++)printf("%02x",sndbuf[i]&0xff);printf("\n");
	 //         printf(" geo[dv].jchan %d \n \n",(geo[dv].jchan));
	 //         sleep(10);
 	 if(nowrit==0){
             if((geo[dv].jchan==11)){
	       scan_reset(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
	       // printf(" Emergency load SDR \n");
             }else{
                   scan(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
             }
	 }else{
	   if(writ==1) {
	     if((geo[dv].jchan==11)){
	       scan_reset(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
	     }else{ 
	       scan(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
	     }
	   }
	 } 
	 //  Data readback comparison here:
	 for (int i=0;i<nbytes;i++) {
	   tmp=(rcvbuf[i]>>3)&0x1F;
	   rcvbuf[i]=tmp | (rcvbuf[i+1]<<5&0xE0);
	   /*  if (((rcvbuf[i]^expect[i]) & (rmask[i]))!=0 && cmpflag==1) 
		printf("read back wrong, at i %02d  rdbk %02X  expect %02X  rmask %02X\n",i,rcvbuf[i]&0xFF,expect[i]&0xFF,rmask[i]&0xFF); */
	 }
	 if (cmpflag==1) {
	   for (int i=0;i<nbytes;i++) {
	     fprintf(fpout," %02X",rcvbuf[i]&0xFF);
	     if (i%4==3) fprintf(fpout,"\n");
	   }
	 }
       }
       //
       else if(strcmp(Word[0],"SIR")==0){
	 nowrit=0;
	 sscanf(Word[1],"%d",&nbits);
          nbytes=(nbits-1)/8+1;
          for(int i=2;i<Count;i+=2){
            if(strcmp(Word[i],"TDI")==0){
              for(j=0;j<nbytes;j++){
                sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&snd[j]);
              }
              if(nbytes==1){if(0xfd==(snd[0]&0xff))nowrit=1;} // nowrit=1  
            }
            else if(strcmp(Word[i],"SMASK")==0){
              for(j=0;j<nbytes;j++){
                sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&smask[j]);
              }
            }
            if(strcmp(Word[i],"TDO")==0){
              for(j=0;j<nbytes;j++){
                sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&expect[j]);
              }
            }
            else if(strcmp(Word[i],"MASK")==0){
              for(j=0;j<nbytes;j++){
                sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&rmask[j]);
              }
            }
          }
          for(int i=0;i<nbytes;i++){
            //sndbuf[i]=snd[i]&smask[i];
            sndbuf[i]=snd[i];
          }
          if(nowrit==0){
  	      devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);
	  }
          else{
            if(writ==1) {devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);}
            if(writ==0)printf(" ***************** nowrit %02x \n",sndbuf[0]);
          }
	  /*
          printf("send %2d instr bits %02X %02X %02X %02X %02X\n",nbits,sndbuf[4]&0xFF,sndbuf[3]&0xFF,sndbuf[2]&0xFF,sndbuf[1]&0xFF,sndbuf[0]&0xFF);
          printf("expect %2d instr bits %02X %02X %02X %02X %02X\n",nbits,expect[4]&0xFF,expect[3]&0xFF,expect[2]&0xFF,expect[1]&0xFF,expect[0]&0xFF);
	  */
        }
        else if(strcmp(Word[0],"RUNTEST")==0){
          sscanf(Word[1],"%d",&pause);
          if((devnum==F1DCFEBM)||(devnum==F2DCFEBM)||(devnum==F3DCFEBM)||(devnum==F4DCFEBM)||(devnum==F5DCFEBM)||(devnum==FADCFEBM)){printf(" send runtest\n");fflush(stdout);SendRUNTESTClks(pause);}else{
	  // printf("RUNTEST = %d\n",pause);
	  //usleep(pause);
	  /*   ipd=83*pause;
          // sleep(1);
          t1=(double) clock()/(double) CLOCKS_PER_SEC;
          for(i=0;i<ipd;i++);
          t2=(double) clock()/(double) CLOCKS_PER_SEC;
	  //  if(pause>1000)printf("pause = %f s  while erasing\n",t2-t1); */
	  //          for (i=0;i<pause/100;i++)
	  //  devdo(dv,-1,sndbuf,0,sndbuf,rcvbuf,2);
            pause=pause;
	    if (pause<100000) pause=2*pause+100;
            if (pause>65535) {
              sndbuf[0]=255;
              sndbuf[1]=255;
              for (int looppause=0;looppause<pause/65536;looppause++) {
                devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,1);
                usleep(65535);}
              pause=65535;
	    }
            sndbuf[0]=pause-(pause/256)*256;
            sndbuf[1]=pause/256;
	    // printf(" sndbuf %d %d %d \n",sndbuf[1],sndbuf[0],pause);
            devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,1);
	    usleep(pause);
            // printf(" send sleep \n");  
	    /* printf("pause      %d us\n",pause);*/
	    //#ifdef OSUcc
	    //theController->flush_vme();
	    //#endif OSUcc
          }
        }
       //        else if((strcmp(Word[0],"STATE")==0)&&(strcmp(Word[1],"RESET")==0)&&(strcmp(Word[2],"IDLE;")==0)){
        else if(strcmp(Word[0],"STATE")==0){
          printf(" found STATE look for reset %d x%sx x%sx \n",Count,Word[0],Word[1]);
	  // devdo(dv,-1,sndbuf,0,sndbuf,rcvbuf,2);
	  for(int i=1;i<Count;i++){
	    if(strcmp(Word[i],"RESET;")==0){
              printf(" found RESET %d \n",i);
    	      printf("goto reset idle state\n"); 
	      devdo(dv,-1,sndbuf,0,sndbuf,rcvbuf,2);
            }
	  } 
	   //#ifdef OSUcc
	   //theController->flush_vme();
	   //#endif OSUcc
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
  //
  //#ifdef OSUcc
  //theController->flush_vme();
  //#endif OSUcc
  //
  theController->send_last();
  //
#ifdef OSUcc
  theController->SetUseDelay(false);
#endif
  //
  //sndbuf[0]=0x01;
  //sndbuf[1]=0x00;
  // printf(" sndbuf %d %d %d \n",sndbuf[1],sndbuf[0],pause);
  //devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,1);
  //
}


void DCFEB::dParse(char *buf,int *Count,char **Word)
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



void DCFEB::DCFEB_loopback(){
  nwbuf=100;
  for(int i=0;i<100;i++){
    wbuf[i]=i&0xff;
  }
  eth_write();
}

void DCFEB::DCFEB_Firmware_Simulation(){

      DEVTYPE dv = dscamDevice();
     //int brdn=cfebItr->number();
      printf(" DCFEBM device %d \n",dv);

      /*  while(1){
      for(int i=0;i<17;i++){
      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=i;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      }
      usleep(100000);
      usleep(100000);
      }*/


      while(1){ 
      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=0x0c;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=(VTX6_USR2&0xff);
      cmd[1]=((VTX6_USR2&0x300)>>8);
      sndbuf[1]=0x0f;
      sndbuf[0]=0xff;
      devdo(dv,10,cmd,12,sndbuf,rcvbuf,0);
      // cmd[0]=(VTX6_BYPASS&0xff);
      // cmd[1]=((VTX6_BYPASS&0x300)>>8);
      //devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
      //  devdo(dv,-1,cmd,0,sndbuf,rcvbuf,2);



      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=0x05;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=(VTX6_USR2&0xff);
      cmd[1]=((VTX6_USR2&0x300)>>8);
      sndbuf[1]=0x00;
      sndbuf[0]=0x02;
      devdo(dv,10,cmd,2,sndbuf,rcvbuf,0);
      // cmd[0]=(VTX6_BYPASS&0xff);
      // cmd[1]=((VTX6_BYPASS&0x300)>>8);
      // devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
      // devdo(dv,-1,cmd,0,sndbuf,rcvbuf,2);

      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=0x08;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=(VTX6_USR2&0xff);
      cmd[1]=((VTX6_USR2&0x300)>>8);
      sndbuf[1]=0x00;
      sndbuf[0]=0x03;
      devdo(dv,10,cmd,4,sndbuf,rcvbuf,0);
      //cmd[0]=(VTX6_BYPASS&0xff);
      //cmd[1]=((VTX6_BYPASS&0x300)>>8);
      //devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
      // devdo(dv,-1,cmd,0,sndbuf,rcvbuf,2);

      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=0x09;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=(VTX6_USR2&0xff);
      cmd[1]=((VTX6_USR2&0x300)>>8);
      sndbuf[1]=0x00;
      sndbuf[0]=0x09;
      devdo(dv,10,cmd,5,sndbuf,rcvbuf,0);
      //cmd[0]=(VTX6_BYPASS&0xff);
      //cmd[1]=((VTX6_BYPASS&0x300)>>8);
      //devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
      // devdo(dv,-1,cmd,0,sndbuf,rcvbuf,2);
      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=0x07;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=(VTX6_USR2&0xff);
      cmd[1]=((VTX6_USR2&0x300)>>8);
      sndbuf[1]=0x06;
      sndbuf[0]=0x93;
      devdo(dv,10,cmd,11,sndbuf,rcvbuf,0);
      //cmd[0]=(VTX6_BYPASS&0xff);
      //cmd[1]=((VTX6_BYPASS&0x300)>>8);
      //devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);

      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=0x0a;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=(VTX6_USR2&0xff);
      cmd[1]=((VTX6_USR2&0x300)>>8);
      sndbuf[1]=0x00;
      sndbuf[0]=0x08;
      devdo(dv,10,cmd,6,sndbuf,rcvbuf,0);
      //cmd[0]=(VTX6_BYPASS&0xff);
      //cmd[1]=((VTX6_BYPASS&0x300)>>8);
      //devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
      //  devdo(dv,-1,cmd,0,sndbuf,rcvbuf,2);

      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=0x0b;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=(VTX6_USR2&0xff);
      cmd[1]=((VTX6_USR2&0x300)>>8);
      sndbuf[5]=0x3f;
      sndbuf[4]=0x78;
      sndbuf[3]=0xd2;
      sndbuf[2]=0xbc;
      sndbuf[1]=0x9a;
      sndbuf[0]=0xe1;
      devdo(dv,10,cmd,48,sndbuf,rcvbuf,0);
      // cmd[0]=(VTX6_BYPASS&0xff);
      //cmd[1]=((VTX6_BYPASS&0x300)>>8);
      //devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
      //  devdo(dv,-1,cmd,0,sndbuf,rcvbuf,2);

      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=0x03;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=(VTX6_USR2&0xff);
      cmd[1]=((VTX6_USR2&0x300)>>8);
      sndbuf[5]=0x00;
      sndbuf[4]=0x00;
      sndbuf[3]=0x00;
      sndbuf[2]=0x00;
      sndbuf[1]=0x00;
      sndbuf[0]=0x00;
      devdo(dv,10,cmd,32,sndbuf,rcvbuf,0);
      //cmd[0]=(VTX6_BYPASS&0xff);
      //cmd[1]=((VTX6_BYPASS&0x300)>>8);
      //devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
      //   devdo(dv,-1,cmd,0,sndbuf,rcvbuf,2);

      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=0x0a;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=(VTX6_USR2&0xff);
      cmd[1]=((VTX6_USR2&0x300)>>8);
      sndbuf[1]=0x00;
      sndbuf[0]=0x3f;
      devdo(dv,10,cmd,6,sndbuf,rcvbuf,0);
      //cmd[0]=(VTX6_BYPASS&0xff);
      //cmd[1]=((VTX6_BYPASS&0x300)>>8);
      //devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
      // devdo(dv,-1,cmd,0,sndbuf,rcvbuf,2);

      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=0x0b;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=(VTX6_USR2&0xff);
      cmd[1]=((VTX6_USR2&0x300)>>8);
      int j=0;
      for(int k=0;k<6;k++){
	for(int l=0;l<6;l++){
          if(k==0)sndbuf[j]=0x66;
          if(k==1)sndbuf[j]=0x55;
          if(k==2)sndbuf[j]=0x44;
          if(k==3)sndbuf[j]=0x33;
          if(k==4)sndbuf[j]=0x22;
          if(k==5)sndbuf[j]=0x11;
          j++;
        }
      } 
      devdo(dv,10,cmd,6*48,sndbuf,rcvbuf,0);
      //cmd[0]=(VTX6_BYPASS&0xff);
      //cmd[1]=((VTX6_BYPASS&0x300)>>8);
      //devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
      //devdo(dv,-1,cmd,0,sndbuf,rcvbuf,2);

      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=0x04;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=(VTX6_USR2&0xff);
      cmd[1]=((VTX6_USR2&0x300)>>8);
      sndbuf[1]=0x03;
      sndbuf[0]=0x5b;
      devdo(dv,10,cmd,12,sndbuf,rcvbuf,0);
      //cmd[0]=(VTX6_BYPASS&0xff);
      //cmd[1]=((VTX6_BYPASS&0x300)>>8);
      //devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
      // devdo(dv,-1,cmd,0,sndbuf,rcvbuf,2);

      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=0x0e;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=(VTX6_USR2&0xff);
      cmd[1]=((VTX6_USR2&0x300)>>8);
      sndbuf[5]=0x00;
      sndbuf[4]=0x00;
      sndbuf[3]=0x01;
      sndbuf[2]=0x2a;
      sndbuf[1]=0x05;
      sndbuf[0]=0x06;
      devdo(dv,10,cmd,26,sndbuf,rcvbuf,0);
      // cmd[0]=(VTX6_BYPASS&0xff);
      //cmd[1]=((VTX6_BYPASS&0x300)>>8);
      //devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
      //  devdo(dv,-1,cmd,0,sndbuf,rcvbuf,2);  

      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=0x0e;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=(VTX6_USR2&0xff);
      cmd[1]=((VTX6_USR2&0x300)>>8);
      sndbuf[5]=0x00;
      sndbuf[4]=0x00;
      sndbuf[3]=0x02;
      sndbuf[2]=0x00;
      sndbuf[1]=0x55;
      sndbuf[0]=0x55;
      devdo(dv,10,cmd,26,sndbuf,rcvbuf,0);
      // cmd[0]=(VTX6_BYPASS&0xff);
      //cmd[1]=((VTX6_BYPASS&0x300)>>8);
      // devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
      // devdo(dv,-1,cmd,0,sndbuf,rcvbuf,2);

      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=0x0e;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=(VTX6_USR2&0xff);
      cmd[1]=((VTX6_USR2&0x300)>>8);
      sndbuf[5]=0x00;
      sndbuf[4]=0x00;
      sndbuf[3]=0x03;
      sndbuf[2]=0xe2;
      sndbuf[1]=0x55;
      sndbuf[0]=0x55;
      devdo(dv,10,cmd,26,sndbuf,rcvbuf,0);
      //cmd[0]=(VTX6_BYPASS&0xff);
      //cmd[1]=((VTX6_BYPASS&0x300)>>8);
      //devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
      // devdo(dv,-1,cmd,0,sndbuf,rcvbuf,2);

      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=0x0d;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      //cmd[0]=(VTX6_BYPASS&0xff);
      //cmd[1]=((VTX6_BYPASS&0x300)>>8);
      //devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
      // devdo(dv,-1,cmd,0,sndbuf,rcvbuf,2);

      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=0x10;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=(VTX6_USR2&0xff);
      cmd[1]=((VTX6_USR2&0x300)>>8);
      sndbuf[1]=0x00;
      sndbuf[0]=0x44;
      devdo(dv,10,cmd,9,sndbuf,rcvbuf,0);
      //cmd[0]=(VTX6_BYPASS&0xff);
      //cmd[1]=((VTX6_BYPASS&0x300)>>8);
      //devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
      //devdo(dv,-1,cmd,0,sndbuf,rcvbuf,2);

      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=0x0F;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      //cmd[0]=(VTX6_BYPASS&0xff);
      //cmd[1]=((VTX6_BYPASS&0x300)>>8);
      //devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
      // devdo(dv,-1,cmd,0,sndbuf,rcvbuf,2);
        usleep(100000);
        usleep(100000);
      }
}

void DCFEB::FADC_SetMask(short int mask){
    DEVTYPE dv = dscamDevice();
    std::cout << "Setting dv= " << dv << " to " << std::dec << mask << std::endl;
    cmd[0]=(VTX6_USR1&0xff);
    cmd[1]=((VTX6_USR1&0x300)>>8);
    sndbuf[0]=FADC_MASK;
    devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
    cmd[0]=(VTX6_USR2&0xff);
    cmd[1]=((VTX6_USR2&0x300)>>8);
    sndbuf[0]=(mask&0xFF);
    sndbuf[1]=((mask&0x0F00)>>8);
    devdo(dv,10,cmd,12,sndbuf,rcvbuf,0);
    cmd[0]=(VTX6_BYPASS&0xff);
    cmd[1]=((VTX6_BYPASS&0x300)>>8);
    sndbuf[0]=0;
    devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
}

void DCFEB::FADC_Initialize(){
    DEVTYPE dv = dscamDevice();
    std::cout << "Setting dv= " << dv << " to " << std::dec << NOOP << std::endl;
    cmd[0]=(VTX6_USR1&0xff);
    cmd[1]=((VTX6_USR1&0x300)>>8);
    sndbuf[0]=FADC_INIT;
    devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
    cmd[0]=(VTX6_USR2&0xff);
    cmd[1]=((VTX6_USR2&0x300)>>8);
    sndbuf[0]=NOOP;
    devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
    cmd[0]=(VTX6_BYPASS&0xff);
    cmd[1]=((VTX6_BYPASS&0x300)>>8);
    sndbuf[0]=0;
    devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
}

void DCFEB::FADC_ShiftData(unsigned long int bits){
    DEVTYPE dv = dscamDevice();
    std::cout << "Setting dv= " << dv << " to " << std::dec << bits << std::endl;
    cmd[0]=(VTX6_USR1&0xff);
    cmd[1]=((VTX6_USR1&0x300)>>8);
    sndbuf[0]=FADC_SHIFT;
    devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
    cmd[0]=(VTX6_USR2&0xff);
    cmd[1]=((VTX6_USR2&0x300)>>8);
    sndbuf[0]=(bits&0x000000FF);
    sndbuf[1]=((bits&0x0000FF00)>>8);
    sndbuf[2]=((bits&0x00FF0000)>>16);
    sndbuf[3]=((bits&0x03000000)>>24);
    devdo(dv,10,cmd,26,sndbuf,rcvbuf,0);
    //    sndbuf[0]=NOOP;
    //    devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
    cmd[0]=(VTX6_BYPASS&0xff);
    cmd[1]=((VTX6_BYPASS&0x300)>>8);
    sndbuf[0]=0;
    devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
}

void DCFEB::Set_PipelineDepth(short int depth){
    DEVTYPE dv = dscamDevice();
    std::cout << "Setting dv= " << dv << " to " << std::dec << depth << std::endl;
    cmd[0]=(VTX6_USR1&0xff);
    cmd[1]=((VTX6_USR1&0x300)>>8);
    sndbuf[0]=Pipeline_Depth;
    devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
    for(int i=0;i<20;i++)rcvbuf[i]=0x55;
    cmd[0]=(VTX6_USR2&0xff);
    cmd[1]=((VTX6_USR2&0x300)>>8);
    sndbuf[0]=(depth&0xFF);
    sndbuf[1]=((depth&0x0100)>>8);
    // sndbuf[0]=0xff; sndbuf[1]=0xff;
    devdo(dv,10,cmd,9,sndbuf,rcvbuf,1);
    // printf(" shifted pipeline length %02x %02x %02x %02x %02x %02x \n",rcvbuf[5]&0xff,rcvbuf[4]&0xff,rcvbuf[3]&0xff,rcvbuf[2]&0xff,rcvbuf[1]&0xff,rcvbuf[0]&0xff);
    cmd[0]=(VTX6_BYPASS&0xff);
    cmd[1]=((VTX6_BYPASS&0x300)>>8);
    sndbuf[0]=0;
    devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
}

void DCFEB::Pipeline_Restart(){
    DEVTYPE dv = dscamDevice();
    std::cout << " pipeline restart  dv= " << dv << " to " << std::dec << std::endl;
    cmd[0]=(VTX6_USR1&0xff);
    cmd[1]=((VTX6_USR1&0x300)>>8);
    sndbuf[0]=Pipeline_Restrt;
    devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
    printf(" Pipeline Restarted \n");
    cmd[0]=(VTX6_BYPASS&0xff);
    cmd[1]=((VTX6_BYPASS&0x300)>>8);
    sndbuf[0]=0;
    devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
}

void DCFEB::Set_NSAMPLE(int nsample){
    DEVTYPE dv = dscamDevice();
    std::cout << "Setting dv= " << dv << " to " << std::dec << nsample << std::endl;
    cmd[0]=(VTX6_USR1&0xff);
    cmd[1]=((VTX6_USR1&0x300)>>8);
    sndbuf[0]=Pipeline_NSAMPLE;
    devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
    for(int i=0;i<20;i++)rcvbuf[i]=0x55;
    cmd[0]=(VTX6_USR2&0xff);
    cmd[1]=((VTX6_USR2&0x300)>>8);
    sndbuf[0]=(nsample&0x7F);
    sndbuf[1]=((nsample&0x0000)>>8);
    // sndbuf[0]=0xff; sndbuf[1]=0xff;                                                                                                                                 
    devdo(dv,10,cmd,7,sndbuf,rcvbuf,1);
    // printf(" shifted pipeline length %02x %02x %02x %02x %02x %02x \n",rcvbuf[5]&0xff,rcvbuf[4]&0xff,rcvbuf[3]&0xff,rcvbuf[2]&0xff,rcvbuf[1]&0xff,rcvbuf[0]&0xff);  
    cmd[0]=(VTX6_BYPASS&0xff);
    cmd[1]=((VTX6_BYPASS&0x300)>>8);
    sndbuf[0]=0;
    devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
}


void DCFEB::JTAGPipeline_read(char *rawdata){
    DEVTYPE dv = dscamDevice();
    cmd[0]=(VTX6_USR1&0xff);
    cmd[1]=((VTX6_USR1&0x300)>>8);
    sndbuf[0]=Pipeline_Read;
    devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
    for(int i=0;i<24;i++)rcvbuf[i]=0xF0;
    cmd[0]=(VTX6_USR2&0xff);
    cmd[1]=((VTX6_USR2&0x300)>>8);
    devdo(dv,10,cmd,192,sndbuf,rcvbuf,1);
    for(int i=0;i<24;i++){
      rawdata[i]=rcvbuf[i];
    }
    for(int i=0;i<24;i++)printf("%02x ",rcvbuf[23-i]&0xff);printf("\n");
}


void DCFEB::GetDCFEBData(){
    DEVTYPE dv = dscamDevice();
    for(int sample=0;sample<NSAMPLE;sample++){
      for(int chip=0;chip<6;chip++){
	// for(int chip=0;chip<1;chip++){     // single chip read mod 
    char rawdata[24];
    cmd[0]=(VTX6_USR1&0xff);
    cmd[1]=((VTX6_USR1&0x300)>>8);
    sndbuf[0]=Pipeline_Read;
    devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
    for(int i=0;i<24;i++)rcvbuf[i]=0xF0;
    cmd[0]=(VTX6_USR2&0xff);
    cmd[1]=((VTX6_USR2&0x300)>>8);
    devdo(dv,10,cmd,192,sndbuf,rcvbuf,1);
    for(int i=0;i<24;i++){
      rawdata[i]=rcvbuf[i];
    }
    // printf("x %d %02d - ",chip,sample);for(int i=0;i<24;i++)printf("%02x",rcvbuf[23-i]&0xff);
    //  printf("\n");
    // if(chip==5)printf("\n"); //single chip read mod

      //      JTAGPipeline_read(dv,rawdata);
      //int bitcnt=0;
      int indx[8]={1,4,7,10,13,16,19,22};
      for(int chan=0;chan<16;chan++){
        int iindx=chan/2;
        int evod=chan%2;
        pdata[chip][chan][sample]=0x0000;
        if(evod==0)pdata[chip][chan][sample]=((rawdata[indx[iindx]]<<8)&0x0f00)|(rawdata[indx[iindx]-1]&0x00ff);
        if(evod==1)pdata[chip][chan][sample]=((rawdata[indx[iindx]+1]<<4)&0x0ff0)|((rawdata[indx[iindx]]>>4)&0x000f);
	/*  for(int bit=0;bit<12;bit++){
          char raw=rawdata[bitcnt/8];
          int charbit=bitcnt%8;
          pdata[chip][chan][sample]=pdata[chip][chan][sample]+(((raw>>charbit)&0x0001)<<charbit);
	  bitcnt=bitcnt+1;
	  } */
	//	printf(" chip chan sample pdata %d %d %d %d %04x\n",chip,chan,sample,pdata[chip][chan][sample],pdata[chip][chan][sample]&0x0fff);
      }
    }
    }
  printf(" leaving GetDCFEBData \n");
}

void DCFEB::ReadTemp(){
    DEVTYPE dv = dscamDevice();
    cmd[0]=(VTX6_SYSMON&0xff);
    cmd[1]=((VTX6_SYSMON&0x300)>>8);
    sndbuf[3]=0x04;
    sndbuf[2]=0x00;
    sndbuf[1]=0x00;
    sndbuf[0]=0x00;
    devdo(dv,10,cmd,32,sndbuf,rcvbuf,1);
     printf(" read temperature %02x %02x %02x %02x \n",rcvbuf[3]&0xff,rcvbuf[2]&0xff,rcvbuf[1]&0xff,rcvbuf[0]&0xff);
    unsigned long adcval=((8<<rcvbuf[1])&0x00007f00)+(rcvbuf[0]&0x0000ff);
    float temp=(adcval*503.975/1024.0)-273.15;
    printf(" FEB %d temperature %f \n",dv,temp); 
    cmd[0]=(VTX6_BYPASS&0xff);
    cmd[1]=((VTX6_BYPASS&0x300)>>8);
    sndbuf[0]=0;
    devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
}

void DCFEB::ReadVolts(){  
    DEVTYPE dv = dscamDevice();
    cmd[0]=(VTX6_SYSMON&0xff);
    cmd[1]=((VTX6_SYSMON&0x300)>>8);
    sndbuf[3]=0x04;
    sndbuf[2]=0x01;
    sndbuf[1]=0x00;
    sndbuf[0]=0x00;
    devdo(dv,10,cmd,32,sndbuf,rcvbuf,1);
     printf(" read voltage %02x %02x %02x %02x \n",rcvbuf[3]&0xff,rcvbuf[2]&0xff,rcvbuf[1]&0xff,rcvbuf[0]&0xff);
    unsigned long adcval=((8<<rcvbuf[1])&0x00007f00)+(rcvbuf[0]&0x0000ff);
    float volt0=(adcval/1024.0)*3.000;
    printf(" FEB %d volt0 %f \n",dv,volt0); 
    cmd[0]=(VTX6_BYPASS&0xff);
    cmd[1]=((VTX6_BYPASS&0x300)>>8);
    sndbuf[0]=0;
    devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
}

void DCFEB::Exec_Bit_Slip(int odd){
    DEVTYPE dv = dscamDevice();
    cmd[0]=(VTX6_USR1&0xff);
    cmd[1]=((VTX6_USR1&0x300)>>8);
    if(odd){
      sndbuf[0]=Bit_Slip_Odd;
    }else{
      sndbuf[0]=Bit_Slip_Even;
    }
    devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
    //    printf(" 1 Bit Slip performed \n");
    cmd[0]=(VTX6_BYPASS&0xff);
    cmd[1]=((VTX6_BYPASS&0x300)>>8);
    sndbuf[0]=0;
    devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
}

void  DCFEB::Sel_Chip(int chip){
   DEVTYPE dv = dscamDevice(); 
    cmd[0]=(VTX6_USR1&0xff);
    cmd[1]=((VTX6_USR1&0x300)>>8);
    sndbuf[0]=Chip_Sel;
    devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
    cmd[0]=(VTX6_USR2&0xff);
    cmd[1]=((VTX6_USR2&0x300)>>8);
    sndbuf[0]=chip;
    devdo(dv,10,cmd,3,sndbuf,rcvbuf,1);
    cmd[0]=(VTX6_BYPASS&0xff);
    cmd[1]=((VTX6_BYPASS&0x300)>>8);
    sndbuf[0]=0;
    devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
}


void DCFEB::epromdirect_loadaddress(unsigned int uaddr,unsigned int laddr,int isnd)
{ 
  DEVTYPE dv = dscamDevice();
  unsigned int send1,send2;
  // open direct communications
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);    
  sndbuf[0]=EPROM_mcs;
  sndbuf[1]=0;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  // load address
  send1=((uaddr<<5)&0xffe0)|XPROM_Load_Address;
  cmd[0]=(VTX6_USR2&0xff);
  cmd[1]=((VTX6_USR2&0x300)>>8);
  sndbuf[0]=send1&0xFF;
  sndbuf[1]=(send1>>8)&0xFF; 
  // printf(" loadaddr %02x%02x \n",sndbuf[1]&0xff,sndbuf[0]&0xff);
  devdo(dv,10,cmd,16,sndbuf,rcvbuf,0);
  // printf(" 1read %02x%02x \n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
  send2=laddr;
  cmd[0]=(VTX6_USR2&0xff);
  cmd[1]=((VTX6_USR2&0x300)>>8);
  sndbuf[0]=send2&0xFF;
  sndbuf[1]=(send2>>8)&0xFF;
  // printf(" loadaddr2 %02x%02x \n",sndbuf[1]&0xff,sndbuf[0]&0xff);
  devdo(dv,10,cmd,16,sndbuf,rcvbuf,0);
  // printf(" 2read %02x%02x \n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
  // terminate communications
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  sndbuf[0]=0;
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,isnd);
}

void DCFEB::epromdirect_manual(unsigned int ncnt,unsigned short int *manbuf,int isnd)
{     
  DEVTYPE dv = dscamDevice();
  unsigned int i;
  // open direct communications
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);    
  sndbuf[0]=EPROM_mcs;
  sndbuf[1]=0;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
 
  cmd[0]=(VTX6_USR2&0xff);
  cmd[1]=((VTX6_USR2&0x300)>>8);
  for(i=0;i<ncnt;i++){
    sndbuf[0]= manbuf[i]&0xFF;
    sndbuf[1]=(manbuf[i]>>8)&0xFF; 
    printf(" manbuf %02x%02x \n",sndbuf[1]&0xff,sndbuf[0]&0xff);
    devdo(dv,10,cmd,16,sndbuf,rcvbuf,0);
  }
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  sndbuf[0]=0;
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,isnd);
}

void DCFEB::epromdirect_unlockerase(int isnd)
{ 
  DEVTYPE dv = dscamDevice();
  unsigned int send1;
  // open direct communications
  printf(" dv %d \n",dv);
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);    
  //  printf(" EPROM_mcs %d (dec) \n",EPROM_mcs);
  sndbuf[0]=EPROM_mcs;
  sndbuf[1]=0;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  // unlock and erase
  send1=XPROM_Block_UnLock;
  cmd[0]=(VTX6_USR2&0xff);
  cmd[1]=((VTX6_USR2&0x300)>>8);
  sndbuf[0]=send1&0xFF;
  sndbuf[1]=(send1>>8)&0xFF;
  //printf(" blockunlock %02x%02x \n",sndbuf[1]&0xff,sndbuf[0]&0xff); 
  devdo(dv,10,cmd,16,sndbuf,rcvbuf,0);
  // printf(" 3read %02x%02x \n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
  send1=XPROM_Block_Erase;
  cmd[0]=(VTX6_USR2&0xff);
  cmd[1]=((VTX6_USR2&0x300)>>8);
  sndbuf[0]=send1&0xFF;
  sndbuf[1]=(send1>>8)&0xFF; 
  // printf(" erase %02x%02x \n",sndbuf[1]&0xff,sndbuf[0]&0xff);
  devdo(dv,10,cmd,16,sndbuf,rcvbuf,0);
  // printf(" 4read %02x%02x \n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
  // terminate communications
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  sndbuf[0]=0;
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,isnd);
}

void DCFEB::epromdirect_unlock(int isnd)
{
  DEVTYPE dv = dscamDevice();
  unsigned int send1;
  // open direct communications
  printf(" dv %d \n",dv);
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);    
  //  printf(" EPROM_mcs %d (dec) \n",EPROM_mcs);
  sndbuf[0]=EPROM_mcs;
  sndbuf[1]=0;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  // unlock and erase
  send1=XPROM_Block_UnLock;
  cmd[0]=(VTX6_USR2&0xff);
  cmd[1]=((VTX6_USR2&0x300)>>8);
  sndbuf[0]=send1&0xFF;
  sndbuf[1]=(send1>>8)&0xFF;
  //printf(" blockunlock %02x%02x \n",sndbuf[1]&0xff,sndbuf[0]&0xff); 
  devdo(dv,10,cmd,16,sndbuf,rcvbuf,isnd);
}

void DCFEB::epromdirect_noop(int isnd)
{
  DEVTYPE dv = dscamDevice();
  unsigned int send1;
  // open direct communications
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);    
  sndbuf[0]=EPROM_mcs;
  sndbuf[1]=0;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  //noop
  send1=0;
  cmd[0]=(VTX6_USR2&0xff);
  cmd[1]=((VTX6_USR2&0x300)>>8);
  sndbuf[0]=send1&0xFF;
  sndbuf[1]=(send1>>8)&0xFF;
  devdo(dv,10,cmd,16,sndbuf,rcvbuf,0);
  printf(" noop %02x%02x \n",sndbuf[1]&0xff,sndbuf[0]&0xff);
  // terminate communications
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  sndbuf[0]=0;
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,isnd);
}


void DCFEB::epromdirect_bufferprogram(unsigned int paddr,unsigned short int *prm_dat,int isnd)
{
  DEVTYPE dv = dscamDevice();
  unsigned short int send1;
  // open direct communications
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);    
  sndbuf[0]=EPROM_mcs;
  sndbuf[1]=0;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  // now write data
  send1= (((paddr-1)<<5)&0xffe0)|XPROM_Buffer_Program;
  // printf(" buffer program %04x paddr %ld \n",send1&0xffff,paddr);
  cmd[0]=(VTX6_USR2&0xff);
  cmd[1]=((VTX6_USR2&0x300)>>8);
  sndbuf[0]=send1&0xFF;
  sndbuf[1]=(send1>>8)&0xFF;
  devdo(dv,10,cmd,16,sndbuf,rcvbuf,0);
  // send data
  for(unsigned short int k=0;k<paddr;k++){
    send1=prm_dat[k];
    //printf(" dv %d send data %04x \n",dv,send1&0xffff);
    cmd[0]=(VTX6_USR2&0xff);
    cmd[1]=((VTX6_USR2&0x300)>>8);
    sndbuf[0]=send1&0xFF;
    sndbuf[1]=(send1>>8)&0xFF; 
    devdo(dv,10,cmd,16,sndbuf,rcvbuf,0);
  }
  // terminate communications
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  sndbuf[0]=0;
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,isnd);
}

void DCFEB::epromdirect_lock(int isnd)
{ 
  DEVTYPE dv = dscamDevice();
  unsigned int send1;
  // open direct communications
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);    
  sndbuf[0]=EPROM_mcs;
  sndbuf[1]=0;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  // lock last block
  send1=XPROM_Block_Lock;
  cmd[0]=(VTX6_USR2&0xff);
  cmd[1]=((VTX6_USR2&0x300)>>8);
  sndbuf[0]=send1&0xFF;
  sndbuf[1]=(send1>>8)&0xFF; 
  devdo(dv,10,cmd,16,sndbuf,rcvbuf,0);
  // go to bypass mode
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  sndbuf[0]=0;
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,isnd);
}

void DCFEB::epromdirect_jread_only(unsigned int n,unsigned short int *pdata)
{ 
   DEVTYPE dv = dscamDevice();
  unsigned short int send1;
  int init_val;
  int first;
  first=0;
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);    
  sndbuf[0]=EPROM_read_mcs;
  sndbuf[1]=0;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  send1=0x0000;
  cmd[0]=(VTX6_USR2&0xff);
  cmd[1]=((VTX6_USR2&0x300)>>8);
  sndbuf[0]=send1&0xFF;
  sndbuf[1]=(send1>>8)&0xFF;
  if(first==0){
    init_val = 1;
    first=1;
  }else{
    init_val = 0;
  }
  int send =1;
  for(unsigned int i=0;i<n;i++){
    //    if(i>0 && (i%16)==0) send = 1;
    DCFEBEPROM_read(dv,10,cmd,16,sndbuf,rcvbuf,1,send,init_val);
    pdata[i]=((rcvbuf[1]<<8)&0xff00)|(rcvbuf[0]&0x00ff);
    //    printf("rcvbuf %02X%02X: pdata[%d] %04X\n",rcvbuf[1],rcvbuf[0],i,pdata[i]);
    //    send = 0;
  }
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  sndbuf[0]=0;
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
  // now pack pdata
  /*  for(int i=0;i<n;i++){
    pdata[i]=((rcvbuf[2*i+1]<<8)&0xff00)|(rcvbuf[2*i]&0x00ff);
    }*/
}
void DCFEB::epromdirect_read(unsigned int paddr,unsigned short int *pdata)
{ 
  DEVTYPE dv = dscamDevice();
  unsigned short int send1;
  int s16s,slefts;
  int cpaddr;
  int first;
  first=0;
  cpaddr=0;
  s16s=paddr/16;
  slefts=paddr-s16s*16;
  // printf(" enter direct read %d %d %d \n",paddr,s16s,slefts);
  for(int i=0;i<s16s;i++){
    // now readback
    // open direct communications
    cmd[0]=(VTX6_USR1&0xff);
    cmd[1]=((VTX6_USR1&0x300)>>8);    
    sndbuf[0]=EPROM_mcs;
    sndbuf[1]=0;
    devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
    // buffer read (2039,04)
    send1= (((16-1)<<5)&0xffe0)|XPROM_Read_n;
    // printf(" read_n %04x \n",send1&0xffff);
    cmd[0]=(VTX6_USR2&0xff);
    cmd[1]=((VTX6_USR2&0x300)>>8);
    sndbuf[0]=send1&0xFF;
    sndbuf[1]=(send1>>8)&0xFF;
    devdo(dv,10,cmd,16,sndbuf,rcvbuf,0);
    cmd[0]=(VTX6_BYPASS&0xff);
    cmd[1]=((VTX6_BYPASS&0x300)>>8);
    sndbuf[0]=0;
    devdo(dv,10,cmd,0,sndbuf,rcvbuf,0);
    int sleeep=10;
    sndbuf[0]=sleeep&0xff;
    sndbuf[1]=(sleeep>>8)&0xff;
    devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,0);
    //  epromdirect_status(dv); 
    // jtag readback
    //printf(" EPROM_read_mcs %d %04x \n",EPROM_read_mcs,EPROM_read_mcs);
    cmd[0]=(VTX6_USR1&0xff);
    cmd[1]=((VTX6_USR1&0x300)>>8);    
    sndbuf[0]=EPROM_read_mcs;
    sndbuf[1]=0;
    devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
    send1=0x0000;
    cmd[0]=(VTX6_USR2&0xff);
    cmd[1]=((VTX6_USR2&0x300)>>8);
    sndbuf[0]=send1&0xFF;
    sndbuf[1]=(send1>>8)&0xFF;
    if(first==0){
      DCFEBEPROM_read(dv,10,cmd,16,sndbuf,rcvbuf,1,0,1);
      first=1;
    }else{
       DCFEBEPROM_read(dv,10,cmd,16,sndbuf,rcvbuf,1,0,0);
      first=1;
    }
     for(int ii=1;ii<16;ii++)DCFEBEPROM_read(dv,10,cmd,16,sndbuf,rcvbuf,1,0,0);
    cpaddr=cpaddr+16;
    cmd[0]=(VTX6_BYPASS&0xff);
    cmd[1]=((VTX6_BYPASS&0x300)>>8);
    sndbuf[0]=0;
    if(slefts==0&&i==s16s-1){
      devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
    }else{
      devdo(dv,10,cmd,0,sndbuf,rcvbuf,0);
    }
  }
  if(slefts>0){
  // now readback
  // open direct communications
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);    
  sndbuf[0]=EPROM_mcs;
  sndbuf[1]=0;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  // buffer read (2039,04)
  send1= (((slefts-1)<<5)&0xffe0)|XPROM_Read_n;
  // printf(" read_n %04x \n",send1&0xffff);
  cmd[0]=(VTX6_USR2&0xff);
  cmd[1]=((VTX6_USR2&0x300)>>8);
  sndbuf[0]=send1&0xFF;
  sndbuf[1]=(send1>>8)&0xFF;
  devdo(dv,10,cmd,16,sndbuf,rcvbuf,0);
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  sndbuf[0]=0;
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,0);
  int sleeep2=10;
  sndbuf[0]=sleeep2&0xff;
  sndbuf[1]=(sleeep2>>8)&0xff;
  devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,0);
  //  epromdirect_status(dv); 
  // jtag readback
  //printf(" EPROM_read_mcs %d %04x \n",EPROM_read_mcs,EPROM_read_mcs);
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);    
  sndbuf[0]=EPROM_read_mcs;
  sndbuf[1]=0;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  send1=0x0000;
  cmd[0]=(VTX6_USR2&0xff);
  cmd[1]=((VTX6_USR2&0x300)>>8);
  sndbuf[0]=send1&0xFF;
  sndbuf[1]=(send1>>8)&0xFF;
  if(first==0){
     DCFEBEPROM_read(dv,10,cmd,16,sndbuf,rcvbuf,1,0,1);
    first=1;
    cpaddr++;
  }else{
     DCFEBEPROM_read(dv,10,cmd,16,sndbuf,rcvbuf,1,0,0);
    first=1;
    cpaddr++;
  }
   for(int i=1;i<slefts;i++){DCFEBEPROM_read(dv,10,cmd,16,sndbuf,rcvbuf,1,0,0);cpaddr++;}
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  sndbuf[0]=0;
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
  }
  // now pack pdata
  for(int i=0;i<cpaddr;i++){
    pdata[i]=((rcvbuf[2*i+1]<<8)&0xff00)|(rcvbuf[2*i]&0x00ff);
  }
}
void DCFEB::epromdirect_readtest(unsigned int paddr)
{ 
  DEVTYPE dv = dscamDevice();
  unsigned short int send1;
  // now readback
  // open direct communications
  for(unsigned int i=0;i<paddr;i++){
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);    
  sndbuf[0]=EPROM_mcs;
  sndbuf[1]=0;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  // buffer read (2039,04)
  send1= (((1-1)<<5)&0xffe0)|XPROM_Read_n;
  // printf(" read_n %04x \n",send1&0xffff);
  cmd[0]=(VTX6_USR2&0xff);
  cmd[1]=((VTX6_USR2&0x300)>>8);
  sndbuf[0]=send1&0xFF;
  sndbuf[1]=(send1>>8)&0xFF;
  devdo(dv,10,cmd,16,sndbuf,rcvbuf,0);
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  sndbuf[0]=0;
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
  epromdirect_status(); 
  // jtag readback
  printf(" EPROM_read_mcs %d %04x \n",EPROM_read_mcs,EPROM_read_mcs);
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);    
  sndbuf[0]=EPROM_read_mcs;
  sndbuf[1]=0;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  //  for(unsigned short int k=0;k<paddr;k++){
  send1=0x0000;
    cmd[0]=(VTX6_USR2&0xff);
    cmd[1]=((VTX6_USR2&0x300)>>8);
    sndbuf[0]=send1&0xFF;
    sndbuf[1]=(send1>>8)&0xFF;
    devdo(dv,10,cmd,16,sndbuf,rcvbuf,1);
    printf("epromdata read2 %02x%02x \n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
    //  }
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  sndbuf[0]=0;
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
  } 
}

void DCFEB::epromdirect_timerstart()
{    
  DEVTYPE dv = dscamDevice();
  unsigned short int send1;
  // open direct communications
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);    
  sndbuf[0]=EPROM_mcs;
  sndbuf[1]=0;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  // now write data
  send1=XPROM_Timer_Start;
  cmd[0]=(VTX6_USR2&0xff);
  cmd[1]=((VTX6_USR2&0x300)>>8);
  sndbuf[0]=send1&0xFF;
  sndbuf[1]=(send1>>8)&0xFF;
  devdo(dv,10,cmd,16,sndbuf,rcvbuf,0);
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  sndbuf[0]=0;
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,0);
  printf(" timer start \n"); 
}

void DCFEB::epromdirect_timerstop()
{
  DEVTYPE dv = dscamDevice();
  unsigned short int send1;
  // open direct communications
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);    
  sndbuf[0]=EPROM_mcs;
  sndbuf[1]=0;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  // now write data
  send1= XPROM_Timer_Stop;
  cmd[0]=(VTX6_USR2&0xff);
  cmd[1]=((VTX6_USR2&0x300)>>8);
  sndbuf[0]=send1&0xFF;
  sndbuf[1]=(send1>>8)&0xFF;
  devdo(dv,10,cmd,16,sndbuf,rcvbuf,0);
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  sndbuf[0]=0;
  printf(" timer stop \n");
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,0);
}

void DCFEB::epromdirect_timerreset()
{ 
  DEVTYPE dv = dscamDevice();
  unsigned short int send1;
  // open direct communications
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);    
  sndbuf[0]=EPROM_mcs;
  sndbuf[1]=0;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  // now write data
  send1= XPROM_Timer_Reset;
  cmd[0]=(VTX6_USR2&0xff);
  cmd[1]=((VTX6_USR2&0x300)>>8);
  sndbuf[0]=send1&0xFF;
  sndbuf[1]=(send1>>8)&0xFF;
  devdo(dv,10,cmd,16,sndbuf,rcvbuf,0);
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  sndbuf[0]=0;
  printf(" timer reset \n");
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
}

void DCFEB::epromdirect_clearstatus()
{
  DEVTYPE dv = dscamDevice();
  unsigned short int send1;
  // open direct communications
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);    
  sndbuf[0]=EPROM_mcs;
  sndbuf[1]=0;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  // now write data;
  send1= XPROM_Clear_Status;
  cmd[0]=(VTX6_USR2&0xff);
  cmd[1]=((VTX6_USR2&0x300)>>8);
  sndbuf[0]=send1&0xFF;
  sndbuf[1]=(send1>>8)&0xFF;
  devdo(dv,10,cmd,16,sndbuf,rcvbuf,0);
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  sndbuf[0]=0;
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,0);
}

void DCFEB::epromdirect_timerread()
{
  DEVTYPE dv = dscamDevice();
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);
  // printf(" EPROM_timer_mcs %d \n",EPROM_timer_mcs);
  sndbuf[0]=EPROM_timer_mcs;
  sndbuf[1]=0;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=(VTX6_USR2&0xff);
  cmd[1]=((VTX6_USR2&0x300)>>8);
  sndbuf[0]=0x1f;
  sndbuf[1]=0x2f;
  sndbuf[2]=0x4f;
  sndbuf[3]=0x8f;
  devdo(dv,10,cmd,32,sndbuf,rcvbuf,1);
  int timer_val = (0xff&rcvbuf[3])<<24 | (0xff&rcvbuf[2])<<16 | (0xff&rcvbuf[1])<<8 | 0xff&rcvbuf[0];
  printf(" The TimerRead (hex) is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
  printf(" The TimerRead       is %d us\n",timer_val);
  int minutes = timer_val/60000000;
  float seconds = (float)timer_val/1000000 - 60*minutes;
  printf(" The TimerRead       is %d min %2.2f sec\n",minutes,seconds);
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  sndbuf[0]=0;
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,0);  
}

unsigned short DCFEB::epromdirect_status()
{
  /* status register
     low 8 bits XLINK
      0-blank write status/multiple work program status
      1-block protection status
      2-program suspend status
      3-vpp status
      4-program status
      5-erase/blank check status
      6-erase/suspend status
      7-P.E.C. Status
     high 8 bits Ben
      0-cmd fifo write error
      1-cmd fifo read error
      2-cmd fifo full 
      3-cmd fifo empty
      4-rbk fifo write error
      5-rbk fifo read error
      6-rbk fifo full 
      7-rbk fifo empty
  */  
  DEVTYPE dv = dscamDevice();
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);
  sndbuf[0]=EPROM_status_mcs;
  sndbuf[1]=0;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=(VTX6_USR2&0xff);
  cmd[1]=((VTX6_USR2&0x300)>>8);
  sndbuf[0]=0xFF;
  sndbuf[1]=0xFF;
  devdo(dv,10,cmd,16,sndbuf,rcvbuf,1);
  // printf(" The EPROM Status is %02x%02x \n",0xff&rcvbuf[1],0xff&rcvbuf[0]);
  unsigned short int status=((rcvbuf[1]<<8)&0xff00)|(0xff&rcvbuf[0]);
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  sndbuf[0]=0;
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,0);  
  return status;
}

void DCFEB::epromdirect_disablefifo()
{ 
  DEVTYPE dv = dscamDevice();
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);
  sndbuf[0]=EPROM_disablefifo_mcs;
  sndbuf[1]=0;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  sndbuf[0]=0;
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,0);  
}

void DCFEB::epromdirect_enablefifo()
{
  DEVTYPE dv = dscamDevice();
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);
  sndbuf[0]=EPROM_enablefifo_mcs;
  sndbuf[1]=0;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  sndbuf[0]=0;
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);  
}

void DCFEB::epromdirect_reset()
{
  DEVTYPE dv = dscamDevice();
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);
  sndbuf[0]=EPROM_reset_mcs;
  sndbuf[1]=0;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  sndbuf[0]=0;
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);  
}

int DCFEB::epromdirect_erasepolling()
{
  // unsigned long int fulladdr;
  // unsigned int uaddr,laddr;
  // unsigned int paddr;
  epromdirect_enablefifo();
  epromdirect_clearstatus();
  epromdirect_status();
  epromdirect_unlockerase(2);
  unsigned short int status;
  for(int j=0;j<500;j++){
    // printf(" status %d \n",j);
    status=epromdirect_status();
    if(status&0x0080==0x0080)return 1;
    usleep(10000);
  }
  printf(" erase polling status %04x \n",status);
  printf(" epromdirect_erasepolling: erase status bad \n");
  return 0;
}



int DCFEB::read_mcs(const char *fn){
  return 1;
}


int DCFEB::epromloadtest_mcs()
{
  unsigned short int prm_dat[2048];
  // unsigned short int manbuf[10];
  unsigned int saddr,addr;
  unsigned long int fulladdr;
  unsigned int uaddr,laddr;
  unsigned int paddr;
  // unsigned short int pdata[512];
  saddr=0x00ff;
  addr=0x0000;
  paddr=16;
  for(unsigned int i=0;i<paddr;i++){
    prm_dat[i]=((i<<8)&0xff00)+(i&0xff);
    //  printf(" %d %04x \n",i,prm_dat[i]&0xffff);
  }
  DEVTYPE dv = dscamDevice();
  printf(" Write to device %d \n",dv);
  epromdirect_reset();
  fulladdr=((saddr<<16)&0xffff0000)|(addr&0x0000ffff);
  fulladdr=(fulladdr>>1)&0x7fffffff;
  fulladdr=fulladdr;
  uaddr=(fulladdr>>16)&0xffff;
  laddr=(fulladdr&0xffff);
  epromdirect_enablefifo();
  // load address
  epromdirect_loadaddress(uaddr,laddr,2);
  // unlock and erase
  epromdirect_clearstatus();
  printf(" status after clear \n");
  usleep(1000);
  epromdirect_status();
  epromdirect_unlockerase(2);
  for(int j=0;j<120;j++){
    printf(" status %d \n",j);
    epromdirect_status();
    usleep(100000);
  }
  epromdirect_lock(2);
  usleep(1000);
  printf(" status after lock \n");
  epromdirect_status();
  return 1; 
}

int DCFEB::epromreadtest_mcs()
{
  // unsigned short int prm_dat[2048];
  // unsigned short int manbuf[10];
  unsigned int saddr,addr;
  unsigned long int fulladdr;
  unsigned int uaddr,laddr;
  unsigned int paddr;
  printf(" entered epromreadtest_mcs \n");
  DEVTYPE dv =dscamDevice();
  printf(" dv %d \n",dv);
  epromdirect_reset();
  epromdirect_enablefifo();
  saddr=0x0000;
  addr=0x0020;
  fulladdr=((saddr<<16)&0xffff0000)|(addr&0x0000ffff);
  fulladdr=(fulladdr>>1)&0x7fffffff;
  fulladdr=fulladdr;
  uaddr=(fulladdr>>16)&0xffff;
  laddr=(fulladdr&0xffff);
  uaddr=0x0000;
  laddr=0x0010;
  // load address
  epromdirect_loadaddress(uaddr,laddr,2);
  for(int k=0;k<10000;k++){
    usleep(1000);
    printf(" k %d \n",k);
    unsigned short int pdata[1000];
    paddr=160;
    epromdirect_read(paddr,pdata);
    int cnt=0;
    for(int i=0;i<20;i++){
      for(int j=0;j<8;j++){
         printf("%04x",pdata[cnt]);
         cnt++;
      }
      printf("\n");
    }
  }
  return 1;
}


void DCFEB::epromload_parameters(int paramblock,int nval,unsigned short int  *val){
  /*  The highest four blocks in the eprom are parameter banks of
      length 16k 16 bit words. In mcs addressing (bits) the starting
      addresses are:

           block 0  007f 0000
           block 1  007f 4000
           block 2  007f 8000
           block 3  007f c000

     the config program takes up the range
                    0000 0000
                    0005 4000

      ref: http://www.xilinx.com/support/documentation/data_sheets/ds617.pdf */
  /*                 2-byte words       bytes
      eprom size     0x00800000        0x01000000
      mcs size       0x002A0000        0x00540000
      params addr    0x007f0000        0x00fe0000
      
      mcs file addressing is in bytes
*/

  int paddr,lpcnt;
  unsigned int tt;
  unsigned int ll,addr;
  unsigned long int fulladdr;
  unsigned int uaddr,laddr;
  unsigned int luaddr,lladdr;
  unsigned int cur_blk_addr,nxt_blk_addr = 0;
  unsigned int saddr;
  unsigned short int prm_dat[2000];
  unsigned int block[4]={0x00000000,0x00008000,0x00010000,0x00018000};
  if(nval>1000){
    printf(" Catastrophy:parameter space large rewrite program %d \n",nval);
    return;
  }
  paddr = 0;
  lpcnt = 0;
  ll=16;
  tt=0;
  epromdirect_reset();
  epromdirect_disablefifo();
  epromdirect_timerstop();
  epromdirect_timerreset();
  tt=4; // tt=4
  saddr=0x008e;
  addr=block[paramblock];
  fulladdr=((saddr<<16)&0xffff0000)|(addr&0x0000ffff);
  printf(" parameterwrite fulladdr %08lx \n",fulladdr);
  fulladdr=(fulladdr>>1)&0x7fffffff;
  printf(" parameterwrite fulladdr %08lx \n",fulladdr);
  uaddr=(fulladdr>>16)&0xffff;
  laddr=(fulladdr&0xffff);
  luaddr=uaddr;
  lladdr=laddr;
  cur_blk_addr=fulladdr;
  // load new address
  epromdirect_timerstart();
  epromdirect_loadaddress(uaddr,laddr,0);
  // unlock and erase
  epromdirect_unlockerase(0);
  printf(" unlockerase %04x %04x \n",uaddr,laddr);
  epromdirect_enablefifo();
  cur_blk_addr = nxt_blk_addr;
  epromdirect_disablefifo();
  usleep(2000);
  tt=0; // send data
  for(int ii=0;ii<nval;ii++){
    prm_dat[ii]=val[ii];
  }
  epromdirect_bufferprogram(nval,prm_dat,0);
  epromdirect_enablefifo();
  unsigned int sleeep=1984*64+164;
  //  sndbuf[0]=sleeep&0xff;
  // sndbuf[1]=(sleeep>>8)&0xff;
  // devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,2);
  printf(" write paddr %d sleep %d \n",paddr,sleeep);
  usleep(sleeep);
  paddr=0;
  epromdirect_disablefifo();
  // lock last block load address
  epromdirect_loadaddress((cur_blk_addr>>16)&0xffff,cur_blk_addr&0xffff,0);
  // lock last block
  epromdirect_lock(0);
  epromdirect_enablefifo();
  epromdirect_disablefifo();
  usleep(10);
  printf(" lock \n");
}

void DCFEB::epromread_parameters(int paramblock,int nval,unsigned short int  *val){
  /*  The highest four blocks in the eprom are parameter banks of
      length 16k 16 bit words. In mcs addressing (bits) the starting
      addresses are:

           block 0  007f 000000
           block 1  007f 400000
           block 2  007f 800000
           block 3  007f c00000

      ref: http://www.xilinx.com/support/documentation/data_sheets/ds617.pdf */
  int paddr,lpcnt;
  unsigned int tt;
  unsigned int ll,addr;
  unsigned long int fulladdr;
  unsigned int uaddr,laddr;
  unsigned int luaddr,lladdr;
    unsigned int saddr;
  unsigned int cur_blk_addr;
  unsigned int block[4]={0x00000000,0x00008000,0x00010000,0x00018000};
  if(nval>1000){
    printf(" Catastrophy:parameter space large rewrite program %d\n",nval);
    return;
  }
  paddr = 0;
  lpcnt = 0;
  ll=16;
  tt=0;
  epromdirect_reset();
  epromdirect_enablefifo();
   tt=4; // tt=4
  saddr=0x008e;
  addr=block[paramblock];
  fulladdr=((saddr<<16)&0xffff0000)|(addr&0x0000ffff);
  printf(" parameterread fulladdr %08lx \n",fulladdr);
  fulladdr=(fulladdr>>1)&0x7fffffff;
  printf(" parameterread fulladdr %08lx \n",fulladdr);
  uaddr=(fulladdr>>16)&0xffff;
  laddr=(fulladdr&0xffff);
  luaddr=uaddr;
  lladdr=laddr;
  cur_blk_addr=fulladdr;
  epromdirect_loadaddress(uaddr,laddr,2);
  epromdirect_read(nval,val);
}


void DCFEB::make_eprom_mcs_file(char *wrt)
{
  FILE *fpmcs;
  char chksum;
  char strt[7]={0x02,0x00,0x00,0x04,0x00,0x55,0xFF};
  char endd[5]={0x00,0x00,0x00,0x01,0xFF};
  char pkt[21]={0x10,0x00,0x00,0x00,0x01,0x02,0x03,0x04,0x05,0x06,
		0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0xFF};
  fpmcs=fopen("/tmp/action.mcs","w");
  for(int i=0;i<16;i++)pkt[4+i]=wrt[i];
  chksum=0;
  for(int i=0;i<6;i++)chksum=chksum+strt[i];strt[6]=(-1*chksum)&0xff;
  fprintf(fpmcs,":");
  for(int i=0;i<7;i++)fprintf(fpmcs,"%02X",strt[i]&0xff);fprintf(fpmcs,"\n");
  chksum=0;
  for(int i=0;i<20;i++)chksum=chksum+pkt[i];pkt[20]=(-1*chksum)&0xff;
  fprintf(fpmcs,":");
  for(int i=0;i<21;i++)fprintf(fpmcs,"%02X",pkt[i]&0xff);fprintf(fpmcs,"\n");
  chksum=0;
  for(int i=0;i<4;i++)chksum=chksum+endd[i];endd[4]=(-1*chksum)&0xff;
  fprintf(fpmcs,":");
  for(int i=0;i<5;i++)fprintf(fpmcs,"%02X",endd[i]&0xff);fprintf(fpmcs,"\n");
  fclose(fpmcs);
}

int DCFEB::epromload_mcs(const char *fn,int loadlowhigh)
{
  /* loadlowhigh     0  -   load program at prom address 0x00000000
                     1  -   load program at prom address 0x00550000;
  */
  /*
     mcs files address in bytes
     ben's eprom engine in 2 byte words so shift left one bit
  */
  /*                 2-byte words       bytes
      eprom size     0x00800000        0x01000000
      mcs size       0x002A0000        0x00540000
      params addr    0x007F0000        0x00fd0000
      
      mcs file addressing is in bytes
*/
  DEVTYPE dv = dscamDevice();  
  unsigned short int prm_dat[2048];
  char line[1024];
  FILE *fp;
  int paddr,lpcnt,parms;
  unsigned int tt;
  unsigned int ddl,ddh;
  unsigned int ll,addr;
  unsigned long int fulladdr,fulloffset;
  unsigned int uaddr,laddr;
  unsigned int luaddr,lladdr;
  unsigned int cur_blk_addr,nxt_blk_addr;
  int ldadd;
  unsigned int saddr;
  printf(" dv %d \n",dv);
  //  DEVTYPE dv = dcfebs_[icfeb].dscamDevice();
  fp = fopen(fn,"r");
  if(fp==NULL){
    printf("Failed to open file %s\n",fn);
    return -1;
  }
  paddr = 0;
  lpcnt = 0;
  tt=0;
  ldadd=0;
  fulladdr=0x00540000;
  fulloffset=0x00000000;
  if(loadlowhigh==1)fulloffset=0x00550000;
  cur_blk_addr = 0xFFFFFFFF;
  epromdirect_reset();
  //  epromdirect_enablefifo();
  epromdirect_disablefifo();
  epromdirect_timerstop();
  epromdirect_timerreset();
  while(tt!=1){
    parms = fscanf(fp,":%2x%4x%2x",&ll,&addr,&tt);
    // printf("%1x:%02X %04X %02X\n",parms,ll,addr,tt);
    if(tt==0){
      for(unsigned int ii=0;ii<ll/2;ii++){
        parms = fscanf(fp,"%2x%2x",&ddl,&ddh);
        prm_dat[paddr++] = ((ddh<<8)&0xff00) | (ddl&0x00ff);
        fulladdr=((saddr<<16)&0xffff0000)|(addr&0x0000ffff);
        fulladdr=fulladdr+fulloffset;
	fulladdr=(fulladdr>>1)&0x7fffffff;
        fulladdr=fulladdr+ii;
        uaddr=(fulladdr>>16)&0xffff;
        laddr=(fulladdr&0xffff);
        luaddr=uaddr;
        lladdr=laddr;
        if(fulladdr>=0x7F0000){
          nxt_blk_addr = fulladdr&0xFFFFC000;
	}
	else {
          nxt_blk_addr = fulladdr&0xFFFF0000;
	}
        if(ldadd==0){
          ldadd=1;
          if(nxt_blk_addr != cur_blk_addr){
            // lock last block load address
            if(cur_blk_addr!=0xFFFFFFFF){
              epromdirect_loadaddress((cur_blk_addr>>16)&0xffff,cur_blk_addr&0xffff,0);
              // lock last block
              epromdirect_lock(0);
              //printf(" lock \n");
	    }
            // load new address
            epromdirect_timerstart();
            epromdirect_loadaddress(uaddr,laddr,0);
            // unlock and erase
	    epromdirect_unlockerase(0);
	    // printf(" unlockerase %04x %04x \n",uaddr,laddr);
            epromdirect_enablefifo();
            cur_blk_addr = nxt_blk_addr;
            epromdirect_disablefifo();
            //usleep(2000000);
            epromdirect_erasepolling();
          }
	  else {
            epromdirect_loadaddress(uaddr,laddr,0);
	  }
        }
        if(paddr==1984){
          // buffer program (2039,0c)
	  epromdirect_bufferprogram(paddr,prm_dat,0);
          epromdirect_enablefifo();
          unsigned int sleeep=1984*64+164;
	  //  sndbuf[0]=sleeep&0xff;
	  // sndbuf[1]=(sleeep>>8)&0xff;
	  // devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,2);
          // printf(" write paddr %d sleep %d \n",paddr,sleeep);
          usleep(sleeep);
          paddr=0;
          epromdirect_disablefifo();
        }
	// printf("%08lX %04x %04x: %04hX %d \n",fulladdr,uaddr,laddr,prm_dat[paddr-1],ii);
      }
    }
    else if(tt==4){
       parms = fscanf(fp,"%4x",&saddr);
       printf(" saddr %4x %08lx \n",saddr,fulladdr);

       if(paddr>0){
          // buffer program (2039,0c)
          epromdirect_bufferprogram(paddr,prm_dat,0);
          epromdirect_enablefifo();
          unsigned int sleeep2=paddr*64+164;
	  //  sndbuf[0]=sleeep2&0xff;
	  // sndbuf[1]=(sleeep2>>8)&0xff;
	  // devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,2);
          usleep(sleeep2);
          printf(" write paddr %d sleep2 %d \n",paddr,sleeep2);
          paddr=0;
          epromdirect_disablefifo();
       }
       // signal new base address
       ldadd=0;
    }
    else if(tt==1){ // end of mcs file
       if(paddr>0){
	  epromdirect_bufferprogram(paddr,prm_dat,0);
          epromdirect_timerstop();
	  epromdirect_enablefifo();
          unsigned int sleeep2=paddr*64+164;
	  //  sndbuf[0]=sleeep2&0xff;
	  // sndbuf[1]=(sleeep2>>8)&0xff;
	  // devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,2);
          usleep(sleeep2);
          printf(" write paddr %d sleep2 %d \n",paddr,sleeep2);
          epromdirect_disablefifo();
       }
       // lock last block load address
       // epromdirect_loadaddress(dv,luaddr,lladdr,2);
       // lock last block
       epromdirect_lock(0);
       epromdirect_enablefifo();
       epromdirect_disablefifo();
       usleep(10);
       printf(" lock \n");
    }
    else{
      printf("Unknown type! %d\n",tt);
    } 
    fgets(line,1024,fp); //read checksum and cr
    lpcnt++;
    //    printf("%s %d\n",line,lpcnt);
  } 
  return lpcnt;
}
int DCFEB::check_svf(const char *fn){
  int i,j;
  char buf[512];
  unsigned int config_size;
  char cfg_size[4];
  //  char cfg_cmd[4]={0x00,0x0C,0x00,0x02};
  char a[4];
  unsigned int uia[4];
  int cfg_pos=-99;
  char cfg_char[9];
  char cfg_cmd[8]={'0','0','0','2','0','0','0','c'};
  int deviceid_pos=-99;
  char deviceid_char[9];
  char deviceid[4];
  char deviceid_cmd[8]={'8','0','0','1','8','0','0','c'};
  int cnt,chk;
  char type[8]={'D','C','F','E','B','0','0','0'};
  FILE *fp;
  fp = fopen(fn,"r");
  if(fp==NULL){
    printf("Failed to open file \n");
    return -1;
  }
  cnt=0;
  while(fgets(buf,512,fp)!=NULL){
    // printf(" %s \n",buf);
    if(deviceid_pos==1){
      deviceid_pos=-1;
      for(j=0;j<8;j++)deviceid_char[j]=buf[j+12];deviceid_char[8]='\0';
      sscanf(deviceid_char,"%02x%02x%02x%02x",&uia[0],&uia[1],&uia[2],&uia[3]);
      for(j=0;j<4;j++)a[j]=uia[j]&0xff;
      dshuffle(a,deviceid);
      printf(" DEVICEID:%02X%02X%02X%02X\n",deviceid[0]&0xff,deviceid[1]&0xff,deviceid[2]&0xff,deviceid[3]&0xff);
    }
    if(cfg_pos==1){
      cfg_pos=-1;
      for(j=0;j<8;j++)cfg_char[j]=buf[j+12];cfg_char[8]='\0';
      sscanf(cfg_char,"%02x%02x%02x%02x",&uia[0],&uia[1],&uia[2],&uia[3]);
      for(j=0;j<4;j++)a[j]=uia[j]&0xff;
      dshuffle(a,cfg_size);
      printf(" CFG_SIZE:%02X%02X%02X%02X\n",cfg_size[0]&0xff,cfg_size[1]&0xff,cfg_size[2]&0xff,cfg_size[3]&0xff);
      config_size=((cfg_size[1]<<16)&0x00ff0000)|((cfg_size[2]<<8)&0x0000ff00)|(cfg_size[3]&0x000000ff);
      printf(" config_size %d \n",config_size);

      goto ENDR;
    }  
    for(i=0;i<509;i++){
      chk=0;
      for(j=0;j<8;j++)if(buf[i+j]==type[j])chk++;
      if(chk==8)printf(" %s \n",buf);
      chk=0;
      for(j=0;j<8;j++)if(buf[i+j]==deviceid_cmd[j])chk++;
      if(chk==8){printf(" deviceid found \n");deviceid_pos=1;}
      chk=0;
      for(j=0;j<8;j++)if(buf[i+j]==cfg_cmd[j])chk++;
      if(chk==8){printf(" cfg_cmd found \n");cfg_pos=1;} 
    }
    for(i=0;i<512;i++)buf[i]=0x00;
    cnt=cnt+1;
  }
 ENDR:
  fclose(fp);
  return 1;
}

int DCFEB::check_mcs(const char *fn){
  int parms; 
  unsigned int saddr;
  unsigned int tt;
  unsigned int ddl,ddh;
  unsigned int ll,addr;
  char line[1024];
  int jj,mm;
  int cnt;
  char buf[2];
  char sbuf[16];
  int pass=0;
  int cfg_pos=-99;
  char a[4];
  unsigned int config_size;
  char cfg_size[4];
  char cfg_cmd[4]={0x00,0x0C,0x00,0x02};
  int deviceid_pos=-99;
  char deviceid[4];
  char deviceid_cmd[4]={0x80,0x0C,0x80,0x01};
  char brd[4],timestamp[4];
  unsigned int curt;
  time_t curt_;
  struct tm *loct_;
  FILE *fp;

  fp = fopen(fn,"r");
  if(fp==NULL){
    printf("Failed to open file \n");
    return -1;
  }
  cnt=0;
  tt=0;
  while(tt!=1){    
    parms = fscanf(fp,":%2x%4x%2x",&ll,&addr,&tt);
    // printf("%1x:%02X %04X %02X \n",parms,ll,addr,tt);
    cnt++;
    if(tt==0){
      // printf("%04x%04x ",saddr,addr);
      for(unsigned int ii=0;ii<ll/2;ii++){
        parms = fscanf(fp,"%2x%2x",&ddl,&ddh);
        buf[0]=ddl;
        buf[1]=ddh;
        sbuf[2*ii+0]=ddl;
        sbuf[2*ii+1]=ddh;
        printf("%02x%02x",buf[0]&0xff,buf[1]&0xff);
      }
      printf("\n");
      for(jj=0;jj<13;jj++)if(deviceid_cmd[0]==sbuf[jj]&&deviceid_cmd[1]==sbuf[jj+1]&&deviceid_cmd[2]==sbuf[jj+2]&&deviceid_cmd[3]==sbuf[jj+3]&&deviceid_pos==-99)deviceid_pos=jj;
      for(jj=0;jj<13;jj++)if(cfg_cmd[0]==sbuf[jj]&&cfg_cmd[1]==sbuf[jj+1]&&cfg_cmd[2]==sbuf[jj+2]&&cfg_cmd[3]==sbuf[jj+3]&&cfg_pos==-99)cfg_pos=jj;

      for(mm=0;mm<16;mm++)printf("%02x",sbuf[mm]&0xff);printf("\n");

      if(deviceid_pos>-99){
        if(deviceid_pos==12){
          deviceid_pos=-4;  //defer to next event
        }else{
          for(mm=0;mm<4;mm++){
            a[mm]=sbuf[mm+deviceid_pos+4];
            dshuffle(a,deviceid);
          }                    
          printf(" DEVICEID:%02X%02X%02X%02X\n",deviceid[2]&0xff,deviceid[3]&0xff,deviceid[0]&0xff,deviceid[1]&0xff);
          deviceid_pos=-99;
        }
      }
      if(cfg_pos>-99){
        if(cfg_pos==12){
          cfg_pos=-4;  //defer to next event
        }else{
          for(mm=0;mm<4;mm++)a[mm]=sbuf[mm+cfg_pos+4];
          dshuffle(a,cfg_size);
          printf(" CFG_SIZE:%02X%02X%02X%02X\n",cfg_size[2]&0xff,cfg_size[3]&0xff,cfg_size[0]&0xff,cfg_size[1]&0xff);
          config_size=((cfg_size[3]<<16)&0x00ff0000)|((cfg_size[0]<<8)&0x0000ff00)|(cfg_size[1]&0x000000ff);
	  printf(" config_size %d \n",config_size);
          cfg_pos=-99;
          goto ENDDO;
        }
      }
    }else if(tt==4){
      parms = fscanf(fp,"%4x",&saddr);
    }else if(tt==1){
    }
    fgets(line,1024,fp);
  }
 ENDDO:
  fseek(fp,-31,SEEK_END);
  fscanf(fp,"%02x%02x%02x%02x",(unsigned *)&brd[0],(unsigned *)&brd[1],(unsigned *)&brd[2],(unsigned *)&brd[3]);
  for(mm=0;mm<4;mm++)printf("%02X",brd[mm]&0xff);printf("\n");
  fscanf(fp,"%02x%02x%02x%02x",(unsigned *)&timestamp[0],(unsigned *)&timestamp[1],(unsigned *)&timestamp[2],(unsigned *)&timestamp[3]);
  for(mm=0;mm<4;mm++)printf("%02X",timestamp[mm]&0xff);printf("\n");
  curt=0x00000000;
  curt=curt|((timestamp[0]<<24)&0xff000000)|((timestamp[1]<<16)&0x00ff0000)|((timestamp[2]<<8)&0x0000ff00)|(timestamp[3]&0x000000ff);
  printf(" timestamp %08x \n",curt);
  curt_=(time_t)curt;
  loct_=localtime(&curt_);
  printf("M%02dD%02dY%04dH%02d:%02d",loct_->tm_mon+1,loct_->tm_mday,loct_->tm_year+1900,loct_->tm_hour,loct_->tm_min);
  fclose(fp);

  return pass;
}



int DCFEB::epromread_mcs(const char *fn,FILE *fptst,int show,int loadlowhigh)
{
 
 /* loadlowhigh     0  -   load program at prom address 0x00000000
                     1  -   load program at prom address 0x00055000;
  */
  short int prm_dat[16];
  char line[1024];
  FILE *fp;
  FILE *fout;
  int paddr,lpcnt,parms;
  unsigned int tt;
  unsigned int ddl,ddh;
  unsigned int ll,addr;
  unsigned long int fulladdr,fulloffset;
  unsigned int uaddr = 0,laddr = 0;
  unsigned int luaddr,lladdr;
  // unsigned int send1,send2;
  int ldadd;
  unsigned int saddr;
  char chksum,cs;
  int bad=0;
  unsigned int scnt,scntd,sll[50],addrs[50],saddrs[50],spaddr[50],stt[50],scs[50],sprm_dat[50][16];
  fp = fopen(fn,"r");
  if(fp==NULL){
    printf("Failed to open file %s\n",fn);
    return -1;
  }
  fflush(stdout);
  fout=fopen("out.dat","w");
  fulloffset=0x00000000;
  if(loadlowhigh==1)fulloffset=0x00055000;
  paddr = 0;
  lpcnt = 0;
  tt=0;
  ldadd=0;
  epromdirect_reset();
  epromdirect_enablefifo();
  scnt=0;
  scntd=0;
  while(tt!=1){    
    parms = fscanf(fp,":%2x%4x%2x",&ll,&addr,&tt);
    // printf("%1x:%02X %04X %02X ",parms,ll,addr,tt);
    if(tt==0){
      paddr=0;
      sll[scnt]=ll;addrs[scnt]=addr;saddrs[scnt]=saddr;stt[scnt]=tt;
      for(unsigned int ii=0;ii<ll/2;ii++){
        parms = fscanf(fp,"%2x%2x",&ddl,&ddh);
        prm_dat[paddr] = ((ddh<<8)&0xff00) | (ddl&0x00ff);
        sprm_dat[scnt][paddr]=((ddh<<8)&0xff00) | (ddl&0x00ff);
        scntd++;
        paddr++;
        fulladdr=((saddr<<16)&0xffff0000)|(addr&0x0000ffff);
        fulladdr=fulladdr+fulloffset;
	fulladdr=(fulladdr>>1)&0x7fffffff;
        fulladdr=fulladdr+ii;
        uaddr=(fulladdr>>16)&0xffff;
        laddr=(fulladdr&0xffff);
        luaddr=uaddr;
        lladdr=laddr;
      }
      spaddr[scnt]=paddr;
      fscanf(fp,"%2x",(unsigned *)&cs);
      scs[scnt]=cs;
      scnt++;
      // printf(" scnt %d scntd %d\n",scnt,scntd);
      if(ldadd==0){
	printf(" eprom direct_loadaddress called %04x %04x \n",uaddr,laddr-7);
        epromdirect_loadaddress(uaddr,laddr-7,2);
        ldadd=1;
      }
      if(scntd>=160){
        unsigned short int pdata[16];
        unsigned short int spdata[1000];
        paddr=scntd;
        epromdirect_read(paddr,spdata);
        usleep(10000);
	// printf(" %d data reads \n",paddr);
        int thisdcnt=0;
        for(unsigned int thiscnt=0;thiscnt<scnt;thiscnt++){
          chksum=sll[thiscnt];
          chksum=chksum+((addrs[thiscnt]>>8)&0xff);
          chksum=chksum+(addrs[thiscnt]&0xff);
          chksum=chksum+stt[thiscnt];
          for(unsigned int k=0;k<spaddr[thiscnt];k++){
	    pdata[k]=spdata[thisdcnt];
            prm_dat[k]=sprm_dat[thiscnt][k];
            thisdcnt++;
          }
          for(unsigned int i=0;i<sll[thiscnt]/2;i++){
            chksum=chksum+((pdata[i]>>8)&0xff);
            chksum=chksum+(pdata[i]&0xff);
          }
          if((chksum+scs[thiscnt])&0xff!=0x00)bad++;
	  if(((chksum+scs[thiscnt])&0xff!=0x00)||show==1){
          fprintf(fptst,":%02X %04X %02X ",sll[thiscnt],addrs[thiscnt],stt[thiscnt]);
          for(unsigned int ii=0;ii<spaddr[thiscnt];ii++)fprintf(fptst,"%04x ",prm_dat[ii]&0xffff);fprintf(fptst,"\n");
          fprintf(fptst,"            ");
          for(unsigned int ii=0;ii<spaddr[thiscnt];ii++)fprintf(fptst,"%04x ",pdata[ii]&0xffff);fprintf(fptst,"chksum %02x ",(chksum+scs[thiscnt])&0xff);fprintf(fptst,"\n");
	  }
        } 
        scnt=0;
        scntd=0;
      }
      // printf(" chksum %02x \n",(chksum+cs)&0xff);
    }else if(tt==4){ // newaddress
      if(scnt>0){
        unsigned short int pdata[16];
        unsigned short int spdata[1000];
        paddr=scntd;
        epromdirect_read(paddr,spdata);
	printf(" %d data reads \n",paddr);
        int thisdcnt=0;
        for(unsigned int thiscnt=0;thiscnt<scnt;thiscnt++){
          chksum=sll[thiscnt];
          chksum=chksum+((addrs[thiscnt]>>8)&0xff);
          chksum=chksum+(addrs[thiscnt]&0xff);
          chksum=chksum+stt[thiscnt];
          for(unsigned int k=0;k<spaddr[thiscnt];k++){
	    pdata[k]=spdata[thisdcnt];
            prm_dat[k]=sprm_dat[thiscnt][k];
            thisdcnt++;
          }
          for(unsigned int i=0;i<sll[thiscnt]/2;i++){
            chksum=chksum+((pdata[i]>>8)&0xff);
            chksum=chksum+(pdata[i]&0xff);
          }
          if((chksum+scs[thiscnt])&0xff!=0x00)bad++;
	  if(((chksum+scs[thiscnt])&0xff!=0x00)||show==1){
          fprintf(fptst,":%02X %04X %02X ",sll[thiscnt],addrs[thiscnt],stt[thiscnt]);
          for(unsigned int ii=0;ii<spaddr[thiscnt];ii++)fprintf(fptst,"%04x ",prm_dat[ii]&0xffff);fprintf(fptst,"\n");
          fprintf(fptst,"            ");
          for(unsigned int ii=0;ii<spaddr[thiscnt];ii++)fprintf(fptst,"%04x ",pdata[ii]&0xffff);fprintf(fptst,"chksum %02x ",(chksum+scs[thiscnt])&0xff);fprintf(fptst,"\n");
	  }
        } 
        scnt=0;
        scntd=0;
      }
      parms = fscanf(fp,"%4x",&saddr);
      printf(":%02X %04X %02X",ll,addr,tt); printf(" %4x \n",saddr);fflush(stdout);
      ldadd=0;
    }else if(tt==1){
      if(scnt>0){
        unsigned short int pdata[16];
        unsigned short int spdata[1000];
        paddr=scntd;
        epromdirect_read(paddr,spdata);
	printf(" %d data reads \n",paddr);
        int thisdcnt=0;
        for(unsigned int thiscnt=0;thiscnt<scnt;thiscnt++){
          chksum=sll[thiscnt];
          chksum=chksum+((addrs[thiscnt]>>8)&0xff);
          chksum=chksum+(addrs[thiscnt]&0xff);
          chksum=chksum+stt[thiscnt];
          for(unsigned int k=0;k<spaddr[thiscnt];k++){
	    pdata[k]=spdata[thisdcnt];
            prm_dat[k]=sprm_dat[thiscnt][k];
            thisdcnt++;
          }
          for(unsigned int i=0;i<sll[thiscnt]/2;i++){
            chksum=chksum+((pdata[i]>>8)&0xff);
            chksum=chksum+(pdata[i]&0xff);
          }
          if((chksum+scs[thiscnt])&0xff!=0x00)bad++;
	  if(((chksum+scs[thiscnt])&0xff!=0x00)||show==1){
          fprintf(fptst,":%02X %04X %02X ",sll[thiscnt],addrs[thiscnt],stt[thiscnt]);
          for(unsigned int ii=0;ii<spaddr[thiscnt];ii++)fprintf(fptst,"%04x ",prm_dat[ii]&0xffff);fprintf(fptst,"\n");
          fprintf(fptst,"            ");
          for(unsigned int ii=0;ii<spaddr[thiscnt];ii++)fprintf(fptst,"%04x ",pdata[ii]&0xffff);fprintf(fptst,"chksum %02x ",(chksum+scs[thiscnt])&0xff);fprintf(fptst,"\n");
	  }
        }
        scnt=0;
        scntd=0;
      }
 
    }
    fgets(line,1024,fp);
    lpcnt++;
  }
  fclose(fp);
  fclose(fout);
  return bad;
}

void DCFEB::rdbkvirtex6()
{
  // int j;
  char a[4];
  // int fdexp,fdmsk;

  DEVTYPE dv = dscamDevice();
  printf(" dv %d \n",dv);
 
  FILE *fprbk=fopen("/tmp/rbk2.dat","w");
  // FILE *fdexp=fopen("/tmp/CurrentDCFEB.rbb","r");
  // FILE *fdmsk=fopen("/tmp/CurrentDCFEB.msk","r");
  FILE *faexp=fopen("/tmp/CurrentDCFEB.rba","r");
  FILE *famsk=fopen("/tmp/CurrentDCFEB.msd","r");
  FILE *fprbb=fopen("/tmp/rbb.dat","w");
  FILE *fpmsk=fopen("/tmp/msk.dat","w");
  char tmp[374];
  /* the expected and readback seem to be in sync. the mask bits seem to as 
     well but it wouldn't be suprising if the mask bits need another shift. */
  // rdbkmskexp(fdexp,374,tmp);  // there is a bug!? the binary files lose bytees
  // rdbkmskexp(fdmsk,26,tmp);
  rdbkmskexp_asci(faexp,1,tmp,1);
  rdbkmskexp_asci(famsk,1,tmp,2);
  // restore idle 
  devdo(dv,-1,cmd,0,sndbuf,rcvbuf,2);
 // initialization
  cmd[0]=(VTX6_CFG_IN&0xff);
  cmd[1]=((VTX6_CFG_IN&0x300)>>8);    
 
 // from page 136 ug360 Virtex-6 Configuration users guide
  a[0]=0xFF; //dummy word
  a[1]=0xFF;
  a[2]=0xFF;
  a[3]=0xFF;
  dshuffle2(a,sndbuf,0);
  a[0]=0xAA; // sync word
  a[1]=0x99;
  a[2]=0x55;
  a[3]=0x66;
  dshuffle2(a,sndbuf,4);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,8);
  a[0]=0x30; // type 1 write 1 word to cmd
  a[1]=0x00;
  a[2]=0x80;
  a[3]=0x01;
  dshuffle2(a,sndbuf,12);
  a[0]=0x00; // rcrc command
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x07;
  dshuffle2(a,sndbuf,16);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,20);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,24);
  //  for(int kk=0;kk<28;kk++)printf(" %02x",sndbuf[27-kk]&0xff);printf("\n");
  devdo(dv,10,cmd,28*8,sndbuf,rcvbuf,2);

  /*
  cmd[0]=(VTX6_SHUTDN&0xff);
  cmd[1]=((VTX6_SHUTDN&0x300)>>8);    
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
  printf("VTX6_SHUTDN %02x%02x \n",cmd[1]&0xff,cmd[0]&0xff);
  */
  // read stuff
  cmd[0]=(VTX6_CFG_IN&0xff);
  cmd[1]=((VTX6_CFG_IN&0x300)>>8);
  // printf("VTX6_CFG_IN %02x%02x \n",cmd[1]&0xff,cmd[0]&0xff);    
 // from page 136 ug360 Virtex-6 Configuration users guide
  a[0]=0xFF; //dummy word
  a[1]=0xFF;
  a[2]=0xFF;
  a[3]=0xFF;
  dshuffle2(a,sndbuf,0);
  a[0]=0xAA; // sync word
  a[1]=0x99;
  a[2]=0x55;
  a[3]=0x66;
  dshuffle2(a,sndbuf,4);
  // printf(" sndbuf sync %02x %02x %02x %02x \n",sndbuf[4]&0xff,sndbuf[5]&0xff,sndbuf[6]&0xff,sndbuf[7]&0xff);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,8);
  a[0]=0x30; // type 1 write 1 word to cmd
  a[1]=0x00;
  a[2]=0x80;
  a[3]=0x01;
  dshuffle2(a,sndbuf,12);
  a[0]=0x00; // rcfg command
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x04;
  dshuffle2(a,sndbuf,16);
  a[0]=0x30; // type 1 write 1 word to far
  a[1]=0x00;
  a[2]=0x20;
  a[3]=0x01;
  dshuffle2(a,sndbuf,20);
  a[0]=0x00; // far address = 00000000
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,24);
  a[0]=0x28; // read 0
  a[1]=0x00;
  a[2]=0x60;
  a[3]=0x00;
  dshuffle2(a,sndbuf,28);
  a[0]=0x48; // read nread
  a[1]=0x14;  
  a[2]=0xd0;
  a[3]=0x48;
  //  a[1]=0x00; //810
  // a[2]=0x03;
  //a[3]=0x2A;
  //  a[2]=((nread>>8)&0xff);
  // a[3]=(nread&0x00ff);
  // printf(" nread %08x \n",nread);
  // printf(" read command %02x%02x%02x%02x\n",a[0]&0xff,a[1]&0xff,a[2]&0xff,a[3]&0xff);
  dshuffle2(a,sndbuf,32);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,36);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,40);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,44);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,48);
  for(int kk=0;kk<52;kk++)printf(" %02x",sndbuf[43-kk]&0xff);printf("\n");
  devdo(dv,10,cmd,52*8,sndbuf,rcvbuf,2);
  //sleep(1);
  cmd[0]=(VTX6_CFG_OUT&0xff);
  cmd[1]=((VTX6_CFG_OUT&0x300)>>8);    
  printf("VTX6_CFEB_OUT %02x%02x \n",cmd[1]&0xff,cmd[0]&0xff);
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,0);
  printf(" READBACK: \n");
  // for(int tenblks=0;tenblks<1684;tenblks++){
  // int doshft=0;
  for(int tenblks=0;tenblks<1684;tenblks++){
    printf("\n TENBLOCKS %d:\n",tenblks);
    int totbits=810*32;
    /*    char exp[810*4+10];
    char msk[810*4+10];
    rdbkmskexp(fdexp,810*4,exp);
    rdbkmskexp(fdmsk,810*4,msk); */
    char expa[810*4+10];
    char mska[810*4+10];
    rdbkmskexp_asci(faexp,810*4,expa,0);
    rdbkmskexp_asci(famsk,810*4,mska,0);
    char data_ptr[810*4+10];
    char rtn_ptr[810*4+10];
    if(tenblks==0)scan_dmb_headtail(DATA_REG,data_ptr,totbits,rtn_ptr,1,3,1);
    if(tenblks!=0&&tenblks!=1683)scan_dmb_headtail(DATA_REG,data_ptr,totbits,rtn_ptr,1,0,1);
    if(tenblks==1683)scan_dmb_headtail(DATA_REG,data_ptr,totbits,rtn_ptr,1,1,1);
    usleep(10);
    int byte=0;
    for(unsigned int row=0;row<90;row++){
      int ibad=0;
      for(unsigned int chk=0;chk<36;chk++)if(((rtn_ptr[byte+chk]^expa[byte+chk])&~mska[byte+chk])!=0x00)ibad=1;
      for(unsigned int col=0;col<36;col++)fprintf(fprbk,"%02x",rtn_ptr[byte+col]&0xff);
      if(ibad==0){fprintf(fprbk,"\n");}else{fprintf(fprbk,"   *** \n");}
      for(unsigned int col=0;col<36;col++)fprintf(fprbb,"%02x",expa[byte+col]&0xff);
      if(ibad==0){fprintf(fprbb,"\n");}else{fprintf(fprbb,"   *** \n");}
      for(unsigned int col=0;col<36;col++)fprintf(fpmsk,"%02x",mska[byte+col]&0xff);
      if(ibad==0){fprintf(fpmsk,"\n");}else{fprintf(fpmsk,"   *** \n");}
      if(ibad!=1263){
        printf("rbk:");
        for(unsigned int col=0;col<36;col++)printf("%02x",rtn_ptr[byte+col]&0xff);
        printf("\nexp:");
        for(unsigned int col=0;col<36;col++)printf("%02x",expa[byte+col]&0xff);
        printf("\nmsk:");
        for(unsigned int col=0;col<36;col++)printf("%02x",mska[byte+col]&0xff);
        printf("\n\n");
      }
      byte=byte+36;
    }
  }
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);    
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
  // fclose(fdexp);
  // fclose(fdmsk);
  fclose(faexp);
  fclose(famsk);
  fclose(fprbk);
  fclose(fprbb);
  fclose(fpmsk);
return;
}

int DCFEB::check_rdbkvirtex6(FILE *fpout){
  FILE *fmsk;
  FILE *fexp;
  FILE *fgot;
  int i,j;
  char line1[120],line2[120],line3[120];
  char got[36],msk[36],exp[36];
  int chk,ichk;
  int bad=0;
  // printf(" entered \n");
  fgot=fopen("/tmp/rbk2.dat","r");
  fexp=fopen("/tmp/rbb.dat","r");
  fmsk=fopen("/tmp/msk.dat","r");
  // printf(" fgot %d \n",fgot);
  for(i=0;i<151560;i++){
    fgets(line1,120,fgot);
    fgets(line2,120,fexp);
    fgets(line3,120,fmsk);
    sscanf(line1,"%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",&got[0],&got[1],&got[2],&got[3],&got[4],&got[5],&got[6],&got[7],&got[8],&got[9],&got[10],&got[11],&got[12],&got[13],&got[14],&got[15],&got[16],&got[17],&got[18],&got[19],&got[20],&got[21],&got[22],&got[23],&got[24],&got[25],&got[26],&got[27],&got[28],&got[29],&got[30],&got[31],&got[32],&got[33],&got[34],&got[35]);
    sscanf(line2,"%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",&exp[0],&exp[1],&exp[2],&exp[3],&exp[4],&exp[5],&exp[6],&exp[7],&exp[8],&exp[9],&exp[10],&exp[11],&exp[12],&exp[13],&exp[14],&exp[15],&exp[16],&exp[17],&exp[18],&exp[19],&exp[20],&exp[21],&exp[22],&exp[23],&exp[24],&exp[25],&exp[26],&exp[27],&exp[28],&exp[29],&exp[30],&exp[31],&exp[32],&exp[33],&exp[34],&exp[35]);
   sscanf(line3,"%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",&msk[0],&msk[1],&msk[2],&msk[3],&msk[4],&msk[5],&msk[6],&msk[7],&msk[8],&msk[9],&msk[10],&msk[11],&msk[12],&msk[13],&msk[14],&msk[15],&msk[16],&msk[17],&msk[18],&msk[19],&msk[20],&msk[21],&msk[22],&msk[23],&msk[24],&msk[25],&msk[26],&msk[27],&msk[28],&msk[29],&msk[30],&msk[31],&msk[32],&msk[33],&msk[34],&msk[35]);
   chk=0;
   for(j=0;j<36;j++)if(((got[j]^exp[j])&(~msk[j]))!=0x00){chk=chk+1;ichk=j;}
   if(chk!=0){
     bad=bad+chk;
      fprintf(fpout,"%06d got:",i);for(j=0;j<36;j++)fprintf(fpout,"%02x",got[j]&0xff);fprintf(fpout,"\n");
      fprintf(fpout,"       exp:");for(j=0;j<36;j++)fprintf(fpout,"%02x",exp[j]&0xff);fprintf(fpout,"\n");
      fprintf(fpout,"       msk:");for(j=0;j<36;j++)fprintf(fpout,"%02x",msk[j]&0xff);fprintf(fpout,"\n");
    }
  }
  return bad;
}
   
    
void DCFEB::rdbkframevirtex6(unsigned int far,char *fpga)
{
  /* smallest addressable element in a virtex6 is a 81 word (32 bit word) frame */
  // int j;
  char a[4];
  // int fdexp,fdmsk;

  DEVTYPE dv = dscamDevice();
  printf(" dv %d \n",dv);  // restore idle 
  devdo(dv,-1,cmd,0,sndbuf,rcvbuf,2);
  cmd[0]=(VTX6_CFG_IN&0xff);
  cmd[1]=((VTX6_CFG_IN&0x300)>>8);
  // printf("VTX6_CFG_IN %02x%02x \n",cmd[1]&0xff,cmd[0]&0xff);    
 // from page 136 ug360 Virtex-6 Configuration users guide
  a[0]=0xFF; //dummy word
  a[1]=0xFF;
  a[2]=0xFF;
  a[3]=0xFF;
  dshuffle2(a,sndbuf,0);
  a[0]=0xAA; // sync word
  a[1]=0x99;
  a[2]=0x55;
  a[3]=0x66;
  dshuffle2(a,sndbuf,4);
  // printf(" sndbuf sync %02x %02x %02x %02x \n",sndbuf[4]&0xff,sndbuf[5]&0xff,sndbuf[6]&0xff,sndbuf[7]&0xff);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,8);
  a[0]=0x30; // type 1 write 1 word to cmd
  a[1]=0x00;
  a[2]=0x80;
  a[3]=0x01;
  dshuffle2(a,sndbuf,12);
  a[0]=0x00; // rcfg command
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x04;
  dshuffle2(a,sndbuf,16);
  a[0]=0x30; // type 1 write 1 word to far
  a[1]=0x00;
  a[2]=0x20;
  a[3]=0x01;
  dshuffle2(a,sndbuf,20);
  a[0]=(far>>24)&0xff;  // far  = 00000000
  a[1]=(far>>16)&0xff;
  a[2]=(far>>8)&0xff;
  a[3]=far&0xff;
  printf(" far %02x%02x%02x%02x \n",a[0]&0xff,a[1]&0xff,a[2]&0xff,a[3]&0xff);
  dshuffle2(a,sndbuf,24);
  a[0]=0x28; // read 0
  a[1]=0x00;
  a[2]=0x60;
  a[3]=0x00;
  dshuffle2(a,sndbuf,28);
  int nread=2*81; //xxx2
  a[0]=0x48; // read nread
  a[1]=0x00;  
  a[2]=((nread>>8)&0xff);
  a[3]=(nread&0x00ff);
  printf(" a[2-3] %02x %02x \n",a[2]&0xff,a[3]&0xff);
  dshuffle2(a,sndbuf,32);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,36);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,40);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,44);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,48);
  //  for(int kk=0;kk<52;kk++)printf(" %02x",sndbuf[43-kk]&0xff);printf("\n");
  devdo(dv,10,cmd,52*8,sndbuf,rcvbuf,2);
  cmd[0]=(VTX6_CFG_OUT&0xff);
  cmd[1]=((VTX6_CFG_OUT&0x300)>>8);    
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,0);
  char data_ptr[2*4*81+10]; //xxx3
  char rtn_ptr[2*4*81+10];  //xxx3
  char out[2*81*4];  //xxx3
  scan_dmb(DATA_REG,data_ptr,2*2592,rtn_ptr,1,1); //xxx3
  for(int i=0;i<4*81;i++){out[i]=rtn_ptr[i+324];fpga[i]=rtn_ptr[i+324];}
  /*  int byte=0;
  printf(" virtex6 readback %d \n",frame);
  for(int cnt=0;cnt<2;cnt++){
  for(int i=0;i<9;i++){
    for(unsigned int col=0;col<18;col++)printf("%02x",out[byte+col]&0xff);
    printf("\n");
    byte=byte+18;
  }
  }*/
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);    
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);

  return;
}

void DCFEB::epromread_frame(unsigned int frame,unsigned short int *spdata)
{
  unsigned long int fulladdr;
  unsigned int uaddr,laddr;
  unsigned short int paddr;
  unsigned short int offset=118;
  printf(" icfeb %d \n",number());fflush(stdout);
  DEVTYPE dv = dscamDevice();
  printf(" dv %d \n",dv);  fflush(stdout);// restore idle
  devdo(dv,-1,cmd,0,sndbuf,rcvbuf,2);
  if(frame==0xffffffff){frame=0x00000000;offset=118;}  // check fpga commands
  fulladdr=0x00000000;
  fulladdr=((frame)*2*81&0x0fffffff)+offset;
  paddr=2*81;
  epromdirect_reset();
  epromdirect_enablefifo();
  uaddr=(fulladdr>>16)&0xffff;
  laddr=(fulladdr&0xffff);
  printf(" uaddr %04x laddr %04x \n",uaddr,laddr); fflush(stdout);
  epromdirect_loadaddress(uaddr,laddr,2);
  epromdirect_read(paddr,spdata);
  /* printf("eprom frame read %d:\n",frame);fflush(stdout);
  //  for(int k=0;k<4;k++){
  for(int j=0;j<18;j++){
    for(int i=0;i<9;i++)printf("%04x",spdata[9*j+i]&0xffff);printf("\n");
  }
  printf("\n"); fflush(stdout);*/
  //
}

void DCFEB::wrtframevirtex6(unsigned int frame)
{
  // should work but untested
  // disable jtag routines until tested
  // int j;
  char a[4];
  char expa[81*4];
  // int fdexp,fdmsk;

  DEVTYPE dv = dscamDevice();
  printf(" dv %d \n",dv);  // restore idle 
  devdo(dv,-1,cmd,0,sndbuf,rcvbuf,2);
  FILE *faexp=fopen("/tmp/CurrentDCFEB.rba","r");
  char tmp[10];
  rdbkmskexp_asci(faexp,1,tmp,1);
  for(unsigned int i=0;i<frame+2;i++)rdbkmskexp_asci(faexp,81*4,expa,0);
  cmd[0]=(VTX6_CFG_IN&0xff);
  cmd[1]=((VTX6_CFG_IN&0x300)>>8);
  // printf("VTX6_CFG_IN %02x%02x \n",cmd[1]&0xff,cmd[0]&0xff);    
  a[0]=0xFF; //dummy word
  a[1]=0xFF;
  a[2]=0xFF;
  a[3]=0xFF;
  dshuffle2(a,sndbuf,0);
  a[0]=0xAA; // sync word
  a[1]=0x99;
  a[2]=0x55;
  a[3]=0x66;
  dshuffle2(a,sndbuf,4);
  // printf(" sndbuf sync %02x %02x %02x %02x \n",sndbuf[4]&0xff,sndbuf[5]&0xff,sndbuf[6]&0xff,sndbuf[7]&0xff);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,8);
  a[0]=0x30; // type 1 write 1 word to cmd
  a[1]=0x00;
  a[2]=0x80;
  a[3]=0x01;
  dshuffle2(a,sndbuf,12);
  a[0]=0x00; // rcrc command
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x07;  
  dshuffle2(a,sndbuf,16);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,20);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,24);
  a[0]=0x30; // write 1 word to ID
  a[1]=0x01;
  a[2]=0x80;
  a[3]=0x01;
  dshuffle2(a,sndbuf,28);
  a[0]=0x44; // IDCODE
  a[1]=0x24;
  a[2]=0xA0;
  a[3]=0x93;
  dshuffle2(a,sndbuf,32);
  a[0]=0x30; // type 1 write 1 word to cmd
  a[1]=0x00;
  a[2]=0x80;
  a[3]=0x01;
  dshuffle2(a,sndbuf,36);
  a[0]=0x00; // NULL Command
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,40);
  a[0]=0x30; // write 1 word to far
  a[1]=0x00;
  a[2]=0x20;
  a[3]=0x01;
  dshuffle2(a,sndbuf,44);
  a[0]=0x00;  // far address = 00000000
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,48);
  a[0]=0x30; // type 1 write 1 word to cmd
  a[1]=0x00;
  a[2]=0x80;
  a[3]=0x01;
  dshuffle2(a,sndbuf,52);
  a[0]=0x00; // WCFG
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x01;
  dshuffle2(a,sndbuf,54);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,58);
  a[0]=0x30; // write 1 word to far
  a[1]=0x00;
  a[2]=0x20;
  a[3]=0x01;
  dshuffle2(a,sndbuf,62);
  a[0]=(frame>>24)&0xff;  // far address = 00000000
  a[1]=(frame>>16)&0xff;
  a[2]=(frame>>8)&0xff;
  a[3]=frame&0xff;
  dshuffle2(a,sndbuf,64);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,68);
  a[0]=0x28; // write 81 words (0x51) to FDRI
  a[1]=0x00;
  a[2]=0x60;
  a[3]=0x00;
  dshuffle2(a,sndbuf,72);
  int nread=81;
  a[0]=0x30; // read nread
  a[1]=0x00;  
  a[2]=((nread>>8)&0x0f)|0x40;
  a[3]=(nread&0x00ff);
  printf(" a[2-3] %02x %02x \n",a[2]&0xff,a[3]&0xff);
  dshuffle2(a,sndbuf,76);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,80);
   //  for(int kk=0;kk<84;kk++)printf(" %02x",sndbuf[43-kk]&0xff);printf("\n");
  //  devdo(dv,10,cmd,84*8,sndbuf,rcvbuf,2);
  cmd[0]=(VTX6_CFG_IN&0xff);
  cmd[1]=((VTX6_CFG_IN&0x300)>>8);    
  //  devdo(dv,10,cmd,0,sndbuf,rcvbuf,0);
  char data_ptr[81*4+10];
  char out[810*4];
  for(int i=0;i<84*4;i++)data_ptr[i]=expa[i];
  // scan_dmb(DATA_REG,data_ptr,2592,rtn_ptr,1,1);
  for(int i=0;i<324;i++)out[i]=data_ptr[i];
  int byte=0;
  for(int i=0;i<18;i++){
    printf("wrt:");
    for(unsigned int col=0;col<36;col++)printf("%02x",out[byte+col]&0xff);
    printf("\n");
    byte=byte+36;
  }
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);    
  //  devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
  fclose(faexp);
  return;
}

void DCFEB::LoadFPGACompare_Frame(int framenumber,char* comp){

  /* virtex-6 xc6vlx130t: there are 16840 cfg frames and 16860 total frames.
     the asci files have the 20 extra frames included (not cheack)at 
     upperaddressa boundaries. eprom and virtex6 frame reads can't see 
     these extra frames. Thus when we hit uppderaddra boundaries we must
     do 2 reads
   */
  unsigned int upperaddra[10]={0x00000000,0x00008000,0x00100000,0x00108000,0x00110000,0x00200000,0x00208000,0x00300000,0x00308000,0x00310000};
  char tmp[324];
  unsigned int far=frame2far(framenumber);
  int extra=0;
  for(int i=0;i<10;i++)if(upperaddra[i]<=far)extra=extra+2;
  FILE *faexp=fopen("/tmp/CurrentDCFEB.rba","r");
  rdbkmskexp_asci(faexp,1,tmp,1);
  for(int i=0;i<framenumber-1+extra;i++)rdbkmskexp_asci(faexp,4*81,tmp,0);
  rdbkmskexp_asci(faexp,4*81,tmp,0);
  for(int i=0;i<324;i++)comp[i]=tmp[i];
  /* printf(" compare file:\n");
  for(int i=0;i<18;i++){
    for(int j=0;j<18;j++)printf("%02x",comp[18*i+j]&0xff);printf("\n");    
    }*/
  fclose(faexp);
}

void DCFEB::LoadFPGAMask_Frame(int framenumber,char *msk){
  unsigned int upperaddra[10]={0x00000000,0x00008000,0x00100000,0x00108000,0x00110000,0x00200000,0x00208000,0x00300000,0x00308000,0x00310000};
  char tmp[324];
  unsigned int far=frame2far(framenumber);
  int extra=0;
  for(int i=0;i<10;i++)if(upperaddra[i]<=far)extra=extra+2;
  FILE *famsk=fopen("/tmp/CurrentDCFEB.msd","r");
  rdbkmskexp_asci(famsk,1,tmp,2);
  for(int i=0;i<framenumber-1+extra;i++)rdbkmskexp_asci(famsk,4*81,tmp,0);
  rdbkmskexp_asci(famsk,4*81,tmp,0);
  
  for(int i=0;i<324;i++)msk[i]=tmp[i];
  /* printf(" msk file:\n");
  for(int i=0;i<18;i++){
    for(int j=0;j<18;j++)printf("%02x",msk[18*i+j]&0xff);printf("\n");    
    } */
  fclose(famsk);
}

void DCFEB::FPGAComp_Frame_makefile_randomaccess(){
  char tmp[324]; 
  unsigned int upperaddra[10]={0x00000000,0x00008000,0x00100000,0x00108000,0x00110000,0x00200000,0x00208000,0x00300000,0x00308000,0x00310000};
  FILE *fbin=fopen("/tmp/ranaccess.rba","wb");
  FILE *facmp=fopen("/tmp/CurrentDCFEB.rba","r");
  rdbkmskexp_asci(facmp,1,tmp,1);
  for(int i=0;i<16840;i++){
    int framenumber=i;
    unsigned int far=frame2far(framenumber);
    for(int j=0;j<10;j++)if(upperaddra[j]==far){
      printf(" extra read at address %08d \n",far); 
      rdbkmskexp_asci(facmp,4*81,tmp,0);
      if(j!=0)rdbkmskexp_asci(facmp,4*81,tmp,0);
    }
    rdbkmskexp_asci(facmp,4*81,tmp,0);
    fwrite(tmp,1,4*81,fbin);
  }
  fclose(fbin);
  fclose(facmp);
}

void DCFEB::FPGAMask_Frame_makefile_randomaccess(){
  char tmp[324]; 
  unsigned int upperaddra[10]={0x00000000,0x00008000,0x00100000,0x00108000,0x00110000,0x00200000,0x00208000,0x00300000,0x00308000,0x00310000};
  FILE *fbin=fopen("/tmp/ranaccess.msd","wb");
  FILE *famsk=fopen("/tmp/CurrentDCFEB.msd","r");
  rdbkmskexp_asci(famsk,1,tmp,2);
  for(int i=0;i<16840;i++){
    int framenumber=i;
    unsigned int far=frame2far(framenumber);
    for(int j=0;j<10;j++)if(upperaddra[j]==far){ 
      printf(" extra read  at address %08x \n",far);
      rdbkmskexp_asci(famsk,4*81,tmp,0);
      if(j!=0)rdbkmskexp_asci(famsk,4*81,tmp,0);
    }
    rdbkmskexp_asci(famsk,4*81,tmp,0);
    fwrite(tmp,1,4*81,fbin);
  }
  fclose(fbin);
  fclose(famsk);
}

void DCFEB::LoadFPGAComp_Frame_randomaccess(int framenumber,char *cmp){
  FILE *fbin=fopen("/tmp/ranaccess.rba","rb");
  fseek(fbin,framenumber*324,SEEK_SET);
  fread(cmp,1,324,fbin);
  fclose(fbin);
}

void DCFEB::LoadFPGAMask_Frame_randomaccess(int framenumber,char *msk){
  FILE *fbin=fopen("/tmp/ranaccess.msd","rb");
  fseek(fbin,framenumber*324,SEEK_SET);
  fread(msk,1,324,fbin);
  fclose(fbin);
}

void DCFEB::CheckFPGAEPROMFILE_Frames(int framenumber){
  char cmp[324],msk[324],fpga[324],eprom[324];
  unsigned short int teprom[81*4];
  printf(" entered CheckFPGAEPROMFILE_FRAMES \n");fflush(stdout);
  LoadFPGAComp_Frame_randomaccess(framenumber,cmp);
  printf(" about to call FPGAMask \n");fflush(stdout);
  LoadFPGAMask_Frame_randomaccess(framenumber,msk);
  unsigned int far=frame2far(framenumber);
  rdbkframevirtex6(far,fpga);
  printf(" about to call epromread_frame \n");fflush(stdout);
  epromread_frame(framenumber,teprom);
  for(int i=0;i<162;i++){
    eprom[2*i+1]=(teprom[i]&0x00ff);
    eprom[2*i+0]=((teprom[i]>>8)&0xff);
  }
  int badfpga=0;
  int badeprom=0;
  for(int i=0;i<18;i++){
    for(int j=0;j<18;j++){
     if((fpga[18*i+j]^cmp[18*i+j])&~msk[18*i+j])badfpga++;
     if(eprom[18*i+j]!=cmp[18*i+j])badeprom++;
    }
  }
  if(badfpga>0){
  badfpga=0;
  badeprom=0;
  char end1[5],end2[5];
  printf("Frame Number: %d \n",framenumber);
  for(int i=0;i<18;i++){
    printf("%02d  fpga:",i);
    sprintf(end1," ");
    for(int j=0;j<18;j++){ 
     if((fpga[18*i+j]^cmp[18*i+j])&~msk[18*i+j]){
        badfpga++;
        sprintf(end1,"*");
      }
      printf("%02x",fpga[18*i+j]&0xff);
    }
    printf("%s\n",end1);
    printf("   eprom:");
    sprintf(end2," ");
    for(int j=0;j<18;j++){
      if(eprom[18*i+j]!=cmp[18*i+j]){
        badeprom++;
        sprintf(end2,"*");
      }
      printf("%02x",eprom[18*i+j]&0xff);
    }
    printf("%s\n",end2);
    printf("     cmp:");
    for(int j=0;j<18;j++)printf("%02x",cmp[18*i+j]&0xff);printf("\n");
    printf("     msk:");
    for(int j=0;j<18;j++)printf("%02x",msk[18*i+j]&0xff);printf("\n");
  }
    printf(" badfpga %d badeprom %d \n",badfpga,badeprom);
    if(badfpga>0)printf("Catastrophy!\n");
  }
}

unsigned int  DCFEB::frame2far(unsigned int frame){
  // for virtex-6 xc6vlx130t   
/* frame to far mapping determined experimentally
     reading 2 frames at a time and reading out far 
     -LSD */
  int i;
  unsigned int far;
  unsigned int tframe;
  int ihadd,iladd = 0;
  unsigned int increment,increment2;
  unsigned int totframesa=2472;
  unsigned int numberofframesa[71]={44,36,36,36,36,28,36,36,28,36,36,36,36,28,36,36,28,36,36,28,36,36,28,36,36,44,36,36,36,36,36,36,36,36,36,36,38,36,36,36,36,44,36,36,28,36,36,28,36,36,28,36,36,28,36,36,36,36,28,36,36,28,36,36,36,36,36,36,36,28,30};
  unsigned int upperaddra[5]={0x00000000,0x00008000,0x00100000,0x00108000,0x00110000};
  unsigned int totframesb=896;  
  // unsigned int numberofframesb=896;
  unsigned int upperaddrb[5]={0x00200000,0x00208000,0x00300000,0x00308000,0x00310000};
  unsigned int low[71],high[71];
  unsigned int sum[72];
  increment=0x00000000;
  increment2=0;
  sum[0]=0;
  for(i=0;i<71;i++){
    low[i]=increment;
    high[i]=increment+numberofframesa[i]-1;
    sum[i+1]=sum[i]+numberofframesa[i];
    increment=increment+0x00000080;
  }
  tframe=frame;
  if(frame<5*totframesa){  // a frames
    ihadd=frame/totframesa;
    tframe=frame-ihadd*totframesa;
    for(i=0;i<71;i++){
      if(tframe>=sum[i]&&tframe<=sum[i+1])iladd=i;
    }
    tframe=tframe-sum[iladd];
    far=upperaddra[ihadd]+low[iladd]+tframe;
  }else{                   // b frames
    ihadd=(frame-5*totframesa)/totframesb;
   
    tframe=frame-5*totframesa-ihadd*totframesb;
    far=upperaddrb[ihadd]+tframe;
  }
  return far;
}

int DCFEB::far2frame(unsigned int far){ 
  // for virtex-6 xc6vlx130t
  int frame=0; 
  unsigned int tfar;
  int i;
  unsigned int totframesa=2472;
  unsigned int numberofframesa[71]={44,36,36,36,36,28,36,36,28,36,36,36,36,28,36,36,28,36,36,28,36,36,28,36,36,44,36,36,36,36,36,36,36,36,36,36,38,36,36,36,36,44,36,36,28,36,36,28,36,36,28,36,36,28,36,36,36,36,28,36,36,28,36,36,36,36,36,36,36,28,30};
  unsigned int upperaddra[5]={0x00000000,0x00008000,0x00100000,0x00108000,0x00110000};
  unsigned int totframesb=896;
  //unsigned int numberofframesb=896;
  unsigned int upperaddrb[5]={0x00200000,0x00208000,0x00300000,0x00308000,0x00310000};
  int sum[72];
  unsigned int low[71],high[71];
  unsigned int increment,increment2;
  int type; //a-0 b-1
  int addtype = 0;
  int posa; 
  increment=0x00000000;
  increment2=0;
  sum[0]=0;
  for(i=0;i<71;i++){
    low[i]=increment;
    high[i]=increment+numberofframesa[i]-1;
    sum[i+1]=sum[i]+numberofframesa[i];
    increment=increment+0x00000080;
  }
  type=-1;
  for(i=4;i>-1;i--){
    if(far/upperaddrb[i]>0&&type==-1){
      type=1;
      addtype=i;
    }
  }
  if(type==-1){
   for(i=4;i>0;i--){
    if(far/upperaddra[i]>0&&type==-1){
      type=0;
      addtype=i;
    }
   }
   if(type==-1){
     type=0;
     addtype=0;
   }
  }
  if(type==1){                         // b frame
    frame=5*totframesa;
    frame=frame+addtype*totframesb+(0x00000fff&far); 
    return frame;
  }
  if(type==0)frame=addtype*totframesa; // a frame
  tfar=far-upperaddra[addtype];
  posa=-99;
  for(i=0;i<71;i++){
    if(low[i]<=tfar&&tfar<=high[i])posa=i;
  }
  // printf(" type %d addtype %d posa %d \n",type,addtype,posa);fflush(stdout);
  if(posa==-99)return -99;
  frame=frame+tfar-low[posa]+sum[posa];
  return frame;
}

unsigned long int  DCFEB::febvirtex6dna()
{
  DEVTYPE dv = dscamDevice();
  printf(" febfgpaid: dv %d ",dv);
  cmd[0]=(VTX6_ISC_ENABLE&0xff);
  cmd[1]=((VTX6_ISC_ENABLE&0x300)>>8);
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,1);
  cmd[0]=(VTX6_ISC_DNA&0xff);
  cmd[1]=((VTX6_ISC_DNA&0x300)>>8);
  sndbuf[0]=0xFF;
  sndbuf[1]=0xFF;
  sndbuf[2]=0xFF;
  sndbuf[3]=0xFF;
  sndbuf[4]=0xFF;
  sndbuf[5]=0xFF;
  sndbuf[6]=0xFF;
  sndbuf[7]=0xFF;
  devdo(dv,10,cmd,64,sndbuf,rcvbuf,1);
  (*MyOutput_) << " The FEB " << dv-F1DCFEBM+1 << std::endl;
  (*MyOutput_) << " The FPGA DNA is " << std::hex << std::hex <<(0xff&rcvbuf[7]) << (0xff&rcvbuf[6]) << (0xff&rcvbuf[5]) << (0xff&rcvbuf[4])<<(0xff&rcvbuf[3]) << (0xff&rcvbuf[2]) << (0xff&rcvbuf[1]) << (0xff&rcvbuf[0]) << std::endl;
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  sndbuf[0]=0;
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,0);
  unsigned long int ibrd=0x0000000000000000;
  ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24)|ibrd;
  ibrd=((rcvbuf[4]&0xff)<<32)|((rcvbuf[5]&0xff)<<40)|((rcvbuf[6]&0xff)<<48)|((rcvbuf[7]&0xff)<<56)|ibrd;
  return ibrd;
}


void DCFEB::rdbkstatusvirtex6()
{
   printf("Virtex 6 status: %08x \n",rdbkregvirtex6(VTX6_REG_STAT));
  return;
} 

void DCFEB::rdbkidcodevirtex6()
{

  printf("Virtex 6 IDCODE: %08x \n",rdbkregvirtex6(VTX6_REG_IDCODE));
  return;
}

unsigned int DCFEB::rdbkfarvirtex6()
{
  unsigned int far=rdbkregvirtex6(VTX6_REG_FAR);
  printf("Virtex 6 IDCODE: %08x \n",far);
  return far;
}

void DCFEB::rdbkctl0virtex6()
{
  printf("Virtex 6 ctl0: %08x \n",rdbkregvirtex6(VTX6_REG_CTL0));
  return;
}

void DCFEB::rdbkcrcvirtex6()
{
  printf("Virtex 6 CRC: %08x \n",rdbkregvirtex6(VTX6_REG_CRC));
  return;
}

void DCFEB::wrtcrcvirtex6()
{  
  wrtregvirtex6(VTX6_REG_CRC,0x00000000);
  return;
}

void DCFEB::rdbkwbstarvirtex6()
{
  printf("Virtex 6 WBSTAR: %08x \n",rdbkregvirtex6(VTX6_REG_WBSTAR));
  return;
}

void DCFEB::wrtwbstarvirtex6(unsigned int reg)
{   
  wrtregvirtex6(VTX6_REG_WBSTAR,reg);
  return;
}

unsigned int DCFEB::rdbkregvirtex6(int REGNAME)
{
  char a[4];
  unsigned int value;
  DEVTYPE dv = dscamDevice();
                                                            
  // shift to device
  cmd[0]=(VTX6_CFG_IN&0xff);
  cmd[1]=((VTX6_CFG_IN&0x300)>>8);
  a[0]=0xAA; // sync word
  a[1]=0x99;
  a[2]=0x55;
  a[3]=0x66;
  dshuffle2(a,sndbuf,0);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,4);
  a[0]=0x28; // read register
  a[1]=(REGNAME>>3)&0x03;
  a[2]=(((REGNAME&0x07)<<5)&0xe0);
  a[3]=0x01;
  dshuffle2(a,sndbuf,8);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,12);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,16);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,20);
  devdo(dv,10,cmd,23*8,sndbuf,rcvbuf,2);
  cmd[0]=(VTX6_CFG_OUT&0xff);
  cmd[1]=((VTX6_CFG_OUT&0x300)>>8);
  devdo(dv,10,cmd,32,sndbuf,rcvbuf,1);
  dshuffle2(rcvbuf,a,0);
  value=0x00000000;
  value=((a[0]<<24)&0xff000000)|((a[1]<<16)&0x00ff0000)|((a[2]<<8)&0x0000ff00)|(a[3]&0x000000ff);
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,0);
  return value;
}

void DCFEB::wrtregvirtex6(int REGNAME,unsigned int VALUE)
{  
  char a[4];

  DEVTYPE dv = dscamDevice();
  printf(" dv %d \n",dv);

  FILE *fp=fopen("rbk.dat","w");
  // restore idle                                                                                                                    devdo(dv,-1,cmd,0,sndbuf,rcvbuf,2);
  // shift to device
  cmd[0]=(VTX6_CFG_IN&0xff);
  cmd[1]=((VTX6_CFG_IN&0x300)>>8);
  a[0]=0xAA; // sync word
  a[1]=0x99;
  a[2]=0x55;
  a[3]=0x66;
  dshuffle2(a,sndbuf,0);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,4);
  a[0]=0x30; // write reg
  a[1]=(REGNAME>>3)&0x03;
  a[2]=(((REGNAME&0x07)<<5)&0xc0);
  a[3]=0x01;
  dshuffle2(a,sndbuf,8);
  a[0]=(VALUE>>24)&0xff; // write value
  a[1]=(VALUE>>16)&0xff;
  a[2]=(VALUE>>8)&0xff;
  a[3]=VALUE&0xff;
  dshuffle2(a,sndbuf,12);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,16);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,20);
  devdo(dv,10,cmd,24*8,sndbuf,rcvbuf,2);
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,0);
  fclose(fp);
  return;
}

void DCFEB::clearcrcvirtex6()
{

  char a[4];

  DEVTYPE dv = dscamDevice();
  printf(" dv %d \n",dv);

  FILE *fp=fopen("rbk.dat","w");

  // shift to device
  cmd[0]=(VTX6_CFG_IN&0xff);
  cmd[1]=((VTX6_CFG_IN&0x300)>>8);
  a[0]=0xFF; //dummy word
  a[1]=0xFF;
  a[2]=0xFF;
  a[3]=0xFF;
  dshuffle2(a,sndbuf,0);
  a[0]=0xAA; // sync word
  a[1]=0x99;
  a[2]=0x55;
  a[3]=0x66;
  dshuffle2(a,sndbuf,4);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,8);
  a[0]=0x30; // type 1 write 1 word to cmd
  a[1]=0x00;
  a[2]=0x80;
  a[3]=0x01;
  dshuffle2(a,sndbuf,12);
  a[0]=0x00; // rcrc command
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x07;
  dshuffle2(a,sndbuf,16);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,20);
  a[0]=0x20; // type 1 NOOP word 0
  a[1]=0x00;
  a[2]=0x00;
  a[3]=0x00;
  dshuffle2(a,sndbuf,24);
  for(int kk=0;kk<28;kk++)printf(" %02x",sndbuf[27-kk]&0xff);printf("\n");
  devdo(dv,10,cmd,28*8,sndbuf,rcvbuf,2);
  fclose(fp);
  return;
}

void DCFEB::dshuffle2(char *a,char *b,int off)
{
int i,j,k;
char msk;
char tmp[4];
for(k=0;k<4;k++)tmp[k]=a[k];
for(k=0;k<4;k++)a[k]=tmp[3-k];
for(k=0;k<4;k++)b[k+off]=0x00;
for(i=0;i<4;i++){
  j=3-i;
  for(k=0;k<8;k++){
    msk=(0x01<<k);
    msk=msk&a[i];
    if(msk!=0x00)b[j+off]=b[j+off]|(0x80>>k);
  }
}
}


void DCFEB::dshuffle(char *a,char *b)
{
int i,j,k;
char msk;
for(k=0;k<4;k++)b[k]=0x00;
for(i=0;i<4;i++){
  j=3-i;
  for(k=0;k<8;k++){
    msk=(0x01<<k);
    msk=msk&a[i];
    if(msk!=0x00)b[j]=b[j]|(0x80>>k);
  }
}
}

void DCFEB::rdbkmskexp(FILE *fdesc,int cnt,char *array){
  int n=fread(array,1,cnt,fdesc);
  if(n!=cnt)printf(" n not equal cnt %d %d \n",n,cnt);
  char a[4],b[4];
  for(int i=0;i<cnt;i++){
    a[0]=array[i];
    dshuffle(a,b);
    array[i]=b[3];
  }
}

void DCFEB::rdbkmskexp_asci(FILE *fp,int cnt,char *array,int init){
  int i,k;
  int nlines;
  char line[180];
  if(cnt%4!=0&&init==0)printf(" error in rdbkmskexp_ascii - not divisible by 4 \n");
  nlines=cnt/4;
  if(init==1){
    for(i=0;i<8+65;i++)fgets(line,180,fp);
  }else if(init==2){ // msk init
    for(i=0;i<8;i++)fgets(line,180,fp);
  }else{
    for(i=0;i<nlines;i++){
      fgets(line,180,fp);
      for(k=0;k<4;k++){
        array[4*i+k]=(line[7+8*k]&0x01)*128+(line[6+8*k]&0x01)*64+(line[5+8*k]&0x01)*32+(line[4+8*k]&0x01)*16+(line[3+8*k]&0x01)*8+(line[2+8*k]&0x01)*4+(line[1+8*k]&0x01)*2+(line[0+8*k]&0x01);
      }
    }
  }
}

void DCFEB::programvirtex6(const char *mcsfile){
  // direct program of virtex6 from mcs file.eg:
  //thisODMB_->programvirtex6("/home/durkin/firmware/dcfeb/dcfeb_top_vd1.mcs");
  DEVTYPE dv = dscamDevice();
  int i,init,endr;
  int paddr,parms;
  unsigned int tt;
  unsigned int ddl,ddh;
  unsigned int ll,addr;
  unsigned int saddr;
  unsigned long int fulladdr;
  char line[128];
  char rtn_ptr[512];
  unsigned int ii;
  unsigned int val=0;
  char buf[512];
  int totbits=4096;
  FILE *dwnfp;
  dwnfp    = fopen(mcsfile,"r");
  if(dwnfp<=0){
    printf(" dwnfp %ld \n",(long)dwnfp);
    return;
  }
  printf(" dwnfp %ld \n",(long)dwnfp);
  printf(" dv %d \n",dv);
  // cmd[0]=(VTX6_SHUTDN&0xff);
  // cmd[1]=((VTX6_SHUTDN&0x300)>>8);
  cmd[0]=(VTX6_JPROG&0xff);
  cmd[1]=((VTX6_JPROG&0x300)>>8);
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
  for(int i=0;i<200;i++){
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
  usleep(25000);
  }
  cmd[0]=(VTX6_CFG_IN&0xff);
  cmd[1]=((VTX6_CFG_IN&0x300)>>8);
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
  paddr = 0;
  tt=0;
  init=0;
  endr=0;
  while(tt!=1){
    parms = fscanf(dwnfp,":%2x%4x%2x",&ll,&addr,&tt);
    if(tt==0){
      for(ii=0;ii<ll/2;ii++){
        fscanf(dwnfp,"%2x%2x",&ddl,&ddh);
        if(paddr<100000000){
          buf[val]=ddh;
          val++;
          buf[val]=ddl;
          val++;
	  // printf(" val %d \n",val);
          if(val==512){
	    // for(i=0;i<512;i++)printf("%02X",buf[i]&0xff);printf("\n");
	    val=0;
	    if(init==0){
	      scan_dmb_headtail(DATA_REG,buf,totbits,rtn_ptr,0,3,1);
	      usleep(3000);
	      init=1;
	    }else{
	      scan_dmb_headtail(DATA_REG,buf,totbits,rtn_ptr,0,0,1);
	      usleep(3000);
	    }
	}
      }
      paddr++;
      fulladdr=((saddr<<16)&0xffff0000)|(addr&0x0000ffff);
      fulladdr=(fulladdr>>1)&0x7fffffff;
      fulladdr=fulladdr+ii;
    }
  }else if(tt==4){ // newaddress
    fscanf(dwnfp,"%4x",&saddr);
    printf(":%2x%4x%2x%04x \n",ll&0x02,addr*0x04,tt&0x02,saddr&0x04);
  }else if(tt==1){
    printf(" end tt is 1 val is %d \n",val);
    if(val>0){
      scan_dmb_headtail(DATA_REG,buf,val*8,rtn_ptr,0,1,1);
      endr=1;
    }
  }
    fgets(line,1024,dwnfp);
 }
  printf(" endr is %d \n",endr);fflush(stdout);
 if(endr!=0){
   for(i=0;i<512;i++)buf[i]=0xff;
   scan_dmb_headtail(DATA_REG,buf,totbits,rtn_ptr,0,1,1);
 }
 fclose(dwnfp);  
 cmd[0]=(VTX6_JSTART&0xff);
 cmd[1]=((VTX6_JSTART&0x300)>>8);
 devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
 SendRUNTESTClks(4000);
 cmd[0]=(VTX6_BYPASS&0xff);
 cmd[1]=((VTX6_BYPASS&0x300)>>8);
 devdo(dv,10,cmd,0,sndbuf,rcvbuf,0);
 devdo(dv,-99,cmd,0,sndbuf,rcvbuf,2);
 printf(" exit program virtex6 \n");
 fflush(stdout);
}

int DCFEB::cfeb_testjtag_shift(char *out){
    int ierr = 0;
    DEVTYPE dv = dscamDevice();
    // printf(" cfeb_testjtag_shift DCFEBM dev %d \n",dv);
    cmd[0]=(VTX6_BYPASS&0xff);
    cmd[1]=((VTX6_BYPASS&0x300)>>8);
    for(int i=0;i<8;i++)rcvbuf[i]=0xff;
    sndbuf[0]=0x11;
    sndbuf[1]=0x33;
    sndbuf[2]=0x77;
    sndbuf[3]=0xFF;
    devdo(dv,10,cmd,33,sndbuf,rcvbuf,1);
    // printf(" rcvbuf %02x %02x %02x %02x \n",rcvbuf[3]&0xff,rcvbuf[2]&0xff,rcvbuf[1]&0xff,rcvbuf[0]&0xff);
    char fix[5];
    fix[0]=((rcvbuf[0]&0xfe)>>1)+((rcvbuf[1]&0x01)<<7);
    fix[1]=((rcvbuf[1]&0xfe)>>1)+((rcvbuf[2]&0x01)<<7);
    fix[2]=((rcvbuf[2]&0xfe)>>1)+((rcvbuf[3]&0x01)<<7);
    fix[3]=((rcvbuf[3]&0xfe)>>1)+((rcvbuf[4]&0x01)<<7); 
    printf(" got %02x%02x%02x%02x expect ff773311\n",fix[3]&0xff,fix[2]&0xff,fix[1]&0xff,fix[0]&0xff);
    if((fix[0]&0xFF)!=0x11)ierr=1;
    if((fix[1]&0xFF)!=0x22)ierr=1;
    if((fix[2]&0xFF)!=0x77)ierr=1;
    if((fix[3]&0x3F)!=0xff)ierr=1;
    out[0]=rcvbuf[0]; 
    out[1]=rcvbuf[1];
    out[2]=rcvbuf[2];
    out[3]=rcvbuf[3];
    out[4]=rcvbuf[4];
    return ierr;
}

void DCFEB::BuckeyeShift(int nchips,char chip_mask,char shft_bits[6][6]){
  int lay,i,j;
   DEVTYPE dv   = dscamDevice();
   int brdn = number();
   printf(" shift brdn %d \n",brdn);
      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=CHIP_MASK;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=(VTX6_USR2&0xff);
      cmd[1]=((VTX6_USR2&0x300)>>8);  
      devdo(dv,10,cmd,6,&chip_mask,rcvbuf,0);
      nchips=buckeyeChips_.size();
      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=CHIP_SHFT;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=(VTX6_USR2&0xff);
      cmd[1]=((VTX6_USR2&0x300)>>8);
      j=0;
      for(int ichip = nchips-1; ichip >= 0; --ichip) {
        int chip = buckeyeChips_[ichip].number();
        lay=layers[chip];
        for(i=0;i<6;i++){
          sndbuf[j*6+i]=shft_bits[lay][i];
        }
        j++;
      }
      devdo(dv,10,cmd,nchips*48,sndbuf,rcvbuf,0);
      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=NOOP;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=(VTX6_BYPASS&0xff);
      cmd[1]=((VTX6_BYPASS&0x300)>>8);
      devdo(dv,10,cmd,0,sndbuf,rcvbuf,0);
}

void DCFEB::BuckeyeShiftOut(int nchips,char chip_mask,char shft_bits[6][6],char *shift_out){
int i,j,lay;
int boffset;
int xtrabits = 2;
int swtchbits = -2;
      DEVTYPE dv   = dscamDevice();
      boffset=xtrabits+swtchbits;
      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=CHIP_MASK;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      // cout<<" first devdo call \n";
      cmd[0]=(VTX6_USR2&0xff);
      cmd[1]=((VTX6_USR2&0x300)>>8);
      devdo(dv,10,cmd,6,&chip_mask,rcvbuf,0);
      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=CHIP_SHFT;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=(VTX6_USR2&0xff);
      cmd[1]=((VTX6_USR2&0x300)>>8);
      j=0;
      for(int ichip = nchips-1; ichip >= 0; --ichip) {
        int chip = buckeyeChips_[ichip].number();
        lay=layers[chip];
        for(i=0;i<6;i++){
          sndbuf[j*6+i]=shft_bits[lay][i];
        }
        j++;
      }
      devdo(dv,10,cmd,nchips*48+xtrabits,sndbuf,rcvbuf,0);
      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=NOOP;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=(VTX6_BYPASS&0xff);
      cmd[1]=((VTX6_BYPASS&0x300)>>8);
      devdo(dv,10,cmd,0,sndbuf,rcvbuf,2); 
      for(int k=0;k<6*nchips;k++)shift_out[k]=0;
      int jj=0; 
      for(int k=6*(nchips);k>=0;k--){
        shift_out[jj]=shift_out[jj]|(rcvbuf[k]<<(8-boffset));
        shift_out[jj]=shift_out[jj]|((rcvbuf[k-1]>>boffset)&~(~0<<(8-boffset)));
        jj++;
      }
}

void DCFEB::buck_shift_ext_bc(int nstrip)
{
  char shft_bits[6]={0,0,0,0,0,0};

  //external cap mode: 100
  //When shift, it shift strip 1 first, and low bit first
  int i=(15-nstrip)*3+2;
  if ((nstrip >= 0) && (nstrip < 16)) shft_bits[i/8]=((1<<(i%8))&0xff);

  //enable all CFEBs, and set the CFEB into broadcast mode
  DEVTYPE dv = FADCFEBM;
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);
  sndbuf[0]=CHIP_MASK;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  // (*MyOutput_)<<" first devdo call \n";
  cmd[0]=(VTX6_USR2&0xff);
  cmd[1]=((VTX6_USR2&0x300)>>8);
  sndbuf[0]=0;    //set the chip_in_use to 0 will enable broadcast
  devdo(dv,10,cmd,6,sndbuf,rcvbuf,0);
  //shift in 48 bits for the whole DMB ( 1/30 of non-broadcast )
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);
  sndbuf[0]=CHIP_SHFT;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=(VTX6_USR2&0xff);
  cmd[1]=((VTX6_USR2&0x300)>>8);
  devdo(dv,10,cmd,48,shft_bits,rcvbuf,0);
  //set the function into NOOP, and bypass
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);
  sndbuf[0]=NOOP;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);

  ::usleep(200);
}

void DCFEB::buck_shift_comp_bc(int nstrip)
{
  char shft_bitsa[6]={0,0,0,0,0,0};
  char shft_bitsb[6]={0,0,0,0,0,0};

  //small_cap  =1
  //medium_cap =2
  //large_cap  =3
  //When shift, it shift strip 1 first, and low bit first
  int i=(15-nstrip)*3;
  long long patterna=0x99; //132 for layer 1,3,5 (channel 15 first)
  long long patternb=0x5a; //231 for layer 2,4,6 (channel 15 first)
  if ((nstrip >= 0) && (nstrip < 16)) {
    long long pattern=0;
    pattern=(patterna<<i)>>3;
    if (i==0) pattern=pattern|(patterna<<45);
    if (i==45) pattern=pattern|(patterna>>6);
    for (int j=0;j<6;j++) {
      shft_bitsa[j]=(pattern>>(j*8))&0xff;
    }
    pattern=0;
    pattern=(patternb<<i)>>3;
    if (i==0) pattern=pattern|(patternb<<45);
    if (i==45) pattern=pattern|(patternb>>6);
    for (int j=0;j<6;j++) {
      shft_bitsb[j]=(pattern>>(j*8))&0xff;
    }
  }

  //enable all CFEBs, and set the CFEB into broadcast mode
  DEVTYPE dv = FADCFEBM;
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);
  sndbuf[0]=CHIP_MASK;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  // (*MyOutput_)<<" first devdo call \n";
  cmd[0]=(VTX6_USR2&0xff);
  cmd[1]=((VTX6_USR2&0x300)>>8);
  sndbuf[0]=0x3f;    //set the chip_in_use to 111111 as the patterns are different for different buckeyes
  devdo(dv,10,cmd,6,sndbuf,rcvbuf,0);
  //shift in 48*6 bits for the whole DMB ( 1/5 of non-broadcast )
  cmd[0]=(VTX6_USR1&&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);
  sndbuf[0]=CHIP_SHFT;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=(VTX6_USR2&0xff);
  cmd[1]=((VTX6_USR2&0x300)>>8);
  for (int j=0;j<18;j++) {
    sndbuf[j]=(shft_bitsb[j%6])&0xff;
  }
  for (int j=18;j<36;j++) {
    sndbuf[j]=(shft_bitsa[j%6])&0xff;
  }
  devdo(dv,10,cmd,288,sndbuf,rcvbuf,0);
  //set the function into NOOP, and bypass
  cmd[0]=(VTX6_USR1&0xff);
  cmd[1]=((VTX6_USR1&0x300)>>8);
  sndbuf[0]=NOOP;
  devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=(VTX6_BYPASS&0xff);
  cmd[1]=((VTX6_BYPASS&0x300)>>8);
  devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);

  ::usleep(200);
}


void DCFEB::BuckeyeShiftTest(int nchips,char chip_mask,char *pat){
int i,j;
int xtrabits = 2;
int swtchbits = -2;
  int boffset=xtrabits+swtchbits;
   DEVTYPE dv   = dscamDevice();
   cmd[0]=(VTX6_USR1&0xff);
   cmd[1]=((VTX6_USR1&0x300)>>8);
   printf(" vtx6_usr1 %4x %2x %2x \n",VTX6_USR1,cmd[1]&0XFF,cmd[0]&0XFF);
   sndbuf[0]=CHIP_MASK;
   devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
   cmd[0]=(VTX6_USR2&0xff);
   cmd[1]=((VTX6_USR2&0x300)>>8);
   devdo(dv,10,cmd,6,&chip_mask,rcvbuf,0);
   printf(" nchips %d chip_mask %04x \n",nchips,chip_mask);
   for(i=0;i<6*(nchips+1);i++)pat[i]=0;
   cmd[0]=(VTX6_USR1&0xff);
   cmd[1]=((VTX6_USR1&0x300)>>8);
   sndbuf[0]=CHIP_SHFT;
   devdo(dv,10,cmd,8,sndbuf,rcvbuf,2);
   cmd[0]=(VTX6_USR2&0xff);
   cmd[1]=((VTX6_USR2&0x300)>>8);
   for(i=6;i<6*(nchips+1)+1;i++)sndbuf[i]=0x0F;   
   sndbuf[0]=0xAD;
   sndbuf[1]=0xDE;
   sndbuf[2]=0xD5;
   sndbuf[3]=0xEE;
   sndbuf[4]=0xDF;
   sndbuf[5]=0xBA;
   printf(" bits to shift %d \n",6*(nchips+1)*8+xtrabits);
   devdo(dv,10,cmd,6*(nchips+1)*8+xtrabits,sndbuf,rcvbuf,1);
   j=0;
   //      for(i=6*(nchips+1);i>=6*nchips;i--){
   for(i=6*(nchips+1);i>0;i--){
     printf("shfttst %d %02X \n",i,rcvbuf[i]&0xFF);
     pat[j]=pat[j]|(rcvbuf[i]<<(8-boffset));
     pat[j]=pat[j]|((rcvbuf[i-1]>>boffset)&~(~0<<(8-boffset)));
     j++;
   } 
   cmd[0]=(VTX6_USR1&0xff);
   cmd[1]=((VTX6_USR1&0x300)>>8);
   sndbuf[0]=NOOP;
   devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
   cmd[0]=(VTX6_BYPASS&0xff);
   cmd[1]=((VTX6_BYPASS&0x300)>>8);
   devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
} 

void DCFEB::dcfeb_raw_shift(char febbuf[7][4]){
      DEVTYPE dv   = dscamDevice();
      int brd=number();
      cmd[0]=(VTX6_USR1&0xff);
      cmd[1]=((VTX6_USR1&0x300)>>8);
      sndbuf[0]=STATUS_S;
      devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
      //      cmd[0]=VTX2_BYPASS;
      //      devdo(dv,5,cmd,0,sndbuf,rcvbuf,0);
      cmd[0]=(VTX6_USR2&0xff);
      cmd[1]=((VTX6_USR2&0x300)>>8);
      sndbuf[0]=0xd5;
      sndbuf[1]=0xee;
      sndbuf[2]=0xdf;
      sndbuf[3]=0xba;
      devdo(dv,10,cmd,32,sndbuf,rcvbuf,1);
      //
      febbuf[brd][0]=rcvbuf[0];
      febbuf[brd][1]=rcvbuf[1];
      febbuf[brd][2]=rcvbuf[2];
      febbuf[brd][3]=rcvbuf[3];
      cmd[0]=(VTX6_BYPASS&0xff);
      cmd[1]=((VTX6_BYPASS&0x300)>>8);
      devdo(dv,10,cmd,0,sndbuf,rcvbuf,0);
}

int DCFEB::dcfeb_testjtag_shift(char *out){
    int ierr = 0;
    DEVTYPE dv = dscamDevice();
    // printf(" cfeb_testjtag_shift DCFEBM dev %d \n",dv);
    cmd[0]=(VTX6_BYPASS&0xff);
    cmd[1]=((VTX6_BYPASS&0x300)>>8);
    for(int i=0;i<8;i++)rcvbuf[i]=0xff;
    sndbuf[0]=0x11;
    sndbuf[1]=0x33;
    sndbuf[2]=0x77;
    sndbuf[3]=0xFF;
    devdo(dv,10,cmd,33,sndbuf,rcvbuf,1);
    // printf(" rcvbuf %02x %02x %02x %02x \n",rcvbuf[3]&0xff,rcvbuf[2]&0xff,rcvbuf[1]&0xff,rcvbuf[0]&0xff);
    char fix[5];
    fix[0]=((rcvbuf[0]&0xfe)>>1)+((rcvbuf[1]&0x01)<<7);
    fix[1]=((rcvbuf[1]&0xfe)>>1)+((rcvbuf[2]&0x01)<<7);
    fix[2]=((rcvbuf[2]&0xfe)>>1)+((rcvbuf[3]&0x01)<<7);
    fix[3]=((rcvbuf[3]&0xfe)>>1)+((rcvbuf[4]&0x01)<<7); 
    printf(" got %02x%02x%02x%02x expect ff773311\n",fix[3]&0xff,fix[2]&0xff,fix[1]&0xff,fix[0]&0xff);
    if((fix[0]&0xFF)!=0x11)ierr=1;
    if((fix[1]&0xFF)!=0x22)ierr=1;
    if((fix[2]&0xFF)!=0x77)ierr=1;
    if((fix[3]&0x3F)!=0xff)ierr=1;
    out[0]=rcvbuf[0]; 
    out[1]=rcvbuf[1];
    out[2]=rcvbuf[2];
    out[3]=rcvbuf[3];
    out[4]=rcvbuf[4];
    return ierr;
}

void DCFEB::configure() {
  bool changed = false;
  char bytesToLoad[44], dt[2];

  dt[0] = dt[1] = 0;
  int dthresh = int (4095 * ((3.5 - comp_thresh_cfeb_[number_]) / 3.5));
  for (int i = 0; i < 8; i++)
    {
      dt[0] |= ((dthresh >> (i + 7)) & 1) << (7 - i);
      dt[1] |= ((dthresh >> i) & 1) << (6 - i);
    }
  dt[0] = ((dt[1] << 7) & 0x80) + ((dt[0] >> 1) & 0x7f);
  dt[1] = dt[1] >> 1;

  for (int i = 0; i < 44; i++)
    bytesToLoad[i] = 0;
  epromread_parameters (3, 22, (unsigned short int *) bytesToLoad);
  changed = bytesToLoad[36] != comp_mode_cfeb_[number_]
         || bytesToLoad[37] != comp_timing_cfeb_[number_]
         || bytesToLoad[38] != dt[0]
         || bytesToLoad[39] != dt[1]
         || bytesToLoad[40] != pipeline_length_[number_]
         || bytesToLoad[41] != trigger_clk_phase_[number_]
         || bytesToLoad[42] != daq_clk_phase_[number_];

  printf ("current values of parameters in parameter block 3:\n");
  printf ("  comp_mode: %d\n", bytesToLoad[36]);
  printf ("  comp_timing: %d\n", bytesToLoad[37]);
  printf ("  comp_thresh: %d\n", (bytesToLoad[38] & 0x00ff) | (bytesToLoad[39] & 0xff00));
  printf ("  pipeline_length: %d\n", bytesToLoad[40]);
  printf ("  trigger_clk_phase: %d\n", bytesToLoad[41]);
  printf ("  daq_clk_phase: %d\n", bytesToLoad[42]);

  if (changed)
    {
      bytesToLoad[36] = comp_mode_cfeb_[number_];
      bytesToLoad[37] = comp_timing_cfeb_[number_];
      bytesToLoad[38] = dt[0];
      bytesToLoad[39] = dt[1];
      bytesToLoad[40] = pipeline_length_[number_];
      bytesToLoad[41] = trigger_clk_phase_[number_];
      bytesToLoad[42] = daq_clk_phase_[number_];
      bytesToLoad[43] = 0;

      printf ("parameters from configuration file do not match currently stored values\n");
      epromload_parameters (3, 22, (unsigned short int *) bytesToLoad);

      printf ("  new values of parameters in parameter block 3:\n");
      printf ("    comp_mode: %d\n", bytesToLoad[36]);
      printf ("    comp_timing: %d\n", bytesToLoad[37]);
      printf ("    comp_thresh: %d\n", (bytesToLoad[38] & 0x00ff) | (bytesToLoad[39] & 0xff00));
      printf ("    pipeline_length: %d\n", bytesToLoad[40]);
      printf ("    trigger_clk_phase: %d\n", bytesToLoad[41]);
      printf ("    daq_clk_phase: %d\n", bytesToLoad[42]);
    }
  else
    printf ("parameters in configuration file match currently stored values\n");
}


  } // namespace emu::pc
  } // namespace emu
