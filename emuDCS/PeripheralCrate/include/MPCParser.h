//-----------------------------------------------------------------------
// $Id: MPCParser.h,v 2.0 2005/04/12 08:07:03 geurts Exp $
// $Log: MPCParser.h,v $
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef MPCParser_h
#define MPCParser_h
#include "EmuParser.h"
#include <xercesc/dom/DOM.hpp>

class MPC;

class MPCParser

{


public:
  MPCParser(){}
  explicit MPCParser(xercesc::DOMNode * pNode, int crateNumber);
  
  /// returns the last one parsed
  MPC * mpc() const {return mpc_;} 
 
protected:
  int slot;
  MPC * mpc_;
  EmuParser parser_;
};

#endif






