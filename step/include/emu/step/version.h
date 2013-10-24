#ifndef __emu_step_version_h__
#define __emu_step_version_h__

#include "config/PackageInfo.h"

namespace emustep {
	const std::string package     = "emustep";
	const std::string versions    = "04.00.01";
	const std::string description = "Emu (CSC) System Test of Endcap Peripheral crate and chamber electronics (STEP)";

	const std::string summary     = "Emu (CSC) System Test of Endcap Peripheral crate and chamber electronics (STEP)";
	const std::string authors     = "K. Banicz";
	const std::string link        = "";

	config::PackageInfo getPackageInfo();
	void checkPackageDependencies()
			throw (config::PackageInfo::VersionException);
	std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif

