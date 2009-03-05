/*****************************************************************************\
* $Id: Chamber.h,v 1.1 2009/03/05 16:02:14 paste Exp $
*
* $Log: Chamber.h,v $
* Revision 1.1  2009/03/05 16:02:14  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
*
* Revision 1.10  2009/01/29 15:31:22  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 1.9  2008/09/19 16:53:51  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 1.8  2008/08/26 13:40:08  paste
* Updating and adding documentation
*
* Revision 1.6  2008/08/15 10:23:01  paste
* Fixed EmuFCrateLoggable function definitions
*
* Revision 1.5  2008/08/15 08:35:50  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __EMU_FED_CHAMBER_H__
#define __EMU_FED_CHAMBER_H__

#include <string>

namespace emu {
	namespace fed {

		/** @class Chamber A class for easy access to chamber names and locations. **/
		class Chamber
		{
		public:
		
			friend class ChamberParser;

			/** Default constructor **/
			Chamber();

			/** @returns a human-readable string naming the chamber, like "+1/2/33". **/
			std::string name();

			/** @returns a human-readable string naming the peripheral crate where the DMB reading out the chamber is located, like "VMEp1_2".  **/
			std::string peripheralCrate();
		
			/// The endcap where the chamber is located.  Is either "+" or "-".
			std::string endcap;

			/// The endcap where the chamber is located.  Is either +1 or -1.
			int plusMinus;

			/// The station where the chamber is located.  Is a number 1, 2, 3, or 4.
			unsigned int station;

			/// The "type" of the chamber, or on which annulus it is located.  Is a number 1, 2, or 3.
			unsigned int type;

			/// The chamber number.  The combination of endcap, station, type, and number uniquely defines each chamber.
			unsigned int number;
			
			// @returns a human-readable string naming the fiber cassette where the DMB-to-DDU fiber is connected, like "1/2/c"
			//std::string fiberCassette();
			
		private:

			//int fiberCassetteCrate_;
			//int fiberCassettePos_;
			//std::string fiberCassetteSocket_;
			
			//int peripheralCrateId_;

			/// The peripheral crate ID number where this chamber is connected.
			unsigned int peripheralCrateVMECrate_;
			
			//int peripheralCrateVMESlot_;
		
		};

	}
}

#endif
