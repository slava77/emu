//-----------------------------------------------------------------------
// $Id: VMEParser.h,v 2.3 2006/03/30 13:55:38 mey Exp $
// $Log: VMEParser.h,v $
// Revision 2.3  2006/03/30 13:55:38  mey
// Update
//
// Revision 2.2  2005/11/21 15:47:42  mey
// Update
//
// Revision 2.1  2005/11/02 16:17:16  mey
// Update for new controller
//
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
  //~VMEParser();
  explicit VMEParser(xercesc::DOMNode * pNode, int number);

  Crate *crate() const {return crate_;} 
  VMEController * controller() const {return controller_;}

protected:
  Crate * crate_;
  VMEController * controller_; 
  EmuParser parser_;
};

#endif
