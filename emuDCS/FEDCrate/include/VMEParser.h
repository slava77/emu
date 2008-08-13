//-----------------------------------------------------------------------
// $Id: VMEParser.h,v 3.3 2008/08/13 14:20:41 paste Exp $
// $Log: VMEParser.h,v $
// Revision 3.3  2008/08/13 14:20:41  paste
// Massive update removing "using namespace" code and cleaning out stale header files as preparation for RPMs.
//
// Revision 3.2  2007/07/23 08:20:42  gujh
// Correct
// CorrectCorrectCorrectCorrect
//
// Revision 3.1  2007/07/23 05:02:24  gilmore
// major structural chages to improve multi-crate functionality
//
// Revision 3.0  2006/07/20 21:16:10  geurts
// *** empty log message ***
//
// Revision 1.2  2006/01/21 20:20:14  gilmore
// *** empty log message ***
//
// Revision 1.8  2004/07/19 19:37:57  tfcvs
// Removed unused variables and unsigned some variables in order to prevent compiler warnings (-Wall flag) (FG)
//
//
//-----------------------------------------------------------------------
#ifndef __VMEPARSER_H__
#define __VMEPARSER_H__
/*
 *  class VMEParser
 *  author Alex Tumanov 7/8/03
 *     
 */

//#include "Crate.h"
#include "VMEController.h"
#include "EmuParser.h"
#include <xercesc/dom/DOM.hpp>


XERCES_CPP_NAMESPACE_USE


class VMEParser : public EmuParser
{

public:
	VMEParser(){}
	explicit VMEParser(DOMNode * pNode, int crate = 0);
	
	VMEController * controller() const {return controller_;}

protected:
	VMEController * controller_; 
};

#endif






