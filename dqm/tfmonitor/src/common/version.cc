//
// EmuTFMonitorV.cc

#include "emu/dqm/tfmonitor/version.h"

#include "config/version.h"
#include "toolbox/version.h"
#include "xoap/version.h"
#include "xdaq/version.h"

GETPACKAGEINFO(EmuTFMonitor)

void EmuTFMonitor::checkPackageDependencies()
throw (config::PackageInfo::VersionException)
{
  CHECKDEPENDENCY(toolbox)
  CHECKDEPENDENCY(xoap)
  CHECKDEPENDENCY(xdaq)
}

std::set<std::string, std::less<std::string> > xdaq::getPackageDependencies()
{
  std::set<std::string, std::less<std::string> > dependencies;

  ADDDEPENDENCY(dependencies, toolbox);
  ADDDEPENDENCY(dependencies, xoap);
  ADDDEPENDENCY(dependencies, xdaq);

  return dependencies;
}

