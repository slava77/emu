#ifndef __EMU_DAQ_FU_VERSION_H__
#define __EMU_DAQ_FU_VERSION_H__

#include "config/PackageInfo.h"

namespace emudaqfu {
	const std::string package     = "emudaqfu";
	const std::string versions    = "07.07.00";
	const std::string description = "Emu (CSC) Filter Unit";

	const std::string summary     = "emu/daq/fu";
	const std::string authors     = "Steve Murray, Karoly Banicz";
	const std::string link        = "";

	config::PackageInfo getPackageInfo();
	void checkPackageDependencies()
			throw (config::PackageInfo::VersionException);
	std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif

