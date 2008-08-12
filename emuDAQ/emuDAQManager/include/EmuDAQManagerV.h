#ifndef __EmuDAQManagerV_h__
#define __EmuDAQManagerV_h__

#include "config/PackageInfo.h"

namespace EmuDAQManager
{
    const std::string package     = "emuDAQManager";
    const std::string versions    = "CRUZET4";
    const std::string description = "Manages the EmuDAQ applications";
    const std::string summary     = "emuDAQ/emuDAQManager";
    const std::string link        = "NONE";
    const std::string authors     = "original by S.Murray adapted to Emu by K.Banicz";
    config::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (config::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
