#ifndef DcsDimStructures_h
#define DcsDimStructures_h

// adapted from Valeri Sytnik's e2p program

#define CFEB_NUMBER 5

typedef struct{
  float v33[CFEB_NUMBER];
  float v50[CFEB_NUMBER];
  float v60[CFEB_NUMBER];
  float c33[CFEB_NUMBER];
  float c50[CFEB_NUMBER];
  float c60[CFEB_NUMBER];
  float status[CFEB_NUMBER];
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
  float status;
}ALCT_LV;

typedef struct{

  CFEB_LV cfeb;
  ALCT_LV alct;

  int update_time;
  int slot;
  char VCCMAC[80];

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
  int slot;
  char VCCMAC[80];

}TEMP_1_DimBroker;


#endif
