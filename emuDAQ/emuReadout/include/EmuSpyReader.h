#ifndef __EMU_SPY_READER_H__
#define __EMU_SPY_READER_H__

#include "EmuReader.h"

// DEBUG START
#include <iomanip>
#include <sys/times.h>
class EmuClock{
  clock_t period_;
  clock_t start_;
  clock_t stop_;
  struct tms dummy_;
  
public:
  EmuClock( clock_t period ) :
    period_( period )
  {
    start_ = times( &dummy_ );
  }
  
  bool timeIsUp(){
    stop_ = times( &dummy_ );
    if ( stop_ > start_ + period_ ) {
      std::cout << std::setw(10) << stop_ ;
      start_ = stop_;
      return true;
    }
    return false;
  }
  
};
// DEBUG END

class EmuSpyReader : public EmuReader
{
public:
  enum Status_t {
    EndOfEventMissing = 0x0001,
    Timeout           = 0x0002,
    PacketsMissing    = 0x0004,
    LoopOverwrite     = 0x0008,
    BufferOverwrite   = 0x0010,
    Oversized         = 0x0020
  };

  EmuSpyReader( std::string filename, int format, bool debug=false );
  ~EmuSpyReader();
  void open( std::string filename );
  virtual void resetAndEnable();
  void close();
  int  reset();
  int  enableBlock();
  int  disableBlock();
  int  endBlockRead();
  int  chunkSize();

protected:
  int               theFileDescriptor;
  //all below needed for ddu2004 only
  // new additions for MemoryMapped DDU
  char *buf_data;
  char *buf_start;
  unsigned long int buf_pnt;
  unsigned long int buf_end;
  unsigned long int buf_eend;
  unsigned long int buf_pnt_kern;

  char *ring_start;
  unsigned long int ring_size;
  unsigned long int ring_pnt;
  short ring_loop;
  short ring_loop_kern;
  short ring_loop_kern2;
  short timeout;  // timeout waiting for event
  short packets; // # of packets in event
  short pmissing;    // packets are  missing at beginning
  short pmissing_prev; // packets are missing at end
  short end_event;   // end of event seen
  short overwrite;   // overwrite

  char *tail_start;

// DEBUG START
  int visitCount;
  int oversizedCount; 
  int pmissingCount; 
  int loopOverwriteCount; 
  int bufferOverwriteCount; 
  int timeoutCount; 
  int endEventCount;
  EmuClock ec;
// DEBUG END
  bool insideEventWithMissingPackets;

//   int   readDDU(unsigned short **buf);
//   int   readDCC(unsigned short **buf);
  int   readDDU(unsigned short*& buf);
  int   readDCC(unsigned short*& buf);
  int   dataLengthWithoutPadding( const unsigned short* data, const int dataLength );
};

#endif  // ifndef __EMU_SPY_READER_H__
