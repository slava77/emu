/*****************************************************************************\
* $Id: Facts.cc,v 1.5 2012/10/24 15:14:43 cvuosalo Exp $
\*****************************************************************************/

#include "emu/fed/Facts.h"

const char* const emu::fed::FEDFSMFact::typeName_ = "FEDFSMFact";
const char* const emu::fed::FEDFSMFact::parameterNames_[] = {"from", "to", "configType", "configValue", "result"};

const char* const emu::fed::DDUFMMIRQFact::typeName_ = "DDUFMMIRQFact";
const char* const emu::fed::DDUFMMIRQFact::parameterNames_[] = {"crateNumber", "slotNumber", "rui", "hardError", "syncError", "resetRequested", "fiberStatus", "advancedFiberStatus", "xorStatus", "ddufpgaDebugTrap", "infpga0DebugTrap", "infpga1DebugTrap"};

const char* const emu::fed::DDUFMMResetFact::typeName_ = "DDUFMMResetFact";
const char* const emu::fed::DDUFMMResetFact::parameterNames_[] = {"crateNumber"};

const char* const emu::fed::DDUFMMErrorFact::typeName_ = "DduFmmErrorFact";
const char* const emu::fed::DDUFMMErrorFact::parameterNames_[] = {
	"hardResetRequested",
	"resyncRequested",
	"combinedStatus",
	"combinedStatusStr",
	"ddufpgaDebugTrap",
	"infpga0DebugTrap",
	"infpga1DebugTrap",
	"fmmErrorThreshold",
	"numChambersInErrorForEndcap",
	"fmmsReleased",
	"chambersInError",
	"fibersInError"
};

const char* const emu::fed::DDUStuckInWarningFact::typeName_ = "DduStuckInWarningFact";
const char* const emu::fed::DDUStuckInWarningFact::parameterNames_[] = {
	"hardResetRequested",
	"chambersInWarning",
	"fibersInWarning"
};

const char* const emu::fed::FedRepeatErrorFact::typeName_ = "FedRepeatErrorFact";
const char* const emu::fed::FedRepeatErrorFact::parameterNames_[] = {
	"chambersInError",
	"numChambersInError"
};
