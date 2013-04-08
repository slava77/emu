/* daq_histo.h
 *
 *
 * Author : David Chrisman
 * Date   : 13 Apr 1999
 *
*/
#ifndef __DAQ_HISTO_H__
#define __DAQ_HISTO_H__

#define HISTO_NWORDS    500000
#define HISTO_SIZE      HISTO_NWORDS*sizeof(float)
#define HISTO_SHMEM_KEY "HBK"

typedef struct{
        char h[HISTO_SIZE];
}PAWC;

PAWC pawc_;           /* A global variable */

/*
 * Useful parameters for booking and filling histograms. We have to make
 * sure that each histogram is booked with a unique ID, that's why
 * each device has its own histogram ID offset.
 */

#define STRIP_LANDAU_ID          800  /* hid = STRIP_LANDAU_ID + ilayer       */

#define STRIP_HIST_OFFSET       1000
#define WIRE_HIST_OFFSET        2000  

#define PED_MEAN                4000  /* hid = PED_MEAN + ilayer              */
#define PED_MEAN_VS_STRIP       4000  /* hid = PED_MEAN_VS_STRIP + 10*ilayer  */
#define PED_RMS_VS_STRIP        4000  /* hid = PED_RMS_VS_STRIP + 100*ilayer  */

#define BEAM_CHAM_HIST_OFFSET   5000

#define PED_HIST_OFFSET         6000  /* hid = PED_ID + 100*ilayer + istrip   */
#define NPED_BINS               400   /* Num of bins in the pedestal histos   */
#define PED_LOWER               400.5 /* Lower edge of first channel          */
#define PED_UPPER               800.5 /* Upper edge of last channel             */

#define CLCT_HIST_OFFSET        7000
#define ALCT_HIST_OFFSET        8000
#define TMB_HIST_OFFSET         9000  

#define WIRE_EFF_HIST_OFFSET   10000

#define TRIG_SCINT_HIST_OFFSET 20000

#define RPC_HIST_OFFSET        30000

#endif  /* included already */
