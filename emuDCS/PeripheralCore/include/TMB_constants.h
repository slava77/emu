//-----------------------------------------------------------------------
// $Id: TMB_constants.h,v 2.1 2005/09/06 12:12:28 mey Exp $
// $Log: TMB_constants.h,v $
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

static const unsigned long int	vme_phos4sm_adr	   = 0x000014;
static const unsigned long int	vme_phos4a_adr	   = 0x000016;
static const unsigned long int	vme_phos4b_adr	   = 0x000018;
static const unsigned long int	vme_phos4c_adr	   = 0x00001A;
static const unsigned long int	vme_phos4d_adr	   = 0x00001C;
static const unsigned long int	vme_phos4e_adr	   = 0x00001E;

static const unsigned long int	vme_step_adr	   = 0x000020;
static const unsigned long int	vme_led_adr	   = 0x000022;
static const unsigned long int	vme_adc_adr	   = 0x000024;
static const unsigned long int	vme_dsn_adr	   = 0x000026;

static const unsigned long int	mod_cfg_adr	   = 0x000028;	//For tmb2001a
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
static const unsigned long int  cnt_ctrl_adr       = 0x0000D0;
static const unsigned long int  cnt_rdata_adr      = 0x0000D2;
// this comes from alct_routines2/tmb_vme_reg.h
#define TMB_ADR_BOOT            0x70000 // Hardware Bootstrap Register

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

