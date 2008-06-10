// $Id: EmuFCrateManager.h,v 1.4 2008/06/10 13:40:24 gilmore Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _EmuFCrateManager_h_
#define _EmuFCrateManager_h_

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

#include "LocalEmuApplication.h"
// #include "EmuController.h"
// #include "Crate.h"
// #include "CrateUtilities.h"

// EmuController * MyController;
// Crate *broadcastCrate;

using namespace cgicc;
using namespace std;


#include "xdaq2rc/RcmsStateNotifier.h"

class EmuFCrateManager: public LocalEmuApplication
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
		
	string extractState(xoap::MessageReference message);
	string extractRunNumber(xoap::MessageReference message);

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
	
	void PCsendCommand(string command, string klass) 
		throw (xoap::exception::Exception, xdaq::exception::Exception);
	xoap::MessageReference PCcreateCommandSOAP(string command);
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
	//xdata::String tts_crate_;
	//xdata::String tts_slot_;
	//xdata::String tts_bits_;

	void sendCommand(string command, string klass, int instance)
		throw (xoap::exception::Exception, xdaq::exception::Exception);
	xoap::MessageReference createCommandSOAP(string command);
	//void setParameter(string klass, string name, string type,string value);
	void setParameter(string klass, string name, string type,unsigned int value);
	xoap::MessageReference createParameterSetSOAP(string klass, string name, string type, string value);
	void analyzeReply(xoap::MessageReference message, xoap::MessageReference reply, xdaq::ApplicationDescriptor *app);

	void webRedirect(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);

/*
	class StateTable
	{
	public:
		StateTable(EmuFCrateManager *fedmgr);
		void addApplication(string klass);
		void refresh();
		string getState(string klass, unsigned int instance);
		bool isValidState(string expected);
		void webOutput(xgi::Output *out, string sv_state)
			throw (xgi::exception::Exception);

	private:
		xoap::MessageReference createStateSOAP(string klass);
		string extractState(xoap::MessageReference message, string klass);

		EmuFCrateManager *fedmgr_;
		vector<pair<xdaq::ApplicationDescriptor *, string> > table_;
	} state_table_;
*/

	bool soapConfigured_;
	bool soapLocal_;

};

#endif
