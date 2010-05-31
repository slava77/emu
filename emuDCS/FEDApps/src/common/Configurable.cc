/*****************************************************************************\
* $Id: Configurable.cc,v 1.13 2010/05/31 14:57:20 paste Exp $
\*****************************************************************************/
#include "emu/fed/Configurable.h"
#include "boost/filesystem/operations.hpp"
#include "boost/algorithm/string/case_conv.hpp"

#include "xgi/Method.h"
#include "cgicc/HTMLClasses.h"
#include "xdata/Table.h"
#include "emu/fed/Crate.h"
#include "emu/fed/AutoConfigurator.h"
#include "emu/fed/DBConfigurator.h"
#include "emu/fed/XMLConfigurator.h"
#include "emu/fed/JSONSpiritWriter.h"
#include "emu/fed/SystemDBAgent.h"
#include "xcept/tools.h"

emu::fed::Configurable::Configurable(xdaq::ApplicationStub *stub):
xdaq::WebApplication(stub),
emu::fed::Application(stub),
systemName_("unnamed"),
tstoreInstance_(-1)
{
	// Variables that are to be made available to other applications
	getApplicationInfoSpace()->fireItemAvailable("systemName", &systemName_);
	getApplicationInfoSpace()->fireItemAvailable("xmlFileName", &xmlFile_);
	getApplicationInfoSpace()->fireItemAvailable("dbUsername", &dbUsername_);
	getApplicationInfoSpace()->fireItemAvailable("dbPassword", &dbPassword_);
	getApplicationInfoSpace()->fireItemAvailable("dbKey", &dbKey_);
	getApplicationInfoSpace()->fireItemAvailable("configMode",  &configMode_);
	getApplicationInfoSpace()->fireItemAvailable("tstoreInstance",  &tstoreInstance_);
	
	// HyperDAQ pages
	xgi::bind(this, &emu::fed::Configurable::webGetConfiguration, "GetConfiguration");
	xgi::bind(this, &emu::fed::Configurable::webChangeConfigMode, "ChangeConfigMode");
	xgi::bind(this, &emu::fed::Configurable::webChangeXMLFile, "ChangeXMLFile");
	xgi::bind(this, &emu::fed::Configurable::webChangeDBKey, "ChangeDBKey");
	xgi::bind(this, &emu::fed::Configurable::webReconfigure, "Reconfigure");
}



emu::fed::Configurable::~Configurable()
{
	for (size_t iCrate = 0; iCrate < crateVector_.size(); iCrate++) {
		delete crateVector_[iCrate];
	}
}



void emu::fed::Configurable::webGetConfiguration(xgi::Input *in, xgi::Output *out)
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
	
	// Configuration variables
	output.push_back(JSONSpirit::Pair("configMode", configMode_.toString()));
	output.push_back(JSONSpirit::Pair("xmlFile", xmlFile_.toString()));
	output.push_back(JSONSpirit::Pair("dbKey", dbKey_.toString()));
	output.push_back(JSONSpirit::Pair("systemName", systemName_.toString()));
	
	// And now return everything as JSON
	*out << JSONSpirit::write(output);
}



void emu::fed::Configurable::webChangeConfigMode(xgi::Input *in, xgi::Output *out)
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
	
	xdata::String oldConfigMode = configMode_;
	if (cgi.getElement("configMode") != cgi.getElements().end()) {
		configMode_ = cgi["configMode"]->getValue();
		if (configMode_ != "XML" && configMode_ != "Database" && configMode_ != "Autodetect") {
			std::ostringstream error;
			error << "Configuration mode " << configMode_.toString() << " doesn't make sense, falling back to " << oldConfigMode.toString();
			XCEPT_DECLARE(emu::fed::exception::ConfigurationException, e2, error.str());
			LOG4CPLUS_ERROR(getApplicationLogger(), xcept::stdformat_exception_history(e2));
			notifyQualified("ERROR", e2);
			configMode_ = oldConfigMode;
			output.push_back(JSONSpirit::Pair("exception", error.str()));
		} else {
			LOG4CPLUS_DEBUG(getApplicationLogger(), "Configuration Mode changed to " << configMode_.toString());
		}
	}
	
	output.push_back(JSONSpirit::Pair("configMode", configMode_.toString()));
	*out << JSONSpirit::write(output);
}



void emu::fed::Configurable::webChangeXMLFile(xgi::Input *in, xgi::Output *out)
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
	
	if (cgi.getElement("xmlFile") != cgi.getElements().end()) {
		xdata::String oldXMLFile = xmlFile_;
		xmlFile_ = cgi["xmlFile"]->getValue();
		// Make sure the file exists and, if not, return to the previous value
		if (!boost::filesystem::exists(xmlFile_.toString())) {
			std::ostringstream error;
			error << "Configuration XML file " << xmlFile_.toString() << " doesn't exist, falling back to " << oldXMLFile.toString();
			XCEPT_DECLARE(emu::fed::exception::FileException, e2, error.str());
			LOG4CPLUS_ERROR(getApplicationLogger(), xcept::stdformat_exception_history(e2));
			notifyQualified("ERROR", e2);
			xmlFile_ = oldXMLFile;
			output.push_back(JSONSpirit::Pair("exception", error.str()));
		} else {
			LOG4CPLUS_DEBUG(getApplicationLogger(), "Configuration XML file changed to " << xmlFile_.toString());
		}
	}
	
	output.push_back(JSONSpirit::Pair("xmlFile", xmlFile_.toString()));
	*out << JSONSpirit::write(output);
}



void emu::fed::Configurable::webChangeDBKey(xgi::Input *in, xgi::Output *out)
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
	
	if (cgi.getElement("dbKey") != cgi.getElements().end()) {
		xdata::UnsignedInteger64 oldDBKey = dbKey_;
		// Make sure the file exists and, if not, return to the previous value
		try {
			dbKey_.fromString(cgi["dbKey"]->getValue());
			LOG4CPLUS_DEBUG(getApplicationLogger(), "Configuration XML file changed to " << xmlFile_.toString());
		} catch (xdata::exception::Exception &e) {
			std::ostringstream error;
			error << "DB key " << cgi["dbKey"]->getValue() << " doesn't make sense, falling back to " << oldDBKey.toString();
			XCEPT_DECLARE(emu::fed::exception::ConfigurationException, e2, error.str());
			LOG4CPLUS_ERROR(getApplicationLogger(), xcept::stdformat_exception_history(e2));
			notifyQualified("ERROR", e2);
			dbKey_ = oldDBKey;
			output.push_back(JSONSpirit::Pair("exception", error.str()));
		}
	}
	
	output.push_back(JSONSpirit::Pair("xmlFile", xmlFile_.toString()));
	*out << JSONSpirit::write(output);
}



void emu::fed::Configurable::webReconfigure(xgi::Input *in, xgi::Output *out)
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
	
	try {
		softwareConfigure();
	} catch (emu::fed::exception::ConfigurationException &e) {
		std::ostringstream error;
		error << "Error attempting to reconfigure";
		XCEPT_DECLARE_NESTED(emu::fed::exception::ConfigurationException, e2, error.str(), e);
		LOG4CPLUS_ERROR(getApplicationLogger(), xcept::stdformat_exception_history(e2));
		notifyQualified("ERROR", e2);
		output.push_back(JSONSpirit::Pair("exception", error.str()));
	}
	
	*out << JSONSpirit::write(output);
	
}



void emu::fed::Configurable::softwareConfigure()
throw (emu::fed::exception::ConfigurationException)
{

	for (size_t iCrate = 0; iCrate < crateVector_.size(); iCrate++) {
		delete crateVector_[iCrate];
	}
	
	crateVector_.clear();
	
	if (configMode_ == "Autodetect") {
		
		LOG4CPLUS_INFO(getApplicationLogger(), "Autodetect configuration");
		AutoConfigurator configurator;
		
		try {
			crateVector_ = configurator.setupCrates();
			systemName_ = configurator.getSystemName();
		} catch (emu::fed::exception::Exception &e) {
			std::ostringstream error;
			error << "Unable to autodetect FED objects: " << e.what();
			LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
			XCEPT_RETHROW(emu::fed::exception::ConfigurationException, error.str(), e);
		}
		
	} else if (configMode_ == "XML") {
		
		// PGK Easier parsing.  Less confusing.
		LOG4CPLUS_INFO(getApplicationLogger(), "XML configuration using file " << xmlFile_.toString());
		XMLConfigurator configurator(xmlFile_.toString());
		
		try {
			crateVector_ = configurator.setupCrates();
			systemName_ = configurator.getSystemName();
		} catch (emu::fed::exception::Exception &e) {
			std::ostringstream error;
			error << "Unable to create FED objects by parsing file " << xmlFile_.toString() << ": " << e.what();
			LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
			XCEPT_RETHROW(emu::fed::exception::ConfigurationException, error.str(), e);
		}
		
	} else if (configMode_ == "Database") {
		
		LOG4CPLUS_INFO(getApplicationLogger(), "DB configuration using key " << dbKey_.toString());
		DBConfigurator configurator(this, dbUsername_.toString(), dbPassword_.toString(), dbKey_, tstoreInstance_);
		
		try {
			crateVector_ = configurator.setupCrates();
			systemName_ = configurator.getSystemName();
		} catch (emu::fed::exception::Exception &e) {
			std::ostringstream error;
			error << "Unable to create FED objects using the online database: " << e.what();
			LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
			XCEPT_RETHROW(emu::fed::exception::ConfigurationException, error.str(), e);
		}
		
	} else {
		std::ostringstream error;
		error << "configMode_ \"" << configMode_.toString() << "\" not understood";
		LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
		XCEPT_RAISE(emu::fed::exception::ConfigurationException, error.str());
	}
	
}



std::string emu::fed::Configurable::printConfigureOptions()
{
	std::ostringstream out;
	out << cgicc::div()
		.set("class", "category tier1") << std::endl;
	cgicc::input dbRadio;
	dbRadio.set("type", "radio")
		.set("class", "config_type")
		.set("name", "config_type")
		.set("id", "config_type_database")
		.set("value", "Database");
	if (configMode_ == "Database") dbRadio.set("checked", "checked");
	out << dbRadio << std::endl;
	out << cgicc::label("Database")
		.set("for", "config_type_database") << std::endl;
	out << cgicc::div() << std::endl;
	
	try {
		
		std::map<std::string, std::vector<xdata::UnsignedInteger64> > keys = getDBKeys();
		
		out << cgicc::table()
			.set("class", "noborder dialog tier2") << std::endl;
		out << cgicc::tr() << std::endl;
		out << cgicc::td("System Name: ") << std::endl;
		out << cgicc::td() << std::endl;
		cgicc::select nameSelect;
		nameSelect.set("id", "system_name_select")
			.set("name", "system_name_select")
			.set("class", "system_name_select");
		if (configMode_ != "Database") nameSelect.set("disabled", "true");
		out << nameSelect << std::endl;
		
		for (std::map<std::string, std::vector<xdata::UnsignedInteger64> >::const_iterator iPair = keys.begin(); iPair != keys.end(); ++iPair) {
			cgicc::option nameOption(iPair->first);
			nameOption.set("value", iPair->first);
			if (iPair->first == systemName_.toString()) {
				nameOption.set("selected", "selected");
			}
			out << nameOption << std::endl;
		}
		out << cgicc::select() << std::endl;
		out << cgicc::td() << std::endl;
		
		out << cgicc::td("DB Key: ") << std::endl;
		out << cgicc::td() << std::endl;
		
		for (std::map<std::string, std::vector<xdata::UnsignedInteger64> >::iterator iPair = keys.begin(); iPair != keys.end(); ++iPair) {
			
			cgicc::select keySelect;
			keySelect.set("id", "db_key_select_" + iPair->first)
				.set("name", "db_key_select")
				.set("system", iPair->first);
			if (iPair->first != systemName_.toString()) {
				keySelect.set("class", "configurable_hidden db_key_select");
			} else {
				keySelect.set("class", "db_key_select");
			}
			if (configMode_ != "Database") keySelect.set("disabled", "true");
			out << keySelect << std::endl;
			
			for (std::vector<xdata::UnsignedInteger64>::iterator iKey = iPair->second.begin(); iKey != iPair->second.end(); ++iKey) {
				
				cgicc::option keyOption(iKey->toString());
				keyOption.set("value", iKey->toString());
				if (*iKey == dbKey_) {
					keyOption.set("selected", "selected");
				}
				out << keyOption << std::endl;
			}
			out << cgicc::select() << std::endl;
		}
		out << cgicc::td() << std::endl;
		out << cgicc::tr() << std::endl;
		out << cgicc::table() << std::endl;
		
	} catch (emu::fed::exception::ConfigurationException &e) {
		
		out << cgicc::div("Unable to read keys from the database")
			.set("class", "tier2") << std::endl;
		LOG4CPLUS_ERROR(getApplicationLogger(), xcept::stdformat_exception_history(e));
		notifyQualified("ERROR", e);
	}
	
	out << cgicc::div()
		.set("class", "category tier1") << std::endl;
	cgicc::input xmlRadio;
	xmlRadio.set("type", "radio")
		.set("class", "config_type")
		.set("name", "config_type")
		.set("id", "config_type_xml")
		.set("value", "XML");
	if (configMode_ == "XML") xmlRadio.set("checked", "checked");
	out << xmlRadio << std::endl;
	out << cgicc::label("XML")
		.set("for", "config_type_xml") << std::endl;
	out << cgicc::div() << std::endl;
	
	// Make the path from the directory part of the xmlFile variable
	std::vector<std::string> xmlFiles = getXMLFileNames(boost::filesystem::path(xmlFile_.toString()).branch_path());
	if (xmlFiles.empty()) xmlFiles.push_back("Unable to find valid XML files in " + boost::filesystem::path(xmlFile_.toString()).branch_path().string());
	
	out << cgicc::table()
		.set("class", "noborder dialog tier2") << std::endl;
	out << cgicc::tr() << std::endl;
	out << cgicc::td("XML file name: ") << std::endl;
	out << cgicc::td() << std::endl;
	cgicc::select xmlSelect;
	xmlSelect.set("id", "xml_file_select")
		.set("class", "file_select")
		.set("name", "xml_file_select");
	if (configMode_ != "XML") xmlSelect.set("disabled", "true");
	out << xmlSelect << std::endl;
	for (std::vector<std::string>::const_iterator iFile = xmlFiles.begin(); iFile != xmlFiles.end(); iFile++) {
		cgicc::option opt(*iFile);
		opt.set("value", *iFile);
		if (xmlFile_ == *iFile) opt.set("selected", "true");
		out << opt << std::endl;
	}
	out << cgicc::select() << std::endl;
	out << cgicc::td() << std::endl;
	out << cgicc::tr() << std::endl;
	out << cgicc::table() << std::endl;
	
	out << cgicc::div()
		.set("class", "category tier1") << std::endl;
	cgicc::input autoRadio;
	autoRadio.set("type", "radio")
		.set("class", "config_type")
		.set("name", "config_type")
		.set("id", "config_type_autodetect")
		.set("value", "Autodetect");
	if (configMode_ == "Autodetect") autoRadio.set("checked", "checked");
	out << autoRadio << std::endl;
	out << cgicc::label("Autodetect")
		.set("for", "config_type_autodetect") << std::endl;
	out << cgicc::div();
	
	out << cgicc::button()
		.set("class", "right button")
		.set("id", "reconfigure_button") << std::endl;
	out << cgicc::img()
		.set("class", "icon")
		.set("src", "/emu/emuDCS/FEDApps/images/configure-toolbars.png");
	out << "Reconfigure Software" << std::endl;
	out << cgicc::button() << std::endl;
	
	return out.str();
}



std::map<std::string, std::vector<xdata::UnsignedInteger64> > emu::fed::Configurable::getDBKeys()
throw(emu::fed::exception::ConfigurationException)
{
	std::map<std::string, std::vector<xdata::UnsignedInteger64> > dbKeys;
	
	// Get the keys from the systems table.
	SystemDBAgent agent(this, tstoreInstance_);
	
	std::map<std::string, std::vector<std::pair<xdata::UnsignedInteger64, time_t> > > keyMap;

	
	try {
		agent.connect(dbUsername_, dbPassword_);
		
		keyMap = agent.getAllKeys();
		
	} catch (emu::fed::exception::DBException &e) {
		std::ostringstream error;
		error << "Exception attempting to read DB keys";
		XCEPT_RETHROW(emu::fed::exception::ConfigurationException, error.str(), e);
	}
	
	for (std::map<std::string, std::vector<std::pair<xdata::UnsignedInteger64, time_t> > >::iterator iPair = keyMap.begin(); iPair != keyMap.end(); ++iPair) {
		
		std::vector<std::pair<xdata::UnsignedInteger64, time_t> > keys = iPair->second;
		std::vector<xdata::UnsignedInteger64> keyArray;
		for (std::vector<std::pair<xdata::UnsignedInteger64, time_t> >::iterator iKey = keys.begin(); iKey != keys.end(); ++iKey) {
			keyArray.push_back(iKey->first);
		}
		
		dbKeys[iPair->first] = keyArray;
	}
	
	return dbKeys;
}



std::vector<std::string> emu::fed::Configurable::getXMLFileNames(const boost::filesystem::path &configPath)
{
	std::vector<std::string> xmlFiles;

	if (boost::filesystem::exists(configPath)) {
		// The default iterator is the end iterator.
		boost::filesystem::directory_iterator end;
		for (boost::filesystem::directory_iterator iFile(configPath); iFile != end; iFile++) {
			std::string lastThree;
			try {
				std::string name = iFile->native_file_string();
				lastThree = name.substr(name.length() - 3);
			} catch (...) {
				// Don't do anything with file names shorter than 3 characters
				continue;
			}
			boost::algorithm::to_lower(lastThree);
			if (lastThree == "xml") xmlFiles.push_back(iFile->native_file_string());
		}
	}
	
	return xmlFiles;
}
