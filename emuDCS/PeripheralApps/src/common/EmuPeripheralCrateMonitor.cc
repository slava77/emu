// $Id: EmuPeripheralCrateMonitor.cc

#include "EmuPeripheralCrateMonitor.h"

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <unistd.h> // for sleep()
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <time.h>

/////////////////////////////////////////////////////////////////////
// Instantiation and main page
/////////////////////////////////////////////////////////////////////
EmuPeripheralCrateMonitor::EmuPeripheralCrateMonitor(xdaq::ApplicationStub * s): EmuApplication(s)
{	
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
  xgi::bind(this,&EmuPeripheralCrateMonitor::CrateTMBCountersRight, "CrateTMBCountersRight");
  xgi::bind(this,&EmuPeripheralCrateMonitor::ResetAllCounters, "ResetAllCounters");
  xgi::bind(this,&EmuPeripheralCrateMonitor::FullResetTMBC, "FullResetTMBC");
  xgi::bind(this,&EmuPeripheralCrateMonitor::XmlOutput, "XmlOutput");
  xgi::bind(this,&EmuPeripheralCrateMonitor::DCSOutput, "DCSOutput");
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
  myParameter_ =  0;
  //
  xml_or_db = 0;  /* actual configuration source: 0: xml, 1: db */
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
  //
  this->getApplicationInfoSpace()->fireItemAvailable("XMLorDB", &XML_or_DB_);
  this->getApplicationInfoSpace()->fireItemAvailable("EmuConfigurationID", &EMU_config_ID_);
  this->getApplicationInfoSpace()->fireItemAvailable("runNumber", &runNumber_);
  this->getApplicationInfoSpace()->fireItemAvailable("xmlFileName", &xmlFile_);
  
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
  controller_checked_=false;
  this_tcounter_=0;
  this_dcounter_=0;
  this_ocounter_=0;

  parsed=0;
}

void EmuPeripheralCrateMonitor::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
    throw (toolbox::fsm::exception::Exception) {
  EmuApplication::stateChanged(fsm);
}

void EmuPeripheralCrateMonitor::dummyAction(toolbox::Event::Reference e)
    throw (toolbox::fsm::exception::Exception) {
  // currently do nothing
}
//

void EmuPeripheralCrateMonitor::MonitorStart(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
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
     fireEvent("Enable");
     this->Default(in,out);
}

void EmuPeripheralCrateMonitor::MonitorStop(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
     if(Monitor_On_)
     {
         PCsendCommand("MonitorStop","EmuPeripheralCrateBroadcast");
         Monitor_On_=false;
         std::cout << "Monitor stopped" << std::endl;
     }
     fireEvent("Halt");
     this->Default(in,out);
}

xoap::MessageReference EmuPeripheralCrateMonitor::onFastLoop (xoap::MessageReference message) 
  throw (xoap::exception::Exception) 
{
  // std::cout << "SOAP Fast Loop" << std::endl;
  PublishEmuInfospace(1);
  return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateMonitor::onSlowLoop (xoap::MessageReference message) 
  throw (xoap::exception::Exception) 
{
  // std::cout << "SOAP Slow Loop" << std::endl;
  PublishEmuInfospace(2);
  return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateMonitor::onExtraLoop (xoap::MessageReference message) 
  throw (xoap::exception::Exception) 
{
  // std::cout << "SOAP Extra Loop" << std::endl;
  PublishEmuInfospace(3);
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
        for ( unsigned int i = 0; i < crateVector.size(); i++ )
        {
                toolbox::net::URN urn = this->createQualifiedInfoSpace("EMu_"+(crateVector[i]->GetLabel())+"_PCrate");
                std::cout << "Crate " << i << " " << urn.toString() << std::endl;
                monitorables_.push_back(urn.toString());
                xdata::InfoSpace * is = xdata::getInfoSpaceFactory()->get(urn.toString());

            // for CCB, MPC, TTC etc.
                is->fireItemAvailable("CCB_CSRA1",new xdata::UnsignedShort(0));
                is->fireItemAvailable("CCB_CSRA2",new xdata::UnsignedShort(0));
                is->fireItemAvailable("CCB_CSRA3",new xdata::UnsignedShort(0));
                is->fireItemAvailable("TTC_BRSTR",new xdata::UnsignedShort(0));
                is->fireItemAvailable("TTC_DTSTR",new xdata::UnsignedShort(0));
                is->fireItemAvailable("QPLL",new xdata::String("uninitialized"));
                is->fireItemAvailable("CCBstatus",new xdata::String("uninitialized"));
                is->fireItemAvailable("MPCstatus",new xdata::String("uninitialized"));
                is->fireItemAvailable("CCBcounter",new xdata::Vector<xdata::UnsignedShort>());
                is->fireItemAvailable("CCBtime",new xdata::TimeVal);

            // for TMB fast counters
                is->fireItemAvailable("TMBcounter",new xdata::Vector<xdata::UnsignedInteger32>());
                is->fireItemAvailable("TMBftime",new xdata::TimeVal);
                is->fireItemAvailable("OTMBcounter",new xdata::Vector<xdata::UnsignedInteger32>());
                is->fireItemAvailable("OTMBftime",new xdata::TimeVal);

            // for DMB fast counters
                is->fireItemAvailable("DMBcounter",new xdata::Vector<xdata::UnsignedShort>());
                is->fireItemAvailable("DMBftime",new xdata::TimeVal);

            // for DCS temps, voltages
                is->fireItemAvailable("DCStemps",new xdata::Vector<xdata::UnsignedShort>());
                is->fireItemAvailable("DCSstime",new xdata::TimeVal);

         }
     Monitor_Ready_=true;
}

void EmuPeripheralCrateMonitor::PublishEmuInfospace(int cycle)
{
   //   cycle = 1  fast loop (e.g. TMB/DMB counters)
   //           2  slow loop (e.g. temps/voltages)
   //           3  extra loop (e.g. CCB MPC TTC status)

      emu::pc::Crate * now_crate;
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
          if(cycle==3 && now_crate && !(now_crate->IsAlive()))
          {
             bool cr = (now_crate->vmeController()->SelfTest()) && (now_crate->vmeController()->exist(13));
             now_crate->SetLife( cr );
          }
          if(now_crate && now_crate->IsAlive()) 
          {
             is = xdata::getInfoSpaceFactory()->get(monitorables_[i]);
             if(cycle==3)
             {  
                now_crate-> MonitorCCB(cycle, buf);
                if(buf2[0])  
                {   // buf2[0]==0 means no good data back
                   xdata::Vector<xdata::UnsignedShort> *ccbdata = dynamic_cast<xdata::Vector<xdata::UnsignedShort> *>(is->find("CCBcounter"));
                   if(ccbdata->size()==0) 
                      for(unsigned ii=0; ii<buf2[0]; ii++) ccbdata->push_back(0);
                   for(unsigned ii=0; ii<buf2[0]; ii++) (*ccbdata)[ii] = buf2[ii+1];

                   // CCB & TTC counters
                   counter16 = dynamic_cast<xdata::UnsignedShort *>(is->find("CCB_CSRA1"));
                   *counter16 = buf2[1];
                   counter16 = dynamic_cast<xdata::UnsignedShort *>(is->find("CCB_CSRA2"));
                   *counter16 = buf2[2];
                   counter16 = dynamic_cast<xdata::UnsignedShort *>(is->find("CCB_CSRA3"));
                   *counter16 = buf2[3];
                   counter16 = dynamic_cast<xdata::UnsignedShort *>(is->find("TTC_BRSTR"));
                   *counter16 = buf2[10];
                   counter16 = dynamic_cast<xdata::UnsignedShort *>(is->find("TTC_DTSTR"));
                   *counter16 = buf2[11];
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
             else if( cycle==2)
             {
                now_crate-> MonitorDCS(cycle, buf);
                if(buf2[0])
                {
                   // std::cout << "DCS counters " << buf2[0] << std::endl;
                   xdata::Vector<xdata::UnsignedShort> *dmbdata = dynamic_cast<xdata::Vector<xdata::UnsignedShort> *>(is->find("DCStemps"));
                   if(dmbdata->size()==0)
                      for(unsigned ii=0; ii<buf2[0]; ii++) dmbdata->push_back(0);
                   for(unsigned ii=0; ii<buf2[0]; ii++) (*dmbdata)[ii] = buf2[ii+1];
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
      }
}

void EmuPeripheralCrateMonitor::MainPage(xgi::Input * in, xgi::Output * out ) 
{
  //
  std::string LoggerName = getApplicationLogger().getName() ;
  std::cout << "Name of Logger is " <<  LoggerName <<std::endl;
  //
  LOG4CPLUS_INFO(getApplicationLogger(), "EmuPeripheralCrate ready");
  //
  MyHeader(in,out,"EmuPeripheralCrateMonitor");

  if(!parsed) ParsingXML();

  *out << "Total Crates : ";
  *out << total_crates_ << cgicc::br() << std::endl ;
  unsigned int active_crates=0;
  for(unsigned i=0; i<crateVector.size(); i++)
     if(crateVector[i]->IsAlive()) active_crates++;
  if( active_crates <= total_crates_) 
     *out << cgicc::b(" Active Crates: ") << active_crates << cgicc::br() << std::endl ;
 
     std::string CheckCrates = toolbox::toString("/%s/CheckCrates",getApplicationDescriptor()->getURN().c_str());
     *out << cgicc::form().set("method","GET").set("action",CheckCrates) << std::endl ;
     *out << cgicc::input().set("type","submit").set("value","Check Crate Controllers") << std::endl ;
     *out << cgicc::form() << std::endl ;

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
//    std::string CrateDMBCounters = toolbox::toString("/%s/CrateDMBCounters",getApplicationDescriptor()->getURN().c_str());
//    *out << cgicc::form().set("method","GET").set("action",CrateDMBCounters).set("target","_blank") << std::endl ;
//    *out << cgicc::input().set("type","submit").set("value","DMB counters").set("name","DMBCounters") << std::endl ;
//    *out << cgicc::form() << std::endl ;
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
  std::string myUrn = getApplicationDescriptor()->getURN().c_str();
  xgi::Utils::getPageHeader(out,title,myUrn,"","");
  //
}
//
void EmuPeripheralCrateMonitor::Default(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<"MainPage"<<"\">" <<std::endl;
}
//
/////////////////////////////////////////////////////////////////////
// SOAP Callback  
/////////////////////////////////////////////////////////////////////

xoap::MessageReference EmuPeripheralCrateMonitor::onMonitorStart (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  std::cout << "SOAP MonitorStart" << std::endl;
  //
  return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateMonitor::onMonitorStop (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  std::cout << "SOAP MonitorStop" << std::endl;
  //
  fireEvent("Halt");
  //
  return createReply(message);
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
    MyController = new emu::pc::EmuController();

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

    if(!Monitor_Ready_) return;

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
  std::vector<emu::pc::TMB*> myVector;
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

        if(crateVector[idx]->IsAlive()) *out << "On ";
        else * out << "Off";
        *out <<cgicc::td();
        *out <<cgicc::td();
      }
      // chamber name
      *out << crateVector[idx]->GetChamber(myVector[tmb])->GetLabel() << cgicc::br();
      // ALCT: LCT sent to TMB
      int dc=myVector[tmb]->GetCounter(1);
      if (dc == 0x3fffffff ) dc = -1;
      *out << "A: " << dc <<"; ";
      // CLCT pretrigger 
      dc=myVector[tmb]->GetCounter(5);
      if (dc == 0x3fffffff ) dc = -1;
      *out << "C: " << dc << cgicc::br();
      // trig allowed, xmit to MPC
      dc=myVector[tmb]->GetCounter(28);
      if (dc == 0x3fffffff ) dc = -1;
      *out << "T: " << dc << "; ";
      // L1A: TMB triggered, TMB in L1A window
      dc=myVector[tmb]->GetCounter(34);
      if (dc == 0x3fffffff ) dc = -1;
      *out << "L: " << dc << cgicc::br();
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
     unsigned TOTAL_COUNTS=9;

    if(!Monitor_Ready_) return;

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
    unsigned short csra1,csra2,csra3,csrm0,brstr,dtstr;
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
    *out << cgicc::b(TCounterName[this_tcounter_]) << std::endl;

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
  std::vector<emu::pc::TMB*> myVector;
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

        if(crateVector[idx]->IsAlive()) *out << "On ";
        else * out << "Off";
        *out <<cgicc::td();
        *out <<cgicc::td();
      }
      if ( myVector[tmb]->GetCounter(this_tcounter_) == 0x3fffffff )
         *out << "-1";
      else 
   	 *out << myVector[tmb]->GetCounter(this_tcounter_);
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
    *out << cgicc::b(DCounterName[this_dcounter_]) << std::endl;

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
  std::vector<emu::pc::DAQMB*> myVector;
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
void EmuPeripheralCrateMonitor::CrateTMBCountersRight(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  //  int counter_idx[30]={ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
  //                       10,11,12,16,17,18,19, 20,21,22,
  //                       23,24,33};
  int counter_idx[23]={ 0 ,  2,  3,  4,  5,  8,  9, 10, 12, 13,
			16, 25, 26, 27, 28, 30, 33, 34, 36, 39,
			40, 47, 48};

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
  emu::pc::Crate *myCrate;
  std::vector<emu::pc::TMB*> myVector;
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
  for (int idx=0; idx<23; idx++) {
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
         output << "-1";
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
  emu::pc::Crate *myCrate;
  std::vector<emu::pc::DAQMB*> myVector;
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
  std::vector<emu::pc::TMB*> myVector;
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
  std::vector<emu::pc::TMB*> myVector;

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

  unsigned int TOTAL_TMB_COUNTERS=49;
  std::vector<emu::pc::TMB*> myVector;
  int o_value, n_value, i_value;
  xdata::InfoSpace * is;

  if(!Monitor_Ready_) return;
  //
  *out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
  *out << "<?xml-stylesheet type=\"text/xml\" href=\"/tmp/counterMonitor/counterMonitor_XSL.xml\"?>" << std::endl;
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
        *out << "\" lct=\"";
//        *out << myVector[j]->GetCounter(13);
        o_value = (*otmbdata)[j*TOTAL_TMB_COUNTERS+13];
        if(o_value == 0x3FFFFFFF || o_value <0) o_value = -1;
        n_value = (*tmbdata)[j*TOTAL_TMB_COUNTERS+13];
        if(n_value == 0x3FFFFFFF || n_value <0) n_value = -1;
        // when a counter has error, should be -1, but the XML displayer needs
        //  non-negative number, so set it to 0 here and in the following:
        i_value = ((o_value>=0 && n_value>=0)?(n_value-o_value):(0));
        if(i_value<-1) i_value=0;
        *out << i_value;
        *out << "\" l1a=\"";
//        *out << myVector[j]->GetCounter(34);
        o_value = (*otmbdata)[j*TOTAL_TMB_COUNTERS+34];
        if(o_value == 0x3FFFFFFF || o_value <0) o_value = -1;
        n_value = (*tmbdata)[j*TOTAL_TMB_COUNTERS+34];
        if(n_value == 0x3FFFFFFF || n_value <0) n_value = -1;
        // counter error, set it to 0:
        i_value = ((o_value>=0 && n_value>=0)?(n_value-o_value):(0));
        if(i_value<-1) i_value=0;
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
        *out << "\" lct=\"";
//        *out << myVector[j]->GetCounter(13);
        n_value = (*tmbdata)[j*TOTAL_TMB_COUNTERS+13];
        // counter error, set it to 0 here:
        if(n_value == 0x3FFFFFFF || n_value <0) n_value = 0;
        *out << n_value;
        *out << "\" l1a=\"";
//        *out << myVector[j]->GetCounter(34);
        n_value = (*tmbdata)[j*TOTAL_TMB_COUNTERS+34];
        // counter error, set it to 0 here:
        if(n_value == 0x3FFFFFFF || n_value <0) n_value = 0;
        *out << n_value;
        *out << "\"/>" << std::endl;
     }
  }

  *out << "  </sample>" << std::endl;
  *out << "</emuCounters>" << std::endl;
}

void EmuPeripheralCrateMonitor::DCSOutput(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  *out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
  *out << "<?xml-stylesheet type=\"text/xml\" href=\"counterMonitor.xsl\"?>" << std::endl;
  *out << "<emuCounters dateTime=\"2008-08-25 16:34:56\">" << std::endl;
  *out << "  <sample name=\"cumulative\" delta_t=\"500\">" << std::endl;

  std::vector<emu::pc::TMB*> myVector;
  for ( unsigned int i = 0; i < crateVector.size(); i++ )
  {
     myVector = crateVector[i]->tmbs();
     for(unsigned int j=0; j<myVector.size(); j++) 
     {
        *out << "    <count chamber=\"";
        *out << crateVector[i]->GetChamber(myVector[j])->GetLabel();
        *out << "\" alct=\"";
        *out << myVector[j]->GetCounter(1);
        *out << "\" clct=\"";
        *out << myVector[j]->GetCounter(5);
        *out << "\" l1a=\"";
        *out << myVector[j]->GetCounter(34);
        *out << "\"/>" << std::endl;
     }
  }

  *out << "  <sample>" << std::endl;
  *out << "<emuCounters>" << std::endl;
}


void EmuPeripheralCrateMonitor::BeamView(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {

  unsigned int TOTAL_TMB_COUNTERS=49;
  std::vector<emu::pc::TMB*> myVector;
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

        o_value = (*otmbdata)[j*TOTAL_TMB_COUNTERS+13];
        if(o_value == 0x3FFFFFFF || o_value <0) o_value = -1;
        n_value = (*tmbdata)[j*TOTAL_TMB_COUNTERS+13];
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

void EmuPeripheralCrateMonitor::CrateStatus(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  MyHeader(in,out,"Crate Status");
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
  int mycrate;
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
  *out << cgicc::h3("Configuration done for Crate  ");
  *out << cgicc::br();
  //
  xdata::InfoSpace * is = xdata::getInfoSpaceFactory()->get(monitorables_[mycrate]);

  xdata::UnsignedShort *counter16;
  unsigned short csra1,csra2,csra3;
  counter16 = dynamic_cast<xdata::UnsignedShort *>(is->find("CCB_CSRA1"));
  csra1=(unsigned short)  *counter16;
  counter16 = dynamic_cast<xdata::UnsignedShort *>(is->find("CCB_CSRA2"));
  csra2=(unsigned short)  *counter16;
  counter16 = dynamic_cast<xdata::UnsignedShort *>(is->find("CCB_CSRA3"));
  csra3=(unsigned short)  *counter16;

 //  int ccbmode = (thisCCB->ReadRegister(0x0))&0x1;
  //
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
  *out << cgicc::fieldset();
  //
  // int read = (thisCCB->ReadRegister(0x4))&0xffff;
  //
  //
  *out << cgicc::fieldset().set("style","font-size: 10pt; font-family: arial;");
  *out << "CCB  slot = 13 FPGA cfg       " << ((csra3>>12)&0x1);
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
  *out << cgicc::span().set("style","color:blue");
  *out << "MPC slot = 12 cfg             " << (csra2&0x1);
  *out << cgicc::span();
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

void EmuPeripheralCrateMonitor::actionPerformed (xdata::Event& e) {
  //
}

  void EmuPeripheralCrateMonitor::CheckCrates(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
  {  
    std::cout << "Button: Check Crate Controllers" << std::endl;
    CheckControllers();
    this->Default(in, out);
  }

void EmuPeripheralCrateMonitor::CheckControllers()
{
    if(total_crates_<=0) return;
    bool cr;
    for(unsigned i=0; i< crateVector.size(); i++)
    {
        cr = (crateVector[i]->vmeController()->SelfTest()) && (crateVector[i]->vmeController()->exist(13));
        crateVector[i]->SetLife( cr );
        if(!cr) std::cout << "Exclude Crate " << crateVector[i]->GetLabel() << std::endl;
    }
    controller_checked_ = true;
}

void EmuPeripheralCrateMonitor::InitCounterNames()
{
    TCounterName.clear();
    DCounterName.clear();
    OCounterName.clear();    
    //
    //
    TCounterName.push_back( "ALCT: alct0 valid pattern flag received                 "); //0
    //
    TCounterName.push_back( "ALCT: alct1 valid pattern flag received                 ");
    TCounterName.push_back( "ALCT: alct data structure error                         "); //2
    TCounterName.push_back( "ALCT: raw hits readout                                  "); //3
    TCounterName.push_back( "ALCT: raw hits readout - CRC error                      "); //4
    TCounterName.push_back( "CLCT: Pretrigger                                        "); //5
    //
    TCounterName.push_back( "CLCT: Pretrigger discarded - no write buffer available  ");
    TCounterName.push_back( "CLCT: Pretrigger discarded - no alct in window          ");
    TCounterName.push_back( "CLCT: CLCT discarded, clct0 invalid pattern             "); //8
    TCounterName.push_back( "CLCT: Bx pretrigger machine waited for triads           "); //9
    TCounterName.push_back( "CLCT: clct0 sent to TMB matching section                "); //10
    //
    TCounterName.push_back( "CLCT: clct1 sent to TMB matching section                ");
    TCounterName.push_back( "TMB:  TMB accepted alct*clct, alct-only, or clct-only   "); //12
    TCounterName.push_back( "TMB:  TMB clct*alct matched trigger                     "); //13
    TCounterName.push_back( "TMB:  TMB alct-only trigger                             ");
    TCounterName.push_back( "TMB:  TMB clct-only trigger                             ");
    //
    TCounterName.push_back( "TMB:  TMB matching rejected event                       "); //16
    TCounterName.push_back( "TMB:  TMB matching discarded an ALCT                    ");
    TCounterName.push_back( "TMB:  TMB matching discarded a CLCT                     ");
    TCounterName.push_back( "TMB:  Matching found no ALCT                            ");
    TCounterName.push_back( "TMB:  Matching found no CLCT                            ");
    //
    TCounterName.push_back( "TMB:  Matching found one ALCT                           ");
    TCounterName.push_back( "TMB:  Matching found one CLCT                           ");
    TCounterName.push_back( "TMB:  Matching found two ALCTs                          ");
    TCounterName.push_back( "TMB:  Matching found two CLCTs                          ");
    TCounterName.push_back( "TMB:  ALCT0 copied into ALCT1 to make 2nd LCT           "); //25
    //
    TCounterName.push_back( "TMB:  CLCT0 copied into CLCT1 to make 2nd LCT           "); //26
    TCounterName.push_back( "TMB:  LCT1 has higher quality than LCT0 (ranking error) "); //27
    TCounterName.push_back( "TMB:  Transmitted LCT0 to MPC                           "); //28
    TCounterName.push_back( "TMB:  Transmitted LCT1 to MPC                           ");
    TCounterName.push_back( "TMB:  MPC accepted LCT0                                 "); //30
    //
    TCounterName.push_back( "TMB:  MPC accepted LCT1                                 ");
    TCounterName.push_back( "TMB:  MPC rejected both LCT0 and LCT1                   ");
    TCounterName.push_back( "L1A:  L1A received                                      "); //33
    TCounterName.push_back( "L1A:  L1A received, TMB in L1A window                   "); //34
    TCounterName.push_back( "L1A:  L1A received, no TMB in window                    ");
    //
    TCounterName.push_back( "L1A:  TMB triggered, no L1A in window                   "); //36
    TCounterName.push_back( "L1A:  TMB readouts completed                            ");  
    TCounterName.push_back( "STAT: CLCT Triads skipped                               "); 
    TCounterName.push_back( "STAT: Raw hits buffer had to be reset                   "); //39
    TCounterName.push_back( "STAT: TTC Resyncs received                              "); //40
    //
    TCounterName.push_back( "HDR:  Pretrigger counter                                ");
    TCounterName.push_back( "HDR:  CLCT counter                                      ");
    TCounterName.push_back( "HDR:  TMB trigger counter                               ");
    TCounterName.push_back( "HDR:  ALCTs received counter                            ");
    TCounterName.push_back( "HDR:  L1As received counter (12 bits)                   ");
    //
    TCounterName.push_back( "HDR:  Readout counter (12 bits)                         ");
    TCounterName.push_back( "HDR:  Orbit counter                                     "); //47
    TCounterName.push_back( "TMB: Time since last Hard Reset                         "); //48
    //
    //    TCounterName.push_back( "ALCT: CRC error                                        "); // 0
    //    TCounterName.push_back( "ALCT: LCT sent to TMB                                  ");
    //    TCounterName.push_back( "ALCT: LCT received data error                          ");
    //    TCounterName.push_back( "ALCT: L1A readout                                      ");
    //    TCounterName.push_back( "CLCT: Pretrigger                                       ");
    //    TCounterName.push_back( "CLCT: Pretrig but no wbuf available                    ");
    //    TCounterName.push_back( "CLCT: Invalid pattern after drift                      ");
    //    TCounterName.push_back( "CLCT: TMB matching rejected event                      ");
    //    TCounterName.push_back( "TMB: CLCT,ALCT,or both trigger                         ");
    //    TCounterName.push_back( "TMB: CLCT,ALCT,or both trigger, trig allowed, xmit MPC ");
    //    TCounterName.push_back( "TMB: CLCT and ALCT matched in time                     "); // 10
    //    TCounterName.push_back( "TMB: ALCT-only trigger                                 ");
    //    TCounterName.push_back( "TMB: CLCT-only trigger                                 ");
    //    TCounterName.push_back( "TMB: No trig pulse response (TMB internal logic check) ");
    //    TCounterName.push_back( "TMB: No MPC transmission (TMB internal logic check)    ");
    //    TCounterName.push_back( "TMB: No MPC response FF pulse (TMB internal logic ck)  ");
    //    TCounterName.push_back( "TMB: MPC accepted LCT0                                 ");
    //    TCounterName.push_back( "TMB: MPC accepted LCT1                                 ");
    //    TCounterName.push_back( "L1A: L1A received                                      ");
    //    TCounterName.push_back( "L1A: TMB triggered, TMB in L1A window                  ");
    //    TCounterName.push_back( "L1A: L1A received, no TMB in window                    "); // 20
    //    TCounterName.push_back( "L1A: TMB triggered, no L1A received                    ");
    //    TCounterName.push_back( "L1A: TMB readout                                       ");
    //    TCounterName.push_back( "CLCT: Triad skipped                                    ");
    //    TCounterName.push_back( "TMB: Raw Hits Buffer Reset due to overflow             ");
    //    TCounterName.push_back( "TMB: No ALCT in trigger                                ");
    //    TCounterName.push_back( "TMB: One ALCT in trigger                               ");
    //    TCounterName.push_back( "TMB: One CLCT in trigger                               ");
    //    TCounterName.push_back( "TMB: Two ALCTs in trigger                              ");
    //    TCounterName.push_back( "TMB: Two CLCTs in trigger                              ");
    //    TCounterName.push_back( "TMB: ALCT0 copied to ALCT1 to make 2nd LCT             "); // 30
    //    TCounterName.push_back( "TMB: CLCT0 copied to CLCT1 to make 2nd LCT             ");
    //    TCounterName.push_back( "TMB: LCT1 has higher quality than LCT0 (ranking error) ");
    //    TCounterName.push_back( "TMB: Time since last Hard Reset                        "); // 33

    DCounterName.push_back( "L1A to LCT delay");  // 0
    DCounterName.push_back( "CFEB DAV delay  ");
    DCounterName.push_back( "TMB DAV delay   ");
    DCounterName.push_back( "ALCT DAV delay  ");
    DCounterName.push_back( "CFEB DAV Scope  ");  // 4
    DCounterName.push_back( "TMB DAV Scope   ");
    DCounterName.push_back( "ALCT DAV Scope  ");
    DCounterName.push_back( "ACTIVE DAV Scope");
    DCounterName.push_back( "L1A to LCT Scope");  // 8

    OCounterName.push_back( "CCB mode  "); // 0
    OCounterName.push_back( "TTCrx     ");
    OCounterName.push_back( "QPLL      ");
    OCounterName.push_back( "MPC       ");
    OCounterName.push_back( "CCB CSRA1 ");
    OCounterName.push_back( "CCB CSRA2 ");
    OCounterName.push_back( "CCB CSRA3 ");
    OCounterName.push_back( "TTC BRSTR ");
    OCounterName.push_back( "TTC DTSTR ");
    OCounterName.push_back( "MPC CSR0  ");
    OCounterName.push_back( "MPC CSR4  "); // 10
    OCounterName.push_back( "MPC CSR7  ");
    OCounterName.push_back( "MPC CSR8  "); // 12
}

// sending and receiving soap commands
////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateMonitor::PCsendCommand(std::string command, std::string klass)
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

xoap::MessageReference EmuPeripheralCrateMonitor::PCcreateCommandSOAP(std::string command) {
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
XDAQ_INSTANTIATOR_IMPL(EmuPeripheralCrateMonitor)
//
