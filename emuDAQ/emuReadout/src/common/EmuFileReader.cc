#include "EmuFileReader.h"

#include <iostream>
// #include <string>
#include <fcntl.h>  // for open()
#include <unistd.h> // for read(), close()
#include <errno.h>  // for errno

EmuFileReader::EmuFileReader( std::string filename, int format, bool debug )
  : EmuReader( filename, format, debug ),
    theFileDescriptor( -1 )
{
//   theFile = new std::ifstream();
  open( filename );
}

EmuFileReader::~EmuFileReader(){
  close();
}

void EmuFileReader::open(std::string filename) {

//   if ( theFile->is_open() ) theFile->close();
//   theFile->open( filename.c_str(), std::ifstream::in | std::ifstream::binary );
//   theFileDescriptor = theFile->rdbuf()->fd();

   theFileDescriptor = ::open(filename.c_str(), O_RDONLY);

   // Abort in case of any failure
   if (theFileDescriptor == -1) {
     if ( theDebugMode ){
       std::cerr << "EmuFileReader: FATAL in open - " << std::strerror(errno) << std::endl;
       std::cerr << "EmuFileReader will abort!!!" << std::endl;
       abort();
     }
     throw std::strerror(errno);
   }

}

void EmuFileReader::close() {
//   theFile->close();
  ::close(theFileDescriptor);
}

// int EmuFileReader::readDDU(unsigned short **buf) {
//   fillBuff=false;
//   int EndofEvent = 0;
//   int count = 0;
//   unsigned short tmp[4];
// // Promoted to class member to avoid passing a pointer-to-local:  unsigned short a[600000];
//   ssize_t bytes_read;
//   bool newEventFound=false;  
//   errorFlag = 0; 

//   if (theDebugMode) std::cout << "ddu read event\n\n" << std::endl;

//   while (EndofEvent != -1) {
//     EndofEvent++;    

//     bytes_read = ::read(theFileDescriptor,tmp,8);  //read only 1 line == 8 bytes of data
// //     bytes_read = theFile->read( (char*)tmp, 8 ).gcount();  //read only 1 line == 8 bytes of data
//     //printf("%d   %04x %04x %04x %04x  %d \n",count,tmp[3],tmp[2],tmp[1],tmp[0],EndofEvent);
//     if(bytes_read != 8) {
//       std::cout << "+++ EmuFileReader::readDDU: Error reading data +++" << bytes_read <<std::endl;
//       count = 0;
//       return count;  //returns false readNextEvent
//     }

//     if((count == 0)&&((tmp[3] & 0xf000)== 0x5000)) {  //beginning of event stream
//       if (theDebugMode) std::cout << "fillbuf" << std::endl;
//       fillBuff=true; 
//       //} else if ((count == 0)&&((tmp[3] & 0xf000)!= 0x5000)){
//       //std::cout << "Beginning of the Event is missing!!!" << std::endl;
//     }
  

  
//     //************ 2 event headers in a row check  (bit 15 of errorFlag) *******    
//     if((tmp[3]==0x8000)&&(tmp[2]==0x0001)&&(tmp[1]==0x8000)) {         
//       if (theDebugMode) std::cout << "New event found" << std::endl;  //ddu header marker
//       if (newEventFound) {
// 	std::cout << "WARNING!!! EVENT IS MISSING EndOfEvent" << std::endl; 
//         errorFlag|=0x8000;
// 	count = 0;
//         EndofEvent = -1;
//         fillBuff = false;
//       }
//       else newEventFound = true;
//     }
//     //**************************************************************************
//     if (newEventFound) check(EndofEvent, count, tmp);

  
//     if(fillBuff) { //only fill buffer if beginning of event is found
//       //printf("%d   %04x %04x %04x %04x  %d \n",count,tmp[3],tmp[2],tmp[1],tmp[0],EndofEvent);
//       a[4*count]   = tmp[0];
//       a[4*count+1] = tmp[1];
//       a[4*count+2] = tmp[2];
//       a[4*count+3] = tmp[3];
//       count++;
//     }


//     //********************Event Trailer never reached check (bit 14 or errorFlag) ****** 
//     if (count>100000) { //this happens if endOfEvent is never reached
//       std::cout << "Error!!! count > 100 000 - failed to find end of event" << std::endl;
//       errorFlag|=0x4000;
//       EndofEvent = -1; 
//       count = 1;    
//     }
//   }

//   *buf=(unsigned short *)a;

//   //printf("\n\n\n  returning %d: \n\n\n",(count*8));

//   return count*8;  //Total number of bytes read

// } //readDDU

int EmuFileReader::readDDU(unsigned short*& buf) {
  fillBuff=false;
  int EndofEvent = 0;
  int count = 0;
  unsigned short tmp[4];
// Promoted to class member to avoid passing a pointer-to-local:  unsigned short a[600000];
  ssize_t bytes_read;
  bool newEventFound=false;  
  errorFlag = 0; 

  if (theDebugMode) std::cout << "ddu read event\n\n" << std::endl;

  while (EndofEvent != -1) {
    EndofEvent++;    

    bytes_read = ::read(theFileDescriptor,tmp,8);  //read only 1 line == 8 bytes of data
//     bytes_read = theFile->read( (char*)tmp, 8 ).gcount();  //read only 1 line == 8 bytes of data
    //printf("%d   %04x %04x %04x %04x  %d \n",count,tmp[3],tmp[2],tmp[1],tmp[0],EndofEvent);
    if(bytes_read != 8) {
      if (theDebugMode) std::cout << "+++ EmuFileReader::readDDU: Error reading data +++" << bytes_read <<std::endl;
      count = 0;
      return count;  //returns false readNextEvent
    }

    if((count == 0)&&((tmp[3] & 0xf000)== 0x5000)) {  //beginning of event stream
      if (theDebugMode) std::cout << "fillbuf" << std::endl;
      fillBuff=true; 
      //} else if ((count == 0)&&((tmp[3] & 0xf000)!= 0x5000)){
      //std::cout << "Beginning of the Event is missing!!!" << std::endl;
    }
  

  
    //************ 2 event headers in a row check  (bit 15 of errorFlag) *******    
    if((tmp[3]==0x8000)&&(tmp[2]==0x0001)&&(tmp[1]==0x8000)) {         
      if (theDebugMode) std::cout << "New event found" << std::endl;  //ddu header marker
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

  buf = a;

  //printf("\n\n\n  returning %d: \n\n\n",(count*8));

  return count*8;  //Total number of bytes read

} //readDDU


int EmuFileReader::check(int & EndofEvent, int count, unsigned short * tmp) {
  //*************************DDU error bit check** ( bit 13 of errorFlag)  **********
  if (EndofEvent == -2) {//check for DDU error bit in trailer
    if (theDebugMode) printf("%d   %04x %04x %04x %04x\n",count,tmp[3],tmp[2],tmp[1],tmp[0]);                
//     //assign DDU error stat
//     errorStat = (tmp[3]<<16)|tmp[2];

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
    if (theDebugMode) std::cout << "DDU word count = " << std::dec<< dduWordCount << std::endl;
    if (theDebugMode) std::cout << "actual word count = " << std::dec<< count+1 << std::endl;
    if (count+1!=dduWordCount) {
      std::cout << "ERROR!!! DDU Word Count differs from actual count " << dduWordCount <<
	"!=" << count+1 << std::endl;
      errorFlag|=0x1000;
    }
  }
  return EndofEvent;
}

int EmuFileReader::readDCC(unsigned short*& buf) {
  // TODO
  return -1;
}
