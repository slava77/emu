#include "emu/daq/server/I2O.h"
#include "emu/daq/reader/Spy.h"
#include <sstream>

emu::daq::server::I2O::I2O( xdaq::Application                    *parentApp,
			    toolbox::exception::HandlerSignature *i2oExceptionHandler,
			    const string                          clientName,
			    const unsigned int                    clientInstance,
			    xdata::Serializable                  *poolSize,
			    xdata::Serializable                  *prescaling,
			    xdata::Serializable                  *onRequest,
			    xdata::Serializable                  *creditsHeld,
			    const Logger                         *logger )
  throw( xcept::Exception )
  : emu::daq::server::Base ( parentApp,
			     clientName,
			     clientInstance,
			     prescaling,
			     onRequest,
			     creditsHeld,
			     logger       ),
    i2oExceptionHandler_( i2oExceptionHandler ),
    poolSize_           (  dynamic_cast<xdata::UnsignedInteger64*>( poolSize ) )
{
  tid_ = i2o::utils::getAddressMap()->getTid(appDescriptor_);
  findClientTid();

  poolFactory_     = toolbox::mem::getMemoryPoolFactory();
  poolName_        = name_ + "_pool";
  
  createCommittedHeapAllocatorMemoryPool();

  messageQueue_.clear();
  nEventsInQueue_ = 0;

  LOG4CPLUS_INFO(logger_, name_ << " server has been created" );
}

emu::daq::server::I2O::~I2O(){
  toolbox::net::URN urn("toolbox-mem-pool", poolName_);
  if ( pool_ && poolFactory_ ) poolFactory_->destroyPool( urn );
  delete allocator_;
  messageQueue_.clear();
}



void emu::daq::server::I2O::findClientTid(){

  findClientDescriptor();

  i2o::utils::AddressMap *i2oAddressMap  = i2o::utils::getAddressMap();

  if ( clientDescriptor_ ){
    LOG4CPLUS_INFO(logger_, name_ << " server found its client.");
    try
      {
	clientTid_ = i2oAddressMap->getTid( clientDescriptor_ );
      }
    catch(xcept::Exception e)
      {
	LOG4CPLUS_WARN(logger_, name_ << " server failed to get clients's Tid.");
      }
  }
}

void emu::daq::server::I2O::createCommittedHeapAllocatorMemoryPool()
  throw ( xcept::Exception )
{

  allocator_ = NULL;
  pool_      = NULL;

    try
    {
        toolbox::net::URN urn("toolbox-mem-pool", poolName_);
        allocator_ = new toolbox::mem::CommittedHeapAllocator(poolSize_->value_);
        pool_ = poolFactory_->createPool(urn, allocator_);
	pool_->setHighThreshold ( (size_t) (*poolSize_ * 0.7));
	LOG4CPLUS_INFO(logger_, 
		       name_ << " server created " << 
		       poolName_ << " of " << 
		       *poolSize_ << " bytes. High threshold at " << 
		       (size_t) (*poolSize_ * 0.7));
    }
    catch (xcept::Exception e)
    {
        string s = "Failed to set up commited pool: " + poolName_;

        XCEPT_RETHROW(xcept::Exception, s, e);
    }
    catch(...)
    {
        string s = "Failed to set up commited pool: " + poolName_ +
                   " : Unknown exception";

        XCEPT_RAISE(xcept::Exception, s);
    }
}

void emu::daq::server::I2O::addData( const int               runNumber, 
				     const int               runStartUTC,
				     const uint64_t          nEvents, 
				     const PositionInEvent_t position, 
				     const uint16_t          errorFlag, 
				     char*                   data, 
				     const size_t            dataLength ){
    LOG4CPLUS_DEBUG(logger_, poolName_ << 
		    ": " << pool_->getMemoryUsage().getUsed() <<
		    " bytes ( " << 100*(float)(pool_->getMemoryUsage().getUsed())/(float)(pool_->getMemoryUsage().getCommitted()) << 
		    " % ) Event " << nEvents <<
		    ". Now " << nEventsInQueue_ <<
		    " events in queue. Holding " << *nEventCreditsHeld_ <<
		    " credits. Prescaling: 1/" << *prescaling_ );

    bool isToBeSent = false;
    if ( sendDataOnRequestOnly_->value_ ){
      if ( nEventCreditsHeld_->value_                  && 
	   nEventCreditsHeld_->value_ > nEventsInQueue_ && 
	   prescaling_->value_                            ){
	if ( nEvents % prescaling_->value_ == 0 ){
	  if( !pool_->isHighThresholdExceeded() )
	    isToBeSent = true;
// 	  else  LOG4CPLUS_WARN(logger_, name_ << " memory pool's high threshold exceeded.");
	}
      }
    }
    else if ( prescaling_->value_ ){
      if ( nEvents % prescaling_->value_ == 0 ){
	if( !pool_->isHighThresholdExceeded() )
	  isToBeSent = true;
// 	else LOG4CPLUS_WARN(logger_, name_ << " memory pool's high threshold exceeded.");
      }
    }

    if ( isToBeSent ){
      LOG4CPLUS_DEBUG(logger_, 
		      "Adding " << dataLength <<
		      " bytes of data of event " << nEvents <<
		      " to be sent." );
      try {
	toolbox::mem::Reference *bufRef = NULL;
	createMessage( bufRef, runNumber, runStartUTC, position, errorFlag, data, dataLength );
	// Append this message to the queue:
	if ( bufRef != NULL ) messageQueue_.push_back( pair< toolbox::mem::Reference*, emu::daq::server::PositionInEvent_t >( bufRef, position ) );
      }
      catch(xcept::Exception e){
	LOG4CPLUS_ERROR(logger_,
			"Failed to append data to be sent to " << clientName_
			<< " : " << stdformat_exception_history(e));
      }
    }

//     LOG4CPLUS_DEBUG(logger_, "emu::daq::server::I2O::addData: " << messageQueue_.size() << " data blocks pending.");

}


void emu::daq::server::I2O::createMessage( toolbox::mem::Reference *bufRef,
					   const int               runNumber, 
					   const int               runStartUTC,
					   const PositionInEvent_t position, 
					   const uint16_t          errorFlag, 
					   char* const             data, 
					   const size_t            dataLength )
  throw ( xcept::Exception )
{
    bufRef = 0;

    size_t dataBufSize = sizeof(I2O_EMU_DATA_MESSAGE_FRAME) + dataLength;

    // Get a free block from the pool
    try
      {
        bufRef = poolFactory_->getFrame( pool_, dataBufSize );
      }
    catch(xcept::Exception e)
    {
        stringstream oss;
        string       s;

        oss << "Failed to allocate a data block from the ";
        oss << poolName_ << " pool";
        oss << " : " << stdformat_exception_history(e);
        s = oss.str();

        XCEPT_RETHROW(xcept::Exception, s, e);
    }


    // Fill block
    try
    {
         fillBlock( bufRef, runNumber, runStartUTC, position, errorFlag, data, dataLength );
    }
    catch(xcept::Exception e)
    {
        stringstream oss;
        string       s;

        oss << "Failed to fill data block for " << clientName_;
        oss << " : " << stdformat_exception_history(e);
        s = oss.str();

        LOG4CPLUS_ERROR(logger_, s);
    }

    // Append block to super-fragment under construction
    messageQueue_.push_back( pair< toolbox::mem::Reference*, emu::daq::server::PositionInEvent_t >( bufRef, position ) );

    if ( position & emu::daq::server::endsEvent ) ++nEventsInQueue_;
    
}

void emu::daq::server::I2O::fillBlock( toolbox::mem::Reference *bufRef,
				       const int               runNumber, 
				       const int               runStartUTC,
				       const PositionInEvent_t position, 
				       const uint16_t          errorFlag, 
				       char* const             data, 
				       const size_t            dataLength )
  throw ( xcept::Exception )
{
    char         *blockAddr        = 0;
    char         *fedAddr          = 0;
    unsigned int i2oMessageSize    = 0;

    ////////////////////////////////////////////////
    // Calculate addresses of block, and FED data //
    ////////////////////////////////////////////////

    blockAddr = (char*)bufRef->getDataLocation();
    fedAddr   = blockAddr + sizeof(I2O_EMU_DATA_MESSAGE_FRAME);


    ///////////////////////////////////////////////
    // Set the data size of the buffer reference //
    ///////////////////////////////////////////////

    // I2O message size in bytes
    i2oMessageSize = sizeof(I2O_EMU_DATA_MESSAGE_FRAME) + dataLength;
    bufRef->setDataSize(i2oMessageSize);

    ///////////////////////////
    // Fill block with zeros //
    ///////////////////////////

    ::memset(blockAddr, 0, i2oMessageSize );
  
    /////////////////////
    // Fill FED data   //
    /////////////////////

    ::memcpy( fedAddr, data, dataLength );


    /////////////////////////////
    // Fill message headers    //
    /////////////////////////////

    I2O_MESSAGE_FRAME                  *stdMsg        = 0;
    I2O_PRIVATE_MESSAGE_FRAME          *pvtMsg        = 0;
    I2O_EMU_DATA_MESSAGE_FRAME         *block         = 0;

    stdMsg    = (I2O_MESSAGE_FRAME*)blockAddr;
    pvtMsg    = (I2O_PRIVATE_MESSAGE_FRAME*)blockAddr;
    block     = (I2O_EMU_DATA_MESSAGE_FRAME*)blockAddr;

    stdMsg->MessageSize      = i2oMessageSize >> 2;
    stdMsg->InitiatorAddress = tid_;
    stdMsg->TargetAddress    = clientTid_;
    stdMsg->Function         = I2O_PRIVATE_MESSAGE;
    stdMsg->VersionOffset    = 0;
    stdMsg->MsgFlags         = 0;  // Point-to-point

    pvtMsg->XFunctionCode  = I2O_EMUCLIENT_CODE;
    pvtMsg->OrganizationID = XDAQ_ORGANIZATION_ID;

    block->runNumber         = runNumber;
    block->runStartUTC       = runStartUTC;
    block->nEventCreditsHeld = *nEventCreditsHeld_-1; // will be updated on sending, but anyway
    block->errorFlag         = errorFlag;

}

void emu::daq::server::I2O::insertNumberOfCreditsHeld( toolbox::mem::Reference *bufRef, PositionInEvent_t position ){
  // Make sure the message contains the current number of credits held.
  I2O_EMU_DATA_MESSAGE_FRAME *emuDataMsgFrame = (I2O_EMU_DATA_MESSAGE_FRAME*) bufRef->getDataLocation();
  if ( position & emu::daq::server::endsEvent )
    // one less because with this a complete event will have been sent over
    emuDataMsgFrame->nEventCreditsHeld = (nEventCreditsHeld_->value_>0 ? *nEventCreditsHeld_ - 1 : 0);
  else
    emuDataMsgFrame->nEventCreditsHeld = *nEventCreditsHeld_;
}

void emu::daq::server::I2O::sendData()
  throw ( xcept::Exception )
{
  while ( isEventEndInQueue() ){
    if ( ( messageQueue_.front().second & emu::daq::server::startsEvent ) && 
	 ( messageQueue_.front().second & emu::daq::server::endsEvent   )    ){
      // A whole event appears to be included in the first message.
      sendNextMessageInQueue();
    }
    else{
      // Here we have a multi-block and/or incomplete event. Merge data and information into a single message:
      sendMergedEventMessage();
    }
    //     LOG4CPLUS_DEBUG(logger_, "emu::daq::server::I2O::sendData: " << messageQueue_.size() << " data blocks pending.");
  }

}

void emu::daq::server::I2O::makeLastBlockEndEvent(){
  // First check if an event has already been in the making.
  if ( messageQueue_.size() == 0 ) return;
  // There will be no more data added to this event. (Apperently this event ended incomplete.)
  // Mark the last block we have as the last of the event.
  deque<pair<toolbox::mem::Reference*,emu::daq::server::PositionInEvent_t> >::reverse_iterator lastBlock = messageQueue_.rbegin();
  lastBlock->second = (emu::daq::server::PositionInEvent_t)( lastBlock->second | emu::daq::server::endsEvent );
  ++nEventsInQueue_;
}

bool emu::daq::server::I2O::isEventEndInQueue(){
  // Returns TRUE if any data blocks with an "endsEvent" mark are still in the queue.
  for ( deque<pair<toolbox::mem::Reference*,emu::daq::server::PositionInEvent_t> >::iterator msg = messageQueue_.begin(); msg != messageQueue_.end(); ++msg ){
    if ( msg->second & emu::daq::server::endsEvent ) return true;
  }
  return false;
}

void emu::daq::server::I2O::sendNextMessageInQueue(){
  // Update message with the current number of credits held:
  insertNumberOfCreditsHeld( messageQueue_.front().first,  messageQueue_.front().second );
  // Send message:
  try{
    appContext_->postFrame( messageQueue_.front().first,
			    appDescriptor_,
			    clientDescriptor_,
			    i2oExceptionHandler_,
			    clientDescriptor_ );
  }
  catch(xcept::Exception& e){
    stringstream oss;
    oss << name_ << " failed to send block";
    XCEPT_RETHROW(xcept::Exception, oss.str(), e);
  }
  // Update counters:
  if ( nEventsInQueue_ > 0 ) --nEventsInQueue_;
  else LOG4CPLUS_DEBUG(logger_, "***** Sent data from queue containing 0 events?!");
  if ( nEventCreditsHeld_->value_ > 0 ) --(nEventCreditsHeld_->value_);
  // Remove message from queue:
  messageQueue_.erase( messageQueue_.begin() );
}

void emu::daq::server::I2O::sendMergedEventMessage(){

  //cout << "Combining error flags:" << endl << flush;

  // Find the data block that ends the event and combine the information up to that block:
  deque<pair<toolbox::mem::Reference*,emu::daq::server::PositionInEvent_t> >::iterator messagePastEventEnd = messageQueue_.begin();
  size_t totalDataSizeInBytes = 0;
  uint16_t combinedErrorFlag = 0x0000;
  uint16_t totalNumberOfPackets = 0;
  for ( deque<pair<toolbox::mem::Reference*,emu::daq::server::PositionInEvent_t> >::iterator msg = messageQueue_.begin(); msg != messageQueue_.end(); ++msg ){
    I2O_MESSAGE_FRAME*          stdMsgFrame = (I2O_MESSAGE_FRAME*)          msg->first->getDataLocation();
    I2O_EMU_DATA_MESSAGE_FRAME* emuMsgFrame = (I2O_EMU_DATA_MESSAGE_FRAME*) msg->first->getDataLocation();
    totalDataSizeInBytes += ( stdMsgFrame->MessageSize << 2 ) - sizeof(I2O_EMU_DATA_MESSAGE_FRAME);
    totalNumberOfPackets += ( emuMsgFrame->errorFlag & 0x0F00 ) >> 8; // The lower 4 bits of the upper byte contains the number of ethernet packets.
    // Combine from emu::daq::reader the error bits that are not specific to the start or end of event:
    combinedErrorFlag |= emuMsgFrame->errorFlag & ( emu::daq::reader::Spy::Timeout         |
						    emu::daq::reader::Spy::PacketsMissing  |
						    emu::daq::reader::Spy::LoopOverwrite   |
						    emu::daq::reader::Spy::BufferOverwrite |
						    emu::daq::reader::Spy::Oversized         );

    //cout << errorFlagToString( emuMsgFrame->errorFlag ) << endl << flush;

    if ( msg == messageQueue_.begin() ){
      // The first block. Does it have the DDU header?
      combinedErrorFlag |= emuMsgFrame->errorFlag & emu::daq::reader::Spy::HeaderMissing;
    }
    if ( msg->second & emu::daq::server::endsEvent ){
      // Marked as the block ending the event. Did the reader find the end of event? Does it have the DDU trailer?
      combinedErrorFlag |= emuMsgFrame->errorFlag & ( emu::daq::reader::Spy::EndOfEventMissing | 
						      emu::daq::reader::Spy::TrailerMissing      );
      // This one ends the event...
      messagePastEventEnd = msg;
      // ...but we want to remember the one past it:
      ++messagePastEventEnd;
      break;
    }
  }

  if ( messagePastEventEnd == messageQueue_.begin() ) return; // Apparently, no end of event in the queue. (Why did we get here, then?!)

  // Pack the total number of packets into the combined error flag:
  combinedErrorFlag |= ( ( totalNumberOfPackets > 15 ? 15 : totalNumberOfPackets ) << 8 ) & 0x0F00; // The lower 4 bits of the upper byte contains the number of ethernet packets.
  // Pack the number of merged messages into the combined error flag:
  int nMerged = messagePastEventEnd - messageQueue_.begin();
  combinedErrorFlag |= ( nMerged > 15 ? 15 : nMerged ) << 12; // The upper 4 bits of the upper byte contains the number of merged messages.

  //cout << errorFlagToString( combinedErrorFlag ) << " (combined)" << endl << flush;

  //
  // Get a free block from the pool
  //
  toolbox::mem::Reference* message = NULL;
  size_t i2oMessageSize = sizeof(I2O_EMU_DATA_MESSAGE_FRAME) + totalDataSizeInBytes;
  try{
    message = poolFactory_->getFrame( pool_, i2oMessageSize );
  }
  catch(xcept::Exception& e){
    stringstream oss;
    oss << "Failed to allocate a data block from the " << poolName_ << " pool" << " : " << stdformat_exception_history(e);
    XCEPT_RETHROW(xcept::Exception, oss.str(), e);
  }

  //
  // Fill message with combined data
  //
  message->setDataSize( i2oMessageSize ); // TODO: needed?
  // Zero the whole message:
  ::memset( (char*)message->getDataLocation(), 0, i2oMessageSize );
  // Fill the generic headers:
  I2O_MESSAGE_FRAME          *stdMessageFrame = (I2O_MESSAGE_FRAME*)         message->getDataLocation();
  I2O_PRIVATE_MESSAGE_FRAME  *pvtMessageFrame = (I2O_PRIVATE_MESSAGE_FRAME*) message->getDataLocation();
  I2O_EMU_DATA_MESSAGE_FRAME *emuMessageFrame = (I2O_EMU_DATA_MESSAGE_FRAME*)message->getDataLocation();

  stdMessageFrame->MessageSize      = i2oMessageSize >> 2;
  stdMessageFrame->InitiatorAddress = tid_;
  stdMessageFrame->TargetAddress    = clientTid_;
  stdMessageFrame->Function         = I2O_PRIVATE_MESSAGE;
  stdMessageFrame->VersionOffset    = 0;
  stdMessageFrame->MsgFlags         = 0;  // Point-to-point
  
  pvtMessageFrame->XFunctionCode  = I2O_EMUCLIENT_CODE;
  pvtMessageFrame->OrganizationID = XDAQ_ORGANIZATION_ID;
  
  // Loop over the messages that make up this event and copy the data from them:
  char* writePointer = (char*) message->getDataLocation() + sizeof(I2O_EMU_DATA_MESSAGE_FRAME);
  for ( deque<pair<toolbox::mem::Reference*,emu::daq::server::PositionInEvent_t> >::iterator msg = messageQueue_.begin(); msg != messagePastEventEnd; ++msg ){
    // Also fill the Emu-specific header with the info from the first block:
    if ( msg == messageQueue_.begin() ){
      I2O_EMU_DATA_MESSAGE_FRAME* emuMsgFrame = (I2O_EMU_DATA_MESSAGE_FRAME*) msg->first->getDataLocation();
      emuMessageFrame->runNumber         = emuMsgFrame->runNumber;
      emuMessageFrame->runStartUTC       = emuMsgFrame->runStartUTC;
      emuMessageFrame->nEventCreditsHeld = ( nEventCreditsHeld_->value_>0 ? *nEventCreditsHeld_ - 1 : 0 );
      emuMessageFrame->errorFlag         = combinedErrorFlag;
    }
    // Copy the data:
    I2O_MESSAGE_FRAME* stdMsgFrame = (I2O_MESSAGE_FRAME*) msg->first->getDataLocation();
    size_t dataSizeInBytes = ( stdMsgFrame->MessageSize << 2 ) - sizeof(I2O_EMU_DATA_MESSAGE_FRAME);
    char* readPointer = (char*) msg->first->getDataLocation() + sizeof(I2O_EMU_DATA_MESSAGE_FRAME);
    ::memcpy( writePointer, readPointer, dataSizeInBytes );
    writePointer += dataSizeInBytes;
    // Free the memory allocated to this message:
    msg->first->release();
  }
  
  // Remove these messages from the queue:
  messageQueue_.erase( messageQueue_.begin(), messagePastEventEnd );

  // Send the merged message:
  try{
    appContext_->postFrame( message,
			    appDescriptor_,
			    clientDescriptor_,
			    i2oExceptionHandler_,
			    clientDescriptor_ );
  }
  catch(xcept::Exception& e){
    stringstream oss;
    oss << name_ << " failed to send merged message";
    XCEPT_RETHROW(xcept::Exception, oss.str(), e);
  }
  // Update counters:
  if ( nEventsInQueue_            > 0 ) --nEventsInQueue_;
  if ( nEventCreditsHeld_->value_ > 0 ) --(nEventCreditsHeld_->value_);
}

string emu::daq::server::I2O::errorFlagToString( U16 errorFlag ){
  stringstream ss;
  ss << binaryFrom( errorFlag );
  return ss.str();
}
