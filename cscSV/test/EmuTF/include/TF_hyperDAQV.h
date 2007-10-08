#ifndef __TF_HYPERDAQ_V_H__
#define __TF_HYPERDAQ_V_H__

#include "config/PackageInfo.h"

namespace TF_hyperDAQ {
	const std::string package     = "TF_hyperDAQ";
	const std::string versions    = "1.0";
	const std::string description = "dummy TF_hyperDAQ";

	const std::string summary     = "emu/cscSV";
	const std::string authors     = "Ichiro Suzuki";
	const std::string link        = "http://localhost:40000/";

	config::PackageInfo getPackageInfo();
	void checkPackageDependencies()
			throw (config::PackageInfo::VersionException);
	std::set<std::string, std::less<std::string> > getPackageDependencies();
};

#endif  // ifndef __TF_HYPERDAQ_V_H__
// vim: set sw=4 ts=4:
