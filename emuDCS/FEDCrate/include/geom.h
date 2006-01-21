#ifndef geom_h
#define geom_h

#include "geom_def.h"

struct GEOM geo[] = {
  {"NONE",-1,0,0,0,0,{0,0,0,0,0},0,0xff},
  {"OUTFIFO",1,PROM_BYPASS,8,1,0,{0,0,0,0,0},0,0xff},
  {"VMEPROM",2,PROM_BYPASS,8,1,0,{0,0,0,0,0},0,0xff},
  {"DDUPROM0",3,PROM_BYPASS,8,1,2,{3,-4 ,0,0,0},1,0xff},
  {"DDUPROM1",3,PROM_BYPASS,8,1,2,{-3,4,0,0,0},0,0xff},
  {"INPROM0",4,PROM_BYPASS,8,1,2,{5,-6,0,0,0},1,0xff},
  {"INPROM1",4,PROM_BYPASS,8,1,2,{-5,6,0,0,0},0,0xff},
  {"DDUFPGA",5,PROM_BYPASS,10,1,0,{0,0,0,0,0},0,0xff},
  {"INFPGA0",6,VTX2_BYPASS,14,1,0,{0,0,0,0,0},0,0xff},
  {"INFPGA1",7,VTX2_BYPASS,14,1,0,{0,0,0,0,0},0,0xff},
  {"SLINK",  8,PROM_BYPASS,8,1,0,{0,0,0,0,0},0,0xff},
  {"VMEPARA",9,PROM_BYPASS,8,1,0,{0,0,0,0,0},0,0xff},
  {"VMESERI",10,PROM_BYPASS,8,1,0,{0,0,0,0,0},0,0xff},
  {"RESET",12,PROM_BYPASS,8,1,0,{0,0,0,0,0},0,0xff},
  {"SADC", 13,PROM_BYPASS,8,1,0,{0,0,0,0,0},0,0xff},
  {"MPROM",2,PROM_BYPASS,16,1,0,{0,0,0,0,0},0,0xff},
  {"INPROM",3,PROM_BYPASS,8,1,0,{0,0,0,0,0},0,0xff},
  {"INCTRL1",4,PROM_BYPASS,8,1,5,{-18,19,20,21,22},0,0xff},
  {"INCTRL2",4,PROM_BYPASS,8,1,5,{18,-19,20,21,22},1,0xff},
  {"INCTRL3",4,PROM_BYPASS,8,1,5,{18,19,-20,21,22},2,0xff},
  {"INCTRL4",4,PROM_BYPASS,8,1,5,{18,19,20,-21,22},3,0xff},
  {"INCTRL5",4,PROM_BYPASS,8,1,5,{18,19,20,21,-22},4,0xff},
  {"MCTRL",11,PROM_BYPASS,10,1,0,{0,0,0,0,0},0,0xff},
  {"RESET1",12,PROM_BYPASS,8,1,2,{-24,25,0,0,0},0,0xff},
   {"RESET2",12,PROM_BYPASS,8,1,2,{24,-25,0,0,0},1,0xff}
};
#define NUMDEV (sizeof (geo) / sizeof (struct GEOM))

#endif
