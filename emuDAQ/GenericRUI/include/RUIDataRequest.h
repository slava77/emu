//-----------------------------------------------------------------------
// $Id: RUIDataRequest.h,v 2.0 2005/04/13 10:53:01 geurts Exp $
// $Log: RUIDataRequest.h,v $
// Revision 2.0  2005/04/13 10:53:01  geurts
// Makefile
//
//
//-----------------------------------------------------------------------
// Description: Trivial little message to get the current event from RU
#ifndef RUIDataRequest_h
#define RUIDataRequest_h

#include "i2o.h"
// need to pick a unique number 0-255
const int RUIDataRequestMsgCode = 137;

struct RUIDataRequest
{
   I2O_PRIVATE_MESSAGE_FRAME PvtMessageFrame; ///< I2O information
};

#endif

