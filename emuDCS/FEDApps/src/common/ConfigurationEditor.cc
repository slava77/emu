/*****************************************************************************\
* $Id: ConfigurationEditor.cc,v 1.9 2009/11/23 12:21:20 paste Exp $
\*****************************************************************************/
#include "emu/fed/ConfigurationEditor.h"

#include <vector>
#include <sstream>
#include <fstream>

#include "xgi/Method.h"
#include "cgicc/HTMLClasses.h"
#include "xdaq2rc/RcmsStateNotifier.h"
#include "xdata/TimeVal.h"
#include "toolbox/TimeVal.h"
#include "emu/base/Alarm.h"
#include "emu/fed/JSONSpiritWriter.h"
#include "emu/fed/XMLConfigurator.h"
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
	
	*out << cgicc::fieldset() << std::endl;
	
	
	if (systemName_ != "") {
		*out << cgicc::div()
			.set("class", "titlebar default_width")
			.set("id", "FED_Configuration_Editor_titlebar") << std::endl;
		*out << cgicc::div("Configuration Editor")
			.set("class", "titletext") << std::endl;
		*out << cgicc::div() << std::endl;
		
		*out << cgicc::fieldset()
			.set("class", "dialog default_width")
			.set("id", "FED_Configuration_Editor_dialog") << std::endl;
		
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
			.set("id", "input_database_key")
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
		
		*out << cgicc::fieldset() << std::endl;
	}
	
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
	
	cgicc::const_file_iterator iFile = cgi.getFile("xmlFile");
	if (iFile == cgi.getFiles().end()) {
		// ERROR!
		LOG4CPLUS_ERROR(getApplicationLogger(), "Error uploading file");
		return;
	} else {
		std::ofstream tempFile("config_fed_upload.xml");
		if (tempFile.good()) {
			iFile->writeToStream(tempFile);
			tempFile.close();
			LOG4CPLUS_INFO(getApplicationLogger(), "Successfully uploaded file " << iFile->getName());
		} else {
			// ERROR!
			if (tempFile.is_open()) tempFile.close();
			LOG4CPLUS_ERROR(getApplicationLogger(), "Error opening local file for writing");
			return;
		}
	}
	
	// Parse the XML file and build crates properly
	XMLConfigurator configurator("config_fed_upload.xml");
	
	try {
		crateVector_ = configurator.setupCrates();
		systemName_ = configurator.getSystemName();
		timeStamp_ = configurator.getTimeStamp();
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Unable to create FED objects by parsing file config_fed_upload.xml: " << e.what();
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		return;
	}

}
