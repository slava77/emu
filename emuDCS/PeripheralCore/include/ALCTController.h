//-----------------------------------------------------------------------
// $Id: ALCTController.h,v 3.3 2006/08/08 16:38:27 rakness Exp $
// $Log: ALCTController.h,v $
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
#ifndef ALCTNEW

#ifndef ALCTController_h
#define ALCTController_h

#include "ALCTClasses.h"
#include <string>
#include <bitset>
  
class TMB;
class elong;
class AnodeChannel;

class ALCTController {
public:
  
  //friend class TMBParser;
  
  enum CHAMBER { ME11, ME12, ME13, ME21, ME22, ME31, ME41 };
  
  ALCTController(TMB * tmb, std::string chamberType);
  
  enum ALCTSTATUS {
    EALCT_SUCCESS   = 0, // Successful completion
    EALCT_FILEOPEN  = 1, // Filename could not be opened
    EALCT_FILEDEFECT= 2, // Configuration file inconsistency
    EALCT_PORT      = 3, // JTAG Device problem
    EALCT_CHAMBER   = 4, // Bad chamber_type_ number
    EALCT_ARG       = 5, // Argument Out Of Range
    EALCT_TESTFAIL  = 6, // Test failure
    EALCT_FIFONOTMT = 7, // FIFO never empties
  };
    
  enum LINKTYPE {LPT, VME};
  typedef enum LINKTYPE ALCTLINK;
  
  void setup(int choice);
  
  #define MAX_LCT_CHIPS    7
  
  /* Note that day, month and year are BCD, ie this year is 0x2000, not 0x7d0 */
  struct lct_type {
    ALCTIDRegister              idreg;
    unsigned char            lct_jtag_disable;
    unsigned char            test_pat_mode;
    unsigned char            accel_mu_mode;
    unsigned char            mask_all;
    unsigned char            unassigned;
    short int                hot_chan_mask[6*4]; //myc++
    short int                test_pattern[6*4];  //myc++
  };
  
  struct alct_params_type {
    ALCTIDRegister               sc_idreg;
    short int                 test_pulse_amp;
    unsigned char             test_pulse_group_mask;
    unsigned char             test_pulse_strip_mask;
    int                       standby_mask[4];
    unsigned char             test_pulse_power_up;
  
    ALCTIDRegister               conc_idreg;
    unsigned char             pretrigger_and_halt;
    unsigned char             inject_test_pattern;
    unsigned char             inject_mode;
    unsigned char             inj_lct_chip_mask;
    unsigned char             drift_delay;
    unsigned char             fifo_mode;
    unsigned char             fifo_last_feb;
    unsigned char             board_id;
    unsigned char             bx_counter_offset;
    unsigned char             alct_jtag_disable;
    unsigned char             alct_test_pat_mode;
    unsigned char             alct_accel_mu_mode;
    unsigned char             alct_mask_all;
    unsigned char             config_spare;
    unsigned char             trig;
    unsigned char             csc_id;
    unsigned char             cs_dly ;
    unsigned char             settst_dly ;
    unsigned char             rs_dly ;
    int                       delay_lines[6] ;
    unsigned char             input_enable ;
    unsigned char             input_disable ;
    unsigned char             coll_mask[8] ;
    float             voltage_18 ;
    float             voltage_33 ;
    float             voltage_551 ;
    float             voltage_552 ;
    float                     current_18 ;
    float             current_33 ;
    float             current_551 ;
    float             current_552 ;
    float             temperatur ;
  
    lct_type                  lct[MAX_LCT_CHIPS];
  };
  
  unsigned char image[6][14];
  unsigned pattern[7][6];
  
  // LPT or VME
  // ==========================
  // Functions to handle ALCT Link type
  // = VB =
  ALCTLINK alct_get_link_type() {
    return alct_link_type;
  }
  ALCTLINK alct_set_link_type(ALCTLINK linktype) {
    alct_link_type = linktype;
    return alct_link_type;
  }
  // ==========================
  void set_empty(int);
  void set_l1a_internal(int);
  inline void SetDelay(unsigned int number,int delay) {
    if (number<(sizeof(delays_)/4)) delays_[number]=delay;
  }
  inline void SetThreshold(unsigned int number,int threshold) {
    if (number<(sizeof(delays_)/4)) thresholds_[number]=threshold;
  }
  //
  inline std::string ALCTController::GetChamberType(){
    return chamber_type_string_;
  }
  //
  /// Set ALCT JTAG Channel
  int alct_fast_set_jtag_channel(int jtagchannel);
  /// Low Level JTAG One Bit Send/Read  Function
  int jam_jtag_io(int tms, int tdi, int read_tdo, unsigned long * ret);
  /// Flush and Decode Received JTAG bits
  void jam_jtag_flush ();
  void GetConf(  unsigned cr[3], int );
  void SetConf(  unsigned cr[3], int );
  int  GetWGNumber();
  void SetUpRandomALCT();
  void SetUpPulsing(long int Amplitude = 0x3f, int StripAfeb = 0, long int StripMask = 0x3f);
  void packControlRegister(unsigned * cr) const;
  /// fills the relevant data members
  void unpackControlRegister(unsigned * cr);
  /// writes to the control register
  void alct_read_hcmask(unsigned long HCmask[22]);      
  void alct_write_hcmask(unsigned long HCmask[22]);      
  // ==========================
  // Set Standard JTAG States
  /// Set JTAG State Machine into Start-IR-Shift State
  int StartIRShift();
  /// Set JTAG State Machine into Start-DR-Shift State
  int StartDRShift();
  // Set JTAG State Machine into Exit-IR-Shift State
  int ExitIRShift();
  // Set JTAG State Machine into Exit-DR-Shift State
  int ExitDRShift();
  // ==========================      
  // Writes Instruction Register (IR) of the JTAG
  int WriteIR(unsigned IR);
  // shifts the data into DR of the JTAG. The IR must be already written with the proper instruction
  // and the JTAG TAP must be already in the ShiftDR state.
  int ShiftIntoDR(unsigned *DR, unsigned sz, unsigned sendtms);
  int ShiftFromDR(unsigned *DR, unsigned sz, unsigned sendtms);
  // Writes data register (DR) of the JTAG. The IR must be already written with the proper instruction.
  int WriteDR(unsigned *DR, unsigned sz);
  // Reads data register (DR) of the JTAG. The IR must be already written with the proper instruction
  int ReadDR(unsigned *DR, unsigned sz);
  // writes the data to any data register, using the instruction.
  // The size of the data register determined automatically, using instruction code.
  int WriteRegister (unsigned instruction, unsigned *data);
  // reads the data from any data register, using the instruction.
  // The size of the data register determined automatically, using instruction code.
  int ReadRegister (unsigned instruction, unsigned *data);
  // writes the data to any data register, using the instruction.
  // The size of the data register determined automatically, using instruction code.
  int elWriteRegister (unsigned instruction, elong *data);
  // reads the data from any data register, using the instruction.
  // The size of the data register determined automatically, using instruction code.
  int elReadRegister (unsigned instruction, elong *data);
  // flips the 32-bit word, so bit 0 is bit 31, bit 1 is bit 30, etc.
  unsigned Flip (unsigned d);
  // flips the byte, so bit 0 is bit 7, bit 1 is bit 6, etc.
  unsigned char FlipByte (unsigned char d);
  // writes 6 delay lines. delay is the array of 6 values by 4 bits, containing the desired delays,
  // pattern is the array of 6 values by 16 bits, containing the patterns.
  // cs is a 3,4,or 7-bit mask for chip select signals of the delay line groups. bit set to 1 enables the group
  int Write6DelayLines(unsigned *delay, unsigned *pattern, unsigned cs);
  // reads 6 delay lines. delay is the array of 6 values by 4 bits, containing the delays upon return ,
  // pattern is the array of 6 values by 16 bits, containing the patterns upon return.
  // cs is a 4-bit mask for chip select signals of the delay line groups. bit set to 1 enables the group
  int Read6DelayLines(unsigned *delay, unsigned *pattern, unsigned cs);
  // generates pattern in the specified WG and ORs it into the image (global variable).
  // If clear != 0, the image is cleared.
  void GeneratePattern(int WG, int clear);
  // generates acc pattern in the specified WG and ORs it into the image (global variable).
  void GenerateAccPattern(int WG);
  void DecodeOS(
		elong * OS,
		unsigned * empty,
		unsigned * hfa,
		unsigned * h,
		unsigned * hn,
		unsigned * hpatb,
		unsigned * lfa,
		unsigned * l,
		unsigned * ln,
		unsigned * lpatb,
		unsigned * daq,
		unsigned * bxn
		);
  
  ALCTSTATUS alct_fast_self_test
    (       
     long* code,
     unsigned long verbose
     );
  
  ALCTSTATUS alct_fire_test_pulse
    (
     char pulse
     );
  
  ALCTSTATUS alct_download_patterns
    (
     const char* filename, 
     unsigned long verbose
     );
  
  ALCTSTATUS alct_download_hot_mask 
    (
     const char* filename, 
     unsigned long verbose
  );
  /// deprecated.  Use the XML, and setConfig.
  
  ALCTSTATUS alct_download_fast_config
  (
          char* filename,
          alct_params_type* params,
          unsigned long verbose
  );
  

  ALCTSTATUS alct_read_fast_config
  (
          alct_params_type* params,
          unsigned long verbose
  );
  
  ALCTSTATUS alct_read_delay_destructively
  (
          long * delays,
          unsigned long verbose
  );
  
  int delayLines() const;
  int delayChains() const;
  ///  deprecated.  Use setDelay();
  ALCTSTATUS alct_download_delay
  (
          char* filename,
          long * delays,
          unsigned long verbose
  );

  /// applies the delays set in the delays_ array.
  ALCTSTATUS setDelays();
  
  // this routine sets the delays for one particular delay line.
  // you must first call at least once alct_download_delay routine before using this one, or else it will return error code 5
  // if delay_line_number == -1 it sets all delay lines to "delay".
  ALCTSTATUS alct_set_delay
  (
      long delay_line_number,
      long delay
  );
  
  ALCTSTATUS alct_change_fast_parameter
  (
          char* parname,
          unsigned long* parval,
          unsigned long verbose
  );
  ALCTSTATUS alct_fast_disable_read();
  
  ALCTSTATUS alct_fast_enable_read();
  
  ALCTSTATUS alct_fast_read_id(ALCTIDRegister & id);
  
  ALCTSTATUS alct_write_test_pulse_reg(unsigned val);
  
  ALCTSTATUS alct_read_serial_number
  (
          unsigned long * alct_sn,
          unsigned long * mc_sn
  );
  
  /*#ifdef ALCT_FAST_LIB_LOCAL*/
  
  // this function inputs the "image" array, which is the image of the hits in the chamber.
  // It should be two-dimentional array: unsigned char image[6][14]. First index represents layers, second
  // represents bytes composing one layer (see illustration below):
  // - - - - - - - - layer 0
  // - - - - - - - -
  // - - - - - - - -
  // - - - - - - - -
  // - - - - - - - -
  // - - - - - - - - layer 5
  // Each dash in this illustration is a byte in the "image" array, representing 8 hits.
  // The output is a two-dimentional "pattern" array: unsigned pattern[7][6].
  // pattern[0][0..5] must be used as an input for Write6DelayLines function to write the
  // LCT0 delay lines, pattern[1][0..5] for LCT1, etc.
  void PrepareDelayLinePatterns();
  
  /*#endif*/
  
  // end from alct_fast_lib
  
  
  // from alct_slow_lib
  
  enum PWR_SPLY
  {
  	V18,	// Power Supply 1.8V
  	V33,	// Power Supply 3.3V
  	V55_1,	// Power Supply 5.5V (1)
  	V55_2	// Power Supply 5.5V (2)
  };
  
  ALCTSTATUS alct_read_slow_config(int * jtag, alct_params_type* params);
  
  // Load configuration parameters from specified file
  ALCTSTATUS alct_download_slow_config
  (
          int* jtag, 
  	char* filename, 
  	alct_params_type* params,
  	unsigned long verbose
  );
  
  /// assumes values read in from XML
  ALCTSTATUS setThresholds();

  /// deprecated.  Reads from file
  ALCTSTATUS alct_download_thresh(int* jtag,
                                char* filename,
                                unsigned long verbose);

  
  // Read ID Code of Slow Control Chip
  // sc_id - ID Code structure
  ALCTSTATUS alct_read_slowcontrol_id
  (
  	ALCTIDRegister* sc_id    
  );
  
  ALCTSTATUS alct_slowcontrol_id_status();
  
  /// Set Threshold value of specified channel
  ALCTSTATUS alct_set_thresh(long channel, long thresh);
  
  /// Read Threshold value from specified channel
  ALCTSTATUS alct_read_thresh(long channel, long* thresh);
  
  // Read OnBoard Temperature
  ALCTSTATUS alct_read_temp
  (
          int* jtag,
  	float* temp
  );
  
  // Read Power Supply Voltages Values
  // channel - Power Supply Channel (1.8, 3.3, 5.5 Volts)
  ALCTSTATUS alct_read_volt
  (
          int* jtag,
  	PWR_SPLY channel,
  	float* volt
  );
  
  // Read Currents Values
  ALCTSTATUS alct_read_current
  (
          int* jtag,
  	PWR_SPLY channel,
  	float* current
  );
  
  // Read row ADC value for onboard Temperature
  ALCTSTATUS alct_read_temp_adc(long* temp);
  
  // Read row ADC value for Voltage of Power Supply
  ALCTSTATUS alct_read_volt_adc
  (
          int* jtag,
  	PWR_SPLY channel,
  	long* volt
  );
  
  // Read row ADC value for Current of Power Supply
  ALCTSTATUS alct_read_current_adc
  (
          int* jtag,
  	PWR_SPLY channel,
  	long* current
  );
  
  // Perform Slow Control Self Test
  // code - Test # when error occured
  // code = 0 - Successeful completion of self-test
  ALCTSTATUS alct_slow_self_test
  (
          int* jtag,
  	long* code
  );
  
  // Set Standby Register value for specified wiregroup
  ALCTSTATUS alct_set_standby
  (
          int* jtag,
  	int group,
  	long standby
  );
  
  // Read Standby Register value for specified wire group
  ALCTSTATUS alct_read_standby
  (
          int* jtag,
  	int group,
  	long* standby
  );
  
  // Set Test Pulse Power Amplitude
  ALCTSTATUS alct_set_test_pulse_amp
  (
          int* jtag,
  	long test_pulse
  );
  
  // Set Test Pulse Power Up/Down 
  ALCTSTATUS alct_set_test_pulse_powerup
  (
          int* jtag,
  	long test_pulse_power_up
  );
  
  // Read Test Pulse Power Up/Down status
  ALCTSTATUS alct_read_test_pulse_powerup
  (
          int* jtag,
  	long* test_pulse_power_up
  );
  
  // Set Test Pulse Group Mask
  ALCTSTATUS alct_set_test_pulse_groupmask
  (
          int* jtag,
  	long test_pulse_group_mask
  );
  
  // Read Test Pulse Group Mask
  ALCTSTATUS alct_read_test_pulse_groupmask
  (
          int* jtag,
  	long* test_pulse_group_mask
  );
  
  // Set Test Pulse Strip Mask
  ALCTSTATUS alct_set_test_pulse_stripmask
  (
          int* jtag,
  	long test_pulse_strip_mask
  );
  
  // Read Test Pulse Strip Mask
  ALCTSTATUS alct_read_test_pulse_stripmask
  (
          int* jtag,
  	long* test_pulse_strip_mask
  );
  
  ALCTSTATUS alct_init_slow_control
  (
          int* jtag, 
  	long thresh 
  );
  
  // end from alct_slow_lib
  
  ALCTSTATUS alct_fast_CheckShiftingPatternViaJTAG
  (
  	unsigned long verbose
  );
  
  ALCTSTATUS DumpFifo();

  void set_defaults(alct_params_type *p);
  void show_params(int access_mode, alct_params_type *p);
  
  void start(int idev);
  void alct_end();
  void send_last();
  
  int do_jtag(int chip_id, int opcode, int mode, const int *first, int length, 
	      int *val);
  int do_jtag2(int chip_id, int opcode, int mode, const int *first, int length, 
	       int *val);
  
  /* Declarations for jtag_io_byte.c by Lisa Gorn */
  
  void jtag_io_byte_(int *nframes, char *tms, char *tdi, char *tdo, 
                     int *step_mode);
  void init_jtag();
  
  void Parse(char *buf,int *Count,char **Word);
  int SVFLoad(int*, const char *, int);
  int NewSVFLoad(int*, char *, int);
  int ConvertCharToInteger(char [], int [], int &);
  int ConstructShift(char HeaderData[8192],int HeaderDataSize, char Data[8192], int DataSize, 
		     char TrailerData[8192], int TrailerDataSize, int[] );
  int ReadIDCODE();
  int nAfebs() { return nAFEBs_; }
  //
  void SetTrigMode(int mode){alct_trig_mode_ = mode;}
  void SetExtTrigEnable(int enable){alct_ext_trig_en_ = enable;}
  void SetTrigInfoEnable(int enable){alct_trig_info_en_ = enable;}
  void SetL1aInternal(int enable){alct_l1a_internal_ = enable;}
  void SetFifoTbins(int tbins){alct_fifo_tbins_ = tbins;}
  void SetFifoPretrig(int pretrig){alct_fifo_pretrig_ = pretrig;}
  void SetL1aDelay(int delay){alct_l1a_delay_ = delay;}
  void SetL1aOffset(int offset) {alct_l1a_offset_ = offset;}
  void SetL1aWindowSize(int size){alct_l1a_window_ = size;}
  void SetPretrigNumberOfLayers(int nph){alct_nph_thresh_ = nph;}
  void SetPretrigNumberOfPattern(int nph){alct_nph_pattern_ = nph;}
  void SetCCBEnable(int enable){alct_ccb_enable_ = enable;}
  void SetAlctInjectMode(int mode){alct_inject_mode_ = mode;}
  void SetSendEmpty(int enable){alct_send_empty_ = enable;}
  void SetDriftDelay(int delay){alct_drift_delay_ = delay;}
  void SetPatternFile(std::string file){alctPatternFile = file;}
  void SetHotChannelFile(std::string file){alctHotChannelFile = file;}
  //
  void setConfig();

  void set_l1a_delay(int delay);
  //
  protected:
  // can't have an anodeChannel, since it needs to download 6 delays at a time.
  //std::vector<AnodeChannel> anodeChannels_;
  bool delays_inited_;
    //fg unsigned int delays_[42];
    //fg int thresholds_[42];
  int alct_trig_mode_;
  int alct_inject_mode_;
  int alct_ext_trig_en_;
  int alct_trig_info_en_;
  /// num fifo time bins before pretrigger
  int alct_fifo_pretrig_;
  int alct_l1a_internal_;
  int alct_l1a_offset_;
  int alct_fifo_tbins_;
  int alct_l1a_delay_;
  int alct_l1a_window_;
  int alct_fifo_mode_;
  /// number of hit planes for pretrigger
  int alct_nph_thresh_;
  /// pattern number threshold for trigger
  int alct_nph_pattern_;
  int alct_ccb_enable_;
  int alct_send_empty_;
  int alct_bxc_offset_;
  int alct_drift_delay_;
  int alct_inject_;
  std::string chamber_type_string_;
  
    /// this holds all the other non-congurable parameters.  It probably should disappear someday
    alct_params_type params_;
  
  
  private:
  //
  unsigned int delays_[42];
  int thresholds_[42];
  //
  TMB * tmb_;
  LINKTYPE alct_link_type;
  CHAMBER chamber_type_;
  std::string alctPatternFile;
  std::string alctHotChannelFile;
  
  unsigned nAFEBs_;
  unsigned n_lct_chips_;
  // defines registry sizes: 288, 384, or 672.  Should be set
  // when chamber type is set.
  unsigned * alct_rsz;
  
  int fd; // slot
  unsigned ALCT_FAST_JTAG_CHANNEL;
  enum {ALCT_FAST_LPT_JTAG_CHANNEL=4, ALCT_FAST_VME_JTAG_CHANNEL=8};
  
  enum {READ, WRITE};
  
  
  // === Socket Stuff
  #define MAX_JTAG_BUF_SIZE 9024
  
  char  sndbuf[MAX_JTAG_BUF_SIZE];
  char  rcvbuf[MAX_JTAG_BUF_SIZE];
  
  // ==================================
  
  bool jtag_initialized_;
  unsigned long rbits[1000]; // max size of the jtag register in bits.
  int nbits;
  unsigned long *readDR; //@@ same as a function name

  /// there's one of these for each field in CRfld.  They correspond to the data fields
  /// in this class, and are used for packing into a control register word
  /// It would be better to redo this using bitfields someday
  char * crParams_[26];
  /// makes crParams_ point to the relevant data fields
  void setCRfld(alct_params_type* params);


};
  
#endif
  
#else
//////////////////////////////////////////////////////////////////////////////////////
//  From here to end is new ALCTController...
//////////////////////////////////////////////////////////////////////////////////////
#ifndef ALCTController_h
#define ALCTController_h

#include <string>
#include "EMUjtag.h"

class TMB;
//
class ALCTController : public EMUjtag {
public:
  //
  ALCTController(TMB * tmb, 
		 std::string chamberType); //chamberType = ME11, ME12, ME13, ME21, ME22, ME31, ME32, ME41, ME42
  ~ALCTController();
  //
  inline void RedirectOutput(std::ostream * Output) { MyOutput_ = Output ; }
  //
  ///////////////////////////////////////////////////////////////////////////
  //  Useful methods to use ALCTController:
  ///////////////////////////////////////////////////////////////////////////
  void configure();                                  //writes software values to all registers on ALCT
  //
  inline std::string GetChamberType() { return chamber_type_string_; }
  inline int GetNumberOfAfebs() { return NumberOfAFEBs_; }
  inline int GetNumberOfGroupsOfDelayChips() { return NumberOfGroupsOfDelayChips_; }
  inline int GetNumberOfWireGroupsInChamber() { return NumberOfWireGroupsInChamber_; }
  inline int GetNumberOfChannelsPerLayer() { return NumberOfChannelsPerLayer_; }
  inline int GetNumberOfChannelsInAlct() { return NumberOfChannelsInAlct_; }
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
  void PrintSlowControlId();                           // print out Read values
  //
  void ReadSlowControlId();                       //fills Read values with values read from ALCT
  //
  ///////////////////////
  //FAST CONTROL ID
  ///////////////////////
  int  GetFastControlChipId();                         // get Read values
  int  GetFastControlVersionId();                      // get Read values
  int  GetFastControlYear();                           // get Read values
  int  GetFastControlDay();                            // get Read values
  int  GetFastControlMonth();                          // get Read values
  void PrintFastControlId();                           // print out Read values				 
  //
  void ReadFastControlId();   	                  //fills Read values with values read from ALCT	 
  //
  //////////////////
  //AFEB THRESHOLDS
  //////////////////
  // N.B. DAC is 8 bits, while ADC is 10 bits => write variable is different than read variable
  void  SetAfebThreshold(int AFEB,                     // set Write values -> AFEB = [0-(GetNumberOfAfebs()-1)]
	 		 int dacValue);                //                     Voltage set = 2.5V * dacValue/256 (8-bit dac)
  int   GetAfebThresholdDAC(int AFEB);                 // get Write values -> AFEB = [0-(GetNumberOfAfebs()-1)]
  int   GetAfebThresholdADC(int AFEB);                 // get Read values -> AFEB = [0-(GetNumberOfAfebs()-1)] (10-bit adc value)
  float GetAfebThresholdVolts(int AFEB);               // get Read values -> return voltage = 2.5V * adcValue/1023 
  //
  void  SetPowerUpAfebThresholds();		       // sets Write values to data-taking defaults                
  void  PrintAfebThresholds();			       // print out Read values				 
  //
  //
  void  WriteAfebThresholds();                    //writes Write values to ALCT DAC
  void  ReadAfebThresholds();			  //fills Read values with values read from ALCT ADC	 
  //
  //////////////////////////////
  // ASIC DELAYS and PATTERNS 
  //////////////////////////////
  void SetAsicDelay(int AFEB,                        // set Write values -> AFEB = [0-(GetNumberOfAfebs()-1)]
		    int delay);                      //                    delay = [0-15] (~2ns steps)
  int  GetAsicDelay(int AFEB);                       // get Read values -> AFEB = [0-(GetNumberOfAfebs()-1)]
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
  void SetTriggerMode(int trigger_mode);            // set Write values...
  //                      trigger_mode = [0-3] -> 0 = pre-trigger on either collision muon or accelerator muon pattern
  //                                              1 = only pre-trigger on accelerator muon
  //                                              2 = only pre-trigger on collision muon
  //                                              3 = pre-trigger on collision muon, veto pre-trigger on accelerator muon
  int  GetTriggerMode();                            // get Read values
  //
  //
  void SetExtTrigEnable(int ext_trig_enable);       // set Write values...
  //                      ext_trig_enable = [0-1] -> 0 = disable
  //                                                 1 = enable (triggers if ext_trig input signal == 1)
  int  GetExtTrigEnable();                          // get Read values
  //
  //
  void SetSendEmpty(int send_empty);                // set Write values...
  //                      send_empty = [0-1] -> 0 = do not send DAQ for empty events
  //                                            1 = do send DAQ for empty events
  int  GetSendEmpty();                              // get Read values
  //
  //
  void SetInjectMode(int inject);                   // set Write values...
  //                       inject = [0-1] -> 0 = input data enabled
  //                                         1 = input data enabled if ext_inject input == 1
  int  GetInjectMode();                             // get Read values
  //
  //
  void SetBxcOffset(int bxc_offset);                // set Write values...
  //                       bxc_offset = [0-255] -> value loaded into internal BX counter upon BC0
  int  GetBxcOffset();                              // get Read values
  //
  //
  void SetPretrigNumberOfLayers(int nph_thresh);    // set Write values...
  //                       nph_thresh = [0-6] -> number of layers needed to generate pretrigger (marks bunch crossing)
  int  GetPretrigNumberOfLayers();                  // get Read values
  //
  //
  void SetPretrigNumberOfPattern(int nph_pattern);  // set Write values...
  //                       nph_pattern = [0-6] -> number of layers needed after "drift delay" (starting from pretrigger) to generate LCT
  int  GetPretrigNumberOfPattern();                 // get Read values
  //
  //
  void SetDriftDelay(int drift_delay);              // set Write values...
  //                       drift_delay = [0-3] -> number of bunch crossings between pretrigger and LCT
  int  GetDriftDelay();                             // get Read values
  //
  //
  void SetFifoTbins(int fifo_tbins);                // set Write values...
  //                       fifo_tbins = [0-31] -> number of bunch crossings in FIFO DAQ readout  
  int  GetFifoTbins();                              // get Read values
  //
  //
  void SetFifoPretrig(int fifo_pretrig);            // set Write values...
  //                       fifo_pretrig = [0-31] -> raw hits will be shown in DAQ readout from (fifo_pretrig-10) bunch crossings before trigger event
  int  GetFifoPretrig();                            // get Read values
  //
  //
  void SetFifoMode(int fifo_mode);                  // set Write values...
  //                       fifo_mode = [0-2] -> 0 = no raw hits dump
  //                                            1 = Full dump (all LCT chips)
  //                                            2 = local dump (LCT chips with hits)
  int  GetFifoMode();                               // get Read values
  //
  //
  void SetL1aDelay(int l1a_delay);                  // set Write values...
  //                       l1a_delay = [0-255] -> number of bunch crossings (after the LCT) of the leading edge of the l1a window 
  int  GetL1aDelay();                               // get Read values
  //
  //
  void SetL1aWindowSize(int size);                  // set Write values...
  //                       size = [0-15] -> width of l1a window in bunch crossings 
  int  GetL1aWindowSize();                          // get Read values
  //
  //
  void SetL1aOffset(int l1a_offset);                // set Write values...
  //                       l1a_offset = [0-15] -> l1a accept counter pre-load value
  int  GetL1aOffset();                              // get Read values
  //
  //
  void SetL1aInternal(int l1a_internal);            // set Write values...
  //                       l1a_internal = [0-1] -> 0 = l1a expected to come from CCB via TMB
  //                                               1 = l1a generated internally (within the l1a window)
  int  GetL1aInternal();                            // get Read values
  //
  //
  void SetBoardId(int board_id);                    // set Write values...
  //                       board_id = [0-7] -> ALCT2001 circuit board ID (defunct)
  int  GetBoardId();                                // get Read values
  //
  //
  void SetCcbEnable(int ccb_enable);                // set Write values...
  //                       ccb_enable = [] -> undocumented parameter
  int  GetCcbEnable();                              // get Read values
  //
  //
  void SetAlctAmode(int alct_amode);                // set Write values...
  //                       alct_amode = [0-3] -> use in conjunction with trig_mode
  //                                             0,1 = prefer Collision muon mode
  //                                             2,3 = prefer Accelerator muon mode
  int  GetAlctAmode();                              // get Read values
  //
  //
  void SetTriggerInfoEnable(int trigger_info_en);   // set Write values...
  //                       trigger_info_en = [0-1] -> 0 = do not write trigger info to FIFO
  //                                                  1 = write trigger info to FIFO
  int  GetTriggerInfoEnable();                      // get Read values
  //
  //
  void SetSnSelect(int sn_select);                  // set Write values...
  //                       sn_select = [0-1] -> 0 = read ALCT serial number via JTAG (defunct)
  //                                            1 = read Mezzanine card serial number via JTAG (defunct)
  int  GetSnSelect();                               // get Read values
  //
  //
  void SetPowerUpConfigurationReg();                // sets Write values to data-taking defaults
  void PrintConfigurationReg();                     // print out Read values
  //
  //
  void WriteConfigurationReg();                 //writes Write values to ALCT
  void ReadConfigurationReg();                  //fills Read values with values read from ALCT
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
protected:
  //
  //
private:
  //
  std::ostream * MyOutput_ ;
  TMB * tmb_ ;
  //
  //
  ////////////////////////////////////////////////////////////////////
  // Private variables specific to the chamber-type:                //
  ////////////////////////////////////////////////////////////////////
  void SetChamberCharacteristics_(std::string chamberType);
  std::string chamber_type_string_;  
  int NumberOfWireGroupsInChamber_;
  //
  ////////////////////////////////////////////////////////////////////
  // Private variables specific to the ALCT-type:                   //
  ////////////////////////////////////////////////////////////////////
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
  //////////////////////////////////////////////////////
  // Slow-control registers private variables:        //
  //////////////////////////////////////////////////////
  char read_slowcontrol_id_[RegSizeAlctSlowFpga_RD_ID_REG/8+1];
  //
  int write_afeb_threshold_[MAX_NUM_AFEBS];
  int read_afeb_threshold_[MAX_NUM_AFEBS];
  int read_adc_(int ADCchipNumber, int ADCchannel);
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
  void SetTestpulseAmplitude_(int dacvalue);        // set Write values -> Voltage = 2.5V * dacvalue/256
  //
  void SetPowerUpTestpulseAmplitude_();             // sets Write values to data-taking defaults
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
  /////////////////////////////////////////////////
  //STANDBY REGISTER - enable power for each AFEB
  /////////////////////////////////////////////////
  int write_standby_register_[RegSizeAlctSlowFpga_WRT_STANDBY_REG];
  int read_standby_register_[RegSizeAlctSlowFpga_RD_STANDBY_REG];
  //
  void SetStandbyRegister_(int AFEB,                   // set Write Values -> AFEB = [0 - GetNumberOfAfebs()-1]
			   int powerswitch);           //                     powerswitch = OFF or ON
  int  GetStandbyRegister_(int AFEB);                  // get Read Values -> AFEB = [0 - GetNumberOfAfebs()-1]
  //
  void SetPowerUpStandbyRegister_();		      // set Write values to data-taking defaults          
  void PrintStandbyRegister_();			      // print out Read values				       
  //
  //
  void WriteStandbyRegister_();               // writes Write values to ALCT
  void ReadStandbyRegister_();		      // fills Read values with values read from ALCT	 
  //
  //
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // vectors of bits for the fast-control registers, variables in these registers, and methods to translate between the two... //
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  char read_fastcontrol_id_[RegSizeAlctFastFpga_RD_ID_REG/8+1];
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
  int write_drift_delay_;
  int write_fifo_tbins_;
  int write_fifo_pretrig_;
  int write_fifo_mode_;
  int write_l1a_delay_;
  int write_l1a_window_;
  int write_l1a_offset_;
  int write_l1a_internal_;
  int write_board_id_;
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
  int read_alct_amode_;
  int read_trigger_info_en_;
  int read_sn_select_;
  void DecodeConfigurationReg_();
  //
  int write_hot_channel_mask_[RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_672];             //make this as large as it could possibly be
  int read_hot_channel_mask_[RegSizeAlctFastFpga_RD_HOTCHAN_MASK_672];               //make this as large as it could possibly be
  bool stop_read_;                                                          // need this to stop JTAG checking from going to infinite loop
  //
  ///////////////////////////////////////////////////////////////////////////////
  // TESTPULSE TRIGGER REGISTER - specify which signal will fire the testpulse
  //////////////////////////////////////////////////////////////////////////////
  int write_trigger_reg_[RegSizeAlctFastFpga_WRT_TRIG_REG];
  int write_pulse_trigger_source_;
  int write_invert_pulse_;
  void FillTriggerRegister_();
  //
  int read_trigger_reg_[RegSizeAlctFastFpga_RD_TRIG_REG];
  int read_pulse_trigger_source_;
  int read_invert_pulse_;
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
  //
  void SetPowerUpTriggerRegister_();	       // sets Write values to data-taking defaults
  void PrintTriggerRegister_();                // print out Read values				 
  //
  //
  void WriteTriggerRegister_();                 //writes Write values to ALCT
  void ReadTriggerRegister_();                  //fills Read values with values read from ALCT
  //
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
  //
  void WriteDelayLineControlReg_();                  //writes Write values to ALCT
  void ReadDelayLineControlReg_();                  //fills Read values with values read from ALCT
  //
};
#endif

#endif
