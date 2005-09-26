//-----------------------------------------------------------------------
// $Id: FileReader.cc,v 2.1 2005/09/26 17:11:11 tumanov Exp $
// $Log: FileReader.cc,v $
// Revision 2.1  2005/09/26 17:11:11  tumanov
// new data format
//
// Revision 2.0  2005/04/13 10:52:57  geurts
// Makefile
//
//
//-----------------------------------------------------------------------
#include "FileReader.h"

#include <iostream>
#include <cstdio>
#include <unistd.h>

int FileReader::readDDU(unsigned short **buf, const bool debug) {
  fillBuff=false;
  int EndofEvent = 0;
  int count = 0;
  unsigned short tmp[4];
  unsigned short a[600000];
  ssize_t bytes_read;
  bool newEventFound=false;  
  errorFlag = 0; 

  if (debug) std::cout << "ddu read event\n\n" << std::endl;

  while (EndofEvent != -1) {
    EndofEvent++;    

    bytes_read = read(fd_schar,tmp,8);  //read only 1 line == 8 bytes of data
    //printf("%d   %04x %04x %04x %04x  %d \n",count,tmp[3],tmp[2],tmp[1],tmp[0],EndofEvent);
    if(bytes_read != 8) {
      std::cout << "+++ DDUReader::readDDU: Error reading data +++" << bytes_read <<std::endl;
      count = 0;
      return count;  //returns false readNextEvent
    }

    if((count == 0)&&((tmp[3] & 0xf000)== 0x5000)) {  //beginning of event stream
      if (debug) std::cout << "fillbuf" << std::endl;
      fillBuff=true; 
      //} else if ((count == 0)&&((tmp[3] & 0xf000)!= 0x5000)){
      //std::cout << "Beginning of the Event is missing!!!" << std::endl;
    }
  

  
    //************ 2 event headers in a row check  (bit 15 of errorFlag) *******    
    if((tmp[3]==0x8000)&&(tmp[2]==0x0001)&&(tmp[1]==0x8000)) {         
      if (debug) std::cout << "New event found" << std::endl;  //ddu header marker
      if (newEventFound) {
	std::cout << "WARNING!!! EVENT IS MISSING EndOfEvent" << std::endl; 
        errorFlag|=0x8000;
	count = 0;
        EndofEvent = -1;
        fillBuff = false;
      }
      else newEventFound = true;
    }
    //**************************************************************************
    if (newEventFound) check(EndofEvent, count, tmp);

  
    if(fillBuff) { //only fill buffer if beginning of event is found
      //printf("%d   %04x %04x %04x %04x  %d \n",count,tmp[3],tmp[2],tmp[1],tmp[0],EndofEvent);
      a[4*count]   = tmp[0];
      a[4*count+1] = tmp[1];
      a[4*count+2] = tmp[2];
      a[4*count+3] = tmp[3];
      count++;
    }


    //********************Event Trailer never reached check (bit 14 or errorFlag) ****** 
    if (count>100000) { //this happens if endOfEvent is never reached
      std::cout << "Error!!! count > 100 000 - failed to find end of event" << std::endl;
      errorFlag|=0x4000;
      EndofEvent = -1; 
      count = 1;    
    }
  }

  *buf=(unsigned short *)a;

  //printf("\n\n\n  returning %d: \n\n\n",(count*8));

  return count*8;  //Total number of bytes read

} //readDDU

