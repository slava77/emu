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
//KK
  end = (file_buffer_end = file_buffer + sizeof(file_buffer)/sizeof(unsigned long long));
  bzero(raw_event,  sizeof(raw_event)  );
  bzero(file_buffer,sizeof(file_buffer));
  word_0=0; word_1=0; word_2=0;
  eventStatus = 0;
//KKend
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

/*
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



*///    //************ 2 event headers in a row check  (bit 15 of errorFlag) *******
/*    if((tmp[3]==0x8000)&&(tmp[2]==0x0001)&&(tmp[1]==0x8000)) {
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
*///    //**************************************************************************
/*    if (newEventFound) check(EndofEvent, count, tmp);


    if(fillBuff) { //only fill buffer if beginning of event is found
      //printf("%d   %04x %04x %04x %04x  %d \n",count,tmp[3],tmp[2],tmp[1],tmp[0],EndofEvent);
      a[4*count]   = tmp[0];
      a[4*count+1] = tmp[1];
      a[4*count+2] = tmp[2];
      a[4*count+3] = tmp[3];
      count++;
    }


*///    //********************Event Trailer never reached check (bit 14 or errorFlag) ******
/*    if (count>100000) { //this happens if endOfEvent is never reached
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
*/

//KK
#include <stdexcept>   // std::runtime_error
int EmuFileReader::readDDU(unsigned short*& buf) {
	// Check for abnormal situation
	if( end>file_buffer_end || end<file_buffer ) throw ( std::runtime_error("Error reading file.") );
	if( !theFileDescriptor ) throw ( std::runtime_error("No file is open.") );

	unsigned long long *start = end;
	unsigned short     *event = raw_event;

	eventStatus = 0;
	size_t dduWordCount = 0;
	end = 0;

	while( !end && dduWordCount<50000 ){
		unsigned long long *dduWord = start;
		unsigned long long preHeader = 0;

		// Did we reach end of current buffer and want to read next block?
		// If it was first time and we don't have file buffer then we won't get inside
		while( dduWord<file_buffer_end && dduWordCount<50000 ){
			word_0 =  word_1; // delay by 2 DDU words
			word_1 =  word_2; // delay by 1 DDU word
			word_2 = *dduWord;// current DDU word
			if( (word_2&0xFFFFFFFFFFFF0000LL)==0x8000000180000000LL ){
				if( eventStatus&Header ){ // Second header
					word_2 = word_1; // Fall back to get rigth preHeader next time
					end = dduWord;   // Even if we end with preHeader of next evet put it to the end of this event too
					break;
				}
				if( dduWordCount>1 ){ // Extra words between trailer and header
					if( (word_0&0xFFFFFFFFFFFF0000LL)==0xFFFFFFFFFFFF0000LL ) eventStatus |= FFFF;
					word_2 = word_1; // Fall back to get rigth preHeader next time
					end = dduWord;
					break;
				}
				eventStatus |= Header;
				if( event==raw_event ) preHeader = word_1; // If preHeader not yet in event then put it there
				start = dduWord;
			}
			if( (word_0&0xFFFFFFFFFFFF0000LL)==0x8000FFFF80000000LL ){
				eventStatus |= Trailer;
				end = ++dduWord;
				break;
			}
			// Increase counters by one DDU word
			dduWord++;
			dduWordCount++;
		}

		// If have DDU Header
		if( preHeader ){
			// Need to account first word of DDU Header
			memcpy(event,&preHeader,sizeof(preHeader));
			event += sizeof(preHeader)/sizeof(unsigned short);
		}

		// Take care of the rest
		memcpy(event,start,(dduWord-start)*sizeof(unsigned long long));
		event += (dduWord-start)*sizeof(unsigned long long)/sizeof(unsigned short);

		// If reach max length
		if( dduWordCount==50000 ){ end = dduWord; break; }

		if( !end ){
			// Need to read next block for the rest of this event
			ssize_t length = ::read(theFileDescriptor,file_buffer,sizeof(file_buffer));
			if( length==-1 ) throw ( std::runtime_error("Error of reading") );
			if( length== 0 ){
				eventStatus |= EndOfStream;
				end = (file_buffer_end = file_buffer + sizeof(file_buffer)/sizeof(unsigned long long));
				break;
			}
			file_buffer_end = file_buffer + length/sizeof(unsigned long long);

			// Will start from the beginning of new buffer next time we read it
			start = file_buffer;
		}
	}

	if( !end ) eventStatus |= DDUoversize;
	if( !(eventStatus&Header) && !(eventStatus&Trailer) && !(eventStatus&FFFF) ) eventStatus |= Unknown;

	buf = (/*const*/ unsigned short*)raw_event;
	theErrorFlag = eventStatus;
	return (eventStatus&FFFF?event-raw_event-4:event-raw_event)*2;
}
//KKend


int EmuFileReader::readDCC(unsigned short*& buf) {
  // TODO
  return -1;
}
