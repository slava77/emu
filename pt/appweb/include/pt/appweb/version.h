/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2007, CERN.                                        *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber, L. Orsini, R. Moser                             *
 *                                                                       *
 * For the licensing terms see LICENSE.                                  *
 * For the list of contributors see CREDITS.                             *
 *************************************************************************/

//
// Version definition for HTTP peer transport
//
#ifndef _pt_appweb_h_
#define _pt_appweb_h_

#include "config/PackageInfo.h"
// !!! Edit this line to reflect the latest package version !!!

// #define PTAPPWEB_VERSION_MAJOR 1
// #define PTAPPWEB_VERSION_MINOR 12
// #define PTAPPWEB_VERSION_PATCH 1
// // If any previous versions available E.g. #define PTAPPWEB_PREVIOUS_VERSIONS "3.8.0,3.8.1"
// #define PTAPPWEB_PREVIOUS_VERSIONS "1.12.0"

#define EMUPTAPPWEB_VERSION_MAJOR 1
#define EMUPTAPPWEB_VERSION_MINOR 0
#define EMUPTAPPWEB_VERSION_PATCH 0
// If any previous versions available E.g. #define EMUPTAPPWEB_PREVIOUS_VERSIONS "3.8.0,3.8.1"
#define EMUPTAPPWEB_PREVIOUS_VERSIONS ""


//
// Template macros
//
#define EMUPTAPPWEB_VERSION_CODE PACKAGE_VERSION_CODE(EMUPTAPPWEB_VERSION_MAJOR,EMUPTAPPWEB_VERSION_MINOR,EMUPTAPPWEB_VERSION_PATCH)
#ifndef EMUPTAPPWEB_PREVIOUS_VERSIONS
#define EMUPTAPPWEB_FULL_VERSION_LIST  PACKAGE_VERSION_STRING(EMUPTAPPWEB_VERSION_MAJOR,EMUPTAPPWEB_VERSION_MINOR,EMUPTAPPWEB_VERSION_PATCH)
#else 
#define EMUPTAPPWEB_FULL_VERSION_LIST  EMUPTAPPWEB_PREVIOUS_VERSIONS "," PACKAGE_VERSION_STRING(EMUPTAPPWEB_VERSION_MAJOR,EMUPTAPPWEB_VERSION_MINOR,EMUPTAPPWEB_VERSION_PATCH)
#endif 

namespace emuptappweb
{
    const std::string package  = "emuptappweb";
    const std::string versions = EMUPTAPPWEB_FULL_VERSION_LIST;
    const std::string summary = "APPWEB peer transport with SOAP and CGI over HTTP service implementations modified to comply with SOAP 1.1";
    const std::string description = "Based on daq/pt/appweb v1.12.1";
		const std::string authors = "Johannes Gutleber, Luciano Orsini, Roland Moser, Karoly Banicz";
    const std::string link = "http://xdaqwiki.cern.ch/index.php/Core_Tools";
    config::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (config::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif

