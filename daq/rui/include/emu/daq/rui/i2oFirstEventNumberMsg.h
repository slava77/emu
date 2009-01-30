#ifndef _i2oEmuFirstEventNumber_h_
#define _i2oEmuFirstEventNumber_h_

#include "i2o/i2o.h"

// callback binding (user defined)
#define I2O_EMU_FIRST_EVENT_NUMBER_CODE 0x0004

typedef struct _I2O_EMU_FIRST_EVENT_NUMBER_MESSAGE_FRAME {
  I2O_PRIVATE_MESSAGE_FRAME PvtMessageFrame;
  unsigned long firstEventNumber;
} I2O_EMU_FIRST_EVENT_NUMBER_MESSAGE_FRAME, *PI2O_EMU_FIRST_EVENT_NUMBER_MESSAGE_FRAME;

#endif
