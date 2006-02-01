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
  //
  ResultTestBootRegister_ = -1;
  ResultTestVMEfpgaDataRegister_ = -1;
  ResultTestFirmwareDate_ = -1;
  ResultTestFirmwareType_ = -1;
  ResultTestFirmwareVersion_ = -1;
  ResultTestFirmwareRevCode_ = -1;
  ResultTestMezzId_ = -1;
  ResultTestPromId_ = -1;
  ResultTestPROMPath_ = -1;
  ResultTestDSN_ = -1;
  ResultTestADC_ = -1;
  ResultTest3d3444_ = -1;
  //
}

TMBTester::~TMBTester(){}
//

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

/////////////////////////////////////////
// TMB Tests:
/////////////////////////////////////////
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
  //dummy = sleep(3);
  //
  ResultTestBootRegister_ = testOK ;
  //
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

  //std::cout << "Read..." << std::endl;

  // Get current status:
  int vme_cfg = tmb_->ReadRegister(vme_ddd0_adr);

  //std::cout << "Done..." << std::endl;

  int write_data,read_data;
  bool tempBool;

  int register_error = 0;

  for (int i=0; i<=15; i++) {
    //
    std::cout << "Looping " << std::endl ;
    //
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
  //int dummy = sleep(3);
  //
  ResultTestVMEfpgaDataRegister_ = testOK;
  //
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
  //int dummy = sleep(3);
  //
  ResultTestFirmwareSlot_ = testOK ;
  //
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
  //int dummy = sleep(3);
  //
  ResultTestFirmwareDate_ = testOK;
  //
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
  //int dummy = sleep(3);
  //
  ResultTestFirmwareType_ = TypeNormal;
  //
  return TypeNormal;
}


bool TMBTester::testFirmwareVersion() {
  (*MyOutput_) << "TMBTester: testing Firmware Version" << std::endl;
  int firmwareData = tmb_->FirmwareVersion();
  int version = (firmwareData>>4) & 0xf;

  bool testOK = compareValues("Firmware Version",version,0xE,true);
  messageOK("Firmware Version",testOK);
  //int dummy = sleep(3);
  //
  ResultTestFirmwareVersion_ = testOK;
  //
  return testOK;
}


bool TMBTester::testFirmwareRevCode(){
  (*MyOutput_) << "TMBTester: testing Firmware Revision Code" << std::endl;

  int firmwareData = tmb_->FirmwareRevCode();

  //  int RevCodeDay = firmwareData & 0x001f;
  //  int RevCodeMonth = (firmwareData>>5) & 0x000f;
  //  int

  int RevCodeYear = (firmwareData>>9) & 0x0007;
  int RevCodeFPGA = (firmwareData>>12) & 0x000F;
  
  bool testOK = compareValues("Firmware Revcode FPGA",RevCodeFPGA,0x04,true);
  messageOK("Firmware Revcode FPGA",testOK);
  //int dummy = sleep(3);
  //
  ResultTestFirmwareRevCode_ = testOK ;
  //
  return testOK;
}


bool TMBTester::testJTAGchain(){
  (*MyOutput_) << "TMBTester: testing User and Boot JTAG chains" << std::endl;

  bool user = testJTAGchain(0);
  bool boot = testJTAGchain(1);

  bool JTAGchainOK = (user && boot);
  messageOK("JTAG chains",JTAGchainOK);
  //int dummy = sleep(3);
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
  //
  (*MyOutput_) << "TMBTester: Checking Mezzanine FPGA and PROMs ID codes" << std::endl;
  bool testOK = false;
  int idcode[5];

  set_jtag_address(TMB_ADR_BOOT);    
  set_jtag_chain(TMB_MEZZ_FPGA_CHAIN);         

  // **Take the chosen chain's TAP to RTI (via the chosen address)**
  jtag_anystate_to_rti();    
  
  int opcode;

  int value[tmb_mezzID_length];

  int write_data[MAX_FRAMES] = {};   //create fat 0 for writing to data registers

  for (int chip_id=0; chip_id<=4; chip_id++){
    if (chip_id == 0) {
      opcode = VTX2_IDCODE;                  // FPGA IDcode opcode, expect v0A30093
    } else { 
      opcode = PROM_IDCODE;                  // PROM IDcode opcode
    }
    jtag_ir_dr(chip_id,opcode,write_data,tmb_mezzID_length,value);
    idcode[chip_id] = bits_to_int(value,32,0);
  }
  //
  bool testFPGAmezz  = compareValues("FPGA Mezz ID code",idcode[0],0x11050093,true);  
  bool testPROMmezz1 = compareValues("PROM Mezz ID code 1",idcode[1],0x05036093,true);  
  bool testPROMmezz2 = compareValues("PROM Mezz ID code 2",idcode[2],0x05036093,true);  
  bool testPROMmezz3 = compareValues("PROM Mezz ID code 3",idcode[3],0x05036093,true);  
  bool testPROMmezz4 = compareValues("PROM Mezz ID code 4",idcode[4],0x05036093,true);  
  //
  testOK = (testFPGAmezz  &&
	    testPROMmezz1 &&
	    testPROMmezz2 &&
	    testPROMmezz3 &&
	    testPROMmezz4 );
  //
  messageOK("Mezzanine FPGA/PROM ID",testOK);
  //int dummy = sleep(3);
  //
  ResultTestMezzId_ = testOK;
  //
  return testOK;
}


bool TMBTester::testPROMid(){
  (*MyOutput_) << "TMBTester: Checking User PROM ID codes" << std::endl;
  int userID[2];
  bool testOK = false;

  set_jtag_address(TMB_ADR_BOOT);    
  set_jtag_chain(TMB_USER_PROM_CHAIN);         

  // **Take the chosen chain's TAP to RTI (via the chosen address)**
  jtag_anystate_to_rti();    
  
  int opcode = PROM_IDCODE;

  int value[tmb_user_promID_length];

  int write_zeros[MAX_FRAMES] = {};   //create fat 0 for writing to data registers

  for (int chip_id=0; chip_id<=1; chip_id++){
    jtag_ir_dr(chip_id,opcode,write_zeros,tmb_user_promID_length,value);

    userID[chip_id] = bits_to_int(value,32,0);
  }

  bool testPROMUserId0 = compareValues("PROM User ID 0",userID[0],0x05022093,true);  
  bool testPROMUserId1 = compareValues("PROM User ID 1",userID[1],0x05022093,true);  

  testOK = (testPROMUserId0 &&
	    testPROMUserId1 );

  messageOK("PROM User ID",testOK);
  //int dummy = sleep(3);
  //
  ResultTestPromId_ = testOK ;
  //
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
  //int dummy = sleep(3);
  //
  ResultTestPROMPath_ = testOK;
  //
  return testOK;
}


bool TMBTester::testDSN(){
  int dummy;

  (*MyOutput_) << "TMBTester: Checking Digital Serial Numbers for TMB" 
	    << std::endl;
  bool tmbDSN = testDSN(0);
  //dummy=sleep(1);

  (*MyOutput_) << "TMBTester: Checking Digital Serial Numbers for Mezzanine" 
	    << std::endl;
  bool mezzanineDSN = testDSN(1);
  //dummy=sleep(1);

  (*MyOutput_) << "TMBTester: Checking Digital Serial Numbers for RAT" 
	    << std::endl;
  bool ratDSN = testDSN(2);
  //dummy=sleep(1);

  messageOK("TMB DSN",tmbDSN);
  messageOK("Mezzanine DSN",mezzanineDSN);
  messageOK("RAT DSN",ratDSN);

  bool DSNOK = (tmbDSN &&
                mezzanineDSN &&
                ratDSN);

  messageOK("All Digital Serial Numbers",DSNOK);
  //dummy = sleep(3);
  //
  ResultTestDSN_ = DSNOK ;
  //
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
  //int dummy = sleep(3);
  //
  ResultTestADC_ = testOK ;
  //
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
  //int dummy = sleep(3);
  //
  ResultTest3d3444_ = testOK;
  //
  return testOK;
}
/////////////////////////////////////////
// END  TMB Tests:
/////////////////////////////////////////



///////////////////////////////////////////////
//  The following belong in RAT.cc
///////////////////////////////////////////////
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


  (*MyOutput_) << "Performing RAT-TMB delay scan..." << std::endl;;

  //step through ddd_delay
  for (pass=0; pass<=1000; pass++) { //collect statistics

    if ( (pass % 100) == 0 ) 
      (*MyOutput_) << "Pass = " << std::dec << pass << std::endl;

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
    (*MyOutput_) << "    " << std::hex << rpc_delay 
		 << "           " << std::hex << rpc_bad[rpc_delay] 
	      <<std::endl;
  }

  return;
}


void TMBTester::ReadRatUser1(){
  (*MyOutput_) << "TMBTester: Read RAT USER1 JTAG (status register)" << std::endl;

  int dummy;
  int read_data;
  int opcode,chip_id;

  // First make sure the RPC FPGA is finished:
  read_data = tmb_->ReadRegister(rpc_cfg_adr);
  int rpc_done = (read_data >> 14) & 0x1;
  (*MyOutput_) << "RPC done = " << rpc_done << std::endl;

  set_jtag_address(TMB_ADR_BOOT);    
  set_jtag_chain(RAT_CHAIN);         

  // **Take the chosen chain's TAP to RTI (via the chosen address)**
  jtag_anystate_to_rti();    

  const int register_length = 32;  
  int data[register_length];

  int idcode[2];
  int write_data[MAX_FRAMES] = {};   //create fat 0 for writing to data registers

  for (chip_id=0; chip_id<=1; chip_id++){
    if (chip_id == 0) {
      opcode = VTX2_IDCODE;                  
    } else { 
      opcode = PROM_IDCODE;                  
    }
    jtag_ir_dr(chip_id,opcode,write_data,register_length,data);

    idcode[chip_id] = bits_to_int(data,32,0);
  }


  int usercode[2];
  for (chip_id=0; chip_id<=1; chip_id++){
    if (chip_id == 0) {
      opcode = VTX2_USERCODE;                  
    } else { 
      opcode = PROM_USERCODE;                  
    }
    jtag_ir_dr(chip_id,opcode,write_data,register_length,data);

    usercode[chip_id] = bits_to_int(data,32,0);
  }

  chip_id=0;
  opcode = VTX2_USR1;
  int user1_value[rat_user1_length];

  step_mode=false;
  jtag_ir_dr(chip_id,opcode,write_data,rat_user1_length,user1_value);

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

  (*MyOutput_) << "RAT PROM ID code = " << idcode[0] << std::endl;
  (*MyOutput_) << "RAT FPGA ID code = " << idcode[1] << std::endl;
  (*MyOutput_) << "RAT PROM User code = " << usercode[0] << std::endl;
  (*MyOutput_) << "RAT FPGA User code = " << usercode[1] << std::endl;

  (*MyOutput_) << "RAT USER1 = ";
  for (first=0; first<7; first++) 
    (*MyOutput_) << rat_user1[first];
  (*MyOutput_) << std::endl;

  decodeRATUser1(user1_value);

  return;
}


void TMBTester::decodeRATUser1(int * data) {

  // ** parse the bit array from the USER1 data register
  // ** to print it out in a human readable form
  int counter=0;
  int i;

  int rs_begin_array[4];
  for (i=0;i<4;i++) 
    rs_begin_array[i] = data[counter++];
  int rs_begin = bits_to_int(rs_begin_array,4,0);
  (*MyOutput_) << "Begin marker = " << rs_begin << std::endl;

  int rs_version_array[4];
  for (i=0;i<4;i++) 
    rs_version_array[i] = data[counter++];
  int rs_version = bits_to_int(rs_version_array,4,0);
  (*MyOutput_) << "Version ID = " << rs_version << std::endl;

  int rs_monthday_array[16];
  for (i=0;i<16;i++) 
    rs_monthday_array[i] = data[counter++];
  int rs_monthday = bits_to_int(rs_monthday_array,16,0);
  (*MyOutput_) << "Version Month/Day = " << rs_monthday << std::endl;

  int rs_year_array[16];
  for (i=0;i<16;i++) 
    rs_year_array[i] = data[counter++];
  int rs_year = bits_to_int(rs_year_array,16,0);
  (*MyOutput_) << "Version Year = " << rs_year << std::endl;

  int rs_syncmode = data[counter++];
  (*MyOutput_) << "1-> 80MHz sync mode = " << rs_syncmode << std::endl;

  int rs_posneg = data[counter++];
  (*MyOutput_) << "1-> Latch 40MHz RPC data on posedge = " << rs_posneg << std::endl;

  int rs_loop = data[counter++];
  (*MyOutput_) << "1-> Loopback mode = " << rs_loop << std::endl;

  int rs_rpc_en_array[2];
  for (i=0;i<2;i++) 
    rs_rpc_en_array[i] = data[counter++];
  int rs_rpc_en = bits_to_int(rs_rpc_en_array,2,0);
  (*MyOutput_) << "RPC driver enabled = " << rs_rpc_en << std::endl;

  int rs_clk_active_array[2];
  for (i=0;i<2;i++) 
    rs_clk_active_array[i] = data[counter++];
  int rs_clk_active = bits_to_int(rs_clk_active_array,2,0);
  (*MyOutput_) << "RPC direct clock status = " << rs_clk_active << std::endl;

  int rs_locked_tmb = data[counter++];
  (*MyOutput_) << "TMB DLL locked = " << rs_locked_tmb << std::endl;

  int rs_locked_rpc0 = data[counter++];
  (*MyOutput_) << "RPC0 DLL locked = " << rs_locked_rpc0 << std::endl;

  int rs_locked_rpc1 = data[counter++];
  (*MyOutput_) << "RPC1 DLL locked = " << rs_locked_rpc1 << std::endl;

  int rs_locklost_tmb = data[counter++];
  (*MyOutput_) << "TMB DLL lost lock = " << rs_locklost_tmb << std::endl;

  int rs_locklost_rpc0 = data[counter++];
  (*MyOutput_) << "RPC0 DLL lost lock = " << rs_locklost_rpc0 << std::endl;

  int rs_locklost_rpc1 = data[counter++];
  (*MyOutput_) << "RPC1 DLL lost lock = " << rs_locklost_rpc1 << std::endl;

  int rs_txok = data[counter++];
  (*MyOutput_) << "ALCT TX OK = " << rs_txok << std::endl;

  int rs_rxok = data[counter++];
  (*MyOutput_) << "ALCT RX OK = " << rs_rxok << std::endl;

  int rs_ntcrit = data[counter++];
  (*MyOutput_) << "Over Temperature Threshold = " << rs_ntcrit << std::endl;

  int rs_rpc_free = data[counter++];
  (*MyOutput_) << "rpc_free0 from TMB = " << rs_rpc_free << std::endl;

  int rs_dsn = data[counter++];
  (*MyOutput_) << "rpc_dsn to TMB = " << rs_dsn << std::endl;

  int rs_dddoe_wr_array[4];
  for (i=0;i<4;i++) 
    rs_dddoe_wr_array[i] = data[counter++];
  int rs_dddoe_wr = bits_to_int(rs_dddoe_wr_array,4,0);
  (*MyOutput_) << "DDD status:  output enables = " << rs_dddoe_wr << std::endl;

  int rs_ddd_wr_array[16];
  for (i=0;i<16;i++) 
    rs_ddd_wr_array[i] = data[counter++];
  int rs_ddd_wr = bits_to_int(rs_ddd_wr_array,16,0);
  (*MyOutput_) << "DDD status:  delay values = " << rs_ddd_wr << std::endl;

  int rs_ddd_auto = data[counter++];
  (*MyOutput_) << "1-> start DDD on power-up = " << rs_ddd_auto << std::endl;

  int rs_ddd_start = data[counter++];
  (*MyOutput_) << "DDD status: start ddd machine  = " << rs_ddd_start << std::endl;

  int rs_ddd_busy = data[counter++];
  (*MyOutput_) << "DDD status: state machine busy = " << rs_ddd_busy << std::endl;

  int rs_ddd_verify_ok = data[counter++];
  (*MyOutput_) << "DDD status: data readback OK = " << rs_ddd_verify_ok << std::endl;

  int rs_rpc0_parity_ok = data[counter++];
  (*MyOutput_) << "RPC0 parity OK currently = " << rs_rpc0_parity_ok << std::endl;

  int rs_rpc1_parity_ok = data[counter++];
  (*MyOutput_) << "RPC1 parity OK currently = " << rs_rpc1_parity_ok << std::endl;

  int rs_rpc0_cnt_perr_array[16];
  for (i=0;i<16;i++) 
    rs_rpc0_cnt_perr_array[i] = data[counter++];
  int rs_rpc0_cnt_perr = bits_to_int(rs_rpc0_cnt_perr_array,16,0);
  (*MyOutput_) << "RPC0 parity error counter = " << rs_rpc0_cnt_perr << std::endl;

  int rs_rpc1_cnt_perr_array[16];
  for (i=0;i<16;i++) 
    rs_rpc1_cnt_perr_array[i] = data[counter++];
  int rs_rpc1_cnt_perr = bits_to_int(rs_rpc1_cnt_perr_array,16,0);
  (*MyOutput_) << "RPC1 parity error counter = " << rs_rpc1_cnt_perr << std::endl;

  int rs_last_opcode_array[5];
  for (i=0;i<5;i++) 
    rs_last_opcode_array[i] = data[counter++];
  int rs_last_opcode = bits_to_int(rs_last_opcode_array,5,0);
  (*MyOutput_) << "Last firmware TAP cmd opcode = " << rs_last_opcode << std::endl;


  int rw_rpc_en_array[2];
  for (i=0;i<2;i++) 
    rw_rpc_en_array[i] = data[counter++];
  int rw_rpc_en = bits_to_int(rw_rpc_en_array,2,0);
  (*MyOutput_) << "rw_rpc_en = " << rw_rpc_en << std::endl;

  int rw_ddd_start = data[counter++];
  (*MyOutput_) << "rw_ddd_start  = " << rw_ddd_start << std::endl;

  int rw_ddd_wr_array[16];
  for (i=0;i<16;i++) 
    rw_ddd_wr_array[i] = data[counter++];
  int rw_ddd_wr = bits_to_int(rw_ddd_wr_array,16,0);
  (*MyOutput_) << "rw_ddd_wr = " << rw_ddd_wr << std::endl;

  int rw_dddoe_wr_array[4];
  for (i=0;i<4;i++) 
    rw_dddoe_wr_array[i] = data[counter++];
  int rw_dddoe_wr = bits_to_int(rw_dddoe_wr_array,4,0);
  (*MyOutput_) << "rw_dddoe_wr = " << rw_dddoe_wr << std::endl;

  int rw_perr_reset = data[counter++];
  (*MyOutput_) << "rw_perr_reset = " << rw_perr_reset << std::endl;

  int rw_parity_odd = data[counter++];
  (*MyOutput_) << "rw_parity_odd = " << rw_parity_odd << std::endl;

  int rw_perr_ignore = data[counter++];
  (*MyOutput_) << "rw_perr_ignore = " << rw_perr_ignore << std::endl;

  int rw_rpc_future_array[6];
  for (i=0;i<6;i++) 
    rw_rpc_future_array[i] = data[counter++];
  int rw_rpc_future = bits_to_int(rw_rpc_future_array,6,0);
  (*MyOutput_) << "rw_rpc_future = " << rw_rpc_future << std::endl;


  int rs_rpc0_pdata_array[19];
  for (i=0;i<19;i++) 
    rs_rpc0_pdata_array[i] = data[counter++];
  int rs_rpc0_pdata = bits_to_int(rs_rpc0_pdata_array,19,0);
  (*MyOutput_) << "RPC0 data (includes 16 pad bits + 3bxn) = " << rs_rpc0_pdata << std::endl;

  int rs_rpc1_pdata_array[19];
  for (i=0;i<19;i++) 
    rs_rpc1_pdata_array[i] = data[counter++];
  int rs_rpc1_pdata = bits_to_int(rs_rpc1_pdata_array,19,0);
  (*MyOutput_) << "RPC1 data (includes 16 pad bits + 3bxn) = " << rs_rpc1_pdata << std::endl;

  int rs_unused_array[29];
  for (i=0;i<29;i++) 
    rs_unused_array[i] = data[counter++];
  int rs_unused = bits_to_int(rs_unused_array,29,0);
  (*MyOutput_) << "Unused bits = " << rs_unused << std::endl;

  int rs_end_array[4];
  for (i=0;i<4;i++) 
    rs_end_array[i] = data[counter++];
  int rs_end = bits_to_int(rs_end_array,4,0);
  (*MyOutput_) << "End marker = " << rs_end << std::endl;

return;
}


int TMBTester::ReadRatUser2(){
  (*MyOutput_) << "TMBTester: Read RAT USER2 JTAG (control register)" << std::endl;

  int dummy;
  int read_data;
  int opcode,chip_id;

  // First make sure the RPC FPGA is finished:
  read_data = tmb_->ReadRegister(rpc_cfg_adr);
  int rpc_done = (read_data >> 14) & 0x1;
  (*MyOutput_) << "RPC done = " << rpc_done << std::endl;

  set_jtag_address(TMB_ADR_BOOT);    
  set_jtag_chain(RAT_CHAIN);         

  // **Take the chosen chain's TAP to RTI (via the chosen address)**
  step_mode = false;
  jtag_anystate_to_rti();    

  const int register_length = 32;  
  int data[register_length];

  int idcode[2];

  int write_zeros[MAX_FRAMES] = {};   //create fat 0 for writing to data registers

  chip_id=0;
  opcode = VTX2_USR2;
  int user2_value[rat_user2_length];

  jtag_ir_dr(chip_id,opcode,write_zeros,rat_user2_length,user2_value);

  int rat_user2 = bits_to_int(user2_value,rat_user2_length,0);

  (*MyOutput_) << "RAT USER2 = " << std::hex << rat_user2 << std::endl;

  //Decode the information to write back to the RAT:
  int i;
  int counter = 0;

  int ws_rpc_en_array[2];
  for (i=0;i<2;i++) 
    ws_rpc_en_array[i] = user2_value[counter++];
  int ws_rpc_en = bits_to_int(ws_rpc_en_array,2,0);

  int ws_ddd_start = user2_value[counter++];

  int ws_ddd_wr_array[16];
  for (i=0;i<16;i++) 
    ws_ddd_wr_array[i] = user2_value[counter++];
  int ws_ddd_wr = bits_to_int(ws_ddd_wr_array,16,0);

  int ws_dddoe_wr_array[4];
  for (i=0;i<4;i++) 
    ws_dddoe_wr_array[i] = user2_value[counter++];
  int ws_dddoe_wr = bits_to_int(ws_dddoe_wr_array,4,0);

  int ws_perr_reset = user2_value[counter++];

  int ws_parity_odd = user2_value[counter++];

  int ws_perr_ignore = user2_value[counter++];

  int ws_rpc_future_array[6];
  for (i=0;i<6;i++) 
    ws_rpc_future_array[i] = user2_value[counter++];
  int ws_rpc_future = bits_to_int(ws_rpc_future_array,6,0);

  //Put data back into User2 (readout was destructive)
  int rsd[rat_user2_length];
  bit_to_array(rat_user2,rsd,rat_user2_length);

  chip_id=0;
  opcode = VTX2_USR2;
  jtag_ir_dr(chip_id,opcode,rsd,rat_user2_length,user2_value);

  (*MyOutput_) << "ws_rpc_en = " << ws_rpc_en << std::endl;
  (*MyOutput_) << "ws_ddd_start  = " << ws_ddd_start << std::endl;
  (*MyOutput_) << "ws_ddd_wr = " << ws_ddd_wr << std::endl;
  (*MyOutput_) << "ws_dddoe_wr = " << ws_dddoe_wr << std::endl;
  (*MyOutput_) << "ws_perr_reset = " << ws_perr_reset << std::endl;
  (*MyOutput_) << "ws_parity_odd = " << ws_parity_odd << std::endl;
  (*MyOutput_) << "ws_perr_ignore = " << ws_perr_ignore << std::endl;
  (*MyOutput_) << "ws_rpc_future = " << ws_rpc_future << std::endl;

  return rat_user2;
}

int TMBTester::read_rat_delay(){

  int write_zeros[MAX_FRAMES] = {};   //create fat 0 for writing to data registers

  //set up your jtag stuff...
  set_jtag_address(TMB_ADR_BOOT);    
  set_jtag_chain(RAT_CHAIN);         

  jtag_anystate_to_rti();    

  int chip_id=0;
  int opcode = VTX2_USR1;
  int user1_value[rat_user1_length];

  jtag_ir_dr(chip_id,opcode,write_zeros,rat_user1_length,user1_value);

  int i;

  int delays[16];
  int offset = 62;
  int bit = 0;
  for (i=offset; i<(offset+16); i++) 
    delays[bit++] = user1_value[i];

  int delay = bits_to_int(delays,16,0);

  int ddd_delay[4];
  for (i=0; i<4; i++) {
    ddd_delay[i] = (delay >> i*4) & 0xf;
    (*MyOutput_) << "DDD delay " << i << " = " << std::hex << ddd_delay[i] << std::endl;
  }

  return delay;
}

void TMBTester::RpcRatDelayScan(int rpc) {

  if ( rpc==0 || rpc==1 ) {
    (*MyOutput_) << "RPC " << rpc << " - RAT delay scan using parity counter" << std::endl;
  } else {
    (*MyOutput_) << "RPC " << rpc << " does not exist" << std::endl;
    return;
  }

  int dummy;
  int delay;

  int parity_counter;

  for (delay = 1; delay<16; delay++) {  //steps of 2ns
    (*MyOutput_) << "set delay = " << delay 
		 << " for RPC " << rpc
		 << std::endl;
    set_rat_delay(rpc,delay);
    dummy = read_rat_delay();

    //    parity_counter = read_rpc_parity_error_counter(rpc);

    //    (*MyOutput_) << "Before reset:  parity error counter = " << parity_counter << std::endl;
    
    //    reset_parity_error_counter(rpc);

    //    parity_counter = read_rpc_parity_error_counter(rpc);
    dummy = read_rpc_data(rpc);

    (*MyOutput_) << "After reset:  parity error counter = " << parity_counter << std::endl;

    dummy = sleep(2);
  }

  return;
}

void TMBTester::reset_parity_error_counter(int rpc) {

  int write_zeros[MAX_FRAMES] = {};
  int i;

  //set up your jtag stuff...
  set_jtag_address(TMB_ADR_BOOT);    
  set_jtag_chain(RAT_CHAIN);         

  jtag_anystate_to_rti();    //bring the state machine to RTI

  int chip_id=0;
  int opcode = VTX2_USR2;
  int user2_value[rat_user2_length];

  jtag_ir_dr(chip_id,opcode,write_zeros,rat_user2_length,user2_value);

  int rsd[rat_user2_length];
  int bitcount = 0;

  // set the new RAT USER2 bits.  The USER2 read is destructive, so the data have to be put back:
  for (i=0; i<rat_user2_length; i++) {
    rsd[i]=user2_value[i];
    if (i==23) 
      rsd[i] = 1;                            // assert parity reset
  }

  jtag_ir_dr(chip_id,opcode,rsd,rat_user2_length,user2_value);

  rsd[23] = 0;                               // de-assert parity reset

  jtag_ir_dr(chip_id,opcode,rsd,rat_user2_length,user2_value);

  return;
}

int TMBTester::read_rpc_parity_error_counter(int rpc) {
  int parity_counter = -1;

  int write_zeros[MAX_FRAMES] = {};
  int i;

  //set up your jtag stuff...
  set_jtag_address(TMB_ADR_BOOT);    
  set_jtag_chain(RAT_CHAIN);         

  jtag_anystate_to_rti();    //bring the state machine to RTI

  int chip_id=0;
  int opcode = VTX2_USR1;
  int user1_value[rat_user1_length];

  jtag_ir_dr(chip_id,opcode,write_zeros,rat_user1_length,user1_value);

  int counter_array[16];
  for (i=0;i<16;i++) 
    counter_array[i] = user1_value[84+16*rpc+i];
  parity_counter = bits_to_int(counter_array,16,0);

  (*MyOutput_) << "RPC " << rpc << " Parity error counter = " << parity_counter << std::endl;  

  return parity_counter;
}

int TMBTester::read_rpc_data(int rpc) {
  int rpc_data = -1;

  int write_zeros[MAX_FRAMES] = {};
  int i;

  //set up your jtag stuff...
  set_jtag_address(TMB_ADR_BOOT);    
  set_jtag_chain(RAT_CHAIN);         

  jtag_anystate_to_rti();    //bring the state machine to RTI

  int chip_id=0;
  int opcode = VTX2_USR1;
  int user1_value[rat_user1_length];

  jtag_ir_dr(chip_id,opcode,write_zeros,rat_user1_length,user1_value);

  int data_array[19];
  for (i=0;i<19;i++) 
    data_array[i] = user1_value[153+19*rpc+i];
  rpc_data = bits_to_int(data_array,19,0);

  (*MyOutput_) << "RPC " << rpc << " Data = " << rpc_data << std::endl;

  return rpc_data;
}

int TMBTester::read_rpc_parity_ok(int rpc) {
  int parity_ok = -1;

  int write_zeros[MAX_FRAMES] = {};
  int i;

  //set up your jtag stuff...
  set_jtag_address(TMB_ADR_BOOT);    
  set_jtag_chain(RAT_CHAIN);         

  jtag_anystate_to_rti();    //bring the state machine to RTI

  int chip_id=0;
  int opcode = VTX2_USR1;
  int user1_value[rat_user1_length];

  jtag_ir_dr(chip_id,opcode,write_zeros,rat_user1_length,user1_value);

  parity_ok = user1_value[82+rpc];

  (*MyOutput_) << "RPC " << rpc << " Parity OK = " << parity_ok << std::endl;  

  return parity_ok;
}


void TMBTester::set_rat_delay(int rpc,int delay) {

  if ( rpc==0 || rpc==1 ) {
    if (delay >= 0 && delay <=15 ) {
      (*MyOutput_) << "Set RPC " << rpc << " delay = " << std::hex << delay << std::endl;
    } else {
      (*MyOutput_) << "only delay values 0-15 allowed" << std::endl;      
      return;
    }
  } else {
    (*MyOutput_) << "RPC " << rpc << " does not exist" << std::endl;
    return;
  }

  int write_zeros[MAX_FRAMES] = {};   //create fat 0 for writing to data registers
  int dummy;

  //set up your jtag stuff...
  set_jtag_address(TMB_ADR_BOOT);    
  set_jtag_chain(RAT_CHAIN);         

  jtag_anystate_to_rti();    

  int chip_id=0;
  int opcode = VTX2_USR2;
  int user2_value[rat_user2_length];

  jtag_ir_dr(chip_id,opcode,write_zeros,rat_user2_length,user2_value);

  delay &= 0x000f;     //strip off extraneous bits
  delay <<= rpc*4;     // put it in the right slots for the JTAG writing...

  int rsd[rat_user2_length];
  int bitcount = 0;

  // set the new RAT USER2 bits.  The USER2 read is destructive, so the data have to be put back:
  for (int i=0; i<rat_user2_length; i++) {
    rsd[i]=user2_value[i];
    if (i==2) 
      rsd[i] = 0;                            // unstart state machine
    if (i>2 && i<19 ) {
      rsd[i] = (delay >> bitcount++) & 0x1;  // new delay values
    }
  }

  jtag_ir_dr(chip_id,opcode,rsd,rat_user2_length,user2_value);


  //Check that the DDD state machine went idle...
  chip_id=0;
  opcode = VTX2_USR1;
  int user1_value[rat_user1_length];

  jtag_ir_dr(chip_id,opcode,write_zeros,rat_user1_length,user1_value);

  int ddd_busy = user1_value[80] & 0x1;

  if (ddd_busy != 0) {
    (*MyOutput_) << "ERROR: State machine stuck busy" << std::endl;
    dummy = sleep(10);
  }


  // Start DDD state machine...
  chip_id=0;
  opcode = VTX2_USR2;
  rsd[2] = 0x1;                         //keep all the other tdi data the same...

  jtag_ir_dr(chip_id,opcode,rsd,rat_user2_length,user2_value);


  //Check that the DDD state machine went busy...
  chip_id=0;
  opcode = VTX2_USR1;

  jtag_ir_dr(chip_id,opcode,write_zeros,rat_user1_length,user1_value);

  ddd_busy = user1_value[80] & 0x1;

  if (ddd_busy != 1) {
    (*MyOutput_) << "ERROR: State machine did not go busy" << std::endl;
    dummy = sleep(10);
  }


  // Unstart DDD state machine again...
  chip_id=0;
  opcode = VTX2_USR2;
  rsd[2] = 0x0;

  jtag_ir_dr(chip_id,opcode,rsd,rat_user2_length,user2_value);


  //Check that the DDD state machine is not busy...
  chip_id=0;
  opcode = VTX2_USR1;

  jtag_ir_dr(chip_id,opcode,write_zeros,rat_user1_length,user1_value);

  ddd_busy = user1_value[80] & 0x1;

  if (ddd_busy != 0) {
    (*MyOutput_) << "ERROR: State machine is busy" << std::endl;
    dummy = sleep(10);
  }

  //Check that the data made it OK...
  int ddd_verify = user1_value[81] & 0x1;
  if (ddd_verify != 1) {
    (*MyOutput_) << "ERROR: DDD data not verified" << std::endl;
    dummy = sleep(10);
  }

  return;
}
///////////////////////////////////////////////
//  END  The following belong in RAT.cc
///////////////////////////////////////////////


/////////////////////////////////////////
// Functions for JTAG:
/////////////////////////////////////////
void TMBTester::set_jtag_chain(int chain) { 

  jtag_chain = chain;
  select_jtag_chain_param();

  return;
}

void TMBTester::select_jtag_chain_param(){
  //select:
  //  - how many chips are on this chain
  //  - how many bits per opcode (per chip)

  //Chains currently not supported by this member:
  //    ALCT_SLOW_USER_CHAIN
  //    ALCT_SLOW_PROM_CHAIN
  //    TMB_USER_PROM_CHAIN  

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

void TMBTester::jtag_ir_dr(int chip_id, 
			   int opcode,
			   int * write_data,
			   int length, 
			   int *read_data) {


  // JTAG instructions and data are loaded for PROM1 first and PROM0 last, i.e.:
  //    JTAG position     Chip ID   Order  Load Name   TMB Name     RAT
  //    -------------     -------   -----  ---------   --------    ------
  //        tdi>0            0                 1        PROM0       FPGA
  //        tdo<1            1                 0        PROM1       PROM
  
  (*MyOutput_) << "TMBTester::JTAG write Instruction register opcode " << std::hex << opcode 
	       << " to JTAG address " << std::hex << jtag_address
	       << " for chip " << std::hex <<chip_id 
	       << std::endl;
  int k;
  int     iframe;                                               //JTAG frame counter
  int tdi[MAX_FRAMES], tms[MAX_FRAMES], tdo[MAX_FRAMES];

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
	bit = (opcode >> ibit) & 0x1;                      //extract bit from opcode
      }
      tms[iframe]=0;                                       // TAP stays in ShfIR
      tdi[iframe++]=bit;                                   // Instruction bit, advance the frame
    }
  }

  // ** Put TAP from ShfIR to Ex1IR ** 
  tms[iframe-1]=1;                                         // Done during the last bit of the opcode

  // ** Put TAP from Ex1IR to RTI **
  for (k = 0; k < 2; k++) {
    tms[iframe] = tms_post_opcode[k];
    tdi[iframe++] = tdi_post_opcode[k];
  }

  //std::cout << "Step 1 " << std::endl ;

  if (iframe > MAX_FRAMES) {
    (*MyOutput_) << "do_jtag IR ERROR: Too many frames -> " << iframe << std::endl;
    (*MyOutput_) << "STOP the program (ctrl-c)...";
    std::cin >> k;
  }

  jtag_io_byte(iframe,tms,tdi,tdo);   

  // **Check that the TDO shifted out instruction register strings**
  int iopbit;
  if ( jtag_chain == TMB_USER_PROM_CHAIN ||
       jtag_chain == TMB_FPGA_CHAIN ) {
    iframe = 4;                                    //start at first opcode bit (counting from 0)

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
    for (k=11; k>=4; k--) {
      (*MyOutput_) << tdo[k] << " ";
    }
    (*MyOutput_) << std::endl;
  }

  // ** Perform the second JTAG operation, which is to read the 
  //    TDO data which ought to be "length" bits long, into the
  //    array "read_data".  Include the possibility to shift in "write_data"
  //    on the tdi line, for destructive ShfDR.

  iframe = 0;

  //std::cout << "Step 2 " << std::endl ;

  // Put TAP in state ShfDR from RTI 
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
      for (ibit=0; ibit<length; ibit++) {                  //up to the number of bits specified for this register
	tms[iframe] = 0;                                   // Stay in ShfDR
	tdi[iframe++] = write_data[ibit];                  // Shift in the data for TDI
      }
    } else {                                                 // bypass register is one frame      
      tms[iframe] = 0;                                       // TAP stays in ShfDR
      tdi[iframe++] = 0;                                     // No data goes out to bypass regs
    }
  }

  tms[iframe-1] = 1;                                         // Last opcode, TAP goes to Ex1DR
  
  // ** Put TAP from Ex1IR to RTI **
  for (k = 0; k < 2; k++) {
    tms[iframe] = tms_post_read[k];
    tdi[iframe++] = tdi_post_read[k];
  }

  if (iframe > MAX_FRAMES) {
    (*MyOutput_) << "do_jtag DR ERROR: Too many frames -> " << iframe << std::endl;
    (*MyOutput_) << "STOP the program (ctrl-c)...";
    std::cin >> k;
  }

  jtag_io_byte(iframe,tms,tdi,tdo);

  // ** copy relevant section of tdo to caller's read array **
  for (ibit=0; ibit<length; ibit++) {
    iframe = ibit+offset;
    if (iframe > MAX_FRAMES) {
      (*MyOutput_) << "do_jtag copy ERROR: Too many frames -> " << iframe << std::endl;
      (*MyOutput_) << "STOP the program (ctrl-c)...";
      std::cin >> k;
    }
    read_data[ibit] = tdo[iframe];
    if (jtag_chain == ALCT_MEZZ_USER_CHAIN) read_data[ibit] = tdo[iframe-1];  //ALCT fix (Jonathan)
  }

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
  //	tck	=	toggled by this routine

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

  int i;

  if (nframes>0) {                       //no frames to send

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

      if (step_mode) {
	  step(iframe,tck_bit,tms_bit,tdi_bit,tdo_bit);
      }

      tck_bit = 0x1 << 2;  //Take TCK high, leave tms,tdi as they were
      jtag_out |= tck_bit | tms_bit | tdi_bit;

      dummy = tmb_->tmb_set_boot_reg(jtag_out);    //write boot register
      dummy = tmb_->tmb_get_boot_reg(&jtag_in);    //read boot register
      //** here is the real tdo, read on the rising edge **
      tdo[iframe] = (jtag_in >> 15) & 0x1;         //extract tdo bit, mask lsb
      tdo_bit = tdo[iframe];

      if (step_mode) {
      	step(iframe,tck_bit,tms_bit,tdi_bit,tdo_bit);
      }      
    }
  }
  //** put JTAG bits into an idle state **
  jtag_out &= 0xfffb;     //Take TCK low, leave others as they were
  dummy = tmb_->tmb_set_boot_reg(jtag_out);        //write boot register

  return;
}

void TMBTester::step(int frame,
		     int tck, 
		     int tms, 
		     int tdi, 
		     int tdo){

  (*MyOutput_) << "frame = " << std::hex << frame
	       << " tck = " << std::hex << tck
	    << " tms = " << std::hex << tms
	    << " tdi = " << std::hex << tdi
	    << " tdo = " << std::hex << tdo
	    << " pause.... enter any number, then return...";
  int dummy;
  std::cin >> dummy;

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
void TMBTester::SetExtCLCTPatternTrigger() {
  // enable the sequencer trigger to begin with the 
  // CLCT external (scintillator) trigger (from the CCB)

  int value = tmb_->ReadRegister(seq_trig_en_adr);
  value |= 0x0020;                 
  tmb_->WriteRegister(seq_trig_en_adr,value);

  return;
}

//////////////////////////////////////////
// END: The following should be in TMB: //
//////////////////////////////////////////
