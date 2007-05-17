// $Id: EmuPeripheralCrate.cc

#include "EmuPeripheralCrate.h"

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <unistd.h> // for sleep()
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <time.h>

using namespace cgicc;
using namespace std;

const string       CFEB_FIRMWARE_FILENAME = "cfeb/cfeb_pro.svf";
const unsigned int EXPECTED_CFEB_USERID   = 0xcfeda092;
//
const string       DMB_FIRMWARE_FILENAME    = "dmb/dmb6cntl_pro.svf";
const unsigned int EXPECTED_DMB_USERID      = 0x48547231;
const string       DMBVME_FIRMWARE_FILENAME = "dmb/dmb6vme_pro.svf";
//
//In order to load firmware automatically from the firmware values in the xml files, 
//the firmware needs to reside in directories in the form:
//    TMB  ->  $HOME/firmware/tmb/YEARMONTHDAY/tmb.svf
//    RAT  ->  $HOME/firmware/rat/YEARMONTHDAY/rat.svf
//    ALCT ->  $HOME/firmware/alct/YEARMONTHDAY/alctXXX/alctXXX.svf
// with the zero-values filled in with 0's.  
// In other words:  9 April 2007 firmware should reside in YEARMONTHDAY=20070409
//
// The XXX in the ALCT firmware specification corresponds to the following structure:
const string ALCT_FIRMWARE_FILENAME_ME11 = "alct288/alct288.svf";
const string ALCT_FIRMWARE_FILENAME_ME11_BACKWARD_NEGATIVE = "alct288bn/alct288bn.svf";
const string ALCT_FIRMWARE_FILENAME_ME11_BACKWARD_POSITIVE = "alct288bp/alct288bp.svf";
const string ALCT_FIRMWARE_FILENAME_ME11_FORWARD_POSITIVE  = "alct288fp/alct288fp.svf";
const string ALCT_FIRMWARE_FILENAME_ME12 = "alct384/alct384.svf";
const string ALCT_FIRMWARE_FILENAME_ME13 = "alct192/alct192.svf";
const string ALCT_FIRMWARE_FILENAME_ME21 = "alct672/alct672.svf";
const string ALCT_FIRMWARE_FILENAME_ME22 = "alct384/alct384.svf";
const string ALCT_FIRMWARE_FILENAME_ME31 = "alct576mirror/alct576mirror.svf";
const string ALCT_FIRMWARE_FILENAME_ME32 = "alct384mirror/alct384mirror.svf";
const string ALCT_FIRMWARE_FILENAME_ME41 = "alct672mirror/alct672mirror.svf";
//
// N.B. not yet able to load automatically from xml for RAT...
const string RAT_FIRMWARE_FILENAME = "rat/20060828/rat.svf";
//
//

  EmuPeripheralCrate::EmuPeripheralCrate(xdaq::ApplicationStub * s): EmuApplication(s)
    //#endif
    {	
    //
    FirmwareDir_ = getenv("HOME");
    FirmwareDir_ += "/firmware/";
    //
    DisplayRatio_ = false;
    AutoRefresh_  = true;
    MyController = 0;
    //thisTMB = 0;
    //thisDMB = 0;
    thisCCB = 0;
    thisMPC = 0;
    rat = 0;
    alct = 0;
    nTrigger_ = 100;
    MenuMonitor_ = 2;
    //
    xgi::bind(this,&EmuPeripheralCrate::Default, "Default");
    xgi::bind(this,&EmuPeripheralCrate::MainPage, "MainPage");
    xgi::bind(this,&EmuPeripheralCrate::setConfFile, "setConfFile");
    xgi::bind(this,&EmuPeripheralCrate::getTestLogFile, "getTestLogFile");
    xgi::bind(this,&EmuPeripheralCrate::getTestLogFileUpload, "getTestLogFileUpload");
    xgi::bind(this,&EmuPeripheralCrate::TmbMPCTest, "TmbMPCTest");
    xgi::bind(this,&EmuPeripheralCrate::InitSystem, "InitSystem");
    xgi::bind(this,&EmuPeripheralCrate::InitChamber, "InitChamber");
    xgi::bind(this,&EmuPeripheralCrate::setRawConfFile, "setRawConfFile");
    xgi::bind(this,&EmuPeripheralCrate::UploadConfFile, "UploadConfFile");
    xgi::bind(this,&EmuPeripheralCrate::TMBStatus, "TMBStatus");
    xgi::bind(this,&EmuPeripheralCrate::SetUnsetRatio, "SetUnsetRatio");
    xgi::bind(this,&EmuPeripheralCrate::SetUnsetAutoRefresh, "SetUnsetAutoRefresh");
    xgi::bind(this,&EmuPeripheralCrate::DefineConfiguration, "DefineConfiguration");
    xgi::bind(this,&EmuPeripheralCrate::LogCrateTestsOutput, "LogCrateTestsOutput");
    //
    xgi::bind(this,&EmuPeripheralCrate::getData0, "getData0");
    xgi::bind(this,&EmuPeripheralCrate::getData1, "getData1");
    xgi::bind(this,&EmuPeripheralCrate::getData2, "getData2");
    xgi::bind(this,&EmuPeripheralCrate::getData3, "getData3");
    xgi::bind(this,&EmuPeripheralCrate::getData4, "getData4");
    xgi::bind(this,&EmuPeripheralCrate::getData5, "getData5");
    xgi::bind(this,&EmuPeripheralCrate::getData6, "getData6");
    xgi::bind(this,&EmuPeripheralCrate::getData7, "getData7");
    xgi::bind(this,&EmuPeripheralCrate::getData8, "getData8");
    xgi::bind(this,&EmuPeripheralCrate::getData9, "getData9");
    xgi::bind(this,&EmuPeripheralCrate::getData10, "getData10");
    xgi::bind(this,&EmuPeripheralCrate::getData11, "getData11");
    xgi::bind(this,&EmuPeripheralCrate::getData12, "getData12");
    xgi::bind(this,&EmuPeripheralCrate::getData13, "getData13");
    xgi::bind(this,&EmuPeripheralCrate::getData14, "getData14");
    xgi::bind(this,&EmuPeripheralCrate::getData15, "getData15");
    xgi::bind(this,&EmuPeripheralCrate::getData16, "getData16");
    xgi::bind(this,&EmuPeripheralCrate::getData17, "getData17");
    xgi::bind(this,&EmuPeripheralCrate::getData18, "getData18");
    xgi::bind(this,&EmuPeripheralCrate::getData19, "getData19");
    xgi::bind(this,&EmuPeripheralCrate::getData20, "getData20");
    xgi::bind(this,&EmuPeripheralCrate::getData21, "getData21");
    xgi::bind(this,&EmuPeripheralCrate::getDataTMBTriggerAlct0Key, "getDataTMBTriggerAlct0Key");
    xgi::bind(this,&EmuPeripheralCrate::getDataTMBTriggerAlct1Key, "getDataTMBTriggerAlct1Key");
    xgi::bind(this,&EmuPeripheralCrate::getDataTMBTriggerAlct0Key, "getDataTMBTriggerClct0Key");
    xgi::bind(this,&EmuPeripheralCrate::getDataTMBTriggerAlct1Key, "getDataTMBTriggerClct1Key");
    //
    xgi::bind(this,&EmuPeripheralCrate::EnableDisableDebug, "EnableDisableDebug");
    xgi::bind(this,&EmuPeripheralCrate::LoadTMBFirmware, "LoadTMBFirmware");
    xgi::bind(this,&EmuPeripheralCrate::LoadALCTFirmware, "LoadALCTFirmware");
    xgi::bind(this,&EmuPeripheralCrate::LoadRATFirmware, "LoadRATFirmware");
    xgi::bind(this,&EmuPeripheralCrate::ReadTMBRegister, "ReadTMBRegister");
    xgi::bind(this,&EmuPeripheralCrate::ReadCCBRegister, "ReadCCBRegister");
    xgi::bind(this,&EmuPeripheralCrate::HardReset, "HardReset");
    xgi::bind(this,&EmuPeripheralCrate::testTMB, "testTMB");
    xgi::bind(this,&EmuPeripheralCrate::Automatic, "Automatic");
    xgi::bind(this,&EmuPeripheralCrate::TMBTests,  "TMBTests");
    xgi::bind(this,&EmuPeripheralCrate::TMBUtils,  "TMBUtils");
    xgi::bind(this,&EmuPeripheralCrate::DMBStatus, "DMBStatus");
    xgi::bind(this,&EmuPeripheralCrate::DMBBoardID, "DMBBoardID");
    xgi::bind(this,&EmuPeripheralCrate::TMBBoardID, "TMBBoardID");
    xgi::bind(this,&EmuPeripheralCrate::RATBoardID, "RATBoardID");
    xgi::bind(this,&EmuPeripheralCrate::CCBStatus, "CCBStatus");
    xgi::bind(this,&EmuPeripheralCrate::CCBUtils, "CCBUtils");
    xgi::bind(this,&EmuPeripheralCrate::CCBLoadFirmware, "CCBLoadFirmware");
    xgi::bind(this,&EmuPeripheralCrate::ControllerUtils, "ControllerUtils");
    xgi::bind(this,&EmuPeripheralCrate::MPCStatus, "MPCStatus");
    xgi::bind(this,&EmuPeripheralCrate::MPCUtils, "MPCUtils");
    xgi::bind(this,&EmuPeripheralCrate::MPCLoadFirmware, "MPCLoadFirmware");
    xgi::bind(this,&EmuPeripheralCrate::DMBTests, "DMBTests");
    xgi::bind(this,&EmuPeripheralCrate::DMBUtils, "DMBUtils");
    xgi::bind(this,&EmuPeripheralCrate::DMBLoadFirmware, "DMBLoadFirmware");
    xgi::bind(this,&EmuPeripheralCrate::DMBVmeLoadFirmware, "DMBVmeLoadFirmware");
    xgi::bind(this,&EmuPeripheralCrate::DMBVmeLoadFirmwareEmergency, "DMBVmeLoadFirmwareEmergency");
    xgi::bind(this,&EmuPeripheralCrate::CFEBLoadFirmware, "CFEBLoadFirmware");
    xgi::bind(this,&EmuPeripheralCrate::CFEBStatus, "CFEBStatus");
    xgi::bind(this,&EmuPeripheralCrate::ALCTStatus, "ALCTStatus");
    xgi::bind(this,&EmuPeripheralCrate::CrateConfiguration, "CrateConfiguration");
    xgi::bind(this,&EmuPeripheralCrate::RATStatus, "RATStatus");
    xgi::bind(this,&EmuPeripheralCrate::CrateTests, "CrateTests");
    xgi::bind(this,&EmuPeripheralCrate::ChamberTests, "ChamberTests");
    xgi::bind(this,&EmuPeripheralCrate::DMBTurnOff, "DMBTurnOff");
    xgi::bind(this,&EmuPeripheralCrate::DMBTurnOn, "DMBTurnOn");
    xgi::bind(this,&EmuPeripheralCrate::TMBPrintCounters, "TMBPrintCounters");
    xgi::bind(this,&EmuPeripheralCrate::TMBResetCounters, "TMBResetCounters");
    xgi::bind(this,&EmuPeripheralCrate::DMBPrintCounters, "DMBPrintCounters");
    xgi::bind(this,&EmuPeripheralCrate::ALCTTiming, "ALCTTiming");
    xgi::bind(this,&EmuPeripheralCrate::ALCTScan, "ALCTScan");
    xgi::bind(this,&EmuPeripheralCrate::FindLv1aDelayComparator, "FindLv1aDelayComparator");
    xgi::bind(this,&EmuPeripheralCrate::FindLv1aDelayALCT, "FindLv1aDelayALCT");
    xgi::bind(this,&EmuPeripheralCrate::CFEBTiming, "CFEBTiming");
    xgi::bind(this,&EmuPeripheralCrate::CFEBScan, "CFEBScan");
    xgi::bind(this,&EmuPeripheralCrate::CalibrationRuns, "CalibrationRuns");
    xgi::bind(this,&EmuPeripheralCrate::TMBStartTrigger, "TMBStartTrigger");
    xgi::bind(this,&EmuPeripheralCrate::EnableL1aRequest, "EnableL1aRequest");
    xgi::bind(this,&EmuPeripheralCrate::TMBL1aTiming, "TMBL1aTiming");
    xgi::bind(this,&EmuPeripheralCrate::ALCTL1aTiming, "ALCTL1aTiming");
    xgi::bind(this,&EmuPeripheralCrate::ALCTvpf,"ALCTvpf");
    xgi::bind(this,&EmuPeripheralCrate::DMBTestAll, "DMBTestAll");
    xgi::bind(this,&EmuPeripheralCrate::DMBTest3, "DMBTest3");
    xgi::bind(this,&EmuPeripheralCrate::DMBTest4, "DMBTest4");
    xgi::bind(this,&EmuPeripheralCrate::DMBTest5, "DMBTest5");
    xgi::bind(this,&EmuPeripheralCrate::DMBTest6, "DMBTest6");
    xgi::bind(this,&EmuPeripheralCrate::DMBTest8, "DMBTest8");
    xgi::bind(this,&EmuPeripheralCrate::DMBTest9, "DMBTest9");
    xgi::bind(this,&EmuPeripheralCrate::DMBTest10, "DMBTest10");
    xgi::bind(this,&EmuPeripheralCrate::DMBTest11, "DMBTest11");
    xgi::bind(this,&EmuPeripheralCrate::TriggerTestInjectALCT, "TriggerTestInjectALCT");
    xgi::bind(this,&EmuPeripheralCrate::TriggerTestInjectCLCT, "TriggerTestInjectCLCT");
    xgi::bind(this,&EmuPeripheralCrate::armScope, "armScope");
    xgi::bind(this,&EmuPeripheralCrate::forceScope, "forceScope");
    xgi::bind(this,&EmuPeripheralCrate::readoutScope, "readoutScope");
    xgi::bind(this,&EmuPeripheralCrate::TMBDumpAllRegisters, "TMBDumpAllRegisters");
    xgi::bind(this,&EmuPeripheralCrate::TMBReadConfiguration, "TMBReadConfiguration");
    xgi::bind(this,&EmuPeripheralCrate::TMBCheckConfiguration, "TMBCheckConfiguration");
    xgi::bind(this,&EmuPeripheralCrate::TMBReadStateMachines, "TMBReadStateMachines");
    xgi::bind(this,&EmuPeripheralCrate::TMBCheckStateMachines, "TMBCheckStateMachines");
    xgi::bind(this,&EmuPeripheralCrate::PowerUp,  "PowerUp");
    xgi::bind(this,&EmuPeripheralCrate::Operator, "Operator");
    xgi::bind(this,&EmuPeripheralCrate::RunNumber, "RunNumber");
    xgi::bind(this,&EmuPeripheralCrate::MPCBoardID, "MPCBoardID");
    xgi::bind(this,&EmuPeripheralCrate::CCBBoardID, "CCBBoardID");
    xgi::bind(this,&EmuPeripheralCrate::ControllerBoardID, "ControllerBoardID");
    xgi::bind(this,&EmuPeripheralCrate::CrateChassisID, "CrateChassisID");
    xgi::bind(this,&EmuPeripheralCrate::CrateRegulatorBoardID, "CrateRegulatorBoardID");
    xgi::bind(this,&EmuPeripheralCrate::PeripheralCrateMotherBoardID, "PeripheralCrateMotherBoardID");
    xgi::bind(this,&EmuPeripheralCrate::ELMBID, "ELMBID");
    xgi::bind(this,&EmuPeripheralCrate::BackplaneID, "BackplaneID");
    xgi::bind(this,&EmuPeripheralCrate::LogDMBTestsOutput, "LogDMBTestsOutput");
    xgi::bind(this,&EmuPeripheralCrate::LogOutput, "LogOutput");
    xgi::bind(this,&EmuPeripheralCrate::LogTestSummary, "LogTestSummary");
    xgi::bind(this,&EmuPeripheralCrate::LogTMBTestsOutput, "LogTMBTestsOutput");
    xgi::bind(this,&EmuPeripheralCrate::FindWinner, "FindWinner");
    xgi::bind(this,&EmuPeripheralCrate::AFFtoL1aAtDMB, "AFFtoL1aAtDMB");
    xgi::bind(this,&EmuPeripheralCrate::RatTmbTiming, "RatTmbTiming");
    xgi::bind(this,&EmuPeripheralCrate::RpcRatTiming, "RpcRatTiming");
    xgi::bind(this,&EmuPeripheralCrate::CalibrationCFEBXtalk, "CalibrationCFEBXtalk");
    xgi::bind(this,&EmuPeripheralCrate::CalibrationCFEBGain, "CalibrationCFEBGain");
    xgi::bind(this,&EmuPeripheralCrate::CalibrationCFEBSaturation, "CalibrationSaturation");
    xgi::bind(this,&EmuPeripheralCrate::CalibrationCFEBPedestal, "CalibrationCFEBPedestal");
    xgi::bind(this,&EmuPeripheralCrate::CalibrationComparatorPulse, "CalibrationComparatorPulse");
    xgi::bind(this,&EmuPeripheralCrate::CalibrationALCTThresholdScan, "CalibrationALCTThresholdScan");
    xgi::bind(this,&EmuPeripheralCrate::CalibrationALCTConnectivity, "CalibrationALCTConnectivity");
    xgi::bind(this,&EmuPeripheralCrate::CalibrationCFEBConnectivity, "CalibrationCFEBConnectivity");
    xgi::bind(this,&EmuPeripheralCrate::LaunchMonitor, "LaunchMonitor");
    xgi::bind(this,&EmuPeripheralCrate::MonitorTMBTrigger, "MonitorTMBTrigger");
    xgi::bind(this,&EmuPeripheralCrate::MonitorTMBTriggerRedirect, "MonitorTMBTriggerRedirect");
    xgi::bind(this,&EmuPeripheralCrate::MenuMonitorTMBTrigger, "MenuMonitorTMBTrigger");
    xgi::bind(this,&EmuPeripheralCrate::AlctKey, "AlctKey");
    xgi::bind(this,&EmuPeripheralCrate::ClctKey, "ClctKey");
    xgi::bind(this,&EmuPeripheralCrate::MonitorTMBTriggerDisplay, "MonitorTMBTriggerDisplay");
    xgi::bind(this,&EmuPeripheralCrate::CrateTMBCounters, "CrateTMBCounters");
    xgi::bind(this,&EmuPeripheralCrate::CrateDMBCounters, "CrateDMBCounters");
    xgi::bind(this,&EmuPeripheralCrate::CrateTMBCountersRight, "CrateTMBCountersRight");
    xgi::bind(this,&EmuPeripheralCrate::CrateStatus, "CrateStatus");
    xgi::bind(this,&EmuPeripheralCrate::CrateXUtils, "CrateXUtils");
    xgi::bind(this,&EmuPeripheralCrate::CrateDumpConfiguration, "CrateDumpConfiguration");
    xgi::bind(this,&EmuPeripheralCrate::CreateMonitorUnit, "CreateMonitorUnit");
    xgi::bind(this,&EmuPeripheralCrate::MonitorFrameLeft, "MonitorFrameLeft");
    xgi::bind(this,&EmuPeripheralCrate::MonitorFrameRight, "MonitorFrameRight");
    xgi::bind(this,&EmuPeripheralCrate::ResetAllCounters, "ResetAllCounters");

    xoap::bind(this,&EmuPeripheralCrate::ReadAllVmePromUserid, "ReadVmePromUserid", XDAQ_NS_URI);
    xoap::bind(this,&EmuPeripheralCrate::LoadAllVmePromUserid, "LoadVmePromUserid", XDAQ_NS_URI);
    xoap::bind(this,&EmuPeripheralCrate::ReadAllCfebPromUserid, "ReadCfebPromUserid", XDAQ_NS_URI);
    xoap::bind(this,&EmuPeripheralCrate::LoadAllCfebPromUserid, "LoadCfebPromUserid", XDAQ_NS_URI);

    //
    //
    // SOAP call-back functions, which relays to *Action method.
    xoap::bind(this, &EmuPeripheralCrate::onConfigure, "Configure", XDAQ_NS_URI);
    xoap::bind(this, &EmuPeripheralCrate::onEnable,    "Enable",    XDAQ_NS_URI);
    xoap::bind(this, &EmuPeripheralCrate::onDisable,   "Disable",   XDAQ_NS_URI);
    xoap::bind(this, &EmuPeripheralCrate::onHalt,      "Halt",      XDAQ_NS_URI);
    //
    xoap::bind(this, &EmuPeripheralCrate::onCalibration,"Calibration",XDAQ_NS_URI);
    //
    // fsm_ is defined in EmuApplication
    fsm_.addState('H', "Halted",     this, &EmuPeripheralCrate::stateChanged);
    fsm_.addState('C', "Configured", this, &EmuPeripheralCrate::stateChanged);
    fsm_.addState('E', "Enabled",    this, &EmuPeripheralCrate::stateChanged);
    //
    fsm_.addStateTransition(
			    'H', 'C', "Configure", this, &EmuPeripheralCrate::configureAction);
    fsm_.addStateTransition(
			    'C', 'C', "Configure", this, &EmuPeripheralCrate::reConfigureAction);
    fsm_.addStateTransition(
			    'C', 'E', "Enable",    this, &EmuPeripheralCrate::enableAction);
    fsm_.addStateTransition(
			    'E', 'E', "Enable",    this, &EmuPeripheralCrate::enableAction);
    fsm_.addStateTransition(
			    'E', 'C', "Disable",   this, &EmuPeripheralCrate::disableAction);
    fsm_.addStateTransition(
			    'C', 'H', "Halt",      this, &EmuPeripheralCrate::haltAction);
    fsm_.addStateTransition(
			    'E', 'H', "Halt",      this, &EmuPeripheralCrate::haltAction);
    fsm_.addStateTransition(
			    'H', 'H', "Halt",      this, &EmuPeripheralCrate::haltAction);
    //
    fsm_.setInitialState('H');
    fsm_.reset();    
    //
    getApplicationInfoSpace()->fireItemAvailable("Calibtype", &CalibType_);
    getApplicationInfoSpace()->fireItemAvailable("Calibnumber", &CalibNumber_);

    // state_ is defined in EmuApplication
    state_ = fsm_.getStateName(fsm_.getCurrentState());
    //
    myParameter_ =  0;
    //
    xmlFile_     = 
      "config.xml" ;
    //
    for(unsigned int dmb=0; dmb<9; dmb++) {
      L1aLctCounter_.push_back(0);
      CfebDavCounter_.push_back(0);
      TmbDavCounter_.push_back(0);
      AlctDavCounter_.push_back(0);
    }
    //
    TMBRegisterValue_ = -1;
    CCBRegisterValue_ = -1;
    Operator_ = "Operator";
    RunNumber_= "-1";
    CalibrationState_ = "None";
    MPCBoardID_ = "-2";
    CCBBoardID_ = "-2";
    ControllerBoardID_ = "-2";
    for (int i=0; i<9; i++) { DMBBoardID_[i] = "-2" ; TMBBoardID_[i] = "-2" ; RATBoardID_[i] = "-2" ;}
    for (int i=0; i<9; i++) 
      for (int j=0; j<5; j++)
	CFEBid_[i][j] = -2;
    CrateChassisID_ = "-2";
    CrateRegulatorBoardID_ = "-2";
    PeripheralCrateMotherBoardID_ = "-2";
    ELMBID_ = "-2";
    BackplaneID_ = "-2";
    //
    for(int i=0; i<9;i++) {
      OutputStringDMBStatus[i] << "DMB-CFEB Status " << i << " output:" << std::endl;
      OutputStringTMBStatus[i] << "TMB-RAT Status " << i << " output:" << std::endl;
      OutputDMBTests[i]        << "DMB-CFEB Tests " << i << " output:" << std::endl;
      OutputTMBTests[i]        << "TMB-RAT Tests " << i << " output:" << std::endl;
      CrateTestsOutput[i]      << "Chamber-Crate Phases " << i << " output:" << std::endl;
    }
    //
    this->getApplicationInfoSpace()->fireItemAvailable("runNumber", &runNumber_);
    this->getApplicationInfoSpace()->fireItemAvailable("xmlFileName", &xmlFile_);
    this->getApplicationInfoSpace()->fireItemAvailable("CalibrationState", &CalibrationState_);
    //
    curlCommand_  = "curl -v";
    curlCookies_  = ".curlCookies";
    CMSUserFile_  = "/home/meydev/.CMSUserFile";
    eLogUserFile_ = ".eLogUserFile";
    eLogURL_      = "https://cmsdaq.cern.ch/elog/CSC";
    //
    this->getApplicationInfoSpace()->fireItemAvailable( "curlCommand",  &curlCommand_  );
    this->getApplicationInfoSpace()->fireItemAvailable( "curlCookies", 	&curlCookies_  );
    this->getApplicationInfoSpace()->fireItemAvailable( "CMSUserFile", 	&CMSUserFile_  );
    this->getApplicationInfoSpace()->fireItemAvailable( "eLogUserFile",	&eLogUserFile_ );
    this->getApplicationInfoSpace()->fireItemAvailable( "eLogURL",     	&eLogURL_      );
    //
    // Create/Retrieve an infospace
    xdata::InfoSpace * is =xdata::InfoSpace::get("urn:xdaq-monitorable:EmuPeripheralCrateData");
    //
    is->fireItemAvailable("myCounter", &myCounter_);
    // attach listener to myCounter_ to detect retrieval event
    is->addItemRetrieveListener ("myCounter", this);
    //
    }
  //
  void EmuPeripheralCrate::actionPerformed (xdata::Event& e)  
     {
       if (e.type() == "ItemRetrieveEvent")
	 {
	   xdata::InfoSpace * is = xdata::InfoSpace::get("urn:xdaq-monitorable:EmuPeripheralCrateData");
	   is->lock();
	   std::string item = dynamic_cast<xdata::ItemRetrieveEvent&>(e).itemName();
	   if ( item == "myCounter")
	     myCounter_ = "meydev:Done" ;
	     std::cout << "Getting myCounter" << std::endl;
	   is->unlock();
	 }
     }     
  //
  void EmuPeripheralCrate::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
    {
      *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/"
	   <<getApplicationDescriptor()->getURN()<<"/"<<"MainPage"<<"\">" <<endl;
    }
  //
  void EmuPeripheralCrate::MainPage(xgi::Input * in, xgi::Output * out ){
    //
    std::string LoggerName = getApplicationLogger().getName() ;
    std::cout << "Name of Logger is " <<  LoggerName <<std::endl;
    //
    //if (getApplicationLogger().exists(getApplicationLogger().getName())) {
    //
    LOG4CPLUS_INFO(getApplicationLogger(), "EmuPeripheralCrate ready");
    //
    //}
    //
    if ( MyController == 0 ) MyController = new EmuController();
    //
    MyHeader(in,out,"EmuPeripheralCrate");
    //
    //
    //*out << cgicc::h1("EmuPeripheralCrate");
    //*out << cgicc::br();
    //
    //std::cout << "The xmlfile is " << xmlFile_.toString() << std::endl;
    //
    //if (tmbVector.size()==0 && dmbVector.size()==0) {
    //
    *out << cgicc::span().set("style","color:blue");
    *out << cgicc::h3(cgicc::b(cgicc::i("Current configuration filename : "))) ;
    *out << cgicc::span();
    //
    *out << xmlFile_.toString() << std::endl ;
    //
    *out << cgicc::br();
    *out << cgicc::br();
    //
    std::string DefineConfiguration =
      toolbox::toString("/%s/DefineConfiguration",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DefineConfiguration) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Define Configuration") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    std::cout << tmbVector.size() << std::endl;
    //
    if (tmbVector.size()>0 || dmbVector.size()>0) {
      //
      if ( Operator_.find("NameOld",0) == string::npos ) {
	//
	// Crate Configuration
	//
	std::string CrateConfiguration =
	  toolbox::toString("/%s/CrateConfiguration",getApplicationDescriptor()->getURN().c_str());
	//
	*out << cgicc::a("[Crate Configuration]").set("href",CrateConfiguration) << endl;
	//
	std::string CrateXUtils =
	  toolbox::toString("/%s/CrateXUtils",getApplicationDescriptor()->getURN().c_str());
	//
	*out << cgicc::a("[Crate Utils]").set("href",CrateXUtils) << endl;
	//
	std::string CrateTests =
	  toolbox::toString("/%s/CrateTests",getApplicationDescriptor()->getURN().c_str());
	//
	*out << cgicc::a("[Crate Tests]").set("href",CrateTests) << endl;
	//
	std::string CalibrationRuns =
	  toolbox::toString("/%s/CalibrationRuns",getApplicationDescriptor()->getURN().c_str());
	//
	*out << cgicc::a("[Calibration Runs]").set("href",CalibrationRuns) << endl;
	//
      }
      //
      *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial; background-color:yellow");
      *out << std::endl;
      *out << cgicc::legend((("Initialisation"))).set("style","color:blue") ;
      //
      std::string InitSystem =
	toolbox::toString("/%s/InitSystem",getApplicationDescriptor()->getURN().c_str());
      //
      *out << cgicc::form().set("method","GET").set("action",InitSystem) << std::endl ;
      *out << cgicc::input().set("type","submit").set("value","Init System") << std::endl ;
      *out << cgicc::form() << std::endl ;
      //
      *out << cgicc::table().set("border","0");
      //
      *out << cgicc::td();
      //
      std::string LaunchMonitor =
	toolbox::toString("/%s/LaunchMonitor",getApplicationDescriptor()->getURN().c_str());
      //
      *out << cgicc::form().set("method","GET").set("action",LaunchMonitor).set("target","_blank") << std::endl ;
      *out << cgicc::input().set("type","submit")
	.set("value","Launch Monitor").set("name","LaunchMonitor") << std::endl ;
      *out << cgicc::form() << std::endl ;
      //
      *out << cgicc::td();
      //
      *out << cgicc::td();
      //
      std::string CrateTMBCounters =
	toolbox::toString("/%s/CrateTMBCounters",getApplicationDescriptor()->getURN().c_str());
      //
      *out << cgicc::form().set("method","GET").set("action",CrateTMBCounters).set("target","_blank") << std::endl ;
      *out << cgicc::input().set("type","submit")
	.set("value","Crate TMB counters").set("name","CrateTMBCounters") << std::endl ;
      *out << cgicc::form() << std::endl ;
      //
      *out << cgicc::td();
      //
      *out << cgicc::td();
      //
      std::string CrateDMBCounters =
	toolbox::toString("/%s/CrateDMBCounters",getApplicationDescriptor()->getURN().c_str());
      //
      *out << cgicc::form().set("method","GET").set("action",CrateDMBCounters).set("target","_blank") << std::endl ;
      *out << cgicc::input().set("type","submit")
	.set("value","Crate DMB counters").set("name","CrateDMBCounters") << std::endl ;
      *out << cgicc::form() << std::endl ;
      //
      *out << cgicc::td();
      //
      *out << cgicc::td();
      //
      std::string CrateStatus =
	toolbox::toString("/%s/CrateStatus",getApplicationDescriptor()->getURN().c_str());
      //
      *out << cgicc::form().set("method","GET").set("action",CrateStatus).set("target","_blank") << std::endl ;
      *out << cgicc::input().set("type","submit")
	.set("value","Crate Status").set("name","CrateStatus") << std::endl ;
      *out << cgicc::form() << std::endl ;
      //
      *out << cgicc::td();
      //
      *out << cgicc::table();
      //
      std::string Operator =
	toolbox::toString("/%s/Operator",getApplicationDescriptor()->getURN().c_str());
      //
      *out << cgicc::form().set("method","GET").set("action",Operator) << std::endl ;
      *out << cgicc::input().set("type","text").set("name","Operator")
	.set("size","20").set("value",Operator_) << std::endl ;
      *out << "Operator" << std::endl;
      *out << cgicc::form() << std::endl ;
      //
      std::string RunNumber =
	toolbox::toString("/%s/RunNumber",getApplicationDescriptor()->getURN().c_str());
      //
      *out << cgicc::form().set("method","GET").set("action",RunNumber) << std::endl ;
      *out << cgicc::input().set("type","text").set("name","RunNumber")
	.set("size","20").set("value",RunNumber_) << std::endl ;
      *out << "RunNumber" << std::endl;
      *out << cgicc::form() << std::endl ;
      //
      std::string LogOutput =
	toolbox::toString("/%s/LogOutput",getApplicationDescriptor()->getURN().c_str());
      //
      *out << cgicc::form().set("method","GET").set("action",LogOutput) << std::endl ;
      *out << cgicc::input().set("type","submit")
	.set("value","Log all output").set("name","LogOutput") << std::endl ;
      *out << cgicc::form() << std::endl ;
      //
      std::string LogTestSummary =
	toolbox::toString("/%s/LogTestSummary",getApplicationDescriptor()->getURN().c_str());
      //
      *out << cgicc::form().set("method","GET").set("action",LogTestSummary) << std::endl ;
      *out << cgicc::input().set("type","submit")
	.set("value","Log Test Summary").set("name","LogTestSummary") << std::endl ;
      *out << cgicc::form() << std::endl ;
      //
      //
      //std::string PowerUp =
      //toolbox::toString("/%s/PowerUp",getApplicationDescriptor()->getURN().c_str());
      //
      //*out << cgicc::form().set("method","GET").set("action","http://emuslice03:1973/urn:xdaq-application:lid=30/")
      //.set("target","_blank") << std::endl ;
      //*out << cgicc::input().set("type","submit").set("value","Power Up") << std::endl ;
      //*out << cgicc::form() << std::endl ;
      //
      //
      *out << cgicc::fieldset();
      //
      *out << std::endl;
      //
    }
    //
    //cout << "Here4" << endl ;
    //
  }
  //
  //
  // SOAP Callback  
  //
  //
  //#ifndef STANDALONE
  //
  // SOAP Callback  
  //
  //
  xoap::MessageReference EmuPeripheralCrate::onCalibration(xoap::MessageReference message) throw (xoap::exception::Exception)
    {
      //
      LOG4CPLUS_INFO(getApplicationLogger(), "Calibration");
      //
      //ostringstream test;
      //message->writeTo(test);
      //
      //string setting = extractCalibrationSetting(message);
      //
      //LOG4CPLUS_INFO(getApplicationLogger(), test.str());
      //LOG4CPLUS_INFO(getApplicationLogger(), "Next");
      //LOG4CPLUS_INFO(getApplicationLogger(), setting);
      //LOG4CPLUS_INFO(getApplicationLogger(), "Done");
      //
      //
      //CalibrationState_ = "Busy";
      //
      // Do something
      //
      //LOG4CPLUS_INFO(getApplicationLogger(), setting);
      //
      ::sleep(1);
      //
      cout<<"soap Apr.11, 2007 "<<endl;
      cout<<"Entered the EMUPERIPHERALCRATE.cc "<<endl;
      printf(" LSD: Entered Calibration \n"); 
       ostringstream test;
       message->writeTo(test);
       cout << test.str() << endl;
        printf(" Print calibtype \n");
	std::string junk = CalibType_;
        cout << junk << endl;
        printf(" Print calibnumber \n");
        cout << CalibNumber_ << endl;

	//CalibrationState_ = setting;
      //
      return createReply(message);
    }
  //
  xoap::MessageReference EmuPeripheralCrate::onConfigure (xoap::MessageReference message) throw (xoap::exception::Exception)
  {
    //
    fireEvent("Configure");
    //
    return createReply(message);
  }

  //
  xoap::MessageReference EmuPeripheralCrate::onEnable (xoap::MessageReference message) throw (xoap::exception::Exception)
  {
    fireEvent("Enable");

    return createReply(message);
  }

  //
  xoap::MessageReference EmuPeripheralCrate::onDisable (xoap::MessageReference message) throw (xoap::exception::Exception)
  {
    fireEvent("Disable");

    return createReply(message);
  }

  //
  xoap::MessageReference EmuPeripheralCrate::onHalt (xoap::MessageReference message) throw (xoap::exception::Exception)
  {
    fireEvent("Halt");

    return createReply(message);
  }

  void EmuPeripheralCrate::configureAction(toolbox::Event::Reference e) 
    throw (toolbox::fsm::exception::Exception)
  {
    //
    printf(" LSD: comment out ConfigureInit for now \n");

    ConfigureInit();
    //
    std::cout << "Configure" << std::endl ;
    LOG4CPLUS_INFO(getApplicationLogger(), "Configure");
    //
    std::cout << xmlFile_.toString() << std::endl;
    LOG4CPLUS_INFO(getApplicationLogger(), xmlFile_.toString());
    //
    std::cout << "Received Message Configure" << std::endl ;
    LOG4CPLUS_INFO(getApplicationLogger(), "Received Message Configure");
    //
    std::cout << "Done!" << std::endl;
    //
  }
  //
  void EmuPeripheralCrate::configureFail(toolbox::Event::Reference e) 
    throw (toolbox::fsm::exception::Exception)
    {
    //
    LOG4CPLUS_INFO(getApplicationLogger(), "Failed");
    //
  }
  //
  void EmuPeripheralCrate::reConfigureAction(toolbox::Event::Reference e) 
    throw (toolbox::fsm::exception::Exception)
  {
    //
    MyController->configure();
    //
    std::cout << "reConfigure" << std::endl ;
    LOG4CPLUS_INFO(getApplicationLogger(), "reConfigure");
    //
  }
  //
  void EmuPeripheralCrate::postToELog( string subject, string body ){
    // Post to eLog:
    //
    std::cout << "Post to elog" << std::endl;
    //
    EmuELog *eel;
    try
      {
	eel = new EmuELog(curlCommand_.toString(),
			  curlCookies_.toString(),
			  CMSUserFile_.toString(),
			  eLogUserFile_.toString(),
			  eLogURL_.toString());
      }
    catch( string e ){
      eel = 0;
      std::cout << "failed " << e << std::endl;
    }
    if ( eel ) {
      std::cout << "PostToElog" << std::endl;
      eel->postMessage( subject, body );
    }
    delete eel;
  }
  // 
  void EmuPeripheralCrate::enableAction(toolbox::Event::Reference e) 
    throw (toolbox::fsm::exception::Exception)
    {
    //
    //MyController->init();
    //
    //MyController->configure();
    //
    std::cout << "Received Message Enable" << std::endl ;
    LOG4CPLUS_INFO(getApplicationLogger(), "Received Message Enable");
  }

  //
  void EmuPeripheralCrate::disableAction(toolbox::Event::Reference e) 
    throw (toolbox::fsm::exception::Exception)
    {
      // do nothing
      std::cout << "Received Message Disable" << std::endl ;
      LOG4CPLUS_INFO(getApplicationLogger(), "Received Message Disable");
    }  
  //
  void EmuPeripheralCrate::haltAction(toolbox::Event::Reference e) 
    throw (toolbox::fsm::exception::Exception)
  {
    // do nothing
    std::cout << "Received Message Halt" << std::endl ;
    LOG4CPLUS_INFO(getApplicationLogger(), "Received Message Halt");
  }  
  //
  void EmuPeripheralCrate::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
    throw (toolbox::fsm::exception::Exception)
    {
      EmuApplication::stateChanged(fsm);
    }
  //
  void EmuPeripheralCrate::MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) 
    throw (xgi::exception::Exception)
  {
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    //
    //*out << cgicc::title(title) << std::endl;
    //*out << "<a href=\"/\"><img border=\"0\" src=\"/daq/xgi/images/XDAQLogo.gif\" title=\"XDAQ\" alt=\"\" style=\"width: 145px; height: 89px;\"></a>" << std::endl;
    //
    std::string myUrn = getApplicationDescriptor()->getURN().c_str();
    xgi::Utils::getPageHeader(out,title,myUrn,"","");
    //
  }
  //
  void EmuPeripheralCrate::CrateTests(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    MyHeader(in,out,"CrateTests");
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << cgicc::legend("Crate Tests").set("style","color:blue") ;
    //
    std::string TmbMPCTest =
      toolbox::toString("/%s/TmbMPCTest",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",TmbMPCTest) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Crate TMB/MPC test") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    if (myCrateTest.GetMpcTMBTestResult() == -1 ) {
      *out << "Not tested yet" <<std::endl;
    } else if (myCrateTest.GetMpcTMBTestResult() == 0 ) {
      *out << "Failed" <<std::endl;
    } else {
      *out << "Passed" <<std::endl;
    }
    //
    *out << cgicc::fieldset();
    //
  }
  //
  void EmuPeripheralCrate::DefineConfiguration(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    MyHeader(in,out,"Define Configuration");
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend("Upload Configuration...").set("style","color:blue") 
	 << cgicc::p() << std::endl ;
    //
    std::string method =
      toolbox::toString("/%s/setConfFile",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","POST").set("action",method) << std::endl ;
    *out << cgicc::input().set("type","text")
      .set("name","xmlFilename")
      .set("size","90")
      .set("ENCTYPE","multipart/form-data")
      .set("value",xmlFile_);
    //
    *out << std::endl;
    //
    *out << cgicc::input().set("type","submit")
      .set("value","Set configuration file local") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    // Upload file...
    //
    std::string methodUpload =
      toolbox::toString("/%s/UploadConfFile",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","POST")
      .set("enctype","multipart/form-data")
      .set("action",methodUpload) << std::endl ;
    //
    *out << cgicc::input().set("type","file")
      .set("name","xmlFilenameUpload")
      .set("size","90") ;
    //
    *out << std::endl;
    //
    *out << cgicc::input().set("type","submit").set("value","Send") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    *out << std::endl;
    //
    std::string methodRaw =
      toolbox::toString("/%s/setRawConfFile",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","POST").set("action",methodRaw) << std::endl ;
    *out << cgicc::textarea().set("name","Text")
      .set("WRAP","OFF")
      .set("rows","10").set("cols","90");
    *out << "Paste configuration..." << endl ;
    *out << cgicc::textarea();
    *out << cgicc::input().set("type","submit").set("value","Send");
    *out << cgicc::form() << std::endl ;    
    //
    *out << std::endl;
    //    
    std::string getTestLogFile =
      toolbox::toString("/%s/getTestLogFile",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","POST").set("action",getTestLogFile) << std::endl ;
    *out << cgicc::input().set("type","text")
      .set("name","TestLogFile")
      .set("size","90")
      .set("ENCTYPE","multipart/form-data")
      .set("value",TestLogFile_);
    //
    *out << cgicc::input().set("type","submit")
      .set("value","Restore Test configuration") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    std::string getTestLogFileUpload =
      toolbox::toString("/%s/getTestLogFileUpload",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","POST")
      .set("enctype","multipart/form-data")
      .set("action",getTestLogFileUpload) << std::endl ;
    //
    *out << cgicc::input().set("type","file")
      .set("name","TestFileUpload")
      .set("size","90") ;
    //
    *out << std::endl;
    //
    *out << cgicc::input().set("type","submit").set("value","Send Test file") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    *out << std::endl;
    //
    *out << cgicc::fieldset();
    *out << std::endl;
    //
  }
  //
  void EmuPeripheralCrate::CalibrationRuns(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    MyHeader(in,out,"CalibrationRuns");
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << cgicc::legend("Calibration Runs").set("style","color:blue") ;
    //
    std::string CalibrationCFEBXtalk =
      toolbox::toString("/%s/CalibrationCFEBXtalk",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",CalibrationCFEBXtalk) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Calibration Strips : CFEB Xtalk") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    std::string CalibrationCFEBGain =
      toolbox::toString("/%s/CalibrationCFEBGain",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",CalibrationCFEBGain) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Calibration Strips : CFEB Gain") 
	 << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    std::string CalibrationCFEBPedestal =
      toolbox::toString("/%s/CalibrationCFEBPedestal",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",CalibrationCFEBPedestal) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Calibration Strips CFEB Pedestal") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    std::string CalibrationCFEBSaturation =
      toolbox::toString("/%s/CalibrationSaturation",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",CalibrationCFEBSaturation) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Calibration Strips : CFEB high-end amplifier saturation") 
	 << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    //
    std::string CalibrationALCTThresholdScan =
      toolbox::toString("/%s/CalibrationALCTThresholdScan",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",CalibrationALCTThresholdScan) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Calibration ALCT : Threshold Scan") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    std::string CalibrationALCTConnectivity =
      toolbox::toString("/%s/CalibrationALCTConnectivity",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",CalibrationALCTConnectivity) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Calibration ALCT : Connectivity") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    std::string CalibrationCFEBConnectivity =
      toolbox::toString("/%s/CalibrationCFEBConnectivity",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",CalibrationCFEBConnectivity) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Calibration CFEB : Connectivity") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    std::string CalibrationComparatorPulse =
      toolbox::toString("/%s/CalibrationComparatorPulse",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",CalibrationComparatorPulse) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Calibration Comparator Pulse") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::fieldset();
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << cgicc::legend("Calibration tools").set("style","color:blue") ;
    //
    std::string FindLv1aDelayComparator =
      toolbox::toString("/%s/FindLv1aDelayComparator",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",FindLv1aDelayComparator) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Find l1a delay for Comparator") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    std::string FindLv1aDelayALCT =
      toolbox::toString("/%s/FindLv1aDelayALCT",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",FindLv1aDelayALCT) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Find l1a delay for ALCT") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::fieldset();
    //
  }
  //
  void EmuPeripheralCrate::CrateConfiguration(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
    {
      //
      MyHeader(in,out,"CrateConfiguration");
      //
      *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial; background-color:#00FF00");
      //*out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial");
      *out << std::endl;
      //
      *out << cgicc::legend("Crate Configuration...").set("style","color:blue") << 
	cgicc::p() << std::endl ;
      //
      //*out << cgicc::body().set("bgcolor=yellow");
      //
      for(int ii=1; ii<28; ii++) {
	//
	*out << cgicc::table().set("border","1");
	//
	*out << cgicc::td();
	//
	*out << "Slot " << setfill('0') << setw(2) << dec << ii << endl;
	//
	*out << cgicc::td();
      //
	if(ii==1) {
	  //
	  char Name[50] ;
	  std::string ControllerBoardID =
	    toolbox::toString("/%s/ControllerBoardID",getApplicationDescriptor()->getURN().c_str());
	  sprintf(Name,"Controller Status");
	  //
	  *out << cgicc::td();
	  *out << "Controller Board ID" ;
	  *out << cgicc::form().set("method","GET").set("action",ControllerBoardID) << std::endl ;
	  *out << cgicc::input().set("type","text").set("name","ControllerBoardID")
	    .set("value",ControllerBoardID_) << std::endl ;
	  *out << cgicc::form() << std::endl ;
	  *out << cgicc::td();
	  //
	  *out << cgicc::td();
	  //
	  std::string ControllerUtils =
	    toolbox::toString("/%s/ControllerUtils?ccb=%d",getApplicationDescriptor()->getURN().c_str(),ii);
	  //
	  *out << cgicc::a("Controller Utils").set("href",ControllerUtils) << endl;
	  //
	  *out << cgicc::td();
	  //
	}
	//
	int slot = thisCrate->ccb()->slot() ;
	//
	if(slot == ii) {
	  //
	  //
	  char Name[50] ;
	  std::string CCBBoardID =
	    toolbox::toString("/%s/CCBBoardID",getApplicationDescriptor()->getURN().c_str());
	  sprintf(Name,"CCB Status slot=%d",slot);
	  //
	  *out << cgicc::td();
	  *out << "CCB Board ID" ;
	  *out << cgicc::form().set("method","GET").set("action",CCBBoardID) << std::endl ;
	  *out << cgicc::input().set("type","text").set("name","CCBBoardID")
	    .set("value",CCBBoardID_) << std::endl ;
	  *out << cgicc::form() << std::endl ;
	  *out << cgicc::td();
	  //
	  *out << cgicc::td();
	  //
	  if ( CCBBoardID_.find("-1") == string::npos ) {
	  //
	  //*out << cgicc::form().set("method","GET").set("action",CCBStatus)
	  //.set("target","_blank") << std::endl ;
	  //*out << cgicc::input().set("type","submit").set("value",Name) << std::endl ;
	  //char buf[20];
	  //sprintf(buf,"%d",ii);
	  //*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ccb");
	  //*out << cgicc::form() << std::endl ;
	  //
	  std::string CCBStatus =
	    toolbox::toString("/%s/CCBStatus?ccb=%d",getApplicationDescriptor()->getURN().c_str(),ii);
	  //
	  *out << cgicc::a("CCB Status").set("href",CCBStatus) << endl;
	  //
	}
	*out << cgicc::td();
	//
	*out << cgicc::td();
	//
	if ( CCBBoardID_.find("-1") == string::npos ) {
	  //
	  sprintf(Name,"CCB Utils slot=%d",slot);
	  //
	  //*out << cgicc::form().set("method","GET").set("action",CCBUtils)
	  //.set("target","_blank") << std::endl ;
	  //*out << cgicc::input().set("type","submit").set("value",Name) << std::endl ;
	  //char buf[20];
	  //sprintf(buf,"%d",ii);
	  //*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ccb");
	  //*out << cgicc::form() << std::endl ;
	  //
	  std::string CCBUtils =
	    toolbox::toString("/%s/CCBUtils?ccb=%d",getApplicationDescriptor()->getURN().c_str(),ii);
	  //
	  *out << cgicc::a("CCB Utils").set("href",CCBUtils) << endl;
	  //
	}
	*out << cgicc::td();
	//
      }
      std::string MPCStatus =
	toolbox::toString("/%s/MPCStatus",getApplicationDescriptor()->getURN().c_str());
      std::string MPCBoardID =
	toolbox::toString("/%s/MPCBoardID",getApplicationDescriptor()->getURN().c_str());
      slot = -1;
      if ( thisMPC ) slot = thisCrate->mpc()->slot() ;
      char Name[50] ;
      sprintf(Name,"MPC Status slot=%d",slot);
      if(slot == ii) {
	//
	*out << cgicc::td();
	*out << "MPC Board ID" ;
	*out << cgicc::form().set("method","GET").set("action",MPCBoardID) << std::endl ;
	*out << cgicc::input().set("type","text").set("name","MPCBoardID")
	  .set("value",MPCBoardID_) << std::endl ;
	*out << cgicc::form() << std::endl ;
	*out << cgicc::td();
	//
	if ( MPCBoardID_.find("-1") == string::npos && thisMPC ) {
 	  *out << cgicc::td();
	  std::string MPCStatus =
	    toolbox::toString("/%s/MPCStatus?mpc=%d",getApplicationDescriptor()->getURN().c_str(),ii);
	  //
	  *out << cgicc::a("MPC Status").set("href",MPCStatus) << endl;
	  //
          *out << cgicc::td();
	  //
          *out << cgicc::td();
 	  std::string MPCUtils =
	    toolbox::toString("/%s/MPCUtils?mpc=%d",getApplicationDescriptor()->getURN().c_str(),ii);
	  //
	  *out << cgicc::a("MPC Utils").set("href",MPCUtils) << endl;
	  //
	  *out << cgicc::td();
	}
      }
      //
      std::string TMBStatus ;
      std::string TMBTests ;
      std::string TMBUtils ;
      std::string TMBBoardID ;
      std::string RATBoardID ;
      //
      TMBStatus  =
	toolbox::toString("/%s/TMBStatus",getApplicationDescriptor()->getURN().c_str());
      TMBBoardID =
	toolbox::toString("/%s/TMBBoardID",getApplicationDescriptor()->getURN().c_str());
      RATBoardID =
	toolbox::toString("/%s/RATBoardID",getApplicationDescriptor()->getURN().c_str());
      TMBTests   =
	toolbox::toString("/%s/TMBTests",getApplicationDescriptor()->getURN().c_str());
      TMBUtils   =
	toolbox::toString("/%s/TMBUtils",getApplicationDescriptor()->getURN().c_str());
      //
      for (unsigned int i=0; i<tmbVector.size(); i++) {
	//
	int slot = tmbVector[i]->slot();
	if(slot == ii) {
	  //
	  *out << cgicc::td();
	  *out << "TMB Board ID" ;
	  *out << cgicc::form().set("method","GET").set("action",TMBBoardID) << std::endl ;
	  char buf[20];
	  sprintf(buf,"TMBBoardID_%d",i);
	  *out << cgicc::input().set("type","text").set("name",buf)
	    .set("value",TMBBoardID_[i]) 
	    .set("size","10")
	       << std::endl ;
	  sprintf(buf,"%d",i);
	  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
	  *out << cgicc::form() << std::endl ;
	  *out << cgicc::td();
	  //
	  *out << cgicc::td();
	  *out << "RAT Board ID" ;
	  *out << cgicc::form().set("method","GET").set("action",RATBoardID) << std::endl ;
	  //
	  sprintf(buf,"RATBoardID_%d",i);
	  *out << cgicc::input().set("type","text").set("name",buf)
	    .set("value",RATBoardID_[i]) 
	    .set("size","10")
	       << std::endl ;
	  sprintf(buf,"%d",i);
	  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","rat");
	  *out << cgicc::form() << std::endl ;
	  *out << cgicc::td();
	  //
	  //
	  sprintf(Name,"TMB Status slot=%d",tmbVector[i]->slot());	
	  *out << cgicc::td();
	  if ( TMBBoardID_[i].find("-1") == string::npos ) {
	    //
	    //*out << cgicc::form().set("method","GET").set("action",TMBStatus)
	    //.set("target","_blank") << std::endl ;
	    //*out << cgicc::input().set("type","submit").set("value",Name) << std::endl ;
	    //sprintf(buf,"%d",i);
	    //*out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
	    //*out << cgicc::form() << std::endl ;
	    //
	    std::string MonitorTMBTrigger =
	      toolbox::toString("/%s/MonitorTMBTrigger?tmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	    *out << cgicc::a("Monitor TMB trigger").set("href",MonitorTMBTrigger) << endl;
	    //
	    *out << cgicc::td();
	    //
	    *out << cgicc::td();
	    //
	    std::string TMBStatus =
	      toolbox::toString("/%s/TMBStatus?tmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	    *out << cgicc::a("TMB Status").set("href",TMBStatus) << endl;
	    //
	  }
	  *out << cgicc::td();
	  //
	  sprintf(Name,"TMB Tests slot=%d",tmbVector[i]->slot());	  
	  *out << cgicc::td();
	  if ( TMBBoardID_[i].find("-1") == string::npos ) {
	    //
	    //*out << cgicc::form().set("method","GET").set("action",TMBTests)
	    //.set("target","_blank") << std::endl ;
	    //*out << cgicc::input().set("type","submit").set("value",Name) << std::endl ;
	    //sprintf(buf,"%d",i);
	    //*out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
	    //*out << cgicc::form() << std::endl ;
	    //
	    std::string TMBTests =
	      toolbox::toString("/%s/TMBTests?tmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	    //
	    *out << cgicc::a("TMB Tests").set("href",TMBTests) << endl;
	    //
	  }
	  *out << cgicc::td();
	  //
	  sprintf(Name,"TMB Utils slot=%d",tmbVector[i]->slot());	  
	  *out << cgicc::td();
	  if ( TMBBoardID_[i].find("-1") == string::npos ) {
	    //
	    //*out << cgicc::form().set("method","GET").set("action",TMBUtils)
	    //.set("target","_blank") << std::endl ;
	    //*out << cgicc::input().set("type","submit").set("value",Name) << std::endl ;
	    //sprintf(buf,"%d",i);
	    //*out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
	    //*out << cgicc::form() << std::endl ;	    
	    //
	    std::string TMBUtils =
	      toolbox::toString("/%s/TMBUtils?tmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	    //
	    *out << cgicc::a("TMB Utils").set("href",TMBUtils) << endl;
	    //
	  }
	  *out << cgicc::td();
	  //
	  //Found TMB...look for DMB...
	  //
	  for (unsigned int iii=0; iii<dmbVector.size(); iii++) {
	    int dmbslot = dmbVector[iii]->slot();
	    std::string ChamberTests =
	      toolbox::toString("/%s/ChamberTests",getApplicationDescriptor()->getURN().c_str());    
	    sprintf(Name,"Chamber tests TMBslot=%d DMBslot=%d",slot,dmbslot);
	    //
	    if ( dmbslot == slot+1 ) {
	      *out << cgicc::td();
	      if ( TMBBoardID_[i].find("-1") == string::npos ) {
		//
		//*out << cgicc::form().set("method","GET").set("action",ChamberTests)
		//.set("target","_blank") << std::endl ;
		//*out << cgicc::input().set("type","submit").set("value",Name) << std::endl ;
		//char buf[20];
		//sprintf(buf,"%d",i);
		//*out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
		//sprintf(buf,"%d",iii);
		//*out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
		//*out << cgicc::form() << std::endl ;
		//
		std::string ChamberTests =
		  toolbox::toString("/%s/ChamberTests?tmb=%d&dmb=%d",getApplicationDescriptor()->getURN().c_str(),i,iii);
		//
		*out << cgicc::a(Name).set("href",ChamberTests) << endl;
		//
		//std::cout << "Creating ChamberUtils i="<<i<<std::endl;
		//
		MyTest[i].SetTMB(tmbVector[i]);
		MyTest[i].SetDMB(dmbVector[iii]);
		MyTest[i].SetCCB(thisCCB);
		MyTest[i].SetMPC(thisMPC);
		//
	      }
	      *out << cgicc::td();
	    }
	  }
	  //
	}
      }
      //
      std::string DMBStatus;
      std::string DMBTests;
      std::string DMBUtils;
      std::string DMBBoardID;
      //
      for (unsigned int i=0; i<dmbVector.size(); i++) {
	    DMBStatus =
	      toolbox::toString("/%s/DMBStatus",getApplicationDescriptor()->getURN().c_str());
	    DMBTests =
	      toolbox::toString("/%s/DMBTests",getApplicationDescriptor()->getURN().c_str());
	    DMBUtils =
	      toolbox::toString("/%s/DMBUtils",getApplicationDescriptor()->getURN().c_str());
	    DMBBoardID =
	      toolbox::toString("/%s/DMBBoardID",getApplicationDescriptor()->getURN().c_str());
	    int slot = dmbVector[i]->slot();
	    if(slot == ii ) {
	      //
	      *out << cgicc::td();
	      *out << "DMB Board ID" ;
	      *out << cgicc::form().set("method","GET").set("action",DMBBoardID) << std::endl ;
	      char buf[20];
	      sprintf(buf,"DMBBoardID_%d",i);
	      *out << cgicc::input().set("type","text").set("name",buf)
		.set("value",DMBBoardID_[i])
		.set("size","10")
		   << std::endl ;
	      sprintf(buf,"%d",i);
	      *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
	      *out << cgicc::form() << std::endl ;
	      *out << cgicc::td();
	      //
	      sprintf(Name,"DMB Status slot=%d",dmbVector[i]->slot());
	      *out << cgicc::td();
	      if ( DMBBoardID_[i].find("-1",0) == string::npos ) {
		//
		//*out << cgicc::form().set("method","GET").set("action",DMBStatus)
		//.set("target","_blank") << std::endl ;
		//*out << cgicc::input().set("type","submit").set("value",Name) << std::endl ;
		//sprintf(buf,"%d",i);
		//*out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
		//*out << cgicc::form() << std::endl ;
		//
		std::string DMBStatus =
		  toolbox::toString("/%s/DMBStatus?dmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
		//
		*out << cgicc::a("DMB Status").set("href",DMBStatus) << endl;
		//
	      }
	      *out << cgicc::td();
	      //
	      sprintf(Name,"DMB Tests  slot=%d",dmbVector[i]->slot());
	      //
	      *out << cgicc::td();
	      if ( DMBBoardID_[i].find("-1",0) == string::npos ) {
		//
		//*out << cgicc::form().set("method","GET").set("action",DMBTests)
		//.set("target","_blank") << std::endl ;
		//*out << cgicc::input().set("type","submit").set("value",Name) << std::endl ;
		//sprintf(buf,"%d",i);
		//*out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
		//*out << cgicc::form() << std::endl ;
		//
		std::string DMBTests =
		  toolbox::toString("/%s/DMBTests?dmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
		//
		*out << cgicc::a("DMB Tests").set("href",DMBTests) << endl;
		//
	      }
	      *out << cgicc::td();
	      //
	      sprintf(Name,"DMB Utils  slot=%d",dmbVector[i]->slot());
	      //
	      *out << cgicc::td();
	      if ( DMBBoardID_[i].find("-1",0) == string::npos ) {
		//*out << cgicc::form().set("method","GET").set("action",DMBUtils)
		//.set("target","_blank") << std::endl ;
		//*out << cgicc::input().set("type","submit").set("value",Name) << std::endl ;
		//sprintf(buf,"%d",i);
		//*out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
		//*out << cgicc::form() << std::endl ;
		//
		std::string DMBUtils =
		  toolbox::toString("/%s/DMBUtils?dmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
		//
		*out << cgicc::a("DMB Utils").set("href",DMBUtils) << endl;
		//
	      }
	      *out << cgicc::td();
	    //
	    }
	  }
	//
	//*out<< cgicc::br() ;
	//
      *out << cgicc::table();
      //
      }
      //
      *out << cgicc::table().set("border","1");
      //
      std::string CrateChassisID =
	toolbox::toString("/%s/CrateChassisID",getApplicationDescriptor()->getURN().c_str());
      //
      *out << cgicc::td();
      *out << "Crate Chassis ID" ;
    *out << cgicc::form().set("method","GET").set("action",CrateChassisID) << std::endl ;
    *out << cgicc::input().set("type","text").set("name","CrateChassisID")
      .set("value",CrateChassisID_) << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    std::string BackplaneID =
      toolbox::toString("/%s/BackplaneID",getApplicationDescriptor()->getURN().c_str());
    //
    *out << "Backplane ID" ;
    *out << cgicc::form().set("method","GET").set("action",BackplaneID) << std::endl ;
    *out << cgicc::input().set("type","text").set("name","BackplaneID")
      .set("value",BackplaneID_) << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    std::string CrateRegulatorBoardID =
      toolbox::toString("/%s/CrateRegulatorBoardID",getApplicationDescriptor()->getURN().c_str());
    //
    *out << "CRB ID" ;
    *out << cgicc::form().set("method","GET").set("action",CrateRegulatorBoardID) << std::endl ;
    *out << cgicc::input().set("type","text").set("name","CrateRegulatorBoardID")
      .set("value",CrateRegulatorBoardID_) << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    std::string PeripheralCrateMotherBoardID =
      toolbox::toString("/%s/PeripheralCrateMotherBoardID",getApplicationDescriptor()->getURN().c_str());
    //
    *out << "PCMB ID" ;
    *out << cgicc::form().set("method","GET").set("action",PeripheralCrateMotherBoardID) << std::endl ;
    *out << cgicc::input().set("type","text").set("name","PeripheralCrateMotherBoardID")
      .set("value",PeripheralCrateMotherBoardID_) << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    std::string ELMBID =
      toolbox::toString("/%s/ELMBID",getApplicationDescriptor()->getURN().c_str());
    //
    *out << "ELMB ID" ;
    *out << cgicc::form().set("method","GET").set("action",ELMBID) << std::endl ;
    *out << cgicc::input().set("type","text").set("name","ELMBID")
      .set("value",ELMBID_) << std::endl ;
    *out << cgicc::form() << std::endl ;
    *out << cgicc::td();
    //
    //
    *out << cgicc::table();
    //
	//
	//*out << cgicc::body();
	*out << cgicc::fieldset();
	//
  }
  //
  void EmuPeripheralCrate::CalibrationALCTThresholdScan(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    CalibDAQ calib(emuSystem_);
    //
    calib.ALCTThresholdScan();
    //
    this->Default(in,out);
    //
  }
  //
  void EmuPeripheralCrate::CalibrationALCTConnectivity(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    CalibDAQ calib(emuSystem_);
    //
    calib.ALCTConnectivity();
    //
    this->Default(in,out);
    //
  }
  //
  void EmuPeripheralCrate::CalibrationCFEBConnectivity(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    CalibDAQ calib(emuSystem_);
    //
    calib.CFEBConnectivity();
    //
    this->Default(in,out);
    //
  }
  //
  void EmuPeripheralCrate::FindLv1aDelayComparator(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    CalibDAQ calib(emuSystem_);
    //
    calib.FindL1aDelayComparator();
    //
    this->Default(in,out);
    //
  }
  //
  void EmuPeripheralCrate::FindLv1aDelayALCT(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    CalibDAQ calib(emuSystem_);
    //
    calib.FindL1aDelayALCT();
    //
    this->Default(in,out);
    //
  }
  //
  void EmuPeripheralCrate::CalibrationCFEBGain(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    CalibDAQ calib(emuSystem_);
    //
    calib.gainCFEB();
    //
    this->Default(in,out);
    //
  }
  //
  void EmuPeripheralCrate::CalibrationCFEBSaturation(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    CalibDAQ calib(emuSystem_);
    //
    calib.CFEBSaturationTest();
    //
    this->Default(in,out);
    //
  }
  //
  void EmuPeripheralCrate::CalibrationCFEBXtalk(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    CalibDAQ calib(emuSystem_);
    //
    calib.timeCFEB();
    //
    this->Default(in,out);
    //
  }
  //
  void EmuPeripheralCrate::CalibrationComparatorPulse(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    CalibDAQ calib(emuSystem_);
    //
    calib.pulseComparatorPulse();
    //
    this->Default(in,out);
    //
  }
  //
  void EmuPeripheralCrate::CalibrationCFEBPedestal(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    CalibDAQ calib(emuSystem_);
    //
    //calib.loadConstants();
    //
    //int nsleep, nstrip, tries;
    //float dac;
    //
    calib.pedestalCFEB();
    //
    this->Default(in,out);
    //
  }
  //
  void EmuPeripheralCrate::CrateXUtils(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    MyHeader(in,out,"Crate Utils");
    //
    std::string CrateDumpConfiguration =
      toolbox::toString("/%s/CrateDumpConfiguration",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",CrateDumpConfiguration) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Dump Crate Configuration").set("name","CrateDumpConfiguration") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
  }
  //
  void EmuPeripheralCrate::CrateDumpConfiguration(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    thisCrate->DumpConfiguration();
    //
  }
  //
  void EmuPeripheralCrate::CrateStatus(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    MyHeader(in,out,"Crate Status");
    //
    *out << cgicc::h3("Configuration done for Crate  ");
    *out << cgicc::br();
    //
    int ccbmode = (thisCCB->ReadRegister(0x0))&0x1;
    //
    *out << cgicc::fieldset().set("style","font-size: 10pt; font-family: arial;");
    *out << "CCB Mode : ";
    if(ccbmode == 1) {
      *out << cgicc::span().set("style","color:green");
      *out << " DLOG";
      *out << cgicc::span();
    } else {
      *out << cgicc::span().set("style","color:red");
      *out << " FPGA";
      *out << cgicc::span();
    }
    *out << cgicc::br();
    *out << cgicc::fieldset();
    //
    int read = (thisCCB->ReadRegister(0x2))&0xffff;
    //
    *out << cgicc::fieldset().set("style","font-size: 10pt; font-family: arial;");
    *out << cgicc::span().set("style","color:blue");
    *out << "MPC slot = 12 cfg             " << (read&0x1);
    *out << cgicc::span();
    *out << cgicc::br();
    //
    *out << cgicc::fieldset();
    //
    *out << cgicc::fieldset().set("style","font-size: 10pt; font-family: arial;");
    //
    *out << "CCB  slot = 13 FPGA cfg       " << ((read>>12)&0x1);
    //
    if(((read>>12)&0x1) == 1) {
      *out << cgicc::span().set("style","color:green");
      *out << " (Done)";
      *out << cgicc::span();
    } else {
      *out << cgicc::span().set("style","color:red");
      *out << " (Failed)";
      *out << cgicc::span();
    }
    //
    *out << cgicc::br();
    //
    *out << "TTCrx ready                   " << ((read>>13)&0x1);
    if(((read>>13)&0x1) == 1) {
      *out << cgicc::span().set("style","color:green");
      *out << " (Ready)";
      *out << cgicc::span();
    } else {
      *out << cgicc::span().set("style","color:red");
      *out << " (Not ready)";
      *out << cgicc::span();
    }
    *out << cgicc::br();
    //
    *out << "QPLL ready                    " << ((read>>14)&0x1);
    if(((read>>14)&0x1) == 1) {
      *out << cgicc::span().set("style","color:green");
      *out << " (Locked)";
      *out << cgicc::span();
    } else {
      *out << cgicc::span().set("style","color:red");
      *out << " (Not locked)";
      *out << cgicc::span();
    }
    *out << cgicc::br();
    //
    *out << "All cfg                       " << ((read>>15)&0x1);
    *out << cgicc::br();
    //
    *out << cgicc::fieldset() ;
    //
    *out << cgicc::fieldset().set("style","font-size: 10pt; font-family: arial;");
    //
    *out << "ALCT slot = 02 cfg            " << ((read>>1)&0x1);
    *out << cgicc::br();
    *out << "ALCT slot = 04 cfg            " << ((read>>2)&0x1);
    *out << cgicc::br();
    *out << "ALCT slot = 06 cfg            " << ((read>>3)&0x1);
    *out << cgicc::br();
    *out << "ALCT slot = 08 cfg            " << ((read>>4)&0x1);
    *out << cgicc::br();
    *out << "ALCT slot = 10 cfg            " << ((read>>5)&0x1);
    *out << cgicc::br();
    *out << "ALCT slot = 14 cfg            " << ((read>>6)&0x1);
    *out << cgicc::br();
    *out << "ALCT slot = 16 cfg            " << ((read>>7)&0x1);
    *out << cgicc::br();
    *out << "ALCT slot = 18 cfg            " << ((read>>8)&0x1);
    *out << cgicc::br();
    *out << "ALCT slot = 20 cfg            " << ((read>>9)&0x1);
    *out << cgicc::br();
    //
    *out << cgicc::fieldset() ;
    //
    *out << cgicc::fieldset().set("style","font-size: 10pt; font-family: arial;");
    //
    *out << "TMB  slot = 02 cfg            " << ((read>>10)&0x1);
    *out << cgicc::br();
    *out << "TMB  slot = 04 cfg            " << ((read>>11)&0x1);
    *out << cgicc::br();
    *out << "TMB  slot = 06 cfg            " << ((read>>12)&0x1);
    *out << cgicc::br();
    *out << "TMB  slot = 08 cfg            " << ((read>>13)&0x1);
    *out << cgicc::br();
    *out << "TMB  slot = 10 cfg            " << ((read>>14)&0x1);
    *out << cgicc::br();
    *out << "TMB  slot = 14 cfg            " << ((read>>15)&0x1);
    *out << cgicc::br();
    //
    read = (thisCCB->ReadRegister(0x4))&0xffff;
    //
    *out << "TMB  slot = 16 cfg            " << ((read)&0x1);
    *out << cgicc::br();
    *out << "TMB  slot = 18 cfg            " << ((read>>1)&0x1);
    *out << cgicc::br();
    *out << "TMB  slot = 20 cfg            " << ((read>>2)&0x1);
    *out << cgicc::br();
    //
    *out << cgicc::fieldset() ;
    //
    *out << cgicc::fieldset().set("style","font-size: 10pt; font-family: arial;");
    //
    *out << "DMB  slot = 03 cfg            " << ((read>>3)&0x1);
    *out << cgicc::br();
    *out << "DMB  slot = 05 cfg            " << ((read>>4)&0x1);
    *out << cgicc::br();
    *out << "DMB  slot = 07 cfg            " << ((read>>5)&0x1);
    *out << cgicc::br();
    *out << "DMB  slot = 09 cfg            " << ((read>>6)&0x1);
    *out << cgicc::br();
    *out << "DMB  slot = 11 cfg            " << ((read>>7)&0x1);
    *out << cgicc::br();
    *out << "DMB  slot = 15 cfg            " << ((read>>8)&0x1);
    *out << cgicc::br();
    *out << "DMB  slot = 17 cfg            " << ((read>>9)&0x1);
    *out << cgicc::br();
    *out << "DMB  slot = 19 cfg            " << ((read>>10)&0x1);
    *out << cgicc::br();
    *out << "DMB  slot = 21 cfg            " << ((read>>11)&0x1);
    *out << cgicc::br();
    //
    *out << cgicc::fieldset() ;
    //
  }
  //
  void EmuPeripheralCrate::CrateTMBCountersRight(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    ostringstream output;
    output << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
    output << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    //
    cgicc::CgiEnvironment cgiEnvi(in);
    std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
    //
    if (AutoRefresh_) {
      *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"1; URL=/"
	   <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<endl;
    }
    //
    output << cgicc::table().set("border","1");
    //
    output <<cgicc::td();
    //
    output <<cgicc::td();
    //
    for(unsigned int tmb=0; tmb<tmbVector.size(); tmb++) {
      tmbVector[tmb]->GetCounters();
      //
      output <<cgicc::td();
      output << "Slot = " <<tmbVector[tmb]->slot();
      output <<cgicc::td();
      //
    }
    //
    output <<cgicc::tr();
    //
    for (int count=0; count<23; count++) {
      //output <<cgicc::tr();
      for(unsigned int tmb=0; tmb<tmbVector.size(); tmb++) {
	output <<cgicc::td();
	//
	if(tmb==0) {
	  output << tmbVector[tmb]->CounterName(count) ;
	  output <<cgicc::td();
	  //
	  output <<cgicc::td();
	  //
	}
	if (DisplayRatio_) {
	  if ( tmbVector[tmb]->GetCounter(16) > 0 ) {
	    output << ((float)(tmbVector[tmb]->GetCounter(count))/(tmbVector[tmb]->GetCounter(16)));
	  } else {
	    output << "-1";
	  }	  
	} else {
	  output << tmbVector[tmb]->GetCounter(count) <<std::endl;
	}
	output <<cgicc::td();
      }
      //output << cgicc::br();
      output <<cgicc::tr();
    }
    //
    output << cgicc::table();
    //
    *out << output.str()<<std::endl;
    //
  }
  //
  void EmuPeripheralCrate::CrateDMBCounters(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    //
    cgicc::CgiEnvironment cgiEnvi(in);
    //
    std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
    //
    *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"1; URL=/"
         <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<endl;
    //
    *out << cgicc::table().set("border","1");
    //
    *out <<cgicc::td();
    //
    *out <<cgicc::td();
    //
    //
    //
    for(unsigned int dmb=0; dmb<dmbVector.size(); dmb++) {
      //
      //for( int iter=0; iter<10; iter++) {
	dmbVector[dmb]->readtimingCounter();
	dmbVector[dmb]->readtimingScope();
	//if( dmbVector[dmb]->GetL1aLctCounter() > 0 ) break;
	//}
    }
    //
    for(unsigned int dmb=0; dmb<dmbVector.size(); dmb++) {
      //
      *out <<cgicc::td();
      *out << "Slot = " <<dmbVector[dmb]->slot();
      *out <<cgicc::td();
      //
    }
    //
    *out <<cgicc::tr();
    //
    *out <<cgicc::td();
    *out << dmbVector[0]->CounterName(0);
    *out <<cgicc::td();
    //
    for(unsigned int dmb=0; dmb<dmbVector.size(); dmb++) {
      *out <<cgicc::td();
      if ( dmbVector[dmb]->GetL1aLctCounter() > 0 ) {
	L1aLctCounter_[dmb] = dmbVector[dmb]->GetL1aLctCounter();
      }
      *out << L1aLctCounter_[dmb] <<std::endl;
      *out <<cgicc::td();
    }
    *out <<cgicc::tr();
    //
    *out <<cgicc::td();
    *out << dmbVector[0]->CounterName(1);
    *out <<cgicc::td();
    //
    for(unsigned int dmb=0; dmb<dmbVector.size(); dmb++) {
      *out <<cgicc::td();
      if ( dmbVector[dmb]->GetCfebDavCounter() > 0 ) CfebDavCounter_[dmb] = dmbVector[dmb]->GetCfebDavCounter();
      *out << CfebDavCounter_[dmb] <<std::endl;
      *out <<cgicc::td();
    }
    *out <<cgicc::tr();
    //
    *out <<cgicc::td();
    *out << dmbVector[0]->CounterName(2);
    *out <<cgicc::td();
    //
    for(unsigned int dmb=0; dmb<dmbVector.size(); dmb++) {
      *out <<cgicc::td();
      if ( dmbVector[dmb]->GetTmbDavCounter() > 0 ) TmbDavCounter_[dmb] = dmbVector[dmb]->GetTmbDavCounter();
      *out << TmbDavCounter_[dmb] <<std::endl;
      *out <<cgicc::td();
    }
    *out <<cgicc::tr();
    //
    *out <<cgicc::td();
    *out << dmbVector[0]->CounterName(3);
    *out <<cgicc::td();
    //
    for(unsigned int dmb=0; dmb<dmbVector.size(); dmb++) {
      *out <<cgicc::td();
      if ( dmbVector[dmb]->GetAlctDavCounter() > 0 ) AlctDavCounter_[dmb] = dmbVector[dmb]->GetAlctDavCounter();
      *out << AlctDavCounter_[dmb] <<std::endl;
      *out <<cgicc::td();
    }
    *out <<cgicc::tr();
    //
    *out <<cgicc::td();
    *out << dmbVector[0]->CounterName(4);
    *out <<cgicc::td();
    //
    for(unsigned int dmb=0; dmb<dmbVector.size(); dmb++) {
      *out <<cgicc::td();
      for( int i=4; i>-1; i--) *out << ((dmbVector[dmb]->GetL1aLctScope()>>i)&0x1) ;
      *out <<cgicc::td();
    }
    *out <<cgicc::tr();
    //
    *out <<cgicc::tr();
    *out <<cgicc::tr();
    //
    *out <<cgicc::td();
    *out << dmbVector[0]->CounterName(5);
    *out <<cgicc::td();
    //
    for(unsigned int dmb=0; dmb<dmbVector.size(); dmb++) {
      *out <<cgicc::td();
      for( int i=4; i>-1; i--) *out << ((dmbVector[dmb]->GetCfebDavScope()>>i)&0x1) ;
      *out <<cgicc::td();
    }
    *out <<cgicc::tr();
    //
    *out <<cgicc::td();
    *out << dmbVector[0]->CounterName(6);
    *out <<cgicc::td();
    //
    for(unsigned int dmb=0; dmb<dmbVector.size(); dmb++) {
      *out <<cgicc::td();
      for( int i=4; i>-1; i--) *out << ((dmbVector[dmb]->GetTmbDavScope()>>i)&0x1) ;
      *out <<cgicc::td();
    }
    *out <<cgicc::tr();
    //
    *out <<cgicc::td();
    *out << dmbVector[0]->CounterName(7);
    *out <<cgicc::td();
    //
    for(unsigned int dmb=0; dmb<dmbVector.size(); dmb++) {
      *out <<cgicc::td();
      for( int i=4; i>-1; i--) *out << ((dmbVector[dmb]->GetAlctDavScope()>>i)&0x1) ;
      *out <<cgicc::td();
    }
    *out <<cgicc::tr();
    //
    *out << cgicc::table();
    //
  }
  //
  void EmuPeripheralCrate::CrateTMBCounters(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::CgiEnvironment cgiEnvi(in);
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    //
    std::string MonitorFrameLeft =
      toolbox::toString("/%s/MonitorFrameLeft",getApplicationDescriptor()->getURN().c_str());
    std::string CrateTMBCountersRight =
      toolbox::toString("/%s/CrateTMBCountersRight",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::frameset().set("cols","200,*");
    *out << cgicc::frame().set("src",MonitorFrameLeft);
    *out << cgicc::frame().set("src",CrateTMBCountersRight);
    *out << cgicc::frameset() ;
    //
  }
  //
  void EmuPeripheralCrate::LaunchMonitor(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    //
    std::string MonitorFrameLeft =
      toolbox::toString("/%s/MonitorFrameLeft",getApplicationDescriptor()->getURN().c_str());
    std::string MonitorFrameRight =
      toolbox::toString("/%s/MonitorFrameRight",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::frameset().set("cols","200,*");
    *out << cgicc::frame().set("src",MonitorFrameLeft);
    *out << cgicc::frame().set("src",MonitorFrameRight);
    *out << cgicc::frameset() ;
    //
    //
    //std::string ResetAllCounters =
    //toolbox::toString("/%s/ResetAllCounters",getApplicationDescriptor()->getURN().c_str());
    //
    //*out << cgicc::form().set("method","GET").set("action",ResetAllCounters) << std::endl ;
    //*out << cgicc::input().set("type","submit").set("value","Reset All Counters") << std::endl ;
    //*out << cgicc::form() << std::endl ;
    //
    //::sleep(1);
    //
    //this->LaunchMonitor(in,out);
    //
  }
  //
  void EmuPeripheralCrate::MonitorFrameRight(xgi::Input * in, xgi::Output * out) 
    throw (xgi::exception::Exception){
    //
    MyHeader(in,out,"Monitor");
    //
    for(int counter=0; counter<22; counter++) {
      //
      Counter_ = counter;
      //
      std::string CreateMonitorUnit =
	toolbox::toString("/%s/CreateMonitorUnit?counter=%d",getApplicationDescriptor()->getURN().c_str(),counter);
      //
      //*out << cgicc::frame().set("src",CreateMonitorUnit)<<std::endl;
      //
      //
      this->CreateMonitorUnit(in,out);
      //
    }
    //
  }
  //
  void EmuPeripheralCrate::MonitorFrameLeft(xgi::Input * in, xgi::Output * out) 
    throw (xgi::exception::Exception){
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    //
    std::string ResetAllCounters =
      toolbox::toString("/%s/ResetAllCounters",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",ResetAllCounters) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Reset All Counters") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    std::string SetUnsetRatio =
      toolbox::toString("/%s/SetUnsetRatio",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",SetUnsetRatio) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Set/Unset Ratio Display") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    std::string SetUnsetAutoRefresh =
      toolbox::toString("/%s/SetUnsetAutoRefresh",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",SetUnsetAutoRefresh) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Set/Unset AutoRefresh") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    this->LaunchMonitor(in,out);
    //
  }
  //
  void EmuPeripheralCrate::MonitorTMBTrigger(xgi::Input * in, xgi::Output * out) 
    throw (xgi::exception::Exception){
    //
    cgicc::CgiEnvironment cgiEnvi(in);
    std::string Page="MonitorTMBTriggerRedirect?"+cgiEnvi.getQueryString();
    //
    *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/"
	 <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<endl;
    //
  }
  //
  void EmuPeripheralCrate::MonitorTMBTriggerRedirect(xgi::Input * in, xgi::Output * out) 
    throw (xgi::exception::Exception){
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
    } else {
      cout << "Not tmb" << endl ;
    }
    //
    //
    std::string MenuMonitorTMBTrigger =
      toolbox::toString("/%s/MenuMonitorTMBTrigger",getApplicationDescriptor()->getURN().c_str());
    //
    std::string MonitorTMBTriggerDisplay =
      toolbox::toString("/%s/MonitorTMBTriggerDisplay?tmb=",getApplicationDescriptor()->getURN().c_str(),tmb);
    //
    *out << cgicc::frameset().set("cols","200,*");
    *out << cgicc::frame().set("src",MenuMonitorTMBTrigger);
    *out << cgicc::frame().set("src",MonitorTMBTriggerDisplay);
    *out << cgicc::frameset() ;
    //
  }
  //
  void EmuPeripheralCrate::MenuMonitorTMBTrigger(xgi::Input * in, xgi::Output * out) 
    throw (xgi::exception::Exception){
    //
    std::string AlctKey =
      toolbox::toString("/%s/AlctKey",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",AlctKey) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Alct key") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    std::string ClctKey =
      toolbox::toString("/%s/ClctKey",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",ClctKey) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Clct key") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    *out << "Menu " << MenuMonitor_ << std::endl;
    //
  }
  //
  void EmuPeripheralCrate::AlctKey(xgi::Input * in, xgi::Output * out) 
    throw (xgi::exception::Exception){
    //
    MenuMonitor_ = 2;
    //
    this->MenuMonitorTMBTrigger(in,out);
    //
  }
  //
  std::string EmuPeripheralCrate::extractCalibrationSetting(xoap::MessageReference message)
    {
      xoap::SOAPElement root = message->getSOAPPart()
	.getEnvelope().getBody();
      xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
      xoap::SOAPName cali     = envelope.createName("Calibration");
      xoap::SOAPName setting  = envelope.createName("Setting");
      //
      std::vector <xoap::SOAPElement> childElements = root.getChildElements(cali);
      //
      std::ostringstream output;
      //
      //output << childElements.size() ;
      //
      for (unsigned int i=0; i<childElements.size(); i++) {
	output << childElements[i].getAttributeValue(setting);
      }
      //
      return output.str();
      //
    }
  //
  //
  void EmuPeripheralCrate::ClctKey(xgi::Input * in, xgi::Output * out) 
    throw (xgi::exception::Exception){
    //
    MenuMonitor_ = 1;
    //
    this->MenuMonitorTMBTrigger(in,out);
    //
  }
  //
  void EmuPeripheralCrate::MonitorTMBTriggerDisplay(xgi::Input * in, xgi::Output * out) 
    throw (xgi::exception::Exception){
    //
    cgicc::CgiEnvironment cgiEnvi(in);
    //
    std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
    //
    *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"2; URL=/"
	 <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<endl;
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
    } else {
      cout << "Not tmb" << endl ;
    }
    //
    std::cout << "MenuMonitor " << MenuMonitor_ << std::endl;
    //
    TMB * thisTMB = tmbVector[tmb];
    //
    int oldValue=0;
    //
    for(int loop=0; loop<nTrigger_; loop++){
      //
      thisTMB->DecodeALCT();
      thisTMB->DecodeCLCT();
      //
      ::usleep(100);
      //
      if ( thisTMB->GetAlct0FirstKey() != oldValue ) {
	oldValue = thisTMB->GetAlct0FirstKey();
	if(thisTMB->GetAlct0FirstKey()>0     ) TMBTriggerAlct0Key[thisTMB->GetAlct0FirstKey()][tmb]++;
	if(thisTMB->GetAlct1SecondKey()>0    ) TMBTriggerAlct1Key[thisTMB->GetAlct1SecondKey()][tmb]++;
	if(thisTMB->GetCLCT0keyHalfStrip()>0 ) TMBTriggerClct0keyHalfStrip[thisTMB->GetCLCT0keyHalfStrip()][tmb]++;
	if(thisTMB->GetCLCT1keyHalfStrip()>0 ) TMBTriggerClct1keyHalfStrip[thisTMB->GetCLCT1keyHalfStrip()][tmb]++;
      }
      //
    }
    //
    std::cout << "MenuMonitor " << MenuMonitor_ << std::endl;
    //
    *out << "<HTML>" <<std::endl;
    *out << "<BODY bgcolor=\"#FFFFFF\">" <<std::endl;
    *out << "<OBJECT classid=\"clsid:D27CDB6E-AE6D-11cf-96B8-444553540000\" codebase=\"http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,0,0\" WIDTH=\"565\" HEIGHT=\"420\" id=\"FC_2_3_Column3D\">" <<std::endl;
    *out << "<PARAM NAME=movie VALUE=\"/daq/extern/FusionCharts/Charts/FC_2_3_Column3D.swf\">" <<std::endl;
    //
    ostringstream output;
    //
    if (MenuMonitor_ == 1 ) {
      output << "<PARAM NAME=\"FlashVars\" VALUE=\"&dataURL=getData" << "TMBTriggerAlct0Key?tmb="<<tmb << "&chartWidth=565&chartHeight=420"<<"\">"<<std::endl;
    }
    //
    if (MenuMonitor_ == 2 ) {
      output << "<PARAM NAME=\"FlashVars\" VALUE=\"&dataURL=getData" << "TMBTriggerClct0Key?tmb="<<tmb << "&chartWidth=565&chartHeight=420"<<"\">"<<std::endl;
    }
    //
    //std::cout << output.str() << std::endl;
    *out << output.str() << std::endl ;
    *out << "<PARAM NAME=quality VALUE=high>" << std::endl ;
    *out << "<PARAM NAME=bgcolor VALUE=#FFFFFF>" << std::endl ;
    //
    ostringstream output2;
    if (MenuMonitor_ == 1 ) {
      output2 << "<EMBED src=\"/daq/extern/FusionCharts/Charts/FC_2_3_Column3D.swf\" FlashVars=\"&dataURL=getData"<< "TMBTriggerAlct0Key?tmb="<<tmb<<"\" quality=high bgcolor=#FFFFFF WIDTH=\"565\" HEIGHT=\"420\" NAME=\"FC_2_3_Column3D\" TYPE=\"application/x-shockwave-flash\" PLUGINSPAGE=\"http://www.macromedia.com/go/getflashplayer\"></EMBED>" << std::endl;
    }
    //
    if (MenuMonitor_ == 2 ) {
      output2 << "<EMBED src=\"/daq/extern/FusionCharts/Charts/FC_2_3_Column3D.swf\" FlashVars=\"&dataURL=getData"<< "TMBTriggerClct0Key?tmb="<<tmb<<"\" quality=high bgcolor=#FFFFFF WIDTH=\"565\" HEIGHT=\"420\" NAME=\"FC_2_3_Column3D\" TYPE=\"application/x-shockwave-flash\" PLUGINSPAGE=\"http://www.macromedia.com/go/getflashplayer\"></EMBED>" << std::endl;
    }
    //
    *out << output2.str() << std::endl;
    //
    *out << "</OBJECT>" << std::endl;
    //
    *out << "<OBJECT classid=\"clsid:D27CDB6E-AE6D-11cf-96B8-444553540000\" codebase=\"http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,0,0\" WIDTH=\"565\" HEIGHT=\"420\" id=\"FC_2_3_Column3D\">" <<std::endl;
    *out << "<PARAM NAME=movie VALUE=\"/daq/extern/FusionCharts/Charts/FC_2_3_Column3D.swf\">" <<std::endl;
    //
    ostringstream output3;
    if(MenuMonitor_ == 1 ){
      output3 << "<PARAM NAME=\"FlashVars\" VALUE=\"&dataURL=getData" << "TMBTriggerAlct1Key?tmb=" << tmb<<"&chartWidth=565&chartHeight=420"<<"\">"<<std::endl;
    }
    //
    if(MenuMonitor_ == 2 ){
      output3 << "<PARAM NAME=\"FlashVars\" VALUE=\"&dataURL=getData" << "TMBTriggerClct1Key?tmb=" << tmb<<"&chartWidth=565&chartHeight=420"<<"\">"<<std::endl;
    }
    //
    //std::cout << output.str() << std::endl;
    *out << output3.str() << std::endl ;
    *out << "<PARAM NAME=quality VALUE=high>" << std::endl ;
    *out << "<PARAM NAME=bgcolor VALUE=#FFFFFF>" << std::endl ;
    //
    ostringstream output4;
    if(MenuMonitor_==1) {
      output4 << "<EMBED src=\"/daq/extern/FusionCharts/Charts/FC_2_3_Column3D.swf\" FlashVars=\"&dataURL=getData"<< "TMBTriggerAlct1Key?tmb="<<tmb<<"\" quality=high bgcolor=#FFFFFF WIDTH=\"565\" HEIGHT=\"420\" NAME=\"FC_2_3_Column3D\" TYPE=\"application/x-shockwave-flash\" PLUGINSPAGE=\"http://www.macromedia.com/go/getflashplayer\"></EMBED>" << std::endl;
    }
    //
    if(MenuMonitor_==2) {
      output4 << "<EMBED src=\"/daq/extern/FusionCharts/Charts/FC_2_3_Column3D.swf\" FlashVars=\"&dataURL=getData"<< "TMBTriggerClct1Key?tmb="<<tmb<<"\" quality=high bgcolor=#FFFFFF WIDTH=\"565\" HEIGHT=\"420\" NAME=\"FC_2_3_Column3D\" TYPE=\"application/x-shockwave-flash\" PLUGINSPAGE=\"http://www.macromedia.com/go/getflashplayer\"></EMBED>" << std::endl;
    }
    //
    *out << output4.str() << std::endl;
    //
    *out << "</OBJECT>" << std::endl;
    //
    *out << "</BODY>" << std::endl;
    *out << "</HTML>" << std::endl;
    //
  }
  //
  void EmuPeripheralCrate::CreateMonitorUnit(xgi::Input * in, xgi::Output * out) 
    throw (xgi::exception::Exception){
    //
    *out << cgicc::fieldset().set("style","font-size: 8pt; font-family: arial;");
    //
    //
    cgicc::Cgicc cgi(in);
    cgicc::form_iterator name = cgi.getElement("counter");
    name = cgi.getElement("counter");
    //
    if(name != cgi.getElements().end()) {
      Counter_ = cgi["counter"]->getIntegerValue();
    } else {
      cout << "No counter" << endl;
      LOG4CPLUS_INFO(getApplicationLogger(), "no counters");
    }
    //
    ChartData[Counter_].clear();
    //
    *out << "Counter= " << Counter_ << std::endl;
    *out << cgicc::br();
    //
    for(unsigned int i=0; i<tmbVector.size(); i++) {
      //
      tmbVector[i]->RedirectOutput(out);
      tmbVector[i]->GetCounters();
      //
      //if ( tmbVector[i]->GetCounter(4)>0) {
      ChartData[Counter_].push_back((float)(tmbVector[i]->GetCounter(Counter_)));
      //} 	  
      //
      if ( Counter_ == 0 ) {
	if ( tmbVector[i]->GetCounter(0) > 0 ) {
	  *out << cgicc::span().set("style","color:red");
	  tmbVector[i]->PrintCounters(Counter_);
	  *out << cgicc::span();
	  *out << cgicc::br();
	} else {
	  *out << cgicc::span().set("style","color:green");
	  tmbVector[i]->PrintCounters(Counter_);
	  *out << cgicc::span();
	  *out << cgicc::br();
	}
	//
      } else if ( Counter_ == 1 ) {
	  if ( tmbVector[i]->GetCounter(1) > 0 ) {
	    *out << cgicc::span().set("style","color:green");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  } else {
	    *out << cgicc::span().set("style","color:red");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  }
	} else if ( Counter_ == 2 ) {
	  if ( tmbVector[i]->GetCounter(2) > 0 ) {
	    *out << cgicc::span().set("style","color:red");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  } else {
	    *out << cgicc::span().set("style","color:green");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  }
	} else if ( Counter_ == 3 ) {
	  if ( tmbVector[i]->GetCounter(3) > 0 ) {
	    *out << cgicc::span().set("style","color:green");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  } else {
	    *out << cgicc::span().set("style","color:red");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  }
	} else if ( Counter_ == 4 ) {
	  if ( tmbVector[i]->GetCounter(4) > 0 ) {
	    *out << cgicc::span().set("style","color:green");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  } else {
	    *out << cgicc::span().set("style","color:red");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  }
	} else if ( Counter_ == 5 ) {
	  if ( tmbVector[i]->GetCounter(5) > 0 ) {
	    *out << cgicc::span().set("style","color:red");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  } else {
	    *out << cgicc::span().set("style","color:green");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  }
	} else if ( Counter_ == 6 ) {
	  if ( tmbVector[i]->GetCounter(Counter_) > 0 ) {
	    *out << cgicc::span().set("style","color:red");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  } else {
	    *out << cgicc::span().set("style","color:green");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  }
	} else if ( Counter_ == 7 ) {
	  if ( tmbVector[i]->GetCounter(Counter_) > 0 ) {
	    *out << cgicc::span().set("style","color:red");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  } else {
	    *out << cgicc::span().set("style","color:green");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  }
	} else if ( Counter_ == 8 ) {
	  if ( tmbVector[i]->GetCounter(Counter_) > 0 ) {
	    *out << cgicc::span().set("style","color:green");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  } else {
	    *out << cgicc::span().set("style","color:red");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  }
	} else if ( Counter_ == 9 ) {
	  if ( tmbVector[i]->GetCounter(Counter_) > 0 ) {
	    *out << cgicc::span().set("style","color:green");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  } else {
	    *out << cgicc::span().set("style","color:red");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  }
	} else if ( Counter_ == 10 ) {
	  if ( tmbVector[i]->GetCounter(Counter_) > 0 ) {
	    *out << cgicc::span().set("style","color:green");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  } else {
	    *out << cgicc::span().set("style","color:red");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  }
	} else if ( Counter_ == 11 ) {
	  if ( tmbVector[i]->GetCounter(Counter_) > 0 ) {
	    *out << cgicc::span().set("style","color:red");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  } else {
	    *out << cgicc::span().set("style","color:green");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  }
	} else if ( Counter_ == 12 ) {
	  if ( tmbVector[i]->GetCounter(Counter_) > 0 ) {
	    *out << cgicc::span().set("style","color:green");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  } else {
	    *out << cgicc::span().set("style","color:red");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  }
	} else if ( Counter_ == 13 ) {
	  if ( tmbVector[i]->GetCounter(Counter_) > 0 ) {
	    *out << cgicc::span().set("style","color:red");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  } else {
	    *out << cgicc::span().set("style","color:green");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  }
	} else if ( Counter_ == 14 ) {
	  if ( tmbVector[i]->GetCounter(Counter_) > 0 ) {
	    *out << cgicc::span().set("style","color:red");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  } else {
	    *out << cgicc::span().set("style","color:green");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  }
	} else if ( Counter_ == 15 ) {
	  if ( tmbVector[i]->GetCounter(Counter_) > 0 ) {
	    *out << cgicc::span().set("style","color:red");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  } else {
	    *out << cgicc::span().set("style","color:green");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  }
	} else if ( Counter_ == 16 ) {
	  if ( tmbVector[i]->GetCounter(Counter_) > 0 ) {
	    *out << cgicc::span().set("style","color:green");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  } else {
	    *out << cgicc::span().set("style","color:red");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  }
	} else if ( Counter_ == 17 ) {
	  if ( tmbVector[i]->GetCounter(Counter_) > 0 ) {
	    *out << cgicc::span().set("style","color:green");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  } else {
	    *out << cgicc::span().set("style","color:red");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  }
	} else if ( Counter_ == 18 ) {
	  if ( tmbVector[i]->GetCounter(Counter_) > 0 ) {
	    *out << cgicc::span().set("style","color:green");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  } else {
	    *out << cgicc::span().set("style","color:red");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  }
	} else if ( Counter_ == 19 ) {
	  if ( tmbVector[i]->GetCounter(Counter_) > 0 ) {
	    *out << cgicc::span().set("style","color:green");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  } else {
	    *out << cgicc::span().set("style","color:red");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  }
	} else if ( Counter_ == 20 ) {
	  if ( tmbVector[i]->GetCounter(Counter_) > 0 ) {
	    *out << cgicc::span().set("style","color:green");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  } else {
	    *out << cgicc::span().set("style","color:red");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  }
	} else if ( Counter_ == 21 ) {
	  if ( tmbVector[i]->GetCounter(Counter_) > 0 ) {
	    *out << cgicc::span().set("style","color:green");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  } else {
	    *out << cgicc::span().set("style","color:red");
	    tmbVector[i]->PrintCounters(Counter_);
	    *out << cgicc::span();
	    *out << cgicc::br();
	  }
	} else {
	  //
	  *out << cgicc::pre() ;
	  tmbVector[i]->PrintCounters(Counter_);
	  *out << cgicc::pre() ;
	  //
	}
	//
	tmbVector[i]->RedirectOutput(&std::cout);
	//
	//cgicc::br();
	//
      }
      //
      this->Display(in,out,Counter_);
      //
      *out << cgicc::fieldset();    
    //
  }
  //
  void EmuPeripheralCrate::Display(xgi::Input * in, xgi::Output * out, int counter ) 
    throw (xgi::exception::Exception)
  {
    //
    *out << "<HTML>" <<std::endl;
    *out << "<BODY bgcolor=\"#FFFFFF\">" <<std::endl;
    *out << "<OBJECT classid=\"clsid:D27CDB6E-AE6D-11cf-96B8-444553540000\" codebase=\"http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,0,0\" WIDTH=\"565\" HEIGHT=\"420\" id=\"FC_2_3_Bar2D\">" <<std::endl;
    *out << "<PARAM NAME=movie VALUE=\"/daq/extern/FusionCharts/Charts/FC_2_3_Bar2D.swf\">" <<std::endl;
    //
    ostringstream output;
    output << "<PARAM NAME=\"FlashVars\" VALUE=\"&dataURL=getData" << counter <<"\">"<<std::endl;
    //
    std::cout << output.str() << std::endl;
    *out << output.str() << std::endl ;
    *out << "<PARAM NAME=quality VALUE=high>" << std::endl ;
    *out << "<PARAM NAME=bgcolor VALUE=#FFFFFF>" << std::endl ;
    //*out << "<EMBED src=\"/daq/extern/FusionCharts/Charts/FC_2_3_Bar2D.swf\" FlashVars=\"&dataURL=getData\" quality=high bgcolor=#FFFFFF WIDTH=\"565\" HEIGHT=\"420\" NAME=\"FC_2_3_Bar2D\" TYPE=\"application/x-shockwave-flash\" PLUGINSPAGE=\"http://www.macromedia.com/go/getflashplayer\"></EMBED>" << std::endl;
    //
    ostringstream output2;
    output2 << "<EMBED src=\"/daq/extern/FusionCharts/Charts/FC_2_3_Bar2D.swf\" FlashVars=\"&dataURL=getData"<< counter <<"\" quality=high bgcolor=#FFFFFF WIDTH=\"565\" HEIGHT=\"420\" NAME=\"FC_2_3_Bar2D\" TYPE=\"application/x-shockwave-flash\" PLUGINSPAGE=\"http://www.macromedia.com/go/getflashplayer\"></EMBED>" << std::endl;
    //
    *out << output2.str() << std::endl;
    //
    *out << "</OBJECT>" << std::endl;
    *out << "</BODY>" << std::endl;
    *out << "</HTML>" << std::endl;
    //
  }
  //
  void EmuPeripheralCrate::getDataTMBTriggerAlct0Key(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
    {
      //
      cgicc::Cgicc cgi(in);
      //
      cgicc::form_iterator name = cgi.getElement("tmb");
      int tmb;
      if(name != cgi.getElements().end()) {
	tmb = cgi["tmb"]->getIntegerValue();
      } else {
	cout << "Not tmb" << endl ;
      }
      //
      *out << "<graph caption='ALCT0 key wire group' subcaption='' xAxisName='ALCT0 wire group' yAxisName='Event' numberPrefix='' showNames='1' animation='0'>" << std::endl;
      //
      for(unsigned int i=0;i<120;i++) {
	ostringstream output;
	if(i%10==0) {
	  output << "<set name='" << i <<"'"<< " value='" << TMBTriggerAlct0Key[i][tmb] << "'" << " />" << std::endl;
	} else {
	  output << "<set name=''"<< " value='" << TMBTriggerAlct0Key[i][tmb] << "'" << " />" << std::endl;
	}
	*out << output.str() << std::endl ;
	//std::cout << output.str() << std::endl ;
      }
      *out << "</graph>" << std::endl;    
    }
  //
  void EmuPeripheralCrate::getDataTMBTriggerAlct1Key(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
    {
      //
      cgicc::Cgicc cgi(in);
      //
      cgicc::form_iterator name = cgi.getElement("tmb");
      int tmb;
      if(name != cgi.getElements().end()) {
	tmb = cgi["tmb"]->getIntegerValue();
      } else {
	cout << "Not tmb" << endl ;
      }
      //
      *out << "<graph caption='ALCT1 key wire group' subcaption='' xAxisName='ALCT1 wire group' yAxisName='Event' numberPrefix='' showNames='1' animation='0'>" << std::endl;
      //
      for(unsigned int i=0;i<120;i++) {
	ostringstream output;
	if(i%10==0) {
	  output << "<set name='" << i <<"'"<< " value='" << TMBTriggerAlct1Key[i][tmb] << "'" << " />" << std::endl;
	} else {
	  output << "<set name=''"<< " value='" << TMBTriggerAlct1Key[i][tmb] << "'" << " />" << std::endl;
	}
	*out << output.str() << std::endl ;
      }
      *out << "</graph>" << std::endl;    
    }
  //
  void EmuPeripheralCrate::getDataTMBTriggerClct0Key(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
    {
      //
      cgicc::Cgicc cgi(in);
      //
      cgicc::form_iterator name = cgi.getElement("tmb");
      int tmb;
      if(name != cgi.getElements().end()) {
	tmb = cgi["tmb"]->getIntegerValue();
      } else {
	cout << "Not tmb" << endl ;
      }
      //
      *out << "<graph caption='CLCT0 key wire group' subcaption='' xAxisName='CLCT0 key HalfStrip' yAxisName='Event' numberPrefix='' showNames='1' animation='0'>" << std::endl;
      //
      for(unsigned int i=0;i<120;i++) {
	ostringstream output;
	if(i%10==0) {
	  output << "<set name='" << i <<"'"<< " value='" << TMBTriggerClct0keyHalfStrip[i][tmb] << "'" << " />" << std::endl;
	} else {
	  output << "<set name=''"<< " value='" << TMBTriggerClct0keyHalfStrip[i][tmb] << "'" << " />" << std::endl;
	}
	*out << output.str() << std::endl ;
	//std::cout << output.str() << std::endl ;
      }
      *out << "</graph>" << std::endl;    
    }
  //
  void EmuPeripheralCrate::getDataTMBTriggerClct1Key(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
    {
      //
      cgicc::Cgicc cgi(in);
      //
      cgicc::form_iterator name = cgi.getElement("tmb");
      int tmb;
      if(name != cgi.getElements().end()) {
	tmb = cgi["tmb"]->getIntegerValue();
      } else {
	cout << "Not tmb" << endl ;
      }
      //
      *out << "<graph caption='ALCT1 key wire group' subcaption='' xAxisName='CLCT1 key HalfStrip' yAxisName='Event' numberPrefix='' showNames='1' animation='0'>" << std::endl;
      //
      for(unsigned int i=0;i<120;i++) {
	ostringstream output;
	if(i%10==0) {
	  output << "<set name='" << i <<"'"<< " value='" << TMBTriggerClct1keyHalfStrip[i][tmb] << "'" << " />" << std::endl;
	} else {
	  output << "<set name=''"<< " value='" << TMBTriggerClct1keyHalfStrip[i][tmb] << "'" << " />" << std::endl;
	}
	*out << output.str() << std::endl ;
      }
      *out << "</graph>" << std::endl;    
    }
  //
  void EmuPeripheralCrate::getData0(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << "<graph caption='ALCT: CRC error' subcaption='Normalized' xAxisName='TMB slot' yAxisName='Rate' numberPrefix='' showNames='1' animation='0'>" << std::endl;
    //
    std::cout << ChartData[0].size() << endl ;
    for(unsigned int i=0;i<tmbVector.size();i++) {
      ostringstream output;
      output << "<set name='" << tmbVector[i]->slot() <<"'"<< " value='" << ChartData[0][i] << "'" << " />" << std::endl;
      *out << output.str() << std::endl ;
    }
    *out << "</graph>" << std::endl;    
  }
  //
  void EmuPeripheralCrate::getData1(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << "<graph caption='ALCT: LCT sent to TMB' subcaption='Normalized' xAxisName='TMB slot' yAxisName='Rate' numberPrefix='' showNames='1'>" << std::endl;
    //
    std::cout << ChartData[1].size() << endl ;
    for(unsigned int i=0;i<tmbVector.size();i++) {
      ostringstream output;
      output << "<set name='" << tmbVector[i]->slot() << "'"<< " value='" << ChartData[1][i] << "'" << " />" << std::endl;
      *out << output.str() << std::endl ;
    }
    *out << "</graph>" << std::endl;    
  }
  //
  void EmuPeripheralCrate::getData2(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << "<graph caption='ALCT: LCT error' subcaption='Normalized' xAxisName='TMB slot' yAxisName='Rate' numberPrefix='' showNames='1'>" << std::endl;
    //
    std::cout << ChartData[2].size() << endl ;
    for(unsigned int i=0;i<tmbVector.size();i++) {
      ostringstream output;
      output << "<set name='" << tmbVector[i]->slot() << "'"<< " value='" << ChartData[2][i] << "'" << " />" << std::endl;
      *out << output.str() << std::endl ;
    }
    *out << "</graph>" << std::endl;    
  }
  //
  void EmuPeripheralCrate::getData3(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << "<graph caption='ALCT: L1A readout' subcaption='Normalized' xAxisName='TMB slot' yAxisName='Rate' numberPrefix='' showNames='1'>" << std::endl;
    //
    std::cout << ChartData[3].size() << endl ;
    for(unsigned int i=0;i<tmbVector.size();i++) {
      ostringstream output;
      output << "<set name='" << tmbVector[i]->slot() << "'"<< " value='" << ChartData[3][i] << "'" << " />" << std::endl;
      *out << output.str() << std::endl ;
    }
    *out << "</graph>" << std::endl;    
  }
  //
  void EmuPeripheralCrate::getData4(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << "<graph caption='CLCT Pretrigger' subcaption='Normalized' xAxisName='TMB slot' yAxisName='Rate' numberPrefix='' showNames='1'>" << std::endl;
    //
    std::cout << ChartData[4].size() << endl ;
    for(unsigned int i=0;i<tmbVector.size();i++) {
      ostringstream output;
      output << "<set name='" << tmbVector[i]->slot() << "'"<< " value='" << ChartData[4][i] << "'" << " />" << std::endl;
      *out << output.str() << std::endl ;
    }
    *out << "</graph>" << std::endl;    
  }
  //
  void EmuPeripheralCrate::getData5(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << "<graph caption='CLCT Pretrig but no wbuf' subcaption='Default' xAxisName='TMB slot' yAxisName='Rate' numberPrefix='' showNames='1'>" << std::endl;
    //
    std::cout << ChartData[5].size() << endl ;
    for(unsigned int i=0;i<tmbVector.size();i++) {
      ostringstream output;
      output << "<set name='" << tmbVector[i]->slot() << "'"<< " value='" << ChartData[5][i] << "'" << " />" << std::endl;
      *out << output.str() << std::endl ;
    }
    *out << "</graph>" << std::endl;    
  }
  //
  void EmuPeripheralCrate::getData6(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << "<graph caption='CLCT: Invalid pattern after drift 0' subcaption='Default' xAxisName='TMB slot' yAxisName='Rate' numberPrefix='' showNames='1'>" << std::endl;
    //
    std::cout << ChartData[6].size() << endl ;
    for(unsigned int i=0;i<tmbVector.size();i++) {
      ostringstream output;
      output << "<set name='" << tmbVector[i]->slot() << "'"<< " value='" << ChartData[6][i] << "'" << " />" << std::endl;
      *out << output.str() << std::endl ;
    }
    *out << "</graph>" << std::endl;    
  }
  //
  void EmuPeripheralCrate::getData7(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << "<graph caption='TMB matching rejected' subcaption='Default' xAxisName='TMB slot' yAxisName='Rate' numberPrefix='' showNames='1'>" << std::endl;
    //
    std::cout << ChartData[7].size() << endl ;
    for(unsigned int i=0;i<tmbVector.size();i++) {
      ostringstream output;
      output << "<set name='" << tmbVector[i]->slot() << "'"<< " value='" << ChartData[7][i] << "'" << " />" << std::endl;
      *out << output.str() << std::endl ;
    }
    *out << "</graph>" << std::endl;    
  }
  //
  void EmuPeripheralCrate::getData8(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << "<graph caption='CLCT or ALCT or both triggered' subcaption='Default' xAxisName='TMB slot' yAxisName='Rate' numberPrefix='' showNames='1'>" << std::endl;
    //
    std::cout << ChartData[8].size() << endl ;
    for(unsigned int i=0;i<tmbVector.size();i++) {
      ostringstream output;
      output << "<set name='" << tmbVector[i]->slot() << "'"<< " value='" << ChartData[8][i] << "'" << " />" << std::endl;
      *out << output.str() << std::endl ;
    }
    *out << "</graph>" << std::endl;    
  }
  //
  void EmuPeripheralCrate::getData9(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << "<graph caption='TMB: CLCT or ALCT or both trigger xmit MPC' subcaption='Default' xAxisName='TMB slot' yAxisName='Rate' numberPrefix='' showNames='1'>" << std::endl;
    //
    std::cout << ChartData[9].size() << endl ;
    for(unsigned int i=0;i<tmbVector.size();i++) {
      ostringstream output;
      output << "<set name='" << tmbVector[i]->slot() << "'"<< " value='" << ChartData[9][i] << "'" << " />" << std::endl;
      *out << output.str() << std::endl ;
    }
    *out << "</graph>" << std::endl;    
  }
  //
  void EmuPeripheralCrate::getData10(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << "<graph caption='TMB: CLCT and ALCT matched in time' subcaption='Default' xAxisName='TMB slot' yAxisName='Rate' numberPrefix='' showNames='1'>" << std::endl;
    //
    std::cout << ChartData[10].size() << endl ;
    for(unsigned int i=0;i<tmbVector.size();i++) {
      ostringstream output;
      output << "<set name='" << tmbVector[i]->slot() << "'"<< " value='" << ChartData[10][i] << "'" << " />" << std::endl;
      *out << output.str() << std::endl ;
    }
    *out << "</graph>" << std::endl;    
  }
  //
  void EmuPeripheralCrate::getData11(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << "<graph caption='TMB: ALCT-only trigger' subcaption='Default' xAxisName='TMB slot' yAxisName='Rate' numberPrefix='' showNames='1'>" << std::endl;
    //
    std::cout << ChartData[11].size() << endl ;
    for(unsigned int i=0;i<tmbVector.size();i++) {
      ostringstream output;
      output << "<set name='" << tmbVector[i]->slot() << "'"<< " value='" << ChartData[11][i] << "'" << " />" << std::endl;
      *out << output.str() << std::endl ;
    }
    *out << "</graph>" << std::endl;    
  }
  //
  void EmuPeripheralCrate::getData12(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << "<graph caption='CLCT-only trigger' subcaption='Default' xAxisName='TMB slot' yAxisName='Rate' numberPrefix='' showNames='1'>" << std::endl;
    //
    std::cout << ChartData[12].size() << endl ;
    for(unsigned int i=0;i<tmbVector.size();i++) {
      ostringstream output;
      output << "<set name='" << tmbVector[i]->slot() << "'"<< " value='" << ChartData[12][i] << "'" << " />" << std::endl;
      *out << output.str() << std::endl ;
    }
    *out << "</graph>" << std::endl;    
  }
  //
  void EmuPeripheralCrate::getData13(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << "<graph caption='TMB: No trig pulse response' subcaption='Default' xAxisName='TMB slot' yAxisName='Rate' numberPrefix='' showNames='1'>" << std::endl;
    //
    std::cout << ChartData[13].size() << endl ;
    for(unsigned int i=0;i<tmbVector.size();i++) {
      ostringstream output;
      output << "<set name='" << tmbVector[i]->slot() << "'"<< " value='" << ChartData[13][i] << "'" << " />" << std::endl;
      *out << output.str() << std::endl ;
    }
    *out << "</graph>" << std::endl;    
  }
  //
  void EmuPeripheralCrate::getData14(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << "<graph caption='TMB: No MPC transmission' subcaption='Default' xAxisName='TMB slot' yAxisName='Rate' numberPrefix='' showNames='1'>" << std::endl;
    //
    for(unsigned int i=0;i<tmbVector.size();i++) {
      ostringstream output;
      output << "<set name='" << tmbVector[i]->slot() << "'"<< " value='" << ChartData[14][i] << "'" << " />" << std::endl;
      *out << output.str() << std::endl ;
    }
    *out << "</graph>" << std::endl;    
  }
  //
  void EmuPeripheralCrate::getData15(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << "<graph caption='No MPC response FF pulse' subcaption='Default' xAxisName='TMB slot' yAxisName='Rate' numberPrefix='' showNames='1'>" << std::endl;
    //
    for(unsigned int i=0;i<tmbVector.size();i++) {
      ostringstream output;
      output << "<set name='" << tmbVector[i]->slot() << "'"<< " value='" << ChartData[15][i] << "'" << " />" << std::endl;
      *out << output.str() << std::endl ;
    }
    *out << "</graph>" << std::endl;    
  }
  //
  void EmuPeripheralCrate::getData16(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << "<graph caption='MPC accepted LCT0' subcaption='Default' xAxisName='TMB slot' yAxisName='Rate' numberPrefix='' showNames='1'>" << std::endl;
    //
    for(unsigned int i=0;i<tmbVector.size();i++) {
      ostringstream output;
      output << "<set name='" << tmbVector[i]->slot() << "'"<< " value='" << ChartData[16][i] << "'" << " />" << std::endl;
      *out << output.str() << std::endl ;
    }
    *out << "</graph>" << std::endl;    
  }
  //
  void EmuPeripheralCrate::getData17(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << "<graph caption='MPC accepted LCT1' subcaption='Default' xAxisName='TMB slot' yAxisName='Rate' numberPrefix='' showNames='1'>" << std::endl;
    //
    for(unsigned int i=0;i<tmbVector.size();i++) {
      ostringstream output;
      output << "<set name='" << tmbVector[i]->slot() << "'"<< " value='" << ChartData[17][i] << "'" << " />" << std::endl;
      *out << output.str() << std::endl ;
      std::cout << output.str() << std::endl;
    }
    *out << "</graph>" << std::endl;    
  }
  //
  void EmuPeripheralCrate::getData18(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << "<graph caption='L1A received' subcaption='Default' xAxisName='TMB slot' yAxisName='Rate' numberPrefix='' showNames='1'>" << std::endl;
    //
    for(unsigned int i=0;i<tmbVector.size();i++) {
      ostringstream output;
      output << "<set name='" << tmbVector[i]->slot() << "'"<< " value='" << ChartData[18][i] << "'" << " />" << std::endl;
      *out << output.str() << std::endl ;
    }
    *out << "</graph>" << std::endl;    
  }
  //
  void EmuPeripheralCrate::getData19(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << "<graph caption='L1A: TMB triggered, TMB in L1A window' subcaption='Default' xAxisName='TMB slot' yAxisName='Rate' numberPrefix='' showNames='1'>" << std::endl;
    //
    for(unsigned int i=0;i<tmbVector.size();i++) {
      ostringstream output;
      output << "<set name='" << tmbVector[i]->slot() << "'"<< " value='" << ChartData[19][i] << "'" << " />" << std::endl;
      *out << output.str() << std::endl ;
    }
    *out << "</graph>" << std::endl;    
  }
  //
  void EmuPeripheralCrate::getData20(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << "<graph caption='L1A: L1A received, no TMB in window' subcaption='Default' xAxisName='TMB slot' yAxisName='Rate' numberPrefix='' showNames='1'>" << std::endl;
    //
    for(unsigned int i=0;i<tmbVector.size();i++) {
      ostringstream output;
      output << "<set name='" << tmbVector[i]->slot() << "'"<< " value='" << ChartData[20][i] << "'" << " />" << std::endl;
      *out << output.str() << std::endl ;
    }
    *out << "</graph>" << std::endl;    
  }
  //
  void EmuPeripheralCrate::getData21(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << "<graph caption='CLCT pre-trigger' subcaption='Default' xAxisName='TMB slot' yAxisName='Rate' numberPrefix='' showNames='1'>" << std::endl;
    //
    for(unsigned int i=0;i<tmbVector.size();i++) {
      ostringstream output;
      output << "<set name='" << tmbVector[i]->slot() << "'"<< " value='" << ChartData[21][i] << "'" << " />" << std::endl;
      *out << output.str() << std::endl ;
    }
    *out << "</graph>" << std::endl;    
  }
  //
  void EmuPeripheralCrate::SetUnsetRatio(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    if ( DisplayRatio_ == false ) {
      DisplayRatio_ = true;
    } else {
      DisplayRatio_ = false;
    }
    //
    this->MonitorFrameLeft(in,out);
    //
  }
  //
  void EmuPeripheralCrate::SetUnsetAutoRefresh(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
    {
    //
    if ( AutoRefresh_ == false ) {
      AutoRefresh_ = true;
    } else {
      AutoRefresh_ = false;
    }      
    //
    this->MonitorFrameLeft(in,out);
    //
  }
  //
  void EmuPeripheralCrate::ResetAllCounters(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    for(unsigned int i=0; i<tmbVector.size(); i++) {
      //
      //*out << cgicc::fieldset().set("style","font-size: 8pt; font-family: arial;");
      //
      tmbVector[i]->ResetCounters();
      //
      //*out << cgicc::fieldset();    
      //
    }
    //
    this->MonitorFrameLeft(in,out);
    //
  }
  //
  void EmuPeripheralCrate::InitSystem(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    cout << "Init System" << endl ;
    LOG4CPLUS_INFO(getApplicationLogger(), "Init System");
    //
    //
    MyController->configure();          // Init system
    //
    //fireEvent("Configure");
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("navigator");
    //
    int navigator;
    if(name != cgi.getElements().end()) {
      navigator = cgi["navigator"]->getIntegerValue();
      cout << "Navigator " << navigator << endl;
      if ( navigator == 1 ) {
	thisCCB->setCCBMode(CCB::VMEFPGA);      // It needs to be in FPGA mode to work.
	this->ChamberTests(in,out);
      }
    } else {
      cout << "No navigator" << endl;
      this->Default(in,out);
    }
  }
  //
  void EmuPeripheralCrate::InitChamber(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "Init Chamber" << endl ;
    LOG4CPLUS_INFO(getApplicationLogger(), "Init Chamber");
    //
    cgicc::Cgicc cgi(in);
    //
    int tmb, dmb;
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "No dmb" << endl;
      dmb = DMB_;
    }
    //
    name = cgi.getElement("tmb");
    //
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "No tmb" << endl;
      tmb = TMB_;
    }
    //
    //DAQMB * thisDMB = dmbVector[dmb];
    //TMB * thisTMB = tmbVector[tmb];
    //
    //MyTest.SetTMB(thisTMB);
    //MyTest.SetDMB(thisDMB);
    //MyTest.SetCCB(thisCCB);
    //
    MyTest[tmb].InitSystem();          // Init chamber
    //
    thisCCB->setCCBMode(CCB::VMEFPGA);      // It needs to be in FPGA mode to work.
    //
    this->ChamberTests(in,out);
    //
  }
  //
  void EmuPeripheralCrate::CCBBoardID(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    CCBBoardID_= cgi["CCBBoardID"]->getValue() ;
    //
    this->CrateConfiguration(in,out);
    //
  }
  //
  //
  void EmuPeripheralCrate::TmbMPCTest(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    //CrateUtilities myCrateTest;
    myCrateTest.SetCrate(thisCrate);
    myCrateTest.MpcTMBTest(1000);
    //
    this->Default(in,out);
    //
  }
  //
  void EmuPeripheralCrate::MPCBoardID(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    MPCBoardID_= cgi["MPCBoardID"]->getValue() ;
    //
    this->CrateConfiguration(in,out);
    //
  }
  //
  void EmuPeripheralCrate::PowerUp(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    *out << h1("Run Valery's program");
    *out << cgicc::a("Voltages").set("href","http://emuslice03:1973/urn:xdaq-application:lid=30/") << endl;
    //
  }
  //
  void EmuPeripheralCrate::Operator(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    cgicc::Cgicc cgi(in);
    //
    Operator_= cgi["Operator"]->getValue() ;
    //
    this->Default(in,out);
  }
  //
  void EmuPeripheralCrate::RunNumber(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    cgicc::Cgicc cgi(in);
    //
    RunNumber_= cgi["RunNumber"]->getValue() ;
    std::cout << "RunNumber " << RunNumber_ << std::endl ;
    LOG4CPLUS_INFO(getApplicationLogger(), "RunNumber");
    //
    this->Default(in,out);
  }
  //
  void EmuPeripheralCrate::DMBTestAll(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    DAQMB * thisDMB = dmbVector[dmb];
    //
    OutputDMBTests[dmb] << "DMB Test3" << endl ;
    //
    thisDMB->RedirectOutput(&OutputDMBTests[dmb]);
    thisDMB->test3();
    thisDMB->test4();
    thisDMB->test5();
    thisDMB->test6();
    thisDMB->test8();
    thisDMB->test9();
    thisDMB->test10();
    thisDMB->RedirectOutput(&std::cout);
    //
    this->DMBTests(in,out);
    //
  }
  //
  void EmuPeripheralCrate::DMBTest3(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    OutputDMBTests[dmb] << "DMB Test3" << endl ;
    //
    DAQMB * thisDMB = dmbVector[dmb];
    //
    thisDMB->RedirectOutput(&OutputDMBTests[dmb]);
    thisDMB->test3();
    thisDMB->RedirectOutput(&std::cout);
    //
    this->DMBTests(in,out);
    //
  }
  //
  void EmuPeripheralCrate::DMBTest4(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    DAQMB * thisDMB = dmbVector[dmb];
    //
    OutputDMBTests[dmb] << "DMB Test4" << endl ;
    //
    thisDMB->RedirectOutput(&OutputDMBTests[dmb]);
    thisDMB->test4();
    thisDMB->RedirectOutput(&std::cout);
    //
    this->DMBTests(in,out);
    //
  }
  //
  void EmuPeripheralCrate::DMBTest5(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    DAQMB * thisDMB = dmbVector[dmb];
    //
    OutputDMBTests[dmb] << "DMB Test5" << endl ;
    //
    thisDMB->RedirectOutput(&OutputDMBTests[dmb]);
    thisDMB->test5();
    thisDMB->RedirectOutput(&std::cout);
    //
    this->DMBTests(in,out);
    //
  }
  //
  void EmuPeripheralCrate::DMBTest6(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    OutputDMBTests[dmb] << "DMB Test6" << endl ;
    //
    DAQMB * thisDMB = dmbVector[dmb];
    //
    thisDMB->RedirectOutput(&OutputDMBTests[dmb]);
    thisDMB->test6();
    thisDMB->RedirectOutput(&std::cout);
    //
    this->DMBTests(in,out);
    //
  }
  //
  void EmuPeripheralCrate::DMBTest8(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    OutputDMBTests[dmb] << "DMB Test8" << endl ;
    //
    DAQMB * thisDMB = dmbVector[dmb];
    //
    thisDMB->RedirectOutput(&OutputDMBTests[dmb]);
    thisDMB->test8();
    thisDMB->RedirectOutput(&std::cout);
    //
    this->DMBTests(in,out);
  }
  //
  void EmuPeripheralCrate::DMBTest9(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    OutputDMBTests[dmb] << "DMB Test9" << endl ;
    //
    DAQMB * thisDMB = dmbVector[dmb];
    //
    thisDMB->RedirectOutput(&OutputDMBTests[dmb]);
    thisDMB->test9();
    thisDMB->RedirectOutput(&std::cout);
    //
    this->DMBTests(in,out);
  }
  //
  void EmuPeripheralCrate::DMBTest10(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    OutputDMBTests[dmb] << "DMB Test10" << endl ;
    //
    DAQMB * thisDMB = dmbVector[dmb];
    //
    thisDMB->RedirectOutput(&OutputDMBTests[dmb]);
    thisDMB->test10();
    thisDMB->RedirectOutput(&std::cout);
    //
    this->DMBTests(in,out);
  }
  //
  void EmuPeripheralCrate::DMBTest11(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    OutputDMBTests[dmb] << "DMB Test11" << endl ;
    //
    DAQMB * thisDMB = dmbVector[dmb];
    //
    thisDMB->RedirectOutput(&std::cout);
    thisDMB->test11();
    thisDMB->RedirectOutput(&std::cout);
    //
    this->DMBTests(in,out);
  }
  //
  void EmuPeripheralCrate::DMBPrintCounters(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    DAQMB * thisDMB = dmbVector[dmb];
    //
    thisDMB->RedirectOutput(&std::cout);
    thisDMB->PrintCounters(1);
    thisDMB->RedirectOutput(&std::cout);
    //
    this->DMBUtils(in,out);
  }
  //
  void EmuPeripheralCrate::ChamberTests(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    int tmb, dmb;
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "No dmb" << endl;
      dmb = DMB_;
    }
    //
    name = cgi.getElement("tmb");
    //
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "No tmb" << endl;
      tmb = TMB_;
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    DAQMB * thisDMB = dmbVector[dmb];
    alct = thisTMB->alctController();
    //
    char Name[50];
    sprintf(Name,"Chamber tests TMBslot=%d DMBslot=%d",thisTMB->slot(),thisDMB->slot());
    //
    MyHeader(in,out,Name);
    //
    //*out << cgicc::h1(Name);
    //*out << cgicc::br();
    //
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend("CSC trigger primitive synchronization and tests").set("style","color:blue") << cgicc::p() << std::endl ;
    //
    char buf[20];
    //
    //    std::string Automatic =
    //      toolbox::toString("/%s/Automatic",getApplicationDescriptor()->getURN().c_str());
    //    //
    //    *out << cgicc::form().set("method","GET").set("action",Automatic) << std::endl ;
    //    *out << cgicc::input().set("type","submit").set("value","Automatic") << std::endl ;
    //    sprintf(buf,"%d",tmb);
    //    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    //    sprintf(buf,"%d",dmb);
    //    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    //    *out << cgicc::form() << std::endl ;
    //
    std::string InitChamber =
      toolbox::toString("/%s/InitChamber",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",InitChamber) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Init Chamber") << std::endl ;
    *out << cgicc::input().set("type","hidden").set("value","1").set("name","navigator");
    *out << cgicc::form() << std::endl ;
    //
    std::string TMBStartTrigger =
      toolbox::toString("/%s/TMBStartTrigger",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",TMBStartTrigger) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","TMBStartTrigger") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    std::string EnableL1aRequest =
      toolbox::toString("/%s/EnableL1aRequest",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",EnableL1aRequest) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","EnableL1aRequest") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    //
    *out << cgicc::pre();
    *out << "----------- Measure relative clock phases with pulsing ---------------" << std::endl;
    *out << cgicc::pre();
    //
    //
    std::string ALCTTiming =
      toolbox::toString("/%s/ALCTTiming",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",ALCTTiming) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","ALCT Timing") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << "alct_rx_clock_phase = " << MyTest[tmb].GetALCTrxPhaseTest() <<  " (" << MyTest[tmb].GetALCTrxPhase() << ") " << std::endl;
    *out << cgicc::br();
    *out << "alct_tx_clock_phase = " << MyTest[tmb].GetALCTtxPhaseTest() <<  " (" << MyTest[tmb].GetALCTtxPhase() << ") " << std::endl;
    *out << cgicc::br();
    *out << cgicc::br();
    //
    //
    std::string CFEBTiming =
      toolbox::toString("/%s/CFEBTiming",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",CFEBTiming) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","CFEB Timing") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    for(int i=0;i<5;i++) {
      *out << "cfeb" << i << "delay = " << MyTest[tmb].GetCFEBrxPhaseTest(i) << " ("  << MyTest[tmb].GetCFEBrxPhase(i) << ") " <<std::endl;
      *out << cgicc::br();
    }
    *out << cgicc::br();
    //
    //
    std::string RatTmbTiming =
      toolbox::toString("/%s/RatTmbTiming",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",RatTmbTiming) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","RAT TMB Timing") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << "rat_tmb_delay = " << MyTest[tmb].GetRatTmbDelayTest() << " ("  << MyTest[tmb].GetRatTmbDelay()     << ") " << std::endl;
    *out << cgicc::br();
    *out << cgicc::br();
    //
    //
    *out << cgicc::pre();
    *out << "------------- Measure CLCT-ALCT match timing with cosmic rays --------------" << std::endl;
    *out << cgicc::pre();
    //
    //
    std::string ALCTvpf = 
      toolbox::toString("/%s/ALCTvpf",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",ALCTvpf) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Find ALCT vpf") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << "match_trig_alct_delay = " << MyTest[tmb].GetALCTvpf() << " ("  << MyTest[tmb].GetALCTvpf_configvalue() << ") " << std::endl;
    *out << cgicc::br();
    *out << cgicc::br();
    //
    //
    *out << cgicc::pre();
    *out << "--- Now, make sure LCTs are lined up at sector processor..." << std::endl;
    *out << "... then, measure the delay needed for the winner bit from MPC back to TMB ---" << std::endl;
    *out << cgicc::pre();
    //
    //
    std::string FindWinner =
      toolbox::toString("/%s/FindWinner",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",FindWinner) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Find Winner bits") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << "mpc_rx_delay = " << MyTest[tmb].GetMPCdelayTest() << " ("  << MyTest[tmb].GetMPCdelay()     << ") " << std::endl;
    *out << cgicc::br();
    *out << cgicc::br();
    //
    //
    *out << cgicc::pre();
    *out << "---- Determine the timing of the L1A back to the CFEB with respect to the Active FEB flag -----" << std::endl;
    *out << cgicc::pre();
    //
    //
    std::string AFFtoL1aAtDMB =
      toolbox::toString("/%s/AFFtoL1aAtDMB",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",AFFtoL1aAtDMB) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Active FEB Flag to L1A timing") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << "Active FEB flag to L1A delay = " << MyTest[tmb].GetActiveFebFlagToL1aAtDMB() << std::endl;
    *out << cgicc::br();
    *out << cgicc::br();
    //
    //
    *out << cgicc::pre();
    *out << "---------- Determine the L1A arrival windows at the TMB and ALCT ------------" << std::endl;
    *out << cgicc::pre();
    //
    //
    std::string TMBL1aTiming =
      toolbox::toString("/%s/TMBL1aTiming",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",TMBL1aTiming) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","TMB L1a Timing") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << "tmb_l1a_delay = " << MyTest[tmb].GetTMBL1aTiming() << " ("  << MyTest[tmb].GetTMBL1aTiming_configvalue() << ") " << std::endl;
    *out << cgicc::br();
    *out << cgicc::br();
    //
    //
    std::string ALCTL1aTiming =
      toolbox::toString("/%s/ALCTL1aTiming",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",ALCTL1aTiming) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","ALCT L1a Timing") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << "alct_l1a_delay = " << MyTest[tmb].GetALCTL1aDelay() << " ("  << MyTest[tmb].GetALCTL1aDelay_configvalue() << ") " << std::endl;
    *out << cgicc::br();
    *out << cgicc::br();
    //
    //
    std::string RpcRatTiming =
      toolbox::toString("/%s/RpcRatTiming",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",RpcRatTiming) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","RPC-RAT Timing") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << "rpc0_rat_delay = " << MyTest[tmb].GetRpcRatDelayTest() << " ("  << MyTest[tmb].GetRpcRatDelay()     << ") " << std::endl;
    *out << cgicc::br();
    *out << cgicc::br();
    //
    //
    *out << cgicc::pre();
    *out << "------ Scans over on-chamber electronics with pulsing ------------" << std::endl;
    *out << cgicc::pre();
    //
    //
    std::string ALCTScan =
      toolbox::toString("/%s/ALCTScan",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",ALCTScan) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","ALCT Scan") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    for (int Wire = 0; Wire<(alct->GetNumberOfChannelsInAlct())/6; Wire++) 
      *out << MyTest[tmb].GetALCTWireScan(Wire) ;
    *out << std::endl;
    *out << cgicc::br();
    //
    //
    std::string CFEBScan = 
      toolbox::toString("/%s/CFEBScan",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",CFEBScan) << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::input().set("type","submit").set("value","CFEB Scan") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    for (int CFEBs = 0; CFEBs<5; CFEBs++) {
      *out << "CFEB Id="<<CFEBs<< " " ;
      for (int HalfStrip = 0; HalfStrip<32; HalfStrip++) {
	*out << MyTest[tmb].GetCFEBStripScan(CFEBs,HalfStrip) ;
      }
      *out << std::endl;
      *out << cgicc::br();
    }
    *out << cgicc::br();
    //
    //
    *out << cgicc::fieldset();
    *out << std::endl;
    //
    *out << cgicc::form().set("method","GET") << std::endl ;
    *out << cgicc::textarea().set("name","CrateTestOutput")
      .set("WRAP","OFF")
      .set("rows","20").set("cols","100");
    *out << CrateTestsOutput[tmb].str() << endl ;
    *out << cgicc::textarea();
    *out << cgicc::form() << std::endl ;
    //
    std::string LogCrateTestsOutput = toolbox::toString("/%s/LogCrateTestsOutput",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",LogCrateTestsOutput) << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::input().set("type","submit")
    .set("value","Log output").set("name","LogCrateTestsOutput") << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Clear")
      .set("name","ClearCrateTestsOutput") << std::endl ;
    *out << cgicc::form() << std::endl ;
  }
//
//
  //
  void EmuPeripheralCrate::TMBStartTrigger(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    int tmb, dmb;
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "No dmb" << endl;
    }
    //
    name = cgi.getElement("tmb");
    //
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "No tmb" << endl;
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    //
    thisTMB->StartTTC();
    //
    this->ChamberTests(in,out);
    //
  }
  //
    void EmuPeripheralCrate::EnableL1aRequest(xgi::Input * in, xgi::Output * out ) 
      throw (xgi::exception::Exception)
  {
    //
    cout << "Enable L1a request" << endl ;
    //
    cgicc::Cgicc cgi(in);
    //
    int tmb, dmb;
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "No dmb" << endl;
    }
    //
    name = cgi.getElement("tmb");
    //
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "No tmb" << endl;
    }
    //
    DAQMB * thisDMB = dmbVector[dmb];
    TMB * thisTMB = tmbVector[tmb];
    //
    std::cout << "DMB slot " << thisDMB->slot() << " TMB slot " << thisTMB->slot() << std::endl;
    //
    thisTMB->EnableL1aRequest();
    thisCCB->setCCBMode(CCB::VMEFPGA);
    //
    this->ChamberTests(in,out);
    //
  }
  //
  void EmuPeripheralCrate::ALCTTiming(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "ALCTTiming" << endl;
    LOG4CPLUS_INFO(getApplicationLogger(), "ALCTTiming");
    //
    cgicc::Cgicc cgi(in);
    //
    int tmb, dmb;
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "No dmb" << endl;
      dmb = DMB_;
    }
    //
    name = cgi.getElement("tmb");
    //
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "No tmb" << endl;
      tmb = TMB_;
    }
    //
    //DAQMB * thisDMB = dmbVector[dmb];
    //TMB * thisTMB = tmbVector[tmb];
    //
    //MyTest.SetTMB(thisTMB);
    //MyTest.SetCCB(thisCCB);
    //
    MyTest[tmb].RedirectOutput(&CrateTestsOutput[tmb]);
    MyTest[tmb].ALCTTiming();
    MyTest[tmb].RedirectOutput(&std::cout);
    //
    this->ChamberTests(in,out);
    //
  }
  //
  void EmuPeripheralCrate::Automatic(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "Automatic" << endl;
    LOG4CPLUS_INFO(getApplicationLogger(), "Automatic");
    //
    cgicc::Cgicc cgi(in);
    //
    int tmb, dmb;
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "No dmb" << endl;
      dmb = DMB_;
    }
    //
    name = cgi.getElement("tmb");
    //
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "No tmb" << endl;
      tmb = TMB_;
    }
    //
    //DAQMB * thisDMB = dmbVector[dmb];
    //TMB * thisTMB = tmbVector[tmb];
    //
    //MyTest.SetTMB(thisTMB);
    //MyTest.SetDMB(thisDMB);
    //MyTest.SetCCB(thisCCB);
    //
    //int RXphase, TXphase;
    //
    MyTest[tmb].Automatic();
    //
    this->ChamberTests(in,out);
    //
  }
  //
  void EmuPeripheralCrate::CFEBTiming(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "CFEBTiming" << endl;
    LOG4CPLUS_INFO(getApplicationLogger(), "CFEBTiming");
    //
    cgicc::Cgicc cgi(in);
    //
    int tmb, dmb;
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "No dmb" << endl;
      dmb = DMB_;
    }
    //
    name = cgi.getElement("tmb");
    //
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "No tmb" << endl;
      tmb = TMB_;
    }
    //
    //DAQMB * thisDMB = dmbVector[dmb];
    //TMB * thisTMB = tmbVector[tmb];
    //
    //MyTest.SetTMB(thisTMB);
    //MyTest.SetDMB(thisDMB);
    //MyTest.SetCCB(thisCCB);
    //
    MyTest[tmb].RedirectOutput(&CrateTestsOutput[tmb]);
    MyTest[tmb].CFEBTiming();
    MyTest[tmb].RedirectOutput(&std::cout);
    //
    this->ChamberTests(in,out);
    //
  }
  //
  void EmuPeripheralCrate::TMBL1aTiming(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
    {
      //
      cout << "TMBL1aTiming" << endl;
      LOG4CPLUS_INFO(getApplicationLogger(), "TMBL1aTiming");
      //
      cgicc::Cgicc cgi(in);
      //
      int tmb, dmb;
      //
      cgicc::form_iterator name = cgi.getElement("dmb");
      //
      if(name != cgi.getElements().end()) {
	dmb = cgi["dmb"]->getIntegerValue();
	cout << "DMB " << dmb << endl;
	DMB_ = dmb;
      } else {
	cout << "No dmb" << endl;
	dmb = DMB_;
      }
      //
      name = cgi.getElement("tmb");
      //
      if(name != cgi.getElements().end()) {
	tmb = cgi["tmb"]->getIntegerValue();
	cout << "TMB " << tmb << endl;
	TMB_ = tmb;
      } else {
	cout << "No tmb" << endl;
	tmb = TMB_;
      }
      //
      //DAQMB * thisDMB = dmbVector[dmb];
      //TMB * thisTMB = tmbVector[tmb];
      //
      //MyTest.SetTMB(thisTMB);
      //MyTest.SetDMB(thisDMB);
      //MyTest.SetCCB(thisCCB);
      //
      MyTest[tmb].RedirectOutput(&CrateTestsOutput[tmb]);
      MyTest[tmb].FindTMB_L1A_delay(150,165);
      MyTest[tmb].RedirectOutput(&std::cout);
      //
      this->ChamberTests(in,out);
      //
    }
  //
  void EmuPeripheralCrate::ALCTL1aTiming(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "ALCTL1aTiming" << endl;
    LOG4CPLUS_INFO(getApplicationLogger(), "ALCTL1aTiming");
    //
    cgicc::Cgicc cgi(in);
    //
    int tmb, dmb;
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "No dmb" << endl;
      dmb = DMB_;
    }
    //
    name = cgi.getElement("tmb");
    //
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "No tmb" << endl;
      tmb = TMB_;
    }
    //
    //DAQMB * thisDMB = dmbVector[dmb];
    //TMB * thisTMB = tmbVector[tmb];
    //
    //MyTest.SetTMB(thisTMB);
    //MyTest.SetDMB(thisDMB);
    //MyTest.SetCCB(thisCCB);
    //
    MyTest[tmb].RedirectOutput(&CrateTestsOutput[tmb]);
    MyTest[tmb].FindALCT_L1A_delay(155,170);
    MyTest[tmb].RedirectOutput(&std::cout);
    //
    this->ChamberTests(in,out);
    //
  }
  //
  void EmuPeripheralCrate::ALCTvpf(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "ALCTvpf" << endl;
    LOG4CPLUS_INFO(getApplicationLogger(), "ALCTvpf");
    //
    cgicc::Cgicc cgi(in);
    //
    int tmb, dmb;
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "No dmb" << endl;
    }
    //
    name = cgi.getElement("tmb");
    //
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "No tmb" << endl;
    }
    //
    //DAQMB * thisDMB = dmbVector[dmb];
    //TMB * thisTMB = tmbVector[tmb];
    //
    //MyTest.SetTMB(thisTMB);
    //MyTest.SetDMB(thisDMB);
    //MyTest.SetCCB(thisCCB);
    //
    MyTest[tmb].RedirectOutput(&CrateTestsOutput[tmb]);
    MyTest[tmb].FindALCTvpf();
    MyTest[tmb].RedirectOutput(&std::cout);
    //
    this->ChamberTests(in,out);
    //
  }
  //
  void EmuPeripheralCrate::ALCTScan(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "ALCTScan" << endl;
    LOG4CPLUS_INFO(getApplicationLogger(), "ALCTScan");
    //
    cgicc::Cgicc cgi(in);
    //
    int tmb, dmb;
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "No dmb" << endl;
    }
    //
    name = cgi.getElement("tmb");
    //
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "No tmb" << endl;
    }
    //
    //DAQMB * thisDMB = dmbVector[dmb];
    //TMB * thisTMB = tmbVector[tmb];
    //
    //MyTest.SetTMB(thisTMB);
    //MyTest.SetCCB(thisCCB);
    //
    //MyTest[tmb].RedirectOutput(&CrateTestsOutput[tmb]);
    MyTest[tmb].RedirectOutput(&std::cout);
    MyTest[tmb].ALCTChamberScan();
    MyTest[tmb].RedirectOutput(&std::cout);
    //
    this->ChamberTests(in,out);
    //
  }
  //
  void EmuPeripheralCrate::CFEBScan(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "CFEBScan" << endl;
    LOG4CPLUS_INFO(getApplicationLogger(), "CFEBScan");
    //
    cgicc::Cgicc cgi(in);
    //
    int tmb, dmb;
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "No dmb" << endl;
    }
    //
    name = cgi.getElement("tmb");
    //
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "No tmb" << endl;
    }
    //
    //DAQMB * thisDMB = dmbVector[dmb];
    //TMB * thisTMB = tmbVector[tmb];
    //
    //MyTest.SetTMB(thisTMB);
    //MyTest.SetCCB(thisCCB);
    //MyTest.SetDMB(thisDMB);
    //
    MyTest[tmb].RedirectOutput(&CrateTestsOutput[tmb]);
    MyTest[tmb].CFEBChamberScan();
    MyTest[tmb].RedirectOutput(&std::cout);
    //
    this->ChamberTests(in,out);
    //
  }
  //
  void EmuPeripheralCrate::FindWinner(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "FindWinner" << endl;
    LOG4CPLUS_INFO(getApplicationLogger(), "FindWinner");
    //
    cgicc::Cgicc cgi(in);
    //
    int tmb, dmb;
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "No dmb" << endl;
    }
    //
    name = cgi.getElement("tmb");
    //
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "No tmb" << endl;
    }
    //
    //DAQMB * thisDMB = dmbVector[dmb];
    //TMB * thisTMB = tmbVector[tmb];
    //
    //MyTest.SetTMB(thisTMB);
    //MyTest.SetCCB(thisCCB);
    //MyTest.SetDMB(thisDMB);
    //MyTest.SetMPC(thisMPC);
    //
    MyTest[tmb].RedirectOutput(&CrateTestsOutput[tmb]);
    MyTest[tmb].FindWinner(2);
    MyTest[tmb].RedirectOutput(&std::cout);
    //
    this->ChamberTests(in,out);
    //
  }
//
  void EmuPeripheralCrate::AFFtoL1aAtDMB(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "Measure Active FEB Flag to L1A timing at DMB" << endl;
    //    LOG4CPLUS_INFO(getApplicationLogger(), "Measure Active FEB Flag to L1A timing at DMB");
    //
    cgicc::Cgicc cgi(in);
    //
    int tmb, dmb;
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "No dmb" << endl;
    }
    //
    name = cgi.getElement("tmb");
    //
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "No tmb" << endl;
    }
    //
    //DAQMB * thisDMB = dmbVector[dmb];
    //TMB * thisTMB = tmbVector[tmb];
    //
    //MyTest.SetTMB(thisTMB);
    //MyTest.SetCCB(thisCCB);
    //MyTest.SetDMB(thisDMB);
    //MyTest.SetMPC(thisMPC);
    //
    MyTest[tmb].RedirectOutput(&CrateTestsOutput[tmb]);
    MyTest[tmb].MeasureTimingOfActiveFebFlagToL1aAtDMB(140,155);
    MyTest[tmb].RedirectOutput(&std::cout);
    //
    this->ChamberTests(in,out);
    //
  }
  //
  void EmuPeripheralCrate::RatTmbTiming(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "RatTmbTiming" << endl;
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "No tmb" << endl;
    }
    //
    //DAQMB * thisDMB = dmbVector[dmb];
    //TMB * thisTMB = tmbVector[tmb];
    //
    //MyTest.SetTMB(thisTMB);
    //MyTest.SetCCB(thisCCB);
    //MyTest.SetDMB(thisDMB);
    //MyTest.SetMPC(thisMPC);
    //
    MyTest[tmb].RedirectOutput(&CrateTestsOutput[tmb]);
    MyTest[tmb].RatTmbDelayScan();
    MyTest[tmb].RedirectOutput(&std::cout);
    //
    this->ChamberTests(in,out);
    //
  }
  //
  void EmuPeripheralCrate::RpcRatTiming(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "RpcRatTiming" << endl;
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "No tmb" << endl;
    }
    //
    MyTest[tmb].RedirectOutput(&CrateTestsOutput[tmb]);
    MyTest[tmb].RpcRatDelayScan();
    MyTest[tmb].RedirectOutput(&std::cout);
    //
    this->ChamberTests(in,out);
    //
  }
  //
  void EmuPeripheralCrate::MakeReference(xgi::Input * in , xgi::Output * out )
  {
    //
    cgicc::Cgicc cgi(in);
    //
    const CgiEnvironment& env = cgi.getEnvironment();
    //
    std::string dmbStr = env.getQueryString() ;
    //int dmb = atoi(dmbStr.c_str());
    //
    std::string test =  env.getReferrer() ;
    cout << test << endl ;
    //
    *out << cgicc::a("Back").set("href",test) << endl;
  }
  //
  void EmuPeripheralCrate::DMBTurnOff(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    DAQMB * thisDMB = dmbVector[dmb];
    //
    cout << "DMBTurnOff" << endl;
    //
    if (thisDMB) {
      thisDMB->lowv_onoff(0x0);
    }
    //
    this->DMBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::DMBLoadFirmware(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    //    DAQMB * thisDMB = dmbVector[dmb];
    /*
    int mindmb = dmb;
    int maxdmb = dmb+1;
      if (thisDMB->slot() == 25) { //if DMB slot = 25, loop over each dmb
      mindmb = 0;
      maxdmb = dmbVector.size()-1;
      }*/
    //
    thisCCB->hardReset();
    //
    //    for (dmb=mindmb; dmb<maxdmb; dmb++) {
      //
      DAQMB * thisDMB = dmbVector[dmb];
      //
      if (thisDMB) {
	//
	cout << "DMBLoadFirmware in slot " << thisDMB->slot() << endl;
	if (thisDMB->slot()==25) cout <<" Broadcast Loading the control FPGA insode one crate"<<endl;
	//
	::sleep(1);
	unsigned short int dword[2];
	dword[0]=0;
	//
	char *outp=(char *)dword;
	//char *name = DMBFirmware_.toString().c_str() ;
	thisDMB->epromload(MPROM,DMBFirmware_.toString().c_str(),1,outp);  // load mprom
	//
      }
      //    }
    ::sleep(5);
    thisCCB->hardReset();
    //
    this->DMBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::DMBVmeLoadFirmware(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    DAQMB * thisDMB = dmbVector[dmb];
    //
    int mindmb = dmb;
    int maxdmb = dmb+1;
    if (thisDMB->slot() == 25) { //if DMB slot = 25, loop over each dmb
      mindmb = 0;
      maxdmb = dmbVector.size()-1;
    }
    //
    thisCCB->hardReset();
    //
    for (dmb=mindmb; dmb<maxdmb; dmb++) {
      //
      thisDMB = dmbVector[dmb];
      //
      if (thisDMB) {
	//
	cout << "DMBVmeLoadFirmware in slot " << thisDMB->slot() << endl;
	//
	::sleep(1);
	//
	unsigned short int dword[2];
	dword[0]=thisDMB->mbpromuser(0);
	// dword[0] = 0x01bd;
	// dword[1] = 0xff00;  to manually change the DMB ID.
	char * outp=(char *)dword;   // recast dword
	thisDMB->epromload(VPROM,DMBVmeFirmware_.toString().c_str(),1,outp);  // load mprom
	//Test the random trigger
	//	thisDMB->set_rndmtrg_rate(-1);
	//	thisDMB->set_rndmtrg_rate(-1);
	//	thisDMB->toggle_rndmtrg_start();

      }
      //
    }
    ::sleep(1);
    thisCCB->hardReset(); //disable this when testing the random_trigger
    //
    this->DMBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::DMBVmeLoadFirmwareEmergency(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    int dmbNumber = 0;
    //
    cgicc::form_iterator name2 = cgi.getElement("DMBNumber");
    //int registerValue = -1;
    if(name2 != cgi.getElements().end()) {
      dmbNumber = cgi["DMBNumber"]->getIntegerValue();
      //
    }
    //
    std::cout << "Loading DMB# " <<dmbNumber << std::endl ;

    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    DAQMB * thisDMB = dmbVector[dmb];
    //
    int mindmb = dmb;
    int maxdmb = dmb+1;
    if (thisDMB->slot() == 25) { //if DMB slot = 25, loop over each dmb
      cout <<" The emergency load is NOT available for DMB slot25"<<endl;
      cout <<" Please use individual slot loading !!!"<<endl;
      return;
    }
    //
    thisCCB->hardReset();
    if (thisDMB) {
	//
      cout << "DMB Vme Load Firmware Emergency in slot " << thisDMB->slot() << endl;
      LOG4CPLUS_INFO(getApplicationLogger(),"Started DMB Vme Load Firmware Emergency");
	//
      ::sleep(1);
	//
      unsigned short int dword[2];
      dword[0]=dmbNumber&0x03ff;
      dword[1]=0xDB00;
      char * outp=(char *)dword;  
      thisDMB->epromload(RESET,DMBVmeFirmware_.toString().c_str(),1,outp);  // load mprom
    }
    ::sleep(1);
    thisCCB->hardReset();
    //
    //
    this->DMBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::CFEBLoadFirmware(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    LOG4CPLUS_INFO(getApplicationLogger(),"Started CFEB firmware download");
    //
    cgicc::Cgicc cgi(in);
    //
    int dmbNumber = -1;
    //
    cgicc::form_iterator name2 = cgi.getElement("DMBNumber");
    //int registerValue = -1;
    if(name2 != cgi.getElements().end()) {
      dmbNumber = cgi["DMBNumber"]->getIntegerValue();
      //
    }
    //
    std::cout << "Loading DMBNumber " <<dmbNumber << std::endl ;
    //*out << "Loading DMBNumber " <<dmbNumber ;
    //*out << cgicc::br();
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    DAQMB * thisDMB = dmbVector[dmb];
    int mindmb = dmb;
    int maxdmb = dmb+1;
    if (thisDMB->slot() == 25) { //if DMB slot = 25, loop over each cfeb
      mindmb = 0;
      maxdmb = dmbVector.size()-1;
    }
    for (dmb=mindmb; dmb<maxdmb; dmb++) {
      //
      thisDMB = dmbVector[dmb];
      //
      cout << "CFEBLoadFirmware - DMB " << dmb << endl;
      //
      thisCCB->hardReset();
      //
      if (thisDMB) {
	//
	vector<CFEB> thisCFEBs = thisDMB->cfebs();
	//
	::sleep(1);
	//
	if (dmbNumber == -1 ) {
	  for (unsigned int i=0; i<thisCFEBs.size(); i++) {
	    ostringstream dum;
	    dum << "loading CFEB firmware for DMB=" << dmb << " CFEB="<< i << std::endl;
	    LOG4CPLUS_INFO(getApplicationLogger(), dum.str());
	    unsigned short int dword[2];
	    dword[0]=thisDMB->febpromuser(thisCFEBs[i]);
	    CFEBid_[dmb][i] = dword[0];  // fill summary file with user ID value read from this CFEB
	    char * outp=(char *)dword;   // recast dword
	    thisDMB->epromload(thisCFEBs[i].promDevice(),CFEBFirmware_.toString().c_str(),1,outp);  // load mprom
	  }
	} else {
	  std::cout << "loading CFEB firmware for DMB=" << dmb << " CFEB="<< dmbNumber << std::endl;
	  unsigned short int dword[2];
	  for (unsigned int i=0; i<thisCFEBs.size(); i++) {
	    if (thisCFEBs[i].number() == dmbNumber ) {
	      dword[0]=thisDMB->febpromuser(thisCFEBs[i]);
	      CFEBid_[dmb][i] = dword[0];  // fill summary file with user ID value read from this CFEB
	      char * outp=(char *)dword;   // recast dword
	      thisDMB->epromload(thisCFEBs[i].promDevice(),CFEBFirmware_.toString().c_str(),1,outp);  // load mprom
	    }
	  }
	}
      }
      ::sleep(1);
      thisCCB->hardReset();
    }
    //
    this->DMBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::DMBTurnOn(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    DAQMB * thisDMB = dmbVector[dmb];
    //
    cout << "DMBTurnOn" << endl;
    //
    if (thisDMB) {
      thisDMB->lowv_onoff(0x3f);
    }
    //
    this->DMBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::TMBPrintCounters(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "TMBPrintCounters "<< endl;
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    //
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    //
    LOG4CPLUS_INFO(getApplicationLogger(), "Start PrintCounters");
    thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
    thisTMB->GetCounters();
    thisTMB->PrintCounters();
    thisTMB->RedirectOutput(&std::cout);
    LOG4CPLUS_INFO(getApplicationLogger(), "Done PrintCounters");
    //
    this->TMBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::TMBResetCounters(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    //
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    thisTMB->ResetCounters();
    //
    this->TMBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::TriggerTestInjectALCT(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    //
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
    thisTMB->TriggerTestInjectALCT();
    thisTMB->RedirectOutput(&std::cout);
    //
    this->TMBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::armScope(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    //
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
    thisTMB->scope(1,0,29);
    thisTMB->RedirectOutput(&std::cout);
    //
    this->TMBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::forceScope(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    //
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
    thisTMB->ForceScopeTrigger();
    thisTMB->RedirectOutput(&std::cout);
    //
    this->TMBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::readoutScope(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    //
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
    thisTMB->ClearScintillatorVeto();
    thisTMB->scope(0,1);
    thisTMB->RedirectOutput(&std::cout);
    //
    this->TMBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::TriggerTestInjectCLCT(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    //
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
    thisTMB->TriggerTestInjectCLCT();
    thisTMB->RedirectOutput(&std::cout);
    //
    this->TMBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::TMBDumpAllRegisters(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    //
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
    thisTMB->DumpAllRegisters();
    thisTMB->RedirectOutput(&std::cout);
    //
    this->TMBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::TMBReadConfiguration(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    //
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    ALCTController * alct = thisTMB->alctController();
    //
    thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
    alct->RedirectOutput(&OutputStringTMBStatus[tmb]);
    //
    thisTMB->ReadTMBConfiguration();
    thisTMB->PrintTMBConfiguration();
    alct->ReadALCTConfiguration();
    alct->PrintALCTConfiguration();
    //
    thisTMB->RedirectOutput(&std::cout);
    alct->RedirectOutput(&std::cout);
    //
    this->TMBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::TMBCheckConfiguration(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    //
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    ALCTController * alct = thisTMB->alctController();
    //
    thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
    alct->RedirectOutput(&OutputStringTMBStatus[tmb]);
    //
    thisTMB->CheckTMBConfiguration();
    alct->CheckALCTConfiguration();
    //
    thisTMB->RedirectOutput(&std::cout);
    alct->RedirectOutput(&std::cout);
    //
    this->TMBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::TMBReadStateMachines(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    //
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    //
    thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
    //
    thisTMB->ReadVMEStateMachine();
    thisTMB->PrintVMEStateMachine();
    thisTMB->ReadJTAGStateMachine();
    thisTMB->PrintJTAGStateMachine();
    thisTMB->ReadDDDStateMachine();
    thisTMB->PrintDDDStateMachine();
    thisTMB->ReadRawHitsHeader();
    thisTMB->PrintRawHitsHeader();
    //
    thisTMB->RedirectOutput(&std::cout);

    //
    this->TMBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::TMBCheckStateMachines(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    //
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    //
    thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
    //
    thisTMB->CheckVMEStateMachine();
    thisTMB->CheckJTAGStateMachine();
    thisTMB->CheckDDDStateMachine();
    thisTMB->CheckRawHitsHeader();
    //
    thisTMB->RedirectOutput(&std::cout);
    //
    this->TMBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::ALCTStatus(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "Not tmb" << endl ;
      tmb = TMB_;
    }
    //
    ALCTController * alct = tmbVector[tmb]->alctController();
    //
    MyHeader(in,out,"ALCTStatus");
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend("ALCT Status").set("style","color:blue") << cgicc::p() << std::endl ;
    //
    alct->ReadSlowControlId();
    //
    alct->RedirectOutput(out);
    alct->PrintSlowControlId();
    alct->RedirectOutput(&std::cout);
    //
    *out << cgicc::br();
    //
    alct->ReadFastControlId();
    //
    if (alct->GetFastControlRegularMirrorType() == alct->GetExpectedFastControlRegularMirrorType() &&
	alct->GetFastControlAlctType()          == alct->GetExpectedFastControlAlctType()          &&
	alct->GetFastControlYear()              == alct->GetExpectedFastControlYear()              &&
	alct->GetFastControlMonth()             == alct->GetExpectedFastControlMonth()             &&
	alct->GetFastControlDay()               == alct->GetExpectedFastControlDay()               ) {
      //
      // OK to this point... further checks for ME11...
      if ( (alct->GetChamberType()).find("ME11") != string::npos ) {
	//
	if (alct->GetFastControlBackwardForwardType()  == alct->GetExpectedFastControlBackwardForwardType() &&
	    alct->GetFastControlNegativePositiveType() == alct->GetExpectedFastControlNegativePositiveType() ) {
	  *out << cgicc::span().set("style","color:green");  //OK if in here and ME11
	} else {
	  *out << cgicc::span().set("style","color:red");    //not OK if didn't pass this check and ME11
	}
      } else {
	*out << cgicc::span().set("style","color:green");    //OK if in here and not ME11
      }
    } else { 
      //
      *out << cgicc::span().set("style","color:red");      //didn't pass first checks
    }
    //
    alct->RedirectOutput(out);
    alct->PrintFastControlId();
    alct->RedirectOutput(&std::cout);
    //
    *out << cgicc::span();
    *out << cgicc::fieldset();
    //
  }
  //
  void EmuPeripheralCrate::RATStatus(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    // NEED TO PUT IN RAT FIRMWARE GREEN/RED FLAG
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "Not tmb" << endl ;
      tmb = TMB_;
    }
    //
    RAT * rat = tmbVector[tmb]->getRAT();
    //
    MyHeader(in,out,"RATStatus");
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend("RAT Status").set("style","color:blue") << cgicc::p() << std::endl ;
    //
    *out << cgicc::br();
    //
    *out << cgicc::pre();
    rat->RedirectOutput(out);
    rat->ReadRatUser1();
    rat->PrintRatUser1();
    rat->RedirectOutput(&std::cout);
    *out << cgicc::pre();
    //
    *out << cgicc::fieldset();
    //
  }
  //
  void EmuPeripheralCrate::CFEBStatus(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "Not dmb" << endl ;
      dmb = DMB_;
    }
    //
    DAQMB * thisDMB = dmbVector[dmb];
    //
    MyHeader(in,out,"CFEBStatus");
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend("CFEB Status").set("style","color:blue") << cgicc::p() << std::endl ;
    //
    char buf[200];
    //
    std::vector<CFEB> cfebs = thisDMB->cfebs() ;
    //
    typedef std::vector<CFEB>::iterator CFEBItr;
    //
    for(CFEBItr cfebItr = cfebs.begin(); cfebItr != cfebs.end(); ++cfebItr) {
      sprintf(buf,"CFEB %d : ",(*cfebItr).number());
      *out << buf;
      //
      //*out << cgicc::br();
      //
      sprintf(buf,"CFEB prom user id : %08x CFEB fpga user id : %08x ",
	      (int)thisDMB->febpromuser(*cfebItr),
	      (int)thisDMB->febfpgauser(*cfebItr));
      //
      if ( thisDMB->febfpgauser(*cfebItr) == EXPECTED_CFEB_USERID ) {
	*out << cgicc::span().set("style","color:green");
	*out << buf;
	*out << cgicc::span();
      } else {
	*out << cgicc::span().set("style","color:red");
	*out << buf;
	*out << " (Should be 0x" << std::hex << EXPECTED_CFEB_USERID << ") ";
	*out << cgicc::span();
      }
      //
      *out << cgicc::br();
      //
    }
    //
    *out << cgicc::fieldset();
    //
    //thisDMB->cfebs_readstatus();
    //
    *out << std::endl;
    //
  }
  //
  void EmuPeripheralCrate::CCBStatus(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    char Name[50] ;
    sprintf(Name,"CCB Status slot=%d",thisCCB->slot());
    //
    MyHeader(in,out,Name);
    //
    *out << cgicc::h1(Name);
    *out << cgicc::br();
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend("CCB Info").set("style","color:blue") << cgicc::p() << std::endl ;
    //
    thisCCB->RedirectOutput(out);
    //
    thisCCB->firmwareVersion();
    *out << cgicc::br();
    //
    thisCCB->ReadTTCrxID();
    int readValue   = thisCCB->GetReadTTCrxID();
    int configValue = thisCCB->GetTTCrxID();
    //
    char buf[50];
    sprintf(buf,"Onboard TTCrx ID = %d",readValue);
    //
    if ( readValue == configValue ) {
      *out << cgicc::span().set("style","color:green");
      *out << buf;
      *out << cgicc::span();
    } else {
      *out << cgicc::span().set("style","color:red");
      *out << buf;
      *out << " (TTCrxID value in configuration incorrectly set to " << std::dec << configValue << ") ";
      *out << cgicc::span();
    }
    //
    thisCCB->RedirectOutput(&std::cout);
    //
    *out << cgicc::fieldset();
    //
  }
  //
  void EmuPeripheralCrate::ControllerBoardID(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    ControllerBoardID_= cgi["ControllerBoardID"]->getValue() ;
    //
    this->CrateConfiguration(in,out);
    //
  }
  //
  void EmuPeripheralCrate::CrateChassisID(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    CrateChassisID_= cgi["CrateChassisID"]->getValue() ;
    //
    this->CrateConfiguration(in,out);
    //
  }
  //
  void EmuPeripheralCrate::CrateRegulatorBoardID(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    CrateRegulatorBoardID_= cgi["CrateRegulatorBoardID"]->getValue() ;
    //
    this->CrateConfiguration(in,out);
    //
  }
  //
  void EmuPeripheralCrate::PeripheralCrateMotherBoardID(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    PeripheralCrateMotherBoardID_= cgi["PeripheralCrateMotherBoardID"]->getValue() ;
    //
    this->CrateConfiguration(in,out);
    //
  }
  //
  void EmuPeripheralCrate::ELMBID(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    ELMBID_= cgi["ELMBID"]->getValue() ;
    //
    this->CrateConfiguration(in,out);
    //
  }
  //
  void EmuPeripheralCrate::BackplaneID(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    BackplaneID_= cgi["BackplaneID"]->getValue() ;
    //
    this->CrateConfiguration(in,out);
    //
  }
  //
  void EmuPeripheralCrate::ControllerUtils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    char Name[50] ;
    sprintf(Name,"Controller Utils slot=%d",thisCCB->slot());
    //
    MyHeader(in,out,Name);
    //
    std::string EnableDisableDebug =
      toolbox::toString("/%s/EnableDisableDebug",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",EnableDisableDebug)
	 << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Enable/Disable Debug") 
	 << std::endl ;
    //
    if ( thisCrate->vmeController()->GetDebug() == 0 ) {
      *out << "Debug disabled";
    } else {
      *out << "Debug enabled";
    }
    //
    *out << cgicc::form() << std::endl ;
    //
  }
  //
  void EmuPeripheralCrate::EnableDisableDebug(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    if ( thisCrate->vmeController()->GetDebug() == 0 ) {
      std::cout << "debug 1 " << std::endl;
      thisCrate->vmeController()->Debug(1);
    } else {
      std::cout << "debug 0 " << std::endl;
      thisCrate->vmeController()->Debug(0);
    }
    //
    this->ControllerUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::CCBUtils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    char Name[50] ;
    sprintf(Name,"CCB Utils slot=%d",thisCCB->slot());
    //
    MyHeader(in,out,Name);
    //
    *out << cgicc::h1(Name);
    *out << cgicc::br();
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend("CCB Utils").set("style","color:blue") << cgicc::p() << std::endl ;
    //
    std::string ReadCCBRegister =
      toolbox::toString("/%s/ReadCCBRegister",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",ReadCCBRegister) << std::endl ;
    *out << "Read Register (hex)..." << std:: endl;
    *out << cgicc::input().set("type","text").set("value","0")
      .set("name","CCBRegister") << std::endl ;
    *out << "Register value : (hex) " << std::hex << CCBRegisterValue_ << std::endl;
    *out << cgicc::form() << std::endl ;
    //
    std::string HardReset =
      toolbox::toString("/%s/HardReset",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",HardReset) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","HardReset");
    *out << cgicc::form() << std::endl ;
    //
    CCBFirmware_ = FirmwareDir_+"ccb/"+"ccb2004p_030507.svf";
    //
    std::string CCBLoadFirmware =
       toolbox::toString("/%s/CCBLoadFirmware",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",CCBLoadFirmware) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Load CCB Firmware") << std::endl ;
//    sprintf(buf,"%d",ccb);
//    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << CCBFirmware_.toString();
    *out << cgicc::form() << std::endl ;

    *out << cgicc::fieldset();
    //
  }

  void EmuPeripheralCrate::CCBLoadFirmware(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    int debugMode(0);
    int jch(6);
    printf("Programming CCB using %s\n", CCBFirmware_.toString().c_str());
    int status = thisCCB->svfLoad(&jch,CCBFirmware_.toString().c_str(),debugMode);
    if (status >= 0){
      cout << "=== Programming finished"<< endl;
      cout << "=== " << status << " Verify Errors  occured" << endl;
    }
    else{
      cout << "=== Fatal Error. Exiting with " <<  status << endl;
    }
    //
    thisCCB->hardReset();
    //
    this->CCBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::MPCStatus(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    char Name[50] ;
    sprintf(Name,"MPC Status slot=%d",thisMPC->slot());
    //
    MyHeader(in,out,Name);
    //
    *out << cgicc::h1(Name);
    *out << cgicc::br();
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend("MPC Info").set("style","color:blue") << cgicc::p() << std::endl ;
    //
    thisMPC->RedirectOutput(out);
    thisMPC->firmwareVersion();
    thisMPC->RedirectOutput(&std::cout);
    //
    *out << cgicc::fieldset();
    //
  }

  void EmuPeripheralCrate::MPCUtils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    char Name[50] ;
    sprintf(Name,"MPC Utils slot=%d",thisMPC->slot());
    //
    MyHeader(in,out,Name);
    //
    *out << cgicc::h1(Name);
    *out << cgicc::br();
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend("MPC Utils").set("style","color:blue") << cgicc::p() << std::endl ;
    //
    MPCFirmware_ = FirmwareDir_+"mpc/"+"mpc2004_102706.svf";
    //
    std::string MPCLoadFirmware =
       toolbox::toString("/%s/MPCLoadFirmware",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",MPCLoadFirmware) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Load MPC Firmware") << std::endl ;
//    sprintf(buf,"%d",ccb);
//    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << MPCFirmware_.toString();
    *out << cgicc::form() << std::endl ;

    *out << cgicc::fieldset();
    //
  }

  void EmuPeripheralCrate::MPCLoadFirmware(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    int debugMode(0);
    int jch(6);
    printf("Programming MPC using %s\n", MPCFirmware_.toString().c_str());
    int status = thisMPC->svfLoad(&jch,MPCFirmware_.toString().c_str(),debugMode);
    if (status >= 0){
      cout << "=== Programming finished"<< endl;
      cout << "=== " << status << " Verify Errors  occured" << endl;
    }
    else{
      cout << "=== Fatal Error. Exiting with " <<  status << endl;
    }
    //
    thisCCB->hardReset();
    //
    this->MPCUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::TMBTests(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "Not tmb" << endl ;
      tmb = TMB_;
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    //
    char Name[50];
    sprintf(Name,"TMB Tests slot=%d",thisTMB->slot());	  
    //
    MyHeader(in,out,Name);
    //
    //*out << cgicc::h1(Name);
    //*out << cgicc::br();
    //
    alct = thisTMB->alctController();
    //
    char buf[20];
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << endl ;
    //
    std::string RunAllTests =
      toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",RunAllTests)
	 << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Run All TMB tests") 
      .set("style","color:blue") 
	 << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden")
      .set("value",buf).set("name","tmb");
    *out << cgicc::input().set("type","hidden")
      .set("value","0").set("name","tmbTestid");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::table().set("border","1");
    *out << cgicc::td().set("ALIGN","center");
    //
    std::string testBootRegister =
      toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",testBootRegister)
	 << std::endl ;
    if ( tmbTestVector[tmb].GetResultTestBootRegister() == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test Boot register") 
	.set("style","color:blue") 
	   << std::endl ;
    }
    else if ( tmbTestVector[tmb].GetResultTestBootRegister() > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test Boot register") 
	.set("style","color:green") 
	   << std::endl ;
    }
    else {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test Boot register") 
	.set("style","color:red") 
	   << std::endl ;
    }
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::input().set("type","hidden")
      .set("value","1").set("name","tmbTestid");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::td().set("ALIGN","center");
    //
    //std::string testVMEfpgaDataRegister =
    //toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
    //
    //*out << cgicc::form().set("method","GET").set("action",testVMEfpgaDataRegister)
    // << std::endl ;
    //
    //if ( tmbTestVector[tmb].GetResultTestVMEfpgaDataRegister() == -1 ) {
    //*out << cgicc::input().set("type","submit")
    //.set("value","TMB test VME fpga data register") 
    //.set("style","color:blue") 
    //   << std::endl ;
    //} else if ( tmbTestVector[tmb].GetResultTestVMEfpgaDataRegister() > 0 ) {
    //*out << cgicc::input().set("type","submit")
    //.set("value","TMB test VME fpga data register") 
    //.set("style","color:green") 
    //   << std::endl ;
    //} else {
    //*out << cgicc::input().set("type","submit")
    //.set("value","TMB test VME fpga data register") 
    //.set("style","color:red") 
    //   << std::endl ;
    //}
    //
    //sprintf(buf,"%d",tmb);
    //*out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    //*out << cgicc::input().set("type","hidden")
    //.set("value","2").set("name","tmbTestid");
    //*out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::td().set("ALIGN","center");
    //    
    std::string testFirmwareDate =
      toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",testFirmwareDate)
	 << std::endl ;
    //
    if ( tmbTestVector[tmb].GetResultTestFirmwareDate() == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test firmware date") 
	.set("style","color:blue") 
	   << std::endl ;
    } else if ( tmbTestVector[tmb].GetResultTestFirmwareDate() > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test firmware date") 
	.set("style","color:green") 
	   << std::endl ;
    } else {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test firmware date") 
	.set("style","color:red") 
	   << std::endl ;
    }
    //
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::input().set("type","hidden")
      .set("value","3").set("name","tmbTestid");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::tr().set("ALIGN","center");
    *out << cgicc::td();
    //
    std::string testFirmwareType =
      toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",testFirmwareType)
	 << std::endl ;
    //
    if ( tmbTestVector[tmb].GetResultTestFirmwareType() == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test firmware type") 
	.set("style","color:blue") 
	   << std::endl ;
    } else if ( tmbTestVector[tmb].GetResultTestFirmwareType() > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test firmware type") 
	.set("style","color:green") 
	   << std::endl ;
    } else {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test firmware type") 
	.set("style","color:red") 
	   << std::endl ;
    }
    //
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::input().set("type","hidden")
      .set("value","4").set("name","tmbTestid");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::td();
    //
    std::string testFirmwareVersion =
      toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",testFirmwareVersion)
	 << std::endl ;
    //
    if ( tmbTestVector[tmb].GetResultTestFirmwareVersion() == -1 ) {
    *out << cgicc::input().set("type","submit")
      .set("value","TMB test firmware version") 
      .set("style","color:blue") 
	 << std::endl ;
    } else if ( tmbTestVector[tmb].GetResultTestFirmwareVersion() > 0 ) {
    *out << cgicc::input().set("type","submit")
      .set("value","TMB test firmware version") 
      .set("style","color:green") 
	 << std::endl ;
    } else {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test firmware version") 
	.set("style","color:red") 
	   << std::endl ;
    }
    //
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::input().set("type","hidden")
      .set("value","5").set("name","tmbTestid");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::td();
    //
    std::string testFirmwareRevCode =
      toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",testFirmwareRevCode)
	 << std::endl ;
    //
    if ( tmbTestVector[tmb].GetResultTestFirmwareRevCode() == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test firmware RevCode Id") 
	.set("style","color:blue") 
	   << std::endl ;
    } else if ( tmbTestVector[tmb].GetResultTestFirmwareRevCode() > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test firmware RevCode Id") 
	.set("style","color:green") 
	   << std::endl ;
    } else {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test firmware RevCode Id") 
	.set("style","color:red") 
	   << std::endl ;
    }
    //
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::input().set("type","hidden")
      .set("value","6").set("name","tmbTestid");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    *out << cgicc::tr();
    //
    *out << cgicc::tr().set("ALIGN","center");
    *out << cgicc::td();
    //
    std::string testMezzId =
      toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",testMezzId)
	 << std::endl ;
    //
    if ( tmbTestVector[tmb].GetResultTestMezzId() == -1 ) {
    *out << cgicc::input().set("type","submit")
      .set("value","TMB test mezzanine Id") 
      .set("style","color:blue") 
	 << std::endl ;
    } else if ( tmbTestVector[tmb].GetResultTestMezzId() > 0 ) {
    *out << cgicc::input().set("type","submit")
      .set("value","TMB test mezzanine Id") 
      .set("style","color:green") 
	 << std::endl ;
    } else {
      *out << cgicc::input().set("type","submit")
      .set("value","TMB test mezzanine Id") 
      .set("style","color:red") 
	 << std::endl ;
    }
    //
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::input().set("type","hidden")
      .set("value","7").set("name","tmbTestid");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::td();
    //
    std::string testPromId =
      toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",testPromId)
	 << std::endl ;
    //
    if ( tmbTestVector[tmb].GetResultTestPromId() == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test PROM Id") 
	.set("style","color:blue") 
	   << std::endl ;
    } else if ( tmbTestVector[tmb].GetResultTestPromId() > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test PROM Id") 
	.set("style","color:green") 
	   << std::endl ;
    } else {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test PROM Id") 
	.set("style","color:red") 
	   << std::endl ;
    }
    //
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::input().set("type","hidden")
      .set("value","8").set("name","tmbTestid");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::td();
    //
    std::string testPROMPath =
      toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",testPROMPath)
	 << std::endl ;
    //
    if ( tmbTestVector[tmb].GetResultTestPROMPath() == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test PROM path") 
	.set("style","color:blue") 
	   << std::endl ;
    } else if ( tmbTestVector[tmb].GetResultTestPROMPath() > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test PROM path") 
	.set("style","color:green") 
	   << std::endl ;
    } else {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test PROM path") 
	.set("style","color:red") 
	   << std::endl ;
    }
    //
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::input().set("type","hidden")
      .set("value","9").set("name","tmbTestid");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    *out << cgicc::tr();
    //
    *out << cgicc::tr().set("ALIGN","center");
    *out << cgicc::td();
    //
    std::string testDSN =
      toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",testDSN)
	 << std::endl ;
    //
    if ( tmbTestVector[tmb].GetResultTestDSN() == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test DSN")
	.set("style","color:blue")  
	   << std::endl ;
    } else if ( tmbTestVector[tmb].GetResultTestDSN() > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test DSN") 
	.set("style","color:green") 
	   << std::endl ;
    } else {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test DSN") 
	.set("style","color:red") 
	   << std::endl ;
    }
    //
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::input().set("type","hidden")
      .set("value","10").set("name","tmbTestid");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::td();
    //
    std::string testADC =
      toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",testADC)
	 << std::endl ;
    //
    if ( tmbTestVector[tmb].GetResultTestADC() == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test ADC")
	.set("style","color:blue")  
	   << std::endl ;
    } else if ( tmbTestVector[tmb].GetResultTestADC() > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test ADC")
	.set("style","color:green")  
	   << std::endl ;
    } else {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test ADC")
	.set("style","color:red")  
	   << std::endl ;
    }
    //
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::input().set("type","hidden")
      .set("value","11").set("name","tmbTestid");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::td();
    //
    ///////////////////////////////////////////////////////////////////////
    //
    std::string test3d3444 =
      toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",test3d3444)
	 << std::endl ;
    //
    if ( tmbTestVector[tmb].GetResultTest3d3444() == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test 3d3444") 
	.set("style","color:blue")  
	   << std::endl ;
    } else if ( tmbTestVector[tmb].GetResultTest3d3444() > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test 3d3444") 
	.set("style","color:green")  
	   << std::endl ;
    } else {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test 3d3444") 
	.set("style","color:red")  
	   << std::endl ;
    }
    //
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::input().set("type","hidden")
      .set("value","12").set("name","tmbTestid");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    *out << cgicc::tr();
    //
    *out << cgicc::tr().set("ALIGN","center");
    *out << cgicc::td();
    //
    ///////////////////////////////////////////////////////////
    //
    std::string testRATtemper =
      toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",testRATtemper)
	 << std::endl ;
    //
    if ( tmbTestVector[tmb].GetResultTestRATtemper() == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test RAT temperature") 
	.set("style","color:blue")  
	   << std::endl ;
    } else if ( tmbTestVector[tmb].GetResultTestRATtemper() > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test RAT temperature") 
	.set("style","color:green")  
	   << std::endl ;
    } else {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test RAT temperature") 
	.set("style","color:red")  
	   << std::endl ;
    }
    //
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::input().set("type","hidden")
      .set("value","13").set("name","tmbTestid");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::td();
    //
    ////////////////////////////////////////////////////////////////
    //
    std::string testRATidCodes =
      toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",testRATidCodes)
	 << std::endl ;
    //
    if ( tmbTestVector[tmb].GetResultTestRATidCodes() == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test RAT Id Codes") 
	.set("style","color:blue")  
	   << std::endl ;
    } else if ( tmbTestVector[tmb].GetResultTestRATidCodes() > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test RAT Id Codes") 
	.set("style","color:green")  
	   << std::endl ;
    } else {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test RAT Id Codes") 
	.set("style","color:red")  
	   << std::endl ;
    }
    //
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::input().set("type","hidden")
      .set("value","14").set("name","tmbTestid");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::td();
    //
    ////////////////////////////////////////////////////////////////////
    //
    std::string testRATuserCodes =
      toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",testRATtemper)
	 << std::endl ;
    //
    if ( tmbTestVector[tmb].GetResultTestRATuserCodes() == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test RAT User Codes") 
	.set("style","color:blue")  
	   << std::endl ;
    } else if ( tmbTestVector[tmb].GetResultTestRATuserCodes() > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test RAT User Codes") 
	.set("style","color:green")  
	   << std::endl ;
    } else {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test RAT User Codes") 
	.set("style","color:red")  
	   << std::endl ;
    }
    //
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::input().set("type","hidden")
      .set("value","15").set("name","tmbTestid");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    *out << cgicc::tr();
    //
    *out << cgicc::tr().set("ALIGN","center");
    *out << cgicc::td().set("VALIGN","bottom");
    //
    /////////////////////////////////////////////////////////////////////
    //
    std::string testU760K =
      toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",testU760K)
	 << std::endl ;
    //
    if ( tmbTestVector[tmb].GetResultTestU76chip() == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test U760K") 
	.set("style","color:blue")  
	   << std::endl ;
    } else if ( tmbTestVector[tmb].GetResultTestU76chip() > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test U760K") 
	.set("style","color:green")  
	   << std::endl ;
    } else {
      *out << cgicc::input().set("type","submit")
	.set("value","TMB test U760K") 
	.set("style","color:red")  
	   << std::endl ;
    }
    //
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::input().set("type","hidden")
      .set("value","16").set("name","tmbTestid");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    *out << cgicc::table();
    //
    /////////////////////////////////////////////////////////////////////
    //
    *out << cgicc::textarea().set("name","TMBTestOutput")
      .set("WRAP","OFF")
      .set("rows","20").set("cols","100");
    *out << OutputTMBTests[tmb].str() << endl ;
    *out << cgicc::textarea();
    //    
    std::string LogTMBTestsOutput =
      toolbox::toString("/%s/LogTMBTestsOutput",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",LogTMBTestsOutput) << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::input().set("type","submit")
      .set("value","Log output").set("name","LogTMBTestsOutput") << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Clear")
      .set("name","ClearTMBTestsOutput") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::fieldset();
    //
  }
  //
  void EmuPeripheralCrate::testTMB(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    //
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    }
    //
    name = cgi.getElement("tmbTestid");
    //
    int tmbTestid;
    if(name != cgi.getElements().end()) {
      tmbTestid = cgi["tmbTestid"]->getIntegerValue();
      cout << "tmbTestid " << tmbTestid << endl;
    }
    //
    tmbTestVector[tmb].RedirectOutput(&OutputTMBTests[tmb]);
    if ( tmbTestid == 1 || tmbTestid == 0 ) {
      tmbTestVector[tmb].testBootRegister();
      ::sleep(1);
      printf("Test1");
    }
    if ( tmbTestid == 2 || tmbTestid == 0 ) {
      tmbTestVector[tmb].testVMEfpgaDataRegister();
      printf("Test2");
    }
    if ( tmbTestid == 3 || tmbTestid == 0 ) {
      tmbTestVector[tmb].testFirmwareDate();
      printf("Test3");
    }
    if ( tmbTestid == 4 || tmbTestid == 0 ) {
      tmbTestVector[tmb].testFirmwareType();
      printf("Test4");
    }
    if ( tmbTestid == 5 || tmbTestid == 0 ) {
      tmbTestVector[tmb].testFirmwareVersion();
      printf("Test5");
    }
    if ( tmbTestid == 6 || tmbTestid == 0 ) {
      tmbTestVector[tmb].testFirmwareRevCode();
      printf("Test6");
    }
    if ( tmbTestid == 7 || tmbTestid == 0 ) {
      tmbTestVector[tmb].testMezzId();
      printf("Test7");
    }
    if ( tmbTestid == 8 || tmbTestid == 0 ) {
      tmbTestVector[tmb].testPROMid();
      printf("Test8");
    }
    if ( tmbTestid == 9 || tmbTestid == 0 ) {
      tmbTestVector[tmb].testPROMpath();
      printf("Test9");
    }
    if ( tmbTestid == 10 || tmbTestid == 0 ) {
      tmbTestVector[tmb].testDSN();
      printf("Test10");
    }
    if ( tmbTestid == 11 || tmbTestid == 0 ) {
      tmbTestVector[tmb].testADC();
      printf("Test11");
    }
    if ( tmbTestid == 12 || tmbTestid == 0 ) {
      tmbTestVector[tmb].test3d3444();
      printf("Test12");
    }
    if ( tmbTestid == 13 || tmbTestid == 0 ) {
      tmbTestVector[tmb].testRATtemper();
      printf("Test13");
    }
    if ( tmbTestid == 14 || tmbTestid == 0 ) {
      tmbTestVector[tmb].testRATidCodes();
      printf("Test14");
    }
    if ( tmbTestid == 15 || tmbTestid == 0 ) {
      tmbTestVector[tmb].testRATuserCodes();
      printf("Test15");
    }
    if ( tmbTestid == 16 || tmbTestid == 0 ) {
      tmbTestVector[tmb].testU76chip();
      printf("Test16");
    }
    //
    tmbTestVector[tmb].RedirectOutput(&std::cout);
    //
    //std::cout << "Done" << std::endl ;
    //
    this->TMBTests(in,out);
    //
  }
  //
  void EmuPeripheralCrate::TMBStatus(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "Not tmb" << endl ;
      tmb = TMB_;
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    //
    char Name[50];
    sprintf(Name,"TMB Status slot=%d",thisTMB->slot());	
    //
    alct = thisTMB->alctController();
    rat  = thisTMB->getRAT();
    //
    if (alct) {
      std::string ALCTStatus =
	toolbox::toString("/%s/ALCTStatus?tmb=%d",getApplicationDescriptor()->getURN().c_str(),tmb);
      //
      *out << cgicc::a("ALCT Status").set("href",ALCTStatus) << endl;
      //
    }
    //
    if (rat) {
      std::string RATStatus =
	toolbox::toString("/%s/RATStatus?tmb=%d",getApplicationDescriptor()->getURN().c_str(),tmb);
      //
      *out << cgicc::a("RAT Status").set("href",RATStatus) << endl;
      //
    }
    //
    MyHeader(in,out,Name);
    //
    //*out << cgicc::h1(Name);
    //*out << cgicc::br();
    //
    char buf[200] ;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend("TMB Info").set("style","color:blue") << cgicc::p() << std::endl ;
    //
    *out << cgicc::pre();
    //
    // read the registers:
    thisTMB->FirmwareDate();
    thisTMB->FirmwareYear();
    //
    // get the read values:
    int day   = thisTMB->GetReadTmbFirmwareDay();
    int month = thisTMB->GetReadTmbFirmwareMonth();
    int year  = thisTMB->GetReadTmbFirmwareYear();
    //
    // get the expected values:
    int expected_day   = thisTMB->GetExpectedTmbFirmwareDay();
    int expected_month = thisTMB->GetExpectedTmbFirmwareMonth();
    int expected_year  = thisTMB->GetExpectedTmbFirmwareYear();
    //
    //
    sprintf(buf,"TMB Firmware date         : %02x/%02x/%04x ",month,day,year);
    //
    if ( month == expected_month &&
	 day   == expected_day   &&
	 year  == expected_year ) {
      *out << cgicc::span().set("style","color:green");
      *out << buf;
      *out << cgicc::span();
    } else {
      *out << cgicc::span().set("style","color:red");
      *out << buf;
      *out << cgicc::span();
    }
    //
    //
    *out << cgicc::br();
    //
    // read the registers:
    thisTMB->FirmwareVersion();
    //
    // get the read values:
    int type = thisTMB->GetReadTmbFirmwareType();
    //
    // get the expected values:
    int expected_type = thisTMB->GetExpectedTmbFirmwareType();

    sprintf(buf,"Firmware Type             : %01x ",type);       
    //
    if ( type == expected_type ) {
      *out << cgicc::span().set("style","color:green");
      *out << buf;
      *out << cgicc::span();
    } else {
      *out << cgicc::span().set("style","color:red");
      *out << buf;
      *out << cgicc::span();
    }
    //
    //
    *out << cgicc::br();
    //
    // read the registers:
    thisTMB->FirmwareVersion();
    //
    // get the read values:
    int version = thisTMB->GetReadTmbFirmwareVersion();
    //
    int expected_version = thisTMB->GetExpectedTmbFirmwareVersion();
    //
    sprintf(buf,"Firmware Version Code     : %01x ",version);       
    //
    if ( version == expected_version ){
      *out << cgicc::span().set("style","color:green");
      *out << buf ;
      *out << cgicc::span();
    } else {
      *out << cgicc::span().set("style","color:red");
      *out << buf ;
      *out << cgicc::span();
    }
    //
    *out << cgicc::br();
    //
    sprintf(buf,"Geographic Address        : %02d ",((thisTMB->FirmwareVersion()>>8)&0x1f));       
    if ( ((thisTMB->FirmwareVersion()>>8)&0x1f) == thisTMB->slot() ){
      *out << cgicc::span().set("style","color:green");
      *out << buf ;
      *out << cgicc::span();
    } else {
      *out << cgicc::span().set("style","color:red");
      *out << buf ;
      *out << cgicc::span();
      //
    }
    //
    *out << cgicc::br();
    //
    sprintf(buf,"Firmware Revision Code    : %04x ",((thisTMB->FirmwareRevCode())&0xffff));       
    *out << buf ;
    *out << cgicc::br();
    //
    sprintf(buf,"Power Comparator          : %02x ",((thisTMB->PowerComparator())&0x1f));       
    if ( (thisTMB->PowerComparator())&0x1f == 0x1f ) {
      *out << cgicc::span().set("style","color:green");
      *out << buf ;
      *out << cgicc::span();
    } else {
      *out << cgicc::span().set("style","color:red");
      *out << buf ;
      *out << cgicc::span();
    }
    //
    *out << cgicc::br();
    //
    *out << cgicc::pre();
    //
    *out << cgicc::fieldset();
    //
    *out << cgicc::fieldset();
    *out << cgicc::legend("CLCT Info").set("style","color:blue") << cgicc::p() << std::endl ;
    *out << cgicc::pre();
    thisTMB->RedirectOutput(out);
    thisTMB->DecodeCLCT();
    thisTMB->RedirectOutput(&std::cout);
    *out << cgicc::pre();
    *out << cgicc::fieldset();
    //
    if (alct) {
      *out << cgicc::fieldset();
      *out << cgicc::legend("ALCT Info").set("style","color:blue") << cgicc::p() << std::endl ;
      *out << cgicc::pre();
      thisTMB->RedirectOutput(out);
      thisTMB->DecodeALCT();
      thisTMB->RedirectOutput(&std::cout);
      *out << cgicc::pre();
      *out << cgicc::fieldset();
    }
    //
    *out << std::endl;    
    //
  }
  //
  void EmuPeripheralCrate::DMBBoardID(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception){
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "Not dmb" << endl ;
      dmb = DMB_;
    }
    //
    char buf[20];
    sprintf(buf,"DMBBoardID_%d",dmb);
    DMBBoardID_[dmb] = cgi[buf]->getValue();
    //
    this->CrateConfiguration(in,out);
    //
    }
  //
  void EmuPeripheralCrate::TMBBoardID(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception){
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "Not tmb" << endl ;
      tmb = TMB_;
    }
    //
    char buf[20];
    sprintf(buf,"TMBBoardID_%d",tmb);
    TMBBoardID_[tmb] = cgi[buf]->getValue();
    //
    this->CrateConfiguration(in,out);
    //
    }
  //
  void EmuPeripheralCrate::RATBoardID(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception){
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("rat");
    int rat;
    if(name != cgi.getElements().end()) {
      rat = cgi["rat"]->getIntegerValue();
      cout << "RAT " << rat << endl;
      RAT_ = rat;
    } else {
      cout << "Not rat" << endl ;
      rat = RAT_;
    }
    //
    char buf[20];
    sprintf(buf,"RATBoardID_%d",rat);
    RATBoardID_[rat] = cgi[buf]->getValue();
    //
    this->CrateConfiguration(in,out);
    //
    }
  //
  void EmuPeripheralCrate::DMBStatus(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "Not dmb" << endl ;
      dmb = DMB_;
    }
    //
    DAQMB * thisDMB = dmbVector[dmb];
    //
    char Name[50];
    sprintf(Name,"DMB Status slot=%d",thisDMB->slot());	
    //
    MyHeader(in,out,Name);
    //
    //*out << cgicc::h1(Name);
    //*out << cgicc::br();
    //
    char buf[200] ;
    //
    if( thisDMB->cfebs().size() > 0 ) {
      std::string CFEBStatus =
	toolbox::toString("/%s/CFEBStatus?dmb=%d",getApplicationDescriptor()->getURN().c_str(),dmb);
      *out << cgicc::a("CFEB Status").set("href",CFEBStatus) << endl;
    }
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend("DMB IDs").set("style","color:blue") << cgicc::p() << std::endl ;
    //
    *out << cgicc::pre();
    //
    thisDMB->vmefpgaid();
    sprintf(buf,"DMB vme FPGA : Version %d Revision %x Day %d Month %d Year %d",
	    (int)thisDMB->GetFirmwareVersion(),(int)thisDMB->GetFirmwareRevision(),
	    (int)thisDMB->GetFirmwareDay(),(int)thisDMB->GetFirmwareMonth(),(int)thisDMB->GetFirmwareYear());
    *out << buf ;
    *out << cgicc::br();
    //
    sprintf(buf,"DMB prom VME->Motherboard          : %08x ",(int)thisDMB->mbpromuser(0));
    *out << buf ;
    *out << cgicc::br();
    //
    sprintf(buf,"DMB prom Motherboard Controller    : %08x ",(int)thisDMB->mbpromuser(1));
    *out << buf  ;
    *out << cgicc::br();
    //
    sprintf(buf,"DMB fpga id                        : %08x ",(int)thisDMB->mbfpgaid());
    *out << buf  ;
    *out << cgicc::br();
      //
    sprintf(buf,"DMB prom VME->Motherboard ID       : %08x ",(int)thisDMB->mbpromid(0));
    *out << buf  ;
    *out << cgicc::br();
    //
    sprintf(buf,"DMB prom Motherboard Controller ID : %08x ",(int) thisDMB->mbpromid(1));
    *out << buf  ;
    *out << cgicc::br();
      //
    sprintf(buf,"DMB fpga user id                   : %x ", (int) thisDMB->mbfpgauser());

    if ( thisDMB->mbfpgauser() == EXPECTED_DMB_USERID ) {
	*out << cgicc::span().set("style","color:green");
	*out << buf;
	*out << cgicc::span();
    } else {
      *out << cgicc::span().set("style","color:red");
      *out << buf;
      *out << cgicc::span();
    }
    
    //
    *out << cgicc::pre();
    //
    *out << cgicc::br();
    //
    //thisDMB->lowv_dump();
    //thisDMB->daqmb_adc_dump();
    //thisDMB->daqmb_promfpga_dump();
    //
    *out << cgicc::fieldset();
    *out << endl ;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << endl ;
    //
    *out << cgicc::legend("Voltages, Temperatures, & Currents").set("style","color:blue") 
	 << cgicc::p() << std::endl ;
    //
    *out << cgicc::table().set("border","1");
    //
    float value;
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB1 3.3V = %3.2f ",(value=thisDMB->lowv_adc(3,3))/1000.);
    if ( value/1000. < 3.3*(0.95) ||
	 value/1000. > 3.3*(1.05) ) {	 
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB1 5.0V = %3.2f ",(value=thisDMB->lowv_adc(3,4))/1000.);
    if ( value/1000. < 5.0*0.95 ||
	 value/1000. > 5.0*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB1 6.0V = %3.2f ",(value=thisDMB->lowv_adc(3,5))/1000.);
    if ( value/1000. < 6.0*0.95 ||
	 value/1000. > 6.0*1.05  ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::table().set("border","1");
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB2 3.3V = %3.2f ",(value=thisDMB->lowv_adc(3,6))/1000.);
    if ( value/1000. < 3.3*0.95 ||
	 value/1000. > 3.3*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB2 5.0V = %3.2f ",(value=thisDMB->lowv_adc(3,7))/1000.);
    if ( value/1000. < 5.0*0.95 ||
	 value/1000. > 5.0*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB2 6.0V = %3.2f ",(value=thisDMB->lowv_adc(4,0))/1000.);
    if ( value/1000. < 6.0*0.95 ||
	 value/1000. > 6.0*1.95 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::table().set("border","1");
    *out << cgicc::td();
    sprintf(buf,"CFEB3 3.3V = %3.2f ",(value=thisDMB->lowv_adc(4,1))/1000.);
    if ( value/1000. < 3.3*0.95 ||
	 value/1000. > 3.3*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
      //
    *out << cgicc::td();
    sprintf(buf,"CFEB3 5.0V = %3.2f ",(value=thisDMB->lowv_adc(4,2))/1000.);
    if (  value/1000. < 5.0*0.95 ||
	  value/1000. > 5.0*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB3 6.0V = %3.2f ",(value=thisDMB->lowv_adc(4,3))/1000.);
    if ( value/1000. < 6.0*0.95 ||
	 value/1000. > 6.0*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::table().set("border","1");
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB4 3.3V = %3.2f ",(value=thisDMB->lowv_adc(4,4))/1000.);
    if ( value/1000. < 3.3*0.95 ||
	 value/1000. > 3.3*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB4 5.0V = %3.2f ",(value=thisDMB->lowv_adc(4,5))/1000.);
    if ( value/1000. < 5.0*0.95 ||
	 value/1000. > 5.0*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB4 6.0V = %3.2f ",(value=thisDMB->lowv_adc(4,6))/1000.);
    if ( value/1000. < 6.0*0.95 ||
	 value/1000. > 6.0*1.05 ){
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::table().set("border","1");
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB5 3.3V = %3.2f ",(value=thisDMB->lowv_adc(4,7))/1000.);
    if ( value/1000. < 3.3*0.95 ||
	 value/1000. > 3.3*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB5 5.0V = %3.2f ",(value=thisDMB->lowv_adc(5,0))/1000.);
    if ( value/1000. < 5.0*0.95 ||
	 value/1000. > 5.0*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB5 6.0V = %3.2f ",(value=thisDMB->lowv_adc(5,1))/1000.);
    if ( value/1000. < 6.0*0.95 ||
	 value/1000. > 6.0*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::br();
    //
    *out << cgicc::table().set("border","1");
    //
    *out << cgicc::td();
    sprintf(buf,"ALCT  3.3V = %3.2f ",(value=thisDMB->lowv_adc(5,2))/1000.);
    if ( value/1000. < 3.3*0.95 ||
	 value/1000. > 3.3*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"ALCT  1.8V = %3.2f ",(value=thisDMB->lowv_adc(5,3))/1000.);
    if ( value/1000. < 1.8*0.95 ||
	 value/1000. > 1.8*1.95 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"ALCT  5.5V B = %3.2f ",(value=thisDMB->lowv_adc(5,4))/1000.);
    if ( value/1000. < 5.5*0.95 ||
	 value/1000. > 5.5*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"ALCT  5.5V A = %3.2f ",(value=thisDMB->lowv_adc(5,5))/1000.);
    if ( value/1000. < 5.5*0.95 ||
	 value/1000. > 5.5*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::br();
    //
    *out << cgicc::table().set("border","1");;
    //
    *out << cgicc::td();
    sprintf(buf,"DMB temperature = %3.1f ",(value=thisDMB->readthermx(0)));
    if ( value > 50 && value < 95 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"FEB1 temperature = %3.1f ",(value=thisDMB->readthermx(1)));
    if ( value > 50 && value < 95 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
      //
    *out << cgicc::td();
    sprintf(buf,"FEB2 temperature = %3.1f ",(value=thisDMB->readthermx(2)));
    if ( value > 50 && value < 95 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::tr();
    //
    *out << cgicc::td();
    sprintf(buf,"FEB3 temperature = %3.1f ",(value=thisDMB->readthermx(3)));
    if ( value > 50 && value < 95 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"FEB4 temperature = %3.1f ",(value=thisDMB->readthermx(4)));
    if ( value > 50 && value < 95 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"FEB5 temperature = %3.1f ",(value=thisDMB->readthermx(5)));
    if ( value > 50 && value < 95 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::tr();
    //
    *out << cgicc::table();
    //
    *out << cgicc::br();
    //
    *out << cgicc::table().set("border","1");;
    //
    *out << cgicc::td();
    float readout = thisDMB->adcplus(2,0) ;
    sprintf(buf,"DMB DAC1 = %3.1f ",readout);
    //
    if ( readout > 3400 && readout < 3600 ) {
      *out << cgicc::span().set("style","color:black");
    } else {
      *out << cgicc::span().set("style","color:black");
    }
    //
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    readout = thisDMB->adcplus(2,1) ;
    sprintf(buf,"DMB DAC2 = %3.1f ",readout);
    if ( readout > 3400 && readout < 3600 ) {
      *out << cgicc::span().set("style","color:black");
    } else {
      *out << cgicc::span().set("style","color:black");
    }
    //
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    readout = thisDMB->adcplus(2,2) ;
    sprintf(buf,"DMB DAC3 = %3.1f ",readout);
    if ( readout > 3400 && readout < 3600 ) {
      *out << cgicc::span().set("style","color:black");
    } else {
      *out << cgicc::span().set("style","color:black");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    readout = thisDMB->adcplus(2,3) ;
    sprintf(buf,"DMB DAC4 = %3.1f ",readout);
    if ( readout > 3400 && readout < 3600 ) {
      *out << cgicc::span().set("style","color:black");
    } else {
      *out << cgicc::span().set("style","color:black");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    readout = thisDMB->adcplus(2,4) ;
    sprintf(buf,"DMB DAC5 = %3.1f ",readout);
    if ( readout > 3400 && readout < 3600 ) {
      *out << cgicc::span().set("style","color:black");
    } else {
      *out << cgicc::span().set("style","color:black");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::table().set("border","1");;
    //
    *out << cgicc::td();
    readout = thisDMB->adcplus(1,6) ;
    sprintf(buf,"1.8V Chip1 = %3.1f ",readout);
    if ( readout > 1700 && readout < 1900 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    readout = thisDMB->adcplus(2,6) ;
    sprintf(buf,"1.8V Chip2 = %3.1f ",readout);
    if ( readout > 1700 && readout < 1900 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    readout = thisDMB->adcplus(3,6) ;
    sprintf(buf,"1.8V Chip3 = %3.1f ",readout);
    if ( readout > 1700 && readout < 1900 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::br();
    //
    // DMB currents:
    //
    *out << cgicc::table().set("border","1");
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB1 3.3 V, I = %3.2f ",(value=thisDMB->lowv_adc(1,0))/1000.);
    if ( value/1000. < 3.3*(0.95) ||
	 value/1000. > 3.3*(1.05) ) {	 
      *out << cgicc::span().set("style","color:black");
    } else {
      *out << cgicc::span().set("style","color:black");
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB1 5.0 V, I = %3.2f ",(value=thisDMB->lowv_adc(1,1))/1000.);
    if ( value/1000. < 5.0*0.95 ||
	 value/1000. > 5.0*1.05 ) {
      *out << cgicc::span().set("style","color:black");
    } else {
      *out << cgicc::span().set("style","color:black");
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB1 6.0 V, I = %3.2f ",(value=thisDMB->lowv_adc(1,2))/1000.);
    if ( value/1000. < 6.0*0.95 ||
	 value/1000. > 6.0*1.05  ) {
      *out << cgicc::span().set("style","color:black");
    } else {
      *out << cgicc::span().set("style","color:black");
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::table().set("border","1");
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB2 3.3 V, I = %3.2f ",(value=thisDMB->lowv_adc(1,3))/1000.);
    if ( value/1000. < 3.3*0.95 ||
	 value/1000. > 3.3*1.05 ) {
      *out << cgicc::span().set("style","color:black");
    } else {
      *out << cgicc::span().set("style","color:black");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB2 5.0 V, I = %3.2f ",(value=thisDMB->lowv_adc(1,4))/1000.);
    if ( value/1000. < 5.0*0.95 ||
	 value/1000. > 5.0*1.05 ) {
      *out << cgicc::span().set("style","color:black");
    } else {
      *out << cgicc::span().set("style","color:black");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB2 6.0 V, I = %3.2f ",(value=thisDMB->lowv_adc(1,5))/1000.);
    if ( value/1000. < 6.0*0.95 ||
	 value/1000. > 6.0*1.95 ) {
      *out << cgicc::span().set("style","color:black");
    } else {
      *out << cgicc::span().set("style","color:black");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::table().set("border","1");
    *out << cgicc::td();
    sprintf(buf,"CFEB3 3.3 V, I = %3.2f ",(value=thisDMB->lowv_adc(1,6))/1000.);
    if ( value/1000. < 3.3*0.95 ||
	 value/1000. > 3.3*1.05 ) {
      *out << cgicc::span().set("style","color:black");
    } else {
      *out << cgicc::span().set("style","color:black");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
      //
    *out << cgicc::td();
    sprintf(buf,"CFEB3 5.0 V, I = %3.2f ",(value=thisDMB->lowv_adc(1,7))/1000.);
    if (  value/1000. < 5.0*0.95 ||
	  value/1000. > 5.0*1.05 ) {
      *out << cgicc::span().set("style","color:black");
    } else {
      *out << cgicc::span().set("style","color:black");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB3 6.0 V, I = %3.2f ",(value=thisDMB->lowv_adc(2,0))/1000.);
    if ( value/1000. < 6.0*0.95 ||
	 value/1000. > 6.0*1.05 ) {
      *out << cgicc::span().set("style","color:black");
    } else {
      *out << cgicc::span().set("style","color:black");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::table().set("border","1");
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB4 3.3 V, I = %3.2f ",(value=thisDMB->lowv_adc(2,1))/1000.);
    if ( value/1000. < 3.3*0.95 ||
	 value/1000. > 3.3*1.05 ) {
      *out << cgicc::span().set("style","color:black");
    } else {
      *out << cgicc::span().set("style","color:black");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB4 5.0 V, I = %3.2f ",(value=thisDMB->lowv_adc(2,2))/1000.);
    if ( value/1000. < 5.0*0.95 ||
	 value/1000. > 5.0*1.05 ) {
      *out << cgicc::span().set("style","color:black");
    } else {
      *out << cgicc::span().set("style","color:black");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB4 6.0 V, I = %3.2f ",(value=thisDMB->lowv_adc(2,3))/1000.);
    if ( value/1000. < 6.0*0.95 ||
	 value/1000. > 6.0*1.05 ){
      *out << cgicc::span().set("style","color:black");
    } else {
      *out << cgicc::span().set("style","color:black");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::table().set("border","1");
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB5 3.3 V, I = %3.2f ",(value=thisDMB->lowv_adc(2,4))/1000.);
    if ( value/1000. < 3.3*0.95 ||
	 value/1000. > 3.3*1.05 ) {
      *out << cgicc::span().set("style","color:black");
    } else {
      *out << cgicc::span().set("style","color:black");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB5 5.0 V, I = %3.2f ",(value=thisDMB->lowv_adc(2,5))/1000.);
    if ( value/1000. < 5.0*0.95 ||
	 value/1000. > 5.0*1.05 ) {
      *out << cgicc::span().set("style","color:black");
    } else {
      *out << cgicc::span().set("style","color:black");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB5 6.0 V, I = %3.2f ",(value=thisDMB->lowv_adc(2,6))/1000.);
    if ( value/1000. < 6.0*0.95 ||
	 value/1000. > 6.0*1.05 ) {
      *out << cgicc::span().set("style","color:black");
    } else {
      *out << cgicc::span().set("style","color:black");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    // ALCT currents
    //
    *out << cgicc::br();
    //
    *out << cgicc::table().set("border","1");
    //
    *out << cgicc::td();
    sprintf(buf,"ALCT  3.3 V, I = %3.2f ",(value=thisDMB->lowv_adc(2,7))/1000.);
    if ( value/1000. < 3.3*0.95 ||
	 value/1000. > 3.3*1.05 ) {
      *out << cgicc::span().set("style","color:black");
    } else {
      *out << cgicc::span().set("style","color:black");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"ALCT  1.8 V, I = %3.2f ",(value=thisDMB->lowv_adc(3,0))/1000.);
    if ( value/1000. < 1.8*0.95 ||
	 value/1000. > 1.8*1.95 ) {
      *out << cgicc::span().set("style","color:black");
    } else {
      *out << cgicc::span().set("style","color:black");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"ALCT  5.5A V, I = %3.2f ",(value=thisDMB->lowv_adc(3,1))/1000.);
    if ( value/1000. < 5.5*0.95 ||
	 value/1000. > 5.5*1.05 ) {
      *out << cgicc::span().set("style","color:black");
    } else {
      *out << cgicc::span().set("style","color:black");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"ALCT  5.5B V, I = %3.2f ",(value=thisDMB->lowv_adc(3,2))/1000.);
    if ( value/1000. < 5.5*0.95 ||
	 value/1000. > 5.5*1.05 ) {
      *out << cgicc::span().set("style","color:black");
    } else {
      *out << cgicc::span().set("style","color:black");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::fieldset();
    *out << std::endl;
    //
    //thisDMB->dmb_readstatus();
    //
  }
  //
  void EmuPeripheralCrate::TMBUtils(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "Not tmb" << endl ;
      tmb = TMB_;
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    //
    char Name[50];
    sprintf(Name,"TMB Utils slot=%d",thisTMB->slot());
    //
    alct = thisTMB->alctController();
    rat = thisTMB->getRAT();
    //
    MyHeader(in,out,Name);
    //
    //*out << cgicc::h1(Name);
    //*out << cgicc::br();
    //
    char buf[200] ;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << endl ;
    //
    *out << cgicc::legend("TMB Utils").set("style","color:blue") ;
    //
    //create filename for firmware based on expected dates...
    char date[8];
    sprintf(date,"%4x%1x%1x%1x%1x",
	    thisTMB->GetExpectedTmbFirmwareYear()&0xffff,
	    (thisTMB->GetExpectedTmbFirmwareMonth()>>4)&0xf,
	    (thisTMB->GetExpectedTmbFirmwareMonth()   )&0xf,
	    (thisTMB->GetExpectedTmbFirmwareDay()  >>4)&0xf,
	    (thisTMB->GetExpectedTmbFirmwareDay()     )&0xf);
    std::string TMBFirmware = FirmwareDir_+"tmb/"+date+"/tmb.svf";
    TMBFirmware_ = TMBFirmware;
    //
    std::string LoadTMBFirmware =
      toolbox::toString("/%s/LoadTMBFirmware",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",LoadTMBFirmware) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Load TMB Firmware") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << TMBFirmware_.toString();
    *out << cgicc::form() << std::endl ;
    //
    if (alct) {
      //
      std::string LoadALCTFirmware =
	toolbox::toString("/%s/LoadALCTFirmware",getApplicationDescriptor()->getURN().c_str());
      //
      *out << cgicc::form().set("method","GET").set("action",LoadALCTFirmware) << std::endl ;
      *out << cgicc::input().set("type","submit").set("value","Load ALCT Firmware") << std::endl ;
      sprintf(buf,"%d",tmb);
      //      *out << ALCTFirmware_.toString() ;
      *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
      *out << cgicc::form() << std::endl ;
      //
    }
    //
    if (rat) {
      //
      std::string RATFirmware = FirmwareDir_+RAT_FIRMWARE_FILENAME;
      RATFirmware_ = RATFirmware;
      //
      std::string LoadRATFirmware =
	toolbox::toString("/%s/LoadRATFirmware",getApplicationDescriptor()->getURN().c_str());
      //
      *out << cgicc::form().set("method","GET").set("action",LoadRATFirmware) << std::endl ;
      *out << cgicc::input().set("type","submit").set("value","Load RAT Firmware") << std::endl ;
      sprintf(buf,"%d",tmb);
      *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
      *out << RATFirmware_.toString() ;
      *out << cgicc::form() << std::endl ;
      //
    }
    //
    std::string ReadTMBRegister =
      toolbox::toString("/%s/ReadTMBRegister",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",ReadTMBRegister) << std::endl ;
    *out << "Read Register (int)..." << std:: endl;
    *out << cgicc::input().set("type","text").set("value","0")
      .set("name","TMBRegister") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << "Register value : (hex) " << std::hex << TMBRegisterValue_ << std::endl;
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::fieldset();
    //
    *out << cgicc::table().set("border","0");
    //
    *out << cgicc::td();
    //
    std::string TMBPrintCounters =
      toolbox::toString("/%s/TMBPrintCounters",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",TMBPrintCounters) ;
    *out << cgicc::input().set("type","submit").set("value","TMB Print Counters") ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::td();
    //
    std::string TMBResetCounters =
      toolbox::toString("/%s/TMBResetCounters",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",TMBResetCounters) ;
    *out << cgicc::input().set("type","submit").set("value","TMB Reset Counters") ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    std::string armScope =
      toolbox::toString("/%s/armScope",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",armScope) ;
    *out << cgicc::input().set("type","submit").set("value","arm Scope") ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
    //
    std::string readoutScope =
      toolbox::toString("/%s/readoutScope",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",readoutScope) ;
    *out << cgicc::input().set("type","submit").set("value","readout Scope") ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
    //
    std::string forceScope =
      toolbox::toString("/%s/forceScope",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",forceScope) ;
    *out << cgicc::input().set("type","submit").set("value","force Scope") ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
    //
    std::string TriggerTestInjectALCT =
      toolbox::toString("/%s/TriggerTestInjectALCT",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",TriggerTestInjectALCT) ;
    *out << cgicc::input().set("type","submit").set("value","TriggerTest : InjectALCT") ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
    //
    std::string TriggerTestInjectCLCT =
      toolbox::toString("/%s/TriggerTestInjectCLCT",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",TriggerTestInjectCLCT) ;
    *out << cgicc::input().set("type","submit").set("value","TriggerTest : InjectCLCT") ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
    //
    //
    std::string TMBDumpAllRegisters =
      toolbox::toString("/%s/TMBDumpAllRegisters",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",TMBDumpAllRegisters) ;
    *out << cgicc::input().set("type","submit").set("value","Dump All TMB VME Registers") ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
    //
    //
    *out << cgicc::table().set("border","0");
    //
    *out << cgicc::td();
    //
    std::string TMBReadConfiguration =
      toolbox::toString("/%s/TMBReadConfiguration",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",TMBReadConfiguration) ;
    *out << cgicc::input().set("type","submit").set("value","Read TMB+ALCT Configuration") ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    *out << cgicc::td();
    //
    std::string TMBCheckConfiguration =
      toolbox::toString("/%s/TMBCheckConfiguration",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",TMBCheckConfiguration) ;
    if ( thisTMB->GetTMBConfigurationStatus() == 1 &&
	 alct->GetALCTConfigurationStatus()   == 1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","Check TMB+ALCT Configuration")
	.set("style","color:green");
    } else if ( thisTMB->GetTMBConfigurationStatus() == 0 ||
		alct->GetALCTConfigurationStatus()   == 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","Check TMB+ALCT Configuration")
	.set("style","color:red");
    } else {
      *out << cgicc::input().set("type","submit")
	.set("value","Check TMB+ALCT Configuration")
	.set("style","color:blue");
    }
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    *out << cgicc::table();
    //
    //
    *out << cgicc::table().set("border","0");
    *out << cgicc::td();
    //
    std::string TMBReadStateMachines =
      toolbox::toString("/%s/TMBReadStateMachines",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",TMBReadStateMachines) ;
    *out << cgicc::input().set("type","submit").set("value","Read TMB State Machines") ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    *out << cgicc::td();
    //
    std::string TMBCheckStateMachines =
      toolbox::toString("/%s/TMBCheckStateMachines",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",TMBCheckStateMachines) ;
    if ( thisTMB->GetVMEStateMachineStatus()  == 1 && 
	 thisTMB->GetJTAGStateMachineStatus() == 1 && 
	 thisTMB->GetDDDStateMachineStatus()  == 1 && 
	 thisTMB->GetRawHitsHeaderStatus()    == 1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","Check TMB State Machines")
	.set("style","color:green");
    } else if ( thisTMB->GetVMEStateMachineStatus()  == 0 || 
		thisTMB->GetJTAGStateMachineStatus() == 0 || 
		thisTMB->GetDDDStateMachineStatus()  == 0 || 
		thisTMB->GetRawHitsHeaderStatus()    == 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","Check TMB State Machines")
	.set("style","color:red");
    } else {
      *out << cgicc::input().set("type","submit")
	.set("value","Check TMB State Machines")
	.set("style","color:blue");
    }
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    // Output area
    //
    *out << cgicc::form().set("method","GET") << std::endl ;
    *out << cgicc::pre();
    *out << cgicc::textarea().set("name","CrateTestTMBOutput")
      .set("rows","50")
      .set("cols","150")
      .set("WRAP","OFF");
    *out << OutputStringTMBStatus[tmb].str() << endl ;
    *out << cgicc::textarea();
    OutputStringTMBStatus[tmb].str("");
    *out << cgicc::pre();
    *out << cgicc::form() << std::endl ;
    //
  }
  //
  void EmuPeripheralCrate::LoadTMBFirmware(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    //const CgiEnvironment& env = cgi.getEnvironment();
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "Not tmb" << endl ;
      tmb = TMB_;
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    //
    thisCCB->hardReset();
    //
    int debugMode(0);
    int jch(5);
    //string chamberType("ME21");
    //ALCTController *alct = new ALCTController(thisTMB,chamberType);
    //
    //thisTMB->disableAllClocks();
    printf("Programming...");
    int status = thisTMB->SVFLoad(&jch,TMBFirmware_.toString().c_str(),debugMode);
    //thisTMB->enableAllClocks();
    //
    if (status >= 0){
      cout << "=== Programming finished"<< endl;
      cout << "=== " << status << " Verify Errors  occured" << endl;
    }
    else{
      cout << "=== Fatal Error. Exiting with " <<  status << endl;
    }
    //
    thisCCB->hardReset();
    //
    this->TMBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::LoadALCTFirmware(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "Not tmb" << endl ;
      tmb = TMB_;
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    int mintmb = tmb;
    int maxtmb = tmb+1;
    if (thisTMB->slot() == 26) { //if TMB slot = 26, loop over each alct according to its type
      mintmb = 0;
      maxtmb = tmbVector.size()-1;
    }
    //
    thisCCB->hardReset();
    //
    std::cout << "Loading ALCT firmware from " << mintmb << " to " << maxtmb << std::endl;
    for (tmb=mintmb; tmb<maxtmb; tmb++) {
      thisTMB = tmbVector[tmb];
      //
      alct = thisTMB->alctController();
      if (!alct) {
	std::cout << "No ALCT present" << std::endl;
	return;
      }
      //create filename for firmware based on expected dates...
      char date[8];
      sprintf(date,"%4x%1x%1x%1x%1x",
	      (alct->GetExpectedFastControlYear()&0xffff),
	      (alct->GetExpectedFastControlMonth()>>4)&0xf,
	      (alct->GetExpectedFastControlMonth()   )&0xf,
	      (alct->GetExpectedFastControlDay()  >>4)&0xf,
	      (alct->GetExpectedFastControlDay()     )&0xf);
      //
      std::string ALCTFirmware = FirmwareDir_+"alct/"+date+"/";
      //
      if ( (alct->GetChamberType()).find("ME11") != string::npos ) {
	//
	if (alct->GetExpectedFastControlBackwardForwardType() == BACKWARD_FIRMWARE_TYPE &&
	    alct->GetExpectedFastControlNegativePositiveType() == NEGATIVE_FIRMWARE_TYPE ) {
	  ALCTFirmware += ALCT_FIRMWARE_FILENAME_ME11_BACKWARD_NEGATIVE;
	} else if (alct->GetExpectedFastControlBackwardForwardType() == BACKWARD_FIRMWARE_TYPE &&
		   alct->GetExpectedFastControlNegativePositiveType() == POSITIVE_FIRMWARE_TYPE ) {
	  ALCTFirmware += ALCT_FIRMWARE_FILENAME_ME11_BACKWARD_POSITIVE;
	} else if (alct->GetExpectedFastControlBackwardForwardType() == FORWARD_FIRMWARE_TYPE &&
		   alct->GetExpectedFastControlNegativePositiveType() == POSITIVE_FIRMWARE_TYPE ) {
	  ALCTFirmware += ALCT_FIRMWARE_FILENAME_ME11_FORWARD_POSITIVE;
	} else {
	  ALCTFirmware += ALCT_FIRMWARE_FILENAME_ME11;
	}
      } else if ( (alct->GetChamberType()).find("ME12") != string::npos ) {
	ALCTFirmware += ALCT_FIRMWARE_FILENAME_ME12;
      } else if ( (alct->GetChamberType()).find("ME13") != string::npos ) {
	ALCTFirmware += ALCT_FIRMWARE_FILENAME_ME13;
      } else if ( (alct->GetChamberType()).find("ME21") != string::npos ) {
	ALCTFirmware += ALCT_FIRMWARE_FILENAME_ME21;
      } else if ( (alct->GetChamberType()).find("ME22") != string::npos ) {
	ALCTFirmware += ALCT_FIRMWARE_FILENAME_ME22;
      } else if ( (alct->GetChamberType()).find("ME31") != string::npos ) {
	ALCTFirmware += ALCT_FIRMWARE_FILENAME_ME31;
      } else if ( (alct->GetChamberType()).find("ME32") != string::npos ) {
	ALCTFirmware += ALCT_FIRMWARE_FILENAME_ME32;
      } else if ( (alct->GetChamberType()).find("ME41") != string::npos ) {
	ALCTFirmware += ALCT_FIRMWARE_FILENAME_ME41;
      } 
      //
      ALCTFirmware_ = ALCTFirmware;
      std::cout <<  "Programming ALCT firmware - slot " << thisTMB->slot() 
		<< " with " << ALCTFirmware_.toString() 
		<< std::endl;
      //
      int debugMode(0);
      int jch(3);
      //
      printf("Reading IDs...") ;
      //
      alct->ReadSlowControlId();
      alct->PrintSlowControlId();
      //
      alct->ReadFastControlId();
      alct->PrintFastControlId();
      //
      thisTMB->disableAllClocks();
      LOG4CPLUS_INFO(getApplicationLogger(), "Programming ALCT");
      //
      int status = alct->SVFLoad(&jch,ALCTFirmware_.toString().c_str(),debugMode);
      thisTMB->enableAllClocks();
      //
      if (status >= 0){
	LOG4CPLUS_INFO(getApplicationLogger(), "Programming ALCT finished");
	//cout << "=== Programming finished"<< endl;
	//cout << "=== " << status << " Verify Errors  occured" << endl;
      }
      else{
	cout << "=== Fatal Error. Exiting with " <<  status << endl;
      }
    }
    //
    thisCCB->hardReset();
    //
    this->TMBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::LoadRATFirmware(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "Not tmb" << endl ;
      tmb = TMB_;
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    //
    std::cout << "Loading RAT firmware " << std::endl;
    //
    rat = thisTMB->getRAT();
    if (!rat) {
      std::cout << "No RAT present" << std::endl;
      return;
    }
    thisCCB->hardReset();
    //
    int debugMode(0);
    int jch(7);
    //
    thisTMB->disableAllClocks();
    printf("Programming...");
    int status = rat->SVFLoad(&jch,RATFirmware_.toString().c_str(),debugMode);
    thisTMB->enableAllClocks();
    //
    if (status >= 0){
      cout << "=== Programming finished"<< endl;
      cout << "=== " << status << " Verify Errors  occured" << endl;
    }
    else{
      cout << "=== Fatal Error. Exiting with " <<  status << endl;
    }
    //
    thisCCB->hardReset();
    //
    this->TMBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::ReadTMBRegister(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    //const CgiEnvironment& env = cgi.getEnvironment();
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "Not tmb" << endl ;
      tmb = TMB_;
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    //
    cgicc::form_iterator name2 = cgi.getElement("TMBRegister");
    int registerValue = -1;
    if(name2 != cgi.getElements().end()) {
      registerValue = cgi["TMBregister"]->getIntegerValue();
      cout << "Register " << registerValue << endl;
      //
      TMBRegisterValue_ = thisTMB->ReadRegister(registerValue);
      //
    }
    //
    this->TMBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::ReadCCBRegister(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name2 = cgi.getElement("CCBRegister");
    int registerValue = -1;
    if(name2 != cgi.getElements().end()) {
      registerValue = strtol(cgi["CCBregister"]->getValue().c_str(),NULL,16);
      cout << "Register " << registerValue << endl;
      //
      CCBRegisterValue_ = thisCCB->ReadRegister(registerValue);
      //
    }
    //
    this->CCBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::HardReset(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    std::cout << "hardReset" << std::endl;
    //
    //thisCCB->hardReset();
    thisCCB->HardReset_crate();
    //
    this->CCBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrate::DMBUtils(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "Not dmb" << endl ;
      dmb = DMB_;
    }
    //
    DAQMB * thisDMB = dmbVector[dmb];
    //
    char Name[50];
    sprintf(Name,"DMB Utils slot=%d",thisDMB->slot());
    //
    MyHeader(in,out,Name);
    //
    *out << cgicc::h1(Name);
    *out << cgicc::br();
    //
    char buf[200] ;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << endl ;
    //
    *out << cgicc::legend("DMB Utils").set("style","color:blue") ;
    //
    *out << cgicc::table().set("border","1");
    //
    *out << cgicc::td();
    //
    std::string DMBTurnOff =
      toolbox::toString("/%s/DMBTurnOff",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBTurnOff) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","DMB Turn Off LV") << std::endl ;
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::td();
    //
    std::string DMBTurnOn =
      toolbox::toString("/%s/DMBTurnOn",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBTurnOn) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","DMB Turn On LV") << std::endl ;
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    std::string DMBPrintCounters =
      toolbox::toString("/%s/DMBPrintCounters",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBPrintCounters)
	 << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","DMB Print Counters") << std::endl ;
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    std::string DMBFirmware = FirmwareDir_+DMB_FIRMWARE_FILENAME;
    DMBFirmware_ = DMBFirmware;
    std::string DMBVmeFirmware = FirmwareDir_+DMBVME_FIRMWARE_FILENAME;
    DMBVmeFirmware_ = DMBVmeFirmware;
    //
    std::string DMBLoadFirmware =
      toolbox::toString("/%s/DMBLoadFirmware",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBLoadFirmware)
	 << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","DMB CONTROL Load Firmware") << std::endl ;
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    std::string DMBVmeLoadFirmware =
      toolbox::toString("/%s/DMBVmeLoadFirmware",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBVmeLoadFirmware)
	 << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","DMB Vme Load Firmware") << std::endl ;
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    std::string DMBVmeLoadFirmwareEmergency =
      toolbox::toString("/%s/DMBVmeLoadFirmwareEmergency",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBVmeLoadFirmwareEmergency)
	 << std::endl ;
    *out << "DMB Board Number:";
    *out <<cgicc::input().set("type","text").set("value","0").set("name","DMBNumber")<<std::endl;
    *out << cgicc::input().set("type","submit").set("value","DMB Vme Load Firmware (Emergency)") << std::endl ;
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    std::string CFEBFirmware = FirmwareDir_+CFEB_FIRMWARE_FILENAME;
    CFEBFirmware_ = CFEBFirmware;
    //
    std::string CFEBLoadFirmware =
      toolbox::toString("/%s/CFEBLoadFirmware",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",CFEBLoadFirmware)
	 << std::endl ;
    *out << "CFEB to download (0-4), (-1 == all) : ";
    *out << cgicc::input().set("type","text").set("value","-1")
      .set("name","DMBNumber") << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","CFEB Load Firmware") << std::endl ;
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::fieldset();
    //
  }
  //
  void EmuPeripheralCrate::DMBTests(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "Not dmb" << endl ;
      dmb = DMB_;
    }
    //
    DAQMB * thisDMB = dmbVector[dmb];
    //
    char Name[50];
    sprintf(Name,"DMB Tests slot=%d",thisDMB->slot());
    //
    MyHeader(in,out,Name);
    //
    //*out << cgicc::h1(Name);
    //*out << cgicc::br();
    //
    char buf[200] ;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << endl ;
    //
    *out << cgicc::legend("DMB Tests").set("style","color:blue") ;
    //
    std::string DMBTestAll =
      toolbox::toString("/%s/DMBTestAll",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBTestAll)
	 << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","DMB Test All ") << std::endl ;
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    //
    *out << cgicc::table().set("border","0");
    //
    *out << cgicc::td();
    //
    std::string DMBTest3 =
      toolbox::toString("/%s/DMBTest3",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBTest3)
	 << std::endl ;
    //
    //std::cout << thisDMB->GetTestStatus(3) << std::endl ;
    //
    if( thisDMB->GetTestStatus(3) == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test3 (Check DMB Fifos)") 
	.set("style","color:blue") 
	   << std::endl ;
    }
    else if( thisDMB->GetTestStatus(3) == 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test3 (Check DMB Fifos)") 
	.set("style","color:green") 
	   << std::endl ;
    } else {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test3 (Check DMB Fifos)") 
	.set("style","color:red") 
	   << std::endl ;
    }
    //
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::td();
    //
    std::string DMBTest4 =
      toolbox::toString("/%s/DMBTest4",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBTest4)
	 << std::endl ;
    if( thisDMB->GetTestStatus(4) == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test4 (Check voltages)")
	.set("style","color:blue") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(4) > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test4 (Check voltages)")
	.set("style","color:red") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(4) ==0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test4 (Check voltages)")
	.set("style","color:green") 
	   << std::endl ;
    }
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::td();
    //
    std::string DMBTest5 =
      toolbox::toString("/%s/DMBTest5",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBTest5)
	 << std::endl ;
    if( thisDMB->GetTestStatus(5) == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test5 (Check Power Register)")
	.set("style","color:blue") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(5) > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test5 (Check Power Register)")
	.set("style","color:red") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(5) ==0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test5 (Check PowerRegister)")
	.set("style","color:green") 
	   << std::endl ;
    }
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::td();
    //
    std::string DMBTest6 =
      toolbox::toString("/%s/DMBTest6",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBTest6)
	 << std::endl ;
    if( thisDMB->GetTestStatus(6) == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test6 (Check FPGA IDs)")
	.set("style","color:blue") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(6) > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test6 (Check FPGA IDs)")
	.set("style","color:red") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(6) ==0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test6 (Check FPGA IDs)")
	.set("style","color:green") 
	   << std::endl ;
    }
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::tr();
    *out << cgicc::td();
    //
    std::string DMBTest8 =
      toolbox::toString("/%s/DMBTest8",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBTest8)
	 << std::endl ;
    if( thisDMB->GetTestStatus(8) == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test8 (Check Comparator DAC/ADC)")
	.set("style","color:blue") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(8) > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test8 (Check Comparator DAC/ADC)")
	.set("style","color:red") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(8) ==0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test8 (Check Comparator DAC/ADC)")
	.set("style","color:green") 
	   << std::endl ;
    }
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::td();
    //
    std::string DMBTest9 =
      toolbox::toString("/%s/DMBTest9",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBTest9)
	 << std::endl ;
    if( thisDMB->GetTestStatus(9) == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test9 (Check CDAC)")
	.set("style","color:blue") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(9) > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test9 (Check CDAC)")
	.set("style","color:red") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(9) ==0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test9 (Check CDAC)")
	.set("style","color:green") 
	   << std::endl ;
    }
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::td();
    //
    std::string DMBTest10 =
      toolbox::toString("/%s/DMBTest10",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBTest10)
	 << std::endl ;
    if( thisDMB->GetTestStatus(10) == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test10 (Check SFM)")
	.set("style","color:blue") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(10) > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test10 (Check SFM)")
	.set("style","color:red") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(10) ==0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test10 (Check SFM)")
	.set("style","color:green") 
	   << std::endl ;
    }
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::tr();
    //
    *out << cgicc::tr();
    *out << cgicc::td();
    /*
    std::string DMBTest11 =
      toolbox::toString("/%s/DMBTest11",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBTest11)
	 << std::endl ;
    if( thisDMB->GetTestStatus(11) == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test11 (Check buckflash)")
	.set("style","color:blue") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(11) > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test11 (Check buckflash)")
	.set("style","color:red") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(11) ==0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test11 (Check buckflash)")
	.set("style","color:green") 
	   << std::endl ;
    }
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    */
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::fieldset() << std::endl;
    //
    *out << cgicc::form().set("method","GET") << std::endl ;
    *out << cgicc::textarea().set("name","CrateTestDMBOutput")
      .set("WRAP","OFF")
      .set("rows","20").set("cols","60");
    *out << OutputDMBTests[dmb].str() << endl ;
    *out << cgicc::textarea();
    *out << cgicc::form();
    //
    std::string method =
      toolbox::toString("/%s/LogDMBTestsOutput",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",method) << std::endl ;
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::input().set("type","submit")
      .set("value","Log output").set("name","LogDMBTestsOutput") << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Clear")
      .set("name","ClearDMBTestsOutput") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    std::cout << "Done" << std::endl;
    //
  }
  //
  void EmuPeripheralCrate::LogDMBTestsOutput(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "LogDMBTestsOutput" << std::endl;
    //
    cgicc::Cgicc cgi(in);
    //
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "Not dmb" << endl ;
      dmb = DMB_;
    }
    //
    cgicc::form_iterator name2 = cgi.getElement("ClearDMBTestsOutput");
    //
    if(name2 != cgi.getElements().end()) {
      cout << "Clear..." << endl;
      cout << cgi["ClearDMBTestsOutput"]->getValue() << std::endl ;
      OutputDMBTests[dmb].str("");
      //
    this->DMBTests(in,out);
    return;
    }
    //
    DAQMB * thisDMB = dmbVector[dmb];
    //
    cout << DMBBoardID_[dmb] << endl ;
    //
    char buf[20];
    sprintf(buf,"DMBTestsLogFile_%d_%s.log",thisDMB->slot(),DMBBoardID_[dmb].c_str());
    //
    ofstream DMBTestsLogFile;
    DMBTestsLogFile.open(buf);
    DMBTestsLogFile << OutputDMBTests[dmb].str() ;
    DMBTestsLogFile.close();
    //
    OutputDMBTests[dmb].str("");
    //
    this->DMBTests(in,out);
    //
  }
  //
  void EmuPeripheralCrate::LogTestSummary(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception){
    //
    time_t rawtime;
    time(&rawtime);
    //
    std::string buf;
    std::string time_dump = ctime(&rawtime);
    std::string time = time_dump.substr(0,time_dump.length()-1);
    //
    while( time.find(" ",0) != string::npos ) {
      //
      int thispos = time.find(" ",0); 
      time.replace(thispos,1,"_");
      //
    }
    //
    buf = "EmuPeripheralCrateTestSummary_"+RunNumber_+"_"+Operator_+time+".log";
    //
    ofstream LogFile;
    LogFile.open(buf.c_str());
    //
    LogFile << " *** Output : Test Summary *** " << std::endl ;
    //
    LogFile << std::endl;
    //
    LogFile << "Operator : " << Operator_ << std::endl ;
    LogFile << "Time     : " << ctime(&rawtime) << std::endl ;
    LogFile << "XML File : " << xmlFile_.toString() << std::endl ;
    //
    LogFile << std::endl ;
    LogFile << "Chassis " << std::setw(7) << CrateChassisID_ << std::endl;
    LogFile << "Backplane " << std::setw(5) << BackplaneID_ << std::endl;
    LogFile << "CRB " << std::setw(6) << CrateRegulatorBoardID_ << std::endl;
    LogFile << "PCMB " << std::setw(5) << PeripheralCrateMotherBoardID_ << std::endl;
    LogFile << "ELMB " << std::setw(5) << ELMBID_ << std::endl;
    LogFile << std::endl ;
    //
    LogFile << "VCC    1" << std::setw(5) << ControllerBoardID_ << std::endl;
    //
    for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) {
      //
      LogFile << "TMB" << std::setw(5) << tmbVector[i]->slot() << std::setw(5) <<
	TMBBoardID_[i] << std::setw(5) << RATBoardID_[i] <<std::setw(5) <<
	tmbTestVector[i].GetResultTestBootRegister() << std::setw(5) <<
	//	tmbTestVector[i].GetResultTestVMEfpgaDataRegister() << std::setw(5) <<
	tmbTestVector[i].GetResultTestFirmwareDate() << std::setw(5) <<
	tmbTestVector[i].GetResultTestFirmwareType() << std::setw(5) <<
	tmbTestVector[i].GetResultTestFirmwareVersion() << std::setw(5) <<
	tmbTestVector[i].GetResultTestFirmwareRevCode() << std::setw(5) <<
	tmbTestVector[i].GetResultTestMezzId() << std::setw(5) <<
	tmbTestVector[i].GetResultTestPromId() << std::setw(5) <<
	tmbTestVector[i].GetResultTestPROMPath() << std::setw(5) <<
	tmbTestVector[i].GetResultTestDSN() << std::setw(5) <<
	tmbTestVector[i].GetResultTestADC() << std::setw(5) <<
	tmbTestVector[i].GetResultTest3d3444() << std::setw(5) <<
	tmbTestVector[i].GetResultTestRATtemper() << std::setw(5) <<
	tmbTestVector[i].GetResultTestRATidCodes() << std::setw(5) <<
	tmbTestVector[i].GetResultTestRATuserCodes() << std::setw(5) <<
	tmbTestVector[i].GetResultTestU76chip() 
	      << std::endl ;
      //
    }
    //
    for(int i=0; i<20; i++) LogFile << "-";
    LogFile << std::endl ;
    //
    LogFile << "MPC     12 " << std::setw(5)  << MPCBoardID_ << std::endl;
    LogFile << "CCB     13 " << std::setw(5)  << CCBBoardID_ << std::endl;
    //
    for(int i=0; i<20; i++) LogFile << "-";
    LogFile << std::endl ;
    //
    for (unsigned int i=0; i<(dmbVector.size()<9?dmbVector.size():9) ; i++) {
      //
      LogFile << "DMB " << std::setw(5) << dmbVector[i]->slot() << std::setw(5) <<
	DMBBoardID_[i] ;
	for (int j=0; j<20; j++) LogFile << std::setw(5) << dmbVector[i]->GetTestStatus(j) ;
      LogFile << std::endl ;
      //
    }
    //
    LogFile << std::endl;
    //
    for (unsigned int dmbctr=0; dmbctr<(dmbVector.size()<9?dmbVector.size():9) ; dmbctr++) {
      DAQMB * thisDMB = dmbVector[dmbctr];
      vector<CFEB> thisCFEBs = thisDMB->cfebs();
      for (unsigned int cfebctr=0; cfebctr<thisCFEBs.size(); cfebctr++) {
	LogFile << "CFEBid " << std::setw(5) << dmbctr 
		<< std::setw(5) << cfebctr 
		<< std::setw(10) << CFEBid_[dmbctr][cfebctr] 
		<< std::endl;
      }
    }
    LogFile << std::endl;
    //
    for(int i=0; i<20; i++) LogFile << "+";
    LogFile << std::endl ;
    LogFile << " CrateTest : " << std::endl;
    for(int i=0; i<20; i++) LogFile << "-";
    LogFile << std::endl ;
    //
    LogFile << "MpcTMBTest " << myCrateTest.GetMpcTMBTestResult() << std::endl ;
    //
    LogFile << std::endl ;
    //
    for(int i=0; i<20; i++) LogFile << "+";
    LogFile << std::endl ;
    LogFile << " Timing scans : " << std::endl;
    for(int i=0; i<20; i++) LogFile << "-";
    LogFile << std::endl ;

    for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) {
      //
      LogFile << "slot                 " << std::setw(3) << i 
	      << std::setw(5) << tmbVector[i]->slot()
	      << std::endl;
      LogFile << "cfeb0delay           " << std::setw(3) << i 
	      << std::setw(5) << MyTest[i].GetCFEBrxPhaseTest(0)
	      << std::endl;
      LogFile << "cfeb1delay           " << std::setw(3) << i 
	      << std::setw(5) << MyTest[i].GetCFEBrxPhaseTest(1)
	      << std::endl;
      LogFile << "cfeb2delay           " << std::setw(3) << i 
	      << std::setw(5) << MyTest[i].GetCFEBrxPhaseTest(2)
	      << std::endl;
      LogFile << "cfeb3delay           " << std::setw(3) << i 
	      << std::setw(5) << MyTest[i].GetCFEBrxPhaseTest(3)
	      << std::endl;
      LogFile << "cfeb4delay           " << std::setw(3) << i 
	      << std::setw(5) << MyTest[i].GetCFEBrxPhaseTest(4)
	      << std::endl;
      LogFile << "alct_tx_clock_delay  " << std::setw(3) << i 
	      << std::setw(5) << MyTest[i].GetALCTtxPhaseTest()
	      << std::endl;
      LogFile << "alct_rx_clock_delay  " << std::setw(3) << i 
	      << std::setw(5) << MyTest[i].GetALCTrxPhaseTest()
	      << std::endl;
      LogFile << "mpc_delay            " << std::setw(3) << i 
	      << std::setw(5) << MyTest[i].GetMPCdelayTest()
	      << std::endl;
      LogFile << "rat_tmb_delay        " << std::setw(3) << i 
	      << std::setw(5) << MyTest[i].GetRatTmbDelayTest()
	      << std::endl;
      LogFile << "rpc0_rat_delay       " << std::setw(3) << i 
	      << std::setw(5) << MyTest[i].GetRpcRatDelayTest(0)
	      << std::endl;
      LogFile << "tmb_l1a_delay        " << std::setw(3) << i 
	      << std::setw(5) << MyTest[i].GetTMBL1aTiming()
	      << std::endl;
      LogFile << "alct_l1a_delay       " << std::setw(3) << i 
	      << std::setw(5) << MyTest[i].GetALCTL1aDelay()
	      << std::endl;
      LogFile << "match_trig_alct_delay" << std::setw(3) << i 
	      << std::setw(5) << MyTest[i].GetALCTvpf()
	      << std::endl;
      LogFile << "TTCrxID              " << std::setw(3) << i 
	      << std::setw(5) << thisCCB->GetReadTTCrxID() 
	      << std::endl;
      for (int CFEBs = 0; CFEBs<5; CFEBs++) {
	LogFile << "cfeb" << CFEBs << "_scan " << std::setw(3) << i;
	for (int HalfStrip = 0; HalfStrip<32; HalfStrip++) 
	  LogFile << std::setw(3) << MyTest[i].GetCFEBStripScan(CFEBs,HalfStrip) ;
	LogFile << std::endl;
      }
      //
      LogFile << "alct_scan  " << std::setw(3) << i;
      for (int Wire = 0; Wire<(tmbVector[i]->alctController()->GetNumberOfChannelsInAlct())/6; Wire++) 
	LogFile << std::setw(3) << MyTest[i].GetALCTWireScan(Wire) ;
      LogFile << std::endl;
      //
      LogFile << std::endl;
    }
    //
    LogFile.close();
    //
    this->Default(in,out);
    //
  }
  //
  void EmuPeripheralCrate::LogOutput(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception){
    //
    std::cout << "LogOuput" << std::endl;
    //
    //
    std::string buf;
    //int test = 1;
    buf = "EmuPeripheralCrateLogFile_"+RunNumber_+"_"+Operator_+".log";
    //
    std::cout << "File to write to" << buf << std::endl ;
    //
    ifstream TextFile ;
    TextFile.open(xmlFile_.toString().c_str());
    //
    ofstream LogFile;
    LogFile.open(buf.c_str());
    while(TextFile.good()) LogFile << (char) TextFile.get() ;
    TextFile.close();
    for (unsigned int i=0; i<tmbVector.size(); i++) {
      LogFile << OutputTMBTests[i].str() ;
      LogFile << CrateTestsOutput[i].str() ;
    }
    for (unsigned int i=0; i<dmbVector.size(); i++) {
      LogFile << OutputDMBTests[i].str() ;
    }
    LogFile.close();    
    //
    this->Default(in,out);
    //
  }
  //
  void EmuPeripheralCrate::LogTMBTestsOutput(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "LogTMBTestsOutput" << std::endl;
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    //
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "Not tmb" << endl ;
      tmb = TMB_;
    }
    //
    cgicc::form_iterator name2 = cgi.getElement("ClearTMBTestsOutput");
    //
    if(name2 != cgi.getElements().end()) {
      cout << "Clear..." << endl;
      cout << cgi["ClearTMBTestsOutput"]->getValue() << std::endl ;
      OutputTMBTests[tmb].str("");
      //
      this->TMBTests(in,out);
      return ;
      //
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    //
    cout << TMBBoardID_[tmb] << endl ;
    //
    char buf[20];
    sprintf(buf,"TMBTestsLogFile_%d_%s.log",thisTMB->slot(),TMBBoardID_[tmb].c_str());
    //
    ofstream TMBTestsLogFile;
    TMBTestsLogFile.open(buf);
    TMBTestsLogFile << OutputTMBTests[tmb].str() ;
    TMBTestsLogFile.close();
    //
    OutputTMBTests[tmb].str("");
    //
    this->TMBTests(in,out);
    //
  }
  //
  void EmuPeripheralCrate::LogCrateTestsOutput(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "LogCrateTestsOutput" << std::endl;
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    //
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "Not tmb" << endl ;
      tmb = TMB_;
    }
    //
    cgicc::form_iterator name2 = cgi.getElement("ClearCrateTestsOutput");
    //
    if(name2 != cgi.getElements().end()) {
      cout << "Clear..." << endl;
      cout << cgi["ClearCrateTestsOutput"]->getValue() << std::endl ;
      CrateTestsOutput[tmb].str("");
      //
      this->ChamberTests(in,out);
      return ;
      //
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    //
    cout << TMBBoardID_[tmb] << endl ;
    //
    char buf[20];
    sprintf(buf,"CrateTestsLogFile_%d_%s.log",thisTMB->slot(),TMBBoardID_[tmb].c_str());
    //
    ofstream CrateTestsLogFile;
    CrateTestsLogFile.open(buf);
    CrateTestsLogFile << CrateTestsOutput[tmb].str() ;
    CrateTestsLogFile.close();
    //
    CrateTestsOutput[tmb].str("");
    //
    this->ChamberTests(in,out);
    //
  }
  //
  void EmuPeripheralCrate::setRawConfFile(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    try
      {
	cout << "setRawConfFile" << endl ;
	//
	cgicc::Cgicc cgi(in);
	//
	ofstream TextFile ;
	TextFile.open("MyTextConfigurationFile.xml");
	TextFile << (cgi["Text"]->getValue()) ; 
	TextFile.close();
	//
	xmlFile_ = "MyTextConfigurationFile.xml" ;
	//
	cout << "Out setRawConfFile" << endl ;
	//
	this->Default(in,out);
      }
    catch (const std::exception & e )
      {
	//XECPT_RAISE(xgi::exception::Exception, e.what());
      }
  }
  //
  void EmuPeripheralCrate::ConfigureInit(){
    //
    if( Configuring() )
      {
	MyController->configure();
      }
    //
  }
  //
  bool EmuPeripheralCrate::Configuring(){
    //
    LOG4CPLUS_INFO(getApplicationLogger(),"Configuring");
    //
    //-- parse XML file
    //
    //cout << "---- XML parser ----" << endl;
    //cout << " Here parser " << endl;
    //PeripheralCrateParser parser;
    //cout << " Using file " << xmlFile_.toString() << endl ;
    //
    //parser.parseFile(xmlFile_.toString().c_str());
    //
    if ( MyController != 0 ) {
      LOG4CPLUS_INFO(getApplicationLogger(), "Delete existing controller");
      delete MyController ;
    }
    //
    MyController = new EmuController();
    //
    // Check if filename exists
    //
    if(xmlFile_.toString().find("http") == string::npos) {
      std::ifstream filename(xmlFile_.toString().c_str());
      if(filename.is_open()) {
	filename.close();
      }
      else {
	LOG4CPLUS_ERROR(getApplicationLogger(), "Filename doesn't exist");
	XCEPT_RAISE (toolbox::fsm::exception::Exception, "Filename doesn't exist");
	return false;
      }
    }
    //
    //cout <<"Entered Configuring"<<endl; //gujh APR20

    MyController->SetConfFile(xmlFile_.toString().c_str());
    MyController->init();
    //
    emuSystem_ = MyController->GetEmuSystem();
    //
    //-- Make sure that only one TMB in one crate is configured
    //
    //cout <<" Read xml file GUJH"<<endl; //gujh APR20

    CrateSelector selector = MyController->selector();
    vector<Crate*> crateVector = selector.crates();
    //
    if (crateVector.size() > 1 ) LOG4CPLUS_ERROR(getApplicationLogger(),"Warning...this configuration file has more than one crate");
    /* durkin kludge set up selectorbroadcasts
    vector<Crate *> tmpcrate=selector.broadcast_crate();
    broadcastCrate=tmpcrate[0];
    broadcastDMB=selector.daqmbs(tmpcrate[0])[0];
    */
    thisCrate = crateVector[0];
    tmbVector = selector.tmbs(crateVector[0]);
    //
    CrateUtils MyCrateUtils;
    MyCrateUtils.SetCrate(crateVector[0]);
    //
    //cout<<" Crate defined GUJH"<<endl; //gujh APR20
    if ( ! crateVector[0] ) {
      std::cout << "Crate doesn't exist" << std::endl;
      assert(crateVector[0]);
    }
    //
    tmbTestVector = MyCrateUtils.TMBTests();
    //
    //if (tmbVector.size() > 1){
    //cerr << "Error: only one TMB in xml file allowed" << endl ;
    //exit(1);
    //}
    //
    dmbVector = selector.daqmbs(crateVector[0]);
    //
    //chamberVector = selector.chambers(crateVector[0]);
    //
    //if (dmbVector.size() > 1){
    //cerr << "Error: only one DMB in xml file allowed" << endl;
    //exit(1);
    //} 
    //
    //-- get pointers to CCB, TMB and DMB
    //
    thisCrate = crateVector[0];
    thisCCB = thisCrate->ccb();
    thisMPC = thisCrate->mpc();
    //
    //TMB * thisTMB = tmbVector[0];
    //DAQMB * thisDMB = dmbVector[0];
    //
    //DDU * thisDDU = thisCrate->ddu();
    //if(thisTMB) alct = thisTMB->alctController();
    //
    std::cout << "Done" << std::endl ;
  }
  //

  void EmuPeripheralCrate::setConfFile(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    try
      {
	//
	cgicc::Cgicc cgi(in);
	//
	const_file_iterator file;
	file = cgi.getFile("xmlFileName");
	//
	cout << "GetFiles string" << endl ;
	//
	if(file != cgi.getFiles().end()) (*file).writeToStream(cout);
	//
	string XMLname = cgi["xmlFileName"]->getValue() ; 
	//
	cout << XMLname  << endl ;
	//
	xmlFile_ = XMLname ;
	//
 	Configuring();
	//
	this->Default(in,out);
	//
      }
    catch (const std::exception & e )
      {
	//XECPT_RAISE(xgi::exception::Exception, e.what());
      }
  }
  //
  //
  void EmuPeripheralCrate::getTestLogFile(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    try
      {
	//
	cgicc::Cgicc cgi(in);
	//
	const_file_iterator file;
	file = cgi.getFile("TestLogFile");
	//
	cout << "GetFiles string" << endl ;
	//
	if(file != cgi.getFiles().end()) (*file).writeToStream(cout);
	//
	string TestLogFile = cgi["TestLogFile"]->getValue() ; 
	//
	cout << TestLogFile  << endl ;
	//
	TestLogFile_ = TestLogFile;
	//
	ParseTestLogFile(TestLogFile);
	//
	this->Default(in,out);
	//
      }
    catch (const std::exception & e )
      {
	//XECPT_RAISE(xgi::exception::Exception, e.what());
      }
  }

  void EmuPeripheralCrate::ParseTestLogFile(xdata::String logFile) 
    throw (xgi::exception::Exception){
    //
    ifstream TextFile ;
    std:: cout << "Opening file " << logFile.toString().c_str() << std::endl ;
    TextFile.open(logFile.toString().c_str());    
    //
    //while(TextFile.good()) std::cout << (char) TextFile.get() ;
    //
    bool FoundXML = false ;
    int nTMB = 0;
    int nDMB = 0;
    //
    std::string line, line0, line1, line2;
    while(TextFile.good()) {
      //std::cout << "Line" <<std::endl ;
      getline(TextFile,line);
      //std::cout << line << std::endl ;
      if ( line.find("XML File") != string::npos ) {
	std::cout << "Found" << std::endl ;
	int start = line.find("/");
	int end   = line.length();
	xmlFile_ = line.substr(start,end-start) ;
	//
	Configuring();
	FoundXML = true ;
	//
      }
      //
      //
      if ( FoundXML ) { // Processed XML File
	if ( line.find("Chassis") != string::npos ) {	  
	  //
	  int ID;
	  istringstream instring(line);
	  //
	  instring >> line0 >> ID;
	  std::cout << "Chassis.Setting " << ID << std::endl ;
	  //
	  char buf[20];
	  sprintf(buf,"%d",ID);
	  CrateChassisID_ = buf;
	}
	//
	if ( line.find("Backplane") != string::npos ) {	  
	  //
	  int ID;
	  istringstream instring(line);
	  //
	  instring >> line0 >> ID;
	  std::cout << "Backplane.Setting " << ID << std::endl ;
	  //
	  char buf[20];
	  sprintf(buf,"%d",ID);
	  BackplaneID_ = buf;
	}
	//
	if ( line.find("CRB") != string::npos ) {	  
	  //
	  int ID;
	  istringstream instring(line);
	  //
	  instring >> line0 >> ID;
	  std::cout << "CRB.Setting " << ID << std::endl ;
	  //
	  char buf[20];
	  sprintf(buf,"%d",ID);
	  CrateRegulatorBoardID_ = buf;
	}
	//
	if ( line.find("PCMB") != string::npos ) {	  
	  //
	  int ID;
	  istringstream instring(line);
	  //
	  instring >> line0 >> ID;
	  std::cout << "PCMB.Setting " << ID << std::endl ;
	  //
	  char buf[20];
	  sprintf(buf,"%d",ID);
	  PeripheralCrateMotherBoardID_ = buf;
	}
	//
	if ( line.find("ELMB") != string::npos ) {	  
	  //
	  int ID;
	  istringstream instring(line);
	  //
	  instring >> line0 >> ID;
	  std::cout << "ELMB.Setting " << ID << std::endl ;
	  //
	  char buf[20];
	  sprintf(buf,"%d",ID);
	  ELMBID_ = buf;
	}
	//
	if ( line.find("VCC") != string::npos ) {	  
	  //
	  int slot, boardid;
	  istringstream instring(line);
	  //
	  instring >> line0 >> slot >> boardid;
	  std::cout << "VCC.Setting " << slot << " " << boardid << std::endl ;
	  //
	  char buf[20];
	  sprintf(buf,"%d",boardid);
	  ControllerBoardID_ = buf;
	}
	//
	if ( line.find("TMB ") != string::npos ) {	  
	  //
	  int slot, boardid, testResult[20], ratid;
	  istringstream instring(line);
	  //
	  instring >> line0 >> slot >> boardid >> ratid 
		   >> testResult[0] 
		   >> testResult[1] 
		   >> testResult[2] 
		   >> testResult[3] 
		   >> testResult[4] 
		   >> testResult[5] 
		   >> testResult[6] 
		   >> testResult[7] 
		   >> testResult[8] 
		   >> testResult[9] 
		   >> testResult[10] 
		   >> testResult[11] 
		   >> testResult[12] 
		   >> testResult[13] 
		   >> testResult[14] ;
	  //
	  std::cout << "TMB.Setting " << nTMB << " " << boardid << " " << ratid << " " 
		    << testResult[0] <<  " " << testResult[1] << std::endl ;
	  //
	  char buf[20];
	  sprintf(buf,"%d",boardid);
	  TMBBoardID_[nTMB] = buf ;
	  sprintf(buf,"%d",ratid);
	  RATBoardID_[nTMB] = buf ;
	  //
	  tmbTestVector[nTMB].SetResultTestBootRegister(testResult[0]);
	  //	  tmbTestVector[nTMB].SetResultTestVMEfpgaDataRegister(testResult[1]);
	  tmbTestVector[nTMB].SetResultTestFirmwareDate(testResult[1]); 
	  tmbTestVector[nTMB].SetResultTestFirmwareType(testResult[2]); 
	  tmbTestVector[nTMB].SetResultTestFirmwareVersion(testResult[3]); 
	  tmbTestVector[nTMB].SetResultTestFirmwareRevCode(testResult[4]);
	  tmbTestVector[nTMB].SetResultTestMezzId(testResult[5]);
	  tmbTestVector[nTMB].SetResultTestPromId(testResult[6]);
	  tmbTestVector[nTMB].SetResultTestPROMPath(testResult[7]);
	  tmbTestVector[nTMB].SetResultTestDSN(testResult[8]);
	  tmbTestVector[nTMB].SetResultTestADC(testResult[9]);
	  tmbTestVector[nTMB].SetResultTest3d3444(testResult[10]);
	  tmbTestVector[nTMB].SetResultTestRATtemper(testResult[11]);
	  tmbTestVector[nTMB].SetResultTestRATidCodes(testResult[12]);
	  tmbTestVector[nTMB].SetResultTestRATuserCodes(testResult[13]);
	  tmbTestVector[nTMB].SetResultTestU76chip(testResult[14]);
	  //
	  nTMB++ ;
	  //	  
	}
	//
	if ( line.find("MPC") != string::npos ) {	  
	  //
	  int slot, boardid;
	  istringstream instring(line);
	  //
	  instring >> line0 >> slot >> boardid;
	  std::cout << "MPC.Setting " << slot << " " << boardid << std::endl ;
	  //
	  char buf[20];
	  sprintf(buf,"%d",boardid);
	  MPCBoardID_ = buf;
	}
	//
	if ( line.find("CCB") != string::npos ) {	  
	  //
	  int slot, boardid;
	  istringstream instring(line);
	  //
	  instring >> line0 >> slot >> boardid;
	  std::cout << "CCB.Setting " << slot << " " << boardid << std::endl ;
	  //
	  char buf[20];
	  sprintf(buf,"%d",boardid);
	  CCBBoardID_ = buf;
	}
	//
	if ( line.find("DMB") != string::npos ) {	  
	  //
	  int slot, boardid, testResult[20];
	  istringstream instring(line);
	  //
	  instring >> line0 >> slot >> boardid 
		   >> testResult[0] 
		   >> testResult[1] 
		   >> testResult[2] 
		   >> testResult[3] 
		   >> testResult[4] 
		   >> testResult[5] 
		   >> testResult[6] 
		   >> testResult[7] 
		   >> testResult[8] 
		   >> testResult[9] 
		   >> testResult[10] 
		   >> testResult[11] 
		   >> testResult[12] 
		   >> testResult[13] 
		   >> testResult[14] 
		   >> testResult[15] 
		   >> testResult[16] 
		   >> testResult[17] 
		   >> testResult[18] 
		   >> testResult[19] ;
	  //
	  std::cout << "DMB.Setting " << nDMB << " " << boardid 
		    << "slot " << slot 
		    << " " << testResult[0] <<  " " << testResult[1] << std::endl ;
	  //
	  char buf[20];
	  sprintf(buf,"%d",boardid);
	  DMBBoardID_[nDMB] = buf;
	  //
	  if ( dmbVector[nDMB]->slot() == slot ) {
	    for(int i=0; i<20; i++) dmbVector[nDMB]->SetTestStatus(i,testResult[i]);
	  }
	  //
	  nDMB++;
	  //
	}
	//
	if ( line.find("CFEBid") != string::npos ) {
	  //
	  int vectorId, CFEBnum, cfebID;
	  istringstream instring(line);
	  //
	  instring >> line0 >> vectorId >> CFEBnum >> cfebID;
	  std::cout << "CFEB.Setting DMB " << vectorId 
		    << " CFEB " << CFEBnum 
		    << " -> ID number = " << cfebID 
		    << std::endl ;
	  //
	  CFEBid_[vectorId][CFEBnum]=cfebID;
	  //
	}
	//
	if ( line.find("MpcTMBTest") != string::npos ) {
	  //
	  int result;
	  istringstream instring(line);
	  //
	  instring >> line0 >> result ;
	  //
	  myCrateTest.SetMpcTMBTestResult(result);
	  //
	}
	//
	if ( line.find("cfeb0delay") != string::npos ) {	  
	  //
	  int vectorid, result;
	  istringstream instring(line);
	  //
	  instring >> line0 >> vectorid >> result;
	  //
	  MyTest[vectorid].SetCFEBrxPhaseTest(0,result);
	}
	//
	if ( line.find("cfeb1delay") != string::npos ) {	  
	  //
	  int vectorid, result;
	  istringstream instring(line);
	  //
	  instring >> line0 >> vectorid >> result;
	  //
	  MyTest[vectorid].SetCFEBrxPhaseTest(1,result);
	}
	//
	if ( line.find("cfeb2delay") != string::npos ) {	  
	  //
	  int vectorid, result;
	  istringstream instring(line);
	  //
	  instring >> line0 >> vectorid >> result;
	  //
	  MyTest[vectorid].SetCFEBrxPhaseTest(2,result);
	}
	//
	if ( line.find("cfeb3delay") != string::npos ) {	  
	  //
	  int vectorid, result;
	  istringstream instring(line);
	  //
	  instring >> line0 >> vectorid >> result;
	  //
	  MyTest[vectorid].SetCFEBrxPhaseTest(3,result);
	}
	//
	if ( line.find("cfeb4delay") != string::npos ) {	  
	  //
	  int vectorid, result;
	  istringstream instring(line);
	  //
	  instring >> line0 >> vectorid >> result;
	  //
	  MyTest[vectorid].SetCFEBrxPhaseTest(4,result);
	}
	//
	if ( line.find("alct_tx_clock_delay") != string::npos ) {	  
	  //
	  int vectorid, result;
	  istringstream instring(line);
	  //
	  instring >> line0 >> vectorid >> result;
	  //
	  MyTest[vectorid].SetALCTtxPhaseTest(result);
	}
	//
	if ( line.find("alct_rx_clock_delay") != string::npos ) {	  
	  //
	  int vectorid, result;
	  istringstream instring(line);
	  //
	  instring >> line0 >> vectorid >> result;
	  //
	  MyTest[vectorid].SetALCTrxPhaseTest(result);
	}
	//
	if ( line.find("mpc_delay") != string::npos ) {	  
	  //
	  int vectorid, result;
	  istringstream instring(line);
	  //
	  instring >> line0 >> vectorid >> result;
	  //
	  MyTest[vectorid].SetMPCdelayTest(result);
	}
	//
	if ( line.find("rat_tmb_delay") != string::npos ) {	  
	  //
	  int vectorid, result;
	  istringstream instring(line);
	  //
	  instring >> line0 >> vectorid >> result;
	  //
	  MyTest[vectorid].SetRatTmbDelayTest(result);
	}
	//
	if ( line.find("rpc0_rat_delay") != string::npos ) {	  
	  //
	  int vectorid, result;
	  istringstream instring(line);
	  //
	  instring >> line0 >> vectorid >> result;
	  //
	  MyTest[vectorid].SetRpcRatDelayTest(0,result);
	}
	//
	if ( line.find("tmb_l1a_delay") != string::npos ) {	  
	  //
	  int vectorid, result;
	  istringstream instring(line);
	  //
	  instring >> line0 >> vectorid >> result;
	  //
	  MyTest[vectorid].SetTMBL1aTiming(result);
	}
	//
	if ( line.find("alct_l1a_delay") != string::npos ) {	  
	  //
	  int vectorid, result;
	  istringstream instring(line);
	  //
	  instring >> line0 >> vectorid >> result;
	  //
	  MyTest[vectorid].SetALCTL1aDelay(result);
	}
	//
	if ( line.find("match_trig_alct_delay") != string::npos ) {	  
	  //
	  int vectorid, result;
	  istringstream instring(line);
	  //
	  instring >> line0 >> vectorid >> result;
	  //
	  MyTest[vectorid].SetALCTvpf(result);
	}
	//
	if ( line.find("TTCrxID") != string::npos ) {	  
	  //
	  int vectorid, boardid;
	  istringstream instring(line);
	  //
	  instring >> line0 >> vectorid >> boardid;
	  std::cout << "TTCrxID.Setting " << boardid << std::endl ;
	  //
	  if (thisCCB) 
	    thisCCB->SetReadTTCrxID(boardid);
	}
	//
	if ( line.find("cfeb0_scan") != string::npos ) {	  
	  //
	  int vectorid, hs[32];
	  istringstream instring(line);
	  //
	  instring >> line0 >> vectorid 
		   >> hs[0] >> hs[1] >> hs[2] >> hs[3] >> hs[4] >> hs[5] >> hs[6] >> hs[7] >> hs[8] >> hs[9]
		   >> hs[10] >> hs[11] >> hs[12] >> hs[13] >> hs[14] >> hs[15] >> hs[16] >> hs[17] >> hs[18] >> hs[19]
		   >> hs[20] >> hs[21] >> hs[22] >> hs[23] >> hs[24] >> hs[25] >> hs[26] >> hs[27] >> hs[28] >> hs[29]
		   >> hs[30] >> hs[31];
	  for (int HalfStrip = 0; HalfStrip<32; HalfStrip++) 
	    MyTest[vectorid].SetCFEBStripScan(0,HalfStrip,hs[HalfStrip]);
	}
	//
	if ( line.find("cfeb1_scan") != string::npos ) {	  
	  //
	  int vectorid, hs[32];
	  istringstream instring(line);
	  //
	  instring >> line0 >> vectorid 
		   >> hs[0] >> hs[1] >> hs[2] >> hs[3] >> hs[4] >> hs[5] >> hs[6] >> hs[7] >> hs[8] >> hs[9]
		   >> hs[10] >> hs[11] >> hs[12] >> hs[13] >> hs[14] >> hs[15] >> hs[16] >> hs[17] >> hs[18] >> hs[19]
		   >> hs[20] >> hs[21] >> hs[22] >> hs[23] >> hs[24] >> hs[25] >> hs[26] >> hs[27] >> hs[28] >> hs[29]
		   >> hs[30] >> hs[31];
	  for (int HalfStrip = 0; HalfStrip<32; HalfStrip++) 
	    MyTest[vectorid].SetCFEBStripScan(1,HalfStrip,hs[HalfStrip]);
	}
	//
	if ( line.find("cfeb2_scan") != string::npos ) {	  
	  //
	  int vectorid, hs[32];
	  istringstream instring(line);
	  //
	  instring >> line0 >> vectorid 
		   >> hs[0] >> hs[1] >> hs[2] >> hs[3] >> hs[4] >> hs[5] >> hs[6] >> hs[7] >> hs[8] >> hs[9]
		   >> hs[10] >> hs[11] >> hs[12] >> hs[13] >> hs[14] >> hs[15] >> hs[16] >> hs[17] >> hs[18] >> hs[19]
		   >> hs[20] >> hs[21] >> hs[22] >> hs[23] >> hs[24] >> hs[25] >> hs[26] >> hs[27] >> hs[28] >> hs[29]
		   >> hs[30] >> hs[31];
	  for (int HalfStrip = 0; HalfStrip<32; HalfStrip++) 
	    MyTest[vectorid].SetCFEBStripScan(2,HalfStrip,hs[HalfStrip]);
	}
	//
	if ( line.find("cfeb3_scan") != string::npos ) {	  
	  //
	  int vectorid, hs[32];
	  istringstream instring(line);
	  //
	  instring >> line0 >> vectorid 
		   >> hs[0] >> hs[1] >> hs[2] >> hs[3] >> hs[4] >> hs[5] >> hs[6] >> hs[7] >> hs[8] >> hs[9]
		   >> hs[10] >> hs[11] >> hs[12] >> hs[13] >> hs[14] >> hs[15] >> hs[16] >> hs[17] >> hs[18] >> hs[19]
		   >> hs[20] >> hs[21] >> hs[22] >> hs[23] >> hs[24] >> hs[25] >> hs[26] >> hs[27] >> hs[28] >> hs[29]
		   >> hs[30] >> hs[31];
	  for (int HalfStrip = 0; HalfStrip<32; HalfStrip++) 
	    MyTest[vectorid].SetCFEBStripScan(3,HalfStrip,hs[HalfStrip]);
	}
	//
	if ( line.find("cfeb4_scan") != string::npos ) {	  
	  //
	  int vectorid, hs[32];
	  istringstream instring(line);
	  //
	  instring >> line0 >> vectorid 
		   >> hs[0] >> hs[1] >> hs[2] >> hs[3] >> hs[4] >> hs[5] >> hs[6] >> hs[7] >> hs[8] >> hs[9]
		   >> hs[10] >> hs[11] >> hs[12] >> hs[13] >> hs[14] >> hs[15] >> hs[16] >> hs[17] >> hs[18] >> hs[19]
		   >> hs[20] >> hs[21] >> hs[22] >> hs[23] >> hs[24] >> hs[25] >> hs[26] >> hs[27] >> hs[28] >> hs[29]
		   >> hs[30] >> hs[31];
	  for (int HalfStrip = 0; HalfStrip<32; HalfStrip++) 
	    MyTest[vectorid].SetCFEBStripScan(4,HalfStrip,hs[HalfStrip]);
	}
	//
	if ( line.find("alct_scan") != string::npos ) {	  
	  //
	  int vectorid, hs[112];
	  istringstream instring(line);
	  //
	  instring >> line0 >> vectorid 
		   >> hs[0] >> hs[1] >> hs[2] >> hs[3] >> hs[4] >> hs[5] >> hs[6] >> hs[7] >> hs[8] >> hs[9]
		   >> hs[10] >> hs[11] >> hs[12] >> hs[13] >> hs[14] >> hs[15] >> hs[16] >> hs[17] >> hs[18] >> hs[19]
		   >> hs[20] >> hs[21] >> hs[22] >> hs[23] >> hs[24] >> hs[25] >> hs[26] >> hs[27] >> hs[28] >> hs[29]
		   >> hs[30] >> hs[31] >> hs[32] >> hs[33] >> hs[34] >> hs[35] >> hs[36] >> hs[37] >> hs[38] >> hs[39]
		   >> hs[40] >> hs[41] >> hs[42] >> hs[43] >> hs[44] >> hs[45] >> hs[46] >> hs[47] >> hs[48] >> hs[49]
		   >> hs[50] >> hs[51] >> hs[52] >> hs[53] >> hs[54] >> hs[55] >> hs[56] >> hs[57] >> hs[58] >> hs[59]
		   >> hs[60] >> hs[61] >> hs[62] >> hs[63] >> hs[64] >> hs[65] >> hs[66] >> hs[67] >> hs[68] >> hs[69]
		   >> hs[70] >> hs[71] >> hs[72] >> hs[73] >> hs[74] >> hs[75] >> hs[76] >> hs[77] >> hs[78] >> hs[79]
		   >> hs[80] >> hs[81] >> hs[82] >> hs[83] >> hs[84] >> hs[85] >> hs[86] >> hs[87] >> hs[88] >> hs[89]
		   >> hs[90] >> hs[91] >> hs[92] >> hs[93] >> hs[94] >> hs[95] >> hs[96] >> hs[97] >> hs[98] >> hs[99]
		   >> hs[100] >> hs[101] >> hs[102] >> hs[103] >> hs[104] >> hs[105] >> hs[106] >> hs[107] >> hs[108] 
		   >> hs[109] >> hs[110] >> hs[111];
	  for (int Wire = 0; Wire<(tmbVector[vectorid]->alctController()->GetNumberOfChannelsInAlct())/6; Wire++) 
	    MyTest[vectorid].SetALCTWireScan(Wire,hs[Wire]) ;

	}
	//
      }
    }
    //
    std::cout << std::endl ;
    //
    TextFile.close();
    //
    
  }
  //
  void EmuPeripheralCrate::getTestLogFileUpload(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    try
      {
	//
	cgicc::Cgicc cgi(in);
	//
	const_file_iterator file;
	file = cgi.getFile("TestFileUpload");
	//
	cout << "GetFiles" << endl ;
	//
	if(file != cgi.getFiles().end()) {
	  ofstream TextFile ;
	  TextFile.open("MyTestLogFile.xml");
	  (*file).writeToStream(TextFile);
	  TextFile.close();
	}
	//
	TestLogFile_ = "MyTestLogFile.xml" ;
	//
	ParseTestLogFile(TestLogFile_);
	//
	//cout << "UploadConfFile done" << endl ;
	//
	this->Default(in,out);
	//
      }
    catch (const std::exception & e )
      {
	//XECPT_RAISE(xgi::exception::Exception, e.what());
      }
  }
  //
  void EmuPeripheralCrate::UploadConfFile(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    try
      {
	//
	cout << "UploadConfFileUpload" << endl ;
	//
	cgicc::Cgicc cgi(in);
	//
	const_file_iterator file;
	file = cgi.getFile("xmlFileNameUpload");
	//
	cout << "GetFiles" << endl ;
	//
	if(file != cgi.getFiles().end()) {
	  ofstream TextFile ;
	  TextFile.open("MyTextConfigurationFile.xml");
	  (*file).writeToStream(TextFile);
	  TextFile.close();
	}
	//
	xmlFile_ = "MyTextConfigurationFile.xml" ;
	//
	Configuring();
	//
	cout << "UploadConfFile done" << endl ;
	//
	this->Default(in,out);
	//
      }
    catch (const std::exception & e )
      {
	//XECPT_RAISE(xgi::exception::Exception, e.what());
      }
  }
//
  xoap::MessageReference EmuPeripheralCrate::ReadAllVmePromUserid (xoap::MessageReference message) throw (xoap::exception::Exception)
  {
    //implement the DMB VME PROM USER_CODE Readback
    std::cout << "DMB VME PROM USER_CODE Readback " << std::endl;
    for (int idmb=0;idmb<dmbVector.size();idmb++)
    {
      if ((dmbVector[idmb]->slot())<22) {
	DAQMB * thisDMB=dmbVector[idmb];
        unsigned long int boardnumber=thisDMB->mbpromuser(0);
        DMBBoardNumber[idmb]=boardnumber;
	cout <<" The DMB Number: "<<idmb<<" is in Slot Number: "<<dmbVector[idmb]->slot()<<endl;
	cout <<" This DMB Board Number: "<<DMBBoardNumber[idmb]<<endl<<endl;
      }
    }
    return createReply(message);
  }

  xoap::MessageReference EmuPeripheralCrate::LoadAllVmePromUserid (xoap::MessageReference message) throw (xoap::exception::Exception)
  {
    //implement the DMB VME PROM USER_CODE programming
    std::cout << "DMB VME PROM USER_CODE Programming " << std::endl;
    usleep(200);
    for (int idmb=0;idmb<dmbVector.size();idmb++)
    {
      if ((dmbVector[idmb]->slot())<22) {
	DAQMB * thisDMB=dmbVector[idmb];
        unsigned long int boardnumber=DMBBoardNumber[idmb];
	char prombrdname[4];
	//	if (idmb==0) boardnumber = 0xdb00000c;
	//	if (idmb==1) boardnumber = 0xdb00021b;
	prombrdname[0]=boardnumber&0xff;
	prombrdname[1]=(boardnumber>>8)&0xff;
	prombrdname[2]=(boardnumber>>16)&0xff;
	prombrdname[3]=(boardnumber>>24)&0xff;
    	cout<<" Loading the board number ..."<<endl;
	thisDMB->epromload_broadcast(VPROM,"/home/cscpc/firmware/dmb/dmb6vme_pro.svf",1,prombrdname,2);
	usleep(200);
	cout <<" The DMB Number: "<<idmb<<" is in Slot Number: "<<dmbVector[idmb]->slot()<<endl;
	cout <<" This DMB is programmed to board number: "<<boardnumber<<endl<<endl;
      }
    }
    return createReply(message);
  }

  xoap::MessageReference EmuPeripheralCrate::ReadAllCfebPromUserid (xoap::MessageReference message) throw (xoap::exception::Exception)
  {
    //implement the CFEB PROM USER_CODE Readback
    std::cout << "CFEB PROM USER_CODE Readback " << std::endl;
    usleep(200);
    for (int idmb=0;idmb<dmbVector.size();idmb++)
    {
      if ((dmbVector[idmb]->slot())<22) {
	DAQMB * thisDMB=dmbVector[idmb];
	cout <<" The DMB Number: "<<idmb<<" is in Slot Number: "<<dmbVector[idmb]->slot()<<endl;
	//loop over the cfebs
	//define CFEBs
	vector <CFEB> thisCFEBs=thisDMB->cfebs();
	for (int i=0;i<thisCFEBs.size();i++) {
          CFEBBoardNumber[idmb][i]=thisDMB->febpromuser(thisCFEBs[i]);
	  cout <<" This CFEB Board Number: "<<CFEBBoardNumber[idmb][i]<<endl;
	}
	cout <<endl;
      }
    }

    return createReply(message);
  }

  xoap::MessageReference EmuPeripheralCrate::LoadAllCfebPromUserid (xoap::MessageReference message) throw (xoap::exception::Exception)
  {
    //implement the CFEB PROM USER_CODE programming
    std::cout << "CFEB PROM USER_CODE Programming " << std::endl;
    for (int idmb=0;idmb<dmbVector.size();idmb++)
    {
      if ((dmbVector[idmb]->slot())<22) {
	DAQMB * thisDMB=dmbVector[idmb];
	cout <<" The DMB Number: "<<idmb<<" is in Slot Number: "<<dmbVector[idmb]->slot()<<endl;
	//loop over the cfebs
	//define CFEBs
	vector <CFEB> thisCFEBs=thisDMB->cfebs();
	for (int i=0;i<thisCFEBs.size();i++) {
	  char promid[4];
	  unsigned long int boardid=CFEBBoardNumber[idmb][i];
	  /*
    	  unsigned long int fpgaid=thisDMB->febfpgaid(thisCFEBs[i]);
	  cout <<" i= "<<i<<endl;
	  if (i==0) boardid=0xcfeb08e5;
	  if (i==1) boardid=0xcfeb08e1;
	  if (i==2) boardid=0xcfeb08e4;
	  if (i==3) boardid=0xcfeb0903;
	  if (i==4) boardid=0xcfeb063a;
	  cout <<" This CFEB Board Number should be set to: "<<boardid<<endl;
	  */
	  promid[0]=boardid&0xff;
	  promid[1]=(boardid>>8)&0xff;
	  promid[2]=(boardid>>16)&0xff;
	  promid[3]=(boardid>>24)&0xff;
	  thisDMB->epromload_broadcast(thisCFEBs[i].promDevice(),"/home/cscpc/firmware/cfeb/cfeb_pro.svf",1,promid,2);
	  usleep(200);
	  cout <<" This CFEB Board Number is set to: "<<boardid<<endl;
	}
	cout <<endl;
      }
    }
    return createReply(message);
  }

//
// provides factory method for instantion of HellWorld application
//
XDAQ_INSTANTIATOR_IMPL(EmuPeripheralCrate)
//
