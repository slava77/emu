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
const int PROMverifyData3        = 0xE2;
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
const int RegSizeAlctSlowFpga_WRT_TESTPULSE_DAC      =  9;  //8-bit DAC needs 9 bits to shift in...
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
///////////////////////////////////////////////////////
// ALCT "slow-control" Mezzanine chain characteristics            
///////////////////////////////////////////////////////
const int ChainAlctSlowMezz       = 0x1;
const int NumberChipsAlctSlowMezz =   1;
//
const int ChipLocationAlctSlowMezzProm       =   0;
const int OpcodeSizeAlctSlowMezzProm         =   8;
//
//
const int RegSizeAlctSlowMezzFpga_PROMidCode =   32;
//
////////////////////////////////////////////////////
// ALCT "fast-control" FPGA chain characteristics
////////////////////////////////////////////////////
const int ChainAlctFastFpga        = 0x2;
const int NumberChipsAlctFastFpga  =   1;
//
const int ChipLocationAlctFastFpga =   0;
const int OpcodeSizeAlctFastFpga   =   5;
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
////////////////////////////////////////////////////////
// ALCT "fast-control" Mezzanine chain characteristics            
////////////////////////////////////////////////////////
const int ChainAlctFastMezz                  = 0x3;      //ALCT Mezzanine FPGA + PROM(s)
const int NumberChipsAlctFastMezz672         =   3;
const int NumberChipsAlctFastMezz288_384     =   2;
//
const int ChipLocationAlctFastMezzFpga       =   0;
const int OpcodeSizeAlctFastMezzFpga         =   5;
//
const int ChipLocationAlctFastMezzProm0      =   1;
const int ChipLocationAlctFastMezzProm1      =   2;
const int OpcodeSizeAlctFastMezzProm         =   8;
//
const int RegSizeAlctFastMezzFpga_FPGAidCode =   32;
const int RegSizeAlctFastMezzFpga_PROMidCode =   32;
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
// dummy values to tell EMUjtag to clear these proms
const int ChipLocationTmbUserPromTMBClear        =    2;
const int ChipLocationTmbUserPromALCTClear       =    3;
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
const int FAST_FPGA_ID_ME11            = 0x0a30093;
//   
const int NUMBER_OF_WIRE_GROUPS_ME12   = 384;
const int FAST_CONTROL_ALCT_TYPE_ME12  = 384;
const int SLOW_CONTROL_ALCT_TYPE_ME12  = 384;
const int FAST_FPGA_ID_ME12            = 0x0a30093;
//
const int NUMBER_OF_WIRE_GROUPS_ME13   = 192; 
const int FAST_CONTROL_ALCT_TYPE_ME13  = 192; 
const int SLOW_CONTROL_ALCT_TYPE_ME13  = 288;
const int FAST_FPGA_ID_ME13            = 0x0a30093;
//
const int NUMBER_OF_WIRE_GROUPS_ME21   = 672;
const int FAST_CONTROL_ALCT_TYPE_ME21  = 672;
const int SLOW_CONTROL_ALCT_TYPE_ME21  = 672;
const int FAST_FPGA_ID_ME21            = 0x0a40093;
//
const int NUMBER_OF_WIRE_GROUPS_ME22   = 384;
const int FAST_CONTROL_ALCT_TYPE_ME22  = 384;
const int SLOW_CONTROL_ALCT_TYPE_ME22  = 384;
const int FAST_FPGA_ID_ME22            = 0x0a30093;
//
const int NUMBER_OF_WIRE_GROUPS_ME31   = 576;
const int FAST_CONTROL_ALCT_TYPE_ME31  = 576; 
const int SLOW_CONTROL_ALCT_TYPE_ME31  = 672;
const int FAST_FPGA_ID_ME31            = 0x0a40093;
//
const int NUMBER_OF_WIRE_GROUPS_ME32   = 384;
const int FAST_CONTROL_ALCT_TYPE_ME32  = 384;
const int SLOW_CONTROL_ALCT_TYPE_ME32  = 384;
const int FAST_FPGA_ID_ME32            = 0x0a30093;
//
const int NUMBER_OF_WIRE_GROUPS_ME41   = 576; 
const int FAST_CONTROL_ALCT_TYPE_ME41  = 576; 
const int SLOW_CONTROL_ALCT_TYPE_ME41  = 672;
const int FAST_FPGA_ID_ME41            = 0x0a40093;
//
const int NUMBER_OF_WIRE_GROUPS_ME42   = 384;
const int FAST_CONTROL_ALCT_TYPE_ME42  = 384;
const int SLOW_CONTROL_ALCT_TYPE_ME42  = 384;
const int FAST_FPGA_ID_ME42            = 0x0a30093;
//
const int NUMBER_OF_GROUPS_OF_DELAY_CHIPS_192   = RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_192 - 2; 
const int NUMBER_OF_GROUPS_OF_DELAY_CHIPS_288   = RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_288 - 2; 
const int NUMBER_OF_GROUPS_OF_DELAY_CHIPS_384   = RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_384 - 2; 
const int NUMBER_OF_GROUPS_OF_DELAY_CHIPS_576   = RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_576 - 2; 
const int NUMBER_OF_GROUPS_OF_DELAY_CHIPS_672   = RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_672 - 2; 
//
const int MAX_NUM_LAYERS = 6;
const int MAX_NUM_AFEBS = 42;
const int MAX_NUM_WIRES_PER_LAYER = 112;  // =42 AFEBs * 16 wires/AFEB / 6 layers
const int MAX_NUM_CFEBS  = 5;
const int MAX_NUM_DISTRIPS_PER_LAYER = 40; //=5 CFEBs * 8 distrips/CFEB/layer
//
//
////////////////////////////////////////
// SetUpPulsing constants:
////////////////////////////////////////
const int PULSE_AFEBS      = 0;
const int PULSE_LAYERS     = 1;
//
////////////////////////////////////////
// AFEB control constants
////////////////////////////////////////
const int standby_register_default      = OFF;
const int alct_hot_channel_mask_default = ON;
//
//
////////////////////////////////////////
// AFEB threshold constants
////////////////////////////////////////
const int afeb_threshold_default   = 128;
//
//-----------------------
// ADC channel/chip map
//-----------------------
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
//-----------------------
// DAC channel/chip map
//-----------------------
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
/////////////////////////////////////////////////////////
// Control register bit-map and power-up default values
/////////////////////////////////////////////////////////
const int trigger_mode_bitlo               = 0;
const int trigger_mode_bithi               = 1;
const int trigger_mode_default             = 0;
//
const int ext_trig_enable_bitlo            = 2;
const int ext_trig_enable_bithi            = 2;
const int ext_trig_enable_default          = OFF;
//
const int send_empty_bitlo                 = 3;
const int send_empty_bithi                 = 3;
const int send_empty_default               = OFF;
//
const int inject_bitlo                     = 4;
const int inject_bithi                     = 4;
const int inject_default                   = 0;
//
const int bxc_offset_bitlo                 = 5;
const int bxc_offset_bithi                 = 12;
const int bxc_offset_default               = 1;
//
const int nph_thresh_bitlo                 = 13;
const int nph_thresh_bithi                 = 15;
const int nph_thresh_default               = 2;
//
const int nph_pattern_bitlo                = 16;
const int nph_pattern_bithi                = 18;
const int nph_pattern_default              = 4;
//
const int alct_drift_delay_bitlo           = 19;
const int alct_drift_delay_bithi           = 20;
const int alct_drift_delay_default         = 3;
//
const int alct_fifo_tbins_bitlo            = 21;
const int alct_fifo_tbins_bithi            = 25;
const int alct_fifo_tbins_default          = 7;
//
const int alct_fifo_pretrig_bitlo          = 26;
const int alct_fifo_pretrig_bithi          = 30;
const int alct_fifo_pretrig_default        = 1;
//
const int alct_fifo_mode_bitlo             = 31;
const int alct_fifo_mode_bithi             = 32;
const int alct_fifo_mode_default           = 1;
//
const int accelerator_pretrig_thresh_bitlo = 33;
const int accelerator_pretrig_thresh_bithi = 35;
const int accelerator_pretrig_thresh_default= 0;
//
const int l1a_delay_bitlo                  = 36;
const int l1a_delay_bithi                  = 43;
const int l1a_delay_default                = 0x78;
//
const int l1a_window_bitlo                 = 44;
const int l1a_window_bithi                 = 47;
const int l1a_window_default               = 3;
//
const int alct_l1a_offset_bitlo            = 48;
const int alct_l1a_offset_bithi            = 51;
const int alct_l1a_offset_default          = 1;
//
const int l1a_internal_bitlo               = 52;
const int l1a_internal_bithi               = 52;
const int l1a_internal_default             = OFF;
//
const int board_id_bitlo                   = 53;
const int board_id_bithi                   = 55;
const int board_id_default                 = 5;
//
const int accelerator_pattern_thresh_bitlo = 56;
const int accelerator_pattern_thresh_bithi = 58;  
const int accelerator_pattern_thresh_default= 0;
//
const int ccb_enable_bitlo                 = 60;
const int ccb_enable_bithi                 = 60;
const int ccb_enable_default               = 0;
//
const int config_in_readout_bitlo          = 64;
const int config_in_readout_bithi          = 64;
const int config_in_readout_default        = OFF;
//
const int alct_amode_bitlo                 = 65;
const int alct_amode_bithi                 = 65;
const int alct_amode_default               = 0;
//
const int trigger_info_en_bitlo            = 67;
const int trigger_info_en_bithi            = 67;
const int trigger_info_en_default          = ON;
//
const int sn_select_bitlo                  = 68;
const int sn_select_bithi                  = 68;
const int sn_select_default                = 0;
//
//
/////////////////////////////////////////////////////
// ASIC delay/pattern chip characteristics/bit-maps:
/////////////////////////////////////////////////////
const int NUMBER_OF_CHIPS_PER_GROUP = 6;
const int NUMBER_OF_LINES_PER_CHIP  = 16;
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
const int asic_delay_value_default        = 10;
//
const int asic_pattern_value_bitlo         = 4;
const int asic_pattern_value_bithi         = 19;
const int asic_pattern_value_default       = OFF;
//
const int NUMBER_OF_ASIC_BITS              = asic_pattern_value_bithi-asic_delay_value_bitlo+1;
//
//
////////////////////////////////////////
// Delay-Line Control Register bit-map:
////////////////////////////////////////
const int delay_line_reset_bitlo           = 0;
const int delay_line_reset_bithi           = 0;
const int delay_line_reset_default         = OFF;
//
const int delay_line_settst_bitlo          = 1;
const int delay_line_settst_bithi          = 1;
const int delay_line_settst_default        = ON;       //needed to make algorithm work--not known why...
//
const int delay_line_group_select_bitlo    = 2;
// -> delay_line_group_select_bithi value dependent on ALCT type...
const int delay_line_group_select_default  = OFF;
//
//
////////////////////////////////////////
// Fast Control ID register bit-map:
////////////////////////////////////////
// pre-DAQ-06 format:
const int fastcontrol_regular_mirror_bitlo_predaq06_format       = 0;
const int fastcontrol_regular_mirror_bithi_predaq06_format       = 0;
//
const int fastcontrol_backward_forward_bitlo_predaq06_format     = 1;
const int fastcontrol_backward_forward_bithi_predaq06_format     = 1;
//
const int fastcontrol_negative_positive_bitlo_predaq06_format    = 2;
const int fastcontrol_negative_positive_bithi_predaq06_format    = 2;
//
const int fastcontrol_alct_type_bitlo_predaq06_format            = 4;
const int fastcontrol_alct_type_bithi_predaq06_format            = 7;
//
const int fastcontrol_firmware_year_bitlo_predaq06_format        = 8;
const int fastcontrol_firmware_year_bithi_predaq06_format        =23;
//
const int fastcontrol_firmware_day_bitlo_predaq06_format         =24;
const int fastcontrol_firmware_day_bithi_predaq06_format         =31;
//
const int fastcontrol_firmware_month_bitlo_predaq06_format       =32;
const int fastcontrol_firmware_month_bithi_predaq06_format       =39;
//
// DAQ-06 format:
const int fastcontrol_firmware_version_bitlo_daq06_format     = 0;
const int fastcontrol_firmware_version_bithi_daq06_format     = 5;
//
const int fastcontrol_alct_type_bitlo_daq06_format            = 6;
const int fastcontrol_alct_type_bithi_daq06_format            = 8;
//
const int fastcontrol_backward_forward_bitlo_daq06_format     = 9;
const int fastcontrol_backward_forward_bithi_daq06_format     = 9;
//
const int fastcontrol_negative_positive_bitlo_daq06_format    =10;
const int fastcontrol_negative_positive_bithi_daq06_format    =10;
//
const int fastcontrol_regular_mirror_bitlo_daq06_format       =11;
const int fastcontrol_regular_mirror_bithi_daq06_format       =11;
//
const int fastcontrol_kill_ghosts_bitlo_daq06_format          =12;
const int fastcontrol_kill_ghosts_bithi_daq06_format          =12;
//
const int fastcontrol_reduced_latency_bitlo_daq06_format      =13;
const int fastcontrol_reduced_latency_bithi_daq06_format      =13;
//
const int fastcontrol_pattern_b_enabled_bitlo_daq06_format    =14;
const int fastcontrol_pattern_b_enabled_bithi_daq06_format    =14;
//
const int fastcontrol_firmware_year_bitlo_daq06_format        =19;
const int fastcontrol_firmware_year_bithi_daq06_format        =30;
//
const int fastcontrol_firmware_day_bitlo_daq06_format         =31;
const int fastcontrol_firmware_day_bithi_daq06_format         =35;
//
const int fastcontrol_firmware_month_bitlo_daq06_format       =36;
const int fastcontrol_firmware_month_bithi_daq06_format       =39;
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
const int DO_NOT_CARE                            = -999;
//
const int FIRMWARE_TYPE_192                      = 1;
const int FIRMWARE_TYPE_288                      = 2;
const int FIRMWARE_TYPE_384                      = 3;
const int FIRMWARE_TYPE_576                      = 5;
const int FIRMWARE_TYPE_672                      = 6;
//
// constants added for DAQ06 format:
const int PATTERN_B_DISABLED                     = 0;
const int PATTERN_B_ENABLED                      = 1;
//
const int NOT_REDUCED_LATENCY_FIRMWARE           = 0;
const int REDUCED_LATENCY_FIRMWARE               = 1;
//
const int KILL_GHOSTS_DISABLED                   = 0;
const int KILL_GHOSTS_ENABLED                    = 1;
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
const int trigger_register_source_default  = OFF;
//
const int trigger_register_invert_bitlo    = 4;
const int trigger_register_invert_bithi    = 4;
const int trigger_register_invert_default  = OFF;
//
const int testpulse_power_setting_default = OFF;
const int testpulse_amplitude_default     = 0;
const int testpulse_groupmask_default     = OFF;
const int testpulse_stripmask_default     = OFF;
//
//
/////////////////////////////////////////////////////
// Collision mask constants
/////////////////////////////////////////////////////
const int NUMBER_OF_BITS_IN_COLLISION_MASK_PER_GROUP = 28;
const int NUMBER_OF_WIREGROUPS_PER_COLLISION_PATTERN_GROUP = 8;
//
const int collision_pattern_mask_default = ON;
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
// FPGA ID values:
const int ID_288384 = 0x0a30093;
const int ID_672    = 0x0a40093;
//
const int jtagSourceBoot = 0;
const int jtagSourceFPGA = 1;
//
//
///////////////////////////////////////////////////////
// "database" of allowed ALCT firmware configurations:
///////////////////////////////////////////////////////
struct allowedFirmwareConfiguration {
  // Defines the hardware access to the ALCT
  std::string VCCIpAddress;
  int         tmb_slot    ;
  int         fpga_id     ;
  //
  // Defines configuration as it should be specified in the configuration database
  std::string crateLabel  ;
  std::string chamberLabel;
  //
  // Defines ALCT firmware type to be loaded
  std::string chambertype          ;
  int         regular_mirror_type   ;
  int         negative_positive_type;
  int         backward_forward_type ;
};
//
// crate controller MAC addresses for each peripheral crate
// See http://www.physics.ohio-state.edu/~cms/Netgear_Switch/VME_Crate2Switch_Port.pdf
//
const std::string VCCipAddr_VMEp1_1  = "02:00:00:00:00:16";
const std::string VCCipAddr_VMEp1_2  = "02:00:00:00:00:3C";
const std::string VCCipAddr_VMEp1_3  = "02:00:00:00:00:2C";
const std::string VCCipAddr_VMEp1_4  = "02:00:00:00:00:20";
const std::string VCCipAddr_VMEp1_5  = "02:00:00:00:00:24";
const std::string VCCipAddr_VMEp1_6  = "02:00:00:00:00:34";
const std::string VCCipAddr_VMEp1_7  = "02:00:00:00:00:35";
const std::string VCCipAddr_VMEp1_8  = "02:00:00:00:00:40";
const std::string VCCipAddr_VMEp1_9  = "02:00:00:00:00:41";
const std::string VCCipAddr_VMEp1_10 = "02:00:00:00:00:0F";
const std::string VCCipAddr_VMEp1_11 = "02:00:00:00:00:0C";
const std::string VCCipAddr_VMEp1_12 = "02:00:00:00:00:09";
const std::string VCCipAddr_VMEp2_1  = "02:00:00:00:00:3D";
const std::string VCCipAddr_VMEp2_2  = "02:00:00:00:00:13";
const std::string VCCipAddr_VMEp2_3  = "02:00:00:00:00:17";
const std::string VCCipAddr_VMEp2_4  = "02:00:00:00:00:43";
const std::string VCCipAddr_VMEp2_5  = "02:00:00:00:00:18";
const std::string VCCipAddr_VMEp2_6  = "02:00:00:00:00:0E";
const std::string VCCipAddr_VMEp3_1  = "02:00:00:00:00:0D";
const std::string VCCipAddr_VMEp3_2  = "02:00:00:00:00:1B";
const std::string VCCipAddr_VMEp3_3  = "02:00:00:00:00:04";
const std::string VCCipAddr_VMEp3_4  = "02:00:00:00:00:1A";
const std::string VCCipAddr_VMEp3_5  = "02:00:00:00:00:03";
const std::string VCCipAddr_VMEp3_6  = "02:00:00:00:00:33";
const std::string VCCipAddr_VMEp4_1  = "02:00:00:00:00:2D";
const std::string VCCipAddr_VMEp4_2  = "02:00:00:00:00:32";
const std::string VCCipAddr_VMEp4_3  = "02:00:00:00:00:31";
const std::string VCCipAddr_VMEp4_4  = "02:00:00:00:00:0A";
const std::string VCCipAddr_VMEp4_5  = "02:00:00:00:00:36";
const std::string VCCipAddr_VMEp4_6  = "02:00:00:00:00:44";
//
const std::string VCCipAddr_VMEm1_1  = "02:00:00:00:00:1F";
const std::string VCCipAddr_VMEm1_2  = "02:00:00:00:00:21";
const std::string VCCipAddr_VMEm1_3  = "02:00:00:00:00:22";
const std::string VCCipAddr_VMEm1_4  = "02:00:00:00:00:46";
const std::string VCCipAddr_VMEm1_5  = "02:00:00:00:00:45";
const std::string VCCipAddr_VMEm1_6  = "02:00:00:00:00:4B";
const std::string VCCipAddr_VMEm1_7  = "02:00:00:00:00:49";
const std::string VCCipAddr_VMEm1_8  = "02:00:00:00:00:11";
const std::string VCCipAddr_VMEm1_9  = "02:00:00:00:00:42";
const std::string VCCipAddr_VMEm1_10 = "02:00:00:00:00:14";
const std::string VCCipAddr_VMEm1_11 = "02:00:00:00:00:12";
const std::string VCCipAddr_VMEm1_12 = "02:00:00:00:00:23";
const std::string VCCipAddr_VMEm2_1  = "02:00:00:00:00:28";
const std::string VCCipAddr_VMEm2_2  = "02:00:00:00:00:2A";
const std::string VCCipAddr_VMEm2_3  = "02:00:00:00:00:3E";
const std::string VCCipAddr_VMEm2_4  = "02:00:00:00:00:0B";
const std::string VCCipAddr_VMEm2_5  = "02:00:00:00:00:08";
const std::string VCCipAddr_VMEm2_6  = "02:00:00:00:00:48";
const std::string VCCipAddr_VMEm3_1  = "02:00:00:00:00:15";
const std::string VCCipAddr_VMEm3_2  = "02:00:00:00:00:2E";
const std::string VCCipAddr_VMEm3_3  = "02:00:00:00:00:27";
const std::string VCCipAddr_VMEm3_4  = "02:00:00:00:00:05";
const std::string VCCipAddr_VMEm3_5  = "02:00:00:00:00:10";
const std::string VCCipAddr_VMEm3_6  = "02:00:00:00:00:3F";
const std::string VCCipAddr_VMEm4_1  = "02:00:00:00:00:3B";
const std::string VCCipAddr_VMEm4_2  = "02:00:00:00:00:2B";
const std::string VCCipAddr_VMEm4_3  = "02:00:00:00:00:37";
const std::string VCCipAddr_VMEm4_4  = "02:00:00:00:00:3A";
const std::string VCCipAddr_VMEm4_5  = "02:00:00:00:00:39";
const std::string VCCipAddr_VMEm4_6  = "02:00:00:00:00:38";
//
//----------------------------------------------------------------------
// Hardware/Configuration names/ALCT firmware combinations that must be 
// satisfied in order to allow loading of firmware to ALCT:
//
const unsigned int number_of_allowed_firmware_configurations = 540;
//
const allowedFirmwareConfiguration allowed_firmware_config[number_of_allowed_firmware_configurations] = {
  //
  //ME11 configurations on plus endcap
  //
  {VCCipAddr_VMEp1_1 ,2 ,ID_288384,"VMEp1_1" ,"ME+1/1/36","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },
  {VCCipAddr_VMEp1_1 ,4 ,ID_288384,"VMEp1_1" ,"ME+1/1/1" ,"ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  {VCCipAddr_VMEp1_1 ,6 ,ID_288384,"VMEp1_1" ,"ME+1/1/2" ,"ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },

  {VCCipAddr_VMEp1_2 ,2 ,ID_288384,"VMEp1_2" ,"ME+1/1/3" ,"ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  {VCCipAddr_VMEp1_2 ,4 ,ID_288384,"VMEp1_2" ,"ME+1/1/4" ,"ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },
  {VCCipAddr_VMEp1_2 ,6 ,ID_288384,"VMEp1_2" ,"ME+1/1/5" ,"ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},

  {VCCipAddr_VMEp1_3 ,2 ,ID_288384,"VMEp1_3" ,"ME+1/1/6" ,"ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },
  {VCCipAddr_VMEp1_3 ,4 ,ID_288384,"VMEp1_3" ,"ME+1/1/7" ,"ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  {VCCipAddr_VMEp1_3 ,6 ,ID_288384,"VMEp1_3" ,"ME+1/1/8" ,"ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },

  {VCCipAddr_VMEp1_4 ,2 ,ID_288384,"VMEp1_4" ,"ME+1/1/9" ,"ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  {VCCipAddr_VMEp1_4 ,4 ,ID_288384,"VMEp1_4" ,"ME+1/1/10","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },
  {VCCipAddr_VMEp1_4 ,6 ,ID_288384,"VMEp1_4" ,"ME+1/1/11","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},

  {VCCipAddr_VMEp1_5 ,2 ,ID_288384,"VMEp1_5" ,"ME+1/1/12","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },
  {VCCipAddr_VMEp1_5 ,4 ,ID_288384,"VMEp1_5" ,"ME+1/1/13","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  {VCCipAddr_VMEp1_5 ,6 ,ID_288384,"VMEp1_5" ,"ME+1/1/14","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },

  {VCCipAddr_VMEp1_6 ,2 ,ID_288384,"VMEp1_6" ,"ME+1/1/15","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  {VCCipAddr_VMEp1_6 ,4 ,ID_288384,"VMEp1_6" ,"ME+1/1/16","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },
  {VCCipAddr_VMEp1_6 ,6 ,ID_288384,"VMEp1_6" ,"ME+1/1/17","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},

  {VCCipAddr_VMEp1_7 ,2 ,ID_288384,"VMEp1_7" ,"ME+1/1/18","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },
  {VCCipAddr_VMEp1_7 ,4 ,ID_288384,"VMEp1_7" ,"ME+1/1/19","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  {VCCipAddr_VMEp1_7 ,6 ,ID_288384,"VMEp1_7" ,"ME+1/1/20","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },

  {VCCipAddr_VMEp1_8 ,2 ,ID_288384,"VMEp1_8" ,"ME+1/1/21","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  {VCCipAddr_VMEp1_8 ,4 ,ID_288384,"VMEp1_8" ,"ME+1/1/22","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },
  {VCCipAddr_VMEp1_8 ,6 ,ID_288384,"VMEp1_8" ,"ME+1/1/23","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},

  {VCCipAddr_VMEp1_9 ,2 ,ID_288384,"VMEp1_9" ,"ME+1/1/24","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },
  {VCCipAddr_VMEp1_9 ,4 ,ID_288384,"VMEp1_9" ,"ME+1/1/25","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  {VCCipAddr_VMEp1_9 ,6 ,ID_288384,"VMEp1_9" ,"ME+1/1/26","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },

  {VCCipAddr_VMEp1_10,2 ,ID_288384,"VMEp1_10","ME+1/1/27","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  {VCCipAddr_VMEp1_10,4 ,ID_288384,"VMEp1_10","ME+1/1/28","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },
  {VCCipAddr_VMEp1_10,6 ,ID_288384,"VMEp1_10","ME+1/1/29","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},

  {VCCipAddr_VMEp1_11,2 ,ID_288384,"VMEp1_11","ME+1/1/30","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },
  {VCCipAddr_VMEp1_11,4 ,ID_288384,"VMEp1_11","ME+1/1/31","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  {VCCipAddr_VMEp1_11,6 ,ID_288384,"VMEp1_11","ME+1/1/32","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },

  {VCCipAddr_VMEp1_12,2 ,ID_288384,"VMEp1_12","ME+1/1/33","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  {VCCipAddr_VMEp1_12,4 ,ID_288384,"VMEp1_12","ME+1/1/34","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },
  {VCCipAddr_VMEp1_12,6 ,ID_288384,"VMEp1_12","ME+1/1/35","ME11",REGULAR_FIRMWARE_TYPE,POSITIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  //
  //ME12 configurations on plus endcap
  //
  {VCCipAddr_VMEp1_1 ,8 ,ID_288384,"VMEp1_1" ,"ME+1/2/36","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_1 ,10,ID_288384,"VMEp1_1" ,"ME+1/2/1" ,"ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_1 ,14,ID_288384,"VMEp1_1" ,"ME+1/2/2" ,"ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp1_2 ,8 ,ID_288384,"VMEp1_2" ,"ME+1/2/3" ,"ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_2 ,10,ID_288384,"VMEp1_2" ,"ME+1/2/4" ,"ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_2 ,14,ID_288384,"VMEp1_2" ,"ME+1/2/5" ,"ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp1_3 ,8 ,ID_288384,"VMEp1_3" ,"ME+1/2/6" ,"ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_3 ,10,ID_288384,"VMEp1_3" ,"ME+1/2/7" ,"ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_3 ,14,ID_288384,"VMEp1_3" ,"ME+1/2/8" ,"ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp1_4 ,8 ,ID_288384,"VMEp1_4" ,"ME+1/2/9" ,"ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_4 ,10,ID_288384,"VMEp1_4" ,"ME+1/2/10","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_4 ,14,ID_288384,"VMEp1_4" ,"ME+1/2/11","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp1_5 ,8 ,ID_288384,"VMEp1_5" ,"ME+1/2/12","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_5 ,10,ID_288384,"VMEp1_5" ,"ME+1/2/13","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_5 ,14,ID_288384,"VMEp1_5" ,"ME+1/2/14","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp1_6 ,8 ,ID_288384,"VMEp1_6" ,"ME+1/2/15","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_6 ,10,ID_288384,"VMEp1_6" ,"ME+1/2/16","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_6 ,14,ID_288384,"VMEp1_6" ,"ME+1/2/17","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp1_7 ,8 ,ID_288384,"VMEp1_7" ,"ME+1/2/18","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_7 ,10,ID_288384,"VMEp1_7" ,"ME+1/2/19","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_7 ,14,ID_288384,"VMEp1_7" ,"ME+1/2/20","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp1_8 ,8 ,ID_288384,"VMEp1_8" ,"ME+1/2/21","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_8 ,10,ID_288384,"VMEp1_8" ,"ME+1/2/22","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_8 ,14,ID_288384,"VMEp1_8" ,"ME+1/2/23","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp1_9 ,8 ,ID_288384,"VMEp1_9" ,"ME+1/2/24","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_9 ,10,ID_288384,"VMEp1_9" ,"ME+1/2/25","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_9 ,14,ID_288384,"VMEp1_9" ,"ME+1/2/26","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp1_10,8 ,ID_288384,"VMEp1_10","ME+1/2/27","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_10,10,ID_288384,"VMEp1_10","ME+1/2/28","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_10,14,ID_288384,"VMEp1_10","ME+1/2/29","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp1_11,8 ,ID_288384,"VMEp1_11","ME+1/2/30","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_11,10,ID_288384,"VMEp1_11","ME+1/2/31","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_11,14,ID_288384,"VMEp1_11","ME+1/2/32","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp1_12,8 ,ID_288384,"VMEp1_12","ME+1/2/33","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_12,10,ID_288384,"VMEp1_12","ME+1/2/34","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_12,14,ID_288384,"VMEp1_12","ME+1/2/35","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  //
  //ME13 configurations on plus endcap
  //
  {VCCipAddr_VMEp1_1 ,16,ID_288384,"VMEp1_1" ,"ME+1/3/36","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_1 ,18,ID_288384,"VMEp1_1" ,"ME+1/3/1" ,"ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_1 ,20,ID_288384,"VMEp1_1" ,"ME+1/3/2" ,"ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp1_2 ,16,ID_288384,"VMEp1_2" ,"ME+1/3/3" ,"ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_2 ,18,ID_288384,"VMEp1_2" ,"ME+1/3/4" ,"ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_2 ,20,ID_288384,"VMEp1_2" ,"ME+1/3/5" ,"ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp1_3 ,16,ID_288384,"VMEp1_3" ,"ME+1/3/6" ,"ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_3 ,18,ID_288384,"VMEp1_3" ,"ME+1/3/7" ,"ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_3 ,20,ID_288384,"VMEp1_3" ,"ME+1/3/8" ,"ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp1_4 ,16,ID_288384,"VMEp1_4" ,"ME+1/3/9" ,"ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_4 ,18,ID_288384,"VMEp1_4" ,"ME+1/3/10","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_4 ,20,ID_288384,"VMEp1_4" ,"ME+1/3/11","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp1_5 ,16,ID_288384,"VMEp1_5" ,"ME+1/3/12","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_5 ,18,ID_288384,"VMEp1_5" ,"ME+1/3/13","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_5 ,20,ID_288384,"VMEp1_5" ,"ME+1/3/14","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp1_6 ,16,ID_288384,"VMEp1_6" ,"ME+1/3/15","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_6 ,18,ID_288384,"VMEp1_6" ,"ME+1/3/16","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_6 ,20,ID_288384,"VMEp1_6" ,"ME+1/3/17","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp1_7 ,16,ID_288384,"VMEp1_7" ,"ME+1/3/18","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_7 ,18,ID_288384,"VMEp1_7" ,"ME+1/3/19","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_7 ,20,ID_288384,"VMEp1_7" ,"ME+1/3/20","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp1_8 ,16,ID_288384,"VMEp1_8" ,"ME+1/3/21","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_8 ,18,ID_288384,"VMEp1_8" ,"ME+1/3/22","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_8 ,20,ID_288384,"VMEp1_8" ,"ME+1/3/23","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp1_9 ,16,ID_288384,"VMEp1_9" ,"ME+1/3/24","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_9 ,18,ID_288384,"VMEp1_9" ,"ME+1/3/25","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_9 ,20,ID_288384,"VMEp1_9" ,"ME+1/3/26","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp1_10,16,ID_288384,"VMEp1_10","ME+1/3/27","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_10,18,ID_288384,"VMEp1_10","ME+1/3/28","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_10,20,ID_288384,"VMEp1_10","ME+1/3/29","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp1_11,16,ID_288384,"VMEp1_11","ME+1/3/30","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_11,18,ID_288384,"VMEp1_11","ME+1/3/31","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_11,20,ID_288384,"VMEp1_11","ME+1/3/32","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp1_12,16,ID_288384,"VMEp1_12","ME+1/3/33","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_12,18,ID_288384,"VMEp1_12","ME+1/3/34","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp1_12,20,ID_288384,"VMEp1_12","ME+1/3/35","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  //
  //ME21 configurations on plus endcap
  //
  {VCCipAddr_VMEp2_1 ,2 ,ID_672   ,"VMEp2_1" ,"ME+2/1/2" ,"ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_1 ,4 ,ID_672   ,"VMEp2_1" ,"ME+2/1/3" ,"ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_1 ,6 ,ID_672   ,"VMEp2_1" ,"ME+2/1/4" ,"ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp2_2 ,2 ,ID_672   ,"VMEp2_2" ,"ME+2/1/5" ,"ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_2 ,4 ,ID_672   ,"VMEp2_2" ,"ME+2/1/6" ,"ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_2 ,6 ,ID_672   ,"VMEp2_2" ,"ME+2/1/7" ,"ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp2_3 ,2 ,ID_672   ,"VMEp2_3" ,"ME+2/1/8" ,"ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_3 ,4 ,ID_672   ,"VMEp2_3" ,"ME+2/1/9" ,"ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_3 ,6 ,ID_672   ,"VMEp2_3" ,"ME+2/1/10","ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp2_4 ,2 ,ID_672   ,"VMEp2_4" ,"ME+2/1/11","ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_4 ,4 ,ID_672   ,"VMEp2_4" ,"ME+2/1/12","ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_4 ,6 ,ID_672   ,"VMEp2_4" ,"ME+2/1/13","ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp2_5 ,2 ,ID_672   ,"VMEp2_5" ,"ME+2/1/14","ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_5 ,4 ,ID_672   ,"VMEp2_5" ,"ME+2/1/15","ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_5 ,6 ,ID_672   ,"VMEp2_5" ,"ME+2/1/16","ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp2_6 ,2 ,ID_672   ,"VMEp2_6" ,"ME+2/1/17","ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_6 ,4 ,ID_672   ,"VMEp2_6" ,"ME+2/1/18","ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_6 ,6 ,ID_672   ,"VMEp2_6" ,"ME+2/1/1" ,"ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  //
  //ME22 configurations on plus endcap
  //
  {VCCipAddr_VMEp2_1 ,8 ,ID_288384,"VMEp2_1" ,"ME+2/2/3" ,"ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_1 ,10,ID_288384,"VMEp2_1" ,"ME+2/2/4" ,"ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_1 ,14,ID_288384,"VMEp2_1" ,"ME+2/2/5" ,"ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_1 ,16,ID_288384,"VMEp2_1" ,"ME+2/2/6" ,"ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_1 ,18,ID_288384,"VMEp2_1" ,"ME+2/2/7" ,"ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_1 ,20,ID_288384,"VMEp2_1" ,"ME+2/2/8" ,"ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp2_2 ,8 ,ID_288384,"VMEp2_2" ,"ME+2/2/9" ,"ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_2 ,10,ID_288384,"VMEp2_2" ,"ME+2/2/10","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_2 ,14,ID_288384,"VMEp2_2" ,"ME+2/2/11","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_2 ,16,ID_288384,"VMEp2_2" ,"ME+2/2/12","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_2 ,18,ID_288384,"VMEp2_2" ,"ME+2/2/13","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_2 ,20,ID_288384,"VMEp2_2" ,"ME+2/2/14","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp2_3 ,8 ,ID_288384,"VMEp2_3" ,"ME+2/2/15","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_3 ,10,ID_288384,"VMEp2_3" ,"ME+2/2/16","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_3 ,14,ID_288384,"VMEp2_3" ,"ME+2/2/17","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_3 ,16,ID_288384,"VMEp2_3" ,"ME+2/2/18","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_3 ,18,ID_288384,"VMEp2_3" ,"ME+2/2/19","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_3 ,20,ID_288384,"VMEp2_3" ,"ME+2/2/20","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp2_4 ,8 ,ID_288384,"VMEp2_4" ,"ME+2/2/21","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_4 ,10,ID_288384,"VMEp2_4" ,"ME+2/2/22","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_4 ,14,ID_288384,"VMEp2_4" ,"ME+2/2/23","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_4 ,16,ID_288384,"VMEp2_4" ,"ME+2/2/24","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_4 ,18,ID_288384,"VMEp2_4" ,"ME+2/2/25","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_4 ,20,ID_288384,"VMEp2_4" ,"ME+2/2/26","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp2_5 ,8 ,ID_288384,"VMEp2_5" ,"ME+2/2/27","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_5 ,10,ID_288384,"VMEp2_5" ,"ME+2/2/28","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_5 ,14,ID_288384,"VMEp2_5" ,"ME+2/2/29","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_5 ,16,ID_288384,"VMEp2_5" ,"ME+2/2/30","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_5 ,18,ID_288384,"VMEp2_5" ,"ME+2/2/31","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_5 ,20,ID_288384,"VMEp2_5" ,"ME+2/2/32","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp2_6 ,8 ,ID_288384,"VMEp2_6" ,"ME+2/2/33","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_6 ,10,ID_288384,"VMEp2_6" ,"ME+2/2/34","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_6 ,14,ID_288384,"VMEp2_6" ,"ME+2/2/35","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_6 ,16,ID_288384,"VMEp2_6" ,"ME+2/2/36","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_6 ,18,ID_288384,"VMEp2_6" ,"ME+2/2/1" ,"ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp2_6 ,20,ID_288384,"VMEp2_6" ,"ME+2/2/2" ,"ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  //
  //ME31 configurations on plus endcap
  //
  {VCCipAddr_VMEp3_1 ,2 ,ID_672   ,"VMEp3_1" ,"ME+3/1/2" ,"ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_1 ,4 ,ID_672   ,"VMEp3_1" ,"ME+3/1/3" ,"ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_1 ,6 ,ID_672   ,"VMEp3_1" ,"ME+3/1/4" ,"ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp3_2 ,2 ,ID_672   ,"VMEp3_2" ,"ME+3/1/5" ,"ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_2 ,4 ,ID_672   ,"VMEp3_2" ,"ME+3/1/6" ,"ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_2 ,6 ,ID_672   ,"VMEp3_2" ,"ME+3/1/7" ,"ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp3_3 ,2 ,ID_672   ,"VMEp3_3" ,"ME+3/1/8" ,"ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_3 ,4 ,ID_672   ,"VMEp3_3" ,"ME+3/1/9" ,"ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_3 ,6 ,ID_672   ,"VMEp3_3" ,"ME+3/1/10","ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp3_4 ,2 ,ID_672   ,"VMEp3_4" ,"ME+3/1/11","ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_4 ,4 ,ID_672   ,"VMEp3_4" ,"ME+3/1/12","ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_4 ,6 ,ID_672   ,"VMEp3_4" ,"ME+3/1/13","ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp3_5 ,2 ,ID_672   ,"VMEp3_5" ,"ME+3/1/14","ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_5 ,4 ,ID_672   ,"VMEp3_5" ,"ME+3/1/15","ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_5 ,6 ,ID_672   ,"VMEp3_5" ,"ME+3/1/16","ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp3_6 ,2 ,ID_672   ,"VMEp3_6" ,"ME+3/1/17","ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_6 ,4 ,ID_672   ,"VMEp3_6" ,"ME+3/1/18","ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_6 ,6 ,ID_672   ,"VMEp3_6" ,"ME+3/1/1" ,"ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  //
  //ME32 configurations on plus endcap
  //
  {VCCipAddr_VMEp3_1 ,8 ,ID_288384,"VMEp3_1" ,"ME+3/2/3" ,"ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_1 ,10,ID_288384,"VMEp3_1" ,"ME+3/2/4" ,"ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_1 ,14,ID_288384,"VMEp3_1" ,"ME+3/2/5" ,"ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_1 ,16,ID_288384,"VMEp3_1" ,"ME+3/2/6" ,"ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_1 ,18,ID_288384,"VMEp3_1" ,"ME+3/2/7" ,"ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_1 ,20,ID_288384,"VMEp3_1" ,"ME+3/2/8" ,"ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp3_2 ,8 ,ID_288384,"VMEp3_2" ,"ME+3/2/9" ,"ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_2 ,10,ID_288384,"VMEp3_2" ,"ME+3/2/10","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_2 ,14,ID_288384,"VMEp3_2" ,"ME+3/2/11","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_2 ,16,ID_288384,"VMEp3_2" ,"ME+3/2/12","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_2 ,18,ID_288384,"VMEp3_2" ,"ME+3/2/13","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_2 ,20,ID_288384,"VMEp3_2" ,"ME+3/2/14","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp3_3 ,8 ,ID_288384,"VMEp3_3" ,"ME+3/2/15","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_3 ,10,ID_288384,"VMEp3_3" ,"ME+3/2/16","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_3 ,14,ID_288384,"VMEp3_3" ,"ME+3/2/17","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_3 ,16,ID_288384,"VMEp3_3" ,"ME+3/2/18","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_3 ,18,ID_288384,"VMEp3_3" ,"ME+3/2/19","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_3 ,20,ID_288384,"VMEp3_3" ,"ME+3/2/20","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp3_4 ,8 ,ID_288384,"VMEp3_4" ,"ME+3/2/21","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_4 ,10,ID_288384,"VMEp3_4" ,"ME+3/2/22","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_4 ,14,ID_288384,"VMEp3_4" ,"ME+3/2/23","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_4 ,16,ID_288384,"VMEp3_4" ,"ME+3/2/24","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_4 ,18,ID_288384,"VMEp3_4" ,"ME+3/2/25","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_4 ,20,ID_288384,"VMEp3_4" ,"ME+3/2/26","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp3_5 ,8 ,ID_288384,"VMEp3_5" ,"ME+3/2/27","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_5 ,10,ID_288384,"VMEp3_5" ,"ME+3/2/28","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_5 ,14,ID_288384,"VMEp3_5" ,"ME+3/2/29","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_5 ,16,ID_288384,"VMEp3_5" ,"ME+3/2/30","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_5 ,18,ID_288384,"VMEp3_5" ,"ME+3/2/31","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_5 ,20,ID_288384,"VMEp3_5" ,"ME+3/2/32","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp3_6 ,8 ,ID_288384,"VMEp3_6" ,"ME+3/2/33","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_6 ,10,ID_288384,"VMEp3_6" ,"ME+3/2/34","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_6 ,14,ID_288384,"VMEp3_6" ,"ME+3/2/35","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_6 ,16,ID_288384,"VMEp3_6" ,"ME+3/2/36","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_6 ,18,ID_288384,"VMEp3_6" ,"ME+3/2/1" ,"ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp3_6 ,20,ID_288384,"VMEp3_6" ,"ME+3/2/2" ,"ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  //
  //ME41 configurations on plus endcap
  //
  {VCCipAddr_VMEp4_1 ,2 ,ID_672   ,"VMEp4_1" ,"ME+4/1/2" ,"ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_1 ,4 ,ID_672   ,"VMEp4_1" ,"ME+4/1/3" ,"ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_1 ,6 ,ID_672   ,"VMEp4_1" ,"ME+4/1/4" ,"ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp4_2 ,2 ,ID_672   ,"VMEp4_2" ,"ME+4/1/5" ,"ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_2 ,4 ,ID_672   ,"VMEp4_2" ,"ME+4/1/6" ,"ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_2 ,6 ,ID_672   ,"VMEp4_2" ,"ME+4/1/7" ,"ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp4_3 ,2 ,ID_672   ,"VMEp4_3" ,"ME+4/1/8" ,"ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_3 ,4 ,ID_672   ,"VMEp4_3" ,"ME+4/1/9" ,"ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_3 ,6 ,ID_672   ,"VMEp4_3" ,"ME+4/1/10","ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp4_4 ,2 ,ID_672   ,"VMEp4_4" ,"ME+4/1/11","ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_4 ,4 ,ID_672   ,"VMEp4_4" ,"ME+4/1/12","ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_4 ,6 ,ID_672   ,"VMEp4_4" ,"ME+4/1/13","ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp4_5 ,2 ,ID_672   ,"VMEp4_5" ,"ME+4/1/14","ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_5 ,4 ,ID_672   ,"VMEp4_5" ,"ME+4/1/15","ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_5 ,6 ,ID_672   ,"VMEp4_5" ,"ME+4/1/16","ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp4_6 ,2 ,ID_672   ,"VMEp4_6" ,"ME+4/1/17","ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_6 ,4 ,ID_672   ,"VMEp4_6" ,"ME+4/1/18","ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_6 ,6 ,ID_672   ,"VMEp4_6" ,"ME+4/1/1" ,"ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  //
  //ME42 configurations on plus endcap
  //
  {VCCipAddr_VMEp4_1 ,8 ,ID_288384,"VMEp4_1" ,"ME+4/2/3" ,"ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_1 ,10,ID_288384,"VMEp4_1" ,"ME+4/2/4" ,"ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_1 ,14,ID_288384,"VMEp4_1" ,"ME+4/2/5" ,"ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_1 ,16,ID_288384,"VMEp4_1" ,"ME+4/2/6" ,"ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_1 ,18,ID_288384,"VMEp4_1" ,"ME+4/2/7" ,"ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_1 ,20,ID_288384,"VMEp4_1" ,"ME+4/2/8" ,"ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp4_2 ,8 ,ID_288384,"VMEp4_2" ,"ME+4/2/9" ,"ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_2 ,10,ID_288384,"VMEp4_2" ,"ME+4/2/10","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_2 ,14,ID_288384,"VMEp4_2" ,"ME+4/2/11","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_2 ,16,ID_288384,"VMEp4_2" ,"ME+4/2/12","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_2 ,18,ID_288384,"VMEp4_2" ,"ME+4/2/13","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_2 ,20,ID_288384,"VMEp4_2" ,"ME+4/2/14","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp4_3 ,8 ,ID_288384,"VMEp4_3" ,"ME+4/2/15","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_3 ,10,ID_288384,"VMEp4_3" ,"ME+4/2/16","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_3 ,14,ID_288384,"VMEp4_3" ,"ME+4/2/17","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_3 ,16,ID_288384,"VMEp4_3" ,"ME+4/2/18","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_3 ,18,ID_288384,"VMEp4_3" ,"ME+4/2/19","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_3 ,20,ID_288384,"VMEp4_3" ,"ME+4/2/20","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp4_4 ,8 ,ID_288384,"VMEp4_4" ,"ME+4/2/21","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_4 ,10,ID_288384,"VMEp4_4" ,"ME+4/2/22","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_4 ,14,ID_288384,"VMEp4_4" ,"ME+4/2/23","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_4 ,16,ID_288384,"VMEp4_4" ,"ME+4/2/24","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_4 ,18,ID_288384,"VMEp4_4" ,"ME+4/2/25","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_4 ,20,ID_288384,"VMEp4_4" ,"ME+4/2/26","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp4_5 ,8 ,ID_288384,"VMEp4_5" ,"ME+4/2/27","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_5 ,10,ID_288384,"VMEp4_5" ,"ME+4/2/28","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_5 ,14,ID_288384,"VMEp4_5" ,"ME+4/2/29","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_5 ,16,ID_288384,"VMEp4_5" ,"ME+4/2/30","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_5 ,18,ID_288384,"VMEp4_5" ,"ME+4/2/31","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_5 ,20,ID_288384,"VMEp4_5" ,"ME+4/2/32","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEp4_6 ,8 ,ID_288384,"VMEp4_6" ,"ME+4/2/33","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_6 ,10,ID_288384,"VMEp4_6" ,"ME+4/2/34","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_6 ,14,ID_288384,"VMEp4_6" ,"ME+4/2/35","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_6 ,16,ID_288384,"VMEp4_6" ,"ME+4/2/36","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_6 ,18,ID_288384,"VMEp4_6" ,"ME+4/2/1" ,"ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEp4_6 ,20,ID_288384,"VMEp4_6" ,"ME+4/2/2" ,"ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  //
  //ME11 configurations on minus endcap
  //
  {VCCipAddr_VMEm1_1 ,2 ,ID_288384,"VMEm1_1" ,"ME-1/1/36","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },
  {VCCipAddr_VMEm1_1 ,4 ,ID_288384,"VMEm1_1" ,"ME-1/1/1" ,"ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  {VCCipAddr_VMEm1_1 ,6 ,ID_288384,"VMEm1_1" ,"ME-1/1/2" ,"ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },

  {VCCipAddr_VMEm1_2 ,2 ,ID_288384,"VMEm1_2" ,"ME-1/1/3" ,"ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  {VCCipAddr_VMEm1_2 ,4 ,ID_288384,"VMEm1_2" ,"ME-1/1/4" ,"ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },
  {VCCipAddr_VMEm1_2 ,6 ,ID_288384,"VMEm1_2" ,"ME-1/1/5" ,"ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},

  {VCCipAddr_VMEm1_3 ,2 ,ID_288384,"VMEm1_3" ,"ME-1/1/6" ,"ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },
  {VCCipAddr_VMEm1_3 ,4 ,ID_288384,"VMEm1_3" ,"ME-1/1/7" ,"ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  {VCCipAddr_VMEm1_3 ,6 ,ID_288384,"VMEm1_3" ,"ME-1/1/8" ,"ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },

  {VCCipAddr_VMEm1_4 ,2 ,ID_288384,"VMEm1_4" ,"ME-1/1/9" ,"ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  {VCCipAddr_VMEm1_4 ,4 ,ID_288384,"VMEm1_4" ,"ME-1/1/10","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },
  {VCCipAddr_VMEm1_4 ,6 ,ID_288384,"VMEm1_4" ,"ME-1/1/11","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},

  {VCCipAddr_VMEm1_5 ,2 ,ID_288384,"VMEm1_5" ,"ME-1/1/12","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },
  {VCCipAddr_VMEm1_5 ,4 ,ID_288384,"VMEm1_5" ,"ME-1/1/13","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  {VCCipAddr_VMEm1_5 ,6 ,ID_288384,"VMEm1_5" ,"ME-1/1/14","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },

  {VCCipAddr_VMEm1_6 ,2 ,ID_288384,"VMEm1_6" ,"ME-1/1/15","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  {VCCipAddr_VMEm1_6 ,4 ,ID_288384,"VMEm1_6" ,"ME-1/1/16","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },
  {VCCipAddr_VMEm1_6 ,6 ,ID_288384,"VMEm1_6" ,"ME-1/1/17","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},

  {VCCipAddr_VMEm1_7 ,2 ,ID_288384,"VMEm1_7" ,"ME-1/1/18","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },
  {VCCipAddr_VMEm1_7 ,4 ,ID_288384,"VMEm1_7" ,"ME-1/1/19","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  {VCCipAddr_VMEm1_7 ,6 ,ID_288384,"VMEm1_7" ,"ME-1/1/20","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },

  {VCCipAddr_VMEm1_8 ,2 ,ID_288384,"VMEm1_8" ,"ME-1/1/21","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  {VCCipAddr_VMEm1_8 ,4 ,ID_288384,"VMEm1_8" ,"ME-1/1/22","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },
  {VCCipAddr_VMEm1_8 ,6 ,ID_288384,"VMEm1_8" ,"ME-1/1/23","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},

  {VCCipAddr_VMEm1_9 ,2 ,ID_288384,"VMEm1_9" ,"ME-1/1/24","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },
  {VCCipAddr_VMEm1_9 ,4 ,ID_288384,"VMEm1_9" ,"ME-1/1/25","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  {VCCipAddr_VMEm1_9 ,6 ,ID_288384,"VMEm1_9" ,"ME-1/1/26","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },

  {VCCipAddr_VMEm1_10,2 ,ID_288384,"VMEm1_10","ME-1/1/27","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  {VCCipAddr_VMEm1_10,4 ,ID_288384,"VMEm1_10","ME-1/1/28","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },
  {VCCipAddr_VMEm1_10,6 ,ID_288384,"VMEm1_10","ME-1/1/29","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},

  {VCCipAddr_VMEm1_11,2 ,ID_288384,"VMEm1_11","ME-1/1/30","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },
  {VCCipAddr_VMEm1_11,4 ,ID_288384,"VMEm1_11","ME-1/1/31","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  {VCCipAddr_VMEm1_11,6 ,ID_288384,"VMEm1_11","ME-1/1/32","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },

  {VCCipAddr_VMEm1_12,2 ,ID_288384,"VMEm1_12","ME-1/1/33","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  {VCCipAddr_VMEm1_12,4 ,ID_288384,"VMEm1_12","ME-1/1/34","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,FORWARD_FIRMWARE_TYPE },
  {VCCipAddr_VMEm1_12,6 ,ID_288384,"VMEm1_12","ME-1/1/35","ME11",REGULAR_FIRMWARE_TYPE,NEGATIVE_FIRMWARE_TYPE,BACKWARD_FIRMWARE_TYPE},
  //
  //ME12 configurations on minus endcap
  //
  {VCCipAddr_VMEm1_1 ,8 ,ID_288384,"VMEm1_1" ,"ME-1/2/36","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_1 ,10,ID_288384,"VMEm1_1" ,"ME-1/2/1" ,"ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_1 ,14,ID_288384,"VMEm1_1" ,"ME-1/2/2" ,"ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm1_2 ,8 ,ID_288384,"VMEm1_2" ,"ME-1/2/3" ,"ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_2 ,10,ID_288384,"VMEm1_2" ,"ME-1/2/4" ,"ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_2 ,14,ID_288384,"VMEm1_2" ,"ME-1/2/5" ,"ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm1_3 ,8 ,ID_288384,"VMEm1_3" ,"ME-1/2/6" ,"ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_3 ,10,ID_288384,"VMEm1_3" ,"ME-1/2/7" ,"ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_3 ,14,ID_288384,"VMEm1_3" ,"ME-1/2/8" ,"ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm1_4 ,8 ,ID_288384,"VMEm1_4" ,"ME-1/2/9" ,"ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_4 ,10,ID_288384,"VMEm1_4" ,"ME-1/2/10","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_4 ,14,ID_288384,"VMEm1_4" ,"ME-1/2/11","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm1_5 ,8 ,ID_288384,"VMEm1_5" ,"ME-1/2/12","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_5 ,10,ID_288384,"VMEm1_5" ,"ME-1/2/13","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_5 ,14,ID_288384,"VMEm1_5" ,"ME-1/2/14","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm1_6 ,8 ,ID_288384,"VMEm1_6" ,"ME-1/2/15","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_6 ,10,ID_288384,"VMEm1_6" ,"ME-1/2/16","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_6 ,14,ID_288384,"VMEm1_6" ,"ME-1/2/17","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm1_7 ,8 ,ID_288384,"VMEm1_7" ,"ME-1/2/18","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_7 ,10,ID_288384,"VMEm1_7" ,"ME-1/2/19","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_7 ,14,ID_288384,"VMEm1_7" ,"ME-1/2/20","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm1_8 ,8 ,ID_288384,"VMEm1_8" ,"ME-1/2/21","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_8 ,10,ID_288384,"VMEm1_8" ,"ME-1/2/22","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_8 ,14,ID_288384,"VMEm1_8" ,"ME-1/2/23","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm1_9 ,8 ,ID_288384,"VMEm1_9" ,"ME-1/2/24","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_9 ,10,ID_288384,"VMEm1_9" ,"ME-1/2/25","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_9 ,14,ID_288384,"VMEm1_9" ,"ME-1/2/26","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm1_10,8 ,ID_288384,"VMEm1_10","ME-1/2/27","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_10,10,ID_288384,"VMEm1_10","ME-1/2/28","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_10,14,ID_288384,"VMEm1_10","ME-1/2/29","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm1_11,8 ,ID_288384,"VMEm1_11","ME-1/2/30","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_11,10,ID_288384,"VMEm1_11","ME-1/2/31","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_11,14,ID_288384,"VMEm1_11","ME-1/2/32","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm1_12,8 ,ID_288384,"VMEm1_12","ME-1/2/33","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_12,10,ID_288384,"VMEm1_12","ME-1/2/34","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_12,14,ID_288384,"VMEm1_12","ME-1/2/35","ME12",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  //
  //ME13 configurations on minus endcap
  //
  {VCCipAddr_VMEm1_1 ,16,ID_288384,"VMEm1_1" ,"ME-1/3/36","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_1 ,18,ID_288384,"VMEm1_1" ,"ME-1/3/1" ,"ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_1 ,20,ID_288384,"VMEm1_1" ,"ME-1/3/2" ,"ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm1_2 ,16,ID_288384,"VMEm1_2" ,"ME-1/3/3" ,"ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_2 ,18,ID_288384,"VMEm1_2" ,"ME-1/3/4" ,"ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_2 ,20,ID_288384,"VMEm1_2" ,"ME-1/3/5" ,"ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm1_3 ,16,ID_288384,"VMEm1_3" ,"ME-1/3/6" ,"ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_3 ,18,ID_288384,"VMEm1_3" ,"ME-1/3/7" ,"ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_3 ,20,ID_288384,"VMEm1_3" ,"ME-1/3/8" ,"ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm1_4 ,16,ID_288384,"VMEm1_4" ,"ME-1/3/9" ,"ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_4 ,18,ID_288384,"VMEm1_4" ,"ME-1/3/10","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_4 ,20,ID_288384,"VMEm1_4" ,"ME-1/3/11","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm1_5 ,16,ID_288384,"VMEm1_5" ,"ME-1/3/12","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_5 ,18,ID_288384,"VMEm1_5" ,"ME-1/3/13","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_5 ,20,ID_288384,"VMEm1_5" ,"ME-1/3/14","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm1_6 ,16,ID_288384,"VMEm1_6" ,"ME-1/3/15","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_6 ,18,ID_288384,"VMEm1_6" ,"ME-1/3/16","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_6 ,20,ID_288384,"VMEm1_6" ,"ME-1/3/17","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm1_7 ,16,ID_288384,"VMEm1_7" ,"ME-1/3/18","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_7 ,18,ID_288384,"VMEm1_7" ,"ME-1/3/19","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_7 ,20,ID_288384,"VMEm1_7" ,"ME-1/3/20","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm1_8 ,16,ID_288384,"VMEm1_8" ,"ME-1/3/21","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_8 ,18,ID_288384,"VMEm1_8" ,"ME-1/3/22","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_8 ,20,ID_288384,"VMEm1_8" ,"ME-1/3/23","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm1_9 ,16,ID_288384,"VMEm1_9" ,"ME-1/3/24","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_9 ,18,ID_288384,"VMEm1_9" ,"ME-1/3/25","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_9 ,20,ID_288384,"VMEm1_9" ,"ME-1/3/26","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm1_10,16,ID_288384,"VMEm1_10","ME-1/3/27","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_10,18,ID_288384,"VMEm1_10","ME-1/3/28","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_10,20,ID_288384,"VMEm1_10","ME-1/3/29","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm1_11,16,ID_288384,"VMEm1_11","ME-1/3/30","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_11,18,ID_288384,"VMEm1_11","ME-1/3/31","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_11,20,ID_288384,"VMEm1_11","ME-1/3/32","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm1_12,16,ID_288384,"VMEm1_12","ME-1/3/33","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_12,18,ID_288384,"VMEm1_12","ME-1/3/34","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm1_12,20,ID_288384,"VMEm1_12","ME-1/3/35","ME13",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  //
  //ME21 configurations on minus endcap
  //
  {VCCipAddr_VMEm2_1 ,2 ,ID_672   ,"VMEm2_1" ,"ME-2/1/2" ,"ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_1 ,4 ,ID_672   ,"VMEm2_1" ,"ME-2/1/3" ,"ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_1 ,6 ,ID_672   ,"VMEm2_1" ,"ME-2/1/4" ,"ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm2_2 ,2 ,ID_672   ,"VMEm2_2" ,"ME-2/1/5" ,"ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_2 ,4 ,ID_672   ,"VMEm2_2" ,"ME-2/1/6" ,"ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_2 ,6 ,ID_672   ,"VMEm2_2" ,"ME-2/1/7" ,"ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm2_3 ,2 ,ID_672   ,"VMEm2_3" ,"ME-2/1/8" ,"ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_3 ,4 ,ID_672   ,"VMEm2_3" ,"ME-2/1/9" ,"ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_3 ,6 ,ID_672   ,"VMEm2_3" ,"ME-2/1/10","ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm2_4 ,2 ,ID_672   ,"VMEm2_4" ,"ME-2/1/11","ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_4 ,4 ,ID_672   ,"VMEm2_4" ,"ME-2/1/12","ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_4 ,6 ,ID_672   ,"VMEm2_4" ,"ME-2/1/13","ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm2_5 ,2 ,ID_672   ,"VMEm2_5" ,"ME-2/1/14","ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_5 ,4 ,ID_672   ,"VMEm2_5" ,"ME-2/1/15","ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_5 ,6 ,ID_672   ,"VMEm2_5" ,"ME-2/1/16","ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm2_6 ,2 ,ID_672   ,"VMEm2_6" ,"ME-2/1/17","ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_6 ,4 ,ID_672   ,"VMEm2_6" ,"ME-2/1/18","ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_6 ,6 ,ID_672   ,"VMEm2_6" ,"ME-2/1/1" ,"ME21",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  //
  //ME22 configurations on minus endcap
  //
  {VCCipAddr_VMEm2_1 ,8 ,ID_288384,"VMEm2_1" ,"ME-2/2/3" ,"ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_1 ,10,ID_288384,"VMEm2_1" ,"ME-2/2/4" ,"ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_1 ,14,ID_288384,"VMEm2_1" ,"ME-2/2/5" ,"ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_1 ,16,ID_288384,"VMEm2_1" ,"ME-2/2/6" ,"ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_1 ,18,ID_288384,"VMEm2_1" ,"ME-2/2/7" ,"ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_1 ,20,ID_288384,"VMEm2_1" ,"ME-2/2/8" ,"ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm2_2 ,8 ,ID_288384,"VMEm2_2" ,"ME-2/2/9" ,"ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_2 ,10,ID_288384,"VMEm2_2" ,"ME-2/2/10","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_2 ,14,ID_288384,"VMEm2_2" ,"ME-2/2/11","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_2 ,16,ID_288384,"VMEm2_2" ,"ME-2/2/12","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_2 ,18,ID_288384,"VMEm2_2" ,"ME-2/2/13","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_2 ,20,ID_288384,"VMEm2_2" ,"ME-2/2/14","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm2_3 ,8 ,ID_288384,"VMEm2_3" ,"ME-2/2/15","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_3 ,10,ID_288384,"VMEm2_3" ,"ME-2/2/16","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_3 ,14,ID_288384,"VMEm2_3" ,"ME-2/2/17","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_3 ,16,ID_288384,"VMEm2_3" ,"ME-2/2/18","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_3 ,18,ID_288384,"VMEm2_3" ,"ME-2/2/19","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_3 ,20,ID_288384,"VMEm2_3" ,"ME-2/2/20","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm2_4 ,8 ,ID_288384,"VMEm2_4" ,"ME-2/2/21","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_4 ,10,ID_288384,"VMEm2_4" ,"ME-2/2/22","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_4 ,14,ID_288384,"VMEm2_4" ,"ME-2/2/23","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_4 ,16,ID_288384,"VMEm2_4" ,"ME-2/2/24","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_4 ,18,ID_288384,"VMEm2_4" ,"ME-2/2/25","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_4 ,20,ID_288384,"VMEm2_4" ,"ME-2/2/26","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm2_5 ,8 ,ID_288384,"VMEm2_5" ,"ME-2/2/27","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_5 ,10,ID_288384,"VMEm2_5" ,"ME-2/2/28","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_5 ,14,ID_288384,"VMEm2_5" ,"ME-2/2/29","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_5 ,16,ID_288384,"VMEm2_5" ,"ME-2/2/30","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_5 ,18,ID_288384,"VMEm2_5" ,"ME-2/2/31","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_5 ,20,ID_288384,"VMEm2_5" ,"ME-2/2/32","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm2_6 ,8 ,ID_288384,"VMEm2_6" ,"ME-2/2/33","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_6 ,10,ID_288384,"VMEm2_6" ,"ME-2/2/34","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_6 ,14,ID_288384,"VMEm2_6" ,"ME-2/2/35","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_6 ,16,ID_288384,"VMEm2_6" ,"ME-2/2/36","ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_6 ,18,ID_288384,"VMEm2_6" ,"ME-2/2/1" ,"ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm2_6 ,20,ID_288384,"VMEm2_6" ,"ME-2/2/2" ,"ME22",REGULAR_FIRMWARE_TYPE,DO_NOT_CARE           ,DO_NOT_CARE           },
  //
  //ME31 configurations on minus endcap
  //
  {VCCipAddr_VMEm3_1 ,2 ,ID_672   ,"VMEm3_1" ,"ME-3/1/2" ,"ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_1 ,4 ,ID_672   ,"VMEm3_1" ,"ME-3/1/3" ,"ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_1 ,6 ,ID_672   ,"VMEm3_1" ,"ME-3/1/4" ,"ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm3_2 ,2 ,ID_672   ,"VMEm3_2" ,"ME-3/1/5" ,"ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_2 ,4 ,ID_672   ,"VMEm3_2" ,"ME-3/1/6" ,"ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_2 ,6 ,ID_672   ,"VMEm3_2" ,"ME-3/1/7" ,"ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm3_3 ,2 ,ID_672   ,"VMEm3_3" ,"ME-3/1/8" ,"ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_3 ,4 ,ID_672   ,"VMEm3_3" ,"ME-3/1/9" ,"ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_3 ,6 ,ID_672   ,"VMEm3_3" ,"ME-3/1/10","ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm3_4 ,2 ,ID_672   ,"VMEm3_4" ,"ME-3/1/11","ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_4 ,4 ,ID_672   ,"VMEm3_4" ,"ME-3/1/12","ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_4 ,6 ,ID_672   ,"VMEm3_4" ,"ME-3/1/13","ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm3_5 ,2 ,ID_672   ,"VMEm3_5" ,"ME-3/1/14","ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_5 ,4 ,ID_672   ,"VMEm3_5" ,"ME-3/1/15","ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_5 ,6 ,ID_672   ,"VMEm3_5" ,"ME-3/1/16","ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm3_6 ,2 ,ID_672   ,"VMEm3_6" ,"ME-3/1/17","ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_6 ,4 ,ID_672   ,"VMEm3_6" ,"ME-3/1/18","ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_6 ,6 ,ID_672   ,"VMEm3_6" ,"ME-3/1/1" ,"ME31",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  //
  //ME32 configurations on minus endcap
  //
  {VCCipAddr_VMEm3_1 ,8 ,ID_288384,"VMEm3_1" ,"ME-3/2/3" ,"ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_1 ,10,ID_288384,"VMEm3_1" ,"ME-3/2/4" ,"ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_1 ,14,ID_288384,"VMEm3_1" ,"ME-3/2/5" ,"ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_1 ,16,ID_288384,"VMEm3_1" ,"ME-3/2/6" ,"ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_1 ,18,ID_288384,"VMEm3_1" ,"ME-3/2/7" ,"ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_1 ,20,ID_288384,"VMEm3_1" ,"ME-3/2/8" ,"ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm3_2 ,8 ,ID_288384,"VMEm3_2" ,"ME-3/2/9" ,"ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_2 ,10,ID_288384,"VMEm3_2" ,"ME-3/2/10","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_2 ,14,ID_288384,"VMEm3_2" ,"ME-3/2/11","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_2 ,16,ID_288384,"VMEm3_2" ,"ME-3/2/12","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_2 ,18,ID_288384,"VMEm3_2" ,"ME-3/2/13","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_2 ,20,ID_288384,"VMEm3_2" ,"ME-3/2/14","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm3_3 ,8 ,ID_288384,"VMEm3_3" ,"ME-3/2/15","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_3 ,10,ID_288384,"VMEm3_3" ,"ME-3/2/16","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_3 ,14,ID_288384,"VMEm3_3" ,"ME-3/2/17","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_3 ,16,ID_288384,"VMEm3_3" ,"ME-3/2/18","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_3 ,18,ID_288384,"VMEm3_3" ,"ME-3/2/19","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_3 ,20,ID_288384,"VMEm3_3" ,"ME-3/2/20","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm3_4 ,8 ,ID_288384,"VMEm3_4" ,"ME-3/2/21","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_4 ,10,ID_288384,"VMEm3_4" ,"ME-3/2/22","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_4 ,14,ID_288384,"VMEm3_4" ,"ME-3/2/23","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_4 ,16,ID_288384,"VMEm3_4" ,"ME-3/2/24","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_4 ,18,ID_288384,"VMEm3_4" ,"ME-3/2/25","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_4 ,20,ID_288384,"VMEm3_4" ,"ME-3/2/26","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm3_5 ,8 ,ID_288384,"VMEm3_5" ,"ME-3/2/27","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_5 ,10,ID_288384,"VMEm3_5" ,"ME-3/2/28","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_5 ,14,ID_288384,"VMEm3_5" ,"ME-3/2/29","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_5 ,16,ID_288384,"VMEm3_5" ,"ME-3/2/30","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_5 ,18,ID_288384,"VMEm3_5" ,"ME-3/2/31","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_5 ,20,ID_288384,"VMEm3_5" ,"ME-3/2/32","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm3_6 ,8 ,ID_288384,"VMEm3_6" ,"ME-3/2/33","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_6 ,10,ID_288384,"VMEm3_6" ,"ME-3/2/34","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_6 ,14,ID_288384,"VMEm3_6" ,"ME-3/2/35","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_6 ,16,ID_288384,"VMEm3_6" ,"ME-3/2/36","ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_6 ,18,ID_288384,"VMEm3_6" ,"ME-3/2/1" ,"ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm3_6 ,20,ID_288384,"VMEm3_6" ,"ME-3/2/2" ,"ME32",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  //
  //ME41 configurations on minus endcap
  //
  {VCCipAddr_VMEm4_1 ,2 ,ID_672   ,"VMEm4_1" ,"ME-4/1/2" ,"ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_1 ,4 ,ID_672   ,"VMEm4_1" ,"ME-4/1/3" ,"ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_1 ,6 ,ID_672   ,"VMEm4_1" ,"ME-4/1/4" ,"ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm4_2 ,2 ,ID_672   ,"VMEm4_2" ,"ME-4/1/5" ,"ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_2 ,4 ,ID_672   ,"VMEm4_2" ,"ME-4/1/6" ,"ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_2 ,6 ,ID_672   ,"VMEm4_2" ,"ME-4/1/7" ,"ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm4_3 ,2 ,ID_672   ,"VMEm4_3" ,"ME-4/1/8" ,"ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_3 ,4 ,ID_672   ,"VMEm4_3" ,"ME-4/1/9" ,"ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_3 ,6 ,ID_672   ,"VMEm4_3" ,"ME-4/1/10","ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm4_4 ,2 ,ID_672   ,"VMEm4_4" ,"ME-4/1/11","ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_4 ,4 ,ID_672   ,"VMEm4_4" ,"ME-4/1/12","ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_4 ,6 ,ID_672   ,"VMEm4_4" ,"ME-4/1/13","ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm4_5 ,2 ,ID_672   ,"VMEm4_5" ,"ME-4/1/14","ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_5 ,4 ,ID_672   ,"VMEm4_5" ,"ME-4/1/15","ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_5 ,6 ,ID_672   ,"VMEm4_5" ,"ME-4/1/16","ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm4_6 ,2 ,ID_672   ,"VMEm4_6" ,"ME-4/1/17","ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_6 ,4 ,ID_672   ,"VMEm4_6" ,"ME-4/1/18","ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_6 ,6 ,ID_672   ,"VMEm4_6" ,"ME-4/1/1" ,"ME41",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm4_1 ,8 ,ID_288384,"VMEm4_1" ,"ME-4/2/3" ,"ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_1 ,10,ID_288384,"VMEm4_1" ,"ME-4/2/4" ,"ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_1 ,14,ID_288384,"VMEm4_1" ,"ME-4/2/5" ,"ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_1 ,16,ID_288384,"VMEm4_1" ,"ME-4/2/6" ,"ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_1 ,18,ID_288384,"VMEm4_1" ,"ME-4/2/7" ,"ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_1 ,20,ID_288384,"VMEm4_1" ,"ME-4/2/8" ,"ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  //
  //ME42 configurations on minus endcap
  //
  {VCCipAddr_VMEm4_2 ,8 ,ID_288384,"VMEm4_2" ,"ME-4/2/9" ,"ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_2 ,10,ID_288384,"VMEm4_2" ,"ME-4/2/10","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_2 ,14,ID_288384,"VMEm4_2" ,"ME-4/2/11","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_2 ,16,ID_288384,"VMEm4_2" ,"ME-4/2/12","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_2 ,18,ID_288384,"VMEm4_2" ,"ME-4/2/13","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_2 ,20,ID_288384,"VMEm4_2" ,"ME-4/2/14","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm4_3 ,8 ,ID_288384,"VMEm4_3" ,"ME-4/2/15","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_3 ,10,ID_288384,"VMEm4_3" ,"ME-4/2/16","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_3 ,14,ID_288384,"VMEm4_3" ,"ME-4/2/17","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_3 ,16,ID_288384,"VMEm4_3" ,"ME-4/2/18","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_3 ,18,ID_288384,"VMEm4_3" ,"ME-4/2/19","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_3 ,20,ID_288384,"VMEm4_3" ,"ME-4/2/20","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm4_4 ,8 ,ID_288384,"VMEm4_4" ,"ME-4/2/21","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_4 ,10,ID_288384,"VMEm4_4" ,"ME-4/2/22","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_4 ,14,ID_288384,"VMEm4_4" ,"ME-4/2/23","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_4 ,16,ID_288384,"VMEm4_4" ,"ME-4/2/24","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_4 ,18,ID_288384,"VMEm4_4" ,"ME-4/2/25","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_4 ,20,ID_288384,"VMEm4_4" ,"ME-4/2/26","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm4_5 ,8 ,ID_288384,"VMEm4_5" ,"ME-4/2/27","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_5 ,10,ID_288384,"VMEm4_5" ,"ME-4/2/28","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_5 ,14,ID_288384,"VMEm4_5" ,"ME-4/2/29","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_5 ,16,ID_288384,"VMEm4_5" ,"ME-4/2/30","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_5 ,18,ID_288384,"VMEm4_5" ,"ME-4/2/31","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_5 ,20,ID_288384,"VMEm4_5" ,"ME-4/2/32","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },

  {VCCipAddr_VMEm4_6 ,8 ,ID_288384,"VMEm4_6" ,"ME-4/2/33","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_6 ,10,ID_288384,"VMEm4_6" ,"ME-4/2/34","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_6 ,14,ID_288384,"VMEm4_6" ,"ME-4/2/35","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_6 ,16,ID_288384,"VMEm4_6" ,"ME-4/2/36","ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_6 ,18,ID_288384,"VMEm4_6" ,"ME-4/2/1" ,"ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           },
  {VCCipAddr_VMEm4_6 ,20,ID_288384,"VMEm4_6" ,"ME-4/2/2" ,"ME42",MIRROR_FIRMWARE_TYPE ,DO_NOT_CARE           ,DO_NOT_CARE           }
  //
};
//
#endif
