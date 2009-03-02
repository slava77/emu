#include <string>
#include <sstream>
#include <fstream>
#include <ostream>
#include <iomanip>
#include "log4cplus/logger.h"

#include "emu/daq/writer/RawDataFile.h"

using namespace std;

void emu::daq::writer::RawDataFile::nameFile(){
  /// Names the file to be written.
  ostringstream fileNameStream;
  fileNameStream << pathToFile_        << "/csc_";
  fileNameStream << setfill('0') << setw(8);
  fileNameStream << runNumber_         << "_";
  fileNameStream << appName_ ;
  fileNameStream << setfill('0') << setw(2);
  fileNameStream << appInstance_       << "_";
  fileNameStream << runType_           << "_";
  fileNameStream.width(3);
  fileNameStream << filesInRunCounter_;
  // Insert start time to make sure the name will be unique if it's not a booked run number or it's a debug run:
  if ( !isBookedRunNumber_ || runType_ == "Debug" ) fileNameStream << "_" << runStartTime_;
  fileName_       = fileNameStream.str() + ".raw";
  markerFileName_ = fileNameStream.str() + ".is_closed";
  metaFileName_   = fileNameStream.str() + ".meta";
}

void emu::daq::writer::RawDataFile::open(){
  /// Opens a binary file for output.
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

void emu::daq::writer::RawDataFile::close(){ 
  /// Closes file, and writes an empty <em>file_name_base</em>.<tt>is_closed</tt> marker file.
  fs_->close();
  LOG4CPLUS_INFO( logger_, "Wrote "                           << 
		  eventsInFileCounter_ << " events ("         << 
		  bytesInFileCounter_  << " bytes) to "       << 
		  fileName_ 	       << "; so far "         << 
		  eventsInRunCounter_  << " events ("         << 
		  bytesInRunCounter_   << " bytes) in run "   << 
		  runNumber_ );
  // Let the world know it's closed (if it indeed is...)
  if ( fs_->is_open() ){
    LOG4CPLUS_ERROR( logger_, fileName_ << " could not be closed.");
  }
  else{
    // .meta file makes writeMarkerFile(); redundant
    writeMetaFile();
  }
}

void emu::daq::writer::RawDataFile::writeMarkerFile(){
  /// Writes an empty <em>file_name_base</em>.<tt>is_closed</tt> marker file.
  if ( fs_->is_open() ) fs_->close(); // just in case...
  fs_->open( markerFileName_.c_str(), ios::out );
  fs_->close();
}

void emu::daq::writer::RawDataFile::writeMetaFile(){
  /// Writes a <em>file_name_base</em>.<tt>meta</tt> metadata file to make CASTOR happy.
  if ( fs_->is_open() ) fs_->close(); // just in case...
  fs_->open( metaFileName_.c_str(), ios::out );

  *fs_ << "runnumber"   << " " << runNumber_           << endl;
  *fs_ << "lumisection" << " " << "0"                  << endl;
  *fs_ << "nevents"     << " " << eventsInFileCounter_ << endl;
  *fs_ << "appname"     << " " << appName_             << endl;
  *fs_ << "instance"    << " " << appInstance_         << endl;
  *fs_ << "appversion"  << " " << appVersion_          << endl;
  *fs_ << "start_time"  << " " << toUnixTime( runStartTime_ ) << endl; // may be 0
  *fs_ << "stop_time"   << " " << toUnixTime( runStopTime_  ) << endl; // may be 0
  *fs_ << "setuplabel"  << " " << "CSC"                << endl;
  *fs_ << "type"        << " " << "edm"                << endl;
  *fs_ << "stream"      << " " << nameStream()         << endl;
  *fs_ << "filename"    << " " << fileName_            << endl;
  *fs_ << "pathname"    << " " << pathToFile_          << endl;
  *fs_ << "hostname"    << " " << host_                << endl;
  *fs_ << "filesize"    << " " << bytesInFileCounter_  << endl;

  fs_->close();
}

string emu::daq::writer::RawDataFile::toString( unsigned int i ){
  /// Converts an unsigned integer to std:string
  ostringstream oss;
  oss << i;
  return oss.str();
}


time_t emu::daq::writer::RawDataFile::toUnixTime( const std::string YYMMDD_hhmmss_UTC ){
  /// Converts time given as string to Unix time
  if ( YYMMDD_hhmmss_UTC.size() < 17 ) return time_t(0);

  struct tm stm;
  std::stringstream ss;

  ss << YYMMDD_hhmmss_UTC.substr( 0,2); ss >> stm.tm_year; ss.clear(); stm.tm_year += 100;
  ss << YYMMDD_hhmmss_UTC.substr( 2,2); ss >> stm.tm_mon;  ss.clear(); stm.tm_mon  -= 1;
  ss << YYMMDD_hhmmss_UTC.substr( 4,2); ss >> stm.tm_mday; ss.clear();
  ss << YYMMDD_hhmmss_UTC.substr( 7,2); ss >> stm.tm_hour; ss.clear();
  ss << YYMMDD_hhmmss_UTC.substr( 9,2); ss >> stm.tm_min;  ss.clear();
  ss << YYMMDD_hhmmss_UTC.substr(11,2); ss >> stm.tm_sec;  ss.clear();

  time_t unixTime = mktime( &stm );

  return ( unixTime < 0 ? time_t(0) : unixTime );
}

std::string emu::daq::writer::RawDataFile::nameStream(){
  /// Names the stream, which will appear as a subdirectory name in CASTOR.
  string streamName;
  if ( runType_.substr(0,5) == "Calib" ) streamName = "Calib";
  else if ( appName_.substr(0,6) == "EmuRUI" ) streamName = "Local";
  else if ( appName_.substr(0,5) == "EmuFU"  ) streamName = "Built";

  if ( runStartTime_.size() < 17 ) return streamName;
  else return streamName+string("20")+runStartTime_.substr(0,2);
}

emu::daq::writer::RawDataFile::RawDataFile(const unsigned int maxFileSize, 
					   const string pathToFile, 
					   const string host, 
					   const string appName, 
					   const unsigned int appInstance, 
					   const string appVersion, 
					   const log4cplus::Logger* logger)
  :maxFileSize_         (maxFileSize)
  ,pathToFile_          (pathToFile)
  ,host_                (host)
  ,appName_             (appName)
  ,appInstance_         (appInstance)
  ,appVersion_          (appVersion)
  ,logger_              (*logger)
  ,runStartTime_        ("")
  ,runStopTime_         ("")
  ,runType_             ("")
  ,runNumber_           (1)
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

emu::daq::writer::RawDataFile::~RawDataFile(){ delete fs_; }

void emu::daq::writer::RawDataFile::startNewRun( const int runNumber, 
						 const bool isBookedRunNumber, 
						 const string runStartTime, 
						 const string runType ){
  /// To be called when a new run starts.
  runNumber_          = runNumber;
  isBookedRunNumber_  = isBookedRunNumber;
  // Run number 0 or 1 cannot possibly have been booked:
  if ( runNumber_ == 0 || runNumber_ == 1 ) isBookedRunNumber_ = false;
  runStartTime_       = runStartTime;
  runType_            = runType;
  bytesInRunCounter_  = 0;
  eventsInRunCounter_ = 0;
  open();
}

void emu::daq::writer::RawDataFile::writeData( const char* buf, const int nBytes ){ 
  /// Writes data to file.
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

void emu::daq::writer::RawDataFile::startNewEvent(){
  /// To be called when a new event starts.
  eventsInFileCounter_++;
  eventsInRunCounter_++;
  if ( bytesInFileCounter_ > maxFileSize_ ){
    close();
    open();
  }
}

void emu::daq::writer::RawDataFile::endRun(){ 
  /// To be called when the run ends.
  close();
  LOG4CPLUS_INFO( logger_, 
		  "End of run "       <<
		  runNumber_          << ". Wrote "  <<
		  eventsInRunCounter_ << " events (" <<
		  bytesInRunCounter_  <<" bytes) in "<<
		  filesInRunCounter_  <<" file"      <<
		  (filesInRunCounter_==1?".":"s.")      );
}


void emu::daq::writer::RawDataFile::endRun( const string runStopTime ){ 
  /// To be called when the run ends.
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

int emu::daq::writer::RawDataFile::getFileSize(){
  /// Gets file size in bytes.
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

void emu::daq::writer::RawDataFile::removeFile(){
  /// Removes file.
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

