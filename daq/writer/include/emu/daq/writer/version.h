#ifndef __emu_writer_version_h__
#define __emu_writer_version_h__

#include "config/PackageInfo.h"

namespace emudaqwriter {
	const std::string package     = "emudaqwriter";
	const std::string versions    = "07.07.00";
	const std::string description = "Emu (CSC) DAQ file writer";

	const std::string summary     = "emu/daq/writer";
	const std::string authors     = "Karoly Banicz";
	const std::string link        = "";

	config::PackageInfo getPackageInfo();
	void checkPackageDependencies()
			throw (config::PackageInfo::VersionException);
	std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
