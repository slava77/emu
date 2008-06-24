#include "config/PackageInfo.h"

namespace EmuPeripheralApps {
	const std::string package  =  "EmuPeripheralApps";
	const std::string versions =  "6.1";
	const std::string summary = "emu/emuDCS/PeripheralApps";
	const std::string description = "Emu Peripheral Crate Application Library";
	const std::string authors = "Jinghua Liu for cms-emu-onlinesoftware";
	const std::string link = "http://cms.cern.ch";
	config::PackageInfo getPackageInfo();
        void checkPackageDependencies();
}

GETPACKAGEINFO(EmuPeripheralApps);

void EmuPeripheralApps::checkPackageDependencies() {}
