/*****************************************************************************\
* $Id: Configurable.cc,v 1.5 2009/07/16 09:24:21 paste Exp $
\*****************************************************************************/
#include "emu/fed/Configurable.h"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/algorithm/string/case_conv.hpp"

#include "xgi/Method.h"
#include "cgicc/HTMLClasses.h"
#include "emu/fed/Crate.h"
#include "emu/fed/AutoConfigurator.h"
#include "emu/fed/DBConfigurator.h"
#include "emu/fed/XMLConfigurator.h"
#include "emu/fed/JSONSpiritWriter.h"

emu::fed::Configurable::Configurable(xdaq::ApplicationStub *stub):
xdaq::WebApplication(stub),
emu::fed::Application(stub),
systemName_("unnamed")
{
	// Variables that are to be made available to other applications
	getApplicationInfoSpace()->fireItemAvailable("systemName", &systemName_);
	getApplicationInfoSpace()->fireItemAvailable("xmlFileName", &xmlFile_);
	getApplicationInfoSpace()->fireItemAvailable("dbUsername", &dbUsername_);
	getApplicationInfoSpace()->fireItemAvailable("dbPassword", &dbPassword_);
	getApplicationInfoSpace()->fireItemAvailable("dbKey", &dbKey_);
	getApplicationInfoSpace()->fireItemAvailable("configMode",  &configMode_);
	
	// HyperDAQ pages
	xgi::bind(this, &emu::fed::Configurable::webGetConfiguration, "GetConfiguration");
	xgi::bind(this, &emu::fed::Configurable::webChangeConfigMode, "ChangeConfigMode");
	xgi::bind(this, &emu::fed::Configurable::webChangeXMLFile, "ChangeXMLFile");
	xgi::bind(this, &emu::fed::Configurable::webChangeXMLFile, "ChangeDBKey");
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
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			XCEPT_DECLARE(emu::fed::exception::ConfigurationException, e2, error.str());
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
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			XCEPT_DECLARE(emu::fed::exception::FileException, e2, error.str());
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
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			XCEPT_DECLARE(emu::fed::exception::ConfigurationException, e2, error.str());
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
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::ConfigurationException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
		output.push_back(JSONSpirit::Pair("exception", error.str()));
	}
	
}



void emu::fed::Configurable::softwareConfigure()
throw (emu::fed::exception::ConfigurationException)
{

	for (size_t iCrate = 0; iCrate < crateVector_.size(); iCrate++) {
		delete crateVector_[iCrate];
	}
	
	crateVector_.clear();
	
	if (configMode_ == "Autodetect") {
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
		
		DBConfigurator configurator(this, dbUsername_.toString(), dbPassword_.toString(), dbKey_);
		
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
	out << cgicc::input().set("type", "radio")
		.set("class", "config_type")
		.set("name", "config_type")
		.set("id", "config_type_database")
		.set("value", "Database") << std::endl;
	out << cgicc::label("Database")
		.set("for", "config_type_database") << std::endl;
	out << cgicc::div() << std::endl;
	
	// TODO: Put the possible configuration keys in this vector
	std::vector<uint64_t> dbKeys;
	
	out << cgicc::table()
		.set("class", "noborder dialog tier2") << std::endl;
	out << cgicc::tr() << std::endl;
	out << cgicc::td("DB Key: ") << std::endl;
	out << cgicc::td() << std::endl;
	out << cgicc::select()
		.set("id", "db_key_select")
		.set("class", "key_select")
		.set("name", "db_key_select")
		.set("disabled", "true") << std::endl;
	for (std::vector<uint64_t>::const_iterator iKey = dbKeys.begin(); iKey != dbKeys.end(); iKey++) {
		std::ostringstream keyString;
		keyString << *iKey;
		cgicc::option opt(keyString.str());
		opt.set("value", keyString.str());
		if (dbKey_ == *iKey) opt.set("selected", "true");
		out << opt << std::endl;
	}
	out << cgicc::select() << std::endl;
	out << cgicc::td() << std::endl;
	out << cgicc::tr() << std::endl;
	out << cgicc::table() << std::endl;
	
	out << cgicc::div()
		.set("class", "category tier1") << std::endl;
	out << cgicc::input()
		.set("type", "radio")
		.set("class", "config_type")
		.set("name", "config_type")
		.set("id", "config_type_xml")
		.set("value", "XML") << std::endl;
	out << cgicc::label("XML")
		.set("for", "config_type_xml") << std::endl;
	out << cgicc::div() << std::endl;
	
	// To access the available files, we need a few directories.
	std::string homeDir(getenv("HOME"));
	boost::filesystem::path configPath;
	configPath = boost::filesystem::path(homeDir + "config/fed/");
	
	// Use boost to get all the xml files in this directory
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
	
	out << cgicc::table()
		.set("class", "noborder dialog tier2") << std::endl;
	out << cgicc::tr() << std::endl;
	out << cgicc::td("XML file name: ") << std::endl;
	out << cgicc::td() << std::endl;
	out << cgicc::select()
		.set("id", "xml_file_select")
		.set("class", "file_select")
		.set("name", "xml_file_select")
		.set("disabled", "true") << std::endl;
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
	out << cgicc::input().set("type", "radio")
		.set("class", "config_type")
		.set("name", "config_type")
		.set("id", "config_type_autodetect")
		.set("value", "Autodetect") << std::endl;
	out << cgicc::label("Autodetect")
		.set("for", "config_type_autodetect") << std::endl;
	out << cgicc::div();
	
	return out.str();
}
