#ifndef _i2oEmuMonitorMsg_h_
#define _i2oEmuMonitorMsg_h_

#include "i2o.h"

// callback binding (user defined)
#define I2O_EMUMONITOR_CODE 0x0001

typedef struct _I2O_EMUMONITOR_CREDIT_MESSAGE_FRAME {
  I2O_PRIVATE_MESSAGE_FRAME PvtMessageFrame;
  uint32_t nEventCredits;
  uint32_t prescalingFactor;
} I2O_EMUMONITOR_CREDIT_MESSAGE_FRAME, *PI2O_EMUMONITOR_CREDIT_MESSAGE_FRAME;

typedef struct _I2O_EMU_DATA_MESSAGE_FRAME {
  I2O_PRIVATE_MESSAGE_FRAME PvtMessageFrame;
  uint32_t  runNumber;
  uint32_t  nEventCreditsHeld;
  uint32_t errorFlag; // for DQM; filled by EmuRUI's server only
} I2O_EMU_DATA_MESSAGE_FRAME, *PI2O_EMU_DATA_MESSAGE_FRAME;

#endif
