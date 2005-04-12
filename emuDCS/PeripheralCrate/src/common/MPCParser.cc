//-----------------------------------------------------------------------
// $Id: MPCParser.cc,v 2.0 2005/04/12 08:07:05 geurts Exp $
// $Log: MPCParser.cc,v $
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include "MPCParser.h"
#include "MPC.h"

MPCParser::MPCParser(xercesc::DOMNode * pNode, int crateNumber)
{
  parser_.parseNode(pNode);
  parser_.fillInt("slot", slot);
  mpc_ = new MPC(crateNumber, slot);
  parser_.fillInt("serializermode",mpc_->TLK2501TxMode_);
  parser_.fillInt("TransparentMode",mpc_->TransparentModeSources_);
  parser_.fillInt("TMBdelays",mpc_->TMBDelayPattern_);
}


























