
/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

//
// Version definition for EmuTFDisplayClient 
//
#ifndef _EmuTFDisplayClientV_h_
#define _EmuTFDisplayClientV_h_

#include "config/PackageInfo.h"

namespace EmuTFDisplayClient 
{
    const std::string package  =  "emuDQM/EmuTFDisplayClient";
    const std::string versions =  "1.0";
    const std::string description = "CMS Emu DQM Display Client";
    const std::string link = "http://www.phys.ufl.edu/cms/emu/dqm";
    const std::string authors = "Victor Barashko <barvic@ufl.edu>";
    const std::string summary = "Emu DQM Display Client Application";
    config::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (config::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
