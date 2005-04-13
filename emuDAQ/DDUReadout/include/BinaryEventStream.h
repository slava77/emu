//-----------------------------------------------------------------------
// $Id: BinaryEventStream.h,v 2.0 2005/04/13 10:52:56 geurts Exp $
// $Log: BinaryEventStream.h,v $
// Revision 2.0  2005/04/13 10:52:56  geurts
// Makefile
//
//
//-----------------------------------------------------------------------
#ifndef BinaryEventStream_h
#define BinaryEventStream_h

/** Writes a binary file, given the character
    buffer input for every event.  It uses the
    EventStream mechanism to change files
    after a given number of events
 */

//#include <string>
#include <fstream>
#include "EventStream.h"

class BinaryEventStream : public EventStream
{
 public:
  BinaryEventStream(int runNumber=0, int eventsPerFile=1000); 
  virtual ~BinaryEventStream() {};
  virtual void openFile();
  virtual void closeFile();

  int writeEnv(char * buffer, unsigned int byteCount);
  
 protected:
  std::ofstream fout;
};
 
#endif
