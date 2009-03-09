/*****************************************************************************\
* $Id: Monitor.cc,v 1.2 2009/03/09 16:03:17 paste Exp $
*
* $Log: Monitor.cc,v $
* Revision 1.2  2009/03/09 16:03:17  paste
* * Updated "ForPage1" routine in Manager with new routines from emu::base::WebReporter
* * Updated inheritance in wake of changes to emu::base::Supervised
* * Added Supervised class to separate XDAQ web-based applications and those with a finite state machine
*
* Revision 1.1  2009/03/05 16:18:25  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
* * Added ForPageOne functionality to the Manager
*
\*****************************************************************************/
#include "emu/fed/Monitor.h"

#include <sstream>
#include <map>
#include <utility>
#include <iomanip>

#include "xgi/Method.h"
#include "cgicc/HTMLClasses.h"
#include "emu/fed/Crate.h"
#include "emu/fed/CrateParser.h"
#include "emu/fed/DDU.h"
#include "emu/fed/DCC.h"
#include "emu/fed/DataTable.h"
#include "emu/fed/DDUDebugger.h"
#include "emu/fed/DCCDebugger.h"
#include "emu/fed/Chamber.h"

XDAQ_INSTANTIATOR_IMPL(emu::fed::Monitor)



emu::fed::Monitor::Monitor(xdaq::ApplicationStub *stub):
xdaq::WebApplication(stub),
Application(stub)
{
	// Variables that are to be made available to other applications
	getApplicationInfoSpace()->fireItemAvailable("xmlFileName", &xmlFile_);
	
	// HyperDAQ pages
	xgi::bind(this, &emu::fed::Monitor::webDefault, "Default");
	xgi::bind(this, &emu::fed::Monitor::getAJAX, "getAJAX");
}



// HyperDAQ pages
void emu::fed::Monitor::webDefault(xgi::Input *in, xgi::Output *out)
{
	// If not configured, parse and configure already!
	if (!crateVector_.size()) {
		try {
			configure();
		} catch (emu::fed::exception::SoftwareException &e) {
			std::ostringstream error;
			error << "Exception caught while configuring";
			LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
			XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
			notifyQualified("FATAL", e2);

			*out << printException(e2);
		}
	}
	
	std::stringstream sTitle;
	sTitle << "FED Crate Monitor (" << endcap_.toString() << ")";
	std::vector<std::string> jsFileNames;
	//jsFileNames.push_back("reload.js");
	jsFileNames.push_back("errorFlasher.js");
	jsFileNames.push_back("monitor.js");
	*out << Header(sTitle.str(), jsFileNames);

	// Starting and stopping monitoring
	*out << cgicc::button().set("id", "start_stop") << std::endl;
	*out << cgicc::img().set("src", "/emu/images/playarrow.png")
		.set("width", "59")
		.set("height", "67")
		.set("style", "vertical-align: middle;") << "Start Monitoring" << cgicc::button() << std::endl;
	
	for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
		*out << cgicc::div(drawCrate(*iCrate)).set("class", "crate") << std::endl;
	}

	//*out << cgicc::textarea().set("id", "debug") << cgicc::textarea();
	
	*out << Footer() << std::endl;
	
}



void emu::fed::Monitor::getAJAX(xgi::Input *in, xgi::Output *out)
{
	
	cgicc::Cgicc cgi(in);
	unsigned int myNumber = 0;
	cgicc::form_iterator name = cgi.getElement("number");
	if (name != cgi.getElements().end()) {
		myNumber = cgi["number"]->getIntegerValue();
	} else {
		// FIXME
		return;
	}

	// Need some header information to be able to return JSON
	cgicc::HTTPResponseHeader jsonHeader("HTTP/1.1", 200, "OK");
	jsonHeader.addHeader("Content-type", "application/json");
	out->setHTTPResponseHeader(jsonHeader);
	
	std::ostringstream json;

	json << "{";
	
	// Loop over crates.
	for (std::vector<Crate *>::const_iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {

		if ((*iCrate)->number() != myNumber) continue;

		std::ostringstream crateNumber;
		crateNumber << (*iCrate)->number();

		json << "\"number\":" << crateNumber.str() << ",";
		
		// Get what it is that we want to return.
		name = cgi.getElement("selection");
		std::string what = "";
		if (name != cgi.getElements().end()) {
			what = cgi["selection"]->getValue();
		} else {
			what = "error";
		}
		
		json << "\"what\":\"" << what << "\",";

		json << "\"boards\":[";

		std::vector<DDU *> myDDUs = (*iCrate)->getDDUs();
		std::vector<DCC *> myDCCs = (*iCrate)->getDCCs();

		// Get data for DDUs
		for (std::vector<DDU *>::const_iterator iDDU = myDDUs.begin(); iDDU != myDDUs.end(); iDDU++) {

			json << "{";

			json << "\"slot\":" << (*iDDU)->slot() << ",";
			json << "\"type\":\"DDU\",";
			json << "\"fmmStatus\":";
			try {
				std::map<std::string, std::string> debugged = DDUDebugger::FMM(((*iDDU)->readParallelStatus()>>8)&0x000F);
				json << "\"" << debugged.begin()->second << "\"";
			} catch (emu::fed::exception::DDUException &e) {
				// FIXME
				json << "\"undefined\"";
				//std::ostringstream error;
				//error << "Exception caught while reading from DDU";
				//LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
				//XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
				//notifyQualified("FATAL", e2);
				
				//out << printException(e2);
				
			}
			json << ",";
			json << "\"L1A\":";
			uint32_t l1aScaler = 0;
			try {
				l1aScaler = (*iDDU)->readL1Scaler(emu::fed::DDUFPGA);
				json << l1aScaler;
			} catch (emu::fed::exception::DDUException &e) {
				// FIXME
				json << "\"undefined\"";
			}
			json << ",";

			if (what == "temperatures") {
				json << "\"temperatures\":[";
				try {
					for (unsigned int iTemp = 0; iTemp < 4; iTemp++) {
						float temperature = (*iDDU)->readTemperature(iTemp);
						std::pair<std::string, std::string> debugged = DDUDebugger::Temperature(temperature);
						json << "{";
						json << "\"name\":\"temp " << iTemp << "\",";
						json << "\"temperature\":\"" << std::fixed << std::setprecision(2) << temperature << "\",";
						json << "\"status\":\"" << debugged.second << "\",";
						json << "\"message\":\"" << debugged.first << "\"";
						json << "}";
						if (iTemp != 3) json << ",";
					}
				} catch (emu::fed::exception::DDUException &e) {
					// FIXME
					json << "Unable to read temperatures";
				}
				json << "]";
				
			} else if (what == "voltages") {
				json << "\"voltages\":[";
				try {
					for (unsigned int iVolt = 0; iVolt < 4; iVolt++) {
						float voltage = (*iDDU)->readVoltage(iVolt);
						std::pair<std::string, std::string> debugged = DDUDebugger::Voltage(iVolt, voltage);
						json << "{";
						if (iVolt == 0) json << "\"name\":\"1.5V\",";
						else if (iVolt == 3) json << "\"name\":\"3.3V\",";
						else json << "\"name\":\"2.5V\",";
						json << "\"voltage\":\"" << std::fixed << std::setprecision(0) << voltage << "\",";
						json << "\"status\":\"" << debugged.second << "\",";
						json << "\"message\":\"" << debugged.first << "\"";
						json << "}";
						if (iVolt != 3) json << ",";
					}
				} catch (emu::fed::exception::DDUException &e) {
					// FIXME
					json << "\"Unable to read voltage\"";
				}
				json << "]";
			
			} else if (what == "counts") {
				json << "\"counts\":[";
				try {
					json << "{\"name\":\"DDUFPGA\",\"count\":" << l1aScaler << "},";
					json << "{\"name\":\"INFPGA0 (0-3)\",\"count\":" << (*iDDU)->readL1Scaler(emu::fed::INFPGA0) << "},";
					json << "{\"name\":\"INFPGA0 (4-7)\",\"count\":" << (*iDDU)->readL1Scaler1(emu::fed::INFPGA0) << "},";
					json << "{\"name\":\"INFPGA1 (8-11)\",\"count\":" << (*iDDU)->readL1Scaler(emu::fed::INFPGA1) << "},";
					json << "{\"name\":\"INFPGA1 (12-15)\",\"count\":" << (*iDDU)->readL1Scaler1(emu::fed::INFPGA1) << "}";

				} catch (emu::fed::exception::DDUException &e) {
					// FIXME
					json << "\"Unable to read L1A scalers\"";
				}
				json << "]";

			} else if (what == "fibers") {
				json << "\"fibers\":[";
				try {
					uint16_t fiberErrors = (*iDDU)->readFiberErrors();
					uint16_t liveFibers = (*iDDU)->readLiveFibers();
					uint16_t unkilledFibers = (*iDDU)->readKillFiber();
					for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
						json << "{";
						json << "\"fiber\":" << iFiber << ",";
						json << "\"name\":\"" << (*iDDU)->getChamber(iFiber)->name() << "\",";
						if (!(unkilledFibers & (1 << iFiber))) {
							json << "\"status\":\"killed\",";
							json << "\"message\":\"killed\"";
						} else if (!(liveFibers & (1 << iFiber))) {
							json << "\"status\":\"undefined\",";
							json << "\"message\":\"!live\"";
						} else if (fiberErrors & (1 << iFiber)) {
							json << "\"status\":\"error\",";
							// FIXME I need some way to quickly identify what is causing the problem
							json << "\"message\":\"\"";
						} else {
							json << "\"status\":\"ok\",";
							json << "\"message\":\"\"";
						}
						json << "}";
						if (iFiber != 14) json << ",";
					}
				} catch (emu::fed::exception::DDUException &e) {
					// FIXME
					json << "\"Unable to read fiber status\"";
				}
				json << "]";
			
			} else if (what == "occupancies") {
				json << "\"occupancies\":[";
				for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
					json << "{";
					json << "\"fiber\":" << iFiber << ",";
					try {
						json << "\"name\":\"" << (*iDDU)->getChamber(iFiber)->name() << "\",";
						std::vector<uint32_t> occupancies = (*iDDU)->readOccupancyMonitor();
						uint32_t dmbCount = (occupancies[0] & 0x0fffffff);
						uint32_t alctCount = (occupancies[1] & 0x0fffffff);
						uint32_t tmbCount = (occupancies[2] & 0x0fffffff);
						uint32_t cfebCount = (occupancies[3] & 0x0fffffff);
						json << "\"DMBcount\":" << dmbCount << ",";
						json << "\"DMBpercent\":\"" << std::fixed << std::setprecision(3) << ((float) dmbCount * 100 / (float) l1aScaler) << "%\",";
						json << "\"ALCTcount\":" << alctCount << ",";
						json << "\"ALCTpercent\":\"" << std::fixed << std::setprecision(3) << ((float) alctCount * 100 / (float) dmbCount) << "%\",";
						json << "\"TMBcount\":" << tmbCount << ",";
						json << "\"TMBpercent\":\"" << std::fixed << std::setprecision(3) << ((float) tmbCount * 100 / (float) dmbCount) << "%\",";
						json << "\"CFEBcount\":" << cfebCount << ",";
						json << "\"CFEBpercent\":\"" << std::fixed << std::setprecision(3) << ((float) alctCount * 100 / (float) cfebCount) << "%\"";
					} catch (emu::fed::exception::DDUException &e) {
						// FIXME
						json << "\"Unable to read occupancies\"";
					}
					json << "}";
					if (iFiber != 14) json << ",";
				}
				json << "]";

			} else {
				json << "\"\"";
			}
			
			json << "}";
			if (!(iDDU + 1 == myDDUs.end() && !myDCCs.size())) json << ",";

		}

		for (std::vector<DCC *>::const_iterator iDCC = myDCCs.begin(); iDCC != myDCCs.end(); iDCC++) {

			json << "{";

			json << "\"slot\":" << (*iDCC)->slot() << ",";
			json << "\"type\":\"DCC\",";
			json << "\"fmmStatus\":";
			try {
				std::map<std::string, std::string> debugged = DCCDebugger::FMMStat(((*iDCC)->readStatusHigh()>>12)&0x000F);
				json << "\"" << debugged.begin()->second << "\"";
			} catch (emu::fed::exception::DCCException &e) {
				json << "\"undefined\"";
				// FIXME
			}
			json << ",";
			json << "\"L1A\":";
			try {
				json << (*iDCC)->readStatusLow();
			} catch (emu::fed::exception::DCCException &e) {
				json << "\"undefined\"";
				// FIXME
			}
			json << ",";

			json << "\"ddurates\":[";
			unsigned int fifoStatus = 0;
			try {
				fifoStatus = ((*iDCC)->readStatusHigh() & 0x0ff0) >> 4;
			} catch (emu::fed::exception::DCCException &e) {
				// FIXME
			}
			for (unsigned int iSlot = 3; iSlot <= 20; iSlot++) {
				try {
					uint16_t rate = (*iDCC)->readDDURate(iSlot);
					// There is no fifo 0 or 6, but that screws up the encoding.
					unsigned int fifo = (*iDCC)->getFIFOFromDDUSlot(iSlot) - 1;
					if (fifo > 5) fifo--;
					fifo = fifo/2;
					json << "{";
					json << "\"slot\":" << iSlot << ",";
					json << "\"rate\":" << rate << ",";
					if (fifoStatus & (1 << (fifo + 3))) {
						json << "\"status\":\"error\",";
						json << "\"message\":\"full\"";
					} else if (fifo < 3 && (fifoStatus & (1 << fifo))) {
						json << "\"status\":\"warning\",";
						json << "\"message\":\"1/2 full\"";
					} else {
						json << "\"status\":\"\",";
						json << "\"message\":\"bits/s\"";
					}
					json << "}";
					if (!(iSlot == 13 && (*iDCC)->slot() == 8) && !(iSlot == 20 && (*iDCC)->slot() == 17)) json << ",";
				} catch (emu::fed::exception::Exception &e) {
					// FIXME
				}
			}
			json << "],";
			
			json << "\"slinkrates\":[";
			unsigned int slinkStatus = 0;
			try {
				slinkStatus = (*iDCC)->readStatusHigh() & 0xf;
			} catch (emu::fed::exception::DCCException &e) {
				// FIXME
			}
			for (unsigned int iLink = 1; iLink <= 2; iLink++) {
				try {
					uint16_t rate = (*iDCC)->readSLinkRate(iLink);
					json << "{";
					json << "\"slink\":" << iLink << ",";
					json << "\"rate\":" << rate << ",";
					if (!(slinkStatus & (1 << (iLink * 2 - 1)))) {
						json << "\"status\":\"error\",";
						json << "\"message\":\"inactive\"";
					} else if (!(slinkStatus & (1 << ((iLink - 1) * 2 )))) {
						json << "\"status\":\"warning\",";
						json << "\"message\":\"backpressure\"";
					} else {
						json << "\"status\":\"\",";
						json << "\"message\":\"bits/s\"";
					}
					json << "}";
					if (iLink != 2) json << ",";
				} catch (emu::fed::exception::Exception &e) {
					// FIXME
				}
			}
			json << "]";

			json << "}";
			if (iDCC + 1 != myDCCs.end()) json << ",";

		}

		json << "]";

		if (iCrate + 1 != crateVector_.end()) json << ",";

	}

	json << "}";

	*out << json.str();
}



void emu::fed::Monitor::configure()
throw (emu::fed::exception::SoftwareException)
{
	// PGK Easier parsing.  Less confusing.
	LOG4CPLUS_DEBUG(getApplicationLogger(), "configure using XML file " << xmlFile_.toString());
	try {
		CrateParser *parser = new CrateParser(xmlFile_.toString().c_str());
		// From the parser, set the crates.
		crateVector_ = parser->getCrates();
		// Get the name of this endcap from the parser, too.  This is specified in the XML
		// for convenience.
		endcap_ = parser->getName();
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to create FED objects by parsing";
		LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("FATAL", e2);
		throw e2;
	} catch (emu::fed::exception::FileException &e) {
		std::ostringstream error;
		error << "Unable to create FED objects due to file exception";
		LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("FATAL", e2);
		throw e2;
	}
}



std::string emu::fed::Monitor::drawCrate(Crate *myCrate)
{
	std::ostringstream out;
	
	DataTable crateTable;
	crateTable->set("class", "crate_table");
	std::ostringstream crateNumber;
	crateNumber << myCrate->number();
	crateTable->set("id", "crate_" + crateNumber.str());
	
	// Draw the header
	crateTable(0,0) << "Crate " << myCrate->number();
	crateTable(0,0)->set("colspan", "4");
	crateTable[0]->set("class", "crate_name");
	
	crateTable(1,1) << "slot";
	crateTable(1,2) << "board" << cgicc::br() << "#L1A";
	crateTable(1,3) << cgicc::form().set("class", "crate_form")
		.set("id", "crate_" + crateNumber.str() + "_form")
		.set("action", "getAJAX")
		.set("method", "get");
	crateTable(1,3) << cgicc::input().set("type", "hidden")
		.set("name", "number")
		.set("value", crateNumber.str());
	crateTable(1,3) << cgicc::select().set("name", "selection");
	crateTable(1,3) << cgicc::option("fiber status").set("value", "fibers");
	crateTable(1,3) << cgicc::option("L1A counts").set("value", "counts");
	crateTable(1,3) << cgicc::option("occupancies").set("value", "occupancies");
	crateTable(1,3) << cgicc::option("temperatures").set("value", "temperatures");
	crateTable(1,3) << cgicc::option("voltages").set("value", "voltages");
	crateTable(1,3) << cgicc::select();
	// FIXME
	//crateTable(1,3) << cgicc::input().set("type", "submit");
	crateTable(1,3) << cgicc::form();
	crateTable[1]->set("class", "headers");
	
	std::vector<DDU *> myDDUs = myCrate->getDDUs();
	unsigned int iRow = 2;
	for (std::vector<DDU *>::iterator iDDU = myDDUs.begin(); iDDU != myDDUs.end(); iDDU++, iRow++) {
		std::ostringstream slotText;
		slotText << (*iDDU)->slot();
		crateTable[iRow]->set("class", "board ddu");
		crateTable(iRow, 0)->set("id", "crate_" + crateNumber.str() + "_slot_" + slotText.str() + "_status");
		crateTable(iRow, 1).setClass("slot");
		crateTable(iRow, 1) << slotText.str();
		crateTable(iRow, 2).setClass("name");
		crateTable(iRow, 2) << "RUI " << myCrate->getRUI((*iDDU)->slot()) << cgicc::br();
		crateTable(iRow, 2) << cgicc::span(" ")
			.set("class", "l1a")
			.set("id", "crate_" + crateNumber.str() + "_slot_" + slotText.str() + "_l1a");
		crateTable(iRow, 3)->set("id", "crate_" + crateNumber.str() + "_slot_" + slotText.str() + "_data");
		crateTable(iRow, 3).setClass("data");
		crateTable(iRow, 3) << "Awaiting data....";
	}
	
	iRow = 2 + myDDUs.size();
	std::vector<DCC *> myDCCs = myCrate->getDCCs();
	for (std::vector<DCC *>::iterator iDCC = myDCCs.begin(); iDCC != myDCCs.end(); iDCC++, iRow++) {
		std::ostringstream slotText;
		slotText << (*iDCC)->slot();
		crateTable[iRow]->set("class", "board dcc");
		crateTable(iRow, 0)->set("id", "crate_" + crateNumber.str() + "_slot_" + slotText.str() + "_status");
		crateTable(iRow, 1).setClass("slot");
		crateTable(iRow, 1) << slotText.str();
		crateTable(iRow, 2).setClass("name");
		crateTable(iRow, 2) << "DCC" << cgicc::br();
		crateTable(iRow, 2) << cgicc::span(" ")
			.set("class", "l1a")
			.set("id", "crate_" + crateNumber.str() + "_slot_" + slotText.str() + "_l1a");
		crateTable(iRow, 3)->set("id", "crate_" + crateNumber.str() + "_slot_" + slotText.str() + "_data");
		crateTable(iRow, 3).setClass("data");
		crateTable(iRow, 3) << "Awaiting data....";
	}
	
	out << crateTable.toHTML();
	
	return out.str();
}
