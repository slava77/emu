#ifndef __emu_step_Application_h__
#define __emu_step_Application_h__


#include "xdaq/ApplicationGroup.h"
#include "xdaq/WebApplication.h"
#include "xdata/Boolean.h"
#include "xdata/InfoSpace.h"
#include "xdata/String.h"
#include "xdata/Boolean.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/Vector.h"
#include "toolbox/fsm/FiniteStateMachine.h"
#include "toolbox/task/Action.h"
#include "toolbox/task/WorkLoop.h"
#include "toolbox/fsm/FailedEvent.h"
#include "toolbox/BSem.h"
#include "xcept/tools.h"
#include "xoap/MessageFactory.h"
#include "xoap/Method.h"

#include <set>
#include <map>
#include <vector>

using namespace std;

namespace emu { namespace step {

    class Application : public virtual xdaq::WebApplication
    {

    public:
      /// constructor
      Application( xdaq::ApplicationStub *s );
      
      void stateChanged( toolbox::fsm::FiniteStateMachine &fsm );
      void fireEvent( const string name );
      void moveToFailedState( xcept::Exception exception );
      string xhtmlformat_exception_history( xcept::Exception& e );

      // FSM transitions
      virtual void configureAction( toolbox::Event::Reference e ) = 0;
      virtual void enableAction   ( toolbox::Event::Reference e ) = 0;
      virtual void haltAction     ( toolbox::Event::Reference e ) = 0;
      virtual void stopAction     ( toolbox::Event::Reference e ) = 0;
      virtual void noAction       ( toolbox::Event::Reference e );
      virtual void failAction     ( toolbox::Event::Reference e );
      virtual void resetAction    ( toolbox::Event::Reference e );

      // SOAP callbacks
      virtual xoap::MessageReference onConfigure( xoap::MessageReference message ){ fireEvent("Configure"); return xoap::createMessage(); }
      virtual xoap::MessageReference onEnable   ( xoap::MessageReference message ){ fireEvent("Enable"); return xoap::createMessage(); }
      virtual xoap::MessageReference onHalt     ( xoap::MessageReference message ){ fireEvent("Halt"); return xoap::createMessage(); }
      virtual xoap::MessageReference onReset    ( xoap::MessageReference message ){ fireEvent("Reset"); return xoap::createMessage(); }
      
    protected:
      toolbox::BSem bsem_;	///< Binary semaphore.
      Logger logger_;
      toolbox::fsm::FiniteStateMachine fsm_; ///< finite state machine
      xdata::String reasonForFailure_;
      xdata::String state_;

      string generateLoggerName();

    };

}}

#endif
