#ifndef __emu_dqm_cscmonitor_version_h
#define __emu_dqm_cscmonitor_version_h

#include "config/PackageInfo.h"

namespace emudqmcscmonitor 
{
const std::string package     = "emudqmcscmonitor";
const std::string versions    = "10.04.02";
const std::string description = "EMU CSC Data Monitoring Module";
const std::string link = "http://www.phys.ufl.edu/cms/emu/dqm";
const std::string authors = "Victor Barashko <barvic@ufl.edu>";
const std::string summary = "Emu CSC DQM Monitoring Modules";

config::PackageInfo getPackageInfo();
void checkPackageDependencies() throw (config::PackageInfo::VersionException);
std::set<std::string, std::less<std::string> > getPackageDependencies();
};

#endif 
