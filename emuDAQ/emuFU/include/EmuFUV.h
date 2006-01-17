#ifndef __EmuFUV_h__
#define __EmuFUV_h__

#include "toolbox/include/PackageInfo.h"

namespace EmuFU
{
    const string package     = "emuDAQ/emuFU";
    const string versions    = "3.5";
    const string description = "Example Filter Unit (EmuFU) to be copied and modified by end-users.";
    const string link        = "NONE";
    toolbox::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (toolbox::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
