// $Id: EmuPeripheralCrateConfig.cc

#include "EmuPeripheralCrateConfig.h"

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
const unsigned int EXPECTED_CFEB_USERID   = 0xcfeda102;
//
const string       DMB_FIRMWARE_FILENAME    = "dmb/dmb6cntl_pro.svf";
const unsigned int EXPECTED_DMB_USERID      = 0x48547241;
const string       DMBVME_FIRMWARE_FILENAME = "dmb/dmb6vme_pro.svf";
const int EXPECTED_DMB_VME_VERSION       = 1547;
const int EXPECTED_DMB_FIRMWARE_REVISION =    1;
const string       VMECC_FIRMWARE_DIR = "vcc"; 
const string       VMECC_FIRMWARE_VER ="4.31";   
//const string       DMBVME_FIRMWARE_FILENAME = "dmb/dmb6vme_v11_r1.svf";
//
//In order to load firmware automatically from the firmware values in the xml files, 
//the firmware needs to reside in directories in the form:
//    TMB  ->  $HOME/firmware/tmb/YEARMONTHDAY/tmb.xsvf   <-- N.B. xsvf format for TMB
//    RAT  ->  $HOME/firmware/rat/YEARMONTHDAY/rat.svf
//    ALCT ->  $HOME/firmware/alct/YEARMONTHDAY/alctXXX/alctXXX.svf
// with the zero-values filled in with 0's.  
// In other words:  9 April 2007 firmware should reside in YEARMONTHDAY=20070409
//
// The XXX in the ALCT firmware specification corresponds to the following structure:
const string ALCT_FIRMWARE_FILENAME_ME11 = "alct288/alct288.svf";//
const string ALCT_FIRMWARE_FILENAME_ME11_BACKWARD_NEGATIVE = "alct288bn/alct288bn.svf";//
const string ALCT_FIRMWARE_FILENAME_ME11_BACKWARD_POSITIVE = "alct288bp/alct288bp.svf";//
const string ALCT_FIRMWARE_FILENAME_ME11_FORWARD_POSITIVE  = "alct288fp/alct288fp.svf";//
const string ALCT_FIRMWARE_FILENAME_ME12 = "alct384/alct384.svf";//
const string ALCT_FIRMWARE_FILENAME_ME13 = "alct192/alct192.svf";//
const string ALCT_FIRMWARE_FILENAME_ME21 = "alct672/alct672.svf";//
const string ALCT_FIRMWARE_FILENAME_ME22 = "alct384/alct384.svf";//
const string ALCT_FIRMWARE_FILENAME_ME31 = "alct576mirror/alct576mirror.svf";//
const string ALCT_FIRMWARE_FILENAME_ME32 = "alct384mirror/alct384mirror.svf";//
const string ALCT_FIRMWARE_FILENAME_ME41 = "alct576mirror/alct576mirror.svf";//
//
// N.B. not yet able to load automatically from xml for RAT...
const string RAT_FIRMWARE_FILENAME = "rat/20060828/rat.svf";

/////////////////////////////////////////////////////////////////////
// Instantiation and main page
/////////////////////////////////////////////////////////////////////
EmuPeripheralCrateConfig::EmuPeripheralCrateConfig(xdaq::ApplicationStub * s): EmuApplication(s)
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
  tmb_vme_ready = -1;
  //
  all_crates_ok = -1;
  for (int i=0; i<60; i++) {
    crate_check_ok[i] = -1;
    for (int j=0; j<9; j++) {
      alct_check_ok[i][j] = -1;
      tmb_check_ok[i][j] = -1;
      dmb_check_ok[i][j] = -1;
    }
  }
  //
  xgi::bind(this,&EmuPeripheralCrateConfig::Default, "Default");
  xgi::bind(this,&EmuPeripheralCrateConfig::MainPage, "MainPage");
  xgi::bind(this,&EmuPeripheralCrateConfig::setConfFile, "setConfFile");
  //
  xgi::bind(this,&EmuPeripheralCrateConfig::InitSystem, "InitSystem");
  //
  //------------------------------------------------------
  // bind buttons -> other pages
  //------------------------------------------------------
  xgi::bind(this,&EmuPeripheralCrateConfig::TMBStatus, "TMBStatus");
  xgi::bind(this,&EmuPeripheralCrateConfig::TMBTests,  "TMBTests");
  xgi::bind(this,&EmuPeripheralCrateConfig::TMBUtils,  "TMBUtils");
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBStatus, "DMBStatus");
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBTests, "DMBTests");
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBUtils, "DMBUtils");
  xgi::bind(this,&EmuPeripheralCrateConfig::CCBStatus, "CCBStatus");
  xgi::bind(this,&EmuPeripheralCrateConfig::CCBUtils, "CCBUtils");
  xgi::bind(this,&EmuPeripheralCrateConfig::MPCStatus, "MPCStatus");
  xgi::bind(this,&EmuPeripheralCrateConfig::MPCUtils, "MPCUtils");
  //
  //------------------------------
  // bind crate utilities
  //------------------------------
  xgi::bind(this,&EmuPeripheralCrateConfig::TmbMPCTest, "TmbMPCTest");
  xgi::bind(this,&EmuPeripheralCrateConfig::MPCSafeWindowScan, "MPCSafeWindowScan");
  //
  xgi::bind(this,&EmuPeripheralCrateConfig::CheckCrates, "CheckCrates");
  xgi::bind(this,&EmuPeripheralCrateConfig::CheckCratesConfiguration, "CheckCratesConfiguration");
  xgi::bind(this,&EmuPeripheralCrateConfig::CheckCrateConfiguration, "CheckCrateConfiguration");
  xgi::bind(this,&EmuPeripheralCrateConfig::CrateSelection, "CrateSelection");
  xgi::bind(this,&EmuPeripheralCrateConfig::setRawConfFile, "setRawConfFile");
  xgi::bind(this,&EmuPeripheralCrateConfig::UploadConfFile, "UploadConfFile");
  xgi::bind(this,&EmuPeripheralCrateConfig::SetUnsetRatio, "SetUnsetRatio");
  xgi::bind(this,&EmuPeripheralCrateConfig::SetUnsetAutoRefresh, "SetUnsetAutoRefresh");
  xgi::bind(this,&EmuPeripheralCrateConfig::DefineConfiguration, "DefineConfiguration");
  xgi::bind(this,&EmuPeripheralCrateConfig::EnableDisableDebug, "EnableDisableDebug");
  xgi::bind(this,&EmuPeripheralCrateConfig::ExcludeIncludeCrate, "ExcludeIncludeCrate");
  xgi::bind(this,&EmuPeripheralCrateConfig::ReadCCBRegister, "ReadCCBRegister");
  xgi::bind(this,&EmuPeripheralCrateConfig::ReadTTCRegister, "ReadTTCRegister");
  xgi::bind(this,&EmuPeripheralCrateConfig::HardReset, "HardReset");
  xgi::bind(this,&EmuPeripheralCrateConfig::testTMB, "testTMB");
  xgi::bind(this,&EmuPeripheralCrateConfig::CCBLoadFirmware, "CCBLoadFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::ControllerUtils, "ControllerUtils"); 
  xgi::bind(this,&EmuPeripheralCrateConfig::ReadVMECCRegisters,  "ReadVMECCRegisters"); 
  xgi::bind(this,&EmuPeripheralCrateConfig::VMECCLoadFirmware,  "VMECCLoadFirmware"); 
  xgi::bind(this,&EmuPeripheralCrateConfig::ControllerUtils, "ControllerUtils");  
  xgi::bind(this,&EmuPeripheralCrateConfig::CrateConfiguration, "CrateConfiguration");
  xgi::bind(this,&EmuPeripheralCrateConfig::CrateTests, "CrateTests");
  xgi::bind(this,&EmuPeripheralCrateConfig::ChamberTests, "ChamberTests");
  xgi::bind(this, &EmuPeripheralCrateConfig::ConfigAllCrates, "ConfigAllCrates");
  xgi::bind(this, &EmuPeripheralCrateConfig::FastConfigCrates, "FastConfigCrates");
  xgi::bind(this, &EmuPeripheralCrateConfig::FastConfigOne, "FastConfigOne");

  xgi::bind(this,&EmuPeripheralCrateConfig::MeasureDAVsForCrate,"MeasureDAVsForCrate");
  xgi::bind(this,&EmuPeripheralCrateConfig::MPCLoadFirmware, "MPCLoadFirmware");


  //-----------------------------------------------
  // DMB tests
  //-----------------------------------------------
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBTestAll, "DMBTestAll");
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBTest3, "DMBTest3");
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBTest4, "DMBTest4");
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBTest5, "DMBTest5");
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBTest6, "DMBTest6");
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBTest8, "DMBTest8");
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBTest9, "DMBTest9");
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBTest10, "DMBTest10");
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBTest11, "DMBTest11");
  //
  //-----------------------------------------------
  // DMB utilities
  //-----------------------------------------------
  xgi::bind(this,&EmuPeripheralCrateConfig::CFEBStatus, "CFEBStatus");
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBPrintCounters, "DMBPrintCounters");
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBTurnOff, "DMBTurnOff");
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBTurnOn, "DMBTurnOn");
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBLoadFirmware, "DMBLoadFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBVmeLoadFirmware, "DMBVmeLoadFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBVmeLoadFirmwareEmergency, "DMBVmeLoadFirmwareEmergency");
  xgi::bind(this,&EmuPeripheralCrateConfig::CFEBLoadFirmware, "CFEBLoadFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::CFEBLoadFirmwareID, "CFEBLoadFirmwareID");
  //
  //-----------------------------------------------
  // TMB tests
  //-----------------------------------------------
  //
  //-----------------------------------------------
  // TMB utilities
  //-----------------------------------------------
  xgi::bind(this,&EmuPeripheralCrateConfig::ALCTStatus, "ALCTStatus");
  xgi::bind(this,&EmuPeripheralCrateConfig::RATStatus, "RATStatus");
  xgi::bind(this,&EmuPeripheralCrateConfig::LoadTMBFirmware, "LoadTMBFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::CheckTMBFirmware, "CheckTMBFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::ClearTMBBootReg, "ClearTMBBootReg");
  xgi::bind(this,&EmuPeripheralCrateConfig::LoadALCTFirmware, "LoadALCTFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::LoadRATFirmware, "LoadRATFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::ReadTMBRegister, "ReadTMBRegister");
  xgi::bind(this,&EmuPeripheralCrateConfig::TMBPrintCounters, "TMBPrintCounters");
  xgi::bind(this,&EmuPeripheralCrateConfig::TMBResetCounters, "TMBResetCounters");
  xgi::bind(this,&EmuPeripheralCrateConfig::TMBCounterForFixedTime, "TMBCounterForFixedTime");
  xgi::bind(this,&EmuPeripheralCrateConfig::CalibrationRuns, "CalibrationRuns");
  xgi::bind(this,&EmuPeripheralCrateConfig::TriggerTestInjectALCT, "TriggerTestInjectALCT");
  xgi::bind(this,&EmuPeripheralCrateConfig::TriggerTestInjectCLCT, "TriggerTestInjectCLCT");
  xgi::bind(this,&EmuPeripheralCrateConfig::armScope, "armScope");
  xgi::bind(this,&EmuPeripheralCrateConfig::forceScope, "forceScope");
  xgi::bind(this,&EmuPeripheralCrateConfig::readoutScope, "readoutScope");
  xgi::bind(this,&EmuPeripheralCrateConfig::TMBDumpAllRegisters, "TMBDumpAllRegisters");
  xgi::bind(this,&EmuPeripheralCrateConfig::TMBConfigure, "TMBConfigure");
  xgi::bind(this,&EmuPeripheralCrateConfig::TMBClearUserProms, "TMBClearUserProms");
  xgi::bind(this,&EmuPeripheralCrateConfig::TMBReadConfiguration, "TMBReadConfiguration");
  xgi::bind(this,&EmuPeripheralCrateConfig::TMBCheckConfiguration, "TMBCheckConfiguration");
  xgi::bind(this,&EmuPeripheralCrateConfig::TMBReadStateMachines, "TMBReadStateMachines");
  xgi::bind(this,&EmuPeripheralCrateConfig::TMBCheckStateMachines, "TMBCheckStateMachines");
  xgi::bind(this,&EmuPeripheralCrateConfig::TMBRawHits, "TMBRawHits");
  xgi::bind(this,&EmuPeripheralCrateConfig::ALCTRawHits, "ALCTRawHits");
  //  xgi::bind(this,&EmuPeripheralCrateConfig::PowerUp,  "PowerUp");
  //
  //----------------------------
  // Bind logging methods
  //----------------------------
  xgi::bind(this,&EmuPeripheralCrateConfig::LogChamberTestsOutput, "LogChamberTestsOutput");
  xgi::bind(this,&EmuPeripheralCrateConfig::LogCrateTestsOutput, "LogCrateTestsOutput");
  xgi::bind(this,&EmuPeripheralCrateConfig::Operator, "Operator");
  xgi::bind(this,&EmuPeripheralCrateConfig::RunNumber, "RunNumber");
  xgi::bind(this,&EmuPeripheralCrateConfig::CrateChassisID, "CrateChassisID");
  xgi::bind(this,&EmuPeripheralCrateConfig::CrateRegulatorBoardID, "CrateRegulatorBoardID");
  xgi::bind(this,&EmuPeripheralCrateConfig::PeripheralCrateMotherBoardID, "PeripheralCrateMotherBoardID");
  xgi::bind(this,&EmuPeripheralCrateConfig::ELMBID, "ELMBID");
  xgi::bind(this,&EmuPeripheralCrateConfig::BackplaneID, "BackplaneID");
  xgi::bind(this,&EmuPeripheralCrateConfig::ControllerBoardID, "ControllerBoardID");
  xgi::bind(this,&EmuPeripheralCrateConfig::MPCBoardID, "MPCBoardID");
  xgi::bind(this,&EmuPeripheralCrateConfig::CCBBoardID, "CCBBoardID");
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBBoardID, "DMBBoardID");
  xgi::bind(this,&EmuPeripheralCrateConfig::TMBBoardID, "TMBBoardID");
  xgi::bind(this,&EmuPeripheralCrateConfig::RATBoardID, "RATBoardID");
  xgi::bind(this,&EmuPeripheralCrateConfig::LogOutput, "LogOutput");
  xgi::bind(this,&EmuPeripheralCrateConfig::LogTestSummary, "LogTestSummary");
  xgi::bind(this,&EmuPeripheralCrateConfig::LogDMBTestsOutput, "LogDMBTestsOutput");
  xgi::bind(this,&EmuPeripheralCrateConfig::LogTMBTestsOutput, "LogTMBTestsOutput");
  //
  //----------------------------------------------
  // Bind chamber test (synchronization) methods
  //----------------------------------------------
  xgi::bind(this,&EmuPeripheralCrateConfig::InitChamber, "InitChamber");                            //should be deprecated
  xgi::bind(this,&EmuPeripheralCrateConfig::ALCTTiming, "ALCTTiming");
  xgi::bind(this,&EmuPeripheralCrateConfig::CFEBTiming, "CFEBTiming");
  xgi::bind(this,&EmuPeripheralCrateConfig::FindDistripHotChannel, "FindDistripHotChannel");
  xgi::bind(this,&EmuPeripheralCrateConfig::setupCoincidencePulsing, "setupCoincidencePulsing");
  xgi::bind(this,&EmuPeripheralCrateConfig::TMBStartTrigger, "TMBStartTrigger");                    //should be deprecated
  xgi::bind(this,&EmuPeripheralCrateConfig::EnableL1aRequest, "EnableL1aRequest");                  //should be deprecated
  xgi::bind(this,&EmuPeripheralCrateConfig::setTMBCounterReadValues, "setTMBCounterReadValues");
  xgi::bind(this,&EmuPeripheralCrateConfig::setDataReadValues, "setDataReadValues");
  xgi::bind(this,&EmuPeripheralCrateConfig::Automatic, "Automatic");
  xgi::bind(this,&EmuPeripheralCrateConfig::ALCTvpf,"ALCTvpf");
  xgi::bind(this,&EmuPeripheralCrateConfig::FindWinner, "FindWinner");
  xgi::bind(this,&EmuPeripheralCrateConfig::TmbLctCableDelay, "TmbLctCableDelay");
  xgi::bind(this,&EmuPeripheralCrateConfig::PrintDmbValuesAndScopes, "PrintDmbValuesAndScopes");
  xgi::bind(this,&EmuPeripheralCrateConfig::TMBL1aTiming, "TMBL1aTiming");
  xgi::bind(this,&EmuPeripheralCrateConfig::ALCTL1aTiming, "ALCTL1aTiming");
  xgi::bind(this,&EmuPeripheralCrateConfig::AlctDavCableDelay, "AlctDavCableDelay");
  xgi::bind(this,&EmuPeripheralCrateConfig::CfebDavCableDelay, "CfebDavCableDelay");
  xgi::bind(this,&EmuPeripheralCrateConfig::RatTmbTiming, "RatTmbTiming");
  xgi::bind(this,&EmuPeripheralCrateConfig::RpcRatTiming, "RpcRatTiming");
  xgi::bind(this,&EmuPeripheralCrateConfig::ALCTScan, "ALCTScan");
  xgi::bind(this,&EmuPeripheralCrateConfig::CFEBScan, "CFEBScan");
  //
  //----------------------------
  // Bind calibration methods
  //----------------------------
  xgi::bind(this,&EmuPeripheralCrateConfig::FindLv1aDelayComparator, "FindLv1aDelayComparator");   
  xgi::bind(this,&EmuPeripheralCrateConfig::FindLv1aDelayALCT, "FindLv1aDelayALCT");
  xgi::bind(this,&EmuPeripheralCrateConfig::CalibrationCFEBXtalk, "CalibrationCFEBXtalk");
  xgi::bind(this,&EmuPeripheralCrateConfig::CalibrationCFEBGain, "CalibrationCFEBGain");
  xgi::bind(this,&EmuPeripheralCrateConfig::CalibrationCFEBSaturation, "CalibrationSaturation");
  xgi::bind(this,&EmuPeripheralCrateConfig::CalibrationCFEBPedestal, "CalibrationCFEBPedestal");
  xgi::bind(this,&EmuPeripheralCrateConfig::CalibrationComparatorPulse, "CalibrationComparatorPulse");
  xgi::bind(this,&EmuPeripheralCrateConfig::CalibrationALCTThresholdScan, "CalibrationALCTThresholdScan");
  xgi::bind(this,&EmuPeripheralCrateConfig::CalibrationALCTConnectivity, "CalibrationALCTConnectivity");
  xgi::bind(this,&EmuPeripheralCrateConfig::CalibrationCFEBConnectivity, "CalibrationCFEBConnectivity");
  //
  //----------------------------
  // Bind monitoring methods
  //----------------------------
  xgi::bind(this,&EmuPeripheralCrateConfig::LaunchMonitor, "LaunchMonitor");
  xgi::bind(this,&EmuPeripheralCrateConfig::MonitorTMBTrigger, "MonitorTMBTrigger");
  xgi::bind(this,&EmuPeripheralCrateConfig::MonitorTMBTriggerRedirect, "MonitorTMBTriggerRedirect");
  xgi::bind(this,&EmuPeripheralCrateConfig::MenuMonitorTMBTrigger, "MenuMonitorTMBTrigger");
  xgi::bind(this,&EmuPeripheralCrateConfig::AlctKey, "AlctKey");
  xgi::bind(this,&EmuPeripheralCrateConfig::ClctKey, "ClctKey");
  xgi::bind(this,&EmuPeripheralCrateConfig::MonitorTMBTriggerDisplay, "MonitorTMBTriggerDisplay");
  xgi::bind(this,&EmuPeripheralCrateConfig::CrateTMBCounters, "CrateTMBCounters");
  xgi::bind(this,&EmuPeripheralCrateConfig::CrateDMBCounters, "CrateDMBCounters");
  xgi::bind(this,&EmuPeripheralCrateConfig::CrateTMBCountersRight, "CrateTMBCountersRight");
  xgi::bind(this,&EmuPeripheralCrateConfig::CrateStatus, "CrateStatus");
  xgi::bind(this,&EmuPeripheralCrateConfig::CrateDumpConfiguration, "CrateDumpConfiguration");
  xgi::bind(this,&EmuPeripheralCrateConfig::CreateMonitorUnit, "CreateMonitorUnit");
  xgi::bind(this,&EmuPeripheralCrateConfig::MonitorFrameLeft, "MonitorFrameLeft");
  xgi::bind(this,&EmuPeripheralCrateConfig::MonitorFrameRight, "MonitorFrameRight");
  xgi::bind(this,&EmuPeripheralCrateConfig::ResetAllCounters, "ResetAllCounters");
  //
  // SOAP call-back functions, which relays to *Action method.
  //-----------------------------------------------------------
  xoap::bind(this, &EmuPeripheralCrateConfig::onConfigure, "Configure", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateConfig::onEnable,    "Enable",    XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateConfig::onDisable,   "Disable",   XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateConfig::onHalt,      "Halt",      XDAQ_NS_URI);
  //
  xoap::bind(this, &EmuPeripheralCrateConfig::onCalibration,"Calibration",XDAQ_NS_URI);
  //
  xoap::bind(this,&EmuPeripheralCrateConfig::ReadAllVmePromUserid ,"ReadVmePromUserid" ,XDAQ_NS_URI);
  xoap::bind(this,&EmuPeripheralCrateConfig::LoadAllVmePromUserid ,"LoadVmePromUserid" ,XDAQ_NS_URI);
  xoap::bind(this,&EmuPeripheralCrateConfig::ReadAllCfebPromUserid,"ReadCfebPromUserid",XDAQ_NS_URI);
  xoap::bind(this,&EmuPeripheralCrateConfig::LoadAllCfebPromUserid,"LoadCfebPromUserid",XDAQ_NS_URI);
  //
  xoap::bind(this,&EmuPeripheralCrateConfig::EnableJtagWriteALCT192      ,"EnableALCT192",XDAQ_NS_URI);
  xoap::bind(this,&EmuPeripheralCrateConfig::EnableJtagWriteALCT288      ,"EnableALCT288",XDAQ_NS_URI);
  xoap::bind(this,&EmuPeripheralCrateConfig::EnableJtagWriteALCT288bn    ,"EnableALCT288bn",XDAQ_NS_URI);
  xoap::bind(this,&EmuPeripheralCrateConfig::EnableJtagWriteALCT288bp    ,"EnableALCT288bp",XDAQ_NS_URI);
  xoap::bind(this,&EmuPeripheralCrateConfig::EnableJtagWriteALCT288fp    ,"EnableALCT288fp",XDAQ_NS_URI);
  xoap::bind(this,&EmuPeripheralCrateConfig::EnableJtagWriteALCT384      ,"EnableALCT384",XDAQ_NS_URI);
  xoap::bind(this,&EmuPeripheralCrateConfig::EnableJtagWriteALCT384Mirror,"EnableALCT384Mirror",XDAQ_NS_URI);
  xoap::bind(this,&EmuPeripheralCrateConfig::EnableJtagWriteALCT576Mirror,"EnableALCT576Mirror",XDAQ_NS_URI);
  xoap::bind(this,&EmuPeripheralCrateConfig::EnableJtagWriteALCT672      ,"EnableALCT672",XDAQ_NS_URI);

  // SOAP for Monitor controll
//  xoap::bind(this,&EmuPeripheralCrateConfig::MonitorStart      ,"MonitorStart",XDAQ_NS_URI);
//  xoap::bind(this,&EmuPeripheralCrateConfig::MonitorStop      ,"MonitorStop",XDAQ_NS_URI);
    xgi::bind(this,&EmuPeripheralCrateConfig::MonitorStart      ,"MonitorStart");
    xgi::bind(this,&EmuPeripheralCrateConfig::MonitorStop      ,"MonitorStop");
    xoap::bind(this,&EmuPeripheralCrateConfig::onFastLoop      ,"FastLoop", XDAQ_NS_URI);
    xoap::bind(this,&EmuPeripheralCrateConfig::onSlowLoop      ,"SlowLoop", XDAQ_NS_URI);
    xoap::bind(this,&EmuPeripheralCrateConfig::onExtraLoop      ,"ExtraLoop", XDAQ_NS_URI);
  //
  //-------------------------------------------------------------
  // fsm_ is defined in EmuApplication
  //-------------------------------------------------------------
  fsm_.addState('H', "Halted",     this, &EmuPeripheralCrateConfig::stateChanged);
  fsm_.addState('C', "Configured", this, &EmuPeripheralCrateConfig::stateChanged);
  fsm_.addState('E', "Enabled",    this, &EmuPeripheralCrateConfig::stateChanged);
  //
  fsm_.addStateTransition('H', 'C', "Configure", this, &EmuPeripheralCrateConfig::configureAction);
  fsm_.addStateTransition('C', 'C', "Configure", this, &EmuPeripheralCrateConfig::reConfigureAction);
  fsm_.addStateTransition('C', 'E', "Enable",    this, &EmuPeripheralCrateConfig::enableAction);
  fsm_.addStateTransition('E', 'E', "Enable",    this, &EmuPeripheralCrateConfig::enableAction);
  fsm_.addStateTransition('E', 'C', "Disable",   this, &EmuPeripheralCrateConfig::disableAction);
  fsm_.addStateTransition('C', 'H', "Halt",      this, &EmuPeripheralCrateConfig::haltAction);
  fsm_.addStateTransition('E', 'H', "Halt",      this, &EmuPeripheralCrateConfig::haltAction);
  fsm_.addStateTransition('H', 'H', "Halt",      this, &EmuPeripheralCrateConfig::haltAction);
  //
  fsm_.setInitialState('H');
  fsm_.reset();    
  //
  // state_ is defined in EmuApplication
  state_ = fsm_.getStateName(fsm_.getCurrentState());
  //
  //----------------------------
  // initialize variables
  //----------------------------
  myParameter_ =  0;
  //
  xmlFile_ = "config.xml" ;
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
  //
  MPCBoardID_ = "-2";
  CCBBoardID_ = "-2";
  ControllerBoardID_ = "-2";
  for (int i=0; i<9; i++) 
   {  DMBBoardID_[i] = "-2" ; 
      TMBBoardID_[i] = "-2" ; 
      RATBoardID_[i] = "-2" ;
   }
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
  }
  CrateTestsOutput << "Crate Tests output:" << std::endl;
  //
  this->getApplicationInfoSpace()->fireItemAvailable("runNumber", &runNumber_);
  this->getApplicationInfoSpace()->fireItemAvailable("xmlFileName", &xmlFile_);
  this->getApplicationInfoSpace()->fireItemAvailable("CalibrationState", &CalibrationState_);
  this->getApplicationInfoSpace()->fireItemAvailable("Calibtype", &CalibType_);
  this->getApplicationInfoSpace()->fireItemAvailable("Calibnumber", &CalibNumber_);
  
  // for XMAS minotoring:

  Monitor_On_ = false;
  Monitor_Ready_ = false;

  global_config_states[0]="UnConfiged";
  global_config_states[1]="Configuring";
  global_config_states[2]="Configed";
  global_run_states[0]="Halted";
  global_run_states[1]="Enabled";
  current_config_state_=0;
  current_run_state_=0;
  total_crates_=0;
  this_crate_no_=0;

  brddb= new BoardsDB();

  parsed=0;
}

void EmuPeripheralCrateConfig::MonitorStart(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
     if(!Monitor_On_)
     {
         if(!Monitor_Ready_)
         {
             CreateEmuInfospace();
             Monitor_Ready_=true;
         }
         PCsendCommand("MonitorStart","EmuPeripheralCrateBroadcast");
         Monitor_On_=true;
         std::cout<< "Monitor Started" << std::endl;
     }
     this->Default(in,out);
}

void EmuPeripheralCrateConfig::MonitorStop(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
     if(Monitor_On_)
     {
         PCsendCommand("MonitorStop","EmuPeripheralCrateBroadcast");
         Monitor_On_=false;
         std::cout << "Monitor stopped" << std::endl;
     }
     this->Default(in,out);
}

xoap::MessageReference EmuPeripheralCrateConfig::onFastLoop (xoap::MessageReference message) 
  throw (xoap::exception::Exception) 
{
  // std::cout << "SOAP Fast Loop" << std::endl;
  PublishEmuInfospace(1);
  return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateConfig::onSlowLoop (xoap::MessageReference message) 
  throw (xoap::exception::Exception) 
{
  // std::cout << "SOAP Slow Loop" << std::endl;
  PublishEmuInfospace(2);
  return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateConfig::onExtraLoop (xoap::MessageReference message) 
  throw (xoap::exception::Exception) 
{
  // std::cout << "SOAP Extra Loop" << std::endl;
  PublishEmuInfospace(3);
  return createReply(message);
}

void EmuPeripheralCrateConfig::CreateEmuInfospace()
{
     if(!parsed) ParsingXML();
     if(total_crates_<=0) return;

        //Create infospaces for monitoring
        monitorables_.clear();
        for ( unsigned int i = 0; i < crateVector.size(); i++ )
        {
                toolbox::net::URN urn = this->createQualifiedInfoSpace("EMu_"+(crateVector[i]->GetLabel())+"_PCrate");
                std::cout << "Crate " << i << " " << urn.toString() << std::endl;
                monitorables_.push_back(urn.toString());
                xdata::InfoSpace * is = xdata::getInfoSpaceFactory()->get(urn.toString());

            // for CCB, MPC, TTC etc.
                is->fireItemAvailable("TTC_BRSTR",new xdata::UnsignedShort(0));
                is->fireItemAvailable("TTC_DTSTR",new xdata::UnsignedShort(0));
                is->fireItemAvailable("QPLL",new xdata::String("uninitialized"));
                is->fireItemAvailable("CCBstatus",new xdata::String("uninitialized"));
                is->fireItemAvailable("MPCstatus",new xdata::String("uninitialized"));

            // for TMB fast counters
                is->fireItemAvailable("TMBcounter",new xdata::Vector<xdata::UnsignedInteger32>);
                is->fireItemAvailable("TMBtime",new xdata::TimeVal);

            // for DMB fast counters
                is->fireItemAvailable("DMBcounter",new xdata::Vector<xdata::UnsignedShort>);
                is->fireItemAvailable("DMBtime",new xdata::TimeVal);

            // for TMB temps, voltages

            // for DMB temps, voltages

         }
     Monitor_Ready_=true;
}

void EmuPeripheralCrateConfig::PublishEmuInfospace(int cycle)
{
   //   cycle = 1  fast loop (e.g. TMB/DMB counters)
   //           2  slow loop (e.g. temps/voltages)
   //           3  extra loop (e.g. CCB MPC TTC status)

      Crate * now_crate;
      xdata::InfoSpace * is;
      char buf[8000];
      // xdata::UnsignedInteger32 *counter32;
      xdata::UnsignedShort *counter16;
      xdata::String *status;
      unsigned long *buf4;
      unsigned short *buf2;

      buf2=(unsigned short *)buf;
      buf4=(unsigned long *)buf;
      if(cycle<1 || cycle>3) return;
      if(total_crates_<=0) return;
      //update infospaces
      for ( unsigned int i = 0; i < crateVector.size(); i++ )
      {
          now_crate=crateVector[i];
          if(now_crate && now_crate->IsAlive()) 
          {
             if(!(now_crate->vmeController()->SelfTest())) continue;
             is = xdata::getInfoSpaceFactory()->get(monitorables_[i]);
             if(cycle==3)
             {  
                now_crate-> MonitorCCB(cycle, buf);
                if(buf[0])  
                {   // buf[0]==0 means no good data back

                   // CCB & TTC counters
                   counter16 = dynamic_cast<xdata::UnsignedShort *>(is->find("TTC_BRSTR"));
                   *counter16 = buf2[10];;
                   counter16 = dynamic_cast<xdata::UnsignedShort *>(is->find("TTC_DTSTR"));
                   *counter16 = buf2[11];;
                   std::stringstream id1, id2, id0;


                   // add this one to help the TTC group's global clock scan
                   id0 <<(((buf2[3]&0x4000) || (buf2[3]&0x2000)==0)?"Unlocked":"Locked");
                   status = dynamic_cast<xdata::String *>(is->find("QPLL"));
                   *status = id0.str();

                   // CCB status
                   id1 << "CCB mode: " << ((buf2[1]&0x1)?"DLOG":"FPGA");
		   id1 << ", TTCrx: " << ((buf2[3]&0x2000)?"Ready":"NotReady");
                   id1 << ", QPLL: " << (((buf2[3]&0x4000) || (buf2[3]&0x2000)==0)?"Unlocked":"Locked");

                   status = dynamic_cast<xdata::String *>(is->find("CCBstatus"));
                   *status = id1.str();

                   // MPC status
                   id2 << "MPC mode: " << ((buf2[12]&0x1)?"Test":"Trig");
		   id2 << ", Transimitter: " << ((buf2[12]&0x0200)?"On":"Off");
                   id2 << ", Serializer: " << ((buf2[12]&0x4000)?"On":"Off");
                   id2 << ", PRBS: " << ((buf2[12]&0x8000)?"Test":"Norm");

                   status = dynamic_cast<xdata::String *>(is->find("MPCstatus"));
                   *status = id2.str();
                }
             }
             else if( cycle==1)
             {
                now_crate-> MonitorTMB(cycle, buf);
                if(buf[0])
                {
                  // std::cout << "TMB counters will be here" << std::endl;
                }
                now_crate-> MonitorDMB(cycle, buf);
                if(buf[0])
                {
                  // std::cout << "DMB counters will be here" << std::endl;
                }
             }
               // is->fireGroupChanged(names, this);
          }
      }
}

void EmuPeripheralCrateConfig::MainPage(xgi::Input * in, xgi::Output * out ) 
{
  //
  std::string LoggerName = getApplicationLogger().getName() ;
  std::cout << "Name of Logger is " <<  LoggerName <<std::endl;
  //
  LOG4CPLUS_INFO(getApplicationLogger(), "EmuPeripheralCrate ready");
  //
  MyHeader(in,out,"EmuPeripheralCrateConfig");

  if(!parsed) ParsingXML();

  *out << "Total Crates : ";
  *out << total_crates_ << cgicc::br() << std::endl ;
  unsigned int active_crates=0;
  for(unsigned i=0; i<crateVector.size(); i++)
     if(crateVector[i]->IsAlive()) active_crates++;
  if( active_crates <= total_crates_) 
     *out << cgicc::b(" Active Crates: ") << active_crates << cgicc::br() << std::endl ;
 
 // Crate Status
  *out << cgicc::span().set("style","color:blue");
  *out << cgicc::b(cgicc::i("System Status: ")) ;
  *out << global_config_states[current_config_state_] << "  ";
  *out << global_run_states[current_run_state_]<< cgicc::br() << std::endl ;
  *out << cgicc::span() << std::endl ;
  //
  *out << cgicc::table().set("border","0");
    //
  *out << cgicc::td();
  std::string CrateConfigureAll = toolbox::toString("/%s/ConfigAllCrates",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CrateConfigureAll) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Write FLASH All Crates") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();

  *out << cgicc::td();
  std::string FastConfigureAll = toolbox::toString("/%s/FastConfigCrates",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",FastConfigureAll) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Configure All Crates") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();

  *out << cgicc::td();
  std::string CheckCrates = toolbox::toString("/%s/CheckCrates",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CheckCrates) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Check Crate Controllers") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();

  *out << cgicc::td();
  std::string CheckCratesConfiguration = toolbox::toString("/%s/CheckCratesConfiguration",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CheckCratesConfiguration) << std::endl ;
  if (all_crates_ok == 1) {
    *out << cgicc::input().set("type","submit").set("value","Check configuration of crates").set("style","color:green") << std::endl ;
  } else if (all_crates_ok == 0) {
    *out << cgicc::input().set("type","submit").set("value","Check configuration of crates").set("style","color:red") << std::endl ;
  } else if (all_crates_ok == -1) {
    *out << cgicc::input().set("type","submit").set("value","Check configuration of crates").set("style","color:blue") << std::endl ;
  }
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();

  *out << cgicc::table();
  //
  int initial_crate = current_crate_;
  //
  if (all_crates_ok == 0) {
    //
    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      //
      SetCurrentCrate(crate_number);
      //
      *out << crateVector[crate_number]->GetLabel() << std::endl;
      //
      if (crate_check_ok[current_crate_] == 0) {
	//
	*out << cgicc::br() << cgicc::span().set("style","color:red");
	//
	bool alct_ok = true;
	bool tmb_ok = true;
	bool dmb_ok = true;
	//
	for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
	  if (alct_check_ok[current_crate_][chamber_index] == 0) alct_ok = false;
	  if (tmb_check_ok[current_crate_][chamber_index] == 0)  tmb_ok = false;
	  if (dmb_check_ok[current_crate_][chamber_index] == 0)  dmb_ok = false;
	}
	//
	if (!alct_ok) {
	  //
	  *out << "Config problems for ALCT: " ;
	  for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) 
	    if (alct_check_ok[current_crate_][chamber_index] == 0) 
	      *out << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetLabel().c_str() << ", ";
	  //
	  *out << cgicc::br() << std::endl ;
	}
	//
	if (!tmb_ok) {
	  //
	  *out << "Config problems for TMB: " ;
	  for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) 
	    if (tmb_check_ok[current_crate_][chamber_index] == 0) 
	      *out << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetLabel().c_str() << ", ";
	  //
	  *out << cgicc::br() << std::endl ;
	}
	//
	if (!dmb_ok) {
	  //
	  *out << "Config problems for DMB: " ;
	  for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) 
	    if (dmb_check_ok[current_crate_][chamber_index] == 0) 
	      *out << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetLabel().c_str() << ", ";
	  //
	  *out << cgicc::br() << std::endl ;
	}
	//
      } else if (crate_check_ok[current_crate_] == 1) {
	//
	*out << cgicc::span().set("style","color:green");
	*out << " OK" << cgicc::br();
      } else if (crate_check_ok[current_crate_] == -1) {
	//
	*out << cgicc::span().set("style","color:blue");
	*out << " Not checked" << cgicc::br();
      }
      *out << cgicc::span() << std::endl ;
    }
  }
  //
  SetCurrentCrate(initial_crate);
  //
  *out << cgicc::br() << std::endl ;
  if(Monitor_On_)
  {
     *out << cgicc::span().set("style","color:green");
     *out << cgicc::b(cgicc::i("Monitor Status: On")) << cgicc::span() << std::endl ;
     std::string TurnOffMonitor = toolbox::toString("/%s/MonitorStop",getApplicationDescriptor()->getURN().c_str());
     *out << cgicc::form().set("method","GET").set("action",TurnOffMonitor) << std::endl ;
     *out << cgicc::input().set("type","submit").set("value","Turn Off Monitor") << std::endl ;
     *out << cgicc::form() << std::endl ;
  } else
  {
     *out << cgicc::b(cgicc::i("Monitor Status: Off")) << std::endl ;
     std::string TurnOnMonitor = toolbox::toString("/%s/MonitorStart",getApplicationDescriptor()->getURN().c_str());
     *out << cgicc::form().set("method","GET").set("action",TurnOnMonitor) << std::endl ;
     *out << cgicc::input().set("type","submit").set("value","Turn On Monitor") << std::endl ;
     *out << cgicc::form() << std::endl ;
  }
  //
  *out << cgicc::br() << cgicc::hr() <<std::endl;

  *out << cgicc::h2("Crate Utilities")<< std::endl;
  //
  *out << cgicc::span().set("style","color:blue");
  *out << cgicc::b(cgicc::i("Current Crate : ")) ;
  *out << ThisCrateID_ << cgicc::span() << std::endl ;
  //
  *out << cgicc::br();
  //

  // Begin select crate
	// Config listbox
	*out << form().set("action",
			"/" + getApplicationDescriptor()->getURN() + "/CrateSelection") << endl;

	int n_keys = crateVector.size();

	*out << "Choose Crate: " << endl;
	*out << cgicc::select().set("name", "runtype") << endl;

	int selected_index = this_crate_no_;
        std::string CrateName;
	for (int i = 0; i < n_keys; ++i) {
                if(crateVector[i]->IsAlive())
                   CrateName = crateVector[i]->GetLabel();
                else
                   CrateName = crateVector[i]->GetLabel() + " NG";
		if (i == selected_index) {
			*out << option()
					.set("value", CrateName)
					.set("selected", "");
		} else {
			*out << option()
					.set("value", CrateName);
		}
		*out << CrateName << option() << endl;
	}

	*out << cgicc::select() << endl;

	*out << input().set("type", "submit")
			.set("name", "command")
			.set("value", "CrateSelection") << endl;
	*out << form() << endl;
     
  //End select crate
 
  *out << cgicc::br()<< std::endl;
  std::cout << "Main Page: "<< std::dec << total_crates_ << " Crates" << std::endl;
  //
  if (tmbVector.size()>0 || dmbVector.size()>0) {
    //
      //
      // Crate Configuration
      //
      std::string CrateConfiguration = toolbox::toString("/%s/CrateConfiguration",getApplicationDescriptor()->getURN().c_str());
      *out << cgicc::a("[Crate Configuration]").set("href",CrateConfiguration) << endl;
      //
      std::string CrateTests = toolbox::toString("/%s/CrateTests",getApplicationDescriptor()->getURN().c_str());
      *out << cgicc::a("[Crate Tests]").set("href",CrateTests) << endl;
      //
      std::string CalibrationRuns = toolbox::toString("/%s/CalibrationRuns",getApplicationDescriptor()->getURN().c_str());
      *out << cgicc::a("[Calibration Runs]").set("href",CalibrationRuns) << endl;
      //
    *out << cgicc::br() << cgicc::br() << cgicc::table().set("border","0");
    //
    *out << cgicc::td();
    std::string FastConfigOne = toolbox::toString("/%s/FastConfigOne",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",FastConfigOne) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Configure Crate") << std::endl ;
    *out << cgicc::form() << std::endl ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    std::string InitSystem = toolbox::toString("/%s/InitSystem",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",InitSystem) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Write FLASH to Crate") << std::endl ;
    *out << cgicc::form() << std::endl ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    std::string CrateDumpConfiguration = toolbox::toString("/%s/CrateDumpConfiguration",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",CrateDumpConfiguration) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Dump Crate Info") << std::endl ;
    *out << cgicc::form() << std::endl ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    std::string CheckCrateConfiguration = toolbox::toString("/%s/CheckCrateConfiguration",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",CheckCrateConfiguration) << std::endl ;
    if (crate_check_ok[current_crate_] == 1) {
      *out << cgicc::input().set("type","submit").set("value","Check crate configuration").set("style","color:green") << std::endl ;
    } else if (crate_check_ok[current_crate_] == 0) {
      *out << cgicc::input().set("type","submit").set("value","Check crate configuration").set("style","color:red") << std::endl ;
    } else if (crate_check_ok[current_crate_] == -1) {
      *out << cgicc::input().set("type","submit").set("value","Check crate configuration").set("style","color:blue") << std::endl ;
    }
    *out << cgicc::form() << std::endl ;
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    if (crate_check_ok[current_crate_] == 0) {
      bool alct_ok = true;
      bool tmb_ok = true;
      bool dmb_ok = true;
      //
      for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
	if (alct_check_ok[current_crate_][chamber_index] == 0) alct_ok = false;
	if (tmb_check_ok[current_crate_][chamber_index] == 0)  tmb_ok = false;
	if (dmb_check_ok[current_crate_][chamber_index] == 0)  dmb_ok = false;

      }
      //
      *out << cgicc::span().set("style","color:green");
      if (!alct_ok) {
	//
	*out << cgicc::span() << std::endl ;
	*out << cgicc::span().set("style","color:red");
	//
	*out << "ALCT: " ;
	for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
	  if (alct_check_ok[current_crate_][chamber_index] == 0) {
	    *out << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetLabel().c_str() << ", ";
	  }
	}
	*out << "... not OK" << cgicc::br() << std::endl ;
      } else {
	*out << "ALCT OK " << cgicc::br() << std::endl;
      }
      *out << cgicc::span() << std::endl ;
      //
      *out << cgicc::span().set("style","color:green");
      if (!tmb_ok) {
	//
	*out << cgicc::span() << std::endl ;
	*out << cgicc::span().set("style","color:red");
	//
	*out << "TMB: " ;
	for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
	  if (tmb_check_ok[current_crate_][chamber_index] == 0) {
	    *out << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetLabel().c_str() << ", ";
	  }
	}
	*out << "... not OK" << cgicc::br() << std::endl ;
      } else {
	*out << "TMB OK " << cgicc::br() << std::endl;
      }
      *out << cgicc::span() << std::endl ;
      //
      *out << cgicc::span().set("style","color:green");
      if (!dmb_ok) {
	//
	*out << cgicc::span() << std::endl ;
	*out << cgicc::span().set("style","color:red");
	//
	*out << "DMB: " ;
	for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
	  if (dmb_check_ok[current_crate_][chamber_index] == 0) {
	    *out << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetLabel().c_str() << ", ";
	  }
	}
	*out << "... not OK" << cgicc::br() << std::endl ;
      } else {
	*out << "DMB OK " << cgicc::br() << std::endl;
      }
      //
      *out << cgicc::span() << std::endl ;
    }
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial; background-color:yellow");
    *out << std::endl;
    *out << cgicc::legend((("Monitoring"))).set("style","color:blue") ;
    //
    *out << cgicc::table().set("border","0");
    //
    *out << cgicc::td();
    //
    std::string ResetTMBC = toolbox::toString("/%s/ResetAllCounters",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",ResetTMBC) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Reset TMB Counters").set("name","ResetTMBC") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    //
    *out << cgicc::td();
    //
    std::string CrateTMBCounters = toolbox::toString("/%s/CrateTMBCountersRight",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",CrateTMBCounters).set("target","_blank") << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","TMB counters").set("name", thisCrate->GetLabel()) << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    //
    *out << cgicc::td();
    //
    std::string CrateDMBCounters = toolbox::toString("/%s/CrateDMBCounters",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",CrateDMBCounters).set("target","_blank") << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","DMB counters").set("name",thisCrate->GetLabel()) << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::td();
    //
    std::string CrateStatus = toolbox::toString("/%s/CrateStatus",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",CrateStatus).set("target","_blank") << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Crate Status").set("name","CrateStatus") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    std::string Operator = toolbox::toString("/%s/Operator",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",Operator) << std::endl ;
    *out << cgicc::input().set("type","text").set("name","Operator").set("size","20").set("value",Operator_) << std::endl ;
    *out << "Operator" << std::endl;
    *out << cgicc::form() << std::endl ;
    //
    std::string RunNumber = toolbox::toString("/%s/RunNumber",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",RunNumber) << std::endl ;
    *out << cgicc::input().set("type","text").set("name","RunNumber").set("size","20").set("value",RunNumber_) << std::endl ;
    *out << "RunNumber" << std::endl;
    *out << cgicc::form() << std::endl ;
    //
    std::string LogOutput = toolbox::toString("/%s/LogOutput",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",LogOutput) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Log all output").set("name","LogOutput") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    std::string LogTestSummary = toolbox::toString("/%s/LogTestSummary",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",LogTestSummary) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Log Test Summary").set("name","LogTestSummary") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::fieldset();
    //
    *out << std::endl;
    //
  }

  *out << cgicc::br() << cgicc::br() << std::endl; 
  *out << cgicc::b(cgicc::i("Configuration filename : ")) ;
  *out << xmlFile_.toString() << cgicc::br() << std::endl ;
  //
  std::string DefineConfiguration = toolbox::toString("/%s/DefineConfiguration",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::a("[Change Configuration File]").set("href",DefineConfiguration) << endl;
  *out << cgicc::br();
  //
}

// 
void EmuPeripheralCrateConfig::MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) 
  throw (xgi::exception::Exception) {
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
void EmuPeripheralCrateConfig::Default(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<"MainPage"<<"\">" <<endl;
}
//
/////////////////////////////////////////////////////////////////////
// SOAP Callback  
/////////////////////////////////////////////////////////////////////
//
xoap::MessageReference EmuPeripheralCrateConfig::onCalibration(xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  LOG4CPLUS_INFO(getApplicationLogger(), "Calibration");
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
  //
  //CalibrationState_ = setting;
  //
  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateConfig::onConfigure (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  std::cout << "SOAP Configure" << std::endl;
  //
  fireEvent("Configure");
  //
  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateConfig::onEnable (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  std::cout << "SOAP Enable" << std::endl;
  //
  current_run_state_ = 1;
  fireEvent("Enable");
  //
  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateConfig::onDisable (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  std::cout << "SOAP Disable" << std::endl;
  //
  current_run_state_ = 0;
  fireEvent("Disable");
  //
  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateConfig::onHalt (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  std::cout << "SOAP Halt" << std::endl;
  //
  fireEvent("Halt");
  //
  return createReply(message);
}
//
void EmuPeripheralCrateConfig::configureAction(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {
  //
  printf(" LSD: comment out ConfigureInit for now \n");
  //
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
void EmuPeripheralCrateConfig::configureFail(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {
  //
  // currently do nothing
  //
  LOG4CPLUS_INFO(getApplicationLogger(), "Failed");
  //
}
//
void EmuPeripheralCrateConfig::reConfigureAction(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {
  //
  // MyController->configure();
  //
  LOG4CPLUS_INFO(getApplicationLogger(), "reConfigure");
  std::cout << "reConfigure" << std::endl ;
  //
}
//
void EmuPeripheralCrateConfig::enableAction(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {
  //
  // currently do nothing
  //
  //MyController->init();
  //
  //MyController->configure();
  //
  std::cout << "Received Message Enable" << std::endl ;
  LOG4CPLUS_INFO(getApplicationLogger(), "Received Message Enable");
}
//
void EmuPeripheralCrateConfig::disableAction(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {
  //
  // currently do nothing
  //
  std::cout << "Received Message Disable" << std::endl ;
  LOG4CPLUS_INFO(getApplicationLogger(), "Received Message Disable");
}  
//
void EmuPeripheralCrateConfig::haltAction(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {
  //
  // currently do nothing
  // 
  std::cout << "Received Message Halt" << std::endl ;
  LOG4CPLUS_INFO(getApplicationLogger(), "Received Message Halt");
}  

void EmuPeripheralCrateConfig::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
  throw (toolbox::fsm::exception::Exception) {
  EmuApplication::stateChanged(fsm);
}

void EmuPeripheralCrateConfig::actionPerformed (xdata::Event& e) {
  //
  if (e.type() == "ItemRetrieveEvent") {
    xdata::InfoSpace * is = xdata::InfoSpace::get("urn:xdaq-monitorable:EmuPeripheralCrateData");
    is->lock();
    std::string item = dynamic_cast<xdata::ItemRetrieveEvent&>(e).itemName();
    if ( item == "myCounter")
      myCounter_ = "meydev:Done" ;
    std::cout << "Getting myCounter" << std::endl;
    is->unlock();
  }
}

  void EmuPeripheralCrateConfig::CrateSelection(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
     cgicc::Cgicc cgi(in);

     string value = cgi.getElement("runtype")->getValue(); 
     std::cout << "Select Crate " << value << endl;
     if(!value.empty())
     {
        ThisCrateID_=value;
        for(unsigned i=0; i< crateVector.size(); i++)
        {
           if(value==crateVector[i]->GetLabel()) this_crate_no_=i;
        }
        SetCurrentCrate(this_crate_no_);
     }
     this->Default(in,out);
  }

  void EmuPeripheralCrateConfig::ConfigAllCrates(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
     std::cout << "Button: ConfigAllCrates" << std::endl;
     ConfigureInit();
//     fireEvent("Configure");
     this->Default(in,out);
  }

  void EmuPeripheralCrateConfig::FastConfigCrates(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
     std::cout << "Button: FastConfigCrates" << std::endl;
     ConfigureInit(2);
//     fireEvent("Configure");
     this->Default(in,out);
  }

  void EmuPeripheralCrateConfig::FastConfigOne(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
     thisCrate->configure(2);
     this->Default(in,out);
  }

///////////////////////////////////////////////////////////////////////////
  void EmuPeripheralCrateConfig::setRawConfFile(xgi::Input * in, xgi::Output * out ) 
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

  void EmuPeripheralCrateConfig::ConfigureInit(int c)
  {

    if(!parsed) ParsingXML();
    
    if( MyController )
      {
        current_config_state_=1;
	MyController->configure(c);
        current_config_state_=2;
      }
    //
  }

  bool EmuPeripheralCrateConfig::ParsingXML(){
    //
    LOG4CPLUS_INFO(getApplicationLogger(),"Parsing Configuration XML");
    //
    // Check if filename exists
    //
    if(xmlFile_.toString().find("http") == string::npos) 
    {
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
    //cout <<"Start Parsing"<<endl;
    if ( MyController != 0 ) {
      LOG4CPLUS_INFO(getApplicationLogger(), "Delete existing controller");
      delete MyController ;
    }
    //
    MyController = new EmuController();

    MyController->SetConfFile(xmlFile_.toString().c_str());
    MyController->init();
    MyController->NotInDCS();
    //
    emuEndcap_ = MyController->GetEmuEndcap();
    if(!emuEndcap_) return false;
    crateVector = emuEndcap_->crates();
    //
    total_crates_=crateVector.size();
    if(total_crates_<=0) return false;
    this_crate_no_=0;

    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      //
      SetCurrentCrate(crate_number);
      for(int i=0; i<9;i++) {
	OutputDMBTests[i][current_crate_] << "DMB-CFEB Tests " 
					  << thisCrate->GetChamber(dmbVector[i]->slot())->GetLabel().c_str() 
					  << " output:" << std::endl;
	OutputTMBTests[i][current_crate_] << "TMB-RAT Tests " 
					  << thisCrate->GetChamber(tmbVector[i]->slot())->GetLabel().c_str() 
					  << " output:" << std::endl;
	ChamberTestsOutput[i][current_crate_] << "Chamber-Crate Phases " 
					      << thisCrate->GetChamber(tmbVector[i]->slot())->GetLabel().c_str() 
					      << " output:" << std::endl;
      }
    }
    //
    SetCurrentCrate(this_crate_no_);

    std::cout << "Parser Done" << std::endl ;
    //
    parsed=1;
    return true;
  }

  void EmuPeripheralCrateConfig::SetCurrentCrate(int cr)
  {  
    if(total_crates_<=0) return;
    thisCrate = crateVector[cr];

    if ( ! thisCrate ) {
      std::cout << "Crate doesn't exist" << std::endl;
      assert(thisCrate);
    }
    
    ThisCrateID_=thisCrate->GetLabel();
    thisCCB = thisCrate->ccb();
    thisMPC = thisCrate->mpc();
    tmbVector = thisCrate->tmbs();
    dmbVector = thisCrate->daqmbs();
    chamberVector = thisCrate->chambers();
        
    tmbTestVector = InitTMBTests(thisCrate);

    current_crate_ = cr;

  }

  void EmuPeripheralCrateConfig::CheckCrates(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
  {  
    std::cout << "Button: Check Crates" << std::endl;
    if(total_crates_<=0) return;
    bool cr;
    for(unsigned i=0; i< crateVector.size(); i++)
    {
        cr = (crateVector[i]->vmeController()->SelfTest()) && (crateVector[i]->vmeController()->exist(13));
        crateVector[i]->SetLife( cr );
        if(!cr) std::cout << "Exclude Crate " << crateVector[i]->GetLabel() << std::endl;
    }

    this->Default(in, out);
  }

void EmuPeripheralCrateConfig::CheckCratesConfiguration(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //
  std::cout << "Button:  Check Configuration of All Active Crates" << std::endl;
  //
  if(total_crates_<=0) return;
  //
  all_crates_ok = 1;
  //
  for(unsigned i=0; i< crateVector.size(); i++) {
    //
    if ( crateVector[i]->IsAlive() ) {
      //
      SetCurrentCrate(i);	
      //
      CheckPeripheralCrateConfiguration();
      //
      all_crates_ok &= crate_check_ok[i];
    }
  }
  //
  this->Default(in, out);
}

void EmuPeripheralCrateConfig::CheckCrateConfiguration(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //  
  std::cout << "Button: Check Crate Configuration" << std::endl;
  //
  CheckPeripheralCrateConfiguration();
  //
  this->Default(in, out);
}

  // This one came from CrateUtils class which no longer exist. 
  // Better put into another class. Leave it here for now. 
  // Liu Dec.25, 2007
  std::vector<TMBTester> EmuPeripheralCrateConfig::InitTMBTests(Crate *MyCrate_)
  {
    std::vector<TMB *>       tmbVector = MyCrate_->tmbs();
    CCB * MyCCB_ = MyCrate_->ccb();
    std::vector<TMBTester>   result;
  //
    for( unsigned i=0; i< tmbVector.size(); i++) {
    //
       TMBTester tmp;
       tmp.setTMB(tmbVector[i]);
       tmp.setCCB(MyCCB_);
       tmp.setRAT(tmbVector[i]->getRAT());
       result.push_back(tmp);
    //
    }
  //
    return result;
  }
//
// Another method which would be better in another class... let's make it work, first....
void EmuPeripheralCrateConfig::CheckPeripheralCrateConfiguration() {
  //
  std::cout << "Configuration check for " << thisCrate->GetLabel() << std::endl;
  //
  std::cout << " .... TMBs received hard resets... "; 
  for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
    std::cout << std::dec << tmbVector[tmb]->ReadRegister(0xE8) << " ";
  }
  std::cout << " seconds ago" << std::endl;
  //
  crate_check_ok[current_crate_] = 1;
  //
  for (unsigned int chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
    //	
    Chamber * thisChamber     = chamberVector[chamber_index];
    TMB * thisTMB             = tmbVector[chamber_index];
    ALCTController * thisALCT = thisTMB->alctController();
    DAQMB * thisDMB           = dmbVector[chamber_index];
    //
    std::cout << "Configuration check for " << thisCrate->GetLabel() << ", " << (thisChamber->GetLabel()).c_str() << std::endl;
    //
    thisTMB->CheckTMBConfiguration();
    tmb_check_ok[current_crate_][chamber_index]  = (int) thisTMB->GetTMBConfigurationStatus();
    //
    thisALCT->CheckALCTConfiguration();
    alct_check_ok[current_crate_][chamber_index] = (int) thisALCT->GetALCTConfigurationStatus();
    //
    dmb_check_ok[current_crate_][chamber_index]  = (int) thisDMB->checkDAQMBXMLValues();
    //
    crate_check_ok[current_crate_] &= tmb_check_ok[current_crate_][chamber_index];
    crate_check_ok[current_crate_] &= alct_check_ok[current_crate_][chamber_index];
    crate_check_ok[current_crate_] &= dmb_check_ok[current_crate_][chamber_index];
    //
  }
  //
  return;
}
//

  void EmuPeripheralCrateConfig::setConfFile(xgi::Input * in, xgi::Output * out ) 
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
	std::cout << "GetFiles string" << endl ;
	//
	if(file != cgi.getFiles().end()) (*file).writeToStream(std::cout);
	//
	string XMLname = cgi["xmlFileName"]->getValue() ; 
	//
	std::cout << XMLname  << endl ;
	//
	xmlFile_ = XMLname ;
	//
 	ParsingXML();
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

  void EmuPeripheralCrateConfig::UploadConfFile(xgi::Input * in, xgi::Output * out ) 
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
	ParsingXML();
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


//////////////////////////////////////////////////////////////////////////
// Layouts of html pages
//////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::CrateTests(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  MyHeader(in,out,"CrateTests");
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << cgicc::legend("Crate Tests").set("style","color:blue") ;
  //
  std::string TmbMPCTest = toolbox::toString("/%s/TmbMPCTest",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TmbMPCTest) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Crate TMB/MPC test") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  if (myCrateTest.GetMpcTMBTestResult() == -1 ) {
    *out << "Not tested yet" << std::endl;
  } else if (myCrateTest.GetMpcTMBTestResult() == 0 ) {
    *out << "Failed" <<std::endl;
  } else {
    *out << "Passed" <<std::endl;
  }
  //
  *out<< cgicc::br() ;
  *out<< cgicc::br() ;
  //
  *out << cgicc::pre();
  *out << "If MPC switch S2-1/2=on/off, the following scan will pass for all values (CMS running)" << std::endl;
  *out << "If MPC switch S2-1/2=off/on, the following scan will determine the MPC safe window" << std::endl;
  *out << cgicc::pre();
  //
  std::string MPCSafeWindowScan = toolbox::toString("/%s/MPCSafeWindowScan",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",MPCSafeWindowScan) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","MPC Safe Window Scan") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::fieldset();
  //
  *out << cgicc::form().set("method","GET") << std::endl ;
  *out << cgicc::textarea().set("name","CrateTestsOutput").set("WRAP","OFF").set("rows","20").set("cols","100");
  *out << CrateTestsOutput.str() << endl ;
  *out << cgicc::textarea();
  *out << cgicc::form() << std::endl ;
  //
  std::string LogCrateTestsOutput = toolbox::toString("/%s/LogCrateTestsOutput",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LogCrateTestsOutput) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Log output").set("name","LogCrateTestsOutput") << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Clear").set("name","ClearCrateTestsOutput") << std::endl ;
  *out << cgicc::form() << std::endl ;
}
//
void EmuPeripheralCrateConfig::DefineConfiguration(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  MyHeader(in,out,"Change Configuration File");
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << std::endl;
  //
  *out << cgicc::legend("Upload Configuration...").set("style","color:blue") << cgicc::p() << std::endl ;
  //
  std::string method = toolbox::toString("/%s/setConfFile",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","POST").set("action",method) << std::endl ;
  *out << cgicc::input().set("type","text").set("name","xmlFilename").set("size","90").set("ENCTYPE","multipart/form-data").set("value",xmlFile_) << std::endl;
  *out << cgicc::input().set("type","submit").set("value","Set configuration file local") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  // Upload file...
  //
  std::string methodUpload = toolbox::toString("/%s/UploadConfFile",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","POST").set("enctype","multipart/form-data").set("action",methodUpload) << std::endl ;
  *out << cgicc::input().set("type","file").set("name","xmlFilenameUpload").set("size","90") << std::endl;
  *out << cgicc::input().set("type","submit").set("value","Send") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  *out << std::endl;
  //
  *out << cgicc::fieldset();
  //
  *out << std::endl;
  //
}
//
void EmuPeripheralCrateConfig::CalibrationRuns(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  MyHeader(in,out,"CalibrationRuns");
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << cgicc::legend("Calibration Runs").set("style","color:blue") ;
  //
  std::string CalibrationCFEBXtalk = toolbox::toString("/%s/CalibrationCFEBXtalk",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CalibrationCFEBXtalk) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Calibration Strips : CFEB Xtalk") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  std::string CalibrationCFEBGain = toolbox::toString("/%s/CalibrationCFEBGain",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CalibrationCFEBGain) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Calibration Strips : CFEB Gain") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  std::string CalibrationCFEBPedestal = toolbox::toString("/%s/CalibrationCFEBPedestal",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CalibrationCFEBPedestal) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Calibration Strips CFEB Pedestal") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  std::string CalibrationCFEBSaturation = toolbox::toString("/%s/CalibrationSaturation",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CalibrationCFEBSaturation) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Calibration Strips : CFEB high-end amplifier saturation") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  //
  std::string CalibrationALCTThresholdScan = toolbox::toString("/%s/CalibrationALCTThresholdScan",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CalibrationALCTThresholdScan) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Calibration ALCT : Threshold Scan") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  std::string CalibrationALCTConnectivity = toolbox::toString("/%s/CalibrationALCTConnectivity",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CalibrationALCTConnectivity) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Calibration ALCT : Connectivity") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  std::string CalibrationCFEBConnectivity = toolbox::toString("/%s/CalibrationCFEBConnectivity",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CalibrationCFEBConnectivity) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Calibration CFEB : Connectivity") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  std::string CalibrationComparatorPulse = toolbox::toString("/%s/CalibrationComparatorPulse",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CalibrationComparatorPulse) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Calibration Comparator Pulse") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::fieldset();
  //
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << cgicc::legend("Calibration tools").set("style","color:blue") ;
  //
  std::string FindLv1aDelayComparator = toolbox::toString("/%s/FindLv1aDelayComparator",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",FindLv1aDelayComparator) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Find l1a delay for Comparator") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  std::string FindLv1aDelayALCT = toolbox::toString("/%s/FindLv1aDelayALCT",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",FindLv1aDelayALCT) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Find l1a delay for ALCT") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::fieldset();
  //
}
//
void EmuPeripheralCrateConfig::CrateConfiguration(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "CrateConfiguration: " << ThisCrateID_ << std::endl;
  MyHeader(in,out,"CrateConfiguration");
  //
  if(thisCrate->IsAlive())
     *out << cgicc::h2("Current Crate: "+ ThisCrateID_ );
  else
     *out << cgicc::span().set("style","color:red") << cgicc::h2("Current Crate: "+ ThisCrateID_ + ",  Excluded") << cgicc::span();

  *out << std::endl;

  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial; background-color:#00FF00");
  *out << std::endl;
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
    //----------------------------------
    // Display crate controller buttons
    //----------------------------------
    //
    if(ii==1) {
      std::string ControllerBoardID = toolbox::toString("/%s/ControllerBoardID",getApplicationDescriptor()->getURN().c_str());
      //
      *out << cgicc::td();
      *out << "Controller Board ID" ;
      *out << cgicc::form().set("method","GET").set("action",ControllerBoardID) << std::endl ;
      *out << cgicc::input().set("type","text").set("name","ControllerBoardID").set("value",ControllerBoardID_) << std::endl ;
      *out << cgicc::form() << std::endl ;
      *out << cgicc::td();
      //
      *out << cgicc::td();
      std::string ControllerUtils = toolbox::toString("/%s/ControllerUtils?ccb=%d",getApplicationDescriptor()->getURN().c_str(),ii);
      *out << cgicc::a("Controller Utils").set("href",ControllerUtils) << endl;
      *out << cgicc::td();
      //
    }
    //
    //----------------------------------
    // Display CCB buttons
    //----------------------------------
    int slot = -1;
    if(thisCCB) slot=thisCCB->slot();
    if(slot == ii) {
      std::string CCBBoardID = toolbox::toString("/%s/CCBBoardID",getApplicationDescriptor()->getURN().c_str());
      //
      *out << cgicc::td();
      *out << "CCB Board ID" ;
      *out << cgicc::form().set("method","GET").set("action",CCBBoardID) << std::endl ;
      *out << cgicc::input().set("type","text").set("name","CCBBoardID").set("value",CCBBoardID_) << std::endl ;
      *out << cgicc::form() << std::endl ;
      *out << cgicc::td();
      //
      *out << cgicc::td();
      if ( CCBBoardID_.find("-1") == string::npos ) {
	std::string CCBStatus = toolbox::toString("/%s/CCBStatus?ccb=%d",getApplicationDescriptor()->getURN().c_str(),ii);
	*out << cgicc::a("CCB Status").set("href",CCBStatus) << endl;
      }
      *out << cgicc::td();
      //
      *out << cgicc::td();
      if ( CCBBoardID_.find("-1") == string::npos ) {
	//sprintf(Name,"CCB Utils slot=%d",slot);
	std::string CCBUtils = toolbox::toString("/%s/CCBUtils?ccb=%d",getApplicationDescriptor()->getURN().c_str(),ii);
	*out << cgicc::a("CCB Utils").set("href",CCBUtils) << endl;
      }
      *out << cgicc::td();
      //
    }
    //
    //----------------------------------
    // Display MPC buttons, if it exists
    //----------------------------------
    std::string MPCStatus = toolbox::toString("/%s/MPCStatus",getApplicationDescriptor()->getURN().c_str());
    std::string MPCBoardID = toolbox::toString("/%s/MPCBoardID",getApplicationDescriptor()->getURN().c_str());
    //
    slot = -1;
    if ( thisMPC ) slot = thisMPC->slot() ;
    if(slot == ii) {
      //
      *out << cgicc::td();
      *out << "MPC Board ID" ;
      *out << cgicc::form().set("method","GET").set("action",MPCBoardID) << std::endl ;
      *out << cgicc::input().set("type","text").set("name","MPCBoardID").set("value",MPCBoardID_) << std::endl ;
      *out << cgicc::form() << std::endl ;
      *out << cgicc::td();
      //
      if ( MPCBoardID_.find("-1") == string::npos && thisMPC ) {
	//
	*out << cgicc::td();
	std::string MPCStatus = toolbox::toString("/%s/MPCStatus?mpc=%d",getApplicationDescriptor()->getURN().c_str(),ii);
	*out << cgicc::a("MPC Status").set("href",MPCStatus) << endl;
	*out << cgicc::td();
	//
	*out << cgicc::td();
	std::string MPCUtils = toolbox::toString("/%s/MPCUtils?mpc=%d",getApplicationDescriptor()->getURN().c_str(),ii);
	*out << cgicc::a("MPC Utils").set("href",MPCUtils) << endl;
	*out << cgicc::td();
      }
    }
    //
    //----------------------------------
    // Display TMB buttons, if it exists
    //----------------------------------
    std::string TMBStatus ;
    std::string TMBTests ;
    std::string TMBUtils ;
    std::string TMBBoardID ;
    std::string RATBoardID ;
    //
    TMBStatus  = toolbox::toString("/%s/TMBStatus" ,getApplicationDescriptor()->getURN().c_str());
    TMBBoardID = toolbox::toString("/%s/TMBBoardID",getApplicationDescriptor()->getURN().c_str());
    RATBoardID = toolbox::toString("/%s/RATBoardID",getApplicationDescriptor()->getURN().c_str());
    TMBTests   = toolbox::toString("/%s/TMBTests"  ,getApplicationDescriptor()->getURN().c_str());
    TMBUtils   = toolbox::toString("/%s/TMBUtils"  ,getApplicationDescriptor()->getURN().c_str());
    //
    for (unsigned int i=0; i<tmbVector.size(); i++) {
      //
      int slot = tmbVector[i]->slot();
      if(slot == ii) {
	//
	char buf[20];
	//
	*out << cgicc::td();
	*out << "TMB Board ID" ;
	*out << cgicc::form().set("method","GET").set("action",TMBBoardID) << std::endl ;
	sprintf(buf,"TMBBoardID_%d",i);
	*out << cgicc::input().set("type","text").set("name",buf).set("value",TMBBoardID_[i]).set("size","10") << std::endl ;
	sprintf(buf,"%d",i);
	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
	*out << cgicc::form() << std::endl ;
	*out << cgicc::td();
	//
	*out << cgicc::td();
	*out << "RAT Board ID" ;
	*out << cgicc::form().set("method","GET").set("action",RATBoardID) << std::endl ;
	sprintf(buf,"RATBoardID_%d",i);
	*out << cgicc::input().set("type","text").set("name",buf).set("value",RATBoardID_[i]).set("size","10") << std::endl ;
	sprintf(buf,"%d",i);
	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","rat");
	*out << cgicc::form() << std::endl ;
	*out << cgicc::td();
	  //
	*out << cgicc::td();
	if ( TMBBoardID_[i].find("-1") == string::npos ) {
	  std::string MonitorTMBTrigger = toolbox::toString("/%s/MonitorTMBTrigger?tmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	  *out << cgicc::a("Monitor TMB trigger").set("href",MonitorTMBTrigger) << endl;
	  *out << cgicc::td();
	  //
	  *out << cgicc::td();
	  std::string TMBStatus = toolbox::toString("/%s/TMBStatus?tmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	  *out << cgicc::a("TMB Status").set("href",TMBStatus) << endl;
	  //
	}
	*out << cgicc::td();
	//
	*out << cgicc::td();
	if ( TMBBoardID_[i].find("-1") == string::npos ) {
	  std::string TMBTests = toolbox::toString("/%s/TMBTests?tmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	  *out << cgicc::a("TMB Tests").set("href",TMBTests) << endl;
	}
	*out << cgicc::td();
	//
	*out << cgicc::td();
	if ( TMBBoardID_[i].find("-1") == string::npos ) {
	    std::string TMBUtils = toolbox::toString("/%s/TMBUtils?tmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	    *out << cgicc::a("TMB Utils").set("href",TMBUtils) << endl;
	}
	*out << cgicc::td();
	//
	//Found TMB...look for DMB...
	//
	for (unsigned int iii=0; iii<dmbVector.size(); iii++) {
	  //
	  int dmbslot = dmbVector[iii]->slot();
	  std::string ChamberTests = toolbox::toString("/%s/ChamberTests",getApplicationDescriptor()->getURN().c_str());    
	  //
	  if ( dmbslot == slot+1 ) {
	    *out << cgicc::td();
	    if ( TMBBoardID_[i].find("-1") == string::npos ) {
	      //
	      char Name[50];
	      sprintf(Name,"Chamber Tests: %s",(thisCrate->GetChamber(slot)->GetLabel()).c_str());
	      //
	      std::string ChamberTests = toolbox::toString("/%s/ChamberTests?tmb=%d&dmb=%d",getApplicationDescriptor()->getURN().c_str(),i,iii);
	      *out << cgicc::a(Name).set("href",ChamberTests) << endl;
	      //
	      //std::cout << "Creating ChamberTests for TMB=" << i << ", DMB=" << iii << std::endl;
	      //
	      MyTest[i][current_crate_].SetTMB(tmbVector[i]);
	      MyTest[i][current_crate_].SetDMB(dmbVector[iii]);
	      MyTest[i][current_crate_].SetCCB(thisCCB);
	      MyTest[i][current_crate_].SetMPC(thisMPC);
	    }
	    *out << cgicc::td();
	  }
	  //
	}
      }
    }
    //
    std::string DMBStatus;
    std::string DMBTests;
    std::string DMBUtils;
    std::string DMBBoardID;
    //
    DMBStatus  = toolbox::toString("/%s/DMBStatus",getApplicationDescriptor()->getURN().c_str());
    DMBTests   = toolbox::toString("/%s/DMBTests",getApplicationDescriptor()->getURN().c_str());
    DMBUtils   = toolbox::toString("/%s/DMBUtils",getApplicationDescriptor()->getURN().c_str());
    DMBBoardID = toolbox::toString("/%s/DMBBoardID",getApplicationDescriptor()->getURN().c_str());
    //
    for (unsigned int i=0; i<dmbVector.size(); i++) {
      int slot = dmbVector[i]->slot();
      if(slot == ii ) {
	//
	char buf[20];
	//
	*out << cgicc::td();
	*out << "DMB Board ID" ;
	*out << cgicc::form().set("method","GET").set("action",DMBBoardID) << std::endl ;
	sprintf(buf,"DMBBoardID_%d",i);
	*out << cgicc::input().set("type","text").set("name",buf).set("value",DMBBoardID_[i]).set("size","10") << std::endl ;
	sprintf(buf,"%d",i);
	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
	*out << cgicc::form() << std::endl ;
	*out << cgicc::td();
	//
	*out << cgicc::td();
	if ( DMBBoardID_[i].find("-1",0) == string::npos ) {
	  std::string DMBStatus = toolbox::toString("/%s/DMBStatus?dmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	  *out << cgicc::a("DMB Status").set("href",DMBStatus) << endl;
	  //
	}
	*out << cgicc::td();
	//
	*out << cgicc::td();
	if ( DMBBoardID_[i].find("-1",0) == string::npos ) {
	  std::string DMBTests = toolbox::toString("/%s/DMBTests?dmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	  *out << cgicc::a("DMB Tests").set("href",DMBTests) << endl;
	}
	*out << cgicc::td();
	//
	*out << cgicc::td();
	if ( DMBBoardID_[i].find("-1",0) == string::npos ) {
	  std::string DMBUtils = toolbox::toString("/%s/DMBUtils?dmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	  *out << cgicc::a("DMB Utils").set("href",DMBUtils) << endl;
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
  // Here are the peripheral crate accoutrements...
  //
  *out << cgicc::table().set("border","1");
  //
  std::string CrateChassisID               = toolbox::toString("/%s/CrateChassisID"              ,getApplicationDescriptor()->getURN().c_str());
  std::string BackplaneID                  = toolbox::toString("/%s/BackplaneID"                 ,getApplicationDescriptor()->getURN().c_str());
  std::string CrateRegulatorBoardID        = toolbox::toString("/%s/CrateRegulatorBoardID"       ,getApplicationDescriptor()->getURN().c_str());
  std::string PeripheralCrateMotherBoardID = toolbox::toString("/%s/PeripheralCrateMotherBoardID",getApplicationDescriptor()->getURN().c_str());
  std::string ELMBID                       = toolbox::toString("/%s/ELMBID"                      ,getApplicationDescriptor()->getURN().c_str());
  //
  *out << cgicc::td();
  *out << "Crate Chassis ID" ;
  *out << cgicc::form().set("method","GET").set("action",CrateChassisID) << std::endl ;
  *out << cgicc::input().set("type","text").set("name","CrateChassisID").set("value",CrateChassisID_) << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  *out << "Backplane ID" ;
  *out << cgicc::form().set("method","GET").set("action",BackplaneID) << std::endl ;
  *out << cgicc::input().set("type","text").set("name","BackplaneID").set("value",BackplaneID_) << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  *out << "CRB ID" ;
  *out << cgicc::form().set("method","GET").set("action",CrateRegulatorBoardID) << std::endl ;
  *out << cgicc::input().set("type","text").set("name","CrateRegulatorBoardID").set("value",CrateRegulatorBoardID_) << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  *out << "PCMB ID" ;
  *out << cgicc::form().set("method","GET").set("action",PeripheralCrateMotherBoardID) << std::endl ;
  *out << cgicc::input().set("type","text").set("name","PeripheralCrateMotherBoardID").set("value",PeripheralCrateMotherBoardID_) << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  *out << "ELMB ID" ;
  *out << cgicc::form().set("method","GET").set("action",ELMBID) << std::endl ;
  *out << cgicc::input().set("type","text").set("name","ELMBID").set("value",ELMBID_) << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::table();
  //
  //
  *out << cgicc::table().set("border","1");
  //
  *out << cgicc::td();
  std::string MeasureL1AsAndDAVsForCrate = toolbox::toString("/%s/MeasureL1AsAndDAVsForCrate",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",MeasureL1AsAndDAVsForCrate) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Find L1A and DAV delays for crate") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string MeasureL1AsForCrate = toolbox::toString("/%s/MeasureL1AsForCrate",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",MeasureL1AsForCrate) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Find L1A delays for crate") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string MeasureDAVsForCrate = toolbox::toString("/%s/MeasureDAVsForCrate",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",MeasureDAVsForCrate) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Find DAV delays for crate") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::table();
  //*out << cgicc::body();
  *out << cgicc::fieldset();
  //
}

void EmuPeripheralCrateConfig::CrateStatus(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
  int read = (thisCCB->ReadRegister(0x4))&0xffff;
  //
  //
  *out << cgicc::fieldset().set("style","font-size: 10pt; font-family: arial;");
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
  if(((read>>14)&0x1) == 0) {
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
  *out << cgicc::fieldset() ;
  //
  read = (thisCCB->ReadRegister(0x2))&0xffff;
  //
  *out << cgicc::fieldset().set("style","font-size: 10pt; font-family: arial;");
  *out << cgicc::span().set("style","color:blue");
  *out << "MPC slot = 12 cfg             " << (read&0x1);
  *out << cgicc::span();
  *out << cgicc::br();
  *out << cgicc::fieldset() ;
  //
  *out << cgicc::fieldset().set("style","font-size: 10pt; font-family: arial;");
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
  *out << cgicc::fieldset() ;
  //
  *out << cgicc::fieldset().set("style","font-size: 10pt; font-family: arial;");
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
  *out << cgicc::fieldset() ;
  //
  *out << cgicc::fieldset().set("style","font-size: 10pt; font-family: arial;");
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
  *out << cgicc::fieldset() ;
  //
}
//
//////////////////////////////////////////////////////////////////////////
// Action:  calibrations
//////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::CalibrationALCTThresholdScan(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  CalibDAQ calib(emuEndcap_);
  //
  calib.ALCTThresholdScan();
  //
  this->Default(in,out);
  //
}
//
void EmuPeripheralCrateConfig::CalibrationALCTConnectivity(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  CalibDAQ calib(emuEndcap_);
  //
  calib.ALCTConnectivity();
  //
  this->Default(in,out);
  //
}
//
void EmuPeripheralCrateConfig::CalibrationCFEBConnectivity(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  CalibDAQ calib(emuEndcap_);
  //
  calib.CFEBConnectivity();
  //
  this->Default(in,out);
  //
}
//
void EmuPeripheralCrateConfig::FindLv1aDelayComparator(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  CalibDAQ calib(emuEndcap_);
  //
  calib.FindL1aDelayComparator();
  //
  this->Default(in,out);
  //
}
//
void EmuPeripheralCrateConfig::FindLv1aDelayALCT(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  CalibDAQ calib(emuEndcap_);
  //
  calib.FindL1aDelayALCT();
  //
  this->Default(in,out);
  //
}
//
void EmuPeripheralCrateConfig::CalibrationCFEBGain(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  CalibDAQ calib(emuEndcap_);
  //
  calib.gainCFEB();
  //
  this->Default(in,out);
  //
}
//
void EmuPeripheralCrateConfig::CalibrationCFEBSaturation(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  CalibDAQ calib(emuEndcap_);
  //
  calib.CFEBSaturationTest();
  //
  this->Default(in,out);
  //
}
//
void EmuPeripheralCrateConfig::CalibrationCFEBXtalk(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  CalibDAQ calib(emuEndcap_);
  //
  calib.timeCFEB();
  //
  this->Default(in,out);
  //
}
//
void EmuPeripheralCrateConfig::CalibrationComparatorPulse(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  CalibDAQ calib(emuEndcap_);
  //
  calib.pulseComparatorPulse();
  //
  this->Default(in,out);
  //
}
//
void EmuPeripheralCrateConfig::CalibrationCFEBPedestal(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  CalibDAQ calib(emuEndcap_);
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
void EmuPeripheralCrateConfig::CrateDumpConfiguration(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  thisCrate->DumpConfiguration();
  //
  this->Default(in,out);
}
//
///////////////////////////////////////////////////////
// Counters displays
///////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::CrateTMBCountersRight(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  ostringstream output;
  output << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
  output << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  //
  cgicc::CgiEnvironment cgiEnvi(in);
  std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
  //
  if (AutoRefresh_) {
    *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"3; URL=/"
	 <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<endl;
  }
  //
  Page=cgiEnvi.getQueryString();
  std::string crate_name=Page.substr(0,Page.find("=", 0) );
  *out << cgicc::b("Crate: "+crate_name) << std::endl;
  vector<TMB*> myVector;
  for ( unsigned int i = 0; i < crateVector.size(); i++ )
  {
     if(crate_name==crateVector[i]->GetLabel()) myVector = crateVector[i]->tmbs();
  }
  if(Monitor_On_)
  {
     *out << cgicc::span().set("style","color:green");
     *out << cgicc::b(cgicc::i("Monitor Status: On")) << cgicc::span() << std::endl ;
  } else 
  { 
     *out << cgicc::span().set("style","color:red");
     *out << cgicc::b(cgicc::i("Monitor Status: Off")) << cgicc::span() << std::endl ;
  }

  output << cgicc::table().set("border","1");
  //
  output <<cgicc::td();
  //
  output <<cgicc::td();
  //
  for(unsigned int tmb=0; tmb<myVector.size(); tmb++) {
// TMB counters are read in the monitoring FastLoop
//    myVector[tmb]->GetCounters();
    //
    output <<cgicc::td();
    output << "Slot = " <<myVector[tmb]->slot();
    output <<cgicc::td();
    //
  }
  //
  output <<cgicc::tr();
  //
  for (int count=0; count<25; count++) {
    //
    for(unsigned int tmb=0; tmb<myVector.size(); tmb++) {
      //
      output <<cgicc::td();
      //
      if(tmb==0) {
	output << myVector[tmb]->CounterName(count) ;
	output <<cgicc::td();
	output <<cgicc::td();
      }
      if (DisplayRatio_) {
	 if ( myVector[tmb]->GetCounter(16) > 0 )
	    output << ((float)(myVector[tmb]->GetCounter(count))/(myVector[tmb]->GetCounter(16)));
	 else 
	    output << "-1";
      } 
      else {
        if ( myVector[tmb]->GetCounter(count) == 0x3fffffff )
           output << "-1";
        else 
   	   output << myVector[tmb]->GetCounter(count);
      }
      output <<cgicc::td();
    }
    output <<cgicc::tr();
  }
  //
  output << cgicc::table();
  //
  *out << output.str()<<std::endl;
  //
}
//
void EmuPeripheralCrateConfig::CrateDMBCounters(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  //
  cgicc::CgiEnvironment cgiEnvi(in);
  //
  std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
  //
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"5; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<endl;
  //
  Page=cgiEnvi.getQueryString();
  std::string crate_name=Page.substr(0,Page.find("=", 0) );
  *out << cgicc::b("Crate: "+crate_name) << std::endl;
  vector<DAQMB*> myVector;
  for ( unsigned int i = 0; i < crateVector.size(); i++ )
  {
     if(crate_name==crateVector[i]->GetLabel()) myVector = crateVector[i]->daqmbs();
  }
  if(Monitor_On_)
  {
     *out << cgicc::span().set("style","color:green");
     *out << cgicc::b(cgicc::i("Monitor Status: On")) << cgicc::span() << std::endl ;
  } else 
  { 
     *out << cgicc::span().set("style","color:red");
     *out << cgicc::b(cgicc::i("Monitor Status: Off")) << cgicc::span() << std::endl ;
  }
  *out << cgicc::table().set("border","1");
  //
  *out <<cgicc::td();
  *out <<cgicc::td();
  //
// DMB counters are read in the monitoring FastLoop
// 
//  for(unsigned int dmb=0; dmb<myVector.size(); dmb++) {
//    myVector[dmb]->readtimingCounter();
//    myVector[dmb]->readtimingScope();
//  }
  //
  for(unsigned int dmb=0; dmb<myVector.size(); dmb++) {
    *out <<cgicc::td();
    *out << "Slot = " <<myVector[dmb]->slot();
    *out <<cgicc::td();
  }
  //
  *out <<cgicc::tr();
  //
  *out <<cgicc::td();
  *out << myVector[0]->CounterName(0);
  *out <<cgicc::td();
  //
  for(unsigned int dmb=0; dmb<myVector.size(); dmb++) {
    //
    *out <<cgicc::td();
    if ( myVector[dmb]->GetL1aLctCounter() > 0 ) {
      L1aLctCounter_[dmb] = myVector[dmb]->GetL1aLctCounter();
    }
    *out << L1aLctCounter_[dmb] <<std::endl;
    *out <<cgicc::td();
    //
  }
  *out <<cgicc::tr();
  //
  *out <<cgicc::td();
  *out << myVector[0]->CounterName(1);
  *out <<cgicc::td();
  //
  for(unsigned int dmb=0; dmb<myVector.size(); dmb++) {
    //
    *out <<cgicc::td();
    if ( myVector[dmb]->GetCfebDavCounter() > 0 ) CfebDavCounter_[dmb] = myVector[dmb]->GetCfebDavCounter();
    *out << CfebDavCounter_[dmb] <<std::endl;
    *out <<cgicc::td();
    //
  }
  *out <<cgicc::tr();
  //
  *out <<cgicc::td();
  *out << myVector[0]->CounterName(2);
  *out <<cgicc::td();
  //
  for(unsigned int dmb=0; dmb<myVector.size(); dmb++) {
    //
    *out <<cgicc::td();
    if ( myVector[dmb]->GetTmbDavCounter() > 0 ) TmbDavCounter_[dmb] = myVector[dmb]->GetTmbDavCounter();
    *out << TmbDavCounter_[dmb] <<std::endl;
    *out <<cgicc::td();
    //
  }
  *out <<cgicc::tr();
  //
  *out <<cgicc::td();
  *out << myVector[0]->CounterName(3);
  *out <<cgicc::td();
  //
  for(unsigned int dmb=0; dmb<myVector.size(); dmb++) {
    //
    *out <<cgicc::td();
    if ( myVector[dmb]->GetAlctDavCounter() > 0 ) AlctDavCounter_[dmb] = myVector[dmb]->GetAlctDavCounter();
    *out << AlctDavCounter_[dmb] <<std::endl;
    *out <<cgicc::td();
  }
  *out <<cgicc::tr();
  //
  *out <<cgicc::td();
  *out << myVector[0]->CounterName(4);
  *out <<cgicc::td();
  //
  for(unsigned int dmb=0; dmb<myVector.size(); dmb++) {
    //
    *out <<cgicc::td();
    for( int i=4; i>-1; i--) *out << ((myVector[dmb]->GetL1aLctScope()>>i)&0x1) ;
    *out <<cgicc::td();
  }
  *out <<cgicc::tr();
  //
  *out <<cgicc::tr();
  *out <<cgicc::tr();
  //
  *out <<cgicc::td();
  *out << myVector[0]->CounterName(5);
  *out <<cgicc::td();
  //
  for(unsigned int dmb=0; dmb<myVector.size(); dmb++) {
    //
    *out <<cgicc::td();
    for( int i=4; i>-1; i--) *out << ((myVector[dmb]->GetCfebDavScope()>>i)&0x1) ;
    *out <<cgicc::td();
    //
  }
  *out <<cgicc::tr();
  //
  *out <<cgicc::td();
  *out << myVector[0]->CounterName(6);
  *out <<cgicc::td();
  //
  for(unsigned int dmb=0; dmb<myVector.size(); dmb++) {
    //
    *out <<cgicc::td();
    for( int i=4; i>-1; i--) *out << ((myVector[dmb]->GetTmbDavScope()>>i)&0x1) ;
    *out <<cgicc::td();
    //
  }
  *out <<cgicc::tr();
  //
  *out <<cgicc::td();
  *out << myVector[0]->CounterName(7);
  *out <<cgicc::td();
  //
  for(unsigned int dmb=0; dmb<myVector.size(); dmb++) {
    //
    *out <<cgicc::td();
    for( int i=4; i>-1; i--) *out << ((myVector[dmb]->GetAlctDavScope()>>i)&0x1) ;
    *out <<cgicc::td();
  }
  *out <<cgicc::tr();
  //
  *out << cgicc::table();
  //
}
//
void EmuPeripheralCrateConfig::CrateTMBCounters(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::CgiEnvironment cgiEnvi(in);
  //
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  //
  std::string MonitorFrameLeft      = toolbox::toString("/%s/MonitorFrameLeft",getApplicationDescriptor()->getURN().c_str());
  std::string CrateTMBCountersRight = toolbox::toString("/%s/CrateTMBCountersRight",getApplicationDescriptor()->getURN().c_str());
  //
  *out << cgicc::frameset().set("cols","200,*");
  *out << cgicc::frame().set("src",MonitorFrameLeft);
  *out << cgicc::frame().set("src",CrateTMBCountersRight);
  *out << cgicc::frameset() ;
  //
}
//
void EmuPeripheralCrateConfig::LaunchMonitor(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  //
  std::string MonitorFrameLeft  = toolbox::toString("/%s/MonitorFrameLeft",getApplicationDescriptor()->getURN().c_str());
  std::string MonitorFrameRight = toolbox::toString("/%s/MonitorFrameRight",getApplicationDescriptor()->getURN().c_str());
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
void EmuPeripheralCrateConfig::MonitorFrameRight(xgi::Input * in, xgi::Output * out) 
  throw (xgi::exception::Exception) {
  //
  MyHeader(in,out,"Monitor");
  //
  for(int counter=0; counter<22; counter++) {
    Counter_ = counter;
    //
    std::string CreateMonitorUnit = toolbox::toString("/%s/CreateMonitorUnit?counter=%d",getApplicationDescriptor()->getURN().c_str(),counter);
    //
    this->CreateMonitorUnit(in,out);
  }
  //
}
//
void EmuPeripheralCrateConfig::MonitorFrameLeft(xgi::Input * in, xgi::Output * out) 
  throw (xgi::exception::Exception) {
  //
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  //
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  //
  std::string ResetAllCounters = toolbox::toString("/%s/ResetAllCounters",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ResetAllCounters) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Reset All Counters") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  std::string SetUnsetRatio = toolbox::toString("/%s/SetUnsetRatio",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",SetUnsetRatio) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Set/Unset Ratio Display") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  std::string SetUnsetAutoRefresh = toolbox::toString("/%s/SetUnsetAutoRefresh",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",SetUnsetAutoRefresh) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Set/Unset AutoRefresh") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  if(Monitor_On_)
  {
     *out << cgicc::span().set("style","color:green");
     *out << cgicc::b(cgicc::i("Monitor Status: On")) << cgicc::span() << std::endl ;
  } else 
  { 
     *out << cgicc::span().set("style","color:red");
     *out << cgicc::b(cgicc::i("Monitor Status: Off")) << cgicc::span() << std::endl ;
  }

  this->LaunchMonitor(in,out);
  //
}
//
void EmuPeripheralCrateConfig::MonitorTMBTrigger(xgi::Input * in, xgi::Output * out) 
  throw (xgi::exception::Exception) {
  //
  cgicc::CgiEnvironment cgiEnvi(in);
  std::string Page="MonitorTMBTriggerRedirect?"+cgiEnvi.getQueryString();
  //
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<endl;
  //
}
//
void EmuPeripheralCrateConfig::MonitorTMBTriggerRedirect(xgi::Input * in, xgi::Output * out) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
  } else {
    cout << "MonitorTMBTrigger:  No tmb" << endl ;
  }
  //
  //
  std::string MenuMonitorTMBTrigger    = toolbox::toString("/%s/MenuMonitorTMBTrigger",getApplicationDescriptor()->getURN().c_str());
  std::string MonitorTMBTriggerDisplay = toolbox::toString("/%s/MonitorTMBTriggerDisplay?tmb=",getApplicationDescriptor()->getURN().c_str(),tmb);
  //
  *out << cgicc::frameset().set("cols","200,*");
  *out << cgicc::frame().set("src",MenuMonitorTMBTrigger);
  *out << cgicc::frame().set("src",MonitorTMBTriggerDisplay);
  *out << cgicc::frameset() ;
  //
}
//
void EmuPeripheralCrateConfig::MenuMonitorTMBTrigger(xgi::Input * in, xgi::Output * out) 
  throw (xgi::exception::Exception){
  //
  std::string AlctKey = toolbox::toString("/%s/AlctKey",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",AlctKey) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Alct key") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  std::string ClctKey = toolbox::toString("/%s/ClctKey",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ClctKey) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Clct key") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  *out << "Menu " << MenuMonitor_ << std::endl;
  //
}
//
void EmuPeripheralCrateConfig::AlctKey(xgi::Input * in, xgi::Output * out) 
  throw (xgi::exception::Exception) {
  //
  MenuMonitor_ = 2;
  //
  this->MenuMonitorTMBTrigger(in,out);
  //
}
//
void EmuPeripheralCrateConfig::ClctKey(xgi::Input * in, xgi::Output * out) 
  throw (xgi::exception::Exception) {
  //
  MenuMonitor_ = 1;
  //
  this->MenuMonitorTMBTrigger(in,out);
  //
}
//
void EmuPeripheralCrateConfig::MonitorTMBTriggerDisplay(xgi::Input * in, xgi::Output * out) 
  throw (xgi::exception::Exception) {
  //
  cgicc::CgiEnvironment cgiEnvi(in);
  //
  std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
  //
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"2; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<endl;
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
void EmuPeripheralCrateConfig::CreateMonitorUnit(xgi::Input * in, xgi::Output * out) 
  throw (xgi::exception::Exception) {
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
// TMB counters are read in the monitoring FastLoop
//    tmbVector[i]->GetCounters();
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
void EmuPeripheralCrateConfig::Display(xgi::Input * in, xgi::Output * out, int counter ) 
  throw (xgi::exception::Exception) {
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

void EmuPeripheralCrateConfig::SetUnsetRatio(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
void EmuPeripheralCrateConfig::SetUnsetAutoRefresh(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
void EmuPeripheralCrateConfig::ResetAllCounters(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  for(unsigned int i=0; i<tmbVector.size(); i++) 
  {
    tmbVector[i]->ResetCounters();
  }

  this->Default(in,out);
}

//
////////////////////////////////////////////////////////////////////////////////////
// Main page methods
////////////////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::InitSystem(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cout << "Init System" << endl ;
  LOG4CPLUS_INFO(getApplicationLogger(), "Init System");
  //
  //
  thisCrate->configure();          // Init system
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
/////////////////////////////////////////////////////////////////////
// Chamber Utilities (synchronization) methods
/////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::InitChamber(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
    cout << "InitChamber:  DMB " << dmb << endl;
    DMB_ = dmb;
  } else {
    cout << "InitChamber:No dmb" << endl;
    dmb = DMB_;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    cout << "InitChamber:  TMB " << tmb << endl;
    TMB_ = tmb;
  } else {
    cout << "InitChamber:  No tmb" << endl;
    tmb = TMB_;
  }
  //
  MyTest[tmb][current_crate_].InitSystem();          // Init chamber
  //
  // Comment out dangerous next line....
  //  thisCCB->setCCBMode(CCB::VMEFPGA);      // It needs to be in FPGA mode to work.
  //
  this->ChamberTests(in,out);
  //
}
//////////////////////////////////////////////////////////////////////////
// Crate utilities methods
//////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::TmbMPCTest(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  myCrateTest.SetCrate(thisCrate);
  //
  myCrateTest.RedirectOutput(&CrateTestsOutput);
  int number_of_loops = 1000;
  myCrateTest.MpcTMBTest(number_of_loops);
  myCrateTest.RedirectOutput(&std::cout);
  //
  this->CrateTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::MPCSafeWindowScan(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  //CrateUtilities myCrateTest;
  myCrateTest.SetCrate(thisCrate);
  //
  myCrateTest.RedirectOutput(&CrateTestsOutput);
  int number_of_loops = 10;
  int min_value       = 15;
  int max_value       = 75;
  myCrateTest.MpcTMBTest(number_of_loops,min_value,max_value);
  myCrateTest.RedirectOutput(&std::cout);
  //
  this->CrateTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::Operator(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  cgicc::Cgicc cgi(in);
  //
  Operator_= cgi["Operator"]->getValue() ;
  //
  this->Default(in,out);
}
//
void EmuPeripheralCrateConfig::RunNumber(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  cgicc::Cgicc cgi(in);
  //
  RunNumber_= cgi["RunNumber"]->getValue() ;
  std::cout << "RunNumber " << RunNumber_ << std::endl ;
  LOG4CPLUS_INFO(getApplicationLogger(), "RunNumber");
  //
  this->Default(in,out);
}
//
//////////////////////////////////////////////////////////////
// DMB tests
//////////////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::DMBTests(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  int dmb;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    cout << "DMB tests: DMB " << dmb << endl;
    DMB_ = dmb;
  } else {
    cout << "DMB tests: No dmb" << endl ;
    dmb = DMB_;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  Chamber * thisChamber = chamberVector[dmb];
  //
  char Name[100];
  sprintf(Name,"%s DMB tests, crate=%s, slot=%d",(thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),thisDMB->slot());
  //
  MyHeader(in,out,Name);
  //
  char buf[200] ;
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << endl ;
  //
  *out << cgicc::legend("DMB Tests").set("style","color:blue") ;
  //
  std::string DMBTestAll = toolbox::toString("/%s/DMBTestAll",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBTestAll) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","DMB Test All ") << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  //
  *out << cgicc::table().set("border","0");
  //
  ////////////////////////////////////////////
  *out << cgicc::td();
  std::string DMBTest3 = toolbox::toString("/%s/DMBTest3",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBTest3) << std::endl ;
  if( thisDMB->GetTestStatus(3) == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","DMB Test3 (Check DMB Fifos)").set("style","color:blue" ) << std::endl ;
    //
  } else if( thisDMB->GetTestStatus(3) == 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","DMB Test3 (Check DMB Fifos)").set("style","color:green") << std::endl ;
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","DMB Test3 (Check DMB Fifos)").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td();
  std::string DMBTest4 = toolbox::toString("/%s/DMBTest4",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBTest4) << std::endl ;
  if( thisDMB->GetTestStatus(4) == -1 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test4 (Check voltages)").set("style","color:blue" ) << std::endl ;
  } 
  if( thisDMB->GetTestStatus(4) > 0 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test4 (Check voltages)").set("style","color:red"  ) << std::endl ;
  }
  if( thisDMB->GetTestStatus(4) ==0 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test4 (Check voltages)").set("style","color:green") << std::endl ;
  }
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td();
  std::string DMBTest5 = toolbox::toString("/%s/DMBTest5",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBTest5) << std::endl ;
  if( thisDMB->GetTestStatus(5) == -1 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test5 (Check Power Register)").set("style","color:blue" ) << std::endl ;
  }
  if( thisDMB->GetTestStatus(5) > 0 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test5 (Check Power Register)").set("style","color:red"  ) << std::endl ;
  }
  if( thisDMB->GetTestStatus(5) ==0 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test5 (Check Power Register)").set("style","color:green") << std::endl ;
  }
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td();
  std::string DMBTest6 = toolbox::toString("/%s/DMBTest6",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBTest6) << std::endl ;
  if( thisDMB->GetTestStatus(6) == -1 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test6 (Check FPGA IDs)").set("style","color:blue" ) << std::endl ;
  }
  if( thisDMB->GetTestStatus(6) > 0 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test6 (Check FPGA IDs)").set("style","color:red"  ) << std::endl ;
  }
  if( thisDMB->GetTestStatus(6) ==0 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test6 (Check FPGA IDs)").set("style","color:green") << std::endl ;
  }
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  /////////////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td();
  std::string DMBTest8 = toolbox::toString("/%s/DMBTest8",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBTest8) << std::endl ;
  if( thisDMB->GetTestStatus(8) == -1 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test8 (Check Comparator DAC/ADC)").set("style","color:blue" ) << std::endl ;
  }
  if( thisDMB->GetTestStatus(8) > 0 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test8 (Check Comparator DAC/ADC)").set("style","color:red"  ) << std::endl ;
  }
  if( thisDMB->GetTestStatus(8) ==0 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test8 (Check Comparator DAC/ADC)").set("style","color:green") << std::endl ;
  }
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td();
  std::string DMBTest9 = toolbox::toString("/%s/DMBTest9",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBTest9) << std::endl ;
  if( thisDMB->GetTestStatus(9) == -1 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test9 (Check CDAC)").set("style","color:blue" ) << std::endl ;
  }
  if( thisDMB->GetTestStatus(9) > 0 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test9 (Check CDAC)").set("style","color:red"  ) << std::endl ;
  }
  if( thisDMB->GetTestStatus(9) ==0 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test9 (Check CDAC)").set("style","color:green") << std::endl ;
  }
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td();
  std::string DMBTest10 = toolbox::toString("/%s/DMBTest10",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBTest10) << std::endl ;
  if( thisDMB->GetTestStatus(10) == -1 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test10 (Check SFM)").set("style","color:blue" ) << std::endl ;
  }
  if( thisDMB->GetTestStatus(10) > 0 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test10 (Check SFM)").set("style","color:red"  ) << std::endl ;
  }
  if( thisDMB->GetTestStatus(10) ==0 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test10 (Check SFM)").set("style","color:green") << std::endl ;
  }
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  /*
   *out << cgicc::td();
   std::string DMBTest11 = toolbox::toString("/%s/DMBTest11",getApplicationDescriptor()->getURN().c_str());
   *out << cgicc::form().set("method","GET").set("action",DMBTest11) << std::endl ;
   if( thisDMB->GetTestStatus(11) == -1 ) {
   *out << cgicc::input().set("type","submit").set("value","DMB Test11 (Check buckflash)").set("style","color:blue" ) << std::endl ;
   }
   if( thisDMB->GetTestStatus(11) > 0 ) {
   *out << cgicc::input().set("type","submit").set("value","DMB Test11 (Check buckflash)").set("style","color:red"  ) << std::endl ;
   }
   if( thisDMB->GetTestStatus(11) ==0 ) {
   *out << cgicc::input().set("type","submit").set("value","DMB Test11 (Check buckflash)").set("style","color:green") << std::endl ;
   }
   sprintf(buf,"%d",dmb);
   *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
   *out << cgicc::form() << std::endl ;
   *out << cgicc::td();
   */
  //
  *out << cgicc::table();
  //
  *out << cgicc::fieldset() << std::endl;
  //
  *out << cgicc::form().set("method","GET") << std::endl ;
  *out << cgicc::textarea().set("name","CrateTestDMBOutput").set("WRAP","OFF").set("rows","20").set("cols","60");
  *out << OutputDMBTests[dmb][current_crate_].str() << endl ;
  *out << cgicc::textarea();
  *out << cgicc::form();
  //
  std::string method = toolbox::toString("/%s/LogDMBTestsOutput",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",method) << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::input().set("type","submit").set("value","Log output").set("name","LogDMBTestsOutput") << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Clear").set("name","ClearDMBTestsOutput") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  //  std::cout << "Done" << std::endl;
  //
}
//
void EmuPeripheralCrateConfig::DMBTestAll(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
  OutputDMBTests[dmb][current_crate_] << "DMB TestAll" << endl ;
  //
  thisDMB->RedirectOutput(&OutputDMBTests[dmb][current_crate_]);
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
void EmuPeripheralCrateConfig::DMBTest3(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    cout << "Test3 DMB " << dmb << endl;
    DMB_ = dmb;
  }
  //
  OutputDMBTests[dmb][current_crate_] << "DMB Test3" << endl ;
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  thisDMB->RedirectOutput(&OutputDMBTests[dmb][current_crate_]);
  thisDMB->test3();
  thisDMB->RedirectOutput(&std::cout);
  //
  this->DMBTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::DMBTest4(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception){
    //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    cout << "Test4 DMB " << dmb << endl;
    DMB_ = dmb;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  OutputDMBTests[dmb][current_crate_] << "DMB Test4" << endl ;
  //
  thisDMB->RedirectOutput(&OutputDMBTests[dmb][current_crate_]);
  thisDMB->test4();
  thisDMB->RedirectOutput(&std::cout);
  //
  this->DMBTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::DMBTest5(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    cout << "Test5 DMB " << dmb << endl;
    DMB_ = dmb;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  OutputDMBTests[dmb][current_crate_] << "DMB Test5" << endl ;
  //
  thisDMB->RedirectOutput(&OutputDMBTests[dmb][current_crate_]);
  thisDMB->test5();
  thisDMB->RedirectOutput(&std::cout);
  //
  this->DMBTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::DMBTest6(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    cout << "Test6 DMB " << dmb << endl;
    DMB_ = dmb;
  }
  //
  OutputDMBTests[dmb][current_crate_] << "DMB Test6" << endl ;
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  thisDMB->RedirectOutput(&OutputDMBTests[dmb][current_crate_]);
  thisDMB->test6();
  thisDMB->RedirectOutput(&std::cout);
  //
  this->DMBTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::DMBTest8(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    cout << "Test8 DMB " << dmb << endl;
    DMB_ = dmb;
  }
  //
  OutputDMBTests[dmb][current_crate_] << "DMB Test8" << endl ;
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  thisDMB->RedirectOutput(&OutputDMBTests[dmb][current_crate_]);
  thisDMB->test8();
  thisDMB->RedirectOutput(&std::cout);
  //
  this->DMBTests(in,out);
}
//
void EmuPeripheralCrateConfig::DMBTest9(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
    //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    cout << "Test9 DMB " << dmb << endl;
    DMB_ = dmb;
  }
  //
  OutputDMBTests[dmb][current_crate_] << "DMB Test9" << endl ;
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  thisDMB->RedirectOutput(&OutputDMBTests[dmb][current_crate_]);
  thisDMB->test9();
  thisDMB->RedirectOutput(&std::cout);
  //
  this->DMBTests(in,out);
}
//
void EmuPeripheralCrateConfig::DMBTest10(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    cout << "Test10 DMB " << dmb << endl;
    DMB_ = dmb;
  }
  //
  OutputDMBTests[dmb][current_crate_] << "DMB Test10" << endl ;
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  thisDMB->RedirectOutput(&OutputDMBTests[dmb][current_crate_]);
  thisDMB->test10();
  thisDMB->RedirectOutput(&std::cout);
  //
  this->DMBTests(in,out);
}
//
void EmuPeripheralCrateConfig::DMBTest11(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    cout << "Test11 DMB " << dmb << endl;
    DMB_ = dmb;
  }
  //
  OutputDMBTests[dmb][current_crate_] << "DMB Test11" << endl ;
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
////////////////////////////////////////////////////////////////
// Chamber Tests (synchronization) methods
////////////////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::ChamberTests(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb, dmb;
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  if( name != cgi.getElements().end() ) {
    dmb = cgi["dmb"]->getIntegerValue();
    cout << "ChamberTests:  DMB " << dmb << endl;
    DMB_ = dmb;
  } else {
    cout << "ChamberTests:  No dmb" << endl;
    dmb = DMB_;
  }
  //
  name = cgi.getElement("tmb");
  //
  if( name != cgi.getElements().end() ) {
    tmb = cgi["tmb"]->getIntegerValue();
    cout << "ChamberTests:  TMB " << tmb << endl;
    TMB_ = tmb;
  } else {
    cout << "ChamberTests:  No tmb" << endl;
    tmb = TMB_;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  DAQMB * thisDMB = dmbVector[dmb];
  alct = thisTMB->alctController();
  //
  Chamber * thisChamber = chamberVector[tmb];
  //
  char Name[100];
  sprintf(Name,"%s synchronization, crate=%s, TMBslot=%d, DMBslot=%d",
	  (thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),thisTMB->slot(),thisDMB->slot());
  //
  MyHeader(in,out,Name);
  //
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  //
  *out << cgicc::legend("CSC trigger primitive synchronization and tests").set("style","color:blue") << cgicc::p() << std::endl ;
  //
  char buf[20];
  //
  //    std::string InitChamber = toolbox::toString("/%s/InitChamber",getApplicationDescriptor()->getURN().c_str());
  //    *out << cgicc::form().set("method","GET").set("action",InitChamber) << std::endl ;
  //    *out << cgicc::input().set("type","submit").set("value","Init Chamber") << std::endl ;
  //    *out << cgicc::input().set("type","hidden").set("value","1").set("name","navigator");
  //    *out << cgicc::form() << std::endl ;
  //
  //    std::string TMBStartTrigger = toolbox::toString("/%s/TMBStartTrigger",getApplicationDescriptor()->getURN().c_str());
  //    *out << cgicc::form().set("method","GET").set("action",TMBStartTrigger) << std::endl ;
  //    *out << cgicc::input().set("type","submit").set("value","TMBStartTrigger") << std::endl ;
  //    sprintf(buf,"%d",tmb);
  //    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  //    sprintf(buf,"%d",dmb);
  //    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  //    *out << cgicc::form() << std::endl ;
  //
  //    std::string EnableL1aRequest = toolbox::toString("/%s/EnableL1aRequest",getApplicationDescriptor()->getURN().c_str());
  //    *out << cgicc::form().set("method","GET").set("action",EnableL1aRequest) << std::endl ;
  //    *out << cgicc::input().set("type","submit").set("value","EnableL1aRequest") << std::endl ;
  //    sprintf(buf,"%d",tmb);
  //    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  //    sprintf(buf,"%d",dmb);
  //    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  //    *out << cgicc::form() << std::endl ;
  //
  //
  *out << cgicc::pre();
  *out << "-------------------------------------------------------------------------" << std::endl;
  *out << " Synchronize the Chamber Trigger and DAQ paths" << std::endl;
  *out << "-------------------------------------------------------------------------" << std::endl;
  *out << "0) Prepare to synchronize system" << std::endl;
  *out << "   a) LV:  on" << std::endl;
  *out << "   b) Restart Peripheral Crate XDAQ" << std::endl;
  *out << "   c) Init System" << std::endl;
  *out << cgicc::pre();
  //
  //
  *out << cgicc::pre();
  *out << "1) Measure relative clock phases with pulsing" << std::endl;
  *out << "   a) LTC:  Stop L1A triggers" << std::endl;
  *out << "   b) Push the following buttons:" << std::endl;
  *out << cgicc::pre();
  //
  std::string ALCTTiming = toolbox::toString("/%s/ALCTTiming",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ALCTTiming) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Scan ALCT tx/rx phases") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << "alct_rx_clock_phase = " << MyTest[tmb][current_crate_].GetALCTrxPhaseTest() 
       <<  " (" << MyTest[tmb][current_crate_].GetALCTrxPhase() << ") " << std::endl;
  *out << cgicc::br();
  *out << "alct_tx_clock_phase = " << MyTest[tmb][current_crate_].GetALCTtxPhaseTest() 
       <<  " (" << MyTest[tmb][current_crate_].GetALCTtxPhase() << ") " << std::endl;
  *out << cgicc::br();
  *out << cgicc::br();
  //
  //
  std::string CFEBTiming = toolbox::toString("/%s/CFEBTiming",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CFEBTiming) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Scan CFEB rx phases") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  for(int i=0;i<5;i++) {
    *out << "cfeb" << i << "delay = " << MyTest[tmb][current_crate_].GetCFEBrxPhaseTest(i) 
	 << " ("  << MyTest[tmb][current_crate_].GetCFEBrxPhase(i) << ") " <<std::endl;
    *out << cgicc::br();
  }
  *out << cgicc::br();
  //
  //
  std::string RatTmbTiming = toolbox::toString("/%s/RatTmbTiming",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",RatTmbTiming) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Scan RAT-TMB phase") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << "rat_tmb_delay = " << MyTest[tmb][current_crate_].GetRatTmbDelayTest() 
       << " ("  << MyTest[tmb][current_crate_].GetRatTmbDelay()     << ") " << std::endl;
  *out << cgicc::br();
  //
  *out << cgicc::pre();
  *out << "   c) Enter above values into xml" << std::endl;
  *out << "   d) Set up the xml file with the desired ALCT and CLCT trigger configuration" << std::endl;
  *out << "   e) Restart Peripheral Crate XDAQ" << std::endl;
  *out << "   f) Init System" << std::endl;
  *out << cgicc::pre();
  //
  //
  *out << cgicc::pre();
  *out << "2) Set up to perform synchronization" << std::endl;
  *out << "   a) LTC Hard Reset" << std::endl;
  *out << "   b) For muons from LHC beam or cosmic rays:" << std::endl;
  *out << "      i ) HV:  on" << std::endl;
  *out << "      ii) skip to step d)" << std::endl;
  *out << "   c) For pulsing from the TTC:" << std::endl;
  *out << "      i ) push the following button" << std::endl;
  *out << cgicc::pre();
  //
  std::string setupCoincidencePulsing = toolbox::toString("/%s/setupCoincidencePulsing",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",setupCoincidencePulsing) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Setup Coincidence Pulsing") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::pre();
  *out << "   d) LTC:  Resync" << std::endl;
  *out << "   e) LTC Enable L1A's" << std::endl;
  *out << "   f) LTC Begin cyclic BGo to fire ADB Sync pulse on CCB" << std::endl;
  *out << "   g) Set the parameters for performing the synchronization scans" << std::endl;
  *out << cgicc::pre();
  //
  std::string setDataReadValues = toolbox::toString("/%s/setDataReadValues",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",setDataReadValues) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Set DMB/ALCT-CLCT match data read values") << std::endl ;
  sprintf(buf,"%d",MyTest[tmb][current_crate_].getNumberOfDataReads());
  *out << "Number of reads" << std::endl;
  *out << cgicc::input().set("type","text").set("value",buf).set("name","number_of_reads") << std::endl ;
  sprintf(buf,"%d",MyTest[tmb][current_crate_].getPauseBetweenDataReads());
  *out << "Pause between reads (usec)" << std::endl;
  *out << cgicc::input().set("type","text").set("value",buf).set("name","pause_btw_reads") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br();
  //
  std::string setTMBCounterReadValues = toolbox::toString("/%s/setTMBCounterReadValues",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",setTMBCounterReadValues) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Set scan values") << std::endl ;
  *out << "TMB L1A min value (bx)" << std::endl;
  sprintf(buf,"%d",MyTest[tmb][current_crate_].getMinTmbL1aDelayValue());
  *out << cgicc::input().set("type","text").set("value",buf).set("name","tmb_l1a_delay_min") << std::endl ;
  *out << "TMB L1A max value (bx)" << std::endl;
  sprintf(buf,"%d",MyTest[tmb][current_crate_].getMaxTmbL1aDelayValue());
  *out << cgicc::input().set("type","text").set("value",buf).set("name","tmb_l1a_delay_max") << std::endl ;
  *out << "ALCT L1A min value (bx)" << std::endl;
  sprintf(buf,"%d",MyTest[tmb][current_crate_].getMinAlctL1aDelayValue());
  *out << cgicc::input().set("type","text").set("value",buf).set("name","alct_l1a_delay_min") << std::endl ;
  *out << "ALCT L1A max value (bx)" << std::endl;
  sprintf(buf,"%d",MyTest[tmb][current_crate_].getMaxAlctL1aDelayValue());
  *out << cgicc::input().set("type","text").set("value",buf).set("name","alct_l1a_delay_max") << std::endl ;
  *out << "Pause at each setting (sec)" << std::endl;
  sprintf(buf,"%d",MyTest[tmb][current_crate_].getPauseAtEachSetting());
  *out << cgicc::input().set("type","text").set("value",buf).set("name","time_to_pause") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::pre();
  *out << "3) Perform all synchronization steps by pushing the following button" << std::endl;
  *out << "   -> This button performs the synchronization steps detailed below " << std::endl;
  *out << "      in order, propagating the measured values from one step to the" << std::endl;
  *out << "      next step" << std::endl;
  *out << cgicc::pre();
  //
  std::string Automatic = toolbox::toString("/%s/Automatic",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",Automatic) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Automatic") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br();
  *out << cgicc::br();
  //
  //
  *out << cgicc::pre();
  *out << "--------------------------------------------------------------------------" << std::endl;
  *out << " Synchronization step-by-step" << std::endl;
  *out << "--------------------------------------------------------------------------" << std::endl;
  *out << "A) Measure CLCT-ALCT match timing with cosmic rays" << std::endl;
  *out << "   -> Measured values are based on current values of:" << std::endl;
  *out << "        * match_trig_window_size" << std::endl;
  *out << "        * match_trig_alct_delay" << std::endl;
  *out << "        * mpc_tx_delay" << std::endl;
  *out << "(assuming that the trigger primitives have already been synchronized at the MPC)" << std::endl;
  *out << cgicc::pre();
  //
  std::string ALCTvpf = toolbox::toString("/%s/ALCTvpf",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ALCTvpf) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Measure CLCT-ALCT match timing") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << "match_trig_alct_delay = " << MyTest[tmb][current_crate_].GetMatchTrigAlctDelayTest() 
       << " ("  << MyTest[tmb][current_crate_].GetALCTvpf_configvalue() << ") " << std::endl;
  *out << cgicc::br();
  *out << "mpc_tx_delay = " << MyTest[tmb][current_crate_].GetMpcTxDelayTest() 
       << " ("  << MyTest[tmb][current_crate_].GetMPCTxDelay_configvalue() << ") " << std::endl;
  *out << cgicc::br();
  *out << cgicc::br();
  //
  //
  *out << cgicc::pre();
  *out << "B) Measure the delay needed for the winner bit from MPC back to TMB" << std::endl;
  *out << cgicc::pre();
  //
  std::string FindWinner = toolbox::toString("/%s/FindWinner",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",FindWinner) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Determine Winner Bit Timing") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << "mpc_rx_delay = " << MyTest[tmb][current_crate_].GetMpcRxDelayTest() 
       << " ("  << MyTest[tmb][current_crate_].GetMPCdelay() << ") " << std::endl;
  *out << cgicc::br();
  *out << cgicc::br();
  //
  //
  *out << cgicc::pre();
  *out << "C) Align the L1A at the CFEB, TMB, and ALCT" << std::endl;
  *out << cgicc::pre();
  //
  std::string TmbLctCableDelay = toolbox::toString("/%s/TmbLctCableDelay",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TmbLctCableDelay) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Determine Active FEB flag (CFEB-TMB) cable delay") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << "tmb_lct_cable_delay = " << MyTest[tmb][current_crate_].GetTmbLctCableDelayTest() 
       << " ("  << MyTest[tmb][current_crate_].GetTmbLctCableDelay_configvalue() << ") " << std::endl;
  *out << cgicc::br();
  //
  std::string TMBL1aTiming = toolbox::toString("/%s/TMBL1aTiming",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBL1aTiming) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Determine TMB L1a Timing") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << "tmb_l1a_delay = " << MyTest[tmb][current_crate_].GetTmbL1aDelayTest() 
       << " ("  << MyTest[tmb][current_crate_].GetTMBL1aTiming_configvalue() << ") " << std::endl;
  *out << cgicc::br();
  //
  std::string ALCTL1aTiming = toolbox::toString("/%s/ALCTL1aTiming",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ALCTL1aTiming) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Determine ALCT L1a Timing") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << "alct_l1a_delay = " << MyTest[tmb][current_crate_].GetAlctL1aDelayTest() 
       << " ("  << MyTest[tmb][current_crate_].GetALCTL1aDelay_configvalue() << ") " << std::endl;
  *out << cgicc::br();
  *out << cgicc::br();
  //
  //
  *out << cgicc::pre();
  *out << "D) Align the Data AVailable (DAV) bits at the DMB" << std::endl;
  *out << cgicc::pre();
  //
  std::string AlctDavCableDelay = toolbox::toString("/%s/AlctDavCableDelay",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",AlctDavCableDelay) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Determine ALCT DAV (ALCT-TMB) cable delay") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << "alct_dav_cable_delay = " << MyTest[tmb][current_crate_].GetAlctDavCableDelayTest() 
       << " ("  << MyTest[tmb][current_crate_].GetAlctDavCableDelay_configvalue() << ") " << std::endl;
  *out << cgicc::br();
  *out << cgicc::br();
  //
  *out << cgicc::pre();
  *out << "-> The following parameter, cfeb_dav_cable_delay, depends on cfeb_cable_delay," << std::endl;
  *out << "   who should be set strictly according to its cable length" << std::endl;
  *out << cgicc::pre();
  //
  std::string CfebDavCableDelay = toolbox::toString("/%s/CfebDavCableDelay",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CfebDavCableDelay) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Determine CFEB DAV (CFEB-DMB) cable delay") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << "cfeb_dav_cable_delay = " << MyTest[tmb][current_crate_].GetCfebDavCableDelayTest() 
       << " ("  << MyTest[tmb][current_crate_].GetCfebDavCableDelay_configvalue() << ") " << std::endl;
  *out << cgicc::br();
  *out << cgicc::br();
  //
  *out << cgicc::pre();
  *out << "E) Check the overall state of the DMB readout" << std::endl;
  *out << "   -> Is the Active FEB Flag to L1A where it should be for this xLatency?" << std::endl;
  *out << "   -> Is the ALCT DAV scope centered at 2?" << std::endl;
  *out << "   -> Is the CFEB DAV scope centered at 2?" << std::endl;
  *out << cgicc::pre();
  //
  std::string PrintDmbValuesAndScopes = toolbox::toString("/%s/PrintDmbValuesAndScopes",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",PrintDmbValuesAndScopes) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Read DMB Values/Scopes") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br();
  //
  //
  *out << cgicc::pre();
  *out << "F) Measure the communication phase of the RPC link board to the RAT" << std::endl;
  *out << "   -> For the future, when RPC Link Boards are connected to the RAT" << std::endl;
  *out << "   -> Make sure the RPC parity-bit is enabled for the following scan" << std::endl;
  *out << cgicc::pre();
  //
  std::string RpcRatTiming = toolbox::toString("/%s/RpcRatTiming",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",RpcRatTiming) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Determine RPC-RAT phase") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << "rpc0_rat_delay = " << MyTest[tmb][current_crate_].GetRpcRatDelayTest() 
       << " ("  << MyTest[tmb][current_crate_].GetRpcRatDelay()     << ") " << std::endl;
  *out << cgicc::br();
  *out << cgicc::br();
  //
  //
  *out << cgicc::pre();
  *out << "------------------------------------------------------------------" << std::endl;
  *out << "         Other tools available pertinent to the chamber           " << std::endl;
  *out << "------------------------------------------------------------------" << std::endl;
  *out << cgicc::pre();
  //
  std::string FindDistripHotChannel = toolbox::toString("/%s/FindDistripHotChannel",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",FindDistripHotChannel) << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::input().set("type","submit").set("value","Find CLCT distrip hot channels") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  std::string ALCTScan = toolbox::toString("/%s/ALCTScan",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ALCTScan) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","ALCT Scan") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  for (int Wire = 0; Wire<(alct->GetNumberOfChannelsInAlct())/6; Wire++) 
    *out << MyTest[tmb][current_crate_].GetALCTWireScan(Wire) ;
  *out << std::endl;
  *out << cgicc::br();
  //
  std::string CFEBScan = toolbox::toString("/%s/CFEBScan",getApplicationDescriptor()->getURN().c_str());
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
      *out << MyTest[tmb][current_crate_].GetCFEBStripScan(CFEBs,HalfStrip) ;
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
  *out << cgicc::textarea().set("name","ChamberTestOutput").set("WRAP","OFF").set("rows","20").set("cols","100");
  *out << ChamberTestsOutput[tmb][current_crate_].str() << endl ;
  *out << cgicc::textarea();
  *out << cgicc::form() << std::endl ;
  //
  std::string LogChamberTestsOutput = toolbox::toString("/%s/LogChamberTestsOutput",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LogChamberTestsOutput) << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","submit").set("value","Log output").set("name","LogChamberTestsOutput") << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Clear").set("name","ClearChamberTestsOutput") << std::endl ;
  *out << cgicc::form() << std::endl ;
}
//
//
void EmuPeripheralCrateConfig::setupCoincidencePulsing(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cout << "Setup coincidence pulsing" << endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Setup Coincidence Pulsing");
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb;
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    cout << "Setup Coincidence Pulsing:  TMB " << tmb << endl;
    TMB_ = tmb;
  } else {
    cout << "Setup Coincidence Pulsing:  No tmb" << endl;
    tmb = TMB_;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].SetupCoincidencePulsing();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::TMBStartTrigger(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
void EmuPeripheralCrateConfig::EnableL1aRequest(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
void EmuPeripheralCrateConfig::ALCTTiming(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
    cout << "ALCTTiming:  DMB " << dmb << endl;
    DMB_ = dmb;
  } else {
    cout << "ALCTTiming:  No dmb" << endl;
    dmb = DMB_;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    cout << "ALCTTiming:  TMB " << tmb << endl;
    TMB_ = tmb;
  } else {
    cout << "ALCTTiming: No tmb" << endl;
    tmb = TMB_;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].ALCTTiming();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::CFEBTiming(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
    cout << "CFEBTiming:  DMB " << dmb << endl;
    DMB_ = dmb;
  } else {
    cout << "CFEBTiming:  No dmb" << endl;
    dmb = DMB_;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    cout << "CFEBTiming:  TMB " << tmb << endl;
    TMB_ = tmb;
  } else {
    cout << "CFEBTiming:  No tmb" << endl;
    tmb = TMB_;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].CFEBTiming();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::Automatic(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cout << "Perform all synchronization steps in order" << endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Automatic");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb;
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "Automatic:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "Automatic:  No tmb" << std::endl;
    tmb = TMB_;
  }
  //
  int dmb;
  name = cgi.getElement("dmb");
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "Automatic:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "Automatic:  No dmb" << std::endl;
    dmb = DMB_;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].Automatic();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::MeasureL1AsAndDAVsForCrate(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cout << "Find L1A and DAV delays for crate" << endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Find L1A and DAV delays for crate");
  //
  cgicc::Cgicc cgi(in);
  //
  for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) {
    //
    
    std::cout << "crate = " << current_crate_ << ", TMB " << i << std::endl;
    //
    MyTest[i][current_crate_].RedirectOutput(&ChamberTestsOutput[i][current_crate_]);
    MyTest[i][current_crate_].FindL1AAndDAVDelays();
    MyTest[i][current_crate_].RedirectOutput(&std::cout);
  }
  //
  this->CrateConfiguration(in,out);
  //
}
//
void EmuPeripheralCrateConfig::MeasureL1AsForCrate(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cout << "Find L1A delays for crate" << endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Find L1A delays for crate");
  //
  cgicc::Cgicc cgi(in);
  //
  for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) {
    //
    
    std::cout << "crate = " << current_crate_ << ", TMB " << i << std::endl;
    //
    MyTest[i][current_crate_].RedirectOutput(&ChamberTestsOutput[i][current_crate_]);
    MyTest[i][current_crate_].FindL1ADelays();
    MyTest[i][current_crate_].RedirectOutput(&std::cout);
  }
  //
  this->CrateConfiguration(in,out);
  //
}
//
void EmuPeripheralCrateConfig::MeasureDAVsForCrate(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cout << "Find DAV cable delays for Crate" << endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Find DAV cable delays");
  //
  cgicc::Cgicc cgi(in);
  //
  for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) {
    //
    
    std::cout << "crate = " << current_crate_ << ", TMB " << i << std::endl;
    //
    MyTest[i][current_crate_].RedirectOutput(&ChamberTestsOutput[i][current_crate_]);
    MyTest[i][current_crate_].FindDAVDelays();
    MyTest[i][current_crate_].RedirectOutput(&std::cout);
  }
  //
  this->CrateConfiguration(in,out);
  //
}
//
void EmuPeripheralCrateConfig::setTMBCounterReadValues(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb;
  cgicc::form_iterator name = cgi.getElement("tmb");
  cgicc::form_iterator name2 = cgi.getElement("tmb_l1a_delay_min");
  cgicc::form_iterator name3 = cgi.getElement("tmb_l1a_delay_max");
  cgicc::form_iterator name4 = cgi.getElement("alct_l1a_delay_min");
  cgicc::form_iterator name5 = cgi.getElement("alct_l1a_delay_max");
  cgicc::form_iterator name6 = cgi.getElement("time_to_pause");
  //
  int tmb_l1a_delay_min  = 100;
  int tmb_l1a_delay_max  = 101;
  int alct_l1a_delay_min = 102;
  int alct_l1a_delay_max = 103;
  int time_to_pause      = 11;
  //
  if(name2 != cgi.getElements().end()) 
    tmb_l1a_delay_min = strtol(cgi["tmb_l1a_delay_min"]->getValue().c_str(),NULL,10);
  if(name3 != cgi.getElements().end()) 
    tmb_l1a_delay_max = strtol(cgi["tmb_l1a_delay_max"]->getValue().c_str(),NULL,10);
  if(name4 != cgi.getElements().end()) 
    alct_l1a_delay_min = strtol(cgi["alct_l1a_delay_min"]->getValue().c_str(),NULL,10);
  if(name5 != cgi.getElements().end()) 
    alct_l1a_delay_max = strtol(cgi["alct_l1a_delay_max"]->getValue().c_str(),NULL,10);
  if(name6 != cgi.getElements().end()) 
    time_to_pause = strtol(cgi["time_to_pause"]->getValue().c_str(),NULL,10);
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "setTMBCounterReadValues: TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "setTMBCounterReadValues:  No tmb" << std::endl;
    tmb = TMB_;
  }
  //
  int dmb;
  name = cgi.getElement("dmb");
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "setTMBCounterReadValues:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "setTMBCounterReadValues:  No dmb" << std::endl;
    dmb = DMB_;
  }
  //
  MyTest[tmb][current_crate_].setMinAlctL1aDelayValue(alct_l1a_delay_min);
  MyTest[tmb][current_crate_].setMaxAlctL1aDelayValue(alct_l1a_delay_max);
  MyTest[tmb][current_crate_].setMinTmbL1aDelayValue(tmb_l1a_delay_min);
  MyTest[tmb][current_crate_].setMaxTmbL1aDelayValue(tmb_l1a_delay_max);
  MyTest[tmb][current_crate_].setPauseAtEachSetting(time_to_pause);
  //
  this->ChamberTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::setDataReadValues(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb;
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "setDataReadValues: TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "setDataReadValues:  No tmb" << std::endl;
    tmb = TMB_;
  }
  //
  int dmb;
  name = cgi.getElement("dmb");
  cgicc::form_iterator name2 = cgi.getElement("number_of_reads");
  cgicc::form_iterator name3 = cgi.getElement("pause_btw_reads");
  //
  int number_of_reads    = 1;
  int pause_btw_reads    = 1000000;
  //
  if(name2 != cgi.getElements().end()) 
    number_of_reads = strtol(cgi["number_of_reads"]->getValue().c_str(),NULL,10);
  if(name3 != cgi.getElements().end()) 
    pause_btw_reads = strtol(cgi["pause_btw_reads"]->getValue().c_str(),NULL,10);
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "SetDataReadValues:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "setDataReadValues:  No dmb" << std::endl;
    dmb = DMB_;
  }
  //
  MyTest[tmb][current_crate_].setNumberOfDataReads(number_of_reads);
  MyTest[tmb][current_crate_].setPauseBetweenDataReads(pause_btw_reads);
  //
  this->ChamberTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::TMBL1aTiming(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
    cout << "TMBL1aTiming:  DMB " << dmb << endl;
    DMB_ = dmb;
  } else {
    cout << "TMBL1aTiming:  No dmb" << endl;
    dmb = DMB_;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMBL1aTiming:  TMB " << std::dec << tmb 
	      << " scan from " << MyTest[tmb][current_crate_].getMinTmbL1aDelayValue()
	      << " to " << MyTest[tmb][current_crate_].getMaxTmbL1aDelayValue()
	      << ", pausing " << MyTest[tmb][current_crate_].getPauseAtEachSetting() << " seconds at each delay value" << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "TMBL1aTiming:  No tmb" << std::endl;
    tmb = TMB_;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].FindTMB_L1A_delay();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::ALCTL1aTiming(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
    cout << "ALCTL1aTiming:  DMB " << dmb << endl;
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
    std::cout << "ALCTL1aTiming:  TMB " << std::dec << tmb 
	      << " scan from " << MyTest[tmb][current_crate_].getMinAlctL1aDelayValue()
	      << " to " << MyTest[tmb][current_crate_].getMaxAlctL1aDelayValue()
	      << ", pausing " << MyTest[tmb][current_crate_].getPauseAtEachSetting() << " seconds at each delay value" << std::endl;
    TMB_ = tmb;
  } else {
    cout << "No tmb" << endl;
    tmb = TMB_;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].FindALCT_L1A_delay();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::ALCTvpf(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cout << "EmuPeripheralCrate:  ALCTvpf" << endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "ALCTvpf");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb;
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    cout << "ALCTvpf:  TMB " << tmb << " will read TMB Raw Hits "
	 << MyTest[tmb][current_crate_].getNumberOfDataReads() << " times" << std::endl;
    TMB_ = tmb;
  } else {
    cout << "ALCTvpf:  No tmb" << endl;
  }
  //
  int dmb;
  name = cgi.getElement("dmb");
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    cout << "ALCTvpf:  DMB " << dmb << endl;
    DMB_ = dmb;
  } else {
    cout << "ALCTvpf:  No dmb" << endl;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].FindALCTinCLCTMatchWindow();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
}
//
void EmuPeripheralCrateConfig::ALCTScan(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
    cout << "ALCTScan:  DMB " << dmb << endl;
    DMB_ = dmb;
  } else {
    cout << "ALCTScan:  No dmb" << endl;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    cout << "ALCTScan:  TMB " << tmb << endl;
    TMB_ = tmb;
  } else {
    cout << "ALCTScan:  No tmb" << endl;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].ALCTChamberScan();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::CFEBScan(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
    cout << "CFEBScan:  DMB " << dmb << endl;
    DMB_ = dmb;
  } else {
    cout << "CFEBScan:  No dmb" << endl;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    cout << "CFEBScan:  TMB " << tmb << endl;
    TMB_ = tmb;
  } else {
    cout << "CFEBScan:  No tmb" << endl;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].CFEBChamberScan();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::FindDistripHotChannel(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cout << "Find CLCT Distrip Hot Channels" << endl;
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb, dmb;
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  if (name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    cout << "FindDistripHotChannel:  TMB " << tmb << endl;
    TMB_ = tmb;
  } else {
    cout << "FindDistripHotChannel:  No tmb" << endl;
  }
  //
  name = cgi.getElement("dmb");
  //
  if (name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    cout << "FindDistripHotChannel:  DMB " << dmb << endl;
    DMB_ = dmb;
  } else {
    cout << "FindDistripHotChannel:  No dmb" << endl;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].FindDistripHotChannels();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::FindWinner(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cout << "EmuPeripheralCrate:  FindWinner" << endl;
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
    cout << "FindWinner:  DMB " << dmb << endl;
    DMB_ = dmb;
  } else {
    cout << "FindWinner:  No dmb" << endl;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    cout << "FindWinner:  TMB " << tmb 
	 << ", pausing " << MyTest[tmb][current_crate_].getPauseAtEachSetting() 
	 << " seconds at each delay value" << std::endl;
    TMB_ = tmb;
  } else {
    cout << "FindWinner:  No tmb" << endl;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].FindWinner();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
}
//
void EmuPeripheralCrateConfig::AlctDavCableDelay(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cout << "Measure ALCT DAV cable delay" << endl;
  //    LOG4CPLUS_INFO(getApplicationLogger(), "Measure ALCT DAV cable delay");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb, dmb;
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    cout << "AlctDavCableDelay:  TMB " << tmb << endl;
    TMB_ = tmb;
  } else {
    cout << "AlctDavCableDelay:  No tmb" << endl;
  }
  //
  name = cgi.getElement("dmb");
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    cout << "AlctDavCableDelay:  DMB " << dmb 
	 << " Read " << MyTest[tmb][current_crate_].getNumberOfDataReads()
	 << " times, pausing " << MyTest[tmb][current_crate_].getPauseBetweenDataReads() 
	 << "usec between each read" << std::endl;
    DMB_ = dmb;
  } else {
    cout << "AlctDavCableDelay:  No dmb" << endl;
  }
  //
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].MeasureAlctDavCableDelay();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::CfebDavCableDelay(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cout << "Measure CFEB DAV cable delay" << endl;
  //    LOG4CPLUS_INFO(getApplicationLogger(), "Measure CFEB DAV cable delay");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb, dmb;
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    cout << "CfebDavCableDelay:  TMB " << tmb << endl;
    TMB_ = tmb;
  } else {
    cout << "CfebDavCableDelay:  No tmb" << endl;
  }
  //
  name = cgi.getElement("dmb");
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    cout << "CfebDavCableDelay:  DMB " << dmb 
	 << " Read " << MyTest[tmb][current_crate_].getNumberOfDataReads()
	 << " times, pausing " << MyTest[tmb][current_crate_].getPauseBetweenDataReads() 
	 << "usec between each read" << std::endl;
    DMB_ = dmb;
  } else {
    cout << "CfebDavCableDelay:  No dmb" << endl;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    cout << "CfebDavCableDelay:  TMB " << tmb << endl;
    TMB_ = tmb;
  } else {
    cout << "CfebDavCableDelay:  No tmb" << endl;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].MeasureCfebDavCableDelay();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::TmbLctCableDelay(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cout << "Measure TMB LCT cable delay" << endl;
  //    LOG4CPLUS_INFO(getApplicationLogger(), "Measure TMB LCT cable delay");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb, dmb;
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    cout << "TmbLctCableDelay:  TMB " << tmb << endl;
    TMB_ = tmb;
  } else {
    cout << "TmbLctCableDelay:  No tmb" << endl;
  }
  //
  name = cgi.getElement("dmb");
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "TmbLctCableDelay:  DMB " << dmb << std::dec
	      << " Read " << MyTest[tmb][current_crate_].getNumberOfDataReads()
	      << " times, pausing " << MyTest[tmb][current_crate_].getPauseBetweenDataReads() 
	      << "usec between each read" << std::endl;
    DMB_ = dmb;
  } else {
    cout << "TmbLctCableDelay:  No dmb" << endl;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].MeasureTmbLctCableDelay();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::PrintDmbValuesAndScopes(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cout << "Print All DMB Values and Scopes" << endl;
  //    LOG4CPLUS_INFO(getApplicationLogger(), "Print All DMB Values and Scopes");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb, dmb;
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    cout << "PrintDmbValuesAndScopes:  DMB " << dmb << endl;
    DMB_ = dmb;
  } else {
    cout << "PrintDmbValuesAndScopes:  No dmb" << endl;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    cout << "PrintDmbValuesAndScopes:  TMB " << tmb << endl;
    TMB_ = tmb;
  } else {
    cout << "PrintDmbValuesAndScopes:  No tmb" << endl;
  }
  //
  MyTest[tmb][current_crate_].ReadAllDmbValuesAndScopes();
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].PrintAllDmbValuesAndScopes();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::RatTmbTiming(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cout << "RatTmbTiming" << endl;
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    cout << "RatTmbTiming:  TMB " << tmb << endl;
    TMB_ = tmb;
  } else {
    cout << "RatTmbTiming:  No tmb" << endl;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].RatTmbDelayScan();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::RpcRatTiming(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cout << "RpcRatTiming" << endl;
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    cout << "RpcRatTiming:  TMB " << tmb << endl;
    TMB_ = tmb;
  } else {
    cout << "RpcRatTiming:  No tmb" << endl;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].RpcRatDelayScan();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
///////////////////////////////////////////////////////////
// DMB utilities
///////////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::CFEBStatus(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
  bool isME13 = false;
  TMB * thisTMB   = tmbVector[dmb];
  ALCTController * thisALCT;
  if (thisTMB) 
    thisALCT = thisTMB->alctController();
  if (thisALCT) 
    if ( (thisALCT->GetChamberType()).find("ME13") != string::npos )
      isME13 = true;
  //
  Chamber * thisChamber = chamberVector[dmb];
  //
  char Name[100];
  sprintf(Name,"%s CFEB status, crate=%s, DMBslot=%d",
	  (thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),thisDMB->slot());
  //
  MyHeader(in,out,Name);
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
    if (cfebItr == cfebs.begin() && isME13) {
      *out << cgicc::span().set("style","color:black");
      *out << buf;
      *out << cgicc::span();
    } else if ( thisDMB->febfpgauser(*cfebItr) == EXPECTED_CFEB_USERID ) {
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
void EmuPeripheralCrateConfig::DMBUtils(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  int dmb;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    cout << "DMBUtils:  DMB " << dmb << endl;
    DMB_ = dmb;
  } else {
    cout << "DMBUtils:  No dmb" << endl ;
    dmb = DMB_;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  Chamber * thisChamber = chamberVector[dmb];
  //
  char Name[100];
  sprintf(Name,"%s DMB utilities, crate=%s, slot=%d",(thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),thisDMB->slot());
  //
  MyHeader(in,out,Name);
  //
  *out << cgicc::h1(Name);
  *out << cgicc::br();
  //
  char buf[200] ;
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << endl ;
  //
  *out << cgicc::legend("DMB Utils").set("style","color:blue") ;
  //
  //    *out << cgicc::table().set("border","1");
  //
  // *out << cgicc::td();
  //
  std::string DMBTurnOff = toolbox::toString("/%s/DMBTurnOff",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBTurnOff) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","DMB Turn Off LV") << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br();
  //
  std::string DMBTurnOn = toolbox::toString("/%s/DMBTurnOn",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBTurnOn) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","DMB Turn On LV") << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br();
  //
  std::string DMBPrintCounters = toolbox::toString("/%s/DMBPrintCounters",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBPrintCounters) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","DMB Print Counters") << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::fieldset();
  //  
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << endl ;
  //
  *out << cgicc::legend("DMB/CFEB Load PROM").set("style","color:red") ;
  //
  std::string DMBFirmware = FirmwareDir_+DMB_FIRMWARE_FILENAME;
  DMBFirmware_ = DMBFirmware;
  std::string DMBVmeFirmware = FirmwareDir_+DMBVME_FIRMWARE_FILENAME;
  DMBVmeFirmware_ = DMBVmeFirmware;
  //
  std::string DMBLoadFirmware = toolbox::toString("/%s/DMBLoadFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBLoadFirmware) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","DMB CONTROL Load Firmware") << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br();
  //
  std::string DMBVmeLoadFirmware = toolbox::toString("/%s/DMBVmeLoadFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBVmeLoadFirmware) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","DMB Vme Load Firmware") << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br();
  //
  std::string DMBVmeLoadFirmwareEmergency = toolbox::toString("/%s/DMBVmeLoadFirmwareEmergency",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBVmeLoadFirmwareEmergency) << std::endl ;
  *out << "DMB Board Number:";
  *out <<cgicc::input().set("type","text").set("value","0").set("name","DMBNumber")<<std::endl;
  *out << cgicc::input().set("type","submit").set("value","DMB Vme Load Firmware (Emergency)") << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br();
  //
  std::string CFEBFirmware = FirmwareDir_+CFEB_FIRMWARE_FILENAME;
  CFEBFirmware_ = CFEBFirmware;
  //
  std::string CFEBLoadFirmware = toolbox::toString("/%s/CFEBLoadFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CFEBLoadFirmware) << std::endl ;
  *out << "CFEB to download (0-4), (-1 == all) : ";
  *out << cgicc::input().set("type","text").set("value","-1").set("name","DMBNumber") << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","CFEB Load Firmware") << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br();
  //
  std::string CFEBLoadFirmwareID = toolbox::toString("/%s/CFEBLoadFirmwareID",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CFEBLoadFirmwareID) << std::endl ;
  *out << "CFEB to download (0-4):";
  *out << cgicc::input().set("type","text").set("value","-1").set("name","DMBNumber");
  *out << " Board Serial_Number:";
  *out << cgicc::input().set("type","text").set("value","0").set("name","CFEBSerialNumber")<<std::endl;
  *out << cgicc::input().set("type","submit").set("value","CFEB Load Firmware/Serial Number recovery") << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::fieldset();
  //
}
//
void EmuPeripheralCrateConfig::DMBTurnOff(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    cout << "DMBTurnOff:  DMB " << dmb << endl;
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
void EmuPeripheralCrateConfig::DMBLoadFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    cout << "DMBLoadFirmware:  DMB " << dmb << endl;
    DMB_ = dmb;
  }
  //
  thisCCB->hardReset();
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
  ::sleep(5);
  thisCCB->hardReset();
  //
  this->DMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::DMBVmeLoadFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    cout << "DMBVmeLoadFirmware:  DMB " << dmb << endl;
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
      dword[1]=0xdb00;
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
void EmuPeripheralCrateConfig::DMBVmeLoadFirmwareEmergency(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  int dmbNumber = 0;
  //
  cgicc::form_iterator name2 = cgi.getElement("DMBNumber");
  //int registerValue = -1;
  if(name2 != cgi.getElements().end()) {
    dmbNumber = cgi["DMBNumber"]->getIntegerValue();
  }
  //
  std::cout << "Loading DMB# " <<dmbNumber << std::endl ;
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
  if (thisDMB->slot() == 25) { 
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

    std::string crate=thisCrate->GetLabel();
    int slot=thisDMB->slot();
    int dmbID=brddb->CrateToDMBID(crate,slot);
    dword[0]=dmbNumber&0x03ff;
    dword[1]=0xDB00;
    if (((dmbNumber&0xfff)==0)||((dmbNumber&0xfff)==0xfff)) dword[0]=dmbID&0x03ff;

    cout<<" The DMB number is set to: "<<dword[0]<<" Entered: "<<dmbNumber<<" Database lookup: "<<dmbID<<endl;
    char * outp=(char *)dword;  
    thisDMB->epromload(RESET,DMBVmeFirmware_.toString().c_str(),1,outp);  // load mprom
  }
  ::sleep(1);
  thisCCB->hardReset();
  //
  this->DMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::CFEBLoadFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
void EmuPeripheralCrateConfig::CFEBLoadFirmwareID(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  LOG4CPLUS_INFO(getApplicationLogger(),"Started CFEB firmware download with Board_number");
  //
  cgicc::Cgicc cgi(in);
  //
  int dmbNumber = -1;
  //
  cgicc::form_iterator name2 = cgi.getElement("DMBNumber");
  //int registerValue = -1;
  if(name2 != cgi.getElements().end()) {
    dmbNumber = cgi["DMBNumber"]->getIntegerValue();
  }
  //
  int cfebSerialNumber = 0;
  //
  cgicc::form_iterator name3 = cgi.getElement("CFEBSerialNumber");
  //int registerValue = -1;
  if(name3 != cgi.getElements().end()) {
    cfebSerialNumber = cgi["CFEBSerialNumber"]->getIntegerValue();
  }
  //
  std::cout << "Loading CFEBNumber " <<dmbNumber << " with serial number: "<<cfebSerialNumber<<std::endl ;
  if (cfebSerialNumber>2600 ||cfebSerialNumber<1 ||
      dmbNumber>4 || dmbNumber<0) {
    std::cout<<"Invalid cfeb number, or serial number"<<std::endl;
    return;
  }
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
  if ((thisDMB->slot() >21) || (thisDMB->slot() <3)){
    std::cout<<" Invalid DMB slot for CFEB Number reloading "<<thisDMB->slot()<<std::endl;
    return;
  }
  //
  cout << "CFEBLoadFirmware - DMB " << dmb << endl;
  //
  //    thisCCB->hardReset();
  //
  if (thisDMB) {
    //
    vector<CFEB> thisCFEBs = thisDMB->cfebs();
    //
    ::sleep(1);
    //
    ostringstream dum;
    dum << "loading CFEB firmware for DMB=" << dmb << " CFEB="<< dmbNumber << std::endl;
    LOG4CPLUS_INFO(getApplicationLogger(), dum.str());
    for (unsigned int i=0; i<thisCFEBs.size(); i++) {
      if (thisCFEBs[i].number() == dmbNumber ) {
	std::cout <<" ThisCFEB[i].promdevice: "<<thisCFEBs[i].promDevice()<<std::endl;
	//force CFEB device switch
	unsigned short int dword[2];
	dword[0]=thisDMB->febpromuser(thisCFEBs[4-i]);
	dword[0]=cfebSerialNumber;
	dword[1]=0xCFEB;
	char * outp=(char *)dword;   // recast dword
	
	thisDMB->epromload(thisCFEBs[i].promDevice(),CFEBFirmware_.toString().c_str(),1,outp);
      }
    }
  }
  //    ::sleep(1);
  //    thisCCB->hardReset();
  //
  this->DMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::DMBTurnOn(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
//////////////////////////////////////////////////////////////////////////////////////
// TMB utilities
//////////////////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::TMBPrintCounters(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
void EmuPeripheralCrateConfig::TMBResetCounters(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
void EmuPeripheralCrateConfig::TMBCounterForFixedTime(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    TMB_ = tmb;
  }
  //
  int wait_time = 10;
  if(name != cgi.getElements().end()) {
    wait_time = cgi["time_to_wait"]->getIntegerValue();
  }
  //
  cout << "Read TMB " << tmb << " counters over " << wait_time << " seconds" << endl;
  //
  TMB * thisTMB = tmbVector[tmb];
  thisTMB->ResetCounters();
  //
  ::sleep(wait_time);
  //
  thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
  thisTMB->GetCounters();
  thisTMB->PrintCounters();
  thisTMB->RedirectOutput(&std::cout);
  //
  this->TMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::TriggerTestInjectALCT(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
void EmuPeripheralCrateConfig::armScope(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
void EmuPeripheralCrateConfig::forceScope(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
void EmuPeripheralCrateConfig::readoutScope(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
void EmuPeripheralCrateConfig::TriggerTestInjectCLCT(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
void EmuPeripheralCrateConfig::TMBDumpAllRegisters(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
void EmuPeripheralCrateConfig::TMBClearUserProms(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    cout << "TMB " << tmb << endl;
    TMB_ = tmb;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
  thisTMB->CheckAndProgramProm(ChipLocationTmbUserPromTMBClear);
  thisTMB->CheckAndProgramProm(ChipLocationTmbUserPromALCTClear);
  thisTMB->RedirectOutput(&std::cout);
  //
  this->TMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::TMBConfigure(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    cout << "Configure:  TMB " << tmb << endl;
    TMB_ = tmb;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  ALCTController * alct = thisTMB->alctController();
  thisTMB->configure();
  alct->configure();
  //
  this->TMBUtils(in,out);
  //
}
// 
void EmuPeripheralCrateConfig::TMBReadConfiguration(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
void EmuPeripheralCrateConfig::TMBCheckConfiguration(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
void EmuPeripheralCrateConfig::TMBReadStateMachines(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
void EmuPeripheralCrateConfig::TMBRawHits(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    cout << "TMBRawHits:  TMB " << tmb << endl;
    TMB_ = tmb;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  //
  thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
  thisTMB->TMBRawhits();
  thisTMB->RedirectOutput(&std::cout);
  //
  this->TMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::ALCTRawHits(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    cout << "ALCTRawHits:  TMB " << tmb << endl;
    TMB_ = tmb;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  //
  thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
  thisTMB->ALCTRawhits();
  thisTMB->RedirectOutput(&std::cout);
  //
  this->TMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::TMBCheckStateMachines(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
void EmuPeripheralCrateConfig::ALCTStatus(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
  Chamber * thisChamber = chamberVector[tmb];
  //
  char Name[100];
  sprintf(Name,"%s ALCT status, crate=%s, TMBslot=%d",
	  (thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),tmbVector[tmb]->slot());

  MyHeader(in,out,Name);
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << std::endl;
  //
  *out << cgicc::legend("ALCT Firmware Status").set("style","color:blue") << cgicc::p() << std::endl ;
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
  //
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  //
  *out << cgicc::legend("Voltages, Currents, and Temperatures").set("style","color:blue") 
       << cgicc::p() << std::endl ;
  //
  alct->ReadAlctTemperatureAndVoltages();
  //
  alct->RedirectOutput(out);
  alct->PrintAlctTemperature();
  alct->RedirectOutput(&std::cout);
  //
  *out << cgicc::br();    
  //
  *out << cgicc::table().set("border","1").set("cellspacing","1").set("cellpadding","8");
  //
  /////////////////////////////////////////////////
  *out << cgicc::td().set("ALIGN","center");
  *out << "power line" << endl;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << "+3.3V" << endl;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","center");  
  *out << "+1.8V" << endl;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << "+5.5V B" << endl;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << "+5.5V A" << endl;
  *out << cgicc::td();
  //
  /////////////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","center");  
  *out << "measured V" << endl;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  float value_3p3volt = alct->GetAlct_3p3_Voltage();
  if ( value_3p3volt < 3.3*0.95 ||
       value_3p3volt > 3.3*1.05 ) {
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");  
  }
  *out << std::setprecision(2) << value_3p3volt << "V";
  *out << cgicc::span();
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  float value_1p8volt = alct->GetAlct_1p8_Voltage();
  if ( value_1p8volt < 1.8*0.95 ||
       value_1p8volt > 1.8*1.95 ) {
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");  
  }
  *out << std::setprecision(2) << value_1p8volt << "V";
  *out << cgicc::span();
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  float value_5p5voltb = alct->GetAlct_5p5b_Voltage();
  // The +5.5V_B ADC inputs on ALCT288 boards are shorted to ground, hence, the ADC values are not useful
  if ( alct->GetNumberOfAfebs() <= 18 ) {
    *out << cgicc::span().set("style","color:black");
  } else if ( value_5p5voltb < 5.5*0.95 ||
	      value_5p5voltb > 5.5*1.05 ) {
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");  
  }
  // The +5.5V_B ADC inputs on ALCT288 boards are shorted to ground, hence, the ADC values are not useful
  if ( alct->GetNumberOfAfebs() <= 18 ) {
    *out << "XXXXX";
  } else {
    *out << std::setprecision(2) << value_5p5voltb << " V";
  }
  *out << cgicc::span();
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  float value_5p5volta = alct->GetAlct_5p5a_Voltage();
  if ( value_5p5volta < 5.5*0.95 ||
       value_5p5volta > 5.5*1.05 ) {
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");  
  }
  *out << std::setprecision(2) << value_5p5volta << " V" ;
  *out << cgicc::span();
  *out << cgicc::td();
  //
  //////////////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << "measured I" << endl;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  float value_3p3amps = alct->GetAlct_3p3_Current();
  if ( value_3p3amps < 3.3*0.95 ||
       value_3p3amps > 3.3*1.05 ) {
    *out << cgicc::span().set("style","color:black");
  } else {
    *out << cgicc::span().set("style","color:black");  
  }
  *out << std::setprecision(2) << value_3p3amps << "A" ;
  *out << cgicc::span();
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  //    float value_1p8amps = thisDMB->lowv_adc(3,0)/1000.;
  float value_1p8amps = alct->GetAlct_1p8_Current();
  //
  if ( value_1p8amps < 1.8*0.95 ||
       value_1p8amps > 1.8*1.95 ) {
    *out << cgicc::span().set("style","color:black");
  } else {
    *out << cgicc::span().set("style","color:black");  
  }
  *out << std::setprecision(2) << value_1p8amps << "A" ;
  *out << cgicc::span();
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","center");
  //    float value_5p5ampsb = thisDMB->lowv_adc(3,2)/1000.;
  float value_5p5ampsb = alct->GetAlct_5p5b_Current();
  // The +5.5V_B ADC inputs on ALCT288 boards are shorted to ground, hence, the ADC values are not useful...
  if ( alct->GetNumberOfAfebs() <= 18 ) {
    *out << cgicc::span().set("style","color:black");
  } else if ( value_5p5ampsb < 5.5*0.95 ||
	      value_5p5ampsb > 5.5*1.05 ) {
    *out << cgicc::span().set("style","color:black");
  } else {
    *out << cgicc::span().set("style","color:black");  
  }
  if ( alct->GetNumberOfAfebs() <= 18 ) {
    *out << "XXXXX" ;
  } else {
    *out << std::setprecision(2) << value_5p5ampsb << " A" ;
  }
  *out << cgicc::span();
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  float value_5p5ampsa = alct->GetAlct_5p5a_Current();
  if ( value_5p5ampsa < 5.5*0.95 ||
       value_5p5ampsa > 5.5*1.05 ) {
    *out << cgicc::span().set("style","color:black");
  } else {
    *out << cgicc::span().set("style","color:black");  
  }
  *out << std::setprecision(2) << value_5p5ampsa << " A" ;
  *out << cgicc::span();
  *out << cgicc::td();
  //
  *out << cgicc::table();
  //
  *out << cgicc::fieldset();
  *out << std::endl;
  //
}
//
void EmuPeripheralCrateConfig::RATStatus(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
  Chamber * thisChamber = chamberVector[tmb];
  //
  char Name[100];
  sprintf(Name,"%s RAT status, crate=%s, TMBslot=%d",
	  (thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),tmbVector[tmb]->slot());
  //
  MyHeader(in,out,Name);
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << std::endl;
  //
  *out << cgicc::legend("RAT Status").set("style","color:blue") << cgicc::p() << std::endl ;
  //
  *out << cgicc::br();
  //
  *out << cgicc::pre();
  //
  tmbTestVector[tmb].testRATuserCodes();
  //
  if ( tmbTestVector[tmb].GetResultTestRATuserCodes() == 1 ) {
    *out << cgicc::span().set("style","color:green");
  } else {
    *out << cgicc::span().set("style","color:red");
  }
  rat->RedirectOutput(out);
  rat->ReadRatUser1();
  rat->PrintRatUser1();
  rat->RedirectOutput(&std::cout);
  //
  *out << cgicc::span();
  *out << cgicc::pre();
  //
  *out << cgicc::fieldset();
  //
}
//
/////////////////////////////////////////////////////////
// CCB utilities
/////////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::CCBStatus(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  char Name[100];
  sprintf(Name,"CCB status, crate=%s, slot=%d",ThisCrateID_.c_str(),thisCCB->slot());
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
  *out << cgicc::br() << "CCB Mode = ";
  int ccb_mode = thisCCB->GetCCBmode();
  switch(ccb_mode) {
  case CCB::TTCrqFPGA:
    *out << "TTCrgFPGA" << endl;
    break;
  case CCB::VMEFPGA:
    *out << "VMEFPGA" << endl;
    break;
  case CCB::DLOG:
    *out << "DLOG" << endl;
    break;
  default:
    *out << "unknown" << endl;
    break;
  }
  //
  *out << cgicc::br() << "CSRA1 =  " << std::hex << thisCCB->ReadRegister(0) << endl;
  *out << cgicc::br() << "CSRA2 =  " << std::hex << thisCCB->ReadRegister(2) << endl;
  *out << cgicc::br() << "CSRA3 =  " << std::hex << thisCCB->ReadRegister(4) << endl;
  *out << cgicc::br() << "CSRB1 =  " << std::hex << thisCCB->ReadRegister(0x20) << endl;
  *out << cgicc::br() << "CSRB18 = " << std::hex << thisCCB->ReadRegister(0x42) << endl;
  //
  *out << cgicc::fieldset();
  //
}
//
void EmuPeripheralCrateConfig::ControllerUtils(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  char Name[50] ;
  sprintf(Name,"Controller Utils slot 1");
  //
  MyHeader(in,out,Name);
  //
  std::string EnableDisableDebug =
    toolbox::toString("/%s/EnableDisableDebug",getApplicationDescriptor()->getURN().c_str());
  //
  *out << cgicc::form().set("method","GET").set("action",EnableDisableDebug) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Enable/Disable Debug") << std::endl ;
  //
  if ( thisCrate->vmeController()->GetDebug() == 0 ) {
    *out << "Debug disabled";
  } else {
    *out << "Debug enabled";
  }
  //
  *out << cgicc::form() << cgicc::br() << std::endl ;
  //
  //
    std::string ReadVMECCRegisters =
      toolbox::toString("/%s/ReadVMECCRegisters",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",ReadVMECCRegisters)
	 << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Read Registers") 
	 << std::endl ;
    *out << cgicc::form() << cgicc::br() << std::endl ;
    //
    std::string VMECCLoadFirmware =
      toolbox::toString("/%s/VMECCLoadFirmware",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",VMECCLoadFirmware)
	 << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Load Firmware") 
	 << std::endl ;
    *out << cgicc::form() << cgicc::br() << std::endl ;
    //
  std::string ExcludeIncludeCrate =
    toolbox::toString("/%s/ExcludeIncludeCrate",getApplicationDescriptor()->getURN().c_str());
  //
  *out << cgicc::form().set("method","GET").set("action",ExcludeIncludeCrate) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Exclude/Include Crate") << std::endl ;
  //
  if ( thisCrate->IsAlive() ) {
    *out << "Crate Included";
  } else {
    *out << "Crate Excluded";
  }  
  *out << cgicc::form() << std::endl ;
  //
}
//
void EmuPeripheralCrateConfig::EnableDisableDebug(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
}

void EmuPeripheralCrateConfig::ExcludeIncludeCrate(xgi::Input * in, xgi::Output * out ) 
 throw (xgi::exception::Exception) {
  	   
  if ( thisCrate->IsAlive() ) {
     std::cout << thisCrate->GetLabel() << " Crate excluded out" << std::endl;
     thisCrate->SetLife(false);
  } else {
     std::cout << thisCrate->GetLabel()  << "Crate included back" << std::endl;
     thisCrate->SetLife(true);
  }
  	   
  this->ControllerUtils(in,out);
}

//
  void EmuPeripheralCrateConfig::ReadVMECCRegisters(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    char buf[50];
   
    thisCrate->vmeController()->init();

    sprintf(buf," entered READVMECCRegisters \n");

    char Name[300];
    sprintf(Name,"VMECC CrateID %s \n ",(thisCrate->GetLabel()).c_str());

//    thisCrate->vmeController()->write_Ethernet_CR(0x0050);
//    thisCrate->vmeController()->write_ResetMisc_CR(0x031B);

    //
    MyHeader(in,out,Name);
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend("VCC IDs").set("style","color:blue") << cgicc::p() << std::endl ;
    //
    *out << cgicc::pre();
    //

    std::cout<<buf<<std::endl;
    sprintf(buf," read_dev_id %08x \n",thisCrate->vmeController()->read_dev_id());
    std::cout<<buf<<std::endl;
    *out << buf ;
    *out << cgicc::br();
    sprintf(buf," read_user_code %08x \n",thisCrate->vmeController()->read_user_code());
    std::cout<<buf<<std::endl;
    *out << buf ;
    *out << cgicc::br();
    char *tmp_cp = thisCrate->vmeController()->read_customer_code();
    if(tmp_cp!=0){
    	sprintf(buf,"PROM currently programmed with %s\n",tmp_cp);
        std::cout<<buf<<std::endl;
        *out << buf ;
        *out << cgicc::br();
     }
      else {
    	sprintf(buf,"Did not receive customer code. Proceeding anyway\n");
        std::cout<<buf<<std::endl;
        *out << buf ;
        *out << cgicc::br();
     }
    *out << std::endl;
    *out << cgicc::fieldset();
    *out << std::endl;

    // dump VCC config info (into stdout log)
    thisCrate->vmeController()->vcc_dump_config();
    //
    //   this->ControllerUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrateConfig::VMECCLoadFirmware(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    thisCrate->vmeController()->init();

    thisCrate->vmeController()->write_Ethernet_CR(0x0050);
    thisCrate->vmeController()->write_ResetMisc_CR(0x031B);

    char buf[50];
    sprintf(buf," Entered VMECCLoadFirmware \n");
    std::cout<<buf<<std::endl;
    std::string VMECCFirmware = FirmwareDir_+VMECC_FIRMWARE_DIR;
    VMECCFirmwareDir_=VMECCFirmware;
   std::cout << " firmware dir: " << VMECCFirmwareDir_ .toString()<< std::endl;
   std::string VMECCFirmwareD=VMECC_FIRMWARE_VER;
    VMECCFirmwareVer_=VMECCFirmwareD;
    std::cout << " firmware ver: " << VMECCFirmwareVer_ .toString()<< std::endl;
    thisCrate->vmeController()->prg_vcc_prom_ver(VMECCFirmwareDir_.toString().c_str(),VMECCFirmwareVer_.toString().c_str());

    // thisCrate->vmeController()->write_Ethernet_CR(0x0010);
    // thisCrate->vmeController()->write_ResetMisc_CR(0x001B);

    this->ControllerUtils(in,out);
    //
  }
//
void EmuPeripheralCrateConfig::CCBUtils(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  char Name[100];
  sprintf(Name,"CCB utilities, crate=%s, slot=%d",ThisCrateID_.c_str(),thisCCB->slot());
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
  *out << cgicc::form().set("method","GET").set("action",ReadCCBRegister) << std::endl ;
  *out << "Read Register (hex)..." << std:: endl;
  *out << cgicc::input().set("type","text").set("value","0").set("name","CCBRegister") << std::endl ;
  *out << "Register value : (hex) " << std::hex << CCBRegisterValue_ << std::endl;
  *out << cgicc::form() << std::endl ;
  //
  std::string HardReset =
    toolbox::toString("/%s/HardReset",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",HardReset) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","HardReset");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br();
  std::string ReadTTCRegister =
    toolbox::toString("/%s/ReadTTCRegister",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ReadTTCRegister) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","ReadTTCRegister");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br();
  *out << cgicc::br();
  //
  //  CCBFirmware_ = FirmwareDir_+"ccb/"+"ccb2004p_021508.svf";
  // we use the 5 march 2007 version for commissioning:
  CCBFirmware_ = FirmwareDir_+"ccb/"+"ccb2004p_030507.svf";
  //
  std::string CCBLoadFirmware =
    toolbox::toString("/%s/CCBLoadFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CCBLoadFirmware) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Load CCB Firmware") << std::endl ;
  //    sprintf(buf,"%d",ccb);
  //    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << CCBFirmware_.toString();
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::fieldset();
  //
}
//
  void EmuPeripheralCrateConfig::CCBLoadFirmware(xgi::Input * in, xgi::Output * out ) 
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
    //    thisCCB->hardReset();
    //
    this->CCBUtils(in,out);
    //
  }
  //
//////////////////////////////////////////////////
// MPC methods
//////////////////////////////////////////////////
void EmuPeripheralCrateConfig::MPCBoardID(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  MPCBoardID_= cgi["MPCBoardID"]->getValue() ;
  //
  this->CrateConfiguration(in,out);
  //
}
//
void EmuPeripheralCrateConfig::MPCStatus(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  char Name[100];
  sprintf(Name,"MPC status, crate=%s, slot=%d",
	  ThisCrateID_.c_str(),thisMPC->slot());
  //
  MyHeader(in,out,Name);
  //
  *out << cgicc::h1(Name);
  *out << cgicc::br();
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  //
  *out << cgicc::legend("MPC Info").set("style","color:blue") << cgicc::p() << std::endl ;
  //
  thisMPC->RedirectOutput(out);
  thisMPC->firmwareVersion();
  thisMPC->RedirectOutput(&std::cout);
  //
  *out << cgicc::br() << "CSR0 = " << std::hex << thisMPC->ReadRegister(0) << endl;
  //
  *out << cgicc::fieldset();
  //
}
//
void EmuPeripheralCrateConfig::MPCUtils(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  char Name[100];
  sprintf(Name,"MPC utilities, crate=%s, slot=%d",
	  ThisCrateID_.c_str(),thisMPC->slot());
  //
  MyHeader(in,out,Name);
  //
  *out << cgicc::h1(Name);
  *out << cgicc::br();
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  //
  *out << cgicc::legend("MPC Utils").set("style","color:blue") << cgicc::p() << std::endl ;
  //
  MPCFirmware_ = FirmwareDir_+"mpc/"+"mpc2004_102706.svf";
  //
  std::string MPCLoadFirmware = toolbox::toString("/%s/MPCLoadFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",MPCLoadFirmware) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Load MPC Firmware") << std::endl ;
  *out << MPCFirmware_.toString();
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::fieldset();
  //
}
//
void EmuPeripheralCrateConfig::MPCLoadFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
  //thisCCB->hardReset();
  //
  this->MPCUtils(in,out);
  //
}

//////////////////////////////////////////////////////////////////////////////
// TMB tests methods
//////////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::TMBTests(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb;
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    cout << "TMBTests: TMB " << tmb << endl;
    TMB_ = tmb;
  } else {
    cout << "TMBTests: No tmb" << endl ;
    tmb = TMB_;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  //
  Chamber * thisChamber = chamberVector[tmb];
  //
  char Name[100];
  sprintf(Name,"%s TMB tests, %s slot=%d",
	  (thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),thisTMB->slot());
  //
  MyHeader(in,out,Name);
  //
  char buf[20];
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << endl ;
  //
  std::string RunAllTests = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",RunAllTests) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Run All TMB tests").set("style","color:blue") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","0").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  //
  //
  *out << cgicc::table().set("border","1");
  //
  ///////////////////////////////////////////
  *out << cgicc::td().set("ALIGN","center");
  std::string testBootRegister = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testBootRegister) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestBootRegister() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test Boot register").set("style","color:blue" ) << std::endl ;
    //
  }  else if ( tmbTestVector[tmb].GetResultTestBootRegister() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test Boot register").set("style","color:green") << std::endl ;
    //
  } else {
    *out << cgicc::input().set("type","submit").set("value","TMB test Boot register").set("style","color:red"  ) << std::endl ;
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","1").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  std::string testFirmwareDate = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testFirmwareDate) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestFirmwareDate() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test firmware date").set("style","color:blue" ) << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestFirmwareDate() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test firmware date").set("style","color:green") << std::endl ;
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test firmware date").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","3").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  std::string testFirmwareType = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testFirmwareType) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestFirmwareType() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test firmware type").set("style","color:blue" )  << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestFirmwareType() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test firmware type").set("style","color:green")  << std::endl ;
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test firmware type").set("style","color:red"  )  << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","4").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::tr();
  //
  ///////////////////////////////////////////////////////
  *out << cgicc::tr().set("ALIGN","center");
  //
  *out << cgicc::td().set("ALIGN","center");
  std::string testFirmwareVersion = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testFirmwareVersion) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestFirmwareVersion() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test firmware version").set("style","color:blue" ) << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestFirmwareVersion() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test firmware version").set("style","color:green") << std::endl ;
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test firmware version").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","5").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  std::string testFirmwareRevCode = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testFirmwareRevCode) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestFirmwareRevCode() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test firmware RevCode Id").set("style","color:blue" ) << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestFirmwareRevCode() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test firmware RevCode Id").set("style","color:green") << std::endl ;
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test firmware RevCode Id").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","6").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //  
  *out << cgicc::td().set("ALIGN","center");
  std::string testMezzId = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testMezzId) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestMezzId() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test mezzanine Id").set("style","color:blue" ) << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestMezzId() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test mezzanine Id").set("style","color:green") << std::endl ;
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test mezzanine Id").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","7").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::tr();
  //
  /////////////////////////////////////////////////////////  
  *out << cgicc::tr().set("ALIGN","center");
  //
  *out << cgicc::td().set("ALIGN","center");
  std::string testPromId = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testPromId) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestPromId() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test PROM Id").set("style","color:blue" ) << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestPromId() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test PROM Id").set("style","color:green") << std::endl ;
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test PROM Id").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","8").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  std::string testPROMPath = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testPROMPath) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestPROMPath() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test PROM path").set("style","color:blue" ) << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestPROMPath() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test PROM path").set("style","color:green") << std::endl ;
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test PROM path").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","9").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  std::string testDSN = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testDSN) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestDSN() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test DSN").set("style","color:blue" ) << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestDSN() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test DSN").set("style","color:green") << std::endl ;
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test DSN").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","10").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::tr();
  //
  /////////////////////////////////////////////////////////  
  *out << cgicc::tr().set("ALIGN","center");
  //
  *out << cgicc::td().set("ALIGN","center");
  std::string testADC = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testADC) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestADC() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test ADC").set("style","color:blue" ) << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestADC() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test ADC").set("style","color:green") << std::endl ;
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test ADC").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","11").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  std::string test3d3444 = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",test3d3444) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTest3d3444() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test 3d3444").set("style","color:blue" ) << std::endl ;
    //  
  } else if ( tmbTestVector[tmb].GetResultTest3d3444() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test 3d3444").set("style","color:green") << std::endl ;
    //
  } else {
    //    
    *out << cgicc::input().set("type","submit").set("value","TMB test 3d3444").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","12").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  std::string testRATtemper = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testRATtemper) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestRATtemper() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test RAT temperature").set("style","color:blue" ) << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestRATtemper() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test RAT temperature").set("style","color:green") << std::endl ;
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test RAT temperature").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","13").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::tr();
  //
  /////////////////////////////////////////////////////////  
  *out << cgicc::tr().set("ALIGN","center");
  //
  *out << cgicc::td().set("ALIGN","center");
  std::string testRATidCodes = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testRATidCodes) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestRATidCodes() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test RAT Id Codes").set("style","color:blue" ) << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestRATidCodes() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test RAT Id Codes").set("style","color:green") << std::endl ;
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test RAT Id Codes").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","14").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //  
  *out << cgicc::td().set("ALIGN","center");
  std::string testRATuserCodes = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testRATuserCodes) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestRATuserCodes() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test RAT User Codes").set("style","color:blue" ) << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestRATuserCodes() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test RAT User Codes").set("style","color:green") << std::endl ;
    //
  } else {
    *out << cgicc::input().set("type","submit").set("value","TMB test RAT User Codes").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","15").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  std::string testU760K = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testU760K)<< std::endl ;
  if ( tmbTestVector[tmb].GetResultTestU76chip() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test U760K").set("style","color:blue" ) << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestU76chip() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test U760K").set("style","color:green") << std::endl ;
    //
  } else {
    *out << cgicc::input().set("type","submit").set("value","TMB test U760K").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","16").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::table();
  //
  /////////////////////////////////////////////////////////////////////
  //
  *out << cgicc::textarea().set("name","TMBTestOutput").set("WRAP","OFF").set("rows","20").set("cols","100");
  *out << OutputTMBTests[tmb][current_crate_].str() << endl ;
  *out << cgicc::textarea();
  //    
  std::string LogTMBTestsOutput = toolbox::toString("/%s/LogTMBTestsOutput",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LogTMBTestsOutput) << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","submit").set("value","Log output").set("name","LogTMBTestsOutput") << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Clear").set("name","ClearTMBTestsOutput") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::fieldset();
  //
}
//
void EmuPeripheralCrateConfig::testTMB(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    cout << "testTMB:  TMB " << tmb << endl;
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
  tmbTestVector[tmb].RedirectOutput(&OutputTMBTests[tmb][current_crate_]);
  if ( tmbTestid == 1 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testBootRegister();
    ::sleep(1);
    //printf("Test1");
  }
  if ( tmbTestid == 2 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testVMEfpgaDataRegister();
    //printf("Test2");
  }
  if ( tmbTestid == 3 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testFirmwareDate();
    //printf("Test3");
  }
  if ( tmbTestid == 4 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testFirmwareType();
    //printf("Test4");
  }
  if ( tmbTestid == 5 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testFirmwareVersion();
    //printf("Test5");
  }
  if ( tmbTestid == 6 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testFirmwareRevCode();
    //printf("Test6");
  }
  if ( tmbTestid == 7 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testMezzId();
    //printf("Test7");
  }
  if ( tmbTestid == 8 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testPROMid();
    //printf("Test8");
  }
  if ( tmbTestid == 9 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testPROMpath();
    //printf("Test9");
  }
  if ( tmbTestid == 10 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testDSN();
    //printf("Test10");
  }
  if ( tmbTestid == 11 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testADC();
    //printf("Test11");
  }
  if ( tmbTestid == 12 || tmbTestid == 0 ) {
    tmbTestVector[tmb].test3d3444();
    //printf("Test12");
  }
  if ( tmbTestid == 13 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testRATtemper();
    //printf("Test13");
  }
  if ( tmbTestid == 14 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testRATidCodes();
    //printf("Test14");
  }
  if ( tmbTestid == 15 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testRATuserCodes();
    //printf("Test15");
  }
  if ( tmbTestid == 16 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testU76chip();
    //printf("Test16");
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
///////////////////////////////////////////////////////////////////////
// TMB utilities methods
///////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::TMBStatus(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    cout << "TMBStatus:  TMB=" << tmb << endl;
    TMB_ = tmb;
  } else {
    cout << "TMBStatus: No TMB" << endl ;
    tmb = TMB_;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
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
  Chamber * thisChamber = chamberVector[tmb];
  //
  char Name[100];
  sprintf(Name,"%s TMB status, crate=%s, slot=%d",
	  (thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),thisTMB->slot());
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
  if ( month == expected_month &&
       day   == expected_day   &&
       year  == expected_year ) {
    *out << cgicc::span().set("style","color:green");
  } else {
    *out << cgicc::span().set("style","color:red");
  }
  sprintf(buf,"TMB Firmware date         : %02x/%02x/%04x ",month,day,year);
  *out << buf;
  *out << cgicc::span();
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
  
  if ( type == expected_type ) {
    *out << cgicc::span().set("style","color:green");
  } else {
    *out << cgicc::span().set("style","color:red");
  }
  sprintf(buf,"Firmware Type             : %01x ",type);       
  *out << buf;
  *out << cgicc::span();
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
  if ( version == expected_version ){
    *out << cgicc::span().set("style","color:green");
  } else {
    *out << cgicc::span().set("style","color:red");
  }
  sprintf(buf,"Firmware Version Code     : %01x ",version);       
  *out << buf ;
  *out << cgicc::span();
  //
  *out << cgicc::br();
  //
  //
  if ( ((thisTMB->FirmwareVersion()>>8)&0x1f) == thisTMB->slot() ){
    *out << cgicc::span().set("style","color:green");
  } else {
    *out << cgicc::span().set("style","color:red");
  }
  sprintf(buf,"Geographic Address        : %02d ",((thisTMB->FirmwareVersion()>>8)&0x1f));       
  *out << buf ;
  *out << cgicc::span();
  //
  *out << cgicc::br();
  //
  //
  sprintf(buf,"Firmware Revision Code    : %04x ",((thisTMB->FirmwareRevCode())&0xffff));       
  *out << buf ;
  *out << cgicc::br();
  //
  //
  if ( (thisTMB->PowerComparator())&0x1f == 0x1f ) {
    *out << cgicc::span().set("style","color:green");
  } else {
    *out << cgicc::span().set("style","color:red");
  }
  sprintf(buf,"Power Comparator          : %02x ",((thisTMB->PowerComparator())&0x1f));       
  *out << buf ;
  *out << cgicc::span();
  //
  //
  *out << cgicc::pre();
  *out << "On-board temperatures and voltages:" << std::endl;
  *out << cgicc::pre();  
  //
  const int TMB_MAX_TEMP = 40;
  //
  int TMBtempFPGA = thisTMB->ReadTMBtempFPGA();  
  if ( TMBtempFPGA > TMB_MAX_TEMP) {  
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");
  }
  *out << "TMB (FPGA)       =   " << TMBtempFPGA << " deg C" << std::endl;
  *out << cgicc::span();
  //
  int TMBtempPCB = thisTMB->ReadTMBtempPCB();  
  if ( TMBtempPCB > TMB_MAX_TEMP ) {  
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");
  }
  *out << "TMB (PCB)        =   " << TMBtempPCB << " deg C" << std::endl;
  *out << cgicc::span();
  //
  int RATtempHSink = rat->ReadRATtempHSink();  
  if ( RATtempHSink > TMB_MAX_TEMP ) {  
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");
  }
  *out << "RAT (Heat sink)  =   " << RATtempHSink << " deg C" << std::endl;
  *out << cgicc::span();
  //
  int RATtempPCB = rat->ReadRATtempPCB();  
  if ( RATtempPCB > TMB_MAX_TEMP ) {  
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");
  }
  *out << "RAT (PCB)        =   " << RATtempPCB << " deg C" << std::endl;
  *out << cgicc::span();
  //
  *out << cgicc::table().set("border","1");
  //
  bool adcOK = tmbTestVector[tmb].testADC();
  ////////////////////////////////////////
  *out << cgicc::td().set("ALIGN","left");
  *out << "Power line";
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << "Voltage (V)";
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << "Current (A)";
  *out << cgicc::td();
  ////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "TMB 5.0 V";
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  if (adcOK) *out << cgicc::span().set("style","color:green");
  else       *out << cgicc::span().set("style","color:red");
  *out << std::setprecision(2) << std::fixed << thisTMB->Get5p0v();
  *out << cgicc::span();
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << std::setprecision(2) << std::fixed << thisTMB->Get5p0a();
  *out << cgicc::td();
  ////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "TMB 3.3 V";
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  if (adcOK) *out << cgicc::span().set("style","color:green");
  else       *out << cgicc::span().set("style","color:red");
  *out << std::setprecision(2) << thisTMB->Get3p3v();
  *out << cgicc::span();
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << std::setprecision(2) << thisTMB->Get3p3a();
  *out << cgicc::td();
  ////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "TMB 1.5 V Core";
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  if (adcOK) *out << cgicc::span().set("style","color:green");
  else       *out << cgicc::span().set("style","color:red");
  *out << std::setprecision(3) << thisTMB->Get1p5vCore();
  *out << cgicc::span();
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << std::setprecision(2) << thisTMB->Get1p5aCore();
  *out << cgicc::td();
  ////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "TMB 1.5 V TT";
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  if (adcOK) *out << cgicc::span().set("style","color:green");
  else       *out << cgicc::span().set("style","color:red");
  *out << std::setprecision(2) << thisTMB->Get1p5vTT();
  *out << cgicc::span();
  *out << cgicc::td();
  ////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "TMB 1.0 V TT";
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  if (adcOK) *out << cgicc::span().set("style","color:green");
  else       *out << cgicc::span().set("style","color:red");
  *out << std::setprecision(2) << thisTMB->Get1p0vTT();
  *out << cgicc::span();
  *out << cgicc::td();
  ////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "RAT 1.8 V";
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  if (adcOK) *out << cgicc::span().set("style","color:green");
  else       *out << cgicc::span().set("style","color:red");
  *out << std::setprecision(2) << thisTMB->Get1p8vRAT();
  *out << cgicc::span();
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << std::setprecision(2) << thisTMB->Get1p8aRAT();
  *out << cgicc::td();
  ////////////////////////////////////////
  *out << cgicc::table();
  //
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
  thisTMB->PrintCLCT();
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
    thisTMB->PrintALCT();
    thisTMB->RedirectOutput(&std::cout);
    *out << cgicc::pre();
    *out << cgicc::fieldset();
  }
  //
  *out << std::endl;    
  //
}
//
void EmuPeripheralCrateConfig::TMBUtils(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    cout << "TMBUtils:  TMB " << tmb << endl;
    TMB_ = tmb;
  } else {
    cout << "TMBUtils:  No TMB" << endl ;
    tmb = TMB_;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  //
  Chamber * thisChamber = chamberVector[tmb];
  //
  char Name[100];
  sprintf(Name,"%s TMB utilities, crate=%s slot=%d",
	  (thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),thisTMB->slot());

  //
  alct = thisTMB->alctController();
  rat = thisTMB->getRAT();
  //
  MyHeader(in,out,Name);
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
  std::string TMBFirmware = FirmwareDir_+"tmb/"+date+"/tmb";   // ".xsvf" is added in SetXsvfFilename
  TMBFirmware_ = TMBFirmware;
  //
  *out << "Load TMB Firmware:  Following the following steps..." << cgicc::br() << std::endl;
  *out << "Step 1)  BE CAREFUL" << cgicc::br() << std::endl;
  *out << "Step 2)  DO NOT POWER OFF CRATE" << cgicc::br() << std::endl;
  *out << "Step 3)  Disable DCS monitoring to crates" << cgicc::br() << std::endl;
  //
  std::string LoadTMBFirmware = toolbox::toString("/%s/LoadTMBFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadTMBFirmware) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Step 4) Load TMB Firmware") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << TMBFirmware_.toString() << ".xsvf";
  *out << cgicc::form() << std::endl ;
  //
  std::string CheckTMBFirmware = toolbox::toString("/%s/CheckTMBFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CheckTMBFirmware) ;
  if ( tmb_vme_ready == 1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","Step 5) Check TMB VME Ready").set("style","color:green");
    //
  } else if ( tmb_vme_ready == 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","Step 5) Check TMB VME Ready").set("style","color:red");
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","Step 5) Check TMB VME Ready").set("style","color:blue");
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  //
  std::string ClearTMBBootReg = toolbox::toString("/%s/ClearTMBBootReg",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ClearTMBBootReg) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Step 6) Enable VME Access to TMB FPGA") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br() << std::endl;
  *out << cgicc::br() << std::endl;
  *out << cgicc::br() << std::endl;
  //
  // remove the ALCT firmware downloading until it can be made robust
  //
  if (alct) {
    std::string LoadALCTFirmware = toolbox::toString("/%s/LoadALCTFirmware",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",LoadALCTFirmware) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Load ALCT Firmware") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
  }
  //
  if (rat) {
    std::string RATFirmware = FirmwareDir_+RAT_FIRMWARE_FILENAME;
    RATFirmware_ = RATFirmware;
    //
    std::string LoadRATFirmware = toolbox::toString("/%s/LoadRATFirmware",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",LoadRATFirmware) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Load RAT Firmware") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << RATFirmware_.toString() ;
    *out << cgicc::form() << std::endl ;
  }
  //
  std::string ReadTMBRegister = toolbox::toString("/%s/ReadTMBRegister",getApplicationDescriptor()->getURN().c_str());
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
  std::string TMBDumpAllRegisters = toolbox::toString("/%s/TMBDumpAllRegisters",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBDumpAllRegisters) ;
  *out << cgicc::input().set("type","submit").set("value","Dump All TMB VME Registers") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  //
  //--------------------------------------------------------
  *out << cgicc::table().set("border","0");
  //
  ////////////////////////////////////////
  *out << cgicc::td().set("ALIGN","left");
  *out << "TMB Counters";
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string TMBPrintCounters = toolbox::toString("/%s/TMBPrintCounters",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBPrintCounters) ;
  *out << cgicc::input().set("type","submit").set("value","Print TMB Counters") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string TMBResetCounters = toolbox::toString("/%s/TMBResetCounters",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBResetCounters) ;
  *out << cgicc::input().set("type","submit").set("value","Reset TMB Counters") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string TMBCounterForFixedTime = toolbox::toString("/%s/TMBCounterForFixedTime",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBCounterForFixedTime) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Read TMB counters--fixed time") << std::endl ;
  *out << "Number of seconds" << std::endl;
  *out << cgicc::input().set("type","text").set("value","10").set("name","time_to_wait") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  ////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "Inject fake data";
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string TriggerTestInjectALCT = toolbox::toString("/%s/TriggerTestInjectALCT",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TriggerTestInjectALCT) ;
  *out << cgicc::input().set("type","submit").set("value","TriggerTest : InjectALCT") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string TriggerTestInjectCLCT = toolbox::toString("/%s/TriggerTestInjectCLCT",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TriggerTestInjectCLCT) ;
  *out << cgicc::input().set("type","submit").set("value","TriggerTest : InjectCLCT") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //////////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "TMB Scope";
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string armScope = toolbox::toString("/%s/armScope",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",armScope) ;
  *out << cgicc::input().set("type","submit").set("value","arm Scope") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string readoutScope = toolbox::toString("/%s/readoutScope",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",readoutScope) ;
  *out << cgicc::input().set("type","submit").set("value","readout Scope") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string forceScope = toolbox::toString("/%s/forceScope",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",forceScope) ;
  *out << cgicc::input().set("type","submit").set("value","force Scope") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //////////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "TMB+ALCT User PROMS";
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string TMBClearUserProms = toolbox::toString("/%s/TMBClearUserProms",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBClearUserProms) ;
  *out << cgicc::input().set("type","submit").set("value","Clear TMB+ALCT User Proms") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //////////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "TMB+ALCT Configuration";
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string TMBConfigure = toolbox::toString("/%s/TMBConfigure",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBConfigure) ;
  *out << cgicc::input().set("type","submit").set("value","Configure TMB+ALCT") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string TMBReadConfiguration = toolbox::toString("/%s/TMBReadConfiguration",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBReadConfiguration) ;
  *out << cgicc::input().set("type","submit").set("value","Read TMB+ALCT Configuration") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string TMBCheckConfiguration = toolbox::toString("/%s/TMBCheckConfiguration",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBCheckConfiguration) ;
  if ( thisTMB->GetTMBConfigurationStatus() == 1 &&
       alct->GetALCTConfigurationStatus()   == 1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","Check TMB+ALCT Configuration").set("style","color:green");
    //
  } else if ( thisTMB->GetTMBConfigurationStatus() == 0 ||
	      alct->GetALCTConfigurationStatus()   == 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","Check TMB+ALCT Configuration").set("style","color:red");
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","Check TMB+ALCT Configuration").set("style","color:blue");
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  /////////////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "State Machines";
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string TMBReadStateMachines = toolbox::toString("/%s/TMBReadStateMachines",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBReadStateMachines) ;
  *out << cgicc::input().set("type","submit").set("value","Read TMB State Machines") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string TMBCheckStateMachines = toolbox::toString("/%s/TMBCheckStateMachines",getApplicationDescriptor()->getURN().c_str());
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
    //
    *out << cgicc::input().set("type","submit").set("value","Check TMB State Machines").set("style","color:red");
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","Check TMB State Machines").set("style","color:blue");
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //////////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "Raw Hits";
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string TMBRawHits = toolbox::toString("/%s/TMBRawHits",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBRawHits) ;
  *out << cgicc::input().set("type","submit").set("value","Read TMB Raw Hits") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string ALCTRawHits = toolbox::toString("/%s/ALCTRawHits",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ALCTRawHits) ;
  *out << cgicc::input().set("type","submit").set("value","Read ALCT Raw Hits") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //--------------------------------------------------------
  *out << cgicc::table();
  //
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
void EmuPeripheralCrateConfig::LoadTMBFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
  tmb_vme_ready = -1;
  //
  TMB * thisTMB = tmbVector[tmb];
  //
  // Put CCB in FPGA mode to make the CCB ignore TTC commands (such as hard reset) during TMB downloading...
  thisCCB->setCCBMode(CCB::VMEFPGA);
  //
  int mintmb = tmb;
  int maxtmb = tmb+1;
  //
  // To remove the loop, comment out the following if (thisTMB->slot() ...) stuff...
  //  if (thisTMB->slot() == 26) { //if TMB slot = 26, loop over each alct according to its type
  //    mintmb = 0;
  //    maxtmb = tmbVector.size()-1;
  //  }
  // end of stuff to remove...
  //
  std::cout << "Loading TMB firmware from " << mintmb << " to " << maxtmb << std::endl;
  //
  int number_of_verify_errors[9] = {};
  //
  for (tmb=mintmb; tmb<maxtmb; tmb++) {
    thisTMB = tmbVector[tmb];
    //
    std::cout << "Loading TMB firmware in slot " << thisTMB->slot() << std::endl;
    //
    ::sleep(10);
    //
    thisTMB->SetXsvfFilename(TMBFirmware_.toString().c_str());
    thisTMB->ProgramTMBProms();
    thisTMB->ClearXsvfFilename();
    //
    //int debugMode(0);
    //int jch(5);
    //
    //thisTMB->disableAllClocks();
    //int status = thisTMB->SVFLoad(&jch,TMBFirmware_.toString().c_str(),debugMode);
    //thisTMB->enableAllClocks();
    //
    number_of_verify_errors[tmb] = thisTMB->GetNumberOfVerifyErrors();
    cout << "=== Programming finished on TMB slot " << thisTMB->slot() << endl;
    cout << "=== " << number_of_verify_errors[tmb] << " Verify Errors occured" << endl;
    //
  }
  //
  int total_number_of_errors = 0;
  for (int i=0; i<9; i++) 
    total_number_of_errors += number_of_verify_errors[i];
  //
  if (total_number_of_errors < 0) {
    cout << "File does not exist, programming did not occur..."<< endl;
    //
  } else if (total_number_of_errors == 0) {
    cout << "Please perform a TTC/CCB hard reset to Load FPGA"<< endl;
    //
  } else {
    cout << "ERROR!!  Total number of verify errors = " << total_number_of_errors << endl;
    for (int i=0; i<9; i++) {
      cout << "TMB slot " << tmbVector[i]->slot() 
	   << " -> Number of errors = " << number_of_verify_errors[i] 
	   << std::endl;
    }
    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << endl;
    cout << "!!!! Do not perform hard reset !!!! " << endl;
    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << endl;
  }
  //
  // Put CCB back into DLOG mode to listen to TTC commands...
  thisCCB->setCCBMode(CCB::DLOG);
  //
  this->TMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::CheckTMBFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
  int mintmb = tmb;
  int maxtmb = tmb+1;
  //
  if (thisTMB->slot() == 26) {
    mintmb = 0;
    maxtmb = tmbVector.size()-1;
  }
  //
  std::cout << "Checking TMB VME Ready from " << mintmb << " to " << maxtmb << std::endl;
  //
  tmb_vme_ready = 1;
  //
  for (tmb=mintmb; tmb<maxtmb; tmb++) {
    thisTMB = tmbVector[tmb];

    short unsigned int BootReg;
    thisTMB->tmb_get_boot_reg(&BootReg);
    std::cout << "Boot register = 0x" << std::hex << BootReg << std::endl;
    //
    if (thisTMB->GetBootVMEReady() != 1) tmb_vme_ready = 0;
  }
  //
  this->TMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::ClearTMBBootReg(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
  thisTMB->tmb_set_boot_reg(0x0);
  //
  this->TMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::LoadALCTFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "LoadALCTFirmware:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "LoadALCTFirmware:  No tmb" << std::endl ;
    tmb = TMB_;
  }
  //
  TMB * thisTMB;
  thisTMB = tmbVector[tmb];
  //
  ALCTController * thisALCT;
  thisALCT = thisTMB->alctController();
  //
  char date[8];
  //create filename for firmware based on expected date...
  //If broadcast slot 26 is picked, this will pick slot 26's date tag for ALL the firmware which is downloaded
  int expected_year  = thisALCT->GetExpectedFastControlYear() ;
  int expected_month = thisALCT->GetExpectedFastControlMonth();
  int expected_day   = thisALCT->GetExpectedFastControlDay()  ;
  //
  sprintf(date,"%04u%02u%02u",
  	  expected_year,
  	  expected_month,
  	  expected_day);
  //
  // pre-DAQ06 format
  //  int expected_year       = thisALCT->GetExpectedFastControlYear() & 0xffff;
  //  int expected_month_tens = (thisALCT->GetExpectedFastControlMonth()>>4) & 0xf;
  //  int expected_month_ones = (thisALCT->GetExpectedFastControlMonth()>>0) & 0xf;
  //  int expected_day_tens   = (thisALCT->GetExpectedFastControlDay()  >>4) & 0xf;
  //  int expected_day_ones   = (thisALCT->GetExpectedFastControlDay()  >>0) & 0xf;
  //  sprintf(date,"%4x%1x%1x%1x%1x",
  //	  expected_year,
  //	  expected_month_tens,
  //	  expected_month_ones,
  //	  expected_day_tens,
  //	  expected_day_ones);
  //
  int mintmb = tmb;
  int maxtmb = tmb+1;
  if (thisTMB->slot() == 26) { //if TMB slot = 26, loop over each alct according to its type
    mintmb = 0;
    maxtmb = tmbVector.size()-1;
  }
  //
  //  thisCCB->hardReset();
  //
  std::cout << "Loading ALCT firmware in the TMB vector from index " << mintmb << " to " << maxtmb << std::endl;
  //
  for (tmb=mintmb; tmb<maxtmb; tmb++) {
    //
    thisTMB = tmbVector[tmb];
    thisALCT = thisTMB->alctController();
    //
    if (!thisALCT) {
      std::cout << "No ALCT present" << std::endl;
      return;
    }
    //
    std::string ALCTFirmware = FirmwareDir_+"alct/"+date+"/";
    //
    if ( (thisALCT->GetChamberType()).find("ME11") != string::npos ) {
      //
      if (thisALCT->GetExpectedFastControlBackwardForwardType() == BACKWARD_FIRMWARE_TYPE &&
	  thisALCT->GetExpectedFastControlNegativePositiveType() == NEGATIVE_FIRMWARE_TYPE ) {
	ALCTFirmware += ALCT_FIRMWARE_FILENAME_ME11_BACKWARD_NEGATIVE;
      } else if (thisALCT->GetExpectedFastControlBackwardForwardType() == BACKWARD_FIRMWARE_TYPE &&
		 thisALCT->GetExpectedFastControlNegativePositiveType() == POSITIVE_FIRMWARE_TYPE ) {
	ALCTFirmware += ALCT_FIRMWARE_FILENAME_ME11_BACKWARD_POSITIVE;
      } else if (thisALCT->GetExpectedFastControlBackwardForwardType() == FORWARD_FIRMWARE_TYPE &&
		 thisALCT->GetExpectedFastControlNegativePositiveType() == POSITIVE_FIRMWARE_TYPE ) {
	ALCTFirmware += ALCT_FIRMWARE_FILENAME_ME11_FORWARD_POSITIVE;
      } else {
	ALCTFirmware += ALCT_FIRMWARE_FILENAME_ME11;
      }
    } else if ( (thisALCT->GetChamberType()).find("ME12") != string::npos ) {
      ALCTFirmware += ALCT_FIRMWARE_FILENAME_ME12;
    } else if ( (thisALCT->GetChamberType()).find("ME13") != string::npos ) {
      ALCTFirmware += ALCT_FIRMWARE_FILENAME_ME13;
    } else if ( (thisALCT->GetChamberType()).find("ME21") != string::npos ) {
      ALCTFirmware += ALCT_FIRMWARE_FILENAME_ME21;
    } else if ( (thisALCT->GetChamberType()).find("ME22") != string::npos ) {
      ALCTFirmware += ALCT_FIRMWARE_FILENAME_ME22;
    } else if ( (thisALCT->GetChamberType()).find("ME31") != string::npos ) {
      ALCTFirmware += ALCT_FIRMWARE_FILENAME_ME31;
    } else if ( (thisALCT->GetChamberType()).find("ME32") != string::npos ) {
      ALCTFirmware += ALCT_FIRMWARE_FILENAME_ME32;
    } else if ( (thisALCT->GetChamberType()).find("ME41") != string::npos ) {
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
    thisALCT->ReadSlowControlId();
    thisALCT->PrintSlowControlId();
    //
    thisALCT->ReadFastControlId();
    thisALCT->PrintFastControlId();
    //

    thisTMB->disableAllClocks();
    LOG4CPLUS_INFO(getApplicationLogger(), "Programming ALCT");
    //
    int status = thisALCT->SVFLoad(&jch,ALCTFirmware_.toString().c_str(),debugMode);
    thisTMB->enableAllClocks();
    //
    if (status >= 0){
      LOG4CPLUS_INFO(getApplicationLogger(), "Programming ALCT finished");
      cout << "=== Programming finished"<< endl;
      //	cout << "=== " << status << " Verify Errors  occured" << endl;
    } else {
      cout << "=== Fatal Error. Exiting with " <<  status << endl;
    }
  }
  //
  //  thisCCB->hardReset();
  //
  this->TMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::LoadRATFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
void EmuPeripheralCrateConfig::ReadTMBRegister(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
//////////////////////////////////////////////////////////////////
// Logging information
///////////////////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::DMBBoardID(xgi::Input * in, xgi::Output * out ) 
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
void EmuPeripheralCrateConfig::TMBBoardID(xgi::Input * in, xgi::Output * out ) 
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
void EmuPeripheralCrateConfig::RATBoardID(xgi::Input * in, xgi::Output * out ) 
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
void EmuPeripheralCrateConfig::CCBBoardID(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  CCBBoardID_= cgi["CCBBoardID"]->getValue() ;
  //
  this->CrateConfiguration(in,out);
  //
}
//
void EmuPeripheralCrateConfig::ControllerBoardID(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  ControllerBoardID_= cgi["ControllerBoardID"]->getValue() ;
  //
  this->CrateConfiguration(in,out);
  //
}
//
void EmuPeripheralCrateConfig::CrateChassisID(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  CrateChassisID_= cgi["CrateChassisID"]->getValue() ;
  //
  this->CrateConfiguration(in,out);
  //
}
//
void EmuPeripheralCrateConfig::CrateRegulatorBoardID(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  CrateRegulatorBoardID_= cgi["CrateRegulatorBoardID"]->getValue() ;
  //
  this->CrateConfiguration(in,out);
  //
}
//
void EmuPeripheralCrateConfig::PeripheralCrateMotherBoardID(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  PeripheralCrateMotherBoardID_= cgi["PeripheralCrateMotherBoardID"]->getValue() ;
  //
  this->CrateConfiguration(in,out);
  //
}
//
void EmuPeripheralCrateConfig::ELMBID(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  ELMBID_= cgi["ELMBID"]->getValue() ;
  //
  this->CrateConfiguration(in,out);
  //
}
//
void EmuPeripheralCrateConfig::BackplaneID(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  BackplaneID_= cgi["BackplaneID"]->getValue() ;
  //
  this->CrateConfiguration(in,out);
  //
}
//
///////////////////////////////////////////////////////////////////////////////////
// DMB status
///////////////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::DMBStatus(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
  bool isME13 = false;
  TMB * thisTMB   = tmbVector[dmb];
  ALCTController * thisALCT;
  if (thisTMB) 
    thisALCT = thisTMB->alctController();
  if (thisALCT) 
    if ( (thisALCT->GetChamberType()).find("ME13") != string::npos )
      isME13 = true;
  //
  Chamber * thisChamber = chamberVector[dmb];
      std::string chamber=thisChamber->GetLabel();
      unsigned long int cfebID[5], cfebIDread[5];
      vector <CFEB> thisCFEBs=thisDMB->cfebs();
      //
      for (unsigned i=0;i<thisCFEBs.size();i++) {
        cfebIDread[i]=thisDMB->febpromuser(thisCFEBs[i]);
        cfebID[i]=brddb->ChamberToCFEBID(chamber,i+1);
        std::cout<<" DB_check CFEB # "<<i<<" ID readback: "<<(cfebIDread[i]&0xfff)<<" Look up from DB: "<<(cfebID[i]&0xfff)<<std::endl;
      }
      std::string crate=thisCrate->GetLabel();
      int slot=thisDMB->slot();
      std::cout<<" Crate: "<<crate<<" slot "<<slot<<std::endl;
      int dmbID=brddb->CrateToDMBID(crate,slot);
      //The readback
      unsigned long int dmbIDread=thisDMB->mbpromuser(0);
      std::cout<<" DB_check DMB ID readback: "<<(dmbIDread&0xfff)<<" look up from DB: "<<(dmbID&0xfff)<<std::endl;
  //
  char Name[100];
  sprintf(Name,"%s DMB status, crate=%s, slot=%d",(thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),thisDMB->slot());	
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
  //
  if ( (int) thisDMB->GetFirmwareVersion() == EXPECTED_DMB_VME_VERSION &&
       (int) thisDMB->GetFirmwareRevision() == EXPECTED_DMB_FIRMWARE_REVISION ) {
    *out << cgicc::span().set("style","color:green");
    *out << buf;
    *out << cgicc::span();
  } else {
    *out << cgicc::span().set("style","color:red");
    *out << buf;
    *out << cgicc::span();
  }
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
  if (isME13) { 
    *out << cgicc::span().set("style","color:black");
  } else if ( value/1000. < 3.3*0.95 ||
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
  if (isME13) { 
    *out << cgicc::span().set("style","color:black");
  } else if ( value/1000. < 5.0*0.95 ||
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
  if (isME13) { 
    *out << cgicc::span().set("style","color:black");
  } else if ( value/1000. < 6.0*0.95 ||
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
  if (isME13) { 
    *out << cgicc::span().set("style","color:black");
  } else if ( value > 50 && value < 95 ) {
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
  //
  void EmuPeripheralCrateConfig::ReadCCBRegister(xgi::Input * in, xgi::Output * out ) 
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
  void EmuPeripheralCrateConfig::ReadTTCRegister(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
      thisCCB->PrintTTCrxRegs();
    //
    this->CCBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrateConfig::HardReset(xgi::Input * in, xgi::Output * out ) 
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
void EmuPeripheralCrateConfig::DMBPrintCounters(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
  void EmuPeripheralCrateConfig::LogDMBTestsOutput(xgi::Input * in, xgi::Output * out ) 
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
      OutputDMBTests[dmb][current_crate_].str("");
      OutputDMBTests[dmb][current_crate_] << "DMB-CFEB Tests " 
					  << thisCrate->GetChamber(dmbVector[dmb]->slot())->GetLabel().c_str() 
					  << " output:" << std::endl;

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
    DMBTestsLogFile << OutputDMBTests[dmb][current_crate_].str() ;
    DMBTestsLogFile.close();
    //
    OutputDMBTests[dmb][current_crate_].str("");
    //
    this->DMBTests(in,out);
    //
  }
  //
  void EmuPeripheralCrateConfig::LogTestSummary(xgi::Input * in, xgi::Output * out ) 
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
    //    buf = "EmuPeripheralCrateTestSummary_"+RunNumber_+"_"+Operator_+time+".log";
    buf = "EmuPeripheralCrateTestSummary_"+RunNumber_+"_"+Operator_+".log";
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
    //    LogFile << std::endl ;
    //    LogFile << "Chassis " << std::setw(7) << CrateChassisID_ << std::endl;
    //    LogFile << "Backplane " << std::setw(5) << BackplaneID_ << std::endl;
    //    LogFile << "CRB " << std::setw(6) << CrateRegulatorBoardID_ << std::endl;
    //    LogFile << "PCMB " << std::setw(5) << PeripheralCrateMotherBoardID_ << std::endl;
    //    LogFile << "ELMB " << std::setw(5) << ELMBID_ << std::endl;
    //    LogFile << std::endl ;
    //    //
    //    LogFile << "VCC    1" << std::setw(5) << ControllerBoardID_ << std::endl;
    //    //
    //    for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) {
    //      //
    //      LogFile << "TMB" << std::setw(5) << tmbVector[i]->slot() << std::setw(5) <<
    //	TMBBoardID_[i] << std::setw(5) << RATBoardID_[i] <<std::setw(5) <<
    //	tmbTestVector[i].GetResultTestBootRegister() << std::setw(5) <<
    //	//	tmbTestVector[i].GetResultTestVMEfpgaDataRegister() << std::setw(5) <<
    //	tmbTestVector[i].GetResultTestFirmwareDate() << std::setw(5) <<
    //	tmbTestVector[i].GetResultTestFirmwareType() << std::setw(5) <<
    //	tmbTestVector[i].GetResultTestFirmwareVersion() << std::setw(5) <<
    //	tmbTestVector[i].GetResultTestFirmwareRevCode() << std::setw(5) <<
    //	tmbTestVector[i].GetResultTestMezzId() << std::setw(5) <<
    //	tmbTestVector[i].GetResultTestPromId() << std::setw(5) <<
    //	tmbTestVector[i].GetResultTestPROMPath() << std::setw(5) <<
    //	tmbTestVector[i].GetResultTestDSN() << std::setw(5) <<
    //	tmbTestVector[i].GetResultTestADC() << std::setw(5) <<
    //	tmbTestVector[i].GetResultTest3d3444() << std::setw(5) <<
    //	tmbTestVector[i].GetResultTestRATtemper() << std::setw(5) <<
    //	tmbTestVector[i].GetResultTestRATidCodes() << std::setw(5) <<
    //	tmbTestVector[i].GetResultTestRATuserCodes() << std::setw(5) <<
    //	tmbTestVector[i].GetResultTestU76chip() 
    //	      << std::endl ;
    //      //
    //    }
    //    //
    //    for(int i=0; i<20; i++) LogFile << "-";
    //    LogFile << std::endl ;
    //    //
    //    LogFile << "MPC     12 " << std::setw(5)  << MPCBoardID_ << std::endl;
    //    LogFile << "CCB     13 " << std::setw(5)  << CCBBoardID_ << std::endl;
    //    //
    //    for(int i=0; i<20; i++) LogFile << "-";
    //    LogFile << std::endl ;
    //
    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      //
      SetCurrentCrate(crate_number);
      //
      //      for (unsigned int i=0; i<(dmbVector.size()<9?dmbVector.size():9) ; i++) {
      //	//
      //	LogFile << "DMB " << std::setw(5) << dmbVector[i]->slot() << std::setw(5) <<
      //	  DMBBoardID_[i] ;
      //	for (int j=0; j<20; j++) LogFile << std::setw(5) << dmbVector[i]->GetTestStatus(j) ;
      //	LogFile << std::endl ;
      //	//
      //      }
      //      //
      //      LogFile << std::endl;
      //
      //      for (unsigned int dmbctr=0; dmbctr<(dmbVector.size()<9?dmbVector.size():9) ; dmbctr++) {
      //      DAQMB * thisDMB = dmbVector[dmbctr];
      //      vector<CFEB> thisCFEBs = thisDMB->cfebs();
      //      for (unsigned int cfebctr=0; cfebctr<thisCFEBs.size(); cfebctr++) {
      //	LogFile << "CFEBid " << std::setw(5) << dmbctr 
      //		<< std::setw(5) << cfebctr 
      //		<< std::setw(10) << CFEBid_[dmbctr][cfebctr] 
      //		<< std::endl;
      //      }
      //    }
      //    LogFile << std::endl;
      //    //
      //    for(int i=0; i<20; i++) LogFile << "+";
      //    LogFile << std::endl ;
      //    LogFile << " CrateTest : " << std::endl;
      //    for(int i=0; i<20; i++) LogFile << "-";
      //    LogFile << std::endl ;
      //    //
      //    LogFile << "MpcTMBTest " << myCrateTest.GetMpcTMBTestResult() << std::endl ;
      //    //
      //    LogFile << std::endl ;
      //
      for(int i=0; i<20; i++) LogFile << "+";
      LogFile << std::endl ;
      LogFile << " Timing scans : " << std::endl;
      for(int i=0; i<20; i++) LogFile << "-";
      LogFile << std::endl ;

      for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) {
	//	
	Chamber * thisChamber = chamberVector[i];
	//
	LogFile << "slot                 " 
		<< std::setw(10) << (thisChamber->GetLabel()).c_str()
		<< std::setw(5) << tmbVector[i]->slot()
		<< std::endl;
	LogFile << "cfeb0delay           " 
		<< std::setw(10) << (thisChamber->GetLabel()).c_str()
		<< std::setw(5) << MyTest[i][current_crate_].GetCFEBrxPhaseTest(0)
		<< std::endl;
	LogFile << "cfeb1delay           " 
		<< std::setw(10) << (thisChamber->GetLabel()).c_str()
		<< std::setw(5) << MyTest[i][current_crate_].GetCFEBrxPhaseTest(1)
		<< std::endl;
	LogFile << "cfeb2delay           " 
		<< std::setw(10) << (thisChamber->GetLabel()).c_str()
		<< std::setw(5) << MyTest[i][current_crate_].GetCFEBrxPhaseTest(2)
		<< std::endl;
	LogFile << "cfeb3delay           " 
		<< std::setw(10) << (thisChamber->GetLabel()).c_str()
		<< std::setw(5) << MyTest[i][current_crate_].GetCFEBrxPhaseTest(3)
		<< std::endl;
	LogFile << "cfeb4delay           " 
		<< std::setw(10) << (thisChamber->GetLabel()).c_str()
		<< std::setw(5) << MyTest[i][current_crate_].GetCFEBrxPhaseTest(4)
		<< std::endl;
	LogFile << "alct_tx_clock_delay  " 
		<< std::setw(10) << (thisChamber->GetLabel()).c_str()
		<< std::setw(5) << MyTest[i][current_crate_].GetALCTtxPhaseTest()
		<< std::endl;
	LogFile << "alct_rx_clock_delay  " 
		<< std::setw(10) << (thisChamber->GetLabel()).c_str()
		<< std::setw(5) << MyTest[i][current_crate_].GetALCTrxPhaseTest()
		<< std::endl;
	LogFile << "rat_tmb_delay        " 
		<< std::setw(10) << (thisChamber->GetLabel()).c_str()
		<< std::setw(5) << MyTest[i][current_crate_].GetRatTmbDelayTest()
		<< std::endl;
	LogFile << "match_trig_alct_delay" 
		<< std::setw(10) << (thisChamber->GetLabel()).c_str()
		<< std::setw(5) << MyTest[i][current_crate_].GetMatchTrigAlctDelayTest()
		<< std::endl;
	LogFile << "mpc_tx_delay         " 
		<< std::setw(10) << (thisChamber->GetLabel()).c_str()
		<< std::setw(5) << MyTest[i][current_crate_].GetMpcTxDelayTest()
		<< std::endl;
	LogFile << "mpc_rx_delay         " 
		<< std::setw(10) << (thisChamber->GetLabel()).c_str()
		<< std::setw(5) << MyTest[i][current_crate_].GetMpcRxDelayTest()
		<< std::endl;
	LogFile << "tmb_lct_cable_delay  " 
		<< std::setw(10) << (thisChamber->GetLabel()).c_str()
		<< std::setw(5) << MyTest[i][current_crate_].GetTmbLctCableDelayTest()
		<< std::endl;
	LogFile << "alct_dav_cable_delay " 
		<< std::setw(10) << (thisChamber->GetLabel()).c_str()
		<< std::setw(5) << MyTest[i][current_crate_].GetAlctDavCableDelayTest()
		<< std::endl;
	LogFile << "cfeb_dav_cable_delay " 
		<< std::setw(10) << (thisChamber->GetLabel()).c_str()
		<< std::setw(5) << MyTest[i][current_crate_].GetCfebDavCableDelayTest()
		<< std::endl;
	LogFile << "tmb_l1a_delay        " 
		<< std::setw(10) << (thisChamber->GetLabel()).c_str()
		<< std::setw(5) << MyTest[i][current_crate_].GetTmbL1aDelayTest()
		<< std::endl;
	LogFile << "alct_l1a_delay       " 
		<< std::setw(10) << (thisChamber->GetLabel()).c_str()
		<< std::setw(5) << MyTest[i][current_crate_].GetAlctL1aDelayTest()
		<< std::endl;
	LogFile << "rpc0_rat_delay       " 
		<< std::setw(10) << (thisChamber->GetLabel()).c_str()
		<< std::setw(5) << MyTest[i][current_crate_].GetRpcRatDelayTest(0)
		<< std::endl;
	LogFile << "TTCrxID              " 
		<< std::setw(10) << (thisChamber->GetLabel()).c_str()
		<< std::setw(5) << thisCCB->GetReadTTCrxID() 
		<< std::endl;
	LogFile << "best_avg_aff_to_l1a  " 
		<< std::setw(10) << (thisChamber->GetLabel()).c_str()
		<< std::setw(10) << MyTest[i][current_crate_].GetBestAverageAFFtoL1A()
		<< std::endl;
	LogFile << "best_avg_alct_dav_scope  " 
		<< std::setw(10) << (thisChamber->GetLabel()).c_str()
		<< std::setw(10) << MyTest[i][current_crate_].GetBestAverageALCTDAVScope()
		<< std::endl;
	LogFile << "best_avg_cfeb_dav_scope  " 
		<< std::setw(10) << (thisChamber->GetLabel()).c_str()
		<< std::setw(10) << MyTest[i][current_crate_].GetBestAverageCFEBDAVScope()
		<< std::endl;
	//      for (int CFEBs = 0; CFEBs<5; CFEBs++) {
	//	LogFile << "cfeb" << CFEBs << "_scan " << std::setw(3) << i;
	//	for (int HalfStrip = 0; HalfStrip<32; HalfStrip++) 
	//	  LogFile << std::setw(3) << MyTest[i][current_crate_].GetCFEBStripScan(CFEBs,HalfStrip) ;
	//	LogFile << std::endl;
	//      }
	//      //
	//      LogFile << "alct_scan  " << std::setw(3) << i;
	//      for (int Wire = 0; Wire<(tmbVector[i]->alctController()->GetNumberOfChannelsInAlct())/6; Wire++) 
	//	LogFile << std::setw(3) << MyTest[i][current_crate_].GetALCTWireScan(Wire) ;
	//      LogFile << std::endl;
	//      //
	LogFile << std::endl;
      }
    }
    //
    LogFile.close();
    //
    this->Default(in,out);
    //
  }
  //
  void EmuPeripheralCrateConfig::LogOutput(xgi::Input * in, xgi::Output * out ) 
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
    LogFile << CrateTestsOutput.str();
    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      //
      SetCurrentCrate(crate_number);

      for (unsigned int i=0; i<tmbVector.size(); i++) {
	LogFile << OutputTMBTests[i][current_crate_].str() ;
	LogFile << ChamberTestsOutput[i][current_crate_].str() ;
      }
      for (unsigned int i=0; i<dmbVector.size(); i++) {
	LogFile << OutputDMBTests[i][current_crate_].str() ;
      }
    }
    LogFile.close();    
    //
    this->Default(in,out);
    //
  }
  //
  void EmuPeripheralCrateConfig::LogTMBTestsOutput(xgi::Input * in, xgi::Output * out ) 
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
      OutputTMBTests[tmb][current_crate_].str("");
      OutputTMBTests[tmb][current_crate_] << "TMB-RAT Tests " 
					  << thisCrate->GetChamber(tmbVector[tmb]->slot())->GetLabel().c_str() 
					  << " output:" << std::endl;
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
    TMBTestsLogFile << OutputTMBTests[tmb][current_crate_].str() ;
    TMBTestsLogFile.close();
    //
    OutputTMBTests[tmb][current_crate_].str("");
    OutputTMBTests[tmb][current_crate_] << "TMB-RAT Tests " 
				      << thisCrate->GetChamber(tmbVector[tmb]->slot())->GetLabel().c_str() 
				      << " output:" << std::endl;
    //
    this->TMBTests(in,out);
    //
  }
  //
  void EmuPeripheralCrateConfig::LogChamberTestsOutput(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "LogChamberTestsOutput" << std::endl;
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
    cgicc::form_iterator name2 = cgi.getElement("ClearChamberTestsOutput");
    //
    if(name2 != cgi.getElements().end()) {
      cout << "Clear..." << endl;
      cout << cgi["ClearChamberTestsOutput"]->getValue() << std::endl ;
      ChamberTestsOutput[tmb][current_crate_].str("");
      ChamberTestsOutput[tmb][current_crate_] << "Chamber-Crate Phases " 
					      << thisCrate->GetChamber(tmbVector[tmb]->slot())->GetLabel().c_str() 
					      << " output:" << std::endl;
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
    sprintf(buf,"ChamberTestsLogFile_%d_%s.log",thisTMB->slot(),TMBBoardID_[tmb].c_str());
    //
    ofstream ChamberTestsLogFile;
    ChamberTestsLogFile.open(buf);
    ChamberTestsLogFile << ChamberTestsOutput[tmb][current_crate_].str() ;
    ChamberTestsLogFile.close();
    //
    ChamberTestsOutput[tmb][current_crate_].str("");
    //
    this->ChamberTests(in,out);
    //
  }
  //
  void EmuPeripheralCrateConfig::LogCrateTestsOutput(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "LogCrateTestsOutput" << std::endl;
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name2 = cgi.getElement("ClearCrateTestsOutput");
    //
    if(name2 != cgi.getElements().end()) {
      cout << "Clear..." << endl;
      cout << cgi["ClearCrateTestsOutput"]->getValue() << std::endl ;
      CrateTestsOutput.str("");
      //
      this->CrateTests(in,out);
      return ;
      //
    }
    //
    char buf[20];
    sprintf(buf,"CrateTestsLogFile.log");
    //
    ofstream CrateTestsLogFile;
    CrateTestsLogFile.open(buf);
    CrateTestsLogFile << CrateTestsOutput.str() ;
    CrateTestsLogFile.close();
    //
    CrateTestsOutput.str("");
    //
    this->CrateTests(in,out);
    //
  }

////////////////////////////////////////////////////////////////////////////////////
// Responses to SOAP messages
////////////////////////////////////////////////////////////////////////////////////
xoap::MessageReference EmuPeripheralCrateConfig::ReadAllVmePromUserid (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  //implement the DMB VME PROM USER_CODE Readback
  //
  std::cout << "DMB VME PROM USER_CODE Readback " << std::endl;

  for(unsigned cv=0; cv<crateVector.size(); cv++) {
    SetCurrentCrate(cv);
    std::cout << "For Crate " << ThisCrateID_ << " : " << std::endl;

  for (unsigned idmb=0;idmb<dmbVector.size();idmb++) {
    //
    if ((dmbVector[idmb]->slot())<22) {
      DAQMB * thisDMB=dmbVector[idmb];
      unsigned long int boardnumber=thisDMB->mbpromuser(0);
      DMBBoardNumber[cv][idmb]=boardnumber;
      cout <<" The DMB Number: "<<idmb<<" is in Slot Number: "<<dmbVector[idmb]->slot()<<endl;
      cout <<" This DMB Board Number: "<<DMBBoardNumber[cv][idmb]<<endl<<endl;
    }
    //
  }

  }
  SetCurrentCrate(this_crate_no_);

  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateConfig::LoadAllVmePromUserid (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  //implement the DMB VME PROM USER_CODE programming
  //
  std::cout << "DMB VME PROM USER_CODE Programming " << std::endl;

  for(unsigned cv=0; cv<crateVector.size(); cv++) {
    SetCurrentCrate(cv);
    std::cout << "For Crate " << ThisCrateID_ << " : " << std::endl;

  usleep(200);
  for (unsigned idmb=0;idmb<dmbVector.size();idmb++) {
    //
    if ((dmbVector[idmb]->slot())<22) {
      DAQMB * thisDMB=dmbVector[idmb];
      unsigned long int boardnumber=DMBBoardNumber[cv][idmb];
      char prombrdname[4];
      //	if (idmb==0) boardnumber = 0xdb00000c;
      //	if (idmb==1) boardnumber = 0xdb00021b;
      //Read database for the board number:
      std::string crate=thisCrate->GetLabel();
      int slot=thisDMB->slot();
      int dmbID=brddb->CrateToDMBID(crate,slot);

      prombrdname[0]=boardnumber&0xff;
      prombrdname[1]=(boardnumber>>8)&0x03;
      prombrdname[2]=0x00;
      prombrdname[3]=0xdb;

      if (((boardnumber&0xfff)==0)||
	  ((boardnumber&0xfff)==0xfff)||
	  ((boardnumber&0xfff)==0xaad)) {
	prombrdname[0]=dmbID&0xff;
	prombrdname[1]=(dmbID>>8)&0x0f;
	cout<<" DMB board number reprogram from Database ..."<<endl;
      }
      //temperarily overwrite all board number using database
      //	prombrdname[0]=dmbID&0xff;
      //	prombrdname[1]=(dmbID>>8)&0x0f;

        cout<<" Loading the board number ..."<<(prombrdname[0]&0xff)+((prombrdname[1]<<8)&0xf00)<<" was set to: "<<(boardnumber&0xffff)<<endl;

      //
      std::string DMBVmeFirmware = FirmwareDir_+DMBVME_FIRMWARE_FILENAME;
      //
      thisDMB->epromload_broadcast(VPROM,DMBVmeFirmware.c_str(),1,prombrdname,2);
      usleep(200);
      cout <<" The DMB Number: "<<idmb<<" is in Slot Number: "<<dmbVector[idmb]->slot()<<endl;
      //  cout <<" This DMB is programmed to board number: "<<boardnumber<<endl<<endl;
    }
    //
  }

  }
  SetCurrentCrate(this_crate_no_);

  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateConfig::ReadAllCfebPromUserid (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  //implement the CFEB PROM USER_CODE Readback
  //
  std::cout << "CFEB PROM USER_CODE Readback " << std::endl;

  for(unsigned cv=0; cv<crateVector.size(); cv++) {
    SetCurrentCrate(cv);
    std::cout << "For Crate " << ThisCrateID_ << " : " << std::endl;

  usleep(200);
  for (unsigned idmb=0;idmb<dmbVector.size();idmb++) {
    //
    if ((dmbVector[idmb]->slot())<22) {
      DAQMB * thisDMB=dmbVector[idmb];
      cout <<" The DMB Number: "<<idmb<<" is in Slot Number: "<<dmbVector[idmb]->slot()<<endl;
      //loop over the cfebs
      //define CFEBs
      vector <CFEB> thisCFEBs=thisDMB->cfebs();
      //
      for (unsigned i=0;i<thisCFEBs.size();i++) {
	CFEBBoardNumber[cv][idmb][i]=thisDMB->febpromuser(thisCFEBs[i]);
	cout <<" This CFEB Board Number: "<<CFEBBoardNumber[cv][idmb][i]<<endl;
      }
      //
      cout <<endl;
    }
    //
  }

  }
  SetCurrentCrate(this_crate_no_);

  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateConfig::LoadAllCfebPromUserid (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  //implement the CFEB PROM USER_CODE programming
  //
  std::cout << "CFEB PROM USER_CODE Programming " << std::endl;

  for(unsigned cv=0; cv<crateVector.size(); cv++) {
    SetCurrentCrate(cv);
    std::cout << "For Crate " << ThisCrateID_ << " : " << std::endl;

  for (unsigned idmb=0;idmb<dmbVector.size();idmb++) {
    //
    if ((dmbVector[idmb]->slot())<22) {
      DAQMB * thisDMB=dmbVector[idmb];
      Chamber * thisChamber=chamberVector[idmb];
      cout <<" The DMB Number: "<<idmb<<" is in Slot Number: "<<dmbVector[idmb]->slot()<<endl;
      //loop over the cfebs
      //define CFEBs
      vector <CFEB> thisCFEBs=thisDMB->cfebs();
      //
      for (unsigned i=0;i<thisCFEBs.size();i++) {
	char promid[4];
	unsigned long int boardid=CFEBBoardNumber[cv][idmb][i];
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
	std::string chamber=thisChamber->GetLabel();
	int cfebID=brddb->ChamberToCFEBID(chamber,i+1);
	//the id readback from CFEB
	promid[0]=boardid&0xff;
	promid[1]=(boardid>>8)&0xff;
	promid[2]=(boardid>>16)&0xff;
	promid[3]=(boardid>>24)&0xff;
	//
	//the ID readback from database
	if (((boardid&0x00000fff)==0) ||
	    ((boardid&0x00000fff)==0xfff) ||
            ((boardid&0x00000fff)==0xaad)) {
	   promid[0]=cfebID&0xff;
	   cout<<" CFEB board number reprogram from Database ..."<<endl;
	}
	promid[1]=(cfebID>>8)&0x0f;
	promid[2]=0xeb;
	promid[3]=0xcf;
	int newcfebid;
	newcfebid=(promid[0]&0xff)+((promid[1]<<8)&0xff00);
        //
	std::string CFEBFirmware = FirmwareDir_+CFEB_FIRMWARE_FILENAME;
	//
	thisDMB->epromload_broadcast(thisCFEBs[i].promDevice(),CFEBFirmware.c_str(),1,promid,2);
	usleep(200);
	        cout <<" This CFEB Board Number is set to: "<<newcfebid;
          cout <<"     was set to: "<<(boardid&0xffff)<<endl;
	cout <<" This CFEB Board Number is set to: CFEB"<<hex<<((promid[1])&0xff)<<((promid[0])&0xff)<<" was set to: "<<hex<<boardid<<endl;
      }
      cout <<endl;
    }
    //
  }

  }
  SetCurrentCrate(this_crate_no_);

  return createReply(message);
}


xoap::MessageReference EmuPeripheralCrateConfig::EnableJtagWriteALCT192 (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  // enables write to JTAG user Register on TMBs attached to ALCT192
  //
  std::cout << "EnableJtagWriteALCT192 to TMBs in slots..." << std::endl;
  //

  for(unsigned cv=0; cv<crateVector.size(); cv++) {
    SetCurrentCrate(cv);
    std::cout << "For Crate " << ThisCrateID_ << " : " << std::endl;

  for (unsigned itmb=0; itmb<tmbVector.size();itmb++) {
    ALCTController * thisALCT = tmbVector[itmb]->alctController();
    //
    if ( (thisALCT->GetChamberType()).find("ME13") != string::npos ) {
      std::cout << "YES slot " << tmbVector[itmb]->slot() << std::endl;
      tmbVector[itmb]->SetJtagDisableWriteToAdr10(0);
    } else {
      std::cout << "NO slot " << tmbVector[itmb]->slot() << std::endl;
      tmbVector[itmb]->SetJtagDisableWriteToAdr10(1);
    }
    tmbVector[itmb]->WriteRegister(0xD4);
    //tmbVector[itmb]->ReadRegister(0xD4);
    //tmbVector[itmb]->PrintTMBRegister(0xD4);
    //
    //unsigned short int BootData;
    //tmbVector[itmb]->tmb_get_boot_reg(&BootData);
    //tmbVector[itmb]->PrintBootRegister();
  }
  //
  std::cout << std::endl;

  }
  SetCurrentCrate(this_crate_no_);

  //
  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateConfig::EnableJtagWriteALCT288 (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  // enables write to JTAG user Register on TMBs attached to ALCT288
  //
  std::cout << "EnableJtagWriteALCT288 Forward Negative to TMBs in slots..." << std::endl;
  //

  for(unsigned cv=0; cv<crateVector.size(); cv++) {
    SetCurrentCrate(cv);
    std::cout << "For Crate " << ThisCrateID_ << " : " << std::endl;

  for (unsigned itmb=0; itmb<tmbVector.size();itmb++) {
    ALCTController * thisALCT = tmbVector[itmb]->alctController();
    //
    if ( (thisALCT->GetChamberType()).find("ME11") != string::npos                          &&
	 (thisALCT->GetExpectedFastControlBackwardForwardType() == FORWARD_FIRMWARE_TYPE)   &&
	 (thisALCT->GetExpectedFastControlNegativePositiveType() == NEGATIVE_FIRMWARE_TYPE) ) {
      std::cout << "YES slot " << tmbVector[itmb]->slot() << std::endl;
      tmbVector[itmb]->SetJtagDisableWriteToAdr10(0);
    } else {
      std::cout << "NO slot " << tmbVector[itmb]->slot() << std::endl;
      tmbVector[itmb]->SetJtagDisableWriteToAdr10(1);
    }
    tmbVector[itmb]->WriteRegister(0xD4);
    //tmbVector[itmb]->ReadRegister(0xD4);
    //tmbVector[itmb]->PrintTMBRegister(0xD4);
    //
    //unsigned short int BootData;
    //tmbVector[itmb]->tmb_get_boot_reg(&BootData);
    //tmbVector[itmb]->PrintBootRegister();
  }
  //
  std::cout << std::endl;

  }
  SetCurrentCrate(this_crate_no_);

  //
  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateConfig::EnableJtagWriteALCT288bn (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  // enables write to JTAG user Register on TMBs attached to ALCT288bn
  //
  std::cout << "EnableJtagWriteALCT288 Backwards Negative to TMBs in slots..." << std::endl;
  //

  for(unsigned cv=0; cv<crateVector.size(); cv++) {
    SetCurrentCrate(cv);
    std::cout << "For Crate " << ThisCrateID_ << " : " << std::endl;

  for (unsigned itmb=0; itmb<tmbVector.size();itmb++) {
    ALCTController * thisALCT = tmbVector[itmb]->alctController();
    //
    if ( (thisALCT->GetChamberType()).find("ME11") != string::npos                          &&
	 (thisALCT->GetExpectedFastControlBackwardForwardType() == BACKWARD_FIRMWARE_TYPE)  &&
	 (thisALCT->GetExpectedFastControlNegativePositiveType() == NEGATIVE_FIRMWARE_TYPE) ) {
      std::cout << "YES slot " << tmbVector[itmb]->slot() << std::endl;
      tmbVector[itmb]->SetJtagDisableWriteToAdr10(0);
    } else {
      std::cout << "NO slot " << tmbVector[itmb]->slot() << std::endl;
      tmbVector[itmb]->SetJtagDisableWriteToAdr10(1);
    }
    tmbVector[itmb]->WriteRegister(0xD4);
    //tmbVector[itmb]->ReadRegister(0xD4);
    //tmbVector[itmb]->PrintTMBRegister(0xD4);
    //
    //unsigned short int BootData;
    //tmbVector[itmb]->tmb_get_boot_reg(&BootData);
    //tmbVector[itmb]->PrintBootRegister();
  }
  //
  std::cout << std::endl;

  }
  SetCurrentCrate(this_crate_no_);

  //
  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateConfig::EnableJtagWriteALCT288bp (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  // enables write to JTAG user Register on TMBs attached to ALCT288bp
  //
  std::cout << "EnableJtagWriteALCT288 Backwards Positive to TMBs in slots..." << std::endl;
  //

  for(unsigned cv=0; cv<crateVector.size(); cv++) {
    SetCurrentCrate(cv);
    std::cout << "For Crate " << ThisCrateID_ << " : " << std::endl;

  for (unsigned itmb=0; itmb<tmbVector.size();itmb++) {
    ALCTController * thisALCT = tmbVector[itmb]->alctController();
    //
    if ( (thisALCT->GetChamberType()).find("ME11") != string::npos                          &&
	 (thisALCT->GetExpectedFastControlBackwardForwardType() == BACKWARD_FIRMWARE_TYPE)  &&
	 (thisALCT->GetExpectedFastControlNegativePositiveType() == POSITIVE_FIRMWARE_TYPE) ) {
      std::cout << "YES slot " << tmbVector[itmb]->slot() << std::endl;
      tmbVector[itmb]->SetJtagDisableWriteToAdr10(0);
    } else {
      std::cout << "NO slot " << tmbVector[itmb]->slot() << std::endl;
      tmbVector[itmb]->SetJtagDisableWriteToAdr10(1);
    }
    tmbVector[itmb]->WriteRegister(0xD4);
    //tmbVector[itmb]->ReadRegister(0xD4);
    //tmbVector[itmb]->PrintTMBRegister(0xD4);
    //
    //unsigned short int BootData;
    //tmbVector[itmb]->tmb_get_boot_reg(&BootData);
    //tmbVector[itmb]->PrintBootRegister();
  }
  //
  std::cout << std::endl;

  }
  SetCurrentCrate(this_crate_no_);

  //
  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateConfig::EnableJtagWriteALCT288fp (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  // enables write to JTAG user Register on TMBs attached to ALCT288fp
  //
  std::cout << "EnableJtagWriteALCT288 Forward Positive to TMBs in slots..." << std::endl;
  //

  for(unsigned cv=0; cv<crateVector.size(); cv++) {
    SetCurrentCrate(cv);
    std::cout << "For Crate " << ThisCrateID_ << " : " << std::endl;

  for (unsigned itmb=0; itmb<tmbVector.size();itmb++) {
    ALCTController * thisALCT = tmbVector[itmb]->alctController();
    //
    if ( (thisALCT->GetChamberType()).find("ME11") != string::npos                          &&
	 (thisALCT->GetExpectedFastControlBackwardForwardType() == FORWARD_FIRMWARE_TYPE)  &&
	 (thisALCT->GetExpectedFastControlNegativePositiveType() == POSITIVE_FIRMWARE_TYPE) ) {
      std::cout << "YES slot " << tmbVector[itmb]->slot() << std::endl;
      tmbVector[itmb]->SetJtagDisableWriteToAdr10(0);
    } else {
      std::cout << "NO slot " << tmbVector[itmb]->slot() << std::endl;
      tmbVector[itmb]->SetJtagDisableWriteToAdr10(1);
    }
    tmbVector[itmb]->WriteRegister(0xD4);
    //tmbVector[itmb]->ReadRegister(0xD4);
    //tmbVector[itmb]->PrintTMBRegister(0xD4);
    //
    //unsigned short int BootData;
    //tmbVector[itmb]->tmb_get_boot_reg(&BootData);
    //tmbVector[itmb]->PrintBootRegister();
  }
  //
  std::cout << std::endl;

  }
  SetCurrentCrate(this_crate_no_);

  //
  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateConfig::EnableJtagWriteALCT384 (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  // enables write to JTAG user Register on TMBs attached to ALCT384
  //
  std::cout << "EnableJtagWriteALCT384 to TMBs in slots..." << std::endl;
  //

  for(unsigned cv=0; cv<crateVector.size(); cv++) {
    SetCurrentCrate(cv);
    std::cout << "For Crate " << ThisCrateID_ << " : " << std::endl;

  for (unsigned itmb=0; itmb<tmbVector.size();itmb++) {
    ALCTController * thisALCT = tmbVector[itmb]->alctController();
    //
    if ( (thisALCT->GetChamberType()).find("ME12") != string::npos ||
	 (thisALCT->GetChamberType()).find("ME22") != string::npos ) {
      std::cout << "YES slot " << tmbVector[itmb]->slot() << std::endl;
      tmbVector[itmb]->SetJtagDisableWriteToAdr10(0);
    } else {
      std::cout << "NO slot " << tmbVector[itmb]->slot() << std::endl;
      tmbVector[itmb]->SetJtagDisableWriteToAdr10(1);
    }
    tmbVector[itmb]->WriteRegister(0xD4);
    //tmbVector[itmb]->ReadRegister(0xD4);
    //tmbVector[itmb]->PrintTMBRegister(0xD4);
    //
    //unsigned short int BootData;
    //tmbVector[itmb]->tmb_get_boot_reg(&BootData);
    //tmbVector[itmb]->PrintBootRegister();
  }
  //
  std::cout << std::endl;

  }
  SetCurrentCrate(this_crate_no_);

  //
  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateConfig::EnableJtagWriteALCT384Mirror (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  // enables write to JTAG user Register on TMBs attached to ALCT384Mirror
  //
  std::cout << "EnableJtagWriteALCT384Mirror to TMBs in slots..." << std::endl;
  //

  for(unsigned cv=0; cv<crateVector.size(); cv++) {
    SetCurrentCrate(cv);
    std::cout << "For Crate " << ThisCrateID_ << " : " << std::endl;

  for (unsigned itmb=0; itmb<tmbVector.size();itmb++) {
    ALCTController * thisALCT = tmbVector[itmb]->alctController();
    //
    if ( (thisALCT->GetChamberType()).find("ME32") != string::npos ) {
      std::cout << "YES slot " << tmbVector[itmb]->slot() << std::endl;
      tmbVector[itmb]->SetJtagDisableWriteToAdr10(0);
    } else {
      std::cout << "NO slot " << tmbVector[itmb]->slot() << std::endl;
      tmbVector[itmb]->SetJtagDisableWriteToAdr10(1);
    }
    tmbVector[itmb]->WriteRegister(0xD4);
    //tmbVector[itmb]->ReadRegister(0xD4);
    //tmbVector[itmb]->PrintTMBRegister(0xD4);
    //
    //unsigned short int BootData;
    //tmbVector[itmb]->tmb_get_boot_reg(&BootData);
    //tmbVector[itmb]->PrintBootRegister();
  }
  //
  std::cout << std::endl;

  }
  SetCurrentCrate(this_crate_no_);

  //
  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateConfig::EnableJtagWriteALCT576Mirror (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  // enables write to JTAG user Register on TMBs attached to ALCT576Mirror
  //
  std::cout << "EnableJtagWriteALCT576Mirror to TMBs in slots..." << std::endl;
  //

  for(unsigned cv=0; cv<crateVector.size(); cv++) {
    SetCurrentCrate(cv);
    std::cout << "For Crate " << ThisCrateID_ << " : " << std::endl;

  for (unsigned itmb=0; itmb<tmbVector.size();itmb++) {
    ALCTController * thisALCT = tmbVector[itmb]->alctController();
    //
    if ( (thisALCT->GetChamberType()).find("ME31") != string::npos ||
	 (thisALCT->GetChamberType()).find("ME41") != string::npos ) {
      std::cout << "YES slot " << tmbVector[itmb]->slot() << std::endl;
      tmbVector[itmb]->SetJtagDisableWriteToAdr10(0);
    } else {
      std::cout << "NO slot " << tmbVector[itmb]->slot() << std::endl;
      tmbVector[itmb]->SetJtagDisableWriteToAdr10(1);
    }
    tmbVector[itmb]->WriteRegister(0xD4);
    //tmbVector[itmb]->ReadRegister(0xD4);
    //tmbVector[itmb]->PrintTMBRegister(0xD4);
    //
    //unsigned short int BootData;
    //tmbVector[itmb]->tmb_get_boot_reg(&BootData);
    //tmbVector[itmb]->PrintBootRegister();
  }
  //
  std::cout << std::endl;

  }
  SetCurrentCrate(this_crate_no_);

  //
  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateConfig::EnableJtagWriteALCT672 (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  // enables write to JTAG user Register on TMBs attached to ALCT672
  //
  std::cout << "EnableJtagWriteALCT672 to TMBs in slots..." << std::endl;
  //

  for(unsigned cv=0; cv<crateVector.size(); cv++) {
    SetCurrentCrate(cv);
    std::cout << "For Crate " << ThisCrateID_ << " : " << std::endl;

  for (unsigned itmb=0; itmb<tmbVector.size();itmb++) {
    ALCTController * thisALCT = tmbVector[itmb]->alctController();
    //
    if ( (thisALCT->GetChamberType()).find("ME21") != string::npos ) {
      std::cout << "YES slot " << tmbVector[itmb]->slot() << std::endl;
      tmbVector[itmb]->SetJtagDisableWriteToAdr10(0);
    } else {
      std::cout << "NO slot " << tmbVector[itmb]->slot() << std::endl;
      tmbVector[itmb]->SetJtagDisableWriteToAdr10(1);
    }
    tmbVector[itmb]->WriteRegister(0xD4);
    //tmbVector[itmb]->ReadRegister(0xD4);
    //tmbVector[itmb]->PrintTMBRegister(0xD4);
    //
    //unsigned short int BootData;
    //tmbVector[itmb]->tmb_get_boot_reg(&BootData);
    //tmbVector[itmb]->PrintBootRegister();
  }
  //
  std::cout << std::endl;

  }
  SetCurrentCrate(this_crate_no_);

  //
  return createReply(message);
}

// sending and receiving soap commands
////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::PCsendCommand(string command, string klass)
  throw (xoap::exception::Exception, xdaq::exception::Exception){
  //
  //This is copied from CSCSupervisor::sendcommand;
  //
  // Exceptions:
  // xoap exceptions are thrown by analyzeReply() for SOAP faults.
  // xdaq exceptions are thrown by postSOAP() for socket level errors.
  //
  // find applications
  std::set<xdaq::ApplicationDescriptor *> apps;
  //
  try {
    apps = getApplicationContext()->getDefaultZone()->getApplicationDescriptors(klass);
  }
  // 
  catch (xdaq::exception::ApplicationDescriptorNotFound e) {
    return; // Do nothing if the target doesn't exist
  }
  //
  // prepare a SOAP message
  xoap::MessageReference message = PCcreateCommandSOAP(command);
  xoap::MessageReference reply;
  xdaq::ApplicationDescriptor *ori=this->getApplicationDescriptor();
  //
  // send the message one-by-one
  std::set<xdaq::ApplicationDescriptor *>::iterator i = apps.begin();
  for (; i != apps.end(); ++i) {
    // postSOAP() may throw an exception when failed.
    reply = getApplicationContext()->postSOAP(message, *ori, *(*i));
    //
    //      PCanalyzeReply(message, reply, *i);
  }
}

xoap::MessageReference EmuPeripheralCrateConfig::PCcreateCommandSOAP(string command) {
  //
  //This is copied from CSCSupervisor::createCommandSOAP
  //
  xoap::MessageReference message = xoap::createMessage();
  xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
  xoap::SOAPName name = envelope.createName(command, "xdaq", "urn:xdaq-soap:3.0");
  envelope.getBody().addBodyElement(name);
  //
  return message;
}

// provides factory method for instantion of HellWorld application
//
XDAQ_INSTANTIATOR_IMPL(EmuPeripheralCrateConfig)
//
