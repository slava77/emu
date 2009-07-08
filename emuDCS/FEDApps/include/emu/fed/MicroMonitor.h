/*****************************************************************************\
* $Id: MicroMonitor.h,v 1.2 2009/07/08 12:03:09 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_MICROMONITOR_H__
#define __EMU_FED_MICROMONITOR_H__

#include "emu/fed/Configurable.h"
#include "emu/fed/Supervised.h"
#include "emu/fed/Exception.h"

namespace emu {
	namespace fed {
		
		/** @class Monitor An XDAq application for monitoring only vital information from the FED crates for passing data to DCS.
		*
		*	@author Phillip Killewald
		**/
		class MicroMonitor: public virtual emu::fed::Configurable, public emu::fed::Supervised
		{
			
		public:
			XDAQ_INSTANTIATOR();
			
			/** Default Constructor **/
			MicroMonitor(xdaq::ApplicationStub *stub);
			
			/** Default destructor **/
			~MicroMonitor();
			
			// HyperDAQ pages
			void DCSOutput(xgi::Input *in, xgi::Output *out);
			
			// FSM transition call-back functions
			/** Send the 'Configure' command to the crates **/
			void configureAction(toolbox::Event::Reference event)
			throw (toolbox::fsm::exception::Exception);
			
			/** Send the 'Enable' command to the crates **/
			void enableAction(toolbox::Event::Reference event)
			throw (toolbox::fsm::exception::Exception);
			
			/** Send the 'Disable' command to the crates **/
			void disableAction(toolbox::Event::Reference event)
			throw (toolbox::fsm::exception::Exception);
			
			/** Send the 'Halt' command to the crates **/
			void haltAction(toolbox::Event::Reference event)
			throw (toolbox::fsm::exception::Exception);
			
			// SOAP call-back functions that send FSM transitions
			/** Start the FSM 'Configure' transition **/
			DEFINE_DEFAULT_SOAP2FSM_ACTION(Configure);
			
			/** Start the FSM 'Enable' transition **/
			DEFINE_DEFAULT_SOAP2FSM_ACTION(Enable);
			
			/** Start the FSM 'Disable' transition **/
			DEFINE_DEFAULT_SOAP2FSM_ACTION(Disable);
			
			/** Start the FSM 'Halt' transition **/
			DEFINE_DEFAULT_SOAP2FSM_ACTION(Halt);
			
			// FSM state change call-back function
			/** Decault FSM state change call-back function **/
			void inline stateChanged(toolbox::fsm::FiniteStateMachine &fsm) { return emu::fed::Supervised::stateChanged(fsm); }
			
			
		private:
			
		};
		
	}
}

#endif
