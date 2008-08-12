#include "EmuI2OServer.h"
#include <sstream>

EmuI2OServer::EmuI2OServer( xdaq::Application                    *parentApp,
			    toolbox::exception::HandlerSignature *i2oExceptionHandler,
			    const string                          clientName,
			    const unsigned int                    clientInstance,
			    xdata::Serializable                  *poolSize,
			    xdata::Serializable                  *prescaling,
			    xdata::Serializable                  *onRequest,
			    xdata::Serializable                  *creditsHeld,
			    const Logger                         *logger )
  throw( xcept::Exception )
  : EmuServer           ( parentApp,
			  clientName,
			  clientInstance,
			  prescaling,
			  onRequest,
			  creditsHeld,
			  logger       ),
    i2oExceptionHandler_( i2oExceptionHandler ),
    poolSize_           (  dynamic_cast<xdata::UnsignedLong*>( poolSize ) )
{
  tid_ = i2o::utils::getAddressMap()->getTid(appDescriptor_);
  findClientTid();

  poolFactory_     = toolbox::mem::getMemoryPoolFactory();
  poolName_        = name_ + "_pool";
  
  createCommittedHeapAllocatorMemoryPool();

  dataBlocks_.clear();
  nEventsInQueue = 0;

  LOG4CPLUS_INFO(logger_, name_ << " server has been created" );
}

EmuI2OServer::~EmuI2OServer(){
  toolbox::net::URN urn("toolbox-mem-pool", poolName_);
  if ( pool_ && poolFactory_ ) poolFactory_->destroyPool( urn );
  delete allocator_;
  dataBlocks_.clear();
}



void EmuI2OServer::findClientTid(){

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

void EmuI2OServer::createCommittedHeapAllocatorMemoryPool()
  throw ( xcept::Exception )
{

  allocator_ = NULL;
  pool_      = NULL;

    try
    {
        toolbox::net::URN urn("toolbox-mem-pool", poolName_);
        allocator_ = new toolbox::mem::CommittedHeapAllocator(poolSize_->value_);
        pool_ = poolFactory_->createPool(urn, allocator_);
	pool_->setHighThreshold ( (unsigned long) (*poolSize_ * 0.7));
	LOG4CPLUS_INFO(logger_, 
		       name_ << " server created " << 
		       poolName_ << " of " << 
		       *poolSize_ << " bytes. High threshold at " << 
		       (unsigned long) (*poolSize_ * 0.7));
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

void EmuI2OServer::addData( const int            runNumber, 
			    const int            runStartUTC,
			    const int            nEvents, 
			    const bool           completesEvent, 
			    const unsigned short errorFlag, 
			    char*                data, 
			    const int            dataLength ){
    LOG4CPLUS_DEBUG(logger_, poolName_ << 
		    ": " << pool_->getMemoryUsage().getUsed() <<
		    " bytes ( " << 100*(float)(pool_->getMemoryUsage().getUsed())/(float)(pool_->getMemoryUsage().getCommitted()) << 
		    " % ) Event " << nEvents <<
		    ". Now " << nEventsInQueue <<
		    " events in queue. Holding " << *nEventCreditsHeld_ <<
		    " credits. Prescaling: 1/" << *prescaling_ );

    bool isToBeSent = false;
    if ( sendDataOnRequestOnly_->value_ ){
      if ( nEventCreditsHeld_->value_                  && 
	   nEventCreditsHeld_->value_ > nEventsInQueue && 
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
	runNumber_   = runNumber;
	runStartUTC_ = runStartUTC;
	errorFlag_   = errorFlag;
	appendNewBlock( data, dataLength, completesEvent );
      }
      catch(xcept::Exception e){
	LOG4CPLUS_ERROR(logger_,
			"Failed to append data to be sent to " << clientName_
			<< " : " << stdformat_exception_history(e));
      }
    }

    dataIsPendingTransmission_ = ( dataBlocks_.size() > 0 && completesEvent );

//     LOG4CPLUS_DEBUG(logger_, "EmuI2OServer::addData: " << dataBlocks_.size() << " data blocks pending.");

}

void EmuI2OServer::appendNewBlock( char* const         data, 
				   const unsigned long dataLength, 
				   const bool          completesEvent )
  throw ( xcept::Exception )
{
    toolbox::mem::Reference *bufRef = 0;

    unsigned long dataBufSize = sizeof(I2O_EMU_DATA_MESSAGE_FRAME) 
                                + dataLength;

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
         fillBlock( bufRef, data, dataLength );
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
    dataBlocks_.push_back( pair< toolbox::mem::Reference*, bool >( bufRef, completesEvent ) );

    if ( completesEvent ) ++nEventsInQueue;
    
}

void EmuI2OServer::fillBlock( toolbox::mem::Reference *bufRef,
			      char* const              data,
			      const unsigned int       dataLength )
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

    block->runNumber         = runNumber_;
    block->runStartUTC       = runStartUTC_;
    block->nEventCreditsHeld = *nEventCreditsHeld_-1; // will be updated on sending, but anyway
    block->errorFlag         = errorFlag_;

}

void EmuI2OServer::insertNumberOfCreditsHeld( toolbox::mem::Reference *bufRef, bool completesEvent ){
  // Make sure the message contains the current number of credits held.
  I2O_EMU_DATA_MESSAGE_FRAME *emuDataMsgFrame = (I2O_EMU_DATA_MESSAGE_FRAME*) bufRef->getDataLocation();
  if ( completesEvent )
    // one less because with this a complete event will have been sent over
    emuDataMsgFrame->nEventCreditsHeld = (nEventCreditsHeld_->value_>0 ? *nEventCreditsHeld_ - 1 : 0);
  else
    emuDataMsgFrame->nEventCreditsHeld = *nEventCreditsHeld_;
}

void EmuI2OServer::sendData()
  throw ( xcept::Exception )
{
  while ( dataIsPendingTransmission_ ){
    try
    {
      // Update message with the current number of credits held...
      insertNumberOfCreditsHeld( dataBlocks_.front().first,  dataBlocks_.front().second );
      // ...and send it.
      appContext_->postFrame
        (
	 dataBlocks_.front().first,
	 appDescriptor_,
	 clientDescriptor_,
	 i2oExceptionHandler_,
	 clientDescriptor_
	 );
    }
    catch(xcept::Exception e)
    {
        stringstream oss;
        oss << name_ << " failed to send block";

        XCEPT_RETHROW(xcept::Exception, oss.str(), e);
    }

    if ( dataBlocks_.front().second ) {
      if ( nEventsInQueue > 0 ) --nEventsInQueue;
      else LOG4CPLUS_DEBUG(logger_, "***** Sent data from queue containing 0 events?!");
    }

    if ( dataBlocks_.front().second && nEventCreditsHeld_->value_ ) --(nEventCreditsHeld_->value_);

    dataBlocks_.erase(dataBlocks_.begin());

    dataIsPendingTransmission_ = dataBlocks_.size() > 0;

//     LOG4CPLUS_DEBUG(logger_, "EmuI2OServer::sendData: " << dataBlocks_.size() << " data blocks pending.");
  }

}

void EmuI2OServer::makeLastBlockCompleteEvent(){
  // First check if an event has already been in the making.
  if ( dataBlocks_.size() == 0 ) return;
  // There will be no more data added to this event. (Apperently this event ended incomplete.)
  // Mark the last block we have as last of event.
  deque<pair<toolbox::mem::Reference*,bool> >::reverse_iterator lastBlock = dataBlocks_.rbegin();
  lastBlock->second = true;
  ++nEventsInQueue;
  dataIsPendingTransmission_ = dataBlocks_.size() > 0;
}
