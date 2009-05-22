/*****************************************************************************\
* $Id: UtilsVersion.h,v 1.7 2009/05/22 11:25:25 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_UTILSVERSION_H__
#define __EMU_FED_UTILSVERSION_H__

#include "config/PackageInfo.h"

namespace emuFEDUtils {
	const std::string package     = "FEDUtils";
	const std::string versions    = "07.04.05";
	const std::string description = "Emu (CSC) FED Utility Libraries";

	const std::string summary     = "Utility libraries used by the Front End Device (FED) crate XDAQ applications (FEDApps) for the CMS Emu (CSC) subdetector";
	const std::string authors     = "Phillip Killewald, Stan Durkin, Jason Gilmore, Jianhui Gu";
	const std::string link        = "http://www.physics.ohio-state.edu/~cms";

	config::PackageInfo getPackageInfo();

	void checkPackageDependencies()
	throw (config::PackageInfo::VersionException);

	std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif

