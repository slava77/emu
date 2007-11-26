#ifndef __EmuTAV_h__
#define __EmuTAV_h__

#include "config/PackageInfo.h"

namespace EmuTA
{
    const std::string package     = "emuTA";
    const std::string versions    = "4.0";
    const std::string description = "A dummy trigger adapter (or rather generator) for the endcap muon local DAQ";
    const std::string summary     = "emuDAQ/emuTA";
    const std::string link        = "NONE";
    const std::string authors     = "original by S.Murray adapted to Emu by K.Banicz";
    config::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (config::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
