// $Id: EmuFCrateManager.h,v 1.11 2008/08/13 14:20:41 paste Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef __EMUFCRATEMANAGER_H__
#define __EMUFCRATEMANAGER_H__

#include <ostream>

#include "xgi/Utils.h"
#include "xgi/Method.h"

#include "xdaq/Application.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"

#include "xdaq/NamespaceURI.h"

#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/Method.h"

#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

#include "xdata/exdr/FixedSizeInputStreamBuffer.h"
#include "xdata/exdr/AutoSizeOutputStreamBuffer.h"
#include "xdata/exdr/Serializer.h"
#include <xdata/String.h>
#include <xdata/Float.h>
#include <xdata/Double.h>
#include <xdata/Integer.h>
#include <xdata/Boolean.h>
#include <xdata/UnsignedLong.h>
#include <xdata/Table.h>

#include "EmuFEDApplication.h"
// #include "EmuController.h"
// #include "Crate.h"
// #include "CrateUtilities.h"

// EmuController * MyController;
// Crate *broadcastCrate;

//using namespace cgicc;
//using namespace std;


#include "xdaq2rc/RcmsStateNotifier.h"

class EmuFCrateManager: public EmuFEDApplication
{
	class StateTable;
	friend class StateTable;

public:

	XDAQ_INSTANTIATOR();

	xdata::String ConfigureState_;
	xdata::Table table_;

	EmuFCrateManager(xdaq::ApplicationStub * s);

	void webDefault(xgi::Input * in, xgi::Output * out )
		throw (xgi::exception::Exception);
	void webFire(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);
	
	//void MainPage(xgi::Input * in, xgi::Output * out )
	//	throw (xgi::exception::Exception);
	//void CheckEmuFCrateState(xgi::Input * in, xgi::Output * out );
	
	void configureAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception);
	void enableAction(toolbox::Event::Reference e) 
		throw (toolbox::fsm::exception::Exception);
	void disableAction(toolbox::Event::Reference e) 
		throw (toolbox::fsm::exception::Exception);
	void haltAction(toolbox::Event::Reference e) 
		throw (toolbox::fsm::exception::Exception);
	// PGK These don't need to be state transitions.
	//void setTTSBitsAction(toolbox::Event::Reference e) 
	//	throw (toolbox::fsm::exception::Exception);
	//void setTTSBitsResponseAction(toolbox::Event::Reference e)
	//	throw (toolbox::fsm::exception::Exception);

	void stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
		throw (toolbox::fsm::exception::Exception);
		
	std::string extractState(xoap::MessageReference message);
	std::string extractRunNumber(xoap::MessageReference message);

	xoap::MessageReference onConfigure (xoap::MessageReference message) 
		throw (xoap::exception::Exception);
	xoap::MessageReference onConfigCalCFEB (xoap::MessageReference message) 
		throw (xoap::exception::Exception);
	xoap::MessageReference onEnable (xoap::MessageReference message) 
		throw (xoap::exception::Exception);
	xoap::MessageReference onDisable (xoap::MessageReference message) 
		throw (xoap::exception::Exception);
	xoap::MessageReference onHalt (xoap::MessageReference message) 
		throw (xoap::exception::Exception);
	
	xoap::MessageReference onSetTTSBits(xoap::MessageReference message) 
		throw (xoap::exception::Exception);
	// PGK We don't acutally need to handle a response, either.
	//xoap::MessageReference onSetTTSBitsResponse(xoap::MessageReference message) 
	//	throw (xoap::exception::Exception);

	void relayMessage (xoap::MessageReference msg) 
		throw (xgi::exception::Exception);
	void SendSOAPMessageXRelaySimple(std::string command,std::string setting);
	void SendSOAPMessageXRelayReturn(std::string command,std::string setting);
	void SendSOAPMessageConfigureXRelay(xgi::Input * in, xgi::Output * out ) 
		throw (xgi::exception::Exception);
	void SendSOAPMessageCalibrationXRelay(xgi::Input * in, xgi::Output * out ) 
		throw (xgi::exception::Exception);
	void SendSOAPMessageConfigure(xgi::Input * in, xgi::Output * out ) 
		throw (xgi::exception::Exception);
	
	void PCsendCommand(std::string command, std::string klass) 
		throw (xoap::exception::Exception, xdaq::exception::Exception);
	xoap::MessageReference PCcreateCommandSOAP(std::string command);
	//  void PCanalyzeReply(xoap::MessageReference message, xoap::MessageReference reply,xdaq::ApplicationDescriptor *app);

	xoap::MessageReference killAllMessage();
	xoap::MessageReference QueryFCrateInfoSpace();
	xoap::MessageReference QueryLTCInfoSpace();
	xoap::MessageReference QueryJobControlInfoSpace();
	xoap::MessageReference ExecuteCommandMessage(std::string port);
	xoap::MessageReference createXRelayMessage(const std::string & command, const std::string & setting, std::set<xdaq::ApplicationDescriptor * > descriptor );

	//xdata::Table getDCCTable(xdaq::ApplicationDescriptor *descriptor);
	

	void CheckEmuFCrateState();

private:

	xdata::UnsignedInteger tts_id_;
	xdata::UnsignedInteger tts_crate_;
	xdata::UnsignedInteger tts_slot_;
	xdata::UnsignedInteger tts_bits_;


	void sendCommand(std::string command, std::string klass, int instance)
		throw (xoap::exception::Exception, xdaq::exception::Exception);
	xoap::MessageReference createCommandSOAP(std::string command);


	void webRedirect(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);

	bool soapConfigured_;
	bool soapLocal_;

};

#endif
