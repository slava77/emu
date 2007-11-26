#ifndef _EmuI2OServer_h_
#define _EmuI2OServer_h_

#include "i2o/Method.h"
#include "xcept/tools.h"
#include "i2o/i2oDdmLib.h"
#include "emu/emuDAQ/emuClient/include/i2oEmuClientMsg.h"
#include "toolbox/mem/CommittedHeapAllocator.h"
#include "toolbox/mem/MemoryPoolFactory.h"
#include "i2o/utils/AddressMap.h"

#include "EmuServer.h"

/// An \ref EmuServer transmitting data via I2O messages.
class EmuI2OServer : public EmuServer {
private:

  toolbox::exception::HandlerSignature       *i2oExceptionHandler_; ///< I2O exception handler

  string                                      poolName_; ///< name of server-to-client data memory pool
  toolbox::mem::CommittedHeapAllocator       *allocator_; ///< allocate fixed size memory
  toolbox::mem::MemoryPoolFactory            *poolFactory_; ///< memory pool factory
  toolbox::mem::Pool                         *pool_; ///< memory pool
  xdata::UnsignedLong                        *poolSize_; ///< committed memory pool size for data sent to client
  I2O_TID                                     clientTid_; ///< target id of client
  I2O_TID                                     tid_; ///< target id of this server

  /// queue of data waiting to be sent
  deque<pair<toolbox::mem::Reference*,bool> > dataBlocks_;

  /// number of \b events already in the queue of data waiting to be sent (=< \ref dataBlocks_ .size())
  unsigned int                                nEventsInQueue;

  /// Finds target id of client.
  void   findClientTid();

  /// Creates a fixed size memory pool on the heap.
  void   createCommittedHeapAllocatorMemoryPool()
    throw ( xcept::Exception );

  /// Appends a block to the queue waiting to be transmitted.

  /// @param data pointer to data
  /// @param dataLength lengh of data in bytes
  /// @param completesEvent \c TRUE if this is the last block of the event
  ///
  void   appendNewBlock( char*               data, 
			 const unsigned long dataLength, 
			 const bool          completesEvent )
    throw ( xcept::Exception );

  /// Fills block with data.

  /// @param bufRef reference to memory block block
  /// @param data pointer to data
  /// @param dataLength lengh of data in bytes
  ///
  void   fillBlock( toolbox::mem::Reference *bufRef,
		    char*                    data,
		    const unsigned int       dataLength )
    throw ( xcept::Exception );

  /// Includes in the message the number of event credits still held.

  /// @param bufRef  reference to memory block block
  /// @param completesEvent \c TRUE if this is the last block of the event
  ///
  void insertNumberOfCreditsHeld( toolbox::mem::Reference *bufRef, bool completesEvent  );

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
  EmuI2OServer( xdaq::Application                    *parentApp,
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
  ~EmuI2OServer();

  /// Adds data to the queue waiting to be transmitted to the client.

  /// @param runNumber run number
  /// @param nEvents number of events read/processed by the parent application
  /// @param completesEvent \c TRUE if this is the last block of the event
  /// @param errorFlag error flag to be transmitted along with the data
  /// @param data data
  /// @param dataLength data length in bytes
  ///
  void   addData( const int            runNumber, 
		  const int            nEvents, 
		  const bool           completesEvent, 
		  const unsigned short errorFlag, 
		  char*                data, 
		  const int            dataLength );

  /// Transmits data to client.
  void   sendData()
    throw ( xcept::Exception );

  /// Marks the last block as last of event.
  void makeLastBlockCompleteEvent();

  /// accessor of target id of client

  /// @return target id of client
  ///
  unsigned long getClientTid(){ return clientTid_; }

  /// dummy in I2O server
  xoap::MessageReference getOldestMessagePendingTransmission(){ xoap::MessageReference m; m=NULL; return m; }
};

#endif
