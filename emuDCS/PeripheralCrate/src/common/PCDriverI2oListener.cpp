//-----------------------------------------------------------------------
// $Id: PCDriverI2oListener.cpp,v 1.1 2005/06/06 08:09:25 geurts Exp $
// $Log: PCDriverI2oListener.cpp,v $
// Revision 1.1  2005/06/06 08:09:25  geurts
// *** empty log message ***
//
// Revision 1.2  2004/06/01 21:59:14  tfcvs
// oops ...
//
//-----------------------------------------------------------------------
// Description: PCDriverI2oListener class member functions

#include "PCDriverI2oListener.h"

PCDriverI2oListener::PCDriverI2oListener()
{
    // I2O message bindings
    i2oBindMethod
    (
        this,
        &PCDriverI2oListener::StartPRBSMode,
        PCDriverStartPRBSModeMsgCode,
        XDAQ_ORGANIZATION_ID,
        0
    );
    i2oBindMethod
    (
        this,
        &PCDriverI2oListener::EndPRBSMode,
        PCDriverEndPRBSModeMsgCode,
        XDAQ_ORGANIZATION_ID,
        0
    );
}
