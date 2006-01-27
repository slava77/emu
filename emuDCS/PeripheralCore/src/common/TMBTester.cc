#include<iostream>
#include<stdio.h>
#include<string>
#include<unistd.h> // for sleep()
#include "TMBTester.h"
#include "TMB.h"
#include "CCB.h"
#include "TMB_constants.h"
#include "JTAG_constants.h"
#include "TMB_JTAG_constants.h"

#ifndef debugV //silent mode
#define PRINT(x)
#define PRINTSTRING(x)
#else //verbose mode
#define PRINT(x) std::cout << #x << ":\t" << x << std::endl;
#define PRINTSTRING(x) std::cout << #x << std::endl;
#endif

TMBTester::TMBTester() {
  tmb_ = 0;
  ccb_ = 0;
  MyOutput_ = & std::cout;
  jtag_address = -1;
  jtag_chain = -1;
  step_mode = false;
}

TMBTester::~TMBTester(){}
//

/////////////////////////////////////////
// General members:
/////////////////////////////////////////
void TMBTester::readreg4(){
  (*MyOutput_) << "Basic test of TMB communication" << std::endl;
  int regtoread = 4;
  (*MyOutput_) << "Getting date from register " << regtoread << std::endl;
  int reg = tmb_->ReadRegister(regtoread);

}

void TMBTester::reset() {
  (*MyOutput_) << "TMBTester: Hard reset through CCB" << std::endl;
  if ( ccb_ ) {
    ccb_->hardReset();  
  } else {
    (*MyOutput_) << "No CCB defined" << std::endl;
  }
}

bool TMBTester::runAllTests() {
  (*MyOutput_) << "TMBTester: Beginning full set of TMB self-tests" << std::endl;

  bool AllOK;

  reset();
  //  readreg4();

  bool bootRegOK = testBootRegister();
  bool VMEfpgaDataRegOK = testVMEfpgaDataRegister();
  bool DateOK = testFirmwareDate();
  bool TypeOK = testFirmwareType();
  bool VersionOK = testFirmwareVersion();
  bool RevCodeOK = testFirmwareRevCode();
  bool MezzIdOK = testMezzId();
  bool PROMidOK = testPROMid();
  bool PROMpathOK = testPROMpath();
  bool dsnOK = testDSN();
  bool adcOK = testADC();
  bool is3d3444OK = test3d3444();

  (*MyOutput_) << "TMBTester Full Test Summary:" << std::endl;

  messageOK("Boot Register............. ",bootRegOK);
  messageOK("VME FPGA Data Register.... ",VMEfpgaDataRegOK);
  messageOK("TMB Firmware date......... ",DateOK);
  messageOK("TMB Firmware type......... ",TypeOK);
  messageOK("TMB Firmware Version...... ",VersionOK);
  messageOK("TMB Firmware Revision Code ",RevCodeOK);
  messageOK("Mezzanine ID (JTAG)....... ",MezzIdOK);
  messageOK("PROM ID (JTAG)............ ",PROMidOK);
  messageOK("PROM Path................. ",PROMpathOK);
  messageOK("Digital Serial Numbers.... ",dsnOK);
  messageOK("Voltages (ADC)............ ",adcOK);
  messageOK("3d3444 verification....... ",is3d3444OK);

  AllOK = (bootRegOK &&
	   VMEfpgaDataRegOK &&
	   DateOK &&
	   TypeOK &&
           VersionOK &&
           RevCodeOK &&
           MezzIdOK &&
           PROMidOK &&
           PROMpathOK &&
           dsnOK &&
           adcOK &&
           is3d3444OK);

  messageOK("TMB all tests............. ",AllOK);

  return AllOK;
}


/////////////////////////////////////////
// Tests:
/////////////////////////////////////////
bool TMBTester::testBootRegister() {
  (*MyOutput_) << "TMBTester: testing Boot Register" << std::endl;

  bool testOK = false;
  int dummy;  
  
  unsigned short int BootData;
  dummy = tmb_->tmb_get_boot_reg(&BootData);
  //  (*MyOutput_) << "Initial boot contents = " << std::hex << BootData << std::endl;

  unsigned short int write_data, read_data;
  int err_reg = 0;

  // walk through the 16 bits on the register
  for (int ibit=0; ibit<=15; ibit++) {
    write_data = 0x1 << ibit;
    dummy = tmb_->tmb_set_boot_reg(write_data);

    dummy = tmb_->tmb_get_boot_reg(&read_data);

    // Remove the read-only bits:
    read_data &= 0x3FFF;
    write_data &= 0x3FFF;
    
    if ( !compareValues("bootreg bit",read_data,write_data,true) ) {
      err_reg++;
    }
  }

  // Restore boot contents
  dummy = tmb_->tmb_set_boot_reg(BootData);
  dummy = tmb_->tmb_get_boot_reg(&read_data);   //check for FPGA final state
  //  (*MyOutput_) << "Final Boot Contents = " << std::hex << read_data << std::endl;    

  testOK = compareValues("Number of boot register errors",err_reg,0,true);  
  messageOK("Boot Register",testOK);
  dummy = sleep(3);
  return testOK; 
}


bool TMBTester::testHardReset(){
  (*MyOutput_) << "TMBTester: checking hard reset TMB via boot register" << std::endl;
  (*MyOutput_) << "NOTE:  TEST NOT NEEDED, as we hard reset by the CCB" << std::endl; 

  bool testOK = false;
  int dummy;

  unsigned short int BootData;
  dummy = tmb_->tmb_get_boot_reg(&BootData);
  //  (*MyOutput_) << "Initial boot contents = " << std::hex << BootData << std::endl;

  unsigned short int write_data, read_data;

  write_data = 0x0200;
  dummy = tmb_->tmb_set_boot_reg(write_data);   //assert hard reset

  dummy = tmb_->tmb_get_boot_reg(&read_data);   //check for FPGA not ready
  read_data &= 0x7FFF;                          // remove tdo

  bool FPGAnotReady = 
    compareValues("Hard reset TMB FPGA not ready",read_data,0x0200,true);

  
  write_data = 0x0000;
  dummy = tmb_->tmb_set_boot_reg(write_data);   //de-assert hard reset

  (*MyOutput_) << "waiting for TMB to reload..." << std::endl;
  dummy = sleep(5);                             

  dummy = tmb_->tmb_get_boot_reg(&read_data);   //check for FPGA not ready
  read_data &= 0x4000;                          // remove bits beside "FPGA ready"

  bool FPGAReady = compareValues("Hard reset TMB FPGA ready",read_data,0x4000,true);

  // Restore boot contents
  dummy = tmb_->tmb_set_boot_reg(BootData);
  dummy = tmb_->tmb_get_boot_reg(&read_data);   //check for FPGA final state
  //  (*MyOutput_) << "Final Boot Contents = " << std::hex << read_data << std::endl;    

  testOK = (FPGAnotReady &&
	    FPGAReady);
  return testOK;
}


bool TMBTester::testVMEfpgaDataRegister(){
  (*MyOutput_) << "TMBTester: testing VME FPGA Data Register" << std::endl;
  bool testOK = false;

  // Get current status:
  int vme_cfg = tmb_->ReadRegister(vme_ddd0_adr);

  int write_data,read_data;
  bool tempBool;

  int register_error = 0;

  for (int i=0; i<=15; i++) {
    write_data = 1 << i;

    tmb_->WriteRegister(vme_ddd0_adr,write_data);  //write walking 1
    read_data = tmb_->ReadRegister(0);               //read base to purge bit3 buffers
    read_data = tmb_->ReadRegister(vme_ddd0_adr);  //read walking 1
    
    tempBool = compareValues("Register value",read_data,write_data,true);
    
    if (!tempBool) register_error++;
  }

  //restore data register...
  tmb_->WriteRegister(vme_ddd0_adr,vme_cfg);

  write_data = 0x001A;       // turn on cylons
  tmb_->WriteRegister(ccb_cfg_adr,write_data);

  testOK = compareValues("Number of VME FPGA data reg errors",register_error,0,true);
  messageOK("VME FPGA data register",testOK);
  int dummy = sleep(3);
  return testOK;
}


bool TMBTester::testFirmwareSlot(){
  (*MyOutput_) << "TMBTester: testing TMB slot number" << std::endl;
  int firmwareData = tmb_->FirmwareVersion();
  int slot = (firmwareData>>8) & 0xf;

  (*MyOutput_) << "TMBTester: TMB Slot (counting from 0) = " 
	    << std::dec << slot << std::endl;

  //VME counts from 0, xml file counts from 1:
  int slotToCompare = ( (*TMBslot) - 1 ) & 0xf;  //only 4 bits assigned to this number...

  bool testOK = compareValues("TMB slot",slot,slotToCompare,true);
  messageOK("TMB slot",testOK);
  int dummy = sleep(3);
  return testOK;
}


bool TMBTester::testFirmwareDate() {
  (*MyOutput_) << "TMBTester: testing Firmware date" << std::endl;

  int firmwareData = tmb_->FirmwareDate();
  int day = firmwareData & 0xff;
  int month = (firmwareData>>8) & 0xff;
  int year=tmb_->FirmwareYear();

  bool testOK = compareValues("Firmware Year",year,0x2005,true);
  messageOK("Firmware Year",testOK);
  int dummy = sleep(3);
  return testOK; 
}


bool TMBTester::testFirmwareType() {
  (*MyOutput_) << "TMBTester: testing Firmware Type" << std::endl;
  bool TypeNormal=false;
  bool TypeDebug=false;

  int firmwareData = tmb_->FirmwareVersion();
  int type = firmwareData & 0xf;

  TypeNormal = compareValues("Firmware Normal",type,0xC,true);
  if (!TypeNormal){
    TypeDebug = compareValues("CAUTION Firmware Debug",type,0xD,true);    
  }
  if (!TypeNormal && !TypeDebug ){
    (*MyOutput_) << 
      "What kind of Firmware is this? Firmware = " << type << std::endl;
  }
  messageOK("Firmware Type Normal",TypeNormal);
  int dummy = sleep(3);
  return TypeNormal;
}


bool TMBTester::testFirmwareVersion() {
  (*MyOutput_) << "TMBTester: testing Firmware Version" << std::endl;
  int firmwareData = tmb_->FirmwareVersion();
  int version = (firmwareData>>4) & 0xf;

  bool testOK = compareValues("Firmware Version",version,0xE,true);
  messageOK("Firmware Version",testOK);
  int dummy = sleep(3);
  return testOK;
}


bool TMBTester::testFirmwareRevCode(){
  (*MyOutput_) << "TMBTester: testing Firmware Revision Code" << std::endl;

  int firmwareData = tmb_->FirmwareRevCode();

  //  int RevCodeDay = firmwareData & 0x001f;
  //  int RevCodeMonth = (firmwareData>>5) & 0x000f;
  //  int RevCodeYear = (firmwareData>>9) & 0x0007;
  int RevCodeFPGA = (firmwareData>>12) & 0x000F;

  bool testOK = compareValues("Firmware Revcode FPGA",RevCodeFPGA,0x04,true);
  messageOK("Firmware Revcode FPGA",testOK);
  int dummy = sleep(3);
  return testOK;
}


bool TMBTester::testJTAGchain(){
  (*MyOutput_) << "TMBTester: testing User and Boot JTAG chains" << std::endl;

  bool user = testJTAGchain(0);
  bool boot = testJTAGchain(1);

  bool JTAGchainOK = (user && boot);
  messageOK("JTAG chains",JTAGchainOK);
  int dummy = sleep(3);
  return JTAGchainOK;
}

bool TMBTester::testJTAGchain(int type){
  (*MyOutput_) << "testJTAGchain: DOES NOT WORK with firmware not in debug mode" << std::endl; 
  (*MyOutput_) << 
   "Therefore...  TMBTester::testJTAGchain() NOT YET IMPLEMENTED" 
     << std::endl;

  bool testOK = false;  
  return testOK;
}


bool TMBTester::testMezzId(){
  (*MyOutput_) << "TMBTester: Checking Mezzanine FPGA and PROMs ID codes" << std::endl;
  bool testOK = false;
  (*MyOutput_) << "TMBTester::testMezzId() NOT YET IMPLEMENTED" << std::endl; 

  /*
  set_jtag_address(TMB_ADR_BOOT);    
  set_jtag_chain(TMB_MEZZ_FPGA_CHAIN);         

  // **Take the chosen chain's TAP to RTI (via the chosen address)**
  jtag_anystate_to_rti();    
  
  int opcode;

  const int register_length = 32;  
  int value[register_length];

  for (int chip_id=0; chip_id<=4; chip_id++){
    if (chip_id == 0) {
      opcode = VTX2_IDCODE;                  // FPGA IDcode opcode, expect v0A30093
    } else { 
      opcode = PROM_IDCODE;                  // PROM IDcode opcode
    }
    do_jtag(chip_id,opcode,register_length,value);
  }

  // GREG, NOW YOU HAVE TO DECODE THE OUTPUT VALUE...
  // FIRST, MAKE THE JTAG GO...
  */
  messageOK("Mezzanine ID",testOK);
  int dummy = sleep(3);
  return testOK;
}


bool TMBTester::testPROMid(){
  (*MyOutput_) << "TMBTester: Checking User PROM ID codes" << std::endl;
  bool testOK = false;
  (*MyOutput_) << "TMBTester::testPROMid() NOT YET IMPLEMENTED" << std::endl; 

  /*
  set_jtag_address(TMB_ADR_BOOT);    
  set_jtag_chain(TMB_USER_PROM_CHAIN);         

  // **Take the chosen chain's TAP to RTI (via the chosen address)**
  jtag_anystate_to_rti();    
  
  int opcode = PROM_IDCODE;

  const int register_length = 32;  
  int value[register_length];

  for (int chip_id=0; chip_id<=1; chip_id++){
    do_jtag(chip_id,opcode,register_length,value);

    // GREG, NOW YOU HAVE TO DECODE THE OUTPUT VALUE...
    // FIRST, MAKE THE JTAG GO...
  }
  */
  messageOK("PROM ID",testOK);
  int dummy = sleep(3);
  return testOK;
}


bool TMBTester::testPROMpath(){
  (*MyOutput_) << "TMBTester: Checking User PROM Data Path" << std::endl;

  bool testOK = false;
  int write_data,read_data;

  int iprom,jprom,prom_adr;
  int prom_clk[2],prom_oe[2],prom_nce[2];
  int prom_src;

  int pat_expect;
  bool temptest=true;

  for (iprom=0; iprom<=1; iprom++) {
    jprom = (iprom+1) % 2;

  // **enable one prom and disable the other, since they share the onboard led bus**

    prom_clk[iprom]=0;    //enable this one
    prom_oe[iprom] =1;
    prom_nce[iprom]=0;

    prom_clk[jprom]=0;    //disable this one
    prom_oe[jprom] =0;
    prom_nce[jprom]=1;
    
    prom_src=1;

    write_data = 
      (prom_src   <<14) |        //0=on-board led, 1=enabled PROM
      (prom_nce[1]<<13) |        //PROM 1 /chip_enable
      (prom_oe[1] <<12) |        //PROM 1 output enable
      (prom_clk[1]<<11) |        //PROM 1 clock
      (prom_nce[0]<<10) |        //PROM 0 /chip_enable
      (prom_oe[0] << 9) |        //PROM 0 output enable
      (prom_clk[0]<< 8);         //PROM 0 clock
    
    tmb_->WriteRegister(vme_prom_adr,write_data);

    // **Read the data from the selected PROM **
    for (prom_adr=0; prom_adr<=9; prom_adr++) {
      read_data = tmb_->ReadRegister(vme_prom_adr) & 0xff;

      pat_expect = 0xff;
      if (prom_adr==0) {
	if (iprom == 0) pat_expect = 0xab;
	if (iprom == 1) pat_expect = 0xcd;
      } else if (prom_adr==9) {
	if (iprom == 0) pat_expect = 0xee;
	if (iprom == 1) pat_expect = 0xbb;
      } else {
	pat_expect = 1 << (prom_adr-1);
      }
      temptest &= compareValues("data = expected",read_data,pat_expect,true);

      // ** Toggle the clock to advance the address **
      prom_clk[iprom]=1;
      write_data = 
	(prom_src   <<14) |        //0=on-board led, 1=enabled PROM
	(prom_nce[1]<<13) |        //PROM 1 /chip_enable
	(prom_oe[1] <<12) |        //PROM 1 output enable
	(prom_clk[1]<<11) |        //PROM 1 clock
	(prom_nce[0]<<10) |        //PROM 0 /chip_enable
	(prom_oe[0] << 9) |        //PROM 0 output enable
	(prom_clk[0]<< 8);         //PROM 0 clock
      tmb_->WriteRegister(vme_prom_adr,write_data);

      prom_clk[iprom]=0;
      write_data = 
	(prom_src   <<14) |        //0=on-board led, 1=enabled PROM
	(prom_nce[1]<<13) |        //PROM 1 /chip_enable
	(prom_oe[1] <<12) |        //PROM 1 output enable
	(prom_clk[1]<<11) |        //PROM 1 clock
	(prom_nce[0]<<10) |        //PROM 0 /chip_enable
	(prom_oe[0] << 9) |        //PROM 0 output enable
	(prom_clk[0]<< 8);         //PROM 0 clock
      tmb_->WriteRegister(vme_prom_adr,write_data);
    
    }

    // ** Turn PROMs off **
    prom_clk[iprom]=0;    //disable this one
    prom_oe[iprom] =0;
    prom_nce[iprom]=1;
    
    prom_src=0;

    write_data = 
      (prom_src   <<14) |        //0=on-board led, 1=enabled PROM
      (prom_nce[1]<<13) |        //PROM 1 /chip_enable
      (prom_oe[1] <<12) |        //PROM 1 output enable
      (prom_clk[1]<<11) |        //PROM 1 clock
      (prom_nce[0]<<10) |        //PROM 0 /chip_enable
      (prom_oe[0] << 9) |        //PROM 0 output enable
      (prom_clk[0]<< 8);         //PROM 0 clock
    
    tmb_->WriteRegister(vme_prom_adr,write_data);
  }

  testOK = temptest;

  messageOK("PROM path",testOK);
  int dummy = sleep(3);
  return testOK;
}


bool TMBTester::testDSN(){
  int dummy;

  (*MyOutput_) << "TMBTester: Checking Digital Serial Numbers for TMB" 
	    << std::endl;
  bool tmbDSN = testDSN(0);
  dummy=sleep(1);

  (*MyOutput_) << "TMBTester: Checking Digital Serial Numbers for Mezzanine" 
	    << std::endl;
  bool mezzanineDSN = testDSN(1);
  dummy=sleep(1);

  (*MyOutput_) << "TMBTester: Checking Digital Serial Numbers for RAT" 
	    << std::endl;
  bool ratDSN = testDSN(2);
  dummy=sleep(1);

  messageOK("TMB DSN",tmbDSN);
  messageOK("Mezzanine DSN",mezzanineDSN);
  messageOK("RAT DSN",ratDSN);

  bool DSNOK = (tmbDSN &&
                mezzanineDSN &&
                ratDSN);

  messageOK("All Digital Serial Numbers",DSNOK);
  dummy = sleep(3);
  return DSNOK;
}

bool TMBTester::testDSN(int BoardType){

  std::bitset<64> dsn;

  // get the digital serial number
  dsn = tmb_->dsnRead(BoardType);

  // compute the CRC
  int crc = dowCRC(dsn);

  //get the 8 most significant bits (from their LSB) to compare with CRC
  int dsntocompare = 0;
  int value;
  for (int bit=63; bit>=56; bit--){
    value = dsntocompare << 1;
    dsntocompare = value | dsn[bit];
  }

  bool crcEqualDSN = compareValues("CRC equal dsn[56-63]",crc,dsntocompare,true);
  bool crcZero = compareValues("CRC value ",crc,0,false);

  bool DSNOK = (crcZero &&
		crcEqualDSN);
  return DSNOK;
}


bool TMBTester::testADC(){
  (*MyOutput_) << "TMBTester: Checking ADC and status" << std::endl;

  bool testOK = false;

  // Voltage status bits...
  int voltage_status = tmb_->PowerComparator();
  
  int vstat_5p0v = voltage_status & 0x1;          // 5.0V power supply OK
  int vstat_3p3v = (voltage_status >> 1) & 0x1;   // 3.3V power supply OK
  int vstat_1p8v = (voltage_status >> 1) & 0x1;   // 1.8V power supply OK
  int vstat_1p5v = (voltage_status >> 1) & 0x1;   // 1.5V power supply OK
  int tcrit      = (voltage_status >> 1) & 0x1;   // FPGA and board temperature OK

  bool test5p0 = compareValues("5.0V status bit",vstat_5p0v,0x1,true);
  bool test3p3 = compareValues("3.3V status bit",vstat_3p3v,0x1,true);
  bool test1p8 = compareValues("1.8V status bit",vstat_1p8v,0x1,true);
  bool test1p5 = compareValues("1.5V status bit",vstat_1p5v,0x1,true);
  bool testTcrit = compareValues("FPGA and board temperatures status bit",tcrit,0x1,true);

  bool voltageDisc = (test5p0 &&
		      test3p3 &&
		      test1p8 &&
		      test1p5 &&
		      testTcrit);

  float adc_voltage[13];

  tmb_->ADCvoltages(adc_voltage);

  float v5p0	=adc_voltage[0];	      
  float	v3p3	=adc_voltage[1];
  float v1p5core=adc_voltage[2];
  float v1p5tt	=adc_voltage[3];
  float v1p0	=adc_voltage[4];
  float a5p0	=adc_voltage[5];	      
  float	a3p3	=adc_voltage[6];
  float	a1p5core=adc_voltage[7];
  float a1p5tt	=adc_voltage[8];
  float	a1p8rat	=adc_voltage[9];	        // if SH921 set 1-2, loop backplane sends 1.500vtt
  //  float v3p3rat	=adc_voltage[9];	// if SH921 set 2-3
  float	v1p8rat =adc_voltage[10];
  float	vref2   =adc_voltage[11];
  float vzero   =adc_voltage[12];
  float	vref    =adc_voltage[13];

  bool v5p0OK     = compareValues("+5.0V TMB      ",v5p0    ,5.010,0.025);
  bool v3p3OK     = compareValues("+3.3V TMB      ",v3p3    ,3.218,0.025);
  bool v1p5coreOK = compareValues("+1.5V core     ",v1p5core,1.506,0.025);
  bool v1p5ttOK   = compareValues("+1.5V TT       ",v1p5tt  ,1.489,0.025);
  bool v1p0OK     = compareValues("+1.0V TT       ",v1p0    ,1.005,0.050);
  bool v1p8ratOK  = compareValues("+1.8V RAT core ",v1p8rat ,1.805,0.025);
  bool vref2OK    = compareValues("+vref/2        ",vref2   ,2.048,0.001);
  bool vzeroOK    = compareValues("+vzero         ",vzero   ,0.0  ,0.001);
  bool vrefOK     = compareValues("+vref          ",vref    ,4.095,0.001);

  float atol = 0.16;
  //  bool a5p0OK     = compareValues("+5.0A TMB      ",a5p0    ,0.245,atol);
  //  bool a3p3OK     = compareValues("+3.3A TMB      ",a3p3    ,1.260,atol);
  //  bool a1p5coreOK = compareValues("+1.5A TMB Core ",a1p5core,0.095,atol);
  //  bool a1p5ttOK   = compareValues("+1.5A TT       ",a1p5tt  ,0.030,atol*1.5);
  //  bool a1p8ratOK  = compareValues("+1.8A RAT Core ",a1p8rat ,0.030,atol*5.0);

  //  float t_local_f_tmb = tmb_temp(0x00,1);               // local temperature command, TMB)
  //  float t_remote_f_tmb = tmb_temp(0x01,1);              // remote temperature command, TMB)
  //  float tcrit_local_f_tmb = tmb_temp(0x05,1);           // (local tcrit command, TMB)
  //  float tcrit_remote_f_tmb = tmb_temp(0x07,1);          // (remote tcrit command, TMB)

  float ttol = 0.2;
  //  bool tlocalOK   = compareValues("T TMB pcb      ",t_local_f_tmb ,75.2,ttol);
  //  bool tremoteOK  = compareValues("T FPGA chip    ",t_remote_f_tmb,78.8,ttol);

  testOK = (v5p0OK     &&
	    v3p3OK     &&    
	    v1p5coreOK &&
	    v1p5ttOK   &&
	    v1p0OK     &&
	    v1p8ratOK  &&
	    vref2OK    &&
	    vzeroOK    &&
	    vrefOK     );//&&
  //	    a5p0OK     &&
  //	    a3p3OK     &&
  //	    a1p5coreOK &&
  //	    a1p5ttOK   &&
  //	    a1p8ratOK  );//&&
	    //	    tlocalOK   &&
	    //	    tremoteOK  );

  messageOK("ADC voltages and temperatures",testOK);
  int dummy = sleep(3);
  return testOK;
}


bool TMBTester::test3d3444(){
  (*MyOutput_) << "TMBTester: Verifying 3d3444 operation" << std::endl;
  bool testOK = false;

  bool tempbool = true;  

  int device; 
  
  unsigned short int ddd_delay;
  int initial_data;
  int delay_data;

  for (device=0; device<=11; device++){

    initial_data=tmb_->tmb_read_delays(device);   //initial value of delays

    for (ddd_delay=0; ddd_delay<=15; ddd_delay++ ) {
      tmb_->tmb_clk_delays(ddd_delay,device);              
      delay_data=tmb_->tmb_read_delays(device);
      tempbool &= compareValues("delay values ",delay_data,ddd_delay,true);
    }

    tmb_->tmb_clk_delays(initial_data,device);              
  }
  
  testOK = tempbool;

  messageOK("3d3444 Verification",testOK);
  int dummy = sleep(3);
  return testOK;
}


void TMBTester::RatTmbDelayScan(){
  //** Find optimal rpc_clock delay = phasing between RAT board and TMB **

  int i,bit;
  int write_data, read_data;
  int ddd_delay;
  int rpc_bad[16] = {};

  //Put RAT into correct mode [0]=sync-mode -> sends a fixed data pattern to TMB to be analyzed
  //                          [1]=posneg    -> inserts 12.5ns (1/2-cycle) delay in RPC data path
  //                                           to improve syncing to rising edge of TMB clock
  //                          [2]=loop_tmb
  //                          [3]=free_tx0
  write_data = 0x0003;
  tmb_->WriteRegister(vme_ratctrl_adr,write_data);

  //here are the arrays of bits we expect from sync mode:
  const int nbits = 19;
  int rpc_rdata_expect[4][nbits];
  bit_to_array(0x2aaaa,rpc_rdata_expect[0],nbits);
  bit_to_array(0x55555,rpc_rdata_expect[1],nbits);
  bit_to_array(0x55555,rpc_rdata_expect[2],nbits);
  bit_to_array(0x2aaaa,rpc_rdata_expect[3],nbits);

  //  for (i=0; i<=3; i++) {
  //    (*MyOutput_) << "rpc_rdata_expect[" << i << "] = ";
  //    for (bit=0; bit<=(nbits-1); bit++) {
  //      (*MyOutput_) << rpc_rdata_expect[i][bit] << " ";
  //    }
  //    (*MyOutput_) << std::endl;
  //  }

  //enable RAT input into TMB...
  read_data = tmb_->ReadRegister(rpc_inj_adr);
  write_data = read_data | 0x0001;
  tmb_->WriteRegister(rpc_inj_adr,write_data);

  //Initial delay values:
  int rpc_delay_default = tmb_->tmb_read_delays(8);

  int irat;

  int rpc_rbxn[4],rpc_rdata[4],rpcData[4];
  int rpc_data_array[4][nbits];

  int pass;
  int count_bad;

  //step through ddd_delay
  for (pass=0; pass<=1000; pass++) {
    for (ddd_delay=0; ddd_delay<=15; ddd_delay++) {
      count_bad=0;

      // ** write the delay to the RPC **
      tmb_->tmb_clk_delays(ddd_delay,8);

      // ** read RAT 80MHz demux registers**
      for (irat=0; irat<=3; irat++) {

	read_data = tmb_->ReadRegister(rpc_cfg_adr);
	read_data &= 0xf9ff;                        //zero out old RAT bank
	write_data = read_data | (irat << 9);       //select RAT RAM bank
	tmb_->WriteRegister(rpc_cfg_adr,write_data);

	read_data = tmb_->ReadRegister(rpc_cfg_adr);
	rpc_rbxn[irat] = (read_data >> 11) & 0x0007;  //RPC MSBS for sync mode

	rpc_rdata[irat] = tmb_->ReadRegister(rpc_rdata_adr) & 0xffff; //RPC RAM read data for sync mode (LSBS)

	rpcData[irat] = rpc_rdata[irat] | (rpc_rbxn[irat] << 16);  //pack MS and LSBs into single integer
	
	bit_to_array(rpcData[irat],rpc_data_array[irat],nbits);
	
	for (i=0; i<=(nbits-1); i++) {
	  if (rpc_data_array[irat][i] != rpc_rdata_expect[irat][i]) count_bad += 1;
	}
      }

      //      for (i=0; i<=3; i++) {
      //	(*MyOutput_) << "rpc_data_array[" << i << "] = ";
      //	for (bit=0; bit<=(nbits-1); bit++) {
      //	  (*MyOutput_) << rpc_data_array[i][bit] << " ";
      //	}
      //	(*MyOutput_) << std::endl;
      //      }

      rpc_bad[ddd_delay] += count_bad;

    }
  }

  // Put RPC delay back to initial values:
  (*MyOutput_) << "Putting delay values back to " << rpc_delay_default << std::endl;
  tmb_->tmb_clk_delays(rpc_delay_default,8);

  // ** Take TMB out of sync mode **
  write_data = 0x0000;
  tmb_->WriteRegister(vme_ratctrl_adr,write_data);

  int rpc_delay;

  // ** print out results **
  (*MyOutput_) << "rpc_delay   bad data count" << std::endl;
  (*MyOutput_) << "---------   --------------" << std::endl;
  for (rpc_delay = 0; rpc_delay <=15; rpc_delay++) {
    (*MyOutput_) << "    " << rpc_delay 
	      << "           " << rpc_bad[rpc_delay] 
	      <<std::endl;
  }

  return;
}


void TMBTester::RatStatusRegister(){
  (*MyOutput_) << "TMBTester: Read RAT USER1 JTAG (status register)" << std::endl;

  int read_data;
  int opcode,chip_id;

  // First make sure the RPC FPGA is finished:
  read_data = tmb_->ReadRegister(rpc_cfg_adr);
  int rpc_done = (read_data >> 14) & 0x1;
  (*MyOutput_) << "RPC done = " << rpc_done << std::endl;

  set_jtag_address(TMB_ADR_BOOT);    
  set_jtag_chain(RAT_CHAIN);         

  (*MyOutput_) << "JTAG address = " << std::hex << jtag_address;
  (*MyOutput_) << " and JTAG chain = " << std::hex << jtag_chain << std::endl;

  // **Take the chosen chain's TAP to RTI (via the chosen address)**
  step_mode = false;
  jtag_anystate_to_rti();    

  const int register_length = 32;  
  int value[register_length];

  int idcode;

  for (chip_id=0; chip_id<=1; chip_id++){
    if (chip_id == 0) {
      opcode = VTX2_IDCODE;                  
    } else { 
      opcode = PROM_IDCODE;                  
    }
    step_mode = false;
    do_jtag(chip_id,opcode,register_length,value);

    idcode = bits_to_int(value,register_length,0);

    (*MyOutput_) << "RAT ID code = " << idcode;

    if (chip_id == 0) {
      (*MyOutput_) << " <- PROM " << std::endl;
    } else if (chip_id == 1) {
      (*MyOutput_) << " <- FPGA " << std::endl;
    }
  }

  int usercode;
  for (chip_id=0; chip_id<=1; chip_id++){
    if (chip_id == 0) {
      opcode = VTX2_USERCODE;                  
    } else { 
      opcode = PROM_USERCODE;                  
    }
    do_jtag(chip_id,opcode,register_length,value);

    usercode = bits_to_int(value,register_length,0);

    (*MyOutput_) << "RAT User code = " << usercode;

    if (chip_id == 0) {
      (*MyOutput_) << " <- PROM " << std::endl;
    } else if (chip_id == 1) {
      (*MyOutput_) << " <- FPGA " << std::endl;
    }
  }

  const int user1_length = 224;
  int user1_value[user1_length];
  chip_id=0;

  do_jtag(chip_id,VTX2_USR1,user1_length,user1_value);

  int bit_array[7][32];
  int first,second;

  int counter=0;
  for (first=0; first<7; first++) {
    for (second=0; second<32; second++) {
      bit_array[first][second] = user1_value[counter++];
    }
  }
  int rat_user1[7];

  for (first=0; first<7; first++) 
    rat_user1[first] = bits_to_int(bit_array[first],32,0);
  
  (*MyOutput_) << "RAT USER1 = ";
  for (first=0; first<7; first++) 
    (*MyOutput_) << rat_user1[first];
  (*MyOutput_) << std::endl;


  return;
}

/////////////////////////////////////////
// Functions for JTAG:
/////////////////////////////////////////
void TMBTester::jtag_anystate_to_rti() {
  //take JTAG tap from any state to TLR then to RTI

  const int mxbits = 6;           // bits of TMS

  int tms[mxbits];
  int tdi[mxbits];
  int tdo[mxbits];

  int tms_rti[mxbits] = {1, 1, 1, 1, 1, 0}; //Anystate to TLR then RTI
  int tdi_rti[mxbits] = {};

  int iframe = 0;                                     //JTAG frame number

  for (int ibit=0; ibit<mxbits; ibit++) {             // go from any state to RTI
    tms[iframe]=tms_rti[ibit];                        // take tap to RTI
    tdi[iframe++]=tdi_rti[ibit];
  }

  jtag_io_byte(iframe,tms,tdi,tdo);

  return;
}


void TMBTester::jtag_io_byte(int nframes,
			     int * tms,
			     int * tdi,
			     int * tdo) {

  //	Clocks tck for nframes number of cycles.
  //	Writes nframes of data to tms and tdi on the falling edge of tck.
  //	Reads tdo on the rising clock edge.
  //
  //	Caller passes tms and tdi byte arrays with 1 bit per byte.	
  //	Returned data is also 1 bit per byte. Inefficent,but easy.
  //
  //	tms[]	=	byte's lsb to write to parallel port
  //	tdi[]	=	byte's lsb to write to parallel port
  //	tdo[]	=	bit read back from parallel port, stored in lsb
  //	tck		=	toggled by this routine

  // get current boot register:
  short unsigned int boot_state;
  int dummy = tmb_->tmb_get_boot_reg(&boot_state);
  boot_state &= 0x7f80;                 //Clear JTAG bits

  //	Set tck,tms,tdi low, select jtag chain, enable VME control of chain

  int tck_bit = 0;                      //TCK low
  int tms_bit = 0;                      //TMS low
  int tdi_bit = 0;                      //TDI low
  int tdo_bit;
  int jtag_en = 1;                      //Boot register sources JTAG

  int sel0 = jtag_chain & 0x1;               //JTAG chain select
  int sel1 = (jtag_chain>>1) & 0x1;
  int sel2 = (jtag_chain>>2) & 0x1;
  int sel3 = (jtag_chain>>3) & 0x1;

  int jtag_word = boot_state;

  jtag_word |= tdi_bit;
  jtag_word |= tms_bit << 1;
  jtag_word |= tck_bit << 2;
  jtag_word |= sel0    << 3;
  jtag_word |= sel1    << 4;
  jtag_word |= sel2    << 5;
  jtag_word |= sel3    << 6;
  jtag_word |= jtag_en << 7;

  dummy = tmb_->tmb_set_boot_reg(jtag_word);  //write boot register
  
  short unsigned int jtag_in;
  int jtag_out;

  //Loop over input data frames
  int iframe;

  if (nframes>=1) {                       //no frames to send

    //loop over input data frames...
    for (iframe=0; iframe<nframes; iframe++) { //arrays count from 0
      tdo[iframe]=0;                               //clear tdo

      tck_bit = 0x0 << 2;                          //take TCK low
      tms_bit = (tms[iframe] & 0x1) << 1;                  //TMS bit
      tdi_bit = (tdi[iframe] & 0x1);                       //TDI bit

      jtag_out = jtag_word & 0x7ff8;               //clear old state
      jtag_out |= tck_bit | tms_bit | tdi_bit;

      dummy = tmb_->tmb_set_boot_reg(jtag_out);    //write boot register

      dummy = tmb_->tmb_get_boot_reg(&jtag_in);    //read boot register
      //** here only read tdo for step_mode **
      tdo[iframe] = (jtag_in >> 15) & 0x1;         //extract tdo bit, mask lsb
      tdo_bit = tdo[iframe];
      std::cout << "tdo[" << iframe << "] = " << tdo[iframe] <<std::endl;

      if (step_mode) {
	step(tck_bit,tms_bit,tdi_bit,tdo_bit);
      }

      tck_bit = 0x1 << 2;  //Take TCK high, leave tms,tdi as they were
      jtag_out |= tck_bit | tms_bit | tdi_bit;
      dummy = tmb_->tmb_set_boot_reg(jtag_out);    //write boot register

      dummy = tmb_->tmb_get_boot_reg(&jtag_in);    //read boot register
      //** here is the real tdo, read on the rising edge **
      tdo[iframe] = (jtag_in << 15) & 0x1;         //extract tdo bit, mask lsb
      tdo_bit = tdo[iframe];
      //      std::cout << "tdo[" << iframe << "] = " << tdo[iframe] <<std::endl;

      if (step_mode) {
	step(tck_bit,tms_bit,tdi_bit,tdo_bit);
      }      
    }
  }
  //** put JTAG bits into an idle state **
  jtag_out &= 0xfffb;     //Take TCK low, leave others as they were
  dummy = tmb_->tmb_set_boot_reg(jtag_out);        //write boot register

  for (iframe=0; iframe<nframes; iframe++){
    std::cout << "tdo[" << iframe << "] = " << tdo[iframe] <<std::endl;
  }


  return;
}

void TMBTester::step(int tck, 
		     int tms, 
		     int tdi, 
		     int tdo){

  (*MyOutput_) << "tck = " << std::hex << tck
	    << " tms = " << std::hex << tms
	    << " tdi = " << std::hex << tdi
	    << " tdo = " << std::hex << tdo
	    << " pause.... enter any number, then return...";
  int dummy;
  std::cin >> dummy;

  return;
}

void TMBTester::set_jtag_chain(int chain) { 

  jtag_chain = chain;
  select_jtag_chain_param();

  return;
}

void TMBTester::select_jtag_chain_param(){
  //select:
  //  - how many chips are on this chain
  //  - how many bits per opcode (per chip)

  devices_in_chain = 0;

  if (jtag_chain == TMB_USER_PROM_CHAIN ||
      jtag_chain == TMB_FPGA_CHAIN) {
    devices_in_chain = 2;
    bits_per_opcode[0] = 8;
    bits_per_opcode[1] = 8;
    bits_per_opcode[2] = 0;
    bits_per_opcode[3] = 0;
    bits_per_opcode[4] = 0;

  } else if (jtag_chain == RAT_CHAIN            ||
	     jtag_chain == ALCT_MEZZ_PROM_CHAIN ||
	     jtag_chain == ALCT_MEZZ_USER_CHAIN ) {
    devices_in_chain = 2;
    bits_per_opcode[0] = 5;
    bits_per_opcode[1] = 8;
    bits_per_opcode[2] = 0;
    bits_per_opcode[3] = 0;
    bits_per_opcode[4] = 0;
  } else if (jtag_chain== TMB_MEZZ_FPGA_CHAIN) {
    devices_in_chain = 5;
    bits_per_opcode[0] = 6;
    bits_per_opcode[1] = 8;
    bits_per_opcode[2] = 8;
    bits_per_opcode[3] = 8;
    bits_per_opcode[4] = 8;
  }

  if (devices_in_chain == 0) {
    (*MyOutput_) << "ERROR unsupported JTAG chain " << jtag_chain <<std::endl;
    int dummy = sleep(5);
  } else {
    (*MyOutput_) << "JTAG chain " << std::hex << jtag_chain 
		 << " has " << devices_in_chain << " devices " 
		 << " and " << bits_per_opcode[0] << " "
		 << ", " << bits_per_opcode[1] << " "
		 << ", " << bits_per_opcode[2] << " "
		 << ", " << bits_per_opcode[3] << " "
		 << ", " << bits_per_opcode[4] << " bits per opcode"
		 <<std::endl;
  }

  return;
}

void TMBTester::do_jtag(int chip_id, 
			int opcode, 
			int length, 
			int *val) {

  // JTAG instructions and data are loaded for PROM1 first and PROM0 last, i.e.:
  //    JTAG position     Chip ID   Order  Load Name   TMB Name     RAT
  //    -------------     -------   -----  ---------   --------    ------
  //        tdi>0            0                 1        PROM0       FPGA
  //        tdo<1            1                 0        PROM1       PROM
  
  //Chains currently not supported by this member:
  //    ALCT_SLOW_USER_CHAIN
  //    ALCT_SLOW_PROM_CHAIN
  //    TMB_USER_PROM_CHAIN  

  (*MyOutput_) << "call TMBTester::do_jtag op " << std::hex << opcode 
	       << " to JTAG address " << std::hex << jtag_address
	       << " for chip " << std::hex <<chip_id 
	       << std::endl;
  int k;
  int     iframe;                                               //JTAG frame counter
  int tdi[MXBITSTREAM], tms[MXBITSTREAM], tdo[MXBITSTREAM];

  int promIR[8] = {1, 0, 0, 2, 2, 0, 0, 0};     //Values the PROM IR should return.... -> 2 = don't care...

  // ** First JTAG operation writes opcodes to the TAP instruction registers...
  // ** Assume TAP controllers begin in the RTI state ***

  iframe = 0;

  // Put TAP in state ShfIR 
  for (k = 0; k < 4; k++) {
    tms[iframe] = tms_pre_opcode[k];
    tdi[iframe++] = tdi_pre_opcode[k];
  }

  // ** Construct opcode for the selected chip (all but chip_id are BYPASS = all 1's) **
  int idevice, ichip, ibit;
  int bit;

  for (idevice=0; idevice<devices_in_chain; idevice++) {   //loop over all the chips up to the number of chips in this chain

    ichip = devices_in_chain - idevice - 1;                //chip order in chain is reversed

    for (ibit=0; ibit<bits_per_opcode[ichip]; ibit++) {    //up to the number of bits in this chip's opcode
      bit = 1;                                             //BYPASS
      if (ichip == chip_id) {                              //this is the chip we want
	bit = (opcode << ibit) & 0x1;                      //extract bit from opcode
      }
      tms[iframe]=0;                                       // TAP stays in ShfIR
      tdi[iframe++]=bit;                                   // Instruction bit, advance the frame
    }
  }

  // ** Put TAP from Ex1IR to RTI **
  for (k = 0; k < 2; k++) {
    tms[iframe] = tms_post_opcode[k];
    tdi[iframe++] = tdi_post_opcode[k];
  }

  if (iframe > MXBITSTREAM) {
    (*MyOutput_) << "do_jtag IR ERROR: Too many frames -> " << iframe << std::endl;
    (*MyOutput_) << "STOP the program (ctrl-c)...";
    std::cin >> k;
  }

  jtag_io_byte(iframe,tms,tdi,tdo);

  for (k=0;k<iframe;k++) {
    std::cout << "tdo[" << k << "] =" << tdo[k] << std::endl;
  }

  // **Check that the TDO shifted out instruction register strings**
  int iopbit;
  if ( jtag_chain != TMB_USER_PROM_CHAIN &&
       jtag_chain != TMB_FPGA_CHAIN ) {
    iframe = 5;                                    //start at first opcode bit

    for (idevice=0; idevice<devices_in_chain; idevice++) {   //loop over all the chips up to the number of chips in this chain

      ichip = devices_in_chain - idevice - 1;                //chip order in chain is reversed

      //      (*MyOutput_) << "Chip " << ichip 
      //		   << " has " << bits_per_opcode[ichip] 
      //		   << " bits per opcode" << std::endl;

      for (ibit=0; ibit<bits_per_opcode[ichip]; ibit++) {    //up to the number of bits in this chip's opcode
	iopbit = tdo[iframe++];                              //current opcode
	if ( iopbit != promIR[ibit] &&
	     promIR[ibit] != 2      ) {
	  (*MyOutput_) << "ERROR in opcode bit" << std::hex << ibit 
		       << ", Read->" << std::hex << iopbit 
		       << ", expected =" << std::hex << promIR[ibit] 
		       << std::endl;
	}	  
      }
    }
    (*MyOutput_) << "JTAG write Instruction Register: ";
    for (k=12; k>=5; k--) {
      (*MyOutput_) << tdo[k] << " ";
    }
    (*MyOutput_) << std::endl;
  }

  (*MyOutput_) << "Waiting for input... " << std::endl;
  std::cin >> k;

  // ** Now that the desired "opcode" has been written to "chip_id",
  //    Perform the second JTAG operation, which is to write TDI bits
  //    for the currently selected data register and read back the 
  //    TDO data which ought to be "length" bits long, into the
  //    array "val"

  iframe = 0;

  // Put TAP in state ShfIR 
  for (k = 0; k < 3; k++) {
    tms[iframe] = tms_pre_read[k];
    tdi[iframe++] = tdi_pre_read[k];
  }

  // ** Set up TMS to shift in the data bits for this chip, BYPASS code for others **
  int offset;
  for (idevice=0; idevice<devices_in_chain; idevice++) {   //loop over all the chips up to the number of chips in this chain

    ichip = devices_in_chain - idevice - 1;                //chip order in chain is reversed

    if (ichip == chip_id) {                                //this is the chip we want
      offset = iframe;                                     //here is the beginning of the data
      for (ibit=0; ibit<bits_per_opcode[ichip]; ibit++)     //up to the number of bits in this chip's opcode
	tms[iframe++] = 0;                                   // Stay in ShfDR, but leave tdi as it was for IR...
    } else {
      tms[iframe] = 0;                                       // TAP stays in ShfDR
      tdi[iframe++] = 0;                                     // No going out to bypass regs, bypass register is one frame      
    }
  }

  tms[iframe-1] = 1;                                         // Last opcode, TAP goes to Ex1DR
  
  // ** Put TAP from Ex1IR to RTI **
  for (k = 0; k < 2; k++) {
    tms[iframe] = tms_post_read[k];
    tdi[iframe++] = tdi_post_read[k];
  }

  if (iframe > MXBITSTREAM) {
    (*MyOutput_) << "do_jtag DR ERROR: Too many frames -> " << iframe << std::endl;
    (*MyOutput_) << "STOP the program (ctrl-c)...";
    std::cin >> k;
  }

  jtag_io_byte(iframe,tms,tdi,tdo);

  // ** copy relevant section of tdo to caller's read array **
  for (ibit=0; ibit<length; ibit++) {
    iframe = ibit+offset;
    if (iframe > MXBITSTREAM) {
      (*MyOutput_) << "do_jtag copy ERROR: Too many frames -> " << iframe << std::endl;
      (*MyOutput_) << "STOP the program (ctrl-c)...";
      std::cin >> k;
    }
    val[ibit] = tdo[iframe];
    if (jtag_chain == ALCT_MEZZ_USER_CHAIN) val[ibit] = tdo[iframe-1];  //ALCT fix (Jonathan)
  }

  return;
}

int TMBTester::bits_to_int(int * bits,
			   int length,
			   int MsbOrLsb) {
  //convert bits array of 1 bit per bye into an integer
  // MsbOrLsb for DACs that take MSB first
  int value;
  int dummy;
  int ibit;

  if (length>32) {
      (*MyOutput_) << "bits_to_int ERROR: Too many bits -> " << length << std::endl;
      (*MyOutput_) << "STOP the program (ctrl-c)...";
      std::cin >> dummy;
  }

  value = 0;
  if (MsbOrLsb == 0) {       // Translate LSB first    
    for (ibit=0; ibit<length; ibit++) 
      value |= ((bits[ibit]&0x1) << ibit);
  } else {                   // Translate MSB first
    for (ibit=0; ibit<length; ibit++) 
      value |= ((bits[length-ibit-1]&0x1) << ibit);
  }

  return value;
}

/////////////////////////////////////////
// END:  Functions for JTAG
/////////////////////////////////////////



/////////////////////////////////////////
// Functions needed to implement tests:
/////////////////////////////////////////
bool TMBTester::compareValues(std::string TypeOfTest, 
                              int testval, 
                              int compareval,
			      bool equal) {

// test if "testval" is equivalent to the expected value: "compareval"
// return depends on if you wanted them to be "equal"

  (*MyOutput_) << "compareValues:  " << TypeOfTest << " -> ";
  if (equal) {
    if (testval == compareval) {
      (*MyOutput_) << "PASS = " << std::hex << compareval << std::endl;
      return true;
    } else {
      (*MyOutput_) << "FAIL!" << std::endl;
      (*MyOutput_) << TypeOfTest 
		<< " expected value = " << std::hex << compareval
		<< ", returned value = " << std:: hex << testval
		<< std::endl;
      return false;
    }
  } else {
    if (testval != compareval) {
      (*MyOutput_) << "PASS -> " << std::hex << testval 
		<< " not equal to " <<std::hex << compareval 
		<< std::endl;
      return true;
    } else {
      (*MyOutput_) << "FAIL!" << std::endl;
      (*MyOutput_) << TypeOfTest 
		<< " expected = returned = " << std::hex << testval
		<< std::endl;
      return false;
    }
  }

}

bool TMBTester::compareValues(std::string TypeOfTest, 
                              float testval, 
                              float compareval,
			      float tolerance) {

// test if "testval" is within "tolerance" of "compareval"...

  (*MyOutput_) << "compareValues tolerance:  " << TypeOfTest << " -> ";

  float err = (testval - compareval)/compareval;

  float fractolerance = tolerance*compareval;

  if (fabs(err)>tolerance) {
      (*MyOutput_) << "FAIL!" << std::endl;
      (*MyOutput_) << TypeOfTest 
		<< " expected = " << compareval 
		<< ", returned = " << testval
		<< " outside of tolerance "<< fractolerance
		<< std::endl;
      return false;
  } else {
      (*MyOutput_) << "PASS!" << std::endl;
      (*MyOutput_) << TypeOfTest 
		<< " value = " << testval
		<< " within "<< tolerance
		<< " of " << compareval
		<< std::endl;
      return true;
  }

}

void TMBTester::messageOK(std::string TypeOfTest,
			  bool testbool){
  (*MyOutput_) << TypeOfTest;
  if (testbool) {
    (*MyOutput_) << " -> PASS" << std::endl;
  } else {
    (*MyOutput_) << " -> FAIL <-" << std::endl;
  }
  return;
}

int TMBTester::dowCRC(std::bitset<64> DSN) {

  // "Calculate CRC x**8 + x**5 + X**4 +1
  //  for 7-byte Dallas Semi i-button data"
  //    header to dow_crc.for, written by Jonathan Kubik

  int ibit;

  int sr[8];
  //initialize CRC shift register
  for (ibit=0; ibit<=7; ibit++) {
    sr[ibit]=0;
  }

  int x8=0;
  int bit;

  //loop over 56 data bits, LSB first:
  for (ibit=0; ibit<=55; ibit++) {
    x8 = DSN[ibit]^sr[7];
    sr[7] = sr[6];
    sr[6] = sr[5];
    sr[5] = sr[4]^x8;
    sr[4] = sr[3]^x8;
    sr[3] = sr[2];
    sr[2] = sr[1];
    sr[1] = sr[0];
    sr[0] = x8;
  }

  //pack shift register into a byte
  int crc=0;
  for (ibit=0; ibit<=7; ibit++) {
    crc |= sr[ibit] << (7-ibit);
  }

  return crc;
}

void TMBTester::bit_to_array(int data, int * array, const int size) {
  int i;
  for (i=0; i<=(size-1); i++) {
    array[i] = (data >> i) & 0x00000001;
  }

  return;
}


float TMBTester::tmb_temp(int cmd,int module) {
  // returns TMB temperature values in Farenheit...

  //	Generates SMB serial clock and data streams to TMB LM84 chip
  //
  //	SMB requires that serial data is stable while clock is high,
  //	so data transitions occur while clock is low,
  //	midway between clock falling edge and rising edge

  //    cmd = 0x00 = local temperature command
  //        = 0x01 = remote temperature command
  //        = 0x05 = local tcrit command  
  //        = 0x07 = remote tcrit command  

  //    module = 1 = TMB
  //           = 2 = RAT

  const int smb_adr = 0x2a;   // float, float state TMB LM84 chip address
  const int adc_adr = vme_adc_adr;

  int smb_data = 0xff;        // null write command

  int sda_bit[29];

  int write_data,read_data;
  int dummy;

  float fdummy;

  // Current ADC register state:
  int adc_status = tmb_->PowerComparator();

  (*MyOutput_) << "Initial ADC status = " << std::hex << adc_status << std::endl;

  // **Step 1 write the data**

  // ** initialize SMB data stream **
  sda_bit[0] = 0;                                // Start
  sda_bit[1] = (smb_adr >> 6) & 1;               // A6
  sda_bit[2] = (smb_adr >> 5) & 1;               // A5
  sda_bit[3] = (smb_adr >> 4) & 1;               // A4
  sda_bit[4] = (smb_adr >> 3) & 1;               // A3
  sda_bit[5] = (smb_adr >> 2) & 1;               // A2
  sda_bit[6] = (smb_adr >> 1) & 1;               // A1
  sda_bit[7] = (smb_adr >> 0) & 1;               // A0
  sda_bit[8] = 0;                                // 0 = write command register                           
  sda_bit[9] = 1;                                // ACK
  sda_bit[10]= (cmd     >> 7) & 1;               // C7 
  sda_bit[11]= (cmd     >> 6) & 1;               // C6 
  sda_bit[12]= (cmd     >> 5) & 1;               // C5 
  sda_bit[13]= (cmd     >> 4) & 1;               // C4 
  sda_bit[14]= (cmd     >> 3) & 1;               // C3 
  sda_bit[15]= (cmd     >> 2) & 1;               // C2 
  sda_bit[16]= (cmd     >> 1) & 1;               // C1 
  sda_bit[17]= (cmd     >> 0) & 1;               // C0 
  sda_bit[18]= 1;                                // ACK
  sda_bit[19]= (smb_data>> 7) & 1;               // D7 write data register
  sda_bit[20]= (smb_data>> 6) & 1;               // D6 write data register
  sda_bit[21]= (smb_data>> 5) & 1;               // D5 write data register
  sda_bit[22]= (smb_data>> 4) & 1;               // D4 write data register
  sda_bit[23]= (smb_data>> 3) & 1;               // D3 write data register
  sda_bit[24]= (smb_data>> 2) & 1;               // D2 write data register
  sda_bit[25]= (smb_data>> 1) & 1;               // D1 write data register
  sda_bit[26]= (smb_data>> 0) & 1;               // D0 write data register
  sda_bit[27]= 1;                                // ACK
  sda_bit[28]= 0;                                // Stop

  // ** Construct SMBclk and SMBdata **

  (*MyOutput_) << "smb address = " << std::hex << smb_adr
	    << ", command = " << std::hex << cmd
	    << ", smb data = " << std::hex << smb_data
	    << std::endl;

  int nclks = 115;
  //  int nclks = 50;

  int sda_clock,scl_clock;
  int sda,scl;

  int i2c_clock;
  for (i2c_clock=0; i2c_clock<=nclks; i2c_clock++) {  //200kHz
    sda_clock = (int) i2c_clock/4;                //50 kHz
    scl_clock = (int) ( (i2c_clock+1)/2 );        //50 kHz shifted 1/2 of a 100kHz cycle

    scl = scl_clock & 1;                          // 0 0 1 1 0 0 1 1 0 0 1 1 ....
    sda = sda_bit[sda_clock];

    (*MyOutput_) << "Before Persistent -> i2c_clock " << i2c_clock << ", scl = " << scl << " sda_bit = " << sda << std::endl;

    if (i2c_clock<3) scl=1;                       // START scl stays high
    if (i2c_clock<2) sda=1;                       // START sda transitions low

    if (i2c_clock>nclks-3) scl=1;                // STOP scl stays high
    if (i2c_clock>nclks-2) sda=1;                // STOP sda transitions high

    (*MyOutput_) << "After Persistent  -> i2c_clock " << i2c_clock << ", scl = " << scl << " sda_bit = " << sda << std::endl;

    //** Write serial clock and data to TMB VME interface **

    write_data = adc_status & 0xf9ff;    //clear bits 9 and 10
    write_data |= scl << 9;
    write_data |= sda << 10;
    tmb_->WriteRegister(adc_adr,write_data);

    (*MyOutput_) << "write_data =" << write_data << std::endl;

    //    dummy=sleep(1);  // adjust wait time so scl is 50kHz or slower...
  }

  //  dummy=sleep(10);

  // Current ADC register state:
  adc_status = tmb_->PowerComparator();

  // **Step 1 read the data**

  // ** initialize SMB data stream **
  sda_bit[0] = 0;                                // Start
  sda_bit[1] = (smb_adr >> 6) & 1;               // A6
  sda_bit[2] = (smb_adr >> 5) & 1;               // A5
  sda_bit[3] = (smb_adr >> 4) & 1;               // A4
  sda_bit[4] = (smb_adr >> 3) & 1;               // A3
  sda_bit[5] = (smb_adr >> 2) & 1;               // A2
  sda_bit[6] = (smb_adr >> 1) & 1;               // A1
  sda_bit[7] = (smb_adr >> 0) & 1;               // A0
  sda_bit[8] = 1;                                // 1 = read data register                           
  sda_bit[9] = 1;                                // ACK
  sda_bit[10]= 1;                                // D7 read from LM84, 1=z output from fpga
  sda_bit[11]= 1;                                // D6
  sda_bit[12]= 1;                                // D5
  sda_bit[13]= 1;                                // D4
  sda_bit[14]= 1;                                // D3
  sda_bit[15]= 1;                                // D2
  sda_bit[16]= 1;                                // D1
  sda_bit[17]= 1;                                // D0
  sda_bit[18]= 1;                                // ACK
  sda_bit[19]= 0;                                // Stop

  int d[20];
  int sda_value;

  nclks = 79;

  for (i2c_clock=0; i2c_clock<=nclks; i2c_clock++) {  //200kHz
    sda_clock = (int) i2c_clock/4;                //50 kHz
    scl_clock = (int) ( (i2c_clock+1)/2 );        //50 kHz shifted 1/2 of a 100kHz cycle

    scl = scl_clock & 1;                          // 0 0 1 1 0 0 1 1 0 0 1 1 ....
    sda = sda_bit[sda_clock];

    if (i2c_clock<3) scl=1;                       // START scl stays high
    if (i2c_clock<2) sda=1;                       // START sda transitions low

    if (i2c_clock>nclks-3) scl=1;                // STOP scl stays high
    if (i2c_clock>nclks-2) sda=1;                // STOP sda transitions high

    //** Write serial clock and data to TMB VME interface **

    write_data = adc_status & 0xf9ff;    //clear bits 9 and 10
    write_data |= scl << 9;
    write_data |= sda << 10;
    tmb_->WriteRegister(adc_adr,write_data);
    //    dummy=sleep(1);  // adjust wait time so scl is 50kHz or slower...

    //Read Serial data from TMB VME interface
    //read on every cycle to keep clock symmetric

    read_data = tmb_->ReadRegister(adc_adr);
    if (scl==1) {
      d[sda_clock] = read_data;
      (*MyOutput_) << "reading serial data d[" << sda_clock << "] =" << d[sda_clock] << std::endl;
    }
    //    dummy=sleep(1);  // adjust wait time so scl is 50kHz or slower...
  }

  // pack data into an integer...

  int ishift = 0;
  if (module == 1) ishift = 10;   //data bit from LM84 on TMB
  if (module == 2) ishift = 11;   //data bit from LM84 on RAT

  int i;
  int data = 0;
  for (i=0; i<=31; i++) {
    if (i<=7) {
      sda_value = (d[17-i]>>ishift) & 0x1;
      data |= sda_value<<i;          //d[7:0]
      //      (*MyOutput_) << "d[17-" << i << "] =" << d[17-i] << "sda_value = " << sda_value << std::endl;
    } else {
      data |= sda_value<<i;          //sign extend if bit 7 indicates negative value      
    }
    //    (*MyOutput_) << "sda_value = " << sda_value << ", data =" << data << std::endl;
  }

  (*MyOutput_) << "Temperature = " << std::dec << data << " deg C -> " << std::hex << data << std::endl;

  float temperature = float (data) * 9. / 5. + 32.;

  return temperature;
}
//////////////////////////////////////////////
// END: Functions needed to implement tests..
//////////////////////////////////////////////



/////////////////////////////////////
// The following should be in TMB: //
/////////////////////////////////////

//////////////////////////////////////////
// END: The following should be in TMB: //
//////////////////////////////////////////
