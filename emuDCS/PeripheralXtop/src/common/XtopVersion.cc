#include "config/PackageInfo.h"

namespace EmuPeripheralXtop {
	const std::string package  =  "EmuPeripheralXtop";
	const std::string versions =  "7.7";
	const std::string summary = "emu/emuDCS/PeripheralXtop";
	const std::string description = "Emu Peripheral Crate Xmas to PVSS Library";
	const std::string authors = "Jinghua Liu for cms-emu-onlinesoftware";
	const std::string link = "http://cms.cern.ch";
	config::PackageInfo getPackageInfo();
        void checkPackageDependencies();
}

GETPACKAGEINFO(EmuPeripheralXtop);

void EmuPeripheralXtop::checkPackageDependencies() {}
