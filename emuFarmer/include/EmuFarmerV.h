#ifndef __EmuFarmerV_h__
#define __EmuFarmerV_h__

#include "toolbox/include/PackageInfo.h"

namespace EmuFarmer
{
    const std::string package     = "emuFarmer";
    const std::string versions    = "1.0";
    const std::string description = "Dispatches XDAQ executives on the Emu farm via JobControl.";
    const std::string link        = "NONE";

    toolbox::PackageInfo getPackageInfo();

    void checkPackageDependencies()
    throw (toolbox::PackageInfo::VersionException);

    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
