/*****************************************************************************\
* $Id: Commander.cc,v 1.7 2009/07/08 12:03:09 paste Exp $
\*****************************************************************************/
#include "emu/fed/Commander.h"

#include <iostream>
#include <iomanip>

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
		
	*out << cgicc::ol()
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
				.set("slot", slotNumber.str()) << std::endl;
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
				.set("slot", slotNumber.str()) << std::endl;
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
	
	*out << cgicc::li("Select commands")
		.set("class", "bold tier0") << std::endl;
	
	// The crate-specific commands
	*out << cgicc::div("Manage Firmware")
		.set("class", "tier0") << std::endl;
	*out << cgicc::div()
		.set("class", "tier1") << std::endl;
	*out << cgicc::button("Manage DDU Firmware")
		.set("id", "ddu_firmware_button") << std::endl;
	*out << cgicc::button("Manage DCC Firmware")
		.set("id", "dcc_firmware_button") << std::endl;
	*out << cgicc::div() << std::endl;
		
	// The DDU-specific commands
	*out << cgicc::div("Read/Write DDU Registers")
		.set("class", "tier0") << std::endl;
	// A series of buttons to make things easier on us
	*out << cgicc::div()
		.set("class", "tier1") << std::endl;
	*out << cgicc::button("Select All DDU Registers")
		.set("id", "all_ddu_registers") << std::endl;
	*out << cgicc::button("Select No DDU Registers")
		.set("id", "no_ddu_registers") << std::endl;
	*out << cgicc::div() << std::endl;
	
	std::vector<Register> DDURegisters;
	DDURegisters.push_back(Register("Flash Board ID", "ddu_flash_boardid", "bold"));
	DDURegisters.push_back(Register("RUI Number", "ddu_rui"));
	DDURegisters.push_back(Register("Flash RUI Number", "ddu_flash_rui", "bold"));
	DDURegisters.push_back(Register("KillFiber", "ddu_killfiber", "bold"));
	DDURegisters.push_back(Register("Flash KillFiber", "ddu_flash_killfiber", "bold"));
	DDURegisters.push_back(Register("GbE Prescale", "ddu_gbe_prescale", "bold"));
	
	DDURegisters.push_back(Register("DDUFPGA L1A Scaler", "ddu_ddufpga_l1a"));
	DDURegisters.push_back(Register("INFPGA0 L1A Scaler (fibers 0-3)", "ddu_infpga0_l1a0"));
	DDURegisters.push_back(Register("INFPGA0 L1A Scaler (fibers 4-7)", "ddu_infpga0_l1a1"));
	DDURegisters.push_back(Register("INFPGA1 L1A Scaler (fibers 8-1)", "ddu_infpga1_l1a0"));
	DDURegisters.push_back(Register("INFPGA1 L1A Scaler (fibers 12-14)", "ddu_infpga1_l1a1"));
	
	DDURegisters.push_back(Register("FMM Status", "ddu_fmm", "bold"));
	DDURegisters.push_back(Register("Per-Fiber Error Status", "ddu_csc_stat"));
	DDURegisters.push_back(Register("Per-Fiber Advanced Error Status", "ddu_advanced_error"));
	DDURegisters.push_back(Register("Per-Fiber FMM Busy", "ddu_fiber_busy"));
	DDURegisters.push_back(Register("Per-Fiber FMM Busy History", "ddu_fiber_busy_history"));
	DDURegisters.push_back(Register("Per-Fiber FMM Full Warning", "ddu_fiber_full"));
	DDURegisters.push_back(Register("Per-Fiber FMM Full Warning History", "ddu_fiber_full_history"));
	DDURegisters.push_back(Register("Per-Fiber FMM Lost Sync", "ddu_fiber_sync"));
	DDURegisters.push_back(Register("Per-Fiber FMM Error", "ddu_fiber_error"));
	
	DDURegisters.push_back(Register("DDUFPGA Status", "ddu_ddufpga_status"));
	DDURegisters.push_back(Register("DDUFPGA Diagnostic Trap", "ddu_ddufpga_diagnostic_trap"));
	DDURegisters.push_back(Register("Per-Fiber DDUFPGA FIFO-Full", "ddu_ddufpga_fifofull"));
	DDURegisters.push_back(Register("Per-Fiber CRC Error", "ddu_crc_error"));
	DDURegisters.push_back(Register("Per-Fiber Transmit Error", "ddu_xmit_error"));
	DDURegisters.push_back(Register("Per-Fiber DMB Error", "ddu_dmb_error"));
	DDURegisters.push_back(Register("Per-Fiber TMB Error", "ddu_tmb_error"));
	DDURegisters.push_back(Register("Per-Fiber ALCT Error", "ddu_alct_error"));
	DDURegisters.push_back(Register("Per-Fiber Lost-In-Event Error", "ddu_lie"));
	DDURegisters.push_back(Register("Per-Fiber Input Corruption History", "ddu_inc_history"));
	DDURegisters.push_back(Register("Per-Fiber DMB Live", "ddu_dmb_live"));
	DDURegisters.push_back(Register("Per-Fiber DMB Live at First Event", "ddu_dmb_live_first"));
	DDURegisters.push_back(Register("Per-Fiber Warning Status", "ddu_warning_monitor"));
	
	DDURegisters.push_back(Register("Temperature 0", "ddu_temperature_0"));
	DDURegisters.push_back(Register("Temperature 1", "ddu_temperature_1"));
	DDURegisters.push_back(Register("Temperature 2", "ddu_temperature_2"));
	DDURegisters.push_back(Register("Temperature 3", "ddu_temperature_3"));
	DDURegisters.push_back(Register("Voltage 1.5", "ddu_voltage_0"));
	DDURegisters.push_back(Register("Voltage 2.5 (1)", "ddu_voltage_1"));
	DDURegisters.push_back(Register("Voltage 2.5 (2)", "ddu_voltage_2"));
	DDURegisters.push_back(Register("Voltage 3.3", "ddu_voltage_3"));
	
	DDURegisters.push_back(Register("Fake L1", "ddu_fake_l1", "bold"));
	DDURegisters.push_back(Register("Flash GbE FIFO Thresholds", "ddu_flash_gbe_thresholds", "bold"));
	DDURegisters.push_back(Register("InRD Status", "ddu_inrd"));
	DDURegisters.push_back(Register("Error Register \"B\"", "ddu_errb"));
	DDURegisters.push_back(Register("Maximum L1A-to-Data Timeout", "ddu_max_timeout"));
	DDURegisters.push_back(Register("Bunch-Crossing Orbit", "ddu_bxorbit", "bold"));
	DDURegisters.push_back(Register("DDUFPGA Output Status", "ddu_ddufpga_output_stat"));
	DDURegisters.push_back(Register("DDUFPGA Input FIFO Status", "ddu_ddufpga_infifo_stat"));
	DDURegisters.push_back(Register("Serial Status", "ddu_serial_status"));
	DDURegisters.push_back(Register("Parallel Status", "ddu_parallel_status"));
	DDURegisters.push_back(Register("Switches", "ddu_switches"));
	
	DDURegisters.push_back(Register("Input Register 0", "ddu_input_reg_0", "bold"));
	DDURegisters.push_back(Register("Input Register 1", "ddu_input_reg_1"));
	DDURegisters.push_back(Register("Input Register 2", "ddu_input_reg_2"));
	DDURegisters.push_back(Register("Test Register 0", "ddu_test_reg_0"));
	DDURegisters.push_back(Register("Test Register 1", "ddu_test_reg_1"));
	DDURegisters.push_back(Register("Test Register 2", "ddu_test_reg_2"));
	DDURegisters.push_back(Register("Test Register 3", "ddu_test_reg_3"));
	DDURegisters.push_back(Register("Test Register 4", "ddu_test_reg_4"));

	DDURegisters.push_back(Register("INFPGA0 Status", "ddu_infpga0_status"));
	DDURegisters.push_back(Register("INFPGA0 Diagnostic Trap", "ddu_infpga0_diagnostic_trap"));
	DDURegisters.push_back(Register("INFPGA0 Fiber Status (fibers 0-7)", "ddu_infpga0_fiber_status"));
	DDURegisters.push_back(Register("INFPGA0 DMB Sync (fibers 0-7)", "ddu_infpga0_dmb_sync"));
	DDURegisters.push_back(Register("INFPGA0 FIFO Status (fibers 0-7)", "ddu_infpga0_fifo_status"));
	DDURegisters.push_back(Register("INFPGA0 FIFO Full (fibers 0-7)", "ddu_infpga0_fifo_full"));
	DDURegisters.push_back(Register("INFPGA0 Link Receive Error (fibers 0-7)", "ddu_infpga0_rxerror"));
	DDURegisters.push_back(Register("INFPGA0 Link Transmit Error (fibers 0-7)", "ddu_infpga0_txerror"));
	DDURegisters.push_back(Register("INFPGA0 Timeout Error (fibers 0-7)", "ddu_infpga0_timeout"));
	DDURegisters.push_back(Register("INFPGA0 Memory Active (fibers 0-7)", "ddu_infpga0_memory_active"));
	DDURegisters.push_back(Register("INFPGA0 Memory Free (fibers 0-7)", "ddu_infpga0_memory_free"));
	DDURegisters.push_back(Register("INFPGA0 Minimum Memory Free (fibers 0-7)", "ddu_infpga0_min_memory"));
	DDURegisters.push_back(Register("INFPGA0 Lost-In-Event (fibers 0-7)", "ddu_infpga0_lie"));
	DDURegisters.push_back(Register("INFPGA0 \"C\" Code (fibers 0-7)", "ddu_infpga0_ccode"));
	DDURegisters.push_back(Register("INFPGA0 DMB Warning (fibers 0-7)", "ddu_infpga0_dmbwarning"));
	DDURegisters.push_back(Register("INFPGA0 Fiber Diagnostic Register 0 (fibers 0-7)", "ddu_infpga0_diag0"));
	DDURegisters.push_back(Register("INFPGA0 Fiber Diagnostic Retister 1 (fibers 0-7)", "ddu_infpga0_diag1"));
	
	DDURegisters.push_back(Register("INFPGA1 Status", "ddu_infpga1_status"));
	DDURegisters.push_back(Register("INFPGA1 Diagnostic Trap", "ddu_infpga1_diagnostic_trap"));
	DDURegisters.push_back(Register("INFPGA1 Fiber Status (fibers 8-14)", "ddu_infpga1_fiber_status"));
	DDURegisters.push_back(Register("INFPGA1 DMB Sync (fibers 8-14)", "ddu_infpga1_dmb_sync"));
	DDURegisters.push_back(Register("INFPGA1 FIFO Status (fibers 8-14)", "ddu_infpga1_fifo_status"));
	DDURegisters.push_back(Register("INFPGA1 FIFO Full (fibers 8-14)", "ddu_infpga1_fifo_full"));
	DDURegisters.push_back(Register("INFPGA1 Link Receive Error (fibers 8-14)", "ddu_infpga1_rxerror"));
	DDURegisters.push_back(Register("INFPGA1 Link Transmit Error (fibers 8-14)", "ddu_infpga1_txerror"));
	DDURegisters.push_back(Register("INFPGA1 Timeout Error (fibers 8-14)", "ddu_infpga1_timeout"));
	DDURegisters.push_back(Register("INFPGA1 Memory Active (fibers 8-14)", "ddu_infpga1_memory_active"));
	DDURegisters.push_back(Register("INFPGA1 Memory Free (fibers 8-14)", "ddu_infpga1_memory_free"));
	DDURegisters.push_back(Register("INFPGA1 Minimum Memory Free (fibers 8-14)", "ddu_infpga1_min_memory"));
	DDURegisters.push_back(Register("INFPGA1 Lost-In-Event (fibers 8-14)", "ddu_infpga1_lie"));
	DDURegisters.push_back(Register("INFPGA1 \"C\" Code (fibers 8-14)", "ddu_infpga1_ccode"));
	DDURegisters.push_back(Register("INFPGA1 DMB Warning (fibers 8-14)", "ddu_infpga1_dmbwarning"));
	DDURegisters.push_back(Register("INFPGA1 Fiber Diagnostic Register 0 (fibers 8-14)", "ddu_infpga1_diag0"));
	DDURegisters.push_back(Register("INFPGA1 Fiber Diagnostic Retister 1 (fibers 8-14)", "ddu_infpga1_diag1"));
	
	// Print the table of registers
	*out << printRegisterTable(DDURegisters, "ddu_registers") << std::endl;
	
	*out << cgicc::li("Select output format")
		.set("class", "bold tier0") << std::endl;
	
	*out << cgicc::ol() << std::endl;
	
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
	
	*out << printConfigureOptions() << std::endl;;
	
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
		
		crateObject.push_back(JSONSpirit::Pair("number", (int) (*iCrate)->number()));
		
		// Make an array of DDUs
		JSONSpirit::Array dduArray;
		
		// Cycle through DDUs
		std::vector<DDU *> dduVector = (*iCrate)->getDDUs();
		for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
			
			// Get universal DDU information
			JSONSpirit::Object dduObject;
			
			// Slot number
			dduObject.push_back(JSONSpirit::Pair("slot", (int) (*iDDU)->slot()));
			
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
			dccObject.push_back(JSONSpirit::Pair("slot", (int) (*iDCC)->slot()));
			
			// FMM status and fifo status simultaneously
			uint16_t statusRegister = 0;
			uint16_t fifoStatus = 0;
			uint8_t slinkStatus = 0;
			std::string statusDecoded = "undefined";
			try {
				statusRegister = (*iDCC)->readStatusHigh();
				uint8_t fmmStatus = (statusRegister >> 12) & 0x000f;
				std::map<std::string, std::string> debugged = DCCDebugger::FMMStat(fmmStatus);
				statusDecoded = debugged.begin()->second;
				
				fifoStatus = (statusRegister & 0x0ff0) >> 4;
				
				slinkStatus = (statusRegister & 0xf);
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
