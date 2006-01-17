#ifndef __EmuDAQtesterV_h__
#define __EmuDAQtesterV_h__

#include "toolbox/include/PackageInfo.h"

namespace EmuDAQtester
{
    const string package     = "emuDAQ/emuDAQtester";
    const string versions    = "3.5";
    const string description = "Tests the EmuDAQ applications.";
    const string link        = "NONE";
    toolbox::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (toolbox::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
