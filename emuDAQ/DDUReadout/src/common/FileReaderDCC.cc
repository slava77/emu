//-----------------------------------------------------------------------
// $Id: FileReaderDCC.cc,v 2.0 2005/04/13 10:52:57 geurts Exp $
// $Log: FileReaderDCC.cc,v $
// Revision 2.0  2005/04/13 10:52:57  geurts
// Makefile
//
//
//-----------------------------------------------------------------------
#include "FileReaderDCC.h"

#include <iostream>
#include <cstdio>
#include <unistd.h>

int FileReaderDCC::check(int & EndofEvent, int count, unsigned short * tmp){


  //*************************DDU error bit check** ( bit 13 of errorFlag)  **********
  if (EndofEvent == -4) {//check for DDU error bit in trailer
    //printf("%d   %04x %04x %04x %04x\n",count,tmp[3],tmp[2],tmp[1],tmp[0]);                
    if (tmp[2]&0x4000) {                                                                    
      std::cout << "DDU Error Detected in DDU Trailer! bit 46 = " << ((tmp[2]&0x4000)>>14) << std::endl;    
      errorFlag|=0x2000;                                                                    
    }                                                                                        
  }                                                                                          


  //END OF EVENT MARK
  if((tmp[3]==0x8000)&&(tmp[2]==0xffff)&&(tmp[1]==0x8000)&&(tmp[0]==0x8000)) {
    EndofEvent = -5;                               //ddu trailer marker
  }


  //*************DDU word count check (bit 12 of errorFlag) *************************
  if (EndofEvent == -3) {//check for DDU word count
    //printf("%d   %04x %04x %04x %04x\n",count,tmp[3],tmp[2],tmp[1],tmp[0]);
    int dduWordCount = ( (tmp[3]&0x00ff) << 16 ) | (tmp[2]&0xFFFF) ; 
    if (debug) std::cout << "DDU word count = " << std::dec<< dduWordCount << std::endl;
    if (debug) std::cout << "actual word count = " << std::dec<< count-1 << std::endl;
    if (count!=dduWordCount+1) {
      std::cout << "ERROR!!! DDU Word Count differs from actual count " << dduWordCount <<
	"!=" << count -1  << std::endl;
      errorFlag|=0x1000;
    }
  }
  return EndofEvent;
}
