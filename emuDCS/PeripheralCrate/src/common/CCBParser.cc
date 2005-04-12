//-----------------------------------------------------------------------
// $Id: CCBParser.cc,v 2.0 2005/04/12 08:07:05 geurts Exp $
// $Log: CCBParser.cc,v $
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
// Revision 1.8  2004/06/05 19:47:05  tfcvs
// Reorganized and debugged TTC control mode for CCB2004 (and 2001).
// CCB2004 mode configurable through XML option: <CCB CCBmode=""> (FG)
//
//
//-----------------------------------------------------------------------
#include "CCBParser.h"
#include "CCB.h"

CCBParser::CCBParser(xercesc::DOMNode *pNode, int crateNumber)
{
  parser_.parseNode(pNode);
  parser_.fillInt("slot", slot);
  // temporarily allow the CCB version to be passed to the constructor
  // this should be removed after once all the old version is carefully
  // shredded :)
  parser_.fillInt("version", version);
  ccb_ = new CCB(crateNumber, slot, version);
  parser_.fillInt("TTCmode", ccb_->TTC);
  parser_.fillInt("CCBmode", ccb_->mCCBMode);
  parser_.fillInt("BX_Orbit" , ccb_->BX_Orbit_);
  parser_.fillInt("SPS25ns", ccb_->SPS25ns_);
  parser_.fillInt("l1aDelay", ccb_->l1aDelay_);
}

