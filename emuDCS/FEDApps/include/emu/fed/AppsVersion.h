/*****************************************************************************\
* $Id: AppsVersion.h,v 1.2 2009/03/05 18:35:52 paste Exp $
*
* $Log: AppsVersion.h,v $
* Revision 1.2  2009/03/05 18:35:52  paste
* * Changed the name of the log files to avoid colons in file names
*
* Revision 1.1  2009/03/05 16:18:24  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
* * Added ForPageOne functionality to the Manager
*
\*****************************************************************************/

#ifndef __EMU_FED_APPSVERSION_H__
#define __EMU_FED_APPSVERSION_H__

#include "config/PackageInfo.h"

namespace emufedapps {
	const std::string package     = "emufedapps";
	const std::string versions    = "07.00.01";
	const std::string description = "Emu (CSC) FED crate XDAq applications";

	const std::string summary     = "A collection of XDAq applications for controlling end-cap muon CSC FED VME hardware.";
	const std::string authors     = "Phillip Killewald, Stan Durkin, Jason Gilmore, Jianhui Gu";
	const std::string link        = "http://www.physics.ohio-state.edu/~cms";

	config::PackageInfo getPackageInfo();
	
	void checkPackageDependencies()
	throw (config::PackageInfo::VersionException);
	
	std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif 

