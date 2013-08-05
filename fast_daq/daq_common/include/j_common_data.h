
#ifndef __J_COMMON_DATA_H__
#define __J_COMMON_DATA_H__

extern "C"
{
#include "daq_conf.h"
#include "csc_event.h"
}

typedef struct {
/* common block diverse */
double ymin[6], ymax[6];

/* common block unpacked */
float strips[NSTRIP][NLAYER];
float strips_peaks[NSTRIP][NLAYER]; // firman
int wires[NWIRE][NLAYER], scint[2][NSCINT][2];

/* common block trigunpacked */
bool wiregroup[NWIRE][NLAYER], halfstrip[2][NSTRIP][NLAYER], alct[2][NWIRE][NLAYER];
bool clct[2][NSTRIP][NLAYER], tmba[NWIRE][NLAYER], tmbc[2][NSTRIP][NLAYER];
int alct_q[2];

/*common block raw */
int sample[5][NLAYER][MAX_SCA_SAMPLE], peak_strip[NLAYER], nsamples;

/*common block wiregeom*/


/*common block atrig_wiregeom */
float chx[11][300], chy[11][300], ymid[11];
float x0, dy, dx;
int nch[11];
}j_common_type;

#ifdef CSC_DISPLAY
extern j_common_type j_data;
#else
j_common_type j_data;
#endif

extern "C" void unpack_data_cc();

#endif
