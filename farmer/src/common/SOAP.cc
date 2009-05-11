#include "emu/farmer/utils/SOAP.h"

#include "xoap/SOAPBody.h"
#include "xoap/SOAPBodyElement.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/MessageFactory.h"

#include "toolbox/Runtime.h"
#include "toolbox/regex.h"
#include <xercesc/util/XMLURL.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include "xcept/tools.h"
#include "xoap/DOMParser.h"
#include "xoap/DOMParserFactory.h"
#include "xoap/domutils.h"

// For postSOAP
#include "pt/PeerTransportAgent.h"
#include "pt/PeerTransportReceiver.h"
#include "pt/PeerTransportSender.h"
#include "pt/SOAPMessenger.h"
#include "xdaq/exception/HostNotFound.h"
#include "xdaq/exception/ApplicationDescriptorNotFound.h"

#include <xercesc/parsers/XercesDOMParser.hpp>


xoap::MessageReference emu::farmer::utils::createStartXdaqExeSOAPMsg( const string& host, const int port, const string& user, 
								      const string& jid,
								      map<string,string> environmentVariables )
  throw (xcept::Exception){
  xoap::MessageReference message = xoap::createMessage();

  try{
    // Construct SOAP
    xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();

    xoap::SOAPName name = envelope.createName("startXdaqExe", "xdaq", "urn:xdaq-soap:3.0");
    xoap::SOAPBodyElement bodyelement = envelope.getBody().addBodyElement(name);
    name = envelope.createName("execPath", "", "");
    bodyelement.addAttribute( name, environmentVariables["XDAQ_ROOT"]+"/bin/xdaq.exe" );
    name = envelope.createName("argv", "", "");
    stringstream ssport; ssport << port;
    bodyelement.addAttribute( name, string("-h ") + host + " -p " + ssport.str() );
    name = envelope.createName("user", "", "");
    bodyelement.addAttribute( name, user );
    name = envelope.createName("jid", "", "");
    //bodyelement.addAttribute( name, string("farmer://") + host + ":" + ssport.str() );
    bodyelement.addAttribute( name, jid );

    name = envelope.createName("EnvironmentVariable", "", "");
    xoap::SOAPElement childelement = bodyelement.addChildElement( name );
    for ( map<string,string>::const_iterator e=environmentVariables.begin(); e!=environmentVariables.end(); ++e ){
      name = envelope.createName( e->first, "", "" );
      childelement.addAttribute( name, e->second );
    }

  }catch( xcept::Exception& e ){
    stringstream ess; ess << "Failed to create StartXdaqExe SOAP message: ";
    XCEPT_RETHROW( xcept::Exception, ess.str(), e );
  }catch( std::exception& e ){
    stringstream ess; ess << "Failed to create StartXdaqExe SOAP message: " << e.what();
    XCEPT_RAISE( xcept::Exception, ess.str() );
  }catch(...){
    stringstream ess; ess << "Failed to create StartXdaqExe SOAP message: unexpected exception.";
    XCEPT_RAISE( xcept::Exception, ess.str() );
  }
  //cout << endl; message->writeTo( cout ); cout.flush(); cout << endl;
  return message;
}

xoap::MessageReference emu::farmer::utils::createConfigureXdaqExeSOAPMsg( string& configuration )
  throw (xcept::Exception){
  xoap::MessageReference message = xoap::createMessage();

  try{
    // We need to nest the configuration XML in the SOAP.

    // Parse the configuration XML into a DOM
    MemBufInputSource inCfg( (XMLByte*)configuration.c_str(), configuration.size(), "dummyId" );
    XercesDOMParser parser;
    parser.parse( inCfg );
    DOMDocument *cfgDOM = parser.getDocument();
    
    // Construct SOAP
    xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
    xoap::SOAPName name = envelope.createName("Configure", "xdaq", "urn:xdaq-soap:3.0");
    xoap::SOAPBodyElement bodyelement = envelope.getBody().addBodyElement(name);
    // Import the configuration's root element to the SOAP document...
    DOMNode *cfgElement =  bodyelement.getDOMNode()->getOwnerDocument()->importNode( cfgDOM->getDocumentElement(), true );
    // ...and append it to the SOAP body element as a child.
    bodyelement.getDOMNode()->appendChild( cfgElement );

  }catch( xcept::Exception& e ){
    stringstream ess; ess << "Failed to create Configure SOAP message: ";
    XCEPT_RETHROW( xcept::Exception, ess.str(), e );
  }catch( std::exception& e ){
    stringstream ess; ess << "Failed to create Configure SOAP message: " << e.what();
    XCEPT_RAISE( xcept::Exception, ess.str() );
  }catch(...){
    stringstream ess; ess << "Failed to create Configure SOAP message: unexpected exception.";
    XCEPT_RAISE( xcept::Exception, ess.str() );
  }
  //cout << endl; message->writeTo( cout ); cout.flush(); cout << endl;
  return message;
}

xoap::MessageReference emu::farmer::utils::createKillByJidSOAPMsg( const string& jid )
  throw (xcept::Exception){
  xoap::MessageReference message = xoap::createMessage();

  try{
    // Construct SOAP
    xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();

    xoap::SOAPName name = envelope.createName("killExec", "xdaq", "urn:xdaq-soap:3.0");
    xoap::SOAPBodyElement bodyelement = envelope.getBody().addBodyElement(name);
    name = envelope.createName("jid", "", "");
    bodyelement.addAttribute( name, jid );
  }catch( xcept::Exception& e ){
    stringstream ess; ess << "Failed to create killExec SOAP message: ";
    XCEPT_RETHROW( xcept::Exception, ess.str(), e );
  }catch( std::exception& e ){
    stringstream ess; ess << "Failed to create killExec SOAP message: " << e.what();
    XCEPT_RAISE( xcept::Exception, ess.str() );
  }catch(...){
    stringstream ess; ess << "Failed to create killExec SOAP message: unexpected exception.";
    XCEPT_RAISE( xcept::Exception, ess.str() );
  }
  //cout << endl; message->writeTo( cout ); cout.flush(); cout << endl;
  return message;
}


xoap::MessageReference emu::farmer::utils::createParameterGetSOAPMsg
( const string appClass,
  const string paramName,
  const string paramType )
  throw (xcept::Exception)
{
  string appNamespace = "urn:xdaq-application:" + appClass;
  string paramXsdType = "xsd:" + paramType;

  try
    {
      xoap::MessageReference message = xoap::createMessage();
      xoap::SOAPPart soapPart = message->getSOAPPart();
      xoap::SOAPEnvelope envelope = soapPart.getEnvelope();
      envelope.addNamespaceDeclaration("xsi",
				       "http://www.w3.org/2001/XMLSchema-instance");
      envelope.addNamespaceDeclaration("xsd",
				       "http://www.w3.org/2001/XMLSchema");
      envelope.addNamespaceDeclaration("soapenc",
				       "http://schemas.xmlsoap.org/soap/encoding/");
      xoap::SOAPBody body = envelope.getBody();
      xoap::SOAPName cmdName =
	envelope.createName("ParameterGet", "xdaq", "urn:xdaq-soap:3.0");
      xoap::SOAPBodyElement cmdElement =
	body.addBodyElement(cmdName);
      xoap::SOAPName propertiesName =
	envelope.createName("properties", "xapp", appNamespace);
      xoap::SOAPElement propertiesElement =
	cmdElement.addChildElement(propertiesName);
      xoap::SOAPName propertiesTypeName =
	envelope.createName("type", "xsi",
			    "http://www.w3.org/2001/XMLSchema-instance");
      propertiesElement.addAttribute(propertiesTypeName, "soapenc:Struct");
      xoap::SOAPName propertyName =
	envelope.createName(paramName, "xapp", appNamespace);
      xoap::SOAPElement propertyElement =
	propertiesElement.addChildElement(propertyName);
      xoap::SOAPName propertyTypeName =
	envelope.createName("type", "xsi",
			    "http://www.w3.org/2001/XMLSchema-instance");

      propertyElement.addAttribute(propertyTypeName, paramXsdType);

      return message;
    }
  catch(xcept::Exception e)
    {
      XCEPT_RETHROW(xcept::Exception,
		    "Failed to create ParameterGet SOAP message for parameter " +
		    paramName + " of type " + paramType, e);
    }
}

string emu::farmer::utils::extractScalarParameterValueFromSoapMsg
( xoap::MessageReference msg,
  const string           paramName )
  throw (xcept::Exception)
{
  try
    {
      xoap::SOAPPart part = msg->getSOAPPart();
      xoap::SOAPEnvelope env = part.getEnvelope();
      xoap::SOAPBody body = env.getBody();
      DOMNode *bodyNode = body.getDOMNode();
      DOMNodeList *bodyList = bodyNode->getChildNodes();
      DOMNode *responseNode = findNode(bodyList, "ParameterGetResponse");
      DOMNodeList *responseList = responseNode->getChildNodes();
      DOMNode *propertiesNode = findNode(responseList, "properties");
      DOMNodeList *propertiesList = propertiesNode->getChildNodes();
      DOMNode *paramNode = findNode(propertiesList, paramName);
      DOMNodeList *paramList = paramNode->getChildNodes();
      DOMNode *valueNode = paramList->item(0);
      string paramValue = xoap::XMLCh2String(valueNode->getNodeValue());

      return paramValue;
    }
  catch(xcept::Exception e)
    {
      XCEPT_RETHROW(xcept::Exception,
		    "Parameter " + paramName + " not found", e);
    }
  catch(...)
    {
      XCEPT_RAISE(xcept::Exception,
		  "Parameter " + paramName + " not found");
    }
}

string emu::farmer::utils::extractJidFromSoapMsg
( xoap::MessageReference msg )
  throw (xcept::Exception)
{
  string paramValue;
  try
    {
      xoap::SOAPPart part = msg->getSOAPPart();
      xoap::SOAPEnvelope env = part.getEnvelope();
      xoap::SOAPBody body = env.getBody();
      DOMNode *bodyNode = body.getDOMNode();
      XMLCh* jid=XMLString::transcode("rcms:jid");
      DOMNodeList *jidElements = bodyNode->getOwnerDocument()->getElementsByTagName( jid );
      XMLString::release( &jid );
      if ( jidElements->getLength() == 1 ){
	paramValue = xoap::XMLCh2String( jidElements->item(0)->getFirstChild()->getNodeValue() );
      }
      else{
	XCEPT_RAISE(xcept::Exception, "No rcms:jid element found in SOAP reply from JobControl.");
      }
      return paramValue;
    }
  catch(xcept::Exception e)
    {
      XCEPT_RETHROW(xcept::Exception, "Parameter jid not found", e);
    }
  catch(...)
    {
      XCEPT_RAISE(xcept::Exception, "Parameter jid not found");
    }
}

xoap::MessageReference emu::farmer::utils::postSOAP
(
 xoap::MessageReference message, 
 const string& fromURL,
 const string& toURL,
 const int localId
 ) 
  throw (xcept::Exception)
// Adapted from xdaq::ApplicationContextImpl::postSOAP.
// This is necessary for sending SOAP to contexts not defined in this process's config file.
{
	
  bool setSOAPAction = false;
  if ( message->getMimeHeaders()->getHeader("SOAPAction").size() == 0 )
    {
      stringstream URN;
      URN << "urn:xdaq-application:lid=" << localId;
      message->getMimeHeaders()->setHeader("SOAPAction", URN.str());		
      setSOAPAction = true;
    }
	
  xoap::SOAPBody b = message->getSOAPPart().getEnvelope().getBody();
  DOMNode* node = b.getDOMNode();
	
  DOMNodeList* bodyList = node->getChildNodes();
  DOMNode* command = bodyList->item(0);
	
  if (command->getNodeType() == DOMNode::ELEMENT_NODE) 
    {                
      try
	{	
	  // Local dispatch: if remote and local address are on same host, get local messenger
			
	  // Get the address on the fly from the URL
	  pt::Address::Reference remoteAddress = pt::getPeerTransportAgent()->createAddress(toURL,"soap");
				
	  pt::Address::Reference localAddress  = pt::getPeerTransportAgent()->createAddress(fromURL,"soap");
	  //pt::getPeerTransportAgent()->createAddress(getApplicationDescriptor()->getContextDescriptor()->getURL(),"soap");
			
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

DOMNode* emu::farmer::utils::findNode( DOMNodeList *nodeList, const string& nodeLocalName )
  throw (xcept::Exception)
{
    DOMNode            *node = 0;
    string             name  = "";
    unsigned int       i     = 0;

    for(i=0; i<nodeList->getLength(); i++)
    {
        node = nodeList->item(i);

        if(node->getNodeType() == DOMNode::ELEMENT_NODE)
        {
            name = xoap::XMLCh2String(node->getLocalName());

            if(name == nodeLocalName)
            {
                return node;
            }
        }
    }

    XCEPT_RAISE(xcept::Exception,
        "Failed to find node with local name: " + nodeLocalName);
}
