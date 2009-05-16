/*****************************************************************************\
* $Id: Chamber.h,v 1.4 2009/05/16 18:54:26 paste Exp $
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
			
			/** Constructor used to set all the variables **/
			Chamber(unsigned int plusMinus, unsigned int station, unsigned int ring, unsigned int number, unsigned int pCrateTriggerSector = 0);
			
			/** Constructor with a string for the endcap **/
			Chamber(std::string endcap, unsigned int station, unsigned int ring, unsigned int number, unsigned int pCrateTriggerSector = 0);

			/** @returns a human-readable string naming the chamber, like "+1/2/33". **/
			inline std::string name() { return name_; }

			/** @returns a human-readable string naming the peripheral crate where the DMB reading out the chamber is located, like "VMEp1_2".  **/
			inline std::string peripheralCrateName() { return peripheralCrateName_; }

			/// The endcap where the chamber is located.  Is either "+" or "-".
			inline std::string getEndcap() { return endcap_; }

			/// The endcap where the chamber is located.  Is either 1 or 2.
			inline unsigned int getPlusMinus() { return plusMinus_; }

			/// The station where the chamber is located.  Is a number 1, 2, 3, or 4.
			inline unsigned int getStation() { return station_; }

			/// The "ring" of the chamber, or on which annulus it is located.  Is a number 1, 2, or 3.
			inline unsigned int getRing() { return ring_; }

			/// The chamber number.  The combination of endcap, station, type, and number uniquely defines each chamber.
			inline unsigned int getNumber() { return number_; }

		private:
		
			/// The name of the chamber (so I don't have to regenerate it every time)
			std::string name_;

			/// The name of the peripheral crate
			std::string peripheralCrateName_;
			
			/// The endcap (+ or -)
			std::string endcap_;
			
			/// Whether this is the plus or minus side (1 or 2, matching CMSSW's notation)
			unsigned int plusMinus_;
			
			/// The station
			unsigned int station_;
			
			/// The ring
			unsigned int ring_;
			
			/// The chamber number
			unsigned int number_;

		};

	}
}

#endif
