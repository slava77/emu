/*****************************************************************************\
* $Id: Chamber.h,v 1.6 2008/08/15 10:23:01 paste Exp $
*
* $Log: Chamber.h,v $
* Revision 1.6  2008/08/15 10:23:01  paste
* Fixed EmuFCrateLoggable function definitions
*
* Revision 1.5  2008/08/15 08:35:50  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __CHAMBER_H__
#define __CHAMBER_H__

#include <string>

namespace emu {
	namespace fed {

		class Chamber
		{
		public:
			friend class ChamberParser; // Friends can access my private members.
		
			Chamber();
			~Chamber();
		
			std::string endcap;
			int station;
			int type;
			int number;
		
			std::string name();
			std::string peripheralCrate();
			std::string fiberCassette();
		
		private:
			/* PGK We might need to make these visible to the outside world at some point.
			For now, though I don't see why they can't be private. */
			int fiberCassetteCrate_;
			int fiberCassettePos_;
			std::string fiberCassetteSocket_;
		
			int peripheralCrateId_;
			int peripheralCrateVMECrate_;
			int peripheralCrateVMESlot_;
		
		};

	}
}

#endif
