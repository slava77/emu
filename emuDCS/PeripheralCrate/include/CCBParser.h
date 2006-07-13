//-----------------------------------------------------------------------
// $Id: CCBParser.h,v 2.1 2006/07/13 15:46:37 mey Exp $
// $Log: CCBParser.h,v $
// Revision 2.1  2006/07/13 15:46:37  mey
// New Parser strurture
//
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
class Crate;

class CCBParser{
public:
  CCBParser(){}
  explicit CCBParser(xercesc::DOMNode * pNode, Crate * crate);
  /// returns the last ccb parsed
  CCB * ccb() const {return ccb_;}

private:
  int slot, useTTClock, BX_Orbit, SPS25ns, l1aDelay, version;
  char * Version; 
  CCB * ccb_;
  EmuParser parser_;
};

#endif






