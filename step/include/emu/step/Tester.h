#ifndef __emu_step_Tester_h__
#define __emu_step_Tester_h__

#include "emu/step/Application.h"
#include "emu/step/Test.h"
#include "emu/step/ChamberMap.h"

#include "emu/utils/System.h"

#include "toolbox/BSem.h"
#include "toolbox/task/Action.h"
#include "toolbox/task/WorkLoop.h"
#include "xdata/Boolean.h"
#include "xdata/InfoSpace.h"
#include "xdata/String.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/UnsignedInteger64.h"
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
      
      vector<string> getDataDirNamesSTL();

      virtual void configureAction( toolbox::Event::Reference e );
      virtual void enableAction   ( toolbox::Event::Reference e );
      virtual void haltAction     ( toolbox::Event::Reference e );
      virtual void stopAction     ( toolbox::Event::Reference e );
      
    private:
      void exportParameters();
      void loadFiles();
      string selectCratesAndChambers( const string& generalVMESettingsXML );
      void createChamberMaps( const string& selectedVMESettingsXML );
      bool configureTestInWorkLoop( toolbox::task::WorkLoop *wl );
      bool enableTestInWorkLoop( toolbox::task::WorkLoop *wl );
      void actionPerformed( xdata::Event& received );
      
      static const string workLoopType_; ///< the type of the work loop
      string workLoopName_;              ///< the name of the work loop
      xdata::String  group_; ///< name of the VME crate group that this emu::step::Tester instance handles
      xdata::String  testParametersFileName_; ///< name of file containing the XML of the parameters of the test
      xdata::String  vmeSettingsFileName_; ///< name of file containing the XML of the PCrate and on-chamber electronics settings
      xdata::String  specialVMESettingsFileName_; ///< name of file containing the XML of the PCrate and on-chamber electronics settings specific to the tests
      xdata::String  testId_;	///< test id
      xdata::Vector< xdata::Bag<ChamberMap> > chamberMaps_; ///< the parameters that the analysis program will use for identifying which chamber the data belongs to
      xdata::Boolean fakeTests_; ///< if \em true, the software just fakes the tests, no attempt is made to access hardware
      xdata::Boolean testConfigured_; ///< if \em true, the test has been configured
      xdata::Boolean testDone_; ///< if \em true, the test has been executed or aborted
      xdata::Vector<xdata::String> crateIds_; ///< ids of crates that this emu::step::Tester instance handles
      xdata::Vector<xdata::String> chamberLabels_; ///< labels (names) of chambers that this emu::step::Tester instance handles
      xdata::Double  progress_;	///< progress in percent of the ongoing test
      xdata::UnsignedInteger64 nEvents_;  ///< the total number of events to be taken in the test
      xdata::UnsignedInteger32 runNumber_;  ///< run number as obtained from the local DAQ
      xdata::String  runStartTime_;	///< time of start of run as obtained from the local DAQ (e.g., 130529_154434_UTC)
      xdata::Vector<xdata::String> dataDirNames_;  ///< all RUIs' data directory names (hostname:directory)

      Test* test_;
      toolbox::task::WorkLoop *workLoop_; ///< work loop for the test procedure to be executed in a separate thread
      toolbox::task::ActionSignature *configureTestSignature_;
      toolbox::task::ActionSignature *enableTestSignature_;
      string testParametersXML_; ///< XML of the parameters of the test
      string vmeSettingsXML_;	///< XML of the PCrate and on-chamber electronics settings
      string specialVMESettingsXML_; ///< XML of the PCrate and on-chamber electronics settings specific to the tests
    };
  }
}

#endif
