#ifndef EmuMonitorV_h
#define EmuMonitorV_h

#include "PackageInfo.h"

namespace EmuMonitor {
	const string package     = "EmuMonitor";
	const string versions    = "1.2";
	const string description = "XDAQ application for EMU monitoring";

	toolbox::PackageInfo getPackageInfo();
	void checkPackageDependencies() throw (toolbox::PackageInfo::VersionException);
	set<string, less<string> > getPackageDependencies();
};

#endif  // ifndef EmuMonitorV_h
// vim: set ai sw=4 ts=4:
