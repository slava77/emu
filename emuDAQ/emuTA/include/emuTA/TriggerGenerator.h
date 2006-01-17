#ifndef __emuTA_TriggerGenerator_h__
#define __emuTA_TriggerGenerator_h__

#include "emu/emuDAQ/emuTA/include/emuTA/exception/Exception.h"
#include "extern/i2o/include/i2o/i2oDdmLib.h"
#include "i2o/include/i2o/Method.h"
#include "interface/evb/include/i2oEVBMsgs.h"
#include "interface/shared/include/i2oXFunctionCodes.h"
#include "toolbox/include/toolbox/mem/MemoryPoolFactory.h"
#include "toolbox/include/toolbox/mem/Pool.h"
#include "xcept/include/xcept/Exception.h"

#include "emu/emuDAQ/emuTA/include/SliceTestTriggerChunk.h"


using namespace std;


namespace emuTA
{

/**
 * Generates dummy triggers.
 */
class TriggerGenerator
{
public:

    /**
     * Generates and returns a single dummy I2O_EVM_TRIGGER message.
     */
    toolbox::mem::Reference *generate
    (
        toolbox::mem::MemoryPoolFactory *poolFactory,
        toolbox::mem::Pool              *pool,
        const I2O_TID                   initiatorAddress,
        const I2O_TID                   targetAddress,
        const unsigned long             triggerSourceId,
        const U32                       eventNumber,
        const U32                       runNumber
    )
    throw (emuTA::exception::Exception);
};

} // End of emuTA namespace

#endif
