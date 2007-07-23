//-----------------------------------------------------------------------
// $Id: VMEParser.h,v 3.1 2007/07/23 05:02:24 gilmore Exp $
// $Log: VMEParser.h,v $
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
#ifndef VMEParser_h
#define VMEParser_h
/*
 *  class VMEParser
 *  author Alex Tumanov 7/8/03
 *     
 */

#include "Crate.h"
#include "VMEController.h"
#include "EmuParser.h"
#include <xercesc/dom/DOM.hpp>


XERCES_CPP_NAMESPACE_USE


class VMEParser : public EmuParser
{

public:
	VMEParser(){}
	explicit VMEParser(DOMNode * pNode, int crate = 0;);
	
	VMEController * controller() const {return controller_;}

protected:
	VMEController * controller_; 
};

#endif






