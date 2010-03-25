#ifndef __emu_daq_rui_FactTypes_h__
#define __emu_daq_rui_FactTypes_h__

namespace emu { namespace daq { namespace rui {

  class RUIStatusFact {
  public:
    enum ParameterName_t { runType, state, nEventsRead, isWritingToFile, nEventsWritten, nParameters };
    static const char*  getTypeName() { return typeName_; }
    static const string getParameterName( const ParameterName_t p ) { return parameterNames_[p]; }
  protected:
    static const char* const typeName_;
    static const char* const parameterNames_[nParameters];
  };
  const char* const RUIStatusFact::typeName_ = "ruiStatusFact";
  const char* const RUIStatusFact::parameterNames_[] = { "runType", "state", "nEventsRead", "isWritingToFile", "nEventsWritten" };

}}} // namespace emu::daq::rui

#endif
