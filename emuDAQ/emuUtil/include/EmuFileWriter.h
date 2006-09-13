#ifndef _EmuFileWriter_h_
#define _EmuFileWriter_h_

#include <string>
#include <sstream>
#include <fstream>
#include "log4cplus/logger.h"
#include <stdio.h>

class EmuFileWriter{

private:
  unsigned int  maxFileSize_; // bytes
  string        pathToFile_;
//   string        filePrefix_;
  string        application_;
  Logger        logger_;
  string        runStartTime_;
  string        runType_;
  unsigned int  runNumber_;
  bool          isBookedRunNumber_; // whether or not this run number was booked in the database
  unsigned int  bytesInFileCounter_;
  unsigned int  eventsInFileCounter_;
  unsigned int  filesInRunCounter_;
  unsigned int  bytesInRunCounter_;
  unsigned int  eventsInRunCounter_;
  string        fileName_;
  std::fstream *fs_;

//   string nameFile(){
//     ostringstream fileNameStream;
//     fileNameStream << pathToFile_        << "/";
//     fileNameStream << runStartTime_      << "_";
//     fileNameStream.fill('0');
//     fileNameStream.width(6);
//     fileNameStream << runNumber_         << "_";
//     fileNameStream.width(3);
//     fileNameStream << filesInRunCounter_ << "_";
//     fileNameStream << application_       << "_";
//     fileNameStream << runType_           << ".bin";
//     return fileNameStream.str();
//   }

  string nameFile(){
    ostringstream fileNameStream;
    fileNameStream << pathToFile_        << "/csc_";
    fileNameStream.fill('0');
    fileNameStream.width(8);
    fileNameStream << runNumber_         << "_";
    fileNameStream << application_       << "_";
    fileNameStream << runType_           << "_";
    fileNameStream.width(3);
    fileNameStream << filesInRunCounter_;
    // Insert start time to make sure the name will be unique if it's not a booked run number:
    if ( !isBookedRunNumber_ ) fileNameStream << "_" << runStartTime_;
    fileNameStream << ".raw";
    return fileNameStream.str();
  }

  void open(){
    fileName_ = nameFile();
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


  void close(){ 
    fs_->close();
    LOG4CPLUS_INFO( logger_, "Wrote "                           << 
		    eventsInFileCounter_ << " events ("         << 
		    bytesInFileCounter_  << " bytes) to "       << 
		    fileName_ 	   << "; so far "               << 
		    eventsInRunCounter_  << " events ("         << 
		    bytesInRunCounter_   << " bytes) in run "   << 
		    runNumber_ );
    // Let the world know it's closed (if it indeed is...)
    if ( fs_->is_open() ){
      LOG4CPLUS_ERROR( logger_, fileName_ << " could not be closed.");
    }
    else{
      fs_->open((fileName_+"_is_closed").c_str(), ios::out );
      fs_->close();
    }
  }


public:
  EmuFileWriter(const unsigned int maxFileSize, const string pathToFile, const string app, const Logger* logger)
    :maxFileSize_         (maxFileSize)
    ,pathToFile_          (pathToFile)
    ,application_         (app)
    ,logger_              (*logger)
    ,runStartTime_        ("")
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

  ~EmuFileWriter(){ delete fs_; }

  void startNewRun( const int runNumber, 
		    const bool isBookedRunNumber, 
		    const string runStartTime, 
		    const string runType ){
    runNumber_          = runNumber;
    isBookedRunNumber_  = isBookedRunNumber;
    runStartTime_       = runStartTime;
    runType_            = runType;
    bytesInRunCounter_  = 0;
    eventsInRunCounter_ = 0;
    open();
  }

  void writeData( const char* buf, const int nBytes ){ 
    fs_->write(buf, nBytes);
    if ( fs_->fail() ){
      LOG4CPLUS_ERROR( logger_, "Error writing to " << fileName_ );
      fs_->clear();
    }
    else{
      bytesInFileCounter_ += nBytes;
      bytesInRunCounter_  += nBytes;
    }
  }

  void startNewEvent(){
    eventsInFileCounter_++;
    eventsInRunCounter_++;
    if ( bytesInFileCounter_ > maxFileSize_ ){
      close();
      open();
    }
  }

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

  int getFileSize(){
    // Returns file size in bytes, or -1 if file size couldn't be obtained.
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

  void removeFile(){
    if ( fs_->is_open() ) fs_->close();
    if ( ::remove( fileName_.c_str() ) == -1 ){
      LOG4CPLUS_WARN( logger_, "Error deleting file " << fileName_ );
    }
    else{
      LOG4CPLUS_INFO( logger_, "Deleted empty file " << fileName_ );
      // Delete the status file too
      ::remove( (fileName_+"_is_closed").c_str() );
    }
  }

  unsigned int getRunNumber(){ return runNumber_; }
};


#endif //#ifndef _EmuFileWriter_h_
