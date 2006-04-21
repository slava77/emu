#ifndef _FileWriter_h_
#define _FileWriter_h_

#include <string>
#include <sstream>
#include <fstream>
#include "log4cplus/logger.h"
#include <stdio.h>

class FileWriter{

private:
  unsigned int  maxFileSize_; // bytes
  string        pathToFile_;
  string        filePrefix_;
  Logger        logger_;
  unsigned int  runNumber_;
  unsigned int  bytesInFileCounter_;
  unsigned int  eventsInFileCounter_;
  unsigned int  filesInRunCounter_;
  unsigned int  bytesInRunCounter_;
  unsigned int  eventsInRunCounter_;
  string        fileName_;
  std::fstream *fs_;

  string nameFile(){
    ostringstream fileNameStream;
    fileNameStream << pathToFile_ << "/" << filePrefix_ <<"_Run";
    fileNameStream.fill('0');
    fileNameStream.width(5);
    fileNameStream << runNumber_ << "_";
    fileNameStream.width(3);
    fileNameStream << filesInRunCounter_ << ".bin";
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
  }


public:
  FileWriter(const unsigned int maxFileSize, const string pathToFile, const string prefix, const Logger* logger)
    :maxFileSize_         (maxFileSize)
    ,pathToFile_          (pathToFile)
    ,filePrefix_          (prefix)
    ,logger_              (*logger)
    ,runNumber_           (0)
    ,bytesInFileCounter_  (0)
    ,eventsInFileCounter_ (0)
    ,filesInRunCounter_   (0)
    ,bytesInRunCounter_   (0)
    ,eventsInRunCounter_  (0)
  {
    fs_ = new std::fstream();
    LOG4CPLUS_DEBUG( logger_, "New file writer has been created." );
  }

  ~FileWriter(){ delete fs_; }

  void startNewRun( const int runNumber ){
    runNumber_          = runNumber;
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
      LOG4CPLUS_INFO( logger_, "Deleted file " << fileName_ );
    }
  }

  unsigned int getRunNumber(){ return runNumber_; }
};


#endif //#ifndef _FileWriter_h_
