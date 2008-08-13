
/** XDAQ-based interface for controlling and debugging FED Crates.
*	Please, PLEASE leave the editing of this file to the experts!
*
*	@author Jason Gilmore     <gilmore@mps.ohio-state.edu>
*	@author Jianhui Gu        <gujh@mps.ohio-state.edu>
*	@author Stan Durkin       <durkin@mps.ohio-state.edu>
*	@author Phillip Killewald <paste@mps.ohio-state.edu>
**/

#ifndef __EMUFCRATEHYPERDAQ_H__
#define __EMUFCRATEHYPERDAQ_H__

#include "EmuFEDApplication.h"

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <time.h>
#include <stdio.h>
#include <map>

#include "xdaq/Application.h"
#include "xgi/Utils.h"
#include "xgi/Method.h"
#include "xdata/UnsignedLong.h"
#include "xdata/String.h"

// For SOAP querying of the FCrate process
#include "xdaq/NamespaceURI.h"  // XDAQ_NS_URI
#include "xoap/MessageFactory.h"  // createMessage()
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"


#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include "cgicc/FormFile.h"

// My Stuff

#include "FEDCrateParser.h"
#include "FEDCrate.h"
#include "DDU.h"
#include "DCC.h"
#include "VMEController.h"
#include "JTAG_constants.h"
//#include "IRQThread.h"

#include "EmuFController.h"

//using namespace cgicc;
//using namespace std;

class EmuFCrateHyperDAQ: public EmuFEDApplication
{
private:
	//int reload;
	//  long int timer,xtimer;
	//bool interrupt_set;
	//time_t interrupt_time;
	//long int ltime;
	unsigned long int tidcode[8];
	unsigned long int tuscode[8];
	//
protected:
	//
	xdata::String svfFile_;
	xdata::String xmlFile_;
	//xdata::UnsignedLong myParameter_;
	//TestBeamCrateController tbController; // PGK ??
	//DDU* thisDDU;
	//DCC* thisDCC;
	//std::stringstream CrateTestsOutput;
	//std::stringstream OutputStringDDUStatus[9];
	//std::stringstream OutputStringDCCStatus[9];
	//std::vector<DDU*> dduVector;
	//std::vector<DCC*> dccVector;
	//std::vector<VMEModule *> moduleVector;
	std::vector<FEDCrate*> crateVector;
	//Crate *thisCrate;
	//std::string Operator_;
	std::string DDUBoardID_[9];
	std::string DCCBoardID_[9];
	int DCC_ratemon[50][12];
	int DCC_ratemon_cnt;
	int DCC_ratemon_ch;
	//int DDU_, DCC_;

	std::string fcState_;
	//
public:

	XDAQ_INSTANTIATOR();

	/** Default Constructor **/
	EmuFCrateHyperDAQ(xdaq::ApplicationStub * s);


	/** Default page when XDAQ loads.  Sets configuration if required and
	*	bounces the user to the main page.
	*
	*	@param *in is a pointer to a standard xgi input object (for passing
	*	things like POST and GET variables to the function.)
	*	@param *out is the xgi output (basically, a stream that outputs to the
	*	browser window.
	*
	*	@throws xgi::exception::Exception for great justice!
	*
	*	@note The *in and *out parameters and the xgi::exception::Exception
	*	throw are common to all xgi-bound functions, and will herein not be
	*	included in the documentation.
	**/
	void Default(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);

	/** Page listing all the DDUs and the DCC along with their various options.
	*
	**/
	void mainPage(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);

	/** Page listing all the configuration options. **/
	void configurePage(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);

	/** Sets to which crate this EmuFCrateHyperDAQ application is talking.
	*	The original idea was to have each EmuFCrateHyperDAQ (and each FCrate)
	*	application talk to two different crates, selectable in some way.  This
	*	was because the original design called for one computer to talk to two
	*	FED crates, and it was seen as sort of rediculous to have two seperate
	*	processes to accomplish this.  However, we may eventually switch back to
	*	the one-crate-per-application model, which would instantly depricate
	*	this function.
	**/
	void setCrate(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);

	/** Loads the configuration file from raw textarea input. **/
	void setRawConfFile(xgi::Input * in, xgi::Output * out )
		throw (xgi::exception::Exception);

	/** Sets the configuration file from a text input std::string (should point to a
	*	file on the server-side disk that XDAQ can access.)
	**/
	void setConfFile(xgi::Input * in, xgi::Output * out )
		throw (xgi::exception::Exception);

	/** Uploads the configuration file from a file input (should point to a
	*	file on the client-side disk.)
	**/
	void UploadConfFile(xgi::Input * in, xgi::Output * out )
		throw (xgi::exception::Exception);

	/** Actually configures the application to talk to the apporpriate crate
	*	(crates?).
	*
	**/
	void Configuring();

	/** Load the firmware to the DDUs.  This is a universal loader, and reads
	*	from the xgi input which firmwares to load, etc.  Perhaps this can be
	*	broken into multiple methods?
	**/
	/*
	void DDUFirmware(xgi::Input * in, xgi::Output * out )
		throw (xgi::exception::Exception);
	*/

	/** Page listing the DDU firmware broadcast options. **/
	void DDUBroadcast(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);

	/** Uploads the broadcastable firmware from the client computer to the
	*	server computer.
	**/
	void DDULoadBroadcast(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);

	/** Broadcasts firmware to all DDUs simultaneously. **/
	void DDUSendBroadcast(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);

	/** Resets a particular DDU either cheating by using TTC, or the "real"
	*	way, by requesting a global reset.  The global reset method may or may
	*	not work ever.
	**/
	void DDUReset(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);

	/** Broadcasts 0xFED8 to the DDU FMM register (?) **/
	void DDUBrcstFED(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);

	/** @Deprecated Merged with the broadcast method **/
	/*
	void DDULoadFirmware(xgi::Input * in, xgi::Output * out )
		throw (xgi::exception::Exception);
	*/

	/** Shows General DDU debugging information. **/
	void DDUDebug(xgi::Input * in, xgi::Output * out )
		throw (xgi::exception::Exception);

	/** Shows Expert DDU debugging information and commands. **/
	void DDUExpert(xgi::Input * in, xgi::Output * out )
		throw (xgi::exception::Exception);

	/** Shows the DDU InFPGA 0 and 1 status page and various communication options. **/
	void InFpga(xgi::Input * in, xgi::Output * out )
		throw (xgi::exception::Exception);

	/** Traps and displays INFPGA debugging information (?)
	*
	*	@param lcode An array corresponding to the status of the INFPGAs (?)
	**/
	//void DDUinTrapDecode(xgi::Input * in, xgi::Output * out,  unsigned long int lcode[10])
		//throw (xgi::exception::Exception);

	/** Page for VME parallel register reading/writing. **/
	void VMEPARA(xgi::Input * in, xgi::Output * out )
    	throw (xgi::exception::Exception);

	/** Page for VEM Serial register reading/writing. **/
	void VMESERI(xgi::Input * in, xgi::Output * out )
		throw (xgi::exception::Exception);

	/** Load data into the DDU from text (?) **/
	void DDUTextLoad(xgi::Input * in, xgi::Output * out )
    	throw (xgi::exception::Exception);

	/** Starts/stops/monitors the IRQ interrupt handler. **/
	void VMEIntIRQ(xgi::Input * in, xgi::Output * out )
		throw (xgi::exception::Exception);

	/** Page for DCC firmware loading/checking. **/
	void DCCFirmware(xgi::Input * in, xgi::Output * out )
		throw (xgi::exception::Exception);

	/** Routine for actually loading the DCC firmware. **/
	void DCCLoadFirmware(xgi::Input * in, xgi::Output * out )
		throw (xgi::exception::Exception);

	/** Debugging routine for XML configuration file loading (?) **/
	void LoadXMLconf(xgi::Input * in, xgi::Output * out )
		throw (xgi::exception::Exception);

	/** Page for reading general debug information from the DCC. **/
	void DCCDebug(xgi::Input * in, xgi::Output * out )
		throw (xgi::exception::Exception);

	/** Page for reading and setting expert registers on the DCC. **/
	void DCCExpert(xgi::Input * in, xgi::Output * out )
		throw (xgi::exception::Exception);

	/** Load data into the DCC from text (?) **/
	void DCCTextLoad(xgi::Input * in, xgi::Output * out )
		throw (xgi::exception::Exception);

	/** Hard-reset the DCC to reset and load firmware. **/
	void DCCFirmwareReset(xgi::Input * in, xgi::Output * out )
		throw (xgi::exception::Exception);

	/** Live DDU voltage monitoring page. **/
	void DDUVoltMon(xgi::Input * in, xgi::Output * out )
		throw (xgi::exception::Exception);

	/** HTTP redirection page using status code 303. **/
	void webRedirect(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);

	/** Live DCC data rate monitoring. **/
	void DCCRateMon(xgi::Input * in, xgi::Output * out )
		throw (xgi::exception::Exception);

	/** Display a static page with a graph for the DCC data rate off the first
	*	S-Link channel.
	**/
	void getDataDCCRate0(xgi::Input * in, xgi::Output * out )
		throw (xgi::exception::Exception);

	/** Display a static page with a graph for the DCC data rate off the second
	*	S-Link channel.
	**/
	void getDataDCCRate1(xgi::Input * in, xgi::Output * out )
		throw (xgi::exception::Exception);

	/** Redirect the user so that the form information is cleared from the URL.
	*
	*	@author Phillip Killewald
	**/
	void webRedirect(xgi::Output *out, std::string location = "");

	/** My patented Select-a-crate/board
	*
	*	@author Phillip Killewald
	**/
	std::string selectACrate(std::string location, std::string what, unsigned int index, unsigned int crateIndex = 0);

};

#endif
