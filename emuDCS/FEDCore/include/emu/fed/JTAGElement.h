/*****************************************************************************\
* $Id: JTAGElement.h,v 1.1 2009/03/05 16:02:14 paste Exp $
*
* $Log: JTAGElement.h,v $
* Revision 1.1  2009/03/05 16:02:14  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
*
* Revision 1.2  2008/09/24 18:42:28  paste
* Fixing JTAGElement double-checkin problem.
*
*
\*****************************************************************************/
#ifndef __EMU_FED_JTAGELEMENT_H__
#define __EMU_FED_JTAGELEMENT_H__

#include <string>
#include <list>

#include "emu/fed/JTAG_constants.h"

namespace emu {
	namespace fed {

		/** @class JTAGElement
		 * A class defining one element in a (possibly) multi-element JTAG
		 * chain.  Works in tandem with a bidirectional linked-list.
		 *
		 * @author Phillip Killewald
		 **/
		class JTAGElement {

		public:

			JTAGElement(std::string myName, enum DEVTYPE myDev, int myChannel, int16_t myBypassCommand, unsigned int myCmdBits, int32_t myBitCode, bool myDirectVME):
				name(myName),
				dev(myDev),
				channel(myChannel),
				bypassCommand(myBypassCommand),
				cmdBits(myCmdBits),
				bitCode(myBitCode),
				directVME(myDirectVME)
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
			
		};

		typedef std::list<JTAGElement *> JTAGChain;
	
	}

}

#endif
