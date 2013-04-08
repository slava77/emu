/* pedestal.h
 *
 *
 * Author : David Chrisman
 * Date   : 13 June 1999
 *
*/
#ifndef __DAQ_PEDESTAL_H_
#define __DAQ_PEDESTAL_H_

int  write_peds(float *mean, float *rms, char *out_file);
int  read_peds (float *mean, float *rms, char *in_file );

int  get_default_peds(float *mean, float *rms);
int  subtract_monitor_peds(float *mean, float *rms);

int  calc_monitor_peds(float *mean, float *rms);
void calc_mean_and_rms(float *histo_conten, float *mean, float *rms);

#endif /* if not included already */
