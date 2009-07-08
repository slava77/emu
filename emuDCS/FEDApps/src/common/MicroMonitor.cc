/*****************************************************************************\
* $Id: MicroMonitor.cc,v 1.2 2009/07/08 12:03:09 paste Exp $
\*****************************************************************************/
#include "emu/fed/MicroMonitor.h"

#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <iomanip>
#include <time.h>

#include "xgi/Method.h"
#include "emu/base/Alarm.h"
#include "emu/fed/Crate.h"
#include "emu/fed/AutoConfigurator.h"
#include "emu/fed/DDU.h"
#include "emu/fed/DCC.h"

XDAQ_INSTANTIATOR_IMPL(emu::fed::MicroMonitor)



emu::fed::MicroMonitor::MicroMonitor(xdaq::ApplicationStub *stub):
xdaq::WebApplication(stub),
emu::fed::Application(stub),
emu::fed::Configurable(stub),
emu::base::Supervised(stub),
emu::fed::Supervised(stub)
{
	
	// HyperDAQ pages
	xgi::bind(this, &emu::fed::MicroMonitor::DCSOutput, "DCSOutput");
	
	// SOAP call-back functions which fire the transitions to the FSM
	BIND_DEFAULT_SOAP2FSM_ACTION(MicroMonitor, Configure);
	BIND_DEFAULT_SOAP2FSM_ACTION(MicroMonitor, Enable);
	BIND_DEFAULT_SOAP2FSM_ACTION(MicroMonitor, Disable);
	BIND_DEFAULT_SOAP2FSM_ACTION(MicroMonitor, Halt);
	
	// FSM state definitions and state-change call-back functions
	fsm_.addState('H', "Halted", this, &emu::fed::MicroMonitor::stateChanged);
	fsm_.addState('C', "Configured", this, &emu::fed::MicroMonitor::stateChanged);
	fsm_.addState('E', "Enabled", this, &emu::fed::MicroMonitor::stateChanged);
	
	// FSM transition definitions
	fsm_.addStateTransition('H', 'C', "Configure", this, &emu::fed::MicroMonitor::configureAction);
	fsm_.addStateTransition('C', 'C', "Configure", this, &emu::fed::MicroMonitor::configureAction);
	fsm_.addStateTransition('E', 'C', "Configure", this, &emu::fed::MicroMonitor::configureAction);
	fsm_.addStateTransition('F', 'C', "Configure", this, &emu::fed::MicroMonitor::configureAction);
	
	fsm_.addStateTransition('H', 'C', "Disable", this, &emu::fed::MicroMonitor::disableAction);
	fsm_.addStateTransition('C', 'C', "Disable", this, &emu::fed::MicroMonitor::disableAction);
	fsm_.addStateTransition('E', 'C', "Disable", this, &emu::fed::MicroMonitor::disableAction);
	fsm_.addStateTransition('F', 'C', "Disable", this, &emu::fed::MicroMonitor::disableAction);
	
	fsm_.addStateTransition('H', 'E', "Enable", this, &emu::fed::MicroMonitor::enableAction);
	fsm_.addStateTransition('C', 'E', "Enable", this, &emu::fed::MicroMonitor::enableAction);
	fsm_.addStateTransition('E', 'E', "Enable", this, &emu::fed::MicroMonitor::enableAction);
	fsm_.addStateTransition('F', 'E', "Enable", this, &emu::fed::MicroMonitor::enableAction);
	
	fsm_.addStateTransition('H', 'H', "Halt", this, &emu::fed::MicroMonitor::haltAction);
	fsm_.addStateTransition('C', 'H', "Halt", this, &emu::fed::MicroMonitor::haltAction);
	fsm_.addStateTransition('E', 'H', "Halt", this, &emu::fed::MicroMonitor::haltAction);
	fsm_.addStateTransition('F', 'H', "Halt", this, &emu::fed::MicroMonitor::haltAction);
	
	fsm_.setInitialState('H');
	fsm_.reset();
	
	state_ = fsm_.getStateName(fsm_.getCurrentState());
	
	// Configure the software with the Autoconfigurator
	configMode_ = "Autodetect";
	try {
		softwareConfigure();
		REVOKE_ALARM("MicroMonitorConfigure", NULL);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Unable to properly configure the MicroMonitor.  Please destroy and re-initialize the application.";
		LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
		RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "MicroMonitorConfigure", "ERROR", error.str(), e.getProperty("tag"), NULL, e);
	}
}



emu::fed::MicroMonitor::~MicroMonitor()
{
}



// HyperDAQ pages
void emu::fed::MicroMonitor::DCSOutput(xgi::Input *in, xgi::Output *out)
{
	// Get the data from each DDU and print it out.  Simple as that.
	for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
		
		std::vector<DDU *> myDDUs = (*iCrate)->getDDUs();
		for (std::vector<DDU *>::iterator iDDU = myDDUs.begin(); iDDU != myDDUs.end(); iDDU++) {

			bool goodRead = true;
			
			// I don't want to have to read the flash every time I do this, so I rely on the AutoConfigurator to do its job.
			uint16_t rui = (*iDDU)->getRUI();
			
			// Timestamp in UTC (I hope)
			time_t timestamp = time(NULL);
			
			// I can put anything I want here, so I am going to use the fiber status
			uint16_t reserved = 0;
			try {
				reserved = (*iDDU)->readFiberErrors();
			} catch (emu::fed::exception::Exception &e) {
				goodRead = false;
			}
			
			// Voltages
			std::vector<float> voltages(4, 9999);
			for (size_t iVolt = 0; iVolt < 4; iVolt++) {
				try {
					voltages[iVolt] = (*iDDU)->readVoltage(iVolt);
				} catch (emu::fed::exception::Exception &e) {
					goodRead = false;
				}
			}
			
			// Temperatures
			std::vector<float> temperatures(4, 999);
			for (size_t iTemp = 0; iTemp < 4; iTemp++) {
				try {
					temperatures[iTemp] = (*iDDU)->readTemperature(iTemp);
				} catch (emu::fed::exception::Exception &e) {
					goodRead = false;
				}
			}
			
			// Trailer is fixed
			uint16_t trailer = 0xBEEF;
			
			// Compile the string and output
			*out << "DDU" << rui << " " << timestamp << " " << (goodRead ? 1 : 0) << " " << reserved << " ";
			for (std::vector<float>::const_iterator iVolt = voltages.begin(); iVolt != voltages.end(); iVolt++) {
				*out << (*iVolt) << " ";
			}
			for (std::vector<float>::const_iterator iTemp = temperatures.begin(); iTemp != temperatures.end(); iTemp++) {
				*out << (*iTemp) << " ";
			}
			*out << trailer << std::endl;
		}
	}
}



void emu::fed::MicroMonitor::configureAction(toolbox::Event::Reference event)
throw (toolbox::fsm::exception::Exception)
{
	
	LOG4CPLUS_DEBUG(getApplicationLogger(), "FSM transition received:  Configure");
	
	// Configure the software with the Autoconfigurator
	configMode_ = "Autodetect";
	try {
		softwareConfigure();
		REVOKE_ALARM("MicroMonitorConfigure", NULL);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Unable to properly configure the MicroMonitor.  Please destroy and re-initialize the application.";
		LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
		RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "MicroMonitorConfigure", "ERROR", error.str(), e.getProperty("tag"), NULL, e);
		XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e);
	}
	
}



void emu::fed::MicroMonitor::enableAction(toolbox::Event::Reference event)
throw (toolbox::fsm::exception::Exception)
{
	
	LOG4CPLUS_DEBUG(getApplicationLogger(), "FSM transition received:  Enable");
	// Dummy
}



void emu::fed::MicroMonitor::disableAction(toolbox::Event::Reference event)
throw (toolbox::fsm::exception::Exception)
{
	
	LOG4CPLUS_DEBUG(getApplicationLogger(), "FSM transition received:  Disable");
	// Dummy
}



void emu::fed::MicroMonitor::haltAction(toolbox::Event::Reference event)
throw (toolbox::fsm::exception::Exception)
{
	
	LOG4CPLUS_DEBUG(getApplicationLogger(), "FSM transition received:  Halt");
	// Dummy
}

