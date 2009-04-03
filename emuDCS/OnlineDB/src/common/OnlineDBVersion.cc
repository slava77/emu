#include "config/PackageInfo.h"

namespace EmuOnlineDB {
        const std::string package  =  "EmuOnlineDB";
        const std::string versions =  "7.2";
        const std::string summary = "emu/emuDCS/OnlineDB";
        const std::string description = "Emu Online Database Library";
        const std::string authors = "Stan Durkin";
        const std::string link = "http://cern.ch/cscdoc";
        config::PackageInfo getPackageInfo();
        void checkPackageDependencies();
}

GETPACKAGEINFO(EmuOnlineDB);

void EmuOnlineDB::checkPackageDependencies() {}
