#ifndef _emu_daq_server_I2O_h_
#define _emu_daq_server_I2O_h_

#include "i2o/Method.h"
#include "xcept/tools.h"
#include "toolbox/mem/CommittedHeapAllocator.h"
#include "toolbox/mem/MemoryPoolFactory.h"
#include "i2o/utils/AddressMap.h"
#include "xdata/UnsignedInteger64.h"

#include "emu/daq/server/i2oMsg.h"
#include "emu/daq/server/Base.h"

namespace emu{
  namespace daq{
    namespace server{

/// An \ref emu::daq::server transmitting data via I2O messages.
class I2O : public emu::daq::server::Base {
private:

  toolbox::exception::HandlerSignature       *i2oExceptionHandler_; ///< I2O exception handler

  string                                      poolName_; ///< name of server-to-client data memory pool
  toolbox::mem::CommittedHeapAllocator       *allocator_; ///< allocate fixed size memory
  toolbox::mem::MemoryPoolFactory            *poolFactory_; ///< memory pool factory
  toolbox::mem::Pool                         *pool_; ///< memory pool
  xdata::UnsignedInteger64                   *poolSize_; ///< committed memory pool size for data sent to client
  I2O_TID                                     clientTid_; ///< target id of client
  I2O_TID                                     tid_; ///< target id of this server

  /// queue of data waiting to be sent
  deque<pair<toolbox::mem::Reference*,PositionInEvent_t> > messageQueue_;

  /// number of \b events already in the queue of data waiting to be sent (=< \ref messageQueue_ .size())
  unsigned int                                nEventsInQueue_;

  /// Finds target id of client.
  void   findClientTid();

  /// Creates a fixed size memory pool on the heap.
  void   createCommittedHeapAllocatorMemoryPool()
    throw ( xcept::Exception );

  /// Creates a message and fills it with the data and metadata
  ///
  /// @param bufRef pointer to the memory containing the message
  /// @param runNumber run number
  /// @param runStartUTC start time (Unix epoch) of run
  /// @param position position of data inside the event
  /// @param errorFlag error flag
  /// @param data pointer to the data
  /// @param dataLength data size in bytes
  ///
  void createMessage( toolbox::mem::Reference *bufRef,
		      const int               runNumber, 
		      const int               runStartUTC,
		      const PositionInEvent_t position, 
		      const uint16_t          errorFlag, 
		      char* const             data, 
		      const size_t            dataLength )
    throw ( xcept::Exception );

  /// Fills block with data.

  /// @param bufRef reference to memory block block
  /// @param data pointer to data
  /// @param dataLength lengh of data in bytes
  ///
  void   fillBlock( toolbox::mem::Reference *bufRef,
		    const int               runNumber, 
		    const int               runStartUTC,
		    const PositionInEvent_t position, 
		    const uint16_t          errorFlag, 
		    char* const             data, 
		    const size_t            dataLength )
    throw ( xcept::Exception );

  /// Includes in the message the number of event credits still held.

  /// @param bufRef  reference to memory block block
  /// @param position position of data in the event (at start, middle, or end)
  ///
  void insertNumberOfCreditsHeld( toolbox::mem::Reference *bufRef, const PositionInEvent_t position );

  ///
  /// Checks if the message queue contains at least one end of event.
  ///
  /// @return \c TRUE if the message queue contains at least one end of event, \c FALSE otherwise
  ///
  bool isEventEndInQueue();

  /// 
  /// Sends the first (oldest) message in the queue, and then removes it.
  ///
  void sendNextMessageInQueue();

  /// 
  /// Find the data block that ends the event, merge the messages up to that block into one single message, remove them from the message queue, and send the merged message instead.
  ///
  void sendMergedEventMessage();

public:

  /// constructor

  /// @param parentApp parentApp parent application of this server
  /// @param i2oExceptionHandler I2O exception handler
  /// @param clientName client name (must be passed by value to make sure it remains unchanged here
  /// even after the it is modified in the parent app's info space)
  /// @param clientInstance instance of client of this server (must be passed by value to make sure 
  /// it remains unchanged here even after the it is modified in the parent app's info space)
  /// @param poolSize size of memory pool used in the data transmission to the client
  /// @param prescaling if prescaling is \e n, only every <em>n</em>th event will be sent
  /// @param onRequest  if \c TRUE , data is sent only if credits have been received
  /// @param creditsHeld number of events yet to be transmitted
  /// @param logger logger
  I2O( xdaq::Application                    *parentApp,
       toolbox::exception::HandlerSignature *i2oExceptionHandler,
       const string                          clientName,
       const unsigned int                    clientInstance,
       xdata::Serializable                  *poolSize,
       xdata::Serializable                  *prescaling,
       xdata::Serializable                  *onRequest,
       xdata::Serializable                  *creditsHeld,
       const Logger                         *logger )
    throw( xcept::Exception );

  /// destructor
  ~I2O();

  /// Adds data to the queue waiting to be transmitted to the client.

  /// @param runNumber run number
  /// @param nEvents number of events read/processed by the parent application
  /// @param position position of data in the event (at start, middle, or end)
  /// @param errorFlag error flag to be transmitted along with the data
  /// @param data data
  /// @param dataLength data length in bytes
  ///
  void   addData( const int               runNumber, 
		  const int               runStartUTC,
		  const uint64_t          nEvents, 
		  const PositionInEvent_t position, 
		  const uint16_t          errorFlag, 
		  char*                   data, 
		  const size_t            dataLength );

  /// Transmits data to client.
  void   sendData()
    throw ( xcept::Exception );

  /// Marks the last block as last of event.
  void makeLastBlockEndEvent();

  /// accessor of target id of client

  /// @return target id of client
  ///
  I2O_TID getClientTid(){ return clientTid_; }

  /// dummy in I2O server
  xoap::MessageReference getOldestMessagePendingTransmission(){ xoap::MessageReference m; m=NULL; return m; }


  string errorFlagToString( U16 errorFlag );


  template <typename T>
  std::string binaryFrom( const T& t )
  {
    std::stringstream ss;
    for ( size_t i = sizeof( T ); i > 0; --i ){
      if ( i < sizeof( T ) ) ss << " ";
      for ( size_t j = 8; j > 0; --j ){
	size_t k = 8*(i-1) + j-1;
	ss << bool( t & ( T( 1 ) << k ) );
      }
    }
    return ss.str();
  }
  
};
}}} // namespace emu::daq::server
#endif
