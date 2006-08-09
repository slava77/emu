//-----------------------------------------------------------------------
// $Id: MPCParser.cc,v 3.2 2006/08/09 11:56:23 mey Exp $
// $Log: MPCParser.cc,v $
// Revision 3.2  2006/08/09 11:56:23  mey
// Got rid of friend classes in MPCParser
//
// Revision 3.1  2006/08/07 14:14:10  mey
// Added BoardId
//
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
  //
  int value;
  //
  if(parser_.fillInt("serializermode",value)){
    mpc_->SetTLK2501TxMode(value);
  }
  //
  if ( parser_.fillInt("TransparentMode",value)){
    mpc_->SetTransparentMode(value);
  }
  //
  if(parser_.fillInt("TMBdelays",value)){
    mpc_->SetTMBDelays(value);
  }
  //
  if(parser_.fillInt("BoardID",value)){
    mpc_->SetBoardID(value);
  }
  //
}


























