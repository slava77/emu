#ifndef __emu_daq_rui_version_h__
#define __emu_daq_rui_version_h__

#include "config/PackageInfo.h"

namespace emudaqrui {
	const std::string package     = "emudaqrui";
	const std::string versions    = "07.07.00";
	const std::string description = "Emu (CSC) Readout Unit Input";

	const std::string summary     = "emu/daq/rui";
	const std::string authors     = "Steve Murray, Karoly Banicz";
	const std::string link        = "";

	config::PackageInfo getPackageInfo();
	void checkPackageDependencies()
			throw (config::PackageInfo::VersionException);
	std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif

