#ifndef _EmuPeripheralCrateMonitor_h_
#define _EmuPeripheralCrateMonitor_h_

#include "xdaq/Application.h"
//#include "xdaq/Zone.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"
#include "xdata/Float.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/UnsignedShort.h"
#include "xdata/Integer32.h"
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
#include "emu/pc/CCB.h"
#include "emu/pc/MPC.h"
// #include "emu/pc/ALCTController.h"
#include "emu/pc/EmuEndcap.h"
//
#include "emu/pc/EmuPeripheralCrateBase.h"

namespace emu {
  namespace pc {  
  
class EmuPeripheralCrateMonitor: public EmuPeripheralCrateBase
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
  //
  std::string xmlFile;
  EmuController * MyController;
  //
  CCB* thisCCB ;
  ALCTController *alct ;
  RAT * rat;
  MPC * thisMPC;
  //
  int tmb_vme_ready;
  //
  std::vector<TMB*>   tmbVector;
  std::vector<DAQMB*> dmbVector;
  std::vector<Crate*> crateVector;
  std::vector<Chamber*> chamberVector;
  Crate *thisCrate;
  std::string RunNumber_;

  int parsed;
  int current_config_state_;  // index to global_config_states[]
  int current_run_state_;     // index to global_run_states[]
  std::string global_config_states[4];
  std::string global_run_states[4];
  unsigned int total_crates_;
  int this_crate_no_;
  std::string ThisCrateID_;
  bool controller_checked_;
  int this_tcounter_;
  int this_dcounter_;
  int this_ocounter_;
  int DCS_this_crate_no_;
  std::string DCS_ThisCrateID_;
  std::string main_url_;
  
  int endcap_side;
  std::vector< std::string> monitorables_;
  bool Monitor_On_, Monitor_Ready_;
  bool fast_on, slow_on, extra_on, reload_vcc;
  int fast_count, slow_count, extra_count;
  xdata::UnsignedShort fastloop, slowloop, extraloop;
  toolbox::task::Timer * timer_;
  //
  std::vector<int> vcc_reset, dcs_mask, tmb_mask, dmb_mask;
  std::vector<bool> crate_off;
  //
  std::vector< std::string> TCounterName;
  std::vector< std::string> DCounterName;
  std::vector< std::string> OCounterName;
  std::vector< std::string> LVCounterName;
  std::vector< std::string> TECounterName;
  std::vector<int> IsErrCounter;
  //
  EmuEndcap * emuEndcap_;
  //
  // for beam monitor
  long long int total_min, total_max;
  double O_T_min, O_T_max, R_L_min, R_L_max, T_B_min, T_B_max;

  int dcs_station, dcs_ring, dcs_chamber;
      
public:
  //
  XDAQ_INSTANTIATOR();
  //
  EmuPeripheralCrateMonitor(xdaq::ApplicationStub * s);
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void MainPage(xgi::Input * in, xgi::Output * out );
  void MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) throw (xgi::exception::Exception); 


private:

  xoap::MessageReference onFastLoop(xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onSlowLoop(xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onExtraLoop(xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onMonitorStart(xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onMonitorStop(xoap::MessageReference message) throw (xoap::exception::Exception);

  void MonitorStart(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void MonitorStop(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void ReadingOn();
  void ReadingOff();
  void CreateEmuInfospace();
  void PublishEmuInfospace(int cycle);
  void CrateTMBCounters(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CrateDMBCounters(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ResetAllCounters(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void FullResetTMBC(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void XmlOutput(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CrateSelection(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CrateStatus(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CheckCrates(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void SwitchBoard(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  // define states
  void stateChanged(toolbox::fsm::FiniteStateMachine &fsm) throw (toolbox::fsm::exception::Exception);
  void dummyAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);
  void TCounterSelection(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void DCounterSelection(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void OCounterSelection(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ChamberView(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void CrateView(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void BeamView(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void InitCounterNames();
      
  void check_controllers();
  bool ParsingXML();
  void SetCurrentCrate(int crate);
  int current_crate_;
  //
  // all DCS related methods
  //
  void DCSOutput(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void DCSDefault(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void DCSMain(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void DCSChamSel(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void DCSStatSel(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void DCSCrateSel(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void DCSCrateLV(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void DCSCrateCUR(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void DCSCrateTemp(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void DCSChamber(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  //
  // for EmuPage1
  //
  void ForEmuPage1(xgi::Input *in, xgi::Output *out) throw (xgi::exception::Exception);
  void msgHandler(std::string msg, int msglevel=0);
};

  } // namespace emu::pc
} // namespace emu
  
#endif
