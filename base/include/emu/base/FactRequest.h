#ifndef __FACTREQUEST_H__
#define __FACTREQUEST_H__

#include "emu/base/Fact.h"
#include <string>
#include <vector>

namespace emu { namespace base {
  using namespace std;

  ///
  /// 
  ///
  class FactRequest{
  public:

    friend ostream& operator<<( ostream& os,       emu::base::FactRequest& fr );
    friend ostream& operator<<( ostream& os, const emu::base::FactRequest& fr );

    FactRequest(){}
    FactRequest( const emu::base::Component& component, const string& factType )
      : component_( component ),
	factType_ ( factType  )
    {}

    const emu::base::Component& getComponent()   const { return component_;   }
    const string&               getFactType()    const { return factType_;    }
    emu::base::FactRequest& setComponentId( const string& componentId     ){ component_.addId( componentId ); return *this; }
    emu::base::FactRequest& setComponent  ( const emu::base::Component& c ){ component_ = c;                  return *this; }
    emu::base::FactRequest& setFactType   ( const string& factType        ){ factType_  = factType;           return *this; }
  private:
    emu::base::Component component_;
    string               factType_;
  };

  ///
  /// 
  ///
  class FactRequestCollection {
  public:

    friend ostream& operator<<( ostream& os,       emu::base::FactRequestCollection& frc );
    friend ostream& operator<<( ostream& os, const emu::base::FactRequestCollection& frc );

    const string& getRequestId() const { return requestId_; }
    const vector<emu::base::FactRequest>& getRequests() const { return requests_; }
    emu::base::FactRequestCollection& addRequest( const emu::base::FactRequest& fr ){ requests_.push_back( fr );  return *this; }
    emu::base::FactRequestCollection& setRequestId( const string& id ){ requestId_ = id;  return *this; }
  private:
    string requestId_;
    vector<emu::base::FactRequest> requests_;
  };
}} // namespace emu::base
#endif  // ifndef __FACTREQUEST_H__
