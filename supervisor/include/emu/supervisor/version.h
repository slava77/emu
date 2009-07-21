#ifndef __emu_supervisor_version_h__
#define __emu_supervisor_version_h__

#include "config/PackageInfo.h"

namespace emusupervisor {
	const std::string package     = "emusupervisor";
	const std::string versions    = "07.07.00";
	const std::string description = "Emu (CSC) Supervisor";

	const std::string summary     = "emu/supervisor";
	const std::string authors     = "Ichiro Suzuki";
	const std::string link        = "http://localhost:20000/";

	config::PackageInfo getPackageInfo();
	void checkPackageDependencies()
			throw (config::PackageInfo::VersionException);
	std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif

