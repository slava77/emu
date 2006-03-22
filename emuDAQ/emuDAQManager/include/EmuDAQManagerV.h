#ifndef __EmuDAQManagerV_h__
#define __EmuDAQManagerV_h__

#include "toolbox/include/PackageInfo.h"

namespace EmuDAQManager
{
    const string package     = "emuDAQ/emuDAQManager";
    const string versions    = "3.5";
    const string description = "Manages the EmuDAQ applications.";
    const string link        = "NONE";
    toolbox::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (toolbox::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
