#ifndef __emu_base_version_h__
#define __emu_base_version_h__

#include "config/PackageInfo.h"

namespace emubase {
	const std::string package     = "emubase";
	const std::string versions    = "07.03.00";
	const std::string description = "Emu (CSC) applications' base classes";

	const std::string summary     = "emu/base";
	const std::string authors     = "Ichiro Suzuki";
	const std::string link        = "http://localhost:20000/";

	config::PackageInfo getPackageInfo();
	void checkPackageDependencies()
			throw (config::PackageInfo::VersionException);
	std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif  // ifndef __EMU_BASE_VERSION_H__

