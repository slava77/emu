#ifndef TMB_JTAG_constants_h
#define TMB_JTAG_constants_h

// ** JTAG chains (2004 TMB):
#define ALCT_SLOW_USER_CHAIN  0x0     // ALCT Slow Control user
#define ALCT_SLOW_PROM_CHAIN  0x1     // ALCT Slow Control PROM
#define ALCT_MEZZ_USER_CHAIN  0x2     // ALCT Mezzanine user
#define ALCT_MEZZ_PROM_CHAIN  0x3     // ALCT Mezzanine PROM
#define TMB_MEZZ_FPGA_CHAIN   0x4     // TMB Mezzanine FPGA + FPGA PROMs 
#define TMB_USER_PROM_CHAIN   0x8     // TMB User PROMs 
#define TMB_FPGA_CHAIN        0xc     // TMB FPGA Monitor (for TMB self-test)
#define RAT_CHAIN             0xd     // RAT FPGA+PROM

#define MAX_FRAMES           1000     //Maximum number of frames in an i/o cycle
#define MAX_NUM_CHIPS           5     //Maximum number of devices in chain

// **bring the TMS to the appropriate state (from RTI) using the following sequences:
const char     tms_pre_opcode[4] = { 1, 1, 0, 0 };     //go from RTI to ShfIR  
const char     tdi_pre_opcode[4] = { 0, 0, 0, 0 };  

const char     tms_post_opcode[2] = { 1, 0 };          //Ex1IR to RTI
const char     tdi_post_opcode[2] = { 0, 0 };       

const char     tms_pre_read[3] = { 1, 0, 0 };          //go from RTI to ShfDR  
const char     tdi_pre_read[3] = { 0, 0, 0 };  

const char     tms_post_read[2] = { 1, 0 };            //Ex1DR to RTI
const char     tdi_post_read[2] = { 0, 0 };       

// **some lengths of registers:
const int tmb_mezzID_length = 32;  
const int tmb_user_promID_length = 32;  

const int rat_user1_length = 224;
const int rat_user2_length = 32;

#endif
