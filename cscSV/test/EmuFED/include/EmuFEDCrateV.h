#ifndef __EMU_FED_CRATE_V_H__
#define __EMU_FED_CRATE_V_H__

#include "PackageInfo.h"

namespace EmuFEDCrate {
	const string package     = "EmuFEDCrate";
	const string versions    = "1.0";
	const string description = "dummy XDAQ application to test CSC Supervisor";
	const string link        = "http://localhost:40000/";

	toolbox::PackageInfo getPackageInfo();
	void checkPackageDependencies() throw (toolbox::PackageInfo::VersionException);
	set<string, less<string> > getPackageDependencies();
};

#endif  // ifndef __EMU_FED_CRATE_V_H__
// vim: set ai sw=4 ts=4:
