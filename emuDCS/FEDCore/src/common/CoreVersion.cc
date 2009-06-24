/*****************************************************************************\
* $Id: CoreVersion.cc,v 1.4 2009/06/24 04:53:51 paste Exp $
\*****************************************************************************/

#include "emu/fed/CoreVersion.h"

GETPACKAGEINFO(EmuFEDCore)

void EmuFEDCore::checkPackageDependencies()
throw (config::PackageInfo::VersionException)
{
}

std::set<std::string, std::less<std::string> > EmuFEDCore::getPackageDependencies()
{
	std::set<std::string, std::less<std::string> > dependencies;
	return dependencies;
}

