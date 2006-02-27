#ifndef _EmuI2OServer_h_
#define _EmuI2OServer_h_

#include "i2o/include/i2o/Method.h"
#include "xcept/include/xcept/tools.h"
#include "extern/i2o/include/i2o/i2oDdmLib.h"
#include "emu/emuDAQ/emuClient/include/i2oEmuClientMsg.h"
#include "toolbox/mem/CommittedHeapAllocator.h"
#include "toolbox/include/toolbox/mem/MemoryPoolFactory.h"
#include "i2o/utils/include/i2o/utils/AddressMap.h"

#include "EmuServer.h"

class EmuI2OServer : public EmuServer {
private:

  toolbox::exception::HandlerSignature       *i2oExceptionHandler_;

  string                                      poolName_;
  toolbox::mem::CommittedHeapAllocator       *allocator_;
  toolbox::mem::MemoryPoolFactory            *poolFactory_;
  toolbox::mem::Pool                         *pool_;
  xdata::UnsignedLong                        *poolSize_;    // committed memory pool size for data sent to client
  I2O_TID                                     clientTid_;

  // Queue of data waitng to be sent. Each block carries one DDU's or DCC's one event
  deque<pair<toolbox::mem::Reference*,bool> > dataBlocks_;

  // The number of _events_ already in the queue of data waiting to be sent (=<dataBlocks_.size())
  unsigned int                                nEventsInQueue;

  void   findClientTid();
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
  void insertNumberOfCreditsHeld( toolbox::mem::Reference *bufRef, bool completesEvent  );

public:
  // clientName must be passed by value to make sure it remains unchanged here
  // even after the it is modified in the parent app's info space
  EmuI2OServer( xdaq::Application                    *parentApp,
		toolbox::exception::HandlerSignature *i2oExceptionHandler,
		const string                          clientName,
		xdata::Serializable                  *poolSize,
		xdata::Serializable                  *prescaling,
		xdata::Serializable                  *onRequest,
		xdata::Serializable                  *creditsHeld,
		const Logger                         *logger )
    throw( xcept::Exception );
  ~EmuI2OServer();
  void   addData(  const int  runNumber, 
		   const int  nEvents, 
		   const bool completesEvent, 
		   char* const data, 
		   const int  dataLength );
  void   sendData()
    throw ( xcept::Exception );
  unsigned long getClientTid(){ return clientTid_; }
  xoap::MessageReference getOldestMessagePendingTransmission(){ xoap::MessageReference m; m=NULL; return m; }
};

#endif
