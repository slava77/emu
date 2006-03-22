#include "EmuDAQManagerV.h"

#include "toolbox/include/toolboxV.h"
#include "xdaq/include/xdaqV.h"
#include "xoap/include/xoap/version.h"

#include <string>
#include <set>

GETPACKAGEINFO(EmuDAQManager)

void EmuDAQManager::checkPackageDependencies() throw (toolbox::PackageInfo::VersionException)
{
    CHECKDEPENDENCY(toolbox)
    CHECKDEPENDENCY(xdaq)
    CHECKDEPENDENCY(xoap)
}

std::set<std::string, std::less<std::string> > EmuDAQManager::getPackageDependencies()
{
    std::set<std::string, std::less<std::string> > dependencies;


    ADDDEPENDENCY(dependencies,toolbox);
    ADDDEPENDENCY(dependencies,xdaq);
    ADDDEPENDENCY(dependencies,xoap);

    return dependencies;
}
