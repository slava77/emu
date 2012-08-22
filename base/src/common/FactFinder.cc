// Expert system web service namespace URI
#define ESWS_NS_URI "http://www.cern.ch/cms/csc/dw/ws/factCollectionInput"
// Expert system data model namespace URI
#define ESD_NS_URI "http://www.cern.ch/cms/csc/dw/model"
// Expert system input service namespace URI
#define ESIS_NS_URI "http://www.cern.ch/cms/csc/dw/service"

#include "emu/base/FactFinder.h"

#include <exception>
#include <unistd.h> // for usleep

#include "xcept/tools.h"
#include "xdaq/NamespaceURI.h"
#include "xoap/domutils.h"
#include "xoap/MessageFactory.h"
#include "xoap/MessageReference.h"
#include "xoap/Method.h"
#include "xoap/SOAPBody.h"
#include "xoap/SOAPBodyElement.h"
#include "xoap/SOAPEnvelope.h"

#include "toolbox/task/TimerFactory.h"
#include "toolbox/TimeInterval.h" 

#include "xdaq/XceptSerializer.h"

// For emu::base::FactFinder::Application::postSOAP
#include "toolbox/net/URL.h"
#include "pt/PeerTransportAgent.h"
#include "pt/PeerTransportSender.h"
#include "pt/SOAPMessenger.h"

#ifdef CSCTF
#include "ts/framework/CellAbstract.h"
#include "ts/worker/include/CellContext.h"
#endif

using namespace std;

emu::base::FactFinder::FactFinder( xdaq::ApplicationStub *stub, emu::base::FactCollection::Source_t source, int periodInSec )
  throw (xcept::Exception)
#ifdef CSCTF
  : tsframework::CellAbstract( stub ),
#else
  : xdaq::WebApplication( stub ),
#endif
    factFinderBSem_( toolbox::BSem::EMPTY ),
    expertSystemURL_( "NOT_YET_KNOWN" ), // From this value we know it hasn't been configured yet.
    source_( source ),
    maxQueueLength_( 1000 ),
    targetDescriptor_( NULL ),
    stopwatch_( new emu::base::Stopwatch() ),
    moratoriumAfterTimeout_( 60 ),
    isDisabled_( false ){
  xoap::bind( this, &emu::base::FactFinder::onFactRequest, "factRequestCollection",  ESD_NS_URI );

  isFactFinderInDebugMode_ = false;
  getApplicationInfoSpace()->fireItemAvailable( "expertSystemURL"        ,  &expertSystemURL_         );
  getApplicationInfoSpace()->fireItemAvailable( "isFactFinderInDebugMode",  &isFactFinderInDebugMode_ );

  stringstream workLoopName;
  workLoopName << getApplicationDescriptor()->getClassName() << "." << getApplicationDescriptor()->getInstance();
  factWorkLoop_ = toolbox::task::getWorkLoopFactory()->getWorkLoop( workLoopName.str(), "waiting" );
  sendFactsSignature_  = toolbox::task::bind( this, &emu::base::FactFinder::sendFactsInWorkLoop, "sendFactsInWorkLoop" );
  // We do not submit task "sendFactsInWorkLoop" yet as "expertSystemURL" is not yet known inside the constructor.
  // It is going to be submitted once (and from then on resubmitted by itself) if the workloop is not active yet.
  // At the same time, the workloop is going to be activated and the active workloop will mean the task has been submitted.
  // Therefore we do not activate the workloop here, either.

  stringstream timerName;
  timerName << "FactFinderTimer." << getApplicationDescriptor()->getClassName() << "." << getApplicationDescriptor()->getInstance();
  if ( periodInSec > 0 ){
    try{
      toolbox::task::Timer * timer = toolbox::task::getTimerFactory()->createTimer( timerName.str() );
      toolbox::TimeInterval interval(periodInSec,0); // period in sec 
      toolbox::TimeVal start( toolbox::TimeVal::gettimeofday() + toolbox::TimeVal( 2, 0 ) ); // start in 2 seconds from now
      timer->scheduleAtFixedRate( start, this, interval,  0, "" );
    } catch(xcept::Exception& e){
      stringstream ss;
      ss << "Failed to create " << timerName << " , therefore no scheduled fact finding will be done: " << xcept::stdformat_exception_history(e);
      LOG4CPLUS_ERROR( getApplicationLogger(), ss.str() );
      XCEPT_DECLARE( xcept::Exception, eObj, ss.str() );
      this->notifyQualified( "error", eObj );
    }
  }
  factFinderBSem_.give();
}

emu::base::FactFinder::~FactFinder(){
  delete stopwatch_;
}

xoap::MessageReference
emu::base::FactFinder::onFactRequest( xoap::MessageReference message )
  throw (xoap::exception::Exception){
  if ( isFactFinderInDebugMode_.value_ ) cout << "*** emu::base::FactFinder::onFactRequest" << endl;

//   emu::base::FactRequest factRequest = emu::base::FactFinder::parseFactRequestSOAP( message );
//   emu::base::FactCollection factCollection = collectFacts( factRequest );
//   return emu::base::FactFinder::createFactsSOAP( factCollection );

  // Parse message:
  emu::base::FactRequestCollection factRequestCollection = emu::base::FactFinder::parseFactRequestSOAP( message );
  // Queue request for handling:
  factFinderBSem_.take();
  factRequestCollections_.push_back( factRequestCollection );
  // If queue is too long, drop the oldest requests:
  while ( factRequestCollections_.size() > maxQueueLength_ ){
    factRequestCollections_.pop_front();
    LOG4CPLUS_WARN( getApplicationLogger(), "Incoming request queue full. Discarding the oldest request." );
  }
  factFinderBSem_.give();

  // Handle them in another thread:
  if ( ! isDisabled_ && ! factWorkLoop_->isActive() ){
    factWorkLoop_->activate();
    factWorkLoop_->submit( sendFactsSignature_ );
  }

  if ( isFactFinderInDebugMode_.value_ ) cout << "emu::base::FactFinder::onFactRequest ***" << endl;
  return emu::base::FactFinder::createRequestAcknowlegdementSOAP();
}

emu::base::FactRequestCollection
emu::base::FactFinder::parseFactRequestSOAP( xoap::MessageReference message ) const
  throw (xoap::exception::Exception){
  if ( isFactFinderInDebugMode_.value_ ) cout << "*** emu::base::FactFinder::parseFactRequestSOAP" << endl;
  emu::base::FactRequestCollection factRequestCollection;
  try {
      // Get DOM document from SOAP
      DOMDocument *doc = message->getSOAPPart().getEnvelope().getBody().getDOMNode()->getOwnerDocument();
      if ( isFactFinderInDebugMode_.value_ ) cout << "doc: " << doc << endl;
      if ( doc == NULL ) return factRequestCollection;
//       // Get request id
//       DOMNodeList *l = doc->getElementsByTagNameNS( xoap::XStr( ESD_NS_URI ), xoap::XStr("factRequestCollection") );
//       if ( l->getLength() > 0 ){
// 	DOMNamedNodeMap *a = l->item(0)->getAttributes();
// 	if ( a ){
// 	  DOMNode *ri = a->getNamedItem( xoap::XStr("requestId") );
// 	  if ( ri ) factRequestCollection.setRequestId( xoap::XMLCh2String( ri->getNodeValue() ) ); 
// 	}
//       }
      // Get request id
      DOMNodeList *ids = doc->getElementsByTagNameNS( xoap::XStr( ESD_NS_URI ), xoap::XStr("requestId") );
      if ( ids->getLength() > 0 ){
	factRequestCollection.setRequestId( xoap::XMLCh2String( ids->item(0)->getTextContent() ) );
      }
      // Get the requested items
      DOMNodeList *requests = doc->getElementsByTagNameNS( xoap::XStr( ESD_NS_URI ), xoap::XStr("factRequest") );
      for( unsigned int i=0; i<requests->getLength(); i++ ){
	emu::base::FactRequest factRequest;
	DOMNode *request = requests->item(i);
	DOMNodeList *nodeList = request->getChildNodes();
	for( unsigned int j=0; j<nodeList->getLength(); j++ ){
	    DOMNode *node = nodeList->item(j);
	    if( node->getNodeType() == DOMNode::ELEMENT_NODE ){
	      string name = xoap::XMLCh2String( node->getLocalName() );
	      if ( name == "component_id" ) factRequest.setComponentId( xoap::XMLCh2String( node->getTextContent() ) );
	      if ( name == "factType"     ) factRequest.setFactType   ( xoap::XMLCh2String( node->getTextContent() ) );
	    }
	}
	factRequestCollection.addRequest( factRequest );
      }
  }
  catch(xcept::Exception& e){
    XCEPT_RETHROW(xoap::exception::Exception, "Failed to parse fact request SOAP message ", e);
  }
  catch(...){
    XCEPT_RAISE(xoap::exception::Exception, "Failed to parse fact request SOAP message ");
  }
  if ( isFactFinderInDebugMode_.value_ ) cout << factRequestCollection;
  if ( isFactFinderInDebugMode_.value_ ) cout << "emu::base::FactFinder::parseFactRequestSOAP ***" << endl;
  return factRequestCollection;
}

xoap::MessageReference
emu::base::FactFinder::createRequestAcknowlegdementSOAP() const {
  xoap::MessageReference reply = xoap::createMessage();
  try {
    xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
    xoap::SOAPBody body = envelope.getBody();
    xoap::SOAPName requestAcknowledgedName = envelope.createName("factRequestResponse", "esd", ESD_NS_URI);
    xoap::SOAPElement bodyElement = body.addBodyElement( requestAcknowledgedName );
  }
  catch( xoap::exception::Exception& e ){
    XCEPT_RETHROW ( xoap::exception::Exception, "Failed to create fact request acknowledgement SOAP message: ", e ); 
  }
  catch( xcept::Exception& e ){
    XCEPT_RETHROW( xoap::exception::Exception, "Failed to create fact request acknowledgement SOAP message: ", e );
  }
  catch( std::exception& e ){
    stringstream ss; ss << "Failed to create fact request acknowledgement SOAP message: " << e.what();
    XCEPT_RAISE( xoap::exception::Exception, ss.str() );
  }
  catch(...){
    XCEPT_RAISE( xoap::exception::Exception, "Failed to create fact request acknowledgement SOAP message, unexpected exception." );
  }

  return reply;
}

xoap::MessageReference
emu::base::FactFinder::createFactsSOAP( const emu::base::FactCollection& factCollection ) const {
  if ( isFactFinderInDebugMode_.value_ ) cout << "*** emu::base::FactFinde::createFactsSOAP" << endl;

  xoap::MessageReference message = xoap::createMessage();
  try {
    xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
    xoap::SOAPName exSysOperationName = envelope.createName("input", "ws", ESWS_NS_URI);
    xoap::SOAPName factCollectionName = envelope.createName("factCollection", "", "");
    xoap::SOAPName sourceName         = envelope.createName("source", "esd", ESD_NS_URI);
    xoap::SOAPName requestIdName      = envelope.createName("requestId", "", "");
    xoap::SOAPName timeName           = envelope.createName("time", "esd", ESD_NS_URI);
    xoap::SOAPName componentIdName    = envelope.createName("component_id", "esd", ESD_NS_URI);
    xoap::SOAPName runName            = envelope.createName("run", "esd", ESD_NS_URI);
    xoap::SOAPName severityName       = envelope.createName("severity", "esd", ESD_NS_URI);
    xoap::SOAPName descriptionName    = envelope.createName("descr", "esd", ESD_NS_URI);
    xoap::SOAPName unitName           = envelope.createName("unit", "esd", ESD_NS_URI);

    xoap::SOAPBody body = envelope.getBody();
    xoap::SOAPElement exSysOperationElement = body.addBodyElement( exSysOperationName );
    xoap::SOAPElement collectionElement = exSysOperationElement.addChildElement( factCollectionName );
    // string logLevel = getApplicationContext()->getLogLevel();
    if ( isFactFinderInDebugMode_.value_ ){
      // Add the optional serviceInstructions element in order to use strict service with synchronous connection for debugging.
      xoap::SOAPName serverInstructionsName = envelope.createName("serviceInstructions", "service", ESIS_NS_URI);
      xoap::SOAPName asyncAttributeName   = envelope.createName("async"  , "", "");
      xoap::SOAPName strictAttributeName  = envelope.createName("strict" , "", "");
      xoap::SOAPName persistAttributeName = envelope.createName("persist", "", "");
      xoap::SOAPElement serverInstructionsElemenet = collectionElement.addChildElement( serverInstructionsName );
      serverInstructionsElemenet.addAttribute( asyncAttributeName  , "false" );
      serverInstructionsElemenet.addAttribute( strictAttributeName , "true"  );
      serverInstructionsElemenet.addAttribute( persistAttributeName, "false" );
    }
    xoap::SOAPElement sourceElement = collectionElement.addChildElement( sourceName );
    sourceElement.addTextNode( factCollection.getSource().c_str() );
    
    vector<emu::base::Fact>::iterator f;
    vector<emu::base::Fact> facts = factCollection.getFacts();
    for ( f = facts.begin(); f != facts.end(); f++ ){
      //cout << *f;

      xoap::SOAPName    factName    = envelope.createName( f->getName().c_str(), "esd", ESD_NS_URI);
      xoap::SOAPElement factElement = collectionElement.addChildElement( factName );
      
      factElement.addChildElement( timeName        ).addTextNode( f->getTime()          .c_str() );
      factElement.addChildElement( componentIdName ).addTextNode( f->getOneComponentId().c_str() );
      if ( f->getRun()        .size() ) factElement.addChildElement( runName         ).addTextNode( f->getRun()        .c_str() );
      if ( f->getSeverity()   .size() ) factElement.addChildElement( severityName    ).addTextNode( f->getSeverity()   .c_str() );
      if ( f->getDescription().size() ) factElement.addChildElement( descriptionName ).addTextNode( f->getDescription().c_str() );

      emu::base::Fact::Parameters_t::const_iterator p;
      for ( p = f->getParameters().begin(); p != f->getParameters().end(); ++p ){
	xoap::SOAPName    parameterName    = envelope.createName(p->first.c_str(), "esd", ESD_NS_URI);
	xoap::SOAPElement parameterElement = factElement.addChildElement( parameterName );
	if ( p->second.size() == 1 ){ // scalar parameter
	  parameterElement.addTextNode( p->second.at( 0 ).c_str() );
	}
	else if ( p->second.size() > 1 ){ // array parameter
	  xoap::SOAPName arrayElementName = envelope.createName("arrayElement", "esd", ESD_NS_URI);
	  for (vector<string>::const_iterator s = p->second.begin(); s != p->second.end(); ++s){
	    xoap::SOAPElement arrayElementElement = parameterElement.addChildElement( arrayElementName );
	    arrayElementElement.addTextNode( s->c_str() );
	  }
	}
      }

    }
    
  }
  catch( xoap::exception::Exception& e ){
    XCEPT_RETHROW ( xoap::exception::Exception, "Failed to create fact SOAP message: ", e ); 
  }
  catch( xcept::Exception& e ){
    XCEPT_RETHROW( xoap::exception::Exception, "Failed to create fact SOAP message: ", e );
  }
  catch( std::exception& e ){
    stringstream ss; ss << "Failed to create fact SOAP message: " << e.what();
    XCEPT_RAISE( xoap::exception::Exception, ss.str() );
  }
  catch(...){
    XCEPT_RAISE( xoap::exception::Exception, "Failed to create fact SOAP message, unexpected exception." );
  }

  if ( isFactFinderInDebugMode_.value_ ) { cout << "Sending SOAP:" << endl; message->writeTo( cout ); cout << endl; cout.flush(); }
  if ( isFactFinderInDebugMode_.value_ ) cout << "emu::base::FactFinde::createFactsSOAP ***" << endl;
  return message;
}

void
emu::base::FactFinder::sendFacts(){
  // Get the facts:
  emu::base::FactCollection facts( findFacts().setSource( source_ ) );
  // Queue them for sending:
  factFinderBSem_.take();
  factsToSend_.push_back( facts );
  if ( isFactFinderInDebugMode_.value_ ) cout << endl << endl << "factsToSend_.size() = " << factsToSend_.size() << endl << endl;
  // If queue is too long, drop the oldest facts:
  while ( factsToSend_.size() > maxQueueLength_ ){ 
    factsToSend_.pop_front();
    LOG4CPLUS_WARN( getApplicationLogger(), "Outgoing fact queue full. Discarding the oldest fact." );
  }
  factFinderBSem_.give();
  // Send them in another thread:
  if ( ! isDisabled_ && ! factWorkLoop_->isActive() ){
    factWorkLoop_->activate();
    factWorkLoop_->submit( sendFactsSignature_ );
  }
}

void
emu::base::FactFinder::sendFact( const string& componentId, const string& factType ){
  emu::base::Component component( componentId );
  sendFact( component, factType );
}

void
emu::base::FactFinder::sendFact( const emu::base::Component& component, const string& factType ){
  // Get the fact:
  emu::base::Fact fact( findFact( component, factType ) );
  // Bail out if untyped fact (which has no name) is returned, indicating failure of fact-finding:
  if ( fact.getName().size() == 0 ) return;
  // Set the component in case the user forgot to:
  fact.setComponent( component );
  // Wrap fact in a fact collection:
  emu::base::FactCollection facts;
  facts.setSource( source_ );
  facts.addFact( fact );
  // Queue them for sending:
  factFinderBSem_.take();
  factsToSend_.push_back( facts );
  if ( isFactFinderInDebugMode_.value_ ) cout << endl << endl << "factsToSend_.size() = " << factsToSend_.size() << endl << endl;
  // If queue is too long, drop the oldest facts:
  while ( factsToSend_.size() > maxQueueLength_ ){
    factsToSend_.pop_front();
    LOG4CPLUS_WARN( getApplicationLogger(), "Outgoing fact queue full. Discarding the oldest fact." );
  }
  factFinderBSem_.give();
  // Send them in another thread:
  if ( ! isDisabled_ && ! factWorkLoop_->isActive() ){
    factWorkLoop_->activate();
    factWorkLoop_->submit( sendFactsSignature_ );
  }
}

bool
emu::base::FactFinder::sendFactsInWorkLoop( toolbox::task::WorkLoop *wl ){
  //if ( isFactFinderInDebugMode_.value_ ) cout << "*** emu::base::FactFinder::sendFactsInWorkLoop" << endl << flush;
  bool isToBeResubmitted = false;

  factFinderBSem_.take();

  // Check whether expertSystemURL_ has already been assigned the value given in the configuration.
  if ( expertSystemURL_ == "NOT_YET_KNOWN" ){
    // Configuration has apparently not yet taken place. Come back later.
    factFinderBSem_.give();
    return true;
  }

  // Check format of expert system's URL, and disable sending if it's wrong.
  try{
    toolbox::net::URL u( expertSystemURL_ );
    isDisabled_ = false;
  }
  catch (toolbox::net::exception::MalformedURL& mu){
    XCEPT_DECLARE_NESTED( xcept::Exception, eObj, "Sending facts will be disabled: ", mu );
    LOG4CPLUS_WARN( getApplicationLogger(),  xcept::stdformat_exception_history(eObj) );
    this->notifyQualified( "warning", eObj );
    isDisabled_ = true;
  }
  
  // If disabled, return and do not resubmit.
  if ( isDisabled_ ){
    //if ( isFactFinderInDebugMode_.value_ ) cout << "emu::base::FactFinder::sendFactsInWorkLoop ***" << endl << flush;
    factFinderBSem_.give();
    return false;
  }

  bool isMoratoriumImposed = false;

  // First handle the requests (one at a time)
  if ( factRequestCollections_.size() ){
    if ( isFactFinderInDebugMode_.value_ ) cout << "   Handling request:" << endl << factRequestCollections_.front();
    emu::base::FactCollection factCollection;

    try{
      factCollection = collectFacts( factRequestCollections_.front() );
    } catch(xcept::Exception& e){
      stringstream ss;
      ss << "Failed to collect requested facts: " << xcept::stdformat_exception_history(e);
      LOG4CPLUS_WARN( getApplicationLogger(), ss.str() );
      XCEPT_DECLARE( xcept::Exception, eObj, ss.str() );
      this->notifyQualified( "warning", eObj );
    } catch( exception& e ){
      stringstream ss;
      ss << "Failed to collect requested facts: " << e.what();
      LOG4CPLUS_WARN( getApplicationLogger(), ss.str() );
      XCEPT_DECLARE( xcept::Exception, eObj, ss.str() );
      this->notifyQualified( "warning", eObj );
    } catch( ... ){
      stringstream ss;
      ss << "Failed to collect requested facts: Unexpected exception.";
      LOG4CPLUS_WARN( getApplicationLogger(), ss.str() );
      XCEPT_DECLARE( xcept::Exception, eObj, ss.str() );
      this->notifyQualified( "warning", eObj );
    }

    // Explicitly requested facts jump sending queue:
    if ( factCollection.size() > 0 ) factsToSend_.push_front( factCollection );
    factRequestCollections_.pop_front();
    isToBeResubmitted |= ( factRequestCollections_.size() > 0 );
  }

  // Send facts (one at a time).
  if ( factsToSend_.size() ){
    // Check if a SOAP timeout has occurred recently. If so, skip sending this time. The moratorium on sending is moratoriumAfterTimeout_ seconds long.
    if ( stopwatch_->read() > moratoriumAfterTimeout_ ){
      if ( isFactFinderInDebugMode_.value_ ) cout << "   Sending fact:" << endl << factsToSend_.front();

      try{
	bool isSent = sendFactsInSOAP( factsToSend_.front() );
	if ( isSent ) factsToSend_.pop_front(); // delete it only if it's been sent
      } catch(xcept::Exception& e){
	stringstream ss;
	ss << "Failed to send facts: " << xcept::stdformat_exception_history(e);
	LOG4CPLUS_WARN( getApplicationLogger(), ss.str() );
	XCEPT_DECLARE( xcept::Exception, eObj, ss.str() );
	this->notifyQualified( "warning", eObj );
      } catch( exception& e ){
	stringstream ss;
	ss << "Failed to send facts: " << e.what();
	LOG4CPLUS_WARN( getApplicationLogger(), ss.str() );
	XCEPT_DECLARE( xcept::Exception, eObj, ss.str() );
	this->notifyQualified( "warning", eObj );
      } catch( ... ){
	stringstream ss;
	ss << "Failed to send facts: Unexpected exception.";
	LOG4CPLUS_WARN( getApplicationLogger(), ss.str() );
	XCEPT_DECLARE( xcept::Exception, eObj, ss.str() );
	this->notifyQualified( "warning", eObj );
      }

    }
    else{
      isMoratoriumImposed = true;
    }
    isToBeResubmitted |= ( factsToSend_.size() > 0 );
  }

  factFinderBSem_.give();

  // If a moratorium is being imposed, wait a bit before resubmitting.
  if ( isMoratoriumImposed ){
    LOG4CPLUS_WARN( getApplicationLogger(), "A " << moratoriumAfterTimeout_ << "s moratorium was started on sending further facts following a SOAP timeout " << stopwatch_->read() << "s ago." );
    ::sleep( 10 );
  }

  //if ( isFactFinderInDebugMode_.value_ ) cout << "emu::base::FactFinder::sendFactsInWorkLoop ***" << endl;
  // return isToBeResubmitted;
  ::usleep( 5000 );
  return true;
}

emu::base::FactCollection
emu::base::FactFinder::collectFacts( const FactRequestCollection& requestCollection ) {
  emu::base::FactCollection fc;
  try{
    fc.setRequestId( requestCollection.getRequestId() ).setSource( source_ );
    vector<emu::base::FactRequest>::const_iterator fr;
    for ( fr = requestCollection.getRequests().begin(); fr != requestCollection.getRequests().end(); ++fr ){
      emu::base::Fact f = findFact( fr->getComponent(), fr->getFactType() );
      // Discard untyped fact (which has no name, indicating failure of fact-finding):
      if ( f.getName().size() > 0 ){
	// Set the component id in case the user forgot to:
	f.setComponent( fr->getComponent() );
	fc.addFact( f );
      }
    }
  } catch(xcept::Exception& e){
    stringstream ss;
    ss << "Failed to collect requested facts: " << xcept::stdformat_exception_history(e);
    LOG4CPLUS_WARN( getApplicationLogger(), ss.str() );
    XCEPT_DECLARE( xcept::Exception, eObj, ss.str() );
    this->notifyQualified( "warning", eObj );
  } catch( exception& e ){
    stringstream ss;
    ss << "Failed to collect requested facts: " << e.what();
    LOG4CPLUS_WARN( getApplicationLogger(), ss.str() );
    XCEPT_DECLARE( xcept::Exception, eObj, ss.str() );
    this->notifyQualified( "warning", eObj );
  } catch( ... ){
    stringstream ss;
    ss << "Failed to collect requested facts: Unexpected exception.";
    LOG4CPLUS_WARN( getApplicationLogger(), ss.str() );
    XCEPT_DECLARE( xcept::Exception, eObj, ss.str() );
    this->notifyQualified( "warning", eObj );
  }
  return fc;
}

bool
emu::base::FactFinder::sendFactsInSOAP( const emu::base::FactCollection& facts ){
  if ( isFactFinderInDebugMode_.value_ ) cout << "*** emu::base::FactFinder::sendFactsInSOAP" << endl;
  try{

    xoap::MessageReference msg = createFactsSOAP( facts );

    //xoap::MessageReference reply = getApplicationContext()->postSOAP( msg, *getApplicationDescriptor(), *targetDescriptor_ );
    xoap::MessageReference reply = postSOAP( msg, expertSystemURL_, "" );

    if ( isFactFinderInDebugMode_.value_ ) { cout << "SOAP reply received:" << endl; reply->writeTo( cout ); cout << endl; cout.flush(); }

    // Check if the reply indicates a fault occurred
    xoap::SOAPBody replyBody = reply->getSOAPPart().getEnvelope().getBody();
    if( replyBody.hasFault() ){
      xoap::SOAPFault soapFault = replyBody.getFault();
      stringstream ss;
      // We cannot use targetDescriptor until the context URL handling bug (lower-casing all) is fixed.
      // ss << "Received fault reply from " << targetDescriptor_->getClassName() << "." << targetDescriptor_->getInstance() << " : " << getSOAPFaultCode( soapFault );
      // ss << "Received fault reply from " << targetDescriptor_->getClassName() << "." << targetDescriptor_->getInstance() << " : " << getSOAPFaultCode( soapFault );
      ss << "Received fault reply from " << " expert system " << endl
	 << " Fault code: " << soapFault.getFaultCode() << endl
	 << " Fault string: " << soapFault.getFaultString() << endl;
//       if ( soapFault.hasDetail() ){
// 	// XceptSerializer probably works with XDAQ fault detail only...
// 	xcept::Exception fd;
// 	xdaq::XceptSerializer::importFrom( soapFault.getDetail().getDOM(), fd );
// 	XCEPT_RETHROW(xcept::Exception, ss.str(), fd);
//       }
//       else{
// 	XCEPT_RAISE(xcept::Exception, ss.str());
//       }
      XCEPT_RAISE(xcept::Exception, ss.str());
    }
    if ( isFactFinderInDebugMode_.value_ ) cout << "emu::base::FactFinder::sendFactsInSOAP ***" << endl << flush;
    return true; // sending successful
  } catch(xcept::Exception& e){
    stringstream ss;
    ss << "Failed to send fact SOAP message: " << xcept::stdformat_exception_history(e);
    LOG4CPLUS_WARN( getApplicationLogger(), ss.str() );
    XCEPT_DECLARE( xcept::Exception, eObj, ss.str() );
    this->notifyQualified( "warning", eObj );
  } catch( exception& e ){
    stringstream ss;
    ss << "Failed to send fact SOAP message: " << e.what();
    LOG4CPLUS_WARN( getApplicationLogger(), ss.str() );
    XCEPT_DECLARE( xcept::Exception, eObj, ss.str() );
    this->notifyQualified( "warning", eObj );
  } catch( ... ){
    stringstream ss;
    ss << "Failed to send fact SOAP message: Unexpected exception.";
    LOG4CPLUS_WARN( getApplicationLogger(), ss.str() );
    XCEPT_DECLARE( xcept::Exception, eObj, ss.str() );
    this->notifyQualified( "warning", eObj );
  }
  if ( isFactFinderInDebugMode_.value_ ) cout << "emu::base::FactFinder::sendFactsInSOAP ***" << endl << flush;
  return false; // failed to send
}

string emu::base::FactFinder::getSOAPFaultCode( xoap::SOAPFault soapFault ){
  // SOAP Fault v 1.2, see http://www.w3.org/TR/2003/REC-soap12-part1-20030624/#soapfault
  DOMNodeList* l = soapFault.getDOMNode()->getChildNodes();
  for (unsigned int i = 0; i < l->getLength(); i++){
    DOMNode* c = l->item(i);
    if (xoap::XMLCh2String(c->getLocalName()) == "Code" ){
      DOMNodeList* ll = c->getChildNodes();
      for (unsigned int j = 0; j < ll->getLength(); j++){
	DOMNode* cc = ll->item(j);
	if (xoap::XMLCh2String(cc->getLocalName()) == "Value" ){
	  xoap::SOAPNode n(cc);
	  return n.getValue();
	}
      }
    } 
  }
  return "";  
}

void
emu::base::FactFinder::findTargetDescriptor(){
  if ( isFactFinderInDebugMode_.value_ ) cout << "*** emu::base::FactFinder::findTargetDescriptor" << endl;
  string targetClass = "DataWarehouse";
  std::set<xdaq::ApplicationDescriptor *> descriptors = getApplicationContext()->getDefaultZone()->getApplicationDescriptors(targetClass); // This doesn't throw.
  if ( descriptors.size() == 0 ){
    stringstream ss;
    ss << "Failed to find application descriptor for " << targetClass;
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  else{
    targetDescriptor_ = *descriptors.begin();
  }
  if ( isFactFinderInDebugMode_.value_ ) cout << "emu::base::FactFinder::findTargetDescriptor ***" << endl;
}


xoap::MessageReference
emu::base::FactFinder::postSOAP( xoap::MessageReference message, 
				 const string& URL,
				 const string& SOAPAction )
// Adapted from xdaq::ApplicationContextImpl::postSOAP.
// This is necessary for sending SOAP to contexts not defined in this process's config file.
{
	
  bool setSOAPAction = false;
  if ( message->getMimeHeaders()->getHeader("SOAPAction").size() == 0 )
    {
      message->getMimeHeaders()->setHeader("SOAPAction", SOAPAction);		
      setSOAPAction = true;
    }
	      
  xoap::SOAPBody b = message->getSOAPPart().getEnvelope().getBody();
  DOMNode* node = b.getDOMNode();
	
  DOMNodeList* bodyList = node->getChildNodes();
  DOMNode* command = bodyList->item(0);
	
  if (command->getNodeType() == DOMNode::ELEMENT_NODE) 
    {                

      try{
	// Check format of URL
	toolbox::net::URL u( URL );
      }
      catch (toolbox::net::exception::MalformedURL& mu){
	  XCEPT_RETHROW (xcept::Exception, "Failed to post SOAP message", mu);
      }

      try
	{	
	  // Local dispatch: if remote and local address are on same host, get local messenger
			
	  // Get the address on the fly from the URL
	  pt::Address::Reference remoteAddress = pt::getPeerTransportAgent()
	    ->createAddress(URL,"soap");
				
	  pt::Address::Reference localAddress = 
	    pt::getPeerTransportAgent()->createAddress(getApplicationDescriptor()->getContextDescriptor()->getURL(),"soap");
			
	  // force here protocol http, service soap, because at this point we know over withc protocol/service to send.
	  // this allows specifying a host URL without the SOAP service qualifier
	  //		
	  std::string protocol = remoteAddress->getProtocol();
			
	  pt::PeerTransportSender* s = dynamic_cast<pt::PeerTransportSender*>(pt::getPeerTransportAgent()->getPeerTransport (protocol, "soap", pt::Sender));

	  // These two lines cannot be merges, since a reference that is a temporary object
	  // would delete the contained object pointer immediately after use.
	  //
	  pt::Messenger::Reference mr = s->getMessenger(remoteAddress, localAddress);
	  pt::SOAPMessenger& m = dynamic_cast<pt::SOAPMessenger&>(*mr);



	  // Fill the headers
	  std::multimap<std::string, std::string, std::less<std::string> >& allHeaders = message->getMimeHeaders()->getAllHeaders();
	  std::multimap<std::string, std::string, std::less<std::string> >::iterator i;
        
	  std::string headers = "";
	  for (i = allHeaders.begin(); i != allHeaders.end(); i++)
	    {
	      headers += (*i).first;
	      headers += ": ";
	      headers += (*i).second;
	      headers += "\r\n";
	    }
	  // cout << endl << "MIME headers" << endl << headers << endl;;



	  xoap::MessageReference rep = m.send(message);	 
			
	  if (setSOAPAction)
	    {
	      message->getMimeHeaders()->removeHeader("SOAPAction");
	    }
	  return rep;
	}
      catch (xdaq::exception::HostNotFound& hnf)
	{
	  XCEPT_RETHROW (xcept::Exception, "Failed to post SOAP message", hnf);
	} 
      catch (xdaq::exception::ApplicationDescriptorNotFound& acnf)
	{
	  XCEPT_RETHROW (xcept::Exception, "Failed to post SOAP message", acnf);
	}
      catch (pt::exception::Exception& pte)
	{
	  // We land here if any of the following occurs:
	  // * Connection times out.
	  // * Host cannot be resolved.
	  // * Connection is refused (e.g., because of wrong port number).
	  // * HTTP status code 404 (Not found) (e.g., in reply to http://emulab02.cern.ch:8080/cdw/wrongservice)
	  // Start the stopwatch to time a moratorium on sending.
	  stopwatch_->start();
	  XCEPT_RETHROW (xcept::Exception, "Failed to post SOAP message", pte);
	}
      catch(std::exception& se)
	{
	  XCEPT_RAISE (xcept::Exception, se.what());
	}
      catch(...)
	{
	  XCEPT_RAISE (xcept::Exception, "Failed to post SOAP message, unknown exception");
	}
    } 
  else
    {
      /*applicationDescriptorFactory_.unlock();
       */
      XCEPT_RAISE (xcept::Exception, "Bad SOAP message. Cannot find command tag");
    }

}

void
emu::base::FactFinder::timeExpired(toolbox::task::TimerEvent& e){
  if ( isFactFinderInDebugMode_.value_ ) { cout << "*** emu::base::FactFinder::timeExpired" << endl; cout.flush(); }
  sendFacts();
  if ( isFactFinderInDebugMode_.value_ ) { cout << "emu::base::FactFinder::timeExpired ***" << endl; cout.flush(); }
}
