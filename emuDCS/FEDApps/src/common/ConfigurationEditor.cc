/*****************************************************************************\
* $Id: ConfigurationEditor.cc,v 1.12 2010/02/04 10:40:03 paste Exp $
\*****************************************************************************/
#include "emu/fed/ConfigurationEditor.h"

#include <sstream>
#include <fstream>
#include <cctype>

#include "xgi/Method.h"
#include "cgicc/HTMLClasses.h"
#include "xdaq2rc/RcmsStateNotifier.h"
#include "xdata/TimeVal.h"
#include "toolbox/TimeVal.h"
#include "toolbox/string.h"
#include "emu/base/Alarm.h"
#include "emu/fed/JSONSpiritWriter.h"
#include "emu/fed/XMLConfigurator.h"
#include "emu/fed/DBConfigurator.h"
#include "emu/fed/Crate.h"
#include "emu/fed/VMEController.h"
#include "emu/fed/DDU.h"
#include "emu/fed/DCC.h"
#include "emu/fed/Fiber.h"
#include "emu/fed/FIFO.h"
#include "emu/fed/SystemDBAgent.h"

XDAQ_INSTANTIATOR_IMPL(emu::fed::ConfigurationEditor)

emu::fed::ConfigurationEditor::ConfigurationEditor(xdaq::ApplicationStub *stub):
xdaq::WebApplication(stub),
emu::fed::Application(stub),
systemName_(""),
dbKey_(0)
{

	// Variables that are to be made available to other applications
	xdata::InfoSpace *infoSpace = getApplicationInfoSpace();
	infoSpace->fireItemAvailable("dbUsername", &dbUsername_);
	infoSpace->fireItemAvailable("dbPassword", &dbPassword_);

	// HyperDAQ pages
	xgi::bind(this, &emu::fed::ConfigurationEditor::webDefault, "Default");
	xgi::bind(this, &emu::fed::ConfigurationEditor::webUploadFile, "UploadFile");
	xgi::bind(this, &emu::fed::ConfigurationEditor::webGetDBKeys, "GetDBKeys");
	xgi::bind(this, &emu::fed::ConfigurationEditor::webLoadFromDB, "LoadFromDB");
	xgi::bind(this, &emu::fed::ConfigurationEditor::webCreateNew, "CreateNew");
	xgi::bind(this, &emu::fed::ConfigurationEditor::webWriteXML, "WriteXML");
	xgi::bind(this, &emu::fed::ConfigurationEditor::webSystem, "System");
	xgi::bind(this, &emu::fed::ConfigurationEditor::webCrate, "Crate");
	xgi::bind(this, &emu::fed::ConfigurationEditor::webController, "Controller");
	xgi::bind(this, &emu::fed::ConfigurationEditor::webDDU, "DDU");
	xgi::bind(this, &emu::fed::ConfigurationEditor::webFiber, "Fiber");
	xgi::bind(this, &emu::fed::ConfigurationEditor::webDCC, "DCC");
	xgi::bind(this, &emu::fed::ConfigurationEditor::webFIFO, "FIFO");
	xgi::bind(this, &emu::fed::ConfigurationEditor::webUploadToDB, "UploadToDB");

	timeStamp_ = time(NULL);
}


// HyperDAQ pages
void emu::fed::ConfigurationEditor::webDefault(xgi::Input *in, xgi::Output *out)
{

	std::vector<std::string> jsFileNames;
	jsFileNames.push_back("definitions.js");
	jsFileNames.push_back("configurationEditor.js");
	jsFileNames.push_back("common.js");
	*out << Header("FED Crate Configuration Editor", jsFileNames);

	*out << cgicc::div()
		.set("class", "titlebar default_width")
		.set("id", "FED_Configuration_Selection_titlebar") << std::endl;
	*out << cgicc::div("Configuration Selection")
		.set("class", "titletext") << std::endl;
	*out << cgicc::div() << std::endl;

	*out << cgicc::fieldset()
		.set("class", "dialog default_width")
		.set("id", "FED_Configuration_Selection_dialog") << std::endl;

	*out << cgicc::div("Upload XML file")
		.set("class", "category tier0") << std::endl;
	std::ostringstream formAction;
	formAction << getApplicationDescriptor()->getContextDescriptor()->getURL() << "/" << getApplicationDescriptor()->getURN() << "/UploadFile";
	*out << cgicc::form()
		.set("name", "xmlFileForm")
		.set("id", "xml_file_form")
		.set("style", "display: inline;")
		.set("method", "POST")
		.set("enctype", "multipart/form-data")
		.set("action", formAction.str())
		.set("target", "xml_file_frame") << std::endl;
	*out << cgicc::input()
		.set("class", "tier1")
		.set("name", "xmlFile")
		.set("id", "xml_file_upload")
		.set("type", "file") << std::endl;
	*out << cgicc::form() << std::endl;
	*out << cgicc::button("Upload")
		.set("name", "xmlFileButton")
		.set("id", "xml_file_button") << std::endl;
	*out << cgicc::iframe()
		.set("name", "xml_file_frame")
		.set("id", "xml_file_frame")
		.set("style", "display: none;") << std::endl;
	*out << cgicc::iframe() << std::endl;

	*out << cgicc::div("Select pre-existing configuration")
		.set("class", "category tier0") << std::endl;
	*out << cgicc::div()
		.set("class", "tier1") << std::endl;
	*out << cgicc::span("System:") << std::endl;
	*out << cgicc::select()
		.set("name", "configurationDescription")
		.set("id", "configuration_description") << std::endl;
	*out << cgicc::option("Loading...") << std::endl;
	*out << cgicc::select() << std::endl;
	*out << cgicc::span("Key:") << std::endl;
	*out << cgicc::select()
		.set("name", "configurationKey")
		.set("id", "configuration_key") << std::endl;
	*out << cgicc::option("Loading...") << std::endl;
	*out << cgicc::select() << std::endl;
	*out << cgicc::button("Load from database")
		.set("name", "loadButton")
		.set("id", "load_button") << std::endl;
	*out << cgicc::div() << std::endl;

	*out << cgicc::div("Create a configuration from scratch")
		.set("class", "category tier0") << std::endl;
	*out << cgicc::div()
		.set("class", "tier1") << std::endl;
	*out << cgicc::button("Create empty configuration")
		.set("name", "createButton")
		.set("id", "create_button") << std::endl;
	*out << cgicc::div() << std::endl;

	*out << cgicc::fieldset() << std::endl;

	//if (crateVector_.size() || systemName_ != "") {
	*out << cgicc::div()
		.set("class", "titlebar default_width")
		.set("id", "FED_Configuration_Editor_titlebar") << std::endl;
	*out << cgicc::div("Configuration Editor")
		.set("class", "titletext") << std::endl;
	*out << cgicc::div() << std::endl;

	*out << cgicc::fieldset()
		.set("class", "dialog default_width")
		.set("id", "FED_Configuration_Editor_dialog") << std::endl;

	*out << cgicc::div("Find a fiber")
		.set("class", "category tier0") << std::endl;
	*out << cgicc::div()
		.set("class", "tier1") << std::endl;
	*out << cgicc::span("Chamber/SP name: ") << std::endl;
	*out << cgicc::input()
		.set("type", "text")
		.set("id", "find_a_fiber")
		.set("class", "find_a_fiber") << std::endl;
	*out << cgicc::button("Find")
		.set("class", "find_button")
		.set("id", "find_button") << std::endl;
	*out << cgicc::span("No matching fiber found")
		.set("id", "find_a_fiber_error")
		.set("class", "red hidden") << std::endl;
	*out << cgicc::div() << std::endl;

	*out << cgicc::table()
		.set("class", "tier0") << std::endl;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::td()
		.set("class", "configure_cell")
		.set("id", "system_cell") << std::endl;
	*out << cgicc::td() << std::endl;
	*out << cgicc::td("&gt;")
		.set("class", "configure_cell crate_cell hidden") << std::endl;
	*out << cgicc::td()
		.set("class", "configure_cell crate_cell hidden")
		.set("id", "crate_cell") << std::endl;
	*out << cgicc::td() << std::endl;
	*out << cgicc::td("&gt;")
		.set("class", "configure_cell board_cell hidden") << std::endl;
	*out << cgicc::td()
		.set("class", "configure_cell board_cell hidden")
		.set("id", "board_cell") << std::endl;
	*out << cgicc::td() << std::endl;
	*out << cgicc::td("&gt;")
		.set("class", "configure_cell input_cell fiber_cell hidden") << std::endl;
	*out << cgicc::td()
		.set("class", "configure_cell input_cell fiber_cell hidden")
		.set("id", "fiber_cell") << std::endl;
	*out << cgicc::td() << std::endl;
	*out << cgicc::td("&gt;")
		.set("class", "configure_cell input_cell fifo_cell hidden") << std::endl;
	*out << cgicc::td()
		.set("class", "configure_cell input_cell fifo_cell hidden")
		.set("id", "fifo_cell") << std::endl;
	*out << cgicc::td() << std::endl;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::table() << std::endl;

	*out << cgicc::div()
		.set("id", "table_area") << std::endl;
	*out << cgicc::div() << std::endl;

	/*
	*out << cgicc::div("System")
		.set("class", "category tier0") << std::endl;
	*out << cgicc::table()
		.set("class", "tier1") << std::endl;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::td("Database key") << std::endl;
	*out << cgicc::td() << std::endl;
	*out << cgicc::input()
		.set("type", "text")
		.set("id", "input_database_key")
		.set("value", dbKey_.toString()) << std::endl;
	*out << cgicc::td() << std::endl;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::td("Name") << std::endl;
	*out << cgicc::td() << std::endl;
	*out << cgicc::input()
		.set("type", "text")
		.set("id", "input_database_name")
		.set("value", systemName_.toString()) << std::endl;
	*out << cgicc::td() << std::endl;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::td("Timestamp") << std::endl;
	*out << cgicc::td()
		.set("id", "timestamp") << std::endl;
	*out << toolbox::TimeVal(timeStamp_).toString(toolbox::TimeVal::gmt) << std::endl;
	*out << cgicc::td() << std::endl;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::table() << std::endl;

	*out << cgicc::div()
		.set("class", "tier1 bold") << std::endl;
	*out << cgicc::img()
		.set("id", "crates_open_close")
		.set("class", "crates_open_close pointer")
		.set("src", "/emu/emuDCS/FEDApps/images/plus.png") << std::endl;
	*out << cgicc::span("Crates")
		.set("class", "crates_open_close pointer") << std::endl;
	*out << cgicc::div() << std::endl;

	for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {

		std::ostringstream sCrateNumber;
		sCrateNumber << (*iCrate)->getNumber();
		std::string crateNumber = sCrateNumber.str();
		*out << cgicc::table()
			.set("id", "config_crate_table_" + crateNumber)
			.set("class", "tier2 config_crate_table hidden")
			.set("crates_hidden", "1")
			.set("crate", crateNumber) << std::endl;
		*out << cgicc::tr() << std::endl;
		*out << cgicc::td()
			.set("class", "bold config_crate_name crate_open_close pointer")
			.set("crate", crateNumber) << std::endl;
		*out << cgicc::img()
			.set("class", "crate_open_close pointer")
			.set("crate", crateNumber)
			.set("src", "/emu/emuDCS/FEDApps/images/plus.png") << std::endl;
		*out << "Crate " + crateNumber << std::endl;
		*out << cgicc::td() << std::endl;
		*out << cgicc::td() << std::endl;
		*out << cgicc::button()
			.set("class", "delete_crate")
			.set("crate", crateNumber);
		*out << cgicc::img()
			.set("class", "icon")
			.set("src", "/emu/emuDCS/FEDApps/images/list-remove.png");
		*out << "Delete crate" << std::endl;
		*out << cgicc::button() << std::endl;
		*out << cgicc::td() << std::endl;
		*out << cgicc::tr() << std::endl;
		*out << cgicc::tr()
			.set("id", "config_crate_number_row_" + crateNumber)
			.set("class", "hidden")
			.set("crates_hidden", "1")
			.set("crate_hidden", "1")
			.set("crate", crateNumber) << std::endl;
		*out << cgicc::td("Number") << std::endl;
		*out << cgicc::td() << std::endl;
		*out << cgicc::input()
			.set("type", "text")
			.set("class", "input_crate_number")
			.set("crate", crateNumber)
			.set("value", crateNumber) << std::endl;
		*out << cgicc::td() << std::endl;
		*out << cgicc::tr() << std::endl;
		*out << cgicc::table() << std::endl;

		*out << cgicc::table()
			.set("id", "config_controller_table_" + crateNumber)
			.set("class", "tier3 controller_table hidden")
			.set("crates_hidden", "1")
			.set("crate_hidden", "1")
			.set("crate", crateNumber) << std::endl;
		*out << cgicc::tr() << std::endl;
		*out << cgicc::td("VME Controller")
			.set("class", "bold controller_name")
			.set("crate", crateNumber) << std::endl;
		*out << cgicc::td() << std::endl;
		*out << cgicc::td() << std::endl;
		*out << cgicc::tr() << std::endl;
		*out << cgicc::tr() << std::endl;
		*out << cgicc::td("Device") << std::endl;
		*out << cgicc::td() << std::endl;
		std::ostringstream controllerDevice;
		controllerDevice << (*iCrate)->getController()->getDevice();
		*out << cgicc::input()
			.set("type", "text")
			.set("class", "input_controller_device")
			.set("crate", crateNumber)
			.set("value", controllerDevice.str()) << std::endl;
		*out << cgicc::td() << std::endl;
		*out << cgicc::tr() << std::endl;
		*out << cgicc::tr() << std::endl;
		*out << cgicc::td("Link") << std::endl;
		*out << cgicc::td() << std::endl;
		std::ostringstream controllerLink;
		controllerLink << (*iCrate)->getController()->getLink();
		*out << cgicc::input()
			.set("type", "text")
			.set("class", "input_controller_link")
			.set("crate", crateNumber)
			.set("value", controllerLink.str()) << std::endl;
		*out << cgicc::td() << std::endl;
		*out << cgicc::tr() << std::endl;
		*out << cgicc::table() << std::endl;

		*out << cgicc::div()
			.set("id", "config_ddus_" + crateNumber)
			.set("class", "bold tier3 hidden")
			.set("crates_hidden", "1")
			.set("crate_hidden", "1")
			.set("crate", crateNumber) << std::endl;
		*out << cgicc::img()
			.set("class", "ddus_open_close pointer")
			.set("crate", crateNumber)
			.set("src", "/emu/emuDCS/FEDApps/images/plus.png") << std::endl;
		*out << cgicc::span("DDUs")
			.set("class", "ddus_open_close pointer")
			.set("crate", crateNumber) << std::endl;
		*out << cgicc::div() << std::endl;

		std::vector<DDU *> ddus = (*iCrate)->getDDUs();
		for (std::vector<DDU *>::iterator iDDU = ddus.begin(); iDDU != ddus.end(); ++iDDU) {

			std::ostringstream sDDUNumber;
			sDDUNumber << (*iDDU)->getRUI();
			std::string dduNumber = sDDUNumber.str();

			*out << cgicc::table()
				.set("id", "config_ddu_" + crateNumber + "_" + dduNumber)
				.set("class", "tier4 ddu_table hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("ddus_hidden", "1")
				.set("crate", crateNumber)
				.set("rui", dduNumber) << std::endl;
			*out << cgicc::tr() << std::endl;
			*out << cgicc::td()
				.set("class", "bold ddu_name ddu_open_close pointer")
				.set("rui", dduNumber)
				.set("crate", crateNumber) << std::endl;
			*out << cgicc::img()
				.set("class", "ddu_open_close pointer")
				.set("rui", dduNumber)
				.set("crate", crateNumber)
				.set("src", "/emu/emuDCS/FEDApps/images/plus.png") << std::endl;
			*out << "DDU " + dduNumber << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::button()
				.set("class", "delete_ddu")
				.set("crate", crateNumber)
				.set("rui", dduNumber);
			*out << cgicc::img()
				.set("class", "icon")
				.set("src", "/emu/emuDCS/FEDApps/images/list-remove.png");
			*out << "Delete DDU" << std::endl;
			*out << cgicc::button() << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;
			*out << cgicc::tr()
				.set("id", "config_ddu_row_slot_" + crateNumber + "_" + dduNumber)
				.set("class", "hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("ddus_hidden", "1")
				.set("ddu_hidden", "1")
				.set("crate", crateNumber)
				.set("rui", dduNumber) << std::endl;
			*out << cgicc::td("Slot") << std::endl;
			*out << cgicc::td() << std::endl;
			std::ostringstream slotNumber;
			slotNumber << (*iDDU)->getSlot();
			*out << cgicc::input()
				.set("type", "text")
				.set("class", "input_ddu_slot")
				.set("crate", crateNumber)
				.set("rui", dduNumber)
				.set("value", slotNumber.str()) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;
			*out << cgicc::tr()
				.set("id", "config_ddu_row_rui_" + crateNumber + "_" + dduNumber)
				.set("class", "hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("ddus_hidden", "1")
				.set("ddu_hidden", "1")
				.set("crate", crateNumber)
				.set("rui", dduNumber) << std::endl;
			*out << cgicc::td("RUI") << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::input()
				.set("type", "text")
				.set("class", "input_ddu_rui")
				.set("crate", crateNumber)
				.set("rui", dduNumber)
				.set("value", dduNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;
			*out << cgicc::tr()
				.set("id", "config_ddu_row_fmmid_" + crateNumber + "_" + dduNumber)
				.set("class", "hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("ddus_hidden", "1")
				.set("ddu_hidden", "1")
				.set("crate", crateNumber)
				.set("rui", dduNumber) << std::endl;
			*out << cgicc::td("FMM ID") << std::endl;
			*out << cgicc::td() << std::endl;
			std::ostringstream fmmID;
			fmmID << (*iDDU)->getFMMID();
			*out << cgicc::input()
				.set("type", "text")
				.set("class", "input_ddu_fmm_id")
				.set("crate", crateNumber)
				.set("rui", dduNumber)
				.set("value", fmmID.str()) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;
			*out << cgicc::tr()
				.set("id", "config_ddu_row_fec_" + crateNumber + "_" + dduNumber)
				.set("class", "hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("ddus_hidden", "1")
				.set("ddu_hidden", "1")
				.set("crate", crateNumber)
				.set("rui", dduNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::label("Enable forced error checks")
				.set("for", "input_ddu_force_checks_" + crateNumber + "_" + dduNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::td() << std::endl;
			cgicc::input fecInput;
			fecInput.set("type", "checkbox")
				.set("id", "input_ddu_force_checks_" + crateNumber + "_" + dduNumber)
				.set("class", "input_ddu_force_checks")
				.set("crate", crateNumber)
				.set("rui", dduNumber);
			if ((*iDDU)->getKillFiber() & (1 << 15)) fecInput.set("checked", "checked");
			*out << fecInput << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;
			*out << cgicc::tr()
				.set("id", "config_ddu_row_alct_" + crateNumber + "_" + dduNumber)
				.set("class", "hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("ddus_hidden", "1")
				.set("ddu_hidden", "1")
				.set("crate", crateNumber)
				.set("rui", dduNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::label("Enable forced ALCT checks")
				.set("for", "input_ddu_force_alct_" + crateNumber + "_" + dduNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::td() << std::endl;
			cgicc::input alctInput;
			alctInput.set("type", "checkbox")
				.set("id", "input_ddu_force_alct_" + crateNumber + "_" + dduNumber)
				.set("class", "input_ddu_force_alct")
				.set("crate", crateNumber)
				.set("rui", dduNumber);
			if ((*iDDU)->getKillFiber() & (1 << 16)) alctInput.set("checked", "checked");
			*out << alctInput << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;
			*out << cgicc::tr()
				.set("id", "config_ddu_row_tmb_" + crateNumber + "_" + dduNumber)
				.set("class", "hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("ddus_hidden", "1")
				.set("ddu_hidden", "1")
				.set("crate", crateNumber)
				.set("rui", dduNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::label("Enable forced TMB checks")
				.set("for", "input_ddu_force_tmb_" + crateNumber + "_" + dduNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::td() << std::endl;
			cgicc::input tmbInput;
			tmbInput.set("type", "checkbox")
				.set("id", "input_ddu_force_tmb_" + crateNumber + "_" + dduNumber)
				.set("class", "input_ddu_force_tmb")
				.set("crate", crateNumber)
				.set("rui", dduNumber);
			if ((*iDDU)->getKillFiber() & (1 << 17)) tmbInput.set("checked", "checked");
			*out << tmbInput << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;
			*out << cgicc::tr()
				.set("id", "config_ddu_row_cfeb_" + crateNumber + "_" + dduNumber)
				.set("class", "hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("ddus_hidden", "1")
				.set("ddu_hidden", "1")
				.set("crate", crateNumber)
				.set("rui", dduNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::label("Enable forced CFEB checks")
				.set("for", "input_ddu_force_cfeb_" + crateNumber + "_" + dduNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::td() << std::endl;
			cgicc::input cfebInput;
			cfebInput.set("type", "checkbox")
				.set("id", "input_ddu_force_cfeb_" + crateNumber + "_" + dduNumber)
				.set("class", "input_ddu_force_cfeb")
				.set("crate", crateNumber)
				.set("rui", dduNumber);
			if ((*iDDU)->getKillFiber() & (1 << 18)) cfebInput.set("checked", "checked");
			*out << cfebInput << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;
			*out << cgicc::tr()
				.set("id", "config_ddu_row_dmb_" + crateNumber + "_" + dduNumber)
				.set("class", "hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("ddus_hidden", "1")
				.set("ddu_hidden", "1")
				.set("crate", crateNumber)
				.set("rui", dduNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::label("Enable forced normal DMB")
				.set("for", "input_ddu_force_dmb_" + crateNumber + "_" + dduNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::td() << std::endl;
			cgicc::input dmbInput;
			dmbInput.set("type", "checkbox")
				.set("id", "input_ddu_force_dmb_" + crateNumber + "_" + dduNumber)
				.set("class", "input_ddu_force_dmb")
				.set("crate", crateNumber)
				.set("rui", dduNumber);
			if ((*iDDU)->getKillFiber() & (1 << 19)) dmbInput.set("checked", "checked");
			*out << dmbInput << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;
			*out << cgicc::tr()
				.set("id", "config_ddu_row_gbe_" + crateNumber + "_" + dduNumber)
				.set("class", "hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("ddus_hidden", "1")
				.set("ddu_hidden", "1")
				.set("crate", crateNumber)
				.set("rui", dduNumber) << std::endl;
			*out << cgicc::td("Gigabit ethernet prescale") << std::endl;
			*out << cgicc::td() << std::endl;
			std::ostringstream gbePrescale;
			gbePrescale << (*iDDU)->getGbEPrescale();
			*out << cgicc::input()
				.set("type", "text")
				.set("class", "input_ddu_gbe_prescale")
				.set("crate", crateNumber)
				.set("rui", dduNumber)
				.set("value", gbePrescale.str()) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;
			*out << cgicc::tr()
				.set("id", "config_ddu_row_ccb_" + crateNumber + "_" + dduNumber)
				.set("class", "hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("ddus_hidden", "1")
				.set("ddu_hidden", "1")
				.set("crate", crateNumber)
				.set("rui", dduNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::label("Invert CCB command signals")
				.set("for", "input_ddu_invert_ccb_" + crateNumber + "_" + dduNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::td() << std::endl;
			cgicc::input ccbInput;
			ccbInput.set("type", "checkbox")
				.set("id", "input_ddu_invert_ccb_" + crateNumber + "_" + dduNumber)
				.set("class", "input_ddu_invert_ccb")
				.set("crate", crateNumber)
				.set("rui", dduNumber);
			if ((*iDDU)->getRUI() == 0xc0) ccbInput.set("checked", "checked");
			*out << ccbInput << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;
			*out << cgicc::table() << std::endl;

			*out << cgicc::div()
				.set("id", "config_fibers_" + crateNumber + "_" + dduNumber)
				.set("class", "bold tier4 hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("ddus_hidden", "1")
				.set("ddu_hidden", "1")
				.set("crate", crateNumber)
				.set("rui", dduNumber) << std::endl;
			*out << cgicc::img()
				.set("class", "fibers_open_close pointer")
				.set("crate", crateNumber)
				.set("rui", dduNumber)
				.set("src", "/emu/emuDCS/FEDApps/images/plus.png") << std::endl;
			*out << cgicc::span("Fibers")
				.set("class", "fibers_open_close pointer")
				.set("crate", crateNumber)
				.set("rui", dduNumber) << std::endl;
			*out << cgicc::div() << std::endl;

			std::vector<Fiber *> fibers = (*iDDU)->getFibers();
			for (std::vector<Fiber *>::iterator iFiber = fibers.begin(); iFiber != fibers.end(); ++iFiber) {

				std::ostringstream sFiberNumber;
				sFiberNumber << (*iFiber)->getFiberNumber();
				std::string fiberNumber = sFiberNumber.str();

				*out << cgicc::table()
					.set("id", "config_fiber_table_" + crateNumber + "_" + dduNumber + "_" + fiberNumber)
					.set("class", "tier5 fiber_table hidden")
					.set("crates_hidden", "1")
					.set("crate_hidden", "1")
					.set("ddus_hidden", "1")
					.set("ddu_hidden", "1")
					.set("fibers_hidden", "1")
					.set("crate", crateNumber)
					.set("rui", dduNumber)
					.set("fiber", fiberNumber) << std::endl;
				*out << cgicc::tr() << std::endl;
				*out << cgicc::td("Fiber " + fiberNumber)
					.set("class", "bold fiber_name")
					.set("crate", crateNumber)
					.set("rui", dduNumber)
					.set("fiber", fiberNumber) << std::endl;
				*out << cgicc::td() << std::endl;
				*out << cgicc::button()
					.set("class", "delete_fiber")
					.set("crate", crateNumber)
					.set("rui", dduNumber)
					.set("fiber", fiberNumber);
				*out << cgicc::img()
					.set("class", "icon")
					.set("src", "/emu/emuDCS/FEDApps/images/list-remove.png");
				*out << "Delete Fiber" << std::endl;
				*out << cgicc::button() << std::endl;
				*out << cgicc::td() << std::endl;
				*out << cgicc::tr() << std::endl;
				*out << cgicc::tr()
					.set("id", "config_fiber_row_number_" + crateNumber + "_" + dduNumber + "_" + fiberNumber)
					.set("class", "hidden")
					.set("crates_hidden", "1")
					.set("crate_hidden", "1")
					.set("ddus_hidden", "1")
					.set("ddu_hidden", "1")
					.set("fibers_hidden", "1")
					.set("crate", crateNumber)
					.set("rui", dduNumber)
					.set("fiber", fiberNumber) << std::endl;
				*out << cgicc::td("Number") << std::endl;
				*out << cgicc::td() << std::endl;
				*out << cgicc::input()
					.set("type", "text")
					.set("class", "input_fiber_number")
					.set("crate", crateNumber)
					.set("rui", dduNumber)
					.set("fiber", fiberNumber)
					.set("value", fiberNumber) << std::endl;
				*out << cgicc::td() << std::endl;
				*out << cgicc::tr() << std::endl;
				*out << cgicc::tr()
					.set("id", "config_fiber_row_name_" + crateNumber + "_" + dduNumber + "_" + fiberNumber)
					.set("class", "hidden")
					.set("crates_hidden", "1")
					.set("crate_hidden", "1")
					.set("ddus_hidden", "1")
					.set("ddu_hidden", "1")
					.set("fibers_hidden", "1")
					.set("rui", dduNumber)
					.set("crate", crateNumber)
					.set("fiber", fiberNumber) << std::endl;
				*out << cgicc::td("Name") << std::endl;
				*out << cgicc::td() << std::endl;
				*out << cgicc::input()
					.set("type", "text")
					.set("class", "input_fiber_name")
					.set("crate", crateNumber)
					.set("rui", dduNumber)
					.set("fiber", fiberNumber)
					.set("value", (*iFiber)->getName()) << std::endl;
				*out << cgicc::td() << std::endl;
				*out << cgicc::tr() << std::endl;
				*out << cgicc::tr()
					.set("id", "config_fiber_row_killed_" + crateNumber + "_" + dduNumber + "_" + fiberNumber)
					.set("class", "hidden")
					.set("crates_hidden", "1")
					.set("crate_hidden", "1")
					.set("ddus_hidden", "1")
					.set("ddu_hidden", "1")
					.set("fibers_hidden", "1")
					.set("rui", dduNumber)
					.set("crate", crateNumber)
					.set("fiber", fiberNumber) << std::endl;
				*out << cgicc::td() << std::endl;
				*out << cgicc::label("Killed")
				.set("for", "input_fiber_killed_" + crateNumber + "_" + dduNumber + "_" + fiberNumber) << std::endl;
				*out << cgicc::td() << std::endl;
				*out << cgicc::td() << std::endl;
				cgicc::input killInput;
				killInput.set("type", "checkbox")
					.set("id", "input_fiber_killed_" + crateNumber + "_" + dduNumber + "_" + fiberNumber)
					.set("class", "input_fiber_killed")
					.set("rui", dduNumber)
					.set("crate", crateNumber)
					.set("fiber", fiberNumber);
				if ((*iFiber)->isKilled()) killInput.set("checked", "checked");
				*out << killInput << std::endl;
				*out << cgicc::td() << std::endl;
				*out << cgicc::tr() << std::endl;
				*out << cgicc::table() << std::endl;

			}

			*out << cgicc::div()
				.set("id", "config_fiber_add_" + crateNumber + "_" + dduNumber)
				.set("class", "tier5 hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("ddus_hidden", "1")
				.set("ddu_hidden", "1")
				.set("fibers_hidden", "1")
				.set("crate", crateNumber)
				.set("rui", dduNumber) << std::endl;
			*out << cgicc::button()
				.set("class", "add_fiber")
				.set("crate", crateNumber)
				.set("rui", dduNumber);
			*out << cgicc::img()
				.set("class", "icon")
				.set("src", "/emu/emuDCS/FEDApps/images/list-add.png");
			*out << "Add Fiber" << std::endl;
			*out << cgicc::button() << std::endl;
			*out << cgicc::div() << std::endl;

		}

		*out << cgicc::div()
			.set("id", "config_ddu_add_" + crateNumber)
			.set("class", "tier4 hidden")
			.set("crates_hidden", "1")
			.set("crate_hidden", "1")
			.set("ddus_hidden", "1")
			.set("crate", crateNumber) << std::endl;
		*out << cgicc::button()
			.set("class", "add_ddu")
			.set("crate", crateNumber);
		*out << cgicc::img()
			.set("class", "icon")
			.set("src", "/emu/emuDCS/FEDApps/images/list-add.png");
		*out << "Add DDU" << std::endl;
		*out << cgicc::button() << std::endl;
		*out << cgicc::div() << std::endl;

		*out << cgicc::div()
			.set("id", "config_dccs_" + crateNumber)
			.set("class", "bold tier3 hidden")
			.set("crates_hidden", "1")
			.set("crate_hidden", "1")
			.set("crate", crateNumber) << std::endl;
		*out << cgicc::img()
			.set("class", "dccs_open_close pointer")
			.set("crate", crateNumber)
			.set("src", "/emu/emuDCS/FEDApps/images/plus.png") << std::endl;
		*out << cgicc::span("DCCs")
			.set("class", "dccs_open_close pointer")
			.set("crate", crateNumber) << std::endl;
		*out << cgicc::div() << std::endl;

		std::vector<DCC *> dccs = (*iCrate)->getDCCs();
		for (std::vector<DCC *>::iterator iDCC = dccs.begin(); iDCC != dccs.end(); ++iDCC) {

			std::ostringstream sDCCNumber;
			sDCCNumber << (*iDCC)->getFMMID();
			std::string dccNumber = sDCCNumber.str();

			*out << cgicc::table()
				.set("id", "config_dcc_table_" + crateNumber + "_" + dccNumber)
				.set("class", "tier4 dcc_table hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("dccs_hidden", "1")
				.set("crate", crateNumber)
				.set("fmmid", dccNumber) << std::endl;
			*out << cgicc::tr() << std::endl;
			*out << cgicc::td()
				.set("class", "bold dcc_name dcc_open_close pointer")
				.set("fmmid", dccNumber)
				.set("crate", crateNumber) << std::endl;
			*out << cgicc::img()
				.set("class", "dcc_open_close pointer")
				.set("fmmid", dccNumber)
				.set("crate", crateNumber)
				.set("src", "/emu/emuDCS/FEDApps/images/plus.png") << std::endl;
			*out << "DCC " + dccNumber << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::button()
				.set("class", "delete_dcc")
				.set("crate", crateNumber)
				.set("fmmid", dccNumber);
			*out << cgicc::img()
				.set("class", "icon")
				.set("src", "/emu/emuDCS/FEDApps/images/list-remove.png");
			*out << "Delete DCC" << std::endl;
			*out << cgicc::button() << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;
			*out << cgicc::tr()
				.set("id", "config_dcc_row_slot_" + crateNumber + "_" + dccNumber)
				.set("class", "hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("dccs_hidden", "1")
				.set("dcc_hidden", "1")
				.set("crate", crateNumber)
				.set("fmmid", dccNumber) << std::endl;
			*out << cgicc::td("Slot") << std::endl;
			*out << cgicc::td() << std::endl;
			std::ostringstream slotNumber;
			slotNumber << (*iDCC)->getSlot();
			*out << cgicc::input()
				.set("type", "text")
				.set("class", "input_dcc_slot")
				.set("fmmid", dccNumber)
				.set("crate", crateNumber)
				.set("value", slotNumber.str()) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;
			*out << cgicc::tr()
				.set("id", "config_dcc_row_fmmid_" + crateNumber + "_" + dccNumber)
				.set("class", "hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("dccs_hidden", "1")
				.set("dcc_hidden", "1")
				.set("crate", crateNumber)
				.set("fmmid", dccNumber) << std::endl;
			*out << cgicc::td("FMM ID") << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::input()
				.set("type", "text")
				.set("class", "input_dcc_fmm_id")
				.set("fmmid", dccNumber)
				.set("crate", crateNumber)
				.set("value", dccNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;
			*out << cgicc::tr()
				.set("id", "config_dcc_row_slink1_" + crateNumber + "_" + dccNumber)
				.set("class", "hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("dccs_hidden", "1")
				.set("dcc_hidden", "1")
				.set("crate", crateNumber)
				.set("fmmid", dccNumber) << std::endl;
			*out << cgicc::td("SLink 1 ID") << std::endl;
			*out << cgicc::td() << std::endl;
			std::ostringstream slink1ID;
			slink1ID << (*iDCC)->getSLinkID(1);
			*out << cgicc::input()
					.set("type", "text")
					.set("class", "input_dcc_slink1")
					.set("fmmid", dccNumber)
					.set("crate", crateNumber)
					.set("value", slink1ID.str()) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;
			*out << cgicc::tr()
				.set("id", "config_dcc_row_slink2" + crateNumber + "_" + dccNumber)
				.set("class", "hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("dccs_hidden", "1")
				.set("dcc_hidden", "1")
				.set("crate", crateNumber)
				.set("fmmid", dccNumber) << std::endl;
			*out << cgicc::td("SLink 2 ID") << std::endl;
			*out << cgicc::td() << std::endl;
			std::ostringstream slink2ID;
			slink2ID << (*iDCC)->getSLinkID(2);
			*out << cgicc::input()
				.set("type", "text")
				.set("class", "input_dcc_slink2")
				.set("fmmid", dccNumber)
				.set("crate", crateNumber)
				.set("value", slink2ID.str()) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;
			*out << cgicc::tr()
				.set("id", "config_dcc_row_sw_" + crateNumber + "_" + dccNumber)
				.set("class", "hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("dccs_hidden", "1")
				.set("dcc_hidden", "1")
				.set("crate", crateNumber)
				.set("fmmid", dccNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::label("Enable software switch")
				.set("for", "input_dcc_sw_switch_" + crateNumber + "_" + dccNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::td() << std::endl;
			cgicc::input swInput;
			swInput.set("type", "checkbox")
				.set("id", "input_dcc_sw_switch_" + crateNumber + "_" + dccNumber)
				.set("class", "input_dcc_sw_switch")
				.set("crate", crateNumber)
				.set("fmmid", dccNumber);
			if ((*iDCC)->getSoftwareSwitch() & 0x200) swInput.set("checked", "checked");
			*out << swInput << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;
			*out << cgicc::tr()
				.set("id", "config_dcc_row_ttc_" + crateNumber + "_" + dccNumber)
				.set("class", "hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("dccs_hidden", "1")
				.set("dcc_hidden", "1")
				.set("crate", crateNumber)
				.set("fmmid", dccNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::label("Ignore TTCRx-not-ready signal")
				.set("for", "input_dcc_ignore_ttc_" + crateNumber + "_" + dccNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::td() << std::endl;
			cgicc::input ttcInput;
			ttcInput.set("type", "checkbox")
				.set("id", "input_dcc_ignore_ttc_" + crateNumber + "_" + dccNumber)
				.set("class", "input_dcc_ignore_ttc")
				.set("crate", crateNumber)
				.set("fmmid", dccNumber);
			if ((*iDCC)->getSoftwareSwitch() & 0x1000) ttcInput.set("checked", "checked");
			*out << ttcInput << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;
			*out << cgicc::tr()
				.set("id", "config_dcc_row_slinkbp_" + crateNumber + "_" + dccNumber)
				.set("class", "hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("dccs_hidden", "1")
				.set("dcc_hidden", "1")
				.set("crate", crateNumber)
				.set("fmmid", dccNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::label("Ignore SLink backpressure")
				.set("for", "input_dcc_ignore_backpressure_" + crateNumber + "_" + dccNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::td() << std::endl;
			cgicc::input bpInput;
			bpInput.set("type", "checkbox")
				.set("id", "input_dcc_ignore_backpressure_" + crateNumber + "_" + dccNumber)
				.set("class", "input_dcc_ignore_backpressure")
				.set("crate", crateNumber)
				.set("fmmid", dccNumber);
			if ((*iDCC)->getSoftwareSwitch() & 0x2000) bpInput.set("checked", "checked");
			*out << bpInput << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;
			*out << cgicc::tr()
				.set("id", "config_dcc_row_slinknp_" + crateNumber + "_" + dccNumber)
				.set("class", "hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("dccs_hidden", "1")
				.set("dcc_hidden", "1")
				.set("crate", crateNumber)
				.set("fmmid", dccNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::label("Ignore SLink-not-present")
				.set("for", "input_dcc_ignore_slink_" + crateNumber + "_" + dccNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::td() << std::endl;
			cgicc::input slinkInput;
			slinkInput.set("type", "checkbox")
				.set("id", "input_dcc_ignore_slink_" + crateNumber + "_" + dccNumber)
				.set("class", "input_dcc_ignore_slink")
				.set("crate", crateNumber)
				.set("fmmid", dccNumber);
			if ((*iDCC)->getSoftwareSwitch() & 0x4000) slinkInput.set("checked", "checked");
			*out << slinkInput << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;
			*out << cgicc::tr()
				.set("id", "config_dcc_row_sw4_" + crateNumber + "_" + dccNumber)
				.set("class", "hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("dccs_hidden", "1")
				.set("dcc_hidden", "1")
				.set("crate", crateNumber)
				.set("fmmid", dccNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::label("Switch bit 4")
				.set("for", "input_dcc_sw4_" + crateNumber + "_" + dccNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::td() << std::endl;
			cgicc::input sw4Input;
			sw4Input.set("type", "checkbox")
				.set("id", "input_dcc_sw4_" + crateNumber + "_" + dccNumber)
				.set("class", "input_dcc_sw4")
				.set("crate", crateNumber)
				.set("fmmid", dccNumber);
			if ((*iDCC)->getSoftwareSwitch() & 0x10) sw4Input.set("checked", "checked");
			*out << sw4Input << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;
			*out << cgicc::tr()
				.set("id", "config_dcc_row_sw5_" + crateNumber + "_" + dccNumber)
				.set("class", "hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("dccs_hidden", "1")
				.set("dcc_hidden", "1")
				.set("crate", crateNumber)
				.set("fmmid", dccNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::label("Switch bit 5")
				.set("for", "input_dcc_sw5_" + crateNumber + "_" + dccNumber) << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::td() << std::endl;
			cgicc::input sw5Input;
			sw5Input.set("type", "checkbox")
				.set("id", "input_dcc_sw5_" + crateNumber + "_" + dccNumber)
				.set("class", "input_dcc_sw5")
				.set("crate", crateNumber)
				.set("fmmid", dccNumber);
			if ((*iDCC)->getSoftwareSwitch() & 0x20) sw5Input.set("checked", "checked");
			*out << sw5Input << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;
			*out << cgicc::table() << std::endl;

			*out << cgicc::div()
				.set("id", "config_fifos_" + crateNumber + "_" + dccNumber)
				.set("class", "bold tier4 hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("dccs_hidden", "1")
				.set("dcc_hidden", "1")
				.set("crate", crateNumber)
				.set("fmmid", dccNumber) << std::endl;
			*out << cgicc::img()
				.set("class", "fifos_open_close pointer")
				.set("fmmid", dccNumber)
				.set("crate", crateNumber)
				.set("src", "/emu/emuDCS/FEDApps/images/plus.png") << std::endl;
			*out << cgicc::span("FIFOs")
				.set("class", "fifos_open_close pointer")
				.set("crate", crateNumber)
				.set("fmmid", dccNumber) << std::endl;
			*out << cgicc::div() << std::endl;

			std::vector<FIFO *> fifos = (*iDCC)->getFIFOs();
			for (std::vector<FIFO *>::iterator iFIFO = fifos.begin(); iFIFO != fifos.end(); ++iFIFO) {

				std::ostringstream sFIFONumber;
				sFIFONumber << (*iFIFO)->getNumber();
				std::string fifoNumber = sFIFONumber.str();

				*out << cgicc::table()
					.set("id", "config_fifo_table_" + crateNumber + "_" + dccNumber + "_" + fifoNumber)
					.set("class", "tier5 fifo_table hidden")
					.set("crates_hidden", "1")
					.set("crate_hidden", "1")
					.set("dccs_hidden", "1")
					.set("fifos_hidden", "1")
					.set("crate", crateNumber)
					.set("fmmid", dccNumber)
					.set("fifo", fifoNumber) << std::endl;
				*out << cgicc::tr() << std::endl;
				*out << cgicc::td("FIFO " + fifoNumber)
					.set("class", "bold fifo_name")
					.set("crate", crateNumber)
					.set("fmmid", dccNumber)
					.set("fifo", fifoNumber) << std::endl;
				*out << cgicc::td() << std::endl;
				*out << cgicc::button()
					.set("class", "delete_fifo")
					.set("crate", crateNumber)
					.set("fmmid", dccNumber)
					.set("fifo", fifoNumber);
				*out << cgicc::img()
					.set("class", "icon")
					.set("src", "/emu/emuDCS/FEDApps/images/list-remove.png");
				*out << "Delete FIFO" << std::endl;
				*out << cgicc::button() << std::endl;
				*out << cgicc::td() << std::endl;
				*out << cgicc::tr() << std::endl;
				*out << cgicc::tr()
					.set("id", "config_fifo_row_number_" + crateNumber + "_" + dccNumber + "_" + fifoNumber)
					.set("class", "hidden")
					.set("crates_hidden", "1")
					.set("crate_hidden", "1")
					.set("dccs_hidden", "1")
					.set("fifos_hidden", "1")
					.set("fmmid", dccNumber)
					.set("crate", crateNumber)
					.set("fifo", fifoNumber) << std::endl;
				*out << cgicc::td("Number") << std::endl;
				*out << cgicc::td() << std::endl;
				*out << cgicc::input()
					.set("type", "text")
					.set("class", "input_fifo_number")
					.set("crate", crateNumber)
					.set("fmmid", dccNumber)
					.set("fifo", fifoNumber)
					.set("value", fifoNumber) << std::endl;
				*out << cgicc::td() << std::endl;
				*out << cgicc::tr() << std::endl;
				*out << cgicc::tr()
					.set("id", "config_fifo_row_rui_" + crateNumber + "_" + dccNumber + "_" + fifoNumber)
					.set("class", "hidden")
					.set("crates_hidden", "1")
					.set("crate_hidden", "1")
					.set("dccs_hidden", "1")
					.set("fifos_hidden", "1")
					.set("fmmid", dccNumber)
					.set("crate", crateNumber)
					.set("fifo", fifoNumber) << std::endl;
				*out << cgicc::td("RUI") << std::endl;
				*out << cgicc::td() << std::endl;
				std::ostringstream rui;
				rui << (*iFIFO)->getRUI();
				*out << cgicc::input()
					.set("type", "text")
					.set("class", "input_fifo_rui")
					.set("crate", crateNumber)
					.set("fmmid", dccNumber)
					.set("fifo", fifoNumber)
					.set("value", rui.str()) << std::endl;
				*out << cgicc::td() << std::endl;
				*out << cgicc::tr() << std::endl;
				*out << cgicc::tr()
					.set("id", "config_fifo_row_inuse_" + crateNumber + "_" + dccNumber + "_" + fifoNumber)
					.set("class", "hidden")
					.set("crates_hidden", "1")
					.set("crate_hidden", "1")
					.set("dccs_hidden", "1")
					.set("fifos_hidden", "1")
					.set("fmmid", dccNumber)
					.set("crate", crateNumber)
					.set("fifo", fifoNumber) << std::endl;
				*out << cgicc::td() << std::endl;
				*out << cgicc::label("In use")
					.set("for", "input_fifo_used_" + crateNumber + "_" + dccNumber + "_" + fifoNumber) << std::endl;
				*out << cgicc::td() << std::endl;
				*out << cgicc::td() << std::endl;
				cgicc::input useInput;
				useInput.set("type", "checkbox")
					.set("id", "input_fifo_used_" + crateNumber + "_" + dccNumber + "_" + fifoNumber)
					.set("class", "input_fifo_used")
					.set("crate", crateNumber)
					.set("fmmid", dccNumber)
					.set("fifo", fifoNumber);
				if ((*iFIFO)->isUsed()) useInput.set("checked", "checked");
				*out << useInput << std::endl;
				*out << cgicc::td() << std::endl;
				*out << cgicc::tr() << std::endl;
				*out << cgicc::table() << std::endl;

			}

			*out << cgicc::div()
				.set("id", "config_fifo_add_" + crateNumber + "_" + dccNumber)
				.set("class", "tier5 hidden")
				.set("crates_hidden", "1")
				.set("crate_hidden", "1")
				.set("dccs_hidden", "1")
				.set("fifos_hidden", "1")
				.set("crate", crateNumber)
				.set("fmmid", dccNumber) << std::endl;
			*out << cgicc::button()
				.set("class", "add_fifo")
				.set("crate", crateNumber)
				.set("fmmid", dccNumber);
			*out << cgicc::img()
				.set("class", "icon")
				.set("src", "/emu/emuDCS/FEDApps/images/list-add.png");
			*out << "Add FIFO" << std::endl;
			*out << cgicc::button() << std::endl;
			*out << cgicc::div() << std::endl;
		}

		*out << cgicc::div()
			.set("id", "config_dcc_add_" + crateNumber)
			.set("class", "tier4 hidden")
			.set("crates_hidden", "1")
			.set("crate_hidden", "1")
			.set("dccs_hidden", "1")
			.set("crate", crateNumber) << std::endl;
		*out << cgicc::button()
			.set("class", "add_dcc")
			.set("crate", crateNumber);
		*out << cgicc::img()
			.set("class", "icon")
			.set("src", "/emu/emuDCS/FEDApps/images/list-add.png");
		*out << "Add DCC" << std::endl;
		*out << cgicc::button() << std::endl;
		*out << cgicc::div() << std::endl;

	}

	*out << cgicc::div()
		.set("id", "config_crate_add")
		.set("class", "tier2 hidden")
		.set("crates_hidden", "1") << std::endl;
	*out << cgicc::button()
		.set("class", "add_crate");
	*out << cgicc::img()
		.set("class", "icon")
		.set("src", "/emu/emuDCS/FEDApps/images/list-add.png");
	*out << "Add crate" << std::endl;
	*out << cgicc::button() << std::endl;
	*out << cgicc::div() << std::endl;
	*/

	*out << cgicc::button()
		.set("class", "right upload_to_db action_button")
		.set("id", "upload_to_db");
	*out << "Upload configuration to database";
	*out << cgicc::button() << std::endl;

	*out << cgicc::button()
		.set("class", "right write_xml action_button")
		.set("id", "write_xml");
	*out << "Write configuration to XML";
	*out << cgicc::button() << std::endl;

	*out << cgicc::fieldset() << std::endl;
	//}

	*out << Footer() << std::endl;

}



void emu::fed::ConfigurationEditor::webGetDBKeys(xgi::Input *in, xgi::Output *out)
{
	cgicc::Cgicc cgi(in);

	// Need some header information to be able to return JSON
	if (cgi.getElement("debug") == cgi.getElements().end() || cgi["debug"]->getIntegerValue() != 1) {
		cgicc::HTTPResponseHeader jsonHeader("HTTP/1.1", 200, "OK");
		jsonHeader.addHeader("Content-type", "application/json");
		out->setHTTPResponseHeader(jsonHeader);
	}

	// Get the keys from the configurations table.
	SystemDBAgent agent(this);

	std::map<std::string, std::vector<std::pair<xdata::UnsignedInteger64, time_t> > > keyMap;

	// Make a JSON output object
	JSONSpirit::Object output;
	JSONSpirit::Array keySets;

	try {
		agent.connect(dbUsername_, dbPassword_);

		keyMap = agent.getAllKeys();

	} catch (emu::fed::exception::DBException &e) {
		// Signal the user that there has been an error
		JSONSpirit::Object systemObject;
		JSONSpirit::Array keyArray;
		keyArray.push_back("Exception loading keys");
		systemObject.push_back(JSONSpirit::Pair("name", "Exception loading keys"));
		systemObject.push_back(JSONSpirit::Pair("keys", keyArray));
		systemObject.push_back(JSONSpirit::Pair("error", e.what()));
		keySets.push_back(systemObject);

		std::ostringstream error;
		error << "Error loading keys from database: " << e.what();
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		notifyQualified("ERROR", e);
	}

	for (std::map<std::string, std::vector<std::pair<xdata::UnsignedInteger64, time_t> > >::iterator iPair = keyMap.begin(); iPair != keyMap.end(); ++iPair) {

		JSONSpirit::Object systemObject;
		JSONSpirit::Array keyArray;

		std::vector<std::pair<xdata::UnsignedInteger64, time_t> > keys = iPair->second;
		for (std::vector<std::pair<xdata::UnsignedInteger64, time_t> >::iterator iKey = keys.begin(); iKey != keys.end(); ++iKey) {
			keyArray.push_back(iKey->first.toString());
		}

		systemObject.push_back(JSONSpirit::Pair("name", iPair->first));
		systemObject.push_back(JSONSpirit::Pair("keys", keyArray));
		keySets.push_back(systemObject);
	}

	output.push_back(JSONSpirit::Pair("systems", keySets));

	*out << JSONSpirit::write(output);
}



void emu::fed::ConfigurationEditor::webUploadFile(xgi::Input *in, xgi::Output *out)
{
	cgicc::Cgicc cgi(in);

	const std::string ofile = "/tmp/config_fed_upload.xml";
	cgicc::const_file_iterator iFile = cgi.getFile("xmlFile");
	if (iFile == cgi.getFiles().end()) {
		// ERROR!
		std::ostringstream error;
		error << "Error uploading file:  unable to find file in cgi data";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE(emu::fed::exception::DBException, e, error.str());
		notifyQualified("ERROR", e);

		return;
	} else {
		std::ofstream tempFile(ofile.c_str());
		if (tempFile.good()) {
			iFile->writeToStream(tempFile);
			tempFile.close();
			LOG4CPLUS_INFO(getApplicationLogger(), "Successfully uploaded file " << iFile->getName());
		} else {
			// ERROR!
			if (tempFile.is_open()) tempFile.close();
			std::ostringstream error;
			error << "Error opening local file " << ofile << " for writing";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			XCEPT_DECLARE(emu::fed::exception::DBException, e, error.str());
			notifyQualified("ERROR", e);
			return;
		}
	}

	// Parse the XML file and build crates properly
	XMLConfigurator configurator(ofile);

	try {
		crateVector_ = configurator.setupCrates(true);
		systemName_ = configurator.getSystemName();
		timeStamp_ = configurator.getTimeStamp();
		dbKey_ = 0;
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Unable to create FED objects by parsing file " << ofile << ": " << e.what();
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE(emu::fed::exception::DBException, e, error.str());
		notifyQualified("ERROR", e);
		return;
	}

}



void emu::fed::ConfigurationEditor::webLoadFromDB(xgi::Input *in, xgi::Output *out)
{
	cgicc::Cgicc cgi(in);

	if (cgi.getElement("key") != cgi.getElements().end()) {
		dbKey_ = cgi["key"]->getIntegerValue();
	} else {
		// Unable to load key.
		// TODO report error via JSON
		std::ostringstream error;
		error << "Unable to find parameter 'key' in POST data";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		return;
	}

	// Get the configuration from the DB
	DBConfigurator configurator(this, dbUsername_.toString(), dbPassword_.toString(), dbKey_);

	try {
		crateVector_ = configurator.setupCrates(true);
		systemName_ = configurator.getSystemName();
		timeStamp_ = configurator.getTimeStamp();
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Unable to create FED objects by loading key " << dbKey_.toString() << ": " << e.what();
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		return;
	}
}



void emu::fed::ConfigurationEditor::webCreateNew(xgi::Input *in, xgi::Output *out)
{

	crateVector_.clear();
	dbKey_ = 0;
	systemName_ = "new configuration";
	timeStamp_ = time(NULL);

	// TODO respond via JSON?
	return;

}



void emu::fed::ConfigurationEditor::webWriteXML(xgi::Input *in, xgi::Output *out)
{
	cgicc::Cgicc cgi(in);

	try {
		std::string output = XMLConfigurator::makeXML(crateVector_, systemName_);

		// Need some header information to be able to return JSON
		if (cgi.getElement("debug") == cgi.getElements().end() || cgi["debug"]->getIntegerValue() != 1) {
			cgicc::HTTPResponseHeader xmlHeader("HTTP/1.1", 200, "OK");
			xmlHeader.addHeader("Content-Type", "text/xml");
			std::ostringstream attachment;
			attachment << "attachment; filename=fed-system-" << toolbox::escape(systemName_ == "" ? "unnamed" : systemName_.toString()) << "-" << toolbox::TimeVal(timeStamp_).toString(toolbox::TimeVal::gmt) << ".xml";
			xmlHeader.addHeader("Content-Disposition", attachment.str());
			out->setHTTPResponseHeader(xmlHeader);
		}

		*out << output;

	} catch (emu::fed::exception::Exception &e) {

		*out << printException(e);

	}
}



void emu::fed::ConfigurationEditor::webSystem(xgi::Input *in, xgi::Output *out)
{
	cgicc::Cgicc cgi(in);

	// Need some header information to be able to return JSON
	if (cgi.getElement("debug") == cgi.getElements().end() || cgi["debug"]->getIntegerValue() != 1) {
		cgicc::HTTPResponseHeader jsonHeader("HTTP/1.1", 200, "OK");
		jsonHeader.addHeader("Content-type", "application/json");
		out->setHTTPResponseHeader(jsonHeader);
	}

	// Make JSON output
	JSONSpirit::Object output;

	// Get the action to perform
	std::string action;
	if (cgi.getElement("action") != cgi.getElements().end()) {
		action = cgi["action"]->getValue();
	} else {
		output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
		*out << JSONSpirit::write(output);
		return;
	}

	// Switch on the action
	if (action == "get") {

		JSONSpirit::Object systemObject;

		// Push back system information
		systemObject.push_back(JSONSpirit::Pair("name", systemName_.toString()));
		systemObject.push_back(JSONSpirit::Pair("key", dbKey_.toString()));
		systemObject.push_back(JSONSpirit::Pair("timestamp", toolbox::TimeVal(timeStamp_).toString(toolbox::TimeVal::gmt)));

		// Crates are stored in an array
		JSONSpirit::Array crateArray;

		// Get crate information
		std::set<unsigned int> crateSet;
		for (std::vector<Crate *>::const_iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); ++iCrate) {

			// Each crate is an object
			JSONSpirit::Object crateObject;
			crateObject.push_back(JSONSpirit::Pair("number", (int) (*iCrate)->getNumber()));
			crateArray.push_back(crateObject);
			crateSet.insert((*iCrate)->getNumber());

		}

		// Find the next crate number to use
		unsigned int nextCrate = 0;
		while (1) {
			if (crateSet.find(nextCrate) == crateSet.end()) break;
			++nextCrate;
		}

		// Add the crates
		systemObject.push_back(JSONSpirit::Pair("crates", crateArray));
		output.push_back(JSONSpirit::Pair("nextCrate", (uint64_t) nextCrate));

		// Add the system to the output
		output.push_back(JSONSpirit::Pair("system", systemObject));

	} else if (action == "update_name") {

		// Get the name
		std::string name;
		if (cgi.getElement("value") != cgi.getElements().end()) {
			name = cgi["value"]->getValue();
		} else {
			output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
			*out << JSONSpirit::write(output);
			return;
		}

		systemName_ = name;

	} else if (action == "update_key") {

		// Get the database key
		uint64_t key;
		if (cgi.getElement("value") != cgi.getElements().end()) {
			try {
				key = getIntegerValue(cgi["value"]->getValue());
			} catch (emu::fed::exception::Exception &e) {
				output.push_back(JSONSpirit::Pair("error", "Unable to parse DB number.  " + std::string(e.what())));
				*out << JSONSpirit::write(output);
				return;
			}
		} else {
			output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
			*out << JSONSpirit::write(output);
			return;
		}

		dbKey_ = key;

	}

	output.push_back(JSONSpirit::Pair("ok", true));
	*out << JSONSpirit::write(output);
}



void emu::fed::ConfigurationEditor::webCrate(xgi::Input *in, xgi::Output *out)
{
	cgicc::Cgicc cgi(in);

	// Need some header information to be able to return JSON
	if (cgi.getElement("debug") == cgi.getElements().end() || cgi["debug"]->getIntegerValue() != 1) {
		cgicc::HTTPResponseHeader jsonHeader("HTTP/1.1", 200, "OK");
		jsonHeader.addHeader("Content-type", "application/json");
		out->setHTTPResponseHeader(jsonHeader);
	}

	// Make JSON output
	JSONSpirit::Object output;

	// Get the action to perform
	std::string action;
	if (cgi.getElement("action") != cgi.getElements().end()) {
		action = cgi["action"]->getValue();
	} else {
		output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
		*out << JSONSpirit::write(output);
		return;
	}

	// Crate number is universal
	unsigned int crateNumber;
	if (cgi.getElement("crate") != cgi.getElements().end()) {
		try {
			crateNumber = getIntegerValue(cgi["crate"]->getValue());
		} catch (emu::fed::exception::Exception &e) {
			output.push_back(JSONSpirit::Pair("error", "Unable to parse crate number.  " + std::string(e.what())));
			*out << JSONSpirit::write(output);
			return;
		}
	} else {
		output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
		*out << JSONSpirit::write(output);
		return;
	}

	// Switch on the action
	if (action == "get") {

		// Find that crate
		Crate *crate = NULL;
		for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); ++iCrate) {
			if ((*iCrate)->getNumber() == crateNumber) {
				crate = (*iCrate);
				break;
			}
		}
		if (crate == NULL) {
			output.push_back(JSONSpirit::Pair("error", "Unable to find crate."));
			*out << JSONSpirit::write(output);
			return;
		}

		// The crate itself is an object with that name
		JSONSpirit::Object crateObject;

		// Push back crate information
		crateObject.push_back(JSONSpirit::Pair("number", (uint64_t) crate->getNumber()));

		// The crate has boards
		JSONSpirit::Object controllerObject;
		JSONSpirit::Array dduArray;
		JSONSpirit::Array dccArray;

		std::vector<DDU *> dduVector = crate->getDDUs();
		std::vector<DCC *> dccVector = crate->getDCCs();

		// Controller is obvious, so we skip it

		// DDUs first
		for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); ++iDDU) {

			JSONSpirit::Object dduObject;
			dduObject.push_back(JSONSpirit::Pair("rui", (uint64_t) (*iDDU)->getRUI()));
			dduObject.push_back(JSONSpirit::Pair("slot", (uint64_t) (*iDDU)->getSlot()));

			dduArray.push_back(dduObject);

		}
		crateObject.push_back(JSONSpirit::Pair("ddus", dduArray));

		// DCCs second
		for (std::vector<DCC *>::iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); ++iDCC) {

			JSONSpirit::Object dccObject;
			dccObject.push_back(JSONSpirit::Pair("fmmid", (uint64_t) (*iDCC)->getFMMID()));
			dccObject.push_back(JSONSpirit::Pair("slot", (uint64_t) (*iDCC)->getSlot()));

			dccArray.push_back(dccObject);

		}
		crateObject.push_back(JSONSpirit::Pair("dccs", dccArray));

		// Get the next available RUI, FMMID, and slot
		std::set<unsigned int> ruiSet;
		std::set<unsigned int> fmmidSet;
		std::set<unsigned int> slotSet;
		for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); ++iCrate) {
			std::vector<DDU *> ddus = (*iCrate)->getDDUs();
			std::vector<DCC *> dccs = (*iCrate)->getDCCs();
			std::vector<VMEModule *> modules = (*iCrate)->getBoards<VMEModule>();
			for (std::vector<DDU *>::iterator iDDU = ddus.begin(); iDDU != ddus.end(); ++iDDU) {
				ruiSet.insert((*iDDU)->getRUI());
			}
			for (std::vector<DCC *>::iterator iDCC = dccs.begin(); iDCC != dccs.end(); ++iDCC) {
				fmmidSet.insert((*iDCC)->getFMMID());
			}
			if ((*iCrate)->getNumber() == crateNumber) {
				for (std::vector<VMEModule *>::iterator iBoard = modules.begin(); iBoard != modules.end(); ++iBoard) {
					slotSet.insert((*iBoard)->getSlot());
				}
			}
		}

		// Find the next available RUI, FMMID, and slot numbers
		unsigned int nextRUI = 0;
		while (1) {
			if (ruiSet.find(nextRUI) == ruiSet.end()) break;
			++nextRUI;
		}

		unsigned int nextFMMID = 0;
		while (1) {
			if (fmmidSet.find(nextFMMID) == fmmidSet.end()) break;
			++nextFMMID;
		}

		unsigned int nextSlot = 0;
		while (1) {
			if (slotSet.find(nextSlot) == slotSet.end()) break;
			++nextSlot;
		}

		output.push_back(JSONSpirit::Pair("nextRUI", (uint64_t) nextRUI));
		output.push_back(JSONSpirit::Pair("nextFMMID", (uint64_t) nextFMMID));
		output.push_back(JSONSpirit::Pair("nextSlot", (uint64_t) nextSlot));

		// Add the crate to the output
		output.push_back(JSONSpirit::Pair("crate", crateObject));

	} else if (action == "add") {

		// Get the new crate number
		unsigned int newCrate = 0;
		if (cgi.getElement("value") != cgi.getElements().end()) {
			try {
				newCrate = getIntegerValue(cgi["value"]->getValue());
			} catch (emu::fed::exception::Exception &e) {
				output.push_back(JSONSpirit::Pair("error", "Unable to parse new crate number.  " + std::string(e.what())));
				*out << JSONSpirit::write(output);
				return;
			}
		} else {
			output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
			*out << JSONSpirit::write(output);
			return;
		}

		// Check if the crate already exists.
		std::set<unsigned int> crateSet;
		crateSet.insert(newCrate);
		for (std::vector<Crate *>::const_iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); ++iCrate) {
			crateSet.insert((*iCrate)->getNumber());
			if ((*iCrate)->getNumber() == newCrate) {
				output.push_back(JSONSpirit::Pair("error", "The crate number is already in use."));
				*out << JSONSpirit::write(output);
				return;
			}
		}

		// Make the new crate
		Crate *crate = new Crate(newCrate);
		crate->setController(new VMEController(0, 0, true));
		crateVector_.push_back(crate);

		// Find the next crate number
		unsigned int nextCrate = 0;
		while (1) {
			if (crateSet.find(nextCrate) == crateSet.end()) break;
			++nextCrate;
		}

		output.push_back(JSONSpirit::Pair("nextCrate", (uint64_t) nextCrate));

		// Return the new crate number
		output.push_back(JSONSpirit::Pair("value", (uint64_t) newCrate));

	} else if (action == "update_number") {

		// Get the new crate number
		unsigned int newCrate = 0;
		if (cgi.getElement("value") != cgi.getElements().end()) {
			try {
				newCrate = getIntegerValue(cgi["value"]->getValue());
			} catch (emu::fed::exception::Exception &e) {
				output.push_back(JSONSpirit::Pair("error", "Unable to parse new crate number.  " + std::string(e.what())));
				*out << JSONSpirit::write(output);
				return;
			}
		} else {
			output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
			*out << JSONSpirit::write(output);
			return;
		}

		// Check if the other crate is around
		Crate *crate = NULL;
		for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); ++iCrate) {
			if ((*iCrate)->getNumber() == crateNumber) {
				crate = (*iCrate);
				break;
			}
		}
		if (crate == NULL) {
			output.push_back(JSONSpirit::Pair("error", "Unable to find crate to modify."));
			*out << JSONSpirit::write(output);
			return;
		}

		// Check if the new crate is around
		std::set<unsigned int> crateSet;
		crateSet.insert(newCrate);
		for (std::vector<Crate *>::const_iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); ++iCrate) {
			if ((*iCrate)->getNumber() != crateNumber) crateSet.insert((*iCrate)->getNumber());
			if ((*iCrate)->getNumber() == newCrate) {
				output.push_back(JSONSpirit::Pair("error", "The crate number is already in use."));
				*out << JSONSpirit::write(output);
				return;
			}
		}

		// Else set and return
		crate->setNumber(newCrate);

		// Find the next crate number
		unsigned int nextCrate = 0;
		while (1) {
			if (crateSet.find(nextCrate) == crateSet.end()) break;
			++nextCrate;
		}

		output.push_back(JSONSpirit::Pair("nextCrate", (uint64_t) nextCrate));

		if (newCrate > 5) {
			output.push_back(JSONSpirit::Pair("warning", "Crate numbers are typically between 0 and 5."));
		}

		output.push_back(JSONSpirit::Pair("value", (uint64_t) newCrate));
		output.push_back(JSONSpirit::Pair("previous", (uint64_t) crateNumber));

	} else if (action == "delete") {

		// Check if the old crate is around
		bool crateFound = false;
		std::set<unsigned int> crateSet;
		std::vector<Crate *>::iterator eraseMe;
		for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); ++iCrate) {
			if ((*iCrate)->getNumber() == crateNumber) {
				crateFound = true;
				eraseMe = iCrate;
			} else {
				crateSet.insert((*iCrate)->getNumber());
			}
		}
		if (!crateFound) {
			output.push_back(JSONSpirit::Pair("error", "Unable to find crate to delete."));
			*out << JSONSpirit::write(output);
			return;
		}
		crateVector_.erase(eraseMe);

		// Find the next crate number
		unsigned int nextCrate = 0;
		while (1) {
			if (crateSet.find(nextCrate) == crateSet.end()) break;
			++nextCrate;
		}

		output.push_back(JSONSpirit::Pair("nextCrate", (uint64_t) nextCrate));

		output.push_back(JSONSpirit::Pair("value", (uint64_t) crateNumber));

	} else {
		output.push_back(JSONSpirit::Pair("error", "The action \"" + action + "\" is not recognized."));
		*out << JSONSpirit::write(output);
		return;
	}

	output.push_back(JSONSpirit::Pair("ok", true));
	*out << JSONSpirit::write(output);

}



void emu::fed::ConfigurationEditor::webController(xgi::Input *in, xgi::Output *out)
{
	cgicc::Cgicc cgi(in);

	// Need some header information to be able to return JSON
	if (cgi.getElement("debug") == cgi.getElements().end() || cgi["debug"]->getIntegerValue() != 1) {
		cgicc::HTTPResponseHeader jsonHeader("HTTP/1.1", 200, "OK");
		jsonHeader.addHeader("Content-type", "application/json");
		out->setHTTPResponseHeader(jsonHeader);
	}

	// Make JSON output
	JSONSpirit::Object output;

	// Get the action to perform
	std::string action;
	if (cgi.getElement("action") != cgi.getElements().end()) {
		action = cgi["action"]->getValue();
	} else {
		output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
		*out << JSONSpirit::write(output);
		return;
	}

	// Crate number is universal
	unsigned int crateNumber;
	if (cgi.getElement("crate") != cgi.getElements().end()) {
		try {
			crateNumber = getIntegerValue(cgi["crate"]->getValue());
		} catch (emu::fed::exception::Exception &e) {
			output.push_back(JSONSpirit::Pair("error", "Unable to parse crate number.  " + std::string(e.what())));
			*out << JSONSpirit::write(output);
			return;
		}
	} else {
		output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
		*out << JSONSpirit::write(output);
		return;
	}

	// Switch on the action
	if (action == "get") {

		// Find that crate
		Crate *crate = NULL;
		for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); ++iCrate) {
			if ((*iCrate)->getNumber() == crateNumber) {
				crate = (*iCrate);
				break;
			}
		}
		if (crate == NULL) {
			output.push_back(JSONSpirit::Pair("error", "Unable to find crate."));
			*out << JSONSpirit::write(output);
			return;
		}

		// The controller itself is an object with that name
		JSONSpirit::Object controllerObject;

		// Push back controller information
		controllerObject.push_back(JSONSpirit::Pair("device", (uint64_t) crate->getController()->getDevice()));
		controllerObject.push_back(JSONSpirit::Pair("link", (uint64_t) crate->getController()->getLink()));
		controllerObject.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));

		// Add the controller to the output
		output.push_back(JSONSpirit::Pair("controller", controllerObject));

	} else if (action == "update_device") {

		// Find that crate
		Crate *crate = NULL;
		for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); ++iCrate) {
			if ((*iCrate)->getNumber() == crateNumber) {
				crate = (*iCrate);
				break;
			}
		}
		if (crate == NULL) {
			output.push_back(JSONSpirit::Pair("error", "Unable to find crate."));
			*out << JSONSpirit::write(output);
			return;
		}

		// Get the new device number
		uint64_t value;
		if (cgi.getElement("value") != cgi.getElements().end()) {
			try {
				value = getIntegerValue(cgi["value"]->getValue());
			} catch (emu::fed::exception::Exception &e) {
				output.push_back(JSONSpirit::Pair("error", "Unable to parse device number.  " + std::string(e.what())));
				*out << JSONSpirit::write(output);
				return;
			}
		} else {
			output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
			*out << JSONSpirit::write(output);
			return;
		}

		// Set the device number.  Have to delete the controller first.
		unsigned int link = crate->getController()->getLink();
		delete crate->getController();
		crate->setController(new VMEController(value, link, true));

		// Don't need to do anything more.

	} else if (action == "update_link") {

		// Find that crate
		Crate *crate = NULL;
		for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); ++iCrate) {
			if ((*iCrate)->getNumber() == crateNumber) {
				crate = (*iCrate);
				break;
			}
		}
		if (crate == NULL) {
			output.push_back(JSONSpirit::Pair("error", "Unable to find crate."));
			*out << JSONSpirit::write(output);
			return;
		}

		// Get the new link number
		uint64_t value;
		if (cgi.getElement("value") != cgi.getElements().end()) {
			try {
				value = getIntegerValue(cgi["value"]->getValue());
			} catch (emu::fed::exception::Exception &e) {
				output.push_back(JSONSpirit::Pair("error", "Unable to parse link number.  " + std::string(e.what())));
				*out << JSONSpirit::write(output);
				return;
			}
		} else {
			output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
			*out << JSONSpirit::write(output);
			return;
		}

		// Set the link number.  Have to delete the controller first.
		unsigned int device = crate->getController()->getDevice();
		delete crate->getController();
		crate->setController(new VMEController(device, value, true));

		// Don't need to do anything more.

	} else {
		output.push_back(JSONSpirit::Pair("error", "The action \"" + action + "\" is not recognized."));
		*out << JSONSpirit::write(output);
		return;
	}

	output.push_back(JSONSpirit::Pair("ok", true));
	*out << JSONSpirit::write(output);
}



void emu::fed::ConfigurationEditor::webDDU(xgi::Input *in, xgi::Output *out)
{
	cgicc::Cgicc cgi(in);

	// Need some header information to be able to return JSON
	if (cgi.getElement("debug") == cgi.getElements().end() || cgi["debug"]->getIntegerValue() != 1) {
		cgicc::HTTPResponseHeader jsonHeader("HTTP/1.1", 200, "OK");
		jsonHeader.addHeader("Content-type", "application/json");
		out->setHTTPResponseHeader(jsonHeader);
	}

	// Make JSON output
	JSONSpirit::Object output;

	// Get the action to perform
	std::string action;
	if (cgi.getElement("action") != cgi.getElements().end()) {
		action = cgi["action"]->getValue();
	} else {
		output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
		*out << JSONSpirit::write(output);
		return;
	}

	// Crate number is universal
	unsigned int crateNumber;
	if (cgi.getElement("crate") != cgi.getElements().end()) {
		try {
			crateNumber = getIntegerValue(cgi["crate"]->getValue());
		} catch (emu::fed::exception::Exception &e) {
			output.push_back(JSONSpirit::Pair("error", "Unable to parse crate number.  " + std::string(e.what())));
			*out << JSONSpirit::write(output);
			return;
		}
	} else {
		output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
		*out << JSONSpirit::write(output);
		return;
	}

	// Find the crate
	Crate *crate = NULL;
	for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
		if ((*iCrate)->getNumber() == crateNumber) {
			crate = (*iCrate);
			break;
		}
	}
	if (crate == NULL) {
		std::ostringstream error;
		error << "Unable to find crate matching number " << crateNumber;
		output.push_back(JSONSpirit::Pair("error", error.str()));
		*out << JSONSpirit::write(output);
		return;
	}

	// Get rui
	unsigned int rui = 0;
	if (cgi.getElement("rui") != cgi.getElements().end()) {
		try {
			rui = getIntegerValue(cgi["rui"]->getValue());
		} catch (emu::fed::exception::Exception &e) {
			output.push_back(JSONSpirit::Pair("error", "Unable to parse RUI.  " + std::string(e.what())));
			*out << JSONSpirit::write(output);
			return;
		}
	} else {
		output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
		*out << JSONSpirit::write(output);
		return;
	}

	// Switch on the action
	if (action == "get") {

		// Find the DDU
		std::vector<DDU *> dduVector = crate->getDDUs();
		DDU *ddu = NULL;
		for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); ++iDDU) {
			if ((*iDDU)->getRUI() == rui) {
				ddu = (*iDDU);
				break;
			}
		}
		if (ddu == NULL) {
			std::ostringstream error;
			error << "Unable to find DDU matching RUI " << rui;
			output.push_back(JSONSpirit::Pair("error", error.str()));
			*out << JSONSpirit::write(output);
			return;
		}

		// Return everything about the DDU
		JSONSpirit::Object dduObject;
		dduObject.push_back(JSONSpirit::Pair("slot", (uint64_t) ddu->getSlot()));
		dduObject.push_back(JSONSpirit::Pair("rui", (uint64_t) ddu->getRUI()));
		dduObject.push_back(JSONSpirit::Pair("fmmid", (uint64_t) ddu->getFMMID()));
		dduObject.push_back(JSONSpirit::Pair("gbe_prescale", (uint64_t) ddu->getGbEPrescale()));
		// Options
		uint32_t killfiber = ddu->getKillFiber();
		dduObject.push_back(JSONSpirit::Pair("force_checks", (bool) (killfiber & 0x8000)));
		dduObject.push_back(JSONSpirit::Pair("force_alct", (bool) (killfiber & 0x10000)));
		dduObject.push_back(JSONSpirit::Pair("force_tmb", (bool) (killfiber & 0x20000)));
		dduObject.push_back(JSONSpirit::Pair("force_cfeb", (bool) (killfiber & 0x40000)));
		dduObject.push_back(JSONSpirit::Pair("force_dmb", (bool) (killfiber & 0x80000)));

		// Get the fiber numbers as well
		std::vector<Fiber *> fiberVector = ddu->getFibers();
		std::set<unsigned int> fiberSet;
		JSONSpirit::Array fiberArray;
		for (std::vector<Fiber *>::iterator iFiber = fiberVector.begin(); iFiber != fiberVector.end(); ++iFiber) {
			JSONSpirit::Object fiberObject;
			fiberObject.push_back(JSONSpirit::Pair("fiber", (uint64_t) (*iFiber)->getFiberNumber()));
			fiberSet.insert((*iFiber)->getFiberNumber());
			fiberArray.push_back(fiberObject);
		}

		// Add the fibers
		dduObject.push_back(JSONSpirit::Pair("fibers", fiberArray));

		// Find the next available fiber number
		unsigned int nextFiber = 0;
		while (1) {
			if (fiberSet.find(nextFiber) == fiberSet.end()) break;
			++nextFiber;
		}

		output.push_back(JSONSpirit::Pair("nextFiber", (uint64_t) nextFiber));
		output.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));

		// Return the DDU
		output.push_back(JSONSpirit::Pair("ddu", dduObject));

	} else if (action == "add") {

		// Make sure that DDU doesn't already exist in this crate
		std::vector<DDU *> dduVector = crate->getDDUs();
		for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); ++iDDU) {
			if ((*iDDU)->getRUI() == rui) {
				output.push_back(JSONSpirit::Pair("error", "That RUI already exists in this crate"));
				*out << JSONSpirit::write(output);
				return;
			}
		}

		// Get slot
		unsigned int slot = 0;
		if (cgi.getElement("slot") != cgi.getElements().end()) {
			try {
				slot = getIntegerValue(cgi["slot"]->getValue());
			} catch (emu::fed::exception::Exception &e) {
				output.push_back(JSONSpirit::Pair("error", "Unable to parse slot.  " + std::string(e.what())));
				*out << JSONSpirit::write(output);
				return;
			}
		} else {
			output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
			*out << JSONSpirit::write(output);
			return;
		}

		// Make sure that slot isn't already occupied
		for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); ++iDDU) {
			if ((*iDDU)->getSlot() == slot) {
				output.push_back(JSONSpirit::Pair("error", "That slot is already occupied in this crate"));
				*out << JSONSpirit::write(output);
				return;
			}
		}

		// Make the new DDU
		DDU *ddu = new DDU(slot, true);
		ddu->setRUI(rui);
		crate->addBoard(ddu);

		// Get the next available RUI and slot
		std::set<unsigned int> ruiSet;
		std::set<unsigned int> slotSet;
		for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); ++iCrate) {
			std::vector<DDU *> ddus = (*iCrate)->getDDUs();
			std::vector<VMEModule *> modules = (*iCrate)->getBoards<VMEModule>();
			for (std::vector<DDU *>::iterator iDDU = ddus.begin(); iDDU != ddus.end(); ++iDDU) {
				ruiSet.insert((*iDDU)->getRUI());
			}
			if ((*iCrate)->getNumber() == crateNumber) {
				for (std::vector<VMEModule *>::iterator iBoard = modules.begin(); iBoard != modules.end(); ++iBoard) {
					slotSet.insert((*iBoard)->getSlot());
				}
			}
		}

		// Find a new RUI (or die)
		unsigned int nextRUI = 0;
		while (1) {
			if (ruiSet.find(nextRUI) == ruiSet.end()) break;
			++nextRUI;
		}

		// Find a new slot (or die)
		unsigned int nextSlot = 0;
		while (1) {
			if (slotSet.find(nextSlot) == slotSet.end()) break;
			++nextSlot;
		}

		// Return everything we need to return
		output.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));
		output.push_back(JSONSpirit::Pair("value", (uint64_t) rui));
		output.push_back(JSONSpirit::Pair("nextRUI", (uint64_t) nextRUI));
		output.push_back(JSONSpirit::Pair("nextSlot", (uint64_t) nextSlot));

	} else if (action == "delete") {

		// Find the DDU
		std::vector<DDU *> dduVector = crate->getDDUs();
		DDU *ddu = NULL;
		for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); ++iDDU) {
			if ((*iDDU)->getRUI() == rui) {
				ddu = (*iDDU);
			}
		}
		if (ddu == NULL) {
			std::ostringstream error;
			error << "Unable to find DDU matching RUI " << rui;
			output.push_back(JSONSpirit::Pair("error", error.str()));
			*out << JSONSpirit::write(output);
			return;
		}

		// Delete the board
		crate->deleteBoard(ddu);

		// Get the next available RUI and slot
		std::set<unsigned int> ruiSet;
		std::set<unsigned int> slotSet;
		for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); ++iCrate) {
			std::vector<DDU *> ddus = (*iCrate)->getDDUs();
			std::vector<VMEModule *> modules = (*iCrate)->getBoards<VMEModule>();
			for (std::vector<DDU *>::iterator iDDU = ddus.begin(); iDDU != ddus.end(); ++iDDU) {
				ruiSet.insert((*iDDU)->getRUI());
			}
			if ((*iCrate)->getNumber() == crateNumber) {
				for (std::vector<VMEModule *>::iterator iBoard = modules.begin(); iBoard != modules.end(); ++iBoard) {
					slotSet.insert((*iBoard)->getSlot());
				}
			}
		}

		// Find a new RUI (or die)
		unsigned int nextRUI = 0;
		while (1) {
			if (ruiSet.find(nextRUI) == ruiSet.end()) break;
			++nextRUI;
		}

		// Find a new slot (or die)
		unsigned int nextSlot = 0;
		while (1) {
			if (slotSet.find(nextSlot) == slotSet.end()) break;
			++nextSlot;
		}

		// Return everything we need to return
		output.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));
		output.push_back(JSONSpirit::Pair("value", (uint64_t) rui));
		output.push_back(JSONSpirit::Pair("nextRUI", (uint64_t) nextRUI));
		output.push_back(JSONSpirit::Pair("nextSlot", (uint64_t) nextSlot));

	} else if (action == "update_rui") {

		// Find the DDU
		std::vector<DDU *> dduVector = crate->getDDUs();
		DDU *ddu = NULL;
		for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); ++iDDU) {
			if ((*iDDU)->getRUI() == rui) {
				ddu = (*iDDU);
			}
		}
		if (ddu == NULL) {
			std::ostringstream error;
			error << "Unable to find DDU matching RUI " << rui;
			output.push_back(JSONSpirit::Pair("error", error.str()));
			*out << JSONSpirit::write(output);
			return;
		}

		// Get the new rui
		unsigned int value = 0;
		if (cgi.getElement("value") != cgi.getElements().end()) {
			try {
				value = getIntegerValue(cgi["value"]->getValue());
			} catch (emu::fed::exception::Exception &e) {
				output.push_back(JSONSpirit::Pair("error", "Unable to parse RUI.  " + std::string(e.what())));
				*out << JSONSpirit::write(output);
				return;
			}
		} else {
			output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
			*out << JSONSpirit::write(output);
			return;
		}

		// See if there is a DDU with the new RUI already
		for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); ++iDDU) {
			if ((*iDDU)->getRUI() == value) {
				std::ostringstream error;
				error << "There already exists a DDU matching RUI " << rui;
				output.push_back(JSONSpirit::Pair("error", error.str()));
				*out << JSONSpirit::write(output);
				return;
			}
		}

		// Update the board
		ddu->setRUI(value);

		// Get the next available RUI
		std::set<unsigned int> ruiSet;
		for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); ++iCrate) {
			std::vector<DDU *> ddus = (*iCrate)->getDDUs();
			for (std::vector<DDU *>::iterator iDDU = ddus.begin(); iDDU != ddus.end(); ++iDDU) {
				ruiSet.insert((*iDDU)->getRUI());
			}
		}

		// Find a new RUI (or die)
		unsigned int nextRUI = 0;
		while (1) {
			if (ruiSet.find(nextRUI) == ruiSet.end()) break;
			++nextRUI;
		}

		// Return everything we need to return
		output.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));
		output.push_back(JSONSpirit::Pair("previous", (uint64_t) rui));
		output.push_back(JSONSpirit::Pair("value", (uint64_t) value));
		output.push_back(JSONSpirit::Pair("nextRUI", (uint64_t) nextRUI));

	} else if (action == "update_slot") {

		// Find the DDU
		std::vector<DDU *> dduVector = crate->getDDUs();
		DDU *ddu = NULL;
		for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); ++iDDU) {
			if ((*iDDU)->getRUI() == rui) {
				ddu = (*iDDU);
			}
		}
		if (ddu == NULL) {
			std::ostringstream error;
			error << "Unable to find DDU matching RUI " << rui;
			output.push_back(JSONSpirit::Pair("error", error.str()));
			*out << JSONSpirit::write(output);
			return;
		}

		// Get the new slot
		unsigned int value = 0;
		if (cgi.getElement("value") != cgi.getElements().end()) {
			try {
				value = getIntegerValue(cgi["value"]->getValue());
			} catch (emu::fed::exception::Exception &e) {
				output.push_back(JSONSpirit::Pair("error", "Unable to parse RUI.  " + std::string(e.what())));
				*out << JSONSpirit::write(output);
				return;
			}
		} else {
			output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
			*out << JSONSpirit::write(output);
			return;
		}

		// Error if the slot is bad
		if (value < 2 || value > 20) {
			output.push_back(JSONSpirit::Pair("error", "Slot numbers must be between 2 and 20."));
			*out << JSONSpirit::write(output);
			return;
		}

		// See if there is a board with the new slot already
		std::vector<VMEModule *> boardVector = crate->getBoards<VMEModule>();
		for (std::vector<VMEModule *>::iterator iBoard = boardVector.begin(); iBoard != boardVector.end(); ++iBoard) {
			if ((*iBoard)->getSlot() == value) {
				std::ostringstream error;
				error << "There already exists a board matching slot " << value;
				output.push_back(JSONSpirit::Pair("error", error.str()));
				*out << JSONSpirit::write(output);
				return;
			}
		}

		// Update the board
		ddu->setSlot(value);

		// Warn if the slot is strange
		if (value == 8 || value == 17) {
			output.push_back(JSONSpirit::Pair("warning", "Slot numbers 8 and 17 are usually reserved for DCCs."));
		} else if (value == 2) {
			output.push_back(JSONSpirit::Pair("warning", "Slot 2 is usually reserved for the Track-Finder DDU."));
		}

		// Get the next available slot
		std::set<unsigned int> slotSet;
		for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); ++iCrate) {
			std::vector<VMEModule *> modules = (*iCrate)->getBoards<VMEModule>();
			if ((*iCrate)->getNumber() == crateNumber) {
				for (std::vector<VMEModule *>::iterator iBoard = modules.begin(); iBoard != modules.end(); ++iBoard) {
					slotSet.insert((*iBoard)->getSlot());
				}
			}
		}

		// Find a new slot (or die)
		unsigned int nextSlot = 0;
		while (1) {
			if (slotSet.find(nextSlot) == slotSet.end()) break;
			++nextSlot;
		}

		// Return everything we need to return
		output.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));
		output.push_back(JSONSpirit::Pair("rui", (uint64_t) rui));
		output.push_back(JSONSpirit::Pair("value", (uint64_t) value));
		output.push_back(JSONSpirit::Pair("nextSlot", (uint64_t) nextSlot));

	} else if (action == "update_fmmid") {

		// Find the DDU
		std::vector<DDU *> dduVector = crate->getDDUs();
		DDU *ddu = NULL;
		for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); ++iDDU) {
			if ((*iDDU)->getRUI() == rui) {
				ddu = (*iDDU);
			}
		}
		if (ddu == NULL) {
			std::ostringstream error;
			error << "Unable to find DDU matching RUI " << rui;
			output.push_back(JSONSpirit::Pair("error", error.str()));
			*out << JSONSpirit::write(output);
			return;
		}

		// Get the new FMMID
		unsigned int value = 0;
		if (cgi.getElement("value") != cgi.getElements().end()) {
			try {
				value = getIntegerValue(cgi["value"]->getValue());
			} catch (emu::fed::exception::Exception &e) {
				output.push_back(JSONSpirit::Pair("error", "Unable to parse FMMID.  " + std::string(e.what())));
				*out << JSONSpirit::write(output);
				return;
			}
		} else {
			output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
			*out << JSONSpirit::write(output);
			return;
		}

		// Update the board
		ddu->setFMMID(value);

		// Return everything we need to return
		output.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));
		output.push_back(JSONSpirit::Pair("rui", (uint64_t) rui));
		output.push_back(JSONSpirit::Pair("value", (uint64_t) value));

	} else if (action == "update_gbe") {

		// Find the DDU
		std::vector<DDU *> dduVector = crate->getDDUs();
		DDU *ddu = NULL;
		for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); ++iDDU) {
			if ((*iDDU)->getRUI() == rui) {
				ddu = (*iDDU);
			}
		}
		if (ddu == NULL) {
			std::ostringstream error;
			error << "Unable to find DDU matching RUI " << rui;
			output.push_back(JSONSpirit::Pair("error", error.str()));
			*out << JSONSpirit::write(output);
			return;
		}

		// Get the new GbE
		unsigned int value = 0;
		if (cgi.getElement("value") != cgi.getElements().end()) {
			try {
				value = getIntegerValue(cgi["value"]->getValue());
			} catch (emu::fed::exception::Exception &e) {
				output.push_back(JSONSpirit::Pair("error", "Unable to parse GbE prescale.  " + std::string(e.what())));
				*out << JSONSpirit::write(output);
				return;
			}
		} else {
			output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
			*out << JSONSpirit::write(output);
			return;
		}

		// Update the board
		ddu->setGbEPrescale(value);

		// Return everything we need to return
		output.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));
		output.push_back(JSONSpirit::Pair("rui", (uint64_t) rui));
		output.push_back(JSONSpirit::Pair("value", (uint64_t) value));

	} else if (action.substr(0,12) == "update_force") {

		// Find the DDU
		std::vector<DDU *> dduVector = crate->getDDUs();
		DDU *ddu = NULL;
		for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); ++iDDU) {
			if ((*iDDU)->getRUI() == rui) {
				ddu = (*iDDU);
			}
		}
		if (ddu == NULL) {
			std::ostringstream error;
			error << "Unable to find DDU matching RUI " << rui;
			output.push_back(JSONSpirit::Pair("error", error.str()));
			*out << JSONSpirit::write(output);
			return;
		}

		// Get the new setting
		bool value = false;
		if (cgi.getElement("value") != cgi.getElements().end()) {
			value = cgi["value"]->getValue() == "on";
		}

		// Update the board
		uint32_t killFiber = ddu->getKillFiber();
		if (action == "update_force_checks") {
			if (value) killFiber |= 0x8000;
			else killFiber &= 0xf7fff;
		} else if (action == "update_force_alct") {
			if (value) killFiber |= 0x10000;
			else killFiber &= 0xeffff;
		} else if (action == "update_force_tmb") {
			if (value) killFiber |= 0x20000;
			else killFiber &= 0xdffff;
		} else if (action == "update_force_cfeb") {
			if (value) killFiber |= 0x40000;
			else killFiber &= 0xbffff;
		} else if (action == "update_force_dmb") {
			if (value) killFiber |= 0x80000;
			else killFiber &= 0x7ffff;
		}
		ddu->setKillFiber(killFiber);

		// Return everything we need to return
		output.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));
		output.push_back(JSONSpirit::Pair("rui", (uint64_t) rui));
		output.push_back(JSONSpirit::Pair("value", (bool) value));

	} else {
		output.push_back(JSONSpirit::Pair("error", "The action \"" + action + "\" is not recognized."));
		*out << JSONSpirit::write(output);
		return;
	}

	output.push_back(JSONSpirit::Pair("ok", true));
	*out << JSONSpirit::write(output);

}



void emu::fed::ConfigurationEditor::webFiber(xgi::Input *in, xgi::Output *out)
{
	cgicc::Cgicc cgi(in);

	// Need some header information to be able to return JSON
	if (cgi.getElement("debug") == cgi.getElements().end() || cgi["debug"]->getIntegerValue() != 1) {
		cgicc::HTTPResponseHeader jsonHeader("HTTP/1.1", 200, "OK");
		jsonHeader.addHeader("Content-type", "application/json");
		out->setHTTPResponseHeader(jsonHeader);
	}

	// Make JSON output
	JSONSpirit::Object output;

	// Get the action to perform
	std::string action;
	if (cgi.getElement("action") != cgi.getElements().end()) {
		action = cgi["action"]->getValue();
	} else {
		output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
		*out << JSONSpirit::write(output);
		return;
	}

	// Crate number is universal
	unsigned int crateNumber;
	if (cgi.getElement("crate") != cgi.getElements().end()) {
		try {
			crateNumber = getIntegerValue(cgi["crate"]->getValue());
		} catch (emu::fed::exception::Exception &e) {
			output.push_back(JSONSpirit::Pair("error", "Unable to parse crate number.  " + std::string(e.what())));
			*out << JSONSpirit::write(output);
			return;
		}
	} else {
		output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
		*out << JSONSpirit::write(output);
		return;
	}

	// Find the crate
	Crate *crate = NULL;
	for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
		if ((*iCrate)->getNumber() == crateNumber) {
			crate = (*iCrate);
			break;
		}
	}
	if (crate == NULL) {
		std::ostringstream error;
		error << "Unable to find crate matching number " << crateNumber;
		output.push_back(JSONSpirit::Pair("error", error.str()));
		*out << JSONSpirit::write(output);
		return;
	}

	// Get rui
	unsigned int rui = 0;
	if (cgi.getElement("rui") != cgi.getElements().end()) {
		try {
			rui = getIntegerValue(cgi["rui"]->getValue());
		} catch (emu::fed::exception::Exception &e) {
			output.push_back(JSONSpirit::Pair("error", "Unable to parse RUI.  " + std::string(e.what())));
			*out << JSONSpirit::write(output);
			return;
		}
	} else {
		output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
		*out << JSONSpirit::write(output);
		return;
	}

	// Find DDU
	std::vector<DDU *> dduVector = crate->getDDUs();
	DDU *ddu = NULL;
	for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
		if ((*iDDU)->getRUI() == rui) {
			ddu = (*iDDU);
			break;
		}
	}
	if (ddu == NULL) {
		std::ostringstream error;
		error << "Unable to find DDU matching number " << rui;
		output.push_back(JSONSpirit::Pair("error", error.str()));
		*out << JSONSpirit::write(output);
		return;
	}

	// Get fiber number
	unsigned int number = 0;
	if (cgi.getElement("fiber") != cgi.getElements().end()) {
		try {
			number = getIntegerValue(cgi["fiber"]->getValue());
		} catch (emu::fed::exception::Exception &e) {
			output.push_back(JSONSpirit::Pair("error", "Unable to parse fiber number.  " + std::string(e.what())));
			*out << JSONSpirit::write(output);
			return;
		}
	} else {
		output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
		*out << JSONSpirit::write(output);
		return;
	}


	// Switch on the action
	if (action == "get") {

		// Find the fiber
		std::vector<Fiber *> fiberVector = ddu->getFibers();
		Fiber *fiber = NULL;
		for (std::vector<Fiber *>::iterator iFiber = fiberVector.begin(); iFiber != fiberVector.end(); ++iFiber) {
			if ((*iFiber)->getFiberNumber() == number) {
				fiber = (*iFiber);
			}
		}
		if (fiber == NULL) {
			std::ostringstream error;
			error << "Unable to find fiber matching number " << number;
			output.push_back(JSONSpirit::Pair("error", error.str()));
			*out << JSONSpirit::write(output);
			return;
		}

		// The fiber itself is an object
		JSONSpirit::Object fiberObject;

		// Push back everything we need
		fiberObject.push_back(JSONSpirit::Pair("fiber", (uint64_t) number));
		fiberObject.push_back(JSONSpirit::Pair("name", fiber->getName()));
		fiberObject.push_back(JSONSpirit::Pair("killed", fiber->isKilled()));

		output.push_back(JSONSpirit::Pair("fiber", fiberObject));
		output.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));
		output.push_back(JSONSpirit::Pair("rui", (uint64_t) rui));

	} else if (action == "add") {

		// Complain if the number is too large
		if (number > 14) {
			output.push_back(JSONSpirit::Pair("error", "Only 15 fiber inputs are allowed per DDU."));
			*out << JSONSpirit::write(output);
			return;
		}

		// Make sure the target fiber does not already exist
		std::vector<Fiber *> fiberVector = ddu->getFibers();
		for (std::vector<Fiber *>::iterator iFiber = fiberVector.begin(); iFiber != fiberVector.end(); ++iFiber) {
			if ((*iFiber)->getFiberNumber() == number) {
				output.push_back(JSONSpirit::Pair("error", "That fiber already exists in this DDU."));
				*out << JSONSpirit::write(output);
				return;
			}
		}

		// Make a fiber and add it
		ddu->addFiber(new Fiber(number));

		// Find the next fiber available
		std::set<unsigned int> fiberSet;
		fiberSet.insert(number);
		for (std::vector<Fiber *>::iterator iFiber = fiberVector.begin(); iFiber != fiberVector.end(); ++iFiber) {
			fiberSet.insert((*iFiber)->getFiberNumber());
		}

		// Find a new fiber (or die)
		unsigned int nextFiber = 0;
		while (1) {
			if (fiberSet.find(nextFiber) == fiberSet.end()) break;
			++nextFiber;
		}

		// Push everything back
		output.push_back(JSONSpirit::Pair("fiber", (uint64_t) number));
		output.push_back(JSONSpirit::Pair("value", (uint64_t) number));
		output.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));
		output.push_back(JSONSpirit::Pair("rui", (uint64_t) rui));
		output.push_back(JSONSpirit::Pair("nextFiber", (uint64_t) nextFiber));


	} else if (action == "delete") {

	} else if (action == "update_number") {

	} else if (action == "update_name") {

	} else if (action == "update_killed") {

	} else {
		output.push_back(JSONSpirit::Pair("error", "The action \"" + action + "\" is not recognized."));
		*out << JSONSpirit::write(output);
		return;
	}

	output.push_back(JSONSpirit::Pair("ok", true));
	*out << JSONSpirit::write(output);
/*
	// Get number
	unsigned int fiberNumber = 0;
	if (cgi.getElement("input_fiber_number") != cgi.getElements().end()) {
		fiberNumber = cgi["input_fiber_number"]->getIntegerValue();
	} else {
		output.push_back(JSONSpirit::Pair("error", "Cannot find fiber number in header"));
		*out << JSONSpirit::write(output);
		return;
	}

	// Get name
	std::string name = "";
	if (cgi.getElement("input_fiber_name") != cgi.getElements().end()) {
		name = cgi["input_fiber_name"]->getValue();
	} else {
		output.push_back(JSONSpirit::Pair("error", "Cannot find fiber name in header"));
		*out << JSONSpirit::write(output);
		return;
	}

	// Get killed
	bool killed = false;
	if (cgi.getElement("input_fiber_killed") != cgi.getElements().end()) {
		killed = cgi["input_fiber_killed"]->getIntegerValue();
	} else {
		output.push_back(JSONSpirit::Pair("error", "Cannot find killed checkbox in header"));
		*out << JSONSpirit::write(output);
		return;
	}

	std::string endcap = "?";
	unsigned int station = 0;
	unsigned int ring = 0;
	unsigned int number = 0;

	// Check normal station name first
	if (sscanf(name.c_str(), "%*c%1u/%1u/%02u", &station, &ring, &number) == 3) {
		endcap = "-";
		// CGICC does not understand that %2B is a plus-sign, so check for that here
	} else if (sscanf(name.c_str(), "%1u/%1u/%02u", &station, &ring, &number) == 3) {
		endcap = "+";
		// Else it's probably an SP, so check that
	} else if (sscanf(name.c_str(), "SP%02u", &number) == 1) {
		endcap = (number <= 6) ? "+" : "-";
	}

	myDDU->addFiber(new Fiber(fiberNumber, endcap, station, ring, number, killed));

	*out << JSONSpirit::write(output);
	* */
}



void emu::fed::ConfigurationEditor::webDCC(xgi::Input *in, xgi::Output *out)
{
	cgicc::Cgicc cgi(in);

	// Need some header information to be able to return JSON
	if (cgi.getElement("debug") == cgi.getElements().end() || cgi["debug"]->getIntegerValue() != 1) {
		cgicc::HTTPResponseHeader jsonHeader("HTTP/1.1", 200, "OK");
		jsonHeader.addHeader("Content-type", "application/json");
		out->setHTTPResponseHeader(jsonHeader);
	}

	// Make JSON output
	JSONSpirit::Object output;

	// Get the fake crate and return it
	if (cgi.getElement("fakeCrate") != cgi.getElements().end()) {
		output.push_back(JSONSpirit::Pair("fakeCrate", cgi["fakeCrate"]->getValue()));
	} else {
		output.push_back(JSONSpirit::Pair("error", "Cannot find fakeCrate in header"));
		*out << JSONSpirit::write(output);
		return;
	}

	// Get the crate
	unsigned int crateNumber = 0;

	if (cgi.getElement("crate") != cgi.getElements().end()) {
		crateNumber = cgi["crate"]->getIntegerValue();
		output.push_back(JSONSpirit::Pair("crate", (int) crateNumber));
	} else {
		output.push_back(JSONSpirit::Pair("error", "Cannot find crate number in header"));
		*out << JSONSpirit::write(output);
		return;
	}

	// Find the crate
	Crate *myCrate = NULL;

	for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
		if ((*iCrate)->getNumber() == crateNumber) {
			myCrate = (*iCrate);
			break;
		}
	}
	if (myCrate == NULL) {
		std::ostringstream error;
		error << "Unable to find crate matching number " << crateNumber;
		output.push_back(JSONSpirit::Pair("error", error.str()));
		*out << JSONSpirit::write(output);
		return;
	}

	// Get slot
	unsigned int slot = 0;
	if (cgi.getElement("input_dcc_slot") != cgi.getElements().end()) {
		slot = cgi["input_dcc_slot"]->getIntegerValue();
	} else {
		output.push_back(JSONSpirit::Pair("error", "Cannot find slot number in header"));
		*out << JSONSpirit::write(output);
		return;
	}

	DCC *myDCC = new DCC(slot, true);

	// Get FMMID
	if (cgi.getElement("input_dcc_fmm_id") != cgi.getElements().end()) {
		unsigned int fmmid = cgi["input_dcc_fmm_id"]->getIntegerValue();
		output.push_back(JSONSpirit::Pair("fmmid", (int) fmmid));
		myDCC->setFMMID(fmmid);
	} else {
		output.push_back(JSONSpirit::Pair("error", "Cannot find FMM ID number in header"));
		*out << JSONSpirit::write(output);
		return;
	}

	// Get the fake FMMID and return it
	if (cgi.getElement("fakeFMMID") != cgi.getElements().end()) {
		output.push_back(JSONSpirit::Pair("fakeFMMID", cgi["fakeFMMID"]->getValue()));
	} else {
		output.push_back(JSONSpirit::Pair("error", "Cannot find fakeFMMID in header"));
		*out << JSONSpirit::write(output);
		return;
	}

	// Get SLink1 ID
	if (cgi.getElement("input_dcc_slink1") != cgi.getElements().end()) {
		myDCC->setSLinkID(1, cgi["input_dcc_slink1"]->getIntegerValue());
	} else {
		output.push_back(JSONSpirit::Pair("error", "Cannot find SLink 1 ID number in header"));
		*out << JSONSpirit::write(output);
		return;
	}

	// Get SLink2 ID
	if (cgi.getElement("input_dcc_slink2") != cgi.getElements().end()) {
		myDCC->setSLinkID(2, cgi["input_dcc_slink2"]->getIntegerValue());
	} else {
		output.push_back(JSONSpirit::Pair("error", "Cannot find SLink 2 ID number in header"));
		*out << JSONSpirit::write(output);
		return;
	}

	// Get options
	uint16_t softsw = 0;
	if (cgi.getElement("input_dcc_sw_switch") != cgi.getElements().end()) {
		softsw |= (cgi["input_dcc_sw_switch"]->getIntegerValue() ? 0x200 : 0);
	} else {
		output.push_back(JSONSpirit::Pair("error", "Cannot find software switch checkbox in header"));
		*out << JSONSpirit::write(output);
		return;
	}
	if (cgi.getElement("input_dcc_ignore_ttc") != cgi.getElements().end()) {
		softsw |= (cgi["input_dcc_ignore_ttc"]->getIntegerValue() ? 0x1000 : 0);
	} else {
		output.push_back(JSONSpirit::Pair("error", "Cannot find ignore TTC checkbox in header"));
		*out << JSONSpirit::write(output);
		return;
	}
	if (cgi.getElement("input_dcc_ignore_backpressure") != cgi.getElements().end() && cgi.getElement("input_dcc_ignore_slink") != cgi.getElements().end()) {
		if (cgi["input_dcc_ignore_slink"]->getIntegerValue()) softsw |= 0x4000;
		else if (cgi["input_dcc_ignore_backpressure"]->getIntegerValue()) softsw |= 0x2000;
	} else {
		output.push_back(JSONSpirit::Pair("error", "Cannot find ignore slink/backpressure checkboxes in header"));
		*out << JSONSpirit::write(output);
		return;
	}
	if (cgi.getElement("input_dcc_sw4") != cgi.getElements().end()) {
		softsw |= (cgi["input_dcc_sw4"]->getIntegerValue() ? 0x10 : 0);
	} else {
		output.push_back(JSONSpirit::Pair("error", "Cannot find SW bit 4 checkbox in header"));
		*out << JSONSpirit::write(output);
		return;
	}
	if (cgi.getElement("input_dcc_sw5") != cgi.getElements().end()) {
		softsw |= (cgi["input_dcc_sw5"]->getIntegerValue() ? 0x20 : 0);
	} else {
		output.push_back(JSONSpirit::Pair("error", "Cannot find SW bit 5 checkbox in header"));
		*out << JSONSpirit::write(output);
		return;
	}

	myDCC->setSoftwareSwitch(softsw);

	myCrate->addBoard(myDCC);

	*out << JSONSpirit::write(output);
}



void emu::fed::ConfigurationEditor::webFIFO(xgi::Input *in, xgi::Output *out)
{
	cgicc::Cgicc cgi(in);

	// Need some header information to be able to return JSON
	if (cgi.getElement("debug") == cgi.getElements().end() || cgi["debug"]->getIntegerValue() != 1) {
		cgicc::HTTPResponseHeader jsonHeader("HTTP/1.1", 200, "OK");
		jsonHeader.addHeader("Content-type", "application/json");
		out->setHTTPResponseHeader(jsonHeader);
	}

	// Make JSON output
	JSONSpirit::Object output;

	// Get the crate
	unsigned int crateNumber = 0;

	if (cgi.getElement("crate") != cgi.getElements().end()) {
		crateNumber = cgi["crate"]->getIntegerValue();
	} else {
		output.push_back(JSONSpirit::Pair("error", "Cannot find crate number in header"));
		*out << JSONSpirit::write(output);
		return;
	}

	// Find the crate
	Crate *myCrate = NULL;

	for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); ++iCrate) {
		if ((*iCrate)->getNumber() == crateNumber) {
			myCrate = (*iCrate);
			break;
		}
	}
	if (myCrate == NULL) {
		std::ostringstream error;
		error << "Unable to find crate matching number " << crateNumber;
		output.push_back(JSONSpirit::Pair("error", error.str()));
		*out << JSONSpirit::write(output);
		return;
	}

	// Get FMMID
	unsigned int fmmid;
	if (cgi.getElement("fmmid") != cgi.getElements().end()) {
		fmmid = cgi["fmmid"]->getIntegerValue();
	} else {
		output.push_back(JSONSpirit::Pair("error", "Cannot find FMM ID number in header"));
		*out << JSONSpirit::write(output);
		return;
	}

	// Find the DCC
	DCC *myDCC = NULL;

	std::vector<DCC *> dccVector = myCrate->getDCCs();
	for (std::vector<DCC *>::iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); ++iDCC) {
		if ((*iDCC)->getFMMID() == fmmid) {
			myDCC = (*iDCC);
			break;
		}
	}
	if (myDCC == NULL) {
		std::ostringstream error;
		error << "Unable to find DCC matching FMM ID number " << fmmid;
		output.push_back(JSONSpirit::Pair("error", error.str()));
		*out << JSONSpirit::write(output);
		return;
	}

	// Get number
	unsigned int fifoNumber = 0;
	if (cgi.getElement("input_fifo_number") != cgi.getElements().end()) {
		fifoNumber = cgi["input_fifo_number"]->getIntegerValue();
	} else {
		output.push_back(JSONSpirit::Pair("error", "Cannot find FIFO number in header"));
		*out << JSONSpirit::write(output);
		return;
	}

	// Get RUI
	unsigned int rui = 0;
	if (cgi.getElement("input_fifo_rui") != cgi.getElements().end()) {
		rui = cgi["input_fifo_rui"]->getIntegerValue();
	} else {
		output.push_back(JSONSpirit::Pair("error", "Cannot find FIFO RUI in header"));
		*out << JSONSpirit::write(output);
		return;
	}

	// Get used
	bool used = false;
	if (cgi.getElement("input_fifo_used") != cgi.getElements().end()) {
		used = cgi["input_fifo_used"]->getIntegerValue();
	} else {
		output.push_back(JSONSpirit::Pair("error", "Cannot find used checkbox in header"));
		*out << JSONSpirit::write(output);
		return;
	}

	myDCC->addFIFO(new FIFO(fifoNumber, rui, used));

	*out << JSONSpirit::write(output);
}



void emu::fed::ConfigurationEditor::webUploadToDB(xgi::Input *in, xgi::Output *out)
{
	cgicc::Cgicc cgi(in);

	// Need some header information to be able to return JSON
	if (cgi.getElement("debug") == cgi.getElements().end() || cgi["debug"]->getIntegerValue() != 1) {
		cgicc::HTTPResponseHeader xmlHeader("HTTP/1.1", 200, "OK");
		xmlHeader.addHeader("Content-Type", "text/xml");
		std::ostringstream attachment;
		attachment << "attachment; filename=fed-system-" << toolbox::escape(systemName_ == "" ? "unnamed" : systemName_.toString()) << "-" << toolbox::TimeVal(timeStamp_).toString(toolbox::TimeVal::gmt) << ".xml";
		xmlHeader.addHeader("Content-Disposition", attachment.str());
		out->setHTTPResponseHeader(xmlHeader);
	}

	DBConfigurator configurator(this, dbUsername_, dbPassword_, dbKey_);

	JSONSpirit::Object output;

	try {
		configurator.uploadToDB(crateVector_, systemName_);

		// Need some header information to be able to return JSON
		if (cgi.getElement("debug") == cgi.getElements().end() || cgi["debug"]->getIntegerValue() != 1) {
			cgicc::HTTPResponseHeader jsonHeader("HTTP/1.1", 200, "OK");
			jsonHeader.addHeader("Content-type", "application/json");
			out->setHTTPResponseHeader(jsonHeader);
		}

		output.push_back(JSONSpirit::Pair("systemName", systemName_.toString()));
		output.push_back(JSONSpirit::Pair("key", dbKey_.toString()));

	} catch (emu::fed::exception::ConfigurationException &e) {

		output.push_back(JSONSpirit::Pair("error", e.what()));

	}

	*out << JSONSpirit::write(output);
}



int emu::fed::ConfigurationEditor::getIntegerValue(const std::string &value)
throw (emu::fed::exception::Exception)
{
	int returnMe = 0;

	if (value.substr(0,2) == "0x") {
		// Attempt conversion of hex
		std::istringstream inStream(value.substr(2));
		inStream.exceptions(std::istringstream::failbit);
		try {
			inStream >> std::hex >> returnMe;
		} catch (std::istringstream::failure &e) {
			std::ostringstream error;
			error << "Improper format for hex number " << value;
			XCEPT_RAISE(emu::fed::exception::Exception, error.str());
		}

	} else {
		// Attempt conversion of dec
		std::istringstream inStream(value);
		inStream.exceptions(std::istringstream::failbit);
		try {
			inStream >> returnMe;
		} catch (std::istringstream::failure &e) {
			std::ostringstream error;
			error << "Improper format for decimal number " << value;
			XCEPT_RAISE(emu::fed::exception::Exception, error.str());
		}
	}

	return returnMe;
}
