//-----------------------------------------------------------------------
// $Id: VMEParser.cc,v 1.3 2006/01/27 16:04:50 gilmore Exp $
// $Log: VMEParser.cc,v $
// Revision 1.3  2006/01/27 16:04:50  gilmore
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
  parser_.fillInt("vmeirq_start", controller_->vmeirq_start_); 
  crate_ = new Crate(number, controller_); 
}


























