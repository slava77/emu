#ifndef DcsDimStructures_h
#define DcsDimStructures_h

// adapted from Valeri Sytnik's e2p program

// status bit pattern (Aug. 2010):
//   bit 0 (value   1):  misc. errors
//       1 (value   2):  chamber power off from Configuration DB
//       2 (value   4):  data corrupted (in infospace or during transimission)
//       3 (value   8):  VCC not accessible
//       4 (value  16):  DMB Reading error
//       5 (value  32):  crate OFF
//       6 (value  64):  this DMB module caused VCC reading trouble
//       7 (value 128):  TMB Reading error
//       8 (value 256):  this TMB module caused VCC reading trouble                                                                                        

#define CFEB_NUMBER 5

typedef struct{
  float v33[CFEB_NUMBER];
  float v50[CFEB_NUMBER];
  float v60[CFEB_NUMBER];
  float c33[CFEB_NUMBER];
  float c50[CFEB_NUMBER];
  float c60[CFEB_NUMBER];
}CFEB_LV;

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

  CFEB_LV cfeb;
  ALCT_LV alct;
  TMB_LV  tmb;
  
  int update_time;
  int status;

}LV_1_DimBroker;



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
