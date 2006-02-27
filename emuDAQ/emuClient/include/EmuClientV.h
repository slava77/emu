// $Id: EmuClientV.h,v 3.1 2006/02/27 12:49:27 banicz Exp $

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
// Version definition for MStreamIO
//
#ifndef _EmuClientV_h_
#define _EmuClientV_h_

#include "PackageInfo.h"

namespace EmuClient 
{
    const std::string package  =  "EmuClient";
    const std::string versions =  "3.0";
    const std::string description = "Example Emu Client";
    const std::string link = "";
    toolbox::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (toolbox::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
