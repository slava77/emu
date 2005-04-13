//-----------------------------------------------------------------------
// $Id: HardwareDDU.h,v 2.0 2005/04/13 10:52:57 geurts Exp $
// $Log: HardwareDDU.h,v $
// Revision 2.0  2005/04/13 10:52:57  geurts
// Makefile
//
// Revision 1.2  2004/07/29 20:16:28  tfcvs
// tumanov
//
// Revision 1.1  2004/07/29 16:20:44  tfcvs
// *** empty log message ***
//
// Revision 1.10  2004/06/12 13:24:34  tfcvs
// minor changes in hardware readout; removal of unused variables (and warnings) (FG)
//
// Revision 1.9  2004/06/11 07:59:46  tfcvs
// New DDU readout code: improved blocked schar readout.
//
//-----------------------------------------------------------------------
#ifndef HardwareDDU_h
#define HardwareDDU_h
#include "DDUReader.h"
#include <string>


class HardwareDDU : public DDUReader {
public:
  HardwareDDU(std::string="/dev/schar");
  virtual ~HardwareDDU();
  virtual void Configure() {/* nope */};
  virtual int reset();
  virtual int enableBlock();
  virtual int disableBlock();
  virtual int endBlockRead();
  virtual int chunkSize();
  int readDDU(unsigned short **buf, const bool debug = false);
};

#endif

