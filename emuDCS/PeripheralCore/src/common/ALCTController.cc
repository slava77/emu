//-----------------------------------------------------------------------
// $Id: ALCTController.cc,v 3.5 2006/08/08 16:38:27 rakness Exp $
// $Log: ALCTController.cc,v $
// Revision 3.5  2006/08/08 16:38:27  rakness
// ALCTnew: remove parameters not supported in firmware
//
// Revision 3.4  2006/08/04 15:49:58  mey
// Update
//
// Revision 3.3  2006/08/03 19:00:48  mey
// Trigger on 1/1
//
// Revision 3.2  2006/08/03 18:50:49  mey
// Replaced sleep with ::sleep
//
// Revision 3.1  2006/07/23 15:42:51  rakness
// index Hot Channel Mask from 0
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.45  2006/07/20 11:07:59  rakness
// make many functions private
//
// Revision 2.44  2006/07/19 18:11:17  rakness
// Error checking on ALCTNEW
//
// Revision 2.43  2006/07/18 15:23:14  mey
// UPdate
//
// Revision 2.42  2006/07/18 12:21:55  rakness
// ALCT threshold scan with ALCTNEW
//
// Revision 2.41  2006/07/14 11:46:31  rakness
// compiler switch possible for ALCTNEW
//
// Revision 2.40  2006/07/12 15:06:59  rakness
// option for cleaned up ALCT
//
// Revision 2.39  2006/07/12 12:07:11  mey
// ALCT connectivity
//
// Revision 2.38  2006/07/04 15:06:19  mey
// Fixed JTAG
//
// Revision 2.37  2006/06/23 13:40:26  mey
// Fixed bug
//
// Revision 2.36  2006/06/22 13:06:14  mey
// Update
//
// Revision 2.35  2006/05/31 11:04:44  mey
// UPdate
//
// Revision 2.34  2006/05/30 07:22:20  mey
// UPdate
//
// Revision 2.33  2006/05/23 09:01:21  rakness
// Update
//
// Revision 2.32  2006/05/19 15:13:32  mey
// UPDate
//
// Revision 2.31  2006/05/19 12:46:48  mey
// Update
//
// Revision 2.30  2006/05/18 08:35:44  mey
// Update
//
// Revision 2.29  2006/04/27 18:46:04  mey
// UPdate
//
// Revision 2.28  2006/04/20 15:33:54  mey
// Update
//
// Revision 2.27  2006/03/30 13:55:38  mey
// Update
//
// Revision 2.26  2006/03/24 14:35:04  mey
// Update
//
// Revision 2.25  2006/03/16 19:30:02  mey
// UPdate
//
// Revision 2.24  2006/03/15 16:42:57  mey
// Update
//
// Revision 2.23  2006/03/13 13:23:17  mey
// Update
//
// Revision 2.22  2006/03/10 15:55:28  mey
// Update
//
// Revision 2.21  2006/03/10 10:51:47  mey
// Update
//
// Revision 2.20  2006/03/03 07:59:19  mey
// Update
//
// Revision 2.19  2006/02/25 11:25:11  mey
// UPdate
//
// Revision 2.18  2006/02/20 13:31:13  mey
// Update
//
// Revision 2.17  2006/02/07 22:49:25  mey
// UPdate
//
// Revision 2.16  2005/12/16 17:49:27  mey
// Update
//
// Revision 2.15  2005/12/10 11:24:14  mey
// Update
//
// Revision 2.14  2005/12/02 18:12:41  mey
// get rid of D360
//
// Revision 2.13  2005/11/25 23:42:17  mey
// Update
//
// Revision 2.12  2005/11/22 15:14:46  mey
// Update
//
// Revision 2.11  2005/11/21 19:47:06  mey
// Update
//
// Revision 2.10  2005/11/21 15:47:59  mey
// Update
//
// Revision 2.9  2005/11/15 15:22:32  mey
// Update
//
// Revision 2.8  2005/11/10 18:25:21  mey
// Update
//
// Revision 2.7  2005/11/09 20:07:13  mey
// Update
//
// Revision 2.6  2005/11/02 16:16:06  mey
// Update for new controller
//
// Revision 2.5  2005/11/02 10:58:20  mey
// Update bxc_offset
//
// Revision 2.4  2005/08/31 15:12:57  mey
// Bug fixes, updates and new routine for timing in DMB
//
// Revision 2.3  2005/08/15 15:37:48  mey
// Include alct_hotchannel_file
//
// Revision 2.2  2005/08/11 08:13:04  mey
// Update
//
// Revision 2.1  2005/06/06 15:17:18  geurts
// TMB/ALCT timing updates (Martin vd Mey)
//
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
// Revision 1.33  2004/09/15 21:44:42  tfcvs
// New timing routines
//
// Revision 1.32  2004/09/14 00:27:07  tfcvs
// Update files
//
// Revision 1.31  2004/08/12 18:02:09  tfcvs
// Update
//
// Revision 1.30  2004/06/05 19:34:32  tfcvs
// Reintroduced ALCT pattern file download. File configuration set through
// XML <ALCT alct_pattern_file="">. (FG)
//
//-----------------------------------------------------------------------
#ifndef ALCTNEW

#include "ALCTController.h"
#include "ALCTClasses.h"
#include "TMB.h"
#include "elong.h"
#include "alct_registers.h"
#include <sys/ioctl.h>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <cmath>
#include <string>
//#include <algo.h>


//using namespace std;

char* trim(char* str);
char* strtolower(char* str);

const char* alct_errs[] =
{
        "Successful completion",                        // EALCT_SUCCESS
        "File could not be opened",                     // ELACT_FILEOPEN
        "Configuration file is not complete",           // EALCT_FILEDEFECT
        "JTAG Port link problem",                       // EALCT_PORT
        "Bad chamber type",                             // EALCT_CHAMBER
        "Argument out of range",                        // EALCT_ARG
        "Test failure",                                 // EALCT_TESTFAIL
};

const char* alct_selftest_errs[] =
{
        "Self Test success",                            // 0
        "Wrong Chamber Type",                           // 1
        "Problem with Slow Control ID Code",            // 2
        "Problem with Thresholds",                      // 3
        "Problem with Standby Register",                // 4
        "Problem with Test Pulse Power Down",           // 5
        "Problem with Test Pulse Power Up",             // 6
        "Problem with Test Pulse Group Mask",           // 7
        "Problem with Test Pulse Strip Mask",           // 8
        "Problem with reading of 1.8V Voltage",         // 9
        "Problem with reading of 3.3V Voltage",         // 10
        "Problem with reading of 5.5V (1) Voltage",     // 11
        "Problem with reading of 5.5V (2) Voltage",     // 12
        "Problem with reading of 1.8V Current",         // 13
        "Problem with reading of 3.3V Current",         // 14
        "Problem with reading of 5.5V (1) Current",     // 15
        "Problem with reading of 5.5V (2) Current",     // 16
        "Problem with reading OnBoard Temperature"      // 17

};


// IR instruction codes (from alct_fast_lib.h)
enum InstructionCodes
{
      IDRead        = 0,   // Virtex ID register read
      HCMaskRead    = 0x1, // hot mask
      HCMaskWrite   = 0x2,
      RdTrig        = 0x3, // trigger register
      WrTrig        = 0x4,
      RdCfg         = 0x6, // read control register
      WrCfg         = 0x7, // write control register
      Wdly          = 0xd, // write delay lines. cs_dly bits in Par
      Rdly          = 0xe, // read  delay lines. cs_dly bits in Par

      CollMaskRead  = 0x13, // collision pattern mask
      CollMaskWrite = 0x14,
      ParamRegRead  = 0x15, // Delay line control register actually
      ParamRegWrite = 0x16,
      InputEnable   = 0x17, // Commands to disable and enable input
      InputDisable  = 0x18,
      YRwrite       = 0x19, // Output register write (for debugging with UCLA test board)
      OSread        = 0x1a, // Output storage read

      SNread        = 0x1b, // read one bit of serial number
      SNwrite0      = 0x1c, // write 0 bit into serial number chip
      SNwrite1      = 0x1d, // write 1 bit into serial number chip
      SNreset       = 0x1e, // reset serial number chip

      Bypass        = 0x1f

};

enum ALCT_FAST_ERRORS
{
  ALCT_FAST_ERROR_READBACK = 0,
  ALCT_FAST_ERROR_PATTERN
};


Rfield  CRfld[] =
  {
    {   3,  0,    0, "trig_mode"},
    {   1,  2,    0, "ext_trig_en"},
    {   1,  3,    0, "send_empty"},
    {   1,  4,    0, "inject"},
    {0xff,  5,    0, "bxc_offset"},
    {   7, 13,    2, "nph_thresh"},
    {   7, 16,    4, "nph_pattern"},
    {   3, 19,    3, "drift_delay"},
    {0x1f, 21,    7, "fifo_tbins"},
    {0x1f, 26,    1, "fifo_pretrig"},
    {   3, 31,    1, "fifo_mode"},
    {   7, 33,    3, "fifo_lastlct"},
    {0xff, 36, 0x78, "l1a_delay"},
    { 0xf, 44,    3, "l1a_window"},
    { 0xf, 48,    0, "l1a_offset"},
    {   1, 52,    0, "l1a_internal"},
    {   7, 53,    5, "board_id"},
    { 0xf, 56,    0, "bxn_offset"},
    {   1, 60,    0, "ccb_enable"},
    {   1, 61,    1, "alct_jtag_ds"},
    {   3, 62,    0, "alct_tmode"},
    {   3, 64,    0, "alct_amode"},
    {   1, 66,    0,"alct_mask_all"},
    {   1, 67,    1, "trig_info_en"},  // have to provide the field in alct_params_type
    {   1, 68,    0, "sn_select"}   // have to provide the field in alct_params_type
  };

// sizes of the JTAG data registers, depending on the instruction code.
// Instruction code is used as index for this array, to fetch the size
unsigned RegSz384[] =
{
        40, // IDRead        = 0,
        384,// HCMaskRead    = 0x1,
        384,// HCMaskWrite   = 0x2,
        5,      // RdTrig        = 0x3,
        5,      // WrTrig        = 0x4,
        0,
        69,     // RdCfg         = 0x6, // read control register
        69,     // WrCfg         = 0x7, // write control register

        0,
        0,
        0,
        0,
        0,
        120,// Wdly          = 0xd, // write delay lines. cs_dly bits in Par
        121,// Rdly          = 0xe, // read  delay lines. cs_dly bits in Par
        0,

        0,
        0,
        0,
        224,// CollMaskRead  = 0x13,
        224,// CollMaskWrite = 0x14,
        6,      // ParamRegRead  = 0x15,
        6,      // ParamRegWrite = 0x16,
        0,      // InputEnable   = 0x17,

        0,      // InputDisable  = 0x18,
        31,     // YRwrite               = 0x19,
        49,     // OSread                = 0x1a,
        1,  // SNread        = 0x1b,
        0,  // SNwrite0      = 0x1c,
        0,  // SNwrite1      = 0x1d,
        0,  // SNreset       = 0x1e,
        1       // Bypass        = 0x1f
};


unsigned RegSz672[] =
{
        40, // IDRead        = 0,
        672,// HCMaskRead    = 0x1,
        672,// HCMaskWrite   = 0x2,
        5,      // RdTrig        = 0x3,
        5,      // WrTrig        = 0x4,
        0,
        69,     // RdCfg         = 0x6, // read control register
        69,     // WrCfg         = 0x7, // write control register

        0,
        0,
        0,
        0,
        0,
        120,// Wdly          = 0xd, // write delay lines. cs_dly bits in Par
        121,// Rdly          = 0xe, // read  delay lines. cs_dly bits in Par
        0,

        0,
        0,
        0,
        392,// CollMaskRead  = 0x13,
        392,// CollMaskWrite = 0x14,
        9,      // ParamRegRead  = 0x15,
        9,      // ParamRegWrite = 0x16,
        0,      // InputEnable   = 0x17,

        0,      // InputDisable  = 0x18,
        31,     // YRwrite               = 0x19,
        51,     // OSread                = 0x1a, // 49
        1,  // SNread        = 0x1b,
        0,  // SNwrite0      = 0x1c,
        0,  // SNwrite1      = 0x1d,
        0,  // SNreset       = 0x1e,
        1       // Bypass        = 0x1f
};

unsigned RegSz288[] =
{
        40, // IDRead        = 0,
        288,// HCMaskRead    = 0x1,
        288,// HCMaskWrite   = 0x2,
        5,      // RdTrig        = 0x3,
        5,      // WrTrig        = 0x4,
        0,
        69,     // RdCfg         = 0x6, // read control register
        69,     // WrCfg         = 0x7, // write control register

        0,
        0,
        0,
        0,
        0,
        120,// Wdly          = 0xd, // write delay lines. cs_dly bits in Par
        121,// Rdly          = 0xe, // read  delay lines. cs_dly bits in Par
        0,

        0,
        0,
        0,
        168,// CollMaskRead  = 0x13,
        168,// CollMaskWrite = 0x14,
        5,      // ParamRegRead  = 0x15,
        5,      // ParamRegWrite = 0x16,
        0,      // InputEnable   = 0x17,

        0,      // InputDisable  = 0x18,
        31,     // YRwrite               = 0x19,
        49,     // OSread                = 0x1a, // 49
        1,  // SNread        = 0x1b,
        0,  // SNwrite0      = 0x1c,
        0,  // SNwrite1      = 0x1d,
        0,  // SNreset       = 0x1e,
        1       // Bypass        = 0x1f
};

// lengths of the fields in the OS register, starting from LSB
int OSfld384[] = {9, 19, 1, 6, 2, 1, 1, 6, 2, 1, 1};
int OSfld672[] = {9, 19, 1, 7, 2, 1, 1, 7, 2, 1, 1};

chamtypefield chamtype[] =
{
    {"ME1/1",   288 /*288*/, RegSz288,     OSfld384},
    {"ME1/2",   384 /*384*/, RegSz384,     OSfld384},
    {"ME1/3",   288 /*192*/, RegSz288,     OSfld384},
    {"ME2/1",   672 /*672*/, RegSz672,     OSfld672},
    {"ME234/2", 384 /*384*/, RegSz384,     OSfld384},
    {"ME3/1",   672 /*576*/, RegSz672,     OSfld672},
    {"ME4/1",   672 /*576*/, RegSz672,     OSfld672},

};

const int NUM_OF_CHAMBER_TYPE_S = 9;
const int NUM_OF_PWR_CHANNELS = 4;
const int MAX_DAC = 255;
const int NUM_OF_GROUPS = 7;
float volt_table[4][2] = {{1.8, 0.1}, {3.3, 0.1}, {5.5, 0.1}, {5.5, 0.1}};
// const float curr_table[4][2] = {{0.667, 0.1}, {0.726, 0.1}, {0.176, 0.1}, {0.176, 0.1}};
// const float curr_table[4][2] = {{0.650, 0.1}, {1.2, 0.2}, {0.250, 0.1}, {0.176, 0.1}};
float curr_table[4][2] = {{0.620, 0.1}, {1.400, 0.2}, {1.000, 0.1}, {0.500, 0.1}};

// const ROOM_TEMP = 25.0;
float temper[2] = {25.0, 5};


#define WRONG_CHAMBER_TYPE_(CH_TYPE)  (((CH_TYPE) < 0) || ((CH_TYPE) >= NUM_OF_CHAMBER_TYPE_S) || ((CH_TYPE) != chamb_table[CH_TYPE].chmbtype))
#define WRONG_PORT(CH_TYPE) (alct_slowcontrol_id_status())
#define WRONG_CHANNEL(CH_TYPE, CHAN) (((CHAN) < 0) || ((CHAN) > chamb_table[CH_TYPE].wiregroups /alct_table[chamb_table[CH_TYPE].alct].delaylines))
// #define WRONG_CHANNEL(CH_TYPE, CHAN) (((CHAN) < 0) || ((CHAN) >= (alct_table[chamb_table[CH_TYPE].alct].groups * alct_table[chamb_table[CH_TYPE].alct].chips)))
#define WRONG_GROUP(CH_TYPE, GROUP) (((GROUP) < 0) || ((GROUP) >= chamb_table[CH_TYPE].wiregroups / (alct_table[chamb_table[CH_TYPE].alct].chips*alct_table[chamb_table[CH_TYPE].alct].delaylines)))
// #define WRONG_GROUP(CH_TYPE, GROUP) (((GROUP) < 0) || ((GROUP) >= (alct_table[chamb_table[CH_TYPE].alct].groups)))
#define WRONG_STANDBY_MASK(CH_TYPE, MASK) (((MASK) < 0) || ((MASK >= (1 << alct_table[chamb_table[CH_TYPE].alct].chips))))
#define WRONG_DAC_VALUE(THRESH) (((THRESH) < 0)   || ((THRESH) > MAX_DAC))
#define WRONG_PWR_CHANNEL(CHAN) (((CHAN) < 0) || ((CHAN) >= NUM_OF_PWR_CHANNELS))

/*#ifdef ALCT_SLOW_LIB_LOCAL*/
// 3 ALCT Types
typedef enum
{
        ALCT288 = 0,    // ALCT 288 Channels
        ALCT384 = 1,    // ALCT 384 Channels
        ALCT672 = 2     // ALCT 672 Channels
} ALCTTYPE;

// ALCT Parameters
struct alct_type
{
        ALCTTYPE alcttype;
        long channels;   // Channels on ALCT
        long groups;     // Delay chips groups
        long chips;      // Number of delay chips in group
        long delaylines;  // Number of lines in delay chip
};

// Chamber Parameters
struct chamb_type
{
        ALCTController::CHAMBER chmbtype;  // Chamber type
        ALCTTYPE alct;    // Appropriate ALCT type
        long wiregroups;   // Wire groups on chamber
};

alct_type alct_table[3] =
{
        { ALCT288, 288, 3, 6, 16}, // ALCT 288
        { ALCT384, 384, 4, 6, 16}, // ALCT 384
        { ALCT672, 672, 7, 6, 16}  // ALCT 672
};

chamb_type chamb_table[7] =
{
        { ALCTController::ME11, ALCT288, 288}, // ME 1/1
        { ALCTController::ME12, ALCT384, 384}, // ME 1/2
        { ALCTController::ME13, ALCT288, 192}, // ME 1/3
        { ALCTController::ME21, ALCT672, 672}, // ME 2/1
        { ALCTController::ME22, ALCT384, 384}, // ME 2/234
        { ALCTController::ME31, ALCT672, 576}, // ME 3/1
        { ALCTController::ME41, ALCT672, 576}, // ME 4/1
};


// from jtag_b.h

#define jtag_bBitStructSize sizeof(struct JTAG_BBitStruct)

/* Need linux/ioctl.h, from linux/fs.h for these */
/*   The magic number 'Z' is used without deep consideration of alternatives */
/* this number was changed to 'Z' from 'z' to avoid conflict with jtag_a */
#define JTAG_BIORESET                  _IO('Z', 0)
#define JTAG_BIOIDLE                   _IO('Z', 1)
#define JTAG_BIOTARGET                 _IOW('Z', 2, enum jtag_b_target)
#define JTAG_BIOSETTAP                 _IO('Z', 3)
#define JTAG_BIOGETTARGET              _IOR('Z', 4, int *)
#define JTAG_BIOBIT                    _IOWR('Z', 5, char *)
#define JTAG_BIOBIT2                   _IOWR('Z', 14, char *)
#define JTAG_BIOLASTBIT                _IOWR('Z', 6, char *)
#define JTAG_BIOEXCHANGE               _IOWR('Z', 7, struct jtag_bBitStruct *)
#define JTAG_BLIGHTS                   _IO('Z', 8)
#define JTAG_BIOEXCHANGE_ALCT          _IOWR('Z', 9, struct jtag_bBitStruct *)
#define JTAG_BENABLE                   _IO('Z', 10)
#define JTAG_BIORTI                    _IO('Z', 11)
#define JTAG_BIO_1CLK                  _IO('Z', 12)
#define JTAG_BCHANNEL                  _IOWR('Z', 13, int )
#define JTAG_BMODULERESET              _IO('Z', 127)

  enum jtag_b_modes {JTAG_B_DEFAULT=0, JTAG_B_PAUSE, JTAG_B_STRING};
  enum jtag_b_target {JTAG_B_DATA, JTAG_B_INSTR};

struct jtag_bStruct
{
  /* Should there be a DEFAULT VALUE? */
  enum jtag_b_target target;
};

struct JTAG_BBitStruct
{
  /* Structure used to reference a one bit per byte structure of n length */

  enum jtag_b_target target; /* overrides the jtag_bStruct value */
  int n; /* number of bytes to copy; number of bits to transmit */
  char *data; /* data to transmit AND data received */
};



// end from jtag_b.h


ALCTController::ALCTController(TMB * tmb, std::string chamberType) : 
  tmb_(tmb), 
  jtag_initialized_(false), 
  chamber_type_string_(chamberType), 
  alct_link_type(VME), 
  fd(tmb_->slot()),
  delays_inited_(false),
  alct_trig_mode_(0),
  alct_inject_(0),
  alct_ext_trig_en_(0),
  alct_fifo_mode_(1),
  alct_send_empty_(0),
  alct_fifo_pretrig_(7),
  alct_fifo_tbins_(10),
  alct_bxc_offset_(1),
  alct_drift_delay_(3),
  alct_nph_thresh_(2),
  alct_nph_pattern_(2),
  alct_l1a_delay_(146),
  alct_l1a_window_(3),
  alct_l1a_internal_(0),
  alct_ccb_enable_(1),
  alct_l1a_offset_(1)
{
  alctPatternFile="";
  alctHotChannelFile="";
  //
  ALCT_FAST_JTAG_CHANNEL = ALCT_FAST_VME_JTAG_CHANNEL;
  if(chamber_type_string_ == "ME11") chamber_type_ = ME11;
  if(chamber_type_string_ == "ME12") chamber_type_ = ME12;
  if(chamber_type_string_ == "ME13") chamber_type_ = ME13;
  if(chamber_type_string_ == "ME21") chamber_type_ = ME21;
  if(chamber_type_string_ == "ME22") chamber_type_ = ME22;
  if(chamber_type_string_ == "ME32") chamber_type_ = ME22;
  if(chamber_type_string_ == "ME42") chamber_type_ = ME22;
  if(chamber_type_string_ == "ME31") chamber_type_ = ME31;
  if(chamber_type_string_ == "ME41") chamber_type_ = ME41;
  //
  std::cout << "^^^^^^^" << chamber_type_string_ << std::endl ;
  //
  if(WRONG_CHAMBER_TYPE_(chamber_type_)) throw ("Bad chamber type in ALCTController");
  nAFEBs_ = chamb_table[chamber_type_].wiregroups / alct_table[chamb_table[chamber_type_].alct].delaylines;
  n_lct_chips_ = alct_table[chamb_table[chamber_type_].alct].groups;
  std::cout << " ALCT: type " << chamber_type_string_ << " " << chamber_type_ << " AFEBS " << nAFEBs_ << " chips " << n_lct_chips_ << std::endl;
  alct_rsz = chamtype[chamber_type_].RegSz;
  
  // now give a default value for the control registers
  unsigned cr[] = {0x80fc5fc0, 0x20a03786, 0x8}; // default values for CR
  setCRfld(&params_);
  std::cout << "Done" << std::endl ;
  //unpackControlRegister(cr);
  //
}



void ALCTController::setup(int choice)
{
  //
  std::ostringstream dump;
  dump << (int)tmb_->slot();
  //
  tmb_->SendOutput("ALCT : configure() in slot = "+dump.str(),"INFO");
  //
  long value ;
  int err ;
  ALCTIDRegister sc_id, chipID ;
  int slot = tmb_->slot();
  //
  //printf("*setup********************* alct_fifo_pretrig %d \n",alct_fifo_pretrig_);
  //
  // ALCT stuff
  //
  err = alct_read_slowcontrol_id(&sc_id) ;
  std::cout <<  " ALCT Slowcontrol ID " << sc_id << std::endl;
  err = alct_init_slow_control(&slot,(long)10) ;

  printf(" Initialize ALCT err %d \n",err ) ;
  alct_read_standby(&slot, 3, &value) ;
  printf("\n") ;


  printf("\n") ;

  printf ("\nalct_fast_self_test returned %ld\n", alct_fast_self_test ((long int*)NULL,(unsigned long)1));

  alct_fast_read_id(chipID);
  printf ("alct_fast_read_id:\n");
  std::cout << chipID << std::endl;

/*
  if ( choice == 1 ) {
    if ( slot == 0x06 ) 
      printf ("alct_download_fast_config returned %ld\n", alct_download_fast_config 
	      ("configuration_clct_1.file", &alctparam, (unsigned long)1));
    if ( slot == 0x10 ) 
      printf ("alct_download_fast_config returned %ld\n", alct_download_fast_config 
	      ("configuration_clct_2.file", &alctparam, (unsigned long)1));
  }

  if ( choice == 2 ) 
    printf ("alct_download_fast_config returned %ld\n", alct_download_fast_config 
	    ("configuration_alct.file", &alctparam, (unsigned long)1));

  //Print to screen configuration parameters //Mohr (5/30/03)
  printf("\nReading Alct configuration parameters 1... \n");
  alct_read_fast_config(&alctparam,(unsigned long)1);
*/
  setConfig();
  setThresholds();
  //Print to screen configuration parameters //Mohr (5/30/03)
  printf("\nReading Alct configuration parameters ... \n");
  alct_read_fast_config(&params_,(unsigned long)1);

  printf ("alct_download_hot_mask returned %ld\n", alct_download_hot_mask 
	  //(dp->tmb_slt, "hcmask.file", 1));
	  ("hcmask.file", (long)1));


  setDelays();

  /*
  //Read delay chip settings -- is DESTRUCTIVE Mohr (5/27/03)
  alct_read_delay_destructively(slot,delays_,1);
  */
  /*
  //Adjust collision muon patterns //Mohr (6/14/03)
  printf("\nSetting collision muon patterns...\n");
  alct_download_patterns(dp->tmb_slt,"alct_patterns.txt",1);
  */


  // download patterns if filename is not empty.
  // change change verbosity level to 0 for no output.

  if (alctPatternFile != ""){
    int status=alct_download_patterns(alctPatternFile.c_str(),1);
    std::cout << "ALCT: download patterns. Status: " << alct_errs[status] << std::endl;
  }
  //
  if (alctHotChannelFile != ""){
    int status=alct_download_hot_mask(alctHotChannelFile.c_str(),1);
    std::cout << "ALCT: download hotchannel. Status: " << alct_errs[status] << std::endl;
  }

}


// from alct_fast_lib

// Set ALCT JTAG Channel 
int ALCTController::alct_fast_set_jtag_channel(int jtagchannel)
{
        
  if (alct_get_link_type() == LPT)
  {
    ALCT_FAST_JTAG_CHANNEL = ALCT_FAST_LPT_JTAG_CHANNEL;          
    if (ioctl(fd, JTAG_BCHANNEL, jtagchannel ) < 0)
     {
       return 1;
     }
  }

  if (alct_get_link_type() == VME)
  {
    ALCT_FAST_JTAG_CHANNEL = ALCT_FAST_VME_JTAG_CHANNEL;         
  } 
     return (0);
}

// Low Level JTAG One Bit Send/Read  Function
int ALCTController::jam_jtag_io(int tms, int tdi, int read_tdo, unsigned long * ret)
{
  if (alct_get_link_type() == LPT)
    {
      char tdio;
      int i;
      int err = 0;
      if (!jtag_initialized_)
        {
	  jtag_initialized_ = true;
	  for (i = 0; i < 6; i++) // set the TAP to the idle state
	    {
	      tdio = 0;
	      if (ioctl(fd, JTAG_BIOBIT2, &tdio ) < 0) err = 1;
	    }
	  nbits = 0;
        }

      tdio  = (tdi != 0) ? 1 : 0;
      tdio |= (tms != 0) ? 2 : 0;
      if (ioctl(fd, JTAG_BIOBIT2, &tdio ) < 0) err = 1;
      // the driver returns the result in character mode '0' or '1', and inverted (???
      if (ret != NULL) *ret = (tdio == '0') ? 1 : 0;
      if (read_tdo) nbits++;
      return (err == 1) ? -1 : (tdio == '0') ? 1 : 0; 
    }
  // !!! Write Handler for VME Link ?
  return (-1);
}

// Flush and Decode Received JTAG bits
void ALCTController::jam_jtag_flush ()
{
  if (alct_get_link_type() == LPT)
  {  
        unsigned long d, i, wc, b;

	if (readDR != NULL)
        {
                wc = 0;
                d = 0;
                for (unsigned int i = 0; i < nbits; i++)
                {
                        b = ((unsigned long)rbits[i]) << wc;
                        d |= b;
                        if (++wc >= 32)
                        {
                                *readDR = d;
                                readDR++;
                                wc = 0;
                                d = 0;
                        }
                }
		*readDR = d;
	}

	nbits = 0;
        readDR = NULL;
  }
  // ??? Do we need different handler for VME
}

//#########################################################################
/************************************************************************/
// Set Standard JTAG States

// Set JTAG State Machine into Start-IR-Shift State
int ALCTController::StartIRShift()
{
   int err = 0;
   if (alct_get_link_type() == LPT)
   {
        jam_jtag_io (0, 0, 0, NULL);
        jam_jtag_io (1, 0, 0, NULL);
        jam_jtag_io (1, 0, 0, NULL);
        jam_jtag_io (0, 0, 0, NULL);
        err = jam_jtag_io (0, 0, 0, NULL);
   }
   return err;
}

// Set JTAG State Machine into Start-DR-Shift State
int ALCTController::StartDRShift()
{
  int err = 0;
  if (alct_get_link_type() == LPT)
  {
        jam_jtag_io (0, 0, 0, NULL);
        jam_jtag_io (1, 0, 0, NULL);
        jam_jtag_io (0, 0, 0, NULL);
        err = jam_jtag_io (0, 0, 0, NULL);
  }
  return err;
}

// Set JTAG State Machine into Exit-IR-Shift State
int ALCTController::ExitIRShift()
{
  int err = 0;
  if (alct_get_link_type() == LPT)
  {      
        jam_jtag_io (1, 0, 0, NULL);
        jam_jtag_io (0, 0, 0, NULL);
        jam_jtag_io (0, 0, 0, NULL);
        err = jam_jtag_io (0, 0, 0, NULL);
  }
        return err;
}
// Set JTAG State Machine into Exit-DR-Shift State
int ALCTController::ExitDRShift()
{
  int err = 0;
  if (alct_get_link_type() == LPT)
  {
        jam_jtag_io (1, 0, 0, NULL);
        jam_jtag_io (0, 0, 0, NULL);
        jam_jtag_io (0, 0, 0, NULL);
        err = jam_jtag_io (0, 0, 0, NULL);
  }
        return err;
}

/***********************************************************/

// Writes Instruction Register (IR) of the JTAG 
int ALCTController::WriteIR(unsigned IR)
{
  unsigned i;
  if (alct_get_link_type() == LPT)
  {
        int err = 0;
        
        StartIRShift();
        for (i = 1; i <= IRsize; i++)
	  //@@ location of IRsize declaration unknown
        {
          err = jam_jtag_io (i == ALCT_V_IRsize, IR & 1, 0, NULL);
	  //@@ location of ALCT_V_IRsize declaration unknown
                IR >>= 1;
        }
        ExitIRShift();

        return err;
  }
  if (alct_get_link_type() == VME)
  {
   for (i=0; i< sizeof(IR); i++)
     {
       sndbuf[i] = (IR >> 8*i)  & 0x00ff;
     }
   tmb_->start(ALCT_FAST_VME_JTAG_CHANNEL);
   tmb_->scan(INSTR_REG, sndbuf, ALCT_V_IRsize, rcvbuf , 0);
   alct_end();
   return 0;
  }
  return (-1);
}

// shifts the data into DR of the JTAG. The IR must be already written with the proper instruction
// and the JTAG TAP must be already in the ShiftDR state.
int ALCTController::ShiftIntoDR(unsigned *DR, unsigned sz, unsigned sendtms)
{
  static unsigned realsz = 0;
  unsigned d, i, wc;
  int err = 0;
  if (alct_get_link_type() == LPT)
    {
        wc = 0;
        d = *DR;
        for (i = 1; i <= sz; i++)
        {
                err = jam_jtag_io ((i == sz) && sendtms, d & 1, 0, NULL);
                d >>= 1;
                if (++wc >= 32)
                {
                        d = *(++DR);
                        wc = 0;
                }
        }
        return err;
    }
  if (alct_get_link_type() == VME)
    {
        if (sz<=0) return EALCT_ARG;
        if (realsz==0)
          {
            bzero(&sndbuf,sizeof(sndbuf));
            bzero(&rcvbuf,sizeof(rcvbuf));
          }
        for (i=0; i< sz; i++)
          {
            sndbuf[(i+realsz)/8] |= ((*(DR+i/32) >> (i%32)) & 0x01) << ((i+realsz)%8);
          }
        realsz += sz;
        if (sendtms)
          {
            tmb_->start(ALCT_FAST_VME_JTAG_CHANNEL);
	    //
            tmb_->scan(DATA_REG, sndbuf, realsz, rcvbuf , 1);
	    //
            alct_end();
            realsz = 0;
          }
        return 0;
    }
    return (-1);
}

int ALCTController::ShiftFromDR(unsigned *DR, unsigned sz, unsigned sendtms)
{
  //  unsigned d, i, wc, b;
  unsigned i;
  int err = 0;
  if (alct_get_link_type() == LPT)
    {
/*
        wc = 0;
        d = 0;
        for (i = 1; i <= sz; i++)
        {
                b = jam_jtag_io ((i == sz) && sendtms, 0, 1);
                b <<= wc;
                d |= b;
                if (++wc >= 32)
                {
                        *DR = d;
                        DR++;
                        wc = 0;
                        d = 0;
                }
        }
        *DR = d;
 */
        for (i = 1; i <= sz; i++)
        {
                jam_jtag_io ((i == sz) && sendtms, 0, 1, &rbits[i-1]);
        }
        readDR = (unsigned long*)DR;    
        return err;
    }
  if (alct_get_link_type() == VME)
    { 
      if (sz<=0) return EALCT_ARG;
      bzero(&sndbuf, sizeof(sndbuf));
      tmb_->start(ALCT_FAST_VME_JTAG_CHANNEL);
      //
      tmb_->scan(DATA_REG, sndbuf, sz, rcvbuf , 1);
      //
      alct_end();

      for (i=0; i<((sz-1)/32+1); i++)
        {
          *(DR+i) = 0;
        }
      for (i=0; i<((sz-1)/8+1); i++)
        {
          (*(DR+i/4)) |= ((unsigned)(rcvbuf[i] & 0xff)) << 8*(i%4);
        }
      return 0;
    }
  return (-1);
}

// Writes data register (DR) of the JTAG. The IR must be already written with the proper instruction.
int ALCTController::WriteDR(unsigned *DR, unsigned sz)
{
  int err = 0;
  
  StartDRShift();
  err = ShiftIntoDR(DR, sz, 1);
  ExitDRShift();
  
  return err;
}

// Reads data register (DR) of the JTAG. The IR must be already written with the proper instruction
int ALCTController::ReadDR(unsigned *DR, unsigned sz)
{
  int err = 0;
  
  StartDRShift();
  err = ShiftFromDR(DR, sz, 1);
  ExitDRShift();
  
  return err;
}


// writes the data to any data register, using the instruction.
// The size of the data register determined automatically, using instruction code.
int ALCTController::WriteRegister (unsigned instruction, unsigned *data)
{
        int err;
        WriteIR(instruction);
        err = WriteDR(data, alct_rsz[instruction]);
        jam_jtag_flush();
        return err;
}


// reads the data from any data register, using the instruction.
// The size of the data register determined automatically, using instruction code.
int ALCTController::ReadRegister (unsigned instruction, unsigned *data)
{
        int err;
        WriteIR(instruction);
        err = ReadDR(data, alct_rsz[instruction]);
        jam_jtag_flush();
        return err;
}


// writes the data to any data register, using the instruction.
// The size of the data register determined automatically, using instruction code.
int ALCTController::elWriteRegister (unsigned instruction, elong *data)
{
        int err;
        WriteIR(instruction);
        err = WriteDR((unsigned *)data->r, alct_rsz[instruction]);
        jam_jtag_flush();
        return err;
}


// reads the data from any data register, using the instruction.
// The size of the data register determined automatically, using instruction code.
int ALCTController::elReadRegister (unsigned instruction, elong *data)
{
        int err;
        WriteIR(instruction);
        err = ReadDR((unsigned *)data->r, data->l = alct_rsz[instruction]);
        jam_jtag_flush();
        return err;
}

// flips the 32-bit word, so bit 0 is bit 31, bit 1 is bit 30, etc.
unsigned ALCTController::Flip (unsigned d)
{
        unsigned f = 0;
        int i, s;
        unsigned l = 0x80000000, r = 1;

        for (i = 0; i < 16; i++)
        {
                s = (15-i)*2 + 1;

                f |= ((d & r) << s);
                f |= ((d & l) >> s);

                l >>= 1;
                r <<= 1;
        }
        return f;
}

// flips the byte, so bit 0 is bit 7, bit 1 is bit 6, etc.
unsigned char ALCTController::FlipByte (unsigned char d)
{
        unsigned char f = 0;
        int i, s;
        unsigned char l = (unsigned char)0x80, r = 1;

        for (i = 0; i < 4; i++)
        {
                s = (3-i)*2 + 1;

                f |= ((d & r) << s);
                f |= ((d & l) >> s);

                l >>= 1;
                r <<= 1;
        }
        return f;
}

// writes 6 delay lines. delay is the array of 6 values by 4 bits, containing the desired delays,
// pattern is the array of 6 values by 16 bits, containing the patterns.
// cs is a 3,4,or 7-bit mask for chip select signals of the delay line groups. bit set to 1 enables the group
int ALCTController::Write6DelayLines(unsigned *delay, unsigned *pattern, unsigned cs)
{
        unsigned delayflip[6];
        unsigned r = 0x1ff & (~(cs << 2));
        unsigned rr = 0;
        int i, err = 0;
        elong el;
        el.clear();

        for (i = 0; i < 6; i++)
        {
                delayflip[i] = (FlipByte (delay[i])) >> 4;
                
        }
        
        WriteRegister (ParamRegWrite, &r);
        ReadRegister (ParamRegRead, &rr);

        for (i = 0; i < 6; i++)
        {
                el.cat(delayflip[i], 4);
                el.cat(pattern[i], 16);
        }

        err = elWriteRegister(Wdly, &el);   
        
//      r = 0x1ff; // this will connect the shift register outputs to virtex inputs
//      r = 0x1fd; // this will connect the outputs of AFEBs to virtex inputs
//      err = WriteRegister (ParamRegWrite, &r);
        return err;
}

// reads 6 delay lines. delay is the array of 6 values by 4 bits, containing the delays upon return ,
// pattern is the array of 6 values by 16 bits, containing the patterns upon return.
// cs is a 4-bit mask for chip select signals of the delay line groups. bit set to 1 enables the group
int ALCTController::Read6DelayLines(unsigned *delay, unsigned *pattern, unsigned cs)
{
        unsigned delayflip[6];
    unsigned r = 0x1ff & (~(cs << 2));
    unsigned rr = 0;
    int i, err = 0;
    elong el;
    el.clear();

    WriteRegister (ParamRegWrite, &r);
    ReadRegister (ParamRegRead, &rr);

        elReadRegister (Rdly, &el);
        
        for (i = 5; i >= 0; i--)
        {
                pattern[i] = el.split(16);
                delayflip[i] = el.split(4);
        }

        for (i = 0; i < 6; i++)
        {
                delay[i] = (FlipByte (delayflip[i])) >> 4;
        }

    r = 0x1ff;
    err = WriteRegister (ParamRegWrite, &r);
    return err;
}

unsigned char nest[14][6] =
{
        {0x80, 0x80, 0x01, 0x02, 0x02, 0x04},
        {0x01, 0x01, 0x02, 0x04, 0x04, 0x08},
        {0x02, 0x02, 0x04, 0x08, 0x08, 0x10},
        {0x04, 0x04, 0x08, 0x10, 0x10, 0x20},
        {0x08, 0x08, 0x10, 0x20, 0x20, 0x40},
        {0x10, 0x10, 0x20, 0x40, 0x40, 0x80},
        {0x20, 0x20, 0x40, 0x80, 0x80, 0x01},
        {0x40, 0x40, 0x80, 0x01, 0x01, 0x02},
        {0x80, 0x80, 0x01, 0x02, 0x02, 0x04},
        {0x01, 0x01, 0x02, 0x04, 0x04, 0x08},
        {0x02, 0x02, 0x04, 0x08, 0x08, 0x10},
        {0x04, 0x04, 0x08, 0x10, 0x10, 0x20},
        {0x08, 0x08, 0x10, 0x20, 0x20, 0x40},
        {0x10, 0x10, 0x20, 0x40, 0x40, 0x80}
};

// generates pattern in the specified WG and ORs it into the image (global variable).
// If clear != 0, the image is cleared.
void ALCTController::GeneratePattern (int WG, int clear)
{
        int i, j, NestNum, ColNum;
        unsigned char lnest[6], rnest[6];

        if (clear)
        {
                for (i = 0; i < 6; i++) for (j = 0; j < 14; j++) image[i][j] = 0;
        }
        else
        {
                NestNum = WG % 8;
                ColNum = WG/8;
                for (i = 0; i < 6; i++)
                {
                        lnest[i] = nest[NestNum][i];
                        rnest[i] = nest[NestNum][i];
                }
                if (NestNum >= 1 && NestNum <= 11)
                {
                        for (i = 0; i < 6; i++)
                        {
                                image[i][ColNum] |= lnest[i];
                        }
                }
                else
                {
                        if (NestNum == 0)
                        {
                                for (i = 0; i < 6; i++)
                                {
                                        lnest[i] &= 7;
                                        rnest[i] &= 0x80;
                                        image[i][ColNum] |= lnest[i];
                                        if (ColNum > 0) image[i][ColNum-1] |= rnest[i];
                                }
                                
                        }

                        if (NestNum > 11)
                        {
                                for (i = 0; i < 6; i++)
                                {
                                        lnest[i] &= 3;
                                        rnest[i] &= 0xf0;
                                        image[i][ColNum] |= rnest[i];
                                        if (ColNum < 7) image[i][ColNum+1] |= lnest[i];
                                }
                        }
                }
        }
}
//
void ALCTController::SetUpPulsing(long int Amplitude, int StripAfeb,long int stripMask){
  //
  long int PowerUp   = 1 ;
  //
  //long int Amplitude = 0x3f;
  long int StripMask;
  //
  int slot=tmb_->slot();
  //
  std::cout << "Init slot=" << slot << std::endl;
  //
  alct_set_test_pulse_powerup(&slot,0);
  //
  usleep(100);
  //
  alct_set_test_pulse_amp(&slot,Amplitude);
  //
  usleep(100);
  //
  alct_read_test_pulse_stripmask(&slot,&StripMask);
  std::cout << " StripMask = " << std::hex << StripMask << std::endl;
  //
  if(StripAfeb == 0 ) {
    alct_set_test_pulse_stripmask(&slot,0x00);
    alct_set_test_pulse_groupmask(&slot,0xff);
  }
  else if (StripAfeb == 1 ) {
    alct_set_test_pulse_stripmask(&slot,stripMask);
    alct_set_test_pulse_groupmask(&slot,0x00);
  } else {
    std::cout << "ALCTcontroller.SetUpPulsing : Don't know this option" <<std::endl;
  }
  //
  alct_read_test_pulse_stripmask(&slot,&StripMask);
  std::cout << " StripMask = " << std::hex << StripMask << std::endl;
  //
  alct_read_test_pulse_powerup(&slot,&PowerUp);
  std::cout << " PowerUp   = " << std::hex << PowerUp << std::dec << std::endl; //11July05 DM added dec
  //
  alct_fire_test_pulse('s');
  //
  usleep(100);
  //
  alct_set_test_pulse_powerup(&slot,1);
  //
  usleep(100);
  //
  alct_read_test_pulse_powerup(&slot,&PowerUp);
  std::cout << " PowerUp   = " << std::hex << PowerUp << std::dec << std::endl; //11July05 DM added dec
  //
}
//
void ALCTController::SetUpRandomALCT(){
  //
  unsigned long HCmask[22];
  unsigned long HCmask2[22];
  //
  for (int i=0; i< 22; i++) {
    HCmask[i] = 0;
    HCmask2[i] = 0;
  }
  //
  int keyWG  = int(rand()/(RAND_MAX+0.01)*(GetWGNumber())/6/4);
  int keyWG2 = (GetWGNumber())/6-keyWG;
  int ChamberSection = GetWGNumber()/6;
  //
  printf("Injecting at %d and %d\n",keyWG,keyWG2);
  //
  for (int i=0; i< 22; i++) HCmask[i] = 0;
  //
  std::bitset<672> bits(*HCmask) ;
  //
  for (int i=0;i<672;i++){
    if ( i%(GetWGNumber()/6) == keyWG ) bits.set(i);
    if ( i%(GetWGNumber()/6) == (GetWGNumber())/6-keyWG ) bits.set(i);
  }
  //
  std::bitset<32> Convert;
  //
  Convert.reset();
  //
  for (int i=0;i<(GetWGNumber());i++){
    if ( bits.test(i) ) Convert.set(i%32);
     if ( i%32 == 31 ) {
       HCmask[i/32] = Convert.to_ulong();
       Convert.reset();
     }
  }
  //
  alct_write_hcmask(HCmask);
  for(int i=0; i<22; i++) std::cout << std::hex << HCmask[i] << std::endl;
  alct_read_hcmask(HCmask);
  std::cout << std::endl;
  for(int i=0; i<22; i++) std::cout << std::hex << HCmask2[i] << std::endl;
  //
}
//
// generates acc pattern in the specified WG and ORs it into the image (global variable).
void ALCTController::GenerateAccPattern (int WG)
{
        int byten = WG / 8;
        int bitn = WG % 8;
        int i;
        for (i = 0; i < 6; i++) image[i][byten] |= (1 << bitn);
};
/*
char * HCfilename[] =
{
        "enableall.hcm",
        "disable1.hcm",
        "disable2.hcm",
        "disable3.hcm",
        "disable4.hcm",
        "disable5.hcm",
        "disable6.hcm"
};
  */
// qualities for the self test
unsigned long tq[7] =
{
        3,  // all layers are enabled
        2,  // layer 1 is disabled
        2,  // layer 2 is disabled
        2,  // layer 3 is disabled
        2,  // layer 4 is disabled
        2,  // layer 5 is disabled
        2   // layer 6 is disabled
};

void ALCTController::DecodeOS
(
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
)
{
        int * of = chamtype[chamber_type_].OSfield;

        *empty = OS->split(of[10]);

        *hfa   = OS->split(of[9]);
        *h     = OS->split(of[8]);
        *hn    = OS->split(of[7]);
        *hpatb = OS->split(of[6]);
                        
        *lfa   = OS->split(of[5]);
        *l     = OS->split(of[4]);
        *ln    = OS->split(of[3]);
        *lpatb = OS->split(of[2]);
        
        *daq   = OS->split(of[1]);
        *bxn   = OS->split(of[0]);
}

ALCTController::ALCTSTATUS ALCTController::alct_fast_self_test (long* code, unsigned long verbose) {

  unsigned r, ParamReg;
  ALCTSTATUS st = EALCT_SUCCESS;
  unsigned crw[] = {0x80fc5fc0, 0x20a03786, 0x8}; // default values for CR
  unsigned oldcr[3], crr[3];
  unsigned long oldHCmask[22];
        
  alct_fast_set_jtag_channel(ALCT_FAST_JTAG_CHANNEL);
  r = 0x11;
  if (verbose) printf ("Chamber type: %d\n", chamber_type_);

  // just check jtag connection first
  if (WriteRegister (ParamRegWrite, &r) == -1) {
    if (verbose) printf ("JTAG_B driver detected a problem\n");
    st = EALCT_PORT; 
    return st;
  }
  ReadRegister (ParamRegRead, &ParamReg);

  if (ParamReg != r) {
    st = EALCT_TESTFAIL;
    if (code != NULL) *code = ALCT_FAST_ERROR_READBACK;
    if (verbose) printf ("written = %x read = %x\n", r, ParamReg);          
    return st;
  }
  r = 0x1fd;

  WriteRegister (ParamRegWrite, &r);
  ReadRegister (ParamRegRead, &ParamReg);
  if ((ParamReg & 0x1f) != (r & 0x1f)) {
    st = EALCT_TESTFAIL;
    if (code != NULL) *code = ALCT_FAST_ERROR_READBACK;
    if (verbose) printf ("written = %x read = %x\n", r, ParamReg);          
    return st;
  }
        
  // read all registers which are going to be modified during the test
  ReadRegister (RdCfg, oldcr); // read old configuration first
  ReadRegister (HCMaskRead, (unsigned*)oldHCmask);
        
  if (verbose) printf("Configuration register:    %08x %08x %08x\n", crw[2], crw[1], crw[0]);
  WriteRegister (WrCfg, crw); // write test configuration
  ReadRegister (RdCfg, crr); // read back and compare
  if (crw[0] != crr[0] || crw[1] != crr[1] || crw[2] != crr[2]) {
    st = EALCT_TESTFAIL;
    if (code != NULL) *code = ALCT_FAST_ERROR_READBACK;
    if (verbose) printf("Configuration register mismatch:    %08x %08x %08x\n", crr[2], crr[1], crr[0]);
    return st;
  }

  /* // remove slow part of setup //Mohr (3/16/03)        
  st = alct_fast_CheckShiftingPatternViaJTAG(verbose);
  if (st != EALCT_SUCCESS) if (code != NULL) *code = ALCT_FAS_TERROR_PATTERN;
  */
  // restore all registers
  WriteRegister (WrCfg, oldcr); // write the original configuration
  WriteRegister (HCMaskRead, (unsigned*)oldHCmask);
  return st;
}

void ALCTController::SetConf(  unsigned cr[3], int verbose=0 ){
   //
   unsigned crr[3];
   //
   WriteRegister (WrCfg, cr); // write configuration first
   if (verbose) printf("Set to Configuration register:        %08x %08x %08x\n", cr[2], cr[1], cr[0]);
   //
   ReadRegister (RdCfg, crr);
   if (verbose) printf("Read back Configuration register:     %08x %08x %08x\n", crr[2], crr[1], crr[0]);
   if (cr[0] != crr[0] || cr[1] != crr[1] || cr[2] != crr[2])
   {
      if (verbose) printf("Configuration register mismatch:    %08x %08x %08x\n", crr[2], crr[1], crr[0]);
   }
//   
}

int ALCTController::GetWGNumber(){
  //
  return chamtype[chamber_type_].WG_number;
}


void ALCTController::GetConf(  unsigned cr[3], int verbose=0 ){
   //
   ReadRegister (RdCfg, cr); // read configuration first
   if (verbose) printf("Configuration register:    %08x %08x %08x\n", cr[2], cr[1], cr[0]);
   //
}

void ALCTController::alct_read_hcmask(unsigned long HCmask[22]){
   //
   alct_fast_set_jtag_channel(ALCT_FAST_JTAG_CHANNEL);
   ReadRegister  (HCMaskRead, (unsigned*)HCmask);
   usleep(100);
   WriteRegister (HCMaskWrite, (unsigned*)HCmask);
   usleep(100);
   //
}

void ALCTController::alct_write_hcmask(unsigned long HCmask[22]){
   //
   alct_fast_set_jtag_channel(ALCT_FAST_JTAG_CHANNEL);
   WriteRegister (HCMaskWrite, (unsigned*)HCmask);
   usleep(100);
   //
}


ALCTController::ALCTSTATUS ALCTController::alct_fire_test_pulse(char pulse)
{
    ALCTSTATUS st = EALCT_SUCCESS;
    unsigned trval;

    trval = 0x10;                   // !internal
    if (pulse == 'i') trval = 0;    //  internal
    if (pulse == 'a') trval = 0x8;  //  asynch
    if (pulse == 's') trval = 0x4;  //  sync
    if (pulse == 'e') trval = 0xc;  //  external
    if (pulse == 'I') trval = 0x10; // !internal
    if (pulse == 'A') trval = 0x18; // !asynch
    if (pulse == 'S') trval = 0x14; // !sync
    if (pulse == 'E') trval = 0x1c; // !external
    alct_fast_set_jtag_channel(ALCT_FAST_JTAG_CHANNEL);
    if (WriteRegister(WrTrig, &trval) == -1) st = EALCT_PORT;
    printf("test pulse multiplexor = %x\n", trval);
    return st;
}


ALCTController::ALCTSTATUS ALCTController::alct_download_patterns(const char* filename, unsigned long verbose)
{
  int pos ;
  int line ;
  int counter ;
  FILE* patternfile ;
  int zones = chamtype[chamber_type_].WG_number/6/8; 
                   // number of zones with independently programmable patterns
  //int zones = 8;
  unsigned CollMaskData[14] ;
  unsigned CollMaskDataSplit[14] ;
  int map [] = { 2 ,1, 0, 4, 3, 5, 7, 6 ,10, 9, 8, 13, 12, 11, 16, 15, 14,
                 18, 17, 19, 21, 20, 24, 23, 22, 27, 26, 25 } ;
  int i;
        
  int j ;
  ALCTSTATUS st = EALCT_SUCCESS;
  elong el;
  el.clear();
        
  alct_fast_set_jtag_channel(ALCT_FAST_JTAG_CHANNEL);
  for (i = 0; i < 14; i++) {
    CollMaskData[i] = 0 ;
    CollMaskDataSplit[i] = 0xffffffff ;
  }
  counter = 0;
  patternfile = fopen(filename,"r") ;
  if ( patternfile != NULL ) {
    if (verbose) printf ("File found: %s\n",filename) ;
    while ( (line = fgetc(patternfile))!=EOF ) {
      if (verbose) printf ( "%c ",line );
      if (line == '|') {
	counter ++ ;
      }
      if (line == '.') {
	pos = counter/28 ;
	if (verbose) {
	  printf(" Positon %d ",pos);
	  printf(" Counter %d ",counter%28);
	}
	CollMaskDataSplit[pos] &= ~((unsigned)1<<map[counter%28])   ;
	counter ++ ;
      }

    }
    if (verbose) {       
      printf("\n") ;
      for (j = 0; j < zones; j++) {
	printf ( " CollMaskDataSplit[%d] ",j ) ;
	for (i = 0; i < 32; i++) {
	  if ( CollMaskDataSplit[j] & (1 << (31-i) ) ) {
	    printf ( "1" ) ;
	  } else {
	    printf ( "0" ) ;
	  }
	}
	printf(" %x \n",CollMaskDataSplit[j] ) ;
      }
      printf("\n") ;
    }
                
    for (i = 0; i < zones; i++) {
      el.cat(CollMaskDataSplit[i], 28);
    }
    elWriteRegister(CollMaskWrite, &el) ;
    ReadRegister(CollMaskRead, CollMaskData) ;
    for ( i = 0; i < zones; i++) {
      if (verbose) printf ( " Read CollMask %d = 0x%x \n",i,CollMaskData[i] ) ;
    }
    if (WriteRegister(CollMaskWrite, CollMaskData) == -1) st = EALCT_PORT;
  } else {
    if (verbose) printf ("File %s not found.\n", filename);
    st = EALCT_FILEOPEN;
  }
  return st;
}

ALCTController::ALCTSTATUS ALCTController::alct_download_hot_mask (const char* filename, unsigned long verbose)
{
//      unsigned HCmask[13], HCmaskCheck[13];
        unsigned char HCmask[6][14];
        int i, j;
        char line[1000], dummy[1000];
        FILE* hcmaskfile;
        ALCTSTATUS st = EALCT_SUCCESS;
        int block = -1;
        int totblk = chamtype[chamber_type_].WG_number/6/8;
        char commchar;
        int ly;
        elong el, elc;
        el.clear();
        el.clear();

        alct_fast_set_jtag_channel(ALCT_FAST_JTAG_CHANNEL);
        
        for (i = 0; i < 6; i++)
	  for (j = 0; j < 14; j++)
	    HCmask[i][j] = 0xff;
	//
        hcmaskfile = fopen(filename,"r") ;
	//
        if ( hcmaskfile != NULL )
	  {
	    if (verbose) printf("File : %s", filename);
	    i = 0;
	    while (!feof(hcmaskfile))
	      {
		fscanf (hcmaskfile, "%[^\n\r]", line);
		fscanf (hcmaskfile, "%[\n\r]", dummy);
		if (verbose) printf ("%s\n", line);
		sscanf (line, "%c", &commchar);
		if (commchar != '#')
		  {
		    if (strstr (line, "block") != NULL)
		      {
			block = -1;
			if (sscanf (line, "%s %d", dummy, &block) == 2)
			  {
			    if (block > totblk || block < 1)
			      {
				block = -1;
			      }
			    ly = 0;
			    if (verbose) printf ("block = %d\n", block);
			  }
			else block = -1;
		      }
		    else
		      {
			if (block != -1)        
		{
		  j = 0;
		  for (i = 7; i >= 0; i--)
		    {
		      while (line[j] != '1' && line[j] != '0' && line[j] != 0) j++;
		      if (line[j] == 0) break;
		      if (line[j] == '0')
			{
			  HCmask[ly][block-1] &= ~((unsigned char)(1 << i));
		      }
		      j++;
		    }
		  if (i < 0) ly++;
		}
		      }
		  }
	      }
	  }
	else
	  {
	    if (verbose) printf ("Cannot open file: %s\n", filename);
	    st = EALCT_FILEOPEN;
	  }
        if (verbose) printf("\n") ;
        if (verbose)
	  {
	    for (i = 0; i < 6; i++)
	      {
		for (j = totblk-1; j >= 0; j--)
		  printf("%02x ", HCmask[i][j]);
		printf("\n");
	      }
	  }
        
        for (i = 0; i < 6; i++)
	  {
	   for (j = 0; j < totblk; j++)
	     el.cat((unsigned long)HCmask[i][j], 8);
        }
        
        if (verbose)
	  {
	    printf("el.l: %d\n", el.l);
	    for (i = 0; i < el.l/32+1; i++) printf("%08lx\n", el.r[i]);
	  }
        
        if (elWriteRegister(HCMaskWrite, &el) == -1) st = EALCT_PORT;
        elReadRegister(HCMaskRead, &elc);
        elWriteRegister(HCMaskWrite, &el);
        for (i = 0; i < el.l/32; i++)
	{
	  if (el.r[i] != elc.r[i])
	    {
	      if (verbose) printf("Mismatch: i = %02d HCwr = %08lx HCrd = %08lx\n", i, el.r[i], elc.r[i]);
	      st = EALCT_TESTFAIL;
	    }
	}
	
        return st;
}


void ALCTController::setCRfld(alct_params_type* p) {
  if(p != NULL)
  {
    crParams_[0]  = (char*)&(alct_trig_mode_);
    crParams_[1]  = (char*)&(alct_ext_trig_en_);
    crParams_[2]  = (char*)&(alct_send_empty_);
    crParams_[3]  = (char*)&(alct_inject_);
    crParams_[4]  = (char*)&(alct_bxc_offset_);
    crParams_[5]  = (char*)&(alct_nph_thresh_);
    crParams_[6]  = (char*)&(alct_nph_pattern_);
    crParams_[7]  = (char*)&(alct_drift_delay_);
    crParams_[8]  = (char*)&(alct_fifo_tbins_);
    crParams_[9]  = (char*)&(alct_fifo_pretrig_);
    crParams_[10] = (char*)&(alct_fifo_mode_);
    crParams_[11] = (char*)&(p->fifo_last_feb);
    crParams_[12] = (char*)&(alct_l1a_delay_);
    crParams_[13] = (char*)&(alct_l1a_window_);
    crParams_[14] = (char*)&(alct_l1a_offset_);
    crParams_[15] = (char*)&(alct_l1a_internal_);
    crParams_[16] = (char*)NULL;
    crParams_[17] = (char*)NULL;
    crParams_[18] = (char*)&(alct_ccb_enable_);
    crParams_[19] = (char*)NULL;
    crParams_[20] = (char*)NULL;
    crParams_[21] = (char*)NULL;
    crParams_[22] = (char*)NULL;
    crParams_[23] = (char*)NULL;
    crParams_[24] = (char*)NULL;
  }
  else
  {
    for (unsigned int i = 0; i < sizeof (CRfld) / sizeof (struct Rfield); i++) crParams_[i] = NULL;
  }
}


void ALCTController::packControlRegister(unsigned * cr) const {
  //*crParams_[9] = 8;
  for (unsigned int i = 0; i < sizeof (CRfld) / sizeof (struct Rfield); i++)
  {
    if(crParams_[i] != NULL) {
      int wordn = (CRfld[i].bit >> 5) & 3; // number of word in cr to be modified
      int bitn  = (CRfld[i].bit) & 0x1f;   // number of bit in this word the parameter starts from
      int mask  =  CRfld[i].mask << bitn;  // mask to mask out the parameter
      cr[wordn] &= (~mask);            // zero the parameter
      cr[wordn] |= ((*(crParams_[i]) << bitn) & mask); // put the new value in place
    // warning:  fifo_mode is on the edge of the two words. Only values 0 and 1 will work (and only these values are currently implemented)
    }
  }
}


void ALCTController::unpackControlRegister(unsigned * cr) {
  // fills the data fields pointed at by crParams_ with the values packed in cr
  bool verbose = false;
  for (unsigned int i = 0; i < sizeof (CRfld) / sizeof (struct Rfield); i++)
  {
     int wordn = (CRfld[i].bit >> 5) & 3; // number of word in cr
     int bitn  = (CRfld[i].bit) & 0x1f;   // number of bit in this word the parameter starts from
     int mask  =  CRfld[i].mask;  // mask to mask out the parameter
     if (crParams_[i] != NULL)
     {
        *(crParams_[i]) = (cr[wordn] >> bitn) & mask;
     }
     printf ("%s \t %d\n", CRfld[i].name, ((cr[wordn] >> bitn) & mask));
  }
}


void ALCTController::setConfig() {
  //
  ALCTSTATUS st  = EALCT_SUCCESS; 
  bool verbose = true;
  unsigned cr[3]  = {0x80fc5fc0, 0x20a03786, 0x8}; // default values for CR
  unsigned crr[] = {0,0,0};
  //
  packControlRegister(cr);
  //
  printf("fifo_pretrig %d \n",alct_fifo_pretrig_);
    //
  if (verbose) printf("3.Configuration register's new value: %08x %08x %08x\n", cr[2], cr[1], cr[0]);
  if (WriteRegister (WrCfg, cr) == -1)
    {
      printf ("JTAG_B driver detected a problem\n");
      st = EALCT_PORT;
    }
  else
    {
      ReadRegister (RdCfg, crr);
      if (cr[0] != crr[0] || cr[1] != crr[1] || cr[2] != crr[2])
        {
	  st = EALCT_TESTFAIL;
	  printf("Configuration register mismatch:      %08x %08x %08x\n", crr[2], crr[1], crr[0]);
        }
    }
  
  // enable real input from the AFEBs
  unsigned r = 0x1fd;
  WriteRegister (ParamRegWrite, &r);
  
  // Set YRIclkEn (see Verilog code)
  unsigned YR = 0;
  WriteRegister (YRwrite, &YR);
  // enable input.
  WriteIR(InputEnable);
}


ALCTController::ALCTSTATUS ALCTController::alct_download_fast_config
(
    char * filename,
    alct_params_type* params,
    unsigned long verbose
)
{
    FILE *in;
    char parname[20];
    unsigned parval, mask;
    int i, wordn, bitn;
    unsigned cr[] = {0x80fc5fc0, 0x20a03786, 0x8}; // default values for CR
    unsigned crr[] = {0,0,0};
    char line[1000], dline[20], commentch;
    int state = 0; // idle
    ALCTSTATUS st = EALCT_FILEDEFECT;
    unsigned r, YR;
    char tpsrc[] = "IASEiase";
        
    alct_fast_set_jtag_channel(ALCT_FAST_JTAG_CHANNEL);

    in = fopen (filename, "rt");
    if (in != NULL)
    {
        while (!feof(in) && state < 2)
        {
            parname[0] = 0;
            fscanf (in, "%[^\n\r]", line);
            fscanf (in, "%[\n\r]", dline);
            sscanf (line, "%c", &commentch);
            if (commentch != '#')
            {
                if (strstr(line, "<alct_fast>") != NULL) {state = 1; st = EALCT_SUCCESS;}; // begin scan
                if (strstr(line, "</alct_fast>") != NULL) state = 2; // end scan
                if (sscanf (line, "%s %d", parname, &parval) == 2 && state == 1)
                {
                    for (unsigned int i = 0; i < sizeof (CRfld) / sizeof (struct Rfield); i++)
                    {
                        if (strcmp (parname, CRfld[i].name) == 0) // search for the appropriate record in the fields of CR
                        {
                            if (verbose) printf ("%s \t %d\n", parname, parval);
                            if (crParams_[i] != NULL) *(crParams_[i]) = parval; // update record in the parameter structure
                            wordn = (CRfld[i].bit >> 5) & 3; // number of word in cr to be modified
                            bitn  = (CRfld[i].bit) & 0x1f;   // number of bit in this word the parameter starts from
                            mask  =  CRfld[i].mask << bitn;  // mask to mask out the parameter
                            cr[wordn] &= (~mask);            // zero the parameter
                            cr[wordn] |= ((parval << bitn) & mask); // put the new value in place
                            // warning:  fifo_mode is on the edge of the two words. Only values 0 and 1 will work (and only these values are currently implemented)
                        }
                    }
                    if (strcmp (parname, "tpsource") == 0)
                    {
                        // special case of tpsource
                        // the call below actually just sets the test signal multiplexor
                        if (verbose) printf ("%s \t= %d\n", parname, parval);
                        alct_fire_test_pulse(tpsrc[parval & 7]);
                    }
                }
            }
        }
    }
    else
    {
        st = EALCT_FILEOPEN;
        if (verbose) printf ("Cannot open input file: %s\n", filename);
    }
    if (in != NULL) fclose(in);
    if (verbose) printf(    "2.Configuration register's new value: %08x %08x %08x\n", cr[2], cr[1], cr[0]);
    if (WriteRegister (WrCfg, cr) == -1)
    {
        if (verbose) printf ("JTAG_B driver detected a problem\n");
        st = EALCT_PORT; 
    }
    else
    {
        ReadRegister (RdCfg, crr);
        if (cr[0] != crr[0] || cr[1] != crr[1] || cr[2] != crr[2])
        {
            st = EALCT_TESTFAIL;
            if (verbose) printf("Configuration register mismatch:    %08x %08x %08x\n", crr[2], crr[1], crr[0]);
        }
    }
        
    // enable real input from the AFEBs
    r = 0x1fd;
    WriteRegister (ParamRegWrite, &r);
                        
    // Set YRIclkEn (see Verilog code)
    YR = 0;
    WriteRegister (YRwrite, &YR);
    // enable input.
    WriteIR(InputEnable);
        
        
    return st;
}

ALCTController::ALCTSTATUS ALCTController::alct_read_fast_config
(
        alct_params_type* params,
        unsigned long verbose
)
{
   unsigned cr[3]  = {0x80fc5fc0, 0x20a03786, 0x8}; // default values for CR
   ALCTSTATUS st = EALCT_SUCCESS;
        
   alct_fast_set_jtag_channel(ALCT_FAST_JTAG_CHANNEL);
   if (ReadRegister (RdCfg, cr) == -1)
   {
      if (verbose) printf ("JTAG_B driver detected a problem\n");
      st = EALCT_PORT; 
   }
   else
   {
      unpackControlRegister(cr);
   }
   return st;
}

ALCTController::ALCTSTATUS ALCTController::alct_read_delay_destructively
(
        long * delays,
        unsigned long verbose
)
{
        unsigned pattern [42];
        int i;
        ALCTSTATUS st = EALCT_SUCCESS;
        alct_fast_set_jtag_channel(ALCT_FAST_JTAG_CHANNEL);
        if (verbose) printf ("Chamber type: %d, WGs: %d, delay lines: %d, delayChains: %d\n", chamber_type_, chamtype[chamber_type_].WG_number, delayLines(), delayChains());
                
                for (i = 0; i < delayChains(); i++)
                {
                        if (Read6DelayLines((unsigned*)&delays[i*6], pattern, (1 << i)) == -1)
                        {
                                st = EALCT_PORT;
                        }
                }
                if (verbose) for (i = 0; i < delayLines(); i++) printf ("delay line: %02d, delay: %02ld\n", i+1, delays[i]);
        return st;
}


int ALCTController::delayLines() const {
  return chamtype[chamber_type_].WG_number / 16;
}


int ALCTController::delayChains() const {
  return delayLines()/6;
}


ALCTController::ALCTSTATUS ALCTController::alct_download_delay
(
        char * filename,
        long * delays,
        unsigned long verbose
)
{
        unsigned del;
        int i, n;
        FILE * in;      
        ALCTSTATUS st = EALCT_SUCCESS;
        alct_fast_set_jtag_channel(ALCT_FAST_JTAG_CHANNEL);
        if (verbose) printf ("Chamber type: %d, WGs: %d, delay lines: %d, delayChains(): %d\n", chamber_type_, chamtype[chamber_type_].WG_number, delayLines(), delayChains());
        in = fopen (filename, "rt");
        if (in != NULL)
        {
                while (!feof(in))
                {
                        fscanf(in, "%d %d", &n, &del);
                        if (verbose) printf("n = %d, del = %d\n", n, del);
                        if (n > delayLines() || n < 1) st = EALCT_ARG;
                        else
                        {
                                delays_[n-1] = del;
                        }
                }
                st = setDelays();
        }
        else
        {
                st = EALCT_FILEOPEN;
                if (verbose) printf ("Cannot open input file: %s\n", filename);
        }
        if (in != NULL) fclose (in);
        return st;
}


ALCTController::ALCTSTATUS ALCTController::setDelays() 
{
  ALCTSTATUS st = EALCT_SUCCESS;
  unsigned pattern [6] = {1,2,3,0x8000,0x4000,0x2000};
  unsigned int r = 0x1fd;
  //
  for (int i = 0; i < delayChains(); i++)
  {
    if (Write6DelayLines(&delays_[i*6], pattern, (1 << i)) == -1)
    {
      st = EALCT_PORT;
    }
    // enum put into int so it can be passed to WriteRegister
    unsigned int ParamRegWriteInt = ParamRegWrite;
    WriteRegister (ParamRegWriteInt, &r); 
  }
  delays_inited_ = true;
  //
  for (int i = 0; i < delayLines(); i++) {
    printf ("delay line: %02d, delay: %02d\n", i+1, delays_[i]);
  }
  return st;
}
   

// this routine sets the delays for one particular delay line.
// you must first call at least once alct_download_delay routine before using this one, 
// or else it will return error code 5
// if delay_line_number == -1 it sets all delay lines to "delay".
ALCTController::ALCTSTATUS ALCTController::alct_set_delay(long delay_line_number, long delay)
{
    unsigned pattern [6] = {0,0,0,0,0,0};
    unsigned int r = 0x1fd;
    ALCTSTATUS st = EALCT_SUCCESS;
    if (delays_inited_ && delay_line_number >= 0) return st = EALCT_ARG;
    
    if (delay_line_number < 0)
      {
        for (int i = 0; i < delayLines(); i++)
	  delays_[i] = delay;
      }
    else
      delays_[delay_line_number] = delay;
    //
    alct_fast_set_jtag_channel(ALCT_FAST_JTAG_CHANNEL);
    //
    for (int i = 0; i < delayChains(); i++)
      {
        if (Write6DelayLines(&delays_[i*6], pattern, (1 << i)) == -1)
	  {
            st = EALCT_PORT;
	  }
	int ParamRegWriteInt = ParamRegWrite; // enum put into int so it can be passed to WriteRegister
        WriteRegister (ParamRegWriteInt, &r);
      }
    return st;
}

ALCTController::ALCTSTATUS ALCTController::alct_change_fast_parameter
(
        char* parname,
        unsigned long* parval,
        unsigned long verbose
)

{
        unsigned cr[3];
        unsigned crr[] = {0,0,0};
        ALCTSTATUS st = EALCT_TESTFAIL;
        unsigned mask;
        int i, wordn, bitn;
        alct_fast_set_jtag_channel(ALCT_FAST_JTAG_CHANNEL);
        ReadRegister (RdCfg, cr);
        
        for (i = 0; i < sizeof (CRfld) / sizeof (struct Rfield); i++)
        {
                if (strcmp (parname, CRfld[i].name) == 0) // search for the appropriate record in the fields of CR
                {
                        st = EALCT_SUCCESS;
                        if (verbose) printf ("%s \t= %ld\n", parname, *parval);
                        wordn = (CRfld[i].bit >> 5) & 3; // number of word in cr to be modified
                        bitn  = (CRfld[i].bit) & 0x1f;   // number of bit in this word the parameter starts from
                        mask  =  CRfld[i].mask << bitn;  // mask to mask out the parameter
                        cr[wordn] &= (~mask);            // zero the parameter
                        cr[wordn] |= (((*parval) << bitn) & mask); // put the new value in place
                }
        }
        if (st != 0)
        {
                if (verbose) printf ("Unknown parameter name: %s\n", parname);
        }
        else
        {
        
                if (verbose) printf(    "1.Configuration register's new value: %08x %08x %08x\n", cr[2], cr[1], cr[0]);
                if (WriteRegister (WrCfg, cr) == -1)
                {
                        if (verbose) printf ("JTAG_B driver detected a problem\n");
                        st = EALCT_PORT; 
                }
                else
                {       
                        ReadRegister (RdCfg, crr);
                        if (cr[0] != crr[0] || cr[1] != crr[1] || cr[2] != crr[2])
                        {
                                st = EALCT_TESTFAIL;
                                if (verbose) printf("Configuration register mismatch:    %08x %08x %08x\n", crr[2], crr[1], crr[0]);
                        }
                }
        }
        return st;
}

ALCTController::ALCTSTATUS ALCTController::alct_fast_disable_read()
{
  ALCTSTATUS st = EALCT_SUCCESS;
  
  alct_fast_set_jtag_channel(ALCT_FAST_JTAG_CHANNEL);
  
  // Clear YRIclkEn
  unsigned YR = 0;
  WriteRegister (YRwrite, &YR);
  // disable input
  WriteIR(InputDisable);
  
  return st;
}

ALCTController::ALCTSTATUS ALCTController::alct_fast_enable_read()
{
  ALCTSTATUS st = EALCT_SUCCESS;
  unsigned YR;
  
  alct_fast_set_jtag_channel(ALCT_FAST_JTAG_CHANNEL);

  YR = 0x1fd;
  WriteRegister (ParamRegWrite, &YR);

  // Set YRIclkEn
  YR = 1;
  WriteRegister (YRwrite, &YR);
  // enable input
  WriteIR(InputEnable);
  
  return st;
}

ALCTController::ALCTSTATUS ALCTController::alct_fast_read_id(ALCTIDRegister & id)
{
  ALCTSTATUS st = EALCT_SUCCESS;
  unsigned long ID[2];
  alct_fast_set_jtag_channel(ALCT_FAST_JTAG_CHANNEL);
  int stInt = ReadRegister(IDRead, (unsigned *)ID);
  //
  // switch statement to change int value returned by ReadRegister into an enum for ALCTSTATUS
  switch (stInt)
    {
    case 0:
      st = EALCT_SUCCESS;
      break;
    case 1:
      st = EALCT_FILEOPEN;
      break;
    case 2:
      st = EALCT_FILEDEFECT;
      break;
    case 3:
      st = EALCT_PORT;
      break;
    case 4:
      st = EALCT_CHAMBER;
      break;
    case 5:
      st = EALCT_ARG;
      break;
    case 6:
      st = EALCT_TESTFAIL;
      break;
    case 7:
      st = EALCT_FIFONOTMT;
      break;
    }
  //
  id.unpack_fast_id(ID);
  //    
  return st;
}

ALCTController::ALCTSTATUS ALCTController::alct_write_test_pulse_reg
(
        unsigned val
)
{
        ALCTSTATUS st = EALCT_SUCCESS;
        unsigned trval = val << 2;
        alct_fast_set_jtag_channel(ALCT_FAST_JTAG_CHANNEL);
        if (WriteRegister(WrTrig, &trval) == -1) st = EALCT_PORT;
        return st;
}

ALCTController::ALCTSTATUS ALCTController::alct_read_serial_number
(
        unsigned long * alct_sn,
        unsigned long * mc_sn
)
{
        int i;
        unsigned long SNbit, j;
        ALCTSTATUS st = EALCT_SUCCESS;
        elong el;

        alct_fast_set_jtag_channel(ALCT_FAST_JTAG_CHANNEL);

        for (j = 0; j < 2; j++)
        {
                st = alct_change_fast_parameter("sn_select", &j, 0);
                el.clear ();
                // reset DS2401
                WriteIR (SNreset); usleep (1000);
                WriteIR (SNwrite1); usleep (50000);
        
                // write read command 0x33
                WriteIR (SNwrite1); usleep (1000);
                WriteIR (SNwrite1); usleep (1000);
                WriteIR (SNwrite0); usleep (1000);
                WriteIR (SNwrite0); usleep (1000);
                WriteIR (SNwrite1); usleep (1000);
                WriteIR (SNwrite1); usleep (1000);
                WriteIR (SNwrite0); usleep (1000);
                WriteIR (SNwrite0); usleep (1000);
        
                // read 64 bits of SN bit by bit
                for (i = 0; i < 64; i++)
		  {
		    ReadRegister (SNread, (unsigned*)&SNbit);
		    el.cat(SNbit, 1);
		    usleep (1000);
                }
		
                if (j == 0)
		  {
		    alct_sn[0] = el.r[0];
		    alct_sn[1] = el.r[1];
		  }
                else
		  {
		    mc_sn[0] = el.r[0];
                        mc_sn[1] = el.r[1];
		  }
        }
        return st;	
}

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
void ALCTController::PrepareDelayLinePatterns()
{
        int lct;
//      for (lct = 0; lct < 4; lct++)
        for (lct = 0; lct < delayChains(); lct++)
        {
                pattern[lct][0]  =    FlipByte(image[1][lct*2]);
                pattern[lct][0] |= (((unsigned)image[0][lct*2]) << 8);

                pattern[lct][1]  =    FlipByte(image[3][lct*2]);
                pattern[lct][1] |= (((unsigned)image[2][lct*2]) << 8);

                pattern[lct][2]  =    FlipByte(image[5][lct*2]);
                pattern[lct][2] |= (((unsigned)image[4][lct*2]) << 8);

                pattern[lct][3]  =    FlipByte(image[1][lct*2+1]);
                pattern[lct][3] |= (((unsigned)image[0][lct*2+1]) << 8);

                pattern[lct][4]  =    FlipByte(image[3][lct*2+1]);
                pattern[lct][4] |= (((unsigned)image[2][lct*2+1]) << 8);

                pattern[lct][5]  =    FlipByte(image[5][lct*2+1]);
                pattern[lct][5] |= (((unsigned)image[4][lct*2+1]) << 8);

        }
}

// end from alct_fast_lib


// from alct_slow_lib

typedef struct
  {
  char* key;
  char* val;
  } Param;


ALCTController::ALCTSTATUS ALCTController::alct_read_slow_config(int * jtag , alct_params_type* p)
{
  int i, res;
  int j = *jtag;
  long k;

  if (WRONG_PORT(chamber_type_))
    return EALCT_PORT;

  res = alct_read_slowcontrol_id(&p->sc_idreg);
  res = alct_read_test_pulse_groupmask(&j, (long*)&p->test_pulse_group_mask);
  res = alct_read_test_pulse_stripmask(&j, (long*)&p->test_pulse_strip_mask);
  res = alct_read_test_pulse_powerup(&j, (long*)&p->test_pulse_power_up);
  res = alct_read_temp(&j, &p->temperatur);
  res = alct_read_volt(&j, V18, &p->voltage_18);
  res = alct_read_current(&j, V18, &p->current_18);
  res = alct_read_volt(&j, V33, &p->voltage_33);
  res = alct_read_current(&j, V33, &p->current_33);
  res = alct_read_volt(&j, V55_1, &p->voltage_551);
  res = alct_read_current(&j, V55_1, &p->current_551);
  res = alct_read_volt(&j, V55_2, &p->voltage_552);
  res = alct_read_current(&j, V55_2, &p->current_552);

  for (i=0; i< alct_table[chamb_table[chamber_type_].alct].groups; i++)
    {
      res = alct_read_standby(&j, i, (long*)&p->standby_mask);
    }

  // === Read and Correct Thresholds Values for DAC by dividing ADC values by 4
  for (i=0; i< nAFEBs_; i++)
    {
      for (k=0; k< 2000000; k++) {};
      res = alct_read_thresh((long)i, (long*)thresholds_[i]);
      thresholds_[i] >>= 2;
    }
  return (ALCTSTATUS)res;
  // return EALCT_SUCCESS; 
}

// Load configuration parameters from specified file
ALCTController::ALCTSTATUS ALCTController::alct_download_slow_config(int* jtag, 
				     char* filename, 
				     alct_params_type* p,
				     unsigned long verbose)
  {
  FILE* fd;
  char* section;
  char buf[256];
  char key[20];
  char param[20];
  char val[20];
  int  indx,i;
  long  value;
  int  count = 0;
  int j = *jtag;
  int f_section = 0;
//  alct_params_type p;
  ALCTSTATUS res;	
//	const Param id_reg = {"id_reg","09072001B8"};
  const Param tp_pwr = {"tp_pwr","1"};
  const Param tp_dac = {"tp_dac","255"};
  const Param tp_group = {"tp_group","1111111"};
  const Param tp_strip = {"tp_strip","111111"};
  const Param afeb_standby = {"afeb_standby","111111"};
  
  if (WRONG_PORT(chamber_type_))
    return EALCT_PORT;
  
  if (verbose)
    printf("\n=== Loading Configuration\n");

  
  if ((fd = fopen(filename, "r")) == NULL  )
    {
    perror(filename);
    return EALCT_FILEOPEN;
    }

  if (verbose)
    printf("Reading Configuration from ALCT\n");
 
  alct_read_slow_config(jtag, p); 
  while (fgets(buf, 256, fd) != NULL)
    {
    trim(buf);
    if((buf[0]=='<') && (buf[strlen(buf)-1]=='>'))
      {
      f_section = 0;
      section = "<alct_slow>";
      if (!strcasecmp(buf, section)) 
	{
	if (!f_section)
	  {
	  if (verbose)
	    printf("%s Section is found. Processing...\n", strtolower(buf));
	  f_section = 1;
	  continue;
	  }
	}
      section = "</alct_slow>";
      if (!strcasecmp(buf, section))
	{
	f_section = 0;
	continue;
	}
      }
    
    if (f_section)
      {
      if (sscanf(buf,"%[^ =] %s",&key,&val)==2) 
	{
	if (verbose)
	  printf("Key: %-20s\tValue: %s\n", strtolower(key), val);
	strtolower(key);
	if (strstr(key, afeb_standby.key) != NULL)
	  {	
	  if (sscanf(key,"%[^0-9]%d", &param, &indx)==2)
	    {
	    if ( !strcasecmp(param, afeb_standby.key)
		 && (indx>=0)
		 && (indx<n_lct_chips_)
		 && sscanf(val,"%ld",&value) )
	      {
	      value = 0;
	      for (i=0; i<6; i++) if (val[i]=='0') value |= 0<<(5-i);
	      else value |= 1<<(5-i);
	      p->standby_mask[indx] = value;
	      if (verbose)
		printf("Set AFEB Standby Register #%d to 0x%x\n", 
		       indx, p->standby_mask[indx]);
	      res = alct_set_standby(&j, indx, value);
	      if ( res != 0)
		{
		if (verbose)
		  printf("Setting Standby register Error: %d\n", res);
		return res;
		}
	      count++;
	      }
	    }
	  }
	if (!strcasecmp(key, tp_pwr.key))
	  {
	  if (val[0]=='0') value = 0;
	  else value = 1;
	  p->test_pulse_power_up = value;
	  if (verbose)
	    printf("Set Test Pulse Power Up to %d\n", p->test_pulse_power_up);
	  res = alct_set_test_pulse_powerup(&j, value);
	  if ( res != 0)
	    {
	    if (verbose)
	      printf("Setting Test Pulse PowerUp Error: %d\n", res);
	    return res;
	    }
	  count++;
	  }
	if (!strcasecmp(key, tp_dac.key))
	  {
	  if (sscanf(val,"%ld", &value)&&(value>=0)&&(value<256))
	    {
	    p->test_pulse_amp = value;
	    if (verbose)
	      printf("Set Test Pulse DAC to %d\n",  p->test_pulse_amp);
	    
	    res = alct_set_test_pulse_amp(&j, value);
	    if ( res != 0)
	      {
	      if (verbose)
		printf("Setting Test Pulse Amplitude Error: %d\n", res);
	      return res;
	      }
	    
	    count++;
	    }
	  }
	if (!strcasecmp(key, tp_group.key))
	  {
	  value = 0;
	  for (i=0; i<7;i++) if (val[i]=='0') value |= 0<<(6-i);
	  else value |= 1<<(6-i);
	  p->test_pulse_group_mask = value;
	  if (verbose)
	    printf("Set Test Pulse Group Mask to 0x%x\n", 
		   p->test_pulse_group_mask);
	  res = alct_set_test_pulse_groupmask(&j, value);
	  if ( res != 0)
	    {
	    if (verbose)
	      printf("Setting Test Pulse Group Mask Error: %d\n", res);
	    return res;
	    }
	  count++;
	  }
	if (!strcasecmp(key, tp_strip.key))
	  {
	  value = 0;
	  for (i=0; i<6;i++) if (val[i]=='0') value |= 0<<(5-i);
	  else value |= 1<<(5-i);
	  p->test_pulse_strip_mask = value;
	  if (verbose)
	    printf("Set Test Pulse Strip Mask to 0x%x\n", 
		   p->test_pulse_strip_mask);
	  res = alct_set_test_pulse_stripmask(&j, value);
	  if ( res != 0)
	    {
	    if(verbose)
	      printf("Setting Test Pulse Strip Mask Error: %d\n", res);
	    return res;
	    }
	  count++;	
	  }
	}
      
      }
    
    }
  
  fclose(fd);
  
  return EALCT_SUCCESS;
}



ALCTController::ALCTSTATUS ALCTController::setThresholds() {
  // takes the thresholds for the AnodeChannels
  ALCTSTATUS res;
  for(int i = 0; i < delayLines(); ++i) {
    res = alct_set_thresh(i, thresholds_[i]);
  }
  //
  for (int i = 0; i < delayLines(); i++) {
    printf ("delay line: %02d, threshold: %02d\n", i+1, thresholds_[i]);
  }
  //
  return res; 
}


// Read ID Code of Slow Control Chip
// sc_id - ID Code structure
ALCTController::ALCTSTATUS ALCTController::alct_read_slowcontrol_id(ALCTIDRegister* sc_id)
  {
  int val[5];  

  do_jtag(8, 0, READ, idreg_first, idreg_length, val);
  sc_id->unpack_slow_id(val);
 
  return EALCT_SUCCESS;
  }

ALCTController::ALCTSTATUS ALCTController::alct_slowcontrol_id_status()
  {
  ALCTIDRegister sc_id;
  // Current Defined ID Code Value for Slow Control Chip  
  const ALCTIDRegister cur_sc_id = {8, 11, 7, 9, 0x2001};
  ALCTSTATUS res;
	
  res = alct_read_slowcontrol_id(&sc_id);
  if (res != 0)
    return res;

  if(sc_id != cur_sc_id)
    return EALCT_PORT;
	
  return EALCT_SUCCESS;
  }

// Set Threshold value of specified channel
ALCTController::ALCTSTATUS ALCTController::alct_set_thresh(long channel, long thresh)
  {
    //
    //printf("\n *** Setting to %d \n",thresh);
    //
    int val;
    int k, temp_bits, realchannel;
    long i;
    
    static int    first[] = { 0, 12 };
    static int    length = 12;
    
    if (WRONG_PORT(chamber_type_))
      return EALCT_PORT;
    
    if (WRONG_CHANNEL(chamber_type_, channel))
      return EALCT_ARG;
    
    if (WRONG_DAC_VALUE(thresh))
      return EALCT_ARG;
    
    realchannel = channel;
    
    // DAC Remapping for channels higher that 33
    if (channel >= 33) {realchannel += 3;}; 
    
    //printf(" Setting thresh %x channel %d \n",thresh&0xff,realchannel);
    
    temp_bits = (realchannel % 12) << 8 | (thresh & 0xff);
    val = 0;
    //printf(" temp %x \n",temp_bits);
    for (k = 0; k < 12; k++) if (temp_bits & (1 << (11 - k))) val |= 1 << k;
    //printf(" val  %x \n",val);
    do_jtag(8, 010 + realchannel / 12, WRITE, first, length, &val);
    //for (i=0; i<2000000; i++) {}; 
    
    return EALCT_SUCCESS;
  }

// Read Threshold value from specified channel
ALCTController::ALCTSTATUS ALCTController::alct_read_thresh(long channel, long* thresh)
{
  // the result will be maybe 6 or 7 counts higher than you expect, because of another threshold in the hardware
  alct_params_type p;
  int val[3] = {0};
  int k, temp_bits, data;
  long i;
  enum {MAX_AFEBS = 42};
  static int    adc_channel[MAX_AFEBS] = { 1, 0,
                10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
                10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 
		0, 1, 2, 3, 4, 5, 6, 7, 8 };
  static int    adc_chip[MAX_AFEBS] =    { 2, 2,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		3, 3, 3, 3, 3, 3, 3, 3, 3,
		4, 4, 4, 4, 4, 4, 4, 4, 4 };

  static int    first[] = { 0, 4, 5, 20 };
  static int    length = 11;
  
  if (WRONG_PORT(chamber_type_))
    return EALCT_PORT;
  
  if (WRONG_CHANNEL(chamber_type_, channel))
    return EALCT_ARG;

  //printf("Reading back....\n");


  temp_bits = adc_channel[channel];
  data = 0;
  for (k = 0; k < 4; k++) if (temp_bits & (1 << (3 - k))) data |= 1 << k;

  for (i=0; i<2000000; i++) {};
  val[0] = data;
  do_jtag2(8, 020 + adc_chip[channel], WRITE, first, length, val);

  for (i=0; i<2000000; i++) {};
  val[0] = data;
  do_jtag2(8, 020 + adc_chip[channel], WRITE, first, length, val);
  
  temp_bits = 0;
  //printf("val %x \n",val[2]);
  for (k = 0; k < 10; k++ ) if ( val[2] & (1<<k) ) temp_bits |= 1 << (9-k) ;
  *thresh = temp_bits ;

  //printf("ReadBack %d \n",*thresh);
	  
  return EALCT_SUCCESS;
  }

// Read Power Supply Voltages Values
// channel - Power Supply Channel (1.8, 3.3, 5.5 Volts)
ALCTController::ALCTSTATUS ALCTController::alct_read_volt(int* jtag,
			  PWR_SPLY channel,
							  float* volt)
  {
  int j = *jtag;
  long t;

  if (WRONG_PORT(chamber_type_))
    return EALCT_PORT; 
  
  if (WRONG_PWR_CHANNEL(channel))
    return EALCT_ARG; 

  alct_read_volt_adc(jtag, channel, &t);
  const float VOLT_CONVERS_COEF[4] = {0.005878, 0.005878, 0.005878, 0.005878};

  *volt = t*VOLT_CONVERS_COEF[channel];
  return EALCT_SUCCESS;
  }

// Read Currents Values
ALCTController::ALCTSTATUS ALCTController::alct_read_current(int* jtag,
			     PWR_SPLY channel,
							     float* current)
  {
  int j = *jtag;
  long t;

  if (WRONG_PORT(chamber_type_))
    return EALCT_PORT;  

  if(WRONG_PWR_CHANNEL(channel))
    return EALCT_ARG; 
  
  alct_read_current_adc(jtag, channel, &t);
  const float CUR_CONVERS_COEF = 0.002987;

  *current = t*CUR_CONVERS_COEF;
  return EALCT_SUCCESS;
  }

// Read row ADC value for onboard Temperature

// Read row ADC value for Voltage of Power Suply
ALCTController::ALCTSTATUS ALCTController::alct_read_volt_adc(int* jtag,
			      PWR_SPLY channel,
							      long* volt)
  {
  int j = *jtag;
  long i;
  int k, temp_bits, data;
  int val[3] = {0};

  static int    first[] = { 0, 4, 5, 20 };
  static int    length = 11;
                        
  if (WRONG_PORT(chamber_type_))
    return EALCT_PORT;
  
  if (WRONG_PWR_CHANNEL(channel))
    return EALCT_ARG; 
  
  temp_bits = 6 + channel;
  data = 0;
  for (k = 0; k < 4; k++) if (temp_bits & (1 << (3 - k))) data |= 1 << k;

  for (i=0; i<2000000; i++) {};
  val[0] = data;
  fd = j;
  do_jtag2(8, 020 + 2, WRITE, first, length, val);

  for (i=0; i<2000000; i++) {};
  val[0] = data;
  do_jtag2(8, 020 + 2, WRITE, first, length, val);

  temp_bits = 0;
  for (k = 0; k < 10; k++ ) if ( val[2] & (1<<k) ) temp_bits |= 1 << (9-k) ;
 
  *volt = temp_bits;
  
  return EALCT_SUCCESS;
  }

// Read row ADC value for Current of Power Supply
ALCTController::ALCTSTATUS ALCTController::alct_read_current_adc(int* jtag,
								 PWR_SPLY channel,
								 long* current)
  {
  int j = *jtag;
  long i;
  int k, temp_bits, data;
  int val[3] = {0};

  static int    first[] = { 0, 4, 5, 20 };
  static int    length = 11;
                        
  if (WRONG_PORT(chamber_type_))
    return EALCT_PORT;
  
  if (WRONG_PWR_CHANNEL(channel))
    return EALCT_ARG; 
  
  temp_bits = 2 + channel;
  data = 0;
  for (k = 0; k < 4; k++) if (temp_bits & (1 << (3 - k))) data |= 1 << k;

  for (i=0; i<2000000; i++) {};
  val[0] = data;
  j = fd;
  do_jtag2(8, 020 + 2, WRITE, first, length, val);

  for (i=0; i<2000000; i++) {};
  val[0] = data;
  do_jtag2(8, 020 + 2, WRITE, first, length, val);

  temp_bits = 0;
  for (k = 0; k < 10; k++ ) if ( val[2] & (1<<k) ) temp_bits |= 1 << (9-k) ;
  *current = temp_bits; 
  
  return EALCT_SUCCESS;
  }

// Perform Slow Control Self Test
// code - Test # when error occured
// code = 0 - Successeful completion of self-test
ALCTController::ALCTSTATUS ALCTController::alct_slow_self_test(int* jtag,
							       long* code)
  { 
  int i, k, h;
  long val, readval;
  float valf;
  int j = *jtag;
  ALCTSTATUS res;
  ALCTIDRegister sc_id;
  // Current Defined ID Code Value for Slow Control Chip  
  const ALCTIDRegister cur_sc_id = {8, 11, 7, 9, 0x2001};
  int offset[42][2];
  
  if (WRONG_PORT(chamber_type_))
    {
      *code = 2;
      return EALCT_PORT; 
    }

  // Checking Slow Control ID Register
  // ---------------------
  res = alct_read_slowcontrol_id(&sc_id);
  if (res != EALCT_SUCCESS)
    {
      *code = 2;
      return res;
    }
  else
    {
      if(sc_id != cur_sc_id) 
	{
	  *code = 2;
	  return EALCT_TESTFAIL;
	}
    }
  
  

  // Checking Thresholds
  // --------------------
  
  // Turn On AFEBs
  for (i=0; i< alct_table[chamb_table[chamber_type_].alct].groups; i++)
    { alct_set_standby(&j, i, 0x3f);}
  
  // Calculating Offsets for MinDAC(0) and MaxDAC(255) thresholds values
  for ( i=0; i< nAFEBs_; i++)
    {
      res = alct_set_thresh(i, 0);
      for (h=0; h < 2000000; h++) {}
      res = alct_read_thresh(i, &readval);
      offset[i][0] = readval;
      int THRESH_OFFSET_TOLER = 60;
      if (offset[i][0] > THRESH_OFFSET_TOLER)
        {
          *code = 3;
	  printf("OFFSET %d \n",offset[i][0]);
          return EALCT_ARG;
        } 
      res = alct_set_thresh(i, 0xff);
      for (h=0; h < 2000000; h++) {}  
      res = alct_read_thresh(i, &readval);
      offset[i][1] = readval;
      // printf("%d -[0] %d [1] %d\n", i, offset[i][0], offset[i][1] );
      if (offset[i][1]-offset[i][0] <= 0) // Return if MaxDAC Thresh <= MinDAC Thresh ADC values
	{
          *code = 31;
          return EALCT_ARG;
        }
    }
  for (k=0 ; k<6; k++)
    {
      for ( i=0; i< nAFEBs_; i++)
	{
	  val = 50*k;
	  res = alct_set_thresh(i, val);
	  if (res != EALCT_SUCCESS)
	    { 
	      *code = 32;
	      return res;
	    } 
	  
	  for (h=0; h < 2000000; h++) {}
	  readval = 0;
	  res = alct_read_thresh(i, &readval);
	  if (res != EALCT_SUCCESS)
	    {
	      *code = 33;
	      return res;
	    }
	  
	  // Comparing DAC and ADC using offset compensation formula
	  // printf ("Diff %d\n", val*4- (((readval - offset[i][0])*offset[i][1])/(offset[i][1]-offset[i][0])));
	  int THRESH_TOLER = 10;
	  if (fabs((double)val*4- ( ( (readval - offset[i][0]) * offset[i][1]) /(offset[i][1]-offset[i][0])) ) > THRESH_TOLER)
	    {
	      *code = 34;
	      std::cout << "Tolerance " << 
		fabs((double)val*4- ( ( (readval - offset[i][0]) * offset[i][1]) /(offset[i][1]-offset[i][0]))) << std::endl;
	      return EALCT_TESTFAIL;  
	    }
	}
    }
  
  // Checking Standby Register
  // -----------------------
  for (i=0; i< alct_table[chamb_table[chamber_type_].alct].groups; i++)
    {
      val = (0x30|((i&0xf)));
      res = alct_set_standby(&j, i, val);
      if (res != EALCT_SUCCESS)
      {
	*code = 4;
	return res;
      }
      res = alct_read_standby(&j, i, &readval);
      if (res != EALCT_SUCCESS)
	{
	  *code = 41;
	  return res;
	}
      if (readval != val)
	{
	*code = 42;
	return EALCT_TESTFAIL;
      }      
    }

 
    
  // Checking Test Pulse Power Down
  // -----------------------
  val =0;
  res = alct_set_test_pulse_powerup(&j, val);
  if (res != EALCT_SUCCESS)
    {
    *code = 5;
    return res;
    }
  readval = 0;
  res = alct_read_test_pulse_powerup(&j, &readval);
  if (res != EALCT_SUCCESS)
    {
    *code = 51;
    return res;
    } 
  
  if (readval != 0)
    {
    *code = 52;
    return EALCT_TESTFAIL;
    }
  
  // Checking Test Pulse Power Up
  // ------------------------
  val =1;
  res = alct_set_test_pulse_powerup(&j, val);
  if (res != EALCT_SUCCESS)
    {
    *code = 6;
    return res;
    }
  readval = 0;
  res = alct_read_test_pulse_powerup(&j, &readval);
  if (res != EALCT_SUCCESS)
    {
      *code = 61;
      return res;
    } 
  
  if (readval != 1)
    {
      *code = 62;
      std::cout  << "Readval " << readval << std::endl ;
      return EALCT_TESTFAIL;
    }
  
  // Checking Test Pulse Group Mask
  // ------------------------
  for ( i=0; i<128; i++)
    {
      val = i;
      res = alct_set_test_pulse_groupmask(&j, val);
      if (res != EALCT_SUCCESS)
	{
	  *code = 7;
	  return res;
	} 
      res = alct_read_test_pulse_groupmask(&j, &val);
      if (res != EALCT_SUCCESS)
	{
	  *code = 71;
	  return res;
	}
      if (val != i)
	{
	  *code = 72;
	  return EALCT_TESTFAIL;
	}
    }
  
  // Checking Test Pulse Strip Mask
  // ------------------------
  for ( i=0; i<64; i++)
    {
    val = i;
    res = alct_set_test_pulse_stripmask(&j, val);
    if (res != EALCT_SUCCESS)
      {
      *code = 8;
      return res;
      } 
    res = alct_read_test_pulse_stripmask(&j, &val);
    if (res != EALCT_SUCCESS)
      {
      *code = 81;
      return res;
      }
    if (val != i)
      {
	*code = 82;
	return EALCT_TESTFAIL;
      }
    }

  // Checking Voltages
  // ------------------------
  // Voltage 1.8V
  res = alct_read_volt(&j, V18, &valf);
  if (res != EALCT_SUCCESS)
    {
      *code = 9;
      return res;
    }
  if (fabs(valf - volt_table[V18][0]) > volt_table[V18][1])
    {
      *code = 91;
      return EALCT_TESTFAIL;
    }
  
  // Voltage 3.3V
  res = alct_read_volt(&j, V33, &valf);
  //printf("valf = %x \n",valf);
  if (res != EALCT_SUCCESS)
    {
    *code = 101;
    return res;
    }
  if (fabs(valf - volt_table[V33][0]) > volt_table[V33][1])  
    {
    *code = 102;
    return EALCT_TESTFAIL;
    }
  
  // Voltage 5.5V (1)
  res = alct_read_volt(&j, V55_1, &valf);
  //printf("valf = %x \n",valf);
  if (res != EALCT_SUCCESS)
    {
    *code = 11;
    return res;
    }
  if (fabs(valf - volt_table[V55_1][0]) > volt_table[V55_1][1])
    {
    *code = 111;
    //printf("voltagediff %f \n",fabs(valf - volt_table[V55_1][0]) > volt_table[V55_1][1]);
    return EALCT_TESTFAIL;
    }
  // Voltage 5.5V (2)
  res = alct_read_volt(&j, V55_2, &valf);
  if (res != EALCT_SUCCESS)
    {
    *code = 12;
    return res;
    }
  if (fabs(valf - volt_table[V55_2][0]) > volt_table[V55_2][1])
    {
    *code = 121;
    return EALCT_TESTFAIL;
    }

  // Checking Currents
  // ------------------------
  // Current for  1.8V
  res = alct_read_current(&j, V18, &valf);
  if (res != EALCT_SUCCESS)
    {
    *code = 13;
    return res;
    }
  if (fabs(valf - curr_table[V18][0]) > curr_table[V18][1])
    {
    *code = 131;
    return EALCT_TESTFAIL;
    }
  // Current for 3.3V
  res = alct_read_current(&j, V33, &valf);
  if (res != EALCT_SUCCESS)
    {
    *code = 14;
    return res;
    }
  if (fabs(valf - curr_table[V33][0]) > curr_table[V33][1])
    {
    *code = 141;
    return EALCT_TESTFAIL;
    }
  // Current for 5.5V (1)
  res = alct_read_current(&j, V55_1, &valf);
  if (res != EALCT_SUCCESS)
    {
    *code = 15;
    return res;
    }
  if (fabs(valf - curr_table[V55_1][0]) > curr_table[V55_1][1])
    {
    *code = 151;
    return EALCT_TESTFAIL;
    }

  // Current for 5.5V (2)
  valf = 0;
  res = alct_read_current(&j, V55_2, &valf);
  // printf("%.3f\n", valf);
  if (res != EALCT_SUCCESS)
    {
    *code = 16;
    return res;
    }
  if (fabs(valf - curr_table[V55_2][0]) > curr_table[V55_2][1])  
    {
    // printf("%.3f\n", valf);
    *code = 161;
    return EALCT_TESTFAIL;
    }

  // Checking OnBoard Temperature
  // ------------------------
  res = alct_read_temp(&j, &valf);
  if (res != EALCT_SUCCESS)
    {
    *code = 17;
    return res;
    }
  if (fabs(valf - temper[0]) > temper[1])  
    {
    *code = 171;
    return EALCT_TESTFAIL;
    }
  *code = 0;
  return EALCT_SUCCESS;
  }

// Set Standby Register value for specified wiregroup
ALCTController::ALCTSTATUS ALCTController::alct_set_standby(int* jtag,
			    int group,
							    long standby)
  { 
  int j = *jtag;
  int i;
  int val[NUM_OF_GROUPS];
  
  if (WRONG_PORT(chamber_type_))
    return EALCT_PORT; 
 
  if (WRONG_GROUP(chamber_type_, group))
    return EALCT_ARG;

  if (WRONG_STANDBY_MASK(chamber_type_, standby))
    return EALCT_ARG;
  
  fd = j;
  std::cout << "set standby " << group << " " << stdby_first << " " <<  stdby_length << std::endl;
  do_jtag(8, 045, READ, stdby_first, stdby_length, val);
  for (i=0;i<4;i++) printf (" %02x ",val[i] ) ;
  printf("\n") ;
  val[group] = standby;
  do_jtag(8, 044, WRITE, stdby_first, stdby_length, val);

  return EALCT_SUCCESS;
  }

// Read Standby Register value for specified wire group
ALCTController::ALCTSTATUS ALCTController::alct_read_standby(int* jtag,
			     int group,
							     long* standby)
  {
  int j = *jtag;
  int val[NUM_OF_GROUPS];
  
  if (WRONG_PORT(chamber_type_))
    return EALCT_PORT; 

  if (WRONG_GROUP(chamber_type_, group))
    return EALCT_ARG;
  
  fd = j;
  do_jtag(8, 045, READ, stdby_first, stdby_length, val);
  *standby = val[group];
  
  return EALCT_SUCCESS;
  }

// Set Test Pulse Power Amplitude
ALCTController::ALCTSTATUS ALCTController::alct_set_test_pulse_amp(int* jtag,
								   long test_pulse)
  {
  int j = *jtag;
  int val;
  int k;
  
  if (WRONG_PORT(chamber_type_))
    return EALCT_PORT; 

  if(WRONG_DAC_VALUE(test_pulse))
    return EALCT_ARG;
    
  /* For test pulse DAC, reverse the order of the bits */
  val = 0;
  for (k = 0; k < tpdac_length; k++)
    if (test_pulse & (1 << ((tpdac_length-1) - k))) val |= 1 << k;
 
  fd = j;
  do_jtag(8, 3, WRITE, tpdac_first, tpdac_length, &val);

  return EALCT_SUCCESS;	
  }

// Set Test Pulse Power Up/Down 
ALCTController::ALCTSTATUS ALCTController::alct_set_test_pulse_powerup(int* jtag,
								       long test_pulse_power_up)
  {  
    //printf("*** Set_test_pulse_powerup\n");
    
    int j = *jtag;
    int val = test_pulse_power_up;
    
    if (WRONG_PORT(chamber_type_))
    return EALCT_PORT; 
    
    fd = j;
    do_jtag(8, 046, WRITE, tppu_first, tppu_length, &val );
    return EALCT_SUCCESS;	
  }

// Read Test Pulse Power Up/Down status
ALCTController::ALCTSTATUS ALCTController::alct_read_test_pulse_powerup(int* jtag,
									long* test_pulse_power_up)
  {
    int j = *jtag;
    int val;
    
    //printf("*** Read_test_pulse_powerup\n");
    
    if (WRONG_PORT(chamber_type_))
      return EALCT_PORT;     
    
    val = 0;
    fd = j;
    do_jtag(8, 047, READ, tppu_first, tppu_length, &val);
    *test_pulse_power_up = val;
    
    return EALCT_SUCCESS;	
  }

// Set Test Pulse Group Mask
ALCTController::ALCTSTATUS ALCTController::alct_set_test_pulse_groupmask(int* jtag,
									 long test_pulse_group_mask)
  {
  int j = *jtag;
  int val = test_pulse_group_mask;

  if (WRONG_PORT(chamber_type_))
    return EALCT_PORT; 
  
  fd = j;
  do_jtag(8, 040, WRITE, tpg_first, tpg_length, &val );
  return EALCT_SUCCESS;
  }

// Read Test Pulse Group Mask
ALCTController::ALCTSTATUS ALCTController::alct_read_test_pulse_groupmask(int* jtag,
									  long* test_pulse_group_mask)
  {
  int j = *jtag;
  int val;
  
  if (WRONG_PORT(chamber_type_))
    return EALCT_PORT; 

  val = 0;
  fd = j;
  do_jtag(8, 041, READ, tpg_first, tpg_length, &val);
  *test_pulse_group_mask = val;
  
  
  return EALCT_SUCCESS;

  }

// Set Test Pulse Strip Mask
ALCTController::ALCTSTATUS ALCTController::alct_set_test_pulse_stripmask(int* jtag,
									 long test_pulse_strip_mask)
  {
  int j = *jtag;
  int val = test_pulse_strip_mask;

  if (WRONG_PORT(chamber_type_))
    return EALCT_PORT; 
  
  fd = j;
  do_jtag(8, 042, WRITE, tps_first, tps_length, &val );

  return EALCT_SUCCESS;
  }

// Read Test Pulse Strip Mask
ALCTController::ALCTSTATUS ALCTController::alct_read_test_pulse_stripmask(int* jtag,
									  long* test_pulse_strip_mask)
  {
  int j = *jtag;
  int val = 0;
  
  if (WRONG_PORT(chamber_type_))
    return EALCT_PORT; 
    
  fd = j;
  do_jtag(8, 043, READ, tps_first, tps_length, &val);
  *test_pulse_strip_mask = val;
  
  return EALCT_SUCCESS;
  }

ALCTController::ALCTSTATUS ALCTController::alct_init_slow_control(int* jtag, long thresh)
{
  int i;
  ALCTSTATUS res;
  int j= *jtag;
  if (WRONG_PORT(chamber_type_))
    return EALCT_PORT;
  
  res = alct_set_test_pulse_powerup(&j, 0);
  if ( res != 0)
  {
    return res;
  }
  printf("ALCT Station 1 \n") ;
  res = alct_set_test_pulse_amp(&j, 0);
  if ( res != 0)
  {
    return res;
  }
  printf("ALCT Station 2 \n") ;
  res = alct_set_test_pulse_groupmask(&j, 0);
  if ( res != 0)
  {
    return res;
  }
  printf("ALCT Station 3 \n") ;
  res = alct_set_test_pulse_stripmask(&j, 0);
  if ( res != 0)
  {
    return res;
  }
  
  printf("ALCT Station 4 \n") ;
  for (i=0; i< nAFEBs_; i++)
  {
    //printf("Looping %d \n",i) ;
    res = alct_set_thresh(i, thresh);
    if ( res != 0)
    {
      return res;
    }
  }
  
  printf("Station 5 \n") ;
  for (i=0; i< alct_table[chamb_table[chamber_type_].alct].groups; i++)
  { printf( " jtag  %02x  group %02x \n",j,i);
  res = alct_set_standby(&j, i, 0x3f);
  if ( res != 0)
    {
      return res;
    }
  }
  return EALCT_SUCCESS;
}

// end from alct_slow_lib


ALCTController::ALCTSTATUS ALCTController::DumpFifo(){
  //
  elong OS;
  unsigned hfa, h, hn, hpatb, lfa, l, ln, lpatb, daq, bxn, empty;
      // read output FIFO
      elReadRegister(OSread, &OS);// read first dummy word
      for (int i = 0; i < 1030; i++) {       
	// This command reads one word of the information from FIFO
	elReadRegister(OSread, &OS);
	// split OS into fields
	DecodeOS(&OS,&empty,&hfa,&h,&hn,&hpatb,&lfa,&l,&ln,&lpatb,&daq,&bxn);
	if (empty) {
	  printf("Fifo empty\n");
	  break; // fifo empty
	}
      }
  //
}

ALCTController::ALCTSTATUS ALCTController::alct_fast_CheckShiftingPatternViaJTAG
(unsigned long verbose) {

  int ParamRegWriteInt = 0;
  unsigned i, j, k, ii, tc, chi;
  unsigned delay[6], YR;
  elong OS;
  unsigned hfa, h, hn, hpatb, lfa, l, ln, lpatb, daq, bxn, empty;
  unsigned hfound;
  ALCTSTATUS st = EALCT_SUCCESS;
  unsigned lyWGs = chamtype[chamber_type_].WG_number/6;
  elong el;
  int lyc, bitstowrite;
  unsigned long hclong;
  unsigned int r = 0x1ff;

  // put code 5 for delays, to make it distinct
  delay[0] = 0x5;
  delay[1] = 0x5;
  delay[2] = 0x5;
  delay[3] = 0x5;
  delay[4] = 0x5;
  delay[5] = 0x5;

  for (tc = 0; tc < 1; tc++) {
    if (verbose) printf ("\n\ntest pass = %d, %s%c\n", tc, 
			 (tc == 0) ? "all layers are enabled" : "disabled layer: ", (tc == 0) ? ' ' : (tc + '0'));
    el.clear();
    // create the hot channel mask disabling one layer (or no layers on the first pass)
    for (lyc = 1; lyc <= 6; lyc++) {
      if ((unsigned)lyc == tc) hclong = 0;
      else           hclong = 0xffffffff;

      bitstowrite = lyWGs;
      while (bitstowrite > 32) {
	//if (verbose) printf("el.cat: %lx %d\n", hclong, 32);
	el.cat(hclong, 32);
	bitstowrite -= 32;
      }
      //if (verbose) printf("el.cat: %lx %d\n", hclong, bitstowrite);
      el.cat(hclong, bitstowrite);
    }

    if (verbose) {
      printf("el.l: %d\n", el.l);
      for (unsigned int i = 0; i < el.l/32+1; i++) printf("%08lx\n", el.r[i]);
    }
                
    elWriteRegister (HCMaskWrite, &el);
    // alct_download_hot_mask(HCfilename[tc],1);
                              
    for (k = 0; k < lyWGs; k++) {
                                        
      if (verbose) printf ("\nWG = %02d", k);
      // disable input
      WriteIR(InputDisable);
                        
      for (i = 0; i < 7; i++) for (j = 0; j < 6; j++) pattern[i][j] = 0;
      // reset delay lines to 0s
      Write6DelayLines(delay, pattern[0], 0x7f );
      ParamRegWriteInt = ParamRegWrite; // enum put into int so it can be passed to WriteRegister
      WriteRegister (ParamRegWriteInt, &r);

      // enable input. This is done to clear the internal logic with 0s
      WriteIR(InputEnable);
      // disable input
      WriteIR(InputDisable);
                        
      GeneratePattern(0, 1); // clear image
      GenerateAccPattern(k); // create pattern in WGk
                     
      // convert it into delay line pattern
      PrepareDelayLinePatterns();
      // write patterns into delay lines
      for (unsigned int chi = 0; chi < delayChains(); chi++) {
	Write6DelayLines(delay, pattern[chi], (1 << chi));
	ParamRegWriteInt = ParamRegWrite;  // enum put into int so it can be passed to WriteRegister
	WriteRegister (ParamRegWriteInt, &r);
      }
      // Set YRIclkEn (see Verilog code)
      YR = 1;
      WriteRegister (YRwrite, &YR);
      // enable input. Here the input pattern will be processed, and the data written into the output FIFO;
      WriteIR(InputEnable);
      // disable input
      WriteIR(InputDisable);
                        
      // Clear YRIclkEn
      YR = 0;
      WriteRegister (YRwrite, &YR);
                        
      hn = ln = 0xffff;
      ii = 0xffff;
      hfound = 0xffff;
      // read output FIFO
      elReadRegister(OSread, &OS);// read first dummy word
      for (i = 0; i < 1030; i++) {       
	// This command reads one word of the information from FIFO
	elReadRegister(OSread, &OS);
	// split OS into fields
	DecodeOS(&OS,&empty,&hfa,&h,&hn,&hpatb,&lfa,&l,&ln,&lpatb,&daq,&bxn);
	if(h != 0 || l != 0) {
	  ii = i;
	  
	  if( ((h != 0 || l != 0) && (hn != k || hfound != 0xffff)) || (h != tq[tc]) ) {
	    // if the WG found (hn) is not equal to WG in the pattern - complain
	    if (verbose) {
	      printf ("\tError in pattern for WG %02d:\n", k);        
	      printf("\tExpected: i = %03d hfa:%d h:%ld hn:%02d hpatb:%d lfa:%d l:%ld ln:%02d lpatb:%d bxn:%d\n", 
		     ii, 0,tq[tc],k,  0,     1,tq[tc],k,  0,     bxn);
	      printf("\tFound:    i = %03d hfa:%d h:%d hn:%02d hpatb:%d lfa:%d l:%d ln:%02d lpatb:%d bxn:%d\n", 
		     ii, hfa, h, hn, hpatb, lfa, l, ln, lpatb, bxn);
	    }
	    st = EALCT_TESTFAIL;                                         
	  }

	  if (hn == k) hfound = i;
	}
	if (empty) break; // fifo empty
      }
      if (hfound == 0xffff) {
	// the pattern was not found
	if (verbose) {
	  printf ("\tError: pattern WG = %02d, WG not found\n", k);
	  printf ("\tOS = %lx %lx\n", OS.r[1], OS.r[0]);
	}
	st = EALCT_TESTFAIL;
      }
    }
  }
  return st;
}







/*---------------------------------------------------------------------------*/

/* These defaults are either values to be loaded, or values to be expected */

void ALCTController::set_empty(int empty){
  //
  unsigned cr[3];
  //
  GetConf(cr,1);
  //
  cr[0] = cr[0]&0xfffffff7 | ((empty&0x1)<<3) ;
  //
  SetConf(cr,1);
  unpackControlRegister(cr);
  //
}

void ALCTController::set_l1a_internal(int internal){
  //
  unsigned cr[3];
  //
  GetConf(cr,1);
  //
  printf("cr[1] %x \n",cr[1]);
  //
  printf("internal %d \n",internal); 
  //
  cr[1] = cr[1]&0xffefffff | ((internal&0x1)<<20) ;
  //
  printf("cr[1] %x \n",cr[1]);
  //
  SetConf(cr,1);
  unpackControlRegister(cr);
  //
}


void ALCTController::set_l1a_delay(int delay){
  //
  unsigned cr[3];
  //
  GetConf(cr,1);
  //
  printf("cr[1] %x \n",cr[1]);
  //
  printf("delay %d \n",delay); 
  //
  cr[1] = cr[1]&0xfffff00f | ((delay&0xff)<<4) ;
  //
  printf("cr[1] %x \n",cr[1]);
  //
  SetConf(cr,1);
  unpackControlRegister(cr);
  //
}

void ALCTController::set_defaults(alct_params_type *p) {
  int    j, k;

/* Slow Control Chip */
/* -- ID register */
  p->sc_idreg.chip    = 8;
  p->sc_idreg.version = 11;
  p->sc_idreg.day     = 0x10;
  p->sc_idreg.month   = 0x12;
  p->sc_idreg.year    = 0x1999;

/* -- Serial bus */
  p->test_pulse_amp = 0x0;
  for (j = 0; j < nAFEBs_; j++) thresholds_[j] = 140;
  for (j = 0; j < nAFEBs_; j++) delays_[j] = 0;

/* -- TP Group register */
  p->test_pulse_group_mask = 0x0;

/* -- TP Strip register */
  p->test_pulse_strip_mask = 0x0;

/* -- Standby register */
  for (j = 0; j < n_lct_chips_; j++) p->standby_mask[j] = 0x3f;

/* -- TP Power down register */
  p->test_pulse_power_up = 0;

/* Concentrator Chip */
/* -- ID register */
  p->conc_idreg.chip    	= 0x7;
  p->conc_idreg.version 	= 0xc;
  p->conc_idreg.day     	= 0x10;
  p->conc_idreg.month   	= 0x12;
  p->conc_idreg.year    	= 0x1999;

/* -- Configuration register */
  alct_trig_mode_		= 0;
  alct_ext_trig_en_	        = 0;
  alct_send_empty_         	= 0;
  alct_bxc_offset_         	= 0;
  alct_drift_delay_         	= 3;
  p->inject_test_pattern	= 0;

  p->inject_mode		= 0;
  p->inj_lct_chip_mask		= 0x7f;

  alct_nph_thresh_	        = 2;
  alct_nph_pattern_		= 4;

  alct_fifo_tbins_		= 7;
  alct_fifo_pretrig_ 	        = 1;
  alct_fifo_mode_		= 1;
  p->fifo_last_feb		= 3;

  alct_l1a_delay_		= 0x78;
  alct_l1a_window_		= 3;
  alct_l1a_offset_	        = 0;
  alct_l1a_internal_		= 0;

  p->board_id			= 5;
  p->bx_counter_offset		= 0;
  alct_ccb_enable_		        = 1;

  p->alct_jtag_disable		= 0;
  p->alct_test_pat_mode		= 0;
  p->alct_accel_mu_mode		= 0;
  p->alct_mask_all		= 1;
  p->config_spare		= 0;

/* -- Trigger register */
  p->trig		        = 0;

/* LCT chips */
  for (j = 0; j < n_lct_chips_; j++) {
/* -- ID register */
    p->lct[j].idreg.chip	= 0x3;
    p->lct[j].idreg.version	= 0xa;
    p->lct[j].idreg.day	= 0x8;
    p->lct[j].idreg.month	= 0x12;
    p->lct[j].idreg.year	= 0x1999;

/* -- Control register */
    p->lct[j].lct_jtag_disable	= 0;
    p->lct[j].test_pat_mode	= 0;
    p->lct[j].accel_mu_mode	= 0;
    p->lct[j].mask_all		= 1;
    p->lct[j].unassigned	= 0;

/* -- Hot Channel register(s) */
    for (k = 0; k < 6; k++) p->lct[j].hot_chan_mask[k] = 0xffff;

/* -- Arbitrary Test Pattern register(s) */
    p->lct[j].test_pattern[0]	= 0x0020;
    p->lct[j].test_pattern[1]	= 0x0000;
    p->lct[j].test_pattern[2]	= 0x0020;
    p->lct[j].test_pattern[3]	= 0x0020;
    p->lct[j].test_pattern[4]	= 0x0000;
    p->lct[j].test_pattern[5]	= 0x0020;
  }
  return;
}

/*---------------------------------------------------------------------------*/

void ALCTController::show_params(int access_mode, alct_params_type *p) {
  int    j, layer, mode;
  char   tp_mode[4][32] = { "6-hit collision muon in wg 5  ",
                            "6-hit accelerator muon in wg 5",
                            "arbitrary 96-bit pattern      ",
                            "undefined                     " };
  char   acmu_mode[4][26] = { "Ignore accelerator muons ",
                            "Low priority accel muons ",
                            "High priority accel muons",
                            "Ignore collision muons   " };
  char   trig_mode[4][32] = { "Pretrig on any muon            ",
                              "Pretrig only on accel muons    ",
                              "Pretrig only on collision muons",
                              "Pretrig vetoed by accel muon   " };
  char   fifo_mode[4][34] = { "No raw hits dump to DAQMB        ",
                              "Full raw-hits dump (all LCTs)    ",
                              "Local raw-hits dump (2 best LCTs)",
                              "Synchronization read out (nyi)   " };
  char   enabled[2][9] = { "disabled", " enabled" };
  char   power[2][4] = { "off", "on " };
  //char   inject_mode[2][11] = { "inject one", "continuous" };

  if (access_mode == READ) {
    printf("Software versions \n");
    std::cout << "  Concentrator: " << p->conc_idreg << std::endl;
    std::cout << "  Slow Control: " << p->sc_idreg << std::endl;
    for (unsigned int j = 0; j < n_lct_chips_; j++) {
      std::cout << "  LCT" << j << " " << p->lct[j].idreg << std::endl;
    }
  }

  printf("AFEB Thresholds: ");
  for (unsigned int j = 0; j < nAFEBs_; j++) {
    if (j == 12 || j == 24 || j == 36) printf("                 ");
    printf("%4d", (unsigned int)(thresholds_[j]));
    if (j % 3 == 2) printf("  ");
    if ((unsigned) j % 12 == 11 || j == nAFEBs_-1) printf("\n");
  }

  printf("Delays:");
/* printf(" -- not yet implemented --"); */
  for (unsigned int j = 0; j < nAFEBs_; j++) printf("%3d", delays_[j]);
  printf("\n");

  printf("Test pulse parameters:\n  ");
  if (access_mode != READ) printf("amplitude: %3d, ", p->test_pulse_amp);
  printf("group mask: %#2x, ", p->test_pulse_group_mask);
  printf("strip mask: %#2x, ", p->test_pulse_strip_mask);
  mode = p->test_pulse_power_up;
  printf("power: %1d (%s)\n", mode, power[mode]);
  printf("Standby masks: "); 
  for (unsigned int j = 0; j < n_lct_chips_; j++) printf(" 0x%2x", p->standby_mask[j]);
  printf("\n");
  //
  printf("Concentrator configuration\n");
  mode = alct_trig_mode_;
  printf("  trigger mode:   %d (%s)\n", mode, trig_mode[mode]);
  mode = alct_ext_trig_en_;
  printf("  external trig: %s, ", enabled[mode]);
  mode = p->pretrigger_and_halt;
  printf("pretrigger and halt: %s\n", enabled[mode]);
  mode = p->inject_test_pattern;
  printf("  inject test pattern: %s, ", enabled[mode]);
  //
  printf("  number of hit planes for pretrigger:  %d\n", 
	 alct_nph_thresh_);
  printf("  pattern number threshold for trigger: %d\n", alct_nph_pattern_);
  printf("  drift delay: %d\n", alct_drift_delay_);
  printf("  total num fifo time bins: %d, ", alct_fifo_tbins_);
  printf("  num fifo time bins before pretrigger: %d\n", 
	 alct_fifo_pretrig_);
  printf("  last afeb in fifo: %d, ", p->fifo_last_feb);
  mode = p->fifo_mode;
  printf("fifo mode: %d %s\n", mode, fifo_mode[mode]);
  printf("  level 1 accept: delay %d, offset %d, window width %d, "
	 "internal %d\n", alct_l1a_delay_, alct_l1a_offset_, 
	 alct_l1a_window_, alct_l1a_internal_);
  printf("  board id: %d, bxn offset %d, ccb enable %d, "
	 "alct jtag disable %d\n", p->board_id, p->bx_counter_offset, 
	 alct_ccb_enable_, p->alct_jtag_disable);
  mode = p->alct_test_pat_mode;
  printf("  alct test pattern mode: %d %s\n", mode, tp_mode[mode]);
  printf("  alct accel muon mode:   %d %s\n", mode, acmu_mode[mode]);
    printf("  alct mask all: %d, trigger register: 0x%x, ", p->alct_mask_all, 
	   p->trig);
    if (access_mode == READ) printf("  csc id: %d", p->csc_id);
    printf("\n");
    
    for (unsigned int j = 0; j < n_lct_chips_; j++) {
       printf("LCT %d parameters: ", j);
       if (access_mode == READ) {
	  mode = p->lct[j].lct_jtag_disable;
	  printf("   LCT control by jtag is %s, AFEB inputs are %s", 
		 enabled[1-mode], enabled[p->lct[j].mask_all]);
       }
       printf("\n");
       mode = p->lct[j].test_pat_mode;
       printf("  test pattern mode: %d -- %s\n", mode, tp_mode[mode]);
       mode = p->lct[j].accel_mu_mode;
       printf("  accel muon mode:   %d -- %s\n", mode, acmu_mode[mode]);
       printf("  hot channel masks:");
       for (layer = 0; layer < 6; layer++) 
	  printf(" 0x%4x", (unsigned short)(p->lct[j].hot_chan_mask[layer]));
       printf("\n");
       printf("  test patterns:    ");
       for (layer = 0; layer < 6; layer++) 
	  printf(" 0x%4x", (unsigned short)(p->lct[j].test_pattern[layer]));
       printf("\n");
    }
    return;
}

/*---------------------------------------------------------------------------*/


// Read OnBoard Temperature
ALCTController::ALCTSTATUS ALCTController::alct_read_temp(int* jtag,
                                                          float* temp)
  {
  int j = *jtag;
  long t;

  if (WRONG_PORT(chamber_type_))
    return EALCT_PORT;

  alct_read_temp_adc(&t);
  const float TEMP_CONVERS_COEF = 0.1197;
  *temp = t*TEMP_CONVERS_COEF-50;
  return EALCT_SUCCESS;

  }

// Read row ADC value for onboard Temperature
ALCTController::ALCTSTATUS  ALCTController::alct_read_temp_adc(long *adc) {
  int    val[MAX_NUM_VALUES] = {0};

  static int    first[] = { 0, 4, 5, 20 };
  static int    length = 11;

/* Read thresholds one AFEB at a time */
  int chip_id = 8; // myc++ 2
  
  /* Now loop through the channels, doing double writes of the address of the 
   * channel, and unpacking the tdo bits on the second read.
   */
  int opcode = 020 + 2;                  /* 020 is 20 octal = 16 */
  /* reverse the bit order for DACs and ADCs */
  long temp_bits = 10 ; 
  int data = 0;
  for (int k = 0; k < 4; k++) if (temp_bits & (1 << (3 - k))) data |= 1 << k;

  val[0] = data;
  do_jtag2(chip_id, opcode, WRITE, first, length, val);

  val[0] = data;
  do_jtag2(chip_id, opcode, WRITE, first, length, val);

  temp_bits = 0;
  for (int k = 0; k < 10; k++ ) if ( val[2] & (1<<k) ) temp_bits |= 1 << (9-k) ;
  *adc = temp_bits;
  return EALCT_SUCCESS;
}

//------------------------------------------------------------

/*---------------------------------------------------------------------------*/

//@@ from alct_jtag_io.c

int ALCTController::do_jtag(int chip_id, int opcode, int mode, const int *first, int length, 
 int *val) {
#ifdef debugV
  std::cout << "call alct::do_jtag for slot "<< fd << " chip " << chip_id << std::endl;
#endif
  int     i, j, k, m, ichip, j_start, nframes, step_mode = 0;
  char    tms_pre_read[3] = { 1, 0, 0 };
  int     bits_per_opcode[MAX_NUM_CHIPS] = {5,6};  // myc++ { 4, 4, 4, 4, 4, 4, 4, 4, 6 }; back to 4
  int     tms_preop_code[5] = { 0, 1, 1, 0, 0 };
  int     tms_postop_code[3] = { 1, 0, 0 };
  char    tdi[MXBITSTREAM], tms[MXBITSTREAM], tdo[MXBITSTREAM];
  int jtagchannel;

  for (i=0; i< MAX_JTAG_BUF_SIZE; i++)
  {
    sndbuf[i] = 0;
    rcvbuf[i] = 0;
  }


/*  
 * First JTAG operation writes opcodes to the TAP instruction registers 
 */

/* Put TAP in state ShfIR */
  for (k = 0, j = 0; k < 5; k++) 
  {
    tdi[j]   = 0;
    tms[j++] = tms_preop_code[k];
  }

/* Construct opcode for each chip (all but chip_id are BYPASS = all 1's) */
  //printf(" slot - %d\n", fd);
  if(chip_id <= 7)
  {    
    jtagchannel=4;
    ichip=0;
    tmb_->start(8);
  }
  else
  {
    jtagchannel=0; //4;
    ichip=1;
    tmb_->start(6);
  } 

  //printf("\nOpcode 0x%02x \n", opcode);
  for (i=0; i< sizeof(opcode); i++)
    {
      sndbuf[i] = (opcode >> 8*i)  & 0x00ff;
      // printf(" %02x", sndbuf[i]); 
    }
  
  for (k = 0; k < bits_per_opcode[ichip]; k++) 
    {
      tdi[j] = (opcode >> k) & 0x1;
      tms[j++] = 0;
    }
  
  tms[j-1] = 1;        /* TMS goes high on last frame */
  
  /* Put TAP back in RTI mode */
  for (k = 0; k < 3; k++) 
    {
      tdi[j]   = 0;
      tms[j++] = tms_postop_code[k];
    }
  
  /* Do JTAG */
  nframes = j;
  
  for (j = 0; j < nframes; j++) 
    {
      tdo[j] = 0 ;
    }

  tmb_->scan(INSTR_REG, sndbuf, bits_per_opcode[ichip], rcvbuf , 0 );
  
  // jtag_io_byte_(&nframes, tms, tdi, tdo, &step_mode);
  
  /*
   * Second JTAG operation writes data to the selected register of chip_id, 
   * zero to bypass register of all other chips.
   */
  
  /* Put TAP in state ShfDR */
  for (k = 0, j = 0; k < 3; k++) 
    {
      tdi[j]   = 0;
      tms[j++] = tms_pre_read[k];
    }
  
  
  /* Convert the list of data items into a one-byte-per-bit array, if writing,
   * or else fill with zeros.
   */
  for (k = 0; k < ((length-1)/8+1); k++)
    { 
      sndbuf[k] = 0; 
  }
  // printf("\nTDI ");
  j_start = j;
  m = 0;
  for (k = 0; k < length; k++) 
    {
      if (mode == WRITE) 
    {
      if (k == first[m+1]) m++;
      tdi[j] = (val[m] >> (k-first[m])) & 0x1;
      sndbuf[k/8] |= tdi[j] << (k%8);
    }
      else 
	{ 
	  tdi[j] = 0;
	}      
      tms[j++] = 0;
    }
  
  tms[j-1] = 1;        /* TMS goes high on last data frame */
  
  // printf("\nData In ");    
  // for (k = 0; k < ((length-1)/8+1); k++)
  // { printf(" %x", sndbuf[k]); }
  
  /* Put TAP back in RTI mode */
  for (k = 0; k < 3; k++) 
    {
      tdi[j]   = 0;
      tms[j++] = tms_postop_code[k];
  }
  
  /* Do JTAG */
  nframes = j;
  
  // jtag_io_byte_(&nframes, tms, tdi, tdo, &step_mode);

  // if (mode == READ)
  //
  tmb_->scan(DATA_REG, sndbuf, length, rcvbuf, 1);
  //

  //else { tmb_->scan(DATA_REG, sndbuf, length, rcvbuf, 0);}
	


/* Whether reading or writing, convert the one-byte-per-bit tdo data array 
   into a list of values */

 /* 
  for (k = 0; k < ((length-1)/8+1); k++)
  { 
  	rcvbuf[k] = 0; 
  }
  */
  
 /*
  printf("\nTDO "); 
  
  i = 0;
  val[0] = 0;
  j = j_start;
  for (k = 0; k < length; k++) 
  {
    if (k == first[i+1]) val[++i] = 0;
    if (tdo[j++]) 
    {
    	val[i] |= (1 << (k-first[i]));
			rcvbuf[k/8] |= 1 << (k%8);
    }
    
  }

	printf("\nOut ");
	for (k=0; k<i; k++)
	{ printf(" 0x%02x", val[k]); }
   
  printf("\nData Out ");
  for (k = 0; k < ((length-1)/8+1); k++)
  { printf(" 0x%02x", rcvbuf[k]); }
  */
 	//if (mode == READ) 
  //{
    i = 0;
    val[0] = 0;
    for (k = 0; k < length; k++)
    {
      if (k == first[i+1]) val[++i] = 0;
      if ((rcvbuf[k/8] >> (k%8)) & 0x01) 
      { 
      	val[i] |= (1 << (k-first[i])); 
      }
    }

   // printf("\nNet Out ");
   // for (k=0; k<i; k++)
   //  { printf(" 0x%02x", val[k]); }  
  //}
  
  alct_end();
  return 0;
}

//-------------------------------------------------------------------------------

int ALCTController::do_jtag2(int chip_id, int opcode, int mode, const int *first, int length, 
 int *val) {
#ifdef debugV
  std::cout << "call alct::do_jtag2 for slot "<< fd << " chip " << chip_id << std::endl;
#endif

  //std::cout << "call alct::do_jtag2 for slot "<< fd << " chip " << chip_id << " opcode " << opcode << std::endl;

  int     i, j, k, m, ichip, j_start, nframes, step_mode = 0;
  char    tms_pre_read[3] = { 1, 0, 0 };
  int     bits_per_opcode[MAX_NUM_CHIPS] = {4,6};  // myc++ { 4, 4, 4, 4, 4, 4, 4, 4, 6 }; back to 4
  int     tms_preop_code[5] = { 0, 1, 1, 0, 0 };
  int     tms_postop_code[3] = { 1, 0, 0 };
  char    tdi[MXBITSTREAM], tms[MXBITSTREAM], tdo[MXBITSTREAM];
  int jtagchannel;
  int i10;

/*
 * First JTAG operation writes opcodes to the TAP instruction registers 
 */

/* Put TAP in state ShfIR */
  for (k = 0, j = 0; k < 5; k++) 
  {
    tdi[j]   = 0;
    tms[j++] = tms_preop_code[k];
  }

/* Construct opcode for each chip (all but chip_id are BYPASS = all 1's) */
  //printf(" slot - %d\n", fd);
  if(chip_id <= 7)
  {    
    jtagchannel=128;
    ichip=0;
    tmb_->start(8);
  }
  else
  {
    jtagchannel=0; //4;
    ichip=1;
    tmb_->start(6);
  } 

  // xblaster_(&jtagchannel) ;

  //	printf("\n\nOpcode 0x%02x", opcode);
  for (i=0; i< sizeof(opcode); i++)
    {
      sndbuf[i] = (opcode >> 8*i)  & 0x00ff;
    }
  //
  for (k = 0; k < bits_per_opcode[ichip]; k++) 
    {
      tdi[j] = (opcode >> k) & 0x1;
      tms[j++] = 0;
    }
  
  tms[j-1] = 1;        /* TMS goes high on last frame */
  
  /* Put TAP back in RTI mode */
  for (k = 0; k < 3; k++) 
    {
      tdi[j]   = 0;
      tms[j++] = tms_postop_code[k];
    }
  
  /* Do JTAG */
  nframes = j;
  
  for(i10=0;i10<nframes;i10++)
    {
    }
  
  //std::cout << "bits_per_opcode" << bits_per_opcode[ichip] << std::endl;  
	
  //   jtag_io_byte_(&nframes, tms, tdi, tdo, &step_mode);
  //
  tmb_->scan(INSTR_REG, sndbuf, bits_per_opcode[ichip], rcvbuf , 0 );
  //
  
  for (j = 0; j < nframes; j++) {
  }
  for (j = 0; j < nframes; j++) {
  }
  for (j = 0; j < nframes; j++) {
  }
  
  
  /*
   * Second JTAG operation writes data to the selected register of chip_id, 
   * zero to bypass register of all other chips.
   */
  
  /* Put TAP in state ShfDR */
  for (k = 0, j = 0; k < 3; k++) 
    {
      tdi[j]   = 0;
      tms[j++] = tms_pre_read[k];
    }
  
  /* Convert the list of data items into a one-byte-per-bit array, if writing,
   * or else fill with zeros.
   */
  for (k = 0; k < ((length-1)/8+1); k++)
    { sndbuf[k] = 0; }
  //	printf("\nTDI ");
  
  j_start = j;
  m = 0;
  for (k = 0; k < length; k++) 
    {
      if (mode == WRITE) 
	{
	  if (k == first[m+1]) m++;
	  tdi[j] = (val[m] >> (k-first[m])) & 0x1;
	  sndbuf[k/8] |= tdi[j] << (k%8);
	}
      else 
	{
	  tdi[j] = 0;
	}
      tms[j++] = 0;
    }
  tms[j-1] = 1;        /* TMS goes high on last data frame */
	
  // printf("\nData In ");    
  // for (k = 0; k < ((length-1)/8+1); k++)
  // { printf(" %x", sndbuf[k]); }
  
  /* Put TAP back in RTI mode */
  for (k = 0; k < 3; k++) 
    {
      tdi[j]   = 0;
	    tms[j++] = tms_postop_code[k];
  }
	
	/* Do JTAG */
	nframes = j;
	
	for(i10=0;i10<nframes;i10++){
  }
	
	//   jtag_io_byte_(&nframes, tms, tdi, tdo, &step_mode);a
	//
	tmb_->scan(DATA_REG, sndbuf, length, rcvbuf, 1);
	//
	
	for(i10=0;i10<nframes;i10++){
	}
	
	
	
	/* Whether reading or writing, convert the one-byte-per-bit tdo data array 
	   into a list of values */
	
 /*
   for (k = 0; k < ((length-1)/8+1); k++)
   { 
   rcvbuf[k] = 0; 
   }
 */
  
 /* 
    printf("\nTDO ");  
    
    i = 0;
    val[0] = 0;
    j = j_start;
    j = -1 ;
    for (k = 0; k < nframes-1; k++) 
    {
    if (k == first[i+1]) val[++i] = 0;
    if (tdo[j++]) 
    {
    	val[i] |= (1 << (k-first[i]));
			rcvbuf[k/8] |= 1 << (k%8);
    };
  }
  
	printf("\nOut ");
 	for (k=0; k<i; k++)
 	{ printf(" 0x%02x", val[k]); }
  
  printf("\nData Out ");
  for (k = 0; k < ((length-1)/8+1); k++)
  { printf(" 0x%02x", rcvbuf[k]); }
  */   
// 	if (mode == READ) 
//  {
    i = 2;
    val[0] = 0;
    val[1] = 0;
    val[2] = 0;
    //
    //for (k = 0 ; k <= length/8; k++) { printf("do_jtag2.%d %02x \n",k,rcvbuf[k]);} 
    //
    for (k = 1 ; k <= length; k++)
      {
	if (k == first[i+1]) val[++i] = 0;
	if ((rcvbuf[k/8] >> (k%8)) & 0x01) 
	  { 
	    // val[i] |= (1 << (k-first[i]));
	    val[i] |= 1 << (k-1); 
	  }
      }
    //    printf("i - %d\n", i);
    //    printf("\nNet Out ");
    //    for (k=0; k<=i; k++)
    //    { printf(" 0x%02x", val[k]); }  
    //  }
    //
    //printf("val0 %x \n",val[0]);
    //printf("val1 %x \n",val[1]);
    //printf("val2 %x \n",val[2]);
    //
    alct_end();  
  return 0;
}


void ALCTController::start(int idev)
{
  tmb_->start(idev);
}

void ALCTController::alct_end()
{
  tmb_->endDevice();
}

/*
void ALCTController::send_last()
{
  int n,nt1;
  char cmdxx[2];
  char inxx[2];
  if(plev==2){
    tmb_->scan(TERMINATE,cmdxx,0,inxx,2);
    plev=1;
    idevo=0;
  }
}
*/


/*---------------------------------------------------------------------------*/



/* 
 * File: jtag_io_byte.c  -- replaces Jonathan's Fortran version with an
 *                          interface to the OSU jtag driver; note that
 *                          fd has been added to the argument list
 * Author: Lisa Gorn    6 April 2000
 *
 */


//#define MXBITSTREAM  1000       /* Same as Jonathan's Fortran version */
//#define NUM_POSTBITS    3


void ALCTController::jtag_io_byte_(int *nframes, char *tms, char *tdi, char *tdo, 
                   int *step_mode) {
  int    j, nbits, num_prebits;
  char   exchange_bits[MXBITSTREAM];
  struct JTAG_BBitStruct   driver_data;

/* Don't do anything if fd is < 0; this is so alct2000_load can also be
   used just to read the config file into the alct_params structure */
  if (fd < 0) {printf(" fd < 0 \n"); return;}

#ifdef mysimulation
  printf(" simulation works jtag_io_byte_ \n"); return;
#endif

/* TAP reset string is TMS 1,1,1,1,1 */
  if (*nframes==5 && tms[0]==1 && tms[1]==1 && tms[2]==1 && tms[3]==1 && 
   tms[4]==1) {
    /* Reset TAP controller (driver_data is a dummy arg, here) */
    if (ioctl(fd, JTAG_BIOIDLE, &driver_data) < 0) perror("ioctl JTAG_BIOIDLE");
  }

/* Return-to-idle string is TMS 1,1,1,1,1,0 */
  else if (*nframes==6 && tms[0]==1 && tms[1]==1 && tms[2]==1 && tms[3]==1 && 
   tms[4]==1 && tms[5]==0) {
    /* Return TAP controller to idle (driver_data is a dummy arg, here) */
    if (ioctl(fd, JTAG_BIORTI, &driver_data) < 0) perror("ioctl JTAG_BIORTI");
  }


/* Send one clock string is TMS 0 */
  else if (*nframes==1 && tms[0]==0) {
    if (ioctl(fd, JTAG_BIO_1CLK, &driver_data) < 0) perror("ioctl JTAG_BIO_1CLK");
  }

/* Instruction strings begin with TMS 0,1,1,0,0 */
  else if (tms[0]==0 && tms[1]==1 && tms[2]==1 && tms[3]==0 && tms[4]==0) {
    num_prebits = 5;
    nbits = *nframes - (num_prebits + NUM_POSTBITS);

    memcpy(exchange_bits, &tdi[num_prebits], nbits);

    driver_data.target = JTAG_B_INSTR;
    driver_data.n = nbits;
    driver_data.data = exchange_bits;

    if (ioctl(fd, JTAG_BIOEXCHANGE_ALCT, &driver_data) < 0)
     perror("ioctl JTAG_BIOEXCHANGE_ALCT");

    memcpy(&tdo[num_prebits], exchange_bits, nbits);
  }

/* or (in alct_slowcontrol) instruction strings begin with TMS 1,1,0,0 */
  else if (tms[0]==1 && tms[1]==1 && tms[2]==0 && tms[3]==0) {
    num_prebits = 4;
    nbits = *nframes - (num_prebits + NUM_POSTBITS);

    memcpy(exchange_bits, &tdi[num_prebits], nbits);

    driver_data.target = JTAG_B_INSTR;
    driver_data.n = nbits;
    driver_data.data = exchange_bits;

    if (ioctl(fd, JTAG_BIOEXCHANGE_ALCT, &driver_data) < 0)
     perror("ioctl JTAG_BIOEXCHANGE_ALCT");

    memcpy(&tdo[num_prebits], exchange_bits, nbits);
  }

/* Data strings begin with TMS 1,0,0 */
  else if (tms[0]==1 && tms[1]==0 && tms[2]==0) {
    num_prebits = 3;
    nbits = *nframes - (num_prebits + NUM_POSTBITS);
    memcpy(exchange_bits, &tdi[num_prebits], nbits);

    driver_data.target = JTAG_B_DATA;
    driver_data.n = nbits;
    driver_data.data = exchange_bits;

    if (ioctl(fd, JTAG_BIOEXCHANGE_ALCT, &driver_data) < 0)
     perror("ioctl JTAG_BIOEXCHANGE_ALCT");

    memcpy(&tdo[num_prebits], exchange_bits, nbits);

  }  else {
    printf("jtag_io_byte error: unknown sequence TMS = { %d %d %d %d %d %d }\n",
     tms[0], tms[1], tms[2], tms[3], tms[4], tms[5]);
  }

  for (j = 0; j < *nframes; j++) tdo[j] &= 0x1;
  return;
}
/* 
 * File: jtag_io_byte.c  -- replaces Jonathan's Fortran version with an
 *                          interface to the OSU jtag driver; note that
 *                          fd has been added to the argument list
 * Author: Lisa Gorn    6 April 2000
 *
 */


//#define MXBITSTREAM  8000       /* Same as Jonathan's Fortran version */
// causes a warning - previously defined as 1000 on line 8064
//#define NUM_POSTBITS  4 
// causes a warning - previously defined as 3 on line 8065


extern void Parse(char *, int *, char **);
void InsertDelayJTAG(unsigned long d, unsigned int time_units);


const unsigned int MYMILLISECONDS = 1000;
const unsigned int MYMICROSECONDS = 1;
static long loopsPerMillisecond = -1;

// subroutine for delaying by a given number of microseconds or milliseconds
void InsertDelayJTAG(unsigned long d, unsigned int time_units)
{
        unsigned long loops,i;
        clock_t start, finish;
	int db = 0 ;

        if(loopsPerMillisecond < 0)
	  { // determine the timing of the computer if we haven't already done so
	    if (db>4)                printf("start time test\n");
	    start = clock();        // get starting time
	    for(i = 100000000L; i>0L; i--);       // do a whole bunch of empty loops
	    finish = clock();       // get ending time
	    if (db>4)                printf("time test finish\n");
	    // compute the number of empty loops per millisecond
	    loopsPerMillisecond = 100000000L/(finish/1000);
	    if (db>4)                printf("loops_per_milsec:%u\n",(unsigned int)loopsPerMillisecond);
	    
	  }
        if(time_units==MYMICROSECONDS && d>10000)
	  {
	    time_units = MYMILLISECONDS;
                d /= 1000;
	  }
        if(time_units == MYMILLISECONDS)
	  {       // doing millisecond timing here
	    for(loops = (loopsPerMillisecond*d); loops>0; loops--);
	  }
        else
	  {       // doing microsecond timing here
	    for(loops = (loopsPerMillisecond*d)/1000; loops>0; loops--);
	  }
}

int ALCTController::ReadIDCODE(){
  //
  char sndbuf[8];
  char rcvbuf[8];
  //
  sndbuf[0] = 0xff;
  sndbuf[1] = 0x1f;
  sndbuf[2] = 0x00;
  sndbuf[3] = 0x00;
  sndbuf[4] = 0x00;
  sndbuf[5] = 0x00;
  sndbuf[6] = 0x00;
  sndbuf[7] = 0x00;
  //
  rcvbuf[0] = 0x0;
  rcvbuf[1] = 0x0;
  rcvbuf[2] = 0x0;
  rcvbuf[3] = 0x0;
  rcvbuf[4] = 0x0;
  rcvbuf[5] = 0x0;
  rcvbuf[6] = 0x0;
  rcvbuf[7] = 0x0;
  //
  tmb_->start(9);
  //
  tmb_->scan(-1, sndbuf,-1, rcvbuf , 0);
  tmb_->scan(INSTR_REG, sndbuf,13, rcvbuf , 1);
  tmb_->scan(-1, sndbuf,-1, rcvbuf , 0);
  //

  alct_end();
  //
  printf("%x %x %x %x %x %x %x %x\n",rcvbuf[7]&0xff,rcvbuf[6]&0xff,rcvbuf[5]&0xff,rcvbuf[4]&0xff,
	 rcvbuf[3]&0xff,rcvbuf[2]&0xff,rcvbuf[1]&0xff,rcvbuf[0]&0xff);
  //
  // Next
  //
  sndbuf[0] = 0xfe;
  sndbuf[1] = 0x00;
  sndbuf[2] = 0x00;
  sndbuf[3] = 0x00;
  sndbuf[4] = 0x00;
  sndbuf[5] = 0x00;
  sndbuf[6] = 0x00;
  sndbuf[7] = 0x00;
  //
  rcvbuf[0] = 0x0;
  rcvbuf[1] = 0x0;
  rcvbuf[2] = 0x0;
  rcvbuf[3] = 0x0;
  rcvbuf[4] = 0x0;
  rcvbuf[5] = 0x0;
  rcvbuf[6] = 0x0;
  rcvbuf[7] = 0x0;
  //
  again:
  //
  tmb_->start(9);
  //
  tmb_->scan(-1, sndbuf,-1, rcvbuf , 0);
  tmb_->scan(DATA_REG, sndbuf,32, rcvbuf , 1);
  tmb_->scan(-1, sndbuf,-1, rcvbuf , 0);
  //
  alct_end();
  //
  printf("%x %x %x %x %x %x %x %x\n",rcvbuf[7]&0xff,rcvbuf[6]&0xff,rcvbuf[5]&0xff,rcvbuf[4]&0xff,
	 rcvbuf[3]&0xff,rcvbuf[2]&0xff,rcvbuf[1]&0xff,rcvbuf[0]&0xff);
  //
  //goto again;
  //
}

int ALCTController::NewSVFLoad(int *jch, char *fn, int db ){
  //
  int buffersize = 8192;
  char buf[buffersize];
  char bufR[buffersize];
  char bufR1[buffersize];
  //char* dummy ;
  //
  char sndbuf[buffersize];
  char rcvbuf[buffersize];
  //
  for (int i=0; i<buffersize; i++ ) {
     sndbuf[i] = 0;
     rcvbuf[i] = 0;
  }
  //
  buf[0] = '\0';
  bufR[0] = '\0';
  bufR1[0] = '\0';
  //
  FILE *dwnfp;
  std::vector <char*> pointerList;
  char *smask, *tdo, *tdi, *mask, *sir, *sdr, *endline;
  char *runtest;
  char *hir, *tir, *hdr, *tdr, *sri;
  char *dump;
  char smaskvalue[buffersize], tdovalue[buffersize], maskvalue[buffersize], tdivalue[buffersize]; 
  //
  char hirtdivalue[buffersize], hdrtdivalue[buffersize], tirtdivalue[buffersize], tdrtdivalue[buffersize];
  char hirsmaskvalue[buffersize], hdrsmaskvalue[buffersize], tirsmaskvalue[buffersize], tdrsmaskvalue[buffersize];
  //
  int sirvalue, sdrvalue, tirvalue, hirvalue, tdrvalue, hdrvalue ;
  int tirsize, hirsize, tdrsize, hdrsize ;
  int tdivalueInt[buffersize];
  //
  dwnfp = fopen(fn,"r");
  int GettingData = 0;
  //int offset=0;
  int FirstTime=0;
  //
  hirsize = 0;
  hdrsize = 0;
  tdrsize = 0;
  tirsize = 0;
  //
  for(unsigned int i=0; i<sizeof(buf); i++ ) buf[i] = '\0';
  for(unsigned int i=0; i<sizeof(bufR); i++ ) bufR[i] = '\0';
  for(unsigned int i=0; i<sizeof(bufR1); i++ ) bufR1[i] = '\0';
  //
  while (fgets(bufR1,sizeof(bufR1),dwnfp) != NULL) {
     //
     printf("%s\n",bufR1);
     //
     if ( strstr(bufR1,"//") ) continue;
     //
     // Fix for long data sizes
     //
     int i;
     for(i=0;bufR1[i];i++){
	if ( iscntrl(bufR1[i])){
	   bufR1[i] = 0; // end string here
	   break;
	}
     }
     //
     printf("%s\n",bufR1);
     //
     char *test = bufR1;
     char *dest = bufR;
     int counter = 0;
     while ( *test != '\0') {
	*dest = *test;
	dest++;
	test++;
	counter++;
     }
     //
     *dest = '\0';
     //
     printf("********** \n");
     //
     std::cout << std::endl;
     std::cout << "Line Read " << std::endl;
     printf("%s",bufR1);
     std::cout << "Here" << std::endl;
     //
     printf("GettingData %d\n",GettingData);
     //
     if ( strstr(bufR,"//") ) continue;
     //
     if ( GettingData == 0 ) {
	//
	char *test1 = bufR;
	char *test2 = buf;
	//
	int counter=0;
	//
	while ( *test1 != '\0' ) {
	   *test2 = *test1;
	   test2++;
	   counter++;
	   test1++;
	}
	*test2 = '\0';
	printf("Copying and removing end of line \n");
     }
     //
     if ( ! strstr(buf,";") ) {
	if ( FirstTime == 1 ) {
	   char *test1 = bufR;
	   char *test2 = buf;
	   //
	   while( *test2 != '\0' ) test2++; //End of buffer
	   //test2--; //Go back one and append
	   //
	   while ( *test1 != '\0' ) {
	      *test2 = *test1;
	      test2++;
	      test1++;
	   }
	   *test2 = '\0';
	   printf("Appending \n");
	}
	FirstTime = 1;
	GettingData = 1;
	if ( ! strstr(buf,";") ) continue;
     } else {
	FirstTime = 0;
     }
     //
     GettingData = 0 ;
     //
     //
     if (GettingData) continue;
     //
     // Check for comment
     //
     std::cout << "Line Acc  " << std::endl;
     printf("%s", buf) ;
     std::cout << "Here" << std::endl;
     std::cout << std::endl;
     //
     if ( ! strstr(buf,"//") ) {
	if ( (runtest  = strstr(buf,"RUNTEST")) ) {
	   char inst[10], inst2[10];
	   int time;
	   sscanf(runtest,"%s %d %s",inst,&time,inst2);
	   printf("Time to wait %d\n",time);
	   ::sleep((time/1000000)+1);
	   printf("%c7", '\033');
	}
	if ( (smask  = strstr(buf,"SMASK")) ) {
	   pointerList.push_back(smask);
	   printf(" Found SMASK %x \n",(int)smask);
	}
	if ( (tdo    = strstr(buf,"TDO")) )  {
	   pointerList.push_back(tdo);
	   printf(" Found TDO %x \n",(int)tdo);
	}
	if ( (tdi    = strstr(buf,"TDI")) )  {
	   pointerList.push_back(tdi);
	   printf(" Found TDI %x \n",(int)tdi);
	}
	if ( (mask   = strstr(buf," MASK")) ) {
	   pointerList.push_back(mask);
	   printf(" Found MASK %x \n",(int)mask);
	}
	if ( (sdr    = strstr(buf,"SDR")) ) {
	   printf(" Found SDR %x \n",(int)sdr);
	   pointerList.push_back(sdr);
	}
	if ( (sir    = strstr(buf,"SIR")) ) {
	   pointerList.push_back(sir);
	   printf(" Found SIR %x \n",(int)sir);
	}
	if ( (tir    = strstr(buf,"TIR")) ) {
	   pointerList.push_back(tir);
	   printf(" Found TIR %x \n",(int)tir);
	}
	if ( (hir    = strstr(buf,"HIR")) ) {
	   pointerList.push_back(hir);
	   printf(" Found HIR %x \n",(int)hir);
	}
	if ( (tdr    = strstr(buf,"TDR")) ) {
	   pointerList.push_back(tdr);
	   printf(" Found TDR %x \n",(int)tdr);
	}
	if ( (hdr    = strstr(buf,"HDR")) ) {
	   pointerList.push_back(hdr);
	   printf(" Found HDR %x \n",(int)hdr);
	}
	if ( (sri    = strstr(buf,"STATE RESET IDLE")) ) {
	   pointerList.push_back(sri);
	   printf(" Found STATE RESET %x \n",(int)sri);
	}
	if ( (endline    = strstr(buf,";")) ) {
	   pointerList.push_back(endline);
	   printf(" Found ; %x \n",(int)endline);
	}
	//
	sort(pointerList.begin(),pointerList.end());
	//
	smask = 0;
	tdo   = 0;
	tdi   = 0;
	mask  = 0;
	sdr   = 0;
	sir   = 0;
	tir   = 0;
	hir   = 0;
	tdr   = 0;
	hdr   = 0;
	//
	for( unsigned int j=0; j<sizeof(tdivalue); j++) tdivalue[j] = '\0';
	for( unsigned int j=0; j<sizeof(tdovalue); j++) tdovalue[j] = '\0';
	for( unsigned int j=0; j<sizeof(maskvalue); j++) maskvalue[j] = '\0';
	for( unsigned int j=0; j<sizeof(maskvalue); j++) smaskvalue[j] = '\0';
	for( unsigned int j=0; j<sizeof(sndbuf); j++) sndbuf[j] = '\0';
	for( unsigned int j=0; j<sizeof(rcvbuf); j++) rcvbuf[j] = '\0';
	//
	for( int j=0; j<buffersize; j++)tdivalueInt[j] = 0;
	//
	sdrvalue = -1;
	sirvalue = -1;
	hdrvalue = -1;
	hirvalue = -1;
	tdrvalue = -1;
	tirvalue = -1;
	//
	printf("Number of pointers %d \n",pointerList.size());
	//
	for (unsigned int NPointer=1; NPointer<pointerList.size(); NPointer++) {
	   //
	   char Command[buffersize];
	   //
	   for (unsigned int count=0; count<sizeof(Command); count++) Command[count] = 0;
	   //
	   //printf("Pointer %x Pointer %x diff %d \n",pointerList[NPointer],pointerList[NPointer-1],
	   //pointerList[NPointer]-pointerList[NPointer-1]);
	   //
	   //printf("%s \n",pointerList[NPointer]);
	   //printf("%s \n",pointerList[NPointer-1]);
	   //
	   strncpy(Command,pointerList[NPointer-1],(pointerList[NPointer]-pointerList[NPointer-1]));
	   std::cout << "--> " << Command << std::endl;
	   //
	   // Now split further
	   //
	   // Don't forget pointer gets deleted....
	   //
	   char CommandList[10][10] = {"SMASK","TDO","TDI"," MASK","SDR","SIR","TIR","HIR","TDR","HDR"};
	   //
	   for ( int i=0; i<10; i++) {
	      //
	      printf("%s \n",CommandList[i]);
	      //
	      if ( (dump = strstr(Command,CommandList[i]))  ) {
//
		 if ( i == 0 ) smask = strstr(Command,CommandList[i]);
		 if ( i == 1 ) tdo   = strstr(Command,CommandList[i]);
		 if ( i == 2 ) tdi   = strstr(Command,CommandList[i]);
		 if ( i == 3 ) mask  = strstr(Command,CommandList[i]);
		 if ( i == 4 ) sdr   = strstr(Command,CommandList[i]);
		 if ( i == 5 ) sir   = strstr(Command,CommandList[i]);
		 if ( i == 6 ) tir   = strstr(Command,CommandList[i]);
		 if ( i == 7 ) hir   = strstr(Command,CommandList[i]);
		 if ( i == 8 ) tdr   = strstr(Command,CommandList[i]);
		 if ( i == 9 ) hdr   = strstr(Command,CommandList[i]);
		 //
		 // Now find data
		 //
		 char* begin = strstr(Command,"(");
		 char* end   = strstr(Command,")");
		 //
		 //
		 if(begin>0) {
		    if ( i == 0 ) strncpy(smaskvalue,begin+1,end-begin-1);
		    if ( i == 1 ) strncpy(tdovalue,begin+1,end-begin-1);
		    if ( i == 2 ) strncpy(tdivalue,begin+1,end-begin-1);
		    if ( i == 3 ) strncpy(maskvalue,begin+1,end-begin-1);
		 }
		 //
		 char inst[buffersize];
		 //
		 if ( i == 4 ) {
		    sscanf(Command,"%s %d",inst,&sdrvalue);
		 }
		 if ( i == 5 ) sscanf(Command,"%s %d",inst,&sirvalue);
		 if ( i == 6 ) sscanf(Command,"%s %d",inst,&tirvalue);
		 if ( i == 7 ) sscanf(Command,"%s %d",inst,&hirvalue);
		 if ( i == 8 ) sscanf(Command,"%s %d",inst,&tdrvalue);
		 if ( i == 9 ) sscanf(Command,"%s %d",inst,&hdrvalue);
		 //
	      }
	   }
	   //
	}
	//
	pointerList.clear();
	//
	// Now do it
	//
	std::cout << "Do it" << std::endl;
	//
	int nbytes, nbytes32, nbytesNo ;
	//
	if ( hir && hirvalue >= 0 ) {
	   //
	   strcpy(hirtdivalue,tdivalue);
	   strcpy(hirsmaskvalue,smaskvalue);
	   hirsize             = hirvalue;
	}
	if ( hdr && hdrvalue >= 0 ) {
	   //
	   strcpy(hdrtdivalue,tdivalue);
	   strcpy(hdrsmaskvalue,smaskvalue);
	   hdrsize           = hdrvalue;
	   }
	if ( tir && tirvalue >= 0 ) {
	   //
	   strcpy(tirtdivalue,tdivalue);
	   strcpy(tirsmaskvalue,smaskvalue);
	   tirsize          = tirvalue;
	   printf(" <<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Setting tirvalue tirsize %d %s\n",tirsize,tirtdivalue);
	   //
	}
	if ( tdr && tdrvalue >= 0 ) {
	   //
	   strcpy(tdrtdivalue,tdivalue);
	   strcpy(tdrsmaskvalue,smaskvalue);
	   tdrsize          = tdrvalue;
	   //
	}
	//
	if ( tdr ) printf("**************** %s\n",tdrtdivalue);
	if ( tir ) printf("**************** %s\n",tirtdivalue);
	//
	if ( sdr ) {
	   int length = 0;
	   char *test = tdivalue;
	   while( *test != 0 ) {
	      test++;
	      length++;
	   }
	   printf(" Number of bytes to send %d and number read %d\n",sdrvalue,length*4);
	   if ( sdrvalue%4 == 0) {
	      if ( sdrvalue != length*4) return -1;
	   }
	}
	//
	if ( sir ) {
	   int length = 0;
	   char *test = tdivalue;
	   while( *test != 0 ) {
	      test++;
	      length++;
	   }
	   printf(" Number of bytes to send %d and number read %d\n",sirvalue,length*4);
	   if ( sirvalue %4 == 0) {
	      if( sirvalue > length*4 ) return -1;
	   }
	}
	//
	if ( sir || sdr ) {
	   //
	   if ( sir ) std::cout << "SirValue = " << std::dec << sirvalue << std::endl;	   
	   if ( sdr ) std::cout << "SdrValue = " << std::dec << sdrvalue << std::endl;	   
	   //
	   if ( (sirvalue > 0 || sdrvalue > 0 ) ) {
	      //
	      int tdoflag=0;
	      int tdiflag=0;
	      //
	      if ( sir ) nbytes   = (sirvalue+tirsize+hirsize-1)/8+1;
	      if ( sdr ) nbytes   = (sdrvalue+tdrsize+hdrsize-1)/8+1;
	      //
	      if ( sir ) nbytes32 = (tirsize-1)/32+1;
	      if ( sdr ) nbytes32 = (tdrsize-1)/32+1;
	      //
	      if ( sir ) nbytesNo = (sirvalue-1)/8+1;
	      if ( sdr ) nbytesNo = (sdrvalue-1)/8+1;
	      //
	      if ( sir || sdr ) printf("Data entering shift %s\n",tdivalue);
	      //
	      if ( sir ) {
		 for( int i=0; i<buffersize; i++) tdivalueInt[i] = 0;
		 ConstructShift(hirtdivalue,hirsize,tdivalue,sirvalue,tirtdivalue,tirsize,tdivalueInt);
	      }
	      if ( sdr ) {
		 for( int i=0; i<buffersize; i++) tdivalueInt[i] = 0;
		 ConstructShift(hdrtdivalue,hdrsize,tdivalue,sdrvalue,tdrtdivalue,tdrsize,tdivalueInt);
	      }
	      //
	      if ( sir || sdr ) for( int i=0; i<nbytes; i++ ) printf("Data back from shift %x\n",tdivalueInt[i]);
	      //
	      if ( tdo ) tdoflag = 1;
	      if ( tdi ) tdiflag = 1;
	      //
	      if (tdoflag) {
		 for(int i=0; i<nbytes32;i++) printf(" TDO value expected = %s \n",(tdovalue));
	      }
	      //
	      if ( tdiflag ) {
		 //
		 printf("Number of bytes %d\n",nbytes);
		 //
		 for( int i=0; i<nbytes; i++) {
		    sndbuf[i] = tdivalueInt[i]&0xff;
		 }
		 //		 
		 std::cout << " Bytes Send= " << nbytes << std::endl;
		 if ( sir ) printf("Number of bits %d\n",sirvalue);
		 if ( sir ) printf("Number of bits %d\n",sirvalue);
		 //
		 for ( int i=0; i<nbytes; i++) {
		    printf("%d %02x\n",i,sndbuf[i]);
		 }
		 //
		 tmb_->start(9);
		 //
		 tmb_->scan(-1, sndbuf,-1, rcvbuf , 0);
		 if ( sir ) tmb_->scan(INSTR_REG, sndbuf,sirvalue, rcvbuf, 1); // Always read back. Can't hurt...
		 if ( sdr ) tmb_->scan(DATA_REG, sndbuf,sdrvalue, rcvbuf, 1); // Always read back. Can't hurt...
		 tmb_->scan(-1, sndbuf,-1, rcvbuf , 0);
		 //
		 alct_end();
		  }
		  //
		  if (tdoflag) {
		     std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<< Read back TDO"<< std::endl;
		     printf("Nbytes %d\n",nbytesNo);
		     for (int i=0; i<nbytesNo; i++) printf("%02x ",rcvbuf[i]);
		     printf("\n");
		  }
		  //
	   } else {
	      printf("I don't know....\n");
	      return -1;
	   }
	}
	//
     }
     //
     buf[0] = '\0';
     bufR[0] = '\0';
     //
  }
  //
  return 0;
  //
}

int ALCTController::ConvertCharToInteger(char Data[], int DataInt[], int& counter){
//
   printf("Inside ConvertCharToInteger\n");
   //
   char* test = Data;
   counter = 0;
   //
   printf("Data input %s\n",Data);
   //
   char mytry[9];
   char* mypointer = mytry;
   for (unsigned int i=0; i<sizeof(mytry); i++) mytry[i] = '\0';
   mypointer = mytry;
   int InLoop = 0;
//
   while ( *test != '\0' ) {
      //
      //printf("%s\n",test);
      //
      InLoop = 1;
      //
      *mypointer = *test;
      mypointer++;
      test++;
      if ( *test == '\0' ) break;
      *mypointer = *test;
      mypointer++;
      test++;
      if ( *test == '\0' ) break;
      *mypointer = *test;
      mypointer++;
      test++;
      if ( *test == '\0' ) break;
      //
      *mypointer = *test;
      mypointer++;
      test++;
      if ( *test == '\0' ) break;
      *mypointer = *test;
      mypointer++;
      test++;
      if ( *test == '\0' ) break;
      *mypointer = *test;
      mypointer++;
      test++;
      if ( *test == '\0' ) break;
      *mypointer = *test;
      mypointer++;
      test++;
      if ( *test == '\0' ) break;
      //
      *mypointer = *test;
      mypointer++;
      test++;
      //
      InLoop = 0;
      //
      int dummy;
      printf("%s\n",mytry);
      sscanf(mytry,"%x",&dummy);
      DataInt[counter] = dummy;
      counter++;
      for (unsigned int i=0; i<sizeof(mytry); i++) mytry[i] = '\0';
      mypointer = mytry;
      //
   }
   //
   if ( InLoop ) {
      //printf("Break\n");
      int dummy;
      //printf("%s\n",mytry);
      sscanf(mytry,"%x",&dummy);
      //printf("dummy %x\n",dummy);
      //printf("Size of DataInt %d\n",sizeof(DataInt));
      DataInt[counter] = dummy;
      counter++;
      InLoop = 0;
   }
   //
   //printf("counter %d\n",counter);
   //
   //for (int i=0; i<counter;i++) printf("%x %d\n",DataInt[i],i);
   //
   mypointer++;
   *mypointer = '\0';
   //
   return 0;
   //
}

int ALCTController::ConstructShift(char HeaderData[],int HeaderDataSize, char Data[], int DataSize, 
				   char TrailerData[], int TrailerDataSize, int ConstructResult[] ){
   //
   printf("Getting HeaderSize        %d \n",HeaderDataSize);
   printf("Getting DataSize          %d \n",DataSize);
   printf("Getting TrailerDataSize   %d \n",TrailerDataSize);
   //
   printf("HeaderData : %s\n",HeaderData);
   printf("Data       : %s\n",Data);
   printf("TrailerData : %s\n",TrailerData);
   //
   int DataInt[8192];
   int HeaderInt[8192];
   int TrailerInt[8192];
   //
   for( int i=0; i<8192; i++) DataInt[i] = 0;
   for( int i=0; i<8192; i++) HeaderInt[i] = 0;
   for( int i=0; i<8192; i++) TrailerInt[i] = 0;
   //
   int counter = 0;
   int datacounter = 0;
   //
   ConvertCharToInteger(Data,DataInt,datacounter);
   ConvertCharToInteger(HeaderData,HeaderInt,counter);
   ConvertCharToInteger(TrailerData,TrailerInt,counter);
   //
   printf(" Counter = %d \n",datacounter);
   for ( int i=0; i<datacounter; i++ ) printf("Data %x\n",DataInt[i]);
   //
   std::bitset<262144> TrailerDataSet(*TrailerInt);
   std::bitset<262144> DataSet(*DataInt);
   std::bitset<262144> HeaderDataSet(*HeaderInt);
   std::bitset<262144> ConstructShift;
   ConstructShift.reset();
   //
   printf("HeaderData\n");
   for(int i=0; i<HeaderDataSize; i++) {
    if (HeaderDataSet.test(i)) {
    printf("1");
    } else {
    printf("0");
    }
   }
   printf("\n");
//
   printf("Data\n");
   for(int i=0; i<DataSize; i++) {
      if (DataSet.test(i)) {
       printf("1");
    } else {
       printf("0");
    }
   }   
   printf("\n");
   //
   printf("TrailerData\n");
   for(int i=0; i<TrailerDataSize; i++) {
      if (TrailerDataSet.test(i)) {
	 printf("1");
      } else {
	 printf("0");
      }
   }   
   printf("\n");
   //
   for(int i=0; i<TrailerDataSize; i++) if(TrailerDataSet.test(i)) ConstructShift.set(i); 
   for(int i=0; i<DataSize; i++) if(DataSet.test(i)) ConstructShift.set(i+TrailerDataSize); 
   for(int i=0; i<HeaderDataSize; i++) if(HeaderDataSet.test(i)) ConstructShift.set(i+TrailerDataSize+DataSize); 
   //
   int nbits = HeaderDataSize+DataSize+TrailerDataSize;
   //
   //
   for (int i=0; i<nbits; i++ ){
      if (ConstructShift.test(i)) {
	 ConstructResult[i/8] |= (0x1<<((i%8))) ;
      }
   }
   //
   printf("Result\n");
   for(int i=0; i<nbits; i++) {
      if (ConstructShift.test(i)) {
	 printf("1");
      } else {
	 printf("0");
      }
   }   
   //
   printf("\n");
   //
   int nbytes = (HeaderDataSize+DataSize+TrailerDataSize-1)/8+1;
   //
   for( int i=0; i<nbytes; i++) printf(" Data going back %x \n",ConstructResult[i]);
   //
   printf("\n");
   //
   return 0;
   //
}

// ==============================
// ===  Main SVF Load subroutine
// ===  jch - Xblaster JTAG chain number
// ===  fn  - SVF Filename
// ===  db  - Debug level option
// ==============================

int ALCTController::SVFLoad(int *jch, const char *fn, int db )
{
 unsigned char snd[MAXBUFSIZE], rcv[MAXBUFSIZE], expect[MAXBUFSIZE],rmask[MAXBUFSIZE],smask[MAXBUFSIZE],cmpbuf[MAXBUFSIZE];
  unsigned char sndbuf[MAXBUFSIZE],rcvbuf[MAXBUFSIZE], realsnd[MAXBUFSIZE];
  unsigned char sndhdr[MAXBUFSIZE],sndtdr[MAXBUFSIZE], sndhir[MAXBUFSIZE], sndtir[MAXBUFSIZE];
  unsigned char hdrsmask[MAXBUFSIZE],tdrsmask[MAXBUFSIZE], hirsmask[MAXBUFSIZE], tirsmask[MAXBUFSIZE];
  int rcvword;
  FILE *dwnfp, *ftmptdi, *ftmpsmask;
  char buf[MAXBUFSIZE], buf2[256];
//  char buf[8192],buf2[256];
  char *Word[256],*lastn;
  const char *downfile;
  char fStop;
  int jchan;
  unsigned char sndvalue;
  fpos_t ftdi_pos, fsmask_pos;
  unsigned char send_tmp;//, rcv_tmp;
  int i,j,Count,nbytes,nbits,nframes,step_mode,pause;
  int hdrbits = 0, tdrbits = 0, hirbits = 0, tirbits = 0;
  int hdrbytes = 0, tdrbytes = 0, hirbytes = 0, tirbytes = 0; 
  int nowrit, cmpflag, errcntr;
  static int count;
  struct JTAG_BBitStruct   driver_data;
  // int jtag_chain[4] = {1, 0, 5, 4};
  int jtag_chain_tmb[5] = {7, 6, 9, 8, 3};
  // === SIR Go through SelectDRScan->SelectIRScan->CaptureIR->ShiftIR  
  //char tms_pre_sir[4]={ 1, 1, 0, 0 }; 
  char tdi_pre_sir[4]={ 0, 0, 0, 0 };
  // === SDR Go through SelectDRScan->CaptureDR->ShiftDR
  // char tms_pre_sdr[3]={ 1, 0, 0 };
  char tdi_pre_sdr[3]={ 0, 0, 0 };
  // === SDR,SIR Go to RunTestIdle after scan
  // char tms_post[4]={ 0, 1, 1, 0 };
  char tdi_post[4]={ 0, 0, 0, 0 };
  int total_packages ;
  int send_packages ;

  total_packages = 0 ;
  send_packages = 0 ;
  jchan = *jch;
  downfile = fn;
  errcntr = 0;
  if (downfile==NULL)    downfile="default.svf";
  
  dwnfp    = fopen(downfile,"r");
  while (fgets(buf,256,dwnfp) != NULL) 
    {
      memcpy(buf,buf,256);
      Parse(buf, &Count, &(Word[0]));
      if( strcmp(Word[0],"SDR")==0 ) total_packages++ ;
    }
  fclose(dwnfp) ;
  
  printf("=== Programming Design with %s through JTAG chain %d\n",downfile, jchan);  
  printf("=== Have to send %d DATA packages \n",total_packages) ;
  dwnfp    = fopen(downfile,"r");
  
  if (dwnfp == NULL)
    {
      perror(downfile);
      
      return -1;
    }
  
  tmb_->start(jtag_chain_tmb[jchan-1]); 
  
  count=0; 
  nowrit=1;
  step_mode=0;
  while (fgets(buf,256,dwnfp) != NULL)  
    {
      if((buf[0]=='/'&&buf[1]=='/')||buf[0]=='!')
	{
	  if (db>4)          printf("%s",buf);
	}
      else 
	{
	  if(strrchr(buf,';')==0)
	    {
	      lastn=strrchr(buf,'\r');
	      if(lastn!=0)lastn[0]='\0';
	      lastn=strrchr(buf,'\n');
	      if(lastn!=0)lastn[0]='\0';
	      memcpy(buf2,buf,256);
	      Parse(buf2, &Count, &(Word[0]));
	      if(( strcmp(Word[0],"SDR")==0) || (strcmp(Word[0],"SIR")==0))
		{
		  sscanf(Word[1],"%d",&nbits);
		  if (nbits>MAXBUFSIZE) // === Handle Big Bitstreams
		    {
		      if (db) printf("%s Bitstream size %d is bigger than MAXBUFSIZE = %d ...splitting...\n",
				     Word[0], nbits, MAXBUFSIZE);
		      ftmptdi=fopen("tdi_tmp.bit","w");
		      nbytes=(nbits-1)/8+1;
		      if (db)              printf("%d Bytes required\n", nbytes);
		      if(strcmp(Word[2],"TDI")==0)
			{
			  fprintf(ftmptdi,&Word[3][1]);
			  fStop = 0; 
			  do
			    {
			      if (fgets(buf2, 256, dwnfp) != NULL)
				{
				  lastn=strrchr(buf2,'\r');
				  if(lastn!=0)lastn[0]='\0';
				  lastn=strrchr(buf2,'\n');
				  if(lastn!=0)lastn[0]='\0';
				  Parse(buf2, &Count, &(Word[0]));
				  lastn=strchr(Word[0],')');
				  if(lastn!=0){ lastn[0]='\0'; fStop = 1; } 
				  fprintf(ftmptdi, Word[0]);
				}
			      else
				{
				  if (db)                    printf("End of File encountered.\n");
				  return -1;
				}
			    }
			  while (!fStop);
			}
		      ftmpsmask=fopen("smask_tmp.bit","w"); 
		      if(strcmp(Word[1],"SMASK")==0)
			{
			  fprintf(ftmpsmask,&Word[2][1]);
			  fStop = 0;
			  do
			    {
			      if (fgets(buf2, 256, dwnfp) != NULL)
				{
				  lastn=strrchr(buf2,'\r');
				  if(lastn!=0)lastn[0]='\0';
				  lastn=strrchr(buf2,'\n');
				  if(lastn!=0)lastn[0]='\0';
				  Parse(buf2, &Count, &(Word[0]));
				  lastn=strrchr(Word[0],')');
				  if(lastn!=0){ lastn[0]='\0'; fStop = 1; } 
				  fprintf(ftmpsmask, Word[0]);
				}
			      else
				{
				  if (db) printf("End of File encountered.\n");
				  return -1;
				}
			    }
			  while (!fStop); 
			}
		      freopen("tdi_tmp.bit","r",ftmptdi);
		      freopen("smask_tmp.bit","r",ftmpsmask);
		      fseek(ftmptdi,-2, SEEK_END);
		      fseek(ftmpsmask,-2,SEEK_END);
		      count=0;
		      printf("[");
		      driver_data.target = JTAG_B_DATA;
		      driver_data.n = 8;
		      driver_data.data = (char*)&sndbuf[0];
		      if (ioctl(fd, JTAG_BIOSETTAP, &driver_data) < 0)
			perror("ioctl JTAG_BIOSETTAP");
		      do
			{
			  if ((fgetpos(ftmptdi, &ftdi_pos)==0) && (fgetpos( ftmpsmask, &fsmask_pos) ==0))
			    {
			      if (db>10) printf("pos - %d; pos - %d\n", ftdi_pos, fsmask_pos );
			      //
			      if ((fread(buf, 2, 1, ftmptdi)>0) && (fread(buf2, 2, 1, ftmpsmask)>0))
				{
				  sscanf(buf,"%2x",(int *)&snd[0]);
				  sscanf(buf2,"%2x",(int *)&smask[0]);
				  sndbuf[0]=snd[0]&smask[0];
				  for (i=0;i<8;i++)
				    {
				      sndvalue=(sndbuf[0]>>i)&0x01;
				      if ((count*8+i+1)<nbits)
					{  
					  if (db>5)                              printf("%d",sndvalue); 
					  if (ioctl(fd, JTAG_BIOBIT, &sndvalue ) < 0)
					    perror("ioctl JTAG_BIOBIT"); 
					}
				      else
					if ((count*8+i+1)==nbits) 
					  {
					    if (db>5)                              printf("%d",sndvalue);
					    if (ioctl(fd, JTAG_BIOLASTBIT, &sndvalue) < 0)
					      {
						perror("ioctl JTAG_BIOLASTBIT");
					      }
					    if (db) printf(" nbits %d sent ",(count*8+i+1)); 
					    
					  }
				    }
				  count=count+1;
				}
			      fseek(ftmptdi,-4, SEEK_CUR);
			      fseek(ftmpsmask,-4,SEEK_CUR);
			    } 
			}
		      while ((feof(ftmptdi) == false) && (feof(ftmpsmask) == false));
		      //@@ changed from simply while ((ftdi_pos > 0) && (fsmask_pos > 0)), which gave errors
		      if (ioctl(fd, JTAG_BIORTI, &driver_data) < 0)
			perror("ioctl JTAG_BIORTI");
		      if (db)              printf("]...done\n");
		      fclose(ftmptdi); 
		      fclose(ftmpsmask);
		      continue;
		    }
		  else do  // == Handle Normal Bitstreams
		    {
		      lastn=strrchr(buf,'\r');
		      if(lastn!=0)lastn[0]='\0';
		      lastn=strrchr(buf,'\n');
		      if(lastn!=0)lastn[0]='\0';
		      if (fgets(buf2,256,dwnfp) != NULL)
			{
			  strcat(buf,buf2);
			}
		      else 
			{
			  if (db)              printf("End of File encountered.  Quiting\n");
			  return -1;
			}
		    }
		  while (strrchr(buf,';')==0);
		}
	    } 
	  bzero(snd, sizeof(snd));
	  bzero(cmpbuf, sizeof(cmpbuf));
	  bzero(sndbuf, sizeof(sndbuf));
	  bzero(rcvbuf, sizeof(rcvbuf));
	  
	  /*
	    for(i=0;i<MAXBUFSIZE;i++)
	    {
	    snd[i]=0;
	    cmpbuf[i]=0;
	    sndbuf[i]=0;
	    rcvbuf[i]=0;
	    }
	  */
	  Parse(buf, &Count, &(Word[0]));
	  count=count+1;
	  cmpflag=0;
	  // ==================  Parsing commands from SVF file ====================
	  // === Handling HDR ===
	  if(strcmp(Word[0],"HDR")==0)
	    {
	      sscanf(Word[1],"%d",&hdrbits);
	      hdrbytes=(hdrbits)?(hdrbits-1)/8+1:0;
	    if (db)	  
	      printf("Sending %d bits of Header Data\n", hdrbits);
	    // if (db>3)          printf("HDR: Num of bits - %d, num of bytes - %d\n",hdrbits,hdrbytes);
	    for(i=2;i<Count;i+=2)
	      {
		if(strcmp(Word[i],"TDI")==0)
		  {
		    for(j=0;j<hdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(hdrbytes-j-1)+1],"%2X",(int *)&sndhdr[j]);
			// printf("%2X",sndhdr[j]);
		      }
		    // printf("\n%d\n",nbytes);
    		  }
      		if(strcmp(Word[i],"SMASK")==0)
		  {
		    for(j=0;j<hdrbytes;j++)
		      {
      		  	sscanf(&Word[i+1][2*(hdrbytes-j-1)+1],"%2X",(int *)&hdrsmask[j]);
		      }
		  }
     	 	if(strcmp(Word[i],"TDO")==0)
		  {
		    //if (db>2)             cmpflag=1;
		    cmpflag=1;
		    for(j=0;j<hdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(hdrbytes-j-1)+1],"%2X",(int *)&expect[j]);
        		}
		  }
      		if(strcmp(Word[i],"MASK")==0)
		  {
		    for(j=0;j<hdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(hdrbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		      }
		  }
	      }
	    }
	  
	  // === Handling HIR ===
	  else if(strcmp(Word[0],"HIR")==0)
	    {
	      // for(i=0;i<3;i++)sndbuf[i]=tdi_pre_sdr[i];
	      // cmpflag=1;    //disable the comparison for no TDO SDR
	      sscanf(Word[1],"%d",&hirbits);
	      hirbytes=(hirbits)?(hirbits-1)/8+1:0;
	      if (db)	  
		printf("Sending %d bits of Header Data\n", hirbits);
	      // if (db>3)          printf("HIR: Num of bits - %d, num of bytes - %d\n",hirbits,hirbytes);
	      for(i=2;i<Count;i+=2)
		{
		  if(strcmp(Word[i],"TDI")==0)
		  {
		    for(j=0;j<hirbytes;j++)
		      {
			sscanf(&Word[i+1][2*(hirbytes-j-1)+1],"%2X",(int *)&sndhir[j]);
			// printf("%2X",sndhir[j]);
		      }
		    // printf("\n%d\n",nbytes);
    		  }
		  if(strcmp(Word[i],"SMASK")==0)
		    {
		      for(j=0;j<hirbytes;j++)
		      {
      		  	sscanf(&Word[i+1][2*(hirbytes-j-1)+1],"%2X",(int *)&hirsmask[j]);
		      }
		    }
		  if(strcmp(Word[i],"TDO")==0)
		    {
		      //if (db>2)             cmpflag=1;
		      cmpflag=1;
		      for(j=0;j<hirbytes;j++)
			{
			  sscanf(&Word[i+1][2*(hirbytes-j-1)+1],"%2X",(int *)&expect[j]);
			}
		    }
		  if(strcmp(Word[i],"MASK")==0)
		    {
		      for(j=0;j<hirbytes;j++)
			{
			  sscanf(&Word[i+1][2*(hirbytes-j-1)+1],"%2X",(int *)&rmask[j]);
			}
		    }
		}
	    }	
	  
	  // === Handling TDR ===
	  else if(strcmp(Word[0],"TDR")==0)
	    {
	      // for(i=0;i<3;i++)sndbuf[i]=tdi_pre_sdr[i];
	      // cmpflag=1;    //disable the comparison for no TDO SDR
	      sscanf(Word[1],"%d",&tdrbits);
	      tdrbytes=(tdrbits)?(tdrbits-1)/8+1:0;
	      if (db)	  
		printf("Sending %d bits of Tailer Data\n", tdrbits);
	      // if (db>3)          printf("TDR: Num of bits - %d, num of bytes - %d\n",tdrbits,tdrbytes);
	      for(i=2;i<Count;i+=2)
	      {
		if(strcmp(Word[i],"TDI")==0)
		  {
		    for(j=0;j<tdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tdrbytes-j-1)+1],"%2X",(int *)&sndtdr[j]);
			// printf("%2X",sndhir[j]);
		      }
		    // printf("\n%d\n",nbytes);
    		  }
      		if(strcmp(Word[i],"SMASK")==0)
		  {
		    for(j=0;j<tdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tdrbytes-j-1)+1],"%2X",(int *)&tdrsmask[j]);
		      }
		  }
		if(strcmp(Word[i],"TDO")==0)
		  {
		    //if (db>2)             cmpflag=1;
		    cmpflag=1;
		    for(j=0;j<tdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tdrbytes-j-1)+1],"%2X",(int *)&expect[j]);
		      }
		  }
      		if(strcmp(Word[i],"MASK")==0)
		  {
		    for(j=0;j<tdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tdrbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		      }	
		  }
	      }
	    }
	  
	  // === Handling TDR ===
	  else if(strcmp(Word[0],"TIR")==0)
	  {
	    // for(i=0;i<3;i++)sndbuf[i]=tdi_pre_sdr[i];
	    // cmpflag=1;    //disable the comparison for no TDO SDR
	    sscanf(Word[1],"%d",&tirbits);
	    tirbytes=(tirbits)?(tirbits-1)/8+1:0;
	    if (db)	  
	      printf("Sending %d bits of Tailer Data\n", tdrbits);
	    // if (db>3)          printf("TIR: Num of bits - %d, num of bytes - %d\n",tirbits,tirbytes);
	    for(i=2;i<Count;i+=2)
	      {
		if(strcmp(Word[i],"TDI")==0)
		  {
		    for(j=0;j<tirbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tirbytes-j-1)+1],"%2X",(int *)&sndtir[j]);
			    // printf("%2X",sndhir[j]);
		      }
		    // printf("\n%d\n",nbytes);
    		  }
      		if(strcmp(Word[i],"SMASK")==0)
		  {
		    for(j=0;j<tirbytes;j++)
		      {
				  sscanf(&Word[i+1][2*(tirbytes-j-1)+1],"%2X",(int *)&tirsmask[j]);
		      }
		  }
		if(strcmp(Word[i],"TDO")==0)
		  {
		    //if (db>2)             cmpflag=1;
		    cmpflag=1;
		    for(j=0;j<tirbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tirbytes-j-1)+1],"%2X",(int *)&expect[j]);
		      }
		  }
      		if(strcmp(Word[i],"MASK")==0)
		  {
		    for(j=0;j<tirbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tirbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		      }
		  }
	      }
	  }
	  // === Handling SDR ===
	  else if(strcmp(Word[0],"SDR")==0)
	    {
	      for(i=0;i<3;i++)sndbuf[i]=tdi_pre_sdr[i];
	      // cmpflag=1;    //disable the comparison for no TDO SDR
	    sscanf(Word[1],"%d",&nbits);
	    nbytes=(nbits)?(nbits-1)/8+1:0;
	    if (db)	  printf("Sending %d bits Data\n", nbits);
	    // if (db>3)          printf("SDR: Num of bits - %d, num of bytes - %d\n",nbits,nbytes);
	    for(i=2;i<Count;i+=2)
	      {
	      if(strcmp(Word[i],"TDI")==0)
		{
		  for(j=0;j<nbytes;j++)
		    {
		      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&snd[j]);
		      //                printf("%2X",snd[j]);
		    }
		  //                printf("\n%d\n",nbytes);
		}
	      if(strcmp(Word[i],"SMASK")==0)
		{
		  for(j=0;j<nbytes;j++)
		    {
		      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&smask[j]);
		    }
		}
	      if(strcmp(Word[i],"TDO")==0)
		{
		  //if (db>2)             cmpflag=1;
		  cmpflag=1;
		  for(j=0;j<nbytes;j++)
		    {
		      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&expect[j]);
		    }
		}
	      if(strcmp(Word[i],"MASK")==0)
		{
		  for(j=0;j<nbytes;j++)
		    {
		      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		    }
		}
	      }
	    for(i=0;i<nbytes;i++)
	      {
	      send_tmp = snd[i]&smask[i];
	      for(j=0;j<8;j++)
		{
		  if ((i*8+j)< nbits) 
		    { 
		      sndbuf[i*8+j+3]=send_tmp&0x01; 
		    }
		  send_tmp = send_tmp >> 1;
		}
	      }
	    for(i=0;i<4;i++)sndbuf[nbits+3]=tdi_post[i];         
	    nframes=nbits+7;
          // Put send SDR here
	    for (i=0; i< ((hdrbits+nbits+tdrbits-1)/8+1); i++)
	      realsnd[i] = 0;
	    if (hdrbytes>0) {
	      for (i=0;i<hdrbytes;i++)
		realsnd[i]=sndhdr[i];
	    }
	    for (i=0;i<nbits;i++)
	    realsnd[(i+hdrbits)/8] |= (snd[i/8] >> (i%8)) << ((i+hdrbits)%8);
	    if (tdrbytes>0) {
	      for (i=0;i<tdrbits;i++)
		realsnd[(i+hdrbits+nbits)/8] |= (sndtdr[i/8] >> (i%8)) << ((i+hdrbits+nbits)%8);
	    }
	    
	    if (db>6) {	printf("SDR Send Data:\n");
	    for (i=0; i< ((hdrbits+nbits+tdrbits-1)/8+1); i++)
	    printf("%02X",realsnd[i]);
	    printf("\n");
	    }
	    send_packages++ ;
	    printf("%c[0m", '\033');
	    printf("%c[1m", '\033');
	    //printf("%c[2;50H", '\033');
	    if ( send_packages == 1 )   {
	      printf("%c7", '\033');
	    }
	  printf("%c8", '\033'); 
	  printf(" Sending %d/%d ",send_packages,total_packages) ;
	  printf("%c8", '\033'); 
	  printf("%c[0m", '\033');
	  if ( send_packages == total_packages ) printf("\n") ;
	  //
	  tmb_->scan(DATA_REG, (char*)realsnd, hdrbits+nbits+tdrbits, (char*)rcv, 2); 
	  //
	  if (cmpflag==1)
	    {     
	      /*
		for(i=0;i<nbytes;i++)
		{
		rcv_tmp = 0;
		for(j=0;j<8;j++)
		{
		if ((i*8+j) < nbits)
		{
		rcv_tmp |= ((rcvbuf[i*8+j+3]<<7) &0x80); 
		}
		rcv_tmp = rcv_tmp >> (j<7);
		
		}	
		rcv[nbytes-1-i] = rcv_tmp;		
		}
	      */
	      if (db>4){	  	printf("SDR Readback Data:\n");
	      //for(i=0;i<nbytes;i++) printf("%02X",rcv[i]);
	      for (i=0; i< ((hdrbits+nbits+tdrbits-1)/8+1); i++) 
		printf("%02X",rcv[i]);
	      printf("\n");
	      }	
	      
	      for(i=0;i<nbytes;i++)
		{
		  rcvword = rcv[i+(hdrbits/8)]+(((int)rcv[i+1+(hdrbits/8)])<<8);
		  rcvword = rcvword>>(hdrbits%8);
		  rcvword = rcv[i];
		  // if (((rcv[nbytes-1-i]^expect[i]) & (rmask[i]))!=0 && cmpflag==1)
		  if ((((rcvword&0xFF)^expect[i]) & (rmask[i]))!=0 && cmpflag==1)
		    {
		      printf("1.read back wrong, at i %02d  rdbk %02X  expect %02X  rmask %02X\n",i,rcv[i]&0xFF,expect[i]&0xFF,rmask[i]&0xFF);
		      errcntr++;
		    }
		}	
	    }
	  /*         if (cmpflag==1)
		     {
		     for(i=3;i<nbits+3;i++) printf("%d",rcvbuf[i]);
		     printf("\n");
		     }
	  */       
          }
        // === Handling SIR ===
        else if(strcmp(Word[0],"SIR")==0)
          {
	    for(i=0;i<4;i++)sndbuf[i]=tdi_pre_sir[i];
	    // cmpflag=1;    //disable the comparison for no TDO SDR
	    sscanf(Word[1],"%d",&nbits);
	    nbytes=(nbits)?(nbits-1)/8+1:0;
	    if (db)	  printf("Sending %d bits of Command\n",nbits);
	    // if (db>3)          printf("SIR: Num of bits - %d, num of bytes - %d\n",nbits,nbytes);
	    for(i=2;i<Count;i+=2)
	      {
		if(strcmp(Word[i],"TDI")==0)
		  {
		    for(j=0;j<nbytes;j++)
		      {
			sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&snd[j]);
		      }
		  }
		if(strcmp(Word[i],"SMASK")==0)
		  {
		    for(j=0;j<nbytes;j++)
		      {
			sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&smask[j]);
		      }
		  }
		if(strcmp(Word[i],"TDO")==0)
		  {
			cmpflag=1;
			// if (db>2)              cmpflag=1;
			for(j=0;j<nbytes;j++)
			  {
			    sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&expect[j]);
			  }
		  }
		if(strcmp(Word[i],"MASK")==0)
		  {
		    for(j=0;j<nbytes;j++)
		      {
			sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		      }
		  }
	      }
	    for(i=0;i<nbytes;i++)
	      {
		send_tmp = snd[i]&smask[i];
		// printf("\n%d - ", send_tmp);
		for(j=0;j<8;j++)
		  {
		    if ((i*8+j)< nbits) 
		      {
			sndbuf[i*8+j+4]=send_tmp&0x01;
			// printf("%d", sndbuf[i*8+j+4]);
		      }
		    send_tmp = send_tmp >> 1;
		  }
	      }
	    for(i=0;i<4;i++)sndbuf[nbits+4]=tdi_post[i];
	    nframes=nbits+8;
	    // Put send SIR here
	    for (i=0; i< ((hirbits+nbits+tirbits-1)/8+1);  i++)
	      realsnd[i] = 0;
	    if (hirbytes>0) {
	      for (i=0;i<hirbytes;i++)
		realsnd[i]=sndhir[i];
	    }
	    for (i=0;i<nbits;i++)
	      realsnd[(i+hirbits)/8] |= (snd[i/8] >> (i%8)) << ((i+hirbits)%8);
	    if (tirbytes>0) {
	      for (i=0;i<tirbits;i++)
		realsnd[(i+hirbits+nbits)/8] |= (sndtir[i/8] >> (i%8)) << ((i+hirbits+nbits)%8);
	    }
	    //
	    tmb_->scan(INSTR_REG, (char*)realsnd, hirbits+nbits+tirbits, (char*)rcv, 0); 
	    //

	    if (db>6) { 	printf("SIR Send Data:\n");
	    for (i=0; i< ((hirbits+nbits+tirbits-1)/8+1);  i++)
	      printf("%02X",realsnd[i]);
	    printf("\n");
	    }
	    //	  jtag_load_(&fd, &nframes, tms_pre_sir, sndbuf, rcvbuf, &step_mode);
	    if (cmpflag==1)
	      {
		/*               for(i=0;i<nbytes;i++)
				 {
				 rcv_tmp = 0;
				 for(j=0;j<8;j++)
				 {
				 if ((i*8+j) < nbits)
				 {
				 rcv_tmp |= ((rcvbuf[i*8+j+4]<<7) &0x80);
				 }
				 rcv_tmp = rcv_tmp >> (j<7);
				 
				 }
				 rcv[nbytes-1-i] = rcv_tmp;
				 }
		*/
		if (db>4){                printf("SIR Readback Data:\n");
                //for(i=0;i<nbytes;i++) printf("%02X",rcv[i]);
		for (i=0; i< ((hdrbits+nbits+tdrbits-1)/8+1); i++) printf("%02X",rcv[i]);
                printf("\n");
		}
		
                for(i=0;i<nbytes;i++)
		  {
		    rcvword = rcv[i+(hirbits/8)]+(((int)rcv[i+1+(hirbits/8)])<<8);
		    rcvword = rcvword>>(hirbits%8);
		    rcvword = rcv[i];
		    // if (((rcv[nbytes-1-i]^expect[i]) & (rmask[i]))!=0 && cmpflag==1)
		    if ((((rcvword&0xFF)^expect[i]) & (rmask[i]))!=0 && cmpflag==1)
		      {
			printf("2.read back wrong, at i %02d  rdbk %02X  expect %02X  rmask %02X\n",i,rcv[i]&0xFF,expect[i]&0xFF,rmask[i]&0xFF);
                	errcntr++;
		      }
		  }
		
	      }
	    /*
	      if (cmpflag==1)
	      {
	      for(i=4;i<nbits+4;i++) printf("%d",rcvbuf[i]);
	      printf("\n");
	      }
	    */   
          }
	// === Handling RUNTEST ===
	  else if(strcmp(Word[0],"RUNTEST")==0)
	    {
	      sscanf(Word[1],"%d",&pause);
	      //printf("RUNTEST:  %d\n",pause);
	      usleep(pause+100);
	      // InsertDelayJTAG(pause,MYMICROSECONDS);
	    }
	  // === Handling STATE ===
	  else if((strcmp(Word[0],"STATE")==0)&&(strcmp(Word[1],"RESET")==0)&&(strcmp(Word[2],"IDLE;")==0))
	    {
	      //          printf("STATE: goto reset idle state\n");
	      // Put send STATE RESET here
          }
	  // === Handling TRST ===
	  else if(strcmp(Word[0],"TRST")==0)
	    {
	      //          printf("TRST\n");
          }
	// === Handling ENDIR ===
        else if(strcmp(Word[0],"ENDIR")==0)
	  {
	    //          printf("ENDIR\n");
          }
	// === Handling ENDDR ===
        else if(strcmp(Word[0],"ENDDR")==0)
	  {
	    //	   printf("ENDDR\n");
          }
	}
      }
    alct_end();    
    fclose(dwnfp);
    return errcntr; 
}


// ====
// SVF File Parser module
// ====
void ALCTController::Parse(char *buf,int *Count,char **Word)
{

  *Word = buf;
  *Count = 0;
  while(*buf != '\0')  {
    while ((*buf==' ') || (*buf=='\t') || (*buf=='\n') || (*buf=='"')) *(buf++)='\0';
    if ((*buf != '\n') && (*buf != '\0'))  {
      Word[(*Count)++] = buf;

    }
    while ((*buf!=' ')&&(*buf!='\0')&&(*buf!='\n')&&(*buf!='\t')&&(*buf!='"')) {
      buf++;
    }
  }
  buf = '\0';
}



/*
**  TRIM.C - Remove leading, trailing, & excess embedded spaces
**
**  public domain by Bob Stout
*/


#define NUL '\0'

char *strtolower(char *str)
{	
	char *ibuf = str;
	if (str)
	{
		for (ibuf = str; *ibuf; ++ibuf)
			if (isalpha(*ibuf) && isupper(*ibuf)) *ibuf = tolower(*ibuf);
	}
	return str;
}

char *trim(char *str)
{
      char *ibuf = str, *obuf = str;
      int i = 0; 
      int cnt = 0;

      /*
      **  Trap NULL
      */

      if (str)
      {
            /*
            **  Remove leading spaces (from RMLEAD.C)
            */

            for (ibuf = str; *ibuf && isspace(*ibuf); ++ibuf)
                  ;
            if (str != ibuf)
                  memmove(str, ibuf, ibuf - str);

            /*
            **  Collapse embedded spaces (from LV1WS.C)
            */

            while (*ibuf)
            {
                  if ((isspace(*ibuf)||(*ibuf=='=')) && cnt)
//                 if (isspace(*ibuf))
                        ibuf++;
                  else
                  {
                        if (!isspace(*ibuf)&&(*ibuf!='='))
                              cnt = 0;
                        else
                        {
                              *ibuf = ' ';
                              cnt = 1;
                        }
                        obuf[i++] = *ibuf++;
                  }
            }
            obuf[i] = NUL;

            /*
            **  Remove trailing spaces (from RMTRAIL.C)
            */
	
            while (--i >= 0)
            {
                  if (!isspace(obuf[i]))
                        break;
            }
            obuf[++i] = NUL;
      }
      return str;
}

#else
//////////////////////////////////////////////////////////////////////////////////////
//  From here to end is new ALCTController...
//////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <sstream>
#include <iomanip>
#include <unistd.h> // for sleep
#include <vector>
#include <string>
//
#include "ALCTController.h"
#include "EMUjtag.h"
#include "TMB.h"
//
//
ALCTController::ALCTController(TMB * tmb, std::string chamberType) :
  EMUjtag(tmb)
{
  //
  MyOutput_ = &std::cout ;
  (*MyOutput_) << "Creating ALCTController" << std::endl;
  //
  tmb_ = tmb;
  SetChamberCharacteristics_(chamberType);
  //
  SetPowerUpTestpulsePowerSwitchReg_();
  SetPowerUpTestpulseAmplitude_();
  SetPowerUpTestpulseGroupMask_();
  SetPowerUpTestpulseStripMask_();
  SetPowerUpAfebThresholds();
  SetPowerUpStandbyRegister_();
  //
  SetPowerUpDelayLineControlReg_();
  SetPowerUpTriggerRegister_();
  SetPowerUpAsicDelays();
  SetPowerUpAsicPatterns();
  SetPowerUpConfigurationReg();
  SetPowerUpHotChannelMask();
  //
  SetCheckJtagWrite(true);
  stop_read_ = false;
  //
}
//
ALCTController::~ALCTController() {
  //
  //
}
//
//
///////////////////////////////////////////////////////////////////
// Useful methods to use ALCTController...
///////////////////////////////////////////////////////////////////
void ALCTController::SetUpPulsing(int DAC_pulse_amplitude, 
				  int which_set,
				  int mask,
				  int source){
  //
  //  long int StripMask = 0x3f;
  //  long int PowerUp   = 1 ;
  //
  //  int slot=tmb_->slot();
  //
  (*MyOutput_) << "Set up ALCT (slot " << tmb_->slot() 
	       << ") for pulsing: Amplitude=" << std::dec << DAC_pulse_amplitude << std::endl;
  //
  //
  //  alct_set_test_pulse_powerup(&slot,0);
  SetTestpulsePowerSwitchReg_(OFF);
  WriteTestpulsePowerSwitchReg_();
  PrintTestpulsePowerSwitchReg_();  
  //
  //  usleep(100);   // included into WriteTestpulsePowerSwitchReg();
  //
  //  alct_set_test_pulse_amp(&slot,Amplitude);
  SetTestpulseAmplitude_(DAC_pulse_amplitude);
  WriteTestpulseAmplitude_();
  //
  // usleep(100);    // included into WriteTestpulseAmplitude_();
  //
  //  alct_read_test_pulse_stripmask(&slot,&StripMask);
  //  std::cout << " StripMask = " << std::hex << StripMask << std::endl;
  //  ReadTestpulseStripMask_();
  //  PrintTestpulseStripMask_();
  //
  //  if(StripAfeb == 0 ) {
  //    alct_set_test_pulse_stripmask(&slot,0x00);
  //    alct_set_test_pulse_groupmask(&slot,0xff);
  //  }
  //  else if (StripAfeb == 1 ) {
  //    alct_set_test_pulse_stripmask(&slot,stripMask);
  //    alct_set_test_pulse_groupmask(&slot,0x00);
  //  } else {
  //    std::cout << "ALCTcontroller.SetUpPulsing : Don't know this option" <<std::endl;
  //  }
  //
  for (int group=0; group<GetNumberOfGroupsOfDelayChips(); group++)
    SetTestpulseGroupMask_(group,OFF);
  for (int layer=0; layer<MAX_NUM_LAYERS; layer++) 
    SetTestpulseStripMask_(layer,OFF);
  //
  // Choose whether you are pulsing layers with teststrips or AFEBS in groups...
  if (which_set==PULSE_AFEBS) {
    for (int group=0; group<GetNumberOfGroupsOfDelayChips(); group++) {
      int off_or_on = (mask >> group) & 0x1;
      SetTestpulseGroupMask_(group,off_or_on);    
    }
  } else if (which_set==PULSE_LAYERS) {
    for (int layer=0; layer<MAX_NUM_LAYERS; layer++) {
      int off_or_on = (mask >> layer) & 0x1;
      SetTestpulseStripMask_(layer,off_or_on);
    }
  } else {
    std::cout << "ALCTcontroller SetUpPulsing : Set " << which_set 
	      << " not available to pulse..." << std::endl;
  }
  WriteTestpulseGroupMask_();
  PrintTestpulseGroupMask_();
  //
  WriteTestpulseStripMask_();
  PrintTestpulseStripMask_();
  //
  //  alct_read_test_pulse_powerup(&slot,&PowerUp);
  //  std::cout << " PowerUp   = " << std::hex << PowerUp << std::dec << std::endl; //11July05 DM added dec
  //  ReadTestpulsePowerSwitchReg();
  //  PrintTestpulsePowerSwitchReg();
  //
  //  alct_fire_test_pulse('s');
  SetPulseTriggerSource_(source);
  SetInvertPulse_(OFF);
  WriteTriggerRegister_();
  PrintTriggerRegister_();
  //
  //  usleep(100);  // included into WriteTriggerRegister();
  //
  //  alct_set_test_pulse_powerup(&slot,1);
  SetTestpulsePowerSwitchReg_(ON);
  WriteTestpulsePowerSwitchReg_();
  //
  //  usleep(100); // included into WriteTestpulsePowerSwitchReg();
  //
  //  alct_read_test_pulse_powerup(&slot,&PowerUp);
  //  std::cout << " PowerUp   = " << std::hex << PowerUp << std::dec << std::endl; //11July05 DM added dec
  PrintTestpulsePowerSwitchReg_();  
  //
  return;
}
//
//
void ALCTController::SetUpRandomALCT(){
  //
  //  unsigned long HCmask[22];
  //  unsigned long HCmask2[22];
  //
  //    for (int i=0; i< 22; i++) {
  //    HCmask[i] = 0;
  //    HCmask2[i] = 0;
  //  }
  //
  //  int keyWG  = int(rand()/(RAND_MAX+0.01)*(GetWGNumber())/6/4);
  //  int keyWG2 = (GetWGNumber())/6-keyWG;
  //  int ChamberSection = GetWGNumber()/6;
  int keyWG  = int(rand()/(RAND_MAX+0.01)*(GetNumberOfChannelsInAlct())/6/4);
  int keyWG2 = (GetNumberOfChannelsInAlct())/6-keyWG;
  //
  printf("Injecting at %d and %d\n",keyWG,keyWG2);
  //
  //  for (int i=0; i< 22; i++) HCmask[i] = 0;
  //  //
  //  std::bitset<672> bits(*HCmask) ;
  //  //
  //  for (int i=0;i<672;i++){
  //    if ( i%(GetWGNumber()/6) == keyWG ) bits.set(i);
  //    if ( i%(GetWGNumber()/6) == (GetWGNumber())/6-keyWG ) bits.set(i);
  //  }
  //  //
  //  std::bitset<32> Convert;
  //  //
  //  Convert.reset();
  //  //
  //  for (int i=0;i<(GetWGNumber());i++){
  //    if ( bits.test(i) ) Convert.set(i%32);
  //    if ( i%32 == 31 ) {
  //      HCmask[i/32] = Convert.to_ulong();
  //      Convert.reset();
  //    }
  //  }
  //
  //  alct_write_hcmask(HCmask);
  //  for(int i=0; i<22; i++) std::cout << std::hex << HCmask[i] << std::endl;
  //  alct_read_hcmask(HCmask);
  //  std::cout << std::endl;
  //  for(int i=0; i<22; i++) std::cout << std::hex << HCmask2[i] << std::endl;
    for(int layer=0; layer<MAX_NUM_LAYERS; layer++) {
      for(int channel=0; channel<GetNumberOfChannelsInAlct()/6; channel++) {
	if (channel==keyWG || channel==keyWG2) {
	  SetHotChannelMask(layer,channel,ON);
	} else {
	  SetHotChannelMask(layer,channel,OFF);
	}
      }
    }
    WriteHotChannelMask();
    //    ReadHotChannelMask();
    PrintHotChannelMask();
  //
  return;
}
//
//
void ALCTController::configure() {
  //
  std::ostringstream dump;
  dump << (int)tmb_->slot();
  //
  tmb_->SendOutput("ALCT : configure() in slot = "+dump.str(),"INFO");
  (*MyOutput_) << "Configure ALCT..." << std::endl;
  //
  ReadSlowControlId();
  PrintSlowControlId();
  //
  WriteTestpulsePowerSwitchReg_();
  PrintTestpulsePowerSwitchReg_();
  //
  WriteTestpulseAmplitude_();
  //
  WriteTestpulseGroupMask_();
  PrintTestpulseGroupMask_();
  //
  WriteTestpulseStripMask_();
  PrintTestpulseStripMask_();
  //
  WriteAfebThresholds();
  ReadAfebThresholds();
  PrintAfebThresholds();
  //    
  WriteStandbyRegister_();
  PrintStandbyRegister_();
  //
  //
  ReadFastControlId();
  PrintFastControlId();
  //
  WriteDelayLineControlReg_();
  PrintDelayLineControlReg_();
  //
  WriteTriggerRegister_();
  PrintTriggerRegister_();
  //
  WriteAsicDelaysAndPatterns();
  PrintAsicDelays();
  PrintAsicPatterns();
  //
  WriteConfigurationReg();
  PrintConfigurationReg();
  //
  WriteHotChannelMask();
  PrintHotChannelMask();
  //
  return;
}
///////////////////////////////////////////////////////////////////
// END:  Useful methods to use ALCTController...
///////////////////////////////////////////////////////////////////
//
///////////////////////
//SLOW CONTROL ID
///////////////////////
void ALCTController::ReadSlowControlId() {
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
		 read_slowcontrol_id_);
  return;
}
//
void ALCTController::PrintSlowControlId() {
  //
  (*MyOutput_) << "ALCT READ: Slow Control chip ID = " << std::hex << GetSlowControlChipId()
	       << " version " << GetSlowControlVersionId()
	       << ": day = " << GetSlowControlDay()
	       << ", month = " << GetSlowControlMonth()
	       << ", year = " << GetSlowControlYear()
	       << std::dec << std::endl; 
  return;
}
//
int ALCTController::GetSlowControlChipId() { 
  return (read_slowcontrol_id_[0] & 0xf); 
}
//
int ALCTController::GetSlowControlVersionId() { 
  return ((read_slowcontrol_id_[0]>>4) & 0xf); 
}
//
int ALCTController::GetSlowControlYear() { 
  return ((read_slowcontrol_id_[2]<<8) | read_slowcontrol_id_[1]&0xff); 
}
//
int ALCTController::GetSlowControlDay() { 
  return (read_slowcontrol_id_[3] & 0xff); 
}
//
int ALCTController::GetSlowControlMonth() { 
  return (read_slowcontrol_id_[4] & 0xff); 
}
//
//////////////////////////////////
//TESTPULSE POWERSWITCH REGISTER
//////////////////////////////////
void ALCTController::WriteTestpulsePowerSwitchReg_() {
  (*MyOutput_) << "ALCT: WRITE testpulse POWERSWITCH" << std::endl;
  //
  setup_jtag(ChainAlctSlowFpga);
  //
  ShfIR_ShfDR(ChipLocationAlctSlowFpga,
	      ALCT_SLOW_WRT_TESTPULSE_POWERDOWN,
	      RegSizeAlctSlowFpga_WRT_TESTPULSE_POWERDOWN,
	      &write_testpulse_power_setting_);
  usleep(100);
  //
  if ( GetCheckJtagWrite() ) {
    ReadTestpulsePowerSwitchReg_();
    CompareBitByBit(&write_testpulse_power_setting_,
		    &read_testpulse_power_setting_,
		    RegSizeAlctSlowFpga_WRT_TESTPULSE_POWERDOWN);
  }
  //
  return;
}
//
void ALCTController::ReadTestpulsePowerSwitchReg_() {
  //
  (*MyOutput_) << "ALCT: READ testpulse POWERSWITCH" << std::endl;
  //
  setup_jtag(ChainAlctSlowFpga);
  //
  ShfIR_ShfDR(ChipLocationAlctSlowFpga,
	      ALCT_SLOW_RD_TESTPULSE_POWERDOWN,
	      RegSizeAlctSlowFpga_RD_TESTPULSE_POWERDOWN);
  //
  read_testpulse_power_setting_ = bits_to_int(GetDRtdo(),
					      GetRegLength(),
					      LSBfirst);
  return;
}
//
void ALCTController::PrintTestpulsePowerSwitchReg_() {
  //
  (*MyOutput_) << "ALCT READ: Testpulse powerswitch = " << std::dec
	       << GetTestpulsePowerSwitchReg_() << std::endl;
  return;
}
//
void ALCTController::SetTestpulsePowerSwitchReg_(int powerswitch) { 
  //
  write_testpulse_power_setting_ = powerswitch & 0x1; 
  return;
}
//
int ALCTController::GetTestpulsePowerSwitchReg_() { 
  //
  return read_testpulse_power_setting_; 
}
//
void ALCTController::SetPowerUpTestpulsePowerSwitchReg_() { 
  //
  SetTestpulsePowerSwitchReg_(OFF); 
  return;
}
//
////////////////////////////////
//TESTPULSE AMPLITUDE REGISTER
////////////////////////////////
void ALCTController::WriteTestpulseAmplitude_() {
  (*MyOutput_) << "ALCT: WRITE testpulse AMPLITUDE = " 
  	       << write_testpulse_amplitude_dacvalue_ << std::endl;
  //
  int dac[RegSizeAlctSlowFpga_WRT_TESTPULSE_DAC] = {};
  //
  //DAC's require MSB sent first....
  int_to_bits(write_testpulse_amplitude_dacvalue_,
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
  usleep(100);
  //
  return;
}
//
void ALCTController::SetTestpulseAmplitude_(int dacvalue) {
  // 8-bit DAC controls amplitude of analog test pulse sent to AFEBs
  //  Voltage = 2.5V * dacvalue/256
  //
  if (dacvalue > 255) {
    (*MyOutput_) << "ALCT: ERROR testpulse amplitude maximum = 255 " << std::endl;
    return;
  } 
  //
  write_testpulse_amplitude_dacvalue_ = dacvalue & 0xff;
  //
  return;
}
//
void ALCTController:: SetPowerUpTestpulseAmplitude_() { 
  //
  SetTestpulseAmplitude_(0); 
  return;
}
//
////////////////////////////////
//TESTPULSE GROUPMASK REGISTER
////////////////////////////////
void ALCTController::WriteTestpulseGroupMask_() {
  //
  (*MyOutput_) << "ALCT: WRITE testpulse GROUPMASK" << std::endl;
  //
  setup_jtag(ChainAlctSlowFpga);
  //
  ShfIR_ShfDR(ChipLocationAlctSlowFpga,
	      ALCT_SLOW_WRT_TESTPULSE_GRP,
	      RegSizeAlctSlowFpga_WRT_TESTPULSE_GRP,
	      write_testpulse_groupmask_);
  //
  if ( GetCheckJtagWrite() ) {
    ReadTestpulseGroupMask_();
    CompareBitByBit(write_testpulse_groupmask_,
		    read_testpulse_groupmask_,
		    RegSizeAlctSlowFpga_WRT_TESTPULSE_GRP);
  }
  //
  return;
}
//
void ALCTController::ReadTestpulseGroupMask_() {
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
    read_testpulse_groupmask_[i] = *(register_pointer+i);
  //
  return;
}
//
void ALCTController::PrintTestpulseGroupMask_() {
  //
  int testpulse_groupmask = bits_to_int(read_testpulse_groupmask_,
					RegSizeAlctSlowFpga_RD_TESTPULSE_GRP,
					LSBfirst);
  //
  (*MyOutput_) << "ALCT READ: Testpulse groupmask = 0x" 
	       << std::hex << testpulse_groupmask << std::endl;
  return;
}
//
void ALCTController::SetTestpulseGroupMask_(int group,  
					    int mask) { 
  //
  // specify which groups of AFEBs are enabled for analog testpulsing
  //
  if (group < 0 || group >= GetNumberOfGroupsOfDelayChips()) {
    (*MyOutput_) << "SetTestpulseGroupMask: ERROR group value must be between 0 and " 
		 << std::dec << GetNumberOfGroupsOfDelayChips()-1 << std::endl;
    return;
  } 
  write_testpulse_groupmask_[group] = mask & 0x1;
  //
  return;
}
//
int ALCTController::GetTestpulseGroupMask_(int group) {
  return read_testpulse_groupmask_[group]; 
}
//
void ALCTController::SetPowerUpTestpulseGroupMask_() {
  //
  for (int group=0; group<RegSizeAlctSlowFpga_WRT_TESTPULSE_GRP; group++)
    write_testpulse_groupmask_[group] = OFF;
  return;
}
//
/////////////////////////////////
//TESTPULSE STRIPMASK REGISTER
/////////////////////////////////
void ALCTController::WriteTestpulseStripMask_() {
  //
  (*MyOutput_) << "ALCT: WRITE testpulse STRIPMASK" << std::endl;
  //
  setup_jtag(ChainAlctSlowFpga);
  //
  ShfIR_ShfDR(ChipLocationAlctSlowFpga,
	      ALCT_SLOW_WRT_TESTPULSE_STRIP,
	      RegSizeAlctSlowFpga_WRT_TESTPULSE_STRIP,
	      write_testpulse_stripmask_);
  //
  if ( GetCheckJtagWrite() ) {
    ReadTestpulseStripMask_();
    CompareBitByBit(write_testpulse_stripmask_,
		    read_testpulse_stripmask_,
		    RegSizeAlctSlowFpga_WRT_TESTPULSE_STRIP);
  }
  //
  return;
}
//
void ALCTController::ReadTestpulseStripMask_() {
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
    read_testpulse_stripmask_[i] = *(register_pointer+i);
  //
  return;
}
//
void ALCTController::PrintTestpulseStripMask_() {
  //
  int testpulse_stripmask = bits_to_int(read_testpulse_stripmask_,
					RegSizeAlctSlowFpga_RD_TESTPULSE_STRIP,
					LSBfirst);
  //
  (*MyOutput_) << "ALCT READ: testpulse stripmask = 0x" 
	       << std::hex << testpulse_stripmask << std::endl;
  return;
}
//
void ALCTController::SetTestpulseStripMask_(int layer,
					   int mask) {
  //
  if (layer < 0 || layer >= MAX_NUM_LAYERS) {
    (*MyOutput_) << "SetTestpulseStripMask: ERROR AFEB value must be between 0 and " 
		 << std::dec << MAX_NUM_LAYERS-1 << std::endl;
    return;
  } 
  //
  write_testpulse_stripmask_[layer] = mask & 0x1;
  //
  return;
}
//
int ALCTController::GetTestpulseStripMask_(int afeb) {
  //
  if (afeb < 0 || afeb >= MAX_NUM_LAYERS) {
    (*MyOutput_) << "GetTestpulseStripMask: ERROR AFEB value must be between 0 and " 
		 << std::dec << MAX_NUM_LAYERS-1 << std::endl;
    return 999;
  } 
  //
  return read_testpulse_stripmask_[afeb]; 
}
//
void ALCTController::SetPowerUpTestpulseStripMask_() {
  //
  for (int afeb=0; afeb<MAX_NUM_LAYERS; afeb++)
    write_testpulse_stripmask_[afeb] = OFF;
  //
  return;
}
//
//////////////////
//AFEB THRESHOLDS
//////////////////
void ALCTController::WriteAfebThresholds() {
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
void ALCTController::ReadAfebThresholds() {
  //
  (*MyOutput_) << "ALCT: READ afeb THRESHOLDS " << std::endl;
  //
  for (int afeb=0; afeb<GetNumberOfAfebs(); afeb++)
    read_afeb_threshold_[afeb] = read_adc_(afeb_adc_chip[afeb],afeb_adc_channel[afeb]);
  //
  return;
}
//
void ALCTController::PrintAfebThresholds() {
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
void ALCTController::SetAfebThreshold(int afebChannel, int dacvalue) {
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
  write_afeb_threshold_[afebChannel] = dacvalue & 0xff;
  //
  return;
}
//
int ALCTController::GetAfebThresholdDAC(int afebChannel) { 
  //
  return write_afeb_threshold_[afebChannel]; 
}
//
int ALCTController::GetAfebThresholdADC(int afebChannel) { 
  //   
  return read_afeb_threshold_[afebChannel]; 
}
//
float ALCTController::GetAfebThresholdVolts(int afebChannel) {
  return (2.5 * (float)GetAfebThresholdADC(afebChannel) /1023.); 
}
//
int ALCTController::read_adc_(int chip, int channel) {
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
void ALCTController::SetPowerUpAfebThresholds() {
  //
  for (int afeb=0; afeb<MAX_NUM_AFEBS; afeb++) 
    write_afeb_threshold_[afeb] = 128;
  //
  return;
}
//
////////////////////////
//STANDBY REGISTER
////////////////////////
void ALCTController::WriteStandbyRegister_() {
  //
  (*MyOutput_) << "ALCT: WRITE standby register" << std::endl;
  //
  setup_jtag(ChainAlctSlowFpga);  
  //
  ShfIR_ShfDR(ChipLocationAlctSlowFpga,
	      ALCT_SLOW_WRT_STANDBY_REG,
	      RegSizeAlctSlowFpga_WRT_STANDBY_REG,
	      write_standby_register_);
  usleep(100);
  //
  if ( GetCheckJtagWrite() ) {
    ReadStandbyRegister_();
    CompareBitByBit(write_standby_register_,
		    read_standby_register_,
		    RegSizeAlctSlowFpga_WRT_STANDBY_REG);
  }
  //
  return;
}
//
void ALCTController::ReadStandbyRegister_() {
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
    read_standby_register_[i] = *(standby_reg_pointer+i);
  //
  return;
}
//
void ALCTController::PrintStandbyRegister_() {
  //
  const int buffersize = RegSizeAlctSlowFpga_RD_STANDBY_REG/8;
  char tempBuffer[buffersize] = {};
  packCharBuffer(read_standby_register_,
		 RegSizeAlctSlowFpga_RD_STANDBY_REG,
		 tempBuffer);
  //
  (*MyOutput_) << "ALCT READ: Standby Register (right to left)= ";
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
void ALCTController::SetStandbyRegister_(int afebChannel, int powerswitch) {
  //
  if (afebChannel >= GetNumberOfAfebs()) {
    (*MyOutput_) << "Set Standby Register: ALCT" << std::dec << GetNumberOfChannelsInAlct() 
		 << "-> channel " << std::dec << afebChannel
		 << " invalid ... must be between 0 and " << std::dec << GetNumberOfAfebs()-1 
		 << std::endl;
    return;
  } 
  //
  write_standby_register_[afebChannel] = powerswitch & 0x1;
  //
  return;
}
//
int ALCTController::GetStandbyRegister_(int afebChannel) {
  //
  return read_standby_register_[afebChannel]; 
}
//
void ALCTController::SetPowerUpStandbyRegister_() {
  //
  for (int afeb=0; afeb<MAX_NUM_AFEBS; afeb++) 
    write_standby_register_[afeb] = ON;               // default for data taking
  //
  return;
}
//
//////////////////////////////
// FAST CONTROL ID
//////////////////////////////
void ALCTController::ReadFastControlId() {
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
		 read_fastcontrol_id_);
  return;
}
//
void ALCTController::PrintFastControlId() {
  //
  (*MyOutput_) << "ALCT READ: Fast Control chip ID = " << std::hex << GetFastControlChipId()
	       << " version " << GetFastControlVersionId()
	       << ": day = " << GetFastControlDay()
	       << ", month = " << GetFastControlMonth()
	       << ", year = " << GetFastControlYear()
	       << std::dec << std::endl; 
  return;
}
//
int ALCTController::GetFastControlChipId() { 
  return (read_fastcontrol_id_[0] & 0xf); 
}
//
int ALCTController::GetFastControlVersionId() { 
  return ((read_fastcontrol_id_[0]>>4) & 0xf); 
}
//
int ALCTController::GetFastControlYear() { 
  return ((read_fastcontrol_id_[2]<<8) | read_fastcontrol_id_[1]&0xff); 
}
//
int ALCTController::GetFastControlDay() { 
  return (read_fastcontrol_id_[3] & 0xff); 
}
//
int ALCTController::GetFastControlMonth() { 
  return (read_fastcontrol_id_[4] & 0xff); 
}
//
////////////////////////////////
// TESTPULSE TRIGGER REGISTER
////////////////////////////////
void ALCTController::WriteTriggerRegister_() {
  //
  (*MyOutput_) << "ALCT: WRITE TRIGGER REGISTER" << std::endl;
  //
  FillTriggerRegister_();
  //
  setup_jtag(ChainAlctFastFpga);
  //
  ShfIR_ShfDR(ChipLocationAlctFastFpga,
	      ALCT_FAST_WRT_TRIG_REG,
	      RegSizeAlctFastFpga_WRT_TRIG_REG,
	      write_trigger_reg_);
  usleep(100);
  //
  if ( GetCheckJtagWrite() ) {
    ReadTriggerRegister_();
    CompareBitByBit(write_trigger_reg_,
		    read_trigger_reg_,
		    RegSizeAlctFastFpga_WRT_TRIG_REG);
  }
  //
  return;
}
//
void ALCTController::ReadTriggerRegister_() {
  //
  setup_jtag(ChainAlctFastFpga);
  //
  ShfIR_ShfDR(ChipLocationAlctFastFpga,
	      ALCT_FAST_RD_TRIG_REG,
	      RegSizeAlctFastFpga_RD_TRIG_REG);
  //
  int * register_pointer = GetDRtdo();
  for (int i=0; i<RegSizeAlctFastFpga_RD_TRIG_REG; i++)
    read_trigger_reg_[i] = *(register_pointer+i);
  //
  //Print out control register value in hex
  //  int scaler_trigger_reg = bits_to_int(trigger_reg_,
  //				       RegSizeAlctFastFpga_RD_TRIG_REG,
  //				       LSBfirst);
  //  (*MyOutput_) << "ALCT: READ TRIGGER REGISTER = " 
  //  	       << std::hex << scaler_trigger_reg << std::dec << std::endl;

  DecodeTriggerRegister_();
  //
  return;
}
//
void ALCTController::SetPulseTriggerSource_(int source) {
  // Specify which signal will fire the testpulse
  // N.B. The trigger source is a combination of the bits in [0-1] and [2-3]
  //
  write_pulse_trigger_source_ = source & 0xf;
  //
  return;
}
//
void ALCTController::SetInvertPulse_(int mask) {
  // bit = 1 = invert
  //     = 0 = not inverted
  //
  write_invert_pulse_ = mask & 0x1; 
  //
  return;
}  
//
int ALCTController::GetPulseTriggerSource_() {
  //
  return read_pulse_trigger_source_;
}
//
int ALCTController::GetInvertPulse_() {
  //
  return read_invert_pulse_; 
}  
//
void ALCTController::DecodeTriggerRegister_() {
  // ** Extract the trigger register's software values  **
  // ** from the vector of bits read_trigger_reg_[]          **
  //
  int number_of_bits = trigger_register_source_bithi - trigger_register_source_bitlo + 1;  
  read_pulse_trigger_source_ = bits_to_int(read_trigger_reg_+trigger_register_source_bitlo,
					   number_of_bits,
					   LSBfirst);
  //
  number_of_bits = trigger_register_invert_bithi - trigger_register_invert_bitlo + 1;  
  read_invert_pulse_ = bits_to_int(read_trigger_reg_+trigger_register_invert_bitlo,
				   number_of_bits,
				   LSBfirst);
  //
  return;
}
//
void ALCTController::FillTriggerRegister_() {
  // ** Project the trigger register's software values  **
  // ** into the vector of bits write_trigger_reg_[]          **
  //
  int_to_bits(write_pulse_trigger_source_,
	      trigger_register_source_bithi - trigger_register_source_bitlo + 1,
	      write_trigger_reg_ + trigger_register_source_bitlo,
	      LSBfirst);
  //
  int_to_bits(write_invert_pulse_,
	      trigger_register_invert_bithi - trigger_register_invert_bitlo + 1,
	      write_trigger_reg_ + trigger_register_invert_bitlo,
	      LSBfirst);
  //
  return;
}
//
void ALCTController::PrintTriggerRegister_() {
  //
  (*MyOutput_) << "ALCT Testpulse Trigger Register:" << std::endl;
  (*MyOutput_) << "--------------------------------" << std::endl;
  (*MyOutput_) << " trigger source = ";
  if (read_pulse_trigger_source_ == OFF) (*MyOutput_) << "OFF" << std::endl;
  if (read_pulse_trigger_source_ == SELF) (*MyOutput_) << "SELF" << std::endl;
  if (read_pulse_trigger_source_ == ADB_SYNC) (*MyOutput_) << "ADB_SYNC" << std::endl;
  if (read_pulse_trigger_source_ == ADB_ASYNC) (*MyOutput_) << "ADB_ASYNC" << std::endl;
  if (read_pulse_trigger_source_ == LEMO) (*MyOutput_) << "LEMO" << std::endl;
  //
  (*MyOutput_) << " trigger invert =" << std::dec << read_invert_pulse_ << std::endl;
  //
  return;
}
//
void ALCTController::SetPowerUpTriggerRegister_(){
  //
  SetPulseTriggerSource_(OFF);
  SetInvertPulse_(OFF);
  //
  return;
}
//
////////////////////////////////
// DELAY LINE CONTROL REGISTER
////////////////////////////////
void ALCTController::WriteDelayLineControlReg_() {
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
	      write_delay_line_control_reg_);
  //
  if ( GetCheckJtagWrite() ) {
    ReadDelayLineControlReg_();
    CompareBitByBit(write_delay_line_control_reg_,
		    read_delay_line_control_reg_,
		    RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_);
  }
  //
  return;
}
//
void ALCTController::ReadDelayLineControlReg_() {
  //
  setup_jtag(ChainAlctFastFpga);
  //
  ShfIR_ShfDR(ChipLocationAlctFastFpga,
	      ALCT_FAST_RD_DELAYLINE_CTRL_REG,
	      RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_);
  //
  int * register_pointer = GetDRtdo();
  for (int i=0; i<RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_; i++)
    read_delay_line_control_reg_[i] = *(register_pointer+i);
  //
  //Print out control register value in hex
  //  int scaler_delay_line_control_reg = bits_to_int(read_delay_line_control_reg_,
  //    						  RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_,
  //    						  LSBfirst);
  //  (*MyOutput_) << "ALCT READ: delay line CONTROL REGISTER = " 
  //    	       << std::hex << scaler_delay_line_control_reg << std::dec << std::endl;
  //
  DecodeDelayLineControlReg_();
  //
  return;
}
//
void ALCTController::SetDelayLineGroupSelect_(int group,
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
  write_delay_line_group_select_[group] = ~mask & 0x1;
  //
  return;
}
//
void ALCTController::SetDelayLineSettst_(int mask) {
  // bit = 1 = not enabled
  //     = 0 = enabled
  //
  write_delay_line_settst_ = ~mask & 0x1; 
  return;
}  
//
void ALCTController::SetDelayLineReset_(int mask) { 
  // bit = 1 = not enabled
  //     = 0 = enabled
  //
  write_delay_line_reset_ = ~mask & 0x1; 
  return;
}
//
void ALCTController::DecodeDelayLineControlReg_() {
  // ** Extract the delay line control register's software values  **
  // ** from the vector of bits read_delay_line_control_reg_[]     **
  //
  int number_of_bits = delay_line_reset_bithi - delay_line_reset_bitlo + 1;  
  read_delay_line_reset_ = bits_to_int(read_delay_line_control_reg_+delay_line_reset_bitlo,
				       number_of_bits,
				       LSBfirst);
  //
  number_of_bits = delay_line_settst_bithi - delay_line_settst_bitlo + 1;  
  read_delay_line_settst_ = bits_to_int(read_delay_line_control_reg_+delay_line_settst_bitlo,
					number_of_bits,
					LSBfirst);
  //
  for (int index=0; index<GetNumberOfGroupsOfDelayChips(); index++)
    read_delay_line_group_select_[index] = read_delay_line_control_reg_[index+delay_line_group_select_bitlo];
  //
  return;
}
//
void ALCTController::FillDelayLineControlReg_() {
  // ** Project the delay line control register's software values  **
  // ** into the vector of bits write_delay_line_control_reg_[]    **
  //
  int_to_bits(write_delay_line_reset_,
	      delay_line_reset_bithi-delay_line_reset_bitlo+1,
	      write_delay_line_control_reg_+delay_line_reset_bitlo,
	      LSBfirst);
  //
  int_to_bits(write_delay_line_settst_,
	      delay_line_settst_bithi-delay_line_settst_bitlo+1,
	      write_delay_line_control_reg_+delay_line_settst_bitlo,
	      LSBfirst);
  //
  for (int index=0; index<GetNumberOfGroupsOfDelayChips(); index++) 
    write_delay_line_control_reg_[index+delay_line_group_select_bitlo] 
      = write_delay_line_group_select_[index];
  //
  return;
}
//
void ALCTController::PrintDelayLineControlReg_() {
  //
  (*MyOutput_) << "ALCT Delay Line Control Register:" << std::endl;
  (*MyOutput_) << "----------------------------------" << std::endl;
  (*MyOutput_) << "read_delay_line_reset_  = " << std::dec 
	       << read_delay_line_reset_ << std::endl;
  (*MyOutput_) << "read_delay_line_settst_ = " << std::dec 
	       << read_delay_line_settst_ << std::endl;
  for (int index=0; index<GetNumberOfGroupsOfDelayChips(); index++) 
    (*MyOutput_) << "read_delay_line_group_select_[" << std::dec << index 
		 << "] = " << read_delay_line_group_select_[index] << std::endl;
  return;
}
//
void ALCTController::SetPowerUpDelayLineControlReg_(){
  //
  SetDelayLineReset_(OFF);
  SetDelayLineSettst_(ON);                 //default for data taking
  for (int group=0; group<7; group++) 
    SetDelayLineGroupSelect_(group,OFF);
  //
  return;
}
//
////////////////////////////////
// AFEB DELAYS and PATTERNS
////////////////////////////////
void ALCTController::WriteAsicDelaysAndPatterns() {
  //
  (*MyOutput_) << "ALCT: WRITE asic DELAYS and PATTERNS for " 
	       << GetNumberOfGroupsOfDelayChips() << " groups" 
	       << std::endl;
  //
  SetPowerUpDelayLineControlReg_();         // reset the control register values
  //
  for (int group=0; group<GetNumberOfGroupsOfDelayChips(); group++) {
    // write values to one group of AFEBs at a time...
    SetDelayLineGroupSelect_(group,ON);     
    WriteDelayLineControlReg_();
    //    PrintDelayLineControlReg_();
    //
    FillAsicDelaysAndPatterns_(group);
    //
    setup_jtag(ChainAlctFastFpga);
    //
    ShfIR_ShfDR(ChipLocationAlctFastFpga,
		ALCT_FAST_WRT_ASIC_DELAY_LINES,
		RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES,
		write_asic_delays_and_patterns_);
    //
    if ( GetCheckJtagWrite() ) {
      ReadAsicDelaysAndPatterns_(group);
      CompareBitByBit(write_asic_delays_and_patterns_,           
		      read_asic_delays_and_patterns_,            
		      RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES); 
    }
    //
    SetDelayLineGroupSelect_(group,OFF);     
    WriteDelayLineControlReg_();
  }
  //
  return;
}
//
void ALCTController::ReadAsicDelaysAndPatterns() {
  //
  (*MyOutput_) << "ALCT: READ asic DELAYS and PATTERNS for " 
	       << GetNumberOfGroupsOfDelayChips() << " groups" 
	       << std::endl;
  //
  SetPowerUpDelayLineControlReg_();         // reset the control register values
  //
  for (int group=0; group<GetNumberOfGroupsOfDelayChips(); group++) {
    //
    // get values from one group of AFEBs at a time...
    SetDelayLineGroupSelect_(group,ON);     
    WriteDelayLineControlReg_();
    //    PrintDelayLineControlReg_();
    //
    ReadAsicDelaysAndPatterns_(group);
    //
    SetDelayLineGroupSelect_(group,OFF);     
    WriteDelayLineControlReg_();
  }
  //
  return;
}
//
void ALCTController::ReadAsicDelaysAndPatterns_(int group) {
  //N.B. Before using this method, you should already have selected the right
  // delay line control register values
  (*MyOutput_) << "ALCT: READ asic DELAYS and PATTERNS for group " << group << std::endl;
  //
  setup_jtag(ChainAlctFastFpga);
  //
  ShfIR_ShfDR(ChipLocationAlctFastFpga,
	      ALCT_FAST_RD_ASIC_DELAY_LINES,
	      RegSizeAlctFastFpga_RD_ASIC_DELAY_LINES);

  //The first bit of the read register is junk--the data we want starts at index=1
  int * register_pointer = GetDRtdo();
  for (int i=1; i<RegSizeAlctFastFpga_RD_ASIC_DELAY_LINES; i++)  
    read_asic_delays_and_patterns_[i-1] = *(register_pointer+i);
  //
  DecodeAsicDelaysAndPatterns_(group);
  //
  return;
}
//
void ALCTController::DecodeAsicDelaysAndPatterns_(int group) {
  // ** Extract the ASIC delays and pattern software values for 6 afebs and               **
  // ** corresponding channels from the vector of bits read_asic_delays_and_patterns_[]   **
  //
  //print out values read to screen:
  //  char values_read[RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES/8];
  //  packCharBuffer(read_asic_delays_and_patterns_,
  //  		 RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES,
  //  		 values_read);
  //  (*MyOutput_) << "Read values:  group" << group << "-> asic_delays_and_patterns = ";
  //  for (int char_counter=RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES/8-1; char_counter>=0; char_counter--) {
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
    //location of delay bits for this afeb:
    int minimum_delay_bit = afeb_counter * NUMBER_OF_ASIC_BITS + asic_delay_value_bitlo;
    //
    int number_of_bits = asic_delay_value_bithi - asic_delay_value_bitlo + 1;
    int delay_value = bits_to_int(read_asic_delays_and_patterns_+minimum_delay_bit,
				  number_of_bits,
				  LSBfirst);
    //
    read_asic_delay_[afeb] = delay_value;
    //
    //location of pattern value bits for this afeb:
    int minimum_pattern_bit = afeb_counter * NUMBER_OF_ASIC_BITS + asic_pattern_value_bitlo;
    int maximum_pattern_bit = afeb_counter * NUMBER_OF_ASIC_BITS + asic_pattern_value_bithi;
    //
    for (int bit=minimum_pattern_bit; bit<=maximum_pattern_bit; bit++) {
      // Use the asic-group map to find which layer and channels are in this group:
      int layer_number = GetLayerFromAsicMap_(wiregroup_counter);
      int wiregroup_number = GetChannelFromAsicMap_(group,wiregroup_counter);
      //
      read_asic_pattern_[layer_number][wiregroup_number] = read_asic_delays_and_patterns_[bit];
      //
      wiregroup_counter++;
    }
    afeb_counter++;
  }
  return;
}
//
void ALCTController::FillAsicDelaysAndPatterns_(int group) {
  // ** Project the software values of the delays and patterns  **
  // ** for this group of AFEBs into the vector of bits         **
  // ** write_asic_delays_and_patterns_[]                       **
  //
  int minimum_afeb_in_group=group*NUMBER_OF_CHIPS_PER_GROUP;
  int maximum_afeb_in_group=(group+1)*NUMBER_OF_CHIPS_PER_GROUP;
  //
  int afeb_counter = 0;
  int wiregroup_counter = 0;
  //
  for (int afeb=minimum_afeb_in_group; afeb<maximum_afeb_in_group; afeb++) {
    //location of delay value bits for this afeb:
    int minimum_delay_bit = afeb_counter * NUMBER_OF_ASIC_BITS + asic_delay_value_bitlo;
    //
    int_to_bits(write_asic_delay_[afeb],
		asic_delay_value_bithi-asic_delay_value_bitlo+1,
		write_asic_delays_and_patterns_+minimum_delay_bit,
		LSBfirst);
    //
    //location of pattern value bits for this afeb:
    int minimum_pattern_bit = afeb_counter * NUMBER_OF_ASIC_BITS + asic_pattern_value_bitlo;
    int maximum_pattern_bit = afeb_counter * NUMBER_OF_ASIC_BITS + asic_pattern_value_bithi;
    //
    for (int bit=minimum_pattern_bit; bit<=maximum_pattern_bit; bit++) {
      // Use the asic-group map to find which layer and channels are in this group:
      int layer_number = GetLayerFromAsicMap_(wiregroup_counter);
      int wiregroup_number = GetChannelFromAsicMap_(group,wiregroup_counter);
      //
      // Specify the order of the bits in asic_delays_and_patterns_[]:
      write_asic_delays_and_patterns_[bit] = write_asic_pattern_[layer_number][wiregroup_number];
      //
      wiregroup_counter++;
    }
    //
    afeb_counter++;
  }
  //print out values written to screen:
  //  char values_written[RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES/8];
  //  packCharBuffer(write_asic_delays_and_patterns_,
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
int ALCTController::GetLayerFromAsicMap_(int asic_index) {
  return asic_layer_map[asic_index];
}
//
int ALCTController::GetChannelFromAsicMap_(int group, 
					   int asic_index) {
  // Each asic map is the same, except shifted by 16 wires per 96-channel group:
  return asic_wiregroup_map[asic_index] + group*NUMBER_OF_LINES_PER_CHIP; 
}
//
void ALCTController::SetAsicDelay(int afebChannel,
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
  write_asic_delay_[afebChannel] = delay & 0xf;
  //
  return;

}
//
int ALCTController::GetAsicDelay(int afebChannel) {
  if (afebChannel >= GetNumberOfAfebs()) {
    (*MyOutput_) << "GetAsicDelay: ALCT" << std::dec << GetNumberOfChannelsInAlct() 
		 << "-> channel " << std::dec << afebChannel
		 << " invalid ... must be between 0 and " << std::dec << GetNumberOfAfebs()-1 
		 << std::endl;
    return 999;
  } 
  //
  return read_asic_delay_[afebChannel];
}
//
void ALCTController::SetPowerUpAsicDelays() {
  //
  for (int afeb=0; afeb<MAX_NUM_AFEBS; afeb++)
      write_asic_delay_[afeb] = 0;
  //
  return;
}
//
void ALCTController::PrintAsicDelays() {
  //
  (*MyOutput_) << "Asic delay values:" << std::endl;
  (*MyOutput_) << "AFEB   delay (2ns)" << std::endl;
  (*MyOutput_) << "----   -----------" << std::endl;
  for (int afeb=0; afeb<GetNumberOfAfebs(); afeb++)
    (*MyOutput_) << " " << std::dec << afeb << "     " << GetAsicDelay(afeb) << std::endl;
  //
  return;
}
//
void ALCTController::SetAsicPattern(int layer,
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
  write_asic_pattern_[layer][channel] = on_or_off & 0x1;
  //
  return;
}
//
int ALCTController::GetAsicPattern(int layer,
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
  return read_asic_pattern_[layer][channel];
}
//
void ALCTController::SetPowerUpAsicPatterns() {
  //
  for (int layer=0; layer<MAX_NUM_LAYERS; layer++)
    for (int channel=0; channel<MAX_NUM_WIRES_PER_LAYER; channel++)
      write_asic_pattern_[layer][channel] = OFF;
  //
  return;
}
//
void ALCTController::PrintAsicPatterns() {
  //
  char pattern[MAX_NUM_LAYERS][MAX_NUM_WIRES_PER_LAYER/8];
  for (int layer=0; layer<MAX_NUM_LAYERS; layer++)
    packCharBuffer(read_asic_pattern_[layer],
		   GetNumberOfChannelsPerLayer(),
		   pattern[layer]);
  //
  (*MyOutput_) << "READ Asic pattern for ALCT" << std::dec << GetNumberOfChannelsInAlct() 
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
void ALCTController::WriteConfigurationReg() {
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
	      write_config_reg_);
  //
  if ( GetCheckJtagWrite() ) {
    ReadConfigurationReg();
    CompareBitByBit(write_config_reg_,
		    read_config_reg_,
		    RegSizeAlctFastFpga_WRT_CONFIG_REG);
  }
  //
  return;
}
//
void ALCTController::ReadConfigurationReg() {
  //
  setup_jtag(ChainAlctFastFpga);
  //
  ShfIR_ShfDR(ChipLocationAlctFastFpga,
	      ALCT_FAST_RD_CONFIG_REG,
	      RegSizeAlctFastFpga_RD_CONFIG_REG);
  //
  int * register_pointer = GetDRtdo();
  for (int i=0; i<RegSizeAlctFastFpga_RD_CONFIG_REG; i++)
    read_config_reg_[i] = *(register_pointer+i);
  //
  //Print out configuration register in hex...
  char configuration_register[RegSizeAlctFastFpga_RD_CONFIG_REG/8];
  packCharBuffer(read_config_reg_,
		 RegSizeAlctFastFpga_RD_CONFIG_REG,
		 configuration_register);
  //
  (*MyOutput_) << "ALCT READ: configuration register = 0x";
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
void ALCTController::PrintConfigurationReg() {
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
	       << GetInjectMode() << std::endl;
  (*MyOutput_) << "bxc_offset_      = " << std::dec 
	       << GetBxcOffset() << std::endl;
  (*MyOutput_) << "nph_thresh_      = " << std::dec 
	       << GetPretrigNumberOfLayers() << std::endl;
  (*MyOutput_) << "nph_pattern_     = " << std::dec 
	       << GetPretrigNumberOfPattern() << std::endl;
  (*MyOutput_) << "drift_delay_     = " << std::dec
	       << GetDriftDelay() << std::endl;
  (*MyOutput_) << "fifo_tbins_      = " << std::dec 
	       << GetFifoTbins() << std::endl;
  (*MyOutput_) << "fifo_pretrig_    = " << std::dec 
	       << GetFifoPretrig() << std::endl;
  (*MyOutput_) << "fifo_mode_       = " << std::dec
	       << GetFifoMode() << std::endl;
  (*MyOutput_) << "l1a_delay_       = " << std::dec 
	       << GetL1aDelay() << " = 0x" << std::hex
	       << GetL1aDelay() << std::endl;
  (*MyOutput_) << "l1a_window_      = " << std::dec
	       << GetL1aWindowSize() << std::endl;
  (*MyOutput_) << "l1a_offset_      = " << std::dec
	       << GetL1aOffset() << std::endl;
  (*MyOutput_) << "l1a_internal_    = " << std::dec
	       << GetL1aInternal() << std::endl;
  (*MyOutput_) << "board_id_        = " << std::dec
	       << GetBoardId() << std::endl;
  (*MyOutput_) << "ccb_enable_      = " << std::dec
	       << GetCcbEnable() << std::endl;
  (*MyOutput_) << "alct_amode_      = " << std::dec
	       << GetAlctAmode() << std::endl;
  (*MyOutput_) << "trigger_info_en_ = " << std::dec
	       << GetTriggerInfoEnable() << std::endl;
  (*MyOutput_) << "sn_select_       = " << std::dec
	       << GetSnSelect() << std::endl;  
  return;
}
//
void ALCTController::SetTriggerMode(int trigger_mode) { 
  //
  write_trigger_mode_ = trigger_mode; 
  return;
}
//
int ALCTController::GetTriggerMode() { 
  //
  return read_trigger_mode_; 
}
//
void ALCTController::SetExtTrigEnable(int ext_trig_enable) { 
  //
  write_ext_trig_enable_ = ext_trig_enable; 
  return;
}
//
int ALCTController::GetExtTrigEnable() { 
  //
  return read_ext_trig_enable_; 
}
//
void ALCTController::SetSendEmpty(int send_empty) { 
  //
  write_send_empty_ = send_empty; 
  return;
}
//
int ALCTController::GetSendEmpty() { 
  //
  return read_send_empty_; 
}
//
void ALCTController::SetInjectMode(int inject) { 
  //
  write_inject_ = inject; 
  return;
}
//
int ALCTController::GetInjectMode() { 
  //
  return read_inject_; 
}
//
void ALCTController::SetBxcOffset(int bxc_offset) { 
  //
  write_bxc_offset_ = bxc_offset; 
  return;
}
//
int ALCTController::GetBxcOffset() { 
  //
  return read_bxc_offset_; 
}
//
void ALCTController::SetPretrigNumberOfLayers(int nph_thresh) { 
  //
  write_nph_thresh_ = nph_thresh; 
  return;
}
//
int ALCTController::GetPretrigNumberOfLayers() { 
  //
  return read_nph_thresh_; 
}
//
void ALCTController::SetPretrigNumberOfPattern(int nph_pattern) { 
  //
  write_nph_pattern_ = nph_pattern; 
  return;
}
//
int ALCTController::GetPretrigNumberOfPattern() { 
  //
  return read_nph_pattern_; 
}
//
void ALCTController::SetDriftDelay(int drift_delay) { 
  //
  write_drift_delay_ = drift_delay; 
  return;
}
//
int ALCTController::GetDriftDelay() { 
  //
  return read_drift_delay_; 
}
//
void ALCTController::SetFifoTbins(int fifo_tbins) { 
  //
  write_fifo_tbins_ = fifo_tbins; 
  return;
}
//
int ALCTController::GetFifoTbins() { 
  //
  return read_fifo_tbins_; 
}
//
void ALCTController::SetFifoPretrig(int fifo_pretrig) { 
  //
  write_fifo_pretrig_ = fifo_pretrig; 
  return;
}
//
int ALCTController::GetFifoPretrig() { 
  //
  return read_fifo_pretrig_; 
}
//
void ALCTController::SetFifoMode(int fifo_mode) { 
  //
  write_fifo_mode_ = fifo_mode; 
  return;
}
//
int ALCTController::GetFifoMode() { 
  //
  return read_fifo_mode_; 
}
//
void ALCTController::SetL1aDelay(int l1a_delay) { 
  //
  write_l1a_delay_ = l1a_delay; 
  return;
}
//
int ALCTController::GetL1aDelay() { 
  //
  return read_l1a_delay_; 
}
//
void ALCTController::SetL1aWindowSize(int l1a_window) { 
  //
  write_l1a_window_ = l1a_window; 
  return;
}
//
int ALCTController::GetL1aWindowSize() { 
  //
  return read_l1a_window_; 
}
//
void ALCTController::SetL1aOffset(int l1a_offset) { 
  //
  write_l1a_offset_ = l1a_offset; 
  return;
}
//
int ALCTController::GetL1aOffset() { 
  //
  return read_l1a_offset_; 
}
//
void ALCTController::SetL1aInternal(int l1a_internal) { 
  //
  write_l1a_internal_ = l1a_internal; 
  return;
}
int ALCTController::GetL1aInternal() { 
  //
  return read_l1a_internal_; 
}
//
void ALCTController::SetBoardId(int board_id) { 
  //
  write_board_id_ = board_id; 
  return;
}
//
int ALCTController::GetBoardId() { 
  //
  return read_board_id_; 
}
//
void ALCTController::SetCcbEnable(int ccb_enable) { 
  //
  write_ccb_enable_ = ccb_enable; 
  return;
}
//
int ALCTController::GetCcbEnable() { 
  //
  return read_ccb_enable_; 
}
//
void ALCTController::SetAlctAmode(int alct_amode) { 
  //
  write_alct_amode_ = alct_amode; 
  return;
}
//
int ALCTController::GetAlctAmode() { 
  //
  return read_alct_amode_; 
}
//
void ALCTController::SetTriggerInfoEnable(int trigger_info_en) { 
  //
  write_trigger_info_en_ = trigger_info_en; 
  return;
}
int ALCTController::GetTriggerInfoEnable() { 
  //
  return read_trigger_info_en_; 
}
//
void ALCTController::SetSnSelect(int sn_select) { 
  //
  write_sn_select_ = sn_select; 
  return;
}
int ALCTController::GetSnSelect() { 
  //
  return read_sn_select_; 
}
//
void ALCTController::DecodeConfigurationReg_(){
  // ** Extract the configuration register's software values  **
  // ** from the vector of bits read_config_reg_[]            **
  //
  int number_of_bits = trigger_mode_bithi - trigger_mode_bitlo + 1;
  read_trigger_mode_ = bits_to_int(read_config_reg_+trigger_mode_bitlo,
				   number_of_bits,
				   LSBfirst);
  //
  number_of_bits = ext_trig_enable_bithi - ext_trig_enable_bitlo + 1;
  read_ext_trig_enable_ = bits_to_int(read_config_reg_+ext_trig_enable_bitlo,
				      number_of_bits,
				      LSBfirst);
  //
  number_of_bits = send_empty_bithi - send_empty_bitlo + 1;
  read_send_empty_ = bits_to_int(read_config_reg_+send_empty_bitlo,
				 number_of_bits,
				 LSBfirst);
  //
  number_of_bits = inject_bithi - inject_bitlo + 1;
  read_inject_ = bits_to_int(read_config_reg_+inject_bitlo,
			     number_of_bits,
			     LSBfirst);
  //
  number_of_bits = bxc_offset_bithi - bxc_offset_bitlo + 1;
  read_bxc_offset_ = bits_to_int(read_config_reg_+bxc_offset_bitlo,
				 number_of_bits,
				 LSBfirst);
  //
  number_of_bits = nph_thresh_bithi - nph_thresh_bitlo + 1;
  read_nph_thresh_ = bits_to_int(read_config_reg_+nph_thresh_bitlo,
				 number_of_bits,
				 LSBfirst);
  //
  number_of_bits = nph_pattern_bithi - nph_pattern_bitlo + 1;
  read_nph_pattern_ = bits_to_int(read_config_reg_+nph_pattern_bitlo,
				  number_of_bits,
				  LSBfirst);
  //
  number_of_bits = drift_delay_bithi - drift_delay_bitlo + 1;
  read_drift_delay_ = bits_to_int(read_config_reg_+drift_delay_bitlo,
				  number_of_bits,
				  LSBfirst);
  //
  number_of_bits = fifo_tbins_bithi - fifo_tbins_bitlo + 1;
  read_fifo_tbins_ = bits_to_int(read_config_reg_+fifo_tbins_bitlo,
				 number_of_bits,
				 LSBfirst);
  //
  number_of_bits = fifo_pretrig_bithi - fifo_pretrig_bitlo + 1;
  read_fifo_pretrig_ = bits_to_int(read_config_reg_+fifo_pretrig_bitlo,
				   number_of_bits,
				   LSBfirst);
  //
  number_of_bits = fifo_mode_bithi - fifo_mode_bitlo + 1;
  read_fifo_mode_ = bits_to_int(read_config_reg_+fifo_mode_bitlo,
				number_of_bits,
				LSBfirst);
  //
  number_of_bits = l1a_delay_bithi - l1a_delay_bitlo + 1;
  read_l1a_delay_ = bits_to_int(read_config_reg_+l1a_delay_bitlo,
				number_of_bits,
				LSBfirst);
  //
  number_of_bits = l1a_window_bithi - l1a_window_bitlo + 1;
  read_l1a_window_ = bits_to_int(read_config_reg_+l1a_window_bitlo,
				 number_of_bits,
				 LSBfirst);
  //
  number_of_bits = l1a_offset_bithi - l1a_offset_bitlo + 1;
  read_l1a_offset_ = bits_to_int(read_config_reg_+l1a_offset_bitlo,
				 number_of_bits,
				 LSBfirst);
  //
  number_of_bits = l1a_internal_bithi - l1a_internal_bitlo + 1;
  read_l1a_internal_ = bits_to_int(read_config_reg_+l1a_internal_bitlo,
				   number_of_bits,
				   LSBfirst);
  //
  number_of_bits = board_id_bithi - board_id_bitlo + 1;
  read_board_id_ = bits_to_int(read_config_reg_+board_id_bitlo,
			       number_of_bits,
			       LSBfirst);
  //
  number_of_bits = ccb_enable_bithi - ccb_enable_bitlo + 1;
  read_ccb_enable_ = bits_to_int(read_config_reg_+ccb_enable_bitlo,
				 number_of_bits,
				 LSBfirst);
  //
  number_of_bits = alct_amode_bithi - alct_amode_bitlo + 1;
  read_alct_amode_ = bits_to_int(read_config_reg_+alct_amode_bitlo,
				 number_of_bits,
				 LSBfirst);
  //
  number_of_bits = trigger_info_en_bithi - trigger_info_en_bitlo + 1;
  read_trigger_info_en_ = bits_to_int(read_config_reg_+trigger_info_en_bitlo,
				 number_of_bits,
				 LSBfirst);
  //
  number_of_bits = sn_select_bithi - sn_select_bitlo + 1;
  read_sn_select_ = bits_to_int(read_config_reg_+sn_select_bitlo,
				number_of_bits,
				LSBfirst);
  //
  return;
}
//
void ALCTController::FillConfigurationReg_(){
  // ** Project the configuration register's software values  **
  // ** into the vector of bits write_config_reg_[]           **
  //
  int_to_bits(write_trigger_mode_,
	      trigger_mode_bithi-trigger_mode_bitlo+1,
	      write_config_reg_+trigger_mode_bitlo,
	      LSBfirst);
  //
  int_to_bits(write_ext_trig_enable_,
	      ext_trig_enable_bithi-ext_trig_enable_bitlo+1,
	      write_config_reg_+ext_trig_enable_bitlo,
	      LSBfirst);
  //
  int_to_bits(write_send_empty_,
	      send_empty_bithi-send_empty_bitlo+1,
	      write_config_reg_+send_empty_bitlo,
	      LSBfirst);
  //
  int_to_bits(write_inject_,
	      inject_bithi-inject_bitlo+1,
	      write_config_reg_+inject_bitlo,
	      LSBfirst);
  //
  int_to_bits(write_bxc_offset_,
	      bxc_offset_bithi-bxc_offset_bitlo+1,
	      write_config_reg_+bxc_offset_bitlo,
	      LSBfirst);
  //
  int_to_bits(write_nph_thresh_,
	      nph_thresh_bithi-nph_thresh_bitlo+1,
	      write_config_reg_+nph_thresh_bitlo,
	      LSBfirst);
  //
  int_to_bits(write_nph_pattern_,
	      nph_pattern_bithi-nph_pattern_bitlo+1,
	      write_config_reg_+nph_pattern_bitlo,
	      LSBfirst);
  //
  int_to_bits(write_drift_delay_,
	      drift_delay_bithi-drift_delay_bitlo+1,
	      write_config_reg_+drift_delay_bitlo,
	      LSBfirst);
  //
  int_to_bits(write_fifo_tbins_,
	      fifo_tbins_bithi-fifo_tbins_bitlo+1,
	      write_config_reg_+fifo_tbins_bitlo,
	      LSBfirst);
  //
  int_to_bits(write_fifo_pretrig_,
	      fifo_pretrig_bithi-fifo_pretrig_bitlo+1,
	      write_config_reg_+fifo_pretrig_bitlo,
	      LSBfirst);
  //
  int_to_bits(write_fifo_mode_,
	      fifo_mode_bithi-fifo_mode_bitlo+1,
	      write_config_reg_+fifo_mode_bitlo,
	      LSBfirst);
  //
  int_to_bits(write_l1a_delay_,
	      l1a_delay_bithi-l1a_delay_bitlo+1,
	      write_config_reg_+l1a_delay_bitlo,
	      LSBfirst);
  //
  int_to_bits(write_l1a_window_,
	      l1a_window_bithi-l1a_window_bitlo+1,
	      write_config_reg_+l1a_window_bitlo,
	      LSBfirst);
  //
  int_to_bits(write_l1a_offset_,
	      l1a_offset_bithi-l1a_offset_bitlo+1,
	      write_config_reg_+l1a_offset_bitlo,
	      LSBfirst);
  //
  int_to_bits(write_l1a_internal_,
	      l1a_internal_bithi-l1a_internal_bitlo+1,
	      write_config_reg_+l1a_internal_bitlo,
	      LSBfirst);
  //
  int_to_bits(write_board_id_,
	      board_id_bithi-board_id_bitlo+1,
	      write_config_reg_+board_id_bitlo,
	      LSBfirst);
  //
  int_to_bits(write_ccb_enable_,
	      ccb_enable_bithi-ccb_enable_bitlo+1,
	      write_config_reg_+ccb_enable_bitlo,
	      LSBfirst);
  //
  int_to_bits(write_alct_amode_,
	      alct_amode_bithi-alct_amode_bitlo+1,
	      write_config_reg_+alct_amode_bitlo,
	      LSBfirst);
  //
  int_to_bits(write_trigger_info_en_,
	      trigger_info_en_bithi-trigger_info_en_bitlo+1,
	      write_config_reg_+trigger_info_en_bitlo,
	      LSBfirst);
  //
  int_to_bits(write_sn_select_,
	      sn_select_bithi-sn_select_bitlo+1,
	      write_config_reg_+sn_select_bitlo,
	      LSBfirst);
  return;
}
//
void ALCTController::SetPowerUpConfigurationReg() {
  //
  // For bits which are not explicitly set by the methods below, set them to 0:
  for (int i=0; i<RegSizeAlctFastFpga_WRT_CONFIG_REG; i++)
    write_config_reg_[i] = 0;
  //
  SetTriggerMode(0);
  SetExtTrigEnable(0);
  SetSendEmpty(0);
  SetInjectMode(0);
  SetBxcOffset(1);
  SetPretrigNumberOfLayers(2);
  SetPretrigNumberOfPattern(4);
  SetDriftDelay(3);
  SetFifoTbins(7);
  SetFifoPretrig(1);
  SetFifoMode(1);
  //  SetFifoLastLct(0); ->      no longer supported
  SetL1aDelay(120);
  SetL1aWindowSize(3);
  SetL1aOffset(1);
  SetL1aInternal(0);
  SetBoardId(5);
  //  SetBxnOffset(0);   ->      no longer supported
  SetCcbEnable(0);
  //  SetAlctJtagDs(1);  ->      no longer supported
  //  SetAlctTmode(0);   ->      no longer supported
  SetAlctAmode(0);
  //  SetAlctMaskAll(0); ->      no longer supported
  SetTriggerInfoEnable(0);    // default data taking
  SetSnSelect(0);
  //
  return;
}
//
//////////////////////////////
// HOT CHANNEL MASK
//////////////////////////////
void ALCTController::WriteHotChannelMask() {
  //
  (*MyOutput_) << "ALCT: WRITE hot channel mask" << std::endl;
  //
  setup_jtag(ChainAlctFastFpga);
  //
  ShfIR_ShfDR(ChipLocationAlctFastFpga,
  	      ALCT_FAST_WRT_HOTCHAN_MASK,
  	      RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_,
	      write_hot_channel_mask_);
  usleep(100);
  //
  if ( GetCheckJtagWrite() && !stop_read_) {
    ReadHotChannelMask();
    CompareBitByBit(write_hot_channel_mask_,
		    read_hot_channel_mask_,
		    RegSizeAlctFastFpga_RD_HOTCHAN_MASK_);
    stop_read_ = false;
  }
  //
  return;
}
//
void ALCTController::ReadHotChannelMask() {
  //
  (*MyOutput_) << "ALCT: READ hot channel mask (destructive), so write it back in:" << std::endl;
  //
  setup_jtag(ChainAlctFastFpga);
  //
  ShfIR_ShfDR(ChipLocationAlctFastFpga,
	      ALCT_FAST_RD_HOTCHAN_MASK,
	      RegSizeAlctFastFpga_RD_HOTCHAN_MASK_);
  //
  int * register_pointer = GetDRtdo();
  for (int i=0; i<RegSizeAlctFastFpga_RD_HOTCHAN_MASK_; i++)
    read_hot_channel_mask_[i] = *(register_pointer+i);
  //
  usleep(100);
  //
  //The read of the hot channel mask is destructive, so it needs to be reloaded:
  // avoid going into an infinite loop...
  stop_read_ = true;
  WriteHotChannelMask();
  //
  return;
}
//
void ALCTController::PrintHotChannelMask() {
  // Print out hot channel mask in hex for each layer 
  // from right (channel 0) to left (number of channels in layer)
  //
  char hot_channel_mask[RegSizeAlctFastFpga_RD_HOTCHAN_MASK_/8];
  packCharBuffer(read_hot_channel_mask_,
		 RegSizeAlctFastFpga_RD_HOTCHAN_MASK_,
		 hot_channel_mask);
  //
  int char_counter = RegSizeAlctFastFpga_RD_HOTCHAN_MASK_/8 - 1;
  //
  (*MyOutput_) << "READ Hot Channel Mask for ALCT" << std::dec << GetNumberOfChannelsInAlct() 
	       << " (from right to left):" << std::endl;
  //
  for (int layer=5; layer>=0; layer--) {
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
void ALCTController::SetHotChannelMask(int layer,
				       int channel,
				       int on_or_off) {
  if (layer < 0 || layer >= MAX_NUM_LAYERS) {
    (*MyOutput_) << "SetHotChannelMask: layer " << layer 
		 << "... must be between 0 and " << MAX_NUM_LAYERS-1 
		 << std::endl;
    (*MyOutput_) << "Hot Channel Mask Unchanged" << std::endl;
    return;
  }
  if (channel < 0 || channel >= GetNumberOfChannelsPerLayer()) {
    (*MyOutput_) << "SetHotChannelMask: ALCT" << std::dec << GetNumberOfChannelsInAlct() 
		 << "-> channel " << std::dec << channel 
		 << " invalid ... must be between 0 and " << std::dec << GetNumberOfChannelsPerLayer()-1
		 << std::endl;
    (*MyOutput_) << "Hot Channel Mask Unchanged" << std::endl;
    return;
  }
  //
  //index in hot channel mask is determined by layer number and channel number within the layer:
  int index = layer * GetNumberOfChannelsPerLayer() + channel;
  //
  write_hot_channel_mask_[index] = on_or_off;
  //
  return;
}
//
int ALCTController::GetHotChannelMask(int layer,
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
  return read_hot_channel_mask_[index];
}
//
void ALCTController::SetPowerUpHotChannelMask() {
  //
  for (int channel=0; channel<RegSizeAlctFastFpga_RD_HOTCHAN_MASK_672; channel++)
    write_hot_channel_mask_[channel] = ON;
  //
  return;
}
//
//////////////////////////////
// CHAMBER CHARACTERISTICS
//////////////////////////////
void ALCTController::SetChamberCharacteristics_(std::string chamberType) {
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
void ALCTController::SetAlctType_(int type) {
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
    (*MyOutput_) << "ALCTController: ERROR Invalid ALCT type " 
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
#endif

