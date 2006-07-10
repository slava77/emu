#ifndef ALCTnew_h
#define ALCTnew_h

#include <string>
#include "EMUjtag.h"

class TMB;
class ALCTtype;

class ALCTnew : public EMUjtag {
public:
  //
  ALCTnew(TMB * tmb, std::string chamberType); //chamberType = ME11, ME12, ME13, ME21, ME22, ME31, ME32, ME41, ME42
  ~ALCTnew();
  //
  inline void RedirectOutput(std::ostream * Output) { MyOutput_ = Output ; }
  //
  void setup();
  //
  inline int GetNumberOfAfebs() { return NumberOfAFEBs_; }
  inline int GetNumberOfGroupsOfDelayChips() { return NumberOfGroupsOfDelayChips_; }
  inline int GetNumberOfWireGroupsInChamber() { return NumberOfWireGroupsInChamber_; }
  inline int GetNumberOfChannelsPerLayer() { return NumberOfChannelsPerLayer_; }
  inline int GetNumberOfChannelsInAlct() { return NumberOfChannelsInAlct_; }
  //
  ///////////////////////
  //SLOW CONTROL ID
  ///////////////////////
  int  GetSlowControlChipId();    
  int  GetSlowControlVersionId(); 
  int  GetSlowControlYear();      
  int  GetSlowControlDay();       
  int  GetSlowControlMonth();     
  void PrintSlowControlId();                     //print out software values
  //
  //
  void ReadSlowControlId();                      //fills software values with values read from ALCT
  //
  ///////////////////////
  //TESTPULSE POWERSWITCH
  ///////////////////////
  void SetTestpulsePowerSwitchReg(int powerswitch);      // powerswitch = OFF or ON
  int  GetTestpulsePowerSwitchReg();
  void SetPowerUpTestpulsePowerSwitchReg();              //sets software values to power-up values            
  void PrintTestpulsePowerSwitchReg();                   //print out software values				             
  //
  //
  void WriteTestpulsePowerSwitchReg();             //writes software values to testpulse power switch register
  void ReadTestpulsePowerSwitchReg();              //fills software values with values read from ALCT	      
  //
  /////////////////////////////////////////////////////////////////////////////////////////
  //TESTPULSE AMPLITUDE - 8-bit DAC controls amplitude of analog test pulse sent to AFEBs
  /////////////////////////////////////////////////////////////////////////////////////////
  void SetTestpulseAmplitude(int dacvalue);        // Voltage = 2.5V * dacvalue/256
  int  GetTestpulseAmplitude();
  void SetPowerUpTestpulseAmplitude();             // sets software values to power-up values
  //
  //
  void WriteTestpulseAmplitude();               // writes software values to testpulse amplitude register
  //
  ////////////////////////////////////////////////////////////////////////////////
  //TESTPULSE GROUPMASK - which group of AFEB's are enabled for analog testpulse
  ////////////////////////////////////////////////////////////////////////////////
  void SetTestpulseGroupMask(int group,      // group [0-6], where 0 = AFEB 00,01,02,12,13,14 
			     //                                    1 = AFEB 03,04,05,15,16,17 
			     //                                    2 = AFEB 06,07,08,18,19,20 
			     //                                    3 = AFEB 09,10,11,21,22,23 
			     //                                    4 =                        
			     //                                    5 =                        
			     //                                    6 = 
			     int mask);      // mask = OFF or ON
  int  GetTestpulseGroupMask(int group);     // group = [0-6]
  void SetPowerUpTestpulseGroupMask();	     // sets software values to power-up values                  
  void PrintTestpulseGroupMask();	     // print out software values				      
  //
  //
  void WriteTestpulseGroupMask();             // writes software values to testpulse group mask register
  void ReadTestpulseGroupMask();	      // fills software values with values read from ALCT	      
  //
  //////////////////////////////////////////////////////////////
  //TESTPULSE STRIPMASK - which AFEB within the group is firing
  //////////////////////////////////////////////////////////////
  void SetTestpulseStripMask(int AFEBinGroup,     // AFEBinGroup = [0-5]
			     int mask);           // mask = OFF or ON
  int  GetTestpulseStripmask(int AFEBinGroup);    // AFEBinGroup = [0-5]
  void SetPowerUpTestpulseStripMask();	   	  // sets software values to power-up values                  
  void PrintTestpulseStripMask();		  // print out software values				      
  // 
  //
  void WriteTestpulseStripMask();                 // writes software values to testpulse strip mask register
  void ReadTestpulseStripMask();		  // fills software values with values read from ALCT	      
  //
  //////////////////
  //AFEB THRESHOLDS
  //////////////////
  // N.B. DAC is 8 bits, while ADC is 10 bits => write variable is different than read variable
  void  SetAfebThreshold(int AFEB,                     // AFEB = [0 - GetNumberOfAfebs()-1]
	 		 int dacValue);                // Voltage set = 2.5V * dacValue/256 (8-bit dac)
  int   GetAfebThresholdDAC(int AFEB);                 // AFEB = [0 - GetNumberOfAfebs()-1]
  int   GetAfebThresholdADC(int AFEB);                 // AFEB = [0 - GetNumberOfAfebs()-1] (10-bit adc value)
  float GetAfebThresholdVolts(int AFEB);               // return voltage = 2.5V * adcValue/1023 
  void  SetPowerUpAfebThresholds();		       //sets software values to power-up values                
  void  PrintAfebThresholds();			       //print out software values				 
  //
  //
  void  WriteAfebThresholds();                      //writes software values to threshold DACs
  void  ReadAfebThresholds();			    //fills software values with values read from ALCT ADC	 
  //
  ////////////////////////
  //STANDBY REGISTER
  ////////////////////////
  void SetStandbyRegister(int AFEB,                   // AFEB = [0 - GetNumberOfAfebs()-1]
			  int powerswitch);           // powerswitch = OFF or ON
  int  GetStandbyRegister(int AFEB);                  // AFEB = [0 - GetNumberOfAfebs()-1]
  void SetPowerUpStandbyRegister();		      //sets software values to power-up values          
  void PrintStandbyRegister();			      //print out software values				       
  //
  //
  void WriteStandbyRegister();                     //writes software values to AFEB standby register
  void ReadStandbyRegister();			   //fills software values with values read from ALCT	 
  //
  ///////////////////////
  //FAST CONTROL ID
  ///////////////////////
  int  GetFastControlChipId();
  int  GetFastControlVersionId();
  int  GetFastControlYear();     
  int  GetFastControlDay();      
  int  GetFastControlMonth();    
  void PrintFastControlId();               //print out software values				 
  //
  //
  void ReadFastControlId();   		   //fills software values with values read from ALCT	 
  //
  //////////////////////////////
  // DELAY LINE CONTROL REGISTER
  //////////////////////////////
  // The delay line control register is controlled within writing and Reading AfebDelays() and AfebPatterns(), below.
  // Here are methods to see the hardware and software configuration of the control register:
  void ReadDelayLineControlReg();                //fills software values with values read from ALCT
  void PrintDelayLineControlReg();               //print out software values
  //
  //////////////////////////////
  // ASIC DELAYS and PATTERNS
  //////////////////////////////
  void SetAsicDelay(int AFEB,                        // AFEB= [0 - GetNumberOfAfebs()-1]
		    int delay);                      // delay=[0-15] (~2ns steps)
  int  GetAsicDelay(int AFEB);                       // AFEB= [0 - GetNumberOfAfebs()-1]
  void SetPowerUpAsicDelays();  		     // sets software values to power-up values                
  void PrintAsicDelays();                            // print out software values
  //
  void SetAsicPattern(int layer,                     // layer=[0-5]
		      int channel,                   // channel=[0 - GetNumberOfChannelsPerLayer()-1] 
		      int mask);                     // mask= OFF or ON
  int  GetAsicPattern(int layer,                     // layer=[0-5]
		      int channel);                  // channel=[0 - GetNumberOfChannelsPerLayer()-1] 
  void SetPowerUpAsicPatterns();  		     // sets software values to power-up values                
  void PrintAsicPatterns();                          // print out software values
  //
  //
  void WriteAsicDelaysAndPatterns();                 //writes software values of delays to AFEB delay lines
  void ReadAsicDelaysAndPatterns();                  //fills software values with values read from ALCT
  //
  //////////////////////////////
  // CONFIGURATION REGISTER
  //////////////////////////////
  void SetTriggerMode(int trigger_mode); 
  int  GetTriggerMode(); 
  //
  void SetExtTrigEnable(int ext_trig_enable); 
  int  GetExtTrigEnable();
  //
  void SetSendEmpty(int send_empty);
  int  GetSendEmpty(); 
  //
  void SetInject(int inject); 
  int  GetInject(); 
  //
  void SetBxcOffset(int bxc_offset); 
  int  GetBxcOffset(); 
  //
  void SetNphThresh(int nph_thresh); 
  int  GetNphThresh(); 
  //
  void SetNphPattern(int nph_pattern); 
  int  GetNphPattern(); 
  //
  void SetDriftDelay(int drift_delay); 
  int  GetDriftDelay(); 
  //
  void SetFifoTbins(int fifo_tbins); 
  int  GetFifoTbins(); 
  //
  void SetFifoPretrig(int fifo_pretrig); 
  int  GetFifoPretrig(); 
  //
  void SetFifoMode(int fifo_mode); 
  int  GetFifoMode(); 
  //
  void SetFifoLastLct(int fifo_lastlct); 
  int  GetFifoLastLct(); 
  //
  void SetL1aDelay(int l1a_delay); 
  int  GetL1aDelay(); 
  //
  void SetL1aWindow(int l1a_window); 
  int  GetL1aWindow(); 
  //
  void SetL1aOffset(int l1a_offset); 
  int  GetL1aOffset(); 
  //
  void SetL1aInternal(int l1a_internal); 
  int  GetL1aInternal(); 
  //
  void SetBoardId(int board_id); 
  int  GetBoardId(); 
  //
  void SetBxnOffset(int bxn_offset); 
  int  GetBxnOffset(); 
  //
  void SetCcbEnable(int ccb_enable); 
  int  GetCcbEnable(); 
  //
  void SetAlctJtagDs(int alct_jtag_ds); 
  int  GetAlctJtagDs(); 
  //
  void SetAlctTmode(int alct_tmode); 
  int  GetAlctTmode(); 
  //
  void SetAlctAmode(int alct_amode); 
  int  GetAlctAmode(); 
  //
  void SetAlctMaskAll(int alct_mask_all); 
  int  GetAlctMaskAll(); 
  //
  void SetTriggerInfoEn(int trigger_info_en); 
  int  GetTriggerInfoEn(); 
  //
  void SetSnSelect(int sn_select); 
  int  GetSnSelect(); 
  //
  void SetPowerUpConfigurationReg();          //sets software values to default values
  void PrintConfigurationReg();               //print out software values
  //
  //
  void WriteConfigurationReg();                  //writes software values to configuration register
  void ReadConfigurationReg();                   //fills software values with values read from ALCT
  //
  //////////////////////////////
  // HOT CHANNEL MASK
  //////////////////////////////
  void  SetHotChannelMask(int layer,            // layer = 1-6
  	 		  int channel,          // [1-48] for ALCT288, [1-64] for ALCT384, [1-112] for ALCT672
  			  int mask);            // mask = OFF or ON
  int  GetHotChannelMask(int layer,             // layer = 1-6
  			 int channel);          // [1-48] for ALCT288, [1-64] for ALCT384, [1-112] for ALCT672
  void SetPowerUpHotChannelMask();              // sets software values to default values
  void PrintHotChannelMask();                   // prints out software values
  //
  //
  void WriteHotChannelMask();               //writes software values to hot channel mask register
  void ReadHotChannelMask();                //fills software values with values read from ALCT
  //
protected:
  //
  //
private:
  //
  std::ostream * MyOutput_ ;
  TMB * tmb_ ;
  //
  // Things specific to the chamber-type: //
  void SetChamberCharacteristics_(std::string chamberType);
  std::string chamber_type_string_;  
  int NumberOfWireGroupsInChamber_;
  //
  // Things specific to the ALCT-type: //
  void SetAlctType_(int type);
  int NumberOfChannelsInAlct_;
  int NumberOfGroupsOfDelayChips_;
  int NumberOfChipsPerGroup_;
  int NumberOfLinesPerChip_;
  int NumberOfAFEBs_;
  int NumberOfChannelsPerLayer_;
  int RegSizeAlctFastFpga_RD_HOTCHAN_MASK_;
  int RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_;
  int RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_;
  int RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_;
  int RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_;
  int RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_;
  //
  //////////////////////////////
  // Slow-control variables:  //
  //////////////////////////////
  char slowcontrol_id_[RegSizeAlctSlowFpga_RD_ID_REG/8+1];
  //
  int testpulse_power_setting_;
  //
  int testpulse_amplitude_dacvalue_;
  //
  int testpulse_groupmask_[RegSizeAlctSlowFpga_WRT_TESTPULSE_GRP];
  //
  int testpulse_stripmask_[RegSizeAlctSlowFpga_WRT_TESTPULSE_STRIP];
  //
  int read_adc_(int ADCchipNumber, int ADCchannel);
  int afeb_threshold_write_[MAX_NUM_AFEBS];
  int afeb_threshold_read_[MAX_NUM_AFEBS];
  //
  int standby_register_[MAX_NUM_AFEBS];
  //
  //////////////////////////////
  // Fast-control variables:  //
  //////////////////////////////
  char fastcontrol_id_[RegSizeAlctFastFpga_RD_ID_REG/8+1];
  //
  int delay_line_control_reg_[RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_672]; //make this as large as it could possibly be
  void SetDelayLineGroupSelect_(int group,           // 1 group = 6 AFEBs => [0-2] ALCT288, [0-4] ALCT384, [0-6] ALCT672
				int mask);           // mask = ON or OFF
  void SetDelayLineSettst_(int mask);                // mask = ON or OFF
  void SetDelayLineReset_(int mask);                 // mask = ON or OFF
  void SetPowerUpDelayLineControlReg_();             //sets software values to default values  
  void WriteDelayLineControlReg_();                  //writes software values to configuration register
  void DecodeDelayLineControlReg_();
  void FillDelayLineControlReg_();
  int delay_line_reset_;
  int delay_line_settst_;
  int delay_line_group_select_[RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_672-2]; //make this as large as it could possibly be
  //
  void FillAsicDelaysAndPatterns_(int groupOfAfebs);     //each delay line has 6 AFEBs
  void DecodeAsicDelaysAndPatterns_(int groupOfAfebs);     //each delay line has 6 AFEBs
  int GetLayerFromAsicMap_(int asic_index);
  int GetChannelFromAsicMap_(int groupOfAfebs, int asic_index);
  int asic_delay_[MAX_NUM_AFEBS];
  int asic_pattern_[MAX_NUM_LAYERS][MAX_NUM_WIRES_PER_LAYER];
  int asic_delays_and_patterns_[RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES]; //use WRT, since first bit of RD is junk bit
  //
  int config_reg_[RegSizeAlctFastFpga_RD_CONFIG_REG];
  void DecodeConfigurationReg_();
  void FillConfigurationReg_();
  int trigger_mode_;
  int ext_trig_enable_;
  int send_empty_;
  int inject_;
  int bxc_offset_;
  int nph_thresh_;
  int nph_pattern_;
  int drift_delay_;
  int fifo_tbins_;
  int fifo_pretrig_;
  int fifo_mode_;
  int fifo_lastlct_;
  int l1a_delay_;
  int l1a_window_;
  int l1a_offset_;
  int l1a_internal_;
  int board_id_;
  int bxn_offset_;
  int ccb_enable_;
  int alct_jtag_ds_;
  int alct_tmode_;
  int alct_amode_;
  int alct_mask_all_;
  int trigger_info_en_;
  int sn_select_;
  //
  int hot_channel_mask_[RegSizeAlctFastFpga_RD_HOTCHAN_MASK_672];             //make this as large as it could possibly be
  //
};

#endif

