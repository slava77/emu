
<<<<<<< EmuFCrateHyperDAQ.cc
#include "EmuFCrateHyperDAQ.h"

#include "DDUDebugger.h"
#include "DataTable.h"

#include <bitset>
#include <fstream>

XDAQ_INSTANTIATOR_IMPL(EmuFCrateHyperDAQ)


EmuFCrateHyperDAQ::EmuFCrateHyperDAQ(xdaq::ApplicationStub * s):
	LocalEmuApplication(s),
	interrupt_set(false),
	xmlFile_("/home/cscdev/TriDAS/emu/emuDCS/FEDCrate/xml/config.xml"),
	Operator_("Name..."),
	DCC_ratemon_cnt(0),
	fcState_(STATE_UNKNOWN)
{
	xgi::bind(this,&EmuFCrateHyperDAQ::Default, "Default");
	xgi::bind(this,&EmuFCrateHyperDAQ::mainPage, "mainPage");
	xgi::bind(this,&EmuFCrateHyperDAQ::configurePage, "configurePage");
	xgi::bind(this,&EmuFCrateHyperDAQ::setConfFile, "setConfFile");
	xgi::bind(this,&EmuFCrateHyperDAQ::setRawConfFile, "setRawConfFile");
	xgi::bind(this,&EmuFCrateHyperDAQ::UploadConfFile, "UploadConfFile");
	xgi::bind(this,&EmuFCrateHyperDAQ::DDUFirmware, "DDUFirmware");
	xgi::bind(this,&EmuFCrateHyperDAQ::DDUBroadcast, "DDUBroadcast");
	xgi::bind(this,&EmuFCrateHyperDAQ::DDULoadBroadcast, "DDULoadBroadcast");
	xgi::bind(this,&EmuFCrateHyperDAQ::DDUSendBroadcast, "DDUSendBroadcast");
	xgi::bind(this,&EmuFCrateHyperDAQ::DDUReset, "DDUReset");
	xgi::bind(this,&EmuFCrateHyperDAQ::DDUBrcstFED, "DDUBrcstFED");

	xgi::bind(this,&EmuFCrateHyperDAQ::DDUFpga, "DDUFpga");
	xgi::bind(this,&EmuFCrateHyperDAQ::InFpga, "InFpga");
	xgi::bind(this,&EmuFCrateHyperDAQ::VMEPARA, "VMEPARA");
	xgi::bind(this,&EmuFCrateHyperDAQ::VMESERI, "VMESERI");
	xgi::bind(this,&EmuFCrateHyperDAQ::DDUTextLoad, "DDUTextLoad");
	xgi::bind(this,&EmuFCrateHyperDAQ::DDULoadFirmware,"DDULoadFirmware");
	//xgi::bind(this,&EmuFCrateHyperDAQ::DDUtrapDecode,"DDUtrapDecode");
	xgi::bind(this,&EmuFCrateHyperDAQ::LoadXMLconf,"LoadXMLconf");
	xgi::bind(this,&EmuFCrateHyperDAQ::DCCFirmware,"DCCFirmware");
	xgi::bind(this,&EmuFCrateHyperDAQ::DCCLoadFirmware,"DCCLoadFirmware");
	xgi::bind(this,&EmuFCrateHyperDAQ::DCCFirmwareReset,"DCCFirmwareReset");
	xgi::bind(this,&EmuFCrateHyperDAQ::DCCCommands,"DCCCommands");
	xgi::bind(this,&EmuFCrateHyperDAQ::DCCTextLoad, "DCCTextLoad");
	xgi::bind(this,&EmuFCrateHyperDAQ::IRQTester,"IRQTester");
	xgi::bind(this,&EmuFCrateHyperDAQ::DDUVoltMon,"DDUVoltMon");
	xgi::bind(this,&EmuFCrateHyperDAQ::DCCRateMon,"DCCRateMon");
	xgi::bind(this,&EmuFCrateHyperDAQ::getDataDCCRate0,"getDataDCCRate0");
	xgi::bind(this,&EmuFCrateHyperDAQ::getDataDCCRate1,"getDataDCCRate1");
	xgi::bind(this,&EmuFCrateHyperDAQ::setCrate,"setCrate");
	myParameter_ =  0;

	for (int i=0; i<9; i++) { DDUBoardID_[i] = "-1" ; DCCBoardID_[i] = "-1" ; }
	getApplicationInfoSpace()->fireItemAvailable("xmlFileName",&xmlFile_);
	for(int i=0;i<12;i++){
		for(int j=0;j<50;j++){
			DCC_ratemon[j][i]=i+1;
		}
	}
	tidcode[0] = 0x2124a093;
	tidcode[1] = 0x31266093;
	tidcode[2] = 0x31266093;
	tidcode[3] = 0x05036093;
	tidcode[4] = 0x05036093;
	tidcode[5] = 0x05036093;
	tidcode[6] = 0x05036093;
	tidcode[7] = 0x05036093;
	tuscode[0] = 0xcf043a02;
	tuscode[1] = 0xdf025a02;
	tuscode[2] = 0xdf025a02;
	tuscode[3] = 0xb0020a04;
	tuscode[4] = 0xc043dd99;
	tuscode[5] = 0xc143dd99;
	tuscode[6] = 0xd0025a02;
	tuscode[7] = 0xd1025a02;

	// Move the pictures to tmp for display
	std::vector< std::string > picNames;
	picNames.push_back("osu_fed_background.png");
	picNames.push_back("osu_crate.png");
	picNames.push_back("osu_emu.png");
	for (std::vector< std::string >::iterator iName = picNames.begin(); iName != picNames.end(); iName++) {
		std::ifstream picIn;
		picIn.open((*iName).c_str(),std::ios_base::binary);
		if (picIn.is_open()) {
			std::ofstream picOut;
			string newName = "/tmp/" + *iName;
			picOut.open(newName.c_str(),std::ios_base::binary);
			picOut << picIn.rdbuf();
			picOut.close();
			picIn.close();
		}
	}

}




void EmuFCrateHyperDAQ::Default(xgi::Input *in, xgi::Output *out )
	throw (xgi::exception::Exception)
{

	//cout << "--Entering Default" << endl;
/* JRG, Logger stuff to add:
    // if (getApplicationLogger().exists(getApplicationLogger().getName())) {
*/
	//LOG4CPLUS_INFO(getApplicationLogger(), " EmuFEDVME: server startup" << endl);
	//string LoggerName = getApplicationLogger().getName();
	//cout << "Name of Logger is " <<  LoggerName <<endl;

	if (dduVector.size()==0 && dccVector.size()==0) {
		cout << "Empty vectors.  Initializing." << endl;
		Configuring(); // Defaults should be sufficient.
		cout << "SOAP:  Querying FCrate." << endl;
		queryFCrate();
		cout << "SOAP:  FCrate instance " << getApplicationDescriptor()->getInstance() << " returned state \"" << fcState_ << "\"" << endl;
		if (fcState_ == "Halted" || fcState_ == "Failed" || fcState_ == STATE_UNKNOWN) {
			cout << "FCrate reports crates are not configured.  Configuring." << endl;
			crateVector[0]->configure(0);
		} else {
			cout << "FCrate reports crates are configured.  Skipping configuring." << endl;
		}
	}

	webRedirect(out,"mainPage");
	//mainPage(in,out);

}



void EmuFCrateHyperDAQ::mainPage(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception)
{

	//cout << "--Entering mainPage" << endl;

	ostringstream sTitle;
	sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ")";
	*out << Header(sTitle.str(),false);

	try {

		// Check for errors in crates.  Should only have crate numbers 1-5,
		// and there should not be more than one crate with a given number.
		// The crates should also have a unique VME controller.
		int crateerror = 0;
		for (unsigned int icr=0; icr<crateVector.size(); icr++) {
			if (crateVector[icr]->number() > 5 || crateVector[icr]->number() < 1) {
				*out << cgicc::div().set("style","background-color: #000; color: #FAA; font-weight: bold; margin-bottom: 0px;") << "Crate " << crateVector[icr]->number() << " has an invalid number (should be 1-5)" << cgicc::div() << endl;
				crateerror++;
			}
			for (unsigned int jcr=icr+1; jcr<crateVector.size(); jcr++) {
				if (crateVector[icr]->number() == crateVector[jcr]->number()) {
					*out << cgicc::div().set("style","background-color: #000; color: #FAA; font-weight: bold; margin-bottom: 0px;") << "Two crates share crate number " << crateVector[icr]->number() << cgicc::div() << endl;
					crateerror++;
				}
				if (crateVector[icr]->vmeController()->Device() == crateVector[jcr]->vmeController()->Device()) {
					*out << cgicc::div().set("style","background-color: #000; color: #FAA; font-weight: bold; margin-bottom: 0px;") << "Crates " << crateVector[icr]->number() << " and " << crateVector[jcr]->number() << " have the same VME controller device number (" << crateVector[icr]->vmeController()->Device() << ")" << cgicc::div() << endl;
					crateerror++;
				}
			}
		}
		if (crateerror) {
			*out << cgicc::div().set("style","background-color: #000; color: #FAA; font-weight: bold; margin-bottom: 0px;") << "You have " << crateerror << " error" << (crateerror != 1 ? "s" : "") << " in your XML configuration file.  MAKE SURE YOU UNDERSTAND WHAT YOU ARE DOING BEFORE CONTINUING WITH THIS CONFIGURATION.  If you did not expect this message, fix your configuration file and reload it with the button at the bottom of the page." << cgicc::div() << endl;
		}

		// PGK The Reload button is not really useful anymore.
		/*
		string reload = toolbox::toString("/%s",getApplicationDescriptor()->getURN().c_str());
		*out << cgicc::input()
			.set("type","button")
			.set("value","Reload Page")
			.set("onClick","window.location.href='"+reload+"'") << endl; // Javascript trick
		*out << cgicc::br() << endl;
		*/

		// PGK Select-a-Crate
		*out << cgicc::fieldset()
			.set("class","fieldset") << endl;
		*out << cgicc::div("Crate Selection")
			.set("class","legend") << endl;

		*out << cgicc::div()
			.set("style","background-color: #FF0; color: #00A; font-size: 16pt; font-weight: bold; width: 50%; text-align: center; padding: 3px; border: 2px solid #00A; margin: 5px auto 5px auto;");
		*out << "Crate " << thisCrate->number() << " Selected";
		*out << cgicc::div() << endl;

		// PGK The Select-a-Crate only shows up if there is more than 1 crate.
		if (crateVector.size() > 1) {

			string crateform = toolbox::toString("/%s/setCrate",getApplicationDescriptor()->getURN().c_str());
			*out << form()
				.set("method","GET")
				.set("action",crateform) << endl;

			vector<Crate *>::iterator iCrate;
			unsigned int icr = 0;
			for (iCrate = crateVector.begin(); iCrate != crateVector.end(); iCrate++) {
				*out << "<input type=\"radio\" name=\"icrate\" value=\"" << icr << "\" ";
				if (thisCrate->number() == (*iCrate)->number()) *out << "checked=\"checked\" ";
				*out << "/>Crate " << (*iCrate)->number() << "<br />" << endl;
				icr++;
			}
			*out << input().set("type","submit")
				.set("value","Change Crate") << endl;
			*out << form() << endl;

		} else {
			*out << "Only one crate is defined in the configuration XML." << cgicc::br() << endl;
		}

		*out << cgicc::span("Configuration located at " + xmlFile_.toString())
			.set("style","color: #A00; font-size: 10pt;") << endl;

		*out << cgicc::fieldset() << endl;
		*out << br() << endl;

		// PGK Crate monitoring tools
		//DDU_=-99; // ?
		*out << cgicc::fieldset()
			.set("class","fieldset") << endl;
		*out << cgicc::div("Crate Monitoring Tools")
			.set("class","legend") << endl;

		string dduvoltmon = "/" + getApplicationDescriptor()->getURN() + "/DDUVoltMon";
		*out << cgicc::form()
			.set("method","GET")
			.set("action",dduvoltmon)
			.set("target","_blank") << endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("value","Start Volt/Temp Monitor") << endl;
		*out << cgicc::form() << endl;

		string dccratemon = "/" + getApplicationDescriptor()->getURN() + "/DCCRateMon";
		*out << cgicc::form()
			.set("method","GET")
			.set("action",dccratemon)
			.set("target","_blank") << endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("value","Start DCC Rate Monitor") << endl;
		*out << cgicc::form() << endl;

		*out << cgicc::fieldset() << endl;

		// Table sorted by slot...
		*out << cgicc::fieldset()
			.set("class","fieldset") << endl;
		*out << cgicc::div("Board Communication")
			.set("class","legend");
		//cout << " dduVector.size() " << dduVector.size() << endl;

		// PGK CAEN gets in a funny state after doing crate switching.  It doesn't seem to
		//  cause any problems, but it does fail to read anything useful from the first
		//  ddu in dduVector after performing a certain series of reads and crate switches.
		//  If you communicate to any other slot after switching to the new crate, the problem
		//  never shows up.  This is what I'm doing here--communicate to the DCC if it exists.
		//  If there is no DCC, then you'll just have to live with the error, I guess.

		
		if (dccVector.size()) {
			//cout << " pinging DCC to avoid CAEN read error -1" << endl;
			dccVector[0]->mctrl_stath();
		}
		

		// Unfortunately, the DDU routines are called with the index of the
		//  given DDU in the DDU vector, not the module vector.
		//  This will help us keep track of what DDU we are on.
		unsigned int iddu = 0;
		// Same goes for DCCs (when we get more than 1)
		unsigned int idcc = 0;

		// Loop over all the DDUs
		vector<DDU *>::iterator iDDU;
		for (iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
			thisDDU = (*iDDU);
			// Determine if we are working on a DDU or a DCC by module type
			//thisDDU = dynamic_cast<DDU *>(moduleVector[iModule]);
			//thisDCC = dynamic_cast<DCC *>(moduleVector[iModule]);

			// Skip broadcasting
			if (thisDDU->slot() > 21) continue;

			// First, determine the status of the DDU.
			thisCrate->vmeController()->CAEN_err_reset();
			// Do a fast FMM status check
			//unsigned short int DDU_FMM = ((thisDDU->vmepara_status()>>8)&0x000F);
			unsigned short int DDU_FMM = ((thisDDU->readParallelStat()>>8)&0x000F);
			//unsigned short int DDU_FMM = 8; // DEBUG
			// Mark the status with pretty colors
			string fmmClass = "green";
			string statusClass = "ok";
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
			//unsigned short int status = thisDDU->vmepara_CSCstat();
			unsigned short int status = thisDDU->readCSCStat();
			//unsigned short int status = 0; // DEBUG
			// Mark the status with pretty colors
			string cscClass = "green";
			if (thisCrate->vmeController()->CAEN_err()!=0) {
				cscClass = "yellow";
			} else if (status==0x0000) {
				cscClass = "green";
			} else {
				cscClass = "red";
				statusClass = "error";
			}

			// Next, print the table with the status.
			*out << cgicc::table()
				.set("style","width: 90%; border: 2px solid #000; margin: 5px auto 5px auto; text-align: center; border-collapse: collapse;") << endl;

			// First row:  status line
			*out << cgicc::tr() << endl;
			// The first element is special:  board type
			*out << cgicc::td()
				.set("class",statusClass)
				.set("style","border-right: 1px solid #000; font-weight: bold; width: 10%;");
			*out << "DDU";
			*out << cgicc::td() << endl;
			*out << cgicc::td()
				.set("class",fmmClass)
				.set("style","border-bottom: 1px solid #000;")
				.set("colspan","7");
			*out << "FMM Status: " << uppercase << setw(1) << hex << DDU_FMM << dec << "h";
			*out << cgicc::td() << endl;
			*out << cgicc::td()
				.set("class",cscClass)
				.set("style","border-bottom: 1px solid #000;")
				.set("colspan","8");
			*out << "CSC Status: " << uppercase << setw(4) << hex << status << dec << "h";
			*out << cgicc::td() << endl;
			*out << cgicc::tr() << endl;

			// Second row:  chambers
			*out << cgicc::tr()
				.set("style","") << endl;
			// The first element is special:  slot
			*out << cgicc::td()
				.set("class",statusClass)
				.set("style","border-right: 1px solid #000; font-weight: bold; width: 10%;");
			*out << "Slot " << thisDDU->slot();
			*out << cgicc::td() << endl;

			// Loop through the chambers.  They should be in fiber-order.
			for (unsigned int iFiber=0; iFiber<15; iFiber++) {
				Chamber *thisChamber = thisDDU->getChamber(iFiber);
				// DDU::getChamber will return a null pointer if there is
				//  no chamber at that fiber position.
				// Note:  there are no 4/2 chambers yet...
				if (thisChamber != 0 && (thisChamber->station < 4 || thisChamber->type < 2)) {
					// Highlight chambers with problems.
					string chamberClass = "ok";
					if (status & (1<<iFiber)) chamberClass = "bad";
					*out << cgicc::td(thisChamber->name())
						.set("class",chamberClass)
						.set("style","border: 1px solid #000; font-size: 8pt; width: 6%;") << endl;
				} else {
					*out << cgicc::td(thisChamber->name())
						.set("class","undefined")
						.set("style","border: 1px solid #000; font-size: 8pt; width: 6%;") << endl;
				}
			}

			*out << cgicc::tr() << endl;

			// Third row:  commands
			*out << cgicc::tr()
				.set("style","") << endl;
			// The first element is special:  RUI
			stringstream ruiNumberStream;
			ruiNumberStream << thisCrate->getRUI(thisDDU->slot());
			string ruiString = ruiNumberStream.str();
			// This part is terrible.
			string ruiFormGetString = "rui1="+ruiString+"&ddu_input1=&ddu1=&fed_crate1=&ddu_slot1=&dcc_fifo1=&slink1=&fiber_crate1=&fiber_pos1=&fiber_socket1=&crateid1=&cratelabel1=&dmb_slot1=&chamberlabel1=&chamberid1=&rui2=&ddu2=&fed_crate2=&ddu_slot2=&ddu_input2=&dcc_fifo2=&slink2=&fiber_crate2=&fiber_pos2=&fiber_socket2=&crateid2=&cratelabel2=&dmb_slot2=&chamberlabel2=&chamberid2=&switch=ddu_chamber&chamber2=";
			*out << cgicc::td()
				.set("class",statusClass)
				.set("style","border-right: 1px solid #000; font-weight: bold; width: 10%;");
			*out << cgicc::a("RUI #"+ruiString)
				.set("href","http://oraweb03.cern.ch:9000/pls/cms_emu_cern.pro/ddumap.web?"+ruiFormGetString);
			*out << cgicc::td() << endl;

			// Everything else is one big row.
			*out << cgicc::td()
				.set("colspan","15") << endl;

			// Create the buttons with a big loop.
			string appString[5] = {
				"/" + getApplicationDescriptor()->getURN() + "/DDUFirmware",
				"/" + getApplicationDescriptor()->getURN() + "/DDUFpga",
				"/" + getApplicationDescriptor()->getURN() + "/InFpga",
				"/" + getApplicationDescriptor()->getURN() + "/VMEPARA",
				"/" + getApplicationDescriptor()->getURN() + "/VMESERI",
			};

			string appName[5] = {
				"Firmware",
				"dduFPGA",
				"inFPGA 0 and 1",
				"VME Parallel",
				"VME Serial"
			};

			for (unsigned int iButton = 0; iButton < 5; iButton++) {
				// Jason likes a space after the first button.
				if (iButton==0) *out << cgicc::span().set("style","margin-right: 50px;") << endl;
				else *out << cgicc::span() << endl;
				*out << cgicc::form().set("style","display: inline;")
					.set("method","GET")
					.set("action",appString[iButton])
					.set("target","_blank") << endl;
				*out << cgicc::input()
					.set("type","submit")
					.set("value",appName[iButton]) << endl;
				ostringstream dduNumber;
				dduNumber << iddu;
				*out << cgicc::input()
					.set("type","hidden")
					.set("value",dduNumber.str())
					.set("name","ddu") << endl;
				*out << cgicc::form() << endl;

				*out << cgicc::span() << endl;
			}

			*out << cgicc::td() << endl;
			*out << cgicc::tr() << endl;

			*out << cgicc::table();

			// The DDU counter
			iddu++;
		}
		
		// Loop over all the DCCs
		vector<DCC *>::iterator iDCC;
		for (iDCC = dccVector.begin(); iDCC != dccVector.end(); iDCC++) {
			thisDCC = (*iDCC);

			// Skip broadcasting
			if (thisDCC->slot() > 21) continue;

			// First, determine the status of the DCC.
			thisCrate->vmeController()->CAEN_err_reset();
			unsigned short int statush=thisDCC->mctrl_stath();
			unsigned short int statusl=thisDCC->mctrl_statl();
			unsigned short int rdfifoinuse=thisDCC->mctrl_rd_fifoinuse();
			string status;
			// Pretty colors!
			string statusClass = "ok";
			string dccStatus = "green";
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
				.set("style","width: 90%; border: 3px double #000; margin: 5px auto 5px auto; text-align: center; border-collapse: collapse;") << endl;

			// First row:  status line
			*out << cgicc::tr() << endl;
			// The first element is special:  board type
			*out << cgicc::td()
				.set("class",statusClass)
				.set("style","border-right: 1px solid #000; font-weight: bold; width: 10%;");
			*out << "DCC";
			*out << cgicc::td() << endl;
			*out << cgicc::td()
				.set("class",dccStatus)
				.set("style","border-bottom: 1px solid #000; width: 18%;");
			*out << "FMM: " << uppercase << setw(1) << hex << ((statush>>12)&0xf) << dec << "h " << status;
			*out << cgicc::td() << endl;
			*out << cgicc::td()
				.set("class",dccStatus)
				.set("style","border-bottom: 1px solid #000; width: 18%;");
			*out << "SLink Status: " << uppercase << setw(1) << hex << (statush&0xf) << dec << "h";
			*out << cgicc::td() << endl;
			*out << cgicc::td()
				.set("class",dccStatus)
				.set("style","border-bottom: 1px solid #000; width: 18%;");
			*out << "InFIFO full: " << uppercase << setw(2) << hex << ((statush>>4)&0xff) << dec << "h";
			*out << cgicc::td() << endl;
			*out << cgicc::td()
				.set("class",dccStatus)
				.set("style","border-bottom: 1px solid #000; width: 18%;");
			*out << "L1As: " << uppercase << setw(4) << hex << (statusl&0xffff) << dec << "h";
			*out << cgicc::td() << endl;
			*out << cgicc::td()
				.set("class",dccStatus)
				.set("style","border-bottom: 1px solid #000; width: 18%;");
			*out << "FIFO in use: " << uppercase << setw(4) << hex << (rdfifoinuse&0x3ff) << dec << "h";
			*out << cgicc::td() << endl;

			*out << cgicc::tr() << endl;

			// Second row:  commands
			*out << cgicc::tr()
				.set("style","") << endl;
			// The first element is special:  slot
			*out << cgicc::td()
				.set("class",statusClass)
				.set("style","border-right: 1px solid #000; font-weight: bold; width: 10%;");
			*out << "Slot " << thisDCC->slot();
			*out << cgicc::td() << endl;

			// Everything else is one big row.
			*out << cgicc::td()
				.set("colspan","5") << endl;

			// Create the buttons with a big loop.
			string appString[2] = {
				"/" + getApplicationDescriptor()->getURN() + "/DCCFirmware",
				"/" + getApplicationDescriptor()->getURN() + "/DCCCommands"
			};

			string appName[2] = {
				"Firmware",
				"Commands"
			};

			for (unsigned int iButton = 0; iButton < 2; iButton++) {
				// Jason likes a space after the first button.
				if (iButton==0) *out << cgicc::span().set("style","margin-right: 50px;") << endl;
				else *out << cgicc::span() << endl;
				*out << cgicc::form().set("style","display: inline;")
					.set("method","GET")
					.set("action",appString[iButton])
					.set("target","_blank") << endl;
				*out << cgicc::input()
					.set("type","submit")
					.set("value",appName[iButton]) << endl;
				ostringstream dccNumber;
				dccNumber << idcc;
				*out << cgicc::input()
					.set("type","hidden")
					.set("value",dccNumber.str())
					.set("name","dcc") << endl;
				*out << cgicc::form() << endl;

				*out << cgicc::span() << endl;
			}

			*out << cgicc::td() << endl;
			*out << cgicc::tr() << endl;

			*out << cgicc::table();

			idcc++;

		} // end VME Module loop.

		*out << endl;
		*out << cgicc::fieldset() << endl;

		// Broadcast table...
		*out << cgicc::fieldset()
			.set("class","fieldset") << endl;
		*out << cgicc::div("Crate Broadcast")
			.set("class","legend");

		// DDU buttons.
		// Skip if there is only 1 ddu
		if (dduVector.size() > 1) {
			// Skip if you can't find a broadcast DDU in the XML...
			vector<DDU *>::iterator iDDU;
			for (iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
				if ((*iDDU)->slot() <= 21) continue;

				thisDDU = (*iDDU);

				// Broadcast Firmware
				*out << cgicc::span() << endl;
				*out << cgicc::form()
					.set("method","GET")
					.set("action","/" + getApplicationDescriptor()->getURN() + "/DDUBroadcast")
					.set("target","_blank") << endl;
				*out << cgicc::input()
					.set("type","submit")
					.set("value","Broadcast DDU Firmware")
					.set("style","background-color: #FDD; border-color: #F00;") << endl;
				*out << cgicc::form() << endl;
				*out << cgicc::span() << endl;

				// JRG, add DDU Broadcast for FMM Error-report Disable function.
				*out << cgicc::span() << endl;
				*out << cgicc::form()
					.set("method","GET")
					.set("action","/" + getApplicationDescriptor()->getURN() + "/DDUBrcstFED") << endl;
				*out << cgicc::input()
					.set("type","submit")
					.set("value","DDU FMM Error-report Disable") << endl;
				*out << cgicc::form() << endl;
				*out << cgicc::span() << endl;

			}
		}

		// DCC button.
		// Skip if there is only 1 dcc
		if (dccVector.size() > 1) {
			*out << cgicc::br() << endl;
			// Skip if you can't find a broadcast DCC in the XML...
			vector<DCC *>::iterator iDCC;
			for (iDCC = dccVector.begin(); iDCC != dccVector.end(); iDCC++) {
				if ((*iDCC)->slot() <= 21) continue;

				thisDCC = (*iDCC);

				// Broadcast Firmware
				*out << cgicc::span() << endl;
				*out << cgicc::form().set("style","display: inline;")
					.set("method","GET")
					.set("action","/" + getApplicationDescriptor()->getURN() + "/DCCFirmware")
					.set("target","_blank") << endl;
				*out << cgicc::input()
					.set("type","submit")
					.set("value","Broadcast DCC Firmware")
					.set("style","background-color: #FDD; border-color: #F00;") << endl;
				*out << cgicc::form() << endl;
				*out << cgicc::span() << endl;

			}
		}

		*out << cgicc::fieldset() << endl;

		*out << cgicc::br() << endl;

		// PGK Useful information for users.
		*out << cgicc::fieldset()
			.set("class","fieldset") << endl;
		*out << cgicc::div("Useful Links")
			.set("class","legend") << endl;

		*out << cgicc::ul() << endl;
		*out << cgicc::li() << endl;
		*out << cgicc::a("DDU Documentation")
			.set("href","http://www.physics.ohio-state.edu/~cms/ddu") << endl;
		*out << cgicc::li() << endl;
		*out << cgicc::li() << endl;
		*out << cgicc::a("DCC Documentation")
			.set("href","http://www.physics.ohio-state.edu/~cms/dcc") << endl;
		*out << cgicc::li() << endl;
		*out << cgicc::li() << endl;
		*out << cgicc::a("FED Crate Documentation")
			.set("href","http://www.physics.ohio-state.edu/~cms/fed") << endl;
		*out << cgicc::li() << endl;
		*out << cgicc::li() << endl;
		*out << cgicc::a("OSU Firmware Repository")
			.set("href","http://www.physics.ohio-state.edu/~cms/firmwares") << endl;
		*out << cgicc::li() << endl;
		*out << cgicc::ul() << endl;

		*out << cgicc::fieldset() << endl;

		*out << cgicc::br() << endl;
		// JRG, inside end of "normal" DDU/DCC Control top-level page
		//   --add "Go to XML Reload" button here for Martin:
		//*out << "<blockquote> &nbsp; &nbsp; </blockquote>" << endl;
		//*out << "<P> &nbsp; <P> &nbsp; <P> &nbsp; <P> &nbsp; <P> &nbsp;" << endl;
		*out << hr();
		//string loadxmlconf =  toolbox::toString("/%s/LoadXMLconf",getApplicationDescriptor()->getURN().c_str());
		string loadxmlconf = "/" + getApplicationDescriptor()->getURN() + "/configurePage";

		*out << cgicc::fieldset()
			.set("class","fieldset")
			.set("style","background-color: #000; color: #FFF;") << endl;
		*out << cgicc::div("Experts Only")
			.set("class","legend")
			.set("style","color: #F99;") << endl;
		if (fcState_ == "Halted" || fcState_ == STATE_UNKNOWN) {
			*out << cgicc::form().set("style","display: inline;")
				.set("method","POST")
				.set("action",loadxmlconf) << endl;
			*out << cgicc::input().set("type","hidden")
				.set("name","Reload")
				.set("value","1");
			*out << endl;
			*out << cgicc::input()
				.set("type","submit")
				.set("value","Reload XMLconfig")
				.set("style","background-color: #FDD; border-color: #F00;") << endl;
			*out << cgicc::form();
		} else {
			*out << "Change state to \"Halted\" before trying to change the configuration via HyperDAQ." << endl;
		}

		*out << cgicc::fieldset() << endl;

		*out << Footer();

	} catch (const exception & e ) {
		//XECPT_RAISE(xgi::exception::Exception, e.what());
	}
}



void EmuFCrateHyperDAQ::configurePage(xgi::Input *in, xgi::Output *out )
	throw (xgi::exception::Exception)
{
	cout << "--Entering Configure" << endl;

	string method;

	//Title(out,"Configure FED Crate HyperDAQ");

	try {

		/* PGK I hate fieldsets. */
		*out << cgicc::div("Choose one of the following configuration methods").set("class","title") << endl;

		/* Use a local file */
		method =
		toolbox::toString("/%s/setConfFile",getApplicationDescriptor()->getURN().c_str());

		*out << cgicc::fieldset()
			.set("class","fieldset") << endl;
		*out << cgicc::div("Use a file on the server").set("class","legend");

		*out << cgicc::form()
			.set("method","POST")
			.set("action",method) << endl;
		*out << cgicc::span("Absolute path on server: ") << endl;
		*out << cgicc::input().set("type","text")
			.set("name","xmlFilename")
			.set("size","60")
			.set("ENCTYPE","multipart/form-data")
			.set("value",xmlFile_) << endl;
		*out << cgicc::input().set("type","submit").set("name","buttonid")
			.set("value","Initialize and Configure") << endl;
		*out << cgicc::form() << endl;
		*out << cgicc::div("Note: FCrate Configuration always supersedes HyperDAQ Configuration.")
			.set("style","width: 100%; color: #FF0000; font-size: 11pt; font-style: italic; text-align: right;") << endl;

		*out << cgicc::fieldset() << endl;

		/* Upload file */
		method =
		toolbox::toString("/%s/UploadConfFile",getApplicationDescriptor()->getURN().c_str());

		*out << cgicc::fieldset()
			.set("class","fieldset") << endl;
		*out << cgicc::div("Upload a local file")
			.set("class","legend");

		*out << cgicc::form().set("method","POST")
			.set("enctype","multipart/form-data")
			.set("action",method) << endl;
		*out << cgicc::span("Local path (use [Browse] button): ") << endl;
		*out << cgicc::input().set("type","file")
			.set("name","xmlFilenameUpload")
			.set("size","60") << endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("value","Initialize and Configure") << endl;
		*out << cgicc::form() << endl;

		*out << cgicc::fieldset() << endl;

		/* Roll your own */
		method =
		toolbox::toString("/%s/setRawConfFile",getApplicationDescriptor()->getURN().c_str());

		*out << cgicc::fieldset()
			.set("class","fieldset") << endl;
		*out << cgicc::div("Type a custon configuration")
			.set("class","legend");

		*out << cgicc::form().set("method","POST")
			.set("action",method) << endl;
		*out << cgicc::textarea("(Type or paste configuration here)").set("name","Text")
			.set("WRAP","OFF")
			.set("rows","20").set("cols","60") << endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("value","Initialize and Configure");
		*out << cgicc::form() << endl;

		*out << cgicc::fieldset() << endl;

		*out << cgicc::body() << endl;
		*out << cgicc::html() << endl;

	} catch (const exception & e ) {
		//XECPT_RAISE(xgi::exception::Exception, e.what());
	}
}



void EmuFCrateHyperDAQ::setCrate(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception)
{
	cout << "--Entering setCrate" << endl;

	Cgicc cgi(in);

	cout << "Switching crate..." << endl;
	int icrate = 0;
	if (!(cgi["icrate"]->isEmpty())) icrate = cgi["icrate"]->getIntegerValue();

	thisCrate = crateVector[icrate];
	cout << " vme device " << thisCrate->vmeController()->Device() << endl;
	cout << " vme link " << thisCrate->vmeController()->Link() << endl;
	dduVector = thisCrate->ddus();
	dccVector = thisCrate->dccs();
	moduleVector = thisCrate->modules();

	cout << " crate set to " << icrate << ", which is crate number " << thisCrate->number() << endl;
	in = NULL;
//	webRedirect(in, out);
	webRedirect(out,"mainPage");
	//mainPage(in,out);
}



void EmuFCrateHyperDAQ::setRawConfFile(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	cout << "--Entering setRawConfFile" << endl;
	try {
		cout << "setRawConfFile" << endl;

		cgicc::Cgicc cgi(in);

		ofstream TextFile ;
		TextFile.open("MyTextConfigurationFile.xml");
		TextFile << (cgi["Text"]->getValue()) ;
		TextFile.close();

		xmlFile_ = "MyTextConfigurationFile.xml" ;
		cout << "Out setRawConfFile" << endl ;

		cout << "Clearing vectors to reset config." << endl;
		crateVector.clear();
		dduVector.clear();
		dccVector.clear();

		cout << "Load Default..." << endl;
		Default(in, out);
	} catch (const exception & e ) {
	//XECPT_RAISE(xgi::exception::Exception, e.what());
	}
}



void EmuFCrateHyperDAQ::setConfFile(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
 {
	cout << "--Entering setConfFile" << endl;
	try {
		//
		cgicc::Cgicc cgi(in);
		//

		/* No need for this any more.
		int SkipConfig=0;
		string Bid;
		SkipConfig=0;
		cgicc::form_iterator name = cgi.getElement("buttonid");
		if(name != cgi.getElements().end()) {
			Bid = cgi["buttonid"]->getValue();
			if(Bid=="Init Only")SkipConfig=1;
		}
		cout << "setConfFile: ButtonID=" << Bid << ".   SkipConfig=" << SkipConfig << endl ;
		*/

		const_file_iterator file;
		file = cgi.getFile("xmlFileName");
		cout << "GetFiles string" << endl ;
		if(file != cgi.getFiles().end()) (*file).writeToStream(cout);
		string XMLname = cgi["xmlFileName"]->getValue();
		//cout << XMLname  << endl;
		xmlFile_ = XMLname;

		cout << "Clearing vectors to reset config." << endl;
		crateVector.clear();
		dduVector.clear();
		dccVector.clear();

		cout << "Load Default..." << endl;
		Default(in, out);
	} catch (const exception & e ) {
		//XECPT_RAISE(xgi::exception::Exception, e.what());
	}
}



void EmuFCrateHyperDAQ::UploadConfFile(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	try {
		cout << "UploadConfFileUpload" << endl ;
		cgicc::Cgicc cgi(in);
		const_file_iterator file;
		file = cgi.getFile("xmlFileNameUpload");
		cout << "GetFiles" << endl ;
		if(file != cgi.getFiles().end()) {
			ofstream TextFile ;
			TextFile.open("MyTextConfigurationFile.xml");
			(*file).writeToStream(TextFile);
			TextFile.close();
		}
		xmlFile_ = "MyTextConfigurationFile.xml" ;

		cout << "Clearing vectors to reset config." << endl;
		crateVector.clear();
		dduVector.clear();
		dccVector.clear();

		cout << "Load Default..." << endl;
		Default(in, out);
	} catch (const exception & e ) {
		//XECPT_RAISE(xgi::exception::Exception, e.what());
	}
}



void EmuFCrateHyperDAQ::Configuring() {
	cout << "--Entering Configuring" << endl;
/*
	cgicc::Cgicc cgi(in);
	const CgiEnvironment& env = cgi.getEnvironment();
	string crateStr = env.getQueryString() ;
*/

	//cout << "Configuring: SkipConfig=" << SkipConfig << endl ;

	//
	//-- parse XML file
	//
	cout << "---- XML parser ----" << endl;
//	cout << " Here parser " << endl;
	FEDCrateParser parser;
	cout << " Using file " << xmlFile_.toString() << endl ;
	parser.parseFile(xmlFile_.toString().c_str());
	cout <<"---- Parser Finished ----"<<endl;

	cout << " clearing vectors..." << endl;
	crateVector.clear();
	dduVector.clear();
	dccVector.clear();

	cout << " setting vectors..." << endl;
	crateVector = parser.crateVector();
	dduVector = crateVector[0]->ddus();
	dccVector = crateVector[0]->dccs();
	moduleVector = crateVector[0]->modules();
	cout << " crateVector["<<crateVector.size()<<"] dduVector["<<dduVector.size()<<"] dccVector["<<dccVector.size()<<"]" << endl;

	thisCrate = crateVector[0];

// LSD, Make these optional with buttons
// JRG, only start/reset the IRQ handler:
// JRG uncomment?  if(SkipConfig>0) thisCrate->init(0);
	//if(SkipConfig==0) thisCrate->configure(0);

	cout << "EmuFCrateHyperDAQ::Configuring  >done< " << endl ;
}



void EmuFCrateHyperDAQ::DDUFirmware(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{

	cgicc::Cgicc cgi(in);

	cgicc::form_iterator name = cgi.getElement("ddu");
	unsigned int cgiDDU = 0;
	if (name != cgi.getElements().end()) {
		cgiDDU = cgi["ddu"]->getIntegerValue();
		//cout << "DDU inside " << ddu << endl;
	}
	thisDDU = dduVector[cgiDDU];

	ostringstream sTitle;
	sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DDU Firmware (RUI #" << thisCrate->getRUI(thisDDU->slot()) << ")";
	*out << Header(sTitle.str(),false);

// PGK Select-a-DDU
	*out << cgicc::fieldset()
		.set("class","header") << endl;
	*out << cgicc::div("Show this page for a different DDU")
		.set("style","font-weight: bold; font-size: 8pt; width: 100%; text-align: center;") << endl;

	unsigned int iddu = 0;

	*out << cgicc::table()
		.set("style","width: 90%; margin: 2px auto 2px auto; font-size: 8pt; text-align: center;") << endl;
	*out << cgicc::tr() << endl;
	// Loop over all the boards defined.
	for (unsigned int iModule = 0; iModule < moduleVector.size(); iModule++) {

		// Determine if we are working on a DDU or a DCC by module type
		thisDDU = dynamic_cast<DDU *>(moduleVector[iModule]);
		if (thisDDU != 0) {
			// I am a DDU!
			// Skip broadcasting
			if (thisDDU->slot() > 21) continue;

			*out << cgicc::td() << endl;
			if (iddu != cgiDDU) {
				ostringstream location;
				location << "/" + getApplicationDescriptor()->getURN() + "/DDUFirmware?ddu=" << iddu;
				*out << cgicc::a()
					.set("href",location.str());
			}
			*out << "Slot " << thisDDU->slot() << ": RUI #" << thisCrate->getRUI(thisDDU->slot());
			if (iddu != cgiDDU) {
				*out << cgicc::a();
			}
			*out << cgicc::td() << endl;

			iddu++;
		}
	}
	*out << cgicc::tr() << endl;
	*out << cgicc::table() << endl;
	*out << cgicc::fieldset() << endl;

	// Get this DDU back again.
	thisDDU = dduVector[cgiDDU];

	// PGK Display-a-Crate
	*out << cgicc::fieldset()
		.set("class","fieldset") << endl;
	*out << cgicc::div("Crate Selection")
		.set("class","legend") << endl;

	*out << cgicc::div()
		.set("style","background-color: #FF0; color: #00A; font-size: 16pt; font-weight: bold; width: 50%; text-align: center; padding: 3px; border: 2px solid #00A; margin: 5px auto 5px auto;");
	*out << "Crate " << thisCrate->number() << " Selected";
	*out << cgicc::div() << endl;

	*out << cgicc::span("Configuration located at " + xmlFile_.toString())
		.set("style","color: #A00; font-size: 10pt;") << endl;

	*out << cgicc::fieldset() << endl;
	*out << br() << endl;

	*out << cgicc::fieldset()
		.set("class","fieldset") << endl;
	*out << cgicc::div("DDU Firmware User/ID Codes")
		.set("class","legend") << endl;

	// Pick up the FPGA and PROM user/id codes.
	thisCrate->vmeController()->CAEN_err_reset();
	// There are names to these things.
	string deviceNames[8] = {
		"DDUFPGA",
		"INFPGA0",
		"INFPGA1",
		"VMEPROM",
		"DDUPROM0",
		"DDUPROM1",
		"INPROM0",
		"INPROM1"
	};
	// Now we grab the ids and user registers.
	unsigned int idCode[8];
	unsigned int userCode[8];

	idCode[0] = thisDDU->ddufpga_idcode();
	idCode[1] = thisDDU->infpga_idcode0();
	idCode[2] = thisDDU->infpga_idcode1();
	idCode[3] = thisDDU->vmeprom_idcode();
	idCode[4] = thisDDU->dduprom_idcode0();
	idCode[5] = thisDDU->dduprom_idcode1();
	idCode[6] = thisDDU->inprom_idcode0();
	idCode[7] = thisDDU->inprom_idcode1();

	userCode[0] = thisDDU->ddufpga_usercode();
	userCode[1] = thisDDU->infpga_usercode0();
	userCode[2] = thisDDU->infpga_usercode1();
	userCode[3] = thisDDU->vmeprom_usercode();
	userCode[4] = thisDDU->dduprom_usercode0();
	userCode[5] = thisDDU->dduprom_usercode1();
	userCode[6] = thisDDU->inprom_usercode0();
	userCode[7] = thisDDU->inprom_usercode1();

	// Next, print the table with the codes.
	string dduClass = "";
	if (thisCrate->vmeController()->CAEN_err()!=0) {
		dduClass = "caution";
	}

	*out << cgicc::table()
		.set("style","width: 90%; margin: 5px auto 5px auto; text-align: center; border-collapse: collapse; border: 2px solid #000;") << endl;

	// First row:  prom/fpga labels
	*out << cgicc::tr() << endl;
	// The first element is special:  DDU
	*out << cgicc::td()
		.set("class",dduClass)
		.set("style","border-right: 1px solid #000; font-weight: bold; width: 12%;");
	*out << "DDU";
	*out << cgicc::td() << endl;
	// The second element is special:  blank
	*out << cgicc::td()
		.set("class",dduClass)
		.set("style","border-right: 1px solid #000; font-weight: bold; width: 10%;");
	*out << "";
	*out << cgicc::td() << endl;

	// Loop over the ID types
	for (unsigned int iCode = 0; iCode < 8; iCode++) {
		*out << cgicc::td()
			.set("class","")
			.set("style","border: 1px solid #000; width: 10%; font-weight: bold;");
		*out << deviceNames[iCode];
		*out << cgicc::td() << endl;
	}
	*out << cgicc::tr() << endl;

	// Second row:  user codes
	*out << cgicc::tr() << endl;
	// The first element is special:  DDU
	*out << cgicc::td()
		.set("class",dduClass)
		.set("style","border-right: 1px solid #000; font-weight: bold; width: 12%;");
	*out << "Slot " << thisDDU->slot();
	*out << cgicc::td() << endl;
	// The second element is special:  label
	*out << cgicc::td()
		.set("class",dduClass)
		.set("style","border-right: 1px solid #000; font-weight: bold; width: 10%;");
	*out << "User Code";
	*out << cgicc::td() << endl;

	// Loop over the ID types
	for (unsigned int iCode = 0; iCode < 8; iCode++) {
		string codeClass = "ok";
		// The dduproms have the board ID encoded into it...
		if (iCode == 4 || iCode == 5) {
			// Check to see if these match the expectations
			if (userCode[iCode] & 0xffffff00 != tuscode[iCode] & 0xffffff00) {
				codeClass = "bad";
			}
		} else {
			// Check to see if these match the expectations
			if (userCode[iCode] != tuscode[iCode]) {
				codeClass = "bad";
			}
		}

		*out << cgicc::td()
			.set("class",codeClass)
			.set("style","border: 1px solid #000; width: 10%;");
		*out << hex << uppercase << userCode[iCode] << dec;
		*out << cgicc::td() << endl;
	}
	*out << cgicc::tr() << endl;

	// Third row:  id codes
	*out << cgicc::tr() << endl;

	// The first element is special:  RUI
	*out << cgicc::td()
		.set("class",dduClass)
		.set("style","border-right: 1px solid #000; font-weight: bold; width: 10%;");
	*out << "RUI #" << thisCrate->getRUI(thisDDU->slot());
	*out << cgicc::td() << endl;
	// The second element is special:  label
	*out << cgicc::td()
		.set("class",dduClass)
		.set("style","border-right: 1px solid #000; font-weight: bold; width: 10%;");
	*out << "ID Code";
	*out << cgicc::td() << endl;

	// Loop over the ID types again
	for (unsigned int iCode = 0; iCode < 8; iCode++) {
		string codeClass = "ok";
		// Check to see if these match the expectations
		if (idCode[iCode] != tidcode[iCode]) {
			codeClass = "bad";
		}

		*out << cgicc::td()
			.set("class",codeClass)
			.set("style","border: 1px solid #000; width: 10%;");
		*out << hex << uppercase << idCode[iCode] << dec;
		*out << cgicc::td() << endl;
	}
	*out << cgicc::tr() << endl;

	*out << cgicc::table() << endl;

	*out << cgicc::fieldset() << endl;

	thisDDU = dduVector[cgiDDU];

	// Now it's time to put the upload forms for the selected DDU
	*out << cgicc::fieldset()
		.set("class","fieldset") << endl;
	ostringstream uploadTitle;
	uploadTitle << "DDU Firmware Upload (Slot " << thisDDU->slot() << ", RUI #" << thisCrate->getRUI(thisDDU->slot()) << ")";
	*out << cgicc::div(uploadTitle.str())
		.set("class","legend") << endl;

	// There's five things to upload:
	string names[5] = {
		"VMEPROM",
		"DDUPROM0",
		"DDUPROM1",
		"INPROM0",
		"INPROM1"
	};

	// And here are the forms we use to upload them:
	for (unsigned int iPROM = 0; iPROM < 5; iPROM++) {
		*out << cgicc::form()
			.set("method","POST")
			.set("enctype","multipart/form-data")
			.set("action","/"+getApplicationDescriptor()->getURN()+"/DDULoadFirmware") << endl;
		// NOTE: it's not exacly well defined if file type inputs
		// obey the value parameter.
		*out << cgicc::div(names[iPROM])
			.set("style","font-weight: bold;") << endl;
		*out << cgicc::input().set("type","file")
			.set("name","DDULoadSVF")
			.set("size","50") << endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("value","LoadSVF") << endl;
		ostringstream idduValue;
		idduValue << iddu;
		*out << cgicc::input()
			.set("type","hidden")
			.set("value",idduValue.str())
			.set("name","ddu") << endl;
		ostringstream inputValue;
		inputValue << (iPROM+3);
		*out << cgicc::input()
			.set("type","hidden")
			.set("value",inputValue.str())
			.set("name","prom") << endl;
		*out << cgicc::form() << endl;
	}
/*
	*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << endl;
	*out << cgicc::html().set("lang", "en").set("dir","ltr") << endl;
	*out << cgicc::title("DDU Firmware Form") << endl;
	*out << body().set("background","/tmp/bgndcms.jpg") << endl;
	*out << cgicc::div().set("style","font-size: 16pt; font-weight: bold; color: #D00; width: 400px; margin-left: auto; margin-right: auto; text-align: center;") << "Crate " << thisCrate->number() << " Selected" << cgicc::div() << endl;
	*out << "(Load dual proms in order 1-0) (For hard reset use DCC TTC command)" <<endl;
	*out << br() << endl;
	*out << br() << endl;

	string xmltext="";
	for(int i=0;i<=7;i++){
		xmltext="data/ddu_config/";
		printf(" LOOP: %d \n",i);
		*out << cgicc::span().set("style","color:black");
		if(thisDDU->slot()>21){ // Broadcast only (not needed anymore?)
			sprintf(buf," ");

			switch(i) {
			case 1:
				sprintf(buf," <font size=+1> DDU Broadcast Firmware Load</font> ");
				break;
			case 3:
				sprintf(buf,"vmeprom  ");
				break;
			case 4:
				sprintf(buf,"dduprom0 ");
				xmltext="/home/cscfed/firmware/ddu5ctrl_0.svf";
				break;
			case 5:
				sprintf(buf,"dduprom1 ");
				xmltext="/home/cscfed/firmware/ddu5ctrl_1.svf";
				break;
			case 6:
				sprintf(buf,"inprom0  ");
				break;
			case 7:
				sprintf(buf,"inprom1  ");
				break;
			default:
				break;
			}

			printf(" %s ",buf);
			*out << buf << cgicc::span() << endl;
		} else {
			thisCrate->vmeController()->CAEN_err_reset();
			if(i==0){idcode=thisDDU->ddufpga_idcode(); sprintf(buf,"ddufpga  ");}
			if(i==1){idcode=thisDDU->infpga_idcode0(); sprintf(buf,"infpga0  ");}
			if(i==2){idcode=thisDDU->infpga_idcode1(); sprintf(buf,"infpga1  ");}
			if(i==6){idcode=thisDDU->inprom_idcode0(); sprintf(buf,"inprom0  ");}
			if(i==7){idcode=thisDDU->inprom_idcode1(); sprintf(buf,"inprom1  ");}
			if(i==3){idcode=thisDDU->vmeprom_idcode(), sprintf(buf,"vmeprom  ");}
			if(i==4){idcode=thisDDU->dduprom_idcode0();sprintf(buf,"dduprom0 ");}
			if(i==5){idcode=thisDDU->dduprom_idcode1();sprintf(buf,"dduprom1 ");}
			printf(" %s idcode %08lx ",buf,idcode);
			*out<<buf;
			sprintf(buf,"  id: ");
			*out << buf;*out << cgicc::span();
			if(thisCrate->vmeController()->CAEN_err()!=0){
				*out << cgicc::span()
					.set("style","color:yellow;background-color:#dddddd;");
			}else if(idcode!=tidcode[i]){
				*out << cgicc::span()
					.set("style","color:red;background-color:#dddddd;");
			} else {
				*out << cgicc::span()
					.set("style","color:green;background-color:#dddddd");
			}
			sprintf(buf,"%08lX",idcode);
			*out << buf;*out << cgicc::span();

			thisCrate->vmeController()->CAEN_err_reset();
			if(i==0){uscode=thisDDU->ddufpga_usercode();}
			if(i==1){uscode=thisDDU->infpga_usercode0();}
			if(i==2){uscode=thisDDU->infpga_usercode1();}
			if(i==6){uscode=thisDDU->inprom_usercode0();}
			if(i==7){uscode=thisDDU->inprom_usercode1();}
			if(i==3){uscode=thisDDU->vmeprom_usercode();}
			if(i==4){uscode=thisDDU->dduprom_usercode0();}
			if(i==5){uscode=thisDDU->dduprom_usercode1();}
			*out << cgicc::span().set("style","color:black");
			sprintf(buf," usr: ");
			*out << buf;*out << cgicc::span();
			if(i==4||i==5){
				printf(" uscode %06lx ",(uscode>>8)&0x00ffffff);
				printf(" BoardID %2ld \n",uscode&0x000000ff);
			} else {
				printf(" uscode %08lx \n",uscode);
			}
			if(thisCrate->vmeController()->CAEN_err()!=0){
				*out << cgicc::span()
					.set("style","color:yellow;background-color:#dddddd;");
			}else if(i!=4 && i!=5 && uscode!=tuscode[i]){
				*out << cgicc::span()
					.set("style","color:red;background-color:#dddddd;");
			}else if((i==4 || i==5) && (uscode&0xffffff00)!=(tuscode[i]&0xffffff00)){
				*out << cgicc::span()
					.set("style","color:red;background-color:#dddddd;");
			}else{
				*out << cgicc::span()
					.set("style","color:green;background-color:#dddddd;");
			}

			if(i==4||i==5){
				sprintf(buf,"%06lX",(uscode>>8)&0x00ffffff);
				*out << buf;*out << cgicc::span();
				*out << cgicc::span().set("style","color:black");
				sprintf(buf," BoardID: ");
				*out << buf;*out << cgicc::span();
				*out << cgicc::span().set("style","color:blue;background-color:#dddddd;");
				sprintf(buf,"%2ld",uscode&0x000000ff);
			} else {
				sprintf(buf,"%08lX",uscode);
			}
			//sprintf(buf,"%08lX",uscode);
			*out << buf << cgicc::span() << endl;
			if(i==2) *out << br() << endl;
		} // end regular DDU (not broadcast)

		*out << br() << endl;
		printf(" now boxes \n");

		if(i>=3&&i<8){
			string dduloadfirmware =
			toolbox::toString("/%s/DDULoadFirmware",getApplicationDescriptor()->getURN().c_str());

			*out << cgicc::form().set("method","POST")
				.set("enctype","multipart/form-data")
				.set("action",dduloadfirmware) << endl;
				//.set("enctype","multipart/form-data")

			// NOTE: the _input_ tag is what takes the value parameter,
			// not the _form_ tag.
			// Also, it's not exacly well defined if file type inputs
			// obey the value parameter.  The default value will be
			// browser-dependent.
			*out << cgicc::input().set("type","file")
				.set("name","DDULoadSVF")
				.set("value",xmltext)
				.set("size","50") << endl;

			*out << cgicc::input()
				.set("type","submit")
				.set("value","LoadSVF") << endl;
			sprintf(buf,"%d",ddu);
			*out << cgicc::input()
				.set("type","hidden")
				.set("value",buf)
				.set("name","ddu") << endl;
			sprintf(buf,"%d",i);

			if(thisDDU->slot()>21)sprintf(buf,"%d",10+i);
			*out << cgicc::input()
				.set("type","hidden")
				.set("value",buf)
				.set("name","prom") << endl;
			*out << cgicc::form() << endl ;

		} else { // Not a proper thing to load
			*out << endl;
		}
	} // end run over chip types
	*out << cgicc::fieldset()<< endl;
	*out << cgicc::body() << endl;
	*out << cgicc::html() << endl;
	*/

	*out << cgicc::fieldset() << endl;

	*out << Footer() << endl;

}



void EmuFCrateHyperDAQ::DDUBroadcast(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception)
{

	Cgicc cgi(in);

	printf(" entered DDUBroadcast \n");

	*out << HTMLDoctype(HTMLDoctype::eStrict) << endl;
	*out << html().set("lang", "en").set("dir","ltr") << endl;
	*out << title("DDU Broadcast Firmware") << endl;
	*out << body().set("background","/tmp/bgndcms.jpg") << endl;
	*out << cgicc::div().set("style","font-size: 16pt; font-weight: bold; color: #D00; width: 400px; margin-left: auto; margin-right: auto; text-align: center;") << "Crate " << thisCrate->number() << " Selected" << cgicc::div() << endl;
/*
	cout << "begin password" << endl;
	string password = "";
	int password_needed = 0;
	if (!(cgi["password"]->isEmpty())) password = cgi["password"]->getValue();
	cout << "password #1 " << password << endl;
	if (!(cgi["needed"]->isEmpty())) password_needed = cgi["needed"]->getIntegerValue();
	cout << "checking password " << password_needed << endl;
	if (password != "abc123" && password_needed == 1) {
		*out << "Error:  Get the password from an expert before attempting this operation." << endl;
	} else {
*/
	*out << "Use the UPLOAD section to hand off local firmware copies to the server" << br() << "Use the BROADCAST section to actually load the firmware onto the boards" << br() << a().set("style","color: #D00; font-weight: bold;") << endl;

	*out << "!!! DO NOT BROADCAST UNLESS HYPERDAQ KNOWS ABOUT ALL THE DDUS IN THE CRATE !!!" << a() << endl;
	*out << br() << endl;
	*out << br() << endl;

	string title[5] = {"VMEPROM","DDUPROM0","DDUPROM1","INPROM0","INPROM1"};

	// Green, Red, Yellow, Blue
	string error[5] = {"background-color: #CFC;","background-color: #FCC;","background-color: #FF9;","background-color: #CCF;"};


	string dduloadbroadcast = toolbox::toString("/%s/DDULoadBroadcast",getApplicationDescriptor()->getURN().c_str());
	string ddusendbroadcast = toolbox::toString("/%s/DDUSendBroadcast",getApplicationDescriptor()->getURN().c_str());
	string ddureset = toolbox::toString("/%s/DDUReset",getApplicationDescriptor()->getURN().c_str());

	*out << fieldset().set("style","font-size: 13pt; font-family: arial;");
	*out << endl;
	*out << legend("Step 1: DDU firmware, UPLOAD SVF file to server").set("style","color:blue") << endl;

	string vheader[5] = {"b0","c0","c1","d0","d1"};
	string version[5] = {"0","0","0","0","0"};

	// UPLOAD
	for (int i=0; i<=4; i++) {

		*out << cgicc::div().set("style","background-color: #FFF; border-color: #000; border-width: 1px; border-style: solid; margin: 2px 2px 2px 2px; padding: 3px 3px 3px 3px;") << endl;
		*out << a(title[i]).set("style","font-size: 14pt; font-weight: bold;") << br() << endl;

		// Get the version number from the on-disk file
		// Best mashup of perl EVER!
		ostringstream systemcall;
		string printversion;

		// My perl-fu is 1337, indeed!
		systemcall << "perl -e 'while ($line = <>) { if ($line =~ /SIR 8 TDI \\(fd\\) TDO \\(00\\) ;/) { $line = <>; if ($line =~ /TDI \\((........)\\)/) { print $1; } } }' <Current" << title[i] << ".svf >check_ver 2>&1";
		if (!system(systemcall.str().c_str())) {
			ifstream pipein("check_ver",ios::in);
			getline(pipein,printversion);
			pipein.close();
		}

		int verr = 0;

		// Check header on disk
		string checkstring( printversion, 0, 2 );
		if ( checkstring != vheader[i] ) {
			verr = 1;
			printversion = "ERROR READING LOCAL FILE -- UPLOAD A NEW FILE";
		}
		else version[i] = printversion;

		// Compare to hard-coded version
		ostringstream checkstream;
		checkstream << hex << tuscode[i+3];
		if ( checkstream.str() != printversion ) {
			verr = 1;
			printversion += " (should be " + checkstream.str() + ")";
		}

		*out << "Registered version on-disk: " << a().set("style","font-weight: bold;" + error[verr]) << printversion << a() << br() << endl;

		*out << form().set("method","POST")
			.set("enctype","multipart/form-data")
			.set("id","Form" + title[i])
			.set("action",dduloadbroadcast) << endl;
		*out << input().set("type","file")
			.set("name","File")
			.set("id","File" + title[i])
			.set("size","50") << endl;
		*out << input().set("type","button")
			.set("value","Upload SVF")
			.set("onClick","javascript:clickCheck('" + title[i] + "')") << endl;
		*out << input().set("type","hidden")
			.set("name","svftype")
			.set("value",title[i]) << endl;
		*out << form() << endl;

		*out << cgicc::div() << endl;
	}

	*out << fieldset() << endl;
	*out << br() << endl;

	// Tricky: javascript check!
	*out << "<script type=\"text/javascript\">function clickCheck(id) {" << endl;
	*out << "var element = document.getElementById('File' + id);" << endl;
	*out << "if (element.value == '') element.style.backgroundColor = '#FFCCCC';" << endl;
	*out << "else {" << endl;
	*out << "var form = document.getElementById('Form' + id);" << endl;
	*out << "form.submit();" << endl;
	*out << "}" << endl;
	*out << "}</script>" << endl;

	*out << fieldset().set("style","font-size: 13pt; font-family: arial;");
	*out << endl;
	*out << legend("Step 2: DDU firmware, BROADCAST firmware to whole crate").set("style","color:blue") << endl;

	*out << cgicc::div().set("style","background-color: #FFF; border-color: #000; border-width: 1px; border-style: solid; margin: 2px 2px 2px 2px; padding: 3px 3px 3px 3px;") << endl;
	*out << a("Click a button to perform broadcast firmware updates").set("style","font-size: 14pt; font-weight: bold;") << br() << endl;


	string prom[4] = {"VMEPROM","DDUPROM","INPROM","ALL"};

	// BROADCAST/RESET
	for (int i=0; i<=3; i++) {

		if (i == 3) *out << "<!-- ";

		string button = "Broadcast " + prom[i];
		ostringstream number;
		number << i;

		*out << form().set("method","POST")
		  .set("action",ddusendbroadcast) << endl; // calls DDUSendBroadcast
		*out << input().set("type","submit")
			.set("value",button) << endl;
		*out << input().set("type","hidden")
			.set("name","svftype")
			.set("value", number.str().c_str()) << endl;
		for (int j=0; j<=4; j++) {
			*out << input().set("type","hidden")
				.set("name",title[j])
				.set("value",version[j]) << endl;
		}

		*out << form() << endl;

		if (i == 3) *out << " -->";

	}

	*out << form().set("method","POST")
		.set("action",ddureset) << endl;
	*out << input().set("type","submit")
		.set("value","Reset Crate");
	*out << "<input type=\"checkbox\" name=\"useTTC\" CHECKED /> Use DCC Command Bus";
	*out << form() << endl;

	*out << cgicc::div() << endl;
	*out << fieldset() << endl;
	*out << br() << endl;

	// STATUS
	*out << table().set("style","border-width: 2px; border-color: #000; border-style: solid; background-color: #FFF; width: 90%; margin-right: auto; margin-left: auto;");
	*out << tr() << endl;

	*out << td().set("style","font-weight: bold; font-size: 12pt; color: #FFF; background-color: #000;") << "Slot (BN)" << td() << endl;
	for (int i=0; i<=4; i++) {
		*out << td().set("style","font-weight: bold; font-size: 12pt; color: #FFF; background-color: #000;") << title[i] << td() << endl;
	}
	*out << tr() << endl;

	for (unsigned int i=0; i<dduVector.size(); i++) {
		if (dduVector[i]->slot() <= 21) {
			int slot = dduVector[i]->slot();

			ostringstream firmware_version[5];
			ostringstream fpga_version[3];
			firmware_version[0] << hex << dduVector[i]->vmeprom_usercode();
			firmware_version[1] << hex << dduVector[i]->dduprom_usercode0();
			firmware_version[2] << hex << dduVector[i]->dduprom_usercode1();
			firmware_version[3] << hex << dduVector[i]->inprom_usercode0();
			firmware_version[4] << hex << dduVector[i]->inprom_usercode1();

			fpga_version[0] << hex << dduVector[i]->ddufpga_usercode();
			fpga_version[1] << hex << dduVector[i]->infpga_usercode0();
			fpga_version[2] << hex << dduVector[i]->infpga_usercode1();

			string boardnumber0 = firmware_version[1].str().substr(6,2);
			string boardnumber1 = firmware_version[2].str().substr(6,2);

			int err[5] = {0,0,0,0,0};

			int ibn = dduVector[i]->dduprom_usercode0() & 0xff;
			ostringstream bn;
			if (boardnumber0 == boardnumber1)
				bn << dec << ibn;
			else {
				bn << "MISMATCH";
				err[1] = 2;
				err[2] = 2;
			}

			for (int j=0; j<=4; j++) {
				if (j==1 || j==2) {
					if (firmware_version[j].str().substr(0,6) != version[j].substr(0,6)) {
						err[j] = 1;
					} else if (fpga_version[0].str().substr(3,2) != firmware_version[j].str().substr(2,2)) {
						err[j] = 3;
					}
				}
				else if (version[j] != firmware_version[j].str()) {
					err[j] = 1;
				}
				else if (j==3 || j==4) {
					if (fpga_version[j-2].str().substr(2,6) != firmware_version[j].str().substr(2,6)) {
						err[j] = 3;
					}
				}
			}

			*out << tr() << endl;

			string err2 = "";
			if (((dduVector[i]->vmepara_status()>>8)&0x000F)!= 8) {
				err2 = "background-color: #AAA;";
			}

			*out << td().set("style","font-weight: bold; font-size: 12pt; border-color: #000; border-style: solid; border-width: 1px;" + err2) << slot << " (" << bn.str() << ")" << td() << endl;

			for (int j=0; j<=4; j++) {
				*out << td().set("style","font-size: 10pt; border-color: #000; border-style: solid; border-width: 1px;" + error[err[j]]) << firmware_version[j].str() << td() << endl;
			}

			*out << tr() << endl;
		}
	}

	*out << tr() << endl;
	*out << td().set("style","font-weight: bold; font-size: 12pt; color: #FFF; background-color: #000;") << "Legend" << td() << endl;
	*out << td("all good").set("style","font-size: 10pt; border-color: #000; border-style: solid; border-width: 2px;" + error[0]) << endl;
	*out << td("prom/disk mismatch").set("style","font-size: 10pt; border-color: #000; border-style: solid; border-width: 2px;" + error[1]) << endl;
	*out << td("board number mismatch").set("style","font-size: 10pt; border-color: #000; border-style: solid; border-width: 2px;" + error[2]) << endl;
	*out << td("prom/fpga mismatch").set("style","font-size: 10pt; border-color: #000; border-style: solid; border-width: 2px;" + error[3]) << endl;
	*out << td("bad board status").set("style","font-size: 10pt; border-color: #000; border-style: solid; border-width: 2px; background-color: #AAA") << endl;
	*out << tr() << endl;

	*out << table() << br() << endl;

/*	ostringstream debug;
	debug << "Debug information: <br />"
		<< "ddu: " << ddu << "<br />"
		<< "slot: " << thisDDU->slot() << "<br />";
	*out << cgicc::div(debug.str().c_str()).set("style","font-size: 11pt") << endl; */

//	} // if password was correct.

	*out << body() << endl;
	*out << html() << endl;
}



void EmuFCrateHyperDAQ::DDULoadBroadcast(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception)
{

	Cgicc cgi(in);

	printf(" entered DDULoadBroadcast \n");

	string type = cgi["svftype"]->getValue();

	if (type != "VMEPROM" && type != "DDUPROM0" && type != "DDUPROM1" && type != "INPROM0" && type != "INPROM1") {
		cout << "I don't understand that PROM type (" << type << ")." << endl;
		in = NULL;

		webRedirect(out,"DDUBroadcast");
		//this->DDUBroadcast(in,out);
		//return;
	}

	const_file_iterator ifile = cgi.getFile("File");
	if ( (*ifile).getFilename() == "" ) {
		cout << "The file you attempted to upload either doesn't exist, or wasn't properly transferred." << endl;
		in = NULL;
		webRedirect(out,"DDUBroadcast");
		//this->DDUBroadcast(in,out);
		//return;
	}

	string filename = "Current" + type + ".svf";
	ofstream outfile;
	outfile.open(filename.c_str(),ios::trunc);
	if (!outfile.is_open()) {
		cout << "I can't open the file stream for writing (" << filename << ")." << endl;
		in = NULL;
		webRedirect(out,"DDUBroadcast");
		//this->DDUBroadcast(in,out);
		//return;
	}

	(*ifile).writeToStream(outfile);
	outfile.close();

	cout << "downloaded and saved " << filename << " of type " << type << endl;
	in = NULL;
	webRedirect(out,"DDUBroadcast");
	//this->DDUBroadcast(in,out);

}



void EmuFCrateHyperDAQ::DDUSendBroadcast(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception)
{

	Cgicc cgi(in);

	printf(" entered DDUSendBroadcast \n");

	int type = cgi["svftype"]->getIntegerValue();

	if (type != 0 && type != 1 && type != 2 && type != 3) {
		cout << "I don't understand that PROM type (" << type << ")." << endl;
		in = NULL;
		webRedirect(out,"DDUBroadcast");
		//this->DDUBroadcast(in,out);
		//return;
	}

	string name[5] = {"VMEPROM","DDUPROM1","DDUPROM0","INPROM1","INPROM0"};
	enum DEVTYPE devtype[5] = {VMEPROM,DDUPROM1,DDUPROM0,INPROM1,INPROM0};

	// Load the proper version types from the cgi handle.
	string version[5];
	for (int i=0; i<=4; i++) {
		version[i] = cgi[name[i].c_str()]->getValue();
		cout << " version on disk: " << name[i] << " " << version[i] << endl;
	}

	int from, to;
	if (type == 0) { from = 0; to = 0; }
	if (type == 1) { from = 1; to = 2; }
	if (type == 2) { from = 3; to = 4; }
	if (type == 3) { from = 0; to = 4; }

	cout << " From type " << type << ", broadcasting ";
	for (int i=from; i<=to; i++) {
		cout << name[i] << " ";
	}
	cout << endl;

	int bnstore[dduVector.size()];
	int broadcastddu;
	for (unsigned int i=0; i<dduVector.size(); i++) {
		if (dduVector[i]->slot() > 21) broadcastddu = i;
	}

	for (int i=from; i<=to; i++) {
		string filename = "Current" + name[i] + ".svf";
		cout << " broadcasting " << filename << " version " << version[i] << endl;

		char *boardnumber = (char *) malloc(5);
		boardnumber[0]=0x00;boardnumber[1]=0x00;boardnumber[2]=0x00;boardnumber[3]=0x00;

		if (i == 1 || i == 2) {
			thisDDU = dduVector[broadcastddu];
			boardnumber[0] = 1;


			thisDDU->epromload((char *)name[i].c_str(),devtype[i],(char *)filename.c_str(),1,boardnumber,1);

			for (unsigned int ddu=0; ddu<dduVector.size(); ddu++) {
				thisDDU = dduVector[ddu];
				if (thisDDU->slot() > 21) continue;
				int ibn = thisDDU->read_page7();
				bnstore[ddu] = ibn;
				boardnumber[0] = ibn;
				cout << " pause to upload boardnumber " << ibn << endl;
				thisDDU->epromload((char *)name[i].c_str(),devtype[i],(char *)filename.c_str(),1,boardnumber,2);
			}

			cout << " resuming... " << endl;
			boardnumber[0] = 1;
			thisDDU = dduVector[broadcastddu];
			thisDDU->epromload((char *)name[i].c_str(),devtype[i],(char *)filename.c_str(),1,boardnumber,3);

		} else {
			thisDDU = dduVector[broadcastddu];
			thisDDU->epromload((char *)name[i].c_str(),devtype[i],(char *)filename.c_str(),1,boardnumber);
		}
		free(boardnumber);
		cout << " broadcast of " << filename << " complete!" << endl;

	}

	cout << " broadcast operations complete... " << endl;
	cout << " checking firmware versions on PROMs to see if broadcast worked." << endl;
//	cout << "resetting crate via DCC TTC 34 and checking firmware versions..." << endl;

	// I hope there is only one DCC in the crate...
	//dccVector[0]->mctrl_ttccmd(52); // is 34 in hex: DDU hard reset
	//sleep((unsigned int)1);

	for (int i=from; i<=to; i++) { // loop over PROMS
		for (unsigned int ddu=0; ddu<dduVector.size(); ddu++) { // loop over boards
			thisDDU = dduVector[ddu];
			if (thisDDU->slot() > 21) continue;

			cout << "slot: " << thisDDU->slot() << endl;

			string boardversion;
			string checkversion;
			if (i==0) {
				boardversion = thisDDU->vmeprom_usercode();
				checkversion = version[i];
			} else if (i==1) {
				boardversion = thisDDU->dduprom_usercode0();
				checkversion = version[i].substr(0,6);
				checkversion += (bnstore[i] & 0xff);
			} else if (i==2) {
				boardversion = thisDDU->dduprom_usercode1();
				checkversion = version[i].substr(0,6);
				checkversion += (bnstore[i] & 0xff);
			} else if (i==3) {
				boardversion = thisDDU->inprom_usercode0();
				checkversion = version[i];
			} else if (i==4) {
				boardversion = thisDDU->inprom_usercode1();
				checkversion = version[i];
			}

			if (checkversion != boardversion) {
				cout << name[i] << " GOOD! (shows " << checkversion <<")" << endl;
			} else {
				cout << name[i] << " BAD! (shows " << boardversion << ", shoud be " << checkversion << ")" << endl;
			}
		}
	}

	in = NULL;
	webRedirect(out,"DDUBroadcast");
	//this->DDUBroadcast(in,out);

}


/// @note How do I send a global reset request?  Do I need to use SOAP?
void EmuFCrateHyperDAQ::DDUReset(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception)
{

	Cgicc cgi(in);

	string useTTC = cgi["useTTC"]->getValue();
	if (useTTC != "on") useTTC = "off";
	cout << "Entering DDUReset with useTTC " << useTTC << endl;

	if (useTTC == "on") {
		cout << " resetting via TTC 0x34" <<endl;
		dccVector[0]->mctrl_ttccmd(52); // is 34 in hex: DDU hard reset
		sleep((unsigned int)1);
	} else {
		cout << " don't know how to do global reset requests." << endl;
	}

	cout << " reset complete." << endl;
	in = NULL;
	webRedirect(out,"DDUBroadcast");
	//this->DDUBroadcast(in,out);
}


/// @note There should be a more expert function that handles these sorts of things.
void EmuFCrateHyperDAQ::DDUBrcstFED(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception) {
	try {
		Cgicc cgi(in);

		printf(" entered DDUBcstFED \n");
		int broadcastddu;
		for (unsigned int i=0; i<dduVector.size(); i++) {
			if (dduVector[i]->slot() > 21) broadcastddu = i;
		}
		printf("   dduVectorSize=%d, broadcastDDU indexID=%d\n",dduVector.size(),broadcastddu);
		thisDDU = dduVector[broadcastddu];
		thisDDU->vmepara_wr_fmmreg(0xFED8);
		/* PGK
		*out << "<script type=\"text/javascript\">history.back()</script>" << endl;
		string reload = toolbox::toString("/%s",getApplicationDescriptor()->getURN().c_str());
		*/
		// This is a better way to reload the main page, I think.
		webRedirect(out,"mainPage");
		//mainPage(in,out);

	} catch (const exception & e ) {
		printf(" exception raised in DDUBrcstFED \n");
		//XECPT_RAISE(xgi::exception::Exception, e.what());
	}

}


/// @note Try to merge this with the broadcast method.
void EmuFCrateHyperDAQ::DDULoadFirmware(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{

	cgicc::Cgicc cgi(in);

	cgicc::form_iterator name = cgi.getElement("ddu");

	int ddu = 0;
	if(name != cgi.getElements().end()) {
		ddu = cgi["ddu"]->getIntegerValue();
		cout << "DDU " << ddu << endl;
	}
	int prom;
	cgicc::form_iterator name2 = cgi.getElement("prom");
	//
	if(name2 != cgi.getElements().end()) {
		prom = cgi["prom"]->getIntegerValue();
		if(prom==13)prom=9;   // JG, special Broadcast case for VMEPROM...
		if(prom>=10)prom-=10; //  ...now remove the "10+" flag for all cases.
		cout << "PROM " << prom << endl;
	}
	//
	thisDDU = dduVector[ddu];
	//
	//
	cout << "UploadConfFileUpload" << endl;
	//


	const_file_iterator file;
	file = cgi.getFile("DDULoadSVF");
	//string XMLname = (*file).getFilename();
	// cout <<"SVF FILE: " << XMLname  << endl ;


	//
	cout << "GetFiles" << endl ;
	//

	if(file != cgi.getFiles().end()) {
		ofstream TextFile ;
		TextFile.open("MySVFFile.svf");
		(*file).writeToStream(TextFile);
		TextFile.close();
	}
	char buf[400];
	FILE *dwnfp;
	dwnfp    = fopen("MySVFFile.svf","r");
	printf("ready to download PROM %d \n",prom);
	while (fgets(buf,256,dwnfp) != NULL)printf("%s",buf);
	fclose(dwnfp);
	printf(" I am done so prom wont be called %d \n",prom);

	char *cbrdnum;
	cbrdnum=(char*)malloc(5);
	cbrdnum[0]=0x00;cbrdnum[1]=0x00;cbrdnum[2]=0x00;cbrdnum[3]=0x00;
	if(prom==6)thisDDU->epromload("INPROM0",INPROM0,"MySVFFile.svf",1,cbrdnum);
	if(prom==7)thisDDU->epromload("INPROM1",INPROM1,"MySVFFile.svf",1,cbrdnum);
	if(prom==3)thisDDU->epromload("RESET",RESET,"MySVFFile.svf",1,cbrdnum);
	if(prom==9)thisDDU->epromload("VMEPROM",VMEPROM,"MySVFFile.svf",1,cbrdnum);
	if(prom==4||prom==5){
		int brdnum=thisDDU->read_page7();
		cbrdnum[0]=brdnum;
	}
	if(prom==4)thisDDU->epromload("DDUPROM0",DDUPROM0,"MySVFFile.svf",1,cbrdnum);
	if(prom==5)thisDDU->epromload("DDUPROM1",DDUPROM1,"MySVFFile.svf",1,cbrdnum);
	free(cbrdnum);
	in=NULL;
	ostringstream dduString;
	dduString << ddu;
	webRedirect(out,"DDUFirmware?ddu="+dduString.str());
	//this->DDUFirmware(in,out);
	// this->Default(in,out);
}



void EmuFCrateHyperDAQ::DDUFpga(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{


	cgicc::Cgicc cgi(in);

	cgicc::form_iterator name = cgi.getElement("ddu");
	unsigned int cgiDDU = 0;
	if (name != cgi.getElements().end()) {
		cgiDDU = cgi["ddu"]->getIntegerValue();
		cout << "DDU inside " << cgiDDU << endl;
	}
	thisDDU = dduVector[cgiDDU];
	cout << "My slot is " << thisDDU->slot() << endl;

	ostringstream sTitle;
	sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DDUFPGA Controls (RUI #" << thisCrate->getRUI(thisDDU->slot()) << ")";
	*out << Header(sTitle.str(),false);

	// PGK Select-a-DDU
	*out << cgicc::fieldset()
		.set("class","header") << endl;
	*out << cgicc::div("Show this page for a different DDU")
		.set("style","font-weight: bold; font-size: 8pt; width: 100%; text-align: center;") << endl;

	unsigned int iddu = 0;

	*out << cgicc::table()
		.set("style","width: 90%; margin: 2px auto 2px auto; font-size: 8pt; text-align: center;") << endl;
	*out << cgicc::tr() << endl;
	// Loop over all the boards defined.
	for (iddu = 0; iddu < dduVector.size(); iddu++) {

		if (dduVector[iddu]->slot() > 21) continue;

		*out << cgicc::td() << endl;
		if (iddu != cgiDDU) {
			ostringstream location;
			location << "/" + getApplicationDescriptor()->getURN() + "/DDUFpga?ddu=" << iddu;
			*out << cgicc::a()
				.set("href",location.str());
		}
		*out << "Slot " << dduVector[iddu]->slot() << ": RUI #" << thisCrate->getRUI(dduVector[iddu]->slot());
		if (iddu != cgiDDU) {
			*out << cgicc::a();
		}
		*out << cgicc::td() << endl;
	}
	*out << cgicc::tr() << endl;
	*out << cgicc::table() << endl;
	*out << cgicc::fieldset() << endl;

	// PGK Display-a-Crate
	*out << cgicc::fieldset()
		.set("class","fieldset") << endl;
	*out << cgicc::div("Crate Selection")
		.set("class","legend") << endl;

	*out << cgicc::div()
		.set("style","background-color: #FF0; color: #00A; font-size: 16pt; font-weight: bold; width: 50%; text-align: center; padding: 3px; border: 2px solid #00A; margin: 5px auto 5px auto;");
	*out << "Crate " << thisCrate->number() << " Selected";
	*out << cgicc::div() << endl;

	*out << cgicc::span("Configuration located at " + xmlFile_.toString())
		.set("style","color: #A00; font-size: 10pt;") << endl;

	*out << cgicc::fieldset() << endl;
	*out << br() << endl;

	// PGK Let us now print some tables with information from the DDUFPGA.
	// This guy will be used later to show which chambers have errors in the big
	//  table.  Unfortunately, we can only see the fibers on the active DDU.
	//  I should make a note of that on the table.
	unsigned int fibersWithErrors = 0;

	// This is used to check if the debug trap is valid.
	bool debugTrapValid = false;

	// PGK Your guess is as good as mine.
	thisDDU->infpga_shift0 = 0;
	thisDDU->ddu_shift0 = 0;

	// We will use this object to debug all the DDU's problems.
	DDUDebugger *debugger = new DDUDebugger();

	thisCrate->vmeController()->CAEN_err_reset();


	// Display general DDU status information
	*out << cgicc::fieldset()
		.set("class","normal") << endl;
	*out << cgicc::div()
		.set("class","legend") << endl;
	*out << cgicc::a("&plusmn;")
		.set("class","openclose")
		.set("onClick","javascript:toggle('generalTable')") << endl;
	*out << "General DDU Information" << cgicc::div() << endl;

	DataTable generalTable("generalTable");

	generalTable.addColumn("Register");
	generalTable.addColumn("Value");
	generalTable.addColumn("Decoded Status");

	*(generalTable(0,0)->value) << "DDU board ID (16-bit)";
	unsigned long int dduValue = thisDDU->readBoardID();
	*(generalTable(0,1)->value) << dduValue;
	generalTable(0,1)->setClass("none");

	*(generalTable(1,0)->value) << "DDU control FPGA status (32-bit)";
	dduValue = thisDDU->ddu_fpgastat();
	*(generalTable(1,1)->value) << showbase << hex << dduValue;
	if (dduValue & 0x00008000) {
		generalTable(1,1)->setClass("bad");
		debugTrapValid = true;
	}
	else if (dduValue & 0xDE4F4BFF) generalTable(1,1)->setClass("warning");
	else generalTable(1,1)->setClass("ok");
	std::map<string, string> dduComments = debugger->DDUFPGAStat(dduValue);
	for (std::map<string,string>::iterator iComment = dduComments.begin();
		iComment != dduComments.end();
		iComment++) {
		*(generalTable(1,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}

	*(generalTable(2,0)->value) << "DDU output status (16-bit)";
	dduValue = thisDDU->readOutputStat();
	*(generalTable(2,1)->value) << showbase << hex << dduValue;
	if (dduValue & 0x00000080) generalTable(2,1)->setClass("bad");
	else if (dduValue & 0x00004000) generalTable(2,1)->setClass("warning");
	else generalTable(2,1)->setClass("ok");
	dduComments = debugger->OutputStat(dduValue);
	for (std::map<string,string>::iterator iComment = dduComments.begin();
		iComment != dduComments.end();
		iComment++) {
		*(generalTable(2,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}

	*(generalTable(3,0)->value) << "DDU L1 scaler";
	dduValue = thisDDU->readL1Scaler(DDUFPGA);
	*(generalTable(3,1)->value) << dduValue;
	generalTable(3,1)->setClass("none");

	*(generalTable(4,0)->value) << "Error bus A register bits (16-bit)";
	dduValue = thisDDU->readEBReg(1);
	*(generalTable(4,1)->value) << showbase << hex << dduValue;
	if (dduValue & 0x0000C00C) generalTable(4,1)->setClass("bad");
	else if (dduValue & 0x000001C8) generalTable(4,1)->setClass("warning");
	else generalTable(4,1)->setClass("ok");
	dduComments = debugger->EBReg1(dduValue);
	for (std::map<string,string>::iterator iComment = dduComments.begin();
		iComment != dduComments.end();
		iComment++) {
		*(generalTable(4,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}

	*(generalTable(5,0)->value) << "Error bus B register bits (16-bit)";
	dduValue = thisDDU->readEBReg(2);
	*(generalTable(5,1)->value) << showbase << hex << dduValue;
	if (dduValue & 0x00000011) generalTable(5,1)->setClass("bad");
	else if (dduValue & 0x0000D08E) generalTable(5,1)->setClass("warning");
	else generalTable(5,1)->setClass("ok");
	dduComments = debugger->EBReg2(dduValue);
	for (std::map<string,string>::iterator iComment = dduComments.begin();
		iComment != dduComments.end();
		iComment++) {
		*(generalTable(5,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}

	*(generalTable(6,0)->value) << "Error bus C register bits (16-bit)";
	dduValue = thisDDU->readEBReg(3);
	*(generalTable(6,1)->value) << showbase << hex << dduValue;
	if (dduValue & 0x0000BFBF) generalTable(6,1)->setClass("warning");
	else generalTable(6,1)->setClass("ok");
	dduComments = debugger->EBReg3(dduValue);
	for (std::map<string,string>::iterator iComment = dduComments.begin();
		iComment != dduComments.end();
		iComment++) {
		*(generalTable(6,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}

	*out << generalTable.printSummary() << endl;

	// Display only if there are errors.
	if (generalTable.countClass("ok") + generalTable.countClass("none") == generalTable.countRows()) generalTable.setHidden(true);
	*out << generalTable.toHTML() << endl;

	*out << cgicc::fieldset() << endl;

	// Clever trick to help with formating the cut-and-paste.
	*out << cgicc::br()
		.set("style","display: none") << endl;

	
	

	thisCrate->vmeController()->CAEN_err_reset();
	// Display individual fiber information
	*out << cgicc::fieldset()
		.set("class","normal") << endl;
	*out << cgicc::div()
		.set("class","legend") << endl;
	*out << cgicc::a("&plusmn;")
		.set("class","openclose")
		.set("onClick","javascript:toggle('fiberTable')") << endl;
	*out << "Individual Fiber Information" << cgicc::div() << endl;


	DataTable fiberTable("fiberTable");

	fiberTable.addColumn("Register");
	fiberTable.addColumn("Value");
	fiberTable.addColumn("Decoded Chambers");

	*(fiberTable(0,0)->value) << "First event DMBLIVE";
	dduValue = thisDDU->readPermDMBLive();
	*(fiberTable(0,1)->value) << showbase << hex << dduValue;
	fiberTable(0,1)->setClass("none");
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (dduValue & (1<<iFiber)) {
			*(fiberTable(0,2)->value) << cgicc::div(thisDDU->getChamber(iFiber)->name())
				.set("class","none");
		}
	}

	*(fiberTable(1,0)->value) << "Latest event DMBLIVE";
	dduValue = thisDDU->readDMBLive();
	*(fiberTable(1,1)->value) << showbase << hex << dduValue;
	fiberTable(1,1)->setClass("none");
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (dduValue & (1<<iFiber)) {
			*(fiberTable(1,2)->value) << cgicc::div(thisDDU->getChamber(iFiber)->name())
				.set("class","none");
		}
	}

	*(fiberTable(2,0)->value) << "CRC error";
	dduValue = thisDDU->readCRCError();
	*(fiberTable(2,1)->value) << showbase << hex << dduValue;
	if (dduValue) fiberTable(2,1)->setClass("bad");
	else fiberTable(2,1)->setClass("ok");
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (dduValue & (1<<iFiber)) {
			*(fiberTable(2,2)->value) << cgicc::div(thisDDU->getChamber(iFiber)->name())
				.set("class","red");
			fibersWithErrors |= 1<<iFiber;
		}
	}

	*(fiberTable(3,0)->value) << "Data transmit error";
	dduValue = thisDDU->readXmitError();
	*(fiberTable(3,1)->value) << showbase << hex << dduValue;
	if (dduValue) fiberTable(3,1)->setClass("bad");
	else fiberTable(3,1)->setClass("ok");
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (dduValue & (1<<iFiber)) {
			*(fiberTable(3,2)->value) << cgicc::div(thisDDU->getChamber(iFiber)->name())
				.set("class","red");
			fibersWithErrors |= 1<<iFiber;
		}
	}

	*(fiberTable(4,0)->value) << "DMB error";
	dduValue = thisDDU->readDMBError();
	*(fiberTable(4,1)->value) << showbase << hex << dduValue;
	if (dduValue) fiberTable(4,1)->setClass("bad");
	else fiberTable(4,1)->setClass("ok");
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (dduValue & (1<<iFiber)) {
			*(fiberTable(4,2)->value) << cgicc::div(thisDDU->getChamber(iFiber)->name())
				.set("class","red");
			fibersWithErrors |= 1<<iFiber;
		}
	}

	*(fiberTable(5,0)->value) << "TMB error";
	dduValue = thisDDU->readTMBError();
	*(fiberTable(5,1)->value) << showbase << hex << dduValue;
	if (dduValue) fiberTable(5,1)->setClass("bad");
	else fiberTable(5,1)->setClass("ok");
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (dduValue & (1<<iFiber)) {
			*(fiberTable(5,2)->value) << cgicc::div(thisDDU->getChamber(iFiber)->name())
				.set("class","red");
			fibersWithErrors |= 1<<iFiber;
		}
	}

	*(fiberTable(6,0)->value) << "ALCT error";
	dduValue = thisDDU->readALCTError();
	*(fiberTable(6,1)->value) << showbase << hex << dduValue;
	if (dduValue) fiberTable(6,1)->setClass("bad");
	else fiberTable(6,1)->setClass("ok");
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (dduValue & (1<<iFiber)) {
			*(fiberTable(6,2)->value) << cgicc::div(thisDDU->getChamber(iFiber)->name())
				.set("class","red");
			fibersWithErrors |= 1<<iFiber;
		}
	}

	*(fiberTable(7,0)->value) << "Lost-in-event error";
	dduValue = thisDDU->readLIEError();
	*(fiberTable(7,1)->value) << showbase << hex << dduValue;
	if (dduValue) fiberTable(7,1)->setClass("bad");
	else fiberTable(7,1)->setClass("ok");
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (dduValue & (1<<iFiber)) {
			*(fiberTable(7,2)->value) << cgicc::div(thisDDU->getChamber(iFiber)->name())
				.set("class","red");
			fibersWithErrors |= 1<<iFiber;
		}
	}

	*out << fiberTable.printSummary() << endl;

	// Display only if there are errors.
	if (fiberTable.countClass("ok") + fiberTable.countClass("none") == fiberTable.countRows()) fiberTable.setHidden(true);
	*out << fiberTable.toHTML() << endl;

	*out << cgicc::fieldset() << endl;

	// Clever trick to help with formating the cut-and-paste.
	*out << cgicc::br()
		.set("style","display: none") << endl;




	// Display InRD status information
	*out << cgicc::fieldset()
		.set("class","normal") << endl;
	*out << cgicc::div()
		.set("class","legend") << endl;
	*out << cgicc::a("&plusmn;")
		.set("class","openclose")
		.set("onClick","javascript:toggle('inrdTable')") << endl;
	*out << "4-bit InRD Information" << cgicc::div() << endl;


	DataTable inrdTable("inrdTable");

	inrdTable.addColumn("Register");
	inrdTable.addColumn("Value");
	inrdTable.addColumn("Decoded Chambers");

	*(inrdTable(0,0)->value) << "Stuck data error";
	dduValue = thisDDU->checkFIFO(1);
	*(inrdTable(0,1)->value) << showbase << hex << ((dduValue >> 12) & 0xF);
	if ((dduValue >> 12) & 0xF) inrdTable(0,1)->setClass("bad");
	else inrdTable(0,1)->setClass("ok");

	*(inrdTable(1,0)->value) << "Fiber or FIFO connection error";
	//dduValue = thisDDU->checkFIFO(1);
	*(inrdTable(1,1)->value) << showbase << hex << ((dduValue >> 8) & 0xF);
	if ((dduValue >> 8) & 0xF) inrdTable(1,1)->setClass("bad");
	else inrdTable(1,1)->setClass("ok");

	*(inrdTable(2,0)->value) << "L1A mismatch";
	//dduValue = thisDDU->checkFIFO(1);
	*(inrdTable(2,1)->value) << showbase << hex << ((dduValue >> 4) & 0xF);
	if ((dduValue >> 4) & 0xF) inrdTable(2,1)->setClass("warning");
	else inrdTable(2,1)->setClass("ok");

	*(inrdTable(3,0)->value) << "InRD with active fiber";
	//dduValue = thisDDU->checkFIFO(1);
	*(inrdTable(3,1)->value) << showbase << hex << ((dduValue >> 4) & 0xF);
	inrdTable(3,1)->setClass("none");

	*(inrdTable(4,0)->value) << "Active ext. FIFO empty";
	dduValue = thisDDU->checkFIFO(2);
	*(inrdTable(4,1)->value) << showbase << hex << ((dduValue >> 10) & 0xF);
	inrdTable(4,1)->setClass("none");

	*(inrdTable(5,0)->value) << "InRD near full warning";
	//dduValue = thisDDU->checkFIFO(2);
	*(inrdTable(5,1)->value) << showbase << hex << ((dduValue >> 4) & 0xF);
	if ((dduValue >> 4) & 0xF) inrdTable(5,1)->setClass("warning");
	else inrdTable(5,1)->setClass("ok");

	*(inrdTable(6,0)->value) << "Ext. FIFO almost-full";
	//dduValue = thisDDU->checkFIFO(2);
	*(inrdTable(6,1)->value) << showbase << hex << ((dduValue) & 0xF);
	if ((dduValue) & 0xF) inrdTable(6,1)->setClass("questionable");
	else inrdTable(6,1)->setClass("ok");

	*(inrdTable(7,0)->value) << "Special decode bits";
	//dduValue = thisDDU->checkFIFO(2);
	*(inrdTable(7,1)->value) << showbase << hex << ((dduValue >> 8) & 0x43);
	if ((dduValue >> 8) & 0x81) inrdTable(7,1)->setClass("warning");
	else inrdTable(7,1)->setClass("ok");
	dduComments = debugger->FIFO2((dduValue >> 8) & 0x43);
	for (std::map<string,string>::iterator iComment = dduComments.begin();
		iComment != dduComments.end();
		iComment++) {
		*(inrdTable(7,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}

	*(inrdTable(8,0)->value) << "Timeout-EndBusy";
	dduValue = thisDDU->checkFIFO(3);
	*(inrdTable(8,1)->value) << showbase << hex << ((dduValue >> 12) & 0xF);
	if ((dduValue >> 12) & 0xF) inrdTable(8,1)->setClass("bad");
	else inrdTable(8,1)->setClass("ok");

	*(inrdTable(9,0)->value) << "Timeout-EndWait";
	//dduValue = thisDDU->checkFIFO(2);
	*(inrdTable(9,1)->value) << showbase << hex << ((dduValue >> 8) & 0xF);
	if ((dduValue >> 8) & 0xF) inrdTable(9,1)->setClass("warning");
	else inrdTable(9,1)->setClass("ok");

	*(inrdTable(10,0)->value) << "Timeout-Start";
	//dduValue = thisDDU->checkFIFO(2);
	*(inrdTable(10,1)->value) << showbase << hex << ((dduValue >> 4) & 0xF);
	if ((dduValue >> 4) & 0xF) inrdTable(10,1)->setClass("warning");
	else inrdTable(10,1)->setClass("ok");

	*(inrdTable(11,0)->value) << "Lost-in-data error";
	//dduValue = thisDDU->checkFIFO(2);
	*(inrdTable(11,1)->value) << showbase << hex << ((dduValue) & 0xF);
	if ((dduValue) & 0xF) inrdTable(11,1)->setClass("bad");
	else inrdTable(11,1)->setClass("ok");

	*(inrdTable(12,0)->value) << "Raw ext. FIFO empty";
	dduValue = thisDDU->readFFError();
	*(inrdTable(12,1)->value) << showbase << hex << ((dduValue >> 10) & 0xF);
	inrdTable(12,1)->setClass("none");

	*(inrdTable(13,0)->value) << "InRD FIFO full";
	//dduValue = thisDDU->readFFError(2);
	*(inrdTable(13,1)->value) << showbase << hex << ((dduValue >> 4) & 0xF);
	if ((dduValue >> 4) & 0xF) inrdTable(13,1)->setClass("bad");
	else inrdTable(13,1)->setClass("ok");

	*(inrdTable(14,0)->value) << "Ext. FIFO full";
	//dduValue = thisDDU->readFFError(2);
	*(inrdTable(14,1)->value) << showbase << hex << ((dduValue) & 0xF);
	if ((dduValue) & 0xF) inrdTable(14,1)->setClass("bad");
	else inrdTable(14,1)->setClass("ok");

	*(inrdTable(15,0)->value) << "Special decode bits";
	//dduValue = thisDDU->readFFError(2);
	*(inrdTable(15,1)->value) << showbase << hex << ((dduValue >> 8) & 0x43);
	if ((dduValue >> 8) & 0x1) inrdTable(15,1)->setClass("bad");
	else inrdTable(15,1)->setClass("ok");
	dduComments = debugger->FFError((dduValue >> 8) & 0x43);
	for (std::map<string,string>::iterator iComment = dduComments.begin();
		iComment != dduComments.end();
		iComment++) {
		*(inrdTable(15,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}

	*(inrdTable(16,0)->value) << "InRD hard error";
	dduValue = thisDDU->readInRDStat();
	*(inrdTable(16,1)->value) << showbase << hex << ((dduValue >> 12) & 0xF);
	if ((dduValue >> 12) & 0xF) inrdTable(16,1)->setClass("bad");
	else inrdTable(16,1)->setClass("ok");

	*(inrdTable(17,0)->value) << "InRD sync error";
	//dduValue = thisDDU->checkFIFO(2);
	*(inrdTable(17,1)->value) << showbase << hex << ((dduValue >> 8) & 0xF);
	if ((dduValue >> 8) & 0xF) inrdTable(17,1)->setClass("warning");
	else inrdTable(17,1)->setClass("ok");

	*(inrdTable(18,0)->value) << "InRD single event error";
	//dduValue = thisDDU->checkFIFO(2);
	*(inrdTable(18,1)->value) << showbase << hex << ((dduValue >> 4) & 0xF);
	if ((dduValue >> 4) & 0xF) inrdTable(18,1)->setClass("questionable");
	else inrdTable(18,1)->setClass("ok");

	*(inrdTable(19,0)->value) << "InRD timeout error";
	//dduValue = thisDDU->checkFIFO(2);
	*(inrdTable(19,1)->value) << showbase << hex << ((dduValue) & 0xF);
	if ((dduValue) & 0xF) inrdTable(19,1)->setClass("bad");
	else inrdTable(19,1)->setClass("ok");

	*(inrdTable(20,0)->value) << "InRD multiple transmit errors";
	dduValue = thisDDU->ddu_InC_Hist();
	*(inrdTable(20,1)->value) << showbase << hex << ((dduValue) & 0xF);
	if ((dduValue) & 0xF) inrdTable(20,1)->setClass("bad");
	else inrdTable(20,1)->setClass("ok");

	*(inrdTable(21,0)->value) << "Special decode bits";
	//dduValue = thisDDU->readFFError(2);
	*(inrdTable(21,1)->value) << showbase << hex << ((dduValue) & 0xFFF);
	if ((dduValue) & 0xC00) inrdTable(21,1)->setClass("bad");
	else if ((dduValue) & 0x2DF) inrdTable(21,1)->setClass("warning");
	else inrdTable(21,1)->setClass("ok");
	dduComments = debugger->FFError((dduValue) & 0xFFF);
	for (std::map<string,string>::iterator iComment = dduComments.begin();
		iComment != dduComments.end();
		iComment++) {
		*(inrdTable(21,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}

	*out << inrdTable.printSummary() << endl;

	// Display only if there are errors.
	if (inrdTable.countClass("ok") + inrdTable.countClass("none") == inrdTable.countRows()) inrdTable.setHidden(true);
	*out << inrdTable.toHTML() << endl;

	*out << cgicc::fieldset() << endl;

	// Clever trick to help with formating the cut-and-paste.
	*out << cgicc::br()
		.set("style","display: none") << endl;



	// Display miscellanious DDU status information
	*out << cgicc::fieldset()
		.set("class","normal") << endl;
	*out << cgicc::div()
		.set("class","legend") << endl;
	*out << cgicc::a("&plusmn;")
		.set("class","openclose")
		.set("onClick","javascript:toggle('otherTable')") << endl;
	*out << "Other DDU Information" << cgicc::div() << endl;

	DataTable otherTable("otherTable");

	otherTable.addColumn("Register");
	otherTable.addColumn("Value");
	otherTable.addColumn("Decoded Status");

	*(otherTable(0,0)->value) << "DDU near full warning (8-bit)";
	dduValue = thisDDU->readWarnMon();
	*(otherTable(0,1)->value) << showbase << hex << ((dduValue) & 0xFF);
	if ((dduValue) & 0xFF) otherTable(0,1)->setClass("questionable");
	else otherTable(0,1)->setClass("ok");
	dduComments = debugger->WarnMon((dduValue) & 0xFF);
	for (std::map<string,string>::iterator iComment = dduComments.begin();
		iComment != dduComments.end();
		iComment++) {
		*(otherTable(0,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}

	*(otherTable(1,0)->value) << "DDU near full historical (8-bit)";
	//dduValue = thisDDU->readWarnMon();
	*(otherTable(1,1)->value) << showbase << hex << ((dduValue >> 8) & 0xFF);
	if ((dduValue >> 8) & 0xFF) otherTable(1,1)->setClass("questionable");
	else otherTable(1,1)->setClass("ok");
	dduComments = debugger->WarnMon((dduValue >> 8) & 0xFF);
	for (std::map<string,string>::iterator iComment = dduComments.begin();
		iComment != dduComments.end();
		iComment++) {
		*(otherTable(1,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}

	*(otherTable(2,0)->value) << "DDU L1A-to-start max process time";
	dduValue = thisDDU->readMaxTimeoutCount();
	*(otherTable(2,1)->value) << (((dduValue) & 0xFF) * 400.0) << " ns";
	otherTable(2,1)->setClass("none");

	*(otherTable(3,0)->value) << "DDU start-to-end max process time";
	//dduValue = thisDDU->readWarnMon();
	*(otherTable(3,1)->value) << showbase << hex << (((dduValue >> 8) & 0xFF) * 6.4) << " &mu;s";
	otherTable(3,1)->setClass("none");

	*out << otherTable.printSummary() << endl;

	// Display only if there are errors.
	if (otherTable.countClass("ok") + otherTable.countClass("none") == otherTable.countRows()) otherTable.setHidden(true);
	*out << otherTable.toHTML() << endl;

	*out << cgicc::fieldset() << endl;

	// Clever trick to help with formating the cut-and-paste.
	*out << cgicc::br()
		.set("style","display: none") << endl;



	thisCrate->vmeController()->CAEN_err_reset();
	// Display the big debugging information block
	*out << cgicc::fieldset()
		.set("class","fieldset") << endl;
	*out << cgicc::div("DDU Diagnostic Trap Decoding")
		.set("class","legend") << endl;

	if (debugTrapValid) {

		// Here it is.
		string bigComments = debugger->ddu_fpgatrap(thisDDU);

		ostringstream diagCode;
		diagCode << setfill('0');
		diagCode << hex;
		diagCode << setw(8) << thisDDU->fpga_lcode[5] << " ";
		diagCode << setw(8) << thisDDU->fpga_lcode[4] << " ";
		diagCode << setw(8) << thisDDU->fpga_lcode[3] << " ";
		diagCode << setw(8) << thisDDU->fpga_lcode[2] << " ";
		diagCode << setw(8) << thisDDU->fpga_lcode[1] << " ";
		diagCode << setw(8) << thisDDU->fpga_lcode[0];

		// Don't tell anybody, but a div with display: inline is just another
		//  name for a span.  I do this to solve a problem with cgicc and the
		//  way I handle InFPGAs later.  It's not needed at this point, but
		//  internal consistancy is always best.
		*out << cgicc::div("Trap value (192 bits): ")
			.set("style","font-weight: bold; display: inline;");

		*out << cgicc::div(diagCode.str())
			.set("style","display: inline;") << endl;

		*out << cgicc::div(bigComments)
			.set("style","width: 80%; white-space: pre; font-size: 10pt; margin: 10px auto 10px auto; border: 2px solid #666; padding: 2px; font-family: monospace;") << endl;

		*out << cgicc::div("The status registers are frozen in the trap only after an error occurs.  The values in the trap remain valid until a reset.")
			.set("style","font-size: 8pt;") << endl;

	} else {
		*out << cgicc::div("Diagnostic trap is only valid after a DDU error has been detected.")
			.set("style","color: #A00; font-size: 10pt;") << endl;
	}

	*out << cgicc::fieldset() << endl;

	// Clever trick to help with formating the cut-and-paste.
	*out << cgicc::br()
		.set("style","display: none") << endl;


	thisCrate->vmeController()->CAEN_err_reset();
	// Killfiber and xorbit read/set
	*out << cgicc::fieldset()
		.set("class","fieldset") << endl;
	*out << cgicc::div("DDU KillFiber Register")
		.set("class","legend") << endl;

	*out << cgicc::form()
		.set("method","GET")
		.set("action","/"+getApplicationDescriptor()->getURN()+"/DDUTextLoad") << endl;
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","val")
		.set("value","222") << endl;
	ostringstream dduVal;
	dduVal << cgiDDU;
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","ddu")
		.set("value",dduVal.str()) << endl;

	unsigned long int currentKillFiber = thisDDU->ddu_rdkillfiber();
	*out << cgicc::div("Current KillFiber register value: ")
		.set("style","font-weight: bold; display: inline;");
	*out << cgicc::div()
		.set("style","display: inline;");
	*out << showbase << hex << currentKillFiber << cgicc::div() << endl;
	*out << cgicc::br() << endl;
	*out << cgicc::div("New KillFiber register value (change this below): ")
		.set("style","font-weight: bold; display: inline; color: #090;");
	ostringstream kfValue;
	kfValue << hex << showbase << currentKillFiber;
	*out << cgicc::input()
		.set("type","text")
		.set("name","textdata")
		.set("value",kfValue.str())
		.set("size","8")
		.set("id","killFiber") << endl;

	*out << cgicc::input()
		.set("type","submit")
		.set("value","Load KillFiber Register") << endl;

	*out << cgicc::div("This is a volitile register, and will be reprogrammed on a hard reset.  If you wish to make this change perminent, change the configuration file (see the top of this page)")
		.set("style","font-size: 8pt; color: #900;") << endl;

	// A table for the first half of the form.
	*out << cgicc::table()
		.set("style","width: 100%; margin: 5px auto 5px auto; font-size: 8pt;") << endl;
	*out << cgicc::tr() << endl;
	for (unsigned int ifiber = 0; ifiber < 15; ifiber++) {
		Chamber *thisChamber = thisDDU->getChamber(ifiber);
		// Note:  there are no 4/2 chambers yet...
		if (thisChamber != 0 && (thisChamber->station < 4 || thisChamber->type < 2)) {
			if (fibersWithErrors & 1<<ifiber) {
				*out << cgicc::td(thisChamber->name())
					.set("class","bad")
					.set("style","border: 1px solid #000; width: 6%; font-weight: bold;") << endl;
			} else {
				*out << cgicc::td(thisChamber->name())
					.set("class","ok")
					.set("style","border: 1px solid #000; width: 6%; font-weight: bold;") << endl;
			}
		} else {
			*out << cgicc::td(thisChamber->name())
				.set("class","undefined")
				.set("style","border: 1px solid #000; width: 6%;") << endl;
		}
	}
	*out << cgicc::tr();

	*out << cgicc::tr() << endl;
	for (unsigned int ifiber = 0; ifiber < 15; ifiber++) {
		*out << cgicc::td() << endl;
		ostringstream radioName;
		radioName << "fiber" << ifiber;
		ostringstream swapCommand;
		swapCommand << "javascript:toggleBit('killFiber'," << ifiber << ");";
		if (currentKillFiber & 1<<ifiber) {
			*out << cgicc::input()
				.set("type","radio")
				.set("name",radioName.str())
				.set("value","1")
				.set("onChange",swapCommand.str())
				.set("checked","checked") << "Live" << cgicc::br() << endl;
			*out << cgicc::input()
				.set("type","radio")
				.set("name",radioName.str())
				.set("value","0")
				.set("onChange",swapCommand.str()) << "Killed" << endl;
		} else {
			*out << cgicc::input()
				.set("type","radio")
				.set("name",radioName.str())
				.set("value","1")
				.set("onChange",swapCommand.str()) << "Live" << cgicc::br() << endl;
			*out << cgicc::input()
				.set("type","radio")
				.set("name",radioName.str())
				.set("value","0")
				.set("onChange",swapCommand.str())
				.set("checked","checked") << "Killed" << endl;
		}
		*out << cgicc::td() << endl;
	}
	*out << cgicc::tr() << endl;
	*out << cgicc::table() << endl;

	// Now for some check-boxes.
	*out << cgicc::div()
		.set("style","font-size: 8pt;") << endl;
	if (currentKillFiber & (1<<15)) {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box15")
			.set("onChange","javascript:toggleBit('killFiber',15);")
			.set("checked","checked") << endl;
	} else {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box15")
			.set("onChange","javascript:toggleBit('killFiber',15);") << endl;
	}
	*out << "Force all DDU checks" << cgicc::div() << endl;

	*out << cgicc::div()
		.set("style","font-size: 8pt;") << endl;
	if (currentKillFiber & (1<<16)) {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box16")
			.set("onChange","javascript:toggleBit('killFiber',16);")
			.set("checked","checked") << endl;
	} else {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box16")
			.set("onChange","javascript:toggleBit('killFiber',16);") << endl;
	}
	*out << "Force ALCT checks" << cgicc::div() << endl;

	*out << cgicc::div()
		.set("style","font-size: 8pt;") << endl;
	if (currentKillFiber & (1<<17)) {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box17")
			.set("onChange","javascript:toggleBit('killFiber',17);")
			.set("checked","checked") << endl;
	} else {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box17")
			.set("onChange","javascript:toggleBit('killFiber',17);") << endl;
	}
	*out << "Force TMB checks" << cgicc::div() << endl;

	*out << cgicc::div()
		.set("style","font-size: 8pt;") << endl;
	if (currentKillFiber & (1<<18)) {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box18")
			.set("onChange","javascript:toggleBit('killFiber',18);")
			.set("checked","checked") << endl;
	} else {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box18")
			.set("onChange","javascript:toggleBit('killFiber',18);") << endl;
	}
	*out << "Force CFEB checks (enable DAV checks)" << cgicc::div() << endl;

	*out << cgicc::div()
		.set("style","font-size: 8pt;") << endl;
	if (currentKillFiber & (1<<19)) {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box19")
			.set("onChange","javascript:toggleBit('killFiber',19);")
			.set("checked","checked") << endl;
	} else {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box19")
			.set("onChange","javascript:toggleBit('killFiber',19);") << endl;
	}
	*out << "Force normal DDU checks (off enables only SP/TF checks)" << cgicc::div() << endl;

	*out << cgicc::form() << endl;

	*out << cgicc::fieldset() << endl;


	thisCrate->vmeController()->CAEN_err_reset();
	*out << cgicc::fieldset()
		.set("class","fieldset") << endl;
	*out << cgicc::div("CSC Board Occupancies and Percentages")
		.set("class","legend") << endl;

	// Pick up the occupancies.
	thisCrate->vmeController()->CAEN_err_reset();

	// Now we grab what we want.
	unsigned long int scalar = thisDDU->ddu_rdscaler();

	// Make us a DataTable!
	DataTable occuTable("occuTable");

	occuTable.addColumn("Fiber Input");
	occuTable.addColumn("Chamber");
	occuTable.addBreak();
	occuTable.addColumn("DMB<sup>*</sup>");
	occuTable.addColumn("ALCT");
	occuTable.addColumn("TMB");
	occuTable.addColumn("CFEB");

	// PGK It turns out that this is an automatically shifting register.
	//  If you read from it 60 times, you get 15 sets of 4 values, all
	//  different.
	//  This means I need to do this 15 times per board.
	for (unsigned int ifiber = 0; ifiber < 15; ifiber++) {
		thisDDU->ddu_occmon();
		unsigned long int DMBval = thisDDU->fpga_lcode[0] & 0x0fffffff;
		unsigned long int ALCTval = thisDDU->fpga_lcode[1] & 0x0fffffff;
		unsigned long int TMBval = thisDDU->fpga_lcode[2] & 0x0fffffff;
		unsigned long int CFEBval = thisDDU->fpga_lcode[3] & 0x0fffffff;

		(*occuTable(ifiber,0)->value) << ifiber;
		Chamber *thisChamber = thisDDU->getChamber(ifiber);
		(*occuTable(ifiber,1)->value) << thisChamber->name();

		if (thisChamber != 0 && (thisChamber->station < 4 || thisChamber->type < 2)) {
			if (fibersWithErrors & (1 << ifiber)) occuTable(ifiber,1)->setClass("bad");
			else occuTable(ifiber,1)->setClass("ok");

			(*occuTable(ifiber,2)->value) << DMBval;
			(*occuTable(ifiber,2)->value) << "<br />" << setprecision(3) << (scalar ? DMBval*100./scalar : 0) << "%";

			(*occuTable(ifiber,3)->value) << ALCTval;
			(*occuTable(ifiber,3)->value) << "<br />" << setprecision(3) << (DMBval ? ALCTval*100./DMBval : 0) << "%";

			(*occuTable(ifiber,4)->value) << TMBval;
			(*occuTable(ifiber,4)->value) << "<br />" << setprecision(3) << (DMBval ? TMBval*100./DMBval : 0) << "%";

			(*occuTable(ifiber,5)->value) << CFEBval;
			(*occuTable(ifiber,5)->value) << "<br />" << setprecision(3) << (DMBval ? CFEBval*100./DMBval : 0) << "%";

		} else {
			occuTable(ifiber,1)->setClass("undefined");
			(*occuTable(ifiber,2)->value) << "N/A";
			(*occuTable(ifiber,3)->value) << "N/A";
			(*occuTable(ifiber,4)->value) << "N/A";
			(*occuTable(ifiber,5)->value) << "N/A";
		}
	}

	*out << occuTable.toHTML() << endl;

	*out << cgicc::div("<sup>*</sup>The DMB percentages are relative to #L1As; other board percentages are relative to #LCTxL1A hits on the CSC.")
		.set("style","font-size: 8pt;") << endl;
	*out << cgicc::div()
		.set("style","font-size: 8pt;") << endl;
	*out << cgicc::span("Red chamber labels")
		.set("class","bad") << endl;
	*out << cgicc::span(" denote chambers with an error.  The tables above will help diagnose the problem.") << endl;
	*out << cgicc::div() << endl;

	*out << cgicc::fieldset() << endl;

	*out << cgicc::hr() << endl;


	// Some expert-only commands, like sending fake L1As.
	*out << cgicc::fieldset()
		.set("class","expert") << endl;
	*out << cgicc::div()
		.set("class","legend") << endl;
	*out << cgicc::a("&plusmn;")
		.set("class","openclose")
		.set("onClick","javascript:toggle('resetContainer')") << endl;
	*out << "DDU Commands (Expert Only)" << cgicc::div() << endl;

	*out << cgicc::span()
		.set("id","resetContainer")
		.set("style","display: none") << endl;

	// Don't tell anyone, but it's the same form as used in the killfiber and
	//  bxorbit loading.  The same variables are used here as well.
	string ddutextload = "/"+getApplicationDescriptor()->getURN()+"/DDUTextLoad";
	ostringstream dduStream;
	dduStream << cgiDDU;
	*out << cgicc::form()
		.set("method","GET")
		.set("action",ddutextload) << endl;
	*out << cgicc::input()
		.set("name","ddu")
		.set("type","hidden")
		.set("value",dduStream.str()) << endl;
	*out << cgicc::input()
		.set("name","val")
		.set("type","hidden")
		.set("value","226") << endl;
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","textdata")
		.set("size","10")
		.set("ENCTYPE","multipart/form-data")
		.set("value","") << endl;
	*out << cgicc::input()
		.set("type","submit")
		.set("value","Send one fake DDU L1A via VME") << endl;
	*out << cgicc::form() << endl;

	*out << cgicc::form()
		.set("method","GET")
		.set("action",ddutextload) << endl;
	*out << cgicc::input()
		.set("name","ddu")
		.set("type","hidden")
		.set("value",dduStream.str()) << endl;
	*out << cgicc::input()
		.set("name","val")
		.set("type","hidden")
		.set("value","227") << endl;
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","textdata")
		.set("size","10")
		.set("ENCTYPE","multipart/form-data")
		.set("value","") << endl;
	*out << cgicc::input()
		.set("type","submit")
		.set("value","Toggle DDU CFEB-calibration-pulse==L1A feature (default false)") << endl;
	*out << cgicc::form() << endl;

	*out << cgicc::form()
		.set("method","GET")
		.set("action",ddutextload) << endl;
	*out << cgicc::input()
		.set("name","ddu")
		.set("type","hidden")
		.set("value",dduStream.str()) << endl;
	*out << cgicc::input()
		.set("name","val")
		.set("type","hidden")
		.set("value","228") << endl;
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","textdata")
		.set("size","10")
		.set("ENCTYPE","multipart/form-data")
		.set("value","") << endl;
	*out << cgicc::input()
		.set("type","submit")
		.set("value","Reset DDU via VME sync reset") << endl;
	*out << cgicc::form() << endl;

	*out << cgicc::span() << endl;

	*out << cgicc::fieldset() << endl;

	*out << Footer() << endl;
/*
	thisDDU = dduVector[cgiDDU];

	printf(" enter: DDUFpga \n");
	//cgicc::Cgicc cgi(in);
	const CgiEnvironment& env = cgi.getEnvironment();
	string crateStr = env.getQueryString() ;
	cout << crateStr << endl ;
	//cgicc::form_iterator name = cgi.getElement("ddu");
	if(name != cgi.getElements().end()) {
		ddu = cgi["ddu"]->getIntegerValue();
		DDU_ = ddu;
	}else{
		ddu=DDU_;
	}
	thisDDU = dduVector[ddu];
	*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << endl;
	*out << cgicc::html().set("lang", "en").set("dir","ltr") << endl;
	*out << cgicc::title("DDUFPGA Web Form") << endl;
	*out << body().set("background","/tmp/bgndcms.jpg") << endl;
	*out << cgicc::div().set("style","font-size: 16pt; font-weight: bold; color: #D00; width: 400px; margin-left: auto; margin-right: auto; text-align: center;") << "Crate " << thisCrate->number() << " Selected" << cgicc::div() << endl;

	char buf[300],buf2[300],buf3[300],buf4[200];
	char buf5[300],buf6[10],buf7[300],buf8[10];
	unsigned long int ndmb,nalct,ntmb,ncfeb;
	int j,ddu,icrit,icond,icond2,icond3,icond4;
	unsigned long int stat=0,live_fiber=0;
	sprintf(buf,"DDU Control FPGA, VME Slot %d",thisDDU->slot());
	//    *out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;") << endl;
	//    *out << cgicc::legend(buf).set("style","color:blue")  << endl ;
	*out << "<h2 align=center><font color=blue>" << buf << "</font></h2>" << endl;


	for(int i=200;i<230;i++){
		thisDDU->infpga_shift0=0x0000;
		thisCrate->vmeController()->CAEN_err_reset();
		sprintf(buf3," ");
		sprintf(buf4," ");
		sprintf(buf5," ");
		sprintf(buf6," ");
		sprintf(buf7," ");
		sprintf(buf8," ");
		icond=0;
		icond2=0;
		icond3=0;
		icond4=0;
		printf(" LOOP: %d \n",i);
		thisDDU->ddu_shift0=0x0000;
		if(i==200){
			thisDDU->ddu_rd_boardID();
			sprintf(buf,"16-bit DDU Board ID:");
			sprintf(buf2," %04Xh ",thisDDU->ddu_code0);
		}
		if(i==201){
			thisDDU->ddu_fpgastat();
			sprintf(buf,"32-bit DDU Control FPGA Status:");
			stat=((0xffff&thisDDU->ddu_code1)<<16)|(0xffff&thisDDU->ddu_code0);
			//	stat=0xffffffff;
			sprintf(buf2," %08lX ",stat);
			icrit=0;
			if((0x00008000&stat)){
				icrit=1;
				icond=2;
			} else if((0xDE4F4BFF&stat)>0)icond=1;
		}
		if(i==202){
			thisDDU->ddu_rdostat();
			sprintf(buf,"16-bit DDU Output Status:");
			sprintf(buf2," %04X ",thisDDU->ddu_code0);
			stat=0xffff&thisDDU->ddu_code0;
			//	stat=0xffff;
			if((0x00000080&stat)){
				icond=2;
			} else if((0x00004000&stat)>0)icond=1;
		}
		if(i==203){
			thisDDU->ddu_rdscaler();
			sprintf(buf,"DDU L1 Event Scaler, bits [23-0]:");
			sprintf(buf2," %02X%04Xh ",thisDDU->ddu_code1,thisDDU->ddu_code0);
			stat=((thisDDU->ddu_code1)<<16)+thisDDU->ddu_code0;
			sprintf(buf4," = %8lu Dec",stat&0x00ffffff);
		}
		if(i==204){
			thisDDU->ddu_rderareg();
			sprintf(buf,"Error bus A Register bits [15-0]:");
			sprintf(buf2," %04X ",thisDDU->ddu_code0);
			stat=0xffff&thisDDU->ddu_code0;
			//	stat=0xffff;
			if((0x0000C00C&stat)){
				icond=2;
			} else if((0x000001C8&stat)>0)icond=1;
		}
		if(i==205){
			thisDDU->ddu_rderbreg();
			sprintf(buf,"Error bus B Register bits [15-0]:");
			sprintf(buf2," %04X ",thisDDU->ddu_code0);
			stat=0xffff&thisDDU->ddu_code0;
			//	stat=0xffff;
			if((0x00000011&stat)){
				icond=2;
			} else if((0x0000D08E&stat)>0)icond=1;
		}
		if(i==206){
			thisDDU->ddu_rdercreg();
			sprintf(buf,"Error bus C Register bits [15-0]:");
			sprintf(buf2," %04X ",thisDDU->ddu_code0);
			stat=0xffff&thisDDU->ddu_code0;
			//	stat=0xffff;
			// if((0x00008000&stat)>0)icond=1;
			if((0x0000bfbf&stat)>0)icond=1;
		}
		if(i==207){
			*out << br() << " <font color=blue> Fiber Registers below flag which CSCs experienced each condition</font>" << br() << endl;
			thisDDU->ddu_pdmblive();
			sprintf(buf,"First Event DMBLIVE, Fiber[14-0]:");
			sprintf(buf2," %04X ",thisDDU->ddu_code0);
			live_fiber=0x7fff&thisDDU->ddu_code0;
		}
		if(i==208){
			thisDDU->ddu_dmblive();
			sprintf(buf,"Latest Event DMBLIVE, Fiber[14-0]:");
			sprintf(buf2," %04X ",thisDDU->ddu_code0);
		}
		if(i==209){
			thisDDU->ddu_rdcrcerr();
			sprintf(buf,"CRC Error, Fiber[14-0]:");
			sprintf(buf2," %04X ",thisDDU->ddu_code0);
		}
		if(i==210){
			thisDDU->ddu_rdxmiterr();
			sprintf(buf,"Data Transmit Error, Fiber[14-0]:");
			sprintf(buf2," %04X ",thisDDU->ddu_code0);
			fibersWithErrors |= thisDDU->ddu_code0;
		}
		if(i==211){
			thisDDU->ddu_rddmberr();
			sprintf(buf,"DMB Error, Fiber[14-0]:");
			sprintf(buf2," %04X ",thisDDU->ddu_code0);
			fibersWithErrors |= thisDDU->ddu_code0;
		}
		if(i==212){
			thisDDU->ddu_rdtmberr();
			sprintf(buf,"TMB Error, Fiber[14-0]:");
			sprintf(buf2," %04X ",thisDDU->ddu_code0);
			fibersWithErrors |= thisDDU->ddu_code0;
		}
		if(i==213){
			thisDDU->ddu_rdalcterr();
			sprintf(buf,"ALCT Error, Fiber[14-0]:");
			sprintf(buf2," %04X ",thisDDU->ddu_code0);
			fibersWithErrors |= thisDDU->ddu_code0;
		}
		if(i==214){
			thisDDU->ddu_rdlieerr();
			sprintf(buf,"Lost-In-Event Error, Fiber[14-0]:");
			sprintf(buf2," %04X ",thisDDU->ddu_code0);
			fibersWithErrors |= thisDDU->ddu_code0;
		}
		if(i==215){
			*out << br() << " <font color=blue> 4-bit Registers below flag which InRD unit experienced each condition, plus" << br() << " &nbsp &nbsp some individual bit Decoding</font>" << br() << endl;
			thisDDU->ddu_checkFIFOa();
			// sprintf(buf,"FIFO-A Status [15-0]:");
			// sprintf(buf2," %04X ",thisDDU->ddu_code0);
			sprintf(buf,"Stuck Data Error occurred [3-0]:");
			sprintf(buf2," %01Xh ",(0xF000&thisDDU->ddu_code0)>>12);
			if(0xF000&thisDDU->ddu_code0)icond=2;
			sprintf(buf3," &nbsp &nbsp Fiber or FIFO Connection Error occurred [3-0]:");
			sprintf(buf4," %01Xh ",(0x0F00&thisDDU->ddu_code0)>>8);
			if(0x0F00&thisDDU->ddu_code0)icond2=2;
			sprintf(buf5,"L1A Mismatch occurred [3-0]:");
			sprintf(buf6," %01Xh ",(0x00F0&thisDDU->ddu_code0)>>4);
			if(0x00F0&thisDDU->ddu_code0)icond3=1;
			sprintf(buf7," &nbsp &nbsp InRD with active fiber [3-0]:");
			sprintf(buf8," %01Xh ",(0x000F&thisDDU->ddu_code0));
		}
		if(i==216){
			thisDDU->ddu_checkFIFOb();
			// sprintf(buf,"FIFO-B Status [15-0]:");
			// sprintf(buf2," %04X ",thisDDU->ddu_code0);
			sprintf(buf,"Active Ext.FIFO Empty [3-0]:");
			sprintf(buf2," %01Xh ",(0x3C00&thisDDU->ddu_code0)>>10);
			sprintf(buf3," &nbsp &nbsp InRD Near Full Warn occurred [3-0]:");
			sprintf(buf4," %01Xh ",(0x00F0&thisDDU->ddu_code0)>>4);
			if(0x00F0&thisDDU->ddu_code0)icond2=3;
			sprintf(buf5,"Ext.FIFO Almost-Full occurred [3-0]:");
			sprintf(buf6," %01Xh ",(0x000F&thisDDU->ddu_code0));
			if(0x000F&thisDDU->ddu_code0)icond3=3;
			sprintf(buf7," &nbsp &nbsp special decode bits:");
			sprintf(buf8," %02Xh ",(0x4300&thisDDU->ddu_code0)>>8);
			stat=(0xC300&thisDDU->ddu_code0)>>8;
			if(0x0081&stat)icond4=1;
			//	stat=0x00ff;
		}
		if(i==217){
			thisDDU->ddu_checkFIFOc();
			// sprintf(buf,"FIFO-C Status [15-0]:");
			// sprintf(buf2," %04X ",thisDDU->ddu_code0);
			sprintf(buf,"Timeout-EndBusy occurred [3-0]:");
			sprintf(buf2," %01Xh ",(0xF000&thisDDU->ddu_code0)>>12);
			if(0xF000&thisDDU->ddu_code0)icond=2;
			sprintf(buf3," &nbsp &nbsp Timeout-EndWait occurred [3-0]:");
			sprintf(buf4," %01Xh ",(0x0F00&thisDDU->ddu_code0)>>8);
			if(0x0F00&thisDDU->ddu_code0)icond2=2;
			sprintf(buf5,"Timeout-Start occurred [3-0]:");
			sprintf(buf6," %01Xh ",(0x00F0&thisDDU->ddu_code0)>>4);
			if(0x00F0&thisDDU->ddu_code0)icond3=2;
			sprintf(buf7," &nbsp &nbsp Lost In Data Error occurred [3-0]:");
			sprintf(buf8," %01Xh ",(0x000F&thisDDU->ddu_code0));
			if(0x000F&thisDDU->ddu_code0)icond4=2;
		}
		if(i==218){
			thisDDU->ddu_rdfferr();
			// sprintf(buf,"FIFO Full Register bits [14-0]:");
			// sprintf(buf2," %04X ",thisDDU->ddu_code0);
			sprintf(buf,"Raw Ext.FIFO Empty [3-0]:");
			sprintf(buf2," %01Xh ",(0x3C00&thisDDU->ddu_code0)>>10);
			sprintf(buf3," &nbsp &nbsp InRD FIFO Full occurred [3-0]:");
			sprintf(buf4," %01Xh ",(0x00F0&thisDDU->ddu_code0)>>4);
			if(0x00F0&thisDDU->ddu_code0)icond2=2;
			sprintf(buf5,"Ext.FIFO Full occurred [3-0]:");
			sprintf(buf6," %01Xh ",(0x000F&thisDDU->ddu_code0));
			if(0x000F&thisDDU->ddu_code0)icond3=2;
			sprintf(buf7," &nbsp &nbsp special decode bits:");
			sprintf(buf8," %02Xh ",(0x4300&thisDDU->ddu_code0)>>8);
			stat=(0x4300&thisDDU->ddu_code0)>>8;
			//	stat=0x00ff;
			if(0x0001&stat)icond4=2;
		}
		if(i==219){
			thisDDU->ddu_InRDstat();
			// sprintf(buf,"InRDctrl Status [15-0]:");
			// sprintf(buf2," %04X ",thisDDU->ddu_code0);
			sprintf(buf,"InRD Hard Error occurred [3-0]:");
			sprintf(buf2," %01Xh ",(0xF000&thisDDU->ddu_code0)>>12);
			if(0xF000&thisDDU->ddu_code0)icond=2;
			sprintf(buf3," &nbsp &nbsp InRD Sync Error occurred [3-0]:");
			sprintf(buf4," %01Xh ",(0x0F00&thisDDU->ddu_code0)>>8);
			if(0x0F00&thisDDU->ddu_code0)icond2=2;
			sprintf(buf5,"InRD Single Event Error occurred [3-0]:");
			sprintf(buf6," %01Xh ",(0x00F0&thisDDU->ddu_code0)>>4);
			sprintf(buf7," &nbsp &nbsp InRD Timeout Error occurred [3-0]:");
			sprintf(buf8," %01Xh ",(0x000F&thisDDU->ddu_code0));
			if(0x000F&thisDDU->ddu_code0)icond4=2;
		}
		if(i==220){
			thisDDU->ddu_InC_Hist();
			// sprintf(buf,"InRDctrl MxmitErr Reg [15-12] & C-code History [8-0]:");
			// sprintf(buf2," %04X ",thisDDU->ddu_code0);
			sprintf(buf,"InRD Mutliple Transmit Errors occurred [3-0]:");
			sprintf(buf2," %01Xh ",(0xF000&thisDDU->ddu_code0)>>12);
			if(0xF000&thisDDU->ddu_code0)icond=2;

			sprintf(buf3," &nbsp &nbsp special decode bits:");
			sprintf(buf4," %03Xh ",(0x0FFF&thisDDU->ddu_code0));
			stat=(0x0FFF&thisDDU->ddu_code0);
			//	stat=0x0fff;
			if(0x0C00&stat)icond2=1;
			if(0x02DF&stat)icond2=2;
		}
		if(i==221){
			*out << br() << endl;
			thisDDU->ddu_rd_WarnMon();
			sprintf(buf,"8-bit DDU Near Full Warning, current:");
			sprintf(buf2," %02Xh ",(0x00FF&thisDDU->ddu_code0));
			if(0x00FF&thisDDU->ddu_code0)icond=3;
			sprintf(buf3," &nbsp &nbsp historical:");
			sprintf(buf4," %02Xh ",(0xFF00&thisDDU->ddu_code0)>>8);
			if(0xFF00&thisDDU->ddu_code0)icond2=3;
		}
		if(i==222){
			*out << br() << endl;
			thisDDU->ddu_rdkillfiber();
			sprintf(buf,"KillFiber Register bits [19-0] (0=dead, 1=alive):");
			sprintf(buf2," %01X%04X ",thisDDU->ddu_code1,thisDDU->ddu_code0);
			stat=((0x000f&thisDDU->ddu_code1)<<16)|(0xFFFF&thisDDU->ddu_code0);
		}
		if(i==223){
			thisDDU->ddu_rdbxorbit();
			sprintf(buf,"BX_Orbit Register bits [11-0]:");
			sprintf(buf2," %03Xh ",thisDDU->ddu_code0);
		}
		if(i==224){
			if(icrit>0){
				thisDDU->ddu_fpgatrap();
				*out << br() << " <font color=blue> Diagnostic data only valid after Critical Error, traps conditions at that instant</font>" << br() << endl;
				sprintf(buf,"DDU Control diagnostic trap:");
				sprintf(buf2," %08lX %08lX %08lX %08lX %08lX %08lX",thisDDU->fpga_lcode[5],thisDDU->fpga_lcode[4],thisDDU->fpga_lcode[3],thisDDU->fpga_lcode[2],thisDDU->fpga_lcode[1],thisDDU->fpga_lcode[0]);
			}else{
				sprintf(buf," ");
				sprintf(buf2," ");
				thisDDU->ddu_shift0=0xFACE;
			}
		}
		if(i==225){
			if(icrit>=0){
				thisDDU->ddu_maxTimeCount();
			//	  sprintf(buf,"DDU Control max time counted, EndTimeout <font size=-1>(1 unit = 6.4 usec)</font>:");
			//	  sprintf(buf2," %02Xh ",(0xff00&thisDDU->ddu_code0)>>8);
				sprintf(buf,"DDU Control max process time, L1A-to-Start <font size=-1>(1 unit = 400 ns)</font>:");
				sprintf(buf2," %02Xh ",(0x00ff&thisDDU->ddu_code0));
			//	  sprintf(buf3," &nbsp &nbsp StartTimeout <font size=-1>(1 unit = 400 ns)</font>:");
			//	  sprintf(buf4," %02Xh ",(0x00ff&thisDDU->ddu_code0));
				sprintf(buf3," &nbsp &nbsp Start-to-End <font size=-1>(1 unit = 6.4 usec)</font>:");
				sprintf(buf4," %02Xh ",(0xff00&thisDDU->ddu_code0)>>8);
			}else{
				sprintf(buf," ");
				sprintf(buf2," ");
				thisDDU->ddu_shift0=0xFACE;
			}
		}
		if(i==226){
			sprintf(buf,"Generate one Fake DDU L1A via VME:");
			sprintf(buf2," EXPERT ONLY! ");
			icond=2;
			*out << hr();
		}
		if(i==227){
			sprintf(buf,"Toggle DDU 'CFEB Calib-Pulse==L1A' feature (default False): ");
			sprintf(buf2," EXPERT ONLY! ");
			icond=2;
		// JRG, do better later: move to VMEpara and use bit3 of Fake L1A Reg.
		//   --> No...Can't do that because no free lines from VMEfpga to DDUfpga.
		//  add Reg Read & Print on page some other way?  Make F31 shiftable?
		//    sprintf(buf2," %04X <font color=red> EXPERT ONLY! </font> ",0x0008&thisDDU->vmepara_rd_fakel1reg());
		}
		if(i==228){
			sprintf(buf,"DDU Sync Reset via VME: ");
			sprintf(buf2," EXPERT ONLY! ");
			icond=2;
		}
		if(i==229){
			int err=0;
			sprintf(buf3," ");
			sprintf(buf4," ");
			// JRG, add RUI info here:	*out << br() << " <font color=blue size=+1> CSC Board Occupancies </font>" << br() << endl;
			*out << br() << " <font color=blue size=+2> CSC Board Occupancies: </font>";

			//PGK
			//This is the worst way to handle forms EVER,
			//but if you don't include all of the elements in the form,
			//the database lookup will not work.
			// First, let's convert that rui int into a string using streams.
			stringstream ruiNumberStream;
			ruiNumberStream << thisCrate->getRUI(thisDDU->slot());
			string ruiString = ruiNumberStream.str();
			// Second, make the super-long string for the GET part of the form...
			// This should be a sin.
			string ruiFormGetString = "rui1="+ruiString+"&ddu_input1=&ddu1=&fed_crate1=&ddu_slot1=&dcc_fifo1=&slink1=&fiber_crate1=&fiber_pos1=&fiber_socket1=&crateid1=&cratelabel1=&dmb_slot1=&chamberlabel1=&chamberid1=&rui2=&ddu2=&fed_crate2=&ddu_slot2=&ddu_input2=&dcc_fifo2=&slink2=&fiber_crate2=&fiber_pos2=&fiber_socket2=&crateid2=&cratelabel2=&dmb_slot2=&chamberlabel2=&chamberid2=&switch=ddu_chamber&chamber2=";
			// Now, make the link on the page.

			*out << " this DDU is FED Crate " << thisCrate->number() << ", slot " <<thisDDU->slot() << " == ";

			*out << cgicc::a()
				.set("href","http://oraweb03.cern.ch:9000/pls/cms_emu_cern.pro/ddumap.web?"+ruiFormGetString)
				.set("target","_blank")
				<< "RUI #" << thisCrate->getRUI(thisDDU->slot())
				<< cgicc::a() << " <font size=-1> (linked to the Real DB page)</font>" << br() << endl;

			// old oraweb:	.set("href","http://oraweb03.cern.ch:9000/pls/cms_csc_config/ddumap.web?"+ruiFormGetString)
			//	*out << cgicc::table().set("border","0").set("rules","none").set("frame","void");
			*out << cgicc::table().set("align","center").set("width","740").set("cellpadding","5%").set("border","3").set("rules","all").set("frame","border");
			*out << cgicc::colgroup().set("align","center");
			*out << cgicc::col().set("span","2").set("align","center").set("width","20");
			*out << cgicc::col().set("span","4").set("align","center");
			*out << cgicc::thead() << endl;
			*out << cgicc::tr() << endl;
			*out << cgicc::th().set("colspan","2") << " DDU " << cgicc::th() << cgicc::th().set("colspan","4") << " CSC Board Occupancy " << cgicc::th() << cgicc::tr();
			*out << cgicc::tr() << endl;
			*out << cgicc::th() << "Input" << cgicc::th();
			//	*out << cgicc::th() << "Reg.#" << cgicc::th();
			*out << cgicc::th() << "CSC_id<sup>*</sup> " << cgicc::th();
			*out << cgicc::th().set("width","150") << "DMB<sup>**</sup>" << cgicc::th();
			*out << cgicc::th().set("width","150") << "ALCT" << cgicc::th();
			*out << cgicc::th().set("width","150") << "TMB" << cgicc::th();
			*out << cgicc::th().set("width","150") << "CFEB" << cgicc::th() << cgicc::tr() << endl;
			*out << cgicc::thead() << endl;
			*out << cgicc::tbody() << endl;
			for(j=0;j<15;j++){
				thisDDU->ddu_rdscaler();
				stat=((thisDDU->ddu_code1)<<16)+thisDDU->ddu_code0;
				thisDDU->ddu_occmon();
				ndmb=0x0fffffff&thisDDU->fpga_lcode[0];
				nalct=0x0fffffff&thisDDU->fpga_lcode[1];
				ntmb=0x0fffffff&thisDDU->fpga_lcode[2];
				ncfeb=0x0fffffff&thisDDU->fpga_lcode[3];
				// sprintf(buf,"%d </td> <td> %ld ",j,0x0000000f&(thisDDU->fpga_lcode[0]>>28));

				// PGK Check this out!
				ostringstream buffer;
				buffer << j << "</td>";
				if (fibersWithErrors & (1<<j)) {
					buffer << "<td style=\"background-color: #F99\">";
				} else {
					buffer << "<td>";
				}
				buffer << thisDDU->getChamber(j)->name();
				sprintf(buf,buffer.str().c_str());

			// For CSCs with data, for each board print #events & percent vs nDMB;
			//   for DMB print percent vs # L1A.  Useful to detect hot/dead CSCs?
			//   Make RED if no DMBs seen from a LiveFiber.  May need %4.2f for ME1/3...
				while(stat<ndmb){
					stat+=0x01000000;
				}
				if(ndmb>0&&stat>0)sprintf(buf2," %ld &nbsp; &nbsp; %3.1f%% </td><td align=\"center\"> %ld &nbsp; &nbsp; %3.1f%% </td><td align=\"center\"> %ld &nbsp; &nbsp; %3.1f%% </td><td align=\"center\"> %ld &nbsp; &nbsp; %3.1f%% ",ndmb,100.0*ndmb/stat,nalct,100.0*nalct/ndmb,ntmb,100.0*ntmb/ndmb,ncfeb,100.0*ncfeb/ndmb);
				else sprintf(buf2," %ld </td><td> %ld </td><td> %ld </td><td> %ld",ndmb,nalct,ntmb,ncfeb);
				if((ndmb==0&&(live_fiber&(0x0001<<j))==0)||(ndmb>0&&(live_fiber&(0x0001<<j))))
					*out << cgicc::tr() << cgicc::td() << buf << cgicc::td() << cgicc::td() << "<font color=green>" << buf2 << "</font>";
				else *out << cgicc::tr() << cgicc::td() << buf << cgicc::td() << cgicc::td() << "<font color=red>" << buf2 << "</font>";
				if((0xf0000000&thisDDU->fpga_lcode[0])!=(0xf0000000&thisDDU->fpga_lcode[3])){
					err++;
					*out << "<font color=red> * </font>";
					sprintf(buf3," &nbsp * %d End error, last was DDU input = %d read as %ld",err,j,0x0000000f&(thisDDU->fpga_lcode[3]>>28));
				}
				if(thisDDU->ddu_shift0!=0xFACE){
					*out << "<font color=orange> * </font>";
					sprintf(buf4," &nbsp ** JTAG Error in i=%d, DDU input #%d: Shifted %04X",i,j,thisDDU->ddu_shift0);
				}
				*out << cgicc::td() << cgicc::tr() << endl;
			}
			*out << cgicc::tbody() << endl;
			*out << cgicc::table() << endl;
			*out << "&nbsp; &nbsp; <font size=-1> <sup>*</sup> this is the _assumed_ CSC id, based on a local reference file only.</font>" << br() << endl;
			*out << "&nbsp; &nbsp; <font size=-1> <sup>**</sup> the DMB percentage is relative to # L1As; other board percentages are relative to # LCTxL1A hits on the CSC.</font>" << br() << endl;
			if(err>0){
				*out << cgicc::span().set("style","color:red;background-color:#dddddd;");
				*out << buf3 << cgicc::span();
			}
			*out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
			*out << buf4 << cgicc::span();
			sprintf(buf," ");
			sprintf(buf2," ");
			sprintf(buf3," ");
			sprintf(buf4," ");
		}

		if(i==228||i==227||i==222||i==223||i==226){
			string ddutextload = toolbox::toString("/%s/DDUTextLoad",getApplicationDescriptor()->getURN().c_str());
			*out << cgicc::form().set("method","GET").set("action",ddutextload) << endl;
		}
		*out << cgicc::span().set("style","color:black");
		*out << buf << cgicc::span();
		if(icond==1){
			*out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
		}else if(icond==2){
			*out << cgicc::span().set("style","color:red;background-color:#dddddd;");
		}else if(icond==3){
			*out << cgicc::span().set("style","color:blue;background-color:#dddddd;");
		}else{
			*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
		}
		*out << buf2 << cgicc::span();
		*out << cgicc::span().set("style","color:black");
		*out << buf3 << cgicc::span();
		if(icond2==1){
			*out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
		}else if(icond2==2){
			*out << cgicc::span().set("style","color:red;background-color:#dddddd;");
		}else if(icond2==3){
			*out << cgicc::span().set("style","color:blue;background-color:#dddddd;");
		}else{
			*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
		}
		*out << buf4 << cgicc::span();

		if(thisCrate->vmeController()->CAEN_err()!=0){
			*out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
			*out << " **CAEN Error " << cgicc::span();
		}
		if((thisDDU->ddu_shift0!=0xFACE)&&i<226){
			sprintf(buf," **JTAG Error in i=%d, Shifted:%04X",i,thisDDU->ddu_shift0);
			*out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
			*out << buf << cgicc::span();
		}

		if(i>=215&&i<=219) *out << br();
		*out << cgicc::span().set("style","color:black");
		*out << buf5 << cgicc::span();
		if(icond3==1){
			*out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
		}else if(icond3==2){
			*out << cgicc::span().set("style","color:red;background-color:#dddddd;");
		}else if(icond3==3){
			*out << cgicc::span().set("style","color:blue;background-color:#dddddd;");
		}else{
			*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
		}
		*out << buf6 << cgicc::span();
		*out << cgicc::span().set("style","color:black");
		*out << buf7 << cgicc::span();
		if(icond4==1){
			*out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
		}else if(icond4==2){
			*out << cgicc::span().set("style","color:red;background-color:#dddddd;");
		}else{
			*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
		}
		*out << buf8 << cgicc::span();

		if(i==228||i==227||i==222||i==223||i==226){
			string xmltext="";
			if(i==222)xmltext="f7fff";
			if(i==223)xmltext="deb";
			if(i==228||i==227||i==226){
				*out << cgicc::input().set("type","hidden")
					.set("name","textdata")
					.set("size","10")
					.set("ENCTYPE","multipart/form-data")
					.set("value",xmltext)
					.set("style","font-size: 13pt; font-family: arial;") << endl;
			} else {
				*out << cgicc::input().set("type","text")
					.set("name","textdata")
					.set("size","10")
					.set("ENCTYPE","multipart/form-data")
					.set("value",xmltext)
					.set("style","font-size: 13pt; font-family: arial;") << endl;
			}
			if(i==226){
				*out << cgicc::input().set("type","submit")
					.set("value","send-L1A");
			} else if(i==227){
				*out << cgicc::input().set("type","submit")
					.set("value","toggle");
			} else if(i==228){
				*out << cgicc::input().set("type","submit")
					.set("value","reset");
			} else{
				*out << cgicc::input().set("type","submit")
					.set("value","set");
			}
			sprintf(buf,"%d",ddu);
			*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ddu");
			sprintf(buf,"%d",i);
			*out << cgicc::input().set("type","hidden").set("value",buf).set("name","val");

			if(i==222){
				//	  *out << " &nbsp &nbsp bits [14-0]==DDU Fiber Readout Enable";
				*out << cgicc::form();
				*out << endl;
				*out << "<blockquote><font size=-1 face=arial>";
				*out << "key: &nbsp b15==0 forces all DDU Checks Enabled, &nbsp b[14-0]==DDU Fiber Readout Enable (high True)" << br();
				*out << "DDU Check Enable bits (high True): &nbsp b19==Normal DMB Checks (zero to enable only SP/TF checks)," << br() << " &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp b18==CFEB Checks (zero disables DAV checks),  &nbsp b17==TMB Checks,  &nbsp b16==ALCT Checks" << br();
				if((stat&0x00018000)==0x8000) *out << "<font color=blue> &nbsp &nbsp &nbsp ALCT checking is disabled</font>";
				if((stat&0x00028000)==0x8000) *out << "<font color=blue> &nbsp &nbsp &nbsp TMB checking is disabled</font>";
				if((stat&0x00008000)>0&&(stat&0x00030000)<0x00030000) *out << br() ;
				if((stat&0x00048000)==0x8000) *out << "<font color=blue> &nbsp &nbsp &nbsp CFEB DAV/LCT/MOVLP/L1A checks disabled</font>";
				if((stat&0x00088000)==0x8000) *out << "<font color=blue> &nbsp &nbsp &nbsp Some DMB checks disabled for SP/TF compatibility</font>";
				if((stat&0x00008000)==0) *out << "<font color=green> &nbsp &nbsp &nbsp All checks are Enabled</font>";
				*out << "</font></blockquote>" << endl;
			}
			else *out << cgicc::form() << endl;

		}else if(i==201&&(stat&0x0000f000)>0){
			*out << "<blockquote><font size=-1 color=red face=arial>";
			if((stat&0xF0000000)>0){
				if((0x80000000&stat)>0) *out << " DMB LCT/DAV/MOVLP Mismatch &nbsp ";
				if((0x40000000&stat)>0) *out << " CFEB L1A Mismatch &nbsp ";
				if((0x20000000&stat)>0) *out << " <font color=blue>DDUsawNoGoodDMB-CRCs</font> &nbsp ";
				if((0x10000000&stat)>0) *out << " CFEB Count Mismatch";
				if(0x0fffffff&stat) *out << br();
			}
			if((stat&0x0F000000)>0){
				if((0x08000000&stat)>0) *out << " FirstDat Error &nbsp ";
				if((0x04000000&stat)>0) *out << " L1A-FIFO Full occurred &nbsp ";
				if((0x02000000&stat)>0) *out << " Data Stuck in FIFO occurred &nbsp ";
				if((0x01000000&stat)>0) *out << " <font color=blue>NoLiveFiber warning</font>";
				if(0x00ffffff&stat) *out << br();
			}
			if((stat&0x00F00000)>0){
				if((0x00800000&stat)>0) *out << " <font color=blue>Special-word voted-bit warning</font> &nbsp ";
				if((0x00400000&stat)>0) *out << " InRDctrl Error &nbsp ";
				if((0x00200000&stat)>0) *out << " <font color=blue>DAQ Stop bit set</font> &nbsp ";
				if((0x00100000&stat)>0) *out << " <font color=blue>DAQ says Not Ready</font>";
				if((0x00300000&stat)==0x00200000) *out << " <font color=blue>DAQ Applied Backpressure</font>";
				if(0x000fffff&stat) *out << br();
			}
			if((stat&0x000F0000)>0){
				*out << "<font color=orange>";
				if((0x00080000&stat)>0) *out << " TMB Error &nbsp ";
				if((0x00040000&stat)>0) *out << " ALCT Error &nbsp ";
				if((0x00020000&stat)>0) *out << " Trigger Wordcount Error &nbsp ";
				if((0x00010000&stat)>0) *out << " Trigger L1A Match Error";
				if(0x0000ffff&stat) *out << br();
				*out << "</font>";
			}
		// JRG, low-order 16-bit status (most serious errors):
			if((stat&0x0000F000)>0){
				if((0x00008000&stat)>0) *out << " <font color=red>Critical Error ** needs reset **</font> &nbsp ";
				if((0x00004000&stat)>0) *out << " <font color=orange>Single Error, bad event</font> &nbsp ";
				if((0x00002000&stat)>0) *out << " <font color=blue>Single warning, possible data problem</font> &nbsp ";
				if((0x00001000&stat)>0) *out << " <font color=blue>Near Full Warning</font>";
				if(0x00000fff&stat) *out << br();
			}
			if((stat&0x00000F00)>0){
				if((0x00000800&stat)>0) *out << " <font color=blue>64-bit Alignment Error</font> &nbsp ";
				if((0x00000400&stat)>0) *out << " <font color=blue>DDU Control DLL Error (recent)</font> &nbsp ";
				if((0x00000200&stat)>0) *out << " <font color=orange>DMB Error in event</font> &nbsp ";
				if((0x00000100&stat)>0) *out << " <font color=orange>Lost In Event Error</font>";
				if(0x000000ff&stat) *out << br();
			}
			if((stat&0x000000F0)>0){
				if((0x00000080&stat)>0) *out << " Lost In Data Error occurred &nbsp ";
				if((0x00000040&stat)>0) *out << " Timeout Error occurred &nbsp ";
				if((0x00000020&stat)>0) *out << " <font color=orange>Trigger CRC Error</font> &nbsp ";
				if((0x00000010&stat)>0) *out << " Multiple Transmit Errors occurred";
				if(0x0000000f&stat) *out << br();
			}
			if((stat&0x0000000F)>0){
				if((0x00000008&stat)>0) *out << " Lost Sync occurred (FIFO Full or L1A Error) &nbsp ";
				if((0x00000004&stat)>0) *out << " Fiber/FIFO Connection Error occurred &nbsp ";
				if((0x00000002&stat)>0) *out << " <font color=orange>Single L1A Mismatch</font> &nbsp ";
				if((0x00000001&stat)>0) *out << " <font color=orange>DMB or CFEB CRC Error</font>";
			}
			*out << "</font></blockquote>";

		}else if(i==202&&(stat&0x0000D981)>0){
			// *out << "<blockquote><font size=-1 color=black face=arial>";
			*out << "<font size=-1 color=black face=arial>";
			if(0x00001000&stat) *out << " &nbsp &nbsp <font color=green>DDU S-Link Not Present</font>";
			if(0x00000100&stat) *out << " &nbsp &nbsp <font color=blue>SPY/GbE Fiber Disconnected</font>" << endl;
			else if((0x00000200&stat)>0) *out << " &nbsp  &nbsp SPY/GbE FIFO Always Empty";
			if((stat&0x0000ECEF)>0){
				*out << "<blockquote>";
				if((stat&0x0000E000)>0){
					if((0x00008000&stat)>0) *out << " DDU Buffer Overflow occurred &nbsp ";
					if((0x00004000&stat)>0) *out << " <font color=blue>DAQ (DCC/S-Link) Wait occurred</font> &nbsp ";
					if((0x00002000&stat)>0) *out << " DDU S-Link Full occurred &nbsp ";
					//	  if((0x00001000&stat)>0) *out << " DDU S-Link Never Ready";
					if((0x000000cef&stat)>0&&(0x000000cef&stat)<=0x00ff) *out << br();
				}
				if((stat&0x00000E00)>0){
					if((stat&0x00000900)==0x0800||(stat&0x00000500)==0x0400) *out << br();
					if((0x00000800&stat)>0&&(0x00000100&stat)==0) *out << " <font color=blue>DDU GbE Overflow occurred</font> &nbsp ";
					if((0x00000400&stat)>0&&(0x00000100&stat)==0) *out << " GbE Transmit Limit occurred &nbsp ";
					//	    if((0x00000200&stat)>0&&(0x00000100&stat)==0) *out << " GbE FIFO Always Empty &nbsp ";
					//	  if((0x00000100&stat)>0) *out << " <font color=blue>SPY/GbE Fiber Disconnect occurred</font>";
					if(0x000000ef&stat) *out << br();
				}
				if((stat&0x000000F0)>0){
					if((0x00000080&stat)>0) *out << " <font color=red>DDU DAQ-Limited Overflow occurred (DCC/S-Link Wait)</font> &nbsp ";
					if((0x00000040&stat)>0) *out << " <font color=blue>DAQ (DCC/S-Link) Wait</font> &nbsp ";
					if((0x00000020&stat)>0) *out << " DDU S-Link Full/Stop &nbsp ";
					if((0x00000010&stat)>0&&(0x00001000&stat)==0) *out << " <font color=red>DDU S-Link Not Ready</font>";
				}
				if((stat&0x0000000F)>0){
					if((0x0000000e&stat)>0&&(0x00000001&stat)==0) *out << br();
					if((0x00000008&stat)>0&&(0x00000100&stat)==0) *out << " GbE FIFO Full &nbsp ";
					if((0x00000004&stat)>0&&(0x00000100&stat)==0) *out << " DDU Skipped SPY Event (GbE data not sent) &nbsp ";
					if((0x00000002&stat)>0&&(0x00000100&stat)==0) *out << " GbE FIFO Not Empty &nbsp ";
					if((0x00000001&stat)>0) *out << " <font color=blue>DCC Link Not Ready</font>";
				}
				//        *out << "</font></blockquote>";
				*out << "</blockquote></font>";
			}
			else *out << "</font>" << br();

		}else if(i==204&&(stat&0x0000ffff)>0){
			*out << "<blockquote><font size=-1 color=orange face=arial>";
			if((stat&0x0000F000)>0){
				if((0x00008000&stat)>0) *out << " <font color=red>DMB Timeout signal, ** needs reset **</font> &nbsp ";
				if((0x00004000&stat)>0) *out << " <font color=red>Mult L1A Error occurred</font> &nbsp ";
				if((0x00002000&stat)>0) *out << " <font color=blue>L1A-FIFO Near Full Warning</font> &nbsp ";
				if((0x00001000&stat)>0) *out << " <font color=black>GbE FIFO Almost-Full</font>";
				if(0x0fff&stat) *out << br();
			}
			if((stat&0x00000F00)>0){
				if((0x00000800&stat)>0) *out << " <font color=blue>Ext.FIFO Near Full Warning</font> &nbsp ";
			//	  if((0x00000400&stat)>0) *out << " <font color=blue>Near Full Warning</font> &nbsp ";
				if((0x00000400&stat)>0) *out << " <font color=blue>InSingle Warning</font> &nbsp ";
				if((0x00000200&stat)>0) *out << " <font color=black>CFEB-CRC not OK</font> &nbsp ";
				if((0x00000100&stat)>0) *out << " CFEB-CRC End Error";
				if(0x00ff&stat) *out << br();
			}
			if((stat&0x000000F0)>0){
				if((0x00000080&stat)>0) *out << " CFEB-CRC Count Error &nbsp ";
				if((0x00000040&stat)>0) *out << " DMB or CFEB CRC Error &nbsp ";
				//	  if((0x00000020&stat)>0) *out << " <font color=black>Latched Trigger Trail</font> &nbsp ";
				if((0x00000020&stat)>0) *out << " Trigger Readout Error &nbsp ";
				if((0x00000010&stat)>0) *out << " <font color=black>Trigger Trail Done</font>";
				if(0x000f&stat) *out << br();
			}
			if((stat&0x0000000F)>0){
				if((0x00000008&stat)>0) *out << " <font color=red>Start Timeout</font> &nbsp ";
				if((0x00000004&stat)>0) *out << " <font color=red>End Timeout</font> &nbsp ";
				if((0x00000002&stat)>0) *out << " SP/TF Error in last event &nbsp ";
				if((0x00000001&stat)>0) *out << " SP/TF data detected in last event";
			}
			*out << "</font></blockquote>";

		}else if(i==205&&(stat&0x0000ffff)>0){
			if((0x00000020&stat)>0) *out << " &nbsp  &nbsp <font color=green size=-1>Empty CSC in Event flag</font>";
			if((0x0000FFDF&stat)>0) *out << "<blockquote><font size=-1 color=orange face=arial>";
			if((stat&0x0000F000)>0){
				if((0x00008000&stat)>0) *out << " Lost In Event Error &nbsp ";
				if((0x00004000&stat)>0) *out << " DMB Error in Event&nbsp ";
				if((0x00002000&stat)>0) *out << " <font color=blue>Control DLL Error occured</font> &nbsp ";
				if((0x00001000&stat)>0) *out << " 2nd Header First flag";
				if(0x0fdf&stat) *out << br();
			}
			if((stat&0x00000F00)>0){
				if((0x00000800&stat)>0) *out << " <font color=black>Early 2nd Trailer flag</font> &nbsp ";
				if((0x00000400&stat)>0) *out << " <font color=black>Extra 1st Trailer flag</font> &nbsp ";
				if((0x00000200&stat)>0) *out << " <font color=black>Extra 1st Header flag</font> &nbsp ";
				if((0x00000100&stat)>0) *out << " <font color=black>Extra 2nd Header flag</font>";
				if(0x00df&stat) *out << br();
			}
			if((stat&0x000000D0)>0){
				if((0x00000080&stat)>0) *out << " SCA Full detected this Event &nbsp ";
				if((0x00000040&stat)>0) *out << " <font color=blue>Probable DMB Full occurred</font> &nbsp ";
				//	  if((0x00000020&stat)>0) *out << " <font color=green>Empty Event flag</font> &nbsp ";
				if((0x00000010&stat)>0) *out << " <font color=red>Bad Control Word Error occurred</font>";
				if(0x000f&stat) *out << br();
			}
			if((stat&0x0000000F)>0){
				if((0x00000008&stat)>0) *out << " Missed Trigger Trailer Error &nbsp ";
				if((0x00000004&stat)>0) *out << " First Dat Error &nbsp ";
				if((0x00000002&stat)>0) *out << " Bad First Word &nbsp ";
				if((0x00000001&stat)>0) *out << " <font color=red>Confirmed DMB Full occured</font>";
				// if((0x00000001&stat)>0) *out << " <font color=red>Lost In Data occured</font>";
			}
			if((0x0000FFDF&stat)>0) *out << "</font></blockquote>";
			else *out << br();

		}else if(i==206&&(stat&0x0000ffff)>0){
			*out << "<blockquote><font size=-1 color=black face=arial>";
			if((stat&0x0000F000)>0){
				if((0x00008000&stat)>0) *out << " <font color=orange>Trigger Readout Error</font> &nbsp ";
				if((0x00004000&stat)>0) *out << " ALCT Trailer Done &nbsp ";
				if((0x00002000&stat)>0) *out << " <font color=red>2nd ALCT Trailer detected</font> &nbsp ";
				//	  if((0x00002000&stat)>0) *out << " ALCT DAV Vote True occurred &nbsp ";
				if((0x00001000&stat)>0) *out << " ALCT L1A mismatch error occurred";
				if(0x0fff&stat) *out << br();
			}
			if((stat&0x00000F00)>0){
				if((0x00000800&stat)>0) *out << " ALCT CRC Error occurred &nbsp ";
				if((0x00000400&stat)>0) *out << " ALCT Wordcount Error occurred &nbsp ";
				if((0x00000200&stat)>0) *out << " Missing ALCT Trailer occurred &nbsp ";
				if((0x00000100&stat)>0) *out << " ALCT Error occurred";
				if(0x00ff&stat) *out << br();
			}
			if((stat&0x000000F0)>0){
				if((0x00000080&stat)>0) *out << " DMB Critical Error occurred &nbsp ";
				//	  if((0x00000080&stat)>0) *out << " Compare Trigger CRC flag &nbsp ";
				if((0x00000040&stat)>0) *out << " TMB Trailer Done &nbsp ";
				if((0x00000020&stat)>0) *out << " <font color=red>2nd TMB Trailer detected</font> &nbsp ";
				//	  if((0x00000020&stat)>0) *out << " TMB DAV Vote True occurred &nbsp ";
				if((0x00000010&stat)>0) *out << " TMB L1A mismatch error occurred";
				if(0x000f&stat) *out << br();
			}
			if((stat&0x0000000F)>0){
				if((0x00000008&stat)>0) *out << " TMB CRC Error occurred &nbsp ";
				if((0x00000004&stat)>0) *out << " TMB Word Count Error occurred &nbsp ";
				if((0x00000002&stat)>0) *out << " Missing TMB Trailer occurred &nbsp ";
				if((0x00000001&stat)>0) *out << " TMB Error occurred";
			}
			*out << "</font></blockquote>";

		}else if(i==216&&(stat&0x000000ff)>0){
			*out << endl;
			*out << "<font size=-2>";
			if((0x00000040&stat)>0) *out << " &nbsp &nbsp &nbsp L1A FIFO Empty";
			if((0x00000040&stat)==0) *out << " &nbsp &nbsp &nbsp L1A FIFO Not Empty";
			if((0x00000083&stat)>0){
				*out << "<blockquote>";
				if((0x00000080&stat)>0) *out << " <font color=blue>DDU C-code L1A error</font>";
				if((0x00000002&stat)>0) *out << " GbE FIFO Almost-Full occurred &nbsp ";
				if((0x00000001&stat)>0) *out << " <font color=blue>L1A FIFO Almost-Full occurred</font>";
				*out << "</blockquote></font>";
			} else *out << "</font>" << br();
		}else if(i==218&&(stat&0x000000ff)>0){
			*out << "<font size=-2>";
			if((0x00000040&stat)>0) *out << " &nbsp &nbsp &nbsp L1A FIFO Empty";
			if((0x00000040&stat)==0) *out << " &nbsp &nbsp &nbsp L1A FIFO Not Empty";
			*out << endl;
			if((0x00000003&stat)>0){
				*out << "<blockquote>";
				if((0x00000002&stat)>0) *out << " GbE FIFO Full occurred &nbsp ";
				if((0x00000001&stat)>0) *out << " <font color=red>L1A FIFO Full occurred</font>";
				*out << "</blockquote></font>";
			} else *out << "</font>" << br();

		}else if(i==220&&(stat&0x00000fff)>0){
			*out << endl;
			*out << "<blockquote><font size=-1 color=red face=arial>";
			if((stat&0x00000F00)>0){
				if((0x00000800&stat)>0) *out << " InRD End C-Code Error occurred &nbsp ";
				if((0x00000400&stat)>0) *out << " InRD Begin C-Code Error occurred &nbsp ";
				if((0x00000200&stat)>0) *out << " InRD Multiple L1A Mismatches occurred &nbsp ";
				else if((0x00000100&stat)>0) *out << " <font color=blue>InRD Single L1A Mismatch occurred</font>";
				*out << br();
			}
			if((stat&0x000000F0)>0){
				if((0x00000080&stat)>0) *out << " InRD Hard Error occurred &nbsp ";
				if((0x00000040&stat)>0) *out << " InRD Sync Error occurred &nbsp ";
				if((0x00000020&stat)>0) *out << " <font color=blue>InRD Single Error occurred</font> &nbsp ";
				if((0x00000010&stat)>0) *out << " InRD Mem/FIFO Error occurred";
				*out << br();
			}
			if((stat&0x0000000F)>0){
				if((0x00000008&stat)>0) *out << " InRD Fiber Connection Error occurred &nbsp ";
				if((0x00000004&stat)>0) *out << " InRD Multiple Transmit Errors occurred &nbsp ";
				if((0x00000002&stat)>0) *out << " InRD Stuck Data Error occurred &nbsp ";
				if((0x00000001&stat)>0) *out << " InRD Timeout Error occurred";
				*out << br();
			}
			*out << "</font></blockquote>";

		}else if(i==221 && (icond>0 || icond2>0)){
			*out  << endl;
			*out << "<blockquote><font size=-1 face=arial>";
			*out << "key for Near Full bits: &nbsp b7=DDU set FMM Warn, &nbsp b6=DMB set Warn," << br();
			*out << " &nbsp &nbsp &nbsp &nbsp b5=Ext. FIFO set Warn, &nbsp b4==L1A FIFO set Warn, &nbsp b[3-0]=InRD 3-0 set Warn";
			*out << "</font></blockquote>" << endl;

		}else if(i==224 && icrit>0){
			*out << "<blockquote><font size=-2 color=black face=arial>";
			//	DDUtrapDecode(in, out, thisDDU->fpga_lcode);
			this->DDUtrapDecode(in, out);
			*out << "</font></blockquote>";
		}else{
			*out << br() << endl;
		}
	}

	*out << cgicc::body() << endl;
	*out << cgicc::html() << endl;
	*/
}



void EmuFCrateHyperDAQ::InFpga(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{

	cgicc::Cgicc cgi(in);

	cgicc::form_iterator name = cgi.getElement("ddu");
	unsigned int cgiDDU = 0;
	if (name != cgi.getElements().end()) {
		cgiDDU = cgi["ddu"]->getIntegerValue();
		//cout << "DDU inside " << ddu << endl;
	}
	thisDDU = dduVector[cgiDDU];

	ostringstream sTitle;
	sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DDU InFPGA Controls (RUI #" << thisCrate->getRUI(thisDDU->slot()) << ")";
	*out << Header(sTitle.str(),false);

	// PGK Select-a-DDU
	*out << cgicc::fieldset()
		.set("class","header") << endl;
	*out << cgicc::div("Show this page for a different DDU")
		.set("style","font-weight: bold; font-size: 8pt; width: 100%; text-align: center;") << endl;

	unsigned int iddu = 0;

	*out << cgicc::table()
		.set("style","width: 90%; margin: 2px auto 2px auto; font-size: 8pt; text-align: center;") << endl;
	*out << cgicc::tr() << endl;
	// Loop over all the boards defined.
	for (iddu = 0; iddu < dduVector.size(); iddu++) {

		if (dduVector[iddu]->slot() > 21) continue;

		*out << cgicc::td() << endl;
		if (iddu != cgiDDU) {
			ostringstream location;
			location << "/" + getApplicationDescriptor()->getURN() + "/InFpga?ddu=" << iddu;
			*out << cgicc::a()
				.set("href",location.str());
		}
		*out << "Slot " << dduVector[iddu]->slot() << ": RUI #" << thisCrate->getRUI(dduVector[iddu]->slot());
		if (iddu != cgiDDU) {
			*out << cgicc::a();
		}
		*out << cgicc::td() << endl;
	}
	*out << cgicc::tr() << endl;
	*out << cgicc::table() << endl;
	*out << cgicc::fieldset() << endl;

	// PGK Display-a-Crate
	*out << cgicc::fieldset()
		.set("class","fieldset") << endl;
	*out << cgicc::div("Crate Selection")
		.set("class","legend") << endl;

	*out << cgicc::div()
		.set("style","background-color: #FF0; color: #00A; font-size: 16pt; font-weight: bold; width: 50%; text-align: center; padding: 3px; border: 2px solid #00A; margin: 5px auto 5px auto;");
	*out << "Crate " << thisCrate->number() << " Selected";
	*out << cgicc::div() << endl;

	*out << cgicc::span("Configuration located at " + xmlFile_.toString())
		.set("style","color: #A00; font-size: 10pt;") << endl;

	*out << cgicc::fieldset() << endl;
	*out << br() << endl;
	
	// This is used to check if the debug trap is valid.  One for each device.
	bool debugTrapValid[2] = {
		false,
		false
	};

	// Just record what fibers have what errors.  Maybe this will be useful.
	unsigned int fibersWithErrors = 0;
	
	// Start reading some registers!
	thisCrate->vmeController()->CAEN_err_reset();

	// We will use this object to debug all the DDU's problems.
	DDUDebugger *debugger = new DDUDebugger();

	// Do this for both InFPGAs
	enum DEVTYPE devTypes[2] = {
		INFPGA0,
		INFPGA1
	};
	string devNames[2] = {
		"InFPGA0 (fibers 7-0)",
		"InFPGA1 (fibers 14-8)"
	};

	// Display general InFPGA status information
	*out << cgicc::fieldset()
		.set("class","normal") << endl;
	*out << cgicc::div()
		.set("class","legend") << endl;
	*out << cgicc::a("&plusmn;")
		.set("class","openclose")
		.set("onClick","javascript:toggle('generalTable')") << endl;
	*out << "General InFPGA Information" << cgicc::div() << endl;

	DataTable generalTable("generalTable");

	// Names have to come first, because there is only one of each.
	generalTable.addColumn("Register");
	generalTable.addColumn("Value");
	generalTable.addColumn("Decoded Status");
	generalTable.addBreak();
	generalTable.addColumn("Value");
	generalTable.addColumn("Decoded Status");

	for (unsigned int iDevType = 0; iDevType < 2; iDevType++) {
		enum DEVTYPE dt = devTypes[iDevType];

		if (iDevType == 0) {
			*(generalTable(0,0)->value) << "InFPGA status (32-bit)";
			*(generalTable(1,0)->value) << "L1 Event Scaler0/2 (24-bit)";
			*(generalTable(2,0)->value) << "L1 Event Scaler1/3 (24-bit)";
		}

		unsigned long int infpgastat = thisDDU->infpgastat(dt);
		*(generalTable(0,iDevType*2+1)->value) << showbase << hex << infpgastat;
		if (infpgastat & 0x00004000) generalTable(0,iDevType*2+1)->setClass("warning");
		if (infpgastat & 0x00008000) {
			generalTable(0,iDevType*2+1)->setClass("bad");
			debugTrapValid[iDevType] = true;
		}
		if (!(infpgastat & 0x0000C000)) generalTable(0,iDevType*2+1)->setClass("ok");
		std::map<string, string> infpgastatComments = debugger->InFPGAStat(dt,infpgastat);
		for (std::map<string,string>::iterator iComment = infpgastatComments.begin();
			iComment != infpgastatComments.end();
			iComment++) {
			*(generalTable(0,iDevType*2+2)->value) << cgicc::div(iComment->first)
				.set("class",iComment->second);
		}

		unsigned long int L1Scaler = thisDDU->readL1Scaler(dt);
		*(generalTable(1,iDevType*2+1)->value) << L1Scaler;

		unsigned long int L1Scaler1 = thisDDU->readL1Scaler1(dt);
		*(generalTable(2,iDevType*2+1)->value) << L1Scaler1;

	}
	
	// Summary
	*out << generalTable.printSummary() << endl;

	// We use a special case of the toHTML method for InFPGAs.
	// Hide the table if all is keen.
	if (generalTable.countClass("ok") + generalTable.countClass("none")
		== generalTable.countRows() * 2) {
		*out << cgicc::table()
			.set("id","generalTable")
			.set("class","data")
			.set("style","display: none;") << endl;
	} else {
		*out << cgicc::table()
			.set("id","generalTable")
			.set("class","data") << endl;
	}

	// Put the titles of the two InFPGAs here.
	*out << cgicc::tr() << endl;
	*out << cgicc::td(" ") << endl;
	*out << cgicc::td(devNames[0])
		.set("colspan","2")
		.set("style","font-weight: bold; text-align: center; border-right: 3px double #000;")<< endl;
	*out << cgicc::td(devNames[1])
		.set("colspan","2")
		.set("style","font-weight: bold; text-align: center;") << endl;
	*out << cgicc::tr() << endl;

	// Now print the concatonated table.
	*out << generalTable.toHTML(false);

	// And now close the table.
	*out << cgicc::table() << endl;

	*out << cgicc::fieldset() << endl;

	// Clever trick to help with formating the cut-and-paste.
	*out << cgicc::br()
		.set("style","display: none") << endl;


	// Display individual fiber status information
	*out << cgicc::fieldset()
		.set("class","normal") << endl;
	*out << cgicc::div()
		.set("class","legend") << endl;
	*out << cgicc::a("&plusmn;")
		.set("class","openclose")
		.set("onClick","javascript:toggle('fiberTable')") << endl;
	*out << "Individual Fiber Information" << cgicc::div() << endl;

	DataTable fiberTable("fiberTable");

	// Names have to come first, because there is only one of each.
	fiberTable.addColumn("Register");
	fiberTable.addColumn("Value");
	fiberTable.addColumn("Decoded Chambers");
	fiberTable.addBreak();
	fiberTable.addColumn("Value");
	fiberTable.addColumn("Decoded Chambers");

	for (unsigned int iDevType = 0; iDevType < 2; iDevType++) {
		enum DEVTYPE dt = devTypes[iDevType];
		unsigned int fiberOffset = (dt == INFPGA0 ? 0 : 8);

		if (iDevType == 0) {
			*(fiberTable(0,0)->value) << "DMB full";
			*(fiberTable(1,0)->value) << "DMB warning";
			*(fiberTable(2,0)->value) << "Connection error";
			*(fiberTable(3,0)->value) << "Link active";
			*(fiberTable(4,0)->value) << "Stuck data";
			*(fiberTable(5,0)->value) << "L1A mismatch";
			*(fiberTable(6,0)->value) << "GT-Rx error";
			*(fiberTable(7,0)->value) << "Timeout-start";
			*(fiberTable(8,0)->value) << "Timeout-end busy";
			*(fiberTable(9,0)->value) << "Timeout-end wait";
			*(fiberTable(10,0)->value) << "SCA full history";
			*(fiberTable(11,0)->value) << "CSC transmit error";
			*(fiberTable(12,0)->value) << "DDU lost-in-event error";
			*(fiberTable(13,0)->value) << "DDU lost-in-data error";
		}

		unsigned int readDMBWarning = thisDDU->readDMBWarning(dt);
		*(fiberTable(0,iDevType*2+1)->value) << showbase << hex << ((readDMBWarning >> 8) & 0xff);
		if (((readDMBWarning >> 8) & 0xff)) fiberTable(0,iDevType*2+1)->setClass("bad");
		else fiberTable(0,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readDMBWarning >> 8) & 0xff) & (1<<iFiber)) {
				fibersWithErrors |= (1<<(iFiber + fiberOffset));
				*(fiberTable(0,iDevType*2+2)->value) << cgicc::div(thisDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","red");
			}
		}

		*(fiberTable(1,iDevType*2+1)->value) << showbase << hex << ((readDMBWarning) & 0xff);
		if (((readDMBWarning) & 0xff)) fiberTable(1,iDevType*2+1)->setClass("warning");
		else fiberTable(1,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readDMBWarning) & 0xff) & (1<<iFiber)) {
				*(fiberTable(1,iDevType*2+2)->value) << cgicc::div(thisDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","orange");
			}
		}

		unsigned int checkFiber = thisDDU->checkFiber(dt);
		*(fiberTable(2,iDevType*2+1)->value) << showbase << hex << ((checkFiber >> 8) & 0xff);
		if (((checkFiber >> 8) & 0xff)) fiberTable(2,iDevType*2+1)->setClass("bad");
		else fiberTable(2,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((checkFiber >> 8) & 0xff) & (1<<iFiber)) {
				fibersWithErrors |= (1<<(iFiber + fiberOffset));
				*(fiberTable(2,iDevType*2+2)->value) << cgicc::div(thisDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","red");
			}
		}

		*(fiberTable(3,iDevType*2+1)->value) << showbase << hex << ((checkFiber) & 0xff);
		fiberTable(3,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((checkFiber) & 0xff) & (1<<iFiber)) {
				*(fiberTable(3,iDevType*2+2)->value) << cgicc::div(thisDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","none");
			}
		}

		unsigned int readDMBSync = thisDDU->readDMBSync(dt);
		*(fiberTable(4,iDevType*2+1)->value) << showbase << hex << ((readDMBSync >> 8) & 0xff);
		if (((readDMBSync >> 8) & 0xff)) fiberTable(4,iDevType*2+1)->setClass("bad");
		else fiberTable(4,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readDMBSync >> 8) & 0xff) & (1<<iFiber)) {
				fibersWithErrors |= (1<<(iFiber + fiberOffset));
				*(fiberTable(4,iDevType*2+2)->value) << cgicc::div(thisDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","red");
			}
		}

		*(fiberTable(5,iDevType*2+1)->value) << showbase << hex << ((readDMBSync) & 0xff);
		if (((readDMBSync) & 0xff)) fiberTable(5,iDevType*2+1)->setClass("warning");
		else fiberTable(5,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readDMBSync) & 0xff) & (1<<iFiber)) {
				*(fiberTable(5,iDevType*2+2)->value) << cgicc::div(thisDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","orange");
			}
		}

		unsigned int readRxError = thisDDU->readRxError(dt);
		*(fiberTable(6,iDevType*2+1)->value) << showbase << hex << ((readRxError >> 8) & 0xff);
		if (((readRxError >> 8) & 0xff)) fiberTable(6,iDevType*2+1)->setClass("questionable");
		else fiberTable(6,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readRxError >> 8) & 0xff) & (1<<iFiber)) {
				*(fiberTable(6,iDevType*2+2)->value) << cgicc::div(thisDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","blue");
			}
		}

		*(fiberTable(7,iDevType*2+1)->value) << showbase << hex << ((readRxError) & 0xff);
		if (((readRxError) & 0xff)) fiberTable(7,iDevType*2+1)->setClass("bad");
		else fiberTable(7,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readRxError) & 0xff) & (1<<iFiber)) {
				fibersWithErrors |= (1<<(iFiber + fiberOffset));
				*(fiberTable(7,iDevType*2+2)->value) << cgicc::div(thisDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","red");
			}
		}

		unsigned int readTimeout = thisDDU->readTimeout(dt);
		*(fiberTable(8,iDevType*2+1)->value) << showbase << hex << ((readTimeout >> 8) & 0xff);
		if (((readTimeout >> 8) & 0xff)) fiberTable(8,iDevType*2+1)->setClass("bad");
		else fiberTable(8,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readTimeout >> 8) & 0xff) & (1<<iFiber)) {
				fibersWithErrors |= (1<<(iFiber + fiberOffset));
				*(fiberTable(8,iDevType*2+2)->value) << cgicc::div(thisDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","red");
			}
		}

		*(fiberTable(9,iDevType*2+1)->value) << showbase << hex << ((readTimeout) & 0xff);
		if (((readTimeout) & 0xff)) fiberTable(9,iDevType*2+1)->setClass("bad");
		else fiberTable(9,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readTimeout) & 0xff) & (1<<iFiber)) {
				fibersWithErrors |= (1<<(iFiber + fiberOffset));
				*(fiberTable(9,iDevType*2+2)->value) << cgicc::div(thisDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","red");
			}
		}

		unsigned int readTxError = thisDDU->readTxError(dt);
		*(fiberTable(10,iDevType*2+1)->value) << showbase << hex << ((readTxError >> 8) & 0xff);
		fiberTable(10,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readTxError >> 8) & 0xff) & (1<<iFiber)) {
				*(fiberTable(10,iDevType*2+2)->value) << cgicc::div(thisDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","none");
			}
		}

		*(fiberTable(11,iDevType*2+1)->value) << showbase << hex << ((readTxError) & 0xff);
		if (((readTxError) & 0xff)) fiberTable(11,iDevType*2+1)->setClass("bad");
		else fiberTable(11,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readTxError) & 0xff) & (1<<iFiber)) {
				fibersWithErrors |= (1<<(iFiber + fiberOffset));
				*(fiberTable(11,iDevType*2+2)->value) << cgicc::div(thisDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","red");
			}
		}

		unsigned int readLostError = thisDDU->readLostError(dt);
		*(fiberTable(12,iDevType*2+1)->value) << showbase << hex << ((readLostError >> 8) & 0xff);
		if (((readLostError) & 0xff)) fiberTable(12,iDevType*2+1)->setClass("warning");
		else fiberTable(12,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readLostError >> 8) & 0xff) & (1<<iFiber)) {
				*(fiberTable(12,iDevType*2+2)->value) << cgicc::div(thisDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","orange");
			}
		}

		*(fiberTable(13,iDevType*2+1)->value) << showbase << hex << ((readLostError) & 0xff);
		if (((readLostError) & 0xff)) fiberTable(13,iDevType*2+1)->setClass("bad");
		else fiberTable(13,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readLostError) & 0xff) & (1<<iFiber)) {
				fibersWithErrors |= (1<<(iFiber + fiberOffset));
				*(fiberTable(13,iDevType*2+2)->value) << cgicc::div(thisDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","red");
			}
		}
	}

	// Summary
	*out << fiberTable.printSummary() << endl;

	// We use a special case of the toHTML method for InFPGAs.
	// Hide the table if all is keen.
	if (fiberTable.countClass("ok") + fiberTable.countClass("none")
		== fiberTable.countRows() * 2) {
		*out << cgicc::table()
			.set("id","fiberTable")
			.set("class","data")
			.set("style","display: none;") << endl;
	} else {
		*out << cgicc::table()
			.set("id","fiberTable")
			.set("class","data") << endl;
	}

	// Put the titles of the two InFPGAs here.
	*out << cgicc::tr() << endl;
	*out << cgicc::td(" ") << endl;
	*out << cgicc::td(devNames[0])
		.set("colspan","2")
		.set("style","font-weight: bold; text-align: center; border-right: 3px double #000;")<< endl;
	*out << cgicc::td(devNames[1])
		.set("colspan","2")
		.set("style","font-weight: bold; text-align: center;") << endl;
	*out << cgicc::tr() << endl;

	// Now print the concatonated table.
	*out << fiberTable.toHTML(false);

	// And now close the table.
	*out << cgicc::table() << endl;

	*out << cgicc::fieldset() << endl;

	// Clever trick to help with formating the cut-and-paste.
	*out << cgicc::br()
		.set("style","display: none") << endl;


	// Display other fiber information
	*out << cgicc::fieldset()
		.set("class","normal") << endl;
	*out << cgicc::div()
		.set("class","legend") << endl;
	*out << cgicc::a("&plusmn;")
		.set("class","openclose")
		.set("onClick","javascript:toggle('otherTable')") << endl;
	*out << "Other Fiber/InRD Information" << cgicc::div() << endl;

	DataTable otherTable("otherTable");

	// Names have to come first, because there is only one of each.
	otherTable.addColumn("Register");
	otherTable.addColumn("Value");
	otherTable.addColumn("Decoded Chambers/Status");
	otherTable.addBreak();
	otherTable.addColumn("Value");
	otherTable.addColumn("Decoded Chambers/Status");

	for (unsigned int iDevType = 0; iDevType < 2; iDevType++) {
		enum DEVTYPE dt = devTypes[iDevType];
		unsigned int fiberOffset = (dt == INFPGA0 ? 0 : 8);

		if (iDevType == 0) {
			*(otherTable(0,0)->value) << "Input buffer empty";
			*(otherTable(1,0)->value) << "Special decode bits (8-bit)";
			*(otherTable(2,0)->value) << "Input buffer full history";
			*(otherTable(3,0)->value) << "Special decode bits (4-bit)";
			*(otherTable(4,0)->value) << "InRD0/2 C-code status (8-bit)";
			*(otherTable(5,0)->value) << "InRD1/3 C-code status (8-bit)";
		}

		unsigned int readFIFOStat = thisDDU->readFIFOStat(dt);
		*(otherTable(0,iDevType*2+1)->value) << showbase << hex << ((readFIFOStat >> 8) & 0xff);
		//if (((readFIFOStat >> 8) & 0xff)) otherTable(0,iDevType*2+1)->setClass("bad");
		otherTable(0,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readFIFOStat >> 8) & 0xff) & (1<<iFiber)) {
				*(otherTable(0,iDevType*2+2)->value) << cgicc::div(thisDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","none");
			}
		}

		*(otherTable(1,iDevType*2+1)->value) << showbase << hex << ((readFIFOStat) & 0xff);
		if (((readFIFOStat) & 0xfc)) otherTable(1,iDevType*2+1)->setClass("warning");
		else otherTable(1,iDevType*2+1)->setClass("ok");
		std::map<std::string, std::string> readFIFOStatComments = debugger->FIFOStat(dt,(readFIFOStat) & 0xff);
		for (std::map< std::string, std::string >::iterator iComment = readFIFOStatComments.begin();
			iComment != readFIFOStatComments.end();
			iComment++) {
			*(otherTable(1,iDevType*2+2)->value) << cgicc::div(iComment->first)
				.set("class",iComment->second) << endl;
		}

		unsigned int readFIFOFull = thisDDU->readFIFOFull(dt);
		*(otherTable(2,iDevType*2+1)->value) << showbase << hex << ((readFIFOFull) & 0xff);
		if (((readFIFOFull) & 0xff)) otherTable(2,iDevType*2+1)->setClass("bad");
		else otherTable(2,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readFIFOFull) & 0xff) & (1<<iFiber)) {
				*(otherTable(2,iDevType*2+2)->value) << cgicc::div(thisDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","red");
			}
		}

		*(otherTable(3,iDevType*2+1)->value) << showbase << hex << ((readFIFOFull >> 8) & 0xff);
		if (((readFIFOFull >> 8) & 0xf)) otherTable(3,iDevType*2+1)->setClass("warning");
		else otherTable(3,iDevType*2+1)->setClass("ok");
		std::map<std::string, std::string> readFIFOFullComments = debugger->FIFOFull(dt,(readFIFOFull >> 8) & 0xff);
		for (std::map< std::string, std::string >::iterator iComment = readFIFOFullComments.begin();
			iComment != readFIFOFullComments.end();
			iComment++) {
			*(otherTable(3,iDevType*2+2)->value) << cgicc::div(iComment->first)
				.set("class",iComment->second) << endl;
		}

		unsigned int readCCodeStat = thisDDU->infpga_CcodeStat(dt);
		*(otherTable(4,iDevType*2+1)->value) << showbase << hex << ((readCCodeStat) & 0xff);
		if ((readCCodeStat & 0xff) == 0x20) otherTable(4,iDevType*2+1)->setClass("warning");
		else if (readCCodeStat & 0xff) otherTable(4,iDevType*2+1)->setClass("bad");
		else otherTable(4,iDevType*2+1)->setClass("ok");
		std::map<std::string, std::string> readCCodeStatComments = debugger->CCodeStat(dt,(readCCodeStat) & 0xff);
		for (std::map< std::string, std::string >::iterator iComment = readCCodeStatComments.begin();
			iComment != readCCodeStatComments.end();
			iComment++) {
			*(otherTable(4,iDevType*2+2)->value) << cgicc::div(iComment->first)
				.set("class",iComment->second) << endl;
		}

		*(otherTable(5,iDevType*2+1)->value) << showbase << hex << ((readCCodeStat >> 8) & 0xff);
		if (((readCCodeStat >> 8) & 0xff) == 0x20) otherTable(5,iDevType*2+1)->setClass("warning");
		else if (((readCCodeStat >> 8) & 0xff)) otherTable(5,iDevType*2+1)->setClass("bad");
		else otherTable(5,iDevType*2+1)->setClass("ok");
		readCCodeStatComments = debugger->CCodeStat(dt,(readCCodeStat >> 8) & 0xff);
		for (std::map< std::string, std::string >::iterator iComment = readCCodeStatComments.begin();
			iComment != readCCodeStatComments.end();
			iComment++) {
			*(otherTable(5,iDevType*2+2)->value) << cgicc::div(iComment->first)
				.set("class",iComment->second) << endl;
		}
		
	}

	// Summary
	*out << otherTable.printSummary() << endl;

	// We use a special case of the toHTML method for InFPGAs.
	// Hide the table if all is keen.
	if (otherTable.countClass("ok") + otherTable.countClass("none")
		== otherTable.countRows() * 2) {
		*out << cgicc::table()
			.set("id","otherTable")
			.set("class","data")
			.set("style","display: none;") << endl;
	} else {
		*out << cgicc::table()
			.set("id","otherTable")
			.set("class","data") << endl;
	}

	// Put the titles of the two InFPGAs here.
	*out << cgicc::tr() << endl;
	*out << cgicc::td(" ") << endl;
	*out << cgicc::td(devNames[0])
		.set("colspan","2")
		.set("style","font-weight: bold; text-align: center; border-right: 3px double #000;")<< endl;
	*out << cgicc::td(devNames[1])
		.set("colspan","2")
		.set("style","font-weight: bold; text-align: center;") << endl;
	*out << cgicc::tr() << endl;

	// Now print the concatonated table.
	*out << otherTable.toHTML(false);

	// And now close the table.
	*out << cgicc::table() << endl;

	*out << cgicc::fieldset() << endl;

	// Clever trick to help with formating the cut-and-paste.
	*out << cgicc::br()
		.set("style","display: none") << endl;


	// Display memctrl information
	*out << cgicc::fieldset()
		.set("class","normal") << endl;
	*out << cgicc::div()
		.set("class","legend") << endl;
	*out << cgicc::a("&plusmn;")
		.set("class","openclose")
		.set("onClick","javascript:toggle('memTable')") << endl;
	*out << "Memory Control Information (22 fifos)" << cgicc::div() << endl;

	DataTable memTable("memTable");

	// Names have to come first, because there is only one of each.
	memTable.addColumn("Register");
	memTable.addColumn("Value");
	memTable.addColumn("Decoded Status");
	memTable.addBreak();
	memTable.addColumn("Value");
	memTable.addColumn("Decoded Status");

	for (unsigned int iDevType = 0; iDevType < 2; iDevType++) {
		enum DEVTYPE dt = devTypes[iDevType];

		if (iDevType == 0) {
			*(memTable(0,0)->value) << "FIFOs Used, Fibers 3-0/11-8";
			*(memTable(1,0)->value) << "FIFOs Used, Fibers 7-4/15-12";
			*(memTable(2,0)->value) << "MemCtrl0/2 FIFOs Available";
			*(memTable(3,0)->value) << "MemCtrl1/3 FIFOs Available";
			*(memTable(4,0)->value) << "MemCtrl0/2 Minimum FIFOs Available";
			*(memTable(5,0)->value) << "MemCtrl1/3 Minimum FIFOs Available";
			*(memTable(6,0)->value) << "Write Memory Active, Fibers 1-0/9-8";
			*(memTable(7,0)->value) << "Write Memory Active, Fibers 3-2/11-10";
			*(memTable(8,0)->value) << "Write Memory Active, Fibers 5-4/13-12";
			*(memTable(9,0)->value) << "Write Memory Active, Fibers 7-6/15-14";
		}

		unsigned int memValue = thisDDU->readFiberDiagnostics(dt,0);
		*(memTable(0,iDevType*2+1)->value) << showbase << hex << memValue;
		memTable(0,iDevType*2+1)->setClass("ok");
		std::map< std::string, std::string> memComments = debugger->FiberDiagnostics(dt,0,memValue);
		for (std::map< std::string, std::string >::iterator iComment = memComments.begin();
			iComment != memComments.end();
			iComment++) {
			*(memTable(0,iDevType*2+2)->value) << cgicc::div(iComment->first)
				.set("class",iComment->second) << endl;
		}

		memValue = thisDDU->readFiberDiagnostics(dt,1);
		*(memTable(1,iDevType*2+1)->value) << showbase << hex << memValue;
		memTable(1,iDevType*2+1)->setClass("ok");
		memComments = debugger->FiberDiagnostics(dt,1,memValue);
		for (std::map< std::string, std::string >::iterator iComment = memComments.begin();
			iComment != memComments.end();
			iComment++) {
			*(memTable(1,iDevType*2+2)->value) << cgicc::div(iComment->first)
				.set("class",iComment->second) << endl;
		}

		memValue = thisDDU->infpga_MemAvail(dt);
		*(memTable(2,iDevType*2+1)->value) << (memValue & 0x1f);
		if (memValue & 0x1f < 2) memTable(2,iDevType*2+1)->setClass("warning");
		if (memValue & 0x1f < 1) memTable(2,iDevType*2+1)->setClass("bad");
		if (memValue & 0x1f >=2) memTable(2,iDevType*2+1)->setClass("ok");

		//memValue = thisDDU->infpga_MemAvail(dt);
		*(memTable(3,iDevType*2+1)->value) << ((memValue >> 5) & 0x1f);
		if ((memValue >> 5) & 0x1f < 2) memTable(3,iDevType*2+1)->setClass("warning");
		if ((memValue >> 5) & 0x1f < 1) memTable(3,iDevType*2+1)->setClass("bad");
		if ((memValue >> 5) & 0x1f >=2) memTable(3,iDevType*2+1)->setClass("ok");

		memValue = thisDDU->infpga_Min_Mem(dt);
		*(memTable(4,iDevType*2+1)->value) << (memValue & 0x1f);
		if (memValue & 0x1f < 2) memTable(4,iDevType*2+1)->setClass("warning");
		if (memValue & 0x1f < 1) memTable(4,iDevType*2+1)->setClass("bad");
		if (memValue & 0x1f >=2) memTable(4,iDevType*2+1)->setClass("ok");

		//memValue = thisDDU->infpga_Min_Mem(dt);
		*(memTable(5,iDevType*2+1)->value) << ((memValue >> 5) & 0x1f);
		if ((memValue >> 5) & 0x1f < 2) memTable(5,iDevType*2+1)->setClass("warning");
		if ((memValue >> 5) & 0x1f < 1) memTable(5,iDevType*2+1)->setClass("bad");
		if ((memValue >> 5) & 0x1f >=2) memTable(5,iDevType*2+1)->setClass("ok");

		for (unsigned int ireg = 0; ireg < 4; ireg++) {
			memValue = thisDDU->readWriteMemoryActive(dt,ireg);
			*(memTable(ireg + 6,iDevType*2+1)->value) << showbase << hex << memValue;
			memTable(ireg + 6,iDevType*2+1)->setClass("ok");
			memComments = debugger->WriteMemoryActive(dt,ireg,memValue);
			for (std::map< std::string, std::string >::iterator iComment = memComments.begin();
				iComment != memComments.end();
				iComment++) {
				*(memTable(ireg + 6,iDevType*2+2)->value) << cgicc::div(iComment->first)
					.set("class",iComment->second) << endl;
			}
		}
	}

	// Summary
	*out << memTable.printSummary() << endl;

	// We use a special case of the toHTML method for InFPGAs.
	// Hide the table if all is keen.
	if (memTable.countClass("ok") + memTable.countClass("none")
		== memTable.countRows() * 2) {
		*out << cgicc::table()
			.set("id","memTable")
			.set("class","data")
			.set("style","display: none;") << endl;
	} else {
		*out << cgicc::table()
			.set("id","memTable")
			.set("class","data") << endl;
	}

	// Put the titles of the two InFPGAs here.
	*out << cgicc::tr() << endl;
	*out << cgicc::td(" ") << endl;
	*out << cgicc::td(devNames[0])
		.set("colspan","2")
		.set("style","font-weight: bold; text-align: center; border-right: 3px double #000;")<< endl;
	*out << cgicc::td(devNames[1])
		.set("colspan","2")
		.set("style","font-weight: bold; text-align: center;") << endl;
	*out << cgicc::tr() << endl;

	// Now print the concatonated table.
	*out << memTable.toHTML(false);

	// And now close the table.
	*out << cgicc::table() << endl;

	*out << cgicc::fieldset() << endl;

	// Clever trick to help with formating the cut-and-paste.
	*out << cgicc::br()
		.set("style","display: none") << endl;



	// Display the big debugging information block
	*out << cgicc::fieldset()
		.set("class","fieldset") << endl;
	*out << cgicc::div("InFPGA Diagnostic Trap Decoding")
		.set("class","legend") << endl;

	for (unsigned int iDevType = 0; iDevType < 2; iDevType++) {
		enum DEVTYPE dt = devTypes[iDevType];

		*out << cgicc::span()
			.set("style","background-color: #FFF; border: 2px solid #000; padding: 10px; width: 95%; margin: 10px auto 10px auto; display: block;") << endl;
		*out << cgicc::div(devNames[iDevType])
			.set("style","font-size: 14pt; font-weight: bold; width: 100%; text-align: center;") << endl;

		if (debugTrapValid[iDevType]) {

			// Here it is.
			string bigComments = debugger->infpga_trap(thisDDU, dt);

			ostringstream diagCode;
			diagCode << setfill('0');
			diagCode << hex;
			diagCode << setw(8) << thisDDU->fpga_lcode[5] << " ";
			diagCode << setw(8) << thisDDU->fpga_lcode[4] << " ";
			diagCode << setw(8) << thisDDU->fpga_lcode[3] << " ";
			diagCode << setw(8) << thisDDU->fpga_lcode[2] << " ";
			diagCode << setw(8) << thisDDU->fpga_lcode[1] << " ";
			diagCode << setw(8) << thisDDU->fpga_lcode[0];

			*out << cgicc::div("Trap value (194 bits): ")
				.set("style","font-weight: bold; display: inline;");

			*out << cgicc::div(diagCode.str())
				.set("style","display: inline;") << endl;

			*out << cgicc::div(bigComments)
				.set("style","width: 80%; white-space: pre; font-size: 10pt; margin: 10px auto 10px auto; border: 2px solid #666; padding: 2px; font-family: monospace;") << endl;

			*out << cgicc::div("The status registers are frozen in the trap only after an error occurs.  The values in the trap remain valid until a reset.")
				.set("style","font-size: 8pt;") << endl;

		} else {
			*out << cgicc::div("Diagnostic trap is only valid after an InFPGA error has been detected.")
				.set("style","color: #A00; font-size: 10pt;") << endl;
		}
		
		*out << cgicc::span() << endl;
	}

	*out << cgicc::fieldset() << endl;

	*out << cgicc::hr() << endl;


	// Some expert-only commands.
	*out << cgicc::fieldset()
		.set("class","expert") << endl;
	*out << cgicc::div()
		.set("class","legend") << endl;
	*out << cgicc::a("&plusmn;")
		.set("class","openclose")
		.set("onClick","javascript:toggle('resetContainer')") << endl;
	*out << "InFPGA Resets (Expert Only)" << cgicc::div() << endl;

	*out << cgicc::span()
		.set("id","resetContainer")
		.set("style","display: none") << endl;

	string ddutextload = "/"+getApplicationDescriptor()->getURN()+"/DDUTextLoad";
	ostringstream dduStream;
	dduStream << cgiDDU;
	*out << cgicc::form()
		.set("method","GET")
		.set("action",ddutextload) << endl;
	*out << cgicc::input()
		.set("name","ddu")
		.set("type","hidden")
		.set("value",dduStream.str()) << endl;
	*out << cgicc::input()
		.set("name","val")
		.set("type","hidden")
		.set("value","321") << endl;
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","textdata")
		.set("size","10")
		.set("ENCTYPE","multipart/form-data")
		.set("value","") << endl;
	*out << cgicc::input()
		.set("type","submit")
		.set("value","Reset InFPGA0") << endl;
	*out << cgicc::form() << endl;

	*out << cgicc::form()
		.set("method","GET")
		.set("action",ddutextload) << endl;
	*out << cgicc::input()
		.set("name","ddu")
		.set("type","hidden")
		.set("value",dduStream.str()) << endl;
	*out << cgicc::input()
		.set("name","val")
		.set("type","hidden")
		.set("value","421") << endl;
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","textdata")
		.set("size","10")
		.set("ENCTYPE","multipart/form-data")
		.set("value","") << endl;
	*out << cgicc::input()
		.set("type","submit")
		.set("value","Reset InFPGA1") << endl;
	*out << cgicc::form() << endl;

	*out << cgicc::span() << endl;

	*out << cgicc::fieldset() << endl;

	*out << Footer() << endl;

	/*
	try {
		
		printf(" enter: INFpga0 \n");
		cgicc::Cgicc cgi(in);
		const CgiEnvironment& env = cgi.getEnvironment();
		string crateStr = env.getQueryString() ;
		cout << crateStr << endl ;
		cgicc::form_iterator name = cgi.getElement("ddu");
		if(name != cgi.getElements().end()) {
			ddu = cgi["ddu"]->getIntegerValue();
			cout << "DDU inside " << ddu << endl;
			DDU_ = ddu;
		}else{
			ddu=DDU_;
		}
		printf(" DDU %d \n",ddu);
		thisDDU = dduVector[ddu];
		printf(" set up web page \n");
		*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << endl;
		*out << cgicc::html().set("lang", "en").set("dir","ltr") << endl;
		*out << cgicc::title("INFPGA0 Web Form") << endl;
		sprintf(buf,"DDU INFPGA0, VME  Slot %d",thisDDU->slot());
		*out << body().set("background","/tmp/bgndcms.jpg");
		*out << cgicc::div().set("style","font-size: 16pt; font-weight: bold; color: #D00; width: 400px; margin-left: auto; margin-right: auto; text-align: center;") << "Crate " << thisCrate->number() << " Selected" << cgicc::div() << endl;
		*out << "<h2 align=center><font color=blue>" << buf << "</font></h2>" << endl;
		
		char buf[300],buf2[300],buf3[300],buf4[30];
		int ddu,icrit,icond,icond2;
		unsigned long int stat;

		for(int i=300;i<322;i++){
			printf(" LOOP: %d \n",i);
			thisDDU->infpga_shift0=0x0000;
			thisCrate->vmeController()->CAEN_err_reset();
			sprintf(buf3," ");
			sprintf(buf4," ");
			icond=0;
			icond2=0;
			if(i==300){
				thisDDU->infpgastat(INFPGA0);
				stat=((0xffff&thisDDU->infpga_code1)<<16)|(0xffff&thisDDU->infpga_code0);
				sprintf(buf,"infpga0 32-bit Status:");
				sprintf(buf2," %08lX ",stat);
				icrit=0;
				if((0x00004000&stat)>0)icond=1;
				if((0x00008000&stat)>0){
					icrit=1;
					icond=2;
				}
				else if((0xf0004202&stat)>0)icond=1;
			}
			if(i==301){
				thisDDU->infpga_rdscaler(INFPGA0);
				sprintf(buf,"infpga0 L1 Event Scaler0 bits[23-0]:");
				sprintf(buf2," %02X%04X ",thisDDU->infpga_code1,thisDDU->infpga_code0);
				stat=((thisDDU->infpga_code1)<<16)+thisDDU->infpga_code0;
				sprintf(buf4," = %8lu Dec",stat&0x00ffffff);
			}
			if(i==302){
				thisDDU->infpga_rd1scaler(INFPGA0);
				sprintf(buf,"infpga0 L1 Event Scaler1 bits[23-0]:");
				sprintf(buf2," %02X%04X ",thisDDU->infpga_code1,thisDDU->infpga_code0);
				stat=((thisDDU->infpga_code1)<<16)+thisDDU->infpga_code0;
				sprintf(buf4," = %8lu Dec",stat&0x00ffffff);
			}
			if(i==303){
				thisDDU->infpga_DMBwarn(INFPGA0);
				*out << br() << " <font color=blue> Fiber Registers below flag which CSCs experienced each condition since last Reset</font>" << br() << endl;
				sprintf(buf,"infpga0 DMB Full, Fiber[7-0]:");
				sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
				if(thisDDU->infpga_code0&0xff00)icond=1;
				sprintf(buf3," &nbsp &nbsp DMB Warn, Fiber[7-0]:");
				sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
				//	if(thisDDU->infpga_code0&0x00ff)icond2=1;
			}
			if(i==304){
				thisDDU->infpga_CheckFiber(INFPGA0);
				sprintf(buf,"infpga0 Connection Error, Fiber[7-0]:");
				sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
				if(thisDDU->infpga_code0&0xff00)icond=2;
				sprintf(buf3," &nbsp &nbsp Link Active, Fiber[7-0]:");
				sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
			}
			if(i==305){
				thisDDU->infpga_DMBsync(INFPGA0);
				sprintf(buf,"infpga0 Stuck Data, Fiber[7-0]:");
				sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
				if(thisDDU->infpga_code0&0xff00)icond=2;
				sprintf(buf3," &nbsp &nbsp L1A Mismatch, Fiber[7-0]:");
				sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
				if(thisDDU->infpga_code0&0x00ff)icond2=1;
			}
			if(i==306){
				thisDDU->infpga_RxErr(INFPGA0);
				sprintf(buf,"infpga0 DDU GT-Rx Error, Fiber[7-0]:");
				sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
				sprintf(buf3," &nbsp &nbsp Timeout-Start, Fiber[7-0]:");
				sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
				if(thisDDU->infpga_code0&0xff00)icond2=3;
				if(thisDDU->infpga_code0&0x00ff)icond2=2;
			}
			if(i==307){
				thisDDU->infpga_Timeout(INFPGA0);
				sprintf(buf,"infpga0 Timeout-EndBusy, Fiber[7-0]:");
				sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
				if(thisDDU->infpga_code0&0xff00)icond=2;
				sprintf(buf3," &nbsp &nbsp Timeout-EndWait, F[7-0]:");
				sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
				if(thisDDU->infpga_code0&0x00ff)icond2=2;
			}
			if(i==308){
				thisDDU->infpga_XmitErr(INFPGA0);
				sprintf(buf,"infpga0 SCA Full history, Fiber[7-0]:");
				sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
				sprintf(buf3," &nbsp &nbsp CSC Transmit Error, Fiber[7-0]:");
				sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
				if(thisDDU->infpga_code0&0x00ff)icond2=1;
			}
			if(i==309){
				thisDDU->infpga_LostErr(INFPGA0);
				sprintf(buf,"infpga0 DDU LostInEvent Error, Fiber[7-0]:");
				sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
				if(thisDDU->infpga_code0&0xff00)icond=1;
				sprintf(buf3," &nbsp &nbsp DDU LostInData Error, F[7-0]:");
				sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
				if(thisDDU->infpga_code0&0x00ff)icond2=2;
			}
			if(i==310){
				thisDDU->infpga_FIFOstatus(INFPGA0);
				*out << br() << " <font color=blue> These registers need individual bit Decoding</font>" << br() << endl;
				sprintf(buf,"infpga0 Input Buffer Empty, Fiber[7-0]:");
				sprintf(buf2," %02X ",(thisDDU->infpga_code0&0x00ff));
				sprintf(buf3," &nbsp &nbsp special decode (8-bit):");
				sprintf(buf4," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
				stat=thisDDU->infpga_code0;
				if(0xfc00&stat)icond2=1;
			}
			if(i==311){
				thisDDU->infpga_FIFOfull(INFPGA0);
				sprintf(buf,"infpga0 Input Buffer Full history, Fiber[7-0]:");
				sprintf(buf2," %02X ",(thisDDU->infpga_code0&0x00ff));
				if(thisDDU->infpga_code0&0x00ff)icond=2;
				sprintf(buf3," &nbsp &nbsp special decode (4-bit):");
				sprintf(buf4," %01X ",(thisDDU->infpga_code0&0x0f00)>>8);
				if(thisDDU->infpga_code0&0x0f00)icond2=2;
				stat=thisDDU->infpga_code0;
			}
			if(i==312){
				thisDDU->infpga_CcodeStat(INFPGA0);
				sprintf(buf,"infpga0 InRD0 C-code status:");
				sprintf(buf2," %02X ",(thisDDU->infpga_code0&0x00ff));
				if(thisDDU->infpga_code0&0x0020)icond=1;
				if(thisDDU->infpga_code0&0x00df)icond=2;
				sprintf(buf3," &nbsp &nbsp InRD1 C-code status:");
				sprintf(buf4," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
				if(thisDDU->infpga_code0&0x2000)icond2=1;
				if(thisDDU->infpga_code0&0xdf00)icond2=2;
				stat=thisDDU->infpga_code0;
			}

			if(i==313){
				*out << br() << " <font color=blue> Each MemCtrl unit has a pool of 22 internal FIFOs</font>" << br() << endl;
				thisDDU->infpga_FiberDiagA(INFPGA0);
				sprintf(buf,"infpga0 MemCtrl-0 #FIFOs Used, Fibers 3-0: ");
				sprintf(buf2,"%ld, %ld, %ld, %ld <br>",(0x1f000000&thisDDU->fpga_lcode[0])>>24,(0x001f0000&thisDDU->fpga_lcode[0])>>16,(0x00001f00&thisDDU->fpga_lcode[0])>>8,0x0000001f&thisDDU->fpga_lcode[0]);
				thisDDU->infpga_FiberDiagB(INFPGA0);
				sprintf(buf3,"infpga0 MemCtrl-1 #FIFOs Used, Fibers 7-4: ");
				sprintf(buf4,"%ld, %ld, %ld, %ld",(0x1f000000&thisDDU->fpga_lcode[0])>>24,(0x001f0000&thisDDU->fpga_lcode[0])>>16,(0x00001f00&thisDDU->fpga_lcode[0])>>8,0x0000001f&thisDDU->fpga_lcode[0]);
			}
			if(i==314){
				thisDDU->infpga_MemAvail(INFPGA0);
				sprintf(buf,"infpga0 Current FIFO Memory Available:");
				sprintf(buf2,"MemCtrl-0 = %2d free, ",thisDDU->infpga_code0&0x001f);
				sprintf(buf4," &nbsp MemCtrl-1 = %2d free",thisDDU->infpga_code1);
				if(thisDDU->infpga_code0==1)icond=1;
				if(thisDDU->infpga_code0==0)icond=2;
				if(thisDDU->infpga_code1==1)icond2=1;
				if(thisDDU->infpga_code1==0)icond2=2;
			}
			if(i==315){
				thisDDU->infpga_Min_Mem(INFPGA0);
				sprintf(buf,"infpga0 Minimum FIFO Memory Availabile:");
				sprintf(buf2,"MemCtrl-0 min = %d free, ",thisDDU->infpga_code0&0x001f);
				sprintf(buf4," &nbsp MemCtrl-1 min = %d free",thisDDU->infpga_code1);
				if(thisDDU->infpga_code0==1)icond=1;
				if(thisDDU->infpga_code0==0)icond=2;
				if(thisDDU->infpga_code1==1)icond2=1;
				if(thisDDU->infpga_code1==0)icond2=2;
			}

			if(i==316){
				thisDDU->infpga_WrMemActive(INFPGA0,0);
				sprintf(buf,"infpga0 Write Memory Active 0-1:");
				sprintf(buf2," Fiber 0 writing to %02Xh, &nbsp ",thisDDU->infpga_code0);
				if((thisDDU->infpga_code0&0x001f)==0x001f)sprintf(buf2," Fiber 0 Unused, &nbsp ");
				sprintf(buf4," Fiber 1 writing to %02Xh",thisDDU->infpga_code1);
				if((thisDDU->infpga_code1&0x001f)==0x001f)sprintf(buf4," Fiber 1 Unused");
			}
			if(i==317){
				thisDDU->infpga_WrMemActive(INFPGA0,1);
				sprintf(buf,"infpga0 Write Memory Active 2-3:");
				sprintf(buf2," Fiber 2 writing to %02Xh, &nbsp ",thisDDU->infpga_code0);
				if((thisDDU->infpga_code0&0x001f)==0x001f)sprintf(buf2," Fiber 2 Unused, &nbsp ");
				sprintf(buf4," Fiber 3 writing to %02Xh",thisDDU->infpga_code1);
				if((thisDDU->infpga_code1&0x001f)==0x001f)sprintf(buf4," Fiber 3 Unused");
			}
			if(i==318){
				thisDDU->infpga_WrMemActive(INFPGA0,2);
				sprintf(buf,"infpga0 Write Memory Active 4-5:");
				sprintf(buf2," Fiber 4 writing to %02Xh, &nbsp ",thisDDU->infpga_code0);
				if((thisDDU->infpga_code0&0x001f)==0x001f)sprintf(buf2," Fiber 4 Unused, &nbsp ");
				sprintf(buf4," Fiber 5 writing to %02Xh",thisDDU->infpga_code1);
				if((thisDDU->infpga_code1&0x001f)==0x001f)sprintf(buf4," Fiber 5 Unused");
			}
			if(i==319){
				thisDDU->infpga_WrMemActive(INFPGA0,3);
				sprintf(buf,"infpga0 Write Memory Active 6-7:");
				sprintf(buf2," Fiber 6 writing to %02Xh, &nbsp ",thisDDU->infpga_code0);
				if((thisDDU->infpga_code0&0x001f)==0x001f)sprintf(buf2," Fiber 6 Unused, &nbsp ");
				sprintf(buf4," Fiber 7 writing to %02Xh",thisDDU->infpga_code1);
				if((thisDDU->infpga_code1&0x001f)==0x001f)sprintf(buf4," Fiber 7 Unused");
			}
			if(i==320){
				if(icrit>0){
					thisDDU->infpga_trap(INFPGA0);
					*out << br() << " <font color=blue> Diagnostic data only valid after Critical Error, traps conditions at that instant</font>" << br() << endl;
					sprintf(buf,"infpga0 diagnostic trap:");
					sprintf(buf2," %08lX %08lX %08lX %08lX %08lX %08lX",thisDDU->fpga_lcode[5],thisDDU->fpga_lcode[4],thisDDU->fpga_lcode[3],thisDDU->fpga_lcode[2],thisDDU->fpga_lcode[1],thisDDU->fpga_lcode[0]);
				}else{
					sprintf(buf," ");
					sprintf(buf2," ");
					thisDDU->infpga_shift0=0xFACE;
				}
			}
			if(i==321){
				sprintf(buf,"infpga0 reset: ");
				sprintf(buf2," EXPERT ONLY! ");
				icond=2;
			}

			if(i==321){
				string ddutextload =
				toolbox::toString("/%s/DDUTextLoad",getApplicationDescriptor()->getURN().c_str());
				*out << cgicc::form().set("method","GET").set("action",ddutextload) << endl;
			}

			*out << cgicc::span().set("style","color:black");
			*out << buf << cgicc::span();
			if(icond==1){
				*out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
			}else if(icond==2){
				*out << cgicc::span().set("style","color:red;background-color:#dddddd;");
			}else{
				*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
			}
			*out << buf2 << cgicc::span();
			*out << cgicc::span().set("style","color:black");
			*out << buf3 << cgicc::span();

			if(icond2==1){
				*out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
			}else if(icond2==2){
				*out << cgicc::span().set("style","color:red;background-color:#dddddd;");
			}else if(icond2==3){
				*out << cgicc::span().set("style","color:blue;background-color:#dddddd;");
			}else{
				*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
			}
			*out << buf4 << cgicc::span();
			if(thisCrate->vmeController()->CAEN_err()!=0){
				*out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
				*out << " **CAEN Error " << cgicc::span();
			}
			if((thisDDU->infpga_shift0!=0xFACE)&&i!=321){
				sprintf(buf," **JTAG Error, Shifted:%04X",thisDDU->infpga_shift0);
				*out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
				*out << buf << cgicc::span();
			}

			if(i==321){
				*out << cgicc::input().set("type","hidden")
				.set("name","textdata")
				.set("size","10")
				.set("ENCTYPE","multipart/form-data")
				.set("value","")
				.set("style","font-size: 13pt; font-family: arial;")<<endl;
				*out << cgicc::input().set("type","submit")
				.set("value","reset");
				sprintf(buf,"%d",ddu);
				*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ddu");
				sprintf(buf,"%d",i);
				*out << cgicc::input().set("type","hidden").set("value",buf).set("name","val");
				*out << cgicc::form() << endl;
			}else if(i==300&&(stat&0x0000f000)>0){
				*out << "<blockquote><font size=-1 color=red face=arial>";
				if((stat&0xF0000000)>0){
					if((0x80000000&stat)>0) *out << " DLL2 Lock Error &nbsp ";
					// if((0x80000000&stat)>0) *out << " DLL-2 Not Locked &nbsp ";
					// if((0x40000000&stat)>0) *out << " DLL-1 Not Locked &nbsp ";
					if((0x40000000&stat)>0) *out << " 64-bit Filler was used &nbsp ";
					if((0x20000000&stat)>0) *out << " RdCtrl-1 Not Ready &nbsp ";
					if((0x10000000&stat)>0) *out << " RdCtrl-0 Not Ready";
					*out << br();
				}
				if((stat&0x0F000000)>0){
					if((0x08000000&stat)>0) *out << " <font color=blue>NoLiveFiber 0 or 1</font> &nbsp ";
					if((0x04000000&stat)>0) *out << " <font color=blue>DLL Error occurred</font> &nbsp ";
					if((0x02000000&stat)>0) *out << " <font color=black>InRD1 DMB Warn</font> &nbsp ";
					if((0x01000000&stat)>0) *out << " <font color=black>InRD0 DMB Warn</font>";
					*out << br();
				}
				if((stat&0x00F00000)>0){
					if((0x00800000&stat)>0) *out << " <font color=blue>InRD1 DMB Full</font> &nbsp ";
					if((0x00400000&stat)>0) *out << " Mem/FIFO-InRD1 Error &nbsp ";
					if((0x00200000&stat)>0) *out << " MultL1A Error-InRD1 &nbsp ";
					if((0x00100000&stat)>0) *out << " <font color=black>NoLiveFiber4-7</font>";
					*out << br();
				}
				if((stat&0x000F0000)>0){
					if((0x00080000&stat)>0) *out << " <font color=blue>InRD0 DMB Full</font> &nbsp ";
					if((0x00040000&stat)>0) *out << " Mem/FIFO-InRD0 Error &nbsp ";
					if((0x00020000&stat)>0) *out << " MultL1A Error-InRD0 &nbsp ";
					if((0x00010000&stat)>0) *out << " <font color=black>NoLiveFiber0-3</font>";
					*out << br();
				}
				// JRG, low-order 16-bit status (most serious errors):
				if((stat&0x0000F000)>0){
					if((0x00008000&stat)>0) *out << " <font color=red>Critical Error ** needs reset **</font> &nbsp ";
					if((0x00004000&stat)>0) *out << " <font color=orange>Single Error, bad event</font> &nbsp ";
					if((0x00002000&stat)>0) *out << " <font color=blue>Single warning, possible data problem</font> &nbsp ";
					if((0x00001000&stat)>0) *out << " <font color=blue>Near Full Warning</font>";
					*out << br();
				}
				if((stat&0x00000F00)>0){
					if((0x00000800&stat)>0) *out << " <font color=blue>RX Error occurred</font> &nbsp ";
					if((0x00000400&stat)>0) *out << " <font color=blue>DLL Error (recent)</font> &nbsp ";
					if((0x00000200&stat)>0) *out << " <font color=orange>SCA Full detected</font> &nbsp ";
					if((0x00000100&stat)>0) *out << " <font color=blue>Special Word voted-bit warning</font>";
					*out << br();
				}
				if((stat&0x000000F0)>0){
					if((0x00000080&stat)>0) *out << " Stuck Data occurred &nbsp ";
					if((0x00000040&stat)>0) *out << " Timeout occurred &nbsp ";
					if((0x00000020&stat)>0) *out << " Multiple voted-bit Errors &nbsp ";
					if((0x00000010&stat)>0) *out << " Multiple Transmit Errors";
					*out << br();
				}
				if((stat&0x0000000F)>0){
					if((0x00000008&stat)>0) *out << " Mem/FIFO Full Error &nbsp ";
					if((0x00000004&stat)>0) *out << " Fiber Error &nbsp ";
					if((0x00000002&stat)>0) *out << " <font color=orange>L1A Match Error</font> &nbsp ";
					if((0x00000001&stat)>0) *out << " Not Ready Error";
					*out << br();
				}
				*out << "</font></blockquote>";
			}else if(i==320 && icrit>0){
				*out << "<blockquote><font size=-2 color=black face=arial>";
				DDUinTrapDecode(in, out, thisDDU->fpga_lcode);
				*out << "</font></blockquote>";
			}else if(i==310&&(stat&0xff00)>0){
				*out << endl ;
				*out << "<blockquote><font size=-1 color=black face=arial>";
				if((stat&0x00005500)>0){
					*out << " &nbsp InRD0 Status: &nbsp <font color=blue>";
					if((0x00004000&stat)>0) *out << " Ext.FIFO 3/4 Full &nbsp &nbsp ";
					if((0x00001000&stat)>0) *out << " L1A FIFO Almost Full &nbsp &nbsp ";
					if((0x00000400&stat)>0) *out << " MemCtrl Almost Full &nbsp &nbsp ";
					*out << "</font>";
					if((0x00000100&stat)>0) *out << " L1A FIFO Empty";
					if((0x00000100&stat)==0) *out << " L1A FIFO Not Empty";
					*out << br();
				}
				if((stat&0x0000AA00)>0){
					*out << " &nbsp InRD1 Status: &nbsp <font color=blue>";
					if((0x00008000&stat)>0) *out << " Ext.FIFO 3/4 Full &nbsp &nbsp ";
					if((0x00002000&stat)>0) *out << " L1A FIFO Almost Full &nbsp &nbsp ";
					if((0x00000800&stat)>0) *out << " MemCtrl Almost Full &nbsp &nbsp ";
					*out << "</font>";
					if((0x00000200&stat)>0) *out << " L1A FIFO Empty";
					if((0x00000200&stat)==0) *out << " L1A FIFO Not Empty";
					*out << br();
				}
				*out << "</font></blockquote>";
			}else if(i==311&&(stat&0x0f00)>0){
				*out << "<blockquote><font size=-1 color=black face=arial>";
				if((stat&0x00000500)>0){
					*out << " &nbsp InRD0 Status: &nbsp <font color=red>";
					if((0x00000400&stat)>0) *out << " Ext.FIFO Full Occurred &nbsp &nbsp ";
					if((0x00000100&stat)>0) *out << " L1A FIFO Full Occurred";
					*out << "</font>" << br();
				}
				if((stat&0x00000A00)>0){
					*out << " &nbsp InRD1 Status: &nbsp <font color=red>";
					if((0x00000800&stat)>0) *out << " Ext.FIFO Full Occurred &nbsp &nbsp ";
					if((0x00000200&stat)>0) *out << " L1A FIFO Full Occurred";
					*out << "</font>" << br();
				}
				*out << "</font></blockquote>";
			}else if(i==312&&(stat&0xffff)>0){
				*out << "<blockquote><font size=-1 color=black face=arial>";
				if((stat&0x000000ff)>0){
					*out << " &nbsp InRD0: &nbsp <font color=red>";
					if((0x000080&stat)>0) *out << " Critical Error ** needs reset ** &nbsp &nbsp ";
					if((0x000040&stat)>0) *out << " Sync Error ** needs reset ** &nbsp &nbsp ";
					if((0x000020&stat)>0) *out << " <font color=orange>Single Error</font> &nbsp &nbsp ";
					if((0x000010&stat)>0) *out << " FIFO Overflow detected &nbsp &nbsp ";
					if((0x000008&stat)>0) *out << " Fiber Connection Error &nbsp &nbsp ";
					if((0x000004&stat)>0) *out << " Multi-Transmit Error &nbsp &nbsp ";
					if((0x000002&stat)>0) *out << " Stuck Data &nbsp &nbsp ";
					if((0x000001&stat)>0) *out << " Timeout";
					*out  << "</font>" << br();
				}
				if((stat&0x0000ff00)>0){
					*out << " &nbsp InRD1 &nbsp <font color=red>";
					if((0x0008000&stat)>0) *out << " Critical Error ** needs reset ** &nbsp &nbsp ";
					if((0x00004000&stat)>0) *out << " Sync Error ** needs reset ** &nbsp &nbsp ";
					if((0x00002000&stat)>0) *out << " <font color=orange>Single Error</font> &nbsp &nbsp ";
					if((0x00001000&stat)>0) *out << " FIFO Overflow detected &nbsp &nbsp ";
					if((0x00000800&stat)>0) *out << " Fiber Connection Error &nbsp &nbsp ";
					if((0x00000400&stat)>0) *out << " Multi-Transmit Error &nbsp &nbsp ";
					if((0x00000200&stat)>0) *out << " Stuck Data &nbsp &nbsp ";
					if((0x00000100&stat)>0) *out << " Timeout";
					*out  << "</font>" << br();
				}
				*out  << "</font></blockquote>" << br();
			}else{
				*out << br() << endl;
			}
		}

		*out << cgicc::body() << endl;
		*out << cgicc::html() << endl;

	} catch (const exception & e ) {
		//XECPT_RAISE(xgi::exception::Exception, e.what());
	}
	*/
}


/*
void EmuFCrateHyperDAQ::INFpga1(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	try {
		printf(" enter: INFpga1 \n");
		cgicc::Cgicc cgi(in);
		const CgiEnvironment& env = cgi.getEnvironment();
		string crateStr = env.getQueryString() ;
		cout << crateStr << endl ;
		cgicc::form_iterator name = cgi.getElement("ddu");
		int ddu,icrit,icond,icond2;
		unsigned long int stat;
		if(name != cgi.getElements().end()) {
			ddu = cgi["ddu"]->getIntegerValue();
			cout << "DDU inside " << ddu << endl;
			DDU_ = ddu;
		}else{
			ddu=DDU_;
		}
		printf(" DDU %d \n",ddu);
		thisDDU = dduVector[ddu];
		printf(" set up web page \n");
		*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << endl;
		*out << cgicc::html().set("lang", "en").set("dir","ltr") << endl;
		*out << cgicc::title("INFPGA1 Web Form") << endl;
		*out << body().set("background","/tmp/bgndcms.jpg");
		*out << cgicc::div().set("style","font-size: 16pt; font-weight: bold; color: #D00; width: 400px; margin-left: auto; margin-right: auto; text-align: center;") << "Crate " << thisCrate->number() << " Selected" << cgicc::div() << endl;

		char buf[300],buf2[300],buf3[300],buf4[30];
		sprintf(buf,"DDU INFPGA1, VME  Slot %d",thisDDU->slot());
		*out << "<h2 align=center><font color=blue>" << buf << "</font></h2>" << endl;

		for(int i=400;i<422;i++){
			printf(" LOOP: %d \n",i);
			thisDDU->infpga_shift0=0x0000;
			thisCrate->vmeController()->CAEN_err_reset();
			sprintf(buf3," ");
			sprintf(buf4," ");
			icond=0;
			icond2=0;
			if(i==400){
				thisDDU->infpgastat(INFPGA1);
				stat=((0xffff&thisDDU->infpga_code1)<<16)|(0xffff&thisDDU->infpga_code0);
				sprintf(buf,"infpga1 32-bit Status:");
				sprintf(buf2," %08lX ",stat);
				icrit=0;
				if((0x00004000&stat)>0)icond=1;
				if((0x00008000&stat)>0){
					icrit=1;
					icond=2;
				}
				else if((0xf0004202&stat)>0)icond=1;
			}
			if(i==401){
				thisDDU->infpga_rdscaler(INFPGA1);
				sprintf(buf,"infpga1 L1 Event Scaler0 bits[23-0]:");
				sprintf(buf2," %02X%04X ",thisDDU->infpga_code1,thisDDU->infpga_code0);
				stat=((thisDDU->infpga_code1)<<16)+thisDDU->infpga_code0;
				sprintf(buf4," = %8lu Dec",stat&0x00ffffff);
			}
			if(i==402){
				thisDDU->infpga_rd1scaler(INFPGA1);
				sprintf(buf,"infpga1 L1 Event Scaler1 bits[23-0]:");
				sprintf(buf2," %02X%04X ",thisDDU->infpga_code1,thisDDU->infpga_code0);
				stat=((thisDDU->infpga_code1)<<16)+thisDDU->infpga_code0;
				sprintf(buf4," = %8lu Dec",stat&0x00ffffff);
			}
			if(i==403){
				thisDDU->infpga_DMBwarn(INFPGA1);
				*out << br() << " <font color=blue> Fiber Registers below flag which CSCs experienced each condition since last Reset</font>" << br() << endl;
				sprintf(buf,"infpga1 DMB Full, Fiber[14-8]:");
				sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
				if(thisDDU->infpga_code0&0xff00)icond=1;
				sprintf(buf3," &nbsp &nbsp DMB Warn, Fiber[14-8]:");
				sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
				//	if(thisDDU->infpga_code0&0x00ff)icond2=1;
			}
			if(i==404){
				thisDDU->infpga_CheckFiber(INFPGA1);
				sprintf(buf,"infpga1 Connection Error, Fiber[14-8]:");
				sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
				if(thisDDU->infpga_code0&0xff00)icond=2;
				sprintf(buf3," &nbsp &nbsp Link Active, Fiber[14-8]:");
				sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
			}
			if(i==405){
				thisDDU->infpga_DMBsync(INFPGA1);
				sprintf(buf,"infpga1 Stuck Data, Fiber[14-8]:");
				sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
				if(thisDDU->infpga_code0&0xff00)icond=2;
				sprintf(buf3," &nbsp &nbsp L1A Mismatch, Fiber[14-8]:");
				sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
				if(thisDDU->infpga_code0&0x00ff)icond2=1;
			}
			if(i==406){
				thisDDU->infpga_RxErr(INFPGA1);
				sprintf(buf,"infpga1 DDU GT-Rx Error, Fiber[14-8]:");
				sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
				sprintf(buf3," &nbsp &nbsp DDU Timeout-start, Fiber[14-8]:");
				sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
				if(thisDDU->infpga_code0&0xff00)icond2=3;
				if(thisDDU->infpga_code0&0x00ff)icond2=2;
			}
			if(i==407){
				thisDDU->infpga_Timeout(INFPGA1);
				sprintf(buf,"infpga1 DDU Timeout-EndBusy, Fiber[14-8]:");
				sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
				if(thisDDU->infpga_code0&0xff00)icond=2;
				sprintf(buf3," &nbsp &nbsp DDU Timeout-EndWait, F[14-8]:");
				sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
				if(thisDDU->infpga_code0&0x00ff)icond2=2;
			}
			if(i==408){
				thisDDU->infpga_XmitErr(INFPGA1);
				sprintf(buf,"infpga1 SCA Full history, Fiber[14-8]:");
				sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
				sprintf(buf3," &nbsp &nbsp CSC Transmit Error, Fiber[14-8]:");
				sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
				if(thisDDU->infpga_code0&0x00ff)icond2=1;
			}
			if(i==409){
				thisDDU->infpga_LostErr(INFPGA1);
				sprintf(buf,"infpga1 DDU LostInEvent Error, Fiber[14-8]:");
				sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
				if(thisDDU->infpga_code0&0xff00)icond=1;
				sprintf(buf3," &nbsp &nbsp DDU LostInData Error, F[14-8]:");
				sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
				if(thisDDU->infpga_code0&0x00ff)icond2=2;
			}
			if(i==410){
				thisDDU->infpga_FIFOstatus(INFPGA1);
				*out << br() << " <font color=blue> These registers need individual bit Decoding</font>" << br() << endl;
				sprintf(buf,"infpga1 Input Buffer Empty, Fiber[14-8]:");
				sprintf(buf2," %02X ",(thisDDU->infpga_code0&0x00ff));
				sprintf(buf3," &nbsp &nbsp special decode (8-bit):");
				sprintf(buf4," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
				stat=thisDDU->infpga_code0;
				if(0xfc00&stat)icond2=1;
			}
			if(i==411){
				thisDDU->infpga_FIFOfull(INFPGA1);
				sprintf(buf,"infpga1 Input Buffer Full history, Fiber[14-8]:");
				sprintf(buf2," %02X ",(thisDDU->infpga_code0&0x00ff));
				if(thisDDU->infpga_code0&0x00ff)icond=2;
				sprintf(buf3," &nbsp &nbsp special decode (4-bit):");
				sprintf(buf4," %01X ",(thisDDU->infpga_code0&0x0f00)>>8);
				if(thisDDU->infpga_code0&0x0f00)icond2=2;
				stat=thisDDU->infpga_code0;
			}
			if(i==412){
				thisDDU->infpga_CcodeStat(INFPGA1);
				sprintf(buf,"infpga1 InRD2 C-code status:");
				sprintf(buf2," %02X ",(thisDDU->infpga_code0&0x00ff));
				if(thisDDU->infpga_code0&0x0020)icond=1;
				if(thisDDU->infpga_code0&0x00df)icond=2;
				sprintf(buf3," &nbsp &nbsp InRD3 C-code status:");
				sprintf(buf4," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
				if(thisDDU->infpga_code0&0x2000)icond2=1;
				if(thisDDU->infpga_code0&0xdf00)icond2=2;
				stat=thisDDU->infpga_code0;
			}
			if(i==413){
				*out << br() << " <font color=blue> Each MemCtrl unit has a pool of 22 internal FIFOs</font>" << br() << endl;
				thisDDU->infpga_FiberDiagA(INFPGA1);
				sprintf(buf,"infpga1 MemCtrl-2 #FIFOs Used, Fibers 11-8: ");
				sprintf(buf2,"%ld, %ld, %ld, %ld <br>",(0x1f000000&thisDDU->fpga_lcode[0])>>24,(0x001f0000&thisDDU->fpga_lcode[0])>>16,(0x00001f00&thisDDU->fpga_lcode[0])>>8,0x0000001f&thisDDU->fpga_lcode[0]);
				thisDDU->infpga_FiberDiagB(INFPGA1);
				sprintf(buf3,"infpga1 MemCtrl-3 #FIFOs Used, Fibers 14-12: ");
				sprintf(buf4,"%ld, %ld, %ld",(0x1f000000&thisDDU->fpga_lcode[0])>>24,(0x00001f00&thisDDU->fpga_lcode[0])>>8,0x0000001f&thisDDU->fpga_lcode[0]);
			}
			if(i==414){
				thisDDU->infpga_MemAvail(INFPGA1);
				sprintf(buf,"infpga1 Current FIFO Memory Available:");
				sprintf(buf2,"MemCtrl-2 = %2d free, ",thisDDU->infpga_code0&0x001f);
				sprintf(buf4," &nbsp MemCtrl-3 = %2d free",thisDDU->infpga_code1);
				if(thisDDU->infpga_code0==1)icond=1;
				if(thisDDU->infpga_code0==0)icond=2;
				if(thisDDU->infpga_code1==1)icond2=1;
				if(thisDDU->infpga_code1==0)icond2=2;
			}
			if(i==415){
				thisDDU->infpga_Min_Mem(INFPGA1);
				sprintf(buf,"infpga1 Minimum FIFO Memory Availabile:");
				sprintf(buf2,"MemCtrl-2 min = %d free, ",thisDDU->infpga_code0&0x001f);
				sprintf(buf4," &nbsp MemCtrl-3 min = %d free",thisDDU->infpga_code1);
				if(thisDDU->infpga_code0==1)icond=1;
				if(thisDDU->infpga_code0==0)icond=2;
				if(thisDDU->infpga_code1==1)icond2=1;
				if(thisDDU->infpga_code1==0)icond2=2;
			}

			if(i==416){
				thisDDU->infpga_WrMemActive(INFPGA1,0);
				sprintf(buf,"infpga1 Write Memory Active 8-9: ");
				sprintf(buf2," Fiber 8 writing to %02Xh, &nbsp ",thisDDU->infpga_code0);
				if((thisDDU->infpga_code0&0x001f)==0x001f)sprintf(buf2," Fiber 8 Unused, &nbsp ");
				sprintf(buf4," Fiber 9 writing to %02Xh",thisDDU->infpga_code1);
				if((thisDDU->infpga_code1&0x001f)==0x001f)sprintf(buf4," Fiber 9 Unused");
			}
			if(i==417){
				thisDDU->infpga_WrMemActive(INFPGA1,1);
				sprintf(buf,"infpga1 Write Memory Active 10-11: ");
				sprintf(buf2," Fiber 10 writing to %02Xh, &nbsp ",thisDDU->infpga_code0);
				if((thisDDU->infpga_code0&0x001f)==0x001f)sprintf(buf2," Fiber 10 Unused, &nbsp ");
				sprintf(buf4," Fiber 11 writing to %02Xh",thisDDU->infpga_code1);
				if((thisDDU->infpga_code1&0x001f)==0x001f)sprintf(buf4," Fiber 11 Unused");
			}
			if(i==418){
				thisDDU->infpga_WrMemActive(INFPGA1,2);
				sprintf(buf,"infpga1 Write Memory Active 12-13: ");
				sprintf(buf2," Fiber 12 writing to %02Xh, &nbsp ",thisDDU->infpga_code0);
				if((thisDDU->infpga_code0&0x001f)==0x001f)sprintf(buf2," Fiber 12 Unused, &nbsp ");
				sprintf(buf4," Fiber 13 writing to %02Xh",thisDDU->infpga_code1);
				if((thisDDU->infpga_code1&0x001f)==0x001f)sprintf(buf4," Fiber 13 Unused");
			}
			if(i==419){
				thisDDU->infpga_WrMemActive(INFPGA1,3);
				sprintf(buf,"infpga1 Write Memory Active 14: ");
				sprintf(buf2," Fiber 14 writing to %02Xh",thisDDU->infpga_code1);
				if((thisDDU->infpga_code1&0x001f)==0x001f)sprintf(buf2," Fiber 14 Unused");
			}
			if(i==420){
				if(icrit>0){
					thisDDU->infpga_trap(INFPGA1);
					*out << br() << " <font color=blue> Diagnostic data only valid after Critical Error, traps conditions at that instant</font>" << br() << endl;
					sprintf(buf,"infpga1 diagnostic trap:");
					sprintf(buf2," %08lX %08lX %08lX %08lX %08lX %08lX",thisDDU->fpga_lcode[5],thisDDU->fpga_lcode[4],thisDDU->fpga_lcode[3],thisDDU->fpga_lcode[2],thisDDU->fpga_lcode[1],thisDDU->fpga_lcode[0]);
				}else{
					sprintf(buf," ");
					sprintf(buf2," ");
					thisDDU->infpga_shift0=0xFACE;
				}
			}
			if(i==421){
				sprintf(buf,"infpga1 reset: ");
				sprintf(buf2," EXPERT ONLY! ");
				icond=2;
			}

			if(i==421){
				string ddutextload =
				toolbox::toString("/%s/DDUTextLoad",getApplicationDescriptor()->getURN().c_str());
				*out << cgicc::form().set("method","GET").set("action",ddutextload) << endl;
			}

			*out << cgicc::span().set("style","color:black");
			*out << buf << cgicc::span();
			if(icond==1){
				*out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
			}else if(icond==2){
				*out << cgicc::span().set("style","color:red;background-color:#dddddd;");
			}else{
				*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
			}
			*out << buf2 << cgicc::span();
			*out << cgicc::span().set("style","color:black");
			*out << buf3 << cgicc::span();

			if(icond2==1){
				*out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
			}else if(icond2==2){
				*out << cgicc::span().set("style","color:red;background-color:#dddddd;");
			}else if(icond2==3){
				*out << cgicc::span().set("style","color:blue;background-color:#dddddd;");
			}else{
				*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
			}
			*out << buf4 << cgicc::span();
			if(thisCrate->vmeController()->CAEN_err()!=0){
				*out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
				*out << " **CAEN Error " << cgicc::span();
			}
			if((thisDDU->infpga_shift0!=0xFACE)&&i!=421){
				sprintf(buf," **JTAG Error, Shifted:%04X",thisDDU->infpga_shift0);
				*out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
				*out << buf << cgicc::span();
			}

			if(i==421){
				*out << cgicc::input().set("type","hidden")
				.set("name","textdata")
				.set("size","10")
				.set("ENCTYPE","multipart/form-data")
				.set("value","")
				.set("style","font-size: 13pt; font-family: arial;")<<endl;
				*out << cgicc::input().set("type","submit")
				.set("value","reset");
				sprintf(buf,"%d",ddu);
				*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ddu");
				sprintf(buf,"%d",i);
				*out << cgicc::input().set("type","hidden").set("value",buf).set("name","val");
				*out << cgicc::form() << endl ;
			}else if(i==400&&(stat&0x0000f000)>0){
			//	*out << br() << endl;
				*out << "<blockquote><font size=-1 color=red face=arial>";
				if((stat&0xF0000000)>0){
					if((0x80000000&stat)>0) *out << " DLL-2 Lock Error &nbsp ";
					// if((0x80000000&stat)>0) *out << " DLL-2 Not Locked &nbsp ";
					// if((0x40000000&stat)>0) *out << " DLL-1 Not Locked &nbsp ";
					if((0x40000000&stat)>0) *out << " 64-bit Filler was used &nbsp ";
					if((0x20000000&stat)>0) *out << " RdCtrl-3 Not Ready &nbsp ";
					if((0x10000000&stat)>0) *out << " RdCtrl-2 Not Ready";
					*out << br();
				}
				if((stat&0x0F000000)>0){
					if((0x08000000&stat)>0) *out << " <font color=blue>NoLiveFiber 0 or 1</font> &nbsp ";
					if((0x04000000&stat)>0) *out << " <font color=blue>DLL Error occurred</font> &nbsp ";
					if((0x02000000&stat)>0) *out << " <font color=black>InRD3 DMB Warn</font> &nbsp ";
					if((0x01000000&stat)>0) *out << " <font color=black>InRD2 DMB Warn</font>";
					*out << br();
				}
				if((stat&0x00F00000)>0){
					if((0x00800000&stat)>0) *out << " <font color=black>InRD3 DMB Full</font> &nbsp ";
					if((0x00400000&stat)>0) *out << " Mem/FIFO-InRD3 Error &nbsp ";
					if((0x00200000&stat)>0) *out << " MultL1A Error-InRD3 &nbsp ";
					if((0x00100000&stat)>0) *out << " <font color=black>NoLiveFiber12-14</font>";
					*out << br();
				}
				if((stat&0x000F0000)>0){
					if((0x00080000&stat)>0) *out << " <font color=blue>InRD2 DMB Full</font> &nbsp ";
					if((0x00040000&stat)>0) *out << " Mem/FIFO-InRD2 Error &nbsp ";
					if((0x00020000&stat)>0) *out << " MultL1A Error-InRD2 &nbsp ";
					if((0x00010000&stat)>0) *out << " <font color=black>NoLiveFiber8-11</font>";
					*out << br();
				}
				// JRG, low-order 16-bit status (most serious errors):
				if((stat&0x0000F000)>0){
					if((0x00008000&stat)>0) *out << " <font color=red>Critical Error ** needs reset **</font> &nbsp ";
					if((0x00004000&stat)>0) *out << " <font color=orange>Single Error, bad event</font> &nbsp ";
					if((0x00002000&stat)>0) *out << " <font color=blue>Single warning, possible data problem</font> &nbsp ";
					if((0x00001000&stat)>0) *out << " <font color=blue>Near Full Warning</font>";
					*out << br();
				}
				if((stat&0x00000F00)>0){
					if((0x00000800&stat)>0) *out << " <font color=blue>RX Error occurred</font> &nbsp ";
					if((0x00000400&stat)>0) *out << " <font color=blue>DLL Error (recent)</font> &nbsp ";
					if((0x00000200&stat)>0) *out << " <font color=orange>SCA Full detected</font> &nbsp ";
					if((0x00000100&stat)>0) *out << " <font color=blue>Special Word voted-bit warning</font>";
					*out << br();
				}
				if((stat&0x000000F0)>0){
					if((0x00000080&stat)>0) *out << " Stuck Data occurred &nbsp ";
					if((0x00000040&stat)>0) *out << " Timeout &nbsp ";
					if((0x00000020&stat)>0) *out << " Multiple voted-bit Errors &nbsp ";
					if((0x00000010&stat)>0) *out << " Multiple Transmit Errors";
					*out << br();
				}
				if((stat&0x0000000F)>0){
					if((0x00000008&stat)>0) *out << " Mem/FIFO Full Error &nbsp ";
					if((0x00000004&stat)>0) *out << " Fiber Error &nbsp ";
					if((0x00000002&stat)>0) *out << " <font color=orange>L1A Match Error</font> &nbsp ";
					if((0x00000001&stat)>0) *out << " Not Ready Error";
					*out << br();
				}
				*out << "</font></blockquote>";
			}else if(i==420 && icrit>0){
				//	*out << "<blockquote><font size=-1 color=black face=arial>Trap decode goes here" << "</font></blockquote>";
				*out << "<blockquote><font size=-2 color=black face=arial>";
				DDUinTrapDecode(in, out, thisDDU->fpga_lcode);
				*out << "</font></blockquote>";
			}else if(i==410&&(stat&0xff00)>0){
				*out << endl;
				*out << "<blockquote><font size=-1 color=black face=arial>";
				if((stat&0x00005500)>0){
					*out << " &nbsp InRD2 Status: &nbsp <font color=blue>";
					if((0x00004000&stat)>0) *out << " Ext.FIFO 3/4 Full &nbsp &nbsp ";
					if((0x00001000&stat)>0) *out << " L1A FIFO Almost Full &nbsp &nbsp ";
					if((0x00000400&stat)>0) *out << " MemCtrl Almost Full &nbsp &nbsp ";
					*out << "</font>";
					if((0x00000100&stat)>0) *out << " L1A FIFO Empty";
					if((0x00000100&stat)==0) *out << " L1A FIFO Not Empty";
					*out << br();
				}
				if((stat&0x0000AA00)>0){
					*out << " &nbsp InRD3 Status: &nbsp <font color=blue>";
					if((0x00008000&stat)>0) *out << " Ext.FIFO 3/4 Full &nbsp &nbsp ";
					if((0x00002000&stat)>0) *out << " L1A FIFO Almost Full &nbsp &nbsp ";
					if((0x00000800&stat)>0) *out << " MemCtrl Almost Full &nbsp &nbsp ";
					*out << "</font>";
					if((0x00000200&stat)>0) *out << " L1A FIFO Empty";
					if((0x00000200&stat)==0) *out << " L1A FIFO Not Empty";
					*out << br();
				}
				*out << "</font></blockquote>";
			}else if(i==411&&(stat&0x0f00)>0){
				*out << "<blockquote><font size=-1 color=black face=arial>";
				if((stat&0x00000500)>0){
					*out << " &nbsp InRD2 Status: &nbsp <font color=red>";
					if((0x00000400&stat)>0) *out << " Ext.FIFO Full Occurred &nbsp &nbsp ";
					if((0x00000100&stat)>0) *out << " L1A FIFO Full Occurred";
					*out << "</font>" << br();
				}
				if((stat&0x00000A00)>0){
					*out << " &nbsp InRD3 Status: &nbsp <font color=red>";
					if((0x00000800&stat)>0) *out << " Ext.FIFO Full Occurred &nbsp &nbsp ";
					if((0x00000200&stat)>0) *out << " L1A FIFO Full Occurred";
					*out << "</font>" << br();
				}
				*out << "</font></blockquote>";
			}else if(i==412&&(stat&0xffff)>0){
				*out << "<blockquote><font size=-1 color=black face=arial>";
				if((stat&0x000000ff)>0){
					*out << " &nbsp InRD2: &nbsp <font color=red>";
					if((0x000080&stat)>0) *out << " Critical Error ** needs reset ** &nbsp &nbsp ";
					if((0x000040&stat)>0) *out << " Sync Error ** needs reset ** &nbsp &nbsp ";
					if((0x000020&stat)>0) *out << " <font color=orange>Single Error</font> &nbsp &nbsp ";
					if((0x000010&stat)>0) *out << " FIFO Overflow detected &nbsp &nbsp ";
					if((0x000008&stat)>0) *out << " Fiber Connection Error &nbsp &nbsp ";
					if((0x000004&stat)>0) *out << " Multi-Transmit Error &nbsp &nbsp ";
					if((0x000002&stat)>0) *out << " Stuck Data &nbsp &nbsp ";
					if((0x000001&stat)>0) *out << " Timeout";
					*out  << "</font>" << br();
				}
				if((stat&0x0000ff00)>0){
					*out << " &nbsp InRD3 &nbsp <font color=red>";
					if((0x0008000&stat)>0) *out << " Critical Error ** needs reset ** &nbsp &nbsp ";
					if((0x00004000&stat)>0) *out << " Sync Error ** needs reset ** &nbsp &nbsp ";
					if((0x00002000&stat)>0) *out << " <font color=orange>Single Error</font> &nbsp &nbsp ";
					if((0x00001000&stat)>0) *out << " FIFO Overflow detected &nbsp &nbsp ";
					if((0x00000800&stat)>0) *out << " Fiber Connection Error &nbsp &nbsp ";
					if((0x00000400&stat)>0) *out << " Multi-Transmit Error &nbsp &nbsp ";
					if((0x00000200&stat)>0) *out << " Stuck Data &nbsp &nbsp ";
					if((0x00000100&stat)>0) *out << " Timeout";
					*out  << "</font>" << br();
				}
			*out  << "</font></blockquote>" << br();
			}else{
				*out << br() << endl;
			}
		}
		//    *out << cgicc::fieldset() << endl;
		*out << cgicc::body() << endl;
		*out << cgicc::html() << endl;

	} catch (const exception & e ) {
		//XECPT_RAISE(xgi::exception::Exception, e.what());
	}
}
*/

/*
void EmuFCrateHyperDAQ::DDUinTrapDecode(xgi::Input * in, xgi::Output * out,  unsigned long int lcode[10]) 	throw (xgi::exception::Exception)
{
	printf(" enter DDUinTrapDecode \n");
	int i;
	cgicc::Cgicc cgi(in);
	char buf[100],buf1[100],buf2[100],buf3[100],buf4[100];
	char cbuf1[20],cbuf2[20],cbuf3[20],cbuf4[20];
	char sred[20]="<font color=red>";
	char syel[20]="<font color=orange>";
	char sblu[20]="<font color=blue>";
	char sgrn[20]="<font color=green>";
	char snul[20]="</font>";
	sprintf(buf1," ");
	sprintf(buf2," ");
	sprintf(buf3," ");
	sprintf(buf4," ");
	sprintf(cbuf1," ");
	sprintf(cbuf2," ");
	sprintf(cbuf3," ");
	sprintf(cbuf4," ");

	*out << "<pre>" << endl;
	sprintf(buf,"  192-bit DDU InFPGA Diagnostic Trap (24 bytes) \n");
	*out << buf << endl;

	i=23;
	sprintf(buf,"                        LFfull MemAvail C-code End-TO");
	*out << buf << endl;
	sprintf(buf,"      rcv bytes %2d-%2d:",i,i-7);
	sprintf(cbuf1,"%s",sgrn);
	if(0x000f8000&lcode[5]<3)sprintf(cbuf1,"%s",sblu);
	if(0x000f8000&lcode[5]<2)sprintf(cbuf1,"%s",syel);
	if((0x000f8000&lcode[5]<1)||(0xfff00000&lcode[5]))sprintf(cbuf1,"%s",sred);
	sprintf(buf1,"%s   %04lx%s",cbuf1,(0xffff0000&lcode[5])>>16,snul);
	sprintf(cbuf2,"%s",sgrn);
	if((0x001f&lcode[5]<3)||(0x03e0&lcode[5]<3)||(0x7c00&lcode[5]<3)||(0x000f8000&lcode[5]<3))sprintf(cbuf2,"%s",sblu);
	if((0x001f&lcode[5]<2)||(0x03e0&lcode[5]<2)||(0x7c00&lcode[5]<2)||(0x000f8000&lcode[5]<2))sprintf(cbuf2,"%s",syel);
	if((0x001f&lcode[5]<1)||(0x03e0&lcode[5]<1)||(0x7c00&lcode[5]<1)||(0x000f8000&lcode[5]<1))sprintf(cbuf2,"%s",sred);
	if(0x000f8000&lcode[5]<3){
		sprintf(buf2,"%s    <blink>%04lx</blink>%s",cbuf2,0xffff&lcode[5],snul);
	}else{
		sprintf(buf2,"%s    %04lx%s",cbuf2,0xffff&lcode[5],snul);
	}
	sprintf(cbuf3,"%s",sgrn);
	if(0x20200000&lcode[4])sprintf(cbuf3,"%s",syel);
	if(0xdfdf0000&lcode[4])sprintf(cbuf3,"%s",sred);
	sprintf(buf3,"%s    %04lx%s",cbuf3,(0xffff0000&lcode[4])>>16,snul);
	sprintf(cbuf4,"%s",sgrn);
	if(0xffff&lcode[4])sprintf(cbuf4,"%s",sred);
	sprintf(buf4,"%s   %04lx%s",cbuf4,0xffff&lcode[4],snul);
	*out << buf << buf1 << buf2 << buf3 << buf4 << endl;

	i=15;
	sprintf(buf,"                      Start-TO FAF/Nrdy L1err  DMBwarn");
	*out << buf << endl;
	sprintf(buf,"      rcv bytes %2d-%2d:",i,i-7);
	sprintf(cbuf1,"%s",sgrn);
	if(0xff000000&lcode[3])sprintf(cbuf1,"%s",sblu);
	if(0x00ff0000&lcode[3])sprintf(cbuf1,"%s",sred);
	sprintf(buf1,"%s   %04lx%s",cbuf1,(0xffff0000&lcode[3])>>16,snul);
	sprintf(cbuf2,"%s",sgrn);
	if(0xfc00&lcode[3])sprintf(cbuf2,"%s",sblu);
	sprintf(buf2,"%s    %04lx%s",cbuf2,0xffff&lcode[3],snul);
	sprintf(cbuf3,"%s",sgrn);
	if(0x00ff0000&lcode[2])sprintf(cbuf3,"%s",syel);
	if(0xff000000&lcode[2])sprintf(cbuf3,"%s",sred);
	sprintf(buf3,"%s    %04lx%s",cbuf3,(0xffff0000&lcode[2])>>16,snul);
	sprintf(cbuf4,"%s",sgrn);
	if(0x00ff&lcode[2])sprintf(cbuf4,"%s",sblu);
	if(0xff00&lcode[2])sprintf(cbuf4,"%s",sred);
	sprintf(buf4,"%s   %04lx%s",cbuf4,0xffff&lcode[2],snul);
	*out << buf << buf1 << buf2 << buf3 << buf4 << endl;

	i=7;
	sprintf(buf,"                        32-bit-Empty0M  32-bit-status");
	*out << buf << endl;
	sprintf(buf,"      rcv bytes %2d-%2d:",i,i-7);
	sprintf(cbuf1,"%s",sgrn);
	if(0xffff0000&lcode[1]==0xf8000000)sprintf(cbuf1,"%s",sred);
	sprintf(buf1,"%s   %04lx%s",cbuf1,(0xffff0000&lcode[1])>>16,snul);
	sprintf(cbuf2,"%s",sgrn);
	if(0xffff&lcode[1]==0xf800)sprintf(cbuf2,"%s",sred);
	sprintf(buf2,"%s    %04lx%s",cbuf2,0xffff&lcode[1],snul);
	sprintf(cbuf3,"%s",sgrn);
	if(0x0c080000&lcode[0])sprintf(cbuf3,"%s",sblu);
	if(0xf0660000&lcode[0])sprintf(cbuf3,"%s",sred);
	sprintf(buf3,"%s    %04lx%s",cbuf3,(0xffff0000&lcode[0])>>16,snul);
	sprintf(cbuf4,"%s",sgrn);
	if(0x2d00&lcode[0])sprintf(cbuf4,"%s",sblu);
	if(0x4202&lcode[0])sprintf(cbuf4,"%s",syel);
	if(0x80fd&lcode[0])sprintf(cbuf4,"%s",sred);
	sprintf(buf4,"%s   %04lx%s",cbuf4,0xffff&lcode[0],snul);
	*out << buf << buf1 << buf2 << buf3 << buf4 << endl;
	*out << "</pre>" << br() << endl;
}
*/


void EmuFCrateHyperDAQ::VMEPARA(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{

	cgicc::Cgicc cgi(in);

	cgicc::form_iterator name = cgi.getElement("ddu");
	unsigned int cgiDDU = 0;
	if (name != cgi.getElements().end()) {
		cgiDDU = cgi["ddu"]->getIntegerValue();
		//cout << "DDU inside " << ddu << endl;
	}
	thisDDU = dduVector[cgiDDU];

	ostringstream sTitle;
	sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DDU VME Parallel Controls (RUI #" << thisCrate->getRUI(thisDDU->slot()) << ")";
	*out << Header(sTitle.str(),false);

	// PGK Select-a-DDU
	*out << cgicc::fieldset()
		.set("class","header") << endl;
	*out << cgicc::div("Show this page for a different DDU")
		.set("style","font-weight: bold; font-size: 8pt; width: 100%; text-align: center;") << endl;

	unsigned int iddu = 0;

	*out << cgicc::table()
		.set("style","width: 90%; margin: 2px auto 2px auto; font-size: 8pt; text-align: center;") << endl;
	*out << cgicc::tr() << endl;
	// Loop over all the ddus defined.
	for (iddu = 0; iddu < dduVector.size(); iddu++) {

		// Determine if we are working on a DDU or a DCC by module type
		//thisDDU = dynamic_cast<DDU *>(moduleVector[iModule]);
		// Skip broadcasting
		if (dduVector[iddu]->slot() > 21) continue;

		*out << cgicc::td() << endl;
		if (iddu != cgiDDU) {
			ostringstream location;
			location << "/" + getApplicationDescriptor()->getURN() + "/VMEPARA?ddu=" << iddu;
			*out << cgicc::a()
				.set("href",location.str());
		}
		*out << "Slot " << dduVector[iddu]->slot() << ": RUI #" << thisCrate->getRUI(dduVector[iddu]->slot());
		if (iddu != cgiDDU) {
			*out << cgicc::a();
		}
		*out << cgicc::td() << endl;
	}

	*out << cgicc::tr() << endl;
	*out << cgicc::table() << endl;
	*out << cgicc::fieldset() << endl;

	// Get this DDU back again.
	//thisDDU = dduVector[cgiDDU];

	// PGK Display-a-Crate
	*out << cgicc::fieldset()
		.set("class","fieldset") << endl;
	*out << cgicc::div("Crate Selection")
		.set("class","legend") << endl;

	*out << cgicc::div()
		.set("style","background-color: #FF0; color: #00A; font-size: 16pt; font-weight: bold; width: 50%; text-align: center; padding: 3px; border: 2px solid #00A; margin: 5px auto 5px auto;");
	*out << "Crate " << thisCrate->number() << " Selected";
	*out << cgicc::div() << endl;

	*out << cgicc::span("Configuration located at " + xmlFile_.toString())
		.set("style","color: #A00; font-size: 10pt;") << endl;

	*out << cgicc::fieldset() << endl;
	*out << br() << endl;

	// We will use this object to debug all the DDU's problems.
	DDUDebugger *debugger = new DDUDebugger();

	thisCrate->vmeController()->CAEN_err_reset();

	// Display VME Control status information
	*out << cgicc::fieldset()
		.set("class","normal") << endl;
	*out << cgicc::div()
		.set("class","legend") << endl;
	*out << cgicc::a("&plusmn;")
		.set("class","openclose")
		.set("onClick","javascript:toggle('statusTable')") << endl;
	*out << "VME Control Status" << cgicc::div() << endl;

	// New idea:  make the table first, then display it later.
	DataTable statusTable("statusTable");
	statusTable.addColumn("Register");
	statusTable.addColumn("Value");

	unsigned int parallelStat = thisDDU->readParallelStat();
	*(statusTable(0,0)->value) << "VME status register";
	*(statusTable(0,1)->value) << showbase << hex << parallelStat;
	std::map<string,string> parallelStatComments = debugger->ParallelStat(parallelStat);
	for (std::map<string,string>::iterator iComment = parallelStatComments.begin();
		iComment != parallelStatComments.end();
		iComment++) {
		*(statusTable(0,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}
	statusTable[0]->setClass("ok");
	if ((parallelStat >> 8) & 0xF == 0x4) statusTable[0]->setClass("warning");
	else if ((parallelStat >> 8) & 0xF == 0x1) statusTable[0]->setClass("questionable");
	else if ((parallelStat >> 8) & 0xF != 0x8) statusTable[0]->setClass("bad");

	int dduFMM = (parallelStat >> 8) & 0xf;
	*(statusTable(1,0)->value) << "DDU FMM status";
	*(statusTable(1,1)->value) << showbase << hex << dduFMM;
	std::map<string,string> dduFMMComments = debugger->FMMReg(dduFMM);
	for (std::map<string,string>::iterator iComment = dduFMMComments.begin();
		iComment != dduFMMComments.end();
		iComment++) {
		*(statusTable(1,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}
	statusTable[1]->setClass("ok");
	if (dduFMM & 0xF == 0x4) statusTable[1]->setClass("warning");
	else if (dduFMM & 0xF == 0x1) statusTable[1]->setClass("questionable");
	else if (dduFMM & 0xF != 0x8) statusTable[1]->setClass("bad");

	*out << statusTable.printSummary() << endl;

	// Display only if there are errors.
	if (statusTable.countClass("ok") == statusTable.countRows()) statusTable.setHidden(true);
	*out << statusTable.toHTML() << endl;

	*out << cgicc::fieldset() << endl;

	// Clever trick to help with formating the cut-and-paste.
	*out << cgicc::br()
		.set("style","display: none") << endl;

	thisCrate->vmeController()->CAEN_err_reset();
	// Display individual fiber information
	*out << cgicc::fieldset()
		.set("class","normal") << endl;
	*out << cgicc::div()
		.set("class","legend") << endl;
	*out << cgicc::a("&plusmn;")
		.set("class","openclose")
		.set("onClick","javascript:toggle('fmmTable')") << endl;
	*out << "Individual CSC FMM Reports" << cgicc::div() << endl;

	// New idea:  make the table first, then display it later.
	DataTable fmmTable("fmmTable");
	fmmTable.addColumn("Register");
	fmmTable.addColumn("Value");
	fmmTable.addColumn("Decoded Chambers");

	unsigned int cscStat = thisDDU->readCSCStat();
	*(fmmTable(0,0)->value) << "FMM problem report";
	*(fmmTable(0,1)->value) << showbase << hex << cscStat;
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (cscStat & (1<<iFiber)) {
			*(fmmTable(0,2)->value) << cgicc::div(thisDDU->getChamber(iFiber)->name())
				.set("class","red");
		}
	}
	fmmTable[0]->setClass("ok");
	if (cscStat) fmmTable[0]->setClass("bad");

	unsigned int cscBusy = thisDDU->readFMMBusy();
	*(fmmTable(1,0)->value) << "Busy";
	*(fmmTable(1,1)->value) << showbase << hex << cscBusy;
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (cscBusy & (1<<iFiber)) {
			*(fmmTable(1,2)->value) << cgicc::div(thisDDU->getChamber(iFiber)->name());
		}
	}
	fmmTable[1]->setClass("ok");

	unsigned int cscWarn = thisDDU->readFMMFullWarning();
	*(fmmTable(2,0)->value) << "Warning/near full";
	*(fmmTable(2,1)->value) << showbase << hex << cscWarn;
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (cscWarn & (1<<iFiber)) {
			*(fmmTable(2,2)->value) << cgicc::div(thisDDU->getChamber(iFiber)->name())
				.set("class","orange");
		}
	}
	fmmTable[2]->setClass("ok");
	if (cscWarn) fmmTable[2]->setClass("warning");

	unsigned int cscLS = thisDDU->readFMMLostSync();
	*(fmmTable(3,0)->value) << "Lost sync";
	*(fmmTable(3,1)->value) << showbase << hex << cscLS;
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (cscLS & (1<<iFiber)) {
			*(fmmTable(3,2)->value) << cgicc::div(thisDDU->getChamber(iFiber)->name())
				.set("class","red");
		}
	}
	fmmTable[3]->setClass("ok");
	if (cscLS) fmmTable[3]->setClass("bad");

	unsigned int cscError = thisDDU->readFMMError();
	*(fmmTable(4,0)->value) << "Error";
	*(fmmTable(4,1)->value) << showbase << hex << cscError;
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (cscError & (1<<iFiber)) {
			*(fmmTable(4,2)->value) << cgicc::div(thisDDU->getChamber(iFiber)->name())
				.set("class","red");
		}
	}
	fmmTable[4]->setClass("ok");
	if (cscError) fmmTable[4]->setClass("bad");

	unsigned int cscWH = thisDDU->readWarningHistory();
	*(fmmTable(5,0)->value) << "Warning history";
	*(fmmTable(5,1)->value) << showbase << hex << cscWH;
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (cscWH & (1<<iFiber)) {
			*(fmmTable(5,2)->value) << cgicc::div(thisDDU->getChamber(iFiber)->name())
				.set("class","orange");
		}
	}
	fmmTable[5]->setClass("ok");
	if (cscWH) fmmTable[5]->setClass("warning");

	unsigned int cscBH = thisDDU->readBusyHistory();
	*(fmmTable(6,0)->value) << "Busy history";
	*(fmmTable(6,1)->value) << showbase << hex << cscBH;
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (cscBH & (1<<iFiber)) {
			*(fmmTable(6,2)->value) << cgicc::div(thisDDU->getChamber(iFiber)->name());
		}
	}

	*out << fmmTable.printSummary() << endl;

	// Print table only if there are errors.
	if (fmmTable.countClass("ok") == fmmTable.countRows()) fmmTable.setHidden(true);
	*out << fmmTable.toHTML() << endl;

	*out << cgicc::fieldset() << endl;
	*out << cgicc::hr() << endl;

	// Clever trick to help with formating the cut-and-paste.
	*out << cgicc::br()
		.set("style","display: none") << endl;

	thisCrate->vmeController()->CAEN_err_reset();
	// Inreg for serial writes.
	*out << cgicc::fieldset()
		.set("class","expert") << endl;
	*out << cgicc::div()
		.set("class","legend") << endl;
	*out << cgicc::a("&plusmn;")
		.set("class","openclose")
		.set("onClick","javascript:toggle('inregTableContainer')") << endl;
	*out << "Input Registers for Serial Writes (Expert Only)" << cgicc::div() << endl;

	// New idea:  make the table first, then display it later.
	DataTable inregTable("inregTable");
	inregTable.addColumn("Register");
	inregTable.addColumn("Value");
	inregTable.addColumn("New Value");

	for (unsigned int iReg = 0; iReg < 3; iReg++) {
		unsigned int inreg = thisDDU->readInputReg(iReg);
		*(inregTable(iReg,0)->value) << "InReg" << iReg;
		*(inregTable(iReg,1)->value) << showbase << hex << inreg;
		inregTable[iReg]->setClass("none");
		if (iReg == 0) {
			*(inregTable(iReg,2)->value) << inregTable[iReg]->makeForm("/" + getApplicationDescriptor()->getURN() + "/DDUTextLoad",cgiDDU,508) << endl;
		}
	}

	// Here, I use a container to hide the extra stuff.
	*out << cgicc::span()
		.set("id","inregTableContainer")
		.set("style","display: none;") << endl;
	inregTable.setHidden(false);
	*out << inregTable.toHTML() << endl;

	*out << cgicc::div()
		.set("style","font-size: 8pt;") << endl;
	*out << "Input registers are used for VMS serial writing.  You can load up to 48 bits in these three registers with bit 0 of InReg0 being the LSB." << cgicc::br() << endl;
	*out << "The registers are pipelined, InReg0 --> InReg1 --> InReg2." << endl;
	*out << cgicc::div() << endl;

	*out << cgicc::span() << endl;
	*out << cgicc::fieldset() << endl;

	// Clever trick to help with formating the cut-and-paste.
	*out << cgicc::br()
		.set("style","display: none") << endl;

	thisCrate->vmeController()->CAEN_err_reset();
	*out << cgicc::fieldset()
		.set("class","expert") << endl;
	*out << cgicc::div()
		.set("class","legend") << endl;
	*out << cgicc::a("&plusmn;")
		.set("class","openclose")
		.set("onClick","javascript:toggle('expertTableContainer')") << endl;
	*out << "Miscellaneous Registers (Experts Only)" << cgicc::div() << endl;

	// New idea:  make the table first, then display it later.
	DataTable expertTable("expertTable");
	expertTable.addColumn("Register");
	expertTable.addColumn("Value");
	expertTable.addColumn("Decoded Register Info");
	expertTable.addColumn("New Value");

	unsigned int gbePrescale = thisDDU->readGbEPrescale();
	*(expertTable(0,0)->value) << "GbE prescale";
	*(expertTable(0,1)->value) << showbase << hex << gbePrescale;
	std::map<string,string> gbePrescaleComments = debugger->GbEPrescale(gbePrescale);
	for (std::map<string,string>::iterator iComment = gbePrescaleComments.begin();
		iComment != gbePrescaleComments.end();
		iComment++) {
		*(expertTable(0,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}
	*(expertTable(0,3)->value) << expertTable[0]->makeForm("/" + getApplicationDescriptor()->getURN() + "/DDUTextLoad",cgiDDU,512);
	expertTable[0]->setClass("none");

	unsigned int fakeL1 = thisDDU->readFakeL1Reg();
	*(expertTable(1,0)->value) << "Fake L1A Data Passthrough";
	*(expertTable(1,1)->value) << showbase << hex << fakeL1;
	std::map<string,string> fakeL1Comments = debugger->FakeL1Reg(fakeL1);
	for (std::map<string,string>::iterator iComment = fakeL1Comments.begin();
		iComment != fakeL1Comments.end();
		iComment++) {
		*(expertTable(1,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}
	*(expertTable(1,3)->value) << expertTable[1]->makeForm("/" + getApplicationDescriptor()->getURN() + "/DDUTextLoad",cgiDDU,514);
	expertTable[1]->setClass("none");

	unsigned int foe = thisDDU->readFMMReg();
	*(expertTable(2,0)->value) << "F0E + 4-bit FMM";
	*(expertTable(2,1)->value) << showbase << hex << foe;
	std::map<string,string> foeComments = debugger->F0EReg(foe);
	for (std::map<string,string>::iterator iComment = foeComments.begin();
		iComment != foeComments.end();
		iComment++) {
		*(expertTable(2,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}
	*(expertTable(2,3)->value) << expertTable[2]->makeForm("/" + getApplicationDescriptor()->getURN() + "/DDUTextLoad",cgiDDU,517);
	expertTable[2]->setClass("none");

	*(expertTable(3,0)->value) << "Switches";
	*(expertTable(3,1)->value) << showbase << hex << (thisDDU->readSwitches() & 0xff);
	expertTable[3]->setClass("none");

	for (unsigned int iReg = 0; iReg < 5; iReg++) {
		*(expertTable(4 + iReg,0)->value) << "Test register " << iReg;
		*(expertTable(4 + iReg,1)->value) << showbase << hex << (thisDDU->readTestReg(iReg));
		expertTable[4 + iReg]->setClass("none");
	}

	// Again, I use a container to hide the extra stuff.
	*out << cgicc::span()
		.set("id","expertTableContainer")
		.set("style","display: none;") << endl;
	expertTable.setHidden(false);
	*out << expertTable.toHTML() << endl;

	// You can toggle some more things down here.
	string ddutextload = "/"+getApplicationDescriptor()->getURN()+"/DDUTextLoad";
		ostringstream dduStream;
		dduStream << cgiDDU;
	*out << cgicc::form()
		.set("method","GET")
		.set("action",ddutextload) << endl;
	*out << cgicc::input()
		.set("name","ddu")
		.set("type","hidden")
		.set("value",dduStream.str()) << endl;
	// This is a relic from the old days, and is read by DDUTextLoad.
	*out << cgicc::input()
		.set("name","val")
		.set("type","hidden")
		.set("value","513") << endl;
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","textdata")
		.set("size","10")
		.set("ENCTYPE","multipart/form-data")
		.set("value","") << endl;
	*out << cgicc::input()
		.set("type","submit")
		.set("value","Toggle DAQ backpressure (DCC/S-Link wait)") << endl;
	*out << cgicc::form() << endl;
	*out << cgicc::form()
		.set("method","GET")
		.set("action",ddutextload) << endl;
	*out << cgicc::input()
		.set("name","ddu")
		.set("type","hidden")
		.set("value",dduStream.str()) << endl;
	// This is a relic from the old days, and is read by DDUTextLoad.
	*out << cgicc::input()
		.set("name","val")
		.set("type","hidden")
		.set("value","515") << endl;
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","textdata")
		.set("size","10")
		.set("ENCTYPE","multipart/form-data")
		.set("value","") << endl;
	*out << cgicc::input()
		.set("type","submit")
		.set("value","Toggle all fake L1A (data dump/passthrough)") << endl;
	*out << cgicc::form() << endl;
	*out << cgicc::form()
		.set("method","GET")
		.set("action",ddutextload) << endl;
	*out << cgicc::input()
		.set("name","ddu")
		.set("type","hidden")
		.set("value",dduStream.str()) << endl;
	// This is a relic from the old days, and is read by DDUTextLoad.
	*out << cgicc::input()
		.set("name","val")
		.set("type","hidden")
		.set("value","518") << endl;
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","textdata")
		.set("size","10")
		.set("ENCTYPE","multipart/form-data")
		.set("value","") << endl;
	*out << cgicc::input()
		.set("type","submit")
		.set("value","Activate FMM error-report disable setting") << endl;
	*out << cgicc::form() << endl;

	*out << cgicc::div()
		.set("style","font-size: 8pt;") << endl;
	*out << "GbE Prescale bits 0-2 set rate (0 => 1:1, 1 => 1:2, 2=> 1:4 ... 7 => never)" << cgicc::br() << endl;
	*out << "GbE Prescale bit 3=1 sets DCC/S-Link wait ignore" << cgicc::br() << endl;
	*out << "Fake L1A bit 0=1 sets InFPGA0 passthrough" << cgicc::br() << endl;
	*out << "Fake L1A bit 1=1 sets InFPGA1 passthrough" << cgicc::br() << endl;
	*out << "Fake L1A bit 2=1 sets DDUFPGA passthrough" << cgicc::br() << endl;
	*out << "F0E register bits 4-15=0xFED0 disables DDU FMM error reporting" << endl;
	*out << cgicc::div() << endl;

	*out << cgicc::span() << endl;
/*
	string expertNames[9] = {
		"GbE Prescale",
		"Fake L1A Data Passthrough",
		"F0E + 4-bit FMM",
		"Switches",
		"TestReg 0",
		"TestReg 1",
		"TestReg 2",
		"TestReg 3",
		"TestReg 4"
	};
	long int expertValues[9];
	std::map<string, string> expertComments[9];
	bool expertHex[9] = {
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true
	};

	expertValues[0] = thisDDU->readGbEPrescale();
	expertValues[1] = thisDDU->readFakeL1Reg();
	expertValues[2] = thisDDU->readFMMReg();
	expertValues[3] = thisDDU->readSwitches() & 0xff;
	
	for (int iReg = 0; iReg < 5; iReg++) expertValues[iReg + 4] = thisDDU->readTestReg(iReg);

	expertComments[0] = debugger->GbEPrescale(expertValues[0]);
	expertComments[1] = debugger->FakeL1Reg(expertValues[1]);
	expertComments[2] = debugger->F0EReg(expertValues[2]);

	*out << cgicc::table()
		.set("class","data") << endl;

		// Here are the headers...
	*out << cgicc::tr()
		.set("style","font-weight: bold;") << endl;
	*out << cgicc::td("Register") << endl;
	*out << cgicc::td("Value") << endl;
	*out << cgicc::td("Decoded Register") << endl;
	*out << cgicc::td("New Value") << endl;
	*out << cgicc::tr() << endl;

	// Loop over the values we read and make a row for each.
	for (unsigned int iRow = 0; iRow < 9; iRow++) {

		*out << cgicc::tr() << endl;
		*out << cgicc::td()
			.set("style","border-top: solid 1px #000; width: 25%;") << endl;
		*out << expertNames[iRow];
		*out << cgicc::td() << endl;
		*out << cgicc::td()
			.set("style","border-top: solid 1px #000; width: 15%;") << endl;
		if (expertHex[iRow]) {
			*out << "0x" << hex;
		}
		*out << expertValues[iRow] << dec;
		*out << cgicc::td() << endl;
		*out << cgicc::td()
			.set("style","border-top: solid 1px #000; width: 20%;") << endl;
		std::map<string, string>::iterator iComment;
		for (iComment = expertComments[iRow].begin(); iComment != expertComments[iRow].end(); iComment++) {
			*out << cgicc::div(iComment->first)
				.set("class",iComment->second);
		}
		*out << cgicc::td() << endl;
		*out << cgicc::td()
			.set("style","border-top: solid 1px #000; width: 20%;") << endl;
		// You can set these.
		if (iRow >=0 && iRow < 3) {
			string ddutextload = "/"+getApplicationDescriptor()->getURN()+"/DDUTextLoad";
			ostringstream dduStream;
			dduStream << cgiDDU;
			*out << cgicc::form()
				.set("method","GET")
				.set("action",ddutextload) << endl;
			*out << cgicc::input()
				.set("name","ddu")
				.set("type","hidden")
				.set("value",dduStream.str()) << endl;
			// This is a relic from the old days, and is read by DDUTextLoad.
			string valueNumber = (iRow == 0 ? "512" : (iRow == 1 ? "514" : "517"));
			*out << cgicc::input()
				.set("name","val")
				.set("type","hidden")
				.set("value",valueNumber) << endl;
			ostringstream oldValue;
			if (expertHex[iRow]) oldValue << hex;
			oldValue << expertValues[iRow];
			*out << cgicc::input()
				.set("type","text")
				.set("name","textdata")
				.set("size","10")
				.set("ENCTYPE","multipart/form-data")
				.set("value",oldValue.str()) << endl;
			*out << cgicc::input()
				.set("type","submit")
				.set("value","Load") << endl;
			*out << cgicc::form() << endl;
		}
		*out << cgicc::td() << endl;

		*out << cgicc::tr() << endl;
	}
	*out << cgicc::table() << endl;


	// You can toggle some more things down here.
	string ddutextload = "/"+getApplicationDescriptor()->getURN()+"/DDUTextLoad";
		ostringstream dduStream;
		dduStream << cgiDDU;
	*out << cgicc::form()
		.set("method","GET")
		.set("action",ddutextload) << endl;
	*out << cgicc::input()
		.set("name","ddu")
		.set("type","hidden")
		.set("value",dduStream.str()) << endl;
	// This is a relic from the old days, and is read by DDUTextLoad.
	*out << cgicc::input()
		.set("name","val")
		.set("type","hidden")
		.set("value","513") << endl;
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","textdata")
		.set("size","10")
		.set("ENCTYPE","multipart/form-data")
		.set("value","") << endl;
	*out << cgicc::input()
		.set("type","submit")
		.set("value","Toggle DAQ backpressure (DCC/S-Link wait)") << endl;
	*out << cgicc::form() << endl;
	*out << cgicc::form()
		.set("method","GET")
		.set("action",ddutextload) << endl;
	*out << cgicc::input()
		.set("name","ddu")
		.set("type","hidden")
		.set("value",dduStream.str()) << endl;
	// This is a relic from the old days, and is read by DDUTextLoad.
	*out << cgicc::input()
		.set("name","val")
		.set("type","hidden")
		.set("value","515") << endl;
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","textdata")
		.set("size","10")
		.set("ENCTYPE","multipart/form-data")
		.set("value","") << endl;
	*out << cgicc::input()
		.set("type","submit")
		.set("value","Toggle all fake L1A (data dump/passthrough)") << endl;
	*out << cgicc::form() << endl;
	*out << cgicc::form()
		.set("method","GET")
		.set("action",ddutextload) << endl;
	*out << cgicc::input()
		.set("name","ddu")
		.set("type","hidden")
		.set("value",dduStream.str()) << endl;
	// This is a relic from the old days, and is read by DDUTextLoad.
	*out << cgicc::input()
		.set("name","val")
		.set("type","hidden")
		.set("value","518") << endl;
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","textdata")
		.set("size","10")
		.set("ENCTYPE","multipart/form-data")
		.set("value","") << endl;
	*out << cgicc::input()
		.set("type","submit")
		.set("value","Activate FMM error-report disable setting") << endl;
	*out << cgicc::form() << endl;

	*out << cgicc::div()
		.set("style","font-size: 8pt;") << endl;
	*out << "GbE Prescale bits 0-2 set rate (0 => 1:1, 1 => 1:2, 2=> 1:4 ... 7 => never)" << cgicc::br() << endl;
	*out << "GbE Prescale bit 3=1 sets DCC/S-Link wait ignore" << cgicc::br() << endl;
	*out << "Fake L1A bit 0=1 sets InFPGA0 passthrough" << cgicc::br() << endl;
	*out << "Fake L1A bit 1=1 sets InFPGA1 passthrough" << cgicc::br() << endl;
	*out << "Fake L1A bit 2=1 sets DDUFPGA passthrough" << cgicc::br() << endl;
	*out << "F0E register bits 4-15=0xFED0 disables DDU FMM error reporting" << endl;
	*out << cgicc::div() << endl;
*/
	*out << cgicc::fieldset() << endl;

	*out << Footer() << endl;
	
	/*
	printf(" enter: VMEPARA \n");
	cgicc::Cgicc cgi(in);
	//
	const CgiEnvironment& env = cgi.getEnvironment();
	//
	string crateStr = env.getQueryString() ;
	//
	cout << crateStr << endl ;

	cgicc::form_iterator name = cgi.getElement("ddu");
	//
	if(name != cgi.getElements().end()) {
		ddu = cgi["ddu"]->getIntegerValue();
		cout << "DDU inside " << ddu << endl;
		DDU_ = ddu;
	}else{
		ddu=DDU_;
	}
	printf(" DDU %d \n",ddu);
	thisDDU = dduVector[ddu];
	printf(" set up web page \n");
	//
	*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << endl;
	//
	*out << cgicc::html().set("lang", "en").set("dir","ltr") << endl;
	*out << cgicc::title("VMEPARA Web Form") << endl;
	//
	*out << body().set("background","/tmp/bgndcms.jpg");
	*out << cgicc::div().set("style","font-size: 16pt; font-weight: bold; color: #D00; width: 400px; margin-left: auto; margin-right: auto; text-align: center;") << "Crate " << thisCrate->number() << " Selected" << cgicc::div() << endl;


	int ddu,stat,icond;
	char buf[300],buf2[300] ;
	int iblink=0;
	unsigned short int DDU_FMM=0;
	sprintf(buf,"DDU PARALLEL VME  Slot %d",thisDDU->slot());
	//
	*out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
	*out << endl;
	//
	*out << cgicc::legend(buf).set("style","color:blue")  << endl ;
	//

	int fmmreg=0;
	for(int i=500;i<524;i++){
		// JG, note: 524 is hidden, reserved for expert FMM Wr/Rd loop test.
		printf(" LOOP: %d \n",i);
		thisCrate->vmeController()->CAEN_err_reset();
		icond=0;
		if(i==500){
			sprintf(buf,"VMEctrl Status Register:");
			stat=thisDDU->vmepara_status();
			DDU_FMM=(stat>>8)&0x000F;
			sprintf(buf2," %04X ",stat);
		}
		if(i==501){
			*out << " <font color=blue> Items below have 1+15 bits to report FMM status: 1 flag for DDU plus 15 for its CSC Fibers</font>" << br() << endl;
			sprintf(buf,"CSC FMM Problem Report:");
			sprintf(buf2," %04X ",thisDDU->vmepara_CSCstat());
		}
		if(i==502){
			sprintf(buf,"CSC Busy:");
			stat=thisDDU->vmepara_busy();
			if(0xffff&stat)icond=1;
			sprintf(buf2," %04X ",stat);
		}
		if(i==503){
			sprintf(buf,"CSC Warn/Near Full:");
			stat=thisDDU->vmepara_fullwarn();
			if(0xffff&stat)icond=3;
			sprintf(buf2," %04X ",stat);
		}
		if(i==504){
			sprintf(buf,"CSC LostSync:");
			stat=thisDDU->vmepara_lostsync();
			if(0xffff&stat)icond=2;
			sprintf(buf2," %04X ",stat);
		}
		if(i==505){
			sprintf(buf,"CSC Error:");
			stat=thisDDU->vmepara_error();
			if(0xffff&stat)icond=2;
			sprintf(buf2," %04X ",stat);
		}
		if(i==506){
			sprintf(buf,"CSC Warn History:");
			stat=thisDDU->vmepara_warnhist();
			if(0xffff&stat)icond=3;
			sprintf(buf2," %04X ",stat);
		}
		if(i==507){
			sprintf(buf,"CSC Busy History:");
			stat=thisDDU->vmepara_busyhist();
			if(0xffff&stat)icond=1;
			sprintf(buf2," %04X ",stat);
		}
		if(i==508){
			*out << br() << " <font color=blue> 3 16-bit InRegisters, pipelined 0 ->> 1 ->> 2; use this to pre-load VMEserial writes</font>" << br() << endl;
			sprintf(buf,"Write to InReg0:");
			sprintf(buf2," ");
		}
		if(i==509){
			sprintf(buf,"InReg0:");
			sprintf(buf2," %04X ",thisDDU->vmepara_rd_inreg0());
		}
		if(i==510){
			sprintf(buf,"InReg1:");
			sprintf(buf2," %04X ",thisDDU->vmepara_rd_inreg1());
		}
		if(i==511){
			sprintf(buf,"InReg2:");
			sprintf(buf2," %04X ",thisDDU->vmepara_rd_inreg2());
		}
		if(i==512){
			*out << br() << " <font color=blue> Set SPY Rate (bits 2:0); set Ignore DCC/S-Link Wait (bit 3) <br>  Rate 0-7 will transmit 1 event out of 1,2,4,16,128,1024,32768,never</font>" << br() << endl;
			//	*out << br() << " <font color=blue> Set SPY Rate (bits 2:0); set Ignore DCC/S-Link Wait (bit 3) <br>  Rate 0-7 will transmit 1 event out of 1,8,32,128,1024,8192,32768,never</font>" << br() << endl;
			//	*out << br() << " <font color=blue> Select 0-7 for SPY rate = 1 per 1,8,32,128,1024,8192,32768,never</font>" << br() << endl;
			sprintf(buf,"GbE Prescale*:");
			sprintf(buf2," %04X <font color=red> EXPERT ONLY! </font> ",thisDDU->vmepara_rd_GbEprescale());
		}
		if(i==513){
		sprintf(buf,"Toggle DAQ Backpressure Enable (DCC/S-Link Wait):");
		sprintf(buf2," <font color=red> EXPERT ONLY! </font> ");
		}
		if(i==514){
			*out << " <font color=blue> Enable Fake L1A/Data Passthrough for DDU FPGAs: bit 2=DDUctrl, 1=InFPGA1, 0=InFPGA0</font>" << br() << endl;
			sprintf(buf,"Fake L1A Reg*:");
			sprintf(buf2," %04X <font color=red> EXPERT ONLY! </font> ",thisDDU->vmepara_rd_fakel1reg());
		}
		if(i==515){
			sprintf(buf,"Toggle All FakeL1A (data dump/passthrough mode):");
			sprintf(buf2," <font color=red> EXPERT ONLY! </font> ");
		}
		if(i==516){
			sprintf(buf,"Switches:");
			sprintf(buf2," %02X ",thisDDU->vmepara_switch()&0xff);
		}
		if(i==517){
			*out << hr() << " <font color=blue> Items below are Only for Board Testing, Do Not Use!</font>" << br() << endl;
			//	*out << br() << " <font color=blue> Only for 4-bit FMM function testing, Do Not Use!</font>" << br() << endl;
			sprintf(buf,"'F0E' + 4-bit FMM Reg:");
			fmmreg=thisDDU->vmepara_rd_fmmreg();
			sprintf(buf2," %04X <font color=red> EXPERT ONLY! </font> ",fmmreg);
		}

		if(i==518){
			if ((fmmreg&0xFFF0)==0xFED0){
				*out << " <font color=orange> &nbsp; &nbsp; &nbsp; &nbsp; The FMM Error reporing for this DDU is currently DISABLED! </font>" << br() << endl;
			}
			sprintf(buf,"Activate FMM Error-report Disable setting:");
			sprintf(buf2," <font color=red> EXPERT ONLY! </font> ");
		}

		if(i==524){
			sprintf(buf,"Cycle test for FMM Reg, number of loops:");
			sprintf(buf2," <font color=red> EXPERT ONLY! </font> ");
		}
		if(i==519){
			sprintf(buf,"Test Reg0:");
			sprintf(buf2," %04X ",thisDDU->vmepara_rd_testreg0());
		}
		if(i==520){
			sprintf(buf,"Test Reg1:");
			sprintf(buf2," %04X ",thisDDU->vmepara_rd_testreg1());
		}
		if(i==521){
			sprintf(buf,"Test Reg2:");
			sprintf(buf2," %04X ",thisDDU->vmepara_rd_testreg2());
		}
		if(i==522){
			sprintf(buf,"Test Reg3:");
			sprintf(buf2," %04X ",thisDDU->vmepara_rd_testreg3());
		}
		if(i==523){
			sprintf(buf,"Test Reg4:");
			sprintf(buf2," %04X ",thisDDU->vmepara_rd_testreg4());
		}
		if(i==508||i==512||i==514||i==517||i==515||i==513||i==518||i==524){
			string ddutextload = toolbox::toString("/%s/DDUTextLoad",getApplicationDescriptor()->getURN().c_str());
			*out << cgicc::form().set("method","GET").set("action",ddutextload);
		}
		*out << cgicc::span().set("style","color:black");
		*out << buf << cgicc::span();
		if(icond==1){
			*out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
		}else if(icond==2){
			*out << cgicc::span().set("style","color:red;background-color:#dddddd;");
		}else if(icond==3){
			*out << cgicc::span().set("style","color:blue;background-color:#dddddd;");
		}else{
			*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
		}
		*out << buf2 << cgicc::span();

		if(thisCrate->vmeController()->CAEN_err()!=0){
			*out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
			*out << " **CAEN Error " << cgicc::span();
		}
		if(i==500){
			*out << "<font color=black> &nbsp &nbsp " << "Current DDU FMM Report: </font>";
			sprintf(buf2," READY ");
			if(DDU_FMM==4){   // Busy
				*out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
				sprintf(buf2," BUSY ");
			} else if(DDU_FMM==1){    // Warn, near full: reduce trigger rate
				*out << cgicc::span().set("style","color:blue;background-color:#dddddd;");
				iblink=1;
				sprintf(buf2," WARNING, Near Full ");
			} else if(DDU_FMM==8) *out << cgicc::span().set("style","color:green;background-color:#dddddd;");  // Ready
			else if(DDU_FMM==2){    // Sync Lost
				*out << cgicc::span().set("style","color:red;background-color:#dddddd;");
				sprintf(buf2," Lost Sync ");
			} else if(DDU_FMM==0xC){    // Error
				*out << cgicc::span().set("style","color:red;background-color:#dddddd;");
				sprintf(buf2," ERROR ");
			} else{  // Not Defined
				*out << cgicc::span().set("style","color:red;background-color:#dddddd;");
				sprintf(buf2," UNDEFINED ");
				iblink=1;
			}
			if(iblink==1)sprintf(buf,"%1X,<blink> ",DDU_FMM);
			else sprintf(buf,"%1X, ",((stat>>8)&0x000F));
			*out << buf << buf2 << "</blink>";
			*out << cgicc::span();

			sprintf(buf2,"<blockquote><font size=-1 color=black face=arial> slot=%d &nbsp &nbsp ",((~stat)&0x001F));
			*out << buf2;
			if(stat&0x0080) *out << " VME DLL-2 Not Locked &nbsp ";
			if(stat&0x0040) *out << " VME DLL-1 Not Locked &nbsp ";
			if((stat&0x000000C0)&&(stat&0x0000F000)) *out << br();
			if(stat&0x8000) *out << " VME FPGA detects a problem &nbsp ";
			if(stat&0x4000) *out << " &nbsp VME FPGA has a clock problem &nbsp ";
			if(stat&0x2000) *out << " &nbsp VME FPGA is Not Ready &nbsp ";
			if(stat&0x1000) *out << " &nbsp DDU is Not Ready";
			*out << "</font></blockquote>";
			iblink=0;
		}
		printf(" now boxes \n");
		if(i==508||i==512||i==514||i==517||i==515||i==518||i==513||i==524){
			string xmltext="";
			if(i==508)xmltext="ffff";
			if(i==512)xmltext="f0f0";
			if(i==514)xmltext="f0f0";
			if(i==517)xmltext="0f0e";
			if(i==524)xmltext="1600";
			if(i!=515&&i!=513&&i!=518){
				*out << cgicc::input().set("type","text")
					.set("name","textdata")
					.set("size","10")
					.set("ENCTYPE","multipart/form-data")
					.set("value",xmltext)
					.set("style","font-size: 13pt; font-family: arial;")<<endl;
				*out << cgicc::input().set("type","submit")
					.set("value","set");
			} else {
				*out << cgicc::input().set("type","hidden")
					.set("name","textdata")
					.set("size","10")
					.set("ENCTYPE","multipart/form-data")
					.set("value",xmltext)
					.set("style","font-size: 13pt; font-family: arial;")<<endl;
				*out << cgicc::input().set("type","submit")
					.set("value","toggle");
			}
			//	 *out << cgicc::input().set("type","submit")
			//	   .set("value","set");
			sprintf(buf,"%d",ddu);
			*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ddu");
			sprintf(buf,"%d",i);
			*out << cgicc::input().set("type","hidden").set("value",buf).set("name","val");
			*out << cgicc::form() << endl;
		} else {
		*out << br() << endl;
		}
	}

	*out << cgicc::fieldset() << endl;
	*out << cgicc::body() << endl;
	*out << cgicc::html() << endl;
	*/
}



void EmuFCrateHyperDAQ::VMESERI(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{

	cgicc::Cgicc cgi(in);

	cgicc::form_iterator name = cgi.getElement("ddu");
	unsigned int cgiDDU = 0;
	if (name != cgi.getElements().end()) {
		cgiDDU = cgi["ddu"]->getIntegerValue();
		//cout << "DDU inside " << ddu << endl;
	}
	thisDDU = dduVector[cgiDDU];

	ostringstream sTitle;
	sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DDU VME Serial Controls (RUI #" << thisCrate->getRUI(thisDDU->slot()) << ")";
	*out << Header(sTitle.str(),false);

	// PGK Select-a-DDU
	*out << cgicc::fieldset()
		.set("class","header") << endl;
	*out << cgicc::div("Show this page for a different DDU")
		.set("style","font-weight: bold; font-size: 8pt; width: 100%; text-align: center;") << endl;

	*out << cgicc::table()
		.set("style","width: 90%; margin: 2px auto 2px auto; font-size: 8pt; text-align: center;") << endl;
	*out << cgicc::tr() << endl;
	// Loop over all the ddus defined.
	for (unsigned int iddu = 0; iddu < dduVector.size(); iddu++) {

		// Determine if we are working on a DDU or a DCC by module type
		//thisDDU = dynamic_cast<DDU *>(moduleVector[iModule]);
		if (dduVector[iddu]->slot() > 21) continue;

		*out << cgicc::td() << endl;
		if (iddu != cgiDDU) {
			ostringstream location;
			location << "/" + getApplicationDescriptor()->getURN() + "/VMESERI?ddu=" << iddu;
			*out << cgicc::a()
				.set("href",location.str());
		}
		*out << "Slot " << dduVector[iddu]->slot() << ": RUI #" << thisCrate->getRUI(dduVector[iddu]->slot());
		if (iddu != cgiDDU) {
			*out << cgicc::a();
		}
		*out << cgicc::td() << endl;

	}
	*out << cgicc::tr() << endl;
	*out << cgicc::table() << endl;
	*out << cgicc::fieldset() << endl;

	// Get this DDU back again.
	//thisDDU = dduVector[cgiDDU];

	// PGK Display-a-Crate
	*out << cgicc::fieldset()
		.set("class","fieldset") << endl;
	*out << cgicc::div("Crate Selection")
		.set("class","legend") << endl;

	*out << cgicc::div()
		.set("style","background-color: #FF0; color: #00A; font-size: 16pt; font-weight: bold; width: 50%; text-align: center; padding: 3px; border: 2px solid #00A; margin: 5px auto 5px auto;");
	*out << "Crate " << thisCrate->number() << " Selected";
	*out << cgicc::div() << endl;

	*out << cgicc::span("Configuration located at " + xmlFile_.toString())
		.set("style","color: #A00; font-size: 10pt;") << endl;

	*out << cgicc::fieldset() << endl;
	*out << br() << endl;
	
	thisCrate->vmeController()->CAEN_err_reset();

	// No debugger needed here.

	// Display Voltages
	*out << cgicc::fieldset()
		.set("class","normal") << endl;
	*out << cgicc::div()
		.set("class","legend") << endl;
	*out << cgicc::a("&plusmn;")
		.set("class","openclose")
		.set("onClick","javascript:toggle('voltTable')") << endl;
	*out << "Voltages" << cgicc::div() << endl;

	// New idea:  make the table first, then display it later.
	DataTable voltTable("voltTable");
	voltTable.addColumn("Voltage");
	voltTable.addColumn("Value");

	float V15 = thisDDU->adcplus(1,4);
	*(voltTable(0,0)->value) << "Voltage V15";
	*(voltTable(0,1)->value) << setprecision(4) << V15 << " mV";
	voltTable[0]->setClass("ok");
	if (V15 > 1550 || V15 < 1450) voltTable[0]->setClass("warning");
	if (V15 > 1600 || V15 < 1400) voltTable[0]->setClass("bad");
	if (V15 > 3500 || V15 < 0) voltTable[0]->setClass("questionable");

	float V25 = thisDDU->adcplus(1,5);
	*(voltTable(1,0)->value) << "Voltage V25";
	*(voltTable(1,1)->value) << setprecision(4) << V25 << " mV";
	voltTable[1]->setClass("ok");
	if (V25 > 2550 || V25 < 2450) voltTable[1]->setClass("warning");
	if (V25 > 2600 || V25 < 2400) voltTable[1]->setClass("bad");
	if (V25 > 3500 || V25 < 0) voltTable[1]->setClass("questionable");
	
	float V25a = thisDDU->adcplus(1,6);
	*(voltTable(2,0)->value) << "Voltage V25A";
	*(voltTable(2,1)->value) << setprecision(4) << V25a << " mV";
	voltTable[2]->setClass("ok");
	if (V25a > 2550 || V25a < 2450) voltTable[2]->setClass("warning");
	if (V25a > 2600 || V25a < 2400) voltTable[2]->setClass("bad");
	if (V25a > 3500 || V25a < 0) voltTable[2]->setClass("questionable");
	
	float V33 = thisDDU->adcplus(1,7);
	*(voltTable(3,0)->value) << "Voltage V33";
	*(voltTable(3,1)->value) << setprecision(4) << V33 << " mV";
	voltTable[3]->setClass("ok");
	if (V33 > 3350 || V33 < 3250) voltTable[3]->setClass("warning");
	if (V33 > 3400 || V33 < 3200) voltTable[3]->setClass("bad");
	if (V33 > 3500 || V33 < 0) voltTable[3]->setClass("questionable");

	// Print the table summary
	*out << voltTable.printSummary() << endl;

	// Print actual table.  Maybe.
	if (voltTable.countClass("ok") == voltTable.countRows()) voltTable.setHidden(true);
	*out << voltTable.toHTML() << endl;

	*out << cgicc::fieldset() << endl;

	// Clever trick to help with formating the cut-and-paste.
	*out << cgicc::br()
		.set("style","display: none") << endl;

	// Display Temperatures
	*out << cgicc::fieldset()
		.set("class","normal") << endl;
	*out << cgicc::div()
		.set("class","legend") << endl;
	*out << cgicc::a("&plusmn;")
		.set("class","openclose")
		.set("onClick","javascript:toggle('tempTable')") << endl;
	*out << "Temperatures" << cgicc::div() << endl;

	// New idea:  make the table first, then display it later.
	DataTable tempTable("tempTable");
	tempTable.addColumn("Temperature");
	tempTable.addColumn("Value");

	for (unsigned int iTemp = 0; iTemp < 4; iTemp++) {
		float T0 = thisDDU->readthermx(iTemp);
		*(tempTable(iTemp,0)->value) << "Temperature " << iTemp;
		*(tempTable(iTemp,1)->value) << setprecision(4) << T0 << "&deg;F";
		tempTable[iTemp]->setClass("ok");
		if (T0 > 100 || T0 < 60) tempTable[iTemp]->setClass("warning");
		if (T0 > 130 || T0 < 30) tempTable[iTemp]->setClass("bad");
		if (T0 > 170 || T0 < 0) tempTable[iTemp]->setClass("questionable");
	}

	// Print the table summary
	*out << tempTable.printSummary() << endl;

	// Print actual table.  Maybe.
	if (tempTable.countClass("ok") == tempTable.countRows()) tempTable.setHidden(true);
	*out << tempTable.toHTML() << endl;

	*out << cgicc::fieldset() << endl;

	// Clever trick to help with formating the cut-and-paste.
	*out << cgicc::br()
		.set("style","display: none") << endl;

	// Display Serial Flash RAM Status
	*out << cgicc::fieldset()
		.set("class","normal") << endl;
	*out << cgicc::div()
		.set("class","legend") << endl;
	*out << cgicc::a("&plusmn;")
		.set("class","openclose")
		.set("onClick","javascript:toggle('ramStatusTable')") << endl;
	*out << "Serial Flash RAM Status" << cgicc::div() << endl;

	// New idea:  make the table first, then display it later.
	DataTable ramStatusTable("ramStatusTable");
	ramStatusTable.addColumn("Register");
	ramStatusTable.addColumn("Value");

	int ramStatus = thisDDU->read_status();
	*(ramStatusTable(0,0)->value) << "Serial Flash RAM Status";
	*(ramStatusTable(0,1)->value) << showbase << hex << ramStatus;
	ramStatusTable[0]->setClass("ok");
	if (ramStatus & 0x003c != 0x000c) ramStatusTable[0]->setClass("warning");
	if (ramStatus & 0x0080 != 0x0080) ramStatusTable[0]->setClass("bad");

	// Print the table summary
	*out << ramStatusTable.printSummary() << endl;

	// Print actual table.  Maybe.
	if (ramStatusTable.countClass("ok") == ramStatusTable.countRows()) ramStatusTable.setHidden(true);
	*out << ramStatusTable.toHTML() << endl;

	*out << cgicc::fieldset() << endl;
	*out << cgicc::hr() << endl;


	// Display Expert-only writable registers
	*out << cgicc::fieldset()
		.set("class","expert") << endl;
	*out << cgicc::div()
		.set("class","legend") << endl;
	*out << cgicc::a("&plusmn;")
		.set("class","openclose")
		.set("onClick","javascript:toggle('writableTable')") << endl;
	*out << "Writable Flash Pages (Experts Only)" << cgicc::div() << endl;

	// New idea:  make the table first, then display it later.
	DataTable writableTable("writableTable");
	writableTable.addColumn("Page");
	writableTable.addColumn("Value");
	writableTable.addColumn("New Value");

	*(writableTable(0,0)->value) << "Flash live channels";
	*(writableTable(0,1)->value) << showbase << hex << thisDDU->read_page1();
	writableTable[0]->setClass("none");
	// New Value...
	*(writableTable(0,2)->value) << writableTable[0]->makeForm("/" + getApplicationDescriptor()->getURN() + "/DDUTextLoad",cgiDDU,603) << endl;

	thisDDU->read_page5();
	*(writableTable(1,0)->value) << "Flash GbE FIFO thresholds";
	*(writableTable(1,1)->value) << "0x" << hex << ((thisDDU->rcv_serial[4]&0xC0)>>6) << noshowbase << setw(8) << setfill('0') << hex << (((((thisDDU->rcv_serial[2]&0xC0)>>6)|((thisDDU->rcv_serial[5]&0xFF)<<2)|((thisDDU->rcv_serial[4]&0x3F)<<10)) << 16) | (((thisDDU->rcv_serial[0]&0xC0)>>6)|((thisDDU->rcv_serial[3]&0xFF)<<2)|((thisDDU->rcv_serial[2]&0x3F)<<10)));
	writableTable[1]->setClass("none");
	// New Value...
	*(writableTable(1,2)->value) << writableTable[1]->makeForm("/" + getApplicationDescriptor()->getURN() + "/DDUTextLoad",cgiDDU,604) << endl;

	*(writableTable(2,0)->value) << "Flash RUI Code";
	*(writableTable(2,1)->value) << thisDDU->readFlashSourceID();
	writableTable[2]->setClass("none");
	// New Value...
	*(writableTable(2,2)->value) << writableTable[2]->makeForm("/" + getApplicationDescriptor()->getURN() + "/DDUTextLoad",cgiDDU,605) << endl;

	*(writableTable(3,0)->value) << "Flash DDU Board ID";
	*(writableTable(3,1)->value) << thisDDU->readFlashBoardID();
	writableTable[3]->setClass("none");
	// New Value...
	*(writableTable(3,2)->value) << writableTable[3]->makeForm("/" + getApplicationDescriptor()->getURN() + "/DDUTextLoad",cgiDDU,606) << endl;

	writableTable.setHidden(true);
	*out << writableTable.toHTML() << endl;

	*out << cgicc::fieldset() << endl;	

	*out << Footer() << endl;
/*
	printf(" enter VMESERI \n");
	cgicc::Cgicc cgi(in);
	printf(" initialize env \n");
	const CgiEnvironment& env = cgi.getEnvironment();
	printf(" getQueryString \n");
	string crateStr = env.getQueryString() ;
	cout << crateStr << endl ;
	printf(" VMESERI ifs \n");
	cgicc::form_iterator name = cgi.getElement("ddu");
	int ddu,istat;
	float radc;
	if(name != cgi.getElements().end()) {
		ddu = cgi["ddu"]->getIntegerValue();
		cout << "DDU inside " << ddu << endl;
		DDU_ = ddu;
	}else{
		ddu=DDU_;
	}

	thisDDU = dduVector[ddu];
	printf(" set up web page \n");
	*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << endl;
	*out << cgicc::html().set("lang", "en").set("dir","ltr") << endl;
	*out << cgicc::title("VMESERI Web Form") << endl;
	*out << body().set("background","/tmp/bgndcms.jpg") << endl;
	*out << cgicc::div().set("style","font-size: 16pt; font-weight: bold; color: #D00; width: 400px; margin-left: auto; margin-right: auto; text-align: center;") << "Crate " << thisCrate->number() << " Selected" << cgicc::div() << endl;

	char buf[300],buf2[300] ;
	sprintf(buf,"DDU SERIAL VME  Slot %d",thisDDU->slot());
	*out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
	*out << endl;
	*out << cgicc::legend(buf).set("style","color:blue")  << endl ;

	for(int i=600;i<607;i++){
		printf(" LOOP: %d \n",i);
		thisCrate->vmeController()->CAEN_err_reset();
		if(i==602){
			thisDDU->read_status();
			istat=thisDDU->rcv_serial[1]&0xff;
			sprintf(buf,"%02X</font> &nbsp ",istat);
			sprintf(buf2,"<font color=green>");
			if (istat&0x003c!=0x000c)sprintf(buf2,"<font color=orange>");
			else if (istat&0x0080!=0x0080)sprintf(buf2,"<font color=red>");
			*out << "Serial Flash RAM Status: " << buf2 << buf << br() << endl;

			//           sprintf(buf,"Serial Flash RAM Status:");
			//           sprintf(buf2," %02X ",thisDDU->rcv_serial[1]&0xff);
		}
		if(i==603){  // EXPERT!
			thisDDU->read_page1();
			sprintf(buf,"Flash Live Channel Page (hex):");
			sprintf(buf2," %02X%02X <font color=red> EXPERT ONLY! </font> ",thisDDU->rcv_serial[0]&0xff,thisDDU->rcv_serial[1]&0xff);
		}
		unsigned short int code[3];
		if(i==604){  // EXPERT!
			thisDDU->read_page5();
			sprintf(buf,"Flash GbE FIFO Thresh. Page (hex):");
			code[0]=(((thisDDU->rcv_serial[0]&0xC0)>>6)|((thisDDU->rcv_serial[3]&0xFF)<<2)|((thisDDU->rcv_serial[2]&0x3F)<<10));
			code[1]=(((thisDDU->rcv_serial[2]&0xC0)>>6)|((thisDDU->rcv_serial[5]&0xFF)<<2)|((thisDDU->rcv_serial[4]&0x3F)<<10));
			code[2]=((thisDDU->rcv_serial[4]&0xC0)>>6);
			sprintf(buf2,"%01X/%04X/%04X <font color=red> EXPERT ONLY! </font> ",code[2],code[1],code[0]);
		}
		if(i==605){  // EXPERT!  This WILL go into the DDU Header as SrcID. page 7
			sprintf(buf,"Flash DDU RUI ID Page (dec):");
			sprintf(buf2," %d <font color=red> EXPERT ONLY! </font> ",thisDDU->read_page7()&0x0000ffff);
		}
		if(i==606){  // EXPERT!  This is the REAL Board ID, page 3 for VME access.
			sprintf(buf,"Flash DDU Board ID Page (dec):");
			sprintf(buf2," %d <font color=red> EXPERT ONLY! </font> ",thisDDU->read_page3()&0x0000ffff);
		}
		if(i==600){
			radc=thisDDU->adcplus(1,4);
			sprintf(buf,"%5.fmV</font> &nbsp &nbsp ",radc);
			sprintf(buf2,"<font color=green>");
			if (radc<1450||radc>1550)sprintf(buf2,"<font color=orange>");
			if (radc<1400||radc>1600)sprintf(buf2,"<font color=red>");
			*out << "Voltages: V15=" << buf2 << buf;

			radc=thisDDU->adcplus(1,5);
			sprintf(buf,"%5.fmV</font> &nbsp &nbsp ",radc);
			sprintf(buf2,"<font color=green>");
			if (radc<2450||radc>2550)sprintf(buf2,"<font color=orange>");
			if (radc<2400||radc>2600)sprintf(buf2,"<font color=red>");
			*out << "V25=" << buf2 << buf;

			radc=thisDDU->adcplus(1,6);
			sprintf(buf,"%5.fmV</font> &nbsp &nbsp ",radc);
			sprintf(buf2,"<font color=green>");
			if (radc<2450||radc>2550)sprintf(buf2,"<font color=orange>");
			if (radc<2400||radc>2600)sprintf(buf2,"<font color=red>");
			*out << "V25A=" << buf2 << buf;

			radc=thisDDU->adcplus(1,7);
			sprintf(buf,"%5.fmV</font>",radc);
			sprintf(buf2,"<font color=green>");
			if (radc<3250||radc>3400)sprintf(buf2,"<font color=orange>");
			if (radc<3200||radc>3450)sprintf(buf2,"<font color=red>");
			*out << "V33=" << buf2 << buf << br() << endl;
			//	sprintf(buf2,"<font color=black>V25A=</font>%5.2fmV &nbsp <font color=black>V33=</font>%5.2fmV",thisDDU->adcplus(1,6),thisDDU->adcplus(1,7));
			//	sprintf(buf,"Voltages: V15=<font color=green>%5.2fmV</font> &nbsp V25=<font color=green>%5.2fmV</font> &nbsp ",thisDDU->adcplus(1,4),thisDDU->adcplus(1,5));
			//	sprintf(buf2,"<font color=black>V25A=</font>%5.2fmV &nbsp <font color=black>V33=</font>%5.2fmV",thisDDU->adcplus(1,6),thisDDU->adcplus(1,7));
		}
		if(i==601){
			radc=thisDDU->readthermx(0);
			sprintf(buf,"%5.2fF</font> &nbsp &nbsp ",radc);
			sprintf(buf2,"<font color=green>");
			if (radc<60.0||radc>99.0)sprintf(buf2,"<font color=orange>");
			if (radc<35.0||radc>110.0)sprintf(buf2,"<font color=red>");
			*out << "Temperatures: Sensor0=" << buf2 << buf;

			radc=thisDDU->readthermx(1);
			sprintf(buf,"%5.2fF</font> &nbsp &nbsp ",radc);
			sprintf(buf2,"color=green>");
			if (radc<60.0||radc>99.0)sprintf(buf2,"color=orange>");
			if (radc<35.0||radc>110.0)sprintf(buf2,"color=red>");
			*out << "Sensor1=<font " << buf2 << buf;

			radc=thisDDU->readthermx(2);
			sprintf(buf,"%5.2fF</font> &nbsp &nbsp ",radc);
			sprintf(buf2,"<font color=green>");
			if (radc<60.0||radc>99.0)sprintf(buf2,"<font color=orange>");
			if (radc<35.0||radc>110.0)sprintf(buf2,"<font color=red>");
			*out << "Sensor2=" << buf2 << buf;

			radc=thisDDU->readthermx(3);
			sprintf(buf,"%5.2fF</font>",radc);
			sprintf(buf2,"<font color=green>");
			if (radc<60.0||radc>99.0)sprintf(buf2,"<font color=orange>");
			if (radc<35.0||radc>110.0)sprintf(buf2,"<font color=red>");
			*out << "Sensor3=" << buf2 << buf << br() << endl;
			//         sprintf(buf,"Temperatures:");
			//         sprintf(buf2,"%5.2fF %5.2fF %5.2fF %5.2fF ",thisDDU->readthermx(0),thisDDU->readthermx(1),thisDDU->readthermx(2),thisDDU->readthermx(3));
		}
		if(i==603||i==604||i==605||i==606){
			string ddutextload =
			toolbox::toString("/%s/DDUTextLoad",getApplicationDescriptor()->getURN().c_str());
			*out << cgicc::form().set("method","GET").set("action",ddutextload);
		}

		if(i==603||i==604||i==605||i==606){
			*out << cgicc::span().set("style","color:black");
			*out << buf << cgicc::span();
			if(thisCrate->vmeController()->CAEN_err()!=0){
				*out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
			}else{
				*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
			}
			*out << buf2 << cgicc::span();
			*out << endl;
			printf(" now boxes \n");
			//      if(i==603||i==604||i==605){
			string xmltext="7fff";
			if(i==604)xmltext="000880440";
			if(i==605)xmltext="66";
			if(i==606)xmltext="99";
			*out << cgicc::input().set("type","text")
				.set("name","textdata")
				.set("size","10")
				.set("ENCTYPE","multipart/form-data")
				.set("value",xmltext)
				.set("style","font-size: 13pt; font-family: arial;")<<endl;
			*out << cgicc::input().set("type","submit")
				.set("value","set");
			sprintf(buf,"%d",ddu);
			*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ddu");
			sprintf(buf,"%d",i);
			*out << cgicc::input().set("type","hidden").set("value",buf).set("name","val");
			*out << cgicc::form() << endl ;
		}else{
			*out << br() << endl;
		}
	}
	*out << cgicc::fieldset() << endl;
	*out << cgicc::body() << endl;
	*out << cgicc::html() << endl;
	*/
}



void EmuFCrateHyperDAQ::DDUTextLoad(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	try {
		cgicc::Cgicc cgi(in);
		//
		cgicc::form_iterator name = cgi.getElement("ddu");
		//
		int ddu = 0;
		if(name != cgi.getElements().end()) {
			ddu = cgi["ddu"]->getIntegerValue();
			cout << "DDU " << ddu << endl;
		}
		int val;
		cgicc::form_iterator name2 = cgi.getElement("val");
		//
		if(name2 != cgi.getElements().end()) {
			val = cgi["val"]->getIntegerValue();
			cout << "VAL " << val << endl;
		}
		//
		thisDDU = dduVector[ddu];
		//

		string XMLtext = cgi["textdata"]->getValue();
		// PGK a quick fix...
		if (XMLtext.substr(0,2) == "0x") XMLtext = XMLtext.substr(2);
		//
		cout << XMLtext  << endl ;
		unsigned short int para_val;
		unsigned long int send_val;
//		cout << " talking VME to this shitty DDU slot: " << thisDDU->slot() << endl;
		if(val==227)thisDDU->ddu_l1calonoff();
		if(val==228)thisDDU->ddu_reset();
		if(val==222){
			sscanf(XMLtext.data(),"%01hhx%02hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1],&thisDDU->snd_serial[2]);
			send_val=((thisDDU->snd_serial[0]<<16)&0x000f0000)|((thisDDU->snd_serial[1]<<8)&0x0000ff00)|(thisDDU->snd_serial[2]&0x0000ff);
			printf(" loadkillfiber send %05lx \n",send_val);
			thisDDU->ddu_loadkillfiber(send_val);
		}
		if(val==223){
			sscanf(XMLtext.data(),"%01hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1]);
			para_val=((thisDDU->snd_serial[0]<<8)&0x00000f00)|(thisDDU->snd_serial[1]&0x0000ff);
			printf(" ldbxorbit send %d (%03xh) \n",para_val,para_val);
			thisDDU->ddu_loadbxorbit(para_val);
		}
		if(val==226) {
//		  cout << " sending VME L1A to this shitty DDU slot: " << thisDDU->slot() << endl;
			thisDDU->ddu_vmel1a();
			sleep((unsigned int) 2);
		}
		if(val==321)thisDDU->infpga_reset(INFPGA0);
		if(val==421)thisDDU->infpga_reset(INFPGA1);
		if(val==508){
			sscanf(XMLtext.data(),"%02hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1]);
			para_val=((thisDDU->snd_serial[0]<<8))&0xff00|(thisDDU->snd_serial[1]&0x00ff);
			printf(" para_val %04x \n",para_val);
			thisDDU->vmepara_wr_inreg(para_val);
		}
		if(val==512){
			sscanf(XMLtext.data(),"%02hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1]);
			para_val=((thisDDU->snd_serial[0]<<8))&0xff00|(thisDDU->snd_serial[1]&0x00ff);
			printf(" para_val %04x \n",para_val);
			thisDDU->vmepara_wr_GbEprescale(para_val);
		}
		if(val==513){
			unsigned short int scale= thisDDU->vmepara_rd_GbEprescale();
			if (scale&0x0008) {
				para_val=(scale&0xf7f7)|0x8080;
			} else {
				para_val=(scale|0x0808)&0x7f7f;
			}
			printf("          vmepara_wr_GbEprescale, para_val=%x \n",para_val);
			thisDDU->vmepara_wr_GbEprescale(para_val);
		}
		if(val==514){
			sscanf(XMLtext.data(),"%02hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1]);
			para_val=((thisDDU->snd_serial[0]<<8))&0xff00|(thisDDU->snd_serial[1]&0x00ff);
			printf(" para_val %04x \n",para_val);
			thisDDU->vmepara_wr_fakel1reg(para_val);
		}
		if(val==515){
			unsigned short int scale2=thisDDU-> vmepara_rd_fakel1reg();
			if (scale2&0x0007) {
				para_val=0xF0F0;
			} else {
				para_val=0x8787;
			}
			printf("         vmepara_wr_fakel1reg, para_val=%x \n",para_val);
			thisDDU->vmepara_wr_fakel1reg(para_val);
		}
		if(val==517){
			sscanf(XMLtext.data(),"%02hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1]);
			para_val=((thisDDU->snd_serial[0]<<8))&0xff00|(thisDDU->snd_serial[1]&0x00ff);
			printf(" para_val %04x \n",para_val);
			thisDDU->vmepara_wr_fmmreg(para_val);
		}
		if(val==518){
			unsigned short int scale2=thisDDU-> vmepara_rd_fmmreg();
			if ((scale2&0xFFF0)!=0xFED0) {
				para_val=0xFED0;
			} else {
				para_val=0xFED8;  // JRG, also add a Broadcast for this at Top page.
			}
			printf("         vmepara_wr_fmmreg, para_val=%x \n",para_val);
			thisDDU->vmepara_wr_fmmreg(para_val);
		}
		if(val==524){
			int i,n_loop=0, ierror=0;
			sscanf(XMLtext.data(),"%d",&n_loop);
			printf(" FMM cycle test, n_loop=%d \n",n_loop);
			for(i=0;i<n_loop;i++){
				para_val=(rand());
				thisDDU->vmepara_wr_fmmreg(para_val);
				int rd_in=thisDDU->vmepara_rd_fmmreg();
				if (rd_in != para_val){
					printf(" **** sent %04X, read back %04X **** \n",para_val,rd_in);
					ierror++;
				}
			}
			if (ierror>0){
				printf(" ***** FMM cycle test summary: %d errors out of %d tries \n",ierror,n_loop);
			}else{
				printf(" ** FMM cycle test summary: No errors out of %d tries \n",n_loop);
			}
			thisDDU->vmepara_wr_fmmreg(0xd093);  // Disable Over-ride Mode when done
		}
		if(val==603){
			sscanf(XMLtext.data(),"%02hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1]);
			//  printf(" SSCANF Return: %02x %02x \n",thisDDU->snd_serial[0]&0xff,thisDDU->snd_serial[1]&0xff);
			//      para_val=((thisDDU->snd_serial[0]<<8))&0xff00|(thisDDU->snd_serial[1]&0x00ff);
			para_val=thisDDU->snd_serial[0]&0xff;
			if(thisDDU->snd_serial[1]&0xff)para_val=((thisDDU->snd_serial[0]<<8))&0xff00|(thisDDU->snd_serial[1]&0x00ff);
			printf(" para_val %04x \n",para_val);
			thisDDU->vmepara_wr_inreg(para_val);
			thisDDU->vmepara_rd_inreg0();
			thisDDU->write_page1();
			// sleep(1);
		}
		if(val==604){
			thisDDU->snd_serial[5]=0x00;
			sscanf(XMLtext.data(),"%02hhx%02hhx%02hhx%02hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1],&thisDDU->snd_serial[2],&thisDDU->snd_serial[3],&thisDDU->snd_serial[4]);

			para_val=((thisDDU->snd_serial[4]))&0x000f|((thisDDU->snd_serial[3]<<4)&0x0ff0)|((thisDDU->snd_serial[2]<<12)&0xf000);
			printf(" para_val %04x \n",para_val);
			thisDDU->vmepara_wr_inreg(para_val);

			para_val=((thisDDU->snd_serial[2]>>4))&0x000f|((thisDDU->snd_serial[1]<<4)&0x0ff0)|((thisDDU->snd_serial[0]<<12)&0xf000);
			printf(" para_val %04x \n",para_val);
			thisDDU->vmepara_wr_inreg(para_val);

			para_val=((thisDDU->snd_serial[0]>>4))&0x000f;
			printf(" para_val %04x \n",para_val);
			thisDDU->vmepara_wr_inreg(para_val);

			thisDDU->write_page5();
			// sleep(1);
		}
		if(val==605){
		  int pdec;
			sscanf(XMLtext.data(),"%d",&pdec);
			//&thisDDU->snd_serial[0],&thisDDU->snd_serial[1]);
			//  printf(" SSCANF Return: %02x %02x \n",thisDDU->snd_serial[0]&0xff,thisDDU->snd_serial[1]&0xff);
			//  para_val=thisDDU->snd_serial[0]&0xff;
			para_val=pdec&0x00000fff;
			// if(thisDDU->snd_serial[1]&0xff)para_val=((thisDDU->snd_serial[0]<<8))&0xff00|(thisDDU->snd_serial[1]&0x00ff);
			printf(" para_val 0x%04x \n",para_val);
			thisDDU->vmepara_wr_inreg(para_val);
			thisDDU->vmepara_rd_inreg0();
			thisDDU->write_page7();
			//      sleep(1);
		}
		if(val==606){
		  int pdec;
		  sscanf(XMLtext.data(),"%d",&pdec);
		//		  &thisDDU->snd_serial[0],&thisDDU->snd_serial[1]);
		//  printf(" SSCANF Return: %02x %02x \n",thisDDU->snd_serial[0]&0xff,thisDDU->snd_serial[1]&0xff);
		//			para_val=thisDDU->snd_serial[0]&0xff;
			para_val=pdec&0x00007fff;
			// if(thisDDU->snd_serial[1]&0xff)para_val=((thisDDU->snd_serial[0]<<8))&0xff00|(thisDDU->snd_serial[1]&0x00ff);
			printf(" para_val %5d \n",para_val);
			thisDDU->vmepara_wr_inreg(para_val);
			thisDDU->vmepara_rd_inreg0();
			thisDDU->write_page3();
			//      sleep(1);
		}

		ostringstream dduString;
		dduString << ddu;
		if(val/100==2){
			//DDU_=ddu;
			webRedirect(out,"DDUFpga?ddu="+dduString.str());
			//this->DDUFpga(in,out);
		}
		if(val/100==3){
			//DDU_=ddu;
			webRedirect(out,"InFpga?ddu="+dduString.str());
			//this->InFpga(in,out);
		}
		if(val/100==4){
			//DDU_=ddu;
			webRedirect(out,"InFpga?ddu="+dduString.str());
			//this->InFpga(in,out);
		}
		if(val/100==6){
			//DDU_=ddu;
			webRedirect(out,"VMESERI?ddu="+dduString.str());
			//this->VMESERI(in,out);
		}
		if(val/100==5){
			//DDU_=ddu;
			webRedirect(out,"VMEPARA?ddu="+dduString.str());
			//this->VMEPARA(in,out);
		}

	} catch (const exception & e ) {
		printf(" exception raised in DDUTextLoad \n");
		//XECPT_RAISE(xgi::exception::Exception, e.what());
	}
}


/*
void EmuFCrateHyperDAQ::VMEIntIRQ(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
	//cout << "**Entering VMEIntIRQ" << endl;
	cgicc::Cgicc cgi(in);
	// start will be
	//	1 (yes, start and monitor)
	//	2 (don't start, just monitor)
	//	0 (stop)

	unsigned int FEDVME_CSCstat=0;
	int start = 2;
	if (!(cgi["start"]->isEmpty())) start = cgi["start"]->getIntegerValue();

	bool is_started = thisCrate->vmeController()->thread_started();

	//cout << "Entered VMEIntIRQ with start " << start << " is_started " << is_started << endl;

	if ((!is_started && start!=1)) {
		*out << "ACK!  Threads are a mess!  Try again." << endl;
		return;
	}

	IRQThread *thread;
	if (is_started) thread = thisCrate->vmeController()->thread();

	// Check here for pre-existing conditions and report.
	if(!is_started && start==1) {
		cout << " Controller tells me thread is not started.  Starting..." << endl;
		// if IRQ service is not active, start it...
		thisCrate->vmeController()->start_thread(0); // 0 is the "test" run number.
		thread = thisCrate->vmeController()->thread();
		is_started = thisCrate->vmeController()->thread_started();
		// only get cleared if VMEirq service is started via Button:
		for(int i=0;i<(int)dduVector.size();i++){
			thisDDU=dduVector[i];
			int slot=thisDDU->slot();
			if(slot<=21){
				FEDVME_CSCstat=thisDDU->vmepara_CSCstat();
				if(FEDVME_CSCstat!=0x0000){
					thread->data.previous_problem[slot] = FEDVME_CSCstat;
				}
			}
		}
	}

	*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << endl;

	*out << cgicc::html()
		.set("lang", "en")
		.set("dir","ltr") << endl;
	if (start != 0 && is_started) {
		string vmeintirq = toolbox::toString("/%s/VMEIntIRQ",getApplicationDescriptor()->getURN().c_str());
		*out << meta()
			.set("http-equiv","refresh")
			.set("content","8;" + vmeintirq + "?start=2") << endl;
	}
	*out << cgicc::title("VMEIRQ Web Form") << endl;

	*out << body()
		.set("background","/tmp/bgndcms.jpg") << endl;
	*out << cgicc::div()
		.set("style","font-size: 16pt; font-weight: bold; color: #D00; width: 400px; margin-left: auto; margin-right: auto; text-align: center;") << "Crate " << thisCrate->number() << " Selected" << cgicc::div() << endl;

	char buf[300];

	*out << h3("DDU VME IRQ Monitor")
		.set("style","color: blue;") << endl;

	if (start!=0 && is_started) { // Don't stop...

		// Step 1:  time since thread was started
		*out << span("Time since thread began: ") << endl;
		*out << span()
			.set("style","font-weight: bold; color: white; background-color: black;") << endl;

		time_t now;
		time_t start_time = thread->start_time();
		time(&now);
		long int timediff = (long int) difftime(now,start_time);
		int xsec=timediff%60;
		int xmin = ((timediff - xsec)/60) % 60;
		int xhour = ((timediff - xmin*60 - xsec)/3600) % 24;
		int xday = ((timediff - xhour*3600 - xmin*60 - xsec)/(3600*24));


		sprintf(buf,"%dd %02ld:%02ld:%02ld",xday,xhour,xmin,xsec);
		*out << buf;
		*out << span() << br() << endl;

		// Step 1.5:  previous problems
		bool printed = false;
		for (int i=0; i<21; i++) {
			if (!thread->data.previous_problem[i]) continue;
			if (!printed) {
				printed = true;
				// ALERT ALERT ALERT
				*out << cgicc::div("CAUTION CAUTION CAUTION")
					.set("style","width: 100%; clear:both; font-size: 24pt; text-align: center; font-weight: bold; padding: 10px; background-color: yellow; color: black;")
					.set("id","caution") << endl;
			}
			*out << span() << endl;
			*out << "** Slot " << i << " showed status " << hex << thread->data.previous_problem[i] << dec << "h prior to thread initalization." << endl;
			*out << span() << br() << endl;
		}

		// Step 2:  last interrupt
		if (thread->last_error()) {

			// ALERT ALERT ALERT
			*out << cgicc::div("ALERT ALERT ALERT")
				.set("style","width: 100%; clear:both; font-size: 24pt; text-align: center; font-weight: bold; padding: 10px;")
				.set("id","error") << endl;

			// Javascript error flasher
			*out << "<script type='text/javascript'>" << endl;
			*out << "var red = 1;var DHTML = (document.getElementById || document.all || document.layers);function getObj(name){if (document.getElementById){this.obj = document.getElementById(name);this.style = document.getElementById(name).style;}else if (document.all){this.obj = document.all[name];this.style = document.all[name].style;}else if (document.layers){this.obj = document.layers[name];this.style = document.layers[name];}}" << endl;
			*out << "function setcolor(){if (!DHTML) return; var x = new getObj('error');var c1;var c2; if (red){ red = 0; c1 = 'red'; c2 = 'black'; } else { red = 1; c1 = 'black'; c2 = 'red'; } x.style.backgroundColor = c1; x.style.color = c2; setTimeout('setcolor();',200);}" << endl;
			*out << "setTimeout('setcolor();',200)" << endl;
			*out << "</script>" << endl;

			time_t last_time = thread->last_error_time();
			struct tm* timeinfo = localtime(&last_time);
			sprintf(buf,"%s",asctime(timeinfo));
			*out << span() << endl;
			*out << "Last interrupt occured " << buf << endl;
			*out << span() << br() << endl;

			*out << span() << endl;
			*out << "Last slot to send interrupt: " << thread->last_ddu() << endl;
			*out << span() << br() << endl;

			*out << span() << endl;
			*out << "Last error status: " << hex << thread->last_error() << dec << "h" << endl;
			*out << span() << br() << endl;

			// Status table
			*out << table()
				.set("style","width: 80%; margin-right: auto; margin-left: auto; margin-top: 20px; border-color: #D00; border-width: 1px; border-style: solid; clear: both;") << endl;
			*out << tr() << endl;
			*out << td("Slot")
				.set("style","background-color: red; color: black; font-weight: bold; text-align: center;") << endl;
			*out << td("nIRQ")
				.set("style","background-color: red; color: black; font-weight: bold; text-align: center;") << endl;
			*out << td("Last Time")
				.set("style","background-color: red; color: black; font-weight: bold; text-align: center;") << endl;
			*out << td("Last Error")
				.set("style","background-color: red; color: black; font-weight: bold; text-align: center;") << endl;
			*out << td("Cumulative Error")
				.set("style","background-color: red; color: black; font-weight: bold; text-align: center;") << endl;
			*out << tr() << endl;

			for (int i=0; i<21; i++) {
				if (!thread->ddu_last_error(i)) continue;
				*out << tr() << endl;
				*out << td().set("style","border: 1px solid black;") << i << td() << endl;
				*out << td().set("style","border: 1px solid black;") << thread->ddu_count(i) << td() << endl;
				time_t interrupt_time = thread->ddu_last_error_time(i);
				struct tm* timeinfo = localtime(&interrupt_time);
				*out << td().set("style","border: 1px solid black;") << asctime(timeinfo) << td() << endl;
				*out << td().set("style","border: 1px solid black;") << hex << thread->ddu_last_error(i) << dec << "h" << td() << endl;
				*out << td().set("style","border: 1px solid black;") << hex << thread->acc_error(i) << dec << "h" << td() << endl;
				*out << tr() << endl;
			}

			*out << table() << endl;

		}

		string vmeintirq = toolbox::toString("/%s/VMEIntIRQ",getApplicationDescriptor()->getURN().c_str());
		*out << cgicc::form()
			.set("method","GET")
			.set("action",vmeintirq) << endl;
		*out << cgicc::input().set("type","submit")
			.set("value","Stop VME IRQ Interrupt Thread")
			.set("style","background-color: #FDD; border-color: #D00;") << endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("value","0")
			.set("name","start") << endl;
		*out << cgicc::form() << endl;

	} else if (is_started) { // STOP!
		thisCrate->vmeController()->end_thread();
		*out << endl;
		*out << "**VME IRQ Interrupt has been disabled" << endl;
	} else {
		*out << "**Unexpected condition.  Try again." << endl;
		*out << "Start: " << start << " is_started: " << ((is_started) ? "true" : "false") << endl;
	}

	*out << cgicc::body() << endl;
	*out << cgicc::html() << endl;

	//cout << "I am still alive" << endl;
}
*/


void EmuFCrateHyperDAQ::DCCFirmware(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	unsigned long int idcode,uscode;
	unsigned long int tidcode[2]={0x05035093,0xf5059093};
	unsigned long int tuscode[2]={0xdcc30081,0xdcc310A8};
	//
	cgicc::Cgicc cgi(in);
	//    printf(" initialize env \n");
	//
	const CgiEnvironment& env = cgi.getEnvironment();
	//
	printf(" getQueryString \n");
	string crateStr = env.getQueryString() ;
	//
	cout << crateStr << endl ;

	cgicc::form_iterator name = cgi.getElement("dcc");
	//
	int dcc;
	if(name != cgi.getElements().end()) {
		dcc = cgi["dcc"]->getIntegerValue();
		cout << "DCC inside " << dcc << endl;
		DCC_ = dcc;
	}else{
		dcc=DCC_;
	}
	thisDCC = dccVector[dcc];
	//
	*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << endl;
	//
	*out << cgicc::html().set("lang", "en").set("dir","ltr") << endl;
	*out << cgicc::title("DCC Firmware Form") << endl;
	//
	*out << body().set("background","/tmp/bgndcms.jpg") << endl;
	*out << cgicc::div().set("style","font-size: 16pt; font-weight: bold; color: #D00; width: 400px; margin-left: auto; margin-right: auto; text-align: center;") << "Crate " << thisCrate->number() << " Selected" << cgicc::div() << endl;

	char buf[300] ;
	sprintf(buf,"DCC Firmware Slot %d",thisDCC->slot());
	//
	*out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
	*out << endl;
	//
	*out << cgicc::legend(buf).set("style","color:blue") << endl ;

	//
	for(int i=0;i<2;i++){
		//*out << cgicc::table().set("border","0").set("rules","none").set("frame","void");
		// *out << cgicc::tr();
		printf(" LOOP: %d \n",i);
		*out << cgicc::span().set("style","color:black");
		if(thisDCC->slot()>21){
			if(i==0){sprintf(buf,"inprom  ");}
			if(i==1){sprintf(buf,"mprom  ");}
			printf(" %s ",buf);
			*out<< buf << endl;;
		}
		if(thisDCC->slot()<=21){
			if(i==0){idcode=thisDCC->inprom_chipid(); sprintf(buf,"inprom  ");}
			if(i==1){idcode=thisDCC->mprom_chipid(); sprintf(buf,"mprom  ");}
			printf(" %s idcode %08lx ",buf,idcode);
			*out<<buf;
			sprintf(buf,"  id: ");
			*out << buf;*out << cgicc::span();
			if(idcode!=tidcode[i]){
				*out << cgicc::span().set("style","color:red;background-color:#dddddd;");
			}else{
				*out << cgicc::span().set("style","color:green;background-color:#dddddd");
			}
			sprintf(buf,"%08lX",idcode);
			*out << buf;*out << cgicc::span();
			thisCrate->vmeController()->CAEN_err_reset();
			if(i==0){uscode=thisDCC->inprom_userid();}
			if(i==1){uscode=thisDCC->mprom_userid();}
			*out << cgicc::span().set("style","color:black");
			sprintf(buf," usr: ");
			*out << buf;*out << cgicc::span();
			printf(" uscode %08lx \n",uscode);
			if(thisCrate->vmeController()->CAEN_err()!=0){
				*out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
			//    }else if(uscode!=tuscode[i]){
			}else if(0xffff0000&uscode!=0xffff0000&tuscode[i]){
				*out << cgicc::span().set("style","color:red;background-color:#dddddd;");
			}else{
				*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
			}
			sprintf(buf,"%08lX",uscode);
			*out << buf ;*out << cgicc::span() << endl;
		}
		*out << br() << endl;
		printf(" now boxes \n");
		if(i<2){
			string dccloadfirmware = toolbox::toString("/%s/DCCLoadFirmware",getApplicationDescriptor()->getURN().c_str());
			//
			*out << cgicc::form().set("method","POST")
				.set("enctype","multipart/form-data")
				.set("action",dccloadfirmware) << endl;;
			//	.set("enctype","multipart/form-data")
			//
			*out << cgicc::input().set("type","file")
				.set("name","DCCLoadSVF")
				.set("size","50") << endl;
			//
			*out << cgicc::input().set("type","submit").set("value","LoadSVF") << endl;
			sprintf(buf,"%d",dcc);
			*out << cgicc::input().set("type","hidden").set("value",buf).set("name","dcc") << endl;
			sprintf(buf,"%d",i);
			*out << cgicc::input().set("type","hidden").set("value",buf).set("name","prom") << endl;
			*out << cgicc::form() << endl ;
			string dccfirmwarereset = toolbox::toString("/%s/DCCFirmwareReset",getApplicationDescriptor()->getURN().c_str());
			*out << cgicc::form().set("method","GET").set("action",dccfirmwarereset) << endl;
			if(i==0){
				*out << cgicc::input().set("type","submit").set("value","Reset InFPGA")<<endl;
			}else{
				*out << cgicc::input().set("type","submit").set("value","Reset MFPGA")<<endl;
			}
			sprintf(buf,"%d",dcc);
			*out << cgicc::input().set("type","hidden").set("value",buf).set("name","dcc") << endl;
			sprintf(buf,"%d",i);
			*out << cgicc::input().set("type","hidden").set("value",buf).set("name","val") << endl;
			*out << cgicc::form() << endl;
		}else{
			*out << endl;
		}
		// *out << cgicc::tr() << endl;
		// *out << cgicc::table() << endl;

		*out << br() << endl;
	}
	*out << cgicc::legend() << endl;
	*out << cgicc::fieldset()<< endl;
	*out << cgicc::body() << endl;
	*out << cgicc::html() << endl;
}



void EmuFCrateHyperDAQ::DCCLoadFirmware(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	try {
		//
		cgicc::Cgicc cgi(in);
		//
		cgicc::form_iterator name = cgi.getElement("dcc");
		//
		int dcc;
		if(name != cgi.getElements().end()) {
			dcc = cgi["dcc"]->getIntegerValue();
			cout << "DCC " << dcc << endl;
			DCC_ = dcc;
		}
		int prom;
		cgicc::form_iterator name2 = cgi.getElement("prom");
		//
		if(name2 != cgi.getElements().end()) {
			prom = cgi["prom"]->getIntegerValue();
			cout << "PROM " << prom << endl;
		}
		//
		printf(" Entered DCCLoadFirmware prom %d \n",prom);
		thisDCC = dccVector[dcc];
		//
		//
		cout << "UploadConfFileUpload" << endl ;
		//


		const_file_iterator file;
		file = cgi.getFile("DCCLoadSVF");

		//
		cout << "GetFiles" << endl ;
		//

		if(file != cgi.getFiles().end()) {
			string XMLname = (*file).getFilename();
			cout <<"SVF FILE: " << XMLname  << endl ;

			ofstream TextFile ;
			TextFile.open("MySVFFile.svf");
			(*file).writeToStream(TextFile);
			TextFile.close();
		}
		/*  char buf[400];
		FILE *dwnfp;
		dwnfp    = fopen("MySVFFile.svf","r");
		while (fgets(buf,256,dwnfp) != NULL)printf("%s",buf);
		fclose(dwnfp); */
		char *cbrdnum;
		printf(" DCC epromload %d \n",prom);
		cbrdnum=(char*)malloc(5);
		cbrdnum[0]=0x00;cbrdnum[1]=0x00;cbrdnum[2]=0x00;cbrdnum[3]=0x00;
		if(prom==1)thisDCC->epromload("MPROM",RESET,"MySVFFile.svf",1,cbrdnum);
		if(prom==0)thisDCC->epromload("INPROM",INPROM,"MySVFFile.svf",1,cbrdnum);
		in=NULL;

		ostringstream dccString;
		dccString << dcc;
		webRedirect(out,"DCCFirmware?dcc="+dccString.str());
		//this->DCCFirmware(in,out);
	} catch (const exception & e ) {
		printf(" exception raised in DCCLoadFirmware \n");
		//XECPT_RAISE(xgi::exception::Exception, e.what());
	}
}



void EmuFCrateHyperDAQ::LoadXMLconf(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	printf(" enter: LoadXMLconf \n");
	cgicc::Cgicc cgi(in);
	//    const CgiEnvironment& env = cgi.getEnvironment();
	//    string URLname = cgi["DefURL"]->getValue() ;
	//    cout << "  should go to this URL: " << URLname  << endl ;
	// Here's how you clear them:
	crateVector.clear();
	dduVector.clear();
	dccVector.clear();
	// It doesn't seem to work, though (PGK)
	printf(" dduVector.size() %d \n",dduVector.size());
	printf(" dccVector.size() %d \n",dccVector.size());
	//    dduVector.size()=0;  // How to set these to zero?!?
	//    dccVector.size()=0;  // Then go to   this->Default(in.out);
	reload=1;

	string loadxmlconf = toolbox::toString("/%s/",getApplicationDescriptor()->getURN().c_str());
	//    cout<<endl<<"  JRGdebug: LoadXMLconf string="<<loadxmlconf<<endl;
	webRedirect(out,"mainPage");
	//mainPage(in,out);
}



void EmuFCrateHyperDAQ::DCCCommands(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	printf(" enter: DCC Commands \n");
	cgicc::Cgicc cgi(in);
	const CgiEnvironment& env = cgi.getEnvironment();
	string crateStr = env.getQueryString();
	cout << crateStr << endl;
	cgicc::form_iterator name = cgi.getElement("dcc");
	int dcc;
	if(name != cgi.getElements().end()) {
		dcc = cgi["dcc"]->getIntegerValue();
		cout << "DCC inside " << dcc << endl;
		DCC_ = dcc;
	}else{
		dcc=DCC_;
	}
	thisDCC = dccVector[dcc];
	printf(" DCC %d \n",dcc);
	printf(" set up web page \n");
	*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << endl;
	*out << cgicc::html().set("lang", "en").set("dir","ltr") << endl;
	*out << cgicc::title("DCC Comands Web Form") << endl;
	*out << body().set("background","/tmp/bgndcms.jpg");
	*out << cgicc::div().set("style","font-size: 16pt; font-weight: bold; color: #D00; width: 400px; margin-left: auto; margin-right: auto; text-align: center;") << "Crate " << thisCrate->number() << " Selected" << cgicc::div() << endl;

	char buf[300],buf2[300],buf3[300];
	sprintf(buf,"DCC Commands VME  Slot %d",thisDCC->slot());
	*out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
	*out << endl;
	*out << cgicc::legend(buf).set("style","color:blue")  << endl;
	int igu;
	for(int i=100;i<111;i++){
		thisCrate->vmeController()->CAEN_err_reset();
		sprintf(buf3," ");
		if(i==100){
			unsigned short int statush=thisDCC->mctrl_stath();
			unsigned short int statusl=thisDCC->mctrl_statl();
			sprintf(buf,"Status:");
			sprintf(buf2," H: %04X L: %04X ",statush,statusl);

		}
		if(i==101){
			sprintf(buf,"BX Reset:");
			sprintf(buf2," ");
		}
		if(i==102){
			sprintf(buf,"EVN Reset:");
			sprintf(buf2," ");
		}
		if(i==103){
			unsigned short int fifouse=thisDCC->mctrl_rd_fifoinuse();
			sprintf(buf,"Set FIFOs Used:");
			sprintf(buf2," %04X ",(fifouse&0x7ff));
		//           sprintf(buf2," ");
			sprintf(buf3,"<font size=-1> (selects which DDUs must be processed by DCC)</font>");
		}
		if(i==104){
			unsigned short int ttccmd=thisDCC->mctrl_rd_ttccmd();
			sprintf(buf,"TTC Command:");
			sprintf(buf2," %04X ",(ttccmd>>2)&0x3f);
		//           sprintf(buf2," ");
			sprintf(buf3,"<font size=-1> (only works when TTC fiber input to DCC is disabled)</font>");
		}
		if(i==105){
			unsigned short int fifouse=thisDCC->mctrl_rd_fifoinuse();
			unsigned short int ttccmd=thisDCC->mctrl_rd_ttccmd();
			sprintf(buf,"Load L1A:");
			sprintf(buf2," %02x , %02x",((ttccmd>>9)&0x60)+((fifouse>>11)&0x1f),(ttccmd>>7)&0x7e );
		}
		if(i==106){
			sprintf(buf," Load L1A(no prompt):");
			sprintf(buf2," ");
		}

		if (i==107) {
			sprintf(buf,"Date Rate Slink0:");
			unsigned short int status[6];
			int dr[6];
			for (igu=0;igu<6;igu++) {
				status[igu]=thisDCC->mctrl_ratemon(igu);
				dr[igu]=((status[igu]&0x3fff)<<(((status[igu]>>14)&0x3)*4));
			}
			sprintf(buf2," %d  ddu3: %d  ddu13: %d  ddu4: %d  ddu12 %d  ddu5: %d",dr[0],dr[1],dr[2],dr[3],dr[4],dr[5]);
		}
		if (i==108) {
			sprintf(buf,"Date Rate Slink1:");
			unsigned short int status[6];
			int dr[6];
			for (igu=6;igu<12;igu++) {
				status[igu-6]=thisDCC->mctrl_ratemon(igu);
				dr[igu-6]=((status[igu-6]&0x3fff)<<(((status[igu-6]>>14)&0x3)*4));
			}
			sprintf(buf2," %d  ddu11: %d  ddu6: %d  ddu10: %d  ddu7: %d  ddu9: %d",dr[0],dr[1],dr[2],dr[3],dr[4],dr[5]);
		}
		if(i==109){
			unsigned short int swset=thisDCC->mctrl_swrd();
			sprintf(buf,"Set switch register:");
			sprintf(buf2," %04X ",(swset&0xffff));
		//           sprintf(buf2," ");
			sprintf(buf3,"<font size=-1> (set the software switch etc)</font>");
		}
		if(i==110){
			unsigned short int fmmset=thisDCC->mctrl_fmmrd();
			sprintf(buf,"Set FMM register:");
			sprintf(buf2," %04X ",(fmmset&0xffff));
		//           sprintf(buf2," ");
			sprintf(buf3,"<font size=-1> (set the FMM status)</font>");
		}


		if((i>100 && i<107)||i==109||i==110) {
			string dcctextload =
			toolbox::toString("/%s/DCCTextLoad",getApplicationDescriptor()->getURN().c_str());
			*out << cgicc::form().set("method","GET").set("action",dcctextload) << endl;
		//	   .set("style","margin-bottom: 0")
		}
		*out << cgicc::span().set("style","color:black");
		*out << buf << cgicc::span();
		if(thisCrate->vmeController()->CAEN_err()!=0){
			*out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
		}else{
			*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
		}
		*out << buf2;
		*out << cgicc::span();
		if((i>100&&i<107)||i==109||i==110) {
			string xmltext="";
			if(i==103) {
				//int readback=thisDCC->mctrl_rd_fifoinuse();
				xmltext="ffff";
				//       xmltext=(readback);
			}
			if(i==104) {
				// int readback2= thisDCC->mctrl_rd_tcccmd();
				xmltext="ffff";
				//  xmltext=(readback2);
			}
			if (i==109) {
				xmltext="0000";
			}
			if (i==110) {
				xmltext="0000";
			}
			if(i==105)xmltext="2,5";
			if(i==103|i==104|i==105|i==109|i==110){
				*out << cgicc::input().set("type","text")
					.set("name","textdata")
					.set("size","10")
					.set("ENCTYPE","multipart/form-data")
					.set("value",xmltext)
					.set("style","font-size: 13pt; font-family: arial;")<<endl;
			}
			*out << cgicc::input().set("type","submit")
				.set("value","set");
			sprintf(buf,"%d",dcc);
			*out << cgicc::input().set("type","hidden").set("value",buf).set("name","dcc");
			sprintf(buf,"%d",i);
			*out << cgicc::input().set("type","hidden").set("value",buf).set("name","val");
			*out << buf3 << cgicc::form() << endl;
		}else{
			*out << br() << endl;
		}
		if(i==103){
			*out << "<blockquote><font size=-1 face=arial>";
			*out << "DCC-FIFO bits map to DDU-Slots" << br();
			*out << " &nbsp &nbsp &nbsp &nbsp b0=Slot3, &nbsp b1=Slot13, &nbsp b2=Slot4, &nbsp b3=Slot12, &nbsp b4=Slot5 &nbsp ---->> Top S-Link" << br();
			*out << " &nbsp &nbsp &nbsp &nbsp b5=Slot11, &nbsp b6=Slot6, &nbsp b7=Slot10, &nbsp b8=Slot7, &nbsp b9=Slot9 &nbsp ---->> Bottom S-Link";
			*out << "</font></blockquote>" << endl;
		}
		if(i==104){
			*out << "<blockquote><font size=-1 face=arial>";
			*out << "Command Code examples (hex):" << br();
			*out << " &nbsp &nbsp &nbsp &nbsp 3=SyncRst, &nbsp 4=PChardRst, &nbsp 1C=SoftRst, &nbsp 34=DDUhardRst";
			*out << "</font></blockquote>" << endl;
		}
		if(i==109){
			*out << "<blockquote><font size=-1 face=arial>";
			*out << "bit4 = sw4; bit5 = sw5" << br();
			*out << " &nbsp bit0=0&bit9=1: Enable software switch"<<br();
			*out << " &nbsp bitC=1&bitF=0: Set TTCrx NOT ready"<<br();
			*out << " &nbsp bitD=1&bitE=0: Ignore SLINK full, &nbsp bitD=0&bitE=1: Ignore SLINK full and Slink_down";
			*out << "</font></blockquote>" << endl;
		}
		if(i==110) {
			*out << "<blockquote><font size=-1 face=arial>";
			*out << " XOR(bit4,bit5): Enable FMM overwrite";
			*out << " &nbsp FMM[3:0]=bit[3:0]";
			*out << "</font></blockquote>" << endl;
		}

	}

	*out << cgicc::fieldset() << endl;
	*out << cgicc::body() << endl;
	*out << cgicc::html() << endl;
}



void EmuFCrateHyperDAQ::DCCTextLoad(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
  try {
		printf(" inside DCCTextLoad \n");
		cgicc::Cgicc cgi(in);
		//
		cgicc::form_iterator name = cgi.getElement("dcc");
		//
		int dcc;
		if(name != cgi.getElements().end()) {
			dcc = cgi["dcc"]->getIntegerValue();
			cout << "DCC " << dcc << endl;
			DCC_ = dcc;
		}
		int val;
		cgicc::form_iterator name2 = cgi.getElement("val");
		//
		if(name2 != cgi.getElements().end()) {
			val = cgi["val"]->getIntegerValue();
			cout << "VAL _new" << val << endl;
		}
		//
		thisDCC = dccVector[dcc];
		//

		string XMLtext;
		if (val==103 || val==104 || val==105 || val==109 ||val==110) {
			XMLtext = cgi["textdata"]->getValue();
			//
			cout << XMLtext  << endl ;
		}
		unsigned int para_val;
		char snd_serial[2];
		static int rate,num;
		char arate;
		char anum;
		if(val==101)thisDCC->mctrl_bxr();
		if(val==102)thisDCC->mctrl_evnr();
		snd_serial[0]=0;
		snd_serial[1]=0;
		if(val==103){
			//      sscanf(XMLtext.data(),"%04x",&para_val);
			istringstream test(XMLtext);
			test >> hex >> para_val;
			printf(" mctrl_fifouse send %04x \n",para_val);
			thisDCC->mctrl_fifoinuse(para_val);
		}
		if(val==104){
			//      istringstream test(XMLtext);
			//	test >> hex >> para_val;
			sscanf(XMLtext.data(),"%x",&para_val);
			printf(" ttc command send %04x \n",para_val);
			thisDCC->mctrl_ttccmd(para_val);
		}
		if(val==105){
			sscanf(XMLtext.data(),"%d,%d",&rate,&num);
			if(rate>255)rate=255;
			if(num>255)num=255;
			arate=rate;anum=num;
			printf(" load L1A send rate:%d num:%d \n",arate,anum);
			thisDCC->mctrl_fakeL1A(arate,anum);
		}
		if(val==106){
			arate=rate;anum=num;
			printf(" load (no prompt) L1A send rate:%d num:%d \n",arate,anum);
			thisDCC->mctrl_fakeL1A(arate,anum);
		}
		if(val==109){
			//      sscanf(XMLtext.data(),"%04x",&para_val);
			istringstream test(XMLtext);
			test >> hex >> para_val;
			printf(" Set switch register %04x \n",para_val);
			thisDCC->mctrl_swset(para_val);
		}
		if(val==110){
			//      sscanf(XMLtext.data(),"%04x",&para_val);
			istringstream test(XMLtext);
			test >> hex >> para_val;
			printf(" Set FMM register %04x \n",para_val);
			thisDCC->mctrl_fmmset(para_val);
		}

		DCC_=dcc;
		ostringstream dccString;
		dccString << dcc;
		webRedirect(out,"DCCCommands?dcc="+dccString.str());
		//this->DCCCommands(in,out);


	} catch (const exception & e ) {
		printf(" exception raised in DCCLoadFirmware \n");
		//XECPT_RAISE(xgi::exception::Exception, e.what());
	}
}


/** @note Depricated? **/
void EmuFCrateHyperDAQ::IRQTester(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	try {    //
		cgicc::Cgicc cgi(in);
		printf(" start sleep \n");
		unsigned int tim=10;
		sleep(tim);
		printf(" end sleep \n");
		//this->VMEIntIRQ(in,out);
	} catch (const exception & e ) {
		printf(" exception raised in DCCLoadFirmware \n");
		//XECPT_RAISE(xgi::exception::Exception, e.what());
	}
}



void EmuFCrateHyperDAQ::DCCFirmwareReset(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
  try {
		//
		cgicc::Cgicc cgi(in);
		//
		cgicc::form_iterator name = cgi.getElement("dcc");
		//
		int dcc;
		if(name != cgi.getElements().end()) {
			dcc = cgi["dcc"]->getIntegerValue();
			cout << "DCC " << dcc << endl;
			DCC_ = dcc;
		}
		int i;
		cgicc::form_iterator name2 = cgi.getElement("val");
		//
		if(name2 != cgi.getElements().end()) {
			i = cgi["val"]->getIntegerValue();
			cout << "val" << i << endl;
		}
		//
		thisDCC = dccVector[dcc];
		if(i==0)thisDCC->hdrst_in();
		if(i==1)thisDCC->hdrst_main();
		unsigned int tim=1;
		sleep(tim);
		in=NULL;
		DCC_=dcc;

		ostringstream dccString;
		dccString << dcc;
		webRedirect(out,"DCCFirmware?dcc="+dccString.str());
		//this->DCCFirmware(in,out);
	} catch (const exception & e ) {
		printf(" exception raised in DCCLoadFirmware \n");
		//XECPT_RAISE(xgi::exception::Exception, e.what());
	}
}



void EmuFCrateHyperDAQ::DDUVoltMon(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	float v_val[4] = {1500,2500,2500,3300};
	float v_delt = 100;
	float t_val[4] = {80,80,80,80};
	float t_delt = 25;

	cgicc::Cgicc cgi(in);

	ostringstream sTitle;
	sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DDU Voltage/Temperature Monitoring";
	*out << Header(sTitle.str());

	// PGK Display-a-Crate
	*out << cgicc::fieldset()
		.set("class","fieldset") << endl;
	*out << cgicc::div("Crate Selection")
		.set("class","legend") << endl;

	*out << cgicc::div()
		.set("style","background-color: #FF0; color: #00A; font-size: 16pt; font-weight: bold; width: 50%; text-align: center; padding: 3px; border: 2px solid #00A; margin: 5px auto 5px auto;");
	*out << "Crate " << thisCrate->number() << " Selected";
	*out << cgicc::div() << endl;

	*out << cgicc::span("Configuration located at " + xmlFile_.toString())
		.set("style","color: #A00; font-size: 10pt;") << endl;

	*out << cgicc::fieldset() << endl;
	*out << br() << endl;

	// PGK Voltages/Temperatures
	*out << cgicc::fieldset()
		.set("class","fieldset") << endl;
	*out << cgicc::div("Voltages and Temperatures")
		.set("class","legend") << endl;

	// Loop through the Modules and pick out the DDUs
	for (unsigned int iModule = 0; iModule < moduleVector.size(); iModule++) {

		// Determine if we are working on a DDU or a DCC by module type
		thisDDU = dynamic_cast<DDU *>(moduleVector[iModule]);
		if (thisDDU != 0) {
			// I am a DDU!
			// Skip broadcasting
			if (thisDDU->slot() > 21) continue;

			// Check Voltages
			thisCrate->vmeController()->CAEN_err_reset();
			string dduClass = "ok";
			string totalVoltClass = "ok";
			// The actual voltage measurements
			double voltage[4] = {0,0,0,0};
			// Each voltage read gets two trials before failing.
			int voltTrial[4] = {0,0,0,0};
			string voltageClass[4] = {"ok","ok","ok","ok"};
			for (unsigned int iVolt = 0; iVolt < 8; iVolt++) {
				// Automatically do each voltage twice:
				unsigned int jVolt = iVolt/2;
				// Voltages are 4-7
				voltage[jVolt] = thisDDU->adcplus(1,jVolt+4);
				if( thisCrate->vmeController()->CAEN_err() != 0) {
					dduClass = "caution";
				}
				if ((voltage[jVolt] > v_val[jVolt] - v_delt) && (voltage[jVolt] < v_val[jVolt] + v_delt)) {
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
			thisCrate->vmeController()->CAEN_err_reset();
			string totalTempClass = "ok";
			// The actual temperatures
			double temp[4] = {0,0,0,0};
			// Each temperature read gets two trials before failing.
			int tempTrial[4] = {0,0,0,0};
			string tempClass[4] = {"ok","ok","ok","ok"};
			for (unsigned int iTemp = 0; iTemp < 8; iTemp++) {
				// Automatically read each temperature twice
				unsigned int jTemp = iTemp/2;
				temp[jTemp] = thisDDU->readthermx(jTemp);
				if( thisCrate->vmeController()->CAEN_err() != 0) {
					dduClass = "caution";
				}
				if ((temp[jTemp] > t_val[jTemp] - t_delt) && (temp[jTemp] < t_val[jTemp] + t_delt)) {
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
				.set("style","width: 90%; border: 2px solid #000; margin: 5px auto 5px auto; text-align: center; border-collapse: collapse;") << endl;

			// First row:  Voltages
			*out << cgicc::tr() << endl;
			// The first element is special:  board type/slot
			*out << cgicc::td()
				.set("class",dduClass)
				.set("style","border-right: 1px solid #000; font-weight: bold; width: 10%;");
			*out << "DDU Slot " << thisDDU->slot();
			*out << cgicc::td() << endl;

			// The second element is special:  label
			*out << cgicc::td()
				.set("class",totalVoltClass)
				.set("style","border: 1px solid #000; font-weight: bold; width: 10%;");
			*out << "Voltages";
			*out << cgicc::td() << endl;

			// Loop over voltages
			for (unsigned int iVolt = 0; iVolt < 4; iVolt++) {
				*out << cgicc::td()
					.set("class",voltageClass[iVolt])
					.set("style","border-right: 1px solid #000; border-bottom: 1px solid #000; width: 20%;");
				*out << "V" << iVolt << ": " << setprecision(4) << voltage[iVolt] << "mV";
				*out << cgicc::td() << endl;
			}
			*out << cgicc::tr() << endl;

			// Second row:  Temperatures
			*out << cgicc::tr() << endl;
			// The first element is special:  rui
			*out << cgicc::td()
				.set("class",dduClass)
				.set("style","border-right: 1px solid #000; font-weight: bold; width: 10%;");
			*out << "RUI #" << thisCrate->getRUI(thisDDU->slot());
			*out << cgicc::td() << endl;

			// The second element is special:  label
			*out << cgicc::td()
				.set("class",totalTempClass)
				.set("style","border-right: 1px solid #000; font-weight: bold; width: 10%;");
			*out << "Temperatures";
			*out << cgicc::td() << endl;

			// Loop over temperatures
			for (unsigned int iTemp = 0; iTemp < 4; iTemp++) {
				*out << cgicc::td()
					.set("class",tempClass[iTemp])
					.set("style","border-right: 1px solid #000; border-bottom: 1px solid #000; width: 20%;");
				*out << "T" << iTemp << ": " << setprecision(4) << temp[iTemp] << "&deg;F";
				*out << cgicc::td() << endl;
			}
			*out << cgicc::tr() << endl;
			*out << cgicc::table() << endl;

		}
	}

	*out << cgicc::fieldset() << endl;

/*
	char buf[300];
	sprintf(buf,"DDU Volt/Temp Monitor");
	//
	*out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
	*out << endl;
	for (int i=0; i<(int)dduVector.size(); i++) {
		*out << cgicc::table().set("border","0").set("rules","none").set("frame","void");
		*out << cgicc::tr();
		thisDDU=dduVector[i];
		int slot = thisDDU->slot();
		if(slot<=21){
			char buf[20];
			sprintf(buf,"DDU Slot: %d ",slot);
			printf(" %s \n",buf);
			*out << buf;
			sprintf(buf,"Voltages:");
			*out << buf;
			for(int j=4;j<8;j++){
				thisCrate->vmeController()->CAEN_err_reset();
				float adc=thisDDU->adcplus(1,j);
			//  printf(" caen_err %d \n",thisDDU->CAEN_err());
				if(thisCrate->vmeController()->CAEN_err()!=0){
					*out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
				} else if((adc>v_val[j-4]-v_delt)&&(adc<v_val[j-4]+v_delt)){
					*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
				}else{
					*out << cgicc::span().set("style","color:red;background-color:#dddddd;");
				}
				char buf2[20];
				sprintf(buf2," %5.0fmV",adc);
				*out << buf2;
				*out << cgicc::span() << endl;
			}
			sprintf(buf,"Temperatures:");
			*out << buf;
			for(int j=0;j<4;j++){
				thisCrate->vmeController()->CAEN_err_reset();
				float adc=thisDDU->readthermx(j);
				if(thisCrate->vmeController()->CAEN_err()!=0){
					*out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
				} else if((adc>t_val[j]-t_delt)&&(adc<t_val[j]+t_delt)){
					*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
				}else{
					*out << cgicc::span().set("style","color:red;background-color:#dddddd;");
				}
				char buf2[20];
				sprintf(buf2," %5.2fF",adc);
				*out << buf2;
				*out << cgicc::span() << endl;
			}
		}
		*out << cgicc::tr() << endl;
		*out << cgicc::table() << endl;
	}
	*out << cgicc::fieldset()<< endl;
*/
	*out << Footer();
}



void EmuFCrateHyperDAQ::webRedirect(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception)
{
	string url = in->getenv("PATH_TRANSLATED");

	HTTPResponseHeader &header = out->getHTTPResponseHeader();

	header.getStatusCode(303);
	header.getReasonPhrase("See Other");
	header.addHeader("Location",
	url.substr(0, url.find("/" + in->getenv("PATH_INFO"))));
}



void EmuFCrateHyperDAQ::DCCRateMon(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	cgicc::CgiEnvironment cgiEnvi(in);
	string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
	//
	*out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"30; URL=/" << getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<endl;


	//
	*out << "<HTML>" <<endl;
	*out << "<BODY bgcolor=\"#FFFFFF\">" <<endl;

	thisDCC=dccVector[0];
	unsigned short int status[12];

	for(int igu=0;igu<12;igu++) {
		status[igu]=thisDCC->mctrl_ratemon(igu);
		DCC_ratemon[DCC_ratemon_cnt][igu]=((status[igu]&0x3fff)<<(((status[igu]>>14)&0x3)*4));
	}
	DCC_ratemon_cnt=DCC_ratemon_cnt+1;
	if(DCC_ratemon_cnt>49)DCC_ratemon_cnt=0;
	for(int igu=0;igu<12;igu++) {
		DCC_ratemon[DCC_ratemon_cnt%50][igu]=0;
		DCC_ratemon[(DCC_ratemon_cnt+1)%50][igu]=0;
	}

	*out << "<OBJECT classid=\"clsid:D27CDB6E-AE6D-11cf-96B8-444553540000\" codebase=\"http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,0,0\" WIDTH=\"565\" HEIGHT=\"420\" id=\"FC_2_3_MSLine\">" <<endl;
	*out << "<PARAM NAME=movie VALUE=\"/daq/extern/FusionCharts/Charts/FC_2_3_MSLine.swf\">" <<endl;
	ostringstream output;
	output << "<PARAM NAME=\"FlashVars\" VALUE=\"&dataURL=getDataDCCRate0" << "&chartWidth=565&chartHeight=420"<<"\">"<<endl;
	*out << output.str() << endl ;
	*out << "<PARAM NAME=quality VALUE=high>" << endl ;
	*out << "<PARAM NAME=bgcolor VALUE=#FFFFFF>" << endl ;
	//
	ostringstream output2;
	output2 << "<EMBED src=\"/daq/extern/FusionCharts/Charts/FC_2_3_MSLine.swf\" FlashVars=\"&dataURL=getData"<< "DCCRate0"<<"\" quality=high bgcolor=#FFFFFF WIDTH=\"565\" HEIGHT=\"420\" NAME=\"FC_2_3_MSLine\" TYPE=\"application/x-shockwave-flash\" PLUGINSPAGE=\"http://www.macromedia.com/go/getflashplayer\"></EMBED>" << endl;
	//
	*out << output2.str() << endl;
	*out << "</OBJECT>" << endl;

	*out << "<OBJECT classid=\"clsid:D27CDB6E-AE6D-11cf-96B8-444553540000\" codebase=\"http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,0,0\" WIDTH=\"565\" HEIGHT=\"420\" id=\"FC_2_3_MSLine\">" <<endl;
	*out << "<PARAM NAME=movie VALUE=\"/daq/extern/FusionCharts/Charts/FC_2_3_MSLine.swf\">" <<endl;
	ostringstream output3;
	output3 << "<PARAM NAME=\"FlashVars\" VALUE=\"&dataURL=getDataDCCRate1" << "&chartWidth=565&chartHeight=420"<<"\">"<<endl;
	*out << output3.str() << endl ;
	*out << "<PARAM NAME=quality VALUE=high>" << endl ;
	*out << "<PARAM NAME=bgcolor VALUE=#FFFFFF>" << endl ;
	//
	ostringstream output4;
	output4 << "<EMBED src=\"/daq/extern/FusionCharts/Charts/FC_2_3_MSLine.swf\" FlashVars=\"&dataURL=getData"<< "DCCRate1"<<"\" quality=high bgcolor=#FFFFFF WIDTH=\"565\" HEIGHT=\"420\" NAME=\"FC_2_3_MSLine\" TYPE=\"application/x-shockwave-flash\" PLUGINSPAGE=\"http://www.macromedia.com/go/getflashplayer\"></EMBED>" << endl;
	//
	*out << output4.str() << endl;
	*out << "</OBJECT>" << endl;




	*out << "</BODY>" << endl;
	*out << "</HTML>" << endl;

}



void EmuFCrateHyperDAQ::getDataDCCRate0(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	//
	*out << "<graph caption='DCC S-Link 0' subcaption='' xAxisName='time' yAxisName='Rate' numberPrefix='' showNames='1' animation='0'>" << endl;
	//      *out << "<graph caption='DCC FPGA 0' subcaption='' xAxisName='time' yAxisName='Rate' numberPrefix='' showNames='1' animation='0'>" << endl;
	//cout << "<graph caption='DCC FPGA 0' subcaption='' xAxisName='time' yAxisName='Rate' numberPrefix='' showNames='1' animation='0'>" << endl;
	*out << "<categories>" << endl;
	//cout << "<categories>" << endl;
	for(int i=0;i<50;i++){
		*out << "<category name='"<<i<<"' hoverText='' />" << endl;
		//cout << "<category name='"<<i<<"' hoverText='' />" << endl;
	}
	*out << "</categories>" <<endl;
	//cout << "</categories>" <<endl;
	for(int ch=0;ch<6;ch++){
		if(ch==0)*out << "<dataset seriesName='S-Link 0 ' color='0080C0' showValues='0'>" <<endl;
		if(ch==1)*out << "<dataset seriesName='Slot 3 DDU ' color='008040' showValues='0'>" <<endl;
		if(ch==2)*out << "<dataset seriesName='Slot 13 DDU ' color='800080' showValues='0'>" <<endl;
		if(ch==3)*out << "<dataset seriesName='Slot 4 DDU ' color='FF8040' showValues='0'>" <<endl;
		if(ch==4)*out << "<dataset seriesName='Slot 12 DDU ' color='FFF000' showValues='0'>" <<endl;
		if(ch==5)*out << "<dataset seriesName='Slot 5 DDU ' color='FF0080' showValues='0'>" <<endl;
		for(unsigned int i=0;i<50;i++) {
			ostringstream output;
			// output << "<set name='" << ch <<"'"<< " value='" << DCC_ratemon[i][ch] << "'" << " />" << endl;
			output << "<set value='" << DCC_ratemon[i][ch] << "'" << " />" << endl;
			*out << output.str() << endl ;
		//cout << output.str() << endl ;
		}
		*out << "</dataset>"<<endl;
		//cout << "</dataset>"<<endl;
	}
	*out << "</graph>" << endl;
	//cout << "</graph>" << endl;
}


/** @note Merge with getDataDCCRate0. **/
void EmuFCrateHyperDAQ::getDataDCCRate1(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	//
	*out << "<graph caption='DCC S-Link 1' subcaption='' xAxisName='time' yAxisName='Rate' numberPrefix='' showNames='1' animation='0'>" << endl;
	//      *out << "<graph caption='DCC FPGA 1' subcaption='' xAxisName='time' yAxisName='Rate' numberPrefix='' showNames='1' animation='0'>" << endl;
	//cout << "<graph caption='DCC FPGA 0' subcaption='' xAxisName='time' yAxisName='Rate' numberPrefix='' showNames='1' animation='0'>" << endl;
	*out << "<categories>" << endl;
	//cout << "<categories>" << endl;
	for(int i=0;i<50;i++){
		*out << "<category name='"<<i<<"' hoverText='' />" << endl;
		//cout << "<category name='"<<i<<"' hoverText='' />" << endl;
	}
	*out << "</categories>" <<endl;
	//cout << "</categories>" <<endl;
	for(int ch=6;ch<12;ch++){
		if(ch==6)*out << "<dataset seriesName='S-Link 1 ' color='0080C0' showValues='0'>" <<endl;
		if(ch==7)*out << "<dataset seriesName='Slot 11 DDU ' color='008040' showValues='0'>" <<endl;
		if(ch==8)*out << "<dataset seriesName='Slot 6 DDU ' color='800080' showValues='0'>" <<endl;
		if(ch==9)*out << "<dataset seriesName='Slot 10 DDU ' color='FF8040' showValues='0'>" <<endl;
		if(ch==10)*out << "<dataset seriesName=' Slot 7 DDU ' color='FFF000' showValues='0'>" <<endl;
		if(ch==11)*out << "<dataset seriesName='DDU Slot 9 DDU ' color='FF0080' showValues='0'>" <<endl;
		for(unsigned int i=0;i<50;i++) {
			ostringstream output;
			// output << "<set name='" << ch <<"'"<< " value='" << DCC_ratemon[i][ch] << "'" << " />" << endl;
			output << "<set value='" << DCC_ratemon[i][ch] << "'" << " />" << endl;
			*out << output.str() << endl ;
			//cout << output.str() << endl ;
		}
		*out << "</dataset>"<<endl;
		//cout << "</dataset>"<<endl;
	}
	*out << "</graph>" << endl;
	//cout << "</graph>" << endl;
}



void EmuFCrateHyperDAQ::queryFCrate()
	throw (xoap::exception::Exception, xoap::exception::Exception) {

	xoap::MessageReference reply = getParameters("EmuFCrate",getApplicationDescriptor()->getInstance());
	xdata::String state = readParameter<xdata::String>(reply, "State");
	fcState_ = state.toString();

}


void EmuFCrateHyperDAQ::webRedirect(xgi::Output *out ,string location)
{
	string url = "/" + getApplicationDescriptor()->getURN() + "/" + location;

	HTTPResponseHeader &header = out->getHTTPResponseHeader();

	header.getStatusCode(303);
	header.getReasonPhrase("See Other");
	header.addHeader("Location",url);
}

