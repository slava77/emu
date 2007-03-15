#include "EmuTFMonitorV.h"

#include "toolboxV.h"
#include "xoap/version.h"
#include "xdaqV.h"

GETPACKAGEINFO(EmuTFMonitor)

void EmuTFMonitor::checkPackageDependencies()
                throw (toolbox::PackageInfo::VersionException)
{
        CHECKDEPENDENCY(toolbox)
        CHECKDEPENDENCY(xoap)
        CHECKDEPENDENCY(xdaq)
}

set<string, less<string> > xdaq::getPackageDependencies()
{
        set<string, less<string> > dependencies;

        ADDDEPENDENCY(dependencies, toolbox);
        ADDDEPENDENCY(dependencies, xoap);
        ADDDEPENDENCY(dependencies, xdaq);

        return dependencies;
}


