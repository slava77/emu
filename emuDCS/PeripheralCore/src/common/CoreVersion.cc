#include "config/PackageInfo.h"

namespace EmuPeripheralCore {
	const std::string package  =  "EmuPeripheralCore";
	const std::string versions =  "4.7";
	const std::string summary = "emu/emuDCS/PeripheralCore";
	const std::string description = "Emu Peripheral Crate Core Library";
	const std::string authors = "Jinghua Liu for cms-emu-onlinesoftware";
	const std::string link = "http://cms.cern.ch";
	config::PackageInfo getPackageInfo();
        void checkPackageDependencies();
}

GETPACKAGEINFO(EmuPeripheralCore);

void EmuPeripheralCore::checkPackageDependencies() {}
