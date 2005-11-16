#ifdef WITHOUT_DDUREADER
#include"FileReaderDDU.cc"
#else
#include"FileReaderDDU.h"
#endif
#include<iostream>
#include<iomanip>
#include<stdio.h>
#include<stdlib.h>
// To compile: g++ -W -Wall -o test FileReaderDDU.cpp -I../../src/common -I../../include/ -DWITHOUT_DDUREADER
// Does the same as: hexdump ./RunNum*.bin | awk --non-decimal-data '{print "  "$2"  "$3"  "$4"  "$5"\n  "$6"  "$7"  "$8"  "$9;}' | less

int main(int argc, char *argv[]	){
	if( argc!=3 ){ std::cerr<<"Two argumets are required: DDU data file and event number"<<std::endl; exit(1); }
	// Create reader
	FileReaderDDU qwe;
	// Open DDU file
	qwe.open(argv[1]);

	// Buffer, buffer size and event number variables
	const unsigned short *buf=0;
	size_t size, nevents=0;

	// Reject following kinds of error events
	qwe.reject(FileReaderDDU::DDUoversize|FileReaderDDU::FFFF|FileReaderDDU::Unknown);
	// Do not select anything in particular
	qwe.select(0);

	// Main cycle
	while( (size = qwe.read(buf)) ){
		// Event to print?
		if( nevents==(unsigned)atoi(argv[2]) ){
			size_t index = 0;
			std::cout<<"Buffer length ( in shorts ): "<<size<<std::endl;
			while( size ){
				std::cout<<std::hex<<"  "<<std::setw(4)<<std::setfill('0')<<buf[index+0]<<"  "<<std::setw(4)<<std::setfill('0')<<buf[index+1]<<"  "<<std::setw(4)<<std::setfill('0')<<buf[index+2]<<"  "<<std::setw(4)<<std::setfill('0')<<buf[index+3]<<std::dec<<std::endl;
				index += 4;
				size-=4;
			}
		}
		nevents++;
	}
	return 0;
}
