/*****************************************************************************\
* $Id: AppsVersion.cc,v 1.3 2009/05/16 18:53:10 paste Exp $
\*****************************************************************************/

#include "emu/fed/AppsVersion.h"
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

