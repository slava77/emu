/*****************************************************************************\
* $Id: Communicator.h,v 1.17 2012/11/27 19:40:06 cvuosalo Exp $
\*****************************************************************************/
#ifndef __EMU_FED_COMMUNICATOR_H__
#define __EMU_FED_COMMUNICATOR_H__

#include "emu/fed/Configurable.h"
#include "emu/fed/Supervised.h"
#include "emu/base/FactFinder.h"
#include "emu/fed/Facts.h"

#include <string>
#include <list>

#include "emu/fed/Crate.h"


namespace emu {
	namespace fed {

		class IRQThreadManager;
		
		/** @class Communicator A class that is directly responsible for hardware communication with the FED Crates. **/
	        class Communicator: virtual public emu::fed::Configurable, public emu::fed::Supervised, public emu::base::FactFinder
		{
			
			friend class IRQThreadManager;  // To get around protection of sendFacts()

		public:
			XDAQ_INSTANTIATOR();

			/** Default constructor **/
			Communicator(xdaq::ApplicationStub *stub);
			
			/** Default destructor **/
			~Communicator();

			// HyperDAQ pages
			/** Default HyperDAQ page **/
			void webDefault(xgi::Input *in, xgi::Output *out);
			
			/** Get the status of the application and report in JSON **/
			void webGetStatus(xgi::Input *in, xgi::Output *out);
			
			/** Override the Configurable method to prevent software reconfigure while enabled. **/
			void webReconfigure(xgi::Input *in, xgi::Output *out);

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

			/** Returns a requested fact **/
			virtual emu::base::Fact findFact(const emu::base::Component& component, const std::string& factType);
			
			/** Returns all facts for dispatch, either by schedule or by explicit call to sendFacts() **/
			virtual emu::base::FactCollection findFacts();

			/** The Communicator itself is not fast enough to pick up IRQs, so the ThreadManager has to be able to send to the Communicator the facts from an IRQ so that the Communicator can report this to the Expert System. **/
			inline void storeFact(const emu::base::Fact &fact) {
				storedFacts_.push_back(fact);
			}

			/** Because FactFinder won't let me do this **/
			inline void sendFact(const std::string &name) {
				FactFinder::sendFact(std::string(""), name);
			}

		protected:

			/** Sends a reset to the DCC in the crate, which also
			* resets the DDU.
			*
			*	@param iCrate is the crate to reset
			*
			**/
			void resetCrate(std::vector<Crate *>::iterator iCrate)
			throw (toolbox::fsm::exception::Exception);

			/** If \c dduInPassthroughMode_, this sets all DDUs in the specified crate to passthrough mode
			*
			*
			*	@param crate is the crate in which to set the DDUs to passthrough mode
			*
			**/
		        void toggleDDUPassthroughMode( Crate* crate );

			/** Configures the FED crates
			**/
			void configureCrates()
			throw (toolbox::fsm::exception::Exception);

		private:

			/** Sets the TTS (FMM) bits on a given board.
			*
			*	@param crate is the crate on which to set the bits
			*	@param slot is the slot number on which to set the bits
			*	@param bits are the bits to set
			*
			*	@note This method will not set bits if the crate/slot combination is not under the command of this instance of the Communicator application. **/
			void writeTTSBits(const unsigned int crate, const unsigned int slot, const int bits)
			throw (emu::fed::exception::TTSException);

			/** Reads back the RSS (FMM) bits from a given board.
			*
			*	@param crate is the crate on which to sread the bits
			*	@param slot is the slot number on which to read the bits
			*
			*	@note This method will not read bits if the crate/slot combination is not under the command of this instance of the Communicator application. **/
			int readTTSBits(const unsigned int crate, const unsigned int slot)
			throw (emu::fed::exception::TTSException);
			
			/** Checks the FIFO and L1A status of the DCC
			*
			*	@param crateNum:  FED crate number
			*	@param myDCCs: list of DCCs in this crate (should be 1)
			*	@param exceptions: list of exceptions to be added to
			* */
			void chkDCCstatus(const unsigned int crateNum,
				std::vector<DCC *> &myDCCs, std::vector<xcept::Exception> &exceptions);

			/// The target crate for TTS tests.
			xdata::UnsignedInteger ttsCrate_;
			
			/// The target slot for TTS tests.
			xdata::UnsignedInteger ttsSlot_;
			
			/// The target FMM bits for TTS tests.
			xdata::Integer ttsBits_;
			
			/// A manager that takes care of FMM interrupt handling.
			IRQThreadManager *TM_;
			
			/// Number of chambers with errors (for Page1)
			xdata::UnsignedInteger fibersWithErrors_;
			
			/// The names of the fibers with errors as a space-separated list, for reporting to Page1
			xdata::String fiberNamesWithErrors_;
			
			/// The current DDU to DCC input rates
			xdata::Float totalDCCInputRate_;
			
			/// The current DCC to S-Link output rates
			xdata::Float totalDCCOutputRate_;

       		        /// If \e true, the DDUs are to generate a fake L1A on receiving an event.
		        xdata::Boolean dduInPassthroughMode_;
			
			/// The threshold number of chambers required to be in an error state before sending an FMM
			xdata::UnsignedInteger fmmErrorThreshold_;
			
			/// Lifetime of the dynamic ignore list of bad chambers -- either by "run" or "red" for red recycle
			xdata::String ignoreListLifetime_;
			
			/// Number of seconds thread should wait after releasing FMMs, default is 5
			xdata::UnsignedInteger waitTimeAfterFMM_;
			
			/// Stored facts to be forcefully sent to the MasterSystem as soon as there is a problem
			std::list<emu::base::Fact> storedFacts_;

		};
	}
}

#endif

