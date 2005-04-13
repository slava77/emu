//-----------------------------------------------------------------------
// $Id: EmuConfigRUI.cc,v 2.0 2005/04/13 10:52:59 geurts Exp $
// $Log: EmuConfigRUI.cc,v $
// Revision 2.0  2005/04/13 10:52:59  geurts
// Makefile
//
// Revision 1.3  2004/11/16 10:44:36  tfcvs
// changed default schar device (oct9 update)
//
// Revision 1.2  2004/10/09 00:04:33  tfcvs
// updated to DDUReadout
//
// Revision 1.1  2004/08/17 23:13:35  tfcvs
// initial version of EMU RUI for the new event builder
//
// Revision 1.3  2004/06/12 13:26:10  tfcvs
// minor changes, mostly additional output to stdout identifying the calling routine (FG)
//
// Revision 1.2  2004/06/11 08:22:23  tfcvs
// *** empty log message ***
//
//-----------------------------------------------------------------------
#include "EmuConfigRUI.h"
#include "FileReaderDDU.h"
#include "HardwareDDU.h"

EmuConfigRUI::EmuConfigRUI() :
  dduMode_("hardware"), dduInput_("/dev/schar2"), ddu_(0)
{
  // Export XDAQ configuration parameters
  exportParam("DDU Mode", dduMode_);
  exportParam("DDU Input", dduInput_);
}


void EmuConfigRUI::Configure() throw(exception) {

  std::cout << "EmuConfigRUI: mode=" << dduMode_ 
	    << " input=" << dduInput_ << std::endl;
  int status = 0;
  if(dduMode_ == "hardware") {
    ddu_ = new HardwareDDU(dduInput_);
  }
  if(dduMode_ == "file") {
    ddu_ = new FileReaderDDU();
  }
  if(!ddu_) {
    std::cerr << "EmuConfigRUI: ERROR - bad DDU mode; use dduMode=file or hardware"
	      << std::endl;
  }

  //fg -- maybe move this to the constructor of HardwareDDU/FileReaderDDU --
  if( ddu_->openFile(dduInput_) != 0 ) {
    // usually you won't get here: openFile typically will abort()
    std::cerr << "EmuConfigRUI: ERROR - cannot open file. Restart Application"
	      << std::endl;
  }

  //fg --  This should not be here but moved to the HardwareDDU() --
  if (dduMode_ == "hardware"){
    // reset the device and make sure we are reading it in Block mode
    ddu_->reset();
    ddu_->enableBlock();
  }
}
