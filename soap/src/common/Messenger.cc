#include "emu/soap/Messenger.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"

#include "xdata/soap/NamespaceURI.h"
#define XSD_NAMESPACE_URI "http://www.w3.org/2001/XMLSchema"
#define XDAQ_APP_URN_BASE "urn:xdaq-application:"

#include "xoap/DOMParser.h"
#include "xoap/DOMParserFactory.h"
#include "xoap/domutils.h"
#include "xdata/soap/Serializer.h"

#include <algorithm>

// For emu::soap::Messenger::postSOAP
#include "toolbox/net/URL.h"
#include "pt/PeerTransportAgent.h"
#include "pt/PeerTransportSender.h"
#include "pt/SOAPMessenger.h"

//TODO: post SOAP to URL, doxygenate

const emu::soap::NamedData emu::soap::Messenger::noAttributes = emu::soap::NamedData();
const emu::soap::NamedData emu::soap::Messenger::noParameters = emu::soap::NamedData();
const std::vector<emu::soap::Attachment> emu::soap::Messenger::noAttachments = std::vector<emu::soap::Attachment>();

emu::soap::Messenger::Messenger(){}

emu::soap::Messenger::Messenger( xdaq::Application *parent ) :
  application_( parent )
{}

xdaq::ApplicationDescriptor* emu::soap::Messenger::getAppDescriptor( const std::string &className, const unsigned int instance ){
  xdaq::ApplicationDescriptor* descriptor;

  try{
    descriptor = application_->getApplicationContext()->getDefaultZone()->getApplicationDescriptor( className, instance );
  }
  catch( xcept::Exception &e ){
    std::stringstream ss;
    ss << "Failed to get application descriptor for class " << className << " instance " << instance;
    XCEPT_RETHROW( xcept::Exception, ss.str(), e );
  }

  return descriptor;
}

void
emu::soap::Messenger::setParameters( xdaq::ApplicationDescriptor *target, const emu::soap::NamedData &parameters ){
  try{
    string targetAppNamespaceURI = string( XDAQ_APP_URN_BASE ) + target->getClassName();

    // Create SOAP message
    xoap::MessageReference message = xoap::createMessage();
    xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
    envelope.addNamespaceDeclaration( "xsi"    , XSI_NAMESPACE_URI     );
    envelope.addNamespaceDeclaration( "xsd"    , XSD_NAMESPACE_URI     );
    envelope.addNamespaceDeclaration( "soapenc", SOAPENC_NAMESPACE_URI );

    // Add command
    xoap::SOAPBody        body       = envelope.getBody();
    xoap::SOAPName        cmdName    = envelope.createName( "ParameterSet", "xdaq", XDAQ_NS_URI );
    xoap::SOAPBodyElement cmdElement = body.addBodyElement( cmdName );

    // Add 'properties' element
    xoap::SOAPName    propertiesName     = envelope.createName( "properties", "xapp", targetAppNamespaceURI );
    xoap::SOAPElement propertiesElement  = cmdElement.addChildElement( propertiesName );
    xoap::SOAPName    propertiesTypeName = envelope.createName( "type", "xsi", XSI_NAMESPACE_URI );
    propertiesElement.addAttribute( propertiesTypeName, "soapenc:Struct" );

    // Add parameters as child elements to 'properties'
    includeParameters( message, &propertiesElement, parameters );

    // Send message
    string s;
    message->writeTo( s );
    LOG4CPLUS_DEBUG( application_->getApplicationLogger(), "Sent:" << endl << "<![CDATA[" << endl << s << endl << "]]>" );
    xoap::MessageReference reply = application_->getApplicationContext()->postSOAP( message, *application_->getApplicationDescriptor(), *target );
    reply->writeTo( s );
    LOG4CPLUS_DEBUG( application_->getApplicationLogger(), "Received:" << endl << "<![CDATA[" << endl << s << endl << "]]>" );
    
    // Check reply for fault
    xoap::SOAPBody replyBody = reply->getSOAPPart().getEnvelope().getBody();
    if ( replyBody.hasFault() ){
      xoap::SOAPFault fault = replyBody.getFault();
      //cout << endl << faultToPlainText( &fault ) << endl;
      throw faultToException( &fault );
    }

  }
  catch( xcept::Exception &e ){
    std::stringstream ss;
    ss << "Failed to set parameter" << ( parameters.size()==1 ? " " : "s " ) << parameters
       << " in application " << target->getClassName()
       << " of instance " << target->getInstance();
    XCEPT_RETHROW( xcept::Exception, ss.str(), e );
  }
  catch( std::exception &e ){
    std::stringstream ss;
    ss << "Failed to set parameter" << ( parameters.size()==1 ? " " : "s " ) << parameters
       << " in application " << target->getClassName()
       << " of instance " << target->getInstance()
       << ": " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...){
    std::stringstream ss;
    ss << "Failed to set parameter" << ( parameters.size()==1 ? " " : "s " ) << parameters
       << " in application " << target->getClassName()
       << " of instance " << target->getInstance()
       << ": Unknown exception.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }

}

void
emu::soap::Messenger::setParameters( const string &className, const unsigned int instance, emu::soap::NamedData &parameters ){
  xdaq::ApplicationDescriptor* target = getAppDescriptor( className, instance );
  setParameters( target, parameters );
}

void
emu::soap::Messenger::setParameters( const string &className, emu::soap::NamedData &parameters ){
  std::set<xdaq::ApplicationDescriptor *> apps = application_->getApplicationContext()->getDefaultZone()->getApplicationDescriptors( className );

  if ( apps.size() == 0 ){
    LOG4CPLUS_WARN( application_->getApplicationLogger(), "Setting parameters " << parameters <<  " to " + className + " aborted: No descriptors found.");
    return;
  }

  for ( std::set<xdaq::ApplicationDescriptor *>::iterator app = apps.begin(); app != apps.end(); ++app ) {
    setParameters( *app, parameters );
  }
}

xoap::MessageReference
emu::soap::Messenger::sendCommand( xdaq::ApplicationDescriptor *target,
				   const std::string &command,
				   const emu::soap::NamedData &parameters,
				   const emu::soap::NamedData &attributes,
				   const vector<emu::soap::Attachment> &attachments ){
  xoap::MessageReference message;
  xoap::MessageReference reply;

  try{

    // Create SOAP message
    xoap::MessageReference message = xoap::createMessage();
    xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
    envelope.addNamespaceDeclaration( "xsi"    , XSI_NAMESPACE_URI     );
    envelope.addNamespaceDeclaration( "xsd"    , XSD_NAMESPACE_URI     );
    envelope.addNamespaceDeclaration( "soapenc", SOAPENC_NAMESPACE_URI );

    // Add command
    xoap::SOAPBody        body       = envelope.getBody();
    xoap::SOAPName        cmdName    = envelope.createName( command, "xdaq", XDAQ_NS_URI );
    xoap::SOAPBodyElement cmdElement = body.addBodyElement( cmdName );

    // Add attributes (if any) to command
    addAttributes( message, &cmdElement, attributes );

    // Add parameters (if any) as child elements to command element
    includeParameters( message, &cmdElement, parameters );

    // Add attachments
    addAttachments( message, attachments );

    // Send message
    string s;
    message->writeTo( s );
    LOG4CPLUS_DEBUG( application_->getApplicationLogger(), "Sent:" << endl << "<![CDATA[" << endl << s << endl << "]]>" );
    reply = application_->getApplicationContext()->postSOAP( message, *application_->getApplicationDescriptor(), *target );
    reply->writeTo( s );
    LOG4CPLUS_DEBUG( application_->getApplicationLogger(), "Received:" << endl << "<![CDATA[" << endl << s << endl << "]]>" );
    
    // Check reply for fault
    xoap::SOAPBody replyBody = reply->getSOAPPart().getEnvelope().getBody();
    if ( replyBody.hasFault() ){
      xoap::SOAPFault fault = replyBody.getFault();
      //cout << endl << faultToPlainText( &fault ) << endl;
      throw faultToException( &fault );
    }

  }
  catch( xcept::Exception &e ){
    std::stringstream ss;
    ss << "Failed to have SOAP command '" << command
       << "' executed in application " << target->getClassName()
       << " of instance " << target->getInstance();
    XCEPT_RETHROW( xcept::Exception, ss.str(), e );
  }
  catch( std::exception &e ){
    std::stringstream ss;
    ss << "Failed to have SOAP command '" << command
       << "' executed in application " << target->getClassName()
       << " of instance " << target->getInstance()
       << ": " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...){
    std::stringstream ss;
    ss << "Failed to have SOAP command '" << command
       << "' executed in application " << target->getClassName()
       << " of instance " << target->getInstance()
       << ": Unknown exception.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }

  return reply;
}

xoap::MessageReference
emu::soap::Messenger::sendCommand( const string &className, 
				   const unsigned int instance,
				   const std::string &command, 
				   const emu::soap::NamedData &parameters,
				   const emu::soap::NamedData &attributes,
				   const vector<emu::soap::Attachment> &attachments ){
  xdaq::ApplicationDescriptor* target = getAppDescriptor( className, instance );
  return sendCommand( target, command, parameters, attributes, attachments );
}

void
emu::soap::Messenger::sendCommand( const string &className, 
				   const std::string &command, 
				   const emu::soap::NamedData &parameters,
				   const emu::soap::NamedData &attributes,
				   const vector<emu::soap::Attachment> &attachments ){
  std::set<xdaq::ApplicationDescriptor *> apps = application_->getApplicationContext()->getDefaultZone()->getApplicationDescriptors( className );

  if ( apps.size() == 0 ){
    LOG4CPLUS_WARN( application_->getApplicationLogger(), "Sending command " << command <<  " to " + className + " aborted: No descriptors found.");
    return;
  }

  for ( std::set<xdaq::ApplicationDescriptor *>::iterator app = apps.begin(); app != apps.end(); ++app ) {
    sendCommand( *app, command, parameters, attributes, attachments );
  }
}

void
emu::soap::Messenger::getParameters( xdaq::ApplicationDescriptor *target, emu::soap::NamedData &parameters ){
  try{
    string targetAppNamespaceURI = string( XDAQ_APP_URN_BASE ) + target->getClassName();

    // Create SOAP message
    xoap::MessageReference message = xoap::createMessage();
    xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
    envelope.addNamespaceDeclaration( "xsi"    , XSI_NAMESPACE_URI     );
    envelope.addNamespaceDeclaration( "xsd"    , XSD_NAMESPACE_URI     );
    envelope.addNamespaceDeclaration( "soapenc", SOAPENC_NAMESPACE_URI );

    // Add command
    xoap::SOAPBody        body       = envelope.getBody();
    xoap::SOAPName        cmdName    = envelope.createName( "ParameterGet", "xdaq", XDAQ_NS_URI );
    xoap::SOAPBodyElement cmdElement = body.addBodyElement( cmdName );

    // Add 'properties' element
    xoap::SOAPName    propertiesName     = envelope.createName( "properties", "xapp", targetAppNamespaceURI );
    xoap::SOAPElement propertiesElement  = cmdElement.addChildElement( propertiesName );
    xoap::SOAPName    propertiesTypeName = envelope.createName( "type", "xsi", XSI_NAMESPACE_URI );
    propertiesElement.addAttribute( propertiesTypeName, "soapenc:Struct" );

    // Add parameters as child elements to 'properties'
    includeParameters( message, &propertiesElement, parameters );

    // Send message
    string s;
    message->writeTo( s );
    LOG4CPLUS_DEBUG( application_->getApplicationLogger(), "Sent:" << endl << "<![CDATA[" << endl << s << endl << "]]>" );
    xoap::MessageReference reply = application_->getApplicationContext()->postSOAP( message, *application_->getApplicationDescriptor(), *target );
    reply->writeTo( s );
    LOG4CPLUS_DEBUG( application_->getApplicationLogger(), "Received:" << endl << "<![CDATA[" << endl << s << endl << "]]>" );
    
    // Check reply for fault
    xoap::SOAPBody replyBody = reply->getSOAPPart().getEnvelope().getBody();
    if ( replyBody.hasFault() ){
      xoap::SOAPFault fault = replyBody.getFault();
      //cout << endl << faultToPlainText( &fault ) << endl;
      throw faultToException( &fault );
    }

    // Parse reply to deserialize parameters
    extractParameters( reply, parameters, string( XDAQ_APP_URN_BASE ) + target->getClassName() );

  }
  catch( xcept::Exception &e ){
    std::stringstream ss;
    ss << "Failed to get parameter" << ( parameters.size()==1 ? " " : "s " ) << parameters
       << " from application " << target->getClassName()
       << " of instance " << target->getInstance();
    XCEPT_RETHROW( xcept::Exception, ss.str(), e );
  }
  catch( std::exception &e ){
    std::stringstream ss;
    ss << "Failed to get parameter" << ( parameters.size()==1 ? " " : "s " ) << parameters
       << " from application " << target->getClassName()
       << " of instance " << target->getInstance()
       << ": " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...){
    std::stringstream ss;
    ss << "Failed to get parameter" << ( parameters.size()==1 ? " " : "s " ) << parameters
       << " from application " << target->getClassName()
       << " of instance " << target->getInstance()
       << ": Unknown exception.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }

}

void
emu::soap::Messenger::getParameters( const string &className, const unsigned int instance, emu::soap::NamedData &parameters ){
  xdaq::ApplicationDescriptor* target = getAppDescriptor( className, instance );
  getParameters( target, parameters );
}

void emu::soap::Messenger::addAttachments( xoap::MessageReference message, const vector<emu::soap::Attachment> &attachments ){
  // Add attachments to message
  for ( vector<emu::soap::Attachment>::const_iterator a=attachments.begin(); a!=attachments.end(); ++a ){
    xoap::AttachmentPart* attachmentPart = message->createAttachmentPart( a->getContent(), a->getContentLength(), a->getContentType() );
    attachmentPart->setContentEncoding( a->getContentEncoding() );
    attachmentPart->setContentId( a->getContentId() );
    attachmentPart->setContentLocation( a->getContentLocation() );
    message->addAttachmentPart( attachmentPart );
  }
}

void emu::soap::Messenger::addAttributes( xoap::MessageReference message, xoap::SOAPElement* element, const emu::soap::NamedData &attributes ){
  // Add attributes to element
  xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
  for ( emu::soap::NamedData::const_iterator a=attributes.begin(); a!=attributes.end(); ++a ){
    xoap::SOAPName attrName = envelope.createName( a->first, "", "" );
    element->addAttribute( attrName, a->second->toString() );
  }
}

void emu::soap::Messenger::includeParameters( xoap::MessageReference message, xoap::SOAPElement* parent, const emu::soap::NamedData &parameters ){
  // Include all requested parameters in parent element
  xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
  string parentNamespaceURI    = xoap::XMLCh2String( parent->getDOMNode()->getNamespaceURI() );
  string parentNamespacePrefix = xoap::XMLCh2String( parent->getDOMNode()->getPrefix()       );
  xdata::soap::Serializer serializer;
  emu::soap::NamedData::const_iterator p;
  for ( p=parameters.begin(); p!=parameters.end(); ++p ){
    xoap::SOAPName    propertyName    = envelope.createName( p->first, parentNamespacePrefix, parentNamespaceURI );
    xoap::SOAPElement propertyElement = parent->addChildElement( propertyName );
    serializer.exportAll( p->second, dynamic_cast<DOMElement*>( propertyElement.getDOMNode()), false );
  }
}

void
emu::soap::Messenger::extractParameters( xoap::MessageReference reply, emu::soap::NamedData &parameters, const string &parametersNamespaceURI ){
  // Parse reply and deserialize the requested parameters
  xoap::DOMParser* parser = xoap::getDOMParserFactory()->get("ParseFromSOAP");
  xdata::soap::Serializer serializer;

  std::stringstream ss;
  reply->writeTo( ss );
  DOMDocument* doc = parser->parse( ss.str() );
  for ( emu::soap::NamedData::iterator p=parameters.begin(); p!=parameters.end(); ++p ){
    DOMNode* n = doc->getElementsByTagNameNS( xoap::XStr( ( parametersNamespaceURI.size()>0 ? parametersNamespaceURI.c_str(): "*" ) ), 
					      xoap::XStr( p->first.c_str() ) 
					      )->item(0);
    if ( n != NULL ){
      serializer.import( p->second, n );
    }
    else{
      std::stringstream ss;
      ss << "Failed to extract parameter '" << p->first << "' from ParameterGetResponse SOAP message ";
      XCEPT_RAISE( xcept::Exception, ss.str() );
    }
  }

  // We're responsible for releasing the memory allocated to DOMDocument
  doc->release();
  // Parser must be explicitly removed, or else it stays in the memory
  xoap::getDOMParserFactory()->destroy("ParseFromSOAP");
}

std::string 
emu::soap::Messenger::faultToPlainText( xoap::SOAPFault* fault ){
  std::stringstream ss;
  ss << std::endl 
     << "code:      " << fault->getFaultCode() << std::endl
     << "summary:   " << fault->getFaultString() << std::endl;
  if ( fault->hasDetail() ){
    xoap::SOAPElement detail = fault->getDetail();
    ss << faultElementToPlainText( &detail, 0 );
  }
  return ss.str();
}

std::string 
emu::soap::Messenger::faultElementToPlainText( xoap::SOAPElement* elem, const int indentDepth ){
  const int indentIncrement = 8;
  std::stringstream ss;
  ss << std::string( indentDepth, ' ' ) << elem->getElementName().getLocalName() 
     << ":" << std::string( std::max(int(1),12-int(elem->getElementName().getLocalName().size())), ' ' );
  std::string value = elem->getValue();
  if ( elem->getDOMNode()->hasChildNodes() ){
    if ( elem->getDOMNode()->getFirstChild()->getNodeType() == DOMNode::TEXT_NODE ){
      if ( xoap::XMLCh2String( elem->getDOMNode()->getFirstChild()->getNodeValue() ).size() > 0 ) ss << value;
      ss << std::endl;
    }
    else{
      ss << std::endl;
      std::vector<xoap::SOAPElement> children = elem->getChildElements();
      for ( std::vector<xoap::SOAPElement>::iterator c=children.begin(); c!=children.end(); ++c ){
	ss << faultElementToPlainText( &*c, indentDepth+indentIncrement );
      }
    }
  }
  else{
    ss << std::endl;
  }
  return ss.str();
}

xcept::Exception
emu::soap::Messenger::faultToException( xoap::SOAPFault* fault ){
  // First create a stack of exception history out of the nested errors in the SOAP reply
  std::vector<xcept::ExceptionInformation> history;
  history.push_back( xcept::ExceptionInformation() );
  std::vector<xcept::ExceptionInformation>::iterator e = history.begin();
  e->setProperty( "faultcode" , fault->getFaultCode() );
  e->setProperty( "identifier", "Fault SOAP reply" );
  e->setProperty( "module"    , string("SOAP fault code: ")+fault->getFaultCode() );
  e->setProperty( "message"   , fault->getFaultString() );
  if ( fault->hasDetail() ){
    if ( fault->getDetail().getChildElements().size() > 0 ) faultElementToException( &fault->getDetail().getChildElements()[0], history, 0 );
  }

  // Print history for debugging
  //   int level = 0;
  //   for ( e=history.begin(); e!=history.end(); ++e ){
  //     cout << "### Stack level " << level++ << "###  ";
  //     std::map<std::string, std::string> properties = e->getProperties();
  //     for ( std::map<std::string, std::string>::iterator p=properties.begin(); p!=properties.end(); ++p ) cout << p->first << ":" << p->second << "  ";
  //     cout << endl;
  //   }

  // Reverse history
  std::vector<xcept::ExceptionInformation> reversedHistory;
  for ( std::vector<xcept::ExceptionInformation>::reverse_iterator rh = history.rbegin(); rh != history.rend(); ++rh ){
    reversedHistory.push_back( *rh );
  }
  // Create exception and attach reversed history to it
  xcept::Exception nested;
  nested.getHistory() = reversedHistory;
  return nested;
}

void
emu::soap::Messenger::faultElementToException( xoap::SOAPElement* elem, 
					  std::vector<xcept::ExceptionInformation> &history, 
					  int level ){
  std::string name  = elem->getElementName().getLocalName();
  std::string value = elem->getValue();
  if ( elem->getDOMNode()->hasChildNodes() ){
    if ( elem->getDOMNode()->getFirstChild()->getNodeType() == DOMNode::TEXT_NODE ){
      if ( xoap::XMLCh2String( elem->getDOMNode()->getFirstChild()->getNodeValue() ).size() > 0 ){
	history[level].setProperty( name, value );
      }
    }
    else{
      std::vector<xoap::SOAPElement> children = elem->getChildElements();
      // Add to stack a new exception
      history.push_back( xcept::ExceptionInformation() );
      for ( std::vector<xoap::SOAPElement>::iterator c=children.begin(); c!=children.end(); ++c ){
	faultElementToException( &*c, history, level+1 );
      }
    }
  }
}

xoap::MessageReference
emu::soap::Messenger::postSOAP( xoap::MessageReference message, 
				const string& URL,
				const string& SOAPAction )
// Adapted from xdaq::ApplicationContextImpl::postSOAP.
// This is necessary for sending SOAP to contexts not defined in this process's config file.
{
	
  bool setSOAPAction = false;
  if ( message->getMimeHeaders()->getHeader("SOAPAction").size()       == 0 &&
       message->getMimeHeaders()->getHeader("Content-Location").size() == 0    )
    {
      // SOAP 1.1
      message->getMimeHeaders()->setHeader("SOAPAction", SOAPAction);
      // SOAP 1.2
      message->getMimeHeaders()->setHeader("Content-Location", SOAPAction);
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
	    pt::getPeerTransportAgent()->createAddress(application_->getApplicationDescriptor()->getContextDescriptor()->getURL(),"soap");
			
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

	  xoap::MessageReference rep = m.send(message);	 
			
	  if (setSOAPAction)
	    {
	      message->getMimeHeaders()->removeHeader("SOAPAction");
	      message->getMimeHeaders()->removeHeader("Content-Location");
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
