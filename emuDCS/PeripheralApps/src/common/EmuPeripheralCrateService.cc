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
  crates_checked = 0;
  all_crates_ok = -1;
  for (int i=0; i<60; i++) {
    crate_check_ok[i] = 0;
    crate_state[i] = 0;
  }
  last_msg=0;
  total_msg=0;
  main_url_="";
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
  xgi::bind(this,&EmuPeripheralCrateService::ChamberOff, "ChamberOff");
  xgi::bind(this,&EmuPeripheralCrateService::FastConfigCrates, "FastConfigCrates");
  xgi::bind(this,&EmuPeripheralCrateService::FastConfigOne, "FastConfigOne");
  xgi::bind(this,&EmuPeripheralCrateService::FlashHistory, "FlashHistory");
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
  ParsingXML();

//  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/" << getApplicationDescriptor()->getURN() << "/MainPage" << "\">" << std::endl;

  if(endcap_side==1)
     MyHeader(in,out,"EmuPeripheralCrateService -- Plus Endcap");
  else if(endcap_side==-1)
     MyHeader(in,out,"EmuPeripheralCrateService -- Minus Endcap");
  else
     MyHeader(in,out,"EmuPeripheralCrateService -- Stand-alone");

  *out << cgicc::span().set("style","color:blue");
  if(Xml_or_Db()==0)
  {
    *out << cgicc::b(cgicc::i("Configuration filename : ")) ;
    *out << xmlFile_.toString();
  }
  else
  {
    *out << cgicc::b(cgicc::i("Configuration ID used in the last action : ")) ;
    *out << GetRealKey();
  }
  *out << cgicc::br() << cgicc::span() << std::endl;
  //
  *out << cgicc::hr() << cgicc::table().set("border","0");

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
  *out << cgicc::td() << cgicc::td();
  if (!GuiButton_)
  {  
     *out << cgicc::span().set("style","color:red") << cgicc::h2("--Buttons Disabled--");
     *out << cgicc::span();
  }
  *out << cgicc::td() << cgicc::td();  
  if (Simulation_)
  {  
     *out << cgicc::span().set("style","color:red") << cgicc::h2("--Simulation On--");
     *out << cgicc::span();
  }
  *out << cgicc::td();
  *out << cgicc::table() << std::endl;

  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial; background-color:#0099FF");
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

  *out << cgicc::td();
  std::string FlashH = toolbox::toString("/%s/FlashHistory",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::a("[Read Flash History]").set("href",FlashH).set("target","_blank") << std::endl;
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
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial; background-color:#0099FF");
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
    *out << cgicc::td();
    std::string poweroff = toolbox::toString("/%s/ChamberOff",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",poweroff) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Chambers Off") << std::endl ;
    *out << cgicc::form() << std::endl ;
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    //
    *out << cgicc::fieldset() << cgicc::hr() << std::endl;
    //
  }
  *out << cgicc::br() << cgicc::b("Last Received Commands") << cgicc::br() << std::endl;
//  *out << cgicc::fieldset().set("style","font-size: 10pt; font-family: arial; color: yellow; background-color:#0099FF");
  *out << std::endl;
  *out << cgicc::textarea().set("name","commands").set("WRAP","OFF").set("rows","20").set("cols","100");
  if(total_msg > 0)
  {
     // commands display
     int idx=last_msg;    
     int max_display = (total_msg < 100) ? total_msg : 100;
     for(int i=0; i< max_display; i++)
     {
        *out << command_msg[idx--] << std::endl;
        if(idx < 0) idx += MAX_MESSAGES;
     }
  }
  *out << cgicc::textarea();
//  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::br() << std::endl; 
}

// 
void EmuPeripheralCrateService::MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) 
  throw (xgi::exception::Exception) {
  //
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  //
  std::string myUrl = getApplicationDescriptor()->getContextDescriptor()->getURL();
  std::string myUrn = getApplicationDescriptor()->getURN().c_str();
  xgi::Utils::getPageHeader(out,title,myUrn,"","");
  //
}
//
void EmuPeripheralCrateService::Default(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  *out << "<head> <meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<"MainPage"<<"\"> </head>" <<std::endl;
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
     ParsingXML(true);

     if(GuiButton_)
     {
        msgHandler("Button:  Power-Up-Init All Crates");

        if(!Simulation_)
        {  int getlock=PCsendCommand("Locked", "emu::pc::EmuPeripheralCrateTimer");
           if(getlock>0)  
              ConfigureInit(2);
           else 
              msgHandler("ERROR: Xmas not responding");
           PCsendCommand("Unlock", "emu::pc::EmuPeripheralCrateTimer");
        }
        else
           ConfigureInit(2);
     }
     this->Default(in,out);
  }

  void EmuPeripheralCrateService::FastConfigOne(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
     ParsingXML(true);

     if(GuiButton_)
     {
        msgHandler("Button:  Power-Up-Init Crate " + ThisCrateID_);
        int rt=0;
        if(!Simulation_)
        {  int getlock=PCsendCommand("Locked", "emu::pc::EmuPeripheralCrateTimer");
           if(getlock>0)  
              rt=thisCrate->configure(2);
           else 
              msgHandler("ERROR: Xmas not responding");
           PCsendCommand("Unlock", "emu::pc::EmuPeripheralCrateTimer");
        }
        crate_state[current_crate_] = (rt==0)?1:0;
        if(rt==0)
        {
           PCsendCommandwithAttr("SoapInfo", "CrateUp", ThisCrateID_, "emu::x2p::EmuDim");
           msgHandler("Info:    Power-Up-Init Crate " + ThisCrateID_ + " done");
        }
     }
     this->Default(in,out);
  }

  void EmuPeripheralCrateService::ConfigureInit(int c)
  {
    if(total_crates_<=0) return;
    current_config_state_=1;
    for(unsigned i=0; i< crateVector.size(); i++)
    {
        if(crateVector[i] && crateVector[i]->IsAlive())
        {   int rt=0;
            std::string cratename = crateVector[i]->GetLabel();
            if(!Simulation_) rt=crateVector[i]->configure(c);
            crate_state[i] = (rt==0)?1:0;
            if(c==2 && rt==0)
            {
               PCsendCommandwithAttr("SoapInfo", "CrateUp", cratename, "emu::x2p::EmuDim");
               msgHandler("Info:    Power-Up-Init Crate " + cratename + " done");
            }
        }
    }
    current_config_state_=2;
    //
  }

  void EmuPeripheralCrateService::CheckCrates(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
  {  
    ParsingXML();

    if(GuiButton_)
    {   
        msgHandler("Button:  Check Crate Controllers");
        int getlock=PCsendCommand("Locked", "emu::pc::EmuPeripheralCrateTimer");
        if(getlock>0)
           check_controllers();
        else
           msgHandler("ERROR: Xmas not responding");
        PCsendCommand("Unlock", "emu::pc::EmuPeripheralCrateTimer");
    }
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
     ParsingXML();

     if(GuiButton_)
     {
        msgHandler("Button:  Hard-Reset Crate " + ThisCrateID_);
 
        if(!Simulation_) thisCCB->hardReset();
     }
     this->Default(in,out);
  }

  void EmuPeripheralCrateService::ChamberOff(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
     ParsingXML();

     if(GuiButton_)
     {
        msgHandler("Button:  Power Off Chambers Crate " + ThisCrateID_);
 
        if(!Simulation_) thisCrate->PowerOff();
     }
     this->Default(in,out);
  }

bool EmuPeripheralCrateService::ParsingXML(bool reload)
{
  if( parsed==0 || (reload && Xml_or_Db() == -1))
  {
    std::string config_src, config_key;
    //
    // Logger logger_ = getApplicationLogger();
    //
    std::cout <<  "EmuPeripheralCrateService reloading..." << std::endl;
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
  std::string myUrl = getApplicationDescriptor()->getContextDescriptor()->getURL();
  std::string myUrn = getApplicationDescriptor()->getURN().c_str();
  main_url_ = myUrl + "/" + myUrn + "/MainPage";
  *out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl
       << "<?xml-stylesheet type=\"text/xml\" href=\"/emu/base/html/EmuPage1_XSL.xml\"?>" << std::endl
       << "<ForEmuPage1 application=\"" << getApplicationDescriptor()->getClassName()
       <<                   "\" url=\"" << getApplicationDescriptor()->getContextDescriptor()->getURL()
       <<         "\" localDateTime=\"" << getLocalDateTime() << "\">" << std::endl;

    *out << "  <monitorable name=\"" << "title"
         <<            "\" value=\"" << "Blue Page " + (std::string)((endcap_side==1)?"Plus":"Minus")
         <<  "\" nameDescription=\"" << " "
         << "\" valueDescription=\"" << "click this to access the PCrate Blue Page "
         <<          "\" nameURL=\"" << " "
         <<         "\" valueURL=\"" << main_url_
         << "\"/>" << std::endl;

    *out << "  <monitorable name=\"" << "VME Access"
         <<            "\" value=\"" << "Enabled"
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

   ParsingXML();

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
  else if (command_name=="CHAMBEROFF")
  {
     for ( unsigned int i = 0; i < crateVector.size(); i++ )
     {
        if(command_argu==crateVector[i]->GetLabel()) 
        {  
           if(!Simulation_) crateVector[i]->PowerOff();
           *out << "Switch Off Successful " <<  command_argu << std::endl;
           msgHandler("Message: Switch Off Chambers " + command_argu);
        }
     }
  }
  else if (command_name=="RELOAD")
  {
     ParsingXML(true);
     msgHandler("Message: Check Configuration DB, auto reload if needed ");
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
        {  
           int rt=0;
           if(!Simulation_) rt=crateVector[i]->configure(2);
           if(rt==0)
           {   *out << "Power Up Successful " <<  command_argu << std::endl;
               msgHandler("Message: Power-Up-Init Crate " + command_argu + " done");
               crate_state[i] = 1;
           } else
           {   *out << "FAILED Power Up " <<  command_argu << " with code: " << rt << std::endl;
               msgHandler("Message: Power-Up-Init Crate " + command_argu + " FAILED");
               crate_state[i] = 0;
           }
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

void EmuPeripheralCrateService::FlashHistory(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
     ParsingXML();

     if(GuiButton_)
     {
        msgHandler("Button:  Read Flash History");
        myTStore = GetEmuTStore();
        if(!myTStore)
        {  std::cout << "Can't create object TStoreReadWriter" << std::endl;
           *out << "Can't create object TStoreReadWriter" << std::endl;
           return;
        }
//        *out << cgicc::textarea().set("name","commands").set("WRAP","OFF").set("rows","20").set("cols","100");
        std::vector<std::pair< std::string, std::string> > flash_list;
        flash_list = myTStore->readFlashList((endcap_side==1)?"plus":"minus");
        *out << "  key        time" << std::endl;

        for(unsigned int i=0;i<flash_list.size(); i++)
        {
            *out << flash_list[i].first+"  "+flash_list[i].second.substr(0,19) << std::endl;
        }
        *out << "Total in FLASH " << flash_list.size() << std::endl;
//        *out << cgicc::textarea();
        flash_list.clear();
     }
}

 }  // namespace emu::pc
}  // namespace emu

XDAQ_INSTANTIATOR_IMPL(emu::pc::EmuPeripheralCrateService)
