#ifndef __emu_daq_manager_Watchdog_h__
#define __emu_daq_manager_Watchdog_h__

#include "emu/daq/manager/Application.h"
#include "emu/daq/manager/AppStates.h"
#include <set>
#include <vector>
#include <string>


namespace emu { namespace daq { namespace manager {

      class Application;

      class Watchdog {
	friend ostream& operator<<( ostream& os,  emu::daq::manager::Watchdog& wd );
      public:
	Watchdog( emu::daq::manager::Application *owner );
	void patrol();
	std::set<xdaq::ApplicationDescriptor*> getAppsInStates( const std::vector<std::string>& states ) const;
      private:
	void insertAllApps( emu::daq::manager::AppStates& appStates );
	emu::daq::manager::Application *application_; ///< Pointer to the owner XDAQ application.
	emu::daq::manager::AppStates currentAppStates_;
	emu::daq::manager::AppStates previousAppStates_;
      };

      ostream& operator<<( ostream& os,  emu::daq::manager::Watchdog& wd );

}}} // namespace emu::daq::manager

#endif
