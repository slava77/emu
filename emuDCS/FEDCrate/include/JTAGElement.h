/*****************************************************************************\
* $Id: JTAGElement.h,v 1.1 2008/09/19 23:13:59 paste Exp $
*
* $Log: JTAGElement.h,v $
* Revision 1.1  2008/09/19 23:13:59  paste
* Fixed a small bug in disabling of error reporting, added missing file.
*
*
\*****************************************************************************/
#ifndef __JTAGELEMENT_H__
#define __JTAGELEMENT_H__

#include <string>

#include "JTAG_constants.h"

namespace emu {
	namespace fed {

		/** @class JTAGElement
		 * A class defining one element in a (possibly) multi-element JTAG
		 * chain.  Works in tandem with a map like a unidirectional linked-list.
		 *
		 * @author Phillip Killewald
		 **/
		class JTAGElement {

		public:

			JTAGElement(std::string myName, int myChannel, int16_t myBypassCommand, unsigned int myCmdBits, int32_t myBitCode, bool myDirectVME, enum DEVTYPE myPreviousDevice):
				name(myName),
				channel(myChannel),
				bypassCommand(myBypassCommand),
				cmdBits(myCmdBits),
				bitCode(myBitCode),
				directVME(myDirectVME),
				previousDevice(myPreviousDevice)
			{}
			
			~JTAGElement() {}
			
			/// The human-readable name of the element
			std::string name;

			/// The DEVTYPE for easy recognition
			enum DEVTYPE dev;

			/// Part of the VME address
			int channel;
			
			/// The command used to bypass this element if need be
			int16_t bypassCommand;

			/// The number of bits in a command
			unsigned int cmdBits;

			/// A bit-code for addressing this particular device
			int32_t bitCode;

			/// Is this device accessable through a direct VME read?
			bool directVME;

			enum DEVTYPE previousDevice;
			
		};

	}

}

#endif
