/*****************************************************************************\
* $Id: ConfigurationEditor.cc,v 1.17 2012/11/25 23:36:49 banicz Exp $
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
dbKey_(0),
tstoreInstance_(-1)
{

	// Variables that are to be made available to other applications
	xdata::InfoSpace *infoSpace = getApplicationInfoSpace();
	infoSpace->fireItemAvailable("dbUsername", &dbUsername_);
	infoSpace->fireItemAvailable("dbPassword", &dbPassword_);
	infoSpace->fireItemAvailable("tstoreInstance", &tstoreInstance_);

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
	xgi::bind(this, &emu::fed::ConfigurationEditor::webFindChamber, "FindChamber");
	xgi::bind(this, &emu::fed::ConfigurationEditor::webSummarize, "Summarize");

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
	*out << cgicc::button("Upload")
		.set("name", "xmlFileButton")
		.set("type", "submit")
		.set("id", "xml_file_button") << std::endl;
	*out << cgicc::form() << std::endl;
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

	*out << cgicc::button()
		.set("class", "right summarize action_button")
		.set("id", "summarize");
	*out << "Summarize configuration";
	*out << cgicc::button() << std::endl;

	*out << cgicc::fieldset() << std::endl;

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
	SystemDBAgent agent(this, tstoreInstance_);

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
		error << "Error loading keys from database";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DBException, e2, error.str(), e);
		LOG4CPLUS_ERROR(getApplicationLogger(), xcept::stdformat_exception_history(e2));
		notifyQualified("ERROR", e2);
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
		XCEPT_DECLARE(emu::fed::exception::DBException, e2, error.str());
		LOG4CPLUS_ERROR(getApplicationLogger(), xcept::stdformat_exception_history(e2));
		notifyQualified("ERROR", e2);

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
			XCEPT_DECLARE(emu::fed::exception::DBException, e2, error.str());
			LOG4CPLUS_ERROR(getApplicationLogger(), xcept::stdformat_exception_history(e2));
			notifyQualified("ERROR", e2);
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
		error << "Unable to create FED objects by parsing file " << ofile;
		XCEPT_DECLARE_NESTED(emu::fed::exception::DBException, e2, error.str(), e);
		LOG4CPLUS_ERROR(getApplicationLogger(), xcept::stdformat_exception_history(e2));
		notifyQualified("ERROR", e2);
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
	DBConfigurator configurator(this, dbUsername_.toString(), dbPassword_.toString(), dbKey_, tstoreInstance_);

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
			output.push_back(JSONSpirit::Pair("value", systemName_.toString()));
			*out << JSONSpirit::write(output);
			return;
		}

		if (name == "") {
			output.push_back(JSONSpirit::Pair("error", "The system name cannot be blank."));
			output.push_back(JSONSpirit::Pair("value", systemName_.toString()));
			*out << JSONSpirit::write(output);
			return;
		}

		systemName_ = name;
		output.push_back(JSONSpirit::Pair("value", name));

	} else if (action == "update_key") {

		// Get the database key
		uint64_t key;
		if (cgi.getElement("value") != cgi.getElements().end()) {
			try {
				key = getIntegerValue(cgi["value"]->getValue());
			} catch (emu::fed::exception::Exception &e) {
				output.push_back(JSONSpirit::Pair("error", "Unable to parse DB number.  " + std::string(e.what())));
				output.push_back(JSONSpirit::Pair("value", dbKey_.toString()));
				*out << JSONSpirit::write(output);
				return;
			}
		} else {
			output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
			output.push_back(JSONSpirit::Pair("value", dbKey_.toString()));
			*out << JSONSpirit::write(output);
			return;
		}

		// Make sure the key is not already in use.
		SystemDBAgent agent(this, tstoreInstance_);

		std::map<std::string, std::vector<std::pair<xdata::UnsignedInteger64, time_t> > > keyMap;

		try {
			agent.connect(dbUsername_, dbPassword_);

			keyMap = agent.getAllKeys();

		} catch (emu::fed::exception::DBException &e) {
			std::ostringstream error;
			error << "Error loading keys from database";
			XCEPT_DECLARE_NESTED(emu::fed::exception::DBException, e2, error.str(), e);
			LOG4CPLUS_ERROR(getApplicationLogger(), xcept::stdformat_exception_history(e2));
			notifyQualified("ERROR", e2);
			output.push_back(JSONSpirit::Pair("error", error.str()));
			output.push_back(JSONSpirit::Pair("value", dbKey_.toString()));
			*out << JSONSpirit::write(output);
			return;
		}

		for (std::map<std::string, std::vector<std::pair<xdata::UnsignedInteger64, time_t> > >::iterator iPair = keyMap.begin(); iPair != keyMap.end(); ++iPair) {

			std::vector<std::pair<xdata::UnsignedInteger64, time_t> > keys = iPair->second;
			for (std::vector<std::pair<xdata::UnsignedInteger64, time_t> >::iterator iKey = keys.begin(); iKey != keys.end(); ++iKey) {
				if (iKey->first == key) {
					output.push_back(JSONSpirit::Pair("error", "That key already has an entry in the database."));
					output.push_back(JSONSpirit::Pair("value", dbKey_.toString()));
					*out << JSONSpirit::write(output);
					return;
				}
			}

		}

		dbKey_ = key;
		output.push_back(JSONSpirit::Pair("value", dbKey_.toString()));

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
		std::vector<VMEModule *> boardVector = crate->getBoards<VMEModule>();
		for (std::vector<VMEModule *>::iterator iVMEModule = boardVector.begin(); iVMEModule != boardVector.end(); ++iVMEModule) {
			if ((*iVMEModule)->getSlot() == slot) {
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
				error << "There already exists a DDU matching RUI " << value;
				output.push_back(JSONSpirit::Pair("error", error.str()));
				*out << JSONSpirit::write(output);
				return;
			}
		}

		// Warn if this is a TrackFinder
		if (value == 0xc0) {
			output.push_back(JSONSpirit::Pair("warning", "This RUI corresponds to a Track-Finder DDU.  If this DDU is in a normal FED crate, this setting could cause the DDU to loose all VME communication!"));
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

		// Find the fiber
		std::vector<Fiber *> fiberVector = ddu->getFibers();
		Fiber *fiber = NULL;
		for (std::vector<Fiber *>::iterator iFiber = fiberVector.begin(); iFiber != fiberVector.end(); ++iFiber) {
			if ((*iFiber)->getFiberNumber() == number) {
				fiber = (*iFiber);
				break;
			}
		}
		if (fiber == NULL) {
			std::ostringstream error;
			error << "Unable to find fiber matching number " << number;
			output.push_back(JSONSpirit::Pair("error", error.str()));
			*out << JSONSpirit::write(output);
			return;
		}

		// Delete the fiber
		ddu->deleteFiber(fiber);

		// Find the next fiber available
		fiberVector = ddu->getFibers();
		std::set<unsigned int> fiberSet;
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

	} else if (action == "update_number") {

		// Find the fiber
		std::vector<Fiber *> fiberVector = ddu->getFibers();
		Fiber *fiber = NULL;
		for (std::vector<Fiber *>::iterator iFiber = fiberVector.begin(); iFiber != fiberVector.end(); ++iFiber) {
			if ((*iFiber)->getFiberNumber() == number) {
				fiber = (*iFiber);
				break;
			}
		}
		if (fiber == NULL) {
			std::ostringstream error;
			error << "Unable to find fiber matching number " << number;
			output.push_back(JSONSpirit::Pair("error", error.str()));
			*out << JSONSpirit::write(output);
			return;
		}

		// Get the new fiber number
		unsigned int value = 0;
		if (cgi.getElement("value") != cgi.getElements().end()) {
			try {
				value = getIntegerValue(cgi["value"]->getValue());
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

		// Complain if the number is too large
		if (value > 14) {
			output.push_back(JSONSpirit::Pair("error", "Only 15 fiber inputs are allowed per DDU."));
			*out << JSONSpirit::write(output);
			return;
		}

		// Check to see if the new fiber already exists
		for (std::vector<Fiber *>::iterator iFiber = fiberVector.begin(); iFiber != fiberVector.end(); ++iFiber) {
			if ((*iFiber)->getFiberNumber() == value) {
				output.push_back(JSONSpirit::Pair("error", "A fiber with that number already exists."));
				*out << JSONSpirit::write(output);
				return;
			}
		}

		// Update the fiber number
		fiber->setFiberNumber(value);

		// Find the next fiber available
		fiberVector = ddu->getFibers();
		std::set<unsigned int> fiberSet;
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
		output.push_back(JSONSpirit::Pair("fiber", (uint64_t) value));
		output.push_back(JSONSpirit::Pair("value", (uint64_t) value));
		output.push_back(JSONSpirit::Pair("previous", (uint64_t) number));
		output.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));
		output.push_back(JSONSpirit::Pair("rui", (uint64_t) rui));
		output.push_back(JSONSpirit::Pair("nextFiber", (uint64_t) nextFiber));

	} else if (action == "update_name") {

		// Find the fiber
		std::vector<Fiber *> fiberVector = ddu->getFibers();
		Fiber *fiber = NULL;
		for (std::vector<Fiber *>::iterator iFiber = fiberVector.begin(); iFiber != fiberVector.end(); ++iFiber) {
			if ((*iFiber)->getFiberNumber() == number) {
				fiber = (*iFiber);
				break;
			}
		}
		if (fiber == NULL) {
			std::ostringstream error;
			error << "Unable to find fiber matching number " << number;
			output.push_back(JSONSpirit::Pair("error", error.str()));
			*out << JSONSpirit::write(output);
			return;
		}

		// Get the new fiber name
		std::string value;
		if (cgi.getElement("value") != cgi.getElements().end()) {
			value = cgi["value"]->getValue();
		} else {
			output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
			*out << JSONSpirit::write(output);
			return;
		}

		// Set the name
		fiber->setName(value);

		// Get the name back
		value = fiber->getName();

		// Push everything back
		output.push_back(JSONSpirit::Pair("fiber", (uint64_t) number));
		output.push_back(JSONSpirit::Pair("value", value));
		output.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));
		output.push_back(JSONSpirit::Pair("rui", (uint64_t) rui));

	} else if (action == "update_killed") {

		// Find the fiber
		std::vector<Fiber *> fiberVector = ddu->getFibers();
		Fiber *fiber = NULL;
		for (std::vector<Fiber *>::iterator iFiber = fiberVector.begin(); iFiber != fiberVector.end(); ++iFiber) {
			if ((*iFiber)->getFiberNumber() == number) {
				fiber = (*iFiber);
				break;
			}
		}
		if (fiber == NULL) {
			std::ostringstream error;
			error << "Unable to find fiber matching number " << number;
			output.push_back(JSONSpirit::Pair("error", error.str()));
			*out << JSONSpirit::write(output);
			return;
		}

		// Get the new setting
		bool value = false;
		if (cgi.getElement("value") != cgi.getElements().end()) {
			value = cgi["value"]->getValue() == "on";
		}

		// Set the killed flag and reload in the DDU
		fiber->setKilled(value);
		ddu->reloadKillFiber();

		// Push everything back
		output.push_back(JSONSpirit::Pair("fiber", (uint64_t) number));
		output.push_back(JSONSpirit::Pair("value", value));
		output.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));
		output.push_back(JSONSpirit::Pair("rui", (uint64_t) rui));

	} else {
		output.push_back(JSONSpirit::Pair("error", "The action \"" + action + "\" is not recognized."));
		*out << JSONSpirit::write(output);
		return;
	}

	output.push_back(JSONSpirit::Pair("ok", true));
	*out << JSONSpirit::write(output);

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

	// Get fmmid
	unsigned int fmmid = 0;
	if (cgi.getElement("fmmid") != cgi.getElements().end()) {
		try {
			fmmid = getIntegerValue(cgi["fmmid"]->getValue());
		} catch (emu::fed::exception::Exception &e) {
			output.push_back(JSONSpirit::Pair("error", "Unable to parse FMM ID.  " + std::string(e.what())));
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

		// Find the DCC
		std::vector<DCC *> dccVector = crate->getDCCs();
		DCC *dcc = NULL;
		for (std::vector<DCC *>::iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); ++iDCC) {
			if ((*iDCC)->getFMMID() == fmmid) {
				dcc = (*iDCC);
				break;
			}
		}
		if (dcc == NULL) {
			std::ostringstream error;
			error << "Unable to find DCC matching FMMID " << fmmid;
			output.push_back(JSONSpirit::Pair("error", error.str()));
			*out << JSONSpirit::write(output);
			return;
		}

		// Return everything about the DCC
		JSONSpirit::Object dccObject;
		dccObject.push_back(JSONSpirit::Pair("slot", (uint64_t) dcc->getSlot()));
		dccObject.push_back(JSONSpirit::Pair("fmmid", (uint64_t) dcc->getFMMID()));
		dccObject.push_back(JSONSpirit::Pair("slink1", (uint64_t) dcc->getSLinkID(1)));
		dccObject.push_back(JSONSpirit::Pair("slink2", (uint64_t) dcc->getSLinkID(2)));
		// Options
		uint16_t sw = dcc->getSoftwareSwitch();
		dccObject.push_back(JSONSpirit::Pair("swswitch", (bool) (sw & 0x200)));
		dccObject.push_back(JSONSpirit::Pair("ignore_ttc", (bool) (sw & 0x1000)));
		dccObject.push_back(JSONSpirit::Pair("ignore_slink", (bool) (sw & 0x4000)));
		dccObject.push_back(JSONSpirit::Pair("ignore_backpressure", (bool) (sw & 0x2000) && !(sw & 0x4000)));
		dccObject.push_back(JSONSpirit::Pair("bit4", (bool) (sw & 0x10)));
		dccObject.push_back(JSONSpirit::Pair("bit5", (bool) (sw & 0x20)));

		// Get the fifo numbers as well
		std::vector<FIFO *> fifoVector = dcc->getFIFOs();
		std::set<unsigned int> fifoSet;
		JSONSpirit::Array fifoArray;
		for (std::vector<FIFO *>::iterator iFIFO = fifoVector.begin(); iFIFO != fifoVector.end(); ++iFIFO) {
			JSONSpirit::Object fifoObject;
			fifoObject.push_back(JSONSpirit::Pair("fifo", (uint64_t) (*iFIFO)->getNumber()));
			fifoSet.insert((*iFIFO)->getNumber());
			fifoArray.push_back(fifoObject);
		}

		// Add the fifos
		dccObject.push_back(JSONSpirit::Pair("fifos", fifoArray));

		// Find the next available fiber number
		unsigned int nextFIFO = 0;
		while (1) {
			if (fifoSet.find(nextFIFO) == fifoSet.end()) break;
			++nextFIFO;
		}

		output.push_back(JSONSpirit::Pair("nextFIFO", (uint64_t) nextFIFO));
		output.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));

		// Return the DCC
		output.push_back(JSONSpirit::Pair("dcc", dccObject));

	} else if (action == "add") {

		// Make sure that DCC doesn't already exist in this crate
		std::vector<DCC *> dccVector = crate->getDCCs();
		for (std::vector<DCC *>::iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); ++iDCC) {
			if ((*iDCC)->getFMMID() == fmmid) {
				output.push_back(JSONSpirit::Pair("error", "That FMMID already exists in this crate"));
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
		std::vector<VMEModule *> boardVector = crate->getBoards<VMEModule>();
		for (std::vector<VMEModule *>::iterator iBoard = boardVector.begin(); iBoard != boardVector.end(); ++iBoard) {
			if ((*iBoard)->getSlot() == slot) {
				output.push_back(JSONSpirit::Pair("error", "That slot is already occupied in this crate"));
				*out << JSONSpirit::write(output);
				return;
			}
		}

		// Make the new DCC
		DCC *dcc = new DCC(slot, true);
		dcc->setFMMID(fmmid);
		crate->addBoard(dcc);

		// Get the next available FMMID and slot
		std::set<unsigned int> fmmidSet;
		std::set<unsigned int> slotSet;
		for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); ++iCrate) {
			std::vector<DCC *> dccs = (*iCrate)->getDCCs();
			std::vector<VMEModule *> modules = (*iCrate)->getBoards<VMEModule>();
			for (std::vector<DCC *>::iterator iDCC = dccs.begin(); iDCC != dccs.end(); ++iDCC) {
				fmmidSet.insert((*iDCC)->getFMMID());
			}
			if ((*iCrate)->getNumber() == crateNumber) {
				for (std::vector<VMEModule *>::iterator iBoard = modules.begin(); iBoard != modules.end(); ++iBoard) {
					slotSet.insert((*iBoard)->getSlot());
				}
			}
		}

		// Find a new FMMID (or die)
		unsigned int nextFMMID = 0;
		while (1) {
			if (fmmidSet.find(nextFMMID) == fmmidSet.end()) break;
			++nextFMMID;
		}

		// Find a new slot (or die)
		unsigned int nextSlot = 0;
		while (1) {
			if (slotSet.find(nextSlot) == slotSet.end()) break;
			++nextSlot;
		}

		// Return everything we need to return
		output.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));
		output.push_back(JSONSpirit::Pair("value", (uint64_t) fmmid));
		output.push_back(JSONSpirit::Pair("nextFMMID", (uint64_t) nextFMMID));
		output.push_back(JSONSpirit::Pair("nextSlot", (uint64_t) nextSlot));

	} else if (action == "delete") {

		// Find the DCC
		std::vector<DCC *> dccVector = crate->getDCCs();
		DCC *dcc = NULL;
		for (std::vector<DCC *>::iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); ++iDCC) {
			if ((*iDCC)->getFMMID() == fmmid) {
				dcc = (*iDCC);
				break;
			}
		}
		if (dcc == NULL) {
			std::ostringstream error;
			error << "Unable to find DCC matching FMMID " << fmmid;
			output.push_back(JSONSpirit::Pair("error", error.str()));
			*out << JSONSpirit::write(output);
			return;
		}

		// Delete the board
		crate->deleteBoard(dcc);

		// Get the next available FMMID and slot
		std::set<unsigned int> fmmidSet;
		std::set<unsigned int> slotSet;
		for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); ++iCrate) {
			std::vector<DCC *> dccs = (*iCrate)->getDCCs();
			std::vector<VMEModule *> modules = (*iCrate)->getBoards<VMEModule>();
			for (std::vector<DCC *>::iterator iDCC = dccs.begin(); iDCC != dccs.end(); ++iDCC) {
				fmmidSet.insert((*iDCC)->getFMMID());
			}
			if ((*iCrate)->getNumber() == crateNumber) {
				for (std::vector<VMEModule *>::iterator iBoard = modules.begin(); iBoard != modules.end(); ++iBoard) {
					slotSet.insert((*iBoard)->getSlot());
				}
			}
		}

		// Find a new FMMID (or die)
		unsigned int nextFMMID = 0;
		while (1) {
			if (fmmidSet.find(nextFMMID) == fmmidSet.end()) break;
			++nextFMMID;
		}

		// Find a new slot (or die)
		unsigned int nextSlot = 0;
		while (1) {
			if (slotSet.find(nextSlot) == slotSet.end()) break;
			++nextSlot;
		}

		// Return everything we need to return
		output.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));
		output.push_back(JSONSpirit::Pair("value", (uint64_t) fmmid));
		output.push_back(JSONSpirit::Pair("nextFMMID", (uint64_t) nextFMMID));
		output.push_back(JSONSpirit::Pair("nextSlot", (uint64_t) nextSlot));

	} else if (action == "update_fmmid") {

		// Find the DCC
		std::vector<DCC *> dccVector = crate->getDCCs();
		DCC *dcc = NULL;
		for (std::vector<DCC *>::iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); ++iDCC) {
			if ((*iDCC)->getFMMID() == fmmid) {
				dcc = (*iDCC);
				break;
			}
		}
		if (dcc == NULL) {
			std::ostringstream error;
			error << "Unable to find DCC matching FMM ID " << fmmid;
			output.push_back(JSONSpirit::Pair("error", error.str()));
			*out << JSONSpirit::write(output);
			return;
		}

		// Get the new fmmid
		unsigned int value = 0;
		if (cgi.getElement("value") != cgi.getElements().end()) {
			try {
				value = getIntegerValue(cgi["value"]->getValue());
			} catch (emu::fed::exception::Exception &e) {
				output.push_back(JSONSpirit::Pair("error", "Unable to parse FMM ID.  " + std::string(e.what())));
				*out << JSONSpirit::write(output);
				return;
			}
		} else {
			output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
			*out << JSONSpirit::write(output);
			return;
		}

		// See if there is a DCC with the new FMMID already
		for (std::vector<DCC *>::iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); ++iDCC) {
			if ((*iDCC)->getFMMID() == value) {
				std::ostringstream error;
				error << "There already exists a DCC matching FMM ID " << value;
				output.push_back(JSONSpirit::Pair("error", error.str()));
				*out << JSONSpirit::write(output);
				return;
			}
		}

		// Update the board
		dcc->setFMMID(value);

		// Get the next available FMMID and slot
		std::set<unsigned int> fmmidSet;
		for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); ++iCrate) {
			std::vector<DCC *> dccs = (*iCrate)->getDCCs();
			std::vector<VMEModule *> modules = (*iCrate)->getBoards<VMEModule>();
			for (std::vector<DCC *>::iterator iDCC = dccs.begin(); iDCC != dccs.end(); ++iDCC) {
				fmmidSet.insert((*iDCC)->getFMMID());
			}
		}

		// Find a new FMMID (or die)
		unsigned int nextFMMID = 0;
		while (1) {
			if (fmmidSet.find(nextFMMID) == fmmidSet.end()) break;
			++nextFMMID;
		}

		// Return everything we need to return
		output.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));
		output.push_back(JSONSpirit::Pair("previous", (uint64_t) fmmid));
		output.push_back(JSONSpirit::Pair("value", (uint64_t) value));
		output.push_back(JSONSpirit::Pair("nextFMMID", (uint64_t) nextFMMID));

	} else if (action == "update_slot") {

		// Find the DCC
		std::vector<DCC *> dccVector = crate->getDCCs();
		DCC *dcc = NULL;
		for (std::vector<DCC *>::iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); ++iDCC) {
			if ((*iDCC)->getFMMID() == fmmid) {
				dcc = (*iDCC);
				break;
			}
		}
		if (dcc == NULL) {
			std::ostringstream error;
			error << "Unable to find DCC matching FMM ID " << fmmid;
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
				output.push_back(JSONSpirit::Pair("error", "Unable to parse slot.  " + std::string(e.what())));
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
		dcc->setSlot(value);

		// Warn if the slot is strange
		if (value == 2) {
			output.push_back(JSONSpirit::Pair("warning", "Slot 2 is usually reserved for the Track-Finder DDU."));
		} else if (value != 8 && value != 17) {
			output.push_back(JSONSpirit::Pair("warning", "Slot numbers 8 and 17 are usually reserved for DCCs."));
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
		output.push_back(JSONSpirit::Pair("fmmid", (uint64_t) fmmid));
		output.push_back(JSONSpirit::Pair("value", (uint64_t) value));
		output.push_back(JSONSpirit::Pair("nextSlot", (uint64_t) nextSlot));

	} else if (action.substr(0,12) == "update_slink") {

		// Find the DCC
		std::vector<DCC *> dccVector = crate->getDCCs();
		DCC *dcc = NULL;
		for (std::vector<DCC *>::iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); ++iDCC) {
			if ((*iDCC)->getFMMID() == fmmid) {
				dcc = (*iDCC);
				break;
			}
		}
		if (dcc == NULL) {
			std::ostringstream error;
			error << "Unable to find DCC matching FMM ID " << fmmid;
			output.push_back(JSONSpirit::Pair("error", error.str()));
			*out << JSONSpirit::write(output);
			return;
		}

		// Get the new ID
		unsigned int value = 0;
		if (cgi.getElement("value") != cgi.getElements().end()) {
			try {
				value = getIntegerValue(cgi["value"]->getValue());
			} catch (emu::fed::exception::Exception &e) {
				output.push_back(JSONSpirit::Pair("error", "Unable to parse SLink ID.  " + std::string(e.what())));
				*out << JSONSpirit::write(output);
				return;
			}
		} else {
			output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
			*out << JSONSpirit::write(output);
			return;
		}

		// See if there is a board with the new ID already
		for (std::vector<DCC *>::iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); ++iDCC) {
			if ((*iDCC)->getSLinkID(1) == value || (*iDCC)->getSLinkID(2) == value ) {
				std::ostringstream error;
				error << "There already exists a board using the SLink ID " << value;
				output.push_back(JSONSpirit::Pair("error", error.str()));
				*out << JSONSpirit::write(output);
				return;
			}
		}

		// Update the board
		if (action == "update_slink_1") {
			// warn if something is fishy
			if (value != dcc->getFMMID()) {
				output.push_back(JSONSpirit::Pair("warning", "Usually SLink ID 1 equals the FMM ID"));
			}
			dcc->setSLinkID(1, value);
		} else if (action == "update_slink_2") {
			// warn if something is fishy
			if (value != (unsigned int) dcc->getSLinkID(1) + 1) {
				output.push_back(JSONSpirit::Pair("warning", "Usually SLink ID 2 is one more than SLink ID 1"));
			}
			dcc->setSLinkID(2, value);
		}

		// Return everything we need to return
		output.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));
		output.push_back(JSONSpirit::Pair("fmmid", (uint64_t) fmmid));
		output.push_back(JSONSpirit::Pair("value", (uint64_t) value));

	} else if (action.substr(0,9) == "update_sw") {

		// Find the DCC
		std::vector<DCC *> dccVector = crate->getDCCs();
		DCC *dcc = NULL;
		for (std::vector<DCC *>::iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); ++iDCC) {
			if ((*iDCC)->getFMMID() == fmmid) {
				dcc = (*iDCC);
				break;
			}
		}
		if (dcc == NULL) {
			std::ostringstream error;
			error << "Unable to find DCC matching FMM ID " << fmmid;
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
		uint16_t sw = dcc->getSoftwareSwitch();
		if (action == "update_sw_switch") {
			if (value) sw |= 0x200;
			else sw &= 0xfdff;
		} else if (action == "update_sw_ttc") {
			if (value) sw |= 0x1000;
			else sw &= 0xefff;
		} else if (action == "update_sw_slink") {
			if (value) {
				sw &= 0xcfff;
				sw |= 0x4000;
			} else sw &= 0xbfff;
		} else if (action == "update_sw_backpressure") {
			if (value) {
				sw &= 0xbfff;
				sw |= 0x2000;
			} else sw &= 0xcfff;
		} else if (action == "update_sw_bit4") {
			if (value) sw |= 0x10;
			else sw &= 0xffef;
		} else if (action == "update_sw_bit5") {
			if (value) sw |= 0x20;
			else sw &= 0xffdf;
		}
		dcc->setSoftwareSwitch(sw);

		// Return everything we need to return
		output.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));
		output.push_back(JSONSpirit::Pair("fmmid", (uint64_t) fmmid));
		output.push_back(JSONSpirit::Pair("value", (bool) value));

	} else {
		output.push_back(JSONSpirit::Pair("error", "The action \"" + action + "\" is not recognized."));
		*out << JSONSpirit::write(output);
		return;
	}

	output.push_back(JSONSpirit::Pair("ok", true));
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

	// Get fmmid
	unsigned int fmmid = 0;
	if (cgi.getElement("fmmid") != cgi.getElements().end()) {
		try {
			fmmid = getIntegerValue(cgi["fmmid"]->getValue());
		} catch (emu::fed::exception::Exception &e) {
			output.push_back(JSONSpirit::Pair("error", "Unable to parse FMM ID.  " + std::string(e.what())));
			*out << JSONSpirit::write(output);
			return;
		}
	} else {
		output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
		*out << JSONSpirit::write(output);
		return;
	}

	// Find DCC
	std::vector<DCC *> dccVector = crate->getDCCs();
	DCC *dcc = NULL;
	for (std::vector<DCC *>::iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); iDCC++) {
		if ((*iDCC)->getFMMID() == fmmid) {
			dcc = (*iDCC);
			break;
		}
	}
	if (dcc == NULL) {
		std::ostringstream error;
		error << "Unable to find DCC matching number " << fmmid;
		output.push_back(JSONSpirit::Pair("error", error.str()));
		*out << JSONSpirit::write(output);
		return;
	}

	// Get FIFO number
	unsigned int number = 0;
	if (cgi.getElement("fifo") != cgi.getElements().end()) {
		try {
			number = getIntegerValue(cgi["fifo"]->getValue());
		} catch (emu::fed::exception::Exception &e) {
			output.push_back(JSONSpirit::Pair("error", "Unable to parse FIFO number.  " + std::string(e.what())));
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

		// Find the fifo
		std::vector<FIFO *> fifoVector = dcc->getFIFOs();
		FIFO *fifo = NULL;
		for (std::vector<FIFO *>::iterator iFIFO = fifoVector.begin(); iFIFO != fifoVector.end(); ++iFIFO) {
			if ((*iFIFO)->getNumber() == number) {
				fifo = (*iFIFO);
			}
		}
		if (fifo == NULL) {
			std::ostringstream error;
			error << "Unable to find FIFO matching number " << number;
			output.push_back(JSONSpirit::Pair("error", error.str()));
			*out << JSONSpirit::write(output);
			return;
		}

		// The FIFO itself is an object
		JSONSpirit::Object fifoObject;

		// Push back everything we need
		fifoObject.push_back(JSONSpirit::Pair("fifo", (uint64_t) number));
		fifoObject.push_back(JSONSpirit::Pair("rui", (uint64_t) fifo->getRUI()));
		fifoObject.push_back(JSONSpirit::Pair("use", fifo->isUsed()));

		output.push_back(JSONSpirit::Pair("fifo", fifoObject));
		output.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));
		output.push_back(JSONSpirit::Pair("fmmid", (uint64_t) fmmid));

	} else if (action == "add") {

		// Complain if the number is too large
		if (number > 9) {
			output.push_back(JSONSpirit::Pair("error", "Only 10 FIFO inputs are allowed per DCC."));
			*out << JSONSpirit::write(output);
			return;
		}

		// Make sure the target FIFO does not already exist
		std::vector<FIFO *> fifoVector = dcc->getFIFOs();
		for (std::vector<FIFO *>::iterator iFIFO = fifoVector.begin(); iFIFO != fifoVector.end(); ++iFIFO) {
			if ((*iFIFO)->getNumber() == number) {
				output.push_back(JSONSpirit::Pair("error", "That FIFO already exists in this DCC."));
				*out << JSONSpirit::write(output);
				return;
			}
		}

		// Make a FIFO and add it
		dcc->addFIFO(new FIFO(number));

		// Find the next FIFO available
		std::set<unsigned int> fifoSet;
		fifoSet.insert(number);
		for (std::vector<FIFO *>::iterator iFIFO = fifoVector.begin(); iFIFO != fifoVector.end(); ++iFIFO) {
			fifoSet.insert((*iFIFO)->getNumber());
		}

		// Find a new FIFO (or die)
		unsigned int nextFIFO = 0;
		while (1) {
			if (fifoSet.find(nextFIFO) == fifoSet.end()) break;
			++nextFIFO;
		}

		// Push everything back
		output.push_back(JSONSpirit::Pair("fifo", (uint64_t) number));
		output.push_back(JSONSpirit::Pair("value", (uint64_t) number));
		output.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));
		output.push_back(JSONSpirit::Pair("fmmid", (uint64_t) fmmid));
		output.push_back(JSONSpirit::Pair("nextFIFO", (uint64_t) nextFIFO));


	} else if (action == "delete") {

		// Find the fifo
		std::vector<FIFO *> fifoVector = dcc->getFIFOs();
		FIFO *fifo = NULL;
		for (std::vector<FIFO *>::iterator iFIFO = fifoVector.begin(); iFIFO != fifoVector.end(); ++iFIFO) {
			if ((*iFIFO)->getNumber() == number) {
				fifo = (*iFIFO);
				break;
			}
		}
		if (fifo == NULL) {
			std::ostringstream error;
			error << "Unable to find FIFO matching number " << number;
			output.push_back(JSONSpirit::Pair("error", error.str()));
			*out << JSONSpirit::write(output);
			return;
		}

		// Delete the fifo
		dcc->deleteFIFO(fifo);

		// Find the next FIFO available
		fifoVector = dcc->getFIFOs();
		std::set<unsigned int> fifoSet;
		for (std::vector<FIFO *>::iterator iFIFO = fifoVector.begin(); iFIFO != fifoVector.end(); ++iFIFO) {
			fifoSet.insert((*iFIFO)->getNumber());
		}

		// Find a new FIFO (or die)
		unsigned int nextFIFO = 0;
		while (1) {
			if (fifoSet.find(nextFIFO) == fifoSet.end()) break;
			++nextFIFO;
		}

		// Push everything back
		output.push_back(JSONSpirit::Pair("fifo", (uint64_t) number));
		output.push_back(JSONSpirit::Pair("value", (uint64_t) number));
		output.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));
		output.push_back(JSONSpirit::Pair("fmmid", (uint64_t) fmmid));
		output.push_back(JSONSpirit::Pair("nextFIFO", (uint64_t) nextFIFO));

	} else if (action == "update_number") {

		// Find the fifo
		std::vector<FIFO *> fifoVector = dcc->getFIFOs();
		FIFO *fifo = NULL;
		for (std::vector<FIFO *>::iterator iFIFO = fifoVector.begin(); iFIFO != fifoVector.end(); ++iFIFO) {
			if ((*iFIFO)->getNumber() == number) {
				fifo = (*iFIFO);
				break;
			}
		}
		if (fifo == NULL) {
			std::ostringstream error;
			error << "Unable to find FIFO matching number " << number;
			output.push_back(JSONSpirit::Pair("error", error.str()));
			*out << JSONSpirit::write(output);
			return;
		}

		// Get the new FIFO number
		unsigned int value = 0;
		if (cgi.getElement("value") != cgi.getElements().end()) {
			try {
				value = getIntegerValue(cgi["value"]->getValue());
			} catch (emu::fed::exception::Exception &e) {
				output.push_back(JSONSpirit::Pair("error", "Unable to parse FIFO number.  " + std::string(e.what())));
				*out << JSONSpirit::write(output);
				return;
			}
		} else {
			output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
			*out << JSONSpirit::write(output);
			return;
		}

		// Complain if the number is too large
		if (value > 9) {
			output.push_back(JSONSpirit::Pair("error", "Only 10 FIFO inputs are allowed per DCC."));
			*out << JSONSpirit::write(output);
			return;
		}

		// Check to see if the new FIFO already exists
		for (std::vector<FIFO *>::iterator iFIFO = fifoVector.begin(); iFIFO != fifoVector.end(); ++iFIFO) {
			if ((*iFIFO)->getNumber() == value) {
				output.push_back(JSONSpirit::Pair("error", "A FIFO with that number already exists."));
				*out << JSONSpirit::write(output);
				return;
			}
		}

		// Update the FIFO number
		fifo->setNumber(value);

		// Find the next FIFO available
		fifoVector = dcc->getFIFOs();
		std::set<unsigned int> fifoSet;
		for (std::vector<FIFO *>::iterator iFIFO = fifoVector.begin(); iFIFO != fifoVector.end(); ++iFIFO) {
			fifoSet.insert((*iFIFO)->getNumber());
		}

		// Find a new FIFO (or die)
		unsigned int nextFIFO = 0;
		while (1) {
			if (fifoSet.find(nextFIFO) == fifoSet.end()) break;
			++nextFIFO;
		}

		// Push everything back
		output.push_back(JSONSpirit::Pair("fifo", (uint64_t) value));
		output.push_back(JSONSpirit::Pair("value", (uint64_t) value));
		output.push_back(JSONSpirit::Pair("previous", (uint64_t) number));
		output.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));
		output.push_back(JSONSpirit::Pair("fmmid", (uint64_t) fmmid));
		output.push_back(JSONSpirit::Pair("nextFIFO", (uint64_t) nextFIFO));

	} else if (action == "update_rui") {

		// Find the fifo
		std::vector<FIFO *> fifoVector = dcc->getFIFOs();
		FIFO *fifo = NULL;
		for (std::vector<FIFO *>::iterator iFIFO = fifoVector.begin(); iFIFO != fifoVector.end(); ++iFIFO) {
			if ((*iFIFO)->getNumber() == number) {
				fifo = (*iFIFO);
				break;
			}
		}
		if (fifo == NULL) {
			std::ostringstream error;
			error << "Unable to find FIFO matching number " << number;
			output.push_back(JSONSpirit::Pair("error", error.str()));
			*out << JSONSpirit::write(output);
			return;
		}

		// Get the new FIFO RUI
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

		// Set the RUI
		fifo->setRUI(value);

		// Warn if the RUI is not found in the crate
		bool found = false;
		std::vector<DDU *> dduVector = crate->getDDUs();
		for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); ++iDDU) {
			if (value == (*iDDU)->getRUI()) {
				found = true;
				break;
			}
		}
		if (!found) {
			output.push_back(JSONSpirit::Pair("warning", "The specified RUI does not match any DDUs in this crate."));
		}

		// Push everything back
		output.push_back(JSONSpirit::Pair("fifo", (uint64_t) number));
		output.push_back(JSONSpirit::Pair("value", (uint64_t) value));
		output.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));
		output.push_back(JSONSpirit::Pair("fmmid", (uint64_t) fmmid));

	} else if (action == "update_use") {

		// Find the fifo
		std::vector<FIFO *> fifoVector = dcc->getFIFOs();
		FIFO *fifo = NULL;
		for (std::vector<FIFO *>::iterator iFIFO = fifoVector.begin(); iFIFO != fifoVector.end(); ++iFIFO) {
			if ((*iFIFO)->getNumber() == number) {
				fifo = (*iFIFO);
				break;
			}
		}
		if (fifo == NULL) {
			std::ostringstream error;
			error << "Unable to find FIFO matching number " << number;
			output.push_back(JSONSpirit::Pair("error", error.str()));
			*out << JSONSpirit::write(output);
			return;
		}

		// Get the new setting
		bool value = false;
		if (cgi.getElement("value") != cgi.getElements().end()) {
			value = cgi["value"]->getValue() == "on";
		}

		// Set the killed flag and reload in the DCC
		fifo->setUsed(value);
		dcc->reloadFIFOInUse();

		// Push everything back
		output.push_back(JSONSpirit::Pair("fifo", (uint64_t) number));
		output.push_back(JSONSpirit::Pair("value", value));
		output.push_back(JSONSpirit::Pair("crate", (uint64_t) crateNumber));
		output.push_back(JSONSpirit::Pair("fmmid", (uint64_t) fmmid));

	} else {
		output.push_back(JSONSpirit::Pair("error", "The action \"" + action + "\" is not recognized."));
		*out << JSONSpirit::write(output);
		return;
	}

	output.push_back(JSONSpirit::Pair("ok", true));
	*out << JSONSpirit::write(output);

}



void emu::fed::ConfigurationEditor::webUploadToDB(xgi::Input *in, xgi::Output *out)
{
	cgicc::Cgicc cgi(in);

	// Need some header information to be able to return JSON
	if (cgi.getElement("debug") == cgi.getElements().end() || cgi["debug"]->getIntegerValue() != 1) {
		cgicc::HTTPResponseHeader jsonHeader("HTTP/1.1", 200, "OK");
		jsonHeader.addHeader("Content-type", "application/json");
		out->setHTTPResponseHeader(jsonHeader);
	}

	DBConfigurator configurator(this, dbUsername_, dbPassword_, dbKey_, tstoreInstance_);

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



void emu::fed::ConfigurationEditor::webFindChamber(xgi::Input *in, xgi::Output *out)
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

	// Get the needle.  I already have a haystack
	std::string needle;
	if (cgi.getElement("needle") != cgi.getElements().end()) {
		needle = cgi["needle"]->getValue();
	} else {
		output.push_back(JSONSpirit::Pair("error", "A network error occurred.  Please try again."));
		*out << JSONSpirit::write(output);
		return;
	}

	// This is required to get around the fact that cgicc doesn't understand '+'.  Seriously?
	if (cgi.getElement("plusPos") != cgi.getElements().end()) {
		unsigned int plusPos = cgi["plusPos"]->getIntegerValue();
		needle.insert(plusPos, "+");
	}

	// Search!  Use simple substring matching, and return the first result
	for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); ++iCrate) {

		std::vector<DDU *> dduVector = (*iCrate)->getDDUs();
		for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); ++iDDU) {

			std::vector<Fiber *> fiberVector = (*iDDU)->getFibers();
			for (std::vector<Fiber *>::iterator iFiber = fiberVector.begin(); iFiber != fiberVector.end(); ++iFiber) {

				if ((*iFiber)->getName().find(needle) != std::string::npos) {

					// The fiber itself is an object
					JSONSpirit::Object fiberObject;

					// Push back everything we need
					fiberObject.push_back(JSONSpirit::Pair("fiber", (uint64_t) (*iFiber)->getFiberNumber()));
					fiberObject.push_back(JSONSpirit::Pair("name", (*iFiber)->getName()));
					fiberObject.push_back(JSONSpirit::Pair("killed", (*iFiber)->isKilled()));

					output.push_back(JSONSpirit::Pair("fiber", fiberObject));
					output.push_back(JSONSpirit::Pair("crate", (uint64_t) (*iCrate)->getNumber()));
					output.push_back(JSONSpirit::Pair("rui", (uint64_t) (*iDDU)->getRUI()));

					*out << JSONSpirit::write(output);
					return;

				}

			}
		}
	}

	// Nothing found
	output.push_back(JSONSpirit::Pair("error", "No fibers match that string."));
	*out << JSONSpirit::write(output);
	return;
}



void emu::fed::ConfigurationEditor::webSummarize(xgi::Input *in, xgi::Output *out)
{
	// Non-standard header
	*out << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">" << std::endl;
	*out << "<html xmlns=\"http://www.w3.org/1999/xhtml\">" << std::endl;
	*out << cgicc::head() << std::endl;
	*out << "<link rel=\"stylesheet\" type=\"text/css\" href=\"/emu/emuDCS/FEDApps/html/FEDApps.css\" />" << std::endl;

	*out << cgicc::head() << std::endl;
	*out << cgicc::body() << std::endl;

	// Output system/key information
	*out << cgicc::table()
		.set("class", "data_table") << std::endl;
	*out << cgicc::tr()
		.set("class", "data_header") << std::endl;
	*out << cgicc::td("System Information")
		.set("colspan", "2") << std::endl;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::tr()
		.set("class", "data_entry") << std::endl;
	*out << cgicc::td("System Name")
		.set("class", "entry_name") << std::endl;
	*out << cgicc::td(systemName_.toString())
		.set("class", "entry_value") << std::endl;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::tr()
		.set("class", "data_entry") << std::endl;
	*out << cgicc::td("DB Key")
		.set("class", "entry_name") << std::endl;
	*out << cgicc::td(dbKey_.toString())
		.set("class", "entry_value") << std::endl;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::tr()
		.set("class", "data_entry") << std::endl;
	*out << cgicc::td("Time Stamp")
		.set("class", "entry_name") << std::endl;
	*out << cgicc::td(toolbox::TimeVal(timeStamp_).toString(toolbox::TimeVal::gmt))
		.set("class", "entry_value") << std::endl;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::table() << std::endl;

	// Output hardware information
	*out << cgicc::table()
		.set("class", "data_table") << std::endl;
	*out << cgicc::tr()
		.set("class", "data_header") << std::endl;
	*out << cgicc::td("Hardware Information")
		.set("colspan", "6") << std::endl;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::tr()
		.set("class", "data_header") << std::endl;
	*out << cgicc::td("Crate Number") << std::endl;
	*out << cgicc::td("Slot") << std::endl;
	*out << cgicc::td("Board Type") << std::endl;
	*out << cgicc::td("Identifier") << std::endl;
	*out << cgicc::td("TF DDU") << std::endl;
	*out << cgicc::td("Global") << std::endl;
	*out << cgicc::tr() << std::endl;

	for (std::vector<Crate *>::const_iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); ++iCrate) {

		unsigned int crateNumber = (*iCrate)->getNumber();

		std::vector<DDU *> dduVector = (*iCrate)->getDDUs();
		for (std::vector<DDU *>::const_iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); ++iDDU) {

			unsigned int slot = (*iDDU)->getSlot();
			std::string boardType = "DDU";
			std::ostringstream identifier;
			identifier << "RUI " << (*iDDU)->getRUI();
			bool trackFinder = ((*iDDU)->getRUI() == 0xc0);
			bool global = ((*iDDU)->getGbEPrescale() != 0x8);

			*out << cgicc::tr()
				.set("class", "data_entry") << std::endl;
			*out << cgicc::td()
				.set("class", "entry_value") << crateNumber << cgicc::td() << std::endl;
			*out << cgicc::td()
				.set("class", "entry_value") << slot << cgicc::td() << std::endl;
			*out << cgicc::td()
				.set("class", "entry_value") << boardType << cgicc::td() << std::endl;
			*out << cgicc::td()
				.set("class", "entry_value") << identifier.str() << cgicc::td() << std::endl;
			*out << cgicc::td()
				.set("class", "entry_value") << (trackFinder ? "true" : "false") << cgicc::td() << std::endl;
			*out << cgicc::td()
				.set("class", "entry_value") << (global ? "true" : "false") << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;

		}

		std::vector<DCC *> dccVector = (*iCrate)->getDCCs();
		for (std::vector<DCC *>::const_iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); ++iDCC) {

			unsigned int slot = (*iDCC)->getSlot();
			std::string boardType = "DCC";
			std::ostringstream identifier;
			bool global = !((*iDCC)->getSoftwareSwitch() & 0x4000);
			identifier << "FMM ID " << (*iDCC)->getFMMID();

			*out << cgicc::tr()
				.set("class", "data_entry") << std::endl;
			*out << cgicc::td()
				.set("class", "entry_value") << crateNumber << cgicc::td() << std::endl;
			*out << cgicc::td()
				.set("class", "entry_value") << slot << cgicc::td() << std::endl;
			*out << cgicc::td()
				.set("class", "entry_value") << boardType << cgicc::td() << std::endl;
			*out << cgicc::td()
				.set("class", "entry_value") << identifier.str() << cgicc::td() << std::endl;
			*out << cgicc::td()
				.set("class", "entry_value") << cgicc::td() << std::endl;
			*out << cgicc::td()
				.set("class", "entry_value") << (global ? "true" : "false") << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;

		}

	}

	*out << cgicc::table() << std::endl;

	// Output killed fibers
	*out << cgicc::table()
		.set("class", "data_table") << std::endl;
	*out << cgicc::tr()
		.set("class", "data_header") << std::endl;
	*out << cgicc::td("Killed Fibers")
		.set("colspan", "5") << std::endl;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::tr()
		.set("class", "data_header") << std::endl;
	*out << cgicc::td("Crate Number") << std::endl;
	*out << cgicc::td("Slot") << std::endl;
	*out << cgicc::td("RUI") << std::endl;
	*out << cgicc::td("Fiber") << std::endl;
	*out << cgicc::td("Chamber/SP") << std::endl;
	*out << cgicc::tr() << std::endl;

	for (std::vector<Crate *>::const_iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); ++iCrate) {

		unsigned int crateNumber = (*iCrate)->getNumber();

		std::vector<DDU *> dduVector = (*iCrate)->getDDUs();
		for (std::vector<DDU *>::const_iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); ++iDDU) {

			unsigned int slot = (*iDDU)->getSlot();
			unsigned int rui = (*iDDU)->getRUI();

			std::vector<Fiber *> fiberVector = (*iDDU)->getFibers();
			for (std::vector<Fiber *>::const_iterator iFiber = fiberVector.begin(); iFiber != fiberVector.end(); ++iFiber) {

				if ((*iFiber)->isKilled()) {

					unsigned int fiber = (*iFiber)->getFiberNumber();
					std::string name = (*iFiber)->getName();

					*out << cgicc::tr()
						.set("class", "data_entry") << std::endl;
					*out << cgicc::td()
						.set("class", "entry_value") << crateNumber << cgicc::td() << std::endl;
					*out << cgicc::td()
						.set("class", "entry_value") << slot << cgicc::td() << std::endl;
					*out << cgicc::td()
						.set("class", "entry_value") << rui << cgicc::td() << std::endl;
					*out << cgicc::td()
						.set("class", "entry_value") << fiber << cgicc::td() << std::endl;
					*out << cgicc::td()
						.set("class", "entry_value") << name << cgicc::td() << std::endl;
					*out << cgicc::tr() << std::endl;

				}

			}

		}

	}

	*out << cgicc::table() << std::endl;

	// Output unused fifos
	*out << cgicc::table()
		.set("class", "data_table") << std::endl;
	*out << cgicc::tr()
		.set("class", "data_header") << std::endl;
	*out << cgicc::td("Unused FIFOs")
		.set("colspan", "5") << std::endl;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::tr()
		.set("class", "data_header") << std::endl;
	*out << cgicc::td("Crate Number") << std::endl;
	*out << cgicc::td("Slot") << std::endl;
	*out << cgicc::td("FMM ID") << std::endl;
	*out << cgicc::td("FIFO") << std::endl;
	*out << cgicc::td("RUI") << std::endl;
	*out << cgicc::tr() << std::endl;

	for (std::vector<Crate *>::const_iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); ++iCrate) {

		unsigned int crateNumber = (*iCrate)->getNumber();

		std::vector<DCC *> dccVector = (*iCrate)->getDCCs();
		for (std::vector<DCC *>::const_iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); ++iDCC) {

			unsigned int slot = (*iDCC)->getSlot();
			unsigned int fmmid = (*iDCC)->getFMMID();

			std::vector<FIFO *> fifoVector = (*iDCC)->getFIFOs();
			for (std::vector<FIFO *>::const_iterator iFIFO = fifoVector.begin(); iFIFO != fifoVector.end(); ++iFIFO) {

				if (!(*iFIFO)->isUsed()) {

					unsigned int fifo = (*iFIFO)->getNumber();
					unsigned int rui = (*iFIFO)->getRUI();

					*out << cgicc::tr()
						.set("class", "data_entry") << std::endl;
					*out << cgicc::td()
						.set("class", "entry_value") << crateNumber << cgicc::td() << std::endl;
					*out << cgicc::td()
						.set("class", "entry_value") << slot << cgicc::td() << std::endl;
					*out << cgicc::td()
						.set("class", "entry_value") << fmmid << cgicc::td() << std::endl;
					*out << cgicc::td()
						.set("class", "entry_value") << fifo << cgicc::td() << std::endl;
					*out << cgicc::td()
						.set("class", "entry_value") << rui << cgicc::td() << std::endl;
					*out << cgicc::tr() << std::endl;

				}

			}

		}

	}

	*out << cgicc::table() << std::endl;

	*out << cgicc::body() << std::endl;
	*out << "</html>" << std::endl;
}
