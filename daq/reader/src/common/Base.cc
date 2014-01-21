#include "emu/daq/reader/Base.h"

#include "emu/daq/reader/DMBHeader.h"
#include "emu/daq/reader/DDUHeader.h"

int emu::daq::reader::Base::readNextEvent(){
  if ( theFormat == DDU || theFormat == DMB ) theNumberOfReadBytes = readDDU( theBuffer );
  else                                        theNumberOfReadBytes = readDCC( theBuffer );
  if ( theDebugMode ) std::cout << " theNumberOfReadBytes " << theNumberOfReadBytes << std::endl;
  if ( theNumberOfReadBytes<=7 ) {
    return theNumberOfReadBytes;
  }
  return theNumberOfReadBytes;
}

int emu::daq::reader::Base::eventNumber(){
  if ( theFormat == DMB ){
    const DMBHeader * dmbHeader = (const DMBHeader *) theBuffer;
    return int( dmbHeader->l1a() );
  }
  // This should work for both DDU and DCC as LV1 is in the CDF part.
  const DDUHeader * dduHeader = (const DDUHeader *) theBuffer;
  return dduHeader->lvl1num();
}
