#ifndef _EmuClientV_h_
#define _EmuClientV_h_

#include "config/PackageInfo.h"

namespace EmuClient 
{
    const std::string package     = "emuDAQ/emuClient";
    const std::string versions    = "08.10.17";
    const std::string description = "Example I2O client for the endcap muon local DAQ";
    const std::string summary     = "emuDAQ/emuClient";
    const std::string link        = "NONE";
    const std::string authors     = "K.Banicz";
    config::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (config::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
