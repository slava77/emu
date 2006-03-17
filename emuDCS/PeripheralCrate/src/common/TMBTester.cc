#include<iostream>
#include<stdio.h>
#include<string>
#include<unistd.h> // for sleep()
#include <stdlib.h>
#include "TMBTester.h"
#include "TMB.h"
#include "CCB.h"
#include "EMUjtag.h"
#include "TMB_constants.h"

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
  rat_ = 0;
  EMUjtag_= 0;
  MyOutput_ = & std::cout;
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

  int dummy;
  bool AllOK;

  //reset();
  //  readreg4();

  bool bootRegOK = testBootRegister();
  dummy = sleep(1);
  bool VMEfpgaDataRegOK = testVMEfpgaDataRegister();
  dummy = sleep(1);
  bool DateOK = testFirmwareDate();
  dummy = sleep(1);
  bool TypeOK = testFirmwareType();
  dummy = sleep(1);
  bool VersionOK = testFirmwareVersion();
  dummy = sleep(1);
  bool RevCodeOK = testFirmwareRevCode();
  dummy = sleep(1);
  bool MezzIdOK = testMezzId();
  dummy = sleep(1);
  bool PROMidOK = testPROMid();
  dummy = sleep(1);
  bool PROMpathOK = testPROMpath();
  dummy = sleep(1);
  bool dsnOK = testDSN();
  dummy = sleep(1);
  bool adcOK = testADC();
  dummy = sleep(1);
  bool is3d3444OK = test3d3444();
  dummy = sleep(1);
  bool isALCTtxrxOK = testALCTtxrx();
  dummy = sleep(1);
  bool isRATtemperOK = testRATtemper();
  dummy = sleep(1);
  bool isRATidCodesOK = testRATidCodes();
  dummy = sleep(1);
  bool isRATuserCodesOK = testRATuserCodes();

  //  jtag_src_boot_reg();

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
  messageOK("ALCT tx/rx cables (RAT)... ",isALCTtxrxOK);
  messageOK("RAT temperature........... ",isRATtemperOK);
  messageOK("RAT ID codes.............. ",isRATidCodesOK);
  messageOK("RAT User codes............ ",isRATuserCodesOK);

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
           is3d3444OK &&
           isALCTtxrxOK &&
	   isRATtemperOK &&
	   isRATidCodesOK &&
	   isRATuserCodesOK);

  messageOK("TMB/RAT internal tests.... ",AllOK);

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

  // walk through the 16 bits on the register, except the JTAG bits...
  for (int ibit=3; ibit<=14; ibit++) {
    write_data = 0x1 << ibit;
    dummy = tmb_->tmb_set_boot_reg(write_data);

    dummy = tmb_->tmb_get_boot_reg(&read_data);

    // Remove the read-only bits:
    read_data &= 0x3FFF;
    write_data &= 0x3FFF;
    
    if ( !compareValues("bootreg bit",read_data,write_data,true) ) {
      err_reg++;
    }
    // Need to pause in between to allow for reloading after hard resets 
    // to various item...
    ::sleep(1);
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
    //    std::cout << "Looping " << std::endl ;
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
  (*MyOutput_) << 
    "TMBTester::testFirmwareSlot() NOT YET IMPLEMENTED" 
     << std::endl;

  bool testOK = false;  
  return testOK;
}

bool TMBTester::testFirmwareDate() {
  (*MyOutput_) << "TMBTester: testing Firmware date" << std::endl;

  int firmwareData = tmb_->FirmwareDate();
  int day = firmwareData & 0xff;
  int month = (firmwareData>>8) & 0xff;
  int year=tmb_->FirmwareYear();

  (*MyOutput_) << "Firmware day.month.year = " << std::hex 
	       << day << "." << month << "." << year << std::endl;

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
  int idcode[5];
  bool testOK = false;

  EMUjtag_->setup_jtag(ChainTmbMezz);

  EMUjtag_->ShfIR_ShfDR(ChipLocationTmbMezzFpga,
			VTX2_IDCODE,
			RegSizeTmbMezzFpga_VTX2_IDCODE);
  int fpgaIdCode = tmb_->bits_to_int(EMUjtag_->GetDRtdo(),EMUjtag_->GetRegLength(),0);

  for (int chip_location=1; chip_location<=4; chip_location++){
    EMUjtag_->ShfIR_ShfDR(chip_location,
			  PROM_IDCODE,
			  RegSizeTmbMezzProm_PROM_IDCODE);

    idcode[chip_location] = tmb_->bits_to_int(EMUjtag_->GetDRtdo(),EMUjtag_->GetRegLength(),0);
  }
  //
  bool testFPGAmezz  = compareValues("FPGA Mezz ID code",fpgaIdCode,0x11050093,true);  
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

  EMUjtag_->setup_jtag(ChainTmbUser);

  for (int chip_location=0; chip_location<=1; chip_location++){
    EMUjtag_->ShfIR_ShfDR(chip_location,
			  PROM_IDCODE,
			  RegSizeTmbUserProm_PROM_IDCODE);
    userID[chip_location] = tmb_->bits_to_int(EMUjtag_->GetDRtdo(),EMUjtag_->GetRegLength(),0);
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
  //BoardType = 0 = TMB
  //          = 1 = Mezzanine
  //          = 2 = RAT

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

  //  float atol = 0.16;
  //  bool a5p0OK     = compareValues("+5.0A TMB      ",a5p0    ,0.245,atol);
  //  bool a3p3OK     = compareValues("+3.3A TMB      ",a3p3    ,1.260,atol);
  //  bool a1p5coreOK = compareValues("+1.5A TMB Core ",a1p5core,0.095,atol);
  //  bool a1p5ttOK   = compareValues("+1.5A TT       ",a1p5tt  ,0.030,atol*1.5);
  //  bool a1p8ratOK  = compareValues("+1.8A RAT Core ",a1p8rat ,0.030,atol*5.0);

  (*MyOutput_) << "+5.0V TMB Current      = " << std::dec << a5p0 << " A" << std::endl;
  (*MyOutput_) << "+3.3V TMB Current      = " << std::dec << a3p3 << " A" << std::endl;
  (*MyOutput_) << "+1.5V TMB core Current = " << std::dec << a1p5core << " A" << std::endl;
  (*MyOutput_) << "+1.5V TMB TT Current   = " << std::dec << a1p5tt << " A" << std::endl;
  (*MyOutput_) << "+1.8V RAT core Current = " << std::dec << a1p8rat << " A" << std::endl;

  float total_power = 
    v5p0*a5p0 +
    v3p3*a3p3 +
    v1p5core*a1p5core +
    v1p5tt*a1p5tt +
    v1p8rat*a1p8rat;
  
  (*MyOutput_) << "Total power = " << std::dec << total_power << " Watts" << std::endl;

  // ** Get TMB and RAT temperatures **
  //GREG  int TMBtempPCB = tmb_->ReadTMBtempPCB();  
  int TMBtempPCB = ReadTMBtempPCB();  
  //GREG  int TMBtempFPGA = tmb_->ReadTMBtempFPGA();  
  int TMBtempFPGA = ReadTMBtempFPGA();  
  //GREG  int RATtempPCB = rat_->ReadRATtempPCB(); 
  int RATtempPCB = ReadRATtempPCB(); 
  //GREG  int RATtempHeatSink = rat_->ReadRATtempHSink(); 
  int RATtempHeatSink = ReadRATtempHSink(); 

  // ** Get TMB and RAT critical temperature settings **
  //GREG  int TMBtCritPCB = tmb_->ReadTMBtCritPCB();  
  int TMBtCritPCB = ReadTMBtCritPCB();  
  //GREG  int TMBtCritFPGA = tmb_->ReadTMBtCritFPGA();  
  int TMBtCritFPGA = ReadTMBtCritFPGA();  
  //GREG  int RATtCritPCB = rat_->ReadRATtCritPCB(); 
  int RATtCritPCB = ReadRATtCritPCB(); 
  //GREGint RATtCritHeatSink = rat_->ReadRATtCritHSink(); 
  int RATtCritHeatSink = ReadRATtCritHSink(); 

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
//
bool TMBTester::testALCTtxrx(){
  (*MyOutput_) << "TMBTester: Testing ALCT tx/rx cables connection to RAT" << std::endl;
  bool testOK = false;

  //GREG  rat_->ReadRatUser1();
  ReadRatUser1();
  //GREGint * user1 = rat_->GetRatUser1();
  int * user1 = GetRatUser1();

  int txval = user1[53];
  int rxval = user1[54];

  bool testTx = compareValues("ALCT tx",txval,0x1,true);
  bool testRx = compareValues("ALCT rx",rxval,0x1,true);

  testOK = (testTx &&
	    testRx );

  messageOK("ALCT tx/rx cables plugged in",testOK);

  ResultTestALCTtxrx_ = testOK ;
  return testOK;
}

bool TMBTester::testRATtemper(){
  (*MyOutput_) << "TMBTester: Testing RAT temperature threshold bit" << std::endl;
  bool testOK = false;

  //GREG  rat_->ReadRatUser1();
  ReadRatUser1();
  //GREGint * user1 = rat_->GetRatUser1();
  int * user1 = GetRatUser1();

  int temperval = user1[55];

  testOK = compareValues("RAT Temperature bit",temperval,0x1,true);

  messageOK("RAT Temperature bit",testOK);

  ResultTestRATtemper_ = testOK ;
  return testOK;
}

bool TMBTester::testRATidCodes(){
  (*MyOutput_) << "TMBTester: Testing RAT ID codes" << std::endl;
  bool testOK = false;

  //GREG  rat_->ReadRatIdCode();
  ReadRatIdCode();
  //GREG int * idcodes = rat_->GetRatIdCode();
  int * idcodes = GetRatIdCode();

  bool FPGAidOK = compareValues("RAT FPGA ID code",idcodes[0],0x20a10093,true);
  bool PROMidOK = compareValues("RAT PROM ID code",idcodes[1],0x05024093,true);

  testOK = (FPGAidOK &&
	    PROMidOK );

  messageOK("RAT ID codes",testOK);

  ResultTestRATidCodes_ = testOK ;
  return testOK;
}

bool TMBTester::testRATuserCodes(){
  (*MyOutput_) << "TMBTester: Testing RAT User codes" << std::endl;
  bool testOK = false;

  //GREG  rat_->ReadRatUserCode();
  ReadRatUserCode();
  //GREGint * usercodes = rat_->GetRatUserCode();
  int * usercodes = GetRatUserCode();

  int fpgauser = usercodes[0] & 0xffffffff;
  int promuser = usercodes[1] & 0xffffffff;

  bool FPGAuserOK = compareValues("RAT FPGA user code",fpgauser,0x02232006,true);
  bool PROMuserOK = compareValues("RAT PROM user code",promuser,0x02232006,true);

  testOK = (FPGAuserOK &&
	    PROMuserOK );

  messageOK("RAT User codes",testOK);

  ResultTestRATuserCodes_ = testOK ;
  return testOK;
}
/////////////////////////////////////////
// END  TMB Tests:
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

  //  Calculate CRC x**8 + x**5 + X**4 +1
  //  for 7-byte Dallas Semi i-button data

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


void TMBTester::jtag_src_boot_reg() {
  (*MyOutput_) << "TMBTester: Force JTAG to go through boot reg" << std::endl;
  unsigned short int BootData, read_data;
  int dummy = tmb_->tmb_get_boot_reg(&BootData);
  (*MyOutput_) << "Initial boot contents = " << std::hex << BootData << std::endl;
  BootData |= 0x0080;
  
  dummy = tmb_->tmb_set_boot_reg(BootData);
  dummy = tmb_->tmb_get_boot_reg(&read_data);   //check for FPGA final state
  (*MyOutput_) << "Final Boot Contents = " << std::hex << read_data << std::endl;    
  return;
}

//////////////////////////////////////////////
// END: Functions needed to implement tests..
//////////////////////////////////////////////


/////////////////////////////////////////
// Some useful methods:
/////////////////////////////////////////
void TMBTester::RpcComputeParity(int rpc) {

  //routine to determine if the rpc_parity_ok_[] bits are correlated with the 
  //number of 1's in the rpc[0:15]+bxn[0:2] data word....
  //   CONCLUSION:  YES.                G. Rakness  7 Feb. 2006

  //Put RAT into correct mode [0]=sync-mode -> sends a fixed data pattern to TMB to be analyzed
  //                          [1]=posneg    -> inserts 12.5ns (1/2-cycle) delay in RPC data path
  //                                           to improve syncing to rising edge of TMB clock
  //                          [2]=loop_tmb
  //                          [3]=free_tx0
  int write_data = 0x0002;
  tmb_->WriteRegister(vme_ratctrl_adr,write_data);

  // find out if the parity error counter is using even or odd parity...
  //GREG  rat_->ReadRpcParity();
  ReadRpcParity();
  //GREG  int parity_used = rat_->GetRpcParityUsed();   // = 1 if odd, 0 if even
  int parity_used = GetRpcParityUsed();   // = 1 if odd, 0 if even

  int data,parity_ok;
  int counter,bxn;
  int evenodd;
  int total_counts[8] = {};
  int parity_match[8] = {};

  for (int event=0;event<100;event++) {
    // get data and parity_ok for this RPC event:
    //GREG    rat_->read_rpc_data();
    read_rpc_data();
    //GREGdata = rat_->GetRpcData(rpc);
    data = GetRpcData(rpc);
    //GREG    parity_ok = rat_->GetRpcParityOK(rpc);
    parity_ok = GetRpcParityOK(rpc);

    // count number of 1's
    counter = 0;
    for (int bit=0;bit<19;bit++) {
      counter += (data >> bit) & 0x1;
    }
	
    bxn = (data >> 16) & 0x7;         //here is the bunch crossing associated with this event
    total_counts[bxn]++;              //total number of events analyzed for this bunch crossing

    evenodd = counter % 2;            //even (0) or odd (1) number of 1's

    if (parity_used == 1) {            //odd parity
      if (evenodd == parity_ok) {
	parity_match[bxn]++;
      } else {
	(*MyOutput_) << "No parity match, data = " 
		     << std::hex << data 
		     << ", counter = " << counter
		     << ", parity_ok = " << parity_ok
		     << std::endl;
	//	::sleep(1);
      }
    } else {                           //even parity
      if (evenodd != parity_ok)
	parity_match[bxn]++;
    }

  }
  
  (*MyOutput_) << "  bxn     total events   parity match" << std::endl;
  (*MyOutput_) << "-------   ------------   ------------" << std::endl;
  for (bxn=0;bxn<8;bxn++) {
    (*MyOutput_) << "   " << std::dec << bxn << "           ";
    (*MyOutput_) << std::dec << total_counts[bxn] << "             ";
    (*MyOutput_) << std::dec << parity_match[bxn] << "        ";
    (*MyOutput_) << std::endl;
  }


  return;
}

void TMBTester::computeBER(int rpc){
  // compute the bit error rate from the RPC->RAT 
  // based on the parity bit counter

  const float rate = 40e+6;    //data coming in at 40MHz
  const float numbits = 15;    //RPC sending 12 bits of random data+2bxn+parity bit
  const int sleepval = 60;     //seconds between each read

  float numberRpcs;

  if (rpc == 0 || rpc == 1) numberRpcs = 1.;
  if (rpc == 2) numberRpcs = 2;

  //GREG  rat_->reset_parity_error_counter();
  reset_parity_error_counter();

  float difference = 0;
  int errors = 0;
  float BER;

  int countzeros;
  int datazero = 0;
  int rpcData[2];

  while (errors == 0) {
    ::sleep(sleepval);

    //** Check that the data being sent isn't all 0's or all 1's...**
    //GREG    rat_->read_rpc_data();
    read_rpc_data();
    for (int i=0; i<2; i++)
      //GREG      rpcData[i] = rat_->GetRpcData(i);
      rpcData[i] = GetRpcData(i);
    
    countzeros = 0;
    for (int i=0;i<16;i++) {
      if (rpc == 2) {
	countzeros += (rpcData[0] >> i) & 0x1;
	countzeros += (rpcData[1] >> i) & 0x1;
      } else {
	countzeros += (rpcData[rpc] >> i) & 0x1;
      }
    }
    
    if ( (countzeros % 16) == 0 ) {
      datazero++; 
    } else {
      datazero = 0;
    }
    
    //GREG    rat_->read_rpc_parity_error_counter();
    read_rpc_parity_error_counter();
    if (rpc == 2) {
      //GREG      errors += rat_->GetRpcParityErrorCounter(0);
      errors += GetRpcParityErrorCounter(0);
      //GREG      errors += rat_->GetRpcParityErrorCounter(1);
      errors += GetRpcParityErrorCounter(1);
    } else {
      //GREG      errors += rat_->GetRpcParityErrorCounter(rpc);
      errors += GetRpcParityErrorCounter(rpc);
    }
    
    difference += (float) sleepval;

    (*MyOutput_) << "Error count = " << std::dec << errors 
		 << " in " << difference << " seconds..."
		 << std::endl;

    BER = 1./(numberRpcs * rate * numbits * difference);

    if (datazero < 20) {
      (*MyOutput_) << "Bit Error Rate = " << BER << std::endl; 
    } else {
      (*MyOutput_) << "RPC's are sending bad data... STOP" << std::endl; 
      break;
    }

  }

  return;
}

/////////////////////////////////////////
// END Some useful methods
/////////////////////////////////////////


///////////////////////////////////////////////
//  The following belong in CrateTiming.cpp
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
  //GREG  rat_->read_rattmb_delay();
  int rpc_delay_default;//GREG = rat_->GetRatTmbDelay();

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

    for (ddd_delay=0; ddd_delay<16; ddd_delay++) {
      count_bad=0;

      // ** write the delay to the RPC **
      //GREG      rat_->set_rattmb_delay(ddd_delay);

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
  write_data = 0x0002;
  tmb_->WriteRegister(vme_ratctrl_adr,write_data);

  int rpc_delay;

  // ** print out results **
  (*MyOutput_) << "**************************" << std::endl;
  (*MyOutput_) << "** TMB-RAT delay results *" << std::endl;
  (*MyOutput_) << "**************************" << std::endl;
  (*MyOutput_) << "rpc_delay   bad data count" << std::endl;
  (*MyOutput_) << "---------   --------------" << std::endl;
  for (rpc_delay = 0; rpc_delay <13; rpc_delay++) {
    (*MyOutput_) << "    " << std::hex << rpc_delay 
		 << "           " << std::hex << rpc_bad[rpc_delay] 
	      <<std::endl;
  }

  window_analysis(rpc_bad,13);

  return;
}

void TMBTester::RpcRatDelayScan(int rpc) {
  //** Find optimal rpc_rat_clock delay = phasing between RPC[rpc] and RAT **

  //Put RAT into correct mode [0]=sync-mode -> sends a fixed data pattern to TMB to be analyzed
  //                          [1]=posneg    -> inserts 12.5ns (1/2-cycle) delay in RPC data path
  //                                           to improve syncing to rising edge of TMB clock
  //                          [2]=loop_tmb
  //                          [3]=free_tx0
  int write_data = 0x0002;
  tmb_->WriteRegister(vme_ratctrl_adr,write_data);

  //GREG  rat_->read_rpcrat_delay();                   //read initial delay values
  read_rpcrat_delay();                   //read initial delay values
  //GREGint initial_delay = rat_->GetRpcRatDelay();  //get values into local variable
  int initial_delay = GetRpcRatDelay();  //get values into local variable

  int delay;

  int parity_err_ctr[16] = {};

  for (delay = 0; delay<=12; delay++) {                             //steps of 2ns
    (*MyOutput_) << "set delay = " << delay 
		 << " for RPC " << rpc
		 << std::endl;
    //GREG    rat_->set_rpcrat_delay(rpc,delay);
    set_rpcrat_delay(rpc,delay);
    //GREGrat_->read_rpcrat_delay();
    read_rpcrat_delay();

    //GREGrat_->reset_parity_error_counter();
    reset_parity_error_counter();
    if (delay>0)
      (*MyOutput_) << "parity error for delay " << delay-1 
		   << " = " << parity_err_ctr[delay-1]
		   << std::endl;
    
    ::sleep(1);                                                    //accumulate statistics

    //GREGrat_->read_rpc_parity_error_counter();
    read_rpc_parity_error_counter();
    //GREGparity_err_ctr[delay] = rat_->GetRpcParityErrorCounter(rpc);
    parity_err_ctr[delay] = GetRpcParityErrorCounter(rpc);
  }

  (*MyOutput_) << "Putting inital delay values back..." << std::endl;
  int value;

  for (int putback=0; putback<4; putback++) {
    value = (initial_delay >> 4*putback) & 0xf;    //parse delay values to reload
    //GREG    rat_->set_rpcrat_delay(putback,value);
    set_rpcrat_delay(putback,value);
  }

  // ** print out results **
  (*MyOutput_) << "********************************" << std::endl;
  (*MyOutput_) << "**** RAT-RPC" << rpc << " delay results ****" << std::endl;
  (*MyOutput_) << "********************************" << std::endl;
  (*MyOutput_) << " delay    parity counter errors" << std::endl;
  (*MyOutput_) << "-------   ---------------------" << std::endl;

  for (delay = 0; delay <=12; delay++) {
    (*MyOutput_) << "   " << std::hex << delay;
    (*MyOutput_) << "               " << std::hex << parity_err_ctr[delay] 
		 << std::endl;
  }

  window_analysis(parity_err_ctr,13);

  return;
}

void TMBTester::window_analysis(int * data, const int length) {
  // ** Determine the best value for the delay setting 
  //    based on vector of data "data", of length "length"

  // ASSUME:  Data wraps around (such as for phase)

  // ASSUME:  Channel is GOOD if it has fewer counts than:
  const int count_threshold = 10;

  // ASSUME:  We want to exclude windows of width less than the following value:
  const int width_threshold = 2;

  (*MyOutput_) << "-----------------------------------------------" << std::endl;
  (*MyOutput_) << "For Window Analysis:" << std::endl;
  (*MyOutput_) << "  -> Counts > " << std::dec << count_threshold 
	       << " considered bad" << std::endl;

  // copy data for wrap-around:
  int twotimeslength = 2 * length;
  int copy[twotimeslength];
  int begin_channel = -1;           
  for (int i=0; i<twotimeslength; i++) {
    copy[i] = data[i % length];
    if (copy[i]>count_threshold && begin_channel<0)   //begin window scan on the first channel of "bad data"
      begin_channel = i;
  }
  if (begin_channel < 0) {
    (*MyOutput_) << std::endl;
    (*MyOutput_) << "Scan is all 0's:  Something is wrong... "<< std::endl;
    return;
  }

  int end_channel = begin_channel+length;

  // Find the windows of "good data", beginning with the first channel of "bad data"
  int window_counter = -1;
  int window_width[length];
  int window_start[length];
  int window_end[length];
  for (int i=0; i<length; i++) {
    window_width[i] = 0;
    window_start[i] = 0;
    window_end[i] = 0;
  }
  for (int delay=begin_channel; delay<end_channel; delay++) {
    if (copy[delay] < count_threshold) {            //Here is a good channel...
      if (copy[delay-1] >= count_threshold)          //and it is a start of a good window
	window_start[++window_counter] = delay;
      window_width[window_counter]++;
    } 
  }

  if (window_counter < 0) {
    (*MyOutput_) << std::endl;
    (*MyOutput_) << "No windows with counts above count_threshold.  Something is wrong... "<< std::endl;
    return;
  }    

  int counter;
  for (counter=0; counter<=window_counter; counter++) {
    window_end[counter] = window_start[counter]+window_width[counter] -1;
    (*MyOutput_) << "Window = " << std::dec << counter;
    (*MyOutput_) << " is " << std::dec << window_width[counter] << " channels wide, ";
    (*MyOutput_) << " from " << std::dec << (window_start[counter] % 13)
		 << " to " << std::dec << (window_end[counter] % 13) << std::endl;    
  }

  (*MyOutput_) << "  -> window must be at least " << std::dec << width_threshold 
	       << " channels wide" << std::endl;

  //Determine middle of window(s)
  int channel;
  float value;
  float channel_ctr;
  float denominator;
  float average_channel[length];

  for (counter=0; counter<=window_counter; counter++) {
    if (window_width[counter] > width_threshold) {                      // exclude narrow width windows
      channel_ctr=0.;
      denominator=0.;
      for (channel=window_start[counter]; channel<=window_end[counter]; channel++) {
	// For average, use unary operation on bits in counter (since 0 = good)
	value = (float) ~(copy[channel] | 0xffff0000);
	channel_ctr += value * (float) channel;
	denominator += value;
      }
      average_channel[counter] = channel_ctr / denominator;
      (*MyOutput_) << "=> BEST DELAY VALUE (window " << std::dec << counter << ") = "
		   <<  (int) (average_channel[counter]+0.5) % 13 << " <=" << std::endl;
    }
  }
  (*MyOutput_) << "-----------------------------------------------" << std::endl;
  return;
}

//////////////////////////////////////////////////
//  END  The following belong in CrateTiming.cpp
//////////////////////////////////////////////////


////////////////////////////////////////////////
// The following belong in RAT.cc
////////////////////////////////////////////////
void TMBTester::rpc_fpga_finished() {
  // Test if the RPC FPGA is finished

  int rpc_done = 0;
  int read_data;
  int counter = 0;
  while (!rpc_done) {
    read_data = tmb_->ReadRegister(rpc_cfg_adr);
    rpc_done = (read_data >> 14) & 0x1;
    counter++;
    if (counter % 1000 == 0) 
      (*MyOutput_) << "Waiting for RPC FPGA to be finished...";
    if (counter > 100000) {
      (*MyOutput_) << "RAT must not be plugged in..." << std::endl;
      (*MyOutput_) << "**********************************" << std::endl;
      (*MyOutput_) << "**** RAT is not going to work ****" << std::endl;
      (*MyOutput_) << "**********************************" << std::endl;
      rpc_done = 1;
    }
  }
  return;
}
//
void TMBTester::ReadRatIdCode(){
  (*MyOutput_) << "RAT: Read RAT ID Codes" << std::endl;

  rpc_fpga_finished();

  EMUjtag_->setup_jtag(ChainRat);

  EMUjtag_->ShfIR_ShfDR(ChipLocationRatFpga,
			VTX2_IDCODE,
			RegSizeRatFpga_VTX2_IDCODE);
  rat_idcode_[0] = tmb_->bits_to_int(EMUjtag_->GetDRtdo(),EMUjtag_->GetRegLength(),0);


  EMUjtag_->ShfIR_ShfDR(ChipLocationRatProm,
			PROM_IDCODE,
			RegSizeRatProm_PROM_IDCODE);
  rat_idcode_[1] = tmb_->bits_to_int(EMUjtag_->GetDRtdo(),EMUjtag_->GetRegLength(),0);

  (*MyOutput_) << "RAT FPGA ID code = " << rat_idcode_[0] << std::endl;
  (*MyOutput_) << "RAT PROM ID code = " << rat_idcode_[1] << std::endl;
  
  return;
}
//
void TMBTester::ReadRatUserCode(){
  (*MyOutput_) << "RAT: Read RAT User Codes" << std::endl;

  rpc_fpga_finished();

  EMUjtag_->setup_jtag(ChainRat);

  EMUjtag_->ShfIR_ShfDR(ChipLocationRatFpga,
			VTX2_USERCODE,
			RegSizeRatFpga_VTX2_USERCODE);
  rat_usercode_[0] = tmb_->bits_to_int(EMUjtag_->GetDRtdo(),EMUjtag_->GetRegLength(),0);

  
  EMUjtag_->ShfIR_ShfDR(ChipLocationRatProm,
			PROM_USERCODE,
			RegSizeRatProm_PROM_USERCODE);
  rat_usercode_[1] = tmb_->bits_to_int(EMUjtag_->GetDRtdo(),EMUjtag_->GetRegLength(),0);


  (*MyOutput_) << "RAT FPGA User code = " << rat_usercode_[0] << std::endl;
  (*MyOutput_) << "RAT PROM User code = " << rat_usercode_[1] << std::endl;
  
  return;
}
//
void TMBTester::ReadRatUser1(){
  (*MyOutput_) << "RAT: Read RAT USER1 JTAG (status register)" << std::endl;

  rpc_fpga_finished();

  //clear user1_value_
  for (int i=0; i<MAX_FRAMES; i++) 
    user1_value_[i] = 0;

  EMUjtag_->setup_jtag(ChainRat);
		       
  EMUjtag_->ShfIR_ShfDR(ChipLocationRatFpga,
			VTX2_USR1,
			RegSizeRatFpga_VTX2_USR1);

  //Fill user1_value_ with JTAG data...
  rat_user1_length_ = EMUjtag_->GetRegLength();
  int * user1_pointer = EMUjtag_->GetDRtdo();
  
  for (int i=0; i<rat_user1_length_; i++) 
    user1_value_[i] = *(user1_pointer+i);
      
      
  //Print out the USER1 value from right (first bit out) to left (last bit out):
  char rat_user1[rat_user1_length_/8];
  EMUjtag_->packCharBuffer(user1_value_,rat_user1_length_,rat_user1);

  (*MyOutput_) << "RAT USER1 = ";
  for (int counter=(rat_user1_length_/8)-1; counter>=0; counter--) 
    (*MyOutput_) << ((rat_user1[counter] >> 4) & 0xf) << (rat_user1[counter] & 0xf);
  (*MyOutput_) << std::endl;

  //  decodeRATUser1();

  return;
}
//
void TMBTester::decodeRATUser1() {
  // ** parse the bit array from the USER1 data register
  // ** to print it out in a human readable form
  int counter=0;
  int i;

  int rs_begin_array[4];
  for (i=0;i<4;i++) 
    rs_begin_array[i] = user1_value_[counter++];
  int rs_begin = tmb_->bits_to_int(rs_begin_array,4,0);
  (*MyOutput_) << "Begin marker = " << rs_begin << std::endl;

  int rs_version_array[4];
  for (i=0;i<4;i++) 
    rs_version_array[i] = user1_value_[counter++];
  int rs_version = tmb_->bits_to_int(rs_version_array,4,0);
  (*MyOutput_) << "Version ID = " << rs_version << std::endl;

  int rs_monthday_array[16];
  for (i=0;i<16;i++) 
    rs_monthday_array[i] = user1_value_[counter++];
  int rs_monthday = tmb_->bits_to_int(rs_monthday_array,16,0);
  (*MyOutput_) << "Version Month/Day = " << rs_monthday << std::endl;

  int rs_year_array[16];
  for (i=0;i<16;i++) 
    rs_year_array[i] = user1_value_[counter++];
  int rs_year = tmb_->bits_to_int(rs_year_array,16,0);
  (*MyOutput_) << "Version Year = " << rs_year << std::endl;

  int rs_syncmode = user1_value_[counter++];
  (*MyOutput_) << "1-> 80MHz sync mode = " << rs_syncmode << std::endl;

  int rs_posneg = user1_value_[counter++];
  (*MyOutput_) << "1-> Latch 40MHz RPC data on posedge = " << rs_posneg << std::endl;

  int rs_loop = user1_value_[counter++];
  (*MyOutput_) << "1-> Loopback mode = " << rs_loop << std::endl;

  int rs_rpc_en_array[2];
  for (i=0;i<2;i++) 
    rs_rpc_en_array[i] = user1_value_[counter++];
  int rs_rpc_en = tmb_->bits_to_int(rs_rpc_en_array,2,0);
  (*MyOutput_) << "RPC driver enabled = " << rs_rpc_en << std::endl;

  int rs_clk_active_array[2];
  for (i=0;i<2;i++) 
    rs_clk_active_array[i] = user1_value_[counter++];
  int rs_clk_active = tmb_->bits_to_int(rs_clk_active_array,2,0);
  (*MyOutput_) << "RPC direct clock status = " << rs_clk_active << std::endl;

  int rs_locked_tmb = user1_value_[counter++];
  (*MyOutput_) << "TMB DLL locked = " << rs_locked_tmb << std::endl;

  int rs_locked_rpc0 = user1_value_[counter++];
  (*MyOutput_) << "RPC0 DLL locked = " << rs_locked_rpc0 << std::endl;

  int rs_locked_rpc1 = user1_value_[counter++];
  (*MyOutput_) << "RPC1 DLL locked = " << rs_locked_rpc1 << std::endl;

  int rs_locklost_tmb = user1_value_[counter++];
  (*MyOutput_) << "TMB DLL lost lock = " << rs_locklost_tmb << std::endl;

  int rs_locklost_rpc0 = user1_value_[counter++];
  (*MyOutput_) << "RPC0 DLL lost lock = " << rs_locklost_rpc0 << std::endl;

  int rs_locklost_rpc1 = user1_value_[counter++];
  (*MyOutput_) << "RPC1 DLL lost lock = " << rs_locklost_rpc1 << std::endl;

  int rs_txok = user1_value_[counter++];
  (*MyOutput_) << "ALCT TX OK = " << rs_txok << std::endl;

  int rs_rxok = user1_value_[counter++];
  (*MyOutput_) << "ALCT RX OK = " << rs_rxok << std::endl;

  int rs_ntcrit = user1_value_[counter++];
  (*MyOutput_) << "Over Temperature Threshold = " << rs_ntcrit << std::endl;

  int rs_rpc_free = user1_value_[counter++];
  (*MyOutput_) << "rpc_free0 from TMB = " << rs_rpc_free << std::endl;

  int rs_dsn = user1_value_[counter++];
  (*MyOutput_) << "rpc_dsn to TMB = " << rs_dsn << std::endl;

  int rs_dddoe_wr_array[4];
  for (i=0;i<4;i++) 
    rs_dddoe_wr_array[i] = user1_value_[counter++];
  int rs_dddoe_wr = tmb_->bits_to_int(rs_dddoe_wr_array,4,0);
  (*MyOutput_) << "DDD status:  output enables = " << rs_dddoe_wr << std::endl;

  int rs_ddd_wr_array[16];
  for (i=0;i<16;i++) 
    rs_ddd_wr_array[i] = user1_value_[counter++];
  int rs_ddd_wr = tmb_->bits_to_int(rs_ddd_wr_array,16,0);
  (*MyOutput_) << "DDD status:  delay values = " << rs_ddd_wr << std::endl;

  int rs_ddd_auto = user1_value_[counter++];
  (*MyOutput_) << "1-> start DDD on power-up = " << rs_ddd_auto << std::endl;

  int rs_ddd_start = user1_value_[counter++];
  (*MyOutput_) << "DDD status: start ddd machine  = " << rs_ddd_start << std::endl;

  int rs_ddd_busy = user1_value_[counter++];
  (*MyOutput_) << "DDD status: state machine busy = " << rs_ddd_busy << std::endl;

  int rs_ddd_verify_ok = user1_value_[counter++];
  (*MyOutput_) << "DDD status: data readback OK = " << rs_ddd_verify_ok << std::endl;

  int rs_rpc0_parity_ok = user1_value_[counter++];
  (*MyOutput_) << "RPC0 parity OK currently = " << rs_rpc0_parity_ok << std::endl;

  int rs_rpc1_parity_ok = user1_value_[counter++];
  (*MyOutput_) << "RPC1 parity OK currently = " << rs_rpc1_parity_ok << std::endl;

  int rs_rpc0_cnt_perr_array[16];
  for (i=0;i<16;i++) 
    rs_rpc0_cnt_perr_array[i] = user1_value_[counter++];
  int rs_rpc0_cnt_perr = tmb_->bits_to_int(rs_rpc0_cnt_perr_array,16,0);
  (*MyOutput_) << "RPC0 parity error counter = " << rs_rpc0_cnt_perr << std::endl;

  int rs_rpc1_cnt_perr_array[16];
  for (i=0;i<16;i++) 
    rs_rpc1_cnt_perr_array[i] = user1_value_[counter++];
  int rs_rpc1_cnt_perr = tmb_->bits_to_int(rs_rpc1_cnt_perr_array,16,0);
  (*MyOutput_) << "RPC1 parity error counter = " << rs_rpc1_cnt_perr << std::endl;

  int rs_last_opcode_array[5];
  for (i=0;i<5;i++) 
    rs_last_opcode_array[i] = user1_value_[counter++];
  int rs_last_opcode = tmb_->bits_to_int(rs_last_opcode_array,5,0);
  (*MyOutput_) << "Last firmware TAP cmd opcode = " << rs_last_opcode << std::endl;


  int rw_rpc_en_array[2];
  for (i=0;i<2;i++) 
    rw_rpc_en_array[i] = user1_value_[counter++];
  int rw_rpc_en = tmb_->bits_to_int(rw_rpc_en_array,2,0);
  (*MyOutput_) << "rw_rpc_en = " << rw_rpc_en << std::endl;

  int rw_ddd_start = user1_value_[counter++];
  (*MyOutput_) << "rw_ddd_start  = " << rw_ddd_start << std::endl;

  int rw_ddd_wr_array[16];
  for (i=0;i<16;i++) 
    rw_ddd_wr_array[i] = user1_value_[counter++];
  int rw_ddd_wr = tmb_->bits_to_int(rw_ddd_wr_array,16,0);
  (*MyOutput_) << "rw_ddd_wr = " << rw_ddd_wr << std::endl;

  int rw_dddoe_wr_array[4];
  for (i=0;i<4;i++) 
    rw_dddoe_wr_array[i] = user1_value_[counter++];
  int rw_dddoe_wr = tmb_->bits_to_int(rw_dddoe_wr_array,4,0);
  (*MyOutput_) << "rw_dddoe_wr = " << rw_dddoe_wr << std::endl;

  int rw_perr_reset = user1_value_[counter++];
  (*MyOutput_) << "rw_perr_reset = " << rw_perr_reset << std::endl;

  int rw_parity_odd = user1_value_[counter++];
  (*MyOutput_) << "rw_parity_odd = " << rw_parity_odd << std::endl;

  int rw_perr_ignore = user1_value_[counter++];
  (*MyOutput_) << "rw_perr_ignore = " << rw_perr_ignore << std::endl;

  int rw_rpc_future_array[6];
  for (i=0;i<6;i++) 
    rw_rpc_future_array[i] = user1_value_[counter++];
  int rw_rpc_future = tmb_->bits_to_int(rw_rpc_future_array,6,0);
  (*MyOutput_) << "rw_rpc_future = " << rw_rpc_future << std::endl;


  int rs_rpc0_pdata_array[19];
  for (i=0;i<19;i++) 
    rs_rpc0_pdata_array[i] = user1_value_[counter++];
  int rs_rpc0_pdata = tmb_->bits_to_int(rs_rpc0_pdata_array,19,0);
  (*MyOutput_) << "RPC0 data (includes 16 pad bits + 3bxn) = " << rs_rpc0_pdata << std::endl;

  int rs_rpc1_pdata_array[19];
  for (i=0;i<19;i++) 
    rs_rpc1_pdata_array[i] = user1_value_[counter++];
  int rs_rpc1_pdata = tmb_->bits_to_int(rs_rpc1_pdata_array,19,0);
  (*MyOutput_) << "RPC1 data (includes 16 pad bits + 3bxn) = " << rs_rpc1_pdata << std::endl;

  int rs_unused_array[29];
  for (i=0;i<29;i++) 
    rs_unused_array[i] = user1_value_[counter++];
  int rs_unused = tmb_->bits_to_int(rs_unused_array,29,0);
  (*MyOutput_) << "Unused bits = " << rs_unused << std::endl;

  int rs_end_array[4];
  for (i=0;i<4;i++) 
    rs_end_array[i] = user1_value_[counter++];
  int rs_end = tmb_->bits_to_int(rs_end_array,4,0);
  (*MyOutput_) << "End marker = " << rs_end << std::endl;

  return;
}
//

void TMBTester::ReadRatUser2(){
  (*MyOutput_) << "RAT: Read RAT USER2 JTAG (control register)" << std::endl;

  rpc_fpga_finished();

  //clear user2_value_
  for (int i=0; i<MAX_FRAMES; i++) 
    user2_value_[i] = 0;

  EMUjtag_->setup_jtag(ChainRat);

  EMUjtag_->ShfIR_ShfDR(ChipLocationRatFpga,
			VTX2_USR2,
			RegSizeRatFpga_VTX2_USR2);

  //Fill user2_ information:
  rat_user2_length_ = EMUjtag_->GetRegLength();
  int * user2_pointer = EMUjtag_->GetDRtdo();

  for (int i=0; i<rat_user2_length_; i++) 
    user2_value_[i] = *(user2_pointer+i);

  //  (*MyOutput_) << "user2_value_ = ";
  //  for (int i=rat_user2_length_-1; i>=0; i--) (*MyOutput_) << user2_value_[i];
  //  (*MyOutput_) << std::endl;

  //Put data back into User2 (readout was destructive)
  int rsd[rat_user2_length_];

  for (int i=0; i<rat_user2_length_; i++) 
    rsd[i] = user2_value_[i];
  
  EMUjtag_->ShfIR_ShfDR(ChipLocationRatFpga,
			VTX2_USR2,
			RegSizeRatFpga_VTX2_USR2,
			rsd);

  //Print out the USER2 value from right (first bit out) to left (last bit out):
  char rat_user2[rat_user2_length_/8];
  EMUjtag_->packCharBuffer(user2_value_,rat_user2_length_,rat_user2);

  (*MyOutput_) << "RAT USER2 = ";
  for (int counter=(rat_user2_length_/8)-1; counter>=0; counter--) 
    (*MyOutput_) << ((rat_user2[counter] >> 4) & 0xf) << (rat_user2[counter] & 0xf);
  (*MyOutput_) << std::endl;

  //  decodeRATUser2();

  return;
}

void TMBTester::decodeRATUser2() {

  // ** parse the bit array from the USER2 data register
  // ** to print it out in a human readable form
  int i;
  int counter = 0;

  int ws_rpc_en_array[2];
  for (i=0;i<2;i++) 
    ws_rpc_en_array[i] = user2_value_[counter++];
  int ws_rpc_en = tmb_->bits_to_int(ws_rpc_en_array,2,0);

  int ws_ddd_start = user2_value_[counter++];

  int ws_ddd_wr_array[16];
  for (i=0;i<16;i++) 
    ws_ddd_wr_array[i] = user2_value_[counter++];
  int ws_ddd_wr = tmb_->bits_to_int(ws_ddd_wr_array,16,0);

  int ws_dddoe_wr_array[4];
  for (i=0;i<4;i++) 
    ws_dddoe_wr_array[i] = user2_value_[counter++];
  int ws_dddoe_wr = tmb_->bits_to_int(ws_dddoe_wr_array,4,0);

  int ws_perr_reset = user2_value_[counter++];

  int ws_parity_odd = user2_value_[counter++];

  int ws_perr_ignore = user2_value_[counter++];

  int ws_rpc_future_array[6];
  for (i=0;i<6;i++) 
    ws_rpc_future_array[i] = user2_value_[counter++];
  int ws_rpc_future = tmb_->bits_to_int(ws_rpc_future_array,6,0);

  (*MyOutput_) << "ws_rpc_en = " << ws_rpc_en << std::endl;
  (*MyOutput_) << "ws_ddd_start  = " << ws_ddd_start << std::endl;
  (*MyOutput_) << "ws_ddd_wr = " << ws_ddd_wr << std::endl;
  (*MyOutput_) << "ws_dddoe_wr = " << ws_dddoe_wr << std::endl;
  (*MyOutput_) << "ws_perr_reset = " << ws_perr_reset << std::endl;
  (*MyOutput_) << "ws_parity_odd = " << ws_parity_odd << std::endl;
  (*MyOutput_) << "ws_perr_ignore = " << ws_perr_ignore << std::endl;
  (*MyOutput_) << "ws_rpc_future = " << ws_rpc_future << std::endl;

  return;
}
//
void TMBTester::WriteRatUser2_(int * data_in){
  (*MyOutput_) << "RAT: Write RAT USER2 JTAG (control register)" << std::endl;

  rpc_fpga_finished();

  EMUjtag_->setup_jtag(ChainRat);
		       
  EMUjtag_->ShfIR_ShfDR(ChipLocationRatFpga,
			VTX2_USR2,
			RegSizeRatFpga_VTX2_USR2,
			data_in);

  return;
}
//
void TMBTester::set_rpcrat_delay(int rpc,int delay) {

  if ( rpc==0 || rpc==1 ) {
    if (delay >= 0 && delay <=12 ) {
      (*MyOutput_) << "Set RPC " << rpc << " delay = " << std::hex << delay << std::endl;
    } else {
      (*MyOutput_) << "only delay values 0-12 allowed" << std::endl;      
      return;
    }
  } else {
    (*MyOutput_) << "RPC " << rpc << " does not exist" << std::endl;
    return;
  }

  ReadRatUser2();               // get the current user2_value_

  delay &= 0x000f;              //strip off extraneous bits

  int lowbit =  2 + rpc*4;      //lowest position of delay bit to set
  int highbit = lowbit+4;       //highest position of delay bit to set

  int rsd[MAX_FRAMES];
  int bitcount = 0;

  // ** Set the new RAT USER2 bits. ** 
  // ** Bits which are unchanged need to be explicitly put back**
  for (int i=0; i<rat_user2_length_; i++) {
    rsd[i]=user2_value_[i];
    if (i==2) 
      rsd[i] = 0;                            // unstart state machine
    if (i>lowbit && i<=highbit ) {
      rsd[i] = (delay >> bitcount++) & 0x1;  // new delay values
    }
  }
  WriteRatUser2_(rsd);

  // **Check that the DDD state machine went idle...**
  int ddd_busy = 1;
  while (ddd_busy != 0) {
    ReadRatUser1();
    ddd_busy = user1_value_[80] & 0x1;

    if (ddd_busy != 0) {
      (*MyOutput_) << "ERROR: State machine stuck busy" << std::endl;
      ::sleep(1);
    }
  }

  // **Start DDD state machine...**
  rsd[2] = 0x1;                         //keep all the other tdi data the same...
  WriteRatUser2_(rsd);

  // **Check that the DDD state machine went busy...**
  ddd_busy = 0;
  while (ddd_busy != 1) {
    ReadRatUser1();
    ddd_busy = user1_value_[80] & 0x1;

    if (ddd_busy != 1) {
      (*MyOutput_) << "ERROR: State machine did not go busy" << std::endl;
      ::sleep(1);
    }
  }

  // **Unstart DDD state machine again...**
  rsd[2] = 0x0;
  WriteRatUser2_(rsd);

  // **Check that the DDD state machine is not busy...**
  ddd_busy = 1;
  while (ddd_busy != 0) {
    ReadRatUser1();
    ddd_busy = user1_value_[80] & 0x1;

    if (ddd_busy != 0) {
      (*MyOutput_) << "ERROR: State machine is busy" << std::endl;
      ::sleep(1);
    }
  }

  // **Check that the data made it OK...**
  int ddd_verify = user1_value_[81] & 0x1;

  if (ddd_verify != 1) {
    (*MyOutput_) << "ERROR: DDD data not verified" << std::endl;
    ::sleep(1);
  }

  // ** Print out the delay values for the RAT ddd chip: **
  read_rpcrat_delay();

  return;
}
//
void TMBTester::read_rpcrat_delay(){
  (*MyOutput_) << "RAT: Read RPC-RAT delays" << std::endl;

  ReadRatUser1();             //get the current USER1 values

  int i;

  int delays[16];
  int offset = 62;
  int bit = 0;
  for (i=offset; i<(offset+16); i++) 
    delays[bit++] = user1_value_[i];

  rpc_rat_delay_ = tmb_->bits_to_int(delays,16,0);
  (*MyOutput_) << "rpc_rat_delay_ = " << std::hex << rpc_rat_delay_ << std::endl;

  int ddd_delay[4];
  for (i=0; i<4; i++) 
    ddd_delay[i] = (rpc_rat_delay_ >> i*4) & 0xf;

  (*MyOutput_) << "RPC0 = " << std::hex << ddd_delay[0] << std::endl;
  (*MyOutput_) << "RPC1 = " << std::hex << ddd_delay[1] << std::endl;
  (*MyOutput_) << "RPC2 = NOT USED" << std::endl;
  (*MyOutput_) << "RPC3 = NOT USED" << std::endl;

  return;
}
//
void TMBTester::set_rattmb_delay(int delay) {

  if (delay < 0 || delay >15 ) {
    (*MyOutput_) << "only delay values 0-15 allowed" << std::endl;      
    return;
  }

  tmb_->tmb_clk_delays(delay,8);

  return;
}
//
void TMBTester::read_rattmb_delay() {
  rat_tmb_delay_ = tmb_->tmb_read_delays(8);

  (*MyOutput_) << "RAT-TMB delay = " << std::hex << rat_tmb_delay_ << std::endl;      

  return;
}
//
void TMBTester::set_perr_ignore(){
  (*MyOutput_) 
    << "RAT: Ignore All 1's and all 1's data words in parity error counter" 
    << std::endl;

  ReadRatUser2();                           // get the current user2_value_

  int rsd[rat_user2_length_];

  // ** Set the new RAT USER2 bits. ** 
  // ** Bits which are unchanged need to be explicitly put back**
  for (int i=0; i<rat_user2_length_; i++) {
    rsd[i]=user2_value_[i];
    if (i==25) 
      rsd[i] = 1;                            // set perr_ignore bit
  }
  WriteRatUser2_(rsd);

  return;
}
//
void TMBTester::unset_perr_ignore(){
  (*MyOutput_) 
    << "RAT: DO NOT ignore all 1's and all 1's data words in parity error counter" 
    << std::endl;

  ReadRatUser2();                           // get the current user2_value_

  int rsd[rat_user2_length_];

  // ** Set the new RAT USER2 bits. ** 
  // ** Bits which are unchanged need to be explicitly put back**
  for (int i=0; i<rat_user2_length_; i++) {
    rsd[i]=user2_value_[i];
    if (i==25) 
      rsd[i] = 0;                            // set perr_ignore bit
  }
  WriteRatUser2_(rsd);

  return;
}
//
void TMBTester::use_parity_odd() {
  (*MyOutput_) 
    << "RAT: Use odd parity in parity error counter" 
    << std::endl;

  ReadRatUser2();                           // get the current user2_value_

  int rsd[rat_user2_length_];

  // ** Set the new RAT USER2 bits. ** 
  // ** Bits which are unchanged need to be explicitly put back**
  for (int i=0; i<rat_user2_length_; i++) {
    rsd[i]=user2_value_[i];
    if (i==24) 
      rsd[i] = 1;                            // set parity odd bit
  }
  WriteRatUser2_(rsd);

  return;
}
//
void TMBTester::use_parity_even() {
  (*MyOutput_) 
    << "RAT: Use even parity in parity error counter" 
    << std::endl;

  ReadRatUser2();                           // get the current user2_value_

  int rsd[rat_user2_length_];

  // ** Set the new RAT USER2 bits. ** 
  // ** Bits which are unchanged need to be explicitly put back**
  for (int i=0; i<rat_user2_length_; i++) {
    rsd[i]=user2_value_[i];
    if (i==24) 
      rsd[i] = 0;                            // set parity even bit
  }
  WriteRatUser2_(rsd);

  return;
}
//
void TMBTester::ReadRpcParity() {
  (*MyOutput_) 
    << "RAT: Determine parity used in parity error counter" 
    << std::endl;

  ReadRatUser2();                           // get the current user2_value_

  rpc_parity_used_ = user2_value_[24]; 

  return;
}
//
void TMBTester::reset_parity_error_counter() {
  (*MyOutput_) << "RAT: Reset RPC parity error counter" << std::endl;

  ReadRatUser2();                           // get the current user2_value_

  int rsd[rat_user2_length_];

  // ** Set the new RAT USER2 bits. ** 
  // ** Bits which are unchanged need to be explicitly put back**
  for (int i=0; i<rat_user2_length_; i++) {
    rsd[i]=user2_value_[i];
    if (i==23) 
      rsd[i] = 1;                            // assert parity reset
  }
  WriteRatUser2_(rsd);

  rsd[23] = 0;                               // de-assert parity reset
  WriteRatUser2_(rsd);

  return;
}
//
void TMBTester::read_rpc_parity_error_counter() {
  (*MyOutput_) << "RAT: Read RPC parity error counter" << std::endl;

  ReadRatUser1();                                   //get the current USER1 value

  int counter_array[2][16];
  for (int rpc=0; rpc<2; rpc++) {
    for (int i=0;i<16;i++) {
      counter_array[rpc][i] = user1_value_[84+16*rpc+i];
      rpc_parity_err_ctr_[rpc] = tmb_->bits_to_int(counter_array[rpc],16,0);
    }
    (*MyOutput_) << "RPC " << rpc 
		 << " Parity error counter = " << rpc_parity_err_ctr_[rpc] 
		 << std::endl;  
  }

  return;
}
//
void TMBTester::read_rpc_data() {
  (*MyOutput_) << "RAT: Read RPC data" << std::endl;

  ReadRatUser1();                                   //get the current USER1 value

  int counter,bit;
  int data_array[2][19];

  for (int rpc=0; rpc<2; rpc++) {
    for (bit=0;bit<19;bit++) {
      data_array[rpc][bit] = user1_value_[153+19*rpc+bit];
      rpc_data_[rpc] = tmb_->bits_to_int(data_array[rpc],19,0);
      rpc_parity_ok_[rpc] = user1_value_[82+rpc];
    }

    //** Count how many ones are in the word... **
    //** Whether the count is even or odd should be correlated with the rpc_parity_ok_[] bit **
    counter = 0;
    for (bit=0;bit<19;bit++) {
      counter += (rpc_data_[rpc] >> bit) & 0x1;
    }

    (*MyOutput_) << "RPC " << rpc 
		 << " Data = " << rpc_data_[rpc] 
		 << " number of bits = " << counter 
		 << " -> Parity ok = " << rpc_parity_ok_[rpc] 
		 << std::endl;
  }

  return;
}
//
int TMBTester::ReadRATtempPCB() {

  int smb_adr = 0x18;   // gnd, gnd state RAT LM84 chip address
  int command = 0x00;   // "local" temperature read
  //GREG  int temperature = tmb_->smb_io(smb_adr,command,2);
  int temperature = smb_io(smb_adr,command,2);

  (*MyOutput_) << "RAT temperature (PCB)                 = " << std::dec << temperature
	       << " deg C " << std::endl;

  return temperature;
}
//
int TMBTester::ReadRATtempHSink() {

  int smb_adr = 0x18;   // gnd, gnd state RAT LM84 chip address
  int command = 0x01;   // "remote" temperature read
  //GREG  int temperature = tmb_->smb_io(smb_adr,command,2);
  int temperature = smb_io(smb_adr,command,2);

  (*MyOutput_) << "RAT temperature (Heat Sink)           = " << std::dec << temperature
	       << " deg C " << std::endl;

  return temperature;
}
//
int TMBTester::ReadRATtCritPCB() {

  int smb_adr = 0x18;   // gnd, gnd state RAT LM84 chip address  
  int command = 0x05;   // "local" temperature critical read
  //GREG  int temperature = tmb_->smb_io(smb_adr,command,2);
  int temperature = smb_io(smb_adr,command,2);

  (*MyOutput_) << "RAT Critical Temperature (PCB)        = " << std::dec << temperature
	       << " deg C " << std::endl;

  return temperature;
}
//
int TMBTester::ReadRATtCritHSink() {

  int smb_adr = 0x18;   // gnd, gnd state RAT LM84 chip address
  int command = 0x07;   // "remote" temperature critical read
  //GREG  int temperature = tmb_->smb_io(smb_adr,command,2);
  int temperature = smb_io(smb_adr,command,2);

  (*MyOutput_) << "RAT Critical Temperature (Heat Sink)  = " << std::dec << temperature
	       << " deg C " << std::endl;

  return temperature;
}
//
////////////////////////////////////////////////
// END The following belong in RAT.cc
////////////////////////////////////////////////


///////////////////////////////////////////////
//  The following belong in TMB.cc
///////////////////////////////////////////////
void TMBTester::ExtClctTrigFromCCBonly() {

  (*MyOutput_) << "Enable CLCT external trigger from TTC (through CCB)" << std::endl;

  int data = tmb_->ReadRegister(seq_trig_en_adr);

  (*MyOutput_) << "TMB Sequencer trigger source before = " << std::hex << data << std::endl;

  data = 0x0020;                                 //allow CLCT external triggers from CCB

  tmb_->WriteRegister(seq_trig_en_adr,data);

  data = tmb_->ReadRegister(seq_trig_en_adr);

  (*MyOutput_) << "TMB Sequencer trigger source after = " << std::hex << data << std::endl;

  return;
}

int TMBTester::ReadTMBtempPCB() {

  int smb_adr = 0x2a;   // float, float state TMB LM84 chip address
  int command = 0x00;   // "local" temperature read
  int temperature = smb_io(smb_adr,command,1);

  (*MyOutput_) << "TMB temperature (PCB)                 = " << std::dec << temperature
	       << " deg C " << std::endl;


  return temperature;
}

int TMBTester::ReadTMBtempFPGA() {

  int smb_adr = 0x2a;   // float, float state TMB LM84 chip address
  int command = 0x01;   // "remote" temperature read
  int temperature = smb_io(smb_adr,command,1);

  (*MyOutput_) << "TMB temperature IC (FPGA)             = " << std::dec << temperature
	       << " deg C " << std::endl;

  return temperature;
}

int TMBTester::ReadTMBtCritPCB() {

  int smb_adr = 0x2a;   // float, float state TMB LM84 chip address
  int command = 0x05;   // "local" temperature critical read
  int temperature = smb_io(smb_adr,command,1);

  (*MyOutput_) << "TMB Critical Temperature IC (PCB)     = " << std::dec << temperature
	       << " deg C " << std::endl;

  return temperature;
}

int TMBTester::ReadTMBtCritFPGA() {

  int smb_adr = 0x2a;   // float, float state TMB LM84 chip address
  int command = 0x07;   // "remote" temperature critical read
  int temperature = smb_io(smb_adr,command,1);

  (*MyOutput_) << "TMB Critical Temperature (FPGA)       = " << std::dec << temperature
	       << " deg C " << std::endl;

  return temperature;
}

int TMBTester::smb_io(int smb_adr, int cmd, int module) {
  //	Generates SMB serial clock and data streams to TMB LM84 chip
  //
  //   -> Returns temperature values in Celcius <-
  //
  //    smb_adr = 0x2a = float, float state TMB LM84 chip address
  //            = 0x18 = gnd, gnd state RAT LM84 chip address
  //
  //    cmd = 0x00 = local temperature command
  //        = 0x01 = remote temperature command
  //        = 0x05 = local tcrit command  
  //        = 0x07 = remote tcrit command  

  //    module = 1 = TMB
  //           = 2 = RAT

  const int adc_adr = vme_adc_adr;

  int smb_data = 0xff;        // null write command

  int write_data,read_data;

  // Current ADC register state:
  int adc_status = tmb_->ReadRegister(adc_adr);

  // **Step 1 write the command to read the data**

  // ** initialize SMB data stream **
  int sda_bit[29];
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
  //
  //	SMB requires that serial data is stable while clock is high,
  //	so data transitions occur while clock is low,
  //	midway between clock falling edge and rising edge

  int nclks = 115;

  int sda_clock,scl_clock;
  int sda,scl;

  int i2c_clock;
  for (i2c_clock=0; i2c_clock<=nclks; i2c_clock++) {  //200kHz
    sda_clock = (int) i2c_clock/4;                //50 kHz
    scl_clock = (int) ( (i2c_clock+1)/2 );        //50 kHz shifted 1/2 of a 100kHz cycle

    scl = scl_clock & 1;                          // 0 0 1 1 0 0 1 1 0 0 1 1 ....
    sda = sda_bit[sda_clock];

    //    (*MyOutput_) << "Before Persistent -> i2c_clock " << i2c_clock << ", scl = " << scl << " sda_bit = " << sda << std::endl;

    if (i2c_clock<3) scl=1;                       // START scl stays high
    if (i2c_clock<2) sda=1;                       // START sda transitions low

    if (i2c_clock>nclks-3) scl=1;                // STOP scl stays high
    if (i2c_clock>nclks-2) sda=1;                // STOP sda transitions high

    //    (*MyOutput_) << "After Persistent  -> i2c_clock " << i2c_clock << ", scl = " << scl << " sda_bit = " << sda << std::endl;

    //** Write serial clock and data to TMB VME interface **

    write_data = adc_status & 0xf9ff;    //clear bits 9 and 10
    write_data |= scl << 9;
    write_data |= sda << 10;
    tmb_->WriteRegister(adc_adr,write_data);
  }

  // Current ADC register state:
  adc_status = tmb_->ReadRegister(adc_adr);

  // **Step 2 read the data**

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

    //** Read Serial data from TMB VME interface **
    // (read on every cycle to keep clock symmetric)
    read_data = tmb_->ReadRegister(adc_adr);
    if (scl==1) {
      d[sda_clock] = read_data;
    }
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
    } else {
      data |= sda_value<<i;          //sign extend if bit 7 indicates negative value      
    }
  }

  //  (*MyOutput_) << "Temperature = " << std::dec << data << " deg C" << std::endl;

  return data;
}
///////////////////////////////////////////////
//  END: The following belong in TMB.cc
///////////////////////////////////////////////

