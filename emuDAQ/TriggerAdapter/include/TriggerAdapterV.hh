//-----------------------------------------------------------------------
// $Id: TriggerAdapterV.hh,v 2.0 2005/07/08 12:15:40 geurts Exp $
// $Log: TriggerAdapterV.hh,v $
// Revision 2.0  2005/07/08 12:15:40  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef TriggerAdapterV_H_included
#define TriggerAdapterV_H_included 1
                                                                               
#include "PackageInfo.h"
                                                                               
namespace TriggerAdapter
{
  const std::string package = "EmuDAQ/TriggerAdapter";
  const std::string versions = "1.0";
  const std::string description = "TriggerAdapter likes muons";
                                                                               
                                                                               
  toolbox::PackageInfo getPackageInfo();
  void checkPackageDependencies() throw (toolbox::PackageInfo::VersionException);
  std::set<std::string,std::less<std::string> > getPackageDependencies();
}

#endif
