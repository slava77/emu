#include "EmuDAQtesterV.h"

#include "toolbox/include/toolboxV.h"
#include "xdaq/include/xdaqV.h"
#include "xoap/include/xoap/version.h"

#include <string>
#include <set>

GETPACKAGEINFO(EmuDAQtester)

void EmuDAQtester::checkPackageDependencies() throw (toolbox::PackageInfo::VersionException)
{
    CHECKDEPENDENCY(toolbox)
    CHECKDEPENDENCY(xdaq)
    CHECKDEPENDENCY(xoap)
}

std::set<std::string, std::less<std::string> > EmuDAQtester::getPackageDependencies()
{
    std::set<std::string, std::less<std::string> > dependencies;


    ADDDEPENDENCY(dependencies,toolbox);
    ADDDEPENDENCY(dependencies,xdaq);
    ADDDEPENDENCY(dependencies,xoap);

    return dependencies;
}
