/*****************************************************************************\
* $Id: JTAGElement.h,v 1.3 2009/07/01 14:17:18 paste Exp $
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

			JTAGElement(const std::string &myName, const enum DEVTYPE &myDev, const int &myChannel, const int16_t &myBypassCommand, const unsigned int &myCmdBits, const int32_t &myBitCode, const bool &myDirectVME):
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
