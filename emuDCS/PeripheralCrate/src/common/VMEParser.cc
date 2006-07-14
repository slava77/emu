//-----------------------------------------------------------------------
// $Id: VMEParser.cc,v 2.9 2006/07/14 12:33:26 mey Exp $
// $Log: VMEParser.cc,v $
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

VMEParser::VMEParser(xercesc::DOMNode * pNode, int number, EmuSystem * emuSystem)
{
  int port;
  //MvdM new Parser std::string VMEaddress;
  //std::string ipAddress;
  std::string VMEaddress;
  parser_.parseNode(pNode);
  parser_.fillString("VMEaddress",VMEaddress); 
  //parser_.fillString("ipAddress",ipAddress); 
  parser_.fillInt("port",port);
  
  controller_ = new VMEController(number); 

// The following is just to show how it works. 
// Must be moved to somewhere else. Jinghua Liu
  controller_->init(VMEaddress, port);

  crate_ = new Crate(number, controller_,emuSystem);
}
//VMEParser::~VMEParser(){
//  delete crate_;
//}











