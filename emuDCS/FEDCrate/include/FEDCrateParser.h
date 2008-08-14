//-----------------------------------------------------------------------
// $Id: FEDCrateParser.h,v 3.6 2008/08/14 14:14:32 paste Exp $
// $Log: FEDCrateParser.h,v $
// Revision 3.6  2008/08/14 14:14:32  paste
// Adding things to namespace emu::fed, condensing include files, renaming VMEParser.
//
// Revision 3.5  2008/08/13 14:20:41  paste
// Massive update removing "using namespace" code and cleaning out stale header files as preparation for RPMs.
//
// Revision 3.4  2008/08/11 15:24:34  paste
// More updates to clean up files, preparing for universal logger and presentation of code.
//
// Revision 3.3  2008/02/01 16:18:14  gilmore
// added fiber-to-CSCid map decode using local XML file as reference
//
// Revision 3.2  2007/07/23 05:02:22  gilmore
// major structural chages to improve multi-crate functionality
//
// Revision 3.0  2006/07/20 21:16:10  geurts
// *** empty log message ***
//
// Revision 1.2  2006/01/21 20:20:14  gilmore
// *** empty log message ***
//
// Revision 1.16  2004/06/11 09:38:17  tfcvs
// improved checks on top elements. print out warnings in case they do not
// comply. (FG)
//
//-----------------------------------------------------------------------
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
		
		private:
			std::vector<FEDCrate *> crateVector_;
		};

	}
}

#endif






