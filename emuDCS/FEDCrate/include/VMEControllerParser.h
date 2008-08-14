//-----------------------------------------------------------------------
// $Id: VMEControllerParser.h,v 3.1 2008/08/14 14:14:32 paste Exp $
// $Log: VMEControllerParser.h,v $
// Revision 3.1  2008/08/14 14:14:32  paste
// Adding things to namespace emu::fed, condensing include files, renaming VMEParser.
//
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
#ifndef __VMECONTROLLERPARSER_H__
#define __VMECONTROLLERPARSER_H__

#include <xercesc/dom/DOM.hpp>

#include "EmuParser.h"
namespace emu {
	namespace fed {

		class VMEController;

		class VMEControllerParser : public EmuParser
		{
		
		public:
			explicit VMEControllerParser(xercesc::DOMNode * pNode, int crate = 0);
			
			inline VMEController *getController() { return vmeController_; }
		
		private:
			VMEController *vmeController_;
		};

	}
}

#endif






