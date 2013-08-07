#include "config/version.h"
#include "xcept/version.h"
#include "xdaq/version.h"
#include "csc_display/version.h"

GETPACKAGEINFO(csc_display)

void csc_display::checkPackageDependencies() throw(config::PackageInfo::VersionException)
{
  CHECKDEPENDENCY(config);
  CHECKDEPENDENCY(xcept);
  CHECKDEPENDENCY(xdaq);
}

std::set<std::string, std::less<std::string> > csc_display::getPackageDependencies()
{
  std::set<std::string, std::less<std::string> > dependencies;

  ADDDEPENDENCY(dependencies, config);
  ADDDEPENDENCY(dependencies, xcept);
  ADDDEPENDENCY(dependencies, xdaq);

  return dependencies;
}

