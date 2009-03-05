/*****************************************************************************\
* $Id: AppsVersion.cc,v 1.1 2009/03/05 16:18:24 paste Exp $
*
* $Log: AppsVersion.cc,v $
* Revision 1.1  2009/03/05 16:18:24  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
* * Added ForPageOne functionality to the Manager
*
\*****************************************************************************/

#include "AppsVersion.h"
#include "emu/base/version.h"
#include "emu/fed/CoreVersion.h"
#include "emu/fed/UtilsVersion.h"

GETPACKAGEINFO(emufedapps)

void emufedapps::checkPackageDependencies()
throw (config::PackageInfo::VersionException)
{
	CHECKDEPENDENCY(emubase);
	CHECKDEPENDENCY(emufedcore);
	CHECKDEPENDENCY(emufedutils);
}

std::set<std::string, std::less<std::string> > emufedapps::getPackageDependencies()
{
	std::set<std::string, std::less<std::string> > dependencies;
	
	ADDDEPENDENCY(dependencies, emubase);
	ADDDEPENDENCY(dependencies, emufedcore);
	ADDDEPENDENCY(dependencies, emufedutils);
	
	return dependencies;
}

