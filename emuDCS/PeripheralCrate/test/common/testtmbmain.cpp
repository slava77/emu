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
#include "EmuController.h"
#include "PeripheralCrateParser.h"

int  FindTMB_L1A_delay(int,int);
TMB *thisTMB ;
CCB *thisCCB ;

int main() {

  int dummy;
  int L1adelay;
  int rpc;
  int delay;
  int trigger;

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

  //TestBeamCrateController tbController;

  EmuController emuController;

  //PeripheralCrateParser parser;
  //parser.parseFile("config.xml");

  emuController.SetConfFile("config.xml");
  emuController.init();

  CrateSelector selector = emuController.selector();
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

  thisCCB = thisCrate->ccb();
  thisTMB = tmbVector[0];

  // !Martin!, the following needs to be obtained from the xml file somehow
  int thisTMBslot = 14;

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
    std::cout << "  2:Boot Register               3:TMB Hard Reset               4:VME FPGA data reg" 
              << std::endl; 
    std::cout << "  5:Slot                        6:Firmware Date                7:Firmware Type"
              << std::endl;
    std::cout << "  8:Firmware Version            9:Firmware Rev Code           10:JTAG chains"
              << std::endl;
    std::cout << " 11:Mezzanine ID               12:PROM ID                     13:PROM Path"
              << std::endl;
    std::cout << " 14:Digital Serial Numbers     15:Voltages, Currents, Temps   16:3d3444 status"
              << std::endl;
    std::cout << " 20:TMB-RAT 80MHz Delay scan   21:Read RAT USER1              22:Read RAT USER2" 
	      << std::endl;
    std::cout << " 23:read RAT delay             24:RAT-RPC delay scan          25:Set RAT-RPC delay" 
	      << std::endl;
    std::cout << " 26:reset RAT parity error ctr 27:Read RPC data               28:Read RAT scope"
	      << std::endl;
    std::cout << " 90:enable CLCT ext trigger    91:Internal L1A from CCB       92:TMB-L1A delay scan" 
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
    case 20:
      testTMB.RatTmbDelayScan();
      break;
    case 21:
      testTMB.ReadRatUser1();
      break;
    case 22:
      dummy = testTMB.ReadRatUser2();
      break;
    case 23:
      dummy = testTMB.read_rat_delay();
      break;
    case 24:
      std::cout << "scan RPC0 (0) or RPC1 (1)" << std::endl;
      std::cin >> rpc;

      testTMB.RpcRatDelayScan(rpc);
      break;
    case 25:
      std::cout << "set RPC0 (0) or RPC1 (1)" << std::endl;
      std::cin >> rpc;

      std::cout << "set delay (0-15)" << std::endl;
      std::cin >> delay;

      testTMB.set_rat_delay(rpc,delay);
      break;
    case 26:
      std::cout << "set RPC0 (0) or RPC1 (1)" << std::endl;
      std::cin >> rpc;

      testTMB.reset_parity_error_counter(rpc);
      break;
    case 27:
      std::cout << "set RPC0 (0) or RPC1 (1)" << std::endl;
      std::cin >> rpc;

      testTMB.read_rpc_data(rpc);
      break;
    case 28:
      std::cout << "trigger (0-127)" << std::endl;
      std::cin >> trigger;

      thisTMB->scope(1,0,trigger);

      thisTMB->scope(0,1,96);
      thisTMB->scope(0,1,97);
      thisTMB->scope(0,1,98);
      break;
    case 90:
      testTMB.SetExtCLCTPatternTrigger();
      break;
    case 91:
      thisTMB->StartTTC();
      break;
    case 92:
      std::cout << "Enter lowest TMB L1A delay value to loop over (bunch crossings, decimal)" << std::endl;
      int idelay_min;
      std::cin >> idelay_min;
      std::cout << "Enter highest TMB L1A delay value to loop over (bunch crossings, decimal)" << std::endl;
      int idelay_max;
      std::cin >> idelay_max;
 
      L1adelay = FindTMB_L1A_delay(idelay_min,idelay_max);
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


int FindTMB_L1A_delay( int idelay_min, int idelay_max ){

  float RightTimeBin = 0;
  int   DataCounter  = 0;

  for (int i = idelay_min; i < idelay_max+1; i++){
    
    thisTMB->lvl1_delay(i);

    thisTMB->ResetCounters();                             // reset counters

    std::cout << "TMB_l1adelay= " << i << ":" << std::endl;
    ::sleep(3);                                           // accumulate statistics
    thisTMB->GetCounters();                               // read counter values
    
    thisTMB->PrintCounters(8);                            // display them to screen
    thisTMB->PrintCounters(19);
    thisTMB->PrintCounters(20);

    if (thisTMB->GetCounter(19) > 0) {
      RightTimeBin += i;
      DataCounter++;
    }

  }

  RightTimeBin /= (float) DataCounter;
  
  std::cout << "Right L1a delay setting is " << RightTimeBin << std::endl;

  return (int) RightTimeBin ;

}

