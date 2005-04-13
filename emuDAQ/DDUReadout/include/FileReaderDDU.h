//-----------------------------------------------------------------------
// $Id: FileReaderDDU.h,v 2.0 2005/04/13 10:52:56 geurts Exp $
// $Log: FileReaderDDU.h,v $
// Revision 2.0  2005/04/13 10:52:56  geurts
// Makefile
//
//
//-----------------------------------------------------------------------
#ifndef FileReaderDDU_h
#define FileReaderDDU_h

#include "FileReader.h"
#include "DDUReader.h"

class FileReaderDDU : public FileReader
{
public:
  FileReaderDDU() {}
  virtual ~FileReaderDDU() {}
  // DDUReader interface
  virtual int check(int & EndofEvent, int count, unsigned short * tmp);  
};

#endif

