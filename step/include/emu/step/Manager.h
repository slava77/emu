#ifndef __emu_step_Manager_h__
#define __emu_step_Manager_h__

#include "emu/step/Application.h"
#include "emu/step/Configuration.h"
#include "emu/soap/Messenger.h"

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
#include "xoap/MessageReference.h"

#include <set>
#include <map>
#include <vector>
#include <limits>

using namespace std;

namespace emu { namespace step {

    class Manager : public emu::step::Application
    {
    public:
      
      /// define factory method for the instantion of AFEBTeststand applications
      XDAQ_INSTANTIATOR();
      
      /// constructor
      Manager( xdaq::ApplicationStub *s );

      // FSM transitions
      virtual void configureAction( toolbox::Event::Reference e );
      virtual void enableAction   ( toolbox::Event::Reference e );
      virtual void haltAction     ( toolbox::Event::Reference e );
      virtual void stopAction     ( toolbox::Event::Reference e );

    private:
      void exportParameters();
      void bindWebInterface();
      void createConfiguration();
      bool testSequenceInWorkLoop( toolbox::task::WorkLoop *wl );
      void defaultWebPage( xgi::Input *in, xgi::Output *out );
      void controlWebPage( xgi::Input *in, xgi::Output *out );
      string createXMLWebPage();
      void updateProgress();
      bool waitForDAQToExecute( const string command, const uint64_t seconds = std::numeric_limits<uint64_t>::max() );
      // bool waitForTestersToExecute( const string command, const uint64_t seconds = std::numeric_limits<uint64_t>::max() );
      void waitForTestsToFinish( const bool isTestDurationUndefined );

      string applicationURLPath_; ///< the path part of the URL of this application
      static const string namespace_; ///< the namespace for Emu STEP
      static const string workLoopType_; ///< the type of the work loop
      string workLoopName_;              ///< the name of the work loop
      xdata::String testParametersFileName_;
      xdata::String specialVMESettingsFileName_;
      emu::step::Configuration* configuration_;  ///< configuration
      xdata::Vector<xdata::String> crateIds_;
      xdata::Vector<xdata::String> testSequence_;
      xdata::Boolean isCurrentTestDurationUndefined_; ///< TRUE if the current test needs to accumulate N events, i.e., it's duration is not defined explicitly or by pulse injections
      toolbox::task::WorkLoop *workLoop_; ///< work loop for the test procedure to be executed in a separate thread
      toolbox::task::ActionSignature *testSequenceSignature_;
      map<string,xdaq::ApplicationDescriptor*> testerDescriptors_; ///< peripheral crate group --> emu::step::Tester application descriptor map
    };

}}

#endif
