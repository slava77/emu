//-----------------------------------------------------------------------
// $Id: FileReader.h,v 2.0 2005/04/13 10:52:56 geurts Exp $
// $Log: FileReader.h,v $
// Revision 2.0  2005/04/13 10:52:56  geurts
// Makefile
//
//
//-----------------------------------------------------------------------
#ifndef FileReader_h
#define FileReader_h

#include "DDUReader.h"

class FileReader : public DDUReader
{
public:
  FileReader() {}
  virtual ~FileReader() {}
  // DDUReader interface
  virtual void Configure() {};
  virtual int reset()        {return 0;}
  virtual int enableBlock()  {return 0;}
  virtual int disableBlock() {return 0;}
  virtual int endBlockRead() {return 0;}
  virtual int readDDU(unsigned short **buf, const bool debug = false);   
  // check of data
  virtual int check(int & EndofEvent, int count, unsigned short * tmp) = 0;
  
protected:
  virtual int chunkSize() {return 0;}

};

#endif

