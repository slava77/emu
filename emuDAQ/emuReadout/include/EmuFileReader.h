#ifndef __EMU_FILE_READER_H__
#define __EMU_FILE_READER_H__

#include "EmuReader.h"

class EmuFileReader : public EmuReader
{
public:
  EmuFileReader( std::string filename, int format, bool debug=false );
  ~EmuFileReader();
  void            open( std::string filename );
  void            close();

protected:
  int             theFileDescriptor;
  unsigned short  a[600000];
  bool            fillBuff;
// moved to base class:  unsigned short  errorFlag;
  int             readDDU( unsigned short*& buf );
  int             readDCC( unsigned short*& buf );

//KK
private:
	unsigned short raw_event[200000];

	unsigned long long word_0, word_1, word_2; // To remember some history
	unsigned long long file_buffer[4000];      // Read data block for efficiency

	unsigned long long *end, *file_buffer_end; // where stoped last time and where is end

	enum {Header=1,Trailer=2,DDUoversize=4,FFFF=8,Unknown=16,EndOfStream=32};
	enum {Type1=Header|Trailer, Type2=Header, Type3=Header|DDUoversize, Type4=Trailer, Type5=Unknown, Type6=Unknown|DDUoversize, Type7=FFFF}; // Andrey Korytov's notations

	unsigned int eventStatus, selectCriteria, acceptCriteria, rejectCriteria;
//KKend
};

#endif  // ifndef __EMU_FILE_READER_H__
