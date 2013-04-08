#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <math.h>
#include "cfortran.h"
#include "hbook.h"
#include "daq_module.h"
#include "daq_histo.h"         /* PED_HIST_OFFSET, NPED_BINS, ... */
#include "csc_parameters.h"    /* NLAYER, NSTRIP, ...    */
#include "daq_conf.h"
#include "csc_event.h"         /* upevt_                 */
#include "pedestal.h"

int write_peds(float *mean, float *rms, char *out_file)
{
	FILE *out_fp;
	int ilayer, istrip;
	int rtval = 0;
 
	out_fp = fopen (out_file, "w");
  
	if (out_fp == NULL) {
		printf("Failed to open pedestal file %s for writing EE", out_file);
		return -1;
	}

	for (ilayer = 0; ilayer < NLAYER; ilayer++) {
		for (istrip = 0; istrip < NSTRIP; istrip++) {
			rtval = fprintf(out_fp, "%5.2f %5.2f ", 
							mean[istrip + NSTRIP*ilayer],
							rms [istrip + NSTRIP*ilayer]   ) ;
			if (rtval == -1) return rtval;
			if (( (istrip+1) % 10) == 0 ) {
				rtval = fprintf(out_fp, "\n");
				if (rtval == -1) return rtval;
			}
		}
	}
    
	fclose(out_fp);

	return 0;
}
  
int read_peds(float *mean, float *rms, char *in_file)
{
  FILE *in_fp;
  int ilayer, istrip;
  int rtval = 0;
 
  in_fp = fopen (in_file, "r");
  
  if (in_fp == NULL) {
    #ifdef OFFLINE_ONLY
    printf(
    #else
    syslog(LOG_LOCAL1|LOG_ERR,
    #endif
      "Failed to open pedestal file %s for reading EE", in_file);
    return -1;
  }

  for (ilayer = 0; ilayer < NLAYER; ilayer++) {
    for (istrip = 0; istrip < NSTRIP; istrip++) {

      if (( (istrip+1) % 10) == 0 ) {
        rtval = fscanf(in_fp, "%f %f \n", 
                  mean + istrip + NSTRIP*ilayer,
                  rms  + istrip + NSTRIP*ilayer   ) ;
      }
      else {
        rtval = fscanf(in_fp, "%f %f ",
                  mean + istrip + NSTRIP*ilayer,
                  rms  + istrip + NSTRIP*ilayer   ) ;
      }

      if (rtval == -1) return rtval;

    }
  }
    
  fclose(in_fp);


  return 0;

}

int get_default_peds(float *mean, float *rms)
{
  char ped_file[80];
  char *path;

  path = getenv("DATADIR");
  if (path == NULL) {
    sprintf(ped_file, "DEFAULT.ped");
  }
  else {
    sprintf(ped_file, "%s/pedestals/DEFAULT.ped", path);
  }

  #ifdef DEBUG
  printf("Read pedestal file %s\n", ped_file);
  #endif

  if (read_peds(mean, rms, ped_file) != 0) {
    #ifdef OFFLINE_ONLY
    printf(
    #else
    syslog(LOG_LOCAL1|LOG_ERR,
      "get_default_peds(): Failed to read pedestal file %s EE", ped_file);
    #endif
    return -1;
  }

  return 0;
}


int subtract_monitor_peds(float *mean, float *rms)
{
  int iddu, ilayer, istrip, ifeb, ifeb_chan, itime;

  for (ifeb = 0; ifeb < NCFEB; ifeb++) {
    if (upevt_.active_febs[ifeb]) {           /* Is this FEB present? */
      for (itime = 0; itime < upevt_.nsca_sample; itime++) {
        for (iddu = 0; iddu < NCFEB_CHAN; iddu++) {

          ifeb_chan = 16*(iddu % 6) + iddu/6;

          /* Find layer and strip from CFEB cable map */
          ilayer = ((map_.CFEB[ifeb][ifeb_chan] & 0xff00)>>8) - 1;
          istrip =  (map_.CFEB[ifeb][ifeb_chan] & 0x00ff)     - 1;

          upevt_.sca[ilayer][istrip][itime] += -mean[istrip + NSTRIP*ilayer];
        }
      }
    }
  }
  return 0;

}


int calc_monitor_peds(float *mean, float *rms)
{
	int   hid;
	int   istrip, ilayer;
	int   ibin, ilower, iupper;
	int   imax;
	float histo_conten[NPED_BINS];
	float xbin;
	float xwidth = (PED_UPPER - PED_LOWER)/(2*NPED_BINS);
	float wt_sum = 0.0, mean_sum = 0.0, rms_sum = 0.0;

	for(ilayer=0; ilayer < NLAYER; ilayer++) {
		for(istrip=0; istrip < NSTRIP; istrip++) {
			hid = PED_HIST_OFFSET + 100*(ilayer + 1) + (istrip + 1);

			HUNPAK(hid, histo_conten, "HIST", 0);

			/* Find the bin with most entries */
			imax = 0;
			for (ibin = 0; ibin < NPED_BINS; ibin++) {
				if (histo_conten[ibin] > histo_conten[imax]) imax = ibin;  
			}

			/* If imax is not too close to one of the array boundries, then
			 * take 15 bins above and below imax. We will use this range of
			 * bins to calculate the mean and standard deviation.
			 */
			ilower = ((imax - 15) < 0) ? 0 : imax - 15;
			iupper = ((imax + 15) >= NPED_BINS) ? NPED_BINS : imax + 15;

			/* Calculated the weighted mean */
			mean_sum = 0;
			wt_sum   = 0;
/*    for (ibin = ilower; ibin < iupper; ibin++) {*/
			for (ibin = 0; ibin < NPED_BINS; ibin++) {
				/* Center of the ith bin */
				xbin = PED_LOWER + (PED_UPPER - PED_LOWER)*((float)ibin/NPED_BINS) +
					xwidth;
				mean_sum += xbin*histo_conten[ibin];
				wt_sum   += histo_conten[ibin];
			}
  

			/* Don't want to divide by zero */
			if (wt_sum != 0.0) {

				mean[istrip + NSTRIP*ilayer] = mean_sum/wt_sum;

				/* Calculate standard deviation */
				rms_sum = 0;
/*      for (ibin = ilower; ibin < iupper; ibin++) {*/
				for (ibin = 0; ibin < NPED_BINS; ibin++) {
					/* Center of the ith bin */
					xbin = PED_LOWER + (PED_UPPER - PED_LOWER)*((float)ibin/NPED_BINS) +
						xwidth;

					rms_sum += (xbin - mean[istrip + NSTRIP*ilayer])*
						(xbin - mean[istrip + NSTRIP*ilayer])*
						histo_conten[ibin];
				}
  
				rms_sum = rms_sum/wt_sum;
				rms[istrip + NSTRIP*ilayer] = sqrt(rms_sum);

			}
			else {
				mean[istrip + NSTRIP*ilayer] = 0.0;
				rms [istrip + NSTRIP*ilayer] = 0.0;
			}

		}  /* end for istrip */
	}  /* end for ilayer */

	return 0;
}
