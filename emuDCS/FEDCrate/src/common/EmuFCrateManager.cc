/*****************************************************************************\
* $Id: EmuFCrateManager.cc,v 1.25 2008/10/29 16:01:44 paste Exp $
*
* $Log: EmuFCrateManager.cc,v $
* Revision 1.25  2008/10/29 16:01:44  paste
* Updated interoperability with primative DCC commands, added new xdata variables for future use.
*
* Revision 1.24  2008/10/22 20:23:58  paste
* Fixes for random FED software crashes attempted.  DCC communication and display reverted to ancient (pointer-based communication) version at the request of Jianhui.
*
* Revision 1.23  2008/10/15 00:46:56  paste
* Attempt to solve certain crashes on Enable/Disable commands.
*
* Revision 1.22  2008/10/09 11:21:19  paste
* Attempt to fix DCC MPROM load.  Added debugging for "Global SOAP death" bug.  Changed the debugging interpretation of certain DCC registers.  Added inline SVG to EmuFCrateManager page for future GUI use.
*
* Revision 1.21  2008/10/04 18:44:06  paste
* Fixed bugs in DCC firmware loading, altered locations of files and updated javascript/css to conform to WC3 XHTML standards.
*
* Revision 1.20  2008/08/25 12:25:49  paste
* Major updates to VMEController/VMEModule handling of CAEN instructions.  Also, added version file for future RPMs.
*
* Revision 1.19  2008/08/18 08:30:15  paste
* Update to fix error propagation from IRQ threads to EmuFCrateManager.
*
* Revision 1.18  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "EmuFCrateManager.h"

#include <vector>
#include <iostream>
#include <ostream>
//#include <unistd.h> // for sleep()
#include <sstream>
//#include <cstdlib>
#include <iomanip>
//#include <time.h>
#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/configurator.h>
#include <math.h>

#include "xdata/UnsignedLong.h"
//#include "xoap/DOMParser.h"
//#include "xoap/domutils.h"
//#include "xdata/xdata.h"
//#include "xgi/Utils.h"
#include "xgi/Method.h"
// #include "xdaq/Application.h"
// #include "xdaq/ApplicationGroup.h"
// #include "xdaq/ApplicationContext.h"
// #include "xdaq/ApplicationStub.h"
// #include "xdaq/exception/Exception.h"
#include "xdaq/NamespaceURI.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/Method.h"
//#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
//#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include "cgicc/HTTPResponseHeader.h"
// #include "xdata/String.h"
// #include "xdata/Float.h"
// #include "xdata/Double.h"
// #include "xdata/Boolean.h"
#include "xdata/exdr/FixedSizeInputStreamBuffer.h"
#include "xdata/exdr/AutoSizeOutputStreamBuffer.h"
#include "xdata/exdr/Serializer.h"
#include "xdaq2rc/RcmsStateNotifier.h"

XDAQ_INSTANTIATOR_IMPL(EmuFCrateManager);

EmuFCrateManager::EmuFCrateManager(xdaq::ApplicationStub * s):
    //    throw (xdaq::exception::Exception) :
	EmuFEDApplication(s),
	//state_table_(this),
	soapConfigured_(false),
	soapLocal_(false)
{
	xdata::InfoSpace *i = getApplicationInfoSpace();
	i->fireItemAvailable("ttsID", &tts_id_);
	i->fireItemAvailable("ttsCrate", &tts_crate_);
	i->fireItemAvailable("ttsSlot", &tts_slot_);
	i->fireItemAvailable("ttsBits", &tts_bits_);

	xgi::bind(this,&EmuFCrateManager::webDefault, "Default");
	xgi::bind(this,&EmuFCrateManager::webFire, "Fire");

	// SOAP call-back functions, which relays to *Action method.
	xoap::bind(this, &EmuFCrateManager::onConfigure, "Configure", XDAQ_NS_URI);
	xoap::bind(this, &EmuFCrateManager::onEnable,    "Enable",    XDAQ_NS_URI);
	xoap::bind(this, &EmuFCrateManager::onDisable,   "Disable",   XDAQ_NS_URI);
	xoap::bind(this, &EmuFCrateManager::onHalt,      "Halt",      XDAQ_NS_URI);
	xoap::bind(this, &EmuFCrateManager::onSetTTSBits, "SetTTSBits", XDAQ_NS_URI);

	// fsm_ is defined in EmuApplication
	fsm_.addState('H', "Halted",     this, &EmuFCrateManager::stateChanged);
	fsm_.addState('C', "Configured", this, &EmuFCrateManager::stateChanged);
	fsm_.addState('E', "Enabled",    this, &EmuFCrateManager::stateChanged);

	fsm_.addStateTransition(
		'H', 'C', "Configure", this, &EmuFCrateManager::configureAction); // valid
	fsm_.addStateTransition(
		'C', 'C', "Configure", this, &EmuFCrateManager::configureAction); // valid

	fsm_.addStateTransition(
		'C', 'E', "Enable",    this, &EmuFCrateManager::enableAction); // valid
	fsm_.addStateTransition(
		'E', 'E', "Enable",    this, &EmuFCrateManager::enableAction); // valid

	fsm_.addStateTransition(
		'E', 'C', "Disable",   this, &EmuFCrateManager::disableAction); // valid

	fsm_.addStateTransition(
		'C', 'H', "Halt",      this, &EmuFCrateManager::haltAction); // valid
	fsm_.addStateTransition(
		'E', 'H', "Halt",      this, &EmuFCrateManager::haltAction); // valid
	fsm_.addStateTransition(
		'H', 'H', "Halt",      this, &EmuFCrateManager::haltAction); // valid
	fsm_.addStateTransition(
		'F', 'H', "Halt",      this, &EmuFCrateManager::haltAction); // valid

	fsm_.setInitialState('H');
	fsm_.reset();

	ConfigureState_ = "None";

	// state_ is defined in EmuApplication
	state_ = fsm_.getStateName(fsm_.getCurrentState());

	// Logger/Appender
	// log file format: EmuFEDYYYY-DOY-HHMMSS_rRUNNUMBER.log
	char datebuf[55];
	char filebuf[255];
	std::time_t theTime = time(NULL);

	std::strftime(datebuf, sizeof(datebuf), "%Y-%m-%d-%H:%M:%S", localtime(&theTime));
	std::sprintf(filebuf,"EmuFCrateManager-%s.log",datebuf);

	log4cplus::SharedAppenderPtr myAppend = new log4cplus::FileAppender(filebuf);
	myAppend->setName("EmuFCrateManagerAppender");

	//Appender Layout
	std::auto_ptr<Layout> myLayout = std::auto_ptr<Layout>(new log4cplus::PatternLayout("%D{%m/%d/%Y %j-%H:%M:%S.%q} %-5p %c, %m%n"));
	// for date code, use the Year %Y, DayOfYear %j and Hour:Min:Sec.mSec
	// only need error data from Log lines with "ErrorData" tag
	myAppend->setLayout( myLayout );

	getApplicationLogger().addAppender(myAppend);

	// TEMP
	getApplicationLogger().setLogLevel(DEBUG_LOG_LEVEL);

}


// HyperDAQ pages
void EmuFCrateManager::webDefault(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	
	// This header manipulation will make inline SVG possible, I think.
	cgicc::HTTPResponseHeader newHeader("HTTP/1.1",200,"OK");
	newHeader.addHeader("Content-Type","application/xhtml+xml");
	out->setHTTPResponseHeader(newHeader);

	std::vector<std::string> jsFileNames;
	jsFileNames.push_back("errorFlasher.js");
	jsFileNames.push_back("reload.js");
	*out << Header("EmuFCrateManager", jsFileNames);

	// Manual state changing
	*out << cgicc::fieldset()
		.set("class","fieldset") << std::endl;
	*out << cgicc::div("Manual state changes")
		.set("class","legend") << std::endl;

	*out << cgicc::div();
	*out << "Present state: ";
	*out << cgicc::span(state_.toString())
		.set("class",state_.toString()) << std::endl;
	*out << cgicc::div() << std::endl;

	// PGK You can't change states if you have been configured from above.
	*out << cgicc::div() << std::endl;
	if (!soapConfigured_) {
		*out << cgicc::form()
			.set("style","display: inline;")
			.set("action","/" + getApplicationDescriptor()->getURN() + "/Fire")
			.set("method","GET") << std::endl;
		if (state_.toString() == "Halted" || state_.toString() == "Configured") {
			*out << cgicc::input()
				.set("name","action")
				.set("type","submit")
				.set("value","Configure") << std::endl;
		}
		if (state_.toString() == "Configured") {
			*out << cgicc::input()
				.set("name","action")
				.set("type","submit")
				.set("value","Enable") << std::endl;
		}
		if (state_.toString() == "Enabled") {
			*out << cgicc::input()
				.set("name","action")
				.set("type","submit")
				.set("value","Disable") << std::endl;
		}
		if (state_.toString() == "Halted" || state_.toString() == "Configured" || state_.toString() == "Enabled" || state_.toString() == "Failed" || state_.toString() == STATE_UNKNOWN) {
			*out << cgicc::input()
				.set("name","action")
				.set("type","submit")
				.set("value","Halt") << std::endl;
		}
		*out << cgicc::form() << std::endl;

	} else {
		*out << "EmuFCrateManager has been configured through SOAP." << std::endl;
		*out << cgicc::br() << "Send the Halt signal to manually change states." << std::endl;
	}
	*out << cgicc::div() << std::endl;
	*out << cgicc::fieldset() << std::endl;


	// EmuFCrate states
	*out << cgicc::fieldset()
		.set("class","fieldset") << std::endl;
	*out << cgicc::div("EmuFCrate states")
		.set("class","legend") << std::endl;

	std::set<xdaq::ApplicationDescriptor * > descriptors =
		getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("EmuFCrate");

	std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptor;
    for ( itDescriptor = descriptors.begin(); itDescriptor != descriptors.end(); itDescriptor++ ) {

		// PGK ping the EmuFCrates for their informations.
		//  This will be used from here on out instead of the status table.
		xoap::MessageReference reply = getParameters((*itDescriptor));

		xdata::String endcap = readParameter<xdata::String>(reply,"endcap");
		
		std::stringstream className;
		className << (*itDescriptor)->getClassName() << "(" << (*itDescriptor)->getInstance() << ") " << endcap.toString();
		std::stringstream url;
		url << (*itDescriptor)->getContextDescriptor()->getURL() << "/" << (*itDescriptor)->getURN();

		*out << cgicc::div()
			.set("style","clear: both");

		*out << cgicc::a(className.str())
			.set("href",url.str()) << std::endl;

		*out << " Present state: " << std::endl;
		xdata::String currentState = readParameter<xdata::String>(reply,"State");
		*out << cgicc::span(currentState)
			.set("class",currentState) << std::endl;

		xdata::String xmlFileName = readParameter<xdata::String>(reply,"xmlFileName");
		*out << cgicc::br() << std::endl;
		*out << cgicc::span("Configuration located at " + xmlFileName.toString())
		.set("style","color: #A00; font-size: 10pt;") << std::endl;
		*out << cgicc::br() << std::endl;

		*out << cgicc::div();

		// Print a table of all the DCC input and output rates.
		if (currentState == "Enabled") {

			xdata::String errorChambers = readParameter<xdata::String>(reply,"errorChambers");

			//LOG4CPLUS_DEBUG(getApplicationLogger(),"I think that there are " << errorChambers.size() << " errors to read out.");

			if (errorChambers != "") {
				*out << cgicc::div()
					<< "Chambers in an error state: " << std::endl;
				*out << cgicc::span()
					.set("class","error") << std::endl;
				*out << errorChambers.toString();
				*out << cgicc::span() << std::endl;
				*out << cgicc::div() << std::endl;
			}

			// PGK The reply is a std::vector of std::vectors of integers.
			// The outer std::vector is indexed by crates.
			// The inner std::vector contains the data from the DCC, with element [0]
			//  being the crate number,
			//  element [1] being the Slink output,
			//  elements [2] through [7] being the fiber inputs to the DCC.
			//
			// The math here is wicked-squiggly, so don't try to follow it.
			//  I should probably clean it up.
			xdata::Vector<xdata::Vector<xdata::UnsignedInteger> > dccInOut = readParameter<xdata::Vector<xdata::Vector<xdata::UnsignedInteger> > >(reply,"dccInOut");

			xdata::Vector<xdata::Vector<xdata::UnsignedInteger> >::iterator iCrate;
			for (iCrate = dccInOut.begin(); iCrate != dccInOut.end(); iCrate++) {
				std::stringstream style;
				style << "margin: 10px auto 10px auto; width: 45%; float: " << ((*iCrate)[0] % 2 ? "left;" : "right;");
				*out << cgicc::div()
					.set("style", style.str()) << std::endl;
				*out << cgicc::table()
					.set("style","border-collapse: collapse; border: solid 2px #000; width: 100%;") << std::endl;
				*out << cgicc::tr()
					.set("style","background-color: #000; color: #FFF; text-align: center; border-bottom: solid 1px #000; font-size: 14pt; font-weight: bold;")  << std::endl;
				*out << cgicc::td()
					.set("colspan","5") << std::endl;
				*out << "Data Rates for Crate " << (*iCrate)[0] << cgicc::td() << std::endl;
				*out << cgicc::tr() << std::endl;
				*out << cgicc::tr() << std::endl;
				for (unsigned int i=2; i<7; i++) {
					unsigned int slot = (i % 2) ? 15 - (i+1)/2 : i/2 + 2;
					*out << cgicc::td()
						.set("style","border: solid 1px #000; background-color: #FFF;") << std::endl;
					*out << "Slot " << slot << cgicc::br() << (*iCrate)[i] << std::endl;
					*out << cgicc::td() << std::endl;
				}
				*out << cgicc::tr() << std::endl;
				*out << cgicc::tr()
					.set("style","background-color: #FFF; text-align: center; border-bottom: solid 1px #000; font-weight: bold;") << std::endl;
				*out << cgicc::td()
					.set("colspan","5") << std::endl;
				*out << "Slink Output 1: " << (*iCrate)[1] << cgicc::td() << cgicc::tr() << std::endl;
				*out << cgicc::tr() << std::endl;
				for (unsigned int i=8; i<13; i++) {
					unsigned int slot = (i % 2) ? (i-1)/2 + 2 : 15 - i/2;
					*out << cgicc::td()
						.set("style","border: solid 1px #000; background-color: #FFF;") << std::endl;
					*out << "Slot " << slot << cgicc::br() << (*iCrate)[i] << std::endl;
					*out << cgicc::td() << std::endl;
				}
				*out << cgicc::tr() << std::endl;
				*out << cgicc::tr()
					.set("style","background-color: #FFF; text-align: center; border-bottom: solid 1px #000; font-weight: bold;") << std::endl;
				*out << cgicc::td()
					.set("colspan","5") << std::endl;
				*out << "Slink Output 2: " << (*iCrate)[7] << cgicc::td() << cgicc::tr() << std::endl;
				*out << cgicc::table() << std::endl;
				*out << cgicc::div() << std::endl;
			}
		}
		
		*out << cgicc::br() << std::endl;
	}

	*out << cgicc::fieldset() << std::endl;

	// Testing SVG
	
	// Radius of an individual station in px
	unsigned int stationRadius = 100;
	
	// Spacing between stations (and sides of SVG canvas) in px
	unsigned int stationSpacing = 10;
	
	// Proportions of chamber sizes in "normal" stations, inside out
	std::vector<float> normalSizes;
	normalSizes.push_back(0.8);
	normalSizes.push_back(1);
	
	// Proportions of chamber sizes in ME1/1 stations, inside out
	std::vector<float> me11Sizes;
	me11Sizes.push_back(0.8);
	me11Sizes.push_back(0.9);
	me11Sizes.push_back(1);
	
	// Proportions of misc. other things
	float centerSize = 0.5;
	float ringSpacing = 0.05;
	
	// Draw the canvas
	*out << "<!--svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << (4 * stationRadius * 2 + 5 * stationSpacing) << "\" height=\"" << (4 * stationRadius + 3 * stationSpacing) << "\" style=\"margin: 5px auto 5px auto;\">" << std::endl;
	
	// Loop over endcaps
	for (unsigned int iEndcap = 1; iEndcap <= 2; iEndcap++) {
		
		// Loop over stations
		for (unsigned int iStation = 1; iStation <= 4; iStation++) {
			
			// Draw the ring itself
			unsigned int centerX = (stationRadius * 2 + stationSpacing) * iStation - stationRadius;
			unsigned int centerY = (stationRadius * 2 + stationSpacing) * iEndcap - stationRadius;
			
			*out << "<circle cx=\"" << centerX << "\" cy=\"" << centerY << "\" r=\"" << stationRadius << "\" stroke=\"#000\" stroke-width=\"3\" fill=\"#FFF\" />" << std::endl;
			
			// Calculate sizes of things
			std::vector<float> radii;
			
			// Station 1 is different
			std::vector<float> sizes;
			if (iStation == 1) sizes = me11Sizes;
			else sizes = normalSizes;
			
			float total = 0;
			for (std::vector<float>::iterator iSize = sizes.begin(); iSize != sizes.end(); iSize++) {
				total += (*iSize);
			}
			
			float totalSizes = centerSize + ringSpacing * (sizes.size() - 1) + total;
			
			float radiusCache = centerSize/totalSizes * stationRadius;
			
			for (std::vector<float>::iterator iSize = sizes.begin(); iSize != sizes.end(); iSize++) {

				radii.push_back(radiusCache);
				radii.push_back(radiusCache + (*iSize)/totalSizes * stationRadius);
				
				radiusCache += ringSpacing/totalSizes * stationRadius + (*iSize)/totalSizes * stationRadius;
			}
			
			// Start drawing chambers
			for (unsigned int iRing = 1; iRing <= sizes.size(); iRing++) {
				
				// Number of chambers in this ring.
				unsigned int nChambers = 18;
				if (iStation == 1 || iRing == 2) {
					nChambers = 36;
				}
				
				// The verticies of the trapizoid
				std::vector<float> xPoints(1,0);
				std::vector<float> yPoints(1,0);
				xPoints.reserve(4);
				yPoints.reserve(4);
				
				xPoints.push_back(cos(3.14159265/nChambers) * sin(3.14159265/nChambers) * 2 * radii[2 * (iRing - 1) + 1]);
				yPoints.push_back(sin(3.14159265/nChambers) * sin(3.14159265/nChambers) * 2 * radii[2 * (iRing - 1) + 1]);
				
				xPoints.push_back(cos(3.14159265/nChambers) * sin(3.14159265/nChambers) * 2 * radii[2 * (iRing - 1)]);
				yPoints.push_back(sin(3.14159265/nChambers) * sin(3.14159265/nChambers) * 2 * radii[2 * (iRing - 1)] + radii[2 * (iRing - 1) + 1] - radii[2 * (iRing - 1)]);
				
				xPoints.push_back(0);
				yPoints.push_back(radii[2 * (iRing - 1) + 1] - radii[2 * (iRing - 1)]);
				
				// The starting angle for chamber 1
				float angularOffset = 90;
				if (nChambers == 18) {
					angularOffset -= 10;
				}
				
				// Draw the chamber and move it to the appropriate location
				for (unsigned int iChamber = 1; iChamber <= nChambers; iChamber++) {
					
					// Set the status
					std::string status = "killed";
					if (iStation == 4 && iRing == 2) {
						status = "undefined";
					}
					
					float angle = angularOffset - (360/nChambers * (iChamber - 1));
					
					// Rotate and move the group
					*out << "<g transform=\"rotate(" << angle << " " << centerX << " " << centerY << ") translate(" << centerX << " " << ((stationRadius * 2 + stationSpacing) * iEndcap - stationRadius - radii[2 * (iRing - 1) + 1]) << ")\" >" << std::endl;
					
					// Draw the chamber
					*out << "<polygon id=\"" << (iEndcap == 1 ? "p" : "m") << iStation << "_" << iRing << "_" << iChamber << "\" stroke=\"#000\" stroke-width=\"1\" class=\"" << status << "\" points=\"";
					for (unsigned int iPoint = 0; iPoint < 4; iPoint++) {
						*out << xPoints[iPoint] << "," << yPoints[iPoint] << " ";
					}
					*out << "\" />" << std::endl;
					*out << "</g>" << std::endl;
				}
			}
		}
		
	}
	
	*out << "</svg-->" << std::endl;

	*out << Footer();

}



void EmuFCrateManager::webFire(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception)
{
	cgicc::Cgicc cgi(in);
	soapLocal_ = true;

	std::string action = "";
	cgicc::form_iterator name = cgi.getElement("action");
	if(name != cgi.getElements().end()) {
		action = cgi["action"]->getValue();
		std::cout << "webFire action: " << action << std::endl;
		LOG4CPLUS_DEBUG(getApplicationLogger(), "Local FSM state change requested: " << action;);
		fireEvent(action);
	}

	webRedirect(in, out);
}



void EmuFCrateManager::webRedirect(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception)
{
	std::string url = in->getenv("PATH_TRANSLATED");

	cgicc::HTTPResponseHeader &header = out->getHTTPResponseHeader();

	header.getStatusCode(303);
	header.getReasonPhrase("See Other");
	header.addHeader("Location",url.substr(0, url.find("/" + in->getenv("PATH_INFO"))));
}



void EmuFCrateManager::configureAction(toolbox::Event::Reference e)
	throw (toolbox::fsm::exception::Exception)
{
	//std::cout << "  inside EmuFCrateManager::configureAction " << std::endl;
	LOG4CPLUS_DEBUG(getApplicationLogger(), "Received SOAP message: Configure");

	if (soapLocal_) {
		soapLocal_ = false;
		soapConfigured_ = false;
	} else {
		soapConfigured_ = true;
	}

	// PGK This is given to us from the CSCSV.  This will determine our logging
	//  preferences.
	LOG4CPLUS_DEBUG(getApplicationLogger(), "Run type is " << runType_.toString());
	if (runType_.toString() == "Debug") {
		getApplicationLogger().setLogLevel(DEBUG_LOG_LEVEL);
	} else {
		getApplicationLogger().setLogLevel(INFO_LOG_LEVEL);
	}

	// PGK Now send it to the EmuFCrates.
	setParameter("EmuFCrate","runType","xsd:string",runType_.toString());

	try{
		sendCommand("Configure","EmuFCrate");
	} catch (xdaq::exception::Exception e) {
		XCEPT_RAISE(toolbox::fsm::exception::Exception, "error in EmuFCrateManager::configureAction");
	}
}


void EmuFCrateManager::enableAction(toolbox::Event::Reference e)
	throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), "Received SOAP message: Enable");
	soapLocal_ = false;

	// PGK If the run number is not set, this is a debug run.
	LOG4CPLUS_DEBUG(getApplicationLogger(), "The run number is " << runNumber_.toString());
	if (runNumber_.toString() == "" || runNumber_.toString() == "0") {
		getApplicationLogger().setLogLevel(DEBUG_LOG_LEVEL);
	}
	// PGK Now send the run number to the EmuFCrates.
	setParameter("EmuFCrate","runNumber","xsd:unsignedLong",runNumber_.toString());

	try{
		sendCommand("Enable","EmuFCrate");
	} catch (xdaq::exception::Exception e) {
		XCEPT_RAISE(toolbox::fsm::exception::Exception, "error in EmuFCrateManager::enableAction");
	}
}


void EmuFCrateManager::disableAction(toolbox::Event::Reference e)
	throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), "Received SOAP message: Disable");
	soapLocal_ = false;

	try{
		sendCommand("Disable","EmuFCrate");
	} catch (xdaq::exception::Exception e) {
		XCEPT_RAISE(toolbox::fsm::exception::Exception, "error in EmuFCrateManager::disableAction");
	}
}


void EmuFCrateManager::haltAction(toolbox::Event::Reference e)
	throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), "Received SOAP message: Halt");
	soapLocal_ = false;
	soapConfigured_ = false;
	try{
		sendCommand("Halt","EmuFCrate");
	} catch (xdaq::exception::Exception e) {
		XCEPT_RAISE(toolbox::fsm::exception::Exception, "error in EmuFCrateManager::haltAction");
	}
}



void EmuFCrateManager::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
	throw (toolbox::fsm::exception::Exception)
{
	EmuApplication::stateChanged(fsm);

	//state_table_.refresh();

	// Check to see if I should fail based on the statuses of the EmuFCrates.
	std::set<xdaq::ApplicationDescriptor *> apps;
	try {
		apps = getApplicationContext()->getDefaultZone()->getApplicationDescriptors("EmuFCrate");
	} catch (xdaq::exception::ApplicationDescriptorNotFound &e) {
		// PGK WOAH!  Proper exception handling!
		LOG4CPLUS_ERROR(getApplicationLogger(), e.what());
		XCEPT_RAISE(xdaq::exception::Exception, "Application class name not found.  Can't continue.");
	}

	std::set<xdaq::ApplicationDescriptor *>::iterator i;
	bool failure = false;
	for (i = apps.begin(); i != apps.end(); ++i) {
		// PGK Use this instead of the state table.
		xoap::MessageReference reply = getParameters((*i));
		xdata::String currentState = readParameter<xdata::String>(reply,"State");
		
		if (currentState == "Failed") {
			failure = true;
			LOG4CPLUS_ERROR(getApplicationLogger(),(*i)->getClassName() << "(" << (*i)->getInstance() << ") shows status: Failed");
		}
	}
	if (failure) {
		//state_ = "Failed";
		XCEPT_RAISE(toolbox::fsm::exception::Exception, "failure in one of the EmuFCrates");
	}

	LOG4CPLUS_DEBUG(getApplicationLogger(), "FSM state changed to " << state_.toString());
	
}



xoap::MessageReference EmuFCrateManager::onConfigure (xoap::MessageReference message) throw (xoap::exception::Exception)
{

	LOG4CPLUS_DEBUG(getApplicationLogger(), "Remote SOAP state change requested: Configure");

	// PGK I avoid errors at all cost.
	if (state_.toString() == "Enabled" || state_.toString() == "Failed") {
		LOG4CPLUS_WARN(getApplicationLogger(), state_.toString() <<"->Configured is not a valid transition.  Fixing by going to Halted first.");
		fireEvent("Halt");
	}

	fireEvent("Configure");

	return createReply(message);
}



xoap::MessageReference EmuFCrateManager::onEnable (xoap::MessageReference message) throw (xoap::exception::Exception)
{

	LOG4CPLUS_DEBUG(getApplicationLogger(), "Remote SOAP state change requested: Enable");

	// PGK I avoid errors at all cost.
	if (state_.toString() == "Halted" || state_.toString() == "Failed") {
		LOG4CPLUS_WARN(getApplicationLogger(), state_.toString() <<"->Enabled is not a valid transition.  Fixing by going to Halted->Configured first.");
		fireEvent("Halt");
		fireEvent("Configure");
	}

	fireEvent("Enable");

	return createReply(message);
}

xoap::MessageReference EmuFCrateManager::onSetTTSBits(xoap::MessageReference message) throw (xoap::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), "Remote SOAP command received: SetTTSBits");

	const std::string fed_app = "EmuFCrate";

	// JRG, decode the Source ID into Crate/Slot locations
	// PGK The tts_id_ is given to us by the CSCSV, so we don't have to do
	//  anything to get it.
	unsigned int srcID = tts_id_;
	if (srcID<748) srcID = 748;
	printf(", srcID=%d\n",srcID);

	if (srcID>830&&srcID<840) {       // crate 2 DDUs, S1-G06g, ME+
		tts_crate_ = 2;
		unsigned int islot = srcID-827; // srcID-831+4
		if (islot>7) islot++;
		tts_slot_ = islot;
	}
	else if (srcID>840&&srcID<850) {  // crate 1 DDUs, S1-G06i, ME+
		tts_crate_ = 1;
		unsigned int islot = srcID-837; // srcID-841+4
		if (islot>7) islot++;
		tts_slot_ = islot;
	}
	else if (srcID==760) { //crate ? TF-DDU, S1-?
		tts_crate_ = 3;  // JRG temp!  Later should be 5!  After ME- installed.
		tts_slot_ = 2;   // check...!
	}
	else { // set crates/slot for DCCs,
		unsigned int icrate = (srcID-748)/2; // will work for both S-Link IDs
		if (icrate>0&&icrate<5) {
			tts_crate_ = icrate;
			tts_slot_ = 8;
		}
	}

	try {
		// JRG: this is the instance for the FED application, NOT really the CrateID
		int instance = tts_crate_;

		// JRG 9/29/07: need to have unique instance for each crate fed_app process
		if (instance > 0) instance--;
		if (srcID == 760) tts_crate_=5;

		setParameter(fed_app,"ttsCrate","xsd:unsignedInt",tts_crate_.toString());
		setParameter(fed_app,"ttsSlot", "xsd:unsignedInt",tts_slot_.toString());
		setParameter(fed_app,"ttsBits", "xsd:unsignedInt",tts_bits_.toString());

		LOG4CPLUS_DEBUG(getApplicationLogger(), "TTSBits being sent to " << fed_app << "(" << instance << ")");
		LOG4CPLUS_DEBUG(getApplicationLogger(), "Crate " << tts_crate_.toString() << " Slot " << tts_slot_.toString() << " Bits " << tts_bits_.toString());

		sendCommand("SetTTSBits", fed_app, instance);

	} catch (xoap::exception::Exception e) {
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,"SOAP fault was returned", e);
		LOG4CPLUS_ERROR(getApplicationLogger(), "setParameter fault");
		//std::cout << "*!* EmuFCrateManager: inside setTTSBitsAction, setParameter fault" << std::endl;
	} catch (xdaq::exception::Exception e) {
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,"Failed to send a command", e);
		LOG4CPLUS_ERROR(getApplicationLogger(), "setParameter failed");
		//std::cout << "*!* EmuFCrateManager: inside setTTSBitsAction, setParameter failed" << std::endl;
	}

	return createReply(message);

}



xoap::MessageReference EmuFCrateManager::onDisable (xoap::MessageReference message) throw (xoap::exception::Exception)
{

	LOG4CPLUS_DEBUG(getApplicationLogger(), "Remote SOAP state change requested: Disable");

	// PGK I avoid errors at all cost.
	if (state_.toString() != "Enabled") {
		LOG4CPLUS_WARN(getApplicationLogger(), state_.toString() <<"->Configured via \"Disable\" is not a valid transition.  Fixing by doing Halted->Configured instead.");
		fireEvent("Halt");
		fireEvent("Configure");
	} else {
		fireEvent("Disable");
	}

	return createReply(message);
}



xoap::MessageReference EmuFCrateManager::onHalt (xoap::MessageReference message) throw (xoap::exception::Exception)
{

	LOG4CPLUS_DEBUG(getApplicationLogger(), "Remote SOAP state change requested: Halt");

	fireEvent("Halt");

	return createReply(message);
}



void EmuFCrateManager::sendCommand(std::string command, std::string klass)
	throw (xoap::exception::Exception, xdaq::exception::Exception)
{
	// Exceptions:
	// xoap exceptions are thrown by analyzeReply() for SOAP faults.
	// xdaq exceptions are thrown by postSOAP() for socket level errors.

	// find applications
	std::set<xdaq::ApplicationDescriptor *> apps;
	try {
		apps = getApplicationContext()->getDefaultZone()->getApplicationDescriptors(klass);
	} catch (xdaq::exception::ApplicationDescriptorNotFound e) {
		return; // Do nothing if the target doesn't exist
	}

	// prepare a SOAP message
	xoap::MessageReference message = createCommandSOAP(command);

	// send the message one-by-one
	
	for (std::set<xdaq::ApplicationDescriptor *>::iterator iApp = apps.begin(); iApp != apps.end(); iApp++) {
		// postSOAP() may throw an exception when failed.
		//xoap::MessageReference reply = getApplicationContext()->postSOAP(message, *getApplicationDescriptor(), **i);
		getApplicationContext()->postSOAP(message, *getApplicationDescriptor(), *(*iApp));
	}
}



void EmuFCrateManager::sendCommand(std::string command, std::string klass, int instance)
		throw (xoap::exception::Exception, xdaq::exception::Exception)
{
	// Exceptions:
	// xoap exceptions are thrown by analyzeReply() for SOAP faults.
	// xdaq exceptions are thrown by postSOAP() for socket level errors.

	// find applications
	//std::cout << "  * EmuFCrateManager: inside sendCommand" << std::endl;
	LOG4CPLUS_DEBUG(getApplicationLogger(), "sendCommand with parameters command("<< command <<") klass("<< klass <<") instance("<< instance <<")");
	xdaq::ApplicationDescriptor *app;
	try {
		app = getApplicationContext()->getDefaultZone()->getApplicationDescriptor(klass, instance);
	} catch (xdaq::exception::ApplicationDescriptorNotFound e) {
		LOG4CPLUS_ERROR(getApplicationLogger(), "sendCommand found no applications matching klass("<< klass <<") instance("<< instance <<")");
		return; // Do nothing if the target doesn't exist
	}

	// prepare a SOAP message
	xoap::MessageReference message = createCommandSOAP(command);
	//std::cout << "  * EmuFCrateManager: sendCommand, created Soap message" << std::endl;

	// send the message
	// postSOAP() may throw an exception when failed.
	//std::cout << "  * EmuFCrateManager: sendCommand, sending Soap message" << std::endl;
	//xoap::MessageReference reply = getApplicationContext()->postSOAP(message, *getApplicationDescriptor(), *app);
	unsigned int iTries = 5;
	while (iTries > 0) {
		try {
			getApplicationContext()->postSOAP(message, *getApplicationDescriptor(), *app);
			break;
		} catch (xoap::exception::Exception &e) {
			LOG4CPLUS_ERROR(getApplicationLogger(), "sendCommand failed sending command("<< command <<") to klass("<< klass <<") instance("<< instance <<"):  exception " << e.what());
			std::string messageOut;
			message->writeTo(messageOut);
			LOG4CPLUS_DEBUG(getApplicationLogger(), "Message was as follows:" << std::endl << messageOut);
			iTries--;
			LOG4CPLUS_DEBUG(getApplicationLogger(), iTries << " remaining.");
		} catch (xdaq::exception::Exception &e) {
			LOG4CPLUS_ERROR(getApplicationLogger(), "sendCommand failed sending command("<< command <<") to klass("<< klass <<") instance("<< instance <<"):  exception " << e.what());
			std::string messageOut;
			message->writeTo(messageOut);
			LOG4CPLUS_DEBUG(getApplicationLogger(), "Message was as follows:" << std::endl << messageOut);
			iTries--;
			LOG4CPLUS_DEBUG(getApplicationLogger(), iTries << " remaining.");
		} catch (...) {
			LOG4CPLUS_ERROR(getApplicationLogger(), "sendCommand failed sending command("<< command <<") to klass("<< klass <<") instance("<< instance <<"):  exception ???");
			std::string messageOut;
			message->writeTo(messageOut);
			LOG4CPLUS_DEBUG(getApplicationLogger(), "Message was as follows:" << std::endl << messageOut);
			iTries--;
			LOG4CPLUS_DEBUG(getApplicationLogger(), iTries << " remaining.");
		}
	}
	
	if (iTries == 0) {
		LOG4CPLUS_ERROR(getApplicationLogger(), "sendCommand maximum number of tries reached.  Setting failed state!");
		XCEPT_RAISE(toolbox::fsm::exception::Exception, "sendCommand maximum number of tries reached");
	}

}


xoap::MessageReference EmuFCrateManager::createCommandSOAP(std::string command)
{
  //std::cout << "  - EmuFCrateManager:  inside createCommandSOAP " << std::endl;
	xoap::MessageReference message = xoap::createMessage();
	xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
	xoap::SOAPName name = envelope.createName(command, "xdaq", XDAQ_NS_URI);
	envelope.getBody().addBodyElement(name);

	return message;
}

