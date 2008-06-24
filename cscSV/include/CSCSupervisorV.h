#ifndef __CSC_SUPERVISOR_V_H__
#define __CSC_SUPERVISOR_V_H__

#include "config/PackageInfo.h"

namespace CSCSupervisor {
	const std::string package     = "CSCSupervisor";
	const std::string versions    = "3.2";
	const std::string description = "CSC supervisor";

	const std::string summary     = "emu/cscSV";
	const std::string authors     = "Ichiro Suzuki";
	const std::string link        = "http://localhost:40000/";

	config::PackageInfo getPackageInfo();
	void checkPackageDependencies()
			throw (config::PackageInfo::VersionException);
	std::set<std::string, std::less<std::string> > getPackageDependencies();
};

#endif  // ifndef __CSC_SUPERVISOR_V_H__
// vim: set ai sw=4 ts=4:
