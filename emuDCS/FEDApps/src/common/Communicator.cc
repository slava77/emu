/*****************************************************************************\
* $Id: Communicator.cc,v 1.16 2009/07/16 09:24:21 paste Exp $
\*****************************************************************************/
#include "emu/fed/Communicator.h"

#include <sstream>
#include <stdlib.h>
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/algorithm/string/case_conv.hpp"

#include "xgi/Method.h"
#include "cgicc/HTMLClasses.h"
#include "emu/fed/Crate.h"
#include "emu/fed/DDU.h"
#include "emu/fed/Fiber.h"
#include "emu/fed/DCC.h"
#include "emu/fed/IRQData.h"
#include "emu/fed/VMEController.h"
#include "emu/fed/CrateParser.h"
#include "emu/fed/JSONSpiritWriter.h"
#include "emu/fed/AutoConfigurator.h"
#include "emu/fed/XMLConfigurator.h"
#include "emu/fed/DBConfigurator.h"
#include "emu/base/Alarm.h"

XDAQ_INSTANTIATOR_IMPL(emu::fed::Communicator)

emu::fed::Communicator::Communicator(xdaq::ApplicationStub *stub):
xdaq::WebApplication(stub),
emu::fed::Application(stub),
emu::fed::Configurable(stub),
emu::base::Supervised(stub),
emu::fed::Supervised(stub),
ttsCrate_(0),
ttsSlot_(0),
ttsBits_(0),
fibersWithErrors_(0),
totalDCCInputRate_(0),
totalDCCOutputRate_(0)
{

	// Variables that are to be made available to other applications
	getApplicationInfoSpace()->fireItemAvailable("ttsCrate", &ttsCrate_);
	getApplicationInfoSpace()->fireItemAvailable("ttsSlot",  &ttsSlot_);
	getApplicationInfoSpace()->fireItemAvailable("ttsBits",  &ttsBits_);
	getApplicationInfoSpace()->fireItemAvailable("fibersWithErrors", &fibersWithErrors_);
	getApplicationInfoSpace()->fireItemAvailable("totalDCCInputRate", &totalDCCInputRate_);
	getApplicationInfoSpace()->fireItemAvailable("totalDCCOutputRate", &totalDCCOutputRate_);
	getApplicationInfoSpace()->fireItemAvailable("fmmErrorThreshold", &fmmErrorThreshold_);

	// HyperDAQ pages
	xgi::bind(this, &emu::fed::Communicator::webDefault, "Default");
	xgi::bind(this, &emu::fed::Communicator::webGetStatus, "GetStatus");

	// SOAP call-back functions which fire the transitions to the FSM
	BIND_DEFAULT_SOAP2FSM_ACTION(Communicator, Configure);
	BIND_DEFAULT_SOAP2FSM_ACTION(Communicator, Enable);
	BIND_DEFAULT_SOAP2FSM_ACTION(Communicator, Disable);
	BIND_DEFAULT_SOAP2FSM_ACTION(Communicator, Halt);

	// Other SOAP call-back functions
	xoap::bind(this, &emu::fed::Communicator::onSetTTSBits, "SetTTSBits", XDAQ_NS_URI);
	xoap::bind(this, &emu::fed::Communicator::onGetParameters, "GetParameters", XDAQ_NS_URI);

	// FSM state definitions and state-change call-back functions
	fsm_.addState('H', "Halted", this, &emu::fed::Communicator::stateChanged);
	fsm_.addState('C', "Configured", this, &emu::fed::Communicator::stateChanged);
	fsm_.addState('E', "Enabled", this, &emu::fed::Communicator::stateChanged);

	// FSM transition definitions
	fsm_.addStateTransition('H', 'C', "Configure", this, &emu::fed::Communicator::configureAction); // valid
	fsm_.addStateTransition('C', 'C', "Configure", this, &emu::fed::Communicator::configureAction); // valid
	fsm_.addStateTransition('E', 'C', "Configure", this, &emu::fed::Communicator::configureAction); // invalid
	fsm_.addStateTransition('F', 'C', "Configure", this, &emu::fed::Communicator::configureAction); // invalid
	
	fsm_.addStateTransition('H', 'C', "Disable", this, &emu::fed::Communicator::disableAction); // invalid
	fsm_.addStateTransition('C', 'C', "Disable", this, &emu::fed::Communicator::disableAction); // invalid
	fsm_.addStateTransition('E', 'C', "Disable", this, &emu::fed::Communicator::disableAction); // valid
	fsm_.addStateTransition('F', 'C', "Disable", this, &emu::fed::Communicator::disableAction); // invalid
	
	fsm_.addStateTransition('H', 'E', "Enable", this, &emu::fed::Communicator::enableAction); // invalid
	fsm_.addStateTransition('C', 'E', "Enable", this, &emu::fed::Communicator::enableAction); // valid
	fsm_.addStateTransition('E', 'E', "Enable", this, &emu::fed::Communicator::enableAction); // invalid
	fsm_.addStateTransition('F', 'E', "Enable", this, &emu::fed::Communicator::enableAction); // invalid
	
	fsm_.addStateTransition('H', 'H', "Halt", this, &emu::fed::Communicator::haltAction); // valid
	fsm_.addStateTransition('C', 'H', "Halt", this, &emu::fed::Communicator::haltAction); // valid
	fsm_.addStateTransition('E', 'H', "Halt", this, &emu::fed::Communicator::haltAction); // vlad
	fsm_.addStateTransition('F', 'H', "Halt", this, &emu::fed::Communicator::haltAction); // valium

	fsm_.setInitialState('H');
	fsm_.reset();

	state_ = fsm_.getStateName(fsm_.getCurrentState());

	// Other initializations
	TM_ = new IRQThreadManager(this, fmmErrorThreshold_);
	configMode_ = "XML";

}



emu::fed::Communicator::~Communicator()
{
	delete TM_;
}



// HyperDAQ pages
void emu::fed::Communicator::webDefault(xgi::Input *in, xgi::Output *out)
{

	// Configure the software so it knows to what it is talking
	try {
		softwareConfigure();
		REVOKE_ALARM("CommunicatorConfigurator", NULL);
	} catch (emu::fed::exception::ConfigurationException &e) {
		std::ostringstream error;
		error << "Unable to properly configure the Communicator software.";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "CommunicatorConfigurator", "ERROR", error.str(), "", NULL, e);
		*out << printException(e);
	}
	
	std::vector<std::string> jsFileNames;
	jsFileNames.push_back("errorFlasher.js");
	jsFileNames.push_back("definitions.js");
	jsFileNames.push_back("communicator.js");
	jsFileNames.push_back("common.js");
	*out << Header("FED Crate Communicator (" + systemName_.toString() + ")", jsFileNames);
	
	// Current condition of the Communicator
	*out << cgicc::div()
		.set("class", "titlebar default_width")
		.set("id", "FED_Communicator_Status_titlebar") << std::endl;
	*out << cgicc::div("FED Communicator Status")
		.set("class", "titletext") << std::endl;
	*out << cgicc::div() << std::endl;
	
	*out << cgicc::div()
		.set("class", "statusbar default_width")
		.set("id", "FED_Communicator_Status_statusbar") << std::endl;
	*out << cgicc::div("Time of last update:")
		.set("class", "timetext") << std::endl;
	*out << cgicc::div("never")
		.set("class", "loadtime")
		.set("id", "FED_Communicator_Status_loadtime") << std::endl;
	*out << cgicc::img()
		.set("class", "loadicon")
		.set("id", "FED_Communicator_Status_loadicon")
		.set("src", "/emu/emuDCS/FEDApps/images/empty.gif")
		.set("alt", "Loading...") << std::endl;
	*out << cgicc::br()
		.set("class", "clear") << std::endl;
	*out << cgicc::div() << std::endl;
	
	*out << cgicc::fieldset()
		.set("class", "dialog default_width")
		.set("id", "FED_Communicator_Status_dialog") << std::endl;
	
	*out << cgicc::img()
		.set("id", "statusicon")
		.set("src", "/emu/emuDCS/FEDApps/images/dialog-warning.png")
		.set("alt", "Status Icon") << std::endl;
	
	*out << cgicc::div()
		.set("class", "category") << std::endl;
	*out << "Current state: ";
	*out << cgicc::span("Unknown")
		.set("class", "Unknown")
		.set("id", "communicator_state") << std::endl;
	*out << cgicc::div() << std::endl;
	
	*out << cgicc::div()
		.set("class", "description")
		.set("id", "status_description") << std::endl;
	*out << "The Communicator has yet to update the displayed status.  Please be patient." << std::endl;
	*out << cgicc::div() << std::endl;
	
	*out << cgicc::table()
		.set("class", "noborder dialog tier2") << std::endl;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::td("Commander URL: ") << std::endl;
	*out << cgicc::td() << std::endl;
	*out << cgicc::a("none found")
		.set("id", "commanderURL") << std::endl;
	*out << cgicc::td() << std::endl;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::td("Monitor URL: ") << std::endl;
	*out << cgicc::td() << std::endl;
	*out << cgicc::a("none found")
		.set("id", "monitorURL") << std::endl;
	*out << cgicc::td() << std::endl;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::table() << std::endl; 
	
	*out << cgicc::div()
		.set("class", "description") << std::endl;
	cgicc::input checkBox;
	checkBox.set("type", "checkbox")
		.set("id", "enable_buttons")
		.set("name", "enable_buttons");
	std::string disableButtons = "true";
	if (ignoreSOAP_) checkBox.set("checked", "true");
	*out << checkBox << std::endl;
	*out << cgicc::label()
		.set("for", "enable_buttons") << std::endl;
	*out << "Enable manual state changes (not recommended)" << std::endl;
	*out << cgicc::label() << std::endl;
	*out << cgicc::div() << std::endl;
	
	*out << cgicc::button()
		.set("class", "left button statechange")
		.set("id", "halt_button")
		.set("command", "Halt")
		.set("disabled", "true")
		.set("style", "display: none") << std::endl;
	*out << cgicc::img()
		.set("class", "icon")
		.set("src", "/emu/emuDCS/FEDApps/images/process-stop.png");
	*out << "Halt" << std::endl;
	*out << cgicc::button() << std::endl;
	
	*out << cgicc::button()
		.set("class", "right button statechange")
		.set("id", "enable_button")
		.set("command", "Enable")
		.set("disabled", "true")
		.set("style", "display: none") << std::endl;
	*out << cgicc::img()
		.set("class", "icon")
		.set("src", "/emu/emuDCS/FEDApps/images/go-next.png");
	*out << "Enable" << std::endl;
	*out << cgicc::button() << std::endl;
	
	*out << cgicc::button()
		.set("class", "right button statechange")
		.set("id", "disable_button")
		.set("command", "Disable")
		.set("disabled", "true")
		.set("style", "display: none") << std::endl;
	*out << cgicc::img()
		.set("class", "icon")
		.set("src", "/emu/emuDCS/FEDApps/images/go-previous.png");
	*out << "Disable" << std::endl;
	*out << cgicc::button() << std::endl;
	
	*out << cgicc::button()
		.set("class", "right button statechange")
		.set("id", "configure_button")
		.set("command", "Configure")
		.set("disabled", "true")
		.set("style", "display: none") << std::endl;
	*out << cgicc::img()
		.set("class", "icon")
		.set("src", "/emu/emuDCS/FEDApps/images/view-refresh.png");
	*out << "Configure" << std::endl;
	*out << cgicc::button() << std::endl;
	
	*out << cgicc::fieldset() << std::endl;


	// Advanced configuration options
	*out << cgicc::div()
		.set("class", "titlebar default_width")
		.set("id", "FED_Communicator_Configuration_titlebar") << std::endl;
	*out << cgicc::div("FED Communicator Configuration Options")
		.set("class", "titletext") << std::endl;
	*out << cgicc::div() << std::endl;
	
	*out << cgicc::div()
		.set("class", "statusbar default_width")
		.set("id", "FED_Communicator_Configuration_statusbar") << std::endl;
	*out << cgicc::div("Time of last update:")
		.set("class", "timetext") << std::endl;
	*out << cgicc::div("never")
		.set("class", "loadtime")
		.set("id", "FED_Communicator_Configuration_loadtime") << std::endl;
	*out << cgicc::img()
		.set("class", "loadicon")
		.set("id", "FED_Communicator_Configuration_loadicon")
		.set("src", "/emu/emuDCS/FEDApps/images/empty.gif")
		.set("alt", "Loading...") << std::endl;
	*out << cgicc::br()
		.set("class", "clear") << std::endl;
	*out << cgicc::div() << std::endl;
	
	*out << cgicc::fieldset()
		.set("class", "dialog default_width")
		.set("id", "FED_Communicator_Configuration_dialog") << std::endl;
	
	*out << printConfigureOptions() << std::endl;
	
	*out << cgicc::fieldset() << std::endl;

	*out << Footer() << std::endl;
}



void emu::fed::Communicator::webGetStatus(xgi::Input *in, xgi::Output *out)
{
	cgicc::Cgicc cgi(in);
	
	// Need some header information to be able to return JSON
	if (cgi.getElement("debug") == cgi.getElements().end() || cgi["debug"]->getIntegerValue() != 1) {
		cgicc::HTTPResponseHeader jsonHeader("HTTP/1.1", 200, "OK");
		jsonHeader.addHeader("Content-type", "application/json");
		out->setHTTPResponseHeader(jsonHeader);
	}
	
	// Make a JSON output object
	JSONSpirit::Object output;

	// Get my state
	output.push_back(JSONSpirit::Pair("state", state_.toString()));
	
	// Other useful variables
	output.push_back(JSONSpirit::Pair("systemName", systemName_.toString()));
	
	// Find the Monitor and Commander applications that match me
	std::string monitorURL = "none found";
	std::string commanderURL = "none found";
	try {
		xdaq::ApplicationDescriptor *monitorApp = findMatchingApplication<xdata::String, std::string>("emu::fed::Monitor", "systemName", systemName_.toString());
		std::ostringstream monitorStream;
		monitorStream << monitorApp->getContextDescriptor()->getURL() << "/" << monitorApp->getURN();
		monitorURL = monitorStream.str();
	} catch (emu::fed::exception::SoftwareException &e) {
		// do nothing.
	}
	
	try {
		xdaq::ApplicationDescriptor *commanderApp = findMatchingApplication<xdata::String, std::string>("emu::fed::Commander", "systemName", systemName_.toString());
		std::ostringstream commanderStream;
		commanderStream << commanderApp->getContextDescriptor()->getURL() << "/" << commanderApp->getURN();
		commanderURL = commanderStream.str();
	} catch (emu::fed::exception::SoftwareException &e) {
		// do nothing.
	}
	output.push_back(JSONSpirit::Pair("monitorURL", monitorURL));
	output.push_back(JSONSpirit::Pair("commanderURL", commanderURL));
	
	// And now return everything as JSON
	*out << JSONSpirit::write(output);
}



void emu::fed::Communicator::configureAction(toolbox::Event::Reference event)
throw (toolbox::fsm::exception::Exception)
{

	LOG4CPLUS_DEBUG(getApplicationLogger(), "FSM transition received:  Configure");
	
	LOG4CPLUS_INFO(getApplicationLogger(), "Configuring Communicator appliction using mode " << configMode_.toString());
	
	// Configure the software so it knows to what it is talking
	try {
		softwareConfigure();
		REVOKE_ALARM("CommunicatorConfigurator", NULL);
	} catch (emu::fed::exception::ConfigurationException &e) {
		std::ostringstream error;
		error << "Unable to properly configure the Communicator software: " << e.what();
		LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
		RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "CommunicatorConfigurator", "ERROR", error.str(), e.getProperty("tag"), NULL, e);
		XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e);
	}

	// The hard reset here is just a precaution.  It costs almost nothing as far as time is concerned, and it might help to clear up problems before configure.
	for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {

		// Only reset if we have a DCC in the crate.
		std::vector<DCC *> dccs = (*iCrate)->getDCCs();

		// Don't reset crate 5 (TF)
		if (dccs.size() > 0 && !(*iCrate)->isTrackFinder()) {
			LOG4CPLUS_DEBUG(getApplicationLogger(), "HARD RESET THROUGH DCC!");
			try {
				dccs[0]->crateHardReset();
				REVOKE_ALARM("CommunicatorConfigureDCCReset", NULL);
			} catch (emu::fed::exception::DCCException &e) {
				std::ostringstream error;
				error << "Hard reset through DCC in crate " << (*iCrate)->getNumber() << " slot " << dccs[0]->slot() << " has failed";
				LOG4CPLUS_WARN(getApplicationLogger(), error.str());
				std::ostringstream tag;
				tag << "FEDCrate " << (*iCrate)->getNumber() << " FMM " << dccs[0]->getFMMID(); 
				RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "CommunicatorConfigureDCCReset", "WARN", error.str(), tag.str(), NULL, e);
				// Not an error--we could still be able to configure properly.
				//XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e);
			}
		}

		// Now we do the configure.  This is big.
		LOG4CPLUS_DEBUG(getApplicationLogger(), "Configuring crate " << (*iCrate)->getNumber());
		try {
			(*iCrate)->configure();
			REVOKE_ALARM("CommunicatorConfigure", NULL);
		} catch (emu::fed::exception::Exception &e) {
			std::ostringstream error;
			error << "Configuration of crate " << (*iCrate)->getNumber() << " has failed";
			LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
			std::ostringstream tag;
			tag << "FEDCrate " << (*iCrate)->getNumber(); 
			RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "CommunicatorConfigure", "ERROR", error.str(), tag.str(), NULL, e);
			XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e);
		}
	}

	// JRG, add loop over all DDUs in the FED Crates
	//   1) verify the L1A scalers at the DDU FPGA == 0, use  DDU::ddu_rdscaler()
	//   2) get the Live Fibers "Link Active" from the InFPGAs and check these
	//	  against the Flash settings (or DB settings or the XML?)
	//	   -use  DDU::infpga_CheckFiber(enum DEVTYPE dv)
	//	   -use "Kill Channel Mask" from  DDU::read_page1()
	//   3) check that the DDU "CSC Error Status" is Zero (like top page).
	//         -Read "CSC status summary for FMM" from  DDU::vmepara_CSCstat()
	//	   -Verify the 32-bit status from DDUFPGA == 0?
	//              use  DDU::ddu_fpgastat()&0xdecfffff   <<- note the mask
	//	   -Verify the 32-bit status from InFPGAs == 0?
	//              use  DDU::infpgastat(enum DEVTYPE dv)&0xf7eedfff   <<- note the mask
	//	 -->> definitely need to ignore some bits though!  see the masks


	for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
		
		// Set FMM error reporting disable.  Not on TF, though
		if (!(*iCrate)->isTrackFinder()) {
			try {
				(*iCrate)->getBroadcastDDU()->disableFMM();
				REVOKE_ALARM("CommunicatorConfigureFMMDisable", NULL);
			} catch (emu::fed::exception::DDUException &e) {
				std::ostringstream error;
				error << "Broadcast FMM disable to crate " << (*iCrate)->getNumber() << " has failed";
				LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
				std::ostringstream tag;
				tag << "FEDCrate " << (*iCrate)->getNumber(); 
				RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "CommunicatorConfigureFMMDisable", "ERROR", error.str(), tag.str(), NULL, e);
				XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e);
			}
		}

		std::vector<DDU *> myDDUs = (*iCrate)->getDDUs();
		for (std::vector<DDU *>::iterator iDDU = myDDUs.begin(); iDDU != myDDUs.end(); iDDU++) {
			
			try {

				LOG4CPLUS_DEBUG(getApplicationLogger(), "Checking XML, FPGA, and flash values for DDU in crate " << (*iCrate)->getNumber() << ", slot " << (*iDDU)->slot());

				uint16_t flashKillFiber = (*iDDU)->readFlashKillFiber();
				uint32_t fpgaKillFiber = (*iDDU)->readKillFiber();
				uint32_t xmlKillFiber = (*iDDU)->getKillFiber();

				LOG4CPLUS_DEBUG(getApplicationLogger(), "killFiber for DDU in crate " << (*iCrate)->getNumber() << ", slot " << (*iDDU)->slot() << ": XML(" << std::hex << xmlKillFiber << std::dec << ") FPGA(" << std::hex << fpgaKillFiber << std::dec << ") flash(" << std::hex << flashKillFiber << std::dec << ")");

				if ((flashKillFiber & 0x7fff) != (uint16_t) (xmlKillFiber & 0x7fff)) {
					LOG4CPLUS_INFO(getApplicationLogger(),"Flash and XML killFiber for DDU in crate " << (*iCrate)->getNumber() << ", slot " << (*iDDU)->slot() << " disagree:  reloading flash");
					(*iDDU)->writeFlashKillFiber(xmlKillFiber & 0x7fff);
					
					// Check again.
					uint16_t newFlashKillFiber = (*iDDU)->readFlashKillFiber();
					if ((newFlashKillFiber & 0x7fff) != (uint16_t) (xmlKillFiber & 0x7fff)) {
						std::ostringstream error;
						error << "Flash (" << std::hex << newFlashKillFiber << ") and XML (" << xmlKillFiber << ") killFiber for DDU in crate " << std::dec << (*iCrate)->getNumber() << ", slot " << (*iDDU)->slot() << " disagree after an attempt was made to reload the flash.";
						LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
						std::ostringstream tag;
						tag << "FEDCrate " << (*iCrate)->getNumber() << " RUI " << (*iDDU)->getRUI() << " FMM " << (*iDDU)->getFMMID(); 
						RAISE_ALARM(emu::fed::exception::ConfigurationException, "CommunicatorConfigureFlashKillFiber", "ERROR", error.str(), tag.str(), NULL);
						XCEPT_RAISE(toolbox::fsm::exception::Exception, error.str());
					}
				}
				REVOKE_ALARM("CommunicatorConfigureFlashKillFiber", NULL);
				
				if ((fpgaKillFiber & 0x7fff) != (xmlKillFiber & 0x7fff)) {
					LOG4CPLUS_INFO(getApplicationLogger(),"FPGA and XML killFiber for DDU in crate " << (*iCrate)->getNumber() << ", slot " << (*iDDU)->slot() << " disagree:  reloading FPGA");
					(*iDDU)->writeKillFiber(xmlKillFiber);
					
					// Check again.
					uint32_t newKillFiber = (*iDDU)->readKillFiber();
					if ((newKillFiber & 0x7fff) != (xmlKillFiber & 0x7fff)) {
						std::ostringstream error;
						error << "FPGA (" << std::hex << newKillFiber << ") and XML (" << xmlKillFiber << ") killFiber for DDU in crate " << std::dec << (*iCrate)->getNumber() << ", slot " << (*iDDU)->slot() << " disagree after an attempt was made to reload the FPGA.";
						LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
						std::ostringstream tag;
						tag << "FEDCrate " << (*iCrate)->getNumber() << " RUI " << (*iDDU)->getRUI() << " FMM " << (*iDDU)->getFMMID(); 
						RAISE_ALARM(emu::fed::exception::ConfigurationException, "CommunicatorConfigureFPGAKillFiber", "ERROR", error.str(), tag.str(), NULL);
						XCEPT_RAISE(toolbox::fsm::exception::Exception, error.str());
					}
				}

				uint16_t fpgaGbEPrescale = (*iDDU)->readGbEPrescale();
				uint16_t xmlGbEPrescale = (*iDDU)->getGbEPrescale();
				
				LOG4CPLUS_DEBUG(getApplicationLogger(), "GbE_Prescale for DDU in crate " << (*iCrate)->getNumber() << ", slot " << (*iDDU)->slot() << ": XML(" << std::hex << xmlGbEPrescale << std::dec << ") FPGA(" << std::hex << fpgaGbEPrescale << std::dec << ")");
				
				if ((fpgaGbEPrescale & 0xf) != xmlGbEPrescale) {
					LOG4CPLUS_INFO(getApplicationLogger(),"FPGA and XML GbEPrescale for DDU in crate " << (*iCrate)->getNumber() << ", slot " << (*iDDU)->slot() << " disagree:  reloading FPGA");
					(*iDDU)->writeGbEPrescale(xmlGbEPrescale & 0xf);
					
					// Check again.
					uint16_t newGbEPrescale = (*iDDU)->readGbEPrescale();
					if ((newGbEPrescale & 0xf) != (xmlGbEPrescale & 0xf)) {
						std::ostringstream error;
						error << "FPGA (" << std::hex << newGbEPrescale << ") and XML (" << xmlGbEPrescale << ") GbEPrescale for DDU in crate " << std::dec << (*iCrate)->getNumber() << ", slot " << (*iDDU)->slot() << " disagree after an attempt was made to reload the FPGA.";
						LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
						std::ostringstream tag;
						tag << "FEDCrate " << (*iCrate)->getNumber() << " RUI " << (*iDDU)->getRUI() << " FMM " << (*iDDU)->getFMMID(); 
						RAISE_ALARM(emu::fed::exception::ConfigurationException, "CommunicatorConfigureFPGAGbEPrescale", "ERROR", error.str(), tag.str(), NULL);
						XCEPT_RAISE(toolbox::fsm::exception::Exception, error.str());
					}
				}
				
				REVOKE_ALARM("CommunicatorConfigureFPGAGbEPrescale", NULL);

				// Now we should check if the RUI matches the flash value and
				//  update it as needed.
				uint32_t flashRUI = (*iDDU)->readFlashRUI();
				uint16_t calculatedRUI = (*iCrate)->getRUI((*iDDU)->slot());
				uint16_t targetRUI = (*iDDU)->getRUI();

				LOG4CPLUS_DEBUG(getApplicationLogger(),"RUI: XML(" << targetRUI << ") flash(" << flashRUI << ") calculated(" << calculatedRUI << ")");
				
				// This causes a warning only
				if (calculatedRUI != targetRUI) {
					std::ostringstream error;
					error << "XML (" << std::hex << targetRUI << ") and Calculated (" << calculatedRUI << ") RUI for DDU in crate " << std::dec << (*iCrate)->getNumber() << ", slot " << (*iDDU)->slot() << " disagree";
					LOG4CPLUS_WARN(getApplicationLogger(), error.str());
					std::ostringstream tag;
					tag << "FEDCrate " << (*iCrate)->getNumber() << " RUI " << (*iDDU)->getRUI() << " FMM " << (*iDDU)->getFMMID();
					RAISE_ALARM(emu::fed::exception::ConfigurationException, "CommunicatorConfigureCalculatedRUI", "WARN", error.str(), tag.str(), NULL);
				}
				REVOKE_ALARM("CommunicatorConfigureCalculatedRUI", NULL);
				
				if (flashRUI != targetRUI) {
					LOG4CPLUS_INFO(getApplicationLogger(),"Flash and XML RUI for DDU in crate " << (*iCrate)->getNumber() << ", slot " << (*iDDU)->slot() << " disagree:  reloading flash");
					(*iDDU)->writeFlashRUI(targetRUI);
					
					// Check again.
					uint16_t newRUI = (*iDDU)->readFlashRUI();
					if (newRUI != targetRUI) {
						std::ostringstream error;
						error << "Flash (" << std::hex << newRUI << ") and XML (" << targetRUI << ") RUI for DDU in crate " << std::dec << (*iCrate)->getNumber() << ", slot " << (*iDDU)->slot() << " disagree after an attempt was made to reload the flash.";
						LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
						std::ostringstream tag;
						tag << "FEDCrate " << (*iCrate)->getNumber() << " RUI " << (*iDDU)->getRUI() << " FMM " << (*iDDU)->getFMMID(); 
						RAISE_ALARM(emu::fed::exception::ConfigurationException, "CommunicatorConfigureFlashRUI", "ERROR", error.str(), tag.str(), NULL);
						XCEPT_RAISE(toolbox::fsm::exception::Exception, error.str());
					}
				}
				
				REVOKE_ALARM("CommunicatorConfigureFlashRUI", NULL);
				
			} catch (emu::fed::exception::DDUException &e) {
				std::ostringstream error;
				error << "Exception in communicating to DDU in crate " << (*iCrate)->getNumber() << ", slot " << (*iDDU)->slot();
				LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
				std::ostringstream tag;
				tag << "FEDCrate " << (*iCrate)->getNumber() << " RUI " << (*iDDU)->getRUI() << " FMM " << (*iDDU)->getFMMID(); 
				RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "CommunicatorConfigureDDU", "ERROR", error.str(), tag.str(), NULL, e);
				XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e);
			}
			
			LOG4CPLUS_DEBUG(getApplicationLogger(), "DDU " << (*iDDU)->getRUI() << " fully configured");
		}
		
		REVOKE_ALARM("CommunicatorConfigureDDU", NULL);
		
		std::vector<DCC *> myDCCs = (*iCrate)->getDCCs();
		for (std::vector<DCC *>::iterator iDCC = myDCCs.begin(); iDCC != myDCCs.end(); iDCC++) {
			
			try {
				uint16_t fpgaFIFOInUse = (*iDCC)->readFIFOInUse();
				uint16_t xmlFIFOInUse = (*iDCC)->getFIFOInUse();
				
				LOG4CPLUS_DEBUG(getApplicationLogger(), "FIFOInUse for DCC in crate " << (*iCrate)->getNumber() << ", slot " << (*iDCC)->slot() << ": XML(" << std::hex << xmlFIFOInUse << std::dec << ") FPGA(" << std::hex << fpgaFIFOInUse << std::dec << ")");
				
				if ((fpgaFIFOInUse & 0x3ff) != xmlFIFOInUse) {
					LOG4CPLUS_INFO(getApplicationLogger(),"FPGA and XML FIFOInUse for DCC in crate " << (*iCrate)->getNumber() << ", slot " << (*iDCC)->slot() << " disagree:  reloading FPGA");
					(*iDCC)->writeFIFOInUse(xmlFIFOInUse);
					
					// Check again.
					uint16_t newFIFOInUse = (*iDCC)->readFIFOInUse();
					if ((newFIFOInUse & 0x3ff) != xmlFIFOInUse) {
						std::ostringstream error;
						error << "FPGA (" << std::hex << newFIFOInUse << ") and XML (" << xmlFIFOInUse << ") FIFOInUse for DCC in crate " << std::dec << (*iCrate)->getNumber() << ", slot " << (*iDCC)->slot() << " disagree after an attempt was made to reload the FPGA.";
						LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
						std::ostringstream tag;
						tag << "FEDCrate " << (*iCrate)->getNumber() << " FMM " << (*iDCC)->getFMMID() << " SLINK1 " << (*iDCC)->getSLinkID(1) << " SLINK2 " << (*iDCC)->getSLinkID(2); 
						RAISE_ALARM(emu::fed::exception::ConfigurationException, "CommunicatorConfigureFPGAFIFOInUse", "ERROR", error.str(), tag.str(), NULL);
						XCEPT_RAISE(toolbox::fsm::exception::Exception, error.str());
					}
				}
				
				REVOKE_ALARM("CommunicatorConfigureFPGAFIFOInUse", NULL);
				
				uint16_t fpgaSoftwareSwitch = (*iDCC)->readSoftwareSwitch();
				uint16_t xmlSoftwareSwitch = (*iDCC)->getSoftwareSwitch();
				
				LOG4CPLUS_DEBUG(getApplicationLogger(), "SoftwareSwitch for DCC in crate " << (*iCrate)->getNumber() << ", slot " << (*iDCC)->slot() << ": XML(" << std::hex << xmlSoftwareSwitch << std::dec << ") FPGA(" << std::hex << fpgaSoftwareSwitch << std::dec << ")");
				
				if (fpgaSoftwareSwitch != xmlSoftwareSwitch) {
					LOG4CPLUS_INFO(getApplicationLogger(),"FPGA and XML SoftwareSwitch for DCC in crate " << (*iCrate)->getNumber() << ", slot " << (*iDCC)->slot() << " disagree:  reloading FPGA");
					(*iDCC)->writeSoftwareSwitch(xmlSoftwareSwitch);
					
					// Check again.
					uint16_t newSoftwareSwitch = (*iDCC)->readSoftwareSwitch();
					if (newSoftwareSwitch != xmlSoftwareSwitch) {
						std::ostringstream error;
						error << "FPGA (" << std::hex << newSoftwareSwitch << ") and XML (" << xmlSoftwareSwitch << ") SoftwareSwitch for DCC in crate " << std::dec << (*iCrate)->getNumber() << ", slot " << (*iDCC)->slot() << " disagree after an attempt was made to reload the FPGA.";
						LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
						std::ostringstream tag;
						tag << "FEDCrate " << (*iCrate)->getNumber() << " FMM " << (*iDCC)->getFMMID() << " SLINK1 " << (*iDCC)->getSLinkID(1) << " SLINK2 " << (*iDCC)->getSLinkID(2); 
						RAISE_ALARM(emu::fed::exception::ConfigurationException, "CommunicatorConfigureFPGASoftwareSwitch", "ERROR", error.str(), tag.str(), NULL);
						XCEPT_RAISE(toolbox::fsm::exception::Exception, error.str());
					}
				}
				
				REVOKE_ALARM("CommunicatorConfigureFPGASoftwareSwitch", NULL);
				
				uint16_t dccL1A = (*iDCC)->readStatusLow(); // should be all 0
				uint16_t status = (*iDCC)->readStatusHigh(); // should 0x2ff5
				
				LOG4CPLUS_DEBUG(getApplicationLogger(), "DCC Status for crate " << (*iCrate)->getNumber() << ", slot " << std::dec << (*iDCC)->slot() << ": L1A: " << dccL1A << ", status: " << std::hex << status << std::dec);
				
				if (dccL1A) {
					std::ostringstream error;
					error << "L1A for DCC in crate " << std::dec << (*iCrate)->getNumber() << ", slot " << (*iDCC)->slot() << " not reset";
					LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
					std::ostringstream tag;
					tag << "FEDCrate " << (*iCrate)->getNumber() << " FMM " << (*iDCC)->getFMMID() << " SLINK1 " << (*iDCC)->getSLinkID(1) << " SLINK2 " << (*iDCC)->getSLinkID(2); 
					RAISE_ALARM(emu::fed::exception::ConfigurationException, "CommunicatorConfigureDCC", "ERROR", error.str(), tag.str(), NULL);
					XCEPT_RAISE(toolbox::fsm::exception::Exception, error.str());
				}
				if (status != 0x2ff5) {
					std::ostringstream error;
					error << "Status for DCC in crate " << std::dec << (*iCrate)->getNumber() << ", slot " << (*iDCC)->slot() << " not reset";
					LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
					std::ostringstream tag;
					tag << "FEDCrate " << (*iCrate)->getNumber() << " FMM " << (*iDCC)->getFMMID() << " SLINK1 " << (*iDCC)->getSLinkID(1) << " SLINK2 " << (*iDCC)->getSLinkID(2); 
					RAISE_ALARM(emu::fed::exception::ConfigurationException, "CommunicatorConfigureDCC", "ERROR", error.str(), tag.str(), NULL);
					//FIXME for local running, if S-Link is not ignored, this will probably fail
					//XCEPT_RAISE(toolbox::fsm::exception::Exception, error.str());
				}

			} catch (emu::fed::exception::DCCException &e) {
				std::ostringstream error;
				error << "Exception in communicating to DCC in crate " << (*iCrate)->getNumber() << ", slot " << (*iDCC)->slot();
				LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
				std::ostringstream tag;
				tag << "FEDCrate " << (*iCrate)->getNumber() << " FMM " << (*iDCC)->getFMMID() << " SLINK1 " << (*iDCC)->getSLinkID(1) << " SLINK2 " << (*iDCC)->getSLinkID(2); 
				RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "CommunicatorConfigureDCC", "ERROR", error.str(), tag.str(), NULL, e);
				XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e);
			}
			
			LOG4CPLUS_DEBUG(getApplicationLogger(), "DCC " << (*iDCC)->getFMMID() << " fully configured");
		}
		
		REVOKE_ALARM("CommunicatorConfigureDCC", NULL);
		
		LOG4CPLUS_DEBUG(getApplicationLogger(), "Crate " << (*iCrate)->getNumber() << " fully configured");
	}
	
	LOG4CPLUS_DEBUG(getApplicationLogger(), "System " << systemName_.toString() << " fully configured");
}



void emu::fed::Communicator::enableAction(toolbox::Event::Reference event)
throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), "FSM transition received:  Enable");

	LOG4CPLUS_DEBUG(getApplicationLogger(), "The run number is " << runNumber_.toString());

	// PGK No hard reset or sync reset is coming any time soon, so we should
	//  do it ourselves.
	for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
		
		std::vector<DCC *> dccs = (*iCrate)->getDCCs();
		if (dccs.size() > 0 && !(*iCrate)->isTrackFinder()) {
			LOG4CPLUS_DEBUG(getApplicationLogger(), "RESYNC THROUGH DCC!");
			try {
				dccs[0]->crateResync();
				REVOKE_ALARM("CommunicatorEnableResync", NULL);
			} catch (emu::fed::exception::DCCException &e) {
				std::ostringstream error;
				error << "Resync through DCC in crate " << (*iCrate)->getNumber() << " slot " << dccs[0]->slot() << " has failed";
				LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
				std::ostringstream tag;
				tag << "FEDCrate " << (*iCrate)->getNumber() << " FMM " << dccs[0]->getFMMID() << " SLINK1 " << dccs[0]->getSLinkID(1) << " SLINK2 " << dccs[0]->getSLinkID(2); 
				RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "CommunicatorEnableResync", "ERROR", error.str(), tag.str(), NULL, e);
				XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e);
			}
			
		} else if ((*iCrate)->isTrackFinder()){
			// TF crate recieves a hard reset on Disable, so I have to make sure the GbE prescale is set properly here.
			DDU *myDDU = (*iCrate)->getDDUs()[0];
			try {
				myDDU->configure();
				REVOKE_ALARM("CommunicatorReconfigureTFDDU", NULL);
			} catch (emu::fed::exception::DDUException &e) {
				std::ostringstream error;
				error << "Unable to reset GbEPrescale of TF DDU";
				LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
				std::ostringstream tag;
				tag << "FEDCrate " << (*iCrate)->getNumber(); 
				RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "CommunicatorReconfigureTFDDU", "ERROR", error.str(), tag.str(), NULL, e);
				XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e);
			}
		}
		
		// Check to see if everything has been configured properly.  This has to be done here because the TF does not get a resync until Enable.
		// Now check the status registers to see if everything has been configured properly
		std::vector<DDU *> ddus = (*iCrate)->getDDUs();
		
		for (std::vector<DDU *>::iterator iDDU = ddus.begin(); iDDU != ddus.end(); iDDU++) {
			LOG4CPLUS_DEBUG(getApplicationLogger(), "Checking status of DDU in crate " << (*iCrate)->getNumber() << ", slot " << (*iDDU)->slot());
			
			uint16_t fmmReg = (*iDDU)->readFMM();
			if ((!(*iCrate)->isTrackFinder() && fmmReg != (0xFED0)) || fmmReg & 0xF != 0) {
				std::ostringstream error;
				error << "FMM register is wrong.  Got " << std::hex << fmmReg << " for DDU in crate " << std::dec << (*iCrate)->getNumber() << ", slot " << (*iDDU)->slot();
				LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
				std::ostringstream tag;
				tag << "FEDCrate " << (*iCrate)->getNumber() << " RUI " << (*iDDU)->getRUI() << " FMM " << (*iDDU)->getFMMID(); 
				RAISE_ALARM(emu::fed::exception::ConfigurationException, "CommunicatorEnableDDU", "ERROR", error.str(), tag.str(), NULL);
				XCEPT_RAISE(toolbox::fsm::exception::Exception, error.str());
			}
			
			uint32_t CSCStat = (*iDDU)->readFiberErrors();
			uint32_t dduFPGAStat = (*iDDU)->readFPGAStatus(DDUFPGA) & 0xdecfffff;  // <<- note the mask
			uint32_t inFPGA0Stat = (*iDDU)->readFPGAStatus(INFPGA0) & 0xf7eedfff;  // <<- note the mask
			uint32_t inFPGA1Stat = (*iDDU)->readFPGAStatus(INFPGA1) & 0xf7eedfff;  // <<- note the mask
		
			LOG4CPLUS_DEBUG(getApplicationLogger(), "DDU Status for crate " << (*iCrate)->getNumber() << ", slot " << std::dec << (*iDDU)->slot() << ": CSC: " << std::hex << CSCStat << ", DDUFPGA: " << dduFPGAStat << ", INFPGA0: " << inFPGA0Stat << ", INFPGA1:" << inFPGA1Stat << std::dec);
			
			uint16_t liveFibers = ((*iDDU)->readFiberStatus(INFPGA0)&0x000000ff) | (((*iDDU)->readFiberStatus(INFPGA1)&0x000000ff)<<8);
			uint16_t killFiber = ((*iDDU)->readKillFiber() & 0x7fff);
			
			LOG4CPLUS_INFO(getApplicationLogger(), "liveFibers/killFibers for crate " << (*iCrate)->getNumber() << ", slot " << std::dec << (*iDDU)->slot() << ": " << std::hex << liveFibers << "/" << killFiber << std::dec);
			
			uint32_t thisL1A = (*iDDU)->readL1Scaler(DDUFPGA);
			LOG4CPLUS_DEBUG(getApplicationLogger(), "L1A Scalar for crate " << std::dec << (*iCrate)->getNumber() << ", slot " << (*iDDU)->slot() << ": " << thisL1A);
			
			if (inFPGA0Stat) {
				std::ostringstream error;
				error << "Configuration failure for DDU in crate " << std::dec << (*iCrate)->getNumber() << ", slot " << (*iDDU)->slot() << ": INFPGA0 status register (" << std::hex << inFPGA0Stat << std::dec << ")";
				LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
				std::ostringstream tag;
				tag << "FEDCrate " << (*iCrate)->getNumber() << " RUI " << (*iDDU)->getRUI() << " FMM " << (*iDDU)->getFMMID(); 
				RAISE_ALARM(emu::fed::exception::ConfigurationException, "CommunicatorEnableDDU", "ERROR", error.str(), tag.str(), NULL);
				XCEPT_RAISE(toolbox::fsm::exception::Exception, error.str());
			}
			if (inFPGA1Stat) {
				std::ostringstream error;
				error << "Configuration failure for DDU in crate " << std::dec << (*iCrate)->getNumber() << ", slot " << (*iDDU)->slot() << ": INFPGA1 status register (" << std::hex << inFPGA1Stat << std::dec << ")";
				LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
				std::ostringstream tag;
				tag << "FEDCrate " << (*iCrate)->getNumber() << " RUI " << (*iDDU)->getRUI() << " FMM " << (*iDDU)->getFMMID(); 
				RAISE_ALARM(emu::fed::exception::ConfigurationException, "CommunicatorEnableDDU", "ERROR", error.str(), tag.str(), NULL);
				XCEPT_RAISE(toolbox::fsm::exception::Exception, error.str());
			}
			if (dduFPGAStat) {
				std::ostringstream error;
				error << "Configuration failure for DDU in crate " << std::dec << (*iCrate)->getNumber() << ", slot " << (*iDDU)->slot() << ": DDUFPGA status register (" << std::hex << dduFPGAStat << std::dec << ")";
				LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
				std::ostringstream tag;
				tag << "FEDCrate " << (*iCrate)->getNumber() << " RUI " << (*iDDU)->getRUI() << " FMM " << (*iDDU)->getFMMID(); 
				RAISE_ALARM(emu::fed::exception::ConfigurationException, "CommunicatorEnableDDU", "ERROR", error.str(), tag.str(), NULL);
				XCEPT_RAISE(toolbox::fsm::exception::Exception, error.str());
			}
			if (CSCStat) {
				std::ostringstream error;
				error << "Configuration failure for DDU in crate " << std::dec << (*iCrate)->getNumber() << ", slot " << (*iDDU)->slot() << ": CSC status register (" << std::hex << CSCStat << std::dec << ")";
				LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
				std::ostringstream tag;
				tag << "FEDCrate " << (*iCrate)->getNumber() << " RUI " << (*iDDU)->getRUI() << " FMM " << (*iDDU)->getFMMID();
				for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
					if (CSCStat && (1 << iFiber)) tag << " chamber " << (*iDDU)->getFiber(iFiber)->getName();
				}
				RAISE_ALARM(emu::fed::exception::ConfigurationException, "CommunicatorEnableDDU", "ERROR", error.str(), tag.str(), NULL);
				XCEPT_RAISE(toolbox::fsm::exception::Exception, error.str());
			}
			if (thisL1A) {
				std::ostringstream error;
				error << "Configuration problem for DDU in crate " << std::dec << (*iCrate)->getNumber() << ", slot " << (*iDDU)->slot() << ": L1A register (" << thisL1A << ") not reset";
				LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
				std::ostringstream tag;
				tag << "FEDCrate " << (*iCrate)->getNumber() << " RUI " << (*iDDU)->getRUI() << " FMM " << (*iDDU)->getFMMID(); 
				RAISE_ALARM(emu::fed::exception::ConfigurationException, "CommunicatorEnableDDU", "ERROR", error.str(), tag.str(), NULL);
				XCEPT_RAISE(toolbox::fsm::exception::Exception, error.str());
			}
			
			LOG4CPLUS_DEBUG(getApplicationLogger(), "DDU " << (*iDDU)->getRUI() << " ready to enable");
		}
		
		REVOKE_ALARM("CommunicatorEnableDDU", NULL);
		
		LOG4CPLUS_DEBUG(getApplicationLogger(), "Crate " << (*iCrate)->getNumber() << " ready to enable");
	}
	
	LOG4CPLUS_DEBUG(getApplicationLogger(), "System " << systemName_.toString() << " ready to enable");

	// PGK You have to wipe the thread manager and start over.
	delete TM_;
	TM_ = new IRQThreadManager(this, fmmErrorThreshold_);
	TM_->setSystemName(systemName_);
	for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
		//if ((*iCrate)->getNumber() > 4) continue; // Don't monitor TF crates!
		TM_->attachCrate(*iCrate);
	}
	
	// PGK We now have the run number from CSCSV
	try {
		TM_->startThreads(runNumber_);
		REVOKE_ALARM("CommunicatorEnableThreads", NULL);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "FMM monitoring threads not started!";
		LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
		RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "CommunicatorEnableThreads", "ERROR", error.str(), "", NULL, e);
		XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e);
	}
	
}



void emu::fed::Communicator::disableAction(toolbox::Event::Reference event)
throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), "FSM transition received:  Disable");

	try {
		TM_->endThreads();
		REVOKE_ALARM("CommunicatorDisableThreads", NULL);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Error in stopping FMM monitoring threads";
		LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
		RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "CommunicatorDisableThreads", "ERROR", error.str(), "", NULL, e);
		XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e);
	}

}



void emu::fed::Communicator::haltAction(toolbox::Event::Reference event)
throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), "FSM transition received:  Halt");

	try {
		TM_->endThreads();
		REVOKE_ALARM("CommunicatorHaltThreads", NULL);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Error in stopping FMM monitoring threads";
		LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
		RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "CommunicatorHaltThreads", "ERROR", error.str(), "", NULL, e);
		XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e);
	}

}



// PGK Ugly, but it must be done.  We have to update the parameters that the
//  Manager asks for or else they won't be updated!
xoap::MessageReference emu::fed::Communicator::onGetParameters(xoap::MessageReference message)
{
	// Configure yourself if you haven't yet.  This is a software-only configure.
	if (!crateVector_.size()) {
		try {
			softwareConfigure();
			REVOKE_ALARM("CommunicatorGetParameters", NULL);
		} catch (emu::fed::exception::ConfigurationException &e) {
			std::ostringstream error;
			error << "Unable to properly configure the Communicator appliction";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "CommunicatorGetParameters", "ERROR", error.str(), e.getProperty("tag"), NULL, e);
		}
	}
	
	fibersWithErrors_ = 0;
	totalDCCInputRate_ = 0;
	totalDCCOutputRate_ = 0;
	
	if (state_.toString() == "Enabled") {
	
		// Report only the number of chambers in an error state
		for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
		
			fibersWithErrors_ = fibersWithErrors_ + TM_->getData()->errorCount[(*iCrate)->getNumber()];
			
			// Average the input/output rates from the DCCs
			std::vector<DCC *> dccVector = (*iCrate)->getDCCs();
			for (std::vector<DCC *>::const_iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); iDCC++) {
				// DDU input FIFOs are 1-5 and 7-11, S-Links are 0 and 6
				for (unsigned int iFIFO = 0; iFIFO < 12; iFIFO++) {
					try {
						switch (iFIFO) {
						
						case 0:
						case 6:
							totalDCCOutputRate_ = totalDCCOutputRate_ + (*iDCC)->readRate(iFIFO);
							break;
						default:
							totalDCCInputRate_ = totalDCCInputRate_ + (*iDCC)->readRate(iFIFO);
							break;
						}
					} catch (emu::fed::exception::DCCException &e) {
						// do nothing
					}
				}
			}
		}
	}
	
	return emu::fed::Application::onGetParameters(message);

}




xoap::MessageReference emu::fed::Communicator::onSetTTSBits(xoap::MessageReference message)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), "Remote SOAP command: SetTTSBits");
	
	// Check to see if this instance is in command of the given crate number and slot
	bool found = false;
	for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
		if ((*iCrate)->getNumber() != ttsCrate_) continue;
		
		for (std::vector<DDU *>::iterator iDDU = (*iCrate)->getDDUs().begin(); iDDU != (*iCrate)->getDDUs().end(); iDDU++) {
			if ((*iDDU)->slot() == ttsSlot_) {
				found = true;
				break;
			}
		}
		if (found) break;
		for (std::vector<DCC *>::iterator iDCC = (*iCrate)->getDCCs().begin(); iDCC != (*iCrate)->getDCCs().end(); iDCC++) {
			if ((*iDCC)->slot() == ttsSlot_) {
				found = true;
				break;
			}
		}
	}
	
	if (!found) {
		LOG4CPLUS_INFO(getApplicationLogger(), "ttsCrate_=" << ttsCrate_.toString() << ", ttsSlot_=" << ttsSlot_.toString() << " is not commanded by this application");
		return createReply(message);
	} else {
		LOG4CPLUS_INFO(getApplicationLogger(), "Writing ttsCrate_=" << ttsCrate_.toString() << " ttsSlot_=" << ttsSlot_.toString() << " ttsBits_=" << ttsBits_.toString());
	}

	// cache TTS bits
	xdata::Integer cachedBits = ttsBits_;
	try {
		// set TTS bits
		writeTTSBits(ttsCrate_, ttsSlot_, ttsBits_);
		// read back TTS bits
		ttsBits_ = readTTSBits(ttsCrate_, ttsSlot_);
		REVOKE_ALARM("TTSWrite", NULL);
	} catch (emu::fed::exception::TTSException &e) {
		std::ostringstream error;
		error << "Set TTS bits in crate " << ttsCrate_.toString() << ", slot " << ttsSlot_.toString() << " has failed";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		RAISE_ALARM_NESTED(emu::fed::exception::TTSException, "TTSWrite", "ERROR", error.str(), "", NULL, e);
		return createReply(message);
	}

	LOG4CPLUS_DEBUG(getApplicationLogger(), "Read back ttsBits_=" << ttsBits_.toString());
	
	if (ttsBits_ != cachedBits) {
		std::ostringstream error;
		error << "Read back ttsBits_=" << ttsBits_.toString() << " from ttsCrate_=" << ttsCrate_.toString() << ", ttsSlot_=" << ttsSlot_.toString() << ", should have been " << cachedBits.toString();
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		RAISE_ALARM(emu::fed::exception::TTSException, "TSSCheck", "WARN", error.str(), "", NULL);
	}
	
	REVOKE_ALARM("TTSCheck", NULL);

	// PGK Remember:  you can always steal the TTSBits status via SOAP if you really, really want it.
	return createReply(message);
}



// Stolen from the now-defunct EmuFController
void emu::fed::Communicator::writeTTSBits(const unsigned int crate, const unsigned int slot, const int bits)
throw (emu::fed::exception::TTSException)
{

	LOG4CPLUS_DEBUG(getApplicationLogger(), "Setting TTS bits on crate " << crate << ", slot " << slot << ", bits " << std::hex << bits << std::dec);
	for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
		if ((*iCrate)->getNumber() != crate) continue;
	
		if (slot == 8 || slot == 18) {
			
			std::vector<DCC *> dccVector = (*iCrate)->getDCCs();
			for (std::vector<DCC *>::iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); iDCC++) {
				if ((*iDCC)->slot() != slot) continue;
				try {
					(*iDCC)->writeFMM((bits | 0x10) & 0xffff);
				} catch (emu::fed::exception::DCCException &e) {
					std::ostringstream error;
					error << "Set TTS bits on DCC in crate " << (*iCrate)->getNumber() << " slot " << (*iDCC)->slot() << " has failed";
					XCEPT_RETHROW(emu::fed::exception::TTSException, error.str(), e);
				}
			}
		} else {
			
			std::vector<DDU *> dduVector = (*iCrate)->getDDUs();
			for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
				if ((*iDDU)->slot() != slot) continue;
				try {
					(*iDDU)->writeFMM((bits | 0xf0e0) & 0xffff);
				} catch (emu::fed::exception::DDUException &e) {
					std::ostringstream error;
					error << "Set TTS bits on DDU in crate " << (*iCrate)->getNumber() << " slot " << (*iDDU)->slot() << " has failed";
					XCEPT_RETHROW(emu::fed::exception::TTSException, error.str(), e);
				}
			}
		}
	}
}


// Stolen from the now-defunct EmuFController
int emu::fed::Communicator::readTTSBits(const unsigned int crate, const unsigned int slot)
throw (emu::fed::exception::TTSException)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), "Reading TTS bits on crate " << crate << ", slot " << slot);
	for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
		if ((*iCrate)->getNumber() != crate) continue;
		
		if (slot == 8 || slot == 18) {
			
			std::vector<DCC *> dccVector = (*iCrate)->getDCCs();
			for (std::vector<DCC *>::iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); iDCC++) {
				if ((*iDCC)->slot() != slot) continue;
				try {
					return (*iDCC)->readFMM() & 0xf;
				} catch (emu::fed::exception::DCCException &e) {
					std::ostringstream error;
					error << "Read TTS bits from DCC in crate " << (*iCrate)->getNumber() << " slot " << (*iDCC)->slot() << " has failed";
					XCEPT_RETHROW(emu::fed::exception::TTSException, error.str(), e);
				}
			}
		} else {
			
			std::vector<DDU *> dduVector = (*iCrate)->getDDUs();
			for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
				if ((*iDDU)->slot() != slot) continue;
				try {
					return (*iDDU)->readFMM() & 0xf;
				} catch (emu::fed::exception::DDUException &e) {
					std::ostringstream error;
					error << "Read TTS bits from DDU in crate " << (*iCrate)->getNumber() << " slot " << (*iDDU)->slot() << " has failed";
					XCEPT_RETHROW(emu::fed::exception::TTSException, error.str(), e);
				}
			}
		}
	}

	return 0;
}

// End of file
// vim: set sw=4 ts=4:
