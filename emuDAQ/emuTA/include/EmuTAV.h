#ifndef __EmuTAV_h__
#define __EmuTAV_h__

#include "toolbox/include/PackageInfo.h"

namespace EmuTA
{
    const string package     = "emuDAQ/emuTA";
    const string versions    = "3.5";
    const string description = "Example trigger adapter (TA) to be copied and modified by end-users.";
    const string link        = "NONE";
    toolbox::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (toolbox::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
