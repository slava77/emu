#include "EmuSLinkReader.h"

#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>

EmuSLinkReader::EmuSLinkReader( int boardIndex,  int format, bool debug )
  : EmuReader( format, debug ),
    theBoardIndex( boardIndex ),
    receiver( NULL ),
    frag( NULL )
{
  open();
  reset();
}

EmuSLinkReader::~EmuSLinkReader(){
  close();
}

void EmuSLinkReader::open() {
  receiver = fedkit_open (theBoardIndex, NULL);
  if (receiver == NULL) {
    exit (1);
  }
  //        sleep(10);
  //  fedkit_set_block_number(receiver,2050);
  // fedkit_set_block_size(receiver,8192);
  fedkit_start (receiver);
}

void EmuSLinkReader::close() {
  fedkit_close (receiver);
}


int EmuSLinkReader::reset(void){
  fedkit_close(receiver);
  sleep(1);
  receiver = fedkit_open(0, NULL);
  if (receiver == NULL) {
    std::cout << "EmuSLinkReader::reset: FAILED TO INITIALIZE. Aborting ..." << std::endl;
    abort();
  }
  fedkit_start (receiver);
  return 0;
}


int EmuSLinkReader::chunkSize() {
  //int cnt[2];
  //std::read(theFileDescriptor,cnt,4);
  //return cnt[0];
  return 8976; //9000;
}

int EmuSLinkReader::readDDU(unsigned short **buf) {
  // TODO
  return -1;
}

int EmuSLinkReader::readDCC(unsigned short **buf) {
  char b[50000];
  char *c,*d;
  int nb,bs,endcond,i,j;
  unsigned long int add,addl;
  int retry;
  unsigned short int tmp[4];
  char *a;
  unsigned int count,len;
  retry=0; 
 GETMORE:
  frag = fedkit_frag_get (receiver, NULL);
  if(frag==0){
    usleep(50);
    retry=retry+1;
    if(retry>=100){
      count=6;
      tmp[0]=0x0003;
      tmp[1]=0xffff;
      tmp[2]=0xffff;
      if (theDebugPrints) printf(" ***** slink: no more data \n");
      *buf=(unsigned short int  *)tmp;
      return count;
    }
    goto GETMORE;
  }
  nb=fedkit_frag_block_number(frag);
  addl=(unsigned long int)fedkit_frag_block(frag,0);
  endcond=0;
  for(i=1;i<nb;i++){
    add=(unsigned long int)fedkit_frag_block(frag,i);
    if(add<addl)endcond=1;
    addl=add;
  }
  if(endcond==1){
    count=frag->wc;
    bs=fedkit_frag_block_size(frag);
    c=b;
    for(i=0;i<nb;i++){
      add=(unsigned long int)fedkit_frag_block(frag,i);
      //  printf(" add %d %08x \n",i,add);
      d=(char *)fedkit_frag_block(frag,i);
      for(j=0;j<bs;j++){
	*c=*d;
	d=d+1;
	c=c+1;
      }
    }
    len=count;
    *buf=(unsigned short int *)b;
    //  printf(" RESORTED TO USING B! \n");
  }else{         
    count=frag->wc;
    a=(char *)fedkit_frag_block(frag,0);
    fedkit_frag_release (frag);
    len=count;
    *buf=(unsigned short int *)a;
  }
  return len;
}
