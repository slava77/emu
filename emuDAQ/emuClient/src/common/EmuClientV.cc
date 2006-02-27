// $Id: EmuClientV.cc,v 1.1 2006/02/27 12:49:28 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#include "toolboxV.h"
#include "xoap/version.h"
#include "xdaqV.h"
#include "EmuClientV.h"

GETPACKAGEINFO(EmuClient)

void EmuClient::checkPackageDependencies() throw (toolbox::PackageInfo::VersionException)
{
        CHECKDEPENDENCY(toolbox)
        CHECKDEPENDENCY(xoap)
	CHECKDEPENDENCY(xdaq)
}

set<string, less<string> > EmuClient::getPackageDependencies()
{
    set<string, less<string> > dependencies;
    ADDDEPENDENCY(dependencies,toolbox);
    ADDDEPENDENCY(dependencies,xoap);
    ADDDEPENDENCY(dependencies,xdaq);
    return dependencies;
}	
