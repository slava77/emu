//-----------------------------------------------------------------------
// $Id: EmuFEDCrateV.h,v 1.1 2006/01/21 20:20:14 gilmore Exp $
// $Log: EmuFEDCrateV.h,v $
// Revision 1.1  2006/01/21 20:20:14  gilmore
// *** empty log message ***
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef EmuPeripheralCrateV_h
#define EmuPeripheralCrateV_h

#include "PackageInfo.h"

namespace EmuPeripheralCrate {
	const std::string package     = "EmuPeripheralCrate";
	const std::string versions    = "1.1";
	const std::string description = "XDAQ application for EMU peripheral crate control";

	toolbox::PackageInfo getPackageInfo();
	void checkPackageDependencies() throw (toolbox::PackageInfo::VersionException);
	set<std::string, less<std::string> > getPackageDependencies();
};

#endif
