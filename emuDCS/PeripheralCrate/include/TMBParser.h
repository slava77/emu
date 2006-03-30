//-----------------------------------------------------------------------
// $Id: TMBParser.h,v 2.1 2006/03/30 13:55:38 mey Exp $
// $Log: TMBParser.h,v $
// Revision 2.1  2006/03/30 13:55:38  mey
// Update
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef TMBParser_h
#define TMBParser_h
#include "EmuParser.h"
#include <xercesc/dom/DOM.hpp>

class TMB;

class TMBParser
{
public:
  TMBParser(){}
  ~TMBParser();
  explicit TMBParser(xercesc::DOMNode * pNode, int crateNumber);
  
  /// returns the last tmb parsed
  TMB * tmb() const {return tmb_;}

private:
  TMB * tmb_;
  EmuParser parser_;
};

#endif






