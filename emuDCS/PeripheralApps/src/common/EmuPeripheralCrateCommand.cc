// $Id: EmuPeripheralCrateCommand.cc

#include "emu/pc/EmuPeripheralCrateCommand.h"

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <unistd.h> // for sleep()
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <time.h>
// for xml parser (Madorsky)
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

//using namespace cgicc;
//using namespace std;

namespace emu {
  namespace pc {

/////////////////////////////////////////////////////////////////////
// Instantiation and main page
/////////////////////////////////////////////////////////////////////
EmuPeripheralCrateCommand::EmuPeripheralCrateCommand(xdaq::ApplicationStub * s): EmuPeripheralCrateBase(s)
{	
  //
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
  all_crates_ok = -1;
  for (int i=0; i<60; i++) {
    crate_check_ok[i] = -1;
    ccb_check_ok[i] = -1;
    mpc_check_ok[i] = -1;
    for (int j=0; j<9; j++) {
      alct_check_ok[i][j] = -1;
      tmb_check_ok[i][j] = -1;
      dmb_check_ok[i][j] = -1;
    }
  }
  //
  xgi::bind(this,&EmuPeripheralCrateCommand::Default, "Default");
  xgi::bind(this,&EmuPeripheralCrateCommand::MainPage, "MainPage");
  //
  //------------------------------------------------------
  // bind buttons -> other pages
  //------------------------------------------------------
  xgi::bind(this,&EmuPeripheralCrateCommand::CheckCrates, "CheckCrates");
  xgi::bind(this,&EmuPeripheralCrateCommand::CheckCratesConfiguration, "CheckCratesConfiguration");

  // SOAP call-back functions, which relays to *Action method.
  //-----------------------------------------------------------
  xoap::bind(this, &EmuPeripheralCrateCommand::onConfigure, "Configure", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateCommand::onEnable,    "Enable",    XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateCommand::onDisable,   "Disable",   XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateCommand::onHalt,      "Halt",      XDAQ_NS_URI);
// cfeb calibration
  xoap::bind(this, &EmuPeripheralCrateCommand::onConfigCalCFEB, "ConfigCalCFEB", XDAQ_NS_URI);
// alct calib commands (Madorsky)
  xoap::bind(this, &EmuPeripheralCrateCommand::onConfigCalALCT, "ConfigCalALCT", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateCommand::onEnableCalALCTConnectivity, "EnableCalALCTConnectivity", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateCommand::onEnableCalALCTThresholds, "EnableCalALCTThresholds", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateCommand::onEnableCalALCTDelays, "EnableCalALCTDelays", XDAQ_NS_URI);
// ---
  //
  //-------------------------------------------------------------
  // fsm_ is defined in EmuApplication
  //-------------------------------------------------------------
  fsm_.addState('H', "Halted",     this, &EmuPeripheralCrateCommand::stateChanged);
  fsm_.addState('C', "Configured", this, &EmuPeripheralCrateCommand::stateChanged);
  fsm_.addState('E', "Enabled",    this, &EmuPeripheralCrateCommand::stateChanged);
  //
  fsm_.addStateTransition('H', 'C', "Configure", this, &EmuPeripheralCrateCommand::configureAction);
  fsm_.addStateTransition('C', 'C', "Configure", this, &EmuPeripheralCrateCommand::reConfigureAction);
  fsm_.addStateTransition('E', 'E', "Configure", this, &EmuPeripheralCrateCommand::reConfigureAction); // invalid, do nothing

  fsm_.addStateTransition('H', 'H', "Enable",    this, &EmuPeripheralCrateCommand::enableAction); // invalid, do nothing
  fsm_.addStateTransition('C', 'E', "Enable",    this, &EmuPeripheralCrateCommand::enableAction);
  fsm_.addStateTransition('E', 'E', "Enable",    this, &EmuPeripheralCrateCommand::enableAction);

  fsm_.addStateTransition('H', 'H', "Disable",   this, &EmuPeripheralCrateCommand::disableAction); // invalid, do nothing
  fsm_.addStateTransition('C', 'C', "Disable",   this, &EmuPeripheralCrateCommand::disableAction);
  fsm_.addStateTransition('E', 'C', "Disable",   this, &EmuPeripheralCrateCommand::disableAction);

  fsm_.addStateTransition('H', 'H', "Halt",      this, &EmuPeripheralCrateCommand::haltAction);
  fsm_.addStateTransition('C', 'H', "Halt",      this, &EmuPeripheralCrateCommand::haltAction);
  fsm_.addStateTransition('E', 'H', "Halt",      this, &EmuPeripheralCrateCommand::haltAction);
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
  xml_or_db = -1;  /* actual configuration source: 0: xml, 1: db , -1: unknown or error */
  XML_or_DB_ = "xml";
  EMU_config_ID_ = "1000002";
  xmlFile_ = "config.xml" ;
  GlobalRun_=0;   
  //
  CCBRegisterValue_ = -1;
  Operator_ = "Operator";
  RunNumber_= "-1";
  CalibrationState_ = "None";
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
  this->getApplicationInfoSpace()->fireItemAvailable("InGlobalRun", &GlobalRun_);
  
  // for XMAS minotoring:

  Monitor_On_ = false;
  Monitor_Ready_ = false;

  ccb_checked_ = false;
  controller_checked_ = false;
  global_config_states[0]="UnConfigured";
  global_config_states[2]="Configuring";
  global_config_states[1]="Configured";
  global_run_states[0]="Halted";
  global_run_states[1]="Enabled";
  current_config_state_=0;
  current_run_state_=0;
  total_crates_=0;
  this_crate_no_=0;

  parsed=0;

}

void EmuPeripheralCrateCommand::MainPage(xgi::Input * in, xgi::Output * out ) 
{
  //
  std::string LoggerName = getApplicationLogger().getName() ;
  std::cout << "Name of Logger is " <<  LoggerName <<std::endl;
  //
  LOG4CPLUS_INFO(getApplicationLogger(), "EmuPeripheralCrate ready");
  //
  MyHeader(in,out,"EmuPeripheralCrateCommand");

  if(!parsed) ParsingXML();

  *out << "Total Crates : ";
  *out << total_crates_ << cgicc::br() << std::endl ;
  unsigned int active_crates=0;
  for(unsigned i=0; i<crateVector.size(); i++)
     if(crateVector[i]->IsAlive()) active_crates++;
  if( active_crates <= total_crates_) 
     *out << cgicc::b(" Active Crates: ") << active_crates << cgicc::br() << std::endl ;
 
 // Crate Status
  *out << cgicc::span().set("style","color:blue");
  *out << cgicc::b(cgicc::i("System Status: ")) ;
  *out << global_config_states[current_config_state_] << "  ";
  *out << global_run_states[current_run_state_]<< cgicc::br() << std::endl ;
  *out << cgicc::span() << std::endl ;
  //
  if(current_run_state_==0)
  {
     *out << cgicc::table().set("border","0");
     //
     *out << cgicc::td();
     std::string CheckCrates = toolbox::toString("/%s/CheckCrates",getApplicationDescriptor()->getURN().c_str());
     *out << cgicc::form().set("method","GET").set("action",CheckCrates) << std::endl ;
     *out << cgicc::input().set("type","submit").set("value","Check Crate Controllers") << std::endl ;
     *out << cgicc::form() << std::endl ;
     *out << cgicc::td();

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

     *out << cgicc::table();
  }
  //
  int initial_crate = current_crate_;
  //
  if (all_crates_ok >= 0) {
    //
    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      //
      SetCurrentCrate(crate_number);
      //
      if (crate_check_ok[current_crate_] == 0) {
	//
	*out << cgicc::span().set("style","color:red");
	//
        if (ccb_check_ok[current_crate_] == 0) *out << thisCrate->GetLabel() << "   CCB   " << cgicc::br() << std::endl ;
        if (mpc_check_ok[current_crate_] == 0) *out << thisCrate->GetLabel() << "   MPC   " << cgicc::br() << std::endl ;
        //
	bool alct_ok = true;
	bool tmb_ok = true;
	bool dmb_ok = true;
	//
	for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
	  if (alct_check_ok[current_crate_][chamber_index] == 0) alct_ok = false;
	  if (tmb_check_ok[current_crate_][chamber_index] == 0)  tmb_ok = false;
	  if (dmb_check_ok[current_crate_][chamber_index] == 0)  dmb_ok = false;
	}
	//
	if (!alct_ok) {
	  //
	  for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) 
	    if (alct_check_ok[current_crate_][chamber_index] == 0) 
	      *out << thisCrate->GetLabel() << "   "
		   << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetLabel().c_str() 
		   << "   ALCT   " << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetProblemDescription() 
		   << cgicc::br();
	}
	//
	if (!tmb_ok) {
	  //
	  for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) 
	    if (tmb_check_ok[current_crate_][chamber_index] == 0) 
	      *out << thisCrate->GetLabel() << "    "
		   << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetLabel().c_str() 
		   << "   TMB   " << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetProblemDescription() 
		   << cgicc::br();
	}
	//
	if (!dmb_ok) {
	  //
	  for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) 
	    if (dmb_check_ok[current_crate_][chamber_index] == 0) 
	      *out << thisCrate->GetLabel() << "    "
		   << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetLabel().c_str() 
		   << "   DMB   " << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetProblemDescription() 
		   << cgicc::br();
	}
	//
      } else if (crate_check_ok[current_crate_] == 1) {
	//
	*out << cgicc::span().set("style","color:green");
	*out << crateVector[crate_number]->GetLabel() << "   OK" << cgicc::br();
      } else if (crate_check_ok[current_crate_] == -1) {
	//
	*out << cgicc::span().set("style","color:blue");
	*out << crateVector[crate_number]->GetLabel() << " Not checked" << cgicc::br();
      }
      *out << cgicc::span() << std::endl ;
    }
  }
  //
  SetCurrentCrate(initial_crate);
  //

  *out << cgicc::br() << cgicc::br() << std::endl; 
  if(xml_or_db==0)
  {
     *out << cgicc::b(cgicc::i("Configuration filename : ")) ;
     *out << xmlFile_.toString() << cgicc::br() << std::endl ;
  }
  else if(xml_or_db==1)
  {
    *out << cgicc::b(cgicc::i("TStore EMU_config_ID : ")) ;
    *out << EMU_config_ID_.toString() << cgicc::br() << std::endl ;
  }

  //
  //
}

// 
void EmuPeripheralCrateCommand::MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) 
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
void EmuPeripheralCrateCommand::Default(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<"MainPage"<<"\">" <<std::endl;
}
//
/////////////////////////////////////////////////////////////////////
// SOAP Callback  
/////////////////////////////////////////////////////////////////////
//
xoap::MessageReference EmuPeripheralCrateCommand::onConfigure (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  std::cout << "SOAP Configure" << std::endl;
  //
  current_config_state_=2;
  fireEvent("Configure");
  //
  if(!parsed) ParsingXML();

//  current_config_state_=(GlobalRun_?1:VerifyCratesConfiguration());
//
// Liu Sept. 12, 2012:  Only do CCB check on the first Configure after initial
//
  if(!ccb_checked_)
  {
     current_config_state_=VerifyCCBs();
     ccb_checked_ = true;
  }
  //
  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateCommand::onEnable (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  std::cout << "SOAP Enable" << std::endl;
  //
  current_run_state_ = 1;
  fireEvent("Enable");
  //
/*  comment out the following line until we find a better solution.
    If too many crates are off, the total time (due to time-out) will be longer than 30 seconds
    and the state transition will fail.
*/    
//  ResetAllTMBCounters();

  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateCommand::onDisable (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  std::cout << "SOAP Disable" << std::endl;
  //
  current_run_state_ = 0;
  fireEvent("Disable");
  //
  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateCommand::onHalt (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  std::cout << "SOAP Halt" << std::endl;
  //
  current_run_state_ = 0;
  fireEvent("Halt");
  //
  return createReply(message);
}
//
void EmuPeripheralCrateCommand::configureAction(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {
  //
  // currently do nothing
  //
  LOG4CPLUS_INFO(getApplicationLogger(), "Configure");
  //
}
//
void EmuPeripheralCrateCommand::configureFail(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {
  //
  // currently do nothing
  //
  LOG4CPLUS_INFO(getApplicationLogger(), "Failed");
  //
}
//
void EmuPeripheralCrateCommand::reConfigureAction(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {
  //
  //
  LOG4CPLUS_INFO(getApplicationLogger(), "reConfigure");
  std::cout << "reConfigure" << std::endl ;
  //
}
//
void EmuPeripheralCrateCommand::enableAction(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {
  //
  // currently do nothing
  //
  std::cout << "Received Message Enable" << std::endl ;
  LOG4CPLUS_INFO(getApplicationLogger(), "Received Message Enable");
}
//
void EmuPeripheralCrateCommand::disableAction(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {
  //
  // currently do nothing
  //
  std::cout << "Received Message Disable" << std::endl ;
  LOG4CPLUS_INFO(getApplicationLogger(), "Received Message Disable");
}  
//
void EmuPeripheralCrateCommand::haltAction(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {
  //
  // currently do nothing
  // 
  std::cout << "Received Message Halt" << std::endl ;
  LOG4CPLUS_INFO(getApplicationLogger(), "Received Message Halt");
}  

void EmuPeripheralCrateCommand::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
  throw (toolbox::fsm::exception::Exception) {
  changeState(fsm);
}

bool EmuPeripheralCrateCommand::ParsingXML()
{
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

  void EmuPeripheralCrateCommand::SetCurrentCrate(int cr)
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

  void EmuPeripheralCrateCommand::CheckCrates(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
  {  
    std::cout << "Button: Check Crate Controllers" << std::endl;
    check_controllers();
    this->Default(in, out);
  }

void EmuPeripheralCrateCommand::check_controllers()
{
    if(total_crates_<=0) return;
    for(unsigned i=0; i< crateVector.size(); i++)
    {
        crateVector[i]->CheckController();
    }
    controller_checked_ = true;
}

void EmuPeripheralCrateCommand::ResetAllTMBCounters()
{
  std::vector<TMB*> myVector;
  if(!parsed) ParsingXML();

  if(total_crates_<=0) return;
  //
  for(unsigned i=0; i< crateVector.size(); i++)
  {
        myVector = crateVector[i]->tmbs();
        for(unsigned int j=0; j<myVector.size(); j++)
        {
          myVector[j]->ResetCounters();
        }
  }
}

void EmuPeripheralCrateCommand::CheckCratesConfiguration(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //
  std::cout << "Button:  Check Configuration of All Active Crates" << std::endl;
  //
  VerifyCratesConfiguration();
  //
  this->Default(in, out);
}

int EmuPeripheralCrateCommand::VerifyCCBs()
{
  if(!parsed) ParsingXML();

  if(total_crates_<=0) return 0;
  if(!controller_checked_) check_controllers();
  //
  OutputCheckConfiguration.str(""); //clear the output string
  int initialcrate=current_crate_;
  //
  all_crates_ok = 1;
  //
  for(unsigned i=0; i< crateVector.size(); i++) {
    //
    // add extra controller check before checking config for each crate
    bool cr = (crateVector[i]->vmeController()->SelfTest()) && (crateVector[i]->vmeController()->exist(13));
    crateVector[i]->SetLife( cr );
    if(!cr) OutputCheckConfiguration << getLocalDateTime()<< " Exclude Crate " << crateVector[i]->GetLabel() << std::endl;
    if ( crateVector[i]->IsAlive() ) {
      OutputCheckConfiguration << getLocalDateTime()<< " Check CCB in Crate " << crateVector[i]->GetLabel() << std::endl;
      //
      crateVector[i]->ccb()->RedirectOutput(&OutputCheckConfiguration);  
      ccb_check_ok[i] = crateVector[i]->ccb()->CheckConfig(1);
      if(ccb_check_ok[i]==0) 
      {
         crateVector[i]->ccb()->configure();
         ccb_check_ok[i] = crateVector[i]->ccb()->CheckConfig();
      }
      crateVector[i]->ccb()->RedirectOutput(&std::cout);
      //
      all_crates_ok &= ccb_check_ok[i];
    }
  }
  SetCurrentCrate(initialcrate);
  //
  //Output the logs to a file...
  //
  std::string logfilename = "/tmp/CCB_ConfigurationCheck_"+getLocalDateTime(true)+".log";
  //
  std::ofstream LogFileCheckConfiguration;
  LogFileCheckConfiguration.open(logfilename.c_str());
  LogFileCheckConfiguration << OutputCheckConfiguration.str() ;
  LogFileCheckConfiguration.close();

  return all_crates_ok;
}


int EmuPeripheralCrateCommand::VerifyCratesConfiguration()
{
  if(!parsed) ParsingXML();

  if(total_crates_<=0) return 0;
  if(!controller_checked_) check_controllers();
  //
  OutputCheckConfiguration.str(""); //clear the output string
  int initialcrate=current_crate_;
  //
  all_crates_ok = 1;
  //
  for(unsigned i=0; i< crateVector.size(); i++) {
    //
    // add extra controller check before checking config for each crate
    bool cr = (crateVector[i]->vmeController()->SelfTest()) && (crateVector[i]->vmeController()->exist(13));
    crateVector[i]->SetLife( cr );
    if(!cr) std::cout << "Exclude Crate " << crateVector[i]->GetLabel() << std::endl;
    if ( crateVector[i]->IsAlive() ) {
      //
      SetCurrentCrate(i);	
      //
      CheckPeripheralCrateConfiguration();
      //
      all_crates_ok &= crate_check_ok[i];
    }
  }
  SetCurrentCrate(initialcrate);
  //
  //Output the errors to a file...
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
  buf = "ConfigurationCheckLogFile"+time+".log";
  //
  std::ofstream LogFileCheckConfiguration;
  LogFileCheckConfiguration.open(buf.c_str());
  LogFileCheckConfiguration << OutputCheckConfiguration.str() ;
  LogFileCheckConfiguration.close();

  return all_crates_ok;
}

// Another method which would be better in another class... let's make it work, first....
void EmuPeripheralCrateCommand::CheckPeripheralCrateConfiguration() {
  //
  std::cout << "Configuration check for " << thisCrate->GetLabel() << std::endl;
  //
  crate_check_ok[current_crate_] = 1;
  //
  OutputCheckConfiguration << "Crate " << thisCrate->GetLabel() << std::endl;
  //
  thisCrate->ccb()->RedirectOutput(&OutputCheckConfiguration);
  ccb_check_ok[current_crate_] = thisCrate->ccb()->CheckConfig();
  crate_check_ok[current_crate_] &=  ccb_check_ok[current_crate_];  
  thisCrate->ccb()->RedirectOutput(&std::cout);
  //
  thisCrate->mpc()->RedirectOutput(&OutputCheckConfiguration);
  mpc_check_ok[current_crate_] = thisCrate->mpc()->CheckConfig();
  crate_check_ok[current_crate_] &=  mpc_check_ok[current_crate_];  
  thisCrate->mpc()->RedirectOutput(&std::cout);
  //
  for (unsigned int chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
    //	
    Chamber * thisChamber     = chamberVector[chamber_index];
    TMB * thisTMB             = tmbVector[chamber_index];
    ALCTController * thisALCT = thisTMB->alctController();
    DAQMB * thisDMB           = dmbVector[chamber_index];
    //
    std::cout << "Configuration check for " << thisCrate->GetLabel() << ", " << (thisChamber->GetLabel()).c_str() << std::endl;
    //
    OutputCheckConfiguration << (thisChamber->GetLabel()).c_str() 
			     << " ... " 
			     << (thisChamber->GetProblemDescription()).c_str() 
			     << std::endl;

    //
    thisTMB->RedirectOutput(&OutputCheckConfiguration);
    thisTMB->CheckTMBConfiguration();
    if (thisTMB->GetNumberOfConfigurationReads() > 1)
      OutputCheckConfiguration << "-> N_read(TMB) = " << thisTMB->GetNumberOfConfigurationReads() << std::endl;
    tmb_check_ok[current_crate_][chamber_index]  = (int) thisTMB->GetTMBConfigurationStatus();
    thisTMB->RedirectOutput(&std::cout);
    //
    thisALCT->RedirectOutput(&OutputCheckConfiguration);
    thisALCT->CheckALCTConfiguration();
    if (thisALCT->GetNumberOfConfigurationReads() > 1)
      OutputCheckConfiguration << "-> N_read(ALCT) = " << thisALCT->GetNumberOfConfigurationReads() << std::endl;
    alct_check_ok[current_crate_][chamber_index] = (int) thisALCT->GetALCTConfigurationStatus();
    thisALCT->RedirectOutput(&std::cout);
    //
    thisDMB->RedirectOutput(&OutputCheckConfiguration);
    dmb_check_ok[current_crate_][chamber_index]  = (int) thisDMB->checkDAQMBXMLValues();
    if (thisDMB->GetNumberOfConfigurationReads() > 1)
      OutputCheckConfiguration << "-> N_read(DMB) = " << thisDMB->GetNumberOfConfigurationReads() << std::endl;
    thisDMB->RedirectOutput(&std::cout);
    //
    crate_check_ok[current_crate_] &= tmb_check_ok[current_crate_][chamber_index];
    crate_check_ok[current_crate_] &= alct_check_ok[current_crate_][chamber_index];
    crate_check_ok[current_crate_] &= dmb_check_ok[current_crate_][chamber_index];
    //
  }
  //
  return;
}
//
xoap::MessageReference EmuPeripheralCrateCommand::onConfigCalCFEB (xoap::MessageReference message)
  throw (xoap::exception::Exception) 
{
  char dmbstatus[11];

  if(!parsed) ParsingXML();
  int cfeb_clk_delay=31;
  int pre_block_end=7;
  int feb_cable_delay=0;
  int dword= (6 | (20<<4) | (10<<9) | (15<<14) ) &0xfffff;
  float dac=1.00;

  for(unsigned i=0; i< crateVector.size(); i++) {
    if ( crateVector[i]->IsAlive() ) {
      SetCurrentCrate(i);

// Liu to debug DMB 
//      thisCrate->vmeController()->Debug(10);

      std::cout << "Configuring DAQMB's in crate " << thisCrate->GetLabel() << std::endl;

//	  thisCCB->hardReset();
      //

      for (unsigned int tmb=0; tmb<tmbVector.size(); tmb++) 
      {
          tmbVector[tmb]->DisableCLCTInputs();
          tmbVector[tmb]->DisableALCTInputs();
          // std::cout << "Disabling inputs for TMB slot  " << tmbVector[tmb]->slot() << std::endl;
      }

      //
      for (unsigned int dmb=0; dmb<dmbVector.size(); dmb++) 
      {
	  //
	  std::cout << "Configuring DAQMB " << (chamberVector[dmb]->GetLabel()).c_str() << std::endl;

          dmbVector[dmb]->calctrl_fifomrst();
          usleep(5000);
          dmbVector[dmb]->restoreCFEBIdle();
          dmbVector[dmb]->restoreMotherboardIdle();
          //   float dac=1.00;
          dmbVector[dmb]->set_cal_dac(dac,dac);
          // int dword= (6 | (20<<4) | (10<<9) | (15<<14) ) &0xfffff;
          dmbVector[dmb]->setcaldelay(dword);

	  dmbVector[dmb]->settrgsrc(1);
	  dmbVector[dmb]->fxpreblkend(pre_block_end);
	  dmbVector[dmb]->SetCfebClkDelay(cfeb_clk_delay);
	  dmbVector[dmb]->setfebdelay(dmbVector[dmb]->GetKillFlatClk());
	  dmbVector[dmb]->load_feb_clk_delay();
	  if(dmbVector[dmb]->GetCfebCableDelay() == 1){
	    // In the calibration, we set all cfeb_cable_delay=0 for all DMB's for
	    // timing analysis.  If the collision setting is normally cfeb_cable_delay=1,
	    // in order to ensure the proper timing of the CFEB DAV, we will need to 
	    // adjust cfeb_dav_cable_delay to follow the change of cfeb_cable_delay...
	    dmbVector[dmb]->SetCfebDavCableDelay(dmbVector[dmb]->GetCfebDavCableDelay()+1);
	  }
	  dmbVector[dmb]->SetCfebCableDelay(feb_cable_delay);
	  dmbVector[dmb]->setcbldly(dmbVector[dmb]->GetCableDelay());
	  dmbVector[dmb]->calctrl_global();
	  //
	  // Now check the DAQMB status.  Did the configuration "take"?
	  std::cout << "After config: check status " << (chamberVector[dmb]->GetLabel()).c_str() << std::endl;
	  usleep(50);
	  dmbVector[dmb]->dmb_readstatus(dmbstatus);
	  if( ((dmbstatus[9]&0xff)==0x00 || (dmbstatus[9]&0xff)==0xff) || 
 	      ((dmbstatus[8]&0xff)==0x00 || (dmbstatus[8]&0xff)==0xff) ||
 	      ((dmbstatus[7]&0xff)==0x00 || (dmbstatus[7]&0xff)==0xff) ||
 	      ((dmbstatus[6]&0xff)==0x00 || (dmbstatus[6]&0xff)==0xff) ||
	      ((dmbstatus[0]&0xff)!=0x21)                              ) {
	    std::cout << "... config check not OK for " << (chamberVector[dmb]->GetLabel()).c_str() << std::endl;
 	  }
      }
    }
  }

  return createReply(message);
}
//

// alct calibrations (Madorsky)
xoap::MessageReference EmuPeripheralCrateCommand::onConfigCalALCT (xoap::MessageReference message) 
  throw (xoap::exception::Exception) 
{
	//
	std::cout<< "Inside onConfigCalALCT-command"<<std::endl;
	calsetup = 0;

	// this will read all crate and chamber info from xml file and fill crateVector
	if(!parsed) ParsingXML();
	int tc = crateVector.size();
	std::cout << "XTEP: crateVector size = " << /*crateVector.size()*/ tc << std::endl;

	// read test configuration from xml into tcs structure
	int cal_conf_res = read_test_config("alct-calib-config.xml", &tcs);
	std::cout << "ALCT calibration configuration reading result (0 == OK): " << cal_conf_res << std::endl;
	return createReply(message);
	//
}

xoap::MessageReference EmuPeripheralCrateCommand::onEnableCalALCTConnectivity (xoap::MessageReference message) 
  throw (xoap::exception::Exception) 
{

	std::cout << std::dec << "XTEP: ALCT connectivity test 12" << std::endl;

	for(unsigned i=0; i< crateVector.size(); i++) 
    {
		
		if ( crateVector[i]->IsAlive() ) 
		{
			
			SetCurrentCrate(i);	
			std::cout << "XTEP: Setting up crate: " << std::dec << ThisCrateID_ << std::endl;
			for (unsigned tn = 0; tn < tmbVector.size(); tn++)
			{
				time_t currentTime;
				time (&currentTime); // fill now with the current time

				int strip_mask = (1 << calsetup);
				std::cout << "XTEP: "<< ctime(&currentTime)  << " setting up chamber: " << thisCrate->GetChamber(tmbVector[tn]->slot())->GetLabel().c_str() << std::endl;
				std::cout << "XTEP: calibration step: " << calsetup << std::endl;
				std::cout << "XTEP: strip mask: " << std::hex << "0x" << strip_mask << std::dec << std::endl;

				tmbVector[tn]->SetCheckJtagWrite(1);
								
				ALCTController * alct = tmbVector[tn]->alctController();
				std::string chamtype = alct->GetChamberType(); // returns "MEXX"

				// map of test pulse amplitudes for all chamber types, taken from config file
				std::map <std::string, int*> tpamp_map;
				tpamp_map["ME11"] = &tcs.t12.alct_test_pulse_amp_11;
				tpamp_map["ME12"] = &tcs.t12.alct_test_pulse_amp_12;
				tpamp_map["ME13"] = &tcs.t12.alct_test_pulse_amp_13;
				tpamp_map["ME21"] = &tcs.t12.alct_test_pulse_amp_21;
				tpamp_map["ME22"] = &tcs.t12.alct_test_pulse_amp_22;
				tpamp_map["ME31"] = &tcs.t12.alct_test_pulse_amp_31;
				tpamp_map["ME32"] = &tcs.t12.alct_test_pulse_amp_32;
				tpamp_map["ME41"] = &tcs.t12.alct_test_pulse_amp_41;
				tpamp_map["ME42"] = &tcs.t12.alct_test_pulse_amp_42;
	      		
				int tpamp = *(tpamp_map[chamtype]);
				alct->SetUpPulsing
				(
					tpamp,
					PULSE_LAYERS, 
					strip_mask,
					ADB_SYNC
				);

				std::cout << "XTEP: chamber type: " << chamtype << "  ALCT test pulse amplitude: " << tpamp << std::endl;

				// set up alct so it sends DAQ block without trigger
				alct->SetSendEmpty(1);
				alct->WriteConfigurationReg();
			}
		}
    }
	calsetup++; // step counter

	::sleep(1);
	return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateCommand::onEnableCalALCTThresholds (xoap::MessageReference message) 
  throw (xoap::exception::Exception) 
{
	std::cout << std::dec << "XTEP: ALCT thresholds test 13" << std::endl;

	int num_thresh     = tcs.t13.thresholds_per_tpamp; // number of thresholds to scan with each test pulse amp
	int first_thresh   = tcs.t13.threshold_first; // first thresh
	int thresh_step    = tcs.t13.threshold_step; // threshold step

	// calculate indexes
	int k = calsetup % num_thresh; // threshold number
	int tpanum = calsetup / num_thresh; // test pulse amplitude number

	int cur_thresh = first_thresh + thresh_step*k;
	int cur_tpamp = tcs.t13.tpamp_first + tpanum * tcs.t13.tpamp_step;
	// every ev_per_strip events switch test strip
	std::cout << "XTEP: Setting tpamp:  " << cur_tpamp << " thresh: " << cur_thresh << std::endl;

	for(unsigned i=0; i< crateVector.size(); i++) 
    {
		
		if ( crateVector[i]->IsAlive() ) 
		{
			
			SetCurrentCrate(i);	
			std::cout << "XTEP: Setting up crate: " << std::dec << ThisCrateID_ << std::endl;
			for (unsigned tn = 0; tn < tmbVector.size(); tn++)
			{
				time_t currentTime;
				time (&currentTime); // fill now with the current time

				std::cout << "XTEP: "<< ctime(&currentTime)  << " setting up chamber: " << thisCrate->GetChamber(tmbVector[tn]->slot())->GetLabel().c_str() << std::endl;
				std::cout << "XTEP: calibration step: " << calsetup << std::endl;

				tmbVector[tn]->SetCheckJtagWrite(1);
								
				ALCTController * alct = tmbVector[tn]->alctController();
				for (int c = 0; c <= alct->MaximumUserIndex(); c++)
					alct->SetAfebThreshold(c, cur_thresh);

				alct->WriteAfebThresholds();

				alct->SetUpPulsing
				(
					cur_tpamp, 
					PULSE_AFEBS, 
					0x7f, // afeb group mask
					ADB_SYNC
				);

				// set up alct so it sends DAQ block without trigger
				alct->SetSendEmpty(1);
				alct->WriteConfigurationReg();
			}
		}
	}
	calsetup++;
	return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateCommand::onEnableCalALCTDelays (xoap::MessageReference message) 
  throw (xoap::exception::Exception) 
{
	std::cout << std::dec << "XTEP: ALCT delays test 14" << std::endl;

	// calsetup is number of delay setting
	int cur_delay = tcs.t14.delay_first + tcs.t14.delay_step * calsetup;

	std::cout << "XTEP: Setting delay:  " << cur_delay << std::endl;

	for(unsigned i=0; i< crateVector.size(); i++) 
    {
		
		if ( crateVector[i]->IsAlive() ) 
		{
			
			SetCurrentCrate(i);	
			std::cout << "XTEP: Setting up crate: " << std::dec << ThisCrateID_ << std::endl;
			for (unsigned tn = 0; tn < tmbVector.size(); tn++)
			{
				time_t currentTime;
				time (&currentTime); // fill now with the current time

				std::cout << "XTEP: "<< ctime(&currentTime)  << " setting up chamber: " << thisCrate->GetChamber(tmbVector[tn]->slot())->GetLabel().c_str() << std::endl;
				std::cout << "XTEP: calibration step: " << calsetup << std::endl;

				tmbVector[tn]->SetCheckJtagWrite(1);
								
				ALCTController * alct = tmbVector[tn]->alctController();

				for (int k = 0; k <= alct->MaximumUserIndex(); k++) 
					alct->SetAsicDelay(k, cur_delay);

				alct->WriteAsicDelaysAndPatterns();

				alct->SetUpPulsing
				(
					tcs.t14.alct_test_pulse_amp, 
					PULSE_AFEBS, 
					0x3fff, // all afebs
					ADB_ASYNC
				);

				// set up alct so it sends DAQ block without trigger
				alct->SetSendEmpty(1);
				alct->WriteConfigurationReg();
			}
		}
	}
	calsetup++;
	return createReply(message);
}


// macro to fill the parameter map
#define map_add(tnum, parname) map##tnum[#parname] = &(tcs->t##tnum.parname)

int EmuPeripheralCrateCommand::read_test_config(char* xmlFile, test_config_struct * tcs) 
{
	char* step_path_ch = getenv("HOME");
	if (step_path_ch == NULL) return -1;
	
	std::string step_config = std::string(step_path_ch) + "/config/pc/" + xmlFile;

	memset(tcs, 0, sizeof(test_config_struct));

	// maps of parameters for each test
	std::map <std::string, int*> map11, map12, map13, map14, map15, map16, map17, map18, map19, map21, map30;
	std::map <std::string, int*>* cur_map; // currently used map

	// fill the maps (see macro above)
	map_add(11, events_total);

	map_add(12, events_per_strip);
	map_add(12, alct_test_pulse_amp_11);
	map_add(12, alct_test_pulse_amp_12);
	map_add(12, alct_test_pulse_amp_13);
	map_add(12, alct_test_pulse_amp_21);
	map_add(12, alct_test_pulse_amp_22);
	map_add(12, alct_test_pulse_amp_31);
	map_add(12, alct_test_pulse_amp_32);
	map_add(12, alct_test_pulse_amp_41);
	map_add(12, alct_test_pulse_amp_42);

	map_add(13, events_per_threshold);
	map_add(13, thresholds_per_tpamp);
	map_add(13, threshold_first);	 
	map_add(13, threshold_step);	 
	map_add(13, tpamps_per_run);	 
	map_add(13, tpamp_first);     
	map_add(13, tpamp_step);	     

	map_add(14, alct_test_pulse_amp);
	map_add(14, events_per_delay);
	map_add(14,	delays_per_run); 
	map_add(14,	delay_first);	 
	map_add(14,	delay_step);	 

	map_add(15, events_total);

	map_add(16, events_per_layer);
	map_add(16, alct_test_pulse_amp);

	map_add(17, dmb_test_pulse_amp);
	map_add(17, events_per_delay); 
	map_add(17, delays_per_strip); 
	map_add(17, delay_first);      
	map_add(17, delay_step);       
	map_add(17, strips_per_run);   
	map_add(17, strip_first);      
	map_add(17, strip_step);       

	map_add(18, events_total);

	map_add(19, scale_turnoff);
	map_add(19, range_turnoff);
	map_add(19, events_per_thresh);
	map_add(19, threshs_per_tpamp);
	map_add(19, thresh_first);
	map_add(19, thresh_step);
	map_add(19, dmb_tpamps_per_strip);
	map_add(19, dmb_tpamp_first);
	map_add(19, dmb_tpamp_step);
	map_add(19, strips_per_run);
	map_add(19, strip_first);
	map_add(19, strip_step);

	map_add(21, dmb_test_pulse_amp);
	map_add(21, cfeb_threshold);    
	map_add(21, events_per_hstrip); 
	map_add(21, hstrips_per_run);   
	map_add(21, hstrip_first);      
	map_add(21, hstrip_step);       

	map_add(30, events_per_delay);
	map_add(30, tmb_l1a_delays_per_run);
	map_add(30, tmb_l1a_delay_first);
	map_add(30, tmb_l1a_delay_step);

	// map of test parameter maps
	std::map <std::string, std::map <std::string, int*>* > test_map; 
	test_map["11"] = &map11;
	test_map["12"] = &map12;
	test_map["13"] = &map13;
	test_map["14"] = &map14;
	test_map["15"] = &map15;
	test_map["16"] = &map16;
	test_map["17"] = &map17;
	test_map["18"] = &map18;
	test_map["19"] = &map19;
	test_map["21"] = &map21;
	test_map["30"] = &map30;

	cur_map = test_map["11"];
  // Initialize -- probably gets overwritten

	std::cout << "Loading test configuration from XML file: "  <<  step_config << std::endl;

	if (step_config == "") 
    {
		std::cout << "Invalid configuration file: " << step_config << std::endl;
		return 1;
    }

	XMLPlatformUtils::Initialize();
	XercesDOMParser *parser = new XercesDOMParser();
	parser->setValidationScheme(XercesDOMParser::Val_Always);
	parser->setDoNamespaces(true);
	parser->setDoSchema(true);
	parser->setValidationSchemaFullChecking(false); // this is default
	parser->setCreateEntityReferenceNodes(true);  // this is default
	parser->setIncludeIgnorableWhitespace (false);

	parser->parse(step_config.c_str());
	DOMDocument *doc = parser->getDocument();
	DOMNodeList *l = doc->getElementsByTagName( XMLString::transcode("XTEP_tests") );
	if( l->getLength() != 1 )
    {
		std::cout << "There is not exactly one STEP_tests node in configuration" << std::endl;
		return 1;
    }
	DOMNodeList *itemList = doc->getElementsByTagName( XMLString::transcode("test_config") );
	if( itemList->getLength() == 0 )
    {
		std::cout << "There are no test configuration sections" << std::endl;
		return 1;
    }

	DOMNode* info;

	for(unsigned int j=0; j<itemList->getLength(); j++)
    {
		info = itemList->item(j); // test config section
		
		std::map<std::string, std::string> obj_info;
		DOMNodeList *children = info->getChildNodes();

		std::string nodename = std::string(XMLString::transcode(info->getNodeName()));
		std::cout <<  "node: " << trim(nodename) << std::endl;

		// test_config node found. Decode parameters
		for(unsigned int i=0; i<children->getLength(); i++)
		{
			std::string paramname = std::string(XMLString::transcode(children->item(i)->getNodeName()));
			trim(paramname);

			if ( children->item(i)->hasChildNodes() ) 
			{
				std::string param = std::string(XMLString::transcode(children->item(i)->getFirstChild()->getNodeValue()));
				trim(param);

				std::cout <<  paramname << " = " << param << std::endl;
				if (paramname.compare("test") == 0) // test number
					cur_map = test_map[param]; // select map of that test
				else // one of the parameters
					*((*cur_map)[paramname]) = atol(param.c_str()); // assign to structure item
				
			}
		}


    }

	delete parser;
	return 0;
}

// removes leading and trailing spaces from a string
std::string& EmuPeripheralCrateCommand::trim(std::string &str)
{
	std::string whitespaces (" \t\f\v\n\r");
	size_t found;
  
	// trailing
	found = str.find_last_not_of(whitespaces);
	if (found != std::string::npos)
		str.erase(found+1);
	else
		str.clear();            // str is all whitespace

	// leading
	found = str.find_last_of(whitespaces);
	if (found != std::string::npos)
		str.erase(0, found+1);

	return str;

}
// ---


 }  // namespace emu::pc
}  // namespace emu
// provides factory method for instantion of HellWorld application
//
XDAQ_INSTANTIATOR_IMPL(emu::pc::EmuPeripheralCrateCommand)
//
