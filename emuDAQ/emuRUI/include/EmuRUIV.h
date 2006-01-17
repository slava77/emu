#ifndef __EmuRUIV_h__
#define __EmuRUIV_h__

#include "toolbox/include/PackageInfo.h"

namespace EmuRUI
{
    const string package     = "emuDAQ/emuRUI";
    const string versions    = "3.5";
    const string description = "Example Readout Unit Interface (RUI) to be copied and modified by end-users.";
    const string link        = "NONE";
    toolbox::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (toolbox::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
