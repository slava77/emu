#ifndef _EmuSOAPClientV_h_
#define _EmuSOAPClientV_h_

#include "config/PackageInfo.h"

namespace EmuSOAPClient 
{
    const std::string package     = "emuDAQ/emuSOAPClient";
    const std::string versions    = "R_20080917";
    const std::string description = "Example SOAP client for the endcap muon local DAQ";
    const std::string summary     = "emuDAQ/emuSOAPClient";
    const std::string link        = "NONE";
    const std::string authors     = "K.Banicz";
    config::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (config::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
