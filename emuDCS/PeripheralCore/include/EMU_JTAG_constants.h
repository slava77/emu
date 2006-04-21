#ifndef EMU_JTAG_constants_h
#define EMU_JTAG_constants_h
//
const int MAX_NUM_FRAMES  =            1000;       //Maximum number of frames in an i/o cycle
const int MAX_BUFFER_SIZE =MAX_NUM_FRAMES/8;       //Maximum number of buffers in JTAG cycle
//
const int MAX_NUM_DEVICES =               5;       //Maximum number of devices on any one chain
//
// bits_to_int/int_to_bits specification...
const int LSBfirst        =               0;
const int MSBfirst        =               1;
//
// Some constants for VMEController_jtag::scan_alct....
const int INSTR_REGISTER  =               0;       //write to Instruction Register
const int DATA_REGISTER   =               1;       //write to Data Register
const int NO_READ_BACK    =               0;       //do not read TDO
const int READ_BACK       =               1;       //read TDO and pack into buffer
//
//
////////////////////////////////////////////////////
// ALCT Slow FPGA configuration register opcodes:
////////////////////////////////////////////////////
const int ALCT_SLOW_RD_ID_REG              =   00;
const int ALCT_SLOW_RESET_THRESH_DAC       =   01;
const int ALCT_SLOW_RESET_DELAY_ASIC       =   02;
const int ALCT_SLOW_WRT_TESTPULSE_DAC      =   03;
const int ALCT_SLOW_WRT_THRESH_DAC0        =  010;
const int ALCT_SLOW_WRT_THRESH_DAC1        =  011;
const int ALCT_SLOW_WRT_THRESH_DAC2        =  012;
const int ALCT_SLOW_WRT_THRESH_DAC3        =  013;
const int ALCT_SLOW_RD_THRESH_ADC0         =  020;
const int ALCT_SLOW_RD_THRESH_ADC1         =  021;
const int ALCT_SLOW_RD_THRESH_ADC2         =  022;
const int ALCT_SLOW_RD_THRESH_ADC3         =  023;
const int ALCT_SLOW_RD_THRESH_ADC4         =  024;
const int ALCT_SLOW_WRT_DELAY_ASIC_GRP0    =  030;
const int ALCT_SLOW_WRT_DELAY_ASIC_GRP1    =  031;
const int ALCT_SLOW_WRT_DELAY_ASIC_GRP2    =  032;
const int ALCT_SLOW_WRT_DELAY_ASIC_GRP3    =  033; 
const int ALCT_SLOW_WRT_DELAY_ASIC_GRP4    =  034;
const int ALCT_SLOW_WRT_DELAY_ASIC_GRP5    =  035; 
const int ALCT_SLOW_WRT_DELAY_ASIC_GRP6    =  036; 
const int ALCT_SLOW_WRT_TESTPULSE_GRP      =  040;   
const int ALCT_SLOW_RD_TESTPULSE_GRP       =  041;   
const int ALCT_SLOW_WRT_TESTPULSE_STRIP    =  042; 
const int ALCT_SLOW_RD_TESTPULSE_STRIP     =  043; 
const int ALCT_SLOW_WRT_STANDBY_REG        =  044; 
const int ALCT_SLOW_RD_STANDBY_REG         =  045; 
const int ALCT_SLOW_WRT_TESTPULSE_POWERDOWN=  046; 
const int ALCT_SLOW_RD_TESTPULSE_POWERDOWN =  047; 
const int ALCT_SLOW_BYPASS                 =  077;
//
////////////////////////////////////////////////////
// ALCT Fast FPGA configuration register opcodes:
////////////////////////////////////////////////////
const int ALCT_FAST_RD_ID_REG              =   0x0;
const int ALCT_FAST_RD_HOTCHAN_MASK        =   0x1;
const int ALCT_FAST_WRT_HOTCHAN_MASK       =   0x2;
const int ALCT_FAST_RD_TRIG_REG            =   0x3;
const int ALCT_FAST_WRT_TRIG_REG           =   0x4;
const int ALCT_FAST_RD_CONFIG_REG          =   0x6;
const int ALCT_FAST_WRT_CONFIG_REG         =   0x7;
const int ALCT_FAST_WRT_ASIC_DELAY_LINES   =   0xd;
const int ALCT_FAST_RD_ASIC_DELAY_LINES    =   0xe;
const int ALCT_FAST_RD_COLLISION_MASK_REG  =  0x13;
const int ALCT_FAST_WRT_COLLISION_MASK_REG =  0x14;
const int ALCT_FAST_RD_DELAYLINE_CTRL_REG  =  0x15;
const int ALCT_FAST_WRT_DELAYLINE_CTRL_REG =  0x16;
const int ALCT_FAST_BYPASS                 =  0x1f;
//
////////////////////////////////////////////////////
// PROM opcodes:
////////////////////////////////////////////////////
const int PROMidCode             = 0xFE;
const int PROMuserCode           = 0xFD;
//
////////////////////////////////////////////////////
// FPGA opcodes:
////////////////////////////////////////////////////
const int FPGAuser1              = 0x02;
const int FPGAuser2              = 0x03;
const int FPGAuserCode           = 0x08;
const int FPGAidCode             = 0x09;
//
//
////-------------------------------//
// JTAG chains and characteristics //
////-------------------------------////
const int ChainAlctSlowFpga       = 0x0;       //ALCT "slow-control" FPGA registers             
const int NumberChipsAlctSlowFpga =   1;
//
const int ChipLocationAlctSlowFpga=   0;
const int OpcodeSizeAlctSlowFpga  =   6;
//
const int RegSizeAlctSlowFpga_RD_ID_REG              = 40;                   
const int RegSizeAlctSlowFpga_RESET_THRESH_DAC       =  1;
const int RegSizeAlctSlowFpga_RESET_DELAY_ASIC       =  1;
const int RegSizeAlctSlowFpga_WRT_TESTPULSE_DAC      =  8;
const int RegSizeAlctSlowFpga_WRT_THRESH_DAC0        = 12;
const int RegSizeAlctSlowFpga_WRT_THRESH_DAC1        = 12;
const int RegSizeAlctSlowFpga_WRT_THRESH_DAC2        = 12;
const int RegSizeAlctSlowFpga_WRT_THRESH_DAC3        = 12;
const int RegSizeAlctSlowFpga_RD_THRESH_ADC0         = 11;  //in ALCT documentation, length = 10... 
const int RegSizeAlctSlowFpga_RD_THRESH_ADC1         = 11;  //but actual size determined to be 11...
const int RegSizeAlctSlowFpga_RD_THRESH_ADC2         = 11;
const int RegSizeAlctSlowFpga_RD_THRESH_ADC3         = 11;
const int RegSizeAlctSlowFpga_RD_THRESH_ADC4         = 11;
const int RegSizeAlctSlowFpga_WRT_DELAY_ASIC_GRP0    = 24;
const int RegSizeAlctSlowFpga_WRT_DELAY_ASIC_GRP1    = 24;
const int RegSizeAlctSlowFpga_WRT_DELAY_ASIC_GRP2    = 24;
const int RegSizeAlctSlowFpga_WRT_DELAY_ASIC_GRP3    = 24;
const int RegSizeAlctSlowFpga_WRT_DELAY_ASIC_GRP4    = 24;
const int RegSizeAlctSlowFpga_WRT_DELAY_ASIC_GRP5    = 24;
const int RegSizeAlctSlowFpga_WRT_DELAY_ASIC_GRP6    = 24;
const int RegSizeAlctSlowFpga_WRT_TESTPULSE_GRP      =  7;
const int RegSizeAlctSlowFpga_RD_TESTPULSE_GRP       =  7;
const int RegSizeAlctSlowFpga_WRT_TESTPULSE_STRIP    =  6;
const int RegSizeAlctSlowFpga_RD_TESTPULSE_STRIP     =  6;
const int RegSizeAlctSlowFpga_WRT_STANDBY_REG        = 42;
const int RegSizeAlctSlowFpga_RD_STANDBY_REG         = 42;
const int RegSizeAlctSlowFpga_WRT_TESTPULSE_POWERDOWN=  1;
const int RegSizeAlctSlowFpga_RD_TESTPULSE_POWERDOWN =  1;
const int RegSizeAlctSlowFpga_BYPASS                 =  1;
//
//
////-------------------------------////
const int ChainAlctSlowProm        = 0x1;       //ALCT "slow-control" PROM           
const int NumberChipsAlctSlowProm  =   3;
//
const int ChipLocationAlctSlowProm0=   0;
const int ChipLocationAlctSlowProm1=   1;
const int ChipLocationAlctSlowProm2=   2;
const int OpcodeSizeAlctSlowProm   =   8;
//
//
////-------------------------------////
const int ChainAlctFastFpga       = 0x2;       //ALCT Mezzanine FPGA registers     
const int NumberChipsAlctFastFpga =   1;
//
const int ChipLocationAlctFastFpga=   0;
const int OpcodeSizeAlctFastFpga  =   5;
//
const int RegSizeAlctFastFpga_RD_ID_REG                 =   40;
const int RegSizeAlctFastFpga_RD_HOTCHAN_MASK_288       =  288;
const int RegSizeAlctFastFpga_RD_HOTCHAN_MASK_384       =  384;
const int RegSizeAlctFastFpga_RD_HOTCHAN_MASK_672       =  672;
const int RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_288      =  288;
const int RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_384      =  384;
const int RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_672      =  672;
const int RegSizeAlctFastFpga_RD_TRIG_REG               =    5;
const int RegSizeAlctFastFpga_WRT_TRIG_REG              =    5;
const int RegSizeAlctFastFpga_RD_CONFIG_REG             =   69;
const int RegSizeAlctFastFpga_WRT_CONFIG_REG            =   69;
const int RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES      =  120;
const int RegSizeAlctFastFpga_RD_ASIC_DELAY_LINES       =  121;
const int RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_288 =  168;
const int RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_384 =  224;
const int RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_672 =  392;
const int RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_288=  168;
const int RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_384=  224;
const int RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_672=  392;
const int RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_288 =    5;
const int RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_384 =    6;
const int RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_672 =    9;
const int RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_288=    5;
const int RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_384=    6;
const int RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_672=    9;
const int RegSizeAlctFastFpga_BYPASS                    =    1;
//
//
////-------------------------------////
const int ChainAlctFastProm             = 0x3;      //ALCT Mezzanine PROM
const int NumberChipsAlctFastProm       =   1;
//
//
////-------------------------------////
const int ChainTmbMezz                  = 0x4;      //TMB Mezzanine FPGA+PROMs   
const int NumberChipsTmbMezz            =   5;      //1 FPGA + 4 PROMs
//
const int ChipLocationTmbMezzFpga       =   0;
const int OpcodeSizeTmbMezzFpga         =   6;
const int RegSizeTmbMezzFpga_FPGAidCode =  32;
//
const int ChipLocationTmbMezzProm0      =   1;
const int ChipLocationTmbMezzProm1      =   2;
const int ChipLocationTmbMezzProm2      =   3;
const int ChipLocationTmbMezzProm3      =   4;
const int OpcodeSizeTmbMezzProm         =   8;
const int RegSizeTmbMezzProm_PROMidCode =  32;
//
//
////-------------------------------////
const int ChainTmbUser                  = 0x8;      //TMB User PROMs             
const int NumberChipsTmbUser            =   2;
//
const int ChipLocationTmbUserProm0      =   0;
const int ChipLocationTmbUserProm1      =   1;
const int OpcodeSizeTmbUserProm         =   8;
const int RegSizeTmbUserProm_PROMidCode =  32;
//
//
////-------------------------------////
const int ChainRat                     = 0xd;     //RAT FPGA+PROMs             
const int NumberChipsRat               =   2;     //1 FPGA + 1 PROM
//
const int ChipLocationRatFpga          =   0;       
const int OpcodeSizeRatFpga            =   5;
const int RegSizeRatFpga_FPGAuser1    = 224;
const int RegSizeRatFpga_FPGAuser2    =  32;
const int RegSizeRatFpga_FPGAuserCode =  32;
const int RegSizeRatFpga_FPGAidCode   =  32;
//
const int ChipLocationRatProm          =   1;      
const int OpcodeSizeRatProm            =   8;
const int RegSizeRatProm_PROMuserCode  =  32;
const int RegSizeRatProm_PROMidCode    =  32;
//
//
#endif
