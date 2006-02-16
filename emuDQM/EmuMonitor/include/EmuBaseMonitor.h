#ifndef EmuBaseMonitor_h
#define EmuBaseMonitor_h
/** This class is a base which receives event data from an RUI
 *  and processes it.  If a target RUI is given, it will ask that RUI for data
 *  as soon as it's done processing an event.  If not, it'll sit and wait for data.
 *  This was factored out of EmuMonitor so it would be easier to test independently.
 \author Rick Wilkinson
 */

#include "xdaq.h"
#include "toolbox.h"
#include "xdata.h"

#include "i2o.h"
#include "i2o/Method.h"
#include "i2o/utils/AddressMap.h"

#include "cgicc.h"
#include "xgi.h"

// #include "evb/ru/include/RuDataReadyMsgListener.h"


class EmuBaseMonitor : public xdaq::WebApplication
{
public:
  
 //! define factory method for instantion of EmuLocalRUI application
//       XDAQ_INSTANTIATOR();

//   EmuBaseMonitor() throw (xdaq::exception::Exception);
  EmuBaseMonitor(xdaq::ApplicationStub* stub) throw (xdaq::exception::Exception);
  ~EmuBaseMonitor();
	
  bool onError ( xcept::Exception& ex, void * context );

  void Configure() throw (toolbox::fsm::exception::Exception);

  void Enable() throw (toolbox::fsm::exception::Exception);

  void Halt() throw (toolbox::fsm::exception::Exception);

/*
  xoap::MessageReference fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception);

  void ConfigureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);
  void EnableAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception );
  void HaltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception );
*/
/*
  // Web callback functions
        void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
        void dispatch (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception);
        void Configure(xgi::Input * in ) throw (xgi::exception::Exception);
        void Enable(xgi::Input * in ) throw (xgi::exception::Exception);
        void Halt(xgi::Input * in ) throw (xgi::exception::Exception);
        void stateMachinePage( xgi::Output * out ) throw (xgi::exception::Exception);
        void failurePage(xgi::Output * out, xgi::exception::Exception & e)  throw (xgi::exception::Exception);
*/

  /// asks for data
  void I2O_sendDataRequest();

  /// the thing that receives the data
  void I2O_ruDataReadyMsg(toolbox::mem::Reference * ref)  throw (i2o::exception::Exception);

  /// makes sure header & trailer OK, not the same L1 number as previousEvent
  bool checkEvent(const char * data, int dataSize);

protected:
  /// default does nothing
  virtual void processEvent(const char * data, int dataSize, unsigned long errorFlag);

private:
  /// the RUI who will receive data requests.  
  //If this is "-1", it just waits for data to be pushed to it
  xdata::Integer dataSourceTargetId_;
  /// the infinite loop needs to be told when to quit
  /// maybe XDAQ has a command for this?
  bool enabled_;

  int previousEventNumber_;

 //  toolbox::fsm::FiniteStateMachine fsm_; // application state machine
 // xgi::WSM wsm_; // Web dialog state machine

  toolbox::exception::HandlerSignature  * errorHandler_;


};

#endif

