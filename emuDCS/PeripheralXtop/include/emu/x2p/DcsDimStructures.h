#ifndef DcsDimStructures_h
#define DcsDimStructures_h

// status bit pattern (Jan. 2011):  (1=problem, 0=OK/ignore)
//   bit 0 (value   1):  misc. errors
//       1 (value   2):  chamber power off from Configuration DB
//       2 (value   4):  data corrupted (in infospace or during transimission)
//       3 (value   8):  VCC not accessible
//       4 (value  16):  DMB Reading error
//       5 (value  32):  crate OFF
//       6 (value  64):  this DMB module caused VCC reading trouble
//       7 (value 128):  TMB Reading error
//       8 (value 256):  this TMB module caused VCC reading trouble                                                                                        
//       9 (value 512):  chamber lost Analog power
//      10 (val  1024):  chamber lost Digital power
//
// CCB bit pattern (Mar. 2011):  (1=problem, 0=OK/ignore)
//   bit 0 (value   1):  ALCT
//       1 (value   2):  TMB
//       2 (value   4):  DMB & CFEB (for chamber type ME1/3, always set to 0)
//       7 (value 128):  CCB tag readback wrong, probably not configured
//       8 (value 256):  CCB status wrong
//       9 (value 512):  MPC Main board and old link problem
//      10 (val  1024):  MPC GTP and new link problem
//
// XMAS info bit pattern (April 2012):
//     ==0:    no action or ignored
//     !=0:
//         lowest byte (bit 0-7)  is state indicator
//               1:     Unknown
//               2:     ON
//               4:     OFF
//         second byte (bit 8-15) is error
//               1: (total value 256):  fatal error
//

#define CFEB_NUMBER 5
#define DCFEB_NUMBER 7
#define DCFEB_MONS 30

typedef struct{
  float v33[CFEB_NUMBER];
  float v50[CFEB_NUMBER];
  float v60[CFEB_NUMBER];
  float c33[CFEB_NUMBER];
  float c50[CFEB_NUMBER];
  float c60[CFEB_NUMBER];
}CFEB_LV;

typedef struct{
  float v30[DCFEB_NUMBER];
  float v40[DCFEB_NUMBER];
  float v55[DCFEB_NUMBER];
  float c30[DCFEB_NUMBER];
  float c40[DCFEB_NUMBER];
  float c55[DCFEB_NUMBER];
}DCFEB_LV;

typedef struct{
  float vcore[DCFEB_NUMBER];
  float vaux1[DCFEB_NUMBER];
}DCFEB_SYSMON;

typedef struct{
  int status[DCFEB_NUMBER];
  int error1[DCFEB_NUMBER];
  int errorm[DCFEB_NUMBER];
  int other1[DCFEB_NUMBER];
}DCFEB_SEU;

typedef struct{
  float FPGA3V;
  float FPGA2V;
  float FPGA1V;
  float LVMB5V;
  float PPIB5V;
  float PPIB3V;
  float PPIBCU;
  int ODMB1;
  int ODMB2;
  int ODMB3;
}ODMB_LV;

typedef struct{
  float v18;
  float v33;
  float v55;
  float v56;
  float c18;
  float c33;
  float c55;
  float c56;
}ALCT_LV;

typedef struct{
  float v50;
  float v33;
  float v15C;
  float v15T;
  float v10T;
  float c50;
  float c33;
  float c15C;
  float c15T;
  float cRAT;
  float vRAT;
  float vREF;
  float vGND;
  float vMAX;
}TMB_LV;
                
typedef struct{

  CFEB_LV cfeb;    // 6 x (F:5)
  ALCT_LV alct;    // (F:8)
  TMB_LV  tmb;     // (F:14)

  float A7v;
  float D7v;       // (F:2)

  int CCB_bits;
  int FPGA_bits;

  int update_time;
  int status;      // (I:4)

}LV_1_DimBroker;
                
typedef struct{

  DCFEB_LV dcfeb;    // 6 x (F:7)
  ALCT_LV alct;      // (F:8)
  TMB_LV  tmb;       // (F:14)
  DCFEB_SYSMON dsys; // 2 x (F:7)

  float A7v;
  float D7v;         // (F:2)

  ODMB_LV odmb;      // (F:7; I:3)
  DCFEB_SEU seu;     // 4 x (I:7)

  int CCB_bits;
  int FPGA_bits;

  int update_time;
  int status;        // (I:4)

}LV_2_DimBroker;
                
typedef struct{

  int VCC_info;
  int CCB_info;
  int MPC_info;
  int Crate_info;
  int update_time;

}CRATE_1_DimBroker;

                
typedef struct{

  int XMAS_info;
  int X2P_info;
  int update_time;

}XMAS_1_DimBroker;


typedef struct{

  float t_daq;
  float t_alct;

  float t_cfeb1;
  float t_cfeb2;
  float t_cfeb3;
  float t_cfeb4;
  float t_cfeb5;

  int update_time;
  int status;

}TEMP_1_DimBroker;

typedef struct{

  float t_odmb;
  float t_otmb;
  float t_alct;
  float t_lvdb;

  float t_fpga[DCFEB_NUMBER];
  float t_pcb1[DCFEB_NUMBER];
  float t_pcb2[DCFEB_NUMBER];

  int update_time;
  int status;

}TEMP_2_DimBroker;


typedef struct{

  float v15;
  float v25a;
  float v25b;
  float v33;

  float td1;
  float td2;
  float td3;
  float td4;

  int update_time;
  int status;
  
}DDU_1_DimBroker;

#endif
