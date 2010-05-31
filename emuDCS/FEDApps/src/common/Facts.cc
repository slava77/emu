/*****************************************************************************\
* $Id: Facts.cc,v 1.1 2010/05/31 14:57:20 paste Exp $
\*****************************************************************************/

#include "emu/fed/Facts.h"

const char* const emu::fed::ConfigurationFact::typeName_ = "ConfigurationFact";
const char* const emu::fed::ConfigurationFact::parameterNames_[] = {"systemName", "configuration", "id"};

const char* const emu::fed::DDUFMMIRQFact::typeName_ = "DDUFMMIRQFact";
const char* const emu::fed::DDUFMMIRQFact::parameterNames_[] = {"crateNumber", "slotNumber", "rui", "hardError", "syncError", "resetRequested", "fiberStatus", "advancedFiberStatus", "xorStatus", "ddufpgaDebugTrap", "infpga0DebugTrap", "infpga1DebugTrap"};

const char* const emu::fed::DDUFMMResetFact::typeName_ = "DDUFMMResetFact";
const char* const emu::fed::DDUFMMResetFact::parameterNames_[] = {"crateNumber"};

