/*****************************************************************************\
* $Id: CoreVersion.h,v 1.4 2009/03/24 19:16:30 paste Exp $
*
* $Log: CoreVersion.h,v $
* Revision 1.4  2009/03/24 19:16:30  paste
* Version bump
*
* Revision 1.3  2009/03/09 23:12:44  paste
* * Fixed a minor bug in DCC MPROM ID/Usercode reading
* * Fixed a major bug in RESET path firmware loading
* * Added debug mode for CAEN reading/writing
*
* Revision 1.2  2009/03/05 21:57:55  paste
* * CAEN Bus Errors no longer throw exceptions.
* This is a temporary fix to a problem where the DCC does not send a DTACK on certain register writes,
* causing the controller to time out and set a bus error.  The write does not fail, so this is not
* actually an exception.
*
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
	const std::string package     = "FEDCore";
	const std::string versions    = "07.01.00";
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

