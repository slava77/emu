#ifndef __FACTCOLLECTION_H__
#define __FACTCOLLECTION_H__

#include "emu/base/Fact.h"
#include <string>
#include <vector>

namespace emu { namespace base {
  using namespace std;
  ///
  /// 
  ///
  class FactCollection {
  public:

    friend ostream& operator<<( ostream& os,       emu::base::FactCollection& f );
    friend ostream& operator<<( ostream& os, const emu::base::FactCollection& f );

    enum Source_t { DCS=0,
		    LOCAL_DQM,
		    LOCAL_DAQ,
		    XMAS,
		    FED,
		    nSources };

    const string& getSource() const { return source_; }
    const string& getRequestId() const { return requestId_; }
    const vector<emu::base::Fact>& getFacts() const { return facts_; }
    const size_t size() const { return facts_.size(); }
    emu::base::FactCollection& addFact( emu::base::Fact& fact ){ facts_.push_back( fact );  return *this; }
    emu::base::FactCollection& setRequestId( const int r );
    emu::base::FactCollection& setRequestId( const string& r ){ requestId_ = r; return *this; }
    emu::base::FactCollection& setSource( const emu::base::FactCollection::Source_t s ){ source_ = sources_[s]; return *this; }

  private:
    static const char* sources_[nSources];
    string source_;
    string requestId_;
    vector<emu::base::Fact> facts_;
  };
}} // namespace emu::base
#endif  // ifndef __FACTCOLLECTION_H__
