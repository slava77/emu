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

void EmuPeripheralCrateConfig::DDUUtils(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("ddu");
  int ddu;
  if(name != cgi.getElements().end()) {
    ddu = cgi["ddu"]->getIntegerValue();
    std::cout << "DDUUtils:  DDU " << ddu << std::endl;
  } else {
    std::cout << "DDUUtils:  No ddu, use 0" << std::endl ;
    ddu = 0;
  }
  //
  DDU * thisDDU = dduVector[ddu];
  if(thisDDU==NULL) return;
  //
  char Name[100];
  sprintf(Name,"DDU utilities, crate=%s, slot=%d", ThisCrateID_.c_str(),thisDDU->slot());
  //
  MyHeader(in,out,Name);
  //
  *out << cgicc::h1(Name);
  //
  //
}
//
///////////////////////////////////////////////////////////////////////////////////
// DDU status
///////////////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateConfig::DDUStatus(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("ddu");
  int ddu;
  if(name != cgi.getElements().end()) {
    ddu = cgi["ddu"]->getIntegerValue();
    std::cout << "DDU " << ddu << std::endl;
  } else {
    std::cout << "Not ddu, use 0" << std::endl ;
    ddu = 0;
  }
  //
  DDU * thisDDU = dduVector[ddu];
  if(thisDDU==NULL) return;
  //
  char Name[100];
  sprintf(Name,"DDU status, crate=%s, slot=%d", ThisCrateID_.c_str(),thisDDU->slot());	
  //
  MyHeader(in,out,Name);
  //
  //*out << cgicc::h1(Name);
  //*out << cgicc::br();
  //
  char buf[200] ;
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("DDU IDs").set("style","color:blue") << std::endl ;
  //
  *out << cgicc::pre();
  //
  sprintf(buf,"DDU Board ID (decimal) : %d ",thisDDU->readFlashBoardID());
  *out << buf << cgicc::br();
  sprintf(buf,"DDU RUI ID (decimal)   : %d ", thisDDU->readFlashRUI());
  *out << buf << cgicc::br();
  *out << "---------------------------------------";
  *out << cgicc::br();
  sprintf(buf,"DDU VME PROM User Code       : %08x ",thisDDU->VmePromUserCode());
  *out << buf << cgicc::br();
  sprintf(buf,"DDU VME PROM ID Code         : %08x ",thisDDU->VmePromIdCode());
  *out << buf << cgicc::br();
  sprintf(buf,"DDU Control FPGA User Code   : %08x ",thisDDU->CtrlFpgaUserCode());
  *out << buf << cgicc::br();
  sprintf(buf,"DDU Control FPGA ID Code     : %08x ",thisDDU->CtrlFpgaIdCode());
  *out << buf << cgicc::br();
  *out << cgicc::pre();
  *out << cgicc::fieldset()<< cgicc::br() << std::endl ;
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("DDU Settings").set("style","color:blue") << std::endl ;
  //
  *out << cgicc::pre();
  sprintf(buf,"DDU Real FMM   : %04x ",thisDDU->readRealFMM());
  *out << buf << cgicc::br();
  sprintf(buf,"DDU Prescale   : %04x ",thisDDU->readGbEPrescale());
  *out << buf << cgicc::br();
  sprintf(buf,"DDU Fake L1A   : %04x ",thisDDU->readFakeL1());
  *out << buf << cgicc::br();
  sprintf(buf,"DDU Switches   : %04x ",thisDDU->readSwitches());
  *out << buf << cgicc::br();
  *out << cgicc::pre();
  *out << cgicc::fieldset()<< cgicc::br() << std::endl ;
  //

  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl ;
  *out << cgicc::legend("Voltages and Temperatures").set("style","color:blue") << std::endl ;
  //
  std::vector<float> temps=thisDDU->readTempsVolts();
  if(temps.size()>=8)
  {
     *out << cgicc::br() << cgicc::table().set("border","1").set("align","left");
     *out << std::setprecision(2) << std::fixed;
     *out <<cgicc::td() << "Temperatures (C)" << cgicc::td();
     for(unsigned i=0; i<4; i++)
        *out <<cgicc::td() << temps[i] << cgicc::td();
     *out << cgicc::tr() << std::endl;
     *out <<cgicc::td() << "Voltages (V)" << cgicc::td();
     for(unsigned i=4; i<8; i++)
        *out <<cgicc::td() << temps[i] << cgicc::td();
     *out << cgicc::tr() << std::endl;
     *out << cgicc::table() << cgicc::br();
     
  }
  *out << cgicc::fieldset() << std::endl ;
}

 }  // namespace emu::pc
}  // namespace emu
