// $Id: EmuSOAPClientV.h,v 3.1 2006/02/27 12:49:27 banicz Exp $

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
#ifndef _EmuSOAPClientV_h_
#define _EmuSOAPClientV_h_

#include "PackageInfo.h"

namespace EmuSOAPClient 
{
    const std::string package  =  "EmuSOAPClient";
    const std::string versions =  "3.0";
    const std::string description = "Example Emu SOAP Client";
    const std::string link = "";
    toolbox::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (toolbox::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
