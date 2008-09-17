#ifndef _SwitchGUI_h_
#define _SwitchGUI_h_

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <unistd.h> // for sleep()
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <time.h>


#include "xdaq/Application.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"
#include "xdaq/NamespaceURI.h"

#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/Method.h"

#include "xgi/Utils.h"
#include "xgi/Method.h"
#include "xdata/UnsignedLong.h"
#include "xdata/String.h"
#include "xdata/InfoSpaceFactory.h"
#include "xdata/InfoSpace.h"
#include "toolbox/net/Utils.h"

#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include "cgicc/FormFile.h"

#include "Switch.h"

class SwitchGUI: public xdaq::Application {
    public:
        XDAQ_INSTANTIATOR();

	SwitchGUI(xdaq::ApplicationStub * s)throw (xdaq::exception::Exception);

	void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);	
	void MainPage(xgi::Input * in, xgi::Output * out );	
	void GotoMain(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
	void BackupSwitch(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);	
	void ResetSwitch(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);	
	void ResetCounters(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
	void CLRcounters(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
	void Maintenance(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);	
	void MacGUI(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
	void GotoMacGUI(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
	void ProblemsGUI(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);	
	void html_port_status(xgi::Input * in, xgi::Output * out );

	/* modifier Phillip Killewald HTML Stuff */
	
	virtual std::string Header(std::string myTitle,bool reload);
	virtual std::string CSS();	
	
	/** Returns the standard Footer for the EmuFCrate applications.
	*
	*	@returns a huge string that is basically the footer code in HTML.  Good
	*	 for outputting straight to the xgi::Output.
	*
	*	@sa the CSS method.
	**/
	virtual std::string Footer();	

    protected:
	xdata::UnsignedLong switch_;
	xdata::UnsignedLong prt_;
	xdata::UnsignedLong slt_;
	xdata::String switchTelnet_;
	xdata::String shutdownPort_;
	xdata::String backupDir_;
};
#endif
