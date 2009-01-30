#ifndef __EMU_DAQ_READER_BASE_H__
#define __EMU_DAQ_READER_BASE_H__

#include "emu/daq/reader/DDUHeader.h"

#include <iostream>
#include <string>
#include <fstream>

namespace emu { namespace daq { namespace reader {

/// Base class for Emu data readout.

///
/// 
///
class Base
{

protected:
  std::string     theName;       ///< name of this object (file name or board number)
  int             theFormat;     ///< DDU or DCC
  bool            theDebugMode;  ///< if \c TRUE , prints debug messages to stdout
  bool            theDeviceIsResetAndEnabled; ///< to know whether it is already reset and enabled
  unsigned short  theErrorFlag;  ///< for DQM
  std::string     theLogMessage; ///< in case anybody is interested...
  unsigned short* theBuffer;     ///< buffer containing event data
  int             theDataLength; ///< in bytes; size of the actual data, without filler words
  int             theNumberOfReadBytes; ///< what we read in; may contain data and filler words as well!

public:
  /// for the two possible formats
  enum { DDU, DCC };

  /// constructor
  ///
  /// @param name name of this reader 
  /// @param format data format
  /// @param debug if \c TRUE , prints debug messages to stdout
  ///
  Base( std::string name, int format, bool debug )
    : theName      ( name   ),
      theFormat    ( format ),
      theDebugMode ( debug  ),
      theDeviceIsResetAndEnabled( false )
  {}

  /// destructor
  virtual ~Base(){}

  /// Reads in the next event, if any.
  int          readNextEvent();

  /// accessor of L1A number
  int          eventNumber();

  /// Has this device been reset and enabled?

  /// @return \c TRUE if reset and enabled
  ///
  bool         isResetAndEnabled(){ return theDeviceIsResetAndEnabled; }

  /// Gets pointer to start of data.

  /// @return pointer to data
  ///
  char*        data()            { return (char*) theBuffer; }

  /// Gets data length in bytes.

  /// @return data length in bytes
  ///
  int          dataLength()      { return theDataLength;     } // in bytes

  /// Sets debug mode.

  /// @param d \c TRUE to switch debug mode, \c FALSE to switch back
  ///
  void         setDebug( bool d ){ theDebugMode = d;         }

  /// accessor of the name of this reader
  std::string  getName()         { return theName;           }

  /// accessor of the error flag
  unsigned short getErrorFlag()  { return theErrorFlag;      }

  /// accessor of the log message
  std::string  getLogMessage()   { return theLogMessage;     }

  /// not documented here
  virtual void resetAndEnable()=0;
  /// not documented here
  virtual int  readDDU( unsigned short*& buf )=0;
  /// not documented here
  virtual int  readDCC( unsigned short*& buf )=0;
};

}}} // namespace emu::daq::reader

#endif  // ifndef __EMU_DAQ_READER_BASE_H__
