/*****************************************************************************\
* $Id: UtilsVersion.h,v 1.2 2009/03/05 18:23:07 paste Exp $
*
* $Log: UtilsVersion.h,v $
* Revision 1.2  2009/03/05 18:23:07  paste
* * Added parsing for new attribute in DCC tag:  Software_Switch.  This is a required attribute, which means an update in XML file format is required.
*
* Revision 1.1  2009/03/05 16:07:52  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
*
\*****************************************************************************/

#ifndef __EMU_FED_UTILSVERSION_H__
#define __EMU_FED_UTILSVERSION_H__

#include "config/PackageInfo.h"

namespace emufedutils {
	const std::string package     = "emufedutils";
	const std::string versions    = "07.00.01";
	const std::string description = "Emu (CSC) FED crate utility libraries";

	const std::string summary     = "A collection of libraries for making programming for end-cap muon CSC FED VME hardware easier.";
	const std::string authors     = "Phillip Killewald, Stan Durkin, Jason Gilmore, Jianhui Gu";
	const std::string link        = "http://www.physics.ohio-state.edu/~cms";

	config::PackageInfo getPackageInfo();
	
	void checkPackageDependencies()
	throw (config::PackageInfo::VersionException);
	
	std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif 

