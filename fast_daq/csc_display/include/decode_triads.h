///////////
// creating a header
// file to house forward declarations
// for functions definded in decode_triads.c (after changing to decode_traids.cc)
///////////

#include <iostream>
#include <stdlib.h>
#include <syslog.h>
#include "daq_conf.h"
#include "csc_event.h"       /* upevt_                 */
#include "csc_parameters.h"  /* NLAYER, NSTRIP, etc... */

/*---------------------------------------------------------*/



void init_array(int triads[NLAYER][NSTRIP/2][NCLCT_BUCKET]);
int  calculate_halfstrip(int triads[NLAYER][NSTRIP/2][NCLCT_BUCKET],
               		 int layer,int distrip, int  bucket); 
void decode_triads_(int clct_halfstrips[NLAYER][NSTRIP*2][NCLCT_BUCKET]);
