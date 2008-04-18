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

using namespace cgicc;
using namespace std;

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
  xgi::bind(this,&EmuPeripheralCrateMonitor::CrateStatus, "CrateStatus");
  xgi::bind(this,&EmuPeripheralCrateMonitor::CrateSelection, "CrateSelection");
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
  //-------------------------------------------------------------
  // fsm_ is defined in EmuApplication
  //-------------------------------------------------------------
  //----------------------------
  // initialize variables
  //----------------------------
  myParameter_ =  0;
  //
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

  parsed=0;
}

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

            // for TMB fast counters
                is->fireItemAvailable("TMBcounter",new xdata::Vector<xdata::UnsignedInteger32>);
                is->fireItemAvailable("TMBtime",new xdata::TimeVal);

            // for DMB fast counters
                is->fireItemAvailable("DMBcounter",new xdata::Vector<xdata::UnsignedShort>);
                is->fireItemAvailable("DMBtime",new xdata::TimeVal);

            // for TMB temps, voltages

            // for DMB temps, voltages

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
          if(now_crate && now_crate->IsAlive()) 
          {
             if(!(now_crate->vmeController()->SelfTest())) continue;
             is = xdata::getInfoSpaceFactory()->get(monitorables_[i]);
             if(cycle==3)
             {  
                now_crate-> MonitorCCB(cycle, buf);
                if(buf[0])  
                {   // buf[0]==0 means no good data back

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
             else if( cycle==1)
             {
                now_crate-> MonitorTMB(cycle, buf);
                if(buf[0])
                {
                  // std::cout << "TMB counters will be here" << std::endl;
                }
                now_crate-> MonitorDMB(cycle, buf);
                if(buf[0])
                {
                  // std::cout << "DMB counters will be here" << std::endl;
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
  *out << cgicc::br() << cgicc::hr() <<std::endl;

  *out << cgicc::h2("Crate View")<< std::endl;
  //
  *out << cgicc::span().set("style","color:blue");
  *out << cgicc::b(cgicc::i("Current Crate : ")) ;
  *out << ThisCrateID_ << cgicc::span() << std::endl ;
  //
  *out << cgicc::br();
  //

  // Begin select crate
        // Config listbox
	*out << form().set("action",
			"/" + getApplicationDescriptor()->getURN() + "/CrateSelection") << endl;

	int n_keys = crateVector.size();

	*out << "Choose Crate: " << endl;
	*out << cgicc::select().set("name", "runtype") << endl;

	int selected_index = this_crate_no_;
        std::string CrateName;
	for (int i = 0; i < n_keys; ++i) {
                if(crateVector[i]->IsAlive())
                   CrateName = crateVector[i]->GetLabel();
                else
                   CrateName = crateVector[i]->GetLabel() + " NG";
		if (i == selected_index) {
			*out << option()
					.set("value", CrateName)
					.set("selected", "");
		} else {
			*out << option()
					.set("value", CrateName);
		}
		*out << CrateName << option() << endl;
	}

	*out << cgicc::select() << endl;

	*out << input().set("type", "submit")
			.set("name", "command")
			.set("value", "CrateSelection") << endl;
	*out << form() << endl;
     
  //End select crate
 
  *out << cgicc::br()<< std::endl;
  std::cout << "Main Page: "<< std::dec << total_crates_ << " Crates" << std::endl;
  //
  if(Monitor_Ready_)
  {
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial; background-color:yellow");
    *out << std::endl;
    *out << cgicc::legend((("Monitoring"))).set("style","color:blue") ;
    //
    *out << cgicc::table().set("border","0");
    //
    *out << cgicc::td();
    //
    std::string ResetTMBC = toolbox::toString("/%s/ResetAllCounters",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",ResetTMBC) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Reset TMB Counters").set("name","ResetTMBC") << std::endl ;
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
  *out << cgicc::br() << cgicc::hr() <<std::endl;

  *out << cgicc::h2("Counter View")<< std::endl;
  //

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
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<"MainPage"<<"\">" <<endl;
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

     string value = cgi.getElement("runtype")->getValue(); 
     std::cout << "Select Crate " << value << endl;
     if(!value.empty())
     {
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
    if(xmlFile_.toString().find("http") == string::npos) 
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

///////////////////////////////////////////////////////
// Counters displays
///////////////////////////////////////////////////////
void EmuPeripheralCrateMonitor::CrateTMBCountersRight(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  ostringstream output;
  output << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eFrames) << std::endl;
  output << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  //
  cgicc::CgiEnvironment cgiEnvi(in);
  std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
  //
    *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"3; URL=/"
	 <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<endl;
  //
  Page=cgiEnvi.getQueryString();
  std::string crate_name=Page.substr(0,Page.find("=", 0) );
  *out << cgicc::b("Crate: "+crate_name) << std::endl;
  vector<TMB*> myVector;
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
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"5; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<endl;
  //
  Page=cgiEnvi.getQueryString();
  std::string crate_name=Page.substr(0,Page.find("=", 0) );
  *out << cgicc::b("Crate: "+crate_name) << std::endl;
  vector<DAQMB*> myVector;
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
void EmuPeripheralCrateMonitor::ResetAllCounters(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  for(unsigned int i=0; i<tmbVector.size(); i++) 
  {
    tmbVector[i]->ResetCounters();
  }

  this->Default(in,out);
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
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"300; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<endl;
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

// sending and receiving soap commands
////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateMonitor::PCsendCommand(string command, string klass)
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

xoap::MessageReference EmuPeripheralCrateMonitor::PCcreateCommandSOAP(string command) {
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
