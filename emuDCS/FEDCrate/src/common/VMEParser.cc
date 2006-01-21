//-----------------------------------------------------------------------
// $Id: VMEParser.cc,v 1.2 2006/01/21 19:55:02 gilmore Exp $
// $Log: VMEParser.cc,v $
// Revision 1.2  2006/01/21 19:55:02  gilmore
// *** empty log message ***
//
// Revision 1.8  2004/07/19 19:37:57  tfcvs
// Removed unused variables and unsigned some variables in order to prevent compiler warnings (-Wall flag) (FG)
//
//
//-----------------------------------------------------------------------
#include "VMEParser.h"
#include "VMEController.h"
#include "Crate.h"

VMEParser::VMEParser(DOMNode * pNode, int number)
{
  int Link;
  int Device;
  parser_.parseNode(pNode);
  parser_.fillInt("Device",Device); 
  parser_.fillInt("Link",Link);
  
  controller_ = new VMEController(Device, Link);
  crate_ = new Crate(number, controller_);
}


























