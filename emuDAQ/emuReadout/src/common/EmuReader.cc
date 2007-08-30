#include "EmuReader.h"

int EmuReader::readNextEvent(){
  // get this CPU hog out of here:     unsigned short ** buf2 = new unsigned short* ;
  if ( theFormat == DDU ) theNumberOfReadBytes = readDDU( theBuffer );
  else                    theNumberOfReadBytes = readDCC( theBuffer );
  if ( theDebugMode ) std::cout << " theNumberOfReadBytes " << theNumberOfReadBytes << std::endl;
  if ( theNumberOfReadBytes<=7 ) {
    return theNumberOfReadBytes;
  }
  // get this CPU hog out of here:    delete buf2;
  // new and delete: ~400s/10^9 on a 2.4GHz Pentium 4 !!!
  return theNumberOfReadBytes;
}

int EmuReader::eventNumber(){
  // This should work for both DDU and DCC as LV1 is in the CDF part.
  const MuEndDDUHeader * dduHeader = (const MuEndDDUHeader *) theBuffer;
  return dduHeader->lvl1num();
}
