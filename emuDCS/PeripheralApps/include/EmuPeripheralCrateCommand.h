#ifndef _EmuPeripheralCrateCommand_h_
#define _EmuPeripheralCrateCommand_h_

#include "xdaq/Application.h"
//#include "xdaq/Zone.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/UnsignedShort.h"
#include "xdata/UnsignedLong.h"
#include "xdata/String.h"
#include "xdata/Vector.h"
#include "xdata/TimeVal.h"
#include "xdaq/NamespaceURI.h"
#include "xdata/InfoSpaceFactory.h"
#include "xcept/tools.h"

#include "toolbox/Event.h"
#include "toolbox/string.h"
#include "toolbox/net/URL.h"
#include "toolbox/task/TimerTask.h"
#include "toolbox/task/Timer.h"
#include "toolbox/task/TimerFactory.h"
#include "toolbox/TimeInterval.h"

#include "xgi/Utils.h"
#include "xgi/Method.h"
#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/Method.h"

#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

// My Stuff

#include "EmuController.h"
#include "VMEController.h"
#include "Crate.h"
#include "DAQMB.h"
#include "TMB.h"
#include "TMBTester.h"
#include "CCB.h"
#include "MPC.h"
#include "TMBTester.h"
#include "ALCTController.h"
#include "RAT.h"
#include "ChamberUtilities.h"
//#include "geom.h"
#include "CrateUtilities.h"
#include "CalibDAQ.h"
#include "EmuEndcap.h"
#include "BoardsDB.h"
#include "EmuTStore.h"
//
#include "EmuApplication.h"


class EmuPeripheralCrateCommand: public EmuApplication, xdata::ActionListener
{
  //
protected:
  //
  xdata::UnsignedLong runNumber_;
  xdata::UnsignedLong maxNumTriggers_;
  xdata::UnsignedLong GlobalRun_;
  //
  int xml_or_db;
  xdata::String  XML_or_DB_;
  xdata::String  EMU_config_ID_;
      
  xdata::String xmlFile_;
  xdata::String CalibrationState_;
  //
  xdata::String curlCommand_;         // the curl command's full path
  xdata::String curlCookies_;         // file for cookies
  xdata::String CMSUserFile_;         // file that contains the username:password for CMS user
  xdata::String eLogUserFile_;        // file that contains the username:password for eLog user
  xdata::String eLogURL_;             // eLog's URL 
  //
  xdata::String TestLogFile_;
  xdata::String myCounter_;
  //
  bool DisplayRatio_;
  bool AutoRefresh_;
  int MenuMonitor_;
  //
  xdata::String CalibType_;
  xdata::UnsignedInteger CalibNumber_;
  std::string CalibTypeStr_; 
  //
  std::string xmlFile;
  xdata::UnsignedLong myParameter_;
  emu::pc::EmuController * MyController;
  emu::pc::EmuTStore * myTStore;
  //
  //TMB * thisTMB ;
  //DAQMB* thisDMB ;
  //
  emu::pc::CCB* thisCCB ;
  emu::pc::ALCTController *alct ;
  emu::pc::RAT * rat;
  emu::pc::MPC * thisMPC;
  emu::db::BoardsDB *brddb;
  emu::pc::CrateUtilities myCrateTest;
  std::ostringstream CrateTestsOutput;
  emu::pc::ChamberUtilities MyTest[10][30];
  std::ostringstream ChamberTestsOutput[10][30];
  std::ostringstream OutputStringDMBStatus[10];
  std::ostringstream OutputStringTMBStatus[10];
  std::ostringstream OutputDMBTests[10][30];
  std::ostringstream OutputTMBTests[10][30];
  std::ostringstream OutputCheckConfiguration;
  std::vector <float> ChartData[100];
  int TMBTriggerAlct0Key[120][9];
  int TMBTriggerAlct1Key[120][9];
  int TMBTriggerClct0keyHalfStrip[120][9];
  int TMBTriggerClct1keyHalfStrip[120][9];
  //
  int tmb_vme_ready;
  //
  int CCBRegisterValue_;
  std::vector<emu::pc::TMB*>   tmbVector;
  std::vector<emu::pc::TMBTester>   tmbTestVector;
  std::vector<emu::pc::DAQMB*> dmbVector;
  std::vector<emu::pc::Crate*> crateVector;
  std::vector<emu::pc::Chamber*> chamberVector;
  emu::pc::Crate *thisCrate;
  std::string Operator_;
  std::string RunNumber_;
  int Counter_;
  int nTrigger_;

  int parsed;
  int current_config_state_;  // index to global_config_states[]
  int current_run_state_;     // index to global_run_states[]
  std::string global_config_states[4];
  std::string global_run_states[4];
  unsigned int total_crates_;
  int this_crate_no_;
  std::string ThisCrateID_;
  
  bool Monitor_On_, Monitor_Ready_;
  //
  bool controller_checked_;
  int current_crate_;
  int all_crates_ok;;
  int crate_check_ok[60];
  int ccb_check_ok[60];
  int mpc_check_ok[60];
  int alct_check_ok[60][9];
  int tmb_check_ok[60][9];
  int dmb_check_ok[60][9];
  //
  std::vector<int> L1aLctCounter_;
  std::vector<int> CfebDavCounter_;
  std::vector<int> TmbDavCounter_;
  std::vector<int> AlctDavCounter_;
  //
  emu::pc::EmuEndcap * emuEndcap_;
  //
public:
  //
  XDAQ_INSTANTIATOR();
  //
  EmuPeripheralCrateCommand(xdaq::ApplicationStub * s);
  void EmuPeripheralCrateCommand::actionPerformed (xdata::Event& e);
  void EmuPeripheralCrateCommand::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void EmuPeripheralCrateCommand::MainPage(xgi::Input * in, xgi::Output * out );

private:

  xoap::MessageReference onFastLoop(xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onSlowLoop(xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onExtraLoop(xoap::MessageReference message) throw (xoap::exception::Exception);
  
  xoap::MessageReference onCalibration(xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onConfigure (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnable (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onDisable (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onHalt (xoap::MessageReference message) throw (xoap::exception::Exception);

  void configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception); 
  void configureFail(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception); 
  void reConfigureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception); 
  void enableAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception); 
  void disableAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception); 
  void haltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception); 
  void stateChanged(toolbox::fsm::FiniteStateMachine &fsm) throw (toolbox::fsm::exception::Exception);
  void MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) throw (xgi::exception::Exception); 

  void CheckCrates(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CheckCratesConfiguration(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CheckCrateConfiguration(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 

  int VerifyCratesConfiguration();
  bool ParsingXML();
  void SetCurrentCrate(int crate);
  void CheckPeripheralCrateConfiguration();
  void CheckControllers();
  //
  xoap::MessageReference PCcreateCommandSOAP(std::string command);
  void PCsendCommand(std::string command, std::string klass) throw (xoap::exception::Exception, xdaq::exception::Exception);
    
};

#endif
