/*****************************************************************************\
* $Id: CrateParser.h,v 1.1 2009/03/05 16:07:52 paste Exp $
*
* $Log: CrateParser.h,v $
* Revision 1.1  2009/03/05 16:07:52  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
*
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
#ifndef __EMU_FED_CRATEPARSER_H__
#define __EMU_FED_CRATEPARSER_H__

#include <vector>
#include <string>

#include "emu/fed/Exception.h"

namespace emu {
	namespace fed {

		class Crate;

		/** @class CrateParser A utility class that will parse a FED configuration XML file and build
		*	from it a vector of FEDCrates.
		*	@sa Crate
		**/
		class CrateParser {
		
		public:

			/** Default constructor.
			*
			*	@param fileName the name of the XML file to parse.
			**/
			explicit CrateParser(const char* fileName)
			throw (emu::fed::exception::ParseException, emu::fed::exception::FileException);

			/** @returns a vector of the crates parsed from the XML file. **/
			inline std::vector<Crate *> getCrates() { return crateVector_; }

			/** @returns the name of this collection of FED crates parsed from the file. **/
			inline std::string getName() { return name_; }
		
		private:

			/// The crates as parsed from the XML file, filled with the appropriate objects.
			std::vector<Crate *> crateVector_;

			/// The name of the collection of FEDCrates from the XML file (typically the endcap).
			std::string name_;
		};

	}
}

#endif






