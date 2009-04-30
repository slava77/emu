//-----------------------------------------------------------------------
// $Id: ALCTController.h,v 1.3 2009/04/30 14:23:15 liu Exp $
// $Log: ALCTController.h,v $
// Revision 1.3  2009/04/30 14:23:15  liu
// fix Get_InvertPulse()
//
// Revision 1.2  2009/03/25 10:19:41  liu
// move header files to include/emu/pc
//
// Revision 1.1  2009/03/25 10:07:42  liu
// move header files to include/emu/pc
//
// Revision 3.37  2008/11/18 17:03:17  rakness
// include ALCT PROM readback
//
// Revision 3.36  2008/08/13 11:30:53  geurts
// introduce emu::pc:: namespaces
// remove any occurences of "using namespace" and make std:: references explicit
//
// Revision 3.35  2008/08/08 15:29:18  liu
// ix Get/Set functions for database
//
// Revision 3.34  2008/08/08 11:01:23  rakness
// centralize logging
//
// Revision 3.33  2008/08/06 17:24:50  rakness
// add known_problem parameter to xml file; add time stamp + number of reads to config check output file
//
// Revision 3.32  2008/08/05 08:40:36  rakness
// add minimum number of times to read when checking configuration
//
// Revision 3.31  2008/07/04 14:24:29  rakness
// add getters for string values
//
// Revision 3.30  2008/06/23 14:26:09  rakness
// add getter functions
//
// Revision 3.29  2008/06/12 21:08:54  rakness
// add firmware tags for DMB, CFEB, MPC, CCB into xml file; add check firmware button
//
// Revision 3.28  2008/05/26 08:24:41  rakness
// for AFEB calibrations:  argument for TMB and ALCT::configure(2) to not write userPROMs; correctly respond to configuration written to broadcast slot
//
// Revision 3.27  2008/04/19 14:56:55  rakness
// ALCT database check before loading ALCT firmware
//
// Revision 3.26  2008/04/09 15:37:23  rakness
// read ALCT fast control FPGA ID
//
// Revision 3.25  2008/01/14 18:17:26  rakness
// correct read of +5.5V_B ADC values on ALCT288
//
// Revision 3.24  2008/01/09 09:45:07  rakness
// modify AFEB mapping and accessors so that user counts from 0 to MaximumUserIndex(), no matter how the AFEBs are physically connected to the ALCT
//
// Revision 3.23  2007/10/09 11:10:35  rakness
// remove RAT and ALCT inheritance from EMUjtag, i.e., make calls to EMUjtag methods explicitly through TMB
//
// Revision 3.22  2007/07/13 11:57:46  rakness
// add read/accessors to ALCT temperature+on-board voltage
//
// Revision 3.21  2007/04/26 07:43:26  rakness
// AFEB delay chip index count from 0 for unconnected AFEBs in ME1/3,3/1,4/1
//
// Revision 3.20  2007/04/19 16:09:18  rakness
// add accel pretrig/pattern to ALCT config reg
//
// Revision 3.19  2007/03/28 17:46:21  rakness
// xml changes:  add ALCT testpulse, remove TTCrxID
//
// Revision 3.18  2007/03/14 08:59:03  rakness
// make parser dumb
//
// Revision 3.17  2007/01/31 16:49:41  rakness
// complete set of TMB/ALCT/RAT xml parameters
//
// Revision 3.16  2006/11/27 12:59:42  rakness
// move some registers to public
//
// Revision 3.15  2006/11/13 16:25:31  mey
// Update
//
// Revision 3.14  2006/11/10 12:43:06  rakness
// include TMB/ALCT configuration and state machine prints+checks to hyperDAQ
//
// Revision 3.13  2006/10/19 09:42:03  rakness
// remove old ALCTController
//
// Revision 3.12  2006/10/12 15:56:00  rakness
// cleaned up configuration checking for ALCT/TMB
//
// Revision 3.11  2006/10/10 15:34:58  rakness
// check TMB/ALCT configuration vs xml
//
// Revision 3.10  2006/10/06 12:15:38  rakness
// expand xml file
//
// Revision 3.9  2006/10/03 07:36:01  mey
// UPdate
//
// Revision 3.8  2006/09/24 15:32:54  rakness
// read new alct fast control register
//
// Revision 3.7  2006/09/15 07:50:40  rakness
// dump config registers
//
// Revision 3.6  2006/09/05 10:13:16  rakness
// ALCT configure from prom
//
// Revision 3.5  2006/08/15 14:16:49  rakness
// add collision mask reg/clean up configure output
//
// Revision 3.4  2006/08/14 13:45:11  rakness
// upgrade ALCTnew to accomodate ALCT 192/576
//
// Revision 3.3  2006/08/08 16:38:27  rakness
// ALCTnew: remove parameters not supported in firmware
//
// Revision 3.2  2006/07/23 15:42:50  rakness
// index Hot Channel Mask from 0
//
// Revision 3.1  2006/07/22 16:12:36  rakness
// clean up RAT/add JTAG checking
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.25  2006/07/20 15:35:17  rakness
// update documentation
//
// Revision 2.24  2006/07/20 11:07:59  rakness
// make many functions private
//
// Revision 2.23  2006/07/19 18:11:17  rakness
// Error checking on ALCTNEW
//
// Revision 2.22  2006/07/18 18:00:15  rakness
// clean up documentation
//
// Revision 2.21  2006/07/18 12:24:30  rakness
// update
//
// Revision 2.20  2006/07/18 12:21:54  rakness
// ALCT threshold scan with ALCTNEW
//
// Revision 2.19  2006/07/14 11:45:49  rakness
// compiler switch possible for ALCTNEW
//
// Revision 2.18  2006/07/12 15:06:49  rakness
// option for cleaned up ALCT
//
// Revision 2.17  2006/07/12 12:07:11  mey
// ALCT connectivity
//
// Revision 2.16  2006/06/23 13:40:25  mey
// Fixed bug
//
// Revision 2.15  2006/05/31 11:04:34  mey
// UPdate
//
// Revision 2.14  2006/05/18 08:35:43  mey
// Update
//
// Revision 2.13  2006/04/27 18:46:03  mey
// UPdate
//
// Revision 2.12  2006/04/06 08:54:32  mey
// Got rif of friend TMBParser
//
// Revision 2.11  2006/03/22 14:36:36  mey
// Update
//
// Revision 2.10  2006/03/15 16:42:57  mey
// Update
//
// Revision 2.9  2006/03/10 15:55:27  mey
// Update
//
// Revision 2.8  2006/02/25 11:24:34  mey
// Changed to constant
//
// Revision 2.7  2006/02/07 22:49:25  mey
// UPdate
//
// Revision 2.6  2005/12/10 11:24:05  mey
// Update
//
// Revision 2.5  2005/11/02 10:58:28  mey
// Update bxc_offset
//
// Revision 2.4  2005/08/31 15:12:56  mey
// Bug fixes, updates and new routine for timing in DMB
//
// Revision 2.3  2005/08/15 15:37:38  mey
// Include alct_hotchannel_file
//
// Revision 2.2  2005/08/11 08:13:59  mey
// Update
//
// Revision 2.1  2005/06/06 15:17:17  geurts
// TMB/ALCT timing updates (Martin vd Mey)
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef ALCTController_h
#define ALCTController_h
//
#include <string>
#include "emu/pc/EMU_JTAG_constants.h"
#include "emu/pc/EmuLogger.h"
//

namespace emu {
  namespace pc {


class TMB;
//
class ALCTController : public EmuLogger 
{
 public:
  //
  //!chamberType = ME11, ME12, ME13, ME21, ME22, ME31, ME32, ME41, ME42
  ALCTController(TMB * tmb, std::string chamberType); 
  ~ALCTController();
  //
  ///////////////////////////////////////////////////////////////////////////
  //  Useful methods to use ALCTController:
  ///////////////////////////////////////////////////////////////////////////
  void configure();                                  //writes software values to all registers on ALCT
  //!c = 2 = do not write configuration to userPROM
  void configure(int c);
  //
  void ReadALCTConfiguration();                      //Read current configuration and fill software read values
  void PrintALCTConfiguration();                     //Print software configuration values
  void CheckALCTConfiguration(int max_number_of_reads); //Check the read values against the write values
  void CheckALCTConfiguration();                        // this method calls the above method with a fixed maximum number of reads
  inline int GetALCTConfigurationStatus() { return alct_configuration_status_; }
  inline int GetNumberOfConfigurationReads() { return number_of_configuration_reads_; }
  //
  bool CheckFirmwareDate();
  //
  void SetFillVmeWriteVecs(bool fill_vectors_or_not);//writes software values to user prom
  bool GetFillVmeWriteVecs();
  void ClearVmeWriteVecs();
  //
  friend std::ostream & operator<<(std::ostream & os, ALCTController & alct);
  //
  inline std::string GetChamberType() { return chamber_type_string_; }
  inline int GetNumberOfAfebs() { return NumberOfAFEBs_; }
  inline int GetNumberOfGroupsOfDelayChips() { return NumberOfGroupsOfDelayChips_; }
  inline int GetNumberOfWireGroupsInChamber() { return NumberOfWireGroupsInChamber_; }
  inline int GetNumberOfChannelsPerLayer() { return NumberOfChannelsPerLayer_; }
  inline int GetNumberOfChannelsInAlct() { return NumberOfChannelsInAlct_; }
  inline int GetNumberOfCollisionPatternGroups() { return NumberOfCollisionPatternGroups_; }
  //!largest AFEB index which the user can access
  inline int MaximumUserIndex() { return (GetHighestAfebIndex() - GetLowestAfebIndex()); }
  // min-max hardware delay chip indices
  inline int GetLowestAfebIndex() { return lowest_afeb_index_; }
  inline int GetHighestAfebIndex() { return highest_afeb_index_; }
  //
  void SetUpPulsing(int DAC_pulse_amplitude=255,     //DAC_pulse_amplitude = [0-255]
		    int which_set=PULSE_AFEBS,       //which_set = [PULSE_LAYERS, PULSE_AFEBS]
		    int strip_mask=0xff,             //mask of enabled layers/AFEBgroups (depending on previous argument) 
  //                                                      -> bit = 1 = ON
  //                                                             = 0 = OFF
		    int source=ADB_SYNC);            //source = [OFF, ADB_SYNC, ADB_ASYNC, LEMO, SELF]
  //
  void SetUpRandomALCT();
  //
  //
  ////////////////////////////
  // Public ALCT Registers...
  ////////////////////////////
  ///////////////////////
  //SLOW CONTROL ID
  ///////////////////////
  int  GetSlowControlChipId();                         // get Read values
  int  GetSlowControlVersionId();                      // get Read values
  int  GetSlowControlYear();                           // get Read values
  int  GetSlowControlDay();                            // get Read values
  int  GetSlowControlMonth();                          // get Read values
  //
  void PrintSlowControlId();                           // print out Read values
  //
  void ReadSlowControlId();                       //fills Read values with values read from ALCT
  //
  //
  ///////////////////////
  //FAST CONTROL ID
  ///////////////////////
  int  GetFastControlRegularMirrorType();              // get Read values for ALCT firmware type
  inline  int GetExpectedFastControlRegularMirrorType() { return expected_fastcontrol_regular_mirror_; } 
  inline void SetExpectedFastControlRegularMirrorType(int regular_mirror) { expected_fastcontrol_regular_mirror_ = regular_mirror; } 
  //
  int  GetFastControlBackwardForwardType();            // get Read values for ALCT firmware type (ME11)
  inline  int GetExpectedFastControlBackwardForwardType() { return expected_fastcontrol_backward_forward_; } 
  inline void SetExpectedFastControlBackwardForwardType(int backward_forward) { expected_fastcontrol_backward_forward_ = backward_forward; } 
  //
  // here is the xml/database/user-friendly interface to prior method...
  void Set_fastcontrol_backward_forward_type(std::string alct_firmware_backwardForward); //alct_firmware_backwardForward = [b,f]
  std::string Get_fastcontrol_backward_forward_type();
  //
  //
  int  GetFastControlNegativePositiveType();           // get Read values for ALCT firmware type (ME11)
  inline  int GetExpectedFastControlNegativePositiveType() { return expected_fastcontrol_negative_positive_; } 
  inline void SetExpectedFastControlNegativePositiveType(int negative_positive) { expected_fastcontrol_negative_positive_ = negative_positive; } 
  //
  // here is the xml/database/user-friendly interface to prior method:
  void Set_fastcontrol_negative_positive_type(std::string alct_firmware_negativePositive); //alct_firmware_negativePositive = [n,p]
  std::string Get_fastcontrol_negative_positive_type();
  //
  //
  int  GetFastControlAlctType();                       // get Read values for ALCT firmware type
  inline  int GetExpectedFastControlAlctType() { return expected_fastcontrol_alct_type_; } 
  inline void SetExpectedFastControlAlctType(int alct_type) { expected_fastcontrol_alct_type_ = alct_type; } 
  //
  int  GetFastControlYear();                           // get Read values
  inline int GetExpectedFastControlYear() { return expected_fastcontrol_firmware_year_; } 
  void SetExpectedFastControlYear(int firmware_year);
  //
  int  GetFastControlDay();                            // get Read values
  inline int GetExpectedFastControlDay() { return expected_fastcontrol_firmware_day_; } 
  void SetExpectedFastControlDay(int firmware_day);
  //
  int  GetFastControlMonth();                          // get Read values
  inline int GetExpectedFastControlMonth() { return expected_fastcontrol_firmware_month_; } 
  void SetExpectedFastControlMonth(int firmware_month); 
  //
  int GetFastControlFPGAIdCode() { return alct_fpga_idcode_; } // get Read values
  inline int GetExpectedFastControlFPGAIdCode() { return expected_alct_fpga_idcode_; } 
  void SetExpectedFastControlFPGAIdCode(int code) { expected_alct_fpga_idcode_ = code; } 
  //
  void PrintFastControlId();                   // print out Read values				 
  //
  //
  void ReadFastControlId();   	                  //fills Read values with values read from ALCT	
  void ReadFastControlMezzIDCodes();              //fills Read values with values read from ALCT	
  //
  inline int GetFastControlMezzFPGAID() { return alct_fpga_idcode_; }
  inline int GetFastControlMezzPROM0ID() { return alct_prom0_idcode_; }
  inline int GetFastControlMezzPROM1ID() { return alct_prom1_idcode_; }
  //
  //
  //////////////////
  //AFEB THRESHOLDS
  //////////////////
  // N.B. DAC is 8 bits, while ADC is 10 bits => write variable is different than read variable
  void  SetAfebThreshold(int AFEB,                     // set Write values -> AFEB = [0-(MaximumUserIndex()-1)]
	 		 int dacValue);                //                     Voltage set = 2.5V * dacValue/256 (8-bit dac)
  int   GetAfebThresholdDAC(int AFEB);                 // get Write values -> AFEB = [0-(MaximumUserIndex()-1)]
  int   GetAfebThresholdADC(int AFEB);                 // get Read values -> AFEB = [0-(MaximumUserIndex()-1)] (10-bit adc value)
  float GetAfebThresholdVolts(int AFEB);               // get Read values -> return voltage = 2.5V * adcValue/1023 
  //
  void  SetPowerUpAfebThresholds();		       // sets Write values to data-taking defaults                
  void  PrintAfebThresholds();			       // print out Read values				 
  //
  //
  void  WriteAfebThresholds();                    //writes Write values to ALCT DAC
  void  ReadAfebThresholds();			  //fills Read values with values read from ALCT ADC	 
  //
  void  ReadAlctTemperatureAndVoltages();         //Read the ALCT values from the ADC
  void  PrintAlctTemperature();                   //Print the ALCT temperature
  inline float GetAlctTemperatureCelcius() { return read_alct_temperature_celcius_; } 
  inline float GetAlct_1p8_Voltage()  { return read_alct_1p8_voltage_;  } 
  inline float GetAlct_3p3_Voltage()  { return read_alct_3p3_voltage_;  } 
  inline float GetAlct_5p5a_Voltage() { return read_alct_5p5a_voltage_; } 
  inline float GetAlct_1p8_Current()  { return read_alct_1p8_current_;  } 
  inline float GetAlct_3p3_Current()  { return read_alct_3p3_current_;  } 
  inline float GetAlct_5p5a_Current() { return read_alct_5p5a_current_; } 
  float GetAlct_5p5b_Voltage();
  float GetAlct_5p5b_Current();
  //
  //////////////////////////////
  // ASIC DELAYS and PATTERNS 
  //////////////////////////////
  void SetAsicDelay(int AFEB,                        // set Write values -> AFEB = [0-(MaximumUserIndex()-1)]
		    int delay);                      //                    delay = [0-15] (~2ns steps)
  int  GetAsicDelay(int AFEB);                       // get Read values -> AFEB = [0-(MaximumUserIndex()-1)]
  int  GetWriteAsicDelay(int AFEB);                  // get Write values -> AFEB = [0-(MaximumUserIndex()-1)]
  //
  void SetPowerUpAsicDelays();  		     // sets Write values to data-taking defaults
  void PrintAsicDelays();                            // print out Read values
  //
  void SetAsicPattern(int layer,                     // set Write values -> layer = [0-5]
		      int channel,                   //                   channel = [0-(GetNumberOfChannelsPerLayer()-1)] 
		      int mask);                     //                      mask = [OFF, ON]
  int  GetAsicPattern(int layer,                     // get Read values -> layer = [0-5]
		      int channel);                  //                  channel = [0-(GetNumberOfChannelsPerLayer()-1)] 
  //
  void SetPowerUpAsicPatterns();  		     // sets Write values to data-taking defaults                
  void PrintAsicPatterns();                          // print out Read values
  //
  //
  void WriteAsicDelaysAndPatterns();            //writes Write values to ALCT
  void ReadAsicDelaysAndPatterns();             //fills Read values with values read from ALCT
  //
  //////////////////////////////
  // CONFIGURATION REGISTER
  //////////////////////////////
  void SetTriggerMode(int trigger_mode); // set Write values...
  //                      trigger_mode = [0-3] -> 0 = pre-trigger on either collision muon or accelerator muon pattern
  //                                              1 = only pre-trigger on accelerator muon
  //                                              2 = only pre-trigger on collision muon
  //                                              3 = pre-trigger on collision muon, veto pre-trigger on accelerator muon
  int  GetTriggerMode();                 // get Read values
  inline int GetWriteTriggerMode(){ return write_trigger_mode_ ; }
  //
  //
  void SetExtTrigEnable(int ext_trig_enable); // set Write values...
  //                        ext_trig_enable = [0,1] -> 0 = off
  //                                                   1 = on (triggers if ext_trig input signal == 1)
  int  GetExtTrigEnable();                    // get Read values
  inline int GetWriteExtTrigEnable(){ return write_ext_trig_enable_ ;}
  //
  //
  void SetSendEmpty(int send_empty); // set Write values...
  //                    send_empty = [0-1] -> 0 = off -> do not send DAQ for empty events
  //                                          1 = on  -> do send DAQ for empty events
  int  GetSendEmpty();               // get Read values
  inline int GetWriteSendEmpty(){ return write_send_empty_;}
  //
  //
  void SetInjectMode(int inject); // set Write values...
  //                     inject = [0-1] -> 0 = input data enabled
  //                                       1 = input data enabled if ext_inject input == 1
  int  GetInjectMode();           // get Read values
  inline int GetWriteInjectMode(){ return write_inject_;}
  //
  //
  void SetBxcOffset(int bxc_offset); // set Write values...
  //                    bxc_offset = [0-255] -> value loaded into internal BX counter upon BC0
  int  GetBxcOffset();               // get Read values
  inline int  GetWriteBxcOffset(){ return write_bxc_offset_; }
  //
  //
  void SetPretrigNumberOfLayers(int nph_thresh); // set Write values...
  //                                nph_thresh = [0-6] -> number of layers needed to generate pretrigger (marks bunch crossing)
  int  GetPretrigNumberOfLayers();               // get Read values
  inline int  GetWritePretrigNumberOfLayers(){ return write_nph_thresh_; }
  //
  //
  void SetPretrigNumberOfPattern(int nph_pattern);  // set Write values...
  //                                 nph_pattern = [0-6] -> number of layers needed after drift delay
  //                                                        (starting from pretrigger) to generate ALCT trigger
  int  GetPretrigNumberOfPattern();                 // get Read values
  inline int GetWritePretrigNumberOfPattern(){ return write_nph_pattern_ ;}
  //
  //
  void SetAcceleratorPretrigThresh(int accelerator_pretrig_thresh); // set Write values...
  //                                   accelerator_pretrig_thresh = [0-6] -> number of layers needed to generate pretrigger 
  //                                                                         for accelerator tracks
  int  GetAcceleratorPretrigThresh();                               // get Read values
  inline int GetWriteAcceleratorPretrigThresh() { return write_accelerator_pretrig_thresh_; }
  //
  //
  void SetAcceleratorPatternThresh(int accelerator_pattern_thresh); // set Write values...
  //                                   accelerator_pattern_thresh = [0-6] -> number of layers needed to generate trigger 
  //                                                                         (after drift_delay) for accelerator tracks
  int  GetAcceleratorPatternThresh();                               // get Read values
  inline int GetWriteAcceleratorPatternThresh() { return write_accelerator_pattern_thresh_; }
  //
  //
  void SetDriftDelay(int drift_delay); // set Write values...
  //                     drift_delay = [0-3] -> number of bunch crossings between pretrigger and LCT
  int  GetDriftDelay();                // get Read values
  inline int GetWriteDriftDelay() { return write_drift_delay_; }
  //
  //
  void SetFifoTbins(int fifo_tbins); // set Write values...
  //                    fifo_tbins = [0-31] -> number of bunch crossings in FIFO DAQ readout  
  int  GetFifoTbins();               // get Read values
  inline int GetWriteFifoTbins(){ return write_fifo_tbins_ ;} 
  //
  //
  void SetFifoPretrig(int fifo_pretrig); // set Write values...
  //                      fifo_pretrig = [0-31] -> raw hits will be shown in DAQ readout from 
  //                                               (fifo_pretrig-10) bunch crossings before trigger event
  int  GetFifoPretrig();                 // get Read values
  inline int  GetWriteFifoPretrig(){ return write_fifo_pretrig_;}
  //
  //
  void SetFifoMode(int fifo_mode); // set Write values...
  //                   fifo_mode = [0-2] -> 0 = no raw hits dump
  //                                        1 = Full dump (all LCT chips)
  //                                        2 = local dump (LCT chips with hits)
  int  GetFifoMode();              // get Read values
  inline int  GetWriteFifoMode(){ return write_fifo_mode_;}
  //
  //
  void SetL1aDelay(int l1a_delay); // set Write values...
  //                   l1a_delay = [0-255] -> number of bunch crossings (after the LCT) of the leading edge of the l1a window 
  int  GetL1aDelay();              // get Read values
  inline int  GetWriteL1aDelay(){ return write_l1a_delay_; }
  //
  //
  void SetL1aWindowSize(int size); // set Write values...
  //                        size = [0-15] -> width of l1a window in bunch crossings 
  int  GetL1aWindowSize();         // get Read values
  inline int  GetWriteL1aWindowSize(){ return write_l1a_window_; }
  //
  //
  void SetL1aOffset(int l1a_offset); // set Write values...
  //                    l1a_offset = [0-15] -> l1a accept counter pre-load value
  int  GetL1aOffset();               // get Read values
  inline int  GetWriteL1aOffset(){ return write_l1a_offset_; }
  //
  //
  void SetL1aInternal(int l1a_internal); // set Write values...
  //                      l1a_internal = [0-1] -> 0 = l1a expected to come from CCB via TMB
  //                                              1 = l1a generated internally (within the l1a window)
  int  GetL1aInternal();                 // get Read values
  inline int  GetWriteL1aInternal(){ return write_l1a_internal_; }
  //
  void SetBoardId(int board_id); // set Write values...
  //                  board_id = [0-7] -> ALCT2001 circuit board ID (defunct)
  int  GetBoardId();             // get Read values
  inline int  GetWriteBoardId() { return write_board_id_; }
  //
  //
  void SetCcbEnable(int ccb_enable); // set Write values...
  //                    ccb_enable = [0,1] -> Selector of BX counter rollover value
  //                                          1 = 3564 (LHC)
  //                                          0 = 924 (test beam)
  int  GetCcbEnable();               // get Read values
  inline int GetWriteCcbEnable(){ return write_ccb_enable_;}
  //
  //
  void SetConfigInReadout(int config_in_readout); // set Write values...
  //                          config_in_readout = [0-1] -> report configuration settings in DAQ readout
  //                                                       0 = do not report
  //                                                       1 = report
  int  GetConfigInReadout();               // get Read values
  inline int GetWriteConfigInReadout() { return write_config_in_readout_; }
  //
  //
  void SetAlctAmode(int alct_amode); // set Write values...
  //                    alct_amode = [0-1] -> use in conjunction with trig_mode
  //                                          0 = prefer Collision muon mode
  //                                          1 = prefer Accelerator muon mode
  int  GetAlctAmode();               // get Read values
  inline int GetWriteAlctAmode(){ return write_alct_amode_; }
  //
  //
  void SetTriggerInfoEnable(int trigger_info_en); // set Write values...
  //                            trigger_info_en = [0-1] -> 0 = do not write trigger info to FIFO
  //                                                       1 = write trigger info to FIFO
  int  GetTriggerInfoEnable();                    // get Read values
  inline int GetWriteTriggerInfoEnable(){ return write_trigger_info_en_; }
  //
  //
  void SetSnSelect(int sn_select); // set Write values...
  //                   sn_select = [0-1] -> 0 = read ALCT serial number via JTAG (defunct)
  //                                        1 = read Mezzanine card serial number via JTAG (defunct)
  int  GetSnSelect();              // get Read values
  inline int GetWriteSnSelect() { return write_sn_select_; }
  //
  //
  void SetPowerUpConfigurationReg();                // sets Write values to data-taking defaults
  void PrintConfigurationReg();                     // print out Read values
  //
  //
  void WriteConfigurationReg();                     //writes Write values to ALCT
  void ReadConfigurationReg();                      //fills Read values with values read from ALCT
  //
  //////////////////////////////
  // HOT CHANNEL MASK
  //////////////////////////////
  void  SetHotChannelMask(int layer,                 // set Write values -> layer = [0-5]
  	 		  int channel,               //                   channel = [0 - (GetNumberOfChannelsPerLayer()-1)]
  			  int mask);                 //                      mask = [OFF, ON]
  int  GetHotChannelMask(int layer,                  // get Read values -> layer = [0-5]
  			 int channel);               //                  channel = [0- (GetNumberOfChannelsPerLayer()-1)]
  //
  void SetPowerUpHotChannelMask();                   // sets Write values to data-taking defaults
  void PrintHotChannelMask();                        // prints out Read values
  //
  //
  void WriteHotChannelMask();                   //writes Write values to ALCT
  void ReadHotChannelMask();                    //fills Read values with values read from ALCT
  //
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  // COLLISION PATTERN MASK - Collision mask is set for 8 wiregroups at a time, e.g. group 0 = wires[0-7]
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  void SetCollisionPatternMask(int group,                   // set Write values -> group = [0 - (GetNumberOfCollisionPatternGroups()-1)]
			       int bitInEnvelope,           //                     bitInEnvelope = [0 - (NUMBER_OF_BITS_IN_COLLISION_MASK-1)]
			       int on_or_off);              //                     on_or_off = [ON, OFF]
  int  GetCollisionPatternMask(int group,                   // get Read values -> group = [0 - (GetNumberOfCollisionPatternGroups()-1)]
			       int bitInEnvelope);          //                    bitInEnvelope = [0 - (NUMBER_OF_BITS_IN_COLLISION_MASK-1)]
  //
  void SetPowerUpCollisionPatternMask();               // sets Write values to data-taking defaults
  void PrintCollisionPatternMask();                    // prints out Read values
  //
  //
  void WriteCollisionPatternMask();                    //writes Write values to ALCT
  void ReadCollisionPatternMask();                     //fills Read values with values read from ALCT
  //
  /////////////////////////////////////////////////
  //STANDBY REGISTER - enable power for each AFEB
  /////////////////////////////////////////////////
  void SetStandbyRegister_(int AFEB,                   // set Write Values -> AFEB = [0 - MaximumUserIndex()-1]
			   int powerswitch);           //                     powerswitch = OFF or ON
  int  GetStandbyRegister_(int AFEB);                  // get Read Values -> AFEB = [0 - MaximumUserIndex()-1]
  //
  void SetPowerUpStandbyRegister_();		      // set Write values to data-taking defaults          
  void PrintStandbyRegister_();			      // print out Read values				       
  //
  //
  void WriteStandbyRegister_();               // writes Write values to ALCT
  void ReadStandbyRegister_();		      // fills Read values with values read from ALCT	 
  //
  //
  ///////////////////////////////////////////////////////////////////////
  //TESTPULSE AMPLITUDE - amplitude of analog test pulse sent to AFEBs
  ///////////////////////////////////////////////////////////////////////
  void SetTestpulseAmplitude(int dacvalue);        // set Write values -> Voltage = 2.5V * dacvalue/256
  inline int GetTestpulseAmplitude() { return write_testpulse_amplitude_dacvalue_; } // get Write value
  //
  ///////////////////////////////////////////////////////////////////////////////
  // TESTPULSE TRIGGER REGISTER - specify which signal will fire the testpulse
  //////////////////////////////////////////////////////////////////////////////
  void FillTriggerRegister_();
  void DecodeTriggerRegister_();
  //
  void SetPulseTriggerSource_(int source);      // set Write value -> source = [OFF, ADB_SYNC, ADB_ASYNC, LEMO, SELF]
  int  GetPulseTriggerSource_();                // get Read value -> return value -> 0  = OFF
  //                                                                                 3  = SELF
  //                                                                                 4  = ADB_SYNC
  //                                                                                 8  = ADB_ASYNC
  //                                                                                 12 = LEMO
  void SetInvertPulse_(int mask);               //set Write value -> mask = [ON, OFF] 
  int  GetInvertPulse_();                       //get Read value -> return value -> 0 = not inverted
  //                                                                                1 = inverted 
  // here is the xml/database/user-friendly version to above method:
  void Set_InvertPulse(std::string invert_pulse); //invert_pulse = [on,off]
  std::string Get_InvertPulse();
  //
  //
  void SetPowerUpTriggerRegister_();	       // sets Write values to data-taking defaults
  void PrintTriggerRegister_();                // print out Read values				 
  //
  //
  void WriteTriggerRegister_();                 //writes Write values to ALCT
  void ReadTriggerRegister_();                  //fills Read values with values read from ALCT
  //
  // meant to be called from SetUpPulsing, or written into from xml file via method below
  inline void SetPulseDirection(int afebs_or_layers) { pulse_direction_ = afebs_or_layers; } //afebs_or_layers = [PULSE_LAYERS,PULSE_AFEBS]
  inline int  GetPulseDirection() { return pulse_direction_; }                      
  //
  // here is the xml/database/user-friendly way to write into above method...
  void Set_PulseDirection(std::string afebs_or_strips); //afebs_or_strips = [afebs,strips]
  std::string Get_PulseDirection(); 
  //
  //
  inline TMB * GetTMB(){ return tmb_;}
  //
  // Methods used to program ALCT prom:
  int CheckFirmwareConfiguration();
  void ProgramALCTProms();
  int SVFLoad(int *, const char *, int);  //this method is to be deprecated...
  //
protected:
  //
  //
private:
  //
  TMB * tmb_ ;
  int debug_;
  //
  int alct_configuration_status_;
  int number_of_configuration_reads_;
  //
  //
  ////////////////////////////////////////////////////////////////////
  // Private variables specific to the chamber-type:                //
  ////////////////////////////////////////////////////////////////////
  void SetChamberCharacteristics_(std::string chamberType);
  std::string chamber_type_string_;  
  int NumberOfWireGroupsInChamber_;
  int NumberOfChannelsPerLayer_;
  //
  ///////////////////////////////////////////////////////////////////////////////////
  // transformation from "user-interface" to "hardware-interface" delay chip index
  //////////////////////////////////////////////////////////////////////////////////
  int UserIndexToHardwareIndex_(int index);
  //
  ////////////////////////////////////////////////////////////////////
  // Private variables specific to the ALCT-type:                   //
  ////////////////////////////////////////////////////////////////////
  void SetFastControlAlctType_(int type_of_fast_control_alct);
  int NumberOfChannelsInAlct_;
  int NumberOfGroupsOfDelayChips_;
  int NumberOfCollisionPatternGroups_; 
  int RegSizeAlctFastFpga_RD_HOTCHAN_MASK_;
  int RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_;
  int RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_;
  int RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_;
  int RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_;
  int RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_;
  //
  void SetSlowControlAlctType_(int type_of_slow_control_alct);
  int NumberOfAFEBs_;
  int lowest_afeb_index_;
  int highest_afeb_index_;
  //
  //////////////////////////////////////////////////////
  // Slow-control registers private variables:        //
  //////////////////////////////////////////////////////
  char read_slowcontrol_id_[RegSizeAlctSlowFpga_RD_ID_REG/8+1];
  int  alct_slow_prom_idcode_;
  //
  int write_afeb_threshold_[MAX_NUM_AFEBS];
  int read_afeb_threshold_[MAX_NUM_AFEBS];
  float read_alct_temperature_celcius_;
  float read_alct_1p8_voltage_;
  float read_alct_3p3_voltage_;
  float read_alct_5p5a_voltage_;
  float read_alct_5p5b_voltage_;
  float read_alct_1p8_current_;
  float read_alct_3p3_current_;
  float read_alct_5p5a_current_;
  float read_alct_5p5b_current_;
  //
  int read_adc_(int ADCchipNumber, int ADCchannel);
  float ConvertADCtoVoltage_(int adc_value);  
  float ConvertADCtoCurrent_(int adc_value);
  float ConvertADCtoTemperature_(int adc_value);
  //
  //////////////////////////
  //TESTPULSE POWERSWITCH
  //////////////////////////
  int write_testpulse_power_setting_;
  int read_testpulse_power_setting_;
  //
  void SetTestpulsePowerSwitchReg_(int powerswitch);      // set Write values -> powerswitch = OFF or ON
  int  GetTestpulsePowerSwitchReg_();                     // get Read values
  //
  void SetPowerUpTestpulsePowerSwitchReg_();              //sets Write values to data-taking defaults
  void PrintTestpulsePowerSwitchReg_();                   //print out Read values				             
  //
  //
  void WriteTestpulsePowerSwitchReg_();             //writes Write values to ALCT
  void ReadTestpulsePowerSwitchReg_();              //fills Read values with values read from ALCT	      
  //
  ///////////////////////////////////////////////////////////////////////
  //TESTPULSE AMPLITUDE - amplitude of analog test pulse sent to AFEBs
  ///////////////////////////////////////////////////////////////////////
  int write_testpulse_amplitude_dacvalue_;
  //
  void SetPowerUpTestpulseAmplitude_();             // sets Write values to data-taking defaults
  void PrintTestpulseAmplitude_();
  //
  //
  void WriteTestpulseAmplitude_();               // writes Write values to ALCT
  //
  ////////////////////////////////////////////////////////////////////////////////
  //TESTPULSE GROUPMASK - which groups of AFEB's are enabled for analog testpulse
  ////////////////////////////////////////////////////////////////////////////////
  int write_testpulse_groupmask_[RegSizeAlctSlowFpga_WRT_TESTPULSE_GRP];
  int read_testpulse_groupmask_[RegSizeAlctSlowFpga_RD_TESTPULSE_GRP];
  //
  void SetTestpulseGroupMask_(int group,      // set Write values...
			      //                 group [0-6], where 0 = AFEB 00,01,02,12,13,14 
			      //                                    1 = AFEB 03,04,05,15,16,17 
			      //                                    2 = AFEB 06,07,08,18,19,20 
			      //                                    3 = AFEB 09,10,11,21,22,23 
			      //                                    4 =                        
			      //                                    5 =                        
			      //                                    6 = 
			      int mask);      // mask = OFF or ON
  int  GetTestpulseGroupMask_(int group);     // get Read values -> group = [0-6]
  //
  void SetPowerUpTestpulseGroupMask_();	     // sets Write values to data-taking defaults                  
  void PrintTestpulseGroupMask_();	     // print out Read values				      
  //
  //
  void WriteTestpulseGroupMask_();         // writes Write values to ALCT
  void ReadTestpulseGroupMask_();          // fills Read values with values read from ALCT	      
  //
  //////////////////////////////////////////////////////////////////////
  //TESTPULSE STRIPMASK - which layers are enabled for analog testpulse
  //////////////////////////////////////////////////////////////////////
  int write_testpulse_stripmask_[RegSizeAlctSlowFpga_WRT_TESTPULSE_STRIP];
  int read_testpulse_stripmask_[RegSizeAlctSlowFpga_RD_TESTPULSE_STRIP];
  //
  void SetTestpulseStripMask_(int layer,           // set Write Values -> layer = [0-5]
			      int mask);           //                      mask = OFF or ON
  int  GetTestpulseStripMask_(int layer);          // get Read Values -> layer = [0-5]
  //
  void SetPowerUpTestpulseStripMask_();	   	  // set Write values to data-taking defaults
  void PrintTestpulseStripMask_();		  // print out Read values				      
  // 
  //
  void WriteTestpulseStripMask_();            // writes Write values to ALCT
  void ReadTestpulseStripMask_();             // fills Read values with values read from ALCT	      
  //
  int write_standby_register_[RegSizeAlctSlowFpga_WRT_STANDBY_REG];
  int read_standby_register_[RegSizeAlctSlowFpga_RD_STANDBY_REG];
  //
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // vectors of bits for the fast-control registers, variables in these registers, and methods to translate between the two... //
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  int read_fastcontrol_id_[RegSizeAlctFastFpga_RD_ID_REG];
  void DecodeFastControlId_();
  //
  int fastcontrol_regular_mirror_;
  int fastcontrol_backward_forward_;
  int fastcontrol_negative_positive_;
  int fastcontrol_alct_type_;
  int fastcontrol_firmware_year_;
  int fastcontrol_firmware_day_;
  int fastcontrol_firmware_month_;
  //
  int expected_fastcontrol_regular_mirror_;
  int expected_fastcontrol_backward_forward_;
  int expected_fastcontrol_negative_positive_;
  int expected_fastcontrol_alct_type_;
  int expected_fastcontrol_firmware_year_;
  int expected_fastcontrol_firmware_day_;
  int expected_fastcontrol_firmware_month_;
  //
  int alct_fpga_idcode_;
  int alct_prom0_idcode_;
  int alct_prom1_idcode_;
  //
  int expected_alct_fpga_idcode_;
  //
  //
  int write_asic_delays_and_patterns_[RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES]; 
  int write_asic_delay_[MAX_NUM_AFEBS];
  int write_asic_pattern_[MAX_NUM_LAYERS][MAX_NUM_WIRES_PER_LAYER];
  void FillAsicDelaysAndPatterns_(int groupOfAfebs);       //each delay line controls one group of 6 AFEBs
  //
  int read_asic_delays_and_patterns_[RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES];  // N.B. It is WRT since RD has one extra bit which is not useful
  int read_asic_delay_[MAX_NUM_AFEBS];
  int read_asic_pattern_[MAX_NUM_LAYERS][MAX_NUM_WIRES_PER_LAYER];
  void DecodeAsicDelaysAndPatterns_(int groupOfAfebs);     //each delay line controls one group of 6 AFEBs
  //
  void WriteAsicDelaysAndPatterns_(int groupOfAfebs);      //writes Write values for this groupOfAfebs to ALCT
  void ReadAsicDelaysAndPatterns_(int groupOfAfebs);       //fills Read values for this groupOfAfebs with values read from ALCT
  int GetLayerFromAsicMap_(int asic_index);
  int GetChannelFromAsicMap_(int groupOfAfebs, int asic_index);
  //
  int write_config_reg_[RegSizeAlctFastFpga_WRT_CONFIG_REG];
  int write_trigger_mode_;
  int write_ext_trig_enable_;
  int write_send_empty_;
  int write_inject_;
  int write_bxc_offset_;
  int write_nph_thresh_;
  int write_nph_pattern_;
  int write_accelerator_pretrig_thresh_;
  int write_accelerator_pattern_thresh_;
  int write_drift_delay_;
  int write_fifo_tbins_;
  int write_fifo_pretrig_;
  int write_fifo_mode_;
  int write_l1a_delay_;
  int write_l1a_window_;
  int write_l1a_offset_;
  int write_l1a_internal_;
  int write_board_id_;
  int write_config_in_readout_;
  int write_ccb_enable_;
  int write_alct_amode_;
  int write_trigger_info_en_;
  int write_sn_select_;
  void FillConfigurationReg_();
  //
  int read_config_reg_[RegSizeAlctFastFpga_RD_CONFIG_REG];
  int read_trigger_mode_;
  int read_ext_trig_enable_;
  int read_send_empty_;
  int read_inject_;
  int read_bxc_offset_;
  int read_nph_thresh_;
  int read_nph_pattern_;
  int read_accelerator_pretrig_thresh_;
  int read_accelerator_pattern_thresh_;
  int read_drift_delay_;
  int read_fifo_tbins_;
  int read_fifo_pretrig_;
  int read_fifo_mode_;
  int read_l1a_delay_;
  int read_l1a_window_;
  int read_l1a_offset_;
  int read_l1a_internal_;
  int read_board_id_;
  int read_ccb_enable_;
  int read_config_in_readout_;
  int read_alct_amode_;
  int read_trigger_info_en_;
  int read_sn_select_;
  void DecodeConfigurationReg_();
  //
  int write_hot_channel_mask_[RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_672];             //make this as large as it could possibly be
  int read_hot_channel_mask_[RegSizeAlctFastFpga_RD_HOTCHAN_MASK_672];               //make this as large as it could possibly be
  //
  int write_collision_pattern_mask_reg_[RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_672]; //make this as large as it could possibly be
  int read_collision_pattern_mask_reg_[RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_672];  //make this as large as it could possibly be
  //
  int write_trigger_reg_[RegSizeAlctFastFpga_WRT_TRIG_REG];
  int write_pulse_trigger_source_;
  int write_invert_pulse_;
  //
  int read_trigger_reg_[RegSizeAlctFastFpga_RD_TRIG_REG];
  int read_pulse_trigger_source_;
  int read_invert_pulse_;
  //
  int pulse_direction_;
  std::string given_invert_pulse_;
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  // DELAY LINE CONTROL REGISTER - Control which group of chips has its delays and patterns written/read
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  int write_delay_line_control_reg_[RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_672]; //make this as large as it could possibly be
  int write_delay_line_reset_;
  int write_delay_line_settst_;
  int write_delay_line_group_select_[RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_672-2]; //make this as large as it could possibly be
  void FillDelayLineControlReg_();
  //
  int read_delay_line_control_reg_[RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_672]; //make this as large as it could possibly be
  int read_delay_line_reset_;
  int read_delay_line_settst_;
  int read_delay_line_group_select_[RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_672-2]; //make this as large as it could possibly be
  void DecodeDelayLineControlReg_();
  //
  void SetDelayLineSettst_(int mask);                 // set Write value -> mask = ON or OFF
  //
  void SetDelayLineReset_(int mask);                  // set Write value -> mask = ON or OFF
  //
  void SetDelayLineGroupSelect_(int group,            //set Write value -> group = [0 - (GetNumberOfGroupsOfDelayChips()-1)]
				int mask);            //                    mask = ON or OFF
  //
  void SetPowerUpDelayLineControlReg_();              //sets Write values to data-taking defaults
  void PrintDelayLineControlReg_();                   //print out Read values
  //
  void WriteDelayLineControlReg_();                  //writes Write values to ALCT
  void ReadDelayLineControlReg_();                  //fills Read values with values read from ALCT
  //
};

  } // namespace emu::pc
  } // namespace emu

#endif
