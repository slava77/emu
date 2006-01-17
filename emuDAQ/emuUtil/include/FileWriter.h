#ifndef _FileWriter_h_
#define _FileWriter_h_

#include <string>
#include <sstream>
#include <fstream>
#include "log4cplus/logger.h"


  class FileWriter{

  private:
    unsigned long  maxFileSize_; // bytes
    string         pathToFile_;
    string         filePrefix_;
    Logger         logger_;
    unsigned long  runNumber_;
    unsigned long  bytesInFileCounter_;
    unsigned long  eventsInFileCounter_;
    unsigned long  filesInRunCounter_;
    unsigned long  bytesInRunCounter_;
    unsigned long  eventsInRunCounter_;
    string         fileName_;
    std::ofstream *ofs_;

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
      ofs_->open(fileName_.c_str(), ofstream::out | ofstream::binary);
      bytesInFileCounter_  = 0;
      eventsInFileCounter_ = 0;
      filesInRunCounter_++;
      LOG4CPLUS_INFO( logger_, "FileWriter opened " << fileName_ );
    }


    void close(){ 
      ofs_->close();
      LOG4CPLUS_INFO( logger_, "FileWriter wrote "<< 
		      eventsInFileCounter_ << " events ("         << 
		      bytesInFileCounter_  << " bytes) to "       << 
		      fileName_ 	   << "; so far "         << 
		      eventsInRunCounter_  << " events ("         << 
		      bytesInRunCounter_   << " bytes) in run "   << 
		      runNumber_ );
    }


  public:
    FileWriter(const unsigned long maxFileSize, const string pathToFile, const string prefix, const Logger* logger)
      :maxFileSize_         (maxFileSize)
      ,pathToFile_          (pathToFile)
      ,filePrefix_          (prefix)
      ,logger_              (*logger)
      ,bytesInFileCounter_  (0)
      ,eventsInFileCounter_ (0)
      ,filesInRunCounter_   (0)
      ,bytesInRunCounter_   (0)
      ,eventsInRunCounter_  (0)
    {
      ofs_ = new std::ofstream();
      LOG4CPLUS_INFO( logger_, "FileWriter: new writer has been created" );
    }

    ~FileWriter(){ delete ofs_; }

    void startNewRun( const int runNumber ){
      runNumber_          = runNumber;
      bytesInRunCounter_  = 0;
      eventsInRunCounter_ = 0;
      open();
    }

    void writeData( const char* buf, const int nBytes ){ 
      ofs_->write(buf, nBytes);
      bytesInFileCounter_ += nBytes;
      bytesInRunCounter_  += nBytes;
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
		      "FileWriter::endRun: Wrote "                << 
		      eventsInRunCounter_ << " events ("          << 
		      bytesInRunCounter_  <<" bytes) in "         <<
		      filesInRunCounter_  <<" file"               <<
		      (filesInRunCounter_==1?"":"s") <<" in run " <<
		      runNumber_ );
    }

  };


#endif //#ifndef _FileWriter_h_
