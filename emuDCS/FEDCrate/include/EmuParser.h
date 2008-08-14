//-----------------------------------------------------------------------
// $Id: EmuParser.h,v 3.2 2008/08/14 14:14:32 paste Exp $
// $Log: EmuParser.h,v $
// Revision 3.2  2008/08/14 14:14:32  paste
// Adding things to namespace emu::fed, condensing include files, renaming VMEParser.
//
// Revision 3.1  2008/08/11 15:24:34  paste
// More updates to clean up files, preparing for universal logger and presentation of code.
//
// Revision 3.0  2006/07/20 21:16:10  geurts
// *** empty log message ***
//
// Revision 1.3  2006/05/09 19:20:02  gilmore
// Fix for DCC configure function.
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef EmuParser_h
#define EmuParser_h

#include <xercesc/dom/DOM.hpp>
#include <string>
#include "EmuFEDLoggable.h"

namespace emu {
	namespace fed {

		class EmuParser: public EmuFEDLoggable
		{
		public:
			EmuParser() {}
			virtual ~EmuParser() {}
			
			void parseNode(xercesc::DOMNode * pNode);
			void fillInt(std::string item, int & target);
			void fillHex(std::string item, int & target);
			void fillFloat(std::string item, float & target);
			void fillString(std::string item, std::string & target);
		
		protected:
			xercesc::DOMNamedNodeMap * pAttributes_;
			int size_;
		
		};

	}
}

#endif

