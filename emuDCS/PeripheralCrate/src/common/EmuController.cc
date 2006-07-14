//-----------------------------------------------------------------------
// $Id: EmuController.cc,v 1.5 2006/07/14 08:11:58 mey Exp $
// $Log: EmuController.cc,v $
// Revision 1.5  2006/07/14 08:11:58  mey
// Got rid of Singleton
//
// Revision 1.4  2006/07/13 15:46:37  mey
// New Parser strurture
//
// Revision 1.3  2006/02/15 22:39:57  mey
// UPdate
//
// Revision 1.2  2006/01/18 12:46:48  mey
// Update
//
// Revision 1.1  2006/01/11 08:55:37  mey
// Update
//
// Revision 1.2  2005/12/20 14:48:26  mey
// Update
//
// Revision 1.1  2005/12/20 14:37:40  mey
// Update
//
// Revision 2.2  2005/12/20 14:20:04  mey
// Update
//
// Revision 2.1  2005/08/11 08:13:04  mey
// Update
//
// Revision 2.0  2005/04/12 08:07:06  geurts
// *** empty log message ***
//
// Revision 1.23  2004/06/18 23:52:34  tfcvs
// Introduced code for DCS/PCcontrol crate sharing. (FG)
//  -code additions still commented out, awaiting validation-
//
// Revision 1.22  2004/06/05 19:37:55  tfcvs
// Clean-up (FG)
//
// Revision 1.21  2004/06/03 22:00:39  tfcvs
// Changed the order in which DMB and TMB/ALCT are configured.
// Apparently, a yet-to-be-identified call in the TMB or ALCT class has
// an adverse effect on DAQMB:set_comp_mode(). Although this relocation
// seems to solve the problem it should not be the final solution ...
// Also, removed the repeated DAQMB:calctrl_fifomrst() call, which does not
// affect operation. (JG/FG)
//
// Revision 1.20  2004/06/01 09:52:07  tfcvs
//  enabled CCB in enable(), added CVS-stuff, cleaned up includes (FG)
//
//-----------------------------------------------------------------------
#include <iostream>
#include <unistd.h> // for sleep
#include <string>
#include "EmuController.h"
#include "Crate.h"
#include "DAQMB.h"
#include "CCB.h"
#include "TMB.h"
#include "MPC.h"
#include "VMEController.h"
#include "EMUParser.h"

EmuController::EmuController(){
  // clear pointers
  xmlFile_     = 
    "/afs/cern.ch/user/m/mey/scratch0/v3.2/TriDAS/emu/emuDCS/PeripheralCrate/config.xml" ;
  //
}

void EmuController::init(){
  //
  std::cout << "Init " << std::endl ;
  //
  //-- parse XML file
  //
  std::cout << "---- XML parser ----" << std::endl;
  std::cout << " Here parser " << std::endl;
  EMUParser parser;
  std::cout << " Using file " << xmlFile_ << std::endl ;
  parser.parseFile(xmlFile_.c_str());
  //
  theSelector.SetEmuSystem(parser.GetEmuSystem());
  std::vector<Crate*> myCrates = theSelector.crates();
  //
  for(unsigned i = 0; i < myCrates.size(); ++i) {
    //myCrates[i]->init();
  }
}
//

void EmuController::configure() {
  // read the configuration
  std::vector<Crate*> myCrates = theSelector.crates();
  
  for(unsigned i = 0; i < myCrates.size(); ++i) {
    if(myCrates[i]) myCrates[i]->configure();
  }
}
//
//
void  EmuController::enable() {
  //
  std::vector<Crate*> myCrates = theSelector.crates();
  //
  for(unsigned i = 0; i < myCrates.size(); ++i) {
    if(myCrates[i]) myCrates[i]->enable();
  }
  std::cout << "TAKING DATA" << std::endl;
}
//
//
void EmuController::disable() {
  std::vector<Crate*> myCrates = theSelector.crates();

  for(unsigned i = 0; i < myCrates.size(); ++i) {
    if(myCrates[i]) myCrates[i]->disable();
  }
}
//
//

