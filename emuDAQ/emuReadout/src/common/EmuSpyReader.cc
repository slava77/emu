#include "EmuSpyReader.h"

#include <iostream>
#include <sstream>
#include <string>
#include <fcntl.h>  // for open()
#include <unistd.h> // for read(), close()
#include <errno.h>  // for errno
#include <sys/ioctl.h>
#include <sys/mman.h>
#include "schar.h"
#include "eth_hook_2.h"
#include <stdexcept>   // std::runtime_error

// DEBUG START
#include <iomanip>
// DEBUG END

EmuSpyReader::EmuSpyReader( std::string filename, int format, bool debug )
  : EmuReader( filename, format, debug ),
    theFileDescriptor( -1 )
// DEBUG START
    ,ec(100)
// DEBUG END
{
  open( filename );
  // MOVED TO enable() START
//   reset();
//   enableBlock();
  // MOVED TO enable() END
//   enable();
// DEBUG START
  visitCount =  0;
  oversizedCount =  0;
  pmissingCount =  0;
  loopOverwriteCount =  0;
  bufferOverwriteCount =  0;
  packetsCount =  0;
  timeoutCount =  0;
  endEventCount = 0;
// DEBUG END
}

EmuSpyReader::~EmuSpyReader(){
  close();
}

void EmuSpyReader::open(std::string filename) {
  theLogMessage = "";
  theFileDescriptor = ::open(filename.c_str(), O_RDONLY);
  if ( theDebugMode ){
    if (filename.find("/dev/schar")==0)
      std::cout << "EmuSpyReader::open: we have got a life one here, Jimmy: " << filename << std::endl;

    // Abort in case of any failure
    if (theFileDescriptor == -1) {
      std::cerr << ": FATAL in open - " << std::strerror(errno) << std::endl;
      std::cerr << " will abort!!!" << std::endl;
      abort();
    }
  }
  if (theFileDescriptor == -1){
    std::stringstream ss;
    ss << "Opening " << theName << ": " << std::strerror(errno);
    throw std::runtime_error( ss.str().c_str() );
  }
 
#ifdef USE_DDU2004
  // MemoryMapped DDU2004 readout
  buf_start = (char *)mmap(NULL,BIGPHYS_PAGES_2*PAGE_SIZE,PROT_READ,MAP_PRIVATE,theFileDescriptor,0);
  if ( theDebugMode ){
    if (buf_start==MAP_FAILED) {
      std::cerr << "EmuSpyReader::open: FATAL in memorymap - " << std::strerror(errno) << std::endl;
      std::cerr << "EmuSpyReader::open will abort!!!" << std::endl;
      abort();
    }
    std::cout << "EmuSpyReader::open: Memory map succeeded " << std::endl;
  }
  if (buf_start==MAP_FAILED){
    std::stringstream ss;
    ss << "Memory map for " << theName << ": " << std::strerror(errno);
    throw std::runtime_error( ss.str().c_str() );
  }
  theLogMessage = "Memory map succeeded.";
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
 
}

void EmuSpyReader::close() {
#ifdef USE_DDU2004
  // new MemoryMapped DDU readout
  if ( theDebugMode ) std::cout << "close and unmmap" << std::endl;
  munmap((void *)buf_start,BIGPHYS_PAGES_2*PAGE_SIZE);
#endif
  ::close(theFileDescriptor);
}


int EmuSpyReader::reset(void){
  int status;
  if ( theDebugMode ) std::cout << "EmuSpyReader: reset" << std::endl;
  if((status=ioctl(theFileDescriptor,SCHAR_RESET))==-1){
     if ( theDebugMode ) std::cout << "EmuSpyReader: error in reset - " << std::strerror(errno) << std::endl;
     else{
       std::stringstream ss;
       ss << "Reset " << theName << ": " << std::strerror(errno);
       throw std::runtime_error( ss.str() );
     }
  }
  theLogMessage += " Reset.";
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

int EmuSpyReader::enableBlock(void){
  int status;
  if ( theDebugMode ) std::cout << "EmuSpyReader: enableBlock" << std::endl;
  if((status=ioctl(theFileDescriptor,SCHAR_BLOCKON))==-1){
    if ( theDebugMode ) std::cout << "EmuSpyReader: error in enableBlock - " << std::strerror(errno) << std::endl;
    else{
      std::stringstream ss;
      ss << "Enable block for " << theName << ": " << std::strerror(errno);
      throw std::runtime_error( ss.str() );
    }
  }
  theLogMessage += " Block enabled.";
  return status;
}

int EmuSpyReader::disableBlock(void){
  int status;
  if ( theDebugMode ) std::cout << "EmuSpyReader: disableBlock" << std::endl;
  if((status=ioctl(theFileDescriptor,SCHAR_BLOCKOFF))==-1){
    if ( theDebugMode ) std::cout << "EmuSpyReader: error in disableBlock - " << std::strerror(errno) << std::endl;
    else{
      std::stringstream ss;
      ss << "Disable block for " << theName << ": " << std::strerror(errno);
      throw std::runtime_error( ss.str() );
    }
  }
  theLogMessage += " Block enabled.";
  return status;
}


int EmuSpyReader::endBlockRead(){
  int status;
  if ( theDebugMode ) std::cout << "EmuSpyReader: endBlockRead" << std::endl;
  if((status=ioctl(theFileDescriptor,SCHAR_END))==-1){
    if ( theDebugMode ) std::cout << "EmuSpyReader: error in endBlockRead - " << std::strerror(errno) << std::endl;
    else{
      std::stringstream ss;
      ss << "End block read from " << theName << ": " << std::strerror(errno);
      throw std::runtime_error( ss.str() );
    }
  }
  return status;
}

int EmuSpyReader::chunkSize() {
  //int cnt[2];
  //std::read(theFileDescriptor,cnt,4);
  //return cnt[0];
  return 8976; //9000;
}

void EmuSpyReader::resetAndEnable(){
  theLogMessage = "";
  reset();
  enableBlock();
  theDeviceIsResetAndEnabled = true;
}

// int EmuSpyReader::readDDU(unsigned short **buf) {
int EmuSpyReader::readDDU(unsigned short*& buf) {
  theLogMessage = "";
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
    count = ::read(theFileDescriptor,&(tmpRead[pointer]),sizeOfPacket);
    if (count <0) std::cout << "EmuSpyReader::readDDU: ERROR - " <<std::strerror(errno) << std::endl;
    
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
                                                                                
//   while (true){
//     buf_pnt_kern=*(unsigned long int *)(buf_start+BIGPHYS_PAGES_2*PAGE_SIZE-TAILPOS);
//     if(iloop>100000){timeout=1; break;}
//     // printf(" %ld %ld \n",buf_pnt,buf_pnt_kern);
//     if(buf_pnt==buf_pnt_kern){for (j=0;j<5000;j++); iloop++; continue;}
//     iloop=0;
//     ring_loop_kern= *(unsigned short int *)(ring_start+ring_pnt*RING_ENTRY_LENGTH);
//     pmissing=ring_loop_kern&0x8000;
//     end_event=ring_loop_kern&0x4000;
//     ring_loop_kern=ring_loop_kern&0x3fff;
//     length=*(unsigned short int *)(ring_start+ring_pnt*RING_ENTRY_LENGTH+4);
//     //cout<<length<<"\n"; //fg
//     ring_loop_kern2=*(unsigned short int *)ring_start;
//     ring_loop_kern2=0x3fff&ring_loop_kern2;

//     if((ring_loop_kern2!=ring_loop)&&(buf_pnt<=buf_pnt_kern)){
//       overwrite=1;
//       if ( theDebugMode ) std::cout << "EmuSpyReader::readDDU:  BUFFER OVERWRITE" << std::endl; 
//       theLogMessage = theName+": buffer overwrite.";
//       break;
//     }
//     if(ring_loop_kern!=ring_loop){
//       overwrite=1;
//       if ( theDebugMode ) std::cout << "EmuSpyReader::readDDU: LOOP OVERWRITE" << std::endl;
//       theLogMessage = theName+": loop overwrite.";
//       break;
//     }
//     if(packets==0){
//       buf_data=buf_start+buf_pnt;
//     }
//     len=len+length;

//     if(pmissing!=0&packets!=0){
//       len=len-length;
//       pmissing_prev=1;
//       break;
//     }

//     buf_pnt=buf_pnt+length;
//     ring_pnt=ring_pnt+1;
//     //    if((buf_pnt > buf_end)||(ring_pnt>=ring_size)){
//     if (((end_event==0x4000)&&(buf_pnt>buf_eend))||(buf_pnt > buf_end)||(ring_pnt>=ring_size)){
//       ring_pnt=0;
//       ring_loop=ring_loop+1;
//       buf_pnt=0;
//     }
//     packets=packets+1;
//     if(len>MAXEVENT_2) break;
//     if(pmissing!=0) break;
//     if(end_event==0x4000) break;
//   }

  // DEBUG START                         
  while (true){
    buf_pnt_kern=*(unsigned long int *)(buf_start+BIGPHYS_PAGES_2*PAGE_SIZE-TAILPOS);
    if(iloop>100000){timeout=1; timeoutCount++; break;}
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
      if ( theDebugMode ) std::cout << "EmuSpyReader::readDDU:  BUFFER OVERWRITE" << std::endl; 
      theLogMessage = theName+": buffer overwrite.";
      bufferOverwriteCount++;
      break;
    }
    if(ring_loop_kern!=ring_loop){
      overwrite=1;
      if ( theDebugMode ) std::cout << "EmuSpyReader::readDDU: LOOP OVERWRITE" << std::endl;
      theLogMessage = theName+": loop overwrite.";
      loopOverwriteCount++;
      break;
    }
    if(packets==0){
      buf_data=buf_start+buf_pnt;
    }
    len=len+length;

    if(pmissing!=0&packets!=0){
      len=len-length;
      pmissing_prev=1;
      packetsCount++;
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
    if(len>MAXEVENT_2) {oversizedCount++; break;}
    if(pmissing!=0) {pmissingCount++; break;}
    if(end_event==0x4000) {endEventCount++; break;}
  }
  visitCount++;
  if ( ec.timeIsUp() ){
    std::cout << " v:" << std::setw(10) << visitCount
	      << " o:" << std::setw(10) << oversizedCount
	      << " m:" << std::setw(10) << pmissingCount
	      << " b:" << std::setw(10) << bufferOverwriteCount
	      << " l:" << std::setw(10) << loopOverwriteCount
	      << " p:" << std::setw(10) << packetsCount
	      << " t:" << std::setw(10) << timeoutCount
	      << " e:" << std::setw(10) << endEventCount
	      << " s:" << std::setw(10) << len 
	      << std::endl;
    std::cout << std::flush;
  }
  // DEBUG END                                                                  


//   if ( timeout ) std::cerr << "EmuSpyReader::readDDU timed out. Event length: " << len << " b" << std::endl;

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
//   *buf=(unsigned short int *)buf_data;
  buf=(unsigned short*)buf_data;

  return len;
  //-------------------------------------------------------------------//
  #endif
  //-------------------------------------------------------------------//
}

int EmuSpyReader::readDCC(unsigned short*& buf) {
  // TODO
  return -1;
}
