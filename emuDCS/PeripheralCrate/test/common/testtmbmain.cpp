#include <iostream>
#include <fstream>
#include <iomanip>
#include <unistd.h> 
#include <string>
//#include <vector>
#include "Crate.h"
#include "TMB.h"
#include "DAQMB.h"
#include "RAT.h"
#include "CCB.h"
#include "TMBTester.h"
#include "EmuController.h"
#include "PeripheralCrateParser.h"
#include "ChamberUtilities.h"
#include "TMB_constants.h"

int  FindTMB_L1A_delay(int,int);
TMB *thisTMB ;

int main() {


  int rpc;
  int delay;
  int trigger;

  // Test board OK:
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
  bool isALCTtxrxOK = false;
  bool isRATtemperOK = false;
  bool isRATidCodesOK = false;
  bool isRATuserCodesOK = false;
  bool HardResetOK = false;
  bool VMEfpgaDataRegisterOK = false;
  bool isU76OK = false;

  int Menu = 999;

  int value,channel,layer;

  CCB *thisCCB ;
  DAQMB *thisDMB;
  ALCTController *alct;
  MPC *thisMPC;
  RAT *myRat;


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
  std::vector<DAQMB*> dmbVector = selector.daqmbs(crateVector[0]);
  if (dmbVector.size() > 1){
    std::cerr << "Error: only one DMB in xml file allowed" << std::endl;
    exit(1);
  }
  
  //-- get pointers to CCB, TMB and DMB
  Crate *thisCrate = crateVector[0];

  thisCCB = thisCrate->ccb();
  thisTMB = tmbVector[0];
  thisDMB = dmbVector[0];
  thisMPC = thisCrate->mpc();
  //  DDU * thisDDU = thisCrate->ddu();
  myRat = new RAT(thisTMB);

  alct = new ALCTController(thisTMB,"ME21");

  // point the TMBTester to the classes we need:
  TMBTester testTMB;
  testTMB.setTMB(thisTMB);
  testTMB.setCCB(thisCCB);
  testTMB.setRAT(myRat);

  // point the ChamberUtilities to the classes we need:
  ChamberUtilities util;
  util.SetCCB(thisCCB);
  util.SetTMB(thisTMB);
  util.SetDMB(thisDMB);
  util.SetMPC(thisMPC);


  while (Menu < 1000 && Menu >=0 ){

    Menu = 999;

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
    std::cout << " 17:ALCT TX/RX cables          18:RAT temperature OK          19:RAT ID codes"
              << std::endl;
    std::cout << " 70:RAT User codes             71:TMB U76 bus-hold chip"
              << std::endl;
    std::cout << std::endl;
    std::cout << " 21:Read RAT USER1" 
	      << std::endl;
    std::cout << " 23:read RAT-RPC delay         25:Set RAT-RPC delay" 
	      << std::endl;
    std::cout << " 26:reset RPC parity error ctr"
	      << std::endl;
    std::cout << " 29:Read RAT Usercodes         30:Read RAT IDcodes            31:Read TMB-RAT delay"
	      << std::endl;
    std::cout << " 32:Set TMB-RAT Delay          "
	      << std::endl;
    std::cout << " 37:Test parity bit computation"
	      << std::endl;
    //
    std::cout << std::endl;
    //
    std::cout << " 90:enable PC->CLCTex from TTC 91:Read scope                  92:TTC command received" 
	      << std::endl;
    std::cout << " 93:TMB Raw hits dump          94:Print counters              95:Reset counters"
	      << std::endl;
    std::cout << " 96:Who triggered TMB?"
	      << std::endl;
    //
    std::cout << std::endl;
    //
    std::cout << " .... New ALCT .... " << std::endl;
    std::cout << "100: Setup                    101:Read slow control ID       102:Read fast control ID"
	      << std::endl;
    std::cout << "110: Read asic delays         111:Set asic delays            112: Read asic patterns"
	      << std::endl;
    std::cout << "113:Set asic patterns         119:Set power-up dly/patt "
	      << std::endl;
    std::cout << "120: Read configuration reg   121:Set Trigger Mode           122:Set L1a delay"
	      << std::endl;
    std::cout << "123: Set CCB enable           129:Set power-up configuration register"
	      << std::endl;
    std::cout << "130: Read hot channel mask    131:Set hot channel mask       139:Set power-up hot channel mask"
	      << std::endl;
    std::cout << "180: Read AFEB thresholds     181:Set AFEB threshold         189:Set power-up AFEB thresholds"
	      << std::endl;
    std::cout << std::endl;
    //
    std::cout << "XSVF file handling" << std::endl;
    std::cout << "200: Write data file          201:Read data file"
	      << std::endl;
    std::cout << "300: Write xsvf files         301:Read xsvf file"
	      << std::endl;
    std::cout << "400: Program user proms       401:Verify user prom           402:Program TMB"
	      << std::endl;
    std::cout << "500: Check state machine        "
	      << std::endl;
    //
    std::cout << std::endl;
    std::cout << "1000:Exit " << std::endl;
    std::cout << " menu choice? (Default = 999)" << std::endl;
    std::cin >> Menu;

    switch (Menu) {
    case 0:
      testTMB.reset();
      break;
    case 1:
      testTMB.runAllTests();
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
    case 17:
      isALCTtxrxOK = testTMB.testALCTtxrx();      
      break;      
    case 18:
      isRATtemperOK = testTMB.testRATtemper();      
      break;      
    case 19:
      isRATidCodesOK = testTMB.testRATidCodes();
      break;
    case 70:
      isRATuserCodesOK = testTMB.testRATuserCodes();
      break;
    case 71:
      isU76OK = testTMB.testU76chip();
      break;
      //
      //
    case 21:
      myRat->ReadRatUser1();
      myRat->PrintRatUser1();
      break;
    case 23:
      myRat->ReadRatUser1();
      myRat->PrintRpcRatDelay();
      break;
    case 25:
      std::cout << "Current RPC-RAT delay values..." << std::endl;
      myRat->ReadRatUser1();
      myRat->PrintRpcRatDelay();

      std::cout << "set RPC0 (0) or RPC1 (1)" << std::endl;
      std::cin >> rpc;

      std::cout << "set delay (0-15)" << std::endl;
      std::cin >> delay;

      myRat->SetRpcRatDelay(rpc,delay);
      myRat->WriteRpcRatDelay();
      myRat->PrintRpcRatDelay();
      break;
    case 26:
      myRat->reset_parity_error_counter();
      break;
    case 29:
      myRat->ReadRatUserCode();
      break;
    case 30:
      myRat->ReadRatIdCode();
      break;
    case 31:
      myRat->ReadRatTmbDelay();
      myRat->PrintRatTmbDelay();
      break;
    case 32:
      std::cout << "Current RAT-TMB delay value..." << std::endl;
      myRat->ReadRatTmbDelay();
      myRat->PrintRatTmbDelay();

      std::cout << "set delay (0-15)" << std::endl;
      std::cin >> delay;

      myRat->SetRatTmbDelay(delay);
      myRat->WriteRatTmbDelay();
      myRat->ReadRatTmbDelay();
      myRat->PrintRatTmbDelay();      
      break;
    case 37:
      std::cout << "compute parity for RPC0 (0) or RPC1 (1)" << std::endl;
      std::cin >> rpc;

      testTMB.RpcComputeParity(rpc);
      break;
      //
      //
    case 90:
      std::cout << "Fully reset Peripheral crate..." << std::endl;
      util.InitSystem();
      ::sleep(1);

      std::cout << "Setting CCB into FPGA mode..." << std::endl;
      thisCCB->setCCBMode(CCB::VMEFPGA);
      std::cout << "Now CCB register 0x20 reads: " << thisCCB->ReadRegister(0x20) << std::endl;
      ::sleep(1);

      std::cout << "Setting CCB to transmit TTCrx commands on backplane(still in FPGA mode)..." << std::endl;
      thisCCB->WriteRegister(0x20,0);
      std::cout << "Now CCB register 0x20 reads: " << thisCCB->ReadRegister(0x20) << std::endl;
      ::sleep(1);

      std::cout << "Enable TTC to trigger(to stop blinking lights)..." << std::endl;            
      thisTMB->StartTTC();
      ::sleep(1);

      std::cout << "Disable CFEB inputs to triggers (to stop blinking lights)..." << std::endl;      
      thisTMB->DisableCLCTInputs();
      ::sleep(1);

      std::cout << "Enable external CLCT for TMB sequencer trigger..." << std::endl;      
      thisTMB->ExtClctTrigFromCCBonly();
      ::sleep(1);


      //      std::cout << "Enable L1A to come from CCB upon request (from TMB)..." << std::endl;      


      //      7) look at scope for receipt of RPC data
      //      8) set window width to latch RPC data
      //      9) do TMB-L1A timing scan 
      break;
    case 91:
      std::cout << "trigger (0-127) " << std::endl;
      std::cout << "                0  = sequencer trigger" << std::endl;      
      std::cout << "                96 = rpc0_bxn[0]" << std::endl;
      std::cout << "               112 = rpc0_nhits[0]" << std::endl;
      std::cin >> trigger;

      thisTMB->scope(1,0,trigger);  // arm the scope to trigger on a particular channel
      //      thisTMB->scope(1,0,0);  // arm the scope to trigger on a particular channel
      ::sleep(1);
      thisTMB->scope(0,1);          // read the scope (triggered on the chosen trigger)
      break;
    case 92:
      std::cout << "TTC command at TMB: " << std::hex 
		<< thisTMB->CCB_command_from_TTC() 
		<< std::endl;
      break;
      //
    case 93:
      thisTMB->TMBRawhits();
      break;
    case 94:
      thisTMB->GetCounters();
      thisTMB->PrintCounters();
      break;
    case 95:
      thisTMB->ResetCounters();
      break;
    case 96:
      std::cout << "TMB trigger source = " << std::hex 
		<< thisTMB->ReadRegister(seq_trig_src_adr) << std::endl;
      break;
      //
    case 100:
      alct->configure();
      break;
    case 101:
      alct->ReadSlowControlId();
      alct->PrintSlowControlId();
      break;
    case 102:
      alct->ReadFastControlId();
      alct->PrintFastControlId();
      break;
    case 110:
      alct->ReadAsicDelaysAndPatterns();
      alct->PrintAsicDelays();
      break;
    case 111:
      std::cout << "afeb channel (0-41)" << std::endl;
      std::cin >> channel;
      std::cout << "delay value (0-15) " << std::endl;
      std::cin >> value;
      alct->SetAsicDelay(channel,value);
      alct->WriteAsicDelaysAndPatterns();
      alct->ReadAsicDelaysAndPatterns();
      alct->PrintAsicDelays();
      break;
    case 112:
      alct->ReadAsicDelaysAndPatterns();
      alct->PrintAsicPatterns();
      break;
    case 113:
      std::cout << "layer (0-5)" << std::endl;
      std::cin >> layer;
      std::cout << "channel (0-111) " << std::endl;
      std::cin >> channel;
      std::cout << "OFF (0) or ON (1) " << std::endl;
      std::cin >> value;
      alct->SetAsicPattern(layer,channel,value);
      alct->WriteAsicDelaysAndPatterns();
      alct->ReadAsicDelaysAndPatterns();
      alct->PrintAsicPatterns();
      break;
    case 119:
      alct->SetPowerUpAsicPatterns();
      alct->SetPowerUpAsicDelays();
      alct->WriteAsicDelaysAndPatterns();
      alct->ReadAsicDelaysAndPatterns();
      alct->PrintAsicDelays();
      alct->PrintAsicPatterns();
      break;
    case 120:
      alct->ReadConfigurationReg();
      alct->PrintConfigurationReg();      
      break;
    case 121:
      std::cout << "trigger mode (1-3) " << std::endl;
      std::cin >> value;
      alct->SetTriggerMode(value);
      alct->WriteConfigurationReg();      
      alct->ReadConfigurationReg();
      alct->PrintConfigurationReg();
      break;
    case 122:
      std::cout << "L1a Delay (0-255) " << std::endl;
      std::cin >> value;
      alct->SetL1aDelay(value);
      alct->WriteConfigurationReg();      
      alct->ReadConfigurationReg();
      alct->PrintConfigurationReg();
      break;
    case 123:
      std::cout << "CCB enable (0-1) " << std::endl;
      std::cin >> value;
      alct->SetCcbEnable(value);
      alct->WriteConfigurationReg();      
      alct->ReadConfigurationReg();
      alct->PrintConfigurationReg();
      break;
    case 129:
      alct->SetPowerUpConfigurationReg();
      alct->WriteConfigurationReg();      
      alct->ReadConfigurationReg();
      alct->PrintConfigurationReg();
      break;
    case 130:
      alct->ReadHotChannelMask();
      alct->PrintHotChannelMask();      
      break;
    case 131:
      std::cout << "Hot channel mask: Layer (1-6)" << std::endl;
      std::cin >> layer;
      std::cout << "channel (1-112)" << std::endl;
      std::cin >> channel;
      std::cout << "OFF (0) or ON (1) " << std::endl;
      std::cin >> value;
      alct->SetHotChannelMask(layer,channel,value);
      alct->WriteHotChannelMask();      
      alct->ReadHotChannelMask();
      alct->PrintHotChannelMask();
      break;
    case 139:
      alct->SetPowerUpHotChannelMask();
      alct->WriteHotChannelMask();      
      alct->ReadHotChannelMask();
      alct->PrintHotChannelMask();
      break;
    case 180:
      alct->ReadAfebThresholds();
      alct->PrintAfebThresholds();
      break;
    case 181:
      std::cout << "threshold: AFEB (0-42)" << std::endl;
      std::cin >> layer;
      std::cout << "value (0-255) " << std::endl;
      std::cin >> value;
      alct->SetAfebThreshold(layer,value);
      alct->WriteAfebThresholds();      
      alct->ReadAfebThresholds();
      alct->PrintAfebThresholds();
      break;
    case 189:
      alct->SetPowerUpAfebThresholds();
      alct->WriteAfebThresholds();      
      alct->ReadAfebThresholds();
      alct->PrintAfebThresholds();
      break;
    case 200:
      alct->SetXsvfFilename("dummy_data");
      alct->CreateUserPromFile();
      break;
    case 201:      
      alct->SetXsvfFilename("dummy_data");
      if ( alct->ReadUserPromFile() ) {
	std::cout << "address (0-32767) " << std::endl;
	std::cin >> value;
	std::cout << "address " << std::hex << value 
		  << " = " << alct->GetUserPromImage(value) 
		  << std::endl;
      } else {
	
      }
      break;
    case 300:
      alct->SetWhichUserProm(ChipLocationTmbUserPromTMB);
      alct->SetXsvfFilename("prom0_test");
      alct->CreateXsvfFile();
      //
      alct->SetWhichUserProm(ChipLocationTmbUserPromALCT);
      alct->SetXsvfFilename("prom1_test");
      alct->CreateXsvfFile();
      break;
    case 301:      
      alct->SetXsvfFilename("prom0_test");
      alct->ReadXsvfFile(true);
      alct->SetXsvfFilename("prom1_test");
      alct->ReadXsvfFile(true);
      break;
    case 400:
      alct->SetWhichUserProm(ChipLocationTmbUserPromTMB);
      alct->SetXsvfFilename("prom0_test");
      alct->ProgramUserProm();
      //
      alct->SetWhichUserProm(ChipLocationTmbUserPromALCT);
      alct->SetXsvfFilename("prom1_test");
      alct->ProgramUserProm();
      break;
    case 401:
      alct->SetWhichUserProm(ChipLocationTmbUserPromALCT);
      alct->SetXsvfFilename("dummy_data");
      alct->VerifyUserProm();
      break;
    case 402:
      alct->SetXsvfFilename("tmb2005e_28july2006_noverify");
      alct->ProgramTMBProms();
      break;
    case 500:
      unsigned short int BootData;
      layer = thisTMB->tmb_get_boot_reg(&BootData);
      std::cout << "Boot contents before = " << std::hex << BootData << std::endl;
      testTMB.reset();      
      alct->CheckVMEStateMachine();
      alct->CheckJTAGStateMachine();
      break;
    default:
      std::cout << "Unknown Menu Option =" << Menu << std::endl; 
      Menu = 1000;
    case 1000:
      std::cout << "Exiting..." << std::endl;
      break;
    }
  }

  return 0;
}


