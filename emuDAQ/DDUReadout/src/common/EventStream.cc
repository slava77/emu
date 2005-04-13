//-----------------------------------------------------------------------
// $Id: EventStream.cc,v 2.0 2005/04/13 10:52:57 geurts Exp $
// $Log: EventStream.cc,v $
// Revision 2.0  2005/04/13 10:52:57  geurts
// Makefile
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
// Revision 1.12  2004/06/19 01:26:28  tfcvs
// added prefix_ string allowing full path for data files. Also added explicit path to runnum.tgt (FG)
//
// Revision 1.11  2004/06/11 07:59:46  tfcvs
// New DDU readout code: improved blocked schar readout.
//
//-----------------------------------------------------------------------
#include "EventStream.h"

#include <iostream>
#include <sstream>
//fg #include <strstream>
#include <string>

EventStream::EventStream(int runNumber, int eventsPerFile, std::string suffix, std::string prefix)
:  eventsInFileCounter_(0), fileNum_(0), runNumber_(runNumber),
   eventsPerFile_(eventsPerFile), suffix_(suffix), prefix_(prefix)
{
}


const char * EventStream::nameFile()
{
  std::ostringstream filename;
  //fg std::strstream filename;
  filename << prefix_ << "RunNum" << runNumber_ << "Evs" << eventsPerFile_*fileNum_
           << "to" << eventsPerFile_*(fileNum_+1)-1 << suffix_ << '\0';
#ifdef DEBUG_
  std::cout << "WANNA OPEN " << filename.str() << std::endl;
#endif
  return filename.str().c_str();
}
  

void EventStream::newEvent() {
  eventsInFileCounter_++;
  if (eventsInFileCounter_ >= eventsPerFile_)
  {
    closeFile();
#ifdef DEBUG_
    std::cout << "OPENING NEW FILE " << std::endl;
#endif
    fileNum_++;
    openFile();
    eventsInFileCounter_=0;
  }
}

