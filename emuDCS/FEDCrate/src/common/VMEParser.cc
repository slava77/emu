//-----------------------------------------------------------------------
// $Id: VMEParser.cc,v 1.1 2005/08/31 18:20:32 gilmore Exp $
// $Log: VMEParser.cc,v $
// Revision 1.1  2005/08/31 18:20:32  gilmore
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
  int port;
  string ipAddress;
  parser_.parseNode(pNode);
  parser_.fillString("ipAddress",ipAddress); 
  parser_.fillInt("port",port);
  
  controller_ = new VMEController(ipAddress, port);
  crate_ = new Crate(number, controller_);
}


























