#ifndef __emu_me11dev_version_h__
#define __emu_me11dev_version_h__

#include "config/PackageInfo.h"

namespace emume11dev {
	const std::string package     = "emume11dev";
	const std::string versions    = "02.00.00";
	const std::string description = "Emu ME1/1 development tools";

	const std::string summary     = "Emu ME1/1 development tools";
	const std::string authors     = "Brant Rumberger, Dan King, Joe Haley";
	const std::string link        = "";

	config::PackageInfo getPackageInfo();
	void checkPackageDependencies()
			throw (config::PackageInfo::VersionException);
	std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif

