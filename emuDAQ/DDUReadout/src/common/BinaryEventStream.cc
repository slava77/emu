//-----------------------------------------------------------------------
// $Id: BinaryEventStream.cc,v 2.0 2005/04/13 10:52:57 geurts Exp $
// $Log: BinaryEventStream.cc,v $
// Revision 2.0  2005/04/13 10:52:57  geurts
// Makefile
//
// Revision 1.4  2004/10/03 16:09:56  tfcvs
// changed type of writeEnv argument to accomodate larger buffers (FG)
//
// Revision 1.3  2004/08/18 16:51:37  tfcvs
// pass compiler
//
// Revision 1.2  2004/07/29 19:50:03  tfcvs
// tumanov
//
// Revision 1.1  2004/07/29 16:21:21  tfcvs
// *** empty log message ***
//
// Revision 1.9  2004/06/11 07:59:46  tfcvs
// New DDU readout code: improved blocked schar readout.
//
//-----------------------------------------------------------------------
#include "BinaryEventStream.h"

#include <iostream>
//#include <string>

BinaryEventStream::BinaryEventStream(int runNumber, int eventsPerFile)
: EventStream(runNumber, eventsPerFile, ".bin")
{
 //fg are we really sure that we open a file here ...
 //fg openFile();
}

  
void BinaryEventStream::openFile() {
  fout.open(nameFile(), std::ofstream::out | std::ofstream::binary);
}


void BinaryEventStream::closeFile() {
  fout.close();
}


int BinaryEventStream::writeEnv(char * buffer, unsigned int byteCount)
{
  fout.write(buffer, byteCount);
  newEvent();
  return 0;
} 

 

