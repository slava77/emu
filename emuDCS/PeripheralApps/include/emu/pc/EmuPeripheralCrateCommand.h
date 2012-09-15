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

#include "emu/pc/VMEController.h"
#include "emu/pc/Crate.h"
#include "emu/pc/DAQMB.h"
#include "emu/pc/TMB.h"
#include "emu/pc/TMBTester.h"
#include "emu/pc/CCB.h"
#include "emu/pc/MPC.h"
#include "emu/pc/TMBTester.h"
#include "emu/pc/ALCTController.h"
#include "emu/pc/RAT.h"
#include "emu/pc/ChamberUtilities.h"
//#include "emu/pc/geom.h"
#include "emu/pc/CrateUtilities.h"
#include "emu/pc/CalibDAQ.h"
#include "emu/pc/EmuEndcap.h"
//
#include "emu/pc/EmuPeripheralCrateBase.h"

namespace emu {
  namespace pc {

// structure definitions for ALCT test parameters (Madorsky)
typedef struct test_configuration_11
{
	int events_total;
} 
test_config_11;

typedef struct test_configuration_12
{
	int events_per_strip;
	int alct_test_pulse_amp_11;
	int alct_test_pulse_amp_12;
	int alct_test_pulse_amp_13;
	int alct_test_pulse_amp_21;
	int alct_test_pulse_amp_22;
	int alct_test_pulse_amp_31;
	int alct_test_pulse_amp_32;
	int alct_test_pulse_amp_41;
	int alct_test_pulse_amp_42;
	
} 
test_config_12;

typedef struct test_configuration_13
{
	int events_per_threshold;
	int thresholds_per_tpamp;
	int threshold_first;	 
	int threshold_step;	 
	int tpamps_per_run;	 
	int tpamp_first;     
	int tpamp_step;	     
} 
test_config_13;

typedef struct test_configuration_14
{
	int alct_test_pulse_amp;
	int events_per_delay;
	int delays_per_run; 
	int delay_first;	 
	int delay_step;	 
} 
test_config_14;

typedef struct test_configuration_15
{
	int events_total;
} 
test_config_15;

typedef struct test_configuration_16
{
	int events_per_layer;
	int alct_test_pulse_amp;
} 
test_config_16;

typedef struct test_configuration_17
{
	int dmb_test_pulse_amp;
	int events_per_delay; 
	int delays_per_strip; 
	int delay_first;      
	int delay_step;       
	int strips_per_run;   
	int strip_first;      
	int strip_step;       
} 
test_config_17;

typedef struct test_configuration_18
{
	int events_total;
} 
test_config_18;

typedef struct test_configuration_19
{
	int scale_turnoff;
	int range_turnoff;
	int events_per_thresh;
	int threshs_per_tpamp;
	int thresh_first;
	int thresh_step;
	int dmb_tpamps_per_strip;
	int dmb_tpamp_first;
	int dmb_tpamp_step;
	int strips_per_run;
	int strip_first;
	int strip_step;
} 
test_config_19;

typedef struct test_configuration_21
{
	int dmb_test_pulse_amp;
	int cfeb_threshold;    
	int events_per_hstrip; 
	int hstrips_per_run;   
	int hstrip_first;      
	int hstrip_step;         
} 
test_config_21;

typedef struct test_configuration_30
{
	int events_per_delay; 
	int tmb_l1a_delays_per_run;
	int tmb_l1a_delay_first;
	int tmb_l1a_delay_step;
} 
test_config_30;

typedef struct test_config_structure
{
	test_config_11 t11;
	test_config_12 t12;
	test_config_13 t13;
	test_config_14 t14;
	test_config_15 t15;
	test_config_16 t16;
	test_config_17 t17;
	test_config_18 t18;
	test_config_19 t19;
	test_config_21 t21;
	test_config_30 t30;
}
test_config_struct;

// ---
  
class EmuPeripheralCrateCommand: public EmuPeripheralCrateBase
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
  //
  //TMB * thisTMB ;
  //DAQMB* thisDMB ;
  //
  CCB* thisCCB ;
  ALCTController *alct ;
  RAT * rat;
  MPC * thisMPC;
  CrateUtilities myCrateTest;
  std::ostringstream CrateTestsOutput;
  ChamberUtilities MyTest[10][30];
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
  std::vector<TMB*>   tmbVector;
  std::vector<TMBTester>   tmbTestVector;
  std::vector<DAQMB*> dmbVector;
  std::vector<Crate*> crateVector;
  std::vector<Chamber*> chamberVector;
  Crate *thisCrate;
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
  bool ccb_checked_;
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
  EmuEndcap * emuEndcap_;
  //
public:
  //
  XDAQ_INSTANTIATOR();
  //
  EmuPeripheralCrateCommand(xdaq::ApplicationStub * s);
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void MainPage(xgi::Input * in, xgi::Output * out );

private:

  xoap::MessageReference onFastLoop(xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onSlowLoop(xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onExtraLoop(xoap::MessageReference message) throw (xoap::exception::Exception);
  
  xoap::MessageReference onCalibration(xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onConfigure (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnable (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onDisable (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onHalt (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onConfigCalCFEB (xoap::MessageReference message) throw (xoap::exception::Exception);
  
  // alct calibration Madorsky
  xoap::MessageReference onConfigCalALCT (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnableCalALCTConnectivity (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnableCalALCTThresholds (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnableCalALCTDelays (xoap::MessageReference message) throw (xoap::exception::Exception);
  // ---

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

  void ResetAllTMBCounters();
  int VerifyCratesConfiguration();
  bool ParsingXML();
  void SetCurrentCrate(int crate);
  void CheckPeripheralCrateConfiguration();
  void check_controllers();
  int VerifyCCBs();

  // alct calibration Madorsky
  int read_test_config(char* xmlFile, test_config_struct * tcs);
  std::string& trim(std::string &str);
  test_config_struct tcs;
  int calsetup;
  // ---

    
};

  } // namespace emu::pc
} // namespace emu
  
#endif
