#ifndef __EMU_SLINK_READER_H__
#define __EMU_SLINK_READER_H__

#include "EmuReader.h"
#include "fedkit.h"
#include "fedkit_sender.h"

/// An EmuReader to read data from S-link (<b>not implemented</b>).
class EmuSLinkReader : public EmuReader
{
public:
  EmuSLinkReader( int boardIndex, int format, bool debug=false );
  ~EmuSLinkReader();
  void            open();
//   void            enable(){}
  int             reset();
  void            close();
  int             chunkSize();

protected:
  int                      theBoardIndex;
  struct fedkit_receiver * receiver;
  struct fedkit_fragment * frag;
  int   readDDU(unsigned short **buf);
  int   readDCC(unsigned short **buf);
};

#endif  // ifndef __EMU_SLINK_READER_H__
