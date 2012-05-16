
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
// Version definition for emuSTEPApplication
//
#ifndef _emuSTEPApplicationV_h_
#define _emuSTEPApplicationV_h_

#include "config/PackageInfo.h"

namespace emuSTEPApplication
{
    const std::string package  =  "emuSTEP/emuSTEPApplication";
    const std::string versions =  "1.0";
    const std::string description = "emuSTEPApplication";
    const std::string link = "";
    const std::string authors = "Alexander Madorsky <madorsky@phys.ufl.edu>";
    const std::string summary = "EMU STEP base module";
    config::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (config::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif

