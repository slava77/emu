// #include "evb/examples/ta/include/ta/TriggerGenerator.h"
#include "emuTA/TriggerGenerator.h"
#include "interface/shared/frl_header.h"
#include "interface/shared/fed_header.h"
#include "interface/shared/fed_trailer.h"
#include "toolbox/mem/MemoryPoolFactory.h"
#include <time.h>


toolbox::mem::Reference *emuTA::TriggerGenerator::generate
(
    toolbox::mem::MemoryPoolFactory *poolFactory,
    toolbox::mem::Pool              *pool,
    const I2O_TID                   initiatorAddress,
    const I2O_TID                   targetAddress,
    const unsigned long             triggerSourceId,
    const U32                       eventNumber,
    const U32                       runNumber
)
throw (emuTA::exception::Exception)
{
    toolbox::mem::Reference            *bufRef     = 0;
    I2O_MESSAGE_FRAME                  *stdMsg     = 0;
    I2O_PRIVATE_MESSAGE_FRAME          *pvtMsg     = 0;
    I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME *block      = 0;
    unsigned long                      bufSize     = 0;
    char                               *payload    = 0;
//     frlh_t                             *frlHeader  = 0;
//     fedh_t                             *fedHeader  = 0;
//     fedt_t                             *fedTrailer = 0;


    bufSize = sizeof(I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME) + // RU builder header
//               sizeof(frlh_t)                             + // FRL header
//               sizeof(fedh_t)                             + // FED header
//               sizeof(fedt_t);                              // FED trailer
      sizeof(SliceTestTriggerChunk);

    //////////////////////////////////////////////////
    // Allocate a buffer for the trigger data frame //
    //////////////////////////////////////////////////

    try
    {
        bufRef = poolFactory->getFrame(pool, bufSize);
        bufRef->setDataSize(bufSize);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuTA::exception::Exception,
                      "Failed to allocate I2O_EVM_TRIGGER message", e);
    }


    /////////////////////
    // fill with zeros //
    /////////////////////

    ::memset(bufRef->getDataLocation(), 0, bufSize);


    //////////////////////////////////////////////////
    // Fill in the fields of the trigger data frame //
    //////////////////////////////////////////////////

    stdMsg = (I2O_MESSAGE_FRAME*)bufRef->getDataLocation();
    pvtMsg = (I2O_PRIVATE_MESSAGE_FRAME*)stdMsg;
    block  = (I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME*)stdMsg;

    stdMsg->MessageSize      = bufSize >> 2;
    stdMsg->InitiatorAddress = initiatorAddress;
    stdMsg->TargetAddress    = targetAddress;
    stdMsg->Function         = I2O_PRIVATE_MESSAGE;
    stdMsg->VersionOffset    = 0;
    stdMsg->MsgFlags         = 0;  // Point-to-point

    pvtMsg->XFunctionCode    = I2O_EVM_TRIGGER;
    pvtMsg->OrganizationID   = XDAQ_ORGANIZATION_ID;

    block->eventNumber             = eventNumber;
    block->nbBlocksInSuperFragment = 1;
    block->blockNb                 = 0;

    payload = ((char*)block) + sizeof(I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME);

//     frlHeader   = (frlh_t*)(payload);
//     fedHeader   = (fedh_t*)(payload + sizeof(frlh_t));
//     fedTrailer  = (fedt_t*)(payload + sizeof(frlh_t) + sizeof(fedh_t));

//     frlHeader->trigno   = eventNumber;
//     frlHeader->segno    = 0;
//     frlHeader->segsize  = FRL_LAST_SEGM | (sizeof(fedh_t) + sizeof(fedt_t));

//     fedHeader->sourceid = triggerSourceId;
//     fedHeader->eventid  = 0x50000000 | eventNumber;

//     fedTrailer->conscheck = 0xDEADFACE;
//     fedTrailer->eventsize = 0xA0000000 |
//                             ((sizeof(fedh_t) + sizeof(fedt_t)) >> 3);

    SliceTestTriggerChunk *sttc = (SliceTestTriggerChunk*) payload;

    sttc->triggerNumber    = eventNumber;
    sttc->runNumber        = runNumber;
    sttc->triggerTime_base = time(NULL);

    // common data format
    sttc->h1a  = (0)<<20; // bunch id
    sttc->h1a |= (0x8)|((SliceTestTriggerChunk_SourceId)<<8); // source

    sttc->h1b  = 0x58000000u;
    sttc->h1b |= (eventNumber&0xFFFFFF);

    sttc->t1a  = 0;

    sttc->t2a  = 0xA0000000u;
    sttc->t2a |= sizeof(SliceTestTriggerChunk)/8;


    return bufRef;
}
