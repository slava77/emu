#include <sstream>

#include "toolbox/net/URL.h"
#include "pt/PeerTransportAgent.h"
#include "pt/SOAPMessenger.h"

#include "EmuSOAPServer.h"

EmuSOAPServer::EmuSOAPServer( xdaq::Application                    *parentApp,
			      const string                          clientName,
			      xdata::Serializable                  *persists,
			      xdata::Serializable                  *prescaling,
			      xdata::Serializable                  *onRequest,
			      xdata::Serializable                  *creditsHeld,
			      const Logger                         *logger )
  throw( xcept::Exception )
  : EmuServer( parentApp,
	       clientName,
	       prescaling,
	       onRequest,
	       creditsHeld,
	       logger       ),
    persists_( dynamic_cast<xdata::Boolean*>( persists ) )
{
  findClientDescriptor();

  createMIMEInfo();

  maxMessagesPendingTransmission_ = 10;

  dataIsPendingTransmission_ = false;

  LOG4CPLUS_INFO(logger_, name_ << " server has been created" );
}

EmuSOAPServer::~EmuSOAPServer(){}

void EmuSOAPServer::createMIMEInfo(){
  toolbox::net::URL address(appContext_->getContextDescriptor()->getURL());
  contentId_       = string("<") + "EmuData@" +  address.getHost() + ">";
  contentType_     = "Emu/data";
  contentLocation_ = address.toString() + "/" + appDescriptor_->getURN();
  contentEncoding_ = "binary";
}

void EmuSOAPServer::addData( const int            runNumber, 
			     const int            nEvents, 
			     const bool           completesEvent, 
			     const unsigned short errorFlag, 
			     char*                data, 
			     const int            dataLength ){

  LOG4CPLUS_DEBUG(logger_, name_ << " has " << messages_.size() <<
		  " messages at event " << nEvents <<
		  ", holding " << *nEventCreditsHeld_ <<
		  " credits. Prescaling: 1/" << *prescaling_ << " (EmuSOAPServer::addData)");

  bool isToBeSent = false;
  if ( sendDataOnRequestOnly_ ){
    if ( nEventCreditsHeld_->value_ && prescaling_->value_ ){
      if ( nEvents % prescaling_->value_ == 0 ){
	isToBeSent = true;
      }
    }
  }
  else if ( prescaling_->value_ ){
    if ( nEvents % prescaling_->value_ == 0 ){
      isToBeSent = true;
    }
  }

  if ( messages_.size() >= maxMessagesPendingTransmission_ ||
       messages_.size() >= nEventCreditsHeld_->value_         ){
    isToBeSent = false;
  }

  if ( isToBeSent ) {
    LOG4CPLUS_DEBUG(logger_, name_ << " SOAP server: Appending data to be sent *****"); 
    try {
      runNumber_ = runNumber;
      errorFlag_ = errorFlag;
      appendData( data, dataLength, completesEvent );
    }
    catch( xoap::exception::Exception& xe ){
      LOG4CPLUS_ERROR(logger_,
		      "Failed to append data to be sent to " << clientName_
		      << " : " << stdformat_exception_history(xe));
      //       XCEPT_RETHROW (xcept::Exception, "Failed to append data to SOAP message: ", xe); 
    }
  }
}

void EmuSOAPServer::appendData( char* const         data, 
				const unsigned long dataLength, 
				const bool          completesEvent )
  throw( xoap::exception::Exception )
{

  LOG4CPLUS_DEBUG(logger_, name_ << " has " << messages_.size() <<
		  " messages, holding " << *nEventCreditsHeld_ <<
		  " credits. Prescaling: 1/" << *prescaling_ << " (EmuSOAPServer::appendData)");

  // See if a new message needs to be created
  if ( messageReference_.isNull() ){
    createMessage();
  }

  // Add data as attachment
  fillMessage( data, dataLength );

  // Append message to queue if complete
  if ( completesEvent ){
    messages_.push_back( messageReference_ );
    messageReference_          = NULL;
    dataIsPendingTransmission_ = true;
  }
}

void EmuSOAPServer::createMessage()
  throw ( xoap::exception::Exception )
{
  messageReference_ = xoap::createMessage();

  xoap::SOAPPart soap = messageReference_->getSOAPPart();
  xoap::SOAPEnvelope envelope = soap.getEnvelope();
                        
  envelope.addNamespaceDeclaration ("xsi",
				    "http://www.w3.org/1999/XMLSchema-instance");
  envelope.addNamespaceDeclaration ("xsd",
				    "http://www.w3.org/1999/XMLSchema");

//   envelope.addNamespaceDeclaration("soapenc",
// 				   "http://schemas.xmlsoap.org/soap/encoding/");
                        
  xoap::SOAPName xsiType = envelope.createName("type",
					       "xsi",
					       "http://www.w3.org/1999/XMLSchema-instance");
  xoap::SOAPName encodingStyle = envelope.createName("encodingStyle",
						     "soap-env",
						     "http://schemas.xmlsoap.org/soap/envelope/");

  xoap::SOAPBody body = envelope.getBody();



  xoap::SOAPName command = envelope.createName("onEmuDataMessage",
					       "xdaq", 
					       "urn:xdaq-soap:3.0");
  xoap::SOAPElement bodyElement = body.addBodyElement(command);
  bodyElement.addAttribute(encodingStyle,
			   "http://schemas.xmlsoap.org/soap/encoding/");

  xoap::SOAPName    name        = envelope.createName( "serverName" );
  xoap::SOAPElement nameElement = bodyElement.addChildElement( name );
  nameElement.addAttribute( xsiType, "xsd:string" );
  nameElement.addTextNode( parentApp_->getApplicationDescriptor()->getClassName() );

  xoap::SOAPName    instance        = envelope.createName( "serverInstance" );
  xoap::SOAPElement instanceElement = bodyElement.addChildElement( instance );
  instanceElement.addAttribute( xsiType, "xsd:int" );
  stringstream ins; ins << parentApp_->getApplicationDescriptor()->getInstance();
  instanceElement.addTextNode( ins.str() );

  xoap::SOAPName    runnum        = envelope.createName( "runNumber" );
  xoap::SOAPElement runnumElement = bodyElement.addChildElement( runnum );
  runnumElement.addAttribute( xsiType, "xsd:int" );
  stringstream rn; rn << runNumber_;
  runnumElement.addTextNode( rn.str() );

  xoap::SOAPName    errorflag        = envelope.createName( "errorFlag" );
  xoap::SOAPElement errorflagElement = bodyElement.addChildElement( errorflag );
  errorflagElement.addAttribute( xsiType, "xsd:int" );
  stringstream ef; ef << errorFlag_;
  errorflagElement.addTextNode( ef.str() );

  xoap::SOAPName    credits        = envelope.createName( "nEventCreditsHeld" );
  xoap::SOAPElement creditsElement = bodyElement.addChildElement( credits );
  creditsElement.addAttribute( xsiType, "xsd:int" );
  creditsElement.addTextNode( nEventCreditsHeld_->toString() );

  //     }
  //   catch(xoap::exception::Exception& xe)
  //     {
  //       XCEPT_RETHROW (xgi::exception::Exception, "Could not create SOAP message: ", xe); 
  // //       LOG4CPLUS_ERROR(logger_,
  // // 		      "Could not create SOAP message: " << 
  // // 		      xcept::stdformat_exception_history(xe));
  //     }
}

void EmuSOAPServer::fillMessage( char* const        data,
				 const unsigned int dataLength )
  throw( xoap::exception::Exception )
{
  //   try
  //     {

  xoap::AttachmentPart* attachment = 
    messageReference_->createAttachmentPart( data, dataLength, contentType_ );
  attachment->setContentEncoding( contentEncoding_ );
  attachment->setContentId( contentId_ );
  attachment->setContentLocation( contentLocation_ );

  messageReference_->addAttachmentPart(attachment);
  //     }
  //   catch(xoap::exception::Exception& xe)
  //     {
  //       LOG4CPLUS_ERROR(logger_,
  // 		      "Could not attach data to SOAP message: " << 
  // 		      xcept::stdformat_exception_history(xe));
  //     }
}

xoap::MessageReference EmuSOAPServer::getOldestMessagePendingTransmission(){

  xoap::MessageReference msg;

  if ( messages_.empty() ){
    return msg;
  }

  msg = messages_.front();
  messages_.pop_front();

  if ( msg.isNull() ){ 
    LOG4CPLUS_DEBUG(logger_, name_ << " SOAP server: Oldest message empty?!"); 
  }
  else{
    // If non-persistent client (curl), decrement number of credits here
    if ( nEventCreditsHeld_->value_ > 0 && !persists_->value_ )
      nEventCreditsHeld_->value_--;
  }

  return msg;

}

void EmuSOAPServer::sendData()
  throw ( xcept::Exception )
{

  if ( messages_.empty() ){
//     LOG4CPLUS_DEBUG(logger_, name_ << " SOAP server has nothing to send!" );
    return;
  }

  LOG4CPLUS_DEBUG(logger_, name_ << " has " << messages_.size() <<
		  " messages, holding " << *nEventCreditsHeld_ <<
		  " credits. Prescaling: 1/" << *prescaling_ << " (EmuSOAPServer::sendData)");

  xoap::MessageReference msg = messages_.front();


  if ( msg.isNull() ){ 
    LOG4CPLUS_DEBUG(logger_, name_ << " SOAP server: Oldest message empty?!"); 
    return;
  }

  // If the client descriptor is known: 
  // (otherwise send data in prompt reply from EmuRUI::onSOAPClientCreditMsg)
  if ( clientDescriptor_ ){
    try
      {
// 	string s;
// 	msg->writeTo(s);
// 	LOG4CPLUS_DEBUG(logger_, name_ << " is sending to " << 
// 			clientDescriptor_->getClassName() << " :" << endl << s );
	printMessages();

	xoap::MessageReference reply = appContext_->postSOAP(msg, clientDescriptor_);

	if ( !reply.isNull() ){
	  if ( nEventCreditsHeld_->value_ > 0 &&
	       !reply->getSOAPPart().getEnvelope().getBody().hasFault() )
	    nEventCreditsHeld_->value_--;
	}

// 	s = "";
// 	reply->writeTo(s);
// 	LOG4CPLUS_DEBUG(logger_, name_ << " got reply: " << s );
      }
    catch (xdaq::exception::Exception& e) 
      {
	XCEPT_RETHROW (xcept::Exception, "Could not send SOAP message", e);
      }

    messages_.pop_front();

  }

//   // To an arbitrary external address
//   pt::Address::Reference destAddress  = pt::getPeerTransportAgent()
//     ->createAddress("URL_REMEMBERED", 
// 		    "soap");
//   pt::Address::Reference localAddress = pt::getPeerTransportAgent()
//     ->createAddress(appContext_->getContextDescriptor()->getURL(), 
// 		    "soap");

//   // These two lines cannot be merged, since a reference that is a temporary object
//   // would delete the contained object pointer immediately after use.

//   pt::Messenger::Reference mr = pt::getPeerTransportAgent()->getMessenger(destAddress,localAddress);
//   pt::SOAPMessenger& m = dynamic_cast<pt::SOAPMessenger&>(*mr);

//   // fill the SOAPAction field with the value remembered
//   msg->getMimeHeaders()->setHeader("SOAPAction", "ACTION_REMEMBERED");

//   xoap::MessageReference r = m.send( msg );
  
//   xoap::SOAPBody rb = r->getSOAPPart().getEnvelope().getBody();
//   if (rb.hasFault())
//     {
//       LOG4CPLUS_ERROR (logger_, rb.getFault().getFaultString());
//     }

}

void EmuSOAPServer::printMessages(){
  stringstream allMessages;
  allMessages << messages_.size() << " messages to be sent from " << name_ << " server" << endl;
//   for ( unsigned int i=0; i<messages_.size(); ++i ){
//     string s;
//     messages_[i]->writeTo(s);
//     allMessages << "Message " << i << endl << s << endl;
//   }
  LOG4CPLUS_DEBUG(logger_, allMessages.str() );
}
