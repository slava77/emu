/*****************************************************************************\
* $Id: CoreVersion.h,v 1.19 2009/07/11 19:36:44 paste Exp $
\*****************************************************************************/

#ifndef __EMU_FED_COREVERSION_H__
#define __EMU_FED_COREVERSION_H__

#include "config/PackageInfo.h"

namespace EmuFEDCore {
	const std::string package     = "EmuFEDCore";
	const std::string versions    = "07.06.03";
	const std::string description = "Emu (CSC) FED Core Libraries";

	const std::string summary     = "Core libraries for controlling Front End Device (FED) crates for the CMS Emu (CSC) subdetector";
	const std::string authors     = "Phillip Killewald, Stan Durkin, Jason Gilmore, Jianhui Gu";
	const std::string link        = "http://www.physics.ohio-state.edu/~cms";

	config::PackageInfo getPackageInfo();

	void checkPackageDependencies()
	throw (config::PackageInfo::VersionException);

	std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif

