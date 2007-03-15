#ifndef EmuTFMonitorV_h
#define EmuTFMonitorV_h

#include "PackageInfo.h"

namespace EmuTFMonitor {
    const string package     = "emuDQM/EmuTFMonitor";
    const string versions    = "1.0";
    const string description = "XDAQ application for EMU TF data monitoring";
    const string link = "http://www.phys.ufl.edu/cms/emu/dqm";

    toolbox::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (toolbox::PackageInfo::VersionException);
    set<string, less<string> > getPackageDependencies();
};

#endif
