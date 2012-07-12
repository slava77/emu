// Revision 0.00 2011/12/12
#include "emu/pc/EPROMBadBit_SVFFileFix.h"

namespace emu {
  namespace pc {


EPROMBadBit_SVFFileFix::EPROMBadBit_SVFFileFix(int nfix,int *fix,const char *tdownfile,const char *tfixfile)
{
  int i;
  unsigned char clr[2];
  srand(333);
  fifo(2,0,clr);
  ibad=0;
  v_instr=1; // expect 0-data 1-instr
  v_pktyp=0; // packet 1-type1  2-type2
  v_data=0; // expect data if 1
  v_ecount=0;
  v_endcrc=0;
  crc16=0x0000;
  badcrc=0;
  fdri_cnt=0;
  tfdri_cnt=0;
  downfile=tdownfile;
  fixfile=tfixfile;
  NFIXBWRD=nfix;
  for(i=0;i<nfix;i++)FIXBWRD[i]=fix[i];
  cmpdebug=1;
} 

EPROMBadBit_SVFFileFix::~EPROMBadBit_SVFFileFix()
{
}

int EPROMBadBit_SVFFileFix::outputfixfile(){
  unsigned char clr[4];
  int i;
  int numcrc = 0;
  int nfix;
  cmpdebug=1;
  secondtry=0;
 SECONDTRY:
  nfix=0;
  v_instr=1; // expect 0-data 1-instr                                         
  v_pktyp=0; // packet 1-type1  2-type2                                       
  v_data=0; // expect data if 1                                               
  v_ecount=0;
  crc16=0x0000;
  fdri_cnt=0;
  tfdri_cnt=0;
  fifo(2,0,clr);
  readsvf(0);  // first pass to get code vs data sections
  if(NBITSTOREDINSVF==2048)ncrcbits=4;
  if(NBITSTOREDINSVF==4096)ncrcbits=5;
  for(i=0;i<fdri_cnt;i++)printf(" %d %d %d \n",i,fdri_pos[i],fdri_size[i]);
  readsvf(1); // second pass to make epromfix.svf
  printf(" FIXBWRD Status \n n  pos  use \n");
  for(i=0;i<NFIXBWRD;i++){
    printf("%d %05d %d \n",i,FIXBWRD[i],FIXBWRDUSE[i]);
    if(FIXBWRDUSE[i]==1)nfix++;
  }
  printf("\n");
  notfixed=0;
  if(nfix!=NFIXBWRD){printf(" THIS PROGRAM PRODUCED A BAD FILE \n");notfixed=1;}
  crc16=0x0000;
  readsvf(2); // third pass to test epromfix.svf
  if(NBITSTOREDINSVF==2048)numcrc=2;
  if(NBITSTOREDINSVF==4096)numcrc=1;
  if(crcchk!=numcrc||dontknowchk!=0||notfixed!=0||badcrc!=0)ibad++;
  if((crcchk!=numcrc||dontknowchk!=0||notfixed!=0||badcrc!=0)&&secondtry==0){
    ibad--;
    secondtry=1;
    goto SECONDTRY;
  }
  printf(" BAD FILES %d \n",ibad);
  printf(" CRC calls should be %d and are %d \n",numcrc,crcchk);
  printf(" Don't Know calls should be 0 and are %d \n",dontknowchk);
  printf(" Not Fixed should be 0 and is %d \n",notfixed);
  printf(" Bad CRC should be 0 and is %d \n",badcrc);
  return ibad; 
}

int EPROMBadBit_SVFFileFix::test(){
  int i,j,k,l,iwrd;
  float rnd;
  unsigned char clr[4];
  cmpdebug=1;
  fifo(2,0,clr);
  int ibad=0;
  for(j=0;j<400000;j++){
    secondtry=0;
SECONDTRY:
    v_instr=1; // expect 0-data 1-instr
    v_pktyp=0; // packet 1-type1  2-type2
    v_data=0; // expect data if 1
    v_ecount=0;
    crc16=0x0000;
    fdri_cnt=0;
    tfdri_cnt=0;
    fifo(2,0,clr);
    rnd=rand();
    rnd=rnd/RAND_MAX;
    // printf(" rnd %f \n",rnd);
    NFIXBWRD=(int)(4*rnd);
    NFIXBWRD=NFIXBWRD+1;
    if(NFIXBWRD==0)NFIXBWRD=1;
    printf(" NFIXBWRD %d \n",NFIXBWRD);
    for(k=0;k<NFIXBWRD;k++){
      rnd=rand();
      rnd=rnd/RAND_MAX;
      iwrd=(int)(rnd*17530);
      // iwrd=(int)(rnd*84300);
      FIXBWRD[k]=iwrd+1;
    }
    if(NFIXBWRD>1){
      for(k=0;k<NFIXBWRD-1;k++){
        for(l=k+1;l<NFIXBWRD;l++){
          if(FIXBWRD[l]<FIXBWRD[k]){
	    // for(k=0;k<NFIXBWRD;k++)printf(" %d",FIXBWRD[k]);printf("\n");
            iwrd=FIXBWRD[k];
            FIXBWRD[k]=FIXBWRD[l];
            FIXBWRD[l]=iwrd;
	  }
        }
      }
    }   
    for(k=0;k<NFIXBWRD;k++)printf(" %d",FIXBWRD[k]);printf("\n");
    int nfix=0;
    readsvf(0);  // first pass to get code vs data sections
    if(NBITSTOREDINSVF==2048)ncrcbits=4;
    if(NBITSTOREDINSVF==4096)ncrcbits=5;
    for(i=0;i<fdri_cnt;i++)printf(" %d %d %d \n",i,fdri_pos[i],fdri_size[i]);
    readsvf(1); // second pass to make epromfix.svf
    printf(" FIXBWRD Status \n n  pos  use \n");
    for(i=0;i<NFIXBWRD;i++){
      printf("%d %05d %d \n",i,FIXBWRD[i],FIXBWRDUSE[i]);
      if(FIXBWRDUSE[i]==1)nfix++;
    }
    printf("\n");
    notfixed=0;
    if(nfix!=NFIXBWRD){printf(" THIS PROGRAM PRODUCED A BAD FILE \n");notfixed=1;}
    crc16=0x0000;
    readsvf(2); // third pass to test epromfix.svf
    if(NBITSTOREDINSVF==2048)numcrc=2;
    if(NBITSTOREDINSVF==4096)numcrc=1;
    printf(" CRC calls should be %d and are %d \n",numcrc,crcchk);
    printf(" Don't Know calls should be 0 and are %d \n",dontknowchk);
    printf(" Not Fixed should be 0 and is %d \n",notfixed);
    printf(" BADCRC should be 0 is %d \n",badcrc); 
    if(crcchk!=numcrc||dontknowchk!=0||notfixed!=0||badcrc!=0)ibad++;
    if(crcchk!=numcrc||dontknowchk!=0||notfixed!=0||badcrc!=0&&secondtry==0){
      ibad--;
      secondtry=1;
      goto SECONDTRY;
    }
    printf(" bad ****************************%d********%d \n",j,ibad);
  }
  printf(" BAD FILES %d \n",ibad); 
  return 1; 
}

void EPROMBadBit_SVFFileFix::readsvf(int ipass){  
  unsigned char snd[1024];
  unsigned char rcv2[1024];
  unsigned char sndbuf[1024];
  unsigned char rcvbuf[1024];
  FILE *vein = NULL;
  FILE *nout = NULL;
  FILE *bogodwnfp = NULL,*dwnfp = NULL;
  char buf[8192],buf2[512];
  char savbuf[8192];
  char *Word[512],*lastn;
  int Count,j,nbits,nbytes;
  int nwrds;
  int i,k,pnt;
  char bogobuf[8192];
  unsigned long int nlines=0;
  unsigned long int line=1;
  int iprint,last;
  int firstpass=0;
  int cnt=0; 
  crcchk=0;
  badcrc=0;
  dontknowchk=0;
  for(i=0;i<4;i++)FIXBWRDUSE[i]=0;
  fixbitcount=0;
  tfdri_cnt=0;
  printf(" PASS %d -----\n",ipass);
  if(ipass==0)vein=fopen("/dev/null","w");
  if(ipass==1)vein=fopen("/dev/null","w");
  if(ipass==2){
    if(cmpdebug==1){
      vein=fopen("problems","w");
    }else{
      vein=fopen("/dev/null","w");
    }
  }
  if(ipass==0){
    if(cmpdebug==1){
      cmp=fopen("epromold.cmp","w");
    }else{
      cmp=fopen("/dev/null","w");
    }
  }
  if(ipass==2){
    if(cmpdebug==1){
      cmp2=fopen("epromfix.cmp","w");
    }else{
      cmp2=fopen("/dev/null","w");
    }
  }
  if(ipass==0)nout=fopen("/dev/null","w");
  if(ipass==1)nout=fopen(fixfile,"w");
  if(ipass==2)nout=fopen("/dev/null","w");
  if(ipass==0||ipass==1){  
    dwnfp    = fopen(downfile,"r");
    bogodwnfp=fopen(downfile,"r");
  } 
  if(ipass==2){  
    dwnfp    = fopen(fixfile,"r");
    bogodwnfp=fopen(fixfile,"r");
  } 
  // printf(" everything open \n");
  fprintf(nout,"// Created from Xilinx svf file to fix bad bit at lines");
  for(i=0;i<NFIXBWRD;i++)fprintf(nout,"%d ",FIXBWRD[i]);fprintf(nout,"\n");
  while (fgets(bogobuf,256,bogodwnfp) != NULL)
    if (strrchr((char*)bogobuf,';')!=0) nlines++;
  float percent;
  while (fgets(buf,256,dwnfp) != NULL) {
    cnt++;
    //printf(" cnt %d \n",cnt);
    last=0;
    percent = (float)line/(float)nlines;
    //      if ((line%20)==0) printf("<   > Processed line %lu of %lu (%.1f%%)\n",line,nlines,percent*100.0);
    fflush(stdout);
    if((buf[0]=='/'&&buf[1]=='/')||buf[0]=='!'){
      fprintf(nout,"%s",buf);
    }else {
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
      for(k=0;k<8192;k++){savbuf[k]=buf[k];last=1;}
      iprint=0;
      Parse(buf, &Count, &(Word[0]));
      if(strcmp(Word[0],"SDR")==0){
	sscanf(Word[1],"%d",&nbits);
	nbytes=(nbits-1)/8+1;
	if(nbits==2048||nbits==4096){
          NBITSTOREDINSVF=nbits;
	  for(i=2;i<Count;i+=2){
	    if(strcmp(Word[i],"TDI")==0){
	      for(j=0;j<nbytes;j++){
		sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&snd[j]);
	      }
	    }
	  }
	  nwrds=nbytes/4;
	  for(i=0;i<nwrds;i++){
	    //sndbuf[i]=snd[i]&smask[i];
	    for(k=0;k<4;k++)sndbuf[k]=snd[4*i+k];
	    // for(k=0;k<4;k++)printf("%02x",snd[i]&0xff);printf("\n");
	    if(nbits==2048)virtexengine(ipass,sndbuf,rcvbuf,vein);
            if(nbits==4096&&firstpass==0){firstpass=1;setflags();}
	    if(nbits==4096)spartanengine(ipass,sndbuf,rcvbuf,vein);
	    fixbitcount++;
	  }
	  if(ipass==1){
	    pnt=0;
	    for(i=0;i<nwrds;i++){
	      iprint=1;
	      fifo(1,nwrds*4,rcv2);
	      fprintf(nout,"SDR %04d TDI (",NBITSTOREDINSVF);
	      for(i=0;i<NBITSTOREDINSVF/8;i++)fprintf(nout,"%02x",rcv2[NBITSTOREDINSVF/8-1-i]&0xff);
	      fprintf(nout,") SMASK (");
	      for(i=0;i<NBITSTOREDINSVF/8;i++)fprintf(nout,"ff");
	      fprintf(nout,") ;\n");
	    } // for i
	  } //pass
	} // if nbits
      } // if sdr
      if(iprint==0&&last==1){fprintf(nout,"%s",savbuf);last=0;}
    } // if(buf
  } // fgets
  fclose(bogodwnfp);
  fclose(dwnfp);
  fclose(nout);
  fflush(vein);
  fclose(vein);
  if(ipass==0)fclose(cmp);
  if(ipass==2)fclose(cmp2);
}

void EPROMBadBit_SVFFileFix::Parse(char *buf,int *Count,char **Word){
  *Word = buf;
  *Count = 0;
  while(*buf != '\0') {
    while ((*buf==' ') || (*buf=='\t') || (*buf=='\n') || (*buf=='"')) *(buf++)='\0';
    if ((*buf != '\n') && (*buf != '\0')) {
      Word[(*Count)++] = buf;
    }
    while ((*buf!=' ')&&(*buf!='\0')&&(*buf!='\n')&&(*buf!='\t')&&(*buf!='"')) {
      buf++;
    }
  }
  *buf = '\0';
}

void EPROMBadBit_SVFFileFix::virtexengine(int ipass,unsigned char *snd,unsigned char *rcv,FILE *vein){
  /* optimized for XILINX Virtex-I */
  int i,j,k,l;
  int itest;
  unsigned long int size;
  unsigned char b[4];
  unsigned char c[6];
  unsigned char crc_c[4];
  int fxbit,diff;
  int ncrcregs=9; // spartan-ii manual XAPP138 ignore all but first 8
  int crcregs[9]={4,11,9,6,5,1,2,0,14}; // only 8 registers contribute to crc
  int ncmds=8;
  int cmds[8]={1,3,4,5,7,9,10,0xd};
  int corr[3]={4,3,3};
  dshuffle2(snd,b,0);
  for(i=0;i<4;i++)c[i]=b[3-i];
  c[4]=v_register;
  //for(i=0;i<4;i++)printf("%02x",c[i]&0xff);printf(" pass %d\n",ipass);
  fprintf(vein," %06d ",fixbitcount);
  for(i=0;i<4;i++)fprintf(vein,"%02x",b[i]&0xff); 
  if(v_instr==0){
    if(v_data==0)fprintf(vein," PROBLEM HAVE NO DATA \n");
    fprintf(vein," data %d(%04x)",v_count,v_count);
    v_count++;
    if(ipass==1){
      fdri_ifirst=-1;
      for(j=0;j<NFIXBWRD;j++){
	if(FIXBWRD[j]==fixbitcount){ 
          fxbit=j;
          for(k=j+1;k<NFIXBWRD;k++){
            diff=FIXBWRD[k]-FIXBWRD[k-1];
            if(diff>3)goto enddif;
            fxbit++;
            for(l=0;l<diff;l++)insertword(svf_NOOP,0);
	  }
	enddif:
	  itest=-1; // test if it is the last in this write
          for(k=0;k<NFIXBWRD;k++){
            //printf(" k %d %d %d %d %d \n",k,tfdri_cnt-1,FIXBWRD[k],fdri_pos[tfdri_cnt-1],fdri_pos[tfdri_cnt-1]+fdri_size[tfdri_cnt-1]+1);
	    if(FIXBWRD[k]>fdri_pos[tfdri_cnt-1]&&FIXBWRD[k]<=fdri_pos[tfdri_cnt-1]+fdri_size[tfdri_cnt-1]+3&&fdri_ifirst==-1&&FIXBWRDUSE[k]==0){itest=k;;}
          }
	  //printf(" %d itest %d fxbit %d \n",tfdri_cnt-1,itest,fxbit);
          if(fxbit==itest){ // it is the last
            size=fdri_size[tfdri_cnt-1]-fdri_ilast;
	    //printf(" %d it is the last size %d %d %d \n",tfdri_cnt-1,size,fdri_size[tfdri_cnt-1],fdri_ilast);
          }else{
            size=FIXBWRD[fxbit+1]-fdri_pos[tfdri_cnt-1]-fdri_ilast-corr[tfdri_cnt-1];
	    //printf(" %d not the last size %d %d %d %d\n",tfdri_cnt-1,size,FIXBWRD[j],fdri_pos[tfdri_cnt-1],fdri_ilast);
            fdri_ilast=size+fdri_ilast;
	  }
          if(tfdri_cnt-1==0)insertword(svf_LONGWRT,size);
          if(tfdri_cnt-1>0&&tfdri_cnt-1<fdri_cnt)insertword(svf_SHORTWRT,size);
          nfixoff=0;  
        }
      }
    }     
    //fprintf(vein," v_count v_ecount %d %d \n",v_count,v_ecount);
    if(v_count==v_ecount){
        v_instr=1;
        v_pktyp=0;
        v_data=0;
        v_ecount=0;
        v_endcrc=1;
    }
    
    for(l=0;l<ncrcregs;l++)if(c[4]==crcregs[l])crcupdate(c);
    for(l=0;l<ncrcregs;l++)if(c[4]==crcregs[l])fprintf(vein," %xcrc%04x ",c[4]&0x0f,crc16&0xffff);
    fprintf(vein,"\n");
  }else if(v_instr==1){
    if(b[0]==0xaa&&b[1]==0x99&&b[2]==0x55&&b[3]==0x66){
      fprintf(vein," SYNC Wrd");
      if(ipass==1&&FIXBWRD[0]==fixbitcount){insertword(svf_NOOP,0);fxbit=0;}
    }else if(b[0]==0xff&&b[1]==0xff&&b[2]==0xff&&b[3]==0xff){
      fprintf(vein," NOOP");
      if(secondtry==1&&fixbitcount==0&&ipass==1)insertword(svf_NOOP,0);
      for(i=0;i<NFIXBWRD;i++)if(FIXBWRD[i]==fixbitcount)FIXBWRDUSE[i]=1;
    }else if(b[0]==0x20&&b[1]==0x00&&b[2]==0x00&&b[3]==0x00){
      fprintf(vein," NOOP");
      for(i=0;i<NFIXBWRD;i++)if(FIXBWRD[i]==fixbitcount){
        FIXBWRDUSE[i]=1;
	insertword(svf_NOOP,0);
      }
    }else if((b[0]==0x00&&b[1]==0x00&&b[2]==0x00&&b[3]==0x00)&&v_data==0){
      fprintf(vein," NOOP");
      for(i=0;i<NFIXBWRD;i++)if(ipass==1&&fixbitcount>fdri_pos[fdri_cnt-1]+fdri_size[fdri_cnt-1]+1){FIXBWRDUSE[i]=1;insertword(svf_NOOP,0);}
    }else if(b[0]==0x30){
      v_pktyp=1;
      v_data=1;
      v_count=0;
      v_register=((0x03&b[1])<<3)|((0xe0&b[2])>>5);
      c[4]=v_register;
      size=256*(b[2]&0x03)+b[3]; 
      if(v_register==2&&size==0)v_data=0;
      if(size>0&&v_register==2){
         v_pktyp=0;
         v_data=1;
         v_instr=0;
         fprintf(vein," write %ld(%04lx) 32 bit words ",size,size);
         v_count=0;
         v_ecount=size;
      }
      if((fdri_cnt>=0&&fdri_cnt<10)&&size>0&&v_register==2&&ipass==0){
        fdri_pos[fdri_cnt]=fixbitcount;
        fdri_size[fdri_cnt]=size;
        fdri_cnt++;
        fprintf(vein,"*");
      }
      if(ipass==1){
        nfixoff=0;
        for(j=0;j<NFIXBWRD;j++){
          if(fixbitcount==FIXBWRD[j]){insertword(svf_NOOP,0);nfixoff=1;;}
          if(fixbitcount==FIXBWRD[j]-1){insertword(svf_NOOP,0);insertword(svf_NOOP,0);nfixoff=2;}
          if((fixbitcount==FIXBWRD[j]-2)&&v_register==2){insertword(svf_NOOP,0);insertword(svf_NOOP,0);insertword(svf_NOOP,0);nfixoff=3;}
        }
      }
      if(v_register==2&&size>0&&ipass==1&&tfdri_cnt<fdri_cnt){
        fdri_ifirst=-1;
        for(j=0;j<NFIXBWRD;j++){
	  //printf(" ***j %d %d %d %d %d \n",j,tfdri_cnt,FIXBWRD[j],fdri_pos[tfdri_cnt]+nfixoff,fdri_pos[tfdri_cnt]+fdri_size[tfdri_cnt]+1);
	  if(FIXBWRD[j]>fdri_pos[tfdri_cnt]+nfixoff&&FIXBWRD[j]<fdri_pos[tfdri_cnt]+fdri_size[tfdri_cnt]+3&&fdri_ifirst==-1)fdri_ifirst=FIXBWRD[j]-fdri_pos[tfdri_cnt]-1;
        }
	//printf(" ****ifirst %d \n",fdri_ifirst);
        if(fdri_ifirst!=-1){
          b[3]=fdri_ifirst&0x00ff;
          b[2]=(b[2]&0xe0)|((fdri_ifirst>>8)&0x0003);
          fdri_ilast=fdri_ifirst;
        }
        tfdri_cnt++;
      }      
      if(v_register==4)fprintf(vein," wrt cmd %ld ",size);
      if(v_register==11)fprintf(vein," wrt flr %ld ",size); 
      if(v_register==9)fprintf(vein," wrt cor %ld ",size);
      if(v_register==6)fprintf(vein," wrt msk %ld ",size);
      if(v_register==5)fprintf(vein," wrt ctl %ld ",size); 
      if(v_register==1)fprintf(vein," wrt far %ld ",size);
      if(v_register==2)fprintf(vein," wrt fdri %ld ",size);
      if(v_register==14)fprintf(vein," wrt idcode %ld",size);
      if(v_register==0){
         fprintf(vein," wrt crc %ld ",size);
         if(ipass==2)crcchk++;
      }
      if(v_register==3)fprintf(vein," wrt fdr0 %ld ",size); 
      if(v_register==8)fprintf(vein," wrt lout %ld ",size);
    }else if(v_data==1){
      v_count++;
      if(v_pktyp==1&&v_register!=4){
        if(v_count<2){
          fprintf(vein," value %d ",v_count);
        }else{
          if(b[0]==0x00&&b[1]==0x00&&b[2]==0x00&&b[3]==0x00){
            fprintf(vein," NOOP ");
            if(ipass==1&&fixbitcount==FIXBWRD[NFIXBWRD-1])if(ipass==1)insertword(svf_NOOP,0);
          }
        }
      }
      if(v_pktyp==1&&v_register==4){
        if(b[3]==1)fprintf(vein," WCFG");
        if(b[3]==3)fprintf(vein," LFRM");
        if(b[3]==4)fprintf(vein," RCFG");
        if(b[3]==5)fprintf(vein," START");
        if(b[3]==7){fprintf(vein," RCRC");crc16=0x0000;}
	if(b[3]==9)fprintf(vein," SWITCH");
      }
      if(v_count==v_ecount){
        v_pktyp=0;
        v_data=0;
        v_ecount=0;
      }
      for(l=0;l<ncrcregs;l++)if(c[4]==crcregs[l]&&c[4]!=4&&b[3]!=7)crcupdate(c);  
      for(l=0;l<ncrcregs;l++)if(c[4]==crcregs[l]&&c[4]!=4&&b[3]!=7)fprintf(vein," %xcrc%04x ",c[4]&0x0f,crc16&0xffff);
      for(l=0;l<ncmds;l++)if(b[3]==cmds[l]&&b[3]!=7&&c[4]==4)crcupdate(c);
      for(l=0;l<ncmds;l++)if(b[3]==cmds[l]&&b[3]!=7&&c[4]==4)fprintf(vein," %xcrc%04x ",c[4]&0x0f,crc16&0xffff);
      if(b[3]==7)fprintf(vein," CRC is set to 0x0000");
      if(v_register==0){
        if(crc16!=0x00000000&ipass==2){
	  fprintf(vein,"    BADCRC IS FOUND ");
          badcrc=1;
        }
      }
    }else if(b[0]==0x50){
      v_pktyp=0;
      v_data=1;
      v_instr=0;
      size=b[1]*256*256+b[2]*256+b[3];
      if(fdri_cnt==0&&ipass==0){
        fdri_pos[fdri_cnt]=fixbitcount;
        fdri_size[fdri_cnt]=size;
        fdri_cnt++;
        fprintf(vein,"*");
      }
      TOTWRDS=size;
      //   size=FIXBWRD[0]-18;
      // b[3]=size&0xff;
      // b[2]=(size>>8)&0xff;
      // b[1]=(size>>16)&0xff;      
      if(ipass==1){
        fdri_ifirst=-1;
        for(j=0;j<NFIXBWRD;j++){
	  if(FIXBWRD[j]>fdri_pos[tfdri_cnt]&&FIXBWRD[j]<fdri_pos[tfdri_cnt]+fdri_size[tfdri_cnt]+1&&fdri_ifirst==-1)fdri_ifirst=FIXBWRD[j]-fdri_pos[tfdri_cnt]-1;
        }
        if(fdri_ifirst!=-1){
          b[3]=fdri_ifirst&0xff;
          b[2]=(fdri_ifirst>>8)&0xff;
          b[1]=(fdri_ifirst>>16)&0xff;
          fdri_ilast=fdri_ifirst;
        }
        tfdri_cnt++;
      }      
      size=TOTWRDS;
      fprintf(vein," write %ld(%04lx) 32 bit words ",size,size);
      v_count=0;
      v_ecount=size;
    }else if(v_endcrc==1&&NBITSTOREDINSVF==4096){
      fprintf(vein," autoCRC ");
      c[4]=0; // the crc is loaded with a crc embedded in the bitstream
      getinversecrc(crc_c);
      fprintf(vein," b%xcrc%02x%02x",c[4]&0x0f,crc_c[0]&0xff,crc_c[1]&0xff);
      crcupdate(c);
      fprintf(vein," a%xcrc%04x ",c[4]&0x0f,crc16&0xffff);
      if(crc16!=0x00000000&ipass==2){
	fprintf(vein,"    BADCRC IS FOUND ");
        badcrc=1;
      }
      v_endcrc=0;
      crc16=0x00000000;
    }else{
      fprintf(vein," don't know");
      if(ipass==2)dontknowchk++;  
    }
    fprintf(vein,"\n");
  }
  dshuffle2(b,rcv,0);
  if(ipass==1)fifo(0,4,rcv);
  if(ipass==0){
     for(i=0;i<4;i++)fprintf(cmp," %02X",rcv[i]&0xff);fprintf(cmp,"\n");
  }
  if(ipass==2){
     for(i=0;i<4;i++)fprintf(cmp2," %02X",rcv[i]&0xff);fprintf(cmp2,"\n");
  }
  // if(ipass==2){
  //   for(i=0;i<4;i++)fprintf(cmp2," %02X",rcv[i]&0xff);fprintf(cmp,"\n");
  // }
  //  for(i=0;i<4;i++)fprintf(vein," b %02x rcv %02x ",b[i]&0xff,rcv[i]&0xff);printf("\n");
  return; 
}



void EPROMBadBit_SVFFileFix::spartanengine(int ipass,unsigned char *snd,unsigned char *rcv,FILE *vein){

/* OPTIMIZED FOR SPARTAN-II */
/* xilinx documentation is bad. The virtex bits into the crc are
 4 address bits + 32 data bits. For the spartan-II it is 
 5 address bits + 32 data bits which contradicts their
 documentation.*/
  int i,j,k,l;
  int itest;
  unsigned long int size;
  unsigned char b[4];
  unsigned char c[6];
  unsigned char crc_c[4];
  int fxbit,diff;
  int ncrcregs=9; // spartan-ii manual XAPP138 ignore all but first 8
  int crcregs[9]={4,11,9,6,5,1,2,0,14}; // only 8 registers contribute to crc
  int ncmds=8;
  int cmds[8]={1,3,4,5,7,9,10,0xd};
  int corr[3]={4,3,3};
  int autocrcflg=0;
  dshuffle2(snd,b,0);
  for(i=0;i<4;i++)c[i]=b[3-i];
  c[4]=v_register;
  //for(i=0;i<4;i++)printf("%02x",c[i]&0xff);printf(" pass %d\n",ipass);
  fprintf(vein," %06d ",fixbitcount);
  for(i=0;i<4;i++)fprintf(vein,"%02x",b[i]&0xff); 
  if(v_instr==0){
    if(v_data==0)fprintf(vein," PROBLEM HAVE NO DATA \n");
    fprintf(vein," data %d(%04x)",v_count,v_count);
    v_count++;
    if(ipass==1){
      fdri_ifirst=-1;
      for(j=0;j<NFIXBWRD;j++){
	if(FIXBWRD[j]==fixbitcount+1){ 
          fxbit=j;
          for(k=j+1;k<NFIXBWRD;k++){
            diff=FIXBWRD[k]-FIXBWRD[k-1];
            if(diff>3)goto enddif;
            fxbit++;
            for(l=0;l<diff;l++)if(ipass==2)insertword(svf_NOOP,0);
	  }
	enddif:
	  itest=-1; // test if it is the last in this write
          for(k=0;k<NFIXBWRD;k++){
            //printf(" k %d %d %d %d %d \n",k,tfdri_cnt-1,FIXBWRD[k],fdri_pos[tfdri_cnt-1],fdri_pos[tfdri_cnt-1]+fdri_size[tfdri_cnt-1]+1);
	    if(FIXBWRD[k]>fdri_pos[tfdri_cnt-1]&&FIXBWRD[k]<=fdri_pos[tfdri_cnt-1]+fdri_size[tfdri_cnt-1]+3&&fdri_ifirst==-1&&FIXBWRDUSE[k]==0){itest=k;;}
          }
	  //printf(" %d itest %d fxbit %d \n",tfdri_cnt-1,itest,fxbit);
          if(fxbit==itest){ // it is the last
            size=fdri_size[tfdri_cnt-1]-fdri_ilast;
	    //printf(" %d it is the last size %d %d %d \n",tfdri_cnt-1,size,fdri_size[tfdri_cnt-1],fdri_ilast);
          }else{
            size=FIXBWRD[fxbit+1]-fdri_pos[tfdri_cnt-1]-fdri_ilast-corr[tfdri_cnt-1];
	    // size=size+1;
	    //printf(" %d not the last size %d %d %d %d\n",tfdri_cnt-1,size,FIXBWRD[j],fdri_pos[tfdri_cnt-1],fdri_ilast);
            fdri_ilast=size+fdri_ilast;
	  }
          c[4]=0; // the crc is loaded with a crc embedded in the bitstream
          getinversecrc(crc_c);
          fprintf(vein," b%xcrc%02x%02x",c[4]&0x0f,crc_c[0]&0xff,crc_c[1]&0xff);
          c[0]=crc_c[1];
          c[1]=crc_c[0];
          if(ipass==1)insertword(svf_AUTOCRC,0);
          crcupdate(c);
          fprintf(vein," a%xcrc%04x ",c[4]&0x0f,crc16&0xffff);
          if(crc16!=0x00000000&ipass==2){
	    fprintf(vein,"    BADCRC IS FOUND ");
            badcrc++;
          }
          crc16=0x00000000;
          if(tfdri_cnt-1==0)if(ipass==1)insertword(svf_LONGWRT,size);
          if(tfdri_cnt-1>0&&tfdri_cnt-1<fdri_cnt)if(ipass==1)insertword(svf_SHORTWRT,size);
          dshuffle2(b,rcv,0);
          fifo(0,4,rcv);
          autocrcflg=1; 
          nfixoff=0;    
        }
      }
    }     
    if(v_count==v_ecount){
        v_instr=1;
        v_pktyp=0;
        v_data=0;
        v_ecount=0;
        v_endcrc=1;
    }
    for(l=0;l<ncrcregs;l++)if(c[4]==crcregs[l])crcupdate(c);
    for(l=0;l<ncrcregs;l++)if(c[4]==crcregs[l])fprintf(vein," %xcrc%04x ",c[4]&0x0f,crc16&0xffff);
    fprintf(vein,"\n");
  }else if(v_instr==1){
    if(b[0]==0xaa&&b[1]==0x99&&b[2]==0x55&&b[3]==0x66){
      fprintf(vein," SYNC Wrd");
      if(ipass==1&&FIXBWRD[0]==fixbitcount){if(ipass==1)insertword(svf_NOOP,0);fxbit=0;}
    }else if(b[0]==0xff&&b[1]==0xff&&b[2]==0xff&&b[3]==0xff){
      fprintf(vein," NOOP");
      if(secondtry==1&&fixbitcount==0&&ipass==1)insertword(svf_NOOP,0);
      for(i=0;i<NFIXBWRD;i++)if(FIXBWRD[i]==fixbitcount)FIXBWRDUSE[i]=1;
    }else if(b[0]==0x20&&b[1]==0x00&&b[2]==0x00&&b[3]==0x00){
      fprintf(vein," NOOP");
      for(i=0;i<NFIXBWRD;i++)if(FIXBWRD[i]==fixbitcount){
        FIXBWRDUSE[i]=1;
	if(ipass==1)insertword(svf_NOOP,0);
      }
    }else if((b[0]==0x00&&b[1]==0x00&&b[2]==0x00&&b[3]==0x00)&&v_data==0&&autocrcflg!=1){
      fprintf(vein," NOOP");
      for(i=0;i<NFIXBWRD;i++)if(ipass==1&&fixbitcount>fdri_pos[fdri_cnt-1]+fdri_size[fdri_cnt-1]+1){FIXBWRDUSE[i]=1;if(ipass==1)insertword(svf_NOOP,0);}
    }else if(b[0]==0x30){
      v_pktyp=1;
      v_data=1;
      v_count=0;
      v_register=((0x03&b[1])<<3)|((0xe0&b[2])>>5);
      c[4]=v_register;
      size=256*(b[2]&0x03)+b[3]; 
      if(v_register==2&&size==0)v_data=0;
      if(size>0&&v_register==2){
         v_pktyp=0;
         v_data=1;
         v_instr=0;
         fprintf(vein," write %ld(%04lx) 32 bit words ",size,size);
         v_count=0;
         v_ecount=size;
      }
      if((fdri_cnt>=0&&fdri_cnt<10)&&size>0&&v_register==2&&ipass==0){
        fdri_pos[fdri_cnt]=fixbitcount;
        fdri_size[fdri_cnt]=size;
        fdri_cnt++;
        fprintf(vein,"*");
      }
      if(ipass==1){
        nfixoff=0;
        for(j=0;j<NFIXBWRD;j++){
          if(fixbitcount==FIXBWRD[j]){if(ipass==1)insertword(svf_NOOP,0);nfixoff=1;;}
	  if(fixbitcount==FIXBWRD[j]-1){if(ipass==1)insertword(svf_NOOP,0);if(ipass==1)insertword(svf_NOOP,0);nfixoff=2;}
          if((fixbitcount==FIXBWRD[j]-2)&&v_register==2){if(ipass==1)insertword(svf_NOOP,0);if(ipass==1)insertword(svf_NOOP,0);if(ipass==1)insertword(svf_NOOP,0);nfixoff=3;}
        }
      }
      if(v_register==2&&size>0&&ipass==1&&tfdri_cnt<fdri_cnt){
        fdri_ifirst=-1;
        for(j=0;j<NFIXBWRD;j++){
	  //printf(" ***j %d %d %d %d %d \n",j,tfdri_cnt,FIXBWRD[j],fdri_pos[tfdri_cnt]+nfixoff,fdri_pos[tfdri_cnt]+fdri_size[tfdri_cnt]+1);
	  if(FIXBWRD[j]>fdri_pos[tfdri_cnt]+nfixoff&&FIXBWRD[j]<fdri_pos[tfdri_cnt]+fdri_size[tfdri_cnt]+3&&fdri_ifirst==-1)fdri_ifirst=FIXBWRD[j]-fdri_pos[tfdri_cnt]+1;
        }
	// printf(" ****ifirst %d \n",fdri_ifirst);
        if(fdri_ifirst!=-1){
          b[3]=fdri_ifirst&0x00ff;
          b[2]=(b[2]&0xe0)|((fdri_ifirst>>8)&0x0003);
          fdri_ilast=fdri_ifirst;
        }
        tfdri_cnt++;
      }      
      if(v_register==4)fprintf(vein," wrt cmd %ld ",size);
      if(v_register==11)fprintf(vein," wrt flr %ld ",size); 
      if(v_register==9)fprintf(vein," wrt cor %ld ",size);
      if(v_register==6)fprintf(vein," wrt msk %ld ",size);
      if(v_register==5)fprintf(vein," wrt ctl %ld ",size); 
      if(v_register==1)fprintf(vein," wrt far %ld ",size);
      if(v_register==2)fprintf(vein," wrt fdri %ld ",size);
      if(v_register==14)fprintf(vein," wrt idcode %ld",size);
      if(v_register==0){
         fprintf(vein," wrt crc %ld ",size);
         if(ipass==2)crcchk++;
      }
      if(v_register==3)fprintf(vein," wrt fdr0 %ld ",size); 
      if(v_register==8)fprintf(vein," wrt lout %ld ",size);
    }else if(v_data==1){
      v_count++;
      if(v_pktyp==1&&v_register!=4){
        if(v_count<2){
          fprintf(vein," value %d ",v_count);
        }else{
          if(b[0]==0x00&&b[1]==0x00&&b[2]==0x00&&b[3]==0x00){
            fprintf(vein," NOOP ");
            if(ipass==1&&fixbitcount==FIXBWRD[NFIXBWRD-1])if(ipass==1)insertword(svf_NOOP,0);
          }
        }
      }
      if(v_pktyp==1&&v_register==4){
        if(b[3]==1)fprintf(vein," WCFG");
        if(b[3]==3)fprintf(vein," LFRM");
        if(b[3]==4)fprintf(vein," RCFG");
        if(b[3]==5)fprintf(vein," START");
        if(b[3]==7){fprintf(vein," RCRC");crc16=0x0000;}
	if(b[3]==9)fprintf(vein," SWITCH");
      }
      if(v_count==v_ecount){
        v_pktyp=0;
        v_data=0;
        v_ecount=0;
      }
      for(l=0;l<ncrcregs;l++)if(c[4]==crcregs[l]&&c[4]!=4&&b[3]!=7)crcupdate(c);  
      for(l=0;l<ncrcregs;l++)if(c[4]==crcregs[l]&&c[4]!=4&&b[3]!=7)fprintf(vein," %xcrc%04x ",c[4]&0x0f,crc16&0xffff);
      for(l=0;l<ncmds;l++)if(b[3]==cmds[l]&&b[3]!=7&&c[4]==4)crcupdate(c);
      for(l=0;l<ncmds;l++)if(b[3]==cmds[l]&&b[3]!=7&&c[4]==4)fprintf(vein," %xcrc%04x ",c[4]&0x0f,crc16&0xffff);
      if(b[3]==7)fprintf(vein," CRC is set to 0x0000");
      if(v_register==0){
        if(crc16!=0x00000000&ipass==2){
	  fprintf(vein,"    BADCRC IS FOUND ");
          badcrc=1;
        }
      }
    }else if(b[0]==0x50){
      v_pktyp=0;
      v_data=1;
      v_instr=0;
      size=b[1]*256*256+b[2]*256+b[3];
      if(fdri_cnt==0&&ipass==0){
        fdri_pos[fdri_cnt]=fixbitcount;
        fdri_size[fdri_cnt]=size;
        fdri_cnt++;
        fprintf(vein,"*");
      }
      TOTWRDS=size;
      //   size=FIXBWRD[0]-18;
      // b[3]=size&0xff;
      // b[2]=(size>>8)&0xff;
      // b[1]=(size>>16)&0xff;      
      if(ipass==1){
        fdri_ifirst=-1;
        for(j=0;j<NFIXBWRD;j++){
	  if(FIXBWRD[j]>fdri_pos[tfdri_cnt]&&FIXBWRD[j]<fdri_pos[tfdri_cnt]+fdri_size[tfdri_cnt]+1&&fdri_ifirst==-1)fdri_ifirst=FIXBWRD[j]-fdri_pos[tfdri_cnt]-1-1; // fix +1
        }
        if(fdri_ifirst!=-1){
          b[3]=fdri_ifirst&0xff;
          b[2]=(fdri_ifirst>>8)&0xff;
          b[1]=(fdri_ifirst>>16)&0xff;
          fdri_ilast=fdri_ifirst;
        }
        tfdri_cnt++;
      }      
      size=TOTWRDS;
      fprintf(vein," write %ld(%04lx) 32 bit words ",size,size);
      v_count=0;
      v_ecount=size;
    }else if(v_endcrc==1&&NBITSTOREDINSVF==4096){
      c[4]=0; // the crc is loaded with a crc embedded in the bitstream
      getinversecrc(crc_c);
      fprintf(vein," b%xcrc%02x%02x",c[4]&0x0f,crc_c[0]&0xff,crc_c[1]&0xff);
      autocrcflg=1;
      c[0]=crc_c[1];
      c[1]=crc_c[0];
      if(ipass==1)insertword(svf_AUTOCRC,0);
      crcupdate(c);
      fprintf(vein," a%xcrc%04x ",c[4]&0x0f,crc16&0xffff);
      if(crc16!=0x00000000&ipass==2){
	fprintf(vein,"    BADCRC IS FOUND ");
        badcrc++;
      }
      v_endcrc=0;
      crc16=0x00000000;
    }else{
      fprintf(vein," don't know");
      if(ipass==2)dontknowchk++;  
    }
    fprintf(vein,"\n");
  }
  dshuffle2(b,rcv,0);
  if(ipass==1&&autocrcflg!=1){
     fifo(0,4,rcv);
  }else if(autocrcflg==1){
    fprintf(vein," SKIPPED AN EVENT \n");
     fixbitcount--;
  }
  if(ipass==0){
     for(i=0;i<4;i++)fprintf(cmp," %02X",rcv[i]&0xff);fprintf(cmp,"\n");
  }
  if(ipass==2){
     for(i=0;i<4;i++)fprintf(cmp2," %02X",rcv[i]&0xff);fprintf(cmp2,"\n");
  }
  // if(ipass==2){
  //   for(i=0;i<4;i++)fprintf(cmp2," %02X",rcv[i]&0xff);fprintf(cmp,"\n");
  // }
  //  for(i=0;i<4;i++)fprintf(vein," b %02x rcv %02x ",b[i]&0xff,rcv[i]&0xff);printf("\n");
  return; 
}





void EPROMBadBit_SVFFileFix::dshuffle2(unsigned char *a,unsigned char *b,int off){
  int i,j,k;
  char msk;
  char tmp[8];
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

void EPROMBadBit_SVFFileFix::crcupdate(unsigned char *in){
  int j,k,bit;
  int nbits;
  char c;
  for(k=0;k<5;k++){
    c=in[k];
    nbits=8;
    if(k==4)nbits=ncrcbits;
    for(j=0;j<nbits;j++){
      bit=(c>>j)&0x01;
      // printf(" %d bit %d \n",j,bit);  
      if(((crc16&0x8000) ? 1 : 0) != bit){
	crc16=(crc16<<1)^CRC16POLY;
      }else{
        crc16 <<=1;
      }
    }
  } 
}

void EPROMBadBit_SVFFileFix::fifo(int irw,int n,unsigned char *data){
  int i;
  // irw 0-write 1-read 2-clear
  if(irw==0){
    for(i=0;i<n;i++)FIFO[i+FIFOpnt]=data[i];
    FIFOpnt=FIFOpnt+n;
  }
  if(irw==1){
    // printf(" fifo read %d %d\n",FIFOpnt,n);
    for(i=0;i<n;i++)data[i]=FIFO[i];
    for(i=n;i<FIFOpnt;i++)FIFO[i-n]=FIFO[i];
    FIFOpnt=FIFOpnt-n;
  }
  if(irw==2){FIFOpnt=0;printf(" clear fifo \n");}
  return;
}

void EPROMBadBit_SVFFileFix::insertword(enum ADDType flag,int size){
  unsigned char noop[4]={0xff,0xff,0xff,0xff};
  unsigned char fdri[4]={0x30,0x00,0x40,0x00};
  unsigned char type2[4]={0x50,0x00,0x00,0x00};
  unsigned char autocrc[4]={0x00,0x00,0x00,0x00};
  int i;
  int shift;
  unsigned char a[4];
  shift=0;
  for(i=0;i<NFIXBWRD;i++)if(FIXBWRD[i]==fixbitcount)FIXBWRDUSE[i]=1;
  if(flag==svf_NOOP){
   dshuffle2(noop,a,0);       
   fifo(0,4,a);
   shift=1;     
  }
  if(flag==svf_LONGWRT){
   dshuffle2(noop,a,0);       
   fifo(0,4,a);     
   dshuffle2(fdri,a,0);
   fifo(0,4,a);
   type2[3]=size&0xff;
   type2[2]=(size>>8)&0xff;
   type2[1]=(size>>16)&0xff;
   dshuffle2(type2,a,0);
   fifo(0,4,a);
   shift=3;
  }
  if(flag==svf_SHORTWRT){
   dshuffle2(noop,a,0);       
   fifo(0,4,a);    
   fdri[3]=size&0xff;
   fdri[2]=(fdri[2]&0x40)|((size>>8)&0x03);    
   dshuffle2(fdri,a,0);
   fifo(0,4,a);
   shift=2;
  }
  if(flag==svf_AUTOCRC){
   autocrc[3]=(crc16&0xff);
   autocrc[2]=((crc16>>8)&0xff);
   autocrc[1]=0x00;
   autocrc[0]=0x00;
   dshuffle2(autocrc,a,0);
   fifo(0,4,a);
   shift=1;
  }
  for(i=0;i<fdri_cnt;i++)fdri_pos[i]=fdri_pos[i]+shift;
  fixbitcount=fixbitcount+shift;
}


void EPROMBadBit_SVFFileFix::getinversecrc(unsigned char *c){
  unsigned char t[4];
  t[0]=(crc16&0xff);
  t[1]=((crc16>>8)&0xff);
  t[2]=0x00;
  t[3]=0x00;
  // for(i=0;i<4;i++)printf("%02x",t[i]&0xff);printf("\n");
  dshuffle2(t,c,0);
  // for(i=0;i<4;i++)printf("%02x",c[i]&0xff);printf("\n");
}

void EPROMBadBit_SVFFileFix::setflags(){
  unsigned char clr[4];
    v_instr=1; // expect 0-data 1-instr
    v_pktyp=0; // packet 1-type1  2-type2
    v_data=0; // expect data if 1
    v_count=-1;
    v_ecount=0;
    v_register=-99;
    crc16=0x0000;
    fdri_cnt=0;
    tfdri_cnt=0;
    v_endcrc=0;
    fifo(2,0,clr);
    fixbitcount=0;
}


} // namespace emu::pc                                                        
} // namespace emu 
