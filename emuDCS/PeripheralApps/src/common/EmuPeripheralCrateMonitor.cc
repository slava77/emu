// $Id: EmuPeripheralCrateMonitor.cc

#include "emu/pc/EmuPeripheralCrateMonitor.h"

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

/////////////////////////////////////////////////////////////////////
// Instantiation and main page
/////////////////////////////////////////////////////////////////////
EmuPeripheralCrateMonitor::EmuPeripheralCrateMonitor(xdaq::ApplicationStub * s): EmuPeripheralCrateBase(s)
{	
  //
  DisplayRatio_ = false;
  AutoRefresh_  = true;
  MyController = 0;
  total_min = 0;
  total_max = 0;
  O_T_min = 0.;
  O_T_max = 0.;
  R_L_min = 0.;
  R_L_max = 0.;
  T_B_min = 0.;
  T_B_max = 0.;
  //
  tmb_vme_ready = -1;
  //
  xgi::bind(this,&EmuPeripheralCrateMonitor::Default, "Default");
  xgi::bind(this,&EmuPeripheralCrateMonitor::MainPage, "MainPage");
  //
  //------------------------------------------------------
  // Bind monitoring methods
  //----------------------------
  xgi::bind(this,&EmuPeripheralCrateMonitor::CrateDMBCounters, "CrateDMBCounters");
  xgi::bind(this,&EmuPeripheralCrateMonitor::CrateTMBCounters, "CrateTMBCounters");
  xgi::bind(this,&EmuPeripheralCrateMonitor::ResetAllCounters, "ResetAllCounters");
  xgi::bind(this,&EmuPeripheralCrateMonitor::FullResetTMBC, "FullResetTMBC");
  xgi::bind(this,&EmuPeripheralCrateMonitor::XmlOutput, "XmlOutput");
  xgi::bind(this,&EmuPeripheralCrateMonitor::SwitchBoard, "SwitchBoard");
  xgi::bind(this,&EmuPeripheralCrateMonitor::CrateStatus, "CrateStatus");
  xgi::bind(this,&EmuPeripheralCrateMonitor::CrateSelection, "CrateSelection");
  xgi::bind(this,&EmuPeripheralCrateMonitor::TCounterSelection, "TCounterSelection");
  xgi::bind(this,&EmuPeripheralCrateMonitor::DCounterSelection, "DCounterSelection");
  xgi::bind(this,&EmuPeripheralCrateMonitor::OCounterSelection, "OCounterSelection");
  xgi::bind(this,&EmuPeripheralCrateMonitor::ChamberView, "ChamberView");
  xgi::bind(this,&EmuPeripheralCrateMonitor::CrateView, "CrateView");
  xgi::bind(this,&EmuPeripheralCrateMonitor::BeamView, "BeamView");

  xgi::bind(this,&EmuPeripheralCrateMonitor::MonitorStart      ,"MonitorStart");
  xgi::bind(this,&EmuPeripheralCrateMonitor::MonitorStop      ,"MonitorStop");

  xgi::bind(this,&EmuPeripheralCrateMonitor::DCSOutput, "DCSOutput");
  xgi::bind(this,&EmuPeripheralCrateMonitor::DCSDefault, "DCSDefault");
  xgi::bind(this,&EmuPeripheralCrateMonitor::DCSMain, "DCSMain");
  xgi::bind(this,&EmuPeripheralCrateMonitor::DCSChamber, "DCSChamber");
  xgi::bind(this,&EmuPeripheralCrateMonitor::DCSCrateLV, "DCSCrateLV");
  xgi::bind(this,&EmuPeripheralCrateMonitor::DCSCrateCUR, "DCSCrateCUR");
  xgi::bind(this,&EmuPeripheralCrateMonitor::DCSCrateTemp, "DCSCrateTemp");
  xgi::bind(this,&EmuPeripheralCrateMonitor::DCSStatSel, "DCSStatSel");
  xgi::bind(this,&EmuPeripheralCrateMonitor::DCSChamSel, "DCSChamSel");
  xgi::bind(this,&EmuPeripheralCrateMonitor::DCSCrateSel, "DCSCrateSel");
  //
  xgi::bind(this,&EmuPeripheralCrateMonitor::ForEmuPage1, "ForEmuPage1");
  //
  // SOAP for Monitor controll
    xoap::bind(this,&EmuPeripheralCrateMonitor::onMonitorStart      ,"MonitorStart",XDAQ_NS_URI);
    xoap::bind(this,&EmuPeripheralCrateMonitor::onMonitorStop      ,"MonitorStop",XDAQ_NS_URI);
    xoap::bind(this,&EmuPeripheralCrateMonitor::onFastLoop      ,"FastLoop", XDAQ_NS_URI);
    xoap::bind(this,&EmuPeripheralCrateMonitor::onSlowLoop      ,"SlowLoop", XDAQ_NS_URI);
    xoap::bind(this,&EmuPeripheralCrateMonitor::onExtraLoop      ,"ExtraLoop", XDAQ_NS_URI);
  //
  xgi::bind(this,&EmuPeripheralCrateMonitor::CheckCrates, "CheckCrates");
  //-------------------------------------------------------------
  // fsm_ is defined in EmuApplication
  //-------------------------------------------------------------
  fsm_.addState('H', "Halted",     this, &EmuPeripheralCrateMonitor::stateChanged);
  fsm_.addState('E', "Enabled",    this, &EmuPeripheralCrateMonitor::stateChanged);
  //
  fsm_.addStateTransition('E', 'E', "Enable",    this, &EmuPeripheralCrateMonitor::dummyAction);
  fsm_.addStateTransition('H', 'E', "Enable",    this, &EmuPeripheralCrateMonitor::dummyAction);
  fsm_.addStateTransition('E', 'H', "Halt",      this, &EmuPeripheralCrateMonitor::dummyAction);
  fsm_.addStateTransition('H', 'H', "Halt",      this, &EmuPeripheralCrateMonitor::dummyAction);
  //
  fsm_.setInitialState('H');
  fsm_.reset();    
  //
  // state_ is defined in EmuApplication
  state_ = fsm_.getStateName(fsm_.getCurrentState());
  //-------------------------------------------------------------
  //----------------------------
  // initialize variables
  //----------------------------
  xml_or_db = 0;  /* actual configuration source: 0: xml, 1: db */
  XML_or_DB_ = "xml";
  EMU_config_ID_ = "1000001";
  xmlFile_ = "config.xml" ;
  main_url_ = "";
  //
  RunNumber_= "-1";
  //
  this->getApplicationInfoSpace()->fireItemAvailable("XMLorDB", &XML_or_DB_);
  this->getApplicationInfoSpace()->fireItemAvailable("EmuConfigurationID", &EMU_config_ID_);
  this->getApplicationInfoSpace()->fireItemAvailable("runNumber", &runNumber_);
  this->getApplicationInfoSpace()->fireItemAvailable("xmlFileName", &xmlFile_);
  
  // for XMAS minotoring:

  Monitor_On_ = false;
  Monitor_Ready_ = false;
  fast_on = true;
  slow_on = true;
  extra_on = true;
  reload_vcc = false;
  fast_count = 0;
  slow_count = 0;
  extra_count = 0;

  global_config_states[0]="UnConfiged";
  global_config_states[1]="Configuring";
  global_config_states[2]="Configed";
  global_run_states[0]="Halted";
  global_run_states[1]="Enabled";
  current_config_state_=0;
  current_run_state_=0;
  total_crates_=0;
  this_crate_no_=0;
  controller_checked_=false;
  this_tcounter_=0;
  this_dcounter_=0;
  this_ocounter_=0;

  DCS_this_crate_no_=0;
  dcs_station=0;
  dcs_chamber=0;

  parsed=0;
}

void EmuPeripheralCrateMonitor::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
    throw (toolbox::fsm::exception::Exception) {
  changeState(fsm);
}

void EmuPeripheralCrateMonitor::dummyAction(toolbox::Event::Reference e)
    throw (toolbox::fsm::exception::Exception) {
  // currently do nothing
}
//

void EmuPeripheralCrateMonitor::ReadingOn()
{
     if(!Monitor_On_)
     {
         if(!Monitor_Ready_)
         {
             CreateEmuInfospace();
             Monitor_Ready_=true;
         }
         PCsendCommand("MonitorStart","emu::pc::EmuPeripheralCrateTimer");
         Monitor_On_=true;
         msgHandler("Monitor Reading On", 1);
     }
     fireEvent("Enable");
}

void EmuPeripheralCrateMonitor::ReadingOff()
{
     if(Monitor_On_)
     {
         PCsendCommand("MonitorStop","emu::pc::EmuPeripheralCrateTimer");
         Monitor_On_=false;
         msgHandler("Monitor Reading Off", 1);
     }
     fireEvent("Halt");
}

void EmuPeripheralCrateMonitor::MonitorStart(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
     ReadingOn();
     this->Default(in,out);
}

void EmuPeripheralCrateMonitor::MonitorStop(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
     ReadingOff();
     this->Default(in,out);
}

/////////////////////////////////////////////////////////////////////
// SOAP Callback  
/////////////////////////////////////////////////////////////////////

xoap::MessageReference EmuPeripheralCrateMonitor::onMonitorStart (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  std::cout << "SOAP MonitorStart" << std::endl;
  //
  ReadingOn();
  //
  return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateMonitor::onMonitorStop (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  std::cout << "SOAP MonitorStop" << std::endl;
  //
  ReadingOff();
  //
  return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateMonitor::onFastLoop (xoap::MessageReference message) 
  throw (xoap::exception::Exception) 
{
  // std::cout << "SOAP Fast Loop" << std::endl;
  fast_count++;
  if(fast_on) PublishEmuInfospace(1);
  return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateMonitor::onSlowLoop (xoap::MessageReference message) 
  throw (xoap::exception::Exception) 
{
  // std::cout << "SOAP Slow Loop" << std::endl;
  slow_count++;
  if(slow_on) PublishEmuInfospace(2);
  return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateMonitor::onExtraLoop (xoap::MessageReference message) 
  throw (xoap::exception::Exception) 
{
  // std::cout << "SOAP Extra Loop" << std::endl;
  extra_count++;
  if(extra_on) PublishEmuInfospace(3);
  return createReply(message);
}

void EmuPeripheralCrateMonitor::CreateEmuInfospace()
{
     if(!parsed) ParsingXML();
     if(total_crates_<=0) return;

        // Initialize TMB,DMB and Other Counter Names
        InitCounterNames();
        //Create infospaces for monitoring
        monitorables_.clear();
        vcc_reset.clear();
        crate_off.clear();
        for ( unsigned int i = 0; i < crateVector.size(); i++ )
        {
                toolbox::net::URN urn = this->createQualifiedInfoSpace("EMu_"+(crateVector[i]->GetLabel())+"_PCrate");
                std::cout << "Crate " << i << " " << urn.toString() << std::endl;
                monitorables_.push_back(urn.toString());
                xdata::InfoSpace * is = xdata::getInfoSpaceFactory()->get(urn.toString());

            // for VCC
                vcc_reset.push_back(0);
                crate_off.push_back(false);

            // for CCB, MPC, TTC etc.
                is->fireItemAvailable("CCBcounter",new xdata::Vector<xdata::UnsignedShort>());
                is->fireItemAvailable("CCBftime",new xdata::TimeVal);

            // for TMB fast counters
                is->fireItemAvailable("TMBcounter",new xdata::Vector<xdata::UnsignedInteger32>());
                is->fireItemAvailable("TMBftime",new xdata::TimeVal);
                is->fireItemAvailable("OTMBcounter",new xdata::Vector<xdata::UnsignedInteger32>());
                is->fireItemAvailable("OTMBftime",new xdata::TimeVal);

            // for DMB fast counters
                is->fireItemAvailable("DMBcounter",new xdata::Vector<xdata::UnsignedShort>());
                is->fireItemAvailable("DMBftime",new xdata::TimeVal);

            // for DCS temps, voltages
                is->fireItemAvailable("DCStemps",new xdata::Vector<xdata::Float>());
                is->fireItemAvailable("DCScrate",new xdata::UnsignedShort(0));
                is->fireItemAvailable("DCSitime",new xdata::UnsignedInteger32(0));
                is->fireItemAvailable("DCSstime",new xdata::TimeVal);
         }
     Monitor_Ready_=true;
}

void EmuPeripheralCrateMonitor::PublishEmuInfospace(int cycle)
{
   //   cycle = 1  fast loop (e.g. TMB/DMB counters)
   //           2  slow loop (e.g. temps/voltages)
   //           3  extra loop (e.g. CCB MPC TTC status)

      Crate * now_crate;
      xdata::InfoSpace * is;
      char buf[8000];
      xdata::UnsignedInteger32 *counter32;
      xdata::UnsignedShort *counter16;
      unsigned long *buf4;
      unsigned short *buf2;
      buf2=(unsigned short *)buf;
      buf4=(unsigned long *)buf;
      if(cycle<1 || cycle>3) return;
      if(total_crates_<=0) return;
      //update infospaces
      for ( unsigned int i = 0; i < crateVector.size(); i++ )
      {
          if(crate_off[i]) continue;
          is = xdata::getInfoSpaceFactory()->get(monitorables_[i]);
          now_crate=crateVector[i];

          // begin: reload VCC's FPGA (F9)
          if(cycle==3 && now_crate && reload_vcc && !(now_crate->IsAlive()))
          {
                int cr = now_crate->CheckController();
                if (cr==1)
                {
                   now_crate->vmeController()->reset();
                   vcc_reset[i] = vcc_reset[i] + 1;
                   // ::sleep(1);
                   // cr = (now_crate->vmeController()->SelfTest()) && (now_crate->vmeController()->exist(13));
                   // now_crate->SetLife( cr );
                   now_crate->SetLife( true );
                   continue;  // skip this round of reading if the VCC has been reloaded
                }
          }
          // end: reload

          if(now_crate && now_crate->IsAlive()) 
          {
             std::string cratename=now_crate->GetLabel();
             if(cycle==3)
             {  
                now_crate-> MonitorCCB(cycle, buf);
                if(buf2[0])  
                {   // buf2[0]==0 means no good data back
                   xdata::Vector<xdata::UnsignedShort> *ccbdata = dynamic_cast<xdata::Vector<xdata::UnsignedShort> *>(is->find("CCBcounter"));
                   if(ccbdata->size()==0) 
                      for(unsigned ii=0; ii<buf2[0]; ii++) ccbdata->push_back(0);
                   for(unsigned ii=0; ii<buf2[0]; ii++) (*ccbdata)[ii] = buf2[ii+1];
                }
             }
             else if( cycle==2)
             {
                now_crate-> MonitorDCS(cycle, buf);
                if(buf2[0])
                {
                   // std::cout << "Crate " << i << " DCS counters " << buf2[0] << std::endl;
                   xdata::Vector<xdata::Float> *dmbdata = dynamic_cast<xdata::Vector<xdata::Float> *>(is->find("DCStemps"));
                   if(dmbdata->size()==0)
                      for(unsigned ii=0; ii<buf2[0]; ii++) dmbdata->push_back(0.);
                   for(unsigned ii=0; ii<buf2[0]; ii++)
                   {   unsigned short rdv = buf2[ii+2];
                       if(rdv >= 0xFFF) rdv = 0;
                       if((ii%48)<40)
                       {
                          (*dmbdata)[ii] = 10.0/4096.0*rdv;

                          // for Voltage & Current reading error handling
                          int dmbslot=(ii/48)*2+3;
                          if(dmbslot>10) dmbslot += 2;
                          if(cratename.substr(4,1)=="4" && dmbslot>7)
                          { // nothing for empty slots
                          }
                          else
                          {
                             if(rdv==0 || rdv>=4065)
                             {  std::cout << "CFEB reading ERROR: " << cratename
                                       << " slot " << dmbslot << " read back " << std::hex << rdv << std::dec
                                       << " at " << getLocalDateTime() << std::endl; 
                             }
                          }

                       }
                       else if((ii%48)<46)
                       {  /* DMB Temps */
                          float Vout= rdv/1000.0;
                          if(Vout >0. && Vout<5.0)
                              (*dmbdata)[ii] =1/(0.001049406423+0.0002133635468*log(65000.0/Vout-13000.0)+0.7522287E-7*pow(log(65000.0/Vout-13000.0),3.0))-273.15;
                          else
                              (*dmbdata)[ii] = -500.0;
                       }
                       else
                       {  /* ALCT Temps */
                          rdv = rdv & 0x3FF;
                          float Vout= (float)(rdv)*1.225/1023.0;
                          if(Vout<1.225)
                              (*dmbdata)[ii] =100.0*(Vout-0.75)+25.0;
                          else
                              (*dmbdata)[ii] = -500.0;

                          // for ALCT temperature reading error handling
                          int tmbslot=(ii/48)*2+2;
                          if(tmbslot>10) tmbslot += 2;
                          if(cratename.substr(4,1)=="4" && tmbslot>6)
                          { // nothing for empty slots
                          }
                          else
                          {
                             if((ii%48)==46 && (rdv==0 || rdv>=1023))
                             {  std::cout << "ALCT Temperature ERROR: " << cratename
                                       << " slot " << tmbslot << " read back " << std::hex << rdv << std::dec
                                       << " at " << getLocalDateTime() << std::endl; 
                             }
                          }
                       }
                   }
                   counter16 = dynamic_cast<xdata::UnsignedShort *>(is->find("DCScrate"));
                   *counter16 = 1;
                   counter32 = dynamic_cast<xdata::UnsignedInteger32 *>(is->find("DCSitime"));
                   *counter32 = time(NULL);
                }
             }
             else if( cycle==1)
             {
                now_crate-> MonitorTMB(cycle, buf);
                if(buf2[0])
                {
                   // std::cout << "TMB counters " << (buf2[0]/2) << std::endl;
                   xdata::Vector<xdata::UnsignedInteger32> *tmbdata = dynamic_cast<xdata::Vector<xdata::UnsignedInteger32> *>(is->find("TMBcounter"));
                   xdata::Vector<xdata::UnsignedInteger32> *otmbdata = dynamic_cast<xdata::Vector<xdata::UnsignedInteger32> *>(is->find("OTMBcounter"));
                   if(otmbdata->size()==0)
                      for(unsigned ii=0; ii<(buf2[0]/2); ii++) otmbdata->push_back(0);
                   if(tmbdata->size()==0)
                   {   
                      for(unsigned ii=0; ii<(buf2[0]/2); ii++) tmbdata->push_back(0);
                   }
                   else
                   {
                      for(unsigned ii=0; ii<(buf2[0]/2); ii++) (*otmbdata)[ii] = (*tmbdata)[ii];
                   }
                   for(unsigned ii=0; ii<(buf2[0]/2); ii++) (*tmbdata)[ii] = buf4[ii+1];
                }
                now_crate-> MonitorDMB(cycle, buf);
                if(buf2[0])
                {
                   // std::cout << "DMB counters " << buf2[0] << std::endl;
                   xdata::Vector<xdata::UnsignedShort> *dmbdata = dynamic_cast<xdata::Vector<xdata::UnsignedShort> *>(is->find("DMBcounter"));
                   if(dmbdata->size()==0)
                      for(unsigned ii=0; ii<buf2[0]; ii++) dmbdata->push_back(0);
                   for(unsigned ii=0; ii<buf2[0]; ii++) (*dmbdata)[ii] = buf2[ii+1];
                }
             }
               // is->fireGroupChanged(names, this);
          }
          else
          {  // for non-communicating crates
             if( cycle==2 )
             {
                   counter16 = dynamic_cast<xdata::UnsignedShort *>(is->find("DCScrate"));
                   *counter16 = 0;
             }
          }

      }
}

void EmuPeripheralCrateMonitor::MainPage(xgi::Input * in, xgi::Output * out ) 
{
  if(!parsed) ParsingXML();
  main_url_ = getApplicationDescriptor()->getContextDescriptor()->getURL();
  //
  std::string LoggerName = getApplicationLogger().getName() ;
  std::cout << "Name of Logger is " <<  LoggerName <<std::endl;
  //
  LOG4CPLUS_INFO(getApplicationLogger(), "EmuPeripheralCrate ready");
  //
  if(endcap_side==1)
  {
     MyHeader(in,out,"EmuPeripheralCrateMonitor -- Plus Endcap");
  }
  else if(endcap_side==-1)
  {
     MyHeader(in,out,"EmuPeripheralCrateMonitor -- Minus Endcap");
  }
  else
  {
     MyHeader(in,out,"EmuPeripheralCrateMonitor -- Stand-alone");
  }
  *out << "Total Crates : ";
  *out << total_crates_ << cgicc::br() << std::endl ;
  unsigned int active_crates=0;
  for(unsigned i=0; i<crateVector.size(); i++)
     if(crateVector[i]->IsAlive()) active_crates++;
  if( active_crates <= total_crates_) 
     *out << cgicc::b(" Active Crates: ") << active_crates << cgicc::br() << std::endl ;
 
     *out << cgicc::table().set("border","0");
     //
     *out << cgicc::td();
     std::string CheckCrates = toolbox::toString("/%s/CheckCrates",getApplicationDescriptor()->getURN().c_str());
     *out << cgicc::form().set("method","GET").set("action",CheckCrates) << std::endl ;
     *out << cgicc::input().set("type","submit").set("value","Check Crate Controllers") << std::endl ;
     *out << cgicc::form() << std::endl ;
     *out << cgicc::td() << cgicc::td() << cgicc::td();
     
  if(Monitor_Ready_)
  {
     *out << cgicc::td();
     std::string DCSMain = toolbox::toString("/%s/DCSMain",getApplicationDescriptor()->getURN().c_str());
     *out << cgicc::form().set("method","GET").set("action",DCSMain).set("target","_blank") << std::endl ;
     *out << cgicc::input().set("type","submit").set("value","DCS Main Page") << std::endl ;
     *out << cgicc::form() << std::endl ;
     *out << cgicc::td();
  }
     *out << cgicc::table();
 

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
     *out << cgicc::span().set("style","color:red");
     *out << cgicc::b(cgicc::i("Monitor Status: Off")) << cgicc::span() << std::endl ;
     std::string TurnOnMonitor = toolbox::toString("/%s/MonitorStart",getApplicationDescriptor()->getURN().c_str());
     *out << cgicc::form().set("method","GET").set("action",TurnOnMonitor) << std::endl ;
     *out << cgicc::input().set("type","submit").set("value","Turn On Monitor") << std::endl ;
     *out << cgicc::form() << std::endl ;
  }
  //
  *out << cgicc::hr() <<std::endl;
  if(Monitor_Ready_)
  {

    *out << cgicc::h3("Endcap View")<< std::endl;
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial; background-color:cyan");
    *out << std::endl;
    *out << cgicc::legend((("Monitoring"))) ;
    //
    *out << cgicc::table().set("border","0");
    //
    *out << cgicc::td();
    std::string EndcapView = toolbox::toString("/%s/ChamberView",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",EndcapView).set("target","_blank") << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","All Chambers").set("name", "EndcapView") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //

    *out << cgicc::td();
    std::string EndcapView2 = toolbox::toString("/%s/CrateView",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",EndcapView2).set("target","_blank") << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","All Crates").set("name", "EndcapView") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //

    *out << cgicc::td();
    std::string beamView = toolbox::toString("/%s/BeamView",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",beamView).set("target","_blank") << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Beam Monitor").set("name", "BeamView") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::fieldset();
    //
    *out << std::endl;
    //


    *out << cgicc::hr() <<std::endl;

    *out << cgicc::h3("Counter View")<< std::endl;

    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial; background-color:cyan");
    *out << std::endl;
    *out << cgicc::legend((("Monitoring"))) ;
    //
  // Begin select TCounter
        // Config listbox
    *out << cgicc::form().set("action",
			"/" + getApplicationDescriptor()->getURN() + "/TCounterSelection").set("target","_blank") << std::endl;
	int n_keys = TCounterName.size();

	// *out << "Choose TMB Counter: " << std::endl;
	*out << cgicc::select().set("name", "selected") << std::endl;

	int selected_index = this_tcounter_;
        std::string CounterName;
	for (int i = 0; i < n_keys; ++i) {
                CounterName = TCounterName[i];
		if (i == selected_index) {
		  *out << cgicc::option()
					.set("value", CounterName)
					.set("selected", "");
		} else {
		  *out << cgicc::option()
					.set("value", CounterName);
		}
		*out << CounterName << cgicc::option() << std::endl;
	}

	*out << cgicc::select() << std::endl;

	*out << cgicc::input().set("type", "submit")
			.set("name", "command")
			.set("value", "TMB Counter View") << std::endl;
	*out << cgicc::form() << std::endl;
     
  //End select TCounter
    //
    //
  // Begin select DCounter
        // Config listbox
	*out << cgicc::form().set("action",
			"/" + getApplicationDescriptor()->getURN() + "/DCounterSelection").set("target","_blank") << std::endl;
	n_keys = DCounterName.size();

	// *out << "Choose DMB Counter: " << std::endl;
	*out << cgicc::select().set("name", "selected") << std::endl;

	selected_index = this_tcounter_;
	for (int i = 0; i < n_keys; ++i) {
                CounterName = DCounterName[i];
		if (i == selected_index) {
		  *out << cgicc::option()
					.set("value", CounterName)
					.set("selected", "");
		} else {
		  *out << cgicc::option()
					.set("value", CounterName);
		}
		*out << CounterName << cgicc::option() << std::endl;
	}

	*out << cgicc::select() << std::endl;

	*out << cgicc::input().set("type", "submit")
			.set("name", "command")
			.set("value", "DMB Counter View") << std::endl;
	*out << cgicc::form() << std::endl;
     
  //End select DCounter
    //
    //
    //
    *out << cgicc::fieldset();
    //
    *out << std::endl;
    //

    *out << cgicc::hr() <<std::endl;
    *out << cgicc::h3("Crate View")<< std::endl;
  
    *out << cgicc::span().set("style","color:blue");
    *out << cgicc::b(cgicc::i("Current Crate : ")) ;
    *out << ThisCrateID_ << cgicc::span() << std::endl ;
 
    *out << cgicc::br();
 

  // Begin select crate
        // Config listbox
    *out << cgicc::form().set("action",
			"/" + getApplicationDescriptor()->getURN() + "/CrateSelection") << std::endl;

	n_keys = crateVector.size();

	*out << "Choose Crate: " << std::endl;
	*out << cgicc::select().set("name", "runtype") << std::endl;

	selected_index = this_crate_no_;
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
    std::cout << "Main Page: "<< std::dec << total_crates_ << " Crates" << std::endl;
  //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial; background-color:cyan");
    *out << std::endl;
    *out << cgicc::legend((("Monitoring"))) ;
    //
    *out << cgicc::table().set("border","0");
    //
    *out << cgicc::td();
    //
    std::string ResetTMBC = toolbox::toString("/%s/ResetAllCounters",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",ResetTMBC) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Reset TMB Counters").set("name",thisCrate->GetLabel()) << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    //
    *out << cgicc::td();
    //
    std::string CrateTMBCounters = toolbox::toString("/%s/CrateTMBCounters",getApplicationDescriptor()->getURN().c_str());
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
    *out << cgicc::input().set("type","submit").set("value","Crate Status").set("name",thisCrate->GetLabel()) << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();

    //
    *out << cgicc::table();
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
}

// 
void EmuPeripheralCrateMonitor::MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) 
  throw (xgi::exception::Exception) {
  //
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  //
  //*out << cgicc::title(title) << std::endl;
  //*out << "<a href=\"/\"><img border=\"0\" src=\"/daq/xgi/images/XDAQLogo.gif\" title=\"XDAQ\" alt=\"\" style=\"width: 145px; height: 89px;\"></a>" << std::endl;
  //
  std::string myUrl = getApplicationDescriptor()->getContextDescriptor()->getURL();
  std::string myUrn = getApplicationDescriptor()->getURN().c_str();
  main_url_ = myUrl + "/" + myUrn + "/MainPage";
  xgi::Utils::getPageHeader(out,title,myUrl,myUrn,"");
  //
}
//
void EmuPeripheralCrateMonitor::Default(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<"MainPage"<<"\">" <<std::endl;
}
//
//
void EmuPeripheralCrateMonitor::DCSDefault(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<"DCSMain"<<"\">" <<std::endl;
}
//

void EmuPeripheralCrateMonitor::DCSMain(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception)
{
       int n_keys, selected_index;
       std::string CounterName, current_ch_name, endcap_name, dcs_chamber_name;
       std::string station_name[8]={"1/1","1/2","1/3","2/1","2/2","3/1","3/2","4/1"};
       int         station_size[8]={ 36,   36,   36,   18,   36,  18,    36,   18  };
       xdata::UnsignedShort xchamber;

  if(!parsed) ParsingXML();

  if(endcap_side==1)
  {
     MyHeader(in,out,"EmuPeripheralCrateMonitor -- DCS Plus Endcap");
     endcap_name="ME+";
  }
  else if(endcap_side==-1)
  {
     MyHeader(in,out,"EmuPeripheralCrateMonitor -- DCS Minus Endcap");
     endcap_name="ME-";
  }
  else
  {
     MyHeader(in,out,"EmuPeripheralCrateMonitor -- DCS Stand-alone");
     endcap_name="ME-";
  }

  if(Monitor_On_)
  {
     *out << cgicc::span().set("style","color:green");
     *out << cgicc::b(cgicc::i("Monitor Status: On")) << cgicc::span() << std::endl ;
  } else
  {
     *out << cgicc::b(cgicc::i("Monitor Status: Off")) << std::endl ;
  }
  //
  *out << cgicc::hr() <<std::endl;
  if(Monitor_Ready_)
  {
    xchamber=dcs_chamber+1;
    dcs_chamber_name=endcap_name+station_name[dcs_station]+"/"+xchamber.toString();
    *out << cgicc::h3("Chamber View")<< std::endl;

    *out << cgicc::span().set("style","color:blue");
    *out << cgicc::b(cgicc::i("Current Chamber : ")) ;
    *out << dcs_chamber_name << cgicc::span() << std::endl;
 
    *out << cgicc::table().set("border","0");
    //
    *out << cgicc::td();
    //
  // Begin select station
    *out << cgicc::form().set("action",
			"/" + getApplicationDescriptor()->getURN() + "/DCSStatSel") << std::endl;
	n_keys = 8;

	*out << cgicc::select().set("name", "selected") << std::endl;

	selected_index = dcs_station;
	for (int i = 0; i < n_keys; ++i) {
                CounterName = endcap_name+station_name[i];
		if (i == selected_index) {
		  *out << cgicc::option()
					.set("value", CounterName)
					.set("selected", "");
		} else {
		  *out << cgicc::option()
					.set("value", CounterName);
		}
		*out << CounterName << cgicc::option() << std::endl;
	}

	*out << cgicc::select() << std::endl;

	*out << cgicc::input().set("type", "submit")
			.set("name", "command")
			.set("value", "Select Station/Ring") << std::endl;
	*out << cgicc::form() << std::endl;
     
  //End select station
    //
    *out << cgicc::td();
    *out << cgicc::td();
    //
  // Begin select chamber
    *out << cgicc::form().set("action",
			"/" + getApplicationDescriptor()->getURN() + "/DCSChamSel") << std::endl;
	n_keys = station_size[dcs_station];

	// *out << "Choose TMB Counter: " << std::endl;
	*out << cgicc::select().set("name", "selected") << std::endl;

	selected_index = dcs_chamber;
	for (int i = 0; i < n_keys; ++i) {
                xchamber=i+1;
                CounterName = endcap_name+station_name[dcs_station]+"/"+xchamber.toString();
		if (i == selected_index) {
		  *out << cgicc::option()
					.set("value", CounterName)
					.set("selected", "");
		} else {
		  *out << cgicc::option()
					.set("value", CounterName);
		}
		*out << CounterName << cgicc::option() << std::endl;
	}

	*out << cgicc::select() << std::endl;

	*out << cgicc::input().set("type", "submit")
			.set("name", "command")
			.set("value", "Select Chamber") << std::endl;
	*out << cgicc::form() << std::endl;
     
  //End select chamber
    *out << cgicc::td();
    *out << cgicc::table() << cgicc::br() << std::endl;
   //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial; background-color:#407080");
    *out << std::endl;
//    *out << cgicc::legend((("Monitoring"))) ;

    std::string DCSchamber = toolbox::toString("/%s/DCSChamber",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",DCSchamber).set("target","_blank") << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Low Voltages and Temperatures").set("name", dcs_chamber_name) << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::fieldset();
    //
    *out << std::endl;
    //
    *out << cgicc::hr() <<std::endl;

    *out << cgicc::h3("Crate View")<< std::endl;
  
    *out << cgicc::span().set("style","color:blue");
    *out << cgicc::b(cgicc::i("Current Crate : ")) ;
    *out << crateVector[DCS_this_crate_no_]->GetLabel() << cgicc::span() << std::endl ;
 
    *out << cgicc::br();
 

  // Begin select crate
        // Config listbox
    *out << cgicc::form().set("action",
			"/" + getApplicationDescriptor()->getURN() + "/DCSCrateSel") << std::endl;

	n_keys = crateVector.size();

	*out << "Choose Crate: " << std::endl;
	*out << cgicc::select().set("name", "runtype") << std::endl;

	selected_index = DCS_this_crate_no_;
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
  //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial; background-color: #407080");
    *out << std::endl;
  //  *out << cgicc::legend((("Monitoring"))) ;
    //
    *out << cgicc::table().set("border","0");
    //
    *out << cgicc::td();
    //
    std::string DCSCrateLV = toolbox::toString("/%s/DCSCrateLV",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",DCSCrateLV).set("target","_blank") << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Low Voltages").set("name", crateVector[DCS_this_crate_no_]->GetLabel()) << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::td();
    //
    std::string DCSCrateCUR = toolbox::toString("/%s/DCSCrateCUR",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",DCSCrateCUR).set("target","_blank") << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Currents").set("name", crateVector[DCS_this_crate_no_]->GetLabel()) << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    //
    *out << cgicc::td();
    //
    std::string DCSCrateTemp = toolbox::toString("/%s/DCSCrateTemp",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",DCSCrateTemp).set("target","_blank") << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Temperatures").set("name",crateVector[DCS_this_crate_no_]->GetLabel()) << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::table();
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
}

  void EmuPeripheralCrateMonitor::DCSChamSel(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
     int         station_size[8]={ 36,   36,   36,   18,   36,  18,    36,   18  };

     cgicc::Cgicc cgi(in);

     std::string in_value = cgi.getElement("selected")->getValue(); 
     if(!in_value.empty())
     {
          dcs_chamber=atoi(in_value.substr(7,2).c_str())-1;
     }
     if(dcs_chamber<0 || dcs_chamber>=station_size[dcs_station]) dcs_chamber=0;
 
     this->DCSDefault(in,out);
  }

  void EmuPeripheralCrateMonitor::DCSStatSel(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
     std::string station_name[8]={"1/1","1/2","1/3","2/1","2/2","3/1","3/2","4/1"};
     int         station_size[8]={ 36,   36,   36,   18,   36,  18,    36,   18  };

     cgicc::Cgicc cgi(in);

     std::string in_value = cgi.getElement("selected")->getValue(); 
     // std::cout << "Select Counter " << in_value << std::endl;
     if(!in_value.empty())
     {
//        int k=in_value.find(" ",0);
//        std::string value = (k) ? in_value.substr(0,k):in_value;
        for(unsigned i=0; i< 8; i++)
        {
           if((in_value.substr(3,3))==station_name[i]) dcs_station=i;
        }
     }
     if(dcs_chamber>=station_size[dcs_station]) dcs_chamber=0;
     this->DCSDefault(in,out);
  }

  void EmuPeripheralCrateMonitor::DCSCrateSel(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
     cgicc::Cgicc cgi(in);

     std::string in_value = cgi.getElement("runtype")->getValue(); 
     std::cout << "DCS Select Crate " << in_value << std::endl;
     if(!in_value.empty())
     {
        int k=in_value.find(" ",0);
        std::string value = (k) ? in_value.substr(0,k):in_value;
        for(unsigned i=0; i< crateVector.size(); i++)
        {
           if(value==crateVector[i]->GetLabel())
           {  
               DCS_this_crate_no_=i;
               DCS_ThisCrateID_=value;
           }
        }
     }
     this->DCSDefault(in,out);
  }

void EmuPeripheralCrateMonitor::DCSChamber(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
  int TOTAL_DCS_COUNTERS=48;
  float temp_max[8]={40., 40., 40., 40., 40., 40., 40., 40.};
  float temp_min[8]={ 5.,  5.,  5.,  5.,  5.,  5.,  5.,  5.};
  float cv_max[3]={3.5, 5.4, 6.5};
  float cv_min[3]={3.1, 4.6, 5.5};
  float av_max[4]={3.5, 2.0, 6.0, 6.0};
  float av_min[4]={3.1, 1.6, 5.0, 5.0};
  float val;
  int cfebs=5;
  unsigned int readtime;
  int difftime;

  if(!Monitor_Ready_) return;
  //
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  //
  cgicc::CgiEnvironment cgiEnvi(in);
  //
  std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
  //
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"300; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<std::endl;
  //
  Page=cgiEnvi.getQueryString();
  std::string cham_name=Page.substr(0,Page.find("=", 0) );
  std::vector<DAQMB*> myVector;
  int mycrate=0, mychamb=0;
  for ( unsigned int i = 0; i < crateVector.size(); i++ )
  {
     myVector = crateVector[i]->daqmbs();
     for ( unsigned int j = 0; j < myVector.size(); j++ )
     {
       if(cham_name==crateVector[i]->GetChamber(myVector[j])->GetLabel())
       {  mycrate = i;
          mychamb = j;
       }
     }
  }
  // chamber 1/3 have only 4 CFEBs
  if(cham_name.substr(3,3)=="1/3") cfebs=4;
  *out << cgicc::b("Chamber: "+ cham_name) << std::endl;
  if(Monitor_On_)
  {
     *out << cgicc::span().set("style","color:green");
     *out << cgicc::b(cgicc::i("Monitor Status: On")) << cgicc::span() << std::endl ;
  } else 
  { 
     *out << cgicc::span().set("style","color:red");
     *out << cgicc::b(cgicc::i("Monitor Status: Off")) << cgicc::span() << std::endl ;
  }
  *out << cgicc::br() << cgicc::b("Crate: "+(crateVector[mycrate]->GetLabel())) << std::endl;

  xdata::InfoSpace * is = xdata::getInfoSpaceFactory()->get(monitorables_[mycrate]);
  xdata::Vector<xdata::Float> *dcsdata = dynamic_cast<xdata::Vector<xdata::Float> *>(is->find("DCStemps"));
  if(dcsdata==NULL || dcsdata->size()==0) return;
  xdata::UnsignedInteger32 *counter32 = dynamic_cast<xdata::UnsignedInteger32 *>(is->find("DCSitime"));
  if (counter32) 
  {
     readtime = (*counter32);
     difftime=time(NULL)-readtime;
     if(difftime>0) *out << " reading was " << difftime << " seconds old" << std::endl;
  }
  *out << std::setprecision(3);

  *out << cgicc::br() << cgicc::b("<center> Low Voltages and Currents </center>") << std::endl;

  // CFEBs
  *out << cgicc::table().set("border","1").set("align","center");
  //
  *out <<cgicc::td() << cgicc::td();
  *out <<cgicc::td() << "3.3V" << cgicc::td();
  *out <<cgicc::td() << "I" << cgicc::td();
  *out <<cgicc::td() << "5V" << cgicc::td();
  *out <<cgicc::td() << "I" << cgicc::td();
  *out <<cgicc::td() << "6V" << cgicc::td();
  *out <<cgicc::td() << "I" << cgicc::td();
  *out << cgicc::tr() << std::endl;

  for(int feb=0; feb<cfebs; feb++)
  {
     *out <<cgicc::td() << "CFEB " << feb+1 << cgicc::td();
     for(int cnt=0; cnt<3; cnt++)
     {
        val=(*dcsdata)[mychamb*TOTAL_DCS_COUNTERS+19+3*feb+cnt];
        *out << cgicc::td();
        if(val<0.)    
           *out << cgicc::span().set("style","color:magenta") << val << cgicc::span();
        else if(val > cv_max[cnt] || val < cv_min[cnt])
           *out << cgicc::span().set("style","color:red") << val << cgicc::span();
        else 
           *out << val;  
        *out << cgicc::td();
        val=(*dcsdata)[mychamb*TOTAL_DCS_COUNTERS+3*feb+cnt];
        *out <<cgicc::td() << val << cgicc::td();
     }
     *out << cgicc::tr() << std::endl;
  }
  *out << cgicc::table() << cgicc::br() << std::endl;

  // ALCT
  *out << cgicc::table().set("border","1").set("align","center");
  //
  *out <<cgicc::td() << cgicc::td();
  *out <<cgicc::td() << "3.3V" << cgicc::td();
  *out <<cgicc::td() << "I" << cgicc::td();
  *out <<cgicc::td() << "1.8V" << cgicc::td();
  *out <<cgicc::td() << "I" << cgicc::td();
  *out <<cgicc::td() << "5.5V B" << cgicc::td();
  *out <<cgicc::td() << "I" << cgicc::td();
  *out <<cgicc::td() << "5.5V A" << cgicc::td();
  *out <<cgicc::td() << "I" << cgicc::td();
  *out << cgicc::tr() << std::endl;

     *out <<cgicc::td() << "ALCT" << cgicc::td();
     for(int cnt=0; cnt<4; cnt++)
     {
        val=(*dcsdata)[mychamb*TOTAL_DCS_COUNTERS+19+15+cnt];
        *out <<cgicc::td();
        if(val<0.)    
           *out << cgicc::span().set("style","color:magenta") << val << cgicc::span();
        else if(val > av_max[cnt] || val < av_min[cnt])
           *out << cgicc::span().set("style","color:red") << val << cgicc::span();
        else 
           *out << val;  
        *out << cgicc::td();
        val=(*dcsdata)[mychamb*TOTAL_DCS_COUNTERS+15+cnt];
        *out <<cgicc::td() << val << cgicc::td();
     }
     *out << cgicc::tr() << std::endl;

  *out << cgicc::table() << cgicc::br() << cgicc::hr()<< std::endl;

  // TEMPs
  *out << cgicc::br() << cgicc::b("<center> Temperatures (C)</center>") << std::endl;
  *out << cgicc::table().set("border","1").set("align","center");
  //
  *out <<cgicc::td() << cgicc::td();
  *out <<cgicc::td() << "DMB" << cgicc::td();
  *out <<cgicc::td() << "CFEB 1" << cgicc::td();
  *out <<cgicc::td() << "CFEB 2" << cgicc::td();
  *out <<cgicc::td() << "CFEB 3" << cgicc::td();
  *out <<cgicc::td() << "CFEB 4" << cgicc::td();
  if(cfebs==5) *out <<cgicc::td() << "CFEB 5" << cgicc::td();
  *out <<cgicc::td() << "ALCT" << cgicc::td();
  *out << cgicc::tr() << std::endl;

     *out <<cgicc::td() << "Temperature (C)" << cgicc::td();
     for(int cnt=0; cnt<7; cnt++)
     {
        if(cfebs<5 && cnt==5) continue;
        val=(*dcsdata)[mychamb*TOTAL_DCS_COUNTERS+40+cnt];
        *out <<cgicc::td();
        if(val<0.)    
           *out << cgicc::span().set("style","color:magenta") << val << cgicc::span();
        else if(val > temp_max[cnt] || val < temp_min[cnt])
           *out << cgicc::span().set("style","color:red") << val << cgicc::span();
        else 
           *out << val;  
        *out <<cgicc::td();
     }

     *out << cgicc::tr() << std::endl;

  *out << cgicc::table() << std::endl;

}

void EmuPeripheralCrateMonitor::DCSCrateLV(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
  int TOTAL_DCS_COUNTERS=48, Total_count=19;
  float lv_max[19]={3.5, 5.4, 6.5, 3.5, 5.4, 6.5, 3.5, 5.4, 6.5, 3.5, 5.4, 6.5, 3.5, 5.4, 6.5, 3.5, 2.0, 6.0, 6.0};
  float lv_min[19]={3.1, 4.6, 5.5, 3.1, 4.6, 5.5, 3.1, 4.6, 5.5, 3.1, 4.6, 5.5, 3.1, 4.6, 5.5, 3.1, 1.6, 5.0, 5.0};
  float val;

  if(!Monitor_Ready_) return;
  //
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  //
  cgicc::CgiEnvironment cgiEnvi(in);
  //
  std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
  //
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"300; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<std::endl;
  //
  Page=cgiEnvi.getQueryString();
  std::string crate_name=Page.substr(0,Page.find("=", 0) );
  std::vector<DAQMB*> myVector;
  int mycrate=0;
  for ( unsigned int i = 0; i < crateVector.size(); i++ )
  {
     if(crate_name==crateVector[i]->GetLabel())
     {  myVector = crateVector[i]->daqmbs();
        mycrate = i;
     }
  }
  *out << cgicc::b("Crate: "+crate_name) << std::endl;
  if(Monitor_On_)
  {
     *out << cgicc::span().set("style","color:green");
     *out << cgicc::b(cgicc::i("Monitor Status: On")) << cgicc::span() << std::endl ;
  } else 
  { 
     *out << cgicc::span().set("style","color:red");
     *out << cgicc::b(cgicc::i("Monitor Status: Off")) << cgicc::span() << std::endl ;
  }

  *out << cgicc::br() << cgicc::b("<center> Low Voltages (Volts) </center>") << std::endl;

  xdata::InfoSpace * is = xdata::getInfoSpaceFactory()->get(monitorables_[mycrate]);

  xdata::Vector<xdata::Float> *dcsdata = dynamic_cast<xdata::Vector<xdata::Float> *>(is->find("DCStemps"));
  if(dcsdata==NULL || dcsdata->size()==0) return;

  *out << cgicc::table().set("border","1").set("align","center");
  //
  *out <<cgicc::td();
  *out <<cgicc::td();
  //
  for(unsigned int dmb=0; dmb<myVector.size(); dmb++) {
    *out <<cgicc::td();
    *out << crateVector[mycrate]->GetChamber(myVector[dmb])->GetLabel();
    *out <<cgicc::td();
  }
  //
  *out <<cgicc::tr();
  //
  for (int count=0; count<Total_count; count++) {
    //
    for(unsigned int dmb=0; dmb<myVector.size(); dmb++) {
      //
      *out <<cgicc::td();
      //
      if(dmb==0) {
        *out << LVCounterName[count] ;
	*out <<cgicc::td() << cgicc::td();
      }
      *out << std::setprecision(3);
      val=(*dcsdata)[dmb*TOTAL_DCS_COUNTERS+19+count];
      if(val<0.)    
         *out << cgicc::span().set("style","color:magenta") << val << cgicc::span();
      else if(val > lv_max[count] || val < lv_min[count])
         *out << cgicc::span().set("style","color:red") << val << cgicc::span();
      else 
         *out << val;  
      *out <<cgicc::td();
    }
    *out <<cgicc::tr();
  }
  //
  *out << cgicc::table();
  //
}

void EmuPeripheralCrateMonitor::DCSCrateCUR(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
  int TOTAL_DCS_COUNTERS=48, Total_count=19;
//  float lv_max[19]={3.5, 5.4, 6.5, 3.5, 5.4, 6.5, 3.5, 5.4, 6.5, 3.5, 5.4, 6.5, 3.5, 5.4, 6.5, 3.5, 2.0, 6.0, 6.0};
//  float lv_min[19]={3.1, 4.6, 5.5, 3.1, 4.6, 5.5, 3.1, 4.6, 5.5, 3.1, 4.6, 5.5, 3.1, 4.6, 5.5, 3.1, 1.6, 5.0, 5.0};
  float val;

  if(!Monitor_Ready_) return;
  //
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  //
  cgicc::CgiEnvironment cgiEnvi(in);
  //
  std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
  //
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"300; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<std::endl;
  //
  Page=cgiEnvi.getQueryString();
  std::string crate_name=Page.substr(0,Page.find("=", 0) );
  std::vector<DAQMB*> myVector;
  int mycrate=0;
  for ( unsigned int i = 0; i < crateVector.size(); i++ )
  {
     if(crate_name==crateVector[i]->GetLabel())
     {  myVector = crateVector[i]->daqmbs();
        mycrate = i;
     }
  }
  *out << cgicc::b("Crate: "+crate_name) << std::endl;
  if(Monitor_On_)
  {
     *out << cgicc::span().set("style","color:green");
     *out << cgicc::b(cgicc::i("Monitor Status: On")) << cgicc::span() << std::endl ;
  } else 
  { 
     *out << cgicc::span().set("style","color:red");
     *out << cgicc::b(cgicc::i("Monitor Status: Off")) << cgicc::span() << std::endl ;
  }
  *out << cgicc::br() << cgicc::b("<center> Currents (Amps) </center>") << std::endl;

  xdata::InfoSpace * is = xdata::getInfoSpaceFactory()->get(monitorables_[mycrate]);

  xdata::Vector<xdata::Float> *dcsdata = dynamic_cast<xdata::Vector<xdata::Float> *>(is->find("DCStemps"));
  if(dcsdata==NULL || dcsdata->size()==0) return;

  *out << cgicc::table().set("border","1").set("align","center");
  //
  *out <<cgicc::td();
  *out <<cgicc::td();
  //
  for(unsigned int dmb=0; dmb<myVector.size(); dmb++) {
    *out <<cgicc::td();
    *out << crateVector[mycrate]->GetChamber(myVector[dmb])->GetLabel();
    *out <<cgicc::td();
  }
  //
  *out <<cgicc::tr();
  //
  for (int count=0; count<Total_count; count++) {
    //
    for(unsigned int dmb=0; dmb<myVector.size(); dmb++) {
      //
      *out <<cgicc::td();
      //
      if(dmb==0) {
        *out << LVCounterName[count] ;
	*out <<cgicc::td() << cgicc::td();
      }
      *out << std::setprecision(3);
      val=(*dcsdata)[dmb*TOTAL_DCS_COUNTERS+count];
      if(val<0.)    
         *out << cgicc::span().set("style","color:magenta") << val << cgicc::span();
//
// warning disabled. need valid ranges.  
//
//      else if(val > lv_max[count] || val < lv_min[count])
//         *out << cgicc::span().set("style","color:red") << val << cgicc::span();
      else 
         *out << val;  
      *out <<cgicc::td();
    }
    *out <<cgicc::tr();
  }
  //
  *out << cgicc::table();
  //
}

void EmuPeripheralCrateMonitor::DCSCrateTemp(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
  int TOTAL_DCS_COUNTERS=48, Total_Temps=7;
  float temp_max[8]={40., 40., 40., 40., 40., 40., 40., 40.};
  float temp_min[8]={ 5.,  5.,  5.,  5.,  5.,  5.,  5.,  5.};
  float val;

  if(!Monitor_Ready_) return;
  //
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  //
  cgicc::CgiEnvironment cgiEnvi(in);
  //
  std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
  //
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"300; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<std::endl;
  //
  Page=cgiEnvi.getQueryString();
  std::string crate_name=Page.substr(0,Page.find("=", 0) );
  std::vector<DAQMB*> myVector;
  int mycrate=0;
  for ( unsigned int i = 0; i < crateVector.size(); i++ )
  {
     if(crate_name==crateVector[i]->GetLabel())
     {  myVector = crateVector[i]->daqmbs();
        mycrate = i;
     }
  }
  *out << cgicc::b("Crate: "+crate_name) << std::endl;
  if(Monitor_On_)
  {
     *out << cgicc::span().set("style","color:green");
     *out << cgicc::b(cgicc::i("Monitor Status: On")) << cgicc::span() << std::endl ;
  } else 
  { 
     *out << cgicc::span().set("style","color:red");
     *out << cgicc::b(cgicc::i("Monitor Status: Off")) << cgicc::span() << std::endl ;
  }

  *out << cgicc::br() << cgicc::b("<center> Temperatures (C) </center>") << std::endl;

  xdata::InfoSpace * is = xdata::getInfoSpaceFactory()->get(monitorables_[mycrate]);

  xdata::Vector<xdata::Float> *dcsdata = dynamic_cast<xdata::Vector<xdata::Float> *>(is->find("DCStemps"));
  if(dcsdata==NULL || dcsdata->size()==0) return;

  *out << cgicc::table().set("border","1").set("align","center");
  //
  *out <<cgicc::td();
  *out <<cgicc::td();
  //
  for(unsigned int dmb=0; dmb<myVector.size(); dmb++) {
    *out <<cgicc::td();
    *out << crateVector[mycrate]->GetChamber(myVector[dmb])->GetLabel();
    *out <<cgicc::td();
  }
  //
  *out <<cgicc::tr();
  //
  for (int count=0; count<Total_Temps; count++) {
    //
    for(unsigned int dmb=0; dmb<myVector.size(); dmb++) {
      //
      *out <<cgicc::td();
      //
      if(dmb==0) {
        *out << TECounterName[count] ;
	*out <<cgicc::td() << cgicc::td();
      }
      *out << std::setprecision(3);
      val=(*dcsdata)[dmb*TOTAL_DCS_COUNTERS+40+count];
      if(val<0.)    
         *out << cgicc::span().set("style","color:magenta") << val << cgicc::span();
      else if(val > temp_max[count] || val < temp_min[count])
         *out << cgicc::span().set("style","color:red") << val << cgicc::span();
      else 
         *out << val;  
      *out <<cgicc::td();
    }
    *out <<cgicc::tr();
  }
  //
  *out << cgicc::table();
  //
    
}

  void EmuPeripheralCrateMonitor::CrateSelection(xgi::Input * in, xgi::Output * out ) 
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

  bool EmuPeripheralCrateMonitor::ParsingXML(){
    //
    LOG4CPLUS_INFO(getApplicationLogger(),"Parsing Configuration XML");
    //
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
    MyController = new EmuController();

    MyController->SetConfFile(xmlFile_.toString().c_str());
    MyController->init();
    // MyController->NotInDCS();
    //
    emuEndcap_ = MyController->GetEmuEndcap();
    if(!emuEndcap_) return false;
    crateVector = emuEndcap_->crates();
    //
    total_crates_=crateVector.size();
    if(total_crates_<=0) return false;
    this_crate_no_=0;

    SetCurrentCrate(this_crate_no_);
    //
    std::string endcap_name=crateVector[0]->GetLabel();
    endcap_side = 0;
    if (endcap_name.substr(3,1)=="p") endcap_side = 1;
    if (endcap_name.substr(3,1)=="m") endcap_side = -1;
    //
    std::cout << "Parser Done" << std::endl ;
    //
    parsed=1;
    return true;
  }

  void EmuPeripheralCrateMonitor::SetCurrentCrate(int cr)
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
    current_crate_ = cr;
  }

void EmuPeripheralCrateMonitor::ChamberView(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
     unsigned int tmbslots[9]={2,4,6,8,10,14,16,18,20};

     cgicc::Cgicc cgi(in);

    // std::cout << "Select Over View " << std::endl;
// now produce the counter view page
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    //
    cgicc::CgiEnvironment cgiEnvi(in);
    std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
    *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"5; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" << std::endl;
    *out << cgicc::b("All Chambers") << std::endl;

  if(Monitor_On_)
  {
     *out << cgicc::span().set("style","color:green");
     *out << cgicc::b(cgicc::i("Monitor Status: On")) << cgicc::span() << std::endl ;
  } else 
  { 
     *out << cgicc::span().set("style","color:red");
     *out << cgicc::b(cgicc::i("Monitor Status: Off")) << cgicc::span() << std::endl ;
  }

    if(!Monitor_Ready_) return;

  *out << cgicc::br() << cgicc::span().set("style","color:blue");
  *out << cgicc::b("A: ") << cgicc::i(" ALCT pattern; ") << cgicc::b("C: ") << cgicc::i(" CLCT pretrig; ");
  *out << cgicc::b("T: ") << cgicc::i(" TMB triggers; ") << cgicc::b("L: ") << cgicc::i(" L1A * TMB");
  *out << cgicc::span() << std::endl ;
  
  *out << cgicc::table().set("border","1");
  //
  *out <<cgicc::td();
  *out << "crate";

  *out <<cgicc::td();
  //
  for(unsigned int tmb=0; tmb<9; tmb++) {
    if(tmb==0)
    {
       *out <<cgicc::td();
       *out << "VCC";
       *out <<cgicc::td();
    }
    *out <<cgicc::td();
    *out << "Slot " << tmbslots[tmb] << "/" << tmbslots[tmb]+1;  
    *out <<cgicc::td();
    //
  }
  //
  *out <<cgicc::tr();
  //
  std::vector<TMB*> myVector;
  for (unsigned int idx=0; idx<crateVector.size(); idx++) {
    myVector = crateVector[idx]->tmbs();
    //
    for(unsigned int tmb=0; tmb<myVector.size(); tmb++) {
      //
      *out <<cgicc::td();
      //
      if(tmb==0) {
        *out << crateVector[idx]->GetLabel() ;
	*out <<cgicc::td();
	*out <<cgicc::td();

        if(crateVector[idx]->IsAlive()) 
           *out << cgicc::span().set("style","color:green") << "On " << cgicc::span();
        else
           *out << cgicc::span().set("style","color:red") << "Off" << cgicc::span();
         *out <<cgicc::td();
        *out <<cgicc::td();
      }
      // chamber name
      *out << crateVector[idx]->GetChamber(myVector[tmb])->GetLabel() << cgicc::br();
      // ALCT: LCT sent to TMB
      int dc=myVector[tmb]->GetCounter(myVector[tmb]->GetALCTSentToTMBCounterIndex());
      if (dc == 0x3fffffff || dc < 0 ) dc = -1;
      if(dc<0) 
      {  *out << cgicc::span().set("style","color:magenta");
         *out << "A: " << dc <<"; ";
         *out << cgicc::span();
      }
      else if(dc==0)
      {  *out << cgicc::span().set("style","color:red");
         *out << "A: " << dc <<"; ";
         *out << cgicc::span();
      }
      else
         *out << "A: " << dc <<"; ";
      // CLCT pretrigger 
      dc=myVector[tmb]->GetCounter(myVector[tmb]->GetCLCTPretriggerCounterIndex());
      if (dc == 0x3fffffff || dc < 0 ) dc = -1;
      if(dc<0) 
      {  *out << cgicc::span().set("style","color:magenta");
         *out << "C: " << dc;
         *out << cgicc::span();
      }
      else if(dc==0)
      {  *out << cgicc::span().set("style","color:red");
         *out << "C: " << dc;
         *out << cgicc::span();
      }
      else
         *out << "C: " << dc;
      *out << cgicc::br();
      // trig allowed, xmit to MPC
      dc=myVector[tmb]->GetCounter(myVector[tmb]->GetLCTSentToMPCCounterIndex());
      if (dc == 0x3fffffff || dc < 0 ) dc = -1;
      if(dc<0) 
      {  *out << cgicc::span().set("style","color:magenta");
         *out << "T: " << dc <<"; ";
         *out << cgicc::span();
      }
      else if(dc==0)
      {  *out << cgicc::span().set("style","color:red");
         *out << "T: " << dc <<"; ";
         *out << cgicc::span();
      }
      else
         *out << "T: " << dc << "; ";
      // L1A: TMB triggered, TMB in L1A window
      dc=myVector[tmb]->GetCounter(myVector[tmb]->GetL1AInTMBWindowCounterIndex());
      if (dc == 0x3fffffff || dc < 0 ) dc = -1;
      if(dc<0) 
      {  *out << cgicc::span().set("style","color:magenta");
         *out << "L: " << dc;
         *out << cgicc::span();
      }
      else if(dc==0)
      {  *out << cgicc::span().set("style","color:red");
         *out << "L: " << dc;
         *out << cgicc::span();
      }
      else
         *out << "L: " << dc;
      *out <<cgicc::td();
    }
    *out <<cgicc::tr();
  }
  //
  *out << cgicc::table() << std::endl;
  //
  }

void EmuPeripheralCrateMonitor::CrateView(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
//     unsigned int tmbslots[9]={2,4,6,8,10,14,16,18,20};
     unsigned TOTAL_COUNTS=10;

     cgicc::Cgicc cgi(in);

     // std::cout << "Select Crate View " << std::endl;
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    //
    cgicc::CgiEnvironment cgiEnvi(in);
    std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
    *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"120; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<std::endl;
    *out << cgicc::b("All Crates") << std::endl;

  if(Monitor_On_)
  {
     *out << cgicc::span().set("style","color:green");
     *out << cgicc::b(cgicc::i("Monitor Status: On")) << cgicc::span() << std::endl ;
  } else 
  { 
     *out << cgicc::span().set("style","color:red");
     *out << cgicc::b(cgicc::i("Monitor Status: Off")) << cgicc::span() << std::endl ;
  }

    if(!Monitor_Ready_) return;

  *out << cgicc::table().set("border","1");
  //
  *out <<cgicc::td();
  *out << "crate";
  *out <<cgicc::td();
  //
  for(unsigned int count=0; count<TOTAL_COUNTS; count++) {
    if(count==0)
    {
       *out <<cgicc::td();
       *out << "VCC";
       *out <<cgicc::td();
    }
    *out <<cgicc::td();
    *out << OCounterName[count];
    *out <<cgicc::td();
    //
  }
  //
  *out <<cgicc::tr();
  //
  for (unsigned int idx=0; idx<crateVector.size(); idx++) {

    // retrieve data from inforspace
    xdata::InfoSpace * is = xdata::getInfoSpaceFactory()->get(monitorables_[idx]);
    xdata::Vector<xdata::UnsignedShort> *ccbdata = dynamic_cast<xdata::Vector<xdata::UnsignedShort> *>(is->find("CCBcounter"));
    unsigned short csra1=0,csra2=0,csra3=0,csrm0=0,brstr=0,dtstr=0;
    if(!ccbdata) continue;
    if(ccbdata->size()>12)
    {
      csra1=(*ccbdata)[0];
      csra2=(*ccbdata)[1];
      csra3=(*ccbdata)[2];
      brstr=(*ccbdata)[9];
      dtstr=(*ccbdata)[10];
      csrm0=(*ccbdata)[11];
    }
    //
    for(unsigned int count=0; count<TOTAL_COUNTS; count++) {
      //
      *out <<cgicc::td();
      //
      if(count==0) {
        *out << crateVector[idx]->GetLabel() ;
	*out <<cgicc::td();
	*out <<cgicc::td();

        if(crateVector[idx]->IsAlive()) 
           *out << cgicc::span().set("style","color:green") << "On " << cgicc::span();
        else 
           *out << cgicc::span().set("style","color:red") << "Off" << cgicc::span();
        *out <<cgicc::td();
        *out <<cgicc::td();
      }
      switch(count)
      {
         case 0:
           if (csra1 & 0x1)
     	     *out << cgicc::span().set("style","color:green") << "DLOG" << cgicc::span();
           else 
             *out << cgicc::span().set("style","color:red") << "FPGA" << cgicc::span();
           break;
         case 1:
   	   if (csra3 & 0x2000)
     	     *out << cgicc::span().set("style","color:green") << "Ready" << cgicc::span();
           else 
             *out << cgicc::span().set("style","color:red") << "No" << cgicc::span();
           break;
         case 2:
   	   if ((csra3 & 0x2000)!=0 && (csra3 & 0x4000)==0 )
     	     *out << cgicc::span().set("style","color:green") << "Locked" << cgicc::span();
           else 
             *out << cgicc::span().set("style","color:red") << "No" << cgicc::span();
           break;
         case 3:
   	   if ((csrm0 & 0x8201)==0x0200)
     	     *out << cgicc::span().set("style","color:green") << "OK" << cgicc::span();
           else 
             *out << cgicc::span().set("style","color:red") << "No" << cgicc::span();
           break;
         case 4:
   	   *out << std::hex << csra1 << std::dec;
           break;
         case 5:
   	   *out << std::hex << csra2 << std::dec;
           break;
         case 6:
   	   *out << std::hex << csra3 << std::dec;
           break;
         case 7:
   	   *out << brstr;
           break;
         case 8:
   	   *out << dtstr;
           break;
         case 9:
           *out << vcc_reset[idx];
           break;
         default:
           *out << "Unknown";
      }
      *out <<cgicc::td();
    }
    *out <<cgicc::tr();
  }
  //
  *out << cgicc::table() << std::endl;
  //
  }

void EmuPeripheralCrateMonitor::TCounterSelection(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
     unsigned int tmbslots[9]={2,4,6,8,10,14,16,18,20};

    if(!Monitor_Ready_) return;
     cgicc::Cgicc cgi(in);

     std::string in_value = cgi.getElement("selected")->getValue(); 
     // std::cout << "Select Counter " << in_value << std::endl;
     if(!in_value.empty())
     {
//        int k=in_value.find(" ",0);
//        std::string value = (k) ? in_value.substr(0,k):in_value;
        for(unsigned i=0; i< TCounterName.size(); i++)
        {
           if(in_value==TCounterName[i]) this_tcounter_=i;
        }
     }
// now produce the counter view page
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    //
    cgicc::CgiEnvironment cgiEnvi(in);
    std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
    *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"5; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<std::endl;

  if(Monitor_On_)
  {
     *out << cgicc::span().set("style","color:green");
     *out << cgicc::b(cgicc::i("Monitor Status: On")) << cgicc::span() << std::endl ;
  } else 
  { 
     *out << cgicc::span().set("style","color:red");
     *out << cgicc::b(cgicc::i("Monitor Status: Off")) << cgicc::span() << std::endl ;
  }

    *out << cgicc::b("<center>"+TCounterName[this_tcounter_]+"</center>" ) << std::endl;

  *out << cgicc::table().set("border","1");
  //
  *out <<cgicc::td();
  *out << "crate";
  *out <<cgicc::td();
  //
  for(unsigned int tmb=0; tmb<9; tmb++) {
    if(tmb==0)
    {
       *out <<cgicc::td();
       *out << "VCC";
       *out <<cgicc::td();
    }
    *out <<cgicc::td();
    *out << "Slot " << tmbslots[tmb];
    *out <<cgicc::td();
    //
  }
  //
  *out <<cgicc::tr();
  //
  std::vector<TMB*> myVector;
  for (unsigned int idx=0; idx<crateVector.size(); idx++) {
    myVector = crateVector[idx]->tmbs();
    //
    for(unsigned int tmb=0; tmb<myVector.size(); tmb++) {
      //
      *out <<cgicc::td();
      //
      if(tmb==0) {
        *out << crateVector[idx]->GetLabel() ;
	*out <<cgicc::td();
	*out <<cgicc::td();

        if(crateVector[idx]->IsAlive())
        { 
           *out << "On ";
        }
        else
        {
           *out << cgicc::span().set("style","color:red");
           *out << "Off";
           *out << cgicc::span();
        }
        *out <<cgicc::td();
        *out <<cgicc::td();
      }
      int value = myVector[tmb]->GetCounter(this_tcounter_);
      /* 0xBAADBAAD from VCC for a failed VME access */
      if (  value == 0x3fffffff || value < 0 )
      {
         *out << cgicc::span().set("style","color:magenta");
         *out << "-1";
         *out << cgicc::span();
      }
      else if ( (IsErrCounter[this_tcounter_]>0 && value>0) || (IsErrCounter[this_tcounter_]==0 && value==0) )
      {
         *out << cgicc::span().set("style","color:red");
         *out << value;
         *out << cgicc::span();
      }
      else 
   	 *out << value;
      *out <<cgicc::td();
    }
    *out <<cgicc::tr();
  }
  //
  *out << cgicc::table() << std::endl;
  //
  }

  void EmuPeripheralCrateMonitor::DCounterSelection(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
     unsigned int dmbslots[9]={3,5,7,9,11,15,17,19,21};

    if(!Monitor_Ready_) return;
     cgicc::Cgicc cgi(in);

     std::string in_value = cgi.getElement("selected")->getValue(); 
     // std::cout << "Select Counter " << in_value << std::endl;
     if(!in_value.empty())
     {
//        int k=in_value.find(" ",0);
//        std::string value = (k) ? in_value.substr(0,k):in_value;
        for(unsigned i=0; i< DCounterName.size(); i++)
        {
           if(in_value==DCounterName[i]) this_dcounter_=i;
        }
     }
// now produce the counter view page
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    //
    cgicc::CgiEnvironment cgiEnvi(in);
    std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
    *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"5; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<std::endl;

  if(Monitor_On_)
  {
     *out << cgicc::span().set("style","color:green");
     *out << cgicc::b(cgicc::i("Monitor Status: On")) << cgicc::span() << std::endl ;
  } else 
  { 
     *out << cgicc::span().set("style","color:red");
     *out << cgicc::b(cgicc::i("Monitor Status: Off")) << cgicc::span() << std::endl ;
  }

    *out << cgicc::b("<center>"+DCounterName[this_dcounter_]+"</center>" ) << std::endl;

  *out << cgicc::table().set("border","1");
  //
  *out <<cgicc::td();
  *out << "crate";
  *out <<cgicc::td();
  //
  for(unsigned int dmb=0; dmb<9; dmb++) {
    if(dmb==0)
    {
       *out <<cgicc::td();
       *out << "VCC";
       *out <<cgicc::td();
    }
    *out <<cgicc::td();
    *out << "Slot " << dmbslots[dmb];
    *out <<cgicc::td();
    //
  }
  //
  *out <<cgicc::tr();
  //
  std::vector<DAQMB*> myVector;
  for (unsigned int idx=0; idx<crateVector.size(); idx++) {
    myVector = crateVector[idx]->daqmbs();
    //
    for(unsigned int dmb=0; dmb<myVector.size(); dmb++) {
      //
      *out <<cgicc::td();
      //
      if(dmb==0) {
        *out << crateVector[idx]->GetLabel() ;
	*out <<cgicc::td();
	*out <<cgicc::td();

        if(crateVector[idx]->IsAlive()) *out << "On ";
        else * out << "Off";
        *out <<cgicc::td();
        *out <<cgicc::td();
      }
      unsigned dc=myVector[dmb]->GetCounter(this_dcounter_);
      if ( this_dcounter_<4 )
   	 *out << dc;
      else 
         for( int ii=4; ii>-1; ii--) *out << ((dc>>ii)&0x1);
      *out <<cgicc::td();
    }
    *out <<cgicc::tr();
  }
  //
  *out << cgicc::table() << std::endl;
  //
  }

  void EmuPeripheralCrateMonitor::OCounterSelection(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
     cgicc::Cgicc cgi(in);

     std::string in_value = cgi.getElement("runtype")->getValue(); 
     // std::cout << "Select Crate " << in_value << std::endl;
     if(!in_value.empty())
     {
//        int k=in_value.find(" ",0);
//        std::string value = (k) ? in_value.substr(0,k):in_value;
        for(unsigned i=0; i< crateVector.size(); i++)
        {
           if(in_value==TCounterName[i]) this_tcounter_=i;
        }
     }
// now produce the counter view page
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    //
    cgicc::CgiEnvironment cgiEnvi(in);
    std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
    *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"5; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<std::endl;
    *out << cgicc::b(TCounterName[this_tcounter_]) << std::endl;

  }

///////////////////////////////////////////////////////
// Counters displays
///////////////////////////////////////////////////////
void EmuPeripheralCrateMonitor::CrateTMBCounters(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  int counter_idx[25]={ 0 ,  1,  2,  3,  7, 11, 13, 14, 15, 16,
                        17, 18, 26, 30, 31, 32, 35, 41, 42, 48,
                        50, 54, 57, 62, 77};

#if 0
  int counter_idx[23]={ 0 ,  1,  3,  4,  5, 13, 14, 17, 20, 28,
                        29, 30, 31, 34, 35, 37, 40, 41, 44, 45,
                        48, 49, 64};
#endif

  std::ostringstream output;
  output << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
  output << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  //
  cgicc::CgiEnvironment cgiEnvi(in);
  std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
  //
    *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"3; URL=/"
	 <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<std::endl;
  //
  Page=cgiEnvi.getQueryString();
  std::string crate_name=Page.substr(0,Page.find("=", 0) );
  *out << cgicc::b("Crate: "+crate_name) << std::endl;
  Crate *myCrate=0;
  std::vector<TMB*> myVector;
  for ( unsigned int i = 0; i < crateVector.size(); i++ )
  {
     if(crate_name==crateVector[i]->GetLabel())
     {   myVector = crateVector[i]->tmbs();
         myCrate = crateVector[i];
     }
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
    output << "Slot " <<myVector[tmb]->slot() << " " << myCrate->GetChamber(myVector[tmb])->GetLabel();
    output <<cgicc::td();
    //
  }
  //
  output <<cgicc::tr();
  //
  int count;
  for (int idx=0; idx<25; idx++) {
    count=counter_idx[idx];
    //
    for(unsigned int tmb=0; tmb<myVector.size(); tmb++) {
      //
      output <<cgicc::td();
      //
      if(tmb==0) {
        output << TCounterName[count] ;
	output <<cgicc::td();
	output <<cgicc::td();
      }
      int value = myVector[tmb]->GetCounter(count);
      /* 0xBAADBAAD from VCC for a failed VME access */
      if (  value == 0x3fffffff || value < 0 )
      {
         output << cgicc::span().set("style","color:magenta");
         output << "-1";
         output << cgicc::span();
      }
      else if ( (IsErrCounter[count]>0 && value>0) || (IsErrCounter[count]==0 && value==0) )
      {
         output << cgicc::span().set("style","color:red");
         output << value;
         output << cgicc::span();
      }
     else
   	 output << value ;
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
void EmuPeripheralCrateMonitor::CrateDMBCounters(xgi::Input * in, xgi::Output * out ) 
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
  Crate *myCrate=0;
  std::vector<DAQMB*> myVector;
  for ( unsigned int i = 0; i < crateVector.size(); i++ )
  {
     if(crate_name==crateVector[i]->GetLabel())
     {  myVector = crateVector[i]->daqmbs();
        myCrate = crateVector[i];
     }
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
    *out << "Slot " <<myVector[dmb]->slot() << " " << myCrate->GetChamber(myVector[dmb])->GetLabel();
    *out <<cgicc::td();
  }
  //
  *out <<cgicc::tr();
  //
  for (int count=0; count<9; count++) {
    //
    for(unsigned int dmb=0; dmb<myVector.size(); dmb++) {
      //
      *out <<cgicc::td();
      //
      if(dmb==0) {
        *out << DCounterName[count] ;
	*out <<cgicc::td();
	*out <<cgicc::td();
      }
      unsigned dc=myVector[dmb]->GetCounter(count);
      if ( count<4 )
         *out << dc;
      else
         for( int ii=4; ii>-1; ii--) *out << ((dc>>ii)&0x1);
      *out <<cgicc::td();
    }
    *out <<cgicc::tr();
  }
  //
  *out << cgicc::table();
  //
}
//
void EmuPeripheralCrateMonitor::ResetAllCounters(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::CgiEnvironment cgiEnvi(in);
  std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
  Page=cgiEnvi.getQueryString();
  std::string crate_name=Page.substr(0,Page.find("=", 0) );
  std::vector<TMB*> myVector;
  for ( unsigned int i = 0; i < crateVector.size(); i++ )
  {
     if(crate_name==crateVector[i]->GetLabel()) myVector = crateVector[i]->tmbs();
  }
  for(unsigned int i=0; i<myVector.size(); i++) 
  {
    myVector[i]->ResetCounters();
  }

  this->Default(in,out);
}

void EmuPeripheralCrateMonitor::FullResetTMBC(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  xdata::InfoSpace * is;
  xdata::Vector<xdata::UnsignedInteger32> *tmbdata;
  xdata::Vector<xdata::UnsignedInteger32> *otmbdata;
  std::vector<TMB*> myVector;

  if(!Monitor_Ready_) return;

  for ( unsigned int i = 0; i < crateVector.size(); i++ )
  {
     is = xdata::getInfoSpaceFactory()->get(monitorables_[i]);
     tmbdata = dynamic_cast<xdata::Vector<xdata::UnsignedInteger32> *>(is->find("TMBcounter"));
     otmbdata = dynamic_cast<xdata::Vector<xdata::UnsignedInteger32> *>(is->find("OTMBcounter"));
     for(unsigned ii=0; ii<(otmbdata->size()); ii++) (*otmbdata)[ii]=0;
     for(unsigned ii=0; ii<(tmbdata->size()); ii++) (*tmbdata)[ii]=0;

     if(crateVector[i]->IsAlive())
     {
        myVector = crateVector[i]->tmbs();
        for(unsigned int j=0; j<myVector.size(); j++) 
        {
          myVector[j]->ResetCounters();
        }
     }
  }
}

void EmuPeripheralCrateMonitor::XmlOutput(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {

  unsigned int TOTAL_TMB_COUNTERS=78;
  std::vector<TMB*> myVector;
  int o_value, n_value, i_value;
  xdata::InfoSpace * is;

  if(!Monitor_Ready_) return;
  //
  *out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
  *out << "<?xml-stylesheet type=\"text/xml\" href=\"/emu/pc/counterMonitor_XSL.xml\"?>" << std::endl;
  *out << "<emuCounters dateTime=\"";
  toolbox::TimeVal currentTime;
  xdata::TimeVal now_time = (xdata::TimeVal)currentTime.gettimeofday();
  *out << now_time.toString();
  *out << "\">" << std::endl;

  *out << "  <sample name=\"sliding\" delta_t=\"5\">" << std::endl;

  for ( unsigned int i = 0; i < crateVector.size(); i++ )
  {
     std::string cratename = crateVector[i]->GetLabel();
     // for debug, should not happen
     if(cratename!=(monitorables_[i].substr(8,cratename.length())))
         std::cout << "ERROR: crates out of order in Monitor " << cratename << " " << monitorables_[i] << std::endl;

     is = xdata::getInfoSpaceFactory()->get(monitorables_[i]);
     xdata::Vector<xdata::UnsignedInteger32> *tmbdata = dynamic_cast<xdata::Vector<xdata::UnsignedInteger32> *>(is->find("TMBcounter"));
     xdata::Vector<xdata::UnsignedInteger32> *otmbdata = dynamic_cast<xdata::Vector<xdata::UnsignedInteger32> *>(is->find("OTMBcounter"));
     
     if(tmbdata==NULL || tmbdata->size()==0) continue;
     if(otmbdata==NULL || otmbdata->size()==0) continue;

     myVector = crateVector[i]->tmbs();
     for(unsigned int j=0; j<myVector.size(); j++) 
     {
        *out << "    <count chamber=\"";
        *out << crateVector[i]->GetChamber(myVector[j])->GetLabel();
        *out << "\" alct=\"";
//        *out << myVector[j]->GetCounter(0);
        o_value = (*otmbdata)[j*TOTAL_TMB_COUNTERS+0];
        if(o_value == 0x3FFFFFFF || o_value <0) o_value = -1;
        n_value = (*tmbdata)[j*TOTAL_TMB_COUNTERS+0];
        if(n_value == 0x3FFFFFFF || n_value <0) n_value = -1;
        // when a counter has error, should be -1

        i_value = ((o_value>=0 && n_value>=0)?(n_value-o_value):(-1));
        if(i_value<-1) i_value=-1;
        *out << i_value;
        *out << "\" clct=\"";
        o_value = (*otmbdata)[j*TOTAL_TMB_COUNTERS+13];
        if(o_value == 0x3FFFFFFF || o_value <0) o_value = -1;
        n_value = (*tmbdata)[j*TOTAL_TMB_COUNTERS+13];
        if(n_value == 0x3FFFFFFF || n_value <0) n_value = -1;
        // when a counter has error, should be -1

        i_value = ((o_value>=0 && n_value>=0)?(n_value-o_value):(-1));
        if(i_value<-1) i_value=-1;
        *out << i_value;
        *out << "\" lct=\"";
//        *out << myVector[j]->GetCounter(13);
        o_value = (*otmbdata)[j*TOTAL_TMB_COUNTERS+30];
        if(o_value == 0x3FFFFFFF || o_value <0) o_value = -1;
        n_value = (*tmbdata)[j*TOTAL_TMB_COUNTERS+30];
        if(n_value == 0x3FFFFFFF || n_value <0) n_value = -1;
        // when a counter has error, should be -1

        i_value = ((o_value>=0 && n_value>=0)?(n_value-o_value):(-1));
        if(i_value<-1) i_value=-1;
        *out << i_value;
        *out << "\" l1a=\"";
//        *out << myVector[j]->GetCounter(34);
        o_value = (*otmbdata)[j*TOTAL_TMB_COUNTERS+54];
        if(o_value == 0x3FFFFFFF || o_value <0) o_value = -1;
        n_value = (*tmbdata)[j*TOTAL_TMB_COUNTERS+54];
        if(n_value == 0x3FFFFFFF || n_value <0) n_value = -1;
        // counter error, set it to -1:
        i_value = ((o_value>=0 && n_value>=0)?(n_value-o_value):(-1));
        if(i_value<-1) i_value=-1;
        *out << i_value;
        *out << "\"/>" << std::endl;
     }
  }

  *out << "  </sample>" << std::endl;

  *out << "  <sample name=\"cumulative\" delta_t=\"1000\">" << std::endl;

  for ( unsigned int i = 0; i < crateVector.size(); i++ )
  {
     std::string cratename = crateVector[i]->GetLabel();

     is = xdata::getInfoSpaceFactory()->get(monitorables_[i]);
     xdata::Vector<xdata::UnsignedInteger32> *tmbdata = dynamic_cast<xdata::Vector<xdata::UnsignedInteger32> *>(is->find("TMBcounter"));
     if(tmbdata==NULL || tmbdata->size()==0) continue;
     
     myVector = crateVector[i]->tmbs();
     for(unsigned int j=0; j<myVector.size(); j++) 
     {
        *out << "    <count chamber=\"";
        *out << crateVector[i]->GetChamber(myVector[j])->GetLabel();
        *out << "\" alct=\"";
//        *out << myVector[j]->GetCounter(0);
        n_value = (*tmbdata)[j*TOTAL_TMB_COUNTERS+0];
        // counter error, set it to -1 here:
        if(n_value == 0x3FFFFFFF || n_value <0) n_value = -1;
        *out << n_value;
        *out << "\" clct=\"";
//        *out << myVector[j]->GetCounter(5);
        n_value = (*tmbdata)[j*TOTAL_TMB_COUNTERS+13];
        // counter error, set it to -1 here:
        if(n_value == 0x3FFFFFFF || n_value <0) n_value = -1;
        *out << n_value;
        *out << "\" lct=\"";
//        *out << myVector[j]->GetCounter(13);
        n_value = (*tmbdata)[j*TOTAL_TMB_COUNTERS+30];
        // counter error, set it to -1 here:
        if(n_value == 0x3FFFFFFF || n_value <0) n_value = -1;
        *out << n_value;
        *out << "\" l1a=\"";
//        *out << myVector[j]->GetCounter(34);
        n_value = (*tmbdata)[j*TOTAL_TMB_COUNTERS+54];
        // counter error, set it to -1 here:
        if(n_value == 0x3FFFFFFF || n_value <0) n_value = -1;
        *out << n_value;
        *out << "\"/>" << std::endl;
     }
  }

  *out << "  </sample>" << std::endl;
  *out << "</emuCounters>" << std::endl;
}

void EmuPeripheralCrateMonitor::DCSOutput(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {

  unsigned int readtime;
  unsigned short crateok;
  float val;
  std::vector<DAQMB*> myVector;
  int TOTAL_DCS_COUNTERS=48;
  xdata::InfoSpace * is;
  std::string mac;
  int ip, slot;

  if(!Monitor_Ready_)
  {  //  X2P will trigger the start of monitoring.
     //   if monitoring already started but VME access stopped by a button, X2P will not do anything
     ReadingOn();
     return;
  }

  for ( unsigned int i = 0; i < crateVector.size(); i++ )
  {
     is = xdata::getInfoSpaceFactory()->get(monitorables_[i]);
     xdata::Vector<xdata::Float> *dmbdata = dynamic_cast<xdata::Vector<xdata::Float> *>(is->find("DCStemps"));
     if(dmbdata==NULL || dmbdata->size()==0) continue;
     xdata::UnsignedInteger32 *counter32 = dynamic_cast<xdata::UnsignedInteger32 *>(is->find("DCSitime"));
     if (counter32==NULL) readtime=0; 
        else readtime = (*counter32);
     xdata::UnsignedShort *counter16 = dynamic_cast<xdata::UnsignedShort *>(is->find("DCScrate"));
     if (counter16==NULL) crateok= 0; 
        else crateok = (*counter16);

     mac=crateVector[i]->vmeController()->GetMAC(0);
     ip=strtol(mac.substr(15,2).c_str(), NULL, 16);
     *out << std::setprecision(5);
     myVector = crateVector[i]->daqmbs();
     for(unsigned int j=0; j<myVector.size(); j++) 
     {
        slot = myVector[j]->slot();
        ip = (ip & 0xff) + slot*256;
        *out << crateVector[i]->GetChamber(myVector[j])->GetLabel();
        *out << " " << crateok << " " << readtime << " " << ip;
        for(int k=0; k<TOTAL_DCS_COUNTERS; k++) 
        {  val= (*dmbdata)[j*TOTAL_DCS_COUNTERS+k];
           *out << " " << val;
        }
        *out << std::endl;
     }
  }

}


void EmuPeripheralCrateMonitor::BeamView(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {

  unsigned int TOTAL_TMB_COUNTERS=78;
  std::vector<TMB*> myVector;
  xdata::InfoSpace * is;

  long long int me_total[5][4], out_total=0, total=0, out_total_i=0, total_i=0;
  double l_sum=0., r_sum=0., t_sum=0., b_sum=0.;
  double l_sum_i=0., r_sum_i=0., t_sum_i=0., b_sum_i=0.;
  double O_T=0., O_T_int=0., R_L=0., R_L_int=0., T_B=0., T_B_int=0.;
  int o_value, n_value, d_value;

  for(int i=0;i<5;i++) for(int j=0;j<4;j++) me_total[i][j]=0;

  if(!Monitor_Ready_) return;

  for ( unsigned int i = 0; i < crateVector.size(); i++ )
  {
     is = xdata::getInfoSpaceFactory()->get(monitorables_[i]);
     xdata::Vector<xdata::UnsignedInteger32> *tmbdata = dynamic_cast<xdata::Vector<xdata::UnsignedInteger32> *>(is->find("TMBcounter"));
     xdata::Vector<xdata::UnsignedInteger32> *otmbdata = dynamic_cast<xdata::Vector<xdata::UnsignedInteger32> *>(is->find("OTMBcounter"));

     if(tmbdata==NULL || tmbdata->size()==0) continue;
     if(otmbdata==NULL || otmbdata->size()==0) continue;

     myVector = crateVector[i]->tmbs();
     for(unsigned int j=0; j<myVector.size(); j++) 
     {
        std::string chname = crateVector[i]->GetChamber(myVector[j])->GetLabel();
        int station = std::atoi(chname.substr(3,1).c_str());
        int ring = std::atoi(chname.substr(5,1).c_str());
        int chnumb = std::atoi(chname.substr(7,2).c_str());

        o_value = (*otmbdata)[j*TOTAL_TMB_COUNTERS+30];
        if(o_value == 0x3FFFFFFF || o_value <0) o_value = -1;
        n_value = (*tmbdata)[j*TOTAL_TMB_COUNTERS+30];
        if(n_value == 0x3FFFFFFF || n_value <0) n_value = -1;
        // when a counter has error, set it to 0 here and in the following:
        d_value = ((o_value>=0 && n_value>=0)?(n_value-o_value):(0));
        if(d_value < 0) d_value = 0;
        if(n_value < 0) n_value = 0;

        total   += d_value;
        total_i += n_value;
        me_total[station][ring] += d_value;
        if(ring>1)
        {  
           out_total   += d_value;
           out_total_i += n_value;
        }           
        else
        {
           if(station==1)
           {
              if(chnumb>=5 && chnumb<=14) 
              {  t_sum += d_value;  t_sum_i += n_value; }
              else if(chnumb>=15 && chnumb<=22)
              {  r_sum += d_value;  r_sum_i += n_value; }
              else if(chnumb>=23 && chnumb<=32)
              {  b_sum += d_value;  b_sum_i += n_value; }
              else 
              {  l_sum += d_value;  l_sum_i += n_value; }
           }
           else
           {
              if(chnumb>=3 && chnumb<=7) 
              {  t_sum += d_value;  t_sum_i += n_value; }
              else if(chnumb>=8 && chnumb<=11)
              {  r_sum += d_value;  r_sum_i += n_value; }
              else if(chnumb>=12 && chnumb<=16)
              {  b_sum += d_value;  b_sum_i += n_value; }
              else 
              {  l_sum += d_value;  l_sum_i += n_value; }
           }
        }
     }
  }
  if(total > total_max) total_max=total;
  if(total < total_min || total_min==0) total_min=total;
  if(total) 
  {  O_T = (double)out_total/(double)total;
     if(O_T > O_T_max) O_T_max = O_T;
     if(O_T < O_T_min) O_T_min = O_T;
  }
  if(r_sum+l_sum>0.)
  {
     R_L = (r_sum-l_sum)/(r_sum+l_sum);
     if(R_L > R_L_max) R_L_max = R_L;
     if(R_L < R_L_min) R_L_min = R_L;
  }
  if(t_sum+b_sum>0.)
  {
     T_B = (t_sum-b_sum)/(t_sum+b_sum);
     if(T_B > T_B_max) T_B_max = T_B;
     if(T_B < T_B_min) T_B_min = T_B;
  }
  if(total_i) 
  {  O_T_int = (double)out_total_i/(double)total_i;
  }
  if(r_sum_i+l_sum_i>0.)
  {
     R_L_int = (r_sum_i-l_sum_i)/(r_sum_i+l_sum_i);
  }
  if(t_sum_i+b_sum_i>0.)
  {
     T_B_int = (t_sum_i-b_sum_i)/(t_sum_i+b_sum_i);
  }
  //
  MyHeader(in,out,"Beam Monitor");
  //
  cgicc::CgiEnvironment cgiEnvi(in);
  //
  std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
  //
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"5; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<std::endl;
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
  //

  *out << cgicc::fieldset().set("style","font-size: 16pt; font-family: courier;");
  *out << cgicc::legend("BEAM Position").set("style","color:green") << std::endl ;
  *out << cgicc::table().set("border","1").set("cellspacing","2");
  //
  // table top
  //
  *out << cgicc::td() << cgicc::td();
  *out << cgicc::td() << "Instantaneous" << cgicc::td();
  *out << cgicc::td() << "Minimum" << cgicc::td();
  *out << cgicc::td() << "Maximum" << cgicc::td();
  *out << cgicc::td() << "Accumulation" << cgicc::td();
  *out << cgicc::tr() << std::endl;
  //
  *out << cgicc::td() << "Total" << cgicc::td();
  *out << cgicc::td() << total << cgicc::td();
  *out << cgicc::td() << total_min << cgicc::td();
  *out << cgicc::td() << total_max << cgicc::td();
  *out << cgicc::td() << total_i << cgicc::td();
  *out << cgicc::tr() << std::endl;
  //
  *out << cgicc::td() << "Outer/Total" << cgicc::td();
  *out << cgicc::td() << O_T << cgicc::td();
  *out << cgicc::td() << O_T_min << cgicc::td();
  *out << cgicc::td() << O_T_max << cgicc::td();
  *out << cgicc::td() << O_T_int << cgicc::td();
  *out << cgicc::tr() << std::endl;
  //
  *out << cgicc::td() << "R-L/R+L" << cgicc::td();
  *out << cgicc::td() << R_L << cgicc::td();
  *out << cgicc::td() << R_L_min << cgicc::td();
  *out << cgicc::td() << R_L_max << cgicc::td();
  *out << cgicc::td() << R_L_int << cgicc::td();
  *out << cgicc::tr() << std::endl;
  //
  *out << cgicc::td() << "T-B/T+B" << cgicc::td();
  *out << cgicc::td() << T_B << cgicc::td();
  *out << cgicc::td() << T_B_min << cgicc::td();
  *out << cgicc::td() << T_B_max << cgicc::td();
  *out << cgicc::td() << T_B_int << cgicc::td();
  *out << cgicc::tr() << std::endl;
  *out << cgicc::table();
 
  *out << cgicc::br();
  *out << cgicc::fieldset();

  *out << cgicc::fieldset().set("style","font-size: 16pt; font-family: courier;");
  *out << cgicc::legend("Chamber Sums").set("style","color:blue") << std::endl ;

  *out << cgicc::table().set("border","1").set("cellspacing","2");

  *out << cgicc::td() << cgicc::td();
  *out << cgicc::td() << "Chamber Sum" << cgicc::td();
  *out << cgicc::td() << cgicc::td();
  *out << cgicc::td() << "Chamber Sum" << cgicc::td();
  *out << cgicc::td() << cgicc::td();
  *out << cgicc::td() << "Chamber Sum"  << cgicc::td() << cgicc::tr() << std::endl;

  *out << cgicc::td() << "ME 1/1" << cgicc::td();
  *out << cgicc::td() << me_total[1][1] << cgicc::td();
  *out << cgicc::td() << "ME 1/2" << cgicc::td();
  *out << cgicc::td() << me_total[1][2] << cgicc::td();
  *out << cgicc::td() << "ME 1/3"  << cgicc::td();
  *out << cgicc::td() << me_total[1][3]  << cgicc::td() << cgicc::tr() << std::endl;

  *out << cgicc::td() << "ME 2/1" << cgicc::td();
  *out << cgicc::td() << me_total[2][1] << cgicc::td();
  *out << cgicc::td() << "ME 2/2" << cgicc::td();
  *out << cgicc::td() << me_total[2][2] << cgicc::td();
  *out << cgicc::td() << cgicc::td();
  *out << cgicc::td() << cgicc::td() << cgicc::tr() << std::endl;

  *out << cgicc::td() << "ME 3/1" << cgicc::td();
  *out << cgicc::td() << me_total[3][1] << cgicc::td();
  *out << cgicc::td() << "ME 3/2" << cgicc::td();
  *out << cgicc::td() << me_total[3][2] << cgicc::td();
  *out << cgicc::td() << cgicc::td();
  *out << cgicc::td() << cgicc::td() << cgicc::tr() << std::endl;

  *out << cgicc::td() << "ME 4/1" << cgicc::td();
  *out << cgicc::td() << me_total[4][1] << cgicc::td();
  *out << cgicc::td() << cgicc::td();
  *out << cgicc::td() << cgicc::td();
  *out << cgicc::td() << cgicc::td();
  *out << cgicc::td() << cgicc::td() << cgicc::tr() << std::endl;

  *out << cgicc::table();
  *out << cgicc::fieldset();

    std::string fullReset = toolbox::toString("/%s/FullResetTMBC",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",fullReset).set("target","_blank") << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Reset Whole Endcap") << std::endl ;
    *out << cgicc::form() << std::endl ;

}

void EmuPeripheralCrateMonitor::SwitchBoard(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception)
{
  cgicc::CgiEnvironment cgiEnvi(in);
  //
  std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
  Page=cgiEnvi.getQueryString();
  std::string command_name=Page.substr(0,Page.find("=", 0) );
  std::string command_argu=Page.substr(Page.find("=", 0)+1);

  if (command_name=="CRATEOFF")
  {
     for ( unsigned int i = 0; i < crateVector.size(); i++ )
     {
        if(command_argu==crateVector[i]->GetLabel())
        {   crate_off[i] = true;
            std::cout << "SwitchBoard: disable crate " << command_argu << std::endl;
        }
     }
  }
  else if (command_name=="CRATEON")
  {
     for ( unsigned int i = 0; i < crateVector.size(); i++ )
     {
        if(command_argu==crateVector[i]->GetLabel())
        {   crate_off[i] = false;
            std::cout << "SwitchBoard: enable crate " << command_argu << std::endl;
        }
     }
  }
  else if (command_name=="LOOPOFF")
  {
     if (command_argu=="FAST") fast_on = false;
     else if (command_argu=="SLOW") slow_on = false;
     else if (command_argu=="EXTRA") extra_on = false;
     std::cout << "SwitchBoard: " << command_argu << " LOOP disabled" << std::endl;
  }
  else if (command_name=="LOOPON")
  {
     if (command_argu=="FAST") fast_on = true;
     else if (command_argu=="SLOW") slow_on = true;
     else if (command_argu=="EXTRA") extra_on = true;
     std::cout << "SwitchBoard: " << command_argu << " LOOP enabled" << std::endl;
  }
  else if (command_name=="VCCRESET")
  {
     if (command_argu=="ON" || command_argu=="on") reload_vcc = true;
     else if (command_argu=="OFF" || command_argu=="off") reload_vcc = false;
     std::cout << "SwitchBoard: VCC Reset " << command_argu << std::endl;
  }
  else if (command_name=="STATUS")
  {
     *out << "Monitor " << (std::string)((Monitor_On_)?"ON":"OFF");
     *out << " Heartbeat " << fast_count;
     *out << " Crates " << crateVector.size() << " ";
     for ( unsigned int i = 0; i < crateVector.size(); i++ )
     {
        char ans;
        if(crate_off[i])  ans='D';
        else              ans='U';
        *out << ans;
     }
     *out << std::endl;
  }
}

void EmuPeripheralCrateMonitor::CrateStatus(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  MyHeader(in,out,"Crate Configuration Status");
  //
  cgicc::CgiEnvironment cgiEnvi(in);
  //
  std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
  //
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"300; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<std::endl;
  //
  Page=cgiEnvi.getQueryString();
  std::string crate_name=Page.substr(0,Page.find("=", 0) );
  *out << cgicc::b("Crate: "+crate_name) << std::endl;
  int mycrate=0;
  for ( unsigned int i = 0; i < crateVector.size(); i++ )
  {
     if(crate_name==crateVector[i]->GetLabel()) mycrate = i;
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
  //
  xdata::InfoSpace * is = xdata::getInfoSpaceFactory()->get(monitorables_[mycrate]);
  xdata::Vector<xdata::UnsignedShort> *ccbdata = dynamic_cast<xdata::Vector<xdata::UnsignedShort> *>(is->find("CCBcounter"));
  if(ccbdata==NULL || ccbdata->size()==0) return;

  unsigned short csra1= (*ccbdata)[0];
  unsigned short csra2= (*ccbdata)[1];
  unsigned short csra3= (*ccbdata)[2];

  *out << cgicc::fieldset().set("style","font-size: 10pt; font-family: arial;");
  *out << "CCB Mode : ";
  if((csra1&0x1) == 1) {
    *out << cgicc::span().set("style","color:green");
    *out << " DLOG";
    *out << cgicc::span();
  } else {
    *out << cgicc::span().set("style","color:red");
    *out << " FPGA";
    *out << cgicc::span();
  }
  *out << cgicc::br();
  *out << "CCB cfg       " << ((csra3>>12)&0x1);
  //
  if(((csra3>>12)&0x1) == 1) {
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
  *out << "TTCrx ready                   " << ((csra3>>13)&0x1);
  if(((csra3>>13)&0x1) == 1) {
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
  *out << "QPLL ready                    " << ((csra3>>14)&0x1);
  if(((csra3>>14)&0x1) == 0) {
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
  *out << "All cfg                       " << ((csra3>>15)&0x1);
  *out << cgicc::br();
  *out << cgicc::fieldset() ;
  //
  // read = (thisCCB->ReadRegister(0x2))&0xffff;
  //
  *out << cgicc::fieldset().set("style","font-size: 10pt; font-family: arial;");
  *out << "MPC cfg             " << (csra2&0x1);
  *out << cgicc::br();
  *out << cgicc::fieldset() ;
  //
  *out << cgicc::fieldset().set("style","font-size: 10pt; font-family: arial;");
  *out << "ALCT slot = 02 cfg            " << ((csra2>>1)&0x1);
  *out << cgicc::br();
  *out << "ALCT slot = 04 cfg            " << ((csra2>>2)&0x1);
  *out << cgicc::br();
  *out << "ALCT slot = 06 cfg            " << ((csra2>>3)&0x1);
  *out << cgicc::br();
  *out << "ALCT slot = 08 cfg            " << ((csra2>>4)&0x1);
  *out << cgicc::br();
  *out << "ALCT slot = 10 cfg            " << ((csra2>>5)&0x1);
  *out << cgicc::br();
  *out << "ALCT slot = 14 cfg            " << ((csra2>>6)&0x1);
  *out << cgicc::br();
  *out << "ALCT slot = 16 cfg            " << ((csra2>>7)&0x1);
  *out << cgicc::br();
  *out << "ALCT slot = 18 cfg            " << ((csra2>>8)&0x1);
  *out << cgicc::br();
  *out << "ALCT slot = 20 cfg            " << ((csra2>>9)&0x1);
  *out << cgicc::br();
  *out << cgicc::fieldset() ;
  //
  *out << cgicc::fieldset().set("style","font-size: 10pt; font-family: arial;");
  *out << "TMB  slot = 02 cfg            " << ((csra2>>10)&0x1);
  *out << cgicc::br();
  *out << "TMB  slot = 04 cfg            " << ((csra2>>11)&0x1);
  *out << cgicc::br();
  *out << "TMB  slot = 06 cfg            " << ((csra2>>12)&0x1);
  *out << cgicc::br();
  *out << "TMB  slot = 08 cfg            " << ((csra2>>13)&0x1);
  *out << cgicc::br();
  *out << "TMB  slot = 10 cfg            " << ((csra2>>14)&0x1);
  *out << cgicc::br();
  *out << "TMB  slot = 14 cfg            " << ((csra2>>15)&0x1);
  *out << cgicc::br();
  //
  // read = (thisCCB->ReadRegister(0x4))&0xffff;
  //
  *out << "TMB  slot = 16 cfg            " << ((csra3)&0x1);
  *out << cgicc::br();
  *out << "TMB  slot = 18 cfg            " << ((csra3>>1)&0x1);
  *out << cgicc::br();
  *out << "TMB  slot = 20 cfg            " << ((csra3>>2)&0x1);
  *out << cgicc::br();
  *out << cgicc::fieldset() ;
  //
  *out << cgicc::fieldset().set("style","font-size: 10pt; font-family: arial;");
  *out << "DMB  slot = 03 cfg            " << ((csra3>>3)&0x1);
  *out << cgicc::br();
  *out << "DMB  slot = 05 cfg            " << ((csra3>>4)&0x1);
  *out << cgicc::br();
  *out << "DMB  slot = 07 cfg            " << ((csra3>>5)&0x1);
  *out << cgicc::br();
  *out << "DMB  slot = 09 cfg            " << ((csra3>>6)&0x1);
  *out << cgicc::br();
  *out << "DMB  slot = 11 cfg            " << ((csra3>>7)&0x1);
  *out << cgicc::br();
  *out << "DMB  slot = 15 cfg            " << ((csra3>>8)&0x1);
  *out << cgicc::br();
  *out << "DMB  slot = 17 cfg            " << ((csra3>>9)&0x1);
  *out << cgicc::br();
  *out << "DMB  slot = 19 cfg            " << ((csra3>>10)&0x1);
  *out << cgicc::br();
  *out << "DMB  slot = 21 cfg            " << ((csra3>>11)&0x1);
  *out << cgicc::br();
  *out << cgicc::fieldset() ;
  //
}

  void EmuPeripheralCrateMonitor::CheckCrates(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
  {  
    msgHandler("Button: Check Crate Controllers", 0);
    check_controllers();
    this->Default(in, out);
  }

void EmuPeripheralCrateMonitor::check_controllers()
{
    if(total_crates_<=0) return;
    for(unsigned i=0; i< crateVector.size(); i++)
    {
        crateVector[i]->CheckController();
    }
    controller_checked_ = true;
}

void EmuPeripheralCrateMonitor::InitCounterNames()
{
    TCounterName.clear();
    DCounterName.clear();
    OCounterName.clear();    
    IsErrCounter.clear();
    //

    TCounterName.push_back( "ALCT: alct0 valid pattern flag received                 "); // 0 --
    TCounterName.push_back( "ALCT: alct1 valid pattern flag received                 ");
    TCounterName.push_back( "ALCT: alct data structure Error                         ");
    TCounterName.push_back( "ALCT: trigger path ECC; 1 bit Error corrected           ");
    TCounterName.push_back( "ALCT: trigger path ECC; 2 bit Error uncorrected         ");

    TCounterName.push_back( "ALCT: trigger path ECC; > 2 bit Error uncorrected       "); // 5
    TCounterName.push_back( "ALCT: trigger path ECC; > 2 bit Error blanked           ");
    TCounterName.push_back( "ALCT: alct replied ECC; 1 bit Error corrected           ");
    TCounterName.push_back( "ALCT: alct replied ECC; 2 bit Error uncorrected         ");
    TCounterName.push_back( "ALCT: alct replied ECC; > 2 bit Error uncorrected       ");

    TCounterName.push_back( "ALCT: raw hits readout                                  "); // 10
    TCounterName.push_back( "ALCT: raw hits readout - CRC Error                      "); 
    TCounterName.push_back( "RESERVED                                                ");
    TCounterName.push_back( "CLCT: Pretrigger                                        "); // 13 --
    TCounterName.push_back( "CLCT: Pretrigger on CFEB0                               ");

    TCounterName.push_back( "CLCT: Pretrigger on CFEB1                               "); // 15
    TCounterName.push_back( "CLCT: Pretrigger on CFEB2                               "); 
    TCounterName.push_back( "CLCT: Pretrigger on CFEB3                               ");
    TCounterName.push_back( "CLCT: Pretrigger on CFEB4                               ");
    TCounterName.push_back( "CLCT: Pretrigger on ME1A CFEB 4 only                    ");

    TCounterName.push_back( "CLCT: Pretrigger on ME1B CFEBs 0-3 only                 "); // 20
    TCounterName.push_back( "CLCT: Discarded, no wrbuf available, buffer stalled     "); 
    TCounterName.push_back( "CLCT: Discarded, no ALCT in window                      ");
    TCounterName.push_back( "CLCT: Discarded, CLCT0 invalid pattern after drift      ");
    TCounterName.push_back( "CLCT: CLCT0 pass hit thresh, fail pid_thresh_postdrift  ");

    TCounterName.push_back( "CLCT: CLCT1 pass hit thresh, fail pid_thresh_postdrift  "); // 25
    TCounterName.push_back( "CLCT: BX pretrig waiting for triads to dissipate        "); 
    TCounterName.push_back( "CLCT: clct0 sent to TMB matching section                ");
    TCounterName.push_back( "CLCT: clct1 sent to TMB matching section                ");
    TCounterName.push_back( "TMB:  TMB accepted alct*clct, alct-only, or clct-only   ");

    TCounterName.push_back( "TMB:  TMB clct*alct matched trigger                     "); // 30 --
    TCounterName.push_back( "TMB:  TMB alct-only trigger                             "); 
    TCounterName.push_back( "TMB:  TMB clct-only trigger                             ");
    TCounterName.push_back( "TMB:  TMB match reject event                            ");
    TCounterName.push_back( "TMB:  TMB match reject event, queued for nontrig readout");

    TCounterName.push_back( "TMB:  TMB matching discarded an ALCT pair               "); // 35
    TCounterName.push_back( "TMB:  TMB matching discarded a CLCT pair                "); 
    TCounterName.push_back( "TMB:  TMB matching discarded CLCT0 from ME1A            ");
    TCounterName.push_back( "TMB:  TMB matching discarded CLCT1 from ME1A            ");
    TCounterName.push_back( "TMB:  Matching found no ALCT                            ");

    TCounterName.push_back( "TMB:  Matching found no CLCT                            "); // 40
    TCounterName.push_back( "TMB:  Matching found one ALCT                           "); 
    TCounterName.push_back( "TMB:  Matching found one CLCT                           ");
    TCounterName.push_back( "TMB:  Matching found two ALCTs                          ");
    TCounterName.push_back( "TMB:  Matching found two CLCTs                          ");

    TCounterName.push_back( "TMB:  ALCT0 copied into ALCT1 to make 2nd LCT           "); // 45
    TCounterName.push_back( "TMB:  CLCT0 copied into CLCT1 to make 2nd LCT           "); 
    TCounterName.push_back( "TMB:  LCT1 has higher quality than LCT0 (ranking Error) ");
    TCounterName.push_back( "TMB:  Transmitted LCT0 to MPC                           ");
    TCounterName.push_back( "TMB:  Transmitted LCT1 to MPC                           ");

    TCounterName.push_back( "TMB:  MPC accepted LCT0                                 "); // 50
    TCounterName.push_back( "TMB:  MPC accepted LCT1                                 "); 
    TCounterName.push_back( "TMB:  MPC rejected both LCT0 and LCT1                   ");
    TCounterName.push_back( "L1A:  L1A received                                      ");
    TCounterName.push_back( "L1A:  L1A received, TMB in L1A window                   "); // 54 --

    TCounterName.push_back( "L1A:  L1A received, no TMB in window                    "); // 55
    TCounterName.push_back( "L1A:  TMB triggered, no L1A in window                   "); 
    TCounterName.push_back( "L1A:  TMB readouts completed                            ");
    TCounterName.push_back( "STAT: CLCT Triads skipped                               ");
    TCounterName.push_back( "STAT: Raw hits buffer had to be reset                   ");

    TCounterName.push_back( "STAT: TTC Resyncs received                              "); // 60
    TCounterName.push_back( "STAT: Sync Error, BC0/BXN=offset mismatch               "); 
    TCounterName.push_back( "STAT: Parity Error in CFEB or RPC raw hits RAM          ");
    TCounterName.push_back( "HDR:  Pretrigger counter                                ");
    TCounterName.push_back( "HDR:  CLCT counter                                      ");

    TCounterName.push_back( "HDR:  TMB trigger counter                               "); // 65
    TCounterName.push_back( "HDR:  ALCTs received counter                            ");
    TCounterName.push_back( "HDR:  L1As received counter (12 bits)                   ");
    TCounterName.push_back( "HDR:  Readout counter (12 bits)                         ");
    TCounterName.push_back( "HDR:  Orbit counter                                     ");

    TCounterName.push_back( "ALCT:Struct Error, expect ALCT0[10:1]=0 when alct0vpf=0 "); // 70
    TCounterName.push_back( "ALCT:Struct Error, expect ALCT1[10:1]=0 when alct1vpf=0 ");
    TCounterName.push_back( "ALCT:Struct Error, expect ALCT0vpf=1 when alct1vpf=1    ");
    TCounterName.push_back( "ALCT:Struct Error, expect ALCT0[10:1]>0 when alct0vpf=1 ");
    TCounterName.push_back( "ALCT:Struct Error, expect ALCT1[10:1]=0 when alct1vpf=1 ");

    TCounterName.push_back( "CCB:  TTCrx lock lost                                   ");// 75
    TCounterName.push_back( "CCB:  qPLL lock lost                                    "); 
    TCounterName.push_back( "TMB:  Time since last Hard Reset                        "); 

#if 0

    TCounterName.push_back( "ALCT: alct0 valid pattern flag received                 "); // 0 --
    TCounterName.push_back( "ALCT: alct1 valid pattern flag received                 ");
    TCounterName.push_back( "ALCT: alct data structure Error                         ");
    TCounterName.push_back( "ALCT: raw hits readout                                  ");
    TCounterName.push_back( "ALCT: raw hits readout - CRC Error                      ");

    TCounterName.push_back( "CLCT: Pretrigger                                        "); // 5 --
    TCounterName.push_back( "CLCT: Pretrigger on ME1A CFEB 4 only                    ");
    TCounterName.push_back( "CLCT: Pretrigger on ME1B CFEBs 0-3 only                 ");
    TCounterName.push_back( "CLCT: Discarded, no wrbuf available, buffer stalled     ");
    TCounterName.push_back( "CLCT: Discarded, no ALCT in window                      ");

    TCounterName.push_back( "CLCT: Discarded, CLCT0 invalid pattern after drift      "); // 10
    TCounterName.push_back( "CLCT: CLCT0 pass hit thresh, fail pid_thresh_postdrift  ");
    TCounterName.push_back( "CLCT: CLCT1 pass hit thresh, fail pid_thresh_postdrift  ");
    TCounterName.push_back( "CLCT: BX pretrig waiting for triads to dissipate        ");
    TCounterName.push_back( "CLCT: clct0 sent to TMB matching section                ");
    TCounterName.push_back( "CLCT: clct1 sent to TMB matching section                "); // 15

    TCounterName.push_back( "TMB:  TMB accepted alct*clct, alct-only, or clct-only   "); // 16
    TCounterName.push_back( "TMB:  TMB clct*alct matched trigger                     "); // 17 --
    TCounterName.push_back( "TMB:  TMB alct-only trigger                             ");
    TCounterName.push_back( "TMB:  TMB clct-only trigger                             ");

    TCounterName.push_back( "TMB:  TMB match reject event                            "); // 20
    TCounterName.push_back( "TMB:  TMB match reject event, queued for nontrig readout");
    TCounterName.push_back( "TMB:  TMB matching discarded an ALCT pair               ");
    TCounterName.push_back( "TMB:  TMB matching discarded a CLCT pair                ");
    TCounterName.push_back( "TMB:  TMB matching discarded CLCT0 from ME1A            ");
    TCounterName.push_back( "TMB:  TMB matching discarded CLCT1 from ME1A            "); // 25

    TCounterName.push_back( "TMB:  Matching found no ALCT                            ");
    TCounterName.push_back( "TMB:  Matching found no CLCT                            ");
    TCounterName.push_back( "TMB:  Matching found one ALCT                           ");
    TCounterName.push_back( "TMB:  Matching found one CLCT                           ");
    TCounterName.push_back( "TMB:  Matching found two ALCTs                          "); // 30
    TCounterName.push_back( "TMB:  Matching found two CLCTs                          ");

    TCounterName.push_back( "TMB:  ALCT0 copied into ALCT1 to make 2nd LCT           ");
    TCounterName.push_back( "TMB:  CLCT0 copied into CLCT1 to make 2nd LCT           ");
    TCounterName.push_back( "TMB:  LCT1 has higher quality than LCT0 (Ranking Error) ");

    TCounterName.push_back( "TMB:  Transmitted LCT0 to MPC                           "); // 35
    TCounterName.push_back( "TMB:  Transmitted LCT1 to MPC                           ");

    TCounterName.push_back( "TMB:  MPC accepted LCT0                                 "); // 37
    TCounterName.push_back( "TMB:  MPC accepted LCT1                                 ");
    TCounterName.push_back( "TMB:  MPC rejected both LCT0 and LCT1                   ");

    TCounterName.push_back( "L1A:  L1A received                                      "); // 40
    TCounterName.push_back( "L1A:  L1A received, TMB in L1A window                   "); // 41 --
    TCounterName.push_back( "L1A:  L1A received, no TMB in window                    ");
    TCounterName.push_back( "L1A:  TMB triggered, no L1A in window                   ");
    TCounterName.push_back( "L1A:  TMB readouts completed                            ");

    TCounterName.push_back( "STAT: CLCT Triads skipped                               "); // 45
    TCounterName.push_back( "STAT: Raw hits buffer had to be reset Error             ");
    TCounterName.push_back( "STAT: TTC Resyncs received                              ");
    TCounterName.push_back( "STAT: Sync Error, BC0/BXN=offset mismatch               ");
    TCounterName.push_back( "STAT: Parity Error in CFEB or RPC raw hits RAM          ");

    TCounterName.push_back( "HDR:  Pretrigger counter                                "); // 50
    TCounterName.push_back( "HDR:  CLCT counter                                      ");
    TCounterName.push_back( "HDR:  TMB trigger counter                               ");
    TCounterName.push_back( "HDR:  ALCTs received counter                            ");
    TCounterName.push_back( "HDR:  L1As received counter (12 bits)                   ");
    TCounterName.push_back( "HDR:  Readout counter (12 bits)                         "); // 55
    TCounterName.push_back( "HDR:  Orbit counter                                     ");

    TCounterName.push_back( "ALCT:Struct Error, expect ALCT0[10:1]=0 when alct0vpf=0 "); // 57
    TCounterName.push_back( "ALCT:Struct Error, expect ALCT1[10:1]=0 when alct1vpf=0 ");
    TCounterName.push_back( "ALCT:Struct Error, expect ALCT0vpf=1 when alct1vpf=1    ");
    TCounterName.push_back( "ALCT:Struct Error, expect ALCT0[10:1]>0 when alct0vpf=1 "); // 60
    TCounterName.push_back( "ALCT:Struct Error, expect ALCT1[10:1]=0 when alct1vpf=1 ");

    TCounterName.push_back( "CCB:  TTCrx lock lost                                   "); //62
    TCounterName.push_back( "CCB:  qPLL lock lost                                    "); //63
    TCounterName.push_back( "TMB: Time since last Hard Reset                         "); //64
#endif

    DCounterName.push_back( "L1A to LCT delay");  // 0
    DCounterName.push_back( "CFEB DAV delay  ");
    DCounterName.push_back( "TMB DAV delay   ");
    DCounterName.push_back( "ALCT DAV delay  ");
    DCounterName.push_back( "CFEB DAV Scope  ");  // 4
    DCounterName.push_back( "TMB DAV Scope   ");
    DCounterName.push_back( "ALCT DAV Scope  ");
    DCounterName.push_back( "ACTIVE DAV Scope");
    DCounterName.push_back( "L1A to LCT Scope");  // 8

    OCounterName.push_back( "CCB mode"); // 0
    OCounterName.push_back( "TTCrx   ");
    OCounterName.push_back( "QPLL    ");
    OCounterName.push_back( "MPC     ");
    OCounterName.push_back( "CSRA1 ");
    OCounterName.push_back( "CSRA2 ");
    OCounterName.push_back( "CSRA3 ");
    OCounterName.push_back( "BRSTR ");
    OCounterName.push_back( "DTSTR ");
    OCounterName.push_back( "RESET ");
    OCounterName.push_back( "MPC CSR0  ");
    OCounterName.push_back( "MPC CSR4  "); // 10
    OCounterName.push_back( "MPC CSR7  ");
    OCounterName.push_back( "MPC CSR8  "); // 12

    LVCounterName.push_back( "CFEB1 3.3V ");  // 0
    LVCounterName.push_back( "CFEB1 5V   ");  //
    LVCounterName.push_back( "CFEB1 6V   ");  //
    LVCounterName.push_back( "CFEB2 3.3V ");  // 3
    LVCounterName.push_back( "CFEB2 5V   ");  //
    LVCounterName.push_back( "CFEB2 6V   ");  //
    LVCounterName.push_back( "CFEB3 3.3V ");  // 6
    LVCounterName.push_back( "CFEB3 5V   ");  //
    LVCounterName.push_back( "CFEB3 6V   ");  //
    LVCounterName.push_back( "CFEB4 3.3V ");  // 9
    LVCounterName.push_back( "CFEB4 5V   ");  //
    LVCounterName.push_back( "CFEB4 6V   ");  //
    LVCounterName.push_back( "CFEB5 3.3V ");  // 12
    LVCounterName.push_back( "CFEB5 5V   ");  //
    LVCounterName.push_back( "CFEB5 6V   ");  //
    LVCounterName.push_back( "ALCT 3.3V  ");  // 15
    LVCounterName.push_back( "ALCT 1.8V  ");  //
    LVCounterName.push_back( "ALCT 5.5V B");  //
    LVCounterName.push_back( "ALCT 5.5V A");  // 18

    TECounterName.push_back( "DMB Temp  ");  // 0
    TECounterName.push_back( "CFEB1 Temp");  // 1
    TECounterName.push_back( "CFEB2 Temp");  // 
    TECounterName.push_back( "CFEB3 Temp");  //
    TECounterName.push_back( "CFEB4 Temp");  // 
    TECounterName.push_back( "CFEB5 Temp");  // 5
    TECounterName.push_back( "ALCT  Temp");  // 
    TECounterName.push_back( "TMB Temp  ");  // 7

    for(unsigned i=0; i<TCounterName.size(); i++)
    { 
      if( TCounterName[i].find("Error") < 100 || TCounterName[i].find("lost")< 100 )
      {   IsErrCounter.push_back(1);
      }
      else
      {   IsErrCounter.push_back(0);
      }
    }
    IsErrCounter[21]=1;
    IsErrCounter[22]=1;
    IsErrCounter[23]=1;
    IsErrCounter[24]=1;
    IsErrCounter[25]=1;
    IsErrCounter[39]=1;
    IsErrCounter[40]=1;
    IsErrCounter[52]=1;
    IsErrCounter[55]=1;
    IsErrCounter[75]=1;
    IsErrCounter[76]=1;

#if 0
    IsErrCounter[8]=1;
    IsErrCounter[9]=1;
    IsErrCounter[11]=1;
    IsErrCounter[12]=1;
    IsErrCounter[26]=1;
    IsErrCounter[27]=1;
#endif

}

void EmuPeripheralCrateMonitor::msgHandler(std::string msg, int msglevel)
{
     std::string logmsg = getLocalDateTime() + " " + msg;
     if(msglevel>0) LOG4CPLUS_INFO(getApplicationLogger(), logmsg);
     std::cout << logmsg << std::endl;
}

void EmuPeripheralCrateMonitor::ForEmuPage1(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
  *out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl
       << "<?xml-stylesheet type=\"text/xml\" href=\"/emu/base/html/EmuPage1_XSL.xml\"?>" << std::endl
       << "<ForEmuPage1 application=\"" << getApplicationDescriptor()->getClassName()
       <<                   "\" url=\"" << getApplicationDescriptor()->getContextDescriptor()->getURL()
       <<         "\" localDateTime=\"" << getLocalDateTime() << "\">" << std::endl;

    *out << "  <monitorable name=\"" << "title"
         <<            "\" value=\"" << "PCrate Monitor " + (std::string)((endcap_side==1)?"Plus":"Minus")
         <<  "\" nameDescription=\"" << " "
         << "\" valueDescription=\"" << " "
         <<          "\" nameURL=\"" << " "
         <<         "\" valueURL=\"" << main_url_
         << "\"/>" << std::endl;

    *out << "  <monitorable name=\"" << "VME Access"
         <<            "\" value=\"" << (std::string)((Monitor_On_)?"ON":"OFF")
         <<  "\" nameDescription=\"" << " "
         << "\" valueDescription=\"" << " "
         <<          "\" nameURL=\"" << " "
         <<         "\" valueURL=\"" << " "
         << "\"/>" << std::endl;

    *out << "  <monitorable name=\"" << "Heartbeat"
         <<            "\" value=\"" << fast_count
         <<  "\" nameDescription=\"" << " "
         << "\" valueDescription=\"" << " "
         <<          "\" nameURL=\"" << " "
         <<         "\" valueURL=\"" << " "
         << "\"/>" << std::endl;

  *out << "</ForEmuPage1>" << std::endl;
}

 }  // namespace emu::pc
}  // namespace emu

// provides factory method for instantion of XDAQ application
//
XDAQ_INSTANTIATOR_IMPL(emu::pc::EmuPeripheralCrateMonitor)
//
