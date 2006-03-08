#ifndef __CSC_SUPERVISOR_V_H__
#define __CSC_SUPERVISOR_V_H__

#include "PackageInfo.h"

namespace CSCSupervisor {
	const string package     = "CSCSupervisor";
	const string versions    = "1.0";
	const string description = "CSC supervisor";
	const string link        = "http://localhost:40000/";

	toolbox::PackageInfo getPackageInfo();
	void checkPackageDependencies() throw (toolbox::PackageInfo::VersionException);
	set<string, less<string> > getPackageDependencies();
};

#endif  // ifndef __CSC_SUPERVISOR_V_H__
// vim: set ai sw=4 ts=4:
