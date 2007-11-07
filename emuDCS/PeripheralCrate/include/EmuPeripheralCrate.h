#ifndef _EmuPeripheralCrate_h_
#define _EmuPeripheralCrate_h_

#include "xdaq/Application.h"
//#include "xdaq/Zone.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"

#include "xgi/Utils.h"
#include "xgi/Method.h"
#include "xdata/UnsignedLong.h"
#include "xdata/String.h"

#include "xdaq/NamespaceURI.h"
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

#include "PeripheralCrateParser.h"
#include "EmuController.h"
#include "VMEController.h"
#include "Crate.h"
#include "DAQMB.h"
#include "TMB.h"
#include "CrateUtils.h"
#include "TMBTester.h"
#include "CCB.h"
#include "MPC.h"
#include "DAQMBTester.h"
#include "TMBTester.h"
#include "ALCTController.h"
#include "RAT.h"
#include "CrateSelector.h"
#include "ChamberUtilities.h"
//#include "geom.h"
#include "InfoSpace.h"
#include "CrateUtilities.h"
#include "CalibDAQ.h"
#include "EmuSystem.h"
//
#include "EmuELog.h"

#include "EmuApplication.h"

unsigned long int DMBBoardNumber[10];
unsigned long int CFEBBoardNumber[10][5];
//
//#ifdef STANDALONE
//class EmuPeripheralCrate: public xdaq::Application 
//#else
class EmuPeripheralCrate: public EmuApplication, xdata::ActionListener
//#endif
{
  //
protected:
  //
  xdata::InfoSpace *appInfoSpace_;
  vector< pair<string, xdata::Serializable *> > stdConfigParams_;
  //
  xdata::UnsignedLong triggerSourceId_;
  //
  // EMu-specific stuff
  //
  xdata::UnsignedLong runNumber_;
  xdata::UnsignedLong maxNumTriggers_;
  //
  xdata::String xmlFile_;
  xdata::String CalibrationState_;
  //
  xdata::String CCBFirmware_;
  xdata::String MPCFirmware_;
  xdata::String TMBFirmware_;
  xdata::String DMBFirmware_;
  xdata::String DMBVmeFirmware_;
  xdata::String RATFirmware_;
  xdata::String ALCTFirmware_;
  xdata::String CFEBFirmware_;
  std::string FirmwareDir_ ;
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
  string CalibTypeStr_; 
  //
  std::string xmlFile;
  xdata::UnsignedLong myParameter_;
  EmuController * MyController;
  //
  //TMB * thisTMB ;
  //DAQMB* thisDMB ;
  //
  CCB* thisCCB ;
  ALCTController *alct ;
  RAT * rat;
  MPC * thisMPC;
  CrateUtilities myCrateTest;
  ostringstream CrateTestsOutput;
  ChamberUtilities MyTest[10];
  ostringstream ChamberTestsOutput[10];
  ostringstream OutputStringDMBStatus[10];
  ostringstream OutputStringTMBStatus[10];
  ostringstream OutputDMBTests[10];
  ostringstream OutputTMBTests[10];
  std::vector <float> ChartData[100];
  int TMBTriggerAlct0Key[120][9];
  int TMBTriggerAlct1Key[120][9];
  int TMBTriggerClct0keyHalfStrip[120][9];
  int TMBTriggerClct1keyHalfStrip[120][9];
  //
  int TMBRegisterValue_;
  int CCBRegisterValue_;
  vector<TMB*>   tmbVector;
  vector<TMBTester>   tmbTestVector;
  vector<DAQMB*> dmbVector;
  Crate *thisCrate;
  std::string Operator_;
  std::string RunNumber_;
  std::string CalibrationCfebTimeEvent_;
  std::string MPCBoardID_;
  std::string CCBBoardID_;
  std::string ControllerBoardID_;
  std::string DMBBoardID_[10];
  std::string TMBBoardID_[10];
  std::string RATBoardID_[10];
  std::string CrateChassisID_;
  std::string CrateRegulatorBoardID_;
  std::string PeripheralCrateMotherBoardID_;
  std::string ELMBID_;
  std::string BackplaneID_;
  int CFEBid_[10][5];
  int TMB_, DMB_,RAT_;
  int Counter_;
  int nTrigger_;
  //
  vector<int> L1aLctCounter_;
  vector<int> CfebDavCounter_;
  vector<int> TmbDavCounter_;
  vector<int> AlctDavCounter_;
  //
  
  //
  EmuSystem * emuSystem_;
  //
public:
  //
  XDAQ_INSTANTIATOR();
  //
  //#ifdef STANDALONE
  //EmuPeripheralCrate(xdaq::ApplicationStub * s): xdaq::Application(s) 
  //#else
  //
  EmuPeripheralCrate(xdaq::ApplicationStub * s);
  void EmuPeripheralCrate::actionPerformed (xdata::Event& e);  
  void EmuPeripheralCrate::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void EmuPeripheralCrate::MainPage(xgi::Input * in, xgi::Output * out );

  xoap::MessageReference ReadAllVmePromUserid (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference LoadAllVmePromUserid (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference ReadAllCfebPromUserid (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference LoadAllCfebPromUserid (xoap::MessageReference message) throw (xoap::exception::Exception);
  //
  xoap::MessageReference EnableJtagWriteALCT192 (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference EnableJtagWriteALCT288 (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference EnableJtagWriteALCT288bn (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference EnableJtagWriteALCT288bp (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference EnableJtagWriteALCT288fp (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference EnableJtagWriteALCT384 (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference EnableJtagWriteALCT384Mirror (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference EnableJtagWriteALCT576Mirror (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference EnableJtagWriteALCT672 (xoap::MessageReference message) throw (xoap::exception::Exception);

private:

  xoap::MessageReference onCalibration(xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onConfigure (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnable (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onDisable (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onHalt (xoap::MessageReference message) throw (xoap::exception::Exception);
  void configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception); 
  void configureFail(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception); 
  void reConfigureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception); 
  void postToELog( string subject, string body );
  void enableAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception); 
  void disableAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception); 
  void haltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception); 
  void stateChanged(toolbox::fsm::FiniteStateMachine &fsm) throw (toolbox::fsm::exception::Exception);
  void MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) throw (xgi::exception::Exception); 
  void CrateTests(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void DefineConfiguration(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CalibrationRuns(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CrateConfiguration(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CalibrationALCTThresholdScan(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CalibrationALCTConnectivity(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CalibrationCFEBConnectivity(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void FindLv1aDelayComparator(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void FindLv1aDelayALCT(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CalibrationCFEBGain(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CalibrationCFEBSaturation(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CalibrationCFEBXtalk(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CalibrationComparatorPulse(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CalibrationCFEBPedestal(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CrateXUtils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CrateDumpConfiguration(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CrateStatus(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CrateTMBCountersRight(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CrateDMBCounters(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CrateTMBCounters(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void LaunchMonitor(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void MonitorFrameRight(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception); 
  void MonitorFrameLeft(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception); 
  void MonitorTMBTrigger(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception); 
  void MonitorTMBTriggerRedirect(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception); 
  void MenuMonitorTMBTrigger(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception); 
  void AlctKey(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception); 
  std::string extractCalibrationSetting(xoap::MessageReference message);
  void ClctKey(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception); 
  void MonitorTMBTriggerDisplay(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception); 
  void CreateMonitorUnit(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception); 
  void Display(xgi::Input * in, xgi::Output * out, int counter ) throw (xgi::exception::Exception); 
  void getDataTMBTriggerAlct0Key(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void getDataTMBTriggerAlct1Key(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void getDataTMBTriggerClct0Key(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void getDataTMBTriggerClct1Key(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void getData0(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void getData1(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void getData2(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void getData3(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void getData4(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void getData5(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void getData6(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void getData7(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void getData8(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void getData9(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void getData10(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void getData11(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void getData12(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void getData13(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void getData14(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void getData15(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void getData16(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void getData17(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void getData18(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void getData19(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void getData20(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void getData21(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void SetUnsetRatio(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void SetUnsetAutoRefresh(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ResetAllCounters(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void InitSystem(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void InitChamber(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CCBBoardID(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TmbMPCTest(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void MPCSafeWindowScan(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void MPCBoardID(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void PowerUp(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void Operator(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void RunNumber(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void DMBTestAll(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void DMBTest3(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void DMBTest4(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void DMBTest5(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void DMBTest6(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void DMBTest8(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void DMBTest9(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void DMBTest10(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void DMBTest11(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void DMBPrintCounters(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ChamberTests(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void setupCoincidencePulsing(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBStartTrigger(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void EnableL1aRequest(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ALCTTiming(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void Automatic(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CFEBTiming(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBL1aTiming(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ALCTL1aTiming(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ALCTvpf(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ALCTScan(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CFEBScan(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void FindDistripHotChannel(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void FindWinner(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void AlctDavCableDelay(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TmbLctCableDelay(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CfebDavCableDelay(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void PrintDmbValuesAndScopes(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void RatTmbTiming(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void RpcRatTiming(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void MakeReference(xgi::Input * in , xgi::Output * out );
  void DMBTurnOff(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void DMBLoadFirmware(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void DMBVmeLoadFirmware(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void DMBVmeLoadFirmwareEmergency(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CFEBLoadFirmware(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CFEBLoadFirmwareID(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void DMBTurnOn(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBPrintCounters(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBResetCounters(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TriggerTestInjectALCT(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void armScope(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void forceScope(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void readoutScope(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TriggerTestInjectCLCT(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBDumpAllRegisters(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBClearUserProms(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBConfigure(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBReadConfiguration(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBCheckConfiguration(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBReadStateMachines(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBCheckStateMachines(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBRawHits(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ALCTRawHits(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ALCTStatus(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void RATStatus(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void CFEBStatus(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void CCBStatus(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void ControllerBoardID(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CrateChassisID(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CrateRegulatorBoardID(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void PeripheralCrateMotherBoardID(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ELMBID(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void BackplaneID(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ControllerUtils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void EnableDisableDebug(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void CCBUtils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void CCBLoadFirmware(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void MPCStatus(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void MPCUtils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void MPCLoadFirmware(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBTests(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void testTMB(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBStatus(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void DMBBoardID(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBBoardID(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void RATBoardID(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void DMBStatus(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBUtils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void LoadTMBFirmware(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void LoadALCTFirmware(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void LoadRATFirmware(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ReadTMBRegister(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ReadCCBRegister(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void HardReset(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void DMBUtils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void DMBTests(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void LogDMBTestsOutput(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void LogTestSummary(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void LogOutput(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void LogTMBTestsOutput(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void LogChamberTestsOutput(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void LogCrateTestsOutput(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void setRawConfFile(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ConfigureInit();
  bool Configuring();
  void setConfFile(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void getTestLogFile(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ParseTestLogFile(xdata::String logFile) throw (xgi::exception::Exception); 
  void getTestLogFileUpload(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void UploadConfFile(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  
};

#endif
