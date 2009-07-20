#ifndef EmuTFMonitorV_h
#define EmuTFMonitorV_h

#include "config/PackageInfo.h"

namespace EmuTFMonitor
{
const std::string package     = "emuDQM/EmuTFMonitor";
const std::string versions    = "2.0";
const std::string description = "XDAQ application for EMU data monitoring";
const std::string link = "http://www.phys.ufl.edu/cms/emu/dqm";
const std::string authors = "Victor Barashko <barvic@ufl.edu>";
const std::string summary = "Emu DQM Monitoring Modules";

config::PackageInfo getPackageInfo();
void checkPackageDependencies() throw (config::PackageInfo::VersionException);
std::set<std::string, std::less<std::string> > getPackageDependencies();
};

#endif  // ifndef EmuTFMonitorV_h
// vim: set ai sw=4 ts=4:
