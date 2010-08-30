/*****************************************************************************\
* $Id: MicroCommander.h,v 1.1 2010/08/30 17:24:29 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_MICROCOMMANDER_H__
#define __EMU_FED_MICROCOMMANDER_H__

#include <vector>
#include <map>
#include <time.h>

#include "emu/fed/Configurable.h"
#include "emu/base/FactFinder.h"
#include "emu/fed/Exception.h"

namespace emu {
	namespace fed {

		/** @class MicroCommander An XDAq application for sending and receiving data to and from the FED crate in a completely safe and atomic way
		*
		*	@author Phillip Killewald
		**/
		class MicroCommander: public virtual emu::fed::Configurable, public emu::base::FactFinder
		{

		public:
			XDAQ_INSTANTIATOR();

			/** Default Constructor **/
			MicroCommander(xdaq::ApplicationStub *stub);

			/** Default destructor **/
			~MicroCommander();

			/** Returns a requested fact **/
			virtual emu::base::Fact findFact(const emu::base::Component& component, const std::string& factType);

			/** Returns all facts for dispatch, either by schedule or by explicit call to sendFacts() **/
			virtual emu::base::FactCollection findFacts();

		};

	}
}

#endif
