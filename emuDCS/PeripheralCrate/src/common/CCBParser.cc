//-----------------------------------------------------------------------
// $Id: CCBParser.cc,v 2.2 2006/05/16 15:54:38 mey Exp $
// $Log: CCBParser.cc,v $
// Revision 2.2  2006/05/16 15:54:38  mey
// UPdate
//
// Revision 2.1  2005/11/03 18:24:56  mey
// I2C routines
//
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
  int mode;
  if ( parser_.fillInt("TTCmode", mode) ) {
    ccb_->SetTTCmode(mode);
  }
  if ( parser_.fillInt("CCBmode", mode) ) {
    ccb_->SetCCBmode(mode);
  }
  int BxOrbit;
  if ( parser_.fillInt("BX_Orbit" , BxOrbit) ) {
    ccb_->SetBxOrbit(BxOrbit);
  }
  int SPS25ns;
  if(parser_.fillInt("SPS25ns", SPS25ns)) {
    ccb_->SetSPS25ns(SPS25ns);
  }
  int delay;
  if (parser_.fillInt("l1aDelay", delay)){
    ccb_->SetL1aDelay(delay);
  }
  int ID;
  if (parser_.fillInt("TTCrxID", ID)){
    ccb_->SetTTCrxID(ID);
  }
}

