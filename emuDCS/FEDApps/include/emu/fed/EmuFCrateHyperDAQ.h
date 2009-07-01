/*****************************************************************************\
* $Id: EmuFCrateHyperDAQ.h,v 1.4 2009/07/01 14:54:03 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_EMUFCRATEHYPERDAQ_H__
#define __EMU_FED_EMUFCRATEHYPERDAQ_H__

#include <string>
#include <vector>
#include <utility> // pair
#include <sstream>
#include <map>

#include "xgi/Utils.h"
#include "xgi/Method.h"
#include "xdata/xdata.h"

#include "Application.h"
#include "emu/fed/Exception.h"
#include "emu/fed/Crate.h"

namespace emu {
	namespace fed {
	
		/** @class EmuFCrateHyperDAQ an XDAq-based interface for controlling and debugging FED Crates.
		*	Please, PLEASE leave the editing of this file to the experts!
		*
		*	@author Stan Durkin       \<durkin@mps.ohio-state.edu\>
		*	@author Jason Gilmore     \<gilmore@mps.ohio-state.edu\>
		*	@author Jianhui Gu        \<gujh@mps.ohio-state.edu\>
		*	@author Phillip Killewald \<paste@mps.ohio-state.edu\>
		**/
		class EmuFCrateHyperDAQ: public emu::fed::Application
		{

		public:
			XDAQ_INSTANTIATOR();

			/** Default Constructor **/
			EmuFCrateHyperDAQ(xdaq::ApplicationStub *stub);
			
			/** Default destructor **/
			~EmuFCrateHyperDAQ();

			/** Default page when XDAQ loads.  Sets configuration if required and
			*	bounces the user to the main page.
			*
			*	@param *in is a pointer to a standard xgi input object (for passing
			*	things like POST and GET variables to the function.)
			*	@param *out is the xgi output (basically, a stream that outputs to the
			*	browser window.)
			*
			*	@note The *in and *out parameters are common to all xgi-bound functions, and will herein not be
			*	included in the documentation.
			**/
			void webDefault(xgi::Input *in, xgi::Output *out);

			/** Page listing all the DDUs and the DCC along with their various options. **/
			void mainPage(xgi::Input *in, xgi::Output *out);

			/** Page listing all the configuration options. **/
			void configurePage(xgi::Input *in, xgi::Output *out);

			/** Loads the configuration file from raw textarea input. **/
			void setRawConfFile(xgi::Input *in, xgi::Output *out );

			/** Sets the configuration file from a text input std::string (should point to a
			*	file on the server-side disk that XDAQ can access.)
			**/
			void setConfFile(xgi::Input *in, xgi::Output *out);

			/** Uploads the configuration file from a file input (should point to a
			*	file on the client-side disk.)
			**/
			void uploadConfFile(xgi::Input *in, xgi::Output *out);

			/** Page listing the DDU firmware broadcast options. **/
			void DDUBroadcast(xgi::Input *in, xgi::Output *out);

			/** Uploads the broadcastable firmware from the client computer to the
			*	server computer.
			**/
			void DDULoadBroadcast(xgi::Input *in, xgi::Output *out);

			/** Broadcasts firmware to all DDUs simultaneously. **/
			void DDUSendBroadcast(xgi::Input *in, xgi::Output *out);

			/** Resets a particular Crate either cheating by using TTC, or the "real"
			*	way, by requesting a global reset.  The global reset method may or may
			*	not work ever.
			**/
			void DDUReset(xgi::Input *in, xgi::Output *out);

			/** Resets a particular Crate either cheating by using TTC, or the "real"
			*	way, by requesting a global reset.  The global reset method may or may
			*	not work ever.
			**/
			void DCCReset(xgi::Input *in, xgi::Output *out);

			/** Shows General DDU debugging information. **/
			void DDUDebug(xgi::Input *in, xgi::Output *out);

			/** Shows Expert DDU debugging information and commands. **/
			void DDUExpert(xgi::Input *in, xgi::Output *out);

			/** Shows the DDU InFPGA 0 and 1 status page and various communication options. **/
			void InFpga(xgi::Input *in, xgi::Output *out);

			/** Page for VME parallel register reading/writing. **/
			void VMEPARA(xgi::Input *in, xgi::Output *out);

			/** Page for VEM Serial register reading/writing. **/
			void VMESERI(xgi::Input *in, xgi::Output *out);

			/** Load data into the DDU **/
			void DDUTextLoad(xgi::Input *in, xgi::Output *out);

			/** Starts/stops/monitors the IRQ interrupt handler. **/
			void VMEIntIRQ(xgi::Input *in, xgi::Output *out);

			/** Page listing the DCC firmware broadcast options. **/
			void DCCBroadcast(xgi::Input *in, xgi::Output *out);

			/** Uploads the broadcastable firmware from the client computer to the
			*	server computer.
			**/
			void DCCLoadBroadcast(xgi::Input *in, xgi::Output *out);

			/** Broadcasts firmware to all DCCs simultaneously. **/
			void DCCSendBroadcast(xgi::Input *in, xgi::Output *out);

			/** Page for reading general debug information from the DCC. **/
			void DCCDebug(xgi::Input *in, xgi::Output *out);

			/** Page for reading and setting expert registers on the DCC. **/
			void DCCExpert(xgi::Input *in, xgi::Output *out);

			/** Load data into the DCC **/
			void DCCTextLoad(xgi::Input *in, xgi::Output *out);

			/** Live DDU voltage monitoring page. **/
			void DDUVoltMon(xgi::Input *in, xgi::Output *out);

		private:
			
			/** Actually configures the application to talk to the apporpriate crate
			*	(crates?).
			*
			**/
			void Configuring()
			throw (emu::fed::exception::ConfigurationException);

			/** My patented Select-a-crate/board
			*
			*	@author Phillip Killewald
			**/
			std::string selectACrate(const std::string &location, const std::string &what, const unsigned int index, const unsigned int crateIndex = 0);

			/** A way to easily get the selected crate from the CGI variables
			*
			* @param cgi the Cgicc object which to parse.
			* @returns a pair of the cgi integer value parsed from cgi and a
			* pointer to the target FEDCrate object
			*
			* @author Phillip Killewald
			**/
			std::pair<unsigned int, Crate *> getCGICrate(const cgicc::Cgicc &cgi)
			throw (emu::fed::exception::OutOfBoundsException);

			/** A way to easily get the selected DDU/DCC from the CGI variables
			*
			* @param cgi The Cgicc object which to parse.
			* @returns a pair of the cgi integer value parsed from cgi and a
			* pointer to the target VMEModule object
			*
			* @author Phillip Killewald
			**/
			template<class T>
			std::pair<unsigned int, T *> getCGIBoard(const cgicc::Cgicc &cgi)
			throw (emu::fed::exception::OutOfBoundsException)
			{

				// Start with the crate
				std::pair<unsigned int, Crate *> cratePair;
				try {
					cratePair = getCGICrate(cgi);
				} catch (emu::fed::exception::OutOfBoundsException &e) {
					std::ostringstream error;
					error << "Error getting crate from CGI input";
					LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
					XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
				}
				Crate *crate = cratePair.second;

				// Then get the slot number
				cgicc::const_form_iterator name = cgi.getElement("slot");
				unsigned int cgiSlot = 0;
				if (name != cgi.getElements().end()) {
					cgiSlot = cgi["slot"]->getIntegerValue();
				} else {
					std::ostringstream error;
					error << "Error getting slot from CGI input";
					LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
					XCEPT_RAISE(emu::fed::exception::OutOfBoundsException, error.str());
				}

				// Now get the vector of boards.
				std::vector<T *> boardVector = crate->getBoards<T>();

				// Find the slot
				for (unsigned int iBoard = 0; iBoard < boardVector.size(); iBoard++) {
					if (boardVector[iBoard]->slot() == cgiSlot) return std::pair<unsigned int, T *> (cgiSlot, boardVector[iBoard]);
				}

				std::ostringstream error;
				error << "Unable to find a board of specified type given crate " << cratePair.first << ", slot " << cgiSlot;
				LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
				XCEPT_RAISE(emu::fed::exception::OutOfBoundsException, error.str());
			}
			
			/// The XML configuration file.
			xdata::String xmlFile_;
			
			/// The crates that are under direct control of this application.
			std::vector<Crate*> crateVector_;
			
			/// For backwards compatability.
			xdata::String systemName_;

		};
		
	}
}

#endif
