#ifndef __emu_database_version_h__
#define __emu_database_version_h__

#include "config/PackageInfo.h"

namespace emudatabase {
	const std::string package     = "emudatabase";
	const std::string versions    = "09.09.00";
	const std::string description = "Emu (CSC) database access tools";

	const std::string summary     = "emu/database";
	const std::string authors     = "Angela Brett";
	const std::string link        = "http://localhost:20000/";

	config::PackageInfo getPackageInfo();
	void checkPackageDependencies()
			throw (config::PackageInfo::VersionException);
	std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif  // ifndef __emu_database_version_h__

