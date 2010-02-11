#ifndef __FACTFINDER_H__
#define __FACTFINDER_H__

#include "emu/base/FactRequest.h"
#include "emu/base/FactCollection.h"

#include "xdaq/WebApplication.h"

#include "xdata/String.h"
#include "xdata/Boolean.h"

#include "toolbox/task/WorkLoop.h"
#include "toolbox/task/WorkLoopFactory.h"
#include "toolbox/BSem.h"

#include "toolbox/task/Timer.h"

#include "xoap/SOAPFault.h"

#include <string>
#include <vector>
#include <deque>
#include <utility>
#include <time.h>

#ifdef CSCTF
#include "ts/framework/CellAbstract.h"
#include "ts/worker/include/CellContext.h"
#endif


namespace emu { namespace base {

  using namespace std;

  class FactFinder 
#ifndef CSCTF
    : public virtual xdaq::WebApplication
#else
    : public virtual tsframework::CellAbstract
#endif
    , public toolbox::task::TimerListener
  {
  public:
    XDAQ_INSTANTIATOR();

    FactFinder( xdaq::ApplicationStub *stub, emu::base::FactCollection::Source_t source, int periodInSec=0 )
      throw (xcept::Exception);
    xoap::MessageReference onFactRequest( xoap::MessageReference message )
      throw (xoap::exception::Exception);
  
  protected:
    xdata::String  expertSystemURL_;
    xdata::Boolean isFactFinderInDebugMode_;

    toolbox::task::WorkLoop *factWorkLoop_;
    toolbox::task::ActionSignature *factRequestSignature_;
    toolbox::task::ActionSignature *sendFactsSignature_;

    void sendFacts();
    pair<time_t,string> getLocalDateTime();
  
    virtual emu::base::Fact           findFact ( const string& component, const string& factType ) = 0;
    virtual emu::base::FactCollection findFacts() = 0;

  private:
    void                              timeExpired(toolbox::task::TimerEvent& e);
    emu::base::FactRequestCollection  parseFactRequestSOAP( xoap::MessageReference message ) const throw (xoap::exception::Exception);
    xoap::MessageReference            createFactsSOAP( const emu::base::FactCollection& factCollection ) const;
    xoap::MessageReference            createRequestAcknowlegdementSOAP() const;
    xoap::MessageReference            postSOAP( xoap::MessageReference message, const string& URL, const string& SOAPAction );
    emu::base::FactCollection         collectFacts( const FactRequestCollection& requestCollection );
    bool                              sendFactsInWorkLoop(toolbox::task::WorkLoop *wl);
    bool                              sendFactsInSOAP( const emu::base::FactCollection& facts );
    string                            getSOAPFaultCode( xoap::SOAPFault soapFault );
    void                              findTargetDescriptor();

    toolbox::BSem                            factFinderBSem_;
    emu::base::FactCollection::Source_t      source_;    
    unsigned int                             maxQueueLength_;
    deque<emu::base::FactRequestCollection>  factRequestCollections_;
    deque<emu::base::FactCollection>         factsToSend_;
    xdaq::ApplicationDescriptor             *targetDescriptor_;
  };

}} // namespace emu::base

#endif  // ifndef __FACTFINDER_H__
