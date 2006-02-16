
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
// Version definition for EmuDisplayServer 
//
#ifndef _EmuDisplayServerV_h_
#define _EmuDisplayServerV_h_

#include "PackageInfo.h"

namespace EmuDisplayServer 
{
    const string package  =  "EmuDisplayServer";
    const string versions =  "1.0";
    const string description = "CMS Emu DQM Display Server";
	const string link = "http://www.phys.ufl.edu/cms/emu/dqm";
    toolbox::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (toolbox::PackageInfo::VersionException);
    set<string, less<string> > getPackageDependencies();
}

#endif
