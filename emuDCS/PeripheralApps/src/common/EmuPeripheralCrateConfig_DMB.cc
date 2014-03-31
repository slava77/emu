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

//////////////////////////////////////////////////////////////
// DMB tests
//////////////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::DMBTests(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  int dmb;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "DMB tests: DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "DMB tests: No dmb" << std::endl ;
    dmb = DMB_;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  Chamber * thisChamber = chamberVector[dmb];
  //
  char Name[100];
  sprintf(Name,"%s DMB tests, crate=%s, slot=%d",(thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),thisDMB->slot());
  //
  MyHeader(in,out,Name);
  //
  char buf[200] ;
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl ;
  //
  *out << cgicc::legend("DMB Tests").set("style","color:blue") ;
  //
  std::string DMBTestAll = toolbox::toString("/%s/DMBTestAll",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBTestAll) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","DMB Test All ") << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  //
  *out << cgicc::table().set("border","0");
  //
  ////////////////////////////////////////////
  *out << cgicc::td();
  std::string DMBTest3 = toolbox::toString("/%s/DMBTest3",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBTest3) << std::endl ;
  if( thisDMB->GetTestStatus(3) == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","DMB Test3 (Check DMB Fifos)").set("style","color:blue" ) << std::endl ;
    //
  } else if( thisDMB->GetTestStatus(3) == 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","DMB Test3 (Check DMB Fifos)").set("style","color:green") << std::endl ;
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","DMB Test3 (Check DMB Fifos)").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td();
  std::string DMBTest4 = toolbox::toString("/%s/DMBTest4",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBTest4) << std::endl ;
  if( thisDMB->GetTestStatus(4) == -1 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test4 (Check voltages)").set("style","color:blue" ) << std::endl ;
  } 
  if( thisDMB->GetTestStatus(4) > 0 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test4 (Check voltages)").set("style","color:red"  ) << std::endl ;
  }
  if( thisDMB->GetTestStatus(4) ==0 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test4 (Check voltages)").set("style","color:green") << std::endl ;
  }
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td();
  std::string DMBTest5 = toolbox::toString("/%s/DMBTest5",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBTest5) << std::endl ;
  if( thisDMB->GetTestStatus(5) == -1 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test5 (Check Power Register)").set("style","color:blue" ) << std::endl ;
  }
  if( thisDMB->GetTestStatus(5) > 0 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test5 (Check Power Register)").set("style","color:red"  ) << std::endl ;
  }
  if( thisDMB->GetTestStatus(5) ==0 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test5 (Check Power Register)").set("style","color:green") << std::endl ;
  }
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td();
  std::string DMBTest6 = toolbox::toString("/%s/DMBTest6",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBTest6) << std::endl ;
  if( thisDMB->GetTestStatus(6) == -1 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test6 (Check FPGA IDs)").set("style","color:blue" ) << std::endl ;
  }
  if( thisDMB->GetTestStatus(6) > 0 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test6 (Check FPGA IDs)").set("style","color:red"  ) << std::endl ;
  }
  if( thisDMB->GetTestStatus(6) ==0 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test6 (Check FPGA IDs)").set("style","color:green") << std::endl ;
  }
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  /////////////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td();
  std::string DMBTest8 = toolbox::toString("/%s/DMBTest8",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBTest8) << std::endl ;
  if( thisDMB->GetTestStatus(8) == -1 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test8 (Check Comparator DAC/ADC)").set("style","color:blue" ) << std::endl ;
  }
  if( thisDMB->GetTestStatus(8) > 0 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test8 (Check Comparator DAC/ADC)").set("style","color:red"  ) << std::endl ;
  }
  if( thisDMB->GetTestStatus(8) ==0 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test8 (Check Comparator DAC/ADC)").set("style","color:green") << std::endl ;
  }
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td();
  std::string DMBTest9 = toolbox::toString("/%s/DMBTest9",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBTest9) << std::endl ;
  if( thisDMB->GetTestStatus(9) == -1 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test9 (Check CDAC)").set("style","color:blue" ) << std::endl ;
  }
  if( thisDMB->GetTestStatus(9) > 0 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test9 (Check CDAC)").set("style","color:red"  ) << std::endl ;
  }
  if( thisDMB->GetTestStatus(9) ==0 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test9 (Check CDAC)").set("style","color:green") << std::endl ;
  }
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td();
  std::string DMBTest10 = toolbox::toString("/%s/DMBTest10",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBTest10) << std::endl ;
  if( thisDMB->GetTestStatus(10) == -1 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test10 (Check SFM)").set("style","color:blue" ) << std::endl ;
  }
  if( thisDMB->GetTestStatus(10) > 0 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test10 (Check SFM)").set("style","color:red"  ) << std::endl ;
  }
  if( thisDMB->GetTestStatus(10) ==0 ) {
    *out << cgicc::input().set("type","submit").set("value","DMB Test10 (Check SFM)").set("style","color:green") << std::endl ;
  }
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::tr();   //change line
  //
  *out << cgicc::td();
  std::string RTRGlow = toolbox::toString("/%s/RTRGlow",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",RTRGlow) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Toggle Random Trigger Low").set("style","color:green") << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td();
  std::string RTRGhigh = toolbox::toString("/%s/RTRGhigh",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",RTRGhigh) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Toggle Random Trigger High").set("style","color:green") << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //

  //
  /*
   *out << cgicc::td();
   std::string DMBTest11 = toolbox::toString("/%s/DMBTest11",getApplicationDescriptor()->getURN().c_str());
   *out << cgicc::form().set("method","GET").set("action",DMBTest11) << std::endl ;
   if( thisDMB->GetTestStatus(11) == -1 ) {
   *out << cgicc::input().set("type","submit").set("value","DMB Test11 (Check buckflash)").set("style","color:blue" ) << std::endl ;
   }
   if( thisDMB->GetTestStatus(11) > 0 ) {
   *out << cgicc::input().set("type","submit").set("value","DMB Test11 (Check buckflash)").set("style","color:red"  ) << std::endl ;
   }
   if( thisDMB->GetTestStatus(11) ==0 ) {
   *out << cgicc::input().set("type","submit").set("value","DMB Test11 (Check buckflash)").set("style","color:green") << std::endl ;
   }
   sprintf(buf,"%d",dmb);
   *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
   *out << cgicc::form() << std::endl ;
   *out << cgicc::td();
   */
  //
  *out << cgicc::table();
  //
  *out << cgicc::fieldset() << std::endl;
  //
  *out << cgicc::form().set("method","GET") << std::endl ;
  *out << cgicc::textarea().set("name","CrateTestDMBOutput").set("WRAP","OFF").set("rows","20").set("cols","60");
  *out << OutputDMBTests[dmb][current_crate_].str() << std::endl ;
  *out << cgicc::textarea();
  *out << cgicc::form();
  //
  std::string method = toolbox::toString("/%s/LogDMBTestsOutput",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",method) << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::input().set("type","submit").set("value","Log output").set("name","LogDMBTestsOutput") << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Clear").set("name","ClearDMBTestsOutput") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  //  std::cout << "Done" << std::endl;
}

//
void EmuPeripheralCrateConfig::DMBTestAll(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
    //
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  OutputDMBTests[dmb][current_crate_] << "DMB TestAll" << std::endl ;
  //
  thisDMB->RedirectOutput(&OutputDMBTests[dmb][current_crate_]);
  thisDMB->test3();
  thisDMB->test4();
  thisDMB->test5();
  thisDMB->test6();
  thisDMB->test8();
  thisDMB->test9();
  thisDMB->test10();
  thisDMB->RedirectOutput(&std::cout);
  //
  this->DMBTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::DMBTest3(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "Test3 DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  OutputDMBTests[dmb][current_crate_] << "DMB Test3" << std::endl ;
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  thisDMB->RedirectOutput(&OutputDMBTests[dmb][current_crate_]);
  thisDMB->test3();
  thisDMB->RedirectOutput(&std::cout);
  //
  this->DMBTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::DMBTest4(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception){
    //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "Test4 DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  OutputDMBTests[dmb][current_crate_] << "DMB Test4" << std::endl ;
  //
  thisDMB->RedirectOutput(&OutputDMBTests[dmb][current_crate_]);
  thisDMB->test4();
  thisDMB->RedirectOutput(&std::cout);
  //
  this->DMBTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::DMBTest5(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "Test5 DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  OutputDMBTests[dmb][current_crate_] << "DMB Test5" << std::endl ;
  //
  thisDMB->RedirectOutput(&OutputDMBTests[dmb][current_crate_]);
  thisDMB->test5();
  thisDMB->RedirectOutput(&std::cout);
  //
  this->DMBTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::DMBTest6(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "Test6 DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  OutputDMBTests[dmb][current_crate_] << "DMB Test6" << std::endl ;
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  thisDMB->RedirectOutput(&OutputDMBTests[dmb][current_crate_]);
  thisDMB->test6();
  thisDMB->RedirectOutput(&std::cout);
  //
  this->DMBTests(in,out);
  //
}
//
void EmuPeripheralCrateConfig::DMBTest8(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "Test8 DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  OutputDMBTests[dmb][current_crate_] << "DMB Test8" << std::endl ;
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  thisDMB->RedirectOutput(&OutputDMBTests[dmb][current_crate_]);
  thisDMB->test8();
  thisDMB->RedirectOutput(&std::cout);
  //
  this->DMBTests(in,out);
}
//
void EmuPeripheralCrateConfig::DMBTest9(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
    //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "Test9 DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  OutputDMBTests[dmb][current_crate_] << "DMB Test9" << std::endl ;
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  thisDMB->RedirectOutput(&OutputDMBTests[dmb][current_crate_]);
  thisDMB->test9();
  thisDMB->RedirectOutput(&std::cout);
  //
  this->DMBTests(in,out);
}
//
void EmuPeripheralCrateConfig::DMBTest10(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "Test10 DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  OutputDMBTests[dmb][current_crate_] << "DMB Test10" << std::endl ;
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  thisDMB->RedirectOutput(&OutputDMBTests[dmb][current_crate_]);
  thisDMB->test10();
  thisDMB->RedirectOutput(&std::cout);
  //
  this->DMBTests(in,out);
}
//
//
void EmuPeripheralCrateConfig::RTRGlow(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "Toggle Randowm Trigger Low, DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  OutputDMBTests[dmb][current_crate_] << "Toggle Random Trigger Low" << std::endl ;
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  thisDMB->RedirectOutput(&OutputDMBTests[dmb][current_crate_]);

  // load random trigger rate (10 KHz per CFEBs and L1A) --> 
  thisDMB->set_rndmtrg_rate(0x9249);
  thisDMB->toggle_rndmtrg_start();

  thisDMB->RedirectOutput(&std::cout);
  //
  this->DMBTests(in,out);
}
//
//
void EmuPeripheralCrateConfig::RTRGhigh(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "Toggle Randowm Trigger High, DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  OutputDMBTests[dmb][current_crate_] << "Toggle Random Trigger High" << std::endl ;
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  thisDMB->RedirectOutput(&OutputDMBTests[dmb][current_crate_]);

  // load random trigger rate (10 KHz per CFEBs and L1A) --> 
  thisDMB->set_rndmtrg_rate(0x2db6d);
  thisDMB->toggle_rndmtrg_start();

  thisDMB->RedirectOutput(&std::cout);
  //
  this->DMBTests(in,out);
}
//
void EmuPeripheralCrateConfig::DMBTest11(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "Test11 DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  OutputDMBTests[dmb][current_crate_] << "DMB Test11" << std::endl ;
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  thisDMB->RedirectOutput(&std::cout);
  thisDMB->test11();
  thisDMB->RedirectOutput(&std::cout);
  //
  this->DMBTests(in,out);
}

  //
  void EmuPeripheralCrateConfig::LogDMBTestsOutput(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    std::cout << "LogDMBTestsOutput" << std::endl;
    //
    cgicc::Cgicc cgi(in);
    //
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    //
    int dmb=0;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      std::cout << "DMB " << dmb << std::endl;
      DMB_ = dmb;
    } else {
      std::cout << "Not dmb" << std::endl ;
      dmb = DMB_;
    }
    //
    cgicc::form_iterator name2 = cgi.getElement("ClearDMBTestsOutput");
    //
    if(name2 != cgi.getElements().end()) {
      std::cout << "Clear..." << std::endl;
      std::cout << cgi["ClearDMBTestsOutput"]->getValue() << std::endl ;
      OutputDMBTests[dmb][current_crate_].str("");
      OutputDMBTests[dmb][current_crate_] << "DMB-CFEB Tests " 
					  << thisCrate->GetChamber(dmbVector[dmb]->slot())->GetLabel().c_str() 
					  << " output:" << std::endl;

      //
    this->DMBTests(in,out);
    return;
    }
    //
    DAQMB * thisDMB = dmbVector[dmb];
    //
    char buf[100];
    sprintf(buf,"/tmp/DMBTestsLogFile_%d.log",thisDMB->slot());
    //
    std::ofstream DMBTestsLogFile;
    DMBTestsLogFile.open(buf);
    DMBTestsLogFile << OutputDMBTests[dmb][current_crate_].str() ;
    DMBTestsLogFile.close();
    //
    OutputDMBTests[dmb][current_crate_].str("");
    //
    this->DMBTests(in,out);
    //
  }

//
///////////////////////////////////////////////////////////
// DMB utilities
///////////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::CFEBStatus(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "Not dmb" << std::endl ;
    dmb = DMB_;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];
  Chamber * thisChamber = chamberVector[dmb];
  //
  char Name[100];
  sprintf(Name,"%s CFEB status, crate=%s, DMBslot=%d",
	  (thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),thisDMB->slot());
  //
  MyHeader(in,out,Name);
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << std::endl;
  //
  *out << cgicc::legend("CFEB ID Readback").set("style","color:blue") << std::endl ;
  //
  char buf[200];
  int hversion, cfeb_index, ndcfebs=0;
  //
  std::vector<CFEB> cfebs = thisDMB->cfebs() ;
  //
  typedef std::vector<CFEB>::iterator CFEBItr;
  //
  int donebits=thisDMB->read_cfeb_done();
  int cfebdone=0;

  for(CFEBItr cfebItr = cfebs.begin(); cfebItr != cfebs.end(); ++cfebItr) {
    hversion=cfebItr->GetHardwareVersion();    //
    cfeb_index = (*cfebItr).number() + 1;
    //
  if(hversion <=1)
  {
    sprintf(buf,"CFEB %d : ",cfeb_index);
    *out << buf;
    //
    //*out << cgicc::br();
    //
    sprintf(buf,"CFEB prom user id : %08x;  CFEB fpga user id : %08x ",
	    thisDMB->febpromuser(*cfebItr),
	    thisDMB->febfpgauser(*cfebItr));
    //
    if ( thisDMB->CheckCFEBFirmwareVersion(*cfebItr) ) {
      *out << cgicc::span().set("style","color:green");
      *out << buf;
      *out << cgicc::span();
    } else {
      *out << cgicc::span().set("style","color:red");
      *out << buf;
      *out << " (Should be 0x" << std::hex << thisDMB->GetExpectedCFEBFirmwareTag(cfeb_index-1) << ") " << std::dec;
      *out << cgicc::span();
    }
  } else if(hversion==2)
  {
    cfebdone=(donebits>>(cfeb_index-1))&1;
    ndcfebs++;
    sprintf(buf,"DCFEB %d : ",cfeb_index);
    *out << buf;
    //
    //*out << cgicc::br();
    //
    sprintf(buf,"DCFEB FPGA DONE: %d;  FPGA id : %08x;  FPGA user code: %08x ",
            cfebdone,
	    thisDMB->febfpgaid(*cfebItr),
	    thisDMB->febfpgauser(*cfebItr));
    //
    if ( cfebdone && thisDMB->CheckCFEBFirmwareVersion(*cfebItr) ) {
      *out << cgicc::span().set("style","color:green");
      *out << buf;
      *out << cgicc::span();
    } else {
      *out << cgicc::span().set("style","color:red");
      *out << buf;
      *out << " (Should be 0x" << std::hex << thisDMB->GetExpectedCFEBFirmwareTag(cfeb_index-1) << ") " << std::dec;
      *out << cgicc::span();
    }
  }
    //
    *out << cgicc::br();
    //
  }
  //
  *out << cgicc::fieldset();
  //
  // section for DCFEB only
  if(ndcfebs>0)
  {
     unsigned short dcfeb_par[7][6];
     std::string color[7][6];
     std::vector<std::string> parname;
     parname.clear();
     parname.push_back("comp mode");
     parname.push_back("comp timing");
     parname.push_back("comp clock phase");
     parname.push_back("sample clock phase");
     parname.push_back("pipeline depth");
     parname.push_back("number of samples");

     *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
     *out << std::endl;
     //
     *out << cgicc::legend("DCFEB Virtex-6 Parameters").set("style","color:blue") << std::endl ;
     *out << cgicc::table().set("border","1");
     *out <<cgicc::td() << "Channel" << cgicc::td();
     for(CFEBItr cfebItr = cfebs.begin(); cfebItr != cfebs.end(); ++cfebItr)
     {
       unsigned i = cfebItr - cfebs.begin();
       char wrd[2];
       *out << cgicc::td() << "CFEB " << (cfebItr - cfebs.begin() + 1) << cgicc::td();
       thisDMB->write_cfeb_selector(cfebItr->SelectorBit());

       thisDMB->autoload_select_readback_wrd(*cfebItr,2);
       thisDMB->autoload_readback_wrd(*cfebItr,wrd);
       dcfeb_par[i][0] = wrd[0] & 0x3;
       dcfeb_par[i][1] = (wrd[0] & 0xc) >> 2;

       thisDMB->autoload_select_readback_wrd(*cfebItr,10);
       thisDMB->autoload_readback_wrd(*cfebItr,wrd);
       dcfeb_par[i][2] = wrd[0];

       thisDMB->autoload_select_readback_wrd(*cfebItr,11);
       thisDMB->autoload_readback_wrd(*cfebItr,wrd);
       dcfeb_par[i][3] = wrd[0];

       thisDMB->autoload_select_readback_wrd(*cfebItr,6);
       thisDMB->autoload_readback_wrd(*cfebItr,wrd);
       dcfeb_par[i][4] = wrd[0];

       thisDMB->autoload_select_readback_wrd(*cfebItr,8);
       thisDMB->autoload_readback_wrd(*cfebItr,wrd);
       dcfeb_par[i][5] = wrd[0];

       color[i][0] = dcfeb_par[i][0] == thisDMB->GetCompModeCfeb(i) ? "green" : "red";
       color[i][1] = dcfeb_par[i][1] == thisDMB->GetCompTimingCfeb(i) ? "green" : "red";
       color[i][2] = dcfeb_par[i][2] == thisDMB->GetCompClockPhaseCfeb(i) ? "green" : "red";
       color[i][3] = dcfeb_par[i][3] == thisDMB->GetADCSampleClockPhaseCfeb(i) ? "green" : "red";
       color[i][4] = dcfeb_par[i][4] == cfebItr->GetPipelineDepth() ? "green" : "red";
       color[i][5] = dcfeb_par[i][5] == thisDMB->GetNSamplesCfeb(i) ? "green" : "red";
     }
     *out << cgicc::tr() << cgicc::tr() << std::endl;
     for (unsigned par=0; par<6; par++)
     {
       *out << cgicc::td() << parname[par] << cgicc::td();
       for(CFEBItr cfebItr = cfebs.begin(); cfebItr != cfebs.end(); ++cfebItr)
       {
         unsigned i = cfebItr - cfebs.begin ();
         *out << cgicc::td() << cgicc::span().set("style","color:"+color[i][par]);
         *out << ((int) dcfeb_par[i][par]);
         *out << cgicc::span() << cgicc::td();
       }
       *out << cgicc::tr() << cgicc::tr() << std::endl;
     }
     *out << cgicc::table() << cgicc::fieldset() << std::endl;
     //
     double monitor_dcfebs[200];
     for(unsigned c=0; c<200; c++) monitor_dcfebs[c] = -1.0;
     std::vector<std::string> chname;
     chname.clear();
     chname.push_back("Temp(FPGA) (C)");
     chname.push_back("Vcc_in (V)");
     chname.push_back("Vcc_aux (V)");
     chname.push_back("DV4P_3_CUR (A)");
     chname.push_back("DV3P_2_CUR (A)");
     chname.push_back("DV3P_25_CUR (A)");
     chname.push_back("V3P_DCOMP (V)");
     chname.push_back("AV54P_3_CUR (A)");
     chname.push_back("AV54P_5_CUR (A)");
     chname.push_back("V3PIO (V)");
     chname.push_back("N.A.");
     chname.push_back("V25IO (V)");
     chname.push_back("V5PACOMP (V)");
     chname.push_back("V5PAMP (V)");
     chname.push_back("V18PDAC (V)");
     chname.push_back("N.A.");
     chname.push_back("V33PAADC (V)");
     chname.push_back("V5PPA (V)");
     chname.push_back("V5PSUB (V)");
     chname.push_back("Temp(PCB 1) (C)");
     chname.push_back("Temp(PCB 2) (C)");
     chname.push_back("Comparator DAC");

     *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
     *out << std::endl;
     //
     *out << cgicc::legend("DCFEB Virtex-6 Monitoring").set("style","color:blue") << std::endl ;
     for(CFEBItr cfebItr = cfebs.begin(); cfebItr != cfebs.end(); ++cfebItr) 
     {
        hversion=cfebItr->GetHardwareVersion();    //
        cfeb_index = (*cfebItr).number();
        if(hversion ==2)
        {
           std::vector<float> mon=thisDMB->dcfeb_fpga_monitor(*cfebItr);
           std::vector<float> dadc=thisDMB->dcfeb_adc(*cfebItr);
           for (unsigned c=0; c<mon.size() && c<19; c++) monitor_dcfebs[cfeb_index*22+c]=mon[c];
           monitor_dcfebs[cfeb_index*22+19]=dadc[3];
           monitor_dcfebs[cfeb_index*22+20]=dadc[4];
           monitor_dcfebs[cfeb_index*22+21]=dadc[0];
        }
     }
     *out << cgicc::table().set("border","1");
     //
     *out <<cgicc::td() << "Channel" << std::setprecision(3)<< cgicc::td();
     for(int ch=0; ch<23; ch++)
     {
       if(ch) *out << cgicc::td() << chname[ch-1] << cgicc::td();
       for(int feb=0; feb<7; feb++)
       {
          if(ch==0) *out << cgicc::td() << "CFEB " << feb+1 << cgicc::td();
          else
          {   *out << cgicc::td();
              if( monitor_dcfebs[feb*22+ch-1]>=0.) *out << monitor_dcfebs[feb*22+ch-1];
              *out << cgicc::td();
          }
       }
     *out << cgicc::tr() << cgicc::tr() << std::endl;
     }
     *out << cgicc::table() << std::setprecision(5) << std::endl;
     
     //
     *out << cgicc::fieldset();

  }
}

void EmuPeripheralCrateConfig::CFEBUtils(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::vector <std::string> FuncName; 
  char Name[100];
  FuncName.push_back("NO OP");
  FuncName.push_back("Sys Reset");
  FuncName.push_back("JTAG Shift Reg (32)"); 
  FuncName.push_back("Update Status Reg (32)"); 
  FuncName.push_back("Comparator DAC (15)"); 
  FuncName.push_back("L1a Delay (2)"); 
  FuncName.push_back("Read FIFO1"); 
  FuncName.push_back("Set F5 F8 F9"); 
  FuncName.push_back("Pre Blockend (4)"); 
  FuncName.push_back("Comparator mode timing (5)"); 
  FuncName.push_back("Buckeye mask (6)"); 
  FuncName.push_back("Shift Buckeye (6*48)");
  FuncName.push_back("ADC mask (12)");
  FuncName.push_back("Initial ADC");
  FuncName.push_back("ADC config memory (26)");
  FuncName.push_back("restart pipeline");
  FuncName.push_back("pipeline depth (9)");
  FuncName.push_back("TTC source (2)");
  FuncName.push_back("Set Calibration to Ext");
  FuncName.push_back("Set Calibration to Int");   
  FuncName.push_back("Samples to read (7)");
  FuncName.push_back("write BPI FIFO (16)");
  FuncName.push_back("read BPI FIFO (16)");
  FuncName.push_back("read BPI status (16)");
  FuncName.push_back("read BPI timer (16)");
  FuncName.push_back("Reset BPI");
  FuncName.push_back("Disable BPI");
  FuncName.push_back("Enable BPI");
  FuncName.push_back("Clock phase (4)");
  FuncName.push_back("TMB transmit mode (2)");
  FuncName.push_back("TMB Half Strip");  
  FuncName.push_back("TMB Layer Mask");  
  FuncName.push_back("DAQ low");  
  FuncName.push_back("DAQ High");  
  FuncName.push_back("Calib DAC (16)");  
  FuncName.push_back("ADC Control (25)");  
  FuncName.push_back("ADC Read (16)");  

  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "Not dmb" << std::endl ;
    dmb = DMB_;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];
  Chamber * thisChamber = chamberVector[dmb];
  //
  sprintf(Name,"%s CFEB Utilities, crate=%s, DMBslot=%d",
	  (thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),thisDMB->slot());
  //
  MyHeader(in,out,Name);

  CFEBDataIn_ = 0;
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("DCFEB JTAG Functions").set("style","color:blue") << std::endl ;
  //

  std::vector<CFEB> cfebs = thisDMB->cfebs() ;
  //
  //
  std::string dmbstring =
      toolbox::toString("%d",dmb);
  std::string CFEBFunct =
      toolbox::toString("/%s/CFEBFunction",getApplicationDescriptor()->getURN().c_str());

  *out << cgicc::h1("This is a debug tool for EXPERTS ONLY!!") << cgicc::br() << std::endl;
  
  // Begin select signal
  // Config listbox

  *out << cgicc::form().set("action", CFEBFunct) << std::endl;
  
  *out << "Choose CFEB: " << std::endl;
  *out << cgicc::select().set("name", "cfeb") << std::endl;
  
  char sbuf[50];
  for (unsigned i = 0; i < cfebs.size(); ++i) {
    sprintf(sbuf,"%d",i);
    if (i == 0) {
      *out << cgicc::option()
	.set("value", sbuf)
	.set("selected", "");
    } else {
      *out << cgicc::option()
	.set("value", sbuf);
    }
    *out << "CFEB " << cfebs[i].number()+1 << cgicc::option() << std::endl;
  }

  *out << cgicc::select() << std::endl;

  int n_keys = FuncName.size();
  int selected_index = 0;
  
  *out << "  Choose CFEB function: " << std::endl;
  *out << cgicc::select().set("name", "runtype") << std::endl;
  
  selected_index = 0;
  for (int i = 0; i < n_keys; ++i) {
    sprintf(sbuf,"%d",i);
    if (i == selected_index) {
      *out << cgicc::option()
	.set("value", sbuf)
	.set("selected", "");
    } else {
      *out << cgicc::option()
	.set("value", sbuf);
    }
    *out << FuncName[i] << cgicc::option() << std::endl;
  }

  *out << cgicc::select() << cgicc::br() << std::endl;

  *out << "CFEB Data in (hex) " << std::endl;
  sprintf(sbuf, "%016lX", CFEBDataIn_);
  *out << cgicc::input().set("type","text").set("value",sbuf).set("name","CFEBDataIn") << std::endl ;
  *out << cgicc::input().set("type","hidden").set("name","dmb").set("value",dmbstring) << std::endl ;          
  *out << cgicc::input().set("type", "submit")
    .set("name", "command")
    .set("value", "Start CFEB Function") << cgicc::br() << std::endl;
   *out << "    ======>> CFEB Data Out (hex): " << std::hex << CFEBDataOut_ << std::endl;
    
  *out << cgicc::form() << cgicc::br() << std::endl;
     
  //End select signal
    //
  *out << cgicc::fieldset()<< cgicc::br() << std::endl;
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("DCFEB Firmware").set("style","color:blue") << std::endl ;
  //
  std::string CFEBreadfirm =
      toolbox::toString("/%s/DCFEBReadFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("action", CFEBreadfirm) << std::endl;
  
  *out << "Choose CFEB: " << std::endl;
  *out << cgicc::select().set("name", "cfeb") << std::endl;
  
  for (unsigned i = 0; i < cfebs.size(); ++i) {
    sprintf(sbuf,"%d",i);
    if (i == 0) {
      *out << cgicc::option()
	.set("value", sbuf)
	.set("selected", "");
    } else {
      *out << cgicc::option()
	.set("value", sbuf);
    }
    *out << "CFEB " << cfebs[i].number()+1 << cgicc::option() << std::endl;
  }

  *out << cgicc::select() << std::endl;
  *out << cgicc::input().set("type","hidden").set("name","dmb").set("value",dmbstring) << std::endl ;          
  *out << cgicc::input().set("type", "submit")
    .set("name", "command")
    .set("value", "Read back DCFEB firmware") << std::endl;
  *out << cgicc::form() << cgicc::br() << std::endl;
  
  //
  std::string CFEBwritefirm =
      toolbox::toString("/%s/DCFEBProgramEprom",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("action", CFEBwritefirm) << std::endl;
  
  *out << "Choose CFEB: " << std::endl;
  *out << cgicc::select().set("name", "cfeb") << std::endl;
  
  for (unsigned i = 0; i < cfebs.size(); ++i) {
    sprintf(sbuf,"%d",i);
    if (i == 0) {
      *out << cgicc::option()
	.set("value", sbuf)
	.set("selected", "");
    } else {
      *out << cgicc::option()
	.set("value", sbuf);
    }
    *out << "CFEB " << cfebs[i].number()+1 << cgicc::option() << std::endl;
  }

  *out << cgicc::select() << std::endl;
  *out << cgicc::input().set("type","hidden").set("name","dmb").set("value",dmbstring) << std::endl ;          
  *out << cgicc::input().set("type", "submit")
    .set("name", "command")
    .set("value", "Program EPROM") << std::endl;
  *out << cgicc::form() << FirmwareDir_+"cfeb/me11_dcfeb.mcs" << cgicc::br() << cgicc::hr() << std::endl;
  
  std::string CFEBwritefirmall =
      toolbox::toString("/%s/DCFEBProgramEpromAll",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("action", CFEBwritefirmall) << std::endl;
  
  *out << cgicc::input().set("type","hidden").set("name","dmb").set("value",dmbstring) << std::endl ;          
  *out << cgicc::input().set("type", "submit")
    .set("name", "command")
    .set("value", "Broadcast Program EPROM - All CFEBs") << std::endl;
  *out << cgicc::form() << FirmwareDir_+"cfeb/me11_dcfeb.mcs" << cgicc::br() << cgicc::hr() << std::endl;
  
  std::string CFEBprogfpga =
      toolbox::toString("/%s/DCFEBProgramFpga",getApplicationDescriptor()->getURN().c_str());
  *out << "Use this one ONLY if power-cycle failed to recover the FPGA:" << cgicc::br()<< std::endl;
  *out << cgicc::form().set("action", CFEBprogfpga) << std::endl;
  
  *out << "Choose CFEB: " << std::endl;
  *out << cgicc::select().set("name", "cfeb") << std::endl;
  
  for (unsigned i = 0; i < cfebs.size(); ++i) {
    sprintf(sbuf,"%d",i);
    if (i == 0) {
      *out << cgicc::option()
	.set("value", sbuf)
	.set("selected", "");
    } else {
      *out << cgicc::option()
	.set("value", sbuf);
    }
    *out << "CFEB " << cfebs[i].number()+1 << cgicc::option() << std::endl;
  }

  *out << cgicc::select() << std::endl;
  *out << cgicc::input().set("type","hidden").set("name","dmb").set("value",dmbstring) << std::endl ;          
  *out << cgicc::input().set("type", "submit")
    .set("name", "command")
    .set("value", "Program FPGA") << std::endl;
  *out << cgicc::form() << FirmwareDir_+"cfeb/me11_dcfeb.mcs"  << cgicc::br() << std::endl;
    //
  *out << cgicc::fieldset()<< cgicc::br() << std::endl;
  //

}

void EmuPeripheralCrateConfig::CFEBFunction(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::vector <int> FuncSize;
  FuncSize.push_back(0);
  FuncSize.push_back(0);
  FuncSize.push_back(32);
  FuncSize.push_back(32);
  FuncSize.push_back(15);
  FuncSize.push_back(2);
  FuncSize.push_back(192);
  FuncSize.push_back(32);
  FuncSize.push_back(4);
  FuncSize.push_back(5);
  FuncSize.push_back(6);
  FuncSize.push_back(288);
  FuncSize.push_back(12);
  FuncSize.push_back(0);
  FuncSize.push_back(26);
  FuncSize.push_back(0);
  FuncSize.push_back(9);
  FuncSize.push_back(2);
  FuncSize.push_back(0);
  FuncSize.push_back(0);
  FuncSize.push_back(7);
  FuncSize.push_back(16);
  FuncSize.push_back(16);
  FuncSize.push_back(16);
  FuncSize.push_back(32);
  FuncSize.push_back(0);
  FuncSize.push_back(0);
  FuncSize.push_back(0);
  FuncSize.push_back(4);
  FuncSize.push_back(2);
  FuncSize.push_back(0);
  FuncSize.push_back(0);
  FuncSize.push_back(0);
  FuncSize.push_back(0);
  FuncSize.push_back(16);
  FuncSize.push_back(25);
  FuncSize.push_back(16);

  cgicc::Cgicc cgi(in);

  cgicc::form_iterator name = cgi.getElement("dmb");
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "Not dmb" << std::endl ;
    dmb = DMB_;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];

     std::string in_value = cgi.getElement("cfeb")->getValue(); 
     unsigned icfeb=atoi(in_value.c_str());
     in_value = cgi.getElement("runtype")->getValue(); 
     int sig=atoi(in_value.c_str());
     cgicc::form_iterator value2 = cgi.getElement("CFEBDataIn");  
     if(value2 != cgi.getElements().end()) 
        CFEBDataIn_ = strtol(cgi["CFEBDataIn"]->getValue().c_str(),NULL,16);

     std::vector<CFEB> cfebs = thisDMB->cfebs() ;
     if(icfeb<0 || icfeb>cfebs.size()) icfeb=0;

     std::cout << "call CFEB " << cfebs[icfeb].number()+1 << " with JTAG function: " << sig << std::endl;
     std::cout << "CFEB Data In: " << std::hex << CFEBDataIn_ << std::dec << std::endl;
     if(sig>0 && sig<=0x3F) std::cout << "data size is: " << FuncSize[sig] << std::endl;
     
     char inbuf[200], outbuf[200];
     for(int i=0;i<200;i++) { inbuf[i]=0; outbuf[i]=0;}
     memcpy(inbuf, &CFEBDataIn_, 8);
     thisDMB->dcfeb_hub(cfebs[icfeb], sig, FuncSize[sig], inbuf, outbuf, 3);
     memcpy(&CFEBDataOut_, outbuf, 8);

     std::cout << "CFEB Data Out: " << std::hex << CFEBDataOut_ << std::dec << std::endl;

     this->CFEBUtils(in,out);                               
     
}

void EmuPeripheralCrateConfig::DCFEBReadFirmware(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception)
{

  cgicc::Cgicc cgi(in);

  cgicc::form_iterator name = cgi.getElement("dmb");
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "Not dmb" << std::endl ;
    dmb = DMB_;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];

     std::string cfeb_value = cgi.getElement("cfeb")->getValue(); 
     unsigned icfeb=atoi(cfeb_value.c_str());
     std::vector<CFEB> cfebs = thisDMB->cfebs() ;
     if(icfeb<0 || icfeb>cfebs.size()) icfeb=0;
     std::string chambername= thisCrate->GetChamber(thisDMB)->GetLabel();
     unsigned t = chambername.find('/');
     unsigned s = chambername.size();
     while(t<=s )
     { 
        chambername.replace(t,1,"_");
        t = chambername.find('/');        
     } 
    std::string mcsfile="/tmp/DCFEB_"+chambername+"_C"+cfeb_value+".mcs";
                

     std::cout << getLocalDateTime() << " DCFEB firmware read back from DMB " << dmb << " CFEB " << cfebs[icfeb].number()+1 << std::endl;

     thisDMB->dcfeb_readfirmware_mcs(cfebs[icfeb], mcsfile.c_str());
     
     std::cout << getLocalDateTime() << " DCFEB firmware read back finished." << std::endl;
     this->CFEBUtils(in,out);           
                    
}
  

void EmuPeripheralCrateConfig::DCFEBProgramFpga(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception)
{

  cgicc::Cgicc cgi(in);

  cgicc::form_iterator name = cgi.getElement("dmb");
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "Not dmb" << std::endl ;
    dmb = DMB_;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];

     std::string cfeb_value = cgi.getElement("cfeb")->getValue(); 
     unsigned icfeb=atoi(cfeb_value.c_str());
     std::vector<CFEB> cfebs = thisDMB->cfebs() ;
     if(icfeb<0 || icfeb>cfebs.size()) icfeb=0;

     std::string mcsfile= FirmwareDir_+ "cfeb/me11_dcfeb.mcs";
                
     std::cout << getLocalDateTime() << " DCFEB program FPGA on DMB " << dmb << " CFEB " << cfebs[icfeb].number()+1 << std::endl;
     std::cout << "Use mcs file: " << mcsfile << std::endl;

     thisDMB->dcfeb_program_virtex6(cfebs[icfeb], mcsfile.c_str());
     
     std::cout << getLocalDateTime() << " DCFEB program FPGA finished." << std::endl;
     this->CFEBUtils(in,out);                    
}
  
void EmuPeripheralCrateConfig::DCFEBProgramEprom(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception)
{

  cgicc::Cgicc cgi(in);

  cgicc::form_iterator name = cgi.getElement("dmb");
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "Not dmb" << std::endl ;
    dmb = DMB_;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];

     std::string cfeb_value = cgi.getElement("cfeb")->getValue(); 
     unsigned icfeb=atoi(cfeb_value.c_str());
     std::vector<CFEB> cfebs = thisDMB->cfebs() ;
     if(icfeb<0 || icfeb>cfebs.size()) icfeb=0;

     std::string mcsfile= FirmwareDir_+ "cfeb/me11_dcfeb.mcs";
                
     std::cout << getLocalDateTime() << " DCFEB program EPROM on DMB " << dmb << " CFEB " << cfebs[icfeb].number()+1 << std::endl;
     std::cout << "Use mcs file: " << mcsfile << std::endl;

     thisDMB->dcfeb_program_eprom(cfebs[icfeb], mcsfile.c_str());
     
     std::cout << getLocalDateTime() << " DCFEB program EPROM finished." << std::endl;
     this->CFEBUtils(in,out);                    
}
  
void EmuPeripheralCrateConfig::DCFEBProgramEpromAll(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception)
{

  cgicc::Cgicc cgi(in);

  cgicc::form_iterator name = cgi.getElement("dmb");
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "Not dmb" << std::endl ;
    dmb = DMB_;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];

     std::vector<CFEB> cfebs = thisDMB->cfebs() ;
     std::string mcsfile= FirmwareDir_+ "cfeb/me11_dcfeb.mcs";
/*
     for(uint icfeb=0; icfeb<cfebs.size(); ++icfeb){
       std::cout << getLocalDateTime() << " DCFEB program EPROM on DMB " << dmb << " CFEB " << cfebs[icfeb].number()+1 << std::endl;
       std::cout << "Use mcs file: " << mcsfile << std::endl;
       
       thisDMB->dcfeb_program_eprom(cfebs[icfeb], mcsfile.c_str());
       
       std::cout << getLocalDateTime() << " DCFEB program EPROM finished." << std::endl;
     }
*/
       std::cout << getLocalDateTime() << " Program all DCFEB EPROMs via broadcast on DMB " << dmb << std::endl;
       std::cout << "Use mcs file: " << mcsfile << std::endl;
       
       thisDMB->dcfeb_program_eprom(cfebs[0], mcsfile.c_str(), 1); // broadcast
       std::cout << getLocalDateTime() << " DCFEB program EPROM finished." << std::endl;
     
     this->CFEBUtils(in,out);                    
}
  
void EmuPeripheralCrateConfig::LVMBStatus(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "Not dmb" << std::endl ;
    dmb = DMB_;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];
  Chamber * thisChamber = chamberVector[dmb];
  //
  char Name[100];
  sprintf(Name,"%s LVMB status, crate=%s, DMBslot=%d",
	  (thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),thisDMB->slot());
  //
  MyHeader(in,out,Name);
  //
  *out << cgicc::fieldset().set("style","font-size: 12pt; font-family: arial;");
  *out << std::endl;
  //
  *out << cgicc::legend("LVMB Readback").set("style","color:blue") << std::endl ;
  //
  char buf[2000], sbuf[100];
  int hversion=thisDMB->GetHardwareVersion();
  int nadcs, indx, cfebs, vstart, feed;
  unsigned short *ubuf=(unsigned short *)buf;
  double val, fvalue[100];
  //
  int n=thisDMB->DCSreadAll(buf);
  if(n<=0) 
  {
     *out << "ERROR: Failed to read LVMB!!!" << cgicc::br() << std::endl;
     return;
  }
  nadcs=5;
  cfebs=5;
  vstart=19;
  feed=38;
  if (hversion==2)
  { 
     nadcs=7;
     cfebs=7;
     vstart=25;
     feed=50;
  }
  for(int i=0; i<8*nadcs; i++)
  {   
      if(ubuf[i]==0xBAAD || ubuf[i]==0xFFFF) fvalue[i]=0.0;
      else fvalue[i]=(ubuf[i]&0xFFF)*10.0/4096.0;
  }
  *out << cgicc::br() << cgicc::b("ADC Channels") << std::endl;
  *out << cgicc::table().set("border","1").set("cellpadding","4") << std::endl;
  //
  *out << std::setprecision(3);
  for(int ch=0; ch<9; ch++)
  {
     *out << cgicc::tr() << cgicc::td();
     if(ch) *out << ch-1 << cgicc::td();
     else *out << "Channel" << cgicc::td();
     for(int adc=0; adc<nadcs; adc++)
     {
        if(ch==0) *out << cgicc::td() << " ADC " << adc+1 << cgicc::td();
        else
        {
           indx=adc*8+ch-1;
              sprintf(sbuf, "% 6.2f ", fvalue[indx]);
              *out << cgicc::td() << sbuf;
              if ((hversion<=1 && indx<19) || (hversion==2 && indx<25)) *out << "A";
              else *out << "V";
           *out << cgicc::td();
        }
     }
     *out << cgicc::tr() << std::endl;
  }
  *out << cgicc::table() << std::endl;

  // CFEBs
  int chn2pos[8];
  for (int i=0; i<8; i++) chn2pos[i]=thisDMB->LVDB_map(i);
  //
  *out << cgicc::br() << cgicc::b("CFEB Low Voltages and Currents") << std::endl;

  *out << cgicc::table().set("border","1").set("cellpadding","4");
  //
  *out <<cgicc::tr() << cgicc::td() << cgicc::td();
  *out <<cgicc::td() << ((hversion<=1)?"3.3 V":"3.0 V") << cgicc::td();
  *out <<cgicc::td() << "I (A)" << cgicc::td();
  *out <<cgicc::td() << ((hversion<=1)?"5.0 V":"4.0 V") << cgicc::td();
  *out <<cgicc::td() << "I (A)" << cgicc::td();
  *out <<cgicc::td() << ((hversion<=1)?"6.0 V":"5.5 V") << cgicc::td();
  *out <<cgicc::td() << "I (A)" << cgicc::td();
  *out << cgicc::tr() << std::endl;

  for(int feb=0; feb<cfebs; feb++)
  {
     int lfeb=chn2pos[feb];
     *out << cgicc::tr();
     *out <<cgicc::td() << "CFEB " << feb+1 << cgicc::td();
     for(int cnt=0; cnt<3; cnt++)
     {
        val=fvalue[vstart+3*lfeb+cnt];
        sprintf(sbuf, " %6.2f ", val);
        *out << cgicc::td();
        *out << sbuf;  
        *out << cgicc::td();
        val=fvalue[3*lfeb+cnt];
        sprintf(sbuf, " %6.2f ", val);
        *out <<cgicc::td();
        *out << sbuf;  
        *out << cgicc::td();
     }
     *out << cgicc::tr() << std::endl;
  }
  *out << cgicc::table() << cgicc::br() << std::endl;

  // ALCT
  *out << cgicc::b("ALCT Low Voltages and Currents") << std::endl;
  *out << cgicc::table().set("border","1").set("cellpadding","4") << std::endl;
  //
  *out << cgicc::tr();
  *out <<cgicc::td() << cgicc::td();
  *out <<cgicc::td() << "3.3 V" << cgicc::td();
  *out <<cgicc::td() << "I (A)" << cgicc::td();
  *out <<cgicc::td() << "1.8 V" << cgicc::td();
  *out <<cgicc::td() << "I (A)" << cgicc::td();
  *out <<cgicc::td() << "5.5 V B" << cgicc::td();
  *out <<cgicc::td() << "I (A)" << cgicc::td();
  *out <<cgicc::td() << "5.5 V A" << cgicc::td();
  *out <<cgicc::td() << "I (A)" << cgicc::td();
  *out << cgicc::tr() << std::endl;

     *out << cgicc::tr();
     *out <<cgicc::td() << "ALCT" << cgicc::td();
     for(int cnt=0; cnt<4; cnt++)
     {
        val=fvalue[vstart+3*cfebs+cnt];
        sprintf(sbuf, " %6.2f ", val);
        *out <<cgicc::td();
        *out << sbuf;  
        *out << cgicc::td();
        val=fvalue[3*cfebs+cnt];
        sprintf(sbuf, " %6.2f ", val);
        *out <<cgicc::td();
        *out << sbuf;  
        *out << cgicc::td();
     }
     *out << cgicc::tr() << std::endl;

  *out << cgicc::table() << cgicc::br()<< std::endl;

  *out << cgicc::b("Feed Voltages") << cgicc::br() << std::endl;
  *out << cgicc::table().set("border","1").set("cellpadding","4") << std::endl;
  //
  *out << cgicc::tr() << cgicc::td() << " Analog Feed " << cgicc::td();
  sprintf(sbuf, " %6.2f V", fvalue[feed]);
  *out  << cgicc::td()<< sbuf<< cgicc::td() << cgicc::tr() << std::endl;
  *out << cgicc::tr() << cgicc::td() << " Digital Feed " << cgicc::td();
  sprintf(sbuf, " %6.2f V", fvalue[feed+1]);
  *out  << cgicc::td()<< sbuf<< cgicc::td() << cgicc::tr() << std::endl;
  *out << cgicc::table() << cgicc::br()<< std::endl;

  if(hversion==2)
  {
     double tp = sqrt(2.1962*1000000 + 1000000*(1.8639-fvalue[55])/3.88)-1481.96;
     *out << cgicc::br() << cgicc::b("LVDB7 Temperature sensor") << std::endl;
     sprintf(sbuf, "%8.2f", tp);
     *out << cgicc::br() << sbuf << " (C)" << cgicc::br()<< std::endl; 
  }
  *out << cgicc::fieldset();
}

void EmuPeripheralCrateConfig::DMBUtils(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  int dmb;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "DMBUtils:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "DMBUtils:  No dmb" << std::endl ;
    dmb = DMB_;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  Chamber * thisChamber = chamberVector[dmb];
  //
  int D_hversion=thisDMB->GetHardwareVersion();
  int tot_p_chans; // total power channels on LVDB
  int allmask;
  char Name[100];
  if(D_hversion<=1)
  {  
     sprintf(Name,"%s DMB utilities, crate=%s, slot=%d",(thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),thisDMB->slot());
     tot_p_chans=6;
     allmask=0x3F;
  }
  else
  {
     sprintf(Name,"%s ODMB utilities, crate=%s, slot=%d",(thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),thisDMB->slot());
     tot_p_chans=8;
     allmask=0xFF;
  }
  //
  MyHeader(in,out,Name);
  //
  *out << cgicc::h1(Name);

  //
  if( thisDMB->cfebs().size() > 0 ) {
    std::string CFEBUtils =
      toolbox::toString("/%s/CFEBUtils?dmb=%d",getApplicationDescriptor()->getURN().c_str(),dmb);
    *out << cgicc::a("CFEB Utilities").set("href",CFEBUtils) << cgicc::br()  << cgicc::br() << std::endl;
  }
  //
  //
  char buf[200], nbuf[100];
  unsigned short *voltbuf;
  voltbuf = (unsigned short *)buf;
  //
  int chn2pos[8];
  for (int i=0; i<8; i++) chn2pos[i]=thisDMB->LVDB_map(i);
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl ;
  //
  *out << cgicc::legend("Power Control").set("style","color:blue") ;
  //
  *out << cgicc::table().set("border","1");
  //
  int power_state[9]={0,0,0,0,0,0,0,0,0};
  int powermask = allmask & thisDMB->GetPowerMask();
  unsigned int power_register = thisDMB->lowv_rdpwrreg();
  // std::cout << "power register is " << std::hex << power_register << std::dec << std::endl;
  if (power_register==0xBAAD)
  {
        std::cout << "Cannot read DMB" << std::endl;
        power_register=0;  // can't read DMB (mostly DMB VME firmware problem), assume 0
  }
  int power_read = power_register&allmask;
  for(int icc=1; icc<=tot_p_chans; icc++)
  {   power_state[icc]= power_register & 1;
      power_register = power_register>>1;
  }
  if(power_read==0 && D_hversion<=1)
  {  // if read back is 0 then
     // try read Low voltages and currents to determine if a CFEB/ALCT is on or off
     power_read = thisDMB->DCSreadAll(buf);
     if (power_read<20)
     {
        std::cout << "Cannot read DMB DCS info" << std::endl;
     }
     else
     {
        for(int icc=0; icc<40; icc++)
        {  // remove the bad readings 0xBAAD etc
           if(voltbuf[icc] >= 0xFFF) voltbuf[icc]=0;
        }
        for(int icc=1; icc<=6; icc++)
        {
           power_read = voltbuf[16+icc*3]+voltbuf[17+icc*3]+voltbuf[18+icc*3];
           power_state[icc]= (power_read>1200) ? 1 : 0;  // roughly 3 volts 
        }
        power_read=power_state[6];
        for(int icc=5; icc>0; icc--)
        {
           power_read = power_read<<1;
           power_read += power_state[icc];
        }
     }
  }
  if(powermask)  for(int icc=1; icc<=tot_p_chans; icc++)
  {   if(powermask & 1)  power_state[icc]= -1;
      powermask = powermask>>1;
  }
  *out << cgicc::td() << cgicc::td();
  for(int icc=1; icc<=tot_p_chans; icc++)
  {
     int licc=chn2pos[icc-1];
     *out << cgicc::td();
     if(power_state[licc+1]>0)
        *out << cgicc::span().set("style","color:green");
     else if(power_state[licc+1]==0)
        *out << cgicc::span().set("style","color:red");
     else
        *out << cgicc::span().set("style","color:black");
     if(icc>0 && icc<tot_p_chans)
     {
           *out << "CFEB " << icc;
     }
     else if(icc==tot_p_chans)
     {
           *out << "ALCT";
     }
     *out << cgicc::span() << cgicc::td();
  }
  *out << cgicc::tr();
  *out << cgicc::td();
  //
//debug
std::cout << "Power Read: " << std::hex << power_read << std::dec <<std::endl;

  std::string DMBTurnOn = toolbox::toString("/%s/DMBTurnOn",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBTurnOn) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Turn All On") << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::td();
  for(int icc=0; icc<tot_p_chans; icc++)
  {
     int licc=chn2pos[icc];
     *out << cgicc::td();
     if(power_state[licc+1]>0)
     {
        *out << "On";
     }
     else if(power_state[licc+1]<0)
     {
        *out << "Masked";
     }
     else
     {
        std::string CFEBTurnOn = toolbox::toString("/%s/CFEBTurnOn",getApplicationDescriptor()->getURN().c_str());
        *out << cgicc::form().set("method","GET").set("action",CFEBTurnOn) << std::endl ;
        *out << cgicc::input().set("type","submit").set("value","Turn On") << std::endl ;
        sprintf(buf,"%d",dmb);
        *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
        sprintf(nbuf, "%d", power_read|(1<<licc) ); 
        *out << cgicc::input().set("type","hidden").set("value",nbuf).set("name","cfeb");
        *out << cgicc::form() << std::endl ;
     }
     *out << cgicc::td();
  }

  *out << cgicc::tr();
  *out << cgicc::td();
  //
  std::string DMBTurnOff = toolbox::toString("/%s/DMBTurnOff",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBTurnOff) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Turn All Off") << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::td();
  for(int icc=0; icc<tot_p_chans; icc++)
  {
     int licc=chn2pos[icc];
     *out << cgicc::td();
     if(power_state[licc+1]>0)
     {
        std::string CFEBTurnOn = toolbox::toString("/%s/CFEBTurnOn",getApplicationDescriptor()->getURN().c_str());
        *out << cgicc::form().set("method","GET").set("action",CFEBTurnOn) << std::endl ;
        *out << cgicc::input().set("type","submit").set("value","Turn Off") << std::endl ;
        sprintf(buf,"%d",dmb);
        *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
        sprintf(nbuf, "%d", power_read & (~(1<<licc)) );
        *out << cgicc::input().set("type","hidden").set("value",nbuf).set("name","cfeb");
        *out << cgicc::form() << std::endl ;
     }
     else if(power_state[licc+1]<0)
     {
        *out << "Masked";
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
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl ;
  //
  *out << cgicc::legend("DMB Utils").set("style","color:blue") ;
  //
  std::string DMBPrintCounters = toolbox::toString("/%s/DMBPrintCounters",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBPrintCounters) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","DMB Print Counters") << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br();
  //
  std::string DMBCheckConfiguration = toolbox::toString("/%s/DMBCheckConfiguration",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBCheckConfiguration) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Check DMB+CFEB Configuration") << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br();
  //
  std::string DMBConfigure = toolbox::toString("/%s/DMBConfigure",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBConfigure) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Configure DMB+CFEBs") << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::fieldset() << cgicc::br();
  //  
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl ;
  //
if(D_hversion<=1)
{
  *out << cgicc::legend("DMB/CFEB Firmware").set("style","color:blue") ;
  //
  std::string DMBLoadFirmware = toolbox::toString("/%s/DMBLoadFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBLoadFirmware) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","DMB CONTROL Load Firmware") << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << DMBFirmware_.toString();
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br();
  //
  std::string DMBVmeLoadFirmware = toolbox::toString("/%s/DMBVmeLoadFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBVmeLoadFirmware) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","DMB Vme Load Firmware") << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << DMBVmeFirmware_.toString();
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br();
  //
  std::string DMBVmeLoadFirmwareEmergency = toolbox::toString("/%s/DMBVmeLoadFirmwareEmergency",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DMBVmeLoadFirmwareEmergency) << std::endl ;
  *out << "DMB Board Number:";
  *out <<cgicc::input().set("type","text").set("value","0").set("name","DMBNumber")<<std::endl;
  *out << cgicc::input().set("type","submit").set("value","DMB Vme Load Firmware (Emergency)") << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br();
  //
  std::string CFEBLoadFirmware = toolbox::toString("/%s/CFEBLoadFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CFEBLoadFirmware) << std::endl ;
  *out << "CFEB to download (1-5), (-1 == all) : ";
  *out << cgicc::input().set("type","text").set("value","-1").set("name","DMBNumber") << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","CFEB Load Firmware") << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br();

  std::string CFEBReadFirmware = toolbox::toString("/%s/CFEBReadFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CFEBReadFirmware) << std::endl ;
  *out << "CFEB to verify (0-4), (-1 == all) : ";
  *out << cgicc::input().set("type","text").set("value","-1").set("name","DMBNumber") << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","CFEB Read Firmware") << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br();
  //
  std::string CFEBLoadFirmwareID = toolbox::toString("/%s/CFEBLoadFirmwareID",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CFEBLoadFirmwareID) << std::endl ;
  *out << "CFEB to download (0-4):";
  *out << cgicc::input().set("type","text").set("value","-1").set("name","DMBNumber");
  *out << " Board Serial_Number:";
  *out << cgicc::input().set("type","text").set("value","0").set("name","CFEBSerialNumber")<<std::endl;
  *out << cgicc::input().set("type","submit").set("value","CFEB Load Firmware/Serial Number recovery") << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br();
  //
  *out << cgicc::td();
  std::string RdVfyCFEBVirtexDMB = toolbox::toString("/%s/RdVfyCFEBVirtexDMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",RdVfyCFEBVirtexDMB) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Check CFEB FPGAs") << std::endl ;
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;;
  *out << cgicc::td();
	*out << cgicc::br() << std::endl;
  //
}
else if(D_hversion==2)
{
     *out << cgicc::legend("ODMB Firmware").set("style","color:blue") ;
     //
     std::string DMBReadFirmware = toolbox::toString("/%s/DMBReadFirmware",getApplicationDescriptor()->getURN().c_str());
     *out << cgicc::form().set("method","GET").set("action",DMBReadFirmware) << std::endl ;
     *out << cgicc::input().set("type","submit").set("value","Read back ODMB Firmware") << std::endl ;
     sprintf(buf,"%d",dmb);
     *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
     *out << cgicc::form() << std::endl ;
     //
     *out << cgicc::br();
     //
     std::string DMBLoadFirmware = toolbox::toString("/%s/DMBLoadFirmware",getApplicationDescriptor()->getURN().c_str());
     *out << cgicc::form().set("method","GET").set("action",DMBLoadFirmware) << std::endl ;
     *out << cgicc::input().set("type","submit").set("value","ODMB Program EPROM") << std::endl ;
     sprintf(buf,"%d",dmb);
     *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
     *out << FirmwareDir_+"odmb/me11_odmb.mcs";
     *out << cgicc::form() << std::endl ;
     *out << cgicc::br();
     //
     std::string ODMBLoadFirmwarePoll = toolbox::toString("/%s/ODMBLoadFirmwarePoll",getApplicationDescriptor()->getURN().c_str());
     *out << cgicc::form().set("method","GET").set("action",ODMBLoadFirmwarePoll) << std::endl ;
     *out << cgicc::input().set("type","submit").set("value","ODMB Program EPROM by Polling BPI") << std::endl ;
     sprintf(buf,"%d",dmb);
     *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
     *out << FirmwareDir_+"odmb/me11_odmb.mcs";
     *out << cgicc::form() << std::endl ;
     //
     *out << cgicc::br() <<cgicc::hr() << std::endl;
     *out << "Use this one ONLY if power-cycle failed to recover the FPGA:" << cgicc::br()<< std::endl;
     std::string DMBLoadFPGA = toolbox::toString("/%s/DMBLoadFPGA",getApplicationDescriptor()->getURN().c_str());
     *out << cgicc::form().set("method","GET").set("action",DMBLoadFPGA) << std::endl ;
     *out << cgicc::input().set("type","submit").set("value","ODMB Program FPGA") << std::endl ;
     sprintf(buf,"%d",dmb);
     *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
     *out << FirmwareDir_+"odmb/me11_odmb.mcs";
     *out << cgicc::form() << std::endl ;
     //
     *out << cgicc::br();
}
	//
  std::string CCBHardResetFromDMBPage = toolbox::toString("/%s/CCBHardResetFromDMBPage",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CCBHardResetFromDMBPage) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","CCB hard reset") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::fieldset();
  //
  // Output area
  //
  *out << cgicc::form().set("method","GET") << std::endl ;
  *out << cgicc::pre();
  *out << cgicc::textarea().set("name","CrateTestDMBOutput").set("rows","30").set("cols","132").set("WRAP","OFF");
	if (total_bad_cfeb_bits >= 0) {
		*out << "CFEB FPGA check:  Total bad bits =  " << std::dec << total_bad_cfeb_bits;
		*out << ", total good bits = " << total_good_cfeb_bits;
		*out << std::endl;
		*out << "See latest cfebvirtex_check.log in ~/firmware/status_check for more details";
		*out << std::endl;
		total_bad_cfeb_bits = -1;	// Turn off display for next time.
	}
	*out << std::endl;
  *out << OutputStringDMBStatus[dmb].str() << std::endl ;
  *out << cgicc::textarea();
  OutputStringDMBStatus[dmb].str("");
  *out << cgicc::pre();
  *out << cgicc::form() << std::endl ;
  //
}
//
void EmuPeripheralCrateConfig::CFEBTurnOn(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    // std::cout << "CFEBTurnOn:  DMB " << dmb << std::endl;
  }
  //
  name = cgi.getElement("cfeb");
  //
  int mask=-1;
  if(name != cgi.getElements().end()) {
    mask = cgi["cfeb"]->getIntegerValue();
    // std::cout << "CFEBTurnOn: mask " << mask << std::endl;
  }
  DAQMB * thisDMB = dmbVector[dmb];
  //
  std::cout << "CFEBTurnOn mask " <<std::hex << mask << std::dec <<std::endl;  
  //
  if (thisDMB && mask>=0)
  {
    mask = mask & 0xFF;
    thisDMB->lowv_onoff(mask);
    ::sleep(1);
  }
  //
  this->DMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::DMBTurnOff(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "DMBTurnOff:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  if (thisDMB) {
    thisDMB->lowv_onoff(0x0);
    ::sleep(1);
  }
  //
  this->DMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::DMBLoadFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "DMBLoadFirmware:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  if (thisDMB) 
  {
   
    int hversion=thisDMB->GetHardwareVersion();
    if(hversion<=1)
    {
    thisCCB->hardReset();
    //
    std::cout << "DMBLoadFirmware in slot " << thisDMB->slot() << std::endl;
    if (thisDMB->slot()==25) std::cout <<" Broadcast Loading the control FPGA insode one crate"<<std::endl;
    //
    ::sleep(1);
    unsigned short int dword[2];
    dword[0]=0;
    //
    char *outp=(char *)dword;
    //char *name = DMBFirmware_.toString().c_str() ;
    thisDMB->epromload(MPROM,DMBFirmware_.toString().c_str(),1,outp);  // load mprom
    //
    ::sleep(5);
    thisCCB->hardReset();
    }
    else if(hversion==2)
    {
       std::string mcsfile= FirmwareDir_+ "odmb/me11_odmb.mcs";
                
       std::cout << getLocalDateTime() << " ODMB program EPROM in slot " << thisDMB->slot() << std::endl;
       std::cout << "Use mcs file: " << mcsfile << std::endl;

       thisDMB->odmb_program_eprom(mcsfile.c_str());
     
       std::cout << getLocalDateTime() << " ODMB program EPROM finished." << std::endl;

    }
  }
  //
  this->DMBUtils(in,out);
  //
}
//
//
void EmuPeripheralCrateConfig::ODMBLoadFirmwarePoll(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "ODMBLoadFirmwarePoll:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  if (thisDMB) 
  {
   
    int hversion=thisDMB->GetHardwareVersion();
    if(hversion==2)
    {
       std::string mcsfile= FirmwareDir_+ "odmb/me11_odmb.mcs";
                
       std::cout << getLocalDateTime() << " ODMB program EPROM by Polling BPI status in slot " << thisDMB->slot() << std::endl;
       std::cout << "Use mcs file: " << mcsfile << std::endl;

       bool success=thisDMB->odmb_program_eprom_poll(mcsfile.c_str());
       if(success)     
          std::cout << getLocalDateTime() << " ODMB program EPROM finished successfully." << std::endl;
       else
           std::cout << getLocalDateTime() << " ODMB program EPROM failed." << std::endl;
    }
  }
  //
  this->DMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::DMBLoadFPGA(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "DMBLoadFPGA:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  if (thisDMB) 
  {
   
    int hversion=thisDMB->GetHardwareVersion();
    if(hversion<=1)
    {
       thisCCB->hardReset();
    }
    else if(hversion==2)
    {
       std::string mcsfile= FirmwareDir_+ "odmb/me11_odmb.mcs";
                
       std::cout << getLocalDateTime() << " ODMB program FPGA in slot " << thisDMB->slot() << std::endl;
       std::cout << "Use mcs file: " << mcsfile << std::endl;

       thisDMB->odmb_program_virtex6(mcsfile.c_str());
     
       std::cout << getLocalDateTime() << " ODMB program FPGA finished." << std::endl;

    }
  }
  //
  this->DMBUtils(in,out);
  //
}

void EmuPeripheralCrateConfig::DMBReadFirmware(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception)
{

  cgicc::Cgicc cgi(in);

  cgicc::form_iterator name = cgi.getElement("dmb");
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "Not dmb" << std::endl ;
    dmb = DMB_;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];

  if(thisDMB)
  {
     int hversion=thisDMB->GetHardwareVersion();
     if(hversion==2)
     {
        std::string chambername= thisCrate->GetChamber(thisDMB)->GetLabel();
        unsigned t = chambername.find('/');
        unsigned s = chambername.size();
        while(t<=s )
        { 
           chambername.replace(t,1,"_");
           t = chambername.find('/');        
        } 
        std::string mcsfile="/tmp/ODMB_"+chambername+".mcs";
                
        std::cout << getLocalDateTime() << " ODMB firmware read back from slot " << thisDMB->slot() << std::endl;

        thisDMB->odmb_readfirmware_mcs(mcsfile.c_str());
     
        std::cout << getLocalDateTime() << " ODMB firmware read back finished." << std::endl;
     }
  }                    
  this->DMBUtils(in, out);
}
//
void EmuPeripheralCrateConfig::DMBVmeLoadFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "DMBVmeLoadFirmware:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  int mindmb = dmb;
  int maxdmb = dmb+1;
  if (thisDMB->slot() == 25) { //if DMB slot = 25, loop over each dmb
    mindmb = 0;
    maxdmb = dmbVector.size()-1;
  }
  //
  thisCCB->hardReset();
  //
  for (dmb=mindmb; dmb<maxdmb; dmb++) {
    //
    thisDMB = dmbVector[dmb];
    //
    if (thisDMB) {
      //
      std::cout << "DMBVmeLoadFirmware in slot " << thisDMB->slot() << std::endl;
      //
      ::sleep(1);
      //
      unsigned short int dword[2];
      dword[0]=thisDMB->mbpromuser(0);
      dword[1]=0xdb00;
      // dword[0] = 0x01bd;
      // dword[1] = 0xff00;  to manually change the DMB ID.
      char * outp=(char *)dword;   // recast dword
      thisDMB->epromload(VPROM,DMBVmeFirmware_.toString().c_str(),1,outp);  // load mprom
      //Test the random trigger
      //	thisDMB->set_rndmtrg_rate(-1);
      //	thisDMB->set_rndmtrg_rate(-1);
      //	thisDMB->toggle_rndmtrg_start();  
    }
    //
  }
  ::sleep(1);
  thisCCB->hardReset(); //disable this when testing the random_trigger
  //
  this->DMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::DMBVmeLoadFirmwareEmergency(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  int dmbNumber = 0;
  //
  cgicc::form_iterator name2 = cgi.getElement("DMBNumber");
  //int registerValue = -1;
  if(name2 != cgi.getElements().end()) {
    dmbNumber = cgi["DMBNumber"]->getIntegerValue();
  }
  //
  std::cout << "Loading DMB# " <<dmbNumber << std::endl ;
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  if (thisDMB->slot() == 25) { 
    std::cout <<" The emergency load is NOT available for DMB slot25"<<std::endl;
    std::cout <<" Please use individual slot loading !!!"<<std::endl;
    return;
  }
  //
  thisCCB->hardReset();
  if (thisDMB) {
    //
    std::cout << "DMB Vme Load Firmware Emergency in slot " << thisDMB->slot() << std::endl;
    LOG4CPLUS_INFO(getApplicationLogger(),"Started DMB Vme Load Firmware Emergency");
    //
    ::sleep(1);
    //
    unsigned short int dword[2];

    std::string crate=thisCrate->GetLabel();
    int slot=thisDMB->slot();
    int dmbID=brddb->CrateToDMBID(crate,slot);
    dword[0]=dmbNumber&0x03ff;
    dword[1]=0xDB00;
    if (((dmbNumber&0xfff)==0)||((dmbNumber&0xfff)==0xfff)) dword[0]=dmbID&0x03ff;

    std::cout<<" The DMB number is set to: "<<dword[0]<<" Entered: "<<dmbNumber<<" Database lookup: "<<dmbID<<std::endl;
    char * outp=(char *)dword;  
    thisDMB->epromload(RESET,DMBVmeFirmware_.toString().c_str(),1,outp);  // load mprom
  }
  ::sleep(1);
  thisCCB->hardReset();
  //
  this->DMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::CFEBReadFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  LOG4CPLUS_INFO(getApplicationLogger(),"Started CFEB firmware Verify");
  //
  cgicc::Cgicc cgi(in);
  //
  int dmbNumber = -1;
  //
  cgicc::form_iterator name2 = cgi.getElement("DMBNumber");
  //int registerValue = -1;
  if(name2 != cgi.getElements().end()) {
    dmbNumber = cgi["DMBNumber"]->getIntegerValue();
  }
  //
  std::cout << "Loading DMBNumber " <<dmbNumber << std::endl ;
  //*out << "Loading DMBNumber " <<dmbNumber ;
  //*out << cgicc::br();
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];
  int mindmb = dmb;
  int maxdmb = dmb+1;
  if (thisDMB->slot() == 25) { //if DMB slot = 25, loop over each cfeb
    mindmb = 0;
    maxdmb = dmbVector.size()-1;
  }
  for (dmb=mindmb; dmb<maxdmb; dmb++) {
    //
    thisDMB = dmbVector[dmb];
    //
    std::cout << "CFEBReadFirmware - DMB " << dmb << std::endl;
    //
    thisCCB->hardReset();
    //
    if (thisDMB) {
      // check the CFEB EPROM firmware first
      //
      std::vector<CFEB> thisCFEBs = thisDMB->cfebs();
      //
      ::sleep(1);
      //
      if (dmbNumber == -1 ) { // This, actually, is a CFEB number, not DMB
	for (unsigned int i=0; i<thisCFEBs.size(); i++) {
	  std::ostringstream dum;
	  dum << "Verifying CFEB firmware for DMB=" << dmb << " CFEB="<< i << std::endl;
	  LOG4CPLUS_INFO(getApplicationLogger(), dum.str());
	  unsigned short int dword[2];
	  dword[0]=thisDMB->febpromuser(thisCFEBs[i]);
	  CFEBid_[dmb][i] = dword[0];  // fill summary file with user ID value read from this CFEB
	  char * outp=(char *)dword;   // recast dword
	  unlink("/tmp/eprom.bit");
	  thisDMB->epromload_verify(thisCFEBs[i].promDevice(),CFEBVerify_.toString().c_str(),1,outp);  // load mprom
	  // create a report from the results above 
	  std::ostringstream logs;
	  int erropen = thisDMB->check_eprom_readback("/tmp/eprom.bit",CFEBCompare_.toString().c_str()); // hardcoded file name; bad, but I didn't start it //KK
	  if(erropen>=0){
	     logs<<" Total number of bad bits: "<<thisDMB->GetNumberOfBadReadbackBits()<<std::endl;
	     for(unsigned int bit=0; bit<thisDMB->GetNumberOfBadReadbackBits() && bit<20; bit++ ){
	        logs << " broken word position: " << std::setw(6) << thisDMB->GetWordWithBadReadbackBit(bit)
	             << ", bad bit position: " << thisDMB->GetBadReadbackBitPosition(bit)
	             << ", bad bit type (type=0 1->0 type=1 0->1): "<< thisDMB->GetBadReadbackBitType(bit)
	             << std::endl;
	     }
             if( thisDMB->GetNumberOfBadReadbackBits()>20 ) logs << "  only first 20 bad CFEB firmware bits were reported above " << std::endl;
	  } else {
	     logs << " file error in check_eprom_readback" << std::endl;
	  }
	  LOG4CPLUS_INFO(getApplicationLogger(), logs.str());
	}
      } else {
	std::cout << "Verifying CFEB firmware for DMB=" << dmb << " CFEB="<< dmbNumber << std::endl;
	unsigned short int dword[2];
	for (unsigned int i=0; i<thisCFEBs.size(); i++) {
	  if (thisCFEBs[i].number() == dmbNumber ) {
	    dword[0]=thisDMB->febpromuser(thisCFEBs[i]);
	    CFEBid_[dmb][i] = dword[0];  // fill summary file with user ID value read from this CFEB
	    char * outp=(char *)dword;   // recast dword
	    unlink("/tmp/eprom.bit");
	    thisDMB->epromload_verify(thisCFEBs[i].promDevice(),CFEBVerify_.toString().c_str(),1,outp);  // load mprom
	    // create a report from the results above 
	    std::ostringstream logs;
	    int erropen = thisDMB->check_eprom_readback("/tmp/eprom.bit",CFEBCompare_.toString().c_str()); // hardcoded file name; bad, but I didn't start it //KK
	    if(erropen>=0){
	       logs<<" Total number of bad bits: "<<thisDMB->GetNumberOfBadReadbackBits()<<std::endl;
	       for(unsigned int bit=0; bit<thisDMB->GetNumberOfBadReadbackBits() && bit<20; bit++ ){
	          logs << " broken word position: " << std::setw(6) << thisDMB->GetWordWithBadReadbackBit(bit)
	               << ", bad bit position: " << thisDMB->GetBadReadbackBitPosition(bit)
	               << ", bad bit type (type=0 1->0 type=1 0->1): "<< thisDMB->GetBadReadbackBitType(bit)
	               << std::endl;
	       }
               if( thisDMB->GetNumberOfBadReadbackBits()>20 ) logs << "  only first 20 bad CFEB firmware bits were reported above " << std::endl;
	    } else {
	       logs << " file error in check_eprom_readback" << std::endl;
	    }
	    LOG4CPLUS_INFO(getApplicationLogger(), logs.str());
	  }
	}
      }

      //
      // check the DMB MPROM firmware
      std::cout << "Verifying DMB MPROM firmware for DMB=" << dmb << std::endl;
      unsigned short int dword[2];
      dword[0]=0;
      char * outp=(char *)dword;   // recast dword
      unlink("/tmp/eprom.bit");
      thisDMB->epromload_verify(MPROM, DMBVerify_.toString().c_str(), 1, outp);    // dmb mprom
      // create a report from the results above 
      std::ostringstream logs;
      int erropen = thisDMB->check_eprom_readback("/tmp/eprom.bit",DMBCompare_.toString().c_str()); // hardcoded file name; bad, but I didn't start it //KK
      if(erropen>=0){
         logs<<" Total number of bad bits: "<<thisDMB->GetNumberOfBadReadbackBits()<<std::endl;
         for(unsigned int bit=0; bit<thisDMB->GetNumberOfBadReadbackBits() && bit<20; bit++ ){
            logs << " broken word position: " << std::setw(6) << thisDMB->GetWordWithBadReadbackBit(bit)
                 << ", bad bit position: " << thisDMB->GetBadReadbackBitPosition(bit)
                 << ", bad bit type (type=0 1->0 type=1 0->1): "<< thisDMB->GetBadReadbackBitType(bit)
                 << std::endl;
         }
         if( thisDMB->GetNumberOfBadReadbackBits()>20 ) logs << "  only first 20 bad DMB MPROM firmware bits were reported above " << std::endl;
      } else {
         logs << " file error in check_eprom_readback" << std::endl;
      }
      LOG4CPLUS_INFO(getApplicationLogger(), logs.str());

      // check the DMB VPROM firmware
      std::cout << "Verifying DMB VPROM firmware for DMB=" << dmb << std::endl;

      dword[0]=thisDMB->mbpromuser(0);
      dword[1]=0xdb00;
      outp=(char *)dword;   // recast dword
      unlink("/tmp/eprom.bit");
      thisDMB->epromload_verify(VPROM, DMBVmeVerify_.toString().c_str(), 1, outp);    // dmb mprom
      // create a report from the results above 
      std::ostringstream logs2;
      erropen = thisDMB->check_eprom_readback("/tmp/eprom.bit",DMBVmeCompare_.toString().c_str()); // hardcoded file name; bad, but I didn't start it //KK
      if(erropen>=0){
         logs2<<" Total number of bad bits: "<<thisDMB->GetNumberOfBadReadbackBits()<<std::endl;
         for(unsigned int bit=0; bit<thisDMB->GetNumberOfBadReadbackBits() && bit<20; bit++ ){
            logs2 << " broken word position: " << std::setw(6) << thisDMB->GetWordWithBadReadbackBit(bit)
                 << ", bad bit position: " << thisDMB->GetBadReadbackBitPosition(bit)
                 << ", bad bit type (type=0 1->0 type=1 0->1): "<< thisDMB->GetBadReadbackBitType(bit)
                 << std::endl;
         }
         if( thisDMB->GetNumberOfBadReadbackBits()>20 ) logs2 << "  only first 20 bad DMB VPROM firmware bits were reported above " << std::endl;
      } else {
         logs2 << " file error in check_eprom_readback" << std::endl;
      }
      LOG4CPLUS_INFO(getApplicationLogger(), logs2.str());

    }
    ::sleep(1);
    thisCCB->hardReset();
  }
  //
  this->DMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::CFEBLoadFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  LOG4CPLUS_INFO(getApplicationLogger(),"Started CFEB firmware download");
  //
  cgicc::Cgicc cgi(in);
  //
  int dmbNumber = -1;
  //
  cgicc::form_iterator name2 = cgi.getElement("DMBNumber");
  //int registerValue = -1;
  if(name2 != cgi.getElements().end()) {
    dmbNumber = cgi["DMBNumber"]->getIntegerValue();
  }
  //
  std::cout << "Loading CFEB " <<dmbNumber << std::endl ;
  dmbNumber--;
  std::cout << "... which is " << dmbNumber << " according to the software... " << std::endl;
  //*out << "Loading DMBNumber " <<dmbNumber ;
  //*out << cgicc::br();
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];
  int mindmb = dmb;
  int maxdmb = dmb+1;
  if (thisDMB->slot() == 25) { //if DMB slot = 25, loop over each cfeb
    mindmb = 0;
    maxdmb = dmbVector.size()-1;
  }
  for (dmb=mindmb; dmb<maxdmb; dmb++) {
    //
    thisDMB = dmbVector[dmb];
    //
    std::cout << "CFEBLoadFirmware - DMB " << dmb << std::endl;
    //
    //    thisCCB->hardReset();
    //
    if (thisDMB) {
      //
      std::vector<CFEB> thisCFEBs = thisDMB->cfebs();
      //
      ::sleep(1);
      //
      if (dmbNumber == -2 || dmbNumber == -1) {
	for (unsigned int i=0; i<thisCFEBs.size(); i++) {
	  std::ostringstream dum;
	  dum << "loading CFEB firmware for DMB=" << dmb << " CFEB="<< i << std::endl;
	  LOG4CPLUS_INFO(getApplicationLogger(), dum.str());
	  unsigned short int dword[2];
	  dword[0]=thisDMB->febpromuser(thisCFEBs[i]);
	  CFEBid_[dmb][i] = dword[0];  // fill summary file with user ID value read from this CFEB
	  char * outp=(char *)dword;   // recast dword
	  thisDMB->epromload(thisCFEBs[i].promDevice(),CFEBFirmware_.toString().c_str(),1,outp);  // load mprom
	}
      } else {
	std::cout << "loading CFEB firmware for DMB=" << dmb << " CFEB="<< dmbNumber << std::endl;
	unsigned short int dword[2];
	for (unsigned int i=0; i<thisCFEBs.size(); i++) {
	  if (thisCFEBs[i].number() == dmbNumber ) {
	    dword[0]=thisDMB->febpromuser(thisCFEBs[i]);
	    CFEBid_[dmb][i] = dword[0];  // fill summary file with user ID value read from this CFEB
	    char * outp=(char *)dword;   // recast dword
	    thisDMB->epromload(thisCFEBs[i].promDevice(),CFEBFirmware_.toString().c_str(),1,outp);  // load mprom
	  }
	}
      }
    }
    //    ::sleep(1);
    //    thisCCB->hardReset();
  }
  //
  this->DMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::CCBHardResetFromDMBPage(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  thisCCB->hardReset();
  //
  this->DMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::CFEBLoadFirmwareID(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  LOG4CPLUS_INFO(getApplicationLogger(),"Started CFEB firmware download with Board_number");
  //
  cgicc::Cgicc cgi(in);
  //
  int dmbNumber = -1;
  //
  cgicc::form_iterator name2 = cgi.getElement("DMBNumber");
  //int registerValue = -1;
  if(name2 != cgi.getElements().end()) {
    dmbNumber = cgi["DMBNumber"]->getIntegerValue();
  }
  //
  int cfebSerialNumber = 0;
  //
  cgicc::form_iterator name3 = cgi.getElement("CFEBSerialNumber");
  //int registerValue = -1;
  if(name3 != cgi.getElements().end()) {
    cfebSerialNumber = cgi["CFEBSerialNumber"]->getIntegerValue();
  }
  //
  std::cout << "Loading CFEBNumber " <<dmbNumber << " with serial number: "<<cfebSerialNumber<<std::endl ;
  if (cfebSerialNumber>2600 ||cfebSerialNumber<1 ||
      dmbNumber>4 || dmbNumber<0) {
    std::cout<<"Invalid cfeb number, or serial number"<<std::endl;
    return;
  }
  //*out << "Loading DMBNumber " <<dmbNumber ;
  //*out << cgicc::br();
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];
  if ((thisDMB->slot() >21) || (thisDMB->slot() <3)){
    std::cout<<" Invalid DMB slot for CFEB Number reloading "<<thisDMB->slot()<<std::endl;
    return;
  }
  //
  std::cout << "CFEBLoadFirmware - DMB " << dmb << std::endl;
  //
  //    thisCCB->hardReset();
  //
  if (thisDMB) {
    //
    std::vector<CFEB> thisCFEBs = thisDMB->cfebs();
    //
    ::sleep(1);
    //
    std::ostringstream dum;
    dum << "loading CFEB firmware for DMB=" << dmb << " CFEB="<< dmbNumber << std::endl;
    LOG4CPLUS_INFO(getApplicationLogger(), dum.str());
    for (unsigned int i=0; i<thisCFEBs.size(); i++) {
      if (thisCFEBs[i].number() == dmbNumber ) {
	std::cout <<" ThisCFEB[i].promdevice: "<<thisCFEBs[i].promDevice()<<std::endl;
	//force CFEB device switch
	unsigned short int dword[2];
	dword[0]=thisDMB->febpromuser(thisCFEBs[4-i]);
	dword[0]=cfebSerialNumber;
	dword[1]=0xCFEB;
	char * outp=(char *)dword;   // recast dword
	
	thisDMB->epromload(thisCFEBs[i].promDevice(),CFEBFirmware_.toString().c_str(),1,outp);
      }
    }
  }
  //    ::sleep(1);
  //    thisCCB->hardReset();
  //
  this->DMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::DMBTurnOn(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "DMBTurnOn DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  int D_hversion=thisDMB->GetHardwareVersion();
  int AllOn = (D_hversion<=1)?0x3F:0xFF;
  if (thisDMB) {
    thisDMB->lowv_onoff(AllOn);
    ::sleep(1);
  }
  //
  this->DMBUtils(in,out);
  //
}
//
void EmuPeripheralCrateConfig::DMBCheckConfiguration(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "DMBCheckConfiguration  DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  thisDMB->RedirectOutput(&OutputStringDMBStatus[dmb]);
  thisDMB->checkDAQMBXMLValues();
  thisDMB->RedirectOutput(&std::cout);
  //
  this->DMBUtils(in,out);
  //
}

void EmuPeripheralCrateConfig::DMBConfigure(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  int dmb=0;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << getLocalDateTime() << " DMBConfigure  DMB " << dmb << std::endl;
    DMB_ = dmb;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  // this is copied from Crate->configure(), part of WRITE FLASH action
  thisDMB->restoreCFEBIdle();
  thisDMB->restoreMotherboardIdle();
  thisDMB->configure();
  //
  this->DMBUtils(in,out);
  //
}

///////////////////////////////////////////////////////////////////////////////////
// DMB status
///////////////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::DMBStatus(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  int dmb;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "Not dmb" << std::endl ;
    dmb = DMB_;
  }
  //
  DAQMB * thisDMB = dmbVector[dmb];
  //
  bool isME13 = false;
  TMB * thisTMB   = tmbVector[dmb];
  ALCTController * thisALCT=0;
  if (thisTMB) 
    thisALCT = thisTMB->alctController();
  if (thisALCT) 
    if ( (thisALCT->GetChamberType()).find("ME13") != std::string::npos )
      isME13 = true;
  //
  Chamber * thisChamber = chamberVector[dmb];
  std::string chamber=thisChamber->GetLabel();
  int hversion=thisDMB->GetHardwareVersion();
  char buf[2000], sbuf[100];
  int nadcs, indx, cfebs, vstart, feed;
  unsigned short *ubuf=(unsigned short *)buf;
  double val, fvalue[100];
  nadcs=5;
  cfebs=5;
  vstart=19;
  feed=38;
  if (hversion==2)
  { 
     nadcs=7;
     cfebs=7;
     vstart=25;
     feed=50;
  }
  //
  char Name[100];
  sprintf(Name,"%s DMB status, crate=%s, slot=%d",(thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),thisDMB->slot());	
  //
  MyHeader(in,out,Name);
  //
  //*out << cgicc::h1(Name);
  //*out << cgicc::br();
  //
  if( thisDMB->cfebs().size() > 0 ) {
    std::string CFEBStatus =
      toolbox::toString("/%s/CFEBStatus?dmb=%d",getApplicationDescriptor()->getURN().c_str(),dmb);
    *out << cgicc::a("CFEB Status").set("href",CFEBStatus) << std::endl;
  }
  std::string LVMBStatus =
      toolbox::toString("/%s/LVMBStatus?dmb=%d",getApplicationDescriptor()->getURN().c_str(),dmb);
  *out << cgicc::a("LVMB Status").set("href",LVMBStatus) << cgicc::br() << std::endl;
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << std::endl;
  //
  *out << cgicc::legend("DMB IDs").set("style","color:blue") << std::endl ;
  //
  *out << cgicc::pre();
  if(hversion<=1)
  {
  //
      unsigned long int cfebID[5], cfebIDread[5];
      std::vector <CFEB> thisCFEBs=thisDMB->cfebs();
      //
      for (unsigned i=0;i<thisCFEBs.size();i++) {
        cfebIDread[i]=thisDMB->febpromuser(thisCFEBs[i]);
        cfebID[i]=brddb->ChamberToCFEBID(chamber,i+1);
        std::cout<<" DB_check CFEB # "<<i<<" ID readback: "<<(cfebIDread[i]&0xfff)<<" Look up from DB: "<<(cfebID[i]&0xfff)<<std::endl;
      }
      std::string crate=thisCrate->GetLabel();
      int slot=thisDMB->slot();
      std::cout<<" Crate: "<<crate<<" slot "<<slot<<std::endl;
      int dmbID=brddb->CrateToDMBID(crate,slot);
      //The readback
      unsigned long int dmbIDread=thisDMB->mbpromuser(0);
      std::cout<<" DB_check DMB ID readback: "<<(dmbIDread&0xfff)<<" look up from DB: "<<(dmbID&0xfff)<<std::endl;

    thisDMB->vmefpgaid();
    sprintf(buf,"DMB vme FPGA : Version %d Revision %x Day %d Month %d Year %d",
	  (int)thisDMB->GetFirmwareVersion(),(int)thisDMB->GetFirmwareRevision(),
	  (int)thisDMB->GetFirmwareDay(),(int)thisDMB->GetFirmwareMonth(),(int)thisDMB->GetFirmwareYear());
    //
    if ( thisDMB->CheckVMEFirmwareVersion() ) {
      *out << cgicc::span().set("style","color:green");
      *out << buf;
      *out << "...OK...";
      *out << cgicc::span();
    } else {
      *out << cgicc::span().set("style","color:red");
      *out << buf;
      *out << "--->> BAD <<--- should be "
	 << std::dec << thisDMB->GetExpectedVMEFirmwareTag();
      *out << cgicc::span();
    }
    *out << cgicc::br();
    //
    sprintf(buf,"DMB prom VME->Motherboard          : %08x ",(int)thisDMB->mbpromuser(0));
    *out << buf ;
    *out << cgicc::br();
    //
    sprintf(buf,"DMB prom Motherboard Controller    : %08x ",(int)thisDMB->mbpromuser(1));
    *out << buf  ;
    *out << cgicc::br();
    //
    sprintf(buf,"DMB fpga id                        : %08x ",(int)thisDMB->mbfpgaid());
    *out << buf  ;
    *out << cgicc::br();
    //
    sprintf(buf,"DMB prom VME->Motherboard ID       : %08x ",(int)thisDMB->mbpromid(0));
    *out << buf  ;
    *out << cgicc::br();
    //
    sprintf(buf,"DMB prom Motherboard Controller ID : %08x ",(int) thisDMB->mbpromid(1));
    *out << buf  ;
    *out << cgicc::br();
    //
    sprintf(buf,"DMB fpga user id                   : %x ", (int) thisDMB->mbfpgauser());
  
    if ( thisDMB->CheckControlFirmwareVersion() ) {
      *out << cgicc::span().set("style","color:green");
      *out << buf;
      *out << "...OK...";
      *out << cgicc::span();
    } else {
      *out << cgicc::span().set("style","color:red");
      *out << buf;
      *out << "--->> BAD <<--- should be "
	 << std::hex << thisDMB->GetExpectedControlFirmwareTag() << std::dec;
      *out << cgicc::span();
    }
  }
  else if(hversion==2)
  {
     int fwv=thisDMB->odmb_firmware_version();
     int fw_xml=thisDMB->GetExpectedControlFirmwareTag();
     sprintf(buf,"ODMB firmware version : V%02X_%02X (tag %02X%02X)",(fwv>>8)&0xFF, fwv&0xFF,(fwv>>8)&0xFF, fwv&0xFF);
     if ( (fwv&0xFFFF)==(fw_xml&0xFFFF) ) 
     {
        *out << cgicc::span().set("style","color:green");
        *out << buf << " ...OK...";
        *out << cgicc::span();
     } else 
     {
        *out << cgicc::span().set("style","color:red");
        *out << buf;
        *out << "--->> BAD <<--- should be ";
        sprintf(buf,"V%02X_%02X (tag %02X%02X)",(fw_xml>>8)&0xFF, fw_xml&0xFF,(fw_xml>>8)&0xFF, fw_xml&0xF);
        *out << buf << cgicc::span();
    }
     *out << cgicc::br();
     int qpll_state=thisDMB->read_qpll_state();
     sprintf(buf,"ODMB QPLL lock state  : %04X ",qpll_state);
     if ( qpll_state ) 
     {
        *out << cgicc::span().set("style","color:green");
        *out << buf << " ...OK...";
        *out << cgicc::span();
     } else 
     {
        *out << cgicc::span().set("style","color:red");
        *out << buf;
        *out << "--->> BAD <<--- should be 1";
        *out << cgicc::span();
    }
    *out  << std::endl;
     int idcode=thisDMB->mbfpgaid();
     sprintf(buf,"ODMB fpga ID Code     : %08X ",idcode);
     if ( (idcode&0xFFFFFFF)==(0x8424A093&0xFFFFFFF) ) 
     {
        *out << cgicc::span().set("style","color:green");
        *out << buf << " ...OK...";
        *out << cgicc::span();
     } else 
     {
        *out << cgicc::span().set("style","color:red");
        *out << buf;
        *out << "--->> BAD <<--- should be 0x8424A093";
        *out << cgicc::span();
    }
     *out << cgicc::br();
     sprintf(buf,"ODMB fpga User Code   : %08X ", (int)thisDMB->mbfpgauser());
     *out << buf << std::endl;
     int unique_id=thisDMB->read_odmb_id();     
     sprintf(buf,"ODMB unique id        : %04X ", unique_id);
     *out << buf << std::endl;
     // ODMB Control
     // DCFEB Control
  }
  //
  *out << cgicc::pre();
  //
  //thisDMB->lowv_dump();
  //thisDMB->daqmb_adc_dump();
  //thisDMB->daqmb_promfpga_dump();
  //
  *out << cgicc::fieldset();
  *out << std::endl ;
 
  if(hversion==2)
  {
     *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
     *out << std::endl ;
     *out << cgicc::legend("ODMB Settings").set("style","color:blue") 
          << std::endl ;
     *out << cgicc::table().set("border","1").set("cellpadding","4");
     *out << cgicc::tr();
     *out << cgicc::td() << "LCT_L1A delay: " << thisDMB->odmb_read_LCT_L1A_delay() << cgicc::td();      
     *out << cgicc::td() << "TMB delay: " << thisDMB->odmb_read_TMB_delay() << cgicc::td();      
     *out << cgicc::td() << "PUSH delay: " << thisDMB->odmb_read_Push_delay() << cgicc::td();      
     *out << cgicc::td() << "ALCT delay: " << thisDMB->odmb_read_ALCT_delay() << cgicc::td();      
     *out << cgicc::tr() << std::endl;
     *out << cgicc::tr();
     *out << cgicc::td() << "Inj delay: " << thisDMB->odmb_read_Inj_delay() << cgicc::td();      
     *out << cgicc::td() << "Ext delay: " << thisDMB->odmb_read_Ext_delay() << cgicc::td();      
     *out << cgicc::td() << "Cal delay: " << thisDMB->odmb_read_Cal_delay() << cgicc::td();      
     *out << cgicc::td() << cgicc::td() << cgicc::tr() << std::endl;
     *out << cgicc::table();
     *out << cgicc::fieldset() << cgicc::br() << std::endl;
  
     std::vector<float> adcs=thisDMB->odmb_fpga_adc();
     *out << std::setprecision(3);
     *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
     *out << std::endl ;
     *out << cgicc::legend("ODMB Temperatures and Voltages").set("style","color:blue") 
          << std::endl ;
     *out << cgicc::table().set("border","1").set("cellpadding","4");
     *out << cgicc::tr();
     *out << cgicc::td() << " FPGA  temperature = " << adcs[0] << "C " << cgicc::td();      
     *out << cgicc::td() << " PCB temperature 1 = " << adcs[6] << "C " << cgicc::td();      
     *out << cgicc::td() << " PCB temperature 2 = " << adcs[3] << "C " << cgicc::td();      
     *out << cgicc::tr() << cgicc::table() << cgicc::br() << std::endl;

     *out << cgicc::table().set("border","1").set("cellpadding","4");
     *out << cgicc::tr();
     *out << cgicc::td() << " FPGA 3.3V = " << adcs[1] << "V " << cgicc::td();      
     *out << cgicc::td() << " FPGA 2.5V = " << adcs[5] << "V " << cgicc::td();      
     *out << cgicc::td() << " FPGA 1.0V = " << adcs[7] << "V " << cgicc::td();      
     *out << cgicc::tr() << std::endl;
     *out << cgicc::tr();
     *out << cgicc::td() << " LVMB 5.0V = " << adcs[8] << "V " << cgicc::td();      
     *out << cgicc::td() << " PPIB 5.0V = " << adcs[2] << "V " << cgicc::td();      
     *out << cgicc::td() << " PPIB 3.3V = " << adcs[4] << "V " << cgicc::td();      
     *out << cgicc::tr() << std::endl;
     *out << cgicc::table();
     *out << cgicc::fieldset() << cgicc::br() << std::endl;
  }

  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << std::endl ;
  //
  *out << cgicc::legend("Chamber Voltages, Temperatures, & Currents").set("style","color:blue") 
       << std::endl ;
  //
  int n=thisDMB->DCSreadAll(buf);
  if(n<=0) 
  {
     *out << "ERROR: Failed to read LVMB!!!" << cgicc::br() << std::endl;
  }
  for(int i=0; i<8*nadcs; i++)
  {   
      if(ubuf[i]==0xBAAD || ubuf[i]==0xFFFF) fvalue[i]=0.0;
      else fvalue[i]=(ubuf[i]&0xFFF)*10.0/4096.0;
  }
  int chn2pos[8];
  for (int i=0; i<8; i++) chn2pos[i]=thisDMB->LVDB_map(i);
  //
  float normv[3]={0.,0.,0.};
  if (hversion<=1) 
     { normv[0]=3.3; normv[1]=5.0; normv[2]=6.0; }
  else if (hversion==2) 
     { normv[0]=3.0; normv[1]=4.0; normv[2]=5.5; }

  *out << cgicc::table().set("border","1").set("cellpadding","4");
  //
  for(int feb=0; feb<cfebs; feb++)
  {
     int lfeb=chn2pos[feb];
     *out << cgicc::tr();
     for(int cnt=0; cnt<3; cnt++)
     {
        val=fvalue[vstart+3*lfeb+cnt];
        sprintf(sbuf, "CFEB%d %3.1fV = %6.2f ",feb+1, normv[cnt], val);
        *out << cgicc::td();
        if ( val < normv[cnt]*(0.95) ||  val > normv[cnt]*(1.05) )	 
           *out << cgicc::span().set("style","color:red");
        else
           *out << cgicc::span().set("style","color:green");
        *out << sbuf;  
        *out << cgicc::span();
        *out << cgicc::td();
     }
     *out << cgicc::tr() << std::endl;
  }
  *out << cgicc::table() << cgicc::br() << std::endl;

  //
  float value;
  //
  *out << cgicc::table().set("border","1");
  //
  *out << cgicc::td();
  value=fvalue[vstart+3*cfebs+0];
  sprintf(buf,"ALCT  3.3V = %3.2f ",value);
  if ( value < 3.3*0.95 ||
       value > 3.3*1.05 ) {
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");  
  }
  *out << buf ;
  *out << cgicc::span();
  *out << cgicc::td();
  //
  *out << cgicc::td();
  value=fvalue[vstart+3*cfebs+1];
  sprintf(buf,"ALCT  1.8V = %3.2f ",value);
  if ( value < 1.8*0.95 ||
       value > 1.8*1.95 ) {
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");  
  }
  *out << buf ;
  *out << cgicc::span();
  *out << cgicc::td();
  //
  *out << cgicc::td();
  value=fvalue[vstart+3*cfebs+2];
  sprintf(buf,"ALCT  5.5V(B) = %3.2f ",value);
  if ( value < 5.5*0.95 ||
       value > 5.5*1.05 ) {
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");  
  }
  *out << buf ;
  *out << cgicc::span();
  *out << cgicc::td();
  //
  *out << cgicc::td();
  value=fvalue[vstart+3*cfebs+3];
  sprintf(buf,"ALCT  5.5V(A) = %3.2f ",value);
  if ( value < 5.5*0.95 ||
       value > 5.5*1.05 ) {
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");  
  }
  *out << buf ;
  *out << cgicc::span();
  *out << cgicc::td();
  //
  *out << cgicc::table();
  //
  *out << cgicc::br();
  //
  *out << cgicc::table().set("border","1");
  //
  *out << cgicc::td();
  value=fvalue[feed];
  sprintf(buf,"Analog  7 V = %3.2f ",value);
  if ( value < 5.0 ||
       value > 7.0*1.5 ) {
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");  
  }
  *out << buf ;
  *out << cgicc::span();
  *out << cgicc::td();
  //
  *out << cgicc::td();
  value=fvalue[feed+1];
  sprintf(buf,"Digital  7 V = %3.2f ",value);
  float ndigif= ((hversion<=1)?7.0:6.0);
  if ( value < ndigif*0.7 ||
       value > ndigif*1.5 ) {
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");  
  }
  *out << buf ;
  *out << cgicc::span();
  *out << cgicc::td();
  //
  *out << cgicc::table();
  //
  *out << cgicc::br();
  //
  if(hversion<=1)
  {
  *out << cgicc::table().set("border","1");;
  //
  *out << cgicc::td();
  sprintf(buf,"DMB temperature = %3.1f C",(value=thisDMB->readthermx(0)));
  if ( value > 10 && value < 45 ) {
    *out << cgicc::span().set("style","color:green");
  } else {
    *out << cgicc::span().set("style","color:red");
  }
  *out << buf ;
  *out << cgicc::span() ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  sprintf(buf,"CFEB1 temperature = %3.1f C",(value=thisDMB->readthermx(1)));
  if ( value > 10 && value < 45 ) {
    *out << cgicc::span().set("style","color:green");
  } else {
    *out << cgicc::span().set("style","color:red");
  }
  *out << buf ;
  *out << cgicc::span() ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  sprintf(buf,"CFEB2 temperature = %3.1f C",(value=thisDMB->readthermx(2)));
  if ( value > 10 && value < 45 ) {
    *out << cgicc::span().set("style","color:green");
  } else {
    *out << cgicc::span().set("style","color:red");
  }
  *out << buf ;
  *out << cgicc::span() ;
  *out << cgicc::td();
  //
  *out << cgicc::tr();
  //
  *out << cgicc::td();
  sprintf(buf,"CFEB3 temperature = %3.1f C",(value=thisDMB->readthermx(3)));
  if ( value > 10 && value < 45 ) {
    *out << cgicc::span().set("style","color:green");
  } else {
    *out << cgicc::span().set("style","color:red");
  }
  *out << buf ;
  *out << cgicc::span() ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  sprintf(buf,"CFEB4 temperature = %3.1f C",(value=thisDMB->readthermx(4)));
  if ( value > 10 && value < 45 ) {
    *out << cgicc::span().set("style","color:green");
  } else {
    *out << cgicc::span().set("style","color:red");
  }
  *out << buf ;
  *out << cgicc::span() ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  sprintf(buf,"CFEB5 temperature = %3.1f C",(value=thisDMB->readthermx(5)));
  if (isME13) { 
    *out << cgicc::span().set("style","color:black");
  } else if ( value > 10 && value < 45 ) {
    *out << cgicc::span().set("style","color:green");
  } else {
    *out << cgicc::span().set("style","color:red");
  }
  *out << buf ;
  *out << cgicc::span() ;
  *out << cgicc::td();
  //
  *out << cgicc::tr();
  //
  *out << cgicc::table();
  //
  *out << cgicc::br();
  //
  *out << cgicc::table().set("border","1");;
  //
  *out << cgicc::td();
  float readout = thisDMB->adcplus(2,0) ;
  sprintf(buf,"DMB DAC1 = %3.1f ",readout);
  //
  if ( readout > 3400 && readout < 3600 ) {
    *out << cgicc::span().set("style","color:black");
  } else {
    *out << cgicc::span().set("style","color:black");
  }
  //
  *out << buf ;
  *out << cgicc::span() ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  readout = thisDMB->adcplus(2,1) ;
  sprintf(buf,"DMB DAC2 = %3.1f ",readout);
  if ( readout > 3400 && readout < 3600 ) {
    *out << cgicc::span().set("style","color:black");
  } else {
    *out << cgicc::span().set("style","color:black");
  }
  //
  *out << buf ;
  *out << cgicc::span() ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  readout = thisDMB->adcplus(2,2) ;
  sprintf(buf,"DMB DAC3 = %3.1f ",readout);
  if ( readout > 3400 && readout < 3600 ) {
    *out << cgicc::span().set("style","color:black");
  } else {
    *out << cgicc::span().set("style","color:black");
  }
  *out << buf ;
  *out << cgicc::span() ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  readout = thisDMB->adcplus(2,3) ;
  sprintf(buf,"DMB DAC4 = %3.1f ",readout);
  if ( readout > 3400 && readout < 3600 ) {
    *out << cgicc::span().set("style","color:black");
  } else {
    *out << cgicc::span().set("style","color:black");
  }
  *out << buf ;
  *out << cgicc::span() ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  readout = thisDMB->adcplus(2,4) ;
  sprintf(buf,"DMB DAC5 = %3.1f ",readout);
  if ( readout > 3400 && readout < 3600 ) {
    *out << cgicc::span().set("style","color:black");
  } else {
    *out << cgicc::span().set("style","color:black");
  }
  *out << buf ;
  *out << cgicc::span() ;
  *out << cgicc::td();
  //
  *out << cgicc::table();
  //
  *out << cgicc::table().set("border","1");;
  //
  *out << cgicc::td();
  readout = thisDMB->adcplus(1,6) ;
  sprintf(buf,"1.8V Chip1 = %3.1f ",readout);
  if ( readout > 1700 && readout < 1900 ) {
    *out << cgicc::span().set("style","color:green");
  } else {
    *out << cgicc::span().set("style","color:red");
  }
  *out << buf ;
  *out << cgicc::span() ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  readout = thisDMB->adcplus(2,6) ;
  sprintf(buf,"1.8V Chip2 = %3.1f ",readout);
  if ( readout > 1700 && readout < 1900 ) {
    *out << cgicc::span().set("style","color:green");
  } else {
    *out << cgicc::span().set("style","color:red");
  }
  *out << buf ;
  *out << cgicc::span() ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  readout = thisDMB->adcplus(3,6) ;
  sprintf(buf,"1.8V Chip3 = %3.1f ",readout);
  if ( readout > 1700 && readout < 1900 ) {
    *out << cgicc::span().set("style","color:green");
  } else {
    *out << cgicc::span().set("style","color:red");
  }
  *out << buf ;
  *out << cgicc::span() ;
  *out << cgicc::td();
  //
  *out << cgicc::table();
  //
  *out << cgicc::br();
  }
  //
  // DMB currents:
  *out << cgicc::table().set("border","1").set("cellpadding","4");
  //
  for(int feb=0; feb<cfebs; feb++)
  {
     int lfeb=chn2pos[feb];
     *out << cgicc::tr();
     for(int cnt=0; cnt<3; cnt++)
     {
        val=fvalue[3*lfeb+cnt];
        sprintf(sbuf, "CFEB%d  %3.1fV, I = %6.2f ",feb+1, normv[cnt], val);
        *out << cgicc::td();
        if(cnt==1 && hversion<=1)
        {
           if ( val< 0.8 ||  val > 1.2 )	 
              *out << cgicc::span().set("style","color:red");
           else
              *out << cgicc::span().set("style","color:green");
        }
        else 
        {
           *out << cgicc::span().set("style","color:black");
        }
        *out << sbuf;  
        *out << cgicc::span();
        *out << cgicc::td();
     }
     *out << cgicc::tr() << std::endl;
  }
  *out << cgicc::table() << cgicc::br() << std::endl;

  // ALCT currents
  //
  *out << cgicc::br();
  //
  *out << cgicc::table().set("border","1");
  //
  *out << cgicc::td();
  value=fvalue[3*cfebs+0];
  sprintf(buf,"ALCT  3.3 V, I = %3.2f ",value);
  *out << buf ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  value=fvalue[3*cfebs+1];
  sprintf(buf,"ALCT  1.8 V, I = %3.2f ",value);
  *out << buf ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  value=fvalue[3*cfebs+2];
  sprintf(buf,"ALCT  5.5V(B), I = %3.2f ",value);
  *out << buf ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  value=fvalue[3*cfebs+3];
  sprintf(buf,"ALCT  5.5(A) V, I = %3.2f ",value);
  *out << buf ;
  *out << cgicc::td();
  //
  *out << cgicc::table();
  //
  *out << cgicc::fieldset();
  *out << std::endl;
  //
  //thisDMB->dmb_readstatus();
  //
}
//
  //
void EmuPeripheralCrateConfig::DMBPrintCounters(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb=0;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      std::cout << "DMB " << dmb << std::endl;
      DMB_ = dmb;
    }
    //
    DAQMB * thisDMB = dmbVector[dmb];
    //
    thisDMB->RedirectOutput(&std::cout);
    thisDMB->PrintCounters(1);
    thisDMB->RedirectOutput(&std::cout);
    //
    this->DMBUtils(in,out);
  }
  //
////////////////////////////////////////////////////////////////////////////////////
// Responses to SOAP messages
////////////////////////////////////////////////////////////////////////////////////
xoap::MessageReference EmuPeripheralCrateConfig::ReadAllVmePromUserid (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  //implement the DMB VME PROM USER_CODE Readback
  //
  std::cout << "DMB VME PROM USER_CODE Readback " << std::endl;

  for(unsigned cv=0; cv<crateVector.size(); cv++) {
    if(!(crateVector[cv]->IsAlive())) continue;
    SetCurrentCrate(cv);
    std::cout << "For Crate " << ThisCrateID_ << " : " << std::endl;

  for (unsigned idmb=0;idmb<dmbVector.size();idmb++) {
    //
    if ((dmbVector[idmb]->slot())<22) {
      DAQMB * thisDMB=dmbVector[idmb];
      unsigned long int boardnumber=thisDMB->mbpromuser(0);
      DMBBoardNumber[cv][idmb]=boardnumber;
      std::cout <<" The DMB Number: "<<idmb<<" is in Slot Number: "<<dmbVector[idmb]->slot()<<std::endl;
      std::cout <<" This DMB Board Number: "<<DMBBoardNumber[cv][idmb]<<std::endl<<std::endl;
    }
    //
  }

  }
  SetCurrentCrate(this_crate_no_);

  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateConfig::LoadAllVmePromUserid (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  //implement the DMB VME PROM USER_CODE programming
  //
  std::cout << "DMB VME PROM USER_CODE Programming " << std::endl;

  for(unsigned cv=0; cv<crateVector.size(); cv++) {
  if(!(crateVector[cv]->IsAlive())) continue;
    SetCurrentCrate(cv);
    std::cout << "For Crate " << ThisCrateID_ << " : " << std::endl;

  usleep(200);
  for (unsigned idmb=0;idmb<dmbVector.size();idmb++) {
    //
    if ((dmbVector[idmb]->slot())<22) {
      DAQMB * thisDMB=dmbVector[idmb];
      unsigned long int boardnumber=DMBBoardNumber[cv][idmb];
      char prombrdname[4];
      //	if (idmb==0) boardnumber = 0xdb00000c;
      //	if (idmb==1) boardnumber = 0xdb00021b;
      //Read database for the board number:
      std::string crate=thisCrate->GetLabel();
      int slot=thisDMB->slot();
      int dmbID=brddb->CrateToDMBID(crate,slot);

      prombrdname[0]=boardnumber&0xff;
      prombrdname[1]=(boardnumber>>8)&0x03;
      prombrdname[2]=0x00;
      prombrdname[3]=0xdb;

      if (((boardnumber&0xfff)==0)||
	  ((boardnumber&0xfff)==0xfff)||
	  ((boardnumber&0xfff)==0xaad)) {
	prombrdname[0]=dmbID&0xff;
	prombrdname[1]=(dmbID>>8)&0x0f;
	std::cout<<" DMB board number reprogram from Database ..."<<std::endl;
      }
      //temperarily overwrite all board number using database
      //	prombrdname[0]=dmbID&0xff;
      //	prombrdname[1]=(dmbID>>8)&0x0f;

        std::cout<<" Loading the board number ..."<<(prombrdname[0]&0xff)+((prombrdname[1]<<8)&0xf00)<<" was set to: "<<(boardnumber&0xffff)<<std::endl;

      thisDMB->epromload_broadcast(VPROM,DMBVmeFirmware_.toString().c_str(),1,prombrdname,2);
      usleep(200);
      std::cout <<" The DMB Number: "<<idmb<<" is in Slot Number: "<<dmbVector[idmb]->slot()<<std::endl;
      //  std::cout <<" This DMB is programmed to board number: "<<boardnumber<<std::endl<<std::endl;
    }
    //
  }

  }
  SetCurrentCrate(this_crate_no_);

  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateConfig::ReadAllCfebPromUserid (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  //implement the CFEB PROM USER_CODE Readback
  //
  std::cout << "CFEB PROM USER_CODE Readback " << std::endl;

  for(unsigned cv=0; cv<crateVector.size(); cv++) {
  if(!(crateVector[cv]->IsAlive())) continue;
    SetCurrentCrate(cv);
    std::cout << "For Crate " << ThisCrateID_ << " : " << std::endl;

  usleep(200);
  for (unsigned idmb=0;idmb<dmbVector.size();idmb++) {
    //
    if ((dmbVector[idmb]->slot())<22) {
      DAQMB * thisDMB=dmbVector[idmb];
      std::cout <<" The DMB Number: "<<idmb<<" is in Slot Number: "<<dmbVector[idmb]->slot()<<std::endl;
      //loop over the cfebs
      //define CFEBs
      std::vector <CFEB> thisCFEBs=thisDMB->cfebs();
      //
      for (unsigned i=0;i<thisCFEBs.size();i++) {
	CFEBBoardNumber[cv][idmb][i]=thisDMB->febpromuser(thisCFEBs[i]);
	std::cout <<" This CFEB Board Number: "<<CFEBBoardNumber[cv][idmb][i]<<std::endl;
      }
      //
      std::cout <<std::endl;
    }
    //
  }

  }
  SetCurrentCrate(this_crate_no_);

  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateConfig::LoadAllCfebPromUserid (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  //implement the CFEB PROM USER_CODE programming
  //
  std::cout << "CFEB PROM USER_CODE Programming " << std::endl;

  for(unsigned cv=0; cv<crateVector.size(); cv++) {
    if(!(crateVector[cv]->IsAlive())) continue;
    SetCurrentCrate(cv);
    std::cout << "For Crate " << ThisCrateID_ << " : " << std::endl;

  for (unsigned idmb=0;idmb<dmbVector.size();idmb++) {
    //
    if ((dmbVector[idmb]->slot())<22) {
      DAQMB * thisDMB=dmbVector[idmb];
      Chamber * thisChamber=chamberVector[idmb];
      std::cout <<" The DMB Number: "<<idmb<<" is in Slot Number: "<<dmbVector[idmb]->slot()<<std::endl;
      //loop over the cfebs
      //define CFEBs
      std::vector <CFEB> thisCFEBs=thisDMB->cfebs();
      //
      for (unsigned i=0;i<thisCFEBs.size();i++) {
	char promid[4];
	unsigned long int boardid=CFEBBoardNumber[cv][idmb][i];
	/*
	  unsigned long int fpgaid=thisDMB->febfpgaid(thisCFEBs[i]);
	  std::cout <<" i= "<<i<<std::endl;
	  if (i==0) boardid=0xcfeb08e5;
	  if (i==1) boardid=0xcfeb08e1;
	  if (i==2) boardid=0xcfeb08e4;
	  if (i==3) boardid=0xcfeb0903;
	  if (i==4) boardid=0xcfeb063a;
	  std::cout <<" This CFEB Board Number should be set to: "<<boardid<<std::endl;
	*/
	std::string chamber=thisChamber->GetLabel();
	int cfebID=brddb->ChamberToCFEBID(chamber,i+1);
	//the id readback from CFEB
	promid[0]=boardid&0xff;
	promid[1]=(boardid>>8)&0xff;
	promid[2]=(boardid>>16)&0xff;
	promid[3]=(boardid>>24)&0xff;
	//
	//the ID readback from database
	if (((boardid&0x00000fff)==0) ||
	    ((boardid&0x00000fff)==0xfff) ||
            ((boardid&0x00000fff)==0xaad)) {
	   promid[0]=cfebID&0xff;
	   std::cout<<" CFEB board number reprogram from Database ..."<<std::endl;
	}
	promid[1]=(cfebID>>8)&0x0f;
	promid[2]=0xeb;
	promid[3]=0xcf;
	int newcfebid;
	newcfebid=(promid[0]&0xff)+((promid[1]<<8)&0xff00);
        //
	thisDMB->epromload_broadcast(thisCFEBs[i].promDevice(),CFEBFirmware_.toString().c_str(),1,promid,2);
	usleep(200);
	        std::cout <<" This CFEB Board Number is set to: "<<newcfebid;
          std::cout <<"     was set to: "<<(boardid&0xffff)<<std::endl;
	std::cout <<" This CFEB Board Number is set to: CFEB"<<std::hex<<((promid[1])&0xff)<<((promid[0])&0xff)<<" was set to: "<<std::hex<<boardid<<std::endl;
      }
      std::cout <<std::endl;
    }
    //
  }

  }
  SetCurrentCrate(this_crate_no_);

  return createReply(message);
}

 }  // namespace emu::pc
}  // namespace emu
