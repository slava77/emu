//-----------------------------------------------------------------------
// $Id: TMB_constants.h,v 3.19 2008/11/28 09:49:27 rakness Exp $
// $Log: TMB_constants.h,v $
// Revision 3.19  2008/11/28 09:49:27  rakness
// include ME1/1 TMB firmware compilation specification into xml file
//
// Revision 3.18  2008/11/24 17:50:40  rakness
// update for TMB version 18 Nov 2008
//
// Revision 3.17  2008/08/13 11:30:53  geurts
// introduce emu::pc:: namespaces
// remove any occurences of "using namespace" and make std:: references explicit
//
// Revision 3.16  2008/07/16 17:28:36  rakness
// (backwards incompatible!) updates for 3 June 2008 TMB firmware and v3 r10 DMB firmware
//
// Revision 3.15  2008/02/28 18:36:36  rakness
// make TMB firmware loading robust against all failure modes except power cuts...
//
// Revision 3.14  2008/01/07 15:08:53  rakness
// add xml parameters:  clct_stagger, clct_blanking, clct_pattern_id_thresh, aff_thresh, min_clct_separation.  Remove xml parameter:  clct_distrip_pretrig_thresh
//
// Revision 3.13  2007/10/25 17:36:12  rakness
// Add option to enable/disable write to USER JTAG register to allow selective masking of broadcast JTAG commands.  Also enable/disable clocks with explicit write rather than read,write
//
// Revision 3.12  2007/10/08 15:04:32  rakness
// add ALCT and TMB raw hits button in hyperDAQ
//
// Revision 3.11  2007/06/21 16:14:02  rakness
// online measurement of ALCT in CLCT matching window
//
// Revision 3.10  2007/05/17 12:52:50  rakness
// ignore_ccb_startstop added to TMB configuration + write configuration to userPROM default
//
// Revision 3.9  2007/04/10 13:31:01  rakness
// add mpc_output_enable, remove rpc2/3
//
// Revision 3.8  2007/01/31 16:49:53  rakness
// complete set of TMB/ALCT/RAT xml parameters
//
// Revision 3.7  2006/11/10 12:43:07  rakness
// include TMB/ALCT configuration and state machine prints+checks to hyperDAQ
//
// Revision 3.6  2006/11/09 08:47:51  rakness
// add rpc0_raw_delay to xml file
//
// Revision 3.5  2006/10/12 15:56:01  rakness
// cleaned up configuration checking for ALCT/TMB
//
// Revision 3.4  2006/10/06 12:15:39  rakness
// expand xml file
//
// Revision 3.3  2006/09/24 13:34:37  rakness
// decode configuration registers
//
// Revision 3.2  2006/09/07 14:55:38  rakness
// mask out read-only bits in userproms
//
// Revision 3.1  2006/09/05 10:13:17  rakness
// ALCT configure from prom
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.4  2006/02/02 14:27:31  mey
// Update
//
// Revision 2.3  2006/01/23 14:21:59  mey
// Update
//
// Revision 2.2  2006/01/23 13:56:52  mey
// Update using Greg's new code
//
// Revision 2.1  2005/09/06 12:12:28  mey
// Added register
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//


namespace emu {
  namespace pc {

//-----------------------------------------------------------------------
//////////////////////////////////////////////
// TMB VME register names:
//////////////////////////////////////////////
static const unsigned long int tmb_adr_boot             = 0x700000; // Hardware Bootstrap Register
//
static const unsigned long int vme_idreg0_adr           = 0x000000;
static const unsigned long int vme_idreg1_adr           = 0x000002;
static const unsigned long int vme_idreg2_adr           = 0x000004;
static const unsigned long int vme_idreg3_adr           = 0x000006;
static const unsigned long int vme_status_adr           = 0x000008;
static const unsigned long int vme_adr0_adr             = 0x00000A;
static const unsigned long int vme_adr1_adr             = 0x00000C;
static const unsigned long int vme_loopbk_adr           = 0x00000E;
//
static const unsigned long int	vme_usr_jtag_adr        = 0x000010;
static const unsigned long int	vme_prom_adr	        = 0x000012;
static const unsigned long int	vme_dddsm_adr	        = 0x000014;
static const unsigned long int	vme_ddd0_adr	        = 0x000016;
static const unsigned long int	vme_ddd1_adr	        = 0x000018;
static const unsigned long int	vme_ddd2_adr	        = 0x00001A;
static const unsigned long int	vme_dddoe_adr	        = 0x00001C;
static const unsigned long int	vme_ratctrl_adr	        = 0x00001E;
//
static const unsigned long int	vme_step_adr	        = 0x000020;
static const unsigned long int	vme_led_adr	        = 0x000022;
static const unsigned long int	vme_adc_adr	        = 0x000024;
static const unsigned long int	vme_dsn_adr	        = 0x000026;
static const unsigned long int	mod_cfg_adr	        = 0x000028;	
static const unsigned long int	ccb_cfg_adr	        = 0x00002A;
static const unsigned long int	ccb_trig_adr	        = 0x00002C;
static const unsigned long int	ccb_stat_adr	        = 0x00002E;	
//
static const unsigned long int	alct_cfg_adr	        = 0x000030;
static const unsigned long int	alct_inj_adr	        = 0x000032;
static const unsigned long int	alct0_inj_adr	        = 0x000034;
static const unsigned long int	alct1_inj_adr	        = 0x000036;
static const unsigned long int	alct_stat_adr	        = 0x000038;
static const unsigned long int	alct_alct0_adr	        = 0x00003A;
static const unsigned long int	alct_alct1_adr	        = 0x00003C;
static const unsigned long int	alct_fifo_adr	        = 0x00003E;
//
static const unsigned long int	dmb_mon_adr	        = 0x000040;
static const unsigned long int	cfeb_inj_adr	        = 0x000042;
static const unsigned long int	cfeb_inj_adr_adr        = 0x000044;
static const unsigned long int	cfeb_inj_wdata_adr      = 0x000046;
static const unsigned long int	cfeb_inj_rdata_adr      = 0x000048;
static const unsigned long int	hcm001_adr	        = 0x00004A;
static const unsigned long int	hcm023_adr	        = 0x00004C;
static const unsigned long int	hcm045_adr	        = 0x00004E;
//
static const unsigned long int	hcm101_adr	        = 0x000050;
static const unsigned long int	hcm123_adr	        = 0x000052;
static const unsigned long int	hcm145_adr	        = 0x000054;
static const unsigned long int	hcm201_adr	        = 0x000056;
static const unsigned long int	hcm223_adr	        = 0x000058;
static const unsigned long int	hcm245_adr	        = 0x00005A;
static const unsigned long int	hcm301_adr	        = 0x00005C;
static const unsigned long int	hcm323_adr	        = 0x00005E;
//
static const unsigned long int	hcm345_adr	        = 0x000060;
static const unsigned long int	hcm401_adr	        = 0x000062;
static const unsigned long int	hcm423_adr	        = 0x000064;
static const unsigned long int	hcm445_adr	        = 0x000066;
static const unsigned long int	seq_trig_en_adr	        = 0x000068;
static const unsigned long int	seq_trig_dly0_adr       = 0x00006A;
static const unsigned long int	seq_trig_dly1_adr       = 0x00006C;
static const unsigned long int	seq_id_adr	        = 0x00006E;
//
static const unsigned long int	seq_clct_adr	        = 0x000070;
static const unsigned long int	seq_fifo_adr	        = 0x000072;
static const unsigned long int	seq_l1a_adr	        = 0x000074;
static const unsigned long int	seq_offset_adr	        = 0x000076;
static const unsigned long int	seq_clct0_adr	        = 0x000078;
static const unsigned long int	seq_clct1_adr	        = 0x00007A;
static const unsigned long int	seq_trig_src_adr        = 0x00007C;
static const unsigned long int	dmb_ram_adr	        = 0x00007E;
//
static const unsigned long int	dmb_wdata_adr	        = 0x000080;
static const unsigned long int	dmb_wdcnt_adr	        = 0x000082;
static const unsigned long int	dmb_rdata_adr	        = 0x000084;
static const unsigned long int	tmb_trig_adr	        = 0x000086;
static const unsigned long int	mpc0_frame0_adr	        = 0x000088;
static const unsigned long int	mpc0_frame1_adr	        = 0x00008A;
static const unsigned long int	mpc1_frame0_adr	        = 0x00008C;
static const unsigned long int	mpc1_frame1_adr	        = 0x00008E;
//
static const unsigned long int	mpc_inj_adr	        = 0x000090;
static const unsigned long int	mpc_ram_adr	        = 0x000092;
static const unsigned long int	mpc_ram_wdata_adr       = 0x000094;
static const unsigned long int	mpc_ram_rdata_adr       = 0x000096;
static const unsigned long int	scp_ctrl_adr	        = 0x000098;
static const unsigned long int	scp_rdata_adr	        = 0x00009A;
static const unsigned long int	ccb_cmd_adr	        = 0x00009C;
static const unsigned long int	buf_stat0_adr           = 0x00009E;
//
static const unsigned long int	buf_stat1_adr	        = 0x0000A0;
static const unsigned long int	buf_stat2_adr	        = 0x0000A2;
static const unsigned long int	buf_stat3_adr	        = 0x0000A4;
static const unsigned long int	buf_stat4_adr	        = 0x0000A6;
static const unsigned long int	alctfifo1_adr	        = 0x0000A8;
static const unsigned long int	alctfifo2_adr	        = 0x0000AA;
static const unsigned long int	seqmod_adr	        = 0x0000AC;
static const unsigned long int	seqsm_adr	        = 0x0000AE;
//
static const unsigned long int	seq_clctm_adr	        = 0x0000B0;
static const unsigned long int	tmbtim_adr	        = 0x0000B2;
static const unsigned long int  lhc_cycle_adr           = 0x0000B4;
static const unsigned long int  rpc_cfg_adr             = 0x0000B6;
static const unsigned long int  rpc_rdata_adr           = 0x0000B8;
static const unsigned long int  rpc_raw_delay_adr       = 0x0000BA;
static const unsigned long int  rpc_inj_adr             = 0x0000BC;
static const unsigned long int  adr_rpc_inj_adr         = 0x0000BE;
//
static const unsigned long int  rpc_inj_wdata_adr       = 0x0000C0;
static const unsigned long int  rpc_inj_rdata_adr       = 0x0000C2;
static const unsigned long int  rpc_tbins_adr           = 0x0000C4;
static const unsigned long int  rpc0_hcm_adr            = 0x0000C6;
static const unsigned long int  rpc1_hcm_adr            = 0x0000C8;
static const unsigned long int  bx0_delay_adr           = 0x0000CA;
static const unsigned long int  non_trig_readout_adr    = 0x0000CC;
static const unsigned long int  scp_trig_adr            = 0x0000CE;
//
static const unsigned long int  cnt_ctrl_adr            = 0x0000D0;
static const unsigned long int  cnt_rdata_adr           = 0x0000D2;
static const unsigned long int  jtag_sm_ctrl_adr        = 0x0000D4;
static const unsigned long int  jtag_sm_wdcnt_adr       = 0x0000D6;
static const unsigned long int  jtag_sm_cksum_adr       = 0x0000D8;
static const unsigned long int  vme_sm_ctrl_adr         = 0x0000DA;
static const unsigned long int  vme_sm_wdcnt_adr        = 0x0000DC;
static const unsigned long int  vme_sm_cksum_adr        = 0x0000DE;
//
static const unsigned long int  num_vme_sm_adr_adr      = 0x0000E0;
static const unsigned long int  vme_wrt_dat_ck_adr      = 0x0000E2;
static const unsigned long int  rat_3d_sm_ctrl_adr      = 0x0000E4;
static const unsigned long int  rat_3d_delays_adr       = 0x0000E6;
static const unsigned long int  uptime_adr              = 0x0000E8;
static const unsigned long int  tmb_stat_adr            = 0x0000EA;
static const unsigned long int  bxn_clct_adr            = 0x0000EC;
static const unsigned long int  bxn_alct_adr            = 0x0000EE;
//
static const unsigned long int  layer_trg_mode_adr      = 0x0000F0;
static const unsigned long int  ise_version_adr         = 0x0000F2;
static const unsigned long int  pattern_find_pretrg_adr = 0x0000F4;
static const unsigned long int  clct_separation_adr     = 0x0000F6;
static const unsigned long int  clct_separation_ram_adr = 0x0000F8;
//
static const int LARGEST_VME_ADDRESS = clct_separation_ram_adr;
//
// TMB counter indices:
const int ALCT_SENT_TO_TMB_COUNTER_INDEX  =  0;
const int CLCT_PRETRIGGER_COUNTER_INDEX   =  5;
const int LCT_SENT_TO_MPC_COUNTER_INDEX   = 35; 
const int L1A_IN_TMB_WINDOW_COUNTER_INDEX = 41; 
  //
//
/////////////////////////////////////////////////////////////////////////////////////
// Bit mappings for VME registers
//  - includes default values for configuration registers (write bits)
//  - includes expected values for registers which are checked 
/////////////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------
//0X70000 = ADR_BOOT:  Hardware Bootstrap Register
//-----------------------------------------------------------------
const int boot_tdi_vmereg                     =  tmb_adr_boot;
const int boot_tdi_bitlo                      =  0;
const int boot_tdi_bithi                      =  0;
//
const int boot_tms_vmereg                     =  tmb_adr_boot;
const int boot_tms_bitlo                      =  1;
const int boot_tms_bithi                      =  1;
//
const int boot_tck_vmereg                     =  tmb_adr_boot;
const int boot_tck_bitlo                      =  2;
const int boot_tck_bithi                      =  2;
//
const int boot_jtag_chain_select_vmereg       =  tmb_adr_boot;
const int boot_jtag_chain_select_bitlo        =  3;
const int boot_jtag_chain_select_bithi        =  6;
//
const int boot_control_jtag_chain_vmereg      =  tmb_adr_boot;
const int boot_control_jtag_chain_bitlo       =  7;
const int boot_control_jtag_chain_bithi       =  7;
const int boot_control_jtag_chain_expected    =  0;           //expect FPGA to control JTAG chain
//
const int boot_hard_reset_alct_vmereg         =  tmb_adr_boot;
const int boot_hard_reset_alct_bitlo          =  8;
const int boot_hard_reset_alct_bithi          =  8;
//
const int boot_hard_reset_tmb_vmereg          =  tmb_adr_boot;
const int boot_hard_reset_tmb_bitlo           =  9;
const int boot_hard_reset_tmb_bithi           =  9;
//
const int boot_disable_hard_reset_alct_vmereg =  tmb_adr_boot;
const int boot_disable_hard_reset_alct_bitlo  = 10;
const int boot_disable_hard_reset_alct_bithi  = 10;
//
const int boot_disable_VME_vmereg             =  tmb_adr_boot;
const int boot_disable_VME_bitlo              = 11;
const int boot_disable_VME_bithi              = 11;
//
const int boot_disable_mezz_clock_vmereg      =  tmb_adr_boot;
const int boot_disable_mezz_clock_bitlo       = 12;
const int boot_disable_mezz_clock_bithi       = 12;
//
const int boot_hard_reset_rat_vmereg          =  tmb_adr_boot;
const int boot_hard_reset_rat_bitlo           = 13;
const int boot_hard_reset_rat_bithi           = 13;
//
const int boot_vme_ready_vmereg               =  tmb_adr_boot;
const int boot_vme_ready_bitlo                = 14;
const int boot_vme_ready_bithi                = 14;
//
const int boot_tdo_vmereg                     =  tmb_adr_boot;
const int boot_tdo_bitlo                      = 15;
const int boot_tdo_bithi                      = 15;
//
//
//-----------------------------------------------------------------
//0X0E = ADR_LOOPBK:  Loop-Back Control Register
//-----------------------------------------------------------------
const int cfeb_oe_vmereg         =  vme_loopbk_adr;
const int cfeb_oe_bitlo          =  0;
const int cfeb_oe_bithi          =  0;
//
const int alct_loop_vmereg       =  vme_loopbk_adr;
const int alct_loop_bitlo        =  1;
const int alct_loop_bithi        =  1;
//
const int enable_alct_rx_vmereg  =  vme_loopbk_adr;
const int enable_alct_rx_bitlo   =  2;
const int enable_alct_rx_bithi   =  2;
const int enable_alct_rx_default =  1;
//
const int enable_alct_tx_vmereg  =  vme_loopbk_adr;
const int enable_alct_tx_bitlo   =  3;
const int enable_alct_tx_bithi   =  3;
const int enable_alct_tx_default =  1;
//
const int rpc_loop_rat_vmereg    =  vme_loopbk_adr;
const int rpc_loop_rat_bitlo     =  4;
const int rpc_loop_rat_bithi     =  4;
//
const int rpc_loop_tmb_vmereg    =  vme_loopbk_adr;
const int rpc_loop_tmb_bitlo     =  5;
const int rpc_loop_tmb_bithi     =  5;
//
const int dmb_loop_vmereg        =  vme_loopbk_adr;
const int dmb_loop_bitlo         =  6;
const int dmb_loop_bithi         =  6;
//
const int dmb_oe_vmereg          =  vme_loopbk_adr;
const int dmb_oe_bitlo           =  7;
const int dmb_oe_bithi           =  7;
//
const int gtl_loop_vmereg        =  vme_loopbk_adr;
const int gtl_loop_bitlo         =  8;
const int gtl_loop_bithi         =  8;
//
const int gtl_oe_vmereg          =  vme_loopbk_adr;
const int gtl_oe_bitlo           =  9;
const int gtl_oe_bithi           =  9;
//
//
//------------------------------------------------------------------
//0X14 = ADR_DDDSM:  3D3444 State Machine Control + DCM Lock Status  
//------------------------------------------------------------------
const int ddd_state_machine_start_vmereg              =  vme_dddsm_adr;
const int ddd_state_machine_start_bitlo               =  0;
const int ddd_state_machine_start_bithi               =  0;
//
const int ddd_state_machine_manual_vmereg             =  vme_dddsm_adr;
const int ddd_state_machine_manual_bitlo              =  1;
const int ddd_state_machine_manual_bithi              =  1;
//
const int ddd_state_machine_latch_vmereg              =  vme_dddsm_adr;
const int ddd_state_machine_latch_bitlo               =  2;
const int ddd_state_machine_latch_bithi               =  2;
//
const int ddd_state_machine_serial_in_vmereg          =  vme_dddsm_adr;
const int ddd_state_machine_serial_in_bitlo           =  3;
const int ddd_state_machine_serial_in_bithi           =  3;
//
const int ddd_state_machine_serial_out_vmereg         =  vme_dddsm_adr;
const int ddd_state_machine_serial_out_bitlo          =  4;
const int ddd_state_machine_serial_out_bithi          =  4;
//
const int ddd_state_machine_autostart_vmereg          =  vme_dddsm_adr;
const int ddd_state_machine_autostart_bitlo           =  5;
const int ddd_state_machine_autostart_bithi           =  5;
//
const int ddd_state_machine_busy_vmereg               =  vme_dddsm_adr;
const int ddd_state_machine_busy_bitlo                =  6;
const int ddd_state_machine_busy_bithi                =  6;
const int ddd_state_machine_busy_expected             =  0;   //expect state machine not to be stuck busy
//
const int ddd_state_machine_verify_ok_vmereg          =  vme_dddsm_adr;
const int ddd_state_machine_verify_ok_bitlo           =  7;
const int ddd_state_machine_verify_ok_bithi           =  7;
const int ddd_state_machine_verify_ok_expected        =  1;   //expect data to verify OK
//
// expect all clocks to lock:
const int ddd_state_machine_clock0_lock_vmereg        =  vme_dddsm_adr;
const int ddd_state_machine_clock0_lock_bitlo         =  8;
const int ddd_state_machine_clock0_lock_bithi         =  8;
const int ddd_state_machine_clock0_lock_expected      =  1;   
//
const int ddd_state_machine_clock0d_lock_vmereg       =  vme_dddsm_adr;
const int ddd_state_machine_clock0d_lock_bitlo        =  9;
const int ddd_state_machine_clock0d_lock_bithi        =  9;
const int ddd_state_machine_clock0d_lock_expected     =  1 ;   
//
const int ddd_state_machine_clock1_lock_vmereg        =  vme_dddsm_adr;
const int ddd_state_machine_clock1_lock_bitlo         = 10;
const int ddd_state_machine_clock1_lock_bithi         = 10;
const int ddd_state_machine_clock1_lock_expected      =  0;  
//
const int ddd_state_machine_clock_alct_lock_vmereg    =  vme_dddsm_adr;
const int ddd_state_machine_clock_alct_lock_bitlo     = 11;
const int ddd_state_machine_clock_alct_lock_bithi     = 11;
const int ddd_state_machine_clock_alct_lock_expected  =  1;  
//
const int ddd_state_machine_clockd_alct_lock_vmereg   =  vme_dddsm_adr;
const int ddd_state_machine_clockd_alct_lock_bitlo    = 12;
const int ddd_state_machine_clockd_alct_lock_bithi    = 12;
const int ddd_state_machine_clockd_alct_lock_expected =  1;  
//
const int ddd_state_machine_clock_mpc_lock_vmereg     =  vme_dddsm_adr;
const int ddd_state_machine_clock_mpc_lock_bitlo      = 13;
const int ddd_state_machine_clock_mpc_lock_bithi      = 13;
const int ddd_state_machine_clock_mpc_lock_expected   =  0;  
//
const int ddd_state_machine_clock_dcc_lock_vmereg     =  vme_dddsm_adr;
const int ddd_state_machine_clock_dcc_lock_bitlo      = 14;
const int ddd_state_machine_clock_dcc_lock_bithi      = 14;
const int ddd_state_machine_clock_dcc_lock_expected   =  1;  
//
const int ddd_state_machine_clock_rpc_lock_vmereg     =  vme_dddsm_adr;
const int ddd_state_machine_clock_rpc_lock_bitlo      = 15;
const int ddd_state_machine_clock_rpc_lock_bithi      = 15;
// Expected value for RPC clock lock depends on rpc_exists_ for this TMB...
//
//
//------------------------------------------------------------------
//0X16 = ADR_DDD0:  3D3444 Chip 0 Delays, 1 step = 2ns
//------------------------------------------------------------------
const int alct_tx_clock_delay_vmereg  =  vme_ddd0_adr;
const int alct_tx_clock_delay_bitlo   =  0;
const int alct_tx_clock_delay_bithi   =  3;
const int alct_tx_clock_delay_default =  5;               //TMB documentation says this should be 8
//
const int alct_rx_clock_delay_vmereg  =  vme_ddd0_adr;
const int alct_rx_clock_delay_bitlo   =  4;
const int alct_rx_clock_delay_bithi   =  7;
const int alct_rx_clock_delay_default =  1;
//
const int dmb_tx_delay_vmereg         =  vme_ddd0_adr;
const int dmb_tx_delay_bitlo          =  8;
const int dmb_tx_delay_bithi          = 11;
const int dmb_tx_delay_default        =  2;
//
const int rat_tmb_delay_vmereg        =  vme_ddd0_adr;
const int rat_tmb_delay_bitlo         = 12;
const int rat_tmb_delay_bithi         = 15;
const int rat_tmb_delay_default       =  9;               //TMB documentation says this should be 0
//
//
//------------------------------------------------------------------
//0X18 = ADR_DDD1:  3D3444 Chip 1 Delays, 1 step = 2ns
//------------------------------------------------------------------
const int tmb1_phase_vmereg  =  vme_ddd1_adr;
const int tmb1_phase_bitlo   =  0;
const int tmb1_phase_bithi   =  3;
const int tmb1_phase_default =  0;
//
const int mpc_phase_vmereg   =  vme_ddd1_adr;
const int mpc_phase_bitlo    =  4;
const int mpc_phase_bithi    =  7;
const int mpc_phase_default  =  0;
//
const int dcc_phase_vmereg   =  vme_ddd1_adr;
const int dcc_phase_bitlo    =  8;
const int dcc_phase_bithi    = 11;
const int dcc_phase_default  =  0;
//
const int cfeb0delay_vmereg  =  vme_ddd1_adr;
const int cfeb0delay_bitlo   = 12;
const int cfeb0delay_bithi   = 15;
const int cfeb0delay_default =  7;
//
//
//------------------------------------------------------------------
//0X1A = ADR_DDD2:  3D3444 Chip 2 Delays, 1 step = 2ns
//------------------------------------------------------------------
const int cfeb1delay_vmereg  =  vme_ddd2_adr;
const int cfeb1delay_bitlo   =  0;
const int cfeb1delay_bithi   =  3;
const int cfeb1delay_default =  7;
//
const int cfeb2delay_vmereg  =  vme_ddd2_adr;
const int cfeb2delay_bitlo   =  4;
const int cfeb2delay_bithi   =  7;
const int cfeb2delay_default =  7;
//
const int cfeb3delay_vmereg  =  vme_ddd2_adr;
const int cfeb3delay_bitlo   =  8;
const int cfeb3delay_bithi   = 11;
const int cfeb3delay_default =  7;
//
const int cfeb4delay_vmereg  =  vme_ddd2_adr;
const int cfeb4delay_bitlo   = 12;
const int cfeb4delay_bithi   = 15;
const int cfeb4delay_default =  7;
//
//
//------------------------------------------------------------------
//0X1E = ADR_RATCTRL:  RAT Module Control
//------------------------------------------------------------------
const int rpc_sync_vmereg    =  vme_ratctrl_adr;
const int rpc_sync_bitlo     =  0;
const int rpc_sync_bithi     =  0;
const int rpc_sync_default   =  0;
//
const int shift_rpc_vmereg   =  vme_ratctrl_adr;
const int shift_rpc_bitlo    =  1;
const int shift_rpc_bithi    =  1;
const int shift_rpc_default  =  0;
//
const int rat_dsn_en_vmereg  =  vme_ratctrl_adr;
const int rat_dsn_en_bitlo   =  4;
const int rat_dsn_en_bithi   =  4;
const int rat_dsn_en_default =  0;
//
//
//------------------------------------------------------------------
//0X2C = ADR_CCB_TRIG:  CCB Trigger Control
//------------------------------------------------------------------
const int alct_ext_trig_l1aen_vmereg     =  ccb_trig_adr;
const int alct_ext_trig_l1aen_bitlo      =  0;
const int alct_ext_trig_l1aen_bithi      =  0;
const int alct_ext_trig_l1aen_default    =  0;
//
const int clct_ext_trig_l1aen_vmereg     =  ccb_trig_adr;
const int clct_ext_trig_l1aen_bitlo      =  1;
const int clct_ext_trig_l1aen_bithi      =  1;
const int clct_ext_trig_l1aen_default    =  0;
//
const int request_l1a_vmereg             =  ccb_trig_adr;
const int request_l1a_bitlo              =  2;
const int request_l1a_bithi              =  2;
const int request_l1a_default            =  1;             //N.B. this should be set to 0 for running
//
const int alct_ext_trig_vme_vmereg       =  ccb_trig_adr;
const int alct_ext_trig_vme_bitlo        =  3;
const int alct_ext_trig_vme_bithi        =  3;
const int alct_ext_trig_vme_default      =  0;
//
const int clct_ext_trig_vme_vmereg       =  ccb_trig_adr;
const int clct_ext_trig_vme_bitlo        =  4;
const int clct_ext_trig_vme_bithi        =  4;
const int clct_ext_trig_vme_default      =  0;
//
const int ext_trig_both_vmereg           =  ccb_trig_adr;
const int ext_trig_both_bitlo            =  5;
const int ext_trig_both_bithi            =  5;
const int ext_trig_both_default          =  0;
//
const int ccb_allow_bypass_vmereg        =  ccb_trig_adr;
const int ccb_allow_bypass_bitlo         =  6;
const int ccb_allow_bypass_bithi         =  6;
const int ccb_allow_bypass_default       =  0;
//
const int ignore_ccb_startstop_vmereg    =  ccb_trig_adr;
const int ignore_ccb_startstop_bitlo     =  7;
const int ignore_ccb_startstop_bithi     =  7;
const int ignore_ccb_startstop_default   =  0;
//
const int internal_l1a_delay_vme_vmereg  =  ccb_trig_adr;
const int internal_l1a_delay_vme_bitlo   =  8;
const int internal_l1a_delay_vme_bithi   = 15;
const int internal_l1a_delay_vme_default =0x72;
//
//
// greg, this needs address 2E added for monitors
//------------------------------------------------------------------
//0X30 = ADR_ALCT_CFG:  ALCT Configuration
//------------------------------------------------------------------
const int cfg_alct_ext_trig_en_vmereg    =  alct_cfg_adr;
const int cfg_alct_ext_trig_en_bitlo     =  0;
const int cfg_alct_ext_trig_en_bithi     =  0;
const int cfg_alct_ext_trig_en_default   =  0;         //TMB documentation says this should be 1            
//
const int cfg_alct_ext_inject_en_vmereg  =  alct_cfg_adr;
const int cfg_alct_ext_inject_en_bitlo   =  1;
const int cfg_alct_ext_inject_en_bithi   =  1;
const int cfg_alct_ext_inject_en_default =  0;            
//
const int cfg_alct_ext_trig_vmereg       =  alct_cfg_adr;
const int cfg_alct_ext_trig_bitlo        =  2;
const int cfg_alct_ext_trig_bithi        =  2;
const int cfg_alct_ext_trig_default      =  0;            
//
const int cfg_alct_ext_inject_vmereg     =  alct_cfg_adr;
const int cfg_alct_ext_inject_bitlo      =  3;
const int cfg_alct_ext_inject_bithi      =  3;
const int cfg_alct_ext_inject_default    =  0;            
//
const int alct_seq_cmd_vmereg            =  alct_cfg_adr;
const int alct_seq_cmd_bitlo             =  4;
const int alct_seq_cmd_bithi             =  6;
const int alct_seq_cmd_default           =  0;            
//
const int alct_clock_en_use_ccb_vmereg   =  alct_cfg_adr;
const int alct_clock_en_use_ccb_bitlo    = 12;
const int alct_clock_en_use_ccb_bithi    = 12;
const int alct_clock_en_use_ccb_default  =  1;            
//
const int alct_clock_en_use_vme_vmereg   =  alct_cfg_adr;
const int alct_clock_en_use_vme_bitlo    = 13;
const int alct_clock_en_use_vme_bithi    = 13;
const int alct_clock_en_use_vme_default  =  0;            
//
//
//------------------------------------------------------------------
//0X32 = ADR_ALCT_INJ:  ALCT Injector Control
//------------------------------------------------------------------
const int alct_clear_vmereg       =  alct_inj_adr;
const int alct_clear_bitlo        =  0;
const int alct_clear_bithi        =  0;
const int alct_clear_default      =  0;
//
const int alct_inject_mux_vmereg  =  alct_inj_adr;
const int alct_inject_mux_bitlo   =  1;
const int alct_inject_mux_bithi   =  1;
const int alct_inject_mux_default =  0;
//
const int alct_sync_clct_vmereg   =  alct_inj_adr;
const int alct_sync_clct_bitlo    =  2;
const int alct_sync_clct_bithi    =  2;
const int alct_sync_clct_default  =  0;
//
const int alct_inj_delay_vmereg   =  alct_inj_adr;
const int alct_inj_delay_bitlo    =  3;
const int alct_inj_delay_bithi    =  7;
const int alct_inj_delay_default  =0xd;                  //TMB documentation says this should be 0x8
//
//
// greg, need to add address 0x38 for 08/12/2008 firmware version 
//------------------------------------------------------------------
//0X3A = ADR_ALCT0_RCD:  ALCT 1st Muon received by TMB
//------------------------------------------------------------------
const int alct0_valid_vmereg   =  alct_alct0_adr;
const int alct0_valid_bitlo    =  0;
const int alct0_valid_bithi    =  0;
//
const int alct0_quality_vmereg =  alct_alct0_adr;
const int alct0_quality_bitlo  =  1;
const int alct0_quality_bithi  =  2;
//
const int alct0_amu_vmereg     =  alct_alct0_adr;
const int alct0_amu_bitlo      =  3;
const int alct0_amu_bithi      =  3;
//
const int alct0_key_wg_vmereg  =  alct_alct0_adr;
const int alct0_key_wg_bitlo   =  4;
const int alct0_key_wg_bithi   = 10;
//
const int alct0_bxn_vmereg     =  alct_alct0_adr;
const int alct0_bxn_bitlo      = 11;
const int alct0_bxn_bithi      = 12;
//
//
//------------------------------------------------------------------
//0X3C = ADR_ALCT1_RCD:  ALCT 2nd Muon received by TMB
//------------------------------------------------------------------
const int alct1_valid_vmereg   =  alct_alct1_adr;
const int alct1_valid_bitlo    =  0;
const int alct1_valid_bithi    =  0;
//
const int alct1_quality_vmereg =  alct_alct1_adr;
const int alct1_quality_bitlo  =  1;
const int alct1_quality_bithi  =  2;
//
const int alct1_amu_vmereg     =  alct_alct1_adr;
const int alct1_amu_bitlo      =  3;
const int alct1_amu_bithi      =  3;
//
const int alct1_key_wg_vmereg  =  alct_alct1_adr;
const int alct1_key_wg_bitlo   =  4;
const int alct1_key_wg_bithi   = 10;
//
const int alct1_bxn_vmereg     =  alct_alct1_adr;
const int alct1_bxn_bitlo      = 11;
const int alct1_bxn_bithi      = 12;
//
//
//------------------------------------------------------------------
//0X3E = ADR_ALCT_FIFO:  ALCT FIFO RAM Status
//------------------------------------------------------------------
const int alct_raw_busy_vmereg       =  alct_fifo_adr;
const int alct_raw_busy_bitlo        =  0;
const int alct_raw_busy_bithi        =  0;
//
const int alct_raw_done_vmereg       =  alct_fifo_adr;
const int alct_raw_done_bitlo        =  1;
const int alct_raw_done_bithi        =  1;
//
const int alct_raw_word_count_vmereg =  alct_fifo_adr;
const int alct_raw_word_count_bitlo  =  2;
const int alct_raw_word_count_bithi  = 12;
//
const int alct_raw_msbs_vmereg       =  alct_fifo_adr;
const int alct_raw_msbs_bitlo        = 13;
const int alct_raw_msbs_bithi        = 14;
//
//
//------------------------------------------------------------------
//0X42 = ADR_CFEB_INJ:  CFEB Injector Control
//------------------------------------------------------------------
const int enableCLCTInputs_vmereg   =  cfeb_inj_adr;
const int enableCLCTInputs_bitlo    =  0;
const int enableCLCTInputs_bithi    =  4;
const int enableCLCTInputs_default  =0x1f;
//
const int cfeb_ram_sel_vmereg       =  cfeb_inj_adr;
const int cfeb_ram_sel_bitlo        =  5;
const int cfeb_ram_sel_bithi        =  9;
const int cfeb_ram_sel_default      =  0;
//
const int cfeb_inj_en_sel_vmereg    =  cfeb_inj_adr;
const int cfeb_inj_en_sel_bitlo     = 10;
const int cfeb_inj_en_sel_bithi     = 14;
const int cfeb_inj_en_sel_default   =0x1f;
//
const int start_pattern_inj_vmereg  =  cfeb_inj_adr;
const int start_pattern_inj_bitlo   = 15;
const int start_pattern_inj_bithi   = 15;
const int start_pattern_inj_default =  0;
//
//
//------------------------------------------------------------------
//0X4A,4C,4E = ADR_HCM001,HCM023,HCM045 = CFEB0 Hot Channel Masks
//0X50,52,54 = ADR_HCM101,HCM123,HCM145 = CFEB1 Hot Channel Masks
//0X56,58,5A = ADR_HCM201,HCM223,HCM245 = CFEB2 Hot Channel Masks
//0X5C,5E,60 = ADR_HCM301,HCM323,HCM345 = CFEB3 Hot Channel Masks
//0X62,64,66 = ADR_HCM401,HCM423,HCM445 = CFEB4 Hot Channel Masks
//------------------------------------------------------------------
// bit-map for layers and distrips covered by each of the registers
const int cfeb0_layer01_hotchannelmask_vmereg          = hcm001_adr;
const int cfeb0_layer01_hotchannelmask_layer_map[16]   = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1};
const int cfeb0_layer01_hotchannelmask_distrip_map[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7};
//
const int cfeb0_layer23_hotchannelmask_vmereg          = hcm023_adr;
const int cfeb0_layer23_hotchannelmask_layer_map[16]   = { 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3};
const int cfeb0_layer23_hotchannelmask_distrip_map[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7};
//
const int cfeb0_layer45_hotchannelmask_vmereg          = hcm045_adr;
const int cfeb0_layer45_hotchannelmask_layer_map[16]   = { 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5};
const int cfeb0_layer45_hotchannelmask_distrip_map[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7};
//
//
const int cfeb1_layer01_hotchannelmask_vmereg          = hcm101_adr;
const int cfeb1_layer01_hotchannelmask_layer_map[16]   = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1};
const int cfeb1_layer01_hotchannelmask_distrip_map[16] = { 8, 9,10,11,12,13,14,15, 8, 9,10,11,12,13,14,15};
//
const int cfeb1_layer23_hotchannelmask_vmereg          = hcm123_adr;
const int cfeb1_layer23_hotchannelmask_layer_map[16]   = { 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3};
const int cfeb1_layer23_hotchannelmask_distrip_map[16] = { 8, 9,10,11,12,13,14,15, 8, 9,10,11,12,13,14,15};
//
const int cfeb1_layer45_hotchannelmask_vmereg          = hcm145_adr;
const int cfeb1_layer45_hotchannelmask_layer_map[16]   = { 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5};
const int cfeb1_layer45_hotchannelmask_distrip_map[16] = { 8, 9,10,11,12,13,14,15, 8, 9,10,11,12,13,14,15};
//
//
const int cfeb2_layer01_hotchannelmask_vmereg          = hcm201_adr;
const int cfeb2_layer01_hotchannelmask_layer_map[16]   = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1};
const int cfeb2_layer01_hotchannelmask_distrip_map[16] = {16,17,18,19,20,21,22,23,16,17,18,19,20,21,22,23};
//
const int cfeb2_layer23_hotchannelmask_vmereg          = hcm223_adr;
const int cfeb2_layer23_hotchannelmask_layer_map[16]   = { 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3};
const int cfeb2_layer23_hotchannelmask_distrip_map[16] = {16,17,18,19,20,21,22,23,16,17,18,19,20,21,22,23};
//
const int cfeb2_layer45_hotchannelmask_vmereg          = hcm245_adr;
const int cfeb2_layer45_hotchannelmask_layer_map[16]   = { 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5};
const int cfeb2_layer45_hotchannelmask_distrip_map[16] = {16,17,18,19,20,21,22,23,16,17,18,19,20,21,22,23};
//
//
const int cfeb3_layer01_hotchannelmask_vmereg          = hcm301_adr;
const int cfeb3_layer01_hotchannelmask_layer_map[16]   = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1};
const int cfeb3_layer01_hotchannelmask_distrip_map[16] = {24,25,26,27,28,29,30,31,24,25,26,27,28,29,30,31};
//
const int cfeb3_layer23_hotchannelmask_vmereg          = hcm323_adr;
const int cfeb3_layer23_hotchannelmask_layer_map[16]   = { 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3};
const int cfeb3_layer23_hotchannelmask_distrip_map[16] = {24,25,26,27,28,29,30,31,24,25,26,27,28,29,30,31};
//
const int cfeb3_layer45_hotchannelmask_vmereg          = hcm345_adr;
const int cfeb3_layer45_hotchannelmask_layer_map[16]   = { 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5};
const int cfeb3_layer45_hotchannelmask_distrip_map[16] = {24,25,26,27,28,29,30,31,24,25,26,27,28,29,30,31};
//
// 
const int cfeb4_layer01_hotchannelmask_vmereg          = hcm401_adr;
const int cfeb4_layer01_hotchannelmask_layer_map[16]   = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1};
const int cfeb4_layer01_hotchannelmask_distrip_map[16] = {32,33,34,35,36,37,38,39,32,33,34,35,36,37,38,39};
//
const int cfeb4_layer23_hotchannelmask_vmereg          = hcm423_adr;
const int cfeb4_layer23_hotchannelmask_layer_map[16]   = { 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3};
const int cfeb4_layer23_hotchannelmask_distrip_map[16] = {32,33,34,35,36,37,38,39,32,33,34,35,36,37,38,39};
//
const int cfeb4_layer45_hotchannelmask_vmereg          = hcm445_adr;
const int cfeb4_layer45_hotchannelmask_layer_map[16]   = { 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5};
const int cfeb4_layer45_hotchannelmask_distrip_map[16] = {32,33,34,35,36,37,38,39,32,33,34,35,36,37,38,39};
//
const int hot_channel_mask_default = ON; 
//
//
//------------------------------------------------------------------
//0X68 = ADR_SEQ_TRIG_EN:  Sequencer Trigger Source Enables
//N.B. See TMB documentation first before controlling CFEBs through this register...
//------------------------------------------------------------------
const int clct_pat_trig_en_vmereg    =  seq_trig_en_adr;
const int clct_pat_trig_en_bitlo     =  0;
const int clct_pat_trig_en_bithi     =  0;
const int clct_pat_trig_en_default   =  1;
//
const int alct_pat_trig_en_vmereg    =  seq_trig_en_adr;
const int alct_pat_trig_en_bitlo     =  1;
const int alct_pat_trig_en_bithi     =  1;
const int alct_pat_trig_en_default   =  0;
//
const int match_pat_trig_en_vmereg   =  seq_trig_en_adr;
const int match_pat_trig_en_bitlo    =  2;
const int match_pat_trig_en_bithi    =  2;
const int match_pat_trig_en_default  =  0;
//
const int adb_ext_trig_en_vmereg     =  seq_trig_en_adr;
const int adb_ext_trig_en_bitlo      =  3;
const int adb_ext_trig_en_bithi      =  3;
const int adb_ext_trig_en_default    =  0;
//
const int dmb_ext_trig_en_vmereg     =  seq_trig_en_adr;
const int dmb_ext_trig_en_bitlo      =  4;
const int dmb_ext_trig_en_bithi      =  4;
const int dmb_ext_trig_en_default    =  0;
//
const int clct_ext_trig_en_vmereg    =  seq_trig_en_adr;
const int clct_ext_trig_en_bitlo     =  5;
const int clct_ext_trig_en_bithi     =  5;
const int clct_ext_trig_en_default   =  0;
//
const int alct_ext_trig_en_vmereg    =  seq_trig_en_adr;
const int alct_ext_trig_en_bitlo     =  6;
const int alct_ext_trig_en_bithi     =  6;
const int alct_ext_trig_en_default   =  0;
//
const int vme_ext_trig_vmereg        =  seq_trig_en_adr;
const int vme_ext_trig_bitlo         =  7;
const int vme_ext_trig_bithi         =  7;
const int vme_ext_trig_default       =  0;
//
const int ext_trig_inject_vmereg     =  seq_trig_en_adr;
const int ext_trig_inject_bitlo      =  8;
const int ext_trig_inject_bithi      =  8;
const int ext_trig_inject_default    =  0;
//
const int all_cfeb_active_vmereg     =  seq_trig_en_adr;
const int all_cfeb_active_bitlo      =  9;
const int all_cfeb_active_bithi      =  9;
const int all_cfeb_active_default    =  0;
//
const int cfebs_enabled_vmereg       =  seq_trig_en_adr;
const int cfebs_enabled_bitlo        = 10;
const int cfebs_enabled_bithi        = 14;
const int cfebs_enabled_default      =0x1f;
//
const int cfeb_enable_source_vmereg  =  seq_trig_en_adr;
const int cfeb_enable_source_bitlo   = 15;
const int cfeb_enable_source_bithi   = 15;
const int cfeb_enable_source_default =  1;
//
//
//------------------------------------------------------------------
//0X6A = ADR_SEQ_TRIG_DLY0:  Sequencer Trigger Source Delays
//------------------------------------------------------------------
const int alct_pretrig_width_vmereg  =  seq_trig_dly0_adr;
const int alct_pretrig_width_bitlo   =  0;
const int alct_pretrig_width_bithi   =  3;
const int alct_pretrig_width_default =  3;
//
const int alct_pretrig_delay_vmereg  =  seq_trig_dly0_adr;
const int alct_pretrig_delay_bitlo   =  4;
const int alct_pretrig_delay_bithi   =  7;
const int alct_pretrig_delay_default =  0;
//
const int alct_pattern_delay_vmereg  =  seq_trig_dly0_adr;
const int alct_pattern_delay_bitlo   =  8;
const int alct_pattern_delay_bithi   = 11;
const int alct_pattern_delay_default =  0;
//
const int adb_ext_trig_delay_vmereg  =  seq_trig_dly0_adr;
const int adb_ext_trig_delay_bitlo   = 12;
const int adb_ext_trig_delay_bithi   = 15;
const int adb_ext_trig_delay_default =  1;
//
//
//------------------------------------------------------------------
//0X6C = ADR_SEQ_TRIG_DLY1:  Sequencer Trigger Source Delays
//------------------------------------------------------------------
const int dmb_ext_trig_delay_vmereg   =  seq_trig_dly1_adr;
const int dmb_ext_trig_delay_bitlo    =  0;
const int dmb_ext_trig_delay_bithi    =  3;
const int dmb_ext_trig_delay_default  =  1;
//
const int clct_ext_trig_delay_vmereg  =  seq_trig_dly1_adr;
const int clct_ext_trig_delay_bitlo   =  4;
const int clct_ext_trig_delay_bithi   =  7;
const int clct_ext_trig_delay_default =  7;
//
const int alct_ext_trig_delay_vmereg  =  seq_trig_dly1_adr;
const int alct_ext_trig_delay_bitlo   =  8;
const int alct_ext_trig_delay_bithi   = 11;
const int alct_ext_trig_delay_default =  7;
//
const int layer_trig_delay_vmereg     =  seq_trig_dly1_adr;
const int layer_trig_delay_bitlo      = 12;
const int layer_trig_delay_bithi      = 15;
const int layer_trig_delay_default    =  0;
//
//
//------------------------------------------------------------------
//0X6E = ADR_SEQ_ID:  Sequencer Board + CSC Ids
//------------------------------------------------------------------
const int tmb_slot_vmereg =  seq_id_adr;
const int tmb_slot_bitlo  =  0;
const int tmb_slot_bithi  =  4;
// TMB is instantiated with a slot number, which will be its default
//
const int csc_id_vmereg   =  seq_id_adr;
const int csc_id_bitlo    =  5;
const int csc_id_bithi    =  8;
const int csc_id_default  =  5;
//
const int run_id_vmereg   =  seq_id_adr;
const int run_id_bitlo    =  9;
const int run_id_bithi    = 12;
const int run_id_default  =  0;
//
//
//------------------------------------------------------------------
//0X70 = ADR_SEQ_CLCT:  Sequencer CLCT configuration
//------------------------------------------------------------------
const int triad_persist_vmereg      =  seq_clct_adr;
const int triad_persist_bitlo       =  0;
const int triad_persist_bithi       =  3;
const int triad_persist_default     =  6;
//
const int hit_thresh_vmereg         =  seq_clct_adr;
const int hit_thresh_bitlo          =  4;
const int hit_thresh_bithi          =  6;
const int hit_thresh_default        =  4;
//
const int aff_thresh_vmereg         =  seq_clct_adr;
const int aff_thresh_bitlo          =  7;
const int aff_thresh_bithi          =  9;
const int aff_thresh_default        =  4; 
//
const int min_hits_pattern_vmereg   =  seq_clct_adr;
const int min_hits_pattern_bitlo    = 10;
const int min_hits_pattern_bithi    = 12;
const int min_hits_pattern_default  =  4;
//
const int drift_delay_vmereg        =  seq_clct_adr;
const int drift_delay_bitlo         = 13;
const int drift_delay_bithi         = 14;
const int drift_delay_default       =  2;
//
const int pretrigger_halt_vmereg    =  seq_clct_adr;
const int pretrigger_halt_bitlo     = 15;
const int pretrigger_halt_bithi     = 15;
const int pretrigger_halt_default   =  0;
//
//
//------------------------------------------------------------------
//0X72 = ADR_SEQ_FIFO:  Sequencer FIFO configuration
//------------------------------------------------------------------
const int FIFOMODE_NoCfebRaw_FullHeader     = 0;
const int FIFOMODE_AllCfebRaw_FullHeader    = 1;
const int FIFOMODE_LocalCfebRaw_FullHeader  = 2;
const int FIFOMODE_NoCfebRaw_ShortHeader    = 3;
const int FIFOMODE_NoCfebRaw_NoHeader       = 4;
//
const int fifo_mode_vmereg         =  seq_fifo_adr;
const int fifo_mode_bitlo          =  0;
const int fifo_mode_bithi          =  2;
const int fifo_mode_default        =  FIFOMODE_AllCfebRaw_FullHeader;
//
const int fifo_tbins_vmereg        =  seq_fifo_adr;
const int fifo_tbins_bitlo         =  3;
const int fifo_tbins_bithi         =  7;
const int fifo_tbins_default       =  7;
//
const int fifo_pretrig_vmereg      =  seq_fifo_adr;
const int fifo_pretrig_bitlo       =  8;
const int fifo_pretrig_bithi       = 12;
const int fifo_pretrig_default     =  2;
//
const int fifo_no_raw_hits_vmereg  =  seq_fifo_adr;
const int fifo_no_raw_hits_bitlo   = 13;
const int fifo_no_raw_hits_bithi   = 13;
const int fifo_no_raw_hits_default =  0;
//
//
//------------------------------------------------------------------
//0X74 = ADR_SEQ_L1A:  Sequencer L1A configuration
//------------------------------------------------------------------
const int l1adelay_vmereg          =  seq_l1a_adr;
const int l1adelay_bitlo           =  0;
const int l1adelay_bithi           =  7;
const int l1adelay_default         =128;
//
const int l1a_window_size_vmereg   =  seq_l1a_adr;
const int l1a_window_size_bitlo    =  8;
const int l1a_window_size_bithi    = 11;
const int l1a_window_size_default  =  3;
//
const int tmb_l1a_internal_vmereg  =  seq_l1a_adr;
const int tmb_l1a_internal_bitlo   = 12;
const int tmb_l1a_internal_bithi   = 12;
const int tmb_l1a_internal_default =  0;
//
//
//------------------------------------------------------------------
//0X76 = ADR_SEQ_OFFSET:  Sequencer Counter Offsets
//------------------------------------------------------------------
const int l1a_offset_vmereg  =  seq_offset_adr;
const int l1a_offset_bitlo   =  0;
const int l1a_offset_bithi   =  3;
const int l1a_offset_default =  0;
//
const int bxn_offset_vmereg  =  seq_offset_adr;
const int bxn_offset_bitlo   =  4;
const int bxn_offset_bithi   = 15;
const int bxn_offset_default =  0;
//
//
//------------------------------------------------------------------
//0X78 = ADR_SEQ_CLCT0:  Sequencer Latched CLCT0
//------------------------------------------------------------------
const int CLCT0_valid_vmereg        =  seq_clct0_adr;
const int CLCT0_valid_bitlo         =  0;
const int CLCT0_valid_bithi         =  0;
//
const int CLCT0_nhit_vmereg         =  seq_clct0_adr;
const int CLCT0_nhit_bitlo          =  1;
const int CLCT0_nhit_bithi          =  3;
//
const int CLCT0_pattern_vmereg      =  seq_clct0_adr;
const int CLCT0_pattern_bitlo       =  4;
const int CLCT0_pattern_bithi       =  7;
//
const int CLCT0_keyHalfStrip_vmereg =  seq_clct0_adr;
const int CLCT0_keyHalfStrip_bitlo  =  8;
const int CLCT0_keyHalfStrip_bithi  = 15;
//
//
//------------------------------------------------------------------
//0X7A = ADR_SEQ_CLCT1:  Sequencer Latched CLCT1
//------------------------------------------------------------------
const int CLCT1_valid_vmereg        =  seq_clct1_adr;
const int CLCT1_valid_bitlo         =  0;
const int CLCT1_valid_bithi         =  0;
//
const int CLCT1_nhit_vmereg         =  seq_clct1_adr;
const int CLCT1_nhit_bitlo          =  1;
const int CLCT1_nhit_bithi          =  3;
//
const int CLCT1_pattern_vmereg      =  seq_clct1_adr;
const int CLCT1_pattern_bitlo       =  4;
const int CLCT1_pattern_bithi       =  7;
//
const int CLCT1_keyHalfStrip_vmereg =  seq_clct1_adr;
const int CLCT1_keyHalfStrip_bitlo  =  8;
const int CLCT1_keyHalfStrip_bithi  = 15;
//
//
//------------------------------------------------------------------
//0X86 = ADR_TMB_TRIG:  TMB Trigger configuration/MPC accept
//------------------------------------------------------------------
const int tmb_sync_err_enable_vmereg  =  tmb_trig_adr;
const int tmb_sync_err_enable_bitlo   =  0;
const int tmb_sync_err_enable_bithi   =  1;
const int tmb_sync_err_enable_default =  3;
//
const int tmb_allow_alct_vmereg       =  tmb_trig_adr;
const int tmb_allow_alct_bitlo        =  2;
const int tmb_allow_alct_bithi        =  2;
const int tmb_allow_alct_default      =  0;
//
const int tmb_allow_clct_vmereg       =  tmb_trig_adr;
const int tmb_allow_clct_bitlo        =  3;
const int tmb_allow_clct_bithi        =  3;
const int tmb_allow_clct_default      =  1;
//
const int tmb_allow_match_vmereg      =  tmb_trig_adr;
const int tmb_allow_match_bitlo       =  4;
const int tmb_allow_match_bithi       =  4;
const int tmb_allow_match_default     =  1;
//
const int mpc_rx_delay_vmereg         =  tmb_trig_adr;
const int mpc_rx_delay_bitlo          =  5;
const int mpc_rx_delay_bithi          =  8;
const int mpc_rx_delay_default        =  7;
//
const int mpc_accept_vmereg           =  tmb_trig_adr;
const int mpc_accept_bitlo            =  9;
const int mpc_accept_bithi            = 10;
//
const int mpc_reserved_vmereg         =  tmb_trig_adr;
const int mpc_reserved_bitlo          = 11;
const int mpc_reserved_bithi          = 12;
//
const int mpc_sel_ttc_bx0_vmereg      =  tmb_trig_adr;
const int mpc_sel_ttc_bx0_bitlo       = 13;
const int mpc_sel_ttc_bx0_bithi       = 13;
const int mpc_sel_ttc_bx0_default     =  1;
//
const int mpc_idle_blank_vmereg       =  tmb_trig_adr;
const int mpc_idle_blank_bitlo        = 14;
const int mpc_idle_blank_bithi        = 14;
const int mpc_idle_blank_default      =  0;
//
//
const int mpc_output_enable_vmereg    =  tmb_trig_adr;
const int mpc_output_enable_bitlo     = 15;
const int mpc_output_enable_bithi     = 15;
const int mpc_output_enable_default   =  1;
//
//
//------------------------------------------------------------------
//0X98 = ADR_SCP_CTRL:  Scope control
//------------------------------------------------------------------
const int scope_in_readout_vmereg    =  scp_ctrl_adr;
const int scope_in_readout_bitlo     =  3;
const int scope_in_readout_bithi     =  3;
const int scope_in_readout_default   =  0;
//
//
//------------------------------------------------------------------
//0XA8 = ADR_ALCTFIFO1:  ALCT Raw Hits RAM control
//------------------------------------------------------------------
const int alct_raw_reset_vmereg         =  alctfifo1_adr;
const int alct_raw_reset_bitlo          =  0;
const int alct_raw_reset_bithi          =  0;
const int alct_raw_reset_default        =  0;
//
const int alct_raw_read_address_vmereg  =  alctfifo1_adr;
const int alct_raw_read_address_bitlo   =  1;
const int alct_raw_read_address_bithi   = 11;
const int alct_raw_read_address_default =  0;
//
const int alct_raw_sync_vmereg          =  alctfifo1_adr;
const int alct_raw_sync_bitlo           = 12;
const int alct_raw_sync_bithi           = 12;
const int alct_raw_sync_default         =  0;
//
const int alct_demux_mode_vmereg        =  alctfifo1_adr;
const int alct_demux_mode_bitlo         = 13;
const int alct_demux_mode_bithi         = 13;
const int alct_demux_mode_default       =  0;
//
//
//------------------------------------------------------------------
//0XAA = ADR_ALCTFIFO2:  ALCT Raw Hits RAM data LSBs
//------------------------------------------------------------------
const int alct_raw_lsbs_vmereg =  alctfifo2_adr;
const int alct_raw_lsbs_bitlo  =  0;
const int alct_raw_lsbs_bithi  = 15;
//
//
//------------------------------------------------------------------
//0XAC = ADR_SEQMOD:  Sequencer Trigger Modifiers
// updated for 1 May 2008 firmware
//------------------------------------------------------------------
const int clct_flush_delay_vmereg              =  seqmod_adr;
const int clct_flush_delay_bitlo               =  0;
const int clct_flush_delay_bithi               =  3;
const int clct_flush_delay_default             =  1;
//
const int wr_buffer_autoclear_vmereg           =  seqmod_adr;
const int wr_buffer_autoclear_bitlo            =  4;
const int wr_buffer_autoclear_bithi            =  4;
const int wr_buffer_autoclear_default          =  1;
//
const int clct_write_continuous_enable_vmereg  =  seqmod_adr;
const int clct_write_continuous_enable_bitlo   =  5;
const int clct_write_continuous_enable_bithi   =  5;
const int clct_write_continuous_enable_default =  0;
//
const int wrt_buf_required_vmereg              =  seqmod_adr;
const int wrt_buf_required_bitlo               =  6;
const int wrt_buf_required_bithi               =  6;
const int wrt_buf_required_default             =  1;
//
const int valid_clct_required_vmereg           =  seqmod_adr;
const int valid_clct_required_bitlo            =  7;
const int valid_clct_required_bithi            =  7;
const int valid_clct_required_default          =  1;
//
const int l1a_allow_match_vmereg               =  seqmod_adr;
const int l1a_allow_match_bitlo                =  8;
const int l1a_allow_match_bithi                =  8;
const int l1a_allow_match_default              =  1;
//
const int l1a_allow_notmb_vmereg               =  seqmod_adr;
const int l1a_allow_notmb_bitlo                =  9;
const int l1a_allow_notmb_bithi                =  9;
const int l1a_allow_notmb_default              =  0;
//
const int l1a_allow_nol1a_vmereg               =  seqmod_adr;
const int l1a_allow_nol1a_bitlo                = 10;
const int l1a_allow_nol1a_bithi                = 10;
const int l1a_allow_nol1a_default              =  0;
//
const int l1a_allow_alct_only_vmereg           =  seqmod_adr;
const int l1a_allow_alct_only_bitlo            = 11;
const int l1a_allow_alct_only_bithi            = 11;
const int l1a_allow_alct_only_default          =  0;
//
const int scint_veto_clr_vmereg                =  seqmod_adr;
const int scint_veto_clr_bitlo                 = 12;
const int scint_veto_clr_bithi                 = 12;
const int scint_veto_clr_default               =  0;
//
const int scint_veto_vme_vmereg                =  seqmod_adr;
const int scint_veto_vme_bitlo                 = 13;
const int scint_veto_vme_bithi                 = 13;
//
//
//------------------------------------------------------------------
//0XAE = ADR_SEQSM:  Sequencer Machine State
//------------------------------------------------------------------
const int clct_state_machine_vmereg      =  seqsm_adr;
const int clct_state_machine_bitlo       =  0;
const int clct_state_machine_bithi       =  2;
//
const int readout_state_machine_vmereg   =  seqsm_adr;
const int readout_state_machine_bitlo    =  3;
const int readout_state_machine_bithi    =  7;
//
const int buffer_queue_full_vmereg       =  seqsm_adr;
const int buffer_queue_full_bitlo        =  8;
const int buffer_queue_full_bithi        =  8;
//
const int buffer_queue_empty_vmereg      =  seqsm_adr;
const int buffer_queue_empty_bitlo       =  9;
const int buffer_queue_empty_bithi       =  9;
//
const int buffer_queue_overflow_vmereg   =  seqsm_adr;
const int buffer_queue_overflow_bitlo    = 10;
const int buffer_queue_overflow_bithi    = 10;
//
const int buffer_queue_underflow_vmereg  =  seqsm_adr;
const int buffer_queue_underflow_bitlo   = 11;
const int buffer_queue_underflow_bithi   = 11;
//
//
//------------------------------------------------------------------
//0XB0 = ADR_SEQCLCTM:  Sequencer CLCT (Most Significant Bits)
//------------------------------------------------------------------
const int CLCT_BXN_vmereg      =  seq_clctm_adr;
const int CLCT_BXN_bitlo       =  0;
const int CLCT_BXN_bithi       =  1;
//
const int CLCT_sync_err_vmereg =  seq_clctm_adr;
const int CLCT_sync_err_bitlo  =  2;
const int CLCT_sync_err_bithi  =  2;
//
//
//------------------------------------------------------------------
//0XB2 = ADR_TMBTIM:  TMB Timing for ALCT*CLCT Coincidence
//------------------------------------------------------------------
const int alct_vpf_delay_vmereg          =  tmbtim_adr;
const int alct_vpf_delay_bitlo           =  0;
const int alct_vpf_delay_bithi           =  3;
const int alct_vpf_delay_default         =  4;
//
const int alct_match_window_size_vmereg  =  tmbtim_adr;
const int alct_match_window_size_bitlo   =  4;
const int alct_match_window_size_bithi   =  7;
const int alct_match_window_size_default =  3;
//
const int mpc_tx_delay_vmereg            =  tmbtim_adr;
const int mpc_tx_delay_bitlo             =  8;
const int mpc_tx_delay_bithi             = 11;
const int mpc_tx_delay_default           =  0;
//
//
//------------------------------------------------------------------
//0XB6 = ADR_RPC_CFG:  RPC Configuration
//------------------------------------------------------------------
const int rpc_exists_vmereg       =  rpc_cfg_adr;
const int rpc_exists_bitlo        =  0;
const int rpc_exists_bithi        =  1;
const int rpc_exists_default      =  0x3;
//
const int rpc_read_enable_vmereg  =  rpc_cfg_adr;
const int rpc_read_enable_bitlo   =  4;
const int rpc_read_enable_bithi   =  4;
const int rpc_read_enable_default =  1;
//
const int rpc_bxn_offset_vmereg   =  rpc_cfg_adr;
const int rpc_bxn_offset_bitlo    =  5;
const int rpc_bxn_offset_bithi    =  8;
const int rpc_bxn_offset_default  =  0;
//
const int rpc_bank_vmereg         =  rpc_cfg_adr;
const int rpc_bank_bitlo          =  9;
const int rpc_bank_bithi          = 10;
const int rpc_bank_default        =  0;
//
const int rpc_rbxn_vmereg         =  rpc_cfg_adr;
const int rpc_rbxn_bitlo          = 11;
const int rpc_rbxn_bithi          = 13;
//
const int rpc_done_vmereg         =  rpc_cfg_adr;
const int rpc_done_bitlo          = 14;
const int rpc_done_bithi          = 14;
//
//
//------------------------------------------------------------------
//0XBA = ADR_RPC_RAW_DELAY:  RPC Raw Hits Data Delay
//------------------------------------------------------------------
const int rpc0_raw_delay_vmereg  =  rpc_raw_delay_adr;
const int rpc0_raw_delay_bitlo   =  0;
const int rpc0_raw_delay_bithi   =  3;
const int rpc0_raw_delay_default =  0;
//
const int rpc1_raw_delay_vmereg  =  rpc_raw_delay_adr;
const int rpc1_raw_delay_bitlo   =  4;
const int rpc1_raw_delay_bithi   =  7;
const int rpc1_raw_delay_default =  0;
//
const int rpc0_bxn_diff_vmereg   =  rpc_raw_delay_adr;
const int rpc0_bxn_diff_bitlo    =  8;
const int rpc0_bxn_diff_bithi    = 11;
//
const int rpc1_bxn_diff_vmereg   =  rpc_raw_delay_adr;
const int rpc1_bxn_diff_bitlo    = 12;
const int rpc1_bxn_diff_bithi    = 15;
//
//
//------------------------------------------------------------------
//0XBC = ADR_RPC_INJ:  RPC Injector Control
//------------------------------------------------------------------
const int rpc_mask_all_vmereg   =  rpc_inj_adr;
const int rpc_mask_all_bitlo    =  0;
const int rpc_mask_all_bithi    =  0;
const int rpc_mask_all_default  =  1;
//
const int inj_mask_rat_vmereg   =  rpc_inj_adr;
const int inj_mask_rat_bitlo    =  1;
const int inj_mask_rat_bithi    =  1;
const int inj_mask_rat_default  =  0;
//
const int inj_mask_rpc_vmereg   =  rpc_inj_adr;
const int inj_mask_rpc_bitlo    =  2;
const int inj_mask_rpc_bithi    =  2;
const int inj_mask_rpc_default  =  1;
//
const int inj_delay_rat_vmereg  =  rpc_inj_adr;
const int inj_delay_rat_bitlo   =  3;
const int inj_delay_rat_bithi   =  6;
const int inj_delay_rat_default =  7;              //TMB documentation says this should be 0
//
const int rpc_inj_sel_vmereg    =  rpc_inj_adr;
const int rpc_inj_sel_bitlo     =  7;
const int rpc_inj_sel_bithi     =  7;
const int rpc_inj_sel_default   =  0;
//
const int rpc_inj_wdata_vmereg  =  rpc_inj_adr;
const int rpc_inj_wdata_bitlo   =  8;
const int rpc_inj_wdata_bithi   = 10;
const int rpc_inj_wdata_default =  0;
//
const int rpc_inj_rdata_vmereg  =  rpc_inj_adr;
const int rpc_inj_rdata_bitlo   = 11;
const int rpc_inj_rdata_bithi   = 13;
//
//
//------------------------------------------------------------------
//0XC4 = ADR_RPC_TBINS:  RPC FIFO Time Bins
//------------------------------------------------------------------
const int fifo_tbins_rpc_vmereg    = rpc_tbins_adr;
const int fifo_tbins_rpc_bitlo     =  0;
const int fifo_tbins_rpc_bithi     =  4;
const int fifo_tbins_rpc_default   =  7;
//
const int fifo_pretrig_rpc_vmereg  = rpc_tbins_adr;
const int fifo_pretrig_rpc_bitlo   =  5;
const int fifo_pretrig_rpc_bithi   =  9;
const int fifo_pretrig_rpc_default =  2;
//
const int rpc_decouple_vmereg      = rpc_tbins_adr;
const int rpc_decouple_bitlo       = 10;
const int rpc_decouple_bithi       = 10;
const int rpc_decouple_default     =  0;
//
//
//------------------------------------------------------------------
//0XCA = ADR_BX0_DELAY:  BX0 to MPC delays
//------------------------------------------------------------------
const int alct_bx0_delay_vmereg   = bx0_delay_adr ;
const int alct_bx0_delay_bitlo    =  0;
const int alct_bx0_delay_bithi    =  3;
const int alct_bx0_delay_default  =  0;
//
const int clct_bx0_delay_vmereg   = bx0_delay_adr ;
const int clct_bx0_delay_bitlo    =  4;
const int clct_bx0_delay_bithi    =  7;
const int clct_bx0_delay_default  =  0;
//
const int alct_bx0_enable_vmereg  = bx0_delay_adr;
const int alct_bx0_enable_bitlo   =  8;
const int alct_bx0_enable_bithi   =  8;
const int alct_bx0_enable_default =  1;
//
//
//-----------------------------------------------------------------------------
//0XCC = ADR_NON_TRIG_RO:  Non-Triggering Event Enables + ME1/1A(1B) reversal 
//-----------------------------------------------------------------------------
const int tmb_allow_alct_nontrig_readout_vmereg    = non_trig_readout_adr ;
const int tmb_allow_alct_nontrig_readout_bitlo     =  0;
const int tmb_allow_alct_nontrig_readout_bithi     =  0;
const int tmb_allow_alct_nontrig_readout_default   =  0;
//
const int tmb_allow_clct_nontrig_readout_vmereg    = non_trig_readout_adr ;
const int tmb_allow_clct_nontrig_readout_bitlo     =  1;
const int tmb_allow_clct_nontrig_readout_bithi     =  1;
const int tmb_allow_clct_nontrig_readout_default   =  0;
//
const int tmb_allow_match_nontrig_readout_vmereg   = non_trig_readout_adr ;
const int tmb_allow_match_nontrig_readout_bitlo    =  2;
const int tmb_allow_match_nontrig_readout_bithi    =  2;
const int tmb_allow_match_nontrig_readout_default  =  1;
//
const int mpc_block_me1a_vmereg                    = non_trig_readout_adr ;
const int mpc_block_me1a_bitlo                     =  3;
const int mpc_block_me1a_bithi                     =  3;
const int mpc_block_me1a_default                   =  1;
//
const int clct_pretrigger_counter_non_me11_vmereg  = non_trig_readout_adr ;
const int clct_pretrigger_counter_non_me11_bitlo   =  4;
const int clct_pretrigger_counter_non_me11_bithi   =  4;
const int clct_pretrigger_counter_non_me11_default =  1;
//
const int csc_me11_vmereg                          = non_trig_readout_adr ;
const int csc_me11_bitlo                           =  5;
const int csc_me11_bithi                           =  5;
//
const int clct_stagger_vmereg                      = non_trig_readout_adr;
const int clct_stagger_bitlo                       =  6;
const int clct_stagger_bithi                       =  6;
//
const int reverse_stagger_vmereg                   = non_trig_readout_adr ;
const int reverse_stagger_bitlo                    =  7;
const int reverse_stagger_bithi                    =  7;
//
const int reverse_me1a_vmereg                      = non_trig_readout_adr ;
const int reverse_me1a_bitlo                       =  8;
const int reverse_me1a_bithi                       =  8;
//
const int reverse_me1b_vmereg                      = non_trig_readout_adr ;
const int reverse_me1b_bitlo                       =  9;
const int reverse_me1b_bithi                       =  9;
//
// Although these are read-only bits, we set it in the xml file to define what TMB firmware type to expect
const int tmb_firmware_compile_type_vmereg         = non_trig_readout_adr ;
const int tmb_firmware_compile_type_bitlo          = 12;
const int tmb_firmware_compile_type_bithi          = 15;
const int tmb_firmware_compile_type_default        = 0xa;
//
// These are the bits in register CC which are readout according to the following firmware compile types:
const int TMB_FIRMWARE_TYPE_A = 0xa;
const int TMB_FIRMWARE_TYPE_B = 0xb;
const int TMB_FIRMWARE_TYPE_C = 0xc;
const int TMB_FIRMWARE_TYPE_D = 0xd;
//
//
//------------------------------------------------------------------
//0XD4 = ADR_JTAGSM0:  JTAG State Machine Control (reads JTAG PROM)
//------------------------------------------------------------------
const int jtag_state_machine_start_vmereg         =  jtag_sm_ctrl_adr;
const int jtag_state_machine_start_bitlo          =  0;
const int jtag_state_machine_start_bithi          =  0;
//
const int jtag_state_machine_sreset_vmereg        =  jtag_sm_ctrl_adr;
const int jtag_state_machine_sreset_bitlo         =  1;
const int jtag_state_machine_sreset_bithi         =  1;
//
// greg, this needs to be changed to allow selection of ALCT userPROM format
const int jtag_state_machine_autostart_vmereg     =  jtag_sm_ctrl_adr;
const int jtag_state_machine_autostart_bitlo      =  2;
const int jtag_state_machine_autostart_bithi      =  2;
const int jtag_state_machine_autostart_expected   =  1;   //expect JTAG state machine to have auto-started
//
const int jtag_state_machine_busy_vmereg          =  jtag_sm_ctrl_adr;
const int jtag_state_machine_busy_bitlo           =  3;
const int jtag_state_machine_busy_bithi           =  3;
//
const int jtag_state_machine_aborted_vmereg       =  jtag_sm_ctrl_adr;
const int jtag_state_machine_aborted_bitlo        =  4;
const int jtag_state_machine_aborted_bithi        =  4;
const int jtag_state_machine_aborted_expected     =  0;   //expect JTAG state machine not to abort
//
const int jtag_state_machine_cksum_ok_vmereg      =  jtag_sm_ctrl_adr;
const int jtag_state_machine_cksum_ok_bitlo       =  5;
const int jtag_state_machine_cksum_ok_bithi       =  5;
const int jtag_state_machine_cksum_ok_expected    =  1;   //expect checksum OK
//
const int jtag_state_machine_wdcnt_ok_vmereg      =  jtag_sm_ctrl_adr;
const int jtag_state_machine_wdcnt_ok_bitlo       =  6;
const int jtag_state_machine_wdcnt_ok_bithi       =  6;
const int jtag_state_machine_wdcnt_ok_expected    =  1;   //expect wordcount OK
//
const int jtag_state_machine_tck_fpga_ok_vmereg   =  jtag_sm_ctrl_adr;
const int jtag_state_machine_tck_fpga_ok_bitlo    =  7;
const int jtag_state_machine_tck_fpga_ok_bithi    =  7;
const int jtag_state_machine_tck_fpga_ok_expected =  1;   //expect FPGA tck OK
//
const int jtag_state_machine_vme_ready_vmereg     =  jtag_sm_ctrl_adr; 
const int jtag_state_machine_vme_ready_bitlo      =  8; 
const int jtag_state_machine_vme_ready_bithi      =  8; 
const int jtag_state_machine_vme_ready_expected   =  1;   //expect VME ready
//
const int jtag_state_machine_ok_vmereg            =  jtag_sm_ctrl_adr;
const int jtag_state_machine_ok_bitlo             =  9;
const int jtag_state_machine_ok_bithi             =  9;
const int jtag_state_machine_ok_expected          =  1;   //expect JTAG state machine OK
//
const int jtag_state_machine_oe_vmereg            =  jtag_sm_ctrl_adr;
const int jtag_state_machine_oe_bitlo             = 10;
const int jtag_state_machine_oe_bithi             = 10;
//
const int jtag_disable_write_to_adr10_vmereg      =  jtag_sm_ctrl_adr;
const int jtag_disable_write_to_adr10_bitlo       = 11;
const int jtag_disable_write_to_adr10_bithi       = 11;
const int jtag_disable_write_to_adr10_default     =  0;
//
const int jtag_state_machine_throttle_vmereg      =  jtag_sm_ctrl_adr;
const int jtag_state_machine_throttle_bitlo       = 12;
const int jtag_state_machine_throttle_bithi       = 15;
//
//
//------------------------------------------------------------------
//0XD6 = ADR_JTAGSM1:  JTAG State Machine Word Count
//------------------------------------------------------------------
const int jtag_state_machine_word_count_vmereg =  jtag_sm_wdcnt_adr; 
const int jtag_state_machine_word_count_bitlo  =  0; 
const int jtag_state_machine_word_count_bithi  = 15; 
//
//
//------------------------------------------------------------------
//0XD8 = ADR_JTAGSM2:  JTAG State Machine Checksum
// greg, add bits 12-15 for firmware 07/09/2008
//------------------------------------------------------------------
const int jtag_state_machine_check_sum_vmereg =  jtag_sm_cksum_adr;
const int jtag_state_machine_check_sum_bitlo  =  0;
const int jtag_state_machine_check_sum_bithi  =  7;
//
const int jtag_state_machine_tck_fpga_vmereg  =  jtag_sm_cksum_adr;
const int jtag_state_machine_tck_fpga_bitlo   =  8;
const int jtag_state_machine_tck_fpga_bithi   = 11;
//
//
//------------------------------------------------------------------
//0XDA = ADR_VMESM0:  VME State Machine Control (reads VME PROM)
//------------------------------------------------------------------
const int vme_state_machine_start_vmereg       =  vme_sm_ctrl_adr;
const int vme_state_machine_start_bitlo        =  0;
const int vme_state_machine_start_bithi        =  0;
//
const int vme_state_machine_sreset_vmereg      =  vme_sm_ctrl_adr;
const int vme_state_machine_sreset_bitlo       =  1;
const int vme_state_machine_sreset_bithi       =  1;
//
const int vme_state_machine_autostart_vmereg   =  vme_sm_ctrl_adr;
const int vme_state_machine_autostart_bitlo    =  2;
const int vme_state_machine_autostart_bithi    =  2;
const int vme_state_machine_autostart_expected =  1;          //expect VME state machine to automatically start
//
const int vme_state_machine_busy_vmereg        =  vme_sm_ctrl_adr;
const int vme_state_machine_busy_bitlo         =  3;
const int vme_state_machine_busy_bithi         =  3;
//
const int vme_state_machine_aborted_vmereg     =  vme_sm_ctrl_adr;
const int vme_state_machine_aborted_bitlo      =  4;
const int vme_state_machine_aborted_bithi      =  4;
const int vme_state_machine_aborted_expected   =  0;             //expect VME state machine not to have aborted
//
const int vme_state_machine_cksum_ok_vmereg    =  vme_sm_ctrl_adr;
const int vme_state_machine_cksum_ok_bitlo     =  5;
const int vme_state_machine_cksum_ok_bithi     =  5;
const int vme_state_machine_cksum_ok_expected  =  1;             //expect checksum OK
//
const int vme_state_machine_wdcnt_ok_vmereg    =  vme_sm_ctrl_adr;
const int vme_state_machine_wdcnt_ok_bitlo     =  6;
const int vme_state_machine_wdcnt_ok_bithi     =  6;
const int vme_state_machine_wdcnt_ok_expected  =  1;             //expect wordcount OK
//
const int vme_state_machine_jtag_auto_vmereg   =  vme_sm_ctrl_adr;
const int vme_state_machine_jtag_auto_bitlo    =  7;
const int vme_state_machine_jtag_auto_bithi    =  7;
const int vme_state_machine_jtag_auto_default  =  1;
//
const int vme_state_machine_vme_ready_vmereg   =  vme_sm_ctrl_adr;
const int vme_state_machine_vme_ready_bitlo    =  8;
const int vme_state_machine_vme_ready_bithi    =  8;
const int vme_state_machine_vme_ready_expected =  1;           //expect VME ready
//
const int vme_state_machine_ok_vmereg          =  vme_sm_ctrl_adr;
const int vme_state_machine_ok_bitlo           =  9;
const int vme_state_machine_ok_bithi           =  9;
const int vme_state_machine_ok_expected        =  1;            //expect VME state machine OK
//
const int vme_state_machine_path_ok_vmereg     =  vme_sm_ctrl_adr;
const int vme_state_machine_path_ok_bitlo      = 10;
const int vme_state_machine_path_ok_bithi      = 10;
const int vme_state_machine_path_ok_expected   =  1;            //expect VME state machine path check OK
//
const int vme_state_machine_throttle_vmereg    =  vme_sm_ctrl_adr;
const int vme_state_machine_throttle_bitlo     = 12;
const int vme_state_machine_throttle_bithi     = 15;
//
//
//------------------------------------------------------------------
//0XDC = ADR_VMESM1:  VME State Machine Word Count
//------------------------------------------------------------------
const int vme_state_machine_word_count_vmereg =  vme_sm_wdcnt_adr;
const int vme_state_machine_word_count_bitlo  =  0;
const int vme_state_machine_word_count_bithi  = 15;
//
//
//------------------------------------------------------------------
//0XDE = ADR_VMESM2:  VME State Machine Checksum
// greg, this needs to have bits [14:13] added for 07/09/2008 firmware
// greg, this needs to be updated for ALCT prom format updates
//------------------------------------------------------------------
const int vme_state_machine_check_sum_vmereg                       =  vme_sm_cksum_adr;
const int vme_state_machine_check_sum_bitlo                        =  0;
const int vme_state_machine_check_sum_bithi                        =  7;
//
const int vme_state_machine_error_missing_header_start_vmereg      =  vme_sm_cksum_adr;
const int vme_state_machine_error_missing_header_start_bitlo       =  8;
const int vme_state_machine_error_missing_header_start_bithi       =  8;
const int vme_state_machine_error_missing_header_start_expected    =  0; //expect no error missing header start
//
const int vme_state_machine_error_missing_header_end_vmereg        =  vme_sm_cksum_adr;
const int vme_state_machine_error_missing_header_end_bitlo         =  9;
const int vme_state_machine_error_missing_header_end_bithi         =  9;
const int vme_state_machine_error_missing_header_end_expected      =  0; //expect no error missing header end
//
const int vme_state_machine_error_missing_data_end_marker_vmereg   =  vme_sm_cksum_adr;
const int vme_state_machine_error_missing_data_end_marker_bitlo    = 10;
const int vme_state_machine_error_missing_data_end_marker_bithi    = 10;
const int vme_state_machine_error_missing_data_end_marker_expected =  0; //expect no error missing data end marker
//
const int vme_state_machine_error_missing_trailer_end_vmereg       =  vme_sm_cksum_adr;
const int vme_state_machine_error_missing_trailer_end_bitlo        = 11;
const int vme_state_machine_error_missing_trailer_end_bithi        = 11;
const int vme_state_machine_error_missing_trailer_end_expected     =  0; //expect no error missing trailer end 
//
const int vme_state_machine_error_word_count_overflow_vmereg       =  vme_sm_cksum_adr;
const int vme_state_machine_error_word_count_overflow_bitlo        = 12;
const int vme_state_machine_error_word_count_overflow_bithi        = 12;
const int vme_state_machine_error_word_count_overflow_expected     =  0; //expect no error word count overflow
//
//
//------------------------------------------------------------------
//0XE0 = ADR_VMESM3:  Number of VME addresses written by VMESM
// greg, this needs state machine and data format state machine vector for 07/09/2008 firmware
//------------------------------------------------------------------
const int vme_state_machine_number_of_vme_writes_vmereg =  num_vme_sm_adr_adr;
const int vme_state_machine_number_of_vme_writes_bitlo  =  0;
const int vme_state_machine_number_of_vme_writes_bithi  = 15;
//
//
//------------------------------------------------------------------
//0XE6 = ADR_DDDR0:  RAT 3D3444 RPC Delays, 1 step = 2ns
//------------------------------------------------------------------
const int rpc0_rat_delay_vmereg  =  rat_3d_delays_adr;
const int rpc0_rat_delay_bitlo   =  0;
const int rpc0_rat_delay_bithi   =  3;
const int rpc0_rat_delay_default =  3;
//
const int rpc1_rat_delay_vmereg  =  rat_3d_delays_adr;
const int rpc1_rat_delay_bitlo   =  4;
const int rpc1_rat_delay_bithi   =  7;
const int rpc1_rat_delay_default =  3;
//
const int rpc2_rat_delay_vmereg  =  rat_3d_delays_adr;
const int rpc2_rat_delay_bitlo   =  8;
const int rpc2_rat_delay_bithi   = 11;
const int rpc2_rat_delay_default =  0;
//
const int rpc3_rat_delay_vmereg  =  rat_3d_delays_adr;
const int rpc3_rat_delay_bitlo   = 12;
const int rpc3_rat_delay_bithi   = 15;
const int rpc3_rat_delay_default =  0;
//
//
//---------------------------------------------------------------------
//0XEA = ADR_BDSTATUS:  Board Status Summary (copy of raw-hits header)
//N.B. these values are tied directly to the VME state machine and 
//     JTAG state machine status words
// greg, this needs jtag state-machine tck count added
//---------------------------------------------------------------------
const int bdstatus_ok_vmereg       =  tmb_stat_adr;
const int bdstatus_ok_bitlo        =  0;
const int bdstatus_ok_bithi        =  0;
const int bdstatus_ok_expected     =  1;    //expect board status OK
//
const int vstat_5p0v_vmereg        =  tmb_stat_adr;
const int vstat_5p0v_bitlo         =  1;
const int vstat_5p0v_bithi         =  1;
const int vstat_5p0v_expected      =  1;    //expect +5.0V status OK
//
const int vstat_3p3v_vmereg        =  tmb_stat_adr;
const int vstat_3p3v_bitlo         =  2;
const int vstat_3p3v_bithi         =  2;
const int vstat_3p3v_expected      =  1;    //expect +3.3V status OK
//
const int vstat_1p8v_vmereg        =  tmb_stat_adr;
const int vstat_1p8v_bitlo         =  3;
const int vstat_1p8v_bithi         =  3;
const int vstat_1p8v_expected      =  1;    //expect +1.8V status OK
//
const int vstat_1p5v_vmereg        =  tmb_stat_adr;
const int vstat_1p5v_bitlo         =  4;
const int vstat_1p5v_bithi         =  4;
const int vstat_1p5v_expected      =  1;    //expect +1.5V status OK
//
const int nt_crit_vmereg           =  tmb_stat_adr;
const int nt_crit_bitlo            =  5;
const int nt_crit_bithi            =  5;
const int nt_crit_expected         =  1;    //expect "not critical" temperature status OK
//
const int vsm_ok_vmereg            =  tmb_stat_adr;
const int vsm_ok_bitlo             =  6;
const int vsm_ok_bithi             =  6;
const int vsm_ok_expected          =  vme_state_machine_ok_expected;  
//
const int vsm_aborted_vmereg       =  tmb_stat_adr;
const int vsm_aborted_bitlo        =  7;
const int vsm_aborted_bithi        =  7;
const int vsm_aborted_expected     =  vme_state_machine_aborted_expected;
//
const int vsm_cksum_ok_vmereg      =  tmb_stat_adr;
const int vsm_cksum_ok_bitlo       =  8;
const int vsm_cksum_ok_bithi       =  8;
const int vsm_cksum_ok_expected    =  vme_state_machine_cksum_ok_expected;
//
const int vsm_wdcnt_ok_vmereg      =  tmb_stat_adr;
const int vsm_wdcnt_ok_bitlo       =  9;
const int vsm_wdcnt_ok_bithi       =  9;
const int vsm_wdcnt_ok_expected    =  vme_state_machine_wdcnt_ok_expected;
//
const int jsm_ok_vmereg            =  tmb_stat_adr;
const int jsm_ok_bitlo             = 10;
const int jsm_ok_bithi             = 10;
const int jsm_ok_expected          =  jtag_state_machine_ok_expected;
//
const int jsm_aborted_vmereg       =  tmb_stat_adr;
const int jsm_aborted_bitlo        = 11;
const int jsm_aborted_bithi        = 11;
const int jsm_aborted_expected     =  jtag_state_machine_aborted_expected;
//
const int jsm_cksum_ok_vmereg      =  tmb_stat_adr;
const int jsm_cksum_ok_bitlo       = 12;
const int jsm_cksum_ok_bithi       = 12;
const int jsm_cksum_ok_expected    =  jtag_state_machine_cksum_ok_expected;
//
const int jsm_wdcnt_ok_vmereg      =  tmb_stat_adr;
const int jsm_wdcnt_ok_bitlo       = 13;
const int jsm_wdcnt_ok_bithi       = 13;
const int jsm_wdcnt_ok_expected    =  jtag_state_machine_wdcnt_ok_expected;
//
const int jsm_tck_fpga_ok_vmereg   =  tmb_stat_adr;
const int jsm_tck_fpga_ok_bitlo    = 14;
const int jsm_tck_fpga_ok_bithi    = 14;
const int jsm_tck_fpga_ok_expected =  jtag_state_machine_tck_fpga_ok_expected;
//
//
//---------------------------------------------------------------------
//0XF0 = ADR_LAYER_TRIG:  Layer-Trigger Mode
//       N.B. Should be set in conjuction with register 0x68.
//---------------------------------------------------------------------
const int layer_trigger_en_vmereg   =  layer_trg_mode_adr;
const int layer_trigger_en_bitlo    =  0;
const int layer_trigger_en_bithi    =  0;
const int layer_trigger_en_default  =  0; 
//
const int layer_trig_thresh_vmereg  =  layer_trg_mode_adr;
const int layer_trig_thresh_bitlo   =  1;
const int layer_trig_thresh_bithi   =  3;
const int layer_trig_thresh_default =  4; 
//
const int number_layers_hit_vmereg  =  layer_trg_mode_adr;
const int number_layers_hit_bitlo   =  4;
const int number_layers_hit_bithi   =  6;
//
const int clct_throttle_vmereg      =  layer_trg_mode_adr;
const int clct_throttle_bitlo       =  8;
const int clct_throttle_bithi       = 15;
const int clct_throttle_default     =  0; 
//
//
//---------------------------------------------------------------------
//0XF4 = ADR_TEMP0:  Pattern Finder Pretrigger
//---------------------------------------------------------------------
const int clct_blanking_vmereg                     =  pattern_find_pretrg_adr;
const int clct_blanking_bitlo                      =  0;
const int clct_blanking_bithi                      =  0;
const int clct_blanking_default                    =  1; 
//
const int clct_pattern_id_thresh_vmereg            =  pattern_find_pretrg_adr;
const int clct_pattern_id_thresh_bitlo             =  2;
const int clct_pattern_id_thresh_bithi             =  5;
const int clct_pattern_id_thresh_default           =  0; 
//
const int clct_pattern_id_thresh_postdrift_vmereg  =  pattern_find_pretrg_adr;
const int clct_pattern_id_thresh_postdrift_bitlo   =  6;
const int clct_pattern_id_thresh_postdrift_bithi   =  9;
const int clct_pattern_id_thresh_postdrift_default =  0; 
//
const int adjacent_cfeb_distance_vmereg            =  pattern_find_pretrg_adr;
const int adjacent_cfeb_distance_bitlo             = 10;
const int adjacent_cfeb_distance_bithi             = 15;
const int adjacent_cfeb_distance_default           =  5; 
//
//
//---------------------------------------------------------------------
//0XF6 = ADR_TEMP1:  CLCT separation
//---------------------------------------------------------------------
const int clct_separation_src_vmereg               =  clct_separation_adr;
const int clct_separation_src_bitlo                =  0;
const int clct_separation_src_bithi                =  0;
const int clct_separation_src_default              =  1; 
//
const int clct_separation_ram_write_enable_vmereg  =  clct_separation_adr;
const int clct_separation_ram_write_enable_bitlo   =  1;
const int clct_separation_ram_write_enable_bithi   =  1;
const int clct_separation_ram_write_enable_default =  0; 
//
const int clct_separation_ram_adr_vmereg           =  clct_separation_adr;
const int clct_separation_ram_adr_bitlo            =  2;
const int clct_separation_ram_adr_bithi            =  5;
const int clct_separation_ram_adr_default          =  0; 
//
// greg, this needs bit-6 to add A or B separataion RAM data readback (or not?)
const int min_clct_separation_vmereg               =  clct_separation_adr;
const int min_clct_separation_bitlo                =  8;
const int min_clct_separation_bithi                = 15;
const int min_clct_separation_default              = 10; 
//
//
// greg this needs 0xfA added (SEU error status) for 08/28/2008
// greg this needs 0xfc added (CCB TTC lock status) for 08/28/2008
//////////////////////////////////////////////
// Bit mapping for TMB Raw Hits
//////////////////////////////////////////////
//-----------
// header 0
//-----------
// beginning of cathode record marker
const int h0_beginning_of_cathode_header_number =  0;
const int h0_beginning_of_cathode_lo_bit        =  0;
const int h0_beginning_of_cathode_hi_bit        = 11;
//
// marker 6
const int h0_marker_6_header_number             =  0;
const int h0_marker_6_lo_bit                    = 12;
const int h0_marker_6_hi_bit                    = 14;
//
//-----------
// header 1
//-----------
// number of time bins per CFEB in dump
const int h1_nTbins_per_cfeb_header_number =  1;
const int h1_nTbins_per_cfeb_lo_bit        =  0;
const int h1_nTbins_per_cfeb_hi_bit        =  4;
//
// CFEBs read out for this event
const int h1_cfebs_read_header_number      =  1;
const int h1_cfebs_read_lo_bit             =  5;
const int h1_cfebs_read_hi_bit             =  9;
//
// fifo mode
// 0 = no raw hits, full header (if buffer was available at pretrigger)
// 1 = all 5 CFEBs raw hits, full header (if buffer was available at pretrigger)
// 2 = Local raw hits, full header (if buffer was available at pretrigger)
// 3 = no raw hits, short header
// 4 = no raw hits, no header
const int h1_fifo_mode_header_number       =  0;
const int h1_fifo_mode_lo_bit              = 12;
const int h1_fifo_mode_hi_bit              = 14;
//
//-----------
// header 2
//-----------
// L1A received and pushed on L1A stack
const int h2_l1a_counter_header_number =  2;
const int h2_l1a_counter_lo_bit        =  0;
const int h2_l1a_counter_hi_bit        =  3;
//
// Chamber ID number (= slot/2 or slot/2-1 if slot>12 at power up)
const int h2_csc_id_header_number      =  2;
const int h2_csc_id_lo_bit             =  4;
const int h2_csc_id_hi_bit             =  7;
//
// module ID number (= VME slot at power up)
const int h2_board_id_header_number    =  2;
const int h2_board_id_lo_bit           =  8;
const int h2_board_id_hi_bit           = 12;
//
// L1A pop type mode:  
// 0 = Normal CLCT trigger with buffer data and L1A window match
// 1 = ALCT-only trigger, no data buffers
// 2 = L1A-only, no matching TMB trigger, no buffer data
// 3 = TMB triggered, no L1A-window match, event has buffer data
const int h2_l1a_type_header_number    =  2;
const int h2_l1a_type_lo_bit           = 13;
const int h2_l1a_type_hi_bit           = 14;
//
//-----------
// header 3
//-----------
// Bunch-crossing number pushed on L1A stack on L1A arrival
const int h3_bxn_counter_header_number   =  3;
const int h3_bxn_counter_lo_bit          =  0;
const int h3_bxn_counter_hi_bit          = 11;
//
// record type: 
//   0 = rawhits no   , header full
//   1 = rawhits full , header full
//   2 = rawhits local, header full
//   3 = rawhits no   , header short (no buffer was available at pretrigger)
const int h3_record_type_header_number   =  3;
const int h3_record_type_lo_bit          = 12;
const int h3_record_type_hi_bit          = 13;
//
// internal logic analyzer scop data included in readout
const int h3_scope_in_data_header_number =  3;
const int h3_scope_in_data_lo_bit        = 14;
const int h3_scope_in_data_hi_bit        = 14;
//
//-----------
// header 4
//-----------
// Number of header words
const int h4_nheader_words_header_number   =  4;
const int h4_nheader_words_lo_bit          =  0;
const int h4_nheader_words_hi_bit          =  4;
//
// Number of CFEBs readout
const int h4_nCFEBs_read_header_number     =  4;
const int h4_nCFEBs_read_lo_bit            =  5;
const int h4_nCFEBs_read_hi_bit            =  7;
//
// Number of CFEBs readout
const int h4_has_buffer_data_header_number =  4;
const int h4_has_buffer_data_lo_bit        =  8;
const int h4_has_buffer_data_hi_bit        =  8;
//
// Number time bins in readout before pretrigger
const int h4_fifo_pretrig_header_number    =  4;
const int h4_fifo_pretrig_lo_bit           =  9;
const int h4_fifo_pretrig_hi_bit           = 13;
//
//-----------
// header 5
//-----------
// L1A number at CLCT pretrigger
const int h5_l1a_at_pretrig_header_number                   =  5;
const int h5_l1a_at_pretrig_lo_bit                          =  0;
const int h5_l1a_at_pretrig_hi_bit                          =  3;
//
// trigger source vector
const int h5_trigger_source_vector_header_number            =  5;
const int h5_trigger_source_vector_lo_bit                   =  4;
const int h5_trigger_source_vector_hi_bit                   = 11;
//
// trigger source halfstrip or distrip
const int h5_trigger_source_halfstrip_distrip_header_number =  5;
const int h5_trigger_source_halfstrip_distrip_lo_bit        = 12;
const int h5_trigger_source_halfstrip_distrip_hi_bit        = 12;
//
//-----------
// header 6
//-----------
// Active CFEB list sent to DMB
const int h6_aff_to_dmb_header_number  =  6;
const int h6_aff_to_dmb_lo_bit         =  0;
const int h6_aff_to_dmb_hi_bit         =  4;
//
// List of instantiated CFEBs
const int h6_cfeb_exists_header_number =  6;
const int h6_cfeb_exists_lo_bit        =  5;
const int h6_cfeb_exists_hi_bit        =  9;
//
// Run info
const int h6_run_info_header_number    =  6;
const int h6_run_info_lo_bit           = 10;
const int h6_run_info_hi_bit           = 13;
//
//-----------
// header 7
//-----------
// bunch crossing number at CLCT pretrigger
const int h7_bxn_at_clct_pretrig_header_number =  7;
const int h7_bxn_at_clct_pretrig_lo_bit        =  0;
const int h7_bxn_at_clct_pretrig_hi_bit        = 11;
//
// bunch crossing number synchronization error
const int h7_sync_err_header_number            =  7;
const int h7_sync_err_lo_bit                   = 12;
const int h7_sync_err_hi_bit                   = 12;
//
//-----------
// header 8
//-----------
// CLCT0 pattern trigger (after drift) LSBS
const int h8_clct0_lsbs_header_number =  8;
const int h8_clct0_lsbs_lo_bit        =  0;
const int h8_clct0_lsbs_hi_bit        = 14;
//
//-----------
// header 9
//-----------
// CLCT1 pattern trigger (after drift) LSBS
const int h9_clct1_lsbs_header_number =  9;
const int h9_clct1_lsbs_lo_bit        =  0;
const int h9_clct1_lsbs_hi_bit        = 14;
//
//-----------
// header 10
//-----------
// CLCT0 pattern trigger (after drift) MSBS
const int h10_clct0_msbs_header_number            = 10;
const int h10_clct0_msbs_lo_bit                   =  0;
const int h10_clct0_msbs_hi_bit                   =  5;
//
// CLCT1 pattern trigger (after drift) MSBS
const int h10_clct1_msbs_header_number            = 10;
const int h10_clct1_msbs_lo_bit                   =  6;
const int h10_clct1_msbs_hi_bit                   = 11;
//
// CLCT0 had invalid pattern after drift
const int h10_clct0_invalid_pattern_header_number = 10;
const int h10_clct0_invalid_pattern_lo_bit        = 12;
const int h10_clct0_invalid_pattern_hi_bit        = 12;
//
//-----------
// header 11
//-----------
// ALCT and CLCT matched in time
const int h11_alct_clct_match_header_number           = 11;
const int h11_alct_clct_match_lo_bit                  =  0;
const int h11_alct_clct_match_hi_bit                  =  0;
//
// ALCT trigger only
const int h11_alct_trig_only_header_number            = 11;
const int h11_alct_trig_only_lo_bit                   =  1;
const int h11_alct_trig_only_hi_bit                   =  1;
//
// CLCT trigger only
const int h11_clct_trig_only_header_number            = 11;
const int h11_clct_trig_only_lo_bit                   =  2;
const int h11_clct_trig_only_hi_bit                   =  2;
//
// ALCT-CLCT0 bunch crossing difference
const int h11_clct0_alct_bxn_diff_header_number       = 11;
const int h11_clct0_alct_bxn_diff_lo_bit              =  3;
const int h11_clct0_alct_bxn_diff_hi_bit              =  4;
//
// ALCT-CLCT1 bunch crossing difference
const int h11_clct1_alct_bxn_diff_header_number       = 11;
const int h11_clct1_alct_bxn_diff_lo_bit              =  5;
const int h11_clct1_alct_bxn_diff_hi_bit              =  6;
//
// Location of ALCT in CLCT match window
const int h11_alct_in_clct_match_window_header_number = 11;
const int h11_alct_in_clct_match_window_lo_bit        =  7;
const int h11_alct_in_clct_match_window_hi_bit        = 10;
//
// triad persistence
const int h11_triad_persistence_header_number         = 11;
const int h11_triad_persistence_lo_bit                = 11;
const int h11_triad_persistence_hi_bit                = 14;
//
//-----------
// header 12
//-----------
// MPC muon0 frame 0 LSBs
const int h12_mpc0_frame0_lsbs_header_number = 12;
const int h12_mpc0_frame0_lsbs_lo_bit        =  0;
const int h12_mpc0_frame0_lsbs_hi_bit        = 14;
//
//-----------
// header 13
//-----------
// MPC muon0 frame 1 LSBs
const int h13_mpc0_frame1_lsbs_header_number = 13;
const int h13_mpc0_frame1_lsbs_lo_bit        =  0;
const int h13_mpc0_frame1_lsbs_hi_bit        = 14;
//
//-----------
// header 14
//-----------
// MPC muon1 frame 0 LSBs
const int h14_mpc1_frame0_lsbs_header_number = 14;
const int h14_mpc1_frame0_lsbs_lo_bit        =  0;
const int h14_mpc1_frame0_lsbs_hi_bit        = 14;
//
//-----------
// header 15
//-----------
// MPC muon1 frame 1 LSBs
const int h15_mpc1_frame1_lsbs_header_number = 15;
const int h15_mpc1_frame1_lsbs_lo_bit        =  0;
const int h15_mpc1_frame1_lsbs_hi_bit        = 14;
//
//-----------
// header 16
//-----------
// MPC muon0 frame 0 MSBs
const int h16_mpc0_frame0_msbs_header_number              = 16;
const int h16_mpc0_frame0_msbs_lo_bit                     =  0;
const int h16_mpc0_frame0_msbs_hi_bit                     =  0;
//
// MPC muon0 frame 1 MSBs
const int h16_mpc0_frame1_msbs_header_number              = 16;
const int h16_mpc0_frame1_msbs_lo_bit                     =  1;
const int h16_mpc0_frame1_msbs_hi_bit                     =  1;
//
// MPC muon1 frame 0 MSBs
const int h16_mpc1_frame0_msbs_header_number              = 16;
const int h16_mpc1_frame0_msbs_lo_bit                     =  2;
const int h16_mpc1_frame0_msbs_hi_bit                     =  2;
//
// MPC muon1 frame 1 MSBs
const int h16_mpc1_frame1_msbs_header_number              = 16;
const int h16_mpc1_frame1_msbs_lo_bit                     =  3;
const int h16_mpc1_frame1_msbs_hi_bit                     =  3;
//
// MPC muon accept response
const int h16_mpc_accept_header_number                    = 16;
const int h16_mpc_accept_lo_bit                           =  4;
const int h16_mpc_accept_hi_bit                           =  5;
//
// CLCT halfstrip pretrigger threshold
const int h16_clct_halfstrip_pretrig_thresh_header_number = 16;
const int h16_clct_halfstrip_pretrig_thresh_lo_bit        =  8;
const int h16_clct_halfstrip_pretrig_thresh_hi_bit        = 10;
//
// CLCT distrip pretrigger threshold
const int h16_clct_distrip_pretrig_thresh_header_number   = 16;
const int h16_clct_distrip_pretrig_thresh_lo_bit          = 11;
const int h16_clct_distrip_pretrig_thresh_hi_bit          = 13;
//
//-----------
// header 17
//-----------
// Write buffer is ready
const int h17_write_buffer_ready_header_number     = 17;
const int h17_write_buffer_ready_lo_bit            =  0;
const int h17_write_buffer_ready_hi_bit            =  0;
//
// Tbin address for pretrig
const int h17_pretrig_tbin_header_number           = 17;
const int h17_pretrig_tbin_lo_bit                  =  1;
const int h17_pretrig_tbin_hi_bit                  =  5;
//
// write buffer address 
const int h17_write_buffer_address_header_number   = 17;
const int h17_write_buffer_address_lo_bit          =  6;
const int h17_write_buffer_address_hi_bit          =  8;
//
// pretrig arrived, no buffer free
const int h17_pretrig_no_free_buffer_header_number = 17;
const int h17_pretrig_no_free_buffer_lo_bit        =  9;
const int h17_pretrig_no_free_buffer_hi_bit        =  9;
//
// buffer full
const int h17_buffer_full_header_number            = 17;
const int h17_buffer_full_lo_bit                   = 10;
const int h17_buffer_full_hi_bit                   = 10;
//
// buffer almost full
const int h17_buffer_almost_full_header_number     = 17;
const int h17_buffer_almost_full_lo_bit            = 11;
const int h17_buffer_almost_full_hi_bit            = 11;
//
// buffer half full
const int h17_buffer_half_full_header_number       = 17;
const int h17_buffer_half_full_lo_bit              = 12;
const int h17_buffer_half_full_hi_bit              = 12;
//
// buffer empty
const int h17_buffer_empty_header_number           = 17;
const int h17_buffer_empty_lo_bit                  = 13;
const int h17_buffer_empty_hi_bit                  = 13;
//
//-----------
// header 18
//-----------
// Number of buffers busy
const int h18_nbuf_busy_header_number          = 18;
const int h18_nbuf_busy_lo_bit                 =  0;
const int h18_nbuf_busy_hi_bit                 =  3;
//
// List of busy buffers
const int h18_buf_busy_header_number           = 18;
const int h18_buf_busy_lo_bit                  =  4;
const int h18_buf_busy_hi_bit                  = 11;
//
// L1A stack overflow
const int h18_l1a_stack_overflow_header_number = 18;
const int h18_l1a_stack_overflow_lo_bit        = 13;
const int h18_l1a_stack_overflow_hi_bit        = 13;
//
//-----------
// header 19
//-----------
// TMB response
const int h19_tmb_trig_pulse_header_number         = 19;
const int h19_tmb_trig_pulse_lo_bit                =  0;
const int h19_tmb_trig_pulse_hi_bit                =  0;
//
// Only ALCT triggered
const int h19_tmb_alct_only_header_number          = 19;
const int h19_tmb_alct_only_lo_bit                 =  1;
const int h19_tmb_alct_only_hi_bit                 =  1;
//
// Only CLCT triggered
const int h19_tmb_clct_only_header_number          = 19;
const int h19_tmb_clct_only_lo_bit                 =  2;
const int h19_tmb_clct_only_hi_bit                 =  2;
//
// ALCT*CLCT triggered
const int h19_tmb_match_header_number              = 19;
const int h19_tmb_match_lo_bit                     =  3;
const int h19_tmb_match_hi_bit                     =  3;
//
// Write buffer ready at pretrig
const int h19_write_buffer_ready_header_number     = 19;
const int h19_write_buffer_ready_lo_bit            =  4;
const int h19_write_buffer_ready_hi_bit            =  4;
//
// write buffer either (ready -or- not required) at pretrig
const int h19_write_buffer_available_header_number = 19;
const int h19_write_buffer_available_lo_bit        =  5;
const int h19_write_buffer_available_hi_bit        =  5;
//
// Tbin address at pretrig
const int h19_write_tbin_address_header_number     = 19;
const int h19_write_tbin_address_lo_bit            =  6;
const int h19_write_tbin_address_hi_bit            = 10;
//
// Address of write buffer at pretrig
const int h19_write_buffer_address_header_number   = 19;
const int h19_write_buffer_address_lo_bit          = 11;
const int h19_write_buffer_address_hi_bit          = 13;
//
//-----------
// header 20
//-----------
// pretrig but no write buffer available
const int h20_discard_no_write_buf_available_header_number = 20;
const int h20_discard_no_write_buf_available_lo_bit        =  0;
const int h20_discard_no_write_buf_available_hi_bit        =  3;
//
// invalid pattern after drift
const int h20_discard_invalid_pattern_header_number        = 20;
const int h20_discard_invalid_pattern_lo_bit               =  4;
const int h20_discard_invalid_pattern_hi_bit               =  7;
//
// TMB rejected event
const int h20_discard_tmb_reject_header_number             = 20;
const int h20_discard_tmb_reject_lo_bit                    =  8;
const int h20_discard_tmb_reject_hi_bit                    = 11;
//
// timeout with no TMB trig pulse
const int h20_timeout_no_tmb_trig_pulse_header_number      = 20;
const int h20_timeout_no_tmb_trig_pulse_lo_bit             = 12;
const int h20_timeout_no_tmb_trig_pulse_hi_bit             = 12;
//
// timeout with no mpc_frame_ff
const int h20_timeout_no_mpc_frame_header_number           = 20;
const int h20_timeout_no_mpc_frame_lo_bit                  = 13;
const int h20_timeout_no_mpc_frame_hi_bit                  = 13;
//
// timeout with no mpc_response_ff
const int h20_timeout_no_mpc_response_header_number        = 20;
const int h20_timeout_no_mpc_response_lo_bit               = 14;
const int h20_timeout_no_mpc_response_hi_bit               = 14;
//
//-----------
// header 21
//-----------
// setting of ALCT delay for match window
const int h21_match_trig_alct_delay_header_number   = 21;
const int h21_match_trig_alct_delay_lo_bit          =  0;
const int h21_match_trig_alct_delay_hi_bit          =  3;
//
// setting of match window width
const int h21_match_trig_window_width_header_number = 21;
const int h21_match_trig_window_width_lo_bit        =  4;
const int h21_match_trig_window_width_hi_bit        =  7;
//
// setting of MPC transmit delay
const int h21_mpc_tx_delay_header_number            = 21;
const int h21_mpc_tx_delay_lo_bit                   =  8;
const int h21_mpc_tx_delay_hi_bit                   = 11;
//
//-----------
// header 22
//-----------
// RPCs connected to this TMB
const int h22_rpc_exist_header_number       = 22;
const int h22_rpc_exist_lo_bit              =  0;
const int h22_rpc_exist_hi_bit              =  1;
//
// RPCs included in readout
const int h22_rpc_list_header_number        = 22;
const int h22_rpc_list_lo_bit               =  2;
const int h22_rpc_list_hi_bit               =  3;
//
// Number of RPCs in readout
const int h22_nrpc_header_number            = 22;
const int h22_nrpc_lo_bit                   =  4;
const int h22_nrpc_hi_bit                   =  5;
//
// RPC readout enabled
const int h22_rpc_read_enable_header_number = 22;
const int h22_rpc_read_enable_lo_bit        =  6;
const int h22_rpc_read_enable_hi_bit        =  6;
//
// Number of layers hit on layer trigger
const int h22_nlayers_hit_header_number     = 22;
const int h22_nlayers_hit_lo_bit            =  7;
const int h22_nlayers_hit_hi_bit            =  9;
//
// Position of L1A in window
const int h22_l1a_in_window_header_number   = 22;
const int h22_l1a_in_window_lo_bit          = 10;
const int h22_l1a_in_window_hi_bit          = 13;
//
//-----------
// header 23
//-----------
// Board status
const int h23_board_status_header_number = 23;
const int h23_board_status_lo_bit        =  0;
const int h23_board_status_hi_bit        = 14;
//
//-----------
// header 24
//-----------
// seconds since last hard reset
const int h24_time_since_hard_reset_header_number = 24;
const int h24_time_since_hard_reset_lo_bit        =  0;
const int h24_time_since_hard_reset_hi_bit        = 14;
//
//-----------
// header 25
//-----------
// Firmware version date code
const int h25_firmware_version_date_code_header_number = 25;
const int h25_firmware_version_date_code_lo_bit        =  0;
const int h25_firmware_version_date_code_hi_bit        = 13;
//
//
/////////////////////////////////////////////////////////////////////////////
// The following stuff should be deprecated
/////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// TMB trigger modes:
//------------------------------------------
const int CLCT_trigger                  =  1;
const int ALCT_trigger                  =  2;
const int Scintillator_trigger          =  3;
const int DMB_trigger                   =  4;
const int ALCT_CLCT_coincidence_trigger =  5;
//
//
//------------------------------------------
// this comes from alct_routines2/tmb_vme_reg.h
//------------------------------------------
//#define TMB_ADR_BOOT            0x70000 // Hardware Bootstrap Register
//
//--------------------------------
//  Definitions to work with TMB
//-------------------------------
#define VME_BOOT_REG    0x04
//
#define ALCT_SLOW_PROG  0x0     // ALCT Slow Control JTAG Programming Chain
#define ALCT_SLOW_USER  0x1     // ALCT Slow Control JTAG User Chain
#define ALCT_FAST_PROG  0x2     // ALCT Fast JTAG Programming Chain
#define ALCT_FAST_USER  0x3     // ALCT Fast JTAG User Chain
#define TMB_FPGA_PROG   0x4     // TMB Mezzanine FPGA + FPGA PROMs JTAG Chain
#define TMB_USER_PROM   0x8     // TMB User PROMs JTAG Chain
#define TMB_FPGA_USER   0xc     // TMB FPGA User JTAG Chain
//
#define TMB_JTAG_SRC    0x08    // Hardware Bootstrap Register JTAG Source Bit
#define JTAG_SOFT_SRC   0x0     // JTAG Sourced by FPGA (TMB_ADR_USR_JTAG VME Register)
#define JTAG_HARD_SRC   0x1     // JTAG Sourced by Bootstrap Register
//
#define ALCT_HARD_RESET 0x0100  // Hardware ALCT FPGA Hard Reset
#define TMB_HARD_RESET  0x0200  // Hardware TMB FPGA Hard Reset
#define TMB_ENABLE_ALCT_RESET 0x0400 // Enable ALCT Hard Reset
#define TMB_ENABLE_VME  0x0800  // Enable TMB to issue VME commands
//
struct tmb_id_regs
{
/* TMB_ADR_IDREG0 */
  unsigned char fw_type;        // TMB Firmware Type, C=Normal CLCT/TMB, D=Debug loopback
  unsigned char fw_version;     // Firmware Version code
  unsigned char brd_geo_addr;   // Geographic address for this board

/* TMB_ADR_IDREG1 */
  unsigned char fw_day;         // DD Firmware Version Day (BCD)
  unsigned char fw_month;       // MM Firmware Version Month (BCD)

/* TMB_ADR_IDREG2 */
  unsigned short int fw_year;   // YYYY Firmware Version Year (BCD)

/* TMB_ADR_IDREG3 */
  unsigned short int fpga_type; // Xilinx FPGA Type XCV1000E (BCD)

};

  } // namespace emu::pc
  } // namespace emu
