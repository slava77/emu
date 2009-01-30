#ifndef _emu_daq_server_i2oMsg_h_
#define _emu_daq_server_i2oMsg_h_

#include "i2o/i2o.h"

// callback binding (user defined)
#define I2O_EMUCLIENT_CODE 0x0001

typedef struct _I2O_EMUCLIENT_CREDIT_MESSAGE_FRAME {
  I2O_PRIVATE_MESSAGE_FRAME PvtMessageFrame;
  U32 nEventCredits;
  U32 prescalingFactor;
} I2O_EMUCLIENT_CREDIT_MESSAGE_FRAME, *PI2O_EMUCLIENT_CREDIT_MESSAGE_FRAME;

typedef struct _I2O_EMU_DATA_MESSAGE_FRAME {
  I2O_PRIVATE_MESSAGE_FRAME PvtMessageFrame;
  U32 runNumber;
  U32 runStartUTC;
  U32 nEventCreditsHeld;
  U16 errorFlag; // for DQM; filled by EmuRUI's server only
} I2O_EMU_DATA_MESSAGE_FRAME, *PI2O_EMU_DATA_MESSAGE_FRAME;

#endif
