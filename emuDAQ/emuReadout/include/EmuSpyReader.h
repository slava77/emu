#ifndef __EMU_SPY_READER_H__
#define __EMU_SPY_READER_H__

#include "EmuReader.h"

class EmuSpyReader : public EmuReader
{
public:
  EmuSpyReader( std::string filename, int format, bool debug=false );
  ~EmuSpyReader();
  void open( std::string filename );
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

//   int   readDDU(unsigned short **buf);
//   int   readDCC(unsigned short **buf);
  int   readDDU(unsigned short*& buf);
  int   readDCC(unsigned short*& buf);
};

#endif  // ifndef __EMU_SPY_READER_H__
