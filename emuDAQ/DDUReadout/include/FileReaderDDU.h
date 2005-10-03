//-----------------------------------------------------------------------
// $Id: FileReaderDDU.h,v 2.1 2005/10/03 20:20:14 geurts Exp $
// $Log: FileReaderDDU.h,v $
// Revision 2.1  2005/10/03 20:20:14  geurts
// Removed hardware-related implementations out of DDUReader, created dependency on driver-include files.
// - openFile is virtual function, HardwareDDU and FileReaderDDU take care of its own implementation
// - schar.h and eth_hook_2.h contain driver and bigphys parameters shared by the DDUReadout and eth_hook_2
//
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
  int openFile(std::string);
};

#endif

