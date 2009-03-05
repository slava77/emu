/*****************************************************************************\
* $Id: UtilsVersion.cc,v 1.1 2009/03/05 16:07:52 paste Exp $
*
* $Log: UtilsVersion.cc,v $
* Revision 1.1  2009/03/05 16:07:52  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
*
\*****************************************************************************/

#include "emu/fed/UtilsVersion.h"
#include "emu/fed/CoreVersion.h"

GETPACKAGEINFO(emufedutils)

void emufedutils::checkPackageDependencies()
throw (config::PackageInfo::VersionException)
{
	CHECKDEPENDENCY(emufedcore);
}

std::set<std::string, std::less<std::string> > emufedutils::getPackageDependencies()
{
	std::set<std::string, std::less<std::string> > dependencies;
	
	ADDDEPENDENCY(dependencies, emufedcore);
	
	return dependencies;
}

