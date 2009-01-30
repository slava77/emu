#ifndef __EMU_DAQ_READER_RAWDATAFILE_H__
#define __EMU_DAQ_READER_RAWDATAFILE_H__

#include "emu/daq/reader/Base.h"

namespace emu { namespace daq { namespace reader {

  /// An EmuReader reading from raw data file.

  ///
  /// 
  ///
  class RawDataFile : public emu::daq::reader::Base
  {
  public:

    /// constructor

    /// @param filename file to read from
    /// @param format format [ EmuReader::DDU | EmuReader::DCC ]
    /// @param debug if \c TRUE , prints debug messages to stdout
    ///
    RawDataFile( std::string filename, int format, bool debug=false );

    /// destructor
    ~RawDataFile();


    /// Opens file.
    void            open( std::string filename );

    /// dummy as file reader won't need resetting and enabling
    virtual void    resetAndEnable(){}

    /// Closes file.
    void            close();

  protected:
    int             theFileDescriptor; ///< file descriptor

    /// Reads DDU data.

    /// @param buf pointer to be set to point to the data
    ///
    /// @return number of bytes read
    ///
    int             readDDU( unsigned short*& buf );

    /// Reads DCC data (<b>not implemented</b>).

    /// @param buf pointer to be set to point to the data
    ///
    /// @return number of bytes read
    ///
    int             readDCC( unsigned short*& buf );

    /// Reads from file.

    /// @param buf pointer to be set to point to the data
    ///
    /// @return number of bytes read
    ///
    int 		  read( unsigned short* &buf);

    //KK
  private:
    unsigned short raw_event[200000]; ///< buffer

    unsigned long long word_0, word_1, word_2; ///< To remember some history
    unsigned long long file_buffer[4000];      ///< Read data block for efficiency

    unsigned long long *end, *file_buffer_end; ///< where it stopped last time and where its end is

  public:
    /// error types
    enum {Header=1,Trailer=2,DDUoversize=4,FFFF=8,Unknown=16,EndOfStream=32};

    /// error types, Andrey Korytov's notations
    enum {Type1=Header|Trailer, Type2=Header, Type3=Header|DDUoversize, Type4=Trailer, Type5=Unknown, Type6=Unknown|DDUoversize, Type7=FFFF}; 

    unsigned int eventStatus, selectCriteria, acceptCriteria, rejectCriteria; ///< status and criteria
    //KKend
  };
}}} // namespace emu::daq::reader

#endif  // ifndef __EMU_DAQ_READER_RAWDATAFILE_H__
