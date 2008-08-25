/*****************************************************************************\
* $Id: EmuFCrateManager.h,v 1.16 2008/08/25 12:25:49 paste Exp $
*
* $Log: EmuFCrateManager.h,v $
* Revision 1.16  2008/08/25 12:25:49  paste
* Major updates to VMEController/VMEModule handling of CAEN instructions.  Also, added version file for future RPMs.
*
* Revision 1.15  2008/08/15 10:40:20  paste
* Working on fixing CAEN controller opening problems
*
* Revision 1.13  2008/08/15 08:35:50  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __EMUFCRATEMANAGER_H__
#define __EMUFCRATEMANAGER_H__

#include <string>

#include "xoap/MessageReference.h"
#include "xdata/Integer.h"
#include "xdata/Table.h"

#include "EmuFEDApplication.h"

class EmuFCrateManager: public EmuFEDApplication
{

public:

	XDAQ_INSTANTIATOR();

	xdata::String ConfigureState_;
	xdata::Table table_;

	EmuFCrateManager(xdaq::ApplicationStub * s);

	void webDefault(xgi::Input * in, xgi::Output * out )
		throw (xgi::exception::Exception);
	void webFire(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);
	
	void configureAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception);
	void enableAction(toolbox::Event::Reference e) 
		throw (toolbox::fsm::exception::Exception);
	void disableAction(toolbox::Event::Reference e) 
		throw (toolbox::fsm::exception::Exception);
	void haltAction(toolbox::Event::Reference e) 
		throw (toolbox::fsm::exception::Exception);

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

	xoap::MessageReference killAllMessage();
	xoap::MessageReference QueryFCrateInfoSpace();
	xoap::MessageReference QueryLTCInfoSpace();
	xoap::MessageReference QueryJobControlInfoSpace();
	xoap::MessageReference ExecuteCommandMessage(std::string port);
	xoap::MessageReference createXRelayMessage(const std::string & command, const std::string & setting, std::set<xdaq::ApplicationDescriptor * > descriptor );

	void CheckEmuFCrateState();

private:

	xdata::UnsignedInteger tts_id_;
	xdata::UnsignedInteger tts_crate_;
	xdata::UnsignedInteger tts_slot_;
	xdata::UnsignedInteger tts_bits_;

	void sendCommand(std::string command, std::string klass, int instance = -1)
		throw (xoap::exception::Exception, xdaq::exception::Exception);
	xoap::MessageReference createCommandSOAP(std::string command);

	void webRedirect(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);

	bool soapConfigured_;
	bool soapLocal_;

};

#endif
