//-----------------------------------------------------------------------
// $Id: TMB_constants.h,v 3.5 2006/10/12 15:56:01 rakness Exp $
// $Log: TMB_constants.h,v $
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
//-----------------------------------------------------------------------

static const unsigned long int vme_idreg0_adr      = 0x000000;
static const unsigned long int vme_idreg1_adr      = 0x000002;
static const unsigned long int vme_idreg2_adr      = 0x000004;
static const unsigned long int vme_idreg3_adr      = 0x000006;
static const unsigned long int vme_status_adr      = 0x000008;
static const unsigned long int vme_adr0_adr        = 0x00000A;
static const unsigned long int vme_adr1_adr        = 0x00000C;
static const unsigned long int vme_loopbk_adr      = 0x00000E;

static const unsigned long int	vme_usr_jtag_adr   = 0x000010;
static const unsigned long int	vme_prom_adr	   = 0x000012;
/*
static const unsigned long int	vme_phos4sm_adr	   = 0x000014;
static const unsigned long int	vme_phos4a_adr	   = 0x000016;
static const unsigned long int	vme_phos4b_adr	   = 0x000018;
static const unsigned long int	vme_phos4c_adr	   = 0x00001A;
static const unsigned long int	vme_phos4d_adr	   = 0x00001C;
static const unsigned long int	vme_phos4e_adr	   = 0x00001E;
*/
static const unsigned long int	vme_dddsm_adr	   = 0x000014;
static const unsigned long int	vme_ddd0_adr	   = 0x000016;
static const unsigned long int	vme_ddd1_adr	   = 0x000018;
static const unsigned long int	vme_ddd2_adr	   = 0x00001A;
static const unsigned long int	vme_dddoe_adr	   = 0x00001C;
static const unsigned long int	vme_ratctrl_adr	   = 0x00001E;
//
static const unsigned long int	vme_step_adr	   = 0x000020;
static const unsigned long int	vme_led_adr	   = 0x000022;
static const unsigned long int	vme_adc_adr	   = 0x000024;
static const unsigned long int	vme_dsn_adr	   = 0x000026;

static const unsigned long int	mod_cfg_adr	   = 0x000028;	
static const unsigned long int	ccb_cfg_adr	   = 0x00002A;
static const unsigned long int	ccb_trig_adr	   = 0x00002C;
static const unsigned long int	ccb_stat_adr	   = 0x00002E;	
static const unsigned long int	alct_cfg_adr	   = 0x000030;
static const unsigned long int	alct_inj_adr	   = 0x000032;
static const unsigned long int	alct0_inj_adr	   = 0x000034;
static const unsigned long int	alct1_inj_adr	   = 0x000036;
static const unsigned long int	alct_stat_adr	   = 0x000038;
static const unsigned long int	alct_alct0_adr	   = 0x00003A;
static const unsigned long int	alct_alct1_adr	   = 0x00003C;
static const unsigned long int	alct_fifo_adr	   = 0x00003E;
static const unsigned long int	dmb_mon_adr	   = 0x000040;
static const unsigned long int	cfeb_inj_adr	   = 0x000042;
static const unsigned long int	cfeb_inj_adr_adr   = 0x000044;
static const unsigned long int	cfeb_inj_wdata_adr = 0x000046;
static const unsigned long int	cfeb_inj_rdata_adr = 0x000048;
static const unsigned long int	hcm001_adr	   = 0x00004A;
static const unsigned long int	hcm023_adr	   = 0x00004C;
static const unsigned long int	hcm045_adr	   = 0x00004E;
static const unsigned long int	hcm101_adr	   = 0x000050;
static const unsigned long int	hcm123_adr	   = 0x000052;
static const unsigned long int	hcm145_adr	   = 0x000054;
static const unsigned long int	hcm201_adr	   = 0x000056;
static const unsigned long int	hcm223_adr	   = 0x000058;
static const unsigned long int	hcm245_adr	   = 0x00005A;
static const unsigned long int	hcm301_adr	   = 0x00005C;
static const unsigned long int	hcm323_adr	   = 0x00005E;
static const unsigned long int	hcm345_adr	   = 0x000060;
static const unsigned long int	hcm401_adr	   = 0x000062;
static const unsigned long int	hcm423_adr	   = 0x000064;
static const unsigned long int	hcm445_adr	   = 0x000066;
static const unsigned long int	seq_trig_en_adr	   = 0x000068;
static const unsigned long int	seq_trig_dly2_adr  = 0x00006A;
static const unsigned long int	seq_trig_dly1_adr  = 0x00006C;
static const unsigned long int	seq_id_adr	   = 0x00006E;
static const unsigned long int	seq_clct_adr	   = 0x000070;
static const unsigned long int	seq_fifo_adr	   = 0x000072;
static const unsigned long int	seq_l1a_adr	   = 0x000074;
static const unsigned long int	seq_offset_adr	   = 0x000076;
static const unsigned long int	seq_clct0_adr	   = 0x000078;
static const unsigned long int	seq_clct1_adr	   = 0x00007A;
static const unsigned long int	seq_trig_src_adr   = 0x00007C;
static const unsigned long int	dmb_ram_adr	   = 0x00007E;
static const unsigned long int	dmb_wdata_adr	   = 0x000080;
static const unsigned long int	dmb_wdcnt_adr	   = 0x000082;
static const unsigned long int	dmb_rdata_adr	   = 0x000084;
static const unsigned long int	tmb_trig_adr	   = 0x000086;
static const unsigned long int	mpc0_frame0_adr	   = 0x000088;
static const unsigned long int	mpc0_frame1_adr	   = 0x00008A;
static const unsigned long int	mpc1_frame0_adr	   = 0x00008C;
static const unsigned long int	mpc1_frame1_adr	   = 0x00008E;
static const unsigned long int	mpc_inj_adr	   = 0x000090;
static const unsigned long int	mpc_ram_adr	   = 0x000092;
static const unsigned long int	mpc_ram_wdata_adr  = 0x000094;
static const unsigned long int	mpc_ram_rdata_adr  = 0x000096;
static const unsigned long int	scp_ctrl_adr	   = 0x000098;
static const unsigned long int	scp_rdata_adr	   = 0x00009A;

static const unsigned long int	ccb_cmd_adr	   = 0x00009C;	//For tmb e-version
static const unsigned long int	buf_stat_adr	   = 0x00009E;
static const unsigned long int	srlpgm_adr	   = 0x0000A0;
static const unsigned long int	alctfifo1_adr	   = 0x0000A2;
static const unsigned long int	alctfifo2_adr	   = 0x0000A4;
static const unsigned long int	adjcfeb0_adr	   = 0x0000A6;
static const unsigned long int	adjcfeb1_adr	   = 0x0000A8;
static const unsigned long int	adjcfeb2_adr	   = 0x0000AA;
static const unsigned long int	seqmod_adr	   = 0x0000AC;
static const unsigned long int	seqsm_adr	   = 0x0000AE;
static const unsigned long int	seq_clctm_adr	   = 0x0000B0;
static const unsigned long int	tmbtim_adr	   = 0x0000B2;
static const unsigned long int  lhc_cycle_adr      = 0x0000B4;
//
static const unsigned long int  rpc_cfg_adr        = 0x0000B6;
static const unsigned long int  rpc_rdata_adr      = 0x0000B8;
static const unsigned long int  rpc_raw_delay_adr  = 0x0000BA;
static const unsigned long int  rpc_inj_adr        = 0x0000BC;
static const unsigned long int  adr_rpc_inj_adr    = 0x0000BE;
//
static const unsigned long int  rpc_inj_wdata_adr  = 0x0000C0;
static const unsigned long int  rpc_inj_rdata_adr  = 0x0000C2;
static const unsigned long int  rpc_bxn_diff_adr   = 0x0000C4;
static const unsigned long int  rpc0_hcm_adr       = 0x0000C6;
static const unsigned long int  rpc1_hcm_adr       = 0x0000C8;
static const unsigned long int  rpc2_hcm_adr       = 0x0000CA;
static const unsigned long int  rpc3_hcm_adr       = 0x0000CC;
//
static const unsigned long int  scp_trig_adr       = 0x0000CE;
//
static const unsigned long int  cnt_ctrl_adr       = 0x0000D0;
static const unsigned long int  cnt_rdata_adr      = 0x0000D2;
//
static const unsigned long int  jtag_sm_ctrl_adr   = 0x0000D4;
static const unsigned long int  jtag_sm_wdcnt_adr  = 0x0000D6;
static const unsigned long int  jtag_sm_cksum_adr  = 0x0000D8;
static const unsigned long int  vme_sm_ctrl_adr    = 0x0000DA;
static const unsigned long int  vme_sm_wdcnt_adr   = 0x0000DC;
static const unsigned long int  vme_sm_cksum_adr   = 0x0000DE;
static const unsigned long int  num_vme_sm_adr_adr = 0x0000E0;
static const unsigned long int  vme_wrt_dat_ck_adr = 0x0000E2;
//
static const unsigned long int  rat_3d_sm_ctrl_adr = 0x0000E4;
static const unsigned long int  rat_3d_delays_adr  = 0x0000E6;
static const unsigned long int  uptime_adr         = 0x0000E8;
static const unsigned long int  tmb_stat_adr       = 0x0000EA;
static const unsigned long int  bxn_clct_adr       = 0x0000EC;
static const unsigned long int  bxn_alct_adr       = 0x0000EE;
//
// this comes from alct_routines2/tmb_vme_reg.h
#define TMB_ADR_BOOT            0x70000 // Hardware Bootstrap Register
/*
#define TMB_ADR_IDREG0          0x00    // ID Register 0
#define TMB_ADR_IDREG1          0x02    // ID Register 1
#define TMB_ADR_IDREG2          0x04    // ID Register 2
#define TMB_ADR_IDREG3          0x06    // ID Register 3

#define TMB_ADR_VME_STATUS      0x08    // VME Status Register
#define TMB_ADR_VME_ADR0        0x0a    // VME Address read-back
#define TMB_ADR_VME_ADR1        0x0c    // VME Address read-back

#define TMB_ADR_LOOPBK          0x0e    // Loop-back Register
#define TMB_ADR_USR_JTAG        0x10    // User JTAG
#define TMB_ADR_PROM            0x12    // PROM

#define TMB_ADR_PHOS4SM         0x14    // PHOS4 State Machine Register + Clock DLLs
#define TMB_ADR_PHOS4A          0x16    // PHOS4 Chip 0 Channels 0,1
#define TMB_ADR_PHOS4B          0x18    // PHOS4 Chip 0 Channels 2,3
#define TMB_ADR_PHOS4C          0x1a    // PHOS4 Chip 0 Channels 4, Chip 1 Channel 0
#define TMB_ADR_PHOS4D          0x1c    // PHOS4 Chip 1 Channels 1,2
#define TMB_ADR_PHOS4E          0x1e    // PHOS4 Chip 1 Channels 3,4

#define TMB_ADR_STEP            0x20    // Step Register
#define TMB_ADR_LED             0x22    // Front Panel LEDs
#define TMB_ADR_ADC             0x24    // ADCs
#define TMB_ADR_DSN             0x26    // Difital Serials

#define TMB_ADR_MOD_CFG         0x28    // TMB Configuration
#define TMB_ADR_CCB_CFG         0x2a    // CCB Configuration
#define TMB_ADR_CCB_TRIG        0x2c    // CCB Trigger Control
#define TMB_ADR_CCB_STAT        0x2e    // CCB Status

#define TMB_ADR_ALCT_CFG        0x30    // ALCT Configuration
#define TMB_ADR_ALCT_INJ        0x32    // ALCT Injector Control
#define TMB_ADR_ALCT0_INJ       0x34    // ALCT Injected ALCT0
#define TMB_ADR_ALCT1_INJ       0x36    // ALCT Injected ALCT1
#define TMB_ADR_ALCT_STAT       0x38    // ALCT Sequencer Control/Status
#define TMB_ADR_ALCT0_RCD       0x3a    // ALCT LCT0 Received by TMB
#define TMB_ADR_ALCT1_RCD       0x3c    // ALCT LCT1 Received by TMB
#define TMB_ALCT_FIFO           0x3e    // ALCT FIFO word count

#define TMB_ADR_DMB_MON         0x40    // DMB Monitored signals

#define TMB_ADR_CFEB_INJ        0x42    // CFEB Injector Control
#define TMB_ADR_CFEB_INJ_ADR    0x44    // CFEB Injector RAM Address
#define TMB_ADR_CFEB_INJ_WDATA  0x46    // CFEB Injector Write Data
#define TMb_ADR_CFEB_INJ_RDATA  0x48    // CFEB INjector Read Data

#define TMB_ADR_HCM001          0x4a    // CFEB0 Ly0,Ly1 Hot Channel Mask
#define TMB_ADR_HCM023          0x4c    // CFEB0 Ly2,Ly3 Hot Channel Mask
#define TMB_ADR_HCM045          0x4e    // CFEB0 Ly4,Ly5 Hot Channel Mask
#define TMB_ADR_HCM101          0x50    // CFEB1 Ly0,Ly1 Hot Channel Mask
#define TMB_ADR_HCM123          0x52    // CFEB1 Ly2,Ly3 Hot Channel Mask
#define TMB_ADR_HCM145          0x54    // CFEB1 Ly4,Ly5 Hot Channel Mask
#define TMB_ADR_HCM201          0x56    // CFEB2 Ly0,Ly1 Hot Channel Mask
#define TMB_ADR_HCM223          0x58    // CFEB2 Ly2,Ly3 Hot Channel Mask
#define TMB_ADR_HCM245          0x5a    // CFEB2 Ly4,Ly5 Hot Channel Mask
#define TMB_ADR_HCM301          0x5c    // CFEB3 Ly0,Ly1 Hot Channel Mask
#define TMB_ADR_HCM323          0x5e    // CFEB3 Ly2,Ly3 Hot Channel Mask
#define TMB_ADR_HCM345          0x60    // CFEB3 Ly4,Ly5 Hot Channel Mask
#define TMB_ADR_HCM401          0x62    // CFEB4 Ly0,Ly1 Hot Channel Mask
#define TMB_ADR_HCM423          0x64    // CFEB4 Ly2,Ly3 Hot Channel Mask
#define TMB_ADR_HCM445          0x66    // CFEB4 Ly4,Ly5 Hot Channel Mask

#define TMB_ADR_SEQ_TRIG        0x68    // Sequencer Trigger Control
#define TMB_ADR_SEQ_CLCT        0x6a    // Sequencer CLCT Configuration
#define TMB_ADR_SEQ_FIFO        0x6c    // Sequencer FIFO Configuration
#define TMB_ADR_SEQ_L1A         0x6e    // Sequencer L1A Configuration
#define TMB_ADR_SEQ_OFFSET      0x70    // Sequencer Counter Offsets
#define TMB_ADR_SEQ_CLCT0       0x72    // Sequencer Latched CLCT0
#define TMB_ADR_SEQ_CLCT1       0x74    // Sequencer Latched CLCT1

#define TMB_ADR_DMB_RAM_ADR     0x76    // Sequencer RAM Address
#define TMB_ADR_DMB_RAM_WDATA   0x78    // Sequencer RAM Write Data
#define TMB_ADR_DMB_RAM_WDCNT   0x7a    // Sequencer RAM Word Count
#define TMB_ADR_DMB_RAM_RDATA   0x7c    // Sequencer RAM Read Data

#define TMB_ADR_TMB_TRIG        0x7e    // TMB Trigger Configuration

#define TMB_ADR_MPC0_FRAME0     0x80    // MPC0 Frame 0 Data sent to MPC
#define TMB_ADR_MPC0_FRAME1     0x82    // MPC0 Frame 1 Data sent to MPC
#define TMB_ADR_MPC1_FRAME0     0x84    // MPC1 Frame 0 Data sent to MPC
#define TMB_ADR_MPC1_FRAME1     0x86    // MPC1 Frame 1 Data sent to MPC

#define TMB_ADR_MPC_INJ         0x88    // MPC Injector Control
#define TMB_ADR_MPC_RAM_ADR     0x8a    // MPC Injector RAM Address
#define TMB_ADR_MPC_RAM_WDATA   0x8c    // MPC Injector RAM Write Data
#define TMB_ADR_MPC_RAM_RDATA   0x8e    // MPC Injector RAM Read Data

#define TMB_ADR_LHC_CYCLE       0xb4    // LHC cycle period, Max BXN + 1
#define TMB_ADR_RPC_CFG         0xb6    // RPC configuration

// this comes from alctroutines2/tmb_vme.h
*/
/*
 *  Definitions to work with TMB
 */

#define VME_BOOT_REG    0x04

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

#define SOFT_TDO        0x01    // Software VME Register JTAG TDO Bit
#define SOFT_TDI        0x02    // Software VME Register JTAG TDI Bit
#define SOFT_TMS        0x03    // Software VME Register JTAG TMS Bit
#define SOFT_TCK        0x08    // Software VME Register JTAG TCK Bit

#define HARD_TDO        0x80    // Hardware Bootstrap Register JTAG TDO Bit
#define HARD_TDI        0x01    // Hardware Bootstrap Register JTAG TDI Bit
#define HARD_TMS        0x02    // Hardware Bootstrap Register JTAG TMS Bit
#define HARD_TCK        0x04    // Hardware Bootstrap Register JTAG TCK Bit

#define ALCT_HARD_RESET 0x0100  // Hardware ALCT FPGA Hard Reset
#define TMB_HARD_RESET  0x0200  // Hardware TMB FPGA Hard Reset
#define TMB_ENABLE_ALCT_RESET 0x0400 // Enable ALCT Hard Reset
#define TMB_ENABLE_VME  0x0800  // Enable TMB to issue VME commands

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
//
const int number_of_allowed_configuration_addresses = 22;
const int allowed_configuration_addresses[number_of_allowed_configuration_addresses] = {
  rpc_cfg_adr,          //0xb6 enable RPC 
  vme_ratctrl_adr,      //0x1e add 1/2-cycle to RPC latching
  vme_loopbk_adr,       //0x0e enable ALCT LVDS rx/tx
  ccb_trig_adr,         //0x2c configure request l1a from CCB 
  seq_fifo_adr,         //0x72 sequencer fifo configuration
  seq_trig_en_adr,      //0x68 sequencer trigger source configuration
  tmbtim_adr,           //0xb2 ALCT*CLCT coincidence timing 
  seq_offset_adr,       //0x76 TMB sequencer counter offsets
  seq_clct_adr,         //0x70 CLCT sequencer configuration
  scp_ctrl_adr,         //0x98 Sequencer readout mode ([5]=1 insert in DMB data), RAM bank to read
  seq_l1a_adr,          //0x74 Sequencer-L1A configuration
  tmb_trig_adr,         //0x86 TMB trigger configuration/MPC accept, delays
  vme_ddd1_adr,         //0x18 delays: CFEB0, DCC, MPC, TMB1
  vme_ddd2_adr,         //0x1a delays: CFEB4, CFEB3, CFEB2, CFEB1
  vme_ddd0_adr,         //0x16 delays: RAT/TMBrx, DMBtx, ALCTrx, ALCTtx
  rat_3d_delays_adr,    //0xE6 delays: RPC0/RATrx, RPC1/RATrx
  cfeb_inj_adr,         //0x42 enable CFEB inputs, RAM read/write
  seq_id_adr,           //0x6E board, csc ID 
  vme_usr_jtag_adr,     //0x10 ALCT JTAG address
  alct_inj_adr,         //0x32 ALCT Injector Control
  seq_trig_dly2_adr,    //0x6A Sequencer Trigger source delays
  seqmod_adr            //0xAC Sequencer Trigger modifiers
};
//
// bits within addresses which are allowed to be set in configuration prom
// N.B. One-to-one correspondance with addresses, above...
const int allowed_configuration_mask[number_of_allowed_configuration_addresses] = {
  0x01ff,               //0xb6:  disable sync-mode, read-only bits 
  0x0002,               //0x1e:  enable only add 1/2-cycle to RPC latching
  0x000c,               //0x0e:  enable only ALCT LVDS rx/tx
  0xff7f,               //0x2c:  disable unassigned 
  0x1fff,               //0x72:  disable unassigned
  0x83ff,               //0x68:  disable individual CFEB control from this register
  //                             -> See TMB documentation first before controlling CFEBs through this register...
  0x0fff,               //0xb2:  disable unassigned
  0xffff,               //0x76:
  0xffff,               //0x70:
  0xff3f,               //0x98:  disable read-only bits
  0x0fff,               //0x74:  disable generate internal L1, read-only bits
  0x67ff,               //0x86:  disable unassigned, read-only bits
  0xffff,               //0x18:
  0xffff,               //0x1a:
  0xffff,               //0x16:
  0x00ff,               //0xE6:  disable unused bits (RPC2/RAT, RPC3/RAT)
  0xffff,               //0x42:
  0x1fff,               //0x6E:  disable unassigned bits
  0x007f,               //0x10:  disable TDO, unassigned bits
  0x0001,               //0x32:  disable injector control bits
  0xffff,               //0x6A:
  0x01df                //0xAC:  disable random LCT enable, pulses outside of L1a window, scintillator functions
};
//
// TMB trigger modes:
const int CLCT_trigger                    =  1;
const int ALCT_trigger                    =  2;
const int Scintillator_trigger            =  3;
const int DMB_trigger                     =  4;
const int ALCT_CLCT_coincidence_trigger   =  5;
//
// Bit mappings for VME registers:
const int rpc_exists_vmereg            =  rpc_cfg_adr;
const int rpc_exists_bitlo             =  0;
const int rpc_exists_bithi             =  3;
const int rpc_exists_default           =  0xF;
//
const int rpc_read_enable_vmereg       =  rpc_cfg_adr;
const int rpc_read_enable_bitlo        =  4;
const int rpc_read_enable_bithi        =  4;
const int rpc_read_enable_default      =  1;
//
const int rpc_bxn_offset_vmereg        =  rpc_cfg_adr;
const int rpc_bxn_offset_bitlo         =  5;
const int rpc_bxn_offset_bithi         =  8;
const int rpc_bxn_offset_default       =  0;
//
//
const int shift_rpc_vmereg             =  vme_ratctrl_adr;
const int shift_rpc_bitlo              =  1;
const int shift_rpc_bithi              =  1;
const int shift_rpc_default            =  0;
//
//
const int enable_alct_rx_vmereg        =  vme_loopbk_adr;
const int enable_alct_rx_bitlo         =  2;
const int enable_alct_rx_bithi         =  2;
const int enable_alct_rx_default       =  1;
//
const int enable_alct_tx_vmereg        =  vme_loopbk_adr;
const int enable_alct_tx_bitlo         =  3;
const int enable_alct_tx_bithi         =  3;
const int enable_alct_tx_default       =  1;
//
//
const int alct_ext_trig_l1aen_vmereg   =  ccb_trig_adr;
const int alct_ext_trig_l1aen_bitlo    =  0;
const int alct_ext_trig_l1aen_bithi    =  0;
const int alct_ext_trig_l1aen_default  =  0;
//
const int clct_ext_trig_l1aen_vmereg   =  ccb_trig_adr;
const int clct_ext_trig_l1aen_bitlo    =  1;
const int clct_ext_trig_l1aen_bithi    =  1;
const int clct_ext_trig_l1aen_default  =  0;
//
const int request_l1a_vmereg           =  ccb_trig_adr;
const int request_l1a_bitlo            =  2;
const int request_l1a_bithi            =  2;
const int request_l1a_default          =  1;
//
const int alct_ext_trig_vme_vmereg     =  ccb_trig_adr;
const int alct_ext_trig_vme_bitlo      =  3;
const int alct_ext_trig_vme_bithi      =  3;
const int alct_ext_trig_vme_default    =  0;
//
const int clct_ext_trig_vme_vmereg     =  ccb_trig_adr;
const int clct_ext_trig_vme_bitlo      =  4;
const int clct_ext_trig_vme_bithi      =  4;
const int clct_ext_trig_vme_default    =  0;
//
const int ext_trig_both_vmereg         =  ccb_trig_adr;
const int ext_trig_both_bitlo          =  5;
const int ext_trig_both_bithi          =  5;
const int ext_trig_both_default        =  0;
//
const int ccb_allow_bypass_vmereg      =  ccb_trig_adr;
const int ccb_allow_bypass_bitlo       =  6;
const int ccb_allow_bypass_bithi       =  6;
const int ccb_allow_bypass_default     =  0;
//
const int internal_l1a_delay_vme_vmereg=  ccb_trig_adr;
const int internal_l1a_delay_vme_bitlo =  8;
const int internal_l1a_delay_vme_bithi =  15;
const int internal_l1a_delay_vme_default= 0x72;
//
//
const int fifo_mode_vmereg             =  seq_fifo_adr;
const int fifo_mode_bitlo              =  0;
const int fifo_mode_bithi              =  2;
const int fifo_mode_default            =  1;
//
const int fifo_tbins_vmereg            =  seq_fifo_adr;
const int fifo_tbins_bitlo             =  3;
const int fifo_tbins_bithi             =  7;
const int fifo_tbins_default           =  7;
//
const int fifo_pretrig_vmereg          =  seq_fifo_adr;
const int fifo_pretrig_bitlo           =  8;
const int fifo_pretrig_bithi           =  12;
const int fifo_pretrig_default         =  2;
//
//
const int clct_pat_trig_en_vmereg      =  seq_trig_en_adr;
const int clct_pat_trig_en_bitlo       =  0;
const int clct_pat_trig_en_bithi       =  0;
const int clct_pat_trig_en_default     =  1;
//
const int alct_pat_trig_en_vmereg      =  seq_trig_en_adr;
const int alct_pat_trig_en_bitlo       =  1;
const int alct_pat_trig_en_bithi       =  1;
const int alct_pat_trig_en_default     =  0;
//
const int match_pat_trig_en_vmereg     =  seq_trig_en_adr;
const int match_pat_trig_en_bitlo      =  2;
const int match_pat_trig_en_bithi      =  2;
const int match_pat_trig_en_default    =  0;
//
const int adb_ext_trig_en_vmereg       =  seq_trig_en_adr;
const int adb_ext_trig_en_bitlo        =  3;
const int adb_ext_trig_en_bithi        =  3;
const int adb_ext_trig_en_default      =  0;
//
const int dmb_ext_trig_en_vmereg       =  seq_trig_en_adr;
const int dmb_ext_trig_en_bitlo        =  4;
const int dmb_ext_trig_en_bithi        =  4;
const int dmb_ext_trig_en_default      =  0;
//
const int clct_ext_trig_en_vmereg      =  seq_trig_en_adr;
const int clct_ext_trig_en_bitlo       =  5;
const int clct_ext_trig_en_bithi       =  5;
const int clct_ext_trig_en_default     =  0;
//
const int alct_ext_trig_en_vmereg      =  seq_trig_en_adr;
const int alct_ext_trig_en_bitlo       =  6;
const int alct_ext_trig_en_bithi       =  6;
const int alct_ext_trig_en_default     =  0;
//
const int vme_ext_trig_en_vmereg       =  seq_trig_en_adr;
const int vme_ext_trig_en_bitlo        =  7;
const int vme_ext_trig_en_bithi        =  7;
const int vme_ext_trig_en_default      =  0;
//
const int ext_trig_inject_vmereg       =  seq_trig_en_adr;
const int ext_trig_inject_bitlo        =  8;
const int ext_trig_inject_bithi        =  8;
const int ext_trig_inject_default      =  0;
//
const int all_cfeb_active_vmereg       =  seq_trig_en_adr;
const int all_cfeb_active_bitlo        =  9;
const int all_cfeb_active_bithi        =  9;
const int all_cfeb_active_default      =  0;
//
const int cfebs_enabled_vmereg         =  seq_trig_en_adr;
const int cfebs_enabled_bitlo          =  10;
const int cfebs_enabled_bithi          =  14;
const int cfebs_enabled_default        =  0x1f;
//
const int cfeb_enable_source_vmereg    =  seq_trig_en_adr;
const int cfeb_enable_source_bitlo     =  15;
const int cfeb_enable_source_bithi     =  15;
const int cfeb_enable_source_default   =  1;
//
//
const int alct_vpf_delay_vmereg        =  tmbtim_adr;
const int alct_vpf_delay_bitlo         =  0;
const int alct_vpf_delay_bithi         =  3;
const int alct_vpf_delay_default       =  1;
//
const int alct_match_window_size_vmereg=  tmbtim_adr;
const int alct_match_window_size_bitlo =  4;
const int alct_match_window_size_bithi =  7;
const int alct_match_window_size_default= 3;
//
const int mpc_tx_delay_vmereg          =  tmbtim_adr;
const int mpc_tx_delay_bitlo           =  8;
const int mpc_tx_delay_bithi           =  11;
const int mpc_tx_delay_default         =  0;
//
//
const int l1a_offset_vmereg            =  seq_offset_adr;
const int l1a_offset_bitlo             =  0;
const int l1a_offset_bithi             =  3;
const int l1a_offset_default           =  0;
//
const int bxn_offset_vmereg            =  seq_offset_adr;
const int bxn_offset_bitlo             =  4;
const int bxn_offset_bithi             =  15;
const int bxn_offset_default           =  0;
//
//
const int triad_persist_vmereg         =  seq_clct_adr;
const int triad_persist_bitlo          =  0;
const int triad_persist_bithi          =  3;
const int triad_persist_default        =  5;
//
const int hs_pretrig_thresh_vmereg     =  seq_clct_adr;
const int hs_pretrig_thresh_bitlo      =  4;
const int hs_pretrig_thresh_bithi      =  6;
const int hs_pretrig_thresh_default    =  4;
//
const int ds_pretrig_thresh_vmereg     =  seq_clct_adr;
const int ds_pretrig_thresh_bitlo      =  7;
const int ds_pretrig_thresh_bithi      =  9;
const int ds_pretrig_thresh_default    =  4;
//
const int min_hits_pattern_vmereg      =  seq_clct_adr;
const int min_hits_pattern_bitlo       =  10;
const int min_hits_pattern_bithi       =  12;
const int min_hits_pattern_default     =  4;
//
const int drift_delay_vmereg           =  seq_clct_adr;
const int drift_delay_bitlo            =  13;
const int drift_delay_bithi            =  14;
const int drift_delay_default          =  2;
//
const int pretrigger_halt_vmereg       =  seq_clct_adr;
const int pretrigger_halt_bitlo        =  15;
const int pretrigger_halt_bithi        =  15;
const int pretrigger_halt_default      =  0;
//
//
const int seq_readmode_in_dmb_vmereg   =  scp_ctrl_adr;
const int seq_readmode_in_dmb_bitlo    =  5;
const int seq_readmode_in_dmb_bithi    =  5;
const int seq_readmode_in_dmb_default  =  0;
//
//
const int l1adelay_vmereg              =  seq_l1a_adr;
const int l1adelay_bitlo               =  0;
const int l1adelay_bithi               =  7;
const int l1adelay_default             =  128;
//
const int l1a_window_size_vmereg       =  seq_l1a_adr;
const int l1a_window_size_bitlo        =  8;
const int l1a_window_size_bithi        =  11;
const int l1a_window_size_default      =  3;
//
//
const int tmb_sync_err_enable_vmereg   =  tmb_trig_adr;
const int tmb_sync_err_enable_bitlo    =  0;
const int tmb_sync_err_enable_bithi    =  1;
const int tmb_sync_err_enable_default  =  0x3;
//
const int tmb_allow_clct_vmereg        =  tmb_trig_adr;
const int tmb_allow_clct_bitlo         =  3;
const int tmb_allow_clct_bithi         =  3;
const int tmb_allow_clct_default       =  1;
//
const int tmb_allow_match_vmereg       =  tmb_trig_adr;
const int tmb_allow_match_bitlo        =  4;
const int tmb_allow_match_bithi        =  4;
const int tmb_allow_match_default      =  1;
//
const int mpc_delay_vmereg             =  tmb_trig_adr;
const int mpc_delay_bitlo              =  5;
const int mpc_delay_bithi              =  8;
const int mpc_delay_default            =  7;
//
const int mpc_sel_ttc_bx0_vmereg       =  tmb_trig_adr;
const int mpc_sel_ttc_bx0_bitlo        =  13;
const int mpc_sel_ttc_bx0_bithi        =  13;
const int mpc_sel_ttc_bx0_default      =  1;
//
const int mpc_idle_blank_vmereg        =  tmb_trig_adr;
const int mpc_idle_blank_bitlo         =  14;
const int mpc_idle_blank_bithi         =  14;
const int mpc_idle_blank_default       =  0;
//
//
const int tmb1_phase_vmereg            =  vme_ddd1_adr;
const int tmb1_phase_bitlo             =  0;
const int tmb1_phase_bithi             =  3;
const int tmb1_phase_default           =  0;
//
const int mpc_phase_vmereg             =  vme_ddd1_adr;
const int mpc_phase_bitlo              =  4;
const int mpc_phase_bithi              =  7;
const int mpc_phase_default            =  0;
//
const int dcc_phase_vmereg             =  vme_ddd1_adr;
const int dcc_phase_bitlo              =  8;
const int dcc_phase_bithi              =  11;
const int dcc_phase_default            =  0;
//
const int cfeb0delay_vmereg            =  vme_ddd1_adr;
const int cfeb0delay_bitlo             =  12;
const int cfeb0delay_bithi             =  15;
const int cfeb0delay_default           =  7;
//
//
const int cfeb1delay_vmereg            =  vme_ddd2_adr;
const int cfeb1delay_bitlo             =  0;
const int cfeb1delay_bithi             =  3;
const int cfeb1delay_default           =  7;
//
const int cfeb2delay_vmereg            =  vme_ddd2_adr;
const int cfeb2delay_bitlo             =  4;
const int cfeb2delay_bithi             =  7;
const int cfeb2delay_default           =  7;
//
const int cfeb3delay_vmereg            =  vme_ddd2_adr;
const int cfeb3delay_bitlo             =  8;
const int cfeb3delay_bithi             =  11;
const int cfeb3delay_default           =  7;
//
const int cfeb4delay_vmereg            =  vme_ddd2_adr;
const int cfeb4delay_bitlo             =  12;
const int cfeb4delay_bithi             =  15;
const int cfeb4delay_default           =  7;
//
//
const int alct_tx_clock_delay_vmereg   =  vme_ddd0_adr;
const int alct_tx_clock_delay_bitlo    =  0;
const int alct_tx_clock_delay_bithi    =  3;
const int alct_tx_clock_delay_default  =  8;
//
const int alct_rx_clock_delay_vmereg   =  vme_ddd0_adr;
const int alct_rx_clock_delay_bitlo    =  4;
const int alct_rx_clock_delay_bithi    =  7;
const int alct_rx_clock_delay_default  =  1;
//
const int dmb_tx_delay_vmereg          =  vme_ddd0_adr;
const int dmb_tx_delay_bitlo           =  8;
const int dmb_tx_delay_bithi           =  11;
const int dmb_tx_delay_default         =  2;
//
const int rat_tmb_delay_vmereg         =  vme_ddd0_adr;
const int rat_tmb_delay_bitlo          =  12;
const int rat_tmb_delay_bithi          =  15;
const int rat_tmb_delay_default        =  0;
//
//
const int rpc0_rat_delay_vmereg        =  rat_3d_delays_adr;
const int rpc0_rat_delay_bitlo         =  0;
const int rpc0_rat_delay_bithi         =  3;
const int rpc0_rat_delay_default       =  3;
//
const int rpc1_rat_delay_vmereg        =  rat_3d_delays_adr;
const int rpc1_rat_delay_bitlo         =  4;
const int rpc1_rat_delay_bithi         =  7;
const int rpc1_rat_delay_default       =  3;
//
const int rpc2_rat_delay_vmereg        =  rat_3d_delays_adr;
const int rpc2_rat_delay_bitlo         =  8;
const int rpc2_rat_delay_bithi         =  11;
const int rpc2_rat_delay_default       =  0;
//
const int rpc3_rat_delay_vmereg        =  rat_3d_delays_adr;
const int rpc3_rat_delay_bitlo         =  12;
const int rpc3_rat_delay_bithi         =  15;
const int rpc3_rat_delay_default       =  0;
//
//
const int enableCLCTInputs_vmereg      =  cfeb_inj_adr;
const int enableCLCTInputs_bitlo       =  0;
const int enableCLCTInputs_bithi       =  4;
const int enableCLCTInputs_default     =  0x1f;
//
const int cfeb_ram_sel_vmereg          =  cfeb_inj_adr;
const int cfeb_ram_sel_bitlo           =  5;
const int cfeb_ram_sel_bithi           =  9;
const int cfeb_ram_sel_default         =  0;
//
const int cfeb_inj_en_sel_vmereg       =  cfeb_inj_adr;
const int cfeb_inj_en_sel_bitlo        =  10;
const int cfeb_inj_en_sel_bithi        =  14;
const int cfeb_inj_en_sel_default      =  0x1f;
//
const int start_pattern_inj_vmereg     =  cfeb_inj_adr;
const int start_pattern_inj_bitlo      =  15;
const int start_pattern_inj_bithi      =  15;
const int start_pattern_inj_default    =  0;
//
//
const int tmb_slot_vmereg              =  seq_id_adr;
const int tmb_slot_bitlo               =  0;
const int tmb_slot_bithi               =  4;
const int tmb_slot_default             =  21;
//
const int csc_id_vmereg                =  seq_id_adr;
const int csc_id_bitlo                 =  5;
const int csc_id_bithi                 =  8;
const int csc_id_default               =  5;
//
const int run_id_vmereg                =  seq_id_adr;
const int run_id_bitlo                 =  9;
const int run_id_bithi                 =  12;
const int run_id_default               =  0;
//
//
const int alct_clear_vmereg            =  alct_inj_adr;
const int alct_clear_bitlo             =  0;
const int alct_clear_bithi             =  0;
const int alct_clear_default           =  0;
//
//
const int alct_trig_width_vmereg       =  seq_trig_dly2_adr;
const int alct_trig_width_bitlo        =  0;
const int alct_trig_width_bithi        =  3;
const int alct_trig_width_default      =  3;
//
const int alct_pretrig_delay_vmereg    =  seq_trig_dly2_adr;
const int alct_pretrig_delay_bitlo     =  4;
const int alct_pretrig_delay_bithi     =  7;
const int alct_pretrig_delay_default   =  0;
//
const int alct_pattern_delay_vmereg    =  seq_trig_dly2_adr;
const int alct_pattern_delay_bitlo     =  8;
const int alct_pattern_delay_bithi     =  11;
const int alct_pattern_delay_default   =  0;
//
const int adb_ext_trig_delay_vmereg    =  seq_trig_dly2_adr;
const int adb_ext_trig_delay_bitlo     =  12;
const int adb_ext_trig_delay_bithi     =  15;
const int adb_ext_trig_delay_default   =  1;
//
//
const int clct_flush_delay_vmereg      =  seqmod_adr;
const int clct_flush_delay_bitlo       =  0;
const int clct_flush_delay_bithi       =  3;
const int clct_flush_delay_default     =  1;
//
const int clct_turbo_vmereg            =  seqmod_adr;
const int clct_turbo_bitlo             =  4;
const int clct_turbo_bithi             =  4;
const int clct_turbo_default           =  0;
//
const int ranlct_enable_vmereg         =  seqmod_adr;
const int ranlct_enable_bitlo          =  5;
const int ranlct_enable_bithi          =  5;
const int ranlct_enable_default        =  0;
//
const int wrt_buf_required_vmereg      =  seqmod_adr;
const int wrt_buf_required_bitlo       =  6;
const int wrt_buf_required_bithi       =  6;
const int wrt_buf_required_default     =  1;
//
const int valid_clct_required_vmereg   =  seqmod_adr;
const int valid_clct_required_bitlo    =  7;
const int valid_clct_required_bithi    =  7;
const int valid_clct_required_default  =  1;
//
const int l1a_allow_match_vmereg       =  seqmod_adr;
const int l1a_allow_match_bitlo        =  8;
const int l1a_allow_match_bithi        =  8;
const int l1a_allow_match_default      =  1;
//
const int l1a_allow_notmb_vmereg       =  seqmod_adr;
const int l1a_allow_notmb_bitlo        =  9;
const int l1a_allow_notmb_bithi        =  9;
const int l1a_allow_notmb_default      =  0;
//
const int l1a_allow_nol1a_vmereg       =  seqmod_adr;
const int l1a_allow_nol1a_bitlo        =  10;
const int l1a_allow_nol1a_bithi        =  10;
const int l1a_allow_nol1a_default      =  0;
//
const int l1a_allow_alct_only_vmereg   =  seqmod_adr;
const int l1a_allow_alct_only_bitlo    =  11;
const int l1a_allow_alct_only_bithi    =  11;
const int l1a_allow_alct_only_default  =  0;
//
const int scint_veto_clr_vmereg        =  seqmod_adr;
const int scint_veto_clr_bitlo         =  12;
const int scint_veto_clr_bithi         =  12;
const int scint_veto_clr_default       =  0;
//
//



