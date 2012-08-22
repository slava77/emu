#include "emu/dqm/common/CSCDqmFact.h"

const char* const EmuMonitorFact::typeName_ = "EmuMonitorFact";
const char* const EmuMonitorFact::parameterNames_[] = { "state", "stateChangeTime", "dqmEvents", "dqmRate", "cscRate", "cscDetected", "cscUnpacked" };

const char* const DqmReportFact::typeName_ = "DqmReportFact";
const char* const DqmReportFact::parameterNames_[] = { "testId" };
