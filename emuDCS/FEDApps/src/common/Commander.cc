/*****************************************************************************\
* $Id: Commander.cc,v 1.13 2009/11/03 15:17:04 paste Exp $
\*****************************************************************************/
#include "emu/fed/Commander.h"

#include <iostream>
#include <iomanip>
#include <map>

#include "xgi/Method.h"
#include "cgicc/HTMLClasses.h"
#include "emu/base/Alarm.h"
#include "emu/fed/VMEController.h"
#include "emu/fed/DDU.h"
#include "emu/fed/DCC.h"
#include "emu/fed/Fiber.h"
#include "emu/fed/FIFO.h"
#include "emu/fed/Crate.h"
#include "emu/fed/DDUDebugger.h"
#include "emu/fed/DCCDebugger.h"
#include "emu/fed/XMLConfigurator.h"
#include "emu/fed/DBConfigurator.h"
#include "emu/fed/AutoConfigurator.h"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/algorithm/string/case_conv.hpp"
#include "emu/fed/JSONSpiritWriter.h"

XDAQ_INSTANTIATOR_IMPL(emu::fed::Commander)

emu::fed::Commander::Commander(xdaq::ApplicationStub *stub):
xdaq::WebApplication(stub),
emu::fed::Application(stub),
emu::fed::Configurable(stub)
{
	// HyperDAQ pages
	xgi::bind(this, &emu::fed::Commander::webDefault, "Default");
	xgi::bind(this, &emu::fed::Commander::webReadDDURegisters, "ReadDDURegisters");
	xgi::bind(this, &emu::fed::Commander::webDisplayDDURegisters, "DisplayDDURegisters");
	xgi::bind(this, &emu::fed::Commander::webGetStatus, "GetStatus");
	
	// Other SOAP call-back functions
	xoap::bind(this, &emu::fed::Commander::onGetParameters, "GetParameters", XDAQ_NS_URI);
	
	// Other initializations
	configMode_ = "XML";
}



emu::fed::Commander::~Commander()
{
}



void emu::fed::Commander::webDefault(xgi::Input *in, xgi::Output *out)
{
	// Configure yourself if you haven't yet.  This is a software-only configure.
	if (!crateVector_.size()) {
		try {
			softwareConfigure();
			REVOKE_ALARM("CommanderDefault", NULL);
		} catch (emu::fed::exception::ConfigurationException &e) {
			std::ostringstream error;
			error << "Unable to properly configure the Commander appliction";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "CommanderDefault", "ERROR", error.str(), e.getProperty("tag"), NULL, e);
		}
	}
	
	std::vector<std::string> jsFiles;
	jsFiles.push_back("errorFlasher.js");
	jsFiles.push_back("definitions.js");
	jsFiles.push_back("commander.js");
	jsFiles.push_back("common.js");
	*out << Header("FED Commander (" + systemName_.toString() + ")", jsFiles);
	
	// Overview of crate status
	*out << cgicc::div()
		.set("class", "titlebar commander_width")
		.set("id", "FED_Commander_Select_titlebar") << std::endl;
	*out << cgicc::div("FED Commander Selection")
		.set("class", "titletext") << std::endl;
	*out << cgicc::div() << std::endl;
	
	*out << cgicc::div()
		.set("class", "statusbar commander_width")
		.set("id", "FED_Commander_Select_statusbar") << std::endl;
	*out << cgicc::div("Time of last update:")
		.set("class", "timetext") << std::endl;
	*out << cgicc::div("never")
		.set("class", "loadtime")
		.set("id", "FED_Commander_Select_loadtime") << std::endl;
	*out << cgicc::img()
		.set("class", "loadicon")
		.set("id", "FED_Commander_Select_loadicon")
		.set("src", "/emu/emuDCS/FEDApps/images/empty.gif")
		.set("alt", "Loading...") << std::endl;
	*out << cgicc::br()
		.set("class", "clear") << std::endl;
	*out << cgicc::div() << std::endl;
	
	*out << cgicc::fieldset()
		.set("class", "dialog commander_width")
		.set("id", "FED_Commander_Select_dialog") << std::endl;
		
	*out << cgicc::ul()
		.set("class", "tier0") << std::endl;
	*out << cgicc::li("Select crates or boards to command")
		.set("class", "bold tier0") << std::endl;
		
	for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
		std::ostringstream crateNumber;
		crateNumber << (*iCrate)->getNumber();
		std::string crateID = "crate_" + crateNumber.str();
		*out << cgicc::div()
			.set("class", "tier0");
		
		// Checkbox for the crate (for firmware commands)
		*out << cgicc::input()
			.set("type", "checkbox")
			.set("class", "crate_checkbox")
			.set("name", crateID)
			.set("id", crateID + "_checkbox")
			.set("crate", crateNumber.str()) << std::endl;
		*out << cgicc::label()
			.set("for", crateID + "_checkbox") << std::endl;
		*out << "Crate " << (*iCrate)->getNumber() << ": ";
		*out << cgicc::label() << std::endl;
		
		// Some handy buttons for selecting groups of elements
		*out << cgicc::button("Select all DDUs")
			.set("id", crateID + "_all_ddus")
			.set("class", "all_ddus")
			.set("crate", crateNumber.str()) << std::endl;
		*out << cgicc::button("Select no DDUs")
			.set("id", crateID + "_no_ddus")
			.set("class", "no_ddus")
			.set("crate", crateNumber.str()) << std::endl;
		*out << cgicc::button("Select all DCCs")
			.set("id", crateID + "_all_dccs")
			.set("class", "all_dccs")
			.set("crate", crateNumber.str()) << std::endl;
		*out << cgicc::button("Select no DCCs")
			.set("id", crateID + "_no_dccs")
			.set("class", "no_dccs")
			.set("crate", crateNumber.str()) << std::endl;
		*out << cgicc::div() << std::endl;
		
		*out << cgicc::table()
			.set("class", "small_crate tier0") << std::endl;
		*out << cgicc::tr()
			.set("class", "header") << std::endl;
		*out << cgicc::td("Slot")
			.set("class", "header") << std::endl;
		*out << cgicc::td("Board")
			.set("class", "header") << std::endl;
		*out << cgicc::td("Fiber/FIFO status")
			.set("class", "header") << std::endl;
		*out << cgicc::tr() << std::endl;
		
		std::vector<DDU *> myDDUs = (*iCrate)->getDDUs();
		for (std::vector<DDU *>::iterator iDDU = myDDUs.begin(); iDDU != myDDUs.end(); iDDU++) {
			std::ostringstream slotNumber;
			slotNumber << (*iDDU)->getSlot();
			std::string slotID = crateID + "_slot_" + slotNumber.str();
			std::ostringstream rui;
			rui << (*iDDU)->getRUI();
			
			*out << cgicc::tr()
				.set("class", "board") << std::endl;
			*out << cgicc::td()
				.set("class", "slot") << std::endl;
			// Check box for this DDU
			*out << cgicc::input()
				.set("type", "checkbox")
				.set("class", "ddu_checkbox")
				.set("id", slotID + "_checkbox")
				.set("name", slotID)
				.set("crate", crateNumber.str())
				.set("slot", slotNumber.str())
				.set("rui", rui.str()) << std::endl;
			*out << cgicc::label()
				.set("for", slotID + "_checkbox") << std::endl;
			*out << (*iDDU)->getSlot() << std::endl;
			*out << cgicc::label() << std::endl;
			*out << cgicc::td() << std::endl;
			
			*out << cgicc::td()
				.set("class", "board")
				.set("id", slotID) << std::endl;
			*out << cgicc::label()
				.set("for", slotID + "_checkbox") << std::endl;
			*out << "DDU " << (*iDDU)->getRUI() << std::endl;
			*out << cgicc::label() << std::endl;
			*out << cgicc::td() << std::endl;
			
			*out << cgicc::td()
				.set("class", "fibers") << std::endl;
			// I am going to strangle CGICC with my bare hands.
			*out << "<table class=\"fibers\">\n<tr class=\"fibers\">\n";
			for (size_t iFiber = 0; iFiber < 15; iFiber++) {
				std::ostringstream fiberID;
				fiberID << slotID << "_fiber_" << iFiber;
				Fiber *myFiber = (*iDDU)->getFiber(iFiber);
				*out << cgicc::td(myFiber->getName())
					.set("class", "fiber")
					.set("id", fiberID.str()) << std::endl;
			}
			*out << "</tr>\n</table>\n";
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;
		}
		
		std::vector<DCC *> myDCCs = (*iCrate)->getDCCs();
		for (std::vector<DCC *>::iterator iDCC = myDCCs.begin(); iDCC != myDCCs.end(); iDCC++) {
			std::ostringstream slotNumber;
			slotNumber << (*iDCC)->getSlot();
			std::string slotID = crateID + "_slot_" + slotNumber.str();
			std::ostringstream fmmid;
			fmmid << (*iDCC)->getFMMID();
			
			*out << cgicc::tr()
				.set("class", "board") << std::endl;
			*out << cgicc::td()
				.set("class", "slot") << std::endl;
			// Check box for this DCC
			*out << cgicc::input()
				.set("type", "checkbox")
				.set("class", "dcc_checkbox")
				.set("id", slotID + "_checkbox")
				.set("name", slotID)
				.set("crate", crateNumber.str())
				.set("slot", slotNumber.str()) 
				.set("fmmid", fmmid.str()) << std::endl;
			*out << cgicc::label()
				.set("for", slotID + "_checkbox") << std::endl;
			*out << (*iDCC)->getSlot() << std::endl;
			*out << cgicc::label() << std::endl;
			*out << cgicc::td() << std::endl;
			
			*out << cgicc::td()
				.set("class", "board")
				.set("id", slotID) << std::endl;
			*out << cgicc::label()
				.set("for", slotID + "_checkbox") << std::endl;
			*out << "DCC " << (*iDCC)->getFMMID() << std::endl;
			*out << cgicc::label() << std::endl;
			*out << cgicc::td() << std::endl;
			
			*out << cgicc::td()
				.set("class", "fibers") << std::endl;
			// I am going to strangle CGICC with my bare hands.
			*out << "<table class=\"fibers\">\n<tr class=\"fibers\">\n";
			for (size_t iFIFO = 0; iFIFO <= 9; iFIFO++) {
				std::ostringstream fifoID;
				fifoID << slotID << "_fifo_" << iFIFO;
				FIFO *myFIFO = (*iDCC)->getFIFO(iFIFO);
				*out << "<td class=\"fiber\" id=\"" << fifoID.str() << "\">" << std::endl;
				*out << "DDU " << myFIFO->getRUI();
				*out << "</td>" << std::endl;
			}
			for (size_t iSlink = 1; iSlink <= 2; iSlink++) {
				std::ostringstream slinkID;
				slinkID << slotID << "_slink_" << iSlink;
				*out << "<td class=\"fiber\" id=\"" << slinkID.str() << "\">" << std::endl;
				*out << "Slink " << iSlink;
				*out << "</td>" << std::endl;
			}
			*out << "</tr>\n</table>\n";
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;
		}
		
		*out << cgicc::table() << std::endl;
	}
	
	// Manage firmware buttons
	*out << cgicc::li("Manage firmware")
		.set("class", "bold tier0") << std::endl;
	
	// A series of buttons for custom dialogs
	*out << cgicc::div()
		.set("class", "tier1") << std::endl;
	*out << cgicc::button("Manage DDU Firmware")
		.set("id", "ddu_firmware_button")
		.set("class", "firmware_button") << std::endl;
	*out << cgicc::button("Manage DCC Firmware")
		.set("id", "dcc_firmware_button")
		.set("class", "firmware_button") << std::endl;
	*out << cgicc::div() << std::endl;
	
	
	// DDU writable registers
	*out << cgicc::li("Writable DDU registers")
		.set("class", "bold tier0") << std::endl;
		
	// A series of buttons for custom dialogs
	*out << cgicc::div()
		.set("class", "tier1") << std::endl;
	*out << cgicc::button("Set Flash Board ID")
		.set("id", "ddu_flash_boardid_dialog")
		.set("class", "ddu_button") << std::endl;
	*out << cgicc::button("Set Flash RUI Number")
		.set("id", "ddu_flash_rui_dialog")
		.set("class", "ddu_button") << std::endl;
	*out << cgicc::button("Set Flash KillFiber")
		.set("id", "ddu_flash_killfiber_dialog")
		.set("class", "ddu_button") << std::endl;
	*out << cgicc::button("Set GbE Prescale")
		.set("id", "ddu_gbe_prescale_dialog")
		.set("class", "ddu_button") << std::endl;
	*out << cgicc::button("Set FMM Status")
		.set("id", "ddu_fmm_dialog")
		.set("class", "ddu_button") << std::endl;
	*out << cgicc::button("Set Fake L1 Passthrough")
		.set("id", "ddu_fake_l1_dialog")
		.set("class", "ddu_button") << std::endl;
	*out << cgicc::button("Set Flash GbE Thresholds")
		.set("id", "ddu_flash_gbe_thresholds_dialog")
		.set("class", "ddu_button") << std::endl;
	*out << cgicc::button("Set Bunch-Crossing Orbit")
		.set("id", "ddu_bxorbit_dialog")
		.set("class", "ddu_button") << std::endl;
	*out << cgicc::button("Set Input Register")
		.set("id", "ddu_input_reg_dialog")
		.set("class", "ddu_button") << std::endl;
	*out << cgicc::div() << std::endl;
	
	
	// DDU readable registers
	*out << cgicc::li("Readable DDU registers")
	.set("class", "bold tier0") << std::endl;
	
	// A series of buttons to make things easier on us
	*out << cgicc::div()
		.set("class", "tier1") << std::endl;
	*out << cgicc::button("Select All DDU Registers")
		.set("id", "all_ddu_registers") << std::endl;
	*out << cgicc::button("Select No DDU Registers")
		.set("id", "no_ddu_registers") << std::endl;
	*out << cgicc::div() << std::endl;
	
	std::vector<Register> DDURegisters;
	DDURegisters.push_back(Register("Flash Board ID", "ddu1"));
	DDURegisters.push_back(Register("RUI Number", "ddu2"));
	DDURegisters.push_back(Register("Flash RUI Number", "ddu3"));
	DDURegisters.push_back(Register("KillFiber", "ddu4"));
	DDURegisters.push_back(Register("Flash KillFiber", "ddu5"));
	DDURegisters.push_back(Register("GbE Prescale", "ddu6"));
	
	DDURegisters.push_back(Register("DDUFPGA L1A Scaler", "ddu7"));
	DDURegisters.push_back(Register("INFPGA0 L1A Scaler (fibers 0-3)", "ddu8"));
	DDURegisters.push_back(Register("INFPGA0 L1A Scaler (fibers 4-7)", "ddu9"));
	DDURegisters.push_back(Register("INFPGA1 L1A Scaler (fibers 8-11)", "ddu10"));
	DDURegisters.push_back(Register("INFPGA1 L1A Scaler (fibers 12-14)", "ddu11"));
	
	DDURegisters.push_back(Register("FMM Register", "ddu12"));
	DDURegisters.push_back(Register("Per-Fiber Error Status", "ddu13"));
	DDURegisters.push_back(Register("Per-Fiber Advanced Error Status", "ddu14"));
	DDURegisters.push_back(Register("Per-Fiber FMM Busy", "ddu15"));
	DDURegisters.push_back(Register("Per-Fiber FMM Busy History", "ddu16"));
	DDURegisters.push_back(Register("Per-Fiber FMM Full Warning", "ddu17"));
	DDURegisters.push_back(Register("Per-Fiber FMM Full Warning History", "ddu18"));
	DDURegisters.push_back(Register("Per-Fiber FMM Lost Sync", "ddu19"));
	DDURegisters.push_back(Register("Per-Fiber FMM Error", "ddu20"));
	
	DDURegisters.push_back(Register("DDUFPGA Status", "ddu21"));
	DDURegisters.push_back(Register("DDUFPGA Diagnostic Trap", "ddu22"));
	DDURegisters.push_back(Register("Per-Fiber DDUFPGA FIFO-Full", "ddu23"));
	DDURegisters.push_back(Register("Per-Fiber CRC Error", "ddu24"));
	DDURegisters.push_back(Register("Per-Fiber Transmit Error", "ddu25"));
	DDURegisters.push_back(Register("Per-Fiber DMB Error", "ddu26"));
	DDURegisters.push_back(Register("Per-Fiber TMB Error", "ddu27"));
	DDURegisters.push_back(Register("Per-Fiber ALCT Error", "ddu28"));
	DDURegisters.push_back(Register("Per-Fiber Lost-In-Event Error", "ddu29"));
	DDURegisters.push_back(Register("Per-Fiber Input Corruption History", "ddu30"));
	DDURegisters.push_back(Register("Per-Fiber DMB Live", "ddu31"));
	DDURegisters.push_back(Register("Per-Fiber DMB Live at First Event", "ddu32"));
	DDURegisters.push_back(Register("Per-Fiber Warning Status", "ddu33"));
	
	DDURegisters.push_back(Register("Temperature 0", "ddu34"));
	DDURegisters.push_back(Register("Temperature 1", "ddu35"));
	DDURegisters.push_back(Register("Temperature 2", "ddu36"));
	DDURegisters.push_back(Register("Temperature 3", "ddu37"));
	DDURegisters.push_back(Register("Voltage 1.5", "ddu38"));
	DDURegisters.push_back(Register("Voltage 2.5 (1)", "ddu39"));
	DDURegisters.push_back(Register("Voltage 2.5 (2)", "ddu40"));
	DDURegisters.push_back(Register("Voltage 3.3", "ddu41"));
	
	DDURegisters.push_back(Register("Fake L1 Passthrough", "ddu42"));
	DDURegisters.push_back(Register("Flash GbE FIFO Thresholds", "ddu43"));
	DDURegisters.push_back(Register("InRD Status", "ddu44"));
	DDURegisters.push_back(Register("Error Bus Register A", "ddu45"));
	DDURegisters.push_back(Register("Error Bus Register B", "ddu46"));
	DDURegisters.push_back(Register("Error Bus Register C", "ddu47"));
	DDURegisters.push_back(Register("Maximum L1A-to-Data Timeout", "ddu48"));
	DDURegisters.push_back(Register("Bunch-Crossing Orbit", "ddu49"));
	DDURegisters.push_back(Register("DDUFPGA Output Status", "ddu50"));
	DDURegisters.push_back(Register("DDUFPGA Input FIFO Status 1", "ddu51"));
	DDURegisters.push_back(Register("DDUFPGA Input FIFO Status 2", "ddu52"));
	DDURegisters.push_back(Register("DDUFPGA Input FIFO Status 3", "ddu53"));
	DDURegisters.push_back(Register("Serial Status", "ddu54"));
	DDURegisters.push_back(Register("Parallel Status", "ddu55"));
	DDURegisters.push_back(Register("Switches", "ddu56"));
	
	DDURegisters.push_back(Register("Input Register 0", "ddu57"));
	DDURegisters.push_back(Register("Input Register 1", "ddu58"));
	DDURegisters.push_back(Register("Input Register 2", "ddu59"));
	DDURegisters.push_back(Register("Test Register 0", "ddu60"));
	DDURegisters.push_back(Register("Test Register 1", "ddu61"));
	DDURegisters.push_back(Register("Test Register 2", "ddu62"));
	DDURegisters.push_back(Register("Test Register 3", "ddu63"));
	DDURegisters.push_back(Register("Test Register 4", "ddu64"));

	DDURegisters.push_back(Register("INFPGA0 Status", "ddu65"));
	DDURegisters.push_back(Register("INFPGA0 Diagnostic Trap", "ddu66"));
	DDURegisters.push_back(Register("INFPGA0 Fiber Status (fibers 0-7)", "ddu67"));
	DDURegisters.push_back(Register("INFPGA0 DMB Sync (fibers 0-7)", "ddu68"));
	DDURegisters.push_back(Register("INFPGA0 FIFO Status (fibers 0-7)", "ddu69"));
	DDURegisters.push_back(Register("INFPGA0 FIFO Full (fibers 0-7)", "ddu70"));
	DDURegisters.push_back(Register("INFPGA0 Link Receive Error (fibers 0-7)", "ddu71"));
	DDURegisters.push_back(Register("INFPGA0 Link Transmit Error (fibers 0-7)", "ddu72"));
	DDURegisters.push_back(Register("INFPGA0 Timeout Error (fibers 0-7)", "ddu73"));
	DDURegisters.push_back(Register("INFPGA0 Memory Active (fibers 0-1)", "ddu74"));
	DDURegisters.push_back(Register("INFPGA0 Memory Active (fibers 2-3)", "ddu75"));
	DDURegisters.push_back(Register("INFPGA0 Memory Active (fibers 4-5)", "ddu76"));
	DDURegisters.push_back(Register("INFPGA0 Memory Active (fibers 6-7)", "ddu77"));
	DDURegisters.push_back(Register("INFPGA0 Memory Free (fibers 0-7)", "ddu78"));
	DDURegisters.push_back(Register("INFPGA0 Minimum Memory Free (fibers 0-7)", "ddu79"));
	DDURegisters.push_back(Register("INFPGA0 Lost-In-Event (fibers 0-7)", "ddu80"));
	DDURegisters.push_back(Register("INFPGA0 \"C\" Code (fibers 0-7)", "ddu81"));
	DDURegisters.push_back(Register("INFPGA0 DMB Warning (fibers 0-7)", "ddu82"));
	DDURegisters.push_back(Register("INFPGA0 Fiber Diagnostic Register 0 (fibers 0-7)", "ddu83"));
	DDURegisters.push_back(Register("INFPGA0 Fiber Diagnostic Register 1 (fibers 0-7)", "ddu84"));
	
	DDURegisters.push_back(Register("INFPGA1 Status", "ddu85"));
	DDURegisters.push_back(Register("INFPGA1 Diagnostic Trap", "ddu86"));
	DDURegisters.push_back(Register("INFPGA1 Fiber Status (fibers 8-14)", "ddu87"));
	DDURegisters.push_back(Register("INFPGA1 DMB Sync (fibers 8-14)", "ddu88"));
	DDURegisters.push_back(Register("INFPGA1 FIFO Status (fibers 8-14)", "ddu89"));
	DDURegisters.push_back(Register("INFPGA1 FIFO Full (fibers 8-14)", "ddu90"));
	DDURegisters.push_back(Register("INFPGA1 Link Receive Error (fibers 8-14)", "ddu91"));
	DDURegisters.push_back(Register("INFPGA1 Link Transmit Error (fibers 8-14)", "ddu92"));
	DDURegisters.push_back(Register("INFPGA1 Timeout Error (fibers 8-14)", "ddu93"));
	DDURegisters.push_back(Register("INFPGA1 Memory Active (fibers 8-9)", "ddu94"));
	DDURegisters.push_back(Register("INFPGA1 Memory Active (fibers 10-11)", "ddu95"));
	DDURegisters.push_back(Register("INFPGA1 Memory Active (fibers 12-13)", "ddu96"));
	DDURegisters.push_back(Register("INFPGA1 Memory Active (fiber 14)", "ddu97"));
	DDURegisters.push_back(Register("INFPGA1 Memory Free (fibers 8-14)", "ddu98"));
	DDURegisters.push_back(Register("INFPGA1 Minimum Memory Free (fibers 8-14)", "ddu99"));
	DDURegisters.push_back(Register("INFPGA1 Lost-In-Event (fibers 8-14)", "ddu100"));
	DDURegisters.push_back(Register("INFPGA1 \"C\" Code (fibers 8-14)", "ddu101"));
	DDURegisters.push_back(Register("INFPGA1 DMB Warning (fibers 8-14)", "ddu102"));
	DDURegisters.push_back(Register("INFPGA1 Fiber Diagnostic Register 0 (fibers 8-14)", "ddu103"));
	DDURegisters.push_back(Register("INFPGA1 Fiber Diagnostic Register 1 (fibers 8-14)", "ddu104"));
	
	// Print the table of registers
	*out << cgicc::div()
		.set("class", "tier1") << std::endl;
	*out << printRegisterTable(DDURegisters, "ddu_registers") << std::endl;
	*out << cgicc::div() << std::endl;

	// Buttons for displaying
	*out << cgicc::div()
		.set("class", "tier1") << std::endl;
	*out << cgicc::button("Display Selected DDU Registers (opens a new window)")
		.set("id", "ddu_display_button")
		.set("class", "ddu_button") << std::endl;
	*out << cgicc::div() << std::endl;
	
	*out << cgicc::ul() << std::endl;
	
	*out << cgicc::fieldset() << std::endl;

	
	// Advanced configuration options
	*out << cgicc::div()
		.set("class", "titlebar default_width")
		.set("id", "FED_Commander_Configuration_titlebar") << std::endl;
	*out << cgicc::div("FED Commander Configuration Options")
		.set("class", "titletext") << std::endl;
	*out << cgicc::div() << std::endl;
	
	*out << cgicc::div()
		.set("class", "statusbar default_width")
		.set("id", "FED_Commander_Configuration_statusbar") << std::endl;
	*out << cgicc::div("Time of last update:")
		.set("class", "timetext") << std::endl;
	*out << cgicc::div("never")
		.set("class", "loadtime")
		.set("id", "FED_Commander_Configuration_loadtime") << std::endl;
	*out << cgicc::img()
		.set("class", "loadicon")
		.set("id", "FED_Commander_Configuration_loadicon")
		.set("src", "/emu/emuDCS/FEDApps/images/empty.gif")
		.set("alt", "Loading...") << std::endl;
	*out << cgicc::br()
		.set("class", "clear") << std::endl;
	*out << cgicc::div() << std::endl;
	
	*out << cgicc::fieldset()
		.set("class", "dialog default_width")
		.set("id", "FED_Commander_Configuration_dialog") << std::endl;
	
	*out << printConfigureOptions() << std::endl;
	
	*out << cgicc::button()
		.set("class", "right button")
		.set("id", "reconfigure_button") << std::endl;
	*out << cgicc::img()
		.set("class", "icon")
		.set("src", "/emu/emuDCS/FEDApps/images/view-refresh.png");
	*out << "Reconfigure Software" << std::endl;
	*out << cgicc::button() << std::endl;
	
	*out << cgicc::fieldset() << std::endl;

	*out << Footer();
}



void emu::fed::Commander::webReadDDURegisters(xgi::Input *in, xgi::Output *out)
{

	// Configure yourself if you haven't yet.  This is a software-only configure.
	if (!crateVector_.size()) {
		try {
			softwareConfigure();
			REVOKE_ALARM("CommanderReadDDURegisters", NULL);
		} catch (emu::fed::exception::ConfigurationException &e) {
			std::ostringstream error;
			error << "Unable to properly configure the Commander appliction";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "CommanderReadDDURegisters", "ERROR", error.str(), e.getProperty("tag"), NULL, e);
		}
	}
	
	cgicc::Cgicc cgi(in);
	
	// Need some header information to be able to return JSON
	if (cgi.getElement("debug") == cgi.getElements().end() || cgi["debug"]->getIntegerValue() != 1) {
		cgicc::HTTPResponseHeader jsonHeader("HTTP/1.1", 200, "OK");
		jsonHeader.addHeader("Content-type", "application/json");
		out->setHTTPResponseHeader(jsonHeader);
	}
	
	// Make a JSON output object
	JSONSpirit::Object output;
	
	// Dig out the DDUs to which I should communicate.
	std::vector<DDU *> targetDDUs;
	// Map the RUIs to the appropriate crate so that later we can return the crate:slot information
	std::map<unsigned int, unsigned int> ruiToCrateMap;
	
	for (std::vector<cgicc::FormEntry>::const_iterator iEntry = cgi.getElements().begin(); iEntry != cgi.getElements().end(); iEntry++) {
		if (iEntry->getName() == "rui") {
			
			std::istringstream ruiStream(iEntry->getValue());
			ruiStream.exceptions(std::istringstream::failbit | std::istringstream::badbit);
			unsigned int rui = 0;
			try {
				ruiStream >> rui;
			} catch (std::istringstream::failure &e) {
				// FIXME Don't do anything for now.
				continue;
			}
			
			// See if we can find the DDU from which the RUI comes
			for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
				
				bool ruiFound = false;
				std::vector<DDU *> dduVector = (*iCrate)->getDDUs();
				for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
					
					if ((*iDDU)->getRUI() == rui) {
						targetDDUs.push_back(*iDDU);
						ruiToCrateMap[rui] = (*iCrate)->getNumber();
						ruiFound = true;
						break;
					}
				}
				
				if (ruiFound) break;
			}
			
		}
		
	}
	
	// Fill the first row with DDU crate/slot information
	JSONSpirit::Array dduArray;
	
	for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
		JSONSpirit::Object dduObject;
		
		dduObject.push_back(JSONSpirit::Pair("rui", (int) (*iDDU)->getRUI()));
		dduObject.push_back(JSONSpirit::Pair("crate", (int) ruiToCrateMap[(*iDDU)->getRUI()]));
		dduObject.push_back(JSONSpirit::Pair("slot", (int) (*iDDU)->getSlot()));
		
		dduArray.push_back(dduObject);
	}
	
	output.push_back(JSONSpirit::Pair("ddus", dduArray));
	
	// Dig out the data from the boards.
	JSONSpirit::Array entryArray;
	
	for (std::vector<cgicc::FormEntry>::const_iterator iEntry = cgi.getElements().begin(); iEntry != cgi.getElements().end(); iEntry++) {		
		if (iEntry->getName() == "reg" && iEntry->getValue().substr(0,3) == "ddu") {
			
			// Figure out which register I am going to read (simple mapping)
			std::istringstream regStream(iEntry->getValue().substr(3));
			regStream.exceptions(std::istringstream::failbit | std::istringstream::badbit);
			unsigned int reg = 0;
			try {
				regStream >> reg;
			} catch (std::istringstream::failure &e) {
				// FIXME Don't do anything for now.
				continue;
			}
			
			// You can't instantiate a variable that crosses case statements, so I have to make sure everything I want exists already.
			JSONSpirit::Object entryObject;
			JSONSpirit::Array valueArray, descriptionArray;
			std::ostringstream sName;
			enum DEVTYPE inFPGA = INFPGA1;
			std::string inFPGAName = "INFPGA1";
			
			// SWITCH!
			switch(reg) {
				
				case 1: // Flash Board ID
					entryObject.push_back(JSONSpirit::Pair("name", "Flash Board ID"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject;
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (*iDDU)->readFlashBoardID()));
						valueObject.push_back(JSONSpirit::Pair("base", 10));
						// No description
						valueArray.push_back(valueObject);
					}
					break;
					
				case 2: // RUI Number
					entryObject.push_back(JSONSpirit::Pair("name", "RUI Number"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject;
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (*iDDU)->readRUI()));
						valueObject.push_back(JSONSpirit::Pair("base", 10));
						// No description
						valueArray.push_back(valueObject);
					}
					break;
					
				case 3: // Flash RUI Number
					entryObject.push_back(JSONSpirit::Pair("name", "Flash RUI Number"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject;
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (*iDDU)->readFlashRUI()));
						valueObject.push_back(JSONSpirit::Pair("base", 10));
						// No description
						valueArray.push_back(valueObject);
					}
					break;
					
				case 4: // Killfiber
					entryObject.push_back(JSONSpirit::Pair("name", "KillFiber"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint32_t value = (*iDDU)->readKillFiber();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						// There are two pieces to the KillFiber:  the killed fibers and the option bits.
						std::map<std::string, std::string> fiberDebug = DDUDebugger::DebugFiber((*iDDU), 0x7fff - (value & 0x7fff), "none");
						std::map<std::string, std::string> fiberDebug2 = DDUDebugger::KillFiber(value);
						fiberDebug.insert(fiberDebug2.begin(), fiberDebug2.end());
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(fiberDebug)));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 5: // Flash Killfiber
					entryObject.push_back(JSONSpirit::Pair("name", "Flash KillFiber"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readFlashKillFiber();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::DebugFiber((*iDDU), 0x7fff - (value & 0x7fff), "none"))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 6: // GbE Prescale
					entryObject.push_back(JSONSpirit::Pair("name", "GbE Prescale"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readGbEPrescale();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::GbEPrescale(value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 7: // DDUFPGA L1A Scaler
					entryObject.push_back(JSONSpirit::Pair("name", "DDUFPGA L1A Scaler"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject;
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (uint64_t) (*iDDU)->readL1Scaler(DDUFPGA)));
						valueObject.push_back(JSONSpirit::Pair("base", 10));
						// No description
						valueArray.push_back(valueObject);
					}
					break;
					
				case 8: // INFPGA0 L1A Scaler (fibers 0-3)
					entryObject.push_back(JSONSpirit::Pair("name", "INFPGA0 L1A Scaler (fibers 0-3)"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject;
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (uint64_t) (*iDDU)->readL1Scaler(INFPGA0)));
						valueObject.push_back(JSONSpirit::Pair("base", 10));
						// No description
						valueArray.push_back(valueObject);
					}
					break;
					
				case 9: // INFPGA0 L1A Scaler (fibers 4-7)
					entryObject.push_back(JSONSpirit::Pair("name", "INFPGA0 L1A Scaler (fibers 4-7)"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject;
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (uint64_t) (*iDDU)->readL1Scaler1(INFPGA0)));
						valueObject.push_back(JSONSpirit::Pair("base", 10));
						// No description
						valueArray.push_back(valueObject);
					}
					break;
					
				case 10: // INFPGA1 L1A Scaler (fibers 8-11)
					entryObject.push_back(JSONSpirit::Pair("name", "INFPGA1 L1A Scaler (fibers 8-11)"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject;
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (uint64_t) (*iDDU)->readL1Scaler(INFPGA1)));
						valueObject.push_back(JSONSpirit::Pair("base", 10));
						// No description
						valueArray.push_back(valueObject);
					}
					break;
					
				case 11: // INFPGA0 L1A Scaler (fibers 12-14)
					entryObject.push_back(JSONSpirit::Pair("name", "INFPGA1 L1A Scaler (fibers 12-14)"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject;
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (uint64_t) (*iDDU)->readL1Scaler1(INFPGA1)));
						valueObject.push_back(JSONSpirit::Pair("base", 10));
						// No description
						valueArray.push_back(valueObject);
					}
					break;
					
				case 12: // FMM Register
					entryObject.push_back(JSONSpirit::Pair("name", "FMM Register"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readFMM();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::FMM(value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 13: // Per-Fiber Error Status 
					entryObject.push_back(JSONSpirit::Pair("name", "Per-Fiber Error Status"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readCSCStatus();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));

						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::DebugFiber((*iDDU), value, "red"))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 14: // Per-Fiber Advanced Error Status 
					entryObject.push_back(JSONSpirit::Pair("name", "Per-Fiber Advanced Error Status"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readAdvancedFiberErrors();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::DebugFiber((*iDDU), value, "red"))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 15: // Per-Fiber FMM Busy
					entryObject.push_back(JSONSpirit::Pair("name", "Per-Fiber FMM Busy"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readFMMBusy();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::DebugFiber((*iDDU), value, "yellow"))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 16: // Per-Fiber FMM Busy History 
					entryObject.push_back(JSONSpirit::Pair("name", "Per-Fiber FMM Busy History"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readBusyHistory();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::DebugFiber((*iDDU), value, "yellow"))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 17: // Per-Fiber FMM Full Warning 
					entryObject.push_back(JSONSpirit::Pair("name", "Per-Fiber FMM Full Warning"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readFMMFullWarning();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::DebugFiber((*iDDU), value, "orange"))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 18: // Per-Fiber FMM Full Warning History 
					entryObject.push_back(JSONSpirit::Pair("name", "Per-Fiber FMM Full Warning History"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readWarningHistory();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::DebugFiber((*iDDU), value, "orange"))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 19: // Per-Fiber FMM Lost Sync 
					entryObject.push_back(JSONSpirit::Pair("name", "Per-Fiber FMM Lost Sync"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readFMMLostSync();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::DebugFiber((*iDDU), value, "red"))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 20: // Per-Fiber FMM Error 
					entryObject.push_back(JSONSpirit::Pair("name", "Per-Fiber FMM Error"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readFMMError();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::DebugFiber((*iDDU), value, "red"))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 21: // DDUFPGA Status 
					entryObject.push_back(JSONSpirit::Pair("name", "DDUFPGA Status"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint32_t value = (*iDDU)->readFPGAStatus(DDUFPGA);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::FPGAStatus(DDUFPGA, value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 22: // DDUFPGA Diagnostic Trap 
					entryObject.push_back(JSONSpirit::Pair("name", "DDUFPGA Diagnostic Trap"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						std::vector<uint16_t> value = (*iDDU)->readDebugTrap(DDUFPGA);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", formatBigNum(value)));
						valueObject.push_back(JSONSpirit::Pair("base", 0));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", formatBigDebug(DDUDebugger::DDUDebugTrap(value, *iDDU))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 23: // Per-Fiber DDUFPGA FIFO-Full 
					entryObject.push_back(JSONSpirit::Pair("name", "Per-Fiber DDUFPGA FIFO-Full"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readFFError();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::FFError(value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 24: // Per-Fiber CRC Error 
					entryObject.push_back(JSONSpirit::Pair("name", "Per-Fiber CRC Error"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readCRCError();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::DebugFiber((*iDDU), value, "red"))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 25: // Per-Fiber Transmit Error 
					entryObject.push_back(JSONSpirit::Pair("name", "Per-Fiber Transmit Error"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readXmitError();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::DebugFiber((*iDDU), value, "red"))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 26: // Per-Fiber DMB Error 
					entryObject.push_back(JSONSpirit::Pair("name", "Per-Fiber DMB Error"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readDMBError();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::DebugFiber((*iDDU), value, "red"))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 27: // Per-Fiber TMB Error 
					entryObject.push_back(JSONSpirit::Pair("name", "Per-Fiber TMB Error"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readTMBError();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::DebugFiber((*iDDU), value, "red"))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 28: // Per-Fiber ALCT Error 
					entryObject.push_back(JSONSpirit::Pair("name", "Per-Fiber ALCT Error"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readALCTError();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::DebugFiber((*iDDU), value, "red"))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 29: // Per-Fiber Lost-In-Event Error 
					entryObject.push_back(JSONSpirit::Pair("name", "Per-Fiber Lost-In-Event Error"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readLIEError();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::DebugFiber((*iDDU), value, "red"))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 30: // Per-Fiber Input Corruption History 
					entryObject.push_back(JSONSpirit::Pair("name", "Per-Fiber Input Corruption History"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readInCHistory();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::InCHistory(value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 31: // Per-Fiber DMB Live 
					entryObject.push_back(JSONSpirit::Pair("name", "Per-Fiber DMB Live"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readDMBLive();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::DebugFiber((*iDDU), (0x7fff - value), "red"))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 32: // Per-Fiber DMB Live at First Event 
					entryObject.push_back(JSONSpirit::Pair("name", "Per-Fiber DMB Live at First Event"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readDMBLiveAtFirstEvent();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::DebugFiber((*iDDU), (0x7fff - value), "yellow"))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 33: // Per-Fiber Warning Status 
					entryObject.push_back(JSONSpirit::Pair("name", "Per-Fiber Warning Status"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readWarningMonitor();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::DebugFiber((*iDDU), value, "orange"))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 34: // Temperature 0
				case 35: // Temperature 1
				case 36: // Temperature 2
				case 37: // Temperature 3
					sName << "Temperature " << (reg - 34);
					entryObject.push_back(JSONSpirit::Pair("name", sName.str()));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						std::ostringstream valueText;
						
						float value = (*iDDU)->readTemperature(reg - 34);
						valueText << std::setprecision(4) << value << " C";
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", valueText.str()));
						valueObject.push_back(JSONSpirit::Pair("base", 0));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::Temperature(value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 38: // Voltage 1.5
				case 39: // Voltage 2.5
				case 40: // Voltage 2.5
				case 41: // Voltage 3.3
					if (reg == 38) sName << "Voltage 1.5";
					else if (reg == 41) sName << "Voltage 3.3";
					else sName << "Voltage 2.5";
					entryObject.push_back(JSONSpirit::Pair("name", sName.str()));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						std::ostringstream valueText;
						
						float value = (*iDDU)->readVoltage(reg - 38);
						valueText << std::setprecision(4) << value << " mV";
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", valueText.str()));
						valueObject.push_back(JSONSpirit::Pair("base", 0));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::Voltage(reg - 38, value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 42: // Fake L1
					entryObject.push_back(JSONSpirit::Pair("name", "Fake L1 Passthrough"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readFakeL1();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::FakeL1(value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 43: // Flash GbE FIFO Thresholds
					entryObject.push_back(JSONSpirit::Pair("name", "Flash GbE FIFO Thresholds"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject;
						
						std::vector<uint16_t> value = (*iDDU)->readFlashGbEFIFOThresholds();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", formatBigNum(value)));
						valueObject.push_back(JSONSpirit::Pair("base", 0));
						
						// No description
						
						valueArray.push_back(valueObject);
					}
					break;
					
				case 44: // InRD Status
					entryObject.push_back(JSONSpirit::Pair("name", "InRD Status"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readInRDStatus();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::InRDStatus(value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 45: // Error Bus Register A
				case 46: // Error Bus Register B
				case 47: // Error Bus Register C
					// Tricky use of chars here
					sName << "Error Bus Register " << char('A' + (reg - 45));
					entryObject.push_back(JSONSpirit::Pair("name", sName.str()));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readEBRegister(reg - 45);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::EBRegister(reg - 45,value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 48: // Maximum L1A-to-Data Timeout
					entryObject.push_back(JSONSpirit::Pair("name", "Maximum L1A-to-Data Timeout"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readMaxTimeoutCount();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::MaxTimeoutCount(value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 49: // Bunch-Crossing Orbit
					entryObject.push_back(JSONSpirit::Pair("name", "Bunch-Crossing Orbit"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject;
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (*iDDU)->readBXOrbit()));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						// No description
						valueArray.push_back(valueObject);
					}
					break;
					
				case 50: // DDUFPGA Output Status
					entryObject.push_back(JSONSpirit::Pair("name", "DDUFPGA Output Status"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readOutputStatus();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::OutputStatus(value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 51: // DDUFPGA Input FIFO Status 0
				case 52: // DDUFPGA Input FIFO Status 1
				case 53: // DDUFPGA Input FIFO Status 2
					sName << "DDUFPGA Input FIFO Status " << (reg - 51);
					entryObject.push_back(JSONSpirit::Pair("name", sName.str()));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readFIFOStatus(reg - 51);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::FIFOStatus(reg - 51, value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 54: // Serial Status
					entryObject.push_back(JSONSpirit::Pair("name", "Serial Status"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint8_t value = (*iDDU)->readSerialStatus();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::SerialStatus(value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 55: // Parallel Status
					entryObject.push_back(JSONSpirit::Pair("name", "Parallel Status"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint8_t value = (*iDDU)->readParallelStatus();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::ParallelStatus(value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 56: // Switches
					entryObject.push_back(JSONSpirit::Pair("name", "Switches"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject;
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (*iDDU)->readSwitches()));
						valueObject.push_back(JSONSpirit::Pair("base", 2));
						// No description
						valueArray.push_back(valueObject);
					}
					break;
					
				case 57: // Input Register 0
				case 58: // Input Register 1
				case 59: // Input Register 2
					sName << "Input Register " << (reg - 57);
					entryObject.push_back(JSONSpirit::Pair("name", sName.str()));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject;
						
						uint16_t value = (*iDDU)->readInputRegister(reg - 57);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						// No description
						valueArray.push_back(valueObject);
					}
					break;
					
				case 60: // Test Register 0
				case 61: // Test Register 1
				case 62: // Test Register 2
				case 63: // Test Register 3
				case 64: // Test Register 4
					sName << "Test Register " << (reg - 60);
					entryObject.push_back(JSONSpirit::Pair("name", sName.str()));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject;
						
						uint16_t value = (*iDDU)->readTestRegister(reg - 60);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						// No description
						valueArray.push_back(valueObject);
					}
					break;
					
				case 65: // INFPGA0 Status
					inFPGA = INFPGA0;
					inFPGAName = "INFPGA0";
				case 85: // INFPGA1 Status
					sName << inFPGAName << " Status";
					entryObject.push_back(JSONSpirit::Pair("name", sName.str()));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint32_t value = (*iDDU)->readFPGAStatus(inFPGA);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::FPGAStatus(inFPGA, value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 66: // INFPGA0 Diagnostic Trap
					inFPGA = INFPGA0;
					inFPGAName = "INFPGA0";
				case 86: // INFPGA1 Diagnostic Trap
					sName << inFPGAName << " Diagnostic Trap";
					entryObject.push_back(JSONSpirit::Pair("name", sName.str()));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						std::vector<uint16_t> value = (*iDDU)->readDebugTrap(inFPGA);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", formatBigNum(value)));
						valueObject.push_back(JSONSpirit::Pair("base", 0));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", formatBigDebug(DDUDebugger::INFPGADebugTrap(inFPGA, value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 67: // INFPGA0 Fiber Status (fibers 0-7)
					inFPGA = INFPGA0;
					inFPGAName = "INFPGA0";
				case 87: // INFPGA1 Fiber Status (fibers 8-14)
					sName << inFPGAName << " Fiber Status";
					entryObject.push_back(JSONSpirit::Pair("name", sName.str()));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readFiberStatus(inFPGA);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::FiberStatus(inFPGA, value, (*iDDU)))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 68: // INFPGA0 DMB Sync (fibers 0-7)
					inFPGA = INFPGA0;
					inFPGAName = "INFPGA0";
				case 88: // INFPGA1 DMB Sync (fibers 8-14)
					sName << inFPGAName << " DMB Sync";
					entryObject.push_back(JSONSpirit::Pair("name", sName.str()));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readDMBSync(inFPGA);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::DMBSync(inFPGA, value, (*iDDU)))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 69: // INFPGA0 FIFO Status (fibers 0-7)
					inFPGA = INFPGA0;
					inFPGAName = "INFPGA0";
				case 89: // INFPGA1 FIFO Status (fibers 8-14)
					sName << inFPGAName << " FIFO Status";
					entryObject.push_back(JSONSpirit::Pair("name", sName.str()));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readFIFOStatus(inFPGA);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::FIFOStatus(inFPGA, value, (*iDDU)))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 70: // INFPGA0 FIFO Full (fibers 0-7)
					inFPGA = INFPGA0;
					inFPGAName = "INFPGA0";
				case 90: // INFPGA1 FIFO Full (fibers 8-14)
					sName << inFPGAName << " FIFO Full";
					entryObject.push_back(JSONSpirit::Pair("name", sName.str()));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readFIFOFull(inFPGA);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::FIFOFull(inFPGA, value, (*iDDU)))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 71: // INFPGA0 Link Receive Error (fibers 0-7)
					inFPGA = INFPGA0;
					inFPGAName = "INFPGA0";
				case 91: // INFPGA1 Link Receive Error (fibers 8-14)
					sName << inFPGAName << " Link Receive Error";
					entryObject.push_back(JSONSpirit::Pair("name", sName.str()));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readRxError(inFPGA);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::RxError(inFPGA, value, (*iDDU)))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 72: // INFPGA0 Link Transmit Error (fibers 0-7)
					inFPGA = INFPGA0;
					inFPGAName = "INFPGA0";
				case 92: // INFPGA1 Link Transmit Error (fibers 8-14)
					sName << inFPGAName << " Link Transmit Error";
					entryObject.push_back(JSONSpirit::Pair("name", sName.str()));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readTxError(inFPGA);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::TxError(inFPGA, value, (*iDDU)))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 73: // INFPGA0 Timeout Error (fibers 0-7)
					inFPGA = INFPGA0;
					inFPGAName = "INFPGA0";
				case 93: // INFPGA1 Timeout Error (fibers 8-14)
					sName << inFPGAName << " Timeout Error";
					entryObject.push_back(JSONSpirit::Pair("name", sName.str()));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readTimeout(inFPGA);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::Timeout(inFPGA, value, (*iDDU)))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 74: // INFPGA0 Memory Active (fibers 0-1)
				case 75: // INFPGA0 Memory Active (fibers 2-3)
				case 76: // INFPGA0 Memory Active (fibers 4-5)
				case 77: // INFPGA0 Memory Active (fibers 6-7)
					inFPGA = INFPGA0;
					inFPGAName = "INFPGA0";
				case 94: // INFPGA1 Memory Active (fibers 8-9)
				case 95: // INFPGA1 Memory Active (fibers 10-11)
				case 96: // INFPGA1 Memory Active (fibers 12-13)
				case 97: // INFPGA1 Memory Active (fiber 14)
					sName << inFPGAName << " Memory Active";
					entryObject.push_back(JSONSpirit::Pair("name", sName.str()));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readActiveWriteMemory(inFPGA, (reg - 74) % 20);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::ActiveWriteMemory(inFPGA, (reg - 74) % 20, value, (*iDDU)))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 78: // INFPGA0 Memory Free (fibers 0-7)
					inFPGA = INFPGA0;
					inFPGAName = "INFPGA0";
				case 98: // INFPGA1 Memory Free (fibers 8-14)
					sName << inFPGAName << " Memory Free";
					entryObject.push_back(JSONSpirit::Pair("name", sName.str()));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readAvailableMemory(inFPGA);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::AvailableMemory(inFPGA, value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 79: // INFPGA0 Minimum Memory Free (fibers 0-7)
					inFPGA = INFPGA0;
					inFPGAName = "INFPGA0";
				case 99: // INFPGA1 Minimum Memory Free (fibers 8-14)
					sName << inFPGAName << " Minimum Memory Free";
					entryObject.push_back(JSONSpirit::Pair("name", sName.str()));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readMinMemory(inFPGA);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::AvailableMemory(inFPGA, value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 80: // INFPGA0 Lost-In-Event (fibers 0-7)
					inFPGA = INFPGA0;
					inFPGAName = "INFPGA0";
				case 100: // INFPGA1 Lost-In-Event (fibers 8-14)
					sName << inFPGAName << " Lost-In-Event";
					entryObject.push_back(JSONSpirit::Pair("name", sName.str()));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readLostError(inFPGA);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::LostError(inFPGA, value, (*iDDU)))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 81: // INFPGA0 \"C\" Code (fibers 0-7)
					inFPGA = INFPGA0;
					inFPGAName = "INFPGA0";
				case 101: // INFPGA1 \"C\" Code (fibers 8-14)
					sName << inFPGAName << " \"C\" Code";
					entryObject.push_back(JSONSpirit::Pair("name", sName.str()));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readCCodeStatus(inFPGA);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::CCodeStatus(inFPGA, value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 82: // INFPGA0 DMB Warning (fibers 0-7)
					inFPGA = INFPGA0;
					inFPGAName = "INFPGA0";
				case 102: // INFPGA1 DMB Warning (fibers 8-14)
					sName << inFPGAName << " DMB Warning";
					entryObject.push_back(JSONSpirit::Pair("name", sName.str()));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readDMBWarning(inFPGA);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::DMBWarning(inFPGA, value, (*iDDU)))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 83: // INFPGA0 Fiber Diagnostic Register 0 (fibers 0-7)
				case 84: // INFPGA0 Fiber Diagnostic Register 1 (fibers 0-7)
					inFPGA = INFPGA0;
					inFPGAName = "INFPGA0";
				case 103: // INFPGA1 Fiber Diagnostic Register 0 (fibers 8-14)
				case 104: // INFPGA1 Fiber Diagnostic Register 1 (fibers 8-14)
					sName << inFPGAName << " Fiber Diagnostic Register " << ((reg - 83) % 20);
					entryObject.push_back(JSONSpirit::Pair("name", sName.str()));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject;
						
						uint32_t value = (*iDDU)->readFiberDiagnostics(inFPGA, (reg - 83) % 20);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						valueObject.push_back(JSONSpirit::Pair("base", 16));
						// No diagnostics?
						valueArray.push_back(valueObject);
					}
					break;
					
				default:
					
					break;

			}
			
			entryObject.push_back(JSONSpirit::Pair("values", valueArray));
			entryObject.push_back(JSONSpirit::Pair("descriptions", descriptionArray));
			
			if (entryObject.size()) entryArray.push_back(entryObject);
		}
		
	}
	
	if (entryArray.size()) output.push_back(JSONSpirit::Pair("entries", entryArray));
	
	*out << JSONSpirit::write(output);

}



void emu::fed::Commander::webDisplayDDURegisters(xgi::Input *in, xgi::Output *out)
{
	// Everything here is embedded in the GET statement.
	// I just pass it on to the GetDDURegisters page and the javascript handles the rest.

	*out << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">" << std::endl;
	*out << "<html xmlns=\"http://www.w3.org/1999/xhtml\">" << std::endl;
	*out << cgicc::head() << std::endl;
	*out << "<link rel=\"stylesheet\" type=\"text/css\" href=\"/emu/emuDCS/FEDApps/html/FEDApps.css\" />" << std::endl;
	
	// Include the javascript files
	// This is a universal global that I want to always have around.
	*out << "<script type=\"text/javascript\">var URL = \"" << getApplicationDescriptor()->getContextDescriptor()->getURL() << "/" << getApplicationDescriptor()->getURN() << "\";</script>";
	// Always include prototype
	*out << "<script type=\"text/javascript\" src=\"/emu/emuDCS/FEDApps/html/prototype.js\"></script>" << std::endl;
	*out << "<script type=\"text/javascript\" src=\"/emu/emuDCS/FEDApps/html/table.js\"></script>" << std::endl;
	*out << "<script type=\"text/javascript\" src=\"/emu/emuDCS/FEDApps/html/commanderDisplay.js\"></script>" << std::endl;
	*out << "<script type=\"text/javascript\" src=\"/emu/emuDCS/FEDApps/html/definitions.js\"></script>" << std::endl;
	*out << "<script type=\"text/javascript\" src=\"/emu/emuDCS/FEDApps/html/common.js\"></script>" << std::endl;
	
	*out << cgicc::head() << std::endl;
	*out << cgicc::body() << std::endl;
	
	*out << cgicc::div()
		.set("class", "titlebar monitor_width")
		.set("id", "FED_Commander_titlebar") << std::endl;
	*out << cgicc::div("Commander Display")
		.set("class", "titletext") << std::endl;
	*out << cgicc::div() << std::endl;
	
	*out << cgicc::div()
		.set("class", "statusbar monitor_width")
		.set("id", "FED_Commander_statusbar") << std::endl;
	*out << cgicc::div("Time of last update:")
		.set("class", "timetext") << std::endl;
	*out << cgicc::div("never")
		.set("class", "loadtime")
		.set("name", "FED_Commander")
		.set("id", "FED_Commander_loadtime") << std::endl;
	*out << cgicc::img()
		.set("class", "loadicon")
		.set("id", "FED_Commander_loadicon")
		.set("src", "/emu/emuDCS/FEDApps/images/empty.gif")
		.set("alt", "Loading...") << std::endl;
	*out << cgicc::br()
		.set("class", "clear") << std::endl;
	*out << cgicc::div() << std::endl;
	
	*out << cgicc::fieldset()
		.set("class", "dialog monitor_width")
		.set("id", "FED_Commander_dialog") << std::endl;
		
	// Useful display modes
	*out << cgicc::table()
		.set("class", "noborder dialog tier1") << std::endl;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::td("Current display mode: ") << std::endl;
	*out << cgicc::td() << std::endl;
	*out << cgicc::select()
		.set("id", "display_select")
		.set("class", "monitor_select")
		.set("name", "FED_Commander") << std::endl;
	*out << cgicc::option("HTML")
		.set("value", "html")
		.set("selected", "true");
	*out << cgicc::option("Text")
		.set("value", "text");
	*out << cgicc::select() << std::endl;
	*out << cgicc::td() << std::endl;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::table() << std::endl;
	
	// A place for the content
	*out << cgicc::div()
		.set("id", "content") << std::endl;
	*out << cgicc::div() << std::endl;
	
	// Update buttons
	*out << cgicc::button()
		.set("id", "display_refresh")
		.set("class", "right button statechange pause_button")
		.set("name", "FED_Commander")
		.set("command", "refresh") << std::endl;
	*out << cgicc::img()
		.set("class", "icon")
		.set("src", "/emu/emuDCS/FEDApps/images/view-refresh.png");
	*out << "Refresh now" << std::endl;
	*out << cgicc::button() << std::endl;
	
	*out << cgicc::button()
		.set("id", "display_start")
		.set("class", "right button statechange start_button")
		.set("name", "FED_Commander")
		.set("command", "start") << std::endl;
	*out << cgicc::img()
		.set("class", "icon")
		.set("src", "/emu/emuDCS/FEDApps/images/media-playback-start.png");
	*out << "Begin refreshing" << std::endl;
	*out << cgicc::button() << std::endl;
	
	*out << cgicc::button()
		.set("id", "display_pause")
		.set("class", "right button statechange pause_button")
		.set("name", "FED_Commander")
		.set("command", "pause")
		.set("disabled", "true") << std::endl;
	*out << cgicc::img()
		.set("class", "icon")
		.set("src", "/emu/emuDCS/FEDApps/images/media-playback-pause.png");
	*out << "Pause refreshing" << std::endl;
	*out << cgicc::button() << std::endl;
	
	*out << cgicc::fieldset() << std::endl;
	
	*out << cgicc::body() << std::endl;
	*out << cgicc::html() << std::endl;
}



void emu::fed::Commander::webGetStatus(xgi::Input *in, xgi::Output *out)
{
	// Configure yourself if you haven't yet.  This is a software-only configure.
	if (!crateVector_.size()) {
		try {
			softwareConfigure();
			REVOKE_ALARM("CommanderGetStatus", NULL);
		} catch (emu::fed::exception::ConfigurationException &e) {
			std::ostringstream error;
			error << "Unable to properly configure the Commander appliction";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "CommanderGetStatus", "ERROR", error.str(), e.getProperty("tag"), NULL, e);
		}
	}
	
	cgicc::Cgicc cgi(in);
	
	// Need some header information to be able to return JSON
	if (cgi.getElement("debug") == cgi.getElements().end() || cgi["debug"]->getIntegerValue() != 1) {
		cgicc::HTTPResponseHeader jsonHeader("HTTP/1.1", 200, "OK");
		jsonHeader.addHeader("Content-type", "application/json");
		out->setHTTPResponseHeader(jsonHeader);
	}
	
	// Make a JSON output object
	JSONSpirit::Object output;
	
	// Cycle through crates
	JSONSpirit::Array crateArray;
	for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
		
		JSONSpirit::Object crateObject;
		
		crateObject.push_back(JSONSpirit::Pair("number", (int) (*iCrate)->getNumber()));
		
		// Make an array of DDUs
		JSONSpirit::Array dduArray;
		
		// Cycle through DDUs
		std::vector<DDU *> dduVector = (*iCrate)->getDDUs();
		for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
			
			// Get universal DDU information
			JSONSpirit::Object dduObject;
			
			// Slot number
			dduObject.push_back(JSONSpirit::Pair("slot", (int) (*iDDU)->getSlot()));
			
			// FMM status (decoded)
			std::string statusDecoded = "undefined";
			try {
				uint8_t fmmStatus = (*iDDU)->readRealFMM();
				statusDecoded = DDUDebugger::RealFMM(fmmStatus).second;
				REVOKE_ALARM("CommanderGetStatusDDU", NULL);
			} catch (emu::fed::exception::DDUException &e) {
				dduObject.push_back(JSONSpirit::Pair("exception", e.what()));
				std::ostringstream tag(e.getProperty("tag"));
				tag << " FEDcrate " << (*iCrate)->getNumber();
				RAISE_ALARM_NESTED(emu::fed::exception::Exception, "CommanderGetStatusDDU", "ERROR", e.what(), tag.str(), NULL, e);
			}
			dduObject.push_back(JSONSpirit::Pair("status", statusDecoded));
			
			// Get fiber status
			uint16_t fiberStatus = 0;
			uint16_t liveFibers = 0;
			uint16_t killedFibers = 0xffff;
			try {
				fiberStatus = (*iDDU)->readFiberErrors();
				liveFibers = (*iDDU)->readLiveFibers();
				killedFibers = (*iDDU)->readKillFiber();
				REVOKE_ALARM("CommanderGetStatusFibers", NULL);
			} catch (emu::fed::exception::DDUException &e) {
				dduObject.push_back(JSONSpirit::Pair("exception", e.what()));
				std::ostringstream tag(e.getProperty("tag"));
				tag << " FEDcrate " << (*iCrate)->getNumber();
				RAISE_ALARM_NESTED(emu::fed::exception::Exception, "CommanderGetStatusFibers", "ERROR", e.what(), tag.str(), NULL, e);
				// All the fibers will look dead.  I guess that's exception enough?
			}
			
			// Cycle through fibers
			JSONSpirit::Array fiberArray;
			for (size_t iFiber = 0; iFiber < 15; iFiber++) {
				
				JSONSpirit::Object fiberObject;
				
				// Fiber number
				fiberObject.push_back(JSONSpirit::Pair("number", (int) iFiber));
				
				// Get the fiber
				Fiber *myFiber = (*iDDU)->getFiber(iFiber);
				std::string myClass = "ok";
				if (!(killedFibers & (1 << myFiber->getFiberNumber()))) myClass = "killed";
				else if (!(liveFibers & (1 << myFiber->getFiberNumber()))) myClass = "undefined";
				else if (fiberStatus & (1 << myFiber->getFiberNumber())) myClass = "error";
				
				fiberObject.push_back(JSONSpirit::Pair("status", myClass));
				
				fiberArray.push_back(fiberObject);
			}
			
			dduObject.push_back(JSONSpirit::Pair("fibers", fiberArray));
			
			dduArray.push_back(dduObject);
			
		}
		
		crateObject.push_back(JSONSpirit::Pair("ddus", dduArray));
		
		// Make an array of DCCs
		JSONSpirit::Array dccArray;
		
		// Cycle through DCCs
		std::vector<DCC *> dccVector = (*iCrate)->getDCCs();
		for (std::vector<DCC *>::iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); iDCC++) {
			
			// Get universal DCC information
			JSONSpirit::Object dccObject;
			
			// Slot number
			dccObject.push_back(JSONSpirit::Pair("slot", (int) (*iDCC)->getSlot()));
			
			// FMM status and fifo status simultaneously
			uint16_t fifoStatus = 0;
			uint8_t slinkStatus = 0;
			std::string statusDecoded = "undefined";
			try {
				uint8_t fmmStatus = (*iDCC)->readFMMStatus();
				std::pair<std::string, std::string> debugged = DCCDebugger::FMMStat(fmmStatus);
				statusDecoded = debugged.second;
				
				fifoStatus = (*iDCC)->readFIFOStatus();
				
				slinkStatus = (*iDCC)->readSLinkStatus();
				REVOKE_ALARM("CommanderGetStatusDCC", NULL);
			} catch (emu::fed::exception::DCCException &e) {
				dccObject.push_back(JSONSpirit::Pair("exception", e.what()));
				std::ostringstream tag(e.getProperty("tag"));
				tag << " FEDcrate " << (*iCrate)->getNumber();
				RAISE_ALARM_NESTED(emu::fed::exception::Exception, "CommanderGetStatusDCC", "ERROR", e.what(), tag.str(), NULL, e);
			}
			dccObject.push_back(JSONSpirit::Pair("status", statusDecoded));

			// DDU FIFO rates
			JSONSpirit::Array fifoArray;
			for (size_t iFIFO = 0; iFIFO <= 9; iFIFO++) {
				JSONSpirit::Object fifoObject;
				
				fifoObject.push_back(JSONSpirit::Pair("number", (int) iFIFO));
				
				// Each bit corresponds to two FIFOs
				unsigned int jFIFO = iFIFO/2;
				
				std::string status = "ok";
				
				FIFO *fifo = (*iDCC)->getFIFO(iFIFO);
				
				if (!fifo->isUsed()) {
					status = "killed";
				} else if (!(fifoStatus & (1 << (jFIFO + 3)))) {
					status = "error";
				} else if (jFIFO < 3 && !(fifoStatus & (1 << jFIFO))) {
					status = "warning";
				}

				fifoObject.push_back(JSONSpirit::Pair("status", status));
				
				fifoArray.push_back(fifoObject);
			}
			dccObject.push_back(JSONSpirit::Pair("fifos", fifoArray));
			
			// SLink rates
			JSONSpirit::Array slinkArray;
			for (unsigned int iLink = 1; iLink <= 2; iLink++) {
				
				JSONSpirit::Object slinkObject;
				
				slinkObject.push_back(JSONSpirit::Pair("number", (int) iLink));

				std::string status = "ok";
				
				if (!(slinkStatus & (1 << (iLink * 2 - 1)))) {
					status = "undefined";
				} else if (!(slinkStatus & (1 << ((iLink - 1) * 2 )))) {
					status = "warning";
				}
				
				slinkObject.push_back(JSONSpirit::Pair("status", status));
				
				slinkArray.push_back(slinkObject);
			}
			
			dccObject.push_back(JSONSpirit::Pair("slinks", slinkArray));
			
			dccArray.push_back(dccObject);
		}
		
		crateObject.push_back(JSONSpirit::Pair("dccs", dccArray));
		
		crateArray.push_back(crateObject);
	}
	
	output.push_back(JSONSpirit::Pair("crates", crateArray));
	
	*out << JSONSpirit::write(output);
}



// PGK Ugly, but it must be done.  We have to update the parameters that the
//  Manager asks for or else they won't be updated!
xoap::MessageReference emu::fed::Commander::onGetParameters(xoap::MessageReference message)
{
	// Configure yourself if you haven't yet.  This is a software-only configure.
	if (!crateVector_.size()) {
		try {
			softwareConfigure();
			REVOKE_ALARM("CommanderGetParameters", NULL);
		} catch (emu::fed::exception::ConfigurationException &e) {
			std::ostringstream error;
			error << "Unable to properly configure the Commander appliction";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "CommanderGetParameters", "ERROR", error.str(), e.getProperty("tag"), NULL, e);
		}
	}
	return emu::fed::Application::onGetParameters(message);
}



std::string emu::fed::Commander::printRegisterTable(const std::vector<Register> &registers, const std::string &id)
{
	std::ostringstream out;
	
	out << cgicc::table()
		.set("class", "registers tier1") << std::endl;
	
	for (unsigned int iRegister = 0; iRegister < registers.size(); iRegister++) {
		
		// Three is a nice number of columns
		if (iRegister % 3 == 0) {
			out << cgicc::tr()
				.set("class", "register") << std::endl;
		}
		
		const Register &myRegister = registers[iRegister];
		
		out << cgicc::td()
			.set("class", "checkbox") << std::endl;
		out << cgicc::input()
			.set("class", id + "_checkbox")
			.set("type", "checkbox")
			.set("id", myRegister.id + "_checkbox")
			.set("name", myRegister.id) << std::endl;
		out << cgicc::td() << std::endl;
		out << cgicc::td()
			.set("class", "register") << std::endl;
		out << cgicc::label()
			.set("for", myRegister.id + "_checkbox")
			.set("class", myRegister.style) << std::endl;
		out << myRegister.description << std::endl;
		out << cgicc::label() << std::endl;
		out << cgicc::td() << std::endl;
		
		if (iRegister % 3 == 2 || iRegister == registers.size()) {
			out << cgicc::tr() << std::endl;
		}
	}
	
	out << cgicc::table() << std::endl;
	
	return out.str();
}



std::string emu::fed::Commander::printDebug(const std::map<std::string, std::string> &debug)
{
	std::ostringstream out;
	for (std::map<std::string, std::string>::const_iterator iPair = debug.begin(); iPair != debug.end(); iPair++) {
		out << printDebug(*iPair) << std::endl;
	}
	return out.str();
}



std::string emu::fed::Commander::printDebug(const std::pair<std::string, std::string> &debug)
{
	std::ostringstream out;
	out << cgicc::div(debug.first)
		.set("class", debug.second);
	return out.str();
}



std::string emu::fed::Commander::formatBigNum(const std::vector<uint16_t> &bigNum)
{
	std::ostringstream out;
	out << std::setfill('0') << std::hex;
	// If there are an odd number of 16-bit values, don't combine the MSB with anything
	unsigned int offset = 0;
	if (bigNum.size() % 2) {
		out << std::setw(4) << bigNum.back();
		if (bigNum.size() > 2) out << " ";
		offset = 1;
	}
	for (std::vector<uint16_t>::const_reverse_iterator iValue = bigNum.rbegin() + offset; iValue != bigNum.rend(); iValue+=2) {
		out << std::setw(4) << *iValue << std::setw(4) << *(iValue + 1);
		// Add a space if this is not the last one
		if (iValue + 2 != bigNum.rend()) out << " ";
	}
	return out.str();
}



std::string emu::fed::Commander::formatBigDebug(const std::vector<std::string> &debug)
{
	std::ostringstream out;
	for (std::vector<std::string>::const_iterator iDebug = debug.begin(); iDebug != debug.end(); iDebug++) {
		out << cgicc::div(*iDebug) << std::endl;
	}
	return out.str();
}
