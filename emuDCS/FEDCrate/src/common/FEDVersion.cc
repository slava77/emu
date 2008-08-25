#include "config/PackageInfo.h"

namespace EmuFEDCrate {
	const std::string package  =  "EmuFEDCrate";
	const std::string versions =  "3.0";
	const std::string summary = "emu/emuDCS/FEDCrate";
	const std::string description = "Emu FED Crate core libraries and application software for XDAQ configuring and monitoring";
	const std::string authors = "Phillip Killewald for cms-emu-onlinesoftware";
	const std::string link = "http://cms.cern.ch";
	config::PackageInfo getPackageInfo();
        void checkPackageDependencies();
}

GETPACKAGEINFO(EmuFEDCrate);

void EmuFEDCrate::checkPackageDependencies() {}

