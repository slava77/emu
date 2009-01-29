/*****************************************************************************\
* $Id: DDUParser.h,v 3.9 2009/01/29 15:31:22 paste Exp $
*
* $Log: DDUParser.h,v $
* Revision 3.9  2009/01/29 15:31:22  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 3.8  2008/09/19 16:53:51  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.7  2008/08/15 10:40:20  paste
* Working on fixing CAEN controller opening problems
*
* Revision 3.5  2008/08/15 08:35:50  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __DDUPARSER_H__
#define __DDUPARSER_H__

#include "Parser.h"
#include "FEDException.h"

namespace emu {

	namespace fed {

		class DDU;

		/** @class DDUParser A parser that builds DDU objects to be loaded into a FEDCrate.
		*	@sa DDU
		**/
		class DDUParser: public Parser
		{
		
		public:

			/** Default constructor.
			*
			*	@param pNode the XML DOM element node to parse.
			**/
			explicit DDUParser(xercesc::DOMElement *pNode)
			throw (ParseException);
		
			/** @returns a pointer to the parsed DDU object. **/
			inline DDU *getDDU() { return ddu_; }

			/** @returns the parsed options. **/
			inline int getOptions() { return options_; }
		
		private:

			/// A DDU object built from the parsed attributes of the DOM node.
			DDU *ddu_;

			/// The parsed options.
			int options_;
		};

	}
}

#endif






