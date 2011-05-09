#include "emu/pc/EmuPeripheralCrateConfig.h"

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <unistd.h> // for sleep()
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <time.h>

namespace emu {
  namespace pc {

const std::string       CFEB_FIRMWARE_FILENAME = "cfeb/cfeb_pro.svf";
const std::string       CFEB_VERIFY_FILENAME = "cfeb/cfeb_verify.svf";
//
const std::string       DMB_FIRMWARE_FILENAME    = "dmb/dmb6cntl_pro.svf";
const std::string       DMBVME_FIRMWARE_FILENAME = "dmb/dmb6vme_pro.svf";
//
//In order to load firmware automatically from the firmware values in the xml files, 
//the firmware needs to reside in directories in the form:
//    TMB  ->  $HOME/firmware/tmb/YEARMONTHDAY/type[A,C,D]/tmb.xsvf   <-- N.B. xsvf format for TMB
//    RAT  ->  $HOME/firmware/rat/YEARMONTHDAY/rat.svf
//    ALCT ->  $HOME/firmware/alct/YEARMONTHDAY/alctXXX/alctXXX.xsvf
// with the zero-values filled in with 0's.  
// In other words:  9 April 2007 firmware should reside in YEARMONTHDAY=20070409
//
// The XXX in the ALCT firmware specification corresponds to the following structure:
const std::string ALCT_FIRMWARE_FILENAME_ME11 = "alct288/alct288";
const std::string ALCT_READBACK_FILENAME_ME11 = "alct288/alct288_verify";
//
const std::string ALCT_FIRMWARE_FILENAME_ME11_BACKWARD_NEGATIVE = "alct288bn/alct288bn";
const std::string ALCT_READBACK_FILENAME_ME11_BACKWARD_NEGATIVE = "alct288bn/alct288bn_verify";
//
const std::string ALCT_FIRMWARE_FILENAME_ME11_BACKWARD_POSITIVE = "alct288bp/alct288bp";
const std::string ALCT_READBACK_FILENAME_ME11_BACKWARD_POSITIVE = "alct288bp/alct288bp_verify";
//
const std::string ALCT_FIRMWARE_FILENAME_ME11_FORWARD_POSITIVE  = "alct288fp/alct288fp";
const std::string ALCT_READBACK_FILENAME_ME11_FORWARD_POSITIVE  = "alct288fp/alct288fp_verify";
//
const std::string ALCT_FIRMWARE_FILENAME_ME12 = "alct384/alct384"; 
const std::string ALCT_READBACK_FILENAME_ME12 = "alct384/alct384_verify";
//
const std::string ALCT_FIRMWARE_FILENAME_ME13 = "alct192/alct192"; 
const std::string ALCT_READBACK_FILENAME_ME13 = "alct192/alct192_verify";
//
const std::string ALCT_FIRMWARE_FILENAME_ME21 = "alct672/alct672";
const std::string ALCT_READBACK_FILENAME_ME21 = "alct672/alct672_verify";
//
const std::string ALCT_FIRMWARE_FILENAME_ME22 = "alct384/alct384";
const std::string ALCT_READBACK_FILENAME_ME22 = "alct384/alct384_verify";
//
const std::string ALCT_FIRMWARE_FILENAME_ME31 = "alct576mirror/alct576mirror";
const std::string ALCT_READBACK_FILENAME_ME31 = "alct576mirror/alct576mirror_verify";
//
const std::string ALCT_FIRMWARE_FILENAME_ME32 = "alct384mirror/alct384mirror";
const std::string ALCT_READBACK_FILENAME_ME32 = "alct384mirror/alct384mirror_verify";
//
const std::string ALCT_FIRMWARE_FILENAME_ME41 = "alct576mirror/alct576mirror";
const std::string ALCT_READBACK_FILENAME_ME41 = "alct576mirror/alct576mirror_verify";
//
const std::string ALCT_FIRMWARE_FILENAME_ME42 = "alct384mirror/alct384mirror";
const std::string ALCT_READBACK_FILENAME_ME42 = "alct384mirror/alct384mirror_verify";
//
// Old svf files (to be deprecated once it is verified that the xsvf firmware is working...
//const std::string ALCT_FIRMWARE_FILENAME_ME11 = "alct288/alct288.svf"; //
//const std::string ALCT_READBACK_FILENAME_ME11 = "readback-192-288-384";//
//
//const std::string ALCT_FIRMWARE_FILENAME_ME11_BACKWARD_NEGATIVE = "alct288bn/alct288bn.svf";//
//const std::string ALCT_READBACK_FILENAME_ME11_BACKWARD_NEGATIVE = "readback-192-288-384";   //
//
//const std::string ALCT_FIRMWARE_FILENAME_ME11_BACKWARD_POSITIVE = "alct288bp/alct288bp.svf";//
//const std::string ALCT_READBACK_FILENAME_ME11_BACKWARD_POSITIVE = "readback-192-288-384";   //
//
//const std::string ALCT_FIRMWARE_FILENAME_ME11_FORWARD_POSITIVE  = "alct288fp/alct288fp.svf";//
//const std::string ALCT_READBACK_FILENAME_ME11_FORWARD_POSITIVE  = "readback-192-288-384";   //
//
//const std::string ALCT_FIRMWARE_FILENAME_ME12 = "alct384/alct384.svf"; //
//const std::string ALCT_READBACK_FILENAME_ME12 = "readback-192-288-384";//
//
//const std::string ALCT_FIRMWARE_FILENAME_ME13 = "alct192/alct192.svf"; //
//const std::string ALCT_READBACK_FILENAME_ME13 = "readback-192-288-384";//
//
//const std::string ALCT_FIRMWARE_FILENAME_ME21 = "alct672/alct672.svf";//
//const std::string ALCT_READBACK_FILENAME_ME21 = "readback-576-672";   //
//
//const std::string ALCT_FIRMWARE_FILENAME_ME22 = "alct384/alct384.svf"; //
//const std::string ALCT_READBACK_FILENAME_ME22 = "readback-192-288-384";//
//
//const std::string ALCT_FIRMWARE_FILENAME_ME31 = "alct576mirror/alct576mirror.svf";//
//const std::string ALCT_READBACK_FILENAME_ME31 = "readback-576-672";               //
//
//const std::string ALCT_FIRMWARE_FILENAME_ME32 = "alct384mirror/alct384mirror.svf";//
//const std::string ALCT_READBACK_FILENAME_ME32 = "readback-192-288-384";           //
//  
//const std::string ALCT_FIRMWARE_FILENAME_ME41 = "alct576mirror/alct576mirror.svf";//
//const std::string ALCT_READBACK_FILENAME_ME41 = "readback-576-672";               //
//
const int CCB_LABEL         = 0;
const int MPC_LABEL         = 1;
const int TMB_LABEL         = 2;
const int ALCT_LABEL        = 3;
const int DMB_VME_LABEL     = 4;
const int DMB_CONTROL_LABEL = 5;
const int CFEB_LABEL[5]     = {6, 7, 8, 9, 10};
//
/////////////////////////////////////////////////////////////////////
// Instantiation and main page
/////////////////////////////////////////////////////////////////////
EmuPeripheralCrateConfig::EmuPeripheralCrateConfig(xdaq::ApplicationStub * s): EmuPeripheralCrateBase(s)
{	
  //
  FirmwareDir_ = getenv("HOME");
  FirmwareDir_ += "/firmware/";
  XMLDIR = "/opt/xdaq/htdocs/emu/emuDCS/PeripheralApps/xml";
  //
  DisplayRatio_ = false;
  AutoRefresh_  = true;
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
      bc0_sync[i][j] = -1;
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
      dmbcfg_ok[i][j]  = -1;
      dmb_config_ok[i][j]  = -1;
      alctcfg_ok[i][j]  = -1;
      alct_lvmb_current_ok[i][j]  = -1;
      alct_adc_current_ok[i][j]  = -1;
      //
      for (int k=0; k<5; k++) {
	cfeb_firmware_ok[i][j][k] = -1;
	cfeb_config_ok[i][j][k]   = -1;
	cfeb_current_ok[i][j][k]  = -1;
      }
    }
  }
  //
  firmware_checked_ = 0;
  number_of_checks_ = 4;
  number_of_hard_resets_ = number_of_checks_ - 1;
  //
  xgi::bind(this,&EmuPeripheralCrateConfig::Default, "Default");
  xgi::bind(this,&EmuPeripheralCrateConfig::MainPage, "MainPage");
  xgi::bind(this,&EmuPeripheralCrateConfig::setConfFile, "setConfFile");
  //
  xgi::bind(this,&EmuPeripheralCrateConfig::ConfigOneCrate, "ConfigOneCrate");
  //
  //------------------------------------------------------
  // bind buttons -> Crate Configuration pages
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
  xgi::bind(this,&EmuPeripheralCrateConfig::CheckBC0Synchronization,"CheckBC0Synchronization");
  xgi::bind(this,&EmuPeripheralCrateConfig::CheckCratesConfiguration, "CheckCratesConfiguration");
  xgi::bind(this,&EmuPeripheralCrateConfig::CheckCratesConfigurationFull, "CheckCratesConfigurationFull");
  xgi::bind(this,&EmuPeripheralCrateConfig::CheckCrateConfiguration, "CheckCrateConfiguration");
  xgi::bind(this,&EmuPeripheralCrateConfig::CheckCrateFirmware, "CheckCrateFirmware");
  //
  //---------------------------------
  // bind check firmware
  //---------------------------------
  xgi::bind(this,&EmuPeripheralCrateConfig::PowerOnFixCFEB, "PowerOnFixCFEB");
  xgi::bind(this,&EmuPeripheralCrateConfig::CheckFirmware, "CheckFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::FixCFEB, "FixCFEB");
  xgi::bind(this,&EmuPeripheralCrateConfig::SetNumberOfHardResets, "SetNumberOfHardResets");
  xgi::bind(this,&EmuPeripheralCrateConfig::ReadbackALCTFirmware, "ReadbackALCTFirmware");
  //
  //---------------------------------
  // bind Expert Tools pages
  //---------------------------------
  xgi::bind(this,&EmuPeripheralCrateConfig::ExpertToolsPage,"ExpertToolsPage");
  xgi::bind(this,&EmuPeripheralCrateConfig::StartPRBS, "StartPRBS");
  xgi::bind(this,&EmuPeripheralCrateConfig::StopPRBS, "StopPRBS");
  xgi::bind(this,&EmuPeripheralCrateConfig::SetRadioactivityTrigger, "SetRadioactivityTrigger");
  xgi::bind(this,&EmuPeripheralCrateConfig::SetRadioactivityTriggerALCTOnly, "SetRadioactivityTriggerALCTOnly");
  xgi::bind(this,&EmuPeripheralCrateConfig::SetTTCDelays, "SetTTCDelays");
  xgi::bind(this,&EmuPeripheralCrateConfig::MeasureAllTMBVoltages, "MeasureAllTMBVoltages");
  xgi::bind(this,&EmuPeripheralCrateConfig::SetTwoLayerTriggerForSystem, "SetTwoLayerTriggerForSystem");
  xgi::bind(this,&EmuPeripheralCrateConfig::MeasureL1AsAndDAVsForSystem,"MeasureL1AsAndDAVsForSystem");
  xgi::bind(this,&EmuPeripheralCrateConfig::MeasureALCTTMBRxTxForSystem,"MeasureALCTTMBRxTxForSystem");
  xgi::bind(this,&EmuPeripheralCrateConfig::MeasureCFEBTMBRxForSystem,"MeasureCFEBTMBRxForSystem");
  xgi::bind(this,&EmuPeripheralCrateConfig::QuickScanForSystem,"QuickScanForSystem");
  //
  //------------------------------
  // bind crate utilities
  //------------------------------
  xgi::bind(this,&EmuPeripheralCrateConfig::TmbMPCTest, "TmbMPCTest");
  xgi::bind(this,&EmuPeripheralCrateConfig::MPCSafeWindowScan, "MPCSafeWindowScan");
  //
  xgi::bind(this,&EmuPeripheralCrateConfig::CheckCrates, "CheckCrates");
  xgi::bind(this,&EmuPeripheralCrateConfig::CrateSelection, "CrateSelection");
  xgi::bind(this,&EmuPeripheralCrateConfig::setRawConfFile, "setRawConfFile");
  xgi::bind(this,&EmuPeripheralCrateConfig::UploadConfFile, "UploadConfFile");
  xgi::bind(this,&EmuPeripheralCrateConfig::DefineConfiguration, "DefineConfiguration");

  xgi::bind(this,&EmuPeripheralCrateConfig::testTMB, "testTMB");
  xgi::bind(this,&EmuPeripheralCrateConfig::CrateConfiguration, "CrateConfiguration");
  xgi::bind(this,&EmuPeripheralCrateConfig::CrateTests, "CrateTests");
  xgi::bind(this,&EmuPeripheralCrateConfig::ChamberTests, "ChamberTests");
  xgi::bind(this,&EmuPeripheralCrateConfig::ConfigAllCrates, "ConfigAllCrates");
  xgi::bind(this,&EmuPeripheralCrateConfig::FastConfigCrates, "FastConfigCrates");
  xgi::bind(this,&EmuPeripheralCrateConfig::FastConfigOne, "FastConfigOne");
  xgi::bind(this,&EmuPeripheralCrateConfig::ConfigIDSelection, "ConfigIDSelection");
  //
  xgi::bind(this,&EmuPeripheralCrateConfig::MeasureL1AsAndDAVsForCrate,"MeasureL1AsAndDAVsForCrate");
  xgi::bind(this,&EmuPeripheralCrateConfig::MeasureL1AsAndDAVsForChamber,"MeasureL1AsAndDAVsForChamber");
  xgi::bind(this,&EmuPeripheralCrateConfig::MeasureL1AsForCrate,"MeasureL1AsForCrate");
  xgi::bind(this,&EmuPeripheralCrateConfig::MeasureDAVsForCrate,"MeasureDAVsForCrate");
  xgi::bind(this,&EmuPeripheralCrateConfig::MeasureALCTTMBRxTxForCrate,"MeasureALCTTMBRxTxForCrate");
  xgi::bind(this,&EmuPeripheralCrateConfig::MeasureCFEBTMBRxForCrate,"MeasureCFEBTMBRxForCrate");
  xgi::bind(this,&EmuPeripheralCrateConfig::SetTwoLayerTriggerForCrate, "SetTwoLayerTriggerForCrate");
  xgi::bind(this,&EmuPeripheralCrateConfig::QuickScanForChamber,"QuickScanForChamber");
  xgi::bind(this,&EmuPeripheralCrateConfig::QuickScanForCrate,"QuickScanForCrate");
  //
  //-----------------------------------------------
  // CCB & MPC routines
  //-----------------------------------------------
  xgi::bind(this,&EmuPeripheralCrateConfig::ReadCCBRegister, "ReadCCBRegister");
  xgi::bind(this,&EmuPeripheralCrateConfig::WriteCCBRegister, "WriteCCBRegister");
  xgi::bind(this,&EmuPeripheralCrateConfig::ReadMPCRegister, "ReadMPCRegister");
  xgi::bind(this,&EmuPeripheralCrateConfig::WriteMPCRegister, "WriteMPCRegister");
  xgi::bind(this,&EmuPeripheralCrateConfig::MPCLoadFirmware, "MPCLoadFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::ReadTTCRegister, "ReadTTCRegister");
  xgi::bind(this,&EmuPeripheralCrateConfig::HardReset, "HardReset");
  xgi::bind(this,&EmuPeripheralCrateConfig::CCBLoadFirmware, "CCBLoadFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::CCBConfig, "CCBConfig");
  xgi::bind(this,&EmuPeripheralCrateConfig::MPCConfig, "MPCConfig");
  xgi::bind(this,&EmuPeripheralCrateConfig::CCBReadFirmware, "CCBReadFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::MPCReadFirmware, "MPCReadFirmware");
  //
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
  xgi::bind(this,&EmuPeripheralCrateConfig::RTRGlow, "RTRGlow");
  xgi::bind(this,&EmuPeripheralCrateConfig::RTRGhigh, "RTRGhigh");

  //
  //-----------------------------------------------
  // DMB utilities
  //-----------------------------------------------
  xgi::bind(this,&EmuPeripheralCrateConfig::CFEBStatus, "CFEBStatus");
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBPrintCounters, "DMBPrintCounters");
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBTurnOff, "DMBTurnOff");
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBTurnOn, "DMBTurnOn");
  xgi::bind(this,&EmuPeripheralCrateConfig::CFEBTurnOn, "CFEBTurnOn");
  xgi::bind(this,&EmuPeripheralCrateConfig::MPCMask, "MPCMask");
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBLoadFirmware, "DMBLoadFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBVmeLoadFirmware, "DMBVmeLoadFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::DMBVmeLoadFirmwareEmergency, "DMBVmeLoadFirmwareEmergency");
  xgi::bind(this,&EmuPeripheralCrateConfig::CFEBLoadFirmware, "CFEBLoadFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::CCBHardResetFromDMBPage, "CCBHardResetFromDMBPage");
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
  xgi::bind(this,&EmuPeripheralCrateConfig::UnjamTMB, "UnjamTMB");  
  xgi::bind(this,&EmuPeripheralCrateConfig::CheckAbilityToLoadALCT, "CheckAbilityToLoadALCT");
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
  xgi::bind(this,&EmuPeripheralCrateConfig::TMBResetSyncError, "TMBResetSyncError");
  xgi::bind(this,&EmuPeripheralCrateConfig::TMBRawHits, "TMBRawHits");
  xgi::bind(this,&EmuPeripheralCrateConfig::ALCTRawHits, "ALCTRawHits");
  xgi::bind(this,&EmuPeripheralCrateConfig::TMBReadFirmware, "TMBReadFirmware");
  xgi::bind(this,&EmuPeripheralCrateConfig::ALCTReadFirmware, "ALCTReadFirmware");
  //
  //----------------------------
  // Bind logging methods
  //----------------------------
  xgi::bind(this,&EmuPeripheralCrateConfig::LogChamberTestsOutput, "LogChamberTestsOutput");
  xgi::bind(this,&EmuPeripheralCrateConfig::LogCrateTestsOutput, "LogCrateTestsOutput");
  xgi::bind(this,&EmuPeripheralCrateConfig::LogALCT_TMB_communicationOutput, "LogALCT_TMB_communicationOutput");
  xgi::bind(this,&EmuPeripheralCrateConfig::LogOutput, "LogOutput");
  xgi::bind(this,&EmuPeripheralCrateConfig::LogTestSummary, "LogTestSummary");
  xgi::bind(this,&EmuPeripheralCrateConfig::LogDMBTestsOutput, "LogDMBTestsOutput");
  xgi::bind(this,&EmuPeripheralCrateConfig::LogTMBTestsOutput, "LogTMBTestsOutput");
  //
  //----------------------------------------------
  // Bind BC0 methods
  //----------------------------------------------
  xgi::bind(this,&EmuPeripheralCrateConfig::ALCTBC0Scan, "ALCTBC0Scan");
  xgi::bind(this,&EmuPeripheralCrateConfig::ALCTBC0ScanForCrate,"ALCTBC0ScanForCrate");
  xgi::bind(this,&EmuPeripheralCrateConfig::ALCTBC0ScanForSystem,"ALCTBC0ScanForSystem");
  xgi::bind(this,&EmuPeripheralCrateConfig::Settmb_bxn_offset, "Settmb_bxn_offset");
  //
  //----------------------------------------------------
  // Bind phase determination (commmunication)  methods
  //----------------------------------------------------
  xgi::bind(this,&EmuPeripheralCrateConfig::ALCT_TMB_communication, "ALCT_TMB_communication");
  xgi::bind(this,&EmuPeripheralCrateConfig::ALCT_TMB_Loopback, "ALCT_TMB_Loopback");
  xgi::bind(this,&EmuPeripheralCrateConfig::TMB_to_ALCT_walking_ones, "TMB_to_ALCT_walking_ones");
  xgi::bind(this,&EmuPeripheralCrateConfig::CFEBTiming, "CFEBTiming");
  xgi::bind(this,&EmuPeripheralCrateConfig::RatTmbTiming, "RatTmbTiming");
  xgi::bind(this,&EmuPeripheralCrateConfig::RpcRatTiming, "RpcRatTiming");
  //
  //----------------------------------------------
  // Bind synchronization methods
  //----------------------------------------------
  xgi::bind(this,&EmuPeripheralCrateConfig::FindDistripHotChannel, "FindDistripHotChannel");
  xgi::bind(this,&EmuPeripheralCrateConfig::setupCoincidencePulsing, "setupCoincidencePulsing");
  xgi::bind(this,&EmuPeripheralCrateConfig::setTMBCounterReadValues, "setTMBCounterReadValues");
  xgi::bind(this,&EmuPeripheralCrateConfig::setDataReadValues, "setDataReadValues");
  xgi::bind(this,&EmuPeripheralCrateConfig::Automatic, "Automatic");
  //  xgi::bind(this,&EmuPeripheralCrateConfig::ALCTvpf,"ALCTvpf");
  xgi::bind(this,&EmuPeripheralCrateConfig::FindWinner, "FindWinner");
  xgi::bind(this,&EmuPeripheralCrateConfig::TmbLctCableDelay, "TmbLctCableDelay");
  xgi::bind(this,&EmuPeripheralCrateConfig::PrintDmbValuesAndScopes, "PrintDmbValuesAndScopes");
  xgi::bind(this,&EmuPeripheralCrateConfig::TMBL1aTiming, "TMBL1aTiming");
  xgi::bind(this,&EmuPeripheralCrateConfig::ALCTL1aTiming, "ALCTL1aTiming");
  xgi::bind(this,&EmuPeripheralCrateConfig::AlctDavCableDelay, "AlctDavCableDelay");
  xgi::bind(this,&EmuPeripheralCrateConfig::CfebDavCableDelay, "CfebDavCableDelay");
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
  xgi::bind(this,&EmuPeripheralCrateConfig::CrateDumpConfiguration, "CrateDumpConfiguration");
  //
  // SOAP call-back functions, which relays to *Action method.
  //-----------------------------------------------------------
  //
  xoap::bind(this,&EmuPeripheralCrateConfig::ReadAllVmePromUserid ,"ReadVmePromUserid" ,XDAQ_NS_URI);
  xoap::bind(this,&EmuPeripheralCrateConfig::LoadAllVmePromUserid ,"LoadVmePromUserid" ,XDAQ_NS_URI);
  xoap::bind(this,&EmuPeripheralCrateConfig::ReadAllCfebPromUserid,"ReadCfebPromUserid",XDAQ_NS_URI);
  xoap::bind(this,&EmuPeripheralCrateConfig::LoadAllCfebPromUserid,"LoadCfebPromUserid",XDAQ_NS_URI);
  //
  //-------------------------------------------------------------
  // fsm_ is defined in EmuApplication
  //-------------------------------------------------------------
  fsm_.addState('H', "Halted",     this, &EmuPeripheralCrateConfig::stateChanged);
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
  Valid_config_ID="";
  InFlash_config_ID="";
  //
  for(unsigned int dmb=0; dmb<9; dmb++) {
    L1aLctCounter_.push_back(0);
    CfebDavCounter_.push_back(0);
    TmbDavCounter_.push_back(0);
    AlctDavCounter_.push_back(0);
  }
  //
  CCBRegisterRead_ = -1;
  CCBRegisterValue_ = -1;
  CCBRegisterWrite_ = -1;
  CCBWriteValue_ = -1;
  MPCRegisterRead_ = -1;
  MPCRegisterValue_ = -1;
  MPCRegisterWrite_ = -1;
  MPCWriteValue_ = -1;
  //
  CalibrationState_ = "None";
  standalone_ = false;
  //
  for (int i=0; i<9; i++) {
    able_to_load_alct[i] = -1;  
    number_of_tmb_firmware_errors[i] = -1;
    number_of_alct_firmware_errors[i] = -1;
  }
  for (int i=0; i<9; i++) 
    for (int j=0; j<5; j++)
      CFEBid_[i][j] = -2;
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
  this->getApplicationInfoSpace()->fireItemAvailable("Standalone", &standalone);
  
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

  if(!parsed) 
  {  
     if(Valid_config_ID=="" && (XML_or_DB_.toString() == "db" || XML_or_DB_.toString() == "db"))
        Valid_config_ID = EMU_config_ID_.toString();
     if(Valid_config_ID=="-1" || Valid_config_ID=="-2")
     {
        // choose a ID from list
        std::vector<std::string> configIDs;
        myTStore = GetEmuTStore();
        if(!myTStore)
        {  std::cout << "Can't create object EmuTStore" << std::endl;
           return;
        }
        InFlash_config_ID = myTStore->getLastConfigIdUsed( (Valid_config_ID=="-1")?"plus":"minus" );
        myTStore->getConfigIds(configIDs,(Valid_config_ID=="-1")?"plus":"minus",50);
        
        *out << "Config ID in FLASH is :" << InFlash_config_ID << cgicc::br() << std::endl;
        int n_ids = configIDs.size();
        if(n_ids>0 && n_ids<51)
        {
          // Begin select config ID
           *out << cgicc::form().set("action",
                   "/" + getApplicationDescriptor()->getURN() + "/ConfigIDSelection") << std::endl;

           *out << "Choose Config ID: " << cgicc::br() << std::endl;
           *out << cgicc::select().set("name", "runtype") << std::endl;

           int selected_index;
           for (int i = 0; i < n_ids; ++i) {
             if(configIDs[i]==InFlash_config_ID)
             {
               selected_index = i;
               *out << cgicc::option()
               .set("value", configIDs[i])
               .set("selected", "");
             } else {
               *out << cgicc::option()
               .set("value", configIDs[i]);
             }
             *out << configIDs[i] << cgicc::option() << std::endl;
           }

           *out << cgicc::select() << std::endl;

           *out << cgicc::input().set("type", "submit")
                 .set("name", "command")
                 .set("value", "Select Config ID") << std::endl;
           *out << cgicc::form() << cgicc::br() << cgicc::hr() << std::endl;
           //End select config
        } 
        return;
     }
     else
     {   ParsingXML();
     }
  }
  *out << "Total Crates : ";
  *out << total_crates_ << cgicc::br() << std::endl ;
  unsigned int active_crates=0;
  for(unsigned i=0; i<crateVector.size(); i++)
     if(crateVector[i]->IsAlive()) active_crates++;
  if( active_crates <= total_crates_) 
     *out << cgicc::b(" Active Crates: ") << active_crates << cgicc::br() << std::endl ;
 
  *out << cgicc::table().set("border","0");
  *out << cgicc::h2("System Utilities")<< std::endl;
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

// for non-standalone mode, use the one in Service (Blue) instead
  if(standalone_) 
  {
     *out << cgicc::td();
     std::string FastConfigureAll = toolbox::toString("/%s/FastConfigCrates",getApplicationDescriptor()->getURN().c_str());
     *out << cgicc::form().set("method","GET").set("action",FastConfigureAll) << std::endl ;
     *out << cgicc::input().set("type","submit").set("value","Crates Power-up Init") << std::endl ;
     *out << cgicc::form() << std::endl ;
     *out << cgicc::td();
  }

  *out << cgicc::td();
  std::string PowerOnFixCFEB = toolbox::toString("/%s/PowerOnFixCFEB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::a("[Firmware Check]").set("href",PowerOnFixCFEB) << std::endl;
  *out << cgicc::td();

  *out << cgicc::td();
  std::string CheckConfigurationPage = toolbox::toString("/%s/CheckConfigurationPage",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::a("[Configuration Check]").set("href",CheckConfigurationPage) << std::endl;
  *out << cgicc::td();

  *out << cgicc::td();
  std::string ExpertToolsPage = toolbox::toString("/%s/ExpertToolsPage",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::a("[Expert Tools Page]").set("href",ExpertToolsPage) << std::endl;
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

  *out << cgicc::table() << std::endl ;
  //
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

// for non-standalone mode, use the one in Service (Blue) instead
    if(standalone_)
    {
       *out << cgicc::td();
       std::string FastConfigOne = toolbox::toString("/%s/FastConfigOne",getApplicationDescriptor()->getURN().c_str());
       *out << cgicc::form().set("method","GET").set("action",FastConfigOne) << std::endl ;
       *out << cgicc::input().set("type","submit").set("value","Power-up Init") << std::endl ;
       *out << cgicc::form() << std::endl ;
       *out << cgicc::td();
    }

    //    *out << cgicc::td();
    //    std::string ConfigOneCr = toolbox::toString("/%s/ConfigOneCrate",getApplicationDescriptor()->getURN().c_str());
    //    *out << cgicc::form().set("method","GET").set("action",ConfigOneCr) << std::endl ;
    //    *out << cgicc::input().set("type","submit").set("value","Write FLASH to Crate") << std::endl ;
    //    *out << cgicc::form() << std::endl ;
    //    *out << cgicc::td();
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
	if (alct_check_ok[current_crate_][chamber_index] > 0) alct_ok = false;
	if (tmb_check_ok[current_crate_][chamber_index] > 0)  tmb_ok = false;
	if (dmb_check_ok[current_crate_][chamber_index] > 0)  dmb_ok = false;

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
	  if (alct_check_ok[current_crate_][chamber_index] > 0) {
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
	  if (tmb_check_ok[current_crate_][chamber_index] > 0) {
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
	  if (dmb_check_ok[current_crate_][chamber_index] > 0) {
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
    *out << cgicc::br() << cgicc::hr() <<std::endl;

    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial; background-color:yellow");
    *out << std::endl;
    *out << cgicc::legend((("Logging"))).set("style","color:blue") ;
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
    *out << Valid_config_ID << cgicc::br() << std::endl ;
    if(InFlash_config_ID!="") *out << "(Currently in FLASH is : " << InFlash_config_ID << " )" << std::endl;
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

void EmuPeripheralCrateConfig::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
  throw (toolbox::fsm::exception::Exception) {
  changeState(fsm);
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
     ConfigureInit(0, 0);
     this->Default(in,out);
  }

  void EmuPeripheralCrateConfig::ConfigOneCrate(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
     std::cout << "Button: ConfigOneCrate" << std::endl;
     thisCrate->configure(0);
     this->Default(in,out);
  }

  void EmuPeripheralCrateConfig::FastConfigCrates(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
     if(standalone_)
     {  
        std::cout << "Button: FastConfigCrates" << std::endl;
        ConfigureInit(2);
     }
     this->Default(in,out);
  }

  void EmuPeripheralCrateConfig::FastConfigOne(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
     if(standalone_)
     {  
        thisCrate->configure(2);
     }
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

  void EmuPeripheralCrateConfig::ConfigureInit(int c, int ID)
  {
    int flashed_crates=0;
    if(!parsed) ParsingXML();

    if(total_crates_<=0) return;
    current_config_state_=1;
    for(unsigned i=0; i< crateVector.size(); i++)
    {
        if(crateVector[i] && crateVector[i]->IsAlive())
        {   crateVector[i]->configure(c, ID);
            flashed_crates++;
        }
    }
    current_config_state_=2;

    // record the WRITE FLASH action in configuration database
    if(c==0 && xml_or_db==1 && flashed_crates>0)
    {
        InFlash_config_ID = Valid_config_ID;
        try 
        {
           myTStore->recordFlashWrite(Valid_config_ID);
           std::cout << getLocalDateTime() << " WRITE FLASH recorded in database. Configuration ID: " << Valid_config_ID << " written to " << flashed_crates << " crates." << std::endl;
        }
        catch( const std::exception & e )
        {
           std::cout << "Failed to write the configuration database!" << std::endl;
        }
    }
   
    //
  }

  void EmuPeripheralCrateConfig::ConfigIDSelection(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
     cgicc::Cgicc cgi(in);

     std::string in_value = cgi.getElement("runtype")->getValue(); 
     std::cout << "Select Config ID " << in_value << std::endl;
     if(!in_value.empty())
     {
       Valid_config_ID=in_value;
     }
     this->Default(in,out);
  }

bool EmuPeripheralCrateConfig::ParsingXML(){
  //
    std::string config_src, config_key;
    //
    Logger logger_ = getApplicationLogger();
    //
    LOG4CPLUS_INFO(logger_, "EmuPeripheralCrate reloading...");
    //
    config_src = XML_or_DB_.toString();
    // std::cout << "XML_or_DB: " << config_src << std::endl;
    if(config_src == "xml" || config_src == "XML")
    {
       config_key = xmlFile_.toString();
    }
    else if (config_src == "db" || config_src == "DB")
    {
       config_key = Valid_config_ID;
    }
    else
    {
       std::cout << "No valid XML_or_DB found..." << std::endl;
       return false;
    }
    if(!CommonParser(config_src, config_key)) return false;
    EmuEndcap *myEndcap = GetEmuEndcap();
    if(myEndcap == NULL) return false;
    myEndcap->NotInDCS();
    xml_or_db = Xml_or_Db();
    crateVector = myEndcap->crates();
    //
    total_crates_=crateVector.size();
    if(total_crates_<=0) return false;
    this_crate_no_=0;

    std::string lab_mode=standalone;
    if(lab_mode=="True" || lab_mode=="TRUE" || lab_mode=="true" || lab_mode=="Yes" || lab_mode=="YES" || lab_mode=="yes")
    {    standalone_ = true;
         std::cout << "PeripheralCrateConfig started in Standalone mode." << std::endl;
    }

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
    for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) {
      MyTest[i][cr].SetTMB(tmbVector[i]);
      MyTest[i][cr].SetDMB(dmbVector[i]);
      MyTest[i][cr].SetCCB(thisCCB);
      MyTest[i][cr].SetMPC(thisMPC);
      // reset ALCT firmware check value for this crate
      able_to_load_alct[i] = -1;
    }
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
    for(unsigned i=0; i< crateVector.size(); i++)
    {
        crateVector[i]->CheckController();
    }
    this->Default(in, out);
  }

//
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
  *out << cgicc::legend("Upload Configuration...").set("style","color:blue") << std::endl ;
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
  if(!parsed)
  {  this->Default(in,out);
     return;
  }
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
  for(int ii=1; ii<22; ii++) {
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
      *out << cgicc::td();
      *out << "VME Crate Controller" ;
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
      *out << cgicc::td();
      *out << "CCB" ;
      *out << cgicc::td();
      //
      *out << cgicc::td();
      std::string CCBStatus = toolbox::toString("/%s/CCBStatus?ccb=%d",getApplicationDescriptor()->getURN().c_str(),ii);
      *out << cgicc::a("CCB Status").set("href",CCBStatus) << std::endl;
      *out << cgicc::td();
      //
      *out << cgicc::td();
      std::string CCBUtils = toolbox::toString("/%s/CCBUtils?ccb=%d",getApplicationDescriptor()->getURN().c_str(),ii);
      *out << cgicc::a("CCB Utils").set("href",CCBUtils) << std::endl;
      *out << cgicc::td();
      //
    }
    //
    //----------------------------------
    // Display MPC buttons, if it exists
    //----------------------------------
    slot = -1;
    if ( thisMPC ) slot = thisMPC->slot() ;
    if(slot == ii) {
      //
      *out << cgicc::td();
      *out << "MPC" ;
      *out << cgicc::td();
      //
      if ( thisMPC ) {
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
    //
    TMBStatus  = toolbox::toString("/%s/TMBStatus" ,getApplicationDescriptor()->getURN().c_str());
    TMBTests   = toolbox::toString("/%s/TMBTests"  ,getApplicationDescriptor()->getURN().c_str());
    TMBUtils   = toolbox::toString("/%s/TMBUtils"  ,getApplicationDescriptor()->getURN().c_str());
    //
    for (unsigned int i=0; i<tmbVector.size(); i++) {
      //
      int slot = tmbVector[i]->slot();
      if(slot == ii) {
	//
	*out << cgicc::td();
	*out << "TMB / RAT / ALCT" ;
	*out << cgicc::td();
	//
	*out << cgicc::td();
	std::string TMBStatus = toolbox::toString("/%s/TMBStatus?tmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	*out << cgicc::a("TMB Status").set("href",TMBStatus) << std::endl;
	*out << cgicc::td();
	//
	*out << cgicc::td();
	std::string TMBTests = toolbox::toString("/%s/TMBTests?tmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	*out << cgicc::a("TMB Tests").set("href",TMBTests) << std::endl;
	*out << cgicc::td();
	//
	*out << cgicc::td();
	std::string TMBUtils = toolbox::toString("/%s/TMBUtils?tmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	*out << cgicc::a("TMB Utils").set("href",TMBUtils) << std::endl;
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
	    char Name[50];
	    sprintf(Name,"Chamber Tests: %s",(thisCrate->GetChamber(slot)->GetLabel()).c_str());
	    std::string ChamberTests = toolbox::toString("/%s/ChamberTests?tmb=%d&dmb=%d",getApplicationDescriptor()->getURN().c_str(),i,iii);
	    *out << cgicc::a(Name).set("href",ChamberTests) << std::endl;
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
    //
    DMBStatus  = toolbox::toString("/%s/DMBStatus",getApplicationDescriptor()->getURN().c_str());
    DMBTests   = toolbox::toString("/%s/DMBTests",getApplicationDescriptor()->getURN().c_str());
    DMBUtils   = toolbox::toString("/%s/DMBUtils",getApplicationDescriptor()->getURN().c_str());
    //
    for (unsigned int i=0; i<dmbVector.size(); i++) {
      int slot = dmbVector[i]->slot();
      if(slot == ii ) {
	//
	*out << cgicc::td();
	*out << "DMB / CFEB" ;
	*out << cgicc::td();
	//
	*out << cgicc::td();
	std::string DMBStatus = toolbox::toString("/%s/DMBStatus?dmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	*out << cgicc::a("DMB Status").set("href",DMBStatus) << std::endl;
	*out << cgicc::td();
	//
	*out << cgicc::td();
	std::string DMBTests = toolbox::toString("/%s/DMBTests?dmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	*out << cgicc::a("DMB Tests").set("href",DMBTests) << std::endl;
	*out << cgicc::td();
	//
	*out << cgicc::td();
	std::string DMBUtils = toolbox::toString("/%s/DMBUtils?dmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	*out << cgicc::a("DMB Utils").set("href",DMBUtils) << std::endl;
	*out << cgicc::td();
	//
      }
    }
    //
    *out << cgicc::table();
    *out << cgicc::br();
  }
  //
  //
  *out << cgicc::table().set("border","1");
  //
  *out << cgicc::td();
  *out << "Data readout synchronization parameters..." << std::endl;
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
  *out << cgicc::td();
  std::string MeasureL1AsAndDAVsForCrate = toolbox::toString("/%s/MeasureL1AsAndDAVsForCrate",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",MeasureL1AsAndDAVsForCrate) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Find L1A and DAV delays for crate") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string QuickScanForCrate = toolbox::toString("/%s/QuickScanForCrate",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",QuickScanForCrate) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Quick Scan for crate") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::tr();
  //
  *out << cgicc::td();
  *out << "Muonic timing scans" << std::endl;
  *out << cgicc::td();  
  //
  *out << cgicc::td();
  std::string MeasureALCTTMBRxTxForCrate = toolbox::toString("/%s/MeasureALCTTMBRxTxForCrate",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",MeasureALCTTMBRxTxForCrate) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Find ALCT rx/tx for crate") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string MeasureCFEBTMBRxForCrate = toolbox::toString("/%s/MeasureCFEBTMBRxForCrate",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",MeasureCFEBTMBRxForCrate) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Find CFEB rx for crate") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  char buf[20];
  std::string Settmb_bxn_offset = toolbox::toString("/%s/Settmb_bxn_offset",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",Settmb_bxn_offset) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Set tmb_bxn_offset for BC0 scan") << std::endl ;
  *out << "tmb_bxn_offset (bx)" << std::endl;
  sprintf(buf,"%d",MyTest[0][current_crate_].getLocalTmbBxnOffset());
  *out << cgicc::input().set("type","text").set("value",buf).set("name","tmb_bxn_offset") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string ALCTBC0ScanForCrate = toolbox::toString("/%s/ALCTBC0ScanForCrate",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ALCTBC0ScanForCrate) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Synchronize ALCT BC0 for crate") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::tr();
  //
  *out << cgicc::td();
  *out << "Other cratewide utilities" << std::endl;
  *out << cgicc::td();  
  //
  *out << cgicc::td();
  std::string SetTwoLayerTriggerForCrate = toolbox::toString("/%s/SetTwoLayerTriggerForCrate",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",SetTwoLayerTriggerForCrate) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Enable two-layer trigger for crate") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::table();
  //*out << cgicc::body();
  *out << cgicc::fieldset();
  //
}
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
  *out << cgicc::legend("Actions").set("style","color:blue") << std::endl ;
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
  std::string CheckBC0Synchronization = toolbox::toString("/%s/CheckBC0Synchronization",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CheckBC0Synchronization) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Check BC0 synchronization") << std::endl ;
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
  *out << cgicc::table();
  //
  *out << cgicc::fieldset();
  //
  int initial_crate = current_crate_;
  //
  if (print_stuff) {
    //
    *out                     << "date_time = " << date_and_time_  << cgicc::br() << std::endl;
    OutputCheckConfiguration << "date_time = " << date_and_time_                << std::endl;
    //
  }
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("Time since last hard reset").set("style","color:blue") << std::endl ;
  //
  bool print_times = false;
  for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) 
    for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) 
      if (time_since_reset[crate_number][chamber_index]>=0) print_times = true;
  //
  if ( print_times ) {
    //
    OutputCheckConfiguration << "Number of seconds since last hard reset for each TMB" << std::endl;
    //
    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      //
      SetCurrentCrate(crate_number);
      //
      *out                     << crateVector[crate_number]->GetLabel();
      OutputCheckConfiguration << crateVector[crate_number]->GetLabel();
      //
      for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
	*out                     << " " << std::dec << time_since_reset[crate_number][chamber_index];
	OutputCheckConfiguration << " " << std::dec << time_since_reset[crate_number][chamber_index];
      }
      //
      *out                     << cgicc::br() << std::endl;
      OutputCheckConfiguration                << std::endl;
    }
  }
  *out << cgicc::fieldset();
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("Number of times BC0 is synchronized at TMB").set("style","color:blue") << std::endl ;
  //
  bool print_sync = false;
  for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) 
    for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) 
      if (bc0_sync[crate_number][chamber_index]>=0) print_sync = true;
  //
  if ( print_sync ) {
    //
    OutputCheckConfiguration << "Number of times BC0 from TMB matched BC0 from ALCT" << std::endl;
    //
    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      //
      SetCurrentCrate(crate_number);
      //
      *out                     << crateVector[crate_number]->GetLabel();
      OutputCheckConfiguration << crateVector[crate_number]->GetLabel();
      //
      for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
	*out                     << " " << std::dec << bc0_sync[crate_number][chamber_index];
	OutputCheckConfiguration << " " << std::dec << bc0_sync[crate_number][chamber_index];
      }
      //
      *out                     << cgicc::br() << std::endl;
      OutputCheckConfiguration                << std::endl;
    }
  }
  //
  *out << cgicc::fieldset();
  //
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("Configuration Status").set("style","color:blue") << std::endl ;
  //
  if (all_crates_ok >= 0) {
    //
    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      //
      SetCurrentCrate(crate_number);
      //
      if ( !crate_check_ok[current_crate_] ) {
	//
	//    OutputCheckConfiguration << "Problem summary for Crate " << thisCrate->GetLabel() << "..." << std::endl;
	//
	if( !(ccb_check_ok[current_crate_]) ) {
	  *out                     << thisCrate->GetLabel() << "<span style=\"color:red\" >, CCB config fail </span>" << cgicc::br() << std::endl;
	  OutputCheckConfiguration << thisCrate->GetLabel() << ", CCB"                << std::endl;
	}
	//
	if( !(mpc_check_ok[current_crate_]) ) {
	  *out                     << thisCrate->GetLabel() << "<span style=\"color:red\" >, MPC config fail </span>" << cgicc::br() << std::endl;
	  OutputCheckConfiguration << thisCrate->GetLabel() << ", MPC"                << std::endl;
	}
	//
	for (unsigned int chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
	  //
	  if( tmb_check_ok[current_crate_][chamber_index]  > 0  ||
	      alct_check_ok[current_crate_][chamber_index] > 0  ||
	      dmb_check_ok[current_crate_][chamber_index]  > 0 ) {
	    //
	    *out << "<span style=\"color:black\" >";
	    *out                     << thisCrate->GetLabel() << ", " << (chamberVector[chamber_index]->GetLabel()).c_str();
	    OutputCheckConfiguration << thisCrate->GetLabel() << ", " << (chamberVector[chamber_index]->GetLabel()).c_str();
	    *out << "</span>";
	    //
	    bool print_description = false;
	    //
	    if( tmb_check_ok[current_crate_][chamber_index] > 0) {
	      //
	      OutputCheckConfiguration << ", TMB, " << tmb_check_ok[current_crate_][chamber_index];
	      //
	      if( tmb_check_ok[current_crate_][chamber_index]        == 1) {
		*out << "<span style=\"color:red\" > TMB config fail, </span>";
	      } else if( tmb_check_ok[current_crate_][chamber_index] == 2) {
		*out << "<span style=\"color:black\" > expected TMB config fail, </span>";
		print_description = true;
	      } else if( tmb_check_ok[current_crate_][chamber_index] == 3) {
		*out << "<span style=\"color:blue\" > did not see expected TMB config failure, </span>";
		print_description = true;
	      } else if( tmb_check_ok[current_crate_][chamber_index] == 4) {
		*out << "<span style=\"color:red\" > TMB FPGA did not program, </span>";
	      } 
	      //
	    } else {
	      //
	      *out                     << ", , ";
	      OutputCheckConfiguration << ", , ";
	    }
	    //
	    if( alct_check_ok[current_crate_][chamber_index] > 0) {
	      //
	      OutputCheckConfiguration << ", ALCT, " << alct_check_ok[current_crate_][chamber_index];
	      //
	      if( alct_check_ok[current_crate_][chamber_index]        == 1) {
		*out << "<span style=\"color:red\" > ALCT config fail, </span>";
	      } else if( alct_check_ok[current_crate_][chamber_index] == 2) {
		*out << "<span style=\"color:black\" > expected ALCT config fail, </span>";
		print_description = true;
	      } else if( alct_check_ok[current_crate_][chamber_index] == 3) {
		*out << "<span style=\"color:blue\" > did not see expected ALCT config failure, </span>";
		print_description = true;
	      } else if( alct_check_ok[current_crate_][chamber_index] == 4) {
		*out << "<span style=\"color:red\" > ALCT FPGA did not program, </span>";
	      } 
	      //
	    } else {
	      //
	      *out                     << ", , ";
	      OutputCheckConfiguration << ", , ";
	    }
	    //
	    if( dmb_check_ok[current_crate_][chamber_index] > 0) {
	      //
	      OutputCheckConfiguration << ", DMB, " << dmb_check_ok[current_crate_][chamber_index];
	      //
	      if( dmb_check_ok[current_crate_][chamber_index]        == 1) {
		*out << "<span style=\"color:red\" > DMB config fail, </span>";
	      } else if( dmb_check_ok[current_crate_][chamber_index] == 2) {
		*out << "<span style=\"color:black\" > expected DMB config fail, </span>";
		print_description = true;
	      } else if( dmb_check_ok[current_crate_][chamber_index] == 3) {
		*out << "<span style=\"color:blue\" > did not see expected DMB config failure, </span>";
		print_description = true;
	      } else if( dmb_check_ok[current_crate_][chamber_index] == 4) {
		*out << "<span style=\"color:red\" > DMB FPGA did not program, </span>";
	      } 
	      //
	    } else {
	      //
	      *out                     << ", , ";
	      OutputCheckConfiguration << ", , ";
	    }
	    //
	    if (print_description) {
	      *out                     << ", " << (chamberVector[chamber_index]->GetProblemDescription()).c_str();
	      OutputCheckConfiguration << ", " << (chamberVector[chamber_index]->GetProblemDescription()).c_str();
	    } else {
	      *out                     << ", ";
	      OutputCheckConfiguration << ", ";
	    }
	    //
	    *out                     << cgicc::br() << std::endl;
	    OutputCheckConfiguration                << std::endl;
	    //
	  } 
	}
	} else if (crate_check_ok[current_crate_] == -1) {
	//
	*out << cgicc::span().set("style","color:blue");
	*out                     << crateVector[crate_number]->GetLabel() << " Not checked" << cgicc::br();
	OutputCheckConfiguration << crateVector[crate_number]->GetLabel() << " Not checked" << std::endl;
	*out << cgicc::span() << std::endl ;
      }

    }
  }
  //
  SetCurrentCrate(initial_crate);
  //
  *out << cgicc::fieldset();
  //
  if (print_stuff) {
    //
    //Output the errors to a file...
    //
    // The peripheral crate labels have the convention:  VME[p,n]N_M.  Here we use 
    // the "p" or "n" to label which endcap we are checking the firmware status on...
    const char * crate_name = crateVector[0]->GetLabel().c_str();
    char endcap_side = crate_name[3];
    //
    // This file is hardcoded as FirmwareDir_/status_check/YEARMODA_HRMN_[p,n]_firmware_status.log
    char filename[200];
    sprintf(filename,"%s/status_check/%s_%c_configuration_check.log",FirmwareDir_.c_str(),date_and_time_,endcap_side);
    //
    //  std::cout << "filename = " << filename << std::endl;
    //
    std::ofstream LogFileCheckConfiguration;
    LogFileCheckConfiguration.open(filename);
    LogFileCheckConfiguration << OutputCheckConfiguration.str() ;
    LogFileCheckConfiguration.close();
    //
  }
  print_stuff = false;
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
  //
  SetCurrentCrate(initialcrate);	
  //
  this->CheckConfigurationPage(in, out);
}
//
void EmuPeripheralCrateConfig::CheckBC0Synchronization(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  cgicc::CgiEnvironment cgiEnvi(in);
  //
  std::cout << "Check BC0 synchronization at TMBs... " << std::endl; 
  //
  int initialcrate=current_crate_;
  //
  for (int i=0; i<60; i++) 
    for (int j=0; j<9; j++) 
      bc0_sync[i][j] = 0;
  //
  for(unsigned i=0; i< crateVector.size(); i++) {
    //
    if ( crateVector[i]->IsAlive() ) {
      //
      SetCurrentCrate(i);	
      //
      for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	for (int count=0; count < 100; count++) {
	  tmbVector[tmb]->ReadRegister(0xCA);
	  bc0_sync[i][tmb] += tmbVector[tmb]->GetReadBx0Match();
	}
	std::cout << " " << bc0_sync[i][tmb];
      }
      std::cout << std::endl;
    }
  }
  //
  SetCurrentCrate(initialcrate);	
  //
  this->CheckConfigurationPage(in, out);
}
//
void EmuPeripheralCrateConfig::CheckCratesConfiguration(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //
  std::cout << "Button:  Check Configuration of All Active Crates" << std::endl;
  //
  int initialcrate=current_crate_;
  //
  if(total_crates_<=0) return;
  //
  print_stuff = true;
  //
  OutputCheckConfiguration.str(""); //clear the output string
  //
  // get the date and time of this check:
  time_t rawtime;
  struct tm * timeinfo;
  //
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  //
  int yearAD = timeinfo->tm_year + 1900;
  int month_counting_from_one = timeinfo->tm_mon + 1;
  int day = timeinfo->tm_mday;
  int hour = timeinfo->tm_hour;     
  int minute = timeinfo->tm_min;     
  //
  sprintf(date_and_time_,"%4u%02u%02u_%02u%02u",yearAD,month_counting_from_one,day,hour,minute);
  //
  std::cout << "Check time since TMBs last received hard resets... " << std::endl; 
  //
  for (int i=0; i<60; i++) {
    for (int j=0; j<9; j++) {
      time_since_reset[i][j] = -1;
      bc0_sync[i][j] = 0;
    }
  }
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
  //
  for(unsigned i=0; i< crateVector.size(); i++) {
    //
    if ( crateVector[i]->IsAlive() ) {
      //
      SetCurrentCrate(i);	
      //
      for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	for (int count=0; count < 100; count++) {
	  tmbVector[tmb]->ReadRegister(0xCA); 
	  bc0_sync[i][tmb] += tmbVector[tmb]->GetReadBx0Match();
	}
	std::cout << " " << bc0_sync[i][tmb];
      }
      std::cout << std::endl;
    }
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
  this->CheckConfigurationPage(in, out);
}
//
void EmuPeripheralCrateConfig::CheckCratesConfigurationFull(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //
  std::cout << "Button:  Check Configuration of All Active Crates including TTCrx" << std::endl;
  //
  int initialcrate=current_crate_;
  //
  if(total_crates_<=0) return;
  //
  print_stuff = true;
  //
  OutputCheckConfiguration.str(""); //clear the output string
  //
  // get the date and time of this check:
  time_t rawtime;
  struct tm * timeinfo;
  //
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  //
  int yearAD = timeinfo->tm_year + 1900;
  int month_counting_from_one = timeinfo->tm_mon + 1;
  int day = timeinfo->tm_mday;
  int hour = timeinfo->tm_hour;     
  int minute = timeinfo->tm_min;     
  //
  sprintf(date_and_time_,"%4u%02u%02u_%02u%02u",yearAD,month_counting_from_one,day,hour,minute);
  //
  std::cout << "Check time since TMBs last received hard resets... " << std::endl; 
  //
  for (int i=0; i<60; i++) {
    for (int j=0; j<9; j++) {
      time_since_reset[i][j] = -1;
      bc0_sync[i][j] = 0;
    }
  }
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
  //
  for(unsigned i=0; i< crateVector.size(); i++) {
    //
    if ( crateVector[i]->IsAlive() ) {
      //
      SetCurrentCrate(i);	
      //
      for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	for (int count=0; count < 100; count++) {
	  tmbVector[tmb]->ReadRegister(0xCA); 
	  bc0_sync[i][tmb] += tmbVector[tmb]->GetReadBx0Match();
	}
	std::cout << " " << bc0_sync[i][tmb];
      }
      std::cout << std::endl;
    }
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
      CheckPeripheralCrateConfiguration(1);
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
  this->ExpertToolsPage(in,out);
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
void EmuPeripheralCrateConfig::CheckPeripheralCrateConfiguration(int full_check) {
  //
  std::cout << "Hardware configuration check for " << thisCrate->GetLabel() << std::endl;
  //  OutputCheckConfiguration << "Hardware configuration check for " << thisCrate->GetLabel() << "..." << std::endl;
  //
  crate_check_ok[current_crate_] = 1;
  //
  //  OutputCheckConfiguration << "Crate " << thisCrate->GetLabel(); // << std::endl;
  //
  // perform the checks on the hardware.  Note that CCB needs to be checked to determine if boards' FPGA's are programmed.
  thisCrate->ccb()->RedirectOutput(&OutputCheckConfiguration);
  ccb_check_ok[current_crate_] = thisCrate->ccb()->CheckConfig(full_check);
  thisCrate->ccb()->RedirectOutput(&std::cout);
  //
  thisCrate->mpc()->RedirectOutput(&OutputCheckConfiguration);
  mpc_check_ok[current_crate_] = thisCrate->mpc()->CheckConfig();
  thisCrate->mpc()->RedirectOutput(&std::cout);
  //
  crate_check_ok[current_crate_] &=  ccb_check_ok[current_crate_];  
  crate_check_ok[current_crate_] &=  mpc_check_ok[current_crate_];  
  //
  //
  // perform the checks on the hardware
  for (unsigned int chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
    Chamber * thisChamber     = chamberVector[chamber_index];
    TMB * thisTMB             = tmbVector[chamber_index];
    ALCTController * thisALCT = thisTMB->alctController();
    DAQMB * thisDMB           = dmbVector[chamber_index];
    //
    std::cout << "Hardware configuration check for " << thisCrate->GetLabel() << ", " << (thisChamber->GetLabel()).c_str() << std::endl;
    OutputCheckConfiguration << "Hardware configuration check for " << thisCrate->GetLabel() << ", " << (thisChamber->GetLabel()).c_str() << "..." << std::endl;
    //
    thisTMB->RedirectOutput(&OutputCheckConfiguration);
    thisTMB->CheckTMBConfiguration();
    tmb_check_ok[current_crate_][chamber_index]  = (int) thisTMB->GetTMBConfigurationStatus();
    thisTMB->RedirectOutput(&std::cout);
    crate_check_ok[current_crate_] &=  tmb_check_ok[current_crate_][chamber_index];  
    //
    thisALCT->RedirectOutput(&OutputCheckConfiguration);
    thisALCT->CheckALCTConfiguration();
    alct_check_ok[current_crate_][chamber_index] = (int) thisALCT->GetALCTConfigurationStatus();
    thisALCT->RedirectOutput(&std::cout);
    crate_check_ok[current_crate_] &=  alct_check_ok[current_crate_][chamber_index];  
    //
    thisDMB->RedirectOutput(&OutputCheckConfiguration);
    dmb_check_ok[current_crate_][chamber_index]  = (int) thisDMB->checkDAQMBXMLValues();
    thisDMB->RedirectOutput(&std::cout); 
    crate_check_ok[current_crate_] &=  dmb_check_ok[current_crate_][chamber_index];  
    //
  }
  //
  // in the following loop, change check_ok values to match the TYPE of configuration error...
  // = 0 = read configuration not OK
  // = 1 = read configuration not OK
  // = 2 = read configuration not OK (or FPGA not programmed), and this has been masked in the problem_mask as such
  // = 3 = read configuration OK, but this has been masked in the problem_mask as a problem
  // = 4 = FPGA not programmed --> don't use this, as it just introduces other problems...
  //
  for (unsigned int chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
    Chamber * thisChamber     = chamberVector[chamber_index];
    //    TMB * thisTMB             = tmbVector[chamber_index];
    //													
    // The following mapping is needed when the crate is not full...
    //    const int slot_to_ccb_index_mapping[21] = {22, 22, 0, 22, 1, 22, 2, 22, 3, 22, 4, 22, 22, 22, 5, 22, 6, 22, 7, 22, 8};    
    //
    if(!(tmb_check_ok[current_crate_][chamber_index])) {
      tmb_check_ok[current_crate_][chamber_index] = 1;
      //
      if(thisChamber->GetExpectedConfigProblemTMB()) 
	tmb_check_ok[current_crate_][chamber_index] = 2;
      //
    } else if ( thisChamber->GetExpectedConfigProblemTMB() ) {
      tmb_check_ok[current_crate_][chamber_index] = 3;
    } else {
      tmb_check_ok[current_crate_][chamber_index] = 0;
    }
    //
    // do not use the signal sent to CCB.  It just hides what the problem is...
    //    if( thisCrate->ccb()->GetReadTMBConfigDone(slot_to_ccb_index_mapping[thisTMB->slot()]) != 
    //	thisCrate->ccb()->GetExpectedTMBConfigDone() ) {
    //      tmb_check_ok[current_crate_][chamber_index] = 4;
    //      //
    //      if(thisChamber->GetExpectedConfigProblemTMB()) 
    //	tmb_check_ok[current_crate_][chamber_index] = 2;
    //    }
    //    
    //
    if(!(alct_check_ok[current_crate_][chamber_index])) {
      alct_check_ok[current_crate_][chamber_index] = 1;
      //
      if(thisChamber->GetExpectedConfigProblemALCT()) 
	alct_check_ok[current_crate_][chamber_index] = 2;
      //
    } else if ( thisChamber->GetExpectedConfigProblemALCT() ) {
      alct_check_ok[current_crate_][chamber_index] = 3;
    } else {
      alct_check_ok[current_crate_][chamber_index] = 0;
    }
    //
    // do not use the signal sent to CCB.  It just hides what the problem is...
    //    if( thisCrate->ccb()->GetReadALCTConfigDone(slot_to_ccb_index_mapping[thisTMB->slot()]) !=  
    //	thisCrate->ccb()->GetExpectedALCTConfigDone() ) {
    //      alct_check_ok[current_crate_][chamber_index] = 4;
    //      //
    //      if(thisChamber->GetExpectedConfigProblemALCT()) 
    //	alct_check_ok[current_crate_][chamber_index] = 2;
    //    }
    //
    //    
    //
    std::string chamberName=toolbox::toString((char *)thisChamber->GetLabel().c_str());
    //
    // DMB has the CFEB check lumped in...
    if(!(dmb_check_ok[current_crate_][chamber_index])) {
      dmb_check_ok[current_crate_][chamber_index] = 1;
      //
      if(thisChamber->GetExpectedConfigProblemDMB()    ||
	 thisChamber->GetExpectedConfigProblemCFEB1()  ||
	 thisChamber->GetExpectedConfigProblemCFEB2()  ||
	 thisChamber->GetExpectedConfigProblemCFEB3()  ||
	 thisChamber->GetExpectedConfigProblemCFEB4()  ||
	 thisChamber->GetExpectedConfigProblemCFEB5()  ) 
	  dmb_check_ok[current_crate_][chamber_index] = 2;

	//
    } else if( (thisChamber->GetExpectedConfigProblemDMB()    ||
		thisChamber->GetExpectedConfigProblemCFEB1()  ||
		thisChamber->GetExpectedConfigProblemCFEB2()  ||
		thisChamber->GetExpectedConfigProblemCFEB3()  ||
		thisChamber->GetExpectedConfigProblemCFEB4()  ||
		thisChamber->GetExpectedConfigProblemCFEB5()) ) {
      dmb_check_ok[current_crate_][chamber_index] = 3;
    } else {
      dmb_check_ok[current_crate_][chamber_index] = 0;
    }
    //
    // do not use the signal sent to CCB.  It just hides what the problem is...
    //    if( (thisCrate->ccb()->GetReadDMBConfigDone(slot_to_ccb_index_mapping[thisTMB->slot()]) !=  
    //	 thisCrate->ccb()->GetExpectedDMBConfigDone()) &&  
    //	(chamberName.find("1/3/")==std::string::npos) ) {           // this is because ME1/3 chambers do not have CFEB4) 
    //      dmb_check_ok[current_crate_][chamber_index] = 4;
    //      //
    //      if(thisChamber->GetExpectedConfigProblemDMB()    ||
    //	 thisChamber->GetExpectedConfigProblemCFEB1()  ||
    //	 thisChamber->GetExpectedConfigProblemCFEB2()  ||
    //	 thisChamber->GetExpectedConfigProblemCFEB3()  ||
    //	 thisChamber->GetExpectedConfigProblemCFEB4()  ||
    //	 thisChamber->GetExpectedConfigProblemCFEB5()  ) 
    //	  dmb_check_ok[current_crate_][chamber_index] = 2;
    //    }
    //
    if ( tmb_check_ok[current_crate_][chamber_index]  > 0 ) crate_check_ok[current_crate_] = 0;
    if ( alct_check_ok[current_crate_][chamber_index] > 0 ) crate_check_ok[current_crate_] = 0;
    if ( dmb_check_ok[current_crate_][chamber_index]  > 0 ) crate_check_ok[current_crate_] = 0;
    //
  }
  //
  return;
}
//

void EmuPeripheralCrateConfig::CheckFirmware(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //
  std::cout << "Checking System Firmware " << std::dec << number_of_checks_ << " times..." << std::endl;
  //
  int initial_crate = current_crate_;
  //
  const float cfeb_minimum_current_value =  0.01;
  const float cfeb_maximum_current_value =  0.85;
  const float alct_maximum_current_value =  0.2 ;
  const float alct_minimum_current_value =  0.05;
  //
  // initialize the number of times each of the following checks are OK.
  //
  for (int i=0; i<60; i++) {
    ccb_firmware_ok[i] = 0;
    mpc_firmware_ok[i] = 0;
    for (int j=0; j<9; j++) {
      alct_firmware_ok[i][j] = 0;
      tmb_firmware_ok[i][j] = 0;
      dmb_vme_firmware_ok[i][j] = 0;
      dmb_control_firmware_ok[i][j] = 0;
      dmbcfg_ok[i][j]  = 0;
      dmb_config_ok[i][j]  = 0;
      alctcfg_ok[i][j]  = 0;
      tmbcfg_ok[i][j]  = 0;
      alct_lvmb_current_ok[i][j]  = 0;
      alct_adc_current_ok[i][j]  = 0;
      //
      for (int k=0; k<5; k++) {
	cfeb_firmware_ok[i][j][k] = 0;
	cfeb_config_ok[i][j][k]   = 0;
	cfeb_current_ok[i][j][k]  = 0;
      }
    }
  }
  //

  for (int hard_reset_index=0; hard_reset_index<number_of_checks_; hard_reset_index++) {
    //
    std::cout << "Firmware check iteration " << hard_reset_index << std::endl;
    //
    // send hard reset from CCB to load FPGA's from EEPROM's in all electronics modules.  
    // If the user does not request a hard reset, just read the values
    if (number_of_hard_resets_>0 && hard_reset_index > 0 ){ //do not send a hard reset on the first iteration...
      //
      for(unsigned crate_index=0; crate_index< crateVector.size(); crate_index++){
	//
	SetCurrentCrate(crate_index);
        //
        std::cout << "CCB Hard Reset for " << thisCrate->GetLabel() << std::endl;
        //
	if (!thisCrate->IsAlive()) continue;
	//
	thisCCB->hardReset();
      }
    }
    //
    // Check that the firmware status is A-OK:
    for(unsigned crate_index=0; crate_index< crateVector.size(); crate_index++){
      //
      SetCurrentCrate(crate_index);
      if (!thisCrate->IsAlive()) continue;
      //
      // check CCB and MPC firmware versions by reading the FPGA ID's
      ccb_firmware_ok[current_crate_] = thisCrate->ccb()->CheckFirmwareDate();
      mpc_firmware_ok[current_crate_] = thisCrate->mpc()->CheckFirmwareDate();
      //
      // read the "FPGA configuration done" bits which have been sent to the CCB
      const int CSRA3=0x04;
      thisCCB->ReadRegister(CSRA3);
      //
      const int CSRA2=0x02;
      thisCCB->ReadRegister(CSRA2);
      //
      // check TMB, DMB, ALCT, and CFEB's attached to this chamber...
      for (unsigned int chamber_index=0; chamber_index<dmbVector.size(); chamber_index++) {
	DAQMB * thisDMB = dmbVector[chamber_index];
	std::vector<CFEB> thisCFEBs = thisDMB->cfebs() ;
        int dslot = thisDMB->slot();
	//
	TMB * thisTMB   = tmbVector[chamber_index];
	ALCTController * thisALCT   = thisTMB->alctController();
	//
        Chamber * thisChamber = chamberVector[chamber_index];
	//
	if(dslot<22){
	  //
	  // check if the CCB has received the "config done" bit from the component's FPGA
	  if (thisCCB->GetReadTMBConfigDone(chamber_index) == thisCCB->GetExpectedTMBConfigDone() )
	    tmbcfg_ok[current_crate_][chamber_index]++;
	  //
	  if (thisCCB->GetReadALCTConfigDone(chamber_index) == thisCCB->GetExpectedALCTConfigDone() )
	    alctcfg_ok[current_crate_][chamber_index]++;
	  //
	  // The DMB config done bit depends on whether or not it is ME1/3.  I.e., if it is ME1/3, this 
	  // check will always fail.  That's OK, that just means we look at the currents and ID's of 
	  // all CFEBs and DMB for ME1/3 in detail, below.
	  if (thisCCB->GetReadDMBConfigDone(chamber_index) == thisCCB->GetExpectedDMBConfigDone() )
	    dmbcfg_ok[current_crate_][chamber_index]++;
	  // 
	  // check firmware versions by reading the FPGA ID's
	  tmb_firmware_ok[current_crate_][chamber_index]         += (int) thisTMB->CheckFirmwareDate();
	  alct_firmware_ok[current_crate_][chamber_index]        += (int) thisALCT->CheckFirmwareDate();
	  dmb_vme_firmware_ok[current_crate_][chamber_index]     += (int) thisDMB->CheckVMEFirmwareVersion();
	  dmb_control_firmware_ok[current_crate_][chamber_index] += (int) thisDMB->CheckControlFirmwareVersion();
	  typedef std::vector<CFEB>::iterator CFEBItr;
	  for(CFEBItr cfebItr = thisCFEBs.begin(); cfebItr != thisCFEBs.end(); ++cfebItr) {
	    int cfeb_index = (*cfebItr).number();
	    cfeb_firmware_ok[current_crate_][chamber_index][cfeb_index] += (int) thisDMB->CheckCFEBFirmwareVersion(*cfebItr);
	  }
	  //
	  // check if the configuration of the CFEBs and DMBs are OK...
	  // in particular, check if the "smoking gun" for firmware loss is OK...
	  //
	  thisDMB->checkDAQMBXMLValues();   //this has the CFEB check implicit in it
	  //
	  // greg, put in cfeb firmware version check in CFEB config check
	  // greg, split up CFEBs in config check
	  if ( thisDMB->GetDAQMBSmokingGunIsOK() ) {
	    dmb_config_ok[current_crate_][chamber_index]++;
	  }
	  //
	  for(unsigned int cfeb_index=0;cfeb_index<thisCFEBs.size();cfeb_index++){
	    int calling_index = cfeb_index+1;
	    if ( thisDMB->GetSmokingGunIsOK(calling_index) )  {   
	      cfeb_config_ok[current_crate_][chamber_index][cfeb_index]++;
	    }
	    //	    std::cout << "smoking gun CFEB " << calling_index << " = " << cfeb_config_ok[current_crate_][chamber_index][cfeb_index];
	  }
	  //
	  // check if the currents drawn by the FPGA are within bounds or without
	  // ALCT current reading from LVMB
          float alct_lvmb_current = thisDMB->lowv_adc(3,0)/1000.;
          if(alct_lvmb_current > alct_maximum_current_value)        //the FPGA is drawing sufficient current to be called "OK"
            alct_lvmb_current_ok[current_crate_][chamber_index]++;
	  else if (alct_lvmb_current < alct_minimum_current_value)
            alct_lvmb_current_ok[current_crate_][chamber_index] += 2; //the FPGA is drawing less current than an unloaded FPGA:  blown fuse!
	  //
	  // ALCT current reading from on-board ADC
	  thisALCT->ReadAlctTemperatureAndVoltages();
	  float alct_adc_current = thisALCT->GetAlct_1p8_Current();
          if(alct_adc_current > alct_maximum_current_value)        //the FPGA is drawing sufficient current to be called "OK"
            alct_adc_current_ok[current_crate_][chamber_index]++;
	  else if (alct_adc_current < alct_minimum_current_value)
            alct_adc_current_ok[current_crate_][chamber_index] += 2; //the FPGA is drawing less current than an unloaded FPGA:  blown fuse!
	  //
	  // get the CFEB currents from LVMB.  Note, the ME1/1 have the cabling in a non-standard order on the LVMB
	  for(unsigned int cfeb_index=0;cfeb_index<thisCFEBs.size();cfeb_index++){
	    //
	    int cur18a[5]={1,1,1,2,2};
	    int cur18b[5]={1,4,7,2,5};
	    int me11_odd[5]={3,1,0,2,4};
	    int me11_even[5]={1,3,4,2,0};
	    int misscable[3][6]={
	      {+14,1,4,3,2,0},
	      {+21,3,0,1,2,4},
	      {+29,3,0,1,2,4}
	    };
	    // std::cout << " LSD febpromid " << cfeb_index << " " << thisChamber->GetLabel() 
	    // << " " << std::hex << thisDMB->febpromid(thisCFEBs[numcfeb]) << std::dec << std::endl;
	    //	    unsigned int mbid = thisDMB->mbfpgaid();
	    //	    std::cout << "LSD:DMB Problem " << thisChamber << " mbfpgaid " << mbid << std::endl;  
	    int lv=cfeb_index;
	    int ts,tr,tc;
	    sscanf(thisChamber->GetLabel().c_str(),"ME%d/%d/%d",&ts,&tr,&tc);
	    int tus=ts;
	    if(ts<0)tus=-tus;
	    if(tus==1&&tr==1){
	      if(tc%2==0){
		lv=me11_even[cfeb_index]; 
	      }else{
		lv=me11_odd[cfeb_index]; 
	      }
	      for(int t=0;t<3;t++){
		if(ts*tc==misscable[t][0]){
		  lv=misscable[t][cfeb_index+1];
		}
	      } 
	    }
	    float cfeb_current = thisDMB->lowv_adc(cur18a[lv],cur18b[lv])/1000.;
	    if(cfeb_current > cfeb_maximum_current_value) {       
	      //the FPGA is drawing sufficient current to be called "OK"
	      cfeb_current_ok[current_crate_][chamber_index][cfeb_index]++;
	    } else if (cfeb_current < cfeb_minimum_current_value) {
	      //the FPGA is drawing less current than an unloaded FPGA:  blown fuse!
	      cfeb_current_ok[current_crate_][chamber_index][cfeb_index] += 2; 
	    }
	  }  //loop over cfeb current check 
	}   // if (slot<22)
      }     // loop over chambers in crate
    }       // loop over crates
  }         // loop over hard resets
  //
  // Now that we've done the hard resets and checked all the checks, next we make a list of which components need their firmware reloaded
  //
  crate_to_reload.clear();
  slot_to_reload.clear();
  component_to_reload.clear();
  component_string.clear();
  reason_for_reload.clear();
  loaded_ok.clear();
  //
  for(unsigned crate_index=0; crate_index< crateVector.size(); crate_index++){
    //
    SetCurrentCrate(crate_index);
    if (!thisCrate->IsAlive()) continue;
    //
    if (ccb_firmware_ok[crate_index] != number_of_checks_) {
      //
      int number_of_bad_readings = number_of_checks_ - ccb_firmware_ok[crate_index];
      //
      crate_to_reload.push_back(crate_index);
      slot_to_reload.push_back(thisCCB->slot());
      component_to_reload.push_back(CCB_LABEL);
      //
      std::ostringstream problem_label;
      problem_label << "CCB"; 
      component_string.push_back(problem_label.str());
      //
      std::ostringstream reason;
      reason << "userID bad " << number_of_bad_readings << "/" << number_of_checks_ << " times";
      reason_for_reload.push_back(reason.str());
      loaded_ok.push_back(-1);
    }
    //
    if (mpc_firmware_ok[crate_index] != number_of_checks_) {
      //
      int number_of_bad_readings = number_of_checks_ - mpc_firmware_ok[crate_index];
      //
      std::ostringstream problem_label;
      problem_label << "MPC"; 
      component_string.push_back(problem_label.str());
      //
      crate_to_reload.push_back(crate_index);
      slot_to_reload.push_back(thisMPC->slot());
      component_to_reload.push_back(MPC_LABEL);
      std::ostringstream reason;
      reason << "userID bad " << number_of_bad_readings << "/" << number_of_checks_ << " times";
      reason_for_reload.push_back(reason.str());
      loaded_ok.push_back(-1);
    }
    //
    for (unsigned int chamber_index=0; chamber_index<dmbVector.size(); chamber_index++) {
      DAQMB * thisDMB = dmbVector[chamber_index];
      std::vector<CFEB> thisCFEBs = thisDMB->cfebs();
      int dslot = thisDMB->slot();
      //
      TMB * thisTMB   = tmbVector[chamber_index];
      int tslot = thisTMB->slot();
      //
      if (
	  //	  alctcfg_ok[crate_index][chamber_index]           != number_of_checks_ ||
	  //	  alct_adc_current_ok[crate_index][chamber_index]  != number_of_checks_ || 
	  //	  alct_firmware_ok[crate_index][chamber_index]     != number_of_checks_ ||
	  alct_lvmb_current_ok[crate_index][chamber_index] != number_of_checks_ 
	  ) {
	crate_to_reload.push_back(crate_index);
	slot_to_reload.push_back(tslot);
	component_to_reload.push_back(ALCT_LABEL);
	//
	std::ostringstream problem_label;
	problem_label << "ALCT"; 
	component_string.push_back(problem_label.str());
	//
	std::ostringstream reason;
	//	if (alct_firmware_ok[crate_index][chamber_index]     != number_of_checks_ ) {
	//	  int number_of_bad_readings = number_of_checks_ - alct_firmware_ok[crate_index][chamber_index];
	//	  reason << "userID bad " << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	//	}
	//
	//	if (alct_adc_current_ok[crate_index][chamber_index]     != number_of_checks_ ) {
	//	  int number_of_bad_readings = number_of_checks_ - alct_adc_current_ok[crate_index][chamber_index];
	//	  reason << "I(ADC) bad " << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	//	}
	//
	if (alct_lvmb_current_ok[crate_index][chamber_index]     != number_of_checks_ ) {
	  int number_of_bad_readings = number_of_checks_ - alct_lvmb_current_ok[crate_index][chamber_index];
	  reason << "I(LVMB) low, " << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	}
	//
	// Although we do not use this as a "smoking gun", we print out if the configuration "done" bit makes it to the CCB...
	if (alctcfg_ok[crate_index][chamber_index]     != number_of_checks_ ) {
	  int number_of_bad_readings = number_of_checks_ - alctcfg_ok[crate_index][chamber_index];
	  reason << "FPGA cfg done bad" << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	}
      	//
	reason_for_reload.push_back(reason.str());
	loaded_ok.push_back(-1);
      }
      //
      if (
	  //tmbcfg_ok[crate_index][chamber_index]       != number_of_checks_ || 
	  tmb_firmware_ok[crate_index][chamber_index] != number_of_checks_  
	  ) {
	crate_to_reload.push_back(crate_index);
	slot_to_reload.push_back(tslot);
	component_to_reload.push_back(TMB_LABEL);
	//
	std::ostringstream problem_label;
	problem_label << "TMB"; 
	component_string.push_back(problem_label.str());
	//
	std::ostringstream reason;
	if (tmb_firmware_ok[crate_index][chamber_index]     != number_of_checks_ ) {
	  int number_of_bad_readings = number_of_checks_ - tmb_firmware_ok[crate_index][chamber_index];
	  reason << "userID bad " << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	}
	//
	if (tmbcfg_ok[crate_index][chamber_index]     != number_of_checks_ ) {
	  int number_of_bad_readings = number_of_checks_ - tmbcfg_ok[crate_index][chamber_index];
	  reason << "cfg bad " << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	}
	//
	reason_for_reload.push_back(reason.str());
	loaded_ok.push_back(-1);
      }
      //
      if (dmb_vme_firmware_ok[crate_index][chamber_index] != number_of_checks_ ) { 
	crate_to_reload.push_back(crate_index);
	slot_to_reload.push_back(dslot);
	component_to_reload.push_back(DMB_VME_LABEL);
	//
	std::ostringstream problem_label;
	problem_label << "DMB VME"; 
	component_string.push_back(problem_label.str());
	//
	std::ostringstream reason;
	if (dmb_vme_firmware_ok[crate_index][chamber_index]     != number_of_checks_ ) {
	  int number_of_bad_readings = number_of_checks_ - dmb_vme_firmware_ok[crate_index][chamber_index];
	  reason << "userID bad " << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	}
	//
	reason_for_reload.push_back(reason.str());
	loaded_ok.push_back(-1);
      }
      //  
      if (
	  //dmb_control_firmware_ok[crate_index][chamber_index] < number_of_checks_ ||
	  dmb_config_ok[crate_index][chamber_index]   < number_of_checks_ 
	  ) {
	crate_to_reload.push_back(crate_index);
	slot_to_reload.push_back(dslot);
	component_to_reload.push_back(DMB_CONTROL_LABEL);
	//
	std::ostringstream problem_label;
	problem_label << "DMB Control FPGA"; 
	component_string.push_back(problem_label.str());
	//
	std::ostringstream reason;
	//	if (dmb_control_firmware_ok[crate_index][chamber_index]     < number_of_checks_ ) {
	//	  int number_of_bad_readings = number_of_checks_ - dmb_control_firmware_ok[crate_index][chamber_index];
	//	  reason << "userID bad " << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	//	}
	//
	if (dmb_config_ok[crate_index][chamber_index] < number_of_checks_ ) {
	  int number_of_bad_readings = number_of_checks_ - dmb_config_ok[crate_index][chamber_index];
	  reason << "Config bits wrong, " << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	}
	//
	reason_for_reload.push_back(reason.str());
	loaded_ok.push_back(-1);
      }
      //
      for(unsigned int cfeb_index=0;cfeb_index<thisCFEBs.size();cfeb_index++){
	if (
	    //	    cfeb_firmware_ok[crate_index][chamber_index][cfeb_index] < number_of_checks_ ||
	    cfeb_config_ok[crate_index][chamber_index][cfeb_index]   < number_of_checks_ ||
	    cfeb_current_ok[crate_index][chamber_index][cfeb_index]  < number_of_checks_ 
	    ) {
	  crate_to_reload.push_back(crate_index);
	  slot_to_reload.push_back(dslot);
	  component_to_reload.push_back(CFEB_LABEL[cfeb_index]);
	  //
	  std::ostringstream problem_label;
	  problem_label << "CFEB " << cfeb_index+1; 
	  component_string.push_back(problem_label.str());
	  //
	  std::ostringstream reason;
	  //	  if (cfeb_firmware_ok[crate_index][chamber_index][cfeb_index]     < number_of_checks_ ) {
	  //	    int number_of_bad_readings = number_of_checks_ - cfeb_firmware_ok[crate_index][chamber_index][cfeb_index];
	  //	    reason << "userID bad " << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	  //	  }
	  //	  //
	  if (cfeb_current_ok[crate_index][chamber_index][cfeb_index]     < number_of_checks_ ) {
	    int number_of_bad_readings = number_of_checks_ - cfeb_current_ok[crate_index][chamber_index][cfeb_index];
	    reason << "I(LVMB) low, " << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	  }
	  //
	  if (cfeb_config_ok[crate_index][chamber_index][cfeb_index]     < number_of_checks_ ) {
	    int number_of_bad_readings = number_of_checks_ - cfeb_config_ok[crate_index][chamber_index][cfeb_index];
	    reason << "Config bits high, " << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	  }
	  //
	  // Although we do not use this as a "smoking gun", we print out if the configuration "done" bit makes it to the CCB...
	  if (dmbcfg_ok[crate_index][chamber_index] != number_of_checks_ ) {
	    //	    int number_of_bad_readings = number_of_checks_ - dmbcfg_ok[crate_index][chamber_index];
	    reason << "FPGA cfg done bad"; // << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	  }
	  //
	  reason_for_reload.push_back(reason.str());
	  loaded_ok.push_back(-1);
	}
      } 
      //
    }    //loop over chambers 
  }      //loop over crates
  //
  //
  // Log the output...
  //
  char date_and_time[13];
  //
  OutputCheckFirmware.str(""); //clear the output string
  //
  // get the date and time of this check:
  time_t rawtime;
  struct tm * timeinfo;
  //
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  //
  int yearAD = timeinfo->tm_year + 1900;
  int month_counting_from_one = timeinfo->tm_mon + 1;
  int day = timeinfo->tm_mday;
  int hour = timeinfo->tm_hour;     
  int minute = timeinfo->tm_min;     
  //
  //
  sprintf(date_and_time,"%4u%02u%02u_%02u%02u",yearAD,month_counting_from_one,day,hour,minute);
  //
  //  *out                << "date_time = " << date_and_time << cgicc::br() << std::endl;
  OutputCheckFirmware << "date_time = " << date_and_time                << std::endl;
  OutputCheckFirmware << "Crate, Chamber, slot, Component, Problem "    << std::endl;
  //
  //
  for (unsigned problem_index=0; problem_index<crate_to_reload.size(); problem_index++) {
    int problem_crate     = crate_to_reload[problem_index];
    int problem_slot      = slot_to_reload[problem_index];
    //
    SetCurrentCrate(problem_crate);
    //
    int within_crate_problem_index = -1;
    for (unsigned int chamber_index=0; chamber_index<dmbVector.size(); chamber_index++) {
      DAQMB * thisDMB = dmbVector[chamber_index];
      int dslot = thisDMB->slot();
      //
      TMB * thisTMB   = tmbVector[chamber_index];
      int tslot = thisTMB->slot();
      //
      if (dslot == problem_slot || tslot == problem_slot) 
	within_crate_problem_index = chamber_index;
    }
    //
    if (within_crate_problem_index < 0) continue;  // this is a CCB or MPC.  skip it for the time being
    //
    Chamber * thisChamber = chamberVector[within_crate_problem_index];
    //
    OutputCheckFirmware << thisCrate->GetLabel()            << ", ";
    OutputCheckFirmware << thisChamber->GetLabel()          << ", ";
    OutputCheckFirmware << problem_slot                     << ", ";
    OutputCheckFirmware << component_string[problem_index]  << ", ";
    OutputCheckFirmware << reason_for_reload[problem_index] << std::endl;
    //
  }
  // write the output file
  //
  // The peripheral crate labels have the convention:  VME[p,n]N_M.  Here we use 
  // the "p" or "n" to label which endcap we are checking the firmware status on...
  const char * crate_name = crateVector[0]->GetLabel().c_str();
  char endcap_side = crate_name[3];
  //
  // This file is hardcoded as FirmwareDir_/status_check/YEARMODA_HRMN_[p,n]_firmware_status.log
  char filename[200];
  sprintf(filename,"%s/status_check/%s_%c_firmware_status.log",FirmwareDir_.c_str(),date_and_time,endcap_side);
  //
  //  std::cout << "filename = " << filename << std::endl;
  //
  std::ofstream LogFileCheckFirmware;
  LogFileCheckFirmware.open(filename);
  LogFileCheckFirmware << OutputCheckFirmware.str() ;
  LogFileCheckFirmware.close();
  //
  firmware_checked_ = 1;
  SetCurrentCrate(initial_crate);
  //
  this->PowerOnFixCFEB(in,out);
}
//
void EmuPeripheralCrateConfig::PowerOnFixCFEB(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //
  MyHeader(in,out,"Check System Firmware");
  //
  std::string GoToMainPage = toolbox::toString("/%s/MainPage",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",GoToMainPage) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Go back to Yellow Page").set("style","color:black") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  char buf[200];
  //
  //  *out << cgicc::br();
  //  std::string SetNumberOfHardResets = toolbox::toString("/%s/SetNumberOfHardResets",getApplicationDescriptor()->getURN().c_str());
  //  *out << cgicc::form().set("method","GET").set("action",SetNumberOfHardResets) << std::endl ;
  //  *out << cgicc::input().set("type","submit").set("value","Number of hard resets").set("style","color:black") << std::endl ;
  //  sprintf(buf,"%d",number_of_hard_resets_);
  //  *out << cgicc::input().set("type","text").set("value",buf).set("name","number_of_hard_resets") << std::endl ;
  //  *out << cgicc::form() << std::endl ;
  //  *out << cgicc::br();
  //  //
  //  if (number_of_hard_resets_ > 0) {
  //    *out << cgicc::br();
  //    *out << "WARNING, you are going to check the firmware " << number_of_checks_ 
  //	 << " times, sending " << number_of_hard_resets_ << " hard resets" << cgicc::br();
  //    *out << "---> DO NOT DO THIS IN THE MIDDLE OF A RUN <---" << cgicc::br();
  //    *out << cgicc::br();
  //  }
  //
  *out << cgicc::br();
  //
  *out << cgicc::table().set("border","2");
  //
  *out << cgicc::tr();
  *out << cgicc::td().set("ALIGN","center");
  *out << cgicc::b(" --> DO NOT CHECK FIRMWARE DURING A GLOBAL RUN <-- ").set("style","color:red");
  *out << cgicc::td() << std::endl;
  *out << cgicc::tr() << std::endl;
  //
  *out << cgicc::tr();
  *out << cgicc::td().set("ALIGN","center");
  *out << cgicc::b(" --> STOP XMAS MONITORING BEFORE FIRMWARE CHECK <-- ").set("style","color:blue");
  *out << cgicc::td() << std::endl;
  *out << cgicc::tr() << std::endl;
  //
  *out << cgicc::tr();
  *out << cgicc::td().set("ALIGN","center");
  std::string CheckFirmware = toolbox::toString("/%s/CheckFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CheckFirmware) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Check firmware in DMBs, CFEBs and ALCTs").set("style","color:black") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td() << std::endl;
  *out << cgicc::tr() << std::endl;
  //
  *out << cgicc::table();
  //
  //
  *out << cgicc::br();
  //
  int initial_crate = current_crate_;
  //
  if (firmware_checked_ == 1) {
    //
    // list the problems found...
    //
    *out << "The following FPGAs showed problems..." << std::endl;
    *out << cgicc::table().set("border","1");
    *out << cgicc::tr();
    *out << cgicc::td() << " Crate "     << cgicc::td();
    *out << cgicc::td() << " Chamber "   << cgicc::td();
    *out << cgicc::td() << " Slot "      << cgicc::td();
    *out << cgicc::td() << " Component " << cgicc::td();
    *out << cgicc::td() << " Reason "    << cgicc::td();
    *out << cgicc::tr() << std::endl;
    //
    for (unsigned problem_index=0; problem_index<crate_to_reload.size(); problem_index++) {
      int problem_crate     = crate_to_reload[problem_index];
      int problem_slot      = slot_to_reload[problem_index];
      int problem_component = component_to_reload[problem_index];
      //
      SetCurrentCrate(problem_crate);
      //
      int within_crate_problem_index = -1;
      for (unsigned int chamber_index=0; chamber_index<dmbVector.size(); chamber_index++) {
	DAQMB * thisDMB = dmbVector[chamber_index];
        int dslot = thisDMB->slot();
	//
	TMB * thisTMB   = tmbVector[chamber_index];
	int tslot = thisTMB->slot();
	//
	if (dslot == problem_slot || tslot == problem_slot) 
	  within_crate_problem_index = chamber_index;
      }
      //
      if (within_crate_problem_index < 0) continue;  // this is a CCB or MPC.  skip it for the time being
      //
      Chamber * thisChamber = chamberVector[within_crate_problem_index];
      //
      *out << cgicc::tr();
      *out << cgicc::td() << thisCrate->GetLabel()            << cgicc::td();
      *out << cgicc::td() << thisChamber->GetLabel()          << cgicc::td();
      *out << cgicc::td() << problem_slot                     << cgicc::td();
      *out << cgicc::td() << component_string[problem_index]  << cgicc::td();
      //
      // has the expert already acknowledged that this is a problem?
      bool known_problem = false;
      //
      if (thisChamber->GetExpectedConfigProblemTMB()   && problem_component == TMB_LABEL         ) known_problem = true;
      if (thisChamber->GetExpectedConfigProblemALCT()  && problem_component == ALCT_LABEL        ) known_problem = true;
      if (thisChamber->GetExpectedConfigProblemDMB()   &&(problem_component == DMB_CONTROL_LABEL||
							  problem_component == DMB_VME_LABEL)    ) known_problem = true;
      if (thisChamber->GetExpectedConfigProblemCFEB1() && problem_component == CFEB_LABEL[0]     ) known_problem = true;
      if (thisChamber->GetExpectedConfigProblemCFEB2() && problem_component == CFEB_LABEL[1]     ) known_problem = true;
      if (thisChamber->GetExpectedConfigProblemCFEB3() && problem_component == CFEB_LABEL[2]     ) known_problem = true;
      if (thisChamber->GetExpectedConfigProblemCFEB4() && problem_component == CFEB_LABEL[3]     ) known_problem = true;
      if (thisChamber->GetExpectedConfigProblemCFEB5() && problem_component == CFEB_LABEL[4]     ) known_problem = true;
      //
      if (known_problem) { 	// The expert has acknowledged this problem.  Label it as such...
	//
	*out << cgicc::td() << "Known problem" << cgicc::td();
	//
      } else {                  // The expert has NOT acknowledged this problem.  It is new and allow the user to fix it...
	//
	*out << cgicc::td() << reason_for_reload[problem_index] << cgicc::td();
	//
	*out << cgicc::td();
	std::string FixCFEB = toolbox::toString("/%s/FixCFEB",getApplicationDescriptor()->getURN().c_str());
	*out << cgicc::form().set("method","GET").set("action",FixCFEB) << std::endl ;
	if (loaded_ok[problem_index] < 0) {
	  *out << cgicc::input().set("type","submit").set("value","Load Firmware").set("style","color:blue") << std::endl ;
	} else if (loaded_ok[problem_index] == 0) {
	  *out << cgicc::input().set("type","submit").set("value","Load Firmware").set("style","color:green") << std::endl ;
	} else if (loaded_ok[problem_index] > 0) {
	  *out << cgicc::input().set("type","submit").set("value","Load Firmware").set("style","color:red") << std::endl ;
	}
	sprintf(buf,"%d",problem_crate);
	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ncrt");
	sprintf(buf,"%d",within_crate_problem_index);
	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ndmb");
	sprintf(buf,"%d",problem_component);
	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ncfeb");
	sprintf(buf,"%d",problem_index);
	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ncmd"); 
	*out << cgicc::form() << std::endl ;
	*out << cgicc::td();
	//
	//	*out << cgicc::td();
	//	*out << cgicc::form().set("method","GET").set("action",FixCFEB) << std::endl ;
	//	*out << cgicc::input().set("type","submit").set("value","CCB Hard Reset").set("style","color:black") << std::endl ;
	//	sprintf(buf,"%d",problem_crate);
	//	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ncrt");
	//	sprintf(buf,"%d",within_crate_problem_index);
	//	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ndmb");
	//	sprintf(buf,"%d",problem_component);
	//	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ncfeb");
	//	int ccb_hard_reset = -1;
	//	sprintf(buf,"%d",ccb_hard_reset);
	//	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ncmd"); 
	//	*out << cgicc::form() << std::endl ;
	//	*out << cgicc::td();
      }
      //
      *out << cgicc::tr() << std::endl;
    }
    *out << cgicc::table() << std::endl;
  }
  //
  SetCurrentCrate(initial_crate);
  //
}
//
void EmuPeripheralCrateConfig::FixCFEB(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  std::cout << "Entered FixCFEB" << std::endl;
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name1 = cgi.getElement("ncrt");
  int crate_index;
  if(name1 != cgi.getElements().end()) {
    crate_index = cgi["ncrt"]->getIntegerValue();
    std::cout << "crate_index = " << crate_index << std::endl;
  } else {
    std::cout << "No crate_index" << std::endl ;
    crate_index=-1;
  }
  cgicc::form_iterator name2 = cgi.getElement("ndmb");
  int chamber_index;
  if(name2 != cgi.getElements().end()) {
    chamber_index = cgi["ndmb"]->getIntegerValue();
    std::cout << "chamber_index = " << chamber_index << std::endl;
  } else {
    std::cout << "No chamber_index" << std::endl ;
    chamber_index=-1;
  }
  cgicc::form_iterator name3 = cgi.getElement("ncfeb");
  int problem_component;
  if(name3 != cgi.getElements().end()) {
    problem_component = cgi["ncfeb"]->getIntegerValue();
    std::cout << "problem_component = " << problem_component << std::endl;
  } else {
    std::cout << "No problem_component" << std::endl ;
    problem_component=-1;
  }
  //
  cgicc::form_iterator name4 = cgi.getElement("ncmd");
  int problem_index;
  if(name4 != cgi.getElements().end()) {
    problem_index = cgi["ncmd"]->getIntegerValue();
    std::cout << "problem_index = " << problem_index << std::endl;
  } else {
    std::cout << "No problem_index" << std::endl ;
    problem_index=-2;
  }
  //
  //
  int initial_crate = current_crate_;
  //
  if(crate_index>-1 && chamber_index>-1 && problem_component>-1 && problem_index>-1){
    //
    SetCurrentCrate(crate_index);
    //
    if (problem_component == CCB_LABEL) {
      //
      //
    } else if (problem_component == MPC_LABEL) {
      //
      //
    } else if (problem_component == TMB_LABEL) {
      //
      //
    } else if (problem_component == ALCT_LABEL) {
      //
      TMB * thisTMB = tmbVector[chamber_index];
      ALCTController  * thisALCT = thisTMB->alctController();
      //
      if (!thisALCT) {
	std::cout << "This ALCT not defined" << std::endl;
	SetCurrentCrate(initial_crate);
	this->PowerOnFixCFEB(in,out);
      }
      //
      int config_check = thisALCT->CheckFirmwareConfiguration();
      //
      if (config_check == 0 || config_check > 1) {
	std::cout << "----------------------------------------------------------------" << std::endl;
	std::cout << "---- ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR------" << std::endl;
	std::cout << "---- Firmware database check did not pass for this crate. ------" << std::endl;
	std::cout << "---- ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR------" << std::endl;
	std::cout << "----------------------------------------------------------------" << std::endl;
	SetCurrentCrate(initial_crate);
	this->PowerOnFixCFEB(in,out);
      }
      //
      //
      // Put CCB in FPGA mode to make the CCB ignore TTC commands (such as hard reset) during ALCT downloading...
      thisCCB->setCCBMode(CCB::VMEFPGA);
      //
      LOG4CPLUS_INFO(getApplicationLogger(), "Program ALCT firmware");
      //
      std::cout <<  "Loading ALCT firmware to slot " << thisTMB->slot() 
		<< " with " << ALCTFirmware_[chamber_index].toString() 
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
      thisTMB->SetXsvfFilename(ALCTFirmware_[chamber_index].toString().c_str());
      thisALCT->ProgramALCTProms();
      thisTMB->ClearXsvfFilename();
      loaded_ok[problem_index] = thisTMB->GetNumberOfVerifyErrors();
      //
      // programming with svf file to be deprecated, since it cannot verify...
      //  int debugMode(0);
      //  int jch(3);
      //  int status = thisALCT->SVFLoad(&jch,ALCTFirmware_[chamber_index].toString().c_str(),debugMode);
      //
      thisTMB->enableAllClocks();
      //
      if (loaded_ok[problem_index] >= 0){
	LOG4CPLUS_INFO(getApplicationLogger(), "Program ALCT firmware finished");
	std::cout << "=== Programming finished"<< std::endl;
	std::cout << "=== " << loaded_ok[problem_index] << " Verify Errors  occured" << std::endl;
      } else {
	std::cout << "=== Fatal Error. Exiting with " << loaded_ok[problem_index] << std::endl;
      }
      //
      // Put CCB back into DLOG mode to listen to TTC commands...
      thisCCB->setCCBMode(CCB::DLOG);
      //
      thisCCB->hardReset(); 
      //
    } else if (problem_component == DMB_VME_LABEL) {
      //
      DAQMB * thisDMB = dmbVector[chamber_index];
      std::cout << "DMB Load VME Firmware in slot " << thisDMB->slot() << std::endl;
      //
      if (thisDMB && thisDMB->slot()!=25) {
	//
	thisCCB->hardReset();
	//
	std::string crate=thisCrate->GetLabel();
	int slot=thisDMB->slot();
	int dmbID=brddb->CrateToDMBID(crate,slot);
	//
	unsigned short int dword[2];	
	dword[0]=dmbID&0x03ff;
	dword[1]=0xDB00;
	std::cout<<" The DMB number is set to: " << dword[0] << " from database lookup: " << dmbID << std::endl;
	char * outp=(char *)dword;
	//  
	thisDMB->epromload(RESET,DMBVmeFirmware_.toString().c_str(),1,outp);  // load mprom
	//
	::sleep(1);
	thisCCB->hardReset(); //disable this when testing the random_trigger
      }
      loaded_ok[problem_index] = 0;
      //
    } else if (problem_component == DMB_CONTROL_LABEL) {
      //
      DAQMB * thisDMB = dmbVector[chamber_index];
      std::cout << "DMB Load Control FPGA Firmware in slot " << thisDMB->slot() << std::endl;
      //
      if (thisDMB && thisDMB->slot()!=25) {
	//
	thisCCB->hardReset();
	//
	unsigned short int dword[2];
	dword[0]=0;
	char *outp=(char *)dword;
	thisDMB->epromload(MPROM,DMBFirmware_.toString().c_str(),1,outp);  // load mprom
	//
	::sleep(5);
	thisCCB->hardReset();
      }
      loaded_ok[problem_index] = 0;
      //
    } else if (problem_component == CFEB_LABEL[0] ||
	       problem_component == CFEB_LABEL[1] ||
	       problem_component == CFEB_LABEL[2] ||
	       problem_component == CFEB_LABEL[3] ||
	       problem_component == CFEB_LABEL[4] ) {
      //
      int cfeb_index = problem_component - CFEB_LABEL[0];      // begin CFEB indexing at 0
      //
      DAQMB * thisDMB = dmbVector[chamber_index];
      std::vector<CFEB> thisCFEBs = thisDMB->cfebs();
      //
      unsigned short int dword[2];
      for (unsigned int i=0; i<thisCFEBs.size(); i++) {
	if (thisCFEBs[i].number() == cfeb_index ) {
	  dword[0]=thisDMB->febpromuser(thisCFEBs[i]);
	  CFEBid_[chamber_index][i] = dword[0];  // fill summary file with user ID value read from this CFEB
	  char * outp=(char *)dword;   // recast dword
	  thisDMB->epromload(thisCFEBs[i].promDevice(),CFEBFirmware_.toString().c_str(),1,outp);  // load mprom
	  thisCCB->hardReset(); 
	}
      }
      loaded_ok[problem_index] = 0;
      //    
    }
  }  else if(problem_index == -1){
    //
    SetCurrentCrate(crate_index);
    thisCCB->hardReset(); 
    //
  }
  //
  SetCurrentCrate(initial_crate);
  //
  //
  //    if(ncmd==2){
  //      // now readback bit contents of prom
  //      char * outp="....";    // recast dword
  //      thisDMB->epromload_verify(thisCFEB.promDevice(),CFEBVerify_.toString().c_str(),1,outp);  // load mprom
  //      std::cout << " time calculation " << std::endl;
  //      time_t rawtime;
  //      time(&rawtime);
  //      std::string buf;
  //      std::string time_dump = ctime(&rawtime);
  //      std::string time = time_dump.substr(0,time_dump.length()-1);
  //      while( time.find(" ",0) != std::string::npos ) {
  //        int thispos = time.find(" ",0);
  //        time.replace(thispos,1,"_");
  //      }
  //      std::cout << "time " << time << std::endl;
  //      std::string temp = toolbox::toString("mv eprom.bit /tmp/verify_%s_slot%d_cfeb%d_%s.bit",crateVector[crate_index]->GetLabel().c_str(),thisDMB->slot(),thisCFEB.number()+1,time.c_str());
  //      std::cout  << temp << std::endl;
  //      system(temp.c_str());
  
  // now reprogram the prom
  //      unsigned short int dword[2];
  //    dword[0]=thisDMB->febpromuser(thisCFEB);
  //    char * outp2=(char *)dword;   // recast dword
  //    thisDMB->epromload(thisCFEB.promDevice(),CFEBFirmware_.toString().c_str(),1,outp2);
  //    // now do a hard reset
  //    thisDMB->lowv_onoff(0x20);
  //    usleep(500000);
  //    thisDMB->lowv_onoff(0x3f);
  //
  this->PowerOnFixCFEB(in,out);
}
//
void EmuPeripheralCrateConfig::ReadbackALCTFirmware(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //
  std::cout << "Readback ALCT..." << std::endl;
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name1 = cgi.getElement("ncrt");
  int ncrt;
  if(name1 != cgi.getElements().end()) {
    ncrt = cgi["ncrt"]->getIntegerValue();
    std::cout << "ncrt " << ncrt << std::endl;
  } else {
    std::cout << "No ncrt specified, taking the current crate..." << std::endl ;
    ncrt=current_crate_;
  }
  //
  cgicc::form_iterator name2 = cgi.getElement("ntmb");
  int tmb;
  if(name2 != cgi.getElements().end()) {
    tmb = cgi["ntmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
  } else {
    std::cout << "No TMB" << std::endl ;
    tmb=-1;
  }
  //
  int initial_crate = current_crate_;
  //
  if(ncrt>-1 && tmb>-1){
    //
    SetCurrentCrate(ncrt);
    TMB * thisTMB = tmbVector[tmb];
    ALCTController * thisALCT = thisTMB->alctController();
    //
    if (thisTMB->slot() < 22) {
      //
      // Put CCB in FPGA mode to make the CCB ignore TTC commands (such as hard reset) during ALCT downloading...
      thisCCB->setCCBMode(CCB::VMEFPGA);
      //
      std::cout <<  "Reading back ALCT firmware from slot " << thisTMB->slot() 
		<< ", using " << ALCTReadback_[tmb].toString() 
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
      thisTMB->SetXsvfFilename(ALCTReadback_[tmb].toString().c_str());
      thisALCT->ProgramALCTProms();
      thisTMB->ClearXsvfFilename();
      //
      thisTMB->enableAllClocks();
      //
      // Put CCB back into DLOG mode to listen to TTC commands...
      thisCCB->setCCBMode(CCB::DLOG);
      //
    }
  }
  //
  SetCurrentCrate(initial_crate);
  //
  this->PowerOnFixCFEB(in,out);
}

//
// Another method which would be better in another class... let's make it work, first....
void EmuPeripheralCrateConfig::CheckPeripheralCrateFirmware() {
  //
  //  std::cout << "Firmware check for " << thisCrate->GetLabel() << std::endl;
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
    TMB * thisTMB             = tmbVector[chamber_index];
    ALCTController * thisALCT = thisTMB->alctController();
    DAQMB * thisDMB           = dmbVector[chamber_index];
    //
    //    Chamber * thisChamber     = chamberVector[chamber_index];
    //    std::cout << "Firmware check for " << thisCrate->GetLabel() << ", " << (thisChamber->GetLabel()).c_str() << std::endl;
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
    std::vector<CFEB> cfebs = thisDMB->cfebs() ;
    typedef std::vector<CFEB>::iterator CFEBItr;
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
////////////////////////////////////////////////////////////////////////////////////
// Expert Tools
////////////////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::ExpertToolsPage(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  char Name[100];
  sprintf(Name,"Expert Tools Page");
  //
  MyHeader(in,out,Name);
  //
  *out << cgicc::h2("!!!Do NOT click these buttons during a GLOBAL RUN!!!");
  *out << cgicc::br();
  //
  //  ///////////////////////
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("Special test configuration").set("style","color:blue") << std::endl ;
  //
  *out << cgicc::table().set("border","0");
  //
  *out << cgicc::td();
  std::string SetRadioactivityTrigger = toolbox::toString("/%s/SetRadioactivityTrigger",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",SetRadioactivityTrigger) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Enable single-layer trigger") << std::endl ;
  *out << cgicc::form() << std::endl ;;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string SetTwoLayerTriggerForSystem = toolbox::toString("/%s/SetTwoLayerTriggerForSystem",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",SetTwoLayerTriggerForSystem) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Enable two-layer trigger") << std::endl ;
  *out << cgicc::form() << std::endl ;;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string SetRadioactivityTriggerALCTOnly = toolbox::toString("/%s/SetRadioactivityTriggerALCTOnly",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",SetRadioactivityTriggerALCTOnly) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Enable single-layer trigger - ALCT only") << std::endl ;
  *out << cgicc::form() << std::endl ;;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string SetTTCDelays = toolbox::toString("/%s/SetTTCDelays",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",SetTTCDelays) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","SetTTCDelays") << std::endl ;
  *out << cgicc::form() << std::endl ;;
  *out << cgicc::td();
  //
  *out << cgicc::table() << std::endl ;
  //
  *out << cgicc::fieldset();
  //
  *out << cgicc::br();
  //
  //  ///////////////////////
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("Determine online synchronization parameters").set("style","color:blue") << std::endl ;
  //
  *out << cgicc::table().set("border","0");
  //
  *out << cgicc::td();
  std::string MeasureALCTTMBRxTxForSystem = toolbox::toString("/%s/MeasureALCTTMBRxTxForSystem",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",MeasureALCTTMBRxTxForSystem) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Find ALCT rx/tx") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string MeasureCFEBTMBRxForSystem = toolbox::toString("/%s/MeasureCFEBTMBRxForSystem",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",MeasureCFEBTMBRxForSystem) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Find CFEB rx") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string ALCTBC0ScanForSystem = toolbox::toString("/%s/ALCTBC0ScanForSystem",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ALCTBC0ScanForSystem) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Synchronize ALCT BC0") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string MeasureL1AsAndDAVsForSystem = toolbox::toString("/%s/MeasureL1AsAndDAVsForSystem",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",MeasureL1AsAndDAVsForSystem) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Find L1As and DAVs") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string QuickScanForSystem = toolbox::toString("/%s/QuickScanForSystem",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",QuickScanForSystem) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","L1As and DAVs for TOF parameters only") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::table() << std::endl ;
  //
  *out << cgicc::fieldset();
  //
  *out << cgicc::br();
  //
  //
  //  ///////////////////////
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("Test MPC to SP links").set("style","color:blue") << std::endl ;
  //
  if(prbs_test_) {
    std::string StopPRBS = toolbox::toString("/%s/StopPRBS",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",StopPRBS) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Stop PRBS test").set("style","color:red") << std::endl ;
    *out << cgicc::form() << std::endl ;;
  } else {
    std::string StartPRBS = toolbox::toString("/%s/StartPRBS",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",StartPRBS) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Start PRBS test").set("style","color:blue") << std::endl ;
    *out << cgicc::form()<< std::endl ;;
  }
  //
  *out << cgicc::fieldset();
  //
  *out << cgicc::br();
  //
  //  ///////////////////////
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("Full configuration check (including Global-run NON-safe parameters)").set("style","color:blue") 
       << std::endl ;
  //
  *out << cgicc::table().set("border","0");
  //
  *out << cgicc::td();
  std::string MeasureAllTMBVoltages = toolbox::toString("/%s/MeasureAllTMBVoltages",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",MeasureAllTMBVoltages) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Measure All TMB Voltages") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string CheckCratesConfigurationFull = toolbox::toString("/%s/CheckCratesConfigurationFull",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CheckCratesConfigurationFull) << std::endl ;
  if (all_crates_ok == 1) {
    *out << cgicc::input().set("type","submit").set("value","Check full configuration of crates").set("style","color:green") << std::endl ;
  } else if (all_crates_ok == 0) {
    *out << cgicc::input().set("type","submit").set("value","Check full configuration of crates").set("style","color:red") << std::endl ;
  } else if (all_crates_ok == -1) {
    *out << cgicc::input().set("type","submit").set("value","Check full configuration of crates").set("style","color:blue") << std::endl ;
  }
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::table() << std::endl ;
  //
  //
  int initial_crate = current_crate_;
  //
  if (print_stuff) {
    //
    *out                     << "date_time = " << date_and_time_  << cgicc::br() << std::endl;
    OutputCheckConfiguration << "date_time = " << date_and_time_                << std::endl;
    //
  }
  //
  if (all_crates_ok >= 0) {
    //
    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      //
      SetCurrentCrate(crate_number);
      //
      if ( !crate_check_ok[current_crate_] ) {
	//
	//    OutputCheckConfiguration << "Problem summary for Crate " << thisCrate->GetLabel() << "..." << std::endl;
	//
	if( !(ccb_check_ok[current_crate_]) ) {
	  *out                     << thisCrate->GetLabel() << "<span style=\"color:red\" >, CCB config fail </span>" << cgicc::br() << std::endl;
	  OutputCheckConfiguration << thisCrate->GetLabel() << ", CCB"                << std::endl;
	}
	//
	if( !(mpc_check_ok[current_crate_]) ) {
	  *out                     << thisCrate->GetLabel() << "<span style=\"color:red\" >, MPC config fail </span>" << cgicc::br() << std::endl;
	  OutputCheckConfiguration << thisCrate->GetLabel() << ", MPC"                << std::endl;
	}
	//
	for (unsigned int chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
	  //
	  if( tmb_check_ok[current_crate_][chamber_index]  > 0  ||
	      alct_check_ok[current_crate_][chamber_index] > 0  ||
	      dmb_check_ok[current_crate_][chamber_index]  > 0 ) {
	    //
	    *out << "<span style=\"color:black\" >";
	    *out                     << thisCrate->GetLabel() << ", " << (chamberVector[chamber_index]->GetLabel()).c_str();
	    OutputCheckConfiguration << thisCrate->GetLabel() << ", " << (chamberVector[chamber_index]->GetLabel()).c_str();
	    *out << "</span>";
	    //
	    bool print_description = false;
	    //
	    if( tmb_check_ok[current_crate_][chamber_index] > 0) {
	      //
	      OutputCheckConfiguration << ", TMB, " << tmb_check_ok[current_crate_][chamber_index];
	      //
	      if( tmb_check_ok[current_crate_][chamber_index]        == 1) {
		*out << "<span style=\"color:red\" > TMB config fail, </span>";
	      } else if( tmb_check_ok[current_crate_][chamber_index] == 2) {
		*out << "<span style=\"color:black\" > expected TMB config fail, </span>";
		print_description = true;
	      } else if( tmb_check_ok[current_crate_][chamber_index] == 3) {
		*out << "<span style=\"color:blue\" > did not see expected TMB config failure, </span>";
		print_description = true;
	      } else if( tmb_check_ok[current_crate_][chamber_index] == 4) {
		*out << "<span style=\"color:red\" > TMB FPGA did not program, </span>";
	      } 
	      //
	    } else {
	      //
	      *out                     << ", , ";
	      OutputCheckConfiguration << ", , ";
	    }
	    //
	    if( alct_check_ok[current_crate_][chamber_index] > 0) {
	      //
	      OutputCheckConfiguration << ", ALCT, " << alct_check_ok[current_crate_][chamber_index];
	      //
	      if( alct_check_ok[current_crate_][chamber_index]        == 1) {
		*out << "<span style=\"color:red\" > ALCT config fail, </span>";
	      } else if( alct_check_ok[current_crate_][chamber_index] == 2) {
		*out << "<span style=\"color:black\" > expected ALCT config fail, </span>";
		print_description = true;
	      } else if( alct_check_ok[current_crate_][chamber_index] == 3) {
		*out << "<span style=\"color:blue\" > did not see expected ALCT config failure, </span>";
		print_description = true;
	      } else if( alct_check_ok[current_crate_][chamber_index] == 4) {
		*out << "<span style=\"color:red\" > ALCT FPGA did not program, </span>";
	      } 
	      //
	    } else {
	      //
	      *out                     << ", , ";
	      OutputCheckConfiguration << ", , ";
	    }
	    //
	    if( dmb_check_ok[current_crate_][chamber_index] > 0) {
	      //
	      OutputCheckConfiguration << ", DMB, " << dmb_check_ok[current_crate_][chamber_index];
	      //
	      if( dmb_check_ok[current_crate_][chamber_index]        == 1) {
		*out << "<span style=\"color:red\" > DMB config fail, </span>";
	      } else if( dmb_check_ok[current_crate_][chamber_index] == 2) {
		*out << "<span style=\"color:black\" > expected DMB config fail, </span>";
		print_description = true;
	      } else if( dmb_check_ok[current_crate_][chamber_index] == 3) {
		*out << "<span style=\"color:blue\" > did not see expected DMB config failure, </span>";
		print_description = true;
	      } else if( dmb_check_ok[current_crate_][chamber_index] == 4) {
		*out << "<span style=\"color:red\" > DMB FPGA did not program, </span>";
	      } 
	      //
	    } else {
	      //
	      *out                     << ", , ";
	      OutputCheckConfiguration << ", , ";
	    }
	    //
	    if (print_description) {
	      *out                     << ", " << (chamberVector[chamber_index]->GetProblemDescription()).c_str();
	      OutputCheckConfiguration << ", " << (chamberVector[chamber_index]->GetProblemDescription()).c_str();
	    } else {
	      *out                     << ", ";
	      OutputCheckConfiguration << ", ";
	    }
	    //
	    *out                     << cgicc::br() << std::endl;
	    OutputCheckConfiguration                << std::endl;
	    //
	  } 
	}
	} else if (crate_check_ok[current_crate_] == -1) {
	//
	*out << cgicc::span().set("style","color:blue");
	*out                     << crateVector[crate_number]->GetLabel() << " Not checked" << cgicc::br();
	OutputCheckConfiguration << crateVector[crate_number]->GetLabel() << " Not checked" << std::endl;
	*out << cgicc::span() << std::endl ;
      }

    }
  }
  //
  SetCurrentCrate(initial_crate);
  //
  *out << cgicc::fieldset();
  //
  if (print_stuff) {
    //
    //Output the errors to a file...
    //
    // The peripheral crate labels have the convention:  VME[p,n]N_M.  Here we use 
    // the "p" or "n" to label which endcap we are checking the firmware status on...
    const char * crate_name = crateVector[0]->GetLabel().c_str();
    char endcap_side = crate_name[3];
    //
    // This file is hardcoded as FirmwareDir_/status_check/YEARMODA_HRMN_[p,n]_firmware_status.log
    char filename[200];
    sprintf(filename,"%s/status_check/%s_%c_configuration_check.log",FirmwareDir_.c_str(),date_and_time_,endcap_side);
    //
    //  std::cout << "filename = " << filename << std::endl;
    //
    std::ofstream LogFileCheckConfiguration;
    LogFileCheckConfiguration.open(filename);
    LogFileCheckConfiguration << OutputCheckConfiguration.str() ;
    LogFileCheckConfiguration.close();
    //
  }
  print_stuff = false;
  //
}
//
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
  this->ExpertToolsPage(in, out);
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
  this->ExpertToolsPage(in, out);
}
//
void EmuPeripheralCrateConfig::SetRadioactivityTrigger(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //  
  std::cout << "Button: Configure all crates to have the Radioactivity Trigger" << std::endl;
  //
  int initial_crate = current_crate_;
  //
  if(total_crates_>0) {
    //
    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      if ( crateVector[crate_number]->IsAlive() ) {
	//
	SetCurrentCrate(crate_number);
	//
	for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	  //
	  // std::cout << "crate = " << current_crate_ << ", TMB " << tmb << std::endl;
	  //
	  TMB * thisTMB = tmbVector[tmb];
	  ALCTController * thisALCT = thisTMB->alctController();
	  //
	  int initial_alct_nplanes_hit_pretrig = thisALCT->GetPretrigNumberOfLayers();
	  int initial_alct_nplanes_hit_pattern = thisALCT->GetPretrigNumberOfPattern();
	  thisALCT->SetPretrigNumberOfLayers(1);
	  thisALCT->SetPretrigNumberOfPattern(1);
	  thisALCT->WriteConfigurationReg();
	  //
	  int initial_clct_nplanes_hit_pretrig = thisTMB->GetHsPretrigThresh();
	  int initial_clct_nplanes_hit_pattern = thisTMB->GetMinHitsPattern();
	  thisTMB->SetHsPretrigThresh(1);
	  thisTMB->SetMinHitsPattern(1);
	  thisTMB->WriteRegister(0x70);
	  //
	  // set the number of BX's that a CFEB channel must be ON in order for TMB to be labeled as "bad"
	  int initial_cfeb_badbits_nbx = thisTMB->GetCFEBBadBitsNbx();
	  thisTMB->SetCFEBBadBitsNbx(20);
	  thisTMB->WriteRegister(0x124);
	  //
	  // Do not send triggers to the SP in this mode... it is too large of rate...
	  //	  int initial_mpc_output_enable = thisTMB->GetMpcOutputEnable();
	  //	  thisTMB->SetMpcOutputEnable(0);
	  //	  thisTMB->WriteRegister(0x86);
	  //
	  //
	  // Reset the software back to the initial values.  Leave the hardware in radioactivity mode...
	  thisALCT->SetPretrigNumberOfLayers(initial_alct_nplanes_hit_pretrig);
	  thisALCT->SetPretrigNumberOfPattern(initial_alct_nplanes_hit_pattern);
	  //
	  thisTMB->SetHsPretrigThresh(initial_clct_nplanes_hit_pretrig);
	  thisTMB->SetMinHitsPattern(initial_clct_nplanes_hit_pattern);
	  //
	  thisTMB->SetCFEBBadBitsNbx(initial_cfeb_badbits_nbx);
	  //
	  //	  thisTMB->SetMpcOutputEnable(initial_mpc_output_enable);
	}
      }
    }
  }
  //
  SetCurrentCrate(initial_crate);
  //
  this->ExpertToolsPage(in,out);
}
//
void EmuPeripheralCrateConfig::SetTwoLayerTrigger(int tmb) {
  //
  TMB * thisTMB = tmbVector[tmb];
  ALCTController * thisALCT = thisTMB->alctController();
  //
  int initial_alct_nplanes_hit_pretrig = thisALCT->GetPretrigNumberOfLayers();
  int initial_alct_nplanes_hit_pattern = thisALCT->GetPretrigNumberOfPattern();
  thisALCT->SetPretrigNumberOfLayers(2);
  thisALCT->SetPretrigNumberOfPattern(2);
  thisALCT->WriteConfigurationReg();
  //
  int initial_clct_nplanes_hit_pretrig = thisTMB->GetHsPretrigThresh();
  int initial_clct_nplanes_hit_pattern = thisTMB->GetMinHitsPattern();
  thisTMB->SetHsPretrigThresh(2);
  thisTMB->SetMinHitsPattern(2);
  thisTMB->WriteRegister(0x70);
  //
  // set the number of BX's that a CFEB channel must be ON in order for TMB to be labeled as "bad"
  int initial_cfeb_badbits_nbx = thisTMB->GetCFEBBadBitsNbx();
  thisTMB->SetCFEBBadBitsNbx(20);
  thisTMB->WriteRegister(0x124);
  //
  // Do not send triggers to the SP in this mode... it is too large of rate...
  //	  int initial_mpc_output_enable = thisTMB->GetMpcOutputEnable();
  //	  thisTMB->SetMpcOutputEnable(0);
  //	  thisTMB->WriteRegister(0x86);
  //
  //
  // Reset the software back to the initial values.  Leave the hardware in radioactivity mode...
  thisALCT->SetPretrigNumberOfLayers(initial_alct_nplanes_hit_pretrig);
  thisALCT->SetPretrigNumberOfPattern(initial_alct_nplanes_hit_pattern);
  //
  thisTMB->SetHsPretrigThresh(initial_clct_nplanes_hit_pretrig);
  thisTMB->SetMinHitsPattern(initial_clct_nplanes_hit_pattern);
  //
  thisTMB->SetCFEBBadBitsNbx(initial_cfeb_badbits_nbx);
  //
  //	  thisTMB->SetMpcOutputEnable(initial_mpc_output_enable);
  //
  return;
}
void EmuPeripheralCrateConfig::SetTwoLayerTriggerForSystem(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //  
  std::cout << "Button: Configure all crates to trigger on 2-layers ALCT*CLCT" << std::endl;
  //
  int initial_crate = current_crate_;
  //
  if(total_crates_>0) {
    //
    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      if ( crateVector[crate_number]->IsAlive() ) {
	//
	SetCurrentCrate(crate_number);
	//
	for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	  // std::cout << "crate = " << current_crate_ << ", TMB " << tmb << std::endl;
	  SetTwoLayerTrigger(tmb);
	}
      }
    }
  }
  //
  SetCurrentCrate(initial_crate);
  //
  this->ExpertToolsPage(in,out);
}
//
void EmuPeripheralCrateConfig::SetTwoLayerTriggerForCrate(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //  
  std::cout << "Button: Configure all chambers in this crate to trigger on 2-layers ALCT*CLCT" << std::endl;
  //
  if ( crateVector[current_crate_]->IsAlive() ) {
    //
    for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
      // std::cout << "crate = " << current_crate_ << ", TMB " << tmb << std::endl;
      SetTwoLayerTrigger(tmb);
    }
  }
  //
  this->CrateConfiguration(in,out);
}
//
void EmuPeripheralCrateConfig::SetRadioactivityTriggerALCTOnly(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //  
  std::cout << "Button: Configure all crates to have the Radioactivity Trigger for ALCT only" << std::endl;
  //
  int initial_crate = current_crate_;
  //
  if(total_crates_>0) {
    //
    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      if ( crateVector[crate_number]->IsAlive() ) {
	//
	SetCurrentCrate(crate_number);
	//
	for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	  //
	  // std::cout << "crate = " << current_crate_ << ", TMB " << tmb << std::endl;
	  //
	  TMB * thisTMB = tmbVector[tmb];
	  ALCTController * thisALCT = thisTMB->alctController();
	  //
	  int initial_alct_nplanes_hit_pretrig = thisALCT->GetPretrigNumberOfLayers();
	  int initial_alct_nplanes_hit_pattern = thisALCT->GetPretrigNumberOfPattern();
	  thisALCT->SetPretrigNumberOfLayers(1);
	  thisALCT->SetPretrigNumberOfPattern(1);
	  thisALCT->WriteConfigurationReg();
	  //
	  // Reset the software back to the initial values.  Leave the hardware in radioactivity mode...
	  thisALCT->SetPretrigNumberOfLayers(initial_alct_nplanes_hit_pretrig);
	  thisALCT->SetPretrigNumberOfPattern(initial_alct_nplanes_hit_pattern);
	  //
	}
      }
    }
  }
  //
  SetCurrentCrate(initial_crate);
  //
  this->ExpertToolsPage(in,out);
}
//
void EmuPeripheralCrateConfig::SetTTCDelays(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //  
  std::cout << "Button: Set TTC Delays on all crates" << std::endl;
  //
  int initial_crate = current_crate_;
  //
  if(total_crates_>0) {
    //
    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      if ( crateVector[crate_number]->IsAlive() ) {
	//
	SetCurrentCrate(crate_number);
	//
	// Put CCB into FPGA mode to talk to the TTCrx board...
	thisCCB->setCCBMode(CCB::VMEFPGA);
	//
	thisCCB->HardResetTTCrx();
	::usleep(1000);
	// need to read the TTCrxID before TTCrx registers can be touched
	thisCCB->ReadTTCrxID();
	//
	// Download coarse delay to TTCrx
	int TTCrxCoarseDelay_ = thisCCB->GetTTCrxCoarseDelay();
	//
	//PrintTTCrxRegs();
	std::cout << "write TTCrxCoarseDelay_ = " << TTCrxCoarseDelay_ << " to register 2" << std::endl;
	//
	if(TTCrxCoarseDelay_>0) {
	  int delay = ((TTCrxCoarseDelay_&0xf)<<4) + (TTCrxCoarseDelay_&0xf);
	  thisCCB->WriteTTCrxReg(2,delay);
	}
	//
	// Download fine delay to TTCrx
	int TTCrxFineDelay_ = thisCCB->GetTTCrxFineDelay();
	//
	if( TTCrxFineDelay_>=0) {
	  int delay = thisCCB->ConvertNanosecondsToFineDelayUnits(TTCrxFineDelay_&0xff);
	  std::cout << "write TTCrxFineDelay_ = " << TTCrxFineDelay_ 
		    << " (TTCrx value = 0x" << std::hex << delay << ") to registers 0 and 1" 
		    << std::endl;
	  thisCCB->WriteTTCrxReg(0,delay);
	  thisCCB->WriteTTCrxReg(1,delay);
	}
	//
	// Enable TTCrx parallel output bus
	//
	thisCCB->WriteTTCrxReg(3,0xB3);  

	// check TTCrx registers
	int rx;
	rx=(int) (thisCCB->ReadTTCrxReg(2).to_ulong());
	if(((rx&0xf) != (TTCrxCoarseDelay_&0xf)) || ((rx&0xf0)>>4 != (TTCrxCoarseDelay_&0xf)))  
	  std::cout << "ERROR: TTCrx Coarse Delay register readback " << std::hex << (rx&0xff) << std::endl; 
	//
	rx=(int)(thisCCB->ReadTTCrxReg(3).to_ulong());
	if((rx&0xff) != 0xB3) 
	  std::cout << "ERROR: TTCrx Control register readback " << std::hex << (rx&0xff) << std::endl; 
	//
	std::cout << "After writing..." << std::endl;
	thisCCB->PrintTTCrxRegs();
	//
	// Put CCB back into DLOG mode to listen to TTC commands...
	thisCCB->setCCBMode(CCB::DLOG);
	//
      }
    }
  }
  //
  SetCurrentCrate(initial_crate);
  //
  this->ExpertToolsPage(in,out);
}
//
void EmuPeripheralCrateConfig::MeasureAllTMBVoltages(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //  
  std::cout << "Button: Measure voltages on all TMB's" << std::endl;
  //
  char current_date_and_time[13];
  //
  // get the date and time of this check:
  time_t rawtime;
  struct tm * timeinfo;
  //
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  //
  int yearAD = timeinfo->tm_year + 1900;
  int month_counting_from_one = timeinfo->tm_mon + 1;
  int day = timeinfo->tm_mday;
  int hour = timeinfo->tm_hour;     
  int minute = timeinfo->tm_min;     
  //
  //
  sprintf(current_date_and_time,"%4u%02u%02u_%02u%02u",yearAD,month_counting_from_one,day,hour,minute);
  std::cout  << "current_date_and_time = " << current_date_and_time << std::endl;
  //
  std::string buf;
  //  buf = "/tmp/TMBVoltages_"+current_date_and_time+".log";
  buf = "/tmp/TMBVoltages.log";
  std::cout << "Logging output to" << buf << std::endl ;
  //
  std::ofstream LogFile;
  LogFile.open(buf.c_str());
  //
  LogFile << "##date_time = " << current_date_and_time                << std::endl;
  LogFile << "##Crate, Chamber, slot, 5.0V, 5.5A, 3.3V, 3.3A, 1.5V core, 1.5A core, 1.5V TT, 1.0V TT, 1.8V RAT, 1.8A RAT" << std::endl;
  //
  int initial_crate = current_crate_;
  //
  if(total_crates_>0) {
    //
    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      if ( crateVector[crate_number]->IsAlive() ) {
	//
	SetCurrentCrate(crate_number);
	//
	for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	  //
	  Chamber * thisChamber = chamberVector[tmb];
	  TMB * thisTMB = tmbVector[tmb];
	  //
	  //	  char Name[100];
	  //	  sprintf(Name,"%s TMB status, crate=%s, slot=%d",
	  //		  (thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),thisTMB->slot());
	  // std::cout << "crate = " << current_crate_ << ", TMB " << tmb << std::endl;
	  //
	  //
	  thisTMB->ADCvoltages();
	  //
	  LogFile << ThisCrateID_.c_str() << " ";
	  LogFile << thisChamber->GetLabel() << " ";
	  LogFile << thisTMB->slot() << " ";
	  LogFile << std::setprecision(2) << std::fixed << thisTMB->Get5p0v() << " ";
	  LogFile << std::setprecision(2) << std::fixed << thisTMB->Get5p0a() << " "; 
	  LogFile << std::setprecision(2) << std::fixed << thisTMB->Get3p3v() << " ";
	  LogFile << std::setprecision(2) << thisTMB->Get3p3a() << " ";
	  LogFile << std::setprecision(3) << thisTMB->Get1p5vCore() << " ";
	  LogFile << std::setprecision(2) << thisTMB->Get1p5aCore() << " ";
	  LogFile << std::setprecision(2) << thisTMB->Get1p5vTT() << " ";
	  LogFile << std::setprecision(2) << thisTMB->Get1p0vTT() << " ";
	  LogFile << std::setprecision(2) << thisTMB->Get1p8vRAT() << " ";
	  LogFile << std::setprecision(2) << thisTMB->Get1p8aRAT() << " ";
	  LogFile << std::endl;
	}
      }
    }
  }
  //
  SetCurrentCrate(initial_crate);
  //
  LogFile.close();    
  //
  this->ExpertToolsPage(in,out);
}
//
/////////////////////////////////////////////////////////////////////
// Chamber Utilities (synchronization) methods
/////////////////////////////////////////////////////////////////////
//
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
  *out << cgicc::legend("Team A tests").set("style","color:blue") << std::endl ;
  //
  char buf[20];
  //
  //
  std::string ALCT_TMB_communication = toolbox::toString("/%s/ALCT_TMB_communication",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ALCT_TMB_communication) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Test ALCT-TMB communication") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
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
	 << " ("  << MyTest[tmb][current_crate_].GetCfebRxClockDelay(i) << ") " <<std::endl;
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
  *out << cgicc::legend("Expert tools").set("style","color:blue") << std::endl ;
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
  //
  std::string ALCTBC0Scan = toolbox::toString("/%s/ALCTBC0Scan",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ALCTBC0Scan) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Measure ALCT BC0 delay") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << "alct_bx0_delay = " << MyTest[tmb][current_crate_].GetAlctBx0DelayTest() 
       << " ("  << MyTest[tmb][current_crate_].GetAlctBx0Delay() << ") " <<std::endl;
  *out << cgicc::br();
  *out << "match_trig_alct_delay = " << MyTest[tmb][current_crate_].GetMatchTrigAlctDelayTest() 
       << " ("  << MyTest[tmb][current_crate_].GetALCTvpf_configvalue() << ") " << std::endl;
  *out << cgicc::br();
  //
  std::string QuickScanForChamber = toolbox::toString("/%s/QuickScanForChamber",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",QuickScanForChamber) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Quick Scan") << std::endl ;
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
  //  *out << "A) Measure CLCT-ALCT match timing" << std::endl;
  //  *out << "   -> Measured values are based on current values of:" << std::endl;
  //  *out << "        * match_trig_window_size" << std::endl;
  //  *out << "        * match_trig_alct_delay" << std::endl;
  //  *out << "        * mpc_tx_delay" << std::endl;
  //  *out << "(assuming that the trigger primitives have already been synchronized at the MPC)" << std::endl;
  //  *out << cgicc::pre();
  //
  //  std::string ALCTvpf = toolbox::toString("/%s/ALCTvpf",getApplicationDescriptor()->getURN().c_str());
  //  *out << cgicc::form().set("method","GET").set("action",ALCTvpf) << std::endl ;
  //  *out << cgicc::input().set("type","submit").set("value","Measure CLCT-ALCT match timing") << std::endl ;
  //  sprintf(buf,"%d",tmb);
  //  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  //  sprintf(buf,"%d",dmb);
  //  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  //  *out << cgicc::form() << std::endl ;
  //  //
  //  *out << "match_trig_alct_delay = " << MyTest[tmb][current_crate_].GetMatchTrigAlctDelayTest() 
  //       << " ("  << MyTest[tmb][current_crate_].GetALCTvpf_configvalue() << ") " << std::endl;
  //  *out << cgicc::br();
  //  *out << "mpc_tx_delay = " << MyTest[tmb][current_crate_].GetMpcTxDelayTest() 
  //       << " ("  << MyTest[tmb][current_crate_].GetMPCTxDelay_configvalue() << ") " << std::endl;
  //  *out << cgicc::br();
  //  *out << cgicc::br();
  //
  //
  *out << cgicc::pre();
  *out << "Measure the delay needed for the winner bit from MPC back to TMB" << std::endl;
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
  *out << "Find the L1A at the TMB and ALCT" << std::endl;
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
  *out << "Align the Data AVailable (DAV) bits for the ALCT" << std::endl;
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
  *out << "Find the L1A at the CFEB" << std::endl;
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
  *out << "Align the Data AVailable (DAV) bits for the CFEB" << std::endl;
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
////////////////////////////////////////////////////////////////
// Tests relevant to checking ALCT cable and rx/tx timing
////////////////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::ALCT_TMB_communication(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb = 99;
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  if( name != cgi.getElements().end() ) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "ALCT_TMB_communication:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "ALCT_TMB_communication:  No tmb" << std::endl;
    tmb = TMB_;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  alct = thisTMB->alctController();
  //
  Chamber * thisChamber = chamberVector[tmb];
  //
  char Name[100];
  sprintf(Name,"%s ALCT-TMB communication, crate=%s, TMBslot=%d",
	  (thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),thisTMB->slot());
  //
  MyHeader(in,out,Name);
  //
  char buf[20];
  //
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  //
  *out << cgicc::legend("New tests").set("style","color:blue") << std::endl ;
  //
  std::string ALCT_TMB_Loopback = toolbox::toString("/%s/ALCT_TMB_Loopback",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ALCT_TMB_Loopback) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Scan ALCT tx/rx phases") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << "alct_tx_clock_delay = " << MyTest[tmb][current_crate_].GetALCTtxPhaseTest() 
       <<  " (" << MyTest[tmb][current_crate_].GetAlctTxClockDelay() << ") " << std::endl;
  *out << cgicc::br();
  *out << "alct_rx_clock_delay = " << MyTest[tmb][current_crate_].GetALCTrxPhaseTest() 
       <<  " (" << MyTest[tmb][current_crate_].GetAlctRxClockDelay() << ") " << std::endl;
  *out << cgicc::br();
  *out << "alct_posneg = " << MyTest[tmb][current_crate_].GetAlctRxPosNegTest() 
       <<  " (" << MyTest[tmb][current_crate_].GetAlctRxPosNeg() << ") " << std::endl;
  *out << cgicc::br();
  *out << "alct_tx_posneg = " << MyTest[tmb][current_crate_].GetAlctTxPosNegTest() 
       <<  " (" << MyTest[tmb][current_crate_].GetAlctTxPosNeg() << ") " << std::endl;
  *out << cgicc::br();
  *out << cgicc::br();
  //
  std::string TMB_to_ALCT_walking_ones = toolbox::toString("/%s/TMB_to_ALCT_walking_ones",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMB_to_ALCT_walking_ones) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Check ALCT-TMB cable") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::fieldset() << std::endl;
  //
  //
  //
  *out << cgicc::form().set("method","GET") << std::endl ;
  *out << cgicc::textarea().set("name","ChamberTestOutput").set("WRAP","OFF").set("rows","20").set("cols","100");
  *out << ALCT_TMB_communicationOutput[tmb][current_crate_].str() << std::endl ;
  *out << cgicc::textarea();
  *out << cgicc::form() << std::endl ;
  //
  std::string LogALCT_TMB_communicationOutput = toolbox::toString("/%s/LogALCT_TMB_communicationOutput",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LogALCT_TMB_communicationOutput) << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","submit").set("value","Log output").set("name","LogALCT_TMB_communicationOutput") << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Clear").set("name","ClearALCT_TMB_communicationOutput") << std::endl ;
  *out << cgicc::form() << std::endl ;
}
//
void EmuPeripheralCrateConfig::ALCT_TMB_Loopback(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "ALCT_TMB_Loopback" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "ALCT_TMB_Loopback");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb = TMB_;
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "ALCT_TMB_Loopback:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "ALCT_TMB_Loopback: No tmb" << std::endl;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ALCT_TMB_communicationOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].ALCT_TMB_Loopback();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ALCT_TMB_communication(in,out);
  //
}
//
void EmuPeripheralCrateConfig::TMB_to_ALCT_walking_ones(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "TMB_to_ALCT_walking_ones" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "TMB_to_ALCT_walking_ones");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb = TMB_;
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB_to_ALCT_walking_ones:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "TMB_to_ALCT_walking_ones: No tmb" << std::endl;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ALCT_TMB_communicationOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].TMB_to_ALCT_walking_ones();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ALCT_TMB_communication(in,out);
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
  int tmb=0, dmb=0;
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
void EmuPeripheralCrateConfig::ALCTBC0Scan(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "ALCTBC0Scan" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "ALCTBC0Scan");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb = TMB_;
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "ALCTBC0Scan:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "ALCTBC0Scan" << std::endl;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].ALCTBC0Scan();
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
  int tmb=0;
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
void EmuPeripheralCrateConfig::QuickScanForChamber(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Quick Scan for chamber" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Quick Scan for chamber");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb;
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "Quick Scan for chamber:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "Quick Scan for chamber:  No tmb" << std::endl;
    tmb = TMB_;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].QuickTimingScan();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
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
void EmuPeripheralCrateConfig::QuickScanForCrate(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Quick Scan for crate" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Quick Scan for crate");
  //
  cgicc::Cgicc cgi(in);
  //
  for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) {
    //
    std::cout << "crate = " << current_crate_ << ", TMB " << i << std::endl;
    //
    MyTest[i][current_crate_].RedirectOutput(&ChamberTestsOutput[i][current_crate_]);
    MyTest[i][current_crate_].QuickTimingScan();
    MyTest[i][current_crate_].RedirectOutput(&std::cout);
  }
  //
  this->CrateConfiguration(in,out);
  //
}
//
void EmuPeripheralCrateConfig::MeasureALCTTMBRxTxForCrate(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Find ALCT rx/tx phase delays for Crate" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Find ALCT rx/tx phase delays for the crate");
  //
  cgicc::Cgicc cgi(in);
  //
  for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) {
    //
    
    std::cout << "crate = " << current_crate_ << ", TMB " << i << std::endl;
    //
    MyTest[i][current_crate_].RedirectOutput(&ALCT_TMB_communicationOutput[i][current_crate_]);
    MyTest[i][current_crate_].ALCT_TMB_Loopback();
    MyTest[i][current_crate_].RedirectOutput(&std::cout);
  }
  //
  this->CrateConfiguration(in,out);
  //
}
//
void EmuPeripheralCrateConfig::MeasureCFEBTMBRxForCrate(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Find CFEB rx phase delays for Crate" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Find CFEB rx phase delays for the crate");
  //
  cgicc::Cgicc cgi(in);
  //
  for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) {
    //
    std::cout << "crate = " << current_crate_ << ", TMB " << i << std::endl;
    //
    MyTest[i][current_crate_].RedirectOutput(&ChamberTestsOutput[i][current_crate_]);
    MyTest[i][current_crate_].CFEBTiming();
    MyTest[i][current_crate_].RedirectOutput(&std::cout);
  }
  //
  this->CrateConfiguration(in,out);
  //
}
//
void EmuPeripheralCrateConfig::ALCTBC0ScanForCrate(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Find alct_bx0_delays for Crate" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Find alct_bx0_delays for the crate");
  //
  cgicc::Cgicc cgi(in);
  //
  for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) {
    //    
    std::cout << "crate = " << current_crate_ << ", TMB " << i << std::endl;
    //
    MyTest[i][current_crate_].RedirectOutput(&ALCT_TMB_communicationOutput[i][current_crate_]);
    MyTest[i][current_crate_].ALCTBC0Scan();
    MyTest[i][current_crate_].RedirectOutput(&std::cout);
  }
  //
  this->CrateConfiguration(in,out);
  //
}
//
void EmuPeripheralCrateConfig::Settmb_bxn_offset(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb_bxn_offset");
  //
  int tmb_bxn_offset  = 3550;
  //
  if(name != cgi.getElements().end()) {
    tmb_bxn_offset = strtol(cgi["tmb_bxn_offset"]->getValue().c_str(),NULL,10);
    std::cout << "Settmb_bxn_offset = " << tmb_bxn_offset << std::endl;
  } else {
    std::cout << "No set value:  Settmb_bxn_offset = " << tmb_bxn_offset << std::endl;
  }
  //
  for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) 
    MyTest[i][current_crate_].setLocalTmbBxnOffset(tmb_bxn_offset);

  //
  this->CrateConfiguration(in,out);
  //
}
//
void EmuPeripheralCrateConfig::MeasureALCTTMBRxTxForSystem(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Find ALCT rx/tx phase delays for System" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Find ALCT rx/tx phase delays for the system");
  //
  cgicc::Cgicc cgi(in);
  //
  if(!parsed) ParsingXML();
  //
  if(total_crates_<=0) return;
  //
  for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
    //
    if ( crateVector[crate_number]->IsAlive() ) {
      //
      SetCurrentCrate(crate_number);
      //
      for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	//
	std::cout << "crate = " << current_crate_ << ", TMB " << tmb << std::endl;
	//
	MyTest[tmb][current_crate_].RedirectOutput(&ALCT_TMB_communicationOutput[tmb][current_crate_]);
	MyTest[tmb][current_crate_].ALCT_TMB_Loopback();
	MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
      }
      //
      SaveLog();
      //
      SaveTestSummary();
      //
    }
  }
  //
  this->ExpertToolsPage(in,out);
  //
}
//
void EmuPeripheralCrateConfig::MeasureCFEBTMBRxForSystem(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Find CFEB rx phase delays for System" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Find CFEB rx phase delays for the system");
  //
  cgicc::Cgicc cgi(in);
  //
  if(!parsed) ParsingXML();
  //
  if(total_crates_<=0) return;
  //
  for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
    //
    if ( crateVector[crate_number]->IsAlive() ) {
      //
      SetCurrentCrate(crate_number);
      //
      for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	//
	std::cout << "crate = " << current_crate_ << ", TMB " << tmb << std::endl;
	//
	MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
	MyTest[tmb][current_crate_].CFEBTiming();
	MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
      }
      //
      SaveLog();
      //
      SaveTestSummary();
      //
    }
  }
  //
  this->ExpertToolsPage(in,out);
  //
}
//
void EmuPeripheralCrateConfig::MeasureL1AsAndDAVsForSystem(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Find L1As and DAVs for System" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Find L1As and DAVs for the system");
  //
  cgicc::Cgicc cgi(in);
  //
  if(!parsed) ParsingXML();
  //
  if(total_crates_<=0) return;
  //
  for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
    //
    if ( crateVector[crate_number]->IsAlive() ) {
      //
      SetCurrentCrate(crate_number);
      //
      for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	//
	std::cout << "crate = " << current_crate_ << ", TMB " << tmb << std::endl;
	//
	MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
	MyTest[tmb][current_crate_].SetupRadioactiveTriggerConditions();
	MyTest[tmb][current_crate_].FindL1AAndDAVDelays();
	MyTest[tmb][current_crate_].ReturnToInitialTriggerConditions();
	MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
	//
      }
      //
      SaveLog();
      //
      SaveTestSummary();
      //
    }
  }
  //
  this->ExpertToolsPage(in,out);
  //
}
//
void EmuPeripheralCrateConfig::ALCTBC0ScanForSystem(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Find ALCT rx/tx phase delays for System" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Find ALCT rx/tx phase delays for the system");
  //
  cgicc::Cgicc cgi(in);
  //
  if(!parsed) ParsingXML();
  //
  if(total_crates_<=0) return;
  //
  for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
    //
    if ( crateVector[crate_number]->IsAlive() ) {
      //
      SetCurrentCrate(crate_number);
      //
      for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	//
	std::cout << "crate = " << current_crate_ << ", TMB " << tmb << std::endl;
	//
	MyTest[tmb][current_crate_].RedirectOutput(&ALCT_TMB_communicationOutput[tmb][current_crate_]);
	MyTest[tmb][current_crate_].ALCTBC0Scan();
	MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
      }
      //
    }
  }
  //
  // This test is so fast, we only need to save stuff at the end....
  SaveLog();
  //
  SaveTestSummary();
  //
  this->ExpertToolsPage(in,out);
  //
}
//
void EmuPeripheralCrateConfig::QuickScanForSystem(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Quick Scan for System" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Quick Scan for system");
  //
  cgicc::Cgicc cgi(in);
  //
  if(!parsed) ParsingXML();
  //
  if(total_crates_<=0) return;
  //
  for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
    //
    if ( crateVector[crate_number]->IsAlive() ) {
      //
      SetCurrentCrate(crate_number);
      //
      for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	//
	std::cout << "crate = " << current_crate_ << ", TMB " << tmb << std::endl;
	//
	MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
	MyTest[tmb][current_crate_].QuickTimingScan();
	MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
	//
      }
      //
      SaveLog();
      //
      SaveTestSummary();
      //
    }
  }
  //
  this->ExpertToolsPage(in,out);
  //
}
//
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
void EmuPeripheralCrateConfig::SetNumberOfHardResets(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "in here..." << std::endl;
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("number_of_hard_resets");
  //
  if(name != cgi.getElements().end()) {
    number_of_hard_resets_ = strtol(cgi["number_of_hard_resets"]->getValue().c_str(),NULL,10);
    std::cout << "Setting number of hard resets to " << number_of_hard_resets_ << std::endl;
    //
    number_of_checks_ = number_of_hard_resets_ + 1;
    std::cout << "Setting number of checks to " << number_of_checks_ << std::endl;
  }
  //
  this->PowerOnFixCFEB(in,out);
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
//void EmuPeripheralCrateConfig::ALCTvpf(xgi::Input * in, xgi::Output * out ) 
  //  throw (xgi::exception::Exception) {
//  //
//  std::cout << "EmuPeripheralCrate:  ALCTvpf" << std::endl;
//  LOG4CPLUS_INFO(getApplicationLogger(), "ALCTvpf");
//  //
//  cgicc::Cgicc cgi(in);
//  //
//  int tmb=0;
//  //
//  cgicc::form_iterator name = cgi.getElement("tmb");
//  //
//  if(name != cgi.getElements().end()) {
//    tmb = cgi["tmb"]->getIntegerValue();
//    std::cout << "ALCTvpf:  TMB " << tmb << " will read TMB Raw Hits "
//	 << MyTest[tmb][current_crate_].getNumberOfDataReads() << " times" << std::endl;
//    TMB_ = tmb;
//  } else {
//    std::cout << "ALCTvpf:  No tmb" << std::endl;
//  }
//  //
//  int dmb;
//  name = cgi.getElement("dmb");
//  //
//  if(name != cgi.getElements().end()) {
//    dmb = cgi["dmb"]->getIntegerValue();
//    std::cout << "ALCTvpf:  DMB " << dmb << std::endl;
//    DMB_ = dmb;
//  } else {
//    std::cout << "ALCTvpf:  No dmb" << std::endl;
//  }
//  //
//  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
//  MyTest[tmb][current_crate_].FindALCTinCLCTMatchWindow();
//  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
//  //
//  this->ChamberTests(in,out);
//}
//
void EmuPeripheralCrateConfig::ALCTScan(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "ALCTScan" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "ALCTScan");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb=0, dmb=0;
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
  int tmb=0, dmb=0;
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
  int tmb=0, dmb=0;
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
  int tmb=0, dmb=0;
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
  int tmb=0, dmb=0;
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
  int tmb=0, dmb=0;
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
  int tmb=0, dmb=0;
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
  int tmb=0, dmb=0;
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
  int tmb=0;
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
  int tmb=0;
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
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
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
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
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
  int tmb=0;
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
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
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
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
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
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
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
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
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
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
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
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
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
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
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
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "Configure:  TMB " << tmb << std::endl;
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
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
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
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
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
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
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
void EmuPeripheralCrateConfig::TMBResetSyncError(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  //
  thisTMB->SetSyncErrReset(1);
  thisTMB->WriteRegister(0x120);
  thisTMB->SetSyncErrReset(0);
  thisTMB->WriteRegister(0x120);
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
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMBRawHits:  TMB " << tmb << std::endl;
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
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "ALCTRawHits:  TMB " << tmb << std::endl;
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
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
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
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "Not tmb" << std::endl ;
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
  *out << cgicc::legend("ALCT Firmware Status").set("style","color:blue") << std::endl ;
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
       << std::endl ;
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
  *out << cgicc::legend("RAT Status").set("style","color:blue") << std::endl ;
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
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "testTMB:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  name = cgi.getElement("tmbTestid");
  //
  int tmbTestid=0;
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
  TMB * thisTMB = tmbVector[tmb];
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
  *out << cgicc::legend("TMB Info").set("style","color:blue") << std::endl ;
  //
  *out << cgicc::pre();
  //
  // read the registers:
  thisTMB->FirmwareDate();
  thisTMB->FirmwareYear();
  thisTMB->FirmwareVersion();
  thisTMB->FirmwareRevCode();
  thisTMB->ReadRegister(0xCC);
  //
  // output the register information to the screen in a nice way:
  //
  int month        = thisTMB->GetReadTmbFirmwareMonth();
  int day          = thisTMB->GetReadTmbFirmwareDay();
  int year         = thisTMB->GetReadTmbFirmwareYear();
  int compile_type = thisTMB->GetReadTMBFirmwareCompileType();
  //  std::cout << " compile type = " << std::hex << compile_type << " or... " <<  thisTMB->GetReadTMBFirmwareCompileType() << std::endl;
  //
  sprintf(buf,"TMB Firmware version (month/day/year - compile type) : (%02d/%02d/%04d - %01x)",month,day,year,compile_type);
  //
  if ( thisTMB->CheckFirmwareDate() ) {
    *out << cgicc::span().set("style","color:green");
    *out << buf;
    *out << "...OK...";
    *out << cgicc::span();
  } else {
    *out << cgicc::span().set("style","color:red");
    *out << buf;
    *out << "--->> BAD <<--- should be ("
	 << std::dec << thisTMB->GetExpectedTmbFirmwareMonth()
	 << "/"      << thisTMB->GetExpectedTmbFirmwareDay()
	 << "/"      << thisTMB->GetExpectedTmbFirmwareYear()
	 << " - "   
	 << std::hex << thisTMB->GetTMBFirmwareCompileType() 
	 << ")";

    *out << cgicc::span();
  }
  *out << cgicc::br();
  //
  //
  int firmware_type = thisTMB->GetReadTmbFirmwareType();
  //
  sprintf(buf,"Firmware Type             : %01x",firmware_type);       
  //
  if ( firmware_type == thisTMB->GetExpectedTmbFirmwareType() ) {
    *out << cgicc::span().set("style","color:green");
    *out << buf;
    *out << cgicc::span();
  } else {
    *out << cgicc::span().set("style","color:red");
    *out << buf;
    *out << "--->> BAD <<--- should be " << std::hex << thisTMB->GetExpectedTmbFirmwareType();
    *out << cgicc::span();
  }
  *out << cgicc::br();
  //
  //
  int firmware_version = thisTMB->GetReadTmbFirmwareVersion();
  //
  if ( firmware_version == thisTMB->GetExpectedTmbFirmwareVersion() ){
    *out << cgicc::span().set("style","color:green");
  } else {
    *out << cgicc::span().set("style","color:red");
  }
  sprintf(buf,"Firmware Version Code     : %01x ",firmware_version);
  *out << buf ;
  *out << cgicc::span();
  *out << cgicc::br();
  //
  //
  int slot_number = ((thisTMB->FirmwareVersion()>>8)&0x1f);
  //
  if ( slot_number == thisTMB->slot() ){
    *out << cgicc::span().set("style","color:green");
  } else {
    *out << cgicc::span().set("style","color:red");
  }
  sprintf(buf,"Geographic Address        : %02d ",slot_number);       
  *out << buf ;
  *out << cgicc::span();
  //
  *out << cgicc::br();
  //
  //
  int firmware_revcode = thisTMB->GetReadTmbFirmwareRevcode();       
  //
  sprintf(buf,"Firmware Revision Code    : %04x ",firmware_revcode);
  *out << buf ;
  *out << cgicc::br();
  //
  //
  int power_status = thisTMB->PowerComparator() & 0x1f;
  //
  if ( power_status == 0x1f ) {
    *out << cgicc::span().set("style","color:green");
  } else {
    *out << cgicc::span().set("style","color:red");
  }
  sprintf(buf,"Power Comparator          : %02x ",power_status);       
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
  *out << cgicc::legend("Comparator Badbits").set("style","color:blue") << std::endl ;
  *out << cgicc::pre();
  thisTMB->RedirectOutput(out);
  thisTMB->ReadRegister(0x122);
  thisTMB->PrintTMBRegister(0x122);
  thisTMB->ReadComparatorBadBits();
  thisTMB->PrintComparatorBadBits();
  thisTMB->RedirectOutput(&std::cout);
  *out << cgicc::pre();
  *out << cgicc::fieldset();
  //
  *out << cgicc::fieldset();
  *out << cgicc::legend("Sync Error status").set("style","color:blue") << std::endl ;
  *out << cgicc::pre();
  thisTMB->RedirectOutput(out);
  thisTMB->ReadRegister(0x120);
  thisTMB->PrintTMBRegister(0x120);
  thisTMB->RedirectOutput(&std::cout);
  *out << cgicc::pre();
  *out << cgicc::fieldset();
  //
  *out << cgicc::fieldset();
  *out << cgicc::legend("CLCT Info").set("style","color:blue") << std::endl ;
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
    *out << cgicc::legend("ALCT Info").set("style","color:blue") << std::endl ;
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
  bool print_it = false;
  for (int j=0;j<9;j++) 
    if (number_of_tmb_firmware_errors[j] >= 0) 
      print_it = true;
  //
  if (print_it) {
    for (unsigned i=0; i<tmbVector.size(); i++) {
      //
      if (number_of_tmb_firmware_errors[i] < 1) {
	*out << cgicc::span().set("style","color:black");
      } else {
	*out << cgicc::span().set("style","color:red");
      }
      *out << "Number of firmware verify errors for TMB in slot " << tmbVector[i]->slot() 
	   << " = " << number_of_tmb_firmware_errors[i] << cgicc::br() << std::endl;
      *out << cgicc::span() << std::endl ;
    }
  }
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
    *out << "firmware version = " << ALCTFirmware_[tmb].toString() << ".xsvf" << cgicc::br() << std::endl;
    //
    *out << cgicc::br() << std::endl;
    //
    *out << "Step 1)  Disable DCS monitoring to crates" << cgicc::br() << std::endl;
    //
    std::string CheckAbilityToLoadALCT = toolbox::toString("/%s/CheckAbilityToLoadALCT",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",CheckAbilityToLoadALCT) << std::endl ;
    //
    int track_checked = -1;
    //
    for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++)
      if (able_to_load_alct[i] == 0) 
	track_checked = 0;
    //
    for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++)
      if (able_to_load_alct[i] > 0) 
	track_checked++;
    //
    //
    if ( track_checked < 0 ) {
      *out << cgicc::input().set("type","submit").set("value","Step 2) ALCT firmware loading check").set("style","color:blue");
    } else if ( track_checked == 0 ) {
      *out << cgicc::input().set("type","submit").set("value","Step 2) ALCT firmware loading check").set("style","color:green");
    } else {
      *out << cgicc::input().set("type","submit").set("value","Step 2) ALCT firmware loading check").set("style","color:red");
    }
    *out << cgicc::form() << std::endl ;
    //
    //
    *out << cgicc::table().set("border","0");
    //
    *out << cgicc::td().set("ALIGN","left");
    std::string LoadALCTFirmware = toolbox::toString("/%s/LoadALCTFirmware",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",LoadALCTFirmware) << std::endl ;
    sprintf(buf,"Step 3) Load Firmware for ALCT in slot %d",tmbVector[tmb]->slot());
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
    *out << cgicc::input().set("type","submit").set("value","Step 3) Load firmware (serially) to all ALCTs in this crate") << std::endl ;
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    print_it = false;
    for (int j=0;j<9;j++) 
      if (number_of_alct_firmware_errors[j] >= 0) 
	print_it = true;
    //
    if (print_it) {
      for (unsigned i=0; i<tmbVector.size(); i++) {
	if (number_of_alct_firmware_errors[i] < 1) {
	  *out << cgicc::span().set("style","color:black");
	} else {
	  *out << cgicc::span().set("style","color:red");
	}
	*out << "Number of firmware verify errors for ALCT in slot " << tmbVector[i]->slot() 
	     << " = " << number_of_alct_firmware_errors[i] << cgicc::br() << std::endl;
	*out << cgicc::span() << std::endl ;
      }
    }
    //
    *out << cgicc::form().set("method","GET").set("action",CCBHardResetFromTMBPage) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Step 4) CCB hard reset") << std::endl ;
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
  *out << "FPGA/JTAG tools";
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string UnjamTMB = toolbox::toString("/%s/UnjamTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",UnjamTMB) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Unjam TMB JTAG chains") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //  *out << cgicc::td().set("ALIGN","left");
  //  std::string ReadbackALCTFirmware = toolbox::toString("/%s/ReadbackALCTFirmware",getApplicationDescriptor()->getURN().c_str());
  //  *out << cgicc::form().set("method","GET").set("action",ReadbackALCTFirmware) << std::endl ;
  //  *out << cgicc::input().set("type","submit").set("value","Readback ALCT PROM").set("style","color:blue") << std::endl ;
  //  sprintf(buf,"%d",tmb);
  //  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","ntmb"); 
  //  *out << cgicc::form() << std::endl ;
  //  *out << cgicc::td();
  //
  ////////////////////////////////////////
  *out << cgicc::tr();
  //
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
  *out << cgicc::td().set("ALIGN","left");
  std::string TMBResetSyncError = toolbox::toString("/%s/TMBResetSyncError",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBResetSyncError) ;
  *out << cgicc::input().set("type","submit").set("value","Clear TMB Sync Error") ;
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
  //////////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "TMB firmware";
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string TMBreadFirmware = toolbox::toString("/%s/TMBReadFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBreadFirmware) ;
  *out << cgicc::input().set("type","submit").set("value","Read back TMB firmware") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() ;
  *out << cgicc::td();
  //
  //////////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "ALCT firmware";
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string ALCTreadFirmware = toolbox::toString("/%s/ALCTReadFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ALCTreadFirmware) ;
  *out << cgicc::input().set("type","submit").set("value","Read back ALCT firmware") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() ;
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
  *out << OutputStringTMBStatus[tmb].str() << std::endl ;
  *out << cgicc::textarea();
  OutputStringTMBStatus[tmb].str("");
  *out << cgicc::pre();
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::fieldset();

}
//
void EmuPeripheralCrateConfig::TMBReadFirmware(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name2 = cgi.getElement("tmb");
  int tmb;
  if(name2 != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "Select TMB " << tmb << std::endl;
  } else {
    std::cout << "No TMB" << std::endl ;
    tmb=-1;
  }
  //
  TMB * thisTMB=NULL;
  if(tmb>=0 && (unsigned)tmb<tmbVector.size())  thisTMB = tmbVector[tmb];
  if(thisTMB)
  {
    std::string chambername= thisCrate->GetChamber(thisTMB)->GetLabel();
    chambername.replace(6,1,"_");
    chambername.replace(4,1,"_");
    std::string mcsfile="/tmp/TMB_"+ chambername + ".mcs";
    std::string jtagfile=XMLDIR+"/tmb.vrf";
    // Put CCB in FPGA mode to make the CCB ignore TTC commands (such as hard reset)
    thisCCB->setCCBMode(CCB::VMEFPGA);
      //
    std::cout  << getLocalDateTime() <<  "Reading back TMB firmware from slot " << thisTMB->slot() << std::endl;
      //
    thisTMB->setup_jtag(ChainTmbMezz);
    thisTMB->read_prom(jtagfile.c_str(),mcsfile.c_str());

    // Put CCB back into DLOG mode to listen to TTC commands...
    thisCCB->setCCBMode(CCB::DLOG);
  }
  //
  this->TMBUtils(in,out);
}
//
void EmuPeripheralCrateConfig::ALCTReadFirmware(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name2 = cgi.getElement("tmb");
  int tmb;
  if(name2 != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "Select TMB " << tmb << std::endl;
  } else {
    std::cout << "No TMB" << std::endl ;
    tmb=-1;
  }
  //
  TMB * thisTMB=NULL;
  if(tmb>=0 && (unsigned)tmb<tmbVector.size())  thisTMB = tmbVector[tmb];
  if(thisTMB)
  {
    std::string chambername= thisCrate->GetChamber(thisTMB)->GetLabel();
    chambername.replace(6,1,"_");
    chambername.replace(4,1,"_");
    std::string mcsfile="/tmp/ALCT_"+ chambername + ".mcs";
    std::string jtagfile=XMLDIR+"/alct_small.vrf";
    // Put CCB in FPGA mode to make the CCB ignore TTC commands (such as hard reset)
    thisCCB->setCCBMode(CCB::VMEFPGA);
      //
    std::cout  << getLocalDateTime() <<  "Reading back ALCT firmware from slot " << thisTMB->slot() << std::endl;
      //
    thisTMB->setup_jtag(ChainAlctFastMezz);
    thisTMB->read_prom(jtagfile.c_str(),mcsfile.c_str());

    // Put CCB back into DLOG mode to listen to TTC commands...
    thisCCB->setCCBMode(CCB::DLOG);
  }
  //
  this->TMBUtils(in,out);
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

  //  create filename for CCB & MPC
    int year  = (thisCCB->GetExpectedFirmwareYear())%100;
    int month = thisCCB->GetExpectedFirmwareMonth();
    int day   = thisCCB->GetExpectedFirmwareDay();
    char ccbdate[7];
    sprintf(ccbdate,"%02u%02u%02u",month,day,year);
    std::ostringstream CCBFirmware;
    CCBFirmware << FirmwareDir_ << "ccb/ccb2004p_" << ccbdate << ".svf";
    CCBFirmware_ = CCBFirmware.str();

    year  = (thisMPC->GetExpectedFirmwareYear())%100;
    month = thisMPC->GetExpectedFirmwareMonth();
    day   = thisMPC->GetExpectedFirmwareDay();
    char mpcdate[7];
    sprintf(mpcdate,"%02u%02u%02u",month,day,year);
    std::ostringstream MPCFirmware;
    MPCFirmware << FirmwareDir_ << "mpc/mpc2004_" << mpcdate << ".svf";
    MPCFirmware_ = MPCFirmware.str();

  //  std::cout << "CCB firmware name: " << CCBFirmware_ << std::endl;
  //  std::cout << "MPC firmware name: " << MPCFirmware_ << std::endl;

  //
  //create filename for TMB, ALCT, and RAT firmware based on expected dates...
  for (unsigned tmb=0; tmb<tmbVector.size(); tmb++) {
    
    // must skip those broadcast slots. added by Liu, May 09 2008 
    if((tmbVector[tmb]->slot())>21) continue;   
    
    //
    TMB * thisTMB = tmbVector[tmb];
    //
    year  = thisTMB->GetExpectedTmbFirmwareYear();
    month = thisTMB->GetExpectedTmbFirmwareMonth();
    day   = thisTMB->GetExpectedTmbFirmwareDay();
    char tmbdate[9];
    sprintf(tmbdate,"%04u%02u%02u",year,month,day);
    //
    std::ostringstream TMBFirmware;
    TMBFirmware << FirmwareDir_ << "tmb/" << tmbdate;
    //
    if (thisTMB->GetTMBFirmwareCompileType() == 0xa ) { 
      TMBFirmware << "/typeA";
    } else if (thisTMB->GetTMBFirmwareCompileType() == 0xc ) {
      TMBFirmware << "/typeC";
    } else if (thisTMB->GetTMBFirmwareCompileType() == 0xd ) {
      TMBFirmware << "/typeD";
    } else {
      std::cout << " = no type determined" << std::endl;
    }
    TMBFirmware << "/tmb";    // ".xsvf" is added in SetXsvfFilename
    TMBFirmware_[tmb] = TMBFirmware.str();
    //    std::cout << "TMB " << tmb << " load " << TMBFirmware_[tmb].toString() << std::endl;
    //
    //
    ALCTController  * thisALCT = tmbVector[tmb]->alctController();
    //
    year  = thisALCT->GetExpectedFastControlYear() ;
    month = thisALCT->GetExpectedFastControlMonth();
    day   = thisALCT->GetExpectedFastControlDay()  ;
    //
    char alctdate[9];
    sprintf(alctdate,"%4u%02u%02u",year,month,day);
    //
    // pre-DAQ06 format
    //  int expected_year       = thisALCT->GetExpectedFastControlYear() & 0xffff;
    //  int expected_month_tens = (thisALCT->GetExpectedFastControlMonth()>>4) & 0xf;
    //  int expected_month_ones = (thisALCT->GetExpectedFastControlMonth()>>0) & 0xf;
    //  int expected_day_tens   = (thisALCT->GetExpectedFastControlDay()  >>4) & 0xf;
    //  int expected_day_ones   = (thisALCT->GetExpectedFastControlDay()  >>0) & 0xf;
    //  sprintf(alctdate,"%4x%1x%1x%1x%1x",
    //	  expected_year,
    //	  expected_month_tens,
    //	  expected_month_ones,
    //	  expected_day_tens,
    //	  expected_day_ones);
    //
    std::ostringstream ALCTFirmware;
    std::ostringstream ALCTReadback;
    ALCTFirmware << FirmwareDir_ << "alct/" << alctdate << "/";
    ALCTReadback << FirmwareDir_ << "alct/readback/";
    //
    if ( (thisALCT->GetChamberType()).find("ME11") != std::string::npos ) {
      //
      if (thisALCT->GetExpectedFastControlBackwardForwardType() == BACKWARD_FIRMWARE_TYPE &&
	  thisALCT->GetExpectedFastControlNegativePositiveType() == NEGATIVE_FIRMWARE_TYPE ) {
	ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME11_BACKWARD_NEGATIVE;
	ALCTReadback << ALCT_READBACK_FILENAME_ME11_BACKWARD_NEGATIVE;
	//
      } else if (thisALCT->GetExpectedFastControlBackwardForwardType() == BACKWARD_FIRMWARE_TYPE &&
		 thisALCT->GetExpectedFastControlNegativePositiveType() == POSITIVE_FIRMWARE_TYPE ) {
	ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME11_BACKWARD_POSITIVE;
	ALCTReadback << ALCT_READBACK_FILENAME_ME11_BACKWARD_POSITIVE;
	//
      } else if (thisALCT->GetExpectedFastControlBackwardForwardType() == FORWARD_FIRMWARE_TYPE &&
		 thisALCT->GetExpectedFastControlNegativePositiveType() == POSITIVE_FIRMWARE_TYPE ) {
	ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME11_FORWARD_POSITIVE;
	ALCTReadback << ALCT_READBACK_FILENAME_ME11_FORWARD_POSITIVE;
	//
      } else {
	ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME11;
	ALCTReadback << ALCT_READBACK_FILENAME_ME11;
      }
    } else if ( (thisALCT->GetChamberType()).find("ME12") != std::string::npos ) {
      ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME12;
      ALCTReadback << ALCT_READBACK_FILENAME_ME12;
    } else if ( (thisALCT->GetChamberType()).find("ME13") != std::string::npos ) {
      ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME13;
      ALCTReadback << ALCT_READBACK_FILENAME_ME13;
    } else if ( (thisALCT->GetChamberType()).find("ME21") != std::string::npos ) {
      ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME21;
      ALCTReadback << ALCT_READBACK_FILENAME_ME21;
    } else if ( (thisALCT->GetChamberType()).find("ME22") != std::string::npos ) {
      ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME22;
      ALCTReadback << ALCT_READBACK_FILENAME_ME22;
    } else if ( (thisALCT->GetChamberType()).find("ME31") != std::string::npos ) {
      ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME31;
      ALCTReadback << ALCT_READBACK_FILENAME_ME31;
    } else if ( (thisALCT->GetChamberType()).find("ME32") != std::string::npos ) {
      ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME32;
      ALCTReadback << ALCT_READBACK_FILENAME_ME32;
    } else if ( (thisALCT->GetChamberType()).find("ME41") != std::string::npos ) {
      ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME41;
      ALCTReadback << ALCT_READBACK_FILENAME_ME41;
    } else if ( (thisALCT->GetChamberType()).find("ME42") != std::string::npos ) {
      ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME42;
      ALCTReadback << ALCT_READBACK_FILENAME_ME42;
    } 
    ALCTFirmware_[tmb] = ALCTFirmware.str();
    ALCTReadback_[tmb] = ALCTReadback.str();
    //    std::cout << "ALCT " << tmb << " load " << ALCTFirmware_[tmb].toString() << std::endl;
    //    std::cout << "ALCT " << tmb << " read " << ALCTReadback_[tmb].toString() << std::endl;
    //
    char date[9];
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
  TMB * thisTMB = tmbVector[tmb];
  //
  tmb_vme_ready = -1;
  //
  number_of_tmb_firmware_errors[tmb]=-1;
  //
  // Put CCB in FPGA mode to make the CCB ignore TTC commands (such as hard reset) during TMB downloading...
  thisCCB->setCCBMode(CCB::VMEFPGA);
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
    number_of_tmb_firmware_errors[tmb] = thisTMB->GetNumberOfVerifyErrors();
    std::cout << "=== Programming TMB firmware finished for slot " << thisTMB->slot() << std::endl;
    std::cout << "=== " << number_of_tmb_firmware_errors[tmb] << " Verify Errors occured" << std::endl;
    //
    if (number_of_tmb_firmware_errors[tmb] < 0) {
      std::cout << "File does not exist, programming did not occur..."<< std::endl;
      //
    } else if (number_of_tmb_firmware_errors[tmb] == 0) {
      std::cout << "Please perform a TTC/CCB hard reset to Load FPGA"<< std::endl;
      //
    } else {
      std::cout << "ERROR!! -> Number of errors = " << number_of_tmb_firmware_errors[tmb] << " not equal to 0!!" << std::endl;
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
  thisCCB->setCCBMode(CCB::DLOG);
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
  bool typeA_only = true;
  int ntmb_typea = 0;
  //
  for (int i=0; i<9;i++) 
    number_of_tmb_firmware_errors[i]=-1;
  //
  // if there is only typeA chambers in this crate, then a single broadcast will suffice...
  //
  for (unsigned ntmb=0;ntmb<(tmbVector.size()<9 ? 9 : tmbVector.size());ntmb++) {
    //
    if (tmbVector[ntmb]->GetClctStagger()) {
      ntmb_typea = ntmb;
    }
    typeA_only &= tmbVector[ntmb]->GetClctStagger();
    //
    //    std::cout << "TMB[" << ntmb << "] csc_stagger = " << tmbVector[ntmb]->GetClctStagger() << std::endl;
  }
  //
  // Create a TMB which all TMB's within a crate will listen to....
  //
  Chamber * thisChamber = new Chamber(thisCrate);  // a dummy chamber
  TMB * thisTMB = new TMB(thisCrate, thisChamber, 26); // must use a dummy chamber, not a real one
  //
  tmb_vme_ready = -1;
  //
  // Put CCB in FPGA mode to make the CCB ignore TTC commands (such as hard reset) during TMB downloading...
  thisCCB->setCCBMode(CCB::VMEFPGA);
  //
  std::cout << "Broadcast TMB firmware " << TMBFirmware_[ntmb_typea].toString()
	    << " to slot " << thisTMB->slot() << " in 5 seconds..." << std::endl;
  ::sleep(5);
  //
  thisTMB->SetXsvfFilename(TMBFirmware_[ntmb_typea].toString().c_str());
  thisTMB->ProgramTMBProms();
  thisTMB->ClearXsvfFilename();
  //
  delete thisTMB;
  //
  if (!typeA_only) {
    for (unsigned ntmb=0;ntmb<(tmbVector.size()<9 ? 9 : tmbVector.size());ntmb++) {
      //
      if (!tmbVector[ntmb]->GetClctStagger()) {
	std::cout << "Loading TMB firmware " << TMBFirmware_[ntmb].toString()
		  << " to slot " << tmbVector[ntmb]->slot() << " in 5 seconds..." << std::endl;
	::sleep(5);
	//
	tmbVector[ntmb]->SetXsvfFilename(TMBFirmware_[ntmb].toString().c_str());
	tmbVector[ntmb]->ProgramTMBProms();
	tmbVector[ntmb]->ClearXsvfFilename();
	number_of_tmb_firmware_errors[ntmb] = tmbVector[ntmb]->GetNumberOfVerifyErrors();
      }
    }
  }
  std::cout << "Please perform a TTC/CCB hard reset to Load FPGA"<< std::endl;
  //
  // Put CCB back into DLOG mode to listen to TTC commands...
  thisCCB->setCCBMode(CCB::DLOG);
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
    TMB * thisTMB = tmbVector[tmb];
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
      TMB * thisTMB = tmbVector[tmb];
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
void EmuPeripheralCrateConfig::UnjamTMB(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  //
  thisTMB->UnjamFPGA();
  //
  this->TMBUtils(in,out);
  //
}
//
//
void EmuPeripheralCrateConfig::CheckAbilityToLoadALCT(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Check ability to load firmware for all ALCTs in this crate..." << std::endl;
  //
  int check_value[10] = {};
  //
  for (unsigned i=0; i<tmbVector.size(); i++) 
    check_value[i] = tmbVector[i]->alctController()->CheckFirmwareConfiguration();
  //
  // print out the results
  //
  for (unsigned i=0; i<tmbVector.size(); i++) {
    //
    std::cout << "TMB in slot " << tmbVector[i]->slot() << " ... ";
    if (check_value[i] == 1) {
      able_to_load_alct[i] = 0;
      std::cout << "OK";
    } else if (check_value[i] == 0) {
      able_to_load_alct[i] = 1;
      std::cout << " ---> FAIL <---";
    }
    std::cout << std::endl;
  }
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
    std::cout << "Load Firmware for ALCT[" << tmb << "]" << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "No ALCT defined to load taking default = " << TMB_ << std::endl ;
    tmb = TMB_;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  ALCTController  * thisALCT = thisTMB->alctController();
  number_of_alct_firmware_errors[tmb]=-1;
  //
  if (!thisALCT) {
    std::cout << "This ALCT not defined" << std::endl;
    this->TMBUtils(in,out);
  }
  if (able_to_load_alct[tmb] != 0) {
    std::cout << "----------------------------------------------------------------" << std::endl;
    std::cout << "---- ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR------" << std::endl;
    std::cout << "---- Firmware database check did not pass for this crate. ------" << std::endl;
    std::cout << "---- ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR------" << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;
    this->TMBUtils(in,out);
  }
  // reset the ALCT check button
  able_to_load_alct[tmb] = -1;
  //
  // Put CCB in FPGA mode to make the CCB ignore TTC commands (such as hard reset) during ALCT downloading...
  thisCCB->setCCBMode(CCB::VMEFPGA);
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
  thisTMB->SetXsvfFilename(ALCTFirmware_[tmb].toString().c_str());
  thisALCT->ProgramALCTProms();
  thisTMB->ClearXsvfFilename();
  number_of_alct_firmware_errors[tmb] = thisTMB->GetNumberOfVerifyErrors();
  //
  // programming with svf file to be deprecated, since it cannot verify...
  //  int debugMode(0);
  //  int jch(3);
  //  int status = thisALCT->SVFLoad(&jch,ALCTFirmware_[tmb].toString().c_str(),debugMode);
  //
  thisTMB->enableAllClocks();
  //
  if (number_of_alct_firmware_errors[tmb] >= 0){
    LOG4CPLUS_INFO(getApplicationLogger(), "Program ALCT firmware finished");
    std::cout << "=== Programming finished"<< std::endl;
    std::cout << "=== " << number_of_alct_firmware_errors[tmb] << " Verify Errors  occured" << std::endl;
  } else {
    std::cout << "=== Fatal Error. Exiting with " << number_of_alct_firmware_errors[tmb] << std::endl;
  }
  //
  // Put CCB back into DLOG mode to listen to TTC commands...
  thisCCB->setCCBMode(CCB::DLOG);
  //
  this->TMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::LoadCrateALCTFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  for (int j=0;j<9;j++)
    number_of_alct_firmware_errors[j]=-1;
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
  thisCCB->setCCBMode(CCB::VMEFPGA);
  //
  for (unsigned i=0; i<tmbVector.size(); i++) {
    TMB * thisTMB = tmbVector[i];
    ALCTController  * thisALCT = thisTMB->alctController();
    //
    if (!thisALCT) {
      std::cout << "This ALCT not defined" << std::endl;
      this->TMBUtils(in,out);
    }
    //
    if (able_to_load_alct[i] != 0) {
      std::cout << "----------------------------------------------------------------" << std::endl;
      std::cout << "---- ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR------" << std::endl;
      std::cout << "---- Firmware database check did not pass for this crate. ------" << std::endl;
      std::cout << "---- ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR------" << std::endl;
      std::cout << "----------------------------------------------------------------" << std::endl;
      this->TMBUtils(in,out);
    }
    // reset the ALCT check button
    able_to_load_alct[i] = -1;
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
    thisTMB->SetXsvfFilename(ALCTFirmware_[i].toString().c_str());
    thisALCT->ProgramALCTProms();
    thisTMB->ClearXsvfFilename();
    number_of_alct_firmware_errors[i] = thisTMB->GetNumberOfVerifyErrors();
    //
    // programming with svf file to be deprecated, since it cannot verify...
    //    int debugMode(0);
    //    int jch(3);
    //    int status = thisALCT->SVFLoad(&jch,ALCTFirmware_[i].toString().c_str(),debugMode);
    //
    thisTMB->enableAllClocks();
    //
    if (number_of_alct_firmware_errors[i] >= 0){
      LOG4CPLUS_INFO(getApplicationLogger(), "Program ALCT firmware finished");
      std::cout << "=== Programming finished"<< std::endl;
      std::cout << "=== " << number_of_alct_firmware_errors[i] << " Verify Errors  occured" << std::endl;
    } else {
      std::cout << "=== Fatal Error. Exiting with " << number_of_alct_firmware_errors[i] << std::endl;
    }
  }
  //
  // Put CCB back into DLOG mode to listen to TTC commands...
  thisCCB->setCCBMode(CCB::DLOG);
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
  TMB * thisTMB = tmbVector[tmb];
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
  void EmuPeripheralCrateConfig::LogTestSummary(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception){
    //
    SaveTestSummary();
    //
    this->Default(in,out);
    //
  }
  //
  void EmuPeripheralCrateConfig::LogOutput(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception){
    //
    SaveLog();
    //
    this->Default(in,out);
    //
  }
void EmuPeripheralCrateConfig::SaveLog() {
  //
  int initial_crate = current_crate_;
  //
  // get the date and time of this check:
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
  buf = "/tmp/EmuPeripheralCrateLogFile_"+time+".log";
  //
  std::cout << "Logging output to" << buf << std::endl ;
  //
  std::ifstream TextFile ;
  TextFile.open(xmlFile_.toString().c_str());
  //
  std::ofstream LogFile;
  LogFile.open(buf.c_str());
  while(TextFile.good()) LogFile << (char) TextFile.get() ;
  TextFile.close();
  //
  LogFile << CrateTestsOutput.str();
  for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
    //
    SetCurrentCrate(crate_number);
    //
    for (unsigned int i=0; i<tmbVector.size(); i++) {
      LogFile << OutputTMBTests[i][current_crate_].str() ;
      LogFile << ChamberTestsOutput[i][current_crate_].str() ;
      LogFile << ALCT_TMB_communicationOutput[i][current_crate_].str() ;
    }
    for (unsigned int i=0; i<dmbVector.size(); i++) {
      LogFile << OutputDMBTests[i][current_crate_].str() ;
    }
  }
  //
  LogFile.close();    
  //
  SetCurrentCrate(initial_crate);
  //
  return;
}
//
void EmuPeripheralCrateConfig::SaveTestSummary() {
  //
  int initial_crate = current_crate_;
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
  buf = "/tmp/EmuPeripheralCrateTestSummary_"+time+".log";
  //
  std::ofstream LogFile;
  LogFile.open(buf.c_str());
  //
  LogFile << " *** Output : Test Summary *** " << std::endl ;
  //
  LogFile << std::endl;
  //
  for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
    //
    SetCurrentCrate(crate_number);
    //
    for(int i=0; i<20; i++) LogFile << "+";
    LogFile << std::endl ;
    LogFile << " Timing scans : " << std::endl;
    for(int i=0; i<20; i++) LogFile << "-";
    LogFile << std::endl ;
    //
    for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) {
      //	
      Chamber * thisChamber = chamberVector[i];
      //
      LogFile << "slot                  " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << tmbVector[i]->slot()
	      << std::endl;
      LogFile << "cfeb0delay            " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxPhaseTest(0)
	      << std::endl;
      LogFile << "cfeb1delay            " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxPhaseTest(1)
	      << std::endl;
      LogFile << "cfeb2delay            " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxPhaseTest(2)
	      << std::endl;
      LogFile << "cfeb3delay            " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxPhaseTest(3)
	      << std::endl;
      LogFile << "cfeb4delay            " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxPhaseTest(4)
	      << std::endl;
      LogFile << "cfeb0posneg           " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxPosnegTest(0)
	      << std::endl;
      LogFile << "cfeb1posneg           " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxPosnegTest(1)
	      << std::endl;
      LogFile << "cfeb2posneg           " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxPosnegTest(2)
	      << std::endl;
      LogFile << "cfeb3posneg           " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxPosnegTest(3)
	      << std::endl;
      LogFile << "cfeb4posneg           " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxPosnegTest(4)
	      << std::endl;
      LogFile << "cfeb0_rxd_int_delay   " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxdIntDelayTest(0)
	      << std::endl;
      LogFile << "cfeb1_rxd_int_delay   " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxdIntDelayTest(1)
	      << std::endl;
      LogFile << "cfeb2_rxd_int_delay   " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxdIntDelayTest(2)
	      << std::endl;
      LogFile << "cfeb3_rxd_int_delay   " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxdIntDelayTest(3)
	      << std::endl;
      LogFile << "cfeb4_rxd_int_delay   " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxdIntDelayTest(4)
	      << std::endl;
      LogFile << "alct_tx_clock_delay   " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetALCTtxPhaseTest()
	      << std::endl;
      LogFile << "alct_rx_clock_delay   " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetALCTrxPhaseTest()
	      << std::endl;
      LogFile << "alct_posneg           " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetAlctRxPosNegTest()
	      << std::endl;
      LogFile << "alct_tx_posneg        " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetAlctTxPosNegTest()
	      << std::endl;
      LogFile << "alct_bx0_delay        " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetAlctBx0DelayTest()
	      << std::endl;
      LogFile << "match_trig_alct_delay " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetMatchTrigAlctDelayTest()
	      << std::endl;
      LogFile << "tmb_bxn_offset        " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetTmbBxnOffsetTest()
	      << std::endl;
      LogFile << "rat_tmb_delay         " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetRatTmbDelayTest()
	      << std::endl;
      LogFile << "mpc_rx_delay          " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetMpcRxDelayTest()
	      << std::endl;
      LogFile << "tmb_lct_cable_delay   " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetTmbLctCableDelayTest()
	      << std::endl;
      LogFile << "alct_dav_cable_delay  " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetAlctDavCableDelayTest()
	      << std::endl;
      LogFile << "cfeb_dav_cable_delay  " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCfebDavCableDelayTest()
	      << std::endl;
      LogFile << "tmb_l1a_delay         " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetTmbL1aDelayTest()
	      << std::endl;
      LogFile << "alct_l1a_delay        " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetAlctL1aDelayTest()
	      << std::endl;
      LogFile << "rpc0_rat_delay        " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetRpcRatDelayTest(0)
	      << std::endl;
      LogFile << "TTCrxID               " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << thisCCB->GetReadTTCrxID() 
	      << std::endl;
      LogFile << "best_avg_aff_to_l1a      " 
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
  SetCurrentCrate(initial_crate);
  //
  return;
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
    int dmb=0;
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
    DAQMB * thisDMB = dmbVector[dmb];
    //
    char buf[20];
    sprintf(buf,"/tmp/DMBTestsLogFile_%d.log",thisDMB->slot());
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
    TMB * thisTMB = tmbVector[tmb];
    //
    char buf[20];
    sprintf(buf,"/tmp/TMBTestsLogFile_%d.log",thisTMB->slot());
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
    TMB * thisTMB = tmbVector[tmb];
    //
    char buf[20];
    sprintf(buf,"/tmp/ChamberTestsLogFile_%d.log",thisTMB->slot());
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
  void EmuPeripheralCrateConfig::LogALCT_TMB_communicationOutput(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    std::cout << "LogALCT_TMB_communicationOutput" << std::endl;
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
    cgicc::form_iterator name2 = cgi.getElement("ClearALCT_TMB_communicationOutput");
    //
    if(name2 != cgi.getElements().end()) {
      std::cout << "Clear..." << std::endl;
      std::cout << cgi["ClearALCT_TMB_communicationOutput"]->getValue() << std::endl ;
      ALCT_TMB_communicationOutput[tmb][current_crate_].str("");
      ALCT_TMB_communicationOutput[tmb][current_crate_] << "Chamber-Crate Phases " 
					      << thisCrate->GetChamber(tmbVector[tmb]->slot())->GetLabel().c_str() 
					      << " output:" << std::endl;
      //
      this->ALCT_TMB_communication(in,out);
      return ;
      //
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    //
    char buf[20];
    sprintf(buf,"/tmp/ALCT_TMB_communicationLogFile_%d.log",thisTMB->slot());
    //
    std::ofstream ALCT_TMB_communicationLogFile;
    ALCT_TMB_communicationLogFile.open(buf);
    ALCT_TMB_communicationLogFile << ALCT_TMB_communicationOutput[tmb][current_crate_].str() ;
    ALCT_TMB_communicationLogFile.close();
    //
    ALCT_TMB_communicationOutput[tmb][current_crate_].str("");
    //
    this->ALCT_TMB_communication(in,out);
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
    sprintf(buf,"/tmp/CrateTestsLogFile.log");
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

 }  // namespace emu::pc
}  // namespace emu

// provides factory method for instantion of HellWorld application
//
XDAQ_INSTANTIATOR_IMPL(emu::pc::EmuPeripheralCrateConfig)
