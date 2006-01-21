//-----------------------------------------------------------------------
// $Id: VMEParser.h,v 1.2 2006/01/21 20:20:14 gilmore Exp $
// $Log: VMEParser.h,v $
// Revision 1.2  2006/01/21 20:20:14  gilmore
// *** empty log message ***
//
// Revision 1.8  2004/07/19 19:37:57  tfcvs
// Removed unused variables and unsigned some variables in order to prevent compiler warnings (-Wall flag) (FG)
//
//
//-----------------------------------------------------------------------
#ifndef VMEParser_h
#define VMEParser_h
/*
 *  class VMEParser
 *  author Alex Tumanov 7/8/03
 *     
 */

class Crate;
class VMEController;
#include "EmuParser.h"
#include <xercesc/dom/DOM.hpp>


XERCES_CPP_NAMESPACE_USE


class VMEParser
{


public:
  VMEParser(){}
  explicit VMEParser(DOMNode * pNode, int number);

  Crate *crate() const {return crate_;} 
  VMEController * controller() const {return controller_;}

protected:
  Crate * crate_;
  VMEController * controller_; 
  EmuParser parser_;
};

#endif






