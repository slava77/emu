#ifndef __EmuRUIV_h__
#define __EmuRUIV_h__

#include "config/PackageInfo.h"

namespace EmuRUI
{
    const std::string package     = "emuRUI";
    const std::string versions    = "R_20080917";
    const std::string description = "Readout Unit Interface (RUI) for the endcap muon local DAQ";
    const std::string summary     = "emuDAQ/emuRUI";
    const std::string link        = "NONE";
    const std::string authors     = "original by S.Murray adapted to Emu by K.Banicz";
    config::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (config::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
