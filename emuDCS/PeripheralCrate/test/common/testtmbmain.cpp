#include <iostream>
#include <fstream>
#include <iomanip>
#include <unistd.h> 
#include <string>
//#include <vector>
#include "Crate.h"
#include "TMB.h"
#include "CCB.h"
#include "TMBTester.h"
#include "TestBeamCrateController.h"
#include "PeripheralCrateParser.h"

int main() {

  // Test board OK:
  bool AllOK = false;

  bool DateOK = false;
  bool TypeOK = false;
  bool VersionOK = false;
  bool SlotOK = false;
  bool RevCodeOK = false;
  bool JTAGchainOK = false;
  bool MezzIdOK = false;
  bool PROMIdOK = false;
  bool PROMpathOK = false;
  bool DSNOK = false;
  bool ADCOK = false;
  bool BootRegOK = false;
  bool is3d3444OK = false;
  bool HardResetOK = false;
  bool VMEfpgaDataRegisterOK = false;

  int Menu = 99;

  TestBeamCrateController tbController;

  PeripheralCrateParser parser;
  parser.parseFile("config.xml");

  CrateSelector selector = tbController.selector();
  std::vector<Crate*> crateVector = selector.crates();

  //-- Make sure that only one TMB in one crate is configured
  if (crateVector.size() > 1){
    std::cerr << "Error: only one PeripheralCrate allowed" << std::endl;
    exit(1);
  }
  std::vector<TMB*> tmbVector = selector.tmbs(crateVector[0]);
  if (tmbVector.size() > 1){
    std::cerr << "Error: only one TMB in xml file allowed" << std::endl;
    exit(1);
  }

  //-- get pointers to CCB, TMB and DMB
  Crate *thisCrate = crateVector[0];
  TMB *thisTMB ;
  CCB *thisCCB ;

  thisCCB = thisCrate->ccb();
  thisTMB = tmbVector[0];

  // !Martin!, the following needs to be obtained from the xml file somehow
  int thisTMBslot = 15;

  // point the TMBTester to the classes we need:
  TMBTester testTMB;

  testTMB.setTMB(thisTMB);
  testTMB.setCCB(thisCCB);
  testTMB.setTMBslot(&thisTMBslot);

  while (Menu < 100 && Menu >=0 ){

    Menu = 99;

    std::cout << std::endl;
    std::cout << " TMB TEST program " << std::endl;
    std::cout << "  0:Init System " 
	      << std::endl;
    std::cout << "  1:Run All Tests, or singly test one of the following... " 
              << std::endl;
    std::cout << "  2:Boot Register             3:TMB Hard Reset            4:VME FPGA data reg" 
              << std::endl;
    std::cout << "  5:Slot                      6:Firmware Date             7:Firmware Type"
              << std::endl;
    std::cout << "  8:Firmware Version          9:Firmware Rev Code        10:JTAG chains"
              << std::endl;
    std::cout << " 11:Mezzanine ID             12:PROM ID                  13:PROM Path"
              << std::endl;
    std::cout << " 14:Digital Serial # (DSN)   15:ADC status               16:3d3444 status"
              << std::endl;
    std::cout << "  99:Read Register 4 " << std::endl;
    std::cout << "  100:Exit " << std::endl;
    std::cout << " menu choice? (Default = 99)" << std::endl;
    std::cin >> Menu;

    switch (Menu) {
    case 0:
      testTMB.reset();
      break;
    case 1:
      AllOK = testTMB.runAllTests();
      break;
    case 2:
      BootRegOK = testTMB.testBootRegister();
      break;
    case 3:
      HardResetOK = testTMB.testHardReset();
      break;
    case 4:
      VMEfpgaDataRegisterOK = testTMB.testVMEfpgaDataRegister();
      break;
    case 5:
      SlotOK = testTMB.testFirmwareSlot();      
      break;      
    case 6:
      DateOK = testTMB.testFirmwareDate();
      break;
    case 7:
      TypeOK = testTMB.testFirmwareType();      
      break;      
    case 8:
      VersionOK = testTMB.testFirmwareVersion();      
      break;      
    case 9:
      RevCodeOK = testTMB.testFirmwareRevCode();      
      break;      
    case 10:
      JTAGchainOK = testTMB.testJTAGchain();      
      break;      
    case 11:
      MezzIdOK = testTMB.testMezzId();      
      break;      
    case 12:
      PROMIdOK = testTMB.testPROMid();      
      break;      
    case 13:
      PROMpathOK = testTMB.testPROMpath();      
      break;      
    case 14:
      DSNOK = testTMB.testDSN();      
      break;      
    case 15:
      ADCOK = testTMB.testADC();      
      break;      
    case 16:
      is3d3444OK = testTMB.test3d3444();      
      break;      
    case 99:
      testTMB.readreg4();      
      break;      
    default:
      std::cout << "Unknown Menu Option =" << Menu << std::endl; 
      Menu=100;
    case 100:
      std::cout << "Exiting..." << std::endl;
      break;
    }
  }

  return 0;
}


