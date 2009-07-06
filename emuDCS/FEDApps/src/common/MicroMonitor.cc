/*****************************************************************************\
* $Id: MicroMonitor.cc,v 1.1 2009/07/06 15:50:10 paste Exp $
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
emu::fed::Configurable(stub)
{
	
	// HyperDAQ pages
	xgi::bind(this, &emu::fed::MicroMonitor::DCSOutput, "DCSOutput");
	
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
