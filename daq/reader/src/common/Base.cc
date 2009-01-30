#include "emu/daq/reader/Base.h"

int emu::daq::reader::Base::readNextEvent(){
  if ( theFormat == DDU ) theNumberOfReadBytes = readDDU( theBuffer );
  else                    theNumberOfReadBytes = readDCC( theBuffer );
  if ( theDebugMode ) std::cout << " theNumberOfReadBytes " << theNumberOfReadBytes << std::endl;
  if ( theNumberOfReadBytes<=7 ) {
    return theNumberOfReadBytes;
  }
  return theNumberOfReadBytes;
}

int emu::daq::reader::Base::eventNumber(){
  // This should work for both DDU and DCC as LV1 is in the CDF part.
  const DDUHeader * dduHeader = (const DDUHeader *) theBuffer;
  return dduHeader->lvl1num();
}
