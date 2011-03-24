#include "emu/pc/EmuPeripheralCrateConfig.h"

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
//
/////////////////////////////////////////////////////////
// CCB utilities
/////////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::CCBStatus(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  char Name[100];
  sprintf(Name,"CCB status, crate=%s, slot=%d",ThisCrateID_.c_str(),thisCCB->slot());
  //
  MyHeader(in,out,Name);
  //
  *out << cgicc::h1(Name);
  *out << cgicc::br();
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << std::endl;
  //
  *out << cgicc::legend("CCB Info").set("style","color:blue") << std::endl ;
  //
  thisCCB->RedirectOutput(out);
  //
  if ( thisCCB->CheckFirmwareDate() ) {
    //
    *out << cgicc::span().set("style","color:green");
    thisCCB->printFirmwareVersion();
    *out << "...OK...";
    *out << cgicc::span();
    //
  } else {
    //
    *out << cgicc::span().set("style","color:red");
    thisCCB->printFirmwareVersion();
    *out << " --->>  BAD  <<--- Should be (" 
	 << std::dec << thisCCB->GetExpectedFirmwareDay() 
	 << "-"      << thisCCB->GetExpectedFirmwareMonth()
	 << "-"      << thisCCB->GetExpectedFirmwareYear()
	 << ")";

    *out << cgicc::span();
  }
  *out << cgicc::br();
  //
  thisCCB->ReadTTCrxID();
  int readValue   = thisCCB->GetReadTTCrxID();
  int configValue = thisCCB->GetTTCrxID();
  //
  char buf[50];
  sprintf(buf,"Onboard TTCrx ID = %d",readValue);
  //
  if ( readValue == configValue ) {
    *out << cgicc::span().set("style","color:green");
    *out << buf;
    *out << cgicc::span();
  } else {
    *out << cgicc::span().set("style","color:red");
    *out << buf;
    *out << " (TTCrxID value in configuration incorrectly set to " << std::dec << configValue << ") ";
    *out << cgicc::span();
  }
  //
  thisCCB->RedirectOutput(&std::cout);
  //
  *out << cgicc::br() << "CCB Mode = ";
  int ccb_mode = thisCCB->GetCCBmode();
  switch(ccb_mode) {
  case CCB::TTCrqFPGA:
    *out << "TTCrgFPGA" << std::endl;
    break;
  case CCB::VMEFPGA:
    *out << "VMEFPGA" << std::endl;
    break;
  case CCB::DLOG:
    *out << "DLOG" << std::endl;
    break;
  default:
    *out << "unknown" << std::endl;
    break;
  }
  //
  *out << cgicc::br() << "CSRA1 =  " << std::hex << thisCCB->ReadRegister(0) << std::endl;
  *out << cgicc::br() << "CSRA2 =  " << std::hex << thisCCB->ReadRegister(2) << std::endl;
  *out << cgicc::br() << "CSRA3 =  " << std::hex << thisCCB->ReadRegister(4) << std::endl;
  *out << cgicc::br() << "CSRB1 =  " << std::hex << thisCCB->ReadRegister(0x20) << std::endl;
  *out << cgicc::br() << "CSRB18 = " << std::hex << thisCCB->ReadRegister(0x42) << std::endl;
  //
  *out << cgicc::fieldset();
  //
}
//

//
void EmuPeripheralCrateConfig::CCBUtils(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  char Name[100];
  char buf[200];
  sprintf(Name,"CCB utilities, crate=%s, slot=%d",ThisCrateID_.c_str(),thisCCB->slot());
  //
  MyHeader(in,out,Name);
  //
  *out << cgicc::h1(Name);
  *out << cgicc::br();
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << std::endl;
  //
  *out << cgicc::legend("CCB Utils").set("style","color:blue") << std::endl ;
  //
  std::string ReadCCBRegister = 
    toolbox::toString("/%s/ReadCCBRegister",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ReadCCBRegister) << std::endl ;
  *out << "Read Register (hex) " << std::endl;
  sprintf(buf, "%04X", CCBRegisterRead_);  
  *out << cgicc::input().set("type","text").set("value",buf).set("name","CCBRegister") << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Read CCB") << std::endl ;
  *out << " Register value (hex): " << std::hex << CCBRegisterValue_ << std::endl;
  *out << cgicc::form() << std::endl ;
  //
  std::string WriteCCBRegister = 
    toolbox::toString("/%s/WriteCCBRegister",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",WriteCCBRegister) << std::endl ;
  *out << "Write Register (hex) " << std::endl;
  sprintf(buf, "%04X", CCBRegisterWrite_);
  *out << cgicc::input().set("type","text").set("value",buf).set("name","CCBRegister") << std::endl ;
  *out << "Register value (hex) " << std::endl;
  sprintf(buf, "%04X", CCBWriteValue_);
  *out << cgicc::input().set("type","text").set("value",buf).set("name","CCBValue") << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Write CCB") << std::endl ;
  *out << cgicc::form() << cgicc::br() << std::endl ;
  //
  std::string ReadTTCRegister =
    toolbox::toString("/%s/ReadTTCRegister",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::a("[Read TTCrx Registers]").set("href",ReadTTCRegister).set("target","_blank") << std::endl;
  //
  *out << cgicc::br() << cgicc::br() << std::endl;
  std::string ccbConfig =
    toolbox::toString("/%s/CCBConfig",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ccbConfig) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","CCB Configure");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br() << std::endl;
  std::string HardReset =
    toolbox::toString("/%s/HardReset",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",HardReset) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","HardReset");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br();
  //
  std::string CCBLoadFirmware =
    toolbox::toString("/%s/CCBLoadFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CCBLoadFirmware) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Load CCB Firmware") << std::endl ;
  //    sprintf(buf,"%d",ccb);
  //    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << CCBFirmware_;
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::fieldset();
  //
}
//
  void EmuPeripheralCrateConfig::CCBLoadFirmware(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    int debugMode(0);
    int jch(6);
    int verify(1);
    std::cout << getLocalDateTime() << " Programming CCB using " << CCBFirmware_ << std::endl;
    int status = thisCCB->svfLoad(&jch,CCBFirmware_.c_str(),debugMode, verify);
    if (status >= 0){
      std::cout << getLocalDateTime() << " Programming finished with " << status << " Verify Errors occured" << std::endl;
    }
    else{
      std::cout << getLocalDateTime() << " Fatal Error. Exiting with " <<  status << std::endl;
    }
    //
    //    thisCCB->hardReset();
    //
    this->CCBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrateConfig::CCBConfig(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    thisCCB->configure();
    this->CCBUtils(in,out);
  }
  //
//////////////////////////////////////////////////
// MPC methods
//////////////////////////////////////////////////
void EmuPeripheralCrateConfig::MPCStatus(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  char Name[100];
  sprintf(Name,"MPC status, crate=%s, slot=%d",
	  ThisCrateID_.c_str(),thisMPC->slot());
  //
  MyHeader(in,out,Name);
  //
  *out << cgicc::h1(Name);
  *out << cgicc::br();
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  //
  *out << cgicc::legend("MPC Info").set("style","color:blue") << std::endl ;
  //
  thisMPC->RedirectOutput(out);
  //
  if ( thisMPC->CheckFirmwareDate() ) {
    //
    *out << cgicc::span().set("style","color:green");
    thisMPC->printFirmwareVersion();
    *out << "...OK...";
    *out << cgicc::span();
    //
  } else {
    //
    *out << cgicc::span().set("style","color:red");
    thisMPC->printFirmwareVersion();
    *out << " --->>  BAD  <<--- Should be (" 
	 << std::dec << thisMPC->GetExpectedFirmwareDay() 
	 << "-"      << thisMPC->GetExpectedFirmwareMonth()
	 << "-"      << thisMPC->GetExpectedFirmwareYear()
	 << ")";

    *out << cgicc::span();
  }
  *out << cgicc::br();
  //
  thisMPC->RedirectOutput(&std::cout);
  //
  *out << cgicc::br() << "CSR0 = " << std::hex << thisMPC->ReadRegister(0) << std::endl;
  *out << cgicc::br() << "CSR1 = " << std::hex << thisMPC->ReadRegister(0xAA) << std::endl;
  *out << cgicc::br() << "CSR2 = " << std::hex << thisMPC->ReadRegister(0xAC) << std::endl;
  *out << cgicc::br() << "CSR3 = " << std::hex << thisMPC->ReadRegister(0xAE) << std::endl;
  *out << cgicc::br() << "CSR4 = " << std::hex << thisMPC->ReadRegister(0xB8) << std::endl;
  //
  *out << cgicc::fieldset();
  //
}
//
void EmuPeripheralCrateConfig::MPCUtils(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  char Name[100];
  char buf[200];
  sprintf(Name,"MPC utilities, crate=%s, slot=%d",
	  ThisCrateID_.c_str(),thisMPC->slot());
  //
  MyHeader(in,out,Name);
  //
  *out << cgicc::h1(Name);
  *out << cgicc::br();
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl ;
  //
  *out << cgicc::legend("TMB Mask Control").set("style","color:blue") ;
  //
  *out << cgicc::table().set("border","1");
  //
  int power_state[10];
  int powermask = thisMPC->ReadMask();
  int power_read = powermask;
  for(int icc=0; icc<9; icc++)
  {   power_state[9-icc]= power_read & 1;
      power_read = power_read>>1;
  }
  for(int icc=1; icc<=9; icc++)
  {
     *out << cgicc::td();
     if(power_state[icc]==0)
        *out << cgicc::span().set("style","color:green");
     else if(power_state[icc]==1)
        *out << cgicc::span().set("style","color:red");
     else
        *out << cgicc::span().set("style","color:black");
     *out << "TMB " << icc;
     *out << cgicc::span() << cgicc::td();
  }
  *out << cgicc::tr();

  for(int icc=1; icc<=9; icc++)
  {
     *out << cgicc::td();
     if(power_state[icc]==0)
     {
        *out << "On";
     }
     else
     {
        std::string MPCMask = toolbox::toString("/%s/MPCMask",getApplicationDescriptor()->getURN().c_str());
        *out << cgicc::form().set("method","GET").set("action",MPCMask) << std::endl ;
        *out << cgicc::input().set("type","submit").set("value","Turn On") << std::endl ;
        sprintf(buf, "%d", powermask^(1<<(9-icc)) ); 
        *out << cgicc::input().set("type","hidden").set("value",buf).set("name","mask");
        *out << cgicc::form() << std::endl ;
     }
     *out << cgicc::td();
  }

  *out << cgicc::tr();

  for(int icc=1; icc<=9; icc++)
  {
     *out << cgicc::td();
     if(power_state[icc]==0)
     {
        std::string MPCMask = toolbox::toString("/%s/MPCMask",getApplicationDescriptor()->getURN().c_str());
        *out << cgicc::form().set("method","GET").set("action",MPCMask) << std::endl ;
        *out << cgicc::input().set("type","submit").set("value","Turn Off") << std::endl ;
        sprintf(buf, "%d", powermask|(1<<(9-icc)) );
        *out << cgicc::input().set("type","hidden").set("value",buf).set("name","mask");
        *out << cgicc::form() << std::endl ;
     }
     else
     {
        *out << "Off";
     }
     *out << cgicc::td();
  }
 // *out << cgicc::tr();
  *out << cgicc::table();
  //
  *out << cgicc::fieldset() << cgicc::br();
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  //
  *out << cgicc::legend("MPC Utils").set("style","color:blue") << std::endl ;
  //
  std::string ReadMPCRegister = 
    toolbox::toString("/%s/ReadMPCRegister",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ReadMPCRegister) << std::endl ;
  *out << "Read Register (hex) " << std::endl;
  sprintf(buf, "%04X", MPCRegisterRead_);  
  *out << cgicc::input().set("type","text").set("value",buf).set("name","MPCRegister") << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Read MPC") << std::endl ;
  *out << " Register value (hex): " << std::hex << MPCRegisterValue_ << std::endl;
  *out << cgicc::form() << std::endl ;
  //
  std::string WriteMPCRegister = 
    toolbox::toString("/%s/WriteMPCRegister",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",WriteMPCRegister) << std::endl ;
  *out << "Write Register (hex) " << std::endl;
  sprintf(buf, "%04X", MPCRegisterWrite_);
  *out << cgicc::input().set("type","text").set("value",buf).set("name","MPCRegister") << std::endl ;
  *out << "Register value (hex) " << std::endl;
  sprintf(buf, "%04X", MPCWriteValue_);
  *out << cgicc::input().set("type","text").set("value",buf).set("name","MPCValue") << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Write MPC") << std::endl ;
  *out << cgicc::form() << cgicc::br() << std::endl ;
  //
  *out << cgicc::br() << cgicc::br() << std::endl;
  std::string mpcConfig =
    toolbox::toString("/%s/MPCConfig",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",mpcConfig) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","MPC Configure");
  *out << cgicc::form() << cgicc::br() << std::endl ;
  //
  std::string MPCLoadFirmware = toolbox::toString("/%s/MPCLoadFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",MPCLoadFirmware) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Load MPC Firmware") << std::endl ;
  *out << MPCFirmware_;
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::fieldset();
  //
}
//
void EmuPeripheralCrateConfig::MPCLoadFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  int debugMode(0);
  int jch(6);
  int verify(1);
  std::cout << getLocalDateTime() << " Programming MPC using " << MPCFirmware_ << std::endl;
  int status = thisMPC->svfLoad(&jch,MPCFirmware_.c_str(),debugMode, verify);
  if (status >= 0){
    std::cout << getLocalDateTime() << " Programming finished with " << status << " Verify Errors occured" << std::endl;
  }
  else{
    std::cout << getLocalDateTime() << " Fatal Error. Exiting with " <<  status << std::endl;
  }
  //
  //thisCCB->hardReset();
  //
  this->MPCUtils(in,out);
  //
}

  void EmuPeripheralCrateConfig::MPCConfig(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    thisMPC->configure();
    this->MPCUtils(in,out);
  }

  //
  void EmuPeripheralCrateConfig::ReadCCBRegister(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name2 = cgi.getElement("CCBRegister");
    int CCBregister = -1;
    if(name2 != cgi.getElements().end()) {
      CCBregister = strtol(cgi["CCBRegister"]->getValue().c_str(),NULL,16);
    }
    if(CCBregister != -1)  
    {  CCBRegisterRead_ = CCBregister;
       std::cout << "CCB read Register: " << std::hex << CCBregister << std::dec << std::endl;
       CCBRegisterValue_ = thisCCB->ReadRegister(CCBregister);
    }    
    //
    this->CCBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrateConfig::WriteCCBRegister(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name2 = cgi.getElement("CCBRegister");
    cgicc::form_iterator value2 = cgi.getElement("CCBValue");
    int CCBregister = -1;
    int CCBvalue = -1;
    if(name2 != cgi.getElements().end()) {
      CCBregister = strtol(cgi["CCBRegister"]->getValue().c_str(),NULL,16);
    }
    if(value2 != cgi.getElements().end()) {
      CCBvalue = strtol(cgi["CCBValue"]->getValue().c_str(),NULL,16);
    }
    if( CCBregister != -1 && CCBvalue != -1)  
    {  CCBRegisterWrite_ = CCBregister;
       CCBWriteValue_ = CCBvalue;
       std::cout << "CCB write Register: " << std::hex << CCBregister
                 << ", Value: " << CCBvalue << std::dec << std::endl;
       thisCCB->WriteRegister(CCBregister, CCBvalue);
    }
    //
    this->CCBUtils(in,out);
    //
  }

  void EmuPeripheralCrateConfig::ReadTTCRegister(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
      std::ostringstream OutputTxt;
    //
      OutputTxt.str(""); //clear the output string
      thisCCB->RedirectOutput(&OutputTxt);
      thisCCB->PrintTTCrxRegs();
      thisCCB->RedirectOutput(&std::cout);
      *out << OutputTxt.str();
    //
  }
  //
  void EmuPeripheralCrateConfig::HardReset(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    std::cout << "hardReset" << std::endl;
    //
    thisCCB->hardReset();
    // thisCCB->HardReset_crate();
    //
    this->CCBUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrateConfig::ReadMPCRegister(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name2 = cgi.getElement("MPCRegister");
    int MPCregister = -1;
    if(name2 != cgi.getElements().end()) {
      MPCregister = strtol(cgi["MPCRegister"]->getValue().c_str(),NULL,16);
    }
    if(MPCregister != -1)  
    {  MPCRegisterRead_ = MPCregister;
       std::cout << "MPC read Register: " << std::hex << MPCregister << std::dec << std::endl;
       MPCRegisterValue_ = thisMPC->ReadRegister(MPCregister);
    }    
    //
    this->MPCUtils(in,out);
    //
  }
  //
  void EmuPeripheralCrateConfig::WriteMPCRegister(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name2 = cgi.getElement("MPCRegister");
    cgicc::form_iterator value2 = cgi.getElement("MPCValue");
    int MPCregister = -1;
    int MPCvalue = -1;
    if(name2 != cgi.getElements().end()) {
      MPCregister = strtol(cgi["MPCRegister"]->getValue().c_str(),NULL,16);
    }
    if(value2 != cgi.getElements().end()) {
      MPCvalue = strtol(cgi["MPCValue"]->getValue().c_str(),NULL,16);
    }
    if( MPCregister != -1 && MPCvalue != -1)  
    {  MPCRegisterWrite_ = MPCregister;
       MPCWriteValue_ = MPCvalue;
       std::cout << "MPC write Register: " << std::hex << MPCregister
                 << ", Value: " << MPCvalue << std::dec << std::endl;
       thisMPC->WriteRegister(MPCregister, MPCvalue);
    }
    //
    this->MPCUtils(in,out);
    //
  }
//
void EmuPeripheralCrateConfig::MPCMask(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("mask");
  //
  int mask=0;
  if(name != cgi.getElements().end()) {
    mask = cgi["mask"]->getIntegerValue();
    // std::cout << "MPCMask: " << mask << std::endl;
  }
  std::cout << "MPC Mask set to " << std::hex << mask << std::dec << std::endl;
  mask = mask & 0x1FF;
  thisMPC->WriteMask(mask);
  //
  this->MPCUtils(in,out);
  //
}

 }  // namespace emu::pc
}  // namespace emu
