// $Id: EmuCompareMcsFiles.h,v 1.1 2011/05/02 13:47:22 liu Exp $

#ifndef _EmuCompareMcsFiles_h_
#define _EmuCompareMcsFiles_h_

#include "xgi/Utils.h"
#include "xgi/Method.h"

#include "xdaq/Application.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"
#include "xdaq/WebApplication.h"

#include "xdaq/NamespaceURI.h"

#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/Method.h"

#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

#include "xdata/exdr/FixedSizeInputStreamBuffer.h"
#include "xdata/exdr/AutoSizeOutputStreamBuffer.h"
#include "xdata/exdr/Serializer.h"
#include <xdata/String.h>
#include <xdata/Float.h>
#include <xdata/Double.h>
#include <xdata/Integer.h>
#include <xdata/Boolean.h>
#include <xdata/UnsignedLong.h>
#include <xdata/UnsignedShort.h>
#include <xdata/Table.h>

#include "toolbox/Event.h"
#include "toolbox/string.h"
#include "toolbox/net/URL.h"

// #include "emu/pc/EmuPeripheralCrateBase.h"

namespace emu {
  namespace pc {
  
class EmuCompareMcsFiles: public xdaq::WebApplication
{
  
public:
  
  XDAQ_INSTANTIATOR();
  
  //
  EmuCompareMcsFiles(xdaq::ApplicationStub * s);
  //
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void MainPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void SelectFile(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void Compare(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void SwitchBoard(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

  int bitcmp(char s, char c);
  int binsize(int mcs, FILE *finp);
  int readmcs(char *binbuf, unsigned start_address, unsigned end_address, FILE *finp);

private:

  xdata::UnsignedShort fastloop, slowloop, extraloop;
  bool newfile, file_loaded[2], file_checked[2], file_valid[2], compared, identical;
  int error_idx[200], bit_error, byte_error;
  char *temp_mcsname[2];
  std::string original_file[2];
  char comment1_buf[4096], comment2_buf[4096];
  char *bbuf1, *bbuf2;
  unsigned fsize[2], start_add[2], end_add[2], common_start, common_end, comp_size;
      
};

  } // namespace emu::pc
} // namespace emu
  
#endif
