//-----------------------------------------------------------------------
// $Id: DDUReader.h,v 2.3 2005/10/03 20:20:14 geurts Exp $
// $Log: DDUReader.h,v $
// Revision 2.3  2005/10/03 20:20:14  geurts
// Removed hardware-related implementations out of DDUReader, created dependency on driver-include files.
// - openFile is virtual function, HardwareDDU and FileReaderDDU take care of its own implementation
// - schar.h and eth_hook_2.h contain driver and bigphys parameters shared by the DDUReadout and eth_hook_2
//
// Revision 2.2  2005/10/03 19:20:22  geurts
// BigPhys/Gbit driver and reader updates to prevent bigphys data corruption
//
// Revision 2.1  2005/09/26 17:11:49  tumanov
// new data format
//
// Revision 2.0  2005/04/13 10:52:56  geurts
// Makefile
//
//
//-----------------------------------------------------------------------
#ifndef DDUReader_h
#define DDUReader_h

#include <fstream>
#include <string>
#include "EventReader.h"


class DDUReader : public EventReader
{
public:
  DDUReader() : nbytes_schar(0), npack_schar(0), nbyte_schar(0) {}
  virtual ~DDUReader() {};
  virtual void Configure() {};
  virtual void Enable() {};
  /// assumes fd_schar has been set
  virtual int readDDU(unsigned short **buf, const bool debug = false) = 0;
  /// assumes readNextEvent has been called

  //virtual int check(int & EndofEvent, int count, unsigned short * tmp);

  static void setDebug(bool value) {debug = value;}

  /// from EventReader interface
  bool readNextEvent();
  int eventNumber();
  int triggerID(void){ return eventNumber(); }

  char * data() {return (char *) theBuffer;}
  int dataLength()            {return theDataLength;}
  virtual int reset()        = 0;
  virtual int enableBlock()  = 0;
  virtual int disableBlock() = 0;
  virtual int endBlockRead() = 0;

  virtual void printStats();
  virtual int openFile(std::string filename);
  void closeFile();

  unsigned short errorFlag;
  unsigned int errorStat;
  bool fillBuff;

protected:
  /// How many bytes to read at a time
  virtual int chunkSize() = 0;

  int fd_schar;
  unsigned long nbytes_schar;
  int npack_schar;
  int nbyte_schar;
  bool liveData_;

  static bool debug;
  //buffer containing event data
  short unsigned int * theBuffer;
  int theDataLength;



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
  int short ring_loop;
  int short ring_loop_kern;
  int short ring_loop_kern2;
  int short timeout;  // timeout waiting for event
  int short packets; // # of packets in event
  int short pmissing;    // packets are  missing at beginning
  int short pmissing_prev; // packets are missing at end
  int short end_event;   // end of event seen
  int short overwrite;   // overwrite

  char *tail_start;

};

#endif
