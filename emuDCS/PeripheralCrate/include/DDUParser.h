//-----------------------------------------------------------------------
// $Id: DDUParser.h,v 2.0 2005/04/12 08:07:03 geurts Exp $
// $Log: DDUParser.h,v $
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef DDUParser_h
#define DDUParser_h
#include "EmuParser.h"
#include <xercesc/dom/DOM.hpp>

class DDU;

class DDUParser
{

public:
  DDUParser(){}
  explicit DDUParser(xercesc::DOMNode * pNode, int crateNumber);
  ///returns the last one parsed
  DDU * ddu() const {return ddu_;} 
 
protected:
  int slot;
  DDU * ddu_;
  EmuParser parser_; 
};

#endif






