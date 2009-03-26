/*****************************************************************************\
* $Id: AppsVersion.h,v 1.8 2009/03/26 20:19:08 paste Exp $
*
* $Log: AppsVersion.h,v $
* Revision 1.8  2009/03/26 20:19:08  paste
* Fixed DDUFPGA Diagnostic Trap display to show all 192 bits correctly
*
* Revision 1.7  2009/03/24 19:11:08  paste
* Fixed a bug that made Manager always return a Failed state after Disable command
*
* Revision 1.6  2009/03/12 14:29:58  paste
* * Fixed image display bug in Monitor
* * Set the firmware routines to explicitly use /tmp instead of relying on the running directory being writable
*
* Revision 1.5  2009/03/09 16:03:16  paste
* * Updated "ForPage1" routine in Manager with new routines from emu::base::WebReporter
* * Updated inheritance in wake of changes to emu::base::Supervised
* * Added Supervised class to separate XDAQ web-based applications and those with a finite state machine
*
* Revision 1.4  2009/03/05 22:04:09  paste
* * Fixed a minor bug involving DCC Expert commands
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
	const std::string package     = "FEDApps";
	const std::string versions    = "07.01.01";
	const std::string description = "Emu (CSC) FED XDAQ Applications";

	const std::string summary     = "Applications to control and monitor the Front End Device (FED) crates for the CMS Emu (CSC) subdetector";
	const std::string authors     = "Phillip Killewald, Stan Durkin, Jason Gilmore, Jianhui Gu";
	const std::string link        = "http://www.physics.ohio-state.edu/~cms";

	config::PackageInfo getPackageInfo();
	
	void checkPackageDependencies()
	throw (config::PackageInfo::VersionException);
	
	std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif 

