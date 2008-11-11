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
#include "CCB.h"

const std::string       CFEB_FIRMWARE_FILENAME = "cfeb/cfeb_pro.svf";
const std::string       CFEB_VERIFY_FILENAME = "cfeb/cfeb_verify.svf";
//
const std::string       DMB_FIRMWARE_FILENAME    = "dmb/dmb6cntl_pro.svf";
const std::string       DMBVME_FIRMWARE_FILENAME = "dmb/dmb6vme_pro.svf";
const std::string       VMECC_FIRMWARE_DIR = "vcc"; 
//const std::string       DMBVME_FIRMWARE_FILENAME = "dmb/dmb6vme_v11_r1.svf";
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
const std::string ALCT_FIRMWARE_FILENAME_ME11 = "alct288/alct288.svf";//
const std::string ALCT_FIRMWARE_FILENAME_ME11_BACKWARD_NEGATIVE = "alct288bn/alct288bn.svf";//
const std::string ALCT_FIRMWARE_FILENAME_ME11_BACKWARD_POSITIVE = "alct288bp/alct288bp.svf";//
const std::string ALCT_FIRMWARE_FILENAME_ME11_FORWARD_POSITIVE  = "alct288fp/alct288fp.svf";//
const std::string ALCT_FIRMWARE_FILENAME_ME12 = "alct384/alct384.svf";//
const std::string ALCT_FIRMWARE_FILENAME_ME13 = "alct192/alct192.svf";//
const std::string ALCT_FIRMWARE_FILENAME_ME21 = "alct672/alct672.svf";//
const std::string ALCT_FIRMWARE_FILENAME_ME22 = "alct384/alct384.svf";//
const std::string ALCT_FIRMWARE_FILENAME_ME31 = "alct576mirror/alct576mirror.svf";//
const std::string ALCT_FIRMWARE_FILENAME_ME32 = "alct384mirror/alct384mirror.svf";//
const std::string ALCT_FIRMWARE_FILENAME_ME41 = "alct576mirror/alct576mirror.svf";//
//
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
  crate_controller_status = -1;
  //
  all_crates_ok = -1;
  for (int i=0; i<60; i++) {
    crate_check_ok[i] = -1;
    ccb_check_ok[i] = -1;
    mpc_check_ok[i] = -1;
    for (int j=0; j<9; j++) {
      alct_check_ok[i][j] = -1;
      tmb_check_ok[i][j] = -1;
      dmb_check_ok[i][j] = -1;
      time_since_reset[i][j] = -1;
    }
  }
  //
  crates_firmware_ok = -1;
  for (int i=0; i<60; i++) {
    crate_firmware_ok[i] = -1;
    ccb_firmware_ok[i] = -1;
    mpc_firmware_ok[i] = -1;
    for (int j=0; j<9; j++) {
      alct_firmware_ok[i][j] = -1;
      tmb_firmware_ok[i][j] = -1;
      dmb_vme_firmware_ok[i][j] = -1;
      dmb_control_firmware_ok[i][j] = -1;
      for (int k=0; k<5; k++) 
	cfeb_firmware_ok[i][j][k] = -1;
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
  //---------------------------------
  // bind check crate configuration
  //---------------------------------
  xgi::bind(this,&EmuPeripheralCrateConfig::CheckConfigurationPage,"CheckConfigurationPage");
  xgi::bind(this,&EmuPeripheralCrateConfig::CheckTimeSinceHardReset,"CheckTimeSinceHardReset");
  xgi::bind(this,&EmuPeripheralCrateConfig::CheckCratesConfiguration, "CheckCratesConfiguration");
  xgi::bind(this,&EmuPeripheralCrateConfig::CheckCrateConfiguration, "CheckCrateConfiguration");
  xgi::bind(this,&EmuPeripheralCrateConfig::CheckCratesFirmware, "CheckCratesFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::CheckCrateFirmware, "CheckCrateFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::PowerOnFixCFEB, "PowerOnFixCFEB");
  xgi::bind(this,&EmuPeripheralCrateConfig::FixCFEB, "FixCFEB");
  //
  //------------------------------
  // bind crate utilities
  //------------------------------
  xgi::bind(this,&EmuPeripheralCrateConfig::TmbMPCTest, "TmbMPCTest");
  xgi::bind(this,&EmuPeripheralCrateConfig::MPCSafeWindowScan, "MPCSafeWindowScan");
  //
  xgi::bind(this,&EmuPeripheralCrateConfig::CheckCrates, "CheckCrates");
  //lsdxgi::bind(this,&EmuPeripheralCrateConfig::CheckSwitch,"CheckSwitch");
  xgi::bind(this,&EmuPeripheralCrateConfig::CrateSelection, "CrateSelection");
  xgi::bind(this,&EmuPeripheralCrateConfig::setRawConfFile, "setRawConfFile");
  xgi::bind(this,&EmuPeripheralCrateConfig::UploadConfFile, "UploadConfFile");
  xgi::bind(this,&EmuPeripheralCrateConfig::SetUnsetRatio, "SetUnsetRatio");
  xgi::bind(this,&EmuPeripheralCrateConfig::SetUnsetAutoRefresh, "SetUnsetAutoRefresh");
  xgi::bind(this,&EmuPeripheralCrateConfig::DefineConfiguration, "DefineConfiguration");
  xgi::bind(this,&EmuPeripheralCrateConfig::ReadCCBRegister, "ReadCCBRegister");
  xgi::bind(this,&EmuPeripheralCrateConfig::ReadTTCRegister, "ReadTTCRegister");
  xgi::bind(this,&EmuPeripheralCrateConfig::HardReset, "HardReset");
  xgi::bind(this,&EmuPeripheralCrateConfig::testTMB, "testTMB");
  xgi::bind(this,&EmuPeripheralCrateConfig::CCBLoadFirmware, "CCBLoadFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::CrateConfiguration, "CrateConfiguration");
  xgi::bind(this,&EmuPeripheralCrateConfig::CrateTests, "CrateTests");
  xgi::bind(this,&EmuPeripheralCrateConfig::ChamberTests, "ChamberTests");
  xgi::bind(this,&EmuPeripheralCrateConfig::ConfigAllCrates, "ConfigAllCrates");
  xgi::bind(this,&EmuPeripheralCrateConfig::FastConfigCrates, "FastConfigCrates");
  xgi::bind(this,&EmuPeripheralCrateConfig::FastConfigOne, "FastConfigOne");
  //
  xgi::bind(this,&EmuPeripheralCrateConfig::MeasureL1AsAndDAVsForCrate,"MeasureL1AsAndDAVsForCrate");
  xgi::bind(this,&EmuPeripheralCrateConfig::MeasureL1AsAndDAVsForChamber,"MeasureL1AsAndDAVsForChamber");
  xgi::bind(this,&EmuPeripheralCrateConfig::MeasureL1AsForCrate,"MeasureL1AsForCrate");
  xgi::bind(this,&EmuPeripheralCrateConfig::MeasureDAVsForCrate,"MeasureDAVsForCrate");
  //
  xgi::bind(this,&EmuPeripheralCrateConfig::MPCLoadFirmware, "MPCLoadFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::StartPRBS, "StartPRBS");
  xgi::bind(this,&EmuPeripheralCrateConfig::StopPRBS, "StopPRBS");

  //-----------------------------------------------
  // VME Controller routines
  //-----------------------------------------------
  xgi::bind(this,&EmuPeripheralCrateConfig::ControllerUtils_Xfer, "ControllerUtils_Xfer");  
  xgi::bind(this,&EmuPeripheralCrateConfig::ControllerUtils, "ControllerUtils");  
  xgi::bind(this,&EmuPeripheralCrateConfig::VMECCGUI_GoTo_General,  "VMECCGUI_GoTo_General");
  xgi::bind(this,&EmuPeripheralCrateConfig::VMECCGUI_GoTo_Intermediate,  "VMECCGUI_GoTo_Intermediate");
  xgi::bind(this,&EmuPeripheralCrateConfig::VMECCGUI_GoTo_Expert,  "VMECCGUI_GoTo_Expert");
  xgi::bind(this,&EmuPeripheralCrateConfig::VMECCGUI_firmware_utils,  "VMECCGUI_firmware_utils");
  xgi::bind(this,&EmuPeripheralCrateConfig::VMECCGUI_cnfg_utils,  "VMECCGUI_cnfg_utils");
  xgi::bind(this,&EmuPeripheralCrateConfig::VMECCGUI_MAC_utils,  "VMECCGUI_MAC_utils");
  xgi::bind(this,&EmuPeripheralCrateConfig::VMECCGUI_FIFO_utils,  "VMECCGUI_FIFO_utils");
  xgi::bind(this,&EmuPeripheralCrateConfig::VMECCGUI_pkt_send,  "VMECCGUI_pkt_send");
  xgi::bind(this,&EmuPeripheralCrateConfig::VMECCGUI_pkt_rcv,  "VMECCGUI_pkt_rcv");
  xgi::bind(this,&EmuPeripheralCrateConfig::VMECCGUI_misc_utils,  "VMECCGUI_misc_utils");
  xgi::bind(this,&EmuPeripheralCrateConfig::VCC_VME_DO,  "VCC_VME_DO");
  xgi::bind(this,&EmuPeripheralCrateConfig::VCC_VME_FILL,  "VCC_VME_FILL");
  xgi::bind(this,&EmuPeripheralCrateConfig::VCC_CRSEL_DO,  "VCC_CRSEL_DO");
  xgi::bind(this,&EmuPeripheralCrateConfig::VCC_PSWD_DO,  "VCC_PSWD_DO");
  xgi::bind(this,&EmuPeripheralCrateConfig::VCC_CMNTSK_DO,  "VCC_CMNTSK_DO");
  xgi::bind(this,&EmuPeripheralCrateConfig::VCC_FRMUTIL_DO,  "VCC_FRMUTIL_DO");
  xgi::bind(this,&EmuPeripheralCrateConfig::VCC_CNFG_DO,  "VCC_CNFG_DO");
  xgi::bind(this,&EmuPeripheralCrateConfig::VCC_MAC_DO,  "VCC_MAC_DO");
  xgi::bind(this,&EmuPeripheralCrateConfig::VCC_FIFO_DO,  "VCC_FIFO_DO");
  xgi::bind(this,&EmuPeripheralCrateConfig::VCC_PKTSND_DO,  "VCC_PKTSND_DO");
  xgi::bind(this,&EmuPeripheralCrateConfig::VCC_PKTRCV_DO,  "VCC_PKTRCV_DO");
  xgi::bind(this,&EmuPeripheralCrateConfig::VCC_MISC_DO,  "VCC_MISC_DO");

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
  xgi::bind(this,&EmuPeripheralCrateConfig::CFEBReadFirmware, "CFEBReadFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::CFEBLoadFirmwareID, "CFEBLoadFirmwareID");
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBCheckConfiguration, "DMBCheckConfiguration");
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
  xgi::bind(this,&EmuPeripheralCrateConfig::CheckCrateControllerFromTMBPage, "CheckCrateControllerFromTMBPage");
  xgi::bind(this,&EmuPeripheralCrateConfig::LoadTMBFirmware, "LoadTMBFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::LoadCrateTMBFirmware, "LoadCrateTMBFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::CCBHardResetFromTMBPage, "CCBHardResetFromTMBPage");
  xgi::bind(this,&EmuPeripheralCrateConfig::CheckTMBFirmware, "CheckTMBFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::ClearTMBBootReg, "ClearTMBBootReg");
  xgi::bind(this,&EmuPeripheralCrateConfig::LoadALCTFirmware, "LoadALCTFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::LoadCrateALCTFirmware, "LoadCrateALCTFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::LoadRATFirmware, "LoadRATFirmware");
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
  // SOAP for Monitor controll
//  xoap::bind(this,&EmuPeripheralCrateConfig::MonitorStart      ,"MonitorStart",XDAQ_NS_URI);
//  xoap::bind(this,&EmuPeripheralCrateConfig::MonitorStop      ,"MonitorStop",XDAQ_NS_URI);
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
  xml_or_db = -1;  /* actual configuration source: 0: xml, 1: db, -1: unknown or error */
  XML_or_DB_ = "xml";
  EMU_config_ID_ = "1000001";
  xmlFile_ = "config.xml" ;
  //
  for(unsigned int dmb=0; dmb<9; dmb++) {
    L1aLctCounter_.push_back(0);
    CfebDavCounter_.push_back(0);
    TmbDavCounter_.push_back(0);
    AlctDavCounter_.push_back(0);
  }
  //
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
  this->getApplicationInfoSpace()->fireItemAvailable("XMLorDB", &XML_or_DB_);
  this->getApplicationInfoSpace()->fireItemAvailable("EmuConfigurationID", &EMU_config_ID_);
  this->getApplicationInfoSpace()->fireItemAvailable("runNumber", &runNumber_);
  this->getApplicationInfoSpace()->fireItemAvailable("xmlFileName", &xmlFile_);
  this->getApplicationInfoSpace()->fireItemAvailable("CalibrationState", &CalibrationState_);
  this->getApplicationInfoSpace()->fireItemAvailable("Calibtype", &CalibType_);
  this->getApplicationInfoSpace()->fireItemAvailable("Calibnumber", &CalibNumber_);
  
  // for XMAS minotoring:

  Monitor_On_ = false;
  Monitor_Ready_ = false;

  global_config_states[0]="UnConfigured";
  global_config_states[1]="Configuring";
  global_config_states[2]="Configured";
  global_run_states[0]="Halted";
  global_run_states[1]="Enabled";
  current_config_state_=0;
  current_run_state_=0;
  total_crates_=0;
  this_crate_no_=0;

  prbs_test_ = false;
  brddb= new emu::db::BoardsDB();

  parsed=0;
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
  std::string CheckCrates = toolbox::toString("/%s/CheckCrates",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CheckCrates) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Check Crate Controllers") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();

  /* lsd  *out << cgicc::td();
  std::string CheckSwitch = toolbox::toString("/%s/CheckSwitch",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CheckSwitch) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Switch Statistics") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td(); lsd*/

  *out << cgicc::td();
  std::string FastConfigureAll = toolbox::toString("/%s/FastConfigCrates",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",FastConfigureAll) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Crates Power-up Init") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();

  std::cout << " PowerOnFixCFEB " << std::endl;
  *out << cgicc::td();
  std::string PowerOnFixCFEB = toolbox::toString("/%s/PowerOnFixCFEB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",PowerOnFixCFEB) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Power On Fix CFEBs").set("style","color:blue") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();

  *out << cgicc::td();
  std::string CheckConfigurationPage = toolbox::toString("/%s/CheckConfigurationPage",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::a("[Check Configuration]").set("href",CheckConfigurationPage) << std::endl;
  *out << cgicc::td();

  *out << cgicc::table();

  *out << cgicc::br() << std::endl;

  *out << cgicc::table().set("border","0");
  *out << cgicc::td();
  std::string CrateConfigureAll = toolbox::toString("/%s/ConfigAllCrates",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CrateConfigureAll) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Write FLASH All Crates") << std::endl ;
  *out << cgicc::form() << cgicc::br() << std::endl ;;
  *out << cgicc::td();

  *out << cgicc::td();
  if(prbs_test_)
  {
    std::string StopPRBS = toolbox::toString("/%s/StopPRBS",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",StopPRBS) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Stop PRBS test").set("style","color:red") << std::endl ;
    *out << cgicc::form() << cgicc::br() << std::endl ;;
  } else
  {
    std::string StartPRBS = toolbox::toString("/%s/StartPRBS",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",StartPRBS) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Start PRBS test").set("style","color:blue") << std::endl ;
    *out << cgicc::form() << cgicc::br() << std::endl ;;
  }
  *out << cgicc::td();

  *out << cgicc::table();

  *out << cgicc::br() << std::endl ;
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
  *out << cgicc::form().set("action",
		     "/" + getApplicationDescriptor()->getURN() + "/CrateSelection") << std::endl;
  
  int n_keys = crateVector.size();
  
  *out << "Choose Crate: " << std::endl;
  *out << cgicc::select().set("name", "runtype") << std::endl;
  
  int selected_index = this_crate_no_;
  std::string CrateName;
  for (int i = 0; i < n_keys; ++i) {
    if(crateVector[i]->IsAlive())
      CrateName = crateVector[i]->GetLabel();
    else
      CrateName = crateVector[i]->GetLabel() + " NG";
    if (i == selected_index) {
      *out << cgicc::option()
	.set("value", CrateName)
	.set("selected", "");
    } else {
      *out << cgicc::option()
	.set("value", CrateName);
    }
    *out << CrateName << cgicc::option() << std::endl;
  }

  *out << cgicc::select() << std::endl;
  
  *out << cgicc::input().set("type", "submit")
    .set("name", "command")
    .set("value", "CrateSelection") << std::endl;
  *out << cgicc::form() << std::endl;
     
  //End select crate
 
  *out << cgicc::br()<< std::endl;
  std::cout << "Main Page: "<< std::dec << active_crates << "/" <<total_crates_ << " Crates" << std::endl;
  //
  if (tmbVector.size()>0 || dmbVector.size()>0) {
    //
    //
    // Crate Configuration
    //
    std::string CrateConfiguration = toolbox::toString("/%s/CrateConfiguration",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::a("[Crate Configuration]").set("href",CrateConfiguration) << std::endl;
    //
    std::string CrateTests = toolbox::toString("/%s/CrateTests",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::a("[Crate Tests]").set("href",CrateTests) << std::endl;
    //
    std::string CalibrationRuns = toolbox::toString("/%s/CalibrationRuns",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::a("[Calibration Runs]").set("href",CalibrationRuns) << std::endl;
    //
    *out << cgicc::br() << cgicc::br() << cgicc::table().set("border","0");
    //
    *out << cgicc::td();
    std::string FastConfigOne = toolbox::toString("/%s/FastConfigOne",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",FastConfigOne) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Power-up Init") << std::endl ;
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
    *out << cgicc::legend((("Logging"))).set("style","color:blue") ;
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
  if(xml_or_db==0)
  {
    *out << cgicc::b(cgicc::i("Configuration filename : ")) ;
    *out << xmlFile_.toString() << cgicc::br() << std::endl ;
    //
    std::string DefineConfiguration = toolbox::toString("/%s/DefineConfiguration",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::a("[Change Configuration File]").set("href",DefineConfiguration) << std::endl;
  }
  else if(xml_or_db==1)
  {
    *out << cgicc::b(cgicc::i("TStore EMU_config_ID : ")) ;
    *out << EMU_config_ID_.toString() << cgicc::br() << std::endl ;
  }
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
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<"MainPage"<<"\">" <<std::endl;
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
  std::cout<<"soap Apr.11, 2007 "<<std::endl;
  std::cout<<"Entered the EMUPERIPHERALCRATE.cc "<<std::endl;
  printf(" LSD: Entered Calibration \n"); 
  std::ostringstream test;
  message->writeTo(test);
  std::cout << test.str() << std::endl;
  printf(" Print calibtype \n");
  std::string junk = CalibType_;
  std::cout << junk << std::endl;
  printf(" Print calibnumber \n");
  std::cout << CalibNumber_ << std::endl;
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

     std::string in_value = cgi.getElement("runtype")->getValue(); 
     std::cout << "Select Crate " << in_value << std::endl;
     if(!in_value.empty())
     {
        int k=in_value.find(" ",0);
        std::string value = (k) ? in_value.substr(0,k):in_value;
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
	std::cout << "setRawConfFile" << std::endl ;
	//
	cgicc::Cgicc cgi(in);
	//
	std::ofstream TextFile ;
	TextFile.open("MyTextConfigurationFile.xml");
	TextFile << (cgi["Text"]->getValue()) ; 
	TextFile.close();
	//
	xmlFile_ = "MyTextConfigurationFile.xml" ;
	//
	std::cout << "Out setRawConfFile" << std::endl ;
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
    
    if(total_crates_<=0) return;
    current_config_state_=1;
    for(unsigned i=0; i< crateVector.size(); i++)
    {
        if(crateVector[i] && crateVector[i]->IsAlive()) crateVector[i]->configure(c);
    }
    current_config_state_=2;
    //
  }

bool EmuPeripheralCrateConfig::ParsingXML(){
  //
  LOG4CPLUS_INFO(getApplicationLogger(),"Parsing Configuration XML");
    //
  std::cout << "XML_or_DB: " << XML_or_DB_.toString() << std::endl;
  if(XML_or_DB_.toString() == "xml" || XML_or_DB_.toString() == "XML")
  {
    // Check if filename exists
    //
    if(xmlFile_.toString().find("http") == std::string::npos) 
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
    MyController = new emu::pc::EmuController();

    MyController->SetConfFile(xmlFile_.toString().c_str());
    MyController->init();
    MyController->NotInDCS();
    //
    emuEndcap_ = MyController->GetEmuEndcap();
    if(!emuEndcap_) return false;
    xml_or_db = 0;
  }
  else if (XML_or_DB_.toString() == "db" || XML_or_DB_.toString() == "DB")
  {
    // from TStore    
    // std::cout << "We are in db" << std::endl;
    myTStore = new emu::pc::EmuTStore(this);
    if(!myTStore)
    {  std::cout << "Can't create object EmuTStore" << std::endl;
       return false;  
    }
    emuEndcap_ = myTStore->getConfiguredEndcap(EMU_config_ID_.toString());   
    if(!emuEndcap_) 
    {  std::cout << "No EmuEndcap returned from TStore" << std::endl;
       return false;
    }
    xml_or_db = 1;
  }
  else
  {
    std::cout << "No valid XML_or_DB found..." << std::endl;
    return false;
  }
    crateVector = emuEndcap_->crates();
    //
    total_crates_=crateVector.size();
    if(total_crates_<=0) return false;
    this_crate_no_=0;

    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      //
      SetCurrentCrate(crate_number);
      for(unsigned i=0; i<dmbVector.size();i++) {
	OutputDMBTests[i][current_crate_] << "DMB-CFEB Tests " 
					  << thisCrate->GetChamber(dmbVector[i]->slot())->GetLabel().c_str() 
					  << " output:" << std::endl;
      }
      for(unsigned i=0; i<tmbVector.size();i++) {
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
    //
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
    //  
    tmbTestVector = InitTMBTests(thisCrate);
    //
    DefineFirmwareFilenames();
    //
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
        cr = crateVector[i]->vmeController()->SelfTest();
        if(!cr) 
        {  std::cout << "Exclude Crate " << crateVector[i]->GetLabel() 
                     << "--Dead Controller " << std::endl;
        }
        else
        {  cr=crateVector[i]->vmeController()->exist(13);
           if(!cr) std::cout << "Exclude Crate " << crateVector[i]->GetLabel() 
                     << "--No VME access " << std::endl;
        }
        crateVector[i]->SetLife( cr );
    }

    this->Default(in, out);
  }

/*lsd
void EmuPeripheralCrateConfig::CheckSwitch(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {
  //
  char Name[50] ;
  sprintf(Name,"Crate Statistics");
  //
  MyHeader(in,out,Name);
  pcswitch.fill_switch_statistics();
  pcswitch.fill_ping();
  *out << pcswitch.html_ping();
  *out << pcswitch.html_port_status();
  pcswitch.copy_stats_new2old();
}
lsd */


//
  // This one came from CrateUtils class which no longer exist. 
  // Better put into another class. Leave it here for now. 
  // Liu Dec.25, 2007
  std::vector<emu::pc::TMBTester> EmuPeripheralCrateConfig::InitTMBTests(emu::pc::Crate *MyCrate_)
  {
    std::vector<emu::pc::TMB *>       tmbVector = MyCrate_->tmbs();
    emu::pc::CCB * MyCCB_ = MyCrate_->ccb();
    std::vector<emu::pc::TMBTester>   result;
  //
    for( unsigned i=0; i< tmbVector.size(); i++) {
    //
       emu::pc::TMBTester tmp;
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
  void EmuPeripheralCrateConfig::setConfFile(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    try
      {
	//
	cgicc::Cgicc cgi(in);
	//
	cgicc::const_file_iterator file;
	file = cgi.getFile("xmlFileName");
	//
	std::cout << "GetFiles string" << std::endl ;
	//
	if(file != cgi.getFiles().end()) (*file).writeToStream(std::cout);
	//
	std::string XMLname = cgi["xmlFileName"]->getValue() ; 
	//
	std::cout << XMLname  << std::endl ;
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
	std::cout << "UploadConfFileUpload" << std::endl ;
	//
	cgicc::Cgicc cgi(in);
	//
	cgicc::const_file_iterator file;
	file = cgi.getFile("xmlFileNameUpload");
	//
	std::cout << "GetFiles" << std::endl ;
	//
	if(file != cgi.getFiles().end()) {
	  std::ofstream TextFile ;
	  TextFile.open("MyTextConfigurationFile.xml");
	  (*file).writeToStream(TextFile);
	  TextFile.close();
	}
	//
	xmlFile_ = "MyTextConfigurationFile.xml" ;
	//
	ParsingXML();
	//
	std::cout << "UploadConfFile done" << std::endl ;
	//
	this->Default(in,out);
	//
      }
    catch (const std::exception & e )
      {
	//XECPT_RAISE(xgi::exception::Exception, e.what());
      }
  }


void EmuPeripheralCrateConfig::StartPRBS(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //  
  std::cout << "Button: Start PRBS Test" << std::endl;
  //
  if(total_crates_>0)
  {
     for(unsigned i=0; i< crateVector.size(); i++) {
        if ( crateVector[i]->IsAlive() ) crateVector[i]->mpc()->enablePRBS();
     }
     prbs_test_=true;
  }
  this->Default(in, out);
}

void EmuPeripheralCrateConfig::StopPRBS(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //  
  std::cout << "Button: Start PRBS Test" << std::endl;
  //
  if(total_crates_>0)
  {
     for(unsigned i=0; i< crateVector.size(); i++) {
        if ( crateVector[i]->IsAlive() ) crateVector[i]->mpc()->disablePRBS();
     }
     prbs_test_=false;
  }
  this->Default(in, out);
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
  *out << CrateTestsOutput.str() << std::endl ;
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
    *out << "Slot " << std::setfill('0') << std::setw(2) << std::dec << ii << std::endl;
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
      std::string ControllerUtils_Xfer = toolbox::toString("/%s/ControllerUtils_Xfer?ccb=%d",getApplicationDescriptor()->getURN().c_str(),ii);
      *out << cgicc::a("Controller Utils").set("href",ControllerUtils_Xfer) << std::endl;
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
      if ( CCBBoardID_.find("-1") == std::string::npos ) {
	std::string CCBStatus = toolbox::toString("/%s/CCBStatus?ccb=%d",getApplicationDescriptor()->getURN().c_str(),ii);
	*out << cgicc::a("CCB Status").set("href",CCBStatus) << std::endl;
      }
      *out << cgicc::td();
      //
      *out << cgicc::td();
      if ( CCBBoardID_.find("-1") == std::string::npos ) {
	//sprintf(Name,"CCB Utils slot=%d",slot);
	std::string CCBUtils = toolbox::toString("/%s/CCBUtils?ccb=%d",getApplicationDescriptor()->getURN().c_str(),ii);
	*out << cgicc::a("CCB Utils").set("href",CCBUtils) << std::endl;
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
      if ( MPCBoardID_.find("-1") == std::string::npos && thisMPC ) {
	//
	*out << cgicc::td();
	std::string MPCStatus = toolbox::toString("/%s/MPCStatus?mpc=%d",getApplicationDescriptor()->getURN().c_str(),ii);
	*out << cgicc::a("MPC Status").set("href",MPCStatus) << std::endl;
	*out << cgicc::td();
	//
	*out << cgicc::td();
	std::string MPCUtils = toolbox::toString("/%s/MPCUtils?mpc=%d",getApplicationDescriptor()->getURN().c_str(),ii);
	*out << cgicc::a("MPC Utils").set("href",MPCUtils) << std::endl;
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
	if ( TMBBoardID_[i].find("-1") == std::string::npos ) {
	  std::string MonitorTMBTrigger = toolbox::toString("/%s/MonitorTMBTrigger?tmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	  *out << cgicc::a("Monitor TMB trigger").set("href",MonitorTMBTrigger) << std::endl;
	  *out << cgicc::td();
	  //
	  *out << cgicc::td();
	  std::string TMBStatus = toolbox::toString("/%s/TMBStatus?tmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	  *out << cgicc::a("TMB Status").set("href",TMBStatus) << std::endl;
	  //
	}
	*out << cgicc::td();
	//
	*out << cgicc::td();
	if ( TMBBoardID_[i].find("-1") == std::string::npos ) {
	  std::string TMBTests = toolbox::toString("/%s/TMBTests?tmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	  *out << cgicc::a("TMB Tests").set("href",TMBTests) << std::endl;
	}
	*out << cgicc::td();
	//
	*out << cgicc::td();
	if ( TMBBoardID_[i].find("-1") == std::string::npos ) {
	    std::string TMBUtils = toolbox::toString("/%s/TMBUtils?tmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	    *out << cgicc::a("TMB Utils").set("href",TMBUtils) << std::endl;
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
	    if ( TMBBoardID_[i].find("-1") == std::string::npos ) {
	      //
	      char Name[50];
	      sprintf(Name,"Chamber Tests: %s",(thisCrate->GetChamber(slot)->GetLabel()).c_str());
	      //
	      std::string ChamberTests = toolbox::toString("/%s/ChamberTests?tmb=%d&dmb=%d",getApplicationDescriptor()->getURN().c_str(),i,iii);
	      *out << cgicc::a(Name).set("href",ChamberTests) << std::endl;
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
	if ( DMBBoardID_[i].find("-1",0) == std::string::npos ) {
	  std::string DMBStatus = toolbox::toString("/%s/DMBStatus?dmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	  *out << cgicc::a("DMB Status").set("href",DMBStatus) << std::endl;
	  //
	}
	*out << cgicc::td();
	//
	*out << cgicc::td();
	if ( DMBBoardID_[i].find("-1",0) == std::string::npos ) {
	  std::string DMBTests = toolbox::toString("/%s/DMBTests?dmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	  *out << cgicc::a("DMB Tests").set("href",DMBTests) << std::endl;
	}
	*out << cgicc::td();
	//
	*out << cgicc::td();
	if ( DMBBoardID_[i].find("-1",0) == std::string::npos ) {
	  std::string DMBUtils = toolbox::toString("/%s/DMBUtils?dmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	  *out << cgicc::a("DMB Utils").set("href",DMBUtils) << std::endl;
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
  emu::pc::CalibDAQ calib(emuEndcap_);
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
  emu::pc::CalibDAQ calib(emuEndcap_);
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
  emu::pc::CalibDAQ calib(emuEndcap_);
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
  emu::pc::CalibDAQ calib(emuEndcap_);
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
  emu::pc::CalibDAQ calib(emuEndcap_);
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
  emu::pc::CalibDAQ calib(emuEndcap_);
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
  emu::pc::CalibDAQ calib(emuEndcap_);
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
  emu::pc::CalibDAQ calib(emuEndcap_);
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
  emu::pc::CalibDAQ calib(emuEndcap_);
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
  emu::pc::CalibDAQ calib(emuEndcap_);
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
  std::ostringstream output;
  output << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
  output << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  //
  cgicc::CgiEnvironment cgiEnvi(in);
  std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
  //
  if (AutoRefresh_) {
    *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"3; URL=/"
	 <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<std::endl;
  }
  //
  Page=cgiEnvi.getQueryString();
  std::string crate_name=Page.substr(0,Page.find("=", 0) );
  *out << cgicc::b("Crate: "+crate_name) << std::endl;
  std::vector<emu::pc::TMB*> myVector;
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
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"5; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<std::endl;
  //
  Page=cgiEnvi.getQueryString();
  std::string crate_name=Page.substr(0,Page.find("=", 0) );
  *out << cgicc::b("Crate: "+crate_name) << std::endl;
  std::vector<emu::pc::DAQMB*> myVector;
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
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<std::endl;
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
    std::cout << "MonitorTMBTrigger:  No tmb" << std::endl ;
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
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"2; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<std::endl;
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
  } else {
    std::cout << "Not tmb" << std::endl ;
  }
  //
  std::cout << "MenuMonitor " << MenuMonitor_ << std::endl;
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
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
  std::ostringstream output;
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
  std::ostringstream output2;
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
  std::ostringstream output3;
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
  std::ostringstream output4;
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
    std::cout << "No counter" << std::endl;
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
    std::ostringstream output;
    output << "<PARAM NAME=\"FlashVars\" VALUE=\"&dataURL=getData" << counter <<"\">"<<std::endl;
    //
    std::cout << output.str() << std::endl;
    *out << output.str() << std::endl ;
    *out << "<PARAM NAME=quality VALUE=high>" << std::endl ;
    *out << "<PARAM NAME=bgcolor VALUE=#FFFFFF>" << std::endl ;
    //*out << "<EMBED src=\"/daq/extern/FusionCharts/Charts/FC_2_3_Bar2D.swf\" FlashVars=\"&dataURL=getData\" quality=high bgcolor=#FFFFFF WIDTH=\"565\" HEIGHT=\"420\" NAME=\"FC_2_3_Bar2D\" TYPE=\"application/x-shockwave-flash\" PLUGINSPAGE=\"http://www.macromedia.com/go/getflashplayer\"></EMBED>" << std::endl;
    //
    std::ostringstream output2;
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
// Check Configuration
////////////////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::CheckConfigurationPage(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  char Name[100];
  sprintf(Name,"CSC Configuration Status");
  //
  MyHeader(in,out,Name);
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  //
  *out << cgicc::legend("Actions").set("style","color:blue") << cgicc::p() << std::endl ;
  //
  *out << cgicc::table().set("border","1");
  //
  *out << cgicc::td();
  std::string CheckTimeSinceHardReset = toolbox::toString("/%s/CheckTimeSinceHardReset",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CheckTimeSinceHardReset) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Check time since last hard reset") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
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
  //
  *out << cgicc::td();
  std::string CheckCratesFirmware = toolbox::toString("/%s/CheckCratesFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CheckCratesFirmware) << std::endl ;
  if (crates_firmware_ok == 1) {
    *out << cgicc::input().set("type","submit").set("value","Check firmware in system").set("style","color:green") << std::endl ;
  } else if (crates_firmware_ok == 0) {
    *out << cgicc::input().set("type","submit").set("value","Check firmware in system").set("style","color:red") << std::endl ;
  } else if (crates_firmware_ok == -1) {
    *out << cgicc::input().set("type","submit").set("value","Check firmware in system").set("style","color:blue") << std::endl ;
  }
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::table();
  //
  *out << cgicc::fieldset();
  //
  int initial_crate = current_crate_;
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("Time since last hard reset").set("style","color:blue") << cgicc::p() << std::endl ;
  //
  bool print_stuff = false;
  for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) 
    for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) 
      if (time_since_reset[crate_number][chamber_index]>0) print_stuff = true;
  //
  if (print_stuff == true) {
    //
    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      //
      SetCurrentCrate(crate_number);
      //
      *out << crateVector[crate_number]->GetLabel();
      //
      for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) 
	*out << " " << std::dec << time_since_reset[crate_number][chamber_index];
      //
      *out << cgicc::br() << std::endl;
    }
  }
  //
  *out << cgicc::fieldset();
  //
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("Firmware Status").set("style","color:blue") << cgicc::p() << std::endl ;
  //
  if (crates_firmware_ok >= 0) {
    //
    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      //
      SetCurrentCrate(crate_number);
      //
      *out << crateVector[crate_number]->GetLabel() << std::endl ;
      //
      if (crate_firmware_ok[current_crate_] == 0) {
	//
	*out << cgicc::br() << cgicc::span().set("style","color:red");
	//
        if (ccb_firmware_ok[current_crate_] == 0) *out << " CCB firmware incorrect " << cgicc::br() << std::endl ;
        if (mpc_firmware_ok[current_crate_] == 0) *out << " MPC firmware incorrect " << cgicc::br() << std::endl ;
        //
	bool alct_ok = true;
	bool tmb_ok = true;
	bool dmb_vme_ok = true;
	bool dmb_control_ok = true;
	bool cfeb_ok = true;
	//
	for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
	  if (alct_firmware_ok[current_crate_][chamber_index] == 0) alct_ok = false;
	  if (tmb_firmware_ok[current_crate_][chamber_index] == 0)  tmb_ok = false;
	  if (dmb_vme_firmware_ok[current_crate_][chamber_index] == 0)  dmb_vme_ok = false;
	  if (dmb_control_firmware_ok[current_crate_][chamber_index] == 0)  dmb_control_ok = false;
	  //
	  std::vector<emu::pc::CFEB> cfebs = dmbVector[chamber_index]->cfebs() ;
	  typedef std::vector<emu::pc::CFEB>::iterator CFEBItr;
	  for(CFEBItr cfebItr = cfebs.begin(); cfebItr != cfebs.end(); ++cfebItr) {
	      int cfeb_index = (*cfebItr).number();
	      if (cfeb_firmware_ok[current_crate_][chamber_index][cfeb_index] == 0)  cfeb_ok = false;
	  }
	}
	//
	if (!alct_ok) {
	  //
	  *out << " ALCT firmware incorrect: " ;
	  for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) 
	    if (alct_firmware_ok[current_crate_][chamber_index] == 0) 
	      *out << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetLabel().c_str() << ", ";
	  //
	  *out << cgicc::br() << std::endl ;
	}
	//
	if (!tmb_ok) {
	  //
	  *out << " TMB firmware incorrect: " ;
	  for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) 
	    if (tmb_firmware_ok[current_crate_][chamber_index] == 0) 
	      *out << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetLabel().c_str() << ", ";
	  //
	  *out << cgicc::br() << std::endl ;
	}
	//
	if (!dmb_vme_ok) {
	  //
	  *out << " DMB VME firmware incorrect: " ;
	  for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) 
	    if (dmb_vme_firmware_ok[current_crate_][chamber_index] == 0) 
	      *out << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetLabel().c_str() << ", ";
	  //
	  *out << cgicc::br() << std::endl ;
	}
	//
	if (!dmb_control_ok) {
	  //
	  *out << " DMB Control FPGA firmware incorrect: " ;
	  for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) 
	    if (dmb_control_firmware_ok[current_crate_][chamber_index] == 0) 
	      *out << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetLabel().c_str() << ", ";
	  //
	  *out << cgicc::br() << std::endl ;
	}
	//
	if (!cfeb_ok) {
	  //
	  *out << " CFEB firmware incorrect: " ;
	  for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
	    //
	    std::vector<emu::pc::CFEB> cfebs = dmbVector[chamber_index]->cfebs() ;
	    typedef std::vector<emu::pc::CFEB>::iterator CFEBItr;
	    for(CFEBItr cfebItr = cfebs.begin(); cfebItr != cfebs.end(); ++cfebItr) {
	      int cfeb_index = (*cfebItr).number();
	      if (cfeb_firmware_ok[current_crate_][chamber_index][cfeb_index] == 0) 
		*out << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetLabel().c_str() << " CFEB " << cfeb_index << ", ";
	    }
	  }
	  //
	  *out << cgicc::br() << std::endl ;
	}
	//
      } else if (crate_firmware_ok[current_crate_] == 1) {
	//
	*out << cgicc::span().set("style","color:green");
	*out << " firmware OK" << cgicc::br();
      } else if (crate_firmware_ok[current_crate_] == -1) {
	//
	*out << cgicc::span().set("style","color:blue");
	*out << " firmware not checked" << cgicc::br();
      }
      *out << cgicc::span() << std::endl ;
    }
  }
  //
  //
  *out << cgicc::fieldset();
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("Configuration Status").set("style","color:blue") << cgicc::p() << std::endl ;
  //
  if (all_crates_ok >= 0) {
    //
    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      //
      SetCurrentCrate(crate_number);
      //
      if (crate_check_ok[current_crate_] == 0) {
	//
	*out << cgicc::span().set("style","color:red");
	//
        if (ccb_check_ok[current_crate_] == 0) *out << thisCrate->GetLabel() << " problem with CCB " << cgicc::br() << std::endl ;
        if (mpc_check_ok[current_crate_] == 0) *out << thisCrate->GetLabel() << " problem with MPC " << cgicc::br() << std::endl ;
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
	// grayson - put my name here so I can search for it
	if (!alct_ok) {
	  //
	  for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) 
	    if (alct_check_ok[current_crate_][chamber_index] == 0) {
          if(thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetProblemDescription().length()) *out << "<span style=\"color:black\" >";
	      *out << thisCrate->GetLabel() << "   "
		   << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetLabel().c_str() 
		   << " problem with ALCT " << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetProblemDescription() 
		   << cgicc::br();
          if(thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetProblemDescription().length()) *out << "</span>";
        }
	}
	//
	if (!tmb_ok) {
	  //
	  for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) 
	    if (tmb_check_ok[current_crate_][chamber_index] == 0) {
          if(thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetProblemDescription().length()) *out << "<span style=\"color:black\" >";
	      *out << thisCrate->GetLabel() << "    "
		   << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetLabel().c_str() 
		   << " problem with TMB " << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetProblemDescription() 
		   << cgicc::br();
          if(thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetProblemDescription().length()) *out << "</span>";
        }
	}
	//
	if (!dmb_ok) {
	  //
	  for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) 
	    if (dmb_check_ok[current_crate_][chamber_index] == 0) {
          if(thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetProblemDescription().length()) *out << "<span style=\"color:black\" >";
	      *out << thisCrate->GetLabel() << "    "
		   << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetLabel().c_str() 
		   << " problem with DMB " << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetProblemDescription() 
		   << cgicc::br();
          if(thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetProblemDescription().length()) *out << "</span>";
	 	}
	}
	//
      } else if (crate_check_ok[current_crate_] == 1) {
	//
	*out << cgicc::span().set("style","color:green");
	*out << crateVector[crate_number]->GetLabel() << "   OK" << cgicc::br();
      } else if (crate_check_ok[current_crate_] == -1) {
	//
	*out << cgicc::span().set("style","color:blue");
	*out << crateVector[crate_number]->GetLabel() << " Not checked" << cgicc::br();
      }
      *out << cgicc::span() << std::endl ;
    }
  }
  //
  SetCurrentCrate(initial_crate);
  //
  *out << cgicc::fieldset();
}
//
void EmuPeripheralCrateConfig::CheckTimeSinceHardReset(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  cgicc::CgiEnvironment cgiEnvi(in);
  //
  std::cout << "Check time since TMBs last received hard resets... " << std::endl; 
  //
  int initialcrate=current_crate_;
  //
  for (int i=0; i<60; i++) 
    for (int j=0; j<9; j++) 
      time_since_reset[i][j] = -1;
  //
  for(unsigned i=0; i< crateVector.size(); i++) {
    //
    if ( crateVector[i]->IsAlive() ) {
      //
      SetCurrentCrate(i);	
      //
      for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	time_since_reset[i][tmb] = tmbVector[tmb]->ReadRegister(0xE8); 
	std::cout << " " << tmbVector[tmb]->ReadRegister(0xE8);
      }
      std::cout << std::endl;
    }
  }

  SetCurrentCrate(initialcrate);	

  if(cgiEnvi.getReferrer().find("CheckConfigurationPage") != std::string::npos)
    this->CheckConfigurationPage(in, out);

 /* 
  if(cgiEnvi.getReferrer().find("CheckCratesConfiguration") != std::string::npos) {
    this->CheckCratesConfiguration(in,out);
  } else { 
    this->CheckConfigurationPage(in, out);
  }
*/

}
//
void EmuPeripheralCrateConfig::CheckCratesConfiguration(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  cgicc::CgiEnvironment cgiEnvi(in);
  //
    if(cgiEnvi.getReferrer().find("CheckTimeSinceHardReset") != std::string::npos) {
      std::cout << "Checking times since last hard reset..." << std::endl;
      this->CheckTimeSinceHardReset(in,out);
    }
  //
  std::cout << "Button:  Check Configuration of All Active Crates" << std::endl;
  //
  OutputCheckConfiguration.str(""); //clear the output string
  //
  if(total_crates_<=0) return;
  //
  OutputCheckConfiguration << "Number of seconds since last hard reset for each TMB" << std::endl;
  int initialcrate=current_crate_;
  //
  for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
    //
    SetCurrentCrate(crate_number);
    //
    OutputCheckConfiguration << crateVector[crate_number]->GetLabel();
    //
    for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) 
      OutputCheckConfiguration << " " << std::dec << time_since_reset[crate_number][chamber_index];
    //
    OutputCheckConfiguration << std::endl;
  }
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
      //
    } else {
      //
      crate_check_ok[i] = -1;
    }
  }
  //
  SetCurrentCrate(initialcrate);
  //
  //Output the errors to a file...
  time_t rawtime;
  time(&rawtime);
  //
  std::string buf;
  std::string time_dump = ctime(&rawtime);
  std::string time = time_dump.substr(0,time_dump.length()-1);
  //
  while( time.find(" ",0) != std::string::npos ) {
    //
    int thispos = time.find(" ",0); 
    time.replace(thispos,1,"_");
    //
  }
  //
  buf = "ConfigurationCheckLogFile"+time+".log";
  //
  std::ofstream LogFileCheckConfiguration;
  LogFileCheckConfiguration.open(buf.c_str());
  LogFileCheckConfiguration << OutputCheckConfiguration.str() ;
  LogFileCheckConfiguration.close();
  //
  this->CheckConfigurationPage(in, out);
}
//
void EmuPeripheralCrateConfig::CheckCrateConfiguration(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //  
  std::cout << "Button: Check Crate Configuration" << std::endl;
  //
  //std::cout << "Crate address = 0x" << std::hex << thisCrate->vmeController()->ipAddress() << std::endl;
  //
  CheckPeripheralCrateConfiguration();
  //
  this->Default(in, out);
}
//
void EmuPeripheralCrateConfig::CheckCratesFirmware(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //
  std::cout << "Button:  Check CSC Firmware of All Active Crates" << std::endl;
  //
  if(total_crates_<=0) return;
  //
  int initialcrate=current_crate_;
  //
  crates_firmware_ok = 1;
  //
  for(unsigned i=0; i< crateVector.size(); i++) {
    //
    if ( crateVector[i]->IsAlive() ) {
      //
      SetCurrentCrate(i);	
      //
      CheckPeripheralCrateFirmware();
      //
      crates_firmware_ok &= crate_firmware_ok[i];
    } else {
      //
      crate_firmware_ok[i] = -1;
    }
  }
  //
  SetCurrentCrate(initialcrate);
  //
  this->CheckConfigurationPage(in, out);
}
//
void EmuPeripheralCrateConfig::CheckCrateFirmware(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //  
  std::cout << "Button: Check CSC firmware in one crate" << std::endl;
  //
  std::cout << "Crate address = 0x" << std::hex << thisCrate->vmeController()->ipAddress() << std::endl;
  //
  CheckPeripheralCrateFirmware();
  //
  this->CheckConfigurationPage(in, out);
}
//
// Another method which would be better in another class... let's make it work, first....
void EmuPeripheralCrateConfig::CheckPeripheralCrateConfiguration() {
  //
  std::cout << "Configuration check for " << thisCrate->GetLabel() << std::endl;
  //
  crate_check_ok[current_crate_] = 1;
  //
  //  OutputCheckConfiguration << "Crate " << thisCrate->GetLabel(); // << std::endl;
  //
  thisCrate->ccb()->RedirectOutput(&OutputCheckConfiguration);
  ccb_check_ok[current_crate_] = thisCrate->ccb()->CheckConfig();
  crate_check_ok[current_crate_] &=  ccb_check_ok[current_crate_];  
  thisCrate->ccb()->RedirectOutput(&std::cout);
  //
  thisCrate->mpc()->RedirectOutput(&OutputCheckConfiguration);
  mpc_check_ok[current_crate_] = thisCrate->mpc()->CheckConfig();
  crate_check_ok[current_crate_] &=  mpc_check_ok[current_crate_];  
  thisCrate->mpc()->RedirectOutput(&std::cout);

  OutputCheckConfiguration << "Crate " << thisCrate->GetLabel(); // << std::endl;

  if(!(ccb_check_ok[current_crate_] && mpc_check_ok[current_crate_])) {
    OutputCheckConfiguration << "; Problem";
    if(!(ccb_check_ok[current_crate_])) OutputCheckConfiguration << " CCB";
    if(!(mpc_check_ok[current_crate_])) OutputCheckConfiguration << " MPC";
  }
  OutputCheckConfiguration << std::endl;



  for (unsigned int chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
    emu::pc::Chamber * thisChamber     = chamberVector[chamber_index];
    emu::pc::TMB * thisTMB             = tmbVector[chamber_index];
    emu::pc::ALCTController * thisALCT = thisTMB->alctController();
    emu::pc::DAQMB * thisDMB           = dmbVector[chamber_index];

    std::string chamberName=toolbox::toString((char *)thisChamber->GetLabel().c_str());
 
    std::cout << "Configuration check for " << thisCrate->GetLabel() << ", " << (thisChamber->GetLabel()).c_str() << std::endl;
    if(thisCrate->ccb()->GetReadALCTConfigDone(chamber_index)-thisCrate->ccb()->GetExpectedALCTConfigDone())
	  OutputCheckConfiguration << "Read and expected ALCT Config differ for " << thisCrate->GetLabel() << ", chamber " << (thisChamber->GetLabel()).c_str() << std::endl;
    if(thisCrate->ccb()->GetReadTMBConfigDone(chamber_index)-thisCrate->ccb()->GetExpectedTMBConfigDone())
	  OutputCheckConfiguration << "Read and expected TMB Config differ for " << thisCrate->GetLabel() << ", chamber " << (thisChamber->GetLabel()).c_str() << std::endl;
    if((thisCrate->ccb()->GetReadDMBConfigDone(chamber_index)-thisCrate->ccb()->GetExpectedDMBConfigDone()) && (chamberName.find("1/3/")==std::string::npos))
	  OutputCheckConfiguration << "Read and expected DMB Config differ for " << thisCrate->GetLabel() << ", chamber " << (thisChamber->GetLabel()).c_str() << std::endl;

    OutputCheckConfiguration << (thisChamber->GetLabel()).c_str() << " ... ";

    thisTMB->RedirectOutput(&OutputCheckConfiguration);
    thisTMB->CheckTMBConfiguration();
    if (thisTMB->GetNumberOfConfigurationReads() > 1)
      OutputCheckConfiguration << "-> N_read(TMB) = " << thisTMB->GetNumberOfConfigurationReads() << std::endl;
    tmb_check_ok[current_crate_][chamber_index]  = (int) thisTMB->GetTMBConfigurationStatus();

      OutputCheckConfiguration << "-> N_read(TMB) = " << thisTMB->GetNumberOfConfigurationReads() << std::endl;   
    thisTMB->RedirectOutput(&std::cout);

    //
    thisALCT->RedirectOutput(&OutputCheckConfiguration);
    thisALCT->CheckALCTConfiguration();
    if (thisALCT->GetNumberOfConfigurationReads() > 1)
      OutputCheckConfiguration << "-> N_read(ALCT) = " << thisALCT->GetNumberOfConfigurationReads() << std::endl;
    alct_check_ok[current_crate_][chamber_index] = (int) thisALCT->GetALCTConfigurationStatus();
    thisALCT->RedirectOutput(&std::cout);

    //
    thisDMB->RedirectOutput(&OutputCheckConfiguration);
    dmb_check_ok[current_crate_][chamber_index]  = (int) thisDMB->checkDAQMBXMLValues();
    if (thisDMB->GetNumberOfConfigurationReads() > 1)
      OutputCheckConfiguration << "-> N_read(DMB) = " << thisDMB->GetNumberOfConfigurationReads() << std::endl;
 
    OutputCheckConfiguration <<  (thisChamber->GetLabel()).c_str();
    if(!(tmb_check_ok[current_crate_][chamber_index] && alct_check_ok[current_crate_][chamber_index] && dmb_check_ok[current_crate_][chamber_index])) {
      OutputCheckConfiguration << "; Problem";
      if(!(tmb_check_ok[current_crate_][chamber_index])) OutputCheckConfiguration << " TMB";
      if(!(alct_check_ok[current_crate_][chamber_index])) OutputCheckConfiguration << " ALCT";
      if(!(dmb_check_ok[current_crate_][chamber_index])) OutputCheckConfiguration << " DMB";
      if(thisChamber->GetProblemDescription().length()) OutputCheckConfiguration << "; known=" << thisChamber->GetProblemDescription();
    }
    OutputCheckConfiguration << std::endl;
    thisDMB->RedirectOutput(&std::cout);
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


void EmuPeripheralCrateConfig::PowerOnFixCFEB(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  int cur18a[5]={1,1,1,2,2};
  int cur18b[5]={1,4,7,2,5};
  int slot2num[22]={0,0,0,0,0,1,0,2,0,3,0,4,0,0,0,5,0,6,0,7,0,8};

  MyHeader(in,out,"Power On Fix Erased CFEB Proms");

  *out << cgicc::table().set("border","0");
  *out << cgicc::tr();
  *out << cgicc::td();
  *out << " Chamber ";
  *out << cgicc::td();
  *out << cgicc::td();
  *out << " Crate ";
  *out << cgicc::td();
  *out << cgicc::td();
  *out << " Slot " ;
  *out << cgicc::td();
  *out << cgicc::td();
  *out << " CFEB " ;
  *out << cgicc::td();
  *out << cgicc::td();
  *out << " Current" ;
  *out << cgicc::td();
  *out << cgicc::td();
  *out << " " ;
  *out << cgicc::td();
  *out << cgicc::tr()<<std::endl;
  // std::cout << " LSD: crate size " << crateVector.size() << std::endl;
  for(unsigned i=0; i< crateVector.size(); i++){
    // std::cout << "LSD: Crate: " << crateVector[i]->GetLabel() << std::endl;
    int  this_crate_no_=i;
    SetCurrentCrate(this_crate_no_);
    int CSRA3=0x04;
    thisCCB->ReadRegister(CSRA3);
    for (unsigned int k=0; k<dmbVector.size(); k++) {
      emu::pc::DAQMB * thisDMB = dmbVector[k];
      int slot = thisDMB->slot();
      if(slot<22){
        int dmbcfg= thisCCB->GetReadDMBConfigDone(slot2num[slot]);
        emu::pc::Chamber *thisChamber = chamberVector[k];
        // std::cout << " LSD: slot: " << slot << " " << " k: " << k << " slot2num " << slot2num[slot] << "daqcfg " << dmbcfg << " chamber " << thisChamber->GetLabel() << std::endl;
        std::vector<emu::pc::CFEB> thisCFEBs = thisDMB->cfebs();
        for(unsigned int j=0;j<thisCFEBs.size();j++){
          int numcfeb;
          for(unsigned int jj=0;jj<thisCFEBs.size();jj++){
            if(j==(unsigned int)thisCFEBs[jj].number())numcfeb=jj;
          }
          std::cout << " LSD febpromid " << " " << thisChamber->GetLabel() << ":" << numcfeb << " " << std::hex << thisDMB->febpromid(thisCFEBs[numcfeb]) << std::dec << std::endl;
        }
        if(dmbcfg==0){
          for(unsigned int j=0;j<thisCFEBs.size();j++){
            int numcfeb;
            for(unsigned int jj=0;jj<thisCFEBs.size();jj++){
              if(j==(unsigned int)thisCFEBs[jj].number())numcfeb=jj;
            }
            // std::cout << " LSD febpromid " << j << " " << thisChamber->GetLabel() << " " << std::hex << thisDMB->febpromid(thisCFE\Bs[numcfeb]) << std::dec << std::endl;
            float current=thisDMB->lowv_adc(cur18a[j],cur18b[j])/1000.;
            if(current<0.81&&current>0.01){
              *out << cgicc::tr();
              *out << cgicc::td();
              *out << thisChamber->GetLabel();
              *out << cgicc::td();
              *out << cgicc::td();
              *out <<  crateVector[i]->GetLabel();
              *out << cgicc::td();
              *out << cgicc::td();
              *out << slot;
              *out << cgicc::td();
              *out << cgicc::td();
              *out << "CFEB"<<j+1;
              *out << cgicc::td();
              *out << cgicc::td();
              *out << current;
              *out << cgicc::td();
              *out << cgicc::td();
              std::string FixCFEB = toolbox::toString("/%s/FixCFEB",getApplicationDescriptor()->getURN().c_str());
              *out << cgicc::form().set("method","GET").set("action",FixCFEB) << std::endl ;
              *out << cgicc::input().set("type","submit").set("value","Fix CFEB").set("style","color:red") << std::endl ;
              char buf[20];
              sprintf(buf,"%d",i);
              *out << cgicc::input().set("type","hidden").set("value",buf).set("name","ncrt");
              sprintf(buf,"%d",k);
              *out << cgicc::input().set("type","hidden").set("value",buf).set("name","ndmb");
              sprintf(buf,"%d",numcfeb);
              *out << cgicc::input().set("type","hidden").set("value",buf).set("name","ncfeb");
              *out << cgicc::form() << std::endl ;
              *out << cgicc::td();
              *out << cgicc::tr() << std::endl;
            }
          }
        }
      }
    }
  }
  *out << cgicc::table() << std::endl;
}
//
void EmuPeripheralCrateConfig::FixCFEB(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  std::cout << " Entered FixCFEB " << std::endl;
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name1 = cgi.getElement("ncrt");
  int ncrt;
  if(name1 != cgi.getElements().end()) {
    ncrt = cgi["ncrt"]->getIntegerValue();
    std::cout << "ncrt " << ncrt << std::endl;
  } else {
    std::cout << "Not ncrt" << std::endl ;
    ncrt=-1;
  }
  cgicc::form_iterator name2 = cgi.getElement("ndmb");
  int ndmb;
  if(name2 != cgi.getElements().end()) {
    ndmb = cgi["ndmb"]->getIntegerValue();
    std::cout << "ndmb " << ndmb << std::endl;
  } else {
    std::cout << "Not ndmb" << std::endl ;
    ndmb=-1;
  }
  cgicc::form_iterator name3 = cgi.getElement("ncfeb");
  int ncfeb;
  if(name3 != cgi.getElements().end()) {
    ncfeb = cgi["ncfeb"]->getIntegerValue();
    std::cout << "ncfeb " << ncfeb << std::endl;
  } else {
    std::cout << "Not ncfeb" << std::endl ;
    ncfeb=-1;
  }
  if(ncrt>-1&&ndmb>-1&&ncfeb>-1){
    SetCurrentCrate(ncrt);
    emu::pc::DAQMB * thisDMB = dmbVector[ndmb];
    std::vector<emu::pc::CFEB> thisCFEBs = thisDMB->cfebs();
    emu::pc::CFEB thisCFEB = thisCFEBs[ncfeb];

    // now readback bit contents of prom
    char * outp;             ;   // recast dword
    thisDMB->epromload_verify(thisCFEB.promDevice(),CFEBVerify_.toString().c_str(),1,outp);  // load mprom
    std::cout << " time calculation " << std::endl;
    time_t rawtime;
    time(&rawtime);
    std::string buf;
    std::string time_dump = ctime(&rawtime);
    std::string time = time_dump.substr(0,time_dump.length()-1);
    while( time.find(" ",0) != std::string::npos ) {
      int thispos = time.find(" ",0);
      time.replace(thispos,1,"_");
    }
    std::cout << "time " << time << std::endl;
    std::string temp = toolbox::toString("mv eprom.bit /tmp/verify_%s_slot%d_cfeb%d_%s.bit",crateVector[ncrt]->GetLabel().c_str(),thisDMB->slot(),thisCFEB.number()+1,time.c_str());
    std::cout  << temp << std::endl;
    system(temp.c_str());

    // now reprogram the prom
    unsigned short int dword[2];
    dword[0]=thisDMB->febpromuser(thisCFEB);
    char * outp2=(char *)dword;   // recast dword
    thisDMB->epromload(thisCFEB.promDevice(),CFEBFirmware_.toString().c_str(),1,outp2);
    // now do a hard reset
    thisCCB->hardReset();
  }
  this->PowerOnFixCFEB(in,out);
}

//
// Another method which would be better in another class... let's make it work, first....
void EmuPeripheralCrateConfig::CheckPeripheralCrateFirmware() {
  //
  std::cout << "Firmware check for " << thisCrate->GetLabel() << std::endl;
  //
  crate_firmware_ok[current_crate_] = 1;
  //
  ccb_firmware_ok[current_crate_] = thisCrate->ccb()->CheckFirmwareDate();
  crate_firmware_ok[current_crate_] &= ccb_firmware_ok[current_crate_];  
  //
  mpc_firmware_ok[current_crate_] = thisCrate->mpc()->CheckFirmwareDate();
  crate_firmware_ok[current_crate_] &= mpc_firmware_ok[current_crate_];  
  //
  for (unsigned int chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
    //	
    emu::pc::Chamber * thisChamber     = chamberVector[chamber_index];
    emu::pc::TMB * thisTMB             = tmbVector[chamber_index];
    emu::pc::ALCTController * thisALCT = thisTMB->alctController();
    emu::pc::DAQMB * thisDMB           = dmbVector[chamber_index];
    //
    std::cout << "Firmware check for " << thisCrate->GetLabel() << ", " << (thisChamber->GetLabel()).c_str() << std::endl;
    //
    tmb_firmware_ok[current_crate_][chamber_index]      = (int) thisTMB->CheckFirmwareDate();
    crate_firmware_ok[current_crate_] &= tmb_firmware_ok[current_crate_][chamber_index];
    //
    alct_firmware_ok[current_crate_][chamber_index]     = (int) thisALCT->CheckFirmwareDate();
    crate_firmware_ok[current_crate_] &= alct_firmware_ok[current_crate_][chamber_index];
    //
    dmb_vme_firmware_ok[current_crate_][chamber_index]  = (int) thisDMB->CheckVMEFirmwareVersion();
    crate_firmware_ok[current_crate_] &= dmb_vme_firmware_ok[current_crate_][chamber_index];
    //
    dmb_control_firmware_ok[current_crate_][chamber_index] = (int) thisDMB->CheckControlFirmwareVersion();
    crate_firmware_ok[current_crate_] &= dmb_control_firmware_ok[current_crate_][chamber_index];
    //
    std::vector<emu::pc::CFEB> cfebs = thisDMB->cfebs() ;
    typedef std::vector<emu::pc::CFEB>::iterator CFEBItr;
    //
    for(CFEBItr cfebItr = cfebs.begin(); cfebItr != cfebs.end(); ++cfebItr) {
      //
      int cfeb_index = (*cfebItr).number();
      //
      cfeb_firmware_ok[current_crate_][chamber_index][cfeb_index] = (int) thisDMB->CheckCFEBFirmwareVersion(*cfebItr);
      crate_firmware_ok[current_crate_] &= cfeb_firmware_ok[current_crate_][chamber_index][cfeb_index];
    }
    //
  }
  //
  return;
}
//
//
/////////////////////////////////////////////////////////////////////
// Chamber Utilities (synchronization) methods
/////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::InitChamber(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Init Chamber" << std::endl ;
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
    std::cout << "InitChamber:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "InitChamber:No dmb" << std::endl;
    dmb = DMB_;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "InitChamber:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "InitChamber:  No tmb" << std::endl;
    tmb = TMB_;
  }
  //
  MyTest[tmb][current_crate_].InitSystem();          // Init chamber
  //
  // Comment out dangerous next line....
  //  thisCCB->setCCBMode(emu::pc::CCB::VMEFPGA);      // It needs to be in FPGA mode to work.
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
    std::cout << "DMB tests: DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "DMB tests: No dmb" << std::endl ;
    dmb = DMB_;
  }
  //
  emu::pc::DAQMB * thisDMB = dmbVector[dmb];
  //
  emu::pc::Chamber * thisChamber = chamberVector[dmb];
  //
  char Name[100];
  sprintf(Name,"%s DMB tests, crate=%s, slot=%d",(thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),thisDMB->slot());
  //
  MyHeader(in,out,Name);
  //
  char buf[200] ;
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl ;
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
  *out << OutputDMBTests[dmb][current_crate_].str() << std::endl ;
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
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  emu::pc::DAQMB * thisDMB = dmbVector[dmb];
  //
  OutputDMBTests[dmb][current_crate_] << "DMB TestAll" << std::endl ;
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
    std::cout << "Test3 DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  OutputDMBTests[dmb][current_crate_] << "DMB Test3" << std::endl ;
  //
  emu::pc::DAQMB * thisDMB = dmbVector[dmb];
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
    std::cout << "Test4 DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  emu::pc::DAQMB * thisDMB = dmbVector[dmb];
  //
  OutputDMBTests[dmb][current_crate_] << "DMB Test4" << std::endl ;
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
    std::cout << "Test5 DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  emu::pc::DAQMB * thisDMB = dmbVector[dmb];
  //
  OutputDMBTests[dmb][current_crate_] << "DMB Test5" << std::endl ;
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
    std::cout << "Test6 DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  OutputDMBTests[dmb][current_crate_] << "DMB Test6" << std::endl ;
  //
  emu::pc::DAQMB * thisDMB = dmbVector[dmb];
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
    std::cout << "Test8 DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  OutputDMBTests[dmb][current_crate_] << "DMB Test8" << std::endl ;
  //
  emu::pc::DAQMB * thisDMB = dmbVector[dmb];
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
    std::cout << "Test9 DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  OutputDMBTests[dmb][current_crate_] << "DMB Test9" << std::endl ;
  //
  emu::pc::DAQMB * thisDMB = dmbVector[dmb];
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
    std::cout << "Test10 DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  OutputDMBTests[dmb][current_crate_] << "DMB Test10" << std::endl ;
  //
  emu::pc::DAQMB * thisDMB = dmbVector[dmb];
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
    std::cout << "Test11 DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  OutputDMBTests[dmb][current_crate_] << "DMB Test11" << std::endl ;
  //
  emu::pc::DAQMB * thisDMB = dmbVector[dmb];
  //
  thisDMB->RedirectOutput(&std::cout);
  thisDMB->test11();
  thisDMB->RedirectOutput(&std::cout);
  //
  this->DMBTests(in,out);
}
//
//
////////////////////////////////////////////////////////////////////////////////////
// Main page methods
////////////////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::InitSystem(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Init System" << std::endl ;
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
    std::cout << "Navigator " << navigator << std::endl;
    if ( navigator == 1 ) {
      thisCCB->setCCBMode(emu::pc::CCB::VMEFPGA);      // It needs to be in FPGA mode to work.
      this->ChamberTests(in,out);
    }
  } else {
    std::cout << "No navigator" << std::endl;
    this->Default(in,out);
  }
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
    std::cout << "ChamberTests:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "ChamberTests:  No dmb" << std::endl;
    dmb = DMB_;
  }
  //
  name = cgi.getElement("tmb");
  //
  if( name != cgi.getElements().end() ) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "ChamberTests:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "ChamberTests:  No tmb" << std::endl;
    tmb = TMB_;
  }
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
  emu::pc::DAQMB * thisDMB = dmbVector[dmb];
  alct = thisTMB->alctController();
  //
  emu::pc::Chamber * thisChamber = chamberVector[tmb];
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
  *out << cgicc::legend("Team A tests").set("style","color:blue") << cgicc::p() << std::endl ;
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
  //  *out << cgicc::pre();
  //  *out << "-------------------------------------------------------------------------" << std::endl;
  //  *out << " Synchronize the Chamber Trigger and DAQ paths" << std::endl;
  //  *out << "-------------------------------------------------------------------------" << std::endl;
  //  *out << "0) Prepare to synchronize system" << std::endl;
  //  *out << "   a) LV:  on" << std::endl;
  //  *out << "   b) Restart Peripheral Crate XDAQ" << std::endl;
  //  *out << "   c) Init System" << std::endl;
  //  *out << cgicc::pre();
  //
  //
  //  *out << cgicc::pre();
  //  *out << "1) Measure relative clock phases with pulsing" << std::endl;
  //  *out << "   a) LTC:  Stop L1A triggers" << std::endl;
  //  *out << "   b) Push the following buttons:" << std::endl;
  //  *out << cgicc::pre();
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
  *out << cgicc::fieldset();
  //
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  //
  *out << cgicc::legend("Expert tools").set("style","color:blue") << cgicc::p() << std::endl ;
  //
  //  *out << cgicc::pre();
  //  *out << "   c) Enter above values into xml" << std::endl;
  //  *out << "   d) Set up the xml file with the desired ALCT and CLCT trigger configuration" << std::endl;
  //  *out << "   e) Restart Peripheral Crate XDAQ" << std::endl;
  //  *out << "   f) Init System" << std::endl;
  //  *out << cgicc::pre();
  //
  //
  //  *out << cgicc::pre();
  //  *out << "2) Set up to perform synchronization" << std::endl;
  //  *out << "   a) LTC Hard Reset" << std::endl;
  //  *out << "   b) For muons from LHC beam or cosmic rays:" << std::endl;
  //  *out << "      i ) HV:  on" << std::endl;
  //  *out << "      ii) skip to step d)" << std::endl;
  //  *out << "   c) For pulsing from the TTC:" << std::endl;
  //  *out << "      i ) push the following button" << std::endl;
  //  *out << cgicc::pre();
  //  //
  *out << cgicc::pre();
  *out << "Snapshot of the DMB timing parameters..." << std::endl;
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
  std::string PrintDmbValuesAndScopes = toolbox::toString("/%s/PrintDmbValuesAndScopes",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",PrintDmbValuesAndScopes) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Read DMB Values/Scopes") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  //  *out << cgicc::pre();
  //  *out << "3) Perform all synchronization steps by pushing the following button" << std::endl;
  //  *out << "   -> This button performs the synchronization steps detailed below " << std::endl;
  //  *out << "      in order, propagating the measured values from one step to the" << std::endl;
  //  *out << "      next step" << std::endl;
  //  *out << cgicc::pre();
  //  //
  //  std::string Automatic = toolbox::toString("/%s/Automatic",getApplicationDescriptor()->getURN().c_str());
  //  *out << cgicc::form().set("method","GET").set("action",Automatic) << std::endl ;
  //  *out << cgicc::input().set("type","submit").set("value","Automatic") << std::endl ;
  //  sprintf(buf,"%d",tmb);
  //  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  //  sprintf(buf,"%d",dmb);
  //  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  //  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br();
  //
  std::string MeasureL1AsAndDAVsForChamber = toolbox::toString("/%s/MeasureL1AsAndDAVsForChamber",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",MeasureL1AsAndDAVsForChamber) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Measure L1As and DAVs") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
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
  *out << "A) Measure CLCT-ALCT match timing" << std::endl;
  //  *out << "   -> Measured values are based on current values of:" << std::endl;
  //  *out << "        * match_trig_window_size" << std::endl;
  //  *out << "        * match_trig_alct_delay" << std::endl;
  //  *out << "        * mpc_tx_delay" << std::endl;
  //  *out << "(assuming that the trigger primitives have already been synchronized at the MPC)" << std::endl;
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
  *out << "C) Find the L1A at the TMB and ALCT" << std::endl;
  *out << cgicc::pre();
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
  *out << "D) Align the Data AVailable (DAV) bits for the ALCT" << std::endl;
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
  //  *out << cgicc::pre();
  //  *out << "-> The following parameter, cfeb_dav_cable_delay, depends on cfeb_cable_delay," << std::endl;
  //  *out << "   who should be set strictly according to its cable length" << std::endl;
  //  *out << cgicc::pre();
  //
  *out << cgicc::pre();
  *out << "E) Find the L1A at the CFEB" << std::endl;
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
  *out << cgicc::br();
  //
  *out << cgicc::pre();
  *out << "F) Align the Data AVailable (DAV) bits for the CFEB" << std::endl;
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
  //  *out << cgicc::pre();
  //  *out << "E) Check the overall state of the DMB readout" << std::endl;
  //  *out << "   -> Is the Active FEB Flag to L1A where it should be for this xLatency?" << std::endl;
  //  *out << "   -> Is the ALCT DAV scope centered at 2?" << std::endl;
  //  *out << "   -> Is the CFEB DAV scope centered at 2?" << std::endl;
  //  *out << cgicc::pre();
  //
  //
  // The following buttons are not being used at the moment...
  //
  //  *out << cgicc::pre();
  //  *out << "F) Measure the communication phase of the RPC link board to the RAT" << std::endl;
  //  *out << "   -> For the future, when RPC Link Boards are connected to the RAT" << std::endl;
  //  *out << "   -> Make sure the RPC parity-bit is enabled for the following scan" << std::endl;
  //  *out << cgicc::pre();
  //
  //  std::string setupCoincidencePulsing = toolbox::toString("/%s/setupCoincidencePulsing",getApplicationDescriptor()->getURN().c_str());
  //  *out << cgicc::form().set("method","GET").set("action",setupCoincidencePulsing) << std::endl ;
  //  *out << cgicc::input().set("type","submit").set("value","Setup Coincidence Pulsing") << std::endl ;
  //  sprintf(buf,"%d",tmb);
  //  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  //  *out << cgicc::form() << std::endl ;
  //  *out << cgicc::br();
  //  *out << cgicc::br();
  //
  //
  *out << cgicc::pre();
  *out << "------------------------------------------------------------------" << std::endl;
  *out << "         Other tools available pertinent to the chamber           " << std::endl;
  *out << "------------------------------------------------------------------" << std::endl;
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
  //  //
  //  //
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
  *out << ChamberTestsOutput[tmb][current_crate_].str() << std::endl ;
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
  std::cout << "Setup coincidence pulsing" << std::endl;
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
    std::cout << "Setup Coincidence Pulsing:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "Setup Coincidence Pulsing:  No tmb" << std::endl;
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
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "No dmb" << std::endl;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "No tmb" << std::endl;
  }
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
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
  std::cout << "Enable L1a request" << std::endl ;
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb, dmb;
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "No dmb" << std::endl;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "No tmb" << std::endl;
  }
  //
  emu::pc::DAQMB * thisDMB = dmbVector[dmb];
  emu::pc::TMB * thisTMB = tmbVector[tmb];
  //
  std::cout << "DMB slot " << thisDMB->slot() << " TMB slot " << thisTMB->slot() << std::endl;
  //
  thisTMB->EnableL1aRequest();
  thisCCB->setCCBMode(emu::pc::CCB::VMEFPGA);
  //
  this->ChamberTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::ALCTTiming(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "ALCTTiming" << std::endl;
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
    std::cout << "ALCTTiming:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "ALCTTiming:  No dmb" << std::endl;
    dmb = DMB_;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "ALCTTiming:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "ALCTTiming: No tmb" << std::endl;
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
  std::cout << "CFEBTiming" << std::endl;
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
    std::cout << "CFEBTiming:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "CFEBTiming:  No dmb" << std::endl;
    dmb = DMB_;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "CFEBTiming:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "CFEBTiming:  No tmb" << std::endl;
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
  std::cout << "Perform all synchronization steps in order" << std::endl;
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
void EmuPeripheralCrateConfig::MeasureL1AsAndDAVsForChamber(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Find L1A and DAV delays for chamber" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Find L1A and DAV delays for chamber");
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
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].SetupRadioactiveTriggerConditions();
  MyTest[tmb][current_crate_].FindL1AAndDAVDelays();
  MyTest[tmb][current_crate_].ReturnToInitialTriggerConditions();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::MeasureL1AsAndDAVsForCrate(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Find L1A and DAV delays for crate" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Find L1A and DAV delays for crate");
  //
  cgicc::Cgicc cgi(in);
  //
  for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) {
    //
    
    std::cout << "crate = " << current_crate_ << ", TMB " << i << std::endl;
    //
    MyTest[i][current_crate_].RedirectOutput(&ChamberTestsOutput[i][current_crate_]);
    MyTest[i][current_crate_].SetupRadioactiveTriggerConditions();
    MyTest[i][current_crate_].FindL1AAndDAVDelays();
    MyTest[i][current_crate_].ReturnToInitialTriggerConditions();
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
  std::cout << "Find L1A delays for crate" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Find L1A delays for crate");
  //
  cgicc::Cgicc cgi(in);
  //
  for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) {
    //
    
    std::cout << "crate = " << current_crate_ << ", TMB " << i << std::endl;
    //
    MyTest[i][current_crate_].RedirectOutput(&ChamberTestsOutput[i][current_crate_]);
    MyTest[i][current_crate_].SetupRadioactiveTriggerConditions();
    MyTest[i][current_crate_].FindL1ADelays();
    MyTest[i][current_crate_].ReturnToInitialTriggerConditions();
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
  std::cout << "Find DAV cable delays for Crate" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Find DAV cable delays");
  //
  cgicc::Cgicc cgi(in);
  //
  for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) {
    //
    
    std::cout << "crate = " << current_crate_ << ", TMB " << i << std::endl;
    //
    MyTest[i][current_crate_].RedirectOutput(&ChamberTestsOutput[i][current_crate_]);
    MyTest[i][current_crate_].SetupRadioactiveTriggerConditions();
    MyTest[i][current_crate_].FindDAVDelays();
    MyTest[i][current_crate_].ReturnToInitialTriggerConditions();
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
  std::cout << "TMBL1aTiming" << std::endl;
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
    std::cout << "TMBL1aTiming:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "TMBL1aTiming:  No dmb" << std::endl;
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
  std::cout << "ALCTL1aTiming" << std::endl;
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
    std::cout << "ALCTL1aTiming:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "No dmb" << std::endl;
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
    std::cout << "No tmb" << std::endl;
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
  std::cout << "EmuPeripheralCrate:  ALCTvpf" << std::endl;
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
    std::cout << "ALCTvpf:  TMB " << tmb << " will read TMB Raw Hits "
	 << MyTest[tmb][current_crate_].getNumberOfDataReads() << " times" << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "ALCTvpf:  No tmb" << std::endl;
  }
  //
  int dmb;
  name = cgi.getElement("dmb");
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "ALCTvpf:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "ALCTvpf:  No dmb" << std::endl;
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
  std::cout << "ALCTScan" << std::endl;
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
    std::cout << "ALCTScan:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "ALCTScan:  No dmb" << std::endl;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "ALCTScan:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "ALCTScan:  No tmb" << std::endl;
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
  std::cout << "CFEBScan" << std::endl;
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
    std::cout << "CFEBScan:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "CFEBScan:  No dmb" << std::endl;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "CFEBScan:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "CFEBScan:  No tmb" << std::endl;
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
  std::cout << "Find CLCT Distrip Hot Channels" << std::endl;
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb, dmb;
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  if (name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "FindDistripHotChannel:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "FindDistripHotChannel:  No tmb" << std::endl;
  }
  //
  name = cgi.getElement("dmb");
  //
  if (name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "FindDistripHotChannel:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "FindDistripHotChannel:  No dmb" << std::endl;
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
  std::cout << "EmuPeripheralCrate:  FindWinner" << std::endl;
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
    std::cout << "FindWinner:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "FindWinner:  No dmb" << std::endl;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "FindWinner:  TMB " << tmb 
	 << ", pausing " << MyTest[tmb][current_crate_].getPauseAtEachSetting() 
	 << " seconds at each delay value" << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "FindWinner:  No tmb" << std::endl;
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
  std::cout << "Measure ALCT DAV cable delay" << std::endl;
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
    std::cout << "AlctDavCableDelay:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "AlctDavCableDelay:  No tmb" << std::endl;
  }
  //
  name = cgi.getElement("dmb");
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "AlctDavCableDelay:  DMB " << dmb 
	 << " Read " << MyTest[tmb][current_crate_].getNumberOfDataReads()
	 << " times, pausing " << MyTest[tmb][current_crate_].getPauseBetweenDataReads() 
	 << "usec between each read" << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "AlctDavCableDelay:  No dmb" << std::endl;
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
  std::cout << "Measure CFEB DAV cable delay" << std::endl;
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
    std::cout << "CfebDavCableDelay:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "CfebDavCableDelay:  No tmb" << std::endl;
  }
  //
  name = cgi.getElement("dmb");
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "CfebDavCableDelay:  DMB " << dmb 
	 << " Read " << MyTest[tmb][current_crate_].getNumberOfDataReads()
	 << " times, pausing " << MyTest[tmb][current_crate_].getPauseBetweenDataReads() 
	 << "usec between each read" << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "CfebDavCableDelay:  No dmb" << std::endl;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "CfebDavCableDelay:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "CfebDavCableDelay:  No tmb" << std::endl;
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
  std::cout << "Measure TMB LCT cable delay" << std::endl;
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
    std::cout << "TmbLctCableDelay:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "TmbLctCableDelay:  No tmb" << std::endl;
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
    std::cout << "TmbLctCableDelay:  No dmb" << std::endl;
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
  std::cout << "Print All DMB Values and Scopes" << std::endl;
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
    std::cout << "PrintDmbValuesAndScopes:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "PrintDmbValuesAndScopes:  No dmb" << std::endl;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "PrintDmbValuesAndScopes:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "PrintDmbValuesAndScopes:  No tmb" << std::endl;
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
  std::cout << "RatTmbTiming" << std::endl;
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "RatTmbTiming:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "RatTmbTiming:  No tmb" << std::endl;
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
  std::cout << "RpcRatTiming" << std::endl;
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "RpcRatTiming:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "RpcRatTiming:  No tmb" << std::endl;
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
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "Not dmb" << std::endl ;
    dmb = DMB_;
  }
  //
  emu::pc::DAQMB * thisDMB = dmbVector[dmb];
  emu::pc::Chamber * thisChamber = chamberVector[dmb];
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
  std::vector<emu::pc::CFEB> cfebs = thisDMB->cfebs() ;
  //
  typedef std::vector<emu::pc::CFEB>::iterator CFEBItr;
  //
  for(CFEBItr cfebItr = cfebs.begin(); cfebItr != cfebs.end(); ++cfebItr) {
    //
    int cfeb_index = (*cfebItr).number();
    //
    sprintf(buf,"CFEB %d : ",cfeb_index);
    *out << buf;
    //
    //*out << cgicc::br();
    //
    sprintf(buf,"CFEB prom user id : %08x CFEB fpga user id : %08x ",
	    (int)thisDMB->febpromuser(*cfebItr),
	    (int)thisDMB->febfpgauser(*cfebItr));
    //
    if ( thisDMB->CheckCFEBFirmwareVersion(*cfebItr) ) {
      *out << cgicc::span().set("style","color:green");
      *out << buf;
      *out << cgicc::span();
    } else {
      *out << cgicc::span().set("style","color:red");
      *out << buf;
      *out << " (Should be 0x" << std::hex << thisDMB->GetExpectedCFEBFirmwareTag(cfeb_index) << ") ";
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
    std::cout << "DMBUtils:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "DMBUtils:  No dmb" << std::endl ;
    dmb = DMB_;
  }
  //
  emu::pc::DAQMB * thisDMB = dmbVector[dmb];
  //
  emu::pc::Chamber * thisChamber = chamberVector[dmb];
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
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl ;
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
  *out << cgicc::br();
  //
  std::string DMBCheckConfiguration = toolbox::toString("/%s/DMBCheckConfiguration",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBCheckConfiguration) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Check DMB+CFEB Configuration") << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::fieldset();
  //  
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl ;
  //
  *out << cgicc::legend("DMB/CFEB Load PROM").set("style","color:red") ;
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

  std::string CFEBReadFirmware = toolbox::toString("/%s/CFEBReadFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CFEBReadFirmware) << std::endl ;
  *out << "CFEB to verify (0-4), (-1 == all) : ";
  *out << cgicc::input().set("type","text").set("value","-1").set("name","DMBNumber") << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","CFEB Read Firmware") << std::endl ;
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
  // Output area
  //
  *out << cgicc::form().set("method","GET") << std::endl ;
  *out << cgicc::pre();
  *out << cgicc::textarea().set("name","CrateTestDMBOutput").set("rows","50").set("cols","150").set("WRAP","OFF");
  *out << OutputStringDMBStatus[dmb].str() << std::endl ;
  *out << cgicc::textarea();
  OutputStringDMBStatus[dmb].str("");
  *out << cgicc::pre();
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
    std::cout << "DMBTurnOff:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  emu::pc::DAQMB * thisDMB = dmbVector[dmb];
  //
  std::cout << "DMBTurnOff" << std::endl;
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
    std::cout << "DMBLoadFirmware:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  thisCCB->hardReset();
  //
  emu::pc::DAQMB * thisDMB = dmbVector[dmb];
  //
  if (thisDMB) {
    //
    std::cout << "DMBLoadFirmware in slot " << thisDMB->slot() << std::endl;
    if (thisDMB->slot()==25) std::cout <<" Broadcast Loading the control FPGA insode one crate"<<std::endl;
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
    std::cout << "DMBVmeLoadFirmware:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  emu::pc::DAQMB * thisDMB = dmbVector[dmb];
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
      std::cout << "DMBVmeLoadFirmware in slot " << thisDMB->slot() << std::endl;
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
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  emu::pc::DAQMB * thisDMB = dmbVector[dmb];
  //
  if (thisDMB->slot() == 25) { 
    std::cout <<" The emergency load is NOT available for DMB slot25"<<std::endl;
    std::cout <<" Please use individual slot loading !!!"<<std::endl;
    return;
  }
  //
  thisCCB->hardReset();
  if (thisDMB) {
    //
    std::cout << "DMB Vme Load Firmware Emergency in slot " << thisDMB->slot() << std::endl;
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

    std::cout<<" The DMB number is set to: "<<dword[0]<<" Entered: "<<dmbNumber<<" Database lookup: "<<dmbID<<std::endl;
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
void EmuPeripheralCrateConfig::CFEBReadFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  LOG4CPLUS_INFO(getApplicationLogger(),"Started CFEB firmware Verify");
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
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  emu::pc::DAQMB * thisDMB = dmbVector[dmb];
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
    std::cout << "CFEBReadFirmware - DMB " << dmb << std::endl;
    //
    thisCCB->hardReset();
    //
    if (thisDMB) {
      //
      std::vector<emu::pc::CFEB> thisCFEBs = thisDMB->cfebs();
      //
      ::sleep(1);
      //
      if (dmbNumber == -1 ) {
	for (unsigned int i=0; i<thisCFEBs.size(); i++) {
	  std::ostringstream dum;
	  dum << "Verifying CFEB firmware for DMB=" << dmb << " CFEB="<< i << std::endl;
	  LOG4CPLUS_INFO(getApplicationLogger(), dum.str());
	  unsigned short int dword[2];
	  dword[0]=thisDMB->febpromuser(thisCFEBs[i]);
	  CFEBid_[dmb][i] = dword[0];  // fill summary file with user ID value read from this CFEB
	  char * outp=(char *)dword;   // recast dword
	  thisDMB->epromload_verify(thisCFEBs[i].promDevice(),CFEBVerify_.toString().c_str(),1,outp);  // load mprom
	}
      } else {
	std::cout << "Verifying CFEB firmware for DMB=" << dmb << " CFEB="<< dmbNumber << std::endl;
	unsigned short int dword[2];
	for (unsigned int i=0; i<thisCFEBs.size(); i++) {
	  if (thisCFEBs[i].number() == dmbNumber ) {
	    dword[0]=thisDMB->febpromuser(thisCFEBs[i]);
	    CFEBid_[dmb][i] = dword[0];  // fill summary file with user ID value read from this CFEB
	    char * outp=(char *)dword;   // recast dword
	    thisDMB->epromload_verify(thisCFEBs[i].promDevice(),CFEBVerify_.toString().c_str(),1,outp);  // load mprom
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
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  emu::pc::DAQMB * thisDMB = dmbVector[dmb];
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
    std::cout << "CFEBLoadFirmware - DMB " << dmb << std::endl;
    //
    thisCCB->hardReset();
    //
    if (thisDMB) {
      //
      std::vector<emu::pc::CFEB> thisCFEBs = thisDMB->cfebs();
      //
      ::sleep(1);
      //
      if (dmbNumber == -1 ) {
	for (unsigned int i=0; i<thisCFEBs.size(); i++) {
	  std::ostringstream dum;
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
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  emu::pc::DAQMB * thisDMB = dmbVector[dmb];
  if ((thisDMB->slot() >21) || (thisDMB->slot() <3)){
    std::cout<<" Invalid DMB slot for CFEB Number reloading "<<thisDMB->slot()<<std::endl;
    return;
  }
  //
  std::cout << "CFEBLoadFirmware - DMB " << dmb << std::endl;
  //
  //    thisCCB->hardReset();
  //
  if (thisDMB) {
    //
    std::vector<emu::pc::CFEB> thisCFEBs = thisDMB->cfebs();
    //
    ::sleep(1);
    //
    std::ostringstream dum;
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
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  emu::pc::DAQMB * thisDMB = dmbVector[dmb];
  //
  std::cout << "DMBTurnOn" << std::endl;
  //
  if (thisDMB) {
    thisDMB->lowv_onoff(0x3f);
  }
  //
  this->DMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::DMBCheckConfiguration(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "DMBCheckConfiguration  DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  emu::pc::DAQMB * thisDMB = dmbVector[dmb];
  //
  thisDMB->RedirectOutput(&OutputStringDMBStatus[dmb]);
  thisDMB->checkDAQMBXMLValues();
  thisDMB->RedirectOutput(&std::cout);
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
  std::cout << "TMBPrintCounters "<< std::endl;
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
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
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
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
  std::cout << "Read TMB " << tmb << " counters over " << wait_time << " seconds" << std::endl;
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
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
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
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
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
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
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
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
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
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
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
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
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
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
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
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
    std::cout << "Configure:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
  emu::pc::ALCTController * alct = thisTMB->alctController();
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
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
  emu::pc::ALCTController * alct = thisTMB->alctController();
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
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
  emu::pc::ALCTController * alct = thisTMB->alctController();
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
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
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
    std::cout << "TMBRawHits:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
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
    std::cout << "ALCTRawHits:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
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
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
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
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "Not tmb" << std::endl ;
    tmb = TMB_;
  }
  //
  emu::pc::ALCTController * alct = tmbVector[tmb]->alctController();
  //
  emu::pc::Chamber * thisChamber = chamberVector[tmb];
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
  alct->RedirectOutput(out);
  //
  if ( alct->CheckFirmwareDate() ) {
    *out << cgicc::span().set("style","color:green");  
    alct->PrintFastControlId();
    *out << "...OK...";
  } else {
    *out << cgicc::span().set("style","color:red");    
    alct->PrintFastControlId();
    *out << " --->>  BAD  <<--- Should be (" 
	 << std::dec << alct->GetExpectedFastControlDay() 
	 << " "      << alct->GetExpectedFastControlMonth()
	 << " "      << alct->GetExpectedFastControlYear()
	 << ")";
  }
  //
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
  *out << "power line" << std::endl;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << "+3.3V" << std::endl;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","center");  
  *out << "+1.8V" << std::endl;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << "+5.5V B" << std::endl;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << "+5.5V A" << std::endl;
  *out << cgicc::td();
  //
  /////////////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","center");  
  *out << "measured V" << std::endl;
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
  *out << "measured I" << std::endl;
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
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "Not tmb" << std::endl ;
    tmb = TMB_;
  }
  //
  emu::pc::RAT * rat = tmbVector[tmb]->getRAT();
  //
  emu::pc::Chamber * thisChamber = chamberVector[tmb];
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
  if ( thisCCB->CheckFirmwareDate() ) {
    //
    *out << cgicc::span().set("style","color:green");
    thisCCB->printFirmwareVersion();
    *out << "...OK...";
    *out << cgicc::span();
    //
  } else {
    //
    *out << cgicc::span().set("style","color:red");
    thisCCB->printFirmwareVersion();
    *out << " --->>  BAD  <<--- Should be (" 
	 << std::dec << thisCCB->GetExpectedFirmwareDay() 
	 << "-"      << thisCCB->GetExpectedFirmwareMonth()
	 << "-"      << thisCCB->GetExpectedFirmwareYear()
	 << ")";

    *out << cgicc::span();
  }
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
  case emu::pc::CCB::TTCrqFPGA:
    *out << "TTCrgFPGA" << std::endl;
    break;
  case emu::pc::CCB::VMEFPGA:
    *out << "VMEFPGA" << std::endl;
    break;
  case emu::pc::CCB::DLOG:
    *out << "DLOG" << std::endl;
    break;
  default:
    *out << "unknown" << std::endl;
    break;
  }
  //
  *out << cgicc::br() << "CSRA1 =  " << std::hex << thisCCB->ReadRegister(0) << std::endl;
  *out << cgicc::br() << "CSRA2 =  " << std::hex << thisCCB->ReadRegister(2) << std::endl;
  *out << cgicc::br() << "CSRA3 =  " << std::hex << thisCCB->ReadRegister(4) << std::endl;
  *out << cgicc::br() << "CSRB1 =  " << std::hex << thisCCB->ReadRegister(0x20) << std::endl;
  *out << cgicc::br() << "CSRB18 = " << std::hex << thisCCB->ReadRegister(0x42) << std::endl;
  //
  *out << cgicc::fieldset();
  //
}
//

//
// BGB New Controller Utils start here 
//

void EmuPeripheralCrateConfig::VCCHeader(xgi::Input * in, xgi::Output * out, std::string title, std::string heading ) 
  throw (xgi::exception::Exception) {
  //
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  //
  *out << cgicc::title(title) << std::endl;
  //
  std::string myUrn = getApplicationDescriptor()->getURN().c_str();
  //  xgi::Utils::getPageHeader(out,title,myUrn,"link","src here");
  //  xgi::Utils::getPageHeader(out,title,myUrn,"","");

  *out << "<table cellpadding=\"2\" cellspacing=\"2\" border=\"0\" style=\"width: 100%; font-family: arial;\">" << std::endl;
  *out << "<tbody>" << std::endl;
  *out << "<tr>" << std::endl;
  //  *out << "<td valign=middle style=\"height: 20px; width: 20px;\"><img border=\"0\" src=\"/usr/images/physics_web.png\" title=\"OSU_Phys\" alt=\"OSU_Phys\"></td><td valign=middle style=\"text-align: left; \">" << std::endl;
  *out << "<td valign=middle style=\"height: 20px; width: 20px;\"><img border=\"0\" src=\"/xgi/images/XDAQLogo.gif\" title=\"XDAQ\" alt=\"XDAQ_logo\"></td><td valign=middle style=\"text-align: left; \">" << std::endl;
  *out << "<p style=\"color:#003366; font-size=200%; font-style: italic; font-weight: bold;\">" << std::endl;
  *out << heading << std::endl;
  *out << "</td></tr></tbody></table>" << std::endl;
  *out << "<hr style=\"width: 100%; height: 1px;\"><br>" << std::endl;

}
void EmuPeripheralCrateConfig::ControllerUtils_Xfer(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
    VCC_UTIL_curr_crate = thisCrate;
    VCC_UTIL_curr_color = "\"#88CCCC\"";
    for(int i=0; i<(int)crateVector.size(); i++){
      crateVector[i]->vmeController()->SetUseDCS(true);
      crateVector[i]->vmeController()->init();
    }
    this->ControllerUtils(in,out);
}

void EmuPeripheralCrateConfig::ControllerUtils(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {

  emu::pc::Crate *lccc;
  static bool first = true;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();
  if(first){
    first = false;
    if ( lccc->vmeController()->GetDebug() == 0 ) {
      VCC_UTIL_cmn_tsk_dbg = "Disabled";
    } else {
      VCC_UTIL_cmn_tsk_dbg = "Enabled";
    }
    VCC_UTIL_cmn_tsk_sn="-";
    VCC_UTIL_cmn_tsk_cc="Not yet read back";
    VCC_UTIL_cmn_tsk_lpbk = "---";
    VCC_UTIL_cmn_tsk_lpbk_color = "#000000";
    VCC_UTIL_acc_cntrl = "disabled";
    VCC_UTIL_expert_pswd = "give me control now!";
  }

  char title[] = "VCC Utilities: Main Page";
  char pbuf[300];

  sprintf(pbuf,"%s<br>Current Crate is %s<br>MAC Addr: %02x-%02x-%02x-%02x-%02x-%02x",title,(lccc->GetLabel()).c_str(),lccc->vmeController()->GetDestMAC(0),lccc->vmeController()->GetDestMAC(1),lccc->vmeController()->GetDestMAC(2),lccc->vmeController()->GetDestMAC(3),lccc->vmeController()->GetDestMAC(4),lccc->vmeController()->GetDestMAC(5));

  VCCHeader(in,out,title,pbuf);

  this->VMECC_UTIL_Menu_Buttons(in,out);

   std::string VCC_CMNTSK_DO =
     toolbox::toString("/%s/VCC_CMNTSK_DO",getApplicationDescriptor()->getURN().c_str());


  *out << "<form action=\"" << VCC_CMNTSK_DO << "\" method=\"GET\">" << std::endl;
  *out << "<fieldset><legend style=\"font-size: 18pt;\" align=\"center\">Common Tasks</legend>" << std::endl;
  *out << "<div align=\"center\">" << std::endl;
  *out << "  <table border=\"3\"  cellspacing=\"2\" cellpadding=\"2\" bgcolor=" << VCC_UTIL_curr_color << " rules=\"none\" style=\"border-collapse: collapse\">" << std::endl;
  *out << "    <tr>" << std::endl;
  *out << "	 <td align=\"right\"><input type=\"submit\" value=\"Toggle Debug\" name=\"cmntsk_tog_dbg\"></td>" << std::endl;
  *out << "      <td align=\"left\">" << VCC_UTIL_cmn_tsk_dbg << "</td>" << std::endl;
  *out << "	 <td align=\"right\"><input type=\"submit\" value=\"VCC S/N\" name=\"cmntsk_vccsn\"></td>" << std::endl;
  *out << "      <td align=\"left\">" << VCC_UTIL_cmn_tsk_sn << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "	 <td align=\"right\"><input type=\"submit\" value=\"Firmware Version\" name=\"cmntsk_cc\"></td>" << std::endl;
  *out << "      <td colspan=\"3\" align=\"left\">" << VCC_UTIL_cmn_tsk_cc << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "	 <td align=\"right\"><input type=\"submit\" value=\"Load PROM\" name=\"cmntsk_ldfrmw\"></td>" << std::endl;
  *out << "      <td colspan=\"3\" align=\"left\"> With firmware version " << vmecc->VCC_frmw_ver << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "	 <td align=\"right\"><input type=\"submit\" value=\"Reload FPGA\" name=\"cmntsk_reload\"></td>" << std::endl;
  *out << "	 <td align=\"right\"><input type=\"submit\" value=\"Loopback Test\" name=\"cmntsk_lpbk\"></td>" << std::endl;
  *out << "      <td align=\"left\" style=\"color: " << VCC_UTIL_cmn_tsk_lpbk_color << "\">" << VCC_UTIL_cmn_tsk_lpbk << "</td>" << std::endl;
  *out << "      <td></td>" << std::endl;
  *out << "    </tr>" << std::endl;
  *out << "  </table>" << std::endl;
  *out << "</div>" << std::endl;
  *out << "</fieldset>" << std::endl;
  *out << "</form>" << std::endl;

  VCC_UTIL_curr_page = VCC_CMNTSK;
}

void EmuPeripheralCrateConfig::VMECC_UTIL_Menu_Buttons(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::string VMECCGUI_GoTo_General =
    toolbox::toString("/%s/VMECCGUI_GoTo_General",getApplicationDescriptor()->getURN().c_str());
  std::string VMECCGUI_GoTo_Intermediate =
    toolbox::toString("/%s/VMECCGUI_GoTo_Intermediate",getApplicationDescriptor()->getURN().c_str());
   std::string VMECCGUI_GoTo_Expert =
     toolbox::toString("/%s/VMECCGUI_GoTo_Expert",getApplicationDescriptor()->getURN().c_str());
   std::string VCC_CRSEL_DO =
     toolbox::toString("/%s/VCC_CRSEL_DO",getApplicationDescriptor()->getURN().c_str());

  *out << "<div align=\"left\">" << std::endl;
  *out << "  <table width=\"100%\" cellpadding=\"4\">" << std::endl;
  *out << "    <tr>" << std::endl;
  *out << "      <td align=\"left\" valign=\"top\">" << std::endl;
  *out << "        <form action=\"" << VMECCGUI_GoTo_General << "\" method=\"GET\">" << std::endl;
  *out << "          <fieldset><legend style=\"font-size: 18pt;\" align=\"left\">General Users</legend>" << std::endl;
  *out << "            <table cellpadding=\"4\">" << std::endl;
  *out << "              <tr>" << std::endl;
  *out << "                <td align=\"left\">" << std::endl;
  *out << "          	   <input type=\"submit\" value=\"Common Tasks\" name=\"gt_cut\" style=\"background-color: #88CCCC;\">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "              </tr>" << std::endl;
  *out << "              <tr>" << std::endl;
  *out << "                <td align=\"left\">" << std::endl;
  *out << "          	   <input type=\"submit\" value=\"Yellow Page\" name=\"gt_ypg\" style=\"background-color: #FFFF00;\">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "              </tr>" << std::endl;
  *out << "              <tr>" << std::endl;
  *out << "                <td align=\"left\">" << std::endl;
  *out << "          	   <input type=\"submit\" value=\"CrateConfig\" name=\"gt_crc\" style=\"background-color: #00FF00;\">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "              </tr>" << std::endl;
  *out << "            </table>" << std::endl;
  *out << "          </fieldset>" << std::endl;
  *out << "        </form>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "      <td align=\"left\" valign=\"top\">" << std::endl;
  *out << "        <form action=\"" << VMECCGUI_GoTo_Intermediate << "\" method=\"GET\">" << std::endl;
  *out << "          <fieldset><legend style=\"font-size: 18pt;\" align=\"left\">Intermediate Users</legend>" << std::endl;
  *out << "            <table cellpadding=\"4\">" << std::endl;
  *out << "              <tr>" << std::endl;
  *out << "                <td align=\"left\">" << std::endl;
  *out << "          	   <input type=\"submit\" value=\"VME Access\" name=\"gt_vmeacc\" style=\"background-color: #008800;\">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "              </tr>" << std::endl;
  *out << "              <tr>" << std::endl;
  *out << "              </tr>" << std::endl;
  *out << "              <tr>" << std::endl;
  *out << "              </tr>" << std::endl;
  *out << "            </table>" << std::endl;
  *out << "          </fieldset>" << std::endl;
  *out << "        </form>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "      <td align=\"center\" valign=\"top\">" << std::endl;
  *out << "        <form action=\"" << VMECCGUI_GoTo_Expert << "\" method=\"GET\">" << std::endl;
  *out << "          <fieldset><legend style=\"font-size: 18pt;\" align=\"center\">Expert Users</legend>" << std::endl;
  *out << "            <table cellpadding=\"4\">" << std::endl;
  *out << "              <tr>" << std::endl;
  *out << "                <td align=\"center\">" << std::endl;
  *out << "          	   <input type=\"submit\" value=\"Expert Mode\" name=\"gt_pswd\">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "                <td align=\"center\">" << std::endl;
  *out << "	             <input type=\"submit\" value=\"User Mode\" name=\"st_usrmd\">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "                <td align=\"center\">" << std::endl;
  *out << "          	   <input type=\"submit\" value=\"Misc. Commands.\" name=\"gt_misc\" style=\"background-color: #FFCC88;\">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "              </tr>" << std::endl;
  *out << "              <tr>" << std::endl;
  *out << "                <td align=\"center\">" << std::endl;
  *out << "                  <input type=\"submit\" value=\"External FIFO\" name=\"gt_fifo\" style=\"background-color: #CCCCFF;\">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "                <td align=\"center\">" << std::endl;
  *out << "                  <input type=\"submit\" value=\"Config Regs\" name=\"gt_crs\" style=\"background-color: #FFCCFF;\">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "                <td align=\"center\">" << std::endl;
  *out << "                  <input type=\"submit\" value=\"MAC Addr's\" name=\"gt_mac\" style=\"background-color: #FFFFCC;\">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "              </tr>" << std::endl;
  *out << "              <tr>" << std::endl;
  *out << "                <td align=\"center\">" << std::endl;
  *out << "                  <input type=\"submit\" value=\"Send Packets\" name=\"gt_pktsnd\" style=\"background-color: #FFCCCC;\" " << VCC_UTIL_acc_cntrl << ">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "                <td align=\"center\">" << std::endl;
  *out << "                  <input type=\"submit\" value=\"Receive Packets\" name=\"gt_pktrcv\" style=\"background-color: #CCFFCC;\">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "                <td align=\"center\">" << std::endl;
  *out << "                  <input type=\"submit\" value=\"Firmware\" name=\"gt_frmw\" style=\"background-color: #CCFFFF;\">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "              </tr>" << std::endl;
  *out << "            </table>" << std::endl;
  *out << "          </fieldset>" << std::endl;
  *out << "        </form>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "      <td align=\"right\" valign=\"top\">" << std::endl;
  *out << "        <form action=\"" << VCC_CRSEL_DO << "\" method=\"GET\">" << std::endl;
                     this->VMECC_UTIL_Crate_Selection(in,out);
  *out << "        </form>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "    </tr>" << std::endl;
  *out << "  </table>" << std::endl;
  *out << "</div>" << std::endl;

}

void EmuPeripheralCrateConfig::VMECCGUI_GoTo_General(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
    std::cout<<" entered VMECCGUI_GoTo_General"<<std::endl;
    cgicc::Cgicc cgi(in);
    const cgicc::CgiEnvironment& env = cgi.getEnvironment();
    std::string guiStr = env.getQueryString() ;
    std::cout << guiStr << std::endl ;

    cgicc::form_iterator gt_cut_name = cgi.getElement("gt_cut");
    cgicc::form_iterator gt_ypg_name = cgi.getElement("gt_ypg");
    cgicc::form_iterator gt_crc_name = cgi.getElement("gt_crc");

    if(gt_cut_name != cgi.getElements().end()) {
      VCC_UTIL_curr_color = "\"#88CCCC\"";
      this->ControllerUtils(in,out);
    }
    else if(gt_ypg_name != cgi.getElements().end()) {
      this->MainPage(in,out);
    }
    else if(gt_crc_name != cgi.getElements().end()) {
      this->CrateConfiguration(in,out);
    }
    else {
      VCC_UTIL_curr_color = "\"#88CCCC\"";
      this->ControllerUtils(in,out);
    }
}

void EmuPeripheralCrateConfig::VMECCGUI_GoTo_Intermediate(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
    std::cout<<" entered VMECCGUI_GoTo_Intermediate"<<std::endl;
    cgicc::Cgicc cgi(in);
    const cgicc::CgiEnvironment& env = cgi.getEnvironment();
    std::string guiStr = env.getQueryString() ;
    std::cout << guiStr << std::endl ;

    cgicc::form_iterator gt_vmeacc_name = cgi.getElement("gt_vmeacc");

    if(gt_vmeacc_name != cgi.getElements().end()) {
      VCC_UTIL_curr_color = "\"#008800\"";
      this->VMECCGUI_VME_access(in,out);
    }
    else {
      VCC_UTIL_curr_color = "\"#88CCCC\"";
      this->ControllerUtils(in,out);
    }
}

void EmuPeripheralCrateConfig::VMECCGUI_GoTo_Expert(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
    std::cout<<" entered VMECCGUI_GoTo_Expert"<<std::endl;
    cgicc::Cgicc cgi(in);
    const cgicc::CgiEnvironment& env = cgi.getEnvironment();
    std::string guiStr = env.getQueryString() ;
    std::cout << guiStr << std::endl ;

    cgicc::form_iterator gt_pswd_name = cgi.getElement("gt_pswd");
    cgicc::form_iterator st_usrmd_name = cgi.getElement("st_usrmd");
    cgicc::form_iterator gt_frmw_name = cgi.getElement("gt_frmw");
    cgicc::form_iterator gt_crs_name = cgi.getElement("gt_crs");
    cgicc::form_iterator gt_mac_name = cgi.getElement("gt_mac");
    cgicc::form_iterator gt_fifo_name = cgi.getElement("gt_fifo");
    cgicc::form_iterator gt_pktsnd_name = cgi.getElement("gt_pktsnd");
    cgicc::form_iterator gt_pktrcv_name = cgi.getElement("gt_pktrcv");
    cgicc::form_iterator gt_misc_name = cgi.getElement("gt_misc");

    if(gt_pswd_name != cgi.getElements().end()) {
      std::string VCC_PSWD_DO =
        toolbox::toString("/%s/VCC_PSWD_DO",getApplicationDescriptor()->getURN().c_str());

      *out << "<form action=\"" << VCC_PSWD_DO << "\" method=\"GET\">" << std::endl;
      *out << "<div align=\"center\">" << std::endl;
      *out << "<fieldset><legend style=\"font-size: 18pt;\" align=\"center\">Access Control</legend>" << std::endl;
      *out << "  <table border=\"3\"  cellspacing=\"2\" cellpadding=\"2\" style=\"border-collapse: collapse\" rules=\"none\">" << std::endl;
      *out << "    <tr>" << std::endl;
      *out << "      <td colspan=\"2\">" << std::endl;
      *out << "        <p>Expert Mode requires<br>that your name be logged.</p>" << std::endl;
      *out << "      </td>" << std::endl;
      *out << "    </tr>" << std::endl;
      *out << "    <tr>" << std::endl;
      *out << "      <td align=\"right\">Name: " << std::endl;
      *out << "      </td>" << std::endl;
      *out << "      <td>" << std::endl;
      *out << "        <input type=\"text\" align=\"left\" maxwidth=\"90\" size=\"9\" value=\"\" name=\"expert_name\" >";
      *out << "      </td>" << std::endl;
      *out << "    </tr>" << std::endl;
      *out << "    <tr>" << std::endl;
      *out << "      <td align=\"right\">Password: " << std::endl;
      *out << "      </td>" << std::endl;
      *out << "      <td>" << std::endl;
      *out << "        <input type=\"password\" align=\"left\" maxwidth=\"90\" size=\"9\" value=\"\" name=\"expert_pswd\">";
      *out << "      </td>" << std::endl;
      *out << "    </tr>" << std::endl;
      *out << "    <tr>" << std::endl;
      *out << "      <td colspan=\"2\" align=\"center\">" << std::endl;
      *out << "        <input type=\"submit\" value=\"Enter\" name=\"expert_mode\">" << std::endl;
      *out << "      </td>" << std::endl;
      *out << "    </tr>" << std::endl;
      *out << "  </table>" << std::endl;
      *out << "</fieldset>" << std::endl;
      *out << "</div>" << std::endl;
      *out << "</form>" << std::endl;
    }
    else if(st_usrmd_name != cgi.getElements().end()) {
      VCC_UTIL_acc_cntrl = "disabled";
      switch(VCC_UTIL_curr_page){
      case VCC_CMNTSK:
        this->ControllerUtils(in,out);
	break;
      case VCC_VME:
	this->VMECCGUI_VME_access(in,out);
	break;
      case VCC_FRMUTIL:
        this->VMECCGUI_firmware_utils(in,out);
	break;
      case VCC_CNFG:
	this->VMECCGUI_cnfg_utils(in,out);
	break;
      case VCC_MAC:
	this->VMECCGUI_MAC_utils(in,out);
	break;
      case VCC_FIFO:
	this->VMECCGUI_FIFO_utils(in,out);
	break;
      case VCC_PKTSND:
	this->VMECCGUI_pkt_send(in,out);
	break;
      case VCC_PKTRCV:
	this->VMECCGUI_pkt_rcv(in,out);
	break;
      case VCC_MISC:
	this->VMECCGUI_misc_utils(in,out);
        break;
      default:
        VCC_UTIL_curr_color = "\"#88CCCC\"";
        this->ControllerUtils(in,out);
	break;
      }
    }
    else if(gt_frmw_name != cgi.getElements().end()) {
      VCC_UTIL_curr_color = "\"#CCFFFF\"";
      this->VMECCGUI_firmware_utils(in,out);
    }
    else if(gt_crs_name != cgi.getElements().end()) {
      VCC_UTIL_curr_color = "\"#FFCCFF\"";
      this->VMECCGUI_cnfg_utils(in,out);
    }
    else if(gt_mac_name != cgi.getElements().end()) {
      VCC_UTIL_curr_color = "\"#FFFFCC\"";
      this->VMECCGUI_MAC_utils(in,out);
    }
    else if(gt_fifo_name != cgi.getElements().end()) {
      VCC_UTIL_curr_color = "\"#CCCCFF\"";
      this->VMECCGUI_FIFO_utils(in,out);
    }
    else if(gt_pktsnd_name != cgi.getElements().end()) {
      VCC_UTIL_curr_color = "\"#FFCCCC\"";
      this->VMECCGUI_pkt_send(in,out);
    }
    else if(gt_pktrcv_name != cgi.getElements().end()) {
      VCC_UTIL_curr_color = "\"#CCFFCC\"";
      this->VMECCGUI_pkt_rcv(in,out);
    }
    else if(gt_misc_name != cgi.getElements().end()) {
      VCC_UTIL_curr_color = "\"#FFCC88\"";
      this->VMECCGUI_misc_utils(in,out);
    }
    else {
      VCC_UTIL_curr_color = "\"#88CCCC\"";
      this->ControllerUtils(in,out);
    }
}

void EmuPeripheralCrateConfig::VMECC_UTIL_Crate_Selection(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  std::string optgroup = "INIT";
  std::string cr_label;
  bool first;
  //
  *out << "<fieldset><legend style=\"font-size: 18pt;\" align=\"right\">Crate Selection</legend>" << std::endl;
  *out << "<div align=\"right\">" << std::endl;
  *out << "<select name=\"crate_sel\">" << std::endl;
  first = true;
  for(int i=0; i< (int)crateVector.size(); i++){
    cr_label = crateVector[i]->GetLabel();
    if(cr_label.compare(0,5,optgroup)!= 0){
      if(!first){
        *out << "  </optgroup>" << std::endl;
      }
      optgroup = cr_label.substr(0,5);
      *out << "  <optgroup label=\"" << optgroup << "\">" << std::endl;
    }
    if(VCC_UTIL_curr_crate == crateVector[i]){
      *out << "    <option selected value=\"" << cr_label << "\">" << cr_label << "</option>" << std::endl;
    }
    else {
      *out << "    <option value=\"" << crateVector[i]->GetLabel() << "\">" << crateVector[i]->GetLabel() << "</option>" << std::endl;
    }
  }
  *out << "  </optgroup>" << std::endl;
  *out << "</select>" << std::endl;
  *out << "<input type=\"submit\" value=\"Set Crate\" name=\"set_crate\">" << std::endl;
  *out << "</div>" << std::endl;
  *out << "</fieldset>" << std::endl;

}

void EmuPeripheralCrateConfig::VCC_CRSEL_DO(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
  int i;
  bool found;

  std::cout<<" entered VCC_CRSEL_DO"<<std::endl;
  cgicc::Cgicc cgi(in);
  const cgicc::CgiEnvironment& env = cgi.getEnvironment();
  std::string guiStr = env.getQueryString() ;
  std::cout << guiStr << std::endl ;
  cgicc::form_iterator crate_sel = cgi.getElement("crate_sel");

  if(crate_sel != cgi.getElements().end()) {
    VCC_UTIL_curr_crate_name = cgi["crate_sel"]->getValue();
    std::cout << "selected " << VCC_UTIL_curr_crate_name << std::endl;
    found = false;
    for(i=0; i<(int)crateVector.size() && !found; i++){
      if(VCC_UTIL_curr_crate_name==crateVector[i]->GetLabel()){
        VCC_UTIL_curr_crate = crateVector[i];
        found = true;
      }
    }
    if(!found){
      std::cout << "Crate " << VCC_UTIL_curr_crate_name << " was not found in crate vector" << std::endl;
      VCC_UTIL_curr_crate_name = VCC_UTIL_curr_crate->GetLabel();
    }
  }
  switch(VCC_UTIL_curr_page){
  case VCC_CMNTSK:
    this->ControllerUtils(in,out);
    break;
  case VCC_VME:
    this->VMECCGUI_VME_access(in,out);
    break;
  case VCC_FRMUTIL:
    this->VMECCGUI_firmware_utils(in,out);
    break;
  case VCC_CNFG:
    this->VMECCGUI_cnfg_utils(in,out);
    break;
  case VCC_MAC:
    this->VMECCGUI_MAC_utils(in,out);
    break;
  case VCC_FIFO:
    this->VMECCGUI_FIFO_utils(in,out);
    break;
  case VCC_PKTSND:
    this->VMECCGUI_pkt_send(in,out);
    break;
  case VCC_PKTRCV:
    this->VMECCGUI_pkt_rcv(in,out);
    break;
  case VCC_MISC:
    this->VMECCGUI_misc_utils(in,out);
    break;
  default:
    VCC_UTIL_curr_color = "\"#88CCCC\"";
    this->ControllerUtils(in,out);
    break;
  }
}

void EmuPeripheralCrateConfig::VCC_PSWD_DO(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
  std::cout<<" entered VCC_PSWD_DO"<<std::endl;
  cgicc::Cgicc cgi(in);
  const cgicc::CgiEnvironment& env = cgi.getEnvironment();
  std::string guiStr = env.getQueryString() ;
  cgicc::form_iterator expert_mode = cgi.getElement("expert_mode");

  if(expert_mode != cgi.getElements().end()) {
    std::string pswd_entered = cgi["expert_pswd"]->getValue();
    std::string name_entered = cgi["expert_name"]->getValue();
    if(pswd_entered == VCC_UTIL_expert_pswd){
      std::cout << "VCC expert access granted to " << name_entered << std::endl;
      VCC_UTIL_acc_cntrl = "enabled";
    }
    else {
      std::cout << "VCC expert access denied to " << name_entered << std::endl;
      VCC_UTIL_acc_cntrl = "disabled";
    }
  }
  switch(VCC_UTIL_curr_page){
  case VCC_CMNTSK:
    this->ControllerUtils(in,out);
    break;
  case VCC_VME:
    this->VMECCGUI_VME_access(in,out);
    break;
  case VCC_FRMUTIL:
    this->VMECCGUI_firmware_utils(in,out);
    break;
  case VCC_CNFG:
    this->VMECCGUI_cnfg_utils(in,out);
    break;
  case VCC_MAC:
    this->VMECCGUI_MAC_utils(in,out);
    break;
  case VCC_FIFO:
    this->VMECCGUI_FIFO_utils(in,out);
    break;
  case VCC_PKTSND:
    this->VMECCGUI_pkt_send(in,out);
    break;
  case VCC_PKTRCV:
    this->VMECCGUI_pkt_rcv(in,out);
    break;
  case VCC_MISC:
    this->VMECCGUI_misc_utils(in,out);
    break;
  default:
    VCC_UTIL_curr_color = "\"#88CCCC\"";
    this->ControllerUtils(in,out);
    break;
  }
}


void EmuPeripheralCrateConfig::VCC_CMNTSK_DO(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
  char ctemp[256];
  unsigned char lb_tst[] = {0x00,Loopback,0xFA,0xCE,0xBA,0xC0};
  int n,i;
  char *cc;

  emu::pc::Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();
  std::cout<<" entered VCC_CMNTSK_DO"<<std::endl;
  cgicc::Cgicc cgi(in);
  const cgicc::CgiEnvironment& env = cgi.getEnvironment();
  std::string guiStr = env.getQueryString() ;
  std::cout << guiStr << std::endl ;
  cgicc::form_iterator cmntsk_tog_dbg = cgi.getElement("cmntsk_tog_dbg");
  cgicc::form_iterator cmntsk_vccsn = cgi.getElement("cmntsk_vccsn");
  cgicc::form_iterator cmntsk_cc = cgi.getElement("cmntsk_cc");
  cgicc::form_iterator cmntsk_ldfrmw = cgi.getElement("cmntsk_ldfrmw");
  cgicc::form_iterator cmntsk_reload = cgi.getElement("cmntsk_reload");
  cgicc::form_iterator cmntsk_lpbk = cgi.getElement("cmntsk_lpbk");

  if(cmntsk_tog_dbg != cgi.getElements().end()) {
    if ( lccc->vmeController()->GetDebug() == 0 ) {
      std::cout << "debug 1 " << std::endl;
      lccc->vmeController()->Debug(1);
      VCC_UTIL_cmn_tsk_dbg = "Enabled";
    } else {
      std::cout << "debug 0 " << std::endl;
      lccc->vmeController()->Debug(0);
      VCC_UTIL_cmn_tsk_dbg = "Disabled";
    }
  }
  if(cmntsk_vccsn != cgi.getElements().end()) {
    emu::pc::SN_t sn = vmecc->rd_ser_num();
    if(sn.status == 0){
      sprintf(ctemp,"VCC%02d",sn.sn);
      VCC_UTIL_cmn_tsk_sn = ctemp;
    }
    else {
      std::cout << "Error while reading serial number. Status=" << sn.status << "  Error type = " << std::hex << sn.err_typ << std::endl;
      VCC_UTIL_cmn_tsk_sn = "-";
    }
  }
  if(cmntsk_cc != cgi.getElements().end()) {
    cc = vmecc->read_customer_code();
    if(cc != 0){
      std::cout << "Customer Code is " << cc << std::endl;
      VCC_UTIL_cmn_tsk_cc=cc;
    } else {
      std::cout << "Customer Code readback failed!" << std::endl;
      VCC_UTIL_cmn_tsk_cc="Readback Failed!";
    }
  }
  if(cmntsk_ldfrmw != cgi.getElements().end()) {
    vmecc->set_clr_bits(emu::pc::SET, emu::pc::ETHER, ETH_CR_SPONT);
    vmecc->set_clr_bits(emu::pc::SET, emu::pc::RST_MISC, RST_CR_MSGLVL);
    std::string PROM_Path = FirmwareDir_+VMECC_FIRMWARE_DIR;
    std::cout << "Path = " << PROM_Path << "\nVer = " << vmecc->VCC_frmw_ver << std::endl;
    vmecc->prg_vcc_prom_ver(PROM_Path.c_str(),vmecc->VCC_frmw_ver.c_str());
  }
  if(cmntsk_reload != cgi.getElements().end()) {
    vmecc->force_reload();
  }
  if(cmntsk_lpbk != cgi.getElements().end()) {
    bool tst_err = false;
    vmecc->nwbuf=6;
    for(i=0;i<vmecc->nwbuf;i++){
      vmecc->wbuf[i]=lb_tst[i];
    }
    n = vmecc->eth_write();    
    n = vmecc->eth_read();
    if(n>6){
      for(i=0;i<vmecc->nwbuf;i++){
	if(vmecc->rbuf[DATA_OFF+i]&0xFF != lb_tst[i]&0xFF) tst_err = true;
      }
      if(tst_err){
        VCC_UTIL_cmn_tsk_lpbk = "failed";
        VCC_UTIL_cmn_tsk_lpbk_color = "#FF0000";
      }
      else {
        VCC_UTIL_cmn_tsk_lpbk = "passed";
        VCC_UTIL_cmn_tsk_lpbk_color = "#000000";
      }
    }
    else {
      VCC_UTIL_cmn_tsk_lpbk = "failed";
      VCC_UTIL_cmn_tsk_lpbk_color = "#FF0000";
    }
  }
  this->ControllerUtils(in,out);
}


void EmuPeripheralCrateConfig::VMECCGUI_VME_access(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  static bool first = true;
  static std::vector<std::string> board_opt,rw_opt,rw_opt_lbl;
  unsigned int opt;

  emu::pc::Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();
  if(first){
    first = false;
    VCC_UTIL_VME_board="";
    VCC_UTIL_VME_sub_addr="";
    VCC_UTIL_VME_rw="";
    VCC_UTIL_VME_data="";
    VCC_UTIL_VME_send_num="1";
    VCC_UTIL_VME_add_addr="";
    VCC_UTIL_VME_add_data=""; 
    VCC_UTIL_VME_brd_sel="TMB1";
    VCC_UTIL_VME_rw_sel="Read";
    VCC_UTIL_VME_msg_data = "";
    VCC_UTIL_VME_rbk_data = "";
    board_opt.push_back("DLY");
    board_opt.push_back("TMB1");
    board_opt.push_back("DMB1");
    board_opt.push_back("TMB2"); 
    board_opt.push_back("DMB2");
    board_opt.push_back("TMB3"); 
    board_opt.push_back("DMB3");
    board_opt.push_back("TMB4"); 
    board_opt.push_back("DMB4");
    board_opt.push_back("TMB5"); 
    board_opt.push_back("DMB5");
    board_opt.push_back("MPC"); 
    board_opt.push_back("CCB");
    board_opt.push_back("TMB6"); 
    board_opt.push_back("DMB6");
    board_opt.push_back("TMB7"); 
    board_opt.push_back("DMB7");
    board_opt.push_back("TMB8"); 
    board_opt.push_back("DMB8");
    board_opt.push_back("TMB9"); 
    board_opt.push_back("DMB9");
    rw_opt_lbl.push_back("Read"); 
    rw_opt_lbl.push_back("Write"); 
    rw_opt.push_back("R"); 
    rw_opt.push_back("W"); 
 }

  char title[] = "VCC Utilities: VME Access";
  char pbuf[300];
  sprintf(pbuf,"%s<br>Current Crate is %s<br>MAC Addr: %02x-%02x-%02x-%02x-%02x-%02x",title,(lccc->GetLabel()).c_str(),lccc->vmeController()->GetDestMAC(0),lccc->vmeController()->GetDestMAC(1),lccc->vmeController()->GetDestMAC(2),lccc->vmeController()->GetDestMAC(3),lccc->vmeController()->GetDestMAC(4),lccc->vmeController()->GetDestMAC(5));
  //
  VCCHeader(in,out,title,pbuf);
  //

  this->VMECC_UTIL_Menu_Buttons(in,out);

   std::string VCC_VME_DO =
     toolbox::toString("/%s/VCC_VME_DO",getApplicationDescriptor()->getURN().c_str());
   std::string VCC_VME_FILL =
     toolbox::toString("/%s/VCC_VME_FILL",getApplicationDescriptor()->getURN().c_str());

  *out << "<fieldset><legend style=\"font-size: 18pt;\" align=\"center\">VME Access</legend>" << std::endl;
  *out << "<div align=\"center\">" << std::endl;
  *out << "  <table border=\"3\" cellspacing=\"2\" cellpadding=\"0\" bgcolor=" << VCC_UTIL_curr_color << " style=\"border-collapse: collapse\">" << std::endl;
  *out << "    <tr><td valign=\"top\">" << std::endl;
  *out << "      <form action=\"" << VCC_VME_FILL << "\" method=\"GET\">" << std::endl;
  *out << "      <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Input</legend>" << std::endl;
  *out << "         <table cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "           <tr>" << std::endl;
  *out << "             <td align=\"center\">Board</td>" << std::endl;
  *out << "             <td align=\"center\">SubAddr</td>" << std::endl;
  *out << "             <td align=\"center\">R/W</td>" << std::endl;
  *out << "             <td align=\"center\">Data</td>" << std::endl;
  *out << "             <td></td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td>" << std::endl;
  *out << "               <select name=\"Brd_Sel\">" << std::endl;
  for(opt=0;opt<board_opt.size();opt++){
    if(board_opt[opt] == VCC_UTIL_VME_brd_sel){
      *out << "                 <option selected value=\"" << board_opt[opt] << "\">" << board_opt[opt] << "</option>" << std::endl;
    }
    else {
      *out << "                 <option value=\"" << board_opt[opt] << "\">" << board_opt[opt] << "</option>" << std::endl;
    }
  }
  *out << "               </select>" << std::endl;
  *out << "             </td>" << std::endl;
  *out << "             <td><input align=\"right\" value=\"" << VCC_UTIL_VME_add_addr << "\" size=\"8\" maxwidth=\"80\" name=\"add_addr\" type=\"text\"></td>" << std::endl;
  *out << "             <td>" << std::endl;
  *out << "               <select name=\"RW_Sel\">" << std::endl;
  for(opt=0;opt<rw_opt.size();opt++){
    if(rw_opt[opt] == VCC_UTIL_VME_rw_sel){
      *out << "                 <option selected value=\"" << rw_opt[opt] << "\">" << rw_opt_lbl[opt] << "</option>" << std::endl;
    }
    else {
      *out << "                 <option value=\"" << rw_opt[opt] << "\">" << rw_opt_lbl[opt] << "</option>" << std::endl;
    }
  }
  *out << "               </select>" << std::endl;
  *out << "             </td>" << std::endl;
  *out << "             <td><input align=\"right\" value=\"" << VCC_UTIL_VME_add_data << "\" size=\"8\" maxwidth=\"80\" name=\"add_data\" type=\"text\"></td>" << std::endl;
  *out << "             <td><input type=\"submit\" value=\"Add to list\" name=\"add2pkt\"></td>" << std::endl;
  *out << "           </tr>" << std::endl;
  *out << "         </table>" << std::endl;
  *out << "      </fieldset>" << std::endl;
  *out << "      </form>" << std::endl;
  *out << "    </td></tr>" << std::endl;
  *out << "    <tr><td valign=\"top\">" << std::endl;
  *out << "      <form action=\"" << VCC_VME_DO << "\" method=\"GET\">" << std::endl;
  *out << "      <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Packet Contents</legend>" << std::endl;
  *out << "         <table cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "           <tr>" << std::endl;
  *out << "             <td align=\"center\">Board</td>" << std::endl;
  *out << "             <td align=\"center\">SubAddr</td>" << std::endl;
  *out << "             <td align=\"center\">R/W</td>" << std::endl;
  *out << "             <td align=\"center\">Data</td>" << std::endl;
  *out << "             <td></td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><textarea name=\"vme_board\" rows=\"5\" cols=\"6\">" << VCC_UTIL_VME_board << "</textarea></td>" << std::endl;
  *out << "             <td><textarea name=\"vme_sub_addr\" rows=\"5\" cols=\"8\">" << VCC_UTIL_VME_sub_addr << "</textarea></td>" << std::endl;
  *out << "             <td><textarea name=\"vme_rw\" rows=\"5\" cols=\"3\">" << VCC_UTIL_VME_rw << "</textarea></td>" << std::endl;
  *out << "             <td><textarea name=\"vme_data\" rows=\"5\" cols=\"8\">" << VCC_UTIL_VME_data << "</textarea></td>" << std::endl;
  *out << "             <td></td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"vme_send\" type=\"submit\" value=\"Send\"></td>" << std::endl;
  *out << "             <td><input align=\"right\" value=\"" << VCC_UTIL_VME_send_num << "\" size=\"3\" maxwidth=\"30\" name=\"send_num\" type=\"text\"> time(s)</td>" << std::endl;
  *out << "             <td align=\"center\" colspan=\"2\"></td>" << std::endl;
  *out << "             <td align=\"center\"><input name=\"vme_clear\" type=\"submit\" value=\"Clear list\"></td>" << std::endl;
  *out << "           </tr>" << std::endl;
  *out << "         </table>" << std::endl;
  *out << "      </fieldset>" << std::endl;
  *out << "      </form>" << std::endl;
  *out << "    </td></tr>" << std::endl;
  *out << "    <tr>" << std::endl;
  *out << "      <td valign=\"top\">" << std::endl;
  *out << "        <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Output</legend>" << std::endl;
  *out << "          <table cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "            <tr>" << std::endl;
  *out << "              <td valign=\"top\" align = \"center\">" << std::endl;
  *out << "                <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Readback Data</legend>" << std::endl;
  *out << "                  <pre><p>" << VCC_UTIL_VME_rbk_data << "</p></pre>" << std::endl;
  *out << "                </fieldset>" << std::endl;
  *out << "              </td>" << std::endl;
  *out << "              <td valign=\"top\" align = \"center\">" << std::endl;
  *out << "                <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Messages</legend>" << std::endl;
  *out << "                  <pre><p>" << VCC_UTIL_VME_msg_data << "</p></pre></td>" << std::endl;
  *out << "                </fieldset>" << std::endl;
  *out << "              </td>" << std::endl;
  *out << "            </tr>" << std::endl;
  *out << "          </table>" << std::endl;
  *out << "        </fieldset>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "    </tr>" << std::endl;
  *out << "  </table>" << std::endl;
  *out << "</div>" << std::endl;
  *out << "</fieldset>" << std::endl;

  VCC_UTIL_curr_page = VCC_VME;
}

void EmuPeripheralCrateConfig::VCC_VME_FILL(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{

    std::cout<<" entered VCC_VME_FILL"<<std::endl;
    cgicc::Cgicc cgi(in);
    const cgicc::CgiEnvironment& env = cgi.getEnvironment();
    std::string guiStr = env.getQueryString() ;
    std::cout << guiStr << std::endl ;

    cgicc::form_iterator add2pkt = cgi.getElement("add2pkt");

    if(add2pkt != cgi.getElements().end()) {
      VCC_UTIL_VME_add_addr = cgi["add_addr"]->getValue();
      VCC_UTIL_VME_add_data = cgi["add_data"]->getValue();
      VCC_UTIL_VME_brd_sel  = cgi["Brd_Sel"]->getValue();
      VCC_UTIL_VME_rw_sel   = cgi["RW_Sel"]->getValue();

      VCC_UTIL_VME_board += VCC_UTIL_VME_brd_sel;
      VCC_UTIL_VME_board += "\n";

      if(VCC_UTIL_VME_brd_sel.compare(0,3,"DLY") == 0){
        VCC_UTIL_VME_sub_addr += "n/a\n";
        VCC_UTIL_VME_rw += "n/a\n";
        VCC_UTIL_VME_data += VCC_UTIL_VME_add_data;
        VCC_UTIL_VME_data += "\n";
      }
      else {
        VCC_UTIL_VME_sub_addr += VCC_UTIL_VME_add_addr;
        VCC_UTIL_VME_sub_addr += "\n";

        VCC_UTIL_VME_rw += VCC_UTIL_VME_rw_sel;
        VCC_UTIL_VME_rw += "\n";

        if(VCC_UTIL_VME_rw_sel.compare(0,3,"W") == 0){
          VCC_UTIL_VME_data += VCC_UTIL_VME_add_data;
          VCC_UTIL_VME_data += "\n";
	}
        else {
          VCC_UTIL_VME_data += "n/a\n";
	}
      }
    }
    this->VMECCGUI_VME_access(in,out);
}

void EmuPeripheralCrateConfig::VCC_VME_DO(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
  //  unsigned short int *pbuf;
  int i,n,nbrds,nvme,offset,itemp,pkt_type;
  bool abrt;
  char ctemp[256];
  char *ptemp1,*ptemp2,*ptemp3;
  struct vcmd_t {
    char brd[10];
    unsigned long addr;
    bool wrt;
    unsigned long data;
    struct vcmd_t *nxt;
    struct vcmd_t *prv;
  } vcmd,*cur,*last;


  emu::pc::Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();

    std::cout<<" entered VCC_VME_DO"<<std::endl;
    cgicc::Cgicc cgi(in);
    const cgicc::CgiEnvironment& env = cgi.getEnvironment();
    std::string guiStr = env.getQueryString() ;
    std::cout << guiStr << std::endl ;

    cgicc::form_iterator vme_send = cgi.getElement("vme_send");
    cgicc::form_iterator vme_clear = cgi.getElement("vme_clear");

    vcmd.prv=NULL;
    nbrds=0;
    abrt=false;
    if(vme_clear != cgi.getElements().end()) {
      VCC_UTIL_VME_board = "";
      VCC_UTIL_VME_sub_addr = "";
      VCC_UTIL_VME_rw = "";
      VCC_UTIL_VME_data = "";
    }
    if(vme_send != cgi.getElements().end()) {
      int send_num = cgi["send_num"]->getIntegerValue();
      VCC_UTIL_VME_send_num = cgi["send_num"]->getValue();
      VCC_UTIL_VME_board = cgi["vme_board"]->getValue();
      VCC_UTIL_VME_sub_addr = cgi["vme_sub_addr"]->getValue();
      VCC_UTIL_VME_rw = cgi["vme_rw"]->getValue();
      VCC_UTIL_VME_data = cgi["vme_data"]->getValue();
      size_t slen = VCC_UTIL_VME_board.length();
      ptemp1 = (char *) malloc(slen+1);
      VCC_UTIL_VME_board.copy(ptemp1,slen);
      ptemp1[slen]='\0';
      ptemp3=ptemp1;
      cur=&vcmd;
      while((ptemp2 = strtok(ptemp3,"\n\r")) !=NULL){
        ptemp3=NULL;
        strcpy(cur->brd,ptemp2);
        nbrds++;
	std::cout << "brd " << nbrds << " is " << cur->brd << std::endl;
	cur->nxt = (struct vcmd_t *) malloc(sizeof(struct vcmd_t));
        cur->nxt->prv=cur;
        cur=cur->nxt;
      }
      last=cur->prv;
      free(cur);
      last->nxt=NULL;
      nvme=nbrds;
      free(ptemp1);
      slen = VCC_UTIL_VME_sub_addr.length();
      ptemp1 = (char *) malloc(slen+1);
      VCC_UTIL_VME_sub_addr.copy(ptemp1,slen);
      ptemp1[slen]='\0';
      ptemp3=ptemp1;
      n=0;
      cur=&vcmd;
      while((ptemp2 = strtok(ptemp3,"\n\r")) !=NULL && n<nbrds){
        ptemp3=NULL;
        cur->addr = strtoul(ptemp2,NULL,16);
        n++;
        cur=cur->nxt;
      }
      free(ptemp1);
      if(n<nbrds){
	std::cout << " Error: not enough data provided for sub address" << std::endl;
	std::cout << " Command not executed" << std::endl;
        abrt=true;
      }
      slen = VCC_UTIL_VME_rw.length();
      ptemp1 = (char *) malloc(slen+1);
      VCC_UTIL_VME_rw.copy(ptemp1,slen);
      ptemp1[slen]='\0';
      ptemp3=ptemp1;
      n=0;
      cur=&vcmd;
      while((ptemp2 = strtok(ptemp3,"\n\r")) !=NULL && n<nbrds){
        ptemp3=NULL;
        cur->wrt=false;
        if(ptemp2[0]=='W') cur->wrt=true;
        n++;
        cur=cur->nxt;
      }
      free(ptemp1);
      if(n<nbrds){
	std::cout << " Error: not enough data provided for R/W entry" << std::endl;
	std::cout << " Command not executed" << std::endl;
        abrt=true;
      }
      slen = VCC_UTIL_VME_data.length();
      ptemp1 = (char *) malloc(slen+1);
      VCC_UTIL_VME_data.copy(ptemp1,slen);
      ptemp1[slen]='\0';
      ptemp3=ptemp1;
      n=0;
      cur=&vcmd;
      while((ptemp2 = strtok(ptemp3,"\n\r")) !=NULL && n<nbrds){
        ptemp3=NULL;
        cur->data = strtoul(ptemp2,NULL,16);
        n++;
        cur=cur->nxt;
      }
      free(ptemp1);
      if(n<nbrds){
	std::cout << " Error: not enough data provided for data entry" << std::endl;
	std::cout << " Command not executed" << std::endl;
        abrt=true;
      }
      if(!abrt){
	offset=4;
        cur=&vcmd;
        n=0;
	for (cur=&vcmd;cur!=NULL && !abrt;cur=cur->nxt){
          n++;
	  vmecc->wbuf[offset+0]=0x00;
	  if(cur->wrt){
	    vmecc->wbuf[offset+1]=0x54;
	  }
	  else {
	    vmecc->wbuf[offset+1]=0x44;
	  }
	  if(strncmp(cur->brd,"TMB",3)==0){
	    sscanf(cur->brd,"TMB%d",&itemp);
	    if(itemp>5)itemp++;
	    vmecc->wbuf[offset+2]=0x00;
	    vmecc->wbuf[offset+3]=((itemp<<4)&0xF0)|((cur->addr>>16)&0x7);
	    vmecc->wbuf[offset+4]=(cur->addr>>8)&0xFF;
	    vmecc->wbuf[offset+5]=cur->addr&0xFF;
	    if(cur->wrt){
	      vmecc->wbuf[offset+6]=(cur->data>>8)&0xFF;
	      vmecc->wbuf[offset+7]=cur->data&0xFF;
	      offset+=8;
	    }
	    else {
	      offset+=6;
	    }
	  }
	  else if(strncmp(cur->brd,"DMB",3)==0){
	    sscanf(cur->brd,"DMB%d",&itemp);
	    if(itemp>5)itemp++;
	    vmecc->wbuf[offset+2]=0x00;
	    vmecc->wbuf[offset+3]=((itemp<<4)&0xF0)|(0x08)|((cur->addr>>16)&0x7);
	    vmecc->wbuf[offset+4]=(cur->addr>>8)&0xFF;
	    vmecc->wbuf[offset+5]=cur->addr&0xFF;
	    if(cur->wrt){
	      vmecc->wbuf[offset+6]=(cur->data>>8)&0xFF;
	      vmecc->wbuf[offset+7]=cur->data&0xFF;
	      offset+=8;
	    }
	    else {
	      offset+=6;
	    }
	  }
	  else if(strncmp(cur->brd,"MPC",3)==0){
	    vmecc->wbuf[offset+2]=0x00;
	    vmecc->wbuf[offset+3]=(0x60)|((cur->addr>>16)&0x7);
	    vmecc->wbuf[offset+4]=(cur->addr>>8)&0xFF;
	    vmecc->wbuf[offset+5]=cur->addr&0xFF;
	    if(cur->wrt){
	      vmecc->wbuf[offset+6]=(cur->data>>8)&0xFF;
	      vmecc->wbuf[offset+7]=cur->data&0xFF;
	      offset+=8;
	    }
	    else {
	      offset+=6;
	    }
	  }
	  else if(strncmp(cur->brd,"CCB",3)==0){
	    vmecc->wbuf[offset+2]=0x00;
	    vmecc->wbuf[offset+3]=(0x68)|((cur->addr>>16)&0x7);
	    vmecc->wbuf[offset+4]=(cur->addr>>8)&0xFF;
	    vmecc->wbuf[offset+5]=cur->addr&0xFF;
	    if(cur->wrt){
	      vmecc->wbuf[offset+6]=(cur->data>>8)&0xFF;
	      vmecc->wbuf[offset+7]=cur->data&0xFF;
	      offset+=8;
	    }
	    else {
	      offset+=6;
	    }
	  }
	  else if(strncmp(cur->brd,"DLY",3)==0){
	    vmecc->wbuf[offset+0]=0x02;
	    vmecc->wbuf[offset+1]=0x00;
	    vmecc->wbuf[offset+2]=(cur->data>>8)&0xFF;
	    vmecc->wbuf[offset+3]=cur->data&0xFF;
	    offset+=4;
	  }
	  else{
            n--;
	    std::cout<<"  Illegal board option: " << cur->brd << std::endl;
	    std::cout<<"  Only executing " << n << " VME commands" << std::endl;
	    abrt=true;
	  }
	}
        nvme=n;
        vmecc->wbuf[2]=(nvme>>8)&0xFF;
        vmecc->wbuf[3]=nvme&0xFF;
        for(n=0;n<send_num;n++){
          std::cout << "VME Data: " << std::endl;
          for(i=2;i<offset;i+=2){
            printf("%02X%02X\n",vmecc->wbuf[i]&0xFF,vmecc->wbuf[i+1]&0xFF);
          }
          vmecc->vme_cmds((offset-2)/2);
	}
      }
      VCC_UTIL_VME_msg_data = "";
      VCC_UTIL_VME_rbk_data = "";
      while((pkt_type=vmecc->rd_pkt())>=0){
        if(pkt_type>emu::pc::INFO_PKT){
          std::cout << vmecc->dcode_msg_pkt(vmecc->rbuf) << std::endl;
          VCC_UTIL_VME_msg_data += vmecc->dcode_msg_pkt(vmecc->rbuf);
          VCC_UTIL_VME_msg_data += "\n";
	}
        else if (pkt_type==emu::pc::VMED16_PKT){
          int nw = ((vmecc->rbuf[WRD_CNT_OFF]&0xff)<<8)|(vmecc->rbuf[WRD_CNT_OFF+1]&0xff);
	  for(i=0;i<nw;i++){
	    sprintf(ctemp,"0x%02X%02X\n",vmecc->rbuf[2*i+DATA_OFF]&0xFF,vmecc->rbuf[2*i+DATA_OFF+1]&0xFF);
	    VCC_UTIL_VME_rbk_data += ctemp;
	  }
	}
        else {
          sprintf(ctemp,"Pkt Type: 0x%02X\n",pkt_type);
          std::cout << ctemp << std::endl;
          VCC_UTIL_VME_msg_data += ctemp;
	}
      }
    }
    this->VMECCGUI_VME_access(in,out);
}

void EmuPeripheralCrateConfig::VMECCGUI_firmware_utils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  static bool first = true;
  emu::pc::Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  if(first){
    first = false;
    for(int i=0;i<7;i++){
        VCC_UTIL_Frmw_rbk_[i]="Not yet read back";
    }
    VCC_UTIL_base_addr_inp = "000";
    VCC_UTIL_Frmw_rbk_[6] = "???";
    VCC_UTIL_Prnt_Rtns = "";
    VCC_UTIL_prom_file_inp = "D783C.V4.29.mcs"; 
  }

  char title[] = "VCC Utilities: Firmware";
  char pbuf[300];
  sprintf(pbuf,"%s<br>Current Crate is %s<br>MAC Addr: %02x-%02x-%02x-%02x-%02x-%02x",title,(lccc->GetLabel()).c_str(),lccc->vmeController()->GetDestMAC(0),lccc->vmeController()->GetDestMAC(1),lccc->vmeController()->GetDestMAC(2),lccc->vmeController()->GetDestMAC(3),lccc->vmeController()->GetDestMAC(4),lccc->vmeController()->GetDestMAC(5));
  //
  VCCHeader(in,out,title,pbuf);
  //

  this->VMECC_UTIL_Menu_Buttons(in,out);

   std::string VCC_FRMUTIL_DO =
     toolbox::toString("/%s/VCC_FRMUTIL_DO",getApplicationDescriptor()->getURN().c_str());

  *out << "<form action=\"" << VCC_FRMUTIL_DO << "\" method=\"GET\">" << std::endl;
  *out << "<fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Firmware Utilities</legend>" << std::endl;
  *out << "<div align=\"center\">" << std::endl;
  *out << "  <table border=\"3\"  cellspacing=\"2\" cellpadding=\"2\" bgcolor=" << VCC_UTIL_curr_color << " style=\"border-collapse: collapse\" id=\"table9\">" << std::endl;
  *out << "    <tr>" << std::endl;
  *out << "      <td valign=\"top\">" << std::endl;
  *out << "        <fieldset ><legend style=\"font-size: 16pt;\" align=\"center\" >Read Backs</legend>" << std::endl;
  *out << "          <div><table cellpadding=\"2\">" << std::endl;
  *out << "	       <tr><td align=\"right\"><input type=\"submit\" value=\"Device ID\" name=\"dvid\"></td><td align=\"left\">" << VCC_UTIL_Frmw_rbk_[0] << "</td></tr>" << std::endl;
  *out << "	       <tr><td align=\"right\"><input type=\"submit\" value=\"User Code\" name=\"rd_uc\"></td><td align=\"left\">" << VCC_UTIL_Frmw_rbk_[1] << "</td></tr>"  << std::endl;
  *out << "	       <tr><td align=\"right\"><input type=\"submit\" value=\"Customer Code\" name=\"rd_cc\"></td><td align=\"left\">" << VCC_UTIL_Frmw_rbk_[2] << "</td></tr>"  << std::endl;
  *out << "	       <tr><td align=\"right\"><input type=\"submit\" value=\"Check Connection\" name=\"chk_con\"></td><td align=\"left\">" << VCC_UTIL_Frmw_rbk_[3] << "</td></tr>"  << std::endl;
  *out << "	       <tr><td align=\"right\"><input type=\"submit\" value=\"JTAG Conrtol Status\" name=\"status\"></td><td align=\"left\">" << VCC_UTIL_Frmw_rbk_[4] << "</td></tr>"  << std::endl;
  *out << "	       <tr><td align=\"right\"><input type=\"submit\" value=\"Print Routines\" name=\"prt_rtn\"></td><td>" << VCC_UTIL_Frmw_rbk_[5] << "</td></tr>"  << std::endl;
  *out << "	     </table></div>" << std::endl;
  *out << "	   </fieldset>" << std::endl;
  *out << "	 </td>" << std::endl;
  *out << "    <td valign=\"top\">" << std::endl;
  *out << "      <fieldset align=top><legend style=\"font-size: 16pt;\" align=\"center\">Inputs</legend>" << std::endl;
  *out << "        <div><table cellpadding=\"2\">" << std::endl;
  *out << "            <tr><td align = \"right\">Base Address:</td>\n";
  *out << "                <td><input align=\"right\" maxwidth=\"30\" size=\"3\" value=\"" << VCC_UTIL_base_addr_inp << "\" name=\"base_addr\" type=\"text\"></td>\n";
  *out << "                <td><input type=\"submit\" value=\"Set\" name=\"set_base\" " << VCC_UTIL_acc_cntrl << "></td>\n";
  *out << "                <td></td></tr>" << std::endl;
  *out << "            <tr><td align = \"right\">Current Value:</td>\n";
  *out << "                <td>" << VCC_UTIL_Frmw_rbk_[6] << "</td>\n";
  *out << "                <td colspan = \"2\"></td></tr>" << std::endl;
  *out << "          </table>" << std::endl;
  *out << "          <hr>" << std::endl;
  *out << "          <table cellpadding=\"2\">" << std::endl;
  *out << "            <tr><td align = \"right\">PROM File:</td>\n";
  *out << "                <td><input align=\"left\" value=\"" << VCC_UTIL_prom_file_inp << "\" size=\"32\" maxwidth=\"256\" name=\"mcs_file\" type=\"text\">\n";
  *out << "                <td><input type=\"submit\" value=\"Read file\" name=\"rdmcs\"></td></tr>" << std::endl;
  *out << "            <tr><td align = \"right\">File in Mem:</td>\n";
  *out << "                <td>" << VCC_UTIL_Frmw_rbk_[7] << "</td>";
  *out << "                <td></td></tr>" << std::endl;
  *out << "          </table></div>" << std::endl;
  *out << "        </fieldset>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "    </tr>" << std::endl;
  *out << "    <tr>" << std::endl;
  *out << "      <td colspan=\"2\" valign=\"top\">" << std::endl;
  *out << "        <fieldset align=top><legend style=\"font-size: 16pt;\" align=\"center\">Configure</legend>" << std::endl;
  *out << "          <div><table cellpadding=\"2\"><tr>" << std::endl;
  *out << "            <td><input type=\"submit\" value=\"Load Routines\" name=\"ld_rtn\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "            <td><input type=\"submit\" value=\"Erase PROM\" name=\"erase\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "            <td><input type=\"submit\" value=\"Program PROM\" name=\"program\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "            <td><input type=\"submit\" value=\"Program with Verify\" name=\"prgver\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "            <td><input type=\"submit\" value=\"Execute Custom Routine\" name=\"custom\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "          </tr><tr>" << std::endl;
  *out << "            <td><input type=\"submit\" value=\"Write Mem to file\" name=\"wrtdat\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "            <td><input type=\"submit\" value=\"Read Back PROM\" name=\"rdbk\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "            <td><input type=\"submit\" value=\"Verify Cmd\" name=\"verify\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "            <td><input type=\"submit\" value=\"Program FPGA\" name=\"jreload\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "            <td><input type=\"submit\" value=\"Abort JTAG Commands\" name=\"abort\"></td>" << std::endl;
  *out << "          </tr></table></div>" << std::endl;
  *out << "        </fieldset>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "    </tr>" << std::endl;
  *out << "    <tr><td colspan = \"2\" align = \"center\"><pre><p>" << VCC_UTIL_Prnt_Rtns << "</p></pre>" << std::endl;
  *out << "    </td></tr>" << std::endl;
  *out << "  </table>" << std::endl;
  *out << "</div>" << std::endl;
  *out << "</fieldset>" << std::endl;
  *out << "</form>" << std::endl;

  VCC_UTIL_curr_page = VCC_FRMUTIL;
}

   // call back action from GUI

void EmuPeripheralCrateConfig::VCC_FRMUTIL_DO(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
  char ctemp[256];
  char *cptemp;
  int i,ptyp,ack;
  emu::pc::Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  emu::pc::CNFG_ptr rbk_cp;
  vmecc=lccc->vmecc();
    std::cout<<" entered VCC_FRMUTIL_DO"<<std::endl;
    cgicc::Cgicc cgi(in);
    const cgicc::CgiEnvironment& env = cgi.getEnvironment();
    std::string guiStr = env.getQueryString() ;
    std::cout << guiStr << std::endl ;
    cgicc::form_iterator dvid_name = cgi.getElement("dvid");
    cgicc::form_iterator rd_uc_name = cgi.getElement("rd_uc");
    cgicc::form_iterator rd_cc_name = cgi.getElement("rd_cc");
    cgicc::form_iterator chk_con_name = cgi.getElement("chk_con");
    cgicc::form_iterator status_name = cgi.getElement("status");
    cgicc::form_iterator prt_rtn_name = cgi.getElement("prt_rtn");
    cgicc::form_iterator set_base_name = cgi.getElement("set_base");
    cgicc::form_iterator base_addr_name = cgi.getElement("base_addr");
    cgicc::form_iterator rdmcs_name = cgi.getElement("rdmcs");
    cgicc::form_iterator mcs_file_name = cgi.getElement("mcs_file");
    cgicc::form_iterator ld_rtn_name = cgi.getElement("ld_rtn");
    cgicc::form_iterator erase_name = cgi.getElement("erase");
    cgicc::form_iterator program_name = cgi.getElement("program");
    cgicc::form_iterator prgver_name = cgi.getElement("prgver");
    cgicc::form_iterator custom_name = cgi.getElement("custom");
    cgicc::form_iterator wrtdat_name = cgi.getElement("wrtdat");
    cgicc::form_iterator rdbk_name = cgi.getElement("rdbk");
    cgicc::form_iterator verify_name = cgi.getElement("verify");
    cgicc::form_iterator jreload_name = cgi.getElement("jreload");
    cgicc::form_iterator abort_name = cgi.getElement("abort");
    unsigned int dvid=0;
    unsigned int uc=0;
    char *cc;
    int con_stat=0;
    int nl=0,n;
    unsigned int status=0;
    unsigned short int base_addr=0;
    char *pch =(char *) &VCC_UTIL_base_addr_inp[0];
    std::cout.fill('0');
    std::cout.setf(std::ios::showbase);
    if(dvid_name != cgi.getElements().end()) {
      dvid = vmecc->read_dev_id();
      if(dvid>0){
        std::cout << "Device ID is " << std::setw(8) << std::hex << dvid << std::endl;
        sprintf(ctemp,"%08X",dvid);
        VCC_UTIL_Frmw_rbk_[0]=ctemp;
      } else {
        std::cout << "Device ID readback failed!" << std::endl;
        VCC_UTIL_Frmw_rbk_[0]="Readback Failed!";
      }
    }
    if(rd_uc_name != cgi.getElements().end()) {
      uc = vmecc->read_user_code();
      if(uc>0){
        std::cout << "User Code is " << std::setw(8) << std::hex  << uc << std::endl;
        sprintf(ctemp,"%08X",uc);
        VCC_UTIL_Frmw_rbk_[1]=ctemp;
      } else {
        std::cout << "User Code readback failed!" << std::endl;
        VCC_UTIL_Frmw_rbk_[1]="Readback Failed!";
      }
    }
    if(rd_cc_name != cgi.getElements().end()) {
      cc = vmecc->read_customer_code();
      if(cc != 0){
        std::cout << "Customer Code is " << cc << std::endl;
        VCC_UTIL_Frmw_rbk_[2]=cc;
      } else {
        std::cout << "Customer Code readback failed!" << std::endl;
        VCC_UTIL_Frmw_rbk_[2]="Readback Failed!";
      }
    }
    if(chk_con_name != cgi.getElements().end()) {
      con_stat = vmecc->chk_jtag_conn();
      if(con_stat == 1){
        std::cout << "JTAG connection is good." << std::endl;
        VCC_UTIL_Frmw_rbk_[3]="Good.";
      }
      else {
        std::cout << "JTAG connection is BAD!" << std::endl;
        VCC_UTIL_Frmw_rbk_[3]="BAD!";
      }
    }
    if(status_name != cgi.getElements().end()) {
      status = vmecc->get_jtag_status();
      if(status>0){
        std::cout << "JTAG Module Status is " << std::setw(4) << std::hex << status << std::endl;
        sprintf(ctemp,"0x%04X",status&0xFFFF);
        VCC_UTIL_Frmw_rbk_[4]=ctemp;
      } else {
        std::cout << "Status readback failed!" << std::endl;
        VCC_UTIL_Frmw_rbk_[4]="Readback Failed!";
      }
    }
    if(prt_rtn_name != cgi.getElements().end()) {
      VCC_UTIL_Prnt_Rtns = vmecc->print_routines();
      VCC_UTIL_Frmw_rbk_[5]="See below for output";
    }
    if(set_base_name != cgi.getElements().end()) {
      VCC_UTIL_base_addr_inp = cgi["base_addr"]->getValue();
      base_addr = (unsigned short) strtol(VCC_UTIL_base_addr_inp.c_str(),&pch,16);
      vmecc->ld_rtn_base_addr(base_addr);
      std::cout << "base address set " << std::setw(4) << std::hex  << base_addr << std::endl;
      VCC_UTIL_Frmw_rbk_[6]= VCC_UTIL_base_addr_inp;
    }
    if(rdmcs_name != cgi.getElements().end()) {
      VCC_UTIL_prom_file_inp = cgi["mcs_file"]->getValue();
      strcpy(ctemp,VCC_UTIL_prom_file_inp.c_str());
      cptemp=strtok(ctemp,".");
      if(cptemp!=NULL) VCC_UTIL_proj = cptemp;
      cptemp=strtok(0,".");
      if(cptemp!=NULL) VCC_UTIL_maj_ver = cptemp+1;
      cptemp=strtok(0,".");
      if(cptemp!=NULL) VCC_UTIL_min_ver = cptemp;
      std::cout << "Project: " << VCC_UTIL_proj << std::endl;
      std::cout << "Major Version: " << VCC_UTIL_maj_ver << std::endl;
      std::cout << "Minor Version: " << VCC_UTIL_min_ver << std::endl;
      VCC_UTIL_PROM_ver = VCC_UTIL_maj_ver + "." + VCC_UTIL_min_ver;
      std::cout << "PROM  Version: " << VCC_UTIL_PROM_ver << std::endl;
      std::string Full_PROM_Filename = FirmwareDir_+VMECC_FIRMWARE_DIR+"/"+VCC_UTIL_prom_file_inp;
      nl = vmecc->read_mcs(Full_PROM_Filename.c_str());
      if(nl>0){
        if(nl==65553){
          std::cout << "File " << Full_PROM_Filename << " has been readin." << std::endl;
          VCC_UTIL_Frmw_rbk_[7]=VCC_UTIL_prom_file_inp;
          VCC_UTIL_PROM_file_init = true;
	}
	else {
          VCC_UTIL_Frmw_rbk_[7]="Readin Failed!";
          std::cout << "Readin Failed!" << std::endl;
          std::cout << "Wrong number of lines readin! Lines read = " << nl << std::endl;
	  VCC_UTIL_PROM_file_init = false;
	}
      }
      else {
        VCC_UTIL_Frmw_rbk_[7]="Readin Failed!";
        std::cout << "Readin Failed!" << std::endl;
        VCC_UTIL_PROM_file_init = false;
      }
    }
    if(ld_rtn_name != cgi.getElements().end()) {
      vmecc->jtag_init();
    }
    if(erase_name != cgi.getElements().end()) {
      int rslt = vmecc->erase_prom();
      if(rslt == 1){
        printf("PROM successfully erased.\n");
      }
      else{
        printf("\a\aPROM is not erased!\n");
      }
    }
    if(program_name != cgi.getElements().end()) {
      if(VCC_UTIL_PROM_file_init){
	int pktnum = 0;
        rbk_cp = vmecc->read_crs();
        vmecc->set_clr_bits(emu::pc::SET, emu::pc::RST_MISC, RST_CR_MSGLVL);
        vmecc->set_clr_bits(emu::pc::SET, emu::pc::ETHER, ETH_CR_SPONT);
	vmecc->program_prom_cmd();
	vmecc->send_prg_prom_data();
	vmecc->send_uc_cc_data(VCC_UTIL_Frmw_rbk_[7].c_str());
	while((n=vmecc->eth_read())>6){
	  pktnum++;
	  ptyp = vmecc->rbuf[PKT_TYP_OFF]&0xff;
	  if(ptyp>=emu::pc::INFO_PKT){
            std::cout << "pktnum B" << pktnum << ": " << vmecc->dcode_msg_pkt(vmecc->rbuf) << std::endl;
	  } else {
            std::cout << "pktnum B" << pktnum << ": ";
	    ack = AK_STATUS(vmecc->rbuf); 
	    switch(ack){
	    case emu::pc::NO_ACK:
	      std::cout << "Non message or no acknowledge packet received" << std::endl;
	      break;
	    case emu::pc::CC_S:
	      std::cout << "Command completed successfully" << std::endl;
	      break;
	    case emu::pc::CC_W:
	      std::cout << "Command completed with a warning" << std::endl;
	      break;
	    case emu::pc::CC_E:
	      std::cout << "Command  completed with an error" << std::endl;
	      break;
	    case emu::pc::CE_I:
	      std::cout << "Command execution finished incomplete" << std::endl;
	      break;
	    case emu::pc::CIP_W: case emu::pc::CIP_E:
	      std::cout << "Command in progress with an error or warning" << std::endl;
	      break;
	    default:
	      std::cout << "Unknow packet returned" << std::endl;
	      break;
	    }
	  }
	}
        vmecc->wrt_crs(emu::pc::RST_MISC, rbk_cp);
        free(rbk_cp);
      }
      else {
        std::cout << "You must read in the mcs file first\n" << std::endl;
      }
    }
    if(prgver_name != cgi.getElements().end()) {
      if(VCC_UTIL_PROM_file_init){
        vmecc->set_clr_bits(emu::pc::SET, emu::pc::ETHER, ETH_CR_SPONT);
        vmecc->set_clr_bits(emu::pc::SET, emu::pc::RST_MISC, RST_CR_MSGLVL);
        emu::pc::CNFG_ptr cp=vmecc->read_crs();
        vmecc->print_crs(cp);
        free(cp);
        std::string PROM_Path = FirmwareDir_+VMECC_FIRMWARE_DIR;
        std::cout << "Path = " << PROM_Path << "\nVer = " << VCC_UTIL_PROM_ver << std::endl;
        vmecc->prg_vcc_prom_ver(PROM_Path.c_str(),VCC_UTIL_PROM_ver.c_str());
      }
      else {
        std::cout << "You must set the version number by reading in the mcs file first\n" << std::endl;
      }
    }
    if(custom_name != cgi.getElements().end()) {
      vmecc->ld_rtn_base_addr(0x180);
      vmecc->exec_routine(emu::pc::JC_Custom_Rtn);
      while((n=vmecc->eth_read())>6){
        ptyp = vmecc->rbuf[PKT_TYP_OFF]&0xff;
	if(ptyp>=emu::pc::INFO_PKT){
	  printf("%s",vmecc->dcode_msg_pkt(vmecc->rbuf));
	} else {
          printf("Rtn_Pkt: ");
          for(i=0;i<n;i++)printf("%02X",vmecc->rbuf[i]&0xFF);
          printf("\n");
	}
      }
    }
    if(wrtdat_name != cgi.getElements().end()) {
      std::string MCS_mem_file = FirmwareDir_+"/vcc/MCS_data_inmem.dat";
      FILE *fp=fopen(MCS_mem_file.c_str(),"w");
      for(i=0;i<0x80000;i++){
        if(i!=0 && i%8==0)fprintf(fp,"\n");
        fprintf(fp,"%04hX",emu::pc::prm_dat[i]);
      }
      fprintf(fp,"\n");
      fclose(fp);
    }
    if(rdbk_name != cgi.getElements().end()) {
      std::string MCS_rbk_file = FirmwareDir_+"/vcc/Prom_rbk.dat";
      vmecc->rd_back_prom(MCS_rbk_file.c_str());
    }
    if(verify_name != cgi.getElements().end()) {
      if(VCC_UTIL_PROM_file_init){
        rbk_cp = vmecc->read_crs();
        vmecc->set_clr_bits(emu::pc::SET, emu::pc::RST_MISC, RST_CR_MSGLVL);
        vmecc->set_clr_bits(emu::pc::SET, emu::pc::ETHER, ETH_CR_SPONT);
        vmecc->verify_prom_cmd();
        vmecc->send_ver_prom_data();
        vmecc->wrt_crs(emu::pc::RST_MISC, rbk_cp);
        free(rbk_cp);
      }
      else {
        printf("You must read in the mcs file first\n");
      }
    }
    if(jreload_name != cgi.getElements().end()) {
      vmecc->reload_fpga();
    }
    if(abort_name != cgi.getElements().end()) {
      vmecc->abort_jtag_cmnds();
    }
    this->VMECCGUI_firmware_utils(in,out);
}

void EmuPeripheralCrateConfig::VMECCGUI_cnfg_utils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  static bool first = true;
  emu::pc::Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  if(first){
    first = false;
    for(int i=0;i<6;i++){
        VCC_UTIL_CR_cur_[i]="-";
        VCC_UTIL_CR_dflt_[i]="-";
        VCC_UTIL_CR_flashA_[i]="-";
        VCC_UTIL_CR_flashB_[i]="-";
    }
    VCC_UTIL_CR_wrt_[0]="0050";
    VCC_UTIL_CR_wrt_[1]="0002";
    VCC_UTIL_CR_wrt_[2]="031B";
    VCC_UTIL_CR_wrt_[3]="EDFF1D0F";
    VCC_UTIL_CR_wrt_[4]="30D4";
    VCC_UTIL_CR_wrt_[5]="03C5";
    VCC_UTIL_CR_wrt_[6]="0";
    VCC_UTIL_CR_cnumA="0";
    VCC_UTIL_CR_cnumB="1";
    VCC_UTIL_CR_sav_cnum="0";
    VCC_UTIL_CR_rstr_cnum="0";
    VCC_UTIL_CR_dflt_cnum="0";
    VCC_UTIL_CR_curr_dflt="-";
    VCC_UTIL_CR_ser_num="-";
  }

  char title[] = "VCC Utilities: Config Regs";
  char pbuf[300];
  sprintf(pbuf,"%s<br>Current Crate is %s<br>MAC Addr: %02x-%02x-%02x-%02x-%02x-%02x",title,(lccc->GetLabel()).c_str(),lccc->vmeController()->GetDestMAC(0),lccc->vmeController()->GetDestMAC(1),lccc->vmeController()->GetDestMAC(2),lccc->vmeController()->GetDestMAC(3),lccc->vmeController()->GetDestMAC(4),lccc->vmeController()->GetDestMAC(5));
  //
  VCCHeader(in,out,title,pbuf);
  //

  this->VMECC_UTIL_Menu_Buttons(in,out);

   std::string VCC_CNFG_DO =
     toolbox::toString("/%s/VCC_CNFG_DO",getApplicationDescriptor()->getURN().c_str());

  *out << "<form action=\"" << VCC_CNFG_DO << "\" method=\"GET\">" << std::endl;
  *out << "<fieldset><legend style=\"font-size: 18pt;\" align=\"center\">Configuration Registers</legend>" << std::endl;
  *out << "<div align=\"center\">" << std::endl;
  *out << "  <table border=\"3\" cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << " rules=\"groups\">" << std::endl;
  *out << "    <colgroup span=\"3\"><colgroup span=\"4\">" << std::endl;
  *out << "    <tr>" << std::endl;
  *out << "      <th colspan=\"3\" style=\"font-size: 14pt; color: blue\">Inputs</th>" << std::endl;
  *out << "      <th colspan=\"4\" style=\"font-size: 14pt; color: blue\">Read Backs</th>" << std::endl;
  *out << "    </tr><tr>" << std::endl; 
  *out << "      <td></td>" << std::endl;
  *out << "      <td></td>" << std::endl;
  *out << "      <td>In Mem</td>" << std::endl;
  *out << "      <td align=\"center\">In FPGA</td>" << std::endl;
  *out << "      <td colspan=\"3\" align=\"center\">In FLASH</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td align=\"center\">Register</td>" << std::endl;
  *out << "      <td>Sel</td>" << std::endl;
  *out << "      <td align=\"center\"><input name=\"wrt_sel_crs\" type=\"submit\" value=\"Write Sel.\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "      <td><input name=\"rbk_cur_crs\" type=\"submit\" value=\"Current\"></td>" << std::endl;
  *out << "      <td><input name=\"rbk_dflt_cnfg\" type=\"submit\" value=\"Default\"></td>" << std::endl;
  *out << "      <td><input name=\"rbk_cnumA\" type=\"submit\" value=\"Cnfg #\"><input maxwidth=\"20\" size=\"2\" value=\"" << VCC_UTIL_CR_cnumA << "\" name=\"cnumA\" type=\"text\"></td>" << std::endl;
  *out << "      <td><input name=\"rbk_cnumB\" type=\"submit\" value=\"Cnfg #\"><input maxwidth=\"20\" size=\"2\" value=\"" << VCC_UTIL_CR_cnumB << "\" name=\"cnumB\" type=\"text\"></td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td align=\"right\">Ethernet</td>" << std::endl;
  *out << "      <td><input name=\"eth_cr_chk\" type=\"checkbox\" value=\"eth_cr\"></td>" << std::endl;
  *out << "      <td align=\"right\"><input align=\"right\" value=\"" << VCC_UTIL_CR_wrt_[0] << "\" size=\"6\" maxwidth=\"60\" name=\"eth_cr_val\" type=\"text\"></td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_cur_[0] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_dflt_[0] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_flashA_[0] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_flashB_[0] << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td align=\"right\">Ext. FIFO</td>" << std::endl;
  *out << "      <td><input name=\"exfifo_cr_chk\" type=\"checkbox\" value=\"exfifo_cr\"></td>" << std::endl;
  *out << "      <td align=\"right\"><input align=\"right\" value=\"" << VCC_UTIL_CR_wrt_[1] << "\" size=\"6\" maxwidth=\"60\" name=\"exfifo_cr_val\" type=\"text\"></td>" << std::endl;
  *out << "       <td align=\"center\">" << VCC_UTIL_CR_cur_[1] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_dflt_[1] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_flashA_[1] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_flashB_[1] << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td align=\"right\">Rst / Misc.</td>" << std::endl;
  *out << "      <td><input name=\"rstmsc_cr_chk\" type=\"checkbox\" value=\"rstmsc_cr\"></td>" << std::endl;
  *out << "      <td align=\"right\"><input align=\"right\" value=\"" << VCC_UTIL_CR_wrt_[2] << "\" size=\"6\" maxwidth=\"60\" name=\"rstmsc_cr_val\" type=\"text\"></td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_cur_[2] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_dflt_[2] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_flashA_[2] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_flashB_[2] << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td align=\"right\">VME</td>" << std::endl;
  *out << "      <td><input name=\"vme_cr_chk\" type=\"checkbox\" value=\"vme_cr\"></td>" << std::endl;
  *out << "      <td align=\"right\"><input align=\"right\" value=\"" << VCC_UTIL_CR_wrt_[3] << "\" size=\"10\" maxwidth=\"120\" name=\"vme_cr_val\" type=\"text\"></td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_cur_[3] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_dflt_[3] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_flashA_[3] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_flashB_[3] << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td align=\"right\">BTO</td>" << std::endl;
  *out << "      <td><input name=\"bto_chk\" type=\"checkbox\" value=\"bto\"></td>" << std::endl;
  *out << "      <td align=\"right\"><input align=\"right\" value=\"" << VCC_UTIL_CR_wrt_[4] << "\" size=\"6\" maxwidth=\"60\" name=\"bto_val\" type=\"text\"></td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_cur_[4] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_dflt_[4] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_flashA_[4] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_flashB_[4] << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td align=\"right\">BGTO</td>" << std::endl;
  *out << "      <td><input name=\"bgto_chk\" type=\"checkbox\" value=\"bgto\"></td>" << std::endl;
  *out << "      <td align=\"right\"><input align=\"right\" value=\"" << VCC_UTIL_CR_wrt_[5] << "\" size=\"6\" maxwidth=\"60\" name=\"bgto_val\" type=\"text\"></td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_cur_[5] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_dflt_[5] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_flashA_[5] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_flashB_[5] << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td><input name=\"rst_crs\" type=\"reset\" value=\"Reset\"></td>" << std::endl;
  *out << "      <td></td>" << std::endl;
  *out << "      <td align=\"center\"><input name=\"wrt_all_crs\" type=\"submit\" value=\"Write All.\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "      <td align=\"center\" colspan=\"2\"><input type=\"submit\" value=\"Save Curr as Cnfg#\" name=\"sav_crs\" " << VCC_UTIL_acc_cntrl << ">" << std::endl;
  *out << "          <input align=\"right\" maxwidth=\"20\" size=\"2\" value=\"" << VCC_UTIL_CR_sav_cnum << "\" name=\"sav_cnum\" type=\"text\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "      <td align=\"center\" colspan=\"2\"><input type=\"submit\" value=\"Restore Cnfg#\" name=\"rstr_crs\" " << VCC_UTIL_acc_cntrl << ">" << std::endl;
  *out << "          <input align=\"right\" maxwidth=\"20\" size=\"2\" value=\"" << VCC_UTIL_CR_rstr_cnum << "\" name=\"rstr_cnum\" type=\"text\"> &nbsp to Current </td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td align=\"right\" colspan=\"2\"><input name=\"wrt_ser\" type=\"submit\" value=\"Write Serial Num.\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "      <td align=\"center\"><input align=\"right\" value=\"" << VCC_UTIL_CR_wrt_[6] << "\" size=\"6\" maxwidth=\"30\" name=\"wrt_ser_val\" type=\"text\"></td>" << std::endl;
  *out << "      <td align=\"center\" colspan=\"2\"><input type=\"submit\" value=\"Set Default Cnfg#\" name=\"set_dflt\" " << VCC_UTIL_acc_cntrl << ">" << std::endl;
  *out << "        <input align=\"right\" maxwidth=\"20\" size=\"2\" value=\"" << VCC_UTIL_CR_dflt_cnum << "\" name=\"dflt_cnum\" type=\"text\"></td>" << std::endl;
  *out << "      <td align=\"center\" colspan=\"2\"><input type=\"submit\" value=\"Read Curr Dflt\" name=\"rd_dflt\"> &nbsp : &nbsp " << VCC_UTIL_CR_curr_dflt << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td colspan=\"3\"></td>" << std::endl;
  *out << "      <td align=\"center\" colspan=\"4\"><input type=\"submit\" value=\"Read Serial Number\" name=\"rd_ser_num\"> &nbsp : &nbsp " << VCC_UTIL_CR_ser_num << "</td>" << std::endl;
  *out << "    </tr>" << std::endl;
  *out << "  </table>" << std::endl;
  *out << "</div>" << std::endl;
  *out << "</fieldset>" << std::endl;
  *out << "</form>" << std::endl;

  VCC_UTIL_curr_page = VCC_CNFG;
}

void EmuPeripheralCrateConfig::VCC_CNFG_DO(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
  char ctemp[256];
  emu::pc::CNFG_ptr rbk_cp;
  unsigned int temp;
  char *pch =(char *) &VCC_UTIL_CR_wrt_[0];
  emu::pc::Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();
    std::cout<<" entered VCC_CNFG_DO"<<std::endl;
    cgicc::Cgicc cgi(in);
    const cgicc::CgiEnvironment& env = cgi.getEnvironment();
    std::string guiStr = env.getQueryString() ;
    std::cout << guiStr << std::endl ;
    cgicc::form_iterator eth_cr_chk_name = cgi.getElement("eth_cr_chk");
    cgicc::form_iterator exfifo_cr_chk_name = cgi.getElement("exfifo_cr_chk");
    cgicc::form_iterator rstmsc_cr_chk_name = cgi.getElement("rstmsc_cr_chk");
    cgicc::form_iterator vme_cr_chk_name = cgi.getElement("vme_cr_chk");
    cgicc::form_iterator bto_chk_name = cgi.getElement("bto_chk");
    cgicc::form_iterator bgto_chk_name = cgi.getElement("bgto_chk");
    cgicc::form_iterator wrt_sel_crs_name = cgi.getElement("wrt_sel_crs");
    cgicc::form_iterator wrt_all_crs_name = cgi.getElement("wrt_all_crs");
    cgicc::form_iterator wrt_ser_name = cgi.getElement("wrt_ser");
    cgicc::form_iterator rbk_cur_crs_name = cgi.getElement("rbk_cur_crs");
    cgicc::form_iterator rbk_dflt_cnfg_name = cgi.getElement("rbk_dflt_cnfg");
    cgicc::form_iterator rbk_cnumA_name = cgi.getElement("rbk_cnumA");
    cgicc::form_iterator rbk_cnumB_name = cgi.getElement("rbk_cnumB");
    cgicc::form_iterator sav_crs_name = cgi.getElement("sav_crs");
    cgicc::form_iterator rstr_crs_name = cgi.getElement("rstr_crs");
    cgicc::form_iterator set_dflt_name = cgi.getElement("set_dflt");
    cgicc::form_iterator rd_dflt_name = cgi.getElement("rd_dflt");
    cgicc::form_iterator rd_ser_num_name = cgi.getElement("rd_ser_num");
    VCC_UTIL_CR_wrt_[0] = cgi["eth_cr_val"]->getValue();
    temp = strtoul(VCC_UTIL_CR_wrt_[0].c_str(),&pch,16);
    VCC_UTIL_CR_to.ether = (unsigned short int) temp;
    VCC_UTIL_CR_wrt_[1] = cgi["exfifo_cr_val"]->getValue();
    temp = strtoul(VCC_UTIL_CR_wrt_[1].c_str(),&pch,16);
    VCC_UTIL_CR_to.ext_fifo = (unsigned short int) temp;
    VCC_UTIL_CR_wrt_[2] = cgi["rstmsc_cr_val"]->getValue();
    temp = strtoul(VCC_UTIL_CR_wrt_[2].c_str(),&pch,16);
    VCC_UTIL_CR_to.rst_misc = (unsigned short int) temp;
    VCC_UTIL_CR_wrt_[3] = cgi["vme_cr_val"]->getValue();
    temp = strtoul(VCC_UTIL_CR_wrt_[3].c_str(),&pch,16);
    VCC_UTIL_CR_to.vme = (unsigned int) temp;
    VCC_UTIL_CR_wrt_[4] = cgi["bto_val"]->getValue();
    temp = strtoul(VCC_UTIL_CR_wrt_[4].c_str(),&pch,16);
    VCC_UTIL_CR_to.vme_bto = (unsigned short int) temp;
    VCC_UTIL_CR_wrt_[5] = cgi["bgto_val"]->getValue();
    temp = strtoul(VCC_UTIL_CR_wrt_[5].c_str(),&pch,16);
    VCC_UTIL_CR_to.vme_bgto = (unsigned short int) temp;

    std::cout << "Values to send to controller are:" << std::endl;
    vmecc->print_crs(&VCC_UTIL_CR_to);
    if(wrt_sel_crs_name != cgi.getElements().end()) {
      if(eth_cr_chk_name != cgi.getElements().end()) {
        vmecc->wrt_crs(0,&VCC_UTIL_CR_to);
      }
      if(exfifo_cr_chk_name != cgi.getElements().end()) {
        vmecc->wrt_crs(1,&VCC_UTIL_CR_to);
      }
      if(rstmsc_cr_chk_name != cgi.getElements().end()) {
        vmecc->wrt_crs(2,&VCC_UTIL_CR_to);
      }
      if(vme_cr_chk_name != cgi.getElements().end()) {
        vmecc->wrt_crs(3,&VCC_UTIL_CR_to);
      }
      if(bto_chk_name != cgi.getElements().end()) {
        vmecc->wrt_crs(4,&VCC_UTIL_CR_to);
      }
      if(bgto_chk_name != cgi.getElements().end()) {
        vmecc->wrt_crs(5,&VCC_UTIL_CR_to);
      }
    }
    if(wrt_all_crs_name != cgi.getElements().end()) {
      vmecc->wrt_crs(8,&VCC_UTIL_CR_to);
    }
    if(wrt_ser_name != cgi.getElements().end()) {
      VCC_UTIL_CR_wrt_[6] = cgi["wrt_ser_val"]->getValue();
      int wrt_sn = cgi["wrt_ser_val"]->getIntegerValue();
      vmecc->wrt_ser_num(wrt_sn);
    }
    if(rbk_cur_crs_name != cgi.getElements().end()) {
      rbk_cp = vmecc->read_crs();
      vmecc->print_crs(rbk_cp);
      sprintf(ctemp,"%04X",rbk_cp->ether);
      VCC_UTIL_CR_cur_[0]=ctemp;
      sprintf(ctemp,"%04X",rbk_cp->ext_fifo);
      VCC_UTIL_CR_cur_[1]=ctemp;
      sprintf(ctemp,"%04X",rbk_cp->rst_misc);
      VCC_UTIL_CR_cur_[2]=ctemp;
      sprintf(ctemp,"%08X",rbk_cp->vme);
      VCC_UTIL_CR_cur_[3]=ctemp;
      sprintf(ctemp,"%04X",rbk_cp->vme_bto);
      VCC_UTIL_CR_cur_[4]=ctemp;
      sprintf(ctemp,"%04X",rbk_cp->vme_bgto);
      VCC_UTIL_CR_cur_[5]=ctemp;
      free(rbk_cp);
    }
    if(rbk_dflt_cnfg_name != cgi.getElements().end()) {
      int cdflt = vmecc->read_cnfg_dflt();
      if(cdflt >= 0){
        sprintf(ctemp,"%d",cdflt);
        VCC_UTIL_CR_curr_dflt = ctemp;
        rbk_cp = vmecc->read_cnfg_num_dcd(cdflt);
        vmecc->print_crs(rbk_cp);
        sprintf(ctemp,"%04X",rbk_cp->ether);
        VCC_UTIL_CR_dflt_[0]=ctemp;
        sprintf(ctemp,"%04X",rbk_cp->ext_fifo);
        VCC_UTIL_CR_dflt_[1]=ctemp;
        sprintf(ctemp,"%04X",rbk_cp->rst_misc);
        VCC_UTIL_CR_dflt_[2]=ctemp;
        sprintf(ctemp,"%08X",rbk_cp->vme);
        VCC_UTIL_CR_dflt_[3]=ctemp;
        sprintf(ctemp,"%04X",rbk_cp->vme_bto);
        VCC_UTIL_CR_dflt_[4]=ctemp;
        sprintf(ctemp,"%04X",rbk_cp->vme_bgto);
        VCC_UTIL_CR_dflt_[5]=ctemp;
        free(rbk_cp);
      }
      else {
	std::cout << "Error while reading default config. number" << std::endl;
        VCC_UTIL_CR_curr_dflt = "-";
      }
    }
    if(rbk_cnumA_name != cgi.getElements().end()) {
      VCC_UTIL_CR_cnumA = cgi["cnumA"]->getValue();
      int cnumA = cgi["cnumA"]->getIntegerValue();
      rbk_cp = vmecc->read_cnfg_num_dcd(cnumA);
      sprintf(ctemp,"%04X",rbk_cp->ether);
      VCC_UTIL_CR_flashA_[0]=ctemp;
      sprintf(ctemp,"%04X",rbk_cp->ext_fifo);
      VCC_UTIL_CR_flashA_[1]=ctemp;
      sprintf(ctemp,"%04X",rbk_cp->rst_misc);
      VCC_UTIL_CR_flashA_[2]=ctemp;
      sprintf(ctemp,"%08X",rbk_cp->vme);
      VCC_UTIL_CR_flashA_[3]=ctemp;
      sprintf(ctemp,"%04X",rbk_cp->vme_bto);
      VCC_UTIL_CR_flashA_[4]=ctemp;
      sprintf(ctemp,"%04X",rbk_cp->vme_bgto);
      VCC_UTIL_CR_flashA_[5]=ctemp;
      vmecc->print_crs(rbk_cp);
      free(rbk_cp);
    }
    if(rbk_cnumB_name != cgi.getElements().end()) {
      VCC_UTIL_CR_cnumB = cgi["cnumB"]->getValue();
      int cnumB = cgi["cnumB"]->getIntegerValue();
      rbk_cp = vmecc->read_cnfg_num_dcd(cnumB);
      sprintf(ctemp,"%04X",rbk_cp->ether);
      VCC_UTIL_CR_flashB_[0]=ctemp;
      sprintf(ctemp,"%04X",rbk_cp->ext_fifo);
      VCC_UTIL_CR_flashB_[1]=ctemp;
      sprintf(ctemp,"%04X",rbk_cp->rst_misc);
      VCC_UTIL_CR_flashB_[2]=ctemp;
      sprintf(ctemp,"%08X",rbk_cp->vme);
      VCC_UTIL_CR_flashB_[3]=ctemp;
      sprintf(ctemp,"%04X",rbk_cp->vme_bto);
      VCC_UTIL_CR_flashB_[4]=ctemp;
      sprintf(ctemp,"%04X",rbk_cp->vme_bgto);
      VCC_UTIL_CR_flashB_[5]=ctemp;
      vmecc->print_crs(rbk_cp);
      free(rbk_cp);
    }
    if(sav_crs_name != cgi.getElements().end()) {
      VCC_UTIL_CR_sav_cnum = cgi["sav_cnum"]->getValue();
      int sav_cnum = cgi["sav_cnum"]->getIntegerValue();
      vmecc->save_cnfg_num(sav_cnum);
    }
    if(rstr_crs_name != cgi.getElements().end()) {
      VCC_UTIL_CR_rstr_cnum = cgi["rstr_cnum"]->getValue();
      int rstr_cnum = cgi["rstr_cnum"]->getIntegerValue();
      vmecc->rstr_cnfg_num(rstr_cnum);
    }
    if(set_dflt_name != cgi.getElements().end()) {
      VCC_UTIL_CR_dflt_cnum = cgi["dflt_cnum"]->getValue();
      int dflt_cnum = cgi["dflt_cnum"]->getIntegerValue();
      vmecc->set_cnfg_dflt(dflt_cnum);
    }
    if(rd_dflt_name != cgi.getElements().end()) {
      int cdflt = vmecc->read_cnfg_dflt();
      if(cdflt >= 0){
        sprintf(ctemp,"%d",cdflt);
        VCC_UTIL_CR_curr_dflt = ctemp;
      }
      else {
	std::cout << "Error while reading default config. number" << std::endl;
        VCC_UTIL_CR_curr_dflt = "-";
      }
    }
    if(rd_ser_num_name != cgi.getElements().end()) {
      emu::pc::SN_t sn = vmecc->rd_ser_num();
      if(sn.status == 0){
        sprintf(ctemp,"VCC%02d",sn.sn);
        VCC_UTIL_CR_ser_num = ctemp;
      }
      else {
	std::cout << "Error while reading serial number. Status=" << sn.status << "  Error type = " << std::hex << sn.err_typ << std::endl;
        VCC_UTIL_CR_ser_num = "-";
      }
    }
    this->VMECCGUI_cnfg_utils(in,out);
}

void EmuPeripheralCrateConfig::VMECCGUI_MAC_utils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  static bool first = true;
  std::string ena_dis;
  emu::pc::Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  if(first){
    first = false;
    VCC_UTIL_MAC_wrt_dev="02-00-00-00-00-3B";
    VCC_UTIL_MAC_wrt_mcast1="FF-FF-FF-FF-FF-FE";
    VCC_UTIL_MAC_wrt_mcast2="FF-FF-FF-FF-FF-FD";
    VCC_UTIL_MAC_wrt_mcast3="FF-FF-FF-FF-FF-FC";
    VCC_UTIL_MAC_wrt_dflt="00-0D-88-B2-65-21";
    VCC_UTIL_MAC_rbk_dev="Not yet read back";
    VCC_UTIL_MAC_rbk_mcast1="Not yet read back";
    VCC_UTIL_MAC_rbk_mcast2="Not yet read back";
    VCC_UTIL_MAC_rbk_mcast3="Not yet read back";
    VCC_UTIL_MAC_rbk_dflt="Not yet read back";
    VCC_UTIL_MAC_ena_dis="disabled";
  }

  char title[] = "VCC Utilities: MAC Addresses";
  char pbuf[300];
  sprintf(pbuf,"%s<br>Current Crate is %s<br>MAC Addr: %02x-%02x-%02x-%02x-%02x-%02x",title,(lccc->GetLabel()).c_str(),lccc->vmeController()->GetDestMAC(0),lccc->vmeController()->GetDestMAC(1),lccc->vmeController()->GetDestMAC(2),lccc->vmeController()->GetDestMAC(3),lccc->vmeController()->GetDestMAC(4),lccc->vmeController()->GetDestMAC(5));
  //
  VCCHeader(in,out,title,pbuf);
  //

  this->VMECC_UTIL_Menu_Buttons(in,out);

   std::string VCC_MAC_DO =
     toolbox::toString("/%s/VCC_MAC_DO",getApplicationDescriptor()->getURN().c_str());
   if(VCC_UTIL_MAC_ena_dis=="disabled"){
     ena_dis = "Ena";
   }
   else {
     ena_dis = "Dis";
   }

  *out << "<form action=\"" << VCC_MAC_DO << "\" method=\"GET\">" << std::endl;
  *out << "<fieldset><legend style=\"font-size: 18pt;\" align=\"center\">MAC Addresses</legend>" << std::endl;
  *out << "<div align=\"center\">" << std::endl;
  *out << "  <table border=\"3\" cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << " rules=\"groups\">" << std::endl;
  *out << "    <colgroup span=\"3\"><colgroup span=\"1\">" << std::endl;
  *out << "    <tr>" << std::endl;
  *out << "      <th colspan=\"3\" style=\"font-size: 14pt; color: blue\">Inputs</th>" << std::endl;
  *out << "      <th style=\"font-size: 14pt; color: blue\">Read Backs</th>" << std::endl;
  *out << "    </tr><tr>" << std::endl; 
  *out << "      <td><input name=\"mac_choice\" type=\"radio\" value=\"dev_mac\"" << VCC_UTIL_MAC_ena_dis << "></td>" << std::endl;
  *out << "      <td> Device MAC </td>" << std::endl;
  *out << "      <td><input align=\"left\" value=\"" << VCC_UTIL_MAC_wrt_dev << "\" size=\"17\" maxwidth=\"170\" name=\"dev_mac_addr\" type=\"text\"></td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_MAC_rbk_dev << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td><input name=\"mac_choice\" type=\"radio\" value=\"mlt1_mac\"></td>" << std::endl;
  *out << "      <td> MultiCast 1 MAC </td>" << std::endl;
  *out << "      <td><input align=\"left\" value=\"" << VCC_UTIL_MAC_wrt_mcast1 << "\" size=\"17\" maxwidth=\"170\" name=\"mlt1_mac_addr\" type=\"text\"></td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_MAC_rbk_mcast1 << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td><input name=\"mac_choice\" type=\"radio\" value=\"mlt2_mac\"></td>" << std::endl;
  *out << "      <td> MultiCast 3 MAC </td>" << std::endl;
  *out << "      <td><input align=\"left\" value=\"" << VCC_UTIL_MAC_wrt_mcast2 << "\" size=\"17\" maxwidth=\"170\" name=\"mlt2_mac_addr\" type=\"text\"></td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_MAC_rbk_mcast2 << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td><input name=\"mac_choice\" type=\"radio\" value=\"mlt3_mac\"></td>" << std::endl;
  *out << "      <td> MultiCast 3 MAC </td>" << std::endl;
  *out << "      <td><input align=\"left\" value=\"" << VCC_UTIL_MAC_wrt_mcast3 << "\" size=\"17\" maxwidth=\"170\" name=\"mlt3_mac_addr\" type=\"text\"></td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_MAC_rbk_mcast3 << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td><input name=\"mac_choice\" type=\"radio\" value=\"dflt_mac\"></td>" << std::endl;
  *out << "      <td> Default Server MAC </td>" << std::endl;
  *out << "      <td><input align=\"left\" value=\"" << VCC_UTIL_MAC_wrt_dflt << "\" size=\"17\" maxwidth=\"170\" name=\"dflt_mac_addr\" type=\"text\"></td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_MAC_rbk_dflt << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td><input name=\"mac_choice\" type=\"radio\" value=\"all_mac\"" << VCC_UTIL_MAC_ena_dis << "></td>" << std::endl;
  *out << "      <td> All MACs </td>" << std::endl;
  *out << "      <td></td>" << std::endl;
  *out << "      <td></td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td></td>" << std::endl;
  *out << "      <td><input name=\"rst_macs\" type=\"reset\" value=\"Reset\"> &nbsp &nbsp <input name=\"ena_dev_mac\" type=\"submit\" value=\"" << ena_dis << "\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "      <td><input name=\"wrt_macs\" type=\"submit\" value=\"Write\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "      <td align=\"center\"><input type=\"submit\" value=\"Read\" name=\"rd_MAC_dcd\"></td>" << std::endl;
  *out << "    </tr>" << std::endl;
  *out << "  </table>" << std::endl;
  *out << "</div>" << std::endl;
  *out << "</fieldset>" << std::endl;
  *out << "</form>" << std::endl;

  VCC_UTIL_curr_page = VCC_MAC;
}

void EmuPeripheralCrateConfig::VCC_MAC_DO(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
  char ctemp[256];
  emu::pc::CNFG_ptr rbk_cp;
  emu::pc::CNFG_t wrt_mac;
  std::string stemp;
  unsigned int temp[6];

  emu::pc::Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();

    std::cout<<" entered VCC_MAC_DO"<<std::endl;
    cgicc::Cgicc cgi(in);
    const cgicc::CgiEnvironment& env = cgi.getEnvironment();
    std::string guiStr = env.getQueryString() ;
    std::cout << guiStr << std::endl ;

    cgicc::form_iterator ena_dev_mac = cgi.getElement("ena_dev_mac");
    cgicc::form_iterator wrt_macs = cgi.getElement("wrt_macs");
    cgicc::form_iterator which_mac = cgi.getElement("mac_choice");
    cgicc::form_iterator rd_macs = cgi.getElement("rd_MAC_dcd");
    if(ena_dev_mac != cgi.getElements().end()) {
      stemp = cgi["ena_dev_mac"]->getValue();
      if(stemp == "Ena"){
        VCC_UTIL_MAC_ena_dis = "";
      }
      else {
	VCC_UTIL_MAC_ena_dis = "disabled";
      }
    }
    if(wrt_macs != cgi.getElements().end()) {
      if(which_mac != cgi.getElements().end()) {
        stemp = cgi["mac_choice"]->getValue();
        std::cout << "Chosen MAC " << stemp.c_str() << std::endl;
        if(stemp == "dev_mac"){
          VCC_UTIL_MAC_wrt_dev = cgi["dev_mac_addr"]->getValue();
          sscanf(VCC_UTIL_MAC_wrt_dev.c_str(),"%2x-%2x-%2x-%2x-%2x-%2x",&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
          for(int i=0;i<6;i++)wrt_mac.mac.device[i] = (unsigned char)(temp[i]&0xFF);
          vmecc->set_macs(0,&wrt_mac);
        }
        if(stemp == "mlt1_mac"){
          VCC_UTIL_MAC_wrt_mcast1 = cgi["mlt1_mac_addr"]->getValue();
          sscanf(VCC_UTIL_MAC_wrt_mcast1.c_str(),"%2x-%2x-%2x-%2x-%2x-%2x",&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
          for(int i=0;i<6;i++)wrt_mac.mac.mcast1[i] = (unsigned char)(temp[i]&0xFF);
          vmecc->set_macs(1,&wrt_mac);
        }
        if(stemp == "mlt2_mac"){
          VCC_UTIL_MAC_wrt_mcast2 = cgi["mlt2_mac_addr"]->getValue();
          sscanf(VCC_UTIL_MAC_wrt_mcast2.c_str(),"%2x-%2x-%2x-%2x-%2x-%2x",&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
          for(int i=0;i<6;i++)wrt_mac.mac.mcast2[i] = (unsigned char)(temp[i]&0xFF);
          vmecc->set_macs(2,&wrt_mac);
        }
        if(stemp == "mlt3_mac"){
          VCC_UTIL_MAC_wrt_mcast3 = cgi["mlt3_mac_addr"]->getValue();
          sscanf(VCC_UTIL_MAC_wrt_mcast3.c_str(),"%2x-%2x-%2x-%2x-%2x-%2x",&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
          for(int i=0;i<6;i++)wrt_mac.mac.mcast3[i] = (unsigned char)(temp[i]&0xFF);
          vmecc->set_macs(3,&wrt_mac);
        }
        if(stemp == "dflt_mac"){
          VCC_UTIL_MAC_wrt_dflt = cgi["dflt_mac_addr"]->getValue();
          sscanf(VCC_UTIL_MAC_wrt_dflt.c_str(),"%2x-%2x-%2x-%2x-%2x-%2x",&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
          for(int i=0;i<6;i++)wrt_mac.mac.dflt_srv[i] = (unsigned char)(temp[i]&0xFF);
          vmecc->set_macs(4,&wrt_mac);
        }
        if(stemp == "all_mac"){
          VCC_UTIL_MAC_wrt_dev = cgi["dev_mac_addr"]->getValue();
          sscanf(VCC_UTIL_MAC_wrt_dev.c_str(),"%2x-%2x-%2x-%2x-%2x-%2x",&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
          for(int i=0;i<6;i++)wrt_mac.mac.device[i] = (unsigned char)(temp[i]&0xFF);
          VCC_UTIL_MAC_wrt_mcast1 = cgi["mlt1_mac_addr"]->getValue();
          sscanf(VCC_UTIL_MAC_wrt_mcast1.c_str(),"%2x-%2x-%2x-%2x-%2x-%2x",&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
          for(int i=0;i<6;i++)wrt_mac.mac.mcast1[i] = (unsigned char)(temp[i]&0xFF);
          VCC_UTIL_MAC_wrt_mcast2 = cgi["mlt2_mac_addr"]->getValue();
          sscanf(VCC_UTIL_MAC_wrt_mcast2.c_str(),"%2x-%2x-%2x-%2x-%2x-%2x",&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
          for(int i=0;i<6;i++)wrt_mac.mac.mcast2[i] = (unsigned char)(temp[i]&0xFF);
          VCC_UTIL_MAC_wrt_mcast3 = cgi["mlt3_mac_addr"]->getValue();
          sscanf(VCC_UTIL_MAC_wrt_mcast3.c_str(),"%2x-%2x-%2x-%2x-%2x-%2x",&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
          for(int i=0;i<6;i++)wrt_mac.mac.mcast3[i] = (unsigned char)(temp[i]&0xFF);
          VCC_UTIL_MAC_wrt_dflt = cgi["dflt_mac_addr"]->getValue();
          sscanf(VCC_UTIL_MAC_wrt_dflt.c_str(),"%2x-%2x-%2x-%2x-%2x-%2x",&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
          for(int i=0;i<6;i++)wrt_mac.mac.dflt_srv[i] = (unsigned char)(temp[i]&0xFF);
          vmecc->set_macs(8,&wrt_mac);
        }
        VCC_UTIL_MAC_ena_dis = "disabled";
      }
    }
    if(rd_macs != cgi.getElements().end()) {
      rbk_cp = vmecc->read_macs_dcd();
      sprintf(ctemp,"%02X-%02X-%02X-%02X-%02X-%02X",rbk_cp->mac.device[0],rbk_cp->mac.device[1],rbk_cp->mac.device[2],rbk_cp->mac.device[3],rbk_cp->mac.device[4],rbk_cp->mac.device[5]);
      VCC_UTIL_MAC_rbk_dev = ctemp;
      sprintf(ctemp,"%02X-%02X-%02X-%02X-%02X-%02X",rbk_cp->mac.mcast1[0],rbk_cp->mac.mcast1[1],rbk_cp->mac.mcast1[2],rbk_cp->mac.mcast1[3],rbk_cp->mac.mcast1[4],rbk_cp->mac.mcast1[5]);
      VCC_UTIL_MAC_rbk_mcast1 = ctemp;
      sprintf(ctemp,"%02X-%02X-%02X-%02X-%02X-%02X",rbk_cp->mac.mcast2[0],rbk_cp->mac.mcast2[1],rbk_cp->mac.mcast2[2],rbk_cp->mac.mcast2[3],rbk_cp->mac.mcast2[4],rbk_cp->mac.mcast2[5]);
      VCC_UTIL_MAC_rbk_mcast2 = ctemp;
      sprintf(ctemp,"%02X-%02X-%02X-%02X-%02X-%02X",rbk_cp->mac.mcast3[0],rbk_cp->mac.mcast3[1],rbk_cp->mac.mcast3[2],rbk_cp->mac.mcast3[3],rbk_cp->mac.mcast3[4],rbk_cp->mac.mcast3[5]);
      VCC_UTIL_MAC_rbk_mcast3 = ctemp;
      sprintf(ctemp,"%02X-%02X-%02X-%02X-%02X-%02X",rbk_cp->mac.dflt_srv[0],rbk_cp->mac.dflt_srv[1],rbk_cp->mac.dflt_srv[2],rbk_cp->mac.dflt_srv[3],rbk_cp->mac.dflt_srv[4],rbk_cp->mac.dflt_srv[5]);
      VCC_UTIL_MAC_rbk_dflt = ctemp;
      vmecc->print_macs(rbk_cp);
      free(rbk_cp);
    }
    this->VMECCGUI_MAC_utils(in,out);
}

void EmuPeripheralCrateConfig::VMECCGUI_FIFO_utils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  emu::pc::CNFG_ptr rbk_cp;
  static bool first = true;
  emu::pc::Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();
  if(first){
    first = false;
    rbk_cp = vmecc->read_crs();
    if((rbk_cp->ext_fifo & FIFO_CR_TST)== FIFO_CR_TST){
      VCC_UTIL_FIFO_mode="Test";
    }
    else {
      VCC_UTIL_FIFO_mode="VME";
    }
    if((rbk_cp->ext_fifo & FIFO_CR_ECC)== FIFO_CR_ECC){
      VCC_UTIL_FIFO_ecc="Ena";
    }
    else {
      VCC_UTIL_FIFO_ecc="Dis";
    }
    if((rbk_cp->ext_fifo & FIFO_CR_INJ)== FIFO_CR_INJ){
      VCC_UTIL_FIFO_inj="On";
    }
    else {
      VCC_UTIL_FIFO_inj="Off";
    }
    VCC_UTIL_FIFO_wrt_pae="127";
    VCC_UTIL_FIFO_wrt_paf="127";
    VCC_UTIL_FIFO_rbk_pae="- - -";
    VCC_UTIL_FIFO_rbk_paf="- - -";
    VCC_UTIL_FIFO_cor_errs="- - -";
    VCC_UTIL_FIFO_uncor_errs="- - -";
    VCC_UTIL_FIFO_wrt_data="Enter\n16 bit\nHex Data";
    VCC_UTIL_FIFO_rbk_data="";
    VCC_UTIL_FIFO_msg_data="";
    VCC_UTIL_FIFO_rd_num="1";
    free(rbk_cp);
  }

  char title[] = "VCC Utilities: External FIFO";
  char pbuf[300];
  sprintf(pbuf,"%s<br>Current Crate is %s<br>MAC Addr: %02x-%02x-%02x-%02x-%02x-%02x",title,(lccc->GetLabel()).c_str(),lccc->vmeController()->GetDestMAC(0),lccc->vmeController()->GetDestMAC(1),lccc->vmeController()->GetDestMAC(2),lccc->vmeController()->GetDestMAC(3),lccc->vmeController()->GetDestMAC(4),lccc->vmeController()->GetDestMAC(5));
  //
  VCCHeader(in,out,title,pbuf);
  //

  this->VMECC_UTIL_Menu_Buttons(in,out);

   std::string VCC_FIFO_DO =
     toolbox::toString("/%s/VCC_FIFO_DO",getApplicationDescriptor()->getURN().c_str());

  *out << "<form action=\"" << VCC_FIFO_DO << "\" method=\"GET\">" << std::endl;
  *out << "<fieldset><legend style=\"font-size: 18pt;\" align=\"center\">External FIFO</legend>" << std::endl;
  *out << "<div align=\"center\">" << std::endl;
  *out << "  <table border=\"3\" cellspacing=\"2\" cellpadding=\"0\" bgcolor=" << VCC_UTIL_curr_color << " style=\"border-collapse: collapse\">" << std::endl;
  //  *out << "    <tr><td rowspan=\"2\" valign=\"top\">" << std::endl;
  *out << "    <tr><td valign=\"top\">" << std::endl;
  *out << "      <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Modes and Resets</legend>" << std::endl;
  *out << "         <table cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "           <tr>" << std::endl;
  *out << "             <td><input name=\"fifo_mode\" type=\"submit\" value=\"FIFO Mode\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "             <td>" << VCC_UTIL_FIFO_mode << "</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"err_inj\" type=\"submit\" value=\"Error Inject\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "             <td>" << VCC_UTIL_FIFO_inj << "</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"ecc_state\" type=\"submit\" value=\"ECC\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "             <td>" << VCC_UTIL_FIFO_ecc << "</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"prst\" type=\"submit\" value=\"Partial RST\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "             <td><input name=\"mrst\" type=\"submit\" value=\"Master RST\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"set_mrk\" type=\"submit\" value=\"Set Mark\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "             <td><input name=\"rst_mark\" type=\"submit\" value=\"Reset Mark\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "           </tr>" << std::endl;
  *out << "         </table>" << std::endl;
  *out << "      </fieldset>" << std::endl;
  *out << "    </td><td valign=\"top\">" << std::endl;
  *out << "      <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Programmable Offsets</legend>" << std::endl;
  *out << "         <table cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "           <tr>" << std::endl;
  *out << "             <td align=\"center\">PAE</td>" << std::endl;
  *out << "             <td align=\"center\">PAF</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td align=\"center\"><input align=\"right\" value=\"" << VCC_UTIL_FIFO_wrt_pae << "\" size=\"7\" maxwidth=\"70\" name=\"pae_off_val\" type=\"text\"></td>" << std::endl;
  *out << "             <td align=\"center\"><input align=\"right\" value=\"" << VCC_UTIL_FIFO_wrt_paf << "\" size=\"7\" maxwidth=\"70\" name=\"paf_off_val\" type=\"text\"></td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td align=\"center\">" << VCC_UTIL_FIFO_rbk_pae << "</td>" << std::endl;
  *out << "             <td align=\"center\">" << VCC_UTIL_FIFO_rbk_paf << "</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"rd_offs\" type=\"submit\" value=\"Read Offsets\"></td>" << std::endl;
  *out << "             <td><input name=\"wrt_offs\" type=\"submit\" value=\"Write Offsets\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "           </tr>" << std::endl;
  *out << "         </table>" << std::endl;
  *out << "      </fieldset>" << std::endl;
  *out << "    </td></tr>" << std::endl;
  //  *out << "    <tr><td rowspan=\"2\" valign=\"top\">" << std::endl;
  *out << "    <tr><td valign=\"top\">" << std::endl;
  *out << "      <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Data to/from FIFO</legend>" << std::endl;
  *out << "         <table cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "           <tr>" << std::endl;
  *out << "             <td rowspan=\"2\"><textarea name=\"fifo_data\" rows=\"3\" cols=\"12\">" << VCC_UTIL_FIFO_wrt_data << "</textarea></td>" << std::endl;
  *out << "             <td><input name=\"wrt_dat_fifo\" type=\"submit\" value=\"Write FIFO\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"rd_dat_fifo\" type=\"submit\" value=\"Read\" " << VCC_UTIL_acc_cntrl << ">" << std::endl;
  *out << "                 <input align=\"right\" value=\"" << VCC_UTIL_FIFO_rd_num << "\" size=\"3\" maxwidth=\"30\" name=\"rd_num\" type=\"text\"> Words</td>" << std::endl;
  *out << "           </tr>" << std::endl;
  *out << "         </table>" << std::endl;
  *out << "      </fieldset>" << std::endl;
  *out << "    </td><td valign=\"top\">" << std::endl;
  *out << "      <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Error Counts</legend>" << std::endl;
  *out << "         <table cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "           <tr>" << std::endl;
  *out << "             <td align=\"center\">Corrected</td>" << std::endl;
  *out << "             <td align=\"center\">UnCorrected</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td align=\"center\">" << VCC_UTIL_FIFO_cor_errs << "</td>" << std::endl;
  *out << "             <td align=\"center\">" << VCC_UTIL_FIFO_uncor_errs << "</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"rd_errs\" type=\"submit\" value=\"Read Counters\"></td>" << std::endl;
  *out << "             <td><input name=\"rst_errs\" type=\"submit\" value=\"Reset Counters\"></td>" << std::endl;
  *out << "           </tr>" << std::endl;
  *out << "         </table>" << std::endl;
  *out << "      </fieldset>" << std::endl;
  *out << "    </td></tr>" << std::endl;
  *out << "    <tr>" << std::endl;
  *out << "      <td valign=\"top\" align = \"center\">" << std::endl;
  *out << "        <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Readback Data</legend>" << std::endl;
  *out << "          <pre><p>" << VCC_UTIL_FIFO_rbk_data << "</p></pre>" << std::endl;
  *out << "        </fieldset>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "      <td valign=\"top\" align = \"center\">" << std::endl;
  *out << "        <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Messages</legend>" << std::endl;
  *out << "          <pre><p>" << VCC_UTIL_FIFO_msg_data << "</p></pre></td>" << std::endl;
  *out << "        </fieldset>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "    </tr>" << std::endl;
  *out << "  </table>" << std::endl;
  *out << "</div>" << std::endl;
  *out << "</fieldset>" << std::endl;
  *out << "</form>" << std::endl;

  VCC_UTIL_curr_page = VCC_FIFO;
}

void EmuPeripheralCrateConfig::VCC_FIFO_DO(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
  unsigned int cor,uncor,full_off,mt_off;
  unsigned int itemp;
  unsigned short int *pbuf;
  int i,nwrds,status,pkt_type;
  char ctemp[256];
  char *ptemp1,*ptemp2,*ptemp3;
  char *pch = (char *) &VCC_UTIL_FIFO_wrt_data;
  std::string stemp;

  emu::pc::Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();

    std::cout<<" entered VCC_FIFO_DO"<<std::endl;
    cgicc::Cgicc cgi(in);
    const cgicc::CgiEnvironment& env = cgi.getEnvironment();
    std::string guiStr = env.getQueryString() ;
    std::cout << guiStr << std::endl ;

    cgicc::form_iterator fifo_mode = cgi.getElement("fifo_mode");
    cgicc::form_iterator err_inj = cgi.getElement("err_inj");
    cgicc::form_iterator ecc_state = cgi.getElement("ecc_state");
    cgicc::form_iterator prst = cgi.getElement("prst");
    cgicc::form_iterator mrst = cgi.getElement("mrst");
    cgicc::form_iterator set_mark = cgi.getElement("set_mark");
    cgicc::form_iterator rst_mark = cgi.getElement("rst_mark");
    cgicc::form_iterator rd_errs = cgi.getElement("rd_errs");
    cgicc::form_iterator rst_errs = cgi.getElement("rst_errs");
    cgicc::form_iterator rd_offs = cgi.getElement("rd_offs");
    cgicc::form_iterator wrt_offs = cgi.getElement("wrt_offs");
    cgicc::form_iterator wrt_dat_fifo = cgi.getElement("wrt_dat_fifo");
    cgicc::form_iterator rd_dat_fifo = cgi.getElement("rd_dat_fifo");

    if(fifo_mode != cgi.getElements().end()) {
      if(VCC_UTIL_FIFO_mode == "Test"){
        vmecc->set_clr_bits(emu::pc::CLR, emu::pc::EXTFIFO, ~FIFO_CR_TST);
        VCC_UTIL_FIFO_mode = "VME";
      }
      else {
        vmecc->set_clr_bits(emu::pc::SET, emu::pc::EXTFIFO, FIFO_CR_TST);
        VCC_UTIL_FIFO_mode = "Test";
      }
    }
    if(err_inj != cgi.getElements().end()) {
      if(VCC_UTIL_FIFO_inj == "On"){
        vmecc->set_clr_bits(emu::pc::CLR, emu::pc::EXTFIFO, ~FIFO_CR_INJ);
        VCC_UTIL_FIFO_inj = "Off";
      }
      else {
        vmecc->set_clr_bits(emu::pc::SET, emu::pc::EXTFIFO, FIFO_CR_INJ);
        VCC_UTIL_FIFO_inj = "On";
      }
    }
    if(ecc_state != cgi.getElements().end()) {
      if(VCC_UTIL_FIFO_ecc == "Ena"){
        vmecc->set_clr_bits(emu::pc::CLR, emu::pc::EXTFIFO, ~FIFO_CR_ECC);
        VCC_UTIL_FIFO_ecc = "Dis";
      }
      else {
        vmecc->set_clr_bits(emu::pc::SET, emu::pc::EXTFIFO, FIFO_CR_ECC);
        VCC_UTIL_FIFO_ecc = "Ena";
      }
    }
    if(prst != cgi.getElements().end()) {
      vmecc->prst_ff();
    }
    if(mrst != cgi.getElements().end()) {
      vmecc->mrst_ff();
    }
    if(set_mark != cgi.getElements().end()) {
      vmecc->set_mark();
    }
    if(rst_mark != cgi.getElements().end()) {
      vmecc->rst_mark();
    }
    if(rd_errs != cgi.getElements().end()) {
      status = vmecc->rd_err_cnts(&cor,&uncor);
      if(status == 0){
        std::cout << "uncorrected errors: " << uncor << std::endl;
        std::cout << "  corrected errors: " << cor << std::endl;
        sprintf(ctemp,"%d",cor);
        VCC_UTIL_FIFO_cor_errs=ctemp;
        sprintf(ctemp,"%d",uncor);
        VCC_UTIL_FIFO_uncor_errs=ctemp;
      }
      else {
        VCC_UTIL_FIFO_cor_errs="Bad status";
        VCC_UTIL_FIFO_uncor_errs="Bad status";
      }
    }
    if(rst_errs != cgi.getElements().end()) {
      vmecc->rst_err_cnt();
    }
    if(rd_offs != cgi.getElements().end()) {
      status = vmecc->rdbk_off(&full_off,&mt_off);
      if(status == 0){
        std::cout << "full offset: " << full_off << std::endl;
        std::cout << "  mt offset: " << mt_off << std::endl;
        sprintf(ctemp,"%d",full_off);
        VCC_UTIL_FIFO_rbk_paf=ctemp;
        sprintf(ctemp,"%d",mt_off);
        VCC_UTIL_FIFO_rbk_pae=ctemp;
      }
      else {
        VCC_UTIL_FIFO_rbk_paf="Bad Status";
        VCC_UTIL_FIFO_rbk_pae="Bad Status";
      }
    }
    if(wrt_offs != cgi.getElements().end()) {
      int paf_off = cgi["paf_off_val"]->getIntegerValue();
      int pae_off = cgi["pae_off_val"]->getIntegerValue();
      VCC_UTIL_FIFO_wrt_paf = cgi["paf_off_val"]->getValue();
      VCC_UTIL_FIFO_wrt_pae = cgi["pae_off_val"]->getValue();
      vmecc->prg_off((unsigned)paf_off,(unsigned)pae_off);
    }
    if(wrt_dat_fifo != cgi.getElements().end()) {
      VCC_UTIL_FIFO_wrt_data = cgi["fifo_data"]->getValue();
      size_t slen = VCC_UTIL_FIFO_wrt_data.length();
      ptemp1 = (char *) malloc(slen+1);
      VCC_UTIL_FIFO_wrt_data.copy(ptemp1,slen);
      ptemp1[slen]='\0';
      ptemp3=ptemp1;
      i=0;
      while((ptemp2 = strtok(ptemp3,"\n\r")) !=NULL){
        ptemp3 = 0;
        itemp = strtoul(ptemp2,&pch,16);
        vmecc->wbuf[2*(++i)]=(itemp>>8)&0xFF;
        vmecc->wbuf[2*i+1]=itemp&0xFF;
      }
      nwrds=i;
      free(ptemp1);
      std::cout << "FIFO Data: " << std::endl;
      for(i=1;i<=nwrds;i++){
        printf("%02X%02X\n",vmecc->wbuf[2*i]&0xFF,vmecc->wbuf[2*i+1]&0xFF);
      }
      vmecc->wrt_ff(nwrds);
    }
    if(rd_dat_fifo != cgi.getElements().end()) {
      VCC_UTIL_FIFO_msg_data = "";
      VCC_UTIL_FIFO_rbk_data = "";
      int nwrds = cgi["rd_num"]->getIntegerValue();
      VCC_UTIL_FIFO_rd_num = cgi["rd_num"]->getValue();
      pbuf = (unsigned short int *) malloc(2*nwrds);
      status = vmecc->rd_ff(nwrds,0,pbuf);
      if(status==0){
        VCC_UTIL_FIFO_msg_data = "Timed out trying to read";
      }
      else {
        if(status==-2){
          pkt_type = vmecc->rbuf[PKT_TYP_OFF]&0XFF;
	  sprintf(ctemp,"Unexpected Pkt Type: 0x%02X\n",pkt_type);
	  std::cout << ctemp << std::endl;
	  VCC_UTIL_FIFO_msg_data += ctemp;
	}
        else if (status==-1){
	  std::cout << vmecc->dcode_msg_pkt(vmecc->rbuf) << std::endl;
	  VCC_UTIL_FIFO_msg_data += vmecc->dcode_msg_pkt(vmecc->rbuf);
	  VCC_UTIL_FIFO_msg_data += "\n";
	}
        else {
          for(i=0;i<nwrds;i++){
	    sprintf(ctemp,"0x%04hX\n",pbuf[i]);
            VCC_UTIL_FIFO_rbk_data += ctemp;
	  }
	}
        while((pkt_type=vmecc->rd_pkt())>=0){
          if(pkt_type>emu::pc::INFO_PKT){
            std::cout << vmecc->dcode_msg_pkt(vmecc->rbuf) << std::endl;
            VCC_UTIL_FIFO_msg_data += vmecc->dcode_msg_pkt(vmecc->rbuf);
            VCC_UTIL_FIFO_msg_data += "\n";
	  }
          else if (pkt_type==emu::pc::EXFIFO_PKT){
            int nw = ((vmecc->rbuf[WRD_CNT_OFF]&0xff)<<8)|(vmecc->rbuf[WRD_CNT_OFF+1]&0xff);
	    for(i=0;i<nw;i++){
	      sprintf(ctemp,"0x%02X%02X\n",vmecc->rbuf[2*i+DATA_OFF]&0xFF,vmecc->rbuf[2*i+DATA_OFF+1]&0xFF);
	      VCC_UTIL_FIFO_rbk_data += ctemp;
	    }
	  }
          else {
            sprintf(ctemp,"Pkt Type: 0x%02X\n",pkt_type);
            std::cout << ctemp << std::endl;
            VCC_UTIL_FIFO_msg_data += ctemp;
	  }
	}
      }
      free(pbuf);
    }
    this->VMECCGUI_FIFO_utils(in,out);
}

void EmuPeripheralCrateConfig::VMECCGUI_pkt_send(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  static bool first = true;
  emu::pc::Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  if(first){
    first = false;
    VCC_UTIL_PKTSND_prcs_tag = "0";
    VCC_UTIL_PKTSND_cmnd = "00";
    VCC_UTIL_PKTSND_data = "Enter\n16 bit\nHex Data\n";
  }

  char title[] = "VCC Utilities: Packet Send Commands";
  char pbuf[300];
  sprintf(pbuf,"%s<br>Current Crate is %s<br>MAC Addr: %02x-%02x-%02x-%02x-%02x-%02x",title,(lccc->GetLabel()).c_str(),lccc->vmeController()->GetDestMAC(0),lccc->vmeController()->GetDestMAC(1),lccc->vmeController()->GetDestMAC(2),lccc->vmeController()->GetDestMAC(3),lccc->vmeController()->GetDestMAC(4),lccc->vmeController()->GetDestMAC(5));
  //
  VCCHeader(in,out,title,pbuf);
  //

  this->VMECC_UTIL_Menu_Buttons(in,out);

   std::string VCC_PKTSND_DO =
     toolbox::toString("/%s/VCC_PKTSND_DO",getApplicationDescriptor()->getURN().c_str());

  *out << "<form action=\"" << VCC_PKTSND_DO << "\" method=\"GET\">" << std::endl;
  *out << "<fieldset><legend  style=\"font-size: 18pt;\"align=\"center\">Packet Send</legend>" << std::endl;
  *out << "<div align=\"center\">" << std::endl;
  *out << "  <table border=\"3\" cellspacing=\"2\" cellpadding=\"0\" bgcolor=" << VCC_UTIL_curr_color << " style=\"border-collapse: collapse\">" << std::endl;
  *out << "    <tr><td valign=\"top\" colspan=\"2\">" << std::endl;
  *out << "      <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Packet Header</legend>" << std::endl;
  *out << "         <table cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "           <tr>" << std::endl;
  *out << "             <td><input name=\"prio_pkt_chk\" type=\"checkbox\" value=\"prio_ck\"></td>" << std::endl;
  *out << "             <td align=\"left\">Priority Packet</td>" << std::endl;
  *out << "             <td></td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"ack_pkt_chk\" type=\"checkbox\" value=\"ack_ck\"></td>" << std::endl;
  *out << "             <td align=\"left\" colspan=\"2\">Acknowledge Request</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td></td>" << std::endl;
  *out << "             <td align=\"right\">Process Tag:</td>" << std::endl;
  *out << "             <td><input align=\"right\" maxwidth=\"40\" size=\"4\" value=\"" << VCC_UTIL_PKTSND_prcs_tag << "\" name=\"prcs_tag\" type=\"text\"></td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td></td>" << std::endl;
  *out << "             <td align=\"right\">Command:</td>" << std::endl;
  *out << "             <td><input align=\"right\" maxwidth=\"40\" size=\"4\" value=\"" << VCC_UTIL_PKTSND_cmnd << "\" name=\"pkt_cmnd\" type=\"text\"></td>" << std::endl;
  *out << "           </tr>" << std::endl;
  *out << "         </table>" << std::endl;
  *out << "      </fieldset>" << std::endl;
  *out << "    </td></tr>" << std::endl;
  *out << "    <tr>" << std::endl;
  *out << "    <td rowspan=\"2\" valign=\"top\">" << std::endl;
  *out << "      <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Data</legend>" << std::endl;
  *out << "        <textarea name=\"pkt_data\" rows=\"5\" cols=\"12\">" << VCC_UTIL_PKTSND_data << "</textarea></td>" << std::endl;
  *out << "      </fieldset>" << std::endl;
  *out << "    </td>" << std::endl;
  *out << "    <td>" << std::endl;
  *out << "      <table cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "        <tr>" << std::endl;
  *out << "          <td><input name=\"rst_seq_id\" type=\"submit\" value=\"Reset Sequential ID\"></td>" << std::endl;
  *out << "        </tr><tr>" << std::endl;
  *out << "          <td><input name=\"snd_pkt_now\" type=\"submit\" value=\"Send Packet Now\"></td>" << std::endl;
  *out << "        </tr>" << std::endl;
  *out << "      </table>" << std::endl;
  *out << "    </td></tr>" << std::endl;
  *out << "  </table>" << std::endl;
  *out << "</div>" << std::endl;
  *out << "</fieldset>" << std::endl;
  *out << "</form>" << std::endl;

  VCC_UTIL_curr_page = VCC_PKTSND;
}

void EmuPeripheralCrateConfig::VCC_PKTSND_DO(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
  unsigned int itemp;
  int i,nwrds;
  char *ptemp1,*ptemp2,*ptemp3;
  char *pch1 = (char *) &VCC_UTIL_PKTSND_cmnd;
  char *pch2 = (char *) &VCC_UTIL_PKTSND_data;

  emu::pc::Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();

    std::cout<<" entered VCC_PKTSND_DO"<<std::endl;
    cgicc::Cgicc cgi(in);
    const cgicc::CgiEnvironment& env = cgi.getEnvironment();
    std::string guiStr = env.getQueryString() ;
    std::cout << guiStr << std::endl ;

    cgicc::form_iterator rst_seq_id = cgi.getElement("rst_seq_id");
    cgicc::form_iterator snd_pkt_now = cgi.getElement("snd_pkt_now");
    cgicc::form_iterator prio_pkt_chk = cgi.getElement("prio_pkt_chk");
    cgicc::form_iterator ack_pkt_chk = cgi.getElement("ack_pkt_chk");

    if(rst_seq_id != cgi.getElements().end()) {
      vmecc->rst_seq_id();
    }
    if(snd_pkt_now != cgi.getElements().end()) {
      int prcs_tag = cgi["prcs_tag"]->getIntegerValue();
      VCC_UTIL_PKTSND_prcs_tag = cgi["prcs_tag"]->getValue();
      VCC_UTIL_PKTSND_cmnd = cgi["pkt_cmnd"]->getValue();
      VCC_UTIL_PKTSND_data = cgi["pkt_data"]->getValue();
      size_t slen = VCC_UTIL_PKTSND_data.length();
      vmecc->wbuf[0] = prcs_tag & 0x1F;
      vmecc->wbuf[1] = strtoul(VCC_UTIL_PKTSND_cmnd.c_str(),&pch1,16) & 0xFF;
      if(prio_pkt_chk != cgi.getElements().end()) {
        vmecc->wbuf[0]= vmecc->wbuf[0] | 0x40;
      }
      if(ack_pkt_chk != cgi.getElements().end()) {
        vmecc->wbuf[0]= vmecc->wbuf[0] | 0x20;
      }
      ptemp1 = (char *) malloc(slen+1);
      VCC_UTIL_PKTSND_data.copy(ptemp1,slen);
      ptemp1[slen]='\0';
      ptemp3=ptemp1;
      i=0;
      while((ptemp2 = strtok(ptemp3,"\n\r")) !=NULL){
        ptemp3 = 0;
        itemp = strtoul(ptemp2,&pch2,16);
        vmecc->wbuf[2*(++i)]=(itemp>>8)&0xFF;
        vmecc->wbuf[2*i+1]=itemp&0xFF;
      }
      nwrds=i;
      free(ptemp1);
//       std::cout << "Packet Data: " << std::endl;
//       for(i=0;i<=nwrds;i++){
//         printf("%02X%02X\n",vmecc->wbuf[2*i]&0xFF,vmecc->wbuf[2*i+1]&0xFF);
//       }
      vmecc->nwbuf = 2*nwrds + 2;
      //      std::cout <<"nwbuf = " << vmecc->nwbuf << std::endl;
      int n = vmecc->eth_write();
      std::cout << "Wrote " << n << " bytes to controller" << std::endl;
    }
    this->VMECCGUI_pkt_send(in,out);
}

void EmuPeripheralCrateConfig::VMECCGUI_pkt_rcv(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  char ctemp[256];
  static bool first = true;
  emu::pc::Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();
  if(first){
    first = false;

    VCC_UTIL_PKTRCV_num_pkt="1";
    VCC_UTIL_PKTRCV_raw_pkt="";
    VCC_UTIL_PKTRCV_pkt_len="0";
    VCC_UTIL_PKTRCV_dstn_addr="xx-xx-xx-xx-xx-xx";
    VCC_UTIL_PKTRCV_src_addr="xx-xx-xx-xx-xx-xx";
    VCC_UTIL_PKTRCV_pkt_flags="";
    VCC_UTIL_PKTRCV_frg_seq="Seq#";
    VCC_UTIL_PKTRCV_frg_seq_num="";
    VCC_UTIL_PKTRCV_ack_num="";
    VCC_UTIL_PKTRCV_ack_stat="";
    VCC_UTIL_PKTRCV_prc_tag="0";
    VCC_UTIL_PKTRCV_pkt_typ_num="00";
    VCC_UTIL_PKTRCV_pkt_type="";
    VCC_UTIL_PKTRCV_pkt_cmnd="";
    VCC_UTIL_PKTRCV_wrd_cnt="0";
    VCC_UTIL_PKTRCV_rbk_data="";

  }
  int pkts_left = vmecc->LeftToRead();
  sprintf(ctemp,"%d",pkts_left);
  VCC_UTIL_PKTRCV_pkts_inbuf = ctemp;

  char title[] = "VCC Utilities: Packet Receive Commands";
  char pbuf[300];
  sprintf(pbuf,"%s<br>Current Crate is %s<br>MAC Addr: %02x-%02x-%02x-%02x-%02x-%02x",title,(lccc->GetLabel()).c_str(),lccc->vmeController()->GetDestMAC(0),lccc->vmeController()->GetDestMAC(1),lccc->vmeController()->GetDestMAC(2),lccc->vmeController()->GetDestMAC(3),lccc->vmeController()->GetDestMAC(4),lccc->vmeController()->GetDestMAC(5));
  //
  VCCHeader(in,out,title,pbuf);
  //

  this->VMECC_UTIL_Menu_Buttons(in,out);

   std::string VCC_PKTRCV_DO =
     toolbox::toString("/%s/VCC_PKTRCV_DO",getApplicationDescriptor()->getURN().c_str());

  *out << "<form action=\"" << VCC_PKTRCV_DO << "\" method=\"GET\">" << std::endl;
  *out << "<fieldset><legend style=\"font-size: 18pt;\" align=\"center\">Packet Receive</legend>" << std::endl;
  *out << "<div align=\"center\">" << std::endl;
  *out << "  <table border=\"3\" cellspacing=\"2\" cellpadding=\"0\" bgcolor=" << VCC_UTIL_curr_color << " style=\"border-collapse: collapse\">" << std::endl;
  *out << "    <tr>" << std::endl;
  *out << "      <td valign=\"top\">" << std::endl;
  *out << "        <table cellspacing=\"2\" cellpadding=\"2\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "          <tr>" << std::endl;
  *out << "            <td align=\"right\">" << VCC_UTIL_PKTRCV_pkts_inbuf << "</td>" << std::endl;
  *out << "            <td colspan=\"2\" align=\"left\">Packets Remain in Buffer</td>" << std::endl;
  *out << "          </tr><tr>" << std::endl;
  *out << "            <td align=\"right\"><input name=\"rd_pkt\" type=\"submit\" value=\"Read\"></td>" << std::endl;
  *out << "            <td><input align=\"right\" maxwidth=\"20\" size=\"3\" value=\"" << VCC_UTIL_PKTRCV_num_pkt << "\" name=\"num_pkt\" type=\"text\"></td>" << std::endl;
  *out << "            <td align=\"left\">Packets from Driver</td>" << std::endl;
  *out << "          </tr><tr>" << std::endl;
  *out << "            <td align=\"right\"><input name=\"flsh_pkt\" type=\"submit\" value=\"Flush\"></td>" << std::endl;
  *out << "            <td colspan=\"2\" align=\"left\">All Packets from Driver</td>" << std::endl;
  *out << "          </tr>" << std::endl;
  *out << "        </table>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td>" << std::endl;
  *out << "        <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Raw Packet</legend>" << std::endl;
  *out << "          <table cellspacing=\"2\" cellpadding=\"2\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "            <tr>" << std::endl;
  *out << "              <td>" << VCC_UTIL_PKTRCV_raw_pkt << "</td>" << std::endl;
  *out << "            </tr>" << std::endl;
  *out << "          </table>" << std::endl;
  *out << "        </fieldset>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td>" << std::endl;
  *out << "        <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Decoded Header</legend>" << std::endl;
  *out << "          <table cellspacing=\"2\" cellpadding=\"2\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "            <tr>" << std::endl;
  *out << "              <td align=\"right\">Dest:</td>" << std::endl;
  *out << "              <td colspan=\"2\">" << VCC_UTIL_PKTRCV_dstn_addr << "</td>" << std::endl;
  *out << "              <td align=\"right\">Pkt Len:" << std::endl;
  *out << "              <td align=\"left\">" << VCC_UTIL_PKTRCV_pkt_len << " bytes</td>" << std::endl;
  *out << "            </tr><tr>" << std::endl;
  *out << "              <td align=\"right\">Source:</td>" << std::endl;
  *out << "              <td colspan=\"2\">" << VCC_UTIL_PKTRCV_src_addr << "</td>" << std::endl;
  *out << "              <td align=\"right\">Data Wrd Cnt:</td>" << std::endl;
  *out << "              <td align=\"left\">" << VCC_UTIL_PKTRCV_wrd_cnt << " words</td>" << std::endl;
  *out << "            </tr><tr>" << std::endl;
  *out << "              <td align=\"right\">Flags:</td>" << std::endl;
  *out << "              <td align=\"left\" colspan=\"2\">" << VCC_UTIL_PKTRCV_pkt_flags << "</td>" << std::endl;
  *out << "              <td align=\"right\">" << VCC_UTIL_PKTRCV_frg_seq << "</td>" << std::endl;
  *out << "              <td align=\"left\">" << VCC_UTIL_PKTRCV_frg_seq_num << "</td>" << std::endl;
  *out << "            </tr><tr>" << std::endl;
  *out << "              <td align=\"right\">Ack/Status:</td>" << std::endl;
  *out << "              <td align=\"left\">" << VCC_UTIL_PKTRCV_ack_num << "</td>" << std::endl;
  *out << "              <td align=\"left\">" << VCC_UTIL_PKTRCV_ack_stat << "</td>" << std::endl;
  *out << "              <td align=\"right\">Process Tag:</td>" << std::endl;
  *out << "              <td align=\"left\">" << VCC_UTIL_PKTRCV_prc_tag << "</td>" << std::endl;
  *out << "            </tr><tr>" << std::endl;
  *out << "              <td align=\"right\">Pkt Type:</td>" << std::endl;
  *out << "              <td align=\"left\">" << VCC_UTIL_PKTRCV_pkt_typ_num << "</td>" << std::endl;
  *out << "              <td align=\"left\">" << VCC_UTIL_PKTRCV_pkt_type << "</td>" << std::endl;
  *out << "              <td align=\"right\">Command:</td>" << std::endl;
  *out << "              <td align=\"left\">" << VCC_UTIL_PKTRCV_pkt_cmnd << "</td>" << std::endl;
  *out << "            </tr>" << std::endl;
  *out << "          </table>" << std::endl;
  *out << "        </fieldset>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td>" << std::endl;
  *out << "        <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Data</legend>" << std::endl;
  *out << "          <table cellspacing=\"2\" cellpadding=\"2\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "            <tr>" << std::endl;
  *out << "              <td align=\"left\">" << std::endl;
  *out << "                <pre><p>" << VCC_UTIL_PKTRCV_rbk_data << "</p></pre>" << std::endl;
  *out << "              </td>" << std::endl;
  *out << "            </tr>" << std::endl;
  *out << "          </table>" << std::endl;
  *out << "        </fieldset>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "    </tr>" << std::endl;
  *out << "  </table>" << std::endl;
  *out << "</div>" << std::endl;
  *out << "</fieldset>" << std::endl;
  *out << "</form>" << std::endl;

  VCC_UTIL_curr_page = VCC_PKTRCV;
}

void EmuPeripheralCrateConfig::VCC_PKTRCV_DO(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
  char ctemp[256];
  char ctemp2[256];
  int i,n;
  int pkt,pkt_type,wc;
  std::string save1,save2;
  union emu::pc::hdr_stat hdr;
  bool new_pkt;
  emu::pc::Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();

    std::cout<<" entered VCC_PKTRCV_DO"<<std::endl;
    cgicc::Cgicc cgi(in);
    const cgicc::CgiEnvironment& env = cgi.getEnvironment();
    std::string guiStr = env.getQueryString() ;
    std::cout << guiStr << std::endl ;

    cgicc::form_iterator rd_pkt = cgi.getElement("rd_pkt");
    cgicc::form_iterator flsh_pkt = cgi.getElement("flsh_pkt");

    if(flsh_pkt != cgi.getElements().end()) {
      VCC_UTIL_PKTRCV_num_pkt = cgi["num_pkt"]->getValue();
      VCC_UTIL_PKTRCV_raw_pkt="";
      VCC_UTIL_PKTRCV_pkt_len="0";
      VCC_UTIL_PKTRCV_dstn_addr="xx-xx-xx-xx-xx-xx";
      VCC_UTIL_PKTRCV_src_addr="xx-xx-xx-xx-xx-xx";
      VCC_UTIL_PKTRCV_pkt_flags="";
      VCC_UTIL_PKTRCV_frg_seq="Seq#";
      VCC_UTIL_PKTRCV_frg_seq_num="";
      VCC_UTIL_PKTRCV_ack_num="";
      VCC_UTIL_PKTRCV_ack_stat="";
      VCC_UTIL_PKTRCV_prc_tag="0";
      VCC_UTIL_PKTRCV_pkt_typ_num="00";
      VCC_UTIL_PKTRCV_pkt_type="";
      VCC_UTIL_PKTRCV_pkt_cmnd="";
      VCC_UTIL_PKTRCV_wrd_cnt="0";
      VCC_UTIL_PKTRCV_rbk_data="";
      while((n=vmecc->eth_read())>6){
        pkt_type = vmecc->rbuf[PKT_TYP_OFF]&0XFF;
        sprintf(ctemp,"Packet Type: %02x\n",pkt_type&0xFF);
        VCC_UTIL_PKTRCV_rbk_data += ctemp;
      }
    }
    if(rd_pkt != cgi.getElements().end()) {
      int npkts = cgi["num_pkt"]->getIntegerValue();
      VCC_UTIL_PKTRCV_num_pkt = cgi["num_pkt"]->getValue();
      VCC_UTIL_PKTRCV_raw_pkt="";
      VCC_UTIL_PKTRCV_pkt_len="0";
      VCC_UTIL_PKTRCV_dstn_addr="xx-xx-xx-xx-xx-xx";
      VCC_UTIL_PKTRCV_src_addr="xx-xx-xx-xx-xx-xx";
      VCC_UTIL_PKTRCV_pkt_flags="";
      VCC_UTIL_PKTRCV_frg_seq="Seq#";
      VCC_UTIL_PKTRCV_frg_seq_num="";
      VCC_UTIL_PKTRCV_ack_num="";
      VCC_UTIL_PKTRCV_ack_stat="";
      save1 = VCC_UTIL_PKTRCV_prc_tag;
      VCC_UTIL_PKTRCV_prc_tag="0";
      VCC_UTIL_PKTRCV_pkt_typ_num="00";
      VCC_UTIL_PKTRCV_pkt_type="";
      save2 = VCC_UTIL_PKTRCV_pkt_cmnd;
      VCC_UTIL_PKTRCV_pkt_cmnd="";
      VCC_UTIL_PKTRCV_wrd_cnt="0";
      VCC_UTIL_PKTRCV_rbk_data="";
      n = 999;
      for(pkt=0;pkt<npkts && n>6;pkt++){
        n=vmecc->eth_read();
        if(n>6){
	  pkt_type = vmecc->rbuf[PKT_TYP_OFF]&0XFF;
	  wc = DAT_WRD_CNT(vmecc->rbuf);
          sprintf(ctemp,"%d",((vmecc->rbuf[PKT_LEN_OFF]&0xff)<<8)|(vmecc->rbuf[PKT_LEN_OFF+1]&0xff));
	  VCC_UTIL_PKTRCV_pkt_len = ctemp;
	  if(pkt_type>emu::pc::INFO_PKT){
	    std::cout << vmecc->dcode_msg_pkt(vmecc->rbuf) << std::endl;
	    VCC_UTIL_PKTRCV_rbk_data += vmecc->dcode_msg_pkt(vmecc->rbuf);
	    VCC_UTIL_PKTRCV_rbk_data += "\n";
	  }
	  else {
	    for(i=0;i<wc;i++){
	      if(i%8==0){
		sprintf(ctemp,"\nPkt%d Type: %02X:",pkt,pkt_type);
		VCC_UTIL_PKTRCV_rbk_data += ctemp;
	      }
	      sprintf(ctemp," %02X%02X",vmecc->rbuf[2*i+DATA_OFF]&0xFF,vmecc->rbuf[2*i+DATA_OFF+1]&0xFF);
	      VCC_UTIL_PKTRCV_rbk_data += ctemp;
	    }
	    strcpy(ctemp,"\n");
	    VCC_UTIL_PKTRCV_rbk_data += ctemp;
	  }
          VCC_UTIL_PKTRCV_raw_pkt = "";
          int neop =2*wc+22;
          int maxn = 36;
	  for(i=0;i<neop && i<maxn;i++){
            sprintf(ctemp,"%02x",vmecc->rbuf[i]&0xFF);
	    VCC_UTIL_PKTRCV_raw_pkt += ctemp;
	  }
          if(neop>i){
            strcpy(ctemp,"...");
	    VCC_UTIL_PKTRCV_raw_pkt += ctemp;
	  }
          int ofs = MAC_DEST_OFF;
          sprintf(ctemp,"%02X-%02X-%02X-%02X-%02X-%02X",vmecc->rbuf[ofs]&0xFF,vmecc->rbuf[ofs+1]&0xFF,vmecc->rbuf[ofs+2]&0xFF,vmecc->rbuf[ofs+3]&0xFF,vmecc->rbuf[ofs+4]&0xFF,vmecc->rbuf[ofs+5]&0xFF);
 	  VCC_UTIL_PKTRCV_dstn_addr = ctemp;
          ofs = MAC_SRC_OFF;
          sprintf(ctemp,"%02X-%02X-%02X-%02X-%02X-%02X",vmecc->rbuf[ofs]&0xFF,vmecc->rbuf[ofs+1]&0xFF,vmecc->rbuf[ofs+2]&0xFF,vmecc->rbuf[ofs+3]&0xFF,vmecc->rbuf[ofs+4]&0xFF,vmecc->rbuf[ofs+5]&0xFF);
 	  VCC_UTIL_PKTRCV_src_addr = ctemp;
          ctemp[0]='\0';
          hdr.full=vmecc->rbuf[HDR_OFF];
          new_pkt=false;
          if(hdr.tg.prio == 1){
            strcat(ctemp,"prio");
	  }
          if(hdr.tg.newp == 1){
            if(strlen(ctemp)>0)strcat(ctemp," : ");
            strcat(ctemp,"new");
            new_pkt=true;
	  }
          if(hdr.tg.frag == 1){
            if(strlen(ctemp)>0)strcat(ctemp," : ");
            strcat(ctemp,"frag");
	  }
          if(hdr.tg.spnt == 1){
            if(strlen(ctemp)>0)strcat(ctemp," : ");
            strcat(ctemp,"spnt");
	  }
 	  VCC_UTIL_PKTRCV_pkt_flags = ctemp;
          if(new_pkt){
            strcpy(ctemp,"Seq#");
            sprintf(ctemp2,"%d",SEQ_PKT_ID(vmecc->rbuf));
	  }
          else {
            strcpy(ctemp,"Frag#");
            sprintf(ctemp2,"%d",FRAG_NUM(vmecc->rbuf));
	  }
 	  VCC_UTIL_PKTRCV_frg_seq = ctemp;
          VCC_UTIL_PKTRCV_frg_seq_num = ctemp2;

          int ack_stat = AK_STATUS(vmecc->rbuf);
          sprintf(ctemp,"%d",ack_stat);
 	  VCC_UTIL_PKTRCV_ack_num = ctemp;
 	  VCC_UTIL_PKTRCV_ack_stat = emu::pc::ak_status[ack_stat].mnem;
          if(WITH_DATA(vmecc->rbuf)==1){
            strcpy(ctemp," with data");
	  }
	  else {
            strcpy(ctemp," no data");
	  }
 	  VCC_UTIL_PKTRCV_ack_stat += ctemp;

          sprintf(ctemp,"%d",pkt_type);
 	  VCC_UTIL_PKTRCV_pkt_typ_num = ctemp;
          switch(pkt_type){
          case emu::pc::LPBCK_PKT:
            strcpy(ctemp,"LPBCK");
            break;
          case emu::pc::TXNWRDS_PKT:
            strcpy(ctemp,"TXNWRDS");
            break;
	  case emu::pc::EXFIFO_PKT:
            strcpy(ctemp,"EXFIFO");
            break;
	  case emu::pc::VMED08_PKT:
            strcpy(ctemp,"VME_D08");
            break;
	  case emu::pc::VMED16_PKT:
            strcpy(ctemp,"VME_D16");
            break;
	  case emu::pc::VMED32_PKT:
            strcpy(ctemp,"VME_D32");
            break;
	  case emu::pc::VMED64_PKT:
            strcpy(ctemp,"VME_D64");
            break;
	  case emu::pc::JTAG_MOD_PKT:
            strcpy(ctemp,"JTAG_MOD");
            break;
	  case emu::pc::CNFG_MOD_PKT:
            strcpy(ctemp,"CNFG_MOD");
            break;
	  case emu::pc::FLSH_RBK_PKT:
            strcpy(ctemp,"FLSH_RBK");
            break;
	  case emu::pc::ETH_NET_PKT:
            strcpy(ctemp,"ETH_NET");
            break;
	  case emu::pc::IHD08_STAT_ID_PKT:
            strcpy(ctemp,"IH_D08");
            break;
	  case emu::pc::IHD16_STAT_ID_PKT:
            strcpy(ctemp,"IH_D16");
            break;
	  case emu::pc::IHD32_STAT_ID_PKT:
            strcpy(ctemp,"IH_D32");
            break;
	  case emu::pc::INFO_PKT:
            strcpy(ctemp,"INFO");
            break;
	  case emu::pc::WARN_PKT:
            strcpy(ctemp,"WARN");
            break;
	  case emu::pc::ERR_PKT:
            strcpy(ctemp,"ERROR");
            break;
	  default:
            strcpy(ctemp,"Unk. Pkt. Typ");
            break;
	  }
 	  VCC_UTIL_PKTRCV_pkt_type = ctemp;

          if(new_pkt){
	    sprintf(ctemp,"%d",PROC_TAG(vmecc->rbuf));
	    VCC_UTIL_PKTRCV_prc_tag = ctemp;
	    save1 = VCC_UTIL_PKTRCV_prc_tag;
	    sprintf(ctemp,"%02X",vmecc->rbuf[CMND_ACK_OFF]&0XFF);
	    VCC_UTIL_PKTRCV_pkt_cmnd = ctemp;
	    save2 = VCC_UTIL_PKTRCV_pkt_cmnd;
	  }
          else{
	    VCC_UTIL_PKTRCV_prc_tag = save1;
	    VCC_UTIL_PKTRCV_pkt_cmnd = save2;
	  }
          sprintf(ctemp,"%d",wc);
          VCC_UTIL_PKTRCV_wrd_cnt = ctemp;
	}
        else {
	  std::cout << npkts << " packets requested " << pkt << " packets read" << std::endl;
          strcpy(ctemp,"No more packets in driver\n");
          VCC_UTIL_PKTRCV_rbk_data += ctemp;
	}
      }
    }
    this->VMECCGUI_pkt_rcv(in,out);
}

void EmuPeripheralCrateConfig::VMECCGUI_misc_utils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  emu::pc::CNFG_ptr rbk_cp;
  int msglvl;
  char ctemp[256];
  static bool first = true;
  emu::pc::Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();
  if(first){
    first = false;
    rbk_cp = vmecc->read_crs();
    if((rbk_cp->rst_misc & RST_CR_WARN)== RST_CR_WARN){
      VCC_UTIL_misc_warn="Enabled";
    }
    else {
      VCC_UTIL_misc_warn="Disabled";
    }
    if((rbk_cp->rst_misc & RST_CR_STRTUP)== RST_CR_STRTUP){
      VCC_UTIL_misc_strtup="Enabled";
    }
    else {
      VCC_UTIL_misc_strtup="Disabled";
    }
    if((rbk_cp->rst_misc & RST_CR_INT)== RST_CR_INT){
      VCC_UTIL_misc_int="Enabled";
    }
    else {
      VCC_UTIL_misc_int="Disabled";
    }
    if((rbk_cp->rst_misc & RST_CR_FP)== RST_CR_FP){
      VCC_UTIL_misc_fp="Enabled";
    }
    else {
      VCC_UTIL_misc_fp="Disabled";
    }
    if((rbk_cp->rst_misc & RST_CR_SRST)== RST_CR_SRST){
      VCC_UTIL_misc_srst="Enabled";
    }
    else {
      VCC_UTIL_misc_srst="Disabled";
    }
    if((rbk_cp->rst_misc & RST_CR_HR)== RST_CR_HR){
      VCC_UTIL_misc_hr="Enabled";
    }
    else {
      VCC_UTIL_misc_hr="Disabled";
    }
    if((rbk_cp->rst_misc & RST_CR_JTAG)== RST_CR_JTAG){
      VCC_UTIL_misc_jtag="Enabled";
    }
    else {
      VCC_UTIL_misc_jtag="Disabled";
    }
    if((rbk_cp->ether & ETH_CR_SPONT)== ETH_CR_SPONT){
      VCC_UTIL_misc_spont="Enabled";
    }
    else {
      VCC_UTIL_misc_spont="Disabled";
    }
    msglvl = (rbk_cp->rst_misc & RST_CR_MSGLVL)>>8;
    sprintf(ctemp,"%d",msglvl);
    VCC_UTIL_misc_rd_msglvl = ctemp;
    VCC_UTIL_misc_wrt_msglvl = ctemp;
    free(rbk_cp);
  }

  char title[] = "VCC Utilities: Misc. Commands";
  char pbuf[300];
  sprintf(pbuf,"%s<br>Current Crate is %s<br>MAC Addr: %02x-%02x-%02x-%02x-%02x-%02x",title,(lccc->GetLabel()).c_str(),lccc->vmeController()->GetDestMAC(0),lccc->vmeController()->GetDestMAC(1),lccc->vmeController()->GetDestMAC(2),lccc->vmeController()->GetDestMAC(3),lccc->vmeController()->GetDestMAC(4),lccc->vmeController()->GetDestMAC(5));
  //
  VCCHeader(in,out,title,pbuf);
  //

  this->VMECC_UTIL_Menu_Buttons(in,out);

   std::string VCC_MISC_DO =
     toolbox::toString("/%s/VCC_MISC_DO",getApplicationDescriptor()->getURN().c_str());

  *out << "<form action=\"" << VCC_MISC_DO << "\" method=\"GET\">" << std::endl;
  *out << "<fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Misc. Commands</legend>" << std::endl;
  *out << "<div align=\"center\">" << std::endl;
  *out << "  <table border=\"3\" cellspacing=\"2\" cellpadding=\"0\" bgcolor=" << VCC_UTIL_curr_color << " style=\"border-collapse: collapse\">" << std::endl;
  *out << "    <tr><td valign=\"top\">" << std::endl;
  *out << "      <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Startup/Shutdown Packets</legend>" << std::endl;
  *out << "         <table cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "           <tr>" << std::endl;
  *out << "             <td>Warn on shutdown:</td>" << std::endl;
  *out << "             <td><input name=\"misc_warn_ena\" type=\"submit\" value=\"Ena\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "             <td>/</td>" << std::endl;
  *out << "             <td><input name=\"misc_warn_dis\" type=\"submit\" value=\"Dis\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "             <td>" << VCC_UTIL_misc_warn << "</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td>Send pkt. on startup:</td>" << std::endl;
  *out << "             <td><input name=\"misc_strt_ena\" type=\"submit\" value=\"Ena\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "             <td>/</td>" << std::endl;
  *out << "             <td><input name=\"misc_strt_dis\" type=\"submit\" value=\"Dis\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "             <td>" << VCC_UTIL_misc_strtup << "</td>" << std::endl;
  *out << "           </tr>" << std::endl;
  *out << "         </table>" << std::endl;
  *out << "      </fieldset>" << std::endl;
  *out << "    </td>" << std::endl;
  *out << "    <td rowspan=\"3\" valign=\"top\">" << std::endl;
  *out << "      <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Reload/Reset Options</legend>" << std::endl;
  *out << "         <table cellspacing=\"2\" cellpadding=\"2\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "           <tr>" << std::endl;
  *out << "             <td></td>" << std::endl;
  *out << "             <td>Source</td>" << std::endl;
  *out << "             <td>Current</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"misc_int_chk\" type=\"checkbox\" value=\"int_ck\"></td>" << std::endl;
  *out << "             <td align=\"left\">Internal</td>" << std::endl;
  *out << "             <td align=\"right\">" << VCC_UTIL_misc_int << "</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"misc_fp_chk\" type=\"checkbox\" value=\"fp_ck\"></td>" << std::endl;
  *out << "             <td align=\"left\">Front Panel</td>" << std::endl;
  *out << "             <td align=\"right\">" << VCC_UTIL_misc_fp << "</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"misc_srst_chk\" type=\"checkbox\" value=\"srst_ck\"></td>" << std::endl;
  *out << "             <td align=\"left\">SYSRESET</td>" << std::endl;
  *out << "             <td align=\"right\">" << VCC_UTIL_misc_srst << "</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"misc_hr_chk\" type=\"checkbox\" value=\"hr_ck\"></td>" << std::endl;
  *out << "             <td align=\"left\">Hard Reset</td>" << std::endl;
  *out << "             <td align=\"right\">" << VCC_UTIL_misc_hr << "</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"misc_jtag_chk\" type=\"checkbox\" value=\"jtag_ck\"></td>" << std::endl;
  *out << "             <td align=\"left\">JTAG (PROM)</td>" << std::endl;
  *out << "             <td align=\"right\">" << VCC_UTIL_misc_jtag << "</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td></td>" << std::endl;
  *out << "             <td><input name=\"misc_rst_src_wrt\" type=\"submit\" value=\"Write\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "             <td><input name=\"misc_rst_src_rd\" type=\"submit\" value=\"Read\"></td>" << std::endl;
  *out << "           </tr>" << std::endl;
  *out << "         </table>" << std::endl;
  *out << "         <table cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "           <tr>" << std::endl;
  *out << "             <td><input name=\"misc_int_rst\" type=\"submit\" value=\"Int. Reload Cmnd.\"></td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"misc_jtag_rst\" type=\"submit\" value=\"JTAG Reload Cmnd.\"></td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"misc_sysrst\" type=\"submit\" value=\"VME SYSRESET\"></td>" << std::endl;
  *out << "           </tr>" << std::endl;
  *out << "         </table>" << std::endl;
  *out << "      </fieldset>" << std::endl;
  *out << "    </td></tr>" << std::endl;
  *out << "    <tr><td valign=\"top\">" << std::endl;
  *out << "      <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Errors, Warnings and Info Packets</legend>" << std::endl;
  *out << "         <table cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "           <tr>" << std::endl;
  *out << "             <td>Spontaneous Packets:</td>" << std::endl;
  *out << "             <td><input name=\"misc_spont_ena\" type=\"submit\" value=\"Ena\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "             <td>/</td>" << std::endl;
  *out << "             <td><input name=\"misc_spont_dis\" type=\"submit\" value=\"Dis\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "             <td>" << VCC_UTIL_misc_spont << "</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td>Message Level:</td>" << std::endl;
  *out << "             <td colspan=\"3\"><input name=\"misc_msglvl\" type=\"submit\" value=\"Set\" " << VCC_UTIL_acc_cntrl << ">" << std::endl;
  *out << "                 <input align=\"right\" maxwidth=\"20\" size=\"2\" value=\"" << VCC_UTIL_misc_wrt_msglvl << "\" name=\"rd_msglvl\" type=\"text\"></td>" << std::endl;
  *out << "             <td>" << VCC_UTIL_misc_rd_msglvl << "</td>" << std::endl;
  *out << "           </tr>" << std::endl;
  *out << "         </table>" << std::endl;
  *out << "      </fieldset>" << std::endl;
  *out << "    </td></tr>" << std::endl;
  *out << "    <tr><td valign=\"top\">" << std::endl;
  *out << "      <input name=\"misc_refresh\" type=\"submit\" value=\"Refresh\">" << std::endl;
  *out << "    </td></tr>" << std::endl;
  *out << "  </table>" << std::endl;
  *out << "</div>" << std::endl;
  *out << "</fieldset>" << std::endl;
  *out << "</form>" << std::endl;

  VCC_UTIL_curr_page = VCC_MISC;
}

void EmuPeripheralCrateConfig::VCC_MISC_DO(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
  emu::pc::CNFG_ptr rbk_cp;
  char ctemp[256];
  int msglvl;
  emu::pc::Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();

    std::cout<<" entered VCC_MISC_DO"<<std::endl;
    cgicc::Cgicc cgi(in);
    const cgicc::CgiEnvironment& env = cgi.getEnvironment();
    std::string guiStr = env.getQueryString() ;
    std::cout << guiStr << std::endl ;

    cgicc::form_iterator misc_warn_ena = cgi.getElement("misc_warn_ena");
    cgicc::form_iterator misc_warn_dis = cgi.getElement("misc_warn_dis");
    cgicc::form_iterator misc_strt_ena = cgi.getElement("misc_strt_ena");
    cgicc::form_iterator misc_strt_dis = cgi.getElement("misc_strt_dis");
    cgicc::form_iterator misc_spont_ena = cgi.getElement("misc_spont_ena");
    cgicc::form_iterator misc_spont_dis = cgi.getElement("misc_spont_dis");
    cgicc::form_iterator misc_msglvl = cgi.getElement("misc_msglvl");
    cgicc::form_iterator misc_rst_src_wrt = cgi.getElement("misc_rst_src_wrt");
    cgicc::form_iterator misc_rst_src_rd = cgi.getElement("misc_rst_src_rd");
    cgicc::form_iterator misc_int_chk = cgi.getElement("misc_int_chk");
    cgicc::form_iterator misc_fp_chk = cgi.getElement("misc_fp_chk");
    cgicc::form_iterator misc_srst_chk = cgi.getElement("misc_srst_chk");
    cgicc::form_iterator misc_hr_chk = cgi.getElement("misc_hr_chk");
    cgicc::form_iterator misc_jtag_chk = cgi.getElement("misc_jtag_chk");
    cgicc::form_iterator misc_int_rst = cgi.getElement("misc_int_rst");
    cgicc::form_iterator misc_jtag_rst = cgi.getElement("misc_jtag_rst");
    cgicc::form_iterator misc_sysrst = cgi.getElement("misc_sysrst");
    cgicc::form_iterator misc_refresh = cgi.getElement("misc_refresh");

    if(misc_warn_ena != cgi.getElements().end()) {
      vmecc->enable_warn_on_shutdown();
      VCC_UTIL_misc_warn = "Enabled";
    }
    if(misc_warn_dis != cgi.getElements().end()) {
      vmecc->disable_warn_on_shutdown();
      VCC_UTIL_misc_warn = "Disabled";
    }
    if(misc_strt_ena != cgi.getElements().end()) {
      vmecc->snd_startup_pkt();
      VCC_UTIL_misc_strtup = "Enabled";
    }
    if(misc_strt_dis != cgi.getElements().end()) {
      vmecc->no_startup_pkt();
      VCC_UTIL_misc_strtup = "Disabled";
    }
    if(misc_spont_ena != cgi.getElements().end()) {
      vmecc->set_clr_bits(emu::pc::SET, emu::pc::ETHER, ETH_CR_SPONT);
      VCC_UTIL_misc_spont = "Enabled";
    }
    if(misc_spont_dis != cgi.getElements().end()) {
      vmecc->set_clr_bits(emu::pc::CLR, emu::pc::ETHER, ~ETH_CR_SPONT);
      VCC_UTIL_misc_spont = "Disabled";
    }
    if(misc_msglvl != cgi.getElements().end()) {
      msglvl = cgi["rd_msglvl"]->getIntegerValue();
      vmecc->set_clr_bits(emu::pc::CLR, emu::pc::RST_MISC, ~RST_CR_MSGLVL);
      vmecc->set_clr_bits(emu::pc::SET, emu::pc::RST_MISC, RST_CR_MSGLVL & (msglvl<<8));
      sprintf(ctemp,"%d",msglvl);
      VCC_UTIL_misc_wrt_msglvl = ctemp;
      rbk_cp = vmecc->read_crs();
      msglvl = (rbk_cp->rst_misc & RST_CR_MSGLVL)>>8;
      sprintf(ctemp,"%d",msglvl);
      VCC_UTIL_misc_rd_msglvl = ctemp;
      free(rbk_cp);
    }
    if(misc_rst_src_wrt != cgi.getElements().end()) {
      unsigned int val = 0;
      if(misc_int_chk != cgi.getElements().end()) {
        val |= RST_CR_INT;
      }
      if(misc_fp_chk != cgi.getElements().end()) {
        val |= RST_CR_FP;
      }
      if(misc_srst_chk != cgi.getElements().end()) {
        val |= RST_CR_SRST;
      }
      if(misc_hr_chk != cgi.getElements().end()) {
        val |= RST_CR_HR;
      }
      if(misc_jtag_chk != cgi.getElements().end()) {
        val |= RST_CR_JTAG;
      }
      vmecc->set_clr_bits(emu::pc::CLR, emu::pc::RST_MISC, ~(0x1F));
      vmecc->set_clr_bits(emu::pc::SET, emu::pc::RST_MISC, val);
    }
    if(misc_rst_src_rd != cgi.getElements().end() || misc_refresh != cgi.getElements().end()) {
      rbk_cp = vmecc->read_crs();
      if((rbk_cp->rst_misc & RST_CR_INT)!=0){
        VCC_UTIL_misc_int = "Enabled";
      }else{
        VCC_UTIL_misc_int = "Disabled";
      }
      if((rbk_cp->rst_misc & RST_CR_FP)!=0){
        VCC_UTIL_misc_fp = "Enabled";
      }else{
        VCC_UTIL_misc_fp = "Disabled";
      }
      if((rbk_cp->rst_misc & RST_CR_SRST)!=0){
        VCC_UTIL_misc_srst = "Enabled";
      }else{
        VCC_UTIL_misc_srst = "Disabled";
      }
      if((rbk_cp->rst_misc & RST_CR_HR)!=0){
        VCC_UTIL_misc_hr = "Enabled";
      }else{
        VCC_UTIL_misc_hr = "Disabled";
      }
      if((rbk_cp->rst_misc & RST_CR_JTAG)!=0){
        VCC_UTIL_misc_jtag = "Enabled";
      }else{
        VCC_UTIL_misc_jtag = "Disabled";
      }
      if(misc_refresh != cgi.getElements().end()){
	if((rbk_cp->rst_misc & RST_CR_WARN)== RST_CR_WARN){
	  VCC_UTIL_misc_warn="Enabled";
	}
	else {
	  VCC_UTIL_misc_warn="Disabled";
	}
	if((rbk_cp->rst_misc & RST_CR_STRTUP)== RST_CR_STRTUP){
	  VCC_UTIL_misc_strtup="Enabled";
	}
	else {
	  VCC_UTIL_misc_strtup="Disabled";
	}
	if((rbk_cp->ether & ETH_CR_SPONT)== ETH_CR_SPONT){
	  VCC_UTIL_misc_spont="Enabled";
	}
	else {
	  VCC_UTIL_misc_spont="Disabled";
	}
	msglvl = (rbk_cp->rst_misc & RST_CR_MSGLVL)>>8;
	sprintf(ctemp,"%d",msglvl);
	VCC_UTIL_misc_rd_msglvl = ctemp;
      }
      free(rbk_cp);
    }
    if(misc_int_rst != cgi.getElements().end()) {
      vmecc->force_reload();
    }
    if(misc_jtag_rst != cgi.getElements().end()) {
      vmecc->reload_fpga();
    }
    if(misc_sysrst != cgi.getElements().end()) {
      vmecc->set_clr_bits(emu::pc::SET, emu::pc::VME, VME_CR_FRC_SRST);
    }
    this->VMECCGUI_misc_utils(in,out);
}
//
// BGB End of New Controller Utils 
//

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
  *out << "Read Register (hex)..." << std::endl;
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
    int verify(1);
    printf("Programming CCB using %s\n", CCBFirmware_.toString().c_str());
    int status = thisCCB->svfLoad(&jch,CCBFirmware_.toString().c_str(),debugMode, verify);
    if (status >= 0){
      std::cout << "=== Programming finished"<< std::endl;
      std::cout << "=== " << status << " Verify Errors  occured" << std::endl;
    }
    else{
      std::cout << "=== Fatal Error. Exiting with " <<  status << std::endl;
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
  //
  if ( thisMPC->CheckFirmwareDate() ) {
    //
    *out << cgicc::span().set("style","color:green");
    thisMPC->printFirmwareVersion();
    *out << "...OK...";
    *out << cgicc::span();
    //
  } else {
    //
    *out << cgicc::span().set("style","color:red");
    thisMPC->printFirmwareVersion();
    *out << " --->>  BAD  <<--- Should be (" 
	 << std::dec << thisMPC->GetExpectedFirmwareDay() 
	 << "-"      << thisMPC->GetExpectedFirmwareMonth()
	 << "-"      << thisMPC->GetExpectedFirmwareYear()
	 << ")";

    *out << cgicc::span();
  }
  *out << cgicc::br();
  //
  thisMPC->RedirectOutput(&std::cout);
  //
  *out << cgicc::br() << "CSR0 = " << std::hex << thisMPC->ReadRegister(0) << std::endl;
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
  int verify(1);
  printf("Programming MPC using %s\n", MPCFirmware_.toString().c_str());
  int status = thisMPC->svfLoad(&jch,MPCFirmware_.toString().c_str(),debugMode, verify);
  if (status >= 0){
    std::cout << "=== Programming finished"<< std::endl;
    std::cout << "=== " << status << " Verify Errors  occured" << std::endl;
  }
  else{
    std::cout << "=== Fatal Error. Exiting with " <<  status << std::endl;
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
    std::cout << "TMBTests: TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "TMBTests: No tmb" << std::endl ;
    tmb = TMB_;
  }
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
  //
  emu::pc::Chamber * thisChamber = chamberVector[tmb];
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
  *out << std::endl ;
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
    *out << cgicc::input().set("type","submit").set("value","TMB Voltages and temps").set("style","color:blue" ) << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestADC() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB Voltages and temps").set("style","color:green") << std::endl ;
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB Voltages and temps").set("style","color:red"  ) << std::endl ;
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
  *out << OutputTMBTests[tmb][current_crate_].str() << std::endl ;
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
    std::cout << "testTMB:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  name = cgi.getElement("tmbTestid");
  //
  int tmbTestid;
  if(name != cgi.getElements().end()) {
    tmbTestid = cgi["tmbTestid"]->getIntegerValue();
    std::cout << "tmbTestid " << tmbTestid << std::endl;
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
    std::cout << "TMBStatus:  TMB=" << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "TMBStatus: No TMB" << std::endl ;
    tmb = TMB_;
  }
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
  //
  alct = thisTMB->alctController();
  rat  = thisTMB->getRAT();
  //
  if (alct) {
    std::string ALCTStatus =
      toolbox::toString("/%s/ALCTStatus?tmb=%d",getApplicationDescriptor()->getURN().c_str(),tmb);
    //
    *out << cgicc::a("ALCT Status").set("href",ALCTStatus) << std::endl;
    //
  }
  //
  if (rat) {
    std::string RATStatus =
      toolbox::toString("/%s/RATStatus?tmb=%d",getApplicationDescriptor()->getURN().c_str(),tmb);
    //
    *out << cgicc::a("RAT Status").set("href",RATStatus) << std::endl;
    //
  }
  //
  emu::pc::Chamber * thisChamber = chamberVector[tmb];
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
  thisTMB->FirmwareVersion();
  thisTMB->FirmwareRevCode();
  //
  // output the register information to the screen in a nice way:
  //
  sprintf(buf,"TMB Firmware date (month/day/year) : (%02d/%02d/%04d)",
	  thisTMB->GetReadTmbFirmwareMonth(),
	  thisTMB->GetReadTmbFirmwareDay(),
	  thisTMB->GetReadTmbFirmwareYear());
  //
  if ( thisTMB->CheckFirmwareDate() ) {
    *out << cgicc::span().set("style","color:green");
    *out << buf;
    *out << "...OK...";
    *out << cgicc::span();
  } else {
    *out << cgicc::span().set("style","color:red");
    *out << buf;
    *out << "--->> BAD <<---, should be ("
	 << thisTMB->GetExpectedTmbFirmwareMonth()
	 << "/"      << thisTMB->GetExpectedTmbFirmwareDay()
	 << "/"      << thisTMB->GetExpectedTmbFirmwareYear()
	 << ")";

    *out << cgicc::span();
  }
  *out << cgicc::br();
  //
  //
  if ( thisTMB->GetReadTmbFirmwareType() == thisTMB->GetExpectedTmbFirmwareType() ) {
    *out << cgicc::span().set("style","color:green");
  } else {
    *out << cgicc::span().set("style","color:red");
  }
  sprintf(buf,"Firmware Type             : %01x ",
	  thisTMB->GetReadTmbFirmwareType());       
  *out << buf;
  *out << cgicc::span();
  *out << cgicc::br();
  //
  //
  if ( thisTMB->GetReadTmbFirmwareVersion() == thisTMB->GetExpectedTmbFirmwareVersion() ){
    *out << cgicc::span().set("style","color:green");
  } else {
    *out << cgicc::span().set("style","color:red");
  }
  sprintf(buf,"Firmware Version Code     : %01x ",
	  thisTMB->GetReadTmbFirmwareVersion());       
  *out << buf ;
  *out << cgicc::span();
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
  sprintf(buf,"Firmware Revision Code    : %04x ",thisTMB->GetReadTmbFirmwareRevcode());       
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
    std::cout << "TMBUtils:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "TMBUtils:  No TMB" << std::endl ;
    tmb = TMB_;
  }
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
  //
  emu::pc::Chamber * thisChamber = chamberVector[tmb];
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
  *out << std::endl ;
  //
  *out << cgicc::legend("Load Firmware through TMB").set("style","color:blue") ;
  //
  //
  *out << cgicc::table().set("border","2");
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << "-->  BE CAREFUL <--";
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << "-->  Do NOT power off crate <--";
  *out << cgicc::td();
  //
  *out << cgicc::table();
  //
  *out << cgicc::br();
  //
  *out << "TMB: " << cgicc::br() << std::endl;
  *out << "firmware version = " << TMBFirmware_[tmb].toString() << ".xsvf" << cgicc::br() << std::endl;
  *out << cgicc::br();
  //
  *out << "Step 1)  Disable DCS monitoring to crates, and TURN OFF ALCTs" << cgicc::br() << std::endl;
  //
  std::string CheckCrateControllerFromTMBPage = toolbox::toString("/%s/CheckCrateControllerFromTMBPage",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CheckCrateControllerFromTMBPage) << std::endl ;
  if ( crate_controller_status == 1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","Step 2) Check VME Crate Controller").set("style","color:green");
    //
  } else if ( crate_controller_status == 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","Step 2) Check VME Crate Controller").set("style","color:red");
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","Step 2) Check VME Crate Controller").set("style","color:blue");
    //
  }
  *out << cgicc::form() << std::endl ;
  //
  //
  *out << cgicc::table().set("border","0");
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string LoadTMBFirmware = toolbox::toString("/%s/LoadTMBFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadTMBFirmware) << std::endl ;
  sprintf(buf,"Step 3) Load Firmware for TMB in slot %d",tmbVector[tmb]->slot());
  *out << cgicc::input().set("type","submit").set("value",buf) << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << "... or ...";
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string LoadCrateTMBFirmware = toolbox::toString("/%s/LoadCrateTMBFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadCrateTMBFirmware) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Step 3) Load firmware (broadcast) to all TMBs in this crate") << std::endl ;
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::table();
  //
  //
  std::string CCBHardResetFromTMBPage = toolbox::toString("/%s/CCBHardResetFromTMBPage",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CCBHardResetFromTMBPage) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Step 4) CCB hard reset") << std::endl ;
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
  *out << cgicc::form() << std::endl ;
  //
  std::string ClearTMBBootReg = toolbox::toString("/%s/ClearTMBBootReg",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ClearTMBBootReg) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Step 6) Enable VME Access to TMB FPGA") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br() << std::endl;
  *out << cgicc::br() << std::endl;
  //
  if (alct) {
    *out << "ALCT: " << cgicc::br() << std::endl;
    //    int i = tmb;
    for (unsigned i=0; i<tmbVector.size(); i++) {
      //
      int check_value = tmbVector[i]->alctController()->CheckFirmwareConfiguration();
      //
      if (check_value == 1) {
	*out << cgicc::span().set("style","color:black");
      } else if (check_value == 0) {
	*out << cgicc::span().set("style","color:red");
	*out << "Note:  Firmware database check FAILED....  Hence, the button below will not load " << std::endl;
      } else if (check_value == -1) {
	*out << cgicc::span().set("style","color:blue");
      } else {
	*out << cgicc::span().set("style","color:green");
      }
      *out << "firmware version for slot " << tmbVector[i]->slot() << " = " << ALCTFirmware_[i].toString() << cgicc::br() << std::endl;
      *out << cgicc::span() << std::endl ;
    }
    //
    *out << cgicc::br() << std::endl;
    //
    *out << "Step 1)  Disable DCS monitoring to crates" << cgicc::br() << std::endl;
    //
    *out << cgicc::table().set("border","0");
    //
    *out << cgicc::td().set("ALIGN","left");
    std::string LoadALCTFirmware = toolbox::toString("/%s/LoadALCTFirmware",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",LoadALCTFirmware) << std::endl ;
    sprintf(buf,"Step 2) Load Firmware for ALCT in slot %d",tmbVector[tmb]->slot());
    *out << cgicc::input().set("type","submit").set("value",buf) << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
    *out << cgicc::td();
    //
    *out << cgicc::td().set("ALIGN","center");
    *out << "... or ...";
    *out << cgicc::td();
    //
    *out << cgicc::td().set("ALIGN","left");
    std::string LoadCrateALCTFirmware = toolbox::toString("/%s/LoadCrateALCTFirmware",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",LoadCrateALCTFirmware) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Step 2) Load firmware (serially) to all ALCTs in this crate") << std::endl ;
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::form().set("method","GET").set("action",CCBHardResetFromTMBPage) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Step 3) CCB hard reset") << std::endl ;
    *out << cgicc::form() << std::endl ;
  }
  //
  *out << cgicc::br() << std::endl;
  *out << cgicc::br() << std::endl;
  //
  if (rat) {
    *out << "RAT: " << cgicc::br() << std::endl;
    *out << "firmware version = " << RATFirmware_[tmb].toString() ;
    std::string LoadRATFirmware = toolbox::toString("/%s/LoadRATFirmware",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",LoadRATFirmware) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Step 1) Load RAT Firmware") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::form().set("method","GET").set("action",CCBHardResetFromTMBPage) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Step 2) CCB hard reset") << std::endl ;
    *out << cgicc::form() << std::endl ;

  }
  //
  *out << cgicc::fieldset();
  //
  *out << cgicc::br() << std::endl;
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << std::endl ;
  //
  *out << cgicc::legend("Other TMB Utilities").set("style","color:blue") ;
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
  //  *out << cgicc::tr();
  //  //
  //  *out << cgicc::td().set("ALIGN","left");
  //  *out << "Inject fake data";
  //  *out << cgicc::td();
  //  //
  //  *out << cgicc::td().set("ALIGN","left");
  //  std::string TriggerTestInjectALCT = toolbox::toString("/%s/TriggerTestInjectALCT",getApplicationDescriptor()->getURN().c_str());
  //  *out << cgicc::form().set("method","GET").set("action",TriggerTestInjectALCT) ;
  //  *out << cgicc::input().set("type","submit").set("value","TriggerTest : InjectALCT") ;
  //  sprintf(buf,"%d",tmb);
  //  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  //  *out << cgicc::form() << std::endl ;
  //  *out << cgicc::td();
  //  //
  //  *out << cgicc::td().set("ALIGN","left");
  //  std::string TriggerTestInjectCLCT = toolbox::toString("/%s/TriggerTestInjectCLCT",getApplicationDescriptor()->getURN().c_str());
  //  *out << cgicc::form().set("method","GET").set("action",TriggerTestInjectCLCT) ;
  //  *out << cgicc::input().set("type","submit").set("value","TriggerTest : InjectCLCT") ;
  //  sprintf(buf,"%d",tmb);
  //  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  //  *out << cgicc::form() << std::endl ;
  //  *out << cgicc::td();
  //  //
  //  //////////////////////////////////////////////
  //  *out << cgicc::tr();
  //  //
  //  *out << cgicc::td().set("ALIGN","left");
  //  *out << "TMB Scope";
  //  *out << cgicc::td();
  //  //
  //  *out << cgicc::td().set("ALIGN","left");
  //  std::string armScope = toolbox::toString("/%s/armScope",getApplicationDescriptor()->getURN().c_str());
  //  *out << cgicc::form().set("method","GET").set("action",armScope) ;
  //  *out << cgicc::input().set("type","submit").set("value","arm Scope") ;
  //  sprintf(buf,"%d",tmb);
  //  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  //  *out << cgicc::form() << std::endl ;
  //  *out << cgicc::td();
  //  //
  //  *out << cgicc::td().set("ALIGN","left");
  //  std::string readoutScope = toolbox::toString("/%s/readoutScope",getApplicationDescriptor()->getURN().c_str());
  //  *out << cgicc::form().set("method","GET").set("action",readoutScope) ;
  //  *out << cgicc::input().set("type","submit").set("value","readout Scope") ;
  //  sprintf(buf,"%d",tmb);
  //  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  //  *out << cgicc::form() << std::endl ;
  //  *out << cgicc::td();
  //  //
  //  *out << cgicc::td().set("ALIGN","left");
  //  std::string forceScope = toolbox::toString("/%s/forceScope",getApplicationDescriptor()->getURN().c_str());
  //  *out << cgicc::form().set("method","GET").set("action",forceScope) ;
  //  *out << cgicc::input().set("type","submit").set("value","force Scope") ;
  //  sprintf(buf,"%d",tmb);
  //  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  //  *out << cgicc::form() << std::endl ;
  //  *out << cgicc::td();
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
  //  *out << cgicc::tr();
  //  //
  //  *out << cgicc::td().set("ALIGN","left");
  //  *out << "Raw Hits";
  //  *out << cgicc::td();
  //  //
  //  *out << cgicc::td().set("ALIGN","left");
  //  std::string TMBRawHits = toolbox::toString("/%s/TMBRawHits",getApplicationDescriptor()->getURN().c_str());
  //  *out << cgicc::form().set("method","GET").set("action",TMBRawHits) ;
  //  *out << cgicc::input().set("type","submit").set("value","Read TMB Raw Hits") ;
  //  sprintf(buf,"%d",tmb);
  //  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  //  *out << cgicc::form() << std::endl ;
  //  *out << cgicc::td();
  //  //
  //  *out << cgicc::td().set("ALIGN","left");
  //  std::string ALCTRawHits = toolbox::toString("/%s/ALCTRawHits",getApplicationDescriptor()->getURN().c_str());
  //  *out << cgicc::form().set("method","GET").set("action",ALCTRawHits) ;
  //  *out << cgicc::input().set("type","submit").set("value","Read ALCT Raw Hits") ;
  //  sprintf(buf,"%d",tmb);
  //  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  //  *out << cgicc::form() << std::endl ;
  //  *out << cgicc::td();
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
  *out << OutputStringTMBStatus[tmb].str() << std::endl ;
  *out << cgicc::textarea();
  OutputStringTMBStatus[tmb].str("");
  *out << cgicc::pre();
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::fieldset();

}
//
void EmuPeripheralCrateConfig::DefineFirmwareFilenames() {
  //
  std::string DMBFirmware = FirmwareDir_+DMB_FIRMWARE_FILENAME;
  DMBFirmware_ = DMBFirmware;
  //
  std::string DMBVmeFirmware = FirmwareDir_+DMBVME_FIRMWARE_FILENAME;
  DMBVmeFirmware_ = DMBVmeFirmware;
  //
  std::string CFEBFirmware = FirmwareDir_+CFEB_FIRMWARE_FILENAME;
  std::string CFEBVerify = FirmwareDir_+CFEB_VERIFY_FILENAME;
  CFEBVerify_ = CFEBVerify;
  CFEBFirmware_ = CFEBFirmware;
  //
  //create filename for TMB, ALCT, and RAT firmware based on expected dates...
  for (unsigned tmb=0; tmb<tmbVector.size(); tmb++) {

    // must skip those broadcast slots. added by Liu, May 09 2008 
    if((tmbVector[tmb]->slot())>21) continue;   

    //
    emu::pc::TMB * thisTMB = tmbVector[tmb];
    emu::pc::ALCTController  * thisALCT = tmbVector[tmb]->alctController();
    //
    char date[8];
    sprintf(date,"%04u%02u%02u",
	    thisTMB->GetExpectedTmbFirmwareYear(),
	    thisTMB->GetExpectedTmbFirmwareMonth(),
	    thisTMB->GetExpectedTmbFirmwareDay());
    //
    std::ostringstream TMBFirmware;
    TMBFirmware << FirmwareDir_ << "tmb/" << date << "/tmb";   // ".xsvf" is added in SetXsvfFilename
    //
    TMBFirmware_[tmb] = TMBFirmware.str();
    //    std::cout << "TMB " << tmb << " load " << TMBFirmware_[tmb].toString() << std::endl;
    //
    int alct_expected_year  = thisALCT->GetExpectedFastControlYear() ;
    int alct_expected_month = thisALCT->GetExpectedFastControlMonth();
    int alct_expected_day   = thisALCT->GetExpectedFastControlDay()  ;
    //
    sprintf(date,"%4u%02u%02u",
	    alct_expected_year,
	    alct_expected_month,
	    alct_expected_day);
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
    std::ostringstream ALCTFirmware;
    ALCTFirmware << FirmwareDir_ << "alct/" << date << "/";
    //
    if ( (thisALCT->GetChamberType()).find("ME11") != std::string::npos ) {
      //
      if (thisALCT->GetExpectedFastControlBackwardForwardType() == BACKWARD_FIRMWARE_TYPE &&
	  thisALCT->GetExpectedFastControlNegativePositiveType() == NEGATIVE_FIRMWARE_TYPE ) {
	ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME11_BACKWARD_NEGATIVE;
      } else if (thisALCT->GetExpectedFastControlBackwardForwardType() == BACKWARD_FIRMWARE_TYPE &&
		 thisALCT->GetExpectedFastControlNegativePositiveType() == POSITIVE_FIRMWARE_TYPE ) {
	ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME11_BACKWARD_POSITIVE;
      } else if (thisALCT->GetExpectedFastControlBackwardForwardType() == FORWARD_FIRMWARE_TYPE &&
		 thisALCT->GetExpectedFastControlNegativePositiveType() == POSITIVE_FIRMWARE_TYPE ) {
	ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME11_FORWARD_POSITIVE;
      } else {
	ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME11;
      }
    } else if ( (thisALCT->GetChamberType()).find("ME12") != std::string::npos ) {
      ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME12;
    } else if ( (thisALCT->GetChamberType()).find("ME13") != std::string::npos ) {
      ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME13;
    } else if ( (thisALCT->GetChamberType()).find("ME21") != std::string::npos ) {
      ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME21;
    } else if ( (thisALCT->GetChamberType()).find("ME22") != std::string::npos ) {
      ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME22;
    } else if ( (thisALCT->GetChamberType()).find("ME31") != std::string::npos ) {
      ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME31;
    } else if ( (thisALCT->GetChamberType()).find("ME32") != std::string::npos ) {
      ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME32;
    } else if ( (thisALCT->GetChamberType()).find("ME41") != std::string::npos ) {
      ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME41;
    } 
    //
    ALCTFirmware_[tmb] = ALCTFirmware.str();
    //    std::cout << "ALCT " << tmb << " load " << ALCTFirmware_[tmb].toString() << std::endl;
    //
    sprintf(date,"%4u%02u%02u",
	    thisTMB->GetExpectedRatFirmwareYear() ,
	    thisTMB->GetExpectedRatFirmwareMonth(),
	    thisTMB->GetExpectedRatFirmwareDay()  );
    //
    std::ostringstream RATFirmware; 
    RATFirmware << FirmwareDir_ << "rat/" << date << "/rat.svf";
    //
    RATFirmware_[tmb] = RATFirmware.str();
    //    std::cout << "RAT " << tmb << " load " << RATFirmware_[tmb].toString() << std::endl;
  }
  //
  return;
}
//
void EmuPeripheralCrateConfig::CheckCrateControllerFromTMBPage(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {  
  //
  crate_controller_status = (int) 
    (crateVector[current_crate_]->vmeController()->SelfTest()) && 
    (crateVector[current_crate_]->vmeController()->exist(13));
  //
  this->TMBUtils(in,out);
}
//
void EmuPeripheralCrateConfig::LoadTMBFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "Load firmware for TMB[" << tmb << "]" << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "No TMB defined to load... taking default = " << TMB_ << std::endl ;
    tmb = TMB_;
  }
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
  //
  tmb_vme_ready = -1;
  //
  // Put CCB in FPGA mode to make the CCB ignore TTC commands (such as hard reset) during TMB downloading...
  thisCCB->setCCBMode(emu::pc::CCB::VMEFPGA);
  //
  int number_of_verify_errors = 0;
  //
  if (thisTMB->slot() < 22) {
    std::cout << "Loading TMB firmware to slot " << thisTMB->slot() 
	      << " with " << TMBFirmware_[tmb].toString()
	      << " in 5 seconds..." << std::endl;
    //
    ::sleep(5);
    //
    thisTMB->SetXsvfFilename(TMBFirmware_[tmb].toString().c_str());
    thisTMB->ProgramTMBProms();
    thisTMB->ClearXsvfFilename();
    //
    number_of_verify_errors = thisTMB->GetNumberOfVerifyErrors();
    std::cout << "=== Programming TMB firmware finished for slot " << thisTMB->slot() << std::endl;
    std::cout << "=== " << number_of_verify_errors << " Verify Errors occured" << std::endl;
    //
    if (number_of_verify_errors < 0) {
      std::cout << "File does not exist, programming did not occur..."<< std::endl;
      //
    } else if (number_of_verify_errors == 0) {
      std::cout << "Please perform a TTC/CCB hard reset to Load FPGA"<< std::endl;
      //
    } else {
      std::cout << "ERROR!! -> Number of errors = " << number_of_verify_errors << " not equal to 0!!" << std::endl;
      std::cout << std::endl;
      std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << std::endl;
      std::cout << "!!!!     IF you are BROADCASTING TO CRATE, this is OK     !!!! " << std::endl;
      std::cout << "!!!!                                                      !!!! " << std::endl;
      std::cout << "!!!! IF you are NOT, this is a PROBLEM, DO NOT HARD RESET !!!! " << std::endl;
      std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << std::endl;
    }
    //
  } 
  //
  // Put CCB back into DLOG mode to listen to TTC commands...
  thisCCB->setCCBMode(emu::pc::CCB::DLOG);
  //
  this->TMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::LoadCrateTMBFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "Load TMB firmware crate, called from TMB[" << tmb << "]" << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "No TMB defined to load... taking default = " << TMB_ << std::endl ;
    tmb = TMB_;
  }
  //
  // Create a TMB which all TMB's within a crate will listen to....
  //
  emu::pc::Chamber * thisChamber = chamberVector[tmb];
  emu::pc::TMB * thisTMB = new emu::pc::TMB(thisCrate, thisChamber, 26);
  //
  tmb_vme_ready = -1;
  //
  // Put CCB in FPGA mode to make the CCB ignore TTC commands (such as hard reset) during TMB downloading...
  thisCCB->setCCBMode(emu::pc::CCB::VMEFPGA);
  //
  std::cout << "Broadcast TMB firmware to slot " << thisTMB->slot() << " in 5 seconds..." << std::endl;
  //
  ::sleep(5);
  //
  thisTMB->SetXsvfFilename(TMBFirmware_[tmb].toString().c_str());
  thisTMB->ProgramTMBProms();
  thisTMB->ClearXsvfFilename();
  //
  std::cout << "Please perform a TTC/CCB hard reset to Load FPGA"<< std::endl;
  //
  // Put CCB back into DLOG mode to listen to TTC commands...
  thisCCB->setCCBMode(emu::pc::CCB::DLOG);
  //
  this->TMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::CCBHardResetFromTMBPage(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  thisCCB->hardReset();
  //
  this->TMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::CheckTMBFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Checking TMB VME Ready for all slots in crate" << std::endl;
  //
  tmb_vme_ready = 1;
  //
  for (unsigned tmb=0; tmb<tmbVector.size(); tmb++) {
    //
    emu::pc::TMB * thisTMB = tmbVector[tmb];
    //
    if (thisTMB->slot() < 22) {
      short unsigned int BootReg;
      thisTMB->tmb_get_boot_reg(&BootReg);
      std::cout << "Boot register = 0x" << std::hex << BootReg << std::endl;
      //
      if (thisTMB->GetBootVMEReady() != 1) tmb_vme_ready = 0;
    }
    //
  }
  //
  this->TMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::ClearTMBBootReg(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  if (tmb_vme_ready == 1) {
    //
    for (unsigned tmb=0; tmb<tmbVector.size(); tmb++) {
      //
      emu::pc::TMB * thisTMB = tmbVector[tmb];
      //
      if (thisTMB->slot() < 22) {
	short unsigned int BootReg;
	thisTMB->tmb_get_boot_reg(&BootReg);
	BootReg &= 0xff7f;                    // Give JTAG chain to the FPGA to configure ALCT on hard reset
	BootReg &= 0xf7ff;                    // Allow FPGA access to the VME register
	thisTMB->tmb_set_boot_reg(BootReg);
	//
      }
    }
  } else {
    //
    std::cout << "TMB is not ready for VME access" << std::endl;
  }
  //
  this->TMBUtils(in,out);
  //
}
//
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
    std::cout << "Load Firmware for ALCT[" << tmb << "]" << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "No ALCT defined to load taking default = " << TMB_ << std::endl ;
    tmb = TMB_;
  }
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
  emu::pc::ALCTController  * thisALCT = thisTMB->alctController();
  //
  if (!thisALCT) {
    std::cout << "This ALCT not defined" << std::endl;
    return;
  }
  //
  // Put CCB in FPGA mode to make the CCB ignore TTC commands (such as hard reset) during ALCT downloading...
  thisCCB->setCCBMode(emu::pc::CCB::VMEFPGA);
  //
  LOG4CPLUS_INFO(getApplicationLogger(), "Program ALCT firmware");
  //
  std::cout <<  "Loading ALCT firmware to slot " << thisTMB->slot() 
	    << " with " << ALCTFirmware_[tmb].toString() 
	    << " in 5 seconds...  Current firmware types are:" << std::endl;
  //
  thisALCT->ReadSlowControlId();
  thisALCT->PrintSlowControlId();
  //
  thisALCT->ReadFastControlId();
  thisALCT->PrintFastControlId();
  //
  ::sleep(5);
  //
  thisTMB->disableAllClocks();
  //
  int debugMode(0);
  int jch(3);
  int status = thisALCT->SVFLoad(&jch,ALCTFirmware_[tmb].toString().c_str(),debugMode);
  //
  thisTMB->enableAllClocks();
  //
  if (status >= 0){
    LOG4CPLUS_INFO(getApplicationLogger(), "Program ALCT firmware finished");
    std::cout << "=== Programming finished"<< std::endl;
    //	cout << "=== " << status << " Verify Errors  occured" << std::endl;
  } else {
    std::cout << "=== Fatal Error. Exiting with " << status << std::endl;
  }
  //
  // Put CCB back into DLOG mode to listen to TTC commands...
  thisCCB->setCCBMode(emu::pc::CCB::DLOG);
  //
  this->TMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::LoadCrateALCTFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "Load ALCT firmware crate, called from TMB[" << tmb << "]" << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "No ALCT defined to load... taking default = " << TMB_ << std::endl ;
    tmb = TMB_;
  }
  //
  // Put CCB in FPGA mode to make the CCB ignore TTC commands (such as hard reset) during ALCT downloading...
  thisCCB->setCCBMode(emu::pc::CCB::VMEFPGA);
  //
  for (unsigned i=0; i<tmbVector.size(); i++) {
    emu::pc::TMB * thisTMB = tmbVector[i];
    emu::pc::ALCTController  * thisALCT = thisTMB->alctController();
    //
    if (!thisALCT) {
      std::cout << "This ALCT not defined" << std::endl;
      return;
    }
    //
    LOG4CPLUS_INFO(getApplicationLogger(), "Program ALCT firmware");
    //
    std::cout <<  "Loading ALCT firmware to slot " << std::dec << thisTMB->slot() 
	      << " with " << ALCTFirmware_[i].toString() 
	      << " in 5 seconds...  Current firmware types are:" << std::endl;
    //
    thisALCT->ReadSlowControlId();
    thisALCT->PrintSlowControlId();
    //
    thisALCT->ReadFastControlId();
    thisALCT->PrintFastControlId();
    //
    ::sleep(5);
    //
    thisTMB->disableAllClocks();
    //
    int debugMode(0);
    int jch(3);
    int status = thisALCT->SVFLoad(&jch,ALCTFirmware_[i].toString().c_str(),debugMode);
    //
    thisTMB->enableAllClocks();
    //
    if (status >= 0){
      LOG4CPLUS_INFO(getApplicationLogger(), "Program ALCT firmware finished");
      std::cout << "=== Programming finished"<< std::endl;
      //	cout << "=== " << status << " Verify Errors  occured" << std::endl;
    } else {
      std::cout << "=== Fatal Error. Exiting with " << status << std::endl;
    }
  }
  //
  // Put CCB back into DLOG mode to listen to TTC commands...
  thisCCB->setCCBMode(emu::pc::CCB::DLOG);
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
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "Not tmb" << std::endl ;
    tmb = TMB_;
  }
  //
  emu::pc::TMB * thisTMB = tmbVector[tmb];
  //
  std::cout << "Loading RAT firmware " << std::endl;
  //
  rat = thisTMB->getRAT();
  if (!rat) {
    std::cout << "No RAT present" << std::endl;
    return;
  }
  //
  thisTMB->disableAllClocks();
  //
  int debugMode(0);
  int jch(7);
  int status = rat->SVFLoad(&jch,RATFirmware_[tmb].toString().c_str(),debugMode);
  //
  thisTMB->enableAllClocks();
  //
  if (status >= 0){
    std::cout << "=== Programming finished"<< std::endl;
    //    std::cout << "=== " << status << " Verify Errors  occured" << std::endl;
  }
  else{
    std::cout << "=== Fatal Error. Exiting with " <<  status << std::endl;
  }
  //
  this->TMBUtils(in,out);
  //
}
//
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
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "Not dmb" << std::endl ;
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
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "Not tmb" << std::endl ;
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
    std::cout << "RAT " << rat << std::endl;
    RAT_ = rat;
  } else {
    std::cout << "Not rat" << std::endl ;
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
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "Not dmb" << std::endl ;
    dmb = DMB_;
  }
  //
  emu::pc::DAQMB * thisDMB = dmbVector[dmb];
  //
  bool isME13 = false;
  emu::pc::TMB * thisTMB   = tmbVector[dmb];
  emu::pc::ALCTController * thisALCT;
  if (thisTMB) 
    thisALCT = thisTMB->alctController();
  if (thisALCT) 
    if ( (thisALCT->GetChamberType()).find("ME13") != std::string::npos )
      isME13 = true;
  //
  emu::pc::Chamber * thisChamber = chamberVector[dmb];
      std::string chamber=thisChamber->GetLabel();
      unsigned long int cfebID[5], cfebIDread[5];
      std::vector <emu::pc::CFEB> thisCFEBs=thisDMB->cfebs();
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
    *out << cgicc::a("CFEB Status").set("href",CFEBStatus) << std::endl;
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
  if ( thisDMB->CheckVMEFirmwareVersion() ) {
    *out << cgicc::span().set("style","color:green");
    *out << buf;
    *out << "...OK...";
    *out << cgicc::span();
  } else {
    *out << cgicc::span().set("style","color:red");
    *out << buf;
    *out << "--->> BAD <<--- should be "
	 << std::dec << thisDMB->GetExpectedVMEFirmwareTag();
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
  
  if ( thisDMB->CheckControlFirmwareVersion() ) {
    *out << cgicc::span().set("style","color:green");
    *out << buf;
    *out << "...OK...";
    *out << cgicc::span();
  } else {
    *out << cgicc::span().set("style","color:red");
    *out << buf;
    *out << "--->> BAD <<--- should be "
	 << std::hex << thisDMB->GetExpectedControlFirmwareTag();
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
  *out << std::endl ;
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << std::endl ;
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
  if ( value/1000. < 0.85 ||
       value/1000. > 1.20 ) {
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");
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
  if ( value/1000. < 0.85 ||
       value/1000. > 1.2 ) {
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");      
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
  if (  value/1000. < 0.85 ||
	value/1000. > 1.2 ) {
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");      
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
  if ( value/1000. < 0.85 ||
       value/1000. > 1.2 ) {
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");  
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
  if ( value/1000. < 0.85 ||
       value/1000. > 1.2 ) {
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");  
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
      std::cout << "Register " << registerValue << std::endl;
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
    thisCCB->hardReset();
    // thisCCB->HardReset_crate();
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
      std::cout << "DMB " << dmb << std::endl;
      DMB_ = dmb;
    }
    //
    emu::pc::DAQMB * thisDMB = dmbVector[dmb];
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
    std::cout << "LogDMBTestsOutput" << std::endl;
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
      std::cout << "DMB " << dmb << std::endl;
      DMB_ = dmb;
    } else {
      std::cout << "Not dmb" << std::endl ;
      dmb = DMB_;
    }
    //
    cgicc::form_iterator name2 = cgi.getElement("ClearDMBTestsOutput");
    //
    if(name2 != cgi.getElements().end()) {
      std::cout << "Clear..." << std::endl;
      std::cout << cgi["ClearDMBTestsOutput"]->getValue() << std::endl ;
      OutputDMBTests[dmb][current_crate_].str("");
      OutputDMBTests[dmb][current_crate_] << "DMB-CFEB Tests " 
					  << thisCrate->GetChamber(dmbVector[dmb]->slot())->GetLabel().c_str() 
					  << " output:" << std::endl;

      //
    this->DMBTests(in,out);
    return;
    }
    //
    emu::pc::DAQMB * thisDMB = dmbVector[dmb];
    //
    std::cout << DMBBoardID_[dmb] << std::endl ;
    //
    char buf[20];
    sprintf(buf,"DMBTestsLogFile_%d_%s.log",thisDMB->slot(),DMBBoardID_[dmb].c_str());
    //
    std::ofstream DMBTestsLogFile;
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
    while( time.find(" ",0) != std::string::npos ) {
      //
      int thispos = time.find(" ",0); 
      time.replace(thispos,1,"_");
      //
    }
    //
    //    buf = "EmuPeripheralCrateTestSummary_"+RunNumber_+"_"+Operator_+time+".log";
    buf = "EmuPeripheralCrateTestSummary_"+RunNumber_+"_"+Operator_+".log";
    //
    std::ofstream LogFile;
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
      //      emu::pc::DAQMB * thisDMB = dmbVector[dmbctr];
      //      vector<emu::pc::CFEB> thisCFEBs = thisDMB->cfebs();
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
	emu::pc::Chamber * thisChamber = chamberVector[i];
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
    std::ifstream TextFile ;
    TextFile.open(xmlFile_.toString().c_str());
    //
    std::ofstream LogFile;
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
    std::cout << "LogTMBTestsOutput" << std::endl;
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    //
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      std::cout << "TMB " << tmb << std::endl;
      TMB_ = tmb;
    } else {
      std::cout << "Not tmb" << std::endl ;
      tmb = TMB_;
    }
    //
    cgicc::form_iterator name2 = cgi.getElement("ClearTMBTestsOutput");
    //
    if(name2 != cgi.getElements().end()) {
      std::cout << "Clear..." << std::endl;
      std::cout << cgi["ClearTMBTestsOutput"]->getValue() << std::endl ;
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
    emu::pc::TMB * thisTMB = tmbVector[tmb];
    //
    std::cout << TMBBoardID_[tmb] << std::endl ;
    //
    char buf[20];
    sprintf(buf,"TMBTestsLogFile_%d_%s.log",thisTMB->slot(),TMBBoardID_[tmb].c_str());
    //
    std::ofstream TMBTestsLogFile;
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
    std::cout << "LogChamberTestsOutput" << std::endl;
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    //
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      std::cout << "TMB " << tmb << std::endl;
      TMB_ = tmb;
    } else {
      std::cout << "Not tmb" << std::endl ;
      tmb = TMB_;
    }
    //
    cgicc::form_iterator name2 = cgi.getElement("ClearChamberTestsOutput");
    //
    if(name2 != cgi.getElements().end()) {
      std::cout << "Clear..." << std::endl;
      std::cout << cgi["ClearChamberTestsOutput"]->getValue() << std::endl ;
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
    emu::pc::TMB * thisTMB = tmbVector[tmb];
    //
    std::cout << TMBBoardID_[tmb] << std::endl ;
    //
    char buf[20];
    sprintf(buf,"ChamberTestsLogFile_%d_%s.log",thisTMB->slot(),TMBBoardID_[tmb].c_str());
    //
    std::ofstream ChamberTestsLogFile;
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
    std::cout << "LogCrateTestsOutput" << std::endl;
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name2 = cgi.getElement("ClearCrateTestsOutput");
    //
    if(name2 != cgi.getElements().end()) {
      std::cout << "Clear..." << std::endl;
      std::cout << cgi["ClearCrateTestsOutput"]->getValue() << std::endl ;
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
    std::ofstream CrateTestsLogFile;
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
    if(!(crateVector[cv]->IsAlive())) continue;
    SetCurrentCrate(cv);
    std::cout << "For Crate " << ThisCrateID_ << " : " << std::endl;

  for (unsigned idmb=0;idmb<dmbVector.size();idmb++) {
    //
    if ((dmbVector[idmb]->slot())<22) {
      emu::pc::DAQMB * thisDMB=dmbVector[idmb];
      unsigned long int boardnumber=thisDMB->mbpromuser(0);
      DMBBoardNumber[cv][idmb]=boardnumber;
      std::cout <<" The DMB Number: "<<idmb<<" is in Slot Number: "<<dmbVector[idmb]->slot()<<std::endl;
      std::cout <<" This DMB Board Number: "<<DMBBoardNumber[cv][idmb]<<std::endl<<std::endl;
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
  if(!(crateVector[cv]->IsAlive())) continue;
    SetCurrentCrate(cv);
    std::cout << "For Crate " << ThisCrateID_ << " : " << std::endl;

  usleep(200);
  for (unsigned idmb=0;idmb<dmbVector.size();idmb++) {
    //
    if ((dmbVector[idmb]->slot())<22) {
      emu::pc::DAQMB * thisDMB=dmbVector[idmb];
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
	std::cout<<" DMB board number reprogram from Database ..."<<std::endl;
      }
      //temperarily overwrite all board number using database
      //	prombrdname[0]=dmbID&0xff;
      //	prombrdname[1]=(dmbID>>8)&0x0f;

        std::cout<<" Loading the board number ..."<<(prombrdname[0]&0xff)+((prombrdname[1]<<8)&0xf00)<<" was set to: "<<(boardnumber&0xffff)<<std::endl;

      //
      std::string DMBVmeFirmware = FirmwareDir_+DMBVME_FIRMWARE_FILENAME;
      //
      thisDMB->epromload_broadcast(VPROM,DMBVmeFirmware.c_str(),1,prombrdname,2);
      usleep(200);
      std::cout <<" The DMB Number: "<<idmb<<" is in Slot Number: "<<dmbVector[idmb]->slot()<<std::endl;
      //  std::cout <<" This DMB is programmed to board number: "<<boardnumber<<std::endl<<std::endl;
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
  if(!(crateVector[cv]->IsAlive())) continue;
    SetCurrentCrate(cv);
    std::cout << "For Crate " << ThisCrateID_ << " : " << std::endl;

  usleep(200);
  for (unsigned idmb=0;idmb<dmbVector.size();idmb++) {
    //
    if ((dmbVector[idmb]->slot())<22) {
      emu::pc::DAQMB * thisDMB=dmbVector[idmb];
      std::cout <<" The DMB Number: "<<idmb<<" is in Slot Number: "<<dmbVector[idmb]->slot()<<std::endl;
      //loop over the cfebs
      //define CFEBs
      std::vector <emu::pc::CFEB> thisCFEBs=thisDMB->cfebs();
      //
      for (unsigned i=0;i<thisCFEBs.size();i++) {
	CFEBBoardNumber[cv][idmb][i]=thisDMB->febpromuser(thisCFEBs[i]);
	std::cout <<" This CFEB Board Number: "<<CFEBBoardNumber[cv][idmb][i]<<std::endl;
      }
      //
      std::cout <<std::endl;
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
    if(!(crateVector[cv]->IsAlive())) continue;
    SetCurrentCrate(cv);
    std::cout << "For Crate " << ThisCrateID_ << " : " << std::endl;

  for (unsigned idmb=0;idmb<dmbVector.size();idmb++) {
    //
    if ((dmbVector[idmb]->slot())<22) {
      emu::pc::DAQMB * thisDMB=dmbVector[idmb];
      emu::pc::Chamber * thisChamber=chamberVector[idmb];
      std::cout <<" The DMB Number: "<<idmb<<" is in Slot Number: "<<dmbVector[idmb]->slot()<<std::endl;
      //loop over the cfebs
      //define CFEBs
      std::vector <emu::pc::CFEB> thisCFEBs=thisDMB->cfebs();
      //
      for (unsigned i=0;i<thisCFEBs.size();i++) {
	char promid[4];
	unsigned long int boardid=CFEBBoardNumber[cv][idmb][i];
	/*
	  unsigned long int fpgaid=thisDMB->febfpgaid(thisCFEBs[i]);
	  std::cout <<" i= "<<i<<std::endl;
	  if (i==0) boardid=0xcfeb08e5;
	  if (i==1) boardid=0xcfeb08e1;
	  if (i==2) boardid=0xcfeb08e4;
	  if (i==3) boardid=0xcfeb0903;
	  if (i==4) boardid=0xcfeb063a;
	  std::cout <<" This CFEB Board Number should be set to: "<<boardid<<std::endl;
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
	   std::cout<<" CFEB board number reprogram from Database ..."<<std::endl;
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
	        std::cout <<" This CFEB Board Number is set to: "<<newcfebid;
          std::cout <<"     was set to: "<<(boardid&0xffff)<<std::endl;
	std::cout <<" This CFEB Board Number is set to: CFEB"<<std::hex<<((promid[1])&0xff)<<((promid[0])&0xff)<<" was set to: "<<std::hex<<boardid<<std::endl;
      }
      std::cout <<std::endl;
    }
    //
  }

  }
  SetCurrentCrate(this_crate_no_);

  return createReply(message);
}
// sending and receiving soap commands
////////////////////////////////////////////////////////////////////
    void EmuPeripheralCrateConfig::PCsendCommand(std::string command, std::string klass)
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

xoap::MessageReference EmuPeripheralCrateConfig::PCcreateCommandSOAP(std::string command) {
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
