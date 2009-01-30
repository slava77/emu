#ifndef __emu_daq_ta_TriggerGenerator_h__
#define __emu_daq_ta_TriggerGenerator_h__

#include "emu/daq/ta/exception/Exception.h"
#include "i2o/i2oDdmLib.h"
#include "i2o/Method.h"
#include "interface/evb/i2oEVBMsgs.h"
#include "interface/shared/i2oXFunctionCodes.h"
#include "toolbox/mem/MemoryPoolFactory.h"
#include "toolbox/mem/Pool.h"
#include "xcept/Exception.h"

//#include "SliceTestTriggerChunk.h"


using namespace std;


namespace emu { namespace daq { namespace ta {

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
    throw (emu::daq::ta::exception::Exception);
};

}}} // namespace emu::daq::ta

#endif
