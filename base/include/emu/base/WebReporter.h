#ifndef __WEBREPORTER_H__
#define __WEBREPORTER_H__

#include "emu/base/WebReportItem.h"

#include "xdaq/WebApplication.h"
#include "xgi/Method.h"

#include <string>
#include <vector>
#include <utility>
#include <time.h>

namespace emu { namespace base {

  using namespace std;

  class WebReporter : public virtual xdaq::WebApplication
  {
  public:
    XDAQ_INSTANTIATOR();

    WebReporter(xdaq::ApplicationStub *stub)
      throw (xdaq::exception::Exception);
  
  protected:
    void ForEmuPage1(xgi::Input *in, xgi::Output *out)
      throw (xgi::exception::Exception);

    pair<time_t,string> getLocalDateTime();
  
    virtual vector<emu::base::WebReportItem> materialToReportOnPage1()=0;
  };

}} // namespace emu::base

#endif  // ifndef __WEBREPORTER_H__

