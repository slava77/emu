#ifndef __EMU_READER_H__
#define __EMU_READER_H__

#include "Muon/METBRawFormat/interface/MuEndDDUHeader.h"

#include <iostream>
#include <string>
#include <fstream>

class EmuReader
{

protected:
  std::string     theName;       // name of this object (file name or board number)
  int             theFormat;     // DDU or DCC
  bool            theDebugMode;
  unsigned short* theBuffer;     // buffer containing event data
  int             theDataLength; // in bytes
  unsigned int    theErrorCount; // counts consequitive read errors

public:
  enum { DDU, DCC };

  EmuReader( std::string name, int format, bool debug )
    : theName      ( name   ),
      theFormat    ( format ),
      theDebugMode ( debug  ),
      theErrorCount( 0      )
  {}

  virtual ~EmuReader(){}

  //   bool readNextEvent() {
  //     // get this CPU hog out of here:     unsigned short ** buf2 = new unsigned short* ;
  //     unsigned short  *buf3;
  //     unsigned short **buf2 = &buf3;
  //     if ( theFormat == DDU ) theDataLength = readDDU(buf2);
  //     else                    theDataLength = readDCC(buf2);
  //     if ( theDebugMode ) std::cout << " theDataLength " << theDataLength << std::endl;
  //     if ( theDataLength<=7 ) {
  //       theErrorCount++;      
  //       return false;
  //     }
  //     theErrorCount = 0;
  // //     unsigned short * buf=(unsigned short *)*buf2;
  // //     theBuffer = buf; 
  //     theBuffer = (unsigned short *)*buf2;
  //     // get this CPU hog out of here:    delete buf2;
  //     // new and delete: ~400s/10^9 on a 2.4GHz Pentium 4 !!!
  //     return true;
  //   }

  bool         readNextEvent();
  int          eventNumber();

  char*        data()            { return (char*) theBuffer; }
  int          dataLength()      { return theDataLength;     } // in bytes
  void         setDebug( bool d ){ theDebugMode = d;         }
  unsigned int getErrorCount()   { return theErrorCount;     }
  std::string  getName()         { return theName;           }

  virtual int  readDDU( unsigned short*& buf )=0;
  virtual int  readDCC( unsigned short*& buf )=0;
};

#endif  // ifndef __EMU_READER_H__
