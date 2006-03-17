#ifndef EMU_JTAG_constants_h
#define EMU_JTAG_constants_h
//
#define MAX_FRAMES                  1000       //Maximum number of frames in an i/o cycle
#define MAX_BUFF_SIZE       MAX_FRAMES/8       //Maximum number of buffers in JTAG cycle
//
#define MAX_NUM_DEVICES                5       //Maximum number of devices on any one chain
//
#define NO_READ_BACK                   0       //do not read TDO
#define READ_BACK                      1       //read TDO and pack into buffer
//
//
////////////////////////////////////////////////////
// ALCT Slow FPGA configuration register opcodes:
////////////////////////////////////////////////////
#define ALCT_SLOW_RD_ID_REG                 00
#define ALCT_SLOW_RESET_THRESH_DAC          01
#define ALCT_SLOW_RESET_DELAY_ASIC          02
#define ALCT_SLOW_WRT_TESTPULSE_DAC         03
#define ALCT_SLOW_WRT_THRESH_DAC0          010
#define ALCT_SLOW_WRT_THRESH_DAC1          011
#define ALCT_SLOW_WRT_THRESH_DAC2          012
#define ALCT_SLOW_WRT_THRESH_DAC3          013
#define ALCT_SLOW_RD_THRESH_ADC0           020
#define ALCT_SLOW_RD_THRESH_ADC1           021
#define ALCT_SLOW_RD_THRESH_ADC2           022
#define ALCT_SLOW_RD_THRESH_ADC3           023
#define ALCT_SLOW_RD_THRESH_ADC4           024
#define ALCT_SLOW_WRT_DELAY_ASIC_GRP0      030 
#define ALCT_SLOW_WRT_DELAY_ASIC_GRP1      031 
#define ALCT_SLOW_WRT_DELAY_ASIC_GRP2      032 
#define ALCT_SLOW_WRT_DELAY_ASIC_GRP3      033 
#define ALCT_SLOW_WRT_DELAY_ASIC_GRP4      034 
#define ALCT_SLOW_WRT_DELAY_ASIC_GRP5      035 
#define ALCT_SLOW_WRT_DELAY_ASIC_GRP6      036 
#define ALCT_SLOW_WRT_TESTPULSE_GRP        040   
#define ALCT_SLOW_RD_TESTPULSE_GRP         041   
#define ALCT_SLOW_WRT_TESTPULSE_STRIP      042 
#define ALCT_SLOW_RD_TESTPULSE_STRIP       043 
#define ALCT_SLOW_WRT_STANDBY_REG          044 
#define ALCT_SLOW_RD_STANDBY_REG           045 
#define ALCT_SLOW_WRT_TESTPULSE_POWERDOWN  046 
#define ALCT_SLOW_RD_TESTPULSE_POWERDOWN   047 
#define ALCT_SLOW_BYPASS                   077
//
////////////////////////////////////////////////////
// ALCT Fast FPGA configuration register opcodes:
////////////////////////////////////////////////////
#define ALCT_FAST_RD_ID_REG                 0x0
#define ALCT_FAST_RD_HOTCHAN_MASK           0x1
#define ALCT_FAST_WRT_HOTCHAN_MASK          0x2
#define ALCT_FAST_RD_TRIG_REG               0x3
#define ALCT_FAST_WRT_TRIG_REG              0x4
#define ALCT_FAST_RD_CONFIG_REG             0x6
#define ALCT_FAST_WRT_CONFIG_REG            0x7
#define ALCT_FAST_WRT_ASIC_DELAY_LINES      0xd
#define ALCT_FAST_RD_ASIC_DELAY_LINES       0xe
#define ALCT_FAST_RD_COLLISION_MASK_REG    0x13
#define ALCT_FAST_WRT_COLLISION_MASK_REG   0x14
#define ALCT_FAST_RD_DELAYLINE_CTRL_REG    0x15
#define ALCT_FAST_WRT_DELAYLINE_CTRL_REG   0x16
#define ALCT_FAST_BYPASS                   0x1f
//
////////////////////////////////////////////////////
// ISPROM opcodes:
////////////////////////////////////////////////////
#define PROM_BYPASS   0xFF
#define PROM_SAMPLE   0x01
#define PROM_EXTEST   0x00
#define PROM_CLAMP    0xFA
#define PROM_HIGHZ    0xFC
#define PROM_IDCODE   0xFE
#define PROM_USERCODE 0xFD
#define PROM_CONFIG   0xEE
//
////////////////////////////////////////////////////
// VIRTEXII FPGA opcodes:
////////////////////////////////////////////////////
#define VTX2_EXTEST   0x00
#define VTX2_SAMPLE   0x01
#define VTX2_USR1     0x02
#define VTX2_USR2     0x03
#define VTX2_CFG_OUT  0x04
#define VTX2_CFG_IN   0x05
#define VTX2_INTEST   0x07
#define VTX2_USERCODE 0x08
#define VTX2_IDCODE   0x09
#define VTX2_HIGHZ    0x0A
#define VTX2_JSTART   0x0C
#define VTX2_JPROG    0x0B
#define VTX2_SHUTDN   0x0D
#define VTX2_BYPASS   0x3F
//
//
////-------------------------------//
// JTAG chains and characteristics //
////-------------------------------////
#define ChainAlctSlowFpga        0x0       //ALCT "slow-control" FPGA registers             
#define NumberChipsAlctSlowFpga    1
//
#define ChipLocationAlctSlowFpga   0
#define OpcodeSizeAlctSlowFpga     6
//
#define RegSizeAlctSlowFpga_RD_ID_REG               40                   
#define RegSizeAlctSlowFpga_RESET_THRESH_DAC         1
#define RegSizeAlctSlowFpga_RESET_DELAY_ASIC         1
#define RegSizeAlctSlowFpga_WRT_TESTPULSE_DAC        8
#define RegSizeAlctSlowFpga_WRT_THRESH_DAC0         12
#define RegSizeAlctSlowFpga_WRT_THRESH_DAC1         12
#define RegSizeAlctSlowFpga_WRT_THRESH_DAC2         12
#define RegSizeAlctSlowFpga_WRT_THRESH_DAC3         12
#define RegSizeAlctSlowFpga_RD_THRESH_ADC0          10
#define RegSizeAlctSlowFpga_RD_THRESH_ADC1          10
#define RegSizeAlctSlowFpga_RD_THRESH_ADC2          10
#define RegSizeAlctSlowFpga_RD_THRESH_ADC3          10
#define RegSizeAlctSlowFpga_RD_THRESH_ADC4          10
#define RegSizeAlctSlowFpga_WRT_DELAY_ASIC_GRP0     24
#define RegSizeAlctSlowFpga_WRT_DELAY_ASIC_GRP1     24
#define RegSizeAlctSlowFpga_WRT_DELAY_ASIC_GRP2     24
#define RegSizeAlctSlowFpga_WRT_DELAY_ASIC_GRP3     24
#define RegSizeAlctSlowFpga_WRT_DELAY_ASIC_GRP4     24
#define RegSizeAlctSlowFpga_WRT_DELAY_ASIC_GRP5     24
#define RegSizeAlctSlowFpga_WRT_DELAY_ASIC_GRP6     24
#define RegSizeAlctSlowFpga_WRT_TESTPULSE_GRP        7
#define RegSizeAlctSlowFpga_RD_TESTPULSE_GRP         7
#define RegSizeAlctSlowFpga_WRT_TESTPULSE_STRIP      6
#define RegSizeAlctSlowFpga_RD_TESTPULSE_STRIP       6
#define RegSizeAlctSlowFpga_WRT_STANDBY_REG         42
#define RegSizeAlctSlowFpga_RD_STANDBY_REG          42
#define RegSizeAlctSlowFpga_WRT_TESTPULSE_POWERDOWN  1
#define RegSizeAlctSlowFpga_RD_TESTPULSE_POWERDOWN   1
#define RegSizeAlctSlowFpga_BYPASS                   1
//
//
////-------------------------------////
#define ChainAlctSlowProm         0x1       //ALCT "slow-control" PROM           
#define NumberChipsAlctSlowProm     3
//
#define ChipLocationAlctSlowProm0   0
#define ChipLocationAlctSlowProm1   1
#define ChipLocationAlctSlowProm2   2
#define OpcodeSizeAlctSlowProm      8
//
//
////-------------------------------////
#define ChainAlctFastFpga        0x2       //ALCT Mezzanine FPGA registers     
#define NumberChipsAlctFastFpga    1
//
#define ChipLocationAlctFastFpga   0
#define OpcodeSizeAlctFastFpga     5
//
#define RegSizeAlctFastFpga_RD_ID_REG                    40
#define RegSizeAlctFastFpga_RD_HOTCHAN_MASK_288         288
#define RegSizeAlctFastFpga_RD_HOTCHAN_MASK_384         384
#define RegSizeAlctFastFpga_RD_HOTCHAN_MASK_672         672
#define RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_288        288
#define RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_384        384
#define RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_672        672
#define RegSizeAlctFastFpga_RD_TRIG_REG                   5
#define RegSizeAlctFastFpga_WRT_TRIG_REG                  5
#define RegSizeAlctFastFpga_RD_CONFIG_REG                69
#define RegSizeAlctFastFpga_WRT_CONFIG_REG               69
#define RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES        120
#define RegSizeAlctFastFpga_RD_ASIC_DELAY_LINES         121
#define RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_288   168
#define RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_384   224
#define RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_672   392
#define RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_288  168
#define RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_384  224
#define RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_672  392
#define RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_288     5
#define RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_384     6
#define RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_672     9
#define RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_288    5
#define RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_384    6
#define RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_672    9
#define RegSizeAlctFastFpga_BYPASS                        1
//
//
////-------------------------------////
#define ChainAlctFastProm              0x3      //ALCT Mezzanine PROM
#define NumberChipsAlctFastProm          1
//
//
////-------------------------------////
#define ChainTmbMezz                   0x4      //TMB Mezzanine FPGA+PROMs   
#define NumberChipsTmbMezz               5      //1 FPGA + 4 PROMs
//
#define ChipLocationTmbMezzFpga          0
#define OpcodeSizeTmbMezzFpga            6
#define RegSizeTmbMezzFpga_VTX2_IDCODE  32
//
#define ChipLocationTmbMezzProm0         1
#define ChipLocationTmbMezzProm1         2
#define ChipLocationTmbMezzProm2         3
#define ChipLocationTmbMezzProm3         4
#define OpcodeSizeTmbMezzProm            8
#define RegSizeTmbMezzProm_PROM_IDCODE  32
//
//
////-------------------------------////
#define ChainTmbUser                   0x8      //TMB User PROMs             
#define NumberChipsTmbUser               2
//
#define ChipLocationTmbUserProm0         0
#define ChipLocationTmbUserProm1         1
#define OpcodeSizeTmbUserProm            8
#define RegSizeTmbUserProm_PROM_IDCODE  32
//
//
////-------------------------------////
#define ChainRat                       0xd     //RAT FPGA+PROMs             
#define NumberChipsRat                   2     //1 FPGA + 1 PROM
//
#define ChipLocationRatFpga              0       
#define OpcodeSizeRatFpga                5
#define RegSizeRatFpga_VTX2_USR1       224
#define RegSizeRatFpga_VTX2_USR2        32
#define RegSizeRatFpga_VTX2_USERCODE    32
#define RegSizeRatFpga_VTX2_IDCODE      32
//
#define ChipLocationRatProm              1       
#define OpcodeSizeRatProm                8
#define RegSizeRatProm_PROM_USERCODE    32
#define RegSizeRatProm_PROM_IDCODE      32
//
//
#endif
