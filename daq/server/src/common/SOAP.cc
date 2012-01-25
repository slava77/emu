#include <sstream>

#include "toolbox/net/URL.h"
#include "pt/PeerTransportAgent.h"
#include "pt/SOAPMessenger.h"

#include "emu/daq/server/SOAP.h"
#include "emu/soap/ToolBox.h"

emu::daq::server::SOAP::SOAP( xdaq::Application                    *parentApp,
			      const string                          clientName,
			      const unsigned int                    clientInstance,
			      xdata::Serializable                  *persists,
			      xdata::Serializable                  *prescaling,
			      xdata::Serializable                  *onRequest,
			      xdata::Serializable                  *creditsHeld,
			      const Logger                         *logger )
  throw( xcept::Exception )
  : emu::daq::server::Base( parentApp,
			    clientName,
			    clientInstance,
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

emu::daq::server::SOAP::~SOAP(){}

void emu::daq::server::SOAP::createMIMEInfo(){
  toolbox::net::URL address(appContext_->getContextDescriptor()->getURL());
  contentId_       = string("<") + "EmuData@" +  address.getHost() + ">";
  contentType_     = "Emu/data";
  contentLocation_ = address.toString() + "/" + appDescriptor_->getURN();
  contentEncoding_ = "binary";
}

void emu::daq::server::SOAP::addData( const int               runNumber, 
				      const int               runStartUTC,
				      const uint64_t          nEvents, 
				      const PositionInEvent_t position, 
				      const uint16_t          errorFlag, 
				      char*                   data, 
				      const size_t            dataLength ){

  LOG4CPLUS_DEBUG(logger_, name_ << " has " << messages_.size() <<
		  " messages at event " << nEvents <<
		  ", holding " << *nEventCreditsHeld_ <<
		  " credits. Prescaling: 1/" << *prescaling_ << " (emu::daq::server::SOAP::addData)");

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
      runNumber_   = runNumber;
      runStartUTC_ = runStartUTC;
      errorFlag_   = errorFlag;
      appendData( data, dataLength, position );
    }
    catch( xoap::exception::Exception& xe ){
      LOG4CPLUS_ERROR(logger_,
		      "Failed to append data to be sent to " << clientName_
		      << " : " << stdformat_exception_history(xe));
      //       XCEPT_RETHROW (xcept::Exception, "Failed to append data to SOAP message: ", xe); 
    }
  }
}

void emu::daq::server::SOAP::appendData( char* const             data, 
					 const size_t            dataLength, 
					 const PositionInEvent_t position )
  throw( xoap::exception::Exception )
{

  LOG4CPLUS_DEBUG(logger_, name_ << " has " << messages_.size() <<
		  " messages, holding " << *nEventCreditsHeld_ <<
		  " credits. Prescaling: 1/" << *prescaling_ << " (emu::daq::server::SOAP::appendData)");

  // See if a new message needs to be created
  if ( messageReference_.isNull() ){
    createMessage();
  }

  // Add data as attachment
  fillMessage( data, dataLength );

  // Append message to queue if complete
  if ( position & emu::daq::server::endsEvent ){
    messages_.push_back( messageReference_ );
    //     messageReference_          = NULL;
    messageReference_ = xoap::MessageReference(NULL);
    dataIsPendingTransmission_ = true;
  }
}

void emu::daq::server::SOAP::makeLastBlockEndEvent(){
  // First check if a message has already been in the making.
  if ( messageReference_.isNull() ) return;
  // There will be no more data added to this message. (Apperently this event ended incomplete.)
  // Add it to the messages to be sent out.
  messages_.push_back( messageReference_ );
  messageReference_ = xoap::MessageReference(NULL);
  dataIsPendingTransmission_ = true;
}

void emu::daq::server::SOAP::createMessage()
  throw ( xoap::exception::Exception )
{
  try{
    xdata::String  serverName    ( parentApp_->getApplicationDescriptor()->getClassName() );
    xdata::Integer serverInstance( parentApp_->getApplicationDescriptor()->getInstance()  );
    xdata::Integer runNumber     ( runNumber_                                             );
    xdata::Integer runStartUTC   ( runStartUTC_                                           );
    xdata::Integer errorFlag     ( errorFlag_                                             );
    
    messageReference_ = emu::soap::createMessage( "onEmuDataMessage", 
						  emu::soap::Parameters()
						  .add( "serverName    "   , &serverName         )
						  .add( "serverInstance"   , &serverInstance     )
						  .add( "runNumber"        , &runNumber          )
						  .add( "runStartUTC"      , &runStartUTC        )
						  .add( "errorFlag"        , &errorFlag          )
						  .add( "nEventCreditsHeld",  nEventCreditsHeld_ ) );
  }
  catch( xcept::Exception &e ){
    XCEPT_RETHROW( xoap::exception::Exception , "Failed to create server reply SOAP message: ", e );
  }
  catch( ... ){
    XCEPT_RAISE( xoap::exception::Exception , "Failed to create server reply SOAP message: Unknown exception." );
  }
}

void emu::daq::server::SOAP::fillMessage( char* const  data,
					  const size_t dataLength )
  throw( xoap::exception::Exception )
{
  try{
    std::vector<emu::soap::Attachment> attachments;
    attachments.push_back( emu::soap::Attachment( dataLength, data ).setContentType( "application/octet-stream" ).setContentEncoding( "binary" ) );
    emu::soap::addAttachments( messageReference_, attachments );
  }
  catch( xcept::Exception &e ){
    XCEPT_RETHROW( xoap::exception::Exception , "Failed to add attachment to server reply SOAP message: ", e );
  }
  catch( ... ){
    XCEPT_RAISE( xoap::exception::Exception , "Failed to add attachment to server reply SOAP message: Unknown exception." );
  }
}


xoap::MessageReference emu::daq::server::SOAP::getOldestMessagePendingTransmission(){

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

void emu::daq::server::SOAP::sendData()
  throw ( xcept::Exception )
{

  if ( messages_.empty() ){
    //     LOG4CPLUS_DEBUG(logger_, name_ << " SOAP server has nothing to send!" );
    return;
  }

  LOG4CPLUS_DEBUG(logger_, name_ << " has " << messages_.size() <<
		  " messages, holding " << *nEventCreditsHeld_ <<
		  " credits. Prescaling: 1/" << *prescaling_ << " (emu::daq::server::SOAP::sendData)");

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

	xoap::MessageReference reply = appContext_->postSOAP(msg, *appDescriptor_, *clientDescriptor_);

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

}

void emu::daq::server::SOAP::printMessages(){
  stringstream allMessages;
  allMessages << messages_.size() << " messages to be sent from " << name_ << " server" << endl;
  //   for ( unsigned int i=0; i<messages_.size(); ++i ){
  //     string s;
  //     messages_[i]->writeTo(s);
  //     allMessages << "Message " << i << endl << s << endl;
  //   }
  LOG4CPLUS_DEBUG(logger_, allMessages.str() );
}
