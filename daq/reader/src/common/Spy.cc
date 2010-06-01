#include "emu/daq/reader/Spy.h"

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

emu::daq::reader::Spy::Spy( std::string filename, int format, bool debug )
  : emu::daq::reader::Base( filename, format, debug ),
    theFileDescriptor( -1 )
{
// DEBUG START
  ec  = new emu::daq::reader::Clock(100);
  ec1 = new emu::daq::reader::Clock(100);
  ec2 = new emu::daq::reader::Clock(100);
  ec3 = new emu::daq::reader::Clock(100);
  ec4 = new emu::daq::reader::Clock(100);
// DEBUG END
  open( filename );
// DEBUG START
  visitCount =  0;
  oversizedCount =  0;
  pmissingCount =  0;
  loopOverwriteCount =  0;
  bufferOverwriteCount =  0;
  timeoutCount =  0;
  endEventCount = 0;
// DEBUG END
}

emu::daq::reader::Spy::~Spy(){
  close();
  delete ec;
  delete ec1;
  delete ec2;
  delete ec3;
  delete ec4;
}

void emu::daq::reader::Spy::open(std::string filename) {
  theLogMessage = "";
  theFileDescriptor = ::open(filename.c_str(), O_RDONLY);
  if ( theDebugMode ){
    if (filename.find("/dev/schar")==0)
      std::cout << "emu::daq::reader::Spy::open: we have got a life one here, Jimmy: " << filename << std::endl;

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
      std::cerr << "emu::daq::reader::Spy::open: FATAL in memorymap - " << std::strerror(errno) << std::endl;
      std::cerr << "emu::daq::reader::Spy::open will abort!!!" << std::endl;
      abort();
    }
    std::cout << "emu::daq::reader::Spy::open: Memory map succeeded " << std::endl;
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
  insideEventWithMissingPackets = false;
#endif
 
}

void emu::daq::reader::Spy::close() {
#ifdef USE_DDU2004
  // new MemoryMapped DDU readout
  if ( theDebugMode ) std::cout << "close and unmmap" << std::endl;
  munmap((void *)buf_start,BIGPHYS_PAGES_2*PAGE_SIZE);
#endif
  ::close(theFileDescriptor);
}


int emu::daq::reader::Spy::reset(void){
  int status;
  if ( theDebugMode ) std::cout << "Spy: reset" << std::endl;
  if((status=ioctl(theFileDescriptor,SCHAR_RESET))==-1){
     if ( theDebugMode ) std::cout << "Spy: error in reset - " << std::strerror(errno) << std::endl;
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
  insideEventWithMissingPackets = false;
#endif
  return status;
}

int emu::daq::reader::Spy::enableBlock(void){
  int status;
  if ( theDebugMode ) std::cout << "Spy: enableBlock" << std::endl;
  if((status=ioctl(theFileDescriptor,SCHAR_BLOCKON))==-1){
    if ( theDebugMode ) std::cout << "Spy: error in enableBlock - " << std::strerror(errno) << std::endl;
    else{
      std::stringstream ss;
      ss << "Enable block for " << theName << ": " << std::strerror(errno);
      throw std::runtime_error( ss.str() );
    }
  }
  theLogMessage += " Block enabled.";
  return status;
}

int emu::daq::reader::Spy::disableBlock(void){
  int status;
  if ( theDebugMode ) std::cout << "Spy: disableBlock" << std::endl;
  if((status=ioctl(theFileDescriptor,SCHAR_BLOCKOFF))==-1){
    if ( theDebugMode ) std::cout << "Spy: error in disableBlock - " << std::strerror(errno) << std::endl;
    else{
      std::stringstream ss;
      ss << "Disable block for " << theName << ": " << std::strerror(errno);
      throw std::runtime_error( ss.str() );
    }
  }
  theLogMessage += " Block enabled.";
  return status;
}


int emu::daq::reader::Spy::endBlockRead(){
  int status;
  if ( theDebugMode ) std::cout << "Spy: endBlockRead" << std::endl;
  if((status=ioctl(theFileDescriptor,SCHAR_END))==-1){
    if ( theDebugMode ) std::cout << "Spy: error in endBlockRead - " << std::strerror(errno) << std::endl;
    else{
      std::stringstream ss;
      ss << "End block read from " << theName << ": " << std::strerror(errno);
      throw std::runtime_error( ss.str() );
    }
  }
  return status;
}

int emu::daq::reader::Spy::chunkSize() {
  //int cnt[2];
  //std::read(theFileDescriptor,cnt,4);
  //return cnt[0];
  return 8976; //9000;
}

void emu::daq::reader::Spy::resetAndEnable(){
  theLogMessage = "";
  reset();
  enableBlock();
  theDeviceIsResetAndEnabled = true;
}

int emu::daq::reader::Spy::readDDU(unsigned short*& buf) {

  theLogMessage = "";
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

  visitCount++;

  // EndOfEventMissing bit will be unset when EndOfEvent is found
  theErrorFlag = EndOfEventMissing;

  while (true){

    // Get the write pointer (relative to buf_start) of the kernel driver.
    buf_pnt_kern=*(unsigned long int *)(buf_start+BIGPHYS_PAGES_2*PAGE_SIZE-TAILPOS);

//     // START DEBUG
//     if ( ec1->timeIsUp() ){
//       std::cout << " v:" << std::setw(10) << visitCount
//                 << " idle loop count=" << iloop
//                 << " buf_pnt_kern=" << buf_pnt_kern
//                 << " buf_pnt=" << buf_pnt
//                 << std::endl << std::flush;
//     }
//     // END DEBUG

    // If no data for a long time, abort.
    if(iloop>50000){timeout=1; timeoutCount++; theErrorFlag|=Timeout; break;}

    // If the write pointer buf_pnt_kern hasn't yet moved from the read pointer buf_pnt, 
    // wait a bit and retry in the next loop.
    if(buf_pnt==buf_pnt_kern){for (j=0;j<5000;j++); iloop++; continue;}

    // The kernel driver has written new data. No more idle looping. Reset the idle loop counter.
    iloop=0;

    // From the current entry of the packet info ring,...
    ring_loop_kern= *(unsigned short int *)(ring_start+ring_pnt*RING_ENTRY_LENGTH);
    // ...get the missing packet flag,...
    pmissing=ring_loop_kern&0x8000;
    // ...the end-of-event flag,...
    end_event=ring_loop_kern&0x4000;
    // ...the reset ("loop-back") counter,...
    ring_loop_kern=ring_loop_kern&0x3fff;
    // ...and the length of data in bytes.
    length=*(unsigned short int *)(ring_start+ring_pnt*RING_ENTRY_LENGTH+4);

    // Get the reset counter from the first entry of the packet info ring...
    ring_loop_kern2=*(unsigned short int *)ring_start;
    ring_loop_kern2=0x3fff&ring_loop_kern2;

//     // START DEBUG
//     if ( ec2->timeIsUp() ){
//       std::cout << " v:" << std::setw(10) << visitCount
//                 << " ring_loop_kern=" << ring_loop_kern
//                 << " ring_loop_kern2=" << ring_loop_kern2
//                 << " ring_loop=" << ring_loop
//                 << " ring_pnt=" << ring_pnt
//                 << " buf_pnt_kern=" << buf_pnt_kern
//                 << " buf_pnt=" << buf_pnt
//                 << " length=" << length
//                 << " pmissing=" << (pmissing==0x8000?"TRUE":"FALSE")
//                 << " end_event=" << (end_event==0x4000?"TRUE":"FALSE")
//                 << std::endl << std::flush;
//     }
//     // END DEBUG


    // ... and compare it with our reset count. If they're different, the
    // kernel has looped back (at least) one time more than we have.
    // If, in addition, the write pointer has moved past our read pointer, data must have
    // been overwritten.
//     if((ring_loop_kern2!=ring_loop)&&(buf_pnt<=buf_pnt_kern)){
    // To be pedantic:
    // Write pointer has looped back one time more than the read pointer and overtaken the read pointer,
    // OR it has looped back more than one time more (unlikely, but who knows...).
    if( ( (ring_loop_kern2==ring_loop+1)&&(buf_pnt<=buf_pnt_kern) ) ||
	(ring_loop_kern2>ring_loop+1)                                  ){
      theLogMessage = theName+": buffer overwrite.";
      bufferOverwriteCount++;
      theErrorFlag|=BufferOverwrite;

//       // START DEBUG
//       if ( ec3->timeIsUp() ){
// 	std::cout << " v:" << std::setw(10) << visitCount
// 		  << " " << theLogMessage << " " 
// 		  << " ring_loop_kern=" << ring_loop_kern
// 		  << " ring_loop_kern2=" << ring_loop_kern2
// 		  << " ring_loop=" << ring_loop
// 		  << " ring_pnt=" << ring_pnt
// 		  << " buf_pnt_kern=" << buf_pnt_kern
// 		  << " buf_pnt=" << buf_pnt
// 		  << " bufferOverwriteCount=" << bufferOverwriteCount
// 		  << " loopOverwriteCount=" << loopOverwriteCount
// 		  << std::endl << std::flush;
//       }
//       // END DEBUG

      // Reset the read pointers.
      buf_pnt  = 0;
      ring_pnt = 0;
      // Synchronize our loop-back counter to the driver's.
      ring_loop = ring_loop_kern2;
      // Read no data this time.
      len = 0;
      // Let the next event start with a clean record.
      insideEventWithMissingPackets = false;

      break;
    }

    // The data may not have been overwritten, but the packet info ring may. 
    // Check whether the driver's loop-back count is different from ours.
    if(ring_loop_kern!=ring_loop){
      theLogMessage = theName+": loop overwrite.";
      loopOverwriteCount++;
      theErrorFlag|=LoopOverwrite;

//       // START DEBUG
//       if ( ec4->timeIsUp() ){
// 	std::cout << " v:" << std::setw(10) << visitCount
// 		  << " " << theLogMessage << " " 
// 		  << " ring_loop_kern=" << ring_loop_kern
// 		  << " ring_loop_kern2=" << ring_loop_kern2
// 		  << " ring_loop=" << ring_loop
// 		  << " ring_pnt=" << ring_pnt
// 		  << " buf_pnt_kern=" << buf_pnt_kern
// 		  << " buf_pnt=" << buf_pnt
// 		  << " bufferOverwriteCount=" << bufferOverwriteCount
// 		  << " loopOverwriteCount=" << loopOverwriteCount
// 		  << std::endl << std::flush;
//       }
//       // END DEBUG

      // Reset the read pointers.
      buf_pnt  = 0;
      ring_pnt = 0;
      // Synchronize our loop-back counter to the driver's.
      ring_loop = ring_loop_kern2;
      // Read no data this time.
      len = 0;
      // Let the next event start with a clean record.
      insideEventWithMissingPackets = false;

      break;
    }

    // Remember the position of the start of data...
    if(packets==0){
      buf_data=buf_start+buf_pnt;
    }
    // ...and add its length to the total.
    len=len+length;

    // Increment data ring pointer...
    buf_pnt=buf_pnt+length;
    // ...and packet info ring pointer.
    ring_pnt=ring_pnt+1;

    // If this packet ends the event but another event may not fit in the remaining space (beyond buf_eend),
    // OR another packet may not fit in the remaining space (beyond buf_end),
    // OR the end of the packet info ring has been reached, 
    // then reset the read pointers (loop back) and increment the loop-back counter.
    // This condition must be exactly the same as that in the driver (eth_hook_<N>.c) for the
    // write and read pointers to loop back from the same point.
    if (((end_event==0x4000)&&(buf_pnt>buf_eend))||(buf_pnt > buf_end)||(ring_pnt>=ring_size)){
      ring_pnt=0;
      ring_loop=ring_loop+1;
      buf_pnt=0;
    }

    // Increment packet count.
    packets=packets+1;

    // Mark this event as oversized to keep a tally
    if(len>MAXEVENT_2){
      theErrorFlag|=Oversized;
    }

    // If this event already has packets missing, don't read it
    if ( insideEventWithMissingPackets ) len = 0;

    // If packets are missing, don't read out anything, just keep a tally.
    if(pmissing!=0){
      pmissingCount++; 
      theErrorFlag|=PacketsMissing;
      // Remember that we are inside a defective event until we reach the end of it.
      // (Or the end of the next event, for that matter, it the end of this one happens to be missing.)
      insideEventWithMissingPackets = true;
      len = 0;
    }

    // Keep a tally of properly ending events.
    if(end_event==0x4000) {
      endEventCount++;
      // unset EndOfEventMissing bit
      theErrorFlag &= ~EndOfEventMissing;
      // Defective or not, this event is ending. Let the next one start with a clean record.
      insideEventWithMissingPackets = false;
      break;
    }

    // break; // HACK: return after each packet

  } // while (true)

  // Keep a tally of oversized events
  if(theErrorFlag & Oversized){
    oversizedCount++;
  }

  // Pack the number of packets into the upper byte of theErrorFlag
  theErrorFlag |= packets << 8; 

  // Periodically write out counters for debugging purposes
  if ( ec->timeIsUp() ){
    std::cout << " v:" << std::setw(10) << visitCount
	      << " o:" << std::setw(10) << oversizedCount
	      << " m:" << std::setw(10) << pmissingCount
	      << " b:" << std::setw(10) << bufferOverwriteCount
	      << " l:" << std::setw(10) << loopOverwriteCount
	      << " t:" << std::setw(10) << timeoutCount
	      << " e:" << std::setw(10) << endEventCount
	      << " p:" << std::setw(10) << packets
	      << " s:" << std::setw(10) << len 
	      << std::endl << std::flush;
  }

  //MAINEND:
  buf=(unsigned short*)buf_data;

  theDataLength = dataLengthWithoutPadding( buf, len );

  //   std::cout << "Data length " << len << std::endl << "without padding " << theDataLength << std::endl << std::flush;

  return len;
}

int emu::daq::reader::Spy::readDCC(unsigned short*& buf) {
  // TODO
  return -1;
}

int emu::daq::reader::Spy::dataLengthWithoutPadding( const unsigned short* data, const int dataLength ){
  // Get the data length without the padding that may have been added by Gbit Ethernet.

  // Jason Gilmore:
  // "The exact format of the Filler is (8*N)-1 "FF" bytes, preceded by a 1-byte
  // count of how many real data bytes in the packet preceded the Filler."

//   const int minEthPacketSize = 32; // 2-byte words (64 bytes)
  const int fillerWordSize   =  4; // 2-byte words (8 bytes); the total filler size is an integer multiple of this

  if ( !dataLength ) return 0;

  // The following conditional would be useful if we examined individual packets. That's not the case:
  // We examine events, which may consist of more than one packet (>64 bytes) and still have padding at the end.
//   if ( dataLength > 2*minEthPacketSize ) return dataLength; // no reason for Ethernet padding

  // Let's go backward looking for the first non-filler 8-byte word:
  for ( int iShort=dataLength/2-fillerWordSize; iShort>=0; iShort-=fillerWordSize ){
    if ( !( ( data[iShort] & 0xFF00 ) == 0xFF00 &&
	    data[iShort+1]            == 0xFFFF &&
	    data[iShort+2]            == 0xFFFF &&
	    data[iShort+3]            == 0xFFFF    ) )
      return 2 * ( iShort + fillerWordSize );
  }

  return 0; // all filler ?!
}
