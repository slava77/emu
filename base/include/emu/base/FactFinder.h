#ifndef __FACTFINDER_H__
#define __FACTFINDER_H__

#include "emu/base/FactRequest.h"
#include "emu/base/FactCollection.h"
#include "emu/base/Stopwatch.h"

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

    /// 
    /// Ctor.
    /// @param stub XDAQ application stub.
    /// @param source Source type of the application.
    /// @param periodInSec Period in seconds of regular scheduled fact finding. If 0, no periodic fact finding will be scheduled.
    ///
    /// @return 
    ///
    FactFinder( xdaq::ApplicationStub *stub, emu::base::FactCollection::Source_t source, int periodInSec=0 )
      throw (xcept::Exception);

    /// 
    /// Dtor.
    ///
    ~FactFinder();

    /// 
    /// Callback on receiving fact request SOAP.
    /// @param message SOAP to send.
    ///
    /// @return SOAP reply. 
    ///
    xoap::MessageReference onFactRequest( xoap::MessageReference message )
      throw (xoap::exception::Exception);
  
  private:
    toolbox::BSem                            factFinderBSem_; ///< Mutex for access to member containers factRequestCollections_ and factsToSend_

  protected:
    xdata::String  expertSystemURL_; ///< The URL of the expert system. Exported infospace parameter.
    xdata::Boolean isFactFinderInDebugMode_; ///< If TRUE, debug messages are printed to stdout, and the expert system replies synchronously and doesn't store the facts. Exported infospace parameter.

    toolbox::task::WorkLoop *factWorkLoop_;
    toolbox::task::ActionSignature *factRequestSignature_;
    toolbox::task::ActionSignature *sendFactsSignature_;

    void sendFacts();
    void sendFact( const string& componentId,             const string& factType );
    void sendFact( const emu::base::Component& component, const string& factType );
  
    virtual emu::base::Fact           findFact ( const emu::base::Component& component, const string& factType ) = 0; ///< Find a fact specified by component and type. To be implemented by the derived class.
    virtual emu::base::FactCollection findFacts() = 0; ///< Find all facts. To be implemented by the derived class.

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

    emu::base::FactCollection::Source_t      source_; ///< Source type of the derived application.
    unsigned int                             maxQueueLength_; ///< Let at most this many fact requests or facts accumulate.
    deque<emu::base::FactRequestCollection>  factRequestCollections_; ///< The container of fact requests received.
    deque<emu::base::FactCollection>         factsToSend_; ///< The container of facts to send.
    xdaq::ApplicationDescriptor             *targetDescriptor_;	///< The app descriptor of the expert system.
    emu::base::Stopwatch                    *stopwatch_; ///< A stopwatch to time the moratirium on sending following a SOAP timeout.
    time_t                                   moratoriumAfterTimeout_; ///< The moratoriumin seconds on sending further facts following a SOAP timeout.
    bool                                     isDisabled_; ///< If TRUE, sending facts is disabled.
  };

}} // namespace emu::base

#endif  // ifndef __FACTFINDER_H__
