//
// version.cc

#include "emu/daq/ta/version.h"

#include "toolbox/version.h"
#include "xoap/version.h"
#include "xdata/version.h"
#include "xdaq/version.h"

using namespace std;

GETPACKAGEINFO(emudaqta)

void emudaqta::checkPackageDependencies()
		throw (config::PackageInfo::VersionException)
{
	CHECKDEPENDENCY(toolbox)
	CHECKDEPENDENCY(xoap)
	CHECKDEPENDENCY(xdata)
	CHECKDEPENDENCY(xdaq)
}

set<string, less<string> > xdaq::getPackageDependencies()
{
	set<string, less<string> > dependencies;

	ADDDEPENDENCY(dependencies, toolbox);
	ADDDEPENDENCY(dependencies, xoap);
	ADDDEPENDENCY(dependencies, xdata);
	ADDDEPENDENCY(dependencies, xdaq);

	return dependencies;
}
