#ifndef EmuMonitorV_h
#define EmuMonitorV_h

#include "config/PackageInfo.h"

namespace EmuMonitor
{
const std::string package     = "emuDQM/EmuMonitor";
const std::string versions    = "2.0";
const std::string description = "XDAQ application for EMU data monitoring";
const std::string link = "http://www.phys.ufl.edu/cms/emu/dqm";
const std::string authors = "Victor Barashko <barvic@ufl.edu>";
const std::string summary = "Emu DQM Monitoring Modules";

config::PackageInfo getPackageInfo();
void checkPackageDependencies() throw (config::PackageInfo::VersionException);
std::set<std::string, std::less<std::string> > getPackageDependencies();
};

#endif  // ifndef EmuMonitorV_h
// vim: set ai sw=4 ts=4:
