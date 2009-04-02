// $Id: EmuDim.h,v 1.5 2009/04/02 14:42:14 liu Exp $

#ifndef _EmuDim_h_
#define _EmuDim_h_

#include "xgi/Utils.h"
#include "xgi/Method.h"

#include "xdaq/WebApplication.h"
#include "xdaq/Application.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"


#include "xdaq/NamespaceURI.h"

#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/Method.h"
#include "xoap/domutils.h"  // XMLCh2String()

#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

#include "xdata/exdr/Serializer.h"
#include <xdata/String.h>
#include <xdata/Float.h>
#include <xdata/Double.h>
#include <xdata/Integer.h>
#include <xdata/Boolean.h>
#include <xdata/UnsignedLong.h>
#include <xdata/UnsignedShort.h>
#include <xdata/Table.h>

#include "toolbox/Event.h"
#include "toolbox/string.h"
#include "toolbox/net/URL.h"
#include "toolbox/task/TimerTask.h"
#include "toolbox/task/Timer.h"
#include "toolbox/task/TimerFactory.h"
#include "toolbox/TimeInterval.h"

#include "dim/dis.hxx"
#include "dim/dic.hxx"

#include "emu/x2p/LOAD.h"
#include "emu/x2p/Chamber.h"

#define TOTAL_CHAMBERS 235

namespace emu {
  namespace x2p {

class EmuDim: public xdaq::WebApplication,
       public toolbox::task::TimerListener
{
  
public:
  
  XDAQ_INSTANTIATOR();
  
  //
  std::string HomeDir_;
  std::string ConfigDir_;  
  //
  xdata::String PeripheralCrateDimFile_;
  xdata::String BadChamberFile_;
  xdata::String XmasDcsUrl_;
  xdata::String BlueDcsUrl_;
  xdata::String TestPrefix_;
  xdata::Integer OpMode_;
  //
  EmuDim(xdaq::ApplicationStub * s);
  //
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void MainPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  //
  void MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) throw (xgi::exception::Exception);
  //
  // for Soap messaging
  //
  xoap::MessageReference  SoapStart(xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference  SoapStop(xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference  createReply(xoap::MessageReference message) throw (xoap::exception::Exception);

  void ButtonStart(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void ButtonStop(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void SwitchBoard(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void timeExpired (toolbox::task::TimerEvent& e);
  void Start();
  void Stop();
  void Setup ();
  void CheckCommand();
  void StartDim (int chs);
  void UpdateAllDim ();
  int UpdateDim (int ch);
  int ReadFromFile(const char *filename);
  int ReadFromXmas ();
  int ParseTXT(char *buff, int buffsize, int source);
  int FillChamber(char *buff, int source);
  int ChnameToNumber(const char *chname);
  int CrateToNumber(const char *chname);
  int PowerUp();
  std::string getLocalDateTime();
  
private:

  LOAD *XmasLoader, *BlueLoader;
  bool inited;
  Chamber chamb[TOTAL_CHAMBERS];

  LV_1_DimBroker EmuDim_lv[TOTAL_CHAMBERS];
  TEMP_1_DimBroker EmuDim_temp[TOTAL_CHAMBERS];
  DimService *LV_1_Service[TOTAL_CHAMBERS], *TEMP_1_Service[TOTAL_CHAMBERS];
  DimCommand *LV_1_Command;
  
  //
  // for monitoring
  bool Monitor_On_, Monitor_Ready_, In_Monitor_, Suspended_;
  toolbox::task::Timer * timer_;
  xdata::UnsignedShort fastloop, slowloop, extraloop;
  int current_state_;

  std::string xmas_root, xmas_load, xmas_start, xmas_stop, xmas_info;
  std::string blue_root, blue_info;
             
  int crate_state[30];
  std::string crate_name[30];
};

  } // namespace emu::x2p
} // namespace emu

#endif
