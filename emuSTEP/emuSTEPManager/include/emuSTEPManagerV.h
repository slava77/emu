
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
// Version definition for emuSTEPManager
//
#ifndef _emuSTEPManagerV_h_
#define _emuSTEPManagerV_h_

#include "config/PackageInfo.h"

namespace emuSTEPManager
{
    const std::string package  =  "emuSTEP/emuSTEPManager";
    const std::string versions =  "1.0";
    const std::string description = "emuSTEPManager";
	const std::string link = "";
    const std::string authors = "Alexander Madorsky <madorsky@phys.ufl.edu>";
    const std::string summary = "EMU STEP Manager";
    config::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (config::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif

