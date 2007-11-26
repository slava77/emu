#ifndef _EmuSOAPServer_h_
#define _EmuSOAPServer_h_

#include "xcept/tools.h"

#include "xdaq/NamespaceURI.h"

#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/Method.h"

#include "EmuServer.h"

/// An \ref EmuServer transmitting data via SOAP messages.
class EmuSOAPServer : public EmuServer {
private:

  /// whether or not its client is non-persistent (e.g. \c curl )
  xdata::Boolean                            *persists_;

  // The message under construction
  xoap::MessageReference                     messageReference_;	///< the SOAP message under construction
  bool                                       messageComplete_; ///< \c TRUE if SOAP message is complete

  // The queue of already complete messages waiting to be transmitted
  deque<xoap::MessageReference>              messages_; ///< queue of messages each carrying one event
  unsigned int                               maxMessagesPendingTransmission_; ///< maximum number of messages in queue

  // To be included in the SOAP message's attachment's MIME part header
  string                                     contentId_; ///< content id to be included in the SOAP message's attachment's MIME part header
  string                                     contentType_; ///< content type to be included in the SOAP message's attachment's MIME part header
  string                                     contentLocation_; ///< content location to be included in the SOAP message's attachment's MIME part header
  string                                     contentEncoding_; ///< content encoding to be included in the SOAP message's attachment's MIME part header

  /// Creates SOAP message.
  void   createMessage()
    throw ( xoap::exception::Exception );

  /// Creates MIME info to be included in the SOAP message's attachment's MIME part header.
  void   createMIMEInfo();

  /// Appends data to the queue waiting to be transmitted.

  /// @param data pointer to data
  /// @param dataLength length of data in bytes
  /// @param completesEvent \c TRUE if this is the last block of the event
  ///
  void   appendData( char* const         data, 
		     const unsigned long dataLength, 
		     const bool          completesEvent )
    throw( xoap::exception::Exception );

  /// Adds data as attachment to SOAP message.

  /// @param data pointer to data
  /// @param dataLength length of data in bytes
  ///
  void   fillMessage( char* const        data,
		      const unsigned int dataLength )
    throw( xoap::exception::Exception );

public:

  /// constructor

  /// @param parentApp parentApp parent application of this server
  /// @param i2oExceptionHandler I2O exception handler
  /// @param clientName client name (must be passed by value to make sure it remains unchanged here
  /// even after the it is modified in the parent app's info space)
  /// @param clientInstance instance of client of this server (must be passed by value to make sure 
  /// it remains unchanged here even after the it is modified in the parent app's info space)
  /// @param persists whether or not its client is non-persistent (e.g. \c curl )
  /// @param prescaling if prescaling is \e n, only every <em>n</em>th event will be sent
  /// @param onRequest  if \c TRUE , data is sent only if credits have been received
  /// @param creditsHeld number of events yet to be transmitted
  /// @param logger logger
  EmuSOAPServer( xdaq::Application                    *parentApp,
		 const string                          clientName,
		 const unsigned int                    clientInstance,
		 xdata::Serializable                  *persists,
		 xdata::Serializable                  *prescaling,
		 xdata::Serializable                  *onRequest,
		 xdata::Serializable                  *creditsHeld,
		 const Logger                         *logger )
    throw( xcept::Exception );

  /// destructor
  ~EmuSOAPServer();

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

  /// dummy in SOAP client
  unsigned long getClientTid(){ return( (unsigned long)0 ); }

  /// Marks the last block as last of event.
  void makeLastBlockCompleteEvent();

  /// Gets reference to the message in the queue that's been waiting for the longest time and will be sent first.

  /// @return reference to the oldest message
  ///
  xoap::MessageReference getOldestMessagePendingTransmission();

  /// Prints messages to logger for debugging purposes.
  void printMessages();
};

#endif
