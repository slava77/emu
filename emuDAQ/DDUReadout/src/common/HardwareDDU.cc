//-----------------------------------------------------------------------
// $Id: HardwareDDU.cc,v 2.2 2005/10/03 20:20:15 geurts Exp $
// $Log: HardwareDDU.cc,v $
// Revision 2.2  2005/10/03 20:20:15  geurts
// Removed hardware-related implementations out of DDUReader, created dependency on driver-include files.
// - openFile is virtual function, HardwareDDU and FileReaderDDU take care of its own implementation
// - schar.h and eth_hook_2.h contain driver and bigphys parameters shared by the DDUReadout and eth_hook_2
//
// Revision 2.1  2005/10/03 19:20:23  geurts
// BigPhys/Gbit driver and reader updates to prevent bigphys data corruption
//
// Revision 2.0  2005/04/13 10:52:57  geurts
// Makefile
//
// Revision 1.4  2004/10/03 16:18:20  tfcvs
// introduced new memory mapped DDU readout, switchable with the USE_DDU2004 switch (FG)
//
// Revision 1.3  2004/08/18 16:51:37  tfcvs
// pass compiler
//
// Revision 1.2  2004/07/29 20:16:14  tfcvs
// tumanov
//
// Revision 1.1  2004/07/29 16:21:21  tfcvs
// *** empty log message ***
//
// Revision 1.17  2004/06/15 13:36:58  tfcvs
// Add missing errno header file. -JM
//
// Revision 1.16  2004/06/12 13:24:34  tfcvs
// minor changes in hardware readout; removal of unused variables (and warnings) (FG)
//
// Revision 1.15  2004/06/11 07:59:46  tfcvs
// New DDU readout code: improved blocked schar readout.
//
//-----------------------------------------------------------------------
#include "HardwareDDU.h"
#include "schar.h"
#include "eth_hook_2.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

HardwareDDU::HardwareDDU(std::string schar_path)
{
  //this->openFile(schar_path.c_str());
  //this->reset();
  //this->enableBlock();
}

HardwareDDU::~HardwareDDU()
{
  this->closeFile();
}

int HardwareDDU::openFile(std::string filename) {
  //std::cout << "filename here was " << filename << std::endl;
  liveData_ = (filename.find("/dev/schar")==0);
  if (liveData_)
    std::cout << "DDUread: we have got a life one here, Jimmy" << std::endl;
  fd_schar = open(filename.c_str(), O_RDONLY);
 
  // Abort in case of any failure
  if (fd_schar == -1) {
    std::cerr << "DDUReader: FATAL in openFile - " << std::strerror(errno) << std::endl;
    std::cerr << "DDUReader will abort!!!" << std::endl;
    abort();
  }
 
#ifdef USE_DDU2004
  // MemoryMapped DDU2004 readout
  buf_start = (char *)mmap(NULL,BIGPHYS_PAGES_2*PAGE_SIZE,PROT_READ,MAP_PRIVATE,fd_schar,0);
  if (buf_start==MAP_FAILED) {
    std::cerr << "DDUReader: FATAL in memorymap - " << std::strerror(errno) << std::endl;
    std::cerr << "DDUReader will abort!!!" << std::endl;
    abort();
  };
  std::cout << "DDUReader: Memory map succeeded " << std::endl;
  buf_end=(BIGPHYS_PAGES_2-RING_PAGES_2)*PAGE_SIZE-MAXPACKET_2;
  buf_eend=(BIGPHYS_PAGES_2-RING_PAGES_2)*PAGE_SIZE-TAILPOS-MAXEVENT_2;
  ring_start=buf_start+(BIGPHYS_PAGES_2-RING_PAGES_2)*PAGE_SIZE;
  ring_size=(RING_PAGES_2*PAGE_SIZE-RING_ENTRY_LENGTH-TAILMEM)/RING_ENTRY_LENGTH;
  tail_start=buf_start+BIGPHYS_PAGES_2*PAGE_SIZE-TAILPOS;
  buf_pnt=0;
  ring_pnt=0;
  ring_loop=0;
  pmissing=0;
  pmissing_prev=0;
#endif
 
  return 0;
}

int HardwareDDU::reset(void){
  int status;
  std::cout << "HardwareDDU: reset" << std::endl;
  if((status=ioctl(fd_schar,SCHAR_RESET))==-1)
     std::cout << "HardwareDDU: error in reset - " << std::strerror(errno) << std::endl;
#ifdef USE_DDU2004
  buf_pnt=0;
  ring_pnt=0;
  ring_loop=0;
  pmissing=0;
  pmissing_prev=0;
  status = 0; // not much use for the status word
#endif
  return status;
}

int HardwareDDU::enableBlock(void){
  int status;
  std::cout << "HardwareDDU: enableBlock" << std::endl;
  if((status=ioctl(fd_schar,SCHAR_BLOCKON))==-1)
    std::cout << "HardwareDDU: error in enableBlock - " << std::strerror(errno) << std::endl;
  return status;
}

int HardwareDDU::disableBlock(void){
  int status;
  std::cout << "HardwareDDU: disableBlock" << std::endl;
  if((status=ioctl(fd_schar,SCHAR_BLOCKOFF))==-1)
    std::cout << "HardwareDDU: error in disableBlock - " << std::strerror(errno) << std::endl;
  return status;
}


int HardwareDDU::endBlockRead(){
  int status;
  std::cout << "HardwareDDU: endBlockRead" << std::endl;
  if((status=ioctl(fd_schar,SCHAR_END))==-1)
    std::cout << "HardwareDDU: error in endBlockRead - " << std::strerror(errno) << std::endl;
  return status;
}

int HardwareDDU::chunkSize() {
  //int cnt[2];
  //std::read(fd_schar,cnt,4);
  //return cnt[0];
  return 8976; //9000;
}


int HardwareDDU::readDDU(unsigned short **buf, const bool debug) {
#ifndef USE_DDU2004
//---------------------------------------------------------------------//
// use the -DUSE_DDU2004 switch in the Makefile to switch between DDU readout modes
//---------------------------------------------------------------------//

  // the size of package is somewhat smaller than 9000 as some
  // stuff gets stripped off before it arrives
  const unsigned int ushortsize = sizeof(unsigned short);
  const unsigned int sizeOfPacket=8976*ushortsize; //9000;
  const int maxPackets=50;
  unsigned char tmpRead[maxPackets*sizeOfPacket];

  // continue to read 8976b packages until readout <8976b
  int pointer(0);
  int count(0);
  int packageCounter(0);


  // it is very reasonable to assume that the event lines up w/ the first package ...
  do {
    // read package and append to big buffer
    count=read(fd_schar,&(tmpRead[pointer]),sizeOfPacket);
    if (count <0) std::cout << "readDDU: ERROR - " <<std::strerror(errno) << std::endl;
    
    // update pointer
    pointer+=count;

    if(count<=64){
      // Find the TRL1 signature (00 80 00 80 ff ff 00 80) and
      // adjust the end of buffer to point after TRL3 (offset: 3x8bytes)
      for (int newPtr=pointer-count-1; newPtr<(pointer  -1); newPtr++){
	if ((tmpRead[newPtr  ]==0x00) && (tmpRead[newPtr+1]==0x80) &&
	    (tmpRead[newPtr+2]==0x00) && (tmpRead[newPtr+3]==0x80) &&
	    (tmpRead[newPtr+4]==0xff) && (tmpRead[newPtr+5]==0xff) &&
	    (tmpRead[newPtr+6]==0x00) && (tmpRead[newPtr+7]==0x80)){
	  pointer=newPtr+3*8;
	  break;
	}  
      }
    }


    // update package counter and check on buffer overflow
    if (++packageCounter>maxPackets){
      std::cerr << "dduRead: WARNING - INTERNAL BUFFER OVERFLOW" << std::endl;
      break;
    }
    // (LG) wait till I stop receiving full packets
    // (LG) should add header/tailer auto-check
  } while ((count * ushortsize)==sizeOfPacket);


#ifdef DEBUG_
  std::cout << "event statistics: " << packageCounter
       << " ("<< pointer*sizeof(tmpRead[0]) << "bytes)" << std::endl;
#endif

  // pass the joy on to rest of the world
    *buf=(unsigned short *)tmpRead;

  return pointer;
#else
//-------------------------------------------------------------------//
//  MemoryMapped DDU2004 readout
//-------------------------------------------------------------------//
  int j;
  int length, len(0);
  long int iloop(0);
  timeout=0;
  overwrite=0;
  pmissing=0;
  pmissing_prev=0;
  packets=0;
                                                                                
  while (true){
    buf_pnt_kern=*(unsigned long int *)(buf_start+BIGPHYS_PAGES_2*PAGE_SIZE-TAILPOS);
    if(iloop>100000){timeout=1; break;}
    // printf(" %ld %ld \n",buf_pnt,buf_pnt_kern);
    if(buf_pnt==buf_pnt_kern){for (j=0;j<5000;j++); iloop++; continue;}
    iloop=0;
    ring_loop_kern= *(unsigned short int *)(ring_start+ring_pnt*RING_ENTRY_LENGTH);
    pmissing=ring_loop_kern&0x8000;
    end_event=ring_loop_kern&0x4000;
    ring_loop_kern=ring_loop_kern&0x3fff;
    length=*(unsigned short int *)(ring_start+ring_pnt*RING_ENTRY_LENGTH+4);
    //cout<<length<<"\n"; //fg
    ring_loop_kern2=*(unsigned short int *)ring_start;
    ring_loop_kern2=0x3fff&ring_loop_kern2;

    if((ring_loop_kern2!=ring_loop)&&(buf_pnt<=buf_pnt_kern)){
      overwrite=1;
      std::cout << "readDDU:  BUFFER OVERWRITE" << std::endl; break;
    }
    if(ring_loop_kern!=ring_loop){
      overwrite=1;
      std::cout << "readDDU: LOOP OVERWRITE" << std::endl; break;
    }
    if(packets==0){
      buf_data=buf_start+buf_pnt;
    }
    len=len+length;

    if(pmissing!=0&packets!=0){
      len=len-length;
      pmissing_prev=1;
      break;
    }

    buf_pnt=buf_pnt+length;
    ring_pnt=ring_pnt+1;
    //    if((buf_pnt > buf_end)||(ring_pnt>=ring_size)){
    if (((end_event==0x4000)&&(buf_pnt>buf_eend))||(buf_pnt > buf_end)||(ring_pnt>=ring_size)){
      ring_pnt=0;
      ring_loop=ring_loop+1;
      buf_pnt=0;
    }
    packets=packets+1;
    if(len>MAXEVENT_2) break;
    if(pmissing!=0) break;
    if(end_event==0x4000) break;
  }

  //fg adjust the length to account for filler bytes
//  char *end_buf_data;
//  end_buf_data = buf_data + len;
//  int i(0);
//  while (*end_buf_data == 0xff){
//    i++;
//    end_buf_data--;
//
//  }
//  if (len>i) len -=i; else len =0;
//
    //-fg

  //MAINEND:
  *buf=(unsigned short int *)buf_data;

  return len;
  //-------------------------------------------------------------------//
  #endif
  //-------------------------------------------------------------------//
}
