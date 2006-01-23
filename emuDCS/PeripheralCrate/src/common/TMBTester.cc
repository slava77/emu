#include<iostream>
#include<stdio.h>
#include<string>
#include<unistd.h> // for sleep()
#include "TMBTester.h"
#include "TMB.h"
#include "CCB.h"
#include "TMB_constants.h"
#include "JTAG_constants.h"

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
}

TMBTester::~TMBTester(){}
//

/////////////////////////////////////////
// General members:
/////////////////////////////////////////
void TMBTester::readreg4(){
  std::cout << "Basic test of TMB communication" << std::endl;
  int regtoread = 4;
  std::cout << "Getting date from register " << regtoread << std::endl;
  int reg = tmb_->ReadRegister(regtoread);

}

void TMBTester::reset() {
  std::cout << "TMBTester: Hard reset through CCB" << std::endl;
  if ( ccb_ ) {
    ccb_->hardReset();  
  } else {
    std::cout << "No CCB defined" << std::endl;
  }
}

bool TMBTester::runAllTests() {
  std::cout << "TMBTester: Beginning full set of TMB self-tests" << std::endl;

  bool AllOK;

  reset();
  //  readreg4();

  bool bootRegOK = testBootRegister();
  bool VMEfpgaDataRegOK = testVMEfpgaDataRegister();
  bool SlotOK = testFirmwareSlot();
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

  std::cout << "TMBTester Full Test Summary:" << std::endl;

  messageOK("Boot Register............. ",bootRegOK);
  messageOK("VME FPGA Data Register.... ",VMEfpgaDataRegOK);
  messageOK("TMB Slot.................. ",SlotOK);
  messageOK("TMB Firmware date......... ",DateOK);
  messageOK("TMB Firmware type......... ",TypeOK);
  messageOK("TMB Firmware Version...... ",VersionOK);
  messageOK("TMB Firmware Revision Code ",RevCodeOK);
  messageOK("Mezzanine ID.............. ",MezzIdOK);
  messageOK("PROM ID................... ",PROMidOK);
  messageOK("PROM Path................. ",PROMpathOK);
  messageOK("Digital Serial Numbers.... ",dsnOK);
  messageOK("Voltages (ADC)............ ",adcOK);
  messageOK("3d3444 verification....... ",is3d3444OK);

  AllOK = (bootRegOK &&
	   VMEfpgaDataRegOK &&
           SlotOK &&
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
  std::cout << "TMBTester: testing Boot Register" << std::endl;

  bool testOK = false;
  int dummy;  
  
  unsigned short int BootData;
  dummy = tmb_->tmb_get_boot_reg(&BootData);
  //  std::cout << "Initial boot contents = " << std::hex << BootData << std::endl;

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
  //  std::cout << "Final Boot Contents = " << std::hex << read_data << std::endl;    

  testOK = compareValues("Number of boot register errors",err_reg,0,true);  
  messageOK("Boot Register",testOK);
  dummy = sleep(3);
  return testOK; 
}


bool TMBTester::testHardReset(){
  std::cout << "TMBTester: checking hard reset TMB via boot register" << std::endl;
  std::cout << "NOTE:  TEST NOT NEEDED, as we hard reset by the CCB" << std::endl; 

  bool testOK = false;
  int dummy;

  unsigned short int BootData;
  dummy = tmb_->tmb_get_boot_reg(&BootData);
  //  std::cout << "Initial boot contents = " << std::hex << BootData << std::endl;

  unsigned short int write_data, read_data;

  write_data = 0x0200;
  dummy = tmb_->tmb_set_boot_reg(write_data);   //assert hard reset

  dummy = tmb_->tmb_get_boot_reg(&read_data);   //check for FPGA not ready
  read_data &= 0x7FFF;                          // remove tdo

  bool FPGAnotReady = 
    compareValues("Hard reset TMB FPGA not ready",read_data,0x0200,true);

  
  write_data = 0x0000;
  dummy = tmb_->tmb_set_boot_reg(write_data);   //de-assert hard reset

  std::cout << "waiting for TMB to reload..." << std::endl;
  dummy = sleep(5);                             

  dummy = tmb_->tmb_get_boot_reg(&read_data);   //check for FPGA not ready
  read_data &= 0x4000;                          // remove bits beside "FPGA ready"

  bool FPGAReady = compareValues("Hard reset TMB FPGA ready",read_data,0x4000,true);

  // Restore boot contents
  dummy = tmb_->tmb_set_boot_reg(BootData);
  dummy = tmb_->tmb_get_boot_reg(&read_data);   //check for FPGA final state
  //  std::cout << "Final Boot Contents = " << std::hex << read_data << std::endl;    

  testOK = (FPGAnotReady &&
	    FPGAReady);
  return testOK;
}


bool TMBTester::testVMEfpgaDataRegister(){
  std::cout << "TMBTester: testing VME FPGA Data Register" << std::endl;
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
  std::cout << "TMBTester: testing TMB slot number" << std::endl;
  int firmwareData = tmb_->FirmwareVersion();
  int slot = (firmwareData>>8) & 0xf;

  std::cout << "TMBTester: TMB Slot (counting from 0) = " 
	    << std::dec << slot << std::endl;

  //VME counts from 0, xml file counts from 1:
  int slotToCompare = ( (*TMBslot) - 1 ) & 0xf;  //only 4 bits assigned to this number...

  bool testOK = compareValues("TMB slot",slot,slotToCompare,true);
  messageOK("TMB slot",testOK);
  int dummy = sleep(3);
  return testOK;
}


bool TMBTester::testFirmwareDate() {
  std::cout << "TMBTester: testing Firmware date" << std::endl;

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
  std::cout << "TMBTester: testing Firmware Type" << std::endl;
  bool TypeNormal=false;
  bool TypeDebug=false;

  int firmwareData = tmb_->FirmwareVersion();
  int type = firmwareData & 0xf;

  TypeNormal = compareValues("Firmware Normal",type,0xC,true);
  if (!TypeNormal){
    TypeDebug = compareValues("CAUTION Firmware Debug",type,0xD,true);    
  }
  if (!TypeNormal && !TypeDebug ){
    std::cout << 
      "What kind of Firmware is this? Firmware = " << type << std::endl;
  }
  messageOK("Firmware Type Normal",TypeNormal);
  int dummy = sleep(3);
  return TypeNormal;
}


bool TMBTester::testFirmwareVersion() {
  std::cout << "TMBTester: testing Firmware Version" << std::endl;
  int firmwareData = tmb_->FirmwareVersion();
  int version = (firmwareData>>4) & 0xf;

  bool testOK = compareValues("Firmware Version",version,0xE,true);
  messageOK("Firmware Version",testOK);
  int dummy = sleep(3);
  return testOK;
}


bool TMBTester::testFirmwareRevCode(){
  std::cout << "TMBTester: testing Firmware Revision Code" << std::endl;

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
  std::cout << "TMBTester: testing User and Boot JTAG chains" << std::endl;

  //  bool user = testJTAGchain(0);
  bool user = false;
  bool boot = testJTAGchain(1);

  bool JTAGchainOK = (user && boot);
  messageOK("JTAG chains",JTAGchainOK);
  int dummy = sleep(3);
  return JTAGchainOK;
}

bool TMBTester::testJTAGchain(int type){
  std::cout << "testJTAGchain: DOES NOT WORK with firmware not in debug mode" << std::endl; 

  bool testOK = false;  
  int dummy;

  //Put boot register to power-up state:
  dummy = tmb_->tmb_set_boot_reg(0);

  //  int source_adr = UserOrBootJTAG(type);

  short unsigned int write_data, read_data;
  int write_pattern;

  // Select the FPGA JTAG chain
  int FPGAchain = 0x000C;

  int gpio_adr = 0x000028;

  bool tests[5];

  //loop over transmit bits... 
  for ( int itx=0; itx<=4; itx++) {  //step through tdi,tms,tck,sel0,sel1

    //write the walking 1 to the JTAG chain...
    write_pattern = 0x1 << itx;
    write_data = write_pattern | (FPGAchain << 3) | (0x1 << 7);
    //    std::cout << "write_data into boot reg = " << std::hex << write_data << std::endl;
    dummy = tmb_->tmb_set_boot_reg(write_data);
    //    tmb_->WriteRegister(source_adr,write_data);

    //copy the tdi to the tdo through gp_io0...
    read_data = tmb_->ReadRegister(gpio_adr);
    write_data = (read_data >> 1) & 0x1;     //get tdi on gp_io1
    tmb_->WriteRegister(gpio_adr,write_data);  //send it back out on gp_io0

    //Read FPGA chain...
    read_data = (tmb_->ReadRegister(gpio_adr) & 0xF);

    if (itx==0 && read_data==0xF) {
      std::cout << "JTAGchain:  Disconnect JTAG cable" << std::endl;
      pause();
    }

    int pat_expect = 0;
    
    switch (itx) {
    case 0:
      pat_expect = 0x3;
      break;
    case 1:
    case 2:
      pat_expect = write_pattern << 1;
    case 3:
    case 4:
      pat_expect = 0xF;
    }

    if (type == 1) {
      tests[itx] = compareValues("Boot JTAG Chain",read_data,pat_expect,true);
    } else {
      tests[itx] = compareValues("User JTAG Chain",read_data,pat_expect,true);
    }
  dummy = sleep(2);
  }

  testOK = (tests[0] &&
	    tests[1] &&
	    tests[2] &&
	    tests[3] &&
	    tests[4] );

  return testOK;
}


bool TMBTester::testMezzId(){
  std::cout << "TMBTester: Checking Mezzanine FPGA and PROMs ID codes" << std::endl;
  std::cout << "testMezzId() WARNING not yet debugged" << std::endl;

  bool testOK = false;

  //Select FPGA Mezzanine FPGA programming JTAG chain from TMB boot register
  int ichain = 0x0004;                           //
  vme_jtag_anystate_to_rti(TMB_ADR_BOOT,ichain); //Take TAP to RTI

  int opcode;

  int reg_len = 32;  

  //Read Virtex2 FPGA (6-bit opcode) and XC18V04 PROM IDcodes (8-bit opcode)
  for (int chip_id=0; chip_id<=4; chip_id++){
    if (chip_id == 0) {
      opcode = 0x09;                  // FPGA IDcode opcode, expect v0A30093
    } else { 
      opcode = 0xFE;                  // PROM IDcode opcode
    }
    //    vme_jtag_write_ir(TMB_ADR_BOOT,ichain,chip_id,opcode);         //write the opcode to the TAP
    //    vme_jtag_write_dr(TMB_ADR_BOOT,ichain,chip_id,tdi,tdo,reg_len);//write the TDI bits
  }

  //Interpret ID code  GREG, FIGURE OUT HOW TO TO THE INTERPRET ID CODE PART
  //AFTER DOING THE VME_JTAG STUFF...
  

  messageOK("Mezzanine ID",testOK);
  int dummy = sleep(3);
  return testOK;
}


bool TMBTester::testPROMid(){
  std::cout << "TMBTester: Checking User PROM ID codes" << std::endl;

  std::cout << 
   "TMBTester: testPROMid() NOT YET IMPLEMENTED" 
     << std::endl;

  bool testOK = false;
  messageOK("PROM ID",testOK);
  int dummy = sleep(3);
  return testOK;
}


bool TMBTester::testPROMpath(){
  std::cout << "TMBTester: Checking User PROM Data Path" << std::endl;

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

  std::cout << "TMBTester: Checking Digital Serial Numbers for TMB" 
	    << std::endl;
  bool tmbDSN = testDSN(0);
  dummy=sleep(1);

  std::cout << "TMBTester: Checking Digital Serial Numbers for Mezzanine" 
	    << std::endl;
  bool mezzanineDSN = testDSN(1);
  dummy=sleep(1);

  std::cout << "TMBTester: Checking Digital Serial Numbers for RAT" 
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
  dsn = dsnRead(BoardType);

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
  std::cout << "TMBTester: Checking ADC and status" << std::endl;

  bool testOK = false;
  int dummy;

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

  ADCvoltages(adc_voltage);

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
  bool a5p0OK     = compareValues("+5.0A TMB      ",a5p0    ,0.245,atol);
  bool a3p3OK     = compareValues("+3.3A TMB      ",a3p3    ,1.260,atol);
  bool a1p5coreOK = compareValues("+1.5A TMB Core ",a1p5core,0.095,atol);
  bool a1p5ttOK   = compareValues("+1.5A TT       ",a1p5tt  ,0.030,atol*1.5);
  bool a1p8ratOK  = compareValues("+1.8A RAT Core ",a1p8rat ,0.030,atol*5.0);

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
	    vrefOK     &&
	    a5p0OK     &&
	    a3p3OK     &&
	    a1p5coreOK &&
	    a1p5ttOK   &&
	    a1p8ratOK  );//&&
	    //	    tlocalOK   &&
	    //	    tremoteOK  );

  messageOK("ADC voltages and temperatures",testOK);
  dummy = sleep(3);
  return testOK;
}


bool TMBTester::test3d3444(){
  std::cout << "TMBTester: Verifying 3d3444 operation" << std::endl;
  bool testOK = false;

  bool tempbool = true;  

  int device; 
  
  unsigned short int ddd_delay;
  int initial_data;
  int delay_data;

  for (device=0; device<=11; device++){

    initial_data=tmb_read_delays(device);   //initial value of delays

    for (ddd_delay=0; ddd_delay<=15; ddd_delay++ ) {
      //      tmb_->tmb_clk_delays(ddd_delay,device);              
      rat_clk_delays(ddd_delay,device);              
      delay_data=tmb_read_delays(device);
      tempbool &= compareValues("delay values ",delay_data,ddd_delay,true);
    }

    tmb_->tmb_clk_delays(initial_data,device);              
  }
  
  testOK = tempbool;

  messageOK("3d3444 Verification",testOK);
  int dummy = sleep(3);
  return testOK;
}


void TMBTester::RatUser1JTAG(){



  return;
}

void TMBTester::RatTmbDelayScan(){
  //** Find optimal rpc_clock delay = phasing between RAT board and TMB **

  //some useful addresses: RAT2005 ver.2
  //const unsigned long int adr_rpc_inj     = 0x0000bc;
  //const unsigned long int vme_ratctrl_adr = 0x00001e;
  //const unsigned long int rpc_cfg_adr     = 0x0000b6;
  //const unsigned long int rpc_rdata_adr   = 0x0000b8;

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
  //    std::cout << "rpc_rdata_expect[" << i << "] = ";
  //    for (bit=0; bit<=(nbits-1); bit++) {
  //      std::cout << rpc_rdata_expect[i][bit] << " ";
  //    }
  //    std::cout << std::endl;
  //  }

  //enable RAT input into TMB...
  read_data = tmb_->ReadRegister(rpc_inj_adr);
  write_data = read_data | 0x0001;
  tmb_->WriteRegister(rpc_inj_adr,write_data);

  //Initial delay values:
  int rpc_delay_default = tmb_read_delays(8);

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
      //NOTE:  The following should be replaced by tmb_->tmb_clk_delays(value,setting)
      //once new_clk_delays has been updated to the following, which includes the RPC
      rat_clk_delays(ddd_delay,8);

      // ** read RAT 80MHz demux registers**
      for (irat=0; irat<=3; irat++) {
	read_data = tmb_->ReadRegister(rpc_cfg_adr);
	read_data &= 0xf9ff;                   //zero out old RAT bank
	write_data = read_data | (irat << 9);  //select RAT RAM bank
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
      //	std::cout << "rpc_data_array[" << i << "] = ";
      //	for (bit=0; bit<=(nbits-1); bit++) {
      //	  std::cout << rpc_data_array[i][bit] << " ";
      //	}
      //	std::cout << std::endl;
      //      }

      rpc_bad[ddd_delay] += count_bad;

    }
  }

  // Put RPC delay back to initial values:
  std::cout << "Putting delay values back to " << rpc_delay_default << std::endl;
  rat_clk_delays(rpc_delay_default,8);

  // ** Take TMB out of sync mode **
  write_data = 0x0002;
  tmb_->WriteRegister(vme_ratctrl_adr,write_data);

  int rpc_delay;

  // ** print out results **
  std::cout << "rpc_delay   bad data count" << std::endl;
  std::cout << "---------   --------------" << std::endl;
  for (rpc_delay = 0; rpc_delay <=15; rpc_delay++) {
    std::cout << "    " << rpc_delay 
	      << "           " << rpc_bad[rpc_delay] 
	      <<std::endl;
  }

  return;
}

/////////////////////////////////////////
// Functions needed to implement tests:
/////////////////////////////////////////
bool TMBTester::compareValues(std::string TypeOfTest, 
                              int testval, 
                              int compareval,
			      bool equal) {

// test if "testval" is equivalent to the expected value: "compareval"
// return depends on if you wanted them to be "equal"

  std::cout << "compareValues:  " << TypeOfTest << " -> ";
  if (equal) {
    if (testval == compareval) {
      std::cout << "PASS = " << std::hex << compareval << std::endl;
      return true;
    } else {
      std::cout << "FAIL!" << std::endl;
      std::cout << TypeOfTest 
		<< " expected value = " << std::hex << compareval
		<< ", returned value = " << std:: hex << testval
		<< std::endl;
      return false;
    }
  } else {
    if (testval != compareval) {
      std::cout << "PASS -> " << std::hex << testval 
		<< " not equal to " <<std::hex << compareval 
		<< std::endl;
      return true;
    } else {
      std::cout << "FAIL!" << std::endl;
      std::cout << TypeOfTest 
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

  std::cout << "compareValues tolerance:  " << TypeOfTest << " -> ";

  float err = (testval - compareval)/compareval;

  float fractolerance = tolerance*compareval;

  if (fabs(err)>tolerance) {
      std::cout << "FAIL!" << std::endl;
      std::cout << TypeOfTest 
		<< " expected = " << compareval 
		<< ", returned = " << testval
		<< " outside of tolerance "<< fractolerance
		<< std::endl;
      return false;
  } else {
      std::cout << "PASS!" << std::endl;
      std::cout << TypeOfTest 
		<< " value = " << testval
		<< " within "<< tolerance
		<< " of " << compareval
		<< std::endl;
      return true;
  }

}

void TMBTester::messageOK(std::string TypeOfTest,
			  bool testbool){
  std::cout << TypeOfTest;
  if (testbool) {
    std::cout << " -> PASS" << std::endl;
  } else {
    std::cout << " -> FAIL <-" << std::endl;
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

int TMBTester::UserOrBootJTAG(int choose){
  // Choose Source for JTAG commands
  int adr = (choose == 1) ? TMB_ADR_BOOT : vme_usr_jtag_adr;
}

void TMBTester::vme_jtag_anystate_to_rti(int address, int chain) {
  //take JTAG tap from any state to TLR then to RTI

  const int mxbits = 6;

  unsigned char tms[mxbits];
  unsigned char tdi[mxbits];
  unsigned char tdo[mxbits];

  unsigned char tms_rti[mxbits] = {1, 1, 1, 1, 1, 0}; //Anystate to TLR then RTI
  unsigned char tdi_rti[mxbits] = {};

  int iframe = 0;                                   //JTAG frame number

  for (int ibit=1; ibit<=mxbits; ibit++) {       // go from any state to RTI
    tms[iframe]=tms_rti[iframe];                   // take tap to RTI
    tdi[iframe]=tdi_rti[iframe];
    iframe++;                                      
  }

  int step_mode = 1;                               // 1=single step, 0=run
  
  int nframes;
  nframes = mxbits;

  vme_jtag_io_byte(address,chain,nframes,tms,tdi,tdo,step_mode);

  return;
}

void TMBTester::vme_jtag_write_ir(int adr,int ichain,int chip_id,int opcode){

  //	Writes opcode to the JTAG Instruction Register for the specified chip.
  //
  //	All chips except current chip are set to bypass mode.
  //
  //	It is presumed that the TAP controllers are presently in the RTI state
  //
  //	JTAG instructions and data are loaded for
  //	PROM1 first and PROM0 last:
  //
  //	JTAG				Load	TMB		RAT
  //	Position Chip ID	Order	Name	Name
  //	tdi>0		0		1		PROM0	FPGA
  //	tdo<1		1		0		PROM1	PROM

  int dummy;

  const int mxbits=128;       //maximum # bits of tms
  const int mxchain=4;        //chain_id
  const int mxdevices=5;      //devices in chain

  unsigned char tms[mxbits],tdi[mxbits],tdo[mxbits];

  const unsigned char tms_pre_opcode[4] = {1,1,0,0};      //RTI to ShfIR
  const unsigned char tdi_pre_opcode[4] = {0,0,0,0};

  const unsigned char tms_post_opcode[2] = {1,0};         //Ex1IR to RTI
  const unsigned char tdi_post_opcode[2] = {0,0};

  const int devices_in_chain[mxchain] = {2,2,2,5};        //chain 1,2,3,4

  const int bits_per_opcode[mxchain][mxdevices] = { 8, 8, 0, 0, 0,
						    5, 8, 0, 0, 0,
						    5, 8, 0, 0, 0,
						    6, 8, 8, 8, 8 };
  const unsigned char prom_ir[8] = {1, 0, 0, 2, 2, 0, 0, 0};  // 2=don't care flag

  //Set up TMS to take TAP from RTI to Shift IR:

  int chain_id = select_jtag_chain(ichain);
  
  int iframe = -1;                                    //JTAG frame number
  for (int preamble=0; preamble<=3; preamble++) {    //set up for writing opcode
    tms[++iframe] = tms_pre_opcode[preamble];
    tdi[iframe] = tdi_pre_opcode[preamble];
  }

  // Set up TMS to shift in the opcode bits for this chip, bypass code for others

  int wr_opcode = opcode;                                  //copy callers opcode
  int ndevices = devices_in_chain[chain_id-1];             //number of chips
  int ichip, nbitsop;
  int ibit;
  int bit;
  for (int idevice=0; idevice<=(ndevices-1); idevice++){   //loop over all chips
    ichip = ndevices - idevice - 1;                        //chip order in chain is reversed
    nbitsop = bits_per_opcode[chain_id][ichip];             //number of bits in its opcode
    for (ibit=1; ibit<=nbitsop; ibit++) {                  //loop opcode bits
      if (ichip==chip_id) {                                //This is the chip we want
	bit = ( wr_opcode/((int)pow(2,ibit-1)) ) & 0x1;    //extract bit
      } else {                                             //all other chips
	bit = 1;                                           //bypass
      }
      iframe++;
      if (iframe>mxbits) {
	std::cout << "vme_write_ir:  TOO MANY FRAMES" << std::endl;
	dummy = sleep(5);
	return;
      }
      tms[iframe]=0;                                      //Tap stays in ShfIR
      tdi[iframe]=bit;                                    //instruction bit
    }
  }

  //set up TMS to take TAP from Ex1IR to RTI

  tms[iframe]=1;
  for (int postamble = 0; postamble<=1; postamble++) {
    iframe++;
    if (iframe>mxbits) {
      std::cout << "vme_write_ir:  TOO MANY FRAMES" << std::endl;
      dummy = sleep(5);
      return;
    }
    tms[iframe]=tms_post_opcode[postamble];
    tdi[iframe]=tdi_post_opcode[postamble];
  }

  //Write JTAG OpCodes, end up with tap in RTI

  int step_mode = 1;                                   // 1=single step, 0=run
  int nframes = iframe;                                // total number of frames
  vme_jtag_io_byte(adr,ichain,nframes,tms,tdi,tdo,step_mode);

  //Check that TDO shifted out instruction register strings

  int iopbit;

  if (ichain!=0x8 && ichain!=0xc) {                    // skip if not TMB
    iframe = 4;                                        // start at first opcode bit
    for (ichip=0; ichip<=(ndevices-1); ichip++){           //loop over all chips
      nbitsop = bits_per_opcode[chain_id][ichip];             //number of bits in its opcode
      for (ibit=0; ibit<=(nbitsop-1); ibit++) {
	iframe++;
	if (iframe>mxbits) {
	  std::cout << "vme_write_ir:  TOO MANY FRAMES" << std::endl;
	  dummy = sleep(5);
	  return;
	}
	iopbit = tdo[iframe];
	if (iopbit!=prom_ir[ibit] && prom_ir[ibit]!=2) {
	  // did not read back proper opcode sequence...
	  // it should be 000xy001 000xy001: x=ISP status, y=Security...
	  std::cout << "vme_jtag_write_ir read bad ir " << std::hex << iopbit << std::endl;
	}
      }
    }
  }

  return;
}

/*void TMBTester::vme_jtag_write_dr(int adr,
				  int ichain,
				  int chip_id,
				  int write_data,
				  int read_data,
				  int nbits){
  std::cout << "vme_jtag_write_dr:  CAUTION not yet debugged" << std::endl;

  //	Writes TDI bits for the currently selected JTAG Data Register
  //	for the specified chip.
  //	Reads back TDO data.
  //	
  //	chip_id		=	device number to write
  //	wr_data()	=	array to send containing nbits packed 1 bit per byte
  //	rd_data()	=	read back data, 1 byte per bit
  //	nbits		=	total number of bits to write
  //
  //	All chips except current chip are set to bypass mode.
  //
  //	It is presumed that the TAP controllers are presently in the RTI state
  //	And that the desired opcode has already been written to the TAP.
  //
  //	JTAG instructions and data are loaded for
  //	PROM1 first and PROM0 last:
  //
  //	JTAG				Load	TMB		RAT
  //	Position Chip ID	Order	Name	Name
  //	tdi>0		0		1		PROM0	FPGA
  //	tdo<1		1		0		PROM1	PROM
  //
  int dummy;

  const int mxchain=4;        //chain_id
  const int mxdevices=5;      //devices in chain
  const int mxbitstream=1000; //Max # bits in a jtag r/w cycle
  const int mxline=80;        //max # character per input file line

  unsigned char write_data[nbits],read_data[nbits];
  unsigned char tms[mxbitstream],tdi[mxbitstream],tdo[mxbitstream];

  const unsigned char tms_pre_read[3] = {1,0,0};      //RTI to ShfDR
  const unsigned char tdi_pre_read[3] = {0,0,0};

  const unsigned char tms_post_read[2] = {1,0};       //Ex1DR to RTI
  const unsigned char tdi_post_read[2] = {0,0};

  const int devices_in_chain[mxchain] = {2,2,2,5};        //chain 1,2,3,4

  const int bits_per_opcode[mxchain][mxdevices] = { 8, 8, 0, 0, 0,
						    5, 8, 0, 0, 0,
						    5, 8, 0, 0, 0,
						    6, 8, 8, 8, 8 };
  const unsigned char prom_ir[8] = {1, 0, 0, 2, 2, 0, 0, 0};  // 2=don't care flag

  //Set up TMS to take TAP from RTI to Shift DR:

  int chain_id = select_jtag_chain(ichain);
  
  int iframe = -1;                                    //JTAG frame number
  for (int preamble=0; preamble<=2; preamble++) {    //set up for writing opcode
    tms[++iframe] = tms_pre_read[preamble];
    tdi[iframe] = tdi_pre_read[preamble];
  }

  // Set up TMS to shift in the opcode bits for this chip, bypass code for others

  int reg_len = nbits;                                     //register length
  int ndevices = devices_in_chain[chain_id-1];             //number of chips

  for (int idevice=0; idevice<=(ndevices-1); idevice++){   //loop over all chips
    ichip = ndevices - idevice - 1;                        //chip order in chain is reversed
    if (ichip==chip_id) {                                //This is the chip we want
      offset=iframe;                                     //start of data
      for (i=1; i<=reg_len; i++){
	iframe++;
	//	GREG YOU ARE HERE........


      }
    } else {
      iframe++;
      if (iframe>mxbits) {
	std::cout << "vme_write_ir:  TOO MANY FRAMES" << std::endl;
	dummy = sleep(5);
	return;
      }
      tms[iframe]=0;                                      //Tap stays in ShfIR
      tdi[iframe]=bit;                                    //instruction bit
    }
  }

  //set up TMS to take TAP from Ex1IR to RTI

  tms[iframe]=1;
  for (int postamble = 0; postamble<=1; postamble++) {
    iframe++;
    if (iframe>mxbits) {
      std::cout << "vme_write_ir:  TOO MANY FRAMES" << std::endl;
      dummy = sleep(5);
      return;
    }
    tms[iframe]=tms_post_opcode[postamble];
    tdi[iframe]=tdi_post_opcode[postamble];
  }

  //Write JTAG OpCodes, end up with tap in RTI

  int step_mode = 1;                                   // 1=single step, 0=run
  int nframes = iframe;                                // total number of frames
  vme_jtag_io_byte(adr,ichain,nframes,tms,tdi,tdo,step_mode);

  //Check that TDO shifted out instruction register strings

  int iopbit;

  if (ichain!=0x8 && ichain!=0xc) {                    // skip if not TMB
    iframe = 4;                                        // start at first opcode bit
    for (ichip=0; ichip<=(ndevices-1); ichip++){           //loop over all chips
      nbitsop = bits_per_opcode[chain_id][ichip];             //number of bits in its opcode
      for (ibit=0; ibit<=(nbitsop-1); ibit++) {
	iframe++;
	if (iframe>mxbits) {
	  std::cout << "vme_write_ir:  TOO MANY FRAMES" << std::endl;
	  dummy = sleep(5);
	  return;
	}
	iopbit = tdo[iframe];
	if (iopbit!=prom_ir[ibit] && prom_ir[ibit]!=2) {
	  // did not read back proper opcode sequence...
	  // it should be 000xy001 000xy001: x=ISP status, y=Security...
	  std::cout << "vme_jtag_write_ir read bad ir " << std::hex << iopbit << std::endl;
	}
      }
    }
  }

  return;
}
*/

void TMBTester::vme_jtag_io_byte(int address, 
				 int chain, 
				 int nframes,
				 unsigned char * tms,
				 unsigned char * tdi,
				 unsigned char * tdo,
				 int step_mode) {

  std::cout << "vme_jtag_io_byte:  CAUTION not yet debugged" << std::endl;

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

  int sel0 = chain & 0x1;               //JTAG chain select
  int sel1 = (chain>>1) & 0x1;
  int sel2 = (chain>>2) & 0x1;
  int sel3 = (chain>>3) & 0x1;

  int jtag_word;

  jtag_word = boot_state;      
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

  if (nframes>=1) {                       //no frames to send

    //loop over input data frames...
    for (int iframe=0; iframe<=(nframes-1); iframe++) { //arrays count from 0
      tdo[iframe]=0;                      //clear tdo

      tck_bit = 0x0 << 2;                   //take TCK low
      tms_bit = tms[iframe] << 1;           //TMS bit
      tdi_bit = tdi[iframe];                //TDI bit

      jtag_out = jtag_word & 0x7ff8;           //clear old state
      jtag_out |= tck_bit | tms_bit | tdi_bit;

      dummy = tmb_->tmb_set_boot_reg(jtag_out);    //write boot register

      dummy = tmb_->tmb_get_boot_reg(&jtag_in);    //read boot register
      tdo[iframe] = (jtag_in << 15) & 0x1;         //extract tdo bit, mask lsb
      tdo_bit = tdo[iframe];

      if (!step_mode) {
	step(tck_bit,tms_bit,tdi_bit,tdo_bit);
      }

      tck_bit = 0x1 << 2;  //Take TCK high, leave tms,tdi as they were
      jtag_out |= tck_bit | tms_bit | tdi_bit;
      dummy = tmb_->tmb_set_boot_reg(jtag_out);    //write boot register

      dummy = tmb_->tmb_get_boot_reg(&jtag_in);    //read boot register
      tdo[iframe] = (jtag_in << 15) & 0x1;         //extract tdo bit, mask lsb

      if (!step_mode) {
	step(tck_bit,tms_bit,tdi_bit,tdo_bit);
      }      
    }
  }
  jtag_out &= 0xfffb;     //Take TCK low, leave others as they were
  dummy = tmb_->tmb_set_boot_reg(jtag_out);        //write boot register

  return;
}

void TMBTester::step(int tck, int tms, int tdi, int tdo){

  std::cout << "tck = " << std::hex << tck
	    << "tms = " << std::hex << tms
	    << "tdi = " << std::hex << tdi
	    << "tdo = " << std::hex << tdo
	    << "pause.... enter any number, then return...";
  int dummy;
  std::cin >> dummy;

  return;
}

int TMBTester::select_jtag_chain(int ichain){

  int dummy;

  int chain_id = -1;

  if (ichain==0x8) chain_id = 1;  //TMB user prom chain
  if (ichain==0xc) chain_id = 1;  //TMB fpga chain
  if (ichain==0xd) chain_id = 2;  //RAT chain
  if (ichain==0x3) chain_id = 3;  //ALCT mez pgm chain
  if (ichain==0x2) chain_id = 3;  //ALCT mez cfg chain
  if (ichain==0x4) chain_id = 4;  //TMB mez FPGA+4 PROMs

  if (chain_id == -1) {
    std::cout << "select_jtag_chain:  ERROR chain_id not implemented" << std::endl;
    dummy = sleep(5);
  }

  return chain_id;
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

  std::cout << "Initial ADC status = " << std::hex << adc_status << std::endl;

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

  std::cout << "smb address = " << std::hex << smb_adr
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

    std::cout << "Before Persistent -> i2c_clock " << i2c_clock << ", scl = " << scl << " sda_bit = " << sda << std::endl;

    if (i2c_clock<3) scl=1;                       // START scl stays high
    if (i2c_clock<2) sda=1;                       // START sda transitions low

    if (i2c_clock>nclks-3) scl=1;                // STOP scl stays high
    if (i2c_clock>nclks-2) sda=1;                // STOP sda transitions high

    std::cout << "After Persistent  -> i2c_clock " << i2c_clock << ", scl = " << scl << " sda_bit = " << sda << std::endl;

    //** Write serial clock and data to TMB VME interface **

    write_data = adc_status & 0xf9ff;    //clear bits 9 and 10
    write_data |= scl << 9;
    write_data |= sda << 10;
    tmb_->WriteRegister(adc_adr,write_data);

    std::cout << "write_data =" << write_data << std::endl;

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
      std::cout << "reading serial data d[" << sda_clock << "] =" << d[sda_clock] << std::endl;
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
      //      std::cout << "d[17-" << i << "] =" << d[17-i] << "sda_value = " << sda_value << std::endl;
    } else {
      data |= sda_value<<i;          //sign extend if bit 7 indicates negative value      
    }
    //    std::cout << "sda_value = " << sda_value << ", data =" << data << std::endl;
  }

  std::cout << "Temperature = " << std::dec << data << " deg C -> " << std::hex << data << std::endl;

  float temperature = float (data) * 9. / 5. + 32.;

  return temperature;
}


/*
int TMBTester::do_jtag(int chip_id, 
		       int opcode, 
		       int mode, 
		       const int *first, 
		       int length, 
		       int *val) {
  
  std::cout << "call TMBTester::do_jtag for chip " << chip_id << std::endl;

  int     i, j, k, m, ichip, j_start, nframes, step_mode = 0;
  char    tms_pre_read[3] = { 1, 0, 0 };
  int     bits_per_opcode[MAX_NUM_CHIPS] = {5,6};  // myc++ { 4, 4, 4, 4, 4, 4, 4, 4, 6 }; back to 4
  int     tms_preop_code[5] = { 0, 1, 1, 0, 0 };  //Jonathan's code has 4 = {1, 1, 0, 0}...
  int     tms_postop_code[3] = { 1, 0, 0 };       //Jonathan's code has 2 = {1, 0}...
  char    tdi[MXBITSTREAM], tms[MXBITSTREAM], tdo[MXBITSTREAM];

  for (i=0; i< MAX_JTAG_BUF_SIZE; i++) {
    sndbuf[i] = 0;
    rcvbuf[i] = 0;
  }

  // First JTAG operation writes opcodes to the TAP instruction registers... 

  // Put TAP in state ShfIR 
  j=0;
  for (k = 0; k < 5; k++) {
    tdi[j]   = 0;
    tms[j++] = tms_preop_code[k];
  }

  // Construct opcode for each chip (all but chip_id are BYPASS = all 1's)
  if(chip_id <= 7) {    //Why <= 7?
    ichip=0;            //this picks the bits_per_opcode
    tmb_->start(8);     //What is this doing?
  } else {
    ichip=1;
    tmb_->start(6);
  } 

  //printf("\nOpcode 0x%02x \n", opcode);
  for (i=0; i< sizeof(opcode); i++) {
    sndbuf[i] = (opcode >> 8*i)  & 0x00ff;
    // printf(" %02x", sndbuf[i]); 
  }
  
  for (k = 0; k < bits_per_opcode[ichip]; k++) {
    tdi[j] = (opcode >> k) & 0x1;
    tms[j++] = 0;
  }
  
  tms[j-1] = 1;        // TMS goes high on last frame
  
  // Put TAP back in RTI mode 
  for (k = 0; k < 3; k++) {
    tdi[j]   = 0;
    tms[j++] = tms_postop_code[k];
  }
  
  // Do JTAG
  nframes = j;
  
  for (j = 0; j < nframes; j++) {
    tdo[j] = 0 ;
  }
  
  tmb_->scan(INSTR_REG, sndbuf, bits_per_opcode[ichip], rcvbuf , 0 );
  
  //  
  //    Second JTAG operation writes data to the selected register of chip_id, 
  //    zero to bypass register of all other chips.
  //   
  
  // Put TAP in state ShfDR 
  for (k = 0, j = 0; k < 3; k++) {
    tdi[j]   = 0;
    tms[j++] = tms_pre_read[k];
  }
  
  //  Convert the list of data items into a one-byte-per-bit array, if writing,
  //  or else fill with zeros.
  for (k = 0; k < ((length-1)/8+1); k++) { 
    sndbuf[k] = 0; 
  }
  // printf("\nTDI ");
  j_start = j;
  m = 0;
  for (k = 0; k < length; k++) {
    if (mode == WRITE) {
      if (k == first[m+1]) m++;
      tdi[j] = (val[m] >> (k-first[m])) & 0x1;
      sndbuf[k/8] |= tdi[j] << (k%8);
    } else { 
      tdi[j] = 0;
    }      
    tms[j++] = 0;
  }
  
  tms[j-1] = 1;        // TMS goes high on last data frame
  
  // printf("\nData In ");    
  // for (k = 0; k < ((length-1)/8+1); k++)
  // { printf(" %x", sndbuf[k]); }
  
  // Put TAP back in RTI mode 
  for (k = 0; k < 3; k++) {
    tdi[j]   = 0;
    tms[j++] = tms_postop_code[k];
  }
  
  // Do JTAG 
  nframes = j;
  
  // jtag_io_byte_(&nframes, tms, tdi, tdo, &step_mode);

  // if (mode == READ)
  //
  tmb_->scan(DATA_REG, sndbuf, length, rcvbuf, 1);
  //

  //else { tmb_->scan(DATA_REG, sndbuf, length, rcvbuf, 0);}
  
  // Whether reading or writing, convert the one-byte-per-bit tdo data array 
  // into a list of values 
  
  //    for (k = 0; k < ((length-1)/8+1); k++){ 
  //      rcvbuf[k] = 0; 
  //    }
  
  //   printf("\nTDO "); 
  
  //   i = 0;
  //   val[0] = 0;
  //   j = j_start;
  //   for (k = 0; k < length; k++) {
  //   if (k == first[i+1]) val[++i] = 0;
  //   if (tdo[j++]) {
  //   val[i] |= (1 << (k-first[i]));
  //   rcvbuf[k/8] |= 1 << (k%8);
  //   }    
  //   }

  //   printf("\nOut ");
  //   for (k=0; k<i; k++)
  //   { printf(" 0x%02x", val[k]); }
   
  //   printf("\nData Out ");
  //   for (k = 0; k < ((length-1)/8+1); k++)
  //   { printf(" 0x%02x", rcvbuf[k]); }

  //if (mode == READ) 
  //{
  i = 0;
  val[0] = 0;
  for (k = 0; k < length; k++) {
    if (k == first[i+1]) val[++i] = 0;
    if ((rcvbuf[k/8] >> (k%8)) & 0x01) { 
      val[i] |= (1 << (k-first[i])); 
    }
  }

  // printf("\nNet Out ");
  // for (k=0; k<i; k++)
  //  { printf(" 0x%02x", val[k]); }  
  //}
  
  tmb_end();
  return 0;
}

void TMBTester::tmb_end() {
  tmb_->endDevice();
}
*/


/////////////////////////////////////
// The following should be in TMB: //
/////////////////////////////////////
std::bitset<64> TMBTester::dsnRead(int type) {
  //type = 0 = TMB
  //     = 1 = Mezzanine DSN
  //     = 2 = RAT DSN

  std::bitset<64> dsn;

  int offset;
  offset = type*5; 

  int wr_data, rd_data;

  int initial_state;
  // ** need to specifically enable RAT to read back DSN **
  if (type == 2) {  
    initial_state = tmb_->ReadRegister(vme_ratctrl_adr);  //initial RAT state

    wr_data = initial_state & 0xfffd;    //0=sync_mode, 1=posneg, 2=loop_tmb, 3=free_tx0, 4=dsn enable
    wr_data |= 0x0010;                   //enable the dsn bit

    tmb_->WriteRegister(vme_ratctrl_adr,wr_data);
  }

  int i;
  int idata;

  // init pulse >480usec
  wr_data = 0x0005; 
  wr_data <<= offset; //send it to correct component
  rd_data = dsnIO(wr_data);

  // ROM Read command = serial 0x33:
  for (i=0; i<=7; i++) {
    idata = (0x33>>i) & 0x1;
    wr_data = (idata<<1) | 0x1; //send "serial write pulse" with "serial SM start"
    wr_data <<= offset; 
    rd_data = dsnIO(wr_data);
  }

  // Read 64 bits of ROM data = 0x3 64 times
  for (i=0; i<=63; i++) {
    wr_data = 0x0003; 
    wr_data <<= offset;
    rd_data = dsnIO(wr_data);

    // pack data into dsn[]
    dsn[i] = (rd_data >> (4+offset)) & 0x1;
  }

  // ** Return the RAT to its initial state **
  if (type == 2) {
    tmb_->WriteRegister(vme_ratctrl_adr,initial_state);
  }

  return dsn;
}

int TMBTester::dsnIO(int writeData){
  //Single I/O cycle for Digital Serial Number...
  //called by dsnRead...

  int adr = vme_dsn_adr;
  int readData;

  // write the desired data word:
  tmb_->WriteRegister(adr,writeData);

  int tmb_busy,mez_busy,rat_busy;
  int busy = 1;
  int nbusy = 1;

  while (busy) {
    readData = tmb_->ReadRegister(adr);
    
    // check busy on all components:
    tmb_busy = (readData>>3) & 0x1;
    mez_busy = (readData>>8) & 0x1;
    rat_busy = (readData>>13) & 0x1;
    busy = tmb_busy | mez_busy | rat_busy;

    if (nbusy%1000 == 0) {
      std::cout << "dsnIO: DSN state machine busy, nbusy = "
                << nbusy << ", readData = " 
		<< std::hex << readData << std::endl;  
    }
    nbusy++;
  }

  // end previous cycle
  tmb_->WriteRegister(adr,0x0000);

  return readData;
}

void TMBTester::ADCvoltages(float * voltage){

  //Read the ADC of the voltage values ->
  //voltage[0] = +5.0V TMB
  //       [1] = +3.3V TMB
  //       [2] = +1.5V core
  //       [3] = +1.5V TT
  //       [4] = +1.0V TT
  //       [5] = +5.0V Current (A) TMB
  //       [6] = +3.3V Current (A) TMB
  //       [7] = +1.5V core Current (A) TMB
  //       [8] = +1.5V TT Current (A) TMB
  //       [9] = if SH921 set 1-2, +1.8V RAT current (A)
  //           = if SH921 set 2-3, +3.3V RAT
  //      [10] = +1.8V RAT core
  //      [11] = reference Voltage * 0.5
  //      [12] = ground (0V)
  //      [13] = reference voltage (= ADC maximized)

  int adc_dout;                      //Voltage monitor ADC serial data receive
  int adc_sclock;                    //Voltage monitor ADC serial clock
  int adc_din;                       //Voltage monitor ADC serial data transmit
  int adc_cs;                        //Voltage monitor ADC chip select

  int adc_shiftin;
  int iclk;

  int write_data, read_data;

  for (int ich=0; ich<=14; ich++){
    adc_dout = 0;

    adc_din    = 0;
    adc_sclock = 0;
    adc_cs     = 1;

    write_data = 0;
    write_data |= (adc_sclock << 6);  
    write_data |= (adc_din    << 7);  
    write_data |= (adc_cs     << 8);  

    tmb_->WriteRegister(vme_adc_adr,write_data);

    adc_shiftin = ich << 4;      //d[7:4]=channel, d[3:2]=length, d[1:0]=ldbf,bip
    if (ich >= 14) adc_shiftin = 0;  //don't send channel 14, it is power-down

    //put adc_shiftin serially in 11 vme writes
    for (iclk=0; iclk<=11; iclk++){

      if (iclk <= 7) {
	adc_din = (adc_shiftin >> (7-iclk)) & 0x1;
      } else {
	adc_din = 0;
      }
      adc_sclock = 0;
      adc_cs     = 0;

      write_data = 0;
      write_data |= (adc_sclock << 6);  
      write_data |= (adc_din    << 7);  
      write_data |= (adc_cs     << 8);  
      
      tmb_->WriteRegister(vme_adc_adr,write_data);

      adc_sclock = 1;
      adc_cs     = 0;

      write_data = 0;
      write_data |= (adc_sclock << 6);  
      write_data |= (adc_din    << 7);  
      write_data |= (adc_cs     << 8);  
      
      tmb_->WriteRegister(vme_adc_adr,write_data);

      read_data = (tmb_->PowerComparator() >> 5) & 0x1;

      //pack output into adc_dout
      adc_dout |= (read_data << (11-iclk));
    }

    adc_din    = 0;
    adc_sclock = 0;
    adc_cs     = 1;

    write_data = 0;
    write_data |= (adc_sclock << 6);  
    write_data |= (adc_din    << 7);  
    write_data |= (adc_cs     << 8);  

    tmb_->WriteRegister(vme_adc_adr,write_data);

    if (ich>=1) {
      voltage[ich-1] = ((float) adc_dout / 4095.)*4.095; //convert adc value to volts
    }

  }

  voltage[0] *= 2.0;                      // 1V/2V
  voltage[5] /= 0.2;                      // 200mV/Amp
  voltage[6] /= 0.2;                      // 200mV/Amp
  voltage[7] /= 0.2;                      // 200mV/Amp
  voltage[8] /= 0.2;                      // 200mV/Amp
  voltage[9] /= 0.2;                      // 200mV/Amp if SH921 set 1-2, else comment out line

  return;
}


int TMBTester::tmb_read_delays(int device) {

  int data;

  // device = 0  = CFEB 0 Clock
  //        = 1  = CFEB 1 clock
  //        = 2  = CFEB 2 clock
  //        = 3  = CFEB 3 clock
  //        = 4  = CFEB 4 clock
  //        = 5  = ALCT rx clock
  //        = 6  = ALCT tx clock
  //        = 7  = DMB tx clock
  //        = 8  = RPC clock
  //        = 9  = TMB1 clock
  //        = 10 = MPC clock
  //        = 11 = DCC clock (CFEB duty cycle correction)

  if (device==0) data = (tmb_->ReadRegister(0x18)>>12) & 0xf; 
  if (device==1) data = (tmb_->ReadRegister(0x1a)>> 0) & 0xf; 
  if (device==2) data = (tmb_->ReadRegister(0x1a)>> 4) & 0xf; 
  if (device==3) data = (tmb_->ReadRegister(0x1a)>> 8) & 0xf; 
  if (device==4) data = (tmb_->ReadRegister(0x1a)>>12) & 0xf; 
  if (device==5) data = (tmb_->ReadRegister(0x16)>> 4) & 0xf; 
  if (device==6) data = (tmb_->ReadRegister(0x16)>> 0) & 0xf; 
  if (device==7) data = (tmb_->ReadRegister(0x16)>> 8) & 0xf; 
  if (device==8) data = (tmb_->ReadRegister(0x16)>>12) & 0xf; 
  if (device==9) data = (tmb_->ReadRegister(0x18)>> 0) & 0xf; 
  if (device==10) data =(tmb_->ReadRegister(0x18)>> 4) & 0xf; 
  if (device==11) data =(tmb_->ReadRegister(0x18)>> 8) & 0xf; 

  return data;
}

void TMBTester::rat_clk_delays(unsigned short int time,int cfeb_id)
{
  // device = 0  = CFEB 0 Clock
  //        = 1  = CFEB 1 clock
  //        = 2  = CFEB 2 clock
  //        = 3  = CFEB 3 clock
  //        = 4  = CFEB 4 clock
  //        = 5  = ALCT rx clock
  //        = 6  = ALCT tx clock
  //        = 7  = DMB tx clock
  //        = 8  = RPC clock
  //        = 9  = TMB1 clock
  //        = 10 = MPC clock
  //        = 11 = DCC clock (CFEB duty cycle correction)
  //        = 1000 = CFEB [0-5] Clock (all CFEB's)

  //GREG, take these out of here when it goes back in place of new_clk_delays:
  enum WRT { LATER, NOW };
  char sndbuf[2],rcvbuf[2];

  //int ierr;
int iloop;
 iloop=0;
  printf(" here write to delay registers \n");
  if ( cfeb_id == 0 ) {
    tmb_->tmb_vme(0x01,0x18,sndbuf,rcvbuf,NOW);
    sndbuf[0]=(((time&0xf)<<4)&0xf0)|(rcvbuf[0]&0x0f);
    sndbuf[1]=rcvbuf[1];
    tmb_->tmb_vme(0x02,0x18,sndbuf,rcvbuf,NOW);
  } 
  if ( cfeb_id == 1 ) {
    tmb_->tmb_vme(0x01,0x1A,sndbuf,rcvbuf,NOW);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(time&0x0f)|(rcvbuf[1]&0xf0);
    tmb_->tmb_vme(0x02,0x1A,sndbuf,rcvbuf,NOW);
  } 
  if ( cfeb_id == 2 ) {
    tmb_->tmb_vme(0x01,0x1A,sndbuf,rcvbuf,NOW);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(((time&0x0f)<<4)&0xf0)|(rcvbuf[1]&0x0f);
    tmb_->tmb_vme(0x02,0x1A,sndbuf,rcvbuf,NOW);
  } 
  if ( cfeb_id == 3 ) {
    tmb_->tmb_vme(0x01,0x1A,sndbuf,rcvbuf,NOW);
    sndbuf[0]=(time&0x0f)|(rcvbuf[0]&0xf0);
    sndbuf[1]=rcvbuf[1];
    tmb_->tmb_vme(0x02,0x1A,sndbuf,rcvbuf,NOW);
  } 
  if ( cfeb_id == 4 ) {
    tmb_->tmb_vme(0x01,0x1A,sndbuf,rcvbuf,NOW);
    sndbuf[0]=(((time&0xf)<<4)&0xf0)|(rcvbuf[0]&0x0f);
    sndbuf[1]=rcvbuf[1];
    tmb_->tmb_vme(0x02,0x1A,sndbuf,rcvbuf,NOW);
  } 
  if ( cfeb_id == 5 ) {
    tmb_->tmb_vme(0x01,0x16,sndbuf,rcvbuf,NOW);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(((time&0x0f)<<4)&0xf0)|(rcvbuf[1]&0x0f);
    tmb_->tmb_vme(0x02,0x16,sndbuf,rcvbuf,NOW);
  } 
  if ( cfeb_id == 6 ) {
    tmb_->tmb_vme(0x01,0x16,sndbuf,rcvbuf,NOW);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(time&0x0f)|(rcvbuf[1]&0xf0);
    tmb_->tmb_vme(0x02,0x16,sndbuf,rcvbuf,NOW);
  } 
  if ( cfeb_id == 7 ) {
    tmb_->tmb_vme(0x01,0x16,sndbuf,rcvbuf,NOW);
    sndbuf[0]=(time&0x0f)|(rcvbuf[0]&0xf0);
    sndbuf[1]=rcvbuf[1];
    tmb_->tmb_vme(0x02,0x16,sndbuf,rcvbuf,NOW);
  } 
  if ( cfeb_id == 8 ) {
    tmb_->tmb_vme(0x01,0x16,sndbuf,rcvbuf,NOW);
    sndbuf[0]=(((time&0xf)<<4)&0xf0)|(rcvbuf[0]&0x0f);
    sndbuf[1]=rcvbuf[1];
    tmb_->tmb_vme(0x02,0x16,sndbuf,rcvbuf,NOW);
  } 
  if ( cfeb_id == 9 ) {
    tmb_->tmb_vme(0x01,0x18,sndbuf,rcvbuf,NOW);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(time&0x0f)|(rcvbuf[1]&0xf0);
    tmb_->tmb_vme(0x02,0x18,sndbuf,rcvbuf,NOW);
  } 
  if ( cfeb_id == 10 ) {
    tmb_->tmb_vme(0x01,0x18,sndbuf,rcvbuf,NOW);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(((time&0x0f)<<4)&0xf0)|(rcvbuf[1]&0x0f);
    tmb_->tmb_vme(0x02,0x18,sndbuf,rcvbuf,NOW);
  } 
  if ( cfeb_id == 11 ) {
    tmb_->tmb_vme(0x01,0x18,sndbuf,rcvbuf,NOW);
    sndbuf[0]=(time&0x0f)|(rcvbuf[0]&0xf0);
    sndbuf[1]=rcvbuf[1];
    tmb_->tmb_vme(0x02,0x18,sndbuf,rcvbuf,NOW);
  } 
  if ( cfeb_id == 1000 ) {
    tmb_->tmb_vme(0x01,0x18,sndbuf,rcvbuf,NOW);
    sndbuf[0]=(((time&0xf)<<4)&0xf0)|(rcvbuf[0]&0x0f);
    sndbuf[1]=rcvbuf[1];
    tmb_->tmb_vme(0x02,0x18,sndbuf,rcvbuf,NOW);
    sndbuf[1]=(time&0x0f)|(((time&0xf)<<4)&0xf0);
    sndbuf[0]=(time&0x0f)|(((time&0xf)<<4)&0xf0);
    tmb_->tmb_vme(0x02,0x1A,sndbuf,rcvbuf,NOW);
  } 

  sndbuf[0]=0x0;
  sndbuf[1]=0x20;
  tmb_->tmb_vme(0x02,0x14,sndbuf,rcvbuf,NOW);
  tmb_->tmb_vme(0x01,0x14,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0]=0x0;
  sndbuf[1]=0x21;
  tmb_->tmb_vme(0x02,0x14,sndbuf,rcvbuf,NOW);
  tmb_->tmb_vme(0x01,0x14,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0]=0x0;
  sndbuf[1]=0x20;
  tmb_->tmb_vme(0x02,0x14,sndbuf,rcvbuf,NOW);
  tmb_->tmb_vme(0x01,0x14,sndbuf,rcvbuf,NOW);
  //
  while ( ((rcvbuf[1]>>6)&(0x1)) ){
    //
    tmb_->tmb_vme(0x01,0x14,sndbuf,rcvbuf,NOW);
    printf("______________ check state machine1 %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
    //
  }

  printf("______________ check state machine1 %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);

  while((rcvbuf[1]&0x40)!=0x00){
    iloop++;
    if(iloop>10){
      printf(" tmb_clk_delays: loop count exceeded so quit \n");
      return;
    }
    tmb_->tmb_vme(0x01,0x14,sndbuf,rcvbuf,NOW);
  }
  //
  sndbuf[0]=rcvbuf[0];
  sndbuf[1]=rcvbuf[1]&0xfe;
  //
  tmb_->tmb_vme(0x02,0x14,sndbuf,rcvbuf,NOW);
  tmb_->tmb_vme(0x01,0x14,sndbuf,rcvbuf,NOW);
  //
  while ( ((rcvbuf[1]>>6)&(0x1)) ){
    tmb_->tmb_vme(0x01,0x14,sndbuf,rcvbuf,NOW);
    printf(" *** check state machine2 %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  }
  //
  if((rcvbuf[1]&0x80)!=0x80){
    printf(" *** check state machine2 %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
    printf(" tmb_clk_delays: something is wrong. Can NOT be verified \n");
    return;
  }

  /* removed for new TMB delay chip
int ierr;
int iloop;
 iloop=0;
  //start(1); 
  printf(" write to delay registers \n");
  if ( cfeb_id == 0 ) {
    tmb_vme(VME_READ,0x1A,sndbuf,rcvbuf,LATER);
    sndbuf[0]=time&0x00ff;
    sndbuf[1]=rcvbuf[1];
    tmb_vme(VME_WRITE,0x1A,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 1 ) {
    tmb_vme(VME_READ,0x1C,sndbuf,rcvbuf,LATER);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1C,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 2 ) {
    tmb_vme(VME_READ,0x1C,sndbuf,rcvbuf,LATER);
    sndbuf[0]=time&0x00ff;
    sndbuf[1]=rcvbuf[1];
    tmb_vme(VME_WRITE,0x1C,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 3 ) {
    tmb_vme(VME_READ,0x1E,sndbuf,rcvbuf,LATER);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1E,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 4 ) {
    tmb_vme(VME_READ,0x1E,sndbuf,rcvbuf,LATER);
    sndbuf[0]=time&0x00ff;
    sndbuf[1]=rcvbuf[1];
    tmb_vme(VME_WRITE,0x1E,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 5 ) {
    tmb_vme(VME_READ,0x16,sndbuf,rcvbuf,LATER);
    sndbuf[0]=time&0x00ff;
    sndbuf[1]=rcvbuf[1];
    tmb_vme(VME_WRITE,0x16,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 6 ) {
    tmb_vme(VME_READ,0x16,sndbuf,rcvbuf,LATER);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=time&0x00ff;
    tmb_vme(VME_WRITE,0x16,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 1000 ) {
    sndbuf[1]=time&0x00ff;
    sndbuf[0]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1A,sndbuf,rcvbuf,LATER);
    sndbuf[1]=time&0x00ff;
    sndbuf[0]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1C,sndbuf,rcvbuf,LATER);
    sndbuf[1]=time&0x00ff;
    sndbuf[0]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1E,sndbuf,rcvbuf,LATER);
  } 

  sndbuf[0]=0x00;
  sndbuf[1]=0x00;
  tmb_vme(VME_READ,0x14,sndbuf,rcvbuf,LATER);
  printf(" check state machine %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  if((rcvbuf[1]&0x88)!=0x00){
    printf(" tmb_clk_delays: state machine not ready return \n");
    return;
  }
  sndbuf[0]=0x00;
  sndbuf[1]=0x33;
  tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,LATER);
  sndbuf[0]=0x00;
  sndbuf[1]=0x77;
  tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,LATER);
  // send delay to dynatem 
  sndbuf[0]=0x7f;
  sndbuf[1]=0xff;
  tmb_vme(0x03,0x00,sndbuf,rcvbuf,LATER); 
   sndbuf[0]=0x00;
  sndbuf[1]=0x33;
  tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,NOW);

 
LOOPBACK:
  iloop=iloop+1;
  if(iloop>100){
    printf(" tmb_clk_delays: loop count exceeded so quit \n");
    return;
  }

  tmb_vme(VME_READ,0x14,sndbuf,rcvbuf,NOW);
  printf(" check state machine2  %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  if((rcvbuf[1]&0x88)!=0x00)goto LOOPBACK;
  printf(" done so unstart state machine \n");
  */
}


//////////////////////////////////////////
// END: The following should be in TMB: //
//////////////////////////////////////////
