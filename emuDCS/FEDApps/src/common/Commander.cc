/*****************************************************************************\
* $Id: Commander.cc,v 1.9 2009/09/29 13:51:00 paste Exp $
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
	*out << cgicc::button("Set Fake L1 Register")
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
	
	DDURegisters.push_back(Register("Fake L1", "ddu42"));
	DDURegisters.push_back(Register("Flash GbE FIFO Thresholds", "ddu43"));
	DDURegisters.push_back(Register("InRD Status", "ddu44"));
	DDURegisters.push_back(Register("Error Register \"B\"", "ddu45"));
	DDURegisters.push_back(Register("Maximum L1A-to-Data Timeout", "ddu46"));
	DDURegisters.push_back(Register("Bunch-Crossing Orbit", "ddu47"));
	DDURegisters.push_back(Register("DDUFPGA Output Status", "ddu48"));
	DDURegisters.push_back(Register("DDUFPGA Input FIFO Status", "ddu49"));
	DDURegisters.push_back(Register("Serial Status", "ddu50"));
	DDURegisters.push_back(Register("Parallel Status", "ddu51"));
	DDURegisters.push_back(Register("Switches", "ddu52"));
	
	DDURegisters.push_back(Register("Input Register 0", "ddu53"));
	DDURegisters.push_back(Register("Input Register 1", "ddu54"));
	DDURegisters.push_back(Register("Input Register 2", "ddu55"));
	DDURegisters.push_back(Register("Test Register 0", "ddu56"));
	DDURegisters.push_back(Register("Test Register 1", "ddu57"));
	DDURegisters.push_back(Register("Test Register 2", "ddu58"));
	DDURegisters.push_back(Register("Test Register 3", "ddu59"));
	DDURegisters.push_back(Register("Test Register 4", "ddu60"));

	DDURegisters.push_back(Register("INFPGA0 Status", "ddu61"));
	DDURegisters.push_back(Register("INFPGA0 Diagnostic Trap", "ddu62"));
	DDURegisters.push_back(Register("INFPGA0 Fiber Status (fibers 0-7)", "ddu63"));
	DDURegisters.push_back(Register("INFPGA0 DMB Sync (fibers 0-7)", "ddu64"));
	DDURegisters.push_back(Register("INFPGA0 FIFO Status (fibers 0-7)", "ddu65"));
	DDURegisters.push_back(Register("INFPGA0 FIFO Full (fibers 0-7)", "ddu66"));
	DDURegisters.push_back(Register("INFPGA0 Link Receive Error (fibers 0-7)", "ddu67"));
	DDURegisters.push_back(Register("INFPGA0 Link Transmit Error (fibers 0-7)", "ddu68"));
	DDURegisters.push_back(Register("INFPGA0 Timeout Error (fibers 0-7)", "ddu69"));
	DDURegisters.push_back(Register("INFPGA0 Memory Active (fibers 0-7)", "ddu70"));
	DDURegisters.push_back(Register("INFPGA0 Memory Free (fibers 0-7)", "ddu71"));
	DDURegisters.push_back(Register("INFPGA0 Minimum Memory Free (fibers 0-7)", "ddu72"));
	DDURegisters.push_back(Register("INFPGA0 Lost-In-Event (fibers 0-7)", "ddu73"));
	DDURegisters.push_back(Register("INFPGA0 \"C\" Code (fibers 0-7)", "ddu74"));
	DDURegisters.push_back(Register("INFPGA0 DMB Warning (fibers 0-7)", "ddu75"));
	DDURegisters.push_back(Register("INFPGA0 Fiber Diagnostic Register 0 (fibers 0-7)", "ddu76"));
	DDURegisters.push_back(Register("INFPGA0 Fiber Diagnostic Register 1 (fibers 0-7)", "ddu77"));
	
	DDURegisters.push_back(Register("INFPGA1 Status", "ddu78"));
	DDURegisters.push_back(Register("INFPGA1 Diagnostic Trap", "ddu79"));
	DDURegisters.push_back(Register("INFPGA1 Fiber Status (fibers 8-14)", "ddu80"));
	DDURegisters.push_back(Register("INFPGA1 DMB Sync (fibers 8-14)", "ddu81"));
	DDURegisters.push_back(Register("INFPGA1 FIFO Status (fibers 8-14)", "ddu82"));
	DDURegisters.push_back(Register("INFPGA1 FIFO Full (fibers 8-14)", "ddu83"));
	DDURegisters.push_back(Register("INFPGA1 Link Receive Error (fibers 8-14)", "ddu84"));
	DDURegisters.push_back(Register("INFPGA1 Link Transmit Error (fibers 8-14)", "ddu85"));
	DDURegisters.push_back(Register("INFPGA1 Timeout Error (fibers 8-14)", "ddu86"));
	DDURegisters.push_back(Register("INFPGA1 Memory Active (fibers 8-14)", "ddu87"));
	DDURegisters.push_back(Register("INFPGA1 Memory Free (fibers 8-14)", "ddu88"));
	DDURegisters.push_back(Register("INFPGA1 Minimum Memory Free (fibers 8-14)", "ddu89"));
	DDURegisters.push_back(Register("INFPGA1 Lost-In-Event (fibers 8-14)", "ddu90"));
	DDURegisters.push_back(Register("INFPGA1 \"C\" Code (fibers 8-14)", "ddu91"));
	DDURegisters.push_back(Register("INFPGA1 DMB Warning (fibers 8-14)", "ddu92"));
	DDURegisters.push_back(Register("INFPGA1 Fiber Diagnostic Register 0 (fibers 8-14)", "ddu93"));
	DDURegisters.push_back(Register("INFPGA1 Fiber Diagnostic Register 1 (fibers 8-14)", "ddu94"));
	
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
			
			JSONSpirit::Object entryObject;
			JSONSpirit::Array valueArray, descriptionArray;
			
			// SWITCH!
			switch(reg) {
				
				case 1: // Flash Board ID
					entryObject.push_back(JSONSpirit::Pair("name", "Flash Board ID"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject;
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (*iDDU)->readFlashBoardID()));
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
						// No description
						valueArray.push_back(valueObject);
					}
					break;
					
				case 4: // Killfiber
					entryObject.push_back(JSONSpirit::Pair("name", "KillFiber"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject;
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) (*iDDU)->readKillFiber()));
						// No description
						valueArray.push_back(valueObject);
					}
					break;
					
				case 5: // Flash Killfiber
					entryObject.push_back(JSONSpirit::Pair("name", "Flash KillFiber"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject;
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (*iDDU)->readFlashKillFiber()));
						// No description
						valueArray.push_back(valueObject);
					}
					break;
					
				case 6: // GbE Prescale
					entryObject.push_back(JSONSpirit::Pair("name", "GbE Prescale"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject;
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (*iDDU)->readGbEPrescale()));
						// No description
						valueArray.push_back(valueObject);
					}
					break;
					
				case 7: // DDUFPGA L1A Scaler
					entryObject.push_back(JSONSpirit::Pair("name", "DDUFPGA L1A Scaler"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject;
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (uint64_t) (*iDDU)->readL1Scaler(DDUFPGA)));
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
						// No description
						valueArray.push_back(valueObject);
					}
					break;
					
				case 12: // FMM Register
					entryObject.push_back(JSONSpirit::Pair("name", "FMM Register"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject;
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (*iDDU)->readFMM()));
						// No description
						valueArray.push_back(valueObject);
					}
					break;
					
				case 13: // Per-Fiber Error Status 
					entryObject.push_back(JSONSpirit::Pair("name", "Per-Fiber Error Status"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readCSCStatus();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", value));

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
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::DDUFPGAStat(value))));
						
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
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::DebugFiber((*iDDU), value, "orange"))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 34: // Temperature 0
					entryObject.push_back(JSONSpirit::Pair("name", "Temperature 0"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						float value = (*iDDU)->readTemperature(0);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", value));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::Temperature(value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 35: // Temperature 1
					entryObject.push_back(JSONSpirit::Pair("name", "Temperature 1"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						float value = (*iDDU)->readTemperature(1);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", value));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::Temperature(value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 36: // Temperature 2
					entryObject.push_back(JSONSpirit::Pair("name", "Temperature 2"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						float value = (*iDDU)->readTemperature(2);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", value));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::Temperature(value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 37: // Temperature 3
					entryObject.push_back(JSONSpirit::Pair("name", "Temperature 3"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						float value = (*iDDU)->readTemperature(3);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", value));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::Temperature(value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 38: // Voltage 1.5
					entryObject.push_back(JSONSpirit::Pair("name", "Voltage 1.5"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						float value = (*iDDU)->readVoltage(0);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", value));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::Voltage(0, value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 39: // Voltage 2.5 (1)
					entryObject.push_back(JSONSpirit::Pair("name", "Voltage 2.5 (1)"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						float value = (*iDDU)->readVoltage(1);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", value));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::Voltage(1, value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 40: // Voltage 2.5 (2)
					entryObject.push_back(JSONSpirit::Pair("name", "Voltage 2.5 (2)"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						float value = (*iDDU)->readVoltage(2);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", value));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::Voltage(2, value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 41: // Voltage 3.3
					entryObject.push_back(JSONSpirit::Pair("name", "Voltage 3.3"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						float value = (*iDDU)->readVoltage(3);
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", value));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::Voltage(3, value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 42: // Fake L1
					entryObject.push_back(JSONSpirit::Pair("name", "Fake L1"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readFakeL1();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::FakeL1Reg(value))));
						
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
						
						// No description
						
						valueArray.push_back(valueObject);
					}
					break;
					
				case 44: // InRD Status
					entryObject.push_back(JSONSpirit::Pair("name", "InRD Status"));
					for (std::vector<DDU *>::iterator iDDU = targetDDUs.begin(); iDDU != targetDDUs.end(); iDDU++) {
						JSONSpirit::Object valueObject, descriptionObject;
						
						uint16_t value = (*iDDU)->readInRDStat();
						valueObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						valueObject.push_back(JSONSpirit::Pair("value", (int) value));
						
						descriptionObject.push_back(JSONSpirit::Pair("rui", (*iDDU)->getRUI()));
						descriptionObject.push_back(JSONSpirit::Pair("value", printDebug(DDUDebugger::InRDStat(value))));
						
						valueArray.push_back(valueObject);
						descriptionArray.push_back(descriptionObject);
					}
					break;
					
				case 45: // Error Register \"B\"
					
					break;
					
				case 46: // Maximum L1A-to-Data Timeout
					
					break;
					
				case 47: // Bunch-Crossing Orbit
					
					break;
					
				case 48: // DDUFPGA Output Status
					
					break;
					
				case 49: // DDUFPGA Input FIFO Status
					
					break;
					
				case 50: // Serial Status
					
					break;
					
				case 51: // Parallel Status
					
					break;
					
				case 52: // Switches
					
					break;
					
				case 53:
					
					break;
					
				case 54:
					
					break;
					
				case 55:
					
					break;
					
				case 56:
					
					break;
					
				case 57:
					
					break;
					
				case 58:
					
					break;
					
				case 59:
					
					break;
					
				case 60:
					
					break;
					
				case 61:
					
					break;
					
				case 62:
					
					break;
					
				case 63:
					
					break;
					
				case 64:
					
					break;
					
				case 65:
					
					break;
					
				case 66:
					
					break;
					
				case 67:
					
					break;
					
				case 68:
					
					break;
					
				case 69:
					
					break;
					
				case 70:
					
					break;
					
				case 71:
					
					break;
					
				case 72:
					
					break;
					
				case 73:
					
					break;
					
				case 74:
					
					break;
					
				case 75:
					
					break;
					
				case 76:
					
					break;
					
				case 77:
					
					break;
					
				case 78:
					
					break;
					
				case 79:
					
					break;
					
				case 80:
					
					break;
					
				case 81:
					
					break;
					
				case 82:
					
					break;
					
				case 83:
					
					break;
					
				case 84:
					
					break;
					
				case 85:
					
					break;
					
				case 86:
					
					break;
					
				case 87:
					
					break;
					
				case 88:
					
					break;
					
				case 89:
					
					break;
					
				case 90:
					
					break;
					
				case 91:
					
					break;
					
				case 92:
					
					break;
					
				case 93:
					
					break;
					
				case 94:
					
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
				uint8_t fmmStatus = ((*iDDU)->readParallelStatus() >> 8) & 0x000F;
				statusDecoded = DDUDebugger::FMM(fmmStatus).begin()->second;
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
	for (std::vector<uint16_t>::const_reverse_iterator iValue = bigNum.rbegin(); iValue != bigNum.rend(); iValue+=2) {
		out << std::setw(4) << *iValue  << std::setw(4) << *(iValue-1) << " ";
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
