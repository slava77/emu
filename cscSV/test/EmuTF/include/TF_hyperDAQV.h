#ifndef __TF_HYPERDAQ_V_H__
#define __TF_HYPERDAQ_V_H__

#include "PackageInfo.h"

namespace TF_hyperDAQ {
	const string package     = "TF_hyperDAQ";
	const string versions    = "1.0";
	const string description = "dummy XDAQ application to test CSC Supervisor";
	const string link        = "http://localhost:40000/";

	toolbox::PackageInfo getPackageInfo();
	void checkPackageDependencies() throw (toolbox::PackageInfo::VersionException);
	set<string, less<string> > getPackageDependencies();
};

#endif  // ifndef __TF_HYPERDAQ_V_H__
// vim: set sw=4 ts=4:
