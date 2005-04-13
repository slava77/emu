//-----------------------------------------------------------------------
// $Id: EmuRUIV.h,v 2.0 2005/04/13 10:52:59 geurts Exp $
// $Log: EmuRUIV.h,v $
// Revision 2.0  2005/04/13 10:52:59  geurts
// Makefile
//
//
//-----------------------------------------------------------------------
#ifndef EmuRUIV_h
#define EmuRUIV_h

#include "PackageInfo.h"

namespace EmuRUI {
  const string package     = "EmuRUI";
  const string versions    = "1.1";
  const string description = "XDAQ application for EMU peripheral crate RUI";

  toolbox::PackageInfo getPackageInfo();
  void checkPackageDependencies() throw (toolbox::PackageInfo::VersionException);
  set<string, less<string> > getPackageDependencies();
};

#endif  // ifndef EmuRUIV_h
