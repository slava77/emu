//-----------------------------------------------------------------------
// $Id: EmuRUIV.cc,v 2.0 2005/04/13 10:52:59 geurts Exp $
// $Log: EmuRUIV.cc,v $
// Revision 2.0  2005/04/13 10:52:59  geurts
// Makefile
//
//
//-----------------------------------------------------------------------
#include "EmuRUIV.h"

#include "xdaqV.h"

GETPACKAGEINFO(EmuRUI)

void EmuRUI::checkPackageDependencies()
		throw (toolbox::PackageInfo::VersionException)
{
	CHECKDEPENDENCY(xdaq)
}

set<string, less<string> > xdaq::getPackageDependencies()
{
	set<string, less<string> > dependencies;

	ADDDEPENDENCY(dependencies, xdaq);

	return dependencies;
}
