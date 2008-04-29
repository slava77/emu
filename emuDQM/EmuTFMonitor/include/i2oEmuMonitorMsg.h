#ifndef _i2oEmuTFMonitorMsg_h_
#define _i2oEmuTFMonitorMsg_h_

#include "i2o/i2o.h"

// callback binding (user defined)
#define I2O_EMUTFMONITOR_CODE 0x0001

typedef struct _I2O_EMUTFMONITOR_CREDIT_MESSAGE_FRAME {
  I2O_PRIVATE_MESSAGE_FRAME PvtMessageFrame;
  unsigned long nEventCredits;
  unsigned long prescalingFactor;
} I2O_EMUTFMONITOR_CREDIT_MESSAGE_FRAME, *PI2O_EMUTFMONITOR_CREDIT_MESSAGE_FRAME;

typedef struct _I2O_EMUTF_DATA_MESSAGE_FRAME {
  I2O_PRIVATE_MESSAGE_FRAME PvtMessageFrame;
  unsigned long  runNumber;
  unsigned long  nEventCreditsHeld;
  unsigned short errorFlag; // for DQM; filled by EmuRUI's server only
} I2O_EMUTF_DATA_MESSAGE_FRAME, *PI2O_EMUTF_DATA_MESSAGE_FRAME;

#endif
