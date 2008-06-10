#ifndef _EmuFileWriter_h_
#define _EmuFileWriter_h_

#include <string>
#include <sstream>
#include <fstream>
#include "log4cplus/logger.h"
#include <stdio.h>

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

class EmuFileWriter{


private:
  unsigned int  maxFileSize_;	///< when exceeding this size [bytes], the file will be closed, and a new one opened
  string        pathToFile_;	///< direcory where the file is to be written
  string        host_;          ///< host name
  string        application_;	///< name of application producing the data
  string        appVersion_;    ///< version of application producing the data
  Logger        logger_;	///< logger
  string        runStartTime_;	///< date and time of start of run
  string        runStopTime_;	///< date and time of end of run
  string        runType_;	///< run type
  unsigned int  runNumber_;	///< run number
  bool          isBookedRunNumber_; ///< whether or not this run number was booked with the database
  unsigned int  bytesInFileCounter_; ///< number of bytes written into this file so far
  unsigned int  eventsInFileCounter_; ///< number of events written into this file so far
  unsigned int  filesInRunCounter_; ///< number of files written in this run so far
  unsigned int  bytesInRunCounter_; ///< number of bytes written in this run so far
  unsigned int  eventsInRunCounter_; ///< number of events written in this run so far
  string        fileName_;	///< file name
  string        markerFileName_; ///< name of marker file [ <em>file_name_base</em>.<tt>is_closed</tt> ]
  string        metaFileName_; ///< name of metadata file [ <em>file_name_base</em>.<tt>meta</tt> ]
  fstream      *fs_;		///< output file stream

  /// Names the file to be written.
  void nameFile(){
    ostringstream fileNameStream;
    fileNameStream << pathToFile_        << "/csc_";
    fileNameStream.fill('0');
    fileNameStream.width(8);
    fileNameStream << runNumber_         << "_";
    fileNameStream << application_       << "_";
    fileNameStream << runType_           << "_";
    fileNameStream.width(3);
    fileNameStream << filesInRunCounter_;
    // Insert start time to make sure the name will be unique if it's not a booked run number or it's a debug run:
    if ( !isBookedRunNumber_ || runType_ == "Debug" ) fileNameStream << "_" << runStartTime_;
    fileName_       = fileNameStream.str() + ".raw";
    markerFileName_ = fileNameStream.str() + ".is_closed";
    metaFileName_   = fileNameStream.str() + ".meta";
  }

  /// Opens a binary file for output.
  void open(){
    nameFile();
    fs_->open(fileName_.c_str(), ios::out | ios::binary);
    if ( fs_->is_open() ){
      bytesInFileCounter_  = 0;
      eventsInFileCounter_ = 0;
      filesInRunCounter_++;
      LOG4CPLUS_INFO( logger_, "Opened " << fileName_ );
    }
    else{
      stringstream oss;
      oss << "Could not open " << fileName_ << " for writing.";
      throw oss.str();
    }
  }

  /// Closes file, and writes an empty <em>file_name_base</em>.<tt>is_closed</tt> marker file.
  void close(){ 
    fs_->close();
    LOG4CPLUS_INFO( logger_, "Wrote "                           << 
		    eventsInFileCounter_ << " events ("         << 
		    bytesInFileCounter_  << " bytes) to "       << 
		    fileName_ 	         << "; so far "         << 
		    eventsInRunCounter_  << " events ("         << 
		    bytesInRunCounter_   << " bytes) in run "   << 
		    runNumber_ );
    // Let the world know it's closed (if it indeed is...)
    if ( fs_->is_open() ){
      LOG4CPLUS_ERROR( logger_, fileName_ << " could not be closed.");
    }
    else{
      writeMarkerFile();
      writeMetaFile();
    }
  }

  /// Writes an empty <em>file_name_base</em>.<tt>is_closed</tt> marker file.
  void writeMarkerFile(){
    if ( fs_->is_open() ) fs_->close(); // just in case...
    fs_->open( markerFileName_.c_str(), ios::out );
    fs_->close();
  }

  /// Writes a <em>file_name_base</em>.<tt>meta</tt> metadata file to make CASTOR happy.
  void writeMetaFile(){
    if ( fs_->is_open() ) fs_->close(); // just in case...
    fs_->open( metaFileName_.c_str(), ios::out );

    *fs_ << "RUNNUMBER"   << " " << runNumber_           << endl;
    *fs_ << "LUMISECTION" << " " << "0"                  << endl;
    *fs_ << "NEVENTS"     << " " << eventsInFileCounter_ << endl;
    *fs_ << "APP_NAME"    << " " << application_         << endl;
    *fs_ << "APP_VERSION" << " " << appVersion_          << endl;
    *fs_ << "START_TIME"  << " " << toUnixTime( runStartTime_ ) << endl; // may be 0
    *fs_ << "STOP_TIME"   << " " << toUnixTime( runStopTime_  ) << endl; // may be 0
    *fs_ << "CHECKSUM"    << " " << "0"                  << endl; // may be 0
    *fs_ << "SAFETY"      << " " << "0"                  << endl; // must be 0
    *fs_ << "DATASET"     << " " << runType_             << endl;
    *fs_ << "STREAM"      << " " << "EDM"                << endl;
    *fs_ << "FILENAME"    << " " << fileName_            << endl;
    *fs_ << "PATHNAME"    << " " << pathToFile_          << endl;
    *fs_ << "HOSTNAME"    << " " << host_                << endl;
    *fs_ << "FILESIZE"    << " " << bytesInFileCounter_  << endl;

    fs_->close();
  }

  /// Converts an unsigned integer to std:string
  string toString( unsigned int i ){
    ostringstream oss;
    oss << i;
    return oss.str();
  }

  /// Converts time given as string to Unix time

  ///
  /// @param YYMMDD_hhmmss_ZZZ Time string. Must be in this format.
  ///
  /// @return Unix time. 0 if conversion fails.
  ///
  time_t toUnixTime( const std::string YYMMDD_hhmmss_ZZZ ){
    if ( YYMMDD_hhmmss_ZZZ.size() < 17 ) return time_t(0);

    struct tm stm;
    std::stringstream ss;

    int localHour = 0;
    int UTCHour   = 0;
    if ( YYMMDD_hhmmss_ZZZ.substr(14,3) == "UTC" ){
      // Figure out the difference between local time and UTC time 
      // in order to transform the given time to local time
      // as mktime will assume its argument is local time.
      time_t timeNow = time( NULL );
      // s will point to a statically allocated variable shared by localtime and gmtime,
      // no point in declaring it twice.    
      struct tm *s = localtime( &timeNow );
      localHour = s->tm_hour;
      s = gmtime( &timeNow );
      UTCHour = s->tm_hour;
    }

    ss << YYMMDD_hhmmss_ZZZ.substr( 0,2); ss >> stm.tm_year; ss.clear(); stm.tm_year += 100;
    ss << YYMMDD_hhmmss_ZZZ.substr( 2,2); ss >> stm.tm_mon;  ss.clear(); stm.tm_mon  -= 1;
    ss << YYMMDD_hhmmss_ZZZ.substr( 4,2); ss >> stm.tm_mday; ss.clear();
    ss << YYMMDD_hhmmss_ZZZ.substr( 7,2); ss >> stm.tm_hour; ss.clear(); stm.tm_hour += localHour - UTCHour;
    ss << YYMMDD_hhmmss_ZZZ.substr( 9,2); ss >> stm.tm_min;  ss.clear();
    ss << YYMMDD_hhmmss_ZZZ.substr(11,2); ss >> stm.tm_sec;  ss.clear();

    time_t unixTime = mktime( &stm );

    return ( unixTime < 0 ? time_t(0) : unixTime );
  }


public:

  /// constructor

  ///
  /// @param maxFileSize maximum file size [bytes]
  /// @param pathToFile direcory where the file is to be written
  /// @param app name of application producing the data
  /// @param logger logger
  ///
  EmuFileWriter(const unsigned int maxFileSize, const string pathToFile, const string host, const string app, const string version, const Logger* logger)
    :maxFileSize_         (maxFileSize)
    ,pathToFile_          (pathToFile)
    ,host_                (host)
    ,application_         (app)
    ,appVersion_          (version)
    ,logger_              (*logger)
    ,runStartTime_        ("")
    ,runStopTime_         ("")
    ,runType_             ("")
    ,runNumber_           (0)
    ,isBookedRunNumber_   (false)
    ,bytesInFileCounter_  (0)
    ,eventsInFileCounter_ (0)
    ,filesInRunCounter_   (0)
    ,bytesInRunCounter_   (0)
    ,eventsInRunCounter_  (0)
  {
    fs_ = new std::fstream();
    LOG4CPLUS_DEBUG( logger_, "New file writer has been created." );
  }

  /// destructor
  ~EmuFileWriter(){ delete fs_; }

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
		    const string runType ){
    runNumber_          = runNumber;
    isBookedRunNumber_  = isBookedRunNumber;
    // Run number 0 cannot possibly have been booked:
    if ( runNumber_ == 0 ) isBookedRunNumber_ = false;
    runStartTime_       = runStartTime;
    runType_            = runType;
    bytesInRunCounter_  = 0;
    eventsInRunCounter_ = 0;
    open();
  }

  /// Writes data to file.

  /// @param buf data buffer
  /// @param nBytes number of bytes to be written
  ///
  void writeData( const char* buf, const int nBytes ){ 
    fs_->write(buf, nBytes);
    if ( fs_->fail() ){
//       LOG4CPLUS_ERROR( logger_, "Error writing to " << fileName_ );
//       fs_->clear();
      stringstream oss;
      oss << "Error writing to " << fileName_;
      throw oss.str();
    }
    else{
      bytesInFileCounter_ += nBytes;
      bytesInRunCounter_  += nBytes;
    }
  }

  /// To be called when a new event starts.
  void startNewEvent(){
    eventsInFileCounter_++;
    eventsInRunCounter_++;
    if ( bytesInFileCounter_ > maxFileSize_ ){
      close();
      open();
    }
  }

  /// To be called when the run ends.
  void endRun(){ 
    close();
    LOG4CPLUS_INFO( logger_, 
		    "End of run "       <<
		    runNumber_          << ". Wrote "  <<
		    eventsInRunCounter_ << " events (" <<
		    bytesInRunCounter_  <<" bytes) in "<<
		    filesInRunCounter_  <<" file"      <<
		    (filesInRunCounter_==1?".":"s.")      );
  }

  /// To be called when the run ends.

  ///
  /// @param runStopTime UTC date and time of the end of run
  void endRun( const string runStopTime ){ 
    runStopTime_ = runStopTime;
    close();
    LOG4CPLUS_INFO( logger_, 
		    "End of run "       <<
		    runNumber_          << ". Wrote "  <<
		    eventsInRunCounter_ << " events (" <<
		    bytesInRunCounter_  <<" bytes) in "<<
		    filesInRunCounter_  <<" file"      <<
		    (filesInRunCounter_==1?".":"s.")      );
  }

  /// Gets file size in bytes.

  ///
  /// @return file size in bytes, or -1 if file couldn't be obtained
  ///
  int getFileSize(){
    // Returns file size in bytes, or -1 if file couldn't be obtained.
    if ( fs_->is_open() ){
      // If it's still open, synchronize buffer, then get size
      int size = (int) ( fs_->flush().seekp(0,ios::end).tellp() );
      if ( fs_->fail() ) return int(-1);
      else               return size;
    }
    else{
      fs_->open( fileName_.c_str(), ios::in | ios::binary );
      if ( fs_->fail() ){ 
	return int(-1);
      }
      else{
	int size = (int) ( fs_->seekg(0,ios::end).tellg() );
	fs_->close();
	return size;
      }
    }
    return int(-1);
  }

  /// Removes file.
  void removeFile(){
    if ( fs_->is_open() ) fs_->close();
    if ( ::remove( fileName_.c_str() ) == -1 ){
      LOG4CPLUS_WARN( logger_, "Error deleting file " << fileName_ );
    }
    else{
      LOG4CPLUS_INFO( logger_, "Deleted empty file " << fileName_ );
      // Delete the status file too
      ::remove( markerFileName_.c_str() );
    }
  }

  /// accessor of run number

  /// @return run number
  ///
  unsigned int getRunNumber(){ return runNumber_; }
};


#endif //#ifndef _EmuFileWriter_h_
