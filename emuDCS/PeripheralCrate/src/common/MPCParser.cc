//-----------------------------------------------------------------------
// $Id: MPCParser.cc,v 3.0 2006/07/20 21:15:48 geurts Exp $
// $Log: MPCParser.cc,v $
// Revision 3.0  2006/07/20 21:15:48  geurts
// *** empty log message ***
//
// Revision 2.1  2006/07/13 15:46:37  mey
// New Parser strurture
//
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include "MPCParser.h"
#include "MPC.h"

MPCParser::MPCParser(xercesc::DOMNode * pNode, Crate * theCrate)
{
  parser_.parseNode(pNode);
  parser_.fillInt("slot", slot);
  mpc_ = new MPC(theCrate, slot);
  parser_.fillInt("serializermode",mpc_->TLK2501TxMode_);
  parser_.fillInt("TransparentMode",mpc_->TransparentModeSources_);
  parser_.fillInt("TMBdelays",mpc_->TMBDelayPattern_);
}


























