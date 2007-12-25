//-----------------------------------------------------------------------
// $Id: VMEParser.cc,v 3.5 2007/12/25 14:14:25 liu Exp $
// $Log: VMEParser.cc,v $
// Revision 3.5  2007/12/25 14:14:25  liu
// update
//
// Revision 3.4  2007/12/25 13:57:05  liu
// update
//
// Revision 3.3  2007/12/17 15:03:13  liu
// remove Crate dependence from VMEController
//
// Revision 3.2  2006/11/28 14:17:16  mey
// UPdate
//
// Revision 3.1  2006/11/15 16:01:37  mey
// Cleaning up code
//
// Revision 3.0  2006/07/20 21:15:48  geurts
// *** empty log message ***
//
// Revision 2.10  2006/07/20 14:03:12  mey
// Update
//
// Revision 2.9  2006/07/14 12:33:26  mey
// New XML structure
//
// Revision 2.8  2006/07/13 15:46:37  mey
// New Parser strurture
//
// Revision 2.7  2006/07/11 14:49:29  mey
// New Parser sturcture ready to go
//
// Revision 2.6  2006/03/30 13:55:38  mey
// Update
//
// Revision 2.5  2006/03/10 13:13:13  mey
// Jinghua's changes
//
// Revision 2.2  2005/11/21 15:48:31  mey
// Update
//
// Revision 2.1  2005/11/02 16:16:36  mey
// Update for new controller
//
// Revision 2.0  2005/04/12 08:07:06  geurts
// *** empty log message ***
//
// Revision 1.8  2004/07/19 19:37:57  tfcvs
// Removed unused variables and unsigned some variables in order to prevent compiler warnings (-Wall flag) (FG)
//
//-----------------------------------------------------------------------
#include "VMEParser.h"
#include "VMEController.h"
#include "Crate.h"
#include "EmuSystem.h"

VMEParser::VMEParser(xercesc::DOMNode * pNode, int CrateID, EmuSystem * emuSystem)
{
  int port;
  //MvdM new Parser std::string VMEaddress;
  //std::string ipAddress;
  std::string VMEaddress;
  parser_.parseNode(pNode);
  parser_.fillString("VMEaddress",VMEaddress); 
  //parser_.fillString("ipAddress",ipAddress); 
  parser_.fillInt("port",port);

  std::string label;
  parser_.fillString("label",label);   

  controller_ = new VMEController(); 

  // The following is just to show how it works. 
  // Must be moved to somewhere else. Jinghua Liu
  
  //controller_->init(VMEaddress, port); //Moved to end of parsing

  controller_->SetVMEAddress(VMEaddress);
  controller_->SetPort(port);
  
  crate_ = new Crate(CrateID,controller_);
  crate_->SetLabel(label);

  emuSystem->addCrate(crate_);  
}
//VMEParser::~VMEParser(){
//  delete crate_;
//}

