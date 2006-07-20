//-----------------------------------------------------------------------
// $Id: MPCParser.h,v 3.0 2006/07/20 21:15:47 geurts Exp $
// $Log: MPCParser.h,v $
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.1  2006/07/13 15:46:37  mey
// New Parser strurture
//
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
class Crate;

class MPCParser{
public:
  MPCParser(){}
  explicit MPCParser(xercesc::DOMNode * , Crate * );
  
  /// returns the last one parsed
  MPC * mpc() const {return mpc_;} 
 
protected:
  int slot;
  MPC * mpc_;
  EmuParser parser_;
};

#endif






