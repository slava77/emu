/*****************************************************************************\
* $Id: Chamber.h,v 1.3 2009/04/28 02:05:19 paste Exp $
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

			/// The "ring" of the chamber, or on which annulus it is located.  Is a number 1, 2, or 3.
			unsigned int ring;

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
