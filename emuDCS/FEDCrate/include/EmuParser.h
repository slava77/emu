/*****************************************************************************\
* $Id: EmuParser.h,v 3.3 2008/08/15 08:35:50 paste Exp $
*
* $Log: EmuParser.h,v $
* Revision 3.3  2008/08/15 08:35:50  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
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

