#ifndef _emu_daq_ta_SliceTestTriggerChunk_h_
#define _emu_daq_ta_SliceTestTriggerChunk_h_

#include <stdint.h>

namespace emu { namespace daq { namespace ta {

// copied from hcal/consumer/include/SliceTestTriggerChunk.hh
struct SliceTestTriggerChunk {
  int32_t  h1a; // CDF
  int32_t  h1b; // HEADER
  int32_t  h2a; // WORDS 
  int32_t  h2b; //
  int32_t  triggerWord;
  int32_t  triggerNumber;
  int32_t  triggerTime_usec;
  int32_t  triggerTime_base;
  int32_t  spillNumber;
  int32_t  runNumber;
  char runNumberSequenceId[16];
  int32_t  orbitNumber;
  int32_t  bunchNumber;
  int32_t  eventStatus; 
  int32_t  filler1;
  int32_t  t1a;   // CDF TRAILER
  int32_t  t2a;   // WORD
};

static const int32_t  SliceTestTriggerChunk_SourceId = 1;
static const int32_t  SliceTestTriggerChunk_endOfRun = 0x0002;

}}} // namespace emu::daq::ta

#endif
