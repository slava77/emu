/*****************************************************************************\
* $Id: Communicator.h,v 1.1 2009/03/05 16:18:24 paste Exp $
*
* $Log: Communicator.h,v $
* Revision 1.1  2009/03/05 16:18:24  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
* * Added ForPageOne functionality to the Manager
*
* Revision 3.21  2009/01/29 15:31:22  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 3.20  2008/10/22 20:23:57  paste
* Fixes for random FED software crashes attempted.  DCC communication and display reverted to ancient (pointer-based communication) version at the request of Jianhui.
*
* Revision 3.19  2008/10/13 11:56:40  paste
* Cleaned up some of the XML config files and scripts, added more SVG, changed the DataTable object to inherit from instead of contain stdlib objects (experimental)
*
* Revision 3.18  2008/09/24 18:38:38  paste
* Completed new VME communication protocols.
*
* Revision 3.17  2008/08/18 08:30:14  paste
* Update to fix error propagation from IRQ threads to CommunicatorManager.
*
* Revision 3.15  2008/08/15 16:14:50  paste
* Fixed threads (hopefully).
*
* Revision 3.14  2008/08/15 10:40:20  paste
* Working on fixing CAEN controller opening problems
*
* Revision 3.12  2008/08/15 08:35:50  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __EMU_FED_COMMUNICATOR_H__
#define __EMU_FED_COMMUNICATOR_H__

#include "Application.h"

#include "xdata/xdata.h"
#include <vector>
#include <string>

#include "emu/fed/Crate.h"
#include "emu/fed/IRQThreadManager.h"


namespace emu {
	namespace fed {
		/** @class Communicator A class that is directly responsible for hardware communication with the FED Crates. **/
		class Communicator : public emu::fed::Application
		{

		public:
			XDAQ_INSTANTIATOR();

			/** Default constructor **/
			Communicator(xdaq::ApplicationStub *stub);

			// HyperDAQ pages
			/** Default HyperDAQ page **/
			void webDefault(xgi::Input *in, xgi::Output *out);

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
			void inline stateChanged(toolbox::fsm::FiniteStateMachine &fsm) { return emu::fed::Application::stateChanged(fsm); }

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
			
			//xdata::String errorChambers_;
			
			/// The target crate for TTS tests.
			xdata::UnsignedInteger ttsCrate_;
			
			/// The target slot for TTS tests.
			xdata::UnsignedInteger ttsSlot_;
			
			/// The target FMM bits for TTS tests.
			xdata::Integer ttsBits_;
			
			/// A manager that takes care of FMM interrupt handling.
			IRQThreadManager *TM_;
			
			/// Whether or not the application has been configured via SOAP or via the web interface.
			bool soapConfigured_;
			
			//xdata::Vector<xdata::Vector<xdata::UnsignedInteger> > dccInOut_;
			//xdata::Vector<xdata::UnsignedInteger> dduNumbers_;
			//xdata::Vector<xdata::UnsignedInteger> dccNumbers_;
			//xdata::Vector<xdata::UnsignedInteger> cscNumbers_;
			
			/// Number of chambers with errors (for PageOne)
			xdata::UnsignedInteger chambersWithErrors_;
			
			/// The crates that this application controls.
			std::vector<Crate *> crateVector_;

		};
	}
}

#endif

