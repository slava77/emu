/*****************************************************************************\
* $Id: FEDCrateParser.h,v 3.9 2009/01/29 15:31:22 paste Exp $
*
* $Log: FEDCrateParser.h,v $
* Revision 3.9  2009/01/29 15:31:22  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 3.8  2008/09/19 16:53:51  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.7  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __FEDCRATEPARSER_H__
#define __FEDCRATEPARSER_H__

#include <vector>
#include <string>

#include "FEDException.h"

namespace emu {
	namespace fed {

		class FEDCrate;

		/** @class FEDCrateParser A utility class that will parse a FED configuration XML file and build
		*	from it a vector of FEDCrates.
		*	@sa FEDCrate
		**/
		class FEDCrateParser {
		
		public:

			/** Default constructor.
			*
			*	@param fileName the name of the XML file to parse.
			**/
			explicit FEDCrateParser(const char* fileName)
			throw (ParseException, FileException);

			/** @returns a vector of the crates parsed from the XML file. **/
			inline std::vector<FEDCrate *> getCrates() { return crateVector_; }

			/** @returns the name of this collection of FED crates parsed from the file. **/
			inline std::string getName() { return name_; }
		
		private:

			/// The crates as parsed from the XML file, filled with the appropriate objects.
			std::vector<FEDCrate *> crateVector_;

			/// The name of the collection of FEDCrates from the XML file (typically the endcap).
			std::string name_;
		};

	}
}

#endif






