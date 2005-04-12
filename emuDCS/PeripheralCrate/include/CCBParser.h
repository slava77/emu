//-----------------------------------------------------------------------
// $Id: CCBParser.h,v 2.0 2005/04/12 08:07:03 geurts Exp $
// $Log: CCBParser.h,v $
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef CCBParser_h
#define CCBParser_h
#include "EmuParser.h"
#include <xercesc/dom/DOM.hpp>

class CCB;

class CCBParser{
public:
  CCBParser(){}
  explicit CCBParser(xercesc::DOMNode * pNode, int crateNumber);
  /// returns the last ccb parsed
  CCB * ccb() const {return ccb_;}

private:
  int slot, useTTClock, BX_Orbit, SPS25ns, l1aDelay, version;
  char * Version; 
  CCB * ccb_;
  EmuParser parser_;
};

#endif






