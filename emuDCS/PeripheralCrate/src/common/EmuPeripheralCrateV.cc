//-----------------------------------------------------------------------
// $Id: EmuPeripheralCrateV.cc,v 2.0 2005/04/12 08:07:05 geurts Exp $
// $Log: EmuPeripheralCrateV.cc,v $
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include "EmuPeripheralCrateV.h"

#include "toolboxV.h"
#include "xoapV.h"
#include "xdaqV.h"

GETPACKAGEINFO(EmuPeripheralCrate)

void EmuPeripheralCrate::checkPackageDependencies()
		throw (toolbox::PackageInfo::VersionException)
{
	CHECKDEPENDENCY(toolbox)
	CHECKDEPENDENCY(xoap)
	CHECKDEPENDENCY(xdaq)
}

set<string, less<string> > xdaq::getPackageDependencies()
{
	set<string, less<string> > dependencies;

	ADDDEPENDENCY(dependencies, toolbox);
	ADDDEPENDENCY(dependencies, xoap);
	ADDDEPENDENCY(dependencies, xdaq);

	return dependencies;
}
