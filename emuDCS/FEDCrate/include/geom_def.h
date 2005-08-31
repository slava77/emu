#ifndef geom_def_h
#define geom_def_h

#include "JTAG_constants.h"

struct GEOM {
    char *nam;
    int jchan;
    char kbypass;
    int kbit;
    int kbybit;
    int nser;
    int seri[5];
    int sxtrbits;
    unsigned int femask;
};

#endif

