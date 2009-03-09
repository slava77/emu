/*****************************************************************************\
* $Id: Manager.h,v 1.2 2009/03/09 16:03:16 paste Exp $
*
* $Log: Manager.h,v $
* Revision 1.2  2009/03/09 16:03:16  paste
* * Updated "ForPage1" routine in Manager with new routines from emu::base::WebReporter
* * Updated inheritance in wake of changes to emu::base::Supervised
* * Added Supervised class to separate XDAQ web-based applications and those with a finite state machine
*
* Revision 1.1  2009/03/05 16:18:24  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
* * Added ForPageOne functionality to the Manager
*
* Revision 1.18  2009/01/29 15:31:22  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 1.17  2008/10/09 11:21:19  paste
* Attempt to fix DCC MPROM load.  Added debugging for "Global SOAP death" bug.  Changed the debugging interpretation of certain DCC registers.  Added inline SVG to EmuFCrateManager page for future GUI use.
*
* Revision 1.16  2008/08/25 12:25:49  paste
* Major updates to VMEController/VMEModule handling of CAEN instructions.  Also, added version file for future RPMs.
*
* Revision 1.15  2008/08/15 10:40:20  paste
* Working on fixing CAEN controller opening problems
*
* Revision 1.13  2008/08/15 08:35:50  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __EMU_FED_MANAGER_H__
#define __EMU_FED_MANAGER_H__

#include <string>
#include <vector>

#include "Application.h"
#include "Supervised.h"
#include "emu/base/WebReporter.h"

namespace emu {
	namespace fed {
		/** @class Manager An XDAq application class that relays commands from the CSCSupervisor class
		*	to the individual Communicator classes.
		**/
		class Manager : public emu::fed::Application, public emu::fed::Supervised, public emu::base::WebReporter
		{

		public:
			XDAQ_INSTANTIATOR();

			/** Default constructor **/
			Manager(xdaq::ApplicationStub *stub);

			// HyperDAQ pages
			/** Default HyperDAQ page **/
			void webDefault(xgi::Input *in, xgi::Output *out);
			
			/** Combines information from the Communicators and gives to CSCPageOne **/
			std::vector<emu::base::WebReportItem> materialToReportOnPage1();

			// FSM transition call-back functions
			/** Send the 'Configure' command to the Communicator applications **/
			void configureAction(toolbox::Event::Reference event)
			throw (toolbox::fsm::exception::Exception);

			/** Send the 'Enable' command to the Communicator applications **/
			void enableAction(toolbox::Event::Reference event)
			throw (toolbox::fsm::exception::Exception);

			/** Send the 'Disable' command to the Communicator applications **/
			void disableAction(toolbox::Event::Reference event)
			throw (toolbox::fsm::exception::Exception);

			/** Send the 'Halt' command to the Communicator applications **/
			void haltAction(toolbox::Event::Reference event)
			throw (toolbox::fsm::exception::Exception);

			/** Transition into an unknown FSM state because the underlying Communicator applications' FSM states do not agree **/
			void unknownAction(toolbox::Event::Reference event);

			// FSM state change call-back function
			/** Decault FSM state change call-back function **/
			void inline stateChanged(toolbox::fsm::FiniteStateMachine &fsm) { return emu::fed::Supervised::stateChanged(fsm); }

			// SOAP call-back functions that send FSM transitions
			/** Start the FSM 'Configure' transition **/
			DEFINE_DEFAULT_SOAP2FSM_ACTION(Configure);

			/** Start the FSM 'Enable' transition **/
			DEFINE_DEFAULT_SOAP2FSM_ACTION(Enable);

			/** Start the FSM 'Disable' transition **/
			DEFINE_DEFAULT_SOAP2FSM_ACTION(Disable);

			/** Start the FSM 'Halt' transition **/
			DEFINE_DEFAULT_SOAP2FSM_ACTION(Halt);

			// Other SOAP call-back functions
			/** Set the TTS bits on the underlying Communicator applications as defined by members ttsID_ and ttsBits_ **/
			xoap::MessageReference onSetTTSBits(xoap::MessageReference message);

		private:

			/** Get the combined FSM states of the underlying Communicator applications.
			*
			*	@param targetState the state that you home the underlying Communicator applications are in.
			*
			*	@returns a string containing the state of the Communicators if they are consistant, "Unknown" if they are inconsistant, and "Failed" if at least one is in a "Failed" state.
			**/
			std::string getUnderlyingStates(std::string targetState);

			/// The TTS ID with which to communicate (sent from above) for FMM tests.
			xdata::Integer ttsID_;

			/// The target TTS bits to set for FMM tests.
			xdata::Integer ttsBits_;

		};
	}
}

#endif
