#ifndef _emu_daq_writer_RawDataFile_h_
#define _emu_daq_writer_RawDataFile_h_

#include <string>
#include <sstream>
#include <fstream>
#include <ostream>
#include <iomanip>
#include "log4cplus/logger.h"

namespace emu { namespace daq { namespace writer {

  /// Class for writing binary data files.

  /// \par
  /// <b>1)</b> Name file as @c csc_RRRRRRRR_AAAAAAII_TTTTTT_FFF.raw
  /// where
  /// \li @c RRRRRRRR 8-digit decimal run number
  /// \li @c AAAAAA name of application producing the data
  /// \li @c II 2-digit instance number of application producing the data
  /// \li @c TTTTTT run type
  /// \li @c NNN 3-digit file counter in this run
  /// \par
  /// Example: \c csc_00012915_EmuRUI00_Monitor_000.raw
  /// \par
  /// If the run type is \c Debug, or the run number was not booked with the run info database, 
  /// the UTC date and time of the start of run are also included 
  /// as \c csc_RRRRRRRR_AAAAAAII_TTTTTT_FFF_YYMMDD_hhmmss_UTC.raw
  /// where
  /// \li @c YY last 2 digits of year
  /// \li @c MM month [1-12]
  /// \li @c DD day [1-31]
  /// \li @c hh hour
  /// \li @c mm minute
  /// \li @c ss second
  /// \par
  /// Example: \c  csc_00057005_EmuRUI01_Debug_000_070710_103403_UTC.raw
  /// \par 
  /// <b>2)</b> Write data to file as long as its size is below a given maximum.
  /// \par 
  /// <b>3)</b> Close file that has exceeded the maximum size, and write an empty marker file with
  /// the same file name but with the extension \c raw replaced with \c is_closed
  /// \par 
  /// <b>4)</b> Repeat from <b>1)</b>.
  using namespace std;

  class RawDataFile{

  private:
    unsigned int      maxFileSize_;	///< when exceeding this size [bytes], the file will be closed, and a new one opened
    string            pathToFile_;	///< direcory where the file is to be written
    string            host_;          ///< host name
    string            appName_;	///< name of application producing the data
    unsigned int      appInstance_;	///< instance of application producing the data
    string            appVersion_;    ///< version of application producing the data
    log4cplus::Logger logger_;	///< logger
    string            runStartTime_;	///< date and time of start of run
    string            runStopTime_;	///< date and time of end of run
    string            runType_;	///< run type
    unsigned int      runNumber_;	///< run number
    bool              isBookedRunNumber_; ///< whether or not this run number was booked with the database
    unsigned int      bytesInFileCounter_; ///< number of bytes written into this file so far
    unsigned int      eventsInFileCounter_; ///< number of events written into this file so far
    unsigned int      filesInRunCounter_; ///< number of files written in this run so far
    unsigned int      bytesInRunCounter_; ///< number of bytes written in this run so far
    unsigned int      eventsInRunCounter_; ///< number of events written in this run so far
    string            fileName_;	///< file name
    string            markerFileName_; ///< name of marker file [ <em>file_name_base</em>.<tt>is_closed</tt> ]
    string            metaFileName_; ///< name of metadata file [ <em>file_name_base</em>.<tt>meta</tt> ]
    fstream          *fs_;		///< output file stream

    /// Names the file to be written.
    void nameFile();

    /// Opens a binary file for output.
    void open();

    /// Closes file, and writes an empty <em>file_name_base</em>.<tt>is_closed</tt> marker file.
    void close();

    /// Writes an empty <em>file_name_base</em>.<tt>is_closed</tt> marker file.
    void writeMarkerFile();

    /// Writes a <em>file_name_base</em>.<tt>meta</tt> metadata file to make CASTOR happy.
    void writeMetaFile();

    /// Converts an unsigned integer to std:string
    string toString( unsigned int i );

    /// Converts time given as string to Unix time

    ///
    /// @param YYMMDD_hhmmss_UTC Time string. Must be in this format.
    ///
    /// @return Unix time. 0 if conversion fails.
    ///
    time_t toUnixTime( const std::string YYMMDD_hhmmss_UTC );

    /// Names the stream, which will appear as a subdirectory name in CASTOR.

    ///
    /// @return Stream name as sssssYYYY, where ssss is
    ///     <b>Local</b> for event fragments (DDU data from RUIs)
    ///     <b>Built</b> for built local Emu events (from FUs)
    ///     <b>Calib</b> for calibration data (DDU data from RUIs)
    /// and YYYY is the year (not appended if runStartTime_ is not in YYMMDD_hhmmss_UTC format).
    ///
    std::string nameStream();

  public:

    /// constructor

    ///
    /// @param maxFileSize maximum file size [bytes]
    /// @param pathToFile direcory where the file is to be written
    /// @param app name of application producing the data
    /// @param logger logger
    ///
    RawDataFile(const unsigned int maxFileSize, 
		const string pathToFile, 
		const string host, 
		const string appName, 
		const unsigned int appInstance, 
		const string appVersion, 
		const log4cplus::Logger* logger);

    /// destructor
    ~RawDataFile();

    /// To be called when a new run starts.

    /// Resets counters and opens a file.
    ///
    /// @param runNumber run number
    /// @param isBookedRunNumber whether or not this run number was booked with the database
    /// @param runStartTime UTC date and time of the start of run
    /// @param runType run type
    ///
    void startNewRun( const int runNumber, 
		      const bool isBookedRunNumber, 
		      const string runStartTime, 
		      const string runType );

    /// Writes data to file.

    /// @param buf data buffer
    /// @param nBytes number of bytes to be written
    ///
    void writeData( const char* buf, const int nBytes );

    /// To be called when a new event starts.
    void startNewEvent();

    /// To be called when the run ends.
    void endRun();

    /// To be called when the run ends.

    ///
    /// @param runStopTime UTC date and time of the end of run
    void endRun( const string runStopTime );

    /// Gets file size in bytes.

    ///
    /// @return file size in bytes, or -1 if file couldn't be obtained
    ///
    int getFileSize();

    /// Removes file.
    void removeFile();

    /// accessor of run number

    /// @return run number
    ///
    unsigned int getRunNumber(){ return runNumber_; }
  };

}}} // namespace emu::daq::writer

#endif //#ifndef _emu_daq_writer_RawDataFile_h_
