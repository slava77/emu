/*****************************************************************************\
* $Id: Communicator.h,v 1.4 2009/05/20 18:18:38 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_COMMUNICATOR_H__
#define __EMU_FED_COMMUNICATOR_H__

#include "Application.h"
#include "Supervised.h"

#include "xdata/xdata.h"
#include <vector>
#include <string>

#include "emu/fed/Crate.h"
#include "emu/fed/IRQThreadManager.h"


namespace emu {
	namespace fed {
		/** @class Communicator A class that is directly responsible for hardware communication with the FED Crates. **/
		class Communicator : public emu::fed::Application, public emu::fed::Supervised
		{

		public:
			XDAQ_INSTANTIATOR();

			/** Default constructor **/
			Communicator(xdaq::ApplicationStub *stub);

			// HyperDAQ pages
			/** Default HyperDAQ page **/
			void webDefault(xgi::Input *in, xgi::Output *out);
			
			/** Get the status of the application and report in JSON **/
			void webGetStatus(xgi::Input *in, xgi::Output *out);
			
			/** Change the type of configuration being used **/
			void webChangeConfigMode(xgi::Input *in, xgi::Output *out);
			
			/** Change the name of the XML file used in configuring **/
			void webChangeXMLFile(xgi::Input *in, xgi::Output *out);

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
			/** Sets TTS bits defined by ttsBits_ on the crate ttsCrate_, slot ttsSlot_, but only if that crate and slot are under the command of this instance of the Communicator application **/
			xoap::MessageReference onSetTTSBits(xoap::MessageReference message);

			/** Serializes the appropriate variables to send to whatever application requests them. **/
			xoap::MessageReference onGetParameters(xoap::MessageReference message);

		private:

			/** Sets the TTS (FMM) bits on a given board.
			*
			*	@param crate is the crate on which to set the bits
			*	@param slot is the slot number on which to set the bits
			*	@param bits are the bits to set
			*
			*	@note This method will not set bits if the crate/slot combination is not under the command of this instance of the Communicator application. **/
			void writeTTSBits(unsigned int crate, unsigned int slot, int bits)
			throw (emu::fed::exception::TTSException);

			/** Reads back the RSS (FMM) bits from a given board.
			*
			*	@param crate is the crate on which to sread the bits
			*	@param slot is the slot number on which to read the bits
			*
			*	@note This method will not read bits if the crate/slot combination is not under the command of this instance of the Communicator application. **/
			int readTTSBits(unsigned int crate, unsigned int slot)
			throw (emu::fed::exception::TTSException);
			
			/// The XML configuration file name.
			xdata::String xmlFile_;
			
			/// The username for database communication.
			xdata::String dbUsername_;
			
			/// The password for database communication.
			xdata::String dbPassword_;
			
			//xdata::String errorChambers_;
			
			/// The target crate for TTS tests.
			xdata::UnsignedInteger ttsCrate_;
			
			/// The target slot for TTS tests.
			xdata::UnsignedInteger ttsSlot_;
			
			/// The target FMM bits for TTS tests.
			xdata::Integer ttsBits_;
			
			/// The way this application is set to be configured
			xdata::String configMode_;
			
			/// A manager that takes care of FMM interrupt handling.
			IRQThreadManager *TM_;
			
			//xdata::Vector<xdata::Vector<xdata::UnsignedInteger> > dccInOut_;
			//xdata::Vector<xdata::UnsignedInteger> dduNumbers_;
			//xdata::Vector<xdata::UnsignedInteger> dccNumbers_;
			//xdata::Vector<xdata::UnsignedInteger> cscNumbers_;
			
			/// Number of chambers with errors (for Page One)
			xdata::UnsignedInteger fibersWithErrors_;
			
			/// The current DDU to DCC input rates
			xdata::Float totalDCCInputRate_;
			
			/// The current DCC to S-Link output rates
			xdata::Float totalDCCOutputRate_;
			
			/// The threshold number of chambers required to be in an error state before sending an FMM
			xdata::UnsignedInteger fmmErrorThreshold_;
			
			/// The crates that this application controls.
			std::vector<Crate *> crateVector_;

		};
	}
}

#endif

