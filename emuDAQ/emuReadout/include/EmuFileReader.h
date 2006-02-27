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
  unsigned short  errorFlag;
//   int             readDDU(unsigned short **buf);
//   int             readDCC(unsigned short **buf);
  int             readDDU( unsigned short*& buf );
  int             readDCC( unsigned short*& buf );
  int             check(int & EndofEvent, int count, unsigned short * tmp);
};

#endif  // ifndef __EMU_FILE_READER_H__
