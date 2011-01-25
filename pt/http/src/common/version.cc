// $Id: version.cc,v 1.3 2011/01/25 17:36:49 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#include "config/version.h"
#include "xcept/version.h"
#include "xoap/version.h"
#include "toolbox/version.h"
#include "xdata/version.h"
#include "xdaq/version.h"
#include "xgi/version.h"
#include "pt/version.h"
#include "pt/http/version.h"

#include <string>
#include <set>

GETPACKAGEINFO(pthttp)

void pthttp::checkPackageDependencies() throw (config::PackageInfo::VersionException)
{
	CHECKDEPENDENCY(config);
	CHECKDEPENDENCY(xcept);
        CHECKDEPENDENCY(toolbox);
	CHECKDEPENDENCY(xdata);
	CHECKDEPENDENCY(pt);
	CHECKDEPENDENCY(xgi);
        CHECKDEPENDENCY(xoap);
	CHECKDEPENDENCY(xdaq);
}

std::set<std::string, std::less<std::string> > pthttp::getPackageDependencies()
{
    std::set<std::string, std::less<std::string> > dependencies;
    ADDDEPENDENCY(dependencies,config);
    ADDDEPENDENCY(dependencies,xcept);
    ADDDEPENDENCY(dependencies,toolbox);
    ADDDEPENDENCY(dependencies,xdata);
    ADDDEPENDENCY(dependencies,pt);
    ADDDEPENDENCY(dependencies,xgi);
    ADDDEPENDENCY(dependencies,xoap);
    ADDDEPENDENCY(dependencies,xdaq);
    return dependencies;
}	
