#include "DQMDataI2oListener.h"

/** \file */


DQMDataI2oListener::DQMDataI2oListener()
{
    i2oBindMethod
    (
        this,
        &DQMDataI2oListener::dqmData,
        DQMDataI2oMsgCode,
        XDAQ_ORGANIZATION_ID,
        0
    );
}
