//-----------------------------------------------------------------------
// $Id: VMEParser.h,v 2.0 2005/04/12 08:07:03 geurts Exp $
// $Log: VMEParser.h,v $
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
// Revision 1.8  2004/07/19 19:37:57  tfcvs
// Removed unused variables and unsigned some variables in order to prevent compiler warnings (-Wall flag) (FG)
//
//-----------------------------------------------------------------------
#ifndef VMEParser_h
#define VMEParser_h
#include "EmuParser.h"
#include <xercesc/dom/DOM.hpp>

class Crate;
class VMEController;

class VMEParser
{


public:
  VMEParser(){}
  explicit VMEParser(xercesc::DOMNode * pNode, int number);

  Crate *crate() const {return crate_;} 
  VMEController * controller() const {return controller_;}

protected:
  Crate * crate_;
  VMEController * controller_; 
  EmuParser parser_;
};

#endif






