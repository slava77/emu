//-----------------------------------------------------------------------
// $Id: JTAG_constants.h,v 3.2 2006/09/14 09:12:03 mey Exp $
// $Log: JTAG_constants.h,v $
// Revision 3.2  2006/09/14 09:12:03  mey
// Update
//
// Revision 3.1  2006/08/02 13:13:45  mey
// Update
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef JTAG_constants_h
#define JTAG_constants_h

#define INSTR_REG 0
#define DATA_REG 1
#define PROM_BYPASS 0xFF
#define TERMINATE 2


/* Buckeye shift variables */

//int shift_array[5][6][16]; 
//int nchips[5]={6,6,6,6,6};
//int chip_use[5][6]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
//int layers[]={2,0,4,5,3,1};

enum DEVTYPE{ALLMUX=-3,ALLSCAM=-2,ALL=-1,NONE,F1PROM,F2PROM,F3PROM,F4PROM,F5PROM,
               FAPROM,F1SCAM,F2SCAM,F3SCAM,F4SCAM,F5SCAM,
               FASCAM,MCTRL,MPROM,VPROM,CDAC,MADC,
              FIFO1,FIFO2,FIFO3,FIFO4,FIFO5,FIFO6,FIFO7,
              LOWVOLT,DEVSTATUS,BUCSHF,RESET,DPROM,DCTRL,TPROM,
              TCTRL,EPROM,ECTRL,DRESET};

#define NORM_RUN    0
#define SMALL_CAP   1
#define MEDIUM_CAP  2
#define LARGE_CAP   3
#define EXT_CAP     4
#define NORM_RUN1   5
#define NORM_RUN2   6
#define KILL_CHAN   7

/* Default delays */

#define CAL_DEF_DELAY 0x3df68
#define FEB_DEF_DELAY 0x15
#define DAV_DEF_DELAY 0xc6254
#define PRE_DEF_DELAY 7
#define COMPMODE_DEF 0x0A

/* JTAG instructions */
#define EXT_TEST 0x00
#define BYPASS   0x81
#define SEL_PATH 0x7E
/* Path options */
/* #define NO_PATH    0x00
#define CPLD_PATH  0x02
#define SWTCH_PATH 0x02
#define LCA_PATH   0x08
#define SCAM_PATH  0x08
#define LCA_PATH_L 0x18
#define OBUF_PATH  0x20
#define MUX_PATH   0x20
#define SWTCH_OBUF 0x22
#define LCA_OBUF   0x28
#define SCA_BRD    0x80
#define BUFF_PATH  0x80 */

/* ISPROM scan instructions */
#define PROM_BYPASS 0xFF
#define PROM_SAMPLE 0x01
#define PROM_EXTEST 0x00
#define PROM_CLAMP 0xFA
#define PROM_HIGHZ 0xFC
#define PROM_IDCODE 0xFE
#define PROM_USERCODE 0xFD
#define PROM_CONFIG 0xEE
/* VIRTEXII fpga scan instructions */
#define VTX2_EXTEST 0x00
#define VTX2_SAMPLE 0x01
#define VTX2_USR1  0x02
#define VTX2_USR2  0x03
#define VTX2_CFG_OUT 0x04
#define VTX2_CFG_IN 0x05
#define VTX2_INTEST 0x07
#define VTX2_USERCODE 0x08
#define VTX2_IDCODE 0x09
#define VTX2_HIGHZ 0x0A
#define VTX2_JSTART 0x0C
#define VTX2_JPROG  0x0B
#define VTX2_SHUTDN 0x0D
#define VTX2_BYPASS 0x3F

/* VIRTEX fpga scan instructions */
#define VTX_EXTEST 0x00
#define VTX_SAMPLE 0x01
#define VTX_USR1  0x02
#define VTX_USR2  0x03
#define VTX_CFG_OUT 0x04
#define VTX_CFG_IN 0x05
#define VTX_INTEST 0x07
#define VTX_USERCODE 0x08
#define VTX_IDCODE 0x09
#define VTX_HIGHZ 0x0A
#define VTX_JSTART 0x0C
#define VTX_BYPASS 0x1F
/* SCAM-FPGA modes (Virtex FPGA) */
#define NORM_MODE  0
#define RESET_MODE 1
#define STATUS_CS 2  //Just shift, no update
#define STATUS_S 3   //Update and shift
#define COMP_DAC   4
#define LCTL1ADELAY 5
#define PREBLKEND  8
#define COMP_MODE  9
#define CHIP_MASK 10
#define CHIP_SHFT 11
/* CALCTRL modes */
#define CAL_FIFOMRST 0xe
#define CAL_GLOBAL 15
#define CAL_FPGARST 7
#define CAL_WRTFIFO 6
#define CAL_THERMSET 17
#define CAL_THERMREAD 18
#define CAL_STATUS 0
#define CAL_TOGGLE_CMODE 14
#define CAL_DELAY 0x11
#define CAL_PROGFEB 20
/* TRGCTRL modes */
#define TRG_STATUS 0
#define TRG_READFIFO 2
#define TRG_DAV_DELAY 0x0d
#define FEB_DELAY 0x10
#define FIFO_RD 0x0c
#define CRATE_ID 0x0f
#define GLOBAL_RST 0x01
#define TRG_RATE 0x13
#define RTRG_TGL 0x14
/* DDU definitions */
#define DCNTRL_RST 0x01
#define ECNTRL_RST 0x01

/* Chip testing modes */
#define NOOP        0
#define SHFT_CHIP   1
#define PRG_DAC     2
#define INJECT      3
#define PULSE       4
#define PED_TRIG    5
#define CHECK_STAT  10
#define UNDEF_1     7
#define LOAD_TRIG   6
#define CYCLE_TRIG  7
#define RUN_TRIG    8
#define CYCLE_T_P  11
#define RUN_T_P    12
#define LOAD_STR    9
#define EXT_OFF    14
#define MB_JRST    15

#endif


