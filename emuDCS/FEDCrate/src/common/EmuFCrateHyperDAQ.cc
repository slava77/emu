/*****************************************************************************\
* $Id: EmuFCrateHyperDAQ.cc,v 3.48 2008/09/19 16:53:52 paste Exp $
*
* $Log: EmuFCrateHyperDAQ.cc,v $
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
#include "EmuFCrateHyperDAQ.h"

#include <string>
#include <vector>
//#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <time.h>
//#include <stdio.h>
#include <map>
#include <bitset>
#include <fstream>
#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/configurator.h>

#include "xdaq/NamespaceURI.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
//#include "xdata/UnsignedLong.h"

//#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
//#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
//#include "cgicc/FormFile.h"

#include "DDUDebugger.h"
#include "DCCDebugger.h"
#include "DataTable.h"
#include "FEDCrateParser.h"
#include "FEDCrate.h"
#include "DDU.h"
#include "Chamber.h"
#include "DCC.h"
#include "VMEController.h"
#include "JTAG_constants.h"

#include "CAENVMElib.h"

XDAQ_INSTANTIATOR_IMPL(EmuFCrateHyperDAQ)


EmuFCrateHyperDAQ::EmuFCrateHyperDAQ(xdaq::ApplicationStub * s):
	EmuFEDApplication(s),
	xmlFile_("/home/cscdev/TriDAS/emu/emuDCS/FEDCrate/xml/config.xml"),
	//Operator_("Name..."),
	//DCC_ratemon_cnt(0),
	fcState_(STATE_UNKNOWN)
{
	xgi::bind(this,&EmuFCrateHyperDAQ::Default, "Default");
	xgi::bind(this,&EmuFCrateHyperDAQ::mainPage, "mainPage");
	xgi::bind(this,&EmuFCrateHyperDAQ::configurePage, "configurePage");
	xgi::bind(this,&EmuFCrateHyperDAQ::setConfFile, "setConfFile");
	xgi::bind(this,&EmuFCrateHyperDAQ::setRawConfFile, "setRawConfFile");
	xgi::bind(this,&EmuFCrateHyperDAQ::UploadConfFile, "UploadConfFile");
	
	//xgi::bind(this,&EmuFCrateHyperDAQ::DDUFirmware, "DDUFirmware");
	xgi::bind(this,&EmuFCrateHyperDAQ::DDUBroadcast, "DDUBroadcast");
	xgi::bind(this,&EmuFCrateHyperDAQ::DDULoadBroadcast, "DDULoadBroadcast");
	xgi::bind(this,&EmuFCrateHyperDAQ::DDUSendBroadcast, "DDUSendBroadcast");
	xgi::bind(this,&EmuFCrateHyperDAQ::DDUReset, "DDUReset");
	//xgi::bind(this,&EmuFCrateHyperDAQ::DDUBrcstFED, "DDUBrcstFED");

	xgi::bind(this,&EmuFCrateHyperDAQ::DDUDebug, "DDUDebug");
	xgi::bind(this,&EmuFCrateHyperDAQ::DDUExpert, "DDUExpert");
	xgi::bind(this,&EmuFCrateHyperDAQ::InFpga, "InFpga");
	xgi::bind(this,&EmuFCrateHyperDAQ::VMEPARA, "VMEPARA");
	xgi::bind(this,&EmuFCrateHyperDAQ::VMESERI, "VMESERI");
	xgi::bind(this,&EmuFCrateHyperDAQ::DDUTextLoad, "DDUTextLoad");
	//xgi::bind(this,&EmuFCrateHyperDAQ::DDULoadFirmware,"DDULoadFirmware");
	//xgi::bind(this,&EmuFCrateHyperDAQ::DDUtrapDecode,"DDUtrapDecode");
	xgi::bind(this,&EmuFCrateHyperDAQ::LoadXMLconf,"LoadXMLconf");
	
	//xgi::bind(this,&EmuFCrateHyperDAQ::DCCFirmware,"DCCFirmware");
	//xgi::bind(this,&EmuFCrateHyperDAQ::DCCLoadFirmware,"DCCLoadFirmware");
	//xgi::bind(this,&EmuFCrateHyperDAQ::DCCFirmwareReset,"DCCFirmwareReset");
	xgi::bind(this,&EmuFCrateHyperDAQ::DCCBroadcast, "DCCBroadcast");
	xgi::bind(this,&EmuFCrateHyperDAQ::DCCLoadBroadcast, "DCCLoadBroadcast");
	xgi::bind(this,&EmuFCrateHyperDAQ::DCCSendBroadcast, "DCCSendBroadcast");
	
	xgi::bind(this,&EmuFCrateHyperDAQ::DCCDebug,"DCCDebug");
	xgi::bind(this,&EmuFCrateHyperDAQ::DCCExpert,"DCCExpert");
	xgi::bind(this,&EmuFCrateHyperDAQ::DCCTextLoad, "DCCTextLoad");
	
	xgi::bind(this,&EmuFCrateHyperDAQ::DDUVoltMon,"DDUVoltMon");
	//xgi::bind(this,&EmuFCrateHyperDAQ::DCCRateMon,"DCCRateMon");
	//xgi::bind(this,&EmuFCrateHyperDAQ::getDataDCCRate0,"getDataDCCRate0");
	//xgi::bind(this,&EmuFCrateHyperDAQ::getDataDCCRate1,"getDataDCCRate1");
	
	//xgi::bind(this,&EmuFCrateHyperDAQ::setCrate,"setCrate");
	//myParameter_ =  0;

	// for (int i=0; i<9; i++) { DDUBoardID_[i] = "-1" ; DCCBoardID_[i] = "-1" ; }
	getApplicationInfoSpace()->fireItemAvailable("xmlFileName",&xmlFile_);
	// for(int i=0;i<12;i++){
	// for(int j=0;j<50;j++){
	// DCC_ratemon[j][i]=i+1;
	// }
	// }

	char datebuf[55];
	char filebuf[255];
	std::time_t theTime = time(NULL);

	std::strftime(datebuf, sizeof(datebuf), "%Y-%m-%d-%H:%M:%S", localtime(&theTime));
	std::sprintf(filebuf,"EmuFCrateHyperDAQ-%s.log",datebuf);

	log4cplus::SharedAppenderPtr myAppend = new log4cplus::FileAppender(filebuf);
	myAppend->setName("EmuFCrateHyperDAQAppender");

	//Appender Layout
	std::auto_ptr<Layout> myLayout = std::auto_ptr<Layout>(new log4cplus::PatternLayout("%D{%m/%d/%Y %j-%H:%M:%S.%q} %-5p %c, %m%n"));
	// for date code, use the Year %Y, DayOfYear %j and Hour:Min:Sec.mSec
	// only need error data from Log lines with "ErrorData" tag
	myAppend->setLayout( myLayout );

	getApplicationLogger().addAppender(myAppend);

	// TEMP
	getApplicationLogger().setLogLevel(DEBUG_LOG_LEVEL);
}




void EmuFCrateHyperDAQ::Default(xgi::Input *in, xgi::Output *out )
	throw (xgi::exception::Exception)
{

	//std::cout << "--Entering Default" << std::endl;
/* JRG, Logger stuff to add:
    // if (getApplicationLogger().exists(getApplicationLogger().getName())) {
*/
	//LOG4CPLUS_INFO(getApplicationLogger(), " EmuFEDVME: server startup" << std::endl);
	//std::string LoggerName = getApplicationLogger().getName();
	//std::cout << "Name of Logger is " <<  LoggerName <<std::endl;

	// PGK Should the ddu and dcc std::vectors not be filled, this means we have
	//  not been properly configured.  We should inform the user about this.
	if (crateVector.size()==0) {
		LOG4CPLUS_INFO(getApplicationLogger(), "This instance of the HyperDAQ has not yet read in its XML file.  Initializing.");
		Configuring(); // Defaults should be sufficient.
		// Get the state of the EmuFCrate that I am associated with.
		xoap::MessageReference reply = getParameters("EmuFCrate",getApplicationDescriptor()->getInstance());
		xdata::String state = readParameter<xdata::String>(reply, "State");
		fcState_ = state.toString();
		LOG4CPLUS_DEBUG(getApplicationLogger(), "EmuFCrate(" << getApplicationDescriptor()->getInstance() << ") returned state \"" << fcState_ << "\"");
		//if (fcState_ == "Halted" || fcState_ == "Failed" || fcState_ == STATE_UNKNOWN) {
			//std::cout << "FCrate reports crates are not configured.  Configuring." << std::endl;
			//crateVector[0]->configure(0);
		//} else {
			//std::cout << "FCrate reports crates are configured.  Skipping configuring." << std::endl;
		//}
	}

	webRedirect(out,"mainPage");
	//mainPage(in,out);

}



void EmuFCrateHyperDAQ::mainPage(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception)
{
	
	//std::cout << "--Entering mainPage" << std::endl;
	// PGK Patented check-for-initialization
	if (crateVector.size()==0) {
		LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
		return Default(in,out);
	}

	cgicc::Cgicc cgi(in);

	// First, I need a crate.
	cgicc::form_iterator name = cgi.getElement("crate");
	unsigned int cgiCrate = 0;
	if(name != cgi.getElements().end()) {
		cgiCrate = cgi["crate"]->getIntegerValue();
	}
	emu::fed::FEDCrate *myCrate = crateVector[cgiCrate];
	
	std::stringstream sTitle;
	sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ")";
	*out << Header(sTitle.str(),false);

	// Test new DCC communication here.
	/* These work
	std::clog << " @@@@@ Testing readTTCCommand @@@@@ " << std::endl;
	unsigned long int test = myCrate->getDCCs()[0]->readTTCCommand();
	std::clog << "readTTCCommand: " << test << std::endl;
	std::clog << " @@@@@ Testing readTTCCommandAdvanced @@@@@ " << std::endl;
	test = myCrate->getDCCs()[0]->readTTCCommandAdvanced();
	std::clog << "readTTCCommandAdvanced: " << test << std::endl;

	std::clog << " @@@@@ Testing readUserCode @@@@@ " << std::endl;
	test = myCrate->getDCCs()[0]->inprom_userid();
	std::clog << "readUserCode(INPROM): " << test << std::endl;
	std::clog << " @@@@@ Testing readUserCodeAdvanced @@@@@ " << std::endl;
	test = myCrate->getDCCs()[0]->readUserCodeAdvanced(emu::fed::INPROM);
	std::clog << "readUserCodeAdvanced(INPROM): " << test << std::endl;

	std::clog << " @@@@@ Testing setTTCCommand @@@@@ " << std::endl;
	myCrate->getDCCs()[0]->setTTCCommand(0x34);
	std::clog << " @@@@@ Testing writeTTCCommandAdvanced @@@@@ " << std::endl;
	myCrate->getDCCs()[0]->writeTTCCommandAdvanced(0x34);
	*/
	
	// I should test the eprom load here, but I need to be sure everything works.
	// Get the svf file from http://www.physics.ohio-state.edu/~cms/firmwares/dcc3drx_v9_r3.svf
	/*
	std::clog << " @@@@@ Testing epromload @@@@@ " << std::endl;
	myCrate->getDCCs()[0]->epromload("INPROM",emu::fed::INPROM,"/tmp/dcc3drx_v9_r3.svf",1);
	std::clog << " @@@@@ Testing loadPROMAdvanced @@@@@ " << std::endl;
	myCrate->getDCCs()[0]->loadPROMAdvanced(emu::fed::INPROM,"/tmp/dcc3drx_v9_r3.svf");
	*/
	/*
	std::clog << " @@@@@ Testing readCSCStat @@@@@ " << std::endl;
	unsigned long int test = myCrate->getDDUs()[0]->readCSCStat();
	std::clog << "readCSCStat: " << std::hex << test << std::endl;
	std::clog << " @@@@@ Testing readCSCStatAdvanced @@@@@ " << std::endl;
	test = myCrate->getDDUs()[0]->readCSCStatAdvanced();
	std::clog << "readCSCStatAdvanced: " << test << std::endl;

	std::clog << " @@@@@ Testing readSerialStat @@@@@ " << std::endl;
	test = myCrate->getDDUs()[0]->readSerialStat();
	std::clog << "readSerialStat: " << test << std::endl;
	std::clog << " @@@@@ Testing readSerialStatAdvanced @@@@@ " << std::endl;
	test = myCrate->getDDUs()[0]->readSerialStatAdvanced();
	std::clog << "readSerialStatAdvanced: " << test << std::endl;

	std::clog << " @@@@@ Testing read_page5 @@@@@ " << std::endl;
	std::vector<int> vtest = myCrate->getDDUs()[0]->read_page5();
	std::clog << "read_page5: ";
	for (std::vector<int>::iterator iTest = vtest.begin(); iTest != vtest.end(); iTest++) {
		std::clog << test;
	}
	std::clog << std::endl;
	std::clog << " @@@@@ Testing readGbEFIFOThresholdsAdvanced @@@@@ " << std::endl;
	std::vector<int16_t> vtest2 = myCrate->getDDUs()[0]->readGbEFIFOThresholdsAdvanced();
	std::clog << "readGbEFIFOThresholdsAdvanced: ";
	for (std::vector<int16_t>::iterator iTest = vtest2.begin(); iTest != vtest2.end(); iTest++) {
		std::clog << test;
	}
	std::clog << std::endl;

	std::clog << " @@@@@ Testing readthermx(2) @@@@@ " << std::endl;
	float ftest = myCrate->getDDUs()[0]->readthermx(2);
	std::clog << "readthermx(2): " << ftest << std::endl;
	std::clog << " @@@@@ Testing readTemperatureAdvanced(2) @@@@@ " << std::endl;
	ftest = myCrate->getDDUs()[0]->readTemperatureAdvanced(2);
	std::clog << "readTemperatureAdvanced(2): " << ftest << std::endl;

	std::clog << " @@@@@ Testing adcplus(1,7) @@@@@ " << std::endl;
	ftest = myCrate->getDDUs()[0]->adcplus(1,7);
	std::clog << "adcplus(1,7): " << ftest << std::endl;
	std::clog << " @@@@@ Testing readVoltageAdvanced(3) @@@@@ " << std::endl;
	ftest = myCrate->getDDUs()[0]->readVoltageAdvanced(3);
	std::clog << "readVoltageAdvanced(3): " << ftest << std::endl;

	std::clog << " @@@@@ Testing ddu_rdkillfiber @@@@@ " << std::endl;
	test = myCrate->getDDUs()[0]->ddu_rdkillfiber();
	std::clog << "ddu_rdkillfiber: " << test << std::endl;
	std::clog << " @@@@@ Testing readKillFiberAdvanced @@@@@ " << std::endl;
	test = myCrate->getDDUs()[0]->readKillFiberAdvanced();
	std::clog << "readKillFiberAdvanced: " << test << std::endl;

	std::clog << " @@@@@ Testing ddu_loadkillfiber(0xf7654) @@@@@ " << std::endl;
	myCrate->getDDUs()[0]->ddu_loadkillfiber(0xf7654);
	std::clog << " @@@@@ Testing writeKillFiberAdvanced(0xf3210) @@@@@ " << std::endl;
	myCrate->getDDUs()[0]->writeKillFiberAdvanced(0xf3210);

	std::clog << " @@@@@ Testing infpga_CcodeStat(emu::fed::INFPGA0) @@@@@ " << std::endl;
	test = myCrate->getDDUs()[0]->infpga_CcodeStat(emu::fed::INFPGA0);
	std::clog << "infpga_CcodeStat(emu::fed::INFPGA0): " << test << std::endl;
	std::clog << " @@@@@ Testing readCCodeStatAdvanced(emu::fed::INFPGA0) @@@@@ " << std::endl;
	test = myCrate->getDDUs()[0]->readCCodeStatAdvanced(emu::fed::INFPGA0);
	std::clog << "readCCodeStatAdvanced(emu::fed::INFPGA0): " << test << std::endl;

	std::clog << " @@@@@ Testing infpga_CcodeStat(emu::fed::INFPGA1) @@@@@ " << std::endl;
	test = myCrate->getDDUs()[0]->infpga_CcodeStat(emu::fed::INFPGA1);
	std::clog << "infpga_CcodeStat(emu::fed::INFPGA1): " << test << std::endl;
	std::clog << " @@@@@ Testing readCCodeStatAdvanced(emu::fed::INFPGA1) @@@@@ " << std::endl;
	test = myCrate->getDDUs()[0]->readCCodeStatAdvanced(emu::fed::INFPGA1);
	std::clog << "readCCodeStatAdvanced(emu::fed::INFPGA1): " << test << std::endl;

	std::clog << " @@@@@ Testing infpga_usercode0() @@@@@ " << std::endl;
	test = myCrate->getDDUs()[0]->infpga_usercode0();
	std::clog << "infpga_usercode0(): " << test << std::endl;
	std::clog << " @@@@@ Testing readUserCodeAdvanced(emu::fed::INFPGA0) @@@@@ " << std::endl;
	test = myCrate->getDDUs()[0]->readUserCodeAdvanced(emu::fed::INFPGA0);
	std::clog << "readUserCodeAdvanced(emu::fed::INFPGA0): " << test << std::endl;

	std::clog << " @@@@@ Testing infpga_usercode1() @@@@@ " << std::endl;
	test = myCrate->getDDUs()[0]->infpga_usercode1();
	std::clog << "infpga_usercode1(): " << test << std::endl;
	std::clog << " @@@@@ Testing readUserCodeAdvanced(emu::fed::INFPGA1) @@@@@ " << std::endl;
	test = myCrate->getDDUs()[0]->readUserCodeAdvanced(emu::fed::INFPGA1);
	std::clog << "readUserCodeAdvanced(emu::fed::INFPGA1): " << test << std::endl;

	std::clog << " @@@@@ Testing infpga_idcode0() @@@@@ " << std::endl;
	test = myCrate->getDDUs()[0]->infpga_idcode0();
	std::clog << "infpga_idcode0(): " << test << std::endl;
	std::clog << " @@@@@ Testing readIDCodeAdvanced(emu::fed::INFPGA0) @@@@@ " << std::endl;
	test = myCrate->getDDUs()[0]->readIDCodeAdvanced(emu::fed::INFPGA0);
	std::clog << "readIDCodeAdvanced(emu::fed::INFPGA0): " << test << std::endl;
	
	std::clog << " @@@@@ Testing infpga_idcode1() @@@@@ " << std::endl;
	test = myCrate->getDDUs()[0]->infpga_idcode1();
	std::clog << "infpga_idcode1(): " << test << std::endl;
	std::clog << " @@@@@ Testing readIDCodeAdvanced(emu::fed::INFPGA1) @@@@@ " << std::endl;
	test = myCrate->getDDUs()[0]->readIDCodeAdvanced(emu::fed::INFPGA1);
	std::clog << "readIDCodeAdvanced(emu::fed::INFPGA1): " << test << std::endl;

	std::clog << " @@@@@ Testing dduprom_usercode0() @@@@@ " << std::endl;
	test = myCrate->getDDUs()[0]->dduprom_usercode0();
	std::clog << "dduprom_usercode0(): " << test << std::endl;
	std::clog << " @@@@@ Testing readUserCodeAdvanced(emu::fed::DDUPROM0) @@@@@ " << std::endl;
	test = myCrate->getDDUs()[0]->readUserCodeAdvanced(emu::fed::DDUPROM0);
	std::clog << "readUserCodeAdvanced(emu::fed::DDUPROM0): " << test << std::endl;
	
	std::clog << " @@@@@ Testing dduprom_usercode1() @@@@@ " << std::endl;
	test = myCrate->getDDUs()[0]->dduprom_usercode1();
	std::clog << "dduprom_usercode1(): " << test << std::endl;
	std::clog << " @@@@@ Testing readUserCodeAdvanced(emu::fed::DDUPROM1) @@@@@ " << std::endl;
	test = myCrate->getDDUs()[0]->readUserCodeAdvanced(emu::fed::DDUPROM1);
	std::clog << "readUserCodeAdvanced(emu::fed::DDUPROM1): " << test << std::endl;
	
	std::clog << " @@@@@ Testing dduprom_idcode0() @@@@@ " << std::endl;
	test = myCrate->getDDUs()[0]->dduprom_idcode0();
	std::clog << "dduprom_idcode0(): " << test << std::endl;
	std::clog << " @@@@@ Testing readIDCodeAdvanced(emu::fed::DDUPROM0) @@@@@ " << std::endl;
	test = myCrate->getDDUs()[0]->readIDCodeAdvanced(emu::fed::DDUPROM0);
	std::clog << "readIDCodeAdvanced(emu::fed::DDUPROM0): " << test << std::endl;
	
	std::clog << " @@@@@ Testing dduprom_idcode1() @@@@@ " << std::endl;
	test = myCrate->getDDUs()[0]->dduprom_idcode1();
	std::clog << "dduprom_idcode1(): " << test << std::endl;
	std::clog << " @@@@@ Testing readIDCodeAdvanced(emu::fed::DDUPROM1) @@@@@ " << std::endl;
	test = myCrate->getDDUs()[0]->readIDCodeAdvanced(emu::fed::DDUPROM1);
	std::clog << "readIDCodeAdvanced(emu::fed::DDUPROM1): " << test << std::endl;
	*/
	/*
	std::clog << " @@@@@ Testing epromload up to usercode @@@@@ " << std::endl;
	char *BN = (char *) malloc(4);
	BN[0] = 0x55; BN[1] = 0; BN[2] = 0; BN[3] = 0;
	myCrate->getDDUs()[0]->epromload("DDUPROM1",emu::fed::DDUPROM1,"/tmp/ddu5ctrl_1.svf",1,(char *)BN,1);
	std::clog << " @@@@@ Testing epromload of usercode @@@@@ " << std::endl;
	myCrate->getDDUs()[0]->epromload("DDUPROM1",emu::fed::DDUPROM1,"/tmp/ddu5ctrl_1.svf",1,(char *)BN,2);
	std::clog << " @@@@@ Testing epromload after usercode @@@@@ " << std::endl;
	myCrate->getDDUs()[0]->epromload("DDUPROM1",emu::fed::DDUPROM1,"/tmp/ddu5ctrl_1.svf",1,(char *)BN,3);
	
	std::clog << " @@@@@ Testing loadPROMAdvanced up to usercode @@@@@ " << std::endl;
	myCrate->getDDUs()[0]->loadPROMAdvanced(emu::fed::DDUPROM1,"/tmp/ddu5ctrl_1.svf","","c045dd99");
	std::clog << " @@@@@ Testing load of usercode @@@@@ " << std::endl;
	std::vector<int16_t> writeMe(2);
	writeMe.push_back(0xdd00 | 0x55);
	writeMe.push_back(0xc045);
	myCrate->getDDUs()[0]->jtagReadWrite(emu::fed::DDUPROM1, 32, writeMe, true);
	std::clog << " @@@@@ Testing loadPROMAdvanced after usercode @@@@@ " << std::endl;
	myCrate->getDDUs()[0]->loadPROMAdvanced(emu::fed::DDUPROM1,"/tmp/ddu5ctrl_1.svf","c045dd99");
	*/
	try {

		// Check for errors in crates.  Should only have crate numbers 1-5,
		// and there should not be more than one crate with a given number.
		// The crates should also have a unique VME controller.
		int crateError = 0;
		for (unsigned int iCrate=0; iCrate<crateVector.size(); iCrate++) {
			if (crateVector[iCrate]->number() > 5 || crateVector[iCrate]->number() < 1) {
				*out << cgicc::div().set("style","background-color: #000; color: #FAA; font-weight: bold; margin-bottom: 0px;") << "Crate " << crateVector[iCrate]->number() << " has an invalid number (should be 1-5)" << cgicc::div() << std::endl;
				crateError++;
			}
			for (unsigned int jCrate=iCrate+1; jCrate<crateVector.size(); jCrate++) {
				if (crateVector[iCrate]->number() == crateVector[jCrate]->number()) {
					*out << cgicc::div().set("style","background-color: #000; color: #FAA; font-weight: bold; margin-bottom: 0px;") << "Two crates share crate number " << crateVector[iCrate]->number() << cgicc::div() << std::endl;
					crateError++;
				}
				if (crateVector[iCrate]->getVMEController()->getDevice() == crateVector[jCrate]->getVMEController()->getDevice()) {
					*out << cgicc::div().set("style","background-color: #000; color: #FAA; font-weight: bold; margin-bottom: 0px;") << "Crates " << crateVector[iCrate]->number() << " and " << crateVector[jCrate]->number() << " have the same VME controller device number (" << crateVector[iCrate]->getVMEController()->getDevice() << ")" << cgicc::div() << std::endl;
					crateError++;
				}
			}
		}
		if (crateError) {
			*out << cgicc::div().set("style","background-color: #000; color: #FAA; font-weight: bold; margin-bottom: 0px;") << "You have " << crateError << " error" << (crateError != 1 ? "s" : "") << " in your XML configuration file.  MAKE SURE YOU UNDERSTAND WHAT YOU ARE DOING BEFORE CONTINUING WITH THIS CONFIGURATION.  If you did not expect this message, fix your configuration file and reload it with the button at the bottom of the page." << cgicc::div() << std::endl;
		}

		// PGK The Reload button is not really useful anymore.
		/*
		std::string reload = toolbox::toString("/%s",getApplicationDescriptor()->getURN().c_str());
		*out << cgicc::input()
			.set("type","button")
			.set("value","Reload Page")
			.set("onClick","window.location.href='"+reload+"'") << std::endl; // Javascript trick
		*out << cgicc::br() << std::endl;
		*/

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
		//std::cout << " dduVector.size() " << dduVector.size() << std::endl;

		// PGK CAEN gets in a funny state after doing crate switching.  It doesn't seem to
		//  cause any problems, but it does fail to read anything useful from the first
		//  ddu in dduVector after performing a certain series of reads and crate switches.
		//  If you communicate to any other slot after switching to the new crate, the problem
		//  never shows up.  This is what I'm doing here--communicate to the DCC if it exists.
		//  If there is no DCC, then you'll just have to live with the error, I guess.

		
		//if (myCrate->getDCCs().size()) {
		//std::cout << " pinging DCC to avoid CAEN read error -1" << std::endl;
		//myCrate->getDCCs()[0]->readStatusLow();
		//}
		

		// Unfortunately, the DDU routines are called with the index of the
		//  given DDU in the DDU std::vector, not the module std::vector.
		//  This will help us keep track of what DDU we are on.
		unsigned int iddu = 0;
		// Same goes for DCCs (when we get more than 1)
		unsigned int idcc = 0;

		// Loop over all the DDUs
		std::vector<emu::fed::DDU *> myDDUs = myCrate->getDDUs();
		std::vector<emu::fed::DDU *>::iterator iDDU;
		for (iDDU = myDDUs.begin(); iDDU != myDDUs.end(); iDDU++) {
			// Determine if we are working on a DDU or a DCC by module type
			//(*iDDU) = dynamic_cast<emu::fed::DDU *>(moduleVector[iModule]);
			//thisDCC = dynamic_cast<emu::fed::DCC *>(moduleVector[iModule]);

			// Skip broadcasting
			if ((*iDDU)->isBroadcast()) continue;

			// First, determine the status of the DDU.
			//myCrate->getVMEController()->CAEN_err_reset();
			// Do a fast FMM status check
			//unsigned short int DDU_FMM = (((*iDDU)->vmepara_status()>>8)&0x000F);
			unsigned short int DDU_FMM = (((*iDDU)->readParallelStat()>>8)&0x000F);
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
			unsigned int fibersWithErrors = (*iDDU)->readCSCStat();
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
			*out << "DDU L1As: " << std::dec << (*iDDU)->ddu_rdscaler();
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
			long int liveFibers = ((*iDDU)->checkFiber(emu::fed::INFPGA0)&0x000000ff) | (((*iDDU)->checkFiber(emu::fed::INFPGA1)&0x000000ff)<<8);
			long int killFiber = (*iDDU)->ddu_rdkillfiber();

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
				emu::fed::Chamber *thisChamber = (*iDDU)->getChamber(iFiber);
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
				location << appString[iButton] << "?crate=" << cgiCrate << "&ddu=" << iddu;
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

			// The DDU counter
			iddu++;
		}
		
		// Loop over all the DCCs
		std::vector<emu::fed::DCC *>::iterator iDCC;
		std::vector<emu::fed::DCC *> myDCCs = myCrate->getDCCs();
		for (iDCC = myDCCs.begin(); iDCC != myDCCs.end(); iDCC++) {

			// Skip broadcasting
			if ((*iDCC)->isBroadcast()) continue;

			// First, determine the status of the DCC.
			//myCrate->getVMEController()->CAEN_err_reset();
			unsigned short int statush = (*iDCC)->readStatusHigh();
			unsigned short int statusl = (*iDCC)->readStatusLow();
			unsigned short int rdfifoinuse = (*iDCC)->readFIFOInUse();
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
			*out << cgicc::td()
				.set("class",dccStatus)
				.set("style","border-bottom: 1px solid #000; width: 18%;");
			*out << "FIFO in use: " << std::uppercase << std::setw(4) << std::hex << (rdfifoinuse&0x3ff) << std::dec << "h";
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
				// Jason likes a space after the first button.
				std::stringstream location;
				location << appString[iButton] << "?crate=" << cgiCrate << "&dcc=" << idcc;
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

			idcc++;

		} // end VME Module loop.

		*out << std::endl;
		*out << cgicc::fieldset() << std::endl;

		// Firmware Table...
		/*
		emu::fed::DDU *broadcastDDU = NULL;
		for (std::vector<emu::fed::DDU *>::iterator iDDU = myDDUs.begin(); iDDU != myDDUs.end(); iDDU++) {
			if ((*iDDU)->slot() > 21) broadcastDDU = (*iDDU);
		}
		emu::fed::DCC *broadcastDCC = NULL;
		for (std::vector<emu::fed::DCC *>::iterator iDCC = myDCCs.begin(); iDCC != myDCCs.end(); iDCC++) {
			if ((*iDCC)->slot() > 21) broadcastDCC = (*iDCC);
		}
		*/

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

		// DCC button.
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
		//std::string loadxmlconf =  toolbox::toString("/%s/LoadXMLconf",getApplicationDescriptor()->getURN().c_str());
		std::string loadxmlconf = "/" + getApplicationDescriptor()->getURN() + "/configurePage";

		*out << cgicc::fieldset()
			.set("class","expert") << std::endl;
		*out << cgicc::div("Experts Only")
			.set("class","legend") << std::endl;
		if (fcState_ == "Halted" || fcState_ == STATE_UNKNOWN) {
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
		} else {
			*out << "Change state to \"Halted\" before trying to change the configuration via HyperDAQ." << std::endl;
		}

		*out << cgicc::fieldset() << std::endl;

		*out << Footer();

	} catch (const std::exception & e ) {
		//XECPT_RAISE(xgi::exception::Exception, e.what());
	}
}



void EmuFCrateHyperDAQ::configurePage(xgi::Input *in, xgi::Output *out )
	throw (xgi::exception::Exception)
{
	std::cout << "--Entering Configure" << std::endl;

	std::string method;

	//Title(out,"Configure FED Crate HyperDAQ");

	try {

		/* PGK I hate fieldsets. */
		*out << cgicc::div("Choose one of the following configuration methods").set("class","title") << std::endl;

		/* Use a local file */
		method =
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
		toolbox::toString("/%s/UploadConfFile",getApplicationDescriptor()->getURN().c_str());

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
		*out << cgicc::div("Type a custon configuration")
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

	} catch (const std::exception & e ) {
		//XECPT_RAISE(xgi::exception::Exception, e.what());
	}
}


/// @Deprecated Use the crate form value instead
/*
void EmuFCrateHyperDAQ::setCrate(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception)
{
	std::cout << "--Entering setCrate" << std::endl;

	if (dduVector.size()==0 && dccVector.size()==0) {
		LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
		return Default(in,out);
	}
	
	Cgicc cgi(in);

	std::cout << "Switching crate..." << std::endl;
	int icrate = 0;
	if (!(cgi["icrate"]->isEmpty())) icrate = cgi["icrate"]->getIntegerValue();

	thisCrate = crateVector[icrate];
	std::cout << " vme device " << thisCrate->getVMEController()->Device() << std::endl;
	std::cout << " vme link " << thisCrate->getVMEController()->Link() << std::endl;
	dduVector = thisCrate->getDDUs();
	dccVector = thisCrate->getDCCs();
	moduleVector = thisCrate->modules();

	std::cout << " crate set to " << icrate << ", which is crate number " << thisCrate->number() << std::endl;
	in = NULL;
//	webRedirect(in, out);
	webRedirect(out,"mainPage");
	//mainPage(in,out);
}
*/



void EmuFCrateHyperDAQ::setRawConfFile(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	std::cout << "--Entering setRawConfFile" << std::endl;
	try {
		std::cout << "setRawConfFile" << std::endl;

		cgicc::Cgicc cgi(in);

		std::ofstream TextFile ;
		TextFile.open("MyTextConfigurationFile.xml");
		TextFile << (cgi["Text"]->getValue()) ;
		TextFile.close();

		xmlFile_ = "MyTextConfigurationFile.xml" ;
		std::cout << "Out setRawConfFile" << std::endl ;

		std::cout << "Clearing std::vectors to reset config." << std::endl;
		crateVector.clear();
		//dduVector.clear();
		//dccVector.clear();

		std::cout << "Load Default..." << std::endl;
		Default(in, out);
	} catch (const std::exception & e ) {
	//XECPT_RAISE(xgi::exception::Exception, e.what());
	}
}



void EmuFCrateHyperDAQ::setConfFile(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
 {
	std::cout << "--Entering setConfFile" << std::endl;
	try {
		//
		cgicc::Cgicc cgi(in);
		//

		/* No need for this any more.
		int SkipConfig=0;
		std::string Bid;
		SkipConfig=0;
		cgicc::form_iterator name = cgi.getElement("buttonid");
		if(name != cgi.getElements().end()) {
			Bid = cgi["buttonid"]->getValue();
			if(Bid=="Init Only")SkipConfig=1;
		}
		std::cout << "setConfFile: ButtonID=" << Bid << ".   SkipConfig=" << SkipConfig << std::endl ;
		*/

		cgicc::const_file_iterator file = cgi.getFile("xmlFileName");
		std::cout << "GetFiles std::string" << std::endl ;
		if(file != cgi.getFiles().end()) (*file).writeToStream(std::cout);
		std::string XMLname = cgi["xmlFileName"]->getValue();
		//std::cout << XMLname  << std::endl;
		xmlFile_ = XMLname;

		std::cout << "Clearing std::vectors to reset config." << std::endl;
		crateVector.clear();
		//dduVector.clear();
		//dccVector.clear();

		std::cout << "Load Default..." << std::endl;
		Default(in, out);
	} catch (const std::exception & e ) {
		//XECPT_RAISE(xgi::exception::Exception, e.what());
	}
}



void EmuFCrateHyperDAQ::UploadConfFile(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	try {
		std::cout << "UploadConfFileUpload" << std::endl ;
		cgicc::Cgicc cgi(in);
		cgicc::const_file_iterator file = cgi.getFile("xmlFileNameUpload");
		std::cout << "GetFiles" << std::endl ;
		if(file != cgi.getFiles().end()) {
			std::ofstream TextFile ;
			TextFile.open("MyTextConfigurationFile.xml");
			(*file).writeToStream(TextFile);
			TextFile.close();
		}
		xmlFile_ = "MyTextConfigurationFile.xml" ;

		std::cout << "Clearing std::vectors to reset config." << std::endl;
		crateVector.clear();
		//dduVector.clear();
		//dccVector.clear();

		std::cout << "Load Default..." << std::endl;
		Default(in, out);
	} catch (const std::exception & e ) {
		//XECPT_RAISE(xgi::exception::Exception, e.what());
	}
}



void EmuFCrateHyperDAQ::Configuring() {
	std::cout << "--Entering Configuring" << std::endl;
/*
	cgicc::Cgicc cgi(in);
	const CgiEnvironment& env = cgi.getEnvironment();
	std::string crateStr = env.getQueryString() ;
*/

	//std::cout << "Configuring: SkipConfig=" << SkipConfig << std::endl ;

	//
	//-- parse XML file
	//
	std::cout << "---- XML parser ----" << std::endl;
//	std::cout << " Here parser " << std::endl;
	emu::fed::FEDCrateParser parser;
	std::cout << " Using file " << xmlFile_.toString() << std::endl ;
	parser.parseFile(xmlFile_.toString().c_str());
	std::cout <<"---- Parser Finished ----"<<std::endl;

	std::cout << " clearing std::vectors..." << std::endl;
	crateVector.clear();
	//dduVector.clear();
	//dccVector.clear();

	std::cout << " setting std::vectors..." << std::endl;
	crateVector = parser.getCrates();

	// Check the controller and see if we need to (a) get the BHandle from
	//  EmuFCrate, or (b) send the BHandle to EmuFCrate.  This is done in
	//  lieu of externs, globals, or singletons.
	
	// First, we must make a system to parse out strings.  String-fu!
	// Strings looke like this:  "Crate# BHandle# Crate# BHandle# Crate# BHandle#..."
	std::map< int, int > BHandles;
	xoap::MessageReference reply = getParameters("EmuFCrate",getApplicationDescriptor()->getInstance());
	xdata::String BHandleString = readParameter<xdata::String>(reply, "BHandles");
	
	LOG4CPLUS_DEBUG(getApplicationLogger(),"Got old handles: " << BHandleString.toString());
	
	std::stringstream sHandles(BHandleString.toString());
	int buffer;
	while (sHandles >> buffer) {
		int crateNumber = buffer;
		sHandles >> buffer;
		int BHandle = buffer;
		
		BHandles[crateNumber] = BHandle;
	}
	
	std::ostringstream newHandles;
	
	for (std::vector< emu::fed::FEDCrate * >::iterator iCrate = crateVector.begin(); iCrate != crateVector.end(); iCrate++) {
		emu::fed::VMEController *myController = (*iCrate)->getVMEController();
		if (myController->getBHandle() == -1) {
			LOG4CPLUS_INFO(getApplicationLogger(),"Controller in crate " << (*iCrate)->number() << " has already been opened by someone else.  Asking EmuFCrate for the BHandle...");
			
			for (std::map<int,int>::iterator iHandle = BHandles.begin(); iHandle != BHandles.end(); iHandle++) {
				if (iHandle->first != (*iCrate)->number()) continue;
				LOG4CPLUS_INFO(getApplicationLogger(),"Found handle " << iHandle->second);
				(*iCrate)->setBHandle(iHandle->second);
				
				newHandles << iHandle->first << " " << iHandle->second << " ";
				
				break;
			}
		} else {
			LOG4CPLUS_INFO(getApplicationLogger(),"Controller in crate " << (*iCrate)->number() << " has been first opened by this application.  Telling EmuFCrate the BHandle...");
			
			bool replaced = false;
			
			for (std::map<int,int>::iterator iHandle = BHandles.begin(); iHandle != BHandles.end(); iHandle++) {
				if (iHandle->first != (*iCrate)->number()) continue;
				LOG4CPLUS_INFO(getApplicationLogger(),"Resetting handle (was " << iHandle->second << ")");
				replaced = true;
				newHandles << (*iCrate)->number() << " " << myController->getBHandle() << " ";
			}
			
			if (!replaced) newHandles << (*iCrate)->number() << " " << myController->getBHandle() << " ";
			
		}

	}
	
	LOG4CPLUS_DEBUG(getApplicationLogger(),"Sending new handles: " << newHandles.str());
	
	setParameter("EmuFCrate","BHandles","xsd:string",newHandles.str(),getApplicationDescriptor()->getInstance());
	
	//dduVector = crateVector[0]->getDDUs();
	//dccVector = crateVector[0]->getDCCs();
	//moduleVector = crateVector[0]->modules();
	//std::cout << " crateVector["<<crateVector.size()<<"] dduVector["<<dduVector.size()<<"] dccVector["<<dccVector.size()<<"]" << std::endl;

	//thisCrate = crateVector[0];

// LSD, Make these optional with buttons
// JRG, only start/reset the IRQ handler:
// JRG uncomment?  if(SkipConfig>0) thisCrate->init(0);
	//if(SkipConfig==0) thisCrate->configure(0);

	std::cout << "EmuFCrateHyperDAQ::Configuring  >done< " << std::endl ;
}



/*
void EmuFCrateHyperDAQ::DDUFirmware(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{

	// PGK Patented check-for-initialization
	if (crateVector.size()==0) {
		LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
		return Default(in,out);
	}

	cgicc::Cgicc cgi(in);
	
	// First, I need a crate.
	cgicc::form_iterator name = cgi.getElement("crate");
	unsigned int cgiCrate = 0;
	if(name != cgi.getElements().end()) {
		cgiCrate = cgi["crate"]->getIntegerValue();
	}
	Crate *myCrate = crateVector[cgiCrate];

	name = cgi.getElement("ddu");
	unsigned int cgiDDU = 0;
	if (name != cgi.getElements().end()) {
		cgiDDU = cgi["ddu"]->getIntegerValue();
		//std::cout << "DDU inside " << ddu << std::endl;
	}
	emu::fed::DDU *myDDU = myCrate->getDDUs()[cgiDDU];

	std::stringstream sTitle;
	sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DDU Firmware (RUI #" << myCrate->getRUI(myDDU->slot()) << ")";
	*out << Header(sTitle.str(),false);

	
	// PGK Select-a-DDU
	*out << cgicc::fieldset()
		.set("class","header") << std::endl;
	*out << cgicc::div("Show this page for a different DDU")
		.set("style","font-weight: bold; font-size: 8pt; width: 100%; text-align: center;") << std::endl;

	unsigned int iddu = 0;

	*out << cgicc::table()
		.set("style","width: 90%; margin: 2px auto 2px auto; font-size: 8pt; text-align: center;") << std::endl;
	*out << cgicc::tr() << std::endl;
	// Loop over all the boards defined.
	for (unsigned int iModule = 0; iModule < moduleVector.size(); iModule++) {

		// Determine if we are working on a DDU or a DCC by module type
		myDDU = dynamic_cast<emu::fed::DDU *>(moduleVector[iModule]);
		if (myDDU != 0) {
			// I am a DDU!
			// Skip broadcasting
			if (myDDU->slot() > 21) continue;

			*out << cgicc::td() << std::endl;
			if (iddu != cgiDDU) {
				std::stringstream location;
				location << "/" + getApplicationDescriptor()->getURN() + "/DDUFirmware?ddu=" << iddu;
				*out << cgicc::a()
					.set("href",location.str());
			}
			*out << "Slot " << myDDU->slot() << ": RUI #" << myCrate->getRUI(myDDU->slot());
			if (iddu != cgiDDU) {
				*out << cgicc::a();
			}
			*out << cgicc::td() << std::endl;

			iddu++;
		}
	}
	*out << cgicc::tr() << std::endl;
	*out << cgicc::table() << std::endl;
	*out << cgicc::fieldset() << std::endl;

	// Get this DDU back again.
	std::vector<emu::fed::DDU *> myDDUs = myCrate->getDDUs();
	myDDU = myDDUs[cgiDDU];

	// PGK Display-a-Crate
	*out << cgicc::fieldset()
		.set("class","fieldset") << std::endl;
	*out << cgicc::div("Crate Selection")
		.set("class","legend") << std::endl;

	*out << cgicc::div()
		.set("style","background-color: #FF0; color: #00A; font-size: 16pt; font-weight: bold; width: 50%; text-align: center; padding: 3px; border: 2px solid #00A; margin: 5px auto 5px auto;");
	*out << "Crate " << myCrate->number() << " Selected";
	*out << cgicc::div() << std::endl;

	*out << cgicc::span("Configuration located at " + xmlFile_.toString())
		.set("style","color: #A00; font-size: 10pt;") << std::endl;

	*out << cgicc::fieldset() << std::endl;
	*out << cgicc::br() << std::endl;

	*out << cgicc::fieldset()
		.set("class","fieldset") << std::endl;
	*out << cgicc::div("DDU Firmware User/ID Codes")
		.set("class","legend") << std::endl;

	// Pick up the FPGA and PROM user/id codes.
	myCrate->getVMEController()->CAEN_err_reset();
	// There are names to these things.
	std::string deviceNames[8] = {
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

	idCode[0] = myDDU->ddufpga_idcode();
	idCode[1] = myDDU->infpga_idcode0();
	idCode[2] = myDDU->infpga_idcode1();
	idCode[3] = myDDU->vmeprom_idcode();
	idCode[4] = myDDU->dduprom_idcode0();
	idCode[5] = myDDU->dduprom_idcode1();
	idCode[6] = myDDU->inprom_idcode0();
	idCode[7] = myDDU->inprom_idcode1();

	userCode[0] = myDDU->ddufpga_usercode();
	userCode[1] = myDDU->infpga_usercode0();
	userCode[2] = myDDU->infpga_usercode1();
	userCode[3] = myDDU->vmeprom_usercode();
	userCode[4] = myDDU->dduprom_usercode0();
	userCode[5] = myDDU->dduprom_usercode1();
	userCode[6] = myDDU->inprom_usercode0();
	userCode[7] = myDDU->inprom_usercode1();

	// Next, print the table with the codes.
	std::string dduClass = "";
	if (myCrate->getVMEController()->CAEN_err()!=0) {
		dduClass = "caution";
	}

	*out << cgicc::table()
		.set("style","width: 90%; margin: 5px auto 5px auto; text-align: center; border-collapse: collapse; border: 2px solid #000;") << std::endl;

	// First row:  prom/fpga labels
	*out << cgicc::tr() << std::endl;
	// The first element is special:  DDU
	*out << cgicc::td()
		.set("class",dduClass)
		.set("style","border-right: 1px solid #000; font-weight: bold; width: 12%;");
	*out << "DDU";
	*out << cgicc::td() << std::endl;
	// The second element is special:  blank
	*out << cgicc::td()
		.set("class",dduClass)
		.set("style","border-right: 1px solid #000; font-weight: bold; width: 10%;");
	*out << "";
	*out << cgicc::td() << std::endl;

	// Loop over the ID types
	for (unsigned int iCode = 0; iCode < 8; iCode++) {
		*out << cgicc::td()
			.set("class","")
			.set("style","border: 1px solid #000; width: 10%; font-weight: bold;");
		*out << deviceNames[iCode];
		*out << cgicc::td() << std::endl;
	}
	*out << cgicc::tr() << std::endl;

	// Second row:  user codes
	*out << cgicc::tr() << std::endl;
	// The first element is special:  DDU
	*out << cgicc::td()
		.set("class",dduClass)
		.set("style","border-right: 1px solid #000; font-weight: bold; width: 12%;");
	*out << "Slot " << myDDU->slot();
	*out << cgicc::td() << std::endl;
	// The second element is special:  label
	*out << cgicc::td()
		.set("class",dduClass)
		.set("style","border-right: 1px solid #000; font-weight: bold; width: 10%;");
	*out << "User Code";
	*out << cgicc::td() << std::endl;

	// Loop over the ID types
	for (unsigned int iCode = 0; iCode < 8; iCode++) {
		std::string codeClass = "ok";
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
		*out << std::hex << std::uppercase << userCode[iCode] << std::dec;
		*out << cgicc::td() << std::endl;
	}
	*out << cgicc::tr() << std::endl;

	// Third row:  id codes
	*out << cgicc::tr() << std::endl;

	// The first element is special:  RUI
	*out << cgicc::td()
		.set("class",dduClass)
		.set("style","border-right: 1px solid #000; font-weight: bold; width: 10%;");
	*out << "RUI #" << myCrate->getRUI(myDDU->slot());
	*out << cgicc::td() << std::endl;
	// The second element is special:  label
	*out << cgicc::td()
		.set("class",dduClass)
		.set("style","border-right: 1px solid #000; font-weight: bold; width: 10%;");
	*out << "ID Code";
	*out << cgicc::td() << std::endl;

	// Loop over the ID types again
	for (unsigned int iCode = 0; iCode < 8; iCode++) {
		std::string codeClass = "ok";
		// Check to see if these match the expectations
		if (idCode[iCode] != tidcode[iCode]) {
			codeClass = "bad";
		}

		*out << cgicc::td()
			.set("class",codeClass)
			.set("style","border: 1px solid #000; width: 10%;");
		*out << std::hex << std::uppercase << idCode[iCode] << std::dec;
		*out << cgicc::td() << std::endl;
	}
	*out << cgicc::tr() << std::endl;

	*out << cgicc::table() << std::endl;

	*out << cgicc::fieldset() << std::endl;

	myDDU = myDDUs[cgiDDU];

	// Now it's time to put the upload forms for the selected DDU
	*out << cgicc::fieldset()
		.set("class","fieldset") << std::endl;
	std::stringstream uploadTitle;
	uploadTitle << "DDU Firmware Upload (Slot " << myDDU->slot() << ", RUI #" << myCrate->getRUI(myDDU->slot()) << ")";
	*out << cgicc::div(uploadTitle.str())
		.set("class","legend") << std::endl;

	// There's five things to upload:
	std::string names[5] = {
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
			.set("action","/"+getApplicationDescriptor()->getURN()+"/DDULoadFirmware") << std::endl;
		// NOTE: it's not exacly well defined if file type inputs
		// obey the value parameter.
		*out << cgicc::div(names[iPROM])
			.set("style","font-weight: bold;") << std::endl;
		*out << cgicc::input().set("type","file")
			.set("name","DDULoadSVF")
			.set("size","50") << std::endl;
		*out << cgicc::input()
			.set("type","submit")
			.set("value","LoadSVF") << std::endl;
		std::stringstream idduValue;
		idduValue << iddu;
		*out << cgicc::input()
			.set("type","hidden")
			.set("value",idduValue.str())
			.set("name","ddu") << std::endl;
		std::stringstream inputValue;
		inputValue << (iPROM+3);
		*out << cgicc::input()
			.set("type","hidden")
			.set("value",inputValue.str())
			.set("name","prom") << std::endl;
		*out << cgicc::form() << std::endl;
	}


	*out << cgicc::fieldset() << std::endl;

	*out << Footer() << std::endl;

}
*/


void EmuFCrateHyperDAQ::DDUBroadcast(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception)
{

	// PGK Patented check-for-initialization
	if (crateVector.size()==0) {
		LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
		return Default(in,out);
	}

	cgicc::Cgicc cgi(in);
	
	// First, I need a crate.
	cgicc::form_iterator name = cgi.getElement("crate");
	unsigned int cgiCrate = 0;
	if(name != cgi.getElements().end()) {
		cgiCrate = cgi["crate"]->getIntegerValue();
	}
	emu::fed::FEDCrate *myCrate = crateVector[cgiCrate];

	
	std::stringstream sTitle;
	sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DDU Firmware Manager";
	*out << Header(sTitle.str(),false);


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
	std::map< std::string, enum emu::fed::DEVTYPE > dduPROMTypes;
	dduPROMTypes["VMEPROM"] = emu::fed::VMEPROM;
	dduPROMTypes["DDUPROM0"] = emu::fed::DDUPROM0;
	dduPROMTypes["DDUPROM1"] = emu::fed::DDUPROM1;
	dduPROMTypes["INPROM0"] = emu::fed::INPROM0;
	dduPROMTypes["INPROM1"] = emu::fed::INPROM1;

	// The device types of the FPGA corresponding to the PROMS
	std::map< std::string, enum emu::fed::DEVTYPE > dduFPGATypes;
	dduFPGATypes["DDUPROM0"] = emu::fed::DDUFPGA;
	dduFPGATypes["DDUPROM1"] = emu::fed::DDUFPGA;
	dduFPGATypes["INPROM0"] = emu::fed::INFPGA0;
	dduFPGATypes["INPROM1"] = emu::fed::INFPGA1;

	// A map of PROM name to the first two identifying digits in the version
	//  number.
	std::map< std::string, std::string > dduPROMHeaders;
	dduPROMHeaders["VMEPROM"] = "b0";
	dduPROMHeaders["DDUPROM0"] = "c0";
	dduPROMHeaders["DDUPROM1"] = "c1";
	dduPROMHeaders["INPROM0"] = "d0";
	dduPROMHeaders["INPROM1"] = "d1";

	// Store the codes to compare them to the FPGAs
	std::map< std::string, std::string > diskPROMCodes;

	*out << cgicc::fieldset()
		.set("class","normal") << std::endl;
	*out << cgicc::div("Step 1:  Upload DDU firmware to disk")
		.set("class","legend") << std::endl;
	
	emu::fed::DataTable diskTable("diskTable");

	diskTable.addColumn("DDU PROM Name");
	diskTable.addColumn("On-Disk Firmware Version");
	diskTable.addColumn("Upload New Firmware");

	// Loop over the prom types and give us a pretty table
	for (unsigned int iprom = 0; iprom < dduPROMNames.size(); iprom++) {
	
		*(diskTable(iprom,0)->value) << dduPROMNames[iprom];

		// Get the version number from the on-disk file
		// Best mashup of perl EVER!
		std::ostringstream systemCall;
		std::string diskVersion;

		// My perl-fu is 1337, indeed!
		systemCall << "perl -e 'while ($line = <>) { if ($line =~ /SIR 8 TDI \\(fd\\) TDO \\(00\\) ;/) { $line = <>; if ($line =~ /TDI \\((........)\\)/) { print $1; } } }' <Current" << dduPROMNames[iprom] << ".svf >check_ver 2>&1";
		if (!system(systemCall.str().c_str())) {
			std::ifstream pipein("check_ver",std::ios::in);
			getline(pipein,diskVersion);
			pipein.close();
		}

		// Now the std::string diskVersion is exactly what is sounds like.
		diskPROMCodes[dduPROMNames[iprom]] = diskVersion;

		// Check to make sure the on-disk header looks like it should for that
		//  particular PROM
		std::string diskHeader( diskVersion, 0, 2 );
		if ( diskHeader != dduPROMHeaders[dduPROMNames[iprom]] ) {
			*(diskTable(iprom,1)->value) << "ERROR READING LOCAL FILE -- UPLOAD A NEW FILE";
			diskTable(iprom,1)->setClass("bad");
		} else {
			*(diskTable(iprom,1)->value) << diskVersion;
			diskTable(iprom,1)->setClass("ok");
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
		*(diskTable(iprom,2)->value) << cgicc::form().set("method","POST")
			.set("enctype","multipart/form-data")
			.set("id","Form" + dduPROMNames[iprom])
			.set("action","/" + getApplicationDescriptor()->getURN() + "/DDULoadBroadcast") << std::endl;
		*(diskTable(iprom,2)->value) << cgicc::input().set("type","file")
			.set("name","File")
			.set("id","File" + dduPROMNames[iprom])
			.set("size","50") << std::endl;
		*(diskTable(iprom,2)->value) << cgicc::input().set("type","button")
			.set("value","Upload SVF")
			.set("onClick","javascript:clickCheck('" + dduPROMNames[iprom] + "')") << std::endl;
		*(diskTable(iprom,2)->value) << cgicc::input().set("type","hidden")
			.set("name","svftype")
			.set("value",dduPROMNames[iprom]) << std::endl;
		*(diskTable(iprom,2)->value) << cgicc::form() << std::endl;
	}	
	// Tricky: javascript check!
	*out << "<script type=\"text/javascript\">function clickCheck(id) {" << std::endl;
	*out << "var element = document.getElementById('File' + id);" << std::endl;
	*out << "if (element.value == '') element.style.backgroundColor = '#FFCCCC';" << std::endl;
	*out << "else {" << std::endl;
	*out << "var form = document.getElementById('Form' + id);" << std::endl;
	*out << "form.submit();" << std::endl;
	*out << "}" << std::endl;
	*out << "}</script>" << std::endl;

	// Print the table to screen.
	*out << diskTable.toHTML() << std::endl;

	*out << cgicc::fieldset() << std::endl;


	// This is the key:  a form that is outside of everything, and bitwise
	//  selectors of the slots to upgrade.
	*out << cgicc::form()
		.set("action","/" + getApplicationDescriptor()->getURN() + "/DDUSendBroadcast")
		.set("method","GET");
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","slots")
		.set("id","slots")
		.set("value","0x0");
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","broadcast")
		.set("id","broadcast")
		.set("value","0");

	for (unsigned int iprom = 0; iprom < dduPROMNames.size(); iprom++) {
		*out << cgicc::input()
			.set("type","hidden")
			.set("name",dduPROMNames[iprom])
			.set("value",diskPROMCodes[dduPROMNames[iprom]]) << std::endl;
	}

	*out << cgicc::fieldset()
		.set("class","normal") << std::endl;
	*out << cgicc::div("Step 2:  Select the slots you to which you intend to load firmware")
		.set("class","legend") << std::endl;
	
	emu::fed::DataTable slotTable("slotTable");

	slotTable.addColumn("Slot number");
	slotTable.addColumn("RUI");
	slotTable.addColumn("DDU board ID");
	for (unsigned int iprom = 0; iprom < dduPROMNames.size(); iprom++) {
		slotTable.addColumn(dduPROMNames[iprom]);
	}


	int iddu = -1;
	std::vector<emu::fed::DDU *> myDDUs = myCrate->getDDUs();
	for (std::vector< emu::fed::DDU * >::iterator iDDU = myDDUs.begin(); iDDU != myDDUs.end(); iDDU++) {
		if ((*iDDU)->slot() >= 21) continue;
		iddu++;
		std::ostringstream bitFlipCommand;
		bitFlipCommand << "Javascript:toggleBit('slots'," << (*iDDU)->slot() << ");";

		*(slotTable(iddu,0)->value) << cgicc::input()
			.set("type","checkbox")
			.set("class","slotBox")
			.set("onChange",bitFlipCommand.str()) << " " << (*iDDU)->slot();
		
		*(slotTable(iddu,1)->value) << myCrate->getRUI((*iDDU)->slot());

		*(slotTable(iddu,2)->value) << (*iDDU)->readFlashBoardID();

		for (unsigned int iprom = 0; iprom < dduPROMNames.size(); iprom++) {
			std::ostringstream promCode, fpgaCode;

			promCode << std::hex << (*iDDU)->readPROMUserCode(dduPROMTypes[dduPROMNames[iprom]]);
			*(slotTable(iddu,3 + iprom)->value) << promCode.str();

			if (dduPROMNames[iprom] != "VMEPROM") {
				fpgaCode << std::hex << (*iDDU)->readFPGAUserCode(dduFPGATypes[dduPROMNames[iprom]]);
			}

			// Check for consistency
			slotTable(iddu,3 + iprom)->setClass("ok");
			if (iprom == 1 || iprom == 2) {
				if (diskPROMCodes[dduPROMNames[iprom]].substr(0,6) != promCode.str().substr(0,6)) {
					slotTable(iddu,3 + iprom)->setClass("bad");
				} else if (fpgaCode.str().substr(3,2) != promCode.str().substr(2,2)) {
					slotTable(iddu,3 + iprom)->setClass("questionable");
				}
			} else if (diskPROMCodes[dduPROMNames[iprom]] != promCode.str()) {
				slotTable(iddu,3 + iprom)->setClass("bad");
			} else if (iprom == 3 || iprom == 4) {
				if (fpgaCode.str().substr(2,6) != promCode.str().substr(2,6)) {
					slotTable(iddu,3 + iprom)->setClass("bad");
				}
			}
		}
	}

	*out << slotTable.toHTML() << std::endl;

	*out << cgicc::input()
		.set("type","button")
		.set("value","Toggle Broadcast")
		.set("onClick","javascript:toggleBoxes('slotBox');")
		.set("style","margin-left: 20%; margin-right: auto;");

	// Tricky:  javascript toggle of check boxes
	*out << "<script type=\"text/javascript\">" << std::endl;
	*out << "function toggleBoxes(id){var x = getElementsByClassName(id); var f = document.getElementById('broadcast'); if(f.value == 0){f.value = 1; for (i=0,j=x.length;i<j;i++) {x[i].disabled=true;}} else {f.value = 0; for (i=0,j=x.length;i<j;i++) {x[i].disabled=false;}}}" << std::endl;
	*out << "</script>" << std::endl;

	*out << cgicc::div()
		.set("style","font-size: 8pt;") << std::endl;
	*out << "Legend: " << cgicc::span("All OK").set("class","ok") << " " << std::endl;
	*out << cgicc::span("Disk/PROM mismatch").set("class","bad") << " " << std::endl;
	*out << cgicc::span("PROM/FPGA mismatch").set("class","questionable") << cgicc::div() << std::endl;

	*out << cgicc::div("DO NOT BROADCAST UNLESS THE CONFIGURATION FILE CONTAINS ALL THE BOARDS PRESENT IN THE CRATE!")
		.set("style","font-size: 8pt;") << std::endl;

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



void EmuFCrateHyperDAQ::DDULoadBroadcast(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception)
{
	// PGK Patented check-for-initialization
	if (crateVector.size()==0) {
		LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
		return Default(in,out);
	}

	cgicc::Cgicc cgi(in);
	
	// First, I need a crate.
	cgicc::form_iterator name = cgi.getElement("crate");
	unsigned int cgiCrate = 0;
	if(name != cgi.getElements().end()) {
		cgiCrate = cgi["crate"]->getIntegerValue();
	}
	//Crate *myCrate = crateVector[cgiCrate];

	printf(" entered DDULoadBroadcast \n");

	std::string type = cgi["svftype"]->getValue();

	if (type != "VMEPROM" && type != "DDUPROM0" && type != "DDUPROM1" && type != "INPROM0" && type != "INPROM1") {
		std::cout << "I don't understand that PROM type (" << type << ")." << std::endl;

		std::ostringstream backLocation;
		backLocation << "DDUBroadcast?crate=" << cgiCrate;
		webRedirect(out,backLocation.str());
		//this->DDUBroadcast(in,out);
		//return;
	}

	cgicc::const_file_iterator ifile = cgi.getFile("File");
	if ( (*ifile).getFilename() == "" ) {
		std::cout << "The file you attempted to upload either doesn't exist, or wasn't properly transferred." << std::endl;
		
		std::ostringstream backLocation;
		backLocation << "DDUBroadcast?crate=" << cgiCrate;
		webRedirect(out,backLocation.str());
		//this->DDUBroadcast(in,out);
		//return;
	}

	std::string filename = "Current" + type + ".svf";
	std::ofstream outfile;
	outfile.open(filename.c_str(),std::ios::trunc);
	if (!outfile.is_open()) {
		std::cout << "I can't open the file stream for writing (" << filename << ")." << std::endl;

		std::ostringstream backLocation;
		backLocation << "DDUBroadcast?crate=" << cgiCrate;
		webRedirect(out,backLocation.str());
		//this->DDUBroadcast(in,out);
		//return;
	}

	(*ifile).writeToStream(outfile);
	outfile.close();

	std::cout << "downloaded and saved " << filename << " of type " << type << std::endl;
	
	std::ostringstream backLocation;
	backLocation << "DDUBroadcast?crate=" << cgiCrate;
	webRedirect(out,backLocation.str());
	//this->DDUBroadcast(in,out);

}



void EmuFCrateHyperDAQ::DDUSendBroadcast(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception)
{

	// PGK Patented check-for-initialization
	if (crateVector.size()==0) {
		LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
		return Default(in,out);
	}

	cgicc::Cgicc cgi(in);
	
	// First, I need a crate.
	cgicc::form_iterator name = cgi.getElement("crate");
	unsigned int cgiCrate = 0;
	if(name != cgi.getElements().end()) {
		cgiCrate = cgi["crate"]->getIntegerValue();
	}
	emu::fed::FEDCrate *myCrate = crateVector[cgiCrate];

	//printf(" entered DDUSendBroadcast \n");

	// unsigned int tidcode[8] = {0x2124a093,0x31266093,0x31266093,0x05036093,0x05036093,0x05036093,0x05036093,0x05036093};
	// unsigned int tuscode[8] = {0xcf043a02,0xdf025a02,0xdf025a02,0xb0020a04,0xc043dd99,0xc143dd99,0xd0025a02,0xd1025a02};

	int type = -1;
	std::string submitCommand = cgi["submit"]->getValue();
	if (submitCommand.substr(5) == "VMEPROM") type = 0;
	if (submitCommand.substr(5) == "DDUPROM") type = 1;
	if (submitCommand.substr(5) == "INPROM") type = 2;
	if (submitCommand.substr(5) == "VMEPROM (Emergency Load)") type = 3;
	if (submitCommand.substr(5) == "ALL") type = 4;

	int broadcast = cgi["broadcast"]->getIntegerValue();
	std::string slotsText = cgi["slots"]->getValue();
	if (slotsText.substr(0,2) == "0x") slotsText = slotsText.substr(2);
	unsigned int slots = 0;
	sscanf(slotsText.data(),"%4x",&slots);

	if (type == 3 && broadcast) {
		LOG4CPLUS_WARN(getApplicationLogger(),"Cannot broadcast VMEPROM firmware via emergency load.  Defaulting to individual board loading");
		slots = 0;
		broadcast = 0; // Can't broadcast emergency VMEPROM.
		// Loop through the DDUs and add them all to the list of slots to load.
		std::vector<emu::fed::DDU *> myDDUs = myCrate->getDDUs();
		for (std::vector<emu::fed::DDU *>::iterator iDDU = myDDUs.begin(); iDDU != myDDUs.end(); iDDU++) {
			if (!(*iDDU)->isBroadcast()) {
				slots |= (*iDDU)->slot();
			}
		}
	}

	// Error:  no slots to load.
	if (!broadcast && !slots) {
		LOG4CPLUS_ERROR(getApplicationLogger(),"No slots selected for firmware loading, and broadcast not set");
		std::ostringstream backLocation;
		backLocation << "DDUBroadcast?crate=" << cgiCrate;
		webRedirect(out,backLocation.str());
	}

	if (type != 0 && type != 1 && type != 2 && type != 3 && type != 5) {
		//std::cout << "I don't understand that PROM type (" << type << ")." << std::endl;
		
		std::ostringstream backLocation;
		backLocation << "DDUBroadcast?crate=" << cgiCrate;
		webRedirect(out,backLocation.str());
		//this->DDUBroadcast(in,out);
		//return;
	}

	std::string promName[6] = {"VMEPROM","DDUPROM1","DDUPROM0","INPROM1","INPROM0","RESET"};
	enum emu::fed::DEVTYPE devType[6] = {emu::fed::VMEPROM,emu::fed::DDUPROM1,emu::fed::DDUPROM0,emu::fed::INPROM1,emu::fed::INPROM0,emu::fed::RESET};

	// Load the proper version types from the cgi handle.
	std::string version[5];
	for (int i=0; i<=4; i++) {
		version[i] = cgi[promName[i].c_str()]->getValue();
		//std::cout << " version on disk: " << promName[i] << " " << version[i] << std::endl;
	}

	int from, to;
	if (type == 0) { from = 0; to = 0; }
	if (type == 1) { from = 1; to = 2; }
	if (type == 2) { from = 3; to = 4; }
	if (type == 3) { from = 5; to = 5; }
	if (type == 4) { from = 0; to = 4; }

	//std::cout << " From type " << type << ", broadcasting ";
	//for (int i=from; i<=to; i++) {
	//	std::cout << promName[i] << " ";
	//}
	//std::cout << std::endl;

	// I thought this was not allowed!
	int bnstore[myCrate->getDDUs().size()];
	for (unsigned int iBoard=0; iBoard<myCrate->getDDUs().size(); iBoard++) {
		if (!broadcast && !(slots & (1 << myCrate->getDDUs()[iBoard]->slot()))) continue;
		else if (broadcast && !myCrate->getDDUs()[iBoard]->isBroadcast()) continue;

		emu::fed::DDU *myDDU = myCrate->getDDUs()[iBoard];
		//std::cout << "Sending to slot " << myDDU->slot() << std::endl;

		for (int i=from; i<=to; i++) {
			LOG4CPLUS_DEBUG(getApplicationLogger(),"Loading firmware to prom " << (i+1) << " of " << (to - from + 1) << "...");
			
			std::string filename = "Current" + promName[i] + ".svf";
			//std::cout << " broadcasting " << filename << " version " << version[i] << std::endl;
			LOG4CPLUS_INFO(getApplicationLogger(),"Loading server file " << filename << " (v " << std::hex << version[i] << std::dec << ") to DDU slot " << myDDU->slot() << "...");

			char *boardnumber = (char *) malloc(5);
			boardnumber[0]=0x00;boardnumber[1]=0x00;boardnumber[2]=0x00;boardnumber[3]=0x00;

			if (i == 1 || i == 2) {
				boardnumber[0] = 1;

				LOG4CPLUS_DEBUG(getApplicationLogger(),"Step 1 of 3: Load firmware up to boardID...");

				myDDU->epromload((char *)promName[i].c_str(),devType[i],(char *)filename.c_str(),1,boardnumber,1);

				if (broadcast) {
					for (unsigned int ddu=0; ddu<myCrate->getDDUs().size(); ddu++) {
						emu::fed::DDU *secondaryDDU = myCrate->getDDUs()[ddu];
						if (secondaryDDU->isBroadcast()) continue;
						int ibn = secondaryDDU->readFlashBoardID();
						bnstore[ddu] = ibn;
						boardnumber[0] = ibn;
						//std::cout << " pause to upload boardnumber " << ibn << std::endl;
						LOG4CPLUS_DEBUG(getApplicationLogger(),"Step 2 of 3: Load boardID " << std::dec << ibn << " to slot " << secondaryDDU->slot() << "...");
						secondaryDDU->epromload((char *)promName[i].c_str(),devType[i],(char *)filename.c_str(),1,boardnumber,2);
					}
				} else {
					int ibn = myDDU->readFlashBoardID();
					boardnumber[0] = ibn;
					LOG4CPLUS_DEBUG(getApplicationLogger(),"Step 2 of 3: Load boardID " << std::dec << ibn << " to slot " << myDDU->slot() << "...");
					myDDU->epromload((char *)promName[i].c_str(),devType[i],(char *)filename.c_str(),1,boardnumber,2);
				}

				LOG4CPLUS_DEBUG(getApplicationLogger(),"Step 3 of 3: Load remaining firmware data...");
				boardnumber[0] = 1;
				myDDU->epromload((char *)promName[i].c_str(),devType[i],(char *)filename.c_str(),1,boardnumber,3);

			} else {
				LOG4CPLUS_DEBUG(getApplicationLogger(),"Step 1 of 1: Load all firmware data...");
				myDDU->epromload((char *)promName[i].c_str(),devType[i],(char *)filename.c_str(),1,boardnumber);
			}
			free(boardnumber);
			LOG4CPLUS_INFO(getApplicationLogger(),"Loading of server file " << filename << " (v " << std::hex << version[i] << std::dec << ") to DDU slot " << myDDU->slot() << " complete.");

		}

	}
	LOG4CPLUS_INFO(getApplicationLogger(),"All firmware loading operations complete, checking PROM ids...");

	for (int i=from; i<=to; i++) { // loop over PROMS
		for (unsigned int ddu=0; ddu<myCrate->getDDUs().size(); ddu++) { // loop over boards
			if (!broadcast && !(slots & (1 << myCrate->getDDUs()[ddu]->slot()))) continue;
			else if (broadcast && myCrate->getDDUs()[ddu]->isBroadcast()) continue;
			emu::fed::DDU *myDDU = myCrate->getDDUs()[ddu];

			std::string boardversion;
			std::string checkversion;
			if (i==0 || i==5) {
				boardversion = myDDU->readPROMUserCode(emu::fed::VMEPROM);
				checkversion = version[i];
			} else if (i==1) {
				boardversion = myDDU->readPROMUserCode(emu::fed::DDUPROM0);
				checkversion = version[i].substr(0,6);
				checkversion += (bnstore[i] & 0xff);
			} else if (i==2) {
				boardversion = myDDU->readPROMUserCode(emu::fed::DDUPROM1);
				checkversion = version[i].substr(0,6);
				checkversion += (bnstore[i] & 0xff);
			} else if (i==3) {
				boardversion = myDDU->readPROMUserCode(emu::fed::INPROM0);
				checkversion = version[i];
			} else if (i==4) {
				boardversion = myDDU->readPROMUserCode(emu::fed::INPROM1);
				checkversion = version[i];
			}

			if (checkversion == boardversion) {
				LOG4CPLUS_INFO(getApplicationLogger(),"DDU slot " << myDDU->slot() << " PROM " << promName[i] << " versions match ("<< std::hex << boardversion << std::dec <<")");
			} else {
				LOG4CPLUS_ERROR(getApplicationLogger(),"DDU slot " << myDDU->slot() << " PROM " << promName[i] << " version mismatch (shows "<< std::hex << boardversion << std::dec << ", should be "<< std::hex << checkversion << std::dec << ")");
			}
		}
	}

	std::ostringstream backLocation;
	backLocation << "DDUBroadcast?crate=" << cgiCrate;
	webRedirect(out,backLocation.str());
	//this->DDUBroadcast(in,out);

}



void EmuFCrateHyperDAQ::DDUReset(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception)
{

	// PGK Patented check-for-initialization
	if (crateVector.size()==0) {
		LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
		return Default(in,out);
	}

	cgicc::Cgicc cgi(in);
	
	// First, I need a crate.
	cgicc::form_iterator name = cgi.getElement("crate");
	unsigned int cgiCrate = 0;
	if(name != cgi.getElements().end()) {
		cgiCrate = cgi["crate"]->getIntegerValue();
	}
	emu::fed::FEDCrate *myCrate = crateVector[cgiCrate];

	myCrate->getDCCs()[0]->crateHardReset();

	std::ostringstream backLocation;
	backLocation << "DDUBroadcast?crate=" << cgiCrate;
	webRedirect(out,backLocation.str());

}


/// @note There should be a more expert function that handles these sorts of things.
/// @Deprecated User DDUExpert functions instead
/*
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
		// This is a better way to reload the main page, I think.
		//std::ostringstream backLocation;
		//backLocation << "mainPage?crate=" << cgiCrate;
		//webRedirect(out,backLocation.str());
		webRedirect(out,"mainPage");
		//mainPage(in,out);

	} catch (const std::exception & e ) {
		printf(" exception raised in DDUBrcstFED \n");
		//XECPT_RAISE(xgi::exception::Exception, e.what());
	}

}
*/

/// @note Try to merge this with the broadcast method.
/// @Deprecated User DDUFirmware functions instead
/*
void EmuFCrateHyperDAQ::DDULoadFirmware(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{

	cgicc::Cgicc cgi(in);

	cgicc::form_iterator name = cgi.getElement("ddu");

	int ddu = 0;
	if(name != cgi.getElements().end()) {
		ddu = cgi["ddu"]->getIntegerValue();
		std::cout << "DDU " << ddu << std::endl;
	}
	int prom;
	cgicc::form_iterator name2 = cgi.getElement("prom");
	//
	if(name2 != cgi.getElements().end()) {
		prom = cgi["prom"]->getIntegerValue();
		if(prom==13)prom=9;   // JG, special Broadcast case for VMEPROM...
		if(prom>=10)prom-=10; //  ...now remove the "10+" flag for all cases.
		std::cout << "PROM " << prom << std::endl;
	}
	//
	thisDDU = dduVector[ddu];
	//
	//
	std::cout << "UploadConfFileUpload" << std::endl;
	//


	cgicc::const_file_iterator file = cgi.getFile("DDULoadSVF");
	//std::string XMLname = (*file).getFilename();
	// std::cout <<"SVF FILE: " << XMLname  << std::endl ;


	//
	std::cout << "GetFiles" << std::endl ;
	//

	if(file != cgi.getFiles().end()) {
		std::ofstream TextFile ;
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
	//if(prom==3)thisDDU->epromload("RESET",RESET,"MySVFFile.svf",1,cbrdnum);
	// DEBUG What happens here?
	if(prom==3)thisDDU->epromload("VMEPROM",VMEPROM,"MySVFFile.svf",1,cbrdnum);
	if(prom==9)thisDDU->epromload("VMEPROM",VMEPROM,"MySVFFile.svf",1,cbrdnum);
	if(prom==4||prom==5){
		int brdnum=thisDDU->read_page3();
		cbrdnum[0]=brdnum;
	}
	if(prom==4)thisDDU->epromload("DDUPROM0",DDUPROM0,"MySVFFile.svf",1,cbrdnum);
	if(prom==5)thisDDU->epromload("DDUPROM1",DDUPROM1,"MySVFFile.svf",1,cbrdnum);
	free(cbrdnum);
	in=NULL;
	//std::ostringstream backLocation;
	//backLocation << "DDUFirmware?crate=" << cgiCrate << "&ddu=" << cgiDDU;
	//webRedirect(out,backLocation.str());
	this->DDUFirmware(in,out);
	// this->Default(in,out);
}
*/


void EmuFCrateHyperDAQ::DDUDebug(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	// PGK Patented check-for-initialization
	if (crateVector.size()==0) {
		LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
		return Default(in,out);
	}

	cgicc::Cgicc cgi(in);
	
	// First, I need a crate.
	cgicc::form_iterator name = cgi.getElement("crate");
	unsigned int cgiCrate = 0;
	if(name != cgi.getElements().end()) {
		cgiCrate = cgi["crate"]->getIntegerValue();
	}
	emu::fed::FEDCrate *myCrate = crateVector[cgiCrate];

	name = cgi.getElement("ddu");
	unsigned int cgiDDU = 0;
	if (name != cgi.getElements().end()) {
		cgiDDU = cgi["ddu"]->getIntegerValue();
		//std::cout << "DDU inside " << ddu << std::endl;
	}
	emu::fed::DDU *myDDU = myCrate->getDDUs()[cgiDDU];

	std::stringstream sTitle;
	sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DDUFPGA Controls (RUI #" << myCrate->getRUI(myDDU->slot()) << ")";
	*out << Header(sTitle.str(),false);

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
	unsigned int fibersWithErrors = myDDU->readCSCStatAdvanced() | myDDU->readAdvancedFiberErrors();

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

	emu::fed::DataTable generalTable("generalTable");

	generalTable.addColumn("Register");
	generalTable.addColumn("Value");
	generalTable.addColumn("Decoded Status");

	*(generalTable(0,0)->value) << "DDU RUI (16-bit)";
	unsigned long int dduValue = myDDU->readRUI();
	*(generalTable(0,1)->value) << dduValue;
	generalTable(0,1)->setClass("none");

	*(generalTable(1,0)->value) << "DDU L1 scaler";
	// PGK gives flakey values.
	//dduValue = myDDU->readL1Scaler(DDUFPGA);
	dduValue = myDDU->ddu_rdscaler();
	*(generalTable(1,1)->value) << dduValue;
	generalTable(1,1)->setClass("none");
	
	*(generalTable(2,0)->value) << "DDU control FPGA status (32-bit)";
	dduValue = myDDU->readFPGAStat(emu::fed::DDUFPGA);
	*(generalTable(2,1)->value) << std::showbase << std::hex << dduValue;
	if (dduValue & 0x00008000) {
		generalTable(2,1)->setClass("bad");
		debugTrapValid = true;
	}
	else if (dduValue & 0xDE4F4BFF) generalTable(2,1)->setClass("warning");
	else generalTable(2,1)->setClass("ok");
	std::map<std::string, std::string> dduComments = emu::fed::DDUDebugger::DDUFPGAStat(dduValue);
	for (std::map<std::string,std::string>::iterator iComment = dduComments.begin();
		iComment != dduComments.end();
		iComment++) {
		*(generalTable(2,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}

	*(generalTable(3,0)->value) << "DDU output status (16-bit)";
	dduValue = myDDU->readOutputStat();
	*(generalTable(3,1)->value) << std::showbase << std::hex << dduValue;
	if (dduValue & 0x00000080) generalTable(3,1)->setClass("bad");
	else if (dduValue & 0x00004000) generalTable(3,1)->setClass("warning");
	else generalTable(3,1)->setClass("ok");
	dduComments = emu::fed::DDUDebugger::OutputStat(dduValue);
	for (std::map<std::string,std::string>::iterator iComment = dduComments.begin();
		iComment != dduComments.end();
		iComment++) {
		*(generalTable(3,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}

	*(generalTable(4,0)->value) << "Error bus A register bits (16-bit)";
	dduValue = myDDU->readEBReg(1);
	*(generalTable(4,1)->value) << std::showbase << std::hex << dduValue;
	if (dduValue & 0x0000C00C) generalTable(4,1)->setClass("bad");
	else if (dduValue & 0x000001C8) generalTable(4,1)->setClass("warning");
	else generalTable(4,1)->setClass("ok");
	dduComments = emu::fed::DDUDebugger::EBReg1(dduValue);
	for (std::map<std::string,std::string>::iterator iComment = dduComments.begin();
		iComment != dduComments.end();
		iComment++) {
		*(generalTable(4,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}

	*(generalTable(5,0)->value) << "Error bus B register bits (16-bit)";
	dduValue = myDDU->readEBReg(2);
	*(generalTable(5,1)->value) << std::showbase << std::hex << dduValue;
	if (dduValue & 0x00000011) generalTable(5,1)->setClass("bad");
	else if (dduValue & 0x0000D08E) generalTable(5,1)->setClass("warning");
	else generalTable(5,1)->setClass("ok");
	dduComments = emu::fed::DDUDebugger::EBReg2(dduValue);
	for (std::map<std::string,std::string>::iterator iComment = dduComments.begin();
		iComment != dduComments.end();
		iComment++) {
		*(generalTable(5,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}

	*(generalTable(6,0)->value) << "Error bus C register bits (16-bit)";
	dduValue = myDDU->readEBReg(3);
	*(generalTable(6,1)->value) << std::showbase << std::hex << dduValue;
	if (dduValue & 0x0000BFBF) generalTable(6,1)->setClass("warning");
	else generalTable(6,1)->setClass("ok");
	dduComments = emu::fed::DDUDebugger::EBReg3(dduValue);
	for (std::map<std::string,std::string>::iterator iComment = dduComments.begin();
		iComment != dduComments.end();
		iComment++) {
		*(generalTable(6,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}

	*out << generalTable.printSummary() << std::endl;

	// Display only if there are errors.
	if (generalTable.countClass("ok") + generalTable.countClass("none") == generalTable.countRows()) generalTable.setHidden(true);
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

	emu::fed::DataTable otherTable("otherTable");

	otherTable.addColumn("Register");
	otherTable.addColumn("Value");
	otherTable.addColumn("Decoded Status");

	*(otherTable(0,0)->value) << "DDU near full warning (8-bit)";
	dduValue = myDDU->readWarnMon();
	*(otherTable(0,1)->value) << std::showbase << std::hex << ((dduValue) & 0xFF);
	if ((dduValue) & 0xFF) otherTable(0,1)->setClass("questionable");
	else otherTable(0,1)->setClass("ok");
	dduComments = emu::fed::DDUDebugger::WarnMon((dduValue) & 0xFF);
	for (std::map<std::string,std::string>::iterator iComment = dduComments.begin();
		iComment != dduComments.end();
		iComment++) {
		*(otherTable(0,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}

	*(otherTable(1,0)->value) << "DDU near full historical (8-bit)";
	//dduValue = myDDU->readWarnMon();
	*(otherTable(1,1)->value) << std::showbase << std::hex << ((dduValue >> 8) & 0xFF);
	if ((dduValue >> 8) & 0xFF) otherTable(1,1)->setClass("questionable");
	else otherTable(1,1)->setClass("ok");
	dduComments = emu::fed::DDUDebugger::WarnMon((dduValue >> 8) & 0xFF);
	for (std::map<std::string,std::string>::iterator iComment = dduComments.begin();
		iComment != dduComments.end();
		iComment++) {
		*(otherTable(1,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}

	*(otherTable(2,0)->value) << "DDU L1A-to-start max process time";
	dduValue = myDDU->readMaxTimeoutCount();
	*(otherTable(2,1)->value) << (((dduValue) & 0xFF) * 400.0) << " ns";
	otherTable(2,1)->setClass("none");

	*(otherTable(3,0)->value) << "DDU start-to-end max process time";
	//dduValue = myDDU->readWarnMon();
	*(otherTable(3,1)->value) << std::showbase << std::hex << (((dduValue >> 8) & 0xFF) * 6.4) << " &mu;s";
	otherTable(3,1)->setClass("none");

	*out << otherTable.printSummary() << std::endl;

	// Display only if there are errors.
	if (otherTable.countClass("ok") + otherTable.countClass("none") == otherTable.countRows()) otherTable.setHidden(true);
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


	emu::fed::DataTable fiberTable("fiberTable");

	fiberTable.addColumn("Register");
	fiberTable.addColumn("Value");
	fiberTable.addColumn("Decoded Chambers");

	*(fiberTable(0,0)->value) << "First event DMBLIVE";
	dduValue = myDDU->readPermDMBLive();
	*(fiberTable(0,1)->value) << std::showbase << std::hex << dduValue;
	fiberTable(0,1)->setClass("none");
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (dduValue & (1<<iFiber)) {
			*(fiberTable(0,2)->value) << cgicc::div(myDDU->getChamber(iFiber)->name())
				.set("class","none");
		}
	}

	*(fiberTable(1,0)->value) << "Latest event DMBLIVE";
	dduValue = myDDU->readDMBLive();
	*(fiberTable(1,1)->value) << std::showbase << std::hex << dduValue;
	fiberTable(1,1)->setClass("none");
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (dduValue & (1<<iFiber)) {
			*(fiberTable(1,2)->value) << cgicc::div(myDDU->getChamber(iFiber)->name())
				.set("class","none");
		}
	}

	*(fiberTable(2,0)->value) << "CRC error";
	dduValue = myDDU->readCRCError();
	*(fiberTable(2,1)->value) << std::showbase << std::hex << dduValue;
	if (dduValue) fiberTable(2,1)->setClass("bad");
	else fiberTable(2,1)->setClass("ok");
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (dduValue & (1<<iFiber)) {
			*(fiberTable(2,2)->value) << cgicc::div(myDDU->getChamber(iFiber)->name())
				.set("class","red");
			//fibersWithErrors |= 1<<iFiber;
		}
	}

	*(fiberTable(3,0)->value) << "Data transmit error";
	dduValue = myDDU->readXmitError();
	*(fiberTable(3,1)->value) << std::showbase << std::hex << dduValue;
	if (dduValue) fiberTable(3,1)->setClass("bad");
	else fiberTable(3,1)->setClass("ok");
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (dduValue & (1<<iFiber)) {
			*(fiberTable(3,2)->value) << cgicc::div(myDDU->getChamber(iFiber)->name())
				.set("class","red");
			//fibersWithErrors |= 1<<iFiber;
		}
	}

	*(fiberTable(4,0)->value) << "DMB error";
	dduValue = myDDU->readDMBError();
	*(fiberTable(4,1)->value) << std::showbase << std::hex << dduValue;
	if (dduValue) fiberTable(4,1)->setClass("bad");
	else fiberTable(4,1)->setClass("ok");
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (dduValue & (1<<iFiber)) {
			*(fiberTable(4,2)->value) << cgicc::div(myDDU->getChamber(iFiber)->name())
				.set("class","red");
			//fibersWithErrors |= 1<<iFiber;
		}
	}

	*(fiberTable(5,0)->value) << "TMB error";
	dduValue = myDDU->readTMBError();
	*(fiberTable(5,1)->value) << std::showbase << std::hex << dduValue;
	if (dduValue) fiberTable(5,1)->setClass("bad");
	else fiberTable(5,1)->setClass("ok");
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (dduValue & (1<<iFiber)) {
			*(fiberTable(5,2)->value) << cgicc::div(myDDU->getChamber(iFiber)->name())
				.set("class","red");
			//fibersWithErrors |= 1<<iFiber;
		}
	}

	*(fiberTable(6,0)->value) << "ALCT error";
	dduValue = myDDU->readALCTError();
	*(fiberTable(6,1)->value) << std::showbase << std::hex << dduValue;
	if (dduValue) fiberTable(6,1)->setClass("bad");
	else fiberTable(6,1)->setClass("ok");
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (dduValue & (1<<iFiber)) {
			*(fiberTable(6,2)->value) << cgicc::div(myDDU->getChamber(iFiber)->name())
				.set("class","red");
			//fibersWithErrors |= 1<<iFiber;
		}
	}

	*(fiberTable(7,0)->value) << "Lost-in-event error";
	dduValue = myDDU->readLIEError();
	*(fiberTable(7,1)->value) << std::showbase << std::hex << dduValue;
	if (dduValue) fiberTable(7,1)->setClass("bad");
	else fiberTable(7,1)->setClass("ok");
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (dduValue & (1<<iFiber)) {
			*(fiberTable(7,2)->value) << cgicc::div(myDDU->getChamber(iFiber)->name())
				.set("class","red");
			//fibersWithErrors |= 1<<iFiber;
		}
	}

	*out << fiberTable.printSummary() << std::endl;

	// Display only if there are errors.
	if (fiberTable.countClass("ok") + fiberTable.countClass("none") == fiberTable.countRows()) fiberTable.setHidden(true);
	*out << fiberTable.toHTML() << std::endl;

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


	emu::fed::DataTable inrdTable("inrdTable");

	inrdTable.addColumn("Register");
	inrdTable.addColumn("Value");
	inrdTable.addColumn("Decoded Chambers");

	*(inrdTable(0,0)->value) << "Stuck data error";
	dduValue = myDDU->checkFIFO(1);
	*(inrdTable(0,1)->value) << std::showbase << std::hex << ((dduValue >> 12) & 0xF);
	if ((dduValue >> 12) & 0xF) inrdTable(0,1)->setClass("bad");
	else inrdTable(0,1)->setClass("ok");

	*(inrdTable(1,0)->value) << "Fiber or FIFO connection error";
	//dduValue = myDDU->checkFIFO(1);
	*(inrdTable(1,1)->value) << std::showbase << std::hex << ((dduValue >> 8) & 0xF);
	if ((dduValue >> 8) & 0xF) inrdTable(1,1)->setClass("bad");
	else inrdTable(1,1)->setClass("ok");

	*(inrdTable(2,0)->value) << "L1A mismatch";
	//dduValue = myDDU->checkFIFO(1);
	*(inrdTable(2,1)->value) << std::showbase << std::hex << ((dduValue >> 4) & 0xF);
	if ((dduValue >> 4) & 0xF) inrdTable(2,1)->setClass("warning");
	else inrdTable(2,1)->setClass("ok");

	*(inrdTable(3,0)->value) << "InRD with active fiber";
	//dduValue = myDDU->checkFIFO(1);
	*(inrdTable(3,1)->value) << std::showbase << std::hex << ((dduValue >> 4) & 0xF);
	inrdTable(3,1)->setClass("none");

	*(inrdTable(4,0)->value) << "Active ext. FIFO empty";
	dduValue = myDDU->checkFIFO(2);
	*(inrdTable(4,1)->value) << std::showbase << std::hex << ((dduValue >> 10) & 0xF);
	inrdTable(4,1)->setClass("none");

	*(inrdTable(5,0)->value) << "InRD near full warning";
	//dduValue = myDDU->checkFIFO(2);
	*(inrdTable(5,1)->value) << std::showbase << std::hex << ((dduValue >> 4) & 0xF);
	if ((dduValue >> 4) & 0xF) inrdTable(5,1)->setClass("warning");
	else inrdTable(5,1)->setClass("ok");

	*(inrdTable(6,0)->value) << "Ext. FIFO almost-full";
	//dduValue = myDDU->checkFIFO(2);
	*(inrdTable(6,1)->value) << std::showbase << std::hex << ((dduValue) & 0xF);
	if ((dduValue) & 0xF) inrdTable(6,1)->setClass("questionable");
	else inrdTable(6,1)->setClass("ok");

	*(inrdTable(7,0)->value) << "Special decode bits";
	//dduValue = myDDU->checkFIFO(2);
	*(inrdTable(7,1)->value) << std::showbase << std::hex << ((dduValue >> 8) & 0x43);
	if ((dduValue >> 8) & 0x81) inrdTable(7,1)->setClass("warning");
	else inrdTable(7,1)->setClass("ok");
	dduComments = emu::fed::DDUDebugger::FIFO2((dduValue >> 8) & 0x43);
	for (std::map<std::string,std::string>::iterator iComment = dduComments.begin();
		iComment != dduComments.end();
		iComment++) {
		*(inrdTable(7,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}

	*(inrdTable(8,0)->value) << "Timeout-EndBusy";
	dduValue = myDDU->checkFIFO(3);
	*(inrdTable(8,1)->value) << std::showbase << std::hex << ((dduValue >> 12) & 0xF);
	if ((dduValue >> 12) & 0xF) inrdTable(8,1)->setClass("bad");
	else inrdTable(8,1)->setClass("ok");

	*(inrdTable(9,0)->value) << "Timeout-EndWait";
	//dduValue = myDDU->checkFIFO(2);
	*(inrdTable(9,1)->value) << std::showbase << std::hex << ((dduValue >> 8) & 0xF);
	if ((dduValue >> 8) & 0xF) inrdTable(9,1)->setClass("warning");
	else inrdTable(9,1)->setClass("ok");

	*(inrdTable(10,0)->value) << "Timeout-Start";
	//dduValue = myDDU->checkFIFO(2);
	*(inrdTable(10,1)->value) << std::showbase << std::hex << ((dduValue >> 4) & 0xF);
	if ((dduValue >> 4) & 0xF) inrdTable(10,1)->setClass("warning");
	else inrdTable(10,1)->setClass("ok");

	*(inrdTable(11,0)->value) << "Lost-in-data error";
	//dduValue = myDDU->checkFIFO(2);
	*(inrdTable(11,1)->value) << std::showbase << std::hex << ((dduValue) & 0xF);
	if ((dduValue) & 0xF) inrdTable(11,1)->setClass("bad");
	else inrdTable(11,1)->setClass("ok");

	*(inrdTable(12,0)->value) << "Raw ext. FIFO empty";
	dduValue = myDDU->readFFError();
	*(inrdTable(12,1)->value) << std::showbase << std::hex << ((dduValue >> 10) & 0xF);
	inrdTable(12,1)->setClass("none");

	*(inrdTable(13,0)->value) << "InRD FIFO full";
	//dduValue = myDDU->readFFError(2);
	*(inrdTable(13,1)->value) << std::showbase << std::hex << ((dduValue >> 4) & 0xF);
	if ((dduValue >> 4) & 0xF) inrdTable(13,1)->setClass("bad");
	else inrdTable(13,1)->setClass("ok");

	*(inrdTable(14,0)->value) << "Ext. FIFO full";
	//dduValue = myDDU->readFFError(2);
	*(inrdTable(14,1)->value) << std::showbase << std::hex << ((dduValue) & 0xF);
	if ((dduValue) & 0xF) inrdTable(14,1)->setClass("bad");
	else inrdTable(14,1)->setClass("ok");

	*(inrdTable(15,0)->value) << "Special decode bits";
	//dduValue = myDDU->readFFError(2);
	*(inrdTable(15,1)->value) << std::showbase << std::hex << ((dduValue >> 8) & 0x43);
	if ((dduValue >> 8) & 0x1) inrdTable(15,1)->setClass("bad");
	else inrdTable(15,1)->setClass("ok");
	dduComments = emu::fed::DDUDebugger::FFError((dduValue >> 8) & 0x43);
	for (std::map<std::string,std::string>::iterator iComment = dduComments.begin();
		iComment != dduComments.end();
		iComment++) {
		*(inrdTable(15,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}

	*(inrdTable(16,0)->value) << "InRD hard error";
	dduValue = myDDU->readInRDStat();
	*(inrdTable(16,1)->value) << std::showbase << std::hex << ((dduValue >> 12) & 0xF);
	if ((dduValue >> 12) & 0xF) inrdTable(16,1)->setClass("bad");
	else inrdTable(16,1)->setClass("ok");

	*(inrdTable(17,0)->value) << "InRD sync error";
	//dduValue = myDDU->checkFIFO(2);
	*(inrdTable(17,1)->value) << std::showbase << std::hex << ((dduValue >> 8) & 0xF);
	if ((dduValue >> 8) & 0xF) inrdTable(17,1)->setClass("warning");
	else inrdTable(17,1)->setClass("ok");

	*(inrdTable(18,0)->value) << "InRD single event error";
	//dduValue = myDDU->checkFIFO(2);
	*(inrdTable(18,1)->value) << std::showbase << std::hex << ((dduValue >> 4) & 0xF);
	if ((dduValue >> 4) & 0xF) inrdTable(18,1)->setClass("questionable");
	else inrdTable(18,1)->setClass("ok");

	*(inrdTable(19,0)->value) << "InRD timeout error";
	//dduValue = myDDU->checkFIFO(2);
	*(inrdTable(19,1)->value) << std::showbase << std::hex << ((dduValue) & 0xF);
	if ((dduValue) & 0xF) inrdTable(19,1)->setClass("bad");
	else inrdTable(19,1)->setClass("ok");

	*(inrdTable(20,0)->value) << "InRD multiple transmit errors";
	dduValue = myDDU->ddu_InC_Hist();
	*(inrdTable(20,1)->value) << std::showbase << std::hex << ((dduValue) & 0xF);
	if ((dduValue) & 0xF) inrdTable(20,1)->setClass("bad");
	else inrdTable(20,1)->setClass("ok");

	*(inrdTable(21,0)->value) << "Special decode bits";
	//dduValue = myDDU->readFFError(2);
	*(inrdTable(21,1)->value) << std::showbase << std::hex << ((dduValue) & 0xFFF);
	if ((dduValue) & 0xC00) inrdTable(21,1)->setClass("bad");
	else if ((dduValue) & 0x2DF) inrdTable(21,1)->setClass("warning");
	else inrdTable(21,1)->setClass("ok");
	dduComments = emu::fed::DDUDebugger::FFError((dduValue) & 0xFFF);
	for (std::map<std::string,std::string>::iterator iComment = dduComments.begin();
		iComment != dduComments.end();
		iComment++) {
		*(inrdTable(21,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}

	*out << inrdTable.printSummary() << std::endl;

	// Display only if there are errors.
	if (inrdTable.countClass("ok") + inrdTable.countClass("none") == inrdTable.countRows()) inrdTable.setHidden(true);
	*out << inrdTable.toHTML() << std::endl;

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
		std::vector<unsigned long int> lcode = myDDU->ddu_fpgatrap();
		std::vector<std::string> bigComments = emu::fed::DDUDebugger::ddu_fpgatrap(lcode, myDDU);

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
	unsigned long int scalar = myDDU->ddu_rdscaler();

	// Make us a DataTable!
	emu::fed::DataTable occuTable("occuTable");

	occuTable.addColumn("Fiber Input");
	occuTable.addColumn("Chamber");
	occuTable.addBreak();
	occuTable.addColumn("DMB<sup>*</sup>");
	occuTable.addColumn("ALCT");
	occuTable.addColumn("TMB");
	occuTable.addColumn("CFEB");

	// It's good to know the fibers that are alive and the fibers that are killed.
	long int liveFibers = (myDDU->checkFiber(emu::fed::INFPGA0)&0x000000ff) | ((myDDU->checkFiber(emu::fed::INFPGA1)&0x000000ff)<<8);
	long int killFiber = myDDU->ddu_rdkillfiber();

	// PGK It turns out that this is an automatically shifting register.
	//  If you read from it 60 times, you get 15 sets of 4 values, all
	//  different.
	//  This means I need to do this 15 times per board.
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		std::vector<unsigned long int> lcode = myDDU->ddu_occmon();
		unsigned long int DMBval = lcode[0] & 0x0fffffff;
		unsigned long int ALCTval = lcode[1] & 0x0fffffff;
		unsigned long int TMBval = lcode[2] & 0x0fffffff;
		unsigned long int CFEBval = lcode[3] & 0x0fffffff;

		(*occuTable(iFiber,0)->value) << iFiber;
		emu::fed::Chamber *thisChamber = myDDU->getChamber(iFiber);

		std::string chamberClass = "ok";

		if (thisChamber != NULL) {
			(*occuTable(iFiber,1)->value) << thisChamber->name();
			if (!(killFiber & (1<<iFiber))) chamberClass = "none";
			else if (!(liveFibers & (1<<iFiber))) chamberClass = "undefined";
			else if (fibersWithErrors & (1<<iFiber)) chamberClass = "bad";

			occuTable(iFiber,1)->setClass(chamberClass);

			(*occuTable(iFiber,2)->value) << DMBval;
			(*occuTable(iFiber,2)->value) << "<br />" << std::setprecision(3) << (scalar ? DMBval*100./scalar : 0) << "%";

			(*occuTable(iFiber,3)->value) << ALCTval;
			(*occuTable(iFiber,3)->value) << "<br />" << std::setprecision(3) << (DMBval ? ALCTval*100./DMBval : 0) << "%";

			(*occuTable(iFiber,4)->value) << TMBval;
			(*occuTable(iFiber,4)->value) << "<br />" << std::setprecision(3) << (DMBval ? TMBval*100./DMBval : 0) << "%";

			(*occuTable(iFiber,5)->value) << CFEBval;
			(*occuTable(iFiber,5)->value) << "<br />" << std::setprecision(3) << (DMBval ? CFEBval*100./DMBval : 0) << "%";

		} else {
			(*occuTable(iFiber,1)->value) << "???";
			occuTable(iFiber,1)->setClass("undefined");
			(*occuTable(iFiber,2)->value) << "N/A";
			(*occuTable(iFiber,3)->value) << "N/A";
			(*occuTable(iFiber,4)->value) << "N/A";
			(*occuTable(iFiber,5)->value) << "N/A";
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

}



void EmuFCrateHyperDAQ::InFpga(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{

	// PGK Patented check-for-initialization
	if (crateVector.size()==0) {
		LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
		return Default(in,out);
	}

	cgicc::Cgicc cgi(in);
	
	// First, I need a crate.
	cgicc::form_iterator name = cgi.getElement("crate");
	unsigned int cgiCrate = 0;
	if(name != cgi.getElements().end()) {
		cgiCrate = cgi["crate"]->getIntegerValue();
	}
	emu::fed::FEDCrate *myCrate = crateVector[cgiCrate];

	name = cgi.getElement("ddu");
	unsigned int cgiDDU = 0;
	if (name != cgi.getElements().end()) {
		cgiDDU = cgi["ddu"]->getIntegerValue();
		//std::cout << "DDU inside " << ddu << std::endl;
	}
	emu::fed::DDU *myDDU = myCrate->getDDUs()[cgiDDU];

	std::stringstream sTitle;
	sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DDU InFPGA Controls (RUI #" << myCrate->getRUI(myDDU->slot()) << ")";
	*out << Header(sTitle.str(),false);

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
	enum emu::fed::DEVTYPE devTypes[2] = {
		emu::fed::INFPGA0,
		emu::fed::INFPGA1
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

	emu::fed::DataTable generalTable("generalTable");

	// Names have to come first, because there is only one of each.
	generalTable.addColumn("Register");
	generalTable.addColumn("Value");
	generalTable.addColumn("Decoded Status");
	generalTable.addBreak();
	generalTable.addColumn("Value");
	generalTable.addColumn("Decoded Status");

	for (unsigned int iDevType = 0; iDevType < 2; iDevType++) {
		enum emu::fed::DEVTYPE dt = devTypes[iDevType];

		if (iDevType == 0) {
			*(generalTable(0,0)->value) << "InFPGA status (32-bit)";
			*(generalTable(1,0)->value) << "L1 Event Scaler0/2 (24-bit)";
			*(generalTable(2,0)->value) << "L1 Event Scaler1/3 (24-bit)";
		}

		unsigned long int infpgastat = myDDU->infpgastat(dt);
		*(generalTable(0,iDevType*2+1)->value) << std::showbase << std::hex << infpgastat;
		if (infpgastat & 0x00004000) generalTable(0,iDevType*2+1)->setClass("warning");
		if (infpgastat & 0x00008000) {
			generalTable(0,iDevType*2+1)->setClass("bad");
			debugTrapValid[iDevType] = true;
		}
		if (!(infpgastat & 0x0000C000)) generalTable(0,iDevType*2+1)->setClass("ok");
		std::map<std::string, std::string> infpgastatComments = emu::fed::DDUDebugger::InFPGAStat(dt,infpgastat);
		for (std::map<std::string,std::string>::iterator iComment = infpgastatComments.begin();
			iComment != infpgastatComments.end();
			iComment++) {
			*(generalTable(0,iDevType*2+2)->value) << cgicc::div(iComment->first)
				.set("class",iComment->second);
		}

		// PGK Flaking out
		//unsigned long int L1Scaler = myDDU->readL1Scaler(dt);
		unsigned long int L1Scaler = myDDU->infpga_rdscaler(dt);
		*(generalTable(1,iDevType*2+1)->value) << L1Scaler;

		// PGK Flaking out
		//unsigned long int L1Scaler1 = myDDU->readL1Scaler1(dt);
		unsigned long int L1Scaler1 = myDDU->infpga_rd1scaler(dt);
		*(generalTable(2,iDevType*2+1)->value) << L1Scaler1;

	}
	
	// Summary
	*out << generalTable.printSummary() << std::endl;

	// We use a special case of the toHTML method for InFPGAs.
	// Hide the table if all is keen.
	if (generalTable.countClass("ok") + generalTable.countClass("none")
		== generalTable.countRows() * 2) {
		*out << cgicc::table()
			.set("id","generalTable")
			.set("class","data")
			.set("style","display: none;") << std::endl;
	} else {
		*out << cgicc::table()
			.set("id","generalTable")
			.set("class","data") << std::endl;
	}

	// Put the titles of the two InFPGAs here.
	*out << cgicc::tr() << std::endl;
	*out << cgicc::td(" ") << std::endl;
	*out << cgicc::td(devNames[0])
		.set("colspan","2")
		.set("style","font-weight: bold; text-align: center; border-right: 3px double #000;")<< std::endl;
	*out << cgicc::td(devNames[1])
		.set("colspan","2")
		.set("style","font-weight: bold; text-align: center;") << std::endl;
	*out << cgicc::tr() << std::endl;

	// Now print the concatonated table.
	*out << generalTable.toHTML(false);

	// And now close the table.
	*out << cgicc::table() << std::endl;

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

	emu::fed::DataTable fiberTable("fiberTable");

	// Names have to come first, because there is only one of each.
	fiberTable.addColumn("Register");
	fiberTable.addColumn("Value");
	fiberTable.addColumn("Decoded Chambers");
	fiberTable.addBreak();
	fiberTable.addColumn("Value");
	fiberTable.addColumn("Decoded Chambers");

	for (unsigned int iDevType = 0; iDevType < 2; iDevType++) {
		enum emu::fed::DEVTYPE dt = devTypes[iDevType];
		unsigned int fiberOffset = (dt == emu::fed::INFPGA0 ? 0 : 8);

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

		unsigned int readDMBWarning = myDDU->readDMBWarning(dt);
		*(fiberTable(0,iDevType*2+1)->value) << std::showbase << std::hex << ((readDMBWarning >> 8) & 0xff);
		if (((readDMBWarning >> 8) & 0xff)) fiberTable(0,iDevType*2+1)->setClass("bad");
		else fiberTable(0,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readDMBWarning >> 8) & 0xff) & (1<<iFiber)) {
				//fibersWithErrors |= (1<<(iFiber + fiberOffset));
				*(fiberTable(0,iDevType*2+2)->value) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","red");
			}
		}

		*(fiberTable(1,iDevType*2+1)->value) << std::showbase << std::hex << ((readDMBWarning) & 0xff);
		if (((readDMBWarning) & 0xff)) fiberTable(1,iDevType*2+1)->setClass("warning");
		else fiberTable(1,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readDMBWarning) & 0xff) & (1<<iFiber)) {
				*(fiberTable(1,iDevType*2+2)->value) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","orange");
			}
		}

		unsigned int checkFiber = myDDU->checkFiber(dt);
		*(fiberTable(2,iDevType*2+1)->value) << std::showbase << std::hex << ((checkFiber >> 8) & 0xff);
		if (((checkFiber >> 8) & 0xff)) fiberTable(2,iDevType*2+1)->setClass("bad");
		else fiberTable(2,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((checkFiber >> 8) & 0xff) & (1<<iFiber)) {
				//fibersWithErrors |= (1<<(iFiber + fiberOffset));
				*(fiberTable(2,iDevType*2+2)->value) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","red");
			}
		}

		*(fiberTable(3,iDevType*2+1)->value) << std::showbase << std::hex << ((checkFiber) & 0xff);
		fiberTable(3,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((checkFiber) & 0xff) & (1<<iFiber)) {
				*(fiberTable(3,iDevType*2+2)->value) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","none");
			}
		}

		unsigned int readDMBSync = myDDU->readDMBSync(dt);
		*(fiberTable(4,iDevType*2+1)->value) << std::showbase << std::hex << ((readDMBSync >> 8) & 0xff);
		if (((readDMBSync >> 8) & 0xff)) fiberTable(4,iDevType*2+1)->setClass("bad");
		else fiberTable(4,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readDMBSync >> 8) & 0xff) & (1<<iFiber)) {
				//fibersWithErrors |= (1<<(iFiber + fiberOffset));
				*(fiberTable(4,iDevType*2+2)->value) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","red");
			}
		}

		*(fiberTable(5,iDevType*2+1)->value) << std::showbase << std::hex << ((readDMBSync) & 0xff);
		if (((readDMBSync) & 0xff)) fiberTable(5,iDevType*2+1)->setClass("warning");
		else fiberTable(5,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readDMBSync) & 0xff) & (1<<iFiber)) {
				*(fiberTable(5,iDevType*2+2)->value) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","orange");
			}
		}

		unsigned int readRxError = myDDU->readRxError(dt);
		*(fiberTable(6,iDevType*2+1)->value) << std::showbase << std::hex << ((readRxError >> 8) & 0xff);
		if (((readRxError >> 8) & 0xff)) fiberTable(6,iDevType*2+1)->setClass("questionable");
		else fiberTable(6,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readRxError >> 8) & 0xff) & (1<<iFiber)) {
				*(fiberTable(6,iDevType*2+2)->value) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","blue");
			}
		}

		*(fiberTable(7,iDevType*2+1)->value) << std::showbase << std::hex << ((readRxError) & 0xff);
		if (((readRxError) & 0xff)) fiberTable(7,iDevType*2+1)->setClass("bad");
		else fiberTable(7,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readRxError) & 0xff) & (1<<iFiber)) {
				//fibersWithErrors |= (1<<(iFiber + fiberOffset));
				*(fiberTable(7,iDevType*2+2)->value) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","red");
			}
		}

		unsigned int readTimeout = myDDU->readTimeout(dt);
		*(fiberTable(8,iDevType*2+1)->value) << std::showbase << std::hex << ((readTimeout >> 8) & 0xff);
		if (((readTimeout >> 8) & 0xff)) fiberTable(8,iDevType*2+1)->setClass("bad");
		else fiberTable(8,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readTimeout >> 8) & 0xff) & (1<<iFiber)) {
				//fibersWithErrors |= (1<<(iFiber + fiberOffset));
				*(fiberTable(8,iDevType*2+2)->value) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","red");
			}
		}

		*(fiberTable(9,iDevType*2+1)->value) << std::showbase << std::hex << ((readTimeout) & 0xff);
		if (((readTimeout) & 0xff)) fiberTable(9,iDevType*2+1)->setClass("bad");
		else fiberTable(9,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readTimeout) & 0xff) & (1<<iFiber)) {
				//fibersWithErrors |= (1<<(iFiber + fiberOffset));
				*(fiberTable(9,iDevType*2+2)->value) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","red");
			}
		}

		unsigned int readTxError = myDDU->readTxError(dt);
		*(fiberTable(10,iDevType*2+1)->value) << std::showbase << std::hex << ((readTxError >> 8) & 0xff);
		fiberTable(10,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readTxError >> 8) & 0xff) & (1<<iFiber)) {
				*(fiberTable(10,iDevType*2+2)->value) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","none");
			}
		}

		*(fiberTable(11,iDevType*2+1)->value) << std::showbase << std::hex << ((readTxError) & 0xff);
		if (((readTxError) & 0xff)) fiberTable(11,iDevType*2+1)->setClass("bad");
		else fiberTable(11,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readTxError) & 0xff) & (1<<iFiber)) {
				//fibersWithErrors |= (1<<(iFiber + fiberOffset));
				*(fiberTable(11,iDevType*2+2)->value) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","red");
			}
		}

		unsigned int readLostError = myDDU->readLostError(dt);
		*(fiberTable(12,iDevType*2+1)->value) << std::showbase << std::hex << ((readLostError >> 8) & 0xff);
		if (((readLostError) & 0xff)) fiberTable(12,iDevType*2+1)->setClass("warning");
		else fiberTable(12,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readLostError >> 8) & 0xff) & (1<<iFiber)) {
				*(fiberTable(12,iDevType*2+2)->value) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","orange");
			}
		}

		*(fiberTable(13,iDevType*2+1)->value) << std::showbase << std::hex << ((readLostError) & 0xff);
		if (((readLostError) & 0xff)) fiberTable(13,iDevType*2+1)->setClass("bad");
		else fiberTable(13,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readLostError) & 0xff) & (1<<iFiber)) {
				//fibersWithErrors |= (1<<(iFiber + fiberOffset));
				*(fiberTable(13,iDevType*2+2)->value) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","red");
			}
		}
	}

	// Summary
	*out << fiberTable.printSummary() << std::endl;

	// We use a special case of the toHTML method for InFPGAs.
	// Hide the table if all is keen.
	if (fiberTable.countClass("ok") + fiberTable.countClass("none")
		== fiberTable.countRows() * 2) {
		*out << cgicc::table()
			.set("id","fiberTable")
			.set("class","data")
			.set("style","display: none;") << std::endl;
	} else {
		*out << cgicc::table()
			.set("id","fiberTable")
			.set("class","data") << std::endl;
	}

	// Put the titles of the two InFPGAs here.
	*out << cgicc::tr() << std::endl;
	*out << cgicc::td(" ") << std::endl;
	*out << cgicc::td(devNames[0])
		.set("colspan","2")
		.set("style","font-weight: bold; text-align: center; border-right: 3px double #000;")<< std::endl;
	*out << cgicc::td(devNames[1])
		.set("colspan","2")
		.set("style","font-weight: bold; text-align: center;") << std::endl;
	*out << cgicc::tr() << std::endl;

	// Now print the concatonated table.
	*out << fiberTable.toHTML(false);

	// And now close the table.
	*out << cgicc::table() << std::endl;

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

	emu::fed::DataTable otherTable("otherTable");

	// Names have to come first, because there is only one of each.
	otherTable.addColumn("Register");
	otherTable.addColumn("Value");
	otherTable.addColumn("Decoded Chambers/Status");
	otherTable.addBreak();
	otherTable.addColumn("Value");
	otherTable.addColumn("Decoded Chambers/Status");

	for (unsigned int iDevType = 0; iDevType < 2; iDevType++) {
		enum emu::fed::DEVTYPE dt = devTypes[iDevType];
		unsigned int fiberOffset = (dt == emu::fed::INFPGA0 ? 0 : 8);

		if (iDevType == 0) {
			*(otherTable(0,0)->value) << "Input buffer empty";
			*(otherTable(1,0)->value) << "Special decode bits (8-bit)";
			*(otherTable(2,0)->value) << "Input buffer full history";
			*(otherTable(3,0)->value) << "Special decode bits (4-bit)";
			*(otherTable(4,0)->value) << "InRD0/2 C-code status (8-bit)";
			*(otherTable(5,0)->value) << "InRD1/3 C-code status (8-bit)";
		}

		unsigned int readFIFOStat = myDDU->readFIFOStat(dt);
		*(otherTable(0,iDevType*2+1)->value) << std::showbase << std::hex << ((readFIFOStat >> 8) & 0xff);
		//if (((readFIFOStat >> 8) & 0xff)) otherTable(0,iDevType*2+1)->setClass("bad");
		otherTable(0,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readFIFOStat >> 8) & 0xff) & (1<<iFiber)) {
				*(otherTable(0,iDevType*2+2)->value) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","none");
			}
		}

		*(otherTable(1,iDevType*2+1)->value) << std::showbase << std::hex << ((readFIFOStat) & 0xff);
		if (((readFIFOStat) & 0xfc)) otherTable(1,iDevType*2+1)->setClass("warning");
		else otherTable(1,iDevType*2+1)->setClass("ok");
		std::map<std::string, std::string> readFIFOStatComments = emu::fed::DDUDebugger::FIFOStat(dt,(readFIFOStat) & 0xff);
		for (std::map< std::string, std::string >::iterator iComment = readFIFOStatComments.begin();
			iComment != readFIFOStatComments.end();
			iComment++) {
			*(otherTable(1,iDevType*2+2)->value) << cgicc::div(iComment->first)
				.set("class",iComment->second) << std::endl;
		}

		unsigned int readFIFOFull = myDDU->readFIFOFull(dt);
		*(otherTable(2,iDevType*2+1)->value) << std::showbase << std::hex << ((readFIFOFull) & 0xff);
		if (((readFIFOFull) & 0xff)) otherTable(2,iDevType*2+1)->setClass("bad");
		else otherTable(2,iDevType*2+1)->setClass("ok");
		for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
			if (((readFIFOFull) & 0xff) & (1<<iFiber)) {
				*(otherTable(2,iDevType*2+2)->value) << cgicc::div(myDDU->getChamber(iFiber + fiberOffset)->name())
					.set("class","red");
			}
		}

		*(otherTable(3,iDevType*2+1)->value) << std::showbase << std::hex << ((readFIFOFull >> 8) & 0xff);
		if (((readFIFOFull >> 8) & 0xf)) otherTable(3,iDevType*2+1)->setClass("warning");
		else otherTable(3,iDevType*2+1)->setClass("ok");
		std::map<std::string, std::string> readFIFOFullComments = emu::fed::DDUDebugger::FIFOFull(dt,(readFIFOFull >> 8) & 0xff);
		for (std::map< std::string, std::string >::iterator iComment = readFIFOFullComments.begin();
			iComment != readFIFOFullComments.end();
			iComment++) {
			*(otherTable(3,iDevType*2+2)->value) << cgicc::div(iComment->first)
				.set("class",iComment->second) << std::endl;
		}

		unsigned int readCCodeStat = myDDU->infpga_CcodeStat(dt);
		*(otherTable(4,iDevType*2+1)->value) << std::showbase << std::hex << ((readCCodeStat) & 0xff);
		if ((readCCodeStat & 0xff) == 0x20) otherTable(4,iDevType*2+1)->setClass("warning");
		else if (readCCodeStat & 0xff) otherTable(4,iDevType*2+1)->setClass("bad");
		else otherTable(4,iDevType*2+1)->setClass("ok");
		std::map<std::string, std::string> readCCodeStatComments = emu::fed::DDUDebugger::CCodeStat(dt,(readCCodeStat) & 0xff);
		for (std::map< std::string, std::string >::iterator iComment = readCCodeStatComments.begin();
			iComment != readCCodeStatComments.end();
			iComment++) {
			*(otherTable(4,iDevType*2+2)->value) << cgicc::div(iComment->first)
				.set("class",iComment->second) << std::endl;
		}

		*(otherTable(5,iDevType*2+1)->value) << std::showbase << std::hex << ((readCCodeStat >> 8) & 0xff);
		if (((readCCodeStat >> 8) & 0xff) == 0x20) otherTable(5,iDevType*2+1)->setClass("warning");
		else if (((readCCodeStat >> 8) & 0xff)) otherTable(5,iDevType*2+1)->setClass("bad");
		else otherTable(5,iDevType*2+1)->setClass("ok");
		readCCodeStatComments = emu::fed::DDUDebugger::CCodeStat(dt,(readCCodeStat >> 8) & 0xff);
		for (std::map< std::string, std::string >::iterator iComment = readCCodeStatComments.begin();
			iComment != readCCodeStatComments.end();
			iComment++) {
			*(otherTable(5,iDevType*2+2)->value) << cgicc::div(iComment->first)
				.set("class",iComment->second) << std::endl;
		}
		
	}

	// Summary
	*out << otherTable.printSummary() << std::endl;

	// We use a special case of the toHTML method for InFPGAs.
	// Hide the table if all is keen.
	if (otherTable.countClass("ok") + otherTable.countClass("none")
		== otherTable.countRows() * 2) {
		*out << cgicc::table()
			.set("id","otherTable")
			.set("class","data")
			.set("style","display: none;") << std::endl;
	} else {
		*out << cgicc::table()
			.set("id","otherTable")
			.set("class","data") << std::endl;
	}

	// Put the titles of the two InFPGAs here.
	*out << cgicc::tr() << std::endl;
	*out << cgicc::td(" ") << std::endl;
	*out << cgicc::td(devNames[0])
		.set("colspan","2")
		.set("style","font-weight: bold; text-align: center; border-right: 3px double #000;")<< std::endl;
	*out << cgicc::td(devNames[1])
		.set("colspan","2")
		.set("style","font-weight: bold; text-align: center;") << std::endl;
	*out << cgicc::tr() << std::endl;

	// Now print the concatonated table.
	*out << otherTable.toHTML(false);

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

	emu::fed::DataTable memTable("memTable");

	// Names have to come first, because there is only one of each.
	memTable.addColumn("Register");
	memTable.addColumn("Value");
	memTable.addColumn("Decoded Status");
	memTable.addBreak();
	memTable.addColumn("Value");
	memTable.addColumn("Decoded Status");

	for (unsigned int iDevType = 0; iDevType < 2; iDevType++) {
		enum emu::fed::DEVTYPE dt = devTypes[iDevType];

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

		unsigned int memValue = myDDU->readFiberDiagnostics(dt,0);
		*(memTable(0,iDevType*2+1)->value) << std::showbase << std::hex << memValue;
		memTable(0,iDevType*2+1)->setClass("ok");
		std::map< std::string, std::string> memComments = emu::fed::DDUDebugger::FiberDiagnostics(dt,0,memValue);
		for (std::map< std::string, std::string >::iterator iComment = memComments.begin();
			iComment != memComments.end();
			iComment++) {
			*(memTable(0,iDevType*2+2)->value) << cgicc::div(iComment->first)
				.set("class",iComment->second) << std::endl;
		}

		memValue = myDDU->readFiberDiagnostics(dt,1);
		*(memTable(1,iDevType*2+1)->value) << std::showbase << std::hex << memValue;
		memTable(1,iDevType*2+1)->setClass("ok");
		memComments = emu::fed::DDUDebugger::FiberDiagnostics(dt,1,memValue);
		for (std::map< std::string, std::string >::iterator iComment = memComments.begin();
			iComment != memComments.end();
			iComment++) {
			*(memTable(1,iDevType*2+2)->value) << cgicc::div(iComment->first)
				.set("class",iComment->second) << std::endl;
		}

		memValue = myDDU->infpga_MemAvail(dt);
		*(memTable(2,iDevType*2+1)->value) << (memValue & 0x1f);
		if ((memValue & 0x1f) == 1) memTable(2,iDevType*2+1)->setClass("warning");
		else if ((memValue & 0x1f) == 0) memTable(2,iDevType*2+1)->setClass("bad");
		else memTable(2,iDevType*2+1)->setClass("ok");

		//memValue = myDDU->infpga_MemAvail(dt);
		*(memTable(3,iDevType*2+1)->value) << ((memValue >> 5) & 0x1f);
		if (((memValue >> 5) & 0x1f) == 1) memTable(3,iDevType*2+1)->setClass("warning");
		if (((memValue >> 5) & 0x1f) == 0) memTable(3,iDevType*2+1)->setClass("bad");
		else memTable(3,iDevType*2+1)->setClass("ok");

		memValue = myDDU->infpga_Min_Mem(dt);
		*(memTable(4,iDevType*2+1)->value) << (memValue & 0x1f);
		if ((memValue & 0x1f) == 1) memTable(4,iDevType*2+1)->setClass("warning");
		else if ((memValue & 0x1f) == 0) memTable(4,iDevType*2+1)->setClass("bad");
		else memTable(4,iDevType*2+1)->setClass("ok");

		//memValue = myDDU->infpga_Min_Mem(dt);
		*(memTable(5,iDevType*2+1)->value) << ((memValue >> 5) & 0x1f);
		if (((memValue >> 5) & 0x1f) == 1) memTable(5,iDevType*2+1)->setClass("warning");
		if (((memValue >> 5) & 0x1f) == 0) memTable(5,iDevType*2+1)->setClass("bad");
		else memTable(5,iDevType*2+1)->setClass("ok");

		for (unsigned int ireg = 0; ireg < 4; ireg++) {
			memValue = myDDU->readWriteMemoryActive(dt,ireg);
			*(memTable(ireg + 6,iDevType*2+1)->value) << std::showbase << std::hex << memValue;
			memTable(ireg + 6,iDevType*2+1)->setClass("ok");
			memComments = emu::fed::DDUDebugger::WriteMemoryActive(dt,ireg,memValue);
			for (std::map< std::string, std::string >::iterator iComment = memComments.begin();
				iComment != memComments.end();
				iComment++) {
				*(memTable(ireg + 6,iDevType*2+2)->value) << cgicc::div(iComment->first)
					.set("class",iComment->second) << std::endl;
			}
		}
	}

	// Summary
	*out << memTable.printSummary() << std::endl;

	// We use a special case of the toHTML method for InFPGAs.
	// Hide the table if all is keen.
	if (memTable.countClass("ok") + memTable.countClass("none")
		== memTable.countRows() * 2) {
		*out << cgicc::table()
			.set("id","memTable")
			.set("class","data")
			.set("style","display: none;") << std::endl;
	} else {
		*out << cgicc::table()
			.set("id","memTable")
			.set("class","data") << std::endl;
	}

	// Put the titles of the two InFPGAs here.
	*out << cgicc::tr() << std::endl;
	*out << cgicc::td(" ") << std::endl;
	*out << cgicc::td(devNames[0])
		.set("colspan","2")
		.set("style","font-weight: bold; text-align: center; border-right: 3px double #000;")<< std::endl;
	*out << cgicc::td(devNames[1])
		.set("colspan","2")
		.set("style","font-weight: bold; text-align: center;") << std::endl;
	*out << cgicc::tr() << std::endl;

	// Now print the concatonated table.
	*out << memTable.toHTML(false);

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
		enum emu::fed::DEVTYPE dt = devTypes[iDevType];

		*out << cgicc::span()
			.set("style","background-color: #FFF; border: 2px solid #000; padding: 10px; width: 95%; margin: 10px auto 10px auto; display: block;") << std::endl;
		*out << cgicc::div(devNames[iDevType])
			.set("style","font-size: 14pt; font-weight: bold; width: 100%; text-align: center;") << std::endl;

		if (debugTrapValid[iDevType]) {

			// Here it is.
			std::vector<unsigned long int> lcode = myDDU->infpga_trap(dt);
			std::vector<std::string> bigComments = emu::fed::DDUDebugger::infpga_trap(lcode, dt);

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

}




void EmuFCrateHyperDAQ::DDUExpert(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	// PGK Patented check-for-initialization
	if (crateVector.size()==0) {
		LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
		return Default(in,out);
	}

	cgicc::Cgicc cgi(in);
	
	// First, I need a crate.
	cgicc::form_iterator name = cgi.getElement("crate");
	unsigned int cgiCrate = 0;
	if(name != cgi.getElements().end()) {
		cgiCrate = cgi["crate"]->getIntegerValue();
	}
	emu::fed::FEDCrate *myCrate = crateVector[cgiCrate];

	name = cgi.getElement("ddu");
	unsigned int cgiDDU = 0;
	if (name != cgi.getElements().end()) {
		cgiDDU = cgi["ddu"]->getIntegerValue();
		//std::cout << "DDU inside " << ddu << std::endl;
	}
	emu::fed::DDU *myDDU = myCrate->getDDUs()[cgiDDU];

	std::stringstream sTitle;
	sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DDU Expert Controls (RUI #" << myCrate->getRUI(myDDU->slot()) << ")";
	*out << Header(sTitle.str(),false);

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
		.set("name","ddu")
		.set("value",dduVal) << std::endl;
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","crate")
		.set("value",crateVal) << std::endl;

	unsigned long int currentKillFiber = myDDU->ddu_rdkillfiber();
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
	long int liveFibers = (myDDU->checkFiber(emu::fed::INFPGA0)&0x000000ff) | ((myDDU->checkFiber(emu::fed::INFPGA1)&0x000000ff)<<8);
	long int killFiber = myDDU->ddu_rdkillfiber();
	unsigned int fibersWithErrors = myDDU->readCSCStatAdvanced() | myDDU->readAdvancedFiberErrors();

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
		emu::fed::Chamber *thisChamber = myDDU->getChamber(iFiber);
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
				.set("value","1")
				.set("onChange",swapCommand.str())
				.set("checked","checked") << "Live" << cgicc::br() << std::endl;
			*out << cgicc::input()
				.set("type","radio")
				.set("name",radioName.str())
				.set("value","0")
				.set("onChange",swapCommand.str()) << "Killed" << std::endl;
		} else {
			*out << cgicc::input()
				.set("type","radio")
				.set("name",radioName.str())
				.set("value","1")
				.set("onChange",swapCommand.str()) << "Live" << cgicc::br() << std::endl;
			*out << cgicc::input()
				.set("type","radio")
				.set("name",radioName.str())
				.set("value","0")
				.set("onChange",swapCommand.str())
				.set("checked","checked") << "Killed" << std::endl;
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
			.set("onChange","javascript:toggleBit('killFiber',15);")
			.set("checked","checked") << std::endl;
	} else {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box15")
			.set("onChange","javascript:toggleBit('killFiber',15);") << std::endl;
	}
	*out << "Force all DDU checks" << cgicc::div() << std::endl;

	*out << cgicc::div()
		.set("style","font-size: 8pt;") << std::endl;
	if (currentKillFiber & (1<<16)) {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box16")
			.set("onChange","javascript:toggleBit('killFiber',16);")
			.set("checked","checked") << std::endl;
	} else {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box16")
			.set("onChange","javascript:toggleBit('killFiber',16);") << std::endl;
	}
	*out << "Force ALCT checks" << cgicc::div() << std::endl;

	*out << cgicc::div()
		.set("style","font-size: 8pt;") << std::endl;
	if (currentKillFiber & (1<<17)) {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box17")
			.set("onChange","javascript:toggleBit('killFiber',17);")
			.set("checked","checked") << std::endl;
	} else {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box17")
			.set("onChange","javascript:toggleBit('killFiber',17);") << std::endl;
	}
	*out << "Force TMB checks" << cgicc::div() << std::endl;

	*out << cgicc::div()
		.set("style","font-size: 8pt;") << std::endl;
	if (currentKillFiber & (1<<18)) {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box18")
			.set("onChange","javascript:toggleBit('killFiber',18);")
			.set("checked","checked") << std::endl;
	} else {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box18")
			.set("onChange","javascript:toggleBit('killFiber',18);") << std::endl;
	}
	*out << "Force CFEB checks (enable DAV checks)" << cgicc::div() << std::endl;

	*out << cgicc::div()
		.set("style","font-size: 8pt;") << std::endl;
	if (currentKillFiber & (1<<19)) {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box19")
			.set("onChange","javascript:toggleBit('killFiber',19);")
			.set("checked","checked") << std::endl;
	} else {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box19")
			.set("onChange","javascript:toggleBit('killFiber',19);") << std::endl;
	}
	*out << "Force normal DDU checks (off enables only SP/TF checks)" << cgicc::div() << std::endl;

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
		.set("name","ddu")
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
		.set("name","ddu")
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
		.set("name","ddu")
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
		.set("name","ddu")
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
		.set("name","ddu")
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
		.set("name","ddu")
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




	//myCrate->getVMEController()->CAEN_err_reset();
	// Inreg for serial writes.
	*out << cgicc::fieldset()
		.set("class","expert") << std::endl;
	*out << cgicc::div()
		.set("class","legend") << std::endl;
	*out << "Input Registers for Flash Writes" << cgicc::div() << std::endl;

	// New idea:  make the table first, then display it later.
	emu::fed::DataTable inregTable("inregTable");
	inregTable.addColumn("Register");
	inregTable.addColumn("Value");
	inregTable.addColumn("New Value");

	for (unsigned int iReg = 0; iReg < 3; iReg++) {
		unsigned int inreg = myDDU->readInputReg(iReg);
		*(inregTable(iReg,0)->value) << "InReg" << iReg;
		*(inregTable(iReg,1)->value) << std::showbase << std::hex << inreg;
		inregTable[iReg]->setClass("none");
		if (iReg == 0) {
			*(inregTable(iReg,2)->value) << inregTable[iReg]->makeForm(dduTextLoad,cgiCrate,cgiDDU,9) << std::endl;
		}
	}

	*out << inregTable.toHTML() << std::endl;

	*out << cgicc::div()
		.set("style","font-size: 8pt;") << std::endl;
	*out << "Input registers are used for VMS serial writing.  You can load up to 48 bits in these three registers with bit 0 of InReg0 being the LSB." << cgicc::br() << std::endl;
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

	// New idea:  make the table first, then display it later.
	emu::fed::DataTable expertTable("expertTable");
	expertTable.addColumn("Register");
	expertTable.addColumn("Value");
	expertTable.addColumn("Decoded Register Info");
	expertTable.addColumn("New Value");

	// FIXME
	unsigned int gbePrescale = myDDU->readGbEPrescale();
	*(expertTable(0,0)->value) << "GbE prescale";
	*(expertTable(0,1)->value) << std::showbase << std::hex << gbePrescale;
	/*std::map<std::string,std::string> gbePrescaleComments = emu::fed::DDUDebugger::GbEPrescale(gbePrescale);
	for (std::map<std::string,std::string>::iterator iComment = gbePrescaleComments.begin();
		iComment != gbePrescaleComments.end();
		iComment++) {
		*(expertTable(0,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}
	*/
	*(expertTable(0,3)->value) << expertTable[0]->makeForm(dduTextLoad,cgiCrate,cgiDDU,11);
	expertTable[0]->setClass("none");

	unsigned int fakeL1 = myDDU->readFakeL1Reg();
	*(expertTable(1,0)->value) << "Fake L1A Data Passthrough";
	*(expertTable(1,1)->value) << std::showbase << std::hex << fakeL1;
	/*
	std::map<std::string,std::string> fakeL1Comments = emu::fed::DDUDebugger::FakeL1Reg(fakeL1);
	for (std::map<std::string,std::string>::iterator iComment = fakeL1Comments.begin();
		iComment != fakeL1Comments.end();
		iComment++) {
		*(expertTable(1,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}
	*/
	*(expertTable(1,3)->value) << expertTable[1]->makeForm(dduTextLoad,cgiCrate,cgiDDU,13);
	expertTable[1]->setClass("none");

	unsigned int foe = myDDU->readFMMReg();
	*(expertTable(2,0)->value) << "F0E + 4-bit FMM";
	*(expertTable(2,1)->value) << std::showbase << std::hex << foe;
	/*
	std::map<std::string,std::string> foeComments = emu::fed::DDUDebugger::F0EReg(foe);
	for (std::map<std::string,std::string>::iterator iComment = foeComments.begin();
		iComment != foeComments.end();
		iComment++) {
		*(expertTable(2,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}
	*/
	*(expertTable(2,3)->value) << expertTable[2]->makeForm(dduTextLoad,cgiCrate,cgiDDU,18);
	expertTable[2]->setClass("none");

	*(expertTable(3,0)->value) << "Switches";
	*(expertTable(3,1)->value) << std::showbase << std::hex << (myDDU->readSwitches() & 0xff);
	expertTable[3]->setClass("none");

	for (unsigned int iReg = 0; iReg < 5; iReg++) {
		*(expertTable(4 + iReg,0)->value) << "Test register " << iReg;
		*(expertTable(4 + iReg,1)->value) << std::showbase << std::hex << (myDDU->readTestReg(iReg));
		expertTable[4 + iReg]->setClass("none");
	}

	*out << expertTable.toHTML() << std::endl;

	// You can toggle some more things down here.
	*out << cgicc::form()
		.set("method","GET")
		.set("action",dduTextLoad) << std::endl;
	*out << cgicc::input()
		.set("name","ddu")
		.set("type","hidden")
		.set("value",dduVal) << std::endl;
	*out << cgicc::input()
		.set("name","crate")
		.set("type","hidden")
		.set("value",crateVal) << std::endl;
	// This is a relic from the old days, and is read by DDUTextLoad.
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
		.set("name","ddu")
		.set("type","hidden")
		.set("value",dduVal) << std::endl;
	*out << cgicc::input()
		.set("name","crate")
		.set("type","hidden")
		.set("value",crateVal) << std::endl;
	// This is a relic from the old days, and is read by DDUTextLoad.
	*out << cgicc::input()
		.set("name","command")
		.set("type","hidden")
		.set("value","12") << std::endl;
	*out << cgicc::input()
		.set("type","submit")
		.set("value","Toggle all fake L1A (data dump/passthrough)") << std::endl;
	*out << cgicc::form() << std::endl;

	*out << cgicc::form()
		.set("method","GET")
		.set("action",dduTextLoad) << std::endl;
	*out << cgicc::input()
		.set("name","ddu")
		.set("type","hidden")
		.set("value",dduVal) << std::endl;
	*out << cgicc::input()
		.set("name","crate")
		.set("type","hidden")
		.set("value",crateVal) << std::endl;
	// This is a relic from the old days, and is read by DDUTextLoad.
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
	*out << "Fake L1A bit 0=1 sets InFPGA0 passthrough" << cgicc::br() << std::endl;
	*out << "Fake L1A bit 1=1 sets InFPGA1 passthrough" << cgicc::br() << std::endl;
	*out << "Fake L1A bit 2=1 sets DDUFPGA passthrough" << cgicc::br() << std::endl;
	*out << "F0E register bits 4-15=0xFED0 disables DDU FMM error reporting" << std::endl;
	*out << cgicc::div() << std::endl;

	*out << cgicc::fieldset() << std::endl;


	// Parallel/flash writable registers
	*out << cgicc::fieldset()
		.set("class","expert") << std::endl;
	*out << cgicc::div()
		.set("class","legend") << std::endl;
	*out << "Writable Flash Pages" << cgicc::div() << std::endl;

	emu::fed::DataTable writableTable("writableTable");
	writableTable.addColumn("Page");
	writableTable.addColumn("Value");
	writableTable.addColumn("New Value");

	*(writableTable(0,0)->value) << "Flash live channels";
	*(writableTable(0,1)->value) << std::showbase << std::hex << myDDU->read_page1();
	writableTable[0]->setClass("none");
	// New Value...
	*(writableTable(0,2)->value) << writableTable[0]->makeForm(dduTextLoad,cgiCrate,cgiDDU,14) << std::endl;

	std::vector<int> lcode = myDDU->read_page5();
	*(writableTable(1,0)->value) << "Flash GbE FIFO thresholds";
	*(writableTable(1,1)->value) << "0x" << std::hex << ((lcode[4]&0xC0)>>6) << std::noshowbase << std::setw(8) << std::setfill('0') << std::hex << (((((lcode[2]&0xC0)>>6)|((lcode[5]&0xFF)<<2)|((lcode[4]&0x3F)<<10)) << 16) | (((lcode[0]&0xC0)>>6)|((lcode[3]&0xFF)<<2)|((lcode[2]&0x3F)<<10)));
	writableTable[1]->setClass("none");
	// New Value...
	*(writableTable(1,2)->value) << writableTable[1]->makeForm(dduTextLoad,cgiCrate,cgiDDU,15) << std::endl;

	*(writableTable(2,0)->value) << "Flash Board ID";
	*(writableTable(2,1)->value) << myDDU->readFlashBoardID();
	writableTable[2]->setClass("none");
	// New Value...
	*(writableTable(2,2)->value) << writableTable[2]->makeForm(dduTextLoad,cgiCrate,cgiDDU,16) << std::endl;

	*(writableTable(3,0)->value) << "Flash DDU RUI";
	*(writableTable(3,1)->value) << myDDU->readFlashRUI();
	writableTable[3]->setClass("none");
	// New Value...
	*(writableTable(3,2)->value) << writableTable[3]->makeForm(dduTextLoad,cgiCrate,cgiDDU,17) << std::endl;

	*out << writableTable.toHTML() << std::endl;

	*out << cgicc::fieldset() << std::endl;	

	
	*out << Footer() << std::endl;
}




void EmuFCrateHyperDAQ::VMEPARA(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{

	// PGK Patented check-for-initialization
	if (crateVector.size()==0) {
		LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
		return Default(in,out);
	}

	cgicc::Cgicc cgi(in);
	
	// First, I need a crate.
	cgicc::form_iterator name = cgi.getElement("crate");
	unsigned int cgiCrate = 0;
	if(name != cgi.getElements().end()) {
		cgiCrate = cgi["crate"]->getIntegerValue();
	}
	emu::fed::FEDCrate *myCrate = crateVector[cgiCrate];

	name = cgi.getElement("ddu");
	unsigned int cgiDDU = 0;
	if (name != cgi.getElements().end()) {
		cgiDDU = cgi["ddu"]->getIntegerValue();
		//std::cout << "DDU inside " << ddu << std::endl;
	}
	emu::fed::DDU *myDDU = myCrate->getDDUs()[cgiDDU];

	std::stringstream sTitle;
	sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DDU VME Parallel Controls (RUI #" << myCrate->getRUI(myDDU->slot()) << ")";
	*out << Header(sTitle.str(),false);

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

	// New idea:  make the table first, then display it later.
	emu::fed::DataTable statusTable("statusTable");
	statusTable.addColumn("Register");
	statusTable.addColumn("Value");

	unsigned int parallelStat = myDDU->readParallelStat();
	*(statusTable(0,0)->value) << "VME status register";
	*(statusTable(0,1)->value) << std::showbase << std::hex << parallelStat;
	std::map<std::string,std::string> parallelStatComments = emu::fed::DDUDebugger::ParallelStat(parallelStat);
	for (std::map<std::string,std::string>::iterator iComment = parallelStatComments.begin();
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
	*(statusTable(1,1)->value) << std::showbase << std::hex << dduFMM;
	std::map<std::string,std::string> dduFMMComments = emu::fed::DDUDebugger::FMMReg(dduFMM);
	for (std::map<std::string,std::string>::iterator iComment = dduFMMComments.begin();
		iComment != dduFMMComments.end();
		iComment++) {
		*(statusTable(1,2)->value) << cgicc::div(iComment->first)
			.set("class",iComment->second);
	}
	statusTable[1]->setClass("ok");
	if (dduFMM & 0xF == 0x4) statusTable[1]->setClass("warning");
	else if (dduFMM & 0xF == 0x1) statusTable[1]->setClass("questionable");
	else if (dduFMM & 0xF != 0x8) statusTable[1]->setClass("bad");

	*out << statusTable.printSummary() << std::endl;

	// Display only if there are errors.
	if (statusTable.countClass("ok") + statusTable.countClass("none") == statusTable.countRows()) statusTable.setHidden(true);
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

	// New idea:  make the table first, then display it later.
	emu::fed::DataTable fmmTable("fmmTable");
	fmmTable.addColumn("Register");
	fmmTable.addColumn("Value");
	fmmTable.addColumn("Decoded Chambers");

	unsigned int cscStat = myDDU->readCSCStatAdvanced() | myDDU->readAdvancedFiberErrors();
	*(fmmTable(0,0)->value) << "FMM problem report";
	*(fmmTable(0,1)->value) << std::showbase << std::hex << cscStat;
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (cscStat & (1<<iFiber)) {
			*(fmmTable(0,2)->value) << cgicc::div(myDDU->getChamber(iFiber)->name())
				.set("class","red");
		}
	}
	fmmTable[0]->setClass("ok");
	if (cscStat) fmmTable[0]->setClass("bad");

	unsigned int cscBusy = myDDU->readFMMBusy();
	*(fmmTable(1,0)->value) << "Busy";
	*(fmmTable(1,1)->value) << std::showbase << std::hex << cscBusy;
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (cscBusy & (1<<iFiber)) {
			*(fmmTable(1,2)->value) << cgicc::div(myDDU->getChamber(iFiber)->name());
		}
	}
	fmmTable[1]->setClass("ok");

	unsigned int cscWarn = myDDU->readFMMFullWarning();
	*(fmmTable(2,0)->value) << "Warning/near full";
	*(fmmTable(2,1)->value) << std::showbase << std::hex << cscWarn;
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (cscWarn & (1<<iFiber)) {
			*(fmmTable(2,2)->value) << cgicc::div(myDDU->getChamber(iFiber)->name())
				.set("class","orange");
		}
	}
	fmmTable[2]->setClass("ok");
	if (cscWarn) fmmTable[2]->setClass("warning");

	unsigned int cscLS = myDDU->readFMMLostSync();
	*(fmmTable(3,0)->value) << "Lost sync";
	*(fmmTable(3,1)->value) << std::showbase << std::hex << cscLS;
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (cscLS & (1<<iFiber)) {
			*(fmmTable(3,2)->value) << cgicc::div(myDDU->getChamber(iFiber)->name())
				.set("class","red");
		}
	}
	fmmTable[3]->setClass("ok");
	if (cscLS) fmmTable[3]->setClass("bad");

	unsigned int cscError = myDDU->readFMMError();
	*(fmmTable(4,0)->value) << "Error";
	*(fmmTable(4,1)->value) << std::showbase << std::hex << cscError;
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (cscError & (1<<iFiber)) {
			*(fmmTable(4,2)->value) << cgicc::div(myDDU->getChamber(iFiber)->name())
				.set("class","red");
		}
	}
	fmmTable[4]->setClass("ok");
	if (cscError) fmmTable[4]->setClass("bad");

	unsigned int cscWH = myDDU->readWarningHistory();
	*(fmmTable(5,0)->value) << "Warning history";
	*(fmmTable(5,1)->value) << std::showbase << std::hex << cscWH;
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (cscWH & (1<<iFiber)) {
			*(fmmTable(5,2)->value) << cgicc::div(myDDU->getChamber(iFiber)->name())
				.set("class","orange");
		}
	}
	fmmTable[5]->setClass("ok");
	if (cscWH) fmmTable[5]->setClass("warning");

	unsigned int cscBH = myDDU->readBusyHistory();
	*(fmmTable(6,0)->value) << "Busy history";
	*(fmmTable(6,1)->value) << std::showbase << std::hex << cscBH;
	for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
		if (cscBH & (1<<iFiber)) {
			*(fmmTable(6,2)->value) << cgicc::div(myDDU->getChamber(iFiber)->name());
		}
	}

	*out << fmmTable.printSummary() << std::endl;

	// Print table only if there are errors.
	if (fmmTable.countClass("ok") + fmmTable.countClass("none") == fmmTable.countRows()) fmmTable.setHidden(true);
	*out << fmmTable.toHTML() << std::endl;

	*out << cgicc::fieldset() << std::endl;

	*out << Footer() << std::endl;
	
}



void EmuFCrateHyperDAQ::VMESERI(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{

	// PGK Patented check-for-initialization
	if (crateVector.size()==0) {
		LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
		return Default(in,out);
	}

	cgicc::Cgicc cgi(in);
	
	// First, I need a crate.
	cgicc::form_iterator name = cgi.getElement("crate");
	unsigned int cgiCrate = 0;
	if(name != cgi.getElements().end()) {
		cgiCrate = cgi["crate"]->getIntegerValue();
	}
	emu::fed::FEDCrate *myCrate = crateVector[cgiCrate];

	name = cgi.getElement("ddu");
	unsigned int cgiDDU = 0;
	if (name != cgi.getElements().end()) {
		cgiDDU = cgi["ddu"]->getIntegerValue();
		//std::cout << "DDU inside " << ddu << std::endl;
	}
	emu::fed::DDU *myDDU = myCrate->getDDUs()[cgiDDU];

	std::stringstream sTitle;
	sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DDU VME Serial Controls (RUI #" << myCrate->getRUI(myDDU->slot()) << ")";
	*out << Header(sTitle.str(),false);

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

	// New idea:  make the table first, then display it later.
	emu::fed::DataTable voltTable("voltTable");
	voltTable.addColumn("Voltage");
	voltTable.addColumn("Value");

	float V15 = myDDU->adcplus(1,4);
	*(voltTable(0,0)->value) << "Voltage V15";
	*(voltTable(0,1)->value) << std::setprecision(4) << V15 << " mV";
	voltTable[0]->setClass("ok");
	if (V15 > 1500*1.025 || V15 < 1500*0.975) voltTable[0]->setClass("warning");
	if (V15 > 1500*1.05 || V15 < 1500*0.95) voltTable[0]->setClass("bad");
	if (V15 > 3500 || V15 < 0) voltTable[0]->setClass("questionable");

	float V25 = myDDU->adcplus(1,5);
	*(voltTable(1,0)->value) << "Voltage V25";
	*(voltTable(1,1)->value) << std::setprecision(4) << V25 << " mV";
	voltTable[1]->setClass("ok");
	if (V25 > 2500*1.025 || V25 < 2500*0.975) voltTable[1]->setClass("warning");
	if (V25 > 2500*1.05 || V25 < 2500*0.95) voltTable[1]->setClass("bad");
	if (V25 > 3500 || V25 < 0) voltTable[1]->setClass("questionable");
	
	float V25a = myDDU->adcplus(1,6);
	*(voltTable(2,0)->value) << "Voltage V25A";
	*(voltTable(2,1)->value) << std::setprecision(4) << V25a << " mV";
	voltTable[2]->setClass("ok");
	if (V25a > 2500*1.025 || V25a < 2500*0.975) voltTable[2]->setClass("warning");
	if (V25a > 2500*1.05 || V25a < 2500*0.95) voltTable[2]->setClass("bad");
	if (V25a > 3500 || V25a < 0) voltTable[2]->setClass("questionable");
	
	float V33 = myDDU->adcplus(1,7);
	*(voltTable(3,0)->value) << "Voltage V33";
	*(voltTable(3,1)->value) << std::setprecision(4) << V33 << " mV";
	voltTable[3]->setClass("ok");
	if (V33 > 3300*1.025 || V33 < 3300*0.975) voltTable[3]->setClass("warning");
	if (V33 > 3300*1.05 || V33 < 3300*0.95) voltTable[3]->setClass("bad");
	if (V33 > 3500 || V33 < 0) voltTable[3]->setClass("questionable");

	// Print the table summary
	*out << voltTable.printSummary() << std::endl;

	// Print actual table.  Maybe.
	if (voltTable.countClass("ok") + voltTable.countClass("none") == voltTable.countRows()) voltTable.setHidden(true);
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

	// New idea:  make the table first, then display it later.
	emu::fed::DataTable tempTable("tempTable");
	tempTable.addColumn("Temperature");
	tempTable.addColumn("Value");

	for (unsigned int iTemp = 0; iTemp < 4; iTemp++) {
		float T0 = myDDU->readthermx(iTemp);
		*(tempTable(iTemp,0)->value) << "Temperature " << iTemp;
		*(tempTable(iTemp,1)->value) << std::setprecision(4) << T0 << "&deg;F";
		tempTable[iTemp]->setClass("ok");
		if (T0 > 80*1.2 || T0 < 80*0.8) tempTable[iTemp]->setClass("warning");
		if (T0 > 80*1.4 || T0 < 80*0.6) tempTable[iTemp]->setClass("bad");
		if (T0 > 170 || T0 < 0) tempTable[iTemp]->setClass("questionable");
	}

	// Print the table summary
	*out << tempTable.printSummary() << std::endl;

	// Print actual table.  Maybe.
	if (tempTable.countClass("ok") + tempTable.countClass("none") == tempTable.countRows()) tempTable.setHidden(true);
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

	// New idea:  make the table first, then display it later.
	emu::fed::DataTable ramStatusTable("ramStatusTable");
	ramStatusTable.addColumn("Register");
	ramStatusTable.addColumn("Value");

	int ramStatus = myDDU->readSerialStat();
	*(ramStatusTable(0,0)->value) << "Serial Flash RAM Status";
	*(ramStatusTable(0,1)->value) << std::showbase << std::hex << ramStatus;
	ramStatusTable[0]->setClass("ok");
	if (ramStatus & 0x003c != 0x000c) ramStatusTable[0]->setClass("warning");
	if (ramStatus & 0x0080 != 0x0080) ramStatusTable[0]->setClass("bad");

	// Print the table summary
	*out << ramStatusTable.printSummary() << std::endl;

	// Print actual table.  Maybe.
	if (ramStatusTable.countClass("ok") + ramStatusTable.countClass("none") == ramStatusTable.countRows()) ramStatusTable.setHidden(true);
	*out << ramStatusTable.toHTML() << std::endl;

	*out << cgicc::fieldset() << std::endl;

	*out << Footer() << std::endl;

}


void EmuFCrateHyperDAQ::DDUTextLoad(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	//try {

	// PGK Patented check-for-initialization
	if (crateVector.size()==0) {
		LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
		return Default(in,out);
	}

	cgicc::Cgicc cgi(in);

	// First, I need a crate.
	cgicc::form_iterator name = cgi.getElement("crate");
	unsigned int cgiCrate = 0;
	if(name != cgi.getElements().end()) {
		cgiCrate = cgi["crate"]->getIntegerValue();
	}
	emu::fed::FEDCrate *myCrate = crateVector[cgiCrate];

	name = cgi.getElement("ddu");
	unsigned int cgiDDU = 0;
	if (name != cgi.getElements().end()) {
		cgiDDU = cgi["ddu"]->getIntegerValue();
	}
	emu::fed::DDU *myDDU = myCrate->getDDUs()[cgiDDU];

	// The command that is intended to be sent to the DDU.
	unsigned int command = 0;
	name = cgi.getElement("command");
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
		location << "crate=" << cgiCrate << "&ddu=" << cgiDDU;
		webRedirect(out,"DDUExpert?"+location.str());
	}


	LOG4CPLUS_DEBUG(getApplicationLogger(),"Attempting DDUTextLoad with crate(" << cgiCrate << ") ddu(" << cgiDDU << ") == slot(" << myDDU->slot() << ") command(" << command << ") XMLText(" << XMLText << ") == uploadValue(" << uploadValue << ") otherText(" << otherText << ") == otherValue(" << otherValue << ")");

	//unsigned short int para_val;
	//unsigned long int send_val;
	
	unsigned int scale;
	
	// Switch on the command.
	switch (command) {

	case (1): // Toggle L1 calibration signal
		myDDU->toggleL1Cal();
		break;

	case (2): // Reset the DDUFPGA
		myDDU->reset(emu::fed::DDUFPGA);
		break;

	case (3): // Load KillFiber
		myDDU->ddu_loadkillfiber(uploadValue);
		break;

	case (4): // Load BXorbit
		myDDU->ddu_loadbxorbit(uploadValue);
		break;

	case (5): // Send fake L1A
		myDDU->vmeL1A();
		break;

	case (6): // Load STFU register
		unsigned int fmm = myDDU->readFMMReg();
		myDDU->writeFMMReg( 0XFED0 | fmm );
		break;

	case (7): // InFPGA reset
		myDDU->reset(emu::fed::INFPGA0);
		break;

	case (8): // Another InFPGA reset.  Not needed?
		myDDU->reset(emu::fed::INFPGA1);
		break;

	case (9): // Write the inreg
		myDDU->writeInputReg(uploadValue);
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
		scale = myDDU->readFakeL1Reg();
		if (scale  & 0x0007) {
			uploadValue = 0xF0F0;
		} else {
			uploadValue = 0x8787;
		}
		// No break...  I know what I am doing.

	case (13): // Write fake L1A register value?
		myDDU->writeFakeL1Reg(uploadValue);
		break;

	case (14): // Write the flash killFiber
		myDDU->writeFlashKillFiber(uploadValue);
		break;

	case (15): // SPECIAL CASE:  write GbEFIFOThresholds
		myDDU->writeFlashGbEFIFOThresholds(otherValue, (uploadValue & 0xffff0000) >> 16, (uploadValue & 0xffff));
		break;

	case (16): // Flash board ID
		myDDU->writeFlashBoardID(uploadValue);
		break;

	case (17): // Flash RUI
		myDDU->writeFlashRUI(uploadValue);
		break;

	case (18): // Load FMM
		myDDU->writeFMMReg( 0XF0E0 | uploadValue );
		break;

	default:
		LOG4CPLUS_ERROR(getApplicationLogger(), "command(" << command << ") not understood.");
		break;
	}

	std::stringstream location;
	location << "crate=" << cgiCrate << "&ddu=" << cgiDDU;
	webRedirect(out,"DDUExpert?"+location.str());

		/*
		if(val==227)myDDU->ddu_l1calonoff();
		if(val==228)myDDU->ddu_reset();
		if(val==222){
			sscanf(XMLtext.data(),"%01hhx%02hhx%02hhx",&myDDU->snd_serial[0],&myDDU->snd_serial[1],&myDDU->snd_serial[2]);
			send_val=((myDDU->snd_serial[0]<<16)&0x000f0000)|((myDDU->snd_serial[1]<<8)&0x0000ff00)|(myDDU->snd_serial[2]&0x0000ff);
			printf(" loadkillfiber send %05lx \n",send_val);
			myDDU->ddu_loadkillfiber(send_val);
		}
		if(val==223){
			sscanf(XMLtext.data(),"%01hhx%02hhx",&myDDU->snd_serial[0],&myDDU->snd_serial[1]);
			para_val=((myDDU->snd_serial[0]<<8)&0x00000f00)|(myDDU->snd_serial[1]&0x0000ff);
			printf(" ldbxorbit send %d (%03xh) \n",para_val,para_val);
			myDDU->ddu_loadbxorbit(para_val);
		}
		if(val==226) {
//		  std::cout << " sending VME L1A to this shitty DDU slot: " << myDDU->slot() << std::endl;
			myDDU->ddu_vmel1a();
			sleep((unsigned int) 2);
		}
		if (val==229) {
			unsigned short int fmmReg = myDDU->vmepara_rd_fmmreg();
			myDDU->vmepara_wr_fmmreg(0xFED0 | (fmmReg & 0xf));
		}
		if(val==321)myDDU->infpga_reset(INFPGA0);
		if(val==421)myDDU->infpga_reset(INFPGA1);
		if(val==508){
			sscanf(XMLtext.data(),"%02hhx%02hhx",&myDDU->snd_serial[0],&myDDU->snd_serial[1]);
			para_val=((myDDU->snd_serial[0]<<8))&0xff00|(myDDU->snd_serial[1]&0x00ff);
			printf(" para_val %04x \n",para_val);
			myDDU->vmepara_wr_inreg(para_val);
		}
		if(val==512){
			sscanf(XMLtext.data(),"%02hhx%02hhx",&myDDU->snd_serial[0],&myDDU->snd_serial[1]);
			para_val=((myDDU->snd_serial[0]<<8))&0xff00|(myDDU->snd_serial[1]&0x00ff);
			printf(" para_val %04x \n",para_val);
			myDDU->vmepara_wr_GbEprescale(para_val);
		}
		if(val==513){
			unsigned short int scale= myDDU->vmepara_rd_GbEprescale();
			if (scale&0x0008) {
				para_val=(scale&0xf7f7)|0x8080;
			} else {
				para_val=(scale|0x0808)&0x7f7f;
			}
			printf("          vmepara_wr_GbEprescale, para_val=%x \n",para_val);
			myDDU->vmepara_wr_GbEprescale(para_val);
		}
		if(val==514){
			sscanf(XMLtext.data(),"%02hhx%02hhx",&myDDU->snd_serial[0],&myDDU->snd_serial[1]);
			para_val=((myDDU->snd_serial[0]<<8))&0xff00|(myDDU->snd_serial[1]&0x00ff);
			printf(" para_val %04x \n",para_val);
			myDDU->vmepara_wr_fakel1reg(para_val);
		}
		if(val==515){
			unsigned short int scale2=myDDU-> vmepara_rd_fakel1reg();
			if (scale2&0x0007) {
				para_val=0xF0F0;
			} else {
				para_val=0x8787;
			}
			printf("         vmepara_wr_fakel1reg, para_val=%x \n",para_val);
			myDDU->vmepara_wr_fakel1reg(para_val);
		}
		if(val==517){
			sscanf(XMLtext.data(),"%02hhx%02hhx",&myDDU->snd_serial[0],&myDDU->snd_serial[1]);
			para_val=((myDDU->snd_serial[0]<<8))&0xff00|(myDDU->snd_serial[1]&0x00ff);
			printf(" para_val %04x \n",para_val);
			myDDU->vmepara_wr_fmmreg(para_val);
		}
		if(val==518){
			unsigned short int scale2=myDDU-> vmepara_rd_fmmreg();
			if ((scale2&0xFFF0)!=0xFED0) {
				para_val=0xFED0;
			} else {
				para_val=0xFED8;  // JRG, also add a Broadcast for this at Top page.
			}
			printf("         vmepara_wr_fmmreg, para_val=%x \n",para_val);
			myDDU->vmepara_wr_fmmreg(para_val);
		}
		if(val==524){
			int i,n_loop=0, ierror=0;
			sscanf(XMLtext.data(),"%d",&n_loop);
			printf(" FMM cycle test, n_loop=%d \n",n_loop);
			for(i=0;i<n_loop;i++){
				para_val=(rand());
				myDDU->vmepara_wr_fmmreg(para_val);
				int rd_in=myDDU->vmepara_rd_fmmreg();
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
			myDDU->vmepara_wr_fmmreg(0xd093);  // Disable Over-ride Mode when done
		}
		if(val==603){
			sscanf(XMLtext.data(),"%02hhx%02hhx",&myDDU->snd_serial[0],&myDDU->snd_serial[1]);
			//  printf(" SSCANF Return: %02x %02x \n",myDDU->snd_serial[0]&0xff,myDDU->snd_serial[1]&0xff);
			//      para_val=((myDDU->snd_serial[0]<<8))&0xff00|(myDDU->snd_serial[1]&0x00ff);
			para_val=myDDU->snd_serial[0]&0xff;
			if(myDDU->snd_serial[1]&0xff)para_val=((myDDU->snd_serial[0]<<8))&0xff00|(myDDU->snd_serial[1]&0x00ff);
			printf(" para_val %04x \n",para_val);
			//myDDU->vmepara_wr_inreg(para_val);
			//myDDU->vmepara_rd_inreg0();
			//myDDU->write_page1();
			//myDDU->writeInputReg(para_val);
			myDDU->writeFlashKillFiber(para_val);
			// sleep(1);
		}
		if(val==604){
			myDDU->snd_serial[5]=0x00;
			sscanf(XMLtext.data(),"%02hhx%02hhx%02hhx%02hhx%02hhx",&myDDU->snd_serial[0],&myDDU->snd_serial[1],&myDDU->snd_serial[2],&myDDU->snd_serial[3],&myDDU->snd_serial[4]);

			para_val=((myDDU->snd_serial[4]))&0x000f|((myDDU->snd_serial[3]<<4)&0x0ff0)|((myDDU->snd_serial[2]<<12)&0xf000);
			printf(" para_val %04x \n",para_val);
			myDDU->vmepara_wr_inreg(para_val);

			para_val=((myDDU->snd_serial[2]>>4))&0x000f|((myDDU->snd_serial[1]<<4)&0x0ff0)|((myDDU->snd_serial[0]<<12)&0xf000);
			printf(" para_val %04x \n",para_val);
			myDDU->vmepara_wr_inreg(para_val);

			para_val=((myDDU->snd_serial[0]>>4))&0x000f;
			printf(" para_val %04x \n",para_val);
			myDDU->vmepara_wr_inreg(para_val);

			myDDU->write_page5();
			// sleep(1);
		}
		if(val==605){
		  int pdec;
			sscanf(XMLtext.data(),"%d",&pdec);
			//&myDDU->snd_serial[0],&myDDU->snd_serial[1]);
			//  printf(" SSCANF Return: %02x %02x \n",myDDU->snd_serial[0]&0xff,myDDU->snd_serial[1]&0xff);
			//  para_val=myDDU->snd_serial[0]&0xff;
			para_val=pdec&0x00000fff;
			// if(myDDU->snd_serial[1]&0xff)para_val=((myDDU->snd_serial[0]<<8))&0xff00|(myDDU->snd_serial[1]&0x00ff);
			printf(" para_val 0x%04x \n",para_val);
			//myDDU->vmepara_wr_inreg(para_val);
			//myDDU->vmepara_rd_inreg0();
			//myDDU->write_page7();
			myDDU->writeFlashBoardID(para_val);
			//      sleep(1);
		}
		if(val==606){
		  int pdec;
		  sscanf(XMLtext.data(),"%d",&pdec);
		//		  &myDDU->snd_serial[0],&myDDU->snd_serial[1]);
		//  printf(" SSCANF Return: %02x %02x \n",myDDU->snd_serial[0]&0xff,myDDU->snd_serial[1]&0xff);
		//			para_val=myDDU->snd_serial[0]&0xff;
			para_val=pdec&0x00007fff;
			// if(myDDU->snd_serial[1]&0xff)para_val=((myDDU->snd_serial[0]<<8))&0xff00|(myDDU->snd_serial[1]&0x00ff);
			printf(" para_val %5d \n",para_val);
			//myDDU->vmepara_wr_inreg(para_val);
			//myDDU->vmepara_rd_inreg0();
			//myDDU->write_page7();
			myDDU->writeFlashRUI(para_val);
			//      sleep(1);
		}

		std::stringstream location;
		location << "crate=" << cgiCrate << "&ddu=" << cgiDDU;
		if(val/100==2){
			//DDU_=ddu;
			webRedirect(out,"DDUExpert?"+location.str());
			//this->DDUDebug(in,out);
		}
		if(val/100==3){
			//DDU_=ddu;
			webRedirect(out,"DDUExpert?"+location.str());
			//this->InFpga(in,out);
		}
		if(val/100==4){
			//DDU_=ddu;
			webRedirect(out,"DDUExpert?"+location.str());
			//this->InFpga(in,out);
		}
		if(val/100==6){
			//DDU_=ddu;
			webRedirect(out,"DDUExpert?"+location.str());
			//this->VMESERI(in,out);
		}
		if(val/100==5){
			//DDU_=ddu;
			webRedirect(out,"DDUExpert?"+location.str());
			//this->VMEPARA(in,out);
		}

	} catch (const std::exception & e ) {
		printf(" exception raised in DDUTextLoad \n");
		//XECPT_RAISE(xgi::exception::Exception, e.what());
	}
		*/
}



void EmuFCrateHyperDAQ::DCCBroadcast(xgi::Input *in, xgi::Output *out)
throw (xgi::exception::Exception)
{
	
	// PGK Patented check-for-initialization
	if (crateVector.size()==0) {
		LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
		return Default(in,out);
	}
	
	cgicc::Cgicc cgi(in);
	
	// First, I need a crate.
	cgicc::form_iterator name = cgi.getElement("crate");
	unsigned int cgiCrate = 0;
	if(name != cgi.getElements().end()) {
		cgiCrate = cgi["crate"]->getIntegerValue();
	}
	emu::fed::FEDCrate *myCrate = crateVector[cgiCrate];
	
	
	std::stringstream sTitle;
	sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DCC Firmware Manager";
	*out << Header(sTitle.str(),false);
	
	
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
	std::map< std::string, enum emu::fed::DEVTYPE > dccPROMTypes;
	dccPROMTypes["INPROM"] = emu::fed::INPROM;
	dccPROMTypes["MPROM"] = emu::fed::MPROM;
	
	// Store the codes to compare them to the FPGAs
	std::map< std::string, std::string > diskPROMCodes;
	
	*out << cgicc::fieldset()
		.set("class","normal") << std::endl;
	*out << cgicc::div("Step 1:  Upload DCC firmware to disk")
		.set("class","legend") << std::endl;
	
	emu::fed::DataTable diskTable("diskTable");
	
	diskTable.addColumn("DCC PROM Name");
	diskTable.addColumn("On-Disk Firmware Version");
	diskTable.addColumn("Upload New Firmware");
	
	// Loop over the prom types and give us a pretty table
	for (unsigned int iprom = 0; iprom < dccPROMNames.size(); iprom++) {
		
		*(diskTable(iprom,0)->value) << dccPROMNames[iprom];
		
		// Get the version number from the on-disk file
		// Best mashup of perl EVER!
		std::ostringstream systemCall;
		std::string diskVersion;
		
		// My perl-fu is 1337, indeed!
		systemCall << "perl -e 'while ($line = <>) { if ($line =~ /SIR 8 TDI \\(fd\\) TDO \\(00\\) ;/) { $line = <>; if ($line =~ /TDI \\((........)\\)/) { print $1; } } }' <Current" << dccPROMNames[iprom] << ".svf >check_ver 2>&1";
		if (!system(systemCall.str().c_str())) {
			std::ifstream pipein("check_ver",std::ios::in);
			getline(pipein,diskVersion);
			pipein.close();
		}
		
		// Now the std::string diskVersion is exactly what is sounds like.
		diskPROMCodes[dccPROMNames[iprom]] = diskVersion;
		
		// Check to make sure the on-disk header looks like it should for that
		//  particular PROM
		std::string diskHeader( diskVersion, 0, 3 );
		if ( diskHeader != "dcc" ) {
			*(diskTable(iprom,1)->value) << "ERROR READING LOCAL FILE -- UPLOAD A NEW FILE";
			diskTable(iprom,1)->setClass("bad");
		} else {
			*(diskTable(iprom,1)->value) << diskVersion;
			diskTable(iprom,1)->setClass("ok");
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
		*(diskTable(iprom,2)->value) << cgicc::form().set("method","POST")
			.set("enctype","multipart/form-data")
			.set("id","Form" + dccPROMNames[iprom])
			.set("action","/" + getApplicationDescriptor()->getURN() + "/DCCLoadBroadcast") << std::endl;
		*(diskTable(iprom,2)->value) << cgicc::input().set("type","file")
			.set("name","File")
			.set("id","File" + dccPROMNames[iprom])
			.set("size","50") << std::endl;
		*(diskTable(iprom,2)->value) << cgicc::input().set("type","button")
			.set("value","Upload SVF")
			.set("onClick","javascript:clickCheck('" + dccPROMNames[iprom] + "')") << std::endl;
		*(diskTable(iprom,2)->value) << cgicc::input().set("type","hidden")
			.set("name","svftype")
			.set("value",dccPROMNames[iprom]) << std::endl;
		*(diskTable(iprom,2)->value) << cgicc::form() << std::endl;
	}
	// Tricky: javascript check!
	*out << "<script type=\"text/javascript\">function clickCheck(id) {" << std::endl;
	*out << "var element = document.getElementById('File' + id);" << std::endl;
	*out << "if (element.value == '') element.style.backgroundColor = '#FFCCCC';" << std::endl;
	*out << "else {" << std::endl;
	*out << "var form = document.getElementById('Form' + id);" << std::endl;
	*out << "form.submit();" << std::endl;
	*out << "}" << std::endl;
	*out << "}</script>" << std::endl;
	
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
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","broadcast")
		.set("id","broadcast")
		.set("value","0");
	
	for (unsigned int iprom = 0; iprom < dccPROMNames.size(); iprom++) {
		*out << cgicc::input()
			.set("type","hidden")
			.set("name",dccPROMNames[iprom])
			.set("value",diskPROMCodes[dccPROMNames[iprom]]) << std::endl;
	}
	
	*out << cgicc::fieldset()
		.set("class","normal") << std::endl;
	*out << cgicc::div("Step 2:  Select the slots you to which you intend to load firmware")
		.set("class","legend") << std::endl;
	
	emu::fed::DataTable slotTable("slotTable");
	
	slotTable.addColumn("Slot number");
	for (unsigned int iprom = 0; iprom < dccPROMNames.size(); iprom++) {
		slotTable.addColumn(dccPROMNames[iprom] + " IDCode");
		slotTable.addColumn(dccPROMNames[iprom] + " UserCode");
	}
	
	
	int idcc = -1;
	std::vector<emu::fed::DCC *> myDCCs = myCrate->getDCCs();
	for (std::vector< emu::fed::DCC * >::iterator iDCC = myDCCs.begin(); iDCC != myDCCs.end(); iDCC++) {
		if ((*iDCC)->slot() >= 21) continue;
		idcc++;
		std::ostringstream bitFlipCommand;
		bitFlipCommand << "Javascript:toggleBit('slots'," << (*iDCC)->slot() << ");";
		
		*(slotTable(idcc,0)->value) << cgicc::input()
			.set("type","checkbox")
			.set("class","slotBox")
			.set("onChange",bitFlipCommand.str()) << " " << (*iDCC)->slot();
		
		for (unsigned int iprom = 0; iprom < dccPROMNames.size(); iprom++) {
			std::ostringstream idCode, userCode;
			
			idCode << std::hex << (*iDCC)->readIDCode(dccPROMTypes[dccPROMNames[iprom]]);
			*(slotTable(idcc,1 + 2*iprom)->value) << idCode.str();
			
			userCode << std::hex << (*iDCC)->readUserCode(dccPROMTypes[dccPROMNames[iprom]]);
			*(slotTable(idcc,2 + 2*iprom)->value) << userCode.str();
			
			// Check for consistency
			slotTable(idcc,1 + 2*iprom)->setClass("ok");
			slotTable(idcc,2 + 2*iprom)->setClass("ok");
			if (diskPROMCodes[dccPROMNames[iprom]] != userCode.str()) {
				slotTable(idcc,2 + 2*iprom)->setClass("bad");
			}
		}
	}
	
	*out << slotTable.toHTML() << std::endl;
	
	*out << cgicc::input()
		.set("type","button")
		.set("value","Toggle Broadcast")
		.set("onClick","javascript:toggleBoxes('slotBox');")
		.set("style","margin-left: 20%; margin-right: auto;");
	
	// Tricky:  javascript toggle of check boxes
	*out << "<script type=\"text/javascript\">" << std::endl;
	*out << "function toggleBoxes(id){var x = getElementsByClassName(id); var f = document.getElementById('broadcast'); if(f.value == 0){f.value = 1; for (i=0,j=x.length;i<j;i++) {x[i].disabled=true;}} else {f.value = 0; for (i=0,j=x.length;i<j;i++) {x[i].disabled=false;}}}" << std::endl;
	*out << "</script>" << std::endl;
	
	*out << cgicc::div()
		.set("style","font-size: 8pt;") << std::endl;
	*out << "Legend: " << cgicc::span("All OK").set("class","ok") << " " << std::endl;
	*out << cgicc::span("Disk/PROM mismatch").set("class","bad") << " " << std::endl;
	
	*out << cgicc::div("DO NOT BROADCAST UNLESS THE CONFIGURATION FILE CONTAINS ALL THE BOARDS PRESENT IN THE CRATE!")
		.set("style","font-size: 8pt;") << std::endl;
	
	*out << cgicc::fieldset() << std::endl;
	
	
	*out << cgicc::fieldset()
		.set("class","normal") << std::endl;
	*out << cgicc::div("Step 3:  Press a button to upload that particular firmware to the selected boards")
		.set("class","legend") << std::endl;
	
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



void EmuFCrateHyperDAQ::DCCLoadBroadcast(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception)
{
	// PGK Patented check-for-initialization
	if (crateVector.size()==0) {
		LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
		return Default(in,out);
	}
	
	cgicc::Cgicc cgi(in);
	
	// First, I need a crate.
	cgicc::form_iterator name = cgi.getElement("crate");
	unsigned int cgiCrate = 0;
	if(name != cgi.getElements().end()) {
		cgiCrate = cgi["crate"]->getIntegerValue();
	}
	//Crate *myCrate = crateVector[cgiCrate];
	
	//printf(" entered DDULoadBroadcast \n");
	
	std::string type = cgi["svftype"]->getValue();
	
	if (type != "INPROM" && type != "MPROM") {
		//std::cout << "I don't understand that PROM type (" << type << ")." << std::endl;
		LOG4CPLUS_ERROR(getApplicationLogger(), "DCCLoadBroadcast: I do not understand the PROM type " << type);
		
		std::ostringstream backLocation;
		backLocation << "DCCBroadcast?crate=" << cgiCrate;
		webRedirect(out,backLocation.str());
		//this->DDUBroadcast(in,out);
		//return;
	}
	
	cgicc::const_file_iterator ifile = cgi.getFile("File");
	if ( (*ifile).getFilename() == "" ) {
		//std::cout << "The file you attempted to upload either doesn't exist, or wasn't properly transferred." << std::endl;
		LOG4CPLUS_ERROR(getApplicationLogger(), "DCCLoadBradcast: The file you attempted to upload either doesn't exist, or wasn't properly transferred");
		
		std::ostringstream backLocation;
		backLocation << "DCCBroadcast?crate=" << cgiCrate;
		webRedirect(out,backLocation.str());
		//this->DDUBroadcast(in,out);
		//return;
	}
	
	std::string filename = "Current" + type + ".svf";
	std::ofstream outfile;
	outfile.open(filename.c_str(),std::ios::trunc);
	if (!outfile.is_open()) {
		LOG4CPLUS_ERROR(getApplicationLogger(), "DCCLoadBroadcast: The file " << filename << " cannot be opened for writing");
		
		std::ostringstream backLocation;
		backLocation << "DDUBroadcast?crate=" << cgiCrate;
		webRedirect(out,backLocation.str());
		//this->DDUBroadcast(in,out);
		//return;
	}
	
	(*ifile).writeToStream(outfile);
	outfile.close();
	
	LOG4CPLUS_DEBUG(getApplicationLogger(), "DCCLoadBroadcast: downloaded and saved " << filename << " of type " << type);
	
	std::ostringstream backLocation;
	backLocation << "DDUBroadcast?crate=" << cgiCrate;
	webRedirect(out,backLocation.str());
	//this->DDUBroadcast(in,out);
	
}


void EmuFCrateHyperDAQ::DCCSendBroadcast(xgi::Input *in, xgi::Output *out)
throw (xgi::exception::Exception)
{
	
	// PGK Patented check-for-initialization
	if (crateVector.size()==0) {
		LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
		return Default(in,out);
	}
	
	cgicc::Cgicc cgi(in);
	
	// First, I need a crate.
	cgicc::form_iterator name = cgi.getElement("crate");
	unsigned int cgiCrate = 0;
	if(name != cgi.getElements().end()) {
		cgiCrate = cgi["crate"]->getIntegerValue();
	}
	emu::fed::FEDCrate *myCrate = crateVector[cgiCrate];
	
	//printf(" entered DDUSendBroadcast \n");
	
	// unsigned int tidcode[8] = {0x2124a093,0x31266093,0x31266093,0x05036093,0x05036093,0x05036093,0x05036093,0x05036093};
	// unsigned int tuscode[8] = {0xcf043a02,0xdf025a02,0xdf025a02,0xb0020a04,0xc043dd99,0xc143dd99,0xd0025a02,0xd1025a02};
	
	int type = -1;
	std::string submitCommand = cgi["submit"]->getValue();
	if (submitCommand.substr(5) == "INPROM") type = 0;
	if (submitCommand.substr(5) == "MPROM") type = 1;
	
	int broadcast = cgi["broadcast"]->getIntegerValue();
	std::string slotsText = cgi["slots"]->getValue();
	if (slotsText.substr(0,2) == "0x") slotsText = slotsText.substr(2);
	unsigned int slots = 0;
	sscanf(slotsText.data(),"%4x",&slots);
	
	if (type == 1 && broadcast) {
		LOG4CPLUS_ERROR(getApplicationLogger(),"Cannot broadcast MPROM firmware");
		std::ostringstream backLocation;
		backLocation << "DCCBroadcast?crate=" << cgiCrate;
		webRedirect(out,backLocation.str());
	}
	
	// Error:  no slots to load.
	if (!broadcast && !slots) {
		LOG4CPLUS_ERROR(getApplicationLogger(),"No slots selected for firmware loading, and broadcast not set");
		std::ostringstream backLocation;
		backLocation << "DCCBroadcast?crate=" << cgiCrate;
		webRedirect(out,backLocation.str());
	}
	
	if (type != 0 && type != 1) {
		//std::cout << "I don't understand that PROM type (" << type << ")." << std::endl;
		LOG4CPLUS_ERROR(getApplicationLogger(),"PROM type not understood");
		std::ostringstream backLocation;
		backLocation << "DCCBroadcast?crate=" << cgiCrate;
		webRedirect(out,backLocation.str());
		//this->DDUBroadcast(in,out);
		//return;
	}
	
	std::string promName[2] = {"INPROM","MPROM"};
	enum emu::fed::DEVTYPE devType[2] = {emu::fed::INPROM,emu::fed::RESET};
	
	// Load the proper version types from the cgi handle.
	std::string version[2];
	for (int i=0; i<=2; i++) {
		version[i] = cgi[promName[i].c_str()]->getValue();
		//std::cout << " version on disk: " << promName[i] << " " << version[i] << std::endl;
	}
	
	//std::cout << " From type " << type << ", broadcasting ";
	//for (int i=from; i<=to; i++) {
	//	std::cout << promName[i] << " ";
	//}
	//std::cout << std::endl;
	
	// I thought this was not allowed!
	//int bnstore[myCrate->getDCCs().size()];
	for (unsigned int iBoard=0; iBoard < myCrate->getDCCs().size(); iBoard++) {
		if (!broadcast && !(slots & (1 << myCrate->getDCCs()[iBoard]->slot()))) continue;
		else if (broadcast && !myCrate->getDCCs()[iBoard]->isBroadcast()) continue;
		
		emu::fed::DCC *myDCC = myCrate->getDCCs()[iBoard];
		//std::cout << "Sending to slot " << myDDU->slot() << std::endl;
		

		LOG4CPLUS_DEBUG(getApplicationLogger(),"Loading firmware to prom " << type << "...");
		
		std::string filename = "Current" + promName[type] + ".svf";
		//std::cout << " broadcasting " << filename << " version " << version[i] << std::endl;
		LOG4CPLUS_INFO(getApplicationLogger(),"Loading server file " << filename << " (v " << std::hex << version[type] << std::dec << ") to DCC slot " << myDCC->slot() << "...");
		
		LOG4CPLUS_DEBUG(getApplicationLogger(),"Step 1 of 1: Load all firmware data...");
		//myDCC->epromload((char *)promName[type].c_str(),devType[type],(char *)filename.c_str(),1);
		myDCC->loadPROMAdvanced(devType[type],(char *) filename.c_str());

		LOG4CPLUS_INFO(getApplicationLogger(),"Loading of server file " << filename << " (v " << std::hex << version[type] << std::dec << ") to DCC slot " << myDCC->slot() << " complete.");
		
	}
	LOG4CPLUS_INFO(getApplicationLogger(),"All firmware loading operations complete, checking PROM ids...");
	
	
	for (unsigned int dcc=0; dcc<myCrate->getDCCs().size(); dcc++) { // loop over boards
		if (!broadcast && !(slots & (1 << myCrate->getDCCs()[dcc]->slot()))) continue;
		else if (broadcast && myCrate->getDCCs()[dcc]->isBroadcast()) continue;
		emu::fed::DCC *myDCC = myCrate->getDCCs()[dcc];
		
		std::ostringstream versionCheck;
		versionCheck << myDCC->readUserCodeAdvanced(devType[type]);
		
		if (version[type] == versionCheck.str()) {
			LOG4CPLUS_INFO(getApplicationLogger(),"DCC slot " << myDCC->slot() << " PROM " << promName[type] << " versions match ("<< std::hex << version[type] << std::dec <<")");
		} else {
			LOG4CPLUS_ERROR(getApplicationLogger(),"DCC slot " << myDCC->slot() << " PROM " << promName[type] << " version mismatch (shows "<< std::hex << versionCheck.str() << std::dec << ", should be "<< std::hex << version[type] << std::dec << ")");
		}
	}
	
	std::ostringstream backLocation;
	backLocation << "DCCBroadcast?crate=" << cgiCrate;
	webRedirect(out,backLocation.str());
	//this->DDUBroadcast(in,out);
}


/*
void EmuFCrateHyperDAQ::DCCFirmware(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{


	// PGK Patented check-for-initialization
	if (crateVector.size()==0) {
		LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
		return Default(in,out);
	}

	cgicc::Cgicc cgi(in);
	
	// First, I need a crate.
	cgicc::form_iterator name = cgi.getElement("crate");
	unsigned int cgiCrate = 0;
	if(name != cgi.getElements().end()) {
		cgiCrate = cgi["crate"]->getIntegerValue();
	}
	emu::fed::FEDCrate *myCrate = crateVector[cgiCrate];

	name = cgi.getElement("dcc");
	unsigned int cgiDCC = 0;
	if (name != cgi.getElements().end()) {
		cgiDCC = cgi["dcc"]->getIntegerValue();
		//std::cout << "DDU inside " << ddu << std::endl;
	}
	emu::fed::DCC *myDCC = myCrate->getDCCs()[cgiDCC];

	unsigned long int idcode,uscode;
	unsigned long int tidcode[2]={0x05035093,0xf5059093};
	unsigned long int tuscode[2]={0xdcc30081,0xdcc310A8};
	//
	//    printf(" initialize env \n");
	//
	const cgicc::CgiEnvironment& env = cgi.getEnvironment();
	//
	printf(" getQueryString \n");
	std::string crateStr = env.getQueryString() ;
	//
	std::cout << crateStr << std::endl ;

	//
	*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
	//
	*out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
	*out << cgicc::title("DCC Firmware Form") << std::endl;
	//
	*out << cgicc::body().set("background","/tmp/bgndcms.jpg") << std::endl;
	*out << cgicc::div().set("style","font-size: 16pt; font-weight: bold; color: #D00; width: 400px; margin-left: auto; margin-right: auto; text-align: center;") << "Crate " << myCrate->number() << " Selected" << cgicc::div() << std::endl;

	char buf[300] ;
	sprintf(buf,"DCC Firmware Slot %d",myDCC->slot());
	//
	*out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
	*out << std::endl;
	//
	*out << cgicc::legend(buf).set("style","color:blue") << std::endl ;

	//
	for(int i=0;i<2;i++){
		// *out << cgicc::table().set("border","0").set("rules","none").set("frame","void");
		// *out << cgicc::tr();
		printf(" LOOP: %d \n",i);
		*out << cgicc::span().set("style","color:black");
		if(myDCC->slot()>21){
			if(i==0){sprintf(buf,"inprom  ");}
			if(i==1){sprintf(buf,"mprom  ");}
			printf(" %s ",buf);
			*out<< buf << std::endl;;
		}
		if(myDCC->slot()<=21){
			if(i==0){idcode=myDCC->inprom_chipid(); sprintf(buf,"inprom  ");}
			if(i==1){idcode=myDCC->mprom_chipid(); sprintf(buf,"mprom  ");}
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
			//myCrate->getVMEController()->CAEN_err_reset();
			if(i==0){uscode=myDCC->inprom_userid();}
			if(i==1){uscode=myDCC->mprom_userid();}
			*out << cgicc::span().set("style","color:black");
			sprintf(buf," usr: ");
			*out << buf;*out << cgicc::span();
			printf(" uscode %08lx \n",uscode);
			//if(myCrate->getVMEController()->CAEN_err()!=0){
				// *out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
			//    }else if(uscode!=tuscode[i]){
			//}else
			if(0xffff0000&uscode!=0xffff0000&tuscode[i]){
				*out << cgicc::span().set("style","color:red;background-color:#dddddd;");
			}else{
				*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
			}
			sprintf(buf,"%08lX",uscode);
			*out << buf ;*out << cgicc::span() << std::endl;
		}
		*out << cgicc::br() << std::endl;
		printf(" now boxes \n");
		if(i<2){
			std::string dccloadfirmware = toolbox::toString("/%s/DCCLoadFirmware",getApplicationDescriptor()->getURN().c_str());
			//
			*out << cgicc::form().set("method","POST")
				.set("enctype","multipart/form-data")
				.set("action",dccloadfirmware) << std::endl;;
			//	.set("enctype","multipart/form-data")
			//
			*out << cgicc::input().set("type","file")
				.set("name","DCCLoadSVF")
				.set("size","50") << std::endl;
			//
			*out << cgicc::input().set("type","submit").set("value","LoadSVF") << std::endl;
			sprintf(buf,"%d",cgiDCC);
			*out << cgicc::input().set("type","hidden").set("value",buf).set("name","dcc") << std::endl;
			sprintf(buf,"%d",i);
			*out << cgicc::input().set("type","hidden").set("value",buf).set("name","prom") << std::endl;
			*out << cgicc::form() << std::endl ;
			std::string dccfirmwarereset = toolbox::toString("/%s/DCCFirmwareReset",getApplicationDescriptor()->getURN().c_str());
			*out << cgicc::form().set("method","GET").set("action",dccfirmwarereset) << std::endl;
			if(i==0){
				*out << cgicc::input().set("type","submit").set("value","Reset InFPGA")<<std::endl;
			}else{
				*out << cgicc::input().set("type","submit").set("value","Reset MFPGA")<<std::endl;
			}
			sprintf(buf,"%d",cgiDCC);
			*out << cgicc::input().set("type","hidden").set("value",buf).set("name","dcc") << std::endl;
			sprintf(buf,"%d",i);
			*out << cgicc::input().set("type","hidden").set("value",buf).set("name","val") << std::endl;
			*out << cgicc::form() << std::endl;
		}else{
			*out << std::endl;
		}
		// *out << cgicc::tr() << std::endl;
		// *out << cgicc::table() << std::endl;

		*out << cgicc::br() << std::endl;
	}
	*out << cgicc::legend() << std::endl;
	*out << cgicc::fieldset()<< std::endl;
	*out << cgicc::body() << std::endl;
	*out << cgicc::html() << std::endl;
}
*/

/*
void EmuFCrateHyperDAQ::DCCLoadFirmware(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	try {
		// PGK Patented check-for-initialization
		if (crateVector.size()==0) {
			LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
			return Default(in,out);
		}

		cgicc::Cgicc cgi(in);
		
		// First, I need a crate.
		cgicc::form_iterator name = cgi.getElement("crate");
		unsigned int cgiCrate = 0;
		if(name != cgi.getElements().end()) {
			cgiCrate = cgi["crate"]->getIntegerValue();
		}
		emu::fed::FEDCrate *myCrate = crateVector[cgiCrate];

		name = cgi.getElement("dcc");
		unsigned int cgiDCC = 0;
		if (name != cgi.getElements().end()) {
			cgiDCC = cgi["dcc"]->getIntegerValue();
			//std::cout << "DDU inside " << ddu << std::endl;
		}
		emu::fed::DCC *myDCC = myCrate->getDCCs()[cgiDCC];
		
		int prom;
		cgicc::form_iterator name2 = cgi.getElement("prom");
		//
		if(name2 != cgi.getElements().end()) {
			prom = cgi["prom"]->getIntegerValue();
			std::cout << "PROM " << prom << std::endl;
		}
		//
		printf(" Entered DCCLoadFirmware prom %d \n",prom);
		//
		//
		std::cout << "UploadConfFileUpload" << std::endl ;
		//


		cgicc::const_file_iterator file = cgi.getFile("DCCLoadSVF");

		//
		std::cout << "GetFiles" << std::endl ;
		//

		if(file != cgi.getFiles().end()) {
			std::string XMLname = (*file).getFilename();
			std::cout <<"SVF FILE: " << XMLname  << std::endl ;

			std::ofstream TextFile ;
			TextFile.open("MySVFFile.svf");
			(*file).writeToStream(TextFile);
			TextFile.close();
		}
//  char buf[400];
// FILE *dwnfp;
// dwnfp    = fopen("MySVFFile.svf","r");
// while (fgets(buf,256,dwnfp) != NULL)printf("%s",buf);
// fclose(dwnfp);
		char *cbrdnum;
		printf(" DCC epromload %d \n",prom);
		cbrdnum=(char*)malloc(5);
		cbrdnum[0]=0x00;cbrdnum[1]=0x00;cbrdnum[2]=0x00;cbrdnum[3]=0x00;
		if(prom==1)myDCC->epromload("MPROM",emu::fed::RESET,"MySVFFile.svf",1,cbrdnum);
		if(prom==0)myDCC->epromload("INPROM",emu::fed::INPROM,"MySVFFile.svf",1,cbrdnum);
		in=NULL;

		std::ostringstream backLocation;
		backLocation << "DCCFirmware?crate=" << cgiCrate << "&dcc=" << cgiDCC;
		webRedirect(out,backLocation.str());
		//this->DCCFirmware(in,out);
	} catch (const std::exception & e ) {
		printf(" exception raised in DCCLoadFirmware \n");
		//XECPT_RAISE(xgi::exception::Exception, e.what());
	}
}
*/


void EmuFCrateHyperDAQ::LoadXMLconf(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	printf(" enter: LoadXMLconf \n");
	cgicc::Cgicc cgi(in);
	//    const CgiEnvironment& env = cgi.getEnvironment();
	//    std::string URLname = cgi["DefURL"]->getValue() ;
	//    std::cout << "  should go to this URL: " << URLname  << std::endl ;
	// Here's how you clear them:
	crateVector.clear();
	//dduVector.clear();
	//dccVector.clear();
	// It doesn't seem to work, though (PGK)
	//printf(" dduVector.size() %d \n",dduVector.size());
	//printf(" dccVector.size() %d \n",dccVector.size());
	//    dduVector.size()=0;  // How to set these to zero?!?
	//    dccVector.size()=0;  // Then go to   this->Default(in.out);
	//reload=1;

	std::string loadxmlconf = toolbox::toString("/%s/",getApplicationDescriptor()->getURN().c_str());
	//    std::cout<<std::endl<<"  JRGdebug: LoadXMLconf std::string="<<loadxmlconf<<std::endl;
	webRedirect(out,"mainPage");
	//mainPage(in,out);
}



void EmuFCrateHyperDAQ::DCCDebug(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	// PGK Patented check-for-initialization
	if (crateVector.size()==0) {
		LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
		return Default(in,out);
	}

	cgicc::Cgicc cgi(in);
	
	// First, I need a crate.
	cgicc::form_iterator name = cgi.getElement("crate");
	unsigned int cgiCrate = 0;
	if(name != cgi.getElements().end()) {
		cgiCrate = cgi["crate"]->getIntegerValue();
	}
	emu::fed::FEDCrate *myCrate = crateVector[cgiCrate];

	name = cgi.getElement("dcc");
	unsigned int cgiDCC = 0;
	if (name != cgi.getElements().end()) {
		cgiDCC = cgi["dcc"]->getIntegerValue();
		//std::cout << "DDU inside " << ddu << std::endl;
	}
	emu::fed::DCC *myDCC = myCrate->getDCCs()[cgiDCC];

	std::stringstream sTitle;
	sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DCC Debugging Information (Crate " << myCrate->number() << " Slot #" << myDCC->slot() << ")";
	*out << Header(sTitle.str(),false);

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

	emu::fed::DataTable generalTable("generalTable");

	generalTable.addColumn("Register");
	generalTable.addColumn("Value");
	generalTable.addColumn("Decoded Status");

	*(generalTable(0,0)->value) << "DCC FMM Status (4-bit)";
	unsigned long int dccValue = myDCC->readStatusHighAdvanced();
	*(generalTable(0,1)->value) << std::showbase << std::hex << ((dccValue & 0xf000) >> 12);
	std::map<std::string, std::string> debugMap = emu::fed::DCCDebugger::FMMStat((dccValue & 0xf000) >> 12);
	for (std::map<std::string, std::string>::iterator iDebug = debugMap.begin(); iDebug != debugMap.end(); iDebug++) {
		*(generalTable(0,2)->value) << cgicc::div(iDebug->first)
			.set("class",iDebug->second);
	}
	if (((dccValue & 0xf000) >> 12) == 0x3) {
		generalTable(0,1)->setClass("bad");
	} else if (((dccValue & 0xf000) >> 12) == 0x1) {
		generalTable(0,1)->setClass("warning");
	} else if (((dccValue & 0xf000) >> 12) == 0x4) {
		generalTable(0,1)->setClass("caution");
	} else if (((dccValue & 0xf000) >> 12) == 0x8) {
		generalTable(0,1)->setClass("bad");
	} else {
		generalTable(0,1)->setClass("ok");
	}

	*(generalTable(1,0)->value) << "DCC FIFO Backpressure (8-bit)";
	*(generalTable(1,1)->value) << std::showbase << std::hex << ((dccValue & 0x0ff0) >> 4);
	for (int iFifo = 0; iFifo < 8; iFifo++) {
		if (!(((dccValue & 0x0ff0) >> 4) & (1<<iFifo))) {
			if (iFifo < 4) {
				*(generalTable(1,2)->value) << cgicc::div()
					.set("class","red") << "FIFO " << (iFifo*2+1) << " or " << (iFifo*2+2) << " (Slot " << myDCC->getDDUSlotFromFIFO(iFifo*2) << " or " << myDCC->getDDUSlotFromFIFO(iFifo*2+1) << ")" << cgicc::div();
			} else if (iFifo == 7) {
				*(generalTable(1,2)->value) << cgicc::div()
					.set("class","red") << "FIFO 9 or 10 (Slot " << myDCC->getDDUSlotFromFIFO(8) << " or " << myDCC->getDDUSlotFromFIFO(9) << ")" << cgicc::div();
			} else {
				*(generalTable(1,2)->value) << cgicc::div()
					.set("class","red") << "Output FIFO to S-Link " << (iFifo/5 + 1) << cgicc::div();
			}
		}
	}
	if (((dccValue & 0x0ff0) >> 4) != 0xbf) {
		generalTable(1,1)->setClass("bad");
	} else {
		generalTable(1,1)->setClass("ok");
	}

	*(generalTable(2,0)->value) << "DCC S-Link Status (4-bit)";
	*(generalTable(2,1)->value) << std::showbase << std::hex << (dccValue & 0xf);
	debugMap = emu::fed::DCCDebugger::SLinkStat(dccValue & 0xf);
	for (std::map<std::string, std::string>::iterator iDebug = debugMap.begin(); iDebug != debugMap.end(); iDebug++) {
		*(generalTable(2,2)->value) << cgicc::div(iDebug->first)
			.set("class",iDebug->second);
	}
	if (!(dccValue & 0x8) || !(dccValue & 0x2)) {
		generalTable(2,1)->setClass("caution");
	}
	if (!(dccValue & 0x4) || !(dccValue & 0x1)) {
		generalTable(2,1)->setClass("bad");
	}

	*(generalTable(3,0)->value) << "DCC L1A Count (16-bit)";
	*(generalTable(3,1)->value) << myDCC->readStatusLowAdvanced();
	generalTable(3,1)->setClass("none");

	*out << generalTable.printSummary() << std::endl;

	// Display only if there are errors.
	if (generalTable.countClass("ok") + generalTable.countClass("none") == generalTable.countRows()) generalTable.setHidden(true);
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

	emu::fed::DataTable ratesTable("ratesTable");

	ratesTable.addColumn("Register");
	ratesTable.addColumn("Value");
	ratesTable.addColumn("Decoded Status");

	*(ratesTable(0,0)->value) << "Input FIFOs Used";
	dccValue = myDCC->readFIFOInUseAdvanced();
	*(ratesTable(0,1)->value) << std::showbase << std::hex << dccValue;
	for (int iFifo = 0; iFifo < 10; iFifo++) {
		if (dccValue & (1<<iFifo)) {
			*(ratesTable(0,2)->value) << cgicc::div()
				.set("class","none") << "FIFO " << (iFifo+1) << " (Slot " << myDCC->getDDUSlotFromFIFO(iFifo) << ")" << cgicc::div();
		}
	}
	ratesTable(0,1)->setClass("none");

	// Get rates for each FIFO, Slink1 first.
	for (int iFifo = 0; iFifo < 5; iFifo++) {
		*(ratesTable(iFifo+1,0)->value) << "FIFO " << (iFifo+1) << " (DDU Slot " << myDCC->getDDUSlotFromFIFO(iFifo) << ")";
		dccValue = myDCC->readRateAdvanced(iFifo+1);
		*(ratesTable(iFifo+1,1)->value) << std::dec << dccValue << " bytes/s";
		ratesTable(iFifo+1,1)->setClass("none");
	}

	*(ratesTable(6,0)->value) << "S-Link 1";
	dccValue = myDCC->readRateAdvanced(0);
	*(ratesTable(6,1)->value) << std::dec << dccValue << " bytes/s";
	ratesTable(6,1)->setClass("none");

	// Get rates for each FIFO, Slink2 second.
	for (int iFifo = 5; iFifo < 10; iFifo++) {
		*(ratesTable(iFifo+2,0)->value) << "FIFO " << (iFifo+1) << " (DDU Slot " << myDCC->getDDUSlotFromFIFO(iFifo) << ")";
		dccValue = myDCC->readRateAdvanced(iFifo+2);
		*(ratesTable(iFifo+2,1)->value) << std::dec << dccValue << " bytes/s";
		ratesTable(iFifo+2,1)->setClass("none");
	}

	*(ratesTable(12,0)->value) << "S-Link 2";
	dccValue = myDCC->readRateAdvanced(6);
	*(ratesTable(12,1)->value) << std::dec << dccValue << " bytes/s";
	ratesTable(12,1)->setClass("none");


	*out << ratesTable.toHTML() << std::endl;

	*out << cgicc::fieldset() << std::endl;

	// Clever trick to help with formating the cut-and-paste.
	*out << cgicc::br()
		.set("style","display: none") << std::endl;
	
	/*
	printf(" enter: DCC Commands \n");
	cgicc::Cgicc cgi(in);
	const CgiEnvironment& env = cgi.getEnvironment();
	std::string crateStr = env.getQueryString();
	std::cout << crateStr << std::endl;
	cgicc::form_iterator name = cgi.getElement("dcc");
	int dcc;
	if(name != cgi.getElements().end()) {
		dcc = cgi["dcc"]->getIntegerValue();
		std::cout << "DCC inside " << dcc << std::endl;
		DCC_ = dcc;
	}else{
		dcc=DCC_;
	}
	thisDCC = dccVector[dcc];
	printf(" DCC %d \n",dcc);
	printf(" set up web page \n");
	*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
	*out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
	*out << cgicc::title("DCC Comands Web Form") << std::endl;
	*out << body().set("background","/tmp/bgndcms.jpg");
	*out << cgicc::div().set("style","font-size: 16pt; font-weight: bold; color: #D00; width: 400px; margin-left: auto; margin-right: auto; text-align: center;") << "Crate " << thisCrate->number() << " Selected" << cgicc::div() << std::endl;

	char buf[300],buf2[300],buf3[300];
	sprintf(buf,"DCC Commands VME  Slot %d",thisDCC->slot());
	*out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
	*out << std::endl;
	*out << cgicc::legend(buf).set("style","color:blue")  << std::endl;
	int igu;
	for(int i=100;i<111;i++){
		thisCrate->getVMEController()->CAEN_err_reset();
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
			std::string dcctextload =
			toolbox::toString("/%s/DCCTextLoad",getApplicationDescriptor()->getURN().c_str());
			*out << cgicc::form().set("method","GET").set("action",dcctextload) << std::endl;
		//	   .set("style","margin-bottom: 0")
		}
		*out << cgicc::span().set("style","color:black");
		*out << buf << cgicc::span();
		if(thisCrate->getVMEController()->CAEN_err()!=0){
			*out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
		}else{
			*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
		}
		*out << buf2;
		*out << cgicc::span();
		if((i>100&&i<107)||i==109||i==110) {
			std::string xmltext="";
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
					.set("style","font-size: 13pt; font-family: arial;")<<std::endl;
			}
			*out << cgicc::input().set("type","submit")
				.set("value","set");
			sprintf(buf,"%d",dcc);
			*out << cgicc::input().set("type","hidden").set("value",buf).set("name","dcc");
			sprintf(buf,"%d",i);
			*out << cgicc::input().set("type","hidden").set("value",buf).set("name","val");
			*out << buf3 << cgicc::form() << std::endl;
		}else{
			*out << cgicc::br() << std::endl;
		}
		if(i==103){
			*out << "<blockquote><font size=-1 face=arial>";
			*out << "DCC-FIFO bits map to DDU-Slots" << cgicc::br();
			*out << " &nbsp &nbsp &nbsp &nbsp b0=Slot3, &nbsp b1=Slot13, &nbsp b2=Slot4, &nbsp b3=Slot12, &nbsp b4=Slot5 &nbsp ---->> Top S-Link" << cgicc::br();
			*out << " &nbsp &nbsp &nbsp &nbsp b5=Slot11, &nbsp b6=Slot6, &nbsp b7=Slot10, &nbsp b8=Slot7, &nbsp b9=Slot9 &nbsp ---->> Bottom S-Link";
			*out << "</font></blockquote>" << std::endl;
		}
		if(i==104){
			*out << "<blockquote><font size=-1 face=arial>";
			*out << "Command Code examples (hex):" << cgicc::br();
			*out << " &nbsp &nbsp &nbsp &nbsp 3=SyncRst, &nbsp 4=PChardRst, &nbsp 1C=SoftRst, &nbsp 34=DDUhardRst";
			*out << "</font></blockquote>" << std::endl;
		}
		if(i==109){
			*out << "<blockquote><font size=-1 face=arial>";
			*out << "bit4 = sw4; bit5 = sw5" << cgicc::br();
			*out << " &nbsp bit0=0&bit9=1: Enable software switch"<<br();
			*out << " &nbsp bitC=1&bitF=0: Set TTCrx NOT ready"<<br();
			*out << " &nbsp bitD=1&bitE=0: Ignore SLINK full, &nbsp bitD=0&bitE=1: Ignore SLINK full and Slink_down";
			*out << "</font></blockquote>" << std::endl;
		}
		if(i==110) {
			*out << "<blockquote><font size=-1 face=arial>";
			*out << " XOR(bit4,bit5): Enable FMM overwrite";
			*out << " &nbsp FMM[3:0]=bit[3:0]";
			*out << "</font></blockquote>" << std::endl;
		}

	}

	*out << cgicc::fieldset() << std::endl;
	*out << cgicc::body() << std::endl;
	*out << cgicc::html() << std::endl;
	*/
	*out << Footer() << std::endl;
}



void EmuFCrateHyperDAQ::DCCExpert(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	// PGK Patented check-for-initialization
	if (crateVector.size()==0) {
		LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
		return Default(in,out);
	}

	cgicc::Cgicc cgi(in);
	
	// First, I need a crate.
	cgicc::form_iterator name = cgi.getElement("crate");
	unsigned int cgiCrate = 0;
	if(name != cgi.getElements().end()) {
		cgiCrate = cgi["crate"]->getIntegerValue();
	}
	emu::fed::FEDCrate *myCrate = crateVector[cgiCrate];

	name = cgi.getElement("dcc");
	unsigned int cgiDCC = 0;
	if (name != cgi.getElements().end()) {
		cgiDCC = cgi["dcc"]->getIntegerValue();
		//std::cout << "DDU inside " << ddu << std::endl;
	}
	emu::fed::DCC *myDCC = myCrate->getDCCs()[cgiDCC];

	std::stringstream sTitle;
	sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DCC Expert Controls (Crate " << myCrate->number() << " Slot #" << myDCC->slot() << ")";
	*out << Header(sTitle.str(),false);

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
		.set("name","dcc")
		.set("value",dccVal) << std::endl;
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","crate")
		.set("value",crateVal) << std::endl;

	unsigned long int currentFIFOs = myDCC->readFIFOInUseAdvanced();
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
		unsigned int slot = myDCC->getDDUSlotFromFIFO(iFifo);
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
				.set("value","1")
				.set("onChange",swapCommand.str())
				.set("checked","checked") << "Live" << cgicc::br() << std::endl;
			*out << cgicc::input()
				.set("type","radio")
				.set("name",radioName.str())
				.set("value","0")
				.set("onChange",swapCommand.str()) << "Killed" << std::endl;
		} else {
			*out << cgicc::input()
				.set("type","radio")
				.set("name",radioName.str())
				.set("value","1")
				.set("onChange",swapCommand.str()) << "Live" << cgicc::br() << std::endl;
			*out << cgicc::input()
				.set("type","radio")
				.set("name",radioName.str())
				.set("value","0")
				.set("onChange",swapCommand.str())
				.set("checked","checked") << "Killed" << std::endl;
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
		.set("name","dcc")
		.set("value",dccVal) << std::endl;
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","crate")
		.set("value",crateVal) << std::endl;

	unsigned long int currentSwitch = myDCC->readSoftwareSwitchAdvanced();
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
			.set("onChange","javascript:toggleBit('switch',9);clearBit('switch',0);")
			.set("checked","checked") << std::endl;
	} else {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box09")
			.set("onChange","javascript:toggleBit('switch',9);clearBit('switch',0);") << std::endl;
	}
	*out << "Enable software switch" << cgicc::div() << std::endl;

	*out << cgicc::div()
		.set("style","font-size: 8pt;") << std::endl;
	if (!(currentSwitch & (1<<15)) && currentSwitch & (1<<12)) {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box1215")
			.set("onChange","javascript:toggleBit('switch',12);clearBit('switch',15);")
			.set("checked","checked") << std::endl;
	} else {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box1215")
			.set("onChange","javascript:toggleBit('switch',12);clearBit('switch',15);") << std::endl;
	}
	*out << "Set TTCrx NOT ready" << cgicc::div() << std::endl;

	*out << cgicc::div()
		.set("style","font-size: 8pt;") << std::endl;
	if (!(currentSwitch & (1<<14)) && currentSwitch & (1<<13)) {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box1314")
			.set("onChange","javascript:toggleBit('switch',13);clearBit('switch',14);")
			.set("checked","checked") << std::endl;
	} else {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box1314")
			.set("onChange","javascript:toggleBit('switch',13);clearBit('switch',14);") << std::endl;
	}
	*out << "Ignore S-Link full" << cgicc::div() << std::endl;

	*out << cgicc::div()
		.set("style","font-size: 8pt;") << std::endl;
	if (!(currentSwitch & (1<<13)) && currentSwitch & (1<<14)) {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box1413")
			.set("onChange","javascript:toggleBit('switch',14);clearBit('switch',13);")
			.set("checked","checked") << std::endl;
	} else {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box1413")
			.set("onChange","javascript:toggleBit('switch',14);clearBit('switch',13);") << std::endl;
	}
	*out << "Ignore S-Link full and S-Link not present" << cgicc::div() << std::endl;

	*out << cgicc::div()
		.set("style","font-size: 8pt;") << std::endl;
	if (currentSwitch & (1<<4)) {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","boxsw4")
			.set("onChange","javascript:toggleBit('switch',4);")
			.set("checked","checked") << std::endl;
	} else {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","boxsw4")
			.set("onChange","javascript:toggleBit('switch',4);") << std::endl;
	}
	*out << "Set \"SW4\"" << cgicc::div() << std::endl;

	*out << cgicc::div()
		.set("style","font-size: 8pt;") << std::endl;
	if (currentSwitch & (1<<5)) {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","boxsw5")
			.set("onChange","javascript:toggleBit('switch',5);")
			.set("checked","checked") << std::endl;
	} else {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","boxsw5")
			.set("onChange","javascript:toggleBit('switch',5);") << std::endl;
	}
	*out << "Set \"SW5\"" << cgicc::div() << std::endl;
	
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
		.set("name","dcc")
		.set("value",dccVal) << std::endl;
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","crate")
		.set("value",crateVal) << std::endl;

	unsigned long int currentFMM = myDCC->readFMMAdvanced();
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
			.set("onChange","javascript:toggleBit('fmm',4);")
			.set("checked","checked") << std::endl;
	} else {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box4x5")
			.set("onChange","javascript:toggleBit('fmm',4);") << std::endl;
	}
	*out << "Enable user override" << cgicc::div() << std::endl;

	*out << cgicc::div()
		.set("style","font-size: 8pt;") << std::endl;
	if (currentFMM & (1<<0)) {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box0")
			.set("onChange","javascript:toggleBit('fmm',0);")
			.set("checked","checked") << std::endl;
	} else {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box0")
			.set("onChange","javascript:toggleBit('fmm',0);") << std::endl;
	}
	*out << "Set FMM bit 0 (Busy)" << cgicc::div() << std::endl;

	*out << cgicc::div()
		.set("style","font-size: 8pt;") << std::endl;
	if (currentFMM & (1<<1)) {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box1")
			.set("onChange","javascript:toggleBit('fmm',1);")
			.set("checked","checked") << std::endl;
	} else {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box1")
			.set("onChange","javascript:toggleBit('fmm',1);") << std::endl;
	}
	*out << "Set FMM bit 1 (Ready)" << cgicc::div() << std::endl;

	*out << cgicc::div()
		.set("style","font-size: 8pt;") << std::endl;
	if (currentFMM & (1<<2)) {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box2")
			.set("onChange","javascript:toggleBit('fmm',2);")
			.set("checked","checked") << std::endl;
	} else {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box2")
			.set("onChange","javascript:toggleBit('fmm',2);") << std::endl;
	}
	*out << "Set FMM bit 2 (Warning)" << cgicc::div() << std::endl;

	*out << cgicc::div()
		.set("style","font-size: 8pt;") << std::endl;
	if (currentFMM & (1<<3)) {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box3")
			.set("onChange","javascript:toggleBit('fmm',3);")
			.set("checked","checked") << std::endl;
	} else {
		*out << cgicc::input()
			.set("type","checkbox")
			.set("name","box3")
			.set("onChange","javascript:toggleBit('fmm',3);") << std::endl;
	}
	*out << "Set FMM bit 3 (Out-of-Sync)" << cgicc::div() << std::endl;
	
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
		.set("name","dcc")
		.set("value",dccVal) << std::endl;
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","crate")
		.set("value",crateVal) << std::endl;

	unsigned long int currentTTCCommand = myDCC->readTTCCommandAdvanced();
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
		.set("name","dcc")
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
		.set("name","dcc")
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
		.set("name","dcc")
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
		.set("name","dcc")
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
		.set("name","dcc")
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
		.set("name","dcc")
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
}



void EmuFCrateHyperDAQ::DCCTextLoad(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	// try {
	
	// PGK Patented check-for-initialization
	if (crateVector.size()==0) {
		LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
		return Default(in,out);
	}

	cgicc::Cgicc cgi(in);

	// First, I need a crate.
	cgicc::form_iterator name = cgi.getElement("crate");
	unsigned int cgiCrate = 0;
	if(name != cgi.getElements().end()) {
		cgiCrate = cgi["crate"]->getIntegerValue();
	}
	emu::fed::FEDCrate *myCrate = crateVector[cgiCrate];

	name = cgi.getElement("dcc");
	unsigned int cgiDCC = 0;
	if (name != cgi.getElements().end()) {
		cgiDCC = cgi["dcc"]->getIntegerValue();
	}
	emu::fed::DCC *myDCC = myCrate->getDCCs()[cgiDCC];

	// The command that is intended to be sent to the DDU.
	unsigned int command = 0;
	name = cgi.getElement("command");
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
	
	// check for errors.
	if ((uploadStream.fail() || !uploadStream.eof()) && XMLText != "") {
		LOG4CPLUS_ERROR(getApplicationLogger(), "DDUTextLoad does not understand XMLText(" << XMLText << ")");
		std::stringstream location;
		location << "crate=" << cgiCrate << "&dcc=" << cgiDCC;
		webRedirect(out,"DCCExpert?"+location.str());
	}


	LOG4CPLUS_DEBUG(getApplicationLogger(),"Attempting DDUTextLoad with crate(" << cgiCrate << ") dcc(" << cgiDCC << ") == slot(" << myDCC->slot() << ") command(" << command << ") XMLText(" << XMLText << ") == uploadValue(" << uploadValue << ")");


	switch (command) {

	case (1): // reset bx?
		myDCC->resetBXAdvanced();
		break;

	case (2): // reset event count?
		myDCC->resetEventsAdvanced();
		break;

	case (3): // set FIFO in use
		myDCC->writeFIFOInUseAdvanced(uploadValue);
		break;

	case (4): // set TTC command
		myDCC->writeTTCCommandAdvanced(uploadValue);
		break;

	case (5): // set fake L1A rate and number
		myDCC->writeFakeL1AAdvanced(uploadValue);
		break;

	case (6): // set SW Switch
		myDCC->writeSoftwareSwitchAdvanced(uploadValue);
		break;

	case (7): // set FMM register
		myDCC->writeFMMAdvanced(uploadValue);
		break;

	default:

		break;

	}
	
	std::ostringstream backLocation;
	backLocation << "DCCExpert?crate=" << cgiCrate << "&dcc=" << cgiDCC;
	webRedirect(out,backLocation.str());
		
		/*
		if(val==101)myDCC->mctrl_bxr();
		if(val==102)myDCC->mctrl_evnr();
		snd_serial[0]=0;
		snd_serial[1]=0;
		if(val==103){
			//      sscanf(XMLtext.data(),"%04x",&para_val);
			istd::stringstream test(XMLtext);
			test >> hex >> para_val;
			printf(" mctrl_fifouse send %04x \n",para_val);
			myDCC->mctrl_fifoinuse(para_val);
		}
		if(val==104){
			//      istd::stringstream test(XMLtext);
			//	test >> hex >> para_val;
			sscanf(XMLtext.data(),"%x",&para_val);
			printf(" ttc command send %04x \n",para_val);
			myDCC->mctrl_ttccmd(para_val);
		}
		if(val==105){
			istd::stringstream test(XMLtext);
			unsigned int testVal;
			test >> hex >> testVal;
			rate = (testVal & 0xff00) >> 8;
			num = (testVal & 0xff);
			//sscanf(XMLtext.data(),"%02hhx%02hhx",&rate,&num);
			//if(rate>255)rate=255;
			//if(num>255)num=255;
			arate=rate;anum=num;
			printf(" load L1A send rate:%d num:%d \n",arate,anum);
			myDCC->mctrl_fakeL1A(arate,anum);
		}
		if(val==106){
			arate=rate;anum=num;
			printf(" load (no prompt) L1A send rate:%d num:%d \n",arate,anum);
			myDCC->mctrl_fakeL1A(arate,anum);
		}
		if(val==109){
			//      sscanf(XMLtext.data(),"%04x",&para_val);
			istd::stringstream test(XMLtext);
			test >> hex >> para_val;
			printf(" Set switch register %04x \n",para_val);
			myDCC->mctrl_swset(para_val);
		}
		if(val==110){
			//      sscanf(XMLtext.data(),"%04x",&para_val);
			istd::stringstream test(XMLtext);
			test >> hex >> para_val;
			printf(" Set FMM register %04x \n",para_val);
			myDCC->mctrl_fmmset(para_val);
		}

		std::ostringstream backLocation;
		backLocation << "DCCExpert?crate=" << cgiCrate << "&dcc=" << cgiDCC;
		webRedirect(out,backLocation.str());
		//this->DCCDebug(in,out);


	} catch (const std::exception & e ) {
		printf(" exception raised in DCCLoadFirmware \n");
		//XECPT_RAISE(xgi::exception::Exception, e.what());
	}
	*/
}


/*
void EmuFCrateHyperDAQ::DCCFirmwareReset(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	try {
		// PGK Patented check-for-initialization
		if (crateVector.size()==0) {
			LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
			return Default(in,out);
		}

		cgicc::Cgicc cgi(in);
		
		// First, I need a crate.
		cgicc::form_iterator name = cgi.getElement("crate");
		unsigned int cgiCrate = 0;
		if(name != cgi.getElements().end()) {
			cgiCrate = cgi["crate"]->getIntegerValue();
		}
		emu::fed::FEDCrate *myCrate = crateVector[cgiCrate];

		name = cgi.getElement("dcc");
		unsigned int cgiDCC = 0;
		if (name != cgi.getElements().end()) {
			cgiDCC = cgi["dcc"]->getIntegerValue();
			//std::cout << "DDU inside " << ddu << std::endl;
		}
		emu::fed::DCC *myDCC = myCrate->getDCCs()[cgiDCC];
	  
		cgicc::form_iterator name2 = cgi.getElement("val");
		//
		unsigned int val;
		if(name2 != cgi.getElements().end()) {
			val = cgi["val"]->getIntegerValue();
			std::cout << "val" << val << std::endl;
		}
		//
		if(val==0)myDCC->hdrst_in();
		if(val==1)myDCC->hdrst_main();
		unsigned int tim=1;
		sleep(tim);
	  
		std::ostringstream backLocation;
		backLocation << "DCCFirmware?crate=" << cgiCrate << "&dcc=" << cgiDCC;
		webRedirect(out,backLocation.str());
		//this->DCCFirmware(in,out);
	} catch (const std::exception & e ) {
		printf(" exception raised in DCCLoadFirmware \n");
		//XECPT_RAISE(xgi::exception::Exception, e.what());
	}
}
*/


void EmuFCrateHyperDAQ::DDUVoltMon(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	// PGK Patented check-for-initialization
	if (crateVector.size()==0) {
		LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
		return Default(in,out);
	}

	cgicc::Cgicc cgi(in);
	
	// First, I need a crate.
	cgicc::form_iterator name = cgi.getElement("crate");
	unsigned int cgiCrate = 0;
	if(name != cgi.getElements().end()) {
		cgiCrate = cgi["crate"]->getIntegerValue();
	}
	emu::fed::FEDCrate *myCrate = crateVector[cgiCrate];

	float v_val[4] = {1500,2500,2500,3300};
	float v_delt = 0.05;
	float t_val[4] = {80,80,80,80};
	float t_delt = 0.20;

	std::stringstream sTitle;
	sTitle << "EmuFCrateHyperDAQ(" << getApplicationDescriptor()->getInstance() << ") DDU Voltage/Temperature Monitoring";
	*out << Header(sTitle.str());
	
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

		emu::fed::DDU *myDDU = myCrate->getDDUs()[iDDU];
		// I am a DDU!
		// Skip broadcasting
		if (myDDU->isBroadcast()) continue;

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
			voltage[jVolt] = myDDU->readVoltageAdvanced(jVolt);
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
			temp[jTemp] = myDDU->readTemperatureAdvanced(jTemp);
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
}



void EmuFCrateHyperDAQ::webRedirect(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception)
{
	std::string url = in->getenv("PATH_TRANSLATED");

	cgicc::HTTPResponseHeader &header = out->getHTTPResponseHeader();

	header.getStatusCode(303);
	header.getReasonPhrase("See Other");
	header.addHeader("Location",
	url.substr(0, url.find("/" + in->getenv("PATH_INFO"))));
}


/*
void EmuFCrateHyperDAQ::DCCRateMon(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	// PGK Patented check-for-initialization
	if (crateVector.size()==0) {
		LOG4CPLUS_INFO(getApplicationLogger(), "Jumping back to Default for proper initialization...");
		return Default(in,out);
	}

	cgicc::Cgicc cgi(in);
	
	// First, I need a crate.
	cgicc::form_iterator name = cgi.getElement("crate");
	unsigned int cgiCrate = 0;
	if(name != cgi.getElements().end()) {
		cgiCrate = cgi["crate"]->getIntegerValue();
	}
	emu::fed::FEDCrate *myCrate = crateVector[cgiCrate];

	cgicc::CgiEnvironment cgiEnvi(in);
	std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
	//
	*out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"30; URL=/" << getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<std::endl;


	//
	*out << "<HTML>" <<std::endl;
	*out << "<BODY bgcolor=\"#FFFFFF\">" <<std::endl;

	emu::fed::DCC *myDCC = myCrate->getDCCs()[0];
	unsigned short int status[12];

	for(int igu=0;igu<12;igu++) {
		status[igu]=myDCC->mctrl_ratemon(igu);
		DCC_ratemon[DCC_ratemon_cnt][igu]=((status[igu]&0x3fff)<<(((status[igu]>>14)&0x3)*4));
	}
	DCC_ratemon_cnt=DCC_ratemon_cnt+1;
	if(DCC_ratemon_cnt>49)DCC_ratemon_cnt=0;
	for(int igu=0;igu<12;igu++) {
		DCC_ratemon[DCC_ratemon_cnt%50][igu]=0;
		DCC_ratemon[(DCC_ratemon_cnt+1)%50][igu]=0;
	}

	*out << "<OBJECT classid=\"clsid:D27CDB6E-AE6D-11cf-96B8-444553540000\" codebase=\"http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,0,0\" WIDTH=\"565\" HEIGHT=\"420\" id=\"FC_2_3_MSLine\">" <<std::endl;
	*out << "<PARAM NAME=movie VALUE=\"/daq/extern/FusionCharts/Charts/FC_2_3_MSLine.swf\">" <<std::endl;
	std::stringstream output;
	output << "<PARAM NAME=\"FlashVars\" VALUE=\"&dataURL=getDataDCCRate0" << "&chartWidth=565&chartHeight=420"<<"\">"<<std::endl;
	*out << output.str() << std::endl ;
	*out << "<PARAM NAME=quality VALUE=high>" << std::endl ;
	*out << "<PARAM NAME=bgcolor VALUE=#FFFFFF>" << std::endl ;
	//
	std::stringstream output2;
	output2 << "<EMBED src=\"/daq/extern/FusionCharts/Charts/FC_2_3_MSLine.swf\" FlashVars=\"&dataURL=getData"<< "DCCRate0"<<"\" quality=high bgcolor=#FFFFFF WIDTH=\"565\" HEIGHT=\"420\" NAME=\"FC_2_3_MSLine\" TYPE=\"application/x-shockwave-flash\" PLUGINSPAGE=\"http://www.macromedia.com/go/getflashplayer\"></EMBED>" << std::endl;
	//
	*out << output2.str() << std::endl;
	*out << "</OBJECT>" << std::endl;

	*out << "<OBJECT classid=\"clsid:D27CDB6E-AE6D-11cf-96B8-444553540000\" codebase=\"http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,0,0\" WIDTH=\"565\" HEIGHT=\"420\" id=\"FC_2_3_MSLine\">" <<std::endl;
	*out << "<PARAM NAME=movie VALUE=\"/daq/extern/FusionCharts/Charts/FC_2_3_MSLine.swf\">" <<std::endl;
	std::stringstream output3;
	output3 << "<PARAM NAME=\"FlashVars\" VALUE=\"&dataURL=getDataDCCRate1" << "&chartWidth=565&chartHeight=420"<<"\">"<<std::endl;
	*out << output3.str() << std::endl ;
	*out << "<PARAM NAME=quality VALUE=high>" << std::endl ;
	*out << "<PARAM NAME=bgcolor VALUE=#FFFFFF>" << std::endl ;
	//
	std::stringstream output4;
	output4 << "<EMBED src=\"/daq/extern/FusionCharts/Charts/FC_2_3_MSLine.swf\" FlashVars=\"&dataURL=getData"<< "DCCRate1"<<"\" quality=high bgcolor=#FFFFFF WIDTH=\"565\" HEIGHT=\"420\" NAME=\"FC_2_3_MSLine\" TYPE=\"application/x-shockwave-flash\" PLUGINSPAGE=\"http://www.macromedia.com/go/getflashplayer\"></EMBED>" << std::endl;
	//
	*out << output4.str() << std::endl;
	*out << "</OBJECT>" << std::endl;




	*out << "</BODY>" << std::endl;
	*out << "</HTML>" << std::endl;

}
*/

/*
void EmuFCrateHyperDAQ::getDataDCCRate0(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	//
	*out << "<graph caption='DCC S-Link 0' subcaption='' xAxisName='time' yAxisName='Rate' numberPrefix='' showNames='1' animation='0'>" << std::endl;
	//      *out << "<graph caption='DCC FPGA 0' subcaption='' xAxisName='time' yAxisName='Rate' numberPrefix='' showNames='1' animation='0'>" << std::endl;
	//std::cout << "<graph caption='DCC FPGA 0' subcaption='' xAxisName='time' yAxisName='Rate' numberPrefix='' showNames='1' animation='0'>" << std::endl;
	*out << "<categories>" << std::endl;
	//std::cout << "<categories>" << std::endl;
	for(int i=0;i<50;i++){
		*out << "<category name='"<<i<<"' hoverText='' />" << std::endl;
		//std::cout << "<category name='"<<i<<"' hoverText='' />" << std::endl;
	}
	*out << "</categories>" <<std::endl;
	//std::cout << "</categories>" <<std::endl;
	for(int ch=0;ch<6;ch++){
		if(ch==0)*out << "<dataset seriesName='S-Link 0 ' color='0080C0' showValues='0'>" <<std::endl;
		if(ch==1)*out << "<dataset seriesName='Slot 3 DDU ' color='008040' showValues='0'>" <<std::endl;
		if(ch==2)*out << "<dataset seriesName='Slot 13 DDU ' color='800080' showValues='0'>" <<std::endl;
		if(ch==3)*out << "<dataset seriesName='Slot 4 DDU ' color='FF8040' showValues='0'>" <<std::endl;
		if(ch==4)*out << "<dataset seriesName='Slot 12 DDU ' color='FFF000' showValues='0'>" <<std::endl;
		if(ch==5)*out << "<dataset seriesName='Slot 5 DDU ' color='FF0080' showValues='0'>" <<std::endl;
		for(unsigned int i=0;i<50;i++) {
			std::stringstream output;
			// output << "<set name='" << ch <<"'"<< " value='" << DCC_ratemon[i][ch] << "'" << " />" << std::endl;
			output << "<set value='" << DCC_ratemon[i][ch] << "'" << " />" << std::endl;
			*out << output.str() << std::endl ;
		//std::cout << output.str() << std::endl ;
		}
		*out << "</dataset>"<<std::endl;
		//std::cout << "</dataset>"<<std::endl;
	}
	*out << "</graph>" << std::endl;
	//std::cout << "</graph>" << std::endl;
}
*/

/** @note Merge with getDataDCCRate0? **/
/*
void EmuFCrateHyperDAQ::getDataDCCRate1(xgi::Input * in, xgi::Output * out )
	throw (xgi::exception::Exception)
{
	//
	*out << "<graph caption='DCC S-Link 1' subcaption='' xAxisName='time' yAxisName='Rate' numberPrefix='' showNames='1' animation='0'>" << std::endl;
	//      *out << "<graph caption='DCC FPGA 1' subcaption='' xAxisName='time' yAxisName='Rate' numberPrefix='' showNames='1' animation='0'>" << std::endl;
	//std::cout << "<graph caption='DCC FPGA 0' subcaption='' xAxisName='time' yAxisName='Rate' numberPrefix='' showNames='1' animation='0'>" << std::endl;
	*out << "<categories>" << std::endl;
	//std::cout << "<categories>" << std::endl;
	for(int i=0;i<50;i++){
		*out << "<category name='"<<i<<"' hoverText='' />" << std::endl;
		//std::cout << "<category name='"<<i<<"' hoverText='' />" << std::endl;
	}
	*out << "</categories>" <<std::endl;
	//std::cout << "</categories>" <<std::endl;
	for(int ch=6;ch<12;ch++){
		if(ch==6)*out << "<dataset seriesName='S-Link 1 ' color='0080C0' showValues='0'>" <<std::endl;
		if(ch==7)*out << "<dataset seriesName='Slot 11 DDU ' color='008040' showValues='0'>" <<std::endl;
		if(ch==8)*out << "<dataset seriesName='Slot 6 DDU ' color='800080' showValues='0'>" <<std::endl;
		if(ch==9)*out << "<dataset seriesName='Slot 10 DDU ' color='FF8040' showValues='0'>" <<std::endl;
		if(ch==10)*out << "<dataset seriesName=' Slot 7 DDU ' color='FFF000' showValues='0'>" <<std::endl;
		if(ch==11)*out << "<dataset seriesName='DDU Slot 9 DDU ' color='FF0080' showValues='0'>" <<std::endl;
		for(unsigned int i=0;i<50;i++) {
			std::stringstream output;
			// output << "<set name='" << ch <<"'"<< " value='" << DCC_ratemon[i][ch] << "'" << " />" << std::endl;
			output << "<set value='" << DCC_ratemon[i][ch] << "'" << " />" << std::endl;
			*out << output.str() << std::endl ;
			//std::cout << output.str() << std::endl ;
		}
		*out << "</dataset>"<<std::endl;
		//std::cout << "</dataset>"<<std::endl;
	}
	*out << "</graph>" << std::endl;
	//std::cout << "</graph>" << std::endl;
}
*/


void EmuFCrateHyperDAQ::webRedirect(xgi::Output *out ,std::string location)
{
	std::string url = "/" + getApplicationDescriptor()->getURN() + "/" + location;

	cgicc::HTTPResponseHeader &header = out->getHTTPResponseHeader();

	header.getStatusCode(303);
	header.getReasonPhrase("See Other");
	header.addHeader("Location",url);
}



std::string EmuFCrateHyperDAQ::selectACrate(std::string location, std::string what, unsigned int index, unsigned int crateIndex) {
	std::ostringstream *out = new std::ostringstream();
	
	for (unsigned int iCrate = 0; iCrate < crateVector.size(); iCrate++) {
		*out << cgicc::table()
			.set("style","width: 100%; border: 1px solid #000; border-collapse: collapse; text-align: center; font-size: 10pt; margin-bottom: 10px;") << std::endl;
		
		std::ostringstream columns;
		columns << (crateVector[iCrate]->getDDUs().size() + crateVector[iCrate]->getDCCs().size());

		std::string selectedCrate = "";
		if (what == "crate" && index == iCrate) selectedCrate = "background-color: #FF9;";
		
		std::ostringstream crateName;
		crateName << "Crate " << crateVector[iCrate]->number();
		
		// For crate-specific pages, link the crates.
		*out << cgicc::tr() << std::endl;
		*out << cgicc::td()
			.set("style",selectedCrate+" border: 1px solid #000;")
			.set("colspan",columns.str()) << std::endl;

		std::ostringstream locationCrate;
		if (what == "crate") {
			locationCrate << "/" << getApplicationDescriptor()->getURN() << "/" << location << "?crate=" << iCrate;
			*out << cgicc::a(crateName.str())
				.set("href",locationCrate.str()) << std::endl;
		} else {
			*out << crateName.str() << std::endl;
		}
		*out << cgicc::td() << std::endl;
		*out << cgicc::tr() << std::endl;

		// For board-specific pages, link the board.
		*out << cgicc::tr() << std::endl;
		for (unsigned int iDDU = 0; iDDU < crateVector[iCrate]->getDDUs().size(); iDDU++) {

			std::string selectedBoard = "";
			if (what == "ddu" && index == iDDU && crateIndex == iCrate) selectedBoard = "background-color: #FF9;";
			
			std::ostringstream boardName;
			if (!crateVector[iCrate]->getDDUs()[iDDU]->isBroadcast()) {
				boardName << "DDU Slot " << crateVector[iCrate]->getDDUs()[iDDU]->slot() << ": RUI #" << crateVector[iCrate]->getRUI(crateVector[iCrate]->getDDUs()[iDDU]->slot());
			} else {
				//boardName << "DDU BROADCAST";
				continue;
			}
			
			*out << cgicc::td()
				.set("style",selectedBoard+" border: 1px solid #000;") << std::endl;

			std::ostringstream locationBoard;
			if (what == "ddu") {
				locationBoard << "/" << getApplicationDescriptor()->getURN() << "/" << location << "?crate=" << iCrate << "&ddu=" << iDDU;
				*out << cgicc::a(boardName.str())
					.set("href",locationBoard.str()) << std::endl;
			} else {
				*out << boardName.str() << std::endl;
			}
			*out << cgicc::td() << std::endl;
		}
		for (unsigned int iDCC = 0; iDCC < crateVector[iCrate]->getDCCs().size(); iDCC++) {

			std::string selectedBoard = "";
			if (what == "dcc" && index == iDCC && crateIndex == iCrate) selectedBoard = "background-color: #FF9;";
			
			std::ostringstream boardName;
			if (!crateVector[iCrate]->getDCCs()[iDCC]->isBroadcast()) {
				boardName << "DCC Slot " << crateVector[iCrate]->getDCCs()[iDCC]->slot();
			} else {
				//boardName << "BROADCAST DCC";
				continue;
			}
			
			*out << cgicc::td()
				.set("style",selectedBoard+" border: 1px solid #000;") << std::endl;

			std::ostringstream locationBoard;
			if (what == "dcc") {
				locationBoard << "/" << getApplicationDescriptor()->getURN() << "/" << location << "?crate=" << iCrate << "&dcc=" << iDCC;
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


