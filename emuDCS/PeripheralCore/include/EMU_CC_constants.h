/* Time outs */
#define RD_TMO_short   200
#define RD_TMO_med    1000
#define RD_TMO_long   3500
#define RD_TMO_xlong 22000

/*
 * Byte offsets into packet
 */
#define MAC_DEST_OFF 0  //Destination MAC Address (6 bytes)
#define MAC_SRC_OFF  6  //Source MAC Address (6 bytes)
#define PKT_LEN_OFF  12 //Packet Length (2 bytes, high order first)
#define HDR_OFF      14 //Header (4 words)
#define PKT_TYP_OFF  15 //Packet type (1 byte)
#define FRAG_NUM_OFF 16 //Packet fragment number (2 words, high order first)
#define PROC_TAG_OFF 16 //Process Tag (5 bits)
#define CMND_ACK_OFF 17 //Command Acknowledged (1 byte)
#define SEQ_PKT_OFF  18 //Sequencial packet ID. (1 word, high order byte first)
#define WRD_CNT_OFF  20 //Word count (13-bits)
#define DATA_OFF     22 //Data offset
#define MSG_OFF      22 //Message offset
#define MAX_DATA_JMB 8994 //Maximum payload for Jumbo packets
#define MAX_DATA_PKT 1494 //Maximum payload for Normal packets




// The following macros assume a base pointer to the packet buffer

/*
 * Header flags and Acknowledge Status in rbuf[HDR_OFF]
 */
#define WITH_DATA(b) ((b[HDR_OFF]>>3)&0x1)
#define AK_STATUS(b) (b[HDR_OFF]&0x7)
#define PKT_FLAGS(b) ((b[HDR_OFF]>>4)&0xF)
#define PROC_TAG(b)  (b[PROC_TAG_OFF]&0x1F)

enum ack_st {NO_ACK =0, CC_S, CC_W, CC_E, CE_I, CIP, CIP_W, CIP_E};
enum pkt_fl {SPONT = 1, FRAG = 2, NEW = 4, PRIO = 8};

union hdr_stat {
  unsigned char full;
  struct {
    unsigned int ackn : 4; //Acknowledge/Status field
    unsigned int spnt : 1; //Spontaneous packet flag
    unsigned int frag : 1; //Packet fragment flag
    unsigned int newp  : 1; //New packet flag (1st of a series of fragments)
    unsigned int prio : 1; //Priority packet flag
  } tg; // tag
};

/*
 * Return Packet types in rbuf[PKT_TYP_OFF] (8-bits)
 */
enum pkt_t {NONE_EXP_PKT = -1,
	    NO_DATA_PKT, LPBCK_PKT, TXNWRDS_PKT, EXFIFO_PKT,
            VMED08_PKT, VMED16_PKT, VMED32_PKT, VMED64_PKT,
            JTAG_MOD_PKT, CNFG_MOD_PKT = 10, FLSH_RBK_PKT, ETH_NET_PKT = 16,
            IHD08_STAT_ID_PKT = 0XF8, IHD16_STAT_ID_PKT, IHD32_STAT_ID_PKT,
            INFO_PKT = 0XFD, WARN_PKT, ERR_PKT};

/*
 * Packet Fragment Number is 4 bytes starting at rbuf[FRAG_NUM_OFF] (32-bits)
 */
#define FRAG_NUM(b) (((b[FRAG_NUM_OFF  ]&0xFF)<<24)| \
                     ((b[FRAG_NUM_OFF+1]&0xFF)<<16)| \
                     ((b[FRAG_NUM_OFF+2]&0xFF)<<8) | \
                      (b[FRAG_NUM_OFF+3]&0xFF))

/*
 * Sequencial Packet ID is 2 bytes starting at rbuf[SEQ_PKT_OFF] (16-bits)
 */
#define SEQ_PKT_ID(b) (((b[SEQ_PKT_OFF]&0xff)<<8)|(b[SEQ_PKT_OFF+1]&0xff))

/*
 * Word Count of data returned at rbuf[WRD_CNT_OFF] (13-bits)
 */
#define DAT_WRD_CNT(b) (((b[WRD_CNT_OFF]&0x1f)<<8)|(b[WRD_CNT_OFF+1]&0xff))



/*
 * For Error, Warning, and Info packet types, the source of the
 * information is given by the Source ID in rbuf[MSG_OFF]>>4 (4-bits)
 * The Message Type is given by bits 2 & 3 of rbuf[MSG_OFF]
 * The Message Universal Code Word is bits 0-9 of the word at MSG_OFF.
 */

#define SOURCE_ID(b)    ((b[MSG_OFF]>>4)&0xF)
#define MSG_TYP_ID(b)   ((b[MSG_OFF]>>2)&0x3)
#define UNV_CODE_WRD(b) (((b[MSG_OFF]&0x3)<<8)|(b[MSG_OFF+1]&0xFF))

enum src_id {MISC = 0, VME_CTRL, VME_MSTR, VME_RDBK, VME_IH, VME_SLV,
             VME_ARB, EXFIFO_MOD, ETH_RCV, ETH_TRNS, JTAG_MOD, FLASH_MOD,
             CNFG_MOD, CP_MOD, RST_HNDLR, STRTUP_SHTDWN};
enum typ_id {HDR_INFO = 0, HDR_WARN, HDR_ERROR, HDR_NT_ALLWD};


/*
 * VME Control module Errors/Warnings/Information
 * Source ID = VME_CTRL
 */

// VME Control module Errors
#define EXTRACT_CTRL_TT(b)   (b[MSG_OFF+3]&3)         //2nd Error word Transfer Type
#define EXTRACT_CTRL_DSZ(b)  ((b[MSG_OFF+3]>>2)&3)    //2nd Error word Data Size
#define EXTRACT_CTRL_WRT(b)  ((b[MSG_OFF+3]>>4)&1)    //2nd Error word Write/Read_Bar
#define EXTRACT_CTRL_ASZ(b)  ((b[MSG_OFF+3]>>5)&7)    //2nd Error word Addr. Size
#define EXTRACT_CTRL_DT(b)   (b[MSG_OFF+2]&7)         //2nd Error word Delay Type
#define EXTRACT_CTRL_AT(b)   ((b[MSG_OFF+2]>>3)&0x1F) //2nd Error word Access Types
#define SECND_WRD(b)   (((b[MSG_OFF+2]&0xFF)<<8)|(b[MSG_OFF+3]&0xFF)) //2nd Error word
union ctrl_stat {           // for 2nd Error word.
  unsigned short int full;
  struct {
    unsigned int tt  : 2; //Transfer Type (2 bit field)
    unsigned int dsz : 2; //Data Size     (2 bit field)
    unsigned int wrt : 1; //Write or Read (1 bit field)
    unsigned int asz : 3; //Address Size  (3 bit field)
    unsigned int dt  : 3; //Delay Type    (3 bit field)
    unsigned int at  : 5; //all "access type" bits (5 bit field)
  } tg; // tag
  struct {
    unsigned int     :11; // place holder
    unsigned int pg  : 1; //Program/Data access
    unsigned int sp  : 1; //Supervisory/Non-Privileged access
    unsigned int lk  : 1; // LOCK Command access
    unsigned int cr  : 1; // Configuration ROM/ Control/Status Reg.
    unsigned int ud  : 1; //User Defined Address Modifier
  } fl; // flag
};


/*
 * VME Master Errors/Warnings/Information
 * Source ID = VME_MSTR
 */

// VME Master Errors
#define EXTRACT_MSTR_TT(b)   (b[MSG_OFF+3]&3)         //2nd Error word Transfer Type
#define EXTRACT_MSTR_DSZ(b)  ((b[MSG_OFF+3]>>2)&3)    //2nd Error word Data Size
#define EXTRACT_MSTR_WRT(b)  ((b[MSG_OFF+3]>>4)&1)    //2nd Error word Write/Read_Bar
#define EXTRACT_MSTR_AM(b)  (((b[MSG_OFF+2]&7)<<3)|((b[MSG_OFF+3]>>5)&7)) //2nd Error word Addr. Mod.
#define EXTRACT_MSTR_STATE(b) ((b[MSG_OFF+2]>>3)&0x1F)
union mstr_stat {           // for 2nd Error word.
  unsigned short int full;
  struct {
    unsigned int tt    : 2;
    unsigned int dsz   : 2;
    unsigned int wrt   : 1;
    unsigned int am    : 6;
    unsigned int state : 5;
  } tg; // tag
};
//VME address when error occured is 3rd thru 6th word (64 bit).


/*
 * VME Readback module Errors/Warnings/Information
 * Source ID = VME_RDBK
 */
// VME Readback module Errors
// VME Readback module Warnings
// No second words

/*
 * VME Interrupt Handler module Errors/Warnings/Information
 * Source ID = VME_IH
 */

// VME Interrupt Handler module Errors
#define EXTRACT_IH_IRQ(b)   (b[MSG_OFF+3]&0x7F)     //2nd Error word
#define EXTRACT_IH_ADD(b)   (b[MSG_OFF+2]&7)        //2nd Error word
#define EXTRACT_IH_STATE(b) ((b[MSG_OFF+2]>>4)&0xF) //2nd Error word


// VME Interrupt Handler module Warnings
#define EXTRACT_IRQ_MSK(b)   (b[MSG_OFF+3]&0x7F)



/*
 * External FIFO  Errors/Warnings/Information
 * Source ID = EXFIFO_MOD
 */
// No second words

/*
 * Control Processors Errors/Warnings/Information
 * Source ID = CP_MOD
 */

// Bus Traffic Controller Errors
// No second words


/*
 * Startup/Shutdown Errors/Warnings/Information
 * Source ID = STRTUP_SHTDWN
 */

// Startup/Shutdown Warnings
// No second words

// Startup/Shutdown Information
// No second words


/*
 *Structure for packet response statistics
 */

struct rspn_t {
  int npkt;
  int nspt;
  int nerr;
  int nberr;
  int nbto;
  int nwrn;
  int ninf;
  int nintr;
  int ndat;
  int nack_only;
  int nbad;
  int buf_cnt;
  int buf_typ;
  int totwrds;
  int ackn;
};

// Codes

struct ucw {
  int code;
  char *type;
  char *mnem;
  char *msg;
};
struct ak_st {
  char *mnem;
  char *status;
};
struct ucw ucwtab[] = {
  {0x000, "INF", "G_No_Info",       "No information."},
  {0x001, "ERR", "CP_Un_Asgn",      "Command has not been assigned to a module."},
  {0x002, "ERR", "CP_Not_Def",      "Command is not defined in this module."},
  {0x003, "ERR", "CP_No_Data",      "Expected data was not present."},
  {0x004, "ERR", "CP_Not_Exec",     "Command was not executed."},
  {0x100, "ERR", "VD_Dat_WtErr",    "VME Direct Data FIFO Write error\n\t\t\t\t"
                                    "(written to while full)."},
  {0x101, "WRN", "VD_Dat_AF",       "VME Direct Data FIFO Almost Full\n\t\t\t\t"
                                    "(stop sending VME Direct Commands)."},
  {0x102, "ERR", "VD_Hdr_WtErr",    "VME Direct MAC/Hdr FIFO Write error\n\t\t\t\t"
                                    "(written to while full)."},
  {0x103, "WRN", "VD_Hdr_AF",       "VME Direct MAC/Hdr FIFO Almost Full\n\t\t\t\t"
                                    "(stop sending VME Direct Commands)."},
  {0x110, "ERR", "VC_Unkn_Addr",    "Unknown VME address."},
  {0x111, "ERR", "VC_Unkn_Dly",     "Unknown VME delay type."},
  {0x112, "ERR", "VC_Incomp_Opt",   "Incompatible options specified in VME cntrl word."},
  {0x113, "ERR", "VC_RdEr_Units",   "Read error -- number of VME units."},
  {0x114, "ERR", "VC_RdEr_CtrlWrd", "Read error -- VME control word."},
  {0x115, "ERR", "VC_RdEr_Addr",    "Read error -- VME address."},
  {0x116, "ERR", "VC_RdEr_Dcnt",    "Read error -- VME data count."},
  {0x117, "ERR", "VC_RdEr_Data",    "Read error -- VME data word."},
  {0x118, "ERR", "VC_MTEr_Fifo",    "FIFO Empty error\n\t\t\t\t"
                                    "(timed-out while waiting to read)."},
  {0x120, "ERR", "VM_BERR_Slv",     "VME Master: Bus error initiated by slave."},
  {0x121, "ERR", "VM_BTO",          "VME Master: Bus time out."},
  {0x122, "ERR", "VM_Not_Sup",      "VME Master: command is not supported."},
  {0x130, "ERR", "VR_Mis_SOP",      "VME Read Controller: Missing start of packet."},
  {0x131, "ERR", "VR_Wrng_Typ",     "VME Read Controller: Wrong packet type seen."},
  {0x132, "ERR", "VR_Rd_TMO",       "VME Read Controller: Timed-out waiting for data."},
  {0x140, "ERR", "VI_BERR_Slv",     "VME Intrpt Hndlr: Bus error initiated by slave."},
  {0x141, "ERR", "VI_BTO",          "VME Intrpt Hndlr: Bus time out."},
  {0x142, "WRN", "VI_Msk_Chg",      "IRQ mask has changed."},
  {0x161, "ERR", "VA_BGTO",         "VME Bus Grant time out"},
  {0x200, "ERR", "EF_Rd_Err",       "External FIFO went empty after read started."},
  {0x201, "ERR", "EF_MT_Err",       "External FIFO was empty when read was requested."},
  {0x202, "ERR", "EF_Rt_Err",       "External FIFO empty or Mark not set when\n\t\t\t\t"
                                    "retransmit read was requested."},
  {0x203, "ERR", "EF_Mk_Err",       "External FIFO attempt to set Mark when\n\t\t\t\t"
                                    "almost empty."},
  {0x204, "ERR", "EF_Wrt_Err",      "External FIFO went full after\n\t\t\t\t"
                                    "write sequence started."},
  {0x205, "WRN", "EF_FF_PAF",       "External FIFO is almost full\n\t\t\t\t"
                                    "(stop sending FIFO or VME commands)."},
  {0x206, "WRN", "EF_V_wrt_Wrn",    "VME command received while in FIFO TEST mode\n\t\t\t\t"
                                    "(data written to FIFO)."},
  {0x207, "ERR", "EF_Rd_V_Err",     "FIFO read command received while inn VME mode\n\t\t\t\t"
                                    "(no data read from FIFO)."},
  {0x208, "ERR", "EF_Mltp_Err",     "Multiple FIFO errors."},
  {0x209, "WRN", "EF_Wrt_Wrn",      "Write command received while in VME mode\n\t\t\t\t"
                                    "(data written to FIFO)."},
  {0x210, "ERR", "ER_Rcv_Err",      "Ethernet receive FIFO went empty before\n\t\t\t\t"
                                    "all packet data was read."},
  {0x230, "WRN", "JT_Buf_AF",       "JTAG buffer is almost full\n\t\t\t\t"
                                    "(stop sending JTAG commands)."},
  {0x231, "ERR", "JT_Buf_Ovfl",     "JTAG buffer overflowed\n\t\t\t\t"
                                    "(some JTAG commands are lost)."},
  {0x232, "INF", "JT_Buf_AMT",      "JTAG buffer is almost MT\n\t\t\t\t"
                                    "(start sending JTAG commands)."},
  {0x233, "ERR", "JT_Buf_RdErr",    "JTAG Buffer Read error."},
  {0x234, "ERR", "JT_Unk_Cmd",      "Unknown JTAG Instruction in memory."},
  {0x235, "ERR", "JT_Ver_Fail",     "PROM Verification Failed."},
  {0x236, "ERR", "JT_Prg_Fail",     "PROM Programming Failed."},
  {0x240, "WRN", "FL_In_AF",        "Flash Control Input FIFO is almost full\n\t\t\t\t"
                                    "(stop sending Flash or Config commands)."},
  {0x241, "ERR", "FL_In_WtErr",     "Flash Control Input FIFO Write error\n\t\t\t\t"
                                    "(attempted write when full; some info lost)."},
  {0x242, "ERR", "FL_In_RdErr",     "Flash Control Input FIFO Read error\n\t\t\t\t"
                                    "(attempted read when empty; data invalid)."},
  {0x243, "ERR", "FL_TRDS_WtErr",   "Flash Control Total Reads FIFO Write error\n\t\t\t\t"
                                    "(attempted write when full; some info lost)."},
  {0x244, "ERR", "FL_TRDS_RdErr",   "Flash Control Total Reads FIFO Read error\n\t\t\t\t"
                                    "(attempted read when empty; data invalid)."},
  {0x245, "ERR", "FL_PgRd_WtErr",   "Flash Control Page/Read FIFO Write error\n\t\t\t\t"
                                    "(attempted write when full; some info lost)."},
  {0x246, "ERR", "FL_PgRd_RdErr",   "Flash Control Page/Read FIFO Read error\n\t\t\t\t"
                                    "(attempted read when empty; data invalid)."},
  {0x247, "ERR", "FL_ADFF_WtErr",   "Flash Control Address FIFO Write error\n\t\t\t\t"
                                    "(attempted write when full; some info lost)."},
  {0x248, "ERR", "FL_ADFF_RdErr",   "Flash Control Address FIFO Read error\n\t\t\t\t"
                                    "(attempted read when empty; data invalid)."},
  {0x250, "ERR", "CF_Mltp_Flsh",    "Multiple errors from Flash Controller\n\t\t\t\t"
                                    "(see following data word for error bits)."},
  {0x251, "ERR", "CF_Crptd_Dat",    "Corrupted data from Flash Controller\n\t\t\t\t"
                                    "(uncorrected bit errors)."},
  {0x252, "WRN", "CF_Bit_Errs",     "Bit errors in data from Flash Controller\n\t\t\t\t"
                                    "were found and corrected."},
  {0x260, "WRN", "RH_xxx_xxx",      ""}, 
  {0x270, "WRN", "SS_Rld_Pndg",     "Controller is shuting down to reload firmware."},
  {0x271, "INF", "SS_Sys_Up",       "Controller system is back up."}
};


struct ak_st ak_status[] = {
  {"NO_ACK", "No acknowledgement requested or not implemented."},
  {"CC_S", "Command completed successfully."},
  {"CC_W", "Command completed with warnings."},
  {"CC_E", "Command completed with errors."},
  {"CE_I", "Command execution finished incomplete."},
  {"CIP", "Command in progress no warnings no errors."},
  {"CIP_W", "Command in progress with warnings."},
  {"CIP_E", "Command in progress with errors."}
};

struct ucw foptab[] = {
  {0x00,"CNFG","Funct_NoOp","No Operation"},
  {0x01,"CNFG","Set_FF_Test","Set external FIFO interface to test mode"},
  {0x02,"CNFG","Set_FF_VME","Set external FIFO interface to VME mode"},
  {0x03,"CNFG","ECC_enable","Enable error corrceting code in FIFO"},
  {0x04,"CNFG","ECC_disable","Disable error corrceting code in FIFO"},
  {0x05,"CNFG","Save_Cnfg_Num","Save cur. config as specifed config num."},
  {0x06,"CNFG","Read_Cnfg_Num_Dir","Read config direct from FLASH"},
  {0x07,"CNFG","Read_Cnfg_Num_Dcd","Read decoded config from FLASH"},
  {0x08,"CNFG","Rstr_Cnfg_Num","Restore config to specified config num"},
  {0x09,"CNFG","Set_Cnfg_Dflt","Set the default config. number"},
  {0x0A,"CNFG","Read_Cnfg_Dflt","Read cur. default config. number"},
  {0x0B,"CNFG","Set_MACs","Set MAC addresses"},
  {0x0C,"CNFG","Read_MACs_Dir","Read MAC address dircetly from FLASH"},
  {0x0D,"CNFG","Read_MACs_Dcd","Read decoded MAC address from FLASH"},
  {0x0E,"CNFG","Read_CRs","Read cur config register settings"},
  {0x0F,"CNFG","Wrt_Eth_CR","Write Ethernet config register."},
  {0x10,"CNFG","Wrt_Ext_CR","Write External FIFO config register."},
  {0x11,"CNFG","Wrt_Rst_CR","Write Reset config register."},
  {0x12,"CNFG","Wrt_VME_CR","Write VME config register."},
  {0x13,"CNFG","Wrt_BTO_CR","Write Bus Time Out config register."},
  {0x14,"CNFG","Wrt_BGTO_CR","Write Bus Grant Time Out config register."},
  {0x15,"CNFG","Wrt_All_CRs","Write all config registers"},
  {0x16,"CNFG","Set_Clr_CRs","Set or Clear config registers"},
  {0x17,"CNFG","Set_Inj_Err","Turn on error injection"},
  {0x18,"CNFG","Rst_Inj_Err","Turn off error injection"},
  {0x19,"CNFG","Warn_On_Shdwn","Enable warning packet on shutdown"},
  {0x1A,"CNFG","No_Warn_On_Shdwn","Disable warning packet on shutdown"},
  {0x1B,"CNFG","Snd_Startup_Pkt","Enable startup packet after reload"},
  {0x1C,"CNFG","No_Startup_Pkt","Disable startup packet after reload"},
  {0x1D,"CNFG","Wrt_Ser_Num","Write serial number to FLASH"},
  {0x1E,"CNFG","Rd_Ser_Num","Read serial number from FLASH"},
  {0x1F,"CNFG","Wrt_CR_ID","Write config reg. by specifying ID"},
  {0x20,"VME","VME_Cmds","Packet of VME commands buffered in FIFO"},
  {0x22,"VME","VME_Dir_Cmds","Packet of VME commands direct to VME cntrl."},
  {0x40,"FLASH","Flash_R_W","Packet of FLASH read or write commands"},
  {0xE0,"EXFF","Wrt_Ext_FF","Write data to external FIFO"},
  {0xE1,"EXFF","Prg_Ext_Off","Program offsets in external FIFO"},
  {0xE2,"EXFF","Rdbk_Ext_Off","Readback offsets from external FIFO"},
  {0xE3,"EXFF","PRst_Ext_FF","Partial reset of external FIFO"},
  {0xE4,"EXFF","Rd_Ext_FF","Read data from external FIFO"},
  {0xE5,"EXFF","RT_Ext_FF","Retransmit data from external FIFO"},
  {0xE6,"EXFF","MRst_Ext_FF","Master reset of external FIFO"},
  {0xE7,"EXFF","ST_MK_Ext_FF","Set Mark in external FIFO"},
  {0xE8,"EXFF","RST_MK_Ext_FF","Reset Mark in external FIFO"},
  {0xE9,"EXFF","Rst_Ext_Err_Cnt","Reset FIFO error counter"},
  {0xEA,"EXFF","Rd_Ext_Err_Cnts","Read FIFO error counter"},
  {0xEF,"EXFF","Flush_2_BOD","Flush FIFO to Begining of Data"},
  {0xF0,"TST","Rst_Seq_ID","Reset Sequencial ID counter"},
  {0xF7,"SLVTST","Gen_Intr","Generate Interrupt on Slave test board"},
  {0xF8,"SLVTST","Gen_Hard_Rst","Generate Hard Reset on Slave test board"},
  {0xF9,"TST","Force_Reload","Force a Reload"},
  {0xFD,"TST","Send_N_Words","Return n words"},
  {0xFE,"TST","Load_User_Reg","Load User Reg (LEDs)"},
  {0xFF,"TST","Loopback","Retrun data to sender."}
};

struct ucw pkttab[] = {
  {0x00,"MISC","NO_DATA_PKT","No data with this packet"},
  {0x01,"DIAG","LPBCK_PKT","Loopback data"},
  {0x02,"DIAG","TXNWRDS_PKT","Transmit n-words data"},
  {0x03,"FIFO","EXFIFO_PKT","External FIFO data"},
  {0x04,"VME","VMED08_PKT","VME data bytes"},
  {0x05,"VME","VMED16_PKT","VME data words"},
  {0x06,"VME","VMED32_PKT","VME data long words"},
  {0x07,"VME","VMED64_PKT","VME data quad words"},
  {0x08,"JTAG","JTAG_MOD_PKT","JTAG module data packet"},
  {0x0A,"CNFG","CNFG_MOD_PKT","Config module data packet"},
  {0x0B,"FLSH","FLSH_RBK_PKT","FLASH module data packet"},
  {0x10,"ETHN","ETH_NET_PKT","Ethernet module data packet"},
  {0xF8,"INTR","IHD08_Stat_ID_PKT","Interrupt with 8-bit status word"},
  {0xF9,"INTR","IHD16_Stat_ID_PKT","Interrupt with 16-bit status word"},
  {0xFA,"INTR","IHD32_Stat_ID_PKT","Interrupt with 32-bit status word"},
  {0xFD,"TST","Info_PKT","Information packet"},
  {0xFE,"TST","Warn_PKT","Warning packet"},
  {0xFF,"TST","Err_PKT","Error packet"}
};

#define NERRCODES (sizeof ucwtab / sizeof(struct ucw))
#define NOPCODES (sizeof foptab / sizeof(struct ucw))
#define NPKTCODES (sizeof pkttab / sizeof(struct ucw))


int nerrcodes = NERRCODES;
int nopcodes = NOPCODES;
int npktcodes = NPKTCODES;


// Universal Message Code Words

#define G_No_Info       0x000
#define CP_Un_Asgn      0x001
#define CP_Not_Def      0x002
#define CP_No_Data      0x003
#define CP_Not_Exec     0x004
#define VD_Dat_WtErr    0x100
#define VD_Dat_AF       0x101
#define VD_Hdr_WtErr    0x102
#define VD_Hdr_AF       0x103
#define VC_Unkn_Addr    0x110
#define VC_Unkn_Dly     0x111
#define VC_Incomp_Opt   0x112
#define VC_RdEr_Units   0x113
#define VC_RdEr_CtrlWrd 0x114
#define VC_RdEr_Addr    0x115
#define VC_RdEr_Dcnt    0x116
#define VC_RdEr_Data    0x117
#define VC_MTEr_Fifo    0x118
#define VM_BERR_Slv     0x120
#define VM_BTO          0x121
#define VM_Not_Sup      0x122
#define VR_Mis_SOP      0x130
#define VR_Wrng_Typ     0x131
#define VR_Rd_TMO       0x132
#define VI_BERR_Slv     0x140
#define VI_BTO          0x141
#define VI_Msk_Chg      0x142
#define VA_BGTO         0x161
#define EF_Rd_Err       0x200
#define EF_MT_Err       0x201
#define EF_Rt_Err       0x202
#define EF_Mk_Err       0x203
#define EF_Wrt_Err      0x204
#define EF_FF_PAF       0x205
#define EF_V_wrt_Wrn    0x206
#define EF_Rd_V_Err     0x207
#define EF_Mltp_Err     0x208
#define EF_Wrt_Wrn      0x209
#define ER_Rcv_Err      0x210
#define JT_Buf_AF       0x230
#define JT_Buf_Ovfl     0x231
#define JT_Buf_AMT      0x232
#define JT_Buf_RdErr    0x233
#define JT_Unk_Cmd      0x234
#define JT_Ver_Fail     0x235
#define JT_Prg_Fail     0x236
#define FL_In_AF        0x240
#define FL_In_WtErr     0x241
#define FL_In_RdErr     0x242
#define FL_TRDS_WtErr   0x243
#define FL_TRDS_RdErr   0x244
#define FL_PgRd_WtErr   0x245
#define FL_PgRd_RdErr   0x246
#define FL_ADFF_WtErr   0x247
#define FL_ADFF_RdErr   0x248
#define CF_Mltp_Flsh    0x250
#define CF_Crptd_Dat    0x251
#define CF_Bit_Errs     0x252
#define RH_xxx_xxx      0x260
#define SS_Rld_Pndg     0x270
#define SS_Sys_Up       0x271

// Function Op Code Words

#define        Funct_NoOp 0x00
#define       Set_FF_Test 0x01
#define        Set_FF_VME 0x02
#define        ECC_enable 0x03
#define       ECC_disable 0x04
#define     Save_Cnfg_Num 0x05
#define Read_Cnfg_Num_Dir 0x06
#define Read_Cnfg_Num_Dcd 0x07
#define     Rstr_Cnfg_Num 0x08
#define     Set_Cnfg_Dflt 0x09
#define    Read_Cnfg_Dflt 0x0A
#define          Set_MACs 0x0B
#define     Read_MACs_Dir 0x0C
#define     Read_MACs_Dcd 0x0D
#define          Read_CRs 0x0E
#define        Wrt_Eth_CR 0x0F
#define        Wrt_Ext_CR 0x10
#define        Wrt_Rst_CR 0x11
#define        Wrt_VME_CR 0x12
#define        Wrt_BTO_CR 0x13
#define       Wrt_BGTO_CR 0x14
#define       Wrt_All_CRs 0x15
#define       Set_Clr_CRs 0x16
#define       Set_Inj_Err 0x17
#define       Rst_Inj_Err 0x18
#define     Warn_On_Shdwn 0x19
#define  No_Warn_On_Shdwn 0x1A
#define   Snd_Startup_Pkt 0x1B
#define    No_Startup_Pkt 0x1C
#define       Wrt_Ser_Num 0x1D
#define        Rd_Ser_Num 0x1E
#define         Wrt_CR_ID 0x1F
#define          VME_Cmds 0x20
#define      VME_Dir_Cmds 0x22
#define         Rd_Dev_ID 0x30
#define      Rd_User_Code 0x31
#define      Rd_Cust_Code 0x32
#define      Rd_Back_PROM 0x33
#define        Erase_PROM 0x34
#define      Program_PROM 0x35
#define       Reload_FPGA 0x36
#define       Verify_PROM 0x37
#define     Chk_JTAG_Conn 0x38
#define      Exec_Routine 0x39
#define  Ld_Rtn_Base_Addr 0x3A
#define     Module_Status 0x3B
#define   Write_JTAG_FIFO 0x3C
#define   Write_Prg_Space 0x3D
#define    Read_Prg_Space 0x3E
#define  Abort_JTAG_Cmnds 0x3F
#define         Flash_R_W 0x40
#define        Wrt_Ext_FF 0xE0
#define       Prg_Ext_Off 0xE1
#define      Rdbk_Ext_Off 0xE2
#define       PRst_Ext_FF 0xE3
#define         Rd_Ext_FF 0xE4
#define         RT_Ext_FF 0xE5
#define       MRst_Ext_FF 0xE6
#define      ST_MK_Ext_FF 0xE7
#define     RST_MK_Ext_FF 0xE8
#define   Rst_Ext_Err_Cnt 0xE9
#define   Rd_Ext_Err_Cnts 0xEA
#define       Flush_2_BOD 0xEF
#define        Rst_Seq_ID 0xF0
#define          Gen_Intr 0xF7 //for slave configuration only
#define      Gen_Hard_Rst 0xF8 //for slave configuration only
#define      Force_Reload 0xF9
#define      Send_N_Words 0xFD
#define     Load_User_Reg 0xFE
#define          Loopback 0xFF

// Return Packet Data Type Code Words
// are defined in an enum in pkt_info.h

// jtag instructions

/*
 * PROM instructions
 */
#define  prm_null_wrd   0X0000
#define  prm_ispen      0X00e8
#define  prm_poll       0X00e3
#define  prm_conld      0X00f0
#define  prm_config_    0X00ee
#define  prm_idcode     0X00fe
#define  ISC_PROGRAM    0X00ea
#define  ISC_ADDR_SHIFT 0X00eb
#define  ISC_ERASE      0X00ec
#define  ISC_DATA_SHIFT 0X00ed
#define  XSC_READ_PROT  0X0004
#define  XSC_DATA_UC    0X0006
#define  XSC_DATA_CC    0X0007
#define  XSC_DATA_DONE  0X0009
#define  XSC_DATA_CCB   0X000c
#define  XSC_DATA_SUCR  0X000e
#define  XSC_READ       0X00ef
#define  XSC_DATA_BTC   0X00f2
#define  XSC_WRT_PROT   0X00f7
#define  XSC_UNLOCK     0Xaa55
#define  prm_bypass     0Xffff

/*
 * JTAG program instruction OpCodes
 */
#define JC_ShI_Imd            0x00
#define JC_ShI_Imd_rbk        0x01
#define JC_ShI_Imd_ver_Imd    0x02
#define JC_ShI_Imd_vermsk_Imd 0x03
#define JC_ShI_Imd_stpupdt    0x0E
#define JC_ShDZ               0x10
#define JC_ShDZrbk            0x11
#define JC_ShDZver_Imd        0x12
#define JC_ShDZverff          0x13
#define JC_ShD_Imd            0x14
#define JC_ShD_Imd_rbk        0x15
#define JC_ShD_Imd_ver_Imd    0x16
#define JC_ShD_Imd_vermsk_Imd 0x17
#define JC_ShDff              0x18
#define JC_ShDffrbk           0x19
#define JC_ShDffverff         0x1B
#define JC_ShDffvermskff      0x1C
#define JC_ShDZstpupdt        0x1D
#define JC_ShD_Imd_stpupdt    0x1E
#define JC_ShDffstpupdt       0x1F
#define JC_SetPollInt         0x20
#define JC_Wait2Poll          0x21
#define JC_TstatLp_Imd        0x22
#define JC_TcntrLp_Imd        0x23
#define JC_TDone              0x24
#define JC_LdLpCntr_Imd       0x25
#define JC_Cont_Data          0x30
#define JC_RstIdl_TAP         0x3D
#define JC_Rst_TAP            0x3E
#define JC_End_Prg            0x3F

/*
 * Default Polling intervals
 */
#define Poll_Dflt  0x3D09
#define Poll_Shrt  0x0075
#define Poll_Long  0x9896

/*
 * JTAG Routines in Program Memory
 */

enum JRTN_ID {JC_Rd_DevID=0, JC_Rd_UC, JC_Rd_CC, JC_Rdbk_PROM, JC_Erase,
              JC_Program, JC_Reload, JC_Verify, JC_Chk_Conn, JC_User, 
              JC_Custom_Rtn=15};

typedef struct jinstr_t {  /* JTAG Instructions */
  unsigned char instr;
  unsigned short bc;
  unsigned short data;
} JINSTR_t;

typedef struct jrtn_t {  /* JTAG Routines */
  int jid;
  short n;
  unsigned short base;
  JINSTR_t *rtn;
} JRTN_t;

typedef struct verify_t {  /* JTAG Verify data */
  unsigned short int *buf;
  int nw;
} VERIFY_t;

//
// Erase Proceedure
//
//
//                   Instuction       Bit Count   Data
//                 ----------------------------------------
JINSTR_t erase[] = {
                   {JC_SetPollInt,      0x0000, Poll_Long},
                   {JC_RstIdl_TAP,      0x0000, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x00d0},
                   {JC_ShI_Imd,         0x0010, XSC_UNLOCK},
                   {JC_ShD_Imd,         0x0018, 0x003f},
                   {JC_Cont_Data,       0x0000, 0x0000},
                   {JC_ShI_Imd_stpupdt, 0x0010, ISC_ERASE},
                   {JC_ShD_Imd,         0x0018, 0x003f},
                   {JC_Cont_Data,       0x0000, 0x0000},
                   {JC_ShI_Imd,         0x0010, prm_poll},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZ,            0x0008, 0x0000},
                   {JC_TstatLp_Imd,     0x0000, 0x0002},
                   {JC_ShI_Imd,         0x0010, prm_conld},
                   {JC_ShI_Imd,         0x0010, prm_bypass},
                   {JC_ShDZ,            0x0001, 0x0000},
                   {JC_End_Prg,         0x0000, prm_null_wrd}};

#define N_erase (sizeof erase / sizeof(JINSTR_t))

//
// Program Proceedure
//
//                   Instuction       Bit Count   Data
//                 ----------------------------------------
JINSTR_t program[] = {
                   {JC_SetPollInt,      0x0000, Poll_Shrt},
                   {JC_RstIdl_TAP,      0x0000, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x00d0},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x0003},
                   {JC_ShI_Imd,         0x0010, XSC_DATA_BTC},
                   {JC_ShD_Imd,         0x0020, 0xffe0},
                   {JC_Cont_Data,       0x0000, 0xffff},
                   {JC_ShI_Imd,         0x0010, ISC_PROGRAM},
                   {JC_ShI_Imd,         0x0010, prm_poll},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZ,            0x0008, 0x0000},
                   {JC_TstatLp_Imd,     0x0000, 0x0002},
                   {JC_RstIdl_TAP,      0x0000, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x00d0},
                   {JC_ShI_Imd,         0x0010, ISC_DATA_SHIFT},
                   {JC_ShDff,           0x0100, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, ISC_ADDR_SHIFT},
                   {JC_ShD_Imd,         0x0018, 0x0000},
                   {JC_Cont_Data,       0x0000, 0x0000},
                   {JC_ShI_Imd,         0x0010, ISC_PROGRAM},
                   {JC_ShI_Imd,         0x0010, prm_poll},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZ,            0x0008, 0x0000},
                   {JC_TstatLp_Imd,     0x0000, 0x0002},
                   {JC_LdLpCntr_Imd,    0x0000, 0x7FFF},
                   {JC_ShI_Imd,         0x0010, ISC_DATA_SHIFT},
                   {JC_ShDff,           0x0100, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, ISC_PROGRAM},
                   {JC_ShI_Imd,         0x0010, prm_poll},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZ,            0x0008, 0x0000},
                   {JC_TstatLp_Imd,     0x0000, 0x0002},
                   {JC_TcntrLp_Imd,     0x0000, 0x0007},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x0003},
                   {JC_ShI_Imd,         0x0010, XSC_DATA_SUCR},
                   {JC_ShD_Imd,         0x0010, 0xfffc},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, ISC_PROGRAM},
                   {JC_ShI_Imd,         0x0010, prm_poll},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZ,            0x0008, 0x0000},
                   {JC_TstatLp_Imd,     0x0000, 0x0002},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x0003},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x0003},
                   {JC_ShI_Imd,         0x0010, XSC_DATA_UC},
                   {JC_ShDff,           0x0020, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, ISC_PROGRAM},
                   {JC_ShI_Imd,         0x0010, prm_poll},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZ,            0x0008, 0x0000},
                   {JC_TstatLp_Imd,     0x0000, 0x0002},
                   {JC_ShI_Imd,         0x0010, XSC_DATA_UC},
                   {JC_ShDZverff,       0x0020, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x0003},
                   {JC_ShI_Imd,         0x0010, ISC_ADDR_SHIFT},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShD_Imd,         0x0018, 0x0000},
                   {JC_Cont_Data,       0x0000, 0x0088},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, XSC_DATA_CC},
                   {JC_ShDff,           0x0100, prm_null_wrd},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, ISC_PROGRAM},
                   {JC_ShI_Imd,         0x0010, prm_poll},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZ,            0x0008, 0x0000},
                   {JC_TstatLp_Imd,     0x0000, 0x0002},
                   {JC_ShI_Imd,         0x0010, prm_conld},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x0003},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x0003},
                   {JC_ShI_Imd,         0x0010, XSC_DATA_CCB},
                   {JC_ShD_Imd,         0x0010, 0xFFF9},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, ISC_PROGRAM},
                   {JC_ShI_Imd,         0x0010, prm_poll},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZ,            0x0008, 0x0000},
                   {JC_TstatLp_Imd,     0x0000, 0x0002},
                   {JC_ShI_Imd,         0x0010, prm_conld},
                   {JC_ShI_Imd,         0x0010, prm_poll},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZ,            0x0008, 0x0000},
                   {JC_TstatLp_Imd,     0x0000, 0x0002},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x0003},
                   {JC_ShI_Imd,         0x0010, XSC_DATA_DONE},
                   {JC_ShD_Imd,         0x0008, 0x00ce},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, ISC_PROGRAM},
                   {JC_ShI_Imd,         0x0010, prm_poll},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZ,            0x0008, 0x0000},
                   {JC_TstatLp_Imd,     0x0000, 0x0002},
                   {JC_ShI_Imd,         0x0010, prm_bypass},
                   {JC_TDone,           0x0000, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, prm_bypass},
                   {JC_ShDZ,            0x0001, 0x0000},
                   {JC_End_Prg,         0x0000, prm_null_wrd}};

#define N_program (sizeof program / sizeof(JINSTR_t))

//
// Verify Proceedure
//
//                   Instuction       Bit Count   Data
//                 ----------------------------------------
JINSTR_t verify[] = {
                   {JC_SetPollInt,      0x0000, Poll_Shrt},
                   {JC_RstIdl_TAP,      0x0000, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x0003},
                   {JC_LdLpCntr_Imd,    0x0000, 0x0400},
                   {JC_ShI_Imd,         0x0010, ISC_ADDR_SHIFT},
                   {JC_ShDff,           0x0018, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, XSC_READ},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZverff,       0x2000, prm_null_wrd},
                   {JC_TcntrLp_Imd,     0x0000, 0x0005},
                   {JC_ShI_Imd,         0x0010, prm_poll},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZver_Imd,     0x0008, 0x0036},
                   {JC_ShI_Imd,         0x0010, prm_conld},
                   {JC_ShI_Imd,         0x0010, prm_poll},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZ,            0x0008, 0x0000},
                   {JC_TstatLp_Imd,     0x0000, 0x0002},
                   {JC_ShI_Imd,         0x0010, prm_bypass},
                   {JC_ShDZ,            0x0001, 0x0000},
                   {JC_End_Prg,         0x0000, prm_null_wrd}};

#define N_verify (sizeof verify / sizeof(JINSTR_t))

//
// Reload Proceedure
//
//                   Instuction       Bit Count   Data
//                 ----------------------------------------
JINSTR_t reload[] = {
                   {JC_RstIdl_TAP,      0x0000, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x0003},
                   {JC_ShI_Imd,         0x0010, prm_conld},
                   {JC_ShI_Imd,         0x0010, prm_poll},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZ,            0x0008, 0x0000},
                   {JC_TstatLp_Imd,     0x0000, 0x0002},
                   {JC_ShI_Imd,         0x0010, prm_config_},
                   {JC_ShI_Imd,         0x0010, prm_conld},
                   {JC_ShI_Imd,         0x0010, prm_poll},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZ,            0x0008, 0x0000},
                   {JC_TstatLp_Imd,     0x0000, 0x0002},
                   {JC_ShI_Imd,         0x0010, prm_bypass},
                   {JC_ShDZ,            0x0001, 0x0000},
                   {JC_End_Prg,         0x0000, prm_null_wrd}};

#define N_reload (sizeof reload / sizeof(JINSTR_t))

//
// Read Device ID Proceedure
//
//                   Instuction       Bit Count   Data
//                 ----------------------------------------

JINSTR_t rd_devid[] = {
                   {JC_RstIdl_TAP,      0x0000, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x0003},
                   {JC_ShI_Imd,         0x0010, prm_idcode},
                   {JC_ShDZrbk,         0x0020, 0x0000},
                   {JC_ShI_Imd,         0x0010, prm_bypass},
                   {JC_ShDZ,            0x0001, 0x0000},
                   {JC_End_Prg,         0x0000, prm_null_wrd}};
/*
JINSTR_t rd_devid[] = {
                   {0x0010,      0x0020, 0x0030},
                   {0x0011,      0x0021, 0x0031},
                   {0x0012,      0x0022, 0x0032},
                   {0x0013,      0x0023, 0x0033},
                   {0x0014,      0x0024, 0x0034},
                   {0x0015,      0x0025, 0x0035},
                   {0x0016,      0x0026, 0x0036},
                   {0x0017,      0x0027, 0x0037},
                   {0x0018,      0x0028, 0x0038},
                   {0x0019,      0x0029, 0x0039},
                   {0x001A,      0x002A, 0x003A},
                   {0x001B,      0x002B, 0x003B},
                   {0x001C,      0x002C, 0x003C},
                   {JC_End_Prg,  0x002D, 0x003D},
                   {0x001E,      0x002E, 0x003E},
                   {0x001F,      0x002F, 0x003F},
                   {JC_End_Prg,JC_End_Prg,JC_End_Prg},
                   {JC_End_Prg,JC_End_Prg,JC_End_Prg},
                   {JC_End_Prg,JC_End_Prg,JC_End_Prg}};
*/
#define N_rd_devid (sizeof rd_devid / sizeof(JINSTR_t))

//
// Read User Code Proceedure
//
//                   Instuction       Bit Count   Data
//                 ----------------------------------------

JINSTR_t rd_uc[] = {
                   {JC_RstIdl_TAP,      0x0000, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x0003},
                   {JC_ShI_Imd,         0x0010, XSC_DATA_UC},
                   {JC_ShDZrbk,         0x0020, 0x0000},
                   {JC_ShI_Imd,         0x0010, prm_bypass},
                   {JC_ShDZ,            0x0001, 0x0000},
                   {JC_End_Prg,         0x0000, prm_null_wrd}};
/*
JINSTR_t rd_uc[] = {
                   {0x0040,      0x0050, 0x0060},
                   {0x0041,      0x0051, 0x0061},
                   {0x0042,      0x0052, 0x0062},
                   {0x0043,      0x0053, 0x0063},
                   {0x0044,      0x0054, 0x0064},
                   {0x0045,      0x0055, 0x0065},
                   {0x0046,      0x0056, 0x0066},
                   {0x0047,      0x0057, 0x0067},
                   {0x0048,      0x0058, 0x0068},
                   {JC_End_Prg,JC_End_Prg,JC_End_Prg},
                   {JC_End_Prg,JC_End_Prg,JC_End_Prg},
                   {JC_End_Prg,JC_End_Prg,JC_End_Prg}};
*/
#define N_rd_uc (sizeof rd_uc / sizeof(JINSTR_t))

//
// Read Customer Code Proceedure
//
//                   Instuction       Bit Count   Data
//                 ----------------------------------------
JINSTR_t rd_cc[] = {
                   {JC_SetPollInt,      0x0000, Poll_Shrt},
                   {JC_RstIdl_TAP,      0x0000, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x0003},
                   {JC_ShI_Imd,         0x0010, ISC_ADDR_SHIFT},
                   {JC_ShD_Imd,         0x0018, 0x0000},
                   {JC_Cont_Data,       0x0000, 0x0088},
                   {JC_ShI_Imd,         0x0010, XSC_DATA_CC},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZrbk,         0x0100, 0x0000},
                   {JC_ShI_Imd,         0x0010, prm_bypass},
                   {JC_ShDZ,            0x0001, 0x0000},
                   {JC_End_Prg,         0x0000, prm_null_wrd}};

#define N_rd_cc (sizeof rd_cc / sizeof(JINSTR_t))

//
// Readback PROM Proceedure
//
//                   Instuction       Bit Count   Data
//                 ----------------------------------------
JINSTR_t rd_prom[] = {
                   {JC_SetPollInt,      0x0000, Poll_Shrt},
                   {JC_RstIdl_TAP,      0x0000, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x0003},
                   {JC_LdLpCntr_Imd,    0x0000, 0x0400},
                   {JC_ShI_Imd,         0x0010, ISC_ADDR_SHIFT},
                   {JC_ShDff,           0x0018, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, XSC_READ},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZrbk,         0x2000, prm_null_wrd},
                   {JC_TcntrLp_Imd,     0x0000, 0x0005},
                   {JC_ShI_Imd,         0x0010, prm_poll},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZver_Imd,     0x0008, 0x0036},
                   {JC_ShI_Imd,         0x0010, prm_conld},
                   {JC_ShI_Imd,         0x0010, prm_poll},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZ,            0x0008, 0x0000},
                   {JC_TstatLp_Imd,     0x0000, 0x0002},
                   {JC_ShI_Imd,         0x0010, prm_bypass},
                   {JC_ShDZ,            0x0001, 0x0000},
                   {JC_End_Prg,         0x0000, prm_null_wrd}};

#define N_rd_prom (sizeof rd_prom / sizeof(JINSTR_t))

//
// Check JTAG Connection Proceedure
//
//                   Instuction       Bit Count   Data
//                 ----------------------------------------
JINSTR_t chk_conn[] = {
                   {JC_RstIdl_TAP,      0x0000, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, prm_bypass},
                   {JC_ShI_Imd_vermsk_Imd, 0x0020, 0xFACE},
                   {JC_Cont_Data,       0x0000, prm_null_wrd},
                   {JC_Cont_Data,       0x0000, prm_null_wrd},
                   {JC_Cont_Data,       0x0000, prm_bypass},
                   {JC_Cont_Data,       0x0000, 0xFACE},
                   {JC_Cont_Data,       0x0000, 0xFFFF},
                   {JC_End_Prg,         0x0000, prm_null_wrd}};

#define N_chk_conn (sizeof chk_conn / sizeof(JINSTR_t))

//
// User Proceedure
//
//                   Instuction       Bit Count   Data
//                 ----------------------------------------
JINSTR_t user[] = {
                   {JC_SetPollInt,      0x0000, Poll_Shrt},
                   {JC_RstIdl_TAP,      0x0000, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x00d0},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x0003},
                   {JC_ShI_Imd,         0x0010, XSC_DATA_BTC},
                   {JC_ShD_Imd,         0x0020, 0xffe0},
                   {JC_Cont_Data,       0x0000, 0xffff},
                   {JC_ShI_Imd,         0x0010, ISC_PROGRAM},
                   {JC_ShI_Imd,         0x0010, prm_poll},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZ,            0x0008, 0x0000},
                   {JC_TstatLp_Imd,     0x0000, 0x0002},
                   {JC_RstIdl_TAP,      0x0000, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x00d0},
                   {JC_ShI_Imd,         0x0010, ISC_DATA_SHIFT},
                   {JC_ShDff,           0x0100, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, ISC_ADDR_SHIFT},
                   {JC_ShD_Imd,         0x0018, 0x0000},
                   {JC_Cont_Data,       0x0000, 0x0000},
                   {JC_ShI_Imd,         0x0010, ISC_PROGRAM},
                   {JC_ShI_Imd,         0x0010, prm_poll},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZ,            0x0008, 0x0000},
                   {JC_TstatLp_Imd,     0x0000, 0x0002},
                   {JC_LdLpCntr_Imd,    0x0000, 0x0001},
                   {JC_ShI_Imd,         0x0010, ISC_DATA_SHIFT},
                   {JC_ShDff,           0x0100, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, ISC_PROGRAM},
                   {JC_ShI_Imd,         0x0010, prm_poll},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZ,            0x0008, 0x0000},
                   {JC_TstatLp_Imd,     0x0000, 0x0002},
                   {JC_TcntrLp_Imd,     0x0000, 0x0007},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x0003},
                   {JC_ShI_Imd,         0x0010, XSC_DATA_SUCR},
                   {JC_ShD_Imd,         0x0010, 0xfffc},
                   {JC_ShI_Imd,         0x0010, ISC_PROGRAM},
                   {JC_ShI_Imd,         0x0010, prm_poll},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZ,            0x0008, 0x0000},
                   {JC_TstatLp_Imd,     0x0000, 0x0002},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x0003},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x0003},
                   {JC_ShI_Imd,         0x0010, XSC_DATA_UC},
                   {JC_ShDff,           0x0020, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, ISC_PROGRAM},
                   {JC_ShI_Imd,         0x0010, prm_poll},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZ,            0x0008, 0x0000},
                   {JC_TstatLp_Imd,     0x0000, 0x0002},
                   {JC_ShI_Imd,         0x0010, XSC_DATA_UC},
                   {JC_ShDZverff,       0x0020, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x0003},
                   {JC_ShI_Imd,         0x0010, ISC_ADDR_SHIFT},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShD_Imd,         0x0018, 0x0000},
                   {JC_Cont_Data,       0x0000, 0x0088},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, XSC_DATA_CC},
                   {JC_ShDff,           0x0100, prm_null_wrd},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, ISC_PROGRAM},
                   {JC_ShI_Imd,         0x0010, prm_poll},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZ,            0x0008, 0x0000},
                   {JC_TstatLp_Imd,     0x0000, 0x0002},
                   {JC_ShI_Imd,         0x0010, prm_conld},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x0003},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x0003},
                   {JC_ShI_Imd,         0x0010, XSC_DATA_CCB},
                   {JC_ShD_Imd,         0x0010, 0xFFF9},
                   {JC_ShI_Imd,         0x0010, ISC_PROGRAM},
                   {JC_ShI_Imd,         0x0010, prm_poll},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZ,            0x0008, 0x0000},
                   {JC_TstatLp_Imd,     0x0000, 0x0002},
                   {JC_ShI_Imd,         0x0010, prm_conld},
                   {JC_ShI_Imd,         0x0010, prm_ispen},
                   {JC_ShD_Imd,         0x0008, 0x0003},
                   {JC_ShI_Imd,         0x0010, XSC_DATA_DONE},
                   {JC_ShD_Imd,         0x0008, 0x00ce},
                   {JC_ShI_Imd,         0x0010, ISC_PROGRAM},
                   {JC_ShI_Imd,         0x0010, prm_poll},
                   {JC_Wait2Poll,       0x0000, prm_null_wrd},
                   {JC_ShDZ,            0x0008, 0x0000},
                   {JC_TstatLp_Imd,     0x0000, 0x0002},
                   {JC_ShI_Imd,         0x0010, prm_bypass},
                   {JC_TDone,           0x0000, prm_null_wrd},
                   {JC_ShI_Imd,         0x0010, prm_bypass},
                   {JC_ShDZ,            0x0001, 0x0000},
                   {JC_End_Prg,         0x0000, prm_null_wrd}};

#define N_user (sizeof user / sizeof(JINSTR_t))


JRTN_t jtr[] = {
               {JC_Rd_DevID,  N_rd_devid, 0x00D0, rd_devid},
               {JC_Rd_UC,     N_rd_uc,    0x00E0, rd_uc},
               {JC_Rd_CC,     N_rd_cc,    0x00F0, rd_cc},
               {JC_Rdbk_PROM, N_rd_prom,  0x0100, rd_prom},
               {JC_Erase,     N_erase,    0x0000, erase},
               {JC_Program,   N_program,  0x0020, program},
               {JC_Reload,    N_reload,   0x00B0, reload},
               {JC_Verify,    N_verify,   0x0090, verify},
               {JC_Chk_Conn,  N_chk_conn, 0x0120, chk_conn},
               {JC_User,      N_user,     0x0180, user}};

unsigned short int prm_dat[0x80000];

 #define MAX_MSG_SIZE 1024 

 JINSTR_t rbk_prg[512];

//
// from cnfg_defs.h
//
typedef struct mac_t {  /* MAC addresses */
        unsigned char device[6];
        unsigned char mcast1[6];
        unsigned char mcast2[6];
        unsigned char mcast3[6];
        unsigned char dflt_srv[6];
} MAC_t;

typedef struct cnfg_t {  /* Configuration elements */
        MAC_t mac;
        unsigned short int ether;
        unsigned short int ext_fifo;
        unsigned short int reset;
        unsigned int vme;
        unsigned short int vme_bto;
        unsigned short int vme_bgto;
} CNFG_t;

typedef CNFG_t *CNFG_ptr;

typedef struct sn_t {  /* Serial Number type structure */
        int status;
        int err_typ;
        int sn;
} SN_t;

#define CNFG_MAXLINE 256

/*
 * Predefine configuration registers
 */
/*
 * MAC Addresses
 */
#define  DEV_MAC      {0x02,0x00,0x00,0x00,0x00,0x00}
#define  BCAST_MAC    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}
#define  MCAST1_MAC   {0xFF,0xFF,0xFF,0xFF,0xFF,0xFE}
#define  MCAST2_MAC   {0xFF,0xFF,0xFF,0xFF,0xFF,0xFD}
#define  MCAST3_MAC   {0xFF,0xFF,0xFF,0xFF,0xFF,0xFC}
#define  DFLT_SRV_MAC {0x00,0x0D,0x88,0xB5,0x86,0xAC}
#define  DFLT_FRM_MAC {0xAA,0xBB,0xCC,0xDD,0xEE,0xFF}
/*
 * Ethernet Configuration Register
 */
#define ETH_CR_DFLT   0x0050   //Spontaneous Packets and Protocol Enabled
#define ETH_CR_SPONT  0x0040   //Spontaneous Packet sending enable bit
/*
 * External FIFO Configuration Register
 */
#define FIFO_CR_DFLT    0x0002  //ECC enabled
#define FIFO_CR_TST     0x0001  //Test mode
#define FIFO_CR_ECC     0x0002  //ECC enabled
#define FIFO_CR_INJ     0x0004  //Inject errors
/*
 * Reset Enables/Misc. Configuration Register
 */
#define RST_CR_DFLT     0x0313  //JTAG, Front Panel, Internal
#define RST_CR_INT      0x0001  //Internal
#define RST_CR_FP       0x0002  //Front Panel
#define RST_CR_SRST     0x0004  //VME SYSRESET
#define RST_CR_HR       0x0008  //CMS Hard Reset
#define RST_CR_JTAG     0x0010  //JTAG through PROM CF- pin
#define RST_CR_WARN     0x0020  //Warning packet on shutdown
#define RST_CR_STRTUP   0x0040  //Send packet on startup
#define RST_CR_MSGLVL   0x0300  //Message Level 0 -- no messages
                                //              1 -- Errors only
                                //              2 -- Errors and Warnings
                                //              3 -- Errors, Warnings and Info
/*
 * VME Configuration Register
 */
#define VME_CR_DFLT     0xEDFF1D0F  //Default settings
#define VME_CR_MSTR     0x00000001  //Enable Master
#define VME_CR_SYSCLK   0x00000002  //Enable SYSCLK
#define VME_CR_BTO      0x00000004  //Enable BUS TimeOut
#define VME_CR_ARB      0x00000008  //Enable Arbiter
#define VME_CR_SLV      0x00000010  //Enable Slave (on controller for testing)
#define VME_CR_LOC      0x00000020  //Enable Location Monitor (not impl. yet)
#define VME_CR_IH       0x00000040  //Enable Interrupt Handler
#define VME_CR_USR      0x00000080  //Enable User Defined I/O
#define VME_CR_HIGH     0x00004000  //Enable High order Address and Data
#define VME_CR_FRC_SRST 0x00008000  //Force a VME SYSRESET
#define VME_CR_UPD_IRQ  0x00800000  //Update IRQ Mask (reset to CR value)
#define VME_CR_SRST_OUT 0x40000000  //Enable SYSRESET output to backplane
#define VME_CR_SRST_IN  0x80000000  //Enable SYSRESET input from backplane
#define SET_MSTR_REQ_TYP(t,v)  ((((t)&3)<<8)|((v)&0xFFFFFCFF))
#define SET_MSTR_REQ_LVL(l,v)  ((((l)&3)<<10)|((v)&0xFFFFF3FF))
#define SET_ARB_TYP(t,v)       ((((t)&3)<<12)|((v)&0xFFFFCFFF))
#define SET_IH_LEVELS(l,v)     ((((l)&0x7F)<<16)|((v)&0xFF80FFFF))
#define SET_IH_REQ_TYP(t,v)    ((((t)&3)<<24)|((v)&0xFCFFFFFF))
#define SET_IH_REQ_LVL(l,v)    ((((l)&3)<<26)|((v)&0xF3FFFFFF))
#define SET_IH_DAT_SZ(s,v)     ((((s)&3)<<28)|((v)&0xCFFFFFFF))
/*
 * VME Bus Time Out Register
 */
#define VME_BTO_DFLT  0x30D4  //Default Bus Time Out (200us)
#define VME_BTO_SHRT  0x00D4  //Short Bus Time Out (3.392us)
#define VME_BTO_MAX   0xFFFF  //Maximum Bus Time Out (1.04856ms)
/*
 * VME Bus Grant Time Out Register
 */
#define VME_BGTO_DFLT  0x0C35  //Default Bus Grant Time Out (50.0us)
#define VME_BGTO_MAX   0xFFFF  //Maximum Bus Grant Time Out (1.04856ms)

/*
 * VME Arbiter types
 */
#define ARB_SGL  1 //Single Level Arbiter
#define ARB_PRI  2 //Prioritized Arbiter
#define ARB_RRS  3 //Round Robin Select Arbiter

/*
 * VME Bus Requester types
 */
#define REQ_ROR       0 //Release on Request Requester
#define REQ_RWD       1 //Release When Done  Requester
#define REQ_FAIR_ROR  2 //Fair Release on Request Requester
#define REQ_FAIR_RWD  3 //Fair Release When Done  Requester
