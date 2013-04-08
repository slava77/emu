/* csc_parameters.h
 * 
 * Author: David Chrisman
 * Date: May 28, 1999
 *
 * Note: csc_parameters.h (C) and csc_parameters.inc (FORTRAN) 
 *       should be identical!
 *
 * Mods.: 13 Oct 99, D.C.: Changed NSCINT from 12 to 64. I don't think
 *        this parameter is being used anywhere.
 *
 */
#ifndef __CSC_PARAMETERS_H__
#define __CSC_PARAMETERS_H__

#define NLAYER          6     /* Number of layers per chamber                */
#define NSTRIP         80     /* Number of strips per layer                  */
#define NWIRE          112    /* Number of wire groups per layer             */
#define NWIRES_MAX     112    /* MAX Number of wire groups per layer         */
#define MAX_WIRE_HITS 6144    /* Max number of TDC hits allow per event      */

#define NMOD_HEAD       4     /* Number of generic module header words       */

#define NLCT_HEAD       8     /* Number of LCT99, 00, 01  header words       */
#define NTMB2_HEAD     21     /* Number of TMB2 header words                 */
#define NTMB2_TRAIL     4     /* Number of TMB2 trailer words                */
#define NDDU2_HEAD      8     /* Number of DDU2 data header words            */
#define NDDU2_TRAIL     8     /* Number of DDU2 trailer words                */
#define NDDU_HEAD       6     /* Number of DDU data header words             */
#define NDDU_TRAIL      2     /* Number of DDU trailer words                 */

#define NCFEB           5     /* Max Number of cathode front-end boards      */
#define NCFEB_CHAN     96     /* Number of channels per CFEB                 */
#define NCFEB_TRAIL     4     /* Number of CFEB trailer words, DMB01         */

#define NAFEB           4     /* Number of anode front-end boards            */
#define NAFEB_CHIP      6     /* Number of chips per AFEB                    */
#define NAFEB_CHAN     96     /* Number of channels per AFEB                 */

#define MAX_SCA_SAMPLE 16     /* Max number of SCA time samples              */
#define NALCT_BUCKET   31     /* Max number of ALCT time buckets             */
#define NCLCT_BUCKET   16     /* Max number of CLCT time buckets             */

#define NRPC_SECTOR     6     /* Number sectors per RPC                      */
#define NRPC_STRIP     32     /* Number of channels per sector               */
#define NRPC_HIT       16     /* Number of hits allowed per channel          */

#define NBEAM_CHAM_LAYER  6   /* Number of Beam Chamber Layers               */

#define NSCINT         48     /* Number of trigger scintillators             */
#define NSCINT_LAYERS   4     /* Number of scintillator layers               */
#define NSCINT_PER_LAYER 12   /* Number of scint (phototubes) per layer      */
#define MAX_SCINT_HITS 100    /* Max Number of trigger scintillators hits    */

#define NSCALER_CHAN   12     /* Number of Lecroy 2551 scaler channels       */

/*
 * FEB data size in 16-bit words:
 * FEB_SIZE = *   (num. FEB chan)*(num. layers) + 1 checksum + 1 trailer
 */
#define FEB_SIZE        (NCFEB_CHAN + 1 + 1)

#define ALCT_NO_DUMP_SIZE    10
#define CLCT_NO_DUMP_SIZE    10
#define ALCT_FULL_DUMP_SIZE 730 /* 10 header words + 
				   12 words * 4 LCTs * 15 time bins */
#define CLCT_FULL_DUMP_SIZE 220    

#endif /* if not included already */
