/*****************************************************************************\
* $Id: Facts.cc,v 1.2 2010/08/13 03:00:07 paste Exp $
\*****************************************************************************/

#include "emu/fed/Facts.h"

const char* const emu::fed::FEDFSMFact::typeName_ = "FEDFSMFact";
const char* const emu::fed::FEDFSMFact::parameterNames_[] = {"from", "to", "configType", "configValue", "result"};

const char* const emu::fed::DDUFMMIRQFact::typeName_ = "DDUFMMIRQFact";
const char* const emu::fed::DDUFMMIRQFact::parameterNames_[] = {"crateNumber", "slotNumber", "rui", "hardError", "syncError", "resetRequested", "fiberStatus", "advancedFiberStatus", "xorStatus", "ddufpgaDebugTrap", "infpga0DebugTrap", "infpga1DebugTrap"};

const char* const emu::fed::DDUFMMResetFact::typeName_ = "DDUFMMResetFact";
const char* const emu::fed::DDUFMMResetFact::parameterNames_[] = {"crateNumber"};

