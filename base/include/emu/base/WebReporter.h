#ifndef __WEBREPORTER_H__
#define __WEBREPORTER_H__

#include "xdaq/WebApplication.h"
#include "xgi/Method.h"

#include <string>
#include <map>

namespace emu { namespace base {

class WebReporter : public virtual xdaq::WebApplication
{
public:
  XDAQ_INSTANTIATOR();

  WebReporter(xdaq::ApplicationStub *stub)
    throw (xdaq::exception::Exception);
  
protected:
  void ForEmuPage1(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);

  std::string getDateTime();
  
  virtual std::map<std::string,std::string> materialToReportOnPage1()=0;
};

}} // namespace emu::base

#endif  // ifndef __WEBREPORTER_H__

