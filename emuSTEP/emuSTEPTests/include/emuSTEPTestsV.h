
/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.                                        *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini                                    *
 *                                                                       *
 * For the licensing terms see LICENSE.                                  *
 * For the list of contributors see CREDITS.                             *
 *************************************************************************/

//
// Version definition for emuSTEPTests
//
#ifndef _emuSTEPTestsV_h_
#define _emuSTEPTestsV_h_

#include "config/PackageInfo.h"

namespace emuSTEPTests
{

  //const std::string package  =  "emuSTEP/emuSTEPTests";
    const std::string package  =  "emuSTEP/emuSTEPTests";
    const std::string versions =  "1.0";
    const std::string description = "emuSTEPTests";
	const std::string link = "";
    const std::string authors = "Alexander Madorsky <madorsky@phys.ufl.edu>";
    const std::string summary = "EMU STEP test module";
    config::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (config::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif

