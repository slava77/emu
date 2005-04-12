#ifndef DcsDimStructures_h
#define DcsDimStructures_h

#include <DcsCommon.h>

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

CFEB_LV Cfeb_o;
ALCT_LV Alct_o;

}LV_1_data;

typedef struct{

  LV_1_data data;

  // char dynatemTcpIp[80];
 int update_value;
 int setNumber;
 char dynatemTcpIp[80];

}LV_1_DimBroker;




typedef struct{

  //  CFEB_LV Cfeb_o;
  /*
     char dynatemTcpIp[80];
     int setNumber; 
     int update_value;
  */
  /*
  float v33[CFEB_NUMBER];
  float v50[CFEB_NUMBER];
  float v60[CFEB_NUMBER];
  float c33[CFEB_NUMBER];
  float c50[CFEB_NUMBER];
  float c60[CFEB_NUMBER]; 
  
  float v18a;
  float v33a;
  float v55a;
  float v56a;
  float c18a;
  float c33a;
  float c55a;
  float c56a; 
  */

  /// ALCT_LV Alct_o;
 
   LV_1_data data;

  //  char dynatemTcpIp[80];
   int update_value;
     int setNumber;
   char dynatemTcpIp[80];

}LV_1_TEST;

//=========================


typedef struct{

  float t_daq;
  float t_alct;

  float t_feb1;
  float t_feb2;
  float t_feb3;
  float t_feb4;
  float t_feb5;


 int update_value;
 int setNumber;
 char dynatemTcpIp[80];

}TEMPERATURE_1_DimBroker;

//=========================


typedef struct{

  int operation_result; // 1 -- OK, 0 -- failed 

 int update_value;
 int setNumber;
 char dynatemTcpIp[80];

}COMMAND_1_DimBroker;


//=========================


typedef struct{

  float lv_ref_18_chip1;
  float lv_ref_18_chip2;
  float lv_ref_18_chip3;
  float lv_ref_dmb_gnd;
  float lv_ref_cfeb1;
  float lv_ref_cfeb2;
  float lv_ref_cfeb3;
  float lv_ref_cfeb4;
  float lv_ref_cfeb5;


 int update_value;
 int setNumber;
 char dynatemTcpIp[80];

}REFERENCE_1_DimBroker;

//=========================

typedef struct{

  char ACK[80];

}RunControlStructure;


//=========================


#endif
