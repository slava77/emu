// $Id: EmuSOAPClient.cc,v 3.1 2006/02/28 08:59:18 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#include "EmuSOAPClient.h"
#include "i2oEmuClientMsg.h"

// #include "i2oStreamIOMsg.h"
#include "Task.h"

#include "toolbox/rlist.h"
#include "toolbox/fsm/FiniteStateMachine.h"
#include "toolbox/mem/Reference.h"
#include "toolbox/mem/MemoryPoolFactory.h"
#include "toolbox/mem/Pool.h"
#include "toolbox/mem/Reference.h"
#include "toolbox/mem/Pool.h"
#include "toolbox/mem/CommittedHeapAllocator.h"

#include "i2o/Method.h"
#include "i2o/utils/AddressMap.h"

#include "xdaq/Application.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/NamespaceURI.h"

#include "xoap/MessageFactory.h"
// #include "xoap/include/xoap/MessageReference.h"
// #include "xoap/include/xoap/SOAPMessage.h"
#include "xoap/MessageReference.h"
#include "xoap/SOAPMessage.h"
#include "xoap/AttachmentPart.h"
#include "xoap/domutils.h"

#include "xcept/Exception.h"
#include "xcept/tools.h"


EmuSOAPClient::EmuSOAPClient(xdaq::ApplicationStub* c)
  throw(xdaq::exception::Exception)
  :xdaq::Application(c)
  ,Task("EmuSOAPClient") 
  ,applicationBSem_(BSem::FULL)
{
  getApplicationInfoSpace()->fireItemAvailable("serversClassName", &serversClassName_);
  getApplicationInfoSpace()->fireItemAvailable("serversClassInstance", &serversClassInstance_);
  getApplicationInfoSpace()->fireItemAvailable("nEventCredits",    &nEventCredits_);
  getApplicationInfoSpace()->fireItemAvailable("prescalingFactor", &prescalingFactor_);

  // Add infospace listeners for exporting data values
  getApplicationInfoSpace()->addItemChangedListener ("serversClassName", this);
  getApplicationInfoSpace()->addItemChangedListener ("serversClassInstance", this);
  getApplicationInfoSpace()->addItemChangedListener ("nEventCredits",    this);
  getApplicationInfoSpace()->addItemChangedListener ("prescalingFactor", this);

  // bind SOAP callback server messages
  xoap::bind(this, &EmuSOAPClient::emuDataSOAPMsg, "onEmuDataMessage", XDAQ_NS_URI );

  stringstream ss;
  ss << getApplicationDescriptor()->getClassName();// << getApplicationDescriptor()->getInstance();
  name_ = ss.str();

  // These are used to make sure the service loop starts only after all parameters have been set.
  hasSet_serversClassName_      = false;
  hasSet_serversClassInstance_  = false;
  hasSet_nEventCredits_         = false;
  hasSet_prescalingFactor_      = false;
  serviceLoopStarted_           = false;
}


int EmuSOAPClient::svc()
{
  LOG4CPLUS_DEBUG (getApplicationLogger(), "starting send loop...");

  while(true)
    {

      sleep(1);
      sendCreditSOAPMessage();

    }

  return 0;
}

//
// used to start the service loop sight after parameter initialization
//	
void EmuSOAPClient::actionPerformed (xdata::Event& e) 
{
  // update measurements monitors		
  if (e.type() == "ItemChangedEvent")
    {
      std::string item = dynamic_cast<xdata::ItemChangedEvent&>(e).itemName();
      if ( item == "serversClassName")
	{
	  try 
	    {
	      destination_ = getApplicationContext()->getApplicationGroup()->getApplicationDescriptors(serversClassName_.toString().c_str());
	      hasSet_serversClassName_ = true;
	    }
	  catch (xdaq::exception::Exception& e)
	    {
	      LOG4CPLUS_ERROR (getApplicationLogger(), 
			       "No " << serversClassName_.toString() << 
			       "found. EmuSOAPClient cannot be configured." << 
			       xcept::stdformat_exception_history(e));
// 	      XCEPT_RETHROW (xdaq::exception::Exception, 
// 			     toolbox::toString("No %s application instance found. EmuSOAPClient cannot be configured.",
// 					       serversClassName_.toString().c_str()),
// 			     e);
	    }	
	}
      if ( item == "serversClassInstance")
	{
	  LOG4CPLUS_INFO(getApplicationLogger(), "Server " << serversClassName_.toString() << " instance " << serversClassInstance_ );
	  hasSet_serversClassInstance_ = true;
	}
      if ( item == "nEventCredits")    hasSet_nEventCredits_    = true;
      if ( item == "prescalingFactor") hasSet_prescalingFactor_ = true;
      // start service loop when all infospace parameters have been set
      if ( 
	  hasSet_serversClassName_      &&
	  hasSet_serversClassInstance_  &&
	  hasSet_nEventCredits_         &&
	  hasSet_prescalingFactor_      &&
	  !serviceLoopStarted_             ) {
	LOG4CPLUS_INFO(getApplicationLogger(),"Starting work loop.");
	if ( this->activate() == 0 ) serviceLoopStarted_ = true;
      }
    }	
}


int EmuSOAPClient::sendCreditSOAPMessage() 
  throw ( xoap::exception::Exception )
{
  xoap::MessageReference msg = xoap::createMessage();

  xoap::SOAPPart soap = msg->getSOAPPart();
  xoap::SOAPEnvelope envelope = soap.getEnvelope();
                        
  envelope.addNamespaceDeclaration ("xsi",
				    "http://www.w3.org/1999/XMLSchema-instance");
  envelope.addNamespaceDeclaration ("xsd",
				    "http://www.w3.org/1999/XMLSchema");

                        
  xoap::SOAPName xsiType = envelope.createName("type",
					       "xsi",
					       "http://www.w3.org/1999/XMLSchema-instance");
  xoap::SOAPName encodingStyle = envelope.createName("encodingStyle",
						     "soap-env",
						     "http://schemas.xmlsoap.org/soap/envelope/");

  xoap::SOAPBody body = envelope.getBody();


  xoap::SOAPName command = envelope.createName("onClientCreditMessage",
					       "xdaq", 
					       "urn:xdaq-soap:3.0");
  xoap::SOAPElement bodyElement = body.addBodyElement(command);
  bodyElement.addAttribute(encodingStyle,
			   "http://schemas.xmlsoap.org/soap/encoding/");

  xoap::SOAPName    name        = envelope.createName( "clientName" );
  xoap::SOAPElement nameElement = bodyElement.addChildElement( name );
  nameElement.addAttribute( xsiType, "xsd:int" );
  nameElement.addTextNode( name_ );

  xoap::SOAPName    credits        = envelope.createName( "nEventCredits" );
  xoap::SOAPElement creditsElement = bodyElement.addChildElement( credits );
  creditsElement.addAttribute( xsiType, "xsd:int" );
  creditsElement.addTextNode( nEventCredits_.toString() );

  xoap::SOAPName prescaling = envelope.createName( "prescalingFactor" );
  xoap::SOAPElement prescalingElement = bodyElement.addChildElement( prescaling );
  prescalingElement.addAttribute( xsiType, "xsd:int" );
  prescalingElement.addTextNode( prescalingFactor_.toString() );
  

  xdaq::ApplicationDescriptor* serverDescriptor = 
    getApplicationContext()
    ->getApplicationGroup()
    ->getApplicationDescriptor( serversClassName_, 0 );

  string s;
  msg->writeTo(s);
  LOG4CPLUS_DEBUG(getApplicationLogger(), "Sending to " << 
		  serverDescriptor->getClassName() << " :" << endl << s );
  xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, serverDescriptor );
  s = "";
  reply->writeTo(s);
  LOG4CPLUS_DEBUG(getApplicationLogger(), "Got reply: " << s );

  return 0;
}

xoap::MessageReference EmuSOAPClient::emuDataSOAPMsg(xoap::MessageReference msg)
  throw (xoap::exception::Exception)
{
//   stringstream ss;
//   std::multimap< std::string, std::string, std::less< std::string > > h = msg->getMimeHeaders()->getAllHeaders();
//   std::multimap< std::string, std::string, std::less< std::string > >::iterator i;
//   for( i = h.begin(); i != h.end(); ++i )
//     ss << "            " << i->first << " " << i->second << endl;
//   string s;
//   msg->writeTo(s);
//   LOG4CPLUS_DEBUG(getApplicationLogger(), 
// 		  "Received data message with " << h.size() << 
// 		  " mime headers:" << endl << ss.str() << s );
  LOG4CPLUS_INFO(getApplicationLogger(), "Received data message.");
  LOG4CPLUS_DEBUG(getApplicationLogger(), printMessageReceived(msg) );

  xoap::MessageReference reply = xoap::createMessage();
  xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
  xoap::SOAPName responseName = envelope.createName( "onMessageResponse", "xdaq", XDAQ_NS_URI);
  envelope.getBody().addBodyElement ( responseName );
  return reply;
}


// TODO:

std::string EmuSOAPClient::printMessageReceived( xoap::MessageReference msg ){
  stringstream ss;

  xoap::SOAPPart part = msg->getSOAPPart();
  xoap::SOAPEnvelope env = part.getEnvelope();
  xoap::SOAPBody body = env.getBody();
  DOMNode *bodyNode = body.getDOMNode();
  DOMNodeList *bodyList = bodyNode->getChildNodes();
  DOMNode *functionNode = findNode(bodyList, "onEmuDataMessage");
  DOMNodeList *parameterList = functionNode->getChildNodes();
  DOMNode *parameterNode = findNode(parameterList, "serverName");
  string sn              = xoap::XMLCh2String(parameterNode->getFirstChild()->getNodeValue());
  parameterNode          = findNode(parameterList, "serverInstance");
  string si              = xoap::XMLCh2String(parameterNode->getFirstChild()->getNodeValue());
  parameterNode          = findNode(parameterList, "runNumber");
  string sr              = xoap::XMLCh2String(parameterNode->getFirstChild()->getNodeValue());
  parameterNode          = findNode(parameterList, "nEventCreditsHeld");
  string sc              = xoap::XMLCh2String(parameterNode->getFirstChild()->getNodeValue());
  
  ss <<
    "Received from server "   << sn <<
    " instance "              << si <<
    ". Run "                  << sr <<
    ". nEventCreditsHeld = "  << sc;

  std::list< xoap::AttachmentPart * > attachments = msg->getAttachments();
  ss << endl << attachments.size() << " attachments:" << endl;
  int count=0;
  for ( std::list< xoap::AttachmentPart * >::iterator a=attachments.begin(); a!=attachments.end(); ++a ){
    (*a)->removeAllMimeHeaders();
    char *data = (*a)->getContent();
    int   size = (*a)->getSize();
    ss << "Attachment " << count << " ("<< size <<" bytes)" <<endl;
    unsigned short *shorts = reinterpret_cast<unsigned short *>(data);
    int            nshorts = size / sizeof(unsigned short);
    ss<<std::hex;
    for(int i = 0; i < nshorts; i+=4)
      {
	ss << "      ";
	ss.width(4); ss.fill('0');    
	ss << shorts[i+3] << " ";
	ss.width(4); ss.fill('0');    
	ss << shorts[i+2] << " ";
	ss.width(4); ss.fill('0');    
	ss << shorts[i+1] << " ";
	ss.width(4); ss.fill('0');    
	ss << shorts[i] << std::endl;
      }
    ss<<std::dec;
    ss.width(0);
    ++count;
  }

  return ss.str();
}

DOMNode *EmuSOAPClient::findNode(DOMNodeList *nodeList,
				 const string nodeLocalName)
  throw ( xoap::exception::Exception )
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

    XCEPT_RAISE( xoap::exception::Exception,
        "Failed to find node with local name: " + nodeLocalName);
}


XDAQ_INSTANTIATOR_IMPL(EmuSOAPClient)
