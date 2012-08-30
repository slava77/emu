#ifndef __emu_step_Tester_h__
#define __emu_step_Tester_h__

#include "emu/step/Application.h"
#include "emu/step/Test.h"

#include "emu/utils/System.h"

#include "toolbox/BSem.h"
#include "toolbox/task/Action.h"
#include "toolbox/task/WorkLoop.h"
#include "xdata/Boolean.h"
#include "xdata/InfoSpace.h"
#include "xdata/String.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/Double.h"
#include "xdata/Vector.h"

#include <vector>
#include <map>
#include <string>

using namespace std;

namespace emu{
  namespace step{
    class Tester 
      : public Application
      , public xdata::ActionListener
    {

    public:
      
      /// define factory method for the instantion of applications
      XDAQ_INSTANTIATOR();
      
      Tester( xdaq::ApplicationStub *s );
      
      virtual void configureAction( toolbox::Event::Reference e );
      virtual void enableAction   ( toolbox::Event::Reference e );
      virtual void haltAction     ( toolbox::Event::Reference e );
      virtual void stopAction     ( toolbox::Event::Reference e );
      
    private:
      void exportParameters();
      void loadFiles();
      string selectCrates( const string& generalVMESettingsXML );
      bool testInWorkLoop( toolbox::task::WorkLoop *wl );
      void actionPerformed( xdata::Event& received );
      
      static const string workLoopType_; ///< the type of the work loop
      string workLoopName_;              ///< the name of the work loop
      xdata::String  group_;
      xdata::String  testParametersFileName_;
      xdata::String  vmeSettingsFileName_;
      xdata::String  specialVMESettingsFileName_;
      xdata::String  testId_;
      xdata::Boolean fakeTests_; ///< if true, the software just fakes the tests, no attempt is made to access hardware
      xdata::Boolean testDone_; ///< if true, the test has been executed or aborted
      xdata::Vector<xdata::String> crateIds_;
      xdata::Double  progress_;	///< progress in percent of the ongoing test

      Test* test_;
      toolbox::task::WorkLoop *workLoop_; ///< work loop for the test procedure to be executed in a separate thread
      toolbox::task::ActionSignature *testSignature_;
      string testParametersXML_;
      string vmeSettingsXML_;
      string specialVMESettingsXML_;
    };
  }
}

#endif
