#ifndef _emu_daq_server_Base_h_
#define _emu_daq_server_Base_h_

#include <string>
#include "log4cplus/logger.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/WebApplication.h"
#include "xdata/InfoSpace.h"
#include "xdata/String.h"
#include "xdata/UnsignedLong.h"
#include "xdata/Boolean.h"


#include "xoap/MessageReference.h"

using namespace std;

namespace emu{
  namespace daq{
    namespace server{

/// Base class for data servers.

/// Receives event credits from client. If \ref sendDataOnRequestOnly_ is TRUE, transmits as many events
/// as credits held, otherwise keeps transmitting events. Events may be prescaled, i.e., only every <em>n</em>th
/// transmitted.
class Base {
protected:

  xdaq::Application                          *parentApp_; ///< parent application of this server

  string                                      name_; ///< server name
  xdaq::ApplicationDescriptor                *appDescriptor_; ///< parent application descriptor
  xdaq::ApplicationContext                   *appContext_; ///< parent application context
  xdaq::Zone                                 *zone_; ///< parent application zone

  string                                      clientName_; ///< name of client of this server
  unsigned int                                clientInstance_; ///< instance of client of this server
  xdaq::ApplicationDescriptor                *clientDescriptor_; ///< application descriptor of client of this server


  xdata::UnsignedLong                        *prescaling_; ///< if prescaling is \e n, only every <em>n</em>th event will be sent
  xdata::Boolean                             *sendDataOnRequestOnly_; ///< if true, data is sent only if credits have been received
  xdata::UnsignedLong                        *nEventCreditsHeld_; ///< number of events (from client) yet to be transmitted
  bool                                        dataIsPendingTransmission_; ///< TRUE if having data scheduled for transmission

  Logger                                      logger_; ///< logger

  int                                         runNumber_; ///< run number to be transmitted along with the data
  int                                         runStartUTC_; ///< UTC run start time to be transmitted along with the data
  unsigned short                              errorFlag_; ///< error flag to be transmitted along with the data

  /// Creates server name.
  string         createName();

  /// Finds application descriptor of client, if any.
  void           findClientDescriptor();

public:
  /// constructor

  /// @param parentApp parent application of this server
  /// @param clientName client name (must be passed by value to make sure it remains unchanged here
  /// even after the it is modified in the parent app's info space)
  /// @param clientInstance instance of client of this server
  /// @param prescaling if prescaling is \e n, only every <em>n</em>th event will be sent
  /// @param onRequest if \c TRUE , data is sent only if credits have been received
  /// @param creditsHeld number of events yet to be transmitted
  /// @param logger logger
  Base( xdaq::Application                    *parentApp,
	const string                          clientName,
	const unsigned int                    clientInstance,
	xdata::Serializable                  *prescaling,
	xdata::Serializable                  *onRequest,
	xdata::Serializable                  *creditsHeld,
	const Logger                         *logger )
    throw( xcept::Exception );

  /// destructor
  virtual ~Base(){}

  /// accessor of client's name

  /// @return client's name
  ///
  string getClientName(){ return clientName_; }

  /// accessor of client's instance

  /// @return client's instance
  ///
  unsigned int getClientInstance(){ return clientInstance_; }

  /// accessor of client's application descriptor

  /// @return client's application descriptor
  ///
  xdaq::ApplicationDescriptor* getClientDescriptor(){ return clientDescriptor_; }

  /// not documented here
  virtual unsigned long getClientTid()=0;

  /// Adds event credits.

  /// @param nCredits number of event credits to add
  /// @param prescaling if prescaling is \e n, only every <em>n</em>th event will be sent
  ///
  void   addCredits( const int nCredits, const int prescaling );

  /// not documented here
  virtual void   addData(   const int            runNumber, 
			    const int            runStartUTC,
			    const int            nEvents, 
			    const bool           completesEvent, 
			    const unsigned short errorFlag, 
			    char*                data, 
			    const int            dataLength )=0;
//     throw ( xcept::Exception )=0;

  /// not documented here
  virtual void   sendData()
    throw ( xcept::Exception )=0;

  /// accessor of \ref dataIsPendingTransmission_

  /// @return TRUE if having data scheduled for transmission
  ///
  bool   dataIsPendingTransmission(){ return dataIsPendingTransmission_; }

  /// not documented here
  virtual void makeLastBlockCompleteEvent()=0;

  /// not documented here
  virtual xoap::MessageReference getOldestMessagePendingTransmission()=0;
};

}}} // namespace emu::daq::server
#endif
