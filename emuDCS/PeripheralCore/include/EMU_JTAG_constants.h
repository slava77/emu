#ifndef EMU_JTAG_constants_h
#define EMU_JTAG_constants_h
//
////-------------------------------////
//     Control constants             //
////-------------------------------////
const int OFF           = 0;
const int ON            = 1;
//
//////////////////////////////////////////////////////
/// bits_to_int/int_to_bits specification 
//////////////////////////////////////////////////////
const int LSBfirst        =               0;
const int MSBfirst        =               1;
//
//////////////////////////////////////////////////////
// Constants for VMEController_jtag::scan_alct     
//////////////////////////////////////////////////////
const int INSTR_REGISTER  =               0;       //write to Instruction Register
const int DATA_REGISTER   =               1;       //write to Data Register
const int NO_READ_BACK    =               0;       //do not read TDO
const int READ_BACK       =               1;       //read TDO and pack into buffer
//
//
////-------------------------------////
//   JTAG configuration constants    //
////-------------------------------////
const int MAX_NUM_FRAMES  =              8196;       //Maximum number of frames in an i/o cycle for TMB
const int MAX_BUFFER_SIZE =MAX_NUM_FRAMES/8+1;       //Maximum number of buffers in JTAG cycle
//
const int MAX_NUM_DEVICES =                 5;       //Maximum number of devices on any one chain
//
////////////////////////////////////////////////////
// ALCT Slow FPGA opcodes:
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
// ALCT Fast FPGA opcodes:
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
const int PROMusercodeData       = 0xE6;
const int PROMerase              = 0xEC;
const int PROMaddress            = 0xEB;
const int PROMsErase             = 0x0A;
const int PROMconLd              = 0xF0;
const int PROMispen              = 0xE8;
const int PROMprogram            = 0xEA;
const int PROMwriteData0         = 0xED;
const int PROMverifyData0        = 0xEF;
const int PROMwriteData3         = 0xF3;
const int PROMverifyData3            = 0xE2;
const int PROMbypass             = 0xFF;
//
////////////////////////////////////////////////////
// FPGA opcodes:
////////////////////////////////////////////////////
const int FPGAuser1              = 0x02;
const int FPGAuser2              = 0x03;
const int FPGAuserCode           = 0x08;
const int FPGAidCode             = 0x09;
//
////////////////////////////////////////////////////
// ALCT "slow-control" FPGA chain characteristics
////////////////////////////////////////////////////
const int ChainAlctSlowFpga       = 0x0;       
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
////////////////////////////////////////////////////
// ALCT "slow-control" PROM chain characteristics            
////////////////////////////////////////////////////
const int ChainAlctSlowProm        = 0x1;
const int NumberChipsAlctSlowProm  =   3;
//
const int ChipLocationAlctSlowProm0=   0;
const int ChipLocationAlctSlowProm1=   1;
const int ChipLocationAlctSlowProm2=   2;
const int OpcodeSizeAlctSlowProm   =   8;
//
////////////////////////////////////////////////////
// ALCT "fast-control" FPGA chain characteristics
////////////////////////////////////////////////////
const int ChainAlctFastFpga       = 0x2;
const int NumberChipsAlctFastFpga =   1;
//
const int ChipLocationAlctFastFpga=   0;
const int OpcodeSizeAlctFastFpga  =   5;
//
const int RegSizeAlctFastFpga_RD_ID_REG                 =   40;
const int RegSizeAlctFastFpga_RD_HOTCHAN_MASK_192       =  192;
const int RegSizeAlctFastFpga_RD_HOTCHAN_MASK_288       =  288;
const int RegSizeAlctFastFpga_RD_HOTCHAN_MASK_384       =  384;
const int RegSizeAlctFastFpga_RD_HOTCHAN_MASK_576       =  576;
const int RegSizeAlctFastFpga_RD_HOTCHAN_MASK_672       =  672;
const int RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_192      =  192;
const int RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_288      =  288;
const int RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_384      =  384;
const int RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_576      =  576;
const int RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_672      =  672;
const int RegSizeAlctFastFpga_RD_TRIG_REG               =    5;
const int RegSizeAlctFastFpga_WRT_TRIG_REG              =    5;
const int RegSizeAlctFastFpga_RD_CONFIG_REG             =   69;
const int RegSizeAlctFastFpga_WRT_CONFIG_REG            =   69;
const int RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES      =  120;
const int RegSizeAlctFastFpga_RD_ASIC_DELAY_LINES       =  121;   //first bit of this register is junk
const int RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_192 =  112;
const int RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_288 =  168;
const int RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_384 =  224;
const int RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_576 =  336;
const int RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_672 =  392;
const int RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_192=  112;
const int RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_288=  168;
const int RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_384=  224;
const int RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_576=  336;
const int RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_672=  392;
const int RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_192 =    5;
const int RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_288 =    5;
const int RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_384 =    6;
const int RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_576 =    9;
const int RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_672 =    9;
const int RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_192=    5;
const int RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_288=    5;
const int RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_384=    6;
const int RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_576=    9;
const int RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_672=    9;
const int RegSizeAlctFastFpga_BYPASS                    =    1;
//
////////////////////////////////////////////////////
// ALCT "fast-control" PROM chain characteristics            
////////////////////////////////////////////////////
const int ChainAlctFastProm             = 0x3;      //ALCT Mezzanine PROM
const int NumberChipsAlctFastProm       =   1;
//
////////////////////////////////////////////////////
// TMB mezzanine chain characteristics            
////////////////////////////////////////////////////
const int ChainTmbMezz                  = 0x4;      
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
////////////////////////////////////////////////////
// TMB user PROM chain characteristics            
////////////////////////////////////////////////////
const int ChainTmbUser                           =  0x8;
const int NumberChipsTmbUser                     =    2;
//
const int ChipLocationTmbUserPromTMB             =    0;
const int ChipLocationTmbUserPromALCT            =    1;
const int OpcodeSizeTmbUserProm                  =    8;
//
const int RegSizeTmbUserProm_PROMidCode          =   32;
const int RegSizeTmbUserProm_PROMuserCode        =   32;
const int RegSizeTmbUserProm_PROMusercodeData    =   32;
const int RegSizeTmbUserProm_PROMaddress         =   16;
const int RegSizeTmbUserProm_PROMispen           =    6;
const int RegSizeTmbUserProm_PROMwriteData0      = 2048;
const int RegSizeTmbUserProm_PROMverifyData0     = 4096;
const int RegSizeTmbUserProm_PROMwriteData3      =    6;
const int RegSizeTmbUserProm_PROMverifyData3     =    6;
const int RegSizeTmbUserProm_PROMbypass          =    1;
//
////////////////////////////////////////////////////
// RAT chain characteristics            
////////////////////////////////////////////////////
const int ChainRat                     = 0xd; 
const int NumberChipsRat               =   2;     //1 FPGA + 1 PROM
//
const int ChipLocationRatFpga          =   0;       
const int OpcodeSizeRatFpga            =   5;
const int RegSizeRatFpga_FPGAuser1     = 224;
const int RegSizeRatFpga_FPGAuser2     =  32;
const int RegSizeRatFpga_FPGAuserCode  =  32;
const int RegSizeRatFpga_FPGAidCode    =  32;
//
const int ChipLocationRatProm          =   1;      
const int OpcodeSizeRatProm            =   8;
const int RegSizeRatProm_PROMuserCode  =  32;
const int RegSizeRatProm_PROMidCode    =  32;
//
//
////-------------------------------////
//     RAT constants                 //
////-------------------------------////
////////////////////////////////////////
// User1 bit-map:
////////////////////////////////////////
const int read_rs_begin_bitlo                   = 0;
const int read_rs_begin_bithi                   = 3;
//
const int read_rs_version_bitlo                 = 4;
const int read_rs_version_bithi                 = 7;
//
const int read_rs_monthday_bitlo                = 8;
const int read_rs_monthday_bithi                = 23;
//
const int read_rs_year_bitlo                    = 24;
const int read_rs_year_bithi                    = 39;
//
const int read_rs_syncmode_bitlo                = 40;
const int read_rs_syncmode_bithi                = 40;
//
const int read_rs_posneg_bitlo                  = 41;
const int read_rs_posneg_bithi                  = 41;
//
const int read_rs_loop_bitlo                    = 42;
const int read_rs_loop_bithi                    = 42;
//
const int read_rs_rpc_en_bitlo                  = 43;
const int read_rs_rpc_en_bithi                  = 44;
//
const int read_rs_clk_active_bitlo              = 45;
const int read_rs_clk_active_bithi              = 46;
//
const int read_rs_locked_tmb_bitlo              = 47;
const int read_rs_locked_tmb_bithi              = 47;
//
const int read_rs_locked_rpc0_bitlo             = 48;
const int read_rs_locked_rpc0_bithi             = 48;
//
const int read_rs_locked_rpc1_bitlo             = 49;
const int read_rs_locked_rpc1_bithi             = 49;
//
const int read_rs_locklost_tmb_bitlo            = 50;
const int read_rs_locklost_tmb_bithi            = 50;
//
const int read_rs_locklost_rpc0_bitlo           = 51;
const int read_rs_locklost_rpc0_bithi           = 51;
//
const int read_rs_locklost_rpc1_bitlo           = 52;
const int read_rs_locklost_rpc1_bithi           = 52;
//
const int read_rs_txok_bitlo                    = 53;
const int read_rs_txok_bithi                    = 53;
//
const int read_rs_rxok_bitlo                    = 54;
const int read_rs_rxok_bithi                    = 54;
//
const int read_rs_ntcrit_bitlo                  = 55;
const int read_rs_ntcrit_bithi                  = 55;
//
const int read_rs_rpc_free_bitlo                = 56;
const int read_rs_rpc_free_bithi                = 56;
//
const int read_rs_dsn_bitlo                     = 57;
const int read_rs_dsn_bithi                     = 57;
//
const int read_rs_dddoe_wr_bitlo                = 58;
const int read_rs_dddoe_wr_bithi                = 61;
//
const int read_rs_ddd_wr_bitlo                  = 62;
const int read_rs_ddd_wr_bithi                  = 77;
//
const int read_rs_ddd_auto_bitlo                = 78;
const int read_rs_ddd_auto_bithi                = 78;
//
const int read_rs_ddd_start_bitlo               = 79;
const int read_rs_ddd_start_bithi               = 79;
//
const int read_rs_ddd_busy_bitlo                = 80;
const int read_rs_ddd_busy_bithi                = 80;
//
const int read_rs_ddd_verify_ok_bitlo           = 81;
const int read_rs_ddd_verify_ok_bithi           = 81;
//
const int read_rs_rpc0_parity_ok_bitlo          = 82;
const int read_rs_rpc0_parity_ok_bithi          = 82;
//
const int read_rs_rpc1_parity_ok_bitlo          = 83;
const int read_rs_rpc1_parity_ok_bithi          = 83;
//
const int read_rs_rpc0_cnt_perr_bitlo           = 84;
const int read_rs_rpc0_cnt_perr_bithi           = 99;
//
const int read_rs_rpc1_cnt_perr_bitlo           = 100;
const int read_rs_rpc1_cnt_perr_bithi           = 115;
//
const int read_rs_last_opcode_bitlo             = 116;
const int read_rs_last_opcode_bithi             = 120;
//
const int read_rw_rpc_en_bitlo                  = 121;
const int read_rw_rpc_en_bithi                  = 122;
//
const int read_rw_ddd_start_bitlo               = 123;
const int read_rw_ddd_start_bithi               = 123;
//
const int read_rw_ddd_wr_bitlo                  = 124;
const int read_rw_ddd_wr_bithi                  = 139;
//
const int read_rw_dddoe_wr_bitlo                = 140;
const int read_rw_dddoe_wr_bithi                = 143;
//
const int read_rw_perr_reset_bitlo              = 144;
const int read_rw_perr_reset_bithi              = 144;
//
const int read_rw_parity_odd_bitlo              = 145;
const int read_rw_parity_odd_bithi              = 145;
//
const int read_rw_perr_ignore_bitlo             = 146;
const int read_rw_perr_ignore_bithi             = 146;
//
const int read_rw_rpc_future_bitlo              = 147;
const int read_rw_rpc_future_bithi              = 152;
//
const int read_rs_rpc0_pdata_bitlo              = 153;
const int read_rs_rpc0_pdata_bithi              = 171;
//
const int read_rs_rpc1_pdata_bitlo              = 172;
const int read_rs_rpc1_pdata_bithi              = 190;
//
const int read_rs_unused_bitlo                  = 191;
const int read_rs_unused_bithi                  = 219;
//
const int read_rs_end_bitlo                     = 220;
const int read_rs_end_bithi                     = 223;
//
////////////////////////////////////////
// User2 bit-map:
////////////////////////////////////////
const int write_rw_rpc_en_bitlo                  = 0;
const int write_rw_rpc_en_bithi                  = 1;
//
const int write_rw_ddd_start_bitlo               = 2;
const int write_rw_ddd_start_bithi               = 2;
//
const int write_rw_ddd_wr_bitlo                  = 3;
const int write_rw_ddd_wr_bithi                  = 18;
//
const int write_rw_dddoe_wr_bitlo                = 19;
const int write_rw_dddoe_wr_bithi                = 22;
//
const int write_rw_perr_reset_bitlo              = 23;
const int write_rw_perr_reset_bithi              = 23;
//
const int write_rw_parity_odd_bitlo              = 24;
const int write_rw_parity_odd_bithi              = 24;
//
const int write_rw_perr_ignore_bitlo             = 25;
const int write_rw_perr_ignore_bithi             = 25;
//
const int write_rw_rpc_future_bitlo              = 26;
const int write_rw_rpc_future_bithi              = 31;
//
//
////-------------------------------////
//     ALCT constants                //
////-------------------------------////
const int NUMBER_OF_WIRE_GROUPS_ME11   = 288;
const int FAST_CONTROL_ALCT_TYPE_ME11  = 288;
const int SLOW_CONTROL_ALCT_TYPE_ME11  = 288;
//   
const int NUMBER_OF_WIRE_GROUPS_ME12   = 384;
const int FAST_CONTROL_ALCT_TYPE_ME12  = 384;
const int SLOW_CONTROL_ALCT_TYPE_ME12  = 384;
//
//const int NUMBER_OF_WIRE_GROUPS_ME13   = 192; //until new firmware
//const int FAST_CONTROL_ALCT_TYPE_ME13  = 192;
const int NUMBER_OF_WIRE_GROUPS_ME13   = 288;
const int FAST_CONTROL_ALCT_TYPE_ME13  = 288;
const int SLOW_CONTROL_ALCT_TYPE_ME13  = 288;
//
const int NUMBER_OF_WIRE_GROUPS_ME21   = 672;
const int FAST_CONTROL_ALCT_TYPE_ME21  = 672;
const int SLOW_CONTROL_ALCT_TYPE_ME21  = 672;
//
const int NUMBER_OF_WIRE_GROUPS_ME22   = 384;
const int FAST_CONTROL_ALCT_TYPE_ME22  = 384;
const int SLOW_CONTROL_ALCT_TYPE_ME22  = 384;
//
//const int NUMBER_OF_WIRE_GROUPS_ME31   = 576; //until new firmware
//const int FAST_CONTROL_ALCT_TYPE_ME31  = 576;
const int NUMBER_OF_WIRE_GROUPS_ME31   = 672;
const int FAST_CONTROL_ALCT_TYPE_ME31  = 672;
const int SLOW_CONTROL_ALCT_TYPE_ME31  = 672;
//
const int NUMBER_OF_WIRE_GROUPS_ME32   = 384;
const int FAST_CONTROL_ALCT_TYPE_ME32  = 384;
const int SLOW_CONTROL_ALCT_TYPE_ME32  = 384;
//
//const int NUMBER_OF_WIRE_GROUPS_ME41   = 576; //until new firmware
//const int FAST_CONTROL_ALCT_TYPE_ME41  = 576;
const int NUMBER_OF_WIRE_GROUPS_ME41   = 672; //until new firmware
const int FAST_CONTROL_ALCT_TYPE_ME41  = 672;
const int SLOW_CONTROL_ALCT_TYPE_ME41  = 672;
//
const int NUMBER_OF_WIRE_GROUPS_ME42   = 384;
const int FAST_CONTROL_ALCT_TYPE_ME42  = 384;
const int SLOW_CONTROL_ALCT_TYPE_ME42  = 384;
//
const int NUMBER_OF_GROUPS_OF_DELAY_CHIPS_192   = RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_192 - 2; 
const int NUMBER_OF_GROUPS_OF_DELAY_CHIPS_288   = RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_288 - 2; 
const int NUMBER_OF_GROUPS_OF_DELAY_CHIPS_384   = RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_384 - 2; 
const int NUMBER_OF_GROUPS_OF_DELAY_CHIPS_576   = RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_576 - 2; 
const int NUMBER_OF_GROUPS_OF_DELAY_CHIPS_672   = RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_672 - 2; 
//
const int MAX_NUM_AFEBS = 42;
const int MAX_NUM_LAYERS = 6;
const int MAX_NUM_WIRES_PER_LAYER = 112;  // =672/6
//
//
////////////////////////////////////////
// SetUpPulsing constants:
////////////////////////////////////////
const int PULSE_AFEBS      = 0;
const int PULSE_LAYERS     = 1;
//
////////////////////////////////////////
// ADC channel/chip map
////////////////////////////////////////
const int afeb_adc_channel[MAX_NUM_AFEBS] = 
  {1, 0,
   10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
   10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
   0, 1, 2, 3, 4, 5, 6, 7, 8,
   0, 1, 2, 3, 4, 5, 6, 7, 8};
const int afeb_adc_chip[MAX_NUM_AFEBS] = 
  {2, 2,
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   3, 3, 3, 3, 3, 3, 3, 3, 3,
   4, 4, 4, 4, 4, 4, 4, 4, 4}; 
//
const int Current1p8_adc_chip              = 2;
const int Current3p3_adc_chip              = 2;
const int Current5p5_1_adc_chip            = 2;
const int Current5p5_2_adc_chip            = 2;
const int Voltage1p8_adc_chip              = 2;
const int Voltage3p3_adc_chip              = 2;
const int Voltage5p5_1_adc_chip            = 2;
const int Voltage5p5_2_adc_chip            = 2;
const int Temperature_adc_chip             = 2;
//
const int Current1p8_adc_channel           = 2;
const int Current3p3_adc_channel           = 3;
const int Current5p5_1_adc_channel         = 4;
const int Current5p5_2_adc_channel         = 5;
const int Voltage1p8_adc_channel           = 6;
const int Voltage3p3_adc_channel           = 7;
const int Voltage5p5_1_adc_channel         = 8;
const int Voltage5p5_2_adc_channel         = 9;
const int Temperature_adc_channel          = 10;
//
////////////////////////////////////////
// DAC channel/chip map
////////////////////////////////////////
const int afeb_dac_channel[MAX_NUM_AFEBS] = 
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 
   0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
   0, 1, 2, 3, 4, 5, 6, 7, 8, 
   0, 1, 2, 3, 4, 5, 6, 7, 8};
const int afeb_dac_chip[MAX_NUM_AFEBS] = 
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 
   3, 3, 3, 3, 3, 3, 3, 3, 3};
//
////////////////////////////////////////
// Control register bit-map:
////////////////////////////////////////
const int trigger_mode_bitlo               = 0;
const int trigger_mode_bithi               = 1;
//
const int ext_trig_enable_bitlo            = 2;
const int ext_trig_enable_bithi            = 2;
//
const int send_empty_bitlo                 = 3;
const int send_empty_bithi                 = 3;
//
const int inject_bitlo                     = 4;
const int inject_bithi                     = 4;
//
const int bxc_offset_bitlo                 = 5;
const int bxc_offset_bithi                 = 12;
//
const int nph_thresh_bitlo                 = 13;
const int nph_thresh_bithi                 = 15;
//
const int nph_pattern_bitlo                = 16;
const int nph_pattern_bithi                = 18;
//
const int alct_drift_delay_bitlo           = 19;
const int alct_drift_delay_bithi           = 20;
//
const int alct_fifo_tbins_bitlo            = 21;
const int alct_fifo_tbins_bithi            = 25;
//
const int alct_fifo_pretrig_bitlo          = 26;
const int alct_fifo_pretrig_bithi          = 30;
//
const int alct_fifo_mode_bitlo             = 31;
const int alct_fifo_mode_bithi             = 32;
//
const int l1a_delay_bitlo                  = 36;
const int l1a_delay_bithi                  = 43;
//
const int l1a_window_bitlo                 = 44;
const int l1a_window_bithi                 = 47;
//
const int alct_l1a_offset_bitlo            = 48;
const int alct_l1a_offset_bithi            = 51;
//
const int l1a_internal_bitlo               = 52;
const int l1a_internal_bithi               = 52;
//
const int board_id_bitlo                   = 53;
const int board_id_bithi                   = 55;
//
const int ccb_enable_bitlo                 = 60;
const int ccb_enable_bithi                 = 60;
//
const int alct_amode_bitlo                 = 64;
const int alct_amode_bithi                 = 65;
//
const int trigger_info_en_bitlo            = 67;
const int trigger_info_en_bithi            = 67;
//
const int sn_select_bitlo                  = 68;
const int sn_select_bithi                  = 68;
//
//
////////////////////////////////////////
// Delay-Line Control Register bit-map:
////////////////////////////////////////
const int delay_line_reset_bitlo           = 0;
const int delay_line_reset_bithi           = 0;
//
const int delay_line_settst_bitlo          = 1;
const int delay_line_settst_bithi          = 1;
//
const int delay_line_group_select_bitlo    = 2;
// -> delay_line_group_select_bithi value dependent on ALCT type...
//
/////////////////////////////////////////////////////
// ASIC delay/pattern chip characteristics/bit-maps:
/////////////////////////////////////////////////////
const int NUMBER_OF_CHIPS_PER_GROUP        = 6;
const int NUMBER_OF_LINES_PER_CHIP         = 16;
//
// Load 96 pattern values into 6 asic chips with the following maps for the layer and wiregroup
const int asic_layer_map[NUMBER_OF_CHIPS_PER_GROUP*NUMBER_OF_LINES_PER_CHIP] =
  {1, 1, 1, 1, 1, 1, 1, 1,
   0, 0, 0, 0, 0, 0, 0, 0,
   3, 3, 3, 3, 3, 3, 3, 3,
   2, 2, 2, 2, 2, 2, 2, 2,
   5, 5, 5, 5, 5, 5, 5, 5,
   4, 4, 4, 4, 4, 4, 4, 4,
   1, 1, 1, 1, 1, 1, 1, 1,
   0, 0, 0, 0, 0, 0, 0, 0,
   3, 3, 3, 3, 3, 3, 3, 3,
   2, 2, 2, 2, 2, 2, 2, 2,
   5, 5, 5, 5, 5, 5, 5, 5,
   4, 4, 4, 4, 4, 4, 4, 4};
const int asic_wiregroup_map[NUMBER_OF_CHIPS_PER_GROUP*NUMBER_OF_LINES_PER_CHIP] =
  {7, 6, 5, 4, 3, 2, 1, 0,
   0, 1, 2, 3, 4, 5, 6, 7,
   7, 6, 5, 4, 3, 2, 1, 0,
   0, 1, 2, 3, 4, 5, 6, 7,
   7, 6, 5, 4, 3, 2, 1, 0,
   0, 1, 2, 3, 4, 5, 6, 7,
   15,14,13,12,11,10,9, 8,
   8, 9,10,11,12,13,14,15,
   15,14,13,12,11,10,9, 8,
   8, 9,10,11,12,13,14,15,
   15,14,13,12,11,10,9, 8,
   8, 9,10,11,12,13,14,15};
//
const int asic_delay_value_bitlo           = 0;
const int asic_delay_value_bithi           = 3;
//
const int asic_pattern_value_bitlo         = 4;
const int asic_pattern_value_bithi         = 19;
//
const int NUMBER_OF_ASIC_BITS              = asic_pattern_value_bithi-asic_delay_value_bitlo+1;
//
//
////////////////////////////////////////
// Fast Control ID register bit-map:
////////////////////////////////////////
const int fastcontrol_regular_mirror_bitlo       = 0;
const int fastcontrol_regular_mirror_bithi       = 0;
//
const int fastcontrol_backward_forward_bitlo     = 1;
const int fastcontrol_backward_forward_bithi     = 1;
//
const int fastcontrol_negative_positive_bitlo    = 2;
const int fastcontrol_negative_positive_bithi    = 2;
//
const int fastcontrol_alct_type_bitlo            = 4;
const int fastcontrol_alct_type_bithi            = 7;
//
const int fastcontrol_firmware_year_bitlo        = 8;
const int fastcontrol_firmware_year_bithi        =23;
//
const int fastcontrol_firmware_day_bitlo         =24;
const int fastcontrol_firmware_day_bithi         =31;
//
const int fastcontrol_firmware_month_bitlo       =32;
const int fastcontrol_firmware_month_bithi       =39;
//
// Constants to decode:
const int REGULAR_FIRMWARE_TYPE                  = 0;
const int MIRROR_FIRMWARE_TYPE                   = 1;
//
const int BACKWARD_FIRMWARE_TYPE                 = 0;
const int FORWARD_FIRMWARE_TYPE                  = 1;
//
const int NEGATIVE_FIRMWARE_TYPE                 = 0;
const int POSITIVE_FIRMWARE_TYPE                 = 1;
//
const int FIRMWARE_TYPE_288                      = 2;
const int FIRMWARE_TYPE_384                      = 3;
const int FIRMWARE_TYPE_672                      = 6;
//
//
/////////////////////////////////////////////////////
// Trigger register constants
/////////////////////////////////////////////////////
// These sources of testpulse triggers are a combination of the bits [0-1] and [2-3]
const int SELF                             = 0x3;
const int ADB_SYNC                         = (0x1<<2);
const int ADB_ASYNC                        = (0x2<<2);
const int LEMO                             = (0x3<<2);
//
const int trigger_register_source_bitlo    = 0;
const int trigger_register_source_bithi    = 3;
//
const int trigger_register_invert_bitlo    = 4;
const int trigger_register_invert_bithi    = 4;
//
//
/////////////////////////////////////////////////////
// Collision mask constants
/////////////////////////////////////////////////////
const int NUMBER_OF_BITS_IN_COLLISION_MASK_PER_GROUP = 28;
const int NUMBER_OF_WIREGROUPS_PER_COLLISION_PATTERN_GROUP = 8;
//
//
////-------------------------------////
//     XSVF constants                //
////-------------------------------////
// 256Kbit prom contains information arranged in 128 blocks of 2048 bits per block.
// So "K" = 1024....
//                                                                           
const int SIZE_OF_PROM                     = 256;                   
const int NUMBER_OF_BITS_PER_ADDRESS       = 8;                   //each address corresponds to 1 byte of information
const int NUMBER_OF_BITS_PER_BLOCK         = RegSizeTmbUserProm_PROMwriteData0;
const int NUMBER_OF_ADDRESSES_PER_BLOCK    = NUMBER_OF_BITS_PER_BLOCK / NUMBER_OF_BITS_PER_ADDRESS;  
const int TOTAL_NUMBER_OF_ADDRESSES        = SIZE_OF_PROM * 1024 / NUMBER_OF_BITS_PER_ADDRESS;  
const int TOTAL_NUMBER_OF_BLOCKS           = SIZE_OF_PROM * 1024 / NUMBER_OF_BITS_PER_BLOCK;
//
//const int MAX_XSVF_IMAGE_NUMBER            = SIZE_OF_PROM*1024;        
const int MAX_XSVF_IMAGE_NUMBER            = 13*1024*1024;        //needed for tmb mezzanine proms
////////////////////////////////////
// XSVF commands:
////////////////////////////////////
const int NUMBER_OF_DIFFERENT_XSVF_COMMANDS= 23;
//
const int XCOMPLETE                        = 0x00;
const int XTDOMASK                         = 0x01;
const int XSIR                             = 0x02;
const int XSDR                             = 0x03;
const int XRUNTEST                         = 0x04;
const int XUNDEFINED5                      = 0x05;
const int XUNDEFINED6                      = 0x06;
const int XREPEAT                          = 0x07;
const int XSDRSIZE                         = 0x08;
const int XSDRTDO                          = 0x09;
const int XSETSDRMASK                      = 0x0a;
const int XSDRINC                          = 0x0b;
const int XSDRB                            = 0x0c;
const int XSDRC                            = 0x0d;
const int XSDRE                            = 0x0e;
const int XSDRTDOB                         = 0x0f;
const int XSDRTDOC                         = 0x10;
const int XSDRTDOE                         = 0x11;
const int XSTATE                           = 0x12;
const int XENDIR                           = 0x13;
const int XENDDR                           = 0x14;
const int XSIR2                            = 0x15;
const int XCOMMENT                         = 0x16;
//
const int TLR                              = 0;
const int RTI                              = 1;
//
/////////////////////////////////////////////
// masks and commands for prom programming:
/////////////////////////////////////////////
const int PROM_ID_256k                     = 0x05022093;
const int MASK_TO_TREAT_512k_LIKE_256k     = 0xfffeefff;
//
const int jtagSourceBoot = 0;
const int jtagSourceFPGA = 1;
//
#endif
