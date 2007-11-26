#include "EmuTAV.h"

#include "toolbox/version.h"
#include "xdata/version.h"
#include "xdaq/version.h"
#include "xoap/version.h"

#include <string>
#include <set>

using namespace std;

GETPACKAGEINFO(EmuTA)

void EmuTA::checkPackageDependencies() throw (config::PackageInfo::VersionException)
{
    CHECKDEPENDENCY(toolbox)
    CHECKDEPENDENCY(xdata)
    CHECKDEPENDENCY(xdaq)
    CHECKDEPENDENCY(xoap)
}

std::set<std::string, std::less<std::string> > EmuTA::getPackageDependencies()
{
    std::set<std::string, std::less<std::string> > dependencies;


    ADDDEPENDENCY(dependencies,toolbox);
    ADDDEPENDENCY(dependencies,xdata);
    ADDDEPENDENCY(dependencies,xdaq);
    ADDDEPENDENCY(dependencies,xoap);

    return dependencies;
}
