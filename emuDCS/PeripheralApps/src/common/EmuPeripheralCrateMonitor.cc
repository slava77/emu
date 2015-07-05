// $Id: EmuPeripheralCrateMonitor.cc

#include "emu/pc/EmuPeripheralCrateMonitor.h"
#include "emu/pc/PROBLEM.h"

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
  xgi::bind(this,&EmuPeripheralCrateMonitor::DatabaseOutput, "DatabaseOutput");
  xgi::bind(this,&EmuPeripheralCrateMonitor::DatabaseOutput2, "DatabaseOutput2");
  xgi::bind(this,&EmuPeripheralCrateMonitor::EmuCounterNames, "EmuCounterNames");
  xgi::bind(this,&EmuPeripheralCrateMonitor::EmuCounterNames2, "EmuCounterNames2");
  xgi::bind(this,&EmuPeripheralCrateMonitor::XmlOutput, "XmlOutput");
  xgi::bind(this,&EmuPeripheralCrateMonitor::SwitchBoard, "SwitchBoard");
  xgi::bind(this,&EmuPeripheralCrateMonitor::CrateStatus, "CrateStatus");
  xgi::bind(this,&EmuPeripheralCrateMonitor::Problems, "Problems");
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
  xgi::bind(this,&EmuPeripheralCrateMonitor::DCSOutput2, "DCSOutput2");
  xgi::bind(this,&EmuPeripheralCrateMonitor::DCSDefault, "DCSDefault");
  xgi::bind(this,&EmuPeripheralCrateMonitor::DCSMain, "DCSMain");
  xgi::bind(this,&EmuPeripheralCrateMonitor::DCSChamber, "DCSChamber");
  xgi::bind(this,&EmuPeripheralCrateMonitor::DCSCrateLV, "DCSCrateLV");
  xgi::bind(this,&EmuPeripheralCrateMonitor::DCSCrateCUR, "DCSCrateCUR");
  xgi::bind(this,&EmuPeripheralCrateMonitor::DCSCrateTemp, "DCSCrateTemp");
  xgi::bind(this,&EmuPeripheralCrateMonitor::DCSCrateTMB, "DCSCrateTMB");
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
  new_data_ = false;
  fast_on = true;
  slow_on = true;
  extra_on = true;
  reload_vcc = true;
  read_dcfeb = true;
  fast_count = 0;
  slow_count = 0;
  extra_count = 0;
  x2p_count = 0;
  x2p_count2 = 0;
  read_interval=0;

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

  dcs_mask.clear();
  tcs_mask.clear();
  tmb_mask.clear();
  dmb_mask.clear();
  vcc_reset.clear();
  crate_off.clear();
  for(unsigned i=0; i<60; i++) 
  {
     first_read[i]=true;
     donebits_changed[i]=false;
  }
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
         for(unsigned i=0; i<60; i++) first_read[i]=true;
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
     new_data_ = false;
     fireEvent("Halt");
}

void EmuPeripheralCrateMonitor::MonitorStart(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
     ReadingOn();
     PCsendCommandwithAttr("SoapInfo", "MonitorState", "ON", "emu::x2p::EmuDim");
     this->Default(in,out);
}

void EmuPeripheralCrateMonitor::MonitorStop(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
     ReadingOff();
     PCsendCommandwithAttr("SoapInfo", "MonitorState", "OFF", "emu::x2p::EmuDim");
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
  if(Monitor_On_ && Monitor_Ready_ && fast_on)
  {   time_t thistime = ::time(NULL);
      if(old_time) read_interval = thistime - old_time;
      old_time = thistime;
      PublishEmuInfospace(1);
  }
  return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateMonitor::onSlowLoop (xoap::MessageReference message) 
  throw (xoap::exception::Exception) 
{
  // std::cout << "SOAP Slow Loop" << std::endl;
  slow_count++;
  if(Monitor_On_ && Monitor_Ready_ && slow_on) 
  {  
      PublishEmuInfospace(2);
      new_data_ = true;
  }
  return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateMonitor::onExtraLoop (xoap::MessageReference message) 
  throw (xoap::exception::Exception) 
{
  // std::cout << "SOAP Extra Loop" << std::endl;
  extra_count++;
  if(Monitor_On_ && Monitor_Ready_ && extra_on) PublishEmuInfospace(3);
  return createReply(message);
}

void EmuPeripheralCrateMonitor::CreateEmuInfospace()
{
     if(!parsed) ParsingXML();
     if(total_crates_<=0) return;

      std::vector<DAQMB*> myDmbs;
      std::vector<TMB*> myTmbs;
      
        // Initialize TMB,DMB and Other Counter Names
        InitCounterNames();
        //Create infospaces for monitoring
        monitorables_.clear();
        for ( unsigned int i = 0; i < crateVector.size(); i++ )
        {
                toolbox::net::URN urn = this->createQualifiedInfoSpace("EMu_"+(crateVector[i]->GetLabel())+"_PCrate");
                std::cout << "Crate " << i << " " << urn.toString() << std::endl;
                monitorables_.push_back(urn.toString());
                xdata::InfoSpace * is = xdata::getInfoSpaceFactory()->get(urn.toString());
                myDmbs=crateVector[i]->daqmbs();
                myTmbs=crateVector[i]->tmbs();
                int upgraded=0;
                for(unsigned j=0; j<myDmbs.size(); j++)
                {
                   if(myDmbs[j]->GetHardwareVersion()==2) upgraded++;
                }

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
                is->fireItemAvailable("DCFEBmons",new xdata::Vector<xdata::Float>());
                is->fireItemAvailable("DCScrate",new xdata::UnsignedShort(0));
                is->fireItemAvailable("DCSchamber",new xdata::UnsignedShort(0));
                is->fireItemAvailable("DCSitime",new xdata::UnsignedInteger32(0));
                is->fireItemAvailable("DCSstime",new xdata::TimeVal);

            // for TMB temps, voltages
                is->fireItemAvailable("TMBvolts",new xdata::Vector<xdata::Float>());
                is->fireItemAvailable("TMBcrate",new xdata::UnsignedShort(0));
                is->fireItemAvailable("TMBchamber",new xdata::UnsignedShort(0));
                is->fireItemAvailable("TMBitime",new xdata::UnsignedInteger32(0));
                is->fireItemAvailable("TMBstime",new xdata::TimeVal);

            // initialize the float vectors
                xdata::Vector<xdata::Float> *dmbdata = dynamic_cast<xdata::Vector<xdata::Float> *>(is->find("DCStemps"));
                if(dmbdata) for(unsigned ii=0; ii<myDmbs.size()*TOTAL_DCS_COUNTERS; ii++) dmbdata->push_back(0.);
                xdata::Vector<xdata::Float> *tmbdata = dynamic_cast<xdata::Vector<xdata::Float> *>(is->find("TMBvolts"));
                if(tmbdata) for(unsigned ii=0; ii<myTmbs.size()*TOTAL_TMB_VOLTAGES; ii++) tmbdata->push_back(0.);
                if(upgraded>0)
                {
                   xdata::Vector<xdata::Float> *febdata = dynamic_cast<xdata::Vector<xdata::Float> *>(is->find("DCFEBmons"));
                   if(febdata) for(unsigned ii=0; ii<myDmbs.size()*TOTAL_DCFEB_MONS; ii++) febdata->push_back(0.);
                }              
         }
     Monitor_Ready_=true;
}

void EmuPeripheralCrateMonitor::PublishEmuInfospace(int cycle)
{
   //   cycle = 1  fast loop (e.g. TMB/DMB counters)
   //           2  slow loop (e.g. temps/voltages)
   //           3  extra loop (e.g. CCB MPC TTC status)


      Crate * now_crate;
      std::vector<DAQMB*> myDmbs;
      xdata::InfoSpace * is;
      char buf[8000];
      xdata::UnsignedInteger32 *counter32;
      xdata::UnsignedShort *counter16;
      unsigned int *buf4;
      unsigned short *buf2;
      buf2=(unsigned short *)buf;
      buf4=(unsigned int *)buf;
      bool dmbpoweroff[9];
      unsigned short ccbtag;

      bzero(buf, 8000);
      if(cycle<1 || cycle>3) return;
      if(total_crates_<=0) return;
      //update infospaces
      for ( unsigned int i = 0; i < crateVector.size(); i++ )
      {
          if(crate_off[i]) continue;
          is = xdata::getInfoSpaceFactory()->get(monitorables_[i]);
          now_crate=crateVector[i];
          if(now_crate==NULL) continue;
          myDmbs = now_crate->daqmbs();
          int upgraded=0;
          for(unsigned int dmbn=0; dmbn<myDmbs.size(); dmbn++)
          {   
              int mask=myDmbs[dmbn]->GetPowerMask();
              dmbpoweroff[dmbn]= (mask==0x3F);
              if(myDmbs[dmbn]->GetHardwareVersion()==2) upgraded++;
          }
                                                                                   
          // begin: reload VCC's FPGA (F9)
          if(cycle>1 && reload_vcc && !(now_crate->IsAlive()))
          {
                int cr = now_crate->CheckController();
                if (cr)
                {
                   now_crate->vmeController()->reset();
                   vcc_reset[i] = vcc_reset[i] + 1;
                   ::sleep(1);
                   // cr = (now_crate->vmeController()->SelfTest()) && (now_crate->vmeController()->exist(13));
                   // now_crate->SetLife( cr );
                   now_crate->SetLife( true );
                   // continue;  // skip this round of reading if the VCC has been reloaded
                }
          }
          // end: reload

          if(now_crate->IsAlive()) 
          {
             std::string cratename=now_crate->GetLabel();
             if(cycle==3)
             {  
                bzero(buf, 8000);
                now_crate-> MonitorCCB(cycle, buf);
                if(buf2[0])  
                {   // buf2[0]==0 means no good data back
                    xdata::Vector<xdata::UnsignedShort> *ccbdata = dynamic_cast<xdata::Vector<xdata::UnsignedShort> *>(is->find("CCBcounter"));
                    if(ccbdata->size()==0) 
                       for(unsigned ii=0; ii<buf2[0]; ii++) ccbdata->push_back(0);
                    for(unsigned ii=0; ii<buf2[0]; ii++) (*ccbdata)[ii] = buf2[ii+1];
                    if(first_read[i])
                    {
                        ccbmpcreg[i][0] = buf2[1];
                        ccbmpcreg[i][1] = buf2[2];
                        ccbmpcreg[i][2] = buf2[3];
                        ccbmpcreg[i][3] = buf2[12];
                        first_read[i]=false;
                    }
                    else
                    {
                        if((ccbmpcreg[i][0]&1) != (buf2[1] &1)) 
                        {
                           std::cout << "Crate "+cratename+" CCB CSRA1 changed from " << std::hex << ccbmpcreg[i][0] << " to " << buf2[1] << " at " << getLocalDateTime() << std::endl;
                           ccbmpcreg[i][0] = buf2[1];
                        }
                        if((ccbmpcreg[i][1]) != (buf2[2])) 
                        {
                           std::cout << "Crate "+cratename+" CCB CSRA2 changed from " << std::hex << ccbmpcreg[i][1] << " to " << buf2[2] << " at " << getLocalDateTime() << std::endl;
                           ccbmpcreg[i][1] = buf2[2];
                           donebits_changed[i] = true;
                        }
                        if((ccbmpcreg[i][2]) != (buf2[3])) 
                        {
                           std::cout << "Crate "+cratename+" CCB CSRA3 changed from " << std::hex << ccbmpcreg[i][2] << " to " << buf2[3] << " at " << getLocalDateTime() << std::endl;
                           ccbmpcreg[i][2] = buf2[3];
                        }
                        if((ccbmpcreg[i][3]) != (buf2[12])) 
                        {
                           std::cout << "Crate "+cratename+" MPC CSR0 changed from " << std::hex << ccbmpcreg[i][3] << " to " << buf2[12] << " at " << getLocalDateTime() << std::endl;
                           ccbmpcreg[i][3] = buf2[12];
                        }
                        std::cout << std::dec;
                    }
                    ccbtag=buf2[19] & 0xFFF0;
                    if(ccbtag !=0xCCB0)
                    {
                        std::cout << "Crate "+cratename+" CCB tag wrong: " << std::hex << ccbtag << std::dec << "; CCB not configured properly!" << std::endl; 
                    }
                }
             }
             else if( cycle==2)
             {
                bzero(buf, 8000);
                now_crate-> MonitorDCS(cycle, buf, dcs_mask[i]|(tcs_mask[i]<<10));
                if(buf2[0])
                {
                   // std::cout << "Crate " << i << " DCS counters " << buf2[0] << std::endl;
                   xdata::Vector<xdata::Float> *dmbdata = dynamic_cast<xdata::Vector<xdata::Float> *>(is->find("DCStemps"));
                   if(dmbdata->size()==0)
                      for(unsigned ii=0; ii<buf2[0]; ii++) dmbdata->push_back(0.);
                   short bad_read[22][7];
                   for(int kk=3; kk<22; kk=kk+2 ) for(int ll=0; ll<7; ll++) bad_read[kk][ll]=0;
                   for(unsigned ii=0; ii<buf2[0]; ii++)
                   {   unsigned short rdv = buf2[ii+2];
                       unsigned boardid=ii/TOTAL_DCS_COUNTERS;
                       int idx=ii%TOTAL_DCS_COUNTERS;
                       int dversion=0;
                       if(myDmbs[boardid]) dversion=myDmbs[boardid]->GetHardwareVersion();
                     if(dversion<=1)
                     {  
                       if(rdv >= 0xFFF) rdv = 0;
                       if(idx<40)
                       {
                          (*dmbdata)[ii] = 10.0/4096.0*rdv;

                          // for Voltage & Current reading error handling
                          int dmbslot=boardid*2+3;
                          if(dmbslot>11) dmbslot += 2;
                          int cfebnum=(ii%TOTAL_DCS_COUNTERS)%19;
                          if(cfebnum>17) cfebnum=17;
                          cfebnum= cfebnum/3 + 1;       
                          if((dcs_mask[i] & (1<<boardid))>0 || dmbpoweroff[boardid] || (ii%TOTAL_DCS_COUNTERS)==17 || (ii%TOTAL_DCS_COUNTERS)>37)
                          { // ignore masked boards
                            // igonre powered-off boards 
                            // ignore ALCT 5.5B current or Analog/Digital Feed
                            // nothing for empty slots
                          }
                          else if(cratename.substr(4,1)=="1" && dmbslot>16 && cfebnum==5)
                          { // ME 1_3 chambers have no CFEB 5
                          }
                          else     
                          {
                             if(rdv==0 || rdv>=4065)  
                             {   bad_read[dmbslot][cfebnum]++;
                                 bad_read[dmbslot][0]++;
                             }
                          }

                       }
                       else if(idx<46)
                       {  /* DMB Temps */
                          float Vout= rdv/1000.0;
                          if(Vout >0. && Vout<5.0)
                              (*dmbdata)[ii] =1/(0.001049406423+0.0002133635468*log(65000.0/Vout-13000.0)+0.7522287E-7*pow(log(65000.0/Vout-13000.0),3.0))-273.15;
                          else
                              (*dmbdata)[ii] = -500.0;
                       }
                       else if(idx==56)
                       {  /* ALCT Temps */
                          rdv = rdv & 0x3FF;
                          float Vout= (float)(rdv)*1.225/1023.0;
                          if(Vout<1.225)
                              (*dmbdata)[ii] =100.0*(Vout-0.75)+25.0;
                          else
                              (*dmbdata)[ii] = -500.0;

                          // for ALCT temperature reading error handling
                          int tmbslot=boardid+2;
                          if(tmbslot>10) tmbslot += 2;
                          if((dcs_mask[i] & (1<<boardid))>0 || dmbpoweroff[boardid])
                          { // ignore masked boards
                            // igonre powered-off boards
                          }
                          else
                          {
                             if(idx==56 && (rdv==0 || rdv>=1023))
                             {  std::cout << "ALCT Temperature ERROR: " << cratename
                                       << " slot " << tmbslot << " read back " << std::hex << rdv << std::dec
                                       << " at " << getLocalDateTime() << std::endl; 
                             }
                          }
                       }
                       else
                       {
                          (*dmbdata)[ii] = 0.01*rdv;
                       }
                     }
                     else if(dversion==2)
                     {
                       if(idx<55)
                       {
                         (*dmbdata)[ii] = 10.0/4096.0*rdv;
                       }
                       else if(idx==55)
                       {
                         float fv=10.0/4096.0*rdv;
                         float t=sqrt(2.1962*1000000 + 1000000*(1.8639-fv)/3.88)-1481.96;
                         (*dmbdata)[ii] = t;
                       }
                       else if(idx==56)
                       {  /* ALCT Temps */
                          rdv = rdv & 0x3FF;
                          float Vout= (float)(rdv)*1.225/1023.0;
                          if(Vout<1.225)
                              (*dmbdata)[ii] =100.0*(Vout-0.75)+25.0;
                          else
                              (*dmbdata)[ii] = -500.0;
                       }
                       else
                       {
                         (*dmbdata)[ii] = 0.01*rdv;
                       }
                     }
                   }
                   for(int kk=3; kk<22; kk=kk+2)
                   {   if(bad_read[kk][0]>0)
                       {  std::cout << "ERROR reading CFEB ";
                          for(int ll=1; ll<=6; ll++)
                             if(bad_read[kk][ll]>0) std::cout << ll << " ";
                          std::cout << cratename << " slot " << kk << " at " << getLocalDateTime() << std::endl; 
                       }
                   }
                   counter16 = dynamic_cast<xdata::UnsignedShort *>(is->find("DCScrate"));
                   *counter16 = 1;
                   counter16 = dynamic_cast<xdata::UnsignedShort *>(is->find("DCSchamber"));
                   *counter16 = buf2[1];
                   counter32 = dynamic_cast<xdata::UnsignedInteger32 *>(is->find("DCSitime"));
                   *counter32 = time(NULL);
                   int badboard= buf2[1]>>10;
                   if(badboard>0 && badboard<10) 
                      std::cout << "Bad DMB #" << badboard << " in crate " << cratename << " at " << getLocalDateTime() << std::endl; 
                }

                if(upgraded>0)
                {
                  /* DCFEB monitorables */
                  bzero(buf, 8000);
                  now_crate-> MonitorDCS2(cycle, buf, dcs_mask[i], read_dcfeb);
                  if(buf2[0])
                  {
                     // std::cout << "Crate " << i << " DCFEB counters " << buf2[0] << std::endl;
                     xdata::Vector<xdata::Float> *febdata = dynamic_cast<xdata::Vector<xdata::Float> *>(is->find("DCFEBmons"));
                     if(febdata->size()==0)
                        for(unsigned ii=0; ii<buf2[0]; ii++) febdata->push_back(0.);
                     for(unsigned ii=0; ii<buf2[0]; ii++)
                     {   
                        unsigned short rdv = buf2[ii+2];
                        (*febdata)[ii] = 0.01*rdv;
                     }
                  }
                }
                
                /* TMB voltages */
                bzero(buf, 8000);
                now_crate-> MonitorTCS(cycle, buf, tcs_mask[i]);
                if(buf2[0])
                {
                   // std::cout << "Crate " << i << " TCS counters " << buf2[0] << std::endl;
                   xdata::Vector<xdata::Float> *dmbdata = dynamic_cast<xdata::Vector<xdata::Float> *>(is->find("TMBvolts"));
                   if(dmbdata->size()==0)
                      for(unsigned ii=0; ii<buf2[0]; ii++) dmbdata->push_back(0.);
                   for(unsigned ii=0; ii<buf2[0]; ii++)
                   {   unsigned short rdv = buf2[ii+2];
                       // unsigned boardid=ii/16;
                       unsigned vindex = ii%16;
                       if(rdv > 0xFFF) rdv = 0;
                       if(vindex<14)
                       {
                          if(vindex==0)  rdv *= 2; /* 5.0V ch: 1mV -> 2mV */
                          if(vindex>4 && vindex<10)  rdv *= 5; /* current ch: 200 mV -> 1 Amp */
                          (*dmbdata)[ii] = 0.001*rdv;  /* ADC values are in millivolts */
                       }
                       else if(vindex==14)
                       {  /* OTMB GTX RX status */
                          rdv = rdv & 0x7F;
                          (*dmbdata)[ii] = rdv+0.1;
                       }
                       else
                       {  /* ALCT Temps */
                          rdv = rdv & 0x3FF;
                          float Vout= (float)(rdv)*1.225/1023.0;
                          if(Vout<1.225)
                              (*dmbdata)[ii] =100.0*(Vout-0.75)+25.0;
                          else
                              (*dmbdata)[ii] = -500.0;
                       }
                   }
                   counter16 = dynamic_cast<xdata::UnsignedShort *>(is->find("TMBcrate"));
                   *counter16 = 1;
                   counter16 = dynamic_cast<xdata::UnsignedShort *>(is->find("TMBchamber"));
                   *counter16 = buf2[1];
                   counter32 = dynamic_cast<xdata::UnsignedInteger32 *>(is->find("TMBitime"));
                   *counter32 = time(NULL);
                   int badboard= buf2[1]>>10;
                   if(badboard>0 && badboard<10) 
                      std::cout << "Bad TMB #" << badboard << " in crate " << cratename << " at " << getLocalDateTime() << std::endl; 
                }

             }
             else if( cycle==1)
             {
                bzero(buf, 8000);
                now_crate-> MonitorTMB(cycle, buf, tmb_mask[i]);
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
                   unsigned reset_time1=0, reset_time2=0, reset_time3=0;
                   if(buf2[0]/2 >= 7*MAX_TMB_COUNTERS) reset_time1 = buf4[MAX_TMB_COUNTERS*6+TOTAL_TMB_COUNTERS];
                   if(buf2[0]/2 >= 8*MAX_TMB_COUNTERS) reset_time2 = buf4[MAX_TMB_COUNTERS*7+TOTAL_TMB_COUNTERS];
                   if(buf2[0]/2 >= 9*MAX_TMB_COUNTERS) reset_time3 = buf4[MAX_TMB_COUNTERS*8+TOTAL_TMB_COUNTERS];
                   
                   if(donebits_changed[i])
                   {   // if CCB CSRA2 changed, then print out time-since-last-hard-reset
                       std::cout << "Hard Reset time in crate " << cratename << " are: " << reset_time1 << " , " << reset_time2<< " and " << reset_time3 << " at " << getLocalDateTime() << std::endl;
                       donebits_changed[i] = false;
                   }
                   int badboard= buf2[1]>>10;
                   if(badboard>0 && badboard<10)
                      std::cout << "Bad TMB #" << badboard << " in crate " << cratename << " at " << getLocalDateTime() << std::endl;
                }
                bzero(buf, 8000);
                now_crate-> MonitorDMB(cycle, buf, dmb_mask[i]);
                if(buf2[0])
                {
                   // std::cout << "DMB counters " << buf2[0] << std::endl;
                   xdata::Vector<xdata::UnsignedShort> *dmbdata = dynamic_cast<xdata::Vector<xdata::UnsignedShort> *>(is->find("DMBcounter"));
                   if(dmbdata->size()==0)
                      for(unsigned ii=0; ii<buf2[0]; ii++) dmbdata->push_back(0);
                   for(unsigned ii=0; ii<buf2[0]; ii++) (*dmbdata)[ii] = buf2[ii+1];
                   int badboard= buf2[1]>>10;
                   if(badboard>0 && badboard<10)
                      std::cout << "Bad DAQMB #" << badboard << " in crate " << cratename << " at " << getLocalDateTime() << std::endl;
                }
                toolbox::TimeVal currentTime;
                last_read_time = (xdata::TimeVal)currentTime.gettimeofday();
             }
               // is->fireGroupChanged(names, this);
          }
          else
          {  // for non-communicating crates
             if( cycle==2 )
             {
                   std::cout << "Crate " << now_crate->GetLabel() << " inactive at " << getLocalDateTime() << std::endl;
                   counter16 = dynamic_cast<xdata::UnsignedShort *>(is->find("DCScrate"));
                   *counter16 = 0;
                   counter32 = dynamic_cast<xdata::UnsignedInteger32 *>(is->find("DCSitime"));
                   *counter32 = time(NULL);
             }
          }

      }
}

void EmuPeripheralCrateMonitor::MainPage(xgi::Input * in, xgi::Output * out ) 
{
  if(!parsed) ParsingXML();
  //
  std::string LoggerName = getApplicationLogger().getName() ;
  // std::cout << "Name of Logger is " <<  LoggerName <<std::endl;
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

    *out << cgicc::td();
    std::string problems = toolbox::toString("/%s/Problems",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",problems).set("target","_blank") << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","All Problems").set("name", "Problems") << std::endl ;
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
    std::cout << "Main Page: "<< std::dec << total_crates_ << " Crates at " << getLocalDateTime() << std::endl;
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
  if(Xml_or_Db()==0)
  {
     *out << cgicc::b(cgicc::i("Configuration filename : ")) ;
     *out << xmlFile_.toString() << cgicc::br() << std::endl ;
  }
  else
  {
     *out << cgicc::b(cgicc::i("TStore EMU_config_ID : ")) ;
     *out << GetRealKey();
  }

  //
}

// 
void EmuPeripheralCrateMonitor::MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) 
  throw (xgi::exception::Exception) {
  //
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << "<html>" << std::endl;
  //
  //*out << cgicc::title(title) << std::endl;
  //*out << "<a href=\"/\"><img border=\"0\" src=\"/daq/xgi/images/XDAQLogo.gif\" title=\"XDAQ\" alt=\"\" style=\"width: 145px; height: 89px;\"></a>" << std::endl;
  //
  std::string myUrl = getApplicationDescriptor()->getContextDescriptor()->getURL();
  std::string myUrn = getApplicationDescriptor()->getURN().c_str();
  xgi::Utils::getPageHeader(out,title,myUrl,myUrn,"");
  //
}
//
void EmuPeripheralCrateMonitor::Default(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  *out << "<head> <meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<"MainPage"<<"\"> </head>" <<std::endl;
}
//
//
void EmuPeripheralCrateMonitor::DCSDefault(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  *out << "<head> <meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<"DCSMain"<<"\"> </head>" <<std::endl;
}
//

void EmuPeripheralCrateMonitor::DCSMain(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception)
{
       int n_keys, selected_index;
       std::string CounterName, current_ch_name, endcap_name, dcs_chamber_name;
       std::string station_name[9]={"1/1","1/2","1/3","2/1","2/2","3/1","3/2","4/1", "4/2"};
       int         station_size[9]={ 36,   36,   36,   18,   36,  18,    36,   18,    18  };
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
	n_keys = 9;
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
    *out << cgicc::td();
    //
    std::string DCSCrateTMB = toolbox::toString("/%s/DCSCrateTMB",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",DCSCrateTMB).set("target","_blank") << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","TMB Voltages").set("name",crateVector[DCS_this_crate_no_]->GetLabel()) << std::endl ;
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
     int         station_size[9]={ 36,   36,   36,   18,   36,  18,    36,   18 , 18 };

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
     std::string station_name[9]={"1/1","1/2","1/3","2/1","2/2","3/1","3/2","4/1","4/2"};
     int         station_size[9]={ 36,   36,   36,   18,   36,  18,    36,   18,   18 };

     cgicc::Cgicc cgi(in);

     std::string in_value = cgi.getElement("selected")->getValue(); 
     // std::cout << "Select Counter " << in_value << std::endl;
     if(!in_value.empty())
     {
//        int k=in_value.find(" ",0);
//        std::string value = (k) ? in_value.substr(0,k):in_value;
        for(unsigned i=0; i< 9; i++)
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
  float temp_max[8]={40., 40., 40., 40., 40., 40., 40., 40.};
  float temp_min[8]={ 5.,  5.,  5.,  5.,  5.,  5.,  5.,  5.};
  float cv_max[3]={3.5, 5.4, 6.5};
  float cv_min[3]={3.1, 4.6, 5.5};
  float av_max[4]={3.5, 2.0, 6.0, 6.0};
  float av_min[4]={3.1, 1.6, 5.0, 5.0};
  float temp_max2[11]={70., 50., 40., 40., 40., 40., 40., 40., 40., 40., 40.};
  float temp_min2[11]={ 5.,  5.,  5.,  5.,  5.,  5.,  5.,  5.,  5.,  5.,  5.};
  float cv_max2[3]={3.2, 4.3, 6.0};
  float cv_min2[3]={2.8, 3.7, 5.0};
  float val;
  int cfebs=5;
  unsigned int readtime;
  int difftime;

  if(!Monitor_Ready_) return;
  //
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
  *out << "<html>" << std::endl;
  //
  cgicc::CgiEnvironment cgiEnvi(in);
  //
  //std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
  std::string Page=cgicc::form_urldecode(cgiEnvi.getQueryString());
  std::string cham_name=Page.substr(0,Page.find("=", 0) );
  std::vector<DAQMB*> myVector;
  int mycrate=-1, mychamb=-1;
  int DHversion=0;
  for ( unsigned int i = 0; i < crateVector.size(); i++ )
  {
     myVector = crateVector[i]->daqmbs();
     for ( unsigned int j = 0; j < myVector.size(); j++ )
     {
       if(cham_name==crateVector[i]->GetChamber(myVector[j])->GetLabel())
       {  mycrate = i;
          mychamb = j;
          DHversion=myVector[j]->GetHardwareVersion();
       }
     }
  }
  if(mycrate<0 || mychamb<0)
  {  
     *out << "<B><H2> Chamber not found </H2></B>" << std::endl;
     return;
  }
  //
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"300; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<std::endl;
  //
  // chamber 1/3 have only 4 CFEBs
  if(cham_name.substr(3,3)=="1/3") cfebs=4;
  if(cham_name.substr(3,3)=="1/1")
  {  
     cfebs=7;
     for(unsigned i=0; i<3;i++) {  cv_max[i]=cv_max2[i];  cv_min[i]=cv_min2[i];  }
  }
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
  bool dcfebok=true;
  xdata::Vector<xdata::Float> *dcfebdata;
  if(DHversion>=2)
  {
     dcfebdata = dynamic_cast<xdata::Vector<xdata::Float> *>(is->find("DCFEBmons"));
     if(dcfebdata==NULL || dcfebdata->size()==0) dcfebok=false;
  }
  xdata::UnsignedInteger32 *counter32 = dynamic_cast<xdata::UnsignedInteger32 *>(is->find("DCSitime"));
  if (counter32) 
  {
     readtime = (*counter32);
     difftime=time(NULL)-readtime;
     if(difftime>0) *out << " reading was " << difftime << " seconds old" << std::endl;
  }
  *out << std::setprecision(2) << std::fixed ;

  *out << cgicc::br() << cgicc::b("<center> Low Voltages and Currents </center>") << std::endl;

  // CFEBs
  *out << cgicc::table().set("border","1").set("align","center");
  //
  *out <<cgicc::td() << cgicc::td();
  if(DHversion<=1)
  {
    *out <<cgicc::td() << "3.3V" << cgicc::td();
    *out <<cgicc::td() << "I" << cgicc::td();
    *out <<cgicc::td() << "5V" << cgicc::td();
    *out <<cgicc::td() << "I" << cgicc::td();
    *out <<cgicc::td() << "6V" << cgicc::td();
    *out <<cgicc::td() << "I" << cgicc::td();
  }
  else if(DHversion==2)
  {
    *out <<cgicc::td() << "3.0V" << cgicc::td();
    *out <<cgicc::td() << "I" << cgicc::td();
    *out <<cgicc::td() << "4.0V" << cgicc::td();
    *out <<cgicc::td() << "I" << cgicc::td();
    *out <<cgicc::td() << "5.5V" << cgicc::td();
    *out <<cgicc::td() << "I" << cgicc::td();
  }
  *out << cgicc::tr() << std::endl;
  for(int feb=0; feb<cfebs; feb++)
  {
     *out <<cgicc::td() << "CFEB " << feb+1 << cgicc::td();
     for(int cnt=0; cnt<3; cnt++)
     {
        val=(*dcsdata)[mychamb*TOTAL_DCS_COUNTERS+19+((DHversion>=2)?6:0)+3*feb+cnt];
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
        val=(*dcsdata)[mychamb*TOTAL_DCS_COUNTERS+19+15+((DHversion>=2)?12:0)+cnt];
        *out <<cgicc::td();
        if(val<0.)    
           *out << cgicc::span().set("style","color:magenta") << val << cgicc::span();
        else if(val > av_max[cnt] || val < av_min[cnt])
           *out << cgicc::span().set("style","color:red") << val << cgicc::span();
        else 
           *out << val;  
        *out << cgicc::td();
        val=(*dcsdata)[mychamb*TOTAL_DCS_COUNTERS+15+((DHversion>=2)?6:0)+cnt];
        *out <<cgicc::td() << val << cgicc::td();
     }
     *out << cgicc::tr() << std::endl;

  *out << cgicc::table() << cgicc::br() << cgicc::hr()<< std::endl;

  // TEMPs
  *out << cgicc::br() << cgicc::b("<center> Temperatures (C)</center>") << std::endl;
  *out << cgicc::table().set("border","1").set("align","center");
  *out << std::setprecision(1) << std::fixed ;
  //
  if(DHversion<=1)
  {
     *out <<cgicc::td() << cgicc::td();
     *out <<cgicc::td() << "DMB" << cgicc::td();
     *out <<cgicc::td() << "CFEB 1" << cgicc::td();
     *out <<cgicc::td() << "CFEB 2" << cgicc::td();
     *out <<cgicc::td() << "CFEB 3" << cgicc::td();
     *out <<cgicc::td() << "CFEB 4" << cgicc::td();
     if(cfebs>=5) *out <<cgicc::td() << "CFEB 5" << cgicc::td();
     *out <<cgicc::td() << "ALCT" << cgicc::td();
     *out << cgicc::tr() << std::endl;

     *out <<cgicc::td() << "Temperature (C)" << cgicc::td();
     for(int cnt=0; cnt<7; cnt++)
     {
        if(cfebs<5 && cnt==5) continue;
        if(cnt==6)
            val=(*dcsdata)[mychamb*TOTAL_DCS_COUNTERS+56];  // ALCT temp is at position 56
        else        
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
  }
  else if(DHversion==2)
  {
     *out <<cgicc::td() << cgicc::td();
     *out <<cgicc::td() << "OTMB" << cgicc::td();
     *out <<cgicc::td() << "ODMB" << cgicc::td();
     *out <<cgicc::td() << "LVDB" << cgicc::td();
     *out <<cgicc::td() << "ALCT" << cgicc::td();
     *out <<cgicc::td() << "CFEB 1" << cgicc::td();
     *out <<cgicc::td() << "CFEB 2" << cgicc::td();
     *out <<cgicc::td() << "CFEB 3" << cgicc::td();
     *out <<cgicc::td() << "CFEB 4" << cgicc::td();
     *out <<cgicc::td() << "CFEB 5" << cgicc::td();
     *out <<cgicc::td() << "CFEB 6" << cgicc::td();
     *out <<cgicc::td() << "CFEB 7" << cgicc::td();
     *out << cgicc::tr() << std::endl;
     float tp[4];
     tp[0]=(*dcsdata)[mychamb*TOTAL_DCS_COUNTERS+57];
     if(dcfebok) tp[1]=(*dcfebdata)[mychamb*TOTAL_DCFEB_MONS+27*cfebs];
      else tp[1]=0.;
     tp[2]=(*dcsdata)[mychamb*TOTAL_DCS_COUNTERS+55];
     tp[3]=(*dcsdata)[mychamb*TOTAL_DCS_COUNTERS+56];
     *out <<cgicc::td() << "Temperature (C)" << cgicc::td();
     for(int cnt=0; cnt<11; cnt++)
     {
        if(cnt<4)  val=tp[cnt];
        else if(dcfebok) val=(*dcfebdata)[mychamb*TOTAL_DCFEB_MONS+27*(cnt-4)];
        else val=0.;
        *out <<cgicc::td();
        if(val<0.)    
           *out << cgicc::span().set("style","color:magenta") << val << cgicc::span();
        else if(val > temp_max2[cnt] || val < temp_min2[cnt])
           *out << cgicc::span().set("style","color:red") << val << cgicc::span();
        else 
           *out << val;  
        *out <<cgicc::td();
     }     
  }

     *out << cgicc::tr() << std::endl;

  *out << cgicc::table() << std::endl;

}

void EmuPeripheralCrateMonitor::DCSCrateLV(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
  int Total_count=21;
  float lv_max[21]={3.5, 5.4, 6.5, 3.5, 5.4, 6.5, 3.5, 5.4, 6.5, 3.5, 5.4, 6.5, 3.5, 5.4, 6.5, 3.5, 2.0, 6.0, 6.0, 8.0, 8.0};
  float lv_min[21]={3.1, 4.6, 5.5, 3.1, 4.6, 5.5, 3.1, 4.6, 5.5, 3.1, 4.6, 5.5, 3.1, 4.6, 5.5, 3.1, 1.6, 5.0, 5.0, 5.0, 5.0};
  float val;

  if(!Monitor_Ready_) return;
  //
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
  *out << "<html>" << std::endl;
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
  for (int count=0; count<Total_count; count++) 
  {
    //
    for(unsigned int dmb=0; dmb<myVector.size(); dmb++) 
    {
      int DHversion=myVector[dmb]->GetHardwareVersion();
      *out <<cgicc::td();
      //
      if(dmb==0) {
        *out << LVCounterName[count] ;
	*out <<cgicc::td() << cgicc::td();
      }
      *out << std::setprecision(2) << std::fixed;
      val=(*dcsdata)[dmb*TOTAL_DCS_COUNTERS+19+((DHversion>=2)?6:0)+((DHversion>=2 && count>=15)?6:0)+count];
      if(val<0.)    
         *out << cgicc::span().set("style","color:magenta") << val << cgicc::span();
      else if(DHversion<=1 && (val > lv_max[count] || val < lv_min[count]))
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
  int Total_count=19;
//  float lv_max[19]={3.5, 5.4, 6.5, 3.5, 5.4, 6.5, 3.5, 5.4, 6.5, 3.5, 5.4, 6.5, 3.5, 5.4, 6.5, 3.5, 2.0, 6.0, 6.0};
//  float lv_min[19]={3.1, 4.6, 5.5, 3.1, 4.6, 5.5, 3.1, 4.6, 5.5, 3.1, 4.6, 5.5, 3.1, 4.6, 5.5, 3.1, 1.6, 5.0, 5.0};
  float val;

  if(!Monitor_Ready_) return;
  //
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
  *out << "<html>" << std::endl;
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
  *out <<cgicc::tr() << std::endl;
  //
  for (int count=0; count<Total_count; count++) 
  {
    //
    for(unsigned int dmb=0; dmb<myVector.size(); dmb++) 
    {
      int DHversion=myVector[dmb]->GetHardwareVersion();
      *out <<cgicc::td();
      //
      if(dmb==0) {
        *out << LVCounterName[count] ;
	*out <<cgicc::td() << cgicc::td();
      }
      *out << std::setprecision(2) << std::fixed ;
      val=(*dcsdata)[dmb*TOTAL_DCS_COUNTERS+((DHversion>=2 && count>=15)?6:0)+count];
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
    *out <<cgicc::tr() << std::endl;
  }
  //
  *out << cgicc::table();
  //
}

void EmuPeripheralCrateMonitor::DCSCrateTemp(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
  int  Total_Temps=7;
  float temp_max[8]={40., 40., 40., 40., 40., 40., 40., 40.};
  float temp_min[8]={ 5.,  5.,  5.,  5.,  5.,  5.,  5.,  5.};
  float val;

  if(!Monitor_Ready_) return;
  //
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
  *out << "<html>" << std::endl;
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
      *out << std::setprecision(1) << std::fixed;
      if(count==6)
         val=(*dcsdata)[dmb*TOTAL_DCS_COUNTERS+56];  // ALCT temp is at position 56  
      else
         val=(*dcsdata)[dmb*TOTAL_DCS_COUNTERS+40+count];  
      if(val<0.)    
      {
         if(count==5 && val < -50.)  *out << "-";
         else  *out << cgicc::span().set("style","color:magenta") << val << cgicc::span();
      } 
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

void EmuPeripheralCrateMonitor::DCSCrateTMB(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
  int TOTAL_DCS_COUNTERS=16, Total_count=14;
  float lv_max[14]={5.3, 3.5, 1.6, 1.6, 1.1, 5.5, 5.5, 5.5, 5.5, 5.5, 5.5, 5.5, 5.5, 5.5};
  float lv_min[14]={4.7, 3.1, 1.4, 1.4, 0.9, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  float val;

  if(!Monitor_Ready_) return;
  //
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
  *out << "<html>" << std::endl;
  //
  cgicc::CgiEnvironment cgiEnvi(in);
  //
  std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
  //
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"300; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<std::endl;
  //
  Page=cgiEnvi.getQueryString();
  std::string crate_name=Page.substr(0,Page.find("=", 0) );
  std::vector<TMB*> myVector;
  int mycrate=0;
  for ( unsigned int i = 0; i < crateVector.size(); i++ )
  {
     if(crate_name==crateVector[i]->GetLabel())
     {  myVector = crateVector[i]->tmbs();
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

  *out << cgicc::br() << cgicc::b("<center> TMB Voltages and Currents </center>") << std::endl;

  xdata::InfoSpace * is = xdata::getInfoSpaceFactory()->get(monitorables_[mycrate]);

  xdata::Vector<xdata::Float> *dcsdata = dynamic_cast<xdata::Vector<xdata::Float> *>(is->find("TMBvolts"));
  if(dcsdata==NULL || dcsdata->size()==0) return;

  *out << cgicc::table().set("border","1").set("align","center");
  //
  *out <<cgicc::td();
  *out <<cgicc::td();
  //
  for(unsigned int tmb=0; tmb<myVector.size(); tmb++) {
    *out <<cgicc::td();
    *out << crateVector[mycrate]->GetChamber(myVector[tmb])->GetLabel();
    *out <<cgicc::td();
  }
  //
  *out <<cgicc::tr();
  //
  for (int count=0; count<Total_count; count++) {
    //
    for(unsigned int tmb=0; tmb<myVector.size(); tmb++) {
      //
      *out <<cgicc::td();
      //
      if(tmb==0) {
        *out << TVCounterName[count] ;
	*out <<cgicc::td() << cgicc::td();
      }
      *out << std::setprecision(2) << std::fixed;
      val=(*dcsdata)[tmb*TOTAL_DCS_COUNTERS+count];
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

  bool EmuPeripheralCrateMonitor::ParsingXML(bool reload)
{
  if( parsed==0 || (reload && Xml_or_Db() == -1))
  {
    std::string config_src, config_key;
    //
    // Logger logger_ = getApplicationLogger();
    //
    std::cout << "EmuPeripheralCrate reloading..." << std::endl;
    //
    config_src = XML_or_DB_.toString();
    // std::cout << "XML_or_DB: " << config_src << std::endl;
    if(config_src == "xml" || config_src == "XML")
    {
       config_key = xmlFile_.toString();
    }
    else if (config_src == "db" || config_src == "DB")
    {
       config_key = EMU_config_ID_.toString();
    }
    else
    {
       std::cout << "No valid XML_or_DB found..." << std::endl;
       return false;
    }
    if(!CommonParser(config_src, config_key)) return false;
    EmuEndcap *myEndcap = GetEmuEndcap();
    if(myEndcap == NULL) return false;
    crateVector = myEndcap->crates();
    //
    total_crates_=crateVector.size();
    if(total_crates_<=0) return false;
    this_crate_no_=0;

    for ( unsigned int i = 0; i < crateVector.size(); i++ )
    { 
        // for masks
        dcs_mask.push_back(0);
        tcs_mask.push_back(0);
        tmb_mask.push_back(0);
        dmb_mask.push_back(0);

        // for VCC
        vcc_reset.push_back(0);
        crate_off.push_back(false);
    }
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
  return false;
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

    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    *out << "<html>" << std::endl;

    cgicc::CgiEnvironment cgiEnvi(in);
    std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
    *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"10; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" << std::endl;
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
  *out << "<tr>" << std::endl;
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
	 *out << cgicc::td() << cgicc::td();

         if(crateVector[idx]->IsAlive()) 
            *out << cgicc::span().set("style","color:green") << "On " << cgicc::span();
         else
            *out << cgicc::span().set("style","color:red") << "Off" << cgicc::span();
         *out <<cgicc::td() << cgicc::td();
         if(myVector[0]->slot() > 2)
         {
            int blanks = (myVector[0]->slot()-2)/2;
            if(myVector[0]->slot() > 10) blanks--;
            for(int i=0; i<blanks; i++) *out << cgicc::td() << cgicc::td();
         }
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
    *out << "<tr>" << std::endl;
  }
  //
  *out << cgicc::table() << std::endl;
  //
  }

void EmuPeripheralCrateMonitor::CrateView(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
//     unsigned int tmbslots[9]={2,4,6,8,10,14,16,18,20};
     unsigned TOTAL_COUNTS=13;

     cgicc::Cgicc cgi(in);

     // std::cout << "Select Crate View " << std::endl;
    cgicc::CgiEnvironment cgiEnvi(in);

    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    *out << "<html>" << std::endl;

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
  *out << "<tr>" << std::endl;
  //
  for (unsigned int idx=0; idx<crateVector.size(); idx++) {

    // retrieve data from inforspace
    xdata::InfoSpace * is = xdata::getInfoSpaceFactory()->get(monitorables_[idx]);
    xdata::Vector<xdata::UnsignedShort> *ccbdata = dynamic_cast<xdata::Vector<xdata::UnsignedShort> *>(is->find("CCBcounter"));
    unsigned short csra1=0,csra2=0,csra3=0,csrm0=0,brstr1=0,brstr2=0,dtstr=0,qploc=0,ttcrd=0,qperr=0;
    unsigned brstr=0;
    if(!ccbdata) continue;
    if(ccbdata->size()>12)
    {
      csra1=(*ccbdata)[0];  // CCB CSRA1
      csra2=(*ccbdata)[1];
      csra3=(*ccbdata)[2];
      brstr1=(*ccbdata)[8];
      brstr2=(*ccbdata)[9];
      dtstr=(*ccbdata)[10];
      csrm0=(*ccbdata)[11];  // MPC CSR0
      qploc=(*ccbdata)[15];
      ttcrd=(*ccbdata)[16];
      qperr=(*ccbdata)[17];
      brstr = brstr1 + (brstr2<<16);
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
   	   *out << qploc;
           break;
         case 10:
   	   *out << ttcrd;
           break;
         case 11:
   	   *out << qperr;
           break;
         case 12:
           *out << vcc_reset[idx];
           break;
         default:
           *out << "Unknown";
      }
      *out <<cgicc::td();
    }
    *out <<"<tr>" << std::endl;
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
    *out << "<html>" << std::endl;
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

    *out << cgicc::b(TCounterName[this_tcounter_]) << std::endl;

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
	 *out << cgicc::td() << cgicc::td();

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
         *out << cgicc::td() << cgicc::td();
         if(myVector[0]->slot() > 2)
         {
            int blanks = (myVector[0]->slot()-2)/2;
            if(myVector[0]->slot() > 10) blanks--;
            for(int i=0; i<blanks; i++) *out << cgicc::td() << cgicc::td();
         }
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
    *out << "<html>" << std::endl;
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

    *out << cgicc::b(DCounterName[this_dcounter_]) << std::endl;

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
	 *out << cgicc::td() << cgicc::td();

         if(crateVector[idx]->IsAlive()) *out << "On ";
         else * out << "Off";
         *out << cgicc::td() <<cgicc::td();
         if(myVector[0]->slot() > 3)
         {
            int blanks = (myVector[0]->slot()-3)/2;
            if(myVector[0]->slot() > 11) blanks--;
            for(int i=0; i<blanks; i++) *out << cgicc::td() << cgicc::td();
         }
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
    *out << "<html>" << std::endl;
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
                        50, 54, 57, 63, 88};

  std::ostringstream output;
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
  *out << "<html>" << std::endl;
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
  *out << "<html>" << std::endl;
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

void EmuPeripheralCrateMonitor::DatabaseOutput(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {

  std::vector<TMB*> myVector;
  std::vector<DAQMB*> myDmbs;
  int n_value;
  xdata::InfoSpace * is;
  char tcname[5]="TC00";

  if((!Monitor_Ready_) || read_interval<=0) return;
  //
  *out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
  *out << "<emuCounters dateTime=\"";
  *out << last_read_time.toString();
  *out << "\" version=\"1.1\">" << std::endl;

//  *out << "  <sample name=\"cumulative\" delta_t=\"1000\">" << std::endl;

  for ( unsigned int i = 0; i < crateVector.size(); i++ )
  {
     if(crate_off[i]) continue;
 
     std::string cratename = crateVector[i]->GetLabel();

     is = xdata::getInfoSpaceFactory()->get(monitorables_[i]);
     xdata::Vector<xdata::UnsignedInteger32> *tmbdata = dynamic_cast<xdata::Vector<xdata::UnsignedInteger32> *>(is->find("TMBcounter"));
     if(tmbdata==NULL || tmbdata->size()==0) continue;
     
     myVector = crateVector[i]->tmbs();
     myDmbs = crateVector[i]->daqmbs();
     for(unsigned int j=0; j<myVector.size(); j++) 
     {
        if(myVector[j]->GetHardwareVersion()>=2) continue;
        int imask = 0x3F & (myDmbs[j]->GetPowerMask());
        if (imask==0x3F) continue;
        
        *out << "    <count chamber=\"";
        *out << crateVector[i]->GetChamber(myVector[j])->GetLabel();
        *out << "\" ";
        for(int tc=0; tc<TOTAL_TMB_COUNTERS; tc++)
        {
           sprintf(tcname+2,"%02d",tc);
           *out << tcname << "=\"";
           n_value = (*tmbdata)[j*MAX_TMB_COUNTERS+tc];
           // the last counter only has 16 bits, not 32 bits
           if( tc==(TOTAL_TMB_COUNTERS-1) ) n_value &= 0xFFFF;
           // for real counters, if counter error, set it to -1 here:
           if((tc < myVector[j]->GetMaxCounter()) && (n_value == 0x3FFFFFFF || n_value <0)) n_value = -1;
           *out << n_value;
           *out << "\" ";
        }
        *out << "/>" << std::endl;
     }
  }

//  *out << "  </sample>" << std::endl;
  *out << "</emuCounters>" << std::endl;
}

void EmuPeripheralCrateMonitor::DatabaseOutput2(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {

  std::vector<TMB*> myVector;
  std::vector<DAQMB*> myDmbs;
  int n_value;
  xdata::InfoSpace * is;
  char tcname[5]="TC00";

  if((!Monitor_Ready_) || read_interval<=0) return;
  //
  *out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
  *out << "<emuCounters dateTime=\"";
  *out << last_read_time.toString();
  *out << "\" version=\"2.0\">" << std::endl;

//  *out << "  <sample name=\"cumulative\" delta_t=\"1000\">" << std::endl;

  for ( unsigned int i = 0; i < crateVector.size(); i++ )
  {
     if(crate_off[i]) continue;
 
     std::string cratename = crateVector[i]->GetLabel();

     is = xdata::getInfoSpaceFactory()->get(monitorables_[i]);
     xdata::Vector<xdata::UnsignedInteger32> *tmbdata = dynamic_cast<xdata::Vector<xdata::UnsignedInteger32> *>(is->find("TMBcounter"));
     if(tmbdata==NULL || tmbdata->size()==0) continue;
     
     myVector = crateVector[i]->tmbs();
     myDmbs = crateVector[i]->daqmbs();
     for(unsigned int j=0; j<myVector.size(); j++) 
     {
        if(myVector[j]->GetHardwareVersion()<=1) continue;
        int imask = 0x7F & (myDmbs[j]->GetPowerMask());
        if (imask==0x7F) continue;
        
        *out << "    <count chamber=\"";
        *out << crateVector[i]->GetChamber(myVector[j])->GetLabel();
        *out << "\" ";
        for(int tc=0; tc<TOTAL_TMB_COUNTERS2; tc++)
        {
           sprintf(tcname+2,"%02d",tc);
           *out << tcname << "=\"";
           n_value = (*tmbdata)[j*MAX_TMB_COUNTERS+tc];
           // the last one counter only has 16 bits, not 32 bits
           if( tc==(TOTAL_TMB_COUNTERS2-1) ) n_value &= 0xFFFF;
           // for real counters, if counter error, set it to -1 here:
           if((tc < myVector[j]->GetMaxCounter()) && (n_value == 0x3FFFFFFF || n_value <0)) n_value = -1;
           *out << n_value;
           *out << "\" ";
        }
        *out << "/>" << std::endl;
     }
  }

//  *out << "  </sample>" << std::endl;
  *out << "</emuCounters>" << std::endl;
}

void EmuPeripheralCrateMonitor::EmuCounterNames(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {

  char tcname[5]="TC00";

  if((!Monitor_Ready_) || read_interval<=0) return;
  //
  *out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
  *out << "<emuCounterNames dateTime=\"";
  toolbox::TimeVal currentTime;
  xdata::TimeVal now_time = (xdata::TimeVal)currentTime.gettimeofday();
  *out << now_time.toString();
  *out << "\" version=\"1.1\">" << std::endl;
  for(int tc=0; tc<TOTAL_TMB_COUNTERS; tc++)
  {
      *out << "    <count name=\"";
      sprintf(tcname+2,"%02d",tc);
      *out << tcname << "\">";
      *out << TCounterName[tc];
      *out << "</count>" << std::endl;
  }

  *out << "</emuCounterNames>" << std::endl;
}

void EmuPeripheralCrateMonitor::EmuCounterNames2(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {

  char tcname[5]="TC00";

  if((!Monitor_Ready_) || read_interval<=0) return;
  //
  *out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
  *out << "<emuCounterNames dateTime=\"";
  toolbox::TimeVal currentTime;
  xdata::TimeVal now_time = (xdata::TimeVal)currentTime.gettimeofday();
  *out << now_time.toString();
  *out << "\" version=\"2.0\">" << std::endl;
  for(int tc=0; tc<TOTAL_TMB_COUNTERS2; tc++)
  {
      *out << "    <count name=\"";
      sprintf(tcname+2,"%02d",tc);
      *out << tcname << "\">";
      *out << TCounterName2[tc];
      *out << "</count>" << std::endl;
  }

  *out << "</emuCounterNames>" << std::endl;
}

void EmuPeripheralCrateMonitor::XmlOutput(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {

  std::vector<TMB*> myVector;
  int o_value, n_value, i_value;
  xdata::InfoSpace * is;

  if((!Monitor_Ready_) || read_interval<=0) return;
  //
  *out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
  *out << "<?xml-stylesheet type=\"text/xml\" href=\"/emu/emuDCS/PeripheralApps/xml/counterMonitor_XSL.xml\"?>" << std::endl;
  *out << "<emuCounters dateTime=\"";
  *out << last_read_time.toString();
  *out << "\">" << std::endl;

  *out << "  <sample name=\"sliding\" delta_t=\"" << read_interval << "\">" << std::endl;

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
        o_value = (*otmbdata)[j*MAX_TMB_COUNTERS+0];
        if(o_value == 0x3FFFFFFF || o_value <0) o_value = -1;
        n_value = (*tmbdata)[j*MAX_TMB_COUNTERS+0];
        if(n_value == 0x3FFFFFFF || n_value <0) n_value = -1;
        // when a counter has error, should be -1

        i_value = ((o_value>=0 && n_value>=0)?(n_value-o_value):(-1));
        if(i_value<-1) i_value=-1;
        *out << i_value;
        *out << "\" clct=\"";
        o_value = (*otmbdata)[j*MAX_TMB_COUNTERS+13];
        if(o_value == 0x3FFFFFFF || o_value <0) o_value = -1;
        n_value = (*tmbdata)[j*MAX_TMB_COUNTERS+13];
        if(n_value == 0x3FFFFFFF || n_value <0) n_value = -1;
        // when a counter has error, should be -1

        i_value = ((o_value>=0 && n_value>=0)?(n_value-o_value):(-1));
        if(i_value<-1) i_value=-1;
        *out << i_value;
        *out << "\" lct=\"";
//        *out << myVector[j]->GetCounter(13);
        o_value = (*otmbdata)[j*MAX_TMB_COUNTERS+30+((myVector[j]->GetHardwareVersion()>=2)?2:0)];
        if(o_value == 0x3FFFFFFF || o_value <0) o_value = -1;
        n_value = (*tmbdata)[j*MAX_TMB_COUNTERS+30+((myVector[j]->GetHardwareVersion()>=2)?2:0)];
        if(n_value == 0x3FFFFFFF || n_value <0) n_value = -1;
        // when a counter has error, should be -1

        i_value = ((o_value>=0 && n_value>=0)?(n_value-o_value):(-1));
        if(i_value<-1) i_value=-1;
        *out << i_value;
        *out << "\" l1a=\"";
//        *out << myVector[j]->GetCounter(34);
        o_value = (*otmbdata)[j*MAX_TMB_COUNTERS+54+((myVector[j]->GetHardwareVersion()>=2)?2:0)];
        if(o_value == 0x3FFFFFFF || o_value <0) o_value = -1;
        n_value = (*tmbdata)[j*MAX_TMB_COUNTERS+54+((myVector[j]->GetHardwareVersion()>=2)?2:0)];
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
        n_value = (*tmbdata)[j*MAX_TMB_COUNTERS+0];
        // counter error, set it to -1 here:
        if(n_value == 0x3FFFFFFF || n_value <0) n_value = -1;
        *out << n_value;
        *out << "\" clct=\"";
//        *out << myVector[j]->GetCounter(5);
        n_value = (*tmbdata)[j*MAX_TMB_COUNTERS+13];
        // counter error, set it to -1 here:
        if(n_value == 0x3FFFFFFF || n_value <0) n_value = -1;
        *out << n_value;
        *out << "\" lct=\"";
//        *out << myVector[j]->GetCounter(13);
        n_value = (*tmbdata)[j*MAX_TMB_COUNTERS+30+((myVector[j]->GetHardwareVersion()>=2)?2:0)];
        // counter error, set it to -1 here:
        if(n_value == 0x3FFFFFFF || n_value <0) n_value = -1;
        *out << n_value;
        *out << "\" l1a=\"";
//        *out << myVector[j]->GetCounter(34);
        n_value = (*tmbdata)[j*MAX_TMB_COUNTERS+54+((myVector[j]->GetHardwareVersion()>=2)?2:0)];
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

           // status bit pattern:
           //   bit 0 (value   1):  misc. errors
           //       1 (value   2):  chamber power off from Configuration DB
           //       2 (value   4):  data corrupted (in infospace or during transimission)
           //       3 (value   8):  VCC not accessible
           //       4 (value  16):  DMB Reading error
           //       5 (value  32):  crate OFF
           //       6 (value  64):  this DMB module caused reading trouble
           //       7 (value 128):  TMB reading error
           //       8 (value 256):  this TMB module caused reading trouble
           //       9 (value 512):  chamber lost Analog power 
           //      10 (val  1024):  chamber lost Digital power 

  unsigned int readtime;
  unsigned short crateok, good_chamber=0, ccbtag, csra2, csra3;
  unsigned short mpcreg0, mpcreg1=0, mpcreg2=0, mpcreg3=0;
  float val, V7;
  std::vector<DAQMB*> myVector;
  xdata::InfoSpace * is;
  int ip, slot, ch_state;
  unsigned int bad_module, ccbbits;
  bool gooddata, goodtmb;

  if(!Monitor_Ready_)
  {  //  X2P will trigger the start of monitoring.
     //   if monitoring already started but VME access stopped by a button, X2P will not do anything
     ReadingOn();
     ::sleep(3);
  }

  x2p_count++;
  std::cout << "Access " << x2p_count << " at " << getLocalDateTime() << std::endl;

  for ( unsigned int i = 0; i < crateVector.size(); i++ )
  {
     myVector = crateVector[i]->daqmbs();
     ip=crateVector[i]->CrateID();
     if(crate_off[i])
     {  // for OFF crates, send -2. in all fields, timestamp is current
        for(unsigned int j=0; j<myVector.size(); j++) 
        {
           int dversion=myVector[j]->GetHardwareVersion();
           if(dversion==2) continue;
           slot = myVector[j]->slot();
           ip = (ip & 0xff) + slot*256;
           *out << crateVector[i]->GetChamber(myVector[j])->GetLabel();

           // status 32 ==>crate OFF
           *out << " 32";

           *out << " " << time(NULL) << " " << ip;
           *out << " 0 0";
           for(int k=0; k<TOTAL_DCS_COUNTERS; k++) 
           {  
              *out << " -2.";
           }
           for(int k=0; k<TOTAL_TMB_VOLTAGES; k++) 
           {  
              *out << " -2.";
           }
           *out << " -50";  // as end-of-line marker
           *out << std::endl;
        }
        continue;
     }  // end of OFF crates 

     int problem_readings=0;
     is = xdata::getInfoSpaceFactory()->get(monitorables_[i]);
     xdata::Vector<xdata::Float> *dmbdata = dynamic_cast<xdata::Vector<xdata::Float> *>(is->find("DCStemps"));
     if(dmbdata==NULL || dmbdata->size()<myVector.size()*TOTAL_DCS_COUNTERS)
     {  gooddata=false;
        crateok=0;
        readtime=0;
     }
     else
     {
        gooddata=true;
        xdata::UnsignedInteger32 *counter32 = dynamic_cast<xdata::UnsignedInteger32 *>(is->find("DCSitime"));
        if (counter32==NULL) readtime=0; 
           else readtime = (*counter32);
        xdata::UnsignedShort *counter16 = dynamic_cast<xdata::UnsignedShort *>(is->find("DCScrate"));
        if (counter16==NULL) crateok= 0; 
           else crateok = (*counter16);
        counter16 = dynamic_cast<xdata::UnsignedShort *>(is->find("DCSchamber"));
        if (counter16==NULL) good_chamber= 0;
           else good_chamber = (*counter16);
     }
     xdata::Vector<xdata::Float> *tmbdata = dynamic_cast<xdata::Vector<xdata::Float> *>(is->find("TMBvolts"));
     if(tmbdata==NULL || tmbdata->size()<myVector.size()*TOTAL_TMB_VOLTAGES)
     {  goodtmb=false;
     }
     else
     {  goodtmb=true;
     }

     xdata::Vector<xdata::UnsignedShort> *ccbdata = dynamic_cast<xdata::Vector<xdata::UnsignedShort> *>(is->find("CCBcounter"));
     if(ccbdata==NULL || ccbdata->size()==0)
     {  
        // If CCB data not available or not read out yet, assume they (bits and tag) are OK
        csra3=0x3FF8;
        ccbbits=0x3FF80000;
        ccbtag= 0xCCB0;
        mpcreg0= 0x0200;
     }
     else
     {
        csra2= (*ccbdata)[1];
        csra3= (*ccbdata)[2];
        ccbbits= (csra3<<16)+csra2;
        ccbtag= (*ccbdata)[18];
        mpcreg0= (*ccbdata)[11];
        mpcreg1= (*ccbdata)[12];
        mpcreg2= (*ccbdata)[13];
        mpcreg3= (*ccbdata)[14];
     }

     ch_state=0;
     bad_module = (good_chamber>>10) & 0xF;

     *out << std::setprecision(5);
//     myVector = crateVector[i]->daqmbs();
     for(unsigned int j=0; j<myVector.size(); j++) 
     {
        int dversion=myVector[j]->GetHardwareVersion();
        if(dversion==2) continue;
        int imask= 0x3F & (myVector[j]->GetPowerMask());
        bool chamber_off = (imask==0x3F);
        slot = myVector[j]->slot();
        int dmbN = slot/2;
        if(dmbN>5) dmbN--;
        ip = (ip & 0xff) + slot*256;
        std::string cscname=crateVector[i]->GetChamber(myVector[j])->GetLabel();
        *out << cscname;

        ch_state=0;
        if (chamber_off) ch_state |= 2;
        else if (!gooddata) ch_state |= 4;
        else if (crateok==0) ch_state |= 8;
        else if ((good_chamber & (1<<j))==0) ch_state |= 16;

        // the module which probably caused reading trouble
        if(gooddata && ch_state>0 && bad_module==j+1) ch_state |= 64;

        if((ch_state & 0x7F)==0) 
        {
            /* Analog power */
            V7=(*dmbdata)[j*TOTAL_DCS_COUNTERS+38];
            if(V7<3.0) ch_state |= 512;
            /* Digital power */
            V7=(*dmbdata)[j*TOTAL_DCS_COUNTERS+39];
            if(V7<3.0) ch_state |= 1024;
        }
        if(ch_state&0xFDD) problem_readings++;   // possible reading error (exclude bits 3,8)
        *out << " " << ch_state; 
        *out << " " << readtime << " " << ip;

// CCB bits and FPGA bits
        int alctbit=(ccbbits>>dmbN)&1;
        int tmbbit=(ccbbits>>(dmbN+9))&1;
        int dmbbit=1-((ccbbits>>(dmbN+18))&1);   // DM: 1=OK, 0=BAD
        if(cscname.substr(3,3)=="1/3") dmbbit=0;  // ME1/3 DMB bit is not valid
        int confbit=alctbit+(tmbbit<<1)+(dmbbit<<2);
//   CCB bit 7 (value 128) for every chamber is the same and is for CCB itself
        if((ccbtag&0xFFF0)!=0xCCB0) confbit += 128; 
//   CCB bit 8 (value 256) is for CCB clock
        if((csra3!=0) && (csra3!=0xFFFF) && (csra3!=0xbaad) && (csra3&0x6000)!=0x2000)
        {
           confbit += 256; 
           std::cout << crateVector[i]->GetLabel() << " CCB bit-8 set with CSRA3=0x" << std::hex << csra3 << std::dec << " at " << getLocalDateTime() << std::endl;
        }
//   MPC bits 9 (value 512) is for MPC discrete logic
        if((mpcreg0!=0) && (mpcreg0!=0xFFFF) && (mpcreg0!=0xbaad) && (mpcreg0&0x8201)!=0x200)
        {
           confbit += 512; 
           std::cout << crateVector[i]->GetLabel() <<  " MPC bit-9 set with CSR0=0x" << std::hex << mpcreg0 << std::dec << " at " << getLocalDateTime() << std::endl;
        }
//   MPC bits 10 (value 1024) is for other MPC status
//   disabled Jan 23,2015: after hard-reset, registers return to 0. 
//        if((mpcreg3&1)!=1 || (mpcreg2&0x30)!=0x30) confbit += 1024; 

        if((ch_state & 0x28)==0)    // if crate-off or VCC problem, no need to set these bits
           *out << " " << confbit << " 0";
         else
           *out << " 0 0";

        *out << std::setprecision(4) << std::fixed;
        for(int k=0; k<TOTAL_DCS_COUNTERS; k++) 
        {  
           /* for error conditions on bits 0,2-8, don't send data */
           if((ch_state & 0x1FD)==0)
           { 
              val= (*dmbdata)[j*TOTAL_DCS_COUNTERS+k];
              *out << " " << val;
           }
           else
           {
              *out << " -2.";
           }             
        }
        for(int k=0; k<TOTAL_TMB_VOLTAGES; k++) 
        {  
           if(goodtmb)
           { 
              val= (*tmbdata)[j*TOTAL_TMB_VOLTAGES+k];
              *out << " " << val;
           }
           else
           {
              *out << " -2.";
           }             
        }
        *out << " -50" << std::endl;  // as end-of-line marker
     }  // end of chamber loop
     if(problem_readings>5)  crateVector[i]->SetLife(false);  // too many reading errors, probably VCC problem
  }  // end of crate loop

}

void EmuPeripheralCrateMonitor::DCSOutput2(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {

           // status bit pattern:
           //   bit 0 (value   1):  misc. errors
           //       1 (value   2):  chamber power off from Configuration DB
           //       2 (value   4):  data corrupted (in infospace or during transimission)
           //       3 (value   8):  VCC not accessible
           //       4 (value  16):  DMB Reading error
           //       5 (value  32):  crate OFF
           //       6 (value  64):  this DMB module caused reading trouble
           //       7 (value 128):  TMB reading error
           //       8 (value 256):  this TMB module caused reading trouble
           //       9 (value 512):  chamber lost Analog power 
           //      10 (val  1024):  chamber lost Digital power 

  unsigned int readtime;
  unsigned short crateok, good_chamber=0, ccbtag, csra2, csra3;
  unsigned short mpcreg0, mpcreg1=0, mpcreg2=0, mpcreg3=0;
  float val, V7;
  std::vector<DAQMB*> myVector;
  xdata::InfoSpace * is;
  int ip, slot, ch_state;
  unsigned int bad_module, ccbbits;
  bool gooddata, goodtmb, goodfeb;

  if(!Monitor_Ready_)
  {  //  X2P will trigger the start of monitoring.
     //   if monitoring already started but VME access stopped by a button, X2P will not do anything
     ReadingOn();
     ::sleep(3);
  }

  x2p_count2++;
  std::cout << "Access2 " << x2p_count2 << " at " << getLocalDateTime() << std::endl;

  for ( unsigned int i = 0; i < crateVector.size(); i++ )
  {
     myVector = crateVector[i]->daqmbs();
     ip=crateVector[i]->CrateID();
     if(crate_off[i])
     {  // for OFF crates, send -2. in all fields, timestamp is current
        for(unsigned int j=0; j<myVector.size(); j++) 
        {
           int dversion=myVector[j]->GetHardwareVersion();
           if(dversion!=2) continue;
           slot = myVector[j]->slot();
           ip = (ip & 0xff) + slot*256;
           *out << crateVector[i]->GetChamber(myVector[j])->GetLabel();

           // status 32 ==>crate OFF
           *out << " 32";

           *out << " " << time(NULL) << " " << ip;
           *out << " 0 0";
           for(int k=0; k<TOTAL_DCS_COUNTERS; k++) 
           {  
              *out << " -2.";
           }
           for(int k=0; k<TOTAL_TMB_VOLTAGES; k++) 
           {  
              *out << " -2.";
           }
           for(int k=0; k<TOTAL_DCFEB_MONS; k++) 
           {  
              *out << " -2.";
           }
           *out << " -50";  // as end-of-line marker
           *out << std::endl;
        }
        continue;
     }  // end of OFF crates 

     int problem_readings=0;
     is = xdata::getInfoSpaceFactory()->get(monitorables_[i]);
     xdata::Vector<xdata::Float> *dmbdata = dynamic_cast<xdata::Vector<xdata::Float> *>(is->find("DCStemps"));
     if(dmbdata==NULL || dmbdata->size()<myVector.size()*TOTAL_DCS_COUNTERS)
     {  gooddata=false;
        crateok=0;
        readtime=0;
     }
     else
     {
        gooddata=true;
        xdata::UnsignedInteger32 *counter32 = dynamic_cast<xdata::UnsignedInteger32 *>(is->find("DCSitime"));
        if (counter32==NULL) readtime=0; 
           else readtime = (*counter32);
        xdata::UnsignedShort *counter16 = dynamic_cast<xdata::UnsignedShort *>(is->find("DCScrate"));
        if (counter16==NULL) crateok= 0; 
           else crateok = (*counter16);
        counter16 = dynamic_cast<xdata::UnsignedShort *>(is->find("DCSchamber"));
        if (counter16==NULL) good_chamber= 0;
           else good_chamber = (*counter16);
     }
     xdata::Vector<xdata::Float> *tmbdata = dynamic_cast<xdata::Vector<xdata::Float> *>(is->find("TMBvolts"));
     if(tmbdata==NULL || tmbdata->size()<myVector.size()*TOTAL_TMB_VOLTAGES)
     {  goodtmb=false;
     }
     else
     {  goodtmb=true;
     }

     xdata::Vector<xdata::Float> *febdata = dynamic_cast<xdata::Vector<xdata::Float> *>(is->find("DCFEBmons"));
     if(tmbdata==NULL)
     {  goodfeb=false;
     }
     else
     {  goodfeb=true;
     }

     xdata::Vector<xdata::UnsignedShort> *ccbdata = dynamic_cast<xdata::Vector<xdata::UnsignedShort> *>(is->find("CCBcounter"));
     if(ccbdata==NULL || ccbdata->size()==0)
     {  
        // If CCB data not available or not read out yet, assume they (bits and tag) are OK
        csra3=0x3FF8;
        ccbbits=0x3FF80000;
        ccbtag= 0xCCB0;
        mpcreg0= 0x0200;
     }
     else
     {
        csra2= (*ccbdata)[1];
        csra3= (*ccbdata)[2];
        ccbbits= (csra3<<16)+csra2;
        ccbtag= (*ccbdata)[18];
        mpcreg0= (*ccbdata)[11];
        mpcreg1= (*ccbdata)[12];
        mpcreg2= (*ccbdata)[13];
        mpcreg3= (*ccbdata)[14];
     }

     ch_state=0;
     bad_module = (good_chamber>>10) & 0xF;

     *out << std::setprecision(5);
     int upgraded=0;
//     myVector = crateVector[i]->daqmbs();
     for(unsigned int j=0; j<myVector.size(); j++) 
     {
        int dversion=myVector[j]->GetHardwareVersion();
        if(dversion!=2) continue;
        int imask= 0xFF & (myVector[j]->GetPowerMask());
        bool chamber_off = (imask==0xFF);
        slot = myVector[j]->slot();
        int dmbN = slot/2;
        if(dmbN>5) dmbN--;
        ip = (ip & 0xff) + slot*256;
        std::string cscname=crateVector[i]->GetChamber(myVector[j])->GetLabel();
        *out << cscname;

        ch_state=0;
        if (chamber_off) ch_state |= 2;
        else if (!gooddata) ch_state |= 4;
        else if (crateok==0) ch_state |= 8;
        else if ((good_chamber & (1<<j))==0) ch_state |= 16;

        // the module which probably caused reading trouble
        if(gooddata && ch_state>0 && bad_module==j+1) ch_state |= 64;

        if((ch_state & 0x7F)==0) 
        {
            /* Analog power */
            V7=(*dmbdata)[j*TOTAL_DCS_COUNTERS+50];
            if(V7<3.0) ch_state |= 512;
            /* Digital power */
            V7=(*dmbdata)[j*TOTAL_DCS_COUNTERS+51];
            if(V7<3.0) ch_state |= 1024;
        }
        if(ch_state&0xFDD) problem_readings++;   // possible reading error (exclude bits 3,8)
        *out << " " << ch_state; 

        *out << " " << readtime << " " << ip;

// CCB bits and FPGA bits
        int alctbit=(ccbbits>>dmbN)&1;
        int tmbbit=(ccbbits>>(dmbN+9))&1;
        int dmbbit=1-((ccbbits>>(dmbN+18))&1);   // DM: 1=OK, 0=BAD
        if(cscname.substr(3,3)=="1/3") dmbbit=0;  // ME1/3 DMB bit is not valid
        int confbit=alctbit+(tmbbit<<1)+(dmbbit<<2);
//   CCB bit 7 (value 128) for every chamber is the same and is for CCB itself
        if((ccbtag&0xFFFF0)!=0xCCB0) confbit += 128; 
//   CCB bit 8 (value 256) is for CCB clock
        if((csra3!=0) && (csra3!=0xFFFF) && (csra3!=0xbaad) && (csra3&0x6000)!=0x2000)
        {
           confbit += 256; 
           std::cout << crateVector[i]->GetLabel() << " CCB bit-8 set with CSRA3=0x" << std::hex << csra3 << std::dec << " at " << getLocalDateTime() << std::endl;
        }
//   MPC bits 9 (value 512) is for MPC discrete logic
        if((mpcreg0!=0) && (mpcreg0!=0xFFFF) && (mpcreg0!=0xbaad) && (mpcreg0&0x8201)!=0x200)
        {
           confbit += 512; 
           std::cout << crateVector[i]->GetLabel() << " MPC bit-9 set with CSR0=0x" << std::hex << mpcreg0 << std::dec << " at " << getLocalDateTime() << std::endl;
        }
//   MPC bits 10 (value 1024) is for other MPC status
//   disabled Jan 23,2015: after hard-reset, registers return to 0. 
//        if((mpcreg3&1)!=1 || (mpcreg2&0x30)!=0x30) confbit += 1024; 

        if((ch_state & 0x28)==0)    // if crate-off or VCC problem, no need to set these bits
           *out << " " << confbit << " 0";
         else
           *out << " 0 0";

        *out << std::setprecision(4) << std::fixed;
        for(int k=0; k<TOTAL_DCS_COUNTERS; k++) 
        {  
           /* for error conditions on bits 0,2-8, don't send data */
           if((ch_state & 0x1FD)==0)
           { 
              val= (*dmbdata)[j*TOTAL_DCS_COUNTERS+k];
              *out << " " << val;
           }
           else
           {
              *out << " -2.";
           }             
        }
        for(int k=0; k<TOTAL_TMB_VOLTAGES; k++) 
        {  
           if(goodtmb)
           { 
              val= (*tmbdata)[j*TOTAL_TMB_VOLTAGES+k];
              *out << " " << val;
           }
           else
           {
              *out << " -2.";
           }             
        }
        for(int k=0; k<TOTAL_DCFEB_MONS; k++) 
        {  
           if(goodfeb)
           { 
              val= (*febdata)[upgraded*TOTAL_DCFEB_MONS+k];
              *out << " " << val;
           }
           else
           {
              *out << " -2.";
           }             
        }
        *out << " -50" << std::endl;  // as end-of-line marker
        upgraded++;
     }  // end of chamber loop
     if(problem_readings>2)  crateVector[i]->SetLife(false);  // too many reading errors, probably VCC problem
  }  // end of crate loop

}


void EmuPeripheralCrateMonitor::BeamView(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {

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

        o_value = (*otmbdata)[j*MAX_TMB_COUNTERS+30];
        if(o_value == 0x3FFFFFFF || o_value <0) o_value = -1;
        n_value = (*tmbdata)[j*MAX_TMB_COUNTERS+30];
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
  *out << cgicc::td() << "ME 4/2" << cgicc::td();
  *out << cgicc::td() << me_total[4][2] << cgicc::td();
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
  if(command_name=="") return;

  if (command_name=="CRATEOFF")
  {
     if(!parsed) return;
     for ( unsigned int i = 0; i < crateVector.size(); i++ )
     {
        if(command_argu=="ALL" || command_argu==crateVector[i]->GetLabel())
        {   crate_off[i] = true;
            std::cout << "SwitchBoard: disable crate " << command_argu << " at " << getLocalDateTime() << std::endl;
        }
     }
  }
  else if (command_name=="CRATEON")
  {
     if(!parsed) return;
     for ( unsigned int i = 0; i < crateVector.size(); i++ )
     {
        if(command_argu=="ALL" || command_argu==crateVector[i]->GetLabel())
        {   crate_off[i] = false;
            crateVector[i]->SetLife(true);
            std::cout << "SwitchBoard: enable crate " << command_argu << " at " << getLocalDateTime() << std::endl;
        }
     }
  }
  else if (command_name=="LOOPOFF")
  {
     if (command_argu=="FAST") fast_on = false;
     else if (command_argu=="SLOW") slow_on = false;
     else if (command_argu=="EXTRA") extra_on = false;     
     else if (command_argu=="ALL")
     {
         fast_on = false;
         slow_on = false;
         extra_on = false;
     }
     std::cout << "SwitchBoard: " << command_argu << " LOOP disabled" << " at " << getLocalDateTime() << std::endl;
  }
  else if (command_name=="LOOPON")
  {
     if (command_argu=="FAST") fast_on = true;
     else if (command_argu=="SLOW") slow_on = true;
     else if (command_argu=="EXTRA") extra_on = true;
     else if (command_argu=="ALL")
     {
         fast_on = true;
         slow_on = true;
         extra_on = true;
     }
     std::cout << "SwitchBoard: " << command_argu << " LOOP enabled" << " at " << getLocalDateTime() << std::endl;
  }
  else if (command_name=="VCCRESET")
  {
     if (command_argu=="ON" || command_argu=="on") reload_vcc = true;
     else if (command_argu=="OFF" || command_argu=="off") reload_vcc = false;
     std::cout << "SwitchBoard: VCC Reset " << command_argu << " at " << getLocalDateTime() << std::endl;
  }
  else if (command_name=="DCFEB")
  {
     if (command_argu=="ON" || command_argu=="on") read_dcfeb = true;
     else if (command_argu=="OFF" || command_argu=="off") read_dcfeb = false;
     std::cout << "SwitchBoard: DCFEB Read " << command_argu << " at " << getLocalDateTime() << std::endl;
  }
  else if (command_name=="DEBUGON")
  {
     if(!parsed) return;
     for ( unsigned int i = 0; i < crateVector.size(); i++ )
     {
        if(command_argu=="ALL" || command_argu==crateVector[i]->GetLabel())
        {   
            crateVector[i]->vmeController()->Debug(1);
            std::cout << "SwitchBoard: enable debug crate " << command_argu << " at " << getLocalDateTime() << std::endl;
        }
     }
  }
  else if (command_name=="DEBUGOFF")
  {
     if(!parsed) return;
     for ( unsigned int i = 0; i < crateVector.size(); i++ )
     {
        if(command_argu=="ALL" || command_argu==crateVector[i]->GetLabel())
        {  
            crateVector[i]->vmeController()->Debug(0);
            std::cout << "SwitchBoard: disable debug crate " << command_argu << " at " << getLocalDateTime() << std::endl;
        }
     }
  }
  else if (command_name=="STATUS")
  {
     if(!parsed) return;
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
  else if (command_name=="NEWDATA")
  {
     if(!parsed) return;
     if(!new_data_)
     {  
        PublishEmuInfospace(2);
        new_data_ = true;
     }
  }
  else if (command_name=="RELOAD")
  {
     std::cout << " Check Configuration DB, auto reload if needed " << " at " << getLocalDateTime() << std::endl;
     ParsingXML(true);
  }
  else if (command_name=="LISTMASK")
  {
     if(!parsed) return;
     // std::cout << "List of all masks:" << std::endl;
     for ( unsigned int i = 0; i < crateVector.size(); i++ )
     {
        if(dcs_mask[i]) 
          *out << "DCS DMB Mask: " << crateVector[i]->GetLabel() << " 0x" << std::hex << dcs_mask[i] << std::dec << std::endl;
        if(tcs_mask[i]) 
          *out << "DCS TMB Mask: " << crateVector[i]->GetLabel() << " 0x" << std::hex << tcs_mask[i] << std::dec << std::endl;
        if(tmb_mask[i]) 
          *out << "TMB Mask: " << crateVector[i]->GetLabel() << " 0x" << std::hex << tmb_mask[i] << std::dec << std::endl;
        if(dmb_mask[i]) 
          *out << "DMB Mask: " << crateVector[i]->GetLabel() << " 0x" << std::hex << dmb_mask[i] << std::dec << std::endl;
     }
  }
  else if (command_name=="MASKON" || command_name=="MASKOFF")
  {
     if(!parsed) return;
     if(command_argu=="" || command_argu.length()<13) return;
     std::string board=command_argu.substr(0,3);
     std::string detail=command_argu.substr(4);
     std::string maskcrate=detail.substr(0,detail.find("-",0));
     std::string maskst=detail.substr(detail.find("-",0)+1);
     if(maskcrate=="" || maskst=="") return;
     int maskid=atoi(maskst.c_str());
     if(maskid<1 || maskid>9) return;
     bool goodmask=false;

     for ( unsigned int i = 0; i < crateVector.size(); i++ )
     {
        if(maskcrate==crateVector[i]->GetLabel())
        {
            if (board=="DCS" || board=="dcs")
            {
               goodmask=true;
               if(command_name=="MASKON") 
                  dcs_mask[i] |= (1<<(maskid-1));
               else
                  dcs_mask[i] &= ~(1<<(maskid-1));
            }
            else if (board=="TCS" || board=="tcs")
            {
               goodmask=true;
               if(command_name=="MASKON") 
                  tcs_mask[i] |= (1<<(maskid-1));
               else
                  tcs_mask[i] &= ~(1<<(maskid-1));
            }
            else if (board=="TMB" || board=="tmb")
            {
               goodmask=true;
               if(command_name=="MASKON")
                  tmb_mask[i] |= (1<<(maskid-1));
               else
                  tmb_mask[i] &= ~(1<<(maskid-1));
            }
            else if (board=="DMB" || board=="dmb")
            {
               goodmask=true;
               if(command_name=="MASKON")
                  dmb_mask[i] |= (1<<(maskid-1));
               else
                  dmb_mask[i] &= ~(1<<(maskid-1));
            }
        }
     }
     if(goodmask)
        std::cout << "SwitchBoard: " << command_name << " " << board << " " << maskcrate << " " << maskid << " at " << getLocalDateTime() << std::endl;
     else
        std::cout << "SwitchBoard: Invalid " << command_name << " " <<  command_argu << " at " << getLocalDateTime() << std::endl;
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
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"200; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<std::endl;
  //
  Page=cgiEnvi.getQueryString();
  std::string crate_name=Page.substr(0,Page.find("=", 0) );
  *out << cgicc::b("Crate: "+crate_name) << std::endl;
  int mycrate=0;
  Crate *myCrate=0;
  std::vector<DAQMB*> myVector;
  for ( unsigned int i = 0; i < crateVector.size(); i++ )
  {
     if(crate_name==crateVector[i]->GetLabel())
     {   myVector = crateVector[i]->daqmbs();
         myCrate = crateVector[i];
         mycrate = i;
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
    *out << " (OK)";
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
  *out << "All cfg " << ((csra3>>15)&0x1);
  *out << cgicc::br() << std::endl;
  *out << cgicc::fieldset() ;
  //
  // read = (thisCCB->ReadRegister(0x2))&0xffff;
  //
  *out << cgicc::fieldset().set("style","font-size: 10pt; font-family: arial;");
  *out << "MPC cfg ";
  if((csra2&0x1) == 0) {
    *out << cgicc::span().set("style","color:green");
    *out << " OK";
    *out << cgicc::span();
  } else {
    *out << cgicc::span().set("style","color:red");
    *out << " No";
    *out << cgicc::span();
  }
  *out << cgicc::br(); 
  *out << cgicc::fieldset() << std::endl;
  //
  *out << cgicc::table().set("border","1");
  //
  *out << cgicc::td() << cgicc::td();
  //
  for(unsigned int dmb=0; dmb<myVector.size(); dmb++) {
    *out << cgicc::td();
    *out << myCrate->GetChamber(myVector[dmb])->GetLabel();
    *out << cgicc::td();
  }
  //
  *out <<cgicc::tr() << std::endl;
  // 
  *out << cgicc::td() << "ALCT (0=OK)" << cgicc::td();
  for (int count=1; count<=9; count++)
  {
     *out <<cgicc::td() << ((csra2>>count)&0x1);
     *out << cgicc::td();
  }
  *out << cgicc::tr() << std::endl;
  *out << cgicc::td() << "TMB (0=OK)" << cgicc::td();
  for (int count=1; count<=9; count++)
  {
     if(count<7)
         *out <<cgicc::td() << ((csra2>>(count+9))&0x1);
     else
         *out <<cgicc::td() << ((csra3>>(count-7))&0x1);
     *out << cgicc::td();
  }
  *out << cgicc::tr() << std::endl;
  *out << cgicc::td() << "DMB (1=OK)" << cgicc::td();
  for (int count=1; count<=9; count++)
  {
     *out <<cgicc::td() << ((csra3>>(count+2))&0x1);
     *out << cgicc::td();
  }
  *out << cgicc::tr() << std::endl;
  *out << cgicc::table();
  *out << "Note: for ME 1/3 chambers, DMB could be always 0" << std::endl;
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

void EmuPeripheralCrateMonitor::Problems(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) 
{
  //
  MyHeader(in,out,"List of All Problems");
  //
  cgicc::CgiEnvironment cgiEnvi(in);
  //
  std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
  //
  int confbit, prob_crate;
  Crate *now_crate=0;
  std::vector<DAQMB*> myDmbs;
  std::vector<PROBLEM *> problist;

  problist.clear();
  if(Monitor_On_)
  {
     *out << cgicc::span().set("style","color:green");
     *out << cgicc::b(cgicc::i("Monitor Status: On")) << cgicc::span() << std::endl ;
  } else 
  { 
     *out << cgicc::span().set("style","color:red");
     *out << cgicc::b(cgicc::i("Monitor Status: Off")) << cgicc::span() << std::endl ;
  }

/*
  1. Known Prolbems from DB
  2. Problems from CCB Configuration bits
  3. Problems from DCS readings
  4. Problems from TMB counters
  5. Problems from configuration checks
  6. Problems from FPGA checks
*/
  for ( unsigned int i = 0; i < crateVector.size(); i++ )
  {
     prob_crate = i+1+(endcap_side<0?30:0);  // Plus 1-30, Minus 31-60
     now_crate=crateVector[i];
     if(now_crate==NULL)
     {  
        continue;
     }
     if(crate_off[i])
     { 
        PROBLEM *prob=new PROBLEM();
        prob->crate(prob_crate);
        prob->module(ProbModule_CRATE);
        prob->source(ProbSource_USER);
        prob->type(ProbType_SKIP);
        problist.push_back(prob);
        continue;
     }
     xdata::InfoSpace * is = xdata::getInfoSpaceFactory()->get(monitorables_[i]);
     xdata::Vector<xdata::UnsignedShort> *ccbdata = dynamic_cast<xdata::Vector<xdata::UnsignedShort> *>(is->find("CCBcounter"));
     if(ccbdata==NULL || ccbdata->size()==0) return;
     myDmbs = now_crate->daqmbs();
     for(unsigned int dmbn=0; dmbn<myDmbs.size(); dmbn++)
     {
        int mask=myDmbs[dmbn]->GetPowerMask();
        if(mask==0x3F)
        {
           int slot=myDmbs[dmbn]->slot();
           int cscN=slot/2;
           if(cscN>5) cscN--;
           PROBLEM *prob=new PROBLEM();
           prob->crate(prob_crate);
           prob->source(ProbSource_CONFDB);
           prob->type(ProbType_OFF);
           prob->module(ProbModule_FEB);
           prob->chamber(cscN);
           problist.push_back(prob);
        }
     }

     unsigned short csra2= (*ccbdata)[1];
     unsigned short csra3= (*ccbdata)[2];
     unsigned ccbbits= (csra3<<16)+csra2;
     *out <<cgicc::tr() << std::endl;
     // "ALCT (0=OK)" 
     for (int count=1; count<=9; count++)
     {
        int slot=count*2;
        if(count>5) slot += 2;
        Chamber* chmb =now_crate->GetChamber(slot);
        if(chmb==NULL) continue;
        confbit=(ccbbits>>count)&0x1;
        if(confbit!=0)
        {
           PROBLEM *prob=new PROBLEM();
           prob->crate(prob_crate);
           prob->source(ProbSource_CCBBIT);
           prob->type(ProbType_DIFF);
           prob->module(ProbModule_FEB);
           prob->chamber(count);
           prob->sub_mod(ProbSubMod_ALCT);
           problist.push_back(prob);
        }
    
     }
     // "TMB (0=OK)"
     for (int count=1; count<=9; count++)
     {
        unsigned int slot=count*2;
        if(count>5) slot += 2;
        TMB* mytmb =now_crate->GetTMB(slot);
        if(mytmb==NULL) continue;
        confbit = (ccbbits>>(count+9))&0x1;
        if(confbit!=0)
        {
           PROBLEM *prob=new PROBLEM();
           prob->crate(prob_crate);
           prob->source(ProbSource_CCBBIT);
           prob->type(ProbType_DIFF);
           prob->module(ProbModule_TMB);
           prob->chamber(count);
           problist.push_back(prob);
        }
      }
      // "DMB (1=OK)"
      for (int count=1; count<=9; count++)
      {
        unsigned slot=count*2+1;
        if(count>5) slot += 2;
        DAQMB* mydmb =now_crate->GetDAQMB(slot);
        if(mydmb==NULL) continue;
        confbit= (ccbbits>>(count+18))&0x1;
        std::string cscname=now_crate->GetChamber(mydmb)->GetLabel();
        if(confbit!=1 && cscname.substr(3,3)!="1/3")
        {
           PROBLEM *prob=new PROBLEM();
           prob->crate(prob_crate);
           prob->source(ProbSource_CCBBIT);
           prob->type(ProbType_DIFF);
           prob->module(ProbModule_DMB);
           prob->chamber(count);
           problist.push_back(prob);
        }
      }  
  }
  *out << "<pre>" << std::endl;
  *out << "Off chambers in Configuration DB" << std::endl;
  for(unsigned int i=0;i<problist.size(); i++)
  {

     if(problist[i]->source()==ProbSource_CONFDB && problist[i]->type()>0)
     {
        int cr=problist[i]->crate();
        if(cr>30) cr -= 30;   // for Minus Endcap
        int chb=problist[i]->chamber();
        if(cr>0 && chb>0)
        {
           Crate * thiscr=crateVector[cr-1];
           if(thiscr)
           {
              std::string crname=thiscr->GetLabel();
              std::string chbname=thiscr->GetChamberName(chb);
              *out << "Crate=" << crname << ", Chamber=" << chbname << std::endl;  
           }
           else
             *out << "Crate=" << problist[i]->crate() << ", Chamber=" << problist[i]->chamber() << std::endl;
        }
     }
  }
  *out << std::endl <<  "Problems in CCB Configuration bits" << std::endl;
  for(unsigned int i=0;i<problist.size(); i++)
  {
     if(problist[i]->source()==ProbSource_CCBBIT && problist[i]->type()>0)
     {
        int cr=problist[i]->crate();
        if(cr>30) cr -= 30;   // for Minus Endcap
        int chb=problist[i]->chamber();
        int md=problist[i]->module();
        if(cr>0 && chb>0)
        {
           Crate * thiscr=crateVector[cr-1];
           if(thiscr)
           {
              std::string crname=thiscr->GetLabel();
              std::string chbname=thiscr->GetChamberName(chb);
              *out << "Crate=" << crname << ", Chamber=" << chbname;
           }
           else
           {
              *out << "Crate=" << problist[i]->crate() << ", Chamber=" << problist[i]->chamber();
           }
           if(md==6)
              *out << ", Module = ALCT " << std::endl;
           else if(md==4)
              *out << ", Module = TMB " << std::endl;
           else if(md==5)
              *out << ", Module = DMB/CFEB " << std::endl;

        }
     }
  }
  *out << "</pre>" << std::endl;
}

void EmuPeripheralCrateMonitor::InitCounterNames()
{
    TCounterName.clear();
    TCounterName2.clear();
    DCounterName.clear();
    OCounterName.clear();    
    IsErrCounter.clear();
    LVCounterName.clear();
    TECounterName.clear();
    TVCounterName.clear();
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
    TCounterName.push_back( "L1A:  TMB readouts lost by 1-event-per-L1A limit        ");
    TCounterName.push_back( "STAT: CLCT Triads skipped                               ");

    TCounterName.push_back( "STAT: Raw hits buffer had to be reset                   "); // 60
    TCounterName.push_back( "STAT: TTC Resyncs received                              "); 
    TCounterName.push_back( "STAT: Sync Error, BC0/BXN=offset mismatch               "); 
    TCounterName.push_back( "STAT: Parity Error in CFEB or RPC raw hits RAM          ");
    TCounterName.push_back( "HDR:  Pretrigger counter                                ");

    TCounterName.push_back( "HDR:  CLCT counter                                      "); // 65
    TCounterName.push_back( "HDR:  TMB trigger counter                               ");
    TCounterName.push_back( "HDR:  ALCTs received counter                            ");
    TCounterName.push_back( "HDR:  L1As received counter (12 bits)                   ");
    TCounterName.push_back( "HDR:  Readout counter (12 bits)                         ");

    TCounterName.push_back( "HDR:  Orbit counter                                     "); // 70
    TCounterName.push_back( "ALCT:Struct Error, expect ALCT0[10:1]=0 when alct0vpf=0 "); 
    TCounterName.push_back( "ALCT:Struct Error, expect ALCT1[10:1]=0 when alct1vpf=0 ");
    TCounterName.push_back( "ALCT:Struct Error, expect ALCT0vpf=1 when alct1vpf=1    ");
    TCounterName.push_back( "ALCT:Struct Error, expect ALCT0[10:1]>0 when alct0vpf=1 ");

    TCounterName.push_back( "ALCT:Struct Error, expect ALCT1[10:1]=0 when alct1vpf=1 "); // 75
    TCounterName.push_back( "ALCT:Struct Error, expect ALCT1!=alct0 when alct0vpf=1  ");
    TCounterName.push_back( "CCB:  TTCrx lock lost                                   ");
    TCounterName.push_back( "CCB:  qPLL lock lost                                    "); // last of real TMB counters
    TCounterName.push_back( "TMB:  CFEB Bad Bits Control                             ");     

    TCounterName.push_back( "TMB:  CFEB Bad Bits Pattern 1                           "); // 80    
    TCounterName.push_back( "TMB:  CFEB Bad Bits Pattern 2                           "); //    
    TCounterName.push_back( "TMB:  CFEB Bad Bits Pattern 3                           "); //    
    TCounterName.push_back( "TMB:  CFEB Bad Bits Pattern 4                           "); //    
    TCounterName.push_back( "TMB:  CFEB Bad Bits Pattern 5                           "); //    

    TCounterName.push_back( "TMB:  CFEB Bad Bits Pattern 6                           "); // 85   
    TCounterName.push_back( "TMB:  CFEB Bad Bits Pattern 7                           "); //     
    TCounterName.push_back( "TMB:  CFEB Bad Bits Pattern 8                           "); //   
    TCounterName.push_back( "TMB:  Time since last Hard Reset                        "); // 88

    TOTAL_TMB_COUNTERS=TCounterName.size();

    TCounterName2.push_back( "ALCT: alct0 valid pattern flag received                 "); // 0 --
    TCounterName2.push_back( "ALCT: alct1 valid pattern flag received                 ");
    TCounterName2.push_back( "ALCT: alct data structure Error                         ");
    TCounterName2.push_back( "ALCT: trigger path ECC; 1 bit Error corrected           ");
    TCounterName2.push_back( "ALCT: trigger path ECC; 2 bit Error uncorrected         ");

    TCounterName2.push_back( "ALCT: trigger path ECC; > 2 bit Error uncorrected       "); // 5
    TCounterName2.push_back( "ALCT: trigger path ECC; > 2 bit Error blanked           ");
    TCounterName2.push_back( "ALCT: alct replied ECC; 1 bit Error corrected           ");
    TCounterName2.push_back( "ALCT: alct replied ECC; 2 bit Error uncorrected         ");
    TCounterName2.push_back( "ALCT: alct replied ECC; > 2 bit Error uncorrected       ");

    TCounterName2.push_back( "ALCT: raw hits readout                                  "); // 10
    TCounterName2.push_back( "ALCT: raw hits readout - CRC Error                      "); 
    TCounterName2.push_back( "RESERVED                                                ");
    TCounterName2.push_back( "CLCT: Pretrigger                                        "); // 13 --
    TCounterName2.push_back( "CLCT: Pretrigger on CFEB0                               ");

    TCounterName2.push_back( "CLCT: Pretrigger on CFEB1                               "); // 15
    TCounterName2.push_back( "CLCT: Pretrigger on CFEB2                               "); 
    TCounterName2.push_back( "CLCT: Pretrigger on CFEB3                               ");
    TCounterName2.push_back( "CLCT: Pretrigger on CFEB4                               ");
    TCounterName2.push_back( "CLCT: Pretrigger on CFEB5                               ");

    TCounterName2.push_back( "CLCT: Pretrigger on CFEB6                               "); // 20
    TCounterName2.push_back( "CLCT: Pretrigger on ME1A CFEB 4 only                    ");

    TCounterName2.push_back( "CLCT: Pretrigger on ME1B CFEBs 0-3 only                 "); // 22
    TCounterName2.push_back( "CLCT: Discarded, no wrbuf available, buffer stalled     "); 
    TCounterName2.push_back( "CLCT: Discarded, no ALCT in window                      ");
    TCounterName2.push_back( "CLCT: Discarded, CLCT0 invalid pattern after drift      ");
    TCounterName2.push_back( "CLCT: CLCT0 pass hit thresh, fail pid_thresh_postdrift  ");

    TCounterName2.push_back( "CLCT: CLCT1 pass hit thresh, fail pid_thresh_postdrift  "); // 27
    TCounterName2.push_back( "CLCT: BX pretrig waiting for triads to dissipate        "); 
    TCounterName2.push_back( "CLCT: clct0 sent to TMB matching section                ");
    TCounterName2.push_back( "CLCT: clct1 sent to TMB matching section                ");
    TCounterName2.push_back( "TMB:  TMB accepted alct*clct, alct-only, or clct-only   ");

    TCounterName2.push_back( "TMB:  TMB clct*alct matched trigger                     "); // 32 --
    TCounterName2.push_back( "TMB:  TMB alct-only trigger                             "); 
    TCounterName2.push_back( "TMB:  TMB clct-only trigger                             ");
    TCounterName2.push_back( "TMB:  TMB match reject event                            ");
    TCounterName2.push_back( "TMB:  TMB match reject event, queued for nontrig readout");

    TCounterName2.push_back( "TMB:  TMB matching discarded an ALCT pair               "); // 37
    TCounterName2.push_back( "TMB:  TMB matching discarded a CLCT pair                "); 
    TCounterName2.push_back( "TMB:  TMB matching discarded CLCT0 from ME1A            ");
    TCounterName2.push_back( "TMB:  TMB matching discarded CLCT1 from ME1A            ");
    TCounterName2.push_back( "TMB:  Matching found no ALCT                            ");

    TCounterName2.push_back( "TMB:  Matching found no CLCT                            "); // 42
    TCounterName2.push_back( "TMB:  Matching found one ALCT                           "); 
    TCounterName2.push_back( "TMB:  Matching found one CLCT                           ");
    TCounterName2.push_back( "TMB:  Matching found two ALCTs                          ");
    TCounterName2.push_back( "TMB:  Matching found two CLCTs                          ");

    TCounterName2.push_back( "TMB:  ALCT0 copied into ALCT1 to make 2nd LCT           "); // 47
    TCounterName2.push_back( "TMB:  CLCT0 copied into CLCT1 to make 2nd LCT           "); 
    TCounterName2.push_back( "TMB:  LCT1 has higher quality than LCT0 (ranking Error) ");
    TCounterName2.push_back( "TMB:  Transmitted LCT0 to MPC                           ");
    TCounterName2.push_back( "TMB:  Transmitted LCT1 to MPC                           ");

    TCounterName2.push_back( "TMB:  MPC accepted LCT0                                 "); // 52
    TCounterName2.push_back( "TMB:  MPC accepted LCT1                                 "); 
    TCounterName2.push_back( "TMB:  MPC rejected both LCT0 and LCT1                   ");
    TCounterName2.push_back( "L1A:  L1A received                                      ");
    TCounterName2.push_back( "L1A:  L1A received, TMB in L1A window                   "); // 56 --

    TCounterName2.push_back( "L1A:  L1A received, no TMB in window                    "); // 57
    TCounterName2.push_back( "L1A:  TMB triggered, no L1A in window                   "); 
    TCounterName2.push_back( "L1A:  TMB readouts completed                            ");
    TCounterName2.push_back( "L1A:  TMB readouts lost by 1-event-per-L1A limit        ");
    TCounterName2.push_back( "STAT: CLCT Triads skipped                               ");

    TCounterName2.push_back( "STAT: Raw hits buffer had to be reset                   "); // 62
    TCounterName2.push_back( "STAT: TTC Resyncs received                              "); 
    TCounterName2.push_back( "STAT: Sync Error, BC0/BXN=offset mismatch               "); 
    TCounterName2.push_back( "STAT: Parity Error in CFEB or RPC raw hits RAM          ");
    TCounterName2.push_back( "HDR:  Pretrigger counter                                ");

    TCounterName2.push_back( "HDR:  CLCT counter                                      "); // 67
    TCounterName2.push_back( "HDR:  TMB trigger counter                               ");
    TCounterName2.push_back( "HDR:  ALCTs received counter                            ");
    TCounterName2.push_back( "HDR:  L1As received counter (12 bits)                   ");
    TCounterName2.push_back( "HDR:  Readout counter (12 bits)                         ");

    TCounterName2.push_back( "HDR:  Orbit counter                                     "); // 72
    TCounterName2.push_back( "ALCT:Struct Error, expect ALCT0[10:1]=0 when alct0vpf=0 "); 
    TCounterName2.push_back( "ALCT:Struct Error, expect ALCT1[10:1]=0 when alct1vpf=0 ");
    TCounterName2.push_back( "ALCT:Struct Error, expect ALCT0vpf=1 when alct1vpf=1    ");
    TCounterName2.push_back( "ALCT:Struct Error, expect ALCT0[10:1]>0 when alct0vpf=1 ");

    TCounterName2.push_back( "ALCT:Struct Error, expect ALCT1[10:1]=0 when alct1vpf=1 "); // 77
    TCounterName2.push_back( "ALCT:Struct Error, expect ALCT1!=alct0 when alct0vpf=1  ");
    TCounterName2.push_back( "CCB:  TTCrx lock lost                                   ");
    TCounterName2.push_back( "CCB:  qPLL lock lost                                    "); 
    TCounterName2.push_back( "GTX:  Optical Receiver Error gtx_rx_err_count0          "); // 81 
    TCounterName2.push_back( "GTX:  Optical Receiver Error gtx_rx_err_count1          "); 
    TCounterName2.push_back( "GTX:  Optical Receiver Error gtx_rx_err_count2          "); 
    TCounterName2.push_back( "GTX:  Optical Receiver Error gtx_rx_err_count3          "); 
    TCounterName2.push_back( "GTX:  Optical Receiver Error gtx_rx_err_count4          "); 
    TCounterName2.push_back( "GTX:  Optical Receiver Error gtx_rx_err_count5          "); 
    TCounterName2.push_back( "GTX:  Optical Receiver Error gtx_rx_err_count6          "); // 87  last of real OTMB counters

    TCounterName2.push_back( "TMB:  CFEB Bad Bits Control                             ");     
    TCounterName2.push_back( "TMB:  CFEB Bad Bits Pattern 1                           "); // 89    
    TCounterName2.push_back( "TMB:  CFEB Bad Bits Pattern 2                           "); //    
    TCounterName2.push_back( "TMB:  CFEB Bad Bits Pattern 3                           "); //    
    TCounterName2.push_back( "TMB:  CFEB Bad Bits Pattern 4                           "); //    
    TCounterName2.push_back( "TMB:  CFEB Bad Bits Pattern 5                           "); //    

    TCounterName2.push_back( "TMB:  CFEB Bad Bits Pattern 6                           "); // 94   
    TCounterName2.push_back( "TMB:  CFEB Bad Bits Pattern 7                           "); //     
    TCounterName2.push_back( "TMB:  CFEB Bad Bits Pattern 8                           "); //   
    TCounterName2.push_back( "TMB:  CFEB Bad Bits Control extension                   "); //     
    TCounterName2.push_back( "TMB:  CFEB Bad Bits Pattern extension 1                 "); //     
    TCounterName2.push_back( "TMB:  CFEB Bad Bits Pattern extension 2                 "); //     
    TCounterName2.push_back( "TMB:  CFEB Bad Bits Pattern extension 3                 "); // 100     

    TCounterName2.push_back( "TMB:  Time since last Hard Reset                        "); // 101

    TOTAL_TMB_COUNTERS2=TCounterName2.size();

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
    OCounterName.push_back( "QP count ");  
    OCounterName.push_back( "RX count ");  // 10
    OCounterName.push_back( "QP error ");
    OCounterName.push_back( "RESET ");
    OCounterName.push_back( "MPC CSR0  "); 
    OCounterName.push_back( "MPC CSR4  "); 
    OCounterName.push_back( "MPC CSR7  ");
    OCounterName.push_back( "MPC CSR8  "); // 17

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
    LVCounterName.push_back( "Analog 7V  ");  // 19
    LVCounterName.push_back( "Digital 7V ");  // 20

    TECounterName.push_back( "DMB Temp  ");  // 0
    TECounterName.push_back( "CFEB1 Temp");  // 1
    TECounterName.push_back( "CFEB2 Temp");  // 
    TECounterName.push_back( "CFEB3 Temp");  //
    TECounterName.push_back( "CFEB4 Temp");  // 
    TECounterName.push_back( "CFEB5 Temp");  // 5
    TECounterName.push_back( "ALCT  Temp");  // 
    TECounterName.push_back( "TMB Temp  ");  // 7

    TVCounterName.push_back( "V 5.0 ");  // 0
    TVCounterName.push_back( "V 3.3 ");  //
    TVCounterName.push_back( "V 1.5Core ");  //
    TVCounterName.push_back( "V 1.5TT ");  // 3
    TVCounterName.push_back( "V 1.0TT ");  //
    TVCounterName.push_back( "I (5.0V)");  //
    TVCounterName.push_back( "I (3.3V)");  // 6
    TVCounterName.push_back( "I (1.5Core)");  //
    TVCounterName.push_back( "I (1.5TT) ");  //
    TVCounterName.push_back( "I (RAT) ");  // 9
    TVCounterName.push_back( "V (RAT) ");  //
    TVCounterName.push_back( "V REF  ");  //
    TVCounterName.push_back( "V GND  ");  // 12
    TVCounterName.push_back( "V MAX  ");  //

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
    IsErrCounter[31]=1;
    IsErrCounter[32]=1;
    IsErrCounter[35]=1;
    IsErrCounter[39]=1;
    IsErrCounter[40]=1;
    IsErrCounter[52]=1;
    IsErrCounter[55]=1;
    IsErrCounter[75]=1;
    IsErrCounter[76]=1;
    for(int i=0; i<9; i++) IsErrCounter[78+i]=1;

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
  std::string myUrl = getApplicationDescriptor()->getContextDescriptor()->getURL();
  std::string myUrn = getApplicationDescriptor()->getURN().c_str();
  main_url_ = myUrl + "/" + myUrn + "/MainPage";
  *out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl
       << "<?xml-stylesheet type=\"text/xml\" href=\"/emu/base/html/EmuPage1_XSL.xml\"?>" << std::endl
       << "<ForEmuPage1 application=\"" << getApplicationDescriptor()->getClassName()
       <<                   "\" url=\"" << getApplicationDescriptor()->getContextDescriptor()->getURL()
       <<         "\" localDateTime=\"" << getLocalDateTime() << "\">" << std::endl;

    *out << "  <monitorable name=\"" << "title"
         <<            "\" value=\"" << "PCrate Monitor " + (std::string)((endcap_side==1)?"Plus":"Minus")
         <<  "\" nameDescription=\"" << " "
         << "\" valueDescription=\"" << "click this to access the PCrate monitoring page"
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
         << "\" valueDescription=\"" << "should be non-zero when VME access is ON"
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
