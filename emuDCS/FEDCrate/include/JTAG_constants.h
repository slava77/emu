#ifndef JTAG_constants_h
#define JTAG_constants_h

#define INSTR_REG 0
#define DATA_REG 1
#define PROM_BYPASS 0xFF
#define TERMINATE 2
enum DEVTYPE {ALLMUX=-3,ALLSCAM=-2,ALL=-1,NONE,
	      OUTFIFO,VMEPROM,DDUPROM0,DDUPROM1,
	      INPROM0,INPROM1,DDUFPGA,INFPGA0,INFPGA1,SLINK,
	      VMEPARA,VMESERI,RESET,SADC,MPROM,INPROM,INCTRL1,
	      INCTRL2,INCTRL3,INCTRL4,INCTRL5,MCTRL,RESET1,RESET2};
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
#define PROM_BYPASS10 0x3FF
/* VIRTEX fpga scan instructions */
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
/* virtex-ii pro20  fpga 14 bit scan instructions */
#define VTX2P20_EXTEST_L 0xC0
#define VTX2P20_EXTEST_H 0x3F
#define VTX2P20_USR1_L 0xC2
#define VTX2P20_USR1_H 0x3F
#define VTX2P20_USR2_L 0xC3
#define VTX2P20_USR2_H 0x3F
#define VTX2P20_BYPASS_L 0xFF
#define VTX2P20_BYPASS_H 0x3F
#define VTX2P20_USERCODE_L 0xC8
#define VTX2P20_USERCODE_H 0x3F
#define VTX2P20_IDCODE_L 0xC9
#define VTX2P20_IDCODE_H 0x3F
/* big prom DCC MPROM 16 bit scan instructions */
#define MPROM_BYPASS_L 0xFF
#define MPROM_BYPASS_H 0xFF
#define MPROM_USERCODE_L 0xFD
#define MPROM_USERCODE_H 0x00
#define MPROM_IDCODE_L 0xFE
#define MPROM_IDCODE_H 0x00
/* virtex-ii pro  fpga 10 bit scan instructions */
#define VTX2P_EXTEST_L 0xC0
#define VTX2P_EXTEST_H 0x03
#define VTX2P_USR1_L 0xC2
#define VTX2P_USR1_H 0x03
#define VTX2P_USR2_L 0xC3
#define VTX2P_USR2_H 0x03
#define VTX2P_BYPASS_L 0xFF
#define VTX2P_BYPASS_H 0x03
#define VTX2P_USERCODE_L 0xC8
#define VTX2P_USERCODE_H 0x03
#define VTX2P_IDCODE_L 0xC9
#define VTX2P_IDCODE_H 0x03

/* VIRTEX-II fpga 8 bit scan instructions */
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
#define STATUS_CS 2
#define STATUS_S 3
#define COMP_DAC   4
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
#define DDUFPGA_RST 0x01
#define ECNTRL_RST 0x01

#endif


