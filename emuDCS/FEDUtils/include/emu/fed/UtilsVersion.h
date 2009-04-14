/*****************************************************************************\
* $Id: UtilsVersion.h,v 1.5 2009/04/14 22:59:13 paste Exp $
*
* $Log: UtilsVersion.h,v $
* Revision 1.5  2009/04/14 22:59:13  paste
* Version bump.
* Added proper reporting of hardware with "tag" exception property.
*
* Revision 1.4  2009/03/31 22:15:04  paste
* Version bump.
*
* Revision 1.3  2009/03/24 19:13:44  paste
* Fixed crashing when ending threads after an IRQ
* Made threads more robust by using slot numbers instead of DDU pointers as map indices
*
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
	const std::string package     = "FEDUtils";
	const std::string versions    = "07.03.00";
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

