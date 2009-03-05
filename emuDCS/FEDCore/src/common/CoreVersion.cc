/*****************************************************************************\
* $Id: CoreVersion.cc,v 1.1 2009/03/05 16:02:14 paste Exp $
*
* $Log: CoreVersion.cc,v $
* Revision 1.1  2009/03/05 16:02:14  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
*
\*****************************************************************************/

#include "emu/fed/CoreVersion.h"

GETPACKAGEINFO(emufedcore)

void emufedcore::checkPackageDependencies()
throw (config::PackageInfo::VersionException)
{
}

std::set<std::string, std::less<std::string> > emufedcore::getPackageDependencies()
{
	std::set<std::string, std::less<std::string> > dependencies;
	return dependencies;
}

