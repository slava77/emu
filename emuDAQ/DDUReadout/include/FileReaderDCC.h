//-----------------------------------------------------------------------
// $Id: FileReaderDCC.h,v 2.0 2005/04/13 10:52:56 geurts Exp $
// $Log: FileReaderDCC.h,v $
// Revision 2.0  2005/04/13 10:52:56  geurts
// Makefile
//
//
//-----------------------------------------------------------------------
#ifndef FileReaderDCC_h
#define FileReaderDCC_h

#include "FileReader.h"
#include "DDUReader.h"

class FileReaderDCC : public FileReader
{
public:
  FileReaderDCC() {}
  virtual ~FileReaderDCC() {}
  virtual int check(int & EndofEvent, int count, unsigned short * tmp);
};

#endif

