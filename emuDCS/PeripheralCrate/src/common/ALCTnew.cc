#include <iostream>
#include <iomanip>
#include <unistd.h> // for sleep
#include <vector>
#include <string>
//
#include "ALCTnew.h"
#include "EMUjtag.h"
#include "TMB.h"
//
//
ALCTnew::ALCTnew(TMB * tmb, std::string chamberType) :
  EMUjtag(tmb)
{
  //
  MyOutput_ = &std::cout ;
  (*MyOutput_) << "Creating ALCTnew" << std::endl;
  //
  tmb_ = tmb;
  SetChamberCharacteristics_(chamberType);
  //
  SetPowerUpTestpulsePowerSwitchReg();
  SetPowerUpTestpulseAmplitude();
  SetPowerUpTestpulseGroupMask();
  SetPowerUpTestpulseStripMask();
  SetPowerUpAfebThresholds();
  SetPowerUpStandbyRegister();
  //
  SetPowerUpDelayLineControlReg_();
  SetPowerUpAsicDelays();
  SetPowerUpAsicPatterns();
  SetPowerUpConfigurationReg();
  SetPowerUpHotChannelMask();
  //
}
//
ALCTnew::~ALCTnew() {
  //
  //
}
//
//
void ALCTnew::setup() {
  (*MyOutput_) << "Set up ALCT..." << std::endl;
  //
  ReadSlowControlId();
  PrintSlowControlId();
  //
  WriteTestpulsePowerSwitchReg();
  ReadTestpulsePowerSwitchReg();
  PrintTestpulsePowerSwitchReg();
  //
  WriteTestpulseAmplitude();
  //
  WriteTestpulseGroupMask();
  ReadTestpulseGroupMask();
  PrintTestpulseGroupMask();
  //
  WriteTestpulseStripMask();
  ReadTestpulseStripMask();
  PrintTestpulseStripMask();
  //
  WriteAfebThresholds();
  ReadAfebThresholds();
  PrintAfebThresholds();
  //    
  WriteStandbyRegister();
  ReadStandbyRegister();
  PrintStandbyRegister();
  //
  //
  ReadFastControlId();
  PrintFastControlId();
  //
  WriteAsicDelaysAndPatterns();
  ReadAsicDelaysAndPatterns();
  PrintAsicDelays();
  PrintAsicPatterns();
  //
  WriteConfigurationReg();
  ReadConfigurationReg();
  PrintConfigurationReg();
  //
  WriteHotChannelMask();
  ReadHotChannelMask();
  PrintHotChannelMask();
  //
  return;
}
///////////////////////
//SLOW CONTROL ID
///////////////////////
void ALCTnew::ReadSlowControlId() {
  //
  (*MyOutput_) << "ALCT: READ slow control ID " << std::endl;
  //
  setup_jtag(ChainAlctSlowFpga);
  //
  ShfIR_ShfDR(ChipLocationAlctSlowFpga,
	      ALCT_SLOW_RD_ID_REG,
	      RegSizeAlctSlowFpga_RD_ID_REG);
  //
  packCharBuffer(GetDRtdo(),
		 GetRegLength(),
		 slowcontrol_id_);
  return;
}
//
void ALCTnew::PrintSlowControlId() {
  //
  (*MyOutput_) << "ALCT: Slow Control chip ID = " << std::hex << GetSlowControlChipId()
	       << " version " << GetSlowControlVersionId()
	       << ": day = " << GetSlowControlDay()
	       << ", month = " << GetSlowControlMonth()
	       << ", year = " << GetSlowControlYear()
	       << std::dec << std::endl; 
  return;
}
//
int ALCTnew::GetSlowControlChipId() { 
  return (slowcontrol_id_[0] & 0xf); 
}
//
int ALCTnew::GetSlowControlVersionId() { 
  return ((slowcontrol_id_[0]>>4) & 0xf); 
}
//
int ALCTnew::GetSlowControlYear() { 
  return ((slowcontrol_id_[2]<<8) | slowcontrol_id_[1]&0xff); 
}
//
int ALCTnew::GetSlowControlDay() { 
  return (slowcontrol_id_[3] & 0xff); 
}
//
int ALCTnew::GetSlowControlMonth() { 
  return (slowcontrol_id_[4] & 0xff); 
}
//
///////////////////////
//TESTPULSE POWERSWITCH
///////////////////////
void ALCTnew::WriteTestpulsePowerSwitchReg() {
  (*MyOutput_) << "ALCT: WRITE testpulse POWERSWITCH" << std::endl;
  //
  setup_jtag(ChainAlctSlowFpga);
  //
  ShfIR_ShfDR(ChipLocationAlctSlowFpga,
	      ALCT_SLOW_WRT_TESTPULSE_POWERDOWN,
	      RegSizeAlctSlowFpga_WRT_TESTPULSE_POWERDOWN,
	      &testpulse_power_setting_);
  return;
}
//
void ALCTnew::ReadTestpulsePowerSwitchReg() {
  //
  (*MyOutput_) << "ALCT: READ testpulse POWERSWITCH" << std::endl;
  //
  setup_jtag(ChainAlctSlowFpga);
  //
  ShfIR_ShfDR(ChipLocationAlctSlowFpga,
	      ALCT_SLOW_RD_TESTPULSE_POWERDOWN,
	      RegSizeAlctSlowFpga_RD_TESTPULSE_POWERDOWN);
  //
  int value = bits_to_int(GetDRtdo(),
			  GetRegLength(),
			  LSBfirst);
  //
  SetTestpulsePowerSwitchReg(value);
  return;
}
//
void ALCTnew::PrintTestpulsePowerSwitchReg() {
  //
  (*MyOutput_) << "ALCT: Testpulse powerswitch = " << std::dec
	       << GetTestpulsePowerSwitchReg() << std::endl;
  return;
}
//
void ALCTnew::SetTestpulsePowerSwitchReg(int powerswitch) {     // powerswitch = OFF or ON
  testpulse_power_setting_ = powerswitch & 0x1; 
  return;
}
//
int ALCTnew::GetTestpulsePowerSwitchReg() { 
  return testpulse_power_setting_; 
}
//
void ALCTnew::SetPowerUpTestpulsePowerSwitchReg() { 
  SetTestpulsePowerSwitchReg(OFF); 
  return;
}
//
///////////////////////
//TESTPULSE AMPLITUDE
///////////////////////
void ALCTnew::WriteTestpulseAmplitude() {
  (*MyOutput_) << "ALCT: WRITE testpulse AMPLITUDE = " 
  	       << GetTestpulseAmplitude() << std::endl;
  //
  int dac[RegSizeAlctSlowFpga_WRT_TESTPULSE_DAC] = {};
  //
  //DAC's require MSB sent first....
  int_to_bits(testpulse_amplitude_dacvalue_,
	      RegSizeAlctSlowFpga_WRT_TESTPULSE_DAC,
	      dac,
	      MSBfirst);
  //  for (int i=0; i<RegSizeAlctSlowFpga_WRT_TESTPULSE_DAC; i++) 
  //    (*MyOutput_) << "dac[" << i << "] = " << dac[i] << std::endl;
  //
  setup_jtag(ChainAlctSlowFpga);
  //      
  ShfIR_ShfDR(ChipLocationAlctSlowFpga,
	      ALCT_SLOW_WRT_TESTPULSE_DAC,
	      RegSizeAlctSlowFpga_WRT_TESTPULSE_DAC,
	      dac);
  return;
}
//
void ALCTnew::SetTestpulseAmplitude(int dacvalue) {
  // 8-bit DAC controls amplitude of analog test pulse sent to AFEBs
  //  Voltage = 2.5V * dacvalue/256
  //
  if (dacvalue > 255) {
    (*MyOutput_) << "ALCT: ERROR testpulse amplitude maximum = 255 " << std::endl;
    return;
  } 
  //
  testpulse_amplitude_dacvalue_ = dacvalue & 0xff;
  //
  return;
}
//
int ALCTnew::GetTestpulseAmplitude() { 
  return testpulse_amplitude_dacvalue_; 
}
//
void ALCTnew:: SetPowerUpTestpulseAmplitude() { 
  SetTestpulseAmplitude(0); 
  return;
}
//
///////////////////////
//TESTPULSE GROUPMASK
///////////////////////
void ALCTnew::WriteTestpulseGroupMask() {
  //
  (*MyOutput_) << "ALCT: WRITE testpulse GROUPMASK" << std::endl;
  //
  setup_jtag(ChainAlctSlowFpga);
  //
  ShfIR_ShfDR(ChipLocationAlctSlowFpga,
	      ALCT_SLOW_WRT_TESTPULSE_GRP,
	      RegSizeAlctSlowFpga_WRT_TESTPULSE_GRP,
	      testpulse_groupmask_);
  return;
}
//
void ALCTnew::ReadTestpulseGroupMask() {
  //
  (*MyOutput_) << "ALCT: READ testpulse GROUPMASK" << std::endl;
  //
  setup_jtag(ChainAlctSlowFpga);
  //
  ShfIR_ShfDR(ChipLocationAlctSlowFpga,
	      ALCT_SLOW_RD_TESTPULSE_GRP,
	      RegSizeAlctSlowFpga_RD_TESTPULSE_GRP);
  //
  int * register_pointer = GetDRtdo();
  for (int i=0; i<RegSizeAlctSlowFpga_RD_TESTPULSE_GRP; i++)
    testpulse_groupmask_[i] = *(register_pointer+i);
  //
  return;
}
//
void ALCTnew::PrintTestpulseGroupMask() {
  //
  int testpulse_groupmask = bits_to_int(testpulse_groupmask_,
					RegSizeAlctSlowFpga_WRT_TESTPULSE_GRP,
					LSBfirst);
  //
  (*MyOutput_) << "ALCT: Testpulse groupmask = 0x" 
	       << std::hex << testpulse_groupmask << std::endl;
  return;
}
//
void ALCTnew::SetTestpulseGroupMask(int group,  
				    int mask) { 
  //
  // specify which groups of AFEBs are enabled for analog testpulsing
  //
  if (group < 0 || group >= RegSizeAlctSlowFpga_WRT_TESTPULSE_GRP) {
    (*MyOutput_) << "SetTestpulseGroupMask: ERROR group value must be between 0 and " 
		 << std::dec << RegSizeAlctSlowFpga_WRT_TESTPULSE_GRP-1 << std::endl;
    return;
  } 
  testpulse_groupmask_[group] = mask & 0x1;
  //
  return;
}
//
int ALCTnew::GetTestpulseGroupMask(int group) {
  return testpulse_groupmask_[group]; 
}
//
void ALCTnew::SetPowerUpTestpulseGroupMask() {
  //
  for (int group=0; group<RegSizeAlctSlowFpga_WRT_TESTPULSE_GRP; group++)
    SetTestpulseGroupMask(group,OFF);
  return;
}
//
///////////////////////
//TESTPULSE STRIPMASK
///////////////////////
void ALCTnew::WriteTestpulseStripMask() {
  //
  (*MyOutput_) << "ALCT: WRITE testpulse STRIPMASK" << std::endl;
  //
  setup_jtag(ChainAlctSlowFpga);
  //
  ShfIR_ShfDR(ChipLocationAlctSlowFpga,
	      ALCT_SLOW_WRT_TESTPULSE_STRIP,
	      RegSizeAlctSlowFpga_WRT_TESTPULSE_STRIP,
	      testpulse_stripmask_);
  return;
}
//
void ALCTnew::ReadTestpulseStripMask() {
  //
  (*MyOutput_) << "ALCT: READ testpulse STRIPMASK" << std::endl;
  //
  setup_jtag(ChainAlctSlowFpga);
  //
  ShfIR_ShfDR(ChipLocationAlctSlowFpga,
	      ALCT_SLOW_RD_TESTPULSE_STRIP,
	      RegSizeAlctSlowFpga_RD_TESTPULSE_STRIP);
  //
  int * register_pointer = GetDRtdo();
  for (int i=0; i<RegSizeAlctSlowFpga_RD_TESTPULSE_STRIP; i++)
    testpulse_stripmask_[i] = *(register_pointer+i);
  //
  return;
}
//
void ALCTnew::PrintTestpulseStripMask() {
  //
  int testpulse_stripmask = bits_to_int(testpulse_stripmask_,
					RegSizeAlctSlowFpga_WRT_TESTPULSE_GRP,
					LSBfirst);
  //
  (*MyOutput_) << "ALCT: testpulse stripmask = 0x" 
	       << std::hex << testpulse_stripmask << std::endl;
  return;
}
//
void ALCTnew::SetTestpulseStripMask(int afeb,
				    int mask) {
  //
  if (afeb < 0 || afeb >= RegSizeAlctSlowFpga_WRT_TESTPULSE_STRIP) {
    (*MyOutput_) << "SetTestpulseStripMask: ERROR AFEB value must be between 0 and " 
		 << std::dec << RegSizeAlctSlowFpga_WRT_TESTPULSE_STRIP-1 << std::endl;
    return;
  } 
  //
  testpulse_stripmask_[afeb] = mask & 0x1;
  //
  return;
}
//
int ALCTnew::GetTestpulseStripmask(int afeb) {
  //
  if (afeb < 0 || afeb >= RegSizeAlctSlowFpga_WRT_TESTPULSE_STRIP) {
    (*MyOutput_) << "GetTestpulseStripMask: ERROR AFEB value must be between 0 and " 
		 << std::dec << RegSizeAlctSlowFpga_WRT_TESTPULSE_STRIP-1 << std::endl;
    return 999;
  } 
  //
  return testpulse_stripmask_[afeb]; 
}
//
void ALCTnew::SetPowerUpTestpulseStripMask() {
  //
  for (int afeb=0; afeb<RegSizeAlctSlowFpga_WRT_TESTPULSE_STRIP; afeb++)
    SetTestpulseStripMask(afeb,OFF);
  //
  return;
}
//
//////////////////
//AFEB THRESHOLDS
//////////////////
void ALCTnew::WriteAfebThresholds() {
  //
  (*MyOutput_) << "ALCT: WRITE afeb THRESHOLDS " << std::endl;
  //
  for (int afebChannel=0; afebChannel<GetNumberOfAfebs(); afebChannel++) {
    //
    // Mapping of AFEB channel picks chip through the opcode...
    int opcode = ALCT_SLOW_WRT_THRESH_DAC0 + afeb_dac_chip[afebChannel];
    //
    // ..... and the DAC channel through TDI:
    int data_to_send = 
      ( (afeb_dac_channel[afebChannel]<<8) & 0xf00 ) | GetAfebThresholdDAC(afebChannel) & 0xff;
    //  (*MyOutput_) << "Data to send = " << data_to_send << std::endl;
    //
    int vector_of_data[RegSizeAlctSlowFpga_WRT_THRESH_DAC0] = {}; //all DAC's have same size register
    //
    int_to_bits(data_to_send,
		RegSizeAlctSlowFpga_WRT_THRESH_DAC0,
		vector_of_data,
		MSBfirst);
    //
    setup_jtag(ChainAlctSlowFpga);
    //
    ShfIR_ShfDR(ChipLocationAlctSlowFpga,
		opcode,
		RegSizeAlctSlowFpga_WRT_THRESH_DAC0,
		vector_of_data);
  }
  return;
}
//
void ALCTnew::ReadAfebThresholds() {
  //
  (*MyOutput_) << "ALCT: READ afeb THRESHOLDS " << std::endl;
  //
  for (int afeb=0; afeb<GetNumberOfAfebs(); afeb++)
    afeb_threshold_read_[afeb] = read_adc_(afeb_adc_chip[afeb],afeb_adc_channel[afeb]);
  //
  return;
}
//
void ALCTnew::PrintAfebThresholds() {
  //
  for (int afeb=0; afeb<GetNumberOfAfebs(); afeb++) 
    (*MyOutput_) << "Afeb " << std::dec << afeb
		 << " write threshold DAC = " << GetAfebThresholdDAC(afeb)
		 << " -> threshold ADC = " << GetAfebThresholdADC(afeb)
		 << " = " << GetAfebThresholdVolts(afeb) << " V" 
		 << std::endl;
  return;
}
//
void ALCTnew::SetAfebThreshold(int afebChannel, int dacvalue) {
  // 8-bit DAC controls threshold on each AFEB
  //  Voltage = 2.5V * dacvalue/256
  //
  if (dacvalue > 255) {
    (*MyOutput_) << "SetAfebThreshold: ERROR threshold maximum = 255 " << std::endl;
    return;
  } 
  //
  if (afebChannel >= GetNumberOfAfebs()) {
    (*MyOutput_) << "SetAfebThreshold: ALCT" << std::dec << GetNumberOfChannelsInAlct() 
		 << "-> channel " << std::dec << afebChannel
		 << " invalid ... must be between 0 and " << std::dec << GetNumberOfAfebs()-1 
		 << std::endl;
    return;
  } 
  //
  afeb_threshold_write_[afebChannel] = dacvalue & 0xff;
  //
  return;
}
//
int ALCTnew::GetAfebThresholdDAC(int afebChannel) {     // AFEB = [0-GetNumberOfAfebs()]
  return afeb_threshold_write_[afebChannel]; 
}
//
int ALCTnew::GetAfebThresholdADC(int afebChannel) {    
  return afeb_threshold_read_[afebChannel]; 
}
//
float ALCTnew::GetAfebThresholdVolts(int afebChannel) {
  return (2.5 * (float)GetAfebThresholdADC(afebChannel) /1023.); 
}
//
int ALCTnew::read_adc_(int chip, int channel) {
  // Pick chip through the opcode:
  int opcode = ALCT_SLOW_RD_THRESH_ADC0 + chip;
  //
  // Specify ADC channel in TDI:
  int data_to_send = (channel&0xf) << 7; // N.B. one more bit than what is specified in ALCT documentation
  //
  int address[RegSizeAlctSlowFpga_RD_THRESH_ADC0] = {}; //all ADC's have same size register
  //
  int_to_bits(data_to_send,
	      RegSizeAlctSlowFpga_RD_THRESH_ADC0,
	      address,
	      MSBfirst);
  //
  setup_jtag(ChainAlctSlowFpga);
  //
  // Need to ShfDR in two times: 
  //   -> First time to shift in the channel on tdi
  ShfIR_ShfDR(ChipLocationAlctSlowFpga,
	      opcode,
	      RegSizeAlctSlowFpga_RD_THRESH_ADC0,
	      address);
  //
  // Give adc time to receive address:
  ::usleep(100);
  //
  //   -> Second time to get the data on tdo
  ShfIR_ShfDR(ChipLocationAlctSlowFpga,
	      opcode,
	      RegSizeAlctSlowFpga_RD_THRESH_ADC0,
	      address);
  //
  int adcValue = bits_to_int(GetDRtdo(),
			     RegSizeAlctSlowFpga_RD_THRESH_ADC0,
			     MSBfirst);
  return adcValue;
}
//
void ALCTnew::SetPowerUpAfebThresholds() {
  //
  for (int afeb=0; afeb<GetNumberOfAfebs(); afeb++) 
    SetAfebThreshold(afeb,128);
  //
  return;
}
//
////////////////////////
//STANDBY REGISTER
////////////////////////
void ALCTnew::WriteStandbyRegister() {
  //
  (*MyOutput_) << "ALCT: WRITE standby register" << std::endl;
  //
  setup_jtag(ChainAlctSlowFpga);  
  //
  ShfIR_ShfDR(ChipLocationAlctSlowFpga,
	      ALCT_SLOW_WRT_STANDBY_REG,
	      RegSizeAlctSlowFpga_WRT_STANDBY_REG,
	      standby_register_);
  return;
}
//
void ALCTnew::ReadStandbyRegister() {
  //
  (*MyOutput_) << "ALCT: READ Standby Register" << std::endl;
  //
  setup_jtag(ChainAlctSlowFpga);  
  //
  ShfIR_ShfDR(ChipLocationAlctSlowFpga,
	      ALCT_SLOW_RD_STANDBY_REG,
	      RegSizeAlctSlowFpga_RD_STANDBY_REG);
  //
  int * standby_reg_pointer = GetDRtdo();
  //
  for (int i=0; i<RegSizeAlctSlowFpga_RD_STANDBY_REG; i++)
    standby_register_[i] = *(standby_reg_pointer+i);
  //
  return;
}
//
void ALCTnew::PrintStandbyRegister() {
  //
  const int buffersize = RegSizeAlctSlowFpga_RD_STANDBY_REG/8;
  char tempBuffer[buffersize] = {};
  packCharBuffer(standby_register_,
		 RegSizeAlctSlowFpga_RD_STANDBY_REG,
		 tempBuffer);
  //
  (*MyOutput_) << "ALCT: Standby Register (right to left)= ";
  for (int i=buffersize; i>=0; i--) {
    if (i == buffersize) {
      (*MyOutput_) << std::hex << (tempBuffer[i] & 0x03) << " ";  //register is 42 bits long
    } else {
      (*MyOutput_) << std::hex << (tempBuffer[i]>>4 & 0xf) 
		   << (tempBuffer[i] & 0xf) << " "; 
    }
  }
  (*MyOutput_) << std::dec <<std::endl;
  //
  return;
}
//
void ALCTnew::SetStandbyRegister(int afebChannel, int powerswitch) {
  //
  if (afebChannel >= GetNumberOfAfebs()) {
    (*MyOutput_) << "Set Standby Register: ALCT" << std::dec << GetNumberOfChannelsInAlct() 
		 << "-> channel " << std::dec << afebChannel
		 << " invalid ... must be between 0 and " << std::dec << GetNumberOfAfebs()-1 
		 << std::endl;
    return;
  } 
  //
  standby_register_[afebChannel] = powerswitch & 0x1;
  //
  return;
}
//
int ALCTnew::GetStandbyRegister(int afebChannel) {
  return standby_register_[afebChannel]; 
}
//
void ALCTnew::SetPowerUpStandbyRegister() {
  //
  for (int afeb=0; afeb<MAX_NUM_AFEBS; afeb++) 
    SetStandbyRegister(afeb,OFF);
  //
  return;
}
//
//////////////////////////////
// FAST CONTROL ID
//////////////////////////////
void ALCTnew::ReadFastControlId() {
  //
  (*MyOutput_) << "ALCT: READ fast control ID " << std::endl;
  //
  setup_jtag(ChainAlctFastFpga);
  //
  ShfIR_ShfDR(ChipLocationAlctFastFpga,
	      ALCT_FAST_RD_ID_REG,
	      RegSizeAlctFastFpga_RD_ID_REG);
  //
  packCharBuffer(GetDRtdo(),
		 GetRegLength(),
		 fastcontrol_id_);
  return;
}
//
void ALCTnew::PrintFastControlId() {
  //
  (*MyOutput_) << "ALCT: Fast Control chip ID = " << std::hex << GetFastControlChipId()
	       << " version " << GetFastControlVersionId()
	       << ": day = " << GetFastControlDay()
	       << ", month = " << GetFastControlMonth()
	       << ", year = " << GetFastControlYear()
	       << std::dec << std::endl; 
  return;
}
//
int ALCTnew::GetFastControlChipId() { 
  return (fastcontrol_id_[0] & 0xf); 
}
//
int ALCTnew::GetFastControlVersionId() { 
  return ((fastcontrol_id_[0]>>4) & 0xf); 
}
//
int ALCTnew::GetFastControlYear() { 
  return ((fastcontrol_id_[2]<<8) | fastcontrol_id_[1]&0xff); 
}
//
int ALCTnew::GetFastControlDay() { 
  return (fastcontrol_id_[3] & 0xff); 
}
//
int ALCTnew::GetFastControlMonth() { 
  return (fastcontrol_id_[4] & 0xff); 
}
//
////////////////////////////////
// DELAY LINE CONTROL REGISTER
////////////////////////////////
void ALCTnew::WriteDelayLineControlReg_() {
  //
  //  (*MyOutput_) << "ALCT: WRITE Delay Line CONTROL REGISTER" << std::endl;
  //
  FillDelayLineControlReg_();
  //
  setup_jtag(ChainAlctFastFpga);
  //
  ShfIR_ShfDR(ChipLocationAlctFastFpga,
	      ALCT_FAST_WRT_DELAYLINE_CTRL_REG,
	      RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_,
	      delay_line_control_reg_);
  return;
}
//
void ALCTnew::ReadDelayLineControlReg() {
  //
  for (int i=0; i<RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_; i++)
    delay_line_control_reg_[i] = 0;
  //
  setup_jtag(ChainAlctFastFpga);
  //
  ShfIR_ShfDR(ChipLocationAlctFastFpga,
	      ALCT_FAST_RD_DELAYLINE_CTRL_REG,
	      RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_);
  //
  int * register_pointer = GetDRtdo();
  for (int i=0; i<RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_; i++)
    delay_line_control_reg_[i] = *(register_pointer+i);
  //
  //Print out control register value in hex
  //  int scaler_delay_line_control_reg = bits_to_int(delay_line_control_reg_,
  //  						  RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_,
  //  						  LSBfirst);
  //  (*MyOutput_) << "ALCT: READ delay line CONTROL REGISTER = " 
  //  	       << std::hex << scaler_delay_line_control_reg << std::dec << std::endl;

  DecodeDelayLineControlReg_();
  //
  return;
}
//
void ALCTnew::SetDelayLineGroupSelect_(int group,
				       int mask){
  // Specify which group of delay chips you are talking to
  // bit = 1 = not enabled
  //     = 0 = enabled
  //
  if (group < 0 || group >= GetNumberOfGroupsOfDelayChips()) {
    (*MyOutput_) << "SetDelayLineGroupSelect: ALCT" << std::dec << GetNumberOfChannelsInAlct() 
		 << "-> group " << std::dec << group
		 << " invalid ... must be between 0 and " << std::dec << GetNumberOfGroupsOfDelayChips()-1 
		 << std::endl;
    (*MyOutput_) << "Delay Line Group unchanged" << std::endl;
    return;
  }
  delay_line_group_select_[group] = ~mask & 0x1;
  //
  return;
}
//
void ALCTnew::SetDelayLineSettst_(int mask) {
  // bit = 1 = not enabled
  //     = 0 = enabled
  //
  delay_line_settst_ = ~mask & 0x1; 
  return;
}  
//
void ALCTnew::SetDelayLineReset_(int mask) { 
  // bit = 1 = not enabled
  //     = 0 = enabled
  //
  delay_line_reset_ = ~mask & 0x1; 
  return;
}
//
void ALCTnew::DecodeDelayLineControlReg_() {
  // ** Extract the delay line control register's software values  **
  // ** from the vector of bits delay_line_control_reg_[]          **
  //
  int number_of_bits = delay_line_reset_bithi - delay_line_reset_bitlo + 1;  
  delay_line_reset_ = bits_to_int(delay_line_control_reg_+delay_line_reset_bitlo,
				  number_of_bits,
				  LSBfirst);
  //
  number_of_bits = delay_line_settst_bithi - delay_line_settst_bitlo + 1;  
  delay_line_settst_ = bits_to_int(delay_line_control_reg_+delay_line_settst_bitlo,
				   number_of_bits,
				   LSBfirst);
  //
  for (int index=0; index<GetNumberOfGroupsOfDelayChips(); index++)
    delay_line_group_select_[index] = delay_line_control_reg_[index+delay_line_group_select_bitlo];
  //
  return;
}
//
void ALCTnew::FillDelayLineControlReg_() {
  // ** Project the delay line control register's software values  **
  // ** into the vector of bits delay_line_control_reg_[]          **
  //
  int_to_bits(delay_line_reset_,
	      delay_line_reset_bithi-delay_line_reset_bitlo+1,
	      delay_line_control_reg_+delay_line_reset_bitlo,
	      LSBfirst);
  //
  int_to_bits(delay_line_settst_,
	      delay_line_settst_bithi-delay_line_settst_bitlo+1,
	      delay_line_control_reg_+delay_line_settst_bitlo,
	      LSBfirst);
  //
  for (int index=0; index<GetNumberOfGroupsOfDelayChips(); index++) 
    delay_line_control_reg_[index+delay_line_group_select_bitlo] 
      = delay_line_group_select_[index];
  //
  return;
}
//
void ALCTnew::PrintDelayLineControlReg() {
  //
  (*MyOutput_) << "ALCT Delay Line Control Register:" << std::endl;
  (*MyOutput_) << "----------------------------------" << std::endl;
  (*MyOutput_) << "delay_line_reset_  = " << std::dec 
	       << delay_line_reset_ << std::endl;
  (*MyOutput_) << "delay_line_settst_ = " << std::dec 
	       << delay_line_settst_ << std::endl;
  for (int index=0; index<GetNumberOfGroupsOfDelayChips(); index++) 
    (*MyOutput_) << "delay_line_group_select_[" << std::dec << index 
		 << "] = " << delay_line_group_select_[index] << std::endl;
  return;
}
//
void ALCTnew::SetPowerUpDelayLineControlReg_(){
  //
  SetDelayLineReset_(OFF);
  SetDelayLineSettst_(OFF);
  for (int group=0; group<7; group++) 
    SetDelayLineGroupSelect_(group,OFF);
  //
  return;
}
//
////////////////////////////////
// AFEB DELAYS and PATTERNS
////////////////////////////////
void ALCTnew::WriteAsicDelaysAndPatterns() {
  //
  (*MyOutput_) << "ALCT: WRITE asic DELAYS and PATTERNS for " 
	       << GetNumberOfGroupsOfDelayChips() << " groups" 
	       << std::endl;
  //
  SetPowerUpDelayLineControlReg_();         // reset the control register values
  WriteDelayLineControlReg_();
  //
  for (int group=0; group<GetNumberOfGroupsOfDelayChips(); group++) {
    // write values to one group of AFEBs at a time...
    SetDelayLineGroupSelect_(group,ON);     
    WriteDelayLineControlReg_();
    //    ReadDelayLineControlReg();
    //    PrintDelayLineControlReg();
    //
    FillAsicDelaysAndPatterns_(group);
    //
    setup_jtag(ChainAlctFastFpga);
    //
    ShfIR_ShfDR(ChipLocationAlctFastFpga,
		ALCT_FAST_WRT_ASIC_DELAY_LINES,
		RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES,
		asic_delays_and_patterns_);
    //
    SetDelayLineGroupSelect_(group,OFF);     
    WriteDelayLineControlReg_();
  }
  //
  return;
}
//
void ALCTnew::ReadAsicDelaysAndPatterns() {
  //
  (*MyOutput_) << "ALCT: READ asic DELAYS and PATTERNS for " 
	       << GetNumberOfGroupsOfDelayChips() << " groups" 
	       << std::endl;
  //
  SetPowerUpDelayLineControlReg_();         // reset the control register values
  WriteDelayLineControlReg_();
  //
  for (int group=0; group<GetNumberOfGroupsOfDelayChips(); group++) {
    //
    for (int i=0; i<RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES; i++)  //120 bits of good data
      asic_delays_and_patterns_[i] = 0;
    //
    // get values from one group of AFEBs at a time...
    SetDelayLineGroupSelect_(group,ON);     
    WriteDelayLineControlReg_();
    //    ReadDelayLineControlReg();
    //    PrintDelayLineControlReg();
    //
    setup_jtag(ChainAlctFastFpga);
    //
    ShfIR_ShfDR(ChipLocationAlctFastFpga,
		ALCT_FAST_RD_ASIC_DELAY_LINES,
		RegSizeAlctFastFpga_RD_ASIC_DELAY_LINES);

    //The first bit of the read register is junk--the data we want starts at index=1
    int * register_pointer = GetDRtdo();
    for (int i=1; i<RegSizeAlctFastFpga_RD_ASIC_DELAY_LINES; i++)  
      asic_delays_and_patterns_[i-1] = *(register_pointer+i);
    //
    DecodeAsicDelaysAndPatterns_(group);
    //
    SetDelayLineGroupSelect_(group,OFF);     
    WriteDelayLineControlReg_();
  }
  //
  return;
}
//
void ALCTnew::DecodeAsicDelaysAndPatterns_(int group) {
  // ** Extract the ASIC delays and pattern software values for 6 afebs and          **
  // ** corresponding channels from the vector of bits asic_delays_and_patterns_[]   **
  //
  //print out values read to screen:
  //  char values_read[RegSizeAlctFastFpga_RD_ASIC_DELAY_LINES/8];
  //  packCharBuffer(asic_delays_and_patterns_,
  //  		 RegSizeAlctFastFpga_RD_ASIC_DELAY_LINES,
  //  		 values_read);
  //  (*MyOutput_) << "Read values:  group" << group << "-> asic_delays_and_patterns = ";
  //  for (int char_counter=RegSizeAlctFastFpga_RD_ASIC_DELAY_LINES/8-1; char_counter>=0; char_counter--) {
  //    //      (*MyOutput_) << "char_counter " << std::dec << char_counter << " -> ";    
  //    (*MyOutput_) << std::hex
  //		 << ((values_read[char_counter] >> 4) & 0xf) 
  //		 << (values_read[char_counter] & 0xf) << " ";
  //  }
  //  (*MyOutput_) << std::endl;
  //    
  int minimum_afeb_in_group=group*NUMBER_OF_CHIPS_PER_GROUP;
  int maximum_afeb_in_group=(group+1)*NUMBER_OF_CHIPS_PER_GROUP;
  //
  int afeb_counter = 0;
  int wiregroup_counter = 0;
  //
  for (int afeb=minimum_afeb_in_group; afeb<maximum_afeb_in_group; afeb++) {
    int minimum_delay_bit = afeb_counter * NUMBER_OF_ASIC_BITS + asic_delay_value_bitlo;

    int number_of_bits = asic_delay_value_bithi - asic_delay_value_bitlo + 1;
    int delay_value = bits_to_int(asic_delays_and_patterns_+minimum_delay_bit,
				  number_of_bits,
				  LSBfirst);
    SetAsicDelay(afeb,delay_value);

    //location of pattern value bits for this afeb in asic_delays_and_patterns[]:
    int minimum_pattern_bit = afeb_counter * NUMBER_OF_ASIC_BITS + asic_pattern_value_bitlo;
    int maximum_pattern_bit = afeb_counter * NUMBER_OF_ASIC_BITS + asic_pattern_value_bithi;
    //
    for (int bit=minimum_pattern_bit; bit<=maximum_pattern_bit; bit++) {
      // Use the asic-group map to find which layer and channels are in this group:
      int layer_number = GetLayerFromAsicMap_(wiregroup_counter);
      int wiregroup_number = GetChannelFromAsicMap_(group,wiregroup_counter);
      //
      SetAsicPattern(layer_number,wiregroup_number,asic_delays_and_patterns_[bit]);
      //
      wiregroup_counter++;
    }
    afeb_counter++;
  }
  return;
}
//
void ALCTnew::FillAsicDelaysAndPatterns_(int group) {
  // ** Project the software values of the delays and patterns  **
  // ** for this group of AFEBs into the vector of bits         **
  // ** asic_delays_and_patterns_[]                             **
  //
  int minimum_afeb_in_group=group*NUMBER_OF_CHIPS_PER_GROUP;
  int maximum_afeb_in_group=(group+1)*NUMBER_OF_CHIPS_PER_GROUP;
  //
  int afeb_counter = 0;
  int wiregroup_counter = 0;
  //
  for (int afeb=minimum_afeb_in_group; afeb<maximum_afeb_in_group; afeb++) {
    //location of delay value bits for this afeb in asic_delays_and_patterns_[]:
    int minimum_delay_bit = afeb_counter * NUMBER_OF_ASIC_BITS + asic_delay_value_bitlo;
    //
    int_to_bits(GetAsicDelay(afeb),
		asic_delay_value_bithi-asic_delay_value_bitlo+1,
		asic_delays_and_patterns_+minimum_delay_bit,
		LSBfirst);
    //
    //location of pattern value bits for this afeb in asic_delays_and_patterns[]:
    int minimum_pattern_bit = afeb_counter * NUMBER_OF_ASIC_BITS + asic_pattern_value_bitlo;
    int maximum_pattern_bit = afeb_counter * NUMBER_OF_ASIC_BITS + asic_pattern_value_bithi;
    //
    for (int bit=minimum_pattern_bit; bit<=maximum_pattern_bit; bit++) {
      // Use the asic-group map to find which layer and channels are in this group:
      int layer_number = GetLayerFromAsicMap_(wiregroup_counter);
      int wiregroup_number = GetChannelFromAsicMap_(group,wiregroup_counter);
      //
      // Specify the order of the bits in asic_delays_and_patterns_[]:
      asic_delays_and_patterns_[bit] = GetAsicPattern(layer_number,wiregroup_number);
      //
      wiregroup_counter++;
    }
    //
    afeb_counter++;
  }
  //print out values written to screen:
  //  char values_written[RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES/8];
  //  packCharBuffer(asic_delays_and_patterns_,
  //  		 RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES,
  //  		 values_written);
  //  (*MyOutput_) << "To write:  group" << group << "-> asic_delays_and_patterns = ";
  //  for (int char_counter=RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES/8-1; char_counter>=0; char_counter--) {
  //    //      (*MyOutput_) << "char_counter " << std::dec << char_counter << " -> ";    
  //    (*MyOutput_) << std::hex
  //		 << ((values_written[char_counter] >> 4) & 0xf) 
  //		 << (values_written[char_counter] & 0xf) << " ";
  //  }
  //  (*MyOutput_) << std::endl;
  //
  return;
}
//
int ALCTnew::GetLayerFromAsicMap_(int asic_index) {
  return asic_layer_map[asic_index];
}
//
int ALCTnew::GetChannelFromAsicMap_(int group, int asic_index) {
  // Each asic map is the same, except shifted by 16 wires per 96-channel group:
  return asic_wiregroup_map[asic_index] + group*NUMBER_OF_LINES_PER_CHIP; 
}
//
void ALCTnew::SetAsicDelay(int afebChannel,
			   int delay) {
  if (delay<0 || delay > 15) {
    (*MyOutput_) << "SetAsicDelay: ERROR delay should be between 0 and 15" << std::endl;
    return;
  } 
  //
  if (afebChannel >= GetNumberOfAfebs()) {
    (*MyOutput_) << "SetAsicDelay: ALCT" << std::dec << GetNumberOfChannelsInAlct() 
		 << "-> channel " << std::dec << afebChannel
		 << " invalid ... must be between 0 and " << std::dec << GetNumberOfAfebs()-1 
		 << std::endl;
    return;
  } 
  //
  asic_delay_[afebChannel] = delay & 0xf;
  //
  return;

}
//
int ALCTnew::GetAsicDelay(int afebChannel) {
  if (afebChannel >= GetNumberOfAfebs()) {
    (*MyOutput_) << "GetAsicDelay: ALCT" << std::dec << GetNumberOfChannelsInAlct() 
		 << "-> channel " << std::dec << afebChannel
		 << " invalid ... must be between 0 and " << std::dec << GetNumberOfAfebs()-1 
		 << std::endl;
    return 999;
  } 
  //
  return asic_delay_[afebChannel];
}
//
void ALCTnew::SetPowerUpAsicDelays() {
  //
  for (int afeb=0; afeb<MAX_NUM_AFEBS; afeb++)
    SetAsicDelay(afeb,0);
  //
  return;
}
//
void ALCTnew::PrintAsicDelays() {
  //
  (*MyOutput_) << "Asic delay values:" << std::endl;
  (*MyOutput_) << "AFEB   delay (2ns)" << std::endl;
  (*MyOutput_) << "----   -----------" << std::endl;
  //  for (int afeb=0; afeb<GetNumberOfAfebs(); afeb++)
  for (int afeb=0; afeb<MAX_NUM_AFEBS; afeb++)
    (*MyOutput_) << " " << std::dec << afeb << "     " << GetAsicDelay(afeb) << std::endl;
  //
  return;
}
//
void ALCTnew::SetAsicPattern(int layer,
			     int channel,
			     int on_or_off) {
  if (layer<0 || layer>=MAX_NUM_LAYERS) {
    (*MyOutput_) << "SetAsicPattern: layer " << std::dec << layer
		 << " invalid ... must be between 0 and " << MAX_NUM_LAYERS-1 
		 << std::endl;
    return;
  } 
  //
  if (channel<0 || channel>=GetNumberOfChannelsPerLayer() ) {
    (*MyOutput_) << "SetAsicPattern: ALCT" << std::dec << GetNumberOfChannelsInAlct() 
		 << " -> channel " << std::dec << channel
		 << " invalid ... must be between 0 and " << GetNumberOfChannelsPerLayer()-1 
		 << std::endl;
    return;
  } 
  asic_pattern_[layer][channel] = on_or_off & 0x1;
  //
  return;
}
//
int ALCTnew::GetAsicPattern(int layer,
			    int channel) {
  if (layer<0 || layer>=MAX_NUM_LAYERS) {
    (*MyOutput_) << "GetAsicPattern: layer " << std::dec << layer
		 << " invalid ... must be between 0 and " << MAX_NUM_LAYERS-1
		 << std::endl;
    return 999;
  } 
  //
  if (channel<0 || channel>=GetNumberOfChannelsPerLayer() ) {
    (*MyOutput_) << "GetAsicPattern: ALCT" << std::dec << GetNumberOfChannelsInAlct() 
		 << " -> channel " << std::dec << channel
		 << " invalid ... must be between 0 and " << GetNumberOfChannelsPerLayer()-1 
		 << std::endl;
    return 999;
  } 
  //
  return asic_pattern_[layer][channel];
}
//
void ALCTnew::SetPowerUpAsicPatterns() {
  //
  for (int layer=0; layer<MAX_NUM_LAYERS; layer++)
    for (int channel=0; channel<MAX_NUM_WIRES_PER_LAYER; channel++)
      SetAsicPattern(layer,channel,OFF);
  //
  return;
}
//
void ALCTnew::PrintAsicPatterns() {
  //
  char pattern[MAX_NUM_LAYERS][MAX_NUM_WIRES_PER_LAYER/8];
  for (int layer=0; layer<MAX_NUM_LAYERS; layer++)
    packCharBuffer(asic_pattern_[layer],
		   GetNumberOfChannelsPerLayer(),
		   pattern[layer]);
  //
  (*MyOutput_) << "Asic pattern for ALCT" << std::dec << GetNumberOfChannelsInAlct() 
	       << " (from right to left):" << std::endl;
  //
  for (int layer=5; layer>=0; layer--) {
    (*MyOutput_) << "Layer " << std::dec << layer << " -> ";    
    //
    for (int char_counter=(GetNumberOfChannelsPerLayer()/8)-1; char_counter>=0; char_counter--) {
      (*MyOutput_) << std::hex
      		   << ((pattern[layer][char_counter] >> 4) & 0xf) 
      		   << (pattern[layer][char_counter] & 0xf) << " ";
    }
      (*MyOutput_) << std::endl;
  }
  //
  return;
}
//
////////////////////////////////
// CONFIGURATION REGISTER
////////////////////////////////
void ALCTnew::WriteConfigurationReg() {
  //
  (*MyOutput_) << "ALCT: WRITE Configuration Register" << std::endl;
  //
  FillConfigurationReg_();
  //
  setup_jtag(ChainAlctFastFpga);
  //
  ShfIR_ShfDR(ChipLocationAlctFastFpga,
  	      ALCT_FAST_WRT_CONFIG_REG,
  	      RegSizeAlctFastFpga_WRT_CONFIG_REG,
	      config_reg_);
  return;
}
//
void ALCTnew::ReadConfigurationReg() {
  // Fill config_reg_ vector with information currently in ALCT
  //
  for (int i=0; i<RegSizeAlctFastFpga_RD_CONFIG_REG; i++)
    config_reg_[i] = 0;
  //
  setup_jtag(ChainAlctFastFpga);
  //
  ShfIR_ShfDR(ChipLocationAlctFastFpga,
	      ALCT_FAST_RD_CONFIG_REG,
	      RegSizeAlctFastFpga_RD_CONFIG_REG);
  //
  int * register_pointer = GetDRtdo();
  for (int i=0; i<RegSizeAlctFastFpga_RD_CONFIG_REG; i++)
    config_reg_[i] = *(register_pointer+i);
  //
  //Print out configuration register in hex...
  char configuration_register[RegSizeAlctFastFpga_RD_CONFIG_REG/8];
  packCharBuffer(config_reg_,
		 RegSizeAlctFastFpga_RD_CONFIG_REG,
		 configuration_register);
  //
  (*MyOutput_) << "ALCT: READ configuration register = 0x";
  for (int counter=RegSizeAlctFastFpga_RD_CONFIG_REG/8; counter>=0; counter--) 
    (*MyOutput_) << std::hex
		 << ((configuration_register[counter] >> 4) & 0xf) 
		 << (configuration_register[counter] & 0xf);
  (*MyOutput_) << std::endl;
  //
  DecodeConfigurationReg_();
  return;
}
//
void ALCTnew::PrintConfigurationReg() {
  //
  (*MyOutput_) << "ALCT configuration register:" << std::endl;
  (*MyOutput_) << "----------------------------" << std::endl;
  (*MyOutput_) << "trigger_mode_    = " << std::dec 
	       << GetTriggerMode() << std::endl;
  (*MyOutput_) << "ext_trig_enable_ = " << std::dec 
	       << GetExtTrigEnable() << std::endl;
  (*MyOutput_) << "send_empty_      = " << std::dec 
	       << GetSendEmpty() << std::endl;
  (*MyOutput_) << "inject_          = " << std::dec 
	       << GetInject() << std::endl;
  (*MyOutput_) << "bxc_offset_      = " << std::dec 
	       << GetBxcOffset() << std::endl;
  (*MyOutput_) << "nph_thresh_      = " << std::dec 
	       << GetNphThresh() << std::endl;
  (*MyOutput_) << "nph_pattern_     = " << std::dec 
	       << GetNphPattern() << std::endl;
  (*MyOutput_) << "drift_delay_     = " << std::dec
	       << GetDriftDelay() << std::endl;
  (*MyOutput_) << "fifo_tbins_      = " << std::dec 
	       << GetFifoTbins() << std::endl;
  (*MyOutput_) << "fifo_pretrig_    = " << std::dec 
	       << GetFifoPretrig() << std::endl;
  (*MyOutput_) << "fifo_mode_       = " << std::dec
	       << GetFifoMode() << std::endl;
  (*MyOutput_) << "fifo_lastlct_    = " << std::dec
	       << GetFifoLastLct() << std::endl;
  (*MyOutput_) << "l1a_delay_       = " << std::dec 
	       << GetL1aDelay() << " = 0x" << std::hex
	       << GetL1aDelay() << std::endl;
  (*MyOutput_) << "l1a_window_      = " << std::dec
	       << GetL1aWindow() << std::endl;
  (*MyOutput_) << "l1a_offset_      = " << std::dec
	       << GetL1aOffset() << std::endl;
  (*MyOutput_) << "l1a_internal_    = " << std::dec
	       << GetL1aInternal() << std::endl;
  (*MyOutput_) << "board_id_        = " << std::dec
	       << GetBoardId() << std::endl;
  (*MyOutput_) << "bxn_offset_      = " << std::dec
	       << GetBxnOffset() << std::endl;
  (*MyOutput_) << "ccb_enable_      = " << std::dec
	       << GetCcbEnable() << std::endl;
  (*MyOutput_) << "alct_jtag_ds_    = " << std::dec
	       << GetAlctJtagDs() << std::endl;
  (*MyOutput_) << "alct_tmode_      = " << std::dec
	       << GetAlctTmode() << std::endl;
  (*MyOutput_) << "alct_amode_      = " << std::dec
	       << GetAlctAmode() << std::endl;
  (*MyOutput_) << "alct_mask_all_   = " << std::dec
	       << GetAlctMaskAll() << std::endl;
  (*MyOutput_) << "trigger_info_en_ = " << std::dec
	       << GetTriggerInfoEn() << std::endl;
  (*MyOutput_) << "sn_select_       = " << std::dec
	       << GetSnSelect() << std::endl;  
  return;
}
//
void ALCTnew::SetTriggerMode(int trigger_mode) { 
  trigger_mode_ = trigger_mode; 
  return;
}
//
int ALCTnew::GetTriggerMode() { 
  return trigger_mode_; 
}
//
void ALCTnew::SetExtTrigEnable(int ext_trig_enable) { 
  ext_trig_enable_ = ext_trig_enable; 
  return;
}
//
int ALCTnew::GetExtTrigEnable() { 
  return ext_trig_enable_; 
}
//
void ALCTnew::SetSendEmpty(int send_empty) { 
  send_empty_ = send_empty; 
  return;
}
//
int ALCTnew::GetSendEmpty() { 
  return send_empty_; 
}
//
void ALCTnew::SetInject(int inject) { 
  inject_ = inject; 
  return;
}
//
int ALCTnew::GetInject() { 
  return inject_; 
}
//
void ALCTnew::SetBxcOffset(int bxc_offset) { 
  bxc_offset_ = bxc_offset; 
  return;
}
//
int ALCTnew::GetBxcOffset() { 
  return bxc_offset_; 
}
//
void ALCTnew::SetNphThresh(int nph_thresh) { 
  nph_thresh_ = nph_thresh; 
  return;
}
//
int ALCTnew::GetNphThresh() { 
  return nph_thresh_; 
}
//
void ALCTnew::SetNphPattern(int nph_pattern) { 
  nph_pattern_ = nph_pattern; 
  return;
}
//
int ALCTnew::GetNphPattern() { 
  return nph_pattern_; 
}
//
void ALCTnew::SetDriftDelay(int drift_delay) { 
  drift_delay_ = drift_delay; 
  return;
}
//
int ALCTnew::GetDriftDelay() { 
  return drift_delay_; 
}
//
void ALCTnew::SetFifoTbins(int fifo_tbins) { 
  fifo_tbins_ = fifo_tbins; 
  return;
}
//
int ALCTnew::GetFifoTbins() { 
  return fifo_tbins_; 
}
//
void ALCTnew::SetFifoPretrig(int fifo_pretrig) { 
  fifo_pretrig_ = fifo_pretrig; 
  return;
}
//
int ALCTnew::GetFifoPretrig() { 
  return fifo_pretrig_; 
}
//
void ALCTnew::SetFifoMode(int fifo_mode) { 
  fifo_mode_ = fifo_mode; 
  return;
}
//
int ALCTnew::GetFifoMode() { 
  return fifo_mode_; 
}
//
void ALCTnew::SetFifoLastLct(int fifo_lastlct) { 
  fifo_lastlct_ = fifo_lastlct; 
  return;
}
//
int ALCTnew::GetFifoLastLct() { 
  return fifo_lastlct_; 
}
//
void ALCTnew::SetL1aDelay(int l1a_delay) { 
  l1a_delay_ = l1a_delay; 
  return;
}
//
int ALCTnew::GetL1aDelay() { 
  return l1a_delay_; 
}
//
void ALCTnew::SetL1aWindow(int l1a_window) { 
  l1a_window_ = l1a_window; 
  return;
}
//
int ALCTnew::GetL1aWindow() { 
  return l1a_window_; 
}
//
void ALCTnew::SetL1aOffset(int l1a_offset) { 
  l1a_offset_ = l1a_offset; 
  return;
}
//
int ALCTnew::GetL1aOffset() { 
  return l1a_offset_; 
}
//
void ALCTnew::SetL1aInternal(int l1a_internal) { 
  l1a_internal_ = l1a_internal; 
  return;
}
int ALCTnew::GetL1aInternal() { 
  return l1a_internal_; 
}
//
void ALCTnew::SetBoardId(int board_id) { 
  board_id_ = board_id; 
  return;
}
//
int ALCTnew::GetBoardId() { 
  return board_id_; 
}
//
void ALCTnew::SetBxnOffset(int bxn_offset) { 
  bxn_offset_ = bxn_offset; 
  return;
}
//
int ALCTnew::GetBxnOffset() { 
  return bxn_offset_; 
}
//
void ALCTnew::SetCcbEnable(int ccb_enable) { 
  ccb_enable_ = ccb_enable; 
  return;
}
//
int ALCTnew::GetCcbEnable() { 
  return ccb_enable_; 
}
//
void ALCTnew::SetAlctJtagDs(int alct_jtag_ds) { 
  alct_jtag_ds_ = alct_jtag_ds; 
  return;
}
int ALCTnew::GetAlctJtagDs() { 
  return alct_jtag_ds_; 
}
//
void ALCTnew::SetAlctTmode(int alct_tmode) { 
  alct_tmode_ = alct_tmode; 
  return;
}
//
int ALCTnew::GetAlctTmode() { 
  return alct_tmode_; 
}
//
void ALCTnew::SetAlctAmode(int alct_amode) { 
  alct_amode_ = alct_amode; 
  return;
}
//
int ALCTnew::GetAlctAmode() { 
  return alct_amode_; 
}
//
void ALCTnew::SetAlctMaskAll(int alct_mask_all) { 
  alct_mask_all_ = alct_mask_all; 
  return;
}
int ALCTnew::GetAlctMaskAll() { 
  return alct_mask_all_; 
}
//
void ALCTnew::SetTriggerInfoEn(int trigger_info_en) { 
  trigger_info_en_ = trigger_info_en; 
  return;
}
int ALCTnew::GetTriggerInfoEn() { 
  return trigger_info_en_; 
}
//
void ALCTnew::SetSnSelect(int sn_select) { 
  sn_select_ = sn_select; 
  return;
}
int ALCTnew::GetSnSelect() { 
  return sn_select_; 
}
//
void ALCTnew::DecodeConfigurationReg_(){
  // ** Extract the configuration register's software values  **
  // ** from the vector of bits config_reg_[]                 **
  //
  int number_of_bits = trigger_mode_bithi - trigger_mode_bitlo + 1;
  trigger_mode_ = bits_to_int(config_reg_+trigger_mode_bitlo,
			      number_of_bits,
			      LSBfirst);
  //
  number_of_bits = ext_trig_enable_bithi - ext_trig_enable_bitlo + 1;
  ext_trig_enable_ = bits_to_int(config_reg_+ext_trig_enable_bitlo,
				 number_of_bits,
				 LSBfirst);
  //
  number_of_bits = send_empty_bithi - send_empty_bitlo + 1;
  send_empty_ = bits_to_int(config_reg_+send_empty_bitlo,
			    number_of_bits,
			    LSBfirst);
  //
  number_of_bits = inject_bithi - inject_bitlo + 1;
  inject_ = bits_to_int(config_reg_+inject_bitlo,
			number_of_bits,
			LSBfirst);
  //
  number_of_bits = bxc_offset_bithi - bxc_offset_bitlo + 1;
  bxc_offset_ = bits_to_int(config_reg_+bxc_offset_bitlo,
			    number_of_bits,
			    LSBfirst);
  //
  number_of_bits = nph_thresh_bithi - nph_thresh_bitlo + 1;
  nph_thresh_ = bits_to_int(config_reg_+nph_thresh_bitlo,
			    number_of_bits,
			    LSBfirst);
  //
  number_of_bits = nph_pattern_bithi - nph_pattern_bitlo + 1;
  nph_pattern_ = bits_to_int(config_reg_+nph_pattern_bitlo,
			     number_of_bits,
			     LSBfirst);
  //
  number_of_bits = drift_delay_bithi - drift_delay_bitlo + 1;
  drift_delay_ = bits_to_int(config_reg_+drift_delay_bitlo,
			     number_of_bits,
			     LSBfirst);
  //
  number_of_bits = fifo_tbins_bithi - fifo_tbins_bitlo + 1;
  fifo_tbins_ = bits_to_int(config_reg_+fifo_tbins_bitlo,
			    number_of_bits,
			    LSBfirst);
  //
  number_of_bits = fifo_pretrig_bithi - fifo_pretrig_bitlo + 1;
  fifo_pretrig_ = bits_to_int(config_reg_+fifo_pretrig_bitlo,
			      number_of_bits,
			      LSBfirst);
  //
  number_of_bits = fifo_mode_bithi - fifo_mode_bitlo + 1;
  fifo_mode_ = bits_to_int(config_reg_+fifo_mode_bitlo,
			   number_of_bits,
			   LSBfirst);
  //
  number_of_bits = fifo_lastlct_bithi - fifo_lastlct_bitlo + 1;
  fifo_lastlct_ = bits_to_int(config_reg_+fifo_lastlct_bitlo,
			      number_of_bits,
			      LSBfirst);
  //
  number_of_bits = l1a_delay_bithi - l1a_delay_bitlo + 1;
  l1a_delay_ = bits_to_int(config_reg_+l1a_delay_bitlo,
			   number_of_bits,
			   LSBfirst);
  //
  number_of_bits = l1a_window_bithi - l1a_window_bitlo + 1;
  l1a_window_ = bits_to_int(config_reg_+l1a_window_bitlo,
			    number_of_bits,
			    LSBfirst);
  //
  number_of_bits = l1a_offset_bithi - l1a_offset_bitlo + 1;
  l1a_offset_ = bits_to_int(config_reg_+l1a_offset_bitlo,
			    number_of_bits,
			    LSBfirst);
  //
  number_of_bits = l1a_internal_bithi - l1a_internal_bitlo + 1;
  l1a_internal_ = bits_to_int(config_reg_+l1a_internal_bitlo,
			      number_of_bits,
			      LSBfirst);
  //
  number_of_bits = board_id_bithi - board_id_bitlo + 1;
  board_id_ = bits_to_int(config_reg_+board_id_bitlo,
			  number_of_bits,
			  LSBfirst);
  //
  number_of_bits = bxn_offset_bithi - bxn_offset_bitlo + 1;
  bxn_offset_ = bits_to_int(config_reg_+bxn_offset_bitlo,
			    number_of_bits,
			    LSBfirst);
  //
  number_of_bits = ccb_enable_bithi - ccb_enable_bitlo + 1;
  ccb_enable_ = bits_to_int(config_reg_+ccb_enable_bitlo,
			    number_of_bits,
			    LSBfirst);
  //
  number_of_bits = alct_jtag_ds_bithi - alct_jtag_ds_bitlo + 1;
  alct_jtag_ds_ = bits_to_int(config_reg_+alct_jtag_ds_bitlo,
			      number_of_bits,
			      LSBfirst);
  //
  number_of_bits = alct_tmode_bithi - alct_tmode_bitlo + 1;
  alct_tmode_ = bits_to_int(config_reg_+alct_tmode_bitlo,
			    number_of_bits,
			    LSBfirst);
  //
  number_of_bits = alct_amode_bithi - alct_amode_bitlo + 1;
  alct_amode_ = bits_to_int(config_reg_+alct_amode_bitlo,
			    number_of_bits,
			    LSBfirst);
  //
  number_of_bits = alct_mask_all_bithi - alct_mask_all_bitlo + 1;
  alct_mask_all_ = bits_to_int(config_reg_+alct_mask_all_bitlo,
			       number_of_bits,
			       LSBfirst);
  //
  number_of_bits = trigger_info_en_bithi - trigger_info_en_bitlo + 1;
  trigger_info_en_ = bits_to_int(config_reg_+trigger_info_en_bitlo,
				 number_of_bits,
				 LSBfirst);
  //
  number_of_bits = sn_select_bithi - sn_select_bitlo + 1;
  sn_select_ = bits_to_int(config_reg_+sn_select_bitlo,
			   number_of_bits,
			   LSBfirst);
  //
  return;
}
//
void ALCTnew::FillConfigurationReg_(){
  // ** Project the configuration register's software values  **
  // ** into the vector of bits config_reg_[]                 **
  //
  int_to_bits(trigger_mode_,
	      trigger_mode_bithi-trigger_mode_bitlo+1,
	      config_reg_+trigger_mode_bitlo,
	      LSBfirst);
  //
  int_to_bits(ext_trig_enable_,
	      ext_trig_enable_bithi-ext_trig_enable_bitlo+1,
	      config_reg_+ext_trig_enable_bitlo,
	      LSBfirst);
  //
  int_to_bits(send_empty_,
	      send_empty_bithi-send_empty_bitlo+1,
	      config_reg_+send_empty_bitlo,
	      LSBfirst);
  //
  int_to_bits(inject_,
	      inject_bithi-inject_bitlo+1,
	      config_reg_+inject_bitlo,
	      LSBfirst);
  //
  int_to_bits(bxc_offset_,
	      bxc_offset_bithi-bxc_offset_bitlo+1,
	      config_reg_+bxc_offset_bitlo,
	      LSBfirst);
  //
  int_to_bits(nph_thresh_,
	      nph_thresh_bithi-nph_thresh_bitlo+1,
	      config_reg_+nph_thresh_bitlo,
	      LSBfirst);
  //
  int_to_bits(nph_pattern_,
	      nph_pattern_bithi-nph_pattern_bitlo+1,
	      config_reg_+nph_pattern_bitlo,
	      LSBfirst);
  //
  int_to_bits(drift_delay_,
	      drift_delay_bithi-drift_delay_bitlo+1,
	      config_reg_+drift_delay_bitlo,
	      LSBfirst);
  //
  int_to_bits(fifo_tbins_,
	      fifo_tbins_bithi-fifo_tbins_bitlo+1,
	      config_reg_+fifo_tbins_bitlo,
	      LSBfirst);
  //
  int_to_bits(fifo_pretrig_,
	      fifo_pretrig_bithi-fifo_pretrig_bitlo+1,
	      config_reg_+fifo_pretrig_bitlo,
	      LSBfirst);
  //
  int_to_bits(fifo_mode_,
	      fifo_mode_bithi-fifo_mode_bitlo+1,
	      config_reg_+fifo_mode_bitlo,
	      LSBfirst);
  //
  int_to_bits(fifo_lastlct_,
	      fifo_lastlct_bithi-fifo_lastlct_bitlo+1,
	      config_reg_+fifo_lastlct_bitlo,
	      LSBfirst);
  //
  int_to_bits(l1a_delay_,
	      l1a_delay_bithi-l1a_delay_bitlo+1,
	      config_reg_+l1a_delay_bitlo,
	      LSBfirst);
  //
  int_to_bits(l1a_window_,
	      l1a_window_bithi-l1a_window_bitlo+1,
	      config_reg_+l1a_window_bitlo,
	      LSBfirst);
  //
  int_to_bits(l1a_offset_,
	      l1a_offset_bithi-l1a_offset_bitlo+1,
	      config_reg_+l1a_offset_bitlo,
	      LSBfirst);
  //
  int_to_bits(l1a_internal_,
	      l1a_internal_bithi-l1a_internal_bitlo+1,
	      config_reg_+l1a_internal_bitlo,
	      LSBfirst);
  //
  int_to_bits(board_id_,
	      board_id_bithi-board_id_bitlo+1,
	      config_reg_+board_id_bitlo,
	      LSBfirst);
  //
  int_to_bits(bxn_offset_,
	      bxn_offset_bithi-bxn_offset_bitlo+1,
	      config_reg_+bxn_offset_bitlo,
	      LSBfirst);
  //
  int_to_bits(ccb_enable_,
	      ccb_enable_bithi-ccb_enable_bitlo+1,
	      config_reg_+ccb_enable_bitlo,
	      LSBfirst);
  //
  int_to_bits(alct_jtag_ds_,
	      alct_jtag_ds_bithi-alct_jtag_ds_bitlo+1,
	      config_reg_+alct_jtag_ds_bitlo,
	      LSBfirst);
  //
  int_to_bits(alct_tmode_,
	      alct_tmode_bithi-alct_tmode_bitlo+1,
	      config_reg_+alct_tmode_bitlo,
	      LSBfirst);
  //
  int_to_bits(alct_amode_,
	      alct_amode_bithi-alct_amode_bitlo+1,
	      config_reg_+alct_amode_bitlo,
	      LSBfirst);
  //
  int_to_bits(alct_mask_all_,
	      alct_mask_all_bithi-alct_mask_all_bitlo+1,
	      config_reg_+alct_mask_all_bitlo,
	      LSBfirst);
  //
  int_to_bits(trigger_info_en_,
	      trigger_info_en_bithi-trigger_info_en_bitlo+1,
	      config_reg_+trigger_info_en_bitlo,
	      LSBfirst);
  //
  int_to_bits(sn_select_,
	      sn_select_bithi-sn_select_bitlo+1,
	      config_reg_+sn_select_bitlo,
	      LSBfirst);
  return;
}
//
void ALCTnew::SetPowerUpConfigurationReg() {
  //
  SetTriggerMode(0);
  SetExtTrigEnable(0);
  SetSendEmpty(0);
  SetInject(0);
  SetBxcOffset(1);
  SetNphThresh(2);
  SetNphPattern(4);
  SetDriftDelay(3);
  SetFifoTbins(7);
  SetFifoPretrig(1);
  SetFifoMode(1);
  SetFifoLastLct(3);
  SetL1aDelay(120);
  SetL1aWindow(3);
  SetL1aOffset(1);
  SetL1aInternal(0);
  SetBoardId(5);
  SetBxnOffset(0);
  SetCcbEnable(0);
  SetAlctJtagDs(1);
  SetAlctTmode(0);
  SetAlctAmode(0);
  SetAlctMaskAll(0);
  SetTriggerInfoEn(1);
  SetSnSelect(0);
  //
  return;
}
//
//////////////////////////////
// HOT CHANNEL MASK
//////////////////////////////
void ALCTnew::WriteHotChannelMask() {
  //
  (*MyOutput_) << "ALCT: WRITE hot channel mask" << std::endl;
  //
  setup_jtag(ChainAlctFastFpga);
  //
  ShfIR_ShfDR(ChipLocationAlctFastFpga,
  	      ALCT_FAST_WRT_HOTCHAN_MASK,
  	      RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_,
	      hot_channel_mask_);
  return;
}
//
void ALCTnew::ReadHotChannelMask() {
  // Fill hot_channel_mask_ vector with information currently in ALCT
  //
  (*MyOutput_) << "ALCT: READ hot channel mask (destructive), so write it back in:" << std::endl;
  //
  for (int i=0; i<RegSizeAlctFastFpga_RD_HOTCHAN_MASK_672; i++)
    hot_channel_mask_[i] = 0;
  //
  setup_jtag(ChainAlctFastFpga);
  //
  ShfIR_ShfDR(ChipLocationAlctFastFpga,
	      ALCT_FAST_RD_HOTCHAN_MASK,
	      RegSizeAlctFastFpga_RD_HOTCHAN_MASK_);
  //
  int * register_pointer = GetDRtdo();
  for (int i=0; i<RegSizeAlctFastFpga_RD_HOTCHAN_MASK_; i++)
    hot_channel_mask_[i] = *(register_pointer+i);
  //
  //The read of the hot channel mask is destructive, so it needs to be reloaded:
  WriteHotChannelMask();
  //
  return;
}
//
void ALCTnew::PrintHotChannelMask() {
  // Print out hot channel mask in hex for each layer 
  // from right (channel 0) to left (number of channels in layer)
  //
  char hot_channel_mask[RegSizeAlctFastFpga_RD_HOTCHAN_MASK_/8];
  packCharBuffer(hot_channel_mask_,
		 RegSizeAlctFastFpga_RD_HOTCHAN_MASK_,
		 hot_channel_mask);
  //
  int char_counter = RegSizeAlctFastFpga_RD_HOTCHAN_MASK_/8 - 1;
  //
  (*MyOutput_) << "Hot Channel Mask for ALCT" << std::dec << GetNumberOfChannelsInAlct() 
	       << " (from right to left):" << std::endl;
  //
  for (int layer=6; layer>0; layer--) {
    (*MyOutput_) << "Layer " << std::dec << layer << " -> ";    
    for (int layer_counter=GetNumberOfChannelsPerLayer()/8; layer_counter>0; layer_counter--) {
      //      (*MyOutput_) << "char_counter " << std::dec << char_counter << " -> ";    
      (*MyOutput_) << std::hex
		   << ((hot_channel_mask[char_counter] >> 4) & 0xf) 
		   << (hot_channel_mask[char_counter] & 0xf) << " ";
      char_counter--;
    }
      (*MyOutput_) << std::endl;
  }
  return;
}
//
void ALCTnew::SetHotChannelMask(int layer,
				int channel,
				int on_or_off) {
  if (layer < 1 || layer > 6) {
    (*MyOutput_) << "SetHotChannelMask: layer " << layer 
		 << "... must be between 1 and 6" << std::endl;
    (*MyOutput_) << "Hot Channel Mask Unchanged" << std::endl;
    return;
  }
  if (channel < 1 || channel > GetNumberOfChannelsPerLayer()) {
    (*MyOutput_) << "SetHotChannelMask: ALCT" << std::dec << GetNumberOfChannelsInAlct() 
		 << "-> channel " << std::dec << channel 
		 << " invalid ... must be between 1 and " << std::dec << GetNumberOfChannelsPerLayer() 
		 << std::endl;
    (*MyOutput_) << "Hot Channel Mask Unchanged" << std::endl;
    return;
  }
  //
  //index in hot channel mask is determined by layer number and channel number within the layer:
  int index = (layer-1) * GetNumberOfChannelsPerLayer() + channel - 1;
  //
  hot_channel_mask_[index] = on_or_off;
  //
  return;
}
//
int ALCTnew::GetHotChannelMask(int layer,
			       int channel) {
  if (layer < 1 || layer > 6) {
    (*MyOutput_) << "SetHotChannelMask: layer " << layer 
		 << "... must be between 1 and 6" << std::endl;
    return -1;
  }
  if (channel < 1 || channel > GetNumberOfChannelsPerLayer()) {
    (*MyOutput_) << "SetHotChannelMask: ALCT" << std::dec << GetNumberOfChannelsInAlct() 
		 << "-> channel " << std::dec << channel 
		 << " invalid ... must be between 1 and " << std::dec << GetNumberOfChannelsPerLayer() 
		 << std::endl;
    return -1;
  }
  //
  //index in hot channel mask is determined by layer number and channel number within the layer:
  int index = (layer-1) * GetNumberOfChannelsPerLayer() + channel - 1;
  //
  return hot_channel_mask_[index];
}
//
void ALCTnew::SetPowerUpHotChannelMask() {
  //
  for (int channel=0; channel<RegSizeAlctFastFpga_RD_HOTCHAN_MASK_672; channel++)
    hot_channel_mask_[channel] = ON;
  //
  return;
}
//
//////////////////////////////
// CHAMBER CHARACTERISTICS
//////////////////////////////
void ALCTnew::SetChamberCharacteristics_(std::string chamberType) {
  //
  chamber_type_string_ = chamberType;
  NumberOfWireGroupsInChamber_ = 0;
  //
  (*MyOutput_) << "Chamber is " << chamber_type_string_ << "->" << std::endl;
  //
  if (chamber_type_string_ == "ME11") {
    NumberOfWireGroupsInChamber_ = 288;
    SetAlctType_(288);
  } else if (chamber_type_string_ == "ME12") {
    NumberOfWireGroupsInChamber_ = 384;
    SetAlctType_(384);
  } else if (chamber_type_string_ == "ME13") {
    NumberOfWireGroupsInChamber_ = 192;
    SetAlctType_(288);
  } else if (chamber_type_string_ == "ME21") {
    NumberOfWireGroupsInChamber_ = 672;
    SetAlctType_(672);
  } else if (chamber_type_string_ == "ME22") {
    NumberOfWireGroupsInChamber_ = 384;
    SetAlctType_(384);
  } else if (chamber_type_string_ == "ME31") {
    NumberOfWireGroupsInChamber_ = 576;
    SetAlctType_(672);
  } else if (chamber_type_string_ == "ME32") {
    NumberOfWireGroupsInChamber_ = 384;
    SetAlctType_(384);
  } else if (chamber_type_string_ == "ME41") {
    NumberOfWireGroupsInChamber_ = 576;
    SetAlctType_(672);
  } else if (chamber_type_string_ == "ME42") {
    NumberOfWireGroupsInChamber_ = 384;
    SetAlctType_(384);
  }
  //
  (*MyOutput_) << ". Number of Wire Groups in chamber = " << NumberOfWireGroupsInChamber_ 
	       << std::hex << std::endl;
  //
  return;
}
//
void ALCTnew::SetAlctType_(int type) {
  //
  RegSizeAlctFastFpga_RD_HOTCHAN_MASK_ = 0;
  RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_ = 0;
  RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_ = 0;
  RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_ = 0;
  RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_ = 0;
  RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_ = 0;
  //
  NumberOfChannelsInAlct_ = type;
  NumberOfGroupsOfDelayChips_ = 0;
  NumberOfAFEBs_ = 0;
  NumberOfChannelsPerLayer_ = 0;
  //
  if (NumberOfChannelsInAlct_ == 288) {
    //
    RegSizeAlctFastFpga_RD_HOTCHAN_MASK_ = 
      RegSizeAlctFastFpga_RD_HOTCHAN_MASK_288; 
    RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_ = 
      RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_288;
    RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_ = 
      RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_288; 
    RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_ =
      RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_288; 
    RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_ =
      RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_288; 
    RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_ =
      RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_288; 
    //
    NumberOfGroupsOfDelayChips_ = 3;
    //
  } else if (NumberOfChannelsInAlct_ == 384) {
    //
    RegSizeAlctFastFpga_RD_HOTCHAN_MASK_ = 
      RegSizeAlctFastFpga_RD_HOTCHAN_MASK_384; 
    RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_ = 
      RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_384;
    RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_ = 
      RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_384; 
    RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_ =
      RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_384; 
    RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_ =
      RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_384; 
    RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_ =
      RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_384; 
    //
    NumberOfGroupsOfDelayChips_ = 4;    
    //
  } else if (NumberOfChannelsInAlct_ == 672) {
    //
    RegSizeAlctFastFpga_RD_HOTCHAN_MASK_ = 
      RegSizeAlctFastFpga_RD_HOTCHAN_MASK_672; 
    RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_ = 
      RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_672;
    RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_ = 
      RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_672; 
    RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_ =
      RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_672; 
    RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_ =
      RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_672; 
    RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_ =
      RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_672; 
    //
    NumberOfGroupsOfDelayChips_ = 7;
  }
  //
  if (NumberOfGroupsOfDelayChips_ == 0) {
    //
    (*MyOutput_) << "ALCTnew: ERROR Invalid ALCT type " 
		 << std::dec << type << std::endl;
    NumberOfChannelsInAlct_ = 0;
    //
  } else {
    //
    NumberOfAFEBs_ = NumberOfWireGroupsInChamber_ / NUMBER_OF_LINES_PER_CHIP;
    NumberOfChannelsPerLayer_ = NumberOfChannelsInAlct_ / MAX_NUM_LAYERS;
    //
    (*MyOutput_) << "........................ ALCT type = " << std::dec << NumberOfChannelsInAlct_ << std::endl; 
    (*MyOutput_) << ".. Number of groups of delay chips =  " << NumberOfGroupsOfDelayChips_ << std::endl; 
    (*MyOutput_) << ".................. Number of AFEBs = " << NumberOfAFEBs_ << std::endl;
  }
  //
  return;
}
//
