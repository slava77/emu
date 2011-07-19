#include "emu/soap/ToolBox.h"
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

xoap::MessageReference 
emu::soap::createMessage( const emu::soap::QualifiedName &command, 
			  const emu::soap::Parameters &parameters,
			  const emu::soap::Attributes &attributes,
			  const vector<emu::soap::Attachment> &attachments ){

  try{
    // Create SOAP message
    xoap::MessageReference message = xoap::createMessage();
    xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
    envelope.addNamespaceDeclaration( "xsi"    , XSI_NAMESPACE_URI     );
    envelope.addNamespaceDeclaration( "xsd"    , XSD_NAMESPACE_URI     );
    envelope.addNamespaceDeclaration( "soapenc", SOAPENC_NAMESPACE_URI );

    // Add command
    xoap::SOAPBody body    = envelope.getBody();
     // Command's prefix _must_ be "xdaq" for LTC so let it default to that... :-(
    xoap::SOAPName cmdName = 
      envelope.createName( command.getName().c_str(), 
			   ( command.getPrefix().size() ? command.getPrefix().c_str() : "xdaq" ),
			   ( command.getNamespaceURI().size() ? command.getNamespaceURI().c_str() : XDAQ_NS_URI ) );
    xoap::SOAPBodyElement cmdElement = body.addBodyElement( cmdName );

    // Add attributes (if any) to command
    emu::soap::addAttributes( message, &cmdElement, attributes );

    // Add parameters (if any) as child elements to command element
    emu::soap::includeParameters( message, &cmdElement, parameters );

    // Add attachments
    emu::soap::addAttachments( message, attachments );

    return message;
  }
  catch( xcept::Exception &e ){
    std::stringstream ss;
    ss << "Failed to create SOAP message for '" << command << "' : ";
    XCEPT_RETHROW( xcept::Exception, ss.str(), e );
  }
  catch( std::exception &e ){
    std::stringstream ss;
    ss << "Failed to create SOAP message for '" << command << "' : "<< e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...){
    std::stringstream ss;
    ss << "Failed to create SOAP message for '" << command << "': Unknown exception.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }

}


void 
emu::soap::addAttachments( xoap::MessageReference message, const vector<emu::soap::Attachment> &attachments ){
  // Add attachments to message
  for ( vector<emu::soap::Attachment>::const_iterator a=attachments.begin(); a!=attachments.end(); ++a ){
    xoap::AttachmentPart* attachmentPart = message->createAttachmentPart( a->getContent(), a->getContentLength(), a->getContentType() );
    attachmentPart->setContentEncoding( a->getContentEncoding() );
    attachmentPart->setContentId( a->getContentId() );
    attachmentPart->setContentLocation( a->getContentLocation() );
    message->addAttachmentPart( attachmentPart );
  }
}

void 
emu::soap::addAttributes( xoap::MessageReference message, xoap::SOAPElement* element, const emu::soap::Attributes &attributes ){
  // Add attributes to element
  string elementNamespaceURI;
  string elementNamespacePrefix;
  if ( attributes.getUsePrefixOfParent() ){
    elementNamespaceURI    = xoap::XMLCh2String( element->getDOMNode()->getNamespaceURI() );
    elementNamespacePrefix = xoap::XMLCh2String( element->getDOMNode()->getPrefix()       );
  }
  xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
  for ( emu::soap::Attributes::const_iterator a=attributes.begin(); a!=attributes.end(); ++a ){
    if ( a->first.getNamespaceURI().size() > 0 ){
      element->addNamespaceDeclaration( a->first.getPrefix(), a->first.getNamespaceURI() );
      xoap::SOAPName attrName = envelope.createName( a->first.getName(), a->first.getPrefix(), a->first.getNamespaceURI() );
      element->addAttribute( attrName, a->second->toString() );
    }
    else{
      xoap::SOAPName attrName = envelope.createName( a->first.getName(), elementNamespacePrefix, elementNamespaceURI );
      element->addAttribute( attrName, a->second->toString() );
    }
  }
}

void 
emu::soap::includeParameters( xoap::MessageReference message, xoap::SOAPElement* parent, emu::soap::Parameters &parameters ){
  try{

    // Include all requested parameters in parent element
    string parentNamespaceURI;
    string parentNamespacePrefix;
    if ( parameters.getUsePrefixOfParent() ){
      parentNamespaceURI    = xoap::XMLCh2String( parent->getDOMNode()->getNamespaceURI() );
      parentNamespacePrefix = xoap::XMLCh2String( parent->getDOMNode()->getPrefix()       );
    }
    xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
    xdata::soap::Serializer serializer;
    emu::soap::Parameters::const_iterator p;
    for ( p=parameters.begin(); p!=parameters.end(); ++p ){
      xoap::SOAPName propertyName = ( p->first.getNamespaceURI().size() > 0
				      ?
				      // Explixit namespace specified.
				      envelope.createName( p->first.getName(),
							   p->first.getPrefix(),
							   p->first.getNamespaceURI() )
				      :
				      // No namespace specified, use default namespace.
				      envelope.createName( p->first.getName(), 
							   parentNamespacePrefix, 
							   parentNamespaceURI )
				      );
      xoap::SOAPElement propertyElement = parent->addChildElement( propertyName );
      serializer.exportAll( p->second.first, dynamic_cast<DOMElement*>( propertyElement.getDOMNode() ), false );
      if ( p->second.second != NULL ){
	emu::soap::addAttributes( message, &propertyElement, *p->second.second );
      }
    }

  }
  catch( xcept::Exception &e ){
    std::stringstream ss;
    ss << "Failed to include parameters " << parameters
    << " in message ";
    XCEPT_RETHROW( xcept::Exception, ss.str(), e );
  }
  catch( std::exception &e ){
    std::stringstream ss;
    ss << "Failed to include parameters " << parameters
       << " in message : " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...){
    std::stringstream ss;
    ss << "Failed to include parameters " << parameters
       << " in message : Unknown exception.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
}

void 
emu::soap::includeParameters( xoap::MessageReference message, xoap::SOAPElement* parent, const emu::soap::Parameters &parameters ){
  try{

    // Include all requested parameters in parent element
    string parentNamespaceURI;
    string parentNamespacePrefix;
    if ( parameters.getUsePrefixOfParent() ){
      parentNamespaceURI    = xoap::XMLCh2String( parent->getDOMNode()->getNamespaceURI() );
      parentNamespacePrefix = xoap::XMLCh2String( parent->getDOMNode()->getPrefix()       );
    }
    xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
    xdata::soap::Serializer serializer;
    emu::soap::Parameters::const_iterator p;
    for ( p=parameters.begin(); p!=parameters.end(); ++p ){
      xoap::SOAPName propertyName = ( p->first.getNamespaceURI().size() > 0
				      ?
				      // Explixit namespace specified.
				      envelope.createName( p->first.getName(),
							   p->first.getPrefix(),
							   p->first.getNamespaceURI() )
				      :
				      // No namespace specified, use default namespace.
				      envelope.createName( p->first.getName(), 
							   parentNamespacePrefix, 
							   parentNamespaceURI )
				      );
      xoap::SOAPElement propertyElement = parent->addChildElement( propertyName );
      serializer.exportAll( p->second.first, dynamic_cast<DOMElement*>( propertyElement.getDOMNode() ), false );
      if ( p->second.second != NULL ){
	emu::soap::addAttributes( message, &propertyElement, *p->second.second );
      }
    }

  }
  catch( xcept::Exception &e ){
    std::stringstream ss;
    ss << "Failed to include parameters " << parameters
    << " in message ";
    XCEPT_RETHROW( xcept::Exception, ss.str(), e );
  }
  catch( std::exception &e ){
    std::stringstream ss;
    ss << "Failed to include parameters " << parameters
       << " in message : " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...){
    std::stringstream ss;
    ss << "Failed to include parameters " << parameters
       << " in message : Unknown exception.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
}

void
emu::soap::extractParameters( xoap::MessageReference reply, emu::soap::Parameters &parameters, const string &parametersNamespaceURI ){
  try{

    // Parse reply and deserialize the requested parameters
    xoap::DOMParser* parser = xoap::getDOMParserFactory()->get("ParseFromSOAP");
    xdata::soap::Serializer serializer;

    string s;
    reply->writeTo( s );
    DOMDocument* doc = parser->parse( s );
    for ( emu::soap::Parameters::iterator p=parameters.begin(); p!=parameters.end(); ++p ){
      DOMNode* n = doc->getElementsByTagNameNS( xoap::XStr( ( parametersNamespaceURI.size()>0 ? parametersNamespaceURI.c_str(): "*" ) ), 
						xoap::XStr( p->first.getName().c_str() ) 
						)->item(0);
      if ( n != NULL ){
	serializer.import( p->second.first, n );
      }
      else{
	std::stringstream ss;
	ss << "Failed to extract parameter '" << p->first << "' from ParameterGetResponse SOAP message ";
	XCEPT_RAISE( xcept::Exception, ss.str() );
      }
    }

    // We're responsible for releasing the memory allocated to DOMDocument
    doc->release();
    // Do not destroy parser as other threads may be using it.
    //xoap::getDOMParserFactory()->destroy("ParseFromSOAP");

  }
  catch( xcept::Exception &e ){
    std::stringstream ss;
    ss << "Failed to extract parameters " << parameters
       << " in namespace " << parametersNamespaceURI;
    XCEPT_RETHROW( xcept::Exception, ss.str(), e );
  }
  catch( std::exception &e ){
    std::stringstream ss;
    ss << "Failed to extract parameters " << parameters
       << " in namespace " << parametersNamespaceURI
       << ": " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( DOMException &e ){
    std::stringstream ss;
    ss << "Failed to extract parameters " << parameters
       << " in namespace " << parametersNamespaceURI
       << ": " << e.getMessage();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...){
    std::stringstream ss;
    ss << "Failed to extract parameters " << parameters
       << " in namespace " << parametersNamespaceURI
       << ": Unknown exception.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
}

std::string 
emu::soap::faultToPlainText( xoap::SOAPFault* fault ){
  std::stringstream ss;

  try{
    ss << std::endl 
       << "code:      " << fault->getFaultCode() << std::endl
       << "summary:   " << fault->getFaultString() << std::endl;
    if ( fault->hasDetail() ){
      xoap::SOAPElement detail = fault->getDetail();
      ss << emu::soap::faultElementToPlainText( &detail, 0 );
    }
  }
  catch( xcept::Exception &e ){
    std::stringstream ss;
    ss << "Failed to convert SOAPFault to plain text : ";
    XCEPT_RETHROW( xcept::Exception, ss.str(), e );
  }
  catch( std::exception &e ){
    std::stringstream ss;
    ss << "Failed to convert SOAPFault to plain text : " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...){
    std::stringstream ss;
    ss << "Failed to convert SOAPFault to plain text : Unknown exception.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }

  return ss.str();
}

std::string 
emu::soap::faultElementToPlainText( xoap::SOAPElement* elem, const int indentDepth ){
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
	ss << emu::soap::faultElementToPlainText( &*c, indentDepth+indentIncrement );
      }
    }
  }
  else{
    ss << std::endl;
  }
  return ss.str();
}

xcept::Exception
emu::soap::faultToException( xoap::SOAPFault* fault ){

  try{
    if ( fault->hasDetail() ){
      xcept::Exception ex;
      xdaq::XceptSerializer::importFrom( fault->getDetail().getDOM(), ex );
      return ex;
    }
    else{
      stringstream ss;
      ss << "Fault code: " << fault->getFaultCode() << ", message: " << fault->getFaultString();
      XCEPT_DECLARE( xcept::Exception, ex, ss.str() );
      return ex;
    }
  }
  catch( xcept::Exception &e ){
    std::stringstream ss;
    ss << "Failed to convert SOAPFault to xcept::Exception : ";
    XCEPT_RETHROW( xcept::Exception, ss.str(), e );
  }
  catch( std::exception &e ){
    std::stringstream ss;
    ss << "Failed to convert SOAPFault to xcept::Exception : " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...){
    std::stringstream ss;
    ss << "Failed to convert SOAPFault to xcept::Exception : Unknown exception.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }

}

xoap::MessageReference 
emu::soap::createFaultReply( const string& code, const string& reason, xcept::Exception* exception ){
  xoap::MessageReference faultReply = xoap::createMessage();
  try{
    xoap::SOAPEnvelope envelope       = faultReply->getSOAPPart().getEnvelope();
    xoap::SOAPName        fault       = envelope.createName( "Fault" );
    xoap::SOAPBodyElement faultElem   = envelope.getBody().addBodyElement( fault );
    xoap::SOAPName    faultcode       = envelope.createName( "faultcode" );
    xoap::SOAPElement faultcodeElem   = faultElem.addChildElement( faultcode );
    xoap::SOAPName    faultstring     = envelope.createName( "faultstring" );
    xoap::SOAPElement faultstringElem = faultElem.addChildElement( faultstring );
    faultcodeElem.addTextNode( code );
    faultstringElem.addTextNode( reason );
    if ( exception != NULL ){
      xoap::SOAPName    detail     = envelope.createName( "detail" );
      xoap::SOAPElement detailElem = faultElem.addChildElement( detail );
      xdaq::XceptSerializer::importFrom( detailElem.getDOM(), *exception );
    }
  }
  catch( xcept::Exception &e ){
    std::stringstream ss;
    ss << "Failed to create SOAP fault reply: ";
    XCEPT_RETHROW( xcept::Exception, ss.str(), e );
  }
  catch( std::exception &e ){
    std::stringstream ss;
    ss << "Failed to create SOAP fault reply: " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...){
    std::stringstream ss;
    ss << "Failed to create SOAP fault reply: Unknown exception.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  return faultReply;
}

void
emu::soap::repairDOM( xoap::MessageReference message ){
  // TODO: Remove emu::soap::repairDOM once a fixed XDAQ version (>=v10.1.0) is deployed.
  try{
    // Get all elements:
    DOMNodeList* nodes = ( dynamic_cast<DOMElement*>( message->getSOAPPart().getEnvelope().getBody().getDOMNode() ) )->getElementsByTagName( xoap::XStr( "*" ) );
    // Loop over all elements:
    for ( XMLSize_t i=0; i < nodes->getLength(); ++i ){
      if ( nodes->item( i )->hasAttributes() ){
	DOMElement* element = dynamic_cast<DOMElement*>( nodes->item( i ) );
	// If 'type' attribute is not associated with any namespace, replace it:
	if ( element->getAttributeNode  (                                  xoap::XStr( "xsi:type" ) ) != NULL &&
	     element->getAttributeNodeNS( xoap::XStr( XSI_NAMESPACE_URI ), xoap::XStr( "type"     ) ) == NULL    ){
	  DOMAttr *condemnedAttribute = element->getAttributeNode( xoap::XStr( "xsi:type" ) );
	  std::string type = xoap::XMLCh2String( condemnedAttribute->getValue() );
	  element->removeAttributeNode( condemnedAttribute );
	  element->setAttributeNS( xoap::XStr( XSI_NAMESPACE_URI ), xoap::XStr( "xsi:type" ), xoap::XStr( type ) );
	}
      }
    }
  }
  catch( xcept::Exception &e ){
    std::stringstream ss;
    ss << "Failed to repair DOM of SOAP message : ";
    XCEPT_RETHROW( xcept::Exception, ss.str(), e );
  }
  catch( std::exception &e ){
    std::stringstream ss;
    ss << "Failed to repair DOM of SOAP message : "<< e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( DOMException &e ){
    std::stringstream ss;
    ss << "Failed to repair DOM of SOAP message : "<< e.getMessage();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...){
    std::stringstream ss;
    ss << "Failed to repair DOM of SOAP message: Unknown exception.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  
}
