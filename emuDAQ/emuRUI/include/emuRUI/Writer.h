#ifndef emuRUI_Writer_h
#define emuRUI_Writer_h

#include <string>
#include <sstream>
#include <fstream>
#include "log4cplus/logger.h"

namespace emuRUI{

  class Writer{

  private:
    unsigned long  runNumber_;
    unsigned long  maxFileSize_; // bytes
    string         pathToFile_;
    unsigned long  bytesInFileCounter_;
    unsigned long  eventsInFileCounter_;
    unsigned long  filesInRunCounter_;
    unsigned long  bytesInRunCounter_;
    unsigned long  eventsInRunCounter_;
    string         fileName_;
    std::ofstream *ofs_;
    Logger         logger_;

    string nameFile(){
      ostringstream fileNameStream;
      fileNameStream << pathToFile_ << "/EmuRUI_Run";
      fileNameStream.fill('0');
      fileNameStream.width(5);
      fileNameStream << runNumber_ << "_";
      fileNameStream.width(3);
      fileNameStream << filesInRunCounter_ << ".bin";
      return fileNameStream.str();
    }

    void openFile(){
      fileName_ = nameFile();
      ofs_->open(fileName_.c_str(), ofstream::out | ofstream::binary);
      bytesInFileCounter_  = 0;
      eventsInFileCounter_ = 0;
      filesInRunCounter_++;
      LOG4CPLUS_INFO( logger_, "emuRUI::Writer::openFile Opened " << fileName_ );
//       cout << "emuRUI::Writer::openFile Opened " << fileName_ << endl;
    }


    void closeFile(){ 
      ofs_->close();
      LOG4CPLUS_INFO( logger_, "emuRUI::Writer::closeFile: Wrote "<< 
		      eventsInFileCounter_ << " events ("         << 
		      bytesInFileCounter_  << " bytes) to file "  << 
		      fileName_ 	   << "; so far "         << 
		      eventsInRunCounter_  << " events ("         << 
		      bytesInRunCounter_   << " bytes) in run "   << 
		      runNumber_ );
    }


  public:
    Writer(unsigned long maxFileSize, string pathToFile, Logger* logger)
      :maxFileSize_         (maxFileSize)
      ,pathToFile_          (pathToFile)
      ,bytesInFileCounter_  (0)
      ,eventsInFileCounter_ (0)
      ,filesInRunCounter_   (0)
      ,bytesInRunCounter_   (0)
      ,eventsInRunCounter_  (0)
      ,logger_(*logger)
    {
      ofs_ = new std::ofstream();
      LOG4CPLUS_INFO( logger_, "EmuRUIWriter::EmuRUIWriter: new writer has been created" );
    }

    ~Writer(){ delete ofs_; }

    void SetLogger( Logger* logger ){ logger_ = *logger; }

    void startNewRun(int runNumber){
      runNumber_          = runNumber;
      bytesInRunCounter_  = 0;
      eventsInRunCounter_ = 0;
      openFile();
    }

    void writeData( char* buf, int nBytes ){ 
      ofs_->write(buf, nBytes);
      bytesInFileCounter_ += nBytes;
      bytesInRunCounter_  += nBytes;
    }

    void startNewEvent(){
      eventsInFileCounter_++;
      eventsInRunCounter_++;
      if ( bytesInFileCounter_ > maxFileSize_ ){
	closeFile();
	openFile();
      }
    }

    void endRun(){ 
      closeFile();
      LOG4CPLUS_INFO( logger_, 
		      "emuRUI::Writer::endRun: Wrote "        << 
		      eventsInRunCounter_ << " events ("      << 
		      bytesInRunCounter_  <<" bytes) in run " << 
		      runNumber_ );
    }

  };

} // namespace emuRUI

#endif //#ifndef EmuRUIWriter_h
