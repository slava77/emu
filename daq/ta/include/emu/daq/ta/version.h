#ifndef __emu_daq_ta_version_h__
#define __emu_daq_ta_version_h__

#include "config/PackageInfo.h"

namespace emudaqta {
	const std::string package     = "emudaqta";
	const std::string versions    = "07.07.00";
	const std::string description = "Emu (CSC) Trigger Adapter";

	const std::string summary     = "emu/daq/ta";
	const std::string authors     = "Steve Murray, Karoly Banicz";
	const std::string link        = "";

	config::PackageInfo getPackageInfo();
	void checkPackageDependencies()
			throw (config::PackageInfo::VersionException);
	std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
