#ifndef EmuMonitorV_h
#define EmuMonitorV_h

#include "PackageInfo.h"

namespace EmuMonitor {
	const std::string package     = "emuDQM/EmuMonitor";
	const std::string versions    = "2.0";
	const std::string description = "XDAQ application for EMU data monitoring";
    const std::string link = "http://www.phys.ufl.edu/cms/emu/dqm";
	
	toolbox::PackageInfo getPackageInfo();
	void checkPackageDependencies() throw (toolbox::PackageInfo::VersionException);
	set<string, less<string> > getPackageDependencies();
};

#endif  // ifndef EmuMonitorV_h
// vim: set ai sw=4 ts=4:
