#ifndef __EmuFUV_h__
#define __EmuFUV_h__

#include "config/PackageInfo.h"

namespace EmuFU
{
    const std::string package     = "emuFU";
    const std::string versions    = "08.10.17";
    const std::string description = "Filter unit for the endcap muon local DAQ";
    const std::string summary     = "emuDAQ/emuFU";
    const std::string link        = "NONE";
    const std::string authors     = "original by S.Murray adapted to Emu by K.Banicz";
    config::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (config::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
