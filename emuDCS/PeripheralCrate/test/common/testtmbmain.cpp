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
#include "ALCTController.h"
#include "ALCTnew.h"

int  FindTMB_L1A_delay(int,int);
TMB *thisTMB ;

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
  ALCTController *OldAlct;
  ALCTnew * NewAlct;
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
  DDU * thisDDU = thisCrate->ddu();
  myRat = new RAT(thisTMB);

  OldAlct = new ALCTController(thisTMB,"ME21");
  NewAlct = new ALCTnew(thisTMB,"ME21");

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
    std::cout << " 21:Read RAT USER1             22:Read RAT USER2" 
	      << std::endl;
    std::cout << " 23:read RAT-RPC delay         25:Set RAT-RPC delay" 
	      << std::endl;
    std::cout << " 26:reset RPC parity error ctr 27:Read RPC data"
	      << std::endl;
    std::cout << " 29:Read RAT Usercodes         30:Read RAT IDcodes            31:Read TMB-RAT delay"
	      << std::endl;
    std::cout << " 32:Set TMB-RAT Delay          33:Parity err ignore all 1s,0s 34:Parity error don't ignore all 1s,0s"
	      << std::endl;
    std::cout << " 35:Use odd parity             36:Use even parity             37:Test parity bit computation"
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
    std::cout << "113:Set asic patterns         114: Read delay line ctrl reg "
	      << std::endl;
    std::cout << "119:Set power-up dly/patt "
	      << std::endl;
    std::cout << "120: Read configuration reg   121:Set Trigger Mode           122:Set L1a delay"
	      << std::endl;
    std::cout << "123: Set CCB enable           124:Set alct tmode             125:Set Mask all"
	      << std::endl;
    std::cout << "129:Set power-up configuration register"
	      << std::endl;
    std::cout << "130: Read hot channel mask    131:Set hot channel mask       139:Set power-up hot channel mask"
	      << std::endl;
    //
    std::cout << "140: Read testpulse power     141:Set testpulse power switch 149:Set power-up testpulsepower switch"
	      << std::endl;
    std::cout << "150: Read testpulse amplitude 151:Set testpulse amplitude    159:Set power-up testpulsepower amplitude"
	      << std::endl;
    std::cout << "160: Read testpulse groupmask 161:Set testpulse groupmask    169:Set power-up testpulse groupmask"
	      << std::endl;
    std::cout << "170: Read testpulse stripmask 171:Set testpulse stripmask    179:Set power-up testpulse stripmask"
	      << std::endl;
    std::cout << "180: Read AFEB thresholds     181:Set AFEB threshold         189:Set power-up AFEB thresholds"
	      << std::endl;
    std::cout << "190: Read standby register    191:Set standby                199:Set power-up standby register"
	      << std::endl;
    std::cout << std::endl;
    //
    std::cout << " .... Old ALCT .... " << std::endl;
    std::cout << "900: Setup"
	      << std::endl;
    //
    std::cout << std::endl;
    //
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
      myRat->decodeRATUser1();
      break;
    case 22:
      myRat->ReadRatUser2();
      myRat->decodeRATUser2();
      break;
    case 23:
      myRat->read_rpcrat_delay();
      break;
    case 25:
      std::cout << "Current RPC-RAT delay values..." << std::endl;
      myRat->read_rpcrat_delay();

      std::cout << "set RPC0 (0) or RPC1 (1)" << std::endl;
      std::cin >> rpc;

      std::cout << "set delay (0-15)" << std::endl;
      std::cin >> delay;

      myRat->set_rpcrat_delay(rpc,delay);
      break;
    case 26:
      myRat->reset_parity_error_counter();
      break;
    case 27:
      myRat->read_rpc_data();
      break;
    case 29:
      myRat->ReadRatUserCode();
      break;
    case 30:
      myRat->ReadRatIdCode();
      break;
    case 31:
      myRat->read_rattmb_delay();
      break;
    case 32:
      std::cout << "Current RAT-TMB delay value..." << std::endl;
      myRat->read_rattmb_delay();

      std::cout << "set delay (0-15)" << std::endl;
      std::cin >> delay;

      myRat->set_rattmb_delay(delay);
      break;
    case 33:
      myRat->set_perr_ignore();
      break;
    case 34:
      myRat->unset_perr_ignore();
      break;
    case 35:
      myRat->use_parity_odd();
      break;
    case 36:
      myRat->use_parity_even();
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
      NewAlct->setup();
      break;
    case 101:
      NewAlct->ReadSlowControlId();
      NewAlct->PrintSlowControlId();
      break;
    case 102:
      NewAlct->ReadFastControlId();
      NewAlct->PrintFastControlId();
      break;
    case 110:
      NewAlct->ReadAsicDelaysAndPatterns();
      NewAlct->PrintAsicDelays();
      break;
    case 111:
      std::cout << "afeb channel (0-41)" << std::endl;
      std::cin >> channel;
      std::cout << "delay value (0-15) " << std::endl;
      std::cin >> value;
      NewAlct->SetAsicDelay(channel,value);
      NewAlct->WriteAsicDelaysAndPatterns();
      NewAlct->ReadAsicDelaysAndPatterns();
      NewAlct->PrintAsicDelays();
      break;
    case 112:
      NewAlct->ReadAsicDelaysAndPatterns();
      NewAlct->PrintAsicPatterns();
      break;
    case 113:
      std::cout << "layer (0-5)" << std::endl;
      std::cin >> layer;
      std::cout << "channel (0-111) " << std::endl;
      std::cin >> channel;
      std::cout << "OFF (0) or ON (1) " << std::endl;
      std::cin >> value;
      NewAlct->SetAsicPattern(layer,channel,value);
      NewAlct->WriteAsicDelaysAndPatterns();
      NewAlct->ReadAsicDelaysAndPatterns();
      NewAlct->PrintAsicPatterns();
      break;
    case 114:
      NewAlct->ReadDelayLineControlReg();
      NewAlct->PrintDelayLineControlReg();
      break;
    case 119:
      NewAlct->SetPowerUpAsicPatterns();
      NewAlct->SetPowerUpAsicDelays();
      NewAlct->WriteAsicDelaysAndPatterns();
      NewAlct->ReadAsicDelaysAndPatterns();
      NewAlct->PrintAsicDelays();
      NewAlct->PrintAsicPatterns();
      break;
    case 120:
      NewAlct->ReadConfigurationReg();
      NewAlct->PrintConfigurationReg();      
      break;
    case 121:
      std::cout << "trigger mode (1-3) " << std::endl;
      std::cin >> value;
      NewAlct->SetTriggerMode(value);
      NewAlct->WriteConfigurationReg();      
      NewAlct->ReadConfigurationReg();
      NewAlct->PrintConfigurationReg();
      break;
    case 122:
      std::cout << "L1a Delay (0-255) " << std::endl;
      std::cin >> value;
      NewAlct->SetL1aDelay(value);
      NewAlct->WriteConfigurationReg();      
      NewAlct->ReadConfigurationReg();
      NewAlct->PrintConfigurationReg();
      break;
    case 123:
      std::cout << "CCB enable (0-1) " << std::endl;
      std::cin >> value;
      NewAlct->SetCcbEnable(value);
      NewAlct->WriteConfigurationReg();      
      NewAlct->ReadConfigurationReg();
      NewAlct->PrintConfigurationReg();
      break;
    case 124:
      std::cout << "Tmode (0-1) " << std::endl;
      std::cin >> value;
      NewAlct->SetAlctTmode(value);
      NewAlct->WriteConfigurationReg();      
      NewAlct->ReadConfigurationReg();
      NewAlct->PrintConfigurationReg();
      break;
    case 125:
      std::cout << "Mask All (0-1) " << std::endl;
      std::cin >> value;
      NewAlct->SetAlctMaskAll(value);
      NewAlct->WriteConfigurationReg();      
      NewAlct->ReadConfigurationReg();
      NewAlct->PrintConfigurationReg();
      break;
    case 129:
      NewAlct->SetPowerUpConfigurationReg();
      NewAlct->WriteConfigurationReg();      
      NewAlct->ReadConfigurationReg();
      NewAlct->PrintConfigurationReg();
      break;
    case 130:
      NewAlct->ReadHotChannelMask();
      NewAlct->PrintHotChannelMask();      
      break;
    case 131:
      std::cout << "Hot channel mask: Layer (1-6)" << std::endl;
      std::cin >> layer;
      std::cout << "channel (1-112)" << std::endl;
      std::cin >> channel;
      std::cout << "OFF (0) or ON (1) " << std::endl;
      std::cin >> value;
      NewAlct->SetHotChannelMask(layer,channel,value);
      NewAlct->WriteHotChannelMask();      
      NewAlct->ReadHotChannelMask();
      NewAlct->PrintHotChannelMask();
      break;
    case 139:
      NewAlct->SetPowerUpHotChannelMask();
      NewAlct->WriteHotChannelMask();      
      NewAlct->ReadHotChannelMask();
      NewAlct->PrintHotChannelMask();
      break;
    case 140:
      NewAlct->ReadTestpulsePowerSwitchReg();
      NewAlct->PrintTestpulsePowerSwitchReg();
      break;
    case 141:
      std::cout << "Testpulse power switch ON (1) or OFF (0) " << std::endl;
      std::cin >> value;
      NewAlct->SetTestpulsePowerSwitchReg(value);
      NewAlct->WriteTestpulsePowerSwitchReg();      
      NewAlct->ReadTestpulsePowerSwitchReg();
      NewAlct->PrintTestpulsePowerSwitchReg();
      break;
    case 149:
      NewAlct->SetPowerUpTestpulsePowerSwitchReg();
      NewAlct->WriteTestpulsePowerSwitchReg();      
      NewAlct->ReadTestpulsePowerSwitchReg();
      NewAlct->PrintTestpulsePowerSwitchReg();
      break;
    case 150:
      std::cout << "Testpulse amplitude software setting = " 
		<< std::dec << NewAlct->GetTestpulseAmplitude() << std::endl;
      break;
    case 151:
      std::cout << "Testpulse amplitude (0-255)" << std::endl;
      std::cin >> value;
      NewAlct->SetTestpulseAmplitude(value);
      NewAlct->WriteTestpulseAmplitude();      
      std::cout << "Testpulse amplitude written.  The software setting = " 
		<< std::dec << NewAlct->GetTestpulseAmplitude() << std::endl;
      break;
    case 159:
      NewAlct->SetPowerUpTestpulseAmplitude();
      NewAlct->WriteTestpulseAmplitude();      
      std::cout << "Power-up testpulse amplitude written.  The software setting = " 
		<< std::dec << NewAlct->GetTestpulseAmplitude() << std::endl;
      break;
    case 160:
      NewAlct->ReadTestpulseGroupMask();
      NewAlct->PrintTestpulseGroupMask();
      break;
    case 161:
      std::cout << "testpulse groupmask: group (0-6)" << std::endl;
      std::cin >> layer;
      std::cout << "OFF (0) or ON (1) " << std::endl;
      std::cin >> value;
      NewAlct->SetTestpulseGroupMask(layer,value);
      NewAlct->WriteTestpulseGroupMask();      
      NewAlct->ReadTestpulseGroupMask();
      NewAlct->PrintTestpulseGroupMask();
      break;
    case 169:
      NewAlct->SetPowerUpTestpulseGroupMask();
      NewAlct->WriteTestpulseGroupMask();      
      NewAlct->ReadTestpulseGroupMask();
      NewAlct->PrintTestpulseGroupMask();
      break;
    case 170:
      NewAlct->ReadTestpulseStripMask();
      NewAlct->PrintTestpulseStripMask();
      break;
    case 171:
      std::cout << "testpulse stripmask: strip (0-6)" << std::endl;
      std::cin >> layer;
      std::cout << "OFF (0) or ON (1) " << std::endl;
      std::cin >> value;
      NewAlct->SetTestpulseStripMask(layer,value);
      NewAlct->WriteTestpulseStripMask();      
      NewAlct->ReadTestpulseStripMask();
      NewAlct->PrintTestpulseStripMask();
      break;
    case 179:
      NewAlct->SetPowerUpTestpulseStripMask();
      NewAlct->WriteTestpulseStripMask();      
      NewAlct->ReadTestpulseStripMask();
      NewAlct->PrintTestpulseStripMask();
      break;
    case 180:
      NewAlct->ReadAfebThresholds();
      NewAlct->PrintAfebThresholds();
      break;
    case 181:
      std::cout << "threshold: AFEB (0-42)" << std::endl;
      std::cin >> layer;
      std::cout << "value (0-255) " << std::endl;
      std::cin >> value;
      NewAlct->SetAfebThreshold(layer,value);
      NewAlct->WriteAfebThresholds();      
      NewAlct->ReadAfebThresholds();
      NewAlct->PrintAfebThresholds();
      break;
    case 189:
      NewAlct->SetPowerUpAfebThresholds();
      NewAlct->WriteAfebThresholds();      
      NewAlct->ReadAfebThresholds();
      NewAlct->PrintAfebThresholds();
      break;
    case 190:
      NewAlct->ReadStandbyRegister();
      NewAlct->PrintStandbyRegister();
      break;
    case 191:
      std::cout << "standby register: AFEB (0-41)" << std::endl;
      std::cin >> layer;
      std::cout << "OFF (0) or ON (1) " << std::endl;
      std::cin >> value;
      NewAlct->SetStandbyRegister(layer,value);
      NewAlct->WriteStandbyRegister();      
      NewAlct->ReadStandbyRegister();
      NewAlct->PrintStandbyRegister();
      break;
    case 199:
      NewAlct->SetPowerUpStandbyRegister();
      NewAlct->WriteStandbyRegister();      
      NewAlct->ReadStandbyRegister();
      NewAlct->PrintStandbyRegister();
      break;
    case 900:
      OldAlct->setup(1);
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


