// $Id: i2oEmuClientMsg.h,v 3.0 2006/01/17 10:37:54 geurts Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _i2oEmuClientMsg_h_
#define _i2oEmuClientMsg_h_

#include "i2o.h"

// callback binding (user defined)
#define I2O_EMUCLIENT_CODE 0x0001

typedef struct _I2O_EMUCLIENT_CREDIT_MESSAGE_FRAME {
  I2O_PRIVATE_MESSAGE_FRAME PvtMessageFrame;
  unsigned long nEventCredits;
  unsigned long prescalingFactor;
} I2O_EMUCLIENT_CREDIT_MESSAGE_FRAME, *PI2O_EMUCLIENT_CREDIT_MESSAGE_FRAME;

typedef struct _I2O_EMU_DATA_MESSAGE_FRAME {
  I2O_PRIVATE_MESSAGE_FRAME PvtMessageFrame;
  unsigned long runNumber;
  unsigned long nEventCreditsHeld;
} I2O_EMU_DATA_MESSAGE_FRAME, *PI2O_EMU_DATA_MESSAGE_FRAME;

#endif
