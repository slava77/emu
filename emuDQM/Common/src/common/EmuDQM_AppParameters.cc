#include "EmuDQM_AppParameters.h"

namespace emu {
  namespace dqm {


   std::string getScalarParam
    (
     xdaq::ApplicationContext *appContext_,
     xdaq::ApplicationDescriptor* appSrcDescriptor,
     xdaq::ApplicationDescriptor* appDescriptor,
     const std::string                 paramName,
     const std::string                 paramType
     )
      throw (emu::dqm::exception::Exception)
    {
      std::string appClass = appDescriptor->getClassName();
      std::string value    = "";


      try
	{
	  xoap::MessageReference msg = createParameterGetSOAPMsg(appClass, paramName, paramType);

	  xoap::MessageReference reply =
            appContext_->postSOAP(msg, *appSrcDescriptor, *appDescriptor);

	  // Check if the reply indicates a fault occurred
	  xoap::SOAPBody replyBody =
            reply->getSOAPPart().getEnvelope().getBody();

	  if(replyBody.hasFault())
	    {
	      std::stringstream oss;
	      std::string s;

	      oss << "Received fault reply: ";
	      oss << replyBody.getFault().getFaultString();
	      s = oss.str();

	      XCEPT_RAISE(emu::dqm::exception::Exception, s);
	    }

	  value = extractScalarParameterValueFromSoapMsg(reply, paramName);
	}
      catch(xcept::Exception e)
	{
	  std::stringstream s;
	  s << "Failed to get scalar parameter "
	    << paramName << " from application "
	    << appDescriptor->getClassName() << appDescriptor->getInstance();

	  XCEPT_RETHROW(emu::dqm::exception::Exception, s.str(), e);
	}

      return value;
    }



    void setScalarParam
    (
     xdaq::ApplicationContext *appContext_,
     xdaq::ApplicationDescriptor* appSrcDescriptor,
     xdaq::ApplicationDescriptor* appDescriptor,
     const std::string                 paramName,
     const std::string                 paramType,
     const std::string                 paramValue
     )
      throw (emu::dqm::exception::Exception)
    {
      std::string appClass = appDescriptor->getClassName();


      try
	{
	  xoap::MessageReference msg = createParameterSetSOAPMsg(appClass,
								 paramName, paramType, paramValue);

	  xoap::MessageReference reply =
	    appContext_->postSOAP(msg, *appSrcDescriptor, *appDescriptor);

	  // Check if the reply indicates a fault occurred
	  xoap::SOAPBody replyBody =
	    reply->getSOAPPart().getEnvelope().getBody();

	  if(replyBody.hasFault())
	    {
	      std::stringstream oss;
	      std::string s;

	      oss << "Received fault reply: ";
	      oss << replyBody.getFault().getFaultString();
	      s = oss.str();

	      XCEPT_RAISE(xcept::Exception, s);
	    }
	}

      catch(xdaq::exception::Exception e)
	{
	  std::string s = "Failed to set scalar parameter";
	  XCEPT_RETHROW(emu::dqm::exception::Exception, s, e);
	}
      catch(emu::dqm::exception::Exception e)
	{
	  std::string s = "Failed to set scalar parameter";
	  XCEPT_RETHROW(emu::dqm::exception::Exception, s, e);
	}
    }

    xoap::MessageReference createParameterGetSOAPMsg
    (
     const std::string appClass,
     const std::string paramName,
     const std::string paramType
     )
      throw (emu::dqm::exception::Exception)
    {
      std::string appNamespace = "urn:xdaq-application:" + appClass;
      std::string paramXsdType = "xsd:" + paramType;

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
	    envelope.createName("properties", appClass, appNamespace);
	  xoap::SOAPElement propertiesElement =
	    cmdElement.addChildElement(propertiesName);
	  xoap::SOAPName propertiesTypeName =
	    envelope.createName("type", "xsi",
				"http://www.w3.org/2001/XMLSchema-instance");
	  propertiesElement.addAttribute(propertiesTypeName, "soapenc:Struct");
	  xoap::SOAPName propertyName =
	    envelope.createName(paramName, appClass, appNamespace);
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
	  XCEPT_RETHROW(emu::dqm::exception::Exception,
			"Failed to create ParameterGet SOAP message for parameter " +
			paramName + " of type " + paramType, e);
	}
    }


    xoap::MessageReference createParameterSetSOAPMsg
    (
     const std::string appClass,
     const std::string paramName,
     const std::string paramType,
     const std::string paramValue
     )
      throw (emu::dqm::exception::Exception)
    {
      std::string appNamespace = "urn:xdaq-application:" + appClass;
      std::string paramXsdType = "xsd:" + paramType;

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
	    envelope.createName("ParameterSet", "xdaq", "urn:xdaq-soap:3.0");
	  xoap::SOAPBodyElement cmdElement =
	    body.addBodyElement(cmdName);
	  xoap::SOAPName propertiesName =
	    envelope.createName("properties", appClass, appNamespace);
	  xoap::SOAPElement propertiesElement =
	    cmdElement.addChildElement(propertiesName);
	  xoap::SOAPName propertiesTypeName =
	    envelope.createName("type", "xsi",
				"http://www.w3.org/2001/XMLSchema-instance");
	  propertiesElement.addAttribute(propertiesTypeName, "soapenc:Struct");
	  xoap::SOAPName propertyName =
	    envelope.createName(paramName, appClass, appNamespace);
	  xoap::SOAPElement propertyElement =
	    propertiesElement.addChildElement(propertyName);
	  xoap::SOAPName propertyTypeName =
	    envelope.createName("type", "xsi",
				"http://www.w3.org/2001/XMLSchema-instance");

	  propertyElement.addAttribute(propertyTypeName, paramXsdType);

	  propertyElement.addTextNode(paramValue);

	  return message;
	}
      catch(xcept::Exception e)
	{
	  XCEPT_RETHROW(emu::dqm::exception::Exception,
			"Failed to create ParameterSet SOAP message for parameter " +
			paramName + " of type " + paramType + " with value " + paramValue,
			e);
	}
    }



    DOMNode *findNode
    (
     DOMNodeList *nodeList,
     const std::string nodeLocalName
     )
      throw (emu::dqm::exception::Exception)
    {
      DOMNode            *node = 0;
      std::string             name  = "";
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
   

      XCEPT_RAISE(emu::dqm::exception::Exception,
		  "Failed to find node with local name: " + nodeLocalName);

    }

    void sendFSMEventToApp
    (
     const std::string                 eventName,
     xdaq::ApplicationContext *appContext_,
     xdaq::ApplicationDescriptor* appSrcDescriptor,
     xdaq::ApplicationDescriptor* appDescriptor
     )
      throw (emu::dqm::exception::Exception)
    {
      try
	{
	  xoap::MessageReference msg = createSimpleSOAPCmdMsg(eventName);
	  xoap::MessageReference reply =
            appContext_->postSOAP(msg, *appSrcDescriptor, *appDescriptor);

	  // Check if the reply indicates a fault occurred
	  xoap::SOAPBody replyBody =
            reply->getSOAPPart().getEnvelope().getBody();

	  if(replyBody.hasFault())
	    {
	      std::stringstream oss;
	      std::string s;

	      oss << "Received fault reply from ";
	      oss << appDescriptor->getClassName();
	      oss << appDescriptor->getInstance();
	      oss << " : " << replyBody.getFault().getFaultString();
	      s = oss.str();

	      XCEPT_RAISE(emu::dqm::exception::Exception, s);
	    }
	}
      catch(xcept::Exception e)
	{
	  std::stringstream oss;
	  std::string       s;

	  oss << "Failed to send FSM event to ";
	  oss << appDescriptor->getClassName();
	  oss << appDescriptor->getInstance();
	  s = oss.str();

	  XCEPT_RETHROW(emu::dqm::exception::Exception, s, e);
	}
    }

    xoap::MessageReference createSimpleSOAPCmdMsg
    (
     const std::string cmdName
     )
      throw (emu::dqm::exception::Exception)
    {
      try
	{
	  xoap::MessageReference message = xoap::createMessage();
	  xoap::SOAPPart soapPart = message->getSOAPPart();
	  xoap::SOAPEnvelope envelope = soapPart.getEnvelope();
	  xoap::SOAPBody body = envelope.getBody();
	  xoap::SOAPName cmdSOAPName =
            envelope.createName(cmdName, "xdaq", "urn:xdaq-soap:3.0");

	  body.addBodyElement(cmdSOAPName);

	  return message;
	}
      catch(xcept::Exception e)
	{
	  XCEPT_RETHROW(emu::dqm::exception::Exception,
			"Failed to create simple SOAP command message for cmdName " +
			cmdName, e);
      
	}
    }



    std::string extractScalarParameterValueFromSoapMsg
    (
     xoap::MessageReference msg,
     const std::string           paramName
     )
      throw (emu::dqm::exception::Exception)
    {
      std::string paramValue="";
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
	  paramValue = xoap::XMLCh2String(valueNode->getNodeValue());

	  return paramValue;
	}

      catch(xcept::Exception e)
	{
	  XCEPT_RETHROW(emu::dqm::exception::Exception,
			"Parameter " + paramName + " not found", e);
	}

      catch(...)
	{
	  XCEPT_RAISE(emu::dqm::exception::Exception,
		      "Parameter " + paramName + " not found");
	  return paramValue;
	}
    }
  }
}
