//-----------------------------------------------------------------------
// $Id: ALCTController.h,v 2.4 2005/08/31 15:12:56 mey Exp $
// $Log: ALCTController.h,v $
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

#include "ALCTClasses.h"
#include <string>
#include <bitset>
  
class TMB;
class elong;
class AnodeChannel;

class ALCTController {
public:
    friend class TMBParser;
 
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
      int SVFLoad(int*, char *, int);
      int NewSVFLoad(int*, char *, int);
      int ConvertCharToInteger(char [], int [], int &);
      int ConstructShift(char HeaderData[8192],int HeaderDataSize, char Data[8192], int DataSize, 
			 char TrailerData[8192], int TrailerDataSize, int[] );
  int ReadIDCODE();
  int nAfebs() { return nAFEBs_; }
  //fg temporarily made public
    unsigned int delays_[42];
    int thresholds_[42];

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
    int alct_drift_delay_;
    std::string chamber_type_string_;
  
    /// this holds all the other non-congurable parameters.  It probably should disappear someday
    alct_params_type params_;
  
  
  private:
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

  void setConfig();

};
  
#endif
  
