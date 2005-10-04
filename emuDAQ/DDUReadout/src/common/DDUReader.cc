//-----------------------------------------------------------------------
// $Id: DDUReader.cc,v 2.3 2005/10/04 17:47:40 geurts Exp $
// $Log: DDUReader.cc,v $
// Revision 2.3  2005/10/04 17:47:40  geurts
// bug update: keep marginal openFile implementation
//
// Revision 2.2  2005/10/03 20:20:15  geurts
// Removed hardware-related implementations out of DDUReader, created dependency on driver-include files.
// - openFile is virtual function, HardwareDDU and FileReaderDDU take care of its own implementation
// - schar.h and eth_hook_2.h contain driver and bigphys parameters shared by the DDUReadout and eth_hook_2
//
// Revision 2.1  2005/10/03 19:20:23  geurts
// BigPhys/Gbit driver and reader updates to prevent bigphys data corruption
//
// Revision 2.0  2005/04/13 10:52:57  geurts
// Makefile
//
// Revision 1.8  2004/10/03 16:18:20  tfcvs
// introduced new memory mapped DDU readout, switchable with the USE_DDU2004 switch (FG)
//
// Revision 1.7  2004/09/29 16:20:32  tfcvs
// tumanov, dcc added
//
// Revision 1.6  2004/08/19 15:24:15  tfcvs
// tumanov
//
// Revision 1.5  2004/08/18 16:51:37  tfcvs
// pass compiler
//
// Revision 1.4  2004/08/18 16:37:03  tfcvs
// use EventReader.h in EmuDAQ/GenericRUI
//
// Revision 1.3  2004/07/29 20:16:14  tfcvs
// tumanov
//
// Revision 1.2  2004/07/29 19:50:03  tfcvs
// tumanov
//
// Revision 1.1  2004/07/29 16:21:21  tfcvs
// *** empty log message ***
//
// Revision 1.16  2004/06/12 13:24:34  tfcvs
// minor changes in hardware readout; removal of unused variables (and warnings) (FG)
//
//-----------------------------------------------------------------------
// Description:   Reads new binary data format
#include "DDUReader.h"

#include <iostream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <cstdio>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string>
#include <sys/mman.h>
#include "Muon/METBRawFormat/interface/MuEndDDUHeader.h"
#include "schar.h"
#include "eth_hook_2.h"


bool DDUReader::debug = false;

int DDUReader::openFile(std::string filename) {

  fd_schar = open(filename.c_str(), O_RDONLY);

  // Abort in case of any failure
  if (fd_schar == -1) {
    std::cerr << "DDUReader: FATAL in openFile - " << std::strerror(errno) << std::endl;
    std::cerr << "DDUReader will abort!!!" << std::endl;
    abort();
  }

  return 0;
}

bool DDUReader::readNextEvent() {
  unsigned short ** buf2 = new unsigned short* ; 
  theDataLength = readDDU(buf2, debug);
  if (debug) std::cout << " theDataLength " << theDataLength << std::endl;
  if(theDataLength<=7) return false;
  unsigned short * buf=(unsigned short *)*buf2;
  theBuffer = buf; 
  delete buf2;  
  return true;
}

void DDUReader::printStats()
{
  std::cout << " npackets " << std::dec << npack_schar
       << " nbytes " << nbytes_schar << std::endl; 
}

void DDUReader::closeFile() {
#ifdef USE_DDU2004
  // new MemoryMapped DDU readout
  std::cout << "close and unmmap" << std::endl;
  munmap((void *)buf_start,BIGPHYS_PAGES_2*PAGE_SIZE);
#endif
  close(fd_schar);
}

int DDUReader::eventNumber() {
  const MuEndDDUHeader * dduHeader = (const MuEndDDUHeader *) theBuffer;
  return dduHeader->lvl1num();
}



