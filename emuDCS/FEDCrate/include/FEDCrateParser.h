/*****************************************************************************\
* $Id: FEDCrateParser.h,v 3.8 2008/09/19 16:53:51 paste Exp $
*
* $Log: FEDCrateParser.h,v $
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

#include "EmuFEDLoggable.h"

namespace emu {
	namespace fed {

		class FEDCrate;

		class FEDCrateParser: public EmuFEDLoggable {
		
		public:
			FEDCrateParser() {}
			
			/** Parse the file
				@param name File Name
			*/
			void parseFile(const char* name);
			
			/* There has to be a way to get the crates in the XML
			back to the user, so that selectCrates will work. */
			inline std::vector<FEDCrate *> getCrates() { return crateVector_; }
			inline std::string getName() { return name_; }
		
		private:
			std::vector<FEDCrate *> crateVector_;
			std::string name_;
		};

	}
}

#endif






