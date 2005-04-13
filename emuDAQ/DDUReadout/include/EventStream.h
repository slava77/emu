//-----------------------------------------------------------------------
// $Id: EventStream.h,v 2.0 2005/04/13 10:52:56 geurts Exp $
// $Log: EventStream.h,v $
// Revision 2.0  2005/04/13 10:52:56  geurts
// Makefile
//
//
//-----------------------------------------------------------------------
#ifndef EventStream_h
#define EventStream_h

/** This class opens a file for a given run number,
    and closes it after a given number of events and
    opens the next one.
    The files will be named something like
    RunNum1Evs0to999.dat
*/


#include <string>

class EventStream 
{
 public:
  EventStream(int runNumber, int eventsPerFile, std::string suffix, std::string prefix=""); 
  virtual ~EventStream() {};
  virtual void openFile() = 0;
  virtual void closeFile() = 0;
  const char * nameFile();
  void setRunNumber(int nrun) {runNumber_ = nrun;}

  /// to be called every event
  void newEvent();
  
 protected:
  int eventsInFileCounter_, fileNum_, runNumber_, eventsPerFile_;
  std::string suffix_;
  std::string prefix_;
};
 
#endif
