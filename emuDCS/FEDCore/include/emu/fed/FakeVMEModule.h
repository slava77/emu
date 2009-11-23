/*****************************************************************************\
* $Id: FakeVMEModule.h,v 1.1 2009/11/23 07:24:31 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_FAKEVMEMODULE_H__
#define __EMU_FED_FAKEVMEMODULE_H__

#include "emu/fed/VMEModule.h"

namespace emu {

	namespace fed {

		class VMEController;

		struct JTAGElement;

		/** @class FakeVMEModule A inherited class for DCC and DDU classes.
		*	The FakeVMEModule will return random numbers for each read, and does nothing for writes.
		*/
		class FakeVMEModule: public VMEModule
		{
		public:

			/** Default constructor.
			*
			*	@param mySlot is the board's slot number in the crate (needed for proper VME communication.)
			**/
			FakeVMEModule(const unsigned int &mySlot);

			virtual ~FakeVMEModule() {};

		protected:

			/** Reads 16 bits from a given VME address.
			*
			*	@param myAddress the address from which to read the data.
			*
			*	@note The slot number should NOT be encoded in myAddress.
			**/
			uint16_t readVME(const uint32_t &myAddress, const bool &debug = false)
			throw (emu::fed::exception::CAENException);

			/** Writes 16 bits to a given VME address.
			*
			*	@param myAddress the address to which to send the data.
			*	@param data the data to send.
			*
			*	@note The slot number should NOT be encoded in myAddress.
			**/
			void writeVME(const uint32_t &myAddress, const uint16_t &data, const bool &debug = false)
			throw (emu::fed::exception::CAENException);

		private:

			/// The slot number of the device (its location within it respective VME crate).
			unsigned int slot_;

			/// The CAEN BHandle used to communicate to the controller of the crate containing this device.
			int16_t BHandle_;

		};

	}
}

#endif

