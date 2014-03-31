//-----------------------------------------------------------------------
// $Id: JTAG_constants.h,v 1.4 2012/10/11 21:26:45 liu Exp $
// $Log: JTAG_constants.h,v $
// Revision 1.4  2012/10/11 21:26:45  liu
// add DCFEB firmware download and readback
//
// Revision 1.3  2012/10/08 02:35:05  liu
// DCFEB update
//
// Revision 1.2  2012/07/12 13:05:01  ahart
//
// Modified to accomodate DCFEB and ODAQMB.
//
// Revision 1.1  2009/03/25 10:07:42  liu
// move header files to include/emu/pc
//
// Revision 3.3  2006/10/12 17:51:54  mey
// Update
//
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
              TCTRL,EPROM,ECTRL,DRESET,F1DCFEBM,F2DCFEBM,F3DCFEBM,F4DCFEBM,F5DCFEBM,F6DCFEBM,F7DCFEBM,FADCFEBM,
              OMCTRL,OCDAC,OMADC,OFIFO1,OFIFO2,OFIFO3,OFIFO4,OFIFO5,OFIFO6,OFIFO7,OFIFO8,OFIFO9,OLOWVOLT, OSTATUS,OBUCSHF, ORESET
};

#define NORM_RUN    0
#define SMALL_CAP   1
#define MEDIUM_CAP  2
#define LARGE_CAP   3
#define EXT_CAP     4
#define NORM_RUN1   5
#define NORM_RUN2   6
#define KILL_CHAN   7

/* Default delays */

//#define CAL_DEF_DELAY 0x3df68
//#define FEB_DEF_DELAY 0x15
//#define DAV_DEF_DELAY 0xc6254
//#define PRE_DEF_DELAY 7
//#define COMPMODE_DEF 0x0A

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
/* CFEB (Virtex) uses only 0-11 */
/* DCFEB extension: 12 and above */
#define NORM_MODE  0
#define RESET_MODE 1
#define STATUS_CS 2  //Just shift, no update                                                                                            
#define STATUS_S 3   //Update and shift                                                                                                 
#define COMP_DAC   4
#define LCTL1ADELAY 5
#define Pipeline_Read 6
#define PREBLKEND  8
#define COMP_MODE  9
#define CHIP_MASK 10
#define CHIP_SHFT 11
#define FADC_MASK 12
#define FADC_INIT 13
#define FADC_SHIFT 14
#define Pipeline_Restrt 15
#define Pipeline_Depth 16
#define TTC_SOURCE 17
#define CALIB_EXTERNAL 18
#define CALIB_INTERNAL 19
#define Pipeline_NSAMPLE 20
#define EPROM_bpi_write 21
#define EPROM_bpi_read 22
#define EPROM_bpi_status 23
#define EPROM_bpi_timer 24
#define EPROM_bpi_reset 25
#define EPROM_bpi_disable 26
#define EPROM_bpi_enable 27
#define FINE_DELAY 28
#define TMB_TRANSMIT_MODE 29
#define TMB_HALF_STRIP 30
#define TMB_LAYER_MASK 31
#define DAQ_low 32
#define DAQ_high 33
#define Calib_DAC 34
#define ADC_ctrl 35
#define ADC_rdbk 36
#define CLEAR_ANYL1AFLAG 40
#define SET_ANYL1AFLAG 41
#define ADC_Fine_Delay 44
#define PRBS_Test_Mode  45
#define PRBS_Inject_Error 46
#define SEM_Control 47
#define SEM_RST_DED 48
#define SEM_CMD 49
#define SEM_SEU_ADD_LINEAR 50
#define SEM_SEU_ADD_PHYSICAL 51
#define REG_SEL_WRD 52
#define REG_RD_WRD 53
#define QPLL_RST 54
#define QPLL_LK_LOST_CNT 55
#define STARTUP_STATUS 56

/* VIRTEX6 fpga scan instructions */
#define VTX6_EXTEST 0x3C0
#define VTX6_SAMPLE 0x3C1
#define VTX6_USR1  0x3C2
#define VTX6_USR2  0x3C3
#define VTX6_CFG_OUT 0x3C4
#define VTX6_CFG_IN 0x3C5
#define VTX6_INTEST 0x3C7
#define VTX6_USERCODE 0x3C8
#define VTX6_IDCODE 0x3C9
#define VTX6_HIGHZ 0x3CA
#define VTX6_JSTART 0x3CC
#define VTX6_JPROG  0x3CB
#define VTX6_SHUTDN 0x3CD
#define VTX6_ISC_ENABLE 0x3D0
#define VTX6_ISC_PROGRAM 0x3D1
#define VTX6_ISC_NOOP 0x3D4
#define VTX6_ISC_DISABLE 0x3D6

#define VTX6_ISC_DNA 0x3D7
#define VTX6_SYSMON 0x3F7
#define VTX6_BYPASS 0x3FF

/* virtex 6 Registers */
#define VTX6_REG_CRC 0
#define VTX6_REG_FAR 1
#define VTX6_REG_FDRI 2
#define VTX6_REG_FDR0 3
#define VTX6_REG_CMS 4
#define VTX6_REG_CTL0 5
#define VTX6_REG_MASK 6
#define VTX6_REG_STAT 7
#define VTX6_REG_LOUT 8
#define VTX6_REG_COR0 9
#define VTX6_REG_MFWR 10
#define VTX6_REG_CBC 11
#define VTX6_REG_IDCODE 12
#define VTX6_REG_AXSS 13
#define VTX6_REG_COR1 14
#define VTX6_REG_CSOB 15
#define VTX6_REG_WBSTAR 16
#define VTX6_REG_TIMER 17
#define VTX6_REG_BOOTSTS 22
#define VTX6_REG_CTL1 24
#define VTX6_REG_DWC  26


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


/* Xilinx xcf128 BPI Prom */
#define XPROM_NoOp 0x00
#define XPROM_Write_1 0x01            //reserved do not use
#define XPROM_Read_1 0x02
#define XPROM_Write_n 0x03            //reserved do not use
#define XPROM_Read_n 0x04             //provide count of n-1
#define XPROM_Read_Array 0x05
#define XPROM_Status_Reg 0x06
#define XPROM_Elec_Sig 0x07
#define XPROM_ReadCFI_Qry 0x08
#define XPROM_Clr_Status_Reg 0x09
#define XPROM_Block_Erase 0x0a
#define XPROM_Program 0x0b           //provide one data word
#define XPROM_Buffer_Program 0x0c    //provide count of (n-1) and n data words
#define XPROM_Buf_Prog_Wrt_n 0x0d    //reserved do not use
#define XPROM_Buf_Prog_Conf 0x0e     //reserved do not use
#define XPROM_PE_Susp 0x0f
#define XPROM_PE_Resume 0x10
#define XPROM_Prot_Reg_Prog 0x11    //provide one data word
#define XPROM_Set_Cnfg_Reg 0x12     //provide one data word
#define XPROM_Block_Lock 0x13
#define XPROM_Block_UnLock 0x14
#define XPROM_Block_UnLock_Down 0x15
#define XPROM_Blank_Check 0x16
#define XPROM_Load_Address 0x17
#define XPROM_BPI_Reset 0x18
#define XPROM_Timer_Start 0x19
#define XPROM_Timer_Stop 0x1A
#define XPROM_Timer_Reset 0x1B
#define XPROM_Clear_Status 0x1C


#define READ_YES 2
#define NOOP_YES 4
#define NO_BYPASS 8

#define D_CFEB 1
#define CTRL_FPGA 2
#define CTRL_PROM 3
#define VME_PROM 4
#define CFEB_PROM 5
#define CFEB_FPGA 6

#define Bit_Slip_Odd 17
#define Bit_Slip_Even 18
#define Chip_Sel 19
#define EPROM_mcs 21
#define EPROM_read_mcs 22
#define EPROM_status_mcs 23
#define EPROM_timer_mcs 24
#define EPROM_reset_mcs 25
#define EPROM_disablefifo_mcs 26
#define EPROM_enablefifo_mcs 27
#define COMP_Fine_Delay  28
#define TMB_TX_MODE 29
#define TMB_TX_SHIFTLAYERS 30
#define TMB_TX_LAYER_MASK 31
#define DAQRate_1p00GB 32
#define DAQRate_2p56GB 33
#define SEM_STATUS 37
#define SEM_Reset_Error_Counters 38
#define SEM_ERRCNT_READ 39
#define L1AHEAD_DISABLE 42
#define L1AHEAD_ENABLE 43

#endif
