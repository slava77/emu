// $Id: EmuPeripheralCrateService.cc

#include "emu/pc/EmuPeripheralCrateService.h"

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <unistd.h> 
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <time.h>

namespace emu {
  namespace pc {
/////////////////////////////////////////////////////////////////////
// Instantiation and main page
/////////////////////////////////////////////////////////////////////
EmuPeripheralCrateService::EmuPeripheralCrateService(xdaq::ApplicationStub * s): EmuPeripheralCrateBase(s)
{	
  MyController = 0;
  //
  crates_checked = 0;
  all_crates_ok = -1;
  for (int i=0; i<60; i++) {
    crate_check_ok[i] = 0;
    crate_state[i] = 0;
  }
  last_msg=0;
  total_msg=0;
  //
  //
  xgi::bind(this,&EmuPeripheralCrateService::Default, "Default");
  xgi::bind(this,&EmuPeripheralCrateService::MainPage, "MainPage");
  //
  // bind crate utilities
  //
  xgi::bind(this,&EmuPeripheralCrateService::CheckCrates, "CheckCrates");
  xgi::bind(this,&EmuPeripheralCrateService::CrateSelection, "CrateSelection");
  xgi::bind(this,&EmuPeripheralCrateService::HardReset, "HardReset");
  xgi::bind(this,&EmuPeripheralCrateService::FastConfigCrates, "FastConfigCrates");
  xgi::bind(this,&EmuPeripheralCrateService::FastConfigOne, "FastConfigOne");
  xgi::bind(this,&EmuPeripheralCrateService::ForEmuPage1, "ForEmuPage1");
  xgi::bind(this,&EmuPeripheralCrateService::SwitchBoard, "SwitchBoard"); 
  //
  //-------------------------------------------------------------
  // fsm_ is defined in EmuApplication
  //-------------------------------------------------------------
  fsm_.addState('H', "Halted",     this, &EmuPeripheralCrateService::stateChanged);
  //
  fsm_.setInitialState('H');
  fsm_.reset();    
  //
  // state_ is defined in EmuApplication
  state_ = fsm_.getStateName(fsm_.getCurrentState());
  //
  //----------------------------
  // initialize variables
  //
  xml_or_db = -1;  /* actual configuration source: 0: xml, 1: db, -1: unknown or error */
  XML_or_DB_ = "xml";
  EMU_config_ID_ = "1000001";
  xmlFile_ = "config.xml" ;
  Simulation_ = false;
  GuiButton_ = true;
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

  global_config_states[0]="UnConfigured";
  global_config_states[1]="Configuring";
  global_config_states[2]="Configured";
  global_run_states[0]="Halted";
  global_run_states[1]="Enabled";
  current_config_state_=0;
  current_run_state_=0;
  total_crates_=0;
  this_crate_no_=0;

  parsed=0;
}

void EmuPeripheralCrateService::MainPage(xgi::Input * in, xgi::Output * out ) 
{
  if(!parsed) ParsingXML();

  if(endcap_side==1)
     MyHeader(in,out,"EmuPeripheralCrateService -- Plus Endcap");
  else if(endcap_side==-1)
     MyHeader(in,out,"EmuPeripheralCrateService -- Minus Endcap");
  else
     MyHeader(in,out,"EmuPeripheralCrateService -- Stand-alone");

  *out << cgicc::table().set("border","0");
    //
  *out << cgicc::td();
  *out << "Total Crates : " << total_crates_ << cgicc::br() << std::endl ;

  unsigned int active_crates=0;
  for(unsigned i=0; i<crateVector.size(); i++)
     if(crateVector[i]->IsAlive()) active_crates++;
  if( active_crates <= total_crates_) 
     *out << cgicc::b(" Active Crates: ") << active_crates  << cgicc::br() << std::endl ;
 
  unsigned int inited_crates=0;
  for(unsigned i=0; i<crateVector.size(); i++)
     if(crate_state[i]==1) inited_crates++;
  if( inited_crates <= total_crates_) 
     *out << cgicc::b(" Initialized Crates: ") << inited_crates << std::endl ;
  *out << cgicc::td();
  if (Simulation_)
  {  *out << cgicc::td();
     *out << cgicc::span().set("style","color:red") << cgicc::h2("Simulation ON");
     *out << cgicc::span();
     *out << cgicc::td();
  }
  *out << cgicc::table() << std::endl;

  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial; background-color:blue");
  *out << std::endl;
  *out << cgicc::table().set("border","0");
    //
  *out << cgicc::td();
  std::string CheckCrates = toolbox::toString("/%s/CheckCrates",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CheckCrates) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Check Crate Controllers") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();

  *out << cgicc::td();
  std::string FastConfigureAll = toolbox::toString("/%s/FastConfigCrates",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",FastConfigureAll) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Crates Power-up Init") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();

  *out << cgicc::table();

  *out << cgicc::fieldset() << std::endl;

  *out <<  cgicc::hr() << cgicc::h2("Crate Utilities")<< std::endl;
  //
  *out << cgicc::span().set("style","color:blue");
  *out << cgicc::b(cgicc::i("Current Crate : ")) ;
  *out << ThisCrateID_ << cgicc::span() << std::endl ;
  //
  *out << cgicc::br();
  //

  // Begin select crate
  // Config listbox
  *out << cgicc::form().set("action",
		     "/" + getApplicationDescriptor()->getURN() + "/CrateSelection") << std::endl;
  
  int n_keys = crateVector.size();
  
  *out << "Choose Crate: " << std::endl;
  *out << cgicc::select().set("name", "runtype") << std::endl;
  
  int selected_index = this_crate_no_;
  std::string CrateName;
  for (int i = 0; i < n_keys; ++i) {
    if(crate_state[i]==0)
      CrateName = crateVector[i]->GetLabel() + " -- ";
    else if(crate_state[i]>0)
      CrateName = crateVector[i]->GetLabel() + " UP ";
    else if(crate_state[i]<0)
      CrateName = crateVector[i]->GetLabel() + " DN ";

    if(!(crateVector[i]->IsAlive()))
      CrateName = CrateName + "  NG";
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
  std::cout << "Main Page: "<< std::dec << active_crates << "/" <<total_crates_ << " Crates" << std::endl;
  //
  if (tmbVector.size()>0 || dmbVector.size()>0) {
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial; background-color:blue");
    *out << std::endl;
    //
    *out << cgicc::table().set("border","0");
    //
    *out << cgicc::td();
    std::string FastConfigOne = toolbox::toString("/%s/FastConfigOne",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",FastConfigOne) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Power-up Init") << std::endl ;
    *out << cgicc::form() << std::endl ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    std::string HardReset = toolbox::toString("/%s/HardReset",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",HardReset) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Hard Reset") << std::endl ;
    *out << cgicc::form() << std::endl ;
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    //
    *out << cgicc::fieldset() << cgicc::hr() << std::endl;
    //
  }
  *out << cgicc::br() << cgicc::b("Last Received Commands") << std::endl;
  *out << cgicc::fieldset().set("style","font-size: 10pt; font-family: arial; color: yellow; background-color:blue");
  *out << std::endl;
  if(total_msg > 0)
  {
     // commands display
     int idx=last_msg;    
     int max_display = (total_msg < 15) ? total_msg : 15;
     for(int i=0; i< max_display; i++)
     {
        *out << command_msg[idx--] << cgicc::br() << std::endl;
        if(idx < 0) idx += MAX_MESSAGES;
     }
  }
  *out << cgicc::fieldset() << std::endl;
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
  *out << cgicc::br();
  //
}

// 
void EmuPeripheralCrateService::MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) 
  throw (xgi::exception::Exception) {
  //
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  //
  std::string myUrn = getApplicationDescriptor()->getURN().c_str();
  xgi::Utils::getPageHeader(out,title,myUrn,"","");
  //
}
//
void EmuPeripheralCrateService::Default(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<"MainPage"<<"\">" <<std::endl;
}
//

void EmuPeripheralCrateService::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
  throw (toolbox::fsm::exception::Exception) {
  changeState(fsm);
}

  void EmuPeripheralCrateService::CrateSelection(xgi::Input * in, xgi::Output * out ) 
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

  void EmuPeripheralCrateService::FastConfigCrates(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
     if(!parsed) ParsingXML();

     msgHandler("Button: Power-Up-Init All Crates");

     ConfigureInit(2);
     this->Default(in,out);
  }

  void EmuPeripheralCrateService::FastConfigOne(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
     if(!parsed) ParsingXML();

     msgHandler("Button: Power-Up-Init Crate " + ThisCrateID_);

     if(!Simulation_) thisCrate->configure(2);
     crate_state[current_crate_] = 1;
     this->Default(in,out);
  }

  void EmuPeripheralCrateService::ConfigureInit(int c)
  {
    if(total_crates_<=0) return;
    current_config_state_=1;
    for(unsigned i=0; i< crateVector.size(); i++)
    {
        if(crateVector[i] && crateVector[i]->IsAlive())
        {   if(!Simulation_) crateVector[i]->configure(c);
            crate_state[i] = 1;
        }
    }
    current_config_state_=2;
    //
  }

  void EmuPeripheralCrateService::CheckCrates(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
  {  
    if(!parsed) ParsingXML();

    msgHandler("Button: Check Crate Controllers");
    check_controllers();
    this->Default(in, out);
  }

  void EmuPeripheralCrateService::check_controllers()
  {
    if(total_crates_<=0) return;
    for(unsigned i=0; i< crateVector.size(); i++)
    {
        if(!Simulation_) crateVector[i]->CheckController();
    }
    crates_checked = 1;
  }

  void EmuPeripheralCrateService::HardReset(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
     if(!parsed) ParsingXML();

     msgHandler("Button: Hard-Reset Crate " + ThisCrateID_);
    //
    if(!Simulation_) thisCCB->hardReset();
    //
    this->Default(in,out);
    //
  }

bool EmuPeripheralCrateService::ParsingXML(){
  //
  Logger logger_ = getApplicationLogger();
  //
  LOG4CPLUS_INFO(logger_, "EmuPeripheralCrateService starts...");
  //
  LOG4CPLUS_INFO(logger_, "Parsing Configuration XML");
    //
  std::cout << "XML_or_DB: " << XML_or_DB_.toString() << std::endl;
  if(XML_or_DB_.toString() == "xml" || XML_or_DB_.toString() == "XML")
  {
    // Check if filename exists
    //
    if(xmlFile_.toString().find("http") == std::string::npos) 
    {
      std::ifstream filename(xmlFile_.toString().c_str());
      if(filename.is_open()) {
	filename.close();
      }
      else {
	LOG4CPLUS_ERROR(logger_, "Filename doesn't exist");
	XCEPT_RAISE (toolbox::fsm::exception::Exception, "Filename doesn't exist");
	return false;
      }
    }
    //
    //cout <<"Start Parsing"<<endl;
    if ( MyController != 0 ) {
      LOG4CPLUS_INFO(logger_, "Delete existing controller");
      delete MyController ;
    }
    //
    MyController = new EmuController();

    MyController->SetConfFile(xmlFile_.toString().c_str());
    MyController->init();
    MyController->NotInDCS();
    //
    emuEndcap_ = MyController->GetEmuEndcap();
    if(!emuEndcap_) return false;
    xml_or_db = 0;
  }
  else if (XML_or_DB_.toString() == "db" || XML_or_DB_.toString() == "DB")
  {
    // from TStore    
    // std::cout << "We are in db" << std::endl;
    myTStore = new EmuTStore(this);
    if(!myTStore)
    {  std::cout << "Can't create object EmuTStore" << std::endl;
       return false;  
    }
    emuEndcap_ = myTStore->getConfiguredEndcap(EMU_config_ID_.toString());   
    if(!emuEndcap_) 
    {  std::cout << "No EmuEndcap returned from TStore" << std::endl;
       return false;
    }
    xml_or_db = 1;
  }
  else
  {
    std::cout << "No valid XML_or_DB found..." << std::endl;
    return false;
  }
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

  void EmuPeripheralCrateService::SetCurrentCrate(int cr)
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

void EmuPeripheralCrateService::msgHandler(std::string msg)
{
     std::string logmsg = getLocalDateTime() + " " + msg;
     total_msg++;
     last_msg++;
     if(last_msg >= MAX_MESSAGES) last_msg=0;
     command_msg[last_msg] = logmsg;
     std::cout << logmsg << std::endl;
}

void EmuPeripheralCrateService::ForEmuPage1(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
  *out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl
       << "<?xml-stylesheet type=\"text/xml\" href=\"/emu/base/html/EmuPage1_XSL.xml\"?>" << std::endl
       << "<ForEmuPage1 application=\"" << getApplicationDescriptor()->getClassName()
       <<                   "\" url=\"" << getApplicationDescriptor()->getContextDescriptor()->getURL()
       <<         "\" localDateTime=\"" << getLocalDateTime() << "\">" << std::endl;

    *out << "  <monitorable name=\"" << "title"
         <<            "\" value=\"" << "PCrate Service " + (std::string)((endcap_side==1)?"Plus":"Minus")
         <<  "\" nameDescription=\"" << " "
         << "\" valueDescription=\"" << " "
         <<          "\" nameURL=\"" << " "
         <<         "\" valueURL=\"" << " "
         << "\"/>" << std::endl;

    *out << "  <monitorable name=\"" << "VME Access"
         <<            "\" value=\"" << "Ready"
         <<  "\" nameDescription=\"" << " "
         << "\" valueDescription=\"" << " "
         <<          "\" nameURL=\"" << " "
         <<         "\" valueURL=\"" << " "
         << "\"/>" << std::endl;

  *out << "</ForEmuPage1>" << std::endl;
}

void EmuPeripheralCrateService::SwitchBoard(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception)
{
  cgicc::CgiEnvironment cgiEnvi(in);
  //
  std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
  Page=cgiEnvi.getQueryString();
  std::string command_name=Page.substr(0,Page.find("=", 0) );
  std::string command_argu=Page.substr(Page.find("=", 0)+1);

  if(!parsed) ParsingXML();

  if (command_name=="STATUS")
  {
     *out << "Crate Status " << crateVector.size() << " ";
     for ( unsigned int i = 0; i < crateVector.size(); i++ )
     {
        char ans;
        switch(crate_state[i])
        { 
           case 1: 
              ans='U'; break;
           case -1: 
              ans='D'; break;
           case -2: 
              ans='N'; break;
           default: 
              ans='-'; break;
        }
        *out << ans;
     }
     *out << std::endl;
     msgHandler("Message: Status Query");
  }
  else if (command_name=="CRATEOFF")
  {
     if(command_argu=="ALL")
     {
        for ( unsigned int i = 0; i < crateVector.size(); i++) crate_state[i]=-1;
        *out << "Switch Off Successful " <<  command_argu << std::endl;
        msgHandler("Message: Switch Off All Crates");
     }
     else for ( unsigned int i = 0; i < crateVector.size(); i++ )
     {
        if(command_argu==crateVector[i]->GetLabel()) 
        {  crate_state[i] = -1;
           *out << "Switch Off Successful " <<  command_argu << std::endl;
           msgHandler("Message: Switch Off Crate " + command_argu);
        }
     }
  }
  else if (command_name=="POWERUP")
  {
     if(command_argu=="ALL")
     {
        if(!Simulation_) ConfigureInit(2);
        *out << "Power Up Successful " <<  command_argu << std::endl;
        msgHandler("Message: Power-Up-Init All Crates");
     }
     else for ( unsigned int i = 0; i < crateVector.size(); i++ )
     {
        if(command_argu==crateVector[i]->GetLabel()) 
        {  crate_state[i] = 1;
           if(!Simulation_) crateVector[i]->configure(2);
           *out << "Power Up Successful " <<  command_argu << std::endl;
           msgHandler("Message: Power-Up-Init Crate " + command_argu);
        }
     }
  }
  else if (command_name=="SIMULATION")
  {
     if(command_argu=="ON" || command_argu=="on")
     {   Simulation_ = true;
         msgHandler("Message: SIMULATION ON; no hardware access");
     }
     else if (command_argu=="OFF" || command_argu=="off")
     {   Simulation_ = false;
         msgHandler("Message: SIMULATION OFF; real hardware access");
     }
     *out << "Simulation Status: " <<  Simulation_ << std::endl;
  }
  else if (command_name=="BUTTON")
  {
     if(command_argu=="ON" || command_argu=="on")
     {   GuiButton_ = true;
         msgHandler("Message: BUTTON ON; GUI action enabled");
     }
     else if (command_argu=="OFF" || command_argu=="off")
     {   GuiButton_ = false;
         msgHandler("Message: BUTTON OFF; GUI action disabled");
     }
     *out << "Button Status: " <<  GuiButton_ << std::endl;
  }
  else
  {
     *out << "Unknown command" << std::endl;
     std::cout << "Unknown command: " << command_name << " " << command_argu << std::endl;
  }
}

 }  // namespace emu::pc
}  // namespace emu

XDAQ_INSTANTIATOR_IMPL(emu::pc::EmuPeripheralCrateService)
