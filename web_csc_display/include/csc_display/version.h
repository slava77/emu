#ifndef _csc_display_version_h_
#define _csc_display_version_h_

#include "config/PackageInfo.h"

#define CSC_DISPLAY_VERSION_MAJOR 1
#define CSC_DISPLAY_VERSION_MINOR 0
#define CSC_DISPLAY_VERSION_PATCH 0
#undef CSC_DISPLAY_PREVIOUS_VERSIONS

#define CSC_DISPLAY_VERSION_CODE PACKAGE_VERSION_CODE(CSC_DISPLAY_VERSION_MAJOR,CSC_DISPLAY_VERSION_MINOR, CSC_DISPLAY_VERSION_PATCH)
#ifndef CSC_DISPLAY_PREVIOUS_VERSIONS
#define CSC_DISPLAY_FULL_VERSION_LIST PACKAGE_VERSION_STRING(CSC_DISPLAY_VERSION_MAJOR,CSC_DISPLAY_VERSION_MINOR,CSC_DISPLAY_VERSION_PATCH)
#else
#define CSC_DISPLAY_FULL_VERSION_LIST CSC_DISPLAY_PREVIOUS_VERSIONS "," PACKAGE_VERSION_STRING(CSC_DISPLAY_VERSION_MAJOR,CSC_DISPLAY_VERSION_MINOR,CSC_DISPLAY_VERSION_PATCH)
#endif

namespace csc_display
{
  const std::string package = "csc_display";
  const std::string versions = CSC_DISPLAY_FULL_VERSION_LIST;
  const std::string summary = "Hello World Example";
  const std::string description = "A simple XDAQ application";
  const std::string authors = "Firman";
  const std::string link = "http://xdaq.web.cern.ch";
  config::PackageInfo getPackageInfo();
  void checkPackageDependencies() throw(config::PackageInfo::VersionException);
  std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif

