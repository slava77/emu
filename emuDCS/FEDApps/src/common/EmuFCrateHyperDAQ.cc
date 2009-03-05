/*****************************************************************************\
* $Id: EmuFCrateHyperDAQ.cc,v 1.1 2009/03/05 16:18:24 paste Exp $
*
* $Log: EmuFCrateHyperDAQ.cc,v $
* Revision 1.1  2009/03/05 16:18:24  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
* * Added ForPageOne functionality to the Manager
*
* Revision 3.60  2009/01/29 15:31:24  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 3.59  2008/11/04 00:51:09  paste
* Fixed problems with DDU firmware uploading and broadcasting.
*
* Revision 3.57  2008/10/30 12:58:52  paste
* Fixed a minor display bug in EmuFCrateHyperDAQ.
*
* Revision 3.56  2008/10/29 16:01:44  paste
* Updated interoperability with primative DCC commands, added new xdata variables for future use.
*
* Revision 3.55  2008/10/22 20:23:58  paste
* Fixes for random FED software crashes attempted.  DCC communication and display reverted to ancient (pointer-based communication) version at the request of Jianhui.
*
* Revision 3.54  2008/10/13 11:56:40  paste
* Cleaned up some of the XML config files and scripts, added more SVG, changed the DataTable object to inherit from instead of contain stdlib objects (experimental)
*
* Revision 3.53  2008/10/09 11:21:19  paste
* Attempt to fix DCC MPROM load.  Added debugging for "Global SOAP death" bug.  Changed the debugging interpretation of certain DCC registers.  Added inline SVG to EmuFCrateManager page for future GUI use.
*
* Revision 3.51  2008/09/30 08:12:24  paste
* Fixed a bug in DDU and DCC Expert Controls
*
* Revision 3.50  2008/09/24 18:38:38  paste
* Completed new VME communication protocols.
*
* Revision 3.49  2008/09/22 14:31:54  paste
* /tmp/cvsY7EjxV
*
* Revision 3.48  2008/09/19 16:53:52  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.47  2008/09/03 17:52:59  paste
* Rebuilt the VMEController and VMEModule classes from the EMULIB_V6_4 tagged versions and backported important changes in attempt to fix "high-bits" bug.
*
* Revision 3.46  2008/09/02 08:39:53  paste
* Better handling and display of new features in the DDU firmware.
*
* Revision 3.45  2008/09/01 11:30:32  paste
* Added features to DDU, IRQThreads corresponding to new DDU firmware.
*
* Revision 3.44  2008/08/31 21:18:27  paste
* Moved buffers from VMEController class to VMEModule class for more rebust communication.
*
* Revision 3.43  2008/08/25 12:25:49  paste
* Major updates to VMEController/VMEModule handling of CAEN instructions.  Also, added version file for future RPMs.
*
* Revision 3.42  2008/08/19 14:51:02  paste
* Update to make VMEModules more independent of VMEControllers.
*
* Revision 3.41  2008/08/15 16:14:51  paste
* Fixed threads (hopefully).
*
* Revision 3.40  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "emu/fed/EmuFCrateHyperDAQ.h"

#include <fstream>
#include <iomanip>

#include "cgicc/HTMLClasses.h"
#include "emu/fed/DDUDebugger.h"
#include "emu/fed/DCCDebugger.h"
#include "emu/fed/DataTable.h"
#include "emu/fed/CrateParser.h"
#include "emu/fed/Crate.h"
#include "emu/fed/DDU.h"
#include "emu/fed/Chamber.h"
#include "emu/fed/DCC.h"
#include "emu/fed/VMEController.h"
#include "emu/fed/JTAG_constants.h"

//#include "CAENVMElib.h"

XDAQ_INSTANTIATOR_IMPL(emu::fed::EmuFCrateHyperDAQ)


emu::fed::EmuFCrateHyperDAQ::EmuFCrateHyperDAQ(xdaq::ApplicationStub *stub):
Application(stub)
{
	
	xgi::bind(this,&emu::fed::EmuFCrateHyperDAQ::webDefault, "Default");
	xgi::bind(this,&emu::fed::EmuFCrateHyperDAQ::mainPage, "mainPage");
	xgi::bind(this,&emu::fed::EmuFCrateHyperDAQ::configurePage, "configurePage");
	xgi::bind(this,&emu::fed::EmuFCrateHyperDAQ::setConfFile, "setConfFile");
	xgi::bind(this,&emu::fed::EmuFCrateHyperDAQ::setRawConfFile, "setRawConfFile");
	xgi::bind(this,&emu::fed::EmuFCrateHyperDAQ::uploadConfFile, "uploadConfFile");
	
	xgi::bind(this,&emu::fed::EmuFCrateHyperDAQ::DDUBroadcast, "DDUBroadcast");
	xgi::bind(this,&emu::fed::EmuFCrateHyperDAQ::DDULoadBroadcast, "DDULoadBroadcast");
	xgi::bind(this,&emu::fed::EmuFCrateHyperDAQ::DDUSendBroadcast, "DDUSendBroadcast");
	xgi::bind(this,&emu::fed::EmuFCrateHyperDAQ::DDUReset, "DDUReset");
	xgi::bind(this,&emu::fed::EmuFCrateHyperDAQ::DCCReset, "DCCReset");

	xgi::bind(this,&emu::fed::EmuFCrateHyperDAQ::DDUDebug, "DDUDebug");
	xgi::bind(this,&emu::fed::EmuFCrateHyperDAQ::DDUExpert, "DDUExpert");
	xgi::bind(this,&emu::fed::EmuFCrateHyperDAQ::InFpga, "InFpga");
	xgi::bind(this,&emu::fed::EmuFCrateHyperDAQ::VMEPARA, "VMEPARA");
	xgi::bind(this,&emu::fed::EmuFCrateHyperDAQ::VMESERI, "VMESERI");
	xgi::bind(this,&emu::fed::EmuFCrateHyperDAQ::DDUTextLoad, "DDUTextLoad");
	
	xgi::bind(this,&emu::fed::EmuFCrateHyperDAQ::DCCBroadcast, "DCCBroadcast");
	xgi::bind(this,&emu::fed::EmuFCrateHyperDAQ::DCCLoadBroadcast, "DCCLoadBroadcast");
	xgi::bind(this,&emu::fed::EmuFCrateHyperDAQ::DCCSendBroadcast, "DCCSendBroadcast");
	
	xgi::bind(this,&emu::fed::EmuFCrateHyperDAQ::DCCDebug,"DCCDebug");
	xgi::bind(this,&emu::fed::EmuFCrateHyperDAQ::DCCExpert,"DCCExpert");
	xgi::bind(this,&emu::fed::EmuFCrateHyperDAQ::DCCTextLoad, "DCCTextLoad");
	
	xgi::bind(this,&emu::fed::EmuFCrateHyperDAQ::DDUVoltMon,"DDUVoltMon");

	getApplicationInfoSpace()->fireItemAvailable("xmlFileName",&xmlFile_);

}




void emu::fed::EmuFCrateHyperDAQ::webDefault(xgi::Input *in, xgi::Output *out)
{

	// PGK Should the vectors not be filled, this means we have
	//  not been properly configured.  We should inform the user about this.
	if (crateVector_.size() == 0) {
		LOG4CPLUS_INFO(getApplicationLogger(), "This instance of the HyperDAQ has not yet read in its XML file.  Initializing.");
		try {
			Configuring(); // Defaults should be sufficient.
		} catch (xcept::Exception &e) {
			std::ostringstream error;
			error << "Exception caught!";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
			notifyQualified("ERROR", e2);
			
			std::vector<xcept::ExceptionInformation> history = e2.getHistory();
			for (std::vector<xcept::ExceptionInformation>::iterator iError = history.begin(); iError != history.end(); iError++) {
				DataTable exceptionTable;
				unsigned int iRow = 0;
				std::map<std::string, std::string> messages = iError->getProperties();
				for (std::map<std::string, std::string>::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++) {
					exceptionTable(iRow, 0) << iMessage->first << ":";
					exceptionTable(iRow, 1) << iMessage->second;
					iRow++;
				}
				*out << cgicc::div(exceptionTable.toHTML())
					.set("class", "exception");
			}
		}
	}

	if (crateVector_.size() != 0) {
		
		std::ostringstream redirect;
		redirect << "mainPage?crate=" << crateVector_[0]->number();
		return webRedirect(out, redirect.str());
		
	} else {
		*out << cgicc::div("No crates could be found.  This is probably due to a bad configuration file.  Check your configuration file (" + xmlFile_.toString() + "), then try reloading this page.")
			.set("class", "exception");
	}

}



void emu::fed::EmuFCrateHyperDAQ::mainPage(xgi::Input *in, xgi::Output *out)
{
	
	try {
		// PGK Patented check-for-initialization
		if (crateVector_.size()==0) {
			LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
			return Default(in,out);
		}

		cgicc::Cgicc cgi(in);

		// First, I need a crate.
		// This is the main page, so don't die if there actually are crates from which to choose.
		// Just pick a reasonable crate.
		std::pair<unsigned int, Crate *> cratePair = getCGICrate(cgi);
		unsigned int cgiCrate = cratePair.first;
		Crate *myCrate = cratePair.second;
		
		std::stringstream sTitle;
		sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ")";
		std::vector<std::string> jsFileNames;
		jsFileNames.push_back("errorFlasher.js");
		*out << Header(sTitle.str(),jsFileNames);


		// Check for errors in crates.  Should only have crate numbers 1-5,
		// and there should not be more than one crate with a given number.
		// The crates should also have a unique VME controller.
		int crateError = 0;
		for (unsigned int iCrate = 0; iCrate < crateVector_.size(); iCrate++) {
			if (crateVector_[iCrate]->number() > 5 || crateVector_[iCrate]->number() < 1) {
				*out << cgicc::div().set("style","background-color: #000; color: #FAA; font-weight: bold; margin-bottom: 0px;") << "Crate " << crateVector_[iCrate]->number() << " has an invalid number (should be 1-5)" << cgicc::div() << std::endl;
				crateError++;
			}
			for (unsigned int jCrate=iCrate+1; jCrate<crateVector_.size(); jCrate++) {
				if (crateVector_[iCrate]->number() == crateVector_[jCrate]->number()) {
					*out << cgicc::div().set("style","background-color: #000; color: #FAA; font-weight: bold; margin-bottom: 0px;") << "Two crates share crate number " << crateVector_[iCrate]->number() << cgicc::div() << std::endl;
					crateError++;
				}
				if (crateVector_[iCrate]->getController()->getDevice() == crateVector_[jCrate]->getController()->getDevice()) {
					*out << cgicc::div().set("style","background-color: #000; color: #FAA; font-weight: bold; margin-bottom: 0px;") << "Crates " << crateVector_[iCrate]->number() << " and " << crateVector_[jCrate]->number() << " have the same VME controller device number (" << crateVector_[iCrate]->getController()->getDevice() << ")" << cgicc::div() << std::endl;
					crateError++;
				}
			}
		}
		if (crateError) {
			*out << cgicc::div().set("style","background-color: #000; color: #FAA; font-weight: bold; margin-bottom: 0px;") << "You have " << crateError << " error" << (crateError != 1 ? "s" : "") << " in your XML configuration file.  MAKE SURE YOU UNDERSTAND WHAT YOU ARE DOING BEFORE CONTINUING WITH THIS CONFIGURATION.  If you did not expect this message, fix your configuration file and reload it with the button at the bottom of the page." << cgicc::div() << std::endl;
		}

		// PGK Select-a-Crate/Slot
		*out << cgicc::fieldset()
			.set("class","fieldset") << std::endl;
		*out << cgicc::div("View this page for a different crate/board")
			.set("class","legend") << std::endl;

		*out << selectACrate("mainPage","crate",cgiCrate,cgiCrate) << std::endl;
		
		*out << cgicc::span("Configuration located at " + xmlFile_.toString())
			.set("style","color: #A00; font-size: 10pt;") << std::endl;

		*out << cgicc::fieldset() << std::endl;
		*out << cgicc::br() << std::endl;

		// PGK Crate monitoring tools
		//DDU_=-99; // ?
		*out << cgicc::fieldset()
			.set("class","fieldset") << std::endl;
		*out << cgicc::div("Crate Monitoring Tools")
			.set("class","legend") << std::endl;

		std::ostringstream location;
		location << "/" + getApplicationDescriptor()->getURN() << "/DDUVoltMon?crate=" << cgiCrate;
		*out << cgicc::a("Start DDU Volt/Temp Monitor")
			.set("href",location.str());
		*out << cgicc::br() << std::endl;

		//location.str("");
		//location << "/" + getApplicationDescriptor()->getURN() << "/DCCRateMon?crate=" << cgiCrate;
		// *out << cgicc::span("Start DCC Rate Monitor")
		//	.set("class","undefined");
		//*out << cgicc::a("Start DDU Volt/Temp Monitor")
		//	.set("href",location.str());
		// *out << cgicc::br() << std::endl;

		*out << cgicc::fieldset() << std::endl;

		// Table sorted by slot...
		*out << cgicc::fieldset()
			.set("class","fieldset") << std::endl;
		*out << cgicc::div("Board Communication")
			.set("class","legend");

		// Loop over all the DDUs
		std::vector<DDU *> myDDUs = myCrate->getDDUs();
		for (std::vector<DDU *>::iterator iDDU = myDDUs.begin(); iDDU != myDDUs.end(); iDDU++) {

			// First, determine the status of the DDU.
			//myCrate->getVMEController()->CAEN_err_reset();
			// Do a fast FMM status check
			//unsigned short int DDU_FMM = (((*iDDU)->vmepara_status()>>8)&0x000F);
			unsigned short int DDU_FMM = (((*iDDU)->readParallelStatus()>>8)&0x000F);
			//unsigned short int DDU_FMM = 8; // DEBUG
			// Mark the status with pretty colors
			std::string fmmClass = "green";
			std::string statusClass = "ok";
			if (DDU_FMM==4) { // Busy
				fmmClass = "orange";
			} else if (DDU_FMM==1) { // Warn, near full: reduce trigger rate
				fmmClass = "yellow";
			} else if (DDU_FMM==8) { // Ready
				fmmClass = "green";
			} else if (DDU_FMM==2 || DDU_FMM==0xC) { // Error or Sync
				fmmClass = "red";
				statusClass = "error";
			} else {
				fmmClass = "undefined";
			}

			// Check for CSC status
			//unsigned short int status = (*iDDU)->vmepara_CSCstat();
			unsigned int fibersWithErrors = (*iDDU)->readCSCStatus();
			unsigned int advancedErrors = (*iDDU)->readAdvancedFiberErrors();
			//unsigned short int status = 0; // DEBUG
			// Mark the status with pretty colors
			std::string cscClass = "green";
			//if (myCrate->getVMEController()->CAEN_err() != 0) {
				//cscClass = "yellow";
			//} else
			if ((fibersWithErrors | advancedErrors) == 0x0000) {
				cscClass = "green";
			} else {
				cscClass = "red";
				statusClass = "error";
			}

			// Next, print the table with the status.
			*out << cgicc::table()
				.set("style","width: 90%; border: 2px solid #000; margin: 5px auto 5px auto; text-align: center; border-collapse: collapse;") << std::endl;

			// First row:  status line
			*out << cgicc::tr() << std::endl;
			// The first element is special:  board type
			*out << cgicc::td()
				.set("class",statusClass)
				.set("style","border-right: 1px solid #000; font-weight: bold; width: 10%;");
			*out << "DDU";
			*out << cgicc::td() << std::endl;
			*out << cgicc::td()
				.set("class",fmmClass)
				.set("style","border-bottom: 1px solid #000;")
				.set("colspan","5");
			*out << "FMM Status: " << std::uppercase << std::setw(1) << std::hex << DDU_FMM << std::dec << "h";
			*out << cgicc::td() << std::endl;
			*out << cgicc::td()
				.set("class",cscClass)
				.set("style","border-bottom: 1px solid #000;")
				.set("colspan","5");
			*out << "CSC/Adv Status: " << std::uppercase << std::setw(4) << std::hex << fibersWithErrors << "/" << std::uppercase << std::setw(4) << advancedErrors << std::dec << "h";
			*out << cgicc::td() << std::endl;
			*out << cgicc::td()
				.set("class","none")
				.set("style","border-bottom: 1px solid #000;")
				.set("colspan","5");
			*out << "DDU L1As: " << std::dec << (*iDDU)->readL1Scaler(DDUFPGA);
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;

			// Second row:  fiber input numbers
			*out << cgicc::tr()
				.set("style","") << std::endl;
			// The first element is special:  slot
			*out << cgicc::td()
				.set("class",statusClass)
				.set("style","border-right: 1px solid #000; font-weight: bold; width: 10%;");
			*out << "Slot " << (*iDDU)->slot();
			*out << cgicc::td() << std::endl;

			// Knowing which chambers are actually alive is a good thing.
			long int liveFibers = ((*iDDU)->readFiberStatus(INFPGA0)&0x000000ff) | (((*iDDU)->readFiberStatus(INFPGA1)&0x000000ff)<<8);
			long int killFiber = (*iDDU)->readKillFiber();

			for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
				std::string chamberClass = "ok";
				if (!(killFiber & (1<<iFiber))) chamberClass = "none";
				else if (!(liveFibers & (1<<iFiber))) chamberClass = "undefined";
				else if ((fibersWithErrors | advancedErrors) & (1<<iFiber)) chamberClass = "bad";

				*out << cgicc::td()
					.set("class",chamberClass)
					.set("style","border: 1px solid #000; border-bottom-width: 0px; font-size: 8pt; width: 6%;") << iFiber << cgicc::td() << std::endl;
			}

			*out << cgicc::tr() << std::endl;
			
			// Third row:  chamber names and statuses
			*out << cgicc::tr()
					.set("style","") << std::endl;
			// The first element is special:  RUI
			std::stringstream ruiNumberStream;
			ruiNumberStream << myCrate->getRUI((*iDDU)->slot());
			std::string ruiString = ruiNumberStream.str();
			// This part is terrible.
			std::string ruiFormGetString = "rui1="+ruiString+"&ddu_input1=&ddu1=&fed_crate1=&ddu_slot1=&dcc_fifo1=&slink1=&fiber_crate1=&fiber_pos1=&fiber_socket1=&crateid1=&cratelabel1=&dmb_slot1=&chamberlabel1=&chamberid1=&rui2=&ddu2=&fed_crate2=&ddu_slot2=&ddu_input2=&dcc_fifo2=&slink2=&fiber_crate2=&fiber_pos2=&fiber_socket2=&crateid2=&cratelabel2=&dmb_slot2=&chamberlabel2=&chamberid2=&switch=ddu_chamber&chamber2=";
			*out << cgicc::td()
					.set("class",statusClass)
					.set("style","border-right: 1px solid #000; font-weight: bold; width: 10%;");
			*out << cgicc::a("RUI #"+ruiString)
					.set("href","http://oraweb.cern.ch/pls/cms_emu_cern.pro/ddumap.web?"+ruiFormGetString);
			*out << cgicc::td() << std::endl;
			
			// Loop through the chambers.  They should be in fiber-order.
			for (unsigned int iFiber=0; iFiber<15; iFiber++) {
				Chamber *thisChamber = (*iDDU)->getChamber(iFiber);
				// DDU::getChamber will return a null pointer if there is
				//  no chamber at that fiber position.
				std::string chamberClass = "ok";
				if (thisChamber != NULL) {
					if (!(killFiber & (1<<iFiber))) chamberClass = "none";
					else if (!(liveFibers & (1<<iFiber))) chamberClass = "undefined";
					else if ((fibersWithErrors | advancedErrors) & (1<<iFiber)) chamberClass = "bad";

					*out << cgicc::td(thisChamber->name())
						.set("class",chamberClass)
						.set("style","border: 1px solid #000; border-top-width: 0px; font-size: 8pt; width: 6%;") << std::endl;
				} else {
					*out << cgicc::td("???")
						.set("class","undefined")
						.set("style","border: 1px solid #000; border-top-width: 0px; font-size: 8pt; width: 6%;") << std::endl;
				}
			}

			*out << cgicc::tr() << std::endl;

			// Fourth row:  commands
			*out << cgicc::tr()
				.set("style","") << std::endl;
			// The first element is blank
			*out << cgicc::td()
				.set("class",statusClass)
				.set("style","border-right: 1px solid #000; font-weight: bold; width: 10%;");
			*out << cgicc::td() << std::endl;

			// Everything else is one big row.
			*out << cgicc::td()
				.set("colspan","15")
				.set("style","font-size: 10pt;") << std::endl;

			// Create the buttons with a big loop.
			std::string appString[5] = {
				"/" + getApplicationDescriptor()->getURN() + "/DDUDebug",
				"/" + getApplicationDescriptor()->getURN() + "/InFpga",
				"/" + getApplicationDescriptor()->getURN() + "/DDUExpert",
				"/" + getApplicationDescriptor()->getURN() + "/VMEPARA",
				"/" + getApplicationDescriptor()->getURN() + "/VMESERI"
			};

			std::string appName[5] = {
				"Debug DDU Status",
				"Individual Fiber Debugging",
				"Expert DDU Commands",
				"VME Parallel",
				"VME Serial"
			};

			for (unsigned int iButton = 0; iButton < 5; iButton++) {
				// Jason likes a space after the first few buttons.
				std::stringstream location;
				location << appString[iButton] << "?crate=" << cgiCrate << "&slot=" << (*iDDU)->slot();
				if (iButton<2) *out << cgicc::a(appName[iButton])
					.set("href",location.str())
					.set("style","float: left; padding: 3px;") << std::endl;
				else *out << cgicc::a(appName[iButton])
					.set("href",location.str())
					.set("style","float: right; padding: 3px;") << std::endl;
			}

			*out << cgicc::br()
				.set("style","clear:both") << std::endl;

			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;

			*out << cgicc::table();

		}
		
		// Loop over all the DCCs
		std::vector<DCC *> myDCCs = myCrate->getDCCs();
		for (std::vector<DCC *>::iterator iDCC = myDCCs.begin(); iDCC != myDCCs.end(); iDCC++) {

			// First, determine the status of the DCC.
			//myCrate->getVMEController()->CAEN_err_reset();
			unsigned short int statush = (*iDCC)->readStatusHigh();
			unsigned short int statusl = (*iDCC)->readStatusLow();
			//unsigned short int rdfifoinuse = (*iDCC)->readFIFOInUse();
			std::string status;
			// Pretty colors!
			std::string statusClass = "ok";
			std::string dccStatus = "green";
			if ((statush&0xf000)==0x2000) {
				status = "(Ready)";
			} else if ((statush&0xf000)==0x4000) {
				dccStatus = "orange";
				statusClass = "warning";
				status ="(Warning)";
			} else if ((statush&0xf000)==0x8000 || (statush&0xf000)==0x3000 ) {
				dccStatus = "red";
				statusClass = "error";
				status ="(Out of sync or error)";
			} else if ((statush&0xf000)==0x1000) {
				dccStatus = "yellow";
				statusClass = "caution";
				status ="(Busy)";
			} else {
				dccStatus = "undefined";
				statusClass = "undefined";
				status ="(CAEN error)";
			}

			// Next, print the table with the status.
			*out << cgicc::table()
				.set("style","width: 90%; border: 3px double #000; margin: 5px auto 5px auto; text-align: center; border-collapse: collapse;") << std::endl;

			// First row:  status line
			*out << cgicc::tr() << std::endl;
			// The first element is special:  board type
			*out << cgicc::td()
				.set("class",statusClass)
				.set("style","border-right: 1px solid #000; font-weight: bold; width: 10%;");
			*out << "DCC";
			*out << cgicc::td() << std::endl;
			*out << cgicc::td()
				.set("class",dccStatus)
				.set("style","border-bottom: 1px solid #000; width: 18%;");
			*out << "FMM: " << std::uppercase << std::setw(1) << std::hex << ((statush>>12)&0xf) << std::dec << "h " << status;
			*out << cgicc::td() << std::endl;
			*out << cgicc::td()
				.set("class",dccStatus)
				.set("style","border-bottom: 1px solid #000; width: 18%;");
			*out << "S-Link Status: " << std::uppercase << std::setw(1) << std::hex << (statush&0xf) << std::dec << "h";
			*out << cgicc::td() << std::endl;
			*out << cgicc::td()
				.set("class",dccStatus)
				.set("style","border-bottom: 1px solid #000; width: 18%;");
			*out << "InFIFO full: " << std::uppercase << std::setw(2) << std::hex << ((statush>>4)&0xff) << std::dec << "h";
			*out << cgicc::td() << std::endl;
			*out << cgicc::td()
				.set("class",dccStatus)
				.set("style","border-bottom: 1px solid #000; width: 18%;");
			*out << "L1As: " << std::dec << (statusl&0xffff);
			*out << cgicc::td() << std::endl;

			*out << cgicc::tr() << std::endl;

			// Second row:  commands
			*out << cgicc::tr()
				.set("style","") << std::endl;
			// The first element is special:  slot
			*out << cgicc::td()
				.set("class",statusClass)
				.set("style","border-right: 1px solid #000; font-weight: bold; width: 10%;");
			*out << "Slot " << (*iDCC)->slot();
			*out << cgicc::td() << std::endl;

			// Everything else is one big row.
			*out << cgicc::td()
				.set("colspan","5")
				.set("style","font-size: 10pt;") << std::endl;

			// Create the buttons with a big loop.
			std::string appString[2] = {
				"/" + getApplicationDescriptor()->getURN() + "/DCCDebug",
				"/" + getApplicationDescriptor()->getURN() + "/DCCExpert"
			};

			std::string appName[2] = {
				"Debug DCC Status",
				"Expert DCC Commands"
			};

			for (unsigned int iButton = 0; iButton < 2; iButton++) {
				std::stringstream location;
				location << appString[iButton] << "?crate=" << cgiCrate << "&slot=" << (*iDCC)->slot();
				if (iButton==0) *out << cgicc::a(appName[iButton]).set("style","float: left; padding: 3px;")
					.set("href",location.str()) << std::endl;
				else *out << cgicc::a(appName[iButton]).set("style","float: right; padding: 3px;")
					.set("href",location.str()) << std::endl;
			}

			*out << cgicc::br()
				.set("clear","both") << std::endl;
			*out << cgicc::td() << std::endl;
			*out << cgicc::tr() << std::endl;


			*out << cgicc::table();

		} // end VME Module loop.

		*out << std::endl;
		*out << cgicc::fieldset() << std::endl;
		

		*out << cgicc::fieldset()
			.set("class","fieldset") << std::endl;
		*out << cgicc::div("Firmware Management")
			.set("class","legend");

		// DDU buttons.
		if (myCrate->getDDUs().size()) {

			// Broadcast Firmware
			*out << cgicc::span() << std::endl;
			std::ostringstream crateVal;
			crateVal << cgiCrate;
			location.str("");
			location << "/" << getApplicationDescriptor()->getURN() << "/DDUBroadcast";
			*out << cgicc::form()
				.set("method","GET")
				.set("action",location.str()) << std::endl;
			*out << cgicc::input()
				.set("type","hidden")
				.set("name","crate")
				.set("value",crateVal.str());
			*out << cgicc::input()
				.set("type","submit")
				.set("value","DDU Firmware Management") << std::endl;
			*out << cgicc::form() << std::endl;
			*out << cgicc::span() << std::endl;

		}
		
		// DCC buttons
		if (myCrate->getDCCs().size()) {
			
			// Broadcast Firmware
			*out << cgicc::span() << std::endl;
			std::ostringstream crateVal;
			crateVal << cgiCrate;
			location.str("");
			location << "/" << getApplicationDescriptor()->getURN() << "/DCCBroadcast";
			*out << cgicc::form()
			.set("method","GET")
			.set("action",location.str()) << std::endl;
			*out << cgicc::input()
			.set("type","hidden")
			.set("name","crate")
			.set("value",crateVal.str());
			*out << cgicc::input()
			.set("type","submit")
			.set("value","DCC Firmware Management") << std::endl;
			*out << cgicc::form() << std::endl;
			*out << cgicc::span() << std::endl;
			
		}


		*out << cgicc::fieldset() << std::endl;

		*out << cgicc::br() << std::endl;

		// PGK Useful information for users.
		*out << cgicc::fieldset()
			.set("class","fieldset") << std::endl;
		*out << cgicc::div("Useful Links")
			.set("class","legend") << std::endl;

		*out << cgicc::ul() << std::endl;
		*out << cgicc::li() << std::endl;
		*out << cgicc::a("DDU Documentation")
			.set("href","http://www.physics.ohio-state.edu/~cms/ddu") << std::endl;
		*out << cgicc::li() << std::endl;
		*out << cgicc::li() << std::endl;
		*out << cgicc::a("DCC Documentation")
			.set("href","http://www.physics.ohio-state.edu/~cms/dcc") << std::endl;
		*out << cgicc::li() << std::endl;
		*out << cgicc::li() << std::endl;
		*out << cgicc::a("FED Crate Documentation")
			.set("href","http://www.physics.ohio-state.edu/~cms/fed") << std::endl;
		*out << cgicc::li() << std::endl;
		*out << cgicc::li() << std::endl;
		*out << cgicc::a("OSU Firmware Repository")
			.set("href","http://www.physics.ohio-state.edu/~cms/firmwares") << std::endl;
		*out << cgicc::li() << std::endl;
		*out << cgicc::ul() << std::endl;

		*out << cgicc::fieldset() << std::endl;

		*out << cgicc::br() << std::endl;
		// JRG, inside end of "normal" DDU/DCC Control top-level page
		//   --add "Go to XML Reload" button here for Martin:
		//*out << "<blockquote> &nbsp; &nbsp; </blockquote>" << std::endl;
		//*out << "<P> &nbsp; <P> &nbsp; <P> &nbsp; <P> &nbsp; <P> &nbsp;" << std::endl;
		*out << cgicc::hr();

		std::string loadxmlconf = "/" + getApplicationDescriptor()->getURN() + "/configurePage";

		*out << cgicc::fieldset()
			.set("class","expert") << std::endl;
		*out << cgicc::div("Experts Only")
			.set("class","legend") << std::endl;

		*out << cgicc::form().set("style","display: inline;")
			.set("method","POST")
			.set("action",loadxmlconf) << std::endl;
		*out << cgicc::input().set("type","hidden")
			.set("name","Reload")
			.set("value","1");
		*out << std::endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("value","Reload XMLconfig") << std::endl;
		*out << cgicc::form();

		*out << cgicc::fieldset() << std::endl;

		*out << Footer();

	} catch (xcept::Exception &e) {
		std::ostringstream error;
		error << "Exception caught!";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
		
		std::vector<xcept::ExceptionInformation> history = e2.getHistory();
		for (std::vector<xcept::ExceptionInformation>::iterator iError = history.begin(); iError != history.end(); iError++) {
			DataTable exceptionTable;
			unsigned int iRow = 0;
			std::map<std::string, std::string> messages = iError->getProperties();
			for (std::map<std::string, std::string>::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++) {
				exceptionTable(iRow, 0) << iMessage->first << ":";
				exceptionTable(iRow, 1) << iMessage->second;
				iRow++;
			}
			*out << cgicc::div(exceptionTable.toHTML())
				.set("class", "exception");
		}
	}
}



void emu::fed::EmuFCrateHyperDAQ::configurePage(xgi::Input *in, xgi::Output *out)
{

	try {

		/* PGK I hate fieldsets. */
		*out << cgicc::div("Choose one of the following configuration methods").set("class","title") << std::endl;

		/* Use a local file */
		std::string method =
		toolbox::toString("/%s/setConfFile",getApplicationDescriptor()->getURN().c_str());

		*out << cgicc::fieldset()
			.set("class","fieldset") << std::endl;
		*out << cgicc::div("Use a file on the server").set("class","legend");

		*out << cgicc::form()
			.set("method","POST")
			.set("action",method) << std::endl;
		*out << cgicc::span("Absolute path on server: ") << std::endl;
		*out << cgicc::input().set("type","text")
			.set("name","xmlFilename")
			.set("size","60")
			.set("ENCTYPE","multipart/form-data")
			.set("value",xmlFile_) << std::endl;
		*out << cgicc::input().set("type","submit").set("name","buttonid")
			.set("value","Initialize and Configure") << std::endl;
		*out << cgicc::form() << std::endl;
		*out << cgicc::div("Note: FCrate Configuration always supersedes HyperDAQ Configuration.")
			.set("style","width: 100%; color: #FF0000; font-size: 11pt; font-style: italic; text-align: right;") << std::endl;

		*out << cgicc::fieldset() << std::endl;

		/* Upload file */
		method =
		toolbox::toString("/%s/uploadConfFile",getApplicationDescriptor()->getURN().c_str());

		*out << cgicc::fieldset()
			.set("class","fieldset") << std::endl;
		*out << cgicc::div("Upload a local file")
			.set("class","legend");

		*out << cgicc::form().set("method","POST")
			.set("enctype","multipart/form-data")
			.set("action",method) << std::endl;
		*out << cgicc::span("Local path (use [Browse] button): ") << std::endl;
		*out << cgicc::input().set("type","file")
			.set("name","xmlFilenameUpload")
			.set("size","60") << std::endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("value","Initialize and Configure") << std::endl;
		*out << cgicc::form() << std::endl;

		*out << cgicc::fieldset() << std::endl;

		/* Roll your own */
		method =
		toolbox::toString("/%s/setRawConfFile",getApplicationDescriptor()->getURN().c_str());

		*out << cgicc::fieldset()
			.set("class","fieldset") << std::endl;
		*out << cgicc::div("Type a custom configuration")
			.set("class","legend");

		*out << cgicc::form().set("method","POST")
			.set("action",method) << std::endl;
		*out << cgicc::textarea("(Type or paste configuration here)").set("name","Text")
			.set("WRAP","OFF")
			.set("rows","20").set("cols","60") << std::endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("value","Initialize and Configure");
		*out << cgicc::form() << std::endl;

		*out << cgicc::fieldset() << std::endl;

		*out << cgicc::body() << std::endl;
		*out << cgicc::html() << std::endl;

	} catch (xcept::Exception &e) {
		std::ostringstream error;
		error << "Exception caught!";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
		
		std::vector<xcept::ExceptionInformation> history = e2.getHistory();
		for (std::vector<xcept::ExceptionInformation>::iterator iError = history.begin(); iError != history.end(); iError++) {
			DataTable exceptionTable;
			unsigned int iRow = 0;
			std::map<std::string, std::string> messages = iError->getProperties();
			for (std::map<std::string, std::string>::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++) {
				exceptionTable(iRow, 0) << iMessage->first << ":";
				exceptionTable(iRow, 1) << iMessage->second;
				iRow++;
			}
			*out << cgicc::div(exceptionTable.toHTML())
				.set("class", "exception");
		}
	}
}



void emu::fed::EmuFCrateHyperDAQ::setRawConfFile(xgi::Input *in, xgi::Output *out)
{

	try {
		LOG4CPLUS_DEBUG(getApplicationLogger(), "Saving input as a raw configuration file");

		cgicc::Cgicc cgi(in);

		cgicc::const_form_iterator fileData = cgi["Text"];
		
		if (fileData != cgi.getElements().end()) {
			std::ofstream TextFile;
			TextFile.open("MyTextConfigurationFile.xml", std::fstream::trunc);
			if (TextFile.is_open()) {
				TextFile << (fileData->getValue());
				TextFile.close();
				xmlFile_ = "MyTextConfigurationFile.xml";
			} else {
				std::ostringstream error;
				error << "Could not write configuration data to disk";
				LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
				XCEPT_DECLARE(emu::fed::exception::FileException, e, error.str());
				notifyQualified("ERROR", e);
			}
		} else {
			std::ostringstream error;
			error << "Could not read configuration data from CGI input";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			XCEPT_DECLARE(emu::fed::exception::FileException, e, error.str());
			notifyQualified("ERROR", e);
		}

		LOG4CPLUS_DEBUG(getApplicationLogger(), "Clearing crate vector to clear configuration");
		crateVector_.clear();

		return Default(in, out);
		
	} catch (xcept::Exception &e) {
		std::ostringstream error;
		error << "Exception caught!";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
		
		std::vector<xcept::ExceptionInformation> history = e2.getHistory();
		for (std::vector<xcept::ExceptionInformation>::iterator iError = history.begin(); iError != history.end(); iError++) {
			DataTable exceptionTable;
			unsigned int iRow = 0;
			std::map<std::string, std::string> messages = iError->getProperties();
			for (std::map<std::string, std::string>::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++) {
				exceptionTable(iRow, 0) << iMessage->first << ":";
				exceptionTable(iRow, 1) << iMessage->second;
				iRow++;
			}
			*out << cgicc::div(exceptionTable.toHTML())
				.set("class", "exception");
		}
	}

}



void emu::fed::EmuFCrateHyperDAQ::setConfFile(xgi::Input *in, xgi::Output *out)
{

	try {
		LOG4CPLUS_DEBUG(getApplicationLogger(), "Setting a new configuration file for use");
		
		cgicc::Cgicc cgi(in);

		cgicc::const_form_iterator fileName = cgi["xmlFileName"];

		if (fileName != cgi.getElements().end()) {
			xmlFile_ = fileName->getValue();
		} else {
			std::ostringstream error;
			error << "Could not read a file name from CGI input";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			XCEPT_DECLARE(emu::fed::exception::FileException, e, error.str());
			notifyQualified("ERROR", e);
		}

		LOG4CPLUS_DEBUG(getApplicationLogger(), "Clearing crate vector to clear configuration");
		crateVector_.clear();

		return Default(in, out);
	} catch (xcept::Exception &e) {
		std::ostringstream error;
		error << "Exception caught!";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
		
		std::vector<xcept::ExceptionInformation> history = e2.getHistory();
		for (std::vector<xcept::ExceptionInformation>::iterator iError = history.begin(); iError != history.end(); iError++) {
			DataTable exceptionTable;
			unsigned int iRow = 0;
			std::map<std::string, std::string> messages = iError->getProperties();
			for (std::map<std::string, std::string>::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++) {
				exceptionTable(iRow, 0) << iMessage->first << ":";
				exceptionTable(iRow, 1) << iMessage->second;
				iRow++;
			}
			*out << cgicc::div(exceptionTable.toHTML())
				.set("class", "exception");
		}
	}
}



void emu::fed::EmuFCrateHyperDAQ::uploadConfFile(xgi::Input *in, xgi::Output *out)
{
	try {
		LOG4CPLUS_DEBUG(getApplicationLogger(), "Uploading a new configuration file for use");
		
		cgicc::Cgicc cgi(in);
		
		cgicc::const_file_iterator file = cgi.getFile("xmlFileNameUpload");

		if (file != cgi.getFiles().end()) {
			std::ofstream TextFile;
			TextFile.open("MyTextConfigurationFile.xml", std::ofstream::trunc);
			if (TextFile.is_open()) {
				file->writeToStream(TextFile);
				TextFile.close();
				xmlFile_ = "MyTextConfigurationFile.xml";
			} else {
				std::ostringstream error;
				error << "Could not write configuration file to disk";
				LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
				XCEPT_DECLARE(emu::fed::exception::FileException, e, error.str());
				notifyQualified("ERROR", e);
			}
		} else {
			std::ostringstream error;
			error << "Could not read file name from CGI input";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			XCEPT_DECLARE(emu::fed::exception::FileException, e, error.str());
			notifyQualified("ERROR", e);
		}

		LOG4CPLUS_DEBUG(getApplicationLogger(), "Clearing crate vector to clear configuration");
		crateVector_.clear();
		
		return Default(in, out);
	} catch (xcept::Exception &e) {
		std::ostringstream error;
		error << "Exception caught!";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
		
		std::vector<xcept::ExceptionInformation> history = e2.getHistory();
		for (std::vector<xcept::ExceptionInformation>::iterator iError = history.begin(); iError != history.end(); iError++) {
			DataTable exceptionTable;
			unsigned int iRow = 0;
			std::map<std::string, std::string> messages = iError->getProperties();
			for (std::map<std::string, std::string>::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++) {
				exceptionTable(iRow, 0) << iMessage->first << ":";
				exceptionTable(iRow, 1) << iMessage->second;
				iRow++;
			}
			*out << cgicc::div(exceptionTable.toHTML())
				.set("class", "exception");
		}
	}
}



void emu::fed::EmuFCrateHyperDAQ::Configuring()
throw (emu::fed::exception::ParseException)
{

	LOG4CPLUS_DEBUG(getApplicationLogger(), "Parsing file " << xmlFile_.toString());
	
	try {
		CrateParser parser(xmlFile_.toString().c_str());
		
		crateVector_ = parser.getCrates();
		
		// Get the name of this endcap from the parser, too.  This is specified in the XML
		// for convenience.
		endcap_ = parser.getName();
		
	} catch (emu::fed::exception::SoftwareException &e) {
		std::ostringstream error;
		error << "Error parsing file " << xmlFile_.toString();
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}

}



void emu::fed::EmuFCrateHyperDAQ::DDUBroadcast(xgi::Input *in, xgi::Output *out)
{

	// PGK Patented check-for-initialization
	if (crateVector_.size()==0) {
		LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
		return Default(in,out);
	}

	cgicc::Cgicc cgi(in);
	
	// First, I need a crate.
	unsigned int cgiCrate = 0;
	Crate *myCrate = NULL;
	
	try {
		std::pair<unsigned int, Crate *> cratePair = getCGICrate(cgi);
		cgiCrate = cratePair.first;
		myCrate = cratePair.second;
	} catch (emu::fed::exception::ParseException &e) {
		*out << printException(e);
		return;
	}

	
	std::stringstream sTitle;
	sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DDU Firmware Manager";
	std::vector<std::string> jsFileNames;
	jsFileNames.push_back("bitFlipper.js");
	jsFileNames.push_back("formChecker.js");
	*out << Header(sTitle.str(),jsFileNames);


	// PGK Select-a-Crate/Slot
	*out << cgicc::fieldset()
		.set("class","fieldset") << std::endl;
	*out << cgicc::div("View this page for a different crate/board")
		.set("class","legend") << std::endl;

	*out << selectACrate("DDUBroadcast","crate",cgiCrate,cgiCrate) << std::endl;

	*out << cgicc::span("Configuration located at " + xmlFile_.toString())
		.set("style","color: #A00; font-size: 10pt;") << std::endl;

	*out << cgicc::fieldset() << std::endl;
	*out << cgicc::br() << std::endl;

	// The names of the PROMs
	std::vector< std::string > dduPROMNames;
	dduPROMNames.push_back("VMEPROM");
	dduPROMNames.push_back("DDUPROM0");
	dduPROMNames.push_back("DDUPROM1");
	dduPROMNames.push_back("INPROM0");
	dduPROMNames.push_back("INPROM1");

	// The device types of the PROMs
	std::vector<enum DEVTYPE> dduPROMTypes;
	dduPROMTypes.push_back(VMEPROM);
	dduPROMTypes.push_back(DDUPROM0);
	dduPROMTypes.push_back(DDUPROM1);
	dduPROMTypes.push_back(INPROM0);
	dduPROMTypes.push_back(INPROM1);

	// The device types of the FPGA corresponding to the PROMS
	std::vector<enum DEVTYPE> dduFPGATypes;
	dduFPGATypes.push_back(NONE);
	dduFPGATypes.push_back(DDUFPGA);
	dduFPGATypes.push_back(DDUFPGA);
	dduFPGATypes.push_back(INFPGA0);
	dduFPGATypes.push_back(INFPGA1);

	// A map of PROM name to the first two identifying digits in the version
	//  number.
	std::vector<uint8_t> dduPROMHeaders;
	dduPROMHeaders.push_back(0xb0);
	dduPROMHeaders.push_back(0xc0);
	dduPROMHeaders.push_back(0xc1);
	dduPROMHeaders.push_back(0xd0);
	dduPROMHeaders.push_back(0xd1);

	// Store the codes to compare them to the FPGAs
	std::vector<uint32_t> diskPROMCodes(5);

	*out << cgicc::fieldset()
		.set("class","normal") << std::endl;
	*out << cgicc::div("Step 1:  Upload DDU firmware to disk")
		.set("class","legend") << std::endl;
	
	DataTable diskTable;
	diskTable->set("id","diskTable");
	
	diskTable(0,0) << "DDU PROM Name";
	diskTable(0,1) << "On-Disk Firmware Version";
	diskTable(0,2) << "Upload New Firmware";
	diskTable[0]->set("class","header");

	// Loop over the prom types and give us a pretty table
	for (unsigned int iprom = 0; iprom < dduPROMNames.size(); iprom++) {
	
		diskTable(iprom + 1,0) << dduPROMNames[iprom];

		// Get the version number from the on-disk file
		// Open the file and read up until the usercode.
		std::string fileName = "Current" + dduPROMNames[iprom] + ".svf";
		std::ifstream inFile(fileName.c_str(), std::ifstream::in);

		unsigned long int diskVersion = 0;
		
		// Can't have bogus files
		if (!inFile.is_open()) {
			LOG4CPLUS_ERROR(getApplicationLogger(), "Cannot open file " << fileName);

		} else {
		
			// Now start parsing the file.  Read lines until we have an eof or the usercode
			while (!inFile.eof()) {
				
				// Each line is a command (or comment)
				std::string myLine;
				getline(inFile, myLine);
				
				
				// Only care about one particular command:  instruction 0xfd
				if (myLine.find("SIR 8 TDI (fd)") != std::string::npos) {
					// The next line contains the usercode.
					// Make sure there is a next line.
					if (inFile.eof()) break;

					getline(inFile, myLine);

					std::stringstream lineStream(myLine);
					std::string parsedLine;
					lineStream >> parsedLine; // SDR
					lineStream >> parsedLine; // 32
					lineStream >> parsedLine; // TDI
					lineStream >> parsedLine; // (########)

					parsedLine = parsedLine.substr(1,8);
					
					sscanf(parsedLine.c_str(), "%08lx", &diskVersion);

					break;
				}
			}

			inFile.close();
		}
		
		// Now the diskVersion is exactly what is sounds like.
		diskPROMCodes[iprom] = diskVersion;

		// Check to make sure the on-disk header looks like it should for that
		//  particular PROM
		if ( diskPROMCodes[iprom] >> 24 != dduPROMHeaders[iprom] ) {
			diskTable(iprom + 1,1) << "ERROR READING LOCAL FILE -- UPLOAD A NEW FILE";
			diskTable(iprom + 1,1).setClass("bad");
		} else {
			diskTable(iprom + 1,1) << std::hex << diskVersion;
			diskTable(iprom + 1,1).setClass("ok");
		}

		// Compare the version on-disk with the magic number given above.
		/*
		std::stringstream checkstream;
		checkstream << std::hex << tuscode[i+3];
		if ( checkstream.str() != printversion ) {
			verr = 1;
			printversion += " (should be " + checkstream.str() + ")";
		}
		*/

		// Make the last part of the table a form for uploading a file.
		diskTable(iprom + 1,2) << cgicc::form().set("method","POST")
			.set("enctype","multipart/form-data")
			.set("id","Form" + dduPROMNames[iprom])
			.set("action","/" + getApplicationDescriptor()->getURN() + "/DDULoadBroadcast") << std::endl;
		diskTable(iprom + 1,2) << cgicc::input().set("type","file")
			.set("name","File")
			.set("id","File" + dduPROMNames[iprom])
			.set("size","50") << std::endl;
		//diskTable(iprom + 1,2) << cgicc::input().set("type","button")
		//	.set("value","Upload SVF")
		//	.set("onClick","javascript:if (formCheck('File" + dduPROMNames[iprom] + "')) { document.getElementById('Form" + dduPROMNames[iprom] + "').submit(); }") << std::endl;
		diskTable(iprom + 1,2) << cgicc::input().set("type","submit")
			.set("value","Upload SVF") << std::endl;
		diskTable(iprom + 1,2) << cgicc::input().set("type","hidden")
			.set("name","svftype")
			.set("value",dduPROMNames[iprom]) << std::endl;
		std::ostringstream crateVal;
		crateVal << cgiCrate;
		diskTable(iprom + 1,2) << cgicc::input().set("type","hidden")
			.set("name","crate")
			.set("value",crateVal.str()) << std::endl;
		diskTable(iprom + 1,2) << cgicc::form() << std::endl;
	}	

	// Print the table to screen.
	*out << diskTable.toHTML() << std::endl;

	*out << cgicc::fieldset() << std::endl;


	// This is the key:  a form that is outside of everything, and bitwise
	//  selectors of the slots to upgrade.
	*out << cgicc::form()
		.set("action","/" + getApplicationDescriptor()->getURN() + "/DDUSendBroadcast")
		.set("method","GET");
		/*
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","slots")
		.set("id","slots")
		.set("value","0x0");
		*/

	for (unsigned int iprom = 0; iprom < dduPROMNames.size(); iprom++) {
		std::stringstream codeStream;
		codeStream << diskPROMCodes[iprom];
		*out << cgicc::input()
			.set("type","hidden")
			.set("name",dduPROMNames[iprom])
			.set("value",codeStream.str()) << std::endl;
	}

	*out << cgicc::fieldset()
		.set("class","normal") << std::endl;
	*out << cgicc::div("Step 2:  Select the slots to which you intend to load firmware")
		.set("class","legend") << std::endl;
	
	DataTable slotTable;
	slotTable->set("id", "slotTable");

	slotTable(0,0) << "Slot number";
	slotTable(0,1) << "RUI";
	slotTable(0,2) << "DDU board ID";
	for (unsigned int iprom = 0; iprom < dduPROMNames.size(); iprom++) {
		slotTable(0,3 + iprom) << dduPROMNames[iprom];
	}
	slotTable[0]->set("class", "header");


	std::vector<DDU *> myDDUs = myCrate->getDDUs();
	for (unsigned int iDDU = 0; iDDU < myDDUs.size(); iDDU++) {

		DDU *myDDU = myDDUs[iDDU];
		
		//std::ostringstream bitFlipCommand;
		//bitFlipCommand << "Javascript:toggleBit('slots'," << myDDU->slot() << ");";
		
		std::ostringstream elementID;
		elementID << "slot" << myDDU->slot();

		slotTable(iDDU + 1,0) << cgicc::input()
			.set("type","checkbox")
			.set("class","slotBox")
			.set("id",elementID.str())
			.set("name",elementID.str())
			//.set("onChange",bitFlipCommand.str()) << " " 
			<< cgicc::label()
			.set("for",elementID.str()) << myDDU->slot() << cgicc::label();
		
		slotTable(iDDU + 1,1) << myCrate->getRUI(myDDU->slot());

		try {
			slotTable(iDDU + 1,2) << myDDU->readFlashBoardID();
		} catch (emu::fed::exception::DDUException &e) {
			*out << printException(e);
		}

		for (unsigned int iprom = 0; iprom < dduPROMNames.size(); iprom++) {

			uint32_t promCode = 0;
			uint32_t fpgaCode = 0;
			
			try {
				promCode = myDDU->readUserCode(dduPROMTypes[iprom]);
			} catch (emu::fed::exception::DDUException &e) {
				*out << printException(e);
			}
			slotTable(iDDU + 1,3 + iprom) << std::hex << promCode;

			if (dduPROMNames[iprom] != "VMEPROM") {
				try {
					fpgaCode = myDDU->readUserCode(dduFPGATypes[iprom]);
				} catch (emu::fed::exception::DDUException &e) {
					*out << printException(e);
				}
			}

			// Check for consistency
			slotTable(iDDU + 1,3 + iprom).setClass("ok");
			// DDUPROMs are tricky
			if (dduFPGATypes[iprom] == DDUFPGA) {
				
				if ((diskPROMCodes[iprom] & 0xffffff00) != (promCode & 0xffffff00)) {
					// Match usercode against PROM code
					slotTable(iDDU + 1,3 + iprom).setClass("bad");
					
				} else if (fpgaCode & 0x000ff000 != promCode & 0x00ff0000) {
					// Match usercode against FPGA code
					slotTable(iDDU + 1,3 + iprom).setClass("questionable");
				}
				
			} else if (diskPROMCodes[iprom] != promCode) {
				// Else match usercode against PROM code
				slotTable(iDDU + 1,3 + iprom).setClass("bad");

			} else if (dduPROMTypes[iprom] == INPROM0 || dduPROMTypes[iprom] == INPROM1) {
				// INPROMs have a special FPGA code
				
				if (fpgaCode & 0x00ffffff != promCode & 0x00ffffff) {
					slotTable(iDDU + 1,3 + iprom).setClass("bad");
				}
			}
		}
	}

	*out << slotTable.toHTML() << std::endl;

	*out << cgicc::input()
		.set("type","checkbox")
		.set("id","broadcast")
		.set("name","broadcast")
		.set("style","margin-left: 20%; margin-right: auto;");
	*out << cgicc::label()
		.set("for","broadcast")
		<< "Enable Broadcast" << cgicc::label();

	*out << cgicc::div()
		.set("style","font-size: 8pt;") << std::endl;
	*out << "Legend: " << cgicc::span("All OK").set("class","ok") << " " << std::endl;
	*out << cgicc::span("Disk/PROM mismatch").set("class","bad") << " " << std::endl;
	*out << cgicc::span("PROM/FPGA mismatch").set("class","questionable") << cgicc::div() << std::endl;

	*out << cgicc::div("DO NOT BROADCAST UNLESS THE CONFIGURATION FILE CONTAINS ALL THE BOARDS PRESENT IN THE CRATE!") << std::endl;

	*out << cgicc::fieldset() << std::endl;


	*out << cgicc::fieldset()
		.set("class","normal") << std::endl;
	*out << cgicc::div("Step 3:  Press a button to upload that particular firmware to the selected boards")
		.set("class","legend") << std::endl;

	*out << cgicc::input()
		.set("type","submit")
		.set("name","submit")
		.set("value","Send VMEPROM (Emergency Load)") << std::endl;
	*out << cgicc::input()
		.set("type","submit")
		.set("name","submit")
		.set("value","Send VMEPROM") << std::endl;
	*out << cgicc::input()
		.set("type","submit")
		.set("name","submit")
		.set("value","Send DDUPROM") << std::endl;
	*out << cgicc::input()
		.set("type","submit")
		.set("name","submit")
		.set("value","Send INPROM") << std::endl;

	// The crate is a must.
	std::ostringstream crateVal;
	crateVal << cgiCrate;
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","crate")
		.set("value",crateVal.str()) << std::endl;
	
	*out << cgicc::fieldset() << std::endl;
	
	*out << cgicc::form() << std::endl;


	*out << cgicc::fieldset()
		.set("class","normal") << std::endl;
	*out << cgicc::div("Step 4:  Hard-reset the crate")
		.set("class","legend") << std::endl;

	*out << cgicc::form()
		.set("action","/" + getApplicationDescriptor()->getURN() + "/DDUReset?crate=" + crateVal.str())
		.set("method","post") << std::endl;
	*out << cgicc::input()
		.set("type","submit")
		.set("value","Reset crate via DCC") << std::endl;
	*out << cgicc::form() << std::endl;

	*out << cgicc::fieldset() << std::endl;

	*out << Footer() << std::endl;

}



void emu::fed::EmuFCrateHyperDAQ::DDULoadBroadcast(xgi::Input *in, xgi::Output *out)
{
	try {
		// PGK Patented check-for-initialization
		if (crateVector_.size()==0) {
			LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
			return Default(in,out);
		}

		cgicc::Cgicc cgi(in);
		
		// First, I need a crate.
		std::pair<unsigned int, Crate *> cratePair = getCGICrate(cgi);
		unsigned int cgiCrate = cratePair.first;
		//Crate *myCrate = cratePair.second;

		std::string type = cgi["svftype"]->getValue();

		if (type != "VMEPROM" && type != "DDUPROM0" && type != "DDUPROM1" && type != "INPROM0" && type != "INPROM1") {
			std::ostringstream error;
			error << "PROM type " << type << " not understood";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			XCEPT_DECLARE(emu::fed::exception::ParseException, e, error.str());
			notifyQualified("ERROR", e);

			std::ostringstream backLocation;
			backLocation << "DDUBroadcast?crate=" << cgiCrate;
			return webRedirect(out,backLocation.str());
		}

		cgicc::const_file_iterator ifile = cgi.getFile("File");
		if ( (*ifile).getFilename() == "" ) {
			std::ostringstream error;
			error << "The file you attempted to upload either doesn't exist, or wasn't properly transferred";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			XCEPT_DECLARE(emu::fed::exception::FileException, e, error.str());
			notifyQualified("ERROR", e);
			
			std::ostringstream backLocation;
			backLocation << "DDUBroadcast?crate=" << cgiCrate;
			return webRedirect(out,backLocation.str());
		}

		std::string filename = "Current" + type + ".svf";
		std::ofstream outfile;
		outfile.open(filename.c_str(),std::ios::trunc);
		if (!outfile.is_open()) {
			std::ostringstream error;
			error << "The file " << filename << " is not accessable for writing";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			XCEPT_DECLARE(emu::fed::exception::FileException, e, error.str());
			notifyQualified("ERROR", e);
			
			std::ostringstream backLocation;
			backLocation << "DDUBroadcast?crate=" << cgiCrate;
			return webRedirect(out,backLocation.str());
		}

		(*ifile).writeToStream(outfile);
		outfile.close();

		LOG4CPLUS_ERROR(getApplicationLogger(), "Downloaded and saved to file " << filename << " firmware for " << type);
		
		std::ostringstream backLocation;
		backLocation << "DDUBroadcast?crate=" << cgiCrate;
		return webRedirect(out,backLocation.str());
		
	} catch (xcept::Exception &e) {
		std::ostringstream error;
		error << "Exception caught!";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
		
		std::vector<xcept::ExceptionInformation> history = e2.getHistory();
		for (std::vector<xcept::ExceptionInformation>::iterator iError = history.begin(); iError != history.end(); iError++) {
			DataTable exceptionTable;
			unsigned int iRow = 0;
			std::map<std::string, std::string> messages = iError->getProperties();
			for (std::map<std::string, std::string>::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++) {
				exceptionTable(iRow, 0) << iMessage->first << ":";
				exceptionTable(iRow, 1) << iMessage->second;
				iRow++;
			}
			*out << cgicc::div(exceptionTable.toHTML())
				.set("class", "exception");
		}
	}

}



void emu::fed::EmuFCrateHyperDAQ::DDUSendBroadcast(xgi::Input *in, xgi::Output *out)
{

	try {
		
		// PGK Patented check-for-initialization
		if (crateVector_.size()==0) {
			LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
			return Default(in,out);
		}

		cgicc::Cgicc cgi(in);
		
		// First, I need a crate.
		std::pair<unsigned int, Crate *> cratePair = getCGICrate(cgi);
		unsigned int cgiCrate = cratePair.first;
		Crate *myCrate = cratePair.second;

		// unsigned int tidcode[8] = {0x2124a093,0x31266093,0x31266093,0x05036093,0x05036093,0x05036093,0x05036093,0x05036093};
		// unsigned int tuscode[8] = {0xcf043a02,0xdf025a02,0xdf025a02,0xb0020a04,0xc043dd99,0xc143dd99,0xd0025a02,0xd1025a02};

		// Now I need to know which PROMs I am loading.
		// I used a trick where the submit button can act as a form field in HTML.
		int type = -1;
		std::string submitCommand = cgi["submit"]->getValue();
		if (submitCommand.substr(5) == "VMEPROM") type = 0;
		if (submitCommand.substr(5) == "DDUPROM") type = 1;
		if (submitCommand.substr(5) == "INPROM") type = 2;
		if (submitCommand.substr(5) == "VMEPROM (Emergency Load)") type = 3;

		// Check to see if I should broadcast or if I should load to certain slots only.
		bool broadcast = cgi.queryCheckbox("broadcast");
		std::vector<unsigned int> slots;

		for (unsigned int iSlot = 1; iSlot <= 20; iSlot++) {
			std::ostringstream slotName;
			slotName << "slot" << iSlot;
			if (cgi.queryCheckbox(slotName.str())) {
				slots.push_back(iSlot);
			}
		}

		if (type == 3 && broadcast) {
			LOG4CPLUS_WARN(getApplicationLogger(),"Cannot broadcast VMEPROM firmware via emergency load.  Defaulting to individual board loading");
			
			slots.clear();
			broadcast = false; // Can't broadcast emergency VMEPROM.
			
			// Loop through the DDUs and add them all to the list of slots to load.
			std::vector<DDU *> myDDUs = myCrate->getDDUs();
			for (std::vector<DDU *>::iterator iDDU = myDDUs.begin(); iDDU != myDDUs.end(); iDDU++) {
				slots.push_back((*iDDU)->slot());
			}
		}

		// Error:  no slots to load.
		if (!broadcast && !slots.size()) {
			LOG4CPLUS_ERROR(getApplicationLogger(),"No slots selected for firmware loading, and broadcast not set");
			std::ostringstream backLocation;
			backLocation << "DDUBroadcast?crate=" << cgiCrate;
			return webRedirect(out,backLocation.str());
		}

		// Error:  bad prom type
		if (type != 0 && type != 1 && type != 2 && type != 3) {
			LOG4CPLUS_ERROR(getApplicationLogger(),"PROM type not understood");
			std::ostringstream backLocation;
			backLocation << "DDUBroadcast?crate=" << cgiCrate;
			return webRedirect(out,backLocation.str());
		}

		// A database of PROM types
		std::vector<std::string> promName;
		promName.push_back("VMEPROM");
		promName.push_back("DDUPROM1");
		promName.push_back("DDUPROM0");
		promName.push_back("INPROM1");
		promName.push_back("INPROM0");
		promName.push_back("VMEPROM");
		std::vector<enum DEVTYPE> devType;
		devType.push_back(VMEPROM);
		devType.push_back(DDUPROM1);
		devType.push_back(DDUPROM0);
		devType.push_back(INPROM1);
		devType.push_back(INPROM0);
		devType.push_back(RESET);

		// Load the proper version integers from the cgi handle.
		std::vector<unsigned long int> version;
		for (int iProm = 0; iProm <= 5; iProm++) {
			// cgicc can't handle things bigger than 31 bits with getIntegerValue.
			// Use getValue instead and parse the string myself.
			unsigned long int versionCache = 0;
			sscanf(cgi[promName[iProm]]->getValue().c_str(), "%lu", &versionCache);
			
			version.push_back(versionCache);

		}

		// We should know which proms we should load once a button is pressed.
		unsigned int from = 1;
		unsigned int to = 0;
		if (type == 0) { from = 0; to = 0; }
		if (type == 1) { from = 1; to = 2; }
		if (type == 2) { from = 3; to = 4; }
		if (type == 3) { from = 5; to = 5; }

		// Make a vector of all the boards to which we need to load firmware.
		std::vector<DDU *> loadTheseDDUs;
		if (broadcast) {
			loadTheseDDUs.push_back(myCrate->getBroadcastDDU());
		} else {
			std::vector<DDU *> dduVector = myCrate->getDDUs();
			for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
				for (unsigned int iSlot = 0; iSlot < slots.size(); iSlot++) {
					if (slots[iSlot] == (*iDDU)->slot()) loadTheseDDUs.push_back((*iDDU));
				}
			}
		}

		// Now load the firmware.
		for (std::vector<DDU *>::iterator iDDU = loadTheseDDUs.begin(); iDDU != loadTheseDDUs.end(); iDDU++) {

			// Loop over all the firmwares we need to load.
			for (unsigned int iProm = from; iProm <= to; iProm++) {

				//LOG4CPLUS_DEBUG(getApplicationLogger(),"Loading firmware to prom " << (iProm+1) << " of " << (to - from + 1) << "...");

				// Get the name of the file we want to load.  It's a standard name.
				std::string fileName = "Current" + promName[iProm] + ".svf";

				LOG4CPLUS_INFO(getApplicationLogger(),"Loading file " << fileName << " (version " << std::hex << version[iProm] << std::dec << ") to DDU slot " << (*iDDU)->slot() << "...");

				// Load the file, but we need to be careful of DDUPROMs,
				// because they need to be treated differently...
				if (type == 1) {

					// Pick out board numbers before writing anything.
					std::vector<uint32_t> boardNumbers;
					std::vector<DDU *> checkTheseDDUs = (broadcast) ? myCrate->getDDUs() : loadTheseDDUs;
					for (std::vector<DDU *>::iterator jDDU = checkTheseDDUs.begin(); jDDU != checkTheseDDUs.end(); jDDU++) {
						boardNumbers.push_back((*jDDU)->readFlashBoardID());
					}

					LOG4CPLUS_DEBUG(getApplicationLogger(),"Step 1 of 3: Load firmware up to " << std::hex << version[iProm] << std::dec << "...");

					// The last parameter tells us where to stop.
					std::stringstream usercodeText;
					usercodeText << std::hex << version[iProm];
					(*iDDU)->loadPROM(devType[iProm], fileName, "", "(" + usercodeText.str() + ")");
					
					//char *boardnumber = (char *) malloc(5);
					//boardnumber[0]=0x01;boardnumber[1]=0x00;boardnumber[2]=0x00;boardnumber[3]=0x00;
					//(*iDDU)->epromload((char *) promName[iProm].c_str(), devType[iProm], (char *) fileName.c_str(), 1, boardnumber, 1);

					LOG4CPLUS_DEBUG(getApplicationLogger(),"Step 2 of 3: Load usercodes...");
					
					// Load the board IDs into the usercodes individually.
					for (unsigned int jDDU = 0; jDDU < checkTheseDDUs.size(); jDDU++) {

						DDU *myDDU = checkTheseDDUs[jDDU];
						uint32_t myBoardNumber = boardNumbers[jDDU];

						// Make the proper usercode.
						std::vector<uint16_t> userCode;
						userCode.push_back((version[iProm] & 0x0000ff00) | (myBoardNumber & 0xff));
						userCode.push_back((version[iProm] & 0xffff0000) >> 16);

						LOG4CPLUS_DEBUG(getApplicationLogger(),"Loading " << std::hex << userCode[1] << userCode[0] << std::dec);
						// Do the write
						myDDU->jtagWrite(devType[iProm], 32, userCode, true);

						//boardnumber[0] = userCode[0] & 0x00ff;
						//myDDU->epromload((char *) promName[iProm].c_str(), devType[iProm], (char *) fileName.c_str(), 1, boardnumber, 2);
					}

					LOG4CPLUS_DEBUG(getApplicationLogger(),"Step 3 of 3: Load firmware after " << std::hex << version[iProm] << std::dec << "...");

					// Now continue the load from after where we left off.  The second-to-last
					// parameter tells us the line from where we should continue.
					(*iDDU)->loadPROM(devType[iProm], fileName, "(" + usercodeText.str() + ")", "");
					//boardnumber[0] = 1;
					//(*iDDU)->epromload((char *) promName[iProm].c_str(), devType[iProm], (char *) fileName.c_str(), 1, boardnumber, 3);

					// Now check to see if the usercode matches.
					if (!broadcast) {
						int32_t checkUserCode = (*iDDU)->readUserCode(devType[iProm]);
						
						if ((checkUserCode & 0xffffff00) != (version[iProm] & 0xffffff00)) {
							LOG4CPLUS_ERROR(getApplicationLogger(), "PROM load failed to " << promName[iProm] << ": expected version " << std::hex << version[iProm] << std::dec << ", read back " << std::hex << checkUserCode << std::dec);
						} else {
							LOG4CPLUS_INFO(getApplicationLogger(), "PROM load succeeded to " << promName[iProm] << ": expected version " << std::hex << version[iProm] << std::dec << ", read back " << std::hex << checkUserCode << std::dec);
						}
					}
					
				} else {
					// All other loads are very simple.
					(*iDDU)->loadPROM(devType[iProm], fileName);

					// Now check to see if the usercode matches.
					uint32_t checkUserCode = (*iDDU)->readUserCode(devType[iProm]);

					if (checkUserCode != version[iProm]) {
						LOG4CPLUS_ERROR(getApplicationLogger(), "PROM load failed to " << promName[iProm] << ": expected version " << std::hex << version[iProm] << std::dec << ", read back " << std::hex << checkUserCode << std::dec);
					} else {
						LOG4CPLUS_INFO(getApplicationLogger(), "PROM load succeeded to " << promName[iProm] << ": expected version " << std::hex << version[iProm] << std::dec << ", read back " << std::hex << checkUserCode << std::dec);
					}
					
				}

			}

		}

		std::ostringstream backLocation;
		backLocation << "DDUBroadcast?crate=" << cgiCrate;
		return webRedirect(out,backLocation.str());
		
	} catch (xcept::Exception &e) {
		std::ostringstream error;
		error << "Exception caught!";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
		
		std::vector<xcept::ExceptionInformation> history = e2.getHistory();
		for (std::vector<xcept::ExceptionInformation>::iterator iError = history.begin(); iError != history.end(); iError++) {
			DataTable exceptionTable;
			unsigned int iRow = 0;
			std::map<std::string, std::string> messages = iError->getProperties();
			for (std::map<std::string, std::string>::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++) {
				exceptionTable(iRow, 0) << iMessage->first << ":";
				exceptionTable(iRow, 1) << iMessage->second;
				iRow++;
			}
			*out << cgicc::div(exceptionTable.toHTML())
				.set("class", "exception");
		}
	}

}



void emu::fed::EmuFCrateHyperDAQ::DDUReset(xgi::Input *in, xgi::Output *out)
{

	try {
		
		// PGK Patented check-for-initialization
		if (crateVector_.size()==0) {
			LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
			return Default(in,out);
		}

		cgicc::Cgicc cgi(in);
		
		// First, I need a crate.
		std::pair<unsigned int, Crate *> cratePair = getCGICrate(cgi);
		unsigned int cgiCrate = cratePair.first;
		Crate *myCrate = cratePair.second;

		// No DCC = no luck.
		if (myCrate->getDCCs().size()) {
			myCrate->getDCCs()[0]->crateHardReset();
		} else {
			LOG4CPLUS_ERROR(getApplicationLogger(), "No DCCs present in configuration: manual crate resets not allowed.");
		}

		std::ostringstream backLocation;
		backLocation << "DDUBroadcast?crate=" << cgiCrate;
		return webRedirect(out,backLocation.str());
		
	} catch (xcept::Exception &e) {
		std::ostringstream error;
		error << "Exception caught!";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
		
		std::vector<xcept::ExceptionInformation> history = e2.getHistory();
		for (std::vector<xcept::ExceptionInformation>::iterator iError = history.begin(); iError != history.end(); iError++) {
			DataTable exceptionTable;
			unsigned int iRow = 0;
			std::map<std::string, std::string> messages = iError->getProperties();
			for (std::map<std::string, std::string>::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++) {
				exceptionTable(iRow, 0) << iMessage->first << ":";
				exceptionTable(iRow, 1) << iMessage->second;
				iRow++;
			}
			*out << cgicc::div(exceptionTable.toHTML())
				.set("class", "exception");
		}
	}

}



void emu::fed::EmuFCrateHyperDAQ::DCCReset(xgi::Input *in, xgi::Output *out)
{
	
	try {
		// PGK Patented check-for-initialization
		if (crateVector_.size()==0) {
			LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
			return Default(in,out);
		}
		
		cgicc::Cgicc cgi(in);
		
		// First, I need a crate.
		std::pair<unsigned int, Crate *> cratePair = getCGICrate(cgi);
		unsigned int cgiCrate = cratePair.first;
		Crate *myCrate = cratePair.second;
		
		// No DCC = no luck.
		std::vector<DCC *> myDCCs = myCrate->getDCCs();
		for (std::vector<DCC *>::iterator iDCC = myDCCs.begin(); iDCC != myDCCs.end(); iDCC++) {
			(*iDCC)->resetPROM(INPROM);
			(*iDCC)->resetPROM(RESET);
		}
		
		std::ostringstream backLocation;
		backLocation << "DCCBroadcast?crate=" << cgiCrate;
		return webRedirect(out,backLocation.str());
		
	} catch (xcept::Exception &e) {
		std::ostringstream error;
		error << "Exception caught!";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
		
		std::vector<xcept::ExceptionInformation> history = e2.getHistory();
		for (std::vector<xcept::ExceptionInformation>::iterator iError = history.begin(); iError != history.end(); iError++) {
			DataTable exceptionTable;
			unsigned int iRow = 0;
			std::map<std::string, std::string> messages = iError->getProperties();
			for (std::map<std::string, std::string>::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++) {
				exceptionTable(iRow, 0) << iMessage->first << ":";
				exceptionTable(iRow, 1) << iMessage->second;
				iRow++;
			}
			*out << cgicc::div(exceptionTable.toHTML())
				.set("class", "exception");
		}
	}
}



void emu::fed::EmuFCrateHyperDAQ::DDUDebug(xgi::Input *in, xgi::Output *out)
{
	try {
		// PGK Patented check-for-initialization
		if (crateVector_.size()==0) {
			LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
			return Default(in,out);
		}

		cgicc::Cgicc cgi(in);
		
		// First, I need a crate.
		std::pair<unsigned int, Crate *> cratePair = getCGICrate(cgi);
		unsigned int cgiCrate = cratePair.first;
		Crate *myCrate = cratePair.second;

		std::pair<unsigned int, DDU *> boardPair = getCGIBoard<DDU>(cgi);
		unsigned int cgiDDU = boardPair.first;
		DDU *myDDU = boardPair.second;

		std::stringstream sTitle;
		sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DDUFPGA Controls (RUI #" << myCrate->getRUI(myDDU->slot()) << ")";
		std::vector<std::string> jsFileNames;
		jsFileNames.push_back("tableToggler.js");
		jsFileNames.push_back("errorFlasher.js");
		*out << Header(sTitle.str(),jsFileNames);

		// PGK Select-a-Crate/Slot
		*out << cgicc::fieldset()
			.set("class","fieldset") << std::endl;
		*out << cgicc::div("View this page for a different crate/board")
			.set("class","legend") << std::endl;

		*out << selectACrate("DDUDebug","ddu",cgiDDU,cgiCrate) << std::endl;

		*out << cgicc::span("Configuration located at " + xmlFile_.toString())
			.set("style","color: #A00; font-size: 10pt;") << std::endl;

		*out << cgicc::fieldset() << std::endl;
		*out << cgicc::br() << std::endl;

		// PGK Let us now print some tables with information from the DDUFPGA.
		// This guy will be used later to show which chambers have errors in the big
		//  table.
		//unsigned int fibersWithErrors = 0;
		// PGK On second thought, this is a more accurate and consistant check.
		unsigned int fibersWithErrors = myDDU->readCSCStatus() | myDDU->readAdvancedFiberErrors();

		// This is used to check if the debug trap is valid.
		bool debugTrapValid = false;

		// PGK Your guess is as good as mine.
		//myDDU->infpga_shift0 = 0;
		//myDDU->ddu_shift0 = 0;

		//myCrate->getVMEController()->CAEN_err_reset();


		// Display general DDU status information
		*out << cgicc::fieldset()
			.set("class","normal") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << cgicc::a("&plusmn;")
			.set("class","openclose")
			.set("onClick","javascript:toggle('generalTable')") << std::endl;
		*out << "General DDU Information" << cgicc::div() << std::endl;

		DataTable generalTable;
		generalTable->set("id", "generalTable");

		generalTable(0,0) << "Register";
		generalTable(0,1) << "Value";
		generalTable(0,2) << "Decoded Status";
		generalTable[0]->set("class", "header");

		generalTable(1,0) << "DDU RUI (16-bit)";
		unsigned long int dduValue = myDDU->readRUI();
		generalTable(1,1) << dduValue;
		generalTable(1,1).setClass("none");

		generalTable(2,0) << "DDU L1 scaler";
		// PGK gives flakey values.
		//dduValue = myDDU->readL1Scaler(DDUFPGA);
		dduValue = myDDU->readL1Scaler(DDUFPGA);
		generalTable(2,1) << dduValue;
		generalTable(2,1).setClass("none");
		
		generalTable(3,0) << "DDU control FPGA status (32-bit)";
		dduValue = myDDU->readFPGAStatus(DDUFPGA);
		generalTable(3,1) << std::showbase << std::hex << dduValue;
		if (dduValue & 0x00008000) {
			generalTable(3,1).setClass("bad");
			debugTrapValid = true;
		}
		else if (dduValue & 0xDE4F4BFF) generalTable(3,1).setClass("warning");
		else generalTable(3,1).setClass("ok");
		std::map<std::string, std::string> dduComments = DDUDebugger::DDUFPGAStat(dduValue);
		for (std::map<std::string,std::string>::iterator iComment = dduComments.begin();
			iComment != dduComments.end();
			iComment++) {
			generalTable(3,2) << cgicc::div(iComment->first)
				.set("class",iComment->second);
		}

		generalTable(4,0) << "DDU output status (16-bit)";
		dduValue = myDDU->readOutputStatus();
		generalTable(4,1) << std::showbase << std::hex << dduValue;
		if (dduValue & 0x00000080) generalTable(4,1).setClass("bad");
		else if (dduValue & 0x00004000) generalTable(4,1).setClass("warning");
		else generalTable(4,1).setClass("ok");
		dduComments = DDUDebugger::OutputStat(dduValue);
		for (std::map<std::string,std::string>::iterator iComment = dduComments.begin();
			iComment != dduComments.end();
			iComment++) {
			generalTable(4,2) << cgicc::div(iComment->first)
				.set("class",iComment->second);
		}

		generalTable(5,0) << "Error bus A register bits (16-bit)";
		dduValue = myDDU->readEBRegister(1);
		generalTable(5,1) << std::showbase << std::hex << dduValue;
		if (dduValue & 0x0000C00C) generalTable(5,1).setClass("bad");
		else if (dduValue & 0x000001C8) generalTable(5,1).setClass("warning");
		else generalTable(5,1).setClass("ok");
		dduComments = DDUDebugger::EBReg1(dduValue);
		for (std::map<std::string,std::string>::iterator iComment = dduComments.begin();
			iComment != dduComments.end();
			iComment++) {
			generalTable(5,2) << cgicc::div(iComment->first)
				.set("class",iComment->second);
		}

		generalTable(6,0) << "Error bus B register bits (16-bit)";
		dduValue = myDDU->readEBRegister(2);
		generalTable(6,1) << std::showbase << std::hex << dduValue;
		if (dduValue & 0x00000011) generalTable(6,1).setClass("bad");
		else if (dduValue & 0x0000D08E) generalTable(6,1).setClass("warning");
		else generalTable(6,1).setClass("ok");
		dduComments = DDUDebugger::EBReg2(dduValue);
		for (std::map<std::string,std::string>::iterator iComment = dduComments.begin();
			iComment != dduComments.end();
			iComment++) {
			generalTable(6,2) << cgicc::div(iComment->first)
				.set("class",iComment->second);
		}

		generalTable(7,0) << "Error bus C register bits (16-bit)";
		dduValue = myDDU->readEBRegister(3);
		generalTable(7,1) << std::showbase << std::hex << dduValue;
		if (dduValue & 0x0000BFBF) generalTable(7,1).setClass("warning");
		else generalTable(7,1).setClass("ok");
		dduComments = DDUDebugger::EBReg3(dduValue);
		for (std::map<std::string,std::string>::iterator iComment = dduComments.begin();
			iComment != dduComments.end();
			iComment++) {
			generalTable(7,2) << cgicc::div(iComment->first)
				.set("class",iComment->second);
		}

		*out << generalTable.printSummary() << std::endl;

		*out << generalTable.toHTML() << std::endl;

		*out << cgicc::fieldset() << std::endl;

		// Clever trick to help with formating the cut-and-paste.
		*out << cgicc::br()
			.set("style","display: none") << std::endl;



		// Display miscellanious DDU status information
		*out << cgicc::fieldset()
			.set("class","normal") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << cgicc::a("&plusmn;")
			.set("class","openclose")
			.set("onClick","javascript:toggle('otherTable')") << std::endl;
		*out << "Other DDU Information" << cgicc::div() << std::endl;

		DataTable otherTable;
		otherTable->set("id", "otherTable");

		otherTable(0,0) << "Register";
		otherTable(0,1) << "Value";
		otherTable(0,2) << "Decoded Status";
		otherTable[0]->set("class", "header");

		otherTable(1,0) << "DDU near full warning (8-bit)";
		dduValue = myDDU->readWarningMonitor();
		otherTable(1,1) << std::showbase << std::hex << ((dduValue) & 0xFF);
		if ((dduValue) & 0xFF) otherTable(1,1).setClass("questionable");
		else otherTable(1,1).setClass("ok");
		dduComments = DDUDebugger::WarnMon((dduValue) & 0xFF);
		for (std::map<std::string,std::string>::iterator iComment = dduComments.begin();
			iComment != dduComments.end();
			iComment++) {
			otherTable(1,2) << cgicc::div(iComment->first)
				.set("class",iComment->second);
		}

		otherTable(2,0) << "DDU near full historical (8-bit)";
		//dduValue = myDDU->readWarnMon();
		otherTable(2,1) << std::showbase << std::hex << ((dduValue >> 8) & 0xFF);
		if ((dduValue >> 8) & 0xFF) otherTable(2,1).setClass("questionable");
		else otherTable(2,1).setClass("ok");
		dduComments = DDUDebugger::WarnMon((dduValue >> 8) & 0xFF);
		for (std::map<std::string,std::string>::iterator iComment = dduComments.begin();
			iComment != dduComments.end();
			iComment++) {
			otherTable(2,2) << cgicc::div(iComment->first)
				.set("class",iComment->second);
		}

		otherTable(3,0) << "DDU L1A-to-start max process time";
		dduValue = myDDU->readMaxTimeoutCount();
		otherTable(3,1) << (((dduValue) & 0xFF) * 400.0) << " ns";
		otherTable(3,1).setClass("none");

		otherTable(4,0) << "DDU start-to-end max process time";
		//dduValue = myDDU->readWarnMon();
		otherTable(4,1) << std::showbase << std::hex << (((dduValue >> 8) & 0xFF) * 6.4) << " &mu;s";
		otherTable(4,1).setClass("none");

		*out << otherTable.printSummary() << std::endl;

		*out << otherTable.toHTML() << std::endl;

		*out << cgicc::fieldset() << std::endl;

		// Clever trick to help with formating the cut-and-paste.
		*out << cgicc::br()
			.set("style","display: none") << std::endl;
		

		

		//myCrate->getVMEController()->CAEN_err_reset();
		// Display individual fiber information
		*out << cgicc::fieldset()
			.set("class","normal") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << cgicc::a("&plusmn;")
			.set("class","openclose")
			.set("onClick","javascript:toggle('fiberTable')") << std::endl;
		*out << "Individual Fiber Information" << cgicc::div() << std::endl;


		DataTable fiberTable;
		fiberTable->set("id", "fiberTable");

		fiberTable(0,0) << "Register";
		fiberTable(0,1) << "Value";
		fiberTable(0,2) << "Decoded Chambers";
		fiberTable[0]->set("class", "header");

		fiberTable(1,0) << "First event DMBLIVE";
		dduValue = myDDU->readDMBLiveAtFirstEvent();
		fiberTable(1,1) << std::showbase << std::hex << dduValue;
		fiberTable(1,1).setClass("none");
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (dduValue & (1<<iFiber)) {
				fiberTable(1,2) << cgicc::div(myDDU->getChamber(iFiber)->name())
					.set("class","none");
			}
		}

		fiberTable(2,0) << "Latest event DMBLIVE";
		dduValue = myDDU->readDMBLive();
		fiberTable(2,1) << std::showbase << std::hex << dduValue;
		fiberTable(2,1).setClass("none");
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (dduValue & (1<<iFiber)) {
				fiberTable(2,2) << cgicc::div(myDDU->getChamber(iFiber)->name())
					.set("class","none");
			}
		}

		fiberTable(3,0) << "CRC error";
		dduValue = myDDU->readCRCError();
		fiberTable(3,1) << std::showbase << std::hex << dduValue;
		if (dduValue) fiberTable(3,1).setClass("bad");
		else fiberTable(3,1).setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (dduValue & (1<<iFiber)) {
				fiberTable(3,2) << cgicc::div(myDDU->getChamber(iFiber)->name())
					.set("class","red");
				//fibersWithErrors |= 1<<iFiber;
			}
		}

		fiberTable(4,0) << "Data transmit error";
		dduValue = myDDU->readXmitError();
		fiberTable(4,1) << std::showbase << std::hex << dduValue;
		if (dduValue) fiberTable(4,1).setClass("bad");
		else fiberTable(4,1).setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (dduValue & (1<<iFiber)) {
				fiberTable(4,2) << cgicc::div(myDDU->getChamber(iFiber)->name())
					.set("class","red");
				//fibersWithErrors |= 1<<iFiber;
			}
		}

		fiberTable(5,0) << "DMB error";
		dduValue = myDDU->readDMBError();
		fiberTable(5,1) << std::showbase << std::hex << dduValue;
		if (dduValue) fiberTable(5,1).setClass("bad");
		else fiberTable(5,1).setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (dduValue & (1<<iFiber)) {
				fiberTable(5,2) << cgicc::div(myDDU->getChamber(iFiber)->name())
					.set("class","red");
				//fibersWithErrors |= 1<<iFiber;
			}
		}

		fiberTable(6,0) << "TMB error";
		dduValue = myDDU->readTMBError();
		fiberTable(6,1) << std::showbase << std::hex << dduValue;
		if (dduValue) fiberTable(6,1).setClass("bad");
		else fiberTable(6,1).setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (dduValue & (1<<iFiber)) {
				fiberTable(6,2) << cgicc::div(myDDU->getChamber(iFiber)->name())
					.set("class","red");
				//fibersWithErrors |= 1<<iFiber;
			}
		}

		fiberTable(7,0) << "ALCT error";
		dduValue = myDDU->readALCTError();
		fiberTable(7,1) << std::showbase << std::hex << dduValue;
		if (dduValue) fiberTable(7,1).setClass("bad");
		else fiberTable(7,1).setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (dduValue & (1<<iFiber)) {
				fiberTable(7,2) << cgicc::div(myDDU->getChamber(iFiber)->name())
					.set("class","red");
				//fibersWithErrors |= 1<<iFiber;
			}
		}

		fiberTable(8,0) << "Lost-in-event error";
		dduValue = myDDU->readLIEError();
		fiberTable(8,1) << std::showbase << std::hex << dduValue;
		if (dduValue) fiberTable(8,1).setClass("bad");
		else fiberTable(8,1).setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (dduValue & (1<<iFiber)) {
				fiberTable(8,2) << cgicc::div(myDDU->getChamber(iFiber)->name())
					.set("class","red");
				//fibersWithErrors |= 1<<iFiber;
			}
		}

		*out << fiberTable.printSummary() << std::endl;

		*out << cgicc::fieldset() << std::endl;

		// Clever trick to help with formating the cut-and-paste.
		*out << cgicc::br()
			.set("style","display: none") << std::endl;




		// Display InRD status information
		*out << cgicc::fieldset()
			.set("class","normal") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << cgicc::a("&plusmn;")
			.set("class","openclose")
			.set("onClick","javascript:toggle('inrdTable')") << std::endl;
		*out << "4-bit InRD Information" << cgicc::div() << std::endl;


		DataTable inrdTable;
		inrdTable->set("id", "inrdTable");

		inrdTable(0,0) << "Register";
		inrdTable(0,1) << "Value";
		inrdTable(0,2) << "Decoded Chambers";
		inrdTable[0]->set("class", "header");

		inrdTable(1,0) << "Stuck data error";
		dduValue = myDDU->readFIFOStatus(1);
		inrdTable(1,1) << std::showbase << std::hex << ((dduValue >> 12) & 0xF);
		if ((dduValue >> 12) & 0xF) inrdTable(1,1).setClass("bad");
		else inrdTable(1,1).setClass("ok");

		inrdTable(2,0) << "Fiber or FIFO connection error";
		//dduValue = myDDU->checkFIFO(1);
		inrdTable(2,1) << std::showbase << std::hex << ((dduValue >> 8) & 0xF);
		if ((dduValue >> 8) & 0xF) inrdTable(2,1).setClass("bad");
		else inrdTable(2,1).setClass("ok");

		inrdTable(3,0) << "L1A mismatch";
		//dduValue = myDDU->checkFIFO(1);
		inrdTable(3,1) << std::showbase << std::hex << ((dduValue >> 4) & 0xF);
		if ((dduValue >> 4) & 0xF) inrdTable(3,1).setClass("warning");
		else inrdTable(3,1).setClass("ok");

		inrdTable(4,0) << "InRD with active fiber";
		//dduValue = myDDU->checkFIFO(1);
		inrdTable(4,1) << std::showbase << std::hex << ((dduValue >> 4) & 0xF);
		inrdTable(4,1).setClass("none");

		inrdTable(5,0) << "Active ext. FIFO empty";
		dduValue = myDDU->readFIFOStatus(2);
		inrdTable(5,1) << std::showbase << std::hex << ((dduValue >> 10) & 0xF);
		inrdTable(5,1).setClass("none");

		inrdTable(6,0) << "InRD near full warning";
		//dduValue = myDDU->checkFIFO(2);
		inrdTable(6,1) << std::showbase << std::hex << ((dduValue >> 4) & 0xF);
		if ((dduValue >> 4) & 0xF) inrdTable(6,1).setClass("warning");
		else inrdTable(6,1).setClass("ok");

		inrdTable(7,0) << "Ext. FIFO almost-full";
		//dduValue = myDDU->checkFIFO(2);
		inrdTable(7,1) << std::showbase << std::hex << ((dduValue) & 0xF);
		if ((dduValue) & 0xF) inrdTable(7,1).setClass("questionable");
		else inrdTable(7,1).setClass("ok");

		inrdTable(8,0) << "Special decode bits";
		//dduValue = myDDU->checkFIFO(2);
		inrdTable(8,1) << std::showbase << std::hex << ((dduValue >> 8) & 0x43);
		if ((dduValue >> 8) & 0x81) inrdTable(8,1).setClass("warning");
		else inrdTable(8,1).setClass("ok");
		dduComments = DDUDebugger::FIFO2((dduValue >> 8) & 0x43);
		for (std::map<std::string,std::string>::iterator iComment = dduComments.begin();
			iComment != dduComments.end();
			iComment++) {
			inrdTable(8,2) << cgicc::div(iComment->first)
				.set("class",iComment->second);
		}

		inrdTable(9,0) << "Timeout-EndBusy";
		dduValue = myDDU->readFIFOStatus(3);
		inrdTable(9,1) << std::showbase << std::hex << ((dduValue >> 12) & 0xF);
		if ((dduValue >> 12) & 0xF) inrdTable(9,1).setClass("bad");
		else inrdTable(9,1).setClass("ok");

		inrdTable(10,0) << "Timeout-EndWait";
		//dduValue = myDDU->checkFIFO(2);
		inrdTable(10,1) << std::showbase << std::hex << ((dduValue >> 8) & 0xF);
		if ((dduValue >> 8) & 0xF) inrdTable(10,1).setClass("warning");
		else inrdTable(10,1).setClass("ok");

		inrdTable(11,0) << "Timeout-Start";
		//dduValue = myDDU->checkFIFO(2);
		inrdTable(11,1) << std::showbase << std::hex << ((dduValue >> 4) & 0xF);
		if ((dduValue >> 4) & 0xF) inrdTable(11,1).setClass("warning");
		else inrdTable(11,1).setClass("ok");

		inrdTable(12,0) << "Lost-in-data error";
		//dduValue = myDDU->checkFIFO(2);
		inrdTable(12,1) << std::showbase << std::hex << ((dduValue) & 0xF);
		if ((dduValue) & 0xF) inrdTable(12,1).setClass("bad");
		else inrdTable(12,1).setClass("ok");

		inrdTable(13,0) << "Raw ext. FIFO empty";
		dduValue = myDDU->readFFError();
		inrdTable(13,1) << std::showbase << std::hex << ((dduValue >> 10) & 0xF);
		inrdTable(13,1).setClass("none");

		inrdTable(14,0) << "InRD FIFO full";
		//dduValue = myDDU->readFFError(2);
		inrdTable(14,1) << std::showbase << std::hex << ((dduValue >> 4) & 0xF);
		if ((dduValue >> 4) & 0xF) inrdTable(14,1).setClass("bad");
		else inrdTable(14,1).setClass("ok");

		inrdTable(15,0) << "Ext. FIFO full";
		//dduValue = myDDU->readFFError(2);
		inrdTable(15,1) << std::showbase << std::hex << ((dduValue) & 0xF);
		if ((dduValue) & 0xF) inrdTable(15,1).setClass("bad");
		else inrdTable(15,1).setClass("ok");

		inrdTable(16,0) << "Special decode bits";
		//dduValue = myDDU->readFFError(2);
		inrdTable(16,1) << std::showbase << std::hex << ((dduValue >> 8) & 0x43);
		if ((dduValue >> 8) & 0x1) inrdTable(16,1).setClass("bad");
		else inrdTable(16,1).setClass("ok");
		dduComments = DDUDebugger::FFError((dduValue >> 8) & 0x43);
		for (std::map<std::string,std::string>::iterator iComment = dduComments.begin();
			iComment != dduComments.end();
			iComment++) {
			inrdTable(16,2) << cgicc::div(iComment->first)
				.set("class",iComment->second);
		}

		inrdTable(17,0) << "InRD hard error";
		dduValue = myDDU->readInRDStat();
		inrdTable(17,1) << std::showbase << std::hex << ((dduValue >> 12) & 0xF);
		if ((dduValue >> 12) & 0xF) inrdTable(17,1).setClass("bad");
		else inrdTable(17,1).setClass("ok");

		inrdTable(18,0) << "InRD sync error";
		//dduValue = myDDU->checkFIFO(2);
		inrdTable(18,1) << std::showbase << std::hex << ((dduValue >> 8) & 0xF);
		if ((dduValue >> 8) & 0xF) inrdTable(18,1).setClass("warning");
		else inrdTable(18,1).setClass("ok");

		inrdTable(19,0) << "InRD single event error";
		//dduValue = myDDU->checkFIFO(2);
		inrdTable(19,1) << std::showbase << std::hex << ((dduValue >> 4) & 0xF);
		if ((dduValue >> 4) & 0xF) inrdTable(19,1).setClass("questionable");
		else inrdTable(19,1).setClass("ok");

		inrdTable(20,0) << "InRD timeout error";
		//dduValue = myDDU->checkFIFO(2);
		inrdTable(20,1) << std::showbase << std::hex << ((dduValue) & 0xF);
		if ((dduValue) & 0xF) inrdTable(20,1).setClass("bad");
		else inrdTable(20,1).setClass("ok");

		inrdTable(21,0) << "InRD multiple transmit errors";
		dduValue = myDDU->readInCHistory();
		inrdTable(21,1) << std::showbase << std::hex << ((dduValue) & 0xF);
		if ((dduValue) & 0xF) inrdTable(21,1).setClass("bad");
		else inrdTable(21,1).setClass("ok");

		inrdTable(22,0) << "Special decode bits";
		//dduValue = myDDU->readFFError(2);
		inrdTable(22,1) << std::showbase << std::hex << ((dduValue) & 0xFFF);
		if ((dduValue) & 0xC00) inrdTable(22,1).setClass("bad");
		else if ((dduValue) & 0x2DF) inrdTable(22,1).setClass("warning");
		else inrdTable(22,1).setClass("ok");
		dduComments = DDUDebugger::FFError((dduValue) & 0xFFF);
		for (std::map<std::string,std::string>::iterator iComment = dduComments.begin();
			iComment != dduComments.end();
			iComment++) {
			inrdTable(22,2) << cgicc::div(iComment->first)
				.set("class",iComment->second);
		}

		*out << inrdTable.printSummary() << std::endl;

		*out << cgicc::fieldset() << std::endl;

		// Clever trick to help with formating the cut-and-paste.
		*out << cgicc::br()
			.set("style","display: none") << std::endl;




		//myCrate->getVMEController()->CAEN_err_reset();
		// Display the big debugging information block
		*out << cgicc::fieldset()
			.set("class","fieldset") << std::endl;
		*out << cgicc::div("DDU Diagnostic Trap Decoding")
			.set("class","legend") << std::endl;

		if (debugTrapValid) {

			// Here it is.
			std::vector<uint16_t> lcode = myDDU->readDebugTrap(DDUFPGA);
			std::vector<std::string> bigComments = DDUDebugger::DDUDebugTrap(lcode, myDDU);

			std::stringstream diagCode;
			diagCode << std::setfill('0');
			diagCode << std::hex;
			diagCode << std::setw(8) << lcode[5] << " ";
			diagCode << std::setw(8) << lcode[4] << " ";
			diagCode << std::setw(8) << lcode[3] << " ";
			diagCode << std::setw(8) << lcode[2] << " ";
			diagCode << std::setw(8) << lcode[1] << " ";
			diagCode << std::setw(8) << lcode[0];

			// Don't tell anybody, but a div with display: inline is just another
			//  name for a span.  I do this to solve a problem with cgicc and the
			//  way I handle InFPGAs later.  It's not needed at this point, but
			//  internal consistancy is always best.
			*out << cgicc::div("Trap value (192 bits): ")
				.set("style","font-weight: bold; display: inline;");

			*out << cgicc::div(diagCode.str())
				.set("style","display: inline;") << std::endl;

			*out << cgicc::div()
				.set("style","width: 95%; font-size: 10pt; margin: 10px auto 10px auto; border: 2px solid #666; padding: 2px; font-family: monospace;") << std::endl;

			for (std::vector<std::string>::iterator iComment = bigComments.begin(); iComment != bigComments.end(); iComment++) {
				*out << (*iComment) << cgicc::br();
			}

			*out << cgicc::div();

			*out << cgicc::div("The status registers are frozen in the trap only after an error occurs.  The values in the trap remain valid until a sync reset.")
				.set("style","font-size: 8pt;") << std::endl;

		} else {
			*out << cgicc::div("Diagnostic trap is only valid after a DDU error has been detected.")
				.set("style","color: #A00; font-size: 10pt;") << std::endl;
		}

		*out << cgicc::fieldset() << std::endl;

		// Clever trick to help with formating the cut-and-paste.
		*out << cgicc::br()
			.set("style","display: none") << std::endl;

		


		//myCrate->getVMEController()->CAEN_err_reset();
		*out << cgicc::fieldset()
			.set("class","fieldset") << std::endl;
		*out << cgicc::div("CSC Board Occupancies and Percentages")
			.set("class","legend") << std::endl;

		// Pick up the occupancies.
		//myCrate->getVMEController()->CAEN_err_reset();

		// Now we grab what we want.
		unsigned long int scalar = myDDU->readL1Scaler(DDUFPGA);

		// Make us a DataTable!
		DataTable occuTable;
		occuTable->set("id", "occuTable");

		occuTable(0,0) << "Fiber Input";
		occuTable(0,1) << "Chamber";
		occuTable(0,1)->set("style", "border-right: double 3px #000;");
		occuTable(0,2) << "DMB<sup>*</sup>";
		occuTable(0,3) << "ALCT";
		occuTable(0,4) << "TMB";
		occuTable(0,5) << "CFEB";

		// It's good to know the fibers that are alive and the fibers that are killed.
		long int liveFibers = (myDDU->readFiberStatus(INFPGA0)&0x000000ff) | ((myDDU->readFiberStatus(INFPGA1)&0x000000ff)<<8);
		long int killFiber = myDDU->readKillFiber();

		// PGK It turns out that this is an automatically shifting register.
		//  If you read from it 60 times, you get 15 sets of 4 values, all
		//  different.
		//  This means I need to do this 15 times per board.
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			std::vector<uint32_t> lcode = myDDU->readOccupancyMonitor();
			unsigned long int DMBval = lcode[0] & 0x0fffffff;
			unsigned long int ALCTval = lcode[1] & 0x0fffffff;
			unsigned long int TMBval = lcode[2] & 0x0fffffff;
			unsigned long int CFEBval = lcode[3] & 0x0fffffff;

			occuTable(iFiber + 1,0) << iFiber;
			Chamber *thisChamber = myDDU->getChamber(iFiber);

			std::string chamberClass = "ok";

			if (thisChamber != NULL) {
				occuTable(iFiber + 1,1) << thisChamber->name();
				if (!(killFiber & (1<<iFiber))) chamberClass = "none";
				else if (!(liveFibers & (1<<iFiber))) chamberClass = "undefined";
				else if (fibersWithErrors & (1<<iFiber)) chamberClass = "bad";
				occuTable(iFiber + 1,1)->set("style", "border-right: double 3px #000;");

				occuTable(iFiber + 1,1).setClass(chamberClass);

				occuTable(iFiber + 1,2) << DMBval;
				occuTable(iFiber + 1,2) << "<br />" << std::setprecision(3) << (scalar ? DMBval*100./scalar : 0) << "%";

				occuTable(iFiber + 1,3) << ALCTval;
				occuTable(iFiber + 1,3) << "<br />" << std::setprecision(3) << (DMBval ? ALCTval*100./DMBval : 0) << "%";

				occuTable(iFiber + 1,4) << TMBval;
				occuTable(iFiber + 1,4) << "<br />" << std::setprecision(3) << (DMBval ? TMBval*100./DMBval : 0) << "%";

				occuTable(iFiber + 1,5) << CFEBval;
				occuTable(iFiber + 1,5) << "<br />" << std::setprecision(3) << (DMBval ? CFEBval*100./DMBval : 0) << "%";

			} else {
				occuTable(iFiber + 1,1) << "???";
				occuTable(iFiber + 1,1).setClass("undefined");
				occuTable(iFiber + 1,2) << "N/A";
				occuTable(iFiber + 1,3) << "N/A";
				occuTable(iFiber + 1,4) << "N/A";
				occuTable(iFiber + 1,5) << "N/A";
			}
		}

		*out << occuTable.toHTML() << std::endl;

		*out << cgicc::div("<sup>*</sup>The DMB percentages are relative to #L1As; other board percentages are relative to #LCTxL1A hits on the CSC.")
			.set("style","font-size: 8pt;") << std::endl;
		*out << cgicc::div()
			.set("style","font-size: 8pt;") << std::endl;
		*out << cgicc::span("Red chamber labels")
			.set("class","bad") << std::endl;
		*out << cgicc::span(" denote chambers with an error.  The tables above will help diagnose the problem.") << std::endl;
		*out << cgicc::div() << std::endl;

		*out << cgicc::fieldset() << std::endl;

		*out << Footer() << std::endl;

	} catch (xcept::Exception &e) {
		std::ostringstream error;
		error << "Exception caught!";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
		
		std::vector<xcept::ExceptionInformation> history = e2.getHistory();
		for (std::vector<xcept::ExceptionInformation>::iterator iError = history.begin(); iError != history.end(); iError++) {
			DataTable exceptionTable;
			unsigned int iRow = 0;
			std::map<std::string, std::string> messages = iError->getProperties();
			for (std::map<std::string, std::string>::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++) {
				exceptionTable(iRow, 0) << iMessage->first << ":";
				exceptionTable(iRow, 1) << iMessage->second;
				iRow++;
			}
			*out << cgicc::div(exceptionTable.toHTML())
			.set("class", "exception");
		}
	}
}



void emu::fed::EmuFCrateHyperDAQ::InFpga(xgi::Input *in, xgi::Output *out)
{

	try {
		
		// PGK Patented check-for-initialization
		if (crateVector_.size()==0) {
			LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
			return Default(in,out);
		}

		cgicc::Cgicc cgi(in);
		
		// First, I need a crate.
		std::pair<unsigned int, Crate *> cratePair = getCGICrate(cgi);
		unsigned int cgiCrate = cratePair.first;
		Crate *myCrate = cratePair.second;

		std::pair<unsigned int, DDU *> boardPair = getCGIBoard<DDU>(cgi);
		unsigned int cgiDDU = boardPair.first;
		DDU *myDDU = boardPair.second;

		std::stringstream sTitle;
		sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DDU InFPGA Controls (RUI #" << myCrate->getRUI(myDDU->slot()) << ")";
		std::vector<std::string> jsFileNames;
		jsFileNames.push_back("tableToggler.js");
		jsFileNames.push_back("errorFlasher.js");
		*out << Header(sTitle.str(),jsFileNames);

		// PGK Select-a-Crate/Slot
		*out << cgicc::fieldset()
			.set("class","fieldset") << std::endl;
		*out << cgicc::div("View this page for a different crate/board")
			.set("class","legend") << std::endl;

		*out << selectACrate("InFpga","ddu",cgiDDU,cgiCrate) << std::endl;

		*out << cgicc::span("Configuration located at " + xmlFile_.toString())
			.set("style","color: #A00; font-size: 10pt;") << std::endl;
		
		*out << cgicc::fieldset() << std::endl;
		*out << cgicc::br() << std::endl;
		
		// This is used to check if the debug trap is valid.  One for each device.
		bool debugTrapValid[2] = {
			false,
			false
		};
		
		// Start reading some registers!
		//myCrate->getVMEController()->CAEN_err_reset();

		// Do this for both InFPGAs
		enum DEVTYPE devTypes[2] = {
			INFPGA0,
			INFPGA1
		};
		std::string devNames[2] = {
			"InFPGA0 (fibers 7-0)",
			"InFPGA1 (fibers 14-8)"
		};

		// Display general InFPGA status information
		*out << cgicc::fieldset()
			.set("class","normal") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << cgicc::a("&plusmn;")
			.set("class","openclose")
			.set("onClick","javascript:toggle('generalTable')") << std::endl;
		*out << "General InFPGA Information" << cgicc::div() << std::endl;

		DataTable generalTable;
		generalTable->set("id", "generalTable").set("class", "data");

		// Names have to come first, because there is only one of each.
		generalTable(0,0) << "Register";
		generalTable(0,1) << "Value";
		generalTable(0,2) << "Decoded Status";
		generalTable(0,3) << "Value";
		generalTable(0,4) << "Decoded Status";
		generalTable[0]->set("class", "header");
		
		generalTable(2,0) << "InFPGA status (32-bit)";
		generalTable(3,0) << "L1 Event Scaler0/2 (24-bit)";
		generalTable(4,0) << "L1 Event Scaler1/3 (24-bit)";
		
		generalTable(1,1)->set("colspan", "2").set("style", "font-weight: bold; text-align: center; border-right: 3px double #000;");
		generalTable(1,1) << devNames[0];
		generalTable(1,2)->set("colspan", "2").set("style", "font-weight: bold; text-align: center; border-right: 3px double #000;");
		generalTable(1,2) << devNames[1];
		
		generalTable(0,2)->set("style", "border-right: double 3px #000;");
		generalTable(2,2)->set("style", "border-right: double 3px #000;");
		generalTable(3,2)->set("style", "border-right: double 3px #000;");
		generalTable(4,2)->set("style", "border-right: double 3px #000;");

		for (unsigned int iDevType = 0; iDevType < 2; iDevType++) {
			enum DEVTYPE dt = devTypes[iDevType];

			unsigned long int infpgastat = myDDU->readFPGAStatus(dt);
			generalTable(2,iDevType*2+1) << std::showbase << std::hex << infpgastat;
			if (infpgastat & 0x00004000) generalTable(2,iDevType*2+1).setClass("warning");
			if (infpgastat & 0x00008000) {
				generalTable(2,iDevType*2+1).setClass("bad");
				debugTrapValid[iDevType] = true;
			}
			if (!(infpgastat & 0x0000C000)) generalTable(2,iDevType*2+1).setClass("ok");
			std::map<std::string, std::string> infpgastatComments = DDUDebugger::InFPGAStat(dt,infpgastat);
			for (std::map<std::string,std::string>::iterator iComment = infpgastatComments.begin();
				iComment != infpgastatComments.end();
				iComment++) {
				generalTable(2,iDevType*2+2) << cgicc::div(iComment->first)
					.set("class",iComment->second);
			}

			// PGK Flaking out
			//unsigned long int L1Scaler = myDDU->readL1Scaler(dt);
			unsigned long int L1Scaler = myDDU->readL1Scaler(dt);
			generalTable(3,iDevType*2+1) << L1Scaler;

			// PGK Flaking out
			//unsigned long int L1Scaler1 = myDDU->readL1Scaler1(dt);
			unsigned long int L1Scaler1 = myDDU->readL1Scaler1(dt);
			generalTable(4,iDevType*2+1) << L1Scaler1;

		}
		
		
		// Summary
		*out << generalTable.printSummary() << std::endl;

		// Now print the concatonated table.
		*out << generalTable.toHTML();

		*out << cgicc::fieldset() << std::endl;

		// Clever trick to help with formating the cut-and-paste.
		*out << cgicc::br()
			.set("style","display: none") << std::endl;


		// Display individual fiber status information
		*out << cgicc::fieldset()
			.set("class","normal") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << cgicc::a("&plusmn;")
			.set("class","openclose")
			.set("onClick","javascript:toggle('fiberTable')") << std::endl;
		*out << "Individual Fiber Information" << cgicc::div() << std::endl;

		DataTable fiberTable;
		fiberTable->set("id", "fiberTable").set("class", "data");

		// Names have to come first, because there is only one of each.
		fiberTable(0,0) << "Register";
		fiberTable(0,1) << "Value";
		fiberTable(0,2) << "Decoded Chambers";
		fiberTable(0,3) << "Value";
		fiberTable(0,4) << "Decoded Chambers";
		fiberTable[0]->set("class", "header");
		
		fiberTable(21,0) << "DMB full";
		fiberTable(3,0) << "DMB warning";
		fiberTable(4,0) << "Connection error";
		fiberTable(5,0) << "Link active";
		fiberTable(6,0) << "Stuck data";
		fiberTable(7,0) << "L1A mismatch";
		fiberTable(8,0) << "GT-Rx error";
		fiberTable(9,0) << "Timeout-start";
		fiberTable(10,0) << "Timeout-end busy";
		fiberTable(11,0) << "Timeout-end wait";
		fiberTable(12,0) << "SCA full history";
		fiberTable(13,0) << "CSC transmit error";
		fiberTable(14,0) << "DDU lost-in-event error";
		fiberTable(15,0) << "DDU lost-in-data error";
		
		fiberTable(1,1)->set("colspan", "2").set("style", "font-weight: bold; text-align: center; border-right: 3px double #000;");
		fiberTable(1,1) << devNames[0];
		fiberTable(1,2)->set("colspan", "2").set("style", "font-weight: bold; text-align: center; border-right: 3px double #000;");
		fiberTable(1,2) << devNames[1];
		
		fiberTable(0,2)->set("style", "border-right: double 3px #000;");
		fiberTable(2,2)->set("style", "border-right: double 3px #000;");
		fiberTable(3,2)->set("style", "border-right: double 3px #000;");
		fiberTable(4,2)->set("style", "border-right: double 3px #000;");
		fiberTable(5,2)->set("style", "border-right: double 3px #000;");
		fiberTable(6,2)->set("style", "border-right: double 3px #000;");
		fiberTable(7,2)->set("style", "border-right: double 3px #000;");
		fiberTable(8,2)->set("style", "border-right: double 3px #000;");
		fiberTable(9,2)->set("style", "border-right: double 3px #000;");
		fiberTable(10,2)->set("style", "border-right: double 3px #000;");
		fiberTable(11,2)->set("style", "border-right: double 3px #000;");
		fiberTable(12,2)->set("style", "border-right: double 3px #000;");
		fiberTable(13,2)->set("style", "border-right: double 3px #000;");
		fiberTable(14,2)->set("style", "border-right: double 3px #000;");
		fiberTable(15,2)->set("style", "border-right: double 3px #000;");

		for (unsigned int iDevType = 0; iDevType < 2; iDevType++) {
			enum DEVTYPE dt = devTypes[iDevType];
			unsigned int fiberOffset = (dt == INFPGA0 ? 0 : 8);

			unsigned int readDMBWarning = myDDU->readDMBWarning(dt);
			fiberTable(2,iDevType*2+1) << std::showbase << std::hex << ((readDMBWarning >> 8) & 0xff);
			if (((readDMBWarning >> 8) & 0xff)) fiberTable(2,iDevType*2+1).setClass("bad");
			else fiberTable(2,iDevType*2+1).setClass("ok");
			for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
				if (((readDMBWarning >> 8) & 0xff) & (1<<iFiber)) {
					//fibersWithErrors |= (1<<(iFiber + fiberOffset));
					fiberTable(2,iDevType*2+2) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
						.set("class","red");
				}
			}

			fiberTable(3,iDevType*2+1) << std::showbase << std::hex << ((readDMBWarning) & 0xff);
			if (((readDMBWarning) & 0xff)) fiberTable(2,iDevType*2+1).setClass("warning");
			else fiberTable(3,iDevType*2+1).setClass("ok");
			for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
				if (((readDMBWarning) & 0xff) & (1<<iFiber)) {
					fiberTable(3,iDevType*2+2) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
						.set("class","orange");
				}
			}

			unsigned int checkFiber = myDDU->readFiberStatus(dt);
			fiberTable(4,iDevType*2+1) << std::showbase << std::hex << ((checkFiber >> 8) & 0xff);
			if (((checkFiber >> 8) & 0xff)) fiberTable(4,iDevType*2+1).setClass("bad");
			else fiberTable(4,iDevType*2+1).setClass("ok");
			for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
				if (((checkFiber >> 8) & 0xff) & (1<<iFiber)) {
					//fibersWithErrors |= (1<<(iFiber + fiberOffset));
					fiberTable(4,iDevType*2+2) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
						.set("class","red");
				}
			}

			fiberTable(5,iDevType*2+1) << std::showbase << std::hex << ((checkFiber) & 0xff);
			fiberTable(5,iDevType*2+1).setClass("ok");
			for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
				if (((checkFiber) & 0xff) & (1<<iFiber)) {
					fiberTable(5,iDevType*2+2) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
						.set("class","none");
				}
			}

			unsigned int readDMBSync = myDDU->readDMBSync(dt);
			fiberTable(6,iDevType*2+1) << std::showbase << std::hex << ((readDMBSync >> 8) & 0xff);
			if (((readDMBSync >> 8) & 0xff)) fiberTable(6,iDevType*2+1).setClass("bad");
			else fiberTable(6,iDevType*2+1).setClass("ok");
			for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
				if (((readDMBSync >> 8) & 0xff) & (1<<iFiber)) {
					//fibersWithErrors |= (1<<(iFiber + fiberOffset));
					fiberTable(6,iDevType*2+2) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
						.set("class","red");
				}
			}

			fiberTable(7,iDevType*2+1) << std::showbase << std::hex << ((readDMBSync) & 0xff);
			if (((readDMBSync) & 0xff)) fiberTable(7,iDevType*2+1).setClass("warning");
			else fiberTable(7,iDevType*2+1).setClass("ok");
			for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
				if (((readDMBSync) & 0xff) & (1<<iFiber)) {
					fiberTable(7,iDevType*2+2) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
						.set("class","orange");
				}
			}

			unsigned int readRxError = myDDU->readRxError(dt);
			fiberTable(8,iDevType*2+1) << std::showbase << std::hex << ((readRxError >> 8) & 0xff);
			if (((readRxError >> 8) & 0xff)) fiberTable(8,iDevType*2+1).setClass("questionable");
			else fiberTable(8,iDevType*2+1).setClass("ok");
			for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
				if (((readRxError >> 8) & 0xff) & (1<<iFiber)) {
					fiberTable(8,iDevType*2+2) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
						.set("class","blue");
				}
			}

			fiberTable(9,iDevType*2+1) << std::showbase << std::hex << ((readRxError) & 0xff);
			if (((readRxError) & 0xff)) fiberTable(9,iDevType*2+1).setClass("bad");
			else fiberTable(9,iDevType*2+1).setClass("ok");
			for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
				if (((readRxError) & 0xff) & (1<<iFiber)) {
					//fibersWithErrors |= (1<<(iFiber + fiberOffset));
					fiberTable(9,iDevType*2+2) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
						.set("class","red");
				}
			}

			unsigned int readTimeout = myDDU->readTimeout(dt);
			fiberTable(20,iDevType*2+1) << std::showbase << std::hex << ((readTimeout >> 8) & 0xff);
			if (((readTimeout >> 8) & 0xff)) fiberTable(20,iDevType*2+1).setClass("bad");
			else fiberTable(20,iDevType*2+1).setClass("ok");
			for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
				if (((readTimeout >> 8) & 0xff) & (1<<iFiber)) {
					//fibersWithErrors |= (1<<(iFiber + fiberOffset));
					fiberTable(20,iDevType*2+2) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
						.set("class","red");
				}
			}

			fiberTable(11,iDevType*2+1) << std::showbase << std::hex << ((readTimeout) & 0xff);
			if (((readTimeout) & 0xff)) fiberTable(11,iDevType*2+1).setClass("bad");
			else fiberTable(11,iDevType*2+1).setClass("ok");
			for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
				if (((readTimeout) & 0xff) & (1<<iFiber)) {
					//fibersWithErrors |= (1<<(iFiber + fiberOffset));
					fiberTable(11,iDevType*2+2) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
						.set("class","red");
				}
			}

			unsigned int readTxError = myDDU->readTxError(dt);
			fiberTable(12,iDevType*2+1) << std::showbase << std::hex << ((readTxError >> 8) & 0xff);
			fiberTable(12,iDevType*2+1).setClass("ok");
			for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
				if (((readTxError >> 8) & 0xff) & (1<<iFiber)) {
					fiberTable(12,iDevType*2+2) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
						.set("class","none");
				}
			}

			fiberTable(13,iDevType*2+1) << std::showbase << std::hex << ((readTxError) & 0xff);
			if (((readTxError) & 0xff)) fiberTable(13,iDevType*2+1).setClass("bad");
			else fiberTable(13,iDevType*2+1).setClass("ok");
			for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
				if (((readTxError) & 0xff) & (1<<iFiber)) {
					//fibersWithErrors |= (1<<(iFiber + fiberOffset));
					fiberTable(13,iDevType*2+2) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
						.set("class","red");
				}
			}

			unsigned int readLostError = myDDU->readLostError(dt);
			fiberTable(14,iDevType*2+1) << std::showbase << std::hex << ((readLostError >> 8) & 0xff);
			if (((readLostError) & 0xff)) fiberTable(14,iDevType*2+1).setClass("warning");
			else fiberTable(14,iDevType*2+1).setClass("ok");
			for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
				if (((readLostError >> 8) & 0xff) & (1<<iFiber)) {
					fiberTable(14,iDevType*2+2) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
						.set("class","orange");
				}
			}

			fiberTable(15,iDevType*2+1) << std::showbase << std::hex << ((readLostError) & 0xff);
			if (((readLostError) & 0xff)) fiberTable(15,iDevType*2+1).setClass("bad");
			else fiberTable(15,iDevType*2+1).setClass("ok");
			for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
				if (((readLostError) & 0xff) & (1<<iFiber)) {
					//fibersWithErrors |= (1<<(iFiber + fiberOffset));
					fiberTable(15,iDevType*2+2) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
						.set("class","red");
				}
			}
		}

		// Summary
		*out << fiberTable.printSummary() << std::endl;

		// Now print the concatonated table.
		*out << fiberTable.toHTML();

		*out << cgicc::fieldset() << std::endl;

		// Clever trick to help with formating the cut-and-paste.
		*out << cgicc::br()
			.set("style","display: none") << std::endl;


		// Display other fiber information
		*out << cgicc::fieldset()
			.set("class","normal") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << cgicc::a("&plusmn;")
			.set("class","openclose")
			.set("onClick","javascript:toggle('otherTable')") << std::endl;
		*out << "Other Fiber/InRD Information" << cgicc::div() << std::endl;

		DataTable otherTable;
		otherTable->set("id", "otherTable").set("class", "data");

		// Names have to come first, because there is only one of each.
		otherTable(0,0) << "Register";
		otherTable(0,1) << "Value";
		otherTable(0,2) << "Decoded Chambers/Status";
		otherTable(0,3) << "Value";
		otherTable(0,4) << "Decoded Chambers/Status";
		
		otherTable(2,0) << "Input buffer empty";
		otherTable(3,0) << "Special decode bits (8-bit)";
		otherTable(4,0) << "Input buffer full history";
		otherTable(5,0) << "Special decode bits (4-bit)";
		otherTable(6,0) << "InRD0/2 C-code status (8-bit)";
		otherTable(7,0) << "InRD1/3 C-code status (8-bit)";
		
		otherTable(1,1)->set("colspan", "2").set("style", "font-weight: bold; text-align: center; border-right: 3px double #000;");
		otherTable(1,1) << devNames[0];
		otherTable(1,2)->set("colspan", "2").set("style", "font-weight: bold; text-align: center; border-right: 3px double #000;");
		otherTable(1,2) << devNames[1];
		
		otherTable(0,2)->set("style", "border-right: double 3px #000;");
		otherTable(2,2)->set("style", "border-right: double 3px #000;");
		otherTable(3,2)->set("style", "border-right: double 3px #000;");
		otherTable(4,2)->set("style", "border-right: double 3px #000;");
		otherTable(5,2)->set("style", "border-right: double 3px #000;");
		otherTable(6,2)->set("style", "border-right: double 3px #000;");
		otherTable(7,2)->set("style", "border-right: double 3px #000;");

		for (unsigned int iDevType = 0; iDevType < 2; iDevType++) {
			enum DEVTYPE dt = devTypes[iDevType];
			unsigned int fiberOffset = (dt == INFPGA0 ? 0 : 8);

			unsigned int readFIFOStat = myDDU->readFIFOStatus(dt);
			otherTable(2,iDevType*2+1) << std::showbase << std::hex << ((readFIFOStat >> 8) & 0xff);
			//if (((readFIFOStat >> 8) & 0xff)) otherTable(0,iDevType*2+1).setClass("bad");
			otherTable(2,iDevType*2+1).setClass("ok");
			for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
				if (((readFIFOStat >> 8) & 0xff) & (1<<iFiber)) {
					otherTable(20,iDevType*2+2) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
						.set("class","none");
				}
			}

			otherTable(3,iDevType*2+1) << std::showbase << std::hex << ((readFIFOStat) & 0xff);
			if (((readFIFOStat) & 0xfc)) otherTable(3,iDevType*2+1).setClass("warning");
			else otherTable(3,iDevType*2+1).setClass("ok");
			std::map<std::string, std::string> readFIFOStatComments = DDUDebugger::FIFOStat(dt,(readFIFOStat) & 0xff);
			for (std::map< std::string, std::string >::iterator iComment = readFIFOStatComments.begin();
				iComment != readFIFOStatComments.end();
				iComment++) {
				otherTable(3,iDevType*2+2) << cgicc::div(iComment->first)
					.set("class",iComment->second) << std::endl;
			}

			unsigned int readFIFOFull = myDDU->readFIFOFull(dt);
			otherTable(4,iDevType*2+1) << std::showbase << std::hex << ((readFIFOFull) & 0xff);
			if (((readFIFOFull) & 0xff)) otherTable(4,iDevType*2+1).setClass("bad");
			else otherTable(4,iDevType*2+1).setClass("ok");
			for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
				if (((readFIFOFull) & 0xff) & (1<<iFiber)) {
					otherTable(4,iDevType*2+2) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
						.set("class","red");
				}
			}

			otherTable(5,iDevType*2+1) << std::showbase << std::hex << ((readFIFOFull >> 8) & 0xff);
			if (((readFIFOFull >> 8) & 0xf)) otherTable(5,iDevType*2+1).setClass("warning");
			else otherTable(5,iDevType*2+1).setClass("ok");
			std::map<std::string, std::string> readFIFOFullComments = DDUDebugger::FIFOFull(dt,(readFIFOFull >> 8) & 0xff);
			for (std::map< std::string, std::string >::iterator iComment = readFIFOFullComments.begin();
				iComment != readFIFOFullComments.end();
				iComment++) {
				otherTable(5,iDevType*2+2) << cgicc::div(iComment->first)
					.set("class",iComment->second) << std::endl;
			}

			unsigned int readCCodeStat = myDDU->readCCodeStatus(dt);
			otherTable(6,iDevType*2+1) << std::showbase << std::hex << ((readCCodeStat) & 0xff);
			if ((readCCodeStat & 0xff) == 0x20) otherTable(6,iDevType*2+1).setClass("warning");
			else if (readCCodeStat & 0xff) otherTable(6,iDevType*2+1).setClass("bad");
			else otherTable(6,iDevType*2+1).setClass("ok");
			std::map<std::string, std::string> readCCodeStatComments = DDUDebugger::CCodeStat(dt,(readCCodeStat) & 0xff);
			for (std::map< std::string, std::string >::iterator iComment = readCCodeStatComments.begin();
				iComment != readCCodeStatComments.end();
				iComment++) {
				otherTable(6,iDevType*2+2) << cgicc::div(iComment->first)
					.set("class",iComment->second) << std::endl;
			}

			otherTable(7,iDevType*2+1) << std::showbase << std::hex << ((readCCodeStat >> 8) & 0xff);
			if (((readCCodeStat >> 8) & 0xff) == 0x20) otherTable(7,iDevType*2+1).setClass("warning");
			else if (((readCCodeStat >> 8) & 0xff)) otherTable(7,iDevType*2+1).setClass("bad");
			else otherTable(7,iDevType*2+1).setClass("ok");
			readCCodeStatComments = DDUDebugger::CCodeStat(dt,(readCCodeStat >> 8) & 0xff);
			for (std::map< std::string, std::string >::iterator iComment = readCCodeStatComments.begin();
				iComment != readCCodeStatComments.end();
				iComment++) {
				otherTable(7,iDevType*2+2) << cgicc::div(iComment->first)
					.set("class",iComment->second) << std::endl;
			}
			
		}

		// Summary
		*out << otherTable.printSummary() << std::endl;

		*out << otherTable.toHTML();

		// And now close the table.
		*out << cgicc::table() << std::endl;

		*out << cgicc::fieldset() << std::endl;

		// Clever trick to help with formating the cut-and-paste.
		*out << cgicc::br()
			.set("style","display: none") << std::endl;


		// Display memctrl information
		*out << cgicc::fieldset()
			.set("class","normal") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << cgicc::a("&plusmn;")
			.set("class","openclose")
			.set("onClick","javascript:toggle('memTable')") << std::endl;
		*out << "Memory Control Information (22 fifos)" << cgicc::div() << std::endl;

		DataTable memTable;
		memTable->set("id", "memTable").set("class", "data");

		// Names have to come first, because there is only one of each.
		memTable(0,0) << "Register";
		memTable(0,1) << "Value";
		memTable(0,2) << "Decoded Status";
		memTable(0,3) << "Value";
		memTable(0,4) << "Decoded Status";
		
		memTable(2,0) << "FIFOs Used, Fibers 3-0/11-8";
		memTable(3,0) << "FIFOs Used, Fibers 7-4/15-12";
		memTable(4,0) << "MemCtrl0/2 FIFOs Available";
		memTable(5,0) << "MemCtrl1/3 FIFOs Available";
		memTable(6,0) << "MemCtrl0/2 Minimum FIFOs Available";
		memTable(7,0) << "MemCtrl1/3 Minimum FIFOs Available";
		memTable(8,0) << "Write Memory Active, Fibers 1-0/9-8";
		memTable(9,0) << "Write Memory Active, Fibers 3-2/11-10";
		memTable(10,0) << "Write Memory Active, Fibers 5-4/13-12";
		memTable(11,0) << "Write Memory Active, Fibers 7-6/15-14";
		
		memTable(1,1)->set("colspan", "2").set("style", "font-weight: bold; text-align: center; border-right: 3px double #000;");
		memTable(1,1) << devNames[0];
		memTable(1,2)->set("colspan", "2").set("style", "font-weight: bold; text-align: center; border-right: 3px double #000;");
		memTable(1,2) << devNames[1];
		
		memTable(0,2)->set("style", "border-right: double 3px #000;");
		memTable(2,2)->set("style", "border-right: double 3px #000;");
		memTable(3,2)->set("style", "border-right: double 3px #000;");
		memTable(4,2)->set("style", "border-right: double 3px #000;");
		memTable(5,2)->set("style", "border-right: double 3px #000;");
		memTable(6,2)->set("style", "border-right: double 3px #000;");
		memTable(7,2)->set("style", "border-right: double 3px #000;");
		memTable(8,2)->set("style", "border-right: double 3px #000;");
		memTable(9,2)->set("style", "border-right: double 3px #000;");
		memTable(10,2)->set("style", "border-right: double 3px #000;");
		memTable(11,2)->set("style", "border-right: double 3px #000;");

		for (unsigned int iDevType = 0; iDevType < 2; iDevType++) {
			enum DEVTYPE dt = devTypes[iDevType];

			unsigned int memValue = myDDU->readFiberDiagnostics(dt,0);
			memTable(2,iDevType*2+1) << std::showbase << std::hex << memValue;
			memTable(2,iDevType*2+1).setClass("ok");
			std::map< std::string, std::string> memComments = DDUDebugger::FiberDiagnostics(dt,0,memValue);
			for (std::map< std::string, std::string >::iterator iComment = memComments.begin();
				iComment != memComments.end();
				iComment++) {
				memTable(2,iDevType*2+2) << cgicc::div(iComment->first)
					.set("class",iComment->second) << std::endl;
			}

			memValue = myDDU->readFiberDiagnostics(dt,1);
			memTable(3,iDevType*2+1) << std::showbase << std::hex << memValue;
			memTable(3,iDevType*2+1).setClass("ok");
			memComments = DDUDebugger::FiberDiagnostics(dt,1,memValue);
			for (std::map< std::string, std::string >::iterator iComment = memComments.begin();
				iComment != memComments.end();
				iComment++) {
				memTable(3,iDevType*2+2) << cgicc::div(iComment->first)
					.set("class",iComment->second) << std::endl;
			}

			memValue = myDDU->readAvailableMemory(dt);
			memTable(4,iDevType*2+1) << (memValue & 0x1f);
			if ((memValue & 0x1f) == 1) memTable(4,iDevType*2+1).setClass("warning");
			else if ((memValue & 0x1f) == 0) memTable(4,iDevType*2+1).setClass("bad");
			else memTable(4,iDevType*2+1).setClass("ok");

			//memValue = myDDU->infpga_MemAvail(dt);
			memTable(5,iDevType*2+1) << ((memValue >> 5) & 0x1f);
			if (((memValue >> 5) & 0x1f) == 1) memTable(5,iDevType*2+1).setClass("warning");
			if (((memValue >> 5) & 0x1f) == 0) memTable(5,iDevType*2+1).setClass("bad");
			else memTable(53,iDevType*2+1).setClass("ok");

			memValue = myDDU->readMinMemory(dt);
			memTable(6,iDevType*2+1) << (memValue & 0x1f);
			if ((memValue & 0x1f) == 1) memTable(6,iDevType*2+1).setClass("warning");
			else if ((memValue & 0x1f) == 0) memTable(64,iDevType*2+1).setClass("bad");
			else memTable(6,iDevType*2+1).setClass("ok");

			//memValue = myDDU->infpga_Min_Mem(dt);
			memTable(75,iDevType*2+1) << ((memValue >> 5) & 0x1f);
			if (((memValue >> 5) & 0x1f) == 1) memTable(7,iDevType*2+1).setClass("warning");
			if (((memValue >> 5) & 0x1f) == 0) memTable(7,iDevType*2+1).setClass("bad");
			else memTable(7,iDevType*2+1).setClass("ok");

			for (unsigned int ireg = 0; ireg < 4; ireg++) {
				memValue = myDDU->readActiveWriteMemory(dt,ireg);
				memTable(ireg + 8,iDevType*2+1) << std::showbase << std::hex << memValue;
				memTable(ireg + 8,iDevType*2+1).setClass("ok");
				memComments = DDUDebugger::WriteMemoryActive(dt,ireg,memValue);
				for (std::map< std::string, std::string >::iterator iComment = memComments.begin();
					iComment != memComments.end();
					iComment++) {
					memTable(ireg + 8,iDevType*2+2) << cgicc::div(iComment->first)
						.set("class",iComment->second) << std::endl;
				}
			}
		}

		// Summary
		*out << memTable.printSummary() << std::endl;

		*out << memTable.toHTML();

		// And now close the table.
		*out << cgicc::table() << std::endl;

		*out << cgicc::fieldset() << std::endl;

		// Clever trick to help with formating the cut-and-paste.
		*out << cgicc::br()
			.set("style","display: none") << std::endl;



		// Display the big debugging information block
		*out << cgicc::fieldset()
			.set("class","fieldset") << std::endl;
		*out << cgicc::div("InFPGA Diagnostic Trap Decoding")
			.set("class","legend") << std::endl;

		for (unsigned int iDevType = 0; iDevType < 2; iDevType++) {
			enum DEVTYPE dt = devTypes[iDevType];

			*out << cgicc::span()
				.set("style","background-color: #FFF; border: 2px solid #000; padding: 10px; width: 95%; margin: 10px auto 10px auto; display: block;") << std::endl;
			*out << cgicc::div(devNames[iDevType])
				.set("style","font-size: 14pt; font-weight: bold; width: 100%; text-align: center;") << std::endl;

			if (debugTrapValid[iDevType]) {

				// Here it is.
				std::vector<uint16_t> lcode = myDDU->readDebugTrap(dt);
				std::vector<std::string> bigComments = DDUDebugger::INFPGADebugTrap(lcode, dt);

				std::stringstream diagCode;
				diagCode << std::setfill('0');
				diagCode << std::hex;
				diagCode << std::setw(8) << lcode[5] << " ";
				diagCode << std::setw(8) << lcode[4] << " ";
				diagCode << std::setw(8) << lcode[3] << " ";
				diagCode << std::setw(8) << lcode[2] << " ";
				diagCode << std::setw(8) << lcode[1] << " ";
				diagCode << std::setw(8) << lcode[0];

				*out << cgicc::div("Trap value (194 bits): ")
					.set("style","font-weight: bold; display: inline;");

				*out << cgicc::div(diagCode.str())
					.set("style","display: inline;") << std::endl;

				*out << cgicc::div()
					.set("style","width: 95%; font-size: 10pt; margin: 10px auto 10px auto; border: 2px solid #666; padding: 2px; font-family: monospace;") << std::endl;

				for (std::vector<std::string>::iterator iComment = bigComments.begin(); iComment != bigComments.end(); iComment++) {
					*out << (*iComment) << cgicc::br();
				}

				*out << cgicc::div();

				*out << cgicc::div("The status registers are frozen in the trap only after an error occurs.  The values in the trap remain valid until a reset.")
					.set("style","font-size: 8pt;") << std::endl;

			} else {
				*out << cgicc::div("Diagnostic trap is only valid after an InFPGA error has been detected.")
					.set("style","color: #A00; font-size: 10pt;") << std::endl;
			}
			
			*out << cgicc::span() << std::endl;
		}

		*out << cgicc::fieldset() << std::endl;

		*out << Footer() << std::endl;
		
	} catch (xcept::Exception &e) {
		std::ostringstream error;
		error << "Exception caught!";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
		
		std::vector<xcept::ExceptionInformation> history = e2.getHistory();
		for (std::vector<xcept::ExceptionInformation>::iterator iError = history.begin(); iError != history.end(); iError++) {
			DataTable exceptionTable;
			unsigned int iRow = 0;
			std::map<std::string, std::string> messages = iError->getProperties();
			for (std::map<std::string, std::string>::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++) {
				exceptionTable(iRow, 0) << iMessage->first << ":";
				exceptionTable(iRow, 1) << iMessage->second;
				iRow++;
			}
			*out << cgicc::div(exceptionTable.toHTML())
				.set("class", "exception");
		}
	}

}




void emu::fed::EmuFCrateHyperDAQ::DDUExpert(xgi::Input *in, xgi::Output *out)
{
	
	try {
		
		// PGK Patented check-for-initialization
		if (crateVector_.size()==0) {
			LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
			return Default(in,out);
		}

		cgicc::Cgicc cgi(in);
		
		// First, I need a crate.
		std::pair<unsigned int, Crate *> cratePair = getCGICrate(cgi);
		unsigned int cgiCrate = cratePair.first;
		Crate *myCrate = cratePair.second;

		std::pair<unsigned int, DDU *> boardPair = getCGIBoard<DDU>(cgi);
		unsigned int cgiDDU = boardPair.first;
		DDU *myDDU = boardPair.second;

		std::stringstream sTitle;
		sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DDU Expert Controls (RUI #" << myCrate->getRUI(myDDU->slot()) << ")";
		std::vector<std::string> jsFileNames;
		jsFileNames.push_back("tableToggler.js");
		jsFileNames.push_back("bitFlipper.js");
		jsFileNames.push_back("errorFlasher.js");
		*out << Header(sTitle.str(),jsFileNames);

		// PGK Select-a-Crate/Slot
		*out << cgicc::fieldset()
			.set("class","fieldset") << std::endl;
		*out << cgicc::div("View this page for a different crate/board")
			.set("class","legend") << std::endl;

		*out << selectACrate("DDUExpert","ddu",cgiDDU,cgiCrate) << std::endl;

		*out << cgicc::span("Configuration located at " + xmlFile_.toString())
			.set("style","color: #A00; font-size: 10pt;") << std::endl;

		*out << cgicc::fieldset() << std::endl;
		*out << cgicc::br() << std::endl;


		//myCrate->getVMEController()->CAEN_err_reset();
		// Killfiber and xorbit read/set
		*out << cgicc::fieldset()
			.set("class","expert") << std::endl;
		*out << cgicc::div("DDU KillFiber Register")
			.set("class","legend") << std::endl;

		// Massively useful for all the things that follow.
		std::string dduTextLoad = "/"+getApplicationDescriptor()->getURN()+"/DDUTextLoad";
		std::ostringstream dduTextLoadStream;
		std::ostringstream dduValStream;
		dduValStream << cgiDDU;
		std::string dduVal = dduValStream.str();
		std::ostringstream crateValStream;
		crateValStream << cgiCrate;
		std::string crateVal = crateValStream.str();

		*out << cgicc::form()
			.set("method","GET")
			.set("action",dduTextLoad) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","command")
			.set("value","3") << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","slot")
			.set("value",dduVal) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","crate")
			.set("value",crateVal) << std::endl;

		unsigned long int currentKillFiber = myDDU->readKillFiber();
		*out << cgicc::div("Current KillFiber register value: ")
			.set("style","font-weight: bold; display: inline;");
		*out << cgicc::div()
			.set("style","display: inline;");
		*out << std::showbase << std::hex << currentKillFiber << cgicc::div() << std::endl;
		*out << cgicc::br() << std::endl;
		*out << cgicc::div("New KillFiber register value (change this below): ")
			.set("style","font-weight: bold; display: inline; color: #090;");
		std::stringstream kfValue;
		kfValue << std::hex << std::showbase << currentKillFiber;
		*out << cgicc::input()
			.set("type","text")
			.set("name","textdata")
			.set("value",kfValue.str())
			.set("size","8")
			.set("id","killFiber") << std::endl;

		*out << cgicc::input()
			.set("type","submit")
			.set("value","Load KillFiber Register") << std::endl;

		*out << cgicc::div("This is a volatile register, and will be reprogrammed on a hard reset.  If you wish to make this change perminent, change the configuration file (see the top of this page)")
			.set("style","font-size: 8pt; color: #900;") << std::endl;

		// A table for the first half of the form.
		*out << cgicc::table()
			.set("style","width: 100%; margin: 5px auto 5px auto; font-size: 8pt; border-collapse: collapse;") << std::endl;
		*out << cgicc::tr() << std::endl;

		// Knowing which chambers are actually alive is a good thing.
		long int liveFibers = (myDDU->readFiberStatus(INFPGA0)&0x000000ff) | ((myDDU->readFiberStatus(INFPGA1)&0x000000ff)<<8);
		long int killFiber = myDDU->readKillFiber();
		unsigned int fibersWithErrors = myDDU->readCSCStatus() | myDDU->readAdvancedFiberErrors();

		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			std::string chamberClass = "ok";
			if (!(killFiber & (1<<iFiber))) chamberClass = "none";
			else if (!(liveFibers & (1<<iFiber))) chamberClass = "undefined";
			else if (fibersWithErrors & (1<<iFiber)) chamberClass = "bad";
			
			*out << cgicc::td()
				.set("class",chamberClass)
				.set("style","border: 1px solid #000; border-bottom-width: 0px; font-size: 8pt; width: 6%;") << std::dec << iFiber << cgicc::td() << std::endl;
		}

		*out << cgicc::tr() << std::endl;
		*out << cgicc::tr() << std::endl;
		
		// Loop through the chambers.  They should be in fiber-order.
		for (unsigned int iFiber=0; iFiber<15; iFiber++) {
			Chamber *thisChamber = myDDU->getChamber(iFiber);
			// DDU::getChamber will return a null pointer if there is
			//  no chamber at that fiber position.
			std::string chamberClass = "ok";
			if (thisChamber != NULL) {
				if (!(killFiber & (1<<iFiber))) chamberClass = "none";
				else if (!(liveFibers & (1<<iFiber))) chamberClass = "undefined";
				else if (fibersWithErrors & (1<<iFiber)) chamberClass = "bad";

				*out << cgicc::td(thisChamber->name())
					.set("class",chamberClass)
					.set("style","border: 1px solid #000; border-top-width: 0px; width: 6%; font-weight: bold;") << std::endl;
			} else {
				*out << cgicc::td("???")
					.set("class","undefined")
					.set("style","border: 1px solid #000; border-top-width: 0px; width: 6%; font-weight: bold;") << std::endl;
			}
		}
		*out << cgicc::tr();

		*out << cgicc::tr() << std::endl;
		for (unsigned int ifiber = 0; ifiber < 15; ifiber++) {
			*out << cgicc::td() << std::endl;
			std::stringstream radioName;
			radioName << "fiber" << ifiber;
			std::stringstream swapCommand;
			swapCommand << "javascript:toggleBit('killFiber'," << ifiber << ");";
			if (currentKillFiber & 1<<ifiber) {
				*out << cgicc::input()
					.set("type","radio")
					.set("name",radioName.str())
					.set("id",radioName.str() + "live")
					.set("value","1")
					.set("onChange",swapCommand.str())
					.set("checked","checked") << cgicc::label("Live").set("for",radioName.str() + "live") << cgicc::br() << std::endl;
				*out << cgicc::input()
					.set("type","radio")
					.set("name",radioName.str())
					.set("id",radioName.str() + "killed")
					.set("value","0")
					.set("onChange",swapCommand.str()) << cgicc::label("Killed").set("for",radioName.str() + "killed") << std::endl;
			} else {
				*out << cgicc::input()
					.set("type","radio")
					.set("name",radioName.str())
					.set("id",radioName.str() + "live")
					.set("value","1")
					.set("onChange",swapCommand.str()) << cgicc::label("Live").set("for",radioName.str() + "live") << cgicc::br() << std::endl;
				*out << cgicc::input()
					.set("type","radio")
					.set("name",radioName.str())
					.set("id",radioName.str() + "killed")
					.set("value","0")
					.set("onChange",swapCommand.str())
					.set("checked","checked") << cgicc::label("Killed").set("for",radioName.str() + "killed") << std::endl;
			}
			*out << cgicc::td() << std::endl;
		}
		*out << cgicc::tr() << std::endl;
		*out << cgicc::table() << std::endl;

		// Now for some check-boxes.
		*out << cgicc::div()
			.set("style","font-size: 8pt;") << std::endl;
		if (currentKillFiber & (1<<15)) {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box15")
				.set("id","box15")
				.set("onChange","javascript:toggleBit('killFiber',15);")
				.set("checked","checked") << std::endl;
		} else {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box15")
				.set("id","box15")
				.set("onChange","javascript:toggleBit('killFiber',15);") << std::endl;
		}
		*out << cgicc::label()
			.set("for","box15")
			<< "Force all DDU checks" << cgicc::label() << cgicc::div() << std::endl;

		*out << cgicc::div()
			.set("style","font-size: 8pt;") << std::endl;
		if (currentKillFiber & (1<<16)) {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box16")
				.set("id","box16")
				.set("onChange","javascript:toggleBit('killFiber',16);")
				.set("checked","checked") << std::endl;
		} else {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box16")
				.set("id","box16")
				.set("onChange","javascript:toggleBit('killFiber',16);") << std::endl;
		}
		*out << cgicc::label()
			.set("for","box16")
			<< "Force ALCT checks" << cgicc::label() << cgicc::div() << std::endl;

		*out << cgicc::div()
			.set("style","font-size: 8pt;") << std::endl;
		if (currentKillFiber & (1<<17)) {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box17")
				.set("id","box17")
				.set("onChange","javascript:toggleBit('killFiber',17);")
				.set("checked","checked") << std::endl;
		} else {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box17")
				.set("id","box17")
				.set("onChange","javascript:toggleBit('killFiber',17);") << std::endl;
		}
		*out << cgicc::label()
			.set("for","box17")
			<< "Force TMB checks" << cgicc::label() << cgicc::div() << std::endl;

		*out << cgicc::div()
			.set("style","font-size: 8pt;") << std::endl;
		if (currentKillFiber & (1<<18)) {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box18")
				.set("id","box18")
				.set("onChange","javascript:toggleBit('killFiber',18);")
				.set("checked","checked") << std::endl;
		} else {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box18")
				.set("id","box18")
				.set("onChange","javascript:toggleBit('killFiber',18);") << std::endl;
		}
		*out << cgicc::label()
			.set("for","box18")
			<< "Force CFEB checks (enable DAV checks)" << cgicc::label() << cgicc::div() << std::endl;

		*out << cgicc::div()
			.set("style","font-size: 8pt;") << std::endl;
		if (currentKillFiber & (1<<19)) {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box19")
				.set("id","box19")
				.set("onChange","javascript:toggleBit('killFiber',19);")
				.set("checked","checked") << std::endl;
		} else {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box19")
				.set("id","box19")
				.set("onChange","javascript:toggleBit('killFiber',19);") << std::endl;
		}
		*out << cgicc::label()
			.set("for","box19")
			<< "Force normal DDU checks (off enables only SP/TF checks)" << cgicc::label() << cgicc::div() << std::endl;

		*out << cgicc::form() << std::endl;

		*out << cgicc::fieldset() << std::endl;



		// DDUFPGA Resets and other commands
		*out << cgicc::fieldset()
			.set("class","expert") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << "DDUFPGA Commands" << cgicc::div() << std::endl;

		*out << cgicc::form()
			.set("method","GET")
			.set("action",dduTextLoad) << std::endl;
		*out << cgicc::input()
			.set("name","slot")
			.set("type","hidden")
			.set("value",dduVal) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","crate")
			.set("value",crateVal) << std::endl;
		*out << cgicc::input()
			.set("name","command")
			.set("type","hidden")
			.set("value","5") << std::endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("value","Send one fake DDU L1A via VME") << std::endl;
		*out << cgicc::form() << std::endl;

		*out << cgicc::form()
			.set("method","GET")
			.set("action",dduTextLoad) << std::endl;
		*out << cgicc::input()
			.set("name","slot")
			.set("type","hidden")
			.set("value",dduVal) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","crate")
			.set("value",crateVal) << std::endl;
		*out << cgicc::input()
			.set("name","command")
			.set("type","hidden")
			.set("value","1") << std::endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("value","Toggle DDU CFEB-calibration-pulse==L1A feature (default false)") << std::endl;
		*out << cgicc::form() << std::endl;

		*out << cgicc::form()
			.set("method","GET")
			.set("action",dduTextLoad) << std::endl;
		*out << cgicc::input()
			.set("name","slot")
			.set("type","hidden")
			.set("value",dduVal) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","crate")
			.set("value",crateVal) << std::endl;
		*out << cgicc::input()
			.set("name","command")
			.set("type","hidden")
			.set("value","2") << std::endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("value","Reset DDU via VME sync reset") << std::endl;
		*out << cgicc::form() << std::endl;

		*out << cgicc::form()
			.set("method","GET")
			.set("action",dduTextLoad) << std::endl;
		*out << cgicc::input()
			.set("name","slot")
			.set("type","hidden")
			.set("value",dduVal) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","crate")
			.set("value",crateVal) << std::endl;
		*out << cgicc::input()
			.set("name","command")
			.set("type","hidden")
			.set("value","6") << std::endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("value","Send FMM-Error-Disable signal") << std::endl;
		*out << cgicc::form() << std::endl;

		*out << cgicc::fieldset() << std::endl;



		// InFPGA resets
		*out << cgicc::fieldset()
			.set("class","expert") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << "InFPGA Resets" << cgicc::div() << std::endl;

		*out << cgicc::form()
			.set("method","GET")
			.set("action",dduTextLoad) << std::endl;
		*out << cgicc::input()
			.set("name","slot")
			.set("type","hidden")
			.set("value",dduVal) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","crate")
			.set("value",crateVal) << std::endl;
		*out << cgicc::input()
			.set("name","command")
			.set("type","hidden")
			.set("value","7") << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","textdata")
			.set("size","10")
			.set("ENCTYPE","multipart/form-data")
			.set("value","") << std::endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("value","Reset InFPGA0") << std::endl;
		*out << cgicc::form() << std::endl;

		*out << cgicc::form()
			.set("method","GET")
			.set("action",dduTextLoad) << std::endl;
		*out << cgicc::input()
			.set("name","slot")
			.set("type","hidden")
			.set("value",dduVal) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","crate")
			.set("value",crateVal) << std::endl;
		*out << cgicc::input()
			.set("name","command")
			.set("type","hidden")
			.set("value","8") << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","textdata")
			.set("size","10")
			.set("ENCTYPE","multipart/form-data")
			.set("value","") << std::endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("value","Reset InFPGA1") << std::endl;
		*out << cgicc::form() << std::endl;

		*out << cgicc::fieldset() << std::endl;	


		
		// Inreg for serial writes.
		*out << cgicc::fieldset()
			.set("class","expert") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << "Input Registers for Flash Writes" << cgicc::div() << std::endl;

		DataTable inregTable;
		inregTable->set("id", "inregTable").set("class", "data");
		
		// Names have to come first, because there is only one of each.
		inregTable(0,0) << "Register";
		inregTable(0,1) << "Value";
		inregTable(0,2) << "New Value";
		
		for (unsigned int iReg = 0; iReg < 3; iReg++) {
			unsigned int inreg = myDDU->readInputRegister(iReg);
			inregTable(iReg + 1,0) << "InReg" << iReg;
			inregTable(iReg + 1,1) << std::showbase << std::hex << inreg;
		}
		
		inregTable(1,2) << cgicc::form()
			.set("method", "GET")
			.set("action", dduTextLoad) << std::endl;
		inregTable(1,2) << cgicc::input()
			.set("name", "slot")
			.set("type", "hidden")
			.set("value", dduVal) << std::endl;
		inregTable(1,2) << cgicc::input()
			.set("type", "hidden")
			.set("name", "crate")
			.set("value", crateVal) << std::endl;
		inregTable(1,2) << cgicc::input()
			.set("name","command")
			.set("type","hidden")
			.set("value","9") << std::endl;
		inregTable(1,2) << cgicc::input()
			.set("name","textdata")
			.set("size","10")
			.set("ENCTYPE","multipart/form-data")
			.set("value",inregTable(1,1).str()) << std::endl;
		inregTable(1,2) << cgicc::input()
			.set("type","submit")
			.set("value","Set Value") << std::endl;
		inregTable(1,2) << cgicc::form() << std::endl;

		*out << inregTable.toHTML() << std::endl;

		*out << cgicc::div()
			.set("style","font-size: 8pt;") << std::endl;
		*out << "Input registers are used for serial writing.  You can load up to 48 bits in these three registers with bit 0 of InReg0 being the LSB." << cgicc::br() << std::endl;
		*out << "The registers are pipelined, InReg0 --> InReg1 --> InReg2." << std::endl;
		*out << cgicc::div() << std::endl;

		*out << cgicc::fieldset() << std::endl;

		// Clever trick to help with formating the cut-and-paste.
		*out << cgicc::br()
			.set("style","display: none") << std::endl;


		// Other Serial registers that nobody but experts care about
		//myCrate->getVMEController()->CAEN_err_reset();
		*out << cgicc::fieldset()
			.set("class","expert") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << "Miscellaneous Serial Registers" << cgicc::div() << std::endl;

		DataTable expertTable;
		expertTable->set("id", "expertTable").set("class", "data");
		
		// Names have to come first, because there is only one of each.
		expertTable(0,0) << "Register";
		expertTable(0,1) << "Value";
		expertTable(0,2) << "Decoded Register Info";
		expertTable(0,3) << "New Value";

		// FIXME
		unsigned int gbePrescale = myDDU->readGbEPrescale();
		expertTable(1,0) << "GbE prescale";
		expertTable(1,1) << std::showbase << std::hex << gbePrescale;
		/*std::map<std::string,std::string> gbePrescaleComments = DDUDebugger::GbEPrescale(gbePrescale);
		for (std::map<std::string,std::string>::iterator iComment = gbePrescaleComments.begin();
			iComment != gbePrescaleComments.end();
			iComment++) {
			expertTable(0,2) << cgicc::div(iComment->first)
				.set("class",iComment->second);
		}
		*/
		expertTable(1,3) << cgicc::form()
			.set("method", "GET")
			.set("action", dduTextLoad) << std::endl;
		expertTable(1,3) << cgicc::input()
			.set("name", "slot")
			.set("type", "hidden")
			.set("value", dduVal) << std::endl;
		expertTable(1,3) << cgicc::input()
			.set("type", "hidden")
			.set("name", "crate")
			.set("value", crateVal) << std::endl;
		expertTable(1,3) << cgicc::input()
			.set("name","command")
			.set("type","hidden")
			.set("value","11") << std::endl;
		expertTable(1,3) << std::hex << cgicc::input()
			.set("name","textdata")
			.set("size","10")
			.set("ENCTYPE","multipart/form-data")
			.set("value",expertTable(1,1).str()) << std::endl;
		expertTable(1,3) << cgicc::input()
			.set("type","submit")
			.set("value","Set Value") << std::endl;
		expertTable(1,3) << cgicc::form() << std::endl;
		
		
		unsigned int fakeL1 = myDDU->readFakeL1();
		expertTable(2,0) << "Fake L1A Data Pass-through";
		expertTable(2,1) << std::showbase << std::hex << fakeL1;
		/*
		std::map<std::string,std::string> fakeL1Comments = DDUDebugger::FakeL1Reg(fakeL1);
		for (std::map<std::string,std::string>::iterator iComment = fakeL1Comments.begin();
			iComment != fakeL1Comments.end();
			iComment++) {
			expertTable(1,2) << cgicc::div(iComment->first)
				.set("class",iComment->second);
		}
		*/
		expertTable(2,3) << cgicc::form()
			.set("method", "GET")
			.set("action", dduTextLoad) << std::endl;
		expertTable(2,3) << cgicc::input()
			.set("name", "slot")
			.set("type", "hidden")
			.set("value", dduVal) << std::endl;
		expertTable(2,3) << cgicc::input()
			.set("type", "hidden")
			.set("name", "crate")
			.set("value", crateVal) << std::endl;
		expertTable(2,3) << cgicc::input()
			.set("name","command")
			.set("type","hidden")
			.set("value","13") << std::endl;
		expertTable(2,3) << std::hex << cgicc::input()
			.set("name","textdata")
			.set("size","10")
			.set("ENCTYPE","multipart/form-data")
			.set("value",expertTable(2,1).str()) << std::endl;
		expertTable(2,3) << cgicc::input()
			.set("type","submit")
			.set("value","Set Value") << std::endl;
		expertTable(2,3) << cgicc::form() << std::endl;
		

		unsigned int foe = myDDU->readFMM();
		expertTable(3,0) << "F0E + 4-bit FMM";
		expertTable(3,1) << std::showbase << std::hex << foe;
		/*
		std::map<std::string,std::string> foeComments = DDUDebugger::F0EReg(foe);
		for (std::map<std::string,std::string>::iterator iComment = foeComments.begin();
			iComment != foeComments.end();
			iComment++) {
			expertTable(2,2) << cgicc::div(iComment->first)
				.set("class",iComment->second);
		}
		*/
		expertTable(3,3) << cgicc::form()
			.set("method", "GET")
			.set("action", dduTextLoad) << std::endl;
		expertTable(3,3) << cgicc::input()
			.set("name", "slot")
			.set("type", "hidden")
			.set("value", dduVal) << std::endl;
		expertTable(3,3) << cgicc::input()
			.set("type", "hidden")
			.set("name", "crate")
			.set("value", crateVal) << std::endl;
		expertTable(3,3) << cgicc::input()
			.set("name","command")
			.set("type","hidden")
			.set("value","18") << std::endl;
		expertTable(3,3) << std::hex << cgicc::input()
			.set("name","textdata")
			.set("size","10")
			.set("ENCTYPE","multipart/form-data")
			.set("value",expertTable(3,1).str()) << std::endl;
		expertTable(3,3) << cgicc::input()
			.set("type","submit")
			.set("value","Set Value") << std::endl;
		expertTable(3,3) << cgicc::form() << std::endl;
		
		
		expertTable(4,0) << "Switches";
		expertTable(4,1) << std::showbase << std::hex << (myDDU->readSwitches() & 0xff);

		for (unsigned int iReg = 0; iReg < 5; iReg++) {
			expertTable(5 + iReg,0) << "Test register " << iReg;
			expertTable(5 + iReg,1) << std::showbase << std::hex << (myDDU->readTestRegister(iReg));
		}

		*out << expertTable.toHTML() << std::endl;

		
		// You can toggle some more things down here.
		*out << cgicc::form()
			.set("method","GET")
			.set("action",dduTextLoad) << std::endl;
		*out << cgicc::input()
			.set("name","slot")
			.set("type","hidden")
			.set("value",dduVal) << std::endl;
		*out << cgicc::input()
			.set("name","crate")
			.set("type","hidden")
			.set("value",crateVal) << std::endl;
		*out << cgicc::input()
			.set("name","command")
			.set("type","hidden")
			.set("value","10") << std::endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("value","Toggle DAQ backpressure (DCC/S-Link wait)") << std::endl;
		*out << cgicc::form() << std::endl;

		*out << cgicc::form()
			.set("method","GET")
			.set("action",dduTextLoad) << std::endl;
		*out << cgicc::input()
			.set("name","slot")
			.set("type","hidden")
			.set("value",dduVal) << std::endl;
		*out << cgicc::input()
			.set("name","crate")
			.set("type","hidden")
			.set("value",crateVal) << std::endl;
		*out << cgicc::input()
			.set("name","command")
			.set("type","hidden")
			.set("value","12") << std::endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("value","Toggle all fake L1A (data dump/pass-through)") << std::endl;
		*out << cgicc::form() << std::endl;

		*out << cgicc::form()
			.set("method","GET")
			.set("action",dduTextLoad) << std::endl;
		*out << cgicc::input()
			.set("name","slot")
			.set("type","hidden")
			.set("value",dduVal) << std::endl;
		*out << cgicc::input()
			.set("name","crate")
			.set("type","hidden")
			.set("value",crateVal) << std::endl;
		*out << cgicc::input()
			.set("name","command")
			.set("type","hidden")
			.set("value","6") << std::endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("value","Activate FMM error-report disable setting") << std::endl;
		*out << cgicc::form() << std::endl;

		*out << cgicc::div()
			.set("style","font-size: 8pt;") << std::endl;
		*out << "GbE Prescale bits 0-2 set rate (0 => 1:1, 1 => 1:2, 2=> 1:4 ... 7 => never)" << cgicc::br() << std::endl;
		*out << "GbE Prescale bit 3=1 sets DCC/S-Link wait ignore" << cgicc::br() << std::endl;
		*out << "Fake L1A bit 0=1 sets InFPGA0 pass-through" << cgicc::br() << std::endl;
		*out << "Fake L1A bit 1=1 sets InFPGA1 pass-through" << cgicc::br() << std::endl;
		*out << "Fake L1A bit 2=1 sets DDUFPGA pass-through" << cgicc::br() << std::endl;
		*out << "F0E register bits 4-15=0xFED0 disables DDU FMM error reporting" << std::endl;
		*out << cgicc::div() << std::endl;

		*out << cgicc::fieldset() << std::endl;


		// Parallel/flash writable registers
		*out << cgicc::fieldset()
			.set("class","expert") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << "Writable Flash Pages" << cgicc::div() << std::endl;
		
		DataTable writableTable;
		writableTable->set("id", "writableTable").set("class", "data");
		
		// Names have to come first, because there is only one of each.
		writableTable(0,0) << "Page";
		writableTable(0,1) << "Value";
		writableTable(0,2) << "New Value";


		writableTable(1,0) << "Flash KillFiber";
		writableTable(1,1) << std::showbase << std::hex << myDDU->readFlashKillFiber();

		writableTable(1,2) << cgicc::form()
			.set("method", "GET")
			.set("action", dduTextLoad) << std::endl;
		writableTable(1,2) << cgicc::input()
			.set("name", "slot")
			.set("type", "hidden")
			.set("value", dduVal) << std::endl;
		writableTable(1,2) << cgicc::input()
			.set("type", "hidden")
			.set("name", "crate")
			.set("value", crateVal) << std::endl;
		writableTable(1,2) << cgicc::input()
			.set("name","command")
			.set("type","hidden")
			.set("value","14") << std::endl;
		writableTable(1,2) << std::hex << cgicc::input()
			.set("name","textdata")
			.set("size","10")
			.set("ENCTYPE","multipart/form-data")
			.set("value",writableTable(1,1).str()) << std::endl;
		writableTable(1,2) << cgicc::input()
			.set("type","submit")
			.set("value","Set Value") << std::endl;
		writableTable(1,2) << cgicc::form() << std::endl;
		

		std::vector<uint16_t> lcode = myDDU->readFlashGbEFIFOThresholds();
		writableTable(1,0) << "Flash GbE FIFO thresholds";
		//writableTable(1,1) << "0x" << std::hex << ((lcode[4]&0xC0)>>6) << std::noshowbase << std::setw(8) << std::setfill('0') << std::hex << (((((lcode[2]&0xC0)>>6)|((lcode[5]&0xFF)<<2)|((lcode[4]&0x3F)<<10)) << 16) | (((lcode[0]&0xC0)>>6)|((lcode[3]&0xFF)<<2)|((lcode[2]&0x3F)<<10)));
		writableTable(2,1) << "0x" << std::hex << std::setw(1) << std::setfill('0') << lcode[2] << std::setw(4) << lcode[1] << std::setw(4) << lcode[0];
		
		writableTable(2,2) << cgicc::form()
			.set("method", "GET")
			.set("action", dduTextLoad) << std::endl;
		writableTable(2,2) << cgicc::input()
			.set("name", "slot")
			.set("type", "hidden")
			.set("value", dduVal) << std::endl;
		writableTable(2,2) << cgicc::input()
			.set("type", "hidden")
			.set("name", "crate")
			.set("value", crateVal) << std::endl;
		writableTable(2,2) << cgicc::input()
			.set("name","command")
			.set("type","hidden")
			.set("value","15") << std::endl;
		writableTable(2,2) << std::hex << cgicc::input()
			.set("name","textdata")
			.set("size","10")
			.set("ENCTYPE","multipart/form-data")
			.set("value",writableTable(2,1).str()) << std::endl;
		writableTable(2,2) << cgicc::input()
			.set("type","submit")
			.set("value","Set Value") << std::endl;
		writableTable(2,2) << cgicc::form() << std::endl;

		
		writableTable(3,0) << "Flash Board ID";
		writableTable(3,1) << myDDU->readFlashBoardID();
		
		writableTable(3,2) << cgicc::form()
			.set("method", "GET")
			.set("action", dduTextLoad) << std::endl;
		writableTable(3,2) << cgicc::input()
			.set("name", "slot")
			.set("type", "hidden")
			.set("value", dduVal) << std::endl;
		writableTable(3,2) << cgicc::input()
			.set("type", "hidden")
			.set("name", "crate")
			.set("value", crateVal) << std::endl;
		writableTable(3,2) << cgicc::input()
			.set("name","command")
			.set("type","hidden")
			.set("value","16") << std::endl;
		writableTable(3,2) << std::hex << cgicc::input()
			.set("name","textdata")
			.set("size","10")
			.set("ENCTYPE","multipart/form-data")
			.set("value",writableTable(3,1).str()) << std::endl;
		writableTable(3,2) << cgicc::input()
			.set("type","submit")
			.set("value","Set Value") << std::endl;
		writableTable(3,2) << cgicc::form() << std::endl;
		

		writableTable(4,0) << "Flash DDU RUI";
		writableTable(4,1) << myDDU->readFlashRUI();

		// New Value...
		writableTable(4,2) << cgicc::form()
			.set("method", "GET")
			.set("action", dduTextLoad) << std::endl;
		writableTable(4,2) << cgicc::input()
			.set("name", "slot")
			.set("type", "hidden")
			.set("value", dduVal) << std::endl;
		writableTable(4,2) << cgicc::input()
			.set("type", "hidden")
			.set("name", "crate")
			.set("value", crateVal) << std::endl;
		writableTable(4,2) << cgicc::input()
			.set("name","command")
			.set("type","hidden")
			.set("value","17") << std::endl;
		writableTable(4,2) << std::hex << cgicc::input()
			.set("name","textdata")
			.set("size","10")
			.set("ENCTYPE","multipart/form-data")
			.set("value",writableTable(4,1).str()) << std::endl;
		writableTable(4,2) << cgicc::input()
			.set("type","submit")
			.set("value","Set Value") << std::endl;
		writableTable(4,2) << cgicc::form() << std::endl;

		
		*out << writableTable.toHTML() << std::endl;

		*out << cgicc::fieldset() << std::endl;	

		
		*out << Footer() << std::endl;
		
	} catch (xcept::Exception &e) {
		std::ostringstream error;
		error << "Exception caught!";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
		
		std::vector<xcept::ExceptionInformation> history = e2.getHistory();
		for (std::vector<xcept::ExceptionInformation>::iterator iError = history.begin(); iError != history.end(); iError++) {
			DataTable exceptionTable;
			unsigned int iRow = 0;
			std::map<std::string, std::string> messages = iError->getProperties();
			for (std::map<std::string, std::string>::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++) {
				exceptionTable(iRow, 0) << iMessage->first << ":";
				exceptionTable(iRow, 1) << iMessage->second;
				iRow++;
			}
			*out << cgicc::div(exceptionTable.toHTML())
				.set("class", "exception");
		}
	}
}




void emu::fed::EmuFCrateHyperDAQ::VMEPARA(xgi::Input *in, xgi::Output *out)
{

	try {
		
		// PGK Patented check-for-initialization
		if (crateVector_.size()==0) {
			LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
			return Default(in,out);
		}

		cgicc::Cgicc cgi(in);
		
		// First, I need a crate.
		std::pair<unsigned int, Crate *> cratePair = getCGICrate(cgi);
		unsigned int cgiCrate = cratePair.first;
		Crate *myCrate = cratePair.second;

		std::pair<unsigned int, DDU *> boardPair = getCGIBoard<DDU>(cgi);
		unsigned int cgiDDU = boardPair.first;
		DDU *myDDU = boardPair.second;

		std::stringstream sTitle;
		sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DDU VME Parallel Controls (RUI #" << myCrate->getRUI(myDDU->slot()) << ")";
		std::vector<std::string> jsFileNames;
		jsFileNames.push_back("tableToggler.js");
		jsFileNames.push_back("errorFlasher.js");
		*out << Header(sTitle.str(),jsFileNames);

		// PGK Select-a-Crate/Slot
		*out << cgicc::fieldset()
			.set("class","fieldset") << std::endl;
		*out << cgicc::div("View this page for a different crate/board")
			.set("class","legend") << std::endl;

		*out << selectACrate("VMEPARA","ddu",cgiDDU,cgiCrate) << std::endl;

		*out << cgicc::span("Configuration located at " + xmlFile_.toString())
			.set("style","color: #A00; font-size: 10pt;") << std::endl;

		*out << cgicc::fieldset() << std::endl;
		*out << cgicc::br() << std::endl;

		//myCrate->getVMEController()->CAEN_err_reset();

		// Display VME Control status information
		*out << cgicc::fieldset()
			.set("class","normal") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << cgicc::a("&plusmn;")
			.set("class","openclose")
			.set("onClick","javascript:toggle('statusTable')") << std::endl;
		*out << "VME Control Status" << cgicc::div() << std::endl;

		DataTable statusTable;
		statusTable->set("id", "statusTable").set("class", "data");
		
		statusTable(0,0) << "Register";
		statusTable(0,1) << "Value";
		statusTable(0,2) << "Decoded Information";
		
		unsigned int parallelStat = myDDU->readParallelStatus();
		statusTable(1,0) << "VME status register";
		statusTable(1,1) << std::showbase << std::hex << parallelStat;
		std::map<std::string,std::string> parallelStatComments = DDUDebugger::ParallelStat(parallelStat);
		for (std::map<std::string,std::string>::iterator iComment = parallelStatComments.begin();
			iComment != parallelStatComments.end();
			iComment++) {
			statusTable(1,2) << cgicc::div(iComment->first)
				.set("class",iComment->second);
		}
		statusTable(1,1).setClass("ok");
		if ((parallelStat >> 8) & 0xF == 0x4) statusTable(1,1).setClass("warning");
		else if ((parallelStat >> 8) & 0xF == 0x1) statusTable(1,1).setClass("questionable");
		else if ((parallelStat >> 8) & 0xF != 0x8) statusTable(1,1).setClass("bad");

		int dduFMM = (parallelStat >> 8) & 0xf;
		statusTable(2,0) << "DDU FMM status";
		statusTable(2,1) << std::showbase << std::hex << dduFMM;
		std::map<std::string,std::string> dduFMMComments = DDUDebugger::FMM(dduFMM);
		for (std::map<std::string,std::string>::iterator iComment = dduFMMComments.begin();
			iComment != dduFMMComments.end();
			iComment++) {
			statusTable(2,2) << cgicc::div(iComment->first)
				.set("class",iComment->second);
		}
		statusTable(2,1).setClass("ok");
		if (dduFMM & 0xF == 0x4) statusTable(2,1).setClass("warning");
		else if (dduFMM & 0xF == 0x1) statusTable(2,1).setClass("questionable");
		else if (dduFMM & 0xF != 0x8) statusTable(2,1).setClass("bad");

		*out << statusTable.printSummary() << std::endl;

		*out << statusTable.toHTML() << std::endl;

		*out << cgicc::fieldset() << std::endl;

		// Clever trick to help with formating the cut-and-paste.
		*out << cgicc::br()
			.set("style","display: none") << std::endl;

		//myCrate->getVMEController()->CAEN_err_reset();
		// Display individual fiber information
		*out << cgicc::fieldset()
			.set("class","normal") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << cgicc::a("&plusmn;")
			.set("class","openclose")
			.set("onClick","javascript:toggle('fmmTable')") << std::endl;
		*out << "Individual CSC FMM Reports" << cgicc::div() << std::endl;

		DataTable fmmTable;
		fmmTable->set("id", "fmmTable").set("class", "data");
		
		fmmTable(0,0) << "Register";
		fmmTable(0,1) << "Value";
		fmmTable(0,2) << "Decoded Chambers";
		
		unsigned int cscStat = myDDU->readCSCStatus() | myDDU->readAdvancedFiberErrors();
		fmmTable(1,0) << "FMM problem report";
		fmmTable(1,1) << std::showbase << std::hex << cscStat;
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (cscStat & (1<<iFiber)) {
				fmmTable(1,2) << cgicc::div(myDDU->getChamber(iFiber)->name())
					.set("class","red");
			}
		}
		fmmTable(1,1).setClass("ok");
		if (cscStat) fmmTable(1,1).setClass("bad");

		unsigned int cscBusy = myDDU->readFMMBusy();
		fmmTable(2,0) << "Busy";
		fmmTable(2,1) << std::showbase << std::hex << cscBusy;
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (cscBusy & (1<<iFiber)) {
				fmmTable(2,2) << cgicc::div(myDDU->getChamber(iFiber)->name());
			}
		}
		fmmTable(2,1).setClass("ok");

		unsigned int cscWarn = myDDU->readFMMFullWarning();
		fmmTable(3,0) << "Warning/near full";
		fmmTable(3,1) << std::showbase << std::hex << cscWarn;
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (cscWarn & (1<<iFiber)) {
				fmmTable(3,2) << cgicc::div(myDDU->getChamber(iFiber)->name())
					.set("class","orange");
			}
		}
		fmmTable(3,1).setClass("ok");
		if (cscWarn) fmmTable(3,1).setClass("warning");

		unsigned int cscLS = myDDU->readFMMLostSync();
		fmmTable(4,0) << "Lost sync";
		fmmTable(4,1) << std::showbase << std::hex << cscLS;
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (cscLS & (1<<iFiber)) {
				fmmTable(4,2) << cgicc::div(myDDU->getChamber(iFiber)->name())
					.set("class","red");
			}
		}
		fmmTable(4,1).setClass("ok");
		if (cscLS) fmmTable(4,1).setClass("bad");

		unsigned int cscError = myDDU->readFMMError();
		fmmTable(5,0) << "Error";
		fmmTable(5,1) << std::showbase << std::hex << cscError;
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (cscError & (1<<iFiber)) {
				fmmTable(5,2) << cgicc::div(myDDU->getChamber(iFiber)->name())
					.set("class","red");
			}
		}
		fmmTable(5,1).setClass("ok");
		if (cscError) fmmTable(5,1).setClass("bad");

		unsigned int cscWH = myDDU->readWarningHistory();
		fmmTable(6,0) << "Warning history";
		fmmTable(6,1) << std::showbase << std::hex << cscWH;
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (cscWH & (1<<iFiber)) {
				fmmTable(6,2) << cgicc::div(myDDU->getChamber(iFiber)->name())
					.set("class","orange");
			}
		}

		unsigned int cscBH = myDDU->readBusyHistory();
		fmmTable(7,0) << "Busy history";
		fmmTable(7,1) << std::showbase << std::hex << cscBH;
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (cscBH & (1<<iFiber)) {
				fmmTable(7,2) << cgicc::div(myDDU->getChamber(iFiber)->name());
			}
		}

		*out << fmmTable.printSummary() << std::endl;

		*out << fmmTable.toHTML() << std::endl;

		*out << cgicc::fieldset() << std::endl;

		*out << Footer() << std::endl;
		
	} catch (xcept::Exception &e) {
		std::ostringstream error;
		error << "Exception caught!";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
		
		std::vector<xcept::ExceptionInformation> history = e2.getHistory();
		for (std::vector<xcept::ExceptionInformation>::iterator iError = history.begin(); iError != history.end(); iError++) {
			DataTable exceptionTable;
			unsigned int iRow = 0;
			std::map<std::string, std::string> messages = iError->getProperties();
			for (std::map<std::string, std::string>::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++) {
				exceptionTable(iRow, 0) << iMessage->first << ":";
				exceptionTable(iRow, 1) << iMessage->second;
				iRow++;
			}
			*out << cgicc::div(exceptionTable.toHTML())
				.set("class", "exception");
		}
	}
	
}



void emu::fed::EmuFCrateHyperDAQ::VMESERI(xgi::Input *in, xgi::Output *out)
{
	
	try {

		// PGK Patented check-for-initialization
		if (crateVector_.size()==0) {
			LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
			return Default(in,out);
		}

		cgicc::Cgicc cgi(in);
		
		// First, I need a crate.
		std::pair<unsigned int, Crate *> cratePair = getCGICrate(cgi);
		unsigned int cgiCrate = cratePair.first;
		Crate *myCrate = cratePair.second;

		std::pair<unsigned int, DDU *> boardPair = getCGIBoard<DDU>(cgi);
		unsigned int cgiDDU = boardPair.first;
		DDU *myDDU = boardPair.second;

		std::stringstream sTitle;
		sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DDU VME Serial Controls (RUI #" << myCrate->getRUI(myDDU->slot()) << ")";
		std::vector<std::string> jsFileNames;
		jsFileNames.push_back("tableToggler.js");
		jsFileNames.push_back("errorFlasher.js");
		*out << Header(sTitle.str(),jsFileNames);

		// PGK Select-a-Crate/Slot
		*out << cgicc::fieldset()
			.set("class","fieldset") << std::endl;
		*out << cgicc::div("View this page for a different crate/board")
			.set("class","legend") << std::endl;

		*out << selectACrate("VMESERI","ddu",cgiDDU,cgiCrate) << std::endl;

		*out << cgicc::span("Configuration located at " + xmlFile_.toString())
			.set("style","color: #A00; font-size: 10pt;") << std::endl;

		*out << cgicc::fieldset() << std::endl;
		*out << cgicc::br() << std::endl;
		
		//myCrate->getVMEController()->CAEN_err_reset();

		// No debugger needed here.

		// Display Voltages
		*out << cgicc::fieldset()
			.set("class","normal") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << cgicc::a("&plusmn;")
			.set("class","openclose")
			.set("onClick","javascript:toggle('voltTable')") << std::endl;
		*out << "Voltages" << cgicc::div() << std::endl;

		DataTable voltTable;
		voltTable->set("id", "voltTable").set("class", "data");
		
		voltTable(0,0) << "Voltage";
		voltTable(0,1) << "Value";
		
		float V15 = myDDU->readVoltage(0);
		voltTable(1,0) << "Voltage V15";
		voltTable(1,1) << std::setprecision(4) << V15 << " mV";
		voltTable(1,1).setClass("ok");
		if (V15 > 1500*1.025 || V15 < 1500*0.975) voltTable(1,1).setClass("warning");
		if (V15 > 1500*1.05 || V15 < 1500*0.95) voltTable(1,1).setClass("bad");
		if (V15 > 3500 || V15 < 0) voltTable(1,1).setClass("questionable");

		float V25 = myDDU->readVoltage(1);
		voltTable(2,0) << "Voltage V25";
		voltTable(2,1) << std::setprecision(4) << V25 << " mV";
		voltTable(2,1).setClass("ok");
		if (V25 > 2500*1.025 || V25 < 2500*0.975) voltTable(2,1).setClass("warning");
		if (V25 > 2500*1.05 || V25 < 2500*0.95) voltTable(2,1).setClass("bad");
		if (V25 > 3500 || V25 < 0) voltTable(2,1).setClass("questionable");
		
		float V25a = myDDU->readVoltage(2);
		voltTable(3,0) << "Voltage V25A";
		voltTable(3,1) << std::setprecision(4) << V25a << " mV";
		voltTable(3,1).setClass("ok");
		if (V25a > 2500*1.025 || V25a < 2500*0.975) voltTable(3,1).setClass("warning");
		if (V25a > 2500*1.05 || V25a < 2500*0.95) voltTable(3,1).setClass("bad");
		if (V25a > 3500 || V25a < 0) voltTable(3,1).setClass("questionable");
		
		float V33 = myDDU->readVoltage(3);
		voltTable(4,0) << "Voltage V33";
		voltTable(4,1) << std::setprecision(4) << V33 << " mV";
		voltTable(4,1).setClass("ok");
		if (V33 > 3300*1.025 || V33 < 3300*0.975) voltTable(4,1).setClass("warning");
		if (V33 > 3300*1.05 || V33 < 3300*0.95) voltTable(4,1).setClass("bad");
		if (V33 > 3500 || V33 < 0) voltTable(4,1).setClass("questionable");

		// Print the table summary
		*out << voltTable.printSummary() << std::endl;

		*out << voltTable.toHTML() << std::endl;

		*out << cgicc::fieldset() << std::endl;

		// Clever trick to help with formating the cut-and-paste.
		*out << cgicc::br()
			.set("style","display: none") << std::endl;

		// Display Temperatures
		*out << cgicc::fieldset()
			.set("class","normal") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << cgicc::a("&plusmn;")
			.set("class","openclose")
			.set("onClick","javascript:toggle('tempTable')") << std::endl;
		*out << "Temperatures" << cgicc::div() << std::endl;

		DataTable tempTable;
		tempTable->set("id", "tempTable").set("class", "data");
		
		tempTable(0,0) << "Temperature";
		tempTable(0,1) << "Value";
		
		for (unsigned int iTemp = 0; iTemp < 4; iTemp++) {
			float T0 = myDDU->readTemperature(iTemp);
			tempTable(iTemp + 1,0) << "Temperature " << iTemp;
			tempTable(iTemp + 1,1) << std::setprecision(4) << T0 << "&deg;F";
			tempTable(iTemp + 1,1).setClass("ok");
			if (T0 > 80*1.2 || T0 < 80*0.8) tempTable(iTemp + 1,1).setClass("warning");
			if (T0 > 80*1.4 || T0 < 80*0.6) tempTable(iTemp + 1,1).setClass("bad");
			if (T0 > 170 || T0 < 0) tempTable(iTemp + 1,1).setClass("questionable");
		}

		// Print the table summary
		*out << tempTable.printSummary() << std::endl;

		*out << tempTable.toHTML() << std::endl;

		*out << cgicc::fieldset() << std::endl;

		// Clever trick to help with formating the cut-and-paste.
		*out << cgicc::br()
			.set("style","display: none") << std::endl;

		// Display Serial Flash RAM Status
		*out << cgicc::fieldset()
			.set("class","normal") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << cgicc::a("&plusmn;")
			.set("class","openclose")
			.set("onClick","javascript:toggle('ramStatusTable')") << std::endl;
		*out << "Serial Flash RAM Status" << cgicc::div() << std::endl;

		DataTable ramStatusTable;
		ramStatusTable->set("id", "ramStatusTable").set("class", "data");
		
		ramStatusTable(0,0) << "Register";
		ramStatusTable(0,1) << "Value";
		

		int ramStatus = myDDU->readSerialStatus();
		ramStatusTable(1,0) << "Serial Flash RAM Status";
		ramStatusTable(1,1) << std::showbase << std::hex << ramStatus;
		ramStatusTable(1,1).setClass("ok");
		if (ramStatus & 0x003c != 0x000c) ramStatusTable(1,1).setClass("warning");
		if (ramStatus & 0x0080 != 0x0080) ramStatusTable(1,1).setClass("bad");

		// Print the table summary
		*out << ramStatusTable.printSummary() << std::endl;

		*out << ramStatusTable.toHTML() << std::endl;

		*out << cgicc::fieldset() << std::endl;

		*out << Footer() << std::endl;

	} catch (xcept::Exception &e) {
		std::ostringstream error;
		error << "Exception caught!";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
		
		std::vector<xcept::ExceptionInformation> history = e2.getHistory();
		for (std::vector<xcept::ExceptionInformation>::iterator iError = history.begin(); iError != history.end(); iError++) {
			DataTable exceptionTable;
			unsigned int iRow = 0;
			std::map<std::string, std::string> messages = iError->getProperties();
			for (std::map<std::string, std::string>::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++) {
				exceptionTable(iRow, 0) << iMessage->first << ":";
				exceptionTable(iRow, 1) << iMessage->second;
				iRow++;
			}
			*out << cgicc::div(exceptionTable.toHTML())
				.set("class", "exception");
		}
	}
}


void emu::fed::EmuFCrateHyperDAQ::DDUTextLoad(xgi::Input *in, xgi::Output *out)
{
	try {

		// PGK Patented check-for-initialization
		if (crateVector_.size()==0) {
			LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
			return Default(in,out);
		}

		cgicc::Cgicc cgi(in);

		// First, I need a crate.
		std::pair<unsigned int, Crate *> cratePair = getCGICrate(cgi);
		unsigned int cgiCrate = cratePair.first;
		//Crate *myCrate = cratePair.second;

		std::pair<unsigned int, DDU *> boardPair = getCGIBoard<DDU>(cgi);
		unsigned int cgiDDU = boardPair.first;
		DDU *myDDU = boardPair.second;

		// The command that is intended to be sent to the DDU.
		unsigned int command = 0;
		cgicc::form_iterator name = cgi.getElement("command");
		if(name != cgi.getElements().end()) {
			command = cgi["command"]->getIntegerValue();
		}

		// Now load the data to be loaded, if applicable.
		std::string XMLText;
		name = cgi.getElement("textdata");
		if(name != cgi.getElements().end()) {
			XMLText = cgi["textdata"]->getValue();
		}

		// The decimal number (everything is loaded with decimal)
		unsigned long int uploadValue = 0;
		std::stringstream uploadStream;
		// If we have a hex number, 0x will prefix it.
		if (XMLText.substr(0,2) == "0x") {
			XMLText = XMLText.substr(2);
			// If we are uploading GbEFIFOThresholds, make sure we skip the first char...
			if (command == 15) XMLText = XMLText.substr(1);
			uploadStream << XMLText;
			uploadStream >> std::hex >> uploadValue;
		} else {
			uploadStream << XMLText;
			uploadStream >> std::dec >> uploadValue;
		}

		unsigned long int otherValue = 0;
		std::stringstream otherStream;
		std::string otherText;
		name = cgi.getElement("textdata");
		if(name != cgi.getElements().end()) {
			otherText = cgi["textdata"]->getValue();
		}
		// If we have a hex number, 0x will prefix it.
		if (otherText.substr(0,2) == "0x") {
			otherText = otherText.substr(2);
			otherText = otherText.substr(0,1);
			otherStream << otherText;
			otherStream >> std::hex >> otherValue;
		} else {
			otherStream << otherText;
			otherStream >> std::dec >> otherValue;
		}
		
		// check for errors.
		if ((uploadStream.fail() || !uploadStream.eof()) && XMLText != "") {
			LOG4CPLUS_ERROR(getApplicationLogger(), "DDUTextLoad does not understand XMLText(" << XMLText << ")");
			std::stringstream location;
			location << "crate=" << cgiCrate << "&slot=" << cgiDDU;
			return webRedirect(out,"DDUExpert?"+location.str());
		}


		LOG4CPLUS_DEBUG(getApplicationLogger(),"Attempting DDUTextLoad with crate(" << cgiCrate << ") ddu(" << cgiDDU << ") == slot(" << myDDU->slot() << ") command(" << command << ") XMLText(" << XMLText << ") == uploadValue(" << uploadValue << ") otherText(" << otherText << ") == otherValue(" << otherValue << ")");

		//unsigned short int para_val;
		//unsigned long int send_val;

		// Switches don't like to have variables declared in the statement.
		unsigned int scale;
		std::vector<uint16_t> writeMe;
		writeMe.reserve(3);
		unsigned int fmm;
		
		// Switch on the command.
		switch (command) {

		case (1): // Toggle L1 calibration signal
			myDDU->toggleL1Calibration();
			break;
			
		case (2): // Reset the DDUFPGA
			myDDU->resetFPGA(DDUFPGA);
			break;
			
		case (3): // Load KillFiber
			myDDU->writeKillFiber(uploadValue);
			break;

		case (4): // Load BXorbit
			myDDU->writeBXOrbit(uploadValue);
			break;

		case (5): // Send fake L1A
			myDDU->sendFakeL1A();
			break;

		case (6): // Load STFU register
			fmm = myDDU->readFMM();
			myDDU->writeFMM( 0XFED0 | fmm );
			break;
			
		case (7): // InFPGA reset
			myDDU->resetFPGA(INFPGA0);
			break;

		case (8): // Another InFPGA reset.  Not needed?
			myDDU->resetFPGA(INFPGA1);
			break;
			
		case (9): // Write the inreg
			myDDU->writeInputRegister(uploadValue);
			break;

		case (10): // Toggle DDU backpressure ignore.
			scale = myDDU->readGbEPrescale();
			if (scale & 0x0008) {
				uploadValue = (scale & 0xf7f7) | 0x8080;
			} else {
				uploadValue = (scale | 0x0808) & 0x7f7f;
			}
			// No break...  I know what I am doing.

		case (11): // Write prescale
			myDDU->writeGbEPrescale(uploadValue);
			break;

		case (12): // Toggle something in the fake L1A register?
			scale = myDDU->readFakeL1();
			if (scale  & 0x0007) {
				uploadValue = 0xF0F0;
			} else {
				uploadValue = 0x8787;
			}
			// No break...  I know what I am doing.

		case (13): // Write fake L1A register value?
			myDDU->writeFakeL1(uploadValue);
			break;

		case (14): // Write the flash killFiber
			myDDU->writeFlashKillFiber(uploadValue);
			break;

		case (15): // SPECIAL CASE:  write GbEFIFOThresholds
			writeMe.push_back(uploadValue & 0xffff);
			writeMe.push_back((uploadValue & 0xffff0000) >> 16);
			writeMe.push_back(otherValue);
			myDDU->writeFlashGbEFIFOThresholds(writeMe);
			break;

		case (16): // Flash board ID
			myDDU->writeFlashBoardID(uploadValue);
			break;

		case (17): // Flash RUI
			myDDU->writeFlashRUI(uploadValue);
			break;

		case (18): // Load FMM
			myDDU->writeFMM( uploadValue );
			break;

		default:
			LOG4CPLUS_ERROR(getApplicationLogger(), "command(" << command << ") not understood.");
			break;
		}

		std::stringstream location;
		location << "crate=" << cgiCrate << "&slot=" << cgiDDU;
		return webRedirect(out,"DDUExpert?"+location.str());

	} catch (xcept::Exception &e) {
		std::ostringstream error;
		error << "Exception caught!";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
		
		std::vector<xcept::ExceptionInformation> history = e2.getHistory();
		for (std::vector<xcept::ExceptionInformation>::iterator iError = history.begin(); iError != history.end(); iError++) {
			DataTable exceptionTable;
			unsigned int iRow = 0;
			std::map<std::string, std::string> messages = iError->getProperties();
			for (std::map<std::string, std::string>::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++) {
				exceptionTable(iRow, 0) << iMessage->first << ":";
				exceptionTable(iRow, 1) << iMessage->second;
				iRow++;
			}
			*out << cgicc::div(exceptionTable.toHTML())
				.set("class", "exception");
		}
	}

}



void emu::fed::EmuFCrateHyperDAQ::DCCBroadcast(xgi::Input *in, xgi::Output *out)
{
	
	try {
		// PGK Patented check-for-initialization
		if (crateVector_.size()==0) {
			LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
			return Default(in,out);
		}
		
		cgicc::Cgicc cgi(in);
		
		// First, I need a crate.
		std::pair<unsigned int, Crate *> cratePair = getCGICrate(cgi);
		unsigned int cgiCrate = cratePair.first;
		Crate *myCrate = cratePair.second;
		
		
		std::stringstream sTitle;
		sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DCC Firmware Manager";
		std::vector<std::string> jsFileNames;
		jsFileNames.push_back("bitFlipper.js");
		jsFileNames.push_back("formChecker.js");
		*out << Header(sTitle.str(),jsFileNames);
		
		
		// PGK Select-a-Crate/Slot
		*out << cgicc::fieldset()
			.set("class","fieldset") << std::endl;
		*out << cgicc::div("View this page for a different crate/board")
			.set("class","legend") << std::endl;
		
		*out << selectACrate("DCCBroadcast","crate",cgiCrate,cgiCrate) << std::endl;
		
		*out << cgicc::span("Configuration located at " + xmlFile_.toString())
			.set("style","color: #A00; font-size: 10pt;") << std::endl;
		
		*out << cgicc::fieldset() << std::endl;
		*out << cgicc::br() << std::endl;
		
		// The names of the PROMs
		std::vector< std::string > dccPROMNames;
		dccPROMNames.push_back("INPROM");
		dccPROMNames.push_back("MPROM");
		
		// The device types of the PROMs
		std::vector<enum DEVTYPE> dccPROMTypes;
		dccPROMTypes.push_back(INPROM);
		dccPROMTypes.push_back(MPROM);
		
		// Store the codes to compare them to the FPGAs
		std::vector<uint32_t> diskPROMCodes;
		
		*out << cgicc::fieldset()
			.set("class","normal") << std::endl;
		*out << cgicc::div("Step 1:  Upload DCC firmware to disk")
			.set("class","legend") << std::endl;
		
		DataTable diskTable;
		diskTable->set("id", "diskTable").set("class", "data");
		
		diskTable(0,0) << "DCC PROM Name";
		diskTable(0,1) << "On-Disk Firmware Version";
		diskTable(0,2) << "Upload New Firmware";
		
		// Loop over the prom types and give us a pretty table
		for (unsigned int iprom = 0; iprom < dccPROMNames.size(); iprom++) {
			
			diskTable(iprom + 1,0) << dccPROMNames[iprom];
			
			// Get the version number from the on-disk file
			// Best mashup of perl EVER!
			//std::ostringstream systemCall;
			//std::string diskVersion;
			std::string fileName = "Current" + dccPROMNames[iprom] + ".svf";

			std::ifstream inFile(fileName.c_str(), std::ifstream::in);
			
			unsigned long int diskVersion = 0;
			
			// Can't have bogus files
			if (!inFile.is_open()) {
				LOG4CPLUS_ERROR(getApplicationLogger(), "Cannot open file " << fileName);
				diskTable(iprom + 1,1) << "ERROR READING LOCAL FILE -- UPLOAD A NEW FILE";
				diskTable(iprom + 1,1).setClass("bad");
			} else {
				
				// Now start parsing the file.  Read lines until we have an eof or the usercode
				while (!inFile.eof()) {
					
					// Each line is a command (or comment)
					std::string myLine;
					getline(inFile, myLine);
					
					// Only care about one particular command:  instruction 0xfd
					if (myLine.find("SIR 8 TDI (fd)") != std::string::npos) {
						// The next line contains the usercode.
						// Make sure there is a next line.
						if (inFile.eof()) break;
						
						getline(inFile, myLine);
						
						std::stringstream lineStream(myLine);
						std::string parsedLine;
						lineStream >> parsedLine; // SDR
						lineStream >> parsedLine; // 32
						lineStream >> parsedLine; // TDI
						lineStream >> parsedLine; // (########)
						
						parsedLine = parsedLine.substr(1,8);
						
						sscanf(parsedLine.c_str(), "%08lx", &diskVersion);

						break;
					}
				}
				
				inFile.close();
			}
			
			// Now the std::string diskVersion is exactly what is sounds like.
			diskPROMCodes.push_back(diskVersion);
			
			// Check to make sure the on-disk header looks like it should for that
			//  particular PROM
			//if (dccPROMTypes[iprom] == MPROM) {
			//	diskTable(iprom + 1,1) << "MPROM has no usercode";
			//	diskTable(iprom + 1,1).setClass("undefined");
			//} else {
			std::stringstream diskVersionString;
			diskVersionString << std::hex << diskVersion;
			std::string diskHeader( diskVersionString.str(), 0, 3 );
			if ( diskHeader != "dcc" ) {
				diskTable(iprom + 1,1) << "ERROR READING LOCAL FILE -- UPLOAD A NEW FILE";
				diskTable(iprom + 1,1).setClass("bad");
			} else {
				diskTable(iprom + 1,1) << std::hex << diskVersion;
				diskTable(iprom + 1,1).setClass("ok");
			}
			//}
			
			// Make the last part of the table a form for uploading a file.
			diskTable(iprom + 1,2) << cgicc::form().set("method","POST")
				.set("enctype","multipart/form-data")
				.set("id","Form" + dccPROMNames[iprom])
				.set("action","/" + getApplicationDescriptor()->getURN() + "/DCCLoadBroadcast") << std::endl;
			diskTable(iprom + 1,2) << cgicc::input().set("type","file")
				.set("name","File")
				.set("id","File" + dccPROMNames[iprom])
				.set("size","50") << std::endl;
			//diskTable(iprom + 1,2) << cgicc::input().set("type","button")
			//	.set("value","Upload SVF")
			//	.set("onClick","javascript:if (formCheck('File" + dccPROMNames[iprom] + "')) { document.getElementById('Form" + dccPROMNames[iprom] + "').submit(); }") << std::endl;
			diskTable(iprom + 1,2) << cgicc::input().set("type","submit")
				.set("value","Upload SVF") << std::endl;
			diskTable(iprom + 1,2) << cgicc::input().set("type","hidden")
				.set("name","svftype")
				.set("value",dccPROMNames[iprom]) << std::endl;
			std::ostringstream crateVal;
			crateVal << cgiCrate;
			diskTable(iprom + 1,2) << cgicc::input().set("type","hidden")
				.set("name","crate")
				.set("value",crateVal.str()) << std::endl;
			diskTable(iprom + 1,2) << cgicc::form() << std::endl;
		}
		
		// Print the table to screen.
		*out << diskTable.toHTML() << std::endl;
		
		*out << cgicc::fieldset() << std::endl;
		
		
		// This is the key:  a form that is outside of everything, and bitwise
		//  selectors of the slots to upgrade.
		*out << cgicc::form()
			.set("action","/" + getApplicationDescriptor()->getURN() + "/DCCSendBroadcast")
			.set("method","GET");
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","slots")
			.set("id","slots")
			.set("value","0x0");
		
		for (unsigned int iprom = 0; iprom < dccPROMNames.size(); iprom++) {
			std::stringstream promCode;
			promCode << diskPROMCodes[iprom];
			*out << cgicc::input()
				.set("type","hidden")
				.set("name",dccPROMNames[iprom])
				.set("value",promCode.str()) << std::endl;
		}
		
		*out << cgicc::fieldset()
			.set("class","normal") << std::endl;
		*out << cgicc::div("Step 2:  Select the slots you to which you intend to load firmware")
			.set("class","legend") << std::endl;
		
		DataTable slotTable;
		slotTable->set("id", "slotTable").set("class", "data");
		
		slotTable(0,0) << "Slot number";
		for (unsigned int iprom = 0; iprom < dccPROMNames.size(); iprom++) {
			slotTable(0, iprom + 1) << dccPROMNames[iprom] + " IDCode";
			slotTable(0, iprom + 2) << dccPROMNames[iprom] + " UserCode";
		}
		
		
		std::vector<DCC *> myDCCs = myCrate->getDCCs();
		for (unsigned int iDCC = 0; iDCC != myDCCs.size(); iDCC++) {
			
			DCC *myDCC = myDCCs[iDCC];
			
			std::ostringstream bitFlipCommand;
			bitFlipCommand << "Javascript:toggleBit('slots'," << myDCC->slot() << ");";
			
			std::ostringstream elementID;
			elementID << "slotBox" << myDCC->slot();
			
			slotTable(iDCC + 1,0) << cgicc::input()
				.set("type","checkbox")
				.set("class","slotBox")
				.set("id",elementID.str())
				.set("onChange",bitFlipCommand.str()) << " "
				<< cgicc::label()
				.set("for",elementID.str()) << myDCC->slot() << cgicc::label();
			
			for (unsigned int iprom = 0; iprom < dccPROMNames.size(); iprom++) {
				
				//if (dccPROMTypes[iprom] == MPROM) {
				//	slotTable(iDCC + 1,1 + 2*iprom) << "MPROM has no IDcode";
				//	slotTable(iDCC + 1,1 + 2*iprom).setClass("undefined");
					
				//	slotTable(iDCC + 1,2 + 2*iprom) << "MPROM has no usercode";
				//	slotTable(iDCC + 1,2 + 2*iprom).setClass("undefined");
				//} else {
				uint32_t idCode = myDCC->readIDCode(dccPROMTypes[iprom]);
				slotTable(iDCC + 1,1 + 2*iprom) << std::hex << idCode;
				
				uint32_t userCode = myDCC->readUserCode(dccPROMTypes[iprom]);
				slotTable(iDCC + 1,2 + 2*iprom) << std::hex << userCode;
				
				// Check for consistency
				slotTable(iDCC + 1,1 + 2*iprom).setClass("none");
				slotTable(iDCC + 1,2 + 2*iprom).setClass("ok");
				if (diskPROMCodes[iprom] != userCode) {
					slotTable(iDCC + 1,2 + 2*iprom).setClass("bad");
				}
				//}
			}
		}
		
		*out << slotTable.toHTML() << std::endl;
		
		*out << cgicc::div()
			.set("style","font-size: 8pt;") << std::endl;
		*out << "Legend: " << cgicc::span("All OK").set("class","ok") << " " << std::endl;
		*out << cgicc::span("Disk/PROM mismatch").set("class","bad") << " " << std::endl;

		*out << cgicc::div() << std::endl;
		
		*out << cgicc::fieldset() << std::endl;
		
		
		*out << cgicc::fieldset()
			.set("class","normal") << std::endl;
		*out << cgicc::div("Step 3:  Press a button to upload that particular firmware to the selected boards")
			.set("class","legend") << std::endl;
		
		*out << cgicc::input()
			.set("type","submit")
			.set("name","submit")
			.set("value","Send MPROM (Emergency Load)") << std::endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("name","submit")
			.set("value","Send INPROM") << std::endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("name","submit")
			.set("value","Send MPROM") << std::endl;
		
		// The crate is a must.
		std::ostringstream crateVal;
		crateVal << cgiCrate;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","crate")
			.set("value",crateVal.str()) << std::endl;
		
		*out << cgicc::fieldset() << std::endl;
		
		*out << cgicc::form() << std::endl;
		
		
		*out << cgicc::fieldset()
			.set("class","normal") << std::endl;
		*out << cgicc::div("Step 4:  Reset the FPGAs")
			.set("class","legend") << std::endl;
		
		*out << cgicc::form()
			.set("action","/" + getApplicationDescriptor()->getURN() + "/DCCReset?crate=" + crateVal.str())
			.set("method","post") << std::endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("value","Reset FPGAs") << std::endl;
		*out << cgicc::form() << std::endl;
		
		*out << cgicc::fieldset() << std::endl;
		
		*out << Footer() << std::endl;
		
	} catch (xcept::Exception &e) {
		std::ostringstream error;
		error << "Exception caught!";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
		
		std::vector<xcept::ExceptionInformation> history = e2.getHistory();
		for (std::vector<xcept::ExceptionInformation>::iterator iError = history.begin(); iError != history.end(); iError++) {
			DataTable exceptionTable;
			unsigned int iRow = 0;
			std::map<std::string, std::string> messages = iError->getProperties();
			for (std::map<std::string, std::string>::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++) {
				exceptionTable(iRow, 0) << iMessage->first << ":";
				exceptionTable(iRow, 1) << iMessage->second;
				iRow++;
			}
			*out << cgicc::div(exceptionTable.toHTML())
				.set("class", "exception");
		}
	}
	
}



void emu::fed::EmuFCrateHyperDAQ::DCCLoadBroadcast(xgi::Input *in, xgi::Output *out)
{
	
	try {
		
		// PGK Patented check-for-initialization
		if (crateVector_.size()==0) {
			LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
			return Default(in,out);
		}
		
		cgicc::Cgicc cgi(in);
		
		// First, I need a crate.
		std::pair<unsigned int, Crate *> cratePair = getCGICrate(cgi);
		unsigned int cgiCrate = cratePair.first;
		//Crate *myCrate = cratePair.second;

		std::string type = cgi["svftype"]->getValue();
		
		if (type != "INPROM" && type != "MPROM") {
			std::ostringstream error;
			error << "PROM type " << type << " not understood";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			XCEPT_DECLARE(emu::fed::exception::ParseException, e, error.str());
			notifyQualified("ERROR", e);
			
			std::ostringstream backLocation;
			backLocation << "DCCBroadcast?crate=" << cgiCrate;
			return webRedirect(out,backLocation.str());
		}
		
		cgicc::const_file_iterator ifile = cgi.getFile("File");
		if ( (*ifile).getFilename() == "" ) {
			std::ostringstream error;
			error << "The file you attempted to upload either doesn't exist, or wasn't properly transferred";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			XCEPT_DECLARE(emu::fed::exception::FileException, e, error.str());
			notifyQualified("ERROR", e);
			
			std::ostringstream backLocation;
			backLocation << "DCCBroadcast?crate=" << cgiCrate;
			return webRedirect(out,backLocation.str());
		}
		
		std::string filename = "Current" + type + ".svf";
		std::ofstream outfile;
		outfile.open(filename.c_str(),std::ios::trunc);
		if (!outfile.is_open()) {
			std::ostringstream error;
			error << "The file " << filename << " is not accessable for writing";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			XCEPT_DECLARE(emu::fed::exception::FileException, e, error.str());
			notifyQualified("ERROR", e);
			
			std::ostringstream backLocation;
			backLocation << "DCCBroadcast?crate=" << cgiCrate;
			return webRedirect(out,backLocation.str());
		}
		
		(*ifile).writeToStream(outfile);
		outfile.close();
		
		LOG4CPLUS_ERROR(getApplicationLogger(), "Downloaded and saved to file " << filename << " firmware for " << type);
		
		std::ostringstream backLocation;
		backLocation << "DCCBroadcast?crate=" << cgiCrate;
		return webRedirect(out,backLocation.str());

	} catch (xcept::Exception &e) {
		std::ostringstream error;
		error << "Exception caught!";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
		
		std::vector<xcept::ExceptionInformation> history = e2.getHistory();
		for (std::vector<xcept::ExceptionInformation>::iterator iError = history.begin(); iError != history.end(); iError++) {
			DataTable exceptionTable;
			unsigned int iRow = 0;
			std::map<std::string, std::string> messages = iError->getProperties();
			for (std::map<std::string, std::string>::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++) {
				exceptionTable(iRow, 0) << iMessage->first << ":";
				exceptionTable(iRow, 1) << iMessage->second;
				iRow++;
			}
			*out << cgicc::div(exceptionTable.toHTML())
				.set("class", "exception");
		}
	}
}



void emu::fed::EmuFCrateHyperDAQ::DCCSendBroadcast(xgi::Input *in, xgi::Output *out)
{
	
	try {
		
		// PGK Patented check-for-initialization
		if (crateVector_.size()==0) {
			LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
			return Default(in,out);
		}
		
		cgicc::Cgicc cgi(in);
		
		// First, I need a crate.
		std::pair<unsigned int, Crate *> cratePair = getCGICrate(cgi);
		unsigned int cgiCrate = cratePair.first;
		Crate *myCrate = cratePair.second;

		// unsigned int tidcode[8] = {0x2124a093,0x31266093,0x31266093,0x05036093,0x05036093,0x05036093,0x05036093,0x05036093};
		// unsigned int tuscode[8] = {0xcf043a02,0xdf025a02,0xdf025a02,0xb0020a04,0xc043dd99,0xc143dd99,0xd0025a02,0xd1025a02};
		
		int type = -1;
		std::string submitCommand = cgi["submit"]->getValue();
		if (submitCommand.substr(5) == "INPROM") type = 0;
		if (submitCommand.substr(5) == "MPROM") type = 1;
		if (submitCommand.substr(5) == "MPROM (Emergency Load)") type = 2;
		
		
		//int broadcast = cgi["broadcast"]->getIntegerValue();
		std::string slotsText = cgi["slots"]->getValue();
		if (slotsText.substr(0,2) == "0x") slotsText = slotsText.substr(2);
		unsigned int slots = 0;
		sscanf(slotsText.data(),"%4x",&slots);
		
		// Error:  no slots to load.
		if (!slots) {
			LOG4CPLUS_ERROR(getApplicationLogger(),"No slots selected for firmware loading, and broadcast not set");
			std::ostringstream backLocation;
			backLocation << "DCCBroadcast?crate=" << cgiCrate;
			return webRedirect(out,backLocation.str());
		}
		
		if (type != 0 && type != 1 && type != 2) {
			std::ostringstream error;
			error << "PROM type not understood";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			XCEPT_DECLARE(emu::fed::exception::ParseException, e, error.str());
			notifyQualified("ERROR", e);
			
			std::ostringstream backLocation;
			backLocation << "DCCBroadcast?crate=" << cgiCrate;
			return webRedirect(out,backLocation.str());
		}
		
		std::vector<std::string> promName;
		promName.push_back("INPROM");
		promName.push_back("MPROM");
		promName.push_back("MPROM");
		std::vector<enum DEVTYPE> devType;
		devType.push_back(INPROM);
		devType.push_back(MPROM);
		devType.push_back(RESET);
		
		// Load the proper version types from the cgi handle.
		std::vector<uint32_t> version;
		for (int iProm = 0; iProm < 2; iProm++) {
			// cgicc can't handle 32-bit ints.  Parse myself from the string.
			std::string versionString = cgi[promName[iProm].c_str()]->getValue();
			unsigned long int versionCache = 0;
			sscanf(versionString.c_str(), "%lu", &versionCache);
			version.push_back(versionCache);
		}
		

		std::vector<DCC *> myDCCs = myCrate->getDCCs();
		for (std::vector<DCC *>::iterator iDCC = myDCCs.begin(); iDCC != myDCCs.end(); iDCC++) {
			
			if (!(slots & (1 << (*iDCC)->slot()))) continue;

			LOG4CPLUS_DEBUG(getApplicationLogger(),"Loading firmware to prom " << type << "...");
			
			std::string filename = "Current" + promName[type] + ".svf";

			LOG4CPLUS_INFO(getApplicationLogger(),"Loading file " << filename << " (v " << std::hex << version[type] << std::dec << ") to DCC slot " << (*iDCC)->slot() << "...");
			
			(*iDCC)->loadPROM(devType[type],(char *) filename.c_str());

			// Check the usercode.
			if (devType[type] != RESET) {
				uint32_t checkCode = (*iDCC)->readUserCode(devType[type]);
				if (checkCode != version[type]) {
					std::ostringstream error;
					error << "Firmware load failed: expected usercode " << std::hex << version[type] << ", got " << checkCode << std::dec;
					LOG4CPLUS_WARN(getApplicationLogger(), error.str());
					XCEPT_DECLARE(emu::fed::exception::ParseException, e, error.str());
					notifyQualified("WARN", e);
				} else {
					LOG4CPLUS_INFO(getApplicationLogger(),"Firmware load succeeded: expected usercode " << std::hex << version[type] << ", got " << checkCode << std::dec);
				}
			}
		}
		
		LOG4CPLUS_INFO(getApplicationLogger(),"All firmware loading operations complete.");
		
		std::ostringstream backLocation;
		backLocation << "DCCBroadcast?crate=" << cgiCrate;
		return webRedirect(out,backLocation.str());
		
	} catch (xcept::Exception &e) {
		std::ostringstream error;
		error << "Exception caught!";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
		
		std::vector<xcept::ExceptionInformation> history = e2.getHistory();
		for (std::vector<xcept::ExceptionInformation>::iterator iError = history.begin(); iError != history.end(); iError++) {
			DataTable exceptionTable;
			unsigned int iRow = 0;
			std::map<std::string, std::string> messages = iError->getProperties();
			for (std::map<std::string, std::string>::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++) {
				exceptionTable(iRow, 0) << iMessage->first << ":";
				exceptionTable(iRow, 1) << iMessage->second;
				iRow++;
			}
			*out << cgicc::div(exceptionTable.toHTML())
				.set("class", "exception");
		}
	}

}



void emu::fed::EmuFCrateHyperDAQ::DCCDebug(xgi::Input *in, xgi::Output *out)
{
	
	try {
			
		// PGK Patented check-for-initialization
		if (crateVector_.size()==0) {
			LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
			return Default(in,out);
		}

		cgicc::Cgicc cgi(in);
		
		// First, I need a crate.
		std::pair<unsigned int, Crate *> cratePair = getCGICrate(cgi);
		unsigned int cgiCrate = cratePair.first;
		Crate *myCrate = cratePair.second;

		std::pair<unsigned int, DCC *> boardPair = getCGIBoard<DCC>(cgi);
		unsigned int cgiDCC = boardPair.first;
		DCC *myDCC = boardPair.second;

		std::stringstream sTitle;
		sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DCC Debugging Information (Crate " << myCrate->number() << " Slot #" << myDCC->slot() << ")";
		std::vector<std::string> jsFileNames;
		jsFileNames.push_back("tableToggler.js");
		jsFileNames.push_back("errorFlasher.js");
		*out << Header(sTitle.str(),jsFileNames);

		// PGK Select-a-Crate/Slot
		*out << cgicc::fieldset()
			.set("class","fieldset") << std::endl;
		*out << cgicc::div("View this page for a different crate/board")
			.set("class","legend") << std::endl;

		*out << selectACrate("DCCDebug","dcc",cgiDCC,cgiCrate) << std::endl;

		*out << cgicc::span("Configuration located at " + xmlFile_.toString())
			.set("style","color: #A00; font-size: 10pt;") << std::endl;
		
		*out << cgicc::fieldset() << std::endl;
		*out << cgicc::br() << std::endl;
		
		// Display general DCC status information
		*out << cgicc::fieldset()
			.set("class","normal") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << cgicc::a("&plusmn;")
			.set("class","openclose")
			.set("onClick","javascript:toggle('generalTable')") << std::endl;
		*out << "General DCC Information" << cgicc::div() << std::endl;

		
		DataTable generalTable;
		generalTable->set("id", "generalTable").set("class", "data");
		
		generalTable(0,0) << "Register";
		generalTable(0,1) << "Value";
		generalTable(0,2) << "Decoded Information";
		
		generalTable(1,0) << "DCC FMM Status (4-bit)";
		unsigned long int dccValue = myDCC->readStatusHigh();
		generalTable(1,1) << std::showbase << std::hex << ((dccValue & 0xf000) >> 12);
		std::map<std::string, std::string> debugMap = DCCDebugger::FMMStat((dccValue & 0xf000) >> 12);
		for (std::map<std::string, std::string>::iterator iDebug = debugMap.begin(); iDebug != debugMap.end(); iDebug++) {
			generalTable(1,2) << cgicc::div(iDebug->first)
				.set("class",iDebug->second);
		}
		if (((dccValue & 0xf000) >> 12) == 0x3) {
			generalTable(1,1).setClass("bad");
		} else if (((dccValue & 0xf000) >> 12) == 0x1) {
			generalTable(1,1).setClass("warning");
		} else if (((dccValue & 0xf000) >> 12) == 0x4) {
			generalTable(1,1).setClass("caution");
		} else if (((dccValue & 0xf000) >> 12) == 0x8) {
			generalTable(1,1).setClass("bad");
		} else {
			generalTable(1,1).setClass("ok");
		}

		generalTable(2,0) << "DCC FIFO Backpressure (8-bit)";
		generalTable(2,1) << std::showbase << std::hex << ((dccValue & 0x0ff0) >> 4);
		debugMap = DCCDebugger::InFIFOStat((dccValue & 0x0ff0) >> 4);
		for (std::map<std::string, std::string>::iterator iDebug = debugMap.begin(); iDebug != debugMap.end(); iDebug++) {
			generalTable(2,2) << cgicc::div(iDebug->first)
			.set("class",iDebug->second);
		}
		if (dccValue & 0x0f80) {
			generalTable(2,1).setClass("bad");
		} else if (dccValue & 0x0070) {
			generalTable(2,1).setClass("warning");
		} else {
			generalTable(2,1).setClass("ok");
		}

		generalTable(3,0) << "DCC S-Link Status (4-bit)";
		generalTable(3,1) << std::showbase << std::hex << (dccValue & 0xf);
		debugMap = DCCDebugger::SLinkStat(dccValue & 0xf);
		for (std::map<std::string, std::string>::iterator iDebug = debugMap.begin(); iDebug != debugMap.end(); iDebug++) {
			generalTable(3,2) << cgicc::div(iDebug->first)
				.set("class",iDebug->second);
		}
		if (!(dccValue & 0x8) || !(dccValue & 0x2)) {
			generalTable(3,1).setClass("caution");
		}
		if (!(dccValue & 0x4) || !(dccValue & 0x1)) {
			generalTable(3,1).setClass("bad");
		}

		generalTable(4,0) << "DCC L1A Count (16-bit)";
		generalTable(4,1) << myDCC->readStatusLow();
		generalTable(4,1).setClass("none");

		*out << generalTable.printSummary() << std::endl;

		*out << generalTable.toHTML() << std::endl;

		*out << cgicc::fieldset() << std::endl;

		// Clever trick to help with formating the cut-and-paste.
		*out << cgicc::br()
			.set("style","display: none") << std::endl;


		// Display DCC input FIFO status information
		*out << cgicc::fieldset()
			.set("class","normal") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << cgicc::a("&plusmn;")
			.set("class","openclose")
			.set("onClick","javascript:toggle('ratesTable')") << std::endl;
		*out << "DCC Input/Output Rate Information" << cgicc::div() << std::endl;

		DataTable ratesTable;
		ratesTable->set("id", "ratesTable").set("class", "data");
		
		ratesTable(0,0) << "Register";
		ratesTable(0,1) << "Value";
		ratesTable(0,2) << "Decoded Information";
		
		
		ratesTable(1,0) << "Input FIFOs Used";
		dccValue = myDCC->readFIFOInUse();
		ratesTable(1,1) << std::showbase << std::hex << dccValue;
		for (int iFifo = 0; iFifo < 10; iFifo++) {
			if (dccValue & (1<<iFifo)) {
			    int fifoToLookup = iFifo+1;
			    if (fifoToLookup >= 6) fifoToLookup++;
				ratesTable(1,2) << cgicc::div()
					.set("class","none") << "FIFO " << (fifoToLookup) << " (Slot " << myDCC->getDDUSlotFromFIFO(fifoToLookup) << ")" << cgicc::div();
			}
		}
		ratesTable(1,1).setClass("none");

		// Get rates for each FIFO, Slink1 first.
		for (int iFifo = 0; iFifo < 5; iFifo++) {
			ratesTable(iFifo+2,0) << "FIFO " << (iFifo+1) << " (DDU Slot " << myDCC->getDDUSlotFromFIFO(iFifo+1) << ")";
			dccValue = myDCC->readRate(iFifo+1);
			ratesTable(iFifo+2,1) << std::dec << dccValue << " bytes/s";
			ratesTable(iFifo+2,1).setClass("none");
		}

		ratesTable(7,0) << "S-Link 1";
		dccValue = myDCC->readRate(0);
		ratesTable(7,1) << std::dec << dccValue << " bytes/s";
		ratesTable(7,1).setClass("none");

		// Get rates for each FIFO, Slink2 second.
		for (int iFifo = 5; iFifo < 10; iFifo++) {
			ratesTable(iFifo+3,0) << "FIFO " << (iFifo+1) << " (DDU Slot " << myDCC->getDDUSlotFromFIFO(iFifo+2) << ")";
			dccValue = myDCC->readRate(iFifo+2);
			ratesTable(iFifo+3,1) << std::dec << dccValue << " bytes/s";
			ratesTable(iFifo+3,1).setClass("none");
		}

		ratesTable(13,0) << "S-Link 2";
		dccValue = myDCC->readRate(6);
		ratesTable(13,1) << std::dec << dccValue << " bytes/s";
		ratesTable(13,1).setClass("none");


		*out << ratesTable.toHTML() << std::endl;

		*out << cgicc::fieldset() << std::endl;

		// Clever trick to help with formating the cut-and-paste.
		*out << cgicc::br()
			.set("style","display: none") << std::endl;
			
		*out << Footer() << std::endl;
		
	} catch (xcept::Exception &e) {
		std::ostringstream error;
		error << "Exception caught!";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
		
		std::vector<xcept::ExceptionInformation> history = e2.getHistory();
		for (std::vector<xcept::ExceptionInformation>::iterator iError = history.begin(); iError != history.end(); iError++) {
			DataTable exceptionTable;
			unsigned int iRow = 0;
			std::map<std::string, std::string> messages = iError->getProperties();
			for (std::map<std::string, std::string>::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++) {
				exceptionTable(iRow, 0) << iMessage->first << ":";
				exceptionTable(iRow, 1) << iMessage->second;
				iRow++;
			}
			*out << cgicc::div(exceptionTable.toHTML())
				.set("class", "exception");
		}
	}
}



void emu::fed::EmuFCrateHyperDAQ::DCCExpert(xgi::Input *in, xgi::Output *out)
{
	
	try {
		
		// PGK Patented check-for-initialization
		if (crateVector_.size()==0) {
			LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
			return Default(in,out);
		}

		cgicc::Cgicc cgi(in);
		
		// First, I need a crate.
		std::pair<unsigned int, Crate *> cratePair = getCGICrate(cgi);
		unsigned int cgiCrate = cratePair.first;
		Crate *myCrate = cratePair.second;

		std::pair<unsigned int, DCC *> boardPair = getCGIBoard<DCC>(cgi);
		unsigned int cgiDCC = boardPair.first;
		DCC *myDCC = boardPair.second;

		std::stringstream sTitle;
		sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DCC Expert Controls (Crate " << myCrate->number() << " Slot #" << myDCC->slot() << ")";
		std::vector<std::string> jsFileNames;
		jsFileNames.push_back("bitFlipper.js");
		jsFileNames.push_back("errorFlasher.js");
		*out << Header(sTitle.str(),jsFileNames);

		// PGK Select-a-Crate/Slot
		*out << cgicc::fieldset()
			.set("class","fieldset") << std::endl;
		*out << cgicc::div("View this page for a different crate/board")
			.set("class","legend") << std::endl;

		*out << selectACrate("DCCExpert","dcc",cgiDCC,cgiCrate) << std::endl;

		*out << cgicc::span("Configuration located at " + xmlFile_.toString())
			.set("style","color: #A00; font-size: 10pt;") << std::endl;
		
		*out << cgicc::fieldset() << std::endl;
		*out << cgicc::br() << std::endl;
		
		// DCC FIFOs in use setting
		*out << cgicc::fieldset()
			.set("class","expert") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << "DCC FIFOs In Use" << cgicc::div() << std::endl;

		// Priceless for the following
		std::string dccTextLoad = "/"+getApplicationDescriptor()->getURN()+"/DCCTextLoad";
		std::ostringstream dccValStream;
		dccValStream << cgiDCC;
		std::string dccVal = dccValStream.str();
		std::ostringstream crateValStream;
		crateValStream << cgiCrate;
		std::string crateVal = crateValStream.str();

		*out << cgicc::form()
			.set("method","GET")
			.set("action",dccTextLoad) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","command")
			.set("value","3") << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","slot")
			.set("value",dccVal) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","crate")
			.set("value",crateVal) << std::endl;

		unsigned long int currentFIFOs = myDCC->readFIFOInUse();
		*out << cgicc::div("Current FIFOs In Use register value: ")
			.set("style","font-weight: bold; display: inline;");
		*out << cgicc::div()
			.set("style","display: inline;");
		*out << std::showbase << std::hex << currentFIFOs << cgicc::div() << std::endl;
		*out << cgicc::br() << std::endl;
		*out << cgicc::div("New FIFOs In Use register value (change this below): ")
			.set("style","font-weight: bold; display: inline; color: #090;");
		std::stringstream kfValue;
		kfValue << std::hex << std::showbase << currentFIFOs;
		*out << cgicc::input()
			.set("type","text")
			.set("name","textdata")
			.set("value",kfValue.str())
			.set("size","8")
			.set("id","fifo") << std::endl;

		*out << cgicc::input()
			.set("type","submit")
			.set("value","Load FIFOs In Use Register") << std::endl;

		*out << cgicc::div("This is a volatile register, and will be reprogrammed on a hard reset.  If you wish to make this change perminent, change the configuration file (see the top of this page)")
			.set("style","font-size: 8pt; color: #900;") << std::endl;

		// A table for the first half of the form.
		*out << cgicc::table()
			.set("style","width: 100%; margin: 5px auto 5px auto; font-size: 8pt; border-collapse: collapse;") << std::endl;
		*out << cgicc::tr() << std::endl;

		for (unsigned int iFifo = 0; iFifo < 10; iFifo++) {
			*out << cgicc::td()
				.set("class","none")
				.set("style","border: 1px solid #000; border-bottom-width: 0px; font-size: 8pt; width: 10%; background-color: #FFF;") << "FIFO " << std::dec << (iFifo+1) << cgicc::td() << std::endl;
		}

		*out << cgicc::tr() << std::endl;
		*out << cgicc::tr() << std::endl;
		
		// Loop through the fifos.
		for (unsigned int iFifo=0; iFifo<10; iFifo++) {
			unsigned int fifoToLookup = iFifo + 1;
			if (fifoToLookup >= 6) fifoToLookup++;
			unsigned int slot = myDCC->getDDUSlotFromFIFO(fifoToLookup);
			if (slot > 3 && slot <= 21) {
				std::ostringstream slotName;
				slotName << "DDU Slot " << slot;
				*out << cgicc::td(slotName.str())
					.set("class","none")
					.set("style","border: 1px solid #000; border-top-width: 0px; width: 10%; font-weight: bold; background-color: #FFF;") << std::endl;
			} else {
				*out << cgicc::td("???")
					.set("class","none")
					.set("style","border: 1px solid #000; border-top-width: 0px; width: 10%; font-weight: bold;") << std::endl;
			}
		}
		*out << cgicc::tr();

		*out << cgicc::tr() << std::endl;
		for (unsigned int iFifo = 0; iFifo < 10; iFifo++) {
			*out << cgicc::td() << std::endl;
			std::stringstream radioName;
			radioName << "fifo" << iFifo;
			std::stringstream swapCommand;
			swapCommand << "javascript:toggleBit('fifo'," << iFifo << ");";
			if (currentFIFOs & 1<<iFifo) {
				*out << cgicc::input()
					.set("type","radio")
					.set("name",radioName.str())
					.set("id",radioName.str() + "live")
					.set("value","1")
					.set("onChange",swapCommand.str())
					.set("checked","checked") << cgicc::label("Live").set("for",radioName.str() + "live") << cgicc::br() << std::endl;
				*out << cgicc::input()
					.set("type","radio")
					.set("name",radioName.str())
					.set("id",radioName.str() + "killed")
					.set("value","0")
					.set("onChange",swapCommand.str()) << cgicc::label("Killed").set("for",radioName.str() + "killed") << std::endl;
			} else {
				*out << cgicc::input()
					.set("type","radio")
					.set("name",radioName.str())
					.set("id",radioName.str() + "live")
					.set("value","1")
					.set("onChange",swapCommand.str()) << cgicc::label("Live").set("for",radioName.str() + "live") << cgicc::br() << std::endl;
				*out << cgicc::input()
					.set("type","radio")
					.set("name",radioName.str())
					.set("id",radioName.str() + "killed")
					.set("value","0")
					.set("onChange",swapCommand.str())
					.set("checked","checked") << cgicc::label("Killed").set("for",radioName.str() + "killed") << std::endl;
			}
			*out << cgicc::td() << std::endl;
		}
		*out << cgicc::tr() << std::endl;
		*out << cgicc::table() << std::endl;

		*out << cgicc::form() << std::endl;
		*out << cgicc::fieldset() << std::endl;
		*out << cgicc::br()
			.set("style","display: none") << std::endl;


		// DCC Software Switch
		*out << cgicc::fieldset()
			.set("class","expert") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << "DCC Software Switch" << cgicc::div() << std::endl;

		*out << cgicc::form()
			.set("method","GET")
			.set("action",dccTextLoad) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","command")
			.set("value","6") << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","slot")
			.set("value",dccVal) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","crate")
			.set("value",crateVal) << std::endl;

		unsigned long int currentSwitch = myDCC->readSoftwareSwitch();
		*out << cgicc::div("Current Software Switch value: ")
			.set("style","font-weight: bold; display: inline;");
		*out << cgicc::div()
			.set("style","display: inline;");
		*out << std::showbase << std::hex << currentSwitch << cgicc::div() << std::endl;
		*out << cgicc::br() << std::endl;
		*out << cgicc::div("New Software Switch value (change this below): ")
			.set("style","font-weight: bold; display: inline; color: #090;");
		kfValue.str("");
		kfValue << std::hex << std::showbase << currentSwitch;
		*out << cgicc::input()
			.set("type","text")
			.set("name","textdata")
			.set("value",kfValue.str())
			.set("size","8")
			.set("id","switch") << std::endl;

		*out << cgicc::input()
			.set("type","submit")
			.set("value","Load Software Switch") << std::endl;
		
		// Now for some check-boxes.
		*out << cgicc::div()
			.set("style","font-size: 8pt;") << std::endl;
		if (!(currentSwitch & (1<<0)) && currentSwitch & (1<<9)) {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box09")
				.set("id","box09")
				.set("onChange","javascript:toggleBit('switch',9);clearBit('switch',0);")
				.set("checked","checked") << std::endl;
		} else {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box09")
				.set("id","box09")
				.set("onChange","javascript:toggleBit('switch',9);clearBit('switch',0);") << std::endl;
		}
		*out << cgicc::label()
			.set("for","box09")
			<< "Enable software switch" << cgicc::label() << cgicc::div() << std::endl;

		*out << cgicc::div()
			.set("style","font-size: 8pt;") << std::endl;
		if (!(currentSwitch & (1<<15)) && currentSwitch & (1<<12)) {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box1215")
				.set("id","box1215")
				.set("onChange","javascript:toggleBit('switch',12);clearBit('switch',15);")
				.set("checked","checked") << std::endl;
		} else {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box1215")
				.set("id","box1215")
				.set("onChange","javascript:toggleBit('switch',12);clearBit('switch',15);") << std::endl;
		}
		*out << cgicc::label()
			.set("for","box1215")
			<< "Set TTCrx NOT ready" << cgicc::label() << cgicc::div() << std::endl;

		*out << cgicc::div()
			.set("style","font-size: 8pt;") << std::endl;
		if (!(currentSwitch & (1<<14)) && currentSwitch & (1<<13)) {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box1314")
				.set("id","box1314")
				.set("onChange","javascript:toggleBit('switch',13);clearBit('switch',14);")
				.set("checked","checked") << std::endl;
		} else {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box1314")
				.set("id","box1314")
				.set("onChange","javascript:toggleBit('switch',13);clearBit('switch',14);") << std::endl;
		}
		*out << cgicc::label()
			.set("for","box1314")
			<< "Ignore S-Link full" << cgicc::label() << cgicc::div() << std::endl;

		*out << cgicc::div()
			.set("style","font-size: 8pt;") << std::endl;
		if (!(currentSwitch & (1<<13)) && currentSwitch & (1<<14)) {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box1413")
				.set("id","box1413")
				.set("onChange","javascript:toggleBit('switch',14);clearBit('switch',13);")
				.set("checked","checked") << std::endl;
		} else {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box1413")
				.set("id","box1413")
				.set("onChange","javascript:toggleBit('switch',14);clearBit('switch',13);") << std::endl;
		}
		*out << cgicc::label()
			.set("for","box1413")
			<< "Ignore S-Link full and S-Link not present" << cgicc::label() << cgicc::div() << std::endl;

		*out << cgicc::div()
			.set("style","font-size: 8pt;") << std::endl;
		if (currentSwitch & (1<<4)) {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","boxsw4")
				.set("id","boxsw4")
				.set("onChange","javascript:toggleBit('switch',4);")
				.set("checked","checked") << std::endl;
		} else {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","boxsw4")
				.set("id","boxsw4")
				.set("onChange","javascript:toggleBit('switch',4);") << std::endl;
		}
		*out << cgicc::label()
			.set("for","boxsw4")
			<< "Set \"SW4\"" << cgicc::label() << cgicc::div() << std::endl;

		*out << cgicc::div()
			.set("style","font-size: 8pt;") << std::endl;
		if (currentSwitch & (1<<5)) {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","boxsw5")
				.set("id","boxsw5")
				.set("onChange","javascript:toggleBit('switch',5);")
				.set("checked","checked") << std::endl;
		} else {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","boxsw5")
				.set("id","boxsw5")
				.set("onChange","javascript:toggleBit('switch',5);") << std::endl;
		}
		*out << cgicc::label()
			.set("for","boxsw5")
			<< "Set \"SW5\"" << cgicc::label() << cgicc::div() << std::endl;
		
		*out << cgicc::form() << std::endl;
		*out << cgicc::fieldset() << std::endl;
		*out << cgicc::br()
			.set("style","display: none;") << std::endl;


		
		// DCC FMM Manipulation
		*out << cgicc::fieldset()
			.set("class","expert") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << "DCC FMM Register" << cgicc::div() << std::endl;

		*out << cgicc::form()
			.set("method","GET")
			.set("action",dccTextLoad) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","command")
			.set("value","7") << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","slot")
			.set("value",dccVal) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","crate")
			.set("value",crateVal) << std::endl;

		unsigned long int currentFMM = myDCC->readFMM();
		*out << cgicc::div("Current FMM register: ")
			.set("style","font-weight: bold; display: inline;");
		*out << cgicc::div()
			.set("style","display: inline;");
		*out << std::showbase << std::hex << currentFMM << cgicc::div() << std::endl;
		*out << cgicc::br() << std::endl;
		*out << cgicc::div("New FMM register value (change this below): ")
			.set("style","font-weight: bold; display: inline; color: #090;");
		kfValue.str("");
		kfValue << std::hex << std::showbase << currentFMM;
		*out << cgicc::input()
			.set("type","text")
			.set("name","textdata")
			.set("value",kfValue.str())
			.set("size","8")
			.set("id","fmm") << std::endl;

		*out << cgicc::input()
			.set("type","submit")
			.set("value","Load FMM register") << std::endl;
		
		// Now for some check-boxes.
		*out << cgicc::div()
			.set("style","font-size: 8pt;") << std::endl;
		if ((currentFMM & (1<<4)) ^ (currentFMM & (1<<5))) {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box4x5")
				.set("id","box4x5")
				.set("onChange","javascript:toggleBit('fmm',4);")
				.set("checked","checked") << std::endl;
		} else {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box4x5")
				.set("id","box4x5")
				.set("onChange","javascript:toggleBit('fmm',4);") << std::endl;
		}
		*out << cgicc::label()
			.set("for","box4x5")
			<< "Enable user override" << cgicc::label() << cgicc::div() << std::endl;

		*out << cgicc::div()
			.set("style","font-size: 8pt;") << std::endl;
		if (currentFMM & (1<<0)) {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box0")
				.set("id","box0")
				.set("onChange","javascript:toggleBit('fmm',0);")
				.set("checked","checked") << std::endl;
		} else {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box0")
				.set("id","box0")
				.set("onChange","javascript:toggleBit('fmm',0);") << std::endl;
		}
		*out << cgicc::label()
			.set("for","box0")
			<< "Set FMM bit 0 (Busy)" << cgicc::label() << cgicc::div() << std::endl;

		*out << cgicc::div()
			.set("style","font-size: 8pt;") << std::endl;
		if (currentFMM & (1<<1)) {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box1")
				.set("id","box1")
				.set("onChange","javascript:toggleBit('fmm',1);")
				.set("checked","checked") << std::endl;
		} else {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box1")
				.set("id","box1")
				.set("onChange","javascript:toggleBit('fmm',1);") << std::endl;
		}
		*out << cgicc::label()
			.set("for","box1")
			<< "Set FMM bit 1 (Ready)" << cgicc::label() << cgicc::div() << std::endl;

		*out << cgicc::div()
			.set("style","font-size: 8pt;") << std::endl;
		if (currentFMM & (1<<2)) {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box2")
				.set("id","box2")
				.set("onChange","javascript:toggleBit('fmm',2);")
				.set("checked","checked") << std::endl;
		} else {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box2")
				.set("id","box2")
				.set("onChange","javascript:toggleBit('fmm',2);") << std::endl;
		}
		*out << cgicc::label()
			.set("for","box2")
			<< "Set FMM bit 2 (Warning)" <<  cgicc::label() << cgicc::div() << std::endl;

		*out << cgicc::div()
			.set("style","font-size: 8pt;") << std::endl;
		if (currentFMM & (1<<3)) {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box3")
				.set("id","box3")
				.set("onChange","javascript:toggleBit('fmm',3);")
				.set("checked","checked") << std::endl;
		} else {
			*out << cgicc::input()
				.set("type","checkbox")
				.set("name","box3")
				.set("id","box3")
				.set("onChange","javascript:toggleBit('fmm',3);") << std::endl;
		}
		*out << cgicc::label()
			.set("for","box3")
			<< "Set FMM bit 3 (Out-of-Sync)" << cgicc::label() << cgicc::div() << std::endl;
		
		*out << cgicc::form() << std::endl;
		*out << cgicc::fieldset() << std::endl;
		*out << cgicc::br()
			.set("style","display: none;") << std::endl;


		// DCC L1A manipulation
		*out << cgicc::fieldset()
			.set("class","expert") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << "DCC L1A" << cgicc::div() << std::endl;

		*out << cgicc::form()
			.set("method","GET")
			.set("action",dccTextLoad) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","command")
			.set("value","4") << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","slot")
			.set("value",dccVal) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","crate")
			.set("value",crateVal) << std::endl;

		unsigned long int currentTTCCommand = myDCC->readTTCCommand();
		unsigned long int currentL1A = ((((currentTTCCommand>>9)&0x60)+((currentFIFOs>>11)&0x1f)) << 8) | ((currentTTCCommand>>7)&0x7e);
		*out << cgicc::div("Current L1A send rate/number: ")
			.set("style","font-weight: bold; display: inline;");
		*out << cgicc::div()
			.set("style","display: inline;");
		*out << std::showbase << std::hex << currentL1A << cgicc::div() << std::endl;
		*out << cgicc::br() << std::endl;
		*out << cgicc::div("New L1A send rate/number: ")
			.set("style","font-weight: bold; display: inline; color: #090;");
		kfValue.str("");
		kfValue << std::hex << std::showbase << currentL1A;
		*out << cgicc::input()
			.set("type","text")
			.set("name","textdata")
			.set("value",kfValue.str())
			.set("size","8") << std::endl;

		*out << cgicc::input()
			.set("type","submit")
			.set("value","Load L1A register") << std::endl;

		*out << cgicc::form() << std::endl;
		*out << cgicc::fieldset() << std::endl;
		*out << cgicc::br()
			.set("style","display: none;") << std::endl;


		
		// DCC TCC Commands
		*out << cgicc::fieldset()
			.set("class","expert") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << "DCC TCC Commands" << cgicc::div() << std::endl;

		*out << cgicc::form()
			.set("method","GET")
			.set("action",dccTextLoad) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","command")
			.set("value","4") << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","slot")
			.set("value",dccVal) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","crate")
			.set("value",crateVal) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","textdata")
			.set("value","3") << std::endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("value","Send Sync Reset to FEDCrate") << std::endl;
		*out << cgicc::form() << std::endl;
		
		*out << cgicc::form()
			.set("method","GET")
			.set("action",dccTextLoad) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","command")
			.set("value","4") << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","slot")
			.set("value",dccVal) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","crate")
			.set("value",crateVal) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","textdata")
			.set("value","4") << std::endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("value","Send PCrate Hard Reset to DCC") << std::endl;
		*out << cgicc::form() << std::endl;

		*out << cgicc::form()
			.set("method","GET")
			.set("action",dccTextLoad) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","command")
			.set("value","4") << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","slot")
			.set("value",dccVal) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","crate")
			.set("value",crateVal) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","textdata")
			.set("value","0x1c") << std::endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("value","Send Soft Reset to FEDCrate") << std::endl;
		*out << cgicc::form() << std::endl;

		*out << cgicc::form()
			.set("method","GET")
			.set("action",dccTextLoad) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","command")
			.set("value","4") << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","slot")
			.set("value",dccVal) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","crate")
			.set("value",crateVal) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","textdata")
			.set("value","0x34") << std::endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("value","Send Hard Reset to FEDCrate") << std::endl;
		*out << cgicc::form() << std::endl;

		*out << cgicc::fieldset() << std::endl;
		*out << cgicc::br()
			.set("style","display: none;") << std::endl;
		

		// Miscellaneous DCC commands
		*out << cgicc::fieldset()
			.set("class","expert") << std::endl;
		*out << cgicc::div()
			.set("class","legend") << std::endl;
		*out << "Other DCC Commands" << cgicc::div() << std::endl;

		*out << cgicc::form()
			.set("method","GET")
			.set("action",dccTextLoad) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","command")
			.set("value","1") << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","slot")
			.set("value",dccVal) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","crate")
			.set("value",crateVal) << std::endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("value","Reset BX Counter") << std::endl;
		*out << cgicc::form() << std::endl;

		*out << cgicc::form()
			.set("method","GET")
			.set("action",dccTextLoad) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","command")
			.set("value","2") << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","slot")
			.set("value",dccVal) << std::endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","crate")
			.set("value",crateVal) << std::endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("value","Reset EVT Counter") << std::endl;
		*out << cgicc::form() << std::endl;

		*out << cgicc::fieldset() << std::endl;
		
		*out << Footer() << std::endl;
		
	} catch (xcept::Exception &e) {
		std::ostringstream error;
		error << "Exception caught!";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
		
		std::vector<xcept::ExceptionInformation> history = e2.getHistory();
		for (std::vector<xcept::ExceptionInformation>::iterator iError = history.begin(); iError != history.end(); iError++) {
			DataTable exceptionTable;
			unsigned int iRow = 0;
			std::map<std::string, std::string> messages = iError->getProperties();
			for (std::map<std::string, std::string>::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++) {
				exceptionTable(iRow, 0) << iMessage->first << ":";
				exceptionTable(iRow, 1) << iMessage->second;
				iRow++;
			}
			*out << cgicc::div(exceptionTable.toHTML())
				.set("class", "exception");
		}
	}
}



void emu::fed::EmuFCrateHyperDAQ::DCCTextLoad(xgi::Input *in, xgi::Output *out)
{
	
	try {
	
		// PGK Patented check-for-initialization
		if (crateVector_.size()==0) {
			LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
			return Default(in,out);
		}

		cgicc::Cgicc cgi(in);

		// First, I need a crate.
		std::pair<unsigned int, Crate *> cratePair = getCGICrate(cgi);
		unsigned int cgiCrate = cratePair.first;
		//Crate *myCrate = cratePair.second;

		std::pair<unsigned int, DCC *> boardPair = getCGIBoard<DCC>(cgi);
		unsigned int cgiDCC = boardPair.first;
		DCC *myDCC = boardPair.second;

		// The command that is intended to be sent to the DDU.
		unsigned int command = 0;
		cgicc::form_iterator name = cgi.getElement("command");
		if(name != cgi.getElements().end()) {
			command = cgi["command"]->getIntegerValue();
		}

		// Now load the data to be loaded, if applicable.
		std::string XMLText;
		name = cgi.getElement("textdata");
		if(name != cgi.getElements().end()) {
			XMLText = cgi["textdata"]->getValue();
		}

		// The decimal number (everything is loaded with decimal)
		unsigned long int uploadValue = 0;
		std::stringstream uploadStream;
		// If we have a hex number, 0x will prefix it.
		if (XMLText.substr(0,2) == "0x") {
			XMLText = XMLText.substr(2);
			uploadStream << XMLText;
			uploadStream >> std::hex >> uploadValue;
		} else {
			uploadStream << XMLText;
			uploadStream >> std::dec >> uploadValue;
		}
		
		// check for errors.
		if ((uploadStream.fail() || !uploadStream.eof()) && XMLText != "") {
			LOG4CPLUS_ERROR(getApplicationLogger(), "DCCTextLoad does not understand XMLText(" << XMLText << ")");
			std::stringstream location;
			location << "crate=" << cgiCrate << "&slot=" << cgiDCC;
			//webRedirect(out,"DCCExpert?"+location.str());
			return webRedirect(out,"DCCCommands?"+location.str());
		}


		LOG4CPLUS_DEBUG(getApplicationLogger(),"Attempting DCCTextLoad with crate(" << cgiCrate << ") dcc(" << cgiDCC << ") == slot(" << myDCC->slot() << ") command(" << command << ") XMLText(" << XMLText << ") == uploadValue(" << uploadValue << ")");


		switch (command) {

		case (1): // reset bx?
			myDCC->resetBX();
			break;

		case (2): // reset event count?
			myDCC->resetEvents();
			break;

		case (3): // set FIFO in use
			myDCC->writeFIFOInUse(uploadValue);
			break;

		case (4): // set TTC command
			myDCC->writeTTCCommand(uploadValue);
			break;

		case (5): // set fake L1A rate and number
			myDCC->writeFakeL1A(uploadValue);
			break;

		case (6): // set SW Switch
			myDCC->writeSoftwareSwitch(uploadValue);
			break;

		case (7): // set FMM register
			myDCC->writeFMM(uploadValue);
			break;

		default:

			break;

		}

		std::ostringstream backLocation;
		//backLocation << "DCCExpert?crate=" << cgiCrate << "&slot=" << cgiDCC;
		backLocation << "DCCCommands?crate=" << cgiCrate << "&slot=" << cgiDCC;
		return webRedirect(out,backLocation.str());
		
	} catch (xcept::Exception &e) {
		std::ostringstream error;
		error << "Exception caught!";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
		
		std::vector<xcept::ExceptionInformation> history = e2.getHistory();
		for (std::vector<xcept::ExceptionInformation>::iterator iError = history.begin(); iError != history.end(); iError++) {
			DataTable exceptionTable;
			unsigned int iRow = 0;
			std::map<std::string, std::string> messages = iError->getProperties();
			for (std::map<std::string, std::string>::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++) {
				exceptionTable(iRow, 0) << iMessage->first << ":";
				exceptionTable(iRow, 1) << iMessage->second;
				iRow++;
			}
			*out << cgicc::div(exceptionTable.toHTML())
				.set("class", "exception");
		}
	}
}



void emu::fed::EmuFCrateHyperDAQ::DDUVoltMon(xgi::Input *in, xgi::Output *out)
{
	
	try {
			
		// PGK Patented check-for-initialization
		if (crateVector_.size()==0) {
			LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
			return Default(in,out);
		}

		cgicc::Cgicc cgi(in);
		
		// First, I need a crate.
		std::pair<unsigned int, Crate *> cratePair = getCGICrate(cgi);
		unsigned int cgiCrate = cratePair.first;
		Crate *myCrate = cratePair.second;

		float v_val[4] = {1500,2500,2500,3300};
		float v_delt = 0.05;
		float t_val[4] = {80,80,80,80};
		float t_delt = 0.20;

		std::stringstream sTitle;
		sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DDU Voltage/Temperature Monitoring";
		std::vector<std::string> jsFileNames;
		jsFileNames.push_back("errorFlasher.js");
		*out << Header(sTitle.str(),jsFileNames);
		
		// PGK Select-a-Crate/Slot
		*out << cgicc::fieldset()
			.set("class","fieldset") << std::endl;
		*out << cgicc::div("View this page for a different crate/board")
			.set("class","legend") << std::endl;

		*out << selectACrate("DDUVoltMon","crate",cgiCrate,cgiCrate) << std::endl;

		*out << cgicc::span("Configuration located at " + xmlFile_.toString())
			.set("style","color: #A00; font-size: 10pt;") << std::endl;

		*out << cgicc::fieldset() << std::endl;
		*out << cgicc::br() << std::endl;

		// PGK Voltages/Temperatures
		*out << cgicc::fieldset()
			.set("class","fieldset") << std::endl;
		*out << cgicc::div("Voltages and Temperatures")
			.set("class","legend") << std::endl;

		// Loop through the Modules and pick out the DDUs
		for (unsigned int iDDU = 0; iDDU < myCrate->getDDUs().size(); iDDU++) {

			DDU *myDDU = myCrate->getDDUs()[iDDU];
			// I am a DDU!

			// Check Voltages
			//myCrate->getVMEController()->CAEN_err_reset();
			std::string dduClass = "ok";
			std::string totalVoltClass = "ok";
			// The actual voltage measurements
			double voltage[4] = {0,0,0,0};
			// Each voltage read gets two trials before failing.
			int voltTrial[4] = {0,0,0,0};
			std::string voltageClass[4] = {"ok","ok","ok","ok"};
			for (unsigned int iVolt = 0; iVolt < 8; iVolt++) {
				// Automatically do each voltage twice:
				unsigned int jVolt = iVolt/2;
				// Voltages are 4-7
				voltage[jVolt] = myDDU->readVoltage(jVolt);
				//if( myCrate->getVMEController()->CAEN_err() != 0) {
					//dduClass = "caution";
				//}
				if ((voltage[jVolt] > v_val[jVolt] - v_val[jVolt] * v_delt) && (voltage[jVolt] < v_val[jVolt] + v_val[jVolt] * v_delt)) {
					// Do nothing?
				} else {
					if (voltTrial[jVolt] == 1) {
						voltageClass[jVolt] = "bad";
						totalVoltClass = "error";
						dduClass = "error";
					} else {
						voltageClass[jVolt] = "caution";
						totalVoltClass = "caution";
						dduClass = "caution";
						voltTrial[jVolt]++;
					}
				}
			}

			// Check Temperatures
			//myCrate->getVMEController()->CAEN_err_reset();
			std::string totalTempClass = "ok";
			// The actual temperatures
			double temp[4] = {0,0,0,0};
			// Each temperature read gets two trials before failing.
			int tempTrial[4] = {0,0,0,0};
			std::string tempClass[4] = {"ok","ok","ok","ok"};
			for (unsigned int iTemp = 0; iTemp < 8; iTemp++) {
				// Automatically read each temperature twice
				unsigned int jTemp = iTemp/2;
				temp[jTemp] = myDDU->readTemperature(jTemp);
				//if( myCrate->getVMEController()->CAEN_err() != 0) {
					//dduClass = "caution";
				//}
				if ((temp[jTemp] > t_val[jTemp] - t_val[jTemp] * t_delt) && (temp[jTemp] < t_val[jTemp] + t_val[jTemp] * t_delt)) {
					// Do nothing?
				} else {
					if (tempTrial[jTemp] == 1) {
						tempClass[jTemp] = "bad";
						totalTempClass = "error";
						dduClass = "error";
					} else {
						tempClass[jTemp] = "caution";
						totalTempClass = "caution";
						dduClass = "caution";
						tempTrial[jTemp]++;
					}
				}
			}

			// Next, print the table with the status.
			*out << cgicc::table()
				.set("style","width: 90%; border: 2px solid #000; margin: 5px auto 5px auto; text-align: center; border-collapse: collapse;") << std::endl;

			// First row:  Voltages
			*out << cgicc::tr() << std::endl;
			// The first element is special:  board type/slot
			*out << cgicc::td()
				.set("class",dduClass)
				.set("style","border-right: 1px solid #000; font-weight: bold; width: 10%;");
			*out << "DDU Slot " << myDDU->slot();
			*out << cgicc::td() << std::endl;

			// The second element is special:  label
			*out << cgicc::td()
				.set("class",totalVoltClass)
				.set("style","border: 1px solid #000; font-weight: bold; width: 10%;");
			*out << "Voltages";
			*out << cgicc::td() << std::endl;

			// Loop over voltages
			for (unsigned int iVolt = 0; iVolt < 4; iVolt++) {
				*out << cgicc::td()
					.set("class",voltageClass[iVolt])
					.set("style","border-right: 1px solid #000; border-bottom: 1px solid #000; width: 20%;");
				*out << "V" << iVolt << ": " << std::setprecision(4) << voltage[iVolt] << "mV";
				*out << cgicc::td() << std::endl;
			}
			*out << cgicc::tr() << std::endl;

			// Second row:  Temperatures
			*out << cgicc::tr() << std::endl;
			// The first element is special:  rui
			*out << cgicc::td()
				.set("class",dduClass)
				.set("style","border-right: 1px solid #000; font-weight: bold; width: 10%;");
			*out << "RUI #" << myCrate->getRUI(myDDU->slot());
			*out << cgicc::td() << std::endl;

			// The second element is special:  label
			*out << cgicc::td()
				.set("class",totalTempClass)
				.set("style","border-right: 1px solid #000; font-weight: bold; width: 10%;");
			*out << "Temperatures";
			*out << cgicc::td() << std::endl;

			// Loop over temperatures
			for (unsigned int iTemp = 0; iTemp < 4; iTemp++) {
				*out << cgicc::td()
					.set("class",tempClass[iTemp])
					.set("style","border-right: 1px solid #000; border-bottom: 1px solid #000; width: 20%;");
				*out << "T" << iTemp << ": " << std::setprecision(4) << temp[iTemp] << "&deg;F";
				*out << cgicc::td() << std::endl;
			}
			*out << cgicc::tr() << std::endl;
			*out << cgicc::table() << std::endl;

		}

		*out << cgicc::fieldset() << std::endl;

		*out << Footer();
		
	} catch (xcept::Exception &e) {
		std::ostringstream error;
		error << "Exception caught!";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
		
		std::vector<xcept::ExceptionInformation> history = e2.getHistory();
		for (std::vector<xcept::ExceptionInformation>::iterator iError = history.begin(); iError != history.end(); iError++) {
			DataTable exceptionTable;
			unsigned int iRow = 0;
			std::map<std::string, std::string> messages = iError->getProperties();
			for (std::map<std::string, std::string>::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++) {
				exceptionTable(iRow, 0) << iMessage->first << ":";
				exceptionTable(iRow, 1) << iMessage->second;
				iRow++;
			}
			*out << cgicc::div(exceptionTable.toHTML())
				.set("class", "exception");
		}
	}
}



std::string emu::fed::EmuFCrateHyperDAQ::selectACrate(std::string location, std::string what, unsigned int index, unsigned int crateIndex)
{
	std::ostringstream *out = new std::ostringstream();
	
	for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
		*out << cgicc::table()
			.set("style","width: 100%; border: 1px solid #000; border-collapse: collapse; text-align: center; font-size: 10pt; margin-bottom: 10px;") << std::endl;
		
		std::ostringstream columns;
		columns << ((*iCrate)->getDDUs().size() + (*iCrate)->getDCCs().size());

		std::string selectedCrate = "";
		if (what == "crate" && index == (*iCrate)->number()) selectedCrate = "background-color: #FF9;";
		
		std::ostringstream crateName;
		crateName << "Crate " << (*iCrate)->number();
		
		// For crate-specific pages, link the crates.
		*out << cgicc::tr() << std::endl;
		*out << cgicc::td()
			.set("style",selectedCrate+" border: 1px solid #000;")
			.set("colspan",columns.str()) << std::endl;

		std::ostringstream locationCrate;
		if (what == "crate") {
			locationCrate << "/" << getApplicationDescriptor()->getURN() << "/" << location << "?crate=" << (*iCrate)->number();
			*out << cgicc::a(crateName.str())
				.set("href",locationCrate.str()) << std::endl;
		} else {
			*out << crateName.str() << std::endl;
		}
		*out << cgicc::td() << std::endl;
		*out << cgicc::tr() << std::endl;

		// For board-specific pages, link the board.
		*out << cgicc::tr() << std::endl;
		std::vector<DDU *> myDDUs = (*iCrate)->getDDUs();
		for (std::vector<DDU *>::iterator iDDU = myDDUs.begin(); iDDU != myDDUs.end(); iDDU++) {

			std::string selectedBoard = "";
			if (what == "ddu" && index == (*iDDU)->slot() && crateIndex == (*iCrate)->number()) selectedBoard = "background-color: #FF9;";
			
			std::ostringstream boardName;
			boardName << "DDU Slot " << (*iDDU)->slot() << ": RUI #" << (*iCrate)->getRUI((*iDDU)->slot());

			*out << cgicc::td()
				.set("style",selectedBoard+" border: 1px solid #000;") << std::endl;

			std::ostringstream locationBoard;
			if (what == "ddu") {
				locationBoard << "/" << getApplicationDescriptor()->getURN() << "/" << location << "?crate=" << (*iCrate)->number() << "&slot=" << (*iDDU)->slot();
				*out << cgicc::a(boardName.str())
					.set("href",locationBoard.str()) << std::endl;
			} else {
				*out << boardName.str() << std::endl;
			}
			*out << cgicc::td() << std::endl;
		}
		
		std::vector<DCC *> myDCCs = (*iCrate)->getDCCs();
		for (std::vector<DCC *>::iterator iDCC = myDCCs.begin(); iDCC != myDCCs.end(); iDCC++) {

			std::string selectedBoard = "";
			if (what == "dcc" && index == (*iDCC)->slot() && crateIndex == (*iCrate)->number()) selectedBoard = "background-color: #FF9;";
			
			std::ostringstream boardName;
			boardName << "DCC Slot " << (*iDCC)->slot();
			
			*out << cgicc::td()
				.set("style",selectedBoard+" border: 1px solid #000;") << std::endl;

			std::ostringstream locationBoard;
			if (what == "dcc") {
				locationBoard << "/" << getApplicationDescriptor()->getURN() << "/" << location << "?crate=" << (*iCrate)->number() << "&slot=" << (*iDCC)->slot();
				*out << cgicc::a(boardName.str())
					.set("href",locationBoard.str()) << std::endl;
			} else {
				*out << boardName.str() << std::endl;
			}
			*out << cgicc::td() << std::endl;
		}
		*out << cgicc::tr() << std::endl;
		*out << cgicc::table() << std::endl;
	}
	
	return out->str();
}



std::pair<unsigned int, emu::fed::Crate *> emu::fed::EmuFCrateHyperDAQ::getCGICrate(cgicc::Cgicc cgi)
throw (emu::fed::exception::OutOfBoundsException)
{

	// get the crate number
	cgicc::form_iterator name = cgi.getElement("crate");
	unsigned int cgiCrate = 0;
	if (name != cgi.getElements().end()) {
		cgiCrate = cgi["crate"]->getIntegerValue();
	} else {
		std::ostringstream error;
		error << "Error getting crate from CGI input";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_RAISE(emu::fed::exception::OutOfBoundsException, error.str());
	}

	// Find the crate
	for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
		if ((*iCrate)->number() == cgiCrate) return std::pair<unsigned int, Crate *> (cgiCrate, (*iCrate));
	}

	std::ostringstream error;
	error << "Unable to find a crate with number " << cgiCrate;
	LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
	XCEPT_RAISE(emu::fed::exception::OutOfBoundsException, error.str());

}



