#ifndef __EmuFarmerV_h__
#define __EmuFarmerV_h__

#include "config/PackageInfo.h"

namespace EmuFarmer
{
    const std::string package     = "emuFarmer";
    const std::string versions    = "1.0";
    const std::string description = "Dispatches XDAQ executives on the Emu farm via JobControl.";
    const std::string summary     = "emuFarmer";
    const std::string link        = "NONE";
    const std::string authors     = "K.Banicz";
    config::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (config::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
