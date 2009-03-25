//-----------------------------------------------------------------------
// $Id: geom.h,v 1.1 2009/03/25 10:07:43 liu Exp $
// $Log: geom.h,v $
// Revision 1.1  2009/03/25 10:07:43  liu
// move header files to include/emu/pc
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef geom_h
#define geom_h

#include "JTAG_constants.h"

struct GEOM {
    char *nam;
    int jchan;
    char kbypass;
    int kbit;
    int kbybit;
    int nser;
    int seri[2];
    int sxtrbits;
    unsigned int femask;
};


static GEOM geo[] = {
{"NONE",-1,0,0,0,0,{0,0},0},
{"F1PROM",1,PROM_BYPASS,8,1,2,{-1,7},0, 0x01},
{"F2PROM",1,PROM_BYPASS,8,1,2,{-2,8},0, 0x02},
{"F3PROM",1,PROM_BYPASS,8,1,2,{-3,9},0, 0x04},
{"F4PROM",1,PROM_BYPASS,8,1,2,{-4,10},0, 0x08},
{"F5PROM",1,PROM_BYPASS,8,1,2,{-5,11},0, 0x10},
{"FAPROM",1,PROM_BYPASS,8,1,2,{-6,12},0, 0x1F},
{"F1SCAM",1,VTX_BYPASS,5,1,2,{1,-7}, 1, 0x01},
{"F2SCAM",1,VTX_BYPASS,5,1,2,{2,-8}, 1, 0x02},
{"F3SCAM",1,VTX_BYPASS,5,1,2,{3,-9}, 1, 0x04},
{"F4SCAM",1,VTX_BYPASS,5,1,2,{4,-10},1, 0x08},
{"F5SCAM",1,VTX_BYPASS,5,1,2,{5,-11},1, 0x10},
{"FASCAM",1,VTX_BYPASS,5,1,2,{6,-12},1, 0x1F},
{"MCTRL",2,VTX_BYPASS, 5,1,0,{0,0},0, 0xff},
{"MPROM",3,PROM_BYPASS,8,1,0,{0,0},0, 0xff},
{"VPROM",4,PROM_BYPASS,8,1,0,{0,0},0, 0xff},
{"CDAC", 5,0,0,0,0,{0,0},0, 0xff},
{"MADC", 6,0,0,0,0,{0,0},0, 0xff},
{"FIFO1",7,0,0,0,0,{0,0},0, 0x01},
{"FIFO2",7,0,0,0,0,{0,0},0, 0x02},
{"FIFO3",7,0,0,0,0,{0,0},0, 0x04},
{"FIFO4",7,0,0,0,0,{0,0},0, 0x08},
{"FIFO5",7,0,0,0,0,{0,0},0, 0x10},
{"FIFO6",7,0,0,0,0,{0,0},0, 0x20},
{"FIFO7",7,0,0,0,0,{0,0},0, 0x40},
{"LOWVOLT", 8,0,0,0,0,{0,0},0, 0xff},
{"STATUS",  9,0,0,0,0,{0,0},0, 0xff},
{"BUCSHF", 10,0,0,0,0,{0,0},0, 0xff},
{"RESET",11,PROM_BYPASS,8,1,0,{0,0},0, 0x99},
{"DPROM",4,PROM_BYPASS,8,1,2,{-29,30},0, 0xff},
{"DCTRL",4,VTX_BYPASS,5,1,2,{29,-30},1, 0xff},
{"TPROM",4,PROM_BYPASS,8,1,2,{-31,32},0, 0xff},
{"TCTRL",4,VTX_BYPASS,3,1,2,{31,-32},1, 0xff},
{"EPROM",2,PROM_BYPASS,8,1,0,{0,0},0, 0xff},
{"ECTRL",3,VTX_BYPASS,5,1,0,{0,0},0, 0xff},
{"DRESET",11,PROM_BYPASS,8,1,0,{0,0},0, 0x99}
};
#define NUMDEV (sizeof (geo) / sizeof (struct GEOM))

#endif

