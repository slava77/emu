//-----------------------------------------------------------------------
// $Id: EmuPeripheralCrateV.h,v 2.0 2005/04/12 08:07:03 geurts Exp $
// $Log: EmuPeripheralCrateV.h,v $
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
