#ifndef __emu_soap_version_h__
#define __emu_soap_version_h__

#include "config/PackageInfo.h"

namespace emusoap {
	const std::string package     = "emusoap";
	const std::string versions    = "11.04.00";
	const std::string description = "SOAP messenger class for Emu (CSC) XDAQ applications";

	const std::string summary     = "emu/soap";
	const std::string authors     = "Karoly Banicz";
	const std::string link        = "http://localhost:20000/";

	config::PackageInfo getPackageInfo();
	void checkPackageDependencies()
			throw (config::PackageInfo::VersionException);
	std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif  // ifndef __emu_soap_version_h__

