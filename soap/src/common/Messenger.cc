#include "emu/soap/ToolBox.h"
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

#include "xdaq/XceptSerializer.h"

#include <algorithm>

// For emu::soap::Messenger::postSOAP
#include "toolbox/net/URL.h"
#include "pt/PeerTransportAgent.h"
#include "pt/PeerTransportSender.h"
#include "pt/SOAPMessenger.h"


emu::soap::Messenger::Messenger(){}

emu::soap::Messenger::Messenger( xdaq::Application *parent ) :
  application_( parent ),
  httpResponseTimeoutSec_( 0 )
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
emu::soap::Messenger::setParameters( xdaq::ApplicationDescriptor *target, const emu::soap::Parameters &parameters ){
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

    // Set the response timeout if different from the default value.
    if ( httpResponseTimeoutSec_ > 0 ) emu::soap::setResponseTimeout( message, httpResponseTimeoutSec_ );

    // Send message
    LOG4CPLUS_DEBUG( application_->getApplicationLogger(), "Sending:" << endl << "<![CDATA[" << endl << toStringWithoutAttachments( message ) << endl << "]]>" );
    xoap::MessageReference reply = application_->getApplicationContext()->postSOAP( message, *application_->getApplicationDescriptor(), *target );
    LOG4CPLUS_DEBUG( application_->getApplicationLogger(), "Received:" << endl << "<![CDATA[" << endl << toStringWithoutAttachments( reply ) << endl << "]]>" );
    
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
emu::soap::Messenger::setParameters( const string &className, const unsigned int instance, const emu::soap::Parameters &parameters ){
  xdaq::ApplicationDescriptor* target = getAppDescriptor( className, instance );
  setParameters( target, parameters );
}

void
emu::soap::Messenger::setParameters( const string &className, const emu::soap::Parameters &parameters ){
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
				   const emu::soap::QualifiedName &command,
				   const emu::soap::Parameters &parameters,
				   const emu::soap::Attributes &attributes,
				   const vector<emu::soap::Attachment> &attachments ){
  xoap::MessageReference message;
  xoap::MessageReference reply;

  try{

    // Create SOAP message
    xoap::MessageReference message = createMessage( command, parameters, attributes, attachments );

    // Set the response timeout if different from the default value.
    if ( httpResponseTimeoutSec_ > 0 ) emu::soap::setResponseTimeout( message, httpResponseTimeoutSec_ );

    // Send message
    LOG4CPLUS_DEBUG( application_->getApplicationLogger(), "Sending:" << endl << "<![CDATA[" << endl << toStringWithoutAttachments( message ) << endl << "]]>" );
    reply = application_->getApplicationContext()->postSOAP( message, *application_->getApplicationDescriptor(), *target );
    LOG4CPLUS_DEBUG( application_->getApplicationLogger(), "Received:" << endl << "<![CDATA[" << endl << toStringWithoutAttachments( reply ) << endl << "]]>" );
    
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
				   const emu::soap::QualifiedName &command, 
				   const emu::soap::Parameters &parameters,
				   const emu::soap::Attributes &attributes,
				   const vector<emu::soap::Attachment> &attachments ){
  xdaq::ApplicationDescriptor* target = getAppDescriptor( className, instance );
  return sendCommand( target, command, parameters, attributes, attachments );
}

void
emu::soap::Messenger::sendCommand( const string &className, 
				   const emu::soap::QualifiedName &command, 
				   const emu::soap::Parameters &parameters,
				   const emu::soap::Attributes &attributes,
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
emu::soap::Messenger::getParameters( xdaq::ApplicationDescriptor *target, emu::soap::Parameters &parameters ){
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

    // Set the response timeout if different from the default value.
    if ( httpResponseTimeoutSec_ > 0 ) emu::soap::setResponseTimeout( message, httpResponseTimeoutSec_ );

    // Send message
    LOG4CPLUS_DEBUG( application_->getApplicationLogger(), "Sending:" << endl << "<![CDATA[" << endl << toStringWithoutAttachments( message ) << endl << "]]>" );
    xoap::MessageReference reply = application_->getApplicationContext()->postSOAP( message, *application_->getApplicationDescriptor(), *target );
    LOG4CPLUS_DEBUG( application_->getApplicationLogger(), "Received:" << endl << "<![CDATA[" << endl << toStringWithoutAttachments( reply ) << endl << "]]>" );
    
    // Check reply for fault
    xoap::SOAPBody replyBody = reply->getSOAPPart().getEnvelope().getBody();
    if ( replyBody.hasFault() ){
      xoap::SOAPFault fault = replyBody.getFault();
      //cout << endl << faultToPlainText( &fault ) << endl;
      throw faultToException( &fault );
    }

    // Set all parameters' namespace URI. (They will be looked for in this namespace when extracted from the reply.)
    for ( emu::soap::Parameters::iterator p=parameters.begin(); p!=parameters.end(); ++p ){
      p->first.setNamespaceURI( targetAppNamespaceURI );
    }
    // Parse reply to deserialize parameters
    extractParameters( reply, parameters );

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
emu::soap::Messenger::getParameters( const string &className, const unsigned int instance, emu::soap::Parameters &parameters ){
  xdaq::ApplicationDescriptor* target = getAppDescriptor( className, instance );
  getParameters( target, parameters );
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
