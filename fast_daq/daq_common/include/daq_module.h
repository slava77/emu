#ifndef __DAQ_MODULE_H__
#define __DAQ_MODULE_H__

#include "csc_parameters.h"  /* NLAYER, NWIRE, etc.... */

#define MAX_CRATE 8
#define MAX_SLOT  25

/* This list of module types should be the same as the one in 
                                                     daq_module.inc */
#define HEAD_ID         0x48454144  /* "HEAD"  event header                 */
#define SCL2551_ID      0x53434C31  /* "SCL1"  2551 scaler                  */
#define SCL4434_ID      0x53434C32  /* "SCL2"  4434 scaler                  */
#define TDC2228_ID      0x54444331  /* "TDC1"  TDC2228                      */
#define TDC2277_ID      0x54444332  /* "TDC2"  TDC2277                      */
#define TDC3377_ID      0x54444333  /* "TDC3"  TDC3377                      */
#define INREG_ID        0x494E5231  /* "INR1"  input register (SEN, Jorway) */
#define OUTREG_ID       0x4F545231  /* "OTR1"  output register (Jorway 41)  */
#define IOREG_ID        0x494F5231  /* "IOR1"  input/output reg (CAEN C291) */
#define ADC2232_ID      0x41444331  /* "ADC1"  ADC2232                      */
#define ADC2249_ID      0x41444332  /* "ADC2"  ADC2249                      */
#define TMB_ID          0x544D4232  /* "TMB2"  trigger mother board (VME 99)*/
#define CCB_ID          0x43434231  /* "CCB1"  clock and control board 1999 */
#define CCB2_ID         0x43434232  /* "CCB2"  clock and control board 2001 */
#define D360_ID         0X44333630  /* "D360"  dynatem readout module       */
#define DMB2_ID         0x444d4232  /* "DMB2"  daq mother board 2001        */
#define MPC_ID          0x4D504331  /* "MPC1"  muon port card 2000          */
#define DDU_ID          0x44445531  /* "DDU1"  PCI DDU card for DAQMB ro 99 */
#define DDU2_ID         0x44445532  /* "DDU2"  D360 card for DAQMB ro 01    */
#define PAT4448_ID      0x50415431  /* "PAT1"  4448 pattern register        */
#define DSC7106_ID      0x44534331  /* "DSC1"  discriminator (Phillips 7106)*/
#define DSCC808_ID      0x44534332  /* "DSC2"  discriminator (CAEN C808)    */
#define DAC_ID          0x44414331  /* "DAC1"  CAMAC DAC (not read out)     */
#define D360_ID         0X44333630  /* "D360"  dynatem readout module       */
#define PULSER_ID       0x504C5352  /* "PLSR"  CAMAC pulser (not read out)  */
#define ALCTCAL_ID      0x4143414C  /* "ACAL"  ALCT JTAG & calib control    */
#define ALCT2_ID        0x414C4332  /* "ALC2"  ALCT as a muon source: 2001  */
#define STRIPCAL_ID     0x4343414C  /* "CCAL"  strip calibration control    */
#define CABLE_MAP_ID    0x434D4346  /* "CMCF"  the cable map                */
#define MOD_CONF_ID     0x4D434346  /* "MCCF"  the module config file       */
#define TRIG_SETUP_ID   0x54534346  /* "TSCF"  the trigger setup file       */
#define AFEB_CONF_ID    0x41464346  /* "AFCF"  the AFEB config file         */
#define ANODE_PATT_ID   0x41504154  /* "APAT"  pattern identification       */
#define ANODE_HOT_ID    0x41484346  /* "AHCF"  anode hot mask file          */
#define ANODE_THRES_ID  0x41544346  /* "ATCF"  anode threshold file         */
#define ANODE_DELAY_ID  0x41444346  /* "ADCF"  anode delay file             */
#define CATH_HOT_ID     0x43484346  /* "CHCF"  cathode hot mask file        */
#define CATH_THRES_ID   0x43544346  /* "CTCF"  cathode threshold file       */
#define USER_NAME_ID    0x5553524E  /* "USRN"  user name identification     */
#define DAQ_REV_ID      0x44415152  /* "DAQR"  daq revision identification  */
#define SITE_ID         0x53495445  /* "SITE"  site identification          */

/* Define some useful macros.
 */

#define SLOT(short)       (int )( short & 0x00FF)
#define CRATE(short)      (int )((short & 0x0F00) >>  8)
#define CRATE_TYPE(short) (int )((short & 0xF000) >> 12)

/* Arrays to map module channels to the chamber channels */
/* Must correspond to the "map" common block in 
 *  readout_control/include/cable_map.inc
 */

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

extern chamber_map_type map_;           /* A Global Variable */

/* Function prototypes */

int decode_EVTHEAD (short *buf);
int decode_STRING  (short *buf, int length, char *string);
int decode_DAQREV  (short *buf); 
int decode_TMB     (short *buf);
int decode_DDU     (short *buf);
int decode_DDU2    (short *buf, int first_time);
int decode_ALCT    (short *buf, int offset, int version);
int decode_ALCT3   (short *buf, int offset, int version);
int decode_ALCT4   (short *buf, int offset, int version);
int decode_CLCT    (short *buf, int offset, int version);
int decode_CLCT2   (short *buf, int offset, int version);
int decode_CFEB    (short *buf, int offset, int version);
int decode_CFEB2   (short *buf, int offset, int version);
int decode_TDC2277 (short *buf);
int decode_TDC3377 (short *buf);
int decode_ALCTCAL (short *buf);
int decode_STRIPCAL(short *buf);
int decode_SCL2551 (short *buf);
int decode_SCL4434 (short *buf);
int decode_DSC7106 (short *buf);
int decode_ALCT2   (short *buf);

int extract_config_file(char *file_name, short *buf);

int read_cable_map(int cable_map_file_id);

#endif /* if not included already */






