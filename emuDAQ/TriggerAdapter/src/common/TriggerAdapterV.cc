//-----------------------------------------------------------------------
// $Id: TriggerAdapterV.cc,v 2.0 2005/07/08 12:15:41 geurts Exp $
// $Log: TriggerAdapterV.cc,v $
// Revision 2.0  2005/07/08 12:15:41  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include "toolboxV.h"
#include "xoapV.h"
#include "xdaqV.h"
#include "TriggerAdapterV.hh"

GETPACKAGEINFO(TriggerAdapter);

void TriggerAdapter::checkPackageDependencies() throw (toolbox::PackageInfo::VersionException) {
  CHECKDEPENDENCY(toolbox);
  CHECKDEPENDENCY(xdaq);
  CHECKDEPENDENCY(xoap);
}

std::set<std::string,std::less<std::string> > TriggerAdapter::getPackageDependencies() {
  std::set<std::string,std::less<std::string> > deps;
  ADDDEPENDENCY(deps,toolbox);
  ADDDEPENDENCY(deps,xoap);
  ADDDEPENDENCY(deps,xdaq);
  return deps;
}
