//-----------------------------------------------------------------------
// $Id: RUIDataRequestListener.h,v 2.0 2005/04/13 10:53:01 geurts Exp $
// $Log: RUIDataRequestListener.h,v $
// Revision 2.0  2005/04/13 10:53:01  geurts
// Makefile
//
//
//-----------------------------------------------------------------------
#ifndef RUIDataRequestListener_h
#define RUIDataRequestListener_h

#include "i2oListener.h"
#include "RUIDataRequest.h"

class RUIDataRequestListener : public virtual i2oListener 
{
public:
  virtual void handleDataRequest(BufRef * ref) = 0;
  RUIDataRequestListener() {
    i2oBindMethod(this, &RUIDataRequestListener::handleDataRequest,
                   RUIDataRequestMsgCode, XDAQ_ORGANIZATION_ID, 0);
  }
};

#endif

