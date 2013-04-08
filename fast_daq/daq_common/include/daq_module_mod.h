#ifndef __DAQ_MODULE_H__
#define __DAQ_MODULE_H__

#include "csc_parameters.h"  /* NLAYER, NWIRE, etc.... */

#define MAX_CRATE 8
#define MAX_SLOT  25

/* This list of module types should be the same as the one in daq_module.inc */
#define HEAD_ID       0x48454144  /* "HEAD"  event header                    */
#define SCL2551_ID    0x53434C31  /* "SCL1"  2551 scaler                     */
#define SCL4434_ID    0x53434C31  /* "SCL1"  2551 scaler                     */
#define TDC2228_ID    0x54444331  /* "TDC1"  TDC2228                         */
#define TDC2277_ID    0x54444332  /* "TDC2"  TDC2277                         */
#define TDC3377_ID    0x54444333  /* "TDC3"  TDC3377                         */
#define INREG_ID      0x494E5231  /* "INR1"  input register (SEN, Jorway)    */
#define OUTREG_ID     0x4F545231  /* "OTR1"  output register (Jorway 41)     */
#define IOREG_ID      0x494F5231  /* "IOR1"  input/output reg (CAEN C291)    */
#define ADC2232_ID    0x41444331  /* "ADC1"  ADC2232                         */
#define ADC2249_ID    0x41444332  /* "ADC2"  ADC2249                         */
#define TMB_ID        0x544D4232  /* "TMB2"  trigger mother board (VME 99)   */
#define CCB_ID        0x43434231  /* "CCB1"  clock and control board 1999    */
#define DDU_ID        0x44445531  /* "DDU1"  PCI DDU card for DAQMB ro 99    */
#define PAT4448_ID    0x50415431  /* "PAT1"  4448 pattern register           */
#define DSC7106_ID    0x44534331  /* "DSC1"  discriminator (Phillips 7106)   */
#define DSCC808_ID    0x44534332  /* "DSC2"  discriminator (CAEN C808)       */
#define DAC_ID        0x44414331  /* "DAC1"  CAMAC DAC (not read out)        */
#define PULSER_ID     0x504C5352  /* "PLSR"  CAMAC pulser (not read out)     */
#define ALCTCAL_ID    0x4143414C  /* "ACAL"  ALCT JTAG & calibration control */
#define STRIPCAL_ID   0x4343414C  /* "CCAL"  strip calibration control       */
#define CABLE_MAP_ID  0x434D4346  /* "CMCF"  the cable map                   */
#define MOD_CONF_ID   0x4D434346  /* "MCCF"  the module config file          */ 
#define TRIG_SETUP_ID 0x54534346  /* "TSCF"  the trigger setup file          */ 
#define AFEB_CONF_ID  0x41464346  /* "AFCF"  the AFEB config file            */ 

/* Define some useful macros.
 */

#define SLOT(short)       (int )( short & 0x00FF)
#define CRATE(short)      (int )((short & 0x0F00) >>  8)
#define CRATE_TYPE(short) (int )((short & 0xF000) >> 12)

/* Arrays to map module channels to the chamber channels */

typedef struct {
short int TDC2277[MAX_CRATE][MAX_SLOT][32];
short int TDC3377[MAX_CRATE][MAX_SLOT][32];
short int ADC2249[MAX_CRATE][MAX_SLOT][12];
short int DSC7106[MAX_CRATE][MAX_SLOT][16];
short int SCALER[MAX_CRATE][MAX_SLOT][32];
short int CFEB[NCFEB][NCFEB_CHAN];
short int AFEB[NAFEB][NAFEB_CHAN];
short int AFEB_CHIP[NLAYER][NWIRE];
}chamber_map_type;

// my_ins // chamber_map_type map_;           /* A Global Variable */

/* Function prototypes */

int decode_EVTHEAD (short *buf);
int decode_TMB     (short *buf);
int decode_DDU     (short *buf);
int decode_ALCT    (short *buf, int offset, int version);
int decode_CLCT    (short *buf, int offset, int version);
int decode_CFEB    (short *buf, int offset, int version);
int decode_TDC2277 (short *buf);
int decode_TDC3377 (short *buf);
int decode_ALCTCAL (short *buf);
int decode_STRIPCAL(short *buf);
int decode_SCL2551 (short *buf);
int decode_SCL4434 (short *buf);
int decode_DSC7106 (short *buf);

int extract_config_file(char *file_name, short *buf);

int read_cable_map(int cable_map_file_id);

#endif /* if not included already */
