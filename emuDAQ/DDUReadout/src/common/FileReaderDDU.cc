//-----------------------------------------------------------------------
// $Id: FileReaderDDU.cc,v 2.2 2005/10/03 20:20:15 geurts Exp $
// $Log: FileReaderDDU.cc,v $
// Revision 2.2  2005/10/03 20:20:15  geurts
// Removed hardware-related implementations out of DDUReader, created dependency on driver-include files.
// - openFile is virtual function, HardwareDDU and FileReaderDDU take care of its own implementation
// - schar.h and eth_hook_2.h contain driver and bigphys parameters shared by the DDUReadout and eth_hook_2
//
// Revision 2.1  2005/09/26 17:11:11  tumanov
// new data format
//
// Revision 2.0  2005/04/13 10:52:57  geurts
// Makefile
//
//
//-----------------------------------------------------------------------
#include "FileReaderDDU.h"
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
 

int FileReaderDDU::openFile(std::string filename) {
   //std::cout << "filename here was " << filename << std::endl;
   //fg liveData_ = (filename.find("/dev/schar")==0);
   //fg if (liveData_)
   //fg   std::cout << "DDUread: we have got a life one here, Jimmy" << std::endl;
   fd_schar = open(filename.c_str(), O_RDONLY);
 
   // Abort in case of any failure
   if (fd_schar == -1) {
     std::cerr << "DDUReader: FATAL in openFile - " << std::strerror(errno) << std::endl;
     std::cerr << "DDUReader will abort!!!" << std::endl;
     abort();
   }
   return 0;
} 


int FileReaderDDU::check(int & EndofEvent, int count, unsigned short * tmp) {

  //*************************DDU error bit check** ( bit 13 of errorFlag)  **********
  if (EndofEvent == -2) {//check for DDU error bit in trailer
    if (debug) printf("%d   %04x %04x %04x %04x\n",count,tmp[3],tmp[2],tmp[1],tmp[0]);                
    //assign DDU error stat
    errorStat = (tmp[3]<<16)|tmp[2];

    if (tmp[2]&0x4000) {                                                                    
      std::cout << "DDU Error Detected in DDU Trailer! bit 46 = " << ((tmp[2]&0x4000)>>14) << std::endl;    
      errorFlag|=0x2000;                                                                    
    }                                                                                        
  }                                                                                          

  if((tmp[3]==0x8000)&&(tmp[2]==0xffff)&&(tmp[1]==0x8000)&&(tmp[0]==0x8000)) {
    EndofEvent = -3;                               //ddu trailer marker
    if (!fillBuff) {
      std::cout <<"reached end of event without header!!!" << std::endl;
      fillBuff = true;
    }
  }

  
  //*************DDU word count check (bit 12 of errorFlag) *************************
  if (EndofEvent == -1) {//check for DDU word count
    //printf("%d   %04x %04x %04x %04x\n",count,tmp[3],tmp[2],tmp[1],tmp[0]);
    int dduWordCount = ( (tmp[3]&0x00ff) << 16 ) | (tmp[2]&0xFFFF) ; 
    if (debug) std::cout << "DDU word count = " << std::dec<< dduWordCount << std::endl;
    if (debug) std::cout << "actual word count = " << std::dec<< count+1 << std::endl;
    if (count+1!=dduWordCount) {
      std::cout << "ERROR!!! DDU Word Count differs from actual count " << dduWordCount <<
	"!=" << count+1 << std::endl;
      errorFlag|=0x1000;
    }
  }
  return EndofEvent;
} 

