#ifndef __emu_daq_manager_version_h__
#define __emu_daq_manager_version_h__

#include "config/PackageInfo.h"

namespace emudaqmanager {
	const std::string package     = "emudaqmanager";
	const std::string versions    = "07.05.00";
	const std::string description = "Emu (CSC) Local DAQ Manager";

	const std::string summary     = "emu/daq/manager";
	const std::string authors     = "Steve Murray, Karoly Banicz";
	const std::string link        = "";

	config::PackageInfo getPackageInfo();
	void checkPackageDependencies()
			throw (config::PackageInfo::VersionException);
	std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif

