#include <iostream>
#include <fstream>
#include <sstream>
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
//
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
  //
  int Menu = 999;
  //
  const int MAX_NUM_LCTS_IN_FILE = 1000;
  unsigned long int lct0[MAX_NUM_LCTS_IN_FILE];
  unsigned long int lct1[MAX_NUM_LCTS_IN_FILE];
  //
  int lctCounter = 0;
  //
  std::ifstream PatternFile;
  PatternFile.open("patterns.txt");
  if ( PatternFile.is_open() ) {
    while (PatternFile.good()) {
      //
      // pattern file has format "MPC[0-1] XXXXXXXX"  where XXXXXXXX is the LCT in hex format
      //
      std::string line;
      std::getline(PatternFile,line);
      //
      std::istringstream instring(line);
      //
      std::string MPC1or2;
      unsigned long int lctValue;
      instring >> MPC1or2 >> std::hex >> lctValue;
      //
      //      std::cout << MPC1or2 << " = 0x" << std::hex << lctValue << std::endl;
      if (lctCounter < MAX_NUM_LCTS_IN_FILE) {
	if (MPC1or2 == "MPC1") {
	  lct0[lctCounter] = lctValue;
	} else if (MPC1or2 == "MPC2") {
	  lct1[lctCounter++] = lctValue;
	}
      }
    }
  }
  //
  PatternFile.close();
  //
  // at the moment the two frames are reversed in the file... switch them to be correct here:
  //  std::cout << "LCT 0 are..." << std::endl;
  for (int i=0; i<lctCounter; i++) {
    //    std::cout << std::dec << std::setw(4) << i << "  0x" << std::hex << lct0[i] << std::endl;
    unsigned long temp = ( ((lct0[i] & 0x0000ffff) << 16) | ((lct0[i] & 0xffff0000) >> 16) );
    lct0[i] = temp;
    //    std::cout << "  ->  0x" << std::hex << lct0[i] << std::endl;
  }
  //
  //  std::cout << "LCT 1 are..." << std::endl;
  for (int i=0; i<lctCounter; i++) {
    //    std::cout << std::dec << std::setw(4) << i << "  0x" << std::hex << lct1[i] << std::endl;
    unsigned long temp = ( ((lct1[i] & 0x0000ffff) << 16) | ((lct1[i] & 0xffff0000) >> 16) );
    lct1[i] = temp;
    //    std::cout << "  ->  0x" << std::hex << lct1[i] << std::endl;
  }
  
  //
  int value,channel,layer;
  int write_data, read_data;
  int busy, verify;
  //
  const bool debug = false;
  //
  std::ofstream outputfile;
  //
  int npulse;
  //
  // for pulsing ALCT teststrips:
  const int pulse_signal = ADB_SYNC;
  float amplitude = 25.;
  int strip_mask_int = 0xff;
  //
  const int ccb_l1a_delay = 174;       // This value is good for tmb::l1a_delay = 158
  int number_of_l1a_at_tmb_histo[255]; // for measuring tmb_l1a_delay, given ccb_l1a_delay
  //
  const int total_number_of_pulses_per_distrip = 10;   //for measuring distrip occupation distribution
  const int number_of_hs = 160;
  int clct0_keyhs_histo[number_of_hs];
  int clct1_keyhs_histo[number_of_hs];
  //
  const int total_number_of_pulses = 101;             //for measuring ALCT in CLCT match window
  const int minbin = 0;
  const int maxbin = 15;
  int AlctInClctMatchWindowHisto[16];
  float average;
  //
  int strip_mask[7];
  int HalfStrip;
  int hp[6];
  //
  std::string tmb_firmware_string = "/home/rakness/firmware/tmb/20070813/tmb";
  char alct_firmware_string[100] = "/home/rakness/firmware/alct/20070828/alct672/alct672.svf";
  char rat_firmware_string[100] = "/home/rakness/firmware/rat/20060828/rat.svf";
  int jch, debug_mode;
  //
  const int number_of_chambers_to_check = 117;
  bool tmb_config_check[number_of_chambers_to_check];
  bool alct_config_check[number_of_chambers_to_check];
  time_t starttime, endtime;
  time_t starttimesubtract, endtimesubtract;
  int time_elapsed;
  int total_time_to_subtract, net_time_elapsed;
  int total_tmb_time , tmbstarttime ,  tmbstoptime ;
  int total_alct_time, alctstarttime,  alctstoptime;
  int total_dmb_time , dmbstarttime ,  dmbstoptime ;
  //
  int alternate;
  int tmp_setting;
  float original_comp_thresh_setting;
  float float_tmp_setting;
  //
  EmuController emuController;
  //
  emuController.SetConfFile("config.xml");
  emuController.init();
  //
  CrateSelector selector = emuController.selector();
  std::vector<Crate*> crateVector = selector.crates();
  //
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
  //
  Crate *thisCrate = crateVector[0];
  CCB * thisCCB = thisCrate->ccb();
  DAQMB * thisDMB = dmbVector[0];
  MPC * thisMPC = thisCrate->mpc();
  TMB * thisTMB = tmbVector[0];
  RAT * myRat = thisTMB->getRAT();
  ALCTController * alct = thisTMB->alctController();
  //
  //TMB * thisTMB;
  //  RAT * myRat;
  //  ALCTController * alct;
  //
  TMBTester testTMB;
  testTMB.setTMB(thisTMB);
  testTMB.setCCB(thisCCB);
  testTMB.setRAT(myRat);
  //
  ChamberUtilities util;
  util.SetCCB(thisCCB);
  util.SetTMB(thisTMB);
  util.SetDMB(thisDMB);
  util.SetMPC(thisMPC);
  //
  while (Menu < 1000 && Menu >=0 ){
    //
    Menu = 999;
    //
    std::cout << std::endl;
    std::cout << " TMB TEST program " << std::endl;
    std::cout << "  0:Init System                 1:Run All TMB tests                                                 " << std::endl;
    //std::cout << "  2:Boot Register               3:TMB Hard Reset              4:VME FPGA data reg                   " << std::endl; 
    std::cout << "  5:Slot                        6:Firmware Date               7:Firmware Type                       " << std::endl;
    std::cout << "  8:Firmware Version            9:Firmware Rev Code          10:JTAG chains                         " << std::endl;
    std::cout << " 11:Mezzanine ID               12:PROM ID                    13:PROM Path                           " << std::endl;
    //std::cout << " 14:Digital Serial Numbers     15:Voltages, Currents, Temps  16:3d3444 status                       " << std::endl;
    //std::cout << " 17:ALCT TX/RX cables          18:RAT temperature OK         19:RAT ID codes                        " << std::endl;
    //std::cout << " 70:RAT User codes             71:TMB U76 bus-hold chip                                             " << std::endl;
    //std::cout << std::endl;
    std::cout << " 21:Read RAT USER1             23:read RAT-RPC delay         25:Set RAT-RPC delay                   " << std::endl;
    //std::cout << " 26:reset RPC parity error ctr 29:Read RAT Usercodes         30:Read RAT IDcodes                    " << std::endl;
    //std::cout << " 31:Read TMB-RAT delay         32:Set TMB-RAT Delay          37:Test parity bit computation         " << std::endl;
    //std::cout << " 38:Test RAT values from TMB   39:configure RAT                                                     " << std::endl;
    //std::cout << std::endl;
    //std::cout << " 87:CFEB timing scan           88:ALCT timing scan                                                  " << std::endl;
    //std::cout << " 90:enable PC->CLCTex from TTC 91:Read scope                  92:TTC command received               " << std::endl;
    //std::cout << " 93:TMB Raw hits dump          94:Print counters              95:Reset counters                     " << std::endl;
    //std::cout << " 96:Who triggered TMB?         97:Dump all TMB registers      98:Clock out TMB user prom            " << std::endl;
    //std::cout << " 99:Distrip HotChannel mask    94:Print counters              95:Reset counters                     " << std::endl;
    std::cout << std::endl;
    std::cout << "101:Read ALCT slow control ID 102: Read ALCT fast control ID                                        " << std::endl;
    //std::cout << "110: Read asic delays         111:Set asic delays            112: Read asic patterns                " << std::endl;
    //std::cout << "113: Set asic patterns        119:Set power-up dly/patt                                             " << std::endl;
    //std::cout << "120: Read configuration reg   121:Set Trigger Mode           122:Set L1a delay                      " << std::endl;
    //std::cout << "123: Set CCB enable           129:Set power-up config. reg.                                         " << std::endl;
    //std::cout << "130: Read hot channel mask    131:Set hot channel mask       139:Set power-up hot channel mask      " << std::endl;
    //std::cout << "180: Read AFEB thresholds     181:Set AFEB threshold         189:Set power-up AFEB thresholds       " << std::endl;
    //std::cout << "190: Read collision pattern   191:Set collision pattern      199:Set power-up collision pattern mask" << std::endl;
    std::cout << std::endl;
    std::cout << "200:Configure TMB/DMB         201:Configure ALCT                                                    " << std::endl;
    //std::cout << "300:Write xsvf files          301:Read xsvf file                                                    " << std::endl;
    //std::cout << "400:Program user proms        401:Check user prom programs                                          " << std::endl;
    std::cout << "403:Clear user proms                                                                                " << std::endl;
    std::cout << std::endl;
    std::cout << "500:Check TMB state machines  501:Read TMB config registers  502:Read ALCT config regs              " << std::endl;
    std::cout << "503:StartTriggers TMB         504:Check TMB config registers 505:Check ALCT config regs             " << std::endl;
    //std::cout << "506:Print TMB state machines  507: Check RAT config registers                                       " << std::endl;
    std::cout << "508:Check TMB+ALCT conf regs  509:Time to program PROMs                                             " << std::endl;
    std::cout << std::endl;
    std::cout << "600:Program TMB firmware      601:Program ALCT firmware      602:Program RAT firmware               " << std::endl;
    //std::cout << std::endl;
    //std::cout << "700:Setup pulse test strips   701:Generate ADB Async         702:Generate ADB Sync                  " << std::endl;
    //std::cout << "703:Setup pulse AFEBs         704:Set up CFEB pulsing        705:Pulse CFEB                         " << std::endl;
    //std::cout << std::endl;
    //std::cout << "706:pulse strips:CCB L1Adelay 707:pulse strips: CLCT Distn  708:pulse strips: match timing          " << std::endl;
    std::cout << "800:Load TMB with MPC data    801:configure MPC                                                     " << std::endl;
    std::cout << std::endl;
    std::cout << "1000:Exit                                                                                           " << std::endl;
    std::cout << " menu choice? (Default = 999)" << std::endl;
    std::cin >> Menu;
    //
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
    case 38:
      std::cout << "RPC-RAT delay read from JTAG..." << std::endl;
      myRat->ReadRatUser1();
      myRat->PrintRpcRatDelay();
      //
      read_data = thisTMB->ReadRegister(rat_3d_sm_ctrl_adr);
      write_data = (read_data & 0xfffe) | 0x0;       // stop state machine
      std::cout << "Stop state machine:  Write 0x" << std::hex << write_data 
		<< " to TMB address = " << rat_3d_sm_ctrl_adr << std::endl;
      thisTMB->WriteRegister(rat_3d_sm_ctrl_adr,write_data);
      //
      write_data = 5 & 0xF;
      thisTMB->WriteRegister(rat_3d_delays_adr,write_data);
      std::cout << "Write RPC-RAT delay of 5 to TMB:  Write 0x" << std::hex << write_data 
		<< " to TMB address = " << rat_3d_delays_adr << std::endl;
      //
      read_data = thisTMB->ReadRegister(rat_3d_sm_ctrl_adr);
      write_data = (read_data & 0xfffe) | 0x1;       // start state machine
      std::cout << "Start state machine:  Write 0x" << std::hex << write_data 
		<< " to TMB address = " << rat_3d_sm_ctrl_adr << std::endl;
      thisTMB->WriteRegister(rat_3d_sm_ctrl_adr,write_data);
      //
      std::cout << "... wait 100usec ... " << std::endl;
      ::usleep(100);
      //
      read_data = thisTMB->ReadRegister(rat_3d_sm_ctrl_adr);
      busy = (read_data >> 6) & 0x1;
      std::cout << "Check state machine after starting:  read 0x" << std::hex << read_data 
		<< " from TMB address = " << rat_3d_sm_ctrl_adr << std::endl;
      std::cout << "... => busy = " << busy << std::endl;   
      //
      read_data = thisTMB->ReadRegister(rat_3d_sm_ctrl_adr);
      write_data = (read_data & 0xfffe) | 0x0;       // stop state machine
      std::cout << "Stop state machine:  Write 0x" << std::hex << write_data 
		<< " to TMB address = " << rat_3d_sm_ctrl_adr << std::endl;
      thisTMB->WriteRegister(rat_3d_sm_ctrl_adr,write_data);
      //
      read_data = thisTMB->ReadRegister(rat_3d_sm_ctrl_adr);
      busy = (read_data >> 6) & 0x1;
      verify = (read_data >> 7) & 0x1;
      std::cout << "Check state machine after starting:  read 0x" << std::hex << read_data 
		<< " from TMB address = " << rat_3d_sm_ctrl_adr << std::endl;
      std::cout << "... => busy = " << busy 
		<< ", verify ok = " << verify << std::endl;   
      //
      std::cout << "RPC-RAT delay read from JTAG..." << std::endl;
      myRat->ReadRatUser1();
      myRat->PrintRpcRatDelay();
      //
      break;
      //
    case 39:
      myRat->configure();
      break;
      //
      //
    case 87:
      std::cout << "CFEB timing scan" << std::endl;
      util.CFEBTiming();
      break;
      //
    case 88:
      std::cout << "ALCT timing scan" << std::endl;
      util.ALCTTiming();
      break;
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
    case 97:
      thisTMB->DumpAllRegisters();
      break;
    case 98:
      layer = ChipLocationTmbUserPromTMB;
      std::cout << "number of addresses to clock out " << std::endl;
      std::cin >> value;
      thisTMB->ClockOutPromProgram(layer,value);
      for (int address = 0; address<value; address++) {
	std::cout << "User prom " << layer
		  << ", address " << address
		  << "-> data = " << thisTMB->GetClockedOutPromImage(address) 
		  << std::endl;
      }
      break;
    case 99:
      //      std::cout << "layer (0-5)" << std::endl;
      //      std::cin >> layer;
      std::cout << "channel (0-39) " << std::endl;
      std::cin >> channel;
      std::cout << "OFF (0) or ON (1) " << std::endl;
      std::cin >> value;
      for (int i=0; i<6; i++)
	thisTMB->SetDistripHotChannelMask(i,channel,value);
      thisTMB->WriteDistripHotChannelMasks();
      thisTMB->ReadDistripHotChannelMasks();
      thisTMB->PrintHotChannelMask();
      //
      break;
      //
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
    case 190:
      alct->ReadCollisionPatternMask();
      alct->PrintCollisionPatternMask();
      break;
    case 191:
      std::cout << "Group: (0-13)" << std::endl;
      std::cin >> layer;
      std::cout << "Bit: (0-27)" << std::endl;
      std::cin >> channel;
      std::cout << "on/off (1/0) " << std::endl;
      std::cin >> value;
      alct->SetCollisionPatternMask(layer,channel,value);
      alct->WriteCollisionPatternMask();      
      alct->ReadCollisionPatternMask();
      alct->PrintCollisionPatternMask();
      break;
    case 199:
      alct->SetPowerUpCollisionPatternMask();
      alct->WriteCollisionPatternMask();      
      alct->ReadCollisionPatternMask();
      alct->PrintCollisionPatternMask();
      break;
    case 200:
      thisTMB->configure();
      thisDMB->configure();
      break;
    case 201:
      alct->configure();
      break;
    case 300:
      thisTMB->SetXsvfFilename("prom0_passtest");
      thisTMB->SetWhichUserProm(ChipLocationTmbUserPromTMB);
      thisTMB->CreateXsvfFile();
      //
      thisTMB->SetXsvfFilename("prom1_passtest");
      thisTMB->SetWhichUserProm(ChipLocationTmbUserPromALCT);
      thisTMB->CreateXsvfFile();
      //
      thisTMB->ClearXsvfFilename();
      //
      break;
    case 301:      
      thisTMB->SetXsvfFilename("prom0_passtest");
      thisTMB->ReadXsvfFile(true);
      thisTMB->SetXsvfFilename("prom1_passtest");
      thisTMB->ReadXsvfFile(true);
      //
      thisTMB->ClearXsvfFilename();
      //
      break;
    case 400:
      thisTMB->SetXsvfFilename("prom0_passtest");
      thisTMB->ProgramUserProm();
      //
      thisTMB->SetXsvfFilename("prom1_passtest");
      thisTMB->ProgramUserProm();
      //
      thisTMB->ClearXsvfFilename();
      //
      break;
    case 401:
      thisTMB->SetXsvfFilename("prom0_passtest");
      thisTMB->SetWhichUserProm(ChipLocationTmbUserPromTMB);
      thisTMB->CheckUserProm();
      //
      thisTMB->SetXsvfFilename("prom1_passtest");
      thisTMB->SetWhichUserProm(ChipLocationTmbUserPromALCT);
      thisTMB->CheckUserProm();
      //
      thisTMB->ClearXsvfFilename();
      //
      break;
    case 403:
      thisTMB->CheckAndProgramProm(ChipLocationTmbUserPromTMBClear);
      thisTMB->CheckAndProgramProm(ChipLocationTmbUserPromALCTClear);
      //
      break;
    case 500:
      outputfile.open("TMBCheckStatus.txt");
      thisTMB->RedirectOutput(&outputfile);
      //
      thisTMB->CheckTMBConfiguration();
      thisTMB->CheckVMEStateMachine();
      alct->CheckALCTConfiguration();
      thisTMB->CheckJTAGStateMachine();
      thisTMB->CheckDDDStateMachine();
      thisTMB->CheckRawHitsHeader();
      //
      std::cout << "TMB/ALCT configuration/state machines status: ";
      if (thisTMB->GetTMBConfigurationStatus() && 
	  thisTMB->GetVMEStateMachineStatus() &&
	  alct->GetALCTConfigurationStatus() && 
	  thisTMB->GetJTAGStateMachineStatus() &&
	  thisTMB->GetDDDStateMachineStatus() &&
	  thisTMB->GetRawHitsHeaderStatus() ) {
	std::cout << "OK";
      } else {
	std::cout << "-> FAIL <-";
      }
      std::cout << std::endl;
      thisTMB->RedirectOutput(&std::cout);
      outputfile.close();
      //
      break;
    case 501:
      outputfile.open("TMBconfiguration.txt");
      thisTMB->RedirectOutput(&outputfile);
      thisTMB->ReadTMBConfiguration();
      thisTMB->PrintTMBConfiguration();
      thisTMB->RedirectOutput(&std::cout);
      outputfile.close();
      //
      break;
    case 502:
      outputfile.open("ALCTconfiguration.txt");
      alct->RedirectOutput(&outputfile);
      alct->ReadALCTConfiguration();
      alct->PrintALCTConfiguration();
      alct->RedirectOutput(&std::cout);
      outputfile.close();
      //
      break;
    case 503:
      thisTMB->StartTTC();
      //
      break;
    case 504:
      thisTMB->CheckTMBConfiguration(); 
      std::cout << "Check TMB configuration vs xml file = "
		<< thisTMB->GetTMBConfigurationStatus()
		<< std::endl;
      //
      break;
    case 505:
      alct->CheckALCTConfiguration(); 
      std::cout << "Check ALCT configuration vs xml file = "
		<< alct->GetALCTConfigurationStatus()
		<< std::endl;
      //
      break;
    case 506:
      outputfile.open("TMBStateMachines.txt");
      thisTMB->RedirectOutput(&outputfile);
      thisTMB->ReadVMEStateMachine();
      thisTMB->PrintVMEStateMachine();
      //
      thisTMB->ReadJTAGStateMachine();
      thisTMB->PrintJTAGStateMachine();
      //
      thisTMB->ReadDDDStateMachine();
      thisTMB->PrintDDDStateMachine();
      //
      thisTMB->ReadRawHitsHeader();
      thisTMB->PrintRawHitsHeader();
      thisTMB->RedirectOutput(&std::cout);
      outputfile.close();
      //
      break;
    case 507:
      myRat->CheckRATConfiguration(); 
      std::cout << "Check RAT configuration vs xml file = "
		<< myRat->GetRATConfigurationStatus()
		<< std::endl;
      //
      break;
    case 508:
      starttime = time (NULL);
      //
      for (int chamber=0; chamber<number_of_chambers_to_check; chamber++) {
	std::cout << "chamber = " << chamber << std::endl;
	thisTMB->CheckTMBConfiguration(); 
	tmb_config_check[chamber] = thisTMB->GetTMBConfigurationStatus();
	std::cout << "Check TMB configuration vs xml file  = " << tmb_config_check[chamber]  << std::endl;
	//
	alct->CheckALCTConfiguration(); 
	alct_config_check[chamber] = alct->GetALCTConfigurationStatus();
	std::cout << "Check ALCT configuration vs xml file = " << alct_config_check[chamber] << std::endl;
      }
      //
      for (int chamber=0; chamber<number_of_chambers_to_check; chamber++) { 
	if (!tmb_config_check[chamber]) 
	  std::cout << "TMB configuration check NOT pass chamber = " << chamber << std::endl;
	if (!alct_config_check[chamber]) 
	  std::cout << "ALCT configuration check NOT pass chamber = " << chamber << std::endl;
      }
      endtime = time (NULL);
      //
      time_elapsed = endtime - starttime;
      //
      std::cout << "Checked " << std::dec <<  number_of_chambers_to_check
		<< " chambers in " << std::dec << time_elapsed << " seconds " << std::endl;
      //
      break;
    case 509:
      //
      starttime = time (NULL);
      //
      total_time_to_subtract = 0;
      //
      alternate = 1;
      //
      original_comp_thresh_setting = thisDMB->GetCompThresh();  // somehow this value gets lost after it is set in the code => get it at the start...
      //
      for (int chamber=0; chamber<number_of_chambers_to_check; chamber++) {
	std::cout << "chamber = " << chamber << std::endl;
	//
	// change the settings on each pass to force a reload of the PROMs and flash memories
	if (alternate == 1) {    
	  tmp_setting = thisTMB->GetL1aDelay() + 1;
	  thisTMB->SetL1aDelay(tmp_setting);
	  //
	  tmp_setting = alct->GetL1aDelay() + 1;
	  alct->SetL1aDelay(tmp_setting);
	  //
	  float_tmp_setting = original_comp_thresh_setting + 0.01;
	  thisDMB->SetCompThresh(float_tmp_setting);
	  //
	  alternate = 0;
	  //
	} else {
	  tmp_setting = thisTMB->GetL1aDelay() - 1;
	  thisTMB->SetL1aDelay(tmp_setting);
	  //
	  tmp_setting = alct->GetL1aDelay() - 1;
	  alct->SetL1aDelay(tmp_setting);
	  //
	  float_tmp_setting = original_comp_thresh_setting - 0.01;
	  thisDMB->SetCompThresh(float_tmp_setting);
	  //
	  alternate = 1;
	  //
	}
	tmbstarttime = time(NULL);
	thisTMB->configure();
	tmbstoptime = time(NULL);
	//
	dmbstarttime = time(NULL);
	thisDMB->configure();
	dmbstoptime = time(NULL);
	//
	alctstarttime = time(NULL);
	alct->configure();
	alctstoptime = time(NULL);
	//
	starttimesubtract = time (NULL);
	//
	thisCCB->HardReset_crate();
	//
	thisTMB->CheckTMBConfiguration(); 
	tmb_config_check[chamber] = thisTMB->GetTMBConfigurationStatus();
	std::cout << "Check TMB configuration vs xml file  = " << tmb_config_check[chamber]  << std::endl;
	//
	alct->CheckALCTConfiguration(); 
	alct_config_check[chamber] = alct->GetALCTConfigurationStatus();
	std::cout << "Check ALCT configuration vs xml file = " << alct_config_check[chamber] << std::endl;
	//
	endtimesubtract = time (NULL);
	//
	total_time_to_subtract += (endtimesubtract - starttimesubtract);
	total_tmb_time   += (tmbstoptime - tmbstarttime);
	total_alct_time  += (alctstoptime - alctstarttime);
	total_dmb_time   += (dmbstoptime - dmbstarttime);
      }
      //
      endtime = time (NULL);
      //
      for (int chamber=0; chamber<number_of_chambers_to_check; chamber++) { 
	if (!tmb_config_check[chamber]) 
	  std::cout << "TMB configuration check NOT pass chamber = " << chamber << std::endl;
	if (!alct_config_check[chamber]) 
	  std::cout << "ALCT configuration check NOT pass chamber = " << chamber << std::endl;
      }
      //
      time_elapsed = endtime - starttime;
      net_time_elapsed = time_elapsed - total_time_to_subtract;
      //
      std::cout << "Checked " << std::dec <<  number_of_chambers_to_check << " chambers" << std::endl; 
      std::cout << "-> Total time = " << std::dec << time_elapsed << " seconds, of which " 
		<< total_time_to_subtract << " seconds were hard reset and checking" << std::endl;
      std::cout << "-> Net  time = " << net_time_elapsed << " seconds" << std::endl;
      std::cout << "-> TMB  time = " << total_tmb_time   << " seconds" << std::endl;
      std::cout << "-> ALCT time = " << total_alct_time  << " seconds" << std::endl;
      std::cout << "-> DMB  time = " << total_dmb_time   << " seconds" << std::endl;
      //
      break;
    case 600:
      std::cout << "Load TMB firmware = " << tmb_firmware_string << std::endl;
      thisTMB->SetXsvfFilename(tmb_firmware_string);
      thisTMB->ProgramTMBProms();
      thisTMB->ClearXsvfFilename();
      //
      break;
    case 601:
      std::cout << "Load ALCT firmware = " << alct_firmware_string << std::endl;
      jch = 3;
      debug_mode = 0;
      alct->SVFLoad(&jch,alct_firmware_string,debug_mode);
      //
      break;
    case 602:
      std::cout << "Load RAT firmware = " << rat_firmware_string << std::endl;
      jch = 7;
      debug_mode = 0;
      myRat->SVFLoad(&jch,rat_firmware_string,debug_mode);
      //
      break;
    case 700:
      std::cout << "Pulse ALCT teststrips with signal = ";
      if (pulse_signal == ADB_SYNC) {
	std::cout << "ADB_SYNC" << std::endl;
      } else if (pulse_signal == ADB_ASYNC) {
	std::cout << "ADB_ASYNC" << std::endl;
      }
      std::cout << "amplitude " << std::endl;
      std::cin >> amplitude;
      std::cout << "layer mask 0" << std::endl;
      std::cin >> strip_mask[0];
      std::cout << "layer mask 1" << std::endl;
      std::cin >> strip_mask[1];
      std::cout << "layer mask 2" << std::endl;
      std::cin >> strip_mask[2];
      std::cout << "layer mask 3" << std::endl;
      std::cin >> strip_mask[3];
      std::cout << "layer mask 4" << std::endl;
      std::cin >> strip_mask[4];
      std::cout << "layer mask 5" << std::endl;
      std::cin >> strip_mask[5];
      strip_mask_int = 
	( (strip_mask[5] & 0x1) << 5 ) |
	( (strip_mask[4] & 0x1) << 4 ) |
	( (strip_mask[3] & 0x1) << 3 ) |
	( (strip_mask[2] & 0x1) << 2 ) |
	( (strip_mask[1] & 0x1) << 1 ) |
	( (strip_mask[0] & 0x1) << 0 );
      alct->SetUpPulsing((int)amplitude,PULSE_LAYERS,strip_mask_int,pulse_signal);
      //
      break;
    case 701:
      thisCCB->setCCBMode(CCB::VMEFPGA);
      thisCCB->GenerateAlctAdbASync();	
      thisCCB->setCCBMode(CCB::DLOG); 
      //
      break;
    case 702:
      thisCCB->setCCBMode(CCB::VMEFPGA);
      thisCCB->GenerateAlctAdbSync();	
      thisCCB->setCCBMode(CCB::DLOG); 
      //
      break;
    case 703:
      std::cout << "Pulse AFEBs with signal = ";
      if (pulse_signal == ADB_SYNC) {
	std::cout << "ADB_SYNC" << std::endl;
      } else if (pulse_signal == ADB_ASYNC) {
	std::cout << "ADB_ASYNC" << std::endl;
      }
      std::cout << "amplitude " << std::endl;
      std::cin >> amplitude;
      std::cout << "AFEB mask 0" << std::endl;
      std::cin >> strip_mask[0];
      std::cout << "AFEB mask 1" << std::endl;
      std::cin >> strip_mask[1];
      std::cout << "AFEB mask 2" << std::endl;
      std::cin >> strip_mask[2];
      std::cout << "AFEB mask 3" << std::endl;
      std::cin >> strip_mask[3];
      std::cout << "AFEB mask 4" << std::endl;
      std::cin >> strip_mask[4];
      std::cout << "AFEB mask 5" << std::endl;
      std::cin >> strip_mask[5];
      std::cout << "AFEB mask 6" << std::endl;
      std::cin >> strip_mask[6];
      std::cout << "AFEB mask 7" << std::endl;
      std::cin >> strip_mask[7];
      strip_mask_int = 
	( (strip_mask[7] & 0x1) << 7 ) |
	( (strip_mask[6] & 0x1) << 6 ) |
	( (strip_mask[5] & 0x1) << 5 ) |
	( (strip_mask[4] & 0x1) << 4 ) |
	( (strip_mask[3] & 0x1) << 3 ) |
	( (strip_mask[2] & 0x1) << 2 ) |
	( (strip_mask[1] & 0x1) << 1 ) |
	( (strip_mask[0] & 0x1) << 0 );
      alct->SetUpPulsing((int)amplitude,PULSE_AFEBS,strip_mask_int,pulse_signal);
      //
      break;
    case 704:
      std::cout << "amplitude " << std::endl;
      std::cin >> amplitude;
      std::cout << "CFEB 0" << std::endl;
      std::cin >> strip_mask[0];
      std::cout << "CFEB 1" << std::endl;
      std::cin >> strip_mask[1];
      std::cout << "CFEB 2" << std::endl;
      std::cin >> strip_mask[2];
      std::cout << "CFEB 3" << std::endl;
      std::cin >> strip_mask[3];
      std::cout << "CFEB 4" << std::endl;
      std::cin >> strip_mask[4];
      strip_mask_int = 
	( (strip_mask[4] & 0x1) << 4 ) |
	( (strip_mask[3] & 0x1) << 3 ) |
	( (strip_mask[2] & 0x1) << 2 ) |
	( (strip_mask[1] & 0x1) << 1 ) |
	( (strip_mask[0] & 0x1) << 0 );
      HalfStrip = 16;
      //
      thisDMB->set_comp_thresh(0.1);
      thisDMB->set_dac(amplitude,0);
      //
      for (int layer = 0; layer < 6; layer++ ) 
	hp[layer] = HalfStrip;       
      //
      thisDMB->trigsetx(hp,strip_mask_int);
      //
      break;
    case 705:
      thisDMB->inject(1,0x4f);
      //
      break;
    case 706:
      //
      // Determine timing of L1A coming from CCB
      //
      util.SetupCoincidencePulsing();
      //
      thisTMB->StartTTC(); //enable the TMB to trigger by faking TTC Resync/BC0 commands.  Need to use LTC for this
      //
      thisCCB->setCCBMode(CCB::VMEFPGA);  // need to use LTC for this
      thisCCB->SetL1aDelay(ccb_l1a_delay); 
      //
      // clear histograms
      for (int i=0; i<255; i++) 
	number_of_l1a_at_tmb_histo[i] = 0;
      //
      for (int delay=140; delay<165; delay++) {
	//
	std::cout << "Set tmb_l1a_delay = " << std::dec << delay << std::endl;
	thisTMB->SetL1aDelay(delay);
	thisTMB->WriteRegister(seq_l1a_adr);
	//
	thisTMB->ResetCounters();
	//
	for (npulse=0; npulse<100; npulse++) {
	  thisCCB->GenerateAlctAdbSync();  
	  ::usleep(100);
	}
	//
	thisTMB->GetCounters();
	if (debug) {
	  thisTMB->PrintCounters(1);
	  thisTMB->PrintCounters(4);
	  thisTMB->PrintCounters(6);
	  thisTMB->PrintCounters(7);
	  thisTMB->PrintCounters(8);
	  thisTMB->PrintCounters(18);
	}
	thisTMB->PrintCounters(19);
	number_of_l1a_at_tmb_histo[delay] = thisTMB->GetCounter(19);
      }
      //
      thisCCB->setCCBMode(CCB::DLOG);
      //
      average = util.AverageHistogram(number_of_l1a_at_tmb_histo,145,165);
      util.PrintHistogram("L1A at TMB",number_of_l1a_at_tmb_histo,145,165,average);
      //
      break;
    case 707:
      //
      // Pulse ALCT teststrips to determine distribution of induced pulses on comparators (CLCT)
      //
      thisTMB->StartTTC(); //enable the TMB to trigger by faking TTC Resync/BC0 commands.  Need to use LTC for this
      //
      thisCCB->setCCBMode(CCB::VMEFPGA);  // need to use LTC for this
      thisCCB->SetL1aDelay(ccb_l1a_delay);
      //
      int min_amplitude;
      int max_amplitude;
      std::cout << "Minimum for ALCT Amplitude scan [0-255] = " << std::endl;
      std::cin  >> min_amplitude;
      std::cout << "Maximum for ALCT Amplitude scan [0-255] = " << std::endl;
      std::cin  >> max_amplitude;
      //
      for (int amplitude_scan = min_amplitude; amplitude_scan<=max_amplitude; amplitude_scan+=2) {  
	//
	//set up the output file
	char buf[20];
	sprintf(buf,"ampl%d.dat",(int)amplitude_scan);
	outputfile.open(buf);
	//
	util.SetupCoincidencePulsing((int)amplitude_scan,pulse_signal);
	//
	// clear histograms, reset counters
	thisTMB->ResetCounters();
	//
	int pretrigger_counter     = 0;
	int old_pretrigger_counter = 0;
	//
	for (int i=0; i<number_of_hs; i++) {
	  clct0_keyhs_histo[i] = 0;
	  clct1_keyhs_histo[i] = 0;
	}
	//
	for (npulse=0; npulse<total_number_of_pulses_per_distrip; npulse++) {
	  //
	  // mask out all channels except a single distrip (for each layer)
	  // Note always mask off channel 0 and channel 39 since these are at the
	  // edges of the chambers, and ring hot when the teststrip is enabled
	  //
	  for (channel=1; channel<39; channel++) { 
	    //
	    for (int layer=0; layer<6; layer++) {
	      for (int zerochannel=0; zerochannel<40; zerochannel++) {
		if (zerochannel == channel) {
		  thisTMB->SetDistripHotChannelMask(layer,zerochannel,1);
		} else {
		  thisTMB->SetDistripHotChannelMask(layer,zerochannel,0);
		}
	      }
	    }
	    thisTMB->WriteDistripHotChannelMasks();
	    //
	    if (debug) {
	      outputfile << "Enable distrip = " << channel << std::endl;
	      thisTMB->ReadDistripHotChannelMasks();
	      thisTMB->RedirectOutput(&outputfile);
	      thisTMB->PrintHotChannelMask();
	    }
	    //
	    thisCCB->GenerateAlctAdbSync();  // need to send via LTC cyclicly, somehow...
	    ::usleep(100);
	    //
	    thisTMB->GetCounters();
	    pretrigger_counter = thisTMB->GetCounter(4);
	    //
	    thisTMB->DecodeCLCT();
	    int clct0_key_halfstrip = thisTMB->GetCLCT0keyHalfStrip();
	    int clct0_key_cfeb      = thisTMB->GetCLCT0Cfeb();
	    int distrip_associated_w_halfstrip = (int)( (clct0_key_cfeb*32 + clct0_key_halfstrip)/4 );
	    if (debug) thisTMB->PrintCLCT();
	    //
	    if ( distrip_associated_w_halfstrip == channel && 
		 old_pretrigger_counter < pretrigger_counter ) {     // make sure this is a new pretrigger (don't double count one event)
	      if (debug) outputfile << "!!!!YES!!!! -> match" << std::endl;
	      // clct0_keyhs_histo[clct0_key_halfstrip]++;   //plot vs. 1/2-strip
	      clct0_keyhs_histo[channel]++;   //plot vs. di-strip
	      old_pretrigger_counter = pretrigger_counter;
	    } else {
	      if (debug) outputfile << "not a match..." << std::endl;
	    }
	    //
	    if (debug) thisTMB->RedirectOutput(&std::cout);
	    //
	  }
	}
	//
	outputfile << "Pulse teststrip " << npulse << " times on ADB_SYNC with amplitude = " << amplitude_scan << std::endl;
	outputfile << "di-strip   CLCT0   " << std::endl;
	outputfile << "--------   -----   " << std::endl;
	for (int i=0; i<=39; i++) 
	  outputfile << std::dec 
		     << std::setw(8) << i 
		     << std::setw(8)  << clct0_keyhs_histo[i] 
		     << std::endl;
	//
	thisTMB->RedirectOutput(&outputfile);
	//
	thisTMB->GetCounters();
	thisTMB->PrintCounters(1);
	thisTMB->PrintCounters(4);
	thisTMB->PrintCounters(6);
	thisTMB->PrintCounters(7);
	thisTMB->PrintCounters(8);
	thisTMB->PrintCounters(18);
	thisTMB->PrintCounters(19);
	//
	thisTMB->RedirectOutput(&std::cout);
	outputfile.close();
	//
      }
      //
      thisCCB->setCCBMode(CCB::DLOG);
      //
      break;
    case 708:
      //
      // Pulse ALCT teststrips to determine ALCT-CLCT match timing
      //
      // Need to study what is the difference with:
      //  - inverted pulse
      //  - amplitude
      //  - masking different CFEBs
      //  - ALCT masks
      //  - what do ALCT data look like?
      //
      //      util.SetupCoincidencePulsing((int)amplitude,pulse_signal);
      util.SetupCoincidencePulsing();
      //
      thisTMB->StartTTC(); //enable the TMB to trigger by faking TTC Resync/BC0 commands, should be able to use LTC
      //
      thisCCB->setCCBMode(CCB::VMEFPGA);   // need to remove this and use the LTC, instead
      thisCCB->SetL1aDelay(ccb_l1a_delay);  
      //
      //set up the output file
      //      char buf[20];
      //      sprintf(buf,"ampl%d.dat",(int)amplitude);
      //      outputfile.open(buf);
      //      alct->RedirectOutput(&outputfile);
      //
      // clear histograms
      thisTMB->ResetCounters();
      for (int bin=minbin; bin<=maxbin; bin++)
	AlctInClctMatchWindowHisto[bin] = 0;
      //
      for (npulse=0; npulse<total_number_of_pulses; npulse++) {
	//
	if (npulse % 100 == 0) std::cout << "Pulse " << std::dec<< npulse << " times..." << std::endl;
	//
	// after following pretrigger, halt until next unhalt signal is set: 
	thisTMB->SetPretriggerHalt(1);  
	thisTMB->WriteRegister(seq_clct_adr);
	::usleep(100);
	//
	if (pulse_signal == ADB_SYNC) {
	  thisCCB->GenerateAlctAdbSync();  // need to send via LTC cyclicly, somehow...
	} else if (pulse_signal == ADB_ASYNC) {
	  thisCCB->GenerateAlctAdbASync();  // need to send via LTC cyclicly, somehow...
	}
	::usleep(100);
	//
	thisTMB->TMBRawhits();
	//thisTMB->PrintTMBRawHits();
	int value = thisTMB->GetAlctInClctMatchWindow();
	AlctInClctMatchWindowHisto[value]++;
	//
	// set unhalt signal to enable normal triggers
	thisTMB->SetPretriggerHalt(0);  
	thisTMB->WriteRegister(seq_clct_adr);
	::usleep(100);
	//
      }
      //
      std::cout << "Amplitude = " << std::dec << amplitude << ", pulse teststrip " << npulse << " times on signal ";
      if (pulse_signal == ADB_SYNC) {
	std::cout << "ADB_SYNC" << std::endl;
      } else if (pulse_signal == ADB_ASYNC) {
	std::cout << "ADB_ASYNC" << std::endl;
      }
      average = util.AverageHistogram(AlctInClctMatchWindowHisto,minbin,maxbin);
      util.PrintHistogram("ALCT in CLCT match window",AlctInClctMatchWindowHisto,minbin,maxbin,average);
      //
      //      thisTMB->RedirectOutput(&outputfile);
      //
      thisTMB->GetCounters();
      thisTMB->PrintCounters(1);
      thisTMB->PrintCounters(4);
      thisTMB->PrintCounters(6);
      thisTMB->PrintCounters(7);
      thisTMB->PrintCounters(8);
      thisTMB->PrintCounters(18);
      thisTMB->PrintCounters(19);
      //
      //      thisTMB->RedirectOutput(&std::cout);
      //      outputfile.close();
      //
      // uncomment following to measure TMB L1A delay value for CCB L1A
      //      }
      thisCCB->setCCBMode(CCB::DLOG);
      //
      break;
    case 800:
      //
      for (int i=0; i<lctCounter; i++) {
	//
	thisTMB->ResetInjectedLCT();
	thisTMB->InjectMPCData(2,lct0[i],lct1[i]);
	std::cout << "Software inject values = " << std::endl;
	for (unsigned int i=0; i<thisTMB->GetInjectedLct0().size(); i++) {
	std::cout << "LCT0 = " << std::hex << thisTMB->GetInjectedLct0()[i] << std::endl;
	std::cout << "LCT1 = " << std::hex << thisTMB->GetInjectedLct1()[i] << std::endl;
	}
	thisTMB->ReadBackMpcRAM(2);
	thisTMB->FireMPCInjector(2);
	//
	thisTMB->DataSendMPC();
	std::cout << "MPC0/1 accepted = " << thisTMB->MPC0Accept() << "/" << thisTMB->MPC1Accept() << std::endl;
	thisTMB->GetCounters();
	thisTMB->PrintCounters(16);
	thisTMB->PrintCounters(17);
	//
	::sleep(1);
      }
      //
      break;
    case 801:
      thisMPC->configure();
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
