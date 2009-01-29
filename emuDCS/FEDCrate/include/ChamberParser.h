/*****************************************************************************\
* $Id: ChamberParser.h,v 1.10 2009/01/29 15:31:22 paste Exp $
*
* $Log: ChamberParser.h,v $
* Revision 1.10  2009/01/29 15:31:22  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 1.9  2008/09/19 16:53:51  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 1.8  2008/08/26 13:40:08  paste
* Updating and adding documentation
*
* Revision 1.6  2008/08/15 10:40:20  paste
* Working on fixing CAEN controller opening problems
*
* Revision 1.4  2008/08/15 08:35:50  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __CHAMBERPARSER_H__
#define __CHAMBERPARSER_H__

#include "Parser.h"
#include "FEDException.h"

namespace emu {
	namespace fed {

		class Chamber;

		/** @class ChamberParser A parser that builds Chamber objects to be loaded into the DDU.
		*	@sa Chamber
		**/
		class ChamberParser: public Parser
		{
		
		public:

			/** Default constructor.
			*
			*	@param pNode the XML DOM element node to parse.
			**/
			explicit ChamberParser(xercesc::DOMElement *pNode)
			throw (ParseException);

			/** @returns a pointer to the parsed Chamber object. **/
			inline Chamber *getChamber() { return chamber_; }

			/** @returns whether or not the chamber is marked to be masked off (killed) at the DDU. **/
			inline const bool isKilled() { return killed_; }

			/** @returns the parsed fiber number of the chamber. **/
			inline const unsigned int getFiber() { return fiber_; }
		
		private:

			/// A Chamber object built from the parsed attributes of the DOM node.
			Chamber *chamber_;

			/// Whether or not the fiber is killed.
			bool killed_;

			/// The fiber number of the chamber.
			unsigned int fiber_;
		};

	}
}

#endif
