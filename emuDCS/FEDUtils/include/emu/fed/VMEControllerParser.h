/*****************************************************************************\
* $Id: VMEControllerParser.h,v 1.1 2009/03/05 16:07:52 paste Exp $
*
* $Log: VMEControllerParser.h,v $
* Revision 1.1  2009/03/05 16:07:52  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
*
* Revision 3.6  2009/01/29 15:31:23  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 3.5  2008/09/19 16:53:51  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.4  2008/08/15 10:40:20  paste
* Working on fixing CAEN controller opening problems
*
* Revision 3.2  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __EMU_FED_VMECONTROLLERPARSER_H__
#define __EMU_FED_VMECONTROLLERPARSER_H__

#include "emu/fed/Parser.h"
#include "emu/fed/Exception.h"

namespace emu {
	namespace fed {

		class VMEController;

		/** @class DDUParser A parser that builds DDU objects to be loaded into a FEDCrate.
		*	@sa VMEController
		**/
		class VMEControllerParser : public Parser
		{
		
		public:

			/** Default constructor.
			*
			*	@param pNode the XML DOM element node to parse.
			**/
			explicit VMEControllerParser(xercesc::DOMElement *pNode)
			throw (emu::fed::exception::ParseException);

			/** @returns a pointer to the parsed VMEController object. **/
			inline VMEController *getController() { return vmeController_; }
		
		private:

			/// A VMEController object built from the parsed attributes of the DOM node.
			VMEController *vmeController_;
		};

	}
}

#endif






