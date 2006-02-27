#ifndef _EmuServer_h_
#define _EmuServer_h_

#include <string>
#include "log4cplus/logger.h"
#include "i2o/include/i2o/Method.h"
#include "xcept/include/xcept/tools.h"
#include "toolbox/mem/CommittedHeapAllocator.h"
#include "emu/emuDAQ/emuClient/include/i2oEmuClientMsg.h"

#include "extern/i2o/include/i2o/i2oDdmLib.h"
#include "xdaq/include/xdaq/ApplicationGroup.h"
#include "xdaq/include/xdaq/WebApplication.h"
#include "xdata/include/xdata/InfoSpace.h"
#include "xdata/include/xdata/String.h"
#include "xdata/include/xdata/UnsignedLong.h"
#include "xdata/include/xdata/Boolean.h"
#include "toolbox/include/toolbox/mem/MemoryPoolFactory.h"
#include "i2o/utils/include/i2o/utils/AddressMap.h"



// #include "log4cplus/logger.h"
// #include "i2o/Method.h"
// #include "xcept/tools.h"
// #include "toolbox/mem/CommittedHeapAllocator.h"
// #include "i2o/i2oDdmLib.h"
// #include "xdaq/ApplicationGroup.h"
// #include "xdaq/WebApplication.h"
// #include "xdata/InfoSpace.h"
// #include "xdata/String.h"
// #include "xdata/UnsignedLong.h"
// #include "xdata/Boolean.h"
// #include "toolbox/mem/MemoryPoolFactory.h"
// #include "i2o/utils/AddressMap.h"

class EmuServer {
private:

  xdaq::Application                          *parentApp_;
  toolbox::exception::HandlerSignature       *i2oExceptionHandler_;

  string                                      name_;
  xdaq::ApplicationDescriptor                *appDescriptor_;
  xdaq::ApplicationContext                   *appContext_;
  xdaq::ApplicationGroup                     *appGroup_;

  string                                      clientName_;
  xdaq::ApplicationDescriptor                *clientDescriptor_;
  I2O_TID                                     clientTid_;


  xdata::UnsignedLong                        *prescaling_;
  xdata::Boolean                             *sendDataOnRequestOnly_;// if true, data is sent only upon request
  xdata::UnsignedLong                        *nEventCreditsHeld_;       // number of credits (from client) yet to be sent
  bool                                        blocksArePendingTransmission_;

  string                                      poolName_;
  toolbox::mem::CommittedHeapAllocator       *allocator_;
  toolbox::mem::MemoryPoolFactory            *poolFactory_;
  toolbox::mem::Pool                         *pool_;
  xdata::UnsignedLong                        *poolSize_;             // committed memory pool size for data sent to client

  Logger                                      logger_;

  int                                         runNumber_;
  deque<pair<toolbox::mem::Reference*,bool> > dataBlocks_; // each block carries one DDU's or DCC's one event

  string createName();
  void   getClientDescriptor();
  void   getClientTid();
  void   createCommittedHeapAllocatorMemoryPool()
    throw ( xcept::Exception );
  void   appendNewBlock( char*               data, 
			 const unsigned long dataLength, 
			 const bool          completesEvent )
    throw ( xcept::Exception );
  void   fillBlock( toolbox::mem::Reference *bufRef,
		    char*                    data,
		    const unsigned int       dataLength )
    throw ( xcept::Exception );

public:
  // clientName must be passed by value to make sure it remains unchanged here
  // even after the it is modified in the parent app's info space
  EmuServer( xdaq::Application                    *parentApp,
	     toolbox::exception::HandlerSignature *i2oExceptionHandler,
	     const string                          clientName,
	     xdata::Serializable                  *poolSize,
	     xdata::Serializable                  *prescaling,
	     xdata::Serializable                  *onRequest,
	     xdata::Serializable                  *creditsHeld,
	     const Logger                         *logger )
    throw( xcept::Exception );
  ~EmuServer();
  string getClientName(){ return clientName_; }
  void   addCredits( const int nCredits, const int prescaling );
  void   addData(  const int  runNumber, 
		   const int  nEvents, 
		   const bool completesEvent, 
		   char* const data, 
		   const int  dataLength );
  void   sendData()
    throw ( xcept::Exception );
  bool   blocksArePendingTransmission(){ return blocksArePendingTransmission_; }
};

// EmuServer::EmuServer( xdaq::Application                    *parentApp,
// 		      toolbox::exception::HandlerSignature *i2oExceptionHandler,
// 		      const string                          clientName,
// 		      xdata::Serializable                  *poolSize,
// 		      xdata::Serializable                  *prescaling,
// 		      xdata::Serializable                  *onRequest,
// 		      xdata::Serializable                  *creditsHeld,
// 		      const Logger                         *logger )
//   throw( xcept::Exception )
//   :parentApp_                    (  parentApp ),
//    i2oExceptionHandler_          (  i2oExceptionHandler ),
//    clientName_                   (  clientName ),
//    prescaling_                   (  dynamic_cast<xdata::UnsignedLong*>( prescaling  ) ),
//    sendDataOnRequestOnly_        (  dynamic_cast<xdata::Boolean*>     ( onRequest   ) ),
//    nEventCreditsHeld_            (  dynamic_cast<xdata::UnsignedLong*>( creditsHeld ) ),
//    poolSize_                     (  dynamic_cast<xdata::UnsignedLong*>( poolSize    ) ),
//    logger_                       ( *logger )
// {
//   name_            = createName();
//   appDescriptor_   = parentApp->getApplicationDescriptor();
//   appContext_      = parentApp->getApplicationContext();
//   appGroup_        = appContext_->getApplicationGroup();
//   poolFactory_     = toolbox::mem::getMemoryPoolFactory();
//   poolName_        = name_ + "_pool";
  
//   createCommittedHeapAllocatorMemoryPool();
//   getClientTid();

//   blocksArePendingTransmission_ = false;

//   LOG4CPLUS_INFO(logger_, name_ << " server has been created" );
// }

// EmuServer::~EmuServer(){
//   toolbox::net::URN urn("toolbox-mem-pool", poolName_);
//   if ( pool_ && poolFactory_ ) poolFactory_->destroyPool( urn );
//   delete allocator_;
// }

// string EmuServer::createName(){
//   ostringstream oss;
  
//   oss << parentApp_->getApplicationDescriptor()->getClassName()
//       << parentApp_->getApplicationDescriptor()->getInstance()
//       << "-to-"
//       << clientName_;

//   return oss.str();
// }

// void EmuServer::createCommittedHeapAllocatorMemoryPool()
//   throw ( xcept::Exception )
// {

//   allocator_ = NULL;
//   pool_      = NULL;

//     try
//     {
//         toolbox::net::URN urn("toolbox-mem-pool", poolName_);
//         allocator_ = new toolbox::mem::CommittedHeapAllocator(poolSize_->value_);
//         pool_ = poolFactory_->createPool(urn, allocator_);
// 	pool_->setHighThreshold ( (unsigned long) (*poolSize_ * 0.7));
// 	LOG4CPLUS_INFO(logger_, 
// 		       name_ << " server created " << 
// 		       poolName_ << " of " << 
// 		       *poolSize_ << " bytes. High threshold at " << 
// 		       (unsigned long) (*poolSize_ * 0.7));
//     }
//     catch (xcept::Exception e)
//     {
//         string s = "Failed to set up commited pool: " + poolName_;

//         XCEPT_RETHROW(xcept::Exception, s, e);
//     }
//     catch(...)
//     {
//         string s = "Failed to set up commited pool: " + poolName_ +
//                    " : Unknown exception";

//         XCEPT_RAISE(xcept::Exception, s);
//     }
// }

// void EmuServer::getClientDescriptor()
// {
//   try
//     {
//       clientDescriptor_ =
// 	appGroup_->getApplicationDescriptor( clientName_, 0 );
//     }
//   catch(xcept::Exception e)
//     {
//       clientDescriptor_ = 0;
//       LOG4CPLUS_WARN(logger_, name_ << " server could not find its client " << clientName_ );
//     }
// }

// void EmuServer::getClientTid(){

//   getClientDescriptor();

//   i2o::utils::AddressMap *i2oAddressMap  = i2o::utils::getAddressMap();

//   if ( clientDescriptor_ ){
//     LOG4CPLUS_INFO(logger_, name_ << " server found its client.");
//     try
//       {
// 	clientTid_ = i2oAddressMap->getTid( clientDescriptor_ );
//       }
//     catch(xcept::Exception e)
//       {
// 	LOG4CPLUS_WARN(logger_, name_ << " server failed to get clients's Tid.");
//       }
//   }
// }

// void EmuServer::addCredits( const int nCredits, const int prescaling ){ 
//   nEventCreditsHeld_->value_ += nCredits;
//   *prescaling_                = prescaling;
//   LOG4CPLUS_INFO(logger_, 
// 		 name_    << " server received credits for " << 
// 		 nCredits << " events prescaled by 1/" << 
// 		 prescaling << ". Now holding " << 
// 		 *nEventCreditsHeld_ );
// }

// void EmuServer::addData( const int  runNumber, 
// 			 const int  nEvents, 
// 			 const bool completesEvent, 
// 			 char*      data, 
// 			 const int  dataLength ){
//     LOG4CPLUS_DEBUG(logger_, poolName_ << 
// 		    ": " << pool_->getMemoryUsage().getUsed() <<
// 		    " bytes ( " << 100*(float)(pool_->getMemoryUsage().getUsed())/(float)(pool_->getMemoryUsage().getCommitted()) << 
// 		    " % ) Event " << nEvents <<
// 		    ", holding " << *nEventCreditsHeld_ <<
// 		    " credits. Prescaling: 1/" << *prescaling_ );

//     bool isToBeSent = false;
//     if ( sendDataOnRequestOnly_ ){
//       if ( nEventCreditsHeld_->value_ && prescaling_->value_ ){
// 	if ( nEvents % prescaling_->value_ == 1 ){
// 	  if( !pool_->isHighThresholdExceeded() )
// 	    isToBeSent = true;
// 	  else  LOG4CPLUS_WARN(logger_, name_ << " memory pool's high threshold exceeded.");
// 	}
//       }
//     }
//     else if ( prescaling_->value_ ){
//       if ( nEvents % prescaling_->value_ == 1 ){
// 	if( !pool_->isHighThresholdExceeded() )
// 	  isToBeSent = true;
// 	else LOG4CPLUS_WARN(logger_, name_ << " memory pool's high threshold exceeded.");
//       }
//     }

//     if ( isToBeSent ) 
//       try {
// 	runNumber_ = runNumber;
// 	appendNewBlock( data, dataLength, completesEvent );
//       }
//       catch(xcept::Exception e){
// 	LOG4CPLUS_ERROR(logger_,
// 			"Failed to append data to be sent to " << clientName_
// 			<< " : " << stdformat_exception_history(e));
//       }

//     blocksArePendingTransmission_ = ( dataBlocks_.size() > 0 && completesEvent );

// }

// void EmuServer::appendNewBlock( char* const         data, 
// 				const unsigned long dataLength, 
// 				const bool          completesEvent )
//   throw ( xcept::Exception )
// {
//     toolbox::mem::Reference *bufRef = 0;

//     unsigned long dataBufSize = sizeof(I2O_EMU_DATA_MESSAGE_FRAME) 
//                                 + dataLength;

//     // Get a free block from the pool
//     try
//       {
//         bufRef = poolFactory_->getFrame( pool_, dataBufSize );
//       }
//     catch(xcept::Exception e)
//     {
//         stringstream oss;
//         string       s;

//         oss << "Failed to allocate a data block from the ";
//         oss << poolName_ << " pool";
//         oss << " : " << stdformat_exception_history(e);
//         s = oss.str();

//         XCEPT_RETHROW(xcept::Exception, s, e);
//     }


//     // Fill block
//     try
//     {
//          fillBlock( bufRef, data, dataLength );
//     }
//     catch(xcept::Exception e)
//     {
//         stringstream oss;
//         string       s;

//         oss << "Failed to fill data block for " << clientName_;
//         oss << " : " << stdformat_exception_history(e);
//         s = oss.str();

//         LOG4CPLUS_ERROR(logger_, s);
//     }

//     // Append block to super-fragment under construction
//     dataBlocks_.push_back( pair< toolbox::mem::Reference*, bool >( bufRef, completesEvent ) );
    
// }

// void EmuServer::fillBlock( toolbox::mem::Reference *bufRef,
// 			   char* const              data,
// 			   const unsigned int       dataLength )
//   throw ( xcept::Exception )
// {
//     char         *blockAddr        = 0;
//     char         *fedAddr          = 0;
//     unsigned int i2oMessageSize    = 0;

//     ////////////////////////////////////////////////
//     // Calculate addresses of block, and FED data //
//     ////////////////////////////////////////////////

//     blockAddr = (char*)bufRef->getDataLocation();
//     fedAddr   = blockAddr + sizeof(I2O_EMU_DATA_MESSAGE_FRAME);


//     ///////////////////////////////////////////////
//     // Set the data size of the buffer reference //
//     ///////////////////////////////////////////////

//     // I2O message size in bytes
//     i2oMessageSize = sizeof(I2O_EMU_DATA_MESSAGE_FRAME) + dataLength;
//     bufRef->setDataSize(i2oMessageSize);

//     ///////////////////////////
//     // Fill block with zeros //
//     ///////////////////////////

//     ::memset(blockAddr, 0, i2oMessageSize );
  
//     /////////////////////
//     // Fill FED data   //
//     /////////////////////

//     ::memcpy( fedAddr, data, dataLength );


//     /////////////////////////////
//     // Fill message headers    //
//     /////////////////////////////

//     I2O_MESSAGE_FRAME                  *stdMsg        = 0;
//     I2O_PRIVATE_MESSAGE_FRAME          *pvtMsg        = 0;
//     I2O_EMU_DATA_MESSAGE_FRAME         *block         = 0;

//     stdMsg    = (I2O_MESSAGE_FRAME*)blockAddr;
//     pvtMsg    = (I2O_PRIVATE_MESSAGE_FRAME*)blockAddr;
//     block     = (I2O_EMU_DATA_MESSAGE_FRAME*)blockAddr;

//     stdMsg->MessageSize    = i2oMessageSize >> 2;
//     stdMsg->TargetAddress  = clientTid_;
//     stdMsg->Function       = I2O_PRIVATE_MESSAGE;
//     stdMsg->VersionOffset  = 0;
//     stdMsg->MsgFlags       = 0;  // Point-to-point

//     pvtMsg->XFunctionCode  = I2O_EMUCLIENT_CODE;
//     pvtMsg->OrganizationID = XDAQ_ORGANIZATION_ID;

//     block->runNumber         = runNumber_;
//     block->nEventCreditsHeld = *nEventCreditsHeld_-1;

// }

// void EmuServer::sendData()
//   throw ( xcept::Exception )
// {
//   while ( blocksArePendingTransmission_ ){
//     try
//     {
//         appContext_->postFrame
//         (
//             dataBlocks_.front().first,
//             appDescriptor_,
//             clientDescriptor_,
//             i2oExceptionHandler_,
//             clientDescriptor_
//         );
//     }
//     catch(xcept::Exception e)
//     {
//         stringstream oss;
//         oss << name_ << " failed to send block";

//         XCEPT_RETHROW(xcept::Exception, oss.str(), e);
//     }


//     if ( dataBlocks_.front().second && nEventCreditsHeld_->value_ ) --(nEventCreditsHeld_->value_);

//     dataBlocks_.erase(dataBlocks_.begin());

//     blocksArePendingTransmission_ = dataBlocks_.size() > 0;
//   }

// }

#endif
