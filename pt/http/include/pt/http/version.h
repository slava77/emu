// $Id: version.h,v 1.1 2010/01/30 15:53:06 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

//
// Version definition for HTTP peer transport
//
#ifndef _pt_http_h_
#define _pt_http_h_

#include "config/PackageInfo.h"
// !!! Edit this line to reflect the latest package version !!!

#define PTHTTP_VERSION_MAJOR 3
#define PTHTTP_VERSION_MINOR 10
#define PTHTTP_VERSION_PATCH 13
// If any previous versions available E.g. #define PTHTTP_PREVIOUS_VERSIONS "3.8.0,3.8.1"
#define PTHTTP_PREVIOUS_VERSIONS "3.10.0,3.10.1,3.10.2,3.10.3,3.10.4,3.10.5,3.10.6,3.10.7,3.10.8,3.10.9,3.10.10,3.10.11,3.10.12"


//
// Template macros
//
#define PTHTTP_VERSION_CODE PACKAGE_VERSION_CODE(PTHTTP_VERSION_MAJOR,PTHTTP_VERSION_MINOR,PTHTTP_VERSION_PATCH)
#ifndef PTHTTP_PREVIOUS_VERSIONS
#define PTHTTP_FULL_VERSION_LIST  PACKAGE_VERSION_STRING(PTHTTP_VERSION_MAJOR,PTHTTP_VERSION_MINOR,PTHTTP_VERSION_PATCH)
#else 
#define PTHTTP_FULL_VERSION_LIST  PTHTTP_PREVIOUS_VERSIONS "," PACKAGE_VERSION_STRING(PTHTTP_VERSION_MAJOR,PTHTTP_VERSION_MINOR,PTHTTP_VERSION_PATCH)
#endif 
namespace pthttp
{
    const std::string package  =  "pthttp";
    const std::string versions =  PTHTTP_FULL_VERSION_LIST;
    const std::string summary = "HTTP peer transport with SOAP and CGI service implementations";
    const std::string description = "";
    const std::string authors = "Johannes Gutleber, Luciano Orsini";
    const std::string link = "http://xdaqwiki.cern.ch/index.php/Core_Tools";
    config::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (config::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif

