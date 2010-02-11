/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2007, CERN.                                        *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber, L. Orsini, R. Moser                             *
 *                                                                       *
 * For the licensing terms see LICENSE.                                  *
 * For the list of contributors see CREDITS.                             *
 *************************************************************************/

#include "config/version.h"
#include "xcept/version.h"
#include "xoap/version.h"
#include "toolbox/version.h"
#include "xdata/version.h"
#include "xdaq/version.h"
#include "xgi/version.h"
#include "pt/version.h"
#include "pt/appweb/version.h"

#include <string>
#include <set>

GETPACKAGEINFO(emuptappweb)

void emuptappweb::checkPackageDependencies() throw (config::PackageInfo::VersionException)
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

std::set<std::string, std::less<std::string> > emuptappweb::getPackageDependencies()
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

