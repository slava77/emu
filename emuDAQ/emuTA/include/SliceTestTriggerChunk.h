#ifndef SliceTestTriggerChunk_h
#define SliceTestTriggerChunk_h

// copied from hcal/consumer/include/SliceTestTriggerChunk.hh
struct SliceTestTriggerChunk {
  unsigned int h1a; // CDF
  unsigned int h1b; // HEADER
  unsigned int h2a; // WORDS 
  unsigned int h2b; //
  unsigned int triggerWord;
  unsigned int triggerNumber;
  unsigned int triggerTime_usec;
  unsigned int triggerTime_base;
  unsigned int spillNumber;
  unsigned int runNumber;
  char runNumberSequenceId[16];
  unsigned int orbitNumber;
  unsigned int bunchNumber;
  unsigned int eventStatus; 
  unsigned int filler1;
  unsigned int t1a;   // CDF TRAILER
  unsigned int t2a;   // WORD
};

static const unsigned int SliceTestTriggerChunk_SourceId = 1;
static const unsigned int SliceTestTriggerChunk_endOfRun = 0x0002;

#endif
