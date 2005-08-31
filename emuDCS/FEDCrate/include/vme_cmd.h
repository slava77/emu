int ife;

unsigned long vmeadd; /* vme base address */
unsigned long vmeadd_tmp;
unsigned long add_i,add_d,add_dh,add_ds,add_dt,add_sw,add_sr,add_rst,add_r;
unsigned long add_reset;
unsigned long add_control_r;
unsigned long add_vmepara;
unsigned long add_vmeser;
unsigned long add_dcc_r,add_dcc_w;
unsigned long add_adcr,add_adcw,add_adcrbb,add_adcws,add_adcrs;

unsigned long msk00={0x00000000};
unsigned long msk01={0x00001000};
unsigned long msk02={0x00002000};
unsigned long msk03={0x00003000};
unsigned long msk04={0x00004000};
unsigned long msk05={0x00005000};
unsigned long msk06={0x00006000};
unsigned long msk07={0x00007000};
unsigned long msk08={0x00008000};
unsigned long msk09={0x00009000};
unsigned long msk0d={0x0000d000};
unsigned long msk7f={0x0007fffe};
unsigned long msk0f={0x0000fffe};

unsigned long msk_clr={0xfffff0ff};
unsigned long msk_rst={0x00000018};
unsigned long msk_i={0x0000001c};
unsigned long msk_d={0x0000000c};
unsigned long msk_dh={0x00000004};
unsigned long msk_ds={0x00000000};
unsigned long msk_dt={0x00000008};
unsigned long msk_sw={0x00000020};
unsigned long msk_sr={0x00000024};
unsigned long msk_r={0x00000014};
unsigned long msk_control_r={0x00082800};
unsigned long msk_vmeser={0x00040000};
unsigned long msk_vmepara={0x00030000};
unsigned long msk_dcc_r={0x00000000};
unsigned long msk_dcc_w={0x00000000};
unsigned long msk_adcr={0x00000004};
unsigned long msk_adcw={0x00000000};
unsigned long msk_adcrbb={0x0000000c};
unsigned long msk_adcws={0x00000020};
unsigned long msk_adcrs={0x00000024};
