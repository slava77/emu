//-----------------------------------------------------------------------
// $Id: DDUParser.cc,v 2.0 2005/04/12 08:07:05 geurts Exp $
// $Log: DDUParser.cc,v $
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include "DDUParser.h"
#include "DDU.h"

DDUParser::DDUParser(xercesc::DOMNode * pNode, int crateNumber)
{ 
  parser_.parseNode(pNode);
  parser_.fillInt("slot", slot);  
  ddu_ = new DDU(crateNumber, slot);
}


























