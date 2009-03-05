/*****************************************************************************\
* $Id: CoreVersion.h,v 1.1 2009/03/05 16:02:14 paste Exp $
*
* $Log: CoreVersion.h,v $
* Revision 1.1  2009/03/05 16:02:14  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
*
\*****************************************************************************/

#ifndef __EMU_FED_COREVERSION_H__
#define __EMU_FED_COREVERSION_H__

#include "config/PackageInfo.h"

namespace emufedcore {
	const std::string package     = "emufedcore";
	const std::string versions    = "07.00.00";
	const std::string description = "Emu (CSC) FED crate core libraries";

	const std::string summary     = "A collection of libraries for controlling end-cap muon CSC FED VME hardware.";
	const std::string authors     = "Phillip Killewald, Stan Durkin, Jason Gilmore, Jianhui Gu";
	const std::string link        = "http://www.physics.ohio-state.edu/~cms";

	config::PackageInfo getPackageInfo();
	
	void checkPackageDependencies()
	throw (config::PackageInfo::VersionException);
	
	std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif 

