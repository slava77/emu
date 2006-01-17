#ifndef EmuRUIWriter_h
#define EmuRUIWriter_h

#include <string>
#include <sstream>
#include <fstream>

class EmuRUIWriter{

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
    cout << "EmuRUIWriter::openFile Opened " << fileName_ << endl;
  }


  void closeFile(){ 
    ofs_->close();
    std::cout<<std::dec;
    cout 
      << "EmuRUIWriter::closeFile: Wrote "<< eventsInFileCounter_ 
      << " events ("                  << bytesInFileCounter_ 
      << " bytes) to file "           << fileName_ 
      << "; so far "                  << eventsInRunCounter_ 
      << " events ("                  << bytesInRunCounter_ 
      << " bytes) in run "            << runNumber_ 
      << endl;
  }


 public:
  EmuRUIWriter(unsigned long maxFileSize, string pathToFile)
    :maxFileSize_         (maxFileSize)
    ,pathToFile_          (pathToFile)
    ,bytesInFileCounter_  (0)
    ,eventsInFileCounter_ (0)
    ,filesInRunCounter_   (0)
    ,bytesInRunCounter_   (0)
    ,eventsInRunCounter_  (0){
    ofs_ = new std::ofstream();
    cout << "EmuRUIWriter::EmuRUIWriter: new writer has been created" << endl;
  }

  ~EmuRUIWriter(){ delete ofs_; }

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
    cout << "EmuRUIWriter::endRun: Wrote " << eventsInRunCounter_ << " events (" << bytesInRunCounter_ <<" bytes) in run " << runNumber_ << endl;
  }

};

#endif //#ifndef EmuRUIWriter_h
