#ifndef __emu_farmer_version_h__
#define __emu_farmer_version_h__

#include "config/PackageInfo.h"

namespace emufarmer {
	const std::string package     = "emufarmer";
	const std::string versions    = "07.05.00";
	const std::string description = "Emu (CSC) Farmer";

	const std::string summary     = "emu/farmer";
	const std::string authors     = "K. Banicz";
	const std::string link        = "http://host:20399/";

	config::PackageInfo getPackageInfo();
	void checkPackageDependencies()
			throw (config::PackageInfo::VersionException);
	std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif

