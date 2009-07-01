/*****************************************************************************\
* $Id: UtilsVersion.cc,v 1.3 2009/07/01 14:51:40 paste Exp $
\*****************************************************************************/

#include "emu/fed/UtilsVersion.h"
#include "emu/fed/CoreVersion.h"
//#include "tstore/version.h"

GETPACKAGEINFO(EmuFEDUtils)

void EmuFEDUtils::checkPackageDependencies()
throw (config::PackageInfo::VersionException)
{
	CHECKDEPENDENCY(EmuFEDCore);
	//CHECKDEPENDENCY(tstore);
}

std::set<std::string, std::less<std::string> > EmuFEDUtils::getPackageDependencies()
{
	std::set<std::string, std::less<std::string> > dependencies;
	
	ADDDEPENDENCY(dependencies, EmuFEDCore);
	//ADDDEPENDENCY(dependencies, tstore);
	
	return dependencies;
}
