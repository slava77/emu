#ifndef _EmuPeripheralCrateConfig_h_
#define _EmuPeripheralCrateConfig_h_

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

#include "emu/pc/EmuController.h"
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
#include "emu/pc/VMECC.h"
#include "emu/pc/EMU_CC_constants.h"
#include "emu/db/BoardsDB.h"
#include "emu/pc/EmuTStore.h"
//
#include "emu/pc/EmuPeripheralCrateBase.h"

namespace emu {
  namespace pc {

class EmuPeripheralCrateConfig: public EmuPeripheralCrateBase
{
  //
protected:
  //
  xdata::UnsignedLong runNumber_;
  xdata::UnsignedLong maxNumTriggers_;
  //
  int xml_or_db;
  xdata::String  XML_or_DB_;
  xdata::String  EMU_config_ID_;

  xdata::String xmlFile_;
  xdata::String CalibrationState_;
  //
  xdata::String CCBFirmware_;
  xdata::String MPCFirmware_;
  xdata::String TMBFirmware_[10];
  xdata::String DMBFirmware_;
  xdata::String DMBVmeFirmware_;
  xdata::String RATFirmware_[10];
  xdata::String ALCTFirmware_[10];
  xdata::String ALCTReadback_[10];
  xdata::String CFEBFirmware_;
  xdata::String CFEBVerify_;
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
  emu::pc::VMECC* vmecc;
  emu::pc::CCB* thisCCB ;
  emu::pc::ALCTController *alct ;
  emu::pc::RAT * rat;
  emu::pc::MPC * thisMPC;
  emu::db::BoardsDB *brddb;
  emu::pc::CrateUtilities myCrateTest;
  std::stringstream CrateTestsOutput;
  emu::pc::ChamberUtilities MyTest[10][30];
  std::ostringstream ChamberTestsOutput[10][30];
  std::ostringstream ALCT_TMB_communicationOutput[10][30];
  std::ostringstream OutputStringDMBStatus[10];
  std::ostringstream OutputStringTMBStatus[10];
  std::ostringstream OutputDMBTests[10][30];
  std::ostringstream OutputTMBTests[10][30];
  std::ostringstream OutputCheckConfiguration;
  std::ostringstream OutputCheckFirmware;
  std::vector <float> ChartData[100];
  int TMBTriggerAlct0Key[120][9];
  int TMBTriggerAlct1Key[120][9];
  int TMBTriggerClct0keyHalfStrip[120][9];
  int TMBTriggerClct1keyHalfStrip[120][9];
  //
  int tmb_vme_ready;
  int crate_controller_status;
  //
  int CCBRegisterRead_, CCBRegisterValue_, CCBRegisterWrite_, CCBWriteValue_;
  int MPCRegisterRead_, MPCRegisterValue_, MPCRegisterWrite_, MPCWriteValue_;
  std::vector<emu::pc::TMB*>   tmbVector;
  std::vector<emu::pc::TMBTester>   tmbTestVector;
  std::vector<emu::pc::DAQMB*> dmbVector;
  std::vector<emu::pc::Crate*> crateVector;
  std::vector<emu::pc::Chamber*> chamberVector;
  emu::pc::Crate *thisCrate;
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

  int parsed;
  int current_config_state_;  // index to global_config_states[]
  int current_run_state_;     // index to global_run_states[]
  std::string global_config_states[4];
  std::string global_run_states[4];
  unsigned int total_crates_;
  int this_crate_no_;
  std::string ThisCrateID_;
  
  std::vector< std::string> monitorables_;
  bool Monitor_On_, Monitor_Ready_;
  xdata::UnsignedShort fastloop, slowloop, extraloop;
  toolbox::task::Timer * timer_;
  //
  std::vector<int> L1aLctCounter_;
  std::vector<int> CfebDavCounter_;
  std::vector<int> TmbDavCounter_;
  std::vector<int> AlctDavCounter_;
  //
  unsigned long int DMBBoardNumber[62][10];
  unsigned long int CFEBBoardNumber[62][10][7];

  //VCC Utilities

  enum VCC_PAGES {VCC_CMNTSK, VCC_VME, VCC_FRMUTIL, VCC_CNFG, VCC_MAC,
   VCC_FIFO, VCC_PKTSND, VCC_PKTRCV, VCC_MISC} VCC_UTIL_curr_page;
  emu::pc::Crate *VCC_UTIL_curr_crate;
  std::string VCC_UTIL_curr_crate_name;
  std::string VCC_UTIL_curr_color;
  std::string VCC_UTIL_acc_cntrl;
  std::string VCC_UTIL_expert_pswd;
  std::string VCC_UTIL_cmn_tsk_dbg;
  std::string VCC_UTIL_cmn_tsk_sn;
  std::string VCC_UTIL_cmn_tsk_cc;
  std::string VCC_UTIL_cmn_tsk_lpbk;
  std::string VCC_UTIL_cmn_tsk_lpbk_color;
  std::string VCC_UTIL_Frmw_rbk_[8]; 
  std::string VCC_UTIL_base_addr_inp; 
  std::string VCC_UTIL_prom_file_inp; 
  std::string VCC_UTIL_proj;
  std::string VCC_UTIL_maj_ver;
  std::string VCC_UTIL_min_ver;
  std::string VCC_UTIL_PROM_ver;
  bool VCC_UTIL_PROM_file_init;
  std::string VCC_UTIL_Prnt_Rtns;
  std::string VCC_UTIL_VME_board;
  std::string VCC_UTIL_VME_sub_addr;
  std::string VCC_UTIL_VME_rw;
  std::string VCC_UTIL_VME_data;
  std::string VCC_UTIL_VME_add_addr;
  std::string VCC_UTIL_VME_add_data;
  std::string VCC_UTIL_VME_brd_sel;
  std::string VCC_UTIL_VME_rw_sel;
  std::string VCC_UTIL_VME_send_num;
  std::string VCC_UTIL_VME_msg_data;
  std::string VCC_UTIL_VME_rbk_data;
  std::string VCC_UTIL_CR_cur_[6];
  std::string VCC_UTIL_CR_dflt_[6];
  std::string VCC_UTIL_CR_flashA_[6];
  std::string VCC_UTIL_CR_flashB_[6];
  std::string VCC_UTIL_CR_wrt_[7];
  std::string VCC_UTIL_CR_cnumA;
  std::string VCC_UTIL_CR_cnumB;
  std::string VCC_UTIL_CR_sav_cnum;
  std::string VCC_UTIL_CR_rstr_cnum;
  std::string VCC_UTIL_CR_dflt_cnum;
  std::string VCC_UTIL_CR_curr_dflt;
  std::string VCC_UTIL_CR_ser_num;
  emu::pc::CNFG_t VCC_UTIL_CR_to;
  emu::pc::CNFG_t VCC_UTIL_CR_from;
  std::string VCC_UTIL_MAC_ena_dis;
  std::string VCC_UTIL_MAC_wrt_dev;
  std::string VCC_UTIL_MAC_wrt_mcast1;
  std::string VCC_UTIL_MAC_wrt_mcast2;
  std::string VCC_UTIL_MAC_wrt_mcast3;
  std::string VCC_UTIL_MAC_wrt_dflt;
  std::string VCC_UTIL_MAC_rbk_dev;
  std::string VCC_UTIL_MAC_rbk_mcast1;
  std::string VCC_UTIL_MAC_rbk_mcast2;
  std::string VCC_UTIL_MAC_rbk_mcast3;
  std::string VCC_UTIL_MAC_rbk_dflt;
  std::string VCC_UTIL_FIFO_mode;
  std::string VCC_UTIL_FIFO_ecc;
  std::string VCC_UTIL_FIFO_inj;
  std::string VCC_UTIL_FIFO_wrt_pae;
  std::string VCC_UTIL_FIFO_wrt_paf;
  std::string VCC_UTIL_FIFO_rbk_pae;
  std::string VCC_UTIL_FIFO_rbk_paf;
  std::string VCC_UTIL_FIFO_cor_errs;
  std::string VCC_UTIL_FIFO_uncor_errs;
  std::string VCC_UTIL_FIFO_wrt_data;
  std::string VCC_UTIL_FIFO_rbk_data;
  std::string VCC_UTIL_FIFO_msg_data;
  std::string VCC_UTIL_FIFO_rd_num;
  std::string VCC_UTIL_misc_warn;
  std::string VCC_UTIL_misc_strtup;
  std::string VCC_UTIL_misc_int;
  std::string VCC_UTIL_misc_fp;
  std::string VCC_UTIL_misc_srst;
  std::string VCC_UTIL_misc_hr;
  std::string VCC_UTIL_misc_jtag;
  std::string VCC_UTIL_misc_spont;
  std::string VCC_UTIL_misc_rd_msglvl;
  std::string VCC_UTIL_misc_wrt_msglvl;
  std::string VCC_UTIL_PKTSND_prcs_tag;
  std::string VCC_UTIL_PKTSND_cmnd;
  std::string VCC_UTIL_PKTSND_data;
  std::string VCC_UTIL_PKTRCV_num_pkt;
  std::string VCC_UTIL_PKTRCV_pkts_inbuf;
  std::string VCC_UTIL_PKTRCV_wrd_cnt;
  std::string VCC_UTIL_PKTRCV_raw_pkt;
  std::string VCC_UTIL_PKTRCV_pkt_len;
  std::string VCC_UTIL_PKTRCV_dstn_addr;
  std::string VCC_UTIL_PKTRCV_src_addr;
  std::string VCC_UTIL_PKTRCV_pkt_flags;
  std::string VCC_UTIL_PKTRCV_frg_seq;
  std::string VCC_UTIL_PKTRCV_frg_seq_num;
  std::string VCC_UTIL_PKTRCV_ack_num;
  std::string VCC_UTIL_PKTRCV_ack_stat;
  std::string VCC_UTIL_PKTRCV_prc_tag;
  std::string VCC_UTIL_PKTRCV_pkt_typ_num;
  std::string VCC_UTIL_PKTRCV_pkt_type;
  std::string VCC_UTIL_PKTRCV_pkt_cmnd;
  std::string VCC_UTIL_PKTRCV_rbk_data;
  
  //
  emu::pc::EmuEndcap * emuEndcap_;
  //
public:
  //
  XDAQ_INSTANTIATOR();
  //
  EmuPeripheralCrateConfig(xdaq::ApplicationStub * s);
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void MainPage(xgi::Input * in, xgi::Output * out );

  xoap::MessageReference ReadAllVmePromUserid (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference LoadAllVmePromUserid (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference ReadAllCfebPromUserid (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference LoadAllCfebPromUserid (xoap::MessageReference message) throw (xoap::exception::Exception);
  //

private:

  void postToELog( std::string subject, std::string body );
  void stateChanged(toolbox::fsm::FiniteStateMachine &fsm) throw (toolbox::fsm::exception::Exception);
  void MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) throw (xgi::exception::Exception); 
  void VCCHeader(xgi::Input * in, xgi::Output * out, std::string title, std::string heading ) throw (xgi::exception::Exception); 
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
  void CrateDumpConfiguration(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CrateStatus(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void MonitorTMBTrigger(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception); 
  void MonitorTMBTriggerRedirect(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception); 
  void MenuMonitorTMBTrigger(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception); 
  void AlctKey(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception); 
  void ClctKey(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception); 
  void MonitorTMBTriggerDisplay(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception); 
  void CreateMonitorUnit(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception); 
  void Display(xgi::Input * in, xgi::Output * out, int counter ) throw (xgi::exception::Exception); 
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
  void RTRGlow(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void RTRGhigh(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void DMBPrintCounters(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ChamberTests(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void setupCoincidencePulsing(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBStartTrigger(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void EnableL1aRequest(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void setTMBCounterReadValues(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void setDataReadValues(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
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
  void CFEBReadFirmware(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CFEBLoadFirmwareID(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void DMBTurnOn(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBPrintCounters(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBResetCounters(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBCounterForFixedTime(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TriggerTestInjectALCT(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void armScope(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void forceScope(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void readoutScope(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TriggerTestInjectCLCT(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBDumpAllRegisters(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBClearUserProms(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void UnjamTMB(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
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
  void ControllerUtils_Xfer(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void ControllerUtils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VMECC_UTIL_Menu_Buttons(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VMECC_UTIL_Crate_Selection(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VMECCGUI_GoTo_General(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VMECCGUI_GoTo_Intermediate(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VMECCGUI_GoTo_Expert(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VCC_PSWD_DO(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VCC_CRSEL_DO(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VMECCGUI_VME_access(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VMECCGUI_firmware_utils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VMECCGUI_cnfg_utils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VMECCGUI_MAC_utils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VMECCGUI_FIFO_utils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VMECCGUI_pkt_send(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VMECCGUI_pkt_rcv(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VMECCGUI_misc_utils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VCC_CMNTSK_DO(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VCC_VME_DO(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VCC_VME_FILL(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VCC_FRMUTIL_DO(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VCC_CNFG_DO(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VCC_MAC_DO(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VCC_FIFO_DO(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VCC_PKTSND_DO(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VCC_PKTRCV_DO(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VCC_MISC_DO(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void CCBUtils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void CCBLoadFirmware(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void MPCStatus(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void MPCUtils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void MPCLoadFirmware(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void MeasureL1AsAndDAVsForCrate(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void MeasureL1AsAndDAVsForChamber(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void MeasureL1AsForCrate(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void MeasureDAVsForCrate(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void MeasureALCTTMBRxTxForCrate(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBTests(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void testTMB(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBStatus(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void DMBBoardID(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBBoardID(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void RATBoardID(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void DMBStatus(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMBUtils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CheckCrateControllerFromTMBPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void LoadTMBFirmware(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void LoadCrateTMBFirmware(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void CCBHardResetFromTMBPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);  
  void CheckTMBFirmware(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ClearTMBBootReg(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CheckAbilityToLoadALCT(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void LoadALCTFirmware(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void LoadCrateALCTFirmware(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void LoadRATFirmware(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ReadCCBRegister(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void WriteCCBRegister(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ReadMPCRegister(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void WriteMPCRegister(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ReadTTCRegister(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void HardReset(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void DMBUtils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void DMBCheckConfiguration(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void DMBTests(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void LogDMBTestsOutput(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void LogTestSummary(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void LogOutput(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void LogTMBTestsOutput(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void LogChamberTestsOutput(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 

  void LogCrateTestsOutput(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CheckCrates(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  //
  void CheckConfigurationPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CheckTimeSinceHardReset(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CheckCratesConfiguration(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CheckCrateConfiguration(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CheckCrateFirmware(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  //
  void PowerOnFixCFEB(xgi::Input * in, xgi::Output * out )throw (xgi::exception::Exception);
  void FixCFEB(xgi::Input * in, xgi::Output * out )throw (xgi::exception::Exception);
  void ReadbackALCTFirmware(xgi::Input * in, xgi::Output * out )throw (xgi::exception::Exception);
  //
  void setRawConfFile(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CrateSelection(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ConfigAllCrates(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void FastConfigCrates(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void FastConfigOne(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ConfigureInit(int c=0);
  bool ParsingXML();
  void SetCurrentCrate(int crate);
  int current_crate_;
  void setConfFile(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void UploadConfFile(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void StartPRBS(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void StopPRBS(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void SetHighRateTrigger(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void SetNormRateTrigger(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  //
  void ALCT_TMB_communication(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ALCT_TMB_Loopback(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void TMB_to_ALCT_walking_ones(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ALCTTiming(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void LogALCT_TMB_communicationOutput(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  //
  void DefineFirmwareFilenames();
  //
  std::vector<emu::pc::TMBTester> InitTMBTests(emu::pc::Crate *);
  void CheckPeripheralCrateConfiguration();
  void CheckPeripheralCrateFirmware();
  bool prbs_test_;
  int all_crates_ok;;
  int crate_check_ok[60];
  int ccb_check_ok[60];
  int mpc_check_ok[60];
  int alct_check_ok[60][9];
  int tmb_check_ok[60][9];
  int dmb_check_ok[60][9];
  int time_since_reset[60][9];
  //
  int crates_firmware_ok;;
  int crate_firmware_ok[60];
  int ccb_firmware_ok[60];
  int mpc_firmware_ok[60];
  int alct_firmware_ok[60][9];
  int tmb_firmware_ok[60][9];
  int dmb_vme_firmware_ok[60][9];
  int dmb_control_firmware_ok[60][9];
  int cfeb_firmware_ok[60][9][5];
  //
  int number_of_alct_firmware_errors[9];
  int number_of_tmb_firmware_errors[9];
  int able_to_load_alct;
  //
  bool print_stuff;
  char date_and_time_[13];
  //
    
};

  } // namespace emu::pc
} // namespace emu
  
#endif
