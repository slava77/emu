#include<iostream>
#include<math.h> 
#include<stdio.h>
#include<string>
#include<unistd.h> // for sleep()
#include <stdlib.h>
#include "emu/pc/TMBTester.h"
#include "emu/pc/TMB.h"
#include "emu/pc/CCB.h"
#include "emu/pc/RAT.h"
#include "emu/pc/TMB_constants.h"

#ifndef debugV //silent mode
#define PRINT(x)
#define PRINTSTRING(x)
#else //verbose mode
#define PRINT(x) std::cout << #x << ":\t" << x << std::endl;
#define PRINTSTRING(x) std::cout << #x << std::endl;
#endif


namespace emu {
  namespace pc {



TMBTester::TMBTester() {
  tmb_ = 0;
  ccb_ = 0;
  rat_ = 0;
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
  ResultTestU76chip_ = -1;
  ResultTestALCTtxrx_ = -1;
  ResultTestRATtemper_ = -1;
  ResultTestRATidCodes_ = -1;
  ResultTestRATuserCodes_ = -1;
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
//
/////////////////////////////////////////
// TMB Tests:
/////////////////////////////////////////
bool TMBTester::runAllTests() {
  (*MyOutput_) << "TMBTester: Beginning full set of TMB self-tests" << std::endl;

  int dummy;
  bool AllOK;

  reset();
  //  readreg4();

  bool bootRegOK = testBootRegister();
  dummy = ::sleep(1);
  //  bool VMEfpgaDataRegOK = testVMEfpgaDataRegister();
  //  dummy = sleep(1);
  bool DateOK = testFirmwareDate();
  dummy = ::sleep(1);
  bool TypeOK = testFirmwareType();
  dummy = ::sleep(1);
  bool VersionOK = testFirmwareVersion();
  dummy = ::sleep(1);
  bool RevCodeOK = testFirmwareRevCode();
  dummy = ::sleep(1);
  bool MezzIdOK = testMezzId();
  dummy = ::sleep(1);
  bool PROMidOK = testPROMid();
  dummy = ::sleep(1);
  bool PROMpathOK = testPROMpath();
  dummy = ::sleep(1);
  bool dsnOK = testDSN();
  dummy = ::sleep(1);
  bool adcOK = testADC();
  dummy = ::sleep(1);
  bool is3d3444OK = test3d3444();
  dummy = ::sleep(1);
  //  bool isALCTtxrxOK = testALCTtxrx();
  //  dummy = sleep(1);
  bool isRATtemperOK = testRATtemper();
  dummy = ::sleep(1);
  bool isRATidCodesOK = testRATidCodes();
  dummy = ::sleep(1);
  bool isRATuserCodesOK = testRATuserCodes();
  dummy = ::sleep(1);
  bool isU76OK = testU76chip();

  (*MyOutput_) << "TMBTester Full Test Summary:" << std::endl;

  messageOK("Boot Register............. ",bootRegOK);
  //  messageOK("VME FPGA Data Register.... ",VMEfpgaDataRegOK);
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
  //  messageOK("ALCT tx/rx cables (RAT)... ",isALCTtxrxOK);
  messageOK("RAT temperature........... ",isRATtemperOK);
  messageOK("RAT ID codes.............. ",isRATidCodesOK);
  messageOK("RAT User codes............ ",isRATuserCodesOK);
  messageOK("TMB U76 bus-hold chip..... ",isU76OK);

  AllOK = (bootRegOK &&
	   //	   VMEfpgaDataRegOK &&
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
	   //           isALCTtxrxOK &&
	   isRATtemperOK &&
	   isRATidCodesOK &&
	   isRATuserCodesOK &&
	   isU76OK);

  (*MyOutput_) << "------------------------------" << std::endl;
  messageOK("TMB/RAT internal tests.... ",AllOK);

  return AllOK;
}
//
bool TMBTester::testBootRegister() {
  (*MyOutput_) << "TMBTester: testing Boot Register" << std::endl;
  //
  bool testOK = false;
  int dummy;  
  //
  unsigned short int BootData;
  dummy = tmb_->tmb_get_boot_reg(&BootData);
  //  (*MyOutput_) << "Initial boot contents = " << std::hex << BootData << std::endl;
  //
  unsigned short int write_data, read_data;
  int err_reg = 0;
  //
  // walk through the 16 bits on the register
  for (int ibit=0; ibit<16; ibit++) {
    write_data = 0x1 << ibit;
    dummy = tmb_->tmb_set_boot_reg(write_data);
    //
    dummy = tmb_->tmb_get_boot_reg(&read_data);
    //
    // Remove the read-only bits:
    read_data &= 0x3FFF;
    write_data &= 0x3FFF;
    //
    if ( !compareValues("bootreg bit",read_data,write_data,true) ) {
      err_reg++;
    }
    // Need to pause in between to allow for reloading after hard resets 
    // to various item...
    ::sleep(1);
  }
  //
  // Restore boot contents
  dummy = tmb_->tmb_set_boot_reg(BootData);
  dummy = tmb_->tmb_get_boot_reg(&read_data);   //check for FPGA final state
  //  (*MyOutput_) << "Final Boot Contents = " << std::hex << read_data << std::endl;    
  //
  testOK = compareValues("Number of boot register errors",err_reg,0,true);  
  messageOK("Boot Register",testOK);
  //dummy = sleep(3);
  //
  ResultTestBootRegister_ = testOK ;
  //
  return testOK; 
}
//
bool TMBTester::testHardReset(){
  (*MyOutput_) << "TMBTester: checking hard reset TMB via boot register" << std::endl;
  (*MyOutput_) << "NOTE:  TEST NOT NEEDED, as we hard reset by the CCB" << std::endl; 
  //
  bool testOK = false;
  int dummy;
  //
  unsigned short int BootData;
  dummy = tmb_->tmb_get_boot_reg(&BootData);
  //  (*MyOutput_) << "Initial boot contents = " << std::hex << BootData << std::endl;
  //
  unsigned short int write_data, read_data;
  //
  write_data = 0x0200;
  dummy = tmb_->tmb_set_boot_reg(write_data);   //assert hard reset
  //
  dummy = tmb_->tmb_get_boot_reg(&read_data);   //check for FPGA not ready
  read_data &= 0x7FFF;                          // remove tdo
  //
  bool FPGAnotReady = 
    compareValues("Hard reset TMB FPGA not ready",read_data,0x0200,true);
  //
  write_data = 0x0000;
  dummy = tmb_->tmb_set_boot_reg(write_data);   //de-assert hard reset
  //
  (*MyOutput_) << "waiting for TMB to reload..." << std::endl;
  dummy = ::sleep(5);                             
  //
  dummy = tmb_->tmb_get_boot_reg(&read_data);   //check for FPGA not ready
  read_data &= 0x4000;                          // remove bits beside "FPGA ready"
  //
  bool FPGAReady = compareValues("Hard reset TMB FPGA ready",read_data,0x4000,true);
  //
  // Restore boot contents
  dummy = tmb_->tmb_set_boot_reg(BootData);
  dummy = tmb_->tmb_get_boot_reg(&read_data);   //check for FPGA final state
  //  (*MyOutput_) << "Final Boot Contents = " << std::hex << read_data << std::endl;    
  //
  testOK = (FPGAnotReady &&
	    FPGAReady);
  return testOK;
}
//
bool TMBTester::testVMEfpgaDataRegister(){
  //
  // NOTE: This test is redundant with the 3d3444 test....       
  //
  (*MyOutput_) << "TMBTester: testing VME FPGA Data Register" << std::endl;
  bool testOK = false;
  //
  //std::cout << "Read..." << std::endl;
  //
  // Get current status:
  int vme_cfg = tmb_->ReadRegister(vme_ddd0_adr);
  //
  //std::cout << "Done..." << std::endl;
  //
  int write_data,read_data;
  bool tempBool;
  //
  int register_error = 0;
  //
  for (int i=0; i<=15; i++) {
    //
    //    std::cout << "Looping " << std::endl ;
    //
    write_data = 1 << i;
    //
    tmb_->WriteRegister(vme_ddd0_adr,write_data);  //write walking 1
    read_data = tmb_->ReadRegister(0);               //read base to purge bit3 buffers
    read_data = tmb_->ReadRegister(vme_ddd0_adr);  //read walking 1
    //
    tempBool = compareValues("Register value",read_data,write_data,true);
    //
    if (!tempBool) register_error++;
  }
  //
  //restore data register...
  tmb_->WriteRegister(vme_ddd0_adr,vme_cfg);
  //
  write_data = 0x001A;       // turn on cylons
  tmb_->WriteRegister(ccb_cfg_adr,write_data);
  //
  testOK = compareValues("Number of VME FPGA data reg errors",register_error,0,true);
  messageOK("VME FPGA data register",testOK);
  //int dummy = sleep(3);
  //
  ResultTestVMEfpgaDataRegister_ = testOK;
  //
  return testOK;
}
//
bool TMBTester::testFirmwareSlot(){
  (*MyOutput_) << "TMBTester::testFirmwareSlot() NOT YET IMPLEMENTED" << std::endl;
  //
  bool testOK = false;  
  return testOK;
}
//
bool TMBTester::testFirmwareDate() {
  //
  (*MyOutput_) << "TMBTester: testing Firmware date" << std::endl;
  //
  // read the registers:
  tmb_->FirmwareDate();
  tmb_->FirmwareYear();
  //
  // get the read values:
  int day   = tmb_->GetReadTmbFirmwareDay();
  int month = tmb_->GetReadTmbFirmwareMonth();
  int year  = tmb_->GetReadTmbFirmwareYear();
  //
  (*MyOutput_) << "Firmware day.month.year = " << std::hex 
	       << day << "." << month << "." << year << std::endl;
  //
  int expected_day   = tmb_->GetExpectedTmbFirmwareDay();
  int expected_month = tmb_->GetExpectedTmbFirmwareMonth();
  int expected_year  = tmb_->GetExpectedTmbFirmwareYear();
  //
  bool dayOK   = compareValues("Firmware Day"  ,day  ,expected_day  ,true);
  bool monthOK = compareValues("Firmware Month",month,expected_month,true);
  bool yearOK  = compareValues("Firmware Year" ,year ,expected_year ,true);
  //
  bool testOK = (dayOK && monthOK && yearOK);
  //
  messageOK("Firmware Date",testOK);
  //int dummy = sleep(3);
  //
  ResultTestFirmwareDate_ = testOK;
  //
  return testOK; 
}
//
bool TMBTester::testFirmwareType() {
  //
  (*MyOutput_) << "TMBTester: testing Firmware Type" << std::endl;
  //
  // read the registers:
  tmb_->FirmwareVersion();
  //
  // get the read values:
  int type = tmb_->GetReadTmbFirmwareType();
  //
  int expected_type = tmb_->GetExpectedTmbFirmwareType();
  //
  bool testOK = compareValues("Firmware Type",type,expected_type,true);
  //
  messageOK("Firmware Type",testOK);
  //int dummy = sleep(3);
  //
  ResultTestFirmwareType_ = testOK;
  //
  return testOK;
}
//
bool TMBTester::testFirmwareVersion() {
  //
  (*MyOutput_) << "TMBTester: testing Firmware Version" << std::endl;
  //
  // read the registers:
  tmb_->FirmwareVersion();
  //
  // get the read values:
  int version = tmb_->GetReadTmbFirmwareVersion();
  //
  int expected_version = tmb_->GetExpectedTmbFirmwareVersion();
  //
  bool testOK = compareValues("Firmware Version",version,expected_version,true);
  //
  messageOK("Firmware Version",testOK);
  //int dummy = sleep(3);
  //
  ResultTestFirmwareVersion_ = testOK;
  //
  return testOK;
}
//
bool TMBTester::testFirmwareRevCode(){
  //
  (*MyOutput_) << "TMBTester: testing Firmware Revision Code" << std::endl;
  //
  // read the registers:
  tmb_->FirmwareRevCode();
  //
  // get the read values:
  int revcode = tmb_->GetReadTmbFirmwareRevcode();
  //
  int expected_revcode = tmb_->GetExpectedTmbFirmwareRevcode();
  //
  bool testOK = compareValues("Firmware Revcode FPGA",revcode,expected_revcode,true);
  //
  messageOK("Firmware Revcode FPGA",testOK);
  //int dummy = sleep(3);
  //
  ResultTestFirmwareRevCode_ = testOK ;
  //
  return testOK;
}
//
bool TMBTester::testJTAGchain(){
  (*MyOutput_) << "TMBTester: testing User and Boot JTAG chains" << std::endl;
  //
  bool user = testJTAGchain(0);
  bool boot = testJTAGchain(1);
  //
  bool JTAGchainOK = (user && boot);
  messageOK("JTAG chains",JTAGchainOK);
  //int dummy = sleep(3);
  return JTAGchainOK;
}
//
bool TMBTester::testJTAGchain(int type){
  (*MyOutput_) << "testJTAGchain: DOES NOT WORK with firmware not in debug mode" << std::endl; 
  (*MyOutput_) << "Therefore...  TMBTester::testJTAGchain() NOT YET IMPLEMENTED" << std::endl;
  //
  bool testOK = false;  
  return testOK;
}
//
bool TMBTester::testMezzId(){
  //
  (*MyOutput_) << "TMBTester: Checking Mezzanine FPGA and PROMs ID codes" << std::endl;
  //
  const int EXPECTED_TMB_MEZZ_FPGA_IDCODE  = 0x01050093;
  const int EXPECTED_TMB_MEZZ_FPGA6_IDCODE = 0x04250093;
  const int EXPECTED_TMB_MEZZ_PROM_IDCODE  = 0x05026093;
  // newer proms have idcode X5036093,  older proms have 05026093.  We just ignore these bytes...
  const int Mezzanine_fpgaid_mask         = 0x0FFFFFFF;
  const int Mezzanine_promid_mask         = 0x0FFEFFFF;
  //
  tmb_->ReadTmbIdCodes();
  //
  int fpgaIdCode  = (tmb_->GetTMBmezzFpgaIdCode() & Mezzanine_fpgaid_mask);

  bool testOK=true;
 int hwv=tmb_->GetHardwareVersion();
 if(hwv<=1)
 {
  int prom0IdCode = (tmb_->GetTMBmezzProm0IdCode() & Mezzanine_promid_mask);
  int prom1IdCode = (tmb_->GetTMBmezzProm1IdCode() & Mezzanine_promid_mask);
  int prom2IdCode = (tmb_->GetTMBmezzProm2IdCode() & Mezzanine_promid_mask);
  int prom3IdCode = (tmb_->GetTMBmezzProm3IdCode() & Mezzanine_promid_mask);
  //
  bool testFPGAmezz  = compareValues("Mezz FPGA ID code"  ,fpgaIdCode ,EXPECTED_TMB_MEZZ_FPGA_IDCODE,true);  
  bool testPROMmezz0 = compareValues("Mezz PROM 0 ID code",prom0IdCode,EXPECTED_TMB_MEZZ_PROM_IDCODE,true);  
  bool testPROMmezz1 = compareValues("Mezz PROM 1 ID code",prom1IdCode,EXPECTED_TMB_MEZZ_PROM_IDCODE,true);  
  bool testPROMmezz2 = compareValues("Mezz PROM 2 ID code",prom2IdCode,EXPECTED_TMB_MEZZ_PROM_IDCODE,true);  
  bool testPROMmezz3 = compareValues("Mezz PROM 3 ID code",prom3IdCode,EXPECTED_TMB_MEZZ_PROM_IDCODE,true);  
  //
        testOK = (testFPGAmezz  &&
		 testPROMmezz0 &&
		 testPROMmezz1 &&
		 testPROMmezz2 &&
		 testPROMmezz3 );
  //
 }
 else if(hwv>=2)
 {
  testOK  = compareValues("Mezz FPGA ID code"  ,fpgaIdCode ,EXPECTED_TMB_MEZZ_FPGA6_IDCODE,true);  
 }
  messageOK("Mezzanine FPGA/PROM ID",testOK);
  ResultTestMezzId_ = testOK;
  //
  return testOK;
}
//
bool TMBTester::testPROMid(){
  //
  (*MyOutput_) << "TMBTester: Checking User PROM ID codes" << std::endl;
  //
  tmb_->ReadTmbIdCodes();
  //
  int userProm0IdCode = tmb_->GetTMBuserProm0IdCode();
  int userProm1IdCode = tmb_->GetTMBuserProm1IdCode();
  //
  //User PROM's could have 
  //    - ID = 0x05022093 for 256kB
  //    - ID = 0x05023093 or 0x05033093 for 512kB
  //
  // We require:  PROM's to be equal...
  bool testSameID = compareValues("ID 0 = ID 1",userProm0IdCode,userProm1IdCode,true);
  //
  // We require:  PROM's to be one of the 3, above (i.e. lsb of bytes 3 and 4 don't matter)...
  bool testPROMUserId0 = compareValues("User PROM 0 ID code",
				       (userProm0IdCode & MASK_TO_TREAT_512k_LIKE_256k),
				       PROM_ID_256k,true);  
  bool testPROMUserId1 = compareValues("User PROM 1 ID code",
				       (userProm1IdCode & MASK_TO_TREAT_512k_LIKE_256k),
				       PROM_ID_256k,true);  
  //
  bool testOK = (testSameID &&
		 testPROMUserId0 &&
		 testPROMUserId1 );
  //
  messageOK("PROM User ID",testOK);
  //int dummy = sleep(3);
  //
  ResultTestPromId_ = testOK ;
  //
  return testOK;
}
//
bool TMBTester::testPROMpath(){
  //
  (*MyOutput_) << "TMBTester: Checking User PROM Data Path" << std::endl;
  //
  int pat_expect1=0, pat_expect2=0;
  //
  bool temptest=true;
  //
  for (int iprom=0; iprom<=1; iprom++) {
    //
    tmb_->ClockOutPromProgram(iprom,10);
    //
    // **Read the data from the selected PROM **
    for (int prom_adr=0; prom_adr<=9; prom_adr++) {
      //
      int read_data = tmb_->GetClockedOutPromImage(prom_adr);
      //
      // there are two possible patterns we could get:
      if (prom_adr==0) {
	if (iprom == 0) pat_expect1 = 0xab;
	if (iprom == 1) pat_expect1 = 0xcd;
      } else if (prom_adr==9) {
	if (iprom == 0) pat_expect1 = 0xee;
	if (iprom == 1) pat_expect1 = 0xbb;
      } else {
	pat_expect1 = 1 << (prom_adr-1);
      }
      if (prom_adr==0) pat_expect2=0x59;
      if (prom_adr==1) pat_expect2=0x6f;
      if (prom_adr==2) pat_expect2=0x75;
      if (prom_adr==3) pat_expect2=0x72;
      if (prom_adr==4) pat_expect2=0x20;
      if (prom_adr==5) pat_expect2=0x6d;
      if (prom_adr==6) pat_expect2=0x6f;
      if (prom_adr==7) pat_expect2=0x74;
      if (prom_adr==8) pat_expect2=0x68;
      if (prom_adr==9) pat_expect2=0x65;
      if (read_data==pat_expect2) {
	temptest &= compareValues("data = expected",read_data,pat_expect2,true);
      } else {
	temptest &= compareValues("data = expected",read_data,pat_expect1,true);
      }
    }
  }
  bool testOK = temptest;
  //
  messageOK("PROM path",testOK);
  //int dummy = sleep(3);
  //
  ResultTestPROMPath_ = testOK;
  //
  return testOK;
}
//
bool TMBTester::testDSN(){
  //
  (*MyOutput_) << "TMBTester: Checking Digital Serial Numbers for TMB" << std::endl;
  bool tmbDSN = testDSN(0);
  //::sleep(1);
  //
  (*MyOutput_) << "TMBTester: Checking Digital Serial Numbers for Mezzanine" << std::endl;
  bool mezzanineDSN = testDSN(1);
  if(tmb_->GetHardwareVersion()>=2)
  {
     (*MyOutput_) << "TMBTester: Checking Virtex-6 FPGA's DNA" << std::endl;
     unsigned char dna[8];
     char buf[40];
     int r=tmb_->virtex6_dna(dna);
     if(r==0)
     {    sprintf(buf, "DNA (Hex) : %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\n", 
           dna[7],dna[6],dna[5],dna[4],dna[3],dna[2],dna[1],dna[0]);  
           (*MyOutput_) << buf;
     }
     else
     {
          (*MyOutput_) << "DNA readback failed!" << std::endl;
     }
  }
  //::sleep(1);
  //
  (*MyOutput_) << "TMBTester: Checking Digital Serial Numbers for RAT" << std::endl;
  bool ratDSN = testDSN(2);
  //::sleep(1);
  //
  messageOK("TMB DSN",tmbDSN);
  messageOK("Mezzanine DSN",mezzanineDSN);
  messageOK("RAT DSN",ratDSN);
  //
  bool DSNOK = (tmbDSN &&
                mezzanineDSN &&
                ratDSN);
  //
  messageOK("All Digital Serial Numbers",DSNOK);
  //::sleep(3);
  //
  ResultTestDSN_ = DSNOK ;
  //
  return DSNOK;
}
//
bool TMBTester::testDSN(int BoardType){
  //BoardType = 0 = TMB
  //          = 1 = Mezzanine
  //          = 2 = RAT

  std::bitset<64> dsn;
  //
  // get the digital serial number
  dsn = tmb_->dsnRead(BoardType);
  (*MyOutput_) << "DSN readback (Hex): " << std::hex << dsn.to_ulong() << std::dec << std::endl;
  //
  // compute the CRC
  int crc = dowCRC(dsn);
  //
  //get the 8 most significant bits (from their LSB) to compare with CRC
  int dsntocompare = 0;
  int value;
  for (int bit=63; bit>=56; bit--){
    value = dsntocompare << 1;
    dsntocompare = value | dsn[bit];
  }
  //
  bool crcEqualDSN = compareValues("CRC equal dsn[56-63]",crc,dsntocompare,true);
  bool crcZero = compareValues("CRC value ",crc,0,false);
  //
  bool DSNOK = (crcZero &&
		crcEqualDSN);
  return DSNOK;
}
//
bool TMBTester::testADC(){
  int hwv=tmb_->GetHardwareVersion();
  //
  (*MyOutput_) << "TMBTester: Checking ADC and status" << std::endl;
  //
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
  //
  bool v5p0OK     = compareValues("+5.0V TMB      ",v5p0    ,5.000,0.025);
  //
  // Measure average +3.314V for 9 slots on back of CRB with DVM at Bat. 904.
  // Measure average 92mV drop through backplane to 3.3V ADC value, which is 44% higher than measured at UCLA: 
  bool v3p3OK;
  if(hwv<=1)
    v3p3OK     = compareValues("+3.3V TMB      ",v3p3    ,3.222,0.025);         // if value goes below 3.135, there could be some instabilities showing up...
  else
    v3p3OK     = compareValues("+3.3V TMB      ",v3p3    ,3.250,0.150);         // if value goes below 3.135, there could be some instabilities showing up...
  //
  float vcore_noload = 1.525;                                                      // Puts Vcore=1.500 at 2.5amp load, midrange of 1.425-to-1.575
  float vcore_expect = vcore_noload - a1p5core*0.010;                              //Expect Vcore-Acore*.010ohms.  At Acore nom=0.870A, Vcore=1.516
  bool v1p5coreOK;
  if(hwv<=1)
     v1p5coreOK = compareValues("+1.5V core     ",v1p5core,vcore_expect,0.0015); // most critical value on TMB
  else
     v1p5coreOK = compareValues("+1.5V core     ",v1p5core,1.5,0.03);  // for Virtex6 FPGA on OTMB, this is relaxed
  
  //
  bool v1p5ttOK   = compareValues("+1.5V TT       ",v1p5tt  ,1.493,0.025);
  bool v1p0OK     = compareValues("+1.0V TT       ",v1p0    ,(hwv<=1)?1.005:1.000,0.005);
  bool v1p8ratOK  = (hwv<=1)?compareValues("+1.8V RAT core ",v1p8rat ,1.805,0.025):true;  // temporarily disable RAT voltage check for OTMB 
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
  //  int TMBtempPCB = 
  tmb_->ReadTMBtempPCB();  
  //  int TMBtempFPGA = 
  tmb_->ReadTMBtempFPGA();  
  //  int RATtempPCB = 
  rat_->ReadRATtempPCB(); 
  //  int RATtempHeatSink = 
  rat_->ReadRATtempHSink(); 

  // ** Get TMB and RAT critical temperature settings **
  //  int TMBtCritPCB = 
  tmb_->ReadTMBtCritPCB();  
  //  int TMBtCritFPGA = 
  tmb_->ReadTMBtCritFPGA();  
  //  int RATtCritPCB = 
  rat_->ReadRATtCritPCB(); 
  //  int RATtCritHeatSink = 
  rat_->ReadRATtCritHSink(); 

  testOK = (voltageDisc&&
	    v5p0OK     &&
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
  //
  (*MyOutput_) << "TMBTester: Verifying 3d3444 operation" << std::endl;
  //
  unsigned long int devices[4] = {vme_ddd0_adr,vme_ddd1_adr,vme_ddd2_adr,rat_3d_delays_adr};
  //
  bool tempbool = true;  
  //
  for (unsigned device=0; device<4; device++) {  //loop through the DDD devices available
    //
    unsigned long vme_address = devices[device];
    //
    int initial_data = tmb_->ReadRegister(vme_address);
    //
    for (unsigned short int channel=0; channel<16; channel+=4) {  //loop through channels on the DDD chip
      //
      for (unsigned short int ddd_delay=0; ddd_delay<16; ddd_delay++ ) {  // loop through all possible delay channels
	//
	int write_data = (ddd_delay << channel);
	tmb_->WriteRegister(vme_address,write_data);
	int read_data = tmb_->ReadRegister(vme_address); 
	tempbool &= compareValues("delay values ",read_data,write_data,true);
      }
    }
    //
    tmb_->WriteRegister(vme_address,initial_data);
  }
  //
  bool testOK = tempbool;
  //
  messageOK("3d3444 Verification",testOK);
  //int dummy = sleep(3);
  //
  ResultTest3d3444_ = testOK;
  //
  return testOK;
}
//
bool TMBTester::testALCTtxrx(){
  //
  (*MyOutput_) << "TMBTester: Testing ALCT tx/rx cables connection to RAT" << std::endl;
  //
  rat_->ReadRatUser1();
  //
  bool testTx = compareValues("ALCT tx",rat_->GetRatTxOk(),0x1,true);
  bool testRx = compareValues("ALCT rx",rat_->GetRatRxOk(),0x1,true);
  //
  bool testOK = (testTx &&
		 testRx );
  //
  messageOK("ALCT tx/rx cables plugged in",testOK);
  //
  ResultTestALCTtxrx_ = testOK ;
  return testOK;
}

bool TMBTester::testRATtemper(){
  //
  (*MyOutput_) << "TMBTester: Testing RAT temperature threshold bit" << std::endl;
  //
  rat_->ReadRatUser1();
  //
  bool testOK = compareValues("RAT Temperature bit",rat_->GetRatCriticalTempBit(),0x1,true);
  //
  messageOK("RAT Temperature bit",testOK);
  //
  ResultTestRATtemper_ = testOK ;
  //
  return testOK;
}

bool TMBTester::testRATidCodes(){
  //
  (*MyOutput_) << "TMBTester: Testing RAT ID codes" << std::endl;
  //
  const int EXPECTED_RAT_FPGA_IDCODE = 0x20a10093;
  const int EXPECTED_RAT_PROM_IDCODE = 0x05024093;
  //
  rat_->ReadRatIdCode();
  //
  int fpgaidcode = rat_->GetRatIdCode(ChipLocationRatFpga);
  //
  bool FPGAidOK = compareValues("RAT FPGA ID code",fpgaidcode,EXPECTED_RAT_FPGA_IDCODE,true);
  //
  //
  // RAT PROM id can be 5034093 or 5024093....
  int promidcode = rat_->GetRatIdCode(ChipLocationRatProm) & 0xfffeffff;
  //
  bool PROMidOK = compareValues("RAT PROM ID code",promidcode,EXPECTED_RAT_PROM_IDCODE,true);
  //
  bool testOK = (FPGAidOK &&
		 PROMidOK );
  //
  messageOK("RAT ID codes",testOK);
  //
  ResultTestRATidCodes_ = testOK ;
  return testOK;
}

bool TMBTester::testRATuserCodes(){
  //
  (*MyOutput_) << "TMBTester: Testing RAT User codes" << std::endl;
  //
  rat_->ReadRatUserCode();
  //
  int day   = rat_->GetReadRatFirmwareDay();
  int month = rat_->GetReadRatFirmwareMonth();
  int year  = rat_->GetReadRatFirmwareYear();
  //
  int expected_day   = tmb_->GetExpectedRatFirmwareDay();
  int expected_month = tmb_->GetExpectedRatFirmwareMonth();
  int expected_year  = tmb_->GetExpectedRatFirmwareYear();
  //
  bool RatFirmwareDayOK   = compareValues("RAT Firmware Day"  ,day  ,expected_day  ,true);
  bool RatFirmwareMonthOK = compareValues("RAT Firmware Month",month,expected_month,true);
  bool RatFirmwareYearOK  = compareValues("RAT Firmware Year" ,year ,expected_year ,true);
  //
  bool testOK = (RatFirmwareDayOK   &&
		 RatFirmwareMonthOK &&
		 RatFirmwareYearOK  );
  //
  messageOK("RAT Firmware Date (User Code)",testOK);
  //
  ResultTestRATuserCodes_ = testOK ;
  return testOK;
}
//
bool TMBTester::testU76chip(){
  //
  (*MyOutput_) << "TMBTester: Testing TMB U76 bus-hold chip" << std::endl;  
  //
  short unsigned int initial_boot;
  int dummy = tmb_->tmb_get_boot_reg(&initial_boot);
  //
  (*MyOutput_) << "Initial Boot Contents = " << std::hex << initial_boot << std::endl;
  //
  //make sure firmware_type is normal
  if (!testFirmwareType()) return false;
  //
  //make sure we have the right types of chips on the Mezzanine
  if (!testMezzId()) return false;
  //
  short unsigned int after_jtag;
  dummy = tmb_->tmb_get_boot_reg(&after_jtag);
  //
  (*MyOutput_) << "Boot Contents after JTAG = " << std::hex << after_jtag << std::endl;
  //
  //put Boot register to power-up state
  unsigned short int power_up = 0xc000;
  tmb_->tmb_set_boot_reg(power_up);
  //
  short unsigned int after_power_up;
  dummy = tmb_->tmb_get_boot_reg(&after_power_up);
  //
  (*MyOutput_) << "Boot Contents after putting back to power up state= " 
	       << std::hex << after_power_up << std::endl;
  //
  bool testJTAGpattern[7];
  int pattern_expect;
  int read_data;
  unsigned short int after_hard_reset;
  bool testOK = true;
  for (int bit=6; bit>=0; bit--) {
    pattern_expect = 1 << bit;                   // write walking 1
    //
    //    (*MyOutput_) << "Write = " << std::hex << pattern_expect << std::endl;
    //
    tmb_->WriteRegister(vme_usr_jtag_adr,pattern_expect);
    //
    tmb_->tmb_set_boot_reg(after_power_up | TMB_HARD_RESET);             
    tmb_->tmb_set_boot_reg(after_power_up);
    ::sleep(1);                                  //give TMB time to reload
    //
    dummy = tmb_->tmb_get_boot_reg(&after_hard_reset);    
    //    (*MyOutput_) << "After hard reset, boot register = " << after_hard_reset << std::endl;
    //
    read_data = tmb_->ReadRegister(vme_usr_jtag_adr);
    read_data &= 0x7f;                                 // mask out lowest 7 bits
    testJTAGpattern[bit] = compareValues("Test pattern",read_data,pattern_expect,true);
    //
    testOK &= testJTAGpattern[bit];
  }
  //
  messageOK("TMB U76 bus-hold chip",testOK);
  //
  ResultTestU76chip_ = testOK ;
  return testOK;
}
//
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
		<< " within "<< fractolerance
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
  rat_->ReadRatUser1();
  int parity_used = rat_->GetRatParityOdd();   // = 1 if odd, 0 if even
  //
  int data,parity_ok;
  int counter,bxn;
  int evenodd;
  int total_counts[8] = {};
  int parity_match[8] = {};
  //
  for (int event=0;event<100;event++) {
    // get data and parity_ok for this RPC event:
    rat_->ReadRatUser1();
    data = rat_->GetRatRpcPdata(rpc);
    parity_ok = rat_->GetRatRpcParityOk(rpc);
    //
    // count number of 1's
    counter = 0;
    for (int bit=0;bit<19;bit++) 
      counter += (data >> bit) & 0x1;
	
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
  //
  const float rate = 40e+6;    //data coming in at 40MHz
  const float numbits = 15;    //RPC sending 12 bits of random data+2bxn+parity bit
  const int sleepval = 60;     //seconds between each read
  //
  float numberRpcs = 0.0;
  //
  if (rpc == 0 || rpc == 1) numberRpcs = 1.;
  if (rpc == 2) numberRpcs = 2;
  //
  rat_->reset_parity_error_counter();
  //
  float difference = 0;
  int errors = 0;
  float BER;
  //
  int countzeros;
  int datazero = 0;
  int rpcData[2];
  //
  while (errors == 0) {
    ::sleep(sleepval);
    //
    //** Check that the data being sent isn't all 0's or all 1's...**
    rat_->ReadRatUser1();
    for (int i=0; i<2; i++)
      rpcData[i] = rat_->GetRatRpcPdata(i);
    //
    countzeros = 0;
    for (int i=0;i<16;i++) {
      if (rpc == 2) {
	countzeros += (rpcData[0] >> i) & 0x1;
	countzeros += (rpcData[1] >> i) & 0x1;
      } else {
	countzeros += (rpcData[rpc] >> i) & 0x1;
      }
    }
    //
    if ( (countzeros % 16) == 0 ) {
      datazero++; 
    } else {
      datazero = 0;
    }
    //    
    if (rpc == 2) {
      errors += rat_->GetRatRpcParityErrorCounter(0);
      errors += rat_->GetRatRpcParityErrorCounter(1);
    } else {
      errors += rat_->GetRatRpcParityErrorCounter(rpc);
    }
    //
    difference += (float) sleepval;
    //
    (*MyOutput_) << "Error count = " << std::dec << errors 
		 << " in " << difference << " seconds..."
		 << std::endl;
    //
    BER = 1./(numberRpcs * rate * numbits * difference);
    //
    if (datazero < 20) {
      (*MyOutput_) << "Bit Error Rate = " << BER << std::endl; 
    } else {
      (*MyOutput_) << "RPC's are sending bad data... STOP" << std::endl; 
      break;
    }
    //
  }
  //
  return;
}
//
/////////////////////////////////////////
// END Some useful methods
/////////////////////////////////////////


///////////////////////////////////////////////
//  The following belong in CrateTiming.cpp
///////////////////////////////////////////////

//////////////////////////////////////////////////
//  END  The following belong in CrateTiming.cpp
//////////////////////////////////////////////////


////////////////////////////////////////////////
// The following belong in RAT.cc
////////////////////////////////////////////////
////////////////////////////////////////////////
// END The following belong in RAT.cc
////////////////////////////////////////////////

///////////////////////////////////////////////
//  The following belong in TMB.cc
///////////////////////////////////////////////
///////////////////////////////////////////////
//  END: The following belong in TMB.cc
///////////////////////////////////////////////



} // namespace emu::pc  
} // namespace emu  

