/*****************************************************************************\
* $Id: Manager.h,v 1.6 2009/07/06 15:50:10 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_MANAGER_H__
#define __EMU_FED_MANAGER_H__

#include <string>
#include <vector>

#include "emu/fed/Application.h"
#include "emu/fed/Supervised.h"
#include "emu/base/WebReporter.h"
#include "emu/fed/JSONSpiritValue.h"

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
			
			/** Get the status of the FSM and report in JSON **/
			void webGetSystemStatus(xgi::Input *in, xgi::Output *out);
			
			/** Get the status of the child Communicator applications and report in JSON **/
			void webGetCommunicatorStatus(xgi::Input *in, xgi::Output *out);
			
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
			*	@returns a string containing the state of the Communicators if they are consistant, "Indefinite" if they are inconsistant, and "Failed" if at least one is in a "Failed" state.
			**/
			std::string getManagerState(const std::string &targetState, const JSONSpirit::Array &underlyingStatus);
			
			/** Get the status of the underlying Communicator applications
			*
			*	@returns a JSON array of application object containing things such as the state of the Communicator, its URL, the associated monitor's URL, etc.
			**/
			JSONSpirit::Array getUnderlyingStatus();
			
			/** Convert an xdata parameter from a "getParameters" message into a "string":"value" JSON pair.
			*
			* @param message the response from the "getParameters" message
			* @param name the name of the variable to extract
			* @param defaultValue the default value for the parameter to take if everything fails
			*
			* @author Phillip Killewald
			**/
			template<typename T, typename V>
			JSONSpirit::Pair toJSONPair(xoap::MessageReference message, const std::string &name, V defaultValue)
			{
				V value = defaultValue;
				try {
					value = readParameter<T>(message, name);
				} catch (emu::fed::exception::SOAPException &e) {
					std::ostringstream error;
					error << "Unable to read parameter '" << name << "'";
					LOG4CPLUS_WARN(getApplicationLogger(), error.str());
					XCEPT_DECLARE_NESTED(emu::fed::exception::SOAPException, e2, error.str(), e);
					notifyQualified("WARN", e2);
				}
				return JSONSpirit::Pair(name, value);
			}

			/// The TTS ID with which to communicate (sent from above) for FMM tests.
			xdata::Integer ttsID_;

			/// The target TTS bits to set for FMM tests.
			xdata::Integer ttsBits_;
			
			/// This is technically a different variable from the 

		};
	}
}

#endif
