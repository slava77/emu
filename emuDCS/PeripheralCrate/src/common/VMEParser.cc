//-----------------------------------------------------------------------
// $Id: VMEParser.cc,v 2.0 2005/04/12 08:07:06 geurts Exp $
// $Log: VMEParser.cc,v $
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

VMEParser::VMEParser(xercesc::DOMNode * pNode, int number)
{
  int port;
  std::string ipAddress;
  parser_.parseNode(pNode);
  parser_.fillString("ipAddress",ipAddress); 
  parser_.fillInt("port",port);
  
  controller_ = new VMEController(number, ipAddress, port);
  crate_ = new Crate(number, controller_);
}


























