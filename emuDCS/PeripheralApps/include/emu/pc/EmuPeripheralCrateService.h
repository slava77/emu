#ifndef _EmuPeripheralCrateService_h_
#define _EmuPeripheralCrateService_h_

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
#include "emu/pc/Crate.h"
#include "emu/pc/DAQMB.h"
#include "emu/pc/TMB.h"
#include "emu/pc/CCB.h"
#include "emu/pc/MPC.h"
#include "emu/pc/EmuEndcap.h"
#include "emu/pc/EmuTStore.h"
//
#include "emu/pc/EmuPeripheralCrateBase.h"

namespace emu {
  namespace pc {

const int MAX_MESSAGES=20;

class EmuPeripheralCrateService: public EmuPeripheralCrateBase
{
  //
protected:
  //
  xdata::UnsignedLong runNumber_;
  //
  int xml_or_db;
  xdata::String  XML_or_DB_;
  xdata::String  EMU_config_ID_;

  xdata::String xmlFile_;
  //
  std::string xmlFile;
  emu::pc::EmuController * MyController;
  emu::pc::EmuTStore * myTStore;
  //
  emu::pc::CCB* thisCCB ;
  emu::pc::MPC * thisMPC;
  //
  std::vector<emu::pc::TMB*>   tmbVector;
  std::vector<emu::pc::DAQMB*> dmbVector;
  std::vector<emu::pc::Crate*> crateVector;
  std::vector<emu::pc::Chamber*> chamberVector;
  emu::pc::Crate *thisCrate;
  std::string RunNumber_;

  int parsed;
  int current_config_state_;  // index to global_config_states[]
  int current_run_state_;     // index to global_run_states[]
  std::string global_config_states[4];
  std::string global_run_states[4];
  unsigned int total_crates_;
  int this_crate_no_;
  int current_crate_;
  std::string ThisCrateID_;
  
  int endcap_side;
  bool Monitor_On_, Monitor_Ready_;
  //
  emu::pc::EmuEndcap * emuEndcap_;
  //
public:
  //
  XDAQ_INSTANTIATOR();
  //
  EmuPeripheralCrateService(xdaq::ApplicationStub * s);
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void MainPage(xgi::Input * in, xgi::Output * out );

private:

  void stateChanged(toolbox::fsm::FiniteStateMachine &fsm) throw (toolbox::fsm::exception::Exception);
  void MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) throw (xgi::exception::Exception); 
  void HardReset(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 

  void CheckCrates(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  //
  void SwitchBoard(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ForEmuPage1(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  //
  void CrateSelection(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void FastConfigCrates(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void FastConfigOne(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception); 
  void ConfigureInit(int c=0);
  bool ParsingXML();
  void SetCurrentCrate(int crate);
  void check_controllers();
  void msgHandler(std::string msg);
  
  bool Simulation_;
  int all_crates_ok;
  int crates_checked;
  int crate_check_ok[60];
  int crate_state[60];
  int last_msg;
  int total_msg;
  std::string command_msg[MAX_MESSAGES];

};

  } // namespace emu::pc
} // namespace emu
  
#endif
