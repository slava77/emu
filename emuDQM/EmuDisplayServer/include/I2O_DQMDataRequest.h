#ifndef __DQMDataI2oListener_h__
#define __DQMDataI2oListener_h__

#include "i2o.h"

#define I2O_DQMDataRequestMsgCode 0x0014;

typedef struct DQMDataRequest_MESSAGE_FRAME
{
   I2O_PRIVATE_MESSAGE_FRAME PvtMessageFrame; ///< I2O information
} I2O_DQMDataRequest_MESSAGE_FRAME, *PI2O_DQMDataRequest_MESSAGE_FRAME;

#endif

